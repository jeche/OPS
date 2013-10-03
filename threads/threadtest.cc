#ifndef CHANGED
// threadtest.cc 
//  Simple test case for the threads assignment.
//
//  Create two threads, and have them context switch
//  back and forth between themselves by calling Thread::Yield, 
//  to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

//----------------------------------------------------------------------
// SimpleThread
//  Loop 5 times, yielding the CPU to another ready thread 
//  each iteration.
//
//  "which" is simply a number identifying the thread, for debugging
//  purposes.
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
//  Set up a ping-pong between two threads, by forking a thread 
//  to call SimpleThread, and then calling SimpleThread ourselves.
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
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//#########################################################################
//Start ProdCons

//todo: 
#include "copyright.h"
#include "system.h"
#include "synch.h"

//----------------------------------------------------------------------
// Producer Consumer Example
// 	Replace the contents of ThreadTest(indef) to run 
//	
//----------------------------------------------------------------------
int N = 5;
char buf[5];
char content[]="Hello World!\n";
Lock *bufLock, *contentLock;
Condition *notFull, *notEmpty;
int contCur=0, putCur=0, getCur=0, bufFree=N;
void printBuf(){
    int i;
    for(i=0;i<5;i++){
        printf("%c", buf[i]);
    }
    printf("\n");
}
int putBuf(char c){
    bufLock->Acquire();
    DEBUG('t', "putBuf has Lock\n");
    while(bufFree==0){notFull->Wait(bufLock);}
    ASSERT(bufLock->isHeldByCurrentThread());
    buf[putCur]=c;
    putCur++;
    putCur=putCur%N;
    bufFree--;
    DEBUG('t', "About to notEmpty->Signal\n");
    notEmpty->Signal(bufLock);

    bufLock->Release(); //********Uncomment if we update Signal and Broadcast
    return 1;
}

