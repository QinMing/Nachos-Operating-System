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
	void Write(char data);
	char Read();

	//public var
	SpaceId in, out;


private:
	Semaphore* read;
	Semaphore* write;
	char buffer[BufferSize];
	int bptr;
};

#endif// PIPE_H