#include "pipe.h"

Pipe::Pipe() {
	read = new Semaphore("can read", 0);
	write = new Semaphore("can write", BufferSize);
	pread = pwrite = 0;
	in = out = 0;
}
Pipe::~Pipe() {
	delete read;
	delete write;
}

//Assume only one thread will access this function 
//so lock (mutex) is not needed
char Pipe::GetChar() {
	read->P();
	char res = buffer[pread];
	pread = ( pread + 1 ) % BufferSize;
	write->V();
	return res;
}

void Pipe::PutChar(char data) {
	write->P();
	buffer[pwrite] = data;
	pwrite = ( pwrite + 1 ) % BufferSize;
	read->V();
}

