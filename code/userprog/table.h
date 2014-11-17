#ifndef TABLE_H
#define TABLE_H

#include "synch.h"

class Table {
private:
    int size;
    void **table; // internal data structure
    Lock *lock;
public:
    /* Create a table to hold at most "size" entries */
    Table(int size);

    ~Table();

    /* Allocate a table slot for "object", returning the "index" of the 
    allocated entry; otherwise, right -1 if no free slots available. */
    int Alloc(void *object);

    /* Retrieve the object from table slot at "index", or NULL if that
    slot has not been allocated. */
    void *Get(int index);

    /* Free the table slot at index. */
    void Release(int index);
};

// global Table pointer
extern Table *processTable;

#endif // TABLE_H