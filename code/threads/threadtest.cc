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
//#include "cTest1.h"

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
    printf("LTD3\n");
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

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

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
	//case 3:
	//	cTest1 test;
	//	test.run();
	//	break;
	default:

		printf("No test specified.\n");

		break;
	}
}