char getBuf(){
    bufLock->Acquire();
    DEBUG('t', "getBuf has Lock\n");
    while(bufFree==N){notEmpty->Wait(bufLock);}
    ASSERT(bufLock->isHeldByCurrentThread());
    char c = buf[getCur];
    //buf[getCur]=NULL;
    getCur++;
    getCur=getCur%N;
    bufFree++;
    DEBUG('t', "About to notFull->Signal\n");
    notFull->Signal(bufLock);
    bufLock->Release(); //********Uncomment if we update Signal and Broadcast
    return c;

}
void Producer(int which){
    DEBUG('t', "Entering Producer\n");
    //char content[]="Hello World!";
    char c='a';
    while(c!='\0'){
        
        //printBuf();
        contentLock->Acquire();
        c = content[contCur];
        contCur++;
        //printf("contCur=%d\n", contCur);
        contentLock->Release();
        if(!putBuf(c)){fprintf(stderr, "putBuf failed\n");exit(1);}
    }
    if(!putBuf('\0')){fprintf(stderr, "putBuf failed\n");exit(1);}
    DEBUG('t', "Exiting Producer\n");
}
void Consumer(int which){
    DEBUG('t', "Entering Consumer\n");
    char c;
    while((c=getBuf())!='\0'){printf("%c", c);}
    //printf("\n");
    DEBUG('t', "Exiting Consumer\n");
    /*int num;
    
    for (num = 0; num < 5; num++) {
    printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }*/
    
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void ProdConsTest(){
    DEBUG('t', "Entering ProdCons\n");
    bufLock = new(std::nothrow) Lock("bufLock");
    contentLock = new(std::nothrow) Lock("contentLock");
    notEmpty = new(std::nothrow) Condition("notEmpty");
    notFull = new(std::nothrow) Condition("notFull");

    Thread *t = new(std::nothrow) Thread("Producer1");
    Thread *t2 = new(std::nothrow) Thread("Producer2");
    Thread *t3 = new(std::nothrow) Thread("Consumer2");
    Thread *t4 = new(std::nothrow) Thread("Consumer3");
    Thread *t5 = new(std::nothrow) Thread("Consumer4");
    Thread *t6 = new(std::nothrow) Thread("Consumer5");
    Thread *t7 = new(std::nothrow) Thread("Consumer6");
    Thread *t8 = new(std::nothrow) Thread("Consumer7");

    t->Fork(Producer, 1);
    t3->Fork(Consumer, 3);

// SimpleThreadPriority
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.  
//----------------------------------------------------------------------

    //SimpleThread(0);
}


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
//  Set up a ping-pong between two threads, by forking a thread 
//  to call SimpleThread, and then calling SimpleThread ourselves.
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
//  Loop 5 times, yielding the CPU to another ready thread 
//  each iteration.
//
//  "which" is simply a number identifying the thread, for debugging
//  purposes.  
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
//  Creates two new threads aside from the main thread.  For the first
//  test, the first, fourth and fifth of these threads will be given 
//  the highest priority and the other two will have an equal lower 
//  priority.  The thread with the highest priority should finish first.  
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
int done = 0;
int* floor = 0;
Lock* elevatorLock = new(std::nothrow) Lock("elevatorLock");
Condition* waiting[4];
Condition* filled;
int waiter[4] = {0, 0, 0, 0};
int curFloor = 0;
int dir = 1;
int peopleIn = 0;

class ElevatorManager {
  private:
    //int curFloor = 0; // can never be greater than 3 or less than 0.
    // int directionup = 1;
    int dest;
    const char* name;

  public:
    ElevatorManager(const char* debugName)
    {
    	name = debugName;
    	// queue = new(std::nothrow) List();
    };
    
    ~ElevatorManager()
    {
    };

    void ArrivingGoingFromTo(int atFloor, int toFloor)
    {
		elevatorLock->Acquire();
		waiter[atFloor]++;
		while(curFloor != atFloor)
		{
			waiting[atFloor]->Wait(elevatorLock);
		}
		waiter[atFloor]--;
		peopleIn++;
		if(waiter[atFloor] == 0)
		{
			filled->Signal(elevatorLock);
		}
		while(curFloor != toFloor)
		{
			waiting[toFloor]->Wait(elevatorLock);
		}
		peopleIn--;
		elevatorLock->Release();

    };

};

ElevatorManager* manager = new(std::nothrow) ElevatorManager("manager");

void ElevatorArrivalHandler(int)
{
	done++;
	// elevatorLock->Acquire();
	// curFloor += dir;
	// waiting[curFloor]->Broadcast(elevatorLock);
	// filled->Wait(elevatorLock);
    	return;
};

//	NOTE: although our definition allows only a single integer argument
//	to be passed to the procedure, it is possible to pass multiple
//	arguments by making them fields of a structure, and passing a pointer
//	to the structure as "arg".
class PersonArgs
{
	public:
		int leaving;
		int going;
		PersonArgs(int g, int l)
		{
			going = g;
			leaving = l;
		};
};

void Person(int arg)
{
	PersonArgs * pArgs;
	pArgs = (PersonArgs*)((void*) arg);
	printf("Leaving = %d, Going = %d", pArgs->leaving, pArgs->going);
	// Give an id?
	// manager->ArrivingGoingFromTo(pArgs->leaving, pArgs->going);
	while(done!=10){}
	return;
}

void Elevator(int floors)
{
	Timer* arrivalTimer = new(std::nothrow) Timer(ElevatorArrivalHandler, 0, false); // Need to make sure interrupt 100
	while(done!=10)
	{
		
	}
	delete timer;
}

void
ElevatorTest()
{
	int floors = 3;
	for(int i = 0; i < floors; i++)
	{
		waiting[i] = new(std::nothrow) Condition("waitingBuffer");
	}
	Thread *t = new(std::nothrow) Thread("elevator");
    t->Fork(Elevator, 1);
    t = new(std::nothrow) Thread("p1");
    PersonArgs* pArgs = new(std::nothrow) PersonArgs(1, 2);
    int arg = (int)pArgs;
    t->Fork(Person, arg);

    
 //   done = 1;
}
#endif
