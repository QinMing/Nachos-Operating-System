#include "copyright.h"
#include "process.h"

Process::Process(char* newname,bool willJoin){
	mainThread = new Thread(newname);
	name = newname;
	willBeJoined = willJoin;
	numThread = 1;
	pipeline = NULL;
	pipeIn = pipeOut = NULL;
}

Process::Process(char* newname,bool willJoin,Thread* t){
	mainThread = t;
	name = newname;
	willBeJoined = willJoin;
	numThread = 1;
	pipeline = NULL;
	pipeIn = pipeOut = NULL;
}

Process::~Process(){

	//should not delete space in Thread::~Thread() because
	//if there are lots of newly created thread, then this space will
	//not be deleted until another thread wakes up from SWITCH(oldThread, nextThread);
	//So here it's just to free the memory in advance.
	delete mainThread->space;
	delete pipeline;

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

int Process::PipelineAdd(bool hasin, bool hasout) {
	if (pipeline == NULL) {
		pipeline = new List();
	}
	if (hasin) {
		//Since we are not recommended to modify List.cc, 
		//we need to remove the element then put it back in order to modify it
		Process* pipe = pipeline->Remove();
		if (pipe == NULL) {
			printf("Error: inappropriate usage of pipeline\n");
			return -1;
		}
		pipeline->Prepend(pipe);
	}
	if (hasout) {

	}
}
void Process::pipeMid() {

}
void Process::pipeEnd() {

}