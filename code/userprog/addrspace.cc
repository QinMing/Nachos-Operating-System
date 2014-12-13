// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "memoryManager.h"
#include "utility.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

#ifdef AddrSpaceTestToggle
//AddrSpaceTest addrSpaceTest;
#endif


//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader (NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace()
{    	
	pageTable = NULL;
	createNewThread = NULL;
	backingStore = NULL;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
	if (pageTable != NULL){
		for (int i=0;i<numPages;i++)
			if (pageTable[i].valid)//This line of code keeps us debuging for 2 whole days!
				mm->FreePage(pageTable[i].physicalPage);
		delete[] pageTable;
	}
	delete exeFile;			// close file

	//mm->Print();
	
	if (backingStore) delete backingStore;
}

//determine which segment does a address located in
//return value: 
int AddrSpace::whichSeg(int virtAddr, Segment* segPtr) {
	if (noffH.code.size > 0) {
		if (( virtAddr >= noffH.code.virtualAddr ) &&
			( virtAddr < noffH.code.virtualAddr + noffH.code.size ))
		{
			( *segPtr ) = noffH.code;
			return 0;
		}
	}
	if (noffH.initData.size > 0) {
		if (( virtAddr >= noffH.initData.virtualAddr ) &&
			( virtAddr < noffH.initData.virtualAddr + noffH.initData.size ))
		{
			( *segPtr ) = noffH.initData;
			return 1;
		}
	}
	if (noffH.uninitData.size > 0) {
		if (( virtAddr >= noffH.uninitData.virtualAddr ) &&
			( virtAddr < noffH.uninitData.virtualAddr + noffH.uninitData.size ))
		{
			( *segPtr ) = noffH.uninitData;
			return 2;
		}
	}
	return 3;
}

//assume there's no bubble in the exe file.
//Because when the first address in the page is not in code, initData or uninitData segment, 
//the whole page will be zero-filled
int AddrSpace::loadPage(int vpn) {
	int readAddr, physAddr, size, segOffs;
	int virtAddr = vpn * PageSize;
	int offs = 0;
	Segment seg;
	bool readFromFile=FALSE;
	
	pageTable[vpn].readOnly = FALSE;

	do {
		physAddr = pageTable[vpn].physicalPage * PageSize + offs;
		switch (whichSeg(virtAddr, &seg)) {
		case 0://code
		{
			segOffs = virtAddr - seg.virtualAddr;
			readAddr = segOffs + seg.inFileAddr;
			size = min(PageSize - offs, seg.size - segOffs);
			exeFile->ReadAt(&( machine->mainMemory[physAddr] ), size, readAddr);
			readFromFile=TRUE;
			if (size==PageSize){
				pageTable[vpn].readOnly = TRUE;
			}
			if (vpn==1)
				ASSERT(machine->mainMemory[physAddr]==7);
			break;
		}
		case 1://initData
		{
			segOffs = virtAddr - seg.virtualAddr;
			readAddr = segOffs + seg.inFileAddr;
			size = min(PageSize - offs, seg.size - segOffs);
			exeFile->ReadAt(&( machine->mainMemory[physAddr] ), size, readAddr);
			readFromFile=TRUE;
			break;
		}
		case 2://uninitData
		{
			size = min(PageSize - offs, seg.size + seg.virtualAddr - virtAddr);
			bzero(&( machine->mainMemory[physAddr] ), size);
			break;
		}
		case 3://stack or others
		{
			bzero(&( machine->mainMemory[physAddr] ), PageSize - offs);
			return 0;//don't use break
		}
		}
		offs += size;
		virtAddr += size;
	} while (offs < PageSize);
	if (readFromFile)
		stats->numPageIns++;
	return 0;
}

//called by memory manager
int AddrSpace::evictPage(int vpn){
	
	if (pageTable[vpn].dirty){
		backingStore->PageOut(&pageTable[vpn]);
	}
	pageTable[vpn].physicalPage = -1;
	pageTable[vpn].valid = FALSE;
	pageTable[vpn].use = FALSE;
	pageTable[vpn].dirty = FALSE;

	return 0;
}

int AddrSpace::pageFault(int vpn) {
	pageTable[vpn].physicalPage = mm->AllocPage(this,vpn);
	
	if (pageTable[vpn].physicalPage == -1){
		printf("Error: run out of physical memory\n");
		//to do://should yield and wait for memory space and try again?
		ASSERT(FALSE);//panic at this time
	}
	
	if(backingStore->PageIn(&pageTable[vpn])==-1){
		loadPage(vpn);
	}
	
	pageTable[vpn].valid = TRUE;
	pageTable[vpn].use = FALSE;
	pageTable[vpn].dirty = FALSE;
	//pageTable[vpn].readOnly is modified in loadPage()
	
	return 0;
}

