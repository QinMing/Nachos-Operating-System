#include "mailbox.h"

Mailbox::Mailbox(char* debugName){
	name = debugName;
	lock = new Lock("mailboxLock");
	canSend = new Condition("canSend");
	canReceive = new Condition("canReceive");;
	full=false;
}
Mailbox::~Mailbox(){
	ASSERT(lock->isAcquired() == 0);//make sure it's not in use
	DEBUG('t', "Deleting mailbox \"%s\"\n", name);
	delete lock;
	delete canSend;
	delete canReceive;
}

void Mailbox::Send(char* word){
	lock->Acquire();

	if (full) canSend->Wait(lock);
	buff=word;
	full=true;
	canReceive->Signal(lock);

	lock->Release();
}

char* Mailbox::Receive(){
	lock->Acquire();

	canReceive->Wait(lock);
	char* message=buff;
	full = false;
	canSend->Signal(lock);

	lock->Release();
	return message;
}

void Mailbox::Send(int message) {
  // acquire the lock
  lock->Acquire();

  // wait for empty buffer
  while (ifull > 0) {
    canSend->Wait(lock);
  }

  // store message in buffer
  ibuff = message;
  ifull++;
  
  // signal receiver
  canRecieve->Signal(lock);
  
  // release the lock
  lock->Release();
}

void Mailbox::Receive(int* message) {
  // acquire the lock
  lock->Acquire();
  
  // wait for messages
  while (ifull == 0) {
    canReceive->Wait(lock);
  }

  // point to the message stored in the buffer
  message = ibuff;
  ifull--;

  // signal sender
  canSend->Signal(lock);
  
  // release the lock
  lock->Release();
}
