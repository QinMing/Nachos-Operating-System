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

//Exit the current runing process.
void exit(){
	printf("the user program Exits, arg=%d\n",(int)machine->ReadRegister(4));
	for (int i=0;i<5;i++)
		printf("[%d]%d\n",i,(int)machine->ReadRegister(i));
	interrupt->Halt();
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

void
	ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
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
			printf("the user program calls Exec(), arg=%d\n",(int)machine->ReadRegister(4));
			for (int i=0;i<10;i++)
				printf("[%d]%d\n",i,(int)machine->ReadRegister(i));
			interrupt->Halt();
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
