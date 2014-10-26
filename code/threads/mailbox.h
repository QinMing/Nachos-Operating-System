#ifndef MAILBOX_H
#define MAILBOX_H

#include "copyright.h"
#include "synch.h"
#include "list.h"

class Mailbox {
public:
	Mailbox(char* debugName);
	~Mailbox();
	char* getName(){
		return name;
	}
	void Send(int message);
	void Receive(int* message);

private:
	char* name;
	Lock* lock;
	Condition* canSend;
	Condition* canReceive;
	Condition* finish;
	int ibuff;
	bool full;
};

#endif //MAILBOX_H
