#ifndef CHANGED
// addrspace.h 
//  Data structures to keep track of executing user programs 
//  (address spaces).
//
//  For now, we don't keep any information about address spaces.
//  The user level CPU state is saved and restored in the thread
//  executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"

#define UserStackSize   1024  // increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);  // Create an address space,
          // initializing it with the program
          // stored in the file "executable"
    ~AddrSpace();     // De-allocate an address space

    void InitRegisters();   // Initialize user-level CPU registers,
          // before jumping to user code

    void SaveState();     // Save/restore address space-specific
    void RestoreState();    // info on a context switch 
    bool ReadMem(int addr, int size, int *value);
    bool WriteMem(int addr, int size, int value);
    ExceptionType Translate(int virtAddr, int* physAddr, int size, bool writing);

  private:
#ifndef USE_TLB
    TranslationEntry *pageTable;  // Assume linear page table translation
#endif          // for now!
    unsigned int numPages;    // Number of pages in the virtual 
          // address space
};

#endif // ADDRSPACE_H
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
// addrspace.h 
//  Data structures to keep track of executing user programs 
//  (address spaces).
//
//  For now, we don't keep any information about address spaces.
//  The user level CPU state is saved and restored in the thread
//  executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"

#define UserStackSize   1024  // increase this as necessary!

class FileShield {
    public:         // Raw console
    int refcount;                    // with the interrupt handler
    OpenFile *file;

    FileShield(){
      refcount = 0;
    }

    void CopyFile(){
      refcount++;
    };

    int CloseFile(){
      refcount--;
      return refcount;
    }

    ~FileShield(){
      if(file != NULL){
        delete file;
      }
    }
    //Lock *lock;               // Only one read/write request
                    // can be sent to the disk at a time
};

class AddrSpace {
  public:
    // Thread *parent;
    // Thread *child;
    // Thread *sibling;
    int child;
    int parent;
    int sibling;
    int exit;
    // Semaphore* death;
    FileShield** fileDescriptors;
    AddrSpace(OpenFile *executable);  // Create an address space,
          // initializing it with the program
          // stored in the file "executable"
    AddrSpace(TranslationEntry *newPageTable, FileShield** avengers, int newNumPages); //Only use if you are forking a new 
          //process and you know the pages needed
          //and are going to copy in the physical pages
    ~AddrSpace();     // De-allocate an address space

    void InitRegisters();   // Initialize user-level CPU registers,
          // before jumping to user code
    void CopySpace();
    void SaveState();     // Save/restore address space-specific
    void RestoreState();    // info on a context switch 
    bool ReadMem(int addr, int size, int *value);
    bool WriteMem(int addr, int size, int value);
    ExceptionType Translate(int virtAddr, int* physAddr, int size, bool writing);
    unsigned int getNumPages();
    AddrSpace* newSpace();

  private:
#ifndef USE_TLB
    TranslationEntry *pageTable;  // Assume linear page table translation
#endif          // for now!
    unsigned int numPages;    // Number of pages in the virtual 
          // address space
};

#endif // ADDRSPACE_H
#endif
