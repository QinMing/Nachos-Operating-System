// progtest.cc
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "synch.h"
#include "addrspace.h"
#include "memoryManager.h"
#include "table.h"
#include "process.h"


MemoryManager *mm;
Table *processTable;//the process table, with 
		//the table index being the SpaceID, and the content being Process*

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

void StartProcess(char *filename)
{
	mm=new MemoryManager(NumPhysPages);
	processTable = new Table(MaxNumProcess);
	
	Process* process = new Process("FirstProcess",0,currentThread);
	SpaceId id = processTable->Alloc(process);
	//if (id == -1)			//run out of process table. but impossible
	process->SetId(id);
	id = process->Load(filename,0,NULL,0);
	ASSERT(id!=-1);			//panic. The initial user program has problem.
	currentThread->space->InitRegisters();		// set the initial register values
	currentThread->space->RestoreState();		// load page table register
	machine->Run();			// jump to the user program
	ASSERT(FALSE);			// machine->Run never returns;
	// the address space exits
	// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg) {
    readAvail->V();
}
static void WriteDone(int arg) {
    writeDone->V();
}

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);

    for (;;) {
        readAvail->P();		// wait for character to arrive
        ch = console->GetChar();
        console->PutChar(ch);	// echo it!
        writeDone->P() ;        // wait for write to finish
        if (ch == 'q') return;  // if q, quit
    }
}
