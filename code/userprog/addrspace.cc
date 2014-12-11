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
//#include "noff.h"
#include "utility.h"
#ifdef HOST_SPARC
#include <strings.h>
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
			mm->FreePage(pageTable[i].physicalPage);
		delete[] pageTable;
	}
	delete exeFile;			// close file

	//mm->Print();
	
	if (backingStore) delete backingStore;
}

//determine which segment does a address located in
//return value: 
//0 : code, initData or uninitData
//1 : stack
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
			return 0;
		}
	}
	if (noffH.uninitData.size > 0) {
		if (( virtAddr >= noffH.uninitData.virtualAddr ) &&
			( virtAddr < noffH.uninitData.virtualAddr + noffH.uninitData.size ))
		{
			( *segPtr ) = noffH.uninitData;
			return 1;
		}
	}
	return 2;
}

//assume there's no bubble in the exe file.
//Because when the first address in the page is not in code, initData or uninitData segment, 
//the whole page will be zero-filled
int AddrSpace::loadPage(int vpn) {
	int readAddr, physAddr, size, segOffs;
	int virtAddr = vpn * PageSize;
	int offs = 0;
	Segment seg;
	
	stats->numPageIns++;

	do {
		physAddr = pageTable[vpn].physicalPage * PageSize + offs;
		switch (whichSeg(virtAddr, &seg)) {
		case 0://code or initData
		{
			segOffs = virtAddr - seg.virtualAddr;
			readAddr = segOffs + seg.inFileAddr;
			size = min(PageSize - offs, seg.size - segOffs);
			exeFile->ReadAt(&( machine->mainMemory[physAddr] ), size, readAddr);
			//printf("====LoadPage[%d]seg.virtualAddr=%d, physAddr=%d, size=%d, readAddr=%d\n", vpn, seg.virtualAddr, physAddr, size, readAddr);
			break;
		}
		case 1://uninitData
		{
			size = min(PageSize - offs, seg.size + seg.virtualAddr - virtAddr);
			bzero(&( machine->mainMemory[physAddr] ), size);
			break;

		}
		case 2://stack or others
		{
			//printf("====ZeroPage[%d]physAddr=%d, size=%d\n", vpn, physAddr, PageSize - offs);
			bzero(&( machine->mainMemory[physAddr] ), PageSize - offs);
			return 0;//don't use break
		}
		}
		offs += size;
		virtAddr += size;
	} while (offs < PageSize);
	return 0;
}

int AddrSpace::pageFault(int vpn) {
	pageTable[vpn].physicalPage = mm->AllocPage(this,vpn);
	if (pageTable[vpn].physicalPage == -1){
		printf("Error: run out of physical memory\n");
		//to do:
		//should yield and wait for memory space
		//and try again
		ASSERT(FALSE);//panic at this time
	}
	pageTable[vpn].valid = TRUE;
	
	return loadPage(vpn);
}

