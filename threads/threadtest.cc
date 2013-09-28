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
    }
    */
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



    //SimpleThread(0);
}


#endif
