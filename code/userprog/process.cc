#include "copyright.h"
#include "process.h"

Process::Process(char* newname){
	name = newname;
	mainThread = new Thread(newname);
	numThread = 1;
}

Process::Process(char* newname,Thread* t){
	name = newname;
	mainThread = t;
	numThread = 1;
}

Process::~Process(){
	ASSERT(numThread == 1);
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
	space->InitRegisters();		// set the initial register values
	space->RestoreState();		// load page table register
	return 0;
}

void Process::Finish(){
	mainThread->Finish();
}