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
int* floor = 0;
Lock* elevatorLock = new(std::nothrow) Lock("elevatorLock");
Condition* waiting;
int going[5];
Condition* filled; 
Condition* arrival;
int uwaiter[5] = {0, 0, 0, 0, 0};
int upwait = 0;
int dwait = 0;
int dwaiter[5] = {0, 0, 0, 0, 0};
int curFloor = 0;
int dir = 1;
int peopleIn = 0;
int moving = 0;
int peopleWait = 0;

class ElevatorManager {
  private:
    int dest;
    const char* name;

  public:
    ElevatorManager(const char* debugName)
    {
    	name = debugName;
    };
    
    ~ElevatorManager()
    {
    };

    int ArrivingGoingFromTo(int atFloor, int toFloor)
    {   /*Acquires the elevator lock*/
        elevatorLock->Acquire();
        int tempDir = 0; 
        /* tempDir will be the direction the person 
        calling the function wants to go the following
        if block determines which way.  It also increments
        the down, or up waiting buffer for the current
        floor as appropriate.  It also increments the 
        number of people waiting to go the appropriate direction*/
        if(atFloor - toFloor < 0){
            tempDir = 1;
            uwaiter[atFloor]++;
            upwait++;
        }
        else{
            tempDir = -1;
            dwaiter[atFloor]++;
            dwait++;
        }
        peopleWait++; /*increment the number of people waiting*/
        /* If this person is the first one to call the elevator
        tell it what direction to go. */
        if(!moving && peopleIn == 0){
	       arrival->Signal(elevatorLock);
           if(atFloor > curFloor){
            dir = 1;
           }
           else if(atFloor < curFloor){
            dir = -1;
           }
           else if(atFloor - toFloor < 0){
            dir = 1;
           }
           else{
            dir = -1;
           }
       }
       /* While the elevator is not at the floor the person is waiting at and while it
       is not going the direction the person wants to go wait */
	   while(curFloor != atFloor || (curFloor == atFloor && dir != tempDir))
	   {
            ASSERT(elevatorLock->isHeldByCurrentThread());
            waiting->Wait(elevatorLock);
	   }
       
       /* Decrement the appropriate wait counters.  Get on the elevator*/
       peopleWait--;
       if(tempDir == -1){
	       dwaiter[atFloor]--;
           dwait--;
           fprintf(stderr, "Passenger from floor %d boarded going down to floor %d.\n", atFloor, toFloor);
       }
       else{
           uwaiter[atFloor]--;
           upwait--;
           fprintf(stderr, "Passenger from floor %d boarded going up to floor %d.\n", atFloor, toFloor);
       }
	   going[toFloor]++;
	   peopleIn++;

       /* If no one is waiting on the current floor to go up or down, tell the elevator
       we are ready for it to go */
	   if(uwaiter[atFloor] == 0 && (tempDir == 1 || atFloor == 3))
	   {
            filled->Signal(elevatorLock);
	   }
       else if(dwaiter[atFloor] == 0 && (tempDir == -1 || atFloor == 0)){
            filled->Signal(elevatorLock);
       }
       /* While the elevator has not reached the person's floor sleep */
	   while(curFloor != toFloor)
	   {
		  waiting->Wait(elevatorLock);
	   }
       /*Get off the elevator.*/
	   peopleIn--;
	   going[toFloor]--;
       /* If you are the last one to get off tell the elevator it can leave */
       if(going[toFloor]==0){
            filled->Signal(elevatorLock);
       }
       int tempVar = curFloor;
       fprintf(stderr, "Passenger going to floor %d exited.\n", toFloor);
	   elevatorLock->Release();
       /* Return the value of the floor you got off on. */
       return tempVar;
    };

};

ElevatorManager* manager;

class PersonArgs
{
	public:
		int leaving;
		int going;
		PersonArgs(int l, int g)
		{
			going = g;
			leaving = l;
		};
};

void Person(int arg)
{
	PersonArgs * pArgs;
	pArgs = (PersonArgs*)((void*) arg);
	fprintf(stderr, "Waiting at floor %d to go to floor %d\n", pArgs->leaving, pArgs->going);
	int c = manager->ArrivingGoingFromTo(pArgs->leaving, pArgs->going);
    fprintf(stderr, "I left from %d and got to %d when I wanted to go to %d.\n", pArgs->leaving, pArgs->going, c);
    ASSERT(c == pArgs->going);
	return;
}


