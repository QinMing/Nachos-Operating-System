#include "copyright.h"
#include "memoryManager.h"
#include "system.h"

#include "sysdep.h"

/* Create a manager to track the allocation of numPages of physical memory.
You will create one by calling the constructor with NumPhysPages as
the parameter.  All physical pages start as free, unallocated pages. */
MemoryManager::MemoryManager(int numPages,EvictMethodType replacementMethod)
{
	numPhysPages = numPages;
	memMap = new BitMap(numPages);
	lock = new Lock("mapLock");

	//for demand paging
	spaceTable = new AddrSpace*[numPages];
	vpnTable = new int[numPages];
	for(int i=0;i<numPages;i++){
		spaceTable[i] = NULL;
		vpnTable[i] = -1;
	}

	//for demand paging: evicting algorithm
	EvictMethod = replacementMethod;
	switch (EvictMethod) {
	case FIFO:
		fifoList = new List();
		break;
	case random:
		break;
	case LRU:
		//int counter[numPhysPages];
		//int i;
		break;
	}
	
	//debug 
	//printf("##NumPhysPages=%d\n",numPages);
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
	if (physNum ==-1){
		
		physNum = victimPage();		
		//ask victim addrspace to evict its page
		//assuming no error		
		spaceTable[physNum]->evictPage(vpnTable[physNum]);
		
		#if 0
		printf("phys %d, (PID %d vpn %d)\t->\t(PID %d vpn %d) \n",physNum,
			   spaceTable[physNum]->debugPid,vpnTable[physNum],
			   space->debugPid, vpn);
		#endif
	}
	
	spaceTable[physNum] = space;
	vpnTable[physNum] = vpn;

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
		ASSERT(!fifoList->IsEmpty());
		ppn = (int)fifoList->Remove();
		//} while (!memMap->Test(ppn));
		
		break;
	case random:
		ppn = Random() % numPhysPages;//change the seed using -rs
		break;
	case LRU:
		ppn=0;
		int i;
		for(i=1;i<numPhysPages;i++){
		    if(machine->LRUcounter[i]>machine->LRUcounter[ppn])
		      ppn=i;
		}
		break;
	}

	//printf("Evicting %d\n",ppn);
	return ppn;
}