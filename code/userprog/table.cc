#include "table.h"

Table::Table(int s) : size(s) {
    table = new void *[s];
    lock = new Lock("Table Lock");
}

Table::~Table() {
    if (lock) delete lock;
    if (table) delete table;
}

/* Allocate a table slot for "object", returning the "index" of the 
allocated entry; otherwise, return -1 if no free slots available. */
int Table::Alloc(void *object) {
    lock->Acquire();
    int index = -1;

    // search for empty table slot and allocate it
    for (int i = 0; i < size; ++i) {
        if (table[i] == NULL) {
            table[i] = object;
            index = i;
            break;
        }
    }

    lock->Release();
    // return the index allocated
    return index;
}

/* Retrieve the object from table slot at "index", or NULL if that
slot has not been allocated. */
void *Table::Get(int index) {
    lock->Acquire();

    void *process = NULL;

    if (index >= 0 && index < size) {
        process = table[index];
    }

    lock->Release();
    return process;
}

/* Free the table slot at index. */
void Table::Release(int index) {
    lock->Acquire();

    if (index >= 0 && index < size) {
        table[index] = NULL; // free table
    }

    lock->Release();
}