void Elevator(int floors)
{
	
    elevatorLock->Acquire();
	while(1)
	{
        fprintf(stderr, "Elevator at floor %d, waiting upwards %d, waiting downward %d, peopleIn %d, peopleWait %d\n", curFloor, upwait, dwait, peopleIn, peopleWait);
        if(peopleIn == 0 && peopleWait == 0){
		  arrival->Wait(elevatorLock);  // If no one wants to get on the elevator yet and there is no one in the elevator wait for an arrival.
        }
        waiting->Broadcast(elevatorLock); // If someone is waiting at your current floor wake them up and have them get on.
        if(dir == 1){
             /* If you are going up check who is waiting to go up on
              that floor and do not move until they are on and all 
              the people going to the current floor are off.*/
            while(uwaiter[curFloor] > 0 || going[curFloor] > 0){
                filled->Wait(elevatorLock);
            }
        }
        else{
            /* If you are going down check who is waiting to go down on
              that floor and do not move until they are on and all 
              the people going to the current floor are off.*/
            while(dwaiter[curFloor] > 0 || going[curFloor] > 0){
                filled->Wait(elevatorLock);
            }
        }
        /* If everyone has left and no one wanted to/wants to get on
        wait until someone else arrives*/
        if(peopleWait == 0 && peopleIn == 0){
            arrival->Wait(elevatorLock);
        }

        if(dir == 1 && peopleIn == 0 && upwait == 0 && dwait > 0){
            /* If the elevator was going up and is now empty, but
            there are still people waiting to go down above you
            continue upwards until you reach them*/
            int num = 0;
            int f = floors;
            while(num == 0 && f >= 0){
                f--;
                num = dwaiter[f];
            }
            
            if(num != 0 && !(f - curFloor > 0)){
                /* If the people waiting on a different, or current
                 floor as the elevator, but want it to change 
                 direction, change the direction.*/
                dir = -dir;
            }

            if(f - curFloor == 0){
                waiting->Broadcast(elevatorLock);
                while(dwaiter[curFloor] > 0 || going[curFloor] > 0){
                    filled->Wait(elevatorLock);
                }                
            }
        }
        else if(dir == -1 && peopleIn == 0 && dwait == 0 && upwait > 0){
            int num = 0;
            int f = -1;
            while(num == 0 && f <= floors){
                f++;
                num = uwaiter[f];
            }
            if(num != 0 && (f - curFloor > 0)){
                dir = -dir;
            }
            if(f - curFloor == 0){
                waiting->Broadcast(elevatorLock);
                while(uwaiter[curFloor] > 0 || going[curFloor] > 0){
                    filled->Wait(elevatorLock);
                }                
            }
        }
        moving = 1; // The elevator is about to start moving.
        /* For loop toggles interrupts 10 times which is the equivalent of 100 ticks */
        for(int d = 0; d < 10; d++){
            IntStatus oldLevel = interrupt->SetLevel(IntOff);
            (void) interrupt->SetLevel(oldLevel);
        }
        curFloor = curFloor + dir; /* Floor has now changed so increase it by the direction you are going.*/
        ASSERT(curFloor < floors); /* Do NOT allow the elevator to go to heaven. */
        ASSERT(curFloor >= 0);     /* Do NOT allow the elevator to descend into the
                                    nether world.*/
        moving = 0; // Elevator should stop moving.
	}
    elevatorLock->Release();
	
}

void
ElevatorTest(int people, int seed)
{
    Thread *t[people + 1];
    char pname[people + 1][5]; 
    int floors = 5;
    PersonArgs* pArgs;
    int arg;
    srand(seed);
    filled = new(std::nothrow) Condition("filled");
    waiting = new(std::nothrow) Condition("waiting");
    manager = new(std::nothrow) ElevatorManager("manager");
    arrival = new(std::nothrow) Condition("arrival");
    Thread *te = new(std::nothrow) Thread("elevator");
    te->Fork(Elevator, floors);
    // for (int i = 0; i< people; i++) { 
    //     sprintf(pname[i], "p%d", i); 
    //     t[i] = new(std::nothrow) Thread(pname[i]); 
    //     pArgs = new(std::nothrow) PersonArgs(rand() % floors, rand() % floors);
    //     arg = (int)pArgs;
    //     t[i]->Fork(Person, arg); 
    // } 
    te = new(std::nothrow) Thread("aaaaaa1"); 
    pArgs = new(std::nothrow) PersonArgs(0, 1);
    arg = (int)pArgs;
    te->Fork(Person, arg);     
    te = new(std::nothrow) Thread("aaaaa2a"); 
    pArgs = new(std::nothrow) PersonArgs(1, 2);
    arg = (int)pArgs;
    te->Fork(Person, arg);     
    // te = new(std::nothrow) Thread("aaa3aaa"); 
    // pArgs = new(std::nothrow) PersonArgs(1, 2);
    // arg = (int)pArgs;
    // te->Fork(Person, arg);     
    te = new(std::nothrow) Thread("aaa4aaa"); 
    pArgs = new(std::nothrow) PersonArgs(2, 1);
    arg = (int)pArgs;
    te->Fork(Person, arg);
    te = new(std::nothrow) Thread("aaa4aaeea"); 
    pArgs = new(std::nothrow) PersonArgs(0, 3);
    arg = (int)pArgs;
    te->Fork(Person, arg);     
    te = new(std::nothrow) Thread("aaaqq4aaa"); 
    pArgs = new(std::nothrow) PersonArgs(3, 0);
    arg = (int)pArgs;
    te->Fork(Person, arg);     
    te = new(std::nothrow) Thread("aaaqq4aaa"); 
    pArgs = new(std::nothrow) PersonArgs(2, 0);
    arg = (int)pArgs;
    te->Fork(Person, arg);     
    // te = new(std::nothrow) Thread("aaa5aaa"); 
    // pArgs = new(std::nothrow) PersonArgs(3, 1);
    // arg = (int)pArgs;
    // te->Fork(Person, arg);     
    // te = t[i] = new(std::nothrow) Thread("aaa2aaa"); 
    // pArgs = new(std::nothrow) PersonArgs(rand() % floors, rand() % floors);
    // arg = (int)pArgs;
    // te->Fork(Person, arg);     
    // te = t[i] = new(std::nothrow) Thread("aaaa3aa"); 
    // pArgs = new(std::nothrow) PersonArgs(rand() % floors, rand() % floors);
    // arg = (int)pArgs;
    // te->Fork(Person, arg);     
    // te = t[i] = new(std::nothrow) Thread("aaaa4aa"); 
    // pArgs = new(std::nothrow) PersonArgs(rand() % floors, rand() % floors);
    // arg = (int)pArgs;
    // te->Fork(Person, arg);     
    // te = t[i] = new(std::nothrow) Thread("aaa1aaa"); 
    // pArgs = new(std::nothrow) PersonArgs(rand() % floors, rand() % floors);
    // arg = (int)pArgs;
    // te->Fork(Person, arg);     
}
#endif
