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

void Mailbox::Send(int message) {
  // acquire the lock
  lock->Acquire();

  // wait for empty buffer
  while (full) {
    canSend->Wait(lock);
  }

  // store message in buffer
  ibuff = message;
  full = true;
  
  // signal receiver
  canReceive->Signal(lock);
  
  //waiting for a receiver to complete
  finish->Wait(lock);
  
  // signal next sender
  canSend->Signal(lock);
  
  // release the lock
  lock->Release();
}

void Mailbox::Receive(int* message) {
  // acquire the lock
  lock->Acquire();
  
  // wait for messages
  while (!full) {
    canReceive->Wait(lock);
  }

  // point to the message stored in the buffer
  (*message)=ibuff;
  full = false;
  
  //signal the sender
  finish->Signal(lock);
  
  // release the lock
  lock->Release();
}
