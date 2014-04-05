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


#ifdef USER_PROGRAM
    bool debugUserProg = false;	// single step user program
#endif
#ifdef FILESYS_NEEDED
    bool format = false;	// format disk
#endif
#ifdef NETWORK
    double rely = 1;		// network reliability
    int netname = 0;		// UNIX socket name
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
static void
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
    int netname = 0;        // UNIX socket name
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
    currentThread = new(std::nothrow) Thread("main");       
    currentThread->setStatus(RUNNING);

    interrupt->Enable();
    CallOnUserAbort(Cleanup);           // if user hits ctl-C
    pid = 0;
    msgctr = 0;
    timeoutctr = 0;
    root = new(std::nothrow) FamilyNode(pid, pid, NULL);
#ifdef USER_PROGRAM
    machine = new(std::nothrow) Machine(debugUserProg); // this must come first
    synchConsole = new(std::nothrow) SynchConsole("synch console");
    bitMap = new(std::nothrow) BitMap(NumPhysPages);
    forking = new(std::nothrow) Semaphore("forking", 1);
    RandomInit(100);
    
    // bitMap->Print();
#endif

    synchDisk = new(std::nothrow) SynchDisk("DISK");
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
    timeout = new(std::nothrow) Thread("timeout");
    timeout->Fork(TimeoutHandlerHelper, 0);
    timer2 = new(std::nothrow) Timer(TimerInterruptHandler2, 0, false);
#endif

#ifndef NETWORK
    synchDisk = new(std::nothrow) SynchDisk("DISK");
#else
    sprintf(diskname,"DISK_%d",netname);
    synchDisk = new(std::nothrow) SynchDisk(diskname);

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
