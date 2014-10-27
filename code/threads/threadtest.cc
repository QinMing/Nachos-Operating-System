// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "mailbox.h"
#include "mailboxTest.h"
#include "whale.h"
#include "whaleTest.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// LockTest1
//----------------------------------------------------------------------

Lock *locktest1 = NULL;

void
LockThread1(int param)
{
    printf("L1:0\n");
    locktest1->Acquire();
    printf("L1:1\n");
    currentThread->Yield();
    printf("L1:2\n");
    locktest1->Release();
    printf("L1:3\n");
}

void
LockThread2(int param)
{
    printf("L2:0\n");
    locktest1->Acquire();
    printf("L2:1\n");
    currentThread->Yield();
    printf("L2:2\n");
    locktest1->Release();
    printf("L2:3\n");
}

//This is Kevin.
//These are tests for Lock(). Uncomment these if you want to see if 
//they make the program abort(which it should)
void
LockThread3(int param){
  //printf("testing: releasing lock that isn't held\n");
  //locktest1->Release();
}

void
LockThread4(int param){
 // printf("testing: deleting a lock that is held\n");
  //locktest1->~Lock();

}

void
LockThreadDelete1(int param){
    locktest1->Acquire();
    printf("LTD1\n");
    locktest1->~Lock();
}

void
LockThreadDelete2(int param){
    locktest1->Acquire();
    printf("LTD2\n");
    locktest1->Release();
}

void
LockThreadDelete3(int param){
    locktest1->Acquire();
    printf("L3:1\n");
    //locktest1->Release();//uncomment if want to test it CAN delete a lock when no one's holding
    printf("L3:2\n");
    locktest1->~Lock();
    printf("successfully deleted\n");
}

void
LockTest1()
{
    DEBUG('t', "Entering LockTest1");

    locktest1 = new Lock("LockTest1");

    Thread *t = new Thread("one");
    t->Fork(LockThread1, 0);
    t = new Thread("two");
    t->Fork(LockThread2, 0);
    t = new Thread("three");
    t->Fork(LockThread3, 0);
    t = new Thread("four");
    t->Fork(LockThread4, 0);
}

//tests a thread deleting its own held lock before releasing
void
LockTestDelete1()
{
    DEBUG('t', "Entering LockTestDelete");

    locktest1 = new Lock("LockTestDelete1");

    Thread *t = new Thread("one");
    t->Fork(LockThreadDelete1, 0);
    t = new Thread("two");
    t->Fork(LockThreadDelete2, 0);
}


//tests a thread holding a lock, and another thread trying to delete that lock
void
LockTestDelete2()
{
    DEBUG('t', "Entering LockTestDelete2");

    locktest1 = new Lock("LockTestDelete1");

    Thread *t = new Thread("one");
    t->Fork(LockThreadDelete2, 0);
    t = new Thread("two");
    t->Fork(LockThreadDelete3, 0);
}

//----------------------------------------------------------------------
// Condition tests
//----------------------------------------------------------------------

Condition *Conditiontest1 = NULL;

//waiting on a condition variable without holding a Lock
//then system abort
//-q 5
void
WaitCvWithoutHold1(int param)
{
    printf("WCWH1\n");
    Conditiontest1->Wait(locktest1);
}

void WaitCvWithoutHold()
{
   DEBUG('t', "WaitCvWithoutHold");

   locktest1 = new Lock("WaitCvWithoutHoldL");
   Conditiontest1 = new Condition("WaitCvWithoutHoldC");
   Thread *t = new Thread("one");
   t->Fork(WaitCvWithoutHold1, 0);
}

//signaling a condition variable wakes only one thread and broadcasting wakes up all threads
//-q 6

void
SignalBroadcast1(int param)
{
    printf("C1:0\n");
    locktest1->Acquire();
    printf("C1:1\n");
    Conditiontest1->Wait(locktest1);
    printf("C1:2\n");
    Conditiontest1->Broadcast(locktest1);
    printf("C1:3\n");
    Conditiontest1->Wait(locktest1);
    printf("C1:4\n");
    
}

