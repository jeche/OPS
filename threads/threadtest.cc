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
FILE *read_from, *write_to;

int N = 5;
int prodActive;
char buf[5];
char content[]="Hello World!";
//char content[]="Wake up it's time to do systems right now\n";
Lock *bufLock, *contentLock;
Condition *notFull, *notEmpty;
int contCur=0, putCur=0, getCur=0, bufFree=N;
Thread *prods[100];
Thread *cons[100];
char prodnames[100][15];
char consnames[100][15];
void printBuf(){
    int i;
    printf("{ ");
    for(i=0;i<5;i++){
        printf("%c ", buf[i]);
    }
    printf("}\n");
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
    printf("Buffer after ");currentThread->Print();printBuf();
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
    buf[getCur]='\0';
    getCur++;
    getCur=getCur%N;
    bufFree++;
    DEBUG('t', "About to notFull->Signal\n");
    printf("Buffer after ");currentThread->Print();printBuf();
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
    prodActive--;
    DEBUG('t', "Exiting Producer\n");
}
void Consumer(int which){
    DEBUG('t', "Entering Consumer\n");
    char c;
    while (1)
     {
        c = getBuf();
        printf("%c", c);
    }
    //while((c=getBuf())!='\0'){printf("%c", c);}
    DEBUG('t', "Exiting Consumer\n");
    
}

//----------------------------------------------------------------------
// ProdConsTest
// Sets up as many producers and consumers as requested by the user and
// then runs the producer/consumer scenario on the string requested by
// the user.
//----------------------------------------------------------------------

void ProdConsTest(int numProducers, int numConsumers){
    DEBUG('t', "Entering ProdCons\n");
    bufLock = new(std::nothrow) Lock("bufLock");
    contentLock = new(std::nothrow) Lock("contentLock");
    notEmpty = new(std::nothrow) Condition("notEmpty");
    notFull = new(std::nothrow) Condition("notFull");
    prodActive=numProducers;
    // Tell wish to read the init script

    for (int i = 0; i < numProducers; i++) {
        sprintf(prodnames[i], "Producer %d", i);
        //fprintf(stderr, "%s", name);
        prods[i] = new(std::nothrow) Thread(prodnames[i]);
        prods[i]->Fork(Producer, i);
    }
    for (int j = 0; j < numConsumers; j++) {
        sprintf(consnames[j], "Consumer %d", j);
        cons[j]= new(std::nothrow) Thread(consnames[j]);
        cons[j]->Fork(Consumer, j);
    }


    // char prodbuf[15];
    // char consbuf[15];
    // sprintf(prodbuf, "Producer %d", numProducers);
    // sprintf(consbuf, "Consumer %d", numConsumers);
    
    // Thread *tprod = new(std::nothrow) Thread("prodbuf0",1);
    // Thread *tprod1 = new(std::nothrow) Thread("prodbuf1",1);
    // Thread *tcons = new(std::nothrow) Thread("consbuf1",0);
    

    // tprod->Fork(Producer, numProducers);
    // tprod1->Fork(Producer, numProducers);
    // tcons->Fork(Consumer, numConsumers);

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
    printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
    printf("***** thread %d finished with priority: %d\n", which, currentThread->getPriority());
}

//----------------------------------------------------------------------
// ThreadTest for priority threads
//  Creates six new threads aside from the main thread. The first, fourth 
//  and fifth of these threads will be given the highest priority and the 
//  other three will have an equal lower priority.  The threads with the 
//  highest priority should finish first.  
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

//Elevator
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
