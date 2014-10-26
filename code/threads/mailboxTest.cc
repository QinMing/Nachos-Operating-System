#include "mailboxTest.h"
Mailbox* mailbox;
void s(int message){
	
	mailbox->Send(message);
	printf("send   =%d\n",message);
}
void r(int which){
	int message;
	mailbox->Receive(&message);
	printf("receive[%d]=%d\n",which,message);
}
void MailboxTest::start(){
	DEBUG('t', "Entering mailboxTest");
	Thread* t;
	mailbox = new Mailbox("test");//we can add more mailboxes
	for (int i=0;i<1;i++){
		t = new Thread("receiver");
		t->Fork(s,0);
		t = new Thread("sender");
		t->Fork(s,0);
		//change the order of receive and send, and test
	}
}