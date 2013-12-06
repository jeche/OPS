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
  int found;
  int i;
  // Sweep through and check for free pages
  TranslationEntry *pageTableEntry = NULL;
  // Fancy clock part -- machine seems to set use bits to false when a page has been used -- not sure why this happens 
  // -- Changed the clock algorithm to check for use bit true instead of use bit false 
  while(1) {
    //fprintf(stderr, "Here\n");
    for (i = 0; i <= NumPhysPages; i++) {
      commutator = (commutator + 1) % NumPhysPages; 
      if (ramPages[commutator]->getStatus() == Free) {
        //fprintf(stderr, "free\n");
        found = commutator;
        // Head should be null, so we can't set the valid and physical page
        ramPages[found]->setStatus(MarkedForReplacement); 
        return found;
      }
    }

    // First scan -- Look for use bit true and dirty bit false
    for (i = 0; i <= NumPhysPages; i++) {
      commutator = (commutator + 1) % NumPhysPages; 
      if (ramPages[commutator]->head == NULL) {
        //fprintf(stderr, "NULL\n");
        found = commutator;
        ramPages[found]->setStatus(MarkedForReplacement);
        return found;
      }
      else {
        pageTableEntry = &(ramPages[commutator]->head->current->pageTable[ramPages[commutator]->vPage]);

        if (ramPages[commutator]->getStatus() != MarkedForReplacement && ramPages[commutator]->getStatus() != Cow && pageTableEntry->use && !pageTableEntry->dirty) {
          //fprintf(stderr, "Not used and not dirty\n");
          found = commutator;
          ramPages[found]->head->current->pageTable[ramPages[found]->vPage].valid = false;  
          ramPages[found]->head->current->pageTable[ramPages[found]->vPage].physicalPage = -1;  
          ramPages[found]->setStatus(MarkedForReplacement); 
          return found;
        }
      }
    }

    // Second scan -- Look for use bit true and dirty bit true -- change use bits as we go
    for (i = 0; i <= NumPhysPages; i++) {
      commutator = (commutator + 1) % NumPhysPages; 
      if (ramPages[commutator]->head == NULL) {
        found = commutator;
        ramPages[commutator]->setStatus(MarkedForReplacement);
        return found;
      }
      else {
        pageTableEntry = &(ramPages[commutator]->head->current->pageTable[ramPages[commutator]->vPage]);
        if (ramPages[commutator]->getStatus() != MarkedForReplacement && ramPages[commutator]->getStatus() != Cow && pageTableEntry->use && pageTableEntry->dirty) {
          found = commutator;
          //fprintf(stderr, "Not used and dirty\n");
          ramPages[found]->head->current->pageTable[ramPages[found]->vPage].valid = false;  
          ramPages[found]->head->current->pageTable[ramPages[found]->vPage].physicalPage = -1;  
          ramPages[found]->setStatus(MarkedForReplacement);
          return found;
        }
        // Set the use bit to true if it is not already
        pageTableEntry->use = true;
      }
    }
  }
}

