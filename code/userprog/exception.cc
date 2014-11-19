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
#include "process.h"
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

//copy a string from user memory to OS memory
//src: char* in virtual memory space
//dst: a pointer to a char* in OS memory. Assume dst == NULL
int userStringCopy(char* src,char** dst){
	char buff[MaxStringLength];
	int virtAddr = (int)src;
	int* data = new int;
	char ch;
	int count = 0;
	do{
		if (! machine->ReadMem(virtAddr,1,data))
			return -1;
		//In fact ReadMem() itself will call machine->RaiseException. I don't know what will happen
		//or we need to use Machine::Translate but not ReadMem if it causes problem.
		ch = (char)(*data);
		buff[count] = ch;
		count ++;
		if (ch == '\0')
			break;
		virtAddr ++;
		if (count >= MaxStringLength){
			//next byte in buff should be out of boundary
			printf("Error: File name too long!");
			buff[MaxStringLength-1] = '\0';
			DEBUG('a', "File name was ""%s""\n",buff);
			return -1;
		}	
	}while(1);
	//at this time count should be the length of buff
	(*dst) = new char[count];
	for (int i=0;i<count;i++){
		(*dst)[i]=buff[i];
	}
	return 0;
}

//Exit the current runing process.
void exit(Thread *t){
	//for now don't take Join stuff into consideration
	printf("the user program Exit(%d)\n",(int)machine->ReadRegister(4));
	//for (int i=29;i<40;i++)
	//	printf("[%d]%d\n",i,(int)machine->ReadRegister(i)); 
	SpaceId processId = t->processId;
	Process* process = (Process*) processTable->Get(processId);
	process->Finish();
	processTable->Release(processId);
	delete process;
	ASSERT(FALSE);
}

void ProcessStart(int arg){
	//degub
	printf("Process ""%s"" starts\n",((Process*)  processTable->Get(currentThread->processId)  )->GetName());

	//reference the process that the currenThread resided
	//((Process*)  processTable->Get(currentThread->processId)  )->Load((char*)filename);
	machine->Run();			// jump to the user program
	ASSERT(FALSE);
}

//Create a process, create a thread
SpaceId exec(char *filename, int argc, char **argv, int willJoin){
	Process* process = new Process("P");
	SpaceId id = processTable->Alloc(process);
	if (id==-1){
		delete process;
		return 0;//maybe too many processes there. Return SpaceId 0 as error code
	}
	process->SetId(id);
	process->Load(filename,argc,argv,willJoin);
	process->mainThread->Fork(ProcessStart, 0);

	//read PC
	int currentPC = machine->ReadRegister(PCReg);
	int nextPC = machine->ReadRegister(NextPCReg);
	//increase PC
	int prevPC = currentPC;
	currentPC = nextPC;
	nextPC += 4;
	machine->WriteRegister(PrevPCReg, prevPC);
	machine->WriteRegister(PCReg, currentPC);
	machine->WriteRegister(NextPCReg, nextPC);
	return id;
}

void
	ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	//for (int i=0;i<10;i++)	printf("[%d]%d\n",i,(int)machine->ReadRegister(i));
	//printf("exception %d %d\n", which, type);
	switch (which){
	case SyscallException:
		switch (type){
		case SC_Halt:
			DEBUG('a', "Shutdown, initiated by user program.\n");
			interrupt->Halt();
			break;
		case SC_Exit:
			exit(currentThread);//will exit the whole process
			break;
		case SC_Exec:
			{
				//read registers
				char* name = NULL;
				int result = userStringCopy((char*)machine->ReadRegister(4),&name) ;
				if (result == -1){
					machine->WriteRegister(2,0);//return SpaceId 0
					return ;
				}
				int argc = machine->ReadRegister(5);
				char** virtArgv = (char**) machine->ReadRegister(6);
				int willJoin = machine->ReadRegister(7);
				int* data = new int;

				//convert argument list
				char** argv=new char*[argc];
				for (int i=0;i<argc;i++){
					//read the string head pointer
					if (  ! machine->ReadMem((int)&(virtArgv[i]) ,4,data)  ){
						machine->WriteRegister(2,0);//return SpaceId 0
						return ;
					}
					//copy the string to OS memory
					result = userStringCopy((char*)(*data),&argv[i]) ;
					if (result == -1){
						machine->WriteRegister(2,0);//return SpaceId 0
						return ;
					}
				}
				//debug
				//for (int i=0;i<argc;i++){
				//	printf("[%d]%s\n",i,argv[i]);
				//}
				result = exec(name,argc,argv,willJoin);
				machine->WriteRegister(2,result);
				delete[] argv;
				break;
			}
		default:
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
