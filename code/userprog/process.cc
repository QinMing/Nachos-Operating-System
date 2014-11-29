#include "copyright.h"
#include "process.h"
#include "synch.h"


Process::Process(char* newname,bool willJoin){
	mainThread = new Thread(newname);
	name = newname;
	numThread = 1;
	
	willBeJoined = willJoin;
	hasJoined = false;
	joinedOnMe = new Condition("JoinedOnMe");
	lock = new Lock("Lock");
	exitStatus =-65535;

	pipeline = NULL;
	pipeIn = pipeOut = NULL;

}

Process::Process(char* newname,bool willJoin,Thread* t){
	mainThread = t;
	name = newname;
	numThread = 1;

	willBeJoined = willJoin;
	hasJoined = false;
	joinedOnMe = new Condition("JoinedOnMe");
	lock = new Lock("Lock");
	exitStatus = -65535;
	
	pipeline = NULL;
	pipeIn = pipeOut = NULL;
}

Process::~Process(){

	// A thread that will be joined is only destroyed once
	// Join has been called on it
	// ASSERT(hasJoined || !willBeJoined);
  
	//should not delete space in Thread::~Thread() because
	//if there are lots of newly created thread, then this space will
	//not be deleted until another thread wakes up from SWITCH(oldThread, nextThread);
	//So here it's just to free the memory in advance.
	delete mainThread->space;
	
	//delete the pipe list, but pipes are deleted by processes that use them. See below.
	delete pipeline;
	
	//delete un-used pipes
	if (pipeIn!=NULL){
		pipeIn->out = 0;
		if (pipeIn->in == 0){
			delete pipeIn;
		}
	}
	if (pipeOut!=NULL){
		pipeOut->in = 0;
		if (pipeOut->out == 0){
			delete pipeOut;
		}
	}

	//Finish the thread
	if (currentThread == mainThread){

		//the thread will then be deleted in scheduler::Run()
		mainThread->Finish();

	}
	else{

		//minor case, when process is created by mistake
		//come to here because the process is being deleted by kenel but not itself.
		ASSERT(!mainThread->getHasForked());//should not be forked
		delete mainThread;

	}

}

void Process::Finish(){
	ASSERT(mainThread == currentThread);
	
	lock->Acquire();
	
	if (hasJoined){
		
		joinedOnMe->Signal(lock);
		joinedOnMe->Wait(lock);
		
	}
	else if(willBeJoined)
	{ // will be joined but hasn't been -> must wait until after Join has been called and returns
		
		joinedOnMe->Wait(lock);
		joinedOnMe->Signal(lock);
		joinedOnMe->Wait(lock);

	}
	
	lock->Release();
}

int Process::Join() {
	// get lock
	lock->Acquire();
	
	if (hasJoined)		// join is not called more that once on the same process
	{
		lock->Release();
		return -65535;
	}
	if (!willBeJoined)	// join is only invoked on processes created to be joined
	{
		lock->Release();
		return -65535;
	}

	willBeJoined = false; // ensure Join() can only be called once
	hasJoined = true;

	joinedOnMe->Signal(lock);

	/*
	//promote the joinee's priority if it's lower than joiner's
	if (currentThread->getPriority() > this->getPriority() ){
		//if the priority is larger than lock holder's
        currentThread->dependThread = this;
		this -> promotePriority( currentThread->getPriority() );
	}
	*/

	// add currentThread to the queue
	joinedOnMe->Wait(lock);
    
	//currentThread->dependThread = NULL;

	joinedOnMe->Signal(lock);

	// release lock
	lock->Release();
	return exitStatus;
}

int Process::Load(char *filename,int argc, char **argv){
	OpenFile *executable = fileSystem->Open(filename);
	if (executable == NULL) {
		printf("Unable to open file %s\n", filename);
		return -1;
	}
	AddrSpace *space = new AddrSpace();
	if (space->Initialize(executable,argc,argv) == -1){
		delete executable;
		delete space;
		return -1;
	}
	mainThread->space = space;
	delete executable;			// close file
	return 0;
}

int Process::PipelineAdd(Process* proc, bool hasin, bool hasout) { 
	if (pipeline == NULL) {
		pipeline = new List();
	}
	if (hasin) {
		//Since we are not recommended to modify List.cc, 
		//we need to remove the element then put it back in order to modify it
		Pipe* pipe = (Pipe*)pipeline->Remove();
		
		if (pipe == NULL) {
			printf("Error: inappropriate usage of pipeline: can't locate the input pipe\n");
			return -1;
		}
		pipe->out = proc->GetId();
		proc->pipeIn = pipe;
		
		pipeline->Prepend(pipe);
	}
	if (hasout) {
		Pipe* pipe = new Pipe();
		
		pipe->in = proc->GetId();
		proc->pipeOut = pipe;
		
		pipeline->Prepend(pipe);
	}
	return 0;
}