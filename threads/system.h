#ifndef CHANGED
// system.h 
//	All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"
#include <new>

// Initialization and cleanup routines
extern void Initialize(int argc, char **argv); 	// Initialization,
						// called before anything else
extern void Cleanup();				// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;			// the thread holding the CPU
extern Thread *threadToBeDestroyed;  		// the thread that just finished
extern Scheduler *scheduler;			// the ready list
extern Interrupt *interrupt;			// interrupt status
extern Statistics *stats;			// performance metrics
extern Timer *timer;				// the hardware alarm clock

#ifdef USER_PROGRAM
#include "machine.h"
extern Machine* machine;	// user program memory and registers
#endif

#ifdef FILESYS_NEEDED 		// FILESYS or FILESYS_STUB 
#include "filesys.h"
extern FileSystem  *fileSystem;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk   *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice* postOffice;
#endif

#endif // SYSTEM_H
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
// system.h 
//	All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"
#include "console.h"
#include "synch.h"
#include "bitmap.h"
//#include "disk.h"
#include <new>

class SynchConsole {
    private:
    Console *console;   
    bool used;
    Condition* notBusy;
    public:         // Raw console
    Semaphore *readAvail;       // To synchronize requesting thread 
                    // with the interrupt handler
    Semaphore *writeDone;
    Lock *busy;

  public:
    static void ReadAvail(int arg) { 
        SynchConsole* cons = (SynchConsole*)arg;
        cons->readAvail->V(); 
    };
    static void WriteDone(int arg) {
        SynchConsole* cons = (SynchConsole*)arg; 
        cons->writeDone->V(); 
    };

    SynchConsole(char* name){
        console = new(std::nothrow) Console(NULL, NULL, ReadAvail, WriteDone, (int) this);
        readAvail = new(std::nothrow) Semaphore("read avail", 0);
        writeDone = new(std::nothrow) Semaphore("write done", 0);
        busy = new(std::nothrow) Lock("busyLock");
        notBusy = new(std::nothrow) Condition("notBusy");

        used = false; 

    }; 
    ~SynchConsole(){
        delete console;
        delete readAvail;
        delete writeDone;
        delete busy;
    };          
    
    void PutChar(char ch){
        busy->Acquire();
        while(used){
            notBusy->Wait(busy);
        }
        used = true;
        console->PutChar(ch);
        writeDone->P();
        used = false;
        notBusy->Broadcast(busy);
        
        busy->Release();
    };
                        //output ch on console; delay if busy
    char GetChar(){
        busy->Acquire();
        while(used){
            notBusy->Wait(busy);
        }
        used = true;
        readAvail->P();
        char c = console->GetChar();
        used = false;
        notBusy->Broadcast(busy);
        busy->Release();
        return c;
    };
                        //return character input on console; if none avaliable, delay until it is input
    void CheckCharAvailable(){

    };

    void WriteDone(){

    };


  
};

enum Status { Free,           // No page here yet!
             InUse,      // Currently in use but not being replaced
             MarkedForReplacement,    // Found by replacement algorithm to be removed
             CowPage
};



class ramEntry{ 
private:
    Status status;
public:
    int pid;
    int refcount;
    int vPage;
    AddrSpace *head;
    ramEntry(int PID, Status soso, int VPage, AddrSpace *first){
        pid = PID;
        status = soso;
        vPage = VPage;
        head = first;
    };
    ~ramEntry(){
    };

    Status getStatus(){
        return status;
    };
    void setStatus(Status s){
        status = s;
    };

};

class diskEntry{
private:
    Semaphore *conDiskPage;
    Status status;
    int refcount;
    AddrSpace *addr1;
    AddrSpace *addr2;
public:
    
    diskEntry(Status s){
        conDiskPage = new(std::nothrow) Semaphore("conDiskPage", 1);
        addr1=NULL;
        addr2=NULL;
        status=s;
        refcount=0;
    };
    ~diskEntry(){
        delete conDiskPage;
    };

