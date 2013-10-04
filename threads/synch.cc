#ifndef CHANGED
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

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new(std::nothrow) List;
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
	queue->Append((void *)currentThread);	// so go to sleep
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

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(const char* debugName) {}
Lock::~Lock() {}
void Lock::Acquire() {}
void Lock::Release() {}

Condition::Condition(const char* debugName) { }
Condition::~Condition() { }
void Condition::Wait(Lock* conditionLock) { ASSERT(false); }
void Condition::Signal(Lock* conditionLock) { }
void Condition::Broadcast(Lock* conditionLock) { }
#else
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// synch.cc 
//  Routines for synchronizing threads.  Three kinds of
//  synchronization routines are defined here: semaphores, locks 
//      and condition variables (the implementation of the last two
//  are left to the reader).
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
#include <string.h>

//----------------------------------------------------------------------
// Semaphore::Semaphore
//  Initialize a semaphore, so that it can be used for synchronization.
//
//  "debugName" is an arbitrary name, useful for debugging.
//  "initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new(std::nothrow) List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
//  De-allocate semaphore, when no longer needed.  Assume no one
//  is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
//  Wait until semaphore value > 0, then decrement.  Checking the
//  value and decrementing must be done atomically, so we
//  need to disable interrupts before checking the value.
//
//  Note that Thread::Sleep assumes that interrupts are disabled
//  when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);   // disable interrupts
    
    while (value == 0) {            // semaphore not available
    queue->Append((void *)currentThread);   // so go to sleep
    currentThread->Sleep();
    } 
    value--;                    // semaphore available, 
                        // consume its value
    
    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
//  Increment semaphore value, waking up a waiter if necessary.
//  As with P(), this operation must be atomic, so we need to disable
//  interrupts.  Scheduler::ReadyToRun() assumes that threads
//  are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)    // make thread ready, consuming the V immediately
    scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

Lock::Lock(const char* debugName) 
{
    name = debugName;
    freed = 1; // 1 denotes free, 0 denotes busy
    threadName = NULL;
    queue = new(std::nothrow) List; 
}
Lock::~Lock() {
    delete queue;
}
void Lock::Acquire() 
{
    int localVal = 0;
    while (localVal == 0) {            // lock not available
        IntStatus oldLevel = interrupt->SetLevel(IntOff);   // disable interrupts
        localVal = freed;
        freed = 0;      // if the lock was free, it's now held by the current thread and localval = 1.  
                        // if not, freed should remain zero and localval will be zero as well 
        if(localVal == 0){              // lock not available
            queue->Append((void *)currentThread);   // so go to sleep
            currentThread->Sleep();
        }

        (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
    }
    // only one thread can reach this point at a time
    threadName = currentThread->getName();    
}

void Lock::Release() 
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    ASSERT(freed == 0); // make sure the lock is still being held
    ASSERT(this->isHeldByCurrentThread()); // make sure the current thread is holding the lock

    thread = (Thread *)queue->Remove();
    if (thread != NULL)    // wake up the first thread waiting in Acquire() if there is one
        scheduler->ReadyToRun(thread);
    freed = 1; // the lock is free to be acquired by another thread
    threadName = NULL;  // ensures the current thread can't do something with the lock since it's released it

    (void) interrupt->SetLevel(oldLevel);
}

bool Lock::isHeldByCurrentThread()
{
    // checks to be sure that the threadName (set in Acquire after a thread has the lock)
    // is the same as the name of the current thread.  Used for assertions
    if(strcmp(threadName, currentThread->getName()) == 0)
    {
        return true;
    }
    return false;
    
}

Condition::Condition(const char* debugName) 
{ 
    name = debugName;
    queue = new(std::nothrow) List;
}

Condition::~Condition() { }
void Condition::Wait(Lock* conditionLock) 
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);   // disable interrupts
    ASSERT(conditionLock->isHeldByCurrentThread());
    queue->Append((void *)currentThread);  // so go to sleep
    conditionLock->Release();  // relinquishes lock until it has been signaled
    currentThread->Sleep();
    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
    conditionLock->Acquire();  // reacquires the lock   
}

void Condition::Signal(Lock* conditionLock) 
{ 
    IntStatus oldLevel = interrupt->SetLevel(IntOff);   // disable interrupts
    ASSERT(conditionLock->isHeldByCurrentThread());
    Thread *thread;
    thread = (Thread *)queue->Remove();
    if (thread != NULL)    // wakes up the first waiting thread if there is one
        scheduler->ReadyToRun(thread); // Puts thread on ready list, which must reaquire lock
    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
}

void Condition::Broadcast(Lock* conditionLock) 
{ 
    IntStatus oldLevel = interrupt->SetLevel(IntOff);   // disable interrupts
    ASSERT(conditionLock->isHeldByCurrentThread());
    Thread *thread;
    thread = (Thread *)queue->Remove();
    while(thread != NULL)    // wakes up all of the waiting threads if there are any
    {
        scheduler->ReadyToRun(thread);
        thread = (Thread *)queue->Remove();
    }
    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
}

#endif
