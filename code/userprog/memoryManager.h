#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "copyright.h"
#include "bitmap.h"
#include "synch.h"
#include "machine.h"
#include "list.h"
#include "addrspace.h"

enum EvictMethodType {
	FIFO,
	random,
	LRU
};

class MemoryManager {

public:
	MemoryManager(int numPages,EvictMethodType replacementMethod);
	~MemoryManager();

	int AllocPage(AddrSpace* space, int vpn);
	void FreePage(int physPageNum);
	bool PageIsAllocated(int physPageNum);
	void Print() { memMap->Print(); }

private:
	int numPhysPages;//now just a copy of NumPhysPages
	BitMap* memMap;
	Lock* lock;

	//used by demand paging
	AddrSpace** spaceTable;
	int* vpnTable;
	int victimPage();

	//demand paging: evicting algorithm
	EvictMethodType EvictMethod;
	List* fifoList;
};

extern MemoryManager *mm;

#endif