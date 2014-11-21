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

	delete mainThread->space;
	//should not delete space in Thread::~Thread() because
	//if there are lots of newly created thread, then this space will
	//not be deleted until another thread wakes up from SWITCH(oldThread, nextThread);
	//So it's just to free the memory in advance.
	
	mainThread->Finish();
	//the thread will then be deleted in scheduler::Run()
}

//willJoin is not used. Could be removed
int Process::Load(char *filename,int argc, char **argv, int willJoin){
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

void Process::Finish(){
}