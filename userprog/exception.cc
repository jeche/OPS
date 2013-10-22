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
    case SC_Create:/*Checks for -> Filename given is a single \0*/
            DEBUG('a', "Create\n");
            stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters****
            whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
            DEBUG('a',"String starts at address %d in user VAS\n", whence);
            int i;
            for (i=0; i<127; i++)
              if ((stringArg[i]=machine->mainMemory[whence++]) == '\0') break;
            if(i==0){DEBUG('a', "Invalid File Name: Must be longer than 0\n");interrupt->Halt();}//User puts a single \0 for the string name of the file, this should not be allowed
            stringArg[127]='\0';
            DEBUG('a', "Argument string is <%s>\n",stringArg);
            if(!fileSystem->Create(stringArg, 16)){DEBUG('a', "Create Failed\n");interrupt->Halt();}
            delete [] stringArg;
            incrementPC = machine->ReadRegister(NextPCReg)+4;
            machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
            machine->WriteRegister(NextPCReg, incrementPC);
            // Needed for checkpoint!
            break;
    case SC_Open:/*Checks for -> */
            DEBUG('a', "Open\n");
            stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters****
            whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
            DEBUG('a',"String starts at address %d in user VAS\n", whence);
            for (int i=0; i<127; i++)
              if ((stringArg[i]=machine->mainMemory[whence++]) == '\0') break;
            if(i==0){DEBUG('a', "Invalid File Name: Must be longer than 0\n");interrupt->Halt();}//Cannot have a file with 'no name'
            stringArg[127]='\0';
            DEBUG('a', "Argument string is <%s>\n",stringArg);
            open = fileSystem->Open(stringArg);
            if(open==NULL){
              DEBUG('a', "File Could not be Found, -1 returned"); 
              descriptor=-1;
              machine->WriteRegister(2, descriptor);
              
              delete [] stringArg;
              incrementPC=machine->ReadRegister(NextPCReg)+4;
              machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
              machine->WriteRegister(NextPCReg, incrementPC);            // Needed for checkpoint!
              break;
            }
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
            if (descriptor != ConsoleInput && descriptor != ConsoleOutput && descriptor < 16 && descriptor > ConsoleOutput) {
              open = fileDescriptors[descriptor];
              if(open == NULL){
                DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                // fprintf(stderr, "%d\n", descriptor);
                interrupt->Halt();
              }
              descriptor = 0;
              // while(descriptor == 0){
              size = open->Read(stringArg, size);
              // }
              // fprintf(stderr, "%d\n", descriptor);
              stringArg[size - 1] = '\0';
              for(int i=0; i < size; i++){
                if((machine->mainMemory[whence++] = stringArg[i]) == '\0') break;
              }
              machine->mainMemory[whence++] = '\0';
              DEBUG('a', "size: %d %s\n", size, stringArg);
              machine->WriteRegister(2, descriptor);  // Assume user allocates for null byte in char*
            }
            else if (descriptor == ConsoleInput) { // Deals with ConsoleInput

            }
            else if(descriptor == ConsoleOutput){

            }
            else{ // Deals with out of bounds of the array.
              DEBUG('a', "Invalid file descriptor.\n");
              interrupt->Halt();
            }
            incrementPC=machine->ReadRegister(NextPCReg)+4;
            machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
            machine->WriteRegister(NextPCReg, incrementPC);
            break;
    case SC_Write:
            DEBUG('a', "Write\n");
            size = machine->ReadRegister(5);
            if (size > 0){
              stringArg = new(std::nothrow) char[size];
              whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
              descriptor = machine->ReadRegister(6);
              DEBUG('a',"String starts at address %d in user VAS\n", whence); // Translation should go somewhere around here.
              for (int i=0; i<size; i++)
                  if ((stringArg[i]=machine->mainMemory[whence++]) == '\0') break;
                stringArg[size]='\0';
                DEBUG('a', "Argument string is <%s>\n",stringArg);
              if (descriptor != ConsoleInput && descriptor != ConsoleOutput && descriptor < 16 && descriptor > ConsoleOutput) {
                open = fileDescriptors[descriptor];
                if(open == NULL){
                  DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                  // fprintf(stderr, "%d\n", descriptor);
                  interrupt->Halt();
                }                
                open->Write(stringArg, size);
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
              else if (descriptor == ConsoleInput){

              }
              else{
                DEBUG('a', "Invalid file descriptor.\n");
                interrupt->Halt();
              }           
            }
            incrementPC=machine->ReadRegister(NextPCReg)+4;
            machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
            machine->WriteRegister(NextPCReg, incrementPC);            
            break;
    case SC_Close:/*Cases Handled -> invalid openfileid, and no file associated with the openfileid*/
            DEBUG('a', "Close\n");
            descriptor = machine->ReadRegister(4);
            if(descriptor<0||descriptor>15){DEBUG('a', "Invalid OpenFileId"); interrupt->Halt();}//invalid openfileid
            open = fileDescriptors[descriptor];
            if(open==NULL){//no file is associated with the openfileid
              DEBUG('a', "No OpenFile is associated with the given OpenFileId");
              incrementPC=machine->ReadRegister(NextPCReg)+4;
              machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
              machine->WriteRegister(NextPCReg, incrementPC);
            }
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