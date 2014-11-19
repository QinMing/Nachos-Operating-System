#ifndef PROCESS_H
#define PROCESS_H

#include "copyright.h"
#include "system.h"

class Process {

public:
	//Public var
	int numThread;
	Thread* mainThread;
	Process(char* newname);//maybe never used. will create a thread
	Process(char* newname,Thread* t);//initialize with a existing thread
	~Process();
	void Join();
	void Load(char *filename);//more args to be added
	void Finish();
	void SetId(SpaceId i){
		id = i;
		mainThread->processId = i;
	}
	SpaceId GetId(){
		return id;
	}

	

private:
	SpaceId id;
	char* name;
};

#endif