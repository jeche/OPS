#ifndef CHANGED
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
#include "synch.h"

#include "addrspace.h"
#include "machine.h"

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

// OpenFile** fileDescriptors = new (std::nothrow) OpenFile*[16]; // Only 16 open files allowed at a time****  First two are console input and console output.
// static Semaphore *readAvail= new(std::nothrow) Semaphore("read avail", 0);
// static Semaphore *writeDone= new(std::nothrow) Semaphore("write done", 0);
// static void ReadAvail(int) { readAvail->V(); }
// static void WriteDone(int) { writeDone->V(); }
// static Console *console;

// int *fileDescriptors[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// int start = 0;
// for(int j = 0; j < 16; j++){
  // fileDescriptors[j] = start;
// }

// ExceptionType
// Translate(int virtAddr, int* physAddr, int size, bool writing) 
// {
//     int i;
//     unsigned int vpn, offset;
//     TranslationEntry *entry;
//     unsigned int pageFrame;

//     DEBUG('a', "\tTranslate 0x%x, %s: ", virtAddr, writing ? "write" : "read");

// // check for alignment errors
//     if (((size == 4) && (virtAddr & 0x3)) || ((size == 2) && (virtAddr & 0x1))){
//   DEBUG('a', "alignment problem at %d, size %d!\n", virtAddr, size);
//   return AddressErrorException;
//     }
    
//     // we must have either a TLB or a page table, but not both!
//     ASSERT(machine->tlb == NULL || machine->pageTable == NULL); 
//     ASSERT(machine->tlb != NULL || machine->pageTable != NULL); 

// // calculate the virtual page number, and offset within the page,
// // from the virtual address
//     vpn = (unsigned) virtAddr / PageSize;
//     offset = (unsigned) virtAddr % PageSize;
    
//     if (machine->tlb == NULL) {    // => page table => vpn is index into table
//   if (vpn >= machine->pageTableSize) {
//       DEBUG('a', "virtual page # %d too large for page table size %d!\n", 
//       virtAddr, machine->pageTableSize);
//       return AddressErrorException;
//   } else if (!machine->pageTable[vpn].valid) {
//       DEBUG('a', "Page table miss, virtual address  %d!\n", 
//       virtAddr);
//       return PageFaultException;
//   }
//   entry = &machine->pageTable[vpn];
//     } else {
//         for (entry = NULL, i = 0; i < TLBSize; i++)
//           if (machine->tlb[i].valid && ((unsigned)machine->tlb[i].virtualPage == vpn)) {
//     entry = &machine->tlb[i];      // FOUND!
//     break;
//       }
//   if (entry == NULL) {        // not found
//           DEBUG('a', "*** no valid TLB entry found for this virtual page!\n");
//           return PageFaultException;    // really, this is a TLB fault,
//             // the page may be in memory,
//             // but not in the TLB
//   }
//     }

//     if (entry->readOnly && writing) { // trying to write to a read-only page
//   DEBUG('a', "%d mapped read-only at %d in TLB!\n", virtAddr, i);
//   return ReadOnlyException;
//     }
//     pageFrame = entry->physicalPage;

//     // if the pageFrame is too big, there is something really wrong! 
//     // An invalid translation was loaded into the page table or TLB. 
//     if (pageFrame >= NumPhysPages) { 
//   DEBUG('a', "*** frame %d > %d!\n", pageFrame, NumPhysPages);
//   return BusErrorException;
//     }
//     entry->use = false;   // set the use, dirty bits
//     if (writing)
//   entry->dirty = true;
//     *physAddr = pageFrame * PageSize + offset;
//     ASSERT((*physAddr >= 0) && ((*physAddr + size) <= MemorySize));
//     DEBUG('a', "phys addr = 0x%x\n", *physAddr);
//     return NoException;
// }

