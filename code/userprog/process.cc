#include "copyright.h"
#include "process.h"

Process::Process(){
	// threadList = new Thread*[];
	numThread = 1;
}

Process::~Process(){
	ASSERT(numThread == 1);
	// delete[] threadList;
}

void AddThread(){
	numThread++;
}

void RmThread(){
	numThread--;
}