int AddrSpace::Initialize(OpenFile *executable, int argc, char **argv, int pid){
	//NoffHeader noffH;
	int size, i;

	exeFile = executable;
	executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
	if ((noffH.noffMagic != NOFFMAGIC) &&
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
		SwapHeader(&noffH);
	if (noffH.noffMagic != NOFFMAGIC) {
		printf("Error: The file may not be a good executable.\n");
		return -1;
	}
	//Check if there is bubble in the virtual space of the executable
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

	//Adding arguments at the end of these three part
	int argHeadVirtAddr = size;
	int argSize = argc * MaxStringLength + argc * sizeof(char*) + 3;// add 4 for alignment problems

	// how big is address space?
	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
		+ argSize + UserStackSize;

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

	printf("noffH.code %d,%d\n", noffH.code.virtualAddr, noffH.code.size);
	printf("noffH.initData %d,%d\n", noffH.initData.virtualAddr, noffH.initData.size);
	printf("noffH.uninitData %d,%d\n", noffH.uninitData.virtualAddr, noffH.uninitData.size);

	//debug
	//printf("mmap:  First phys page=%d, size = %d\n",pageTable[0].physicalPage,size);


	// zero out the entire address space, to zero the unitialized data segment
	// and the stack segment
	//bzero(machine->mainMemory, size);

	//for (i = 0; i < numPages; i++) {
	//	int physAddr=pageTable[i].physicalPage * PageSize;
	//	bzero(&(machine->mainMemory[physAddr]), PageSize);
	//}

	//then, copy in the code and data segments into memory
	/*
	if (noffH.code.size > 0) {
		int readAddr = noffH.code.inFileAddr;
		int virtAddr = noffH.code.virtualAddr;
		int page = virtAddr / PageSize;
		int offs = virtAddr % PageSize;
		int physAddr;

		if (offs != 0){
			size = PageSize - offs;
			if (size > noffH.code.size)
				size = noffH.code.size;
			physAddr = pageTable[page].physicalPage * PageSize + offs;
			executable->ReadAt(&(machine->mainMemory[physAddr]),size,readAddr);
			readAddr += size;
			virtAddr += size;
			page++;
			offs = 0;
		}
		size = PageSize;
		while (virtAddr + PageSize <= noffH.code.virtualAddr + noffH.code.size) {
			physAddr = pageTable[page].physicalPage * PageSize;
			executable->ReadAt(&(machine->mainMemory[physAddr]), size, readAddr);
			pageTable[page].readOnly = TRUE;//a page of entire code
			readAddr += PageSize;
			virtAddr += PageSize;
			page ++;
		}
		if (virtAddr < noffH.code.virtualAddr + noffH.code.size){
			size = noffH.code.virtualAddr + noffH.code.size - virtAddr;
			physAddr = pageTable[page].physicalPage * PageSize;
			executable->ReadAt(&(machine->mainMemory[physAddr]),size,readAddr);
		}
	}*/

	//Adding arguments into user memory
	if (argc >0 ){
		int virtAddr,physAddr;
		for (i=0;i<argc;i++){
			virtAddr = argHeadVirtAddr+ i*MaxStringLength;
			if (!pageTable[virtAddr/PageSize].valid){
				pageFault(virtAddr/PageSize);
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
					ASSERT(pageTable[virtAddr/PageSize].valid);//panic for now
				}
			}
			physAddr = pageTable[virtAddr / PageSize].physicalPage * PageSize + virtAddr % PageSize;
			machine->mainMemory[physAddr] = '\0';
			//debug
			//printf("finish loading ""%s""\n",src);
		}
		argcForMain = argc;
		virtAddr = argHeadVirtAddr + argc*MaxStringLength;
		if (virtAddr & 0x3)//alignment
			virtAddr += 4-(virtAddr & 0x3);
		argvAddrForMain = virtAddr;
		for (i=0;i<argc;i++){
			if (!pageTable[virtAddr/PageSize].valid){
				pageFault(virtAddr/PageSize);
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

//look for a page. returns physical page number
/*int AddrSpace::Translate(int vpn) {
	if (vpn >= pageTableSize) {
		DEBUG('a', "virtual page # %d too large for page table size %d!\n",
			  vpn, pageTableSize);
		return -1;
	} else if (!pageTable[vpn].valid) {
		DEBUG('a', "virtual page # %d invalid!\n",
			  virtAddr);
		return -1;
	}
	return &pageTable[vpn];
}
*/

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

/* codes in project 2
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
	int i;
	int oldNumPages = numPages;
	numPages += UserStackNumPage;
	
	for (i = oldNumPages; i < numPages; i++) {
		pageTable[i].virtualPage = i;
		pageTable[i].physicalPage = mm->AllocPage();
		if (pageTable[i].physicalPage == -1) {
			printf("Error: run out of physical memory\n");
			for (int j = oldNumPages; j<i; j++)
				mm->FreePage(pageTable[j].physicalPage);
			numPages -= UserStackNumPage;
			return -1;
		}
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;  
	}
	
	for (i = oldNumPages; i < numPages; i++) {
		int physAddr = pageTable[i].physicalPage * PageSize;
		bzero(&( machine->mainMemory[physAddr] ), PageSize);
	}
	
	return 0;
}
end of codes in project 2
*/

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
