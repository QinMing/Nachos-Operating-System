#include "copyright.h"
#include "memoryManager.h"

#include "sysdep.h"

/* Create a manager to track the allocation of numPages of physical memory.
You will create one by calling the constructor with NumPhysPages as
the parameter.  All physical pages start as free, unallocated pages. */
MemoryManager::MemoryManager(int numPages)
{
	numPhysPages = numPages;
	memMap = new BitMap(numPages);
	lock = new Lock("mapLock");

	//for demand paging
	spaceTable = new AddrSpace*[numPages];
	vpnTable = new int[numPages];

	//for demand paging: evicting algorithm
	EvictMethod = FIFO;
	switch (EvictMethod) {
	case FIFO:
		fifoList = new List();
		break;
	case random:
		break;
	case LRU:
		break;
	}
}

MemoryManager::~MemoryManager()
{
	delete memMap;
	delete lock;

	delete[] spaceTable;
	delete[] vpnTable;

	switch (EvictMethod) {
	case FIFO:
		delete fifoList;
		break;
	case random:
		break;
	case LRU:
		break;
	}
}

/* Allocate a free page, returning its physical page number or -1
if there are no free pages available. */
int MemoryManager::AllocPage(AddrSpace* space, int vpn) {
	lock->Acquire();
	int physNum = memMap->Find();
	if (physNum != -1) {
		spaceTable[physNum] = space;
		vpnTable[physNum] = vpn;
	}
	else 
	{
		physNum = victimPage();
		
		//ask victim addrspace to evict its page
		spaceTable[physNum]->evictPage(vpnTable[physNum]);
		//assuming no error

		spaceTable[physNum] = space;
		vpnTable[physNum] = vpn;

	}
	if (EvictMethod==FIFO)
		fifoList->Append((void*)physNum);
	lock->Release();
	return physNum;
}

/* Free the physical page and make it available for future allocation. */
void MemoryManager::FreePage(int physPageNum){
	lock->Acquire();
	memMap->Clear(physPageNum);
	spaceTable[physPageNum] = NULL;
	vpnTable[physPageNum] = -1;
	lock->Release();
}

/* True if the physical page is allocated, false otherwise. */
bool MemoryManager::PageIsAllocated(int physPageNum) {
	lock->Acquire();
	bool physAllocated = memMap->Test(physPageNum);
	lock->Release();
	return physAllocated;
}

//return the physical page number of a victim page to be evict
int MemoryManager::victimPage() {
	//assuming lock is already acquired
	int ppn;

	switch (EvictMethod) {
	case FIFO:
		//do {
			//ASSERT(!fifoList->IsEmpty());//Otherwise it will be trapped in dead loop.
			//although impossible
		ppn = (int)fifoList->Remove();
		//} while (!memMap->Test(ppn));
		break;
	case random:
		ppn = Random() % numPhysPages;//change the seed using -rs
		break;
	case LRU:
		break;
	}

	//printf("Evicting %d\n",ppn);
	return ppn;
}