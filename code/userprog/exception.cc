// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"
#include "table.h"

Table *table = new Table(1000);

//copy a string from user memory to OS memory
//assume dst == NULL
int userStringCopy(char* src,char* dst){
	char buff[MaxFileName];
	int virtAddr = (int)src;
	int* data;
	char ch;
	int count = 0;
	do{
		machine->ReadMem(virtAddr,1,data);
		ch = (char)(*data);
		buff[count] = ch;
		count ++;
		if (ch == '\0')
			break;
		virtAddr ++;
		if (count >= MaxFileSize){
			//next byte in buff should be out of boundary
			printf("Error: File name too long!");
			buff[MaxFileName-1] = '\0';
			DEBUG('a', "File name was ""%s""\n",buff);
			return -1;
		}	
	}while(1);
	//at this time count should be the length of buff
	dst = new char[count];
	for (int i=0;i<count;i++){
		dst[i]=buff[i];
	}	
	return 0;
}

//Exit the current runing process.
void exit(){
	int currentPC = machine->ReadRegister(PCReg);
	int nextPC = machine->ReadRegister(NextPCReg);
	
	delete currentThread->space; //deallocate AddrSpace & free pysical page
	table->Release(processId);
	
	printf("the user program Exits, arg=%d\n",(int)machine->ReadRegister(4));
	//for (int i=29;i<40;i++)												//not sure whichc arg to print print user program CPU state, see machine.h
	//	printf("[%d]%d\n",i,(int)machine->ReadRegister(i)); 
	printf("[%d]%d\n", 4, (int)machine->ReadRegister(4));

	currentPC = nextPC;
	nextPC += 4;
	machine->WriteRegister(PCReg, currentPC);
	machine->WriteRegister(NextPCReg, nextPC);
}

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------



void ProcessStart(char *filename){
		
	OpenFile *executable = fileSystem->Open(filename);
	AddrSpace *space;

	if (executable == NULL) {
		printf("Unable to open file %s\n", filename);
		return ;
	}
	space = new AddrSpace();
	space->Initialize(executable);
	currentThread->space = space;

	delete executable;			// close file

	space->InitRegisters();		// set the initial register values
	space->RestoreState();		// load page table register

	machine->Run();			// jump to the user progam
	ASSERT(FALSE);			// machine->Run never returns;

	
}

SpaceId Exec(char *name, int argc, char **argv, int opt){
	printf("the user program calls Exec(), arg=%d\n",(int)machine->ReadRegister(4));
	SpaceId processId;
	processId=table->Alloc;
	if (processId==-1)
	  return 0;
			
	for (int i=0;i<10;i++)
		printf("[%d]%d\n",i,(int)machine->ReadRegister(i));
			
	t = new Thread("one");		//t->Fork(ProcessStart(filename), 0);
	return processId;

}

void
	ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	int currentPC = machine->ReadRegister(PCReg);
	int nextPC = machine->ReadRegister(NextPCReg);
	char* filename;
	Thread *t;

	printf("exception %d %d\n", which, type);
	switch (which){
	case SyscallException:
		switch (type){
		case SC_Halt:
			DEBUG('a', "Shutdown, initiated by user program.\n");
			interrupt->Halt();
			break;
		case SC_Exit:
			exit();
			break;
		case SC_Exec:
			SpaceId Exec(filename);
			break;
		default :
			printf("Unexpected exception type %d %d\n", which, type);
			ASSERT(FALSE);
			break;
		}
		break;
	default:
		printf("Unexpected user mode exception %d %d\n", which, type);
		ASSERT(FALSE);
		break;
	}
}
