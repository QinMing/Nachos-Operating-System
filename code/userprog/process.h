#ifndef PROCESS_H
#define PROCESS_H

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "pipe.h"
#include "synch.h"

#define MaxPipes 64

class Process {

public:
	
	Process(char* newname,bool willJoin);
	~Process();
	int Join();
	int  Load(Thread* firstThread, char *filename, int argc, char **argv);
	void Finish();
	void SetId(SpaceId i) { pid = i; }
	SpaceId GetId() { return pid; }
	char* GetName() { return name; }

	//Public var
	int  numThread;

	//pipeline functions 
	int PipelineAdd(Process* proc, bool hasin, bool hasout);
	
	//pipeline var
	Pipe *pipeIn;	//pointers to pipes
	Pipe *pipeOut;	//If they are NULL, then their OpenFileIds 0 (stdin)
		// and 1 (stdout) are binded to console by default

	//Join var
	int exitStatus;

	//functions for user level threads
	int AddThread(Thread* t);


private:
	SpaceId pid;
	char* name;
	
	bool willBeJoined;
	bool hasJoined;
	Condition* joinedOnMe;
	Lock* lock;

	List *pipeline;

	AddrSpace* space;
	//List *userThreads;
	
};

#endif