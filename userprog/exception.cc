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
    // int size2;
    char whee;
    int i, j;
    AddrSpace *newSpacer;
    Thread *t;
    Semaphore* die;
    IntStatus oldLevel;
    FamilyNode *curr;
    FamilyNode *prev;

    //Exec w/ args variables
    int sp, len, argcount, herece;
    int argvAddr[16];


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
                prev = root;
                //fprintf(stderr, "\n EXITING THREAD IS: %d\n", (int)currentThread);
                DEBUG('a', "Thread exiting %d.\n", (int)currentThread);
                forking->P();
                while(curr->child != (int) currentThread&& curr->next !=NULL){
                  prev = curr;
                  curr = curr->next;  // Iterate to find the correct semphore to V
                }
                if(curr->touched){
                  prev->next = curr->next;
                }
                else{
                  curr->touched = true;
                }
                
                whence = machine->ReadRegister(4); // whence is the exit value for the thread.
                curr->exit = whence;
                forking->V();
                curr->death->V();
                delete currentThread->space;
                currentThread->Finish();
                DEBUG('a', "Failed to exit.  Machine will now terminate.\n");
                break;
        case SC_Join:
                DEBUG('a', "Join\n");
                whence = machine->ReadRegister(4);
                curr = root;
                prev = root;
                forking->P();
                while(( curr->child != whence ||curr->parent != (int)currentThread) && curr->next != NULL){
                  prev = curr;
                  curr = curr->next;  // Iterate to find the correct semapohre to P on
                }
                
                if(curr->parent != (int)currentThread && curr->child !=whence){
                  forking->V();
                  DEBUG('a', "Cannot find appropriate thread ID to join on.\n");
                  machine->WriteRegister(2, -1);  // If you cannot find the child return false.
                }
                else{
                  curr->touched = true;
                  forking->V();
                  //fprintf(stderr, "\n PARENT %d JOINING ON %d\n", (int)currentThread, whence);
                  DEBUG('a', "Parent %d, joining for %d.\n");
                  curr->death->P(); // Wait for child to die.
                  prev->next = curr->next;
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
                for (i=0; i<127; i++){
                  currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);  // Pretending this works.
                  if(stringArg[i] == '\0') break;
                }
                if(i==0){
                  DEBUG('a', "Invalid File Name: Must be longer than 0\n");
                  machine->WriteRegister(2, -1);  // If file name is invalid return -1.
                }// Cannot have a file with 'no name'
                stringArg[127]='\0';
                DEBUG('a', "Argument string is <%s>\n",stringArg);
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
                      currentThread->space->fileDescriptors[i] = new FileShield();
                      currentThread->space->fileDescriptors[i]->file = open; // Bitten in the ass.
                      currentThread->space->fileDescriptors[i]->CopyFile();
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
                forking->P();
                DEBUG('a', "Read\n");
                size = machine->ReadRegister(5);
                whence = machine->ReadRegister(4);
                descriptor = machine->ReadRegister(6);
                if(size > 0){
                  stringArg = new (std::nothrow) char[size + 1];
                  if (descriptor == ConsoleInput) {
                    if (currentThread->space->fileDescriptors[descriptor]->inOut == -1)
                      fromInput = 1;
                  }
                  if (descriptor == ConsoleOutput) {
                    if (currentThread->space->fileDescriptors[descriptor]->inOut == -1)
                      fromOutput = 1;
                  }
                  if (!fromInput && !fromOutput && descriptor < 16) {
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
                        //fprintf(stdout, "Thread: %d Read<%s> Size:%d\n", (int)currentThread, stringArg, size);
                        // if(size != 1){
                        //   ;
                        //   //stringArg[size] = '\0';
                        // }
                        for(i=0; i < size; i++){
                          currentThread->space->WriteMem(whence++, sizeof(char), stringArg[i]);
                          if(stringArg[i] == '\0') break;
                        }
                        DEBUG('a', "%s\n", stringArg);
                        DEBUG('a', "size: %d %s\n", size, stringArg);
                        machine->WriteRegister(2, size);  // Assume user allocates for null byte in char*
                      }
                    }
                  }
                  else if (fromInput) { // Deals with ConsoleInput
                    DEBUG('a', "size: %d %c\n", size, stringArg);
                    for(i = 0; i < size; i++){
                      whee = synchConsole->GetChar();
                      currentThread->space->WriteMem(whence++, sizeof(char), whee);
                    }
                    machine->WriteRegister(2, i);
                  }
                  else{ // Deals with out of bounds of the array.
                    DEBUG('a', "Invalid file descriptor.\n");
                    machine->WriteRegister(2, -1);  // Assume user allocates for null byte in char*
                  }
                  delete stringArg;
                }
                forking->V();
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_Write:
                DEBUG('a', "Write\n");
                forking->P();
                // Issue name: Oh God Why?
                // For some ungodly reason size decides to be 0 immediately after the for loop.  Why?  No idea.  If we have a different size
                // it for some reason then works and sets the other different size to 0.  Another fix we found... was to just reset size every
                // time we need to use it.
                size = machine->ReadRegister(5);
                if (size > 0){
                  stringArg = new(std::nothrow) char[size];
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
                    for (i=0; i<size; i++){
                      currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);
                      if(stringArg[i] == '\0')break;
                    }
                      //stringArg[size]='\0';
                  }
                  else if(size == 1 && !toOutput && !toInput){
                    currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[0]);
                  }
                    DEBUG('a', "Argument string is <%s>\n",stringArg);
                  if (!toInput && !toOutput && descriptor < 16) {
                    open = currentThread->space->fileDescriptors[descriptor]->file;
                    if(open == NULL){
                      DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                      machine->WriteRegister(2, -1);
                    }                
                    open->Write(stringArg, size);
                    if(size == 1)
                    DEBUG('a', "File descriptor for <%s> is %d\n", stringArg, descriptor);
                    
                  }
                  else if (toOutput) {
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
                  delete [] stringArg;           
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
                whence = currentThread->space->fileDescriptors[descriptor]->CloseFile();
                if(whence < 0){//no file is associated with the openfileid
                  DEBUG('a', "No OpenFile is associated with the given OpenFileId\n");
                  machine->WriteRegister(2 , -1);
                }
                if(whence == 0){
                  delete (currentThread->space->fileDescriptors[descriptor]->file);
                }
                currentThread->space->fileDescriptors[descriptor] = NULL;

                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);            
                break;
        case SC_Fork:
                DEBUG('a', "Fork\n");
                if(root==NULL){
                  DEBUG('a', "Root for the family tree is nonexistent.\n");
                  interrupt->Halt();
                }
                curr = root;
                forking->P();
                while(curr->next != NULL){
                  curr = curr->next;

                }
                forking->V();
                
                newSpacer = currentThread->space->newSpace(); // Create an AddrSpace for child
                if (newSpacer->enoughSpace == 0) {
                  // There was not enough space to create the child.  Return a -1 and delete the created addrspace
                  DEBUG('a', "Not enough space to fork child.\n");
                  delete newSpacer;
                  machine->WriteRegister(2, -1);
                }
                else {
                  t = new(std::nothrow) Thread("clone");
                  curr->next = new(std::nothrow) FamilyNode(t);  // Add new parent child relation to family tree.
                  t->space = newSpacer; // Give child its brand new space.
                  t->SaveUserState(); // Write all current machine registers to userRegisters for child.
                  currentThread->SaveUserState(); // Save just in case the Fork gets weird.
                  t->Fork(CopyRegs, (int)currentThread); // Fork child.
                  machine->WriteRegister(2, (int)t); // Write the appropriate return val for parent
                  currentThread->SaveUserState(); // Save again in case of weirdness.
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

                newSpacer = new AddrSpace(open);
                delete open;
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

                  len = strlen(stringArg) + 1;
                  sp -= len;
                  for(i = 0; i < len; i++){
                    newSpacer->WriteMem(sp + i, sizeof(char), stringArg[i]);
                  }
                  argvAddr[0] = sp;

                  
                  for(i = 1; i < 16; i++){
                    memset(stringArg, 0, sizeof(stringArg));
                    currentThread->space->ReadMem(whence, sizeof(int), &herece);
                    if (herece == 0){
                      break;
                    }
                    for(j = 0; j < 127; j++){
                      currentThread->space->ReadMem(herece++, sizeof(char), (int *)&stringArg[j]);
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
                  sp = sp & ~3;
                  DEBUG('a', "argcount %d\n", argcount);
                  sp -= sizeof(int) * argcount;

                  for(i = 0; i < argcount; i++){
                     newSpacer->WriteMem(sp + i*4, sizeof(int), argvAddr[i]);
                  }

                  delete currentThread->space;
                  
                  currentThread->space = newSpacer;
                  
                  newSpacer->RestoreState();

                  machine->WriteRegister(4, argcount);
                  machine->WriteRegister(5, sp);

                  machine->WriteRegister(StackReg, sp - 8);
   
                  machine->Run();
                }
                break;
        case SC_Dup:
                descriptor = machine->ReadRegister(4);
                if(descriptor < 0 || descriptor > 15){
                  DEBUG('a', "Invalid OpenFileId.\n");
                  machine->WriteRegister(2, -1);
                }
                if(currentThread->space->fileDescriptors[descriptor] == NULL){
                  DEBUG('a', "No OpenFile is associated with the given OpenFileId.\n");
                  machine->WriteRegister(2, -1);
                }
                for (i = 0; i < 16; i++) {
                  if (currentThread->space->fileDescriptors[i] == NULL) {
                    currentThread->space->fileDescriptors[i] = currentThread->space->fileDescriptors[descriptor];
                    currentThread->space->fileDescriptors[i]->CopyFile();
                    currentThread->space->fileDescriptors[i]->inOut = 0;
                    break;
                  }
                }
                if (i == 16) {
                  DEBUG('a', "No open space to DUP to.  Please close a file.\n");
                  machine->WriteRegister(2, -1);
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