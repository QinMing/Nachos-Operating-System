#include "copyright.h"
#include "process.h"
#include "synch.h"


Process::Process(char* newname,bool willJoin){
	name = newname;
	mainThread = new Thread(newname);
	numThread = 1;
	
	willBeJoined = willJoin;
	hasJoined = false;
	joinedOnMe = Condition("JoinedOnMe");
	lock = new Lock("Lock");
}

Process::Process(char* newname,bool willJoin,Thread* t){
	name = newname;
	mainThread = t;
	numThread = 1;

	willBeJoined = willJoin;
	hasJoined = false;
	joinedOnMe = Condition("JoinedOnMe");
	lock = new Lock("Lock");
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

	if (currentThread == mainThread){

		//the thread will then be deleted in scheduler::Run()
		mainThread->Finish();

	}
	else{

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

void Process::Join() {
	// get lock
	lock->Acquire();

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