#include "pipe.h"

Pipe::Pipe() {
	read = new Semaphore("can read", 0);
	write = new Semaphore("can write", BufferSize);
	bptr = 0;
	in = out = 0;
}
Pipe::~Pipe() {
	delete read;
	delete write;
}


void Pipe::Write(char data) {
	
}

char Pipe::Read() {

}