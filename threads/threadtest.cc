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

#include "copyright.h"
#include "system.h"
#include "synch.h"

//Start SimpleThreads

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

//End SimpleThreads

//Start ProdCons

//----------------------------------------------------------------------
// Producer Consumer Example
// 	Replace the contents of ThreadTest(indef) to run 
//	
//----------------------------------------------------------------------

int bufsize, visual;
char buf[100];

char content[]="Hello World!";
int contentSize;
Lock *bufLock, *contentLock;
Condition *notFull, *notEmpty;
int contCur=0, putCur=0, getCur=0, bufFree;
Thread *prods[100];
Thread *cons[100];
char prodnames[100][15];
char consnames[100][15];

//Prints the buffer, does not include newline at the end
void printBuf(){
    int i;
    printf("{ ");
    for(i=0;i<bufsize;i++){
        printf("%c ", buf[i]);
    }
    printf("}");
}

//Producers use this to put the character they have acquired in the buffer
int putBuf(char c){
    bufLock->Acquire();
    DEBUG('t', "putBuf has Lock\n");
    while(bufFree==0){notFull->Wait(bufLock);}
    ASSERT(bufLock->isHeldByCurrentThread());
    buf[putCur]=c;
    putCur++;
    putCur=putCur%bufsize;
    bufFree--;
    DEBUG('t', "About to notEmpty->Signal\n");
    if(visual){printf("Buffer after ");currentThread->Print();printBuf();printf(", ");currentThread->Print();printf("put in '%c'\n", c);}
    notEmpty->Signal(bufLock);

    bufLock->Release(); 
    return 1;
}

//Consumers use this to take the next character from the buffer
char getBuf(){
    bufLock->Acquire();
    DEBUG('t', "getBuf has Lock\n");
    while(bufFree==bufsize){notEmpty->Wait(bufLock);}
    ASSERT(bufLock->isHeldByCurrentThread());
    char c = buf[getCur];
    buf[getCur]='_'; //done for the aesthetic printing of the buffer
    getCur++;
    getCur=getCur%bufsize;
    bufFree++;
    DEBUG('t', "About to notFull->Signal\n");
    if(visual){printf("Buffer after ");currentThread->Print();printBuf();printf(", ");currentThread->Print();printf("took out '%c'\n", c);}
    notFull->Signal(bufLock);
    bufLock->Release(); 
    return c;

}

void Producer(int which){
    DEBUG('t', "Entering Producer\n");
    char c='a'; //initializes c so that it can get into the while loop--is immediately replaced by actual content inside the loop
    while(c!='\0'&&contCur<contentSize){
        contentLock->Acquire();  //accounts for multiple producers
        c = content[contCur];
        contCur++;
        contentLock->Release();
        if(!putBuf(c)){fprintf(stderr, "putBuf failed\n");exit(1);}    
    }    
    DEBUG('t', "Exiting Producer\n");
}

void Consumer(int which){
    DEBUG('t', "Entering Consumer\n");
    char c;
    while (1){
        c = getBuf();
        if(!visual){printf("%c", c);}
    }
    DEBUG('t', "Exiting Consumer\n");
    
}

//----------------------------------------------------------------------
// ProdConsTest
// Sets up as many producers and consumers as requested by the user and
// then runs the producer/consumer scenario on the string requested by
// the user.
//----------------------------------------------------------------------

void ProdConsTest(int numProducers, int numConsumers, int bsize, int vflag){
    //Setting up locks and condition variables and other stuff
    DEBUG('t', "Entering ProdCons\n");
    bufLock = new(std::nothrow) Lock("bufLock");
    contentLock = new(std::nothrow) Lock("contentLock");
    notEmpty = new(std::nothrow) Condition("notEmpty");
    notFull = new(std::nothrow) Condition("notFull");
    bufsize=bsize;
    bufFree=bsize;
    visual=vflag;
    contentSize=strlen(content);

    memset(buf, '_', sizeof(buf));
    //Create producers
    for (int i = 0; i < numProducers; i++) {
        sprintf(prodnames[i], "Producer %d", i);
        prods[i] = new(std::nothrow) Thread(prodnames[i]);
        prods[i]->Fork(Producer, i);
    }
    //Create consumers
    for (int j = 0; j < numConsumers; j++) {
        sprintf(consnames[j], "Consumer %d", j);
        cons[j]= new(std::nothrow) Thread(consnames[j]);
        cons[j]->Fork(Consumer, j);
    }
}

