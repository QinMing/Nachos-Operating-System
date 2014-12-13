#include "backingstore.h"
#include "system.h"


 /* Store file name for an AddrSpace backing store */
BackingStore::BackingStore(AddrSpace *as, int nPages, int pid) {
  // set file name to PID
  bsFileName = new char[10];
  sprintf(bsFileName, "Mem%d", pid);
  //printf("+++++++++++%s++++++++++++++",str);

  bsFile = NULL;
  
  this->numPages = nPages;
  valid = new bool[nPages];
  for (int i=0;i<nPages;i++){
	valid[i]=FALSE;
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
  
  if (bsFile==NULL){
	  init();
  }

  // virtual page n in the address space will be stored at n * PageSize in the file
  int offset = pte->virtualPage * PageSize;
  int physAddr = pte->physicalPage * PageSize;


  bsFile->WriteAt(&machine->mainMemory[physAddr], PageSize, offset);
  
// 	// Read from memory belonging to pte, and store it in a buffer
// 	char buffer[PageSize];
// 	for (int i = 0; i < PageSize; i++) {
// 		buffer[i] = (char)machine->mainMemory[physAddr + i]; 
// 	}
// 	bsFile->WriteAt(buffer, PageSize, offset);
  
  stats->numPageOuts++;
  
  //this is the non-stub version
  //bsFile->Seek(offset);
  //bsFile->Write(buffer, PageSize);  
  
  valid[pte->virtualPage]=TRUE;
}

/* Read the virtual page referenced by PTE from the backing store */
int BackingStore::PageIn(TranslationEntry *pte) {
	/*
  int pageInFile;

  // Open file in the init()
  //bsFile = fileSystem->Open(bsFileName);

  // Search for page
  int offset = pte->virtualPage * PageSize;
  bsFile->Seek(offset);
  
  // Read page into buffer
  pageInFile = bsFile->Read(buffer, PageSize);


  */
	if (valid[pte->virtualPage]){
		int offset = pte->virtualPage * PageSize;
		int physAddr = pte->physicalPage * PageSize;
		
		bsFile->ReadAt(&machine->mainMemory[physAddr], PageSize, offset);
		
// 		char buffer[PageSize];
// 		bsFile->ReadAt(buffer, PageSize, offset);
// 		// Write buffer to memory
// 		for (int i = 0; i < PageSize; i++) {
// 			machine->mainMemory[physAddr + i] = (char)buffer[i];
// 		}
		
		stats->numPageIns++;
		return 0;
	}else{
		return -1;
	}	
}