void CopyRegs(int k){
  // // fprintf(stderr, "Oh noes %d\n", (int) currentThread);
  // // fprintcurrentThrea
  // // currentThread->SaveUserState();
  // currentThread->RestoreUserState();
  // machine->WriteRegister(2, 0);
  // // t->SaveUserState();
  // // fprintf(stderr, "oh hey there\n");
  // // t->RestoreUserState();
  // Thread* t = (Thread*)k;
  // t->SaveUserState();
  // fprintf(stderr, "\nCOPYREGS %d\n", (int)currentThread);
  int incrementPC;
  currentThread->RestoreUserState();
  currentThread->space->RestoreState();
  machine->WriteRegister(2, 0);
  // fprintf(stderr, "oh hey there\n");
  incrementPC=machine->ReadRegister(NextPCReg)+4;
  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
  machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
  machine->WriteRegister(NextPCReg, incrementPC);            // Needed for checkpoint!
  machine->Run();
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    int whence;
    int size;
    int toOutput = 0;
    int fromInput = 0;
    char* stringArg;
    OpenFile* open;
    int descriptor = -1;
    int incrementPC;
    // int size2;
    char whee;
    int i, j;
    AddrSpace *newSpacer;
    Thread *t;
    Thread *prev;
    Semaphore* die;
    IntStatus oldLevel;
    FamilyNode *curr;

    //Exec w/ args variables
    int sp, len, argcount, herece;
    int argvAddr[16];


    // fprintf(stderr, "which: %d type: %d\n", (int)which, type);
  switch (which) {
      case SyscallException:
      switch (type) {
        case SC_Halt:
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;
        case SC_Exit:
                DEBUG('a', "Exit\n");
                curr = root;
                while(curr->child != (int) currentThread&& curr->next !=NULL){
                  curr = curr->next;
                  // fprintf(stderr, "joined %d, %d\n", curr->parent , curr->child);
                }
                // die = (Semaphore*)currentThread->space->death;
                whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
                curr->exit = whence;
                // currentThread->space->exit2 = whence;
                // currentThread->space->Clean();
                // fprintf(stderr, "joined %d, %d\n", curr->parent , curr->child);
                curr->death->V();
                // fprintf(stderr, "hello current thread is %d\n", (int)currentThread);

                // currentThread->space->c();
                // oldLevel = interrupt->SetLevel(IntOff);
                delete currentThread->space;
                currentThread->Finish();
                // (void) interrupt->SetLevel(oldLevel);

                // incrementPC=machine->ReadRegister(NextPCReg)+4;
                // machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                // machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                // machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_Join:
                DEBUG('a', "Join\n");
                // bitMap->Print();
                whence = machine->ReadRegister(4);
                curr = root;
                // fprintf(stderr, "whence %d\n", whence);
                while(( curr->child != whence ||curr->parent != (int)currentThread) && curr->next != NULL){
                  curr = curr->next;
                  // fprintf(stderr, "done join %d, %d\n", curr->parent , curr->child);
                }
                if(curr->parent != (int)currentThread && curr->child !=whence){
                  machine->WriteRegister(2, -1);
                }
                else{
                  // fprintf(stderr, "done join %d, %d\n", curr->parent , curr->child);
                  curr->death->P();
                  machine->WriteRegister(2, curr->exit);
                }
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_Create:/*Checks for -> Filename given is a single \0*/
                DEBUG('a', "Create\n");
                stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters****
                whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
                DEBUG('a',"String starts at address %d in user VAS\n", whence);
                for (i=0; i<127; i++)
                  currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);  // Pretending this works.
                  // if ((stringArg[i]=machine->mainMemory[whence++]) == '\0') break; *****
                if(i==0){DEBUG('a', "Invalid File Name: Must be longer than 0\n");interrupt->Halt();}//User puts a single \0 for the string name of the file, this should not be allowed
                stringArg[127]='\0';
                DEBUG('a', "Argument string is <%s>\n",stringArg);
                if(!fileSystem->Create(stringArg, 16)){DEBUG('a', "Create Failed\n");interrupt->Halt();}
                delete [] stringArg;
                // interrupt->Halt();
                incrementPC = machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_Open:/*Checks for -> */
                DEBUG('a', "Open\n");
                stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters****
                whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
                DEBUG('a',"String starts at address %d in user VAS\n", whence);
                for (i=0; i<127; i++){
                  currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);  // Pretending this works.
                  if(stringArg[i] == '\0') break;
                }
                // if ((stringArg[i]=machine->mainMemory[whence++]) == '\0') break; *****
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
                  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                  machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                  machine->WriteRegister(NextPCReg, incrementPC);            // Needed for checkpoint!
                  break;
                }else{                  
                for(i = 2; i < 16; i++){
                  if(currentThread->space->fileDescriptors[i] == NULL){
                    descriptor = i;
                    currentThread->space->fileDescriptors[i] = new FileShield();
                    currentThread->space->fileDescriptors[i]->file = open; // Bitten in the ass.
                    currentThread->space->fileDescriptors[i]->CopyFile();
                    i = 17;
                  }
                }
                machine->WriteRegister(2, descriptor);
                DEBUG('a', "File descriptor for <%s> is %d\n", stringArg, descriptor);
                delete [] stringArg;
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);            // Needed for checkpoint!
                break;}
          break;
        case SC_Read:
                DEBUG('a', "Read\n"); // Please fix.
                size = machine->ReadRegister(5);
                whence = machine->ReadRegister(4);
                descriptor = machine->ReadRegister(6);
                if(size > 0){
                  stringArg = new (std::nothrow) char[size];
                  if (descriptor == ConsoleInput) {
                    if (currentThread->space->fileDescriptors[descriptor]->inOut == -1)
                      fromInput = 1;
                  }
                  if (!fromInput && descriptor != ConsoleOutput && descriptor < 16) {
                    if(currentThread->space->fileDescriptors[descriptor] == NULL){
                      DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                      // fprintf(stderr, "%d\n", descriptor);
                      machine->WriteRegister(2, -1);  // Assume user allocates for null byte in char*
                    }
                    else{
                      open = currentThread->space->fileDescriptors[descriptor]->file;
                      if(open == NULL){
                        DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                        // fprintf(stderr, "%d\n", descriptor);
                        machine->WriteRegister(2, -1);  // Assume user allocates for null byte in char*
                      }
                      else{
                        descriptor = 0;
                        size = open->Read(stringArg, size);
                        if(size != 1){
                          stringArg[size - 1] = '\0';
                        }
                        for(i=0; i < size; i++){
                          currentThread->space->WriteMem(whence++, sizeof(char), stringArg[i]);
                          if(stringArg[i] == '\0') break;
                        }
                        // machine->mainMemory[whence++] = '\0'; *****
                        DEBUG('a', "%s\n", stringArg);
                        DEBUG('a', "size: %d %s\n", size, stringArg);
                        machine->WriteRegister(2, size);  // Assume user allocates for null byte in char*
                      }
                    }
                  }
                  else if (fromInput) { // Deals with ConsoleInput
                    DEBUG('a', "size: %d %c\n", size, stringArg);
                    whee = synchConsole->GetChar();
                    currentThread->space->WriteMem(whence++, sizeof(char), whee);
                    // machine->mainMemory[whence++] = whee; *****
                    DEBUG('a', "size: %d %c\n", size, stringArg);
                    machine->WriteRegister(2, 1);
                  }
                  else{ // Deals with out of bounds of the array.
                    DEBUG('a', "Invalid file descriptor.\n");
                    machine->WriteRegister(2, -1);  // Assume user allocates for null byte in char*
                  }
                  delete stringArg;
                }
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_Write:
                DEBUG('a', "Write\n"); // Please fix.
                // fprintf(stderr, "\nTHREAD WRITING IS: %d\n", (int) currentThread);
                // Issue name: Oh God Why?
                // For some ungodly reason size decides to be 0 immediately after the for loop.  Why?  No idea.  If we have a different size
                // it for some reason then works and sets the other different size to 0.  Another fix we found... was to just reset size every
                // time we need to use it.
                size = machine->ReadRegister(5);
                if (size > 0){
                  stringArg = new(std::nothrow) char[size];
                  whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
                  descriptor = machine->ReadRegister(6);
                  DEBUG('a',"String starts at address %d in user VAS\n", whence); // Translation should go somewhere around here.
                  //Deals with the case when output has been closed and a file has been duped there.
                  if (descriptor == ConsoleOutput) {
                    if (currentThread->space->fileDescriptors[descriptor]->inOut == -1)
                      toOutput = 1;
                  }
                  if(size != 1 && !toOutput){
                    for (i=0; i<size; i++){
                      currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);
                      // fprintf(stderr, "DOPE %c\", stringArg[i]);
                      if(stringArg[i] == '\0')break;
                    }
                    // if ((stringArg[i]=machine->mainMemory[whence++]) == '\0') break; *****
                  
                      stringArg[size]='\0';
                  }
                  else if(size == 1 && !toOutput){
                    currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[0]);
                  }
                    DEBUG('a', "Argument string is <%s>\n",stringArg);
                  if (descriptor != ConsoleInput && !toOutput && descriptor < 16) {
                    // fprintf(stderr, "Yayyyy\n");
                    open = currentThread->space->fileDescriptors[descriptor]->file;
                    // fprintf(stderr, "Yayyyy\n");
                    if(open == NULL){
                      DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                      // fprintf(stderr, "%d\n", descriptor);
                      interrupt->Halt();
                    }                
                    open->Write(stringArg, size);
                    if(size == 1)
                    DEBUG('a', "File descriptor for <%s> is %d\n", stringArg, descriptor);
                    
                  }
                  else if (toOutput) {
                    // fprintf(stderr, "SIZEABLE %d\n", size);
                    // size2 = size;
                    for (i = 0; i < size; i++) {
                      size = machine->ReadRegister(5);
                      // fprintf(stderr, "for loop: %d\n", size);
                      // fprintf(stderr, "for loop: %d\n", i);
                      if(currentThread->space->ReadMem(whence++, sizeof(char), (int *)&whee)){
                        // fprintf(stderr, "truth! %c\n", whee);
                        // fprintf(stderr, "i %d", size);
                      }
                      // fprintf(stderr, "\nTHREAD WRITING IS: %d\n", (int) currentThread);
                      // whee = machine->mainMemory[whence++]; *****
                      synchConsole->PutChar(whee);
                      // fprintf(stderr, "%c", whee);
                      // size = size2;
                      // fprintf(stderr, "sizeable %d\n", size);

                    }
                    // fprintf(stderr, "%c", whee);
                    // fprintf(stderr, "i %d", size);

                    synchConsole->PutChar('\0');
                  }
                  else if (descriptor == ConsoleInput){
                      machine->WriteRegister(2, -1);
                      //I should never be here
                  }
                  else{
                    DEBUG('a', "Invalid file descriptor.\n");
                    interrupt->Halt();
                  } 
                  delete [] stringArg;           
                }
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);            
                break;
        case SC_Close:/*Cases Handled -> invalid openfileid, and no file associated with the openfileid*/
                DEBUG('a', "Close\n");
                descriptor = machine->ReadRegister(4);
                if(descriptor<0||descriptor>15){DEBUG('a', "Invalid OpenFileId"); interrupt->Halt();}//invalid openfileid
                whence = currentThread->space->fileDescriptors[descriptor]->CloseFile();

                if(whence < 0){//no file is associated with the openfileid
                  DEBUG('a', "No OpenFile is associated with the given OpenFileId");
                  // incrementPC=machine->ReadRegister(NextPCReg)+4;
                  // machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                  // machine->WriteRegister(NextPCReg, incrementPC);
                }
                // if(whence == 0){
                  // delete currentThread->space->fileDescriptors[descriptor];
                // }
                // currentThread->space->fileDescriptors[descriptor] = NULL;
                if(whence == 0){
                // delete
                delete (currentThread->space->fileDescriptors[descriptor]->file);
                // delete( open );
                }
                currentThread->space->fileDescriptors[descriptor] = NULL;

                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);            
                // Needed for checkpoint!
                break;
        case SC_Fork:
        /* Fork creates a clone (the child) of the calling user process (the
 * parent). The parent gets the SpaceId of the child as the return value
 * of the Fork; the child gets a 0 return value. If there is an error that
 * prevents the creation of the child, the parent gets a -1 return value.
 */
                DEBUG('a', "Fork\n");
                curr = root;
                while(curr->next != NULL){
                  curr = curr->next;
                }
                t = new(std::nothrow) Thread("clone");
                curr->next = new(std::nothrow) FamilyNode(t);
                // fprintf(stderr, "\nPARENT: %d, CHILD %d\n", (int) currentThread, (int)t);
                newSpacer = currentThread->space->newSpace();
                t->space = newSpacer;
                // t->space->parent = (int)currentThread;
                // currentThread->space->child = (int)t;
                // machine->WriteRegister(2, 0);
                t->SaveUserState();
                currentThread->SaveUserState();
                t->Fork(CopyRegs, (int)currentThread);
                // fprintf(stderr, "\nPOST FORK\n");
                machine->WriteRegister(2, (int)t);
                currentThread->SaveUserState();
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);    
                break;
        case SC_Exec:
        /* Run the executable, stored in the Nachos file "name", in the context
 * of the current address space. Should not return unless there is an
 * error, in which case a -1 is returned.
 */   
                DEBUG('a', "Exec\n");
                argcount = 1;
                stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters****
                whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
                DEBUG('a',"String starts at address %d in user VAS\n", whence);
                for (i=0; i<127; i++){
                  currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);  // Pretending this works.
                  if(stringArg[i] == '\0') break;
                }
                if(i==0){DEBUG('a', "Invalid File Name: Must be longer than 0\n");interrupt->Halt();}//Cannot have a file with 'no name'
                stringArg[127]='\0';
                DEBUG('a', "Argument string is <%s>\n",stringArg);
                open = fileSystem->Open(stringArg);
                ASSERT(open !=NULL);
                newSpacer = new AddrSpace(open);
                delete open;
                for(i = 0; i < 16; i++){
                  if(currentThread->space->fileDescriptors[i]!= NULL){
                    newSpacer->fileDescriptors[i] = currentThread->space->fileDescriptors[i];
                  }
                }
                whence = machine->ReadRegister(5);
                
                newSpacer->InitRegisters();
                sp = machine->ReadRegister(StackReg);

                len = strlen(stringArg) + 1;
                sp -= len;
                for(i = 0; i < len; i++){
                  newSpacer->WriteMem(sp + i, sizeof(char), stringArg[i]);
                }
                argvAddr[0] = sp; // if things are screwy make them explain this.

                
                for(i = 1; i < 16; i++){
                  memset(stringArg, 0, sizeof(stringArg));
                  currentThread->space->ReadMem(whence, sizeof(int), &herece);
                  if (herece == 0){
                    break;
                  }
                  for(j = 0; j < 127; j++){
                    currentThread->space->ReadMem(herece++, sizeof(char), (int *)&stringArg[j]);  // Pretending this works.
                    if(stringArg[j] == '\0') break;
                  }
                  DEBUG('a', "STRINGARG %s\n",stringArg);

                  len = strlen(stringArg) + 1;
                  sp -= len;

                  for(j = 0; j < len; j++){
                    newSpacer->WriteMem(sp+j, sizeof(char), stringArg[j]);
                  }
                  argvAddr[i] = sp;
                  whence = whence + sizeof(int);

                }
                argcount = i;
                sp = sp & ~3; // this supposedly aligns the stack on 4 byte boundary for integer values. Do. Not. Trust.
                DEBUG('a', "argcount %d\n", argcount);
                sp -= sizeof(int) * argcount;

                for(i = 0; i < argcount; i++){
                   newSpacer->WriteMem(sp + i*4, sizeof(int), argvAddr[i]);
                }

                delete currentThread->space;
                
                currentThread->space = newSpacer;
                
                // currentThread->SaveUserState();
                newSpacer->RestoreState();

                machine->WriteRegister(4, argcount);
                machine->WriteRegister(5, sp);

                machine->WriteRegister(StackReg, sp - argcount*4);
 
                machine->Run();//pretending this works
                break;
        case SC_Dup:
                descriptor = machine->ReadRegister(4);
                if(descriptor<0||descriptor>15){DEBUG('a', "Invalid OpenFileId"); interrupt->Halt();}//invalid openfileid //!!!!!!!!!!!!!!!!!!!!!!!!!!!!NO HALTING REMOVE THIS!!!!!!!!!!!!!!!!
                if(currentThread->space->fileDescriptors[descriptor] == NULL){DEBUG('a', "No OpenFile is associated with the given OpenFileId"); interrupt->Halt();}
                for (i = 0; i < 16; i++) {
                  if (currentThread->space->fileDescriptors[i] == NULL) {
                    currentThread->space->fileDescriptors[i] = currentThread->space->fileDescriptors[descriptor];
                    currentThread->space->fileDescriptors[i]->CopyFile();
                    currentThread->space->fileDescriptors[i]->inOut = 0;
                    break;
                  }
                }
                if (i == 16) {
                  //There's no open space.  Cry.
                  interrupt->Halt();
                }
                DEBUG('a', "File descriptor returned by Dup: %d\n", i);
                machine->WriteRegister(2, i);
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
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
#define SC_Dup    10
