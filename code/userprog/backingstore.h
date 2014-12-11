// backingstore.h

#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H

#include "addrspace.h"
#include "filesys.h"
class AddrSpace;

class BackingStore {
 private:
  OpenFile *bsFile;
  int numPages;
  char *bsFileName;
  bool *valid;
 public:
  /* Store file name for an AddrSpace backing store */
  BackingStore(AddrSpace *as, int numPages, int pid);

  ~BackingStore();

  /* Actually create the backing store file (on first evict) */
  void init();

  /* Write the virtual page referenced by PTE to the backing store
     Example invocation: PageOut(&machine->pagetTable[virtualPage] or
     PageOut(&space->pageTable[virtualPage])
  */
  void PageOut(TranslationEntry *pte);

  /* Read the virtual page referenced by PTE from the backing store */
  int PageIn(TranslationEntry *pte);

};


#endif
