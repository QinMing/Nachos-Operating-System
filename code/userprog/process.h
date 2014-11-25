#ifndef PROCESS_H
#define PROCESS_H

#include "copyright.h"
#include "system.h"
#include "pipe.h"

#define MaxPipes 64

class Process {

public:
	//Public var
	int  numThread;
	Thread* mainThread;
	Process(char* newname,bool willJoin);
	Process(char* newname,bool willJoin,Thread* t);//initialize with a existing thread
	~Process();
	void Join();
	int  Load(char *filename,int argc, char **argv);
	void Finish();
	void SetId(SpaceId i){
		pid = i;
		mainThread->processId = i;
	}
	SpaceId GetId(){
		return pid;
	}
	char* GetName(){
		return name;
	}

	//pipeline functions 
	int PipelineAdd(bool hasin, bool hasout);
	
	//pipeline var
	Pipe *pipeIn;	//pointers to pipes
	Pipe *pipeOut;	//If they are NULL, then their OpenFileIds 0 (stdin)
		// and 1 (stdout) are binded to console by default

private:
	SpaceId pid;
	char* name;
	bool willBeJoined;
	List *pipeline;
};

#endif