//End ProdCons

//Start Elevator

int* floor = 0;
Lock* elevatorLock;
Condition* waiting;
int going[5] = {0, 0, 0, 0, 0};
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
int floors;
/* Person threads must call this to tell the elevator
they are waiting and to tell it where they want to go
to.  It returns an integer of the floor number where
they ended up */
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
    else if(atFloor - toFloor > 0){
        tempDir = -1;
        dwaiter[atFloor]++;
        dwait++;
    }
    else if(atFloor - toFloor == 0 && atFloor == floors - 1){
        tempDir = -1;
        dwaiter[atFloor]++;
        dwait++;
    }
    else if(atFloor - toFloor == 0 && atFloor == 0){
        tempDir = 1;
        uwaiter[atFloor]++;
        upwait++;        
    }
    else if(atFloor - toFloor == 0){
        // If someone is a fool and tries to get on the elevator
        // to go to the floor they are currently on they just get
        // back off again.
        printf("%s thought it would be great to get on the elevator at %d, but changed it's mind immediately.\n", currentThread->getName(), atFloor);
        elevatorLock->Release();
        return toFloor;
    }
    
    /* If this person is the first one to call the elevator
    tell it what direction to go. */
    arrival->Signal(elevatorLock);
    if(!moving && peopleIn == 0 && peopleWait == 0){
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
   peopleWait++; /*increment the number of people waiting*/
   /* While the elevator is not at the floor the person is waiting at and while it
   is not going the direction the person wants to go wait */
   while(curFloor != atFloor || (curFloor == atFloor && dir != tempDir))
   {
        ASSERT(elevatorLock->isHeldByCurrentThread());
        waiting->Wait(elevatorLock);
   }
   ASSERT(curFloor == atFloor);
   /* Decrement the appropriate wait counters.  Get on the elevator*/
   peopleWait--;
   if(tempDir == -1){
       dwaiter[atFloor]--;
       dwait--;
       // fprintf(stderr, "%s from floor %d boarded going down to floor %d.\n", currentThread->getName(), atFloor, toFloor);
   }
   else{
       uwaiter[atFloor]--;
       upwait--; 
   }
   printf("%s, ", currentThread->getName());
   
   going[toFloor]++;
   peopleIn++;

   /* If no one is waiting on the current floor to go up or down, tell the elevator
   we are ready for it to go */
   if(uwaiter[atFloor] == 0 && (tempDir == 1 || atFloor == 0))
   {
        printf("got on the elevator going up at floor %d.\n", curFloor);
        filled->Signal(elevatorLock);
   }
   else if(dwaiter[atFloor] == 0 && (tempDir == -1 || atFloor == 3)){
        printf("got on the elevator going down at floor %d.\n", curFloor);
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
   printf("%s, ", currentThread->getName());
   /* If you are the last one to get off tell the elevator it can leave */
   if(going[toFloor]==0){
        printf("got off at %d.\n", curFloor);
        filled->Signal(elevatorLock);
   }
   int tempVar = curFloor;
   // fprintf(stderr, "Passenger going to floor %d exited.\n", toFloor);
   elevatorLock->Release();
   /* Return the value of the floor you got off on. */
   return tempVar;
};

/* Class is used to pass the arguments to tell people where they are
going to and leaving from to each person thread */
class PersonArgs
{
	public:
		int leaving;
		int going;
        int delay;
		PersonArgs(int l, int g, int d)
		{
			going = g;
			leaving = l;
            delay = d;
		};
};

void Person(int arg)
{
	PersonArgs * pArgs;
	pArgs = (PersonArgs*)((void*) arg);
    for(int i = 0; i < pArgs->delay; i++){
        currentThread->Yield();
    }
	// fprintf(stderr, "%s waiting at floor %d to go to floor %d\n", currentThread->getName(), pArgs->leaving, pArgs->going);
	int c = ArrivingGoingFromTo(pArgs->leaving, pArgs->going);
  ASSERT(c == pArgs->going);
	return;
}


void Elevator(int)
{
	
    elevatorLock->Acquire();
	while(1)
	{
        printf("******************************\n");
        printf("Elevator has reached floor %d.\n", curFloor);
        // fprintf(stderr, "Elevator at floor %d, waiting upwards %d, waiting downward %d, peopleIn %d, peopleWait %d, direction is %d\n", curFloor, upwait, dwait, peopleIn, peopleWait, dir);
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
            
            if(num != 0 && !(f - curFloor >= 0)){
                /* If the people waiting on a different, or current
                 floor as the elevator, but want it to change 
                 direction, change the direction.*/
                dir = -dir;
            }

            if(f - curFloor == 0){
                dir = -1;
                if(curFloor + dir < 0 || curFloor + dir >= floors){
                    dir = -dir;
                }
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
            if(num != 0 && (f - curFloor >= 0)){
                dir = -dir;
            }
            if(f - curFloor == 0){
                dir = 1;
                if(curFloor + dir < 0 || curFloor + dir >= floors){
                    dir = -dir;
                }
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
        if(curFloor == floors){
            curFloor = floors - 1;
            dir = -1;
        }
        else if(curFloor < 0){
            curFloor = 0;
            dir = 1;
        }

        ASSERT(curFloor < floors); /* Do NOT allow the elevator to go to heaven. */
        ASSERT(curFloor >= 0);     /* Do NOT allow the elevator to descend into the
                                    nether world.*/
        moving = 0; // Elevator is no longer moving.
	}
    elevatorLock->Release();
	
}

Thread *pers[100];
char persnames[100][15];

void
ElevatorTest(int people, int seed)
{
    floors = 5;
    PersonArgs* pArgs;
    int arg;
    srand(seed);
    elevatorLock = new(std::nothrow) Lock("elevatorLock");
    filled = new(std::nothrow) Condition("filled");
    waiting = new(std::nothrow) Condition("waiting");
    arrival = new(std::nothrow) Condition("arrival");
    Thread *te = new(std::nothrow) Thread("elevator");
    te->Fork(Elevator, floors);
     for (int i = 0; i< people; i++) { 
         sprintf(persnames[i], "p%d", i+1);
         pers[i] = new(std::nothrow) Thread(persnames[i]); 
         pArgs = new(std::nothrow) PersonArgs(rand() % floors, rand() % floors, rand() % 10);
         arg = (int)pArgs;
         pers[i]->Fork(Person, arg); 
     } 
}

//End Elevator

//Start PriorityThreads

//----------------------------------------------------------------------
// SimpleThreadPriority
//  Loop 5 times, yielding the CPU to another ready thread 
//  each iteration.
//
//  "which" is simply a number identifying the thread, for debugging
//  purposes.  
//----------------------------------------------------------------------

Thread *threads[100];
char threadnames[100][15];

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
//  Creates as many threads as the user requests aside from the main 
//  thread.  These threads are all given a random priority (either 0 or
//  1) The threads with the highest priority (0) should finish first.  
//----------------------------------------------------------------------

void
ThreadTestPriority(int numThreads)
{
    DEBUG('t', "Entering SimpleTestPriority");

    for (int i = 0; i < numThreads; i++) {
        sprintf(threadnames[i], "priority thread %d", i); //debug name created here
        int priority = rand() % 2;  //Randomly choose either 0 or 1 for the priority
        threads[i] = new(std::nothrow) Thread(threadnames[i], priority);
        threads[i]->Fork(SimpleThreadPriority, i);
    }
}

//End PriorityThreads
#endif