void
SignalBroadcast2(int param)
{
    printf("C2:0\n");
    locktest1->Acquire();
    printf("C2:1\n");
    Conditiontest1->Wait(locktest1);
    printf("C2:2\n");
    Conditiontest1->Wait(locktest1);
    printf("C2:3\n");
}

void
SignalBroadcast3(int param)
{
    printf("C3:0\n");
    locktest1->Acquire();
    printf("C3:1\n");
    Conditiontest1->Wait(locktest1);
    printf("C3:2\n");
    Conditiontest1->Wait(locktest1);
    printf("C2:3\n");
}

void
SignalBroadcast4(int param)
{
    printf("C4:0\n");
    locktest1->Acquire();
    printf("C4:1\n");
    Conditiontest1->Signal(locktest1);
    printf("C4:2\n");
    Conditiontest1->Wait(locktest1);
    printf("C4:3\n");
    Conditiontest1->Wait(locktest1);
    printf("C2:3\n");
}

void SignalBroadcast()
{
   DEBUG('t', "SignalBroadcast");

   locktest1 = new Lock("SignalBroadcastL");
   Conditiontest1 = new Condition("SignalBroadcastC");
   Thread *t = new Thread("one");
   t->Fork(SignalBroadcast1, 0);
   t = new Thread("two");
   t->Fork(SignalBroadcast2, 0);
   t = new Thread("three");
   t->Fork(SignalBroadcast3, 0);
   t = new Thread("four");
   t->Fork(SignalBroadcast4, 0);
}

//signaling and broadcasting to a condition variable with no waiters is a no-op, and future threads that wait will block
//-q 7

void SignalBroadcast11(int param)
{
    printf("C1:0\n");
    locktest1->Acquire();
    printf("C1:1\n");
    Conditiontest1->Signal(locktest1);
    printf("C1:3\n");
    Conditiontest1->Broadcast(locktest1);
    printf("C1:4\n");
    Conditiontest1->Wait(locktest1);
    printf("C1:5\n");
}

void SignalBroadcast22(int param)
{
    printf("C2:0\n");
    locktest1->Acquire();
    printf("C2:1\n");
    Conditiontest1->Wait(locktest1);
    printf("C2:2\n");
}

void SignalBroadcast2()
{
   DEBUG('t', "SignalBroadcast");

   locktest1 = new Lock("SignalBroadcastL");
   Conditiontest1 = new Condition("SignalBroadcastC");
   Thread *t = new Thread("one");
   t->Fork(SignalBroadcast11, 0);
   t = new Thread("two");
   t->Fork(SignalBroadcast22, 0);
}

//Conditon Variable Deleting tests
//-q 8

void ConditonDelete1(int param)
{
    
    
    printf("C1:0\n");
    locktest1->Acquire();
    //Conditiontest1->~Condition();      //uncomment to see it can delete when no threads are on queue
    //printf("Conditiontest1 deleted\n");//uncomment to see it can delete when no threads are on queue
    printf("C1:1\n");
    Conditiontest1->Wait(locktest1);
    printf("C1:2\n");
    locktest1->Release();
    printf("C1:3\n");
    
}

void ConditonDelete2(int param)
{
    
    printf("C2:0\n");
    locktest1->Acquire();
    printf("C2:1\n");
    Conditiontest1->~Condition(); //deleting a lock while there are threads on queue
    printf("C2:2\n");
  
}

void ConditonDelete()
{
   DEBUG('t', "SignalBroadcast");

   locktest1 = new Lock("ConditonDeleteL");
   Conditiontest1 = new Condition("ConditonDeleteC");
   Thread *t = new Thread("one");
   t->Fork(ConditonDelete1, 0);
   t = new Thread("two");
   t->Fork(ConditonDelete2, 0);
}

void
priorityThread1(int param){
    locktest1->Acquire();
    printf("in priorityThread1\n");
    locktest1->Release();
}

void
priorityThread2(int param){
    locktest1->Acquire();
    printf("in priorityThread2\n");
    locktest1->Release();
}

