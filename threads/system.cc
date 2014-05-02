#ifndef CHANGED
// system.cc 
//	Nachos initialization and cleanup routines.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

// This defines *all* of the global data structures used by Nachos.
// These are all initialized and de-allocated by this file.

Thread *currentThread;			// the thread we are running now
Thread *threadToBeDestroyed;  		// the thread that just finished
Scheduler *scheduler;			// the ready list
Interrupt *interrupt;			// interrupt status
Statistics *stats;			// performance metrics
Timer *timer;				// the hardware timer device,
					// for invoking context switches


#ifdef FILESYS_NEEDED
FileSystem  *fileSystem;
#endif

#ifdef FILESYS
SynchDisk   *synchDisk;//***********************
#endif

#ifdef USER_PROGRAM	// requires either FILESYS or FILESYS_STUB
Machine *machine;	// user program memory and registers
#endif

#ifdef NETWORK
char diskname[50];
int netname;        // UNIX socket name
PostOffice *postOffice;
#endif


// External definition, to allow us to take a pointer to this function
extern void Cleanup();


//----------------------------------------------------------------------
// TimerInterruptHandler
// 	Interrupt handler for the timer device.  The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as 
//	if the interrupted thread called Yield at the point it is 
//	was interrupted.
//
//	"dummy" is because every interrupt handler takes one argument,
//		whether it needs it or not.
//----------------------------------------------------------------------
static void
TimerInterruptHandler(int )
{
    if (interrupt->getStatus() != IdleMode)
	interrupt->YieldOnReturn();
}

//----------------------------------------------------------------------
// Initialize
// 	Initialize Nachos global data structures.  Interpret command
//	line arguments in order to determine flags for the initialization.  
// 
//	"argc" is the number of command line arguments (including the name
//		of the command) -- ex: "nachos -d +" -> argc = 3 
//	"argv" is an array of strings, one for each command line argument
//		ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------
void
Initialize(int argc, char **argv)
{
    int argCount;
    char *debugArgs = (char *)""; 
    bool randomYield = false;
    netname = 0

#ifdef USER_PROGRAM
    bool debugUserProg = false;	// single step user program
#endif
#ifdef FILESYS_NEEDED
    bool format = false;	// format disk
#endif
#ifdef NETWORK
    double rely = 1;		// network reliability

#endif
    
    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
	argCount = 1;
	if (!strcmp(*argv, "-d")) {
	    if (argc == 1)
		debugArgs = (char *) "+";	// turn on all debug flags
	    else {
	    	debugArgs = *(argv + 1);
	    	argCount = 2;
	    }
	} else if (!strcmp(*argv, "-rs")) {
	    ASSERT(argc > 1);
	    RandomInit(atoi(*(argv + 1)));	// initialize pseudo-random
						// number generator
	    randomYield = true;
	    argCount = 2;
	}
#ifdef USER_PROGRAM
	if (!strcmp(*argv, "-s"))
	    debugUserProg = true;
#endif
#ifdef FILESYS_NEEDED
	if (!strcmp(*argv, "-f"))
	    format = true;
#endif
#ifdef NETWORK
	if (!strcmp(*argv, "-n")) {
	    ASSERT(argc > 1);
	    rely = atof(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-m")) {
	    ASSERT(argc > 1);
	    netname = atoi(*(argv + 1));
	    argCount = 2;
	}
#endif
    }

    DebugInit(debugArgs);			// initialize DEBUG messages
    stats = new(std::nothrow) Statistics();			// collect statistics
    interrupt = new(std::nothrow) Interrupt;			// start up interrupt handling
    scheduler = new(std::nothrow) Scheduler();		// initialize the ready queue
    if (randomYield)				// start the timer (if needed)
	timer = new(std::nothrow) Timer(TimerInterruptHandler, 0, randomYield);
    threadToBeDestroyed = NULL;

    // We didn't explicitly allocate the current thread we are running in.
    // But if it ever tries to give up the CPU, we better have a Thread
    // object to save its state. 
    currentThread = new(std::nothrow) Thread("main");		
    currentThread->setStatus(RUNNING);

    interrupt->Enable();
    CallOnUserAbort(Cleanup);			// if user hits ctl-C
    
#ifdef USER_PROGRAM
    machine = new(std::nothrow) Machine(debugUserProg);	// this must come first
#endif
synchDisk = new(std::nothrow) SynchDisk("DISK");
#ifdef FILESYS
    
#endif

#ifdef FILESYS_NEEDED
    fileSystem = new(std::nothrow) FileSystem(format);
#endif

#ifdef NETWORK
    postOffice = new(std::nothrow) PostOffice(netname, rely, 10);
#endif
}

