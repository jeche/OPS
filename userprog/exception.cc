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
// }

void CopyRegs(int k){
  int incrementPC;
  currentThread->RestoreUserState();
  currentThread->space->RestoreState();
  machine->WriteRegister(2, 0);
  incrementPC=machine->ReadRegister(NextPCReg)+4;
  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
  machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
  machine->WriteRegister(NextPCReg, incrementPC);            // Needed for checkpoint!
  machine->Run();
}

//----------------------------------------
// findReplacement
// finds a page to replace
//----------------------------------------

int findReplacement(){
  // int found;
  // int startPos = commutator;
  // // Sweep through and check for free pages
  // TranslationEntry *pageTableEntry = NULL;
  // // Fancy clock part -- machine seems to set use bits to false when a page has been used -- not sure why this happens 
  // // -- Changed the clock algorithm to check for use bit true instead of use bit false 
  // while(1) {
  //  for (commutator; commutator < NumPhysPages; commutator++) {
  //     if (ramPages[commutator]->status == Free) {
  //       found = commutator;
  //       ramPages[commutator]->status = MarkedForReplacement;
  //       commutator++;        
  //       return found;
  //     }
  //   }
  //   for (commutator = 0; commutator < startPos; commutator++) {
  //     if (ramPages[commutator]->status == Free) {
  //       found = commutator;
  //       ramPages[commutator]->status = MarkedForReplacement;
  //       commutator++;
        
  //       return found;
  //     }
  //   }


  //   // First scan -- Look for use bit true and dirty bit false
  //   for (commutator; commutator < NumPhysPages; commutator++) {
  //     int vPage = ramPages[commutator]->vPage;
  //     if (ramPages[commutator]->head->current == NULL) {
  //       found = commutator;
  //       ramPages[commutator]->status = MarkedForReplacement;
  //       commutator++;
        
  //       return found;
  //     }
  //     else {
  //       pageTableEntry = &(ramPages[commutator]->head->current->pageTable[vPage]);

  //       if (pageTableEntry->use && !pageTableEntry->dirty && ramPages[commutator]->status != MarkedForReplacement) {
  //         found = commutator;
  //         ramPages[commutator]->head->current->pageTable[vPage].valid = false;  
  //         ramPages[commutator]->head->current->pageTable[vPage].physicalPage = -1;  
  //         ramPages[commutator]->status = MarkedForReplacement;
  //         commutator++;
          
  //         return found;
  //       }
  //     }
  //   }
  //   for (commutator = 0; commutator < startPos; commutator++) {
  //     int vPage = ramPages[commutator]->vPage;
  //     if (ramPages[commutator]->head->current == NULL) {
  //       found = commutator;
  //       ramPages[commutator]->status = MarkedForReplacement;
  //       commutator++;
        
  //       return found;
  //     }
  //     else {
  //       pageTableEntry = &(ramPages[commutator]->head->current->pageTable[vPage]);
  //       if (pageTableEntry->use && !pageTableEntry->dirty && ramPages[commutator]->status != MarkedForReplacement) {
  //         found = commutator;
  //         ramPages[commutator]->head->current->pageTable[vPage].valid = false;  
  //         ramPages[commutator]->head->current->pageTable[vPage].physicalPage = -1;  
  //         ramPages[commutator]->status = MarkedForReplacement;
  //         commutator++;
  //         return found;
  //       }
  //     }
  //   }

  //   // Second scan -- Look for use bit true and dirty bit true -- change use bits as we go
  //   for (commutator; commutator < NumPhysPages; commutator++) {
  //     int vPage = ramPages[commutator]->vPage;
  //     if (ramPages[commutator]->head->current == NULL) {
  //       found = commutator;
  //       ramPages[commutator]->status = MarkedForReplacement;
  //       commutator++;
        
  //       return found;
  //     }
  //     else {
  //       pageTableEntry = &(ramPages[commutator]->head->current->pageTable[vPage]);
  //       if (pageTableEntry->use && pageTableEntry->dirty && ramPages[commutator]->status != MarkedForReplacement) {
  //         found = commutator;
  //         ramPages[commutator]->head->current->pageTable[vPage].valid = false;  
  //         ramPages[commutator]->head->current->pageTable[vPage].physicalPage = -1;  
  //         ramPages[commutator]->status = MarkedForReplacement;
  //         commutator++;
          
  //         return found;
  //       }
  //       // Set the use bit to true if it is not already
  //       pageTableEntry->use = true;
  //     }
  //   }
  //   for (commutator = 0; commutator < startPos; commutator++) {
  //     int vPage = ramPages[commutator]->vPage;
  //     if (ramPages[commutator]->head->current == NULL) {
  //       found = commutator;
  //       ramPages[commutator]->status = MarkedForReplacement;
  //       commutator++;
        
  //       return found;
  //     }
  //     else {
  //       pageTableEntry = &(ramPages[commutator]->head->current->pageTable[vPage]);
  //       if (pageTableEntry->use && pageTableEntry->dirty && ramPages[commutator]->status != MarkedForReplacement) {
  //         found = commutator;
  //         ramPages[commutator]->head->current->pageTable[vPage].valid = false;  
  //         ramPages[commutator]->head->current->pageTable[vPage].physicalPage = -1;  
  //         ramPages[commutator]->status = MarkedForReplacement;
  //         commutator++;
          
  //         return found;
  //       }
  //       // Set the use bit to true if it is not already
  //       pageTableEntry->use = true;
  //     }
  //   }
  // }
}


