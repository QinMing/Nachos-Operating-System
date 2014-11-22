#ifndef PROCESS_H
#define PROCESS_H

#include "copyright.h"
#include "system.h"

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

	

private:
	SpaceId pid;
	char* name;
	bool willBeJoined;
};

#endif