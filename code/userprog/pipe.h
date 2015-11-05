#ifndef PIPE_H
#define PIPE_H

#include "synch.h"
typedef int SpaceId;
#define BufferSize 64

class Pipe {
public:
	Pipe();
	~Pipe();

	//check PID before calling R/W
	char GetChar();
	void PutChar(char data);
	void Read(char *buffer, int size);
	void Write(char *buffer, int size);
			
	//public var
	SpaceId in, out;


private:
	Semaphore* read;
	Semaphore* write;
	char buffer[BufferSize];
	int pread, pwrite;
};

#endif// PIPE_H