#ifndef CHANGED
// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#ifdef USE_TLB

//----------------------------------------------------------------------
// HandleTLBFault
//      Called on TLB fault. Note that this is not necessarily a page
//      fault. Referenced page may be in memory.
//
//      If free slot in TLB, fill in translation info for page referenced.
//
//      Otherwise, select TLB slot at random and overwrite with translation
//      info for page referenced.
//
//----------------------------------------------------------------------

void
HandleTLBFault(int vaddr)
{
  int vpn = vaddr / PageSize;
  int victim = Random() % TLBSize;
  int i;

  stats->numTLBFaults++;

  // First, see if free TLB slot
  for (i=0; i<TLBSize; i++)
    if (machine->tlb[i].valid == false) {
      victim = i;
      break;
    }

  // Otherwise clobber random slot in TLB

  machine->tlb[victim].virtualPage = vpn;
  machine->tlb[victim].physicalPage = vpn; // Explicitly assumes 1-1 mapping
  machine->tlb[victim].valid = true;
  machine->tlb[victim].dirty = false;
  machine->tlb[victim].use = false;
  machine->tlb[victim].readOnly = false;
}

#endif

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which) {
      case SyscallException:
	switch (type) {
	  case SC_Halt:
            DEBUG('a', "Shutdown, initiated by user program.\n");
            interrupt->Halt();
          default:
	    printf("Undefined SYSCALL %d\n", type);
	    ASSERT(false);
	}
#ifdef USE_TLB
      case PageFaultException:
	HandleTLBFault(machine->ReadRegister(BadVAddrReg));
	break;
#endif
      default: ;
    }
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
// exception.cc 
//  Entry point into the Nachos kernel from user programs.
//  There are two kinds of things that can cause control to
//  transfer back to here from user code:
//
//  syscall -- The user code explicitly requests to call a procedure
//  in the Nachos kernel.  Right now, the only function we support is
//  "Halt".
//
//  exceptions -- The user code does something that the CPU can't handle.
//  For instance, accessing memory that doesn't exist, arithmetic errors,
//  etc.  
//
//  Interrupts (which can also cause control to transfer from user
//  code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "console.h"
#include "synch.h"

#ifdef USE_TLB

//----------------------------------------------------------------------
// HandleTLBFault
//      Called on TLB fault. Note that this is not necessarily a page
//      fault. Referenced page may be in memory.
//
//      If free slot in TLB, fill in translation info for page referenced.
//
//      Otherwise, select TLB slot at random and overwrite with translation
//      info for page referenced.
//
//----------------------------------------------------------------------

void
HandleTLBFault(int vaddr)
{
  int vpn = vaddr / PageSize;
  int victim = Random() % TLBSize;
  int i;

  stats->numTLBFaults++;

  // First, see if free TLB slot
  for (i=0; i<TLBSize; i++)
    if (machine->tlb[i].valid == false) {
      victim = i;
      break;
    }

  // Otherwise clobber random slot in TLB

  machine->tlb[victim].virtualPage = vpn;
  machine->tlb[victim].physicalPage = vpn; // Explicitly assumes 1-1 mapping
  machine->tlb[victim].valid = true;
  machine->tlb[victim].dirty = false;
  machine->tlb[victim].use = false;
  machine->tlb[victim].readOnly = false;
}

#endif

//----------------------------------------------------------------------
// ExceptionHandler
//  Entry point into the Nachos kernel.  Called when a user program
//  is executing, and either does a syscall, or generates an addressing
//  or arithmetic exception.
//
//  For system calls, the following is the calling convention:
//
//  system call code -- r2
//    arg1 -- r4
//    arg2 -- r5
//    arg3 -- r6
//    arg4 -- r7
//
//  The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//  "which" is the kind of exception.  The list of possible exceptions 
//  are in machine.h.
//----------------------------------------------------------------------

OpenFile** fileDescriptors = new (std::nothrow) OpenFile*[16]; // Only 16 open files allowed at a time****  First two are console input and console output.
static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int) { readAvail->V(); }
static void WriteDone(int) { writeDone->V(); }

// int *fileDescriptors[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// int start = 0;
// for(int j = 0; j < 16; j++){
  // fileDescriptors[j] = start;