void
priorityThread3(int param){
    locktest1->Acquire();
    printf("in priorityThread3\n");
    locktest1->Release();
}

void
priorityTest(){
    DEBUG('t', "entering priorityTest()\n");
    locktest1 = new Lock("priorityLock");
    Thread* t = new Thread("one");
    Thread* s = new Thread("two");
    Thread* x = new Thread("three");

    t->setPriority(3);
    printf("priority of thread1: %d\n", t->getPriority());
    t->Fork(priorityThread1, 0);
    s->setPriority(4);
    printf("priority of thread2: %d\n", s->getPriority());
    s->Fork(priorityThread2,0);
    x->setPriority(1);
    printf("priority of thread3: %d\n", x->getPriority());
    x->Fork(priorityThread3, 0);
}

//--------------------------------------
// Thread::Join() Tests
// (1) a thread that will be joined is only destroyed once
//     Join has been called on it
// (2) if a parent calls Join on a child and the child is still
//     executing, the parent waits
// (3) if a parent calls Join on a child and the child has
//     finished executing, the parent does not block
// (4) a thread does not call Join on itself
// (5) Join is only invoked on threads created to be joined
// (6) Join is only called on a thread that has forked
// (7) Join is not called more than once on a thread (if it is,
//     then this could easily lead to a segmentation fault
//     because the child is likely deleted)
//--------------------------------------
 
void child() {
 Thread* childThread = new Thread("Child", 1); 
 locktest1->Acquire();
  printf("in child thread");
  childThread->Join();
  locktest1->Release();
  

}
void JoinTest1() {
  Thread* parent = new Thread("Parent", 1);

  locktest1 = new Lock("Lock");
  
  
  parent->Fork((VoidFunctionPtr)child, 0);
  
  
}


void
Joiner(Thread *joinee)
{
  currentThread->Yield();
  currentThread->Yield();

  printf("Waiting for the Joinee to finish executing.\n");

  currentThread->Yield();
  currentThread->Yield();

  // Note that, in this program, the "joinee" has not finished
  // when the "joiner" calls Join().  You will also need to handle
  // and test for the case when the "joinee" _has_ finished when
  // the "joiner" calls Join().

  joinee->Join();

  currentThread->Yield();
  currentThread->Yield();

  printf("Joinee has finished executing, we can continue.\n");

  currentThread->Yield();
  currentThread->Yield();
}

void
Joinee()
{
  int i;

  for (i = 0; i < 5; i++) {
    printf("Smell the roses.\n");
    currentThread->Yield();
  }

  currentThread->Yield();
  printf("Done smelling the roses!\n");
  currentThread->Yield();
  
}

void
ForkerThread()
{
  Thread *joiner = new Thread("joiner", 0);  // will not be joined
  Thread *joinee = new Thread("joinee", 1);  // WILL be joined

  // fork off the two threads and let them do their business
  joiner->Fork((VoidFunctionPtr) Joiner, (int) joinee);
  joinee->Fork((VoidFunctionPtr) Joinee, 0);

  // this thread is done and can go on its merry way
  printf("Forked off the joiner and joiner threads.\n");
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//-----------------------------------------------------------------------
void
ThreadTest()
{
	switch (testnum) {
	case 1:
		ThreadTest1();
		break;
	case 2:
		LockTest1();
		break;
    case 3:
        LockTestDelete1();
        break;
    case 4:
        LockTestDelete2();
        break;
    case 5:
        WaitCvWithoutHold();
        break;
    case 6:
        SignalBroadcast();
        break;
    case 7:
        SignalBroadcast2();
        break;

    case 8:
        ConditonDelete();
        break;

	case 9:case 10:case 11:case 12: case 13:{
		MailboxTest mailboxTest;
		mailboxTest.start(testnum-9);
		break;
	}


	case 14:
	  JoinTest1();
	  break;
	case 15:
	  ForkerThread();
	  break;
	case 19:
        priorityTest();
        break;
	case 20:{
		WhaleTest whaleTest;
		whaleTest.start();
	break;
      }
	
    default:
		printf("No test specified.\n");
		break;
	}
}

