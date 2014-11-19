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
	delete mainThread;
}

void Process::Start(char *filename){
	OpenFile *executable = fileSystem->Open(filename);
	if (executable == NULL) {
		printf("Unable to open file %s\n", filename);
		return;
	}
	AddrSpace *space = new AddrSpace();
	space->Initialize(executable);
	mainThread->space = space;
	delete executable;			// close file
	space->InitRegisters();		// set the initial register values
	space->RestoreState();		// load page table register
	machine->Run();			// jump to the user program
}

void Process::Finish(){
	delete mainThread->space;
	mainThread->Finish();
}