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
#include "synchconsole.h"
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

//copy a string from user memory to OS memory, for the currently runing process
//src: char* in virtual memory space
//dst: a pointer to a char* in OS memory. 
//If size is not set, then assume dst == NULL
//If size is set, then assume dst points to an allocated empty string in kernel space
int strUser2Kernel(char* src,char** dst){
	char buff[MaxStringLength];
	int virtAddr = (int)src;
	char ch;
	int count = 0;
	do{
		if (! machine->ReadMem(virtAddr,sizeof(char),(int*)&ch))
			return -1;	//In fact ReadMem() itself will call machine->RaiseException.
		buff[count] = ch;
		count ++;
		if (ch == '\0')
			break;
		virtAddr ++;
		if (count >= MaxStringLength){
			//next byte in buff should be out of boundary
			printf("Error: Exceeded the maximun "
				"string length of %d bytes, or the string does not end in null character.\n",MaxStringLength);
			buff[MaxStringLength-1] = '\0';
			DEBUG('a', "The string was ""%s""\n",buff);
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
int strUser2Kernel(char* src,char* dst,int size){
	int virtAddr = (int)src;
	for (int i=0;i<size;i++){
		if (! machine->ReadMem(virtAddr, sizeof(char), (int*)&dst[i]))
			return -1;
		virtAddr++;
	}
	dst[size]='\0';
	return 0;
}

//assume dst != NULL. 
//dst is a pointer to an allocated empty string in virtual space.
//the string src is not necessarily ending in NULL.
int strKernel2User(char* src,char* dst,int size){
	int virtAddr = (int)dst;
	for (int i=0;i<size;i++){
		if (! machine->WriteMem(virtAddr, sizeof(char), src[i]))
			return -1;
		virtAddr++;
	}
	if (! machine->WriteMem(virtAddr, sizeof(char), '\0'))
		return -1;
	return 0;
}

//Exit the current runing process, including currentThread.
void exit(){
	SpaceId pid = currentThread->processId;
	Process* process = (Process*) processTable->Get(pid);
	process->Finish();
	processTable->Release(pid);
	printf("== the user program (PID=%d) Exit(%d)\n",pid,(int)machine->ReadRegister(4));
	delete process;
	ASSERT(FALSE);
}

void ProcessStart(int arg){
	//degug
	printf("Process ""PID=%d"" starts\n",((Process*)  processTable->Get(currentThread->processId)  )->GetId());
	currentThread->space->InitRegisters();		// set the initial register values
	currentThread->space->RestoreState();		// load page table register
	machine->Run();			// jump to the user program
	ASSERT(FALSE);
}

//Create a process, create a thread
SpaceId exec(char *filename, int argc, char **argv, int willJoin){
	Process* process = new Process("P",willJoin);
	SpaceId pid = processTable->Alloc(process);

	if (pid==-1){
		delete process;
		return 0;//maybe too many processes there. Return SpaceId 0 as error code
	}
	process->SetId(pid);
	if (process->Load(filename,argc,argv) == -1){
		delete process;
		return 0;	//Return SpaceId 0 as error code
	}
	process->mainThread->Fork(ProcessStart, willJoin);
	return pid;
}

void IncreasePC(){
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
}

void
	ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	//for (int i=0;i<10;i++)	printf("[%d]%d\n",i,(int)machine->ReadRegister(i));
	//printf("exception %d %d\n", which, type);

	IncreasePC();

	switch (which){

	case SyscallException: // A program executed a system call
		switch (type){

		case SC_Halt:
			DEBUG('a', "Shutdown, initiated by user program.\n");
			interrupt->Halt();
			break;

		case SC_Exit:
			exit();//will exit the whole current process
			break;

		case SC_Exec:
			{
				//read registers
				char* name = NULL;
				int result = strUser2Kernel((char*)machine->ReadRegister(4),&name) ;
				if (result == -1){
					machine->WriteRegister(2,0);//return SpaceId 0
					return ;
				}
				int argc = machine->ReadRegister(5);
				if (argc<0)	printf("Warning: argc less than 0. Assume argc = 0 \n");
				int willJoin = machine->ReadRegister(7);
				char** argv = NULL;

				if (argc>0){
					//convert argument list
					int* data = new int;
					char** virtArgv = (char**) machine->ReadRegister(6);
					argv=new char*[argc];
					for (int i=0;i<argc;i++){
						//read the string head pointer
						if (  ! machine->ReadMem((int)&(virtArgv[i]) ,4,data)  ){
							machine->WriteRegister(2,0);//return SpaceId 0
							return ;
						}
						//copy the string to OS memory
						result = strUser2Kernel((char*)(*data),&argv[i]) ;
						if (result == -1){
							machine->WriteRegister(2,0);//return SpaceId 0
							return ;
						}
					}
				}
				//debug
				/*for (int i=0;i<argc;i++){
					printf("[%d]%s\n",i,argv[i]);
				}*/
				printf("[]""%s""\n", name);
				result = exec(name,argc,argv,willJoin);
				machine->WriteRegister(2,result);
				delete[] argv;
				break;
			}

		case SC_Read:
		case SC_Write:
			{
				int size = machine->ReadRegister(5);
				if (size<=0){
					printf("Error: Nothing to read or write with size 0, or less than 0\n");
					machine->WriteRegister(2,-1);//should we return -1 ?
					return;
				}
				OpenFileId fileId = (OpenFileId)machine->ReadRegister(6);
				if (fileId == ConsoleInput || fileId == ConsoleOutput){

					if (sConsole == NULL)
						sConsole = new SynchConsole();

					char str[size+1];//a string in kernel

					if (type == SC_Read) {		//SC_Read

						char *buffer = (char*)machine->ReadRegister(4);
						sConsole->Read(str,size);
						if (strKernel2User(str,buffer,size) == -1){
							machine->WriteRegister(2,-1);
							return;
						}

					} else {					//SC_Write

						if (strUser2Kernel((char*)machine->ReadRegister(4),str,size) == -1){
							machine->WriteRegister(2,-1);
							return;
						}
						sConsole->Write(str,size);

					}
				}
				break;
			}

		default:
			printf("Unexpected exception type %d %d\n", which, type);
			ASSERT(FALSE);
			break;
		}
		break;
	case PageFaultException: // No valid translation found
	  printf("PageFaultException: No valid translation found. Terminating process...\n");
	  exit();
	  break;
	case ReadOnlyException: // Write attempted to page marked "read-only"
	  printf("ReadOnlyException: Write attempted to page marked \"read-only\". Terminating process...\n");
	  exit();
	  break;
	case BusErrorException: // Translation resulted in an invalid physical addresss
	  printf("BusErrorException: Translation resulted in an invalid physical address. Terminating process...\n");
	  exit();
	  break;
	case AddressErrorException: // Unaligned reference or one that was beyond the end of the address space
	  printf("AddressErrorException: Unaligned reference or one that was beyond the end of the address space. Terminating process...\n");
	  exit();
	  break;
	case OverflowException: // Integer overflow in add or subtract
	  printf("OverflowException: Integer overflow in add or subract. Terminating process...\n");
	  exit();
	  break;
	case IllegalInstrException: // Unimplemented or reserved instruction
	  printf("IllegalInstrException: Unimplemented or reserved instruction. Terminating process...\n");
	  exit();
	  break;
	default:
		printf("Unexpected user mode exception %d %d\n", which, type);
		ASSERT(FALSE);
		break;
	}
}