//----------------------------------------------------------------------
// Cleanup
// 	Nachos is halting.  De-allocate global data structures.
//----------------------------------------------------------------------
void
Cleanup()
{
    printf("\nCleaning up...\n");
#ifdef NETWORK
    delete postOffice;
#endif
    
#ifdef USER_PROGRAM
    delete machine;
#endif

#ifdef FILESYS_NEEDED
    delete fileSystem;
#endif
    delete synchDisk;

#ifdef FILESYS
#endif
    
    delete timer;
    delete scheduler;
    delete interrupt;
    
    Exit(0);
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

// system.cc 
//  Nachos initialization and cleanup routines.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>


// This defines *all* of the global data structures used by Nachos.
// These are all initialized and de-allocated by this file.

Thread *currentThread;          // the thread we are running now
Thread *threadToBeDestroyed;        // the thread that just finished
Scheduler *scheduler;           // the ready list
Interrupt *interrupt;           // interrupt status
Statistics *stats;          // performance metrics
Timer *timer;               // the hardware timer device,
Timer *timer2;
                    // for invoking context switches
Semaphore *forking;
Semaphore *forkexecing;
Semaphore *chillBrother;
Semaphore *execing;

SynchConsole *synchConsole;
BitMap *bitMap;
FamilyNode* root;
unsigned int pid;
unsigned long long timeoutctr;
unsigned int msgctr;
List *allThreads;
List *migThreads;
ForeignThreadNode *foreignRoot;

#ifdef FILESYS_NEEDED
FileSystem  *fileSystem;
#endif

SynchDisk   *synchDisk;
BitMap *diskBitMap;
ramEntry **ramPages;
diskEntry **diskPages;
int commutator;

#ifdef FILESYS

#endif

#ifdef USER_PROGRAM // requires either FILESYS or FILESYS_STUB
Machine *machine;   // user program memory and registers
#endif

#ifdef NETWORK
char diskname[50];
PostOffice *postOffice;
BitMap *mailboxes;
Semaphore *msgCTR;
Timer *timeoutTimer;
Thread *timeout;
Semaphore *activeClientListSem;
Semaphore *migrationSem;

int netname;
int server = -1;
int clients[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
float clientLoad[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
float numReadyProc = 0;
bool migrateInProcess = false;
#endif


// External definition, to allow us to take a pointer to this function
extern void Cleanup();


//----------------------------------------------------------------------
// TimerInterruptHandler
//  Interrupt handler for the timer device.  The timer device is
//  set up to interrupt the CPU periodically (once every TimerTicks).
//  This routine is called each time there is a timer interrupt,
//  with interrupts disabled.
//
//  Note that instead of calling Yield() directly (which would
//  suspend the interrupt handler, not the interrupted thread
//  which is what we wanted to context switch), we set a flag
//  so that once the interrupt handler is done, it will appear as 
//  if the interrupted thread called Yield at the point it is 
//  was interrupted.
//
//  "dummy" is because every interrupt handler takes one argument,
//      whether it needs it or not.
//----------------------------------------------------------------------
/*static void                        **************
TimerInterruptHandler(int )
{
    fprintf(stderr, "Interrupt\n");
    // if ( stats->totalTicks > timeoutctr + TIMEOUT){
    //     timeoutctr = stats->totalTicks;
    //     fprintf(stderr, "Setting Ready to Run\n");
    //     scheduler->ReadyToRun(timeout);
    // }
    if (interrupt->getStatus() != IdleMode)
    interrupt->YieldOnReturn();
}*/
void serverMigrationHandler(int from, int to){
    fprintf(stderr, "I want to migrate a process from machine %d to %d\n",from, to );
    DEBUG('a', "Migration");
    int msgID, adult, oldPID, fromMacher, toMacher;
    PacketHeader outPktHdr;
    MailHeader outMailHdr;
    AckHeader outAckHdr;
    
    MessageNode* recved;
    Mail* recMail, *mail;
    MailNode* curNode;
    ForeignThreadNode *curFTN;
    char pageBuf[128];
    memset(pageBuf, '\0', 128);

    

    //First send to `victim` saying that it has been chosen

    msgCTR->P();
    msgctr++;
    msgID=msgctr;
    msgCTR->V(); 
    outPktHdr.to = from;   
    outMailHdr.to = 1;
    //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
    outMailHdr.from = netname;//1; 
    // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
    outMailHdr.length = 128; // had a plus 1 here before?????????
    outAckHdr.totalSize = 1;// size/MaxMailSize ; 
    outAckHdr.curPack = 0;
    outAckHdr.messageID = msgID;
    outAckHdr.migrateFlag = 0;
    fprintf(stderr, "Starting call\n");
    mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
    postOffice->SendThings(mail, 0);
    fprintf(stderr, "Sent first message\n");

    //Next wait for its response...
    
    recved = postOffice->GrabMessage(0);
    fprintf(stderr, "Received Response\n");
    curNode = recved->head;
    recMail = curNode->cur;

    //Validate its respones? YES, change this later AKA no ASSERT(false)

    ASSERT(recMail->ackHdr.migrateFlag >= 0);

    //Forward the `victim` clients response AKA the data to the `chosen` client
    adult = recMail->ackHdr.migrateFlag;
    msgCTR->P();
    msgctr++;
    msgID=msgctr;
    msgCTR->V(); 
    outPktHdr.to = to;   
    outMailHdr.to = 1;
    //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
    outMailHdr.from = netname;//1; 
    // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
    outMailHdr.length = 128; // had a plus 1 here before?????????
    outAckHdr.totalSize = 1;// size/MaxMailSize ; 
    outAckHdr.curPack = 0;
    outAckHdr.messageID = msgID;
    outAckHdr.migrateFlag = 1;
    outAckHdr.pageID = recMail->ackHdr.pageID;

    oldPID = outAckHdr.pageID;
    fromMacher = from;
    toMacher = to;
    // postOffice->SendThings(mail, netname);

    //Right now I am just passing in the buffer from one message to another, this might
    //need to change to deep copying it over to a new buffer due to bad things happening...

    mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, recMail->data);
    postOffice->SendThings(mail, netname);
    fprintf(stderr, "Waiting on the chosen one\n");
    //Wait for a response from the `chosen` client 
    recved = postOffice->GrabMessage(netname);
    curFTN = foreignRoot;
    while(curFTN->next != NULL){
        curFTN = curFTN->next;
    }
    curFTN->next = new(std::nothrow) ForeignThreadNode(oldPID, recved->head->cur->ackHdr.pageID, fromMacher, toMacher, adult);
    fprintf(stderr, "Responded\n");
    curNode = recved->head;
    recMail = curNode->cur;

    // Give us all the already exited values.



    msgCTR->P();
    msgctr++;
    msgID=msgctr;
    msgCTR->V(); 
    outPktHdr.to = from;   
    outMailHdr.to = 1;
    //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
    outMailHdr.from = netname;//1; 
    // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
    outMailHdr.length = 128; // had a plus 1 here before?????????
    outAckHdr.totalSize = 1;// size/MaxMailSize ; 
    outAckHdr.curPack = 0;
    outAckHdr.messageID = msgID;
    outAckHdr.pageID = oldPID;
    outAckHdr.migrateFlag = 3;


    mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
    postOffice->SendThings(mail, 0);
    //If it is a success, doge is on your side
    //    much happy                          
    //                          very nachos
    //             such network               
    //     many migration             
    //                         wow            
    if(recMail->ackHdr.migrateFlag!=2){ASSERT(false);}

}

void serverMigrationTest(){
    fprintf(stderr, "in serverMigrationTest\n");
    migrateInProcess = true;
    float min, max;
    int from, to;
    max = 0;
    min = 1000;
    from = -1;
    to = -1;
    for(int i = 0; i < 10; i++){
        if(clientLoad[i] != -1 && clientLoad[i]<min){
            min = clientLoad[i];
            to = i;
        }
        if(clientLoad[i]>max){
            max = clientLoad[i];
            from = i;
        }
        fprintf(stderr, "clientLoad[%d]: %f\n", i, clientLoad[i] );
    }
    if(from != -1 && to != -1){
        fprintf(stderr, "from: %d;%f to: %d;%f\n", from, clientLoad[from], to, clientLoad[to]);
        if(max-min > 3){
            serverMigrationHandler(from, to);
        }
    }
    for(int i = 0; i < 10; i++){
        if(clientLoad[i] != -1){
            clientLoad[i] = clientLoad[i]*.8;
        }
    }
    migrateInProcess = false;
    currentThread->Finish();
} 
static void
serverMigrationTestHelper(int a){
    serverMigrationTest();
}  
//----------------------------------------------------------------------
// TimerInterruptHandler
//  Interrupt handler for the timer device.  The timer device is
//  set up to interrupt the CPU periodically (once every TimerTicks).
//  This routine is called each time there is a timer interrupt,
//  with interrupts disabled.
//
//  Note that instead of calling Yield() directly (which would
//  suspend the interrupt handler, not the interrupted thread
//  which is what we wanted to context switch), we set a flag
//  so that once the interrupt handler is done, it will appear as 
//  if the interrupted thread called Yield at the point it is 
//  was interrupted.
//
//  "dummy" is because every interrupt handler takes one argument,
//      whether it needs it or not.
//----------------------------------------------------------------------
static void
TimerInterruptHandler2(int )
{
    //fprintf(stderr, "Interrupt\n");
    unsigned long long TIMEOUTKILLER;
    TIMEOUTKILLER =  100000000;
    if ( stats->totalTicks > timeoutctr + TIMEOUTKILLER){
        timeoutctr = stats->totalTicks;
        postOffice->KaputTime();
        if(server == -1 && migrateInProcess == false){
            Thread *t = new Thread("migrationwoo");
            t->Fork(serverMigrationTestHelper, 42);
        }
        // fprintf(stderr, "Setting Ready to Run\n");
        // fprintf(stderr, "running time out at %ld\n", timeoutctr);
        // scheduler->ReadyToRun(timeout);
    }
    if (interrupt->getStatus() != IdleMode)
    interrupt->YieldOnReturn();
}


void
TimeoutHandler() {
    for(;;) {
        IntStatus oldLevel = interrupt->SetLevel(IntOff);
        timeout->Sleep();
        (void) interrupt->SetLevel(oldLevel);
        // fprintf(stderr, "Awakening mboxes at time %ld\n", stats->totalTicks);
        for (int i = 0; i < 10; i++) {
            postOffice->ackLockAcquire(i);
            postOffice->hasAckSignal(i);
            postOffice->ackLockRelease(i);
        }
    }
}
void
migForkHandler(int k){
    // ASSERT(false);
    // Please work, you can do it, yay, you are awesome nachos, woo, yay
    currentThread->RestoreUserState();
    currentThread->space->RestoreState();
    // run among the other chips little mig thread
    // dance in the melty cheese
    // frolic in the salsa
    machine->Run();

}
void Pager(int clientMachNum){
    for(;;){
        PacketHeader outPktHdr;
        MailHeader outMailHdr;
        AckHeader outAckHdr;
        ASSERT(clientMachNum != 0);
        if(clientMachNum == 1){
            // fprintf(stderr, "ready to grab\n");
        }
        MessageNode* message = postOffice->GrabMessage(clientMachNum);
        // ASSERT(false);
        MailNode* curNode = message->head;
        Mail* curMail = curNode->cur;
        Mail* mail;
        char pageBuf[128];
        int pageNum;
        int msgID;


        if(curMail->mailHdr.length == 1){ //Read
            pageNum = curMail->ackHdr.pageID;
            if(clientLoad[curMail->pktHdr.from] != -1){
                //fprintf(stderr, "%f\n", (float)curMail->ackHdr.migrateFlag);
                clientLoad[curMail->pktHdr.from] = curMail->ackHdr.migrateFlag*.1 + clientLoad[curMail->pktHdr.from] *.9;
            }
            else{
                clientLoad[curMail->pktHdr.from] = curMail->ackHdr.migrateFlag;
            }   
            synchDisk->ReadSector(pageNum, pageBuf);

            msgCTR->P();
            msgctr++;
            msgID = msgctr;
            msgCTR->V(); 

            outPktHdr.to = clientMachNum;   
            outMailHdr.to = 0;
            //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
            outMailHdr.from = clientMachNum;//1; 
            // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
            outMailHdr.length = 128; // had a plus 1 here before?????????
            outAckHdr.totalSize = 1;// size/MaxMailSize ; 
            outAckHdr.curPack = 0;
            outAckHdr.messageID = msgID;
            outAckHdr.pageID = pageNum;
            

            mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            postOffice->SendThings(mail, clientMachNum);
            // postOffice->Send(outPktHdr, outMailHdr, outAckHdr, mail->data);
            // delete mail;

            // fprintf(stderr, "Read Serviced %d %d %d %d\n", curMail->mailHdr.from ,curMail->ackHdr.messageID, msgID, clientMachNum);

        } else if(curMail->mailHdr.length == 128){ //Write
            pageNum = curMail->ackHdr.pageID;
            if(clientLoad[curMail->pktHdr.from] != -1){
                //fprintf(stderr, "%f\n", (float)curMail->ackHdr.migrateFlag);
                //clientLoad[curMail->pktHdr.from] = (float)curMail->ackHdr.migrateFlag*.05 + clientLoad[curMail->pktHdr.from] *.95;
            }
            else{
                clientLoad[curMail->pktHdr.from] = curMail->ackHdr.migrateFlag;
            }  
            msgCTR->P();
            msgctr++;
            msgID=msgctr;
            msgCTR->V(); 
            synchDisk->WriteSector(pageNum, curMail->data);
            outPktHdr.to = clientMachNum;   
            outMailHdr.to = 0;
            //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
            outMailHdr.from = clientMachNum;//1; 
            // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
            outMailHdr.length = 128; // had a plus 1 here before?????????
            outAckHdr.totalSize = 1;// size/MaxMailSize ; 
            outAckHdr.curPack = 0;
            outAckHdr.messageID = msgID;
            outAckHdr.pageID = pageNum;

            curMail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, curMail->data);
            
            // postOffice->Send(outPktHdr, outMailHdr, outAckHdr, curMail->data);
            // delete curMail;

            postOffice->SendThings(curMail, clientMachNum);
            // fprintf(stderr, "Write Serviced\n");

        } else if(curMail->mailHdr.length == 2){
/***            int from = curMail->pktHdr.from;
            activeClientListSem->P();
            activeClientList[from]++; 
            activeClientListSem->V();*/
            int found = diskBitMap->Find();

           // fprintf(stderr, "allocated %d\n", found);

            msgCTR->P();
            msgctr++;
            msgID=msgctr;
            msgCTR->V(); 
            outPktHdr.to = clientMachNum;   
            outMailHdr.to = 0;
            //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
            outMailHdr.from = clientMachNum;//1; 
            // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
            outMailHdr.length = 128; // had a plus 1 here before?????????
            outAckHdr.totalSize = 1;// size/MaxMailSize ; 
            outAckHdr.curPack = 0;
            outAckHdr.messageID = msgID;
            outAckHdr.pageID = found;

            curMail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            postOffice->SendThings(curMail, clientMachNum);
            // postOffice->Send(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            // delete curMail;
            //Handle the case of load balancing here!!!!!!!!
            //Fork off a thread if the condition is met.
/***            activeClientListSem->P();
            if(activeClientList[from] > 4){//MIGRATE!!!!!!!
                activeClientList[from]--;
                activeClientListSem->V();
                Thread *t = new(std::nothrow) Thread("migrationThread");
                t->fork(serverMigrationHelper, from);

            }
            else{activeClientListSem->V();}*/
        
        } else if(curMail->mailHdr.length == 3) {
            // ASSERT(false);
            diskBitMap->Clear(curMail->ackHdr.pageID);

            //fprintf(stderr, "clearing: %d\n", curMail->ackHdr.pageID);

/***            activeClientListSem->P();
            activeClientList[curMail->pktHdr.from]--;
            activeClientListSem->V();
            diskBitMap->Clear(curMail->ackHdr.pageID);*/
            msgCTR->P();
            msgctr++;
            msgID=msgctr;
            msgCTR->V(); 
            outPktHdr.to = clientMachNum;   
            outMailHdr.to = 0;
            //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
            outMailHdr.from = clientMachNum;//1; 
            // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
            outMailHdr.length = 128; // had a plus 1 here before?????????
            outAckHdr.totalSize = 1;// size/MaxMailSize ; 
            outAckHdr.curPack = 0;
            outAckHdr.messageID = msgID;
            outAckHdr.pageID = 0;

            curMail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            postOffice->SendThings(curMail, clientMachNum);
            // postOffice->Send(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            // delete curMail;
            //No load balancing necessary as we are removing a process!
            
        } else if(curMail->mailHdr.length == 4){
            // this is a signal to send out for a network exit, sends to either the original machine or to wherever the parent went
            msgCTR->P();
            msgctr++;
            msgID=msgctr;
            msgCTR->V(); 
            ForeignThreadNode *curFTN = foreignRoot;
            ForeignThreadNode *possParent;
            while(curFTN->next != NULL && (curFTN->curPID != curMail->ackHdr.pageID || curFTN->toMach != curMail->pktHdr.from)){
                curFTN = curFTN->next; 
            }
            for(possParent = foreignRoot; possParent != NULL && (curFTN->parent != possParent->origPID || curFTN->fromMach != possParent->fromMach); possParent = possParent->next){}
            if(possParent != NULL){
                outPktHdr.to = possParent->toMach;
                outMailHdr.from = possParent->toMach;
                outAckHdr.pageID = possParent->curPID; // the pid of the parent on the new machine 
            }
            else{
                outPktHdr.to = curFTN->fromMach;    
                outMailHdr.from = curFTN->fromMach;
                outAckHdr.pageID = curFTN->origPID;
            }
            // while(possParent != NULL)
            // fprintf(stderr, "exit value %d\n", curMail->ackHdr.migrateFlag);
            sprintf(pageBuf, "%d\n", curMail->ackHdr.migrateFlag); // exit value
            // fprintf(stderr, pageBuf);
            
            outMailHdr.to = 1;
            
            outMailHdr.length = 128;
            outAckHdr.totalSize = 1;
            outAckHdr.curPack = 0;
            outAckHdr.messageID = msgID;
            outAckHdr.migrateFlag = 2;
            curMail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            postOffice->SendThings(curMail, curFTN->fromMach);
        } else if (curMail->mailHdr.length == 5){
            // parent was migrated sending to the parent's new machine the child's info, child pid is in pageID
            msgCTR->P();
            msgctr++;
            msgID=msgctr;
            msgCTR->V();
            int parent = curMail->ackHdr.child;
            int origin = clientMachNum;
            ForeignThreadNode *parentLoc;
            for(parentLoc = foreignRoot ; parentLoc != NULL && (parent != parentLoc->origPID || origin != parentLoc->fromMach); parentLoc = parentLoc->next){ fprintf(stderr, "%d %d %d %d\n", parent, parentLoc->origPID, origin, parentLoc->fromMach);}
            ASSERT(parentLoc != NULL);
            outAckHdr.child = curMail->ackHdr.pageID;
            outPktHdr.to = parentLoc->toMach;
            outMailHdr.from = parentLoc->toMach;
            outAckHdr.pageID = parentLoc->curPID;
            

            sprintf(pageBuf, "%d\n", curMail->ackHdr.migrateFlag); // exit value
            outMailHdr.to = 1;
            outMailHdr.length = 128;
            outAckHdr.totalSize = 1;
            outAckHdr.curPack = 0;
            outAckHdr.messageID = msgID;
            outAckHdr.migrateFlag = 2;
            curMail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            postOffice->SendThings(curMail, outMailHdr.from);
        }
        else{
            ASSERT(false);
        }
        
    }
}

void migrationHandler(){
    //*********************************************************
    //*******************Important Note************************
    //***********Interrupts cannot be turned off***************
    //************when any networking stuff is*****************
    //*************going on otherwise we hang******************
    //*********************************************************

    IntStatus oldLevel;
    for(;;){
        PacketHeader outPktHdr;
        MailHeader outMailHdr;
        AckHeader outAckHdr;
        OpenFile* open;
        FamilyNode* curr;
        ForeignThreadNode* curFTN;
        int len, reg, numPages;
        char buffer[20];


        MessageNode* message = postOffice->GrabMessage(1);
        // ASSERT(false);
        //scheduler->Print();
        fprintf(stderr, "\n\n");
        // oldLevel = interrupt->SetLevel(IntOff);// Turn off interupts since we don't want processes to wake up while we are doing this
        // ASSERT(false);
        
        MailNode* curNode = message->head;
        Mail* curMail = curNode->cur;
        Mail* mail;
        char pageBuf[128];
        int msgID;
        memset(pageBuf, '\0', sizeof(pageBuf));
        if (curMail->ackHdr.migrateFlag == 2){
            // Network Exit if child was migrated
            FamilyNode *curr;
            forking->P();
            curr = root;
            while(curr->child != curMail->ackHdr.pageID && curr->next !=NULL){
                  curr = curr->next;  // Iterate to find the correct semphore to V
            }
            if(curr == NULL){
                for(curr = root; curr->next != NULL && (curr->child != -curMail->ackHdr.child || curr->parent != currentThread->space->pid); curr = curr->next){}
                curr->next = new(std::nothrow) FamilyNode(-curMail->ackHdr.child, curMail->ackHdr.pageID, NULL, -1);
                curr = curr->next;
            }
            memset(buffer, '\0', sizeof(buffer));
            int j = 0;
            char c = curMail->data[0];
            while(c != '\n'){
                if(c=='\n'){
                    break;
                }
                buffer[j]=c;
                j++;
                c = curMail->data[j];
            }
            curr->exit = atoi(buffer);
            memset(buffer, '\0', sizeof(buffer));
            curr->death->V();
            forking->V();
        } else if (curMail->ackHdr.migrateFlag == 3){
            // FamilyNode *curr;
            // forking->P();
            // curr = root;
            // while

        } else if(curMail->ackHdr.migrateFlag == 0){//Server is requesting a process


            //First Checkpoint the Process
            //Thread *t = currentThread;
            //ASSERT(false);
            //interrupt->SetLevel(oldLevel);
            Thread* removeThread = scheduler->StealUserThread();
            
            ASSERT(removeThread->space != NULL);
            char* filename = "migckpt";//This should not be a fixed filename, should be "t[netname] [counter]"
            if(!fileSystem->Create(filename, 16)){
                ASSERT(false);
            }
            open = fileSystem->Open(filename);
            
            removeThread->space->writeBackDirty();
            oldLevel = interrupt->SetLevel(IntOff);
            open->Write("#Checkpoint\n", 12);
            //Write the pid to the file, for nExit!!
            len = sprintf(buffer, "%d\n", removeThread->space->pid);
            open->Write(buffer, len);
            for(int i = 0; i < NumTotalRegs; i++){
                reg = machine->ReadRegister(i);
                len = sprintf(buffer, "%d\n", reg);
                open->Write(buffer,len);
                memset(buffer, '\0', sizeof(buffer));//could cause issues if I am not using sizeof correctly
            }
            interrupt->SetLevel(oldLevel);
            numPages = removeThread->space->getNumPages();
            len = sprintf(buffer, "%d\n", numPages);
            open->Write(buffer,len);

            for(int i=0;i<numPages;i++){//Should write the contents of all the pages
                synchDisk->ReadSector(removeThread->space->revPageTable[i].physicalPage, pageBuf);
                open->Write(pageBuf, 128);
            }
            //Now we kill it, making sure to V on its parent so when it joins it doesn't get stuck
            //There may be concurrency issues here, logically it should be fine since we turned interupts off, and slept the only running proc.
            //Might not need to P on the forking semaphore

            // forking->P();
            curr = root;
            outAckHdr.pageID = removeThread->space->pid;
            FamilyNode *curr;
            for(curr = root; curr != NULL; curr = curr->next){
                if (outAckHdr.pageID == curr->parent){
                    curr->migrated = 1;
                }
                else if(outAckHdr.pageID == curr->child){
                    curr->migrated = 2;
                }
            }

            for(curr = root; curr != NULL && curr->child != outAckHdr.pageID; curr = curr->next){}
            // fprintf(stderr, "oldPID: %d\n", removeThread->space->pid);
            // while(curr->child != removeThread->space->pid && curr->next !=NULL){
            //     curr = curr->next;  // Iterate to find the correct semphore to V
            // }
            // if(curr->child != removeThread->space->pid){
            //     ASSERT(false);
            // }
            // else{
            //     curr->exit = 42;
            //     forking->V();
            //     curr->death->V();
                
                
            //     // ASSERT(false);
                
            // }
            chillBrother->P();

            removeThread->space->remDiskPages();
            chillBrother->V();  
            delete removeThread->space;
            removeThread->Murder();
            // ASSERT(false);
            //Now it is time to send back to the server what the file name is so it can pass it on to the target client
            msgCTR->P();
            msgctr++;
            msgID=msgctr;
            msgCTR->V(); 
            outPktHdr.to = server;   
            outMailHdr.to = server;
            //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
            outMailHdr.from = 1;//1; 
            // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
            outMailHdr.length = 128; // had a plus 1 here before?????????
            outAckHdr.totalSize = 1;// size/MaxMailSize ; 
            outAckHdr.curPack = 0;
            outAckHdr.messageID = msgID;
            if(curr != NULL){
                outAckHdr.migrateFlag = curr->parent;
            }


            memset(pageBuf, '\0', sizeof(pageBuf));
            int k;
            for(k = 0; filename[k] != '\0' && k < 128; k++){
                pageBuf[k] = filename[k];
            }
            for(;k < 128; k++){
                pageBuf[k]='\0';
            }
            mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            //postOffice->SendThings(mail, 1);
            postOffice->Send(outPktHdr, outMailHdr, outAckHdr, mail->data);
            delete mail;
            message = postOffice->GrabMessage(1);
            curNode = message->head;
            curMail = curNode->cur;
            int parent = curMail->ackHdr.pageID;
            for(curr = root; curr != NULL; curr = curr->next){
                if(parent == curr->parent && curr->touched && curr->migrated == 1){
                    // Do equivalent of exit here
                    msgCTR->P();
                    msgctr++;
                    msgID=msgctr;
                    msgCTR->V(); 
                    outPktHdr.to = server;   
                    outMailHdr.to = netname;
                    outMailHdr.from = 0;// 1; 
                    outMailHdr.length = 5; 
                    outAckHdr.totalSize = 1;
                    outAckHdr.curPack = 0;
                    outAckHdr.messageID = msgID;
                    outAckHdr.migrateFlag = curr->exit;
                    // whence = machine->ReadRegister(4);
                    outAckHdr.pageID = curr->child;
                    outAckHdr.child = curr->parent;
                    curr->migrated = 3;

                    // Right now I am just passing in the buffer from one message to another, this might
                    // need to change to deep copying it over to a new buffer due to bad things happening... No you don't the deep copy happens
                    // inside the mail packet.

                    mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
                    postOffice->SendThings(mail, 0);
                }
            }

            forking->V();


        }
        else if(curMail->ackHdr.migrateFlag == 1){// Server is sending a process
            // First we should get the process name from the message
            // Essentially do what is done in progtest, first must grab the registers from the checkpoint file though
            // Create new addrspace with the ckpt constructor
            // Don't forget to run a migspace->RestoreState();
            // We are going to make this a context switch as well since there is presumably another thread running
            // ASSERT(false);
            open = fileSystem->Open(curMail->data);
            AddrSpace *migspace;
            int PID, i, j, oldPID;
            char c;

            if(open == NULL){
                ASSERT(false);
            }
            Thread *mig = new(std::nothrow) Thread("migthread");
            //Grab the registers and set them in the new thread
            
            forking->P();// Dubious ********************
            pid++;
            PID = pid;
            open->Read(pageBuf, 12);
            //#Checkpoint Check
            if(strncmp(pageBuf, "#Checkpoint\n", 12)){ASSERT(false);}
            //OrigPid
            j = 0; 
            while(open->Read(&c, 1)){
                if(c=='\n'){break;}
                buffer[j]=c;
                j++;
                if(j>19){fprintf(stderr, "numpages: %s", buffer);ASSERT(false);}
            }
            oldPID = atoi(buffer);
            memset(buffer, '\0', sizeof(buffer));
            //Registers
            for(i=0;i<NumTotalRegs;i++){
                j=0;
                while(open->Read(&c, 1)){
                    if(c=='\n'){break;}
                    buffer[j]=c;
                    j++;
                    if(j>19){ASSERT(false);}
                } 
                j = atoi(buffer);
                memset(buffer, '\0', sizeof(buffer));
                //machine->WriteRegister(i, j);
                mig->userRegisters[i] = j;
            }
            // Grab the numpages form the file
            j = 0; 
            while(open->Read(&c, 1)){
                if(c=='\n'){break;}
                buffer[j]=c;
                j++;
                if(j>19){fprintf(stderr, "numpages: %s", buffer);ASSERT(false);}
            }
            numPages = atoi(buffer);
            memset(buffer, '\0', sizeof(buffer));
            // Create the AddrSpace and assign it to the thread

            migspace = new(std::nothrow) AddrSpace(open, numPages, PID);
            mig->space = migspace;
            
            // lololol we should handle parent child relation somehow, lololol
            curr = root;
            while(curr->next !=NULL){
                curr = curr->next;  // Iterate to find the correct semphore to V
            }
            curr->next = new(std::nothrow) FamilyNode(migspace->pid, 0, migspace, -1);
            curFTN = foreignRoot;
            while(curFTN->next != NULL){
                curFTN = curFTN->next;
            }
            // curFTN->next = new(std::nothrow) ForeignThreadNode(oldPID, migspace->pid);
            fprintf(stderr, "oldPID: %d\n", oldPID);
            // mig->migrate = true;

            mig->migrate = 1;
            mig->Fork(migForkHandler, 42); // it is the meaning of life afterall :)

             
            // fork off the thread and let it run, let it fly, let it be free, let it gallop accross the field of tortilla chips
            // under the streams of melty cheese, past the great jalopeno outcropings, over the meadows of salsa
            //
            forking->V();
            // If I am re-understanding fork, only one thread should be getting back to this point.... 
            // So now we should alert the server that we have just started up the process it sent over
            msgCTR->P();
            msgctr++;
            msgID=msgctr;
            msgCTR->V(); 
            outPktHdr.to = server;   
            outMailHdr.to = server;
            //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
            outMailHdr.from = 1;//1; 
            // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
            outMailHdr.length = 128; // had a plus 1 here before?????????
            outAckHdr.totalSize = 1;// size/MaxMailSize ; 
            outAckHdr.curPack = 0;
            outAckHdr.messageID = msgID;
            outAckHdr.migrateFlag = 2;
            mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            postOffice->Send(outPktHdr, outMailHdr, outAckHdr, mail->data);
            fileSystem->Remove("migckpt");
            delete mail;
            // machine->Run();
            // postOffice->SendThings(mail, 1);

        }
        else{//Error case, should send a message back to the server since it is currently waiting on a message of some kind...?
            ASSERT(false);
            msgCTR->P();
            msgctr++;
            msgID=msgctr;
            msgCTR->V(); 
            outPktHdr.to = server;   
            outMailHdr.to = server;
            //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
            outMailHdr.from = 1;//1; 
            // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
            outMailHdr.length = 128; // had a plus 1 here before?????????
            outAckHdr.totalSize = 1;// size/MaxMailSize ; 
            outAckHdr.curPack = 0;
            outAckHdr.messageID = msgID;
            outAckHdr.migrateFlag = -1;

            mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
            postOffice->Send(outPktHdr, outMailHdr, outAckHdr, mail->data);
            delete mail;


            ASSERT(false);
        }/*
        oldLevel = interrupt->SetLevel(IntOff);

        interrupt->SetLevel(oldLevel);*/

    }
}

//----------------------------------------------------------------------
// nExitHandler
//  Used to handle the issue of network exit and joins, should go through
//  ForeignThreadNode to find the old pid and match to the new, should then
//  fork off a nExitJoinHandler.
//----------------------------------------------------------------------


static void
migrationHelper(int a){
    migrationHandler();
}
static void
PageStuffHandler(int a)
{
    Pager(a);
}


static void TimeoutHandlerHelper(int arg)
{ TimeoutHandler(); }

//----------------------------------------------------------------------
// Initialize
//  Initialize Nachos global data structures.  Interpret command
//  line arguments in order to determine flags for the initialization.  
// 
//  "argc" is the number of command line arguments (including the name
//      of the command) -- ex: "nachos -d +" -> argc = 3 
//  "argv" is an array of strings, one for each command line argument
//      ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------
void
Initialize(int argc, char **argv)
{
    // TIMEOUT =  10000000;
    int argCount;
    char *debugArgs = (char *)""; 
    bool randomYield = false;

#ifdef USER_PROGRAM
    bool debugUserProg = false; // single step user program
#endif
#ifdef FILESYS_NEEDED
    bool format = false;    // format disk
#endif
#ifdef NETWORK
    double rely = 1;        // network reliability
    netname = 0;        // UNIX socket name
#endif
    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
    argCount = 1;
    if (!strcmp(*argv, "-d")) {
        if (argc == 1)
        debugArgs = (char *) "+";   // turn on all debug flags
        else {
            debugArgs = *(argv + 1);
            argCount = 2;
        }
    } else if (!strcmp(*argv, "-rs")) {
        ASSERT(argc > 1);
        RandomInit(atoi(*(argv + 1)));  // initialize pseudo-random
                        // number generator
        randomYield = false;
        argCount = 2;
    }
#ifdef USER_PROGRAM
    if (!strcmp(*argv, "-s"))
        debugUserProg = true;
#endif
#ifdef FILESYS_NEEDED
    if (!strcmp(*argv, "-f"))
        format = true;
#endif
#ifdef NETWORK
    if (!strcmp(*argv, "-n")) {
        ASSERT(argc > 1);
        rely = atof(*(argv + 1));
        argCount = 2;
    } 
    else if (!strcmp(*argv, "-m")) {
        ASSERT(argc > 1);
        netname = atoi(*(argv + 1));
        argCount = 2;
    }
    else if (!strcmp(*argv, "-S")) {
        ASSERT(argc > 1);
        server = atoi(*(argv + 1));
        argCount = 2;
    }
    else if (!strcmp(*argv, "-C")) {
        ASSERT(argc > 1);
        char* client_str = *(argv + 1);
        int count = 0;
        for (int i = 0; client_str[i] != '\0'; i++) {
            if (client_str[i] != ',') {
                clients[count] = atoi(&client_str[i]);
                count++;
            }
        }
        argCount = 2;
    }

#endif
    }

    DebugInit(debugArgs);           // initialize DEBUG messages
    stats = new(std::nothrow) Statistics();         // collect statistics
    interrupt = new(std::nothrow) Interrupt;            // start up interrupt handling
    scheduler = new(std::nothrow) Scheduler();      // initialize the ready queue
    //if (randomYield)                // start the timer (if needed)
    //timer = new(std::nothrow) Timer(TimerInterruptHandler, 0, randomYield);
    
    threadToBeDestroyed = NULL;

    // We didn't explicitly allocate the current thread we are running in.
    // But if it ever tries to give up the CPU, we better have a Thread
    // object to save its state. 
    allThreads = new List;
    migThreads = new List;
    currentThread = new(std::nothrow) Thread("main");       
    currentThread->setStatus(RUNNING);

    interrupt->Enable();
    CallOnUserAbort(Cleanup);           // if user hits ctl-C
    pid = 0;
    msgctr = 0;
    timeoutctr = 0;
    root = new(std::nothrow) FamilyNode(pid, pid, NULL, -1);
    foreignRoot = new(std::nothrow) ForeignThreadNode(NULL, NULL, NULL, NULL, NULL);
#ifdef USER_PROGRAM
    machine = new(std::nothrow) Machine(debugUserProg); // this must come first
    synchConsole = new(std::nothrow) SynchConsole("synch console");
    bitMap = new(std::nothrow) BitMap(NumPhysPages);
    forking = new(std::nothrow) Semaphore("forking", 1);
    RandomInit(100);
    
    // bitMap->Print();
#endif

    // synchDisk = new(std::nothrow) SynchDisk("DISK");
    diskBitMap = new(std::nothrow) BitMap(NumSectors);
    diskPages = new(std::nothrow) diskEntry*[NumSectors];
    ramPages = new(std::nothrow) ramEntry*[NumPhysPages];
    chillBrother = new(std::nothrow) Semaphore("chillBrother", 1);
    execing = new(std::nothrow) Semaphore("execing", 1);
    forkexecing = new(std::nothrow) Semaphore("forkexecing", 1);

    

    for(int i = 0; i < NumPhysPages; i++){
        ramPages[i] = new(std::nothrow) ramEntry(-1, Free, -1, NULL);
    }
    for(int i = 0; i < NumSectors; i++){
        diskPages[i] = new(std::nothrow) diskEntry(Free);
    }
    commutator = 0;

#ifdef FILESYS

#endif

#ifdef FILESYS_NEEDED
    fileSystem = new(std::nothrow) FileSystem(format);
#endif

#ifdef NETWORK
    postOffice = new(std::nothrow) PostOffice(netname, rely, 10);
    mailboxes = new(std::nothrow) BitMap(10);
    msgCTR = new(std::nothrow) Semaphore("msgCTR", 1);
    msgctr = 0;
    timeoutctr = 0;

    // timeout = new(std::nothrow) Thread("timeout");
    // timeout->Fork(TimeoutHandlerHelper, 0);

    timer2 = new(std::nothrow) Timer(TimerInterruptHandler2, 0, randomYield);
#endif

#ifndef NETWORK
    synchDisk = new(std::nothrow) SynchDisk("DISK");
#else

    sprintf(diskname,"DISK_%d",netname);
    synchDisk = new(std::nothrow) SynchDisk(diskname);
    if(server == -1){//You are the server
        mailboxes->Mark(netname);
        for(int u = 0; clients[u] != -1; u++){
            Thread *pagingThread = new (std::nothrow) Thread("server paging Thread");
            mailboxes->Mark(clients[u]);
            ASSERT(clients[u] != 0);
            pagingThread->Fork(PageStuffHandler, clients[u]);
        }
        activeClientListSem = new(std::nothrow) Semaphore("activeClientListSem", 1);
        migrationSem = new(std::nothrow) Semaphore("migrationSem", 1);
    }
    else{//You are a client
        mailboxes->Mark(0);
        mailboxes->Mark(1);
        Thread *migrationThread = new (std::nothrow) Thread("client migration thread");
        migrationThread->Fork(migrationHelper, 42);
    }
#endif
    

}

