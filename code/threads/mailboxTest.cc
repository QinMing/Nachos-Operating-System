#include "mailboxTest.h"
Mailbox* mailbox;
void s(int message){
	printf("[begin]<<           [%d] \n",message);
	mailbox->Send(message);
	printf("[ end ]<<<<<<       [%d]sended=%d\n",message,message);
}
void r(int which){
	printf("[begin]           >>[%d]\n",which);
	int message;
	mailbox->Receive(&message);
	printf("[ end ]       >>>>>>[%d]received=%d\n",which,message);
}
void MailboxTest::start(int testnum){
	DEBUG('b', "Entering mailboxTest");
	Thread* t;
	mailbox = new Mailbox("test");
	switch (testnum){
	case 0://sender first
		t = new Thread("thread");
		t->Fork(s,1);
		t = new Thread("thread");
		t->Fork(s,2);
		t = new Thread("thread");
		t->Fork(s,3);
		t = new Thread("thread");
		t->Fork(s,4);
		t = new Thread("thread");
		t->Fork(s,5);

		t = new Thread("thread");
		t->Fork(r,1);
		t = new Thread("thread");
		t->Fork(r,2);
		t = new Thread("thread");
		t->Fork(r,3);
		t = new Thread("thread");
		t->Fork(r,4);
		t = new Thread("thread");
		t->Fork(r,5);
		break;
	case 1://receiver first
		t = new Thread("thread");
		t->Fork(r,1);
		t = new Thread("thread");
		t->Fork(r,2);
		t = new Thread("thread");
		t->Fork(r,3);
		t = new Thread("thread");
		t->Fork(r,4);
		t = new Thread("thread");
		t->Fork(r,5);
		
		t = new Thread("thread");
		t->Fork(s,1);
		t = new Thread("thread");
		t->Fork(s,2);
		t = new Thread("thread");
		t->Fork(s,3);
		t = new Thread("thread");
		t->Fork(s,4);
		t = new Thread("thread");
		t->Fork(s,5);
		break;
	case 2://only sender
		t = new Thread("thread");
		t->Fork(s,1);
		t = new Thread("thread");
		t->Fork(s,2);
		t = new Thread("thread");
		t->Fork(s,3);
		break;
	case 3://only receiver
		t = new Thread("thread");
		t->Fork(r,1);
		t = new Thread("thread");
		t->Fork(r,2);
		t = new Thread("thread");
		t->Fork(r,3);
		break;
	case 4://interleaving
		for (int i=0;i<5;i++){
			t = new Thread("1");
			t->Fork(s,i);
			t = new Thread("2");
			t->Fork(r,i);
			//change the order of receive and send, and test
			//add a time delay between s and r
		}
		break;
	}
	
}