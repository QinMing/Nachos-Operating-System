
#include "copyright.h"
#include "table.h"
	
	
/* Create a table to hold at most "size" entries. */
Table::Table(int size){
	tabsize = size;
	tab = new void*[size];
	inUse = new bool[size];
	for (int i=0;i<size;i++){
		tab[i] = NULL;
		inUse[i] = false;
	}
	lock = new Lock("TableLock");
}
Table::~Table(){
	
//	for (int i=1;i<tabsize;i++){
//		delete (Process *)tab[i];
//	}
	//assume the object has been deleted outside
	delete[] tab;
	delete lock;
}

/* Allocate a table slot for "object", returning the "index" of the
allocated entry; otherwise, return -1 if no free slots are available. */
int Table::Alloc(void *object){
	lock->Acquire();
	//Notice the table starts from 1 because no in Nachos, no process has SpaceId 0. 
	for (int i=1;i<tabsize;i++){
		if ( inUse[i]==false){
			inUse[i]=true;
			tab[i]=object;
			lock->Release();
			return i;
		}
	}
	printf("process table is full\n");
	lock->Release();
	return -1;
}

/* Retrieve the object from table slot at "index", or NULL if that
slot has not been allocated. */
void* Table::Get(int index){
	lock->Acquire();
	//Notice the table starts from 1 because no in Nachos, no process has SpaceId 0. 
	ASSERT(index>0 && index < tabsize);
	if (inUse[index]==false){
		lock->Release();
		return NULL;
	}else{
		lock->Release();
		return tab[index];
	}
}

/* Free the table slot at index. */
//assuming the object being pointed has already been deleted.
void Table::Release(int index){
	lock->Acquire();
	
	//Notice the table starts from 1 because no in Nachos, no process has SpaceId 0. 
	ASSERT(index>0 && index < tabsize);
	ASSERT(inUse[index]);//panic if the slot is not allocated
	inUse[index]=false;
	//Assume the object has been deleted outside
	
	lock->Release();
}