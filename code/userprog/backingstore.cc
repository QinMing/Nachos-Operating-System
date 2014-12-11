#include "backingstore.h"

 /* Store file name for an AddrSpace backing store */
BackingStore::BackingStore(AddrSpace *as, int numPages, int pid) {
  // set file name to PID
  sprintf(bsFileName, "%d", pid);

  this->numPages = numPages;
}

/* Actually create the backing store file (on first evict) */
BackingStore::init() {
    fileSystem->Create(bsFileName, numPages * PageSize);
}

BackingStore::~BackingStore() {
  // delete file
  fileSystem->Remove(bsFileName);
  // delete file pointer
  delete bsFile;
}
/* Write the virtual page referenced by PTE to the backing store
   Example invocation: PageOut(&machine->pagetTable[virtualPage] or
   PageOut(&space->pageTable[virtualPage])
*/
void BackingStore::PageOut(TranslationEntry *pte) {
  char buffer[PageSize];
  bsFile = fileSystem->Open(bsFileName);

  // Read from memory belonging to pte, and store it in a buffer
  for (int i = 0; i < PageSize; i++) {
    buffer[i] = (char)machine->mainMemory[pte->physicalPage * PageSize + i]; 
  }

  // virtual page n in the address space will be stored at n * PageSize in the file
  int offset = pte->virtualPage * PageSize;

  // write buffer to backing store file
  bsFile->Seek(offset);
  bsFile->Write(buffer, PageSize);
}

/* Read the virtual page referenced by PTE from the backing store */
int BackingStore::PageIn(TranslationEntry *pte) {
  int pageInFile;
  char buffer[PageSize];

  // Open file
  bsFile = fileSystem->Open(bsFileName);

  // Search for page
  int offset = pte->virtualPage * PageSize;
  bsFile->Seek(offset);
  
  // Read page into buffer
  pageInFile = bsFile->Read(buffer, PageSize);

  // Write buffer to memory
  for (int i = 0; i < PageSize; i++) {
    machine->mainMemory[pte->physicalPage * PageSize + i] = (int)buffer[i];
  }  
}
