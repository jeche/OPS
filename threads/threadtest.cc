#ifndef CHANGED
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
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new(std::nothrow) Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}
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
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new(std::nothrow) Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// SimpleThreadPriority
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.  
//----------------------------------------------------------------------

void
SimpleThreadPriority(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d with looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest for priority threads
// 	Creates two new threads aside from the main thread.  For the first
//	test, the first, fourth and fifth of these threads will be given 
//	the highest priority and the other two will have an equal lower 
//	priority.  The thread with the highest priority should finish first.  
//----------------------------------------------------------------------

void
ThreadTestPriority()
{
    DEBUG('t', "Entering SimpleTestPriority");
    DEBUG('t', "Starting the first priority test");

    Thread *t1 = new(std::nothrow) Thread("forked thread1", 0);
    Thread *t2 = new(std::nothrow) Thread("forked thread2", 1);
    Thread *t3 = new(std::nothrow) Thread("forked thread3", 1);
    Thread *t4 = new(std::nothrow) Thread("forked thread4", 0);
    Thread *t5 = new(std::nothrow) Thread("forked thread5", 0);
    Thread *t6 = new(std::nothrow) Thread("forked thread6", 1);

    t1->Fork(SimpleThreadPriority, 1);
    t2->Fork(SimpleThreadPriority, 2);
    t3->Fork(SimpleThreadPriority, 3);
    t4->Fork(SimpleThreadPriority, 4);
    t5->Fork(SimpleThreadPriority, 5);
    t6->Fork(SimpleThreadPriority, 6);
}
#endif //CHANGED