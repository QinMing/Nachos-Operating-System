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

void Process::Load(char *filename,int argc, char **argv, int willJoin){
	OpenFile *executable = fileSystem->Open(filename);
	if (executable == NULL) {
		printf("Unable to open file %s\n", filename);
		return;
	}
	AddrSpace *space = new AddrSpace();
	space->Initialize(executable,argc,argv);
	mainThread->space = space;
	delete executable;			// close file
	space->InitRegisters();		// set the initial register values
	space->RestoreState();		// load page table register
}

void Process::Finish(){
	mainThread->Finish();
}