// }
void
ExceptionHandler(ExceptionType which)
{
    console = new(std::nothrow) Console(NULL, NULL, ReadAvail, WriteDone, 0);
    readAvail = new(std::nothrow) Semaphore("read avail", 0);
    writeDone = new(std::nothrow) Semaphore("write done", 0);
    int type = machine->ReadRegister(2);
    int whence;
    int size;
    char* stringArg;
    OpenFile* open;
    int descriptor = -1;
    int incrementPC;
    // int *ptr[MAX];

  switch (which) {
      case SyscallException:
  switch (type) {
    case SC_Halt:
            DEBUG('a', "Shutdown, initiated by user program.\n");
            interrupt->Halt();
            break;
    case SC_Exit:
            DEBUG('a', "Exit\n");
            break;
    case SC_Join:
            DEBUG('a', "Join\n");
            break;
    case SC_Create:
            DEBUG('a', "Create\n");
            stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters****
            whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
            DEBUG('a',"String starts at address %d in user VAS\n", whence);
            for (int i=0; i<127; i++)
              if ((stringArg[i]=machine->mainMemory[whence++]) == '\0') break;
            stringArg[127]='\0';
            DEBUG('a', "Argument string is <%s>\n",stringArg);
            ASSERT(fileSystem->Create(stringArg, 16));
            delete [] stringArg;
            incrementPC = machine->ReadRegister(NextPCReg)+4;
            machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
            machine->WriteRegister(NextPCReg, incrementPC);
            // Needed for checkpoint!
            break;
    case SC_Open:
            DEBUG('a', "Open\n");
            stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters****
            whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
            DEBUG('a',"String starts at address %d in user VAS\n", whence);
            for (int i=0; i<127; i++)
              if ((stringArg[i]=machine->mainMemory[whence++]) == '\0') break;
            stringArg[127]='\0';
            DEBUG('a', "Argument string is <%s>\n",stringArg);
            open = fileSystem->Open(stringArg);
            for(int i = 2; i < 16; i++){
              if(fileDescriptors[i] == NULL){
                descriptor = i;
                fileDescriptors[i] = open; // Bitten in the ass.
                i = 17;
              }
            }
            machine->WriteRegister(2, descriptor);
            DEBUG('a', "File descriptor for <%s> is %d\n", stringArg, descriptor);
            delete [] stringArg;
            incrementPC=machine->ReadRegister(NextPCReg)+4;
            machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
            machine->WriteRegister(NextPCReg, incrementPC);            // Needed for checkpoint!
            break;
    case SC_Read:
            DEBUG('a', "Read\n");
            size = machine->ReadRegister(5);
            whence = machine->ReadRegister(4);
            descriptor = machine->ReadRegister(6);
            stringArg = new (std::nothrow) char[size];
            if (descriptor != ConsoleInput && descriptor != ConsoleOutput) {
              open = fileDescriptors[descriptor];
              size = open->Read(stringArg, size);
              stringArg[size - 1] = '\0';
              for(int i=0; i < size; i++){
                if((machine->mainMemory[whence++] = stringArg[i]) == '\0') break;
              }
              machine->mainMemory[whence++] = '\0';
              printf("size: %d %s\n", size, stringArg);
              machine->WriteRegister(2, descriptor);  // Assume user allocates for null byte in char*
            }
            //else if (descriptor == ConsoleInput) {

           // }

            
            incrementPC=machine->ReadRegister(NextPCReg)+4;
            machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
            machine->WriteRegister(NextPCReg, incrementPC);

            /* Read "size" bytes from the open file into "buffer".  
             * Return the number of bytes actually read -- if the open file isn't
             * long enough, or if it is an I/O device, and there aren't enough 
             * characters to read, return whatever is available (for I/O devices, 
             * you should always wait until you can return at least one character).
            */
            // int Read(char *buffer, int size, OpenFileId id);

          // Needed for checkpoint!

          // File Sys Read
          //     int Read(char *into, int numBytes); // Read/write bytes from the file,
          // starting at the implicit position.
          // Return the # actually read/written,
          // and increment position in file.
            break;
    case SC_Write:
            //printf("here");
            DEBUG('a', "Write\n");
            size = machine->ReadRegister(5);
            stringArg = new(std::nothrow) char[size]; // Limit on name is 128 characters****
            whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
            descriptor = machine->ReadRegister(6);
            DEBUG('a',"String starts at address %d in user VAS\n", whence);
            for (int i=0; i<size; i++)
                if ((stringArg[i]=machine->mainMemory[whence++]) == '\0') break;
              stringArg[size]='\0';
              DEBUG('a', "Argument string is <%s>\n",stringArg);
            if (descriptor != ConsoleInput && descriptor != ConsoleOutput) {
              
              open = fileDescriptors[descriptor];
              open->Write(stringArg, size);
              // open = fileSystem->Open(stringArg);
              // for(int i = 2; i < 16; i++){
              //   if(fileDescriptors[i] == NULL){
              //     descriptor = i;
              //     fileDescriptors[i] = open;
              //     i = 17;
              //   }
              // }
              //machine->WriteRegister(2, descriptor);
              DEBUG('a', "File descriptor for <%s> is %d\n", stringArg, descriptor);
              delete [] stringArg; 
            }
            else if (descriptor == ConsoleOutput) {
              for (int i = 0; i < size; i++) {
                //readAvail->P();
                //printf("%s", (char*)stringArg[i]);
                console->PutChar(stringArg[i]);
                writeDone->P() ;  
              }
            }           
            incrementPC=machine->ReadRegister(NextPCReg)+4;
            machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
            machine->WriteRegister(NextPCReg, incrementPC);            
            // Needed for checkpoint!
            break;
    case SC_Close:
            DEBUG('a', "Close\n");
            descriptor = machine->ReadRegister(4);
            open = fileDescriptors[descriptor];
            fileDescriptors[descriptor] = NULL;
            delete( open );
            incrementPC=machine->ReadRegister(NextPCReg)+4;
            machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
            machine->WriteRegister(NextPCReg, incrementPC);            
            // Needed for checkpoint!
            break;
    case SC_Fork:
            DEBUG('a', "Fork\n");
            break;
          default:
      printf("Undefined SYSCALL %d\n", type);
      ASSERT(false);
  }
#ifdef USE_TLB
      case PageFaultException:
  HandleTLBFault(machine->ReadRegister(BadVAddrReg));
  break;
#endif
      default: ;
    }
}

#endif

#define SC_Exit   1
#define SC_Exec   2
#define SC_Join   3
#define SC_Create 4
#define SC_Open   5
#define SC_Read   6
#define SC_Write  7
#define SC_Close  8
#define SC_Fork   9