void outputRamPages(){
  for(int i = 0; i < NumPhysPages; i++){
    fprintf(stderr, "RamPage %d for pid %d and vPage %d", i, ramPages[i]->pid, ramPages[i]->vPage);
    if(ramPages[i]->head->current != NULL){
      fprintf(stderr, " validity is %s  and dirt is %s", ramPages[i]->head->current->pageTable[ramPages[i]->vPage].valid?"true":"false", ramPages[i]->head->current->pageTable[ramPages[i]->vPage].dirty?"true":"false");
    }
    fprintf(stderr, "\n");
  }
}

//----------------------------------------
// END findReplacement
//----------------------------------------
void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    int whence;
    int size;
    int toOutput = 0;
    int toInput = 0;
    int fromInput = 0;
    int fromOutput = 0;
    char* stringArg;
    OpenFile* open;
    int descriptor = -1;
    int incrementPC;
    char whee;
    int i, j;
    AddrSpace *newSpacer;
    Thread *t;
    // Semaphore* die;
    IntStatus oldLevel;
    FamilyNode *curr;
    // FamilyNode *prev;
    int vpn;
    //Exec w/ args variables
    int sp, len, argcount, herece;
    int argvAddr[16];
    Status stuffing;
    AddrSpace *oldSpacer;


  switch (which) {
      case SyscallException:
      switch (type) {
        case SC_Halt:
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;
        case SC_Exit:
                DEBUG('z', "Exit\n");
                curr = root;
                DEBUG('a', "Thread exiting %d.\n", currentThread->space->pid);
                forking->P();
                while(curr->child != currentThread->space->pid && curr->next !=NULL){
                  curr = curr->next;  // Iterate to find the correct semphore to V
                }
                if(curr->child != currentThread->space->pid){
                  DEBUG('a', "How the hell do you break an exit?\n");
                  fprintf(stderr, "How the hell do you break an exit?\n");
                }
                else{
                  whence = machine->ReadRegister(4); // whence is the exit value for the thread.
                  curr->exit = whence;
                  forking->V();
                  curr->death->V();
                  delete currentThread->space;
                  currentThread->Finish();
                  DEBUG('a', "Failed to exit.  Machine will now terminate.\n");
                }
                break;
        case SC_Join:
                DEBUG('a', "Join\n");
                whence = machine->ReadRegister(4);
                curr = root;
                forking->P();
                while(( curr->child != whence || curr->parent != currentThread->space->pid) && curr->next != NULL){
                  curr = curr->next;  // Iterate to find the correct semapohre to P on
                }
                
                if(curr->parent != currentThread->space->pid && curr->child != whence){
                  forking->V();
                  DEBUG('a', "Cannot find appropriate thread ID to join on.\n");
                  machine->WriteRegister(2, -1);  // If you cannot find the child return false.
                }
                else{
                  curr->touched = true;
                  forking->V();
                  DEBUG('a', "Parent %d, joining for %d.\n", curr->parent, curr->child);
                  curr->death->P(); // Wait for child to die.
                  machine->WriteRegister(2, curr->exit); // Return the exit value.
                }

                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_Create:/*Checks for -> Filename given is a single \0*/
                DEBUG('a', "Create\n");
                stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters
                whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
                // currentThread->space->ReadMem(whence, sizeof(char), (int *)&stringArg[0]);
                DEBUG('a',"String starts at address %d in user VAS\n", whence);
                for (i=0; i<127; i++){
                  currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);  // Pretending this works.
                }
                if(i==0){
                  DEBUG('a', "Invalid File Name: Must be longer than 0\n");
                  machine->WriteRegister(2, -1);  // If file name is invalid return -1.
                } // User puts a single \0 for the string name of the file, this should not be allowed
                stringArg[127]='\0';
                DEBUG('a', "Argument string is <%s>\n",stringArg);
                if(!fileSystem->Create(stringArg, 16)){
                  DEBUG('a', "Create Failed\n");
                  machine->WriteRegister(2, -1);  // If file name is invalid return -1.
                }
                delete [] stringArg;
                incrementPC = machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_Open:
                DEBUG('a', "Open\n");
                stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters
                whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string
                DEBUG('a',"String starts at address %d in user VAS\n", whence);
                // currentThread->space->ReadMem(whence, sizeof(char), (int *)&stringArg[0]);  // Pretending this works.
                for (i=0; i<127; i++){
                  currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);  // Pretending this works.
                  if(stringArg[i] == '\0') break;
                }
                if(i==0){
                  DEBUG('a', "Invalid File Name: Must be longer than 0\n");
                  machine->WriteRegister(2, -1);  // If file name is invalid return -1.
                }// Cannot have a file with 'no name'
                stringArg[127]='\0';
                DEBUG('a', "Argument string is <%s>\n",stringArg); //was j, fix this later
                open = fileSystem->Open(stringArg);
                if(open==NULL){
                  DEBUG('a', "File Could not be Found, -1 returned"); 
                  descriptor = -1;
                  machine->WriteRegister(2, descriptor);
                  
                  delete [] stringArg;
                  incrementPC=machine->ReadRegister(NextPCReg)+4;
                  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                  machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                  machine->WriteRegister(NextPCReg, incrementPC);            // Needed for checkpoint!
                  break;
                }
                else{                  
                  for(i = 2; i < 16; i++){
                    if(currentThread->space->fileDescriptors[i] == NULL){
                      descriptor = i;
                      forking->P();
                      currentThread->space->fileDescriptors[i] = new FileShield();
                      currentThread->space->fileDescriptors[i]->file = open;
                      currentThread->space->fileDescriptors[i]->CopyFile();
                      forking->V();
                      i = 17;
                    }
                  }
                  if (i == 18) {
                    machine->WriteRegister(2, descriptor);
                  }
                  else {
                    machine->WriteRegister(2, -1);
                  }
                  DEBUG('a', "File descriptor for <%s> is %d\n", stringArg, descriptor);
                  delete [] stringArg;
                  incrementPC=machine->ReadRegister(NextPCReg)+4;
                  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                  machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                  machine->WriteRegister(NextPCReg, incrementPC);            // Needed for checkpoint!
                  break;
                }
                break;
        case SC_Read:
                
                DEBUG('a', "Read\n");
                size = machine->ReadRegister(5);
                whence = machine->ReadRegister(4);
                descriptor = machine->ReadRegister(6);
                /*if(descriptor>=0&&descriptor<=15&&)*/

                  if(size > 0){
                    stringArg = new (std::nothrow) char[128];
                    if (descriptor == ConsoleInput) {
                      if (currentThread->space->fileDescriptors[descriptor]->inOut == -1)
                        fromInput = 1;
                    }
                    if (descriptor == ConsoleOutput) {
                      if (currentThread->space->fileDescriptors[descriptor]->inOut == -1)
                        fromOutput = 1;
                    }
                    if (!fromInput && !fromOutput && descriptor < 16 && descriptor >= 0) {
                      forking->P();
                      if(currentThread->space->fileDescriptors[descriptor] == NULL){
                        DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                        machine->WriteRegister(2, -1);  // Assume user allocates for null byte in char*
                      }
                      else{
                        open = currentThread->space->fileDescriptors[descriptor]->file;
                        if(open == NULL){
                          DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                          machine->WriteRegister(2, -1);  // Assume user allocates for null byte in char*
                        }
                        else{
                          descriptor = 0;
                          size = open->Read(stringArg, size);
                          // currentThread->space->WriteMem(whence, sizeof(char), stringArg[0]);
                          for(i=0; i < size; i++){
                            currentThread->space->WriteMem(whence++, sizeof(char), stringArg[i]);
                            if(stringArg[i] == '\0') break;
                          }
                          DEBUG('a', "%s\n", stringArg);
                          DEBUG('a', "size: %d %s\n", size, stringArg);
                          machine->WriteRegister(2, size);  // Assume user allocates for null byte in char*
                        }
                      }
                      forking->V();
                    }
                    else if (fromInput) { // Deals with ConsoleInput
                      DEBUG('a', "size: %d %c\n", size, stringArg);
                      for(i = 0; i < size; i++){
                        whee = synchConsole->GetChar();
                        // currentThread->space->WriteMem(whence, sizeof(char), whee);
                        currentThread->space->WriteMem(whence++, sizeof(char), whee);
                      }
                      machine->WriteRegister(2, i);
                    }
                    else{ // Deals with out of bounds of the array.
                      DEBUG('a', "Invalid file descriptor.\n");
                      machine->WriteRegister(2, -1);  // Assume user allocates for null byte in char*
                    }
                    delete[] stringArg;
                  }
                
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_Write:
                DEBUG('a', "Write\n");
                // fprintf(stderr,"WHEEEEEEEEEEEEEEEEEEEEEEE\n");
                forking->P();
                size = machine->ReadRegister(5);
                if (size > 0){
                  stringArg = new(std::nothrow) char[128];
                  whence = machine->ReadRegister(4);
                  descriptor = machine->ReadRegister(6);
                  DEBUG('a',"String starts at address %d in user VAS\n", whence);
                  // Deals with the case when output has been closed and a file has been duped there.
                  if (descriptor == ConsoleOutput) {
                    if (currentThread->space->fileDescriptors[descriptor]->inOut == -1)
                      toOutput = 1;
                  }
                  if (descriptor == ConsoleInput) {
                    if (currentThread->space->fileDescriptors[descriptor]->inOut == -1)
                      toInput = 1;
                  }
                  if(size != 1 && !toOutput && !toInput){
                    // currentThread->space->ReadMem(whence, sizeof(char), (int *)&stringArg[0]);
                    for (i=0; i<size; i++){
                      currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);
                      // if(stringArg[i] == '\0') break;
                    }
                      //stringArg[size]='\0';
                  }
                  else if(size == 1 && !toOutput && !toInput){
                    // currentThread->space->ReadMem(whence, sizeof(char), (int *)&stringArg[0]);
                    currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[0]);
                  }
                    DEBUG('a', "Argument string is <%s>\n",stringArg);
                  if (!toInput && !toOutput && descriptor < 16 && descriptor >= 0) {
                    open = currentThread->space->fileDescriptors[descriptor]->file;
                    if(open == NULL){
                      DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                      machine->WriteRegister(2, -1);
                    }
                    else{                
                      open->Write(stringArg, size);
                      // if(size == 1)
                      DEBUG('a', "File descriptor for <%s> is %d\n", stringArg, descriptor);
                    }
                  }
                  else if (toOutput) {
                    // currentThread->space->ReadMem(whence, sizeof(char), (int *)&whee);
                    for (i = 0; i < size; i++) {
                      size = machine->ReadRegister(5);
                      if(currentThread->space->ReadMem(whence++, sizeof(char), (int *)&whee)){}
                      synchConsole->PutChar(whee);

                    }
                    //synchConsole->PutChar('\0');
                  }
                  else if (toInput){ // Should this also check for toInput? *****
                      DEBUG('a', "Invalid file descriptor, cannot write to ConsoleInput.\n");
                      machine->WriteRegister(2, -1);
                  }
                  else{
                    DEBUG('a', "Invalid file descriptor.\n");
                    machine->WriteRegister(2, -1);
                  } 
                  stringArg[size] = '\0';
                  delete[] stringArg;
                }
                forking->V();
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);            
                break;
        case SC_Close:/*Cases Handled -> invalid openfileid, and no file associated with the openfileid*/
                DEBUG('a', "Close\n");
                descriptor = machine->ReadRegister(4);
                if(descriptor < 0 || descriptor > 15){
                  DEBUG('a', "Invalid OpenFileId"); 
                  machine->WriteRegister(2 , -1);
                }//invalid openfileid
                else if(currentThread->space->fileDescriptors[descriptor]==NULL){
                  machine->WriteRegister(2, -1);
                }
                else{
                  forking->P();
                  whence = currentThread->space->fileDescriptors[descriptor]->CloseFile();
                  forking->V();
                  if(whence < 0){//no file is associated with the openfileid
                    DEBUG('a', "No OpenFile is associated with the given OpenFileId\n");
                    machine->WriteRegister(2 , -1);
                  }
                  if(whence == 0){
                    delete (currentThread->space->fileDescriptors[descriptor]->file);
                  }
                  currentThread->space->fileDescriptors[descriptor] = NULL;
                }
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);            
                break;
        case SC_Fork:
                DEBUG('a', "Fork\n");
                if(root==NULL){
                  DEBUG('a', "Root for the family tree is nonexistent.\n");
                }
                curr = root;
                forking->P();
                pid++; // bump pid before adding kid
                size = pid;
                while(curr->next != NULL){
                  curr = curr->next;

                }
                       
                newSpacer = currentThread->space->newSpace(); // Create an AddrSpace for child
                newSpacer->pid = size; // give child's space a pid.
                if (newSpacer->enoughSpace == 0) {
                  // There was not enough space to create the child.  Return a -1 and delete the created addrspace
                  DEBUG('a', "Not enough space to fork child.\n");
                  machine->WriteRegister(2, -1);
                  pid--;
                  forking->V();
                }
                else {
                  t = new(std::nothrow) Thread("clone");
                  curr->next = new(std::nothrow) FamilyNode(newSpacer->pid, currentThread->space->pid);  // Add new parent child relation to family tree.
                  t->space = newSpacer; // Give child its brand new space.
                  t->SaveUserState(); // Write all current machine registers to userRegisters for child.
                  currentThread->SaveUserState(); // Save just in case the Fork gets weird.
                  forking->V();
                  t->Fork(CopyRegs, (int)currentThread); // Fork child.
                  machine->WriteRegister(2, newSpacer->pid); // Write the appropriate return val for parent
                  currentThread->SaveUserState(); // Save again in case of weirdness.
                }
                if(newSpacer->enoughSpace == 0){
                  delete newSpacer;
                }
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);  
                break;
        case SC_Exec:
                DEBUG('a', "Exec\n");
                argcount = 1;
                stringArg = new(std::nothrow) char[128];
                whence = machine->ReadRegister(4);
                DEBUG('a',"String starts at address %d in user VAS\n", whence);
                // currentThread->space->ReadMem(whence, sizeof(char), (int *)&stringArg[0]);
                for (i=0; i<127; i++){
                  currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);
                  if(stringArg[i] == '\0') break;
                }
                if(i==0){
                  DEBUG('a', "Invalid File Name: Must be longer than 0\n");
                  machine->WriteRegister(2, -1);  // Should exec really return or what.....? *****
                }// Cannot have a file with 'no name'
                stringArg[127]='\0';
                DEBUG('a', "Argument string is <%s>\n",stringArg);
                open = fileSystem->Open(stringArg);
                if(open == NULL){
                  DEBUG('a', "Invalid File Name, no such file exists.\n");
                  machine->WriteRegister(2, -1);  // Should exec really return or what.....? *****
                }// Cannot have a file with 'no name'
                else{
                  oldSpacer = currentThread->space;
                  newSpacer = new AddrSpace(open);
                  delete open;
                  newSpacer->pid = currentThread->space->pid; // Transfer pid
                  // fprintf(stderr, "PID = %d\n", newSpacer->pid);
                  
                  if (newSpacer->enoughSpace == 0) {
                    // There was not enough room, return a -1
                    DEBUG('a', "Not enough room to create new Address Space.\n");
                    machine->WriteRegister(2, -1);
                  }
                  else {
                    for(i = 0; i < 16; i++){
                      if(currentThread->space->fileDescriptors[i]!= NULL){
                        newSpacer->fileDescriptors[i] = currentThread->space->fileDescriptors[i];
                      }
                    }
                    whence = machine->ReadRegister(5);
                    
                    newSpacer->InitRegisters();
                    sp = machine->ReadRegister(StackReg);

                    for(i = 0; i < 16; i++){
                      memset(stringArg, 0, sizeof(stringArg));
                      currentThread->space = oldSpacer;
                      currentThread->space->RestoreState();
                      // currentThread->space->ReadMem(whence, sizeof(int), &herece);
                      currentThread->space->ReadMem(whence, sizeof(int), &herece);
                      if (herece == 0){
                        break;
                      }
                      // currentThread->space->ReadMem(herece, sizeof(char), (int *)&stringArg[0]);
                      for(j = 0; j < 127; j++){
                        // currentThread->space = oldSpacer;
                        // currentThread->space->RestoreState();
                        currentThread->space->ReadMem(herece++, sizeof(char), (int *)&stringArg[j]);
                        if(stringArg[j] == '\0') break;
                      }
                      DEBUG('a', "STRINGARG %s\n",stringArg);
                      // for( j = 0; j < 110; j++){
                        // fprintf(stderr, "PAGETABLE: %d %d\n", currentThread->space->revPageTable[j].physicalPage, currentThread->space->pid);
                      // }
                      len = strlen(stringArg) + 1;
                      sp -= len;

                      currentThread->space = newSpacer;
                      currentThread->space->RestoreState();
                      for(j = 0; j < len; j++){
                        // newSpacer->WriteMem(sp+j, sizeof(char), stringArg[j]);
                        newSpacer->WriteMem(sp+j, sizeof(char), stringArg[j]);
                      }
                      argvAddr[i] = sp;
                      whence = whence + sizeof(int);

                    }
                    argcount = i;
                    sp = sp & ~3;
                    DEBUG('a', "argcount %d\n", argcount);
                    sp -= sizeof(int) * argcount;

                    currentThread->space = newSpacer;
                    currentThread->space->RestoreState();
                    for(i = 0; i < argcount; i++){
                       // newSpacer->WriteMem(sp + i*4, sizeof(int), argvAddr[i]);
                       newSpacer->WriteMem(sp + i*4, sizeof(int), argvAddr[i]);
                    }
                    delete [] stringArg;

                    delete oldSpacer;
                    
                    currentThread->space = newSpacer;
                    
                    newSpacer->RestoreState();
                    // for( j = 0; j < 110; j++){
                        // fprintf(stderr, "PAGETABLE2: %d %d\n", currentThread->space->revPageTable[j].physicalPage, currentThread->space->pid);
                    // }
                    machine->WriteRegister(4, argcount);
                    machine->WriteRegister(5, sp);

                    machine->WriteRegister(StackReg, sp - (8 * 4));
                    machine->Run();
                  }
                }
                delete [] stringArg;
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);  
                break;
        case SC_Dup:
                descriptor = machine->ReadRegister(4);
                if(descriptor < 0 || descriptor > 15){
                  DEBUG('a', "Invalid OpenFileId.\n");
                  machine->WriteRegister(2, -1);
                }
                else if(currentThread->space->fileDescriptors[descriptor] == NULL){
                  DEBUG('a', "No OpenFile is associated with the given OpenFileId.\n");
                  machine->WriteRegister(2, -1);
                }
                else{
                  for (i = 0; i < 16; i++) {
                    if (currentThread->space->fileDescriptors[i] == NULL) {
                      currentThread->space->fileDescriptors[i] = currentThread->space->fileDescriptors[descriptor];
                      currentThread->space->fileDescriptors[i]->CopyFile();
                      currentThread->space->fileDescriptors[i]->inOut = 0;
                      break;
                    }
                  }
                  machine->WriteRegister(2, i);
                  if (i == 16) {
                    DEBUG('a', "No open space to DUP to.  Please close a file.\n");
                    machine->WriteRegister(2, -1);
                  }
                }
                // DEBUG('a', "File descriptor returned by Dup: %d\n", i);
                
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        default:
                printf("Undefined SYSCALL %d\n", type);
                ASSERT(false);
      }
      // #ifdef USE_TLB  PUT THIS BACK LATER.
      //   case PageFaultException:
      //   HandleTLBFault(machine->ReadRegister(BadVAddrReg));
      //   break;
      // #endif

        case PageFaultException:
          fromInput = bitMap->Find();
          oldLevel = interrupt->SetLevel(IntOff); // disable interrupts
          vpn =  machine->ReadRegister(BadVAddrReg) / PageSize;
          if(machine->pageTable[vpn].valid == false){
          stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters
          memset(stringArg, 0, sizeof(stringArg));
          synchDisk->ReadSector(currentThread->space->revPageTable[vpn].physicalPage, stringArg);
          if(fromInput == -1){
            descriptor = Random() % NumPhysPages;
          }else{
            descriptor = fromInput;
          }
            // fprintf(stderr, "DESCRIPTOR %d\n", descriptor);
            if(ramPages[descriptor]->head->current != NULL){
              incrementPC = ramPages[descriptor]->vPage;
              // ASSERT(ramPages[descriptor]->head->current->pageTable[incrementPC].valid);
              ramPages[descriptor]->head->current->pageTable[incrementPC].valid = false;
              if(ramPages[descriptor]->head->current->pageTable[incrementPC].dirty){
                // ASSERT(ramPages[descriptor]->head->current->pageTable[incrementPC].dirty);
                // ASSERT(!ramPages[descriptor]->head->current->pageTable[incrementPC].valid);
                synchDisk->WriteSector(ramPages[descriptor]->head->current->revPageTable[incrementPC].physicalPage, &machine->mainMemory[descriptor * PageSize]);
              }
              ramPages[descriptor]->head->current->pageTable[incrementPC].dirty = false;
            }

            for(j = 0; j < 128; j++){
              machine->mainMemory[descriptor * PageSize + j] = stringArg[j];
            }

            currentThread->space->pageTable[vpn].valid = true;
            currentThread->space->pageTable[vpn].dirty = false;
            // currentThread->space->pageTable[vpn].virtualPage = vpn;
            currentThread->space->pageTable[vpn].physicalPage = descriptor;
            ramPages[descriptor]->head->current = currentThread->space;
            ramPages[descriptor]->vPage = vpn;
            ramPages[descriptor]->pid = currentThread->space->pid;
            }
          (void) interrupt->SetLevel(oldLevel); // re-enable interrupts

            // // fprintf(stderr," CHIll");
            // chillBrother->P();
            // vpn =  machine->ReadRegister(BadVAddrReg) / PageSize;
            // if(machine->pageTable[vpn].valid == false){
            //   descriptor = findReplacement();
            //   stuffing = ramPages[descriptor]->status;
            //   ramPages[descriptor]->status = MarkedForReplacement;
            //   // Set to currently being replaced.

            
            //   newSpacer = ramPages[descriptor]->head->current;
            //   if(newSpacer != NULL){
            //     newSpacer->pageTable[ramPages[descriptor]->vPage].valid = false;  
            //     newSpacer->pageTable[ramPages[descriptor]->vPage].physicalPage = -1;  
            //     DEBUG('a', "Page being replaced %d related to %d\n", newSpacer->pageTable[ramPages[descriptor]->vPage].physicalPage, newSpacer->pid);
            //   }
            
            //   if(stuffing != Free){
            //     incrementPC = ramPages[descriptor]->vPage; // incrementPC is now the virtual page #
            //     if(newSpacer != NULL && newSpacer->pageTable[incrementPC].dirty){
            //         // if(incrementPC == 9) {
            //         // fprintf(stderr, "Why me? %d    %d \n", currentThread->space->pid, incrementPC);
            //       // }
            //       for(j = 0; j < 128; j++){
            //         stringArg[j] = machine->mainMemory[newSpacer->pageTable[incrementPC].physicalPage * PageSize + j];
            //         DEBUG('f', "%c", stringArg[j]);
            //       }
                  
            //       newSpacer->revPageTable[ramPages[descriptor]->vPage].dirty = false;
            //       newSpacer->pageTable[ramPages[descriptor]->vPage].dirty = false;
            //       // fprintf(stderr, "%d\n", incrementPC);
            //       synchDisk->WriteSector(newSpacer->revPageTable[incrementPC].physicalPage, stringArg);
            //       DEBUG('a', "Dirty Page %d written in\n", newSpacer->revPageTable[incrementPC].physicalPage);
            //   }
            // }
            // memset(stringArg, '\0', sizeof(stringArg));
            
            // // fprintf(stderr, "Reading from %d, %d asshole pulling this shit: %d\n",vpn, currentThread->space->revPageTable[vpn].physicalPage, currentThread->space->pid);
            // synchDisk->ReadSector(currentThread->space->revPageTable[vpn].physicalPage, stringArg);

            // for(j = 0; j < 128; j++){
            //   machine->mainMemory[descriptor * PageSize + j] = stringArg[j];
            //     // if (vpn == 34 && currentThread->space->pid == 0) {
            //     //   // fprintf(stderr, "dododo %d", currentThread->space->revPageTable[0].physicalPage);
            //     //   DEBUG('p', "%c", stringArg[j]);

            //     // }
            // }
            // currentThread->space->pageTable[vpn].valid = true;
            // currentThread->space->pageTable[vpn].physicalPage = descriptor;
            // ramPages[descriptor]->vPage = currentThread->space->pageTable[vpn].virtualPage;
            // ramPages[descriptor]->head->current = currentThread->space;
            // ramPages[descriptor]->pid = currentThread->space->pid;
            // ramPages[descriptor]->status = InUse;
            // // fprintf(stderr, "PID: %d\n", currentThread->space->pid);
            // DEBUG('a', "Page %d replaced for VAddr %d, Place on disk is: %d\n", descriptor, machine->ReadRegister(BadVAddrReg), currentThread->space->revPageTable[vpn].physicalPage);
            // // fprintf(stderr, "Page %d replaced for VAddr %d, Place on disk is: %d\n", descriptor, machine->ReadRegister(BadVAddrReg), currentThread->space->revPageTable[vpn].physicalPage);
            // delete[] stringArg;
            // }
            // chillBrother->V();
            // // ramPages[descriptor].
            // // for (i=0; i<127; i++){
            //   // currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);  // Pretending this works.
            // // }
            // // if(){
            //   // synchDisk->WriteSector(ramPages[descriptor]->addrSpaceNode->current->revPageTable[ramPages[descriptor]->vPage].physicalPage, );]
            // // }
            // // fprintf(stderr, "ICE ICE BABY");
        break;
        case ReadOnlyException:     // Write attempted to page marked 
              // "read-only"
        fprintf(stderr, "READ ONLY EXCEPTION\n");
        outputRamPages();
        interrupt->Halt();
        break;
        case BusErrorException:     // Translation resulted in an 
        fprintf(stderr, "BUS ERROR EXCEPTION\n");
        outputRamPages();
        interrupt->Halt();
        break;
              // invalid physical address
        case AddressErrorException: // Unaligned reference or one that

        fprintf(stderr, "ADDRESS ERROR EXCEPTION\n");
        fprintf(stderr, "ADDR %d %d\n", machine->ReadRegister(4),machine->ReadRegister(5));
        outputRamPages();
        interrupt->Halt();
        break;
              // was beyond the end of the
              // address space
         case OverflowException:     // Integer overflow in add or sub.
         fprintf(stderr, "OVERFLOW ERROR EXCEPTION\n");
         outputRamPages();
         interrupt->Halt();
         break;
         case IllegalInstrException: // Unimplemented or reserved instr.
         fprintf(stderr, "ILLEGAL INSTRUCTION ERROR EXCEPTION\n");
         outputRamPages();
         interrupt->Halt();
         break;
      default: ;
    }
}

#define SC_Dup    10
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