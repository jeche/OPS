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

#define UserStackSize   2048  // increase this as necessary!

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


#define UserStackSize   4096  // increase this as necessary!

// Class used as a barrier for files to keep track of the number of times that the file is referenced
class FileShield {
    public:         
    int refcount; 
    int inOut;  // Used to check if ConsoleInput or ConsoleOutput have been duped                
    OpenFile *file;

    FileShield(){
      refcount = 0;
      inOut = 0;
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
};


class AddrSpace {
  public:
    unsigned int numPages;
    FileShield** fileDescriptors;
    int pid;
    int enoughSpace;
    TranslationEntry *pageTable;
    //bool cow;
    AddrSpace(OpenFile *executable, int PID);  // Create an address space,
          // initializing it with the program
          // stored in the file "executable"
    AddrSpace(TranslationEntry *newPageTable, TranslationEntry *newRevPageTable, FileShield** avengers, int newNumPages, int newEnoughSpace, bool isCow, int PID); //Only use if you are forking a new 
          //process and you know the pages needed
          //and are going to copy in the physical pages
    AddrSpace(OpenFile *chkpt, int numpages, int PID);
    ~AddrSpace();     // De-allocate an address space

    void InitRegisters();   // Initialize user-level CPU registers,
          // before jumping to user code
    void CopySpace();
    void Clean();
    void SaveState();     // Save/restore address space-specific
    void RestoreState();    // info on a context switch 
    ExceptionType Translate(int virtAddr, int* physAddr, int size, bool writing);
//DC    ExceptionType TranslateDisk(int virtAddr, int* physAddr, int size, bool writing);
    unsigned int getNumPages();
    AddrSpace* newSpace(int PID);
    AddrSpace* cowSpace(int PID);
    bool ReadMem(int addr, int size, int *value);
    bool WriteMem(int addr, int size, int value);
    TranslationEntry *revPageTable;
    int findReplacement2();
    void pageFaultHandler2(int bAddr);
    bool writeBackDirty();
    void printAllPages();
    void remDiskPages();
    int copyCowPage(int rOPage);
    void printAllDiskPages();
    bool isCowAddr();
  private:
    
#ifndef USE_TLB
    //TranslationEntry *pageTable;  // Assume linear page table translation
#endif          // for now!
    bool clean;
    


        // Number of pages in the virtual 
          // address space
};

#endif // ADDRSPACE_H
#endif
