 // synch.cc
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
	name = debugName;
	value = initialValue;
	queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
	delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
	Semaphore::P()
{
	IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts

	while (value == 0) { 			// semaphore not available
		queue->SortedInsert((void *)currentThread, -currentThread->getPriority());	// so go to sleep
		currentThread->Sleep();
	}
	value--; 					// semaphore available,
	// consume its value

	(void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
	Semaphore::V()
{
	Thread *thread;
	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	thread = (Thread *)queue->Remove();
	if (thread != NULL)	   // make thread ready, consuming the V immediately
		scheduler->ReadyToRun(thread);
	value++;
	(void) interrupt->SetLevel(oldLevel);
}


//----------------------------------------------------------------------
// Lock Implementation
//----------------------------------------------------------------------

Lock::Lock(char* debugName) {
	name = debugName;
	held = 0;
	thread = NULL;
	queue = new List();
}

Lock::~Lock() {
	ASSERT(held==0); 
	delete queue;
}

void Lock::Acquire() {

	ASSERT(!isHeldByCurrentThread());//Error: Attempt to acquire the same lock twice

	// disable interrupts
	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	while (held) {
		if (currentThread->getPriority() > thread->getPriority() ){
			//if the priority is larger than lock holder's
            currentThread->dependThread = thread;
			thread -> promotePriority( currentThread->getPriority() );
		}
		queue->SortedInsert((void*)currentThread,-currentThread->getPriority());
		currentThread->Sleep();
	}
    currentThread->dependThread = NULL;
	thread = currentThread;  
	held = 1;
	holderPriority = thread->getPriority();

	// enable interrupts
	(void) interrupt->SetLevel(oldLevel);
}

void Lock::Release() {
	// disable interrupts
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	ASSERT(isHeldByCurrentThread()); // panic if lock is held by another thread

	// release the lock
	held = 0;

	//Bring its briority back
	currentThread->setPriority(holderPriority);
	holderPriority = 0;
	thread = NULL;
	Thread* t = (Thread*)queue->Remove();
	
	if (t != NULL) {
	  // allow next thread to run
	  scheduler->ReadyToRun(t);
	}

	// enable interrupts
	(void) interrupt->SetLevel(oldLevel);
}

bool Lock::isHeldByCurrentThread() {
	return thread == currentThread;
}


//----------------------------------------------------------------------
// Condition Implementation
//----------------------------------------------------------------------

Condition::Condition(char* debugName) {
	name = debugName;
	queue = new List();
}

Condition::~Condition() {
	ASSERT(queue->IsEmpty());
	delete queue;
}

void Condition::Wait(Lock* conditionLock) {

	// disable interrupts
	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	// lock must be held by currentThread
	ASSERT(conditionLock->isHeldByCurrentThread());

	conditionLock->Release();

	// place thread in waitig queue and sleep it
	queue->SortedInsert((void*)currentThread,-currentThread->getPriority());
	currentThread->Sleep();

	conditionLock->Acquire();

	// enable interrupts
	(void) interrupt->SetLevel(oldLevel);
}

void Condition::Signal(Lock* conditionLock) {
	// disable interrupts
	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	// lock must be held by currentThread
	ASSERT(conditionLock->isHeldByCurrentThread());

	// remove thread from waiting queue and let it run
	Thread* t = (Thread*)queue->Remove();
	if (t != NULL) {
		scheduler->ReadyToRun(t);
	}

	// enable interrupts
	(void) interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast(Lock* conditionLock) {
	// disable interrupts
	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	// lock must be held by currentThread
	ASSERT(conditionLock->isHeldByCurrentThread());

	// remove all threads from waiting queue and let them run
	Thread* t = (Thread*)queue->Remove();
	while (t != NULL) {
		scheduler->ReadyToRun(t);
		t = (Thread*)queue->Remove();
	}

	// enable interrupts
	(void) interrupt->SetLevel(oldLevel);
}