void pageFaultHandler(int badVAddr) {
  char* stringArg;
  stringArg = new(std::nothrow) char[128];
  int vpn = badVAddr / PageSize;
  int i;
  int victim;
  stats->numPageFaults++;
  if(!machine->pageTable[vpn].valid){
    if (currentThread->space->pageTable[vpn].readOnly && diskRefCount[currentThread->space->revPageTable[vpn].physicalPage] > 1) {
      for (i = 0; i < NumPhysPages; i++) {
        if (ramPages[i]->pPage == currentThread->space->revPageTable[vpn].physicalPage) {
          currentThread->space->pageTable[vpn].valid = true;
          currentThread->space->pageTable[vpn].physicalPage = i;
          ramPages[i]->refcount++;
          break;
        }
      }
      if (i == NumPhysPages) { // Not found in ramPages, so we need to replace
        victim = findReplacement();
        if(ramPages[victim]->head != NULL && ramPages[victim]->head->current != NULL ){  //Removed this && ramPages[victim]->getStatus() == InUse
          memset(stringArg, 0, sizeof(stringArg));

          if(ramPages[victim]->head->current->pageTable[ramPages[victim]->vPage].dirty){
            ramPages[victim]->head->current->pageTable[ramPages[victim]->vPage].dirty = false;
            for(int q = 0; q < PageSize; q++){
               stringArg[q] = machine->mainMemory[victim * PageSize + q];

            }
            synchDisk->WriteSector(ramPages[victim]->head->current->revPageTable[ramPages[victim]->vPage].physicalPage, stringArg);
          }
        }
        memset(stringArg, 0, sizeof(stringArg));
        synchDisk->ReadSector(currentThread->space->revPageTable[vpn].physicalPage, stringArg);
        for(int q = 0; q < PageSize; q++){
          machine->mainMemory[victim * PageSize + q] = stringArg[q];
        }
        ramPages[victim]->vPage = vpn;
        ramPages[victim]->pid = currentThread->space->pid;
        if (ramPages[victim]->head != NULL && ramPages[victim]->head->current != NULL) {
          delete ramPages[victim]->head;  // Not sure about this line
        }
        ramPages[victim]->head = new(std::nothrow) addrSpaceNode(currentThread->space);
        currentThread->space->pageTable[vpn].valid = true;
        currentThread->space->pageTable[vpn].physicalPage = victim;

        ramPages[victim]->setStatus(Cow);
        ramPages[victim]->pPage = currentThread->space->revPageTable[vpn].physicalPage;
      }
    }

    else if (currentThread->space->pageTable[vpn].readOnly) {
      currentThread->space->pageTable[vpn].readOnly = false;
      victim = findReplacement();
      if(ramPages[victim]->head != NULL && ramPages[victim]->head->current != NULL ){  //Removed this && ramPages[victim]->getStatus() == InUse
        memset(stringArg, 0, sizeof(stringArg));

        if(ramPages[victim]->head->current->pageTable[ramPages[victim]->vPage].dirty){
          ramPages[victim]->head->current->pageTable[ramPages[victim]->vPage].dirty = false;
          for(int q = 0; q < PageSize; q++){
             stringArg[q] = machine->mainMemory[victim * PageSize + q];

          }
          synchDisk->WriteSector(ramPages[victim]->head->current->revPageTable[ramPages[victim]->vPage].physicalPage, stringArg);
        }
      }
      memset(stringArg, 0, sizeof(stringArg));
      synchDisk->ReadSector(currentThread->space->revPageTable[vpn].physicalPage, stringArg);
      for(int q = 0; q < PageSize; q++){
        machine->mainMemory[victim * PageSize + q] = stringArg[q];
      }
      ramPages[victim]->vPage = vpn;
      ramPages[victim]->pid = currentThread->space->pid;
      if (ramPages[victim]->head != NULL && ramPages[victim]->head->current != NULL) {
        delete ramPages[victim]->head;  // Not sure about this line
      }
      ramPages[victim]->head = new(std::nothrow) addrSpaceNode(currentThread->space);
      currentThread->space->pageTable[vpn].valid = true;
      currentThread->space->pageTable[vpn].physicalPage = victim;

      ramPages[victim]->setStatus(InUse);
      ramPages[victim]->pPage = currentThread->space->revPageTable[vpn].physicalPage;
    }

  else {
    victim = findReplacement();

    if(ramPages[victim]->head != NULL && ramPages[victim]->head->current != NULL ){  //Removed this && ramPages[victim]->getStatus() == InUse
      memset(stringArg, 0, sizeof(stringArg));

      if(ramPages[victim]->head->current->pageTable[ramPages[victim]->vPage].dirty){
        ramPages[victim]->head->current->pageTable[ramPages[victim]->vPage].dirty = false;
        for(int q = 0; q < PageSize; q++){
           stringArg[q] = machine->mainMemory[victim * PageSize + q];

        }
        synchDisk->WriteSector(ramPages[victim]->head->current->revPageTable[ramPages[victim]->vPage].physicalPage, stringArg);
      }
    }
    memset(stringArg, 0, sizeof(stringArg));
    synchDisk->ReadSector(currentThread->space->revPageTable[vpn].physicalPage, stringArg);
    for(int q = 0; q < PageSize; q++){
      machine->mainMemory[victim * PageSize + q] = stringArg[q];
    }
    ramPages[victim]->vPage = vpn;
    ramPages[victim]->pid = currentThread->space->pid;
    if (ramPages[victim]->head != NULL && ramPages[victim]->head->current != NULL) {
      delete ramPages[victim]->head;  // Not sure about this line
    }
    ramPages[victim]->head = new(std::nothrow) addrSpaceNode(currentThread->space);
    currentThread->space->pageTable[vpn].valid = true;
    // currentThread->space->pageTable[vpn].valid = true;
    currentThread->space->pageTable[vpn].physicalPage = victim;


    ramPages[victim]->setStatus(InUse);
    ramPages[victim]->pPage = currentThread->space->revPageTable[vpn].physicalPage;
  }
}

void outputRamPages(){
  for(int i = 0; i < NumPhysPages; i++){
    fprintf(stderr, "RamPage %d for pid %d and vPage %d", i, ramPages[i]->pid, ramPages[i]->vPage);
    if(ramPages[i]->head != NULL){
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
    char *whee2;
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
                DEBUG('y', "Exit\n");
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
	          //fprintf(stderr,"NUMPAGEFAULTS %d\n", stats->numPageFaults);
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
                      if(descriptor==2){//printf("hellow\n");
}
                      forking->P();
                      if(currentThread->space->fileDescriptors[descriptor] == NULL){
                        DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                        machine->WriteRegister(2, -1);  
                      }
                      else{
                        open = currentThread->space->fileDescriptors[descriptor]->file;
                        if(open == NULL){
                          DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                          machine->WriteRegister(2, -1);  
                        }
                        else{
                          //descriptor = 0;
                          size = open->Read(stringArg, size);
                          if(descriptor==2){//fprintf(stdout, "Read<%s>\n", stringArg);
}
                          // currentThread->space->WriteMem(whence, sizeof(char), stringArg[0]);
                          //oldLevel = interrupt->SetLevel(IntOff);
                          for(i=0; i < size; i++){
                            currentThread->space->WriteMem(whence++, sizeof(char), stringArg[i]);
                            //interrupt->SetLevel(IntOff);
                            if(stringArg[i] == '\0') break;
                          }
                          //interrupt->SetLevel(oldLevel);
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
                      machine->WriteRegister(2, -1); 
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
                // fprintf(stderr, "\n\n\n\n\n\n<%d>\n", sizeof(char));
                // fprintf(stderr,"WHEEEEEEEEEEEEEEEEEEEEEEE\n");
                forking->P();
                size = machine->ReadRegister(5);
                if (size > 0){
                  stringArg = new(std::nothrow) char[128];
                  whence = machine->ReadRegister(4);
                  descriptor = machine->ReadRegister(6);
                  DEBUG('a',"String starts at address %d in user VAS\n", whence);
                  // Deals with the case when output has been closed and a file has been duped there.
                  // fprintf(stderr, "%d DESCRIPTOR\n", descriptor);
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
                    //oldLevel = interrupt->SetLevel(IntOff);
                    for (i=0; i<size; i++){
                      currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);
                      //interrupt->SetLevel(IntOff);
                      // if(stringArg[i] == '\0') break;
                    }
                    //interrupt->SetLevel(oldLevel);
                      //stringArg[size]='\0';
                  }
                  else if(size == 1 && !toOutput && !toInput){
                    // currentThread->space->ReadMem(whence, sizeof(char), (int *)&stringArg[0]);
                    //oldLevel = interrupt->SetLevel(IntOff);
                    currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[0]);
                    //interrupt->SetLevel(oldLevel);
                  }
                    DEBUG('a', "Argument string is <%s>\n",stringArg);
                    // fprintf(stderr, "I\n");
                  if (toInput != 1 && toOutput != 1 && descriptor < 16 && descriptor >= 0) {
                    // fprintf(stderr, "%d\n",descriptor );
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
                  else if (toOutput == 1) {
                    // fprintf(stderr, "Hey\n");
                    // currentThread->space->ReadMem(whence, sizeof(char), (int *)&whee);
                    DEBUG('a', "Writing To Output\n");
                    for (i = 0; i < size; i++) {
                      size = machine->ReadRegister(5);
                      // fprintf(stderr, "eyey\n");
                      if(currentThread->space->ReadMem(whence++, sizeof(char), (int *)&whee)){}
                      // fprintf(stderr, "herp\n");
                      synchConsole->PutChar(whee);
                      // fprintf(stderr, "aHey\n");
                    }
                    //synchConsole->PutChar('\0');
                  }
                  else if (toInput == 1){ // Should this also check for toInput? *****
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
                DEBUG('a', "WriteEnd\n");
                forking->V();
                // fprintf(stderr , "FINISHEDEXEC\n");
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
                forking->P();
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
                      // currentThread->space->ReadMem(whence, sizeof(int), &herece);
                      currentThread->space->ReadMem(whence, sizeof(int), &herece);
                      if (herece == 0){
                        break;
                      }
                      //fprintf(stderr, "Original argument\n");
                      // currentThread->space->ReadMem(herece, sizeof(char), (int *)&stringArg[0]);
                      for(j = 0; j < 127; j++){
                        // currentThread->space = oldSpacer;
                        // currentThread->space->RestoreState();
                        currentThread->space->ReadMem(herece++, sizeof(char), (int *)&stringArg[j]);
                        //fprintf(stderr, "'%c'", stringArg[j]);
                        if(stringArg[j] == '\0') break;
                      }
                      //fprintf(stderr, "\n");
                      DEBUG('a', "STRINGARG %s\n",stringArg);
                      // for( j = 0; j < 110; j++){
                        // fprintf(stderr, "PAGETABLE: %d %d\n", currentThread->space->revPageTable[j].physicalPage, currentThread->space->pid);
                      // }
                      len = strlen(stringArg) + 1;
                      sp -= len;

                      //fprintf(stderr, "Here is args:");
                      for(j = 0; j < len; j++){
                         //newSpacer->WriteMem(sp+j, sizeof(char), stringArg[j]);
                        newSpacer->WriteMem(sp+j, sizeof(char), stringArg[j]);
                        //fprintf(stderr, "'%c'", stringArg[j]);
                      }
                     // fprintf(stderr, "\n");
                      argvAddr[i] = sp;
                      whence = whence + sizeof(int);

                    }
                    argcount = i;
                    sp = sp & ~3;
                    DEBUG('a', "argcount %d\n", argcount);
                    sp -= sizeof(int) * argcount;

                    for(i = 0; i < argcount; i++){
                       // newSpacer->WriteMem(sp + i*4, sizeof(int), argvAddr[i]);
                       newSpacer->WriteMem(sp + i*4, sizeof(int), argvAddr[i]);
                    }
                    delete [] stringArg;

                    delete currentThread->space;
                    
                    currentThread->space = newSpacer;
                    
                    newSpacer->RestoreState();
                    // for( j = 0; j < 110; j++){
                        // fprintf(stderr, "PAGETABLE2: %d %d\n", currentThread->space->revPageTable[j].physicalPage, currentThread->space->pid);
                    // }
                    machine->WriteRegister(4, argcount);
                    machine->WriteRegister(5, sp);

                    machine->WriteRegister(StackReg, sp - (8 * 4));
                    forking->V();
                    machine->Run();
                  }
                }
                forking->V();
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
          chillBrother->P();
          pageFaultHandler(machine->ReadRegister(BadVAddrReg));
          chillBrother->V();
        break;
        case ReadOnlyException:     // Write attempted to page marked 
              // "read-only"
        fprintf(stderr, "READ ONLY EXCEPTION\n");
        //outputRamPages();
        //interrupt->Halt();

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
        currentThread->space->writeBackDirty();
        currentThread->space->printAllPages();

        interrupt->Halt();
        break;
              // was beyond the end of the
              // address space
         case OverflowException:     // Integer overflow in add or sub.
         fprintf(stderr, "OVERFLOW ERROR EXCEPTION\n");
         outputRamPages();
         // currentThread->space->writeBackDirty();
         currentThread->space->printAllPages();
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
#define SC_CheckPoint 11
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
