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
	delete mainThread;
}

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
	mainThread->Finish();
}