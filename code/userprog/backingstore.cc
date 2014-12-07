#include "backingstore.h"

 /* Create a backing store file for an AddrSpace */
BackingStore::BackingStore(AddrSpace *as) {

}

/* Write the virtual page referenced by PTE to the backing store
   Example invocation: PageOut(&machine->pagetTable[virtualPage] or
   PageOut(&space->pageTable[virtualPage])
*/
void BackingStore::PageOut(TranslationEntry *pte) {

}

/* Read the virtual page referenced by PTE from the backing store */
void BackingStore::PageIn(TranslationEntry *pte) {

}
