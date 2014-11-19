#ifndef PROCESS_H
#define PROCESS_H

#include "copyright.h"
#include "system.h"

class Process {
public:
	Process();
	~Process();
	void Join();
	void Finish();
	void AddThread();
	void RmThread();
	AddrSpace* space;
	SpaceId id;
	int numThread;
	// Thread** threadList;
}

#endif