    Status getStatus(){
        return status;
    };
    void setStatus(Status s){
        conDiskPage->P();
        status=s;
        conDiskPage->V();
    };
    int getRefCount(){
        return refcount;
    };
    AddrSpace* cowSignal(AddrSpace *addr){
        conDiskPage->P();
        if(addr == addr1){
            addr2->cow = false;
            conDiskPage->V();
            return addr2;
        }
        else if(addr == addr2){
            addr1->cow = false;
            conDiskPage->V();
            return addr1;
        }
        else{
            fprintf(stderr, "CowSignal: AddrSpace given not associated with diskEntry\n");
            DEBUG('a', "CowSignal: AddrSpace given not associated with diskEntry\n");
            conDiskPage->V();
            return NULL;
        }

    }
    AddrSpace* otherAddr(AddrSpace *addr){
        conDiskPage->P();
        if(addr == addr1){
            conDiskPage->V();
            return addr2;
        }
        else if(addr == addr2){
            conDiskPage->V();
            return addr1;
        }
        else{
            fprintf(stderr, "otherAddr: AddrSpace given not associated with diskEntry\n");
            DEBUG('a', "otherAddr: AddrSpace given not associated with diskEntry\n");
            conDiskPage->V();
            return NULL;
        }
    };
    void addAddr(AddrSpace *addr){
        //fprintf(stderr, "Adding an Addr: %x\n", addr);
        conDiskPage->P();
        if(addr1==NULL){
            //fprintf(stderr, "first time\n");
            addr1=addr;
            refcount++;
        }
        else if(addr2==NULL){
            //fprintf(stderr, "second time\n");
            addr2=addr;
            refcount++;
        }
        else{
            fprintf(stderr, "AddrSpace already in addr2\n");
            DEBUG('a', "AddrSpace already in addr2\n");
        }
        if(refcount==0){
            status=Free;
        }
        else if(refcount==1){
            status=InUse;
        }
        else{
            status=CowPage;
        }
        conDiskPage->V();
    };
    void removeAddr(AddrSpace *addr){
        conDiskPage->P();
        if(addr == addr1){
            addr1=addr2;
            addr2=NULL;
            refcount--;
        }
        else if(addr == addr2){
            addr2=NULL;
            refcount--;
        }
        else{
            fprintf(stderr, "Address not in diskPage entry\n");
            DEBUG('a', "Address not in diskPage entry\n");
        }
        if(refcount==0){
            status=Free;
        }
        else if(refcount==1){
            status=InUse;
        }
        else{
            status=CowPage;
        }
        conDiskPage->V();
    };
    void displayPage(){
        
        fprintf(stdout, "refcount: %d\n", refcount);
        fprintf(stdout, "AddrSpace1: %d\n", addr1);
        fprintf(stdout, "AddrSpace2: %d\n", addr2);
        
        fprintf(stdout, "Status: %d\n", status);
        
        
    };

};

// Initialization and cleanup routines
extern void Initialize(int argc, char **argv); 	// Initialization,
						// called before anything else
extern void Cleanup();				// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;			// the thread holding the CPU
extern Thread *threadToBeDestroyed;  		// the thread that just finished
extern Scheduler *scheduler;			// the ready list
extern Interrupt *interrupt;			// interrupt status
extern Statistics *stats;			// performance metrics
extern Timer *timer;				// the hardware alarm clock



class FamilyNode{
public:
    int parent;
    bool touched;
    int child;
    int exit;
    Semaphore* death;
    FamilyNode* next;

    FamilyNode(int t, int p){
        death = new(std::nothrow) Semaphore("deadKid", 0);
        parent = p;
        child = t;
        touched = false;
        next=NULL;
    };

    ~FamilyNode(){
        delete death;
        if(next != NULL){
            delete next;
        }
    };
};


#ifdef USER_PROGRAM
#include "machine.h"
extern Machine* machine;	// user program memory and registers
extern Timer *timer2;
extern SynchConsole *synchConsole;
extern Semaphore *forking;
extern BitMap *bitMap;
extern FamilyNode* root;
extern unsigned int pid;
#endif
#ifdef FILESYS_NEEDED 		// FILESYS or FILESYS_STUB 
#include "filesys.h"
extern FileSystem  *fileSystem;
#endif

#include "synchdisk.h"
extern SynchDisk   *synchDisk;
extern BitMap *diskBitMap;
extern ramEntry **ramPages;
extern diskEntry **diskPages;
extern int commutator;
extern Semaphore *chillBrother;
extern Semaphore *execing;
extern Semaphore *forkexecing;
#ifdef FILESYS 
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice* postOffice;
#endif

#endif // SYSTEM_H
#endif
