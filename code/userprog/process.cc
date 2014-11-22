#include "copyright.h"
#include "process.h"

Process::Process(char* newname,bool willJoin){
	name = newname;
	willBeJoined = willJoin;
	mainThread = new Thread(newname);
	numThread = 1;
}

Process::Process(char* newname,bool willJoin,Thread* t){
	name = newname;
	willBeJoined = willJoin;
	mainThread = t;
	numThread = 1;
}

Process::~Process(){

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