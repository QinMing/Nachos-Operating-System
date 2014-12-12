// addrspace.h
//	Data structures to keep track of executing user programs
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "noff.h"
#include "filesys.h"

#include "synch.h"
#include "backingstore.h"
class Semaphore;
class BackingStore;

#define UserStackNumPage	16					// increase this as necessary!
#define UserStackSize		(UserStackNumPage*PageSize) 	
	//need to be a multiple of PageSize (128 B) 
	//when allocating stack for user threads

#define MaxUserThread		16

#define AddrSpaceTestToggle
#ifdef AddrSpaceTestToggle

class AddrSpaceTest {
	public:
		AddrSpaceTest(){
			mainMemory = new char[128];
			pageTable = new TranslationEntry[128];
		}
		~AddrSpaceTest(){
			delete[] pageTable;
			delete[] mainMemory;
		}
		char* mainMemory;
		
		TranslationEntry *pageTable;	
		int maxNumPages;
		int numPages;		// Number of pages in the virtual address space
		
		//arguments to be pass to main() function
		int argcForMain;
		int argvAddrForMain;	//location of char** argv, in virtual space
		
		int compare(int vpn, int physAddr);
		int Initialize(OpenFile *executable, int argc, char **argv);
};
#endif

class AddrSpace {
public:
	AddrSpace();	// Create an address space,
	// initializing it with the program
	// stored in the file "executable"
	~AddrSpace();			// De-allocate an address space

	int Initialize(OpenFile *executable, int argc, char **argv, int pid);
    void InitRegisters();		// Initialize user-level CPU registers,
    // before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch

	//functions for user level threads
	int NewStack();
	void InitNewThreadRegs(int func);

	//functions for demand paging
	int pageFault(int vpn);
	int evictPage(int vpn);


private:
    TranslationEntry *pageTable;
	int maxNumPages;
    int numPages;		// Number of pages in the virtual address space

	//arguments to be pass to main() function
	int argcForMain;
	int argvAddrForMain;	//location of char** argv, in virtual space

	//synchronize user thread creation
	Semaphore *createNewThread;

	//Added in project 3 Demand paging
	NoffHeader noffH;
	OpenFile *exeFile;

	BackingStore* backingStore;

	int loadPage(int vpn);
	int whichSeg(int virtAddr, Segment* segPtr);
	
	#ifdef AddrSpaceTestToggle
	AddrSpaceTest* addrSpaceTest;
	#endif
};

#endif // ADDRSPACE_H
