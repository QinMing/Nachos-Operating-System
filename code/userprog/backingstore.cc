#include "backingstore.h"
#include "system.h"

 /* Store file name for an AddrSpace backing store */
BackingStore::BackingStore(AddrSpace *as, int nPages, int pid) {
  // set file name to PID
  bsFileName = new char[10];
  sprintf(bsFileName, "Mem%d", pid);
  //printf("+++++++++++%s++++++++++++++",str);

  this->numPages = nPages;
  valid = new bool[nPages];
  for (int i=0;i<nPages;i++){
	  valid[i]=false;
  }
}

/* Actually create the backing store file (on first evict) */
void
BackingStore::init() {
    fileSystem->Create(bsFileName, numPages * PageSize);
	bsFile = fileSystem->Open(bsFileName);
}

BackingStore::~BackingStore() {
  // delete file
  fileSystem->Remove(bsFileName);
  // delete file pointer
  delete bsFile;
  delete[] bsFileName;
  delete[] valid;
}
/* Write the virtual page referenced by PTE to the backing store
   Example invocation: PageOut(&machine->pagetTable[virtualPage] or
   PageOut(&space->pageTable[virtualPage])
*/
void BackingStore::PageOut(TranslationEntry *pte) {
  //char buffer[PageSize];
  
  // Read from memory belonging to pte, and store it in a buffer
  //for (int i = 0; i < PageSize; i++) {
  //  buffer[i] = (char)machine->mainMemory[pte->physicalPage * PageSize + i]; 
  //}

  // virtual page n in the address space will be stored at n * PageSize in the file
  int offset = pte->virtualPage * PageSize;
  int physAddr = pte->physicalPage * PageSize;

  // write buffer to backing store file
  bsFile->WriteAt(&machine->mainMemory[physAddr], PageSize, offset);
  
  //this is the non-stub version
  //bsFile->Seek(offset);
  //bsFile->Write(buffer, PageSize);  
  
  valid[pte->virtualPage]=true;
}

/* Read the virtual page referenced by PTE from the backing store */
int BackingStore::PageIn(TranslationEntry *pte) {
	/*
  int pageInFile;
  char buffer[PageSize];

  // Open file in the init()
  //bsFile = fileSystem->Open(bsFileName);

  // Search for page
  int offset = pte->virtualPage * PageSize;
  bsFile->Seek(offset);
  
  // Read page into buffer
  pageInFile = bsFile->Read(buffer, PageSize);

  // Write buffer to memory
  for (int i = 0; i < PageSize; i++) {
    machine->mainMemory[pte->physicalPage * PageSize + i] = (char)buffer[i];
  }  
  */
	if (valid[pte->virtualPage]){
		int offset = pte->virtualPage * PageSize;
		int physAddr = pte->physicalPage * PageSize;
		bsFile->ReadAt(&machine->mainMemory[physAddr], PageSize, offset);
		return 0;
	}else{
		return -1;
	}	
}
