#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "copyright.h"
#include "bitmap.h"
#include "synch.h"

class MemoryManager{

  public:
	  MemoryManager(int numPages);
	  ~MemoryManager();

	  int AllocPage();
	  void FreePage(int physPageNum);
	  bool PageIsAllocated(int physPageNum);
	  void Print(){ memMap->Print(); }
    
  private:  
	 BitMap* memMap;
	 Lock* lock;
};

extern MemoryManager *mm;

#endif