//----------------------------------------------------------------------
// Cleanup
//  Nachos is halting.  De-allocate global data structures.
//----------------------------------------------------------------------
void
Cleanup()
{
    printf("\nCleaning up...\n");
//      for(int i = 0; i < NumPhysPages; i++){
  //  fprintf(stderr, "RamPage %d for pid %d and vPage %d", i, ramPages[i]->pid, ramPages[i]->vPage);
  //  if(ramPages[i]->head->current != NULL){
   //   fprintf(stderr, " validity is %s  and dirt is %s", ramPages[i]->head->current->pageTable[ramPages[i]->vPage].valid?"true":"false", ramPages[i]->head->current->pageTable[ramPages[i]->vPage].dirty?"true":"false");
   // }
   // fprintf(stderr, "\n");
//  }
#ifdef NETWORK
    delete postOffice;
    delete msgCTR;
    delete timeoutTimer;
#endif
    
#ifdef USER_PROGRAM
    delete machine;
    delete synchConsole;
    delete forking;
#endif

#ifdef FILESYS_NEEDED
    delete fileSystem;
#endif

    delete synchDisk; 
    delete chillBrother;
    delete execing;
    delete forkexecing;
#ifdef FILESYS
   
#endif
    
    delete timer;
    delete scheduler;
    delete interrupt;
    
    Exit(0);
}
#endif
