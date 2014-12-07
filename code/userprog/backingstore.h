// backingstore.h

#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H

#include "addrspace.h"

class BackingStore {
 private:
  public:
  /* Create a backing store file for an AddrSpace */
  BackingStore(AddrSpace *as);

  /* Write the virtual page referenced by PTE to the backing store
     Example invocation: PageOut(&machine->pagetTable[virtualPage] or
     PageOut(&space->pageTable[virtualPage])
  */
  void PageOut(TranslationEntry *pte);

  /* Read the virtual page referenced by PTE from the backing store */
  void PageIn(TranslationEntry *pte);

};


#endif