int AddrSpace::Initialize(OpenFile *executable, int argc, char **argv, int pid){
	//NoffHeader noffH;
	int size, i;

	debugPid = pid;
	exeFile = executable;
	executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
	if ((noffH.noffMagic != NOFFMAGIC) &&
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
		SwapHeader(&noffH);
	if (noffH.noffMagic != NOFFMAGIC) {
		printf("Error: The file may not be a good executable.\n");
		return -1;
	}
	//Check if there is ibubble in the virtual space of the executable
	//use i to keep the max possible virtual address
	size = 0;
	if (noffH.code.size > 0)
		size = max(noffH.code.virtualAddr + noffH.code.size, size);
	if (noffH.initData.size > 0)
		size = max(noffH.initData.virtualAddr + noffH.initData.size, size);
	if (noffH.uninitData.size > 0)
		size = max(noffH.uninitData.virtualAddr + noffH.uninitData.size, size);
	if (size != noffH.code.size + noffH.initData.size + noffH.uninitData.size){
		printf("ERROR: There's bubble or overlap in program memory space\n");
		printf("size = %d\n", size);
		printf("noffH.code %d,%d\n", noffH.code.virtualAddr, noffH.code.size);
		printf("noffH.initData %d,%d\n", noffH.initData.virtualAddr, noffH.initData.size);
		printf("noffH.uninitData %d,%d\n", noffH.uninitData.virtualAddr, noffH.uninitData.size);
		return -1;
	}
	
// 	int argHeadVirtAddr = size;
// 	int argSize = argc * MaxStringLength + argc * sizeof(char*) + 3;// add 4 for alignment problems
// 	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
// 		+ argSize + UserStackSize;
		
	//Adding arguments at the end of these three part	
	int argHeadVirtAddr = divRoundUp(size, PageSize)*PageSize;
	int argSize = argc * MaxStringLength + argc * sizeof(char*) + 3;// add 4 for alignment problems
	
	// how big is address space?
	size = argHeadVirtAddr + argSize + UserStackSize;

	numPages = divRoundUp(size, PageSize);

	// to leave room for future user threads
	maxNumPages = numPages + MaxUserThread*UserStackNumPage;

	// Set up the translation
	pageTable = new TranslationEntry[maxNumPages];
	for (i = 0; i < maxNumPages; i++) {		
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
		pageTable[i].valid = FALSE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
		// a separate page, we could set its pages to be read-only
	}
	// Create backing store
	backingStore = new BackingStore(this, maxNumPages, pid);
	
	//Adding arguments into user memory
	if (argc >0 ){
		int virtAddr,physAddr;
		for (i=0;i<argc;i++){
			virtAddr = argHeadVirtAddr+ i*MaxStringLength;
			if (!pageTable[virtAddr/PageSize].valid){
				pageFault(virtAddr/PageSize);
				pageTable[virtAddr/PageSize].use = TRUE;
				pageTable[virtAddr/PageSize].dirty = TRUE;
				ASSERT(pageTable[virtAddr/PageSize].valid);//panic for now
			}
			char* src=argv[i];
			int j = 0;
			while (src[j] != '\0'){
				physAddr = pageTable[virtAddr / PageSize].physicalPage * PageSize + virtAddr % PageSize;
				machine->mainMemory[physAddr] = src[j];
				virtAddr ++;
				j++;
				if ((virtAddr % PageSize==0)&&(!pageTable[virtAddr/PageSize].valid)){
					pageFault(virtAddr/PageSize);
					pageTable[virtAddr/PageSize].use = TRUE;
					pageTable[virtAddr/PageSize].dirty = TRUE;
					ASSERT(pageTable[virtAddr/PageSize].valid);//panic for now
				}
			}
			physAddr = pageTable[virtAddr / PageSize].physicalPage * PageSize + virtAddr % PageSize;
			machine->mainMemory[physAddr] = '\0';
		}
		argcForMain = argc;
		virtAddr = argHeadVirtAddr + argc*MaxStringLength;
		if (virtAddr & 0x3)//alignment
			virtAddr += 4-(virtAddr & 0x3);
		argvAddrForMain = virtAddr;
		for (i=0;i<argc;i++){
			if (!pageTable[virtAddr/PageSize].valid){
				pageFault(virtAddr/PageSize);
				pageTable[virtAddr/PageSize].use = TRUE;
				pageTable[virtAddr/PageSize].dirty = TRUE;
				ASSERT(pageTable[virtAddr/PageSize].valid);//panic for now
			}
			physAddr = pageTable[virtAddr / PageSize].physicalPage * PageSize + virtAddr % PageSize;
			*(char**) &machine->mainMemory[physAddr] = (char*)argHeadVirtAddr + i*MaxStringLength;
			virtAddr += 4;
		}
	}
	//mm->Print();
	return 0;
}

//create new stack space for a new user level thread
int AddrSpace::NewStack() 
{
	if (createNewThread == NULL)
		createNewThread = new Semaphore("createNewThread",1);
	createNewThread->P();

	//For now, only add space at the end, but will not free memory for finished threads.
	if (numPages + UserStackNumPage > maxNumPages) {
		printf("Error: too many user threads!\n");
		return -1;
	}
	numPages += UserStackNumPage;

	return 0;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------
void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);

	// Initial arguments
	machine->WriteRegister(4,argcForMain);
	machine->WriteRegister(5,argvAddrForMain);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

void AddrSpace::InitNewThreadRegs(int func)
{
	int i;

	for (i = 0; i < NumTotalRegs; i++)
		machine->WriteRegister(i, 0);

	machine->WriteRegister(PCReg, func);

	machine->WriteRegister(NextPCReg, func+4);

	machine->WriteRegister(StackReg, numPages * PageSize - 16);
	DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);

	//ready to create another user thread
	createNewThread->V();
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

int AddrSpace::printPage(int vpn){
	ASSERT(this==currentThread->space);
	printf("vpn %d\t\t",vpn);
	//int virtAddr = vpn*PageSize;
	int virtAddr = 128;

	char *ptr = new char;
	for (int i=0;i<PageSize;i++){
		if (!machine->ReadMem(virtAddr+i, 1, (int*)ptr))
			machine->ReadMem(virtAddr+i, 1, (int*)ptr);
		printf("[%3d]0x%3x\t",i,(*ptr));
		if ((i+1)%5==0) printf("\n\t\t");
	}
	delete ptr;
	printf("\n");
	
	return 0;
}
