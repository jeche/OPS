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

#include <string.h>
#include <stdlib.h>
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
  //fprintf(stderr, "forkedPid: %d\n", currentThread->space->pid);
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
  int start = commutator;
  // Sweep through and check for free pages
  TranslationEntry *pageTableEntry = NULL;
  // Fancy clock part -- machine seems to set use bits to false when a page has been used -- not sure why this happens 
  // -- Changed the clock algorithm to check for use bit true instead of use bit false 

  for (i = 0; i <= NumPhysPages; i++) {
    commutator = (commutator + 1) % NumPhysPages; 
    if (commutator == start) {
      continue;
    }
    //fprintf(stderr, "commutator: %d\n", commutator);
    if (ramPages[commutator]->getStatus() == Free) {
      //fprintf(stderr, "free\n");
      found = commutator;
      // Head should be null, so we can't set the valid and physical page
      ramPages[found]->setStatus(MarkedForReplacement); 
      return found;
    }
  }
  while(1){
    for(int j = 0; j < 2; j++) {

       
      // First scan -- Look for use bit true and dirty bit false
      for (i = 0; i <= NumPhysPages; i++) {
        commutator = (commutator + 1) % NumPhysPages; 
        if (commutator == start) {
          continue;
        }
        if (ramPages[commutator]->head == NULL && ramPages[commutator]->getStatus() != MarkedForReplacement && ramPages[commutator]->getStatus() != CowPage) {
          found = commutator;
          ramPages[found]->setStatus(MarkedForReplacement);
          return found;
        }
        else {
          pageTableEntry = &(ramPages[commutator]->head->pageTable[ramPages[commutator]->vPage]);

          if (ramPages[commutator]->getStatus() != MarkedForReplacement && pageTableEntry->use && !pageTableEntry->dirty && ramPages[commutator]->getStatus() != CowPage) {
            found = commutator;
            ramPages[found]->head->pageTable[ramPages[found]->vPage].valid = false;  
            ramPages[found]->head->pageTable[ramPages[found]->vPage].physicalPage = -1;  
            ramPages[found]->setStatus(MarkedForReplacement); 
            return found;
          }
        }
      }

      // Second scan -- Look for use bit true and dirty bit true -- change use bits as we go
      for (i = 0; i <= NumPhysPages; i++) {
        commutator = (commutator + 1) % NumPhysPages;
        if (commutator == start) {
          continue;
        } 
        if (ramPages[commutator]->head == NULL && ramPages[commutator]->getStatus() != MarkedForReplacement && ramPages[commutator]->getStatus() != CowPage) {
          found = commutator;
          ramPages[found]->setStatus(MarkedForReplacement);
          return found;
        }
        else {
          pageTableEntry = &(ramPages[commutator]->head->pageTable[ramPages[commutator]->vPage]);
          if (ramPages[commutator]->getStatus() != MarkedForReplacement && pageTableEntry->use && pageTableEntry->dirty && ramPages[commutator]->getStatus() != CowPage) {
            found = commutator;
            ramPages[found]->head->pageTable[ramPages[found]->vPage].valid = false;  
            ramPages[found]->head->pageTable[ramPages[found]->vPage].physicalPage = -1;  
            ramPages[found]->setStatus(MarkedForReplacement);
            return found;
          }
          // Set the use bit to true if it is not already
          pageTableEntry->use = true;
        }
      }
    }
    // All pages are cow pages, so we need to replace one of these
    if(ramPages[commutator]->getStatus() != MarkedForReplacement){
      found = commutator;
      ramPages[found]->head->pageTable[ramPages[found]->vPage].valid = false;
      ramPages[found]->head->pageTable[ramPages[found]->vPage].physicalPage = -1; 
      AddrSpace *other = diskPages[ramPages[found]->head->revPageTable[ramPages[found]->vPage].physicalPage]->otherAddr(ramPages[found]->head);
      if(other!= NULL){
        other->pageTable[ramPages[found]->vPage].valid = false;
        other->pageTable[ramPages[found]->vPage].physicalPage = -1;
      }
      ASSERT(ramPages[found]->head->isCowAddr()); //********************************************
      ramPages[found]->setStatus(MarkedForReplacement);
      return found; 
    }
    // chillBrother->V();
    // chillBrother->P();
  }
}

void pageFaultHandler(int badVAddr) {
  char* stringArg;
  stringArg = new(std::nothrow) char[128];
  int vpn = badVAddr / PageSize;
  stats->numPageFaults++;
  if(!machine->pageTable[vpn].valid){
    int victim = findReplacement();
    if(ramPages[victim]->head != NULL && !ramPages[victim]->head->pageTable[ramPages[victim]->vPage].readOnly){
      memset(stringArg, 0, sizeof(stringArg));
      if(ramPages[victim]->head->pageTable[ramPages[victim]->vPage].dirty){
        ramPages[victim]->head->pageTable[ramPages[victim]->vPage].dirty = false;
        for(int q = 0; q < PageSize; q++){
           stringArg[q] = machine->mainMemory[victim * PageSize + q];
        }
        synchDisk->WriteSector(ramPages[victim]->head->revPageTable[ramPages[victim]->vPage].physicalPage, stringArg);
      }
    }
    memset(stringArg, 0, sizeof(stringArg));
    synchDisk->ReadSector(currentThread->space->revPageTable[vpn].physicalPage, stringArg);
    for(int q = 0; q < PageSize; q++){
      machine->mainMemory[victim * PageSize + q] = stringArg[q];
    }
    ramPages[victim]->vPage = vpn;
    ramPages[victim]->pid = currentThread->space->pid;
    ramPages[victim]->head = currentThread->space;
    
    if (diskPages[currentThread->space->revPageTable[vpn].physicalPage]->getStatus() == CowPage || currentThread->space->pageTable[vpn].readOnly) {
        AddrSpace *other = diskPages[currentThread->space->revPageTable[vpn].physicalPage]->otherAddr(currentThread->space);
        other->pageTable[vpn].valid = true;
        other->pageTable[vpn].physicalPage = victim;
        ramPages[victim]->setStatus(CowPage);
    }else{
      ramPages[victim]->setStatus(InUse);
    }
    currentThread->space->pageTable[vpn].valid = true;
    currentThread->space->pageTable[vpn].physicalPage = victim;
    DEBUG('j', "PageFaultHandled\n");
  } 
  delete [] stringArg;
}

//-----------------------------
// outputRamPages
//   Debug function we wrote to check and see what is on ramPages.  Should not be used anywhere.
//-----------------------------

void outputRamPages(){
  for(int i = 0; i < NumPhysPages; i++){
    fprintf(stderr, "RamPage %d for pid %d and vPage %d", i, ramPages[i]->pid, ramPages[i]->vPage);
    if(ramPages[i]->head != NULL){
      fprintf(stderr, " validity is %s  and dirt is %s", ramPages[i]->head->pageTable[ramPages[i]->vPage].valid?"true":"false", ramPages[i]->head->pageTable[ramPages[i]->vPage].dirty?"true":"false");
    }
    fprintf(stderr, "\n");
  }
}

class pwrap{
public:
  Mail* m;
  Thread* t;
};
//------------------------------
// sendPacket
//    Handles the sending and waiting for an 'ack'. Should 
//    be used as the method for a forked off thread so as to 
//    not hang the system
//------------------------------
/*
void sendPacket(int mailMessage){
    //fprintf(stderr, "in send packet!!!\n");
    pwrap* p = (pwrap *) mailMessage;
    Mail* mail = (Mail *) p->m;
    MailHeader mailHdr = mail->mailHdr;
    PacketHeader pktHdr = mail->pktHdr;
    AckHeader ackHdr = mail->ackHdr;
    char *data = mail->data;
    int msgID = ackHdr.messageID;
    int fromBox = mailHdr.from;
    int toBox = mailHdr.to;
    int fromMach = pktHdr.from;
    int toMach = pktHdr.to;
    int cPack = ackHdr.curPack;
    unsigned long long systime = stats->totalTicks;
    // fprintf(stderr, "sent pack %d %d\n", msgID, cPack);
    postOffice->Send(pktHdr, mailHdr, ackHdr, data);
    
    postOffice->ackLockAcquire(fromBox);
    unsigned long long TIMEOUT;
    TIMEOUT =  10000000;
    fprintf(stderr, "systime %llu, stats time %llu \n", systime, stats->totalTicks);
    while(!postOffice->CheckAckPO(fromBox, msgID, fromMach, toMach, fromBox, toBox, cPack)){

      if(stats->totalTicks > (systime + TIMEOUT)){//This will need to be modified to account for how often the timeoutctr is incremented
        //fprintf(stderr, "resend %d %d\n", msgID,cPack);
        fprintf(stderr, "current time %llu\n", stats->totalTicks);
        postOffice->ackLockRelease(fromBox);
        postOffice->Send(pktHdr, mailHdr, ackHdr, data);
        postOffice->ackLockAcquire(fromBox);
        systime = stats->totalTicks;
        
      }
      postOffice->hasAckWait(fromBox);
    }
    //postOffice->hasAckSignal(fromBox);
    postOffice->ackLockRelease(fromBox);
    delete mail;
    p->t->Finish();
    
    //fprintf(stderr, "end of function %d %d\n", msgID,cPack);
}
*/



void
ExceptionHandler(ExceptionType which)
{
    //BitMap *msgMap;
    int type = machine->ReadRegister(2);
    int whence;
    int size;
    int toOutput = 0;
    int toInput = 0;
    int fromInput = 0;
    int fromOutput = 0;
    char* stringArg;
    char pageBuf[128];
    OpenFile* open;
    int descriptor = -1;
    int incrementPC;
    
    int adult;

    int packetNums;
    int oldPID;
    int fromMacher;
    int toMacher;
    char whee;
//*     char *whee2;
    int i, j;
    int numPages;
    int flag = 0;
    AddrSpace *newSpacer;
    Thread *t;
    IntStatus oldLevel;
    FamilyNode *curr;
    int vpn;
    //Exec w/ args variables
    int sp, len, argcount, herece;
    int argvAddr[16];
//*    Status stuffing;
    AddrSpace *oldSpacer;

    MessageNode* recved;
    Mail* recMail;
    MailNode* curNode;
    
//*     pwrap* rap;
    int pos=0/*, divisor=1000000000*/;
    char c;
    char buffer[20];

    PacketHeader outPktHdr;
    MailHeader outMailHdr;
    AckHeader outAckHdr;
    char* mailBuffer;
    int machineNum, location, remain, msgID, myMB;
//*    char* bufferList;
    Mail *mail;
//*    NetworkAddress origMachine;
//*    int origID;
    ForeignThreadNode *curFTN;
    unsigned int ui, uj;
    // fprintf(stderr, "I am P Exception Which %d Case %d\n", which, type);
    ((Semaphore *)currentThread->inKernel)->P();
  switch (which) {
      case SyscallException:
      switch (type) {
        case SC_Halt:
                DEBUG('a', "Shutdown, initiated by user program.\n");
                //fprintf(stderr, "Normal Halt\n");
                curr = root;
                // while(curr->next !=NULL){
                //   curr = curr->next;  // Iterate to find the correct semphore to V
                //   //fprintf(stderr, "pid parent %d pid child %d exit %d\n", curr->parent, curr->child, curr->exit);
                // }
                // fprintf(stderr, "I am V\n");
                ((Semaphore *)currentThread->inKernel)->V();
                interrupt->Halt();
                break;
        case SC_Exit:
                //fprintf(stderr, "EXIT\n");
                DEBUG('j', "Exit addr: %d\n", currentThread->space);
                forking->P();
                //oldLevel = interrupt->SetLevel(IntOff);
                if(currentThread->migrate == 1){
                  curFTN = foreignRoot;
                  while(curFTN->next != NULL && curFTN->curPID != currentThread->space->pid){
                      curFTN = curFTN->next;
                  }
                  if(curFTN != NULL){
                    msgCTR->P();
                    msgctr++;
                    msgID=msgctr;
                    msgCTR->V(); 
                    outPktHdr.to = server;   
                    outMailHdr.to = netname;
                    //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
                    outMailHdr.from = 0;//1; 
                    // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
                    outMailHdr.length = 4; // had a plus 1 here before?????????
                    outAckHdr.totalSize = 1;// size/MaxMailSize ; 
                    outAckHdr.curPack = 0;
                    outAckHdr.messageID = msgID;
                    outAckHdr.migrateFlag = machine->ReadRegister(4);
                    whence = machine->ReadRegister(4);
                    fprintf(stderr, "exit value: %d , %d, %d\n", outAckHdr.migrateFlag, machine->ReadRegister(4), whence);
                    outAckHdr.pageID = currentThread->space->pid;

                    //Right now I am just passing in the buffer from one message to another, this might
                    //need to change to deep copying it over to a new buffer due to bad things happening...

                    mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
                    postOffice->SendThings(mail, 0);
                    ((Semaphore *)currentThread->inKernel)->V();
                    // currentThread->Finish();
                  }
                } 
                curr = root;
                DEBUG('j', "Thread exiting %d.\n", currentThread->space->pid);
                
                while(curr->child != currentThread->space->pid && curr->next !=NULL){
                  curr = curr->next;  // Iterate to find the correct semphore to V
                }
                if(curr->migrated == 1){
                  msgCTR->P();
                  msgctr++;
                  msgID=msgctr;
                  msgCTR->V(); 
                  outPktHdr.to = server;   
                  outMailHdr.to = netname;
                  outMailHdr.from = 0;// 1; 
                  outMailHdr.length = 5; 
                  outAckHdr.totalSize = 1;
                  outAckHdr.curPack = 0;
                  outAckHdr.messageID = msgID;
                  outAckHdr.migrateFlag = machine->ReadRegister(4);
                  whence = machine->ReadRegister(4);
                  outAckHdr.pageID = currentThread->space->pid;
                  outAckHdr.child = curr->parent;
                  curr->migrated = 3;

                  // Right now I am just passing in the buffer from one message to another, this might
                  // need to change to deep copying it over to a new buffer due to bad things happening... No you don't the deep copy happens
                  // inside the mail packet.

                  mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
                  postOffice->SendThings(mail, 0);
                  ((Semaphore *)currentThread->inKernel)->V();
                }
                if(curr->child != currentThread->space->pid){
                  DEBUG('a', "How the hell do you break an exit?\n");
                }
                else{
                  whence = machine->ReadRegister(4); // whence is the exit value for the thread.
                  curr->exit = whence;
                  forking->V();
                  curr->death->V();
                  chillBrother->P();
                  //fprintf(stderr, "heeeeerrrrrreeeee\n" );
                  currentThread->space->remDiskPages();
                  chillBrother->V();  
                  delete currentThread->space;
                  curr->touched = true;
                  incrementPC=machine->ReadRegister(NextPCReg)+4;
                // fprintf(stderr, "I am V\n");
                  ((Semaphore *)currentThread->inKernel)->V();
                  currentThread->Finish();

                  DEBUG('a', "Failed to exit.  Machine will now terminate.\n");
                }
                break;
        case SC_Join:
                DEBUG('j', "Join\n");
                whence = machine->ReadRegister(4);
                if(currentThread->migrate == -1){
                  curr = root;
                  forking->P();
                  //oldLevel = interrupt->SetLevel(IntOff);
                                  DEBUG('j', "Join\n");
                  while(( curr->child != whence || curr->parent != currentThread->space->pid) && curr->next != NULL){
                    curr = curr->next;  // Iterate to find the correct semapohre to P on
                  }
                  
                  
                  if(curr->parent != currentThread->space->pid && curr->child != whence){
                    forking->V();
                    DEBUG('j', "Cannot find appropriate thread ID to join on.\n");
                    curr = root;
                  // while(curr->next !=NULL){
                  //   curr = curr->next;  // Iterate to find the correct semphore to V
                  //   fprintf(stderr, "pid parent %d pid child %d exit %d\n", curr->parent, curr->child, curr->exit);
                  // }
                  //fprintf(stderr, "whence: %d, pid: %d\n", whence, currentThread->space->pid);
                    machine->WriteRegister(2, -1);  // If you cannot find the child return false.
                  }

                  else{
                    // curr->touched = true;
                    forking->V();
                    DEBUG('j', "Parent %d, joining for %d.\n", currentThread->space->pid, curr->child);
                    //fprintf(stderr, "addr %d out of join\n", currentThread->space);
                    curr->death->P(); // Wait for child to die.
                    machine->WriteRegister(2, curr->exit); // Return the exit value.
                  }
                }
                else{
                  //Case where the process has been migrated
                  DEBUG('j', "Join(Migrated Thread)");
                  forking->P();
                  for(curr = root; curr != NULL && (curr->child != whence || curr->parent != currentThread->space->pid); curr = curr->next){}
                  if (curr != NULL){
                    forking->V();
                    curr->death->P();
                    machine->WriteRegister(2, curr->exit); // Return the exit value.
                  }
                  else{
                    for(curr = root; curr->next != NULL && (curr->child != whence || curr->parent != currentThread->space->pid); curr = curr->next){}
                    curr->next = new(std::nothrow) FamilyNode(-whence, currentThread->space->pid, NULL, -1);
                    forking->V();

                    // CHECK IF CHILD ALREADY EXITTED

                    curr->next->death->P();
                    machine->WriteRegister(2, curr->next->exit); // Return the exit value.
                  }
                  // GrabMessage from mailbox 2 to tell you when the child has joined
                }
                //(void) interrupt->SetLevel(oldLevel);
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_Create:/*Checks for -> Filename given is a single \0*/
                DEBUG('j', "Create\n");
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
                DEBUG('j', "Open\n");
                stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters
                whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string
                DEBUG('a',"String starts at address %d in user VAS\n", whence);
                for (i=0; i<127; i++){
                  currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]); 
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
                      forking->P();
                      currentThread->space->fileDescriptors[i] = new(std::nothrow) FileShield();
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
                
                DEBUG('j', "Read\n");
                size = machine->ReadRegister(5);
                whence = machine->ReadRegister(4);
                descriptor = machine->ReadRegister(6);

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
                        machine->WriteRegister(2, -1);  
                      }
                      else{
                        open = currentThread->space->fileDescriptors[descriptor]->file;
                        if(open == NULL){
                          DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                          machine->WriteRegister(2, -1);  
                        }
                        else{
                          size = open->Read(stringArg, size);
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
                DEBUG('j', "Write\n");
                // forking->P();
                size = machine->ReadRegister(5);
                // fprintf(stderr, "size: %d\n", size);
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
                    for (i=0; i<size; i++){
                      currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]);
                    }
                  }
                  else if(size == 1 && !toOutput && !toInput){
                    currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[0]);
                  }
                    DEBUG('a', "Argument string is <%s>\n",stringArg);
                  if (toInput != 1 && toOutput != 1 && descriptor < 16 && descriptor >= 0) {
                    open = currentThread->space->fileDescriptors[descriptor]->file;
                    if(open == NULL){
                      DEBUG('a', "Invalid file descriptor.\n");  // Handles if the open file descriptor describes a file that is not open.
                      machine->WriteRegister(2, -1);
                    }
                    else{                
                      open->Write(stringArg, size);
                      DEBUG('a', "File descriptor for <%s> is %d\n", stringArg, descriptor);
                    }
                  }
                  else if (toOutput == 1) {
                    DEBUG('a', "Writing To Output\n");
                    for (i = 0; i < size; i++) {
                      size = machine->ReadRegister(5);
                      if(currentThread->space->ReadMem(whence++, sizeof(char), (int *)&whee)){}
                      synchConsole->PutChar(whee);
                    }
                  }
                  else if (toInput == 1){ 
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
                // forking->V();
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);            
                break;
        case SC_Close:/*Cases Handled -> invalid openfileid, and no file associated with the openfileid*/
                DEBUG('j', "Close\n");
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
                //fprintf(stderr, "fork\n");
                DEBUG('j', "Fork\n");
                if(root==NULL){
                  DEBUG('j', "Root for the family tree is nonexistent.\n");
                }
                curr = root;
                forking->P();
                oldLevel = interrupt->SetLevel(IntOff);
                pid++; // bump pid before adding kid
                size = pid;
                while(curr->next != NULL){
                  curr = curr->next;

                }
                if(currentThread->space->isCowAddr()){//if it is a Cow process then do newspace    
                  DEBUG('j', "Creating a Normal AddrSpace, Parent is a Cow\n");
                  newSpacer = currentThread->space->newSpace(size); // Create an AddrSpace for child
                }
                else{
                  DEBUG('j', "Creating a CowAddrSpace\n");
                  newSpacer = currentThread->space->newSpace(size);
                  // newSpacer = currentThread->space->cowSpace(size);
                }
                //newSpacer->pid = size; // give child's space a pid.
                if (newSpacer->enoughSpace == 0) {
                  // There was not enough space to create the child.  Return a -1 and delete the created addrspace
                  DEBUG('j', "Not enough space to fork child.\n");
                  machine->WriteRegister(2, -1);
                  pid--;
                  chillBrother->P();
                  newSpacer->remDiskPages();
                  chillBrother->V();
                  delete newSpacer;
                  forking->V();
                }
                else {
                  t = new(std::nothrow) Thread("clone");
                  curr->next = new(std::nothrow) FamilyNode(newSpacer->pid, currentThread->space->pid, newSpacer, -1);  // Add new parent child relation to family tree.
                  
                  t->space = newSpacer; // Give child its brand new space.
                  t->SaveUserState(); // Write all current machine registers to userRegisters for child.
                  currentThread->SaveUserState(); // Save just in case the Fork gets weird.
                  machine->WriteRegister(2, newSpacer->pid); // Write the appropriate return val for parent
                  // fprintf(stderr, "newSpacer->pid: %d\n", newSpacer->pid);
                  t->Fork(CopyRegs, (int)currentThread); // Fork child.
                  (void) interrupt->SetLevel(oldLevel);
                  forking->V();

                  //Move back maybe
                  
                  // currentThread->SaveUserState(); // Save again in case of weirdness.
                  DEBUG('j', "ForkEnd\n");
                  
                }
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);  
                break;
        case SC_Exec:
                forking->P();
                oldLevel = interrupt->SetLevel(IntOff);
                DEBUG('j', "Exec\n");
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
                else{
                  open->Read(pageBuf, 12);
                  if(!strncmp(pageBuf, "#Checkpoint\n", 12)){/*Checkpoint ****************/
                    /*This is the case of reconstituting a checkpoint file*/
                    //Read in the Register Values and Put them in the registers
                    currentThread->SaveUserState();//Save the current user's registers so it can return if it fails
                    for(i=0;i<NumTotalRegs;i++){
                      j=0;
                      while(open->Read(&c, 1)){
                        if(c=='\n'){break;}
                        buffer[j]=c;
                        j++;
                        if(j>19){fprintf(stderr, "Exec Error\n");ASSERT(false);currentThread->RestoreUserState();flag = 1;break;}
                      } //***********************************************************************************************************
                      j = atoi(buffer);
                      memset(buffer, '\0', sizeof(buffer));
                      machine->WriteRegister(i, j);
                    }
                    //Read in the numPages
                    while(open->Read(&c, 1)){
                      if(c=='\n'){break;}
                      buffer[j]=c;
                      j++;
                      if(j>19){currentThread->RestoreUserState(); flag = 1;}
                    }
                    numPages = atoi(buffer);
                    memset(buffer, '\0', sizeof(buffer));
                    if(numPages<=0){currentThread->RestoreUserState(); flag = 1;}
                    oldSpacer = currentThread->space;
                    newSpacer = new(std::nothrow) AddrSpace(open, numPages, currentThread->space->pid);//AddrSpace Constructer reads in the pages
                    if(!newSpacer->enoughSpace){currentThread->RestoreUserState(); flag = 1;}
                    if(flag){fprintf(stderr, "Exec Error\n");ASSERT(false);machine->WriteRegister(2, -1);forking->V();}
                    //************************************************************************************************************
                    else{
                      //newSpacer->pid = currentThread->space->pid;
                      currentThread->space = newSpacer;
                      currentThread->space->RestoreState();
                      delete [] stringArg;
                      chillBrother->P();
                      oldSpacer->remDiskPages();//Removes the oldspace from diskpages as well as cleans up other COw stuff if necessary
                      chillBrother->V();
                      delete oldSpacer;
                      
                      currentThread->space = newSpacer;
                      
                      newSpacer->RestoreState();
                      forking->V();
                      (void) interrupt->SetLevel(oldLevel);
                      // fprintf(stderr, "I am V\n");
                      ((Semaphore *)currentThread->inKernel)->V();
                      machine->Run();
                    }
                  }/* Checkpoint End ***************/
                  else{/* Normal Exec *************/
                    open = fileSystem->Open(stringArg);
                    oldSpacer = currentThread->space;
                    newSpacer = new(std::nothrow) AddrSpace(open, currentThread->space->pid);
                    delete open;
                    //newSpacer->pid = currentThread->space->pid; // Transfer pid
                    
                    if (newSpacer->enoughSpace == 0) {
                      // There was not enough room, return a -1
                      DEBUG('a', "Not enough room to create new Address Space.\n");
                      diskBitMap->Print();
                      // fprintf(stderr, "Thingies %d\n", newSpacer->numPages);
                      /*curr = root;
                      while(curr->next !=NULL){
                        curr = curr->next;  // Iterate to find the correct semphore to V
                        if(curr->kiddo != NULL){
                          fprintf(stderr, "numPages: %d", curr->kiddo->numPages);
                        }
                        //fprintf(stderr, "pid parent %d pid child %d exit %d\n", curr->parent, curr->child, curr->exit);

                      }*/
                //fprintf(stderr, "%d", diskBitMap->NumClear());
                      machine->WriteRegister(2, -1);
                      chillBrother->P();
                      newSpacer->remDiskPages();
                      chillBrother->V();
                      (void) interrupt->SetLevel(oldLevel);
                      delete newSpacer;

                    }
                    else {
                      
                      DEBUG('j', "NormalExec\n");
                      
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
                        currentThread->space->ReadMem(whence, sizeof(int), &herece);
                        //currentThread->space->ReadMem(whence, sizeof(int), &herece);
                        if (herece == 0){
                          break;
                        }
                        //currentThread->space->ReadMem(herece, sizeof(char), (int *)&stringArg[0]);
                        for(j = 0; j < 127; j++){
                          currentThread->space->ReadMem(herece++, sizeof(char), (int *)&stringArg[j]);
                          if(stringArg[j] == '\0') break;
                        }
                        DEBUG('a', "STRINGARG %s\n",stringArg);
                        len = strlen(stringArg) + 1;
                        sp -= len;

                        currentThread->space = newSpacer;
                        currentThread->space->RestoreState();
                        for(j = 0; j < len; j++){
                          newSpacer->WriteMem(sp+j, sizeof(char), stringArg[j]);
                          //newSpacer->WriteMem(sp+j, sizeof(char), stringArg[j]);
                        }
                        argvAddr[i] = sp;
                        whence = whence + sizeof(int);

                      }
                      //DEBUG('j', "Finished Normal Exec\n");
                      argcount = i;
                      sp = sp & ~3;
                      DEBUG('a', "argcount %d\n", argcount);
                      sp -= sizeof(int) * argcount;

                      currentThread->space = newSpacer;
                      currentThread->space->RestoreState();
                      for(i = 0; i < argcount; i++){
                         newSpacer->WriteMem(sp + i*4, sizeof(int), argvAddr[i]);
                         //newSpacer->WriteMem(sp + i*4, sizeof(int), argvAddr[i]);
                      }
                      
                      delete [] stringArg;
                      chillBrother->P();
                      //DEBUG('j', "Finished Normal Exec\n");

                      oldSpacer->remDiskPages();
                      chillBrother->V();
                      delete oldSpacer;
                      
                      currentThread->space = newSpacer;
/****/                //     curr->kiddo = newSpacer;
                      newSpacer->RestoreState();
                      forking->V();
                      (void) interrupt->SetLevel(oldLevel);
                      machine->WriteRegister(4, argcount);
                      machine->WriteRegister(5, sp);

                      machine->WriteRegister(StackReg, sp - (8 * 4));
                      DEBUG('j', "Finished Normal Exec\n");
                      // fprintf(stderr, "I am V\n");
                      ((Semaphore *)currentThread->inKernel)->V();
                      machine->Run();
                    
                    }
                  }/*Normal Exec End ***************/
                }
                forking->V();
                delete [] stringArg;
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);  
                break;
        case SC_Dup:
                DEBUG('j', "Dup\n");
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
                
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                break;
        case SC_CheckPoint:
                DEBUG('j', "CheckPoint\n");
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);
                stringArg = new(std::nothrow) char[128]; // Limit on names is 128 characters
                whence = machine->ReadRegister(4); // whence is the Virtual address of first byte of arg string in the single case where virtual == physical.  We will have to translate stuff later.
                currentThread->space->ReadMem(whence, sizeof(char), (int *)&stringArg[0]);
                DEBUG('a',"String starts at address %d in user VAS\n", whence);
                for (i=0; i<127; i++){
                  currentThread->space->ReadMem(whence++, sizeof(char), (int *)&stringArg[i]); 
                }
                if(i==0){
                  DEBUG('a', "Invalid File Name: Must be longer than 0\n");
                  machine->WriteRegister(2, -1);  // If file name is invalid return -1.
                } // User puts a single \0 for the string name of the file, this should not be allowed
                stringArg[127]='\0';
                DEBUG('a', "Argument string is <%s>\n",stringArg);
                if(!fileSystem->Create(stringArg, 16)){
                  DEBUG('a', "Checkpoint Failed\n");
                  machine->WriteRegister(2, -1);  // If file name is invalid return -1.
                }

                open = fileSystem->Open(stringArg);
                if(open == NULL){
                  DEBUG('a', "Checkpoint Failed\n");
                  machine->WriteRegister(2, -1);  // Should exec really return or what.....? *****
                }
                else{
                  currentThread->space->writeBackDirty();
                  open->Write("#Checkpoint\n", 12);
                  machine->WriteRegister(2, 1);
                  for(i=0;i<NumTotalRegs;i++){
                    j = machine->ReadRegister(i);
                    pos = sprintf(buffer, "%d\n", j);
                    open->Write(buffer,pos);
                  }
                    /*end int to string*/
                
                numPages = currentThread->space->getNumPages();
                j = numPages;
                pos = sprintf(buffer, "%d\n", j);
                open->Write(buffer,pos);

                for(i=0;i<numPages;i++){/*Should write the contents of all the pages*/
                  synchDisk->ReadSector(currentThread->space->revPageTable[i].physicalPage, pageBuf);
                  open->Write(pageBuf, 128);
                }
                machine->WriteRegister(2, 0);
                }
                break;
        case SC_Send:
                mailBuffer = new (std::nothrow) char[MaxMailSize];
                // fprintf(stderr, "%d\n", sizeof(mailBuffer));
                memset(mailBuffer, 0, MaxMailSize);
                whence = machine->ReadRegister(4); // message
                size = machine->ReadRegister(5);  // length
                myMB = machine->ReadRegister(6);  // my mailbox for recieves
                machineNum = machine->ReadRegister(7); // machine
                location = machine->ReadRegister(8); // where to send the message
                //fprintf(stderr, "location %d\n", location);
                fprintf(stderr, "\nSENDING\n");
                location = 0;
                remain = size % MaxMailSize;
                if(remain > 0){
                  packetNums = size/MaxMailSize + 1;
                }else{
                  packetNums = size/MaxMailSize;
                }
                msgCTR->P();
                msgctr++;
                msgID=msgctr;
                msgCTR->V();
                // fprintf(stderr, "%d\n", MaxMailSize);
                for(ui = 0; ui < size/MaxMailSize; ui++){
                  for (uj = 0; uj < MaxMailSize; uj++) {
                    currentThread->space->ReadMem(whence++, sizeof(char), (int *)&mailBuffer[uj]);
                    //fprintf(stderr, "%c\n", mailBuffer[j]);
                  }
                  outPktHdr.to = machineNum;   
                  outMailHdr.to = location;
                  //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
                  outMailHdr.from = myMB;//1; 
                  // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
                  outMailHdr.length = MaxMailSize; // had a plus 1 here before?????????
                  outAckHdr.totalSize = packetNums;// size/MaxMailSize ; 
                  outAckHdr.curPack = ui;
                  outAckHdr.messageID = msgID;

                  //fprintf(stderr, "sending!\n");
                  //fprintf(stderr, "%s\n", mailBuffer);
                  mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, mailBuffer);
                  //perror("WHY WHY WHY");
                  // t = new(std::nothrow) Thread("messageSender");
                  //sendPacket(outPktHdr, outMailHdr, outAckHdr, mailBuffer);
                  postOffice->SendThings(mail, myMB);
                  // t->Fork(sendPacket, (int) mail);
                }

                if (remain > 0) {
                  memset(mailBuffer, 0, MaxMailSize);  // make sure maxmailsize is the same as sizeof(mailbuffer)
                  for (j = 0; j < remain; j++) {
                    currentThread->space->ReadMem(whence++, sizeof(char), (int *)&mailBuffer[j]);
                  }
                  outPktHdr.to = machineNum;   
                  outMailHdr.to = location;
                  //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
                  outMailHdr.from = myMB; 
                  // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
                  outMailHdr.length = remain; // had a plus 1 here before?????????
                  outAckHdr.totalSize = packetNums;// size;
                  outAckHdr.curPack = ui;
                  outAckHdr.messageID = msgID;
                  mail = new (std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, mailBuffer);
                  postOffice->SendThings(mail, myMB);
                  // t = new(std::nothrow) Thread("messageSender");
                  // rap = new(std::nothrow) pwrap();
                  // rap->t = t;
                  // rap->m = mail;
                  // t->Fork(sendPacket, (int) rap);
                }
                if (remain > 0) {
                  fprintf(stderr, "sent %d packets %d\n", (size/MaxMailSize) + 1, msgID);
                }
                else {
                  fprintf(stderr, "sent %d packets %d\n", (size/MaxMailSize), msgID);
                }
                delete [] mailBuffer;
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);    
                break;
        case SC_Recv:
                mailBuffer = new (std::nothrow) char[MaxMailSize];
                size = machine->ReadRegister(5);  // length
                whence = machine->ReadRegister(4); // message
                location = machine->ReadRegister(6); // location
                recved = postOffice->GrabMessage(location);
                // postOffice->Receive(location, &inPktHdr, &inMailHdr, &inAckHdr, recved);
                // // origMachine = inPktHdr.from;
                // // origID = inAckHdr.messageID;
                // bufferList = new (std::nothrow) char[recved.];
                // msgMap = new(std::nothrow) BitMap(inAckHdr.totalSize);
                // memset(bufferList, '\0', inAckHdr.totalSize);
                // for (i = 0; i < inMailHdr.length; i++) {
                //   bufferList[i + (inAckHdr.curPack * MaxMailSize)] = mailBuffer[i];
                // }
                // remain = inAckHdr.totalSize % MaxMailSize;
                // if (remain == 0) {
                //   remain = -1; 
                // }
                // else {
                //   remain = 0;
                // }
                // // fprintf(stderr, "%d\n", (inAckHdr.totalSize/MaxMailSize));
                // for (j = 0; j < (inAckHdr.totalSize / MaxMailSize) + remain;) {
                //   memset(mailBuffer, '\0', MaxMailSize);  // make sure maxmailsize is the same as sizeof(mailbuffer)
                  
                //   postOffice->Receive(location, &inPktHdr, &inMailHdr, &inAckHdr, mailBuffer);
                //   while (inPktHdr.from != origMachine && inAckHdr.messageID != origID) { 
                //     //fprintf(stderr, "multiple messages!!!!\n");
                //     //fprintf(stderr, "%d %d %d %d\n", inPktHdr.from, origMachine, inMailHdr.messageID, origID);

                //     postOffice->PutUnwanted(location, inPktHdr, inMailHdr, inAckHdr, mailBuffer);
                //     //postOffice->RestoreUnwanted(location);
                //     memset(mailBuffer, '\0', MaxMailSize);  // make sure maxmailsize is the same as sizeof(mailbuffer)
                //     // postOffice->Receive(location, &inPktHdr, &inMailHdr, &inAckHdr, mailBuffer);
                //     // postOffice->
                //   }
                //   //j = inAckHdr.curPack;
                //   if(bufferList[inAckHdr.curPack * MaxMailSize]=='\0'){
                //     j++;
                //   }
                //   for (i = 0; i < inMailHdr.length; i++) {
                //     bufferList[i + (inAckHdr.curPack * MaxMailSize)] = mailBuffer[i];
                //   }
                // }
                // //fprintf(stderr, "%s\n", bufferList);
               // fprintf(stderr, "RECEIVE");
                curNode = recved->head;
//                curMail = curNode->cur;
                ui = 0;
                incrementPC = 0;
                for (j = 0; j < size; j++) {
                  currentThread->space->WriteMem(whence++, sizeof(char), curNode->cur->data[ui]);
                  ui++;
                  if(ui == curNode->cur->mailHdr.length){
                    ui = 0;
                    incrementPC++;
                    curNode = curNode->next;
                    // if(curNode != NULL){
                    //   curMail = curNode->cur;
                    // }
                  }
                }
                fprintf(stderr, "\nRECV SUCCESS\n");
               // fprintf(stderr, "\n****************************************RECEIVE SUCCESS******************************************************\n");
                // postOffice->RestoreUnwanted(location);
                // delete [] mailBuffer;
                // delete msgMap;

                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);  
                break;
        case SC_GetMailbox:
                //fprintf(stderr, "why am i here???\n");
                whence = mailboxes->Find();
                machine->WriteRegister(2, whence);
                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);    
                break;
        case SC_Migration:

                //So basically this functionality will move into a thread that is constantly asking the clients what
                //their process load is and it can then make a decision on whether or not it needs to move some
                //processes. When this occurs, the messages sent to poll a machine should be a variation of a number in
                //the migrateID(d, which ever capitalization it is).

                //For now, this sys call, which will become the base functionality of the above, simply when called
                //messages one client that it wants a process, receives a reply with a filename in it of a chkpt file. 
                //It then forwards this message to the target client for that client to startup, from which it should 
                //recieve a reply of success, at which point the migration syscall will end.
                DEBUG('a', "Migration");
                scheduler->Print();
                whence = machine->ReadRegister(4);//`victim`
                location = machine->ReadRegister(5);//`chosen` one

                //First send to `victim` saying that it has been chosen

                msgCTR->P();
                msgctr++;
                msgID=msgctr;
                msgCTR->V(); 
                outPktHdr.to = whence;   
                outMailHdr.to = 1;
                //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
                outMailHdr.from = netname;//1; 
                // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
                outMailHdr.length = 128; // had a plus 1 here before?????????
                outAckHdr.totalSize = 1;// size/MaxMailSize ; 
                outAckHdr.curPack = 0;
                outAckHdr.messageID = msgID;
                outAckHdr.migrateFlag = 0;
                fprintf(stderr, "Starting call\n");
                mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
                postOffice->SendThings(mail, 0);
                fprintf(stderr, "Sent first message\n");

                //Next wait for its response...
                
                recved = postOffice->GrabMessage(0);
                fprintf(stderr, "Received Response\n");
                curNode = recved->head;
                recMail = curNode->cur;

                //Validate its respones? YES, change this later AKA no ASSERT(false)

                ASSERT(recMail->ackHdr.migrateFlag >= 0);

                //Forward the `victim` clients response AKA the data to the `chosen` client
                adult = recMail->ackHdr.migrateFlag;
                msgCTR->P();
                msgctr++;
                msgID=msgctr;
                msgCTR->V(); 
                outPktHdr.to = location;   
                outMailHdr.to = 1;
                //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
                outMailHdr.from = netname;//1; 
                // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
                outMailHdr.length = 128; // had a plus 1 here before?????????
                outAckHdr.totalSize = 1;// size/MaxMailSize ; 
                outAckHdr.curPack = 0;
                outAckHdr.messageID = msgID;
                outAckHdr.migrateFlag = 1;
                outAckHdr.pageID = recMail->ackHdr.pageID;

                oldPID = outAckHdr.pageID;
                fromMacher = whence;
                toMacher = location;


                //Right now I am just passing in the buffer from one message to another, this might
                //need to change to deep copying it over to a new buffer due to bad things happening...
                                msgCTR->P();
                msgctr++;
                msgID=msgctr;
                msgCTR->V(); 
                outPktHdr.to = whence;   
                outMailHdr.to = 1;
                //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
                outMailHdr.from = netname;//1; 
                // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
                outMailHdr.length = 128; // had a plus 1 here before?????????
                outAckHdr.totalSize = 1;// size/MaxMailSize ; 
                outAckHdr.curPack = 0;
                outAckHdr.messageID = msgID;
                outAckHdr.pageID = oldPID;
                outAckHdr.migrateFlag = 3;
                mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, recMail->data);
                postOffice->SendThings(mail, netname);
                fprintf(stderr, "Waiting on the chosen one\n");
                //Wait for a response from the `chosen` client 
                recved = postOffice->GrabMessage(netname);
                curFTN = foreignRoot;
                while(curFTN->next != NULL){
                    curFTN = curFTN->next;
                }
                curFTN->next = new(std::nothrow) ForeignThreadNode(oldPID, recved->head->cur->ackHdr.pageID, fromMacher, toMacher, adult);
                fprintf(stderr, "Responded\n");
                curNode = recved->head;
                recMail = curNode->cur;

                // Give us all the already exited values.






                mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, pageBuf);
                postOffice->SendThings(mail, 0);
                //If it is a success, doge is on your side
                //    much happy                          
                //                          very nachos
                //             such network               
                //     many migration             
                //                         wow            
                if(recMail->ackHdr.migrateFlag!=2){ASSERT(false);}
                //Otherwise.... you have brought great shame upon your family

                incrementPC=machine->ReadRegister(NextPCReg)+4;
                machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
                machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
                machine->WriteRegister(NextPCReg, incrementPC);    
                break;
        default:
                printf("Undefined SYSCALL %d\n", type);
                ASSERT(false); //****************************************************
      }
      break;
        case PageFaultException:
          chillBrother->P();
          pageFaultHandler(machine->ReadRegister(BadVAddrReg));
          chillBrother->V();
        break;
        case ReadOnlyException:     // Write attempted to page marked 
              // "read-only"
            chillBrother->P();
            vpn = machine->ReadRegister(BadVAddrReg) / PageSize;

            if(currentThread->space->isCowAddr() && machine->pageTable[vpn].readOnly == true){
              if(!currentThread->space->copyCowPage(machine->ReadRegister(BadVAddrReg))){
                //Go Cry in a hole
                fprintf(stderr, "Not enough space to copy a cow page into its own\n");
                ASSERT(false); //************************************************************************
              }
              chillBrother->V();
            }
            else{
              chillBrother->V();
            }
        break;
        case BusErrorException:     // Translation resulted in an 
        fprintf(stderr, "BUS ERROR EXCEPTION\n");
        //If it is the root process... HALT
        if(currentThread->space->pid == 0){interrupt->Halt();}
        //Otherwise just Exit
        forking->P();
        curr = root;
        DEBUG('j', "Thread exiting %d.\n", currentThread->space->pid);
        
        while(curr->child != currentThread->space->pid && curr->next !=NULL){
          curr = curr->next;  // Iterate to find the correct semphore to V
        }
        if(curr->child != currentThread->space->pid){
          DEBUG('a', "How the hell do you break an exit?\n");
        }
        else{
          whence = machine->ReadRegister(4); // whence is the exit value for the thread.
          curr->exit = whence;
          forking->V();
          curr->death->V();
          chillBrother->P();
          currentThread->space->remDiskPages();
          chillBrother->V();
          delete currentThread->space;
          currentThread->Finish();

          DEBUG('a', "Failed to exit.  Machine will now terminate.\n");
        }
        interrupt->Halt();
        break;
              // invalid physical address
        case AddressErrorException: // Unaligned reference or one that

        fprintf(stderr, "ADDRESS ERROR EXCEPTION\n");
        //If it is the root process... HALT
        if(currentThread->space->pid == 0){interrupt->Halt();}
        //Otherwise just Exit
        forking->P();
        curr = root;
        DEBUG('j', "Thread exiting %d.\n", currentThread->space->pid);
        
        while(curr->child != currentThread->space->pid && curr->next !=NULL){
          curr = curr->next;  // Iterate to find the correct semphore to V
        }
        if(curr->child != currentThread->space->pid){
          DEBUG('a', "How the hell do you break an exit?\n");
        }
        else{
          whence = machine->ReadRegister(4); // whence is the exit value for the thread.
          curr->exit = whence;
          forking->V();
          curr->death->V();
          chillBrother->P();
          currentThread->space->remDiskPages();
          chillBrother->V();
          delete currentThread->space;
          currentThread->Finish();

          DEBUG('a', "Failed to exit.  Machine will now terminate.\n");
        }
        fprintf(stderr, "BROKE EXIT\n");
        interrupt->Halt();
        break;
              // was beyond the end of the
              // address space
         case OverflowException:     // Integer overflow in add or sub.
         fprintf(stderr, "OVERFLOW ERROR EXCEPTION\n");
        //If it is the root process... HALT
        if(currentThread->space->pid == 0){
          interrupt->Halt();}
        //Otherwise just Exit
        forking->P();
        curr = root;
        DEBUG('j', "Thread exiting %d.\n", currentThread->space->pid);
        
        while(curr->child != currentThread->space->pid && curr->next !=NULL){
          curr = curr->next;  // Iterate to find the correct semphore to V
        }
        if(curr->child != currentThread->space->pid){
          DEBUG('a', "How the hell do you break an exit?\n");
        }
        else{
          whence = machine->ReadRegister(4); // whence is the exit value for the thread.
          curr->exit = whence;
          forking->V();
          curr->death->V();
          chillBrother->P();
          currentThread->space->remDiskPages();
          chillBrother->V();
          delete currentThread->space;
          currentThread->Finish();

          DEBUG('a', "Failed to exit.  Machine will now terminate.\n");
        }
         interrupt->Halt();
         break;

         case IllegalInstrException: // Unimplemented or reserved instr.
         fprintf(stderr, "ILLEGAL INSTRUCTION ERROR EXCEPTION\n");
         //If it is the root process... HALT
        if(currentThread->space->pid == 0){interrupt->Halt();}
        //Otherwise just Exit
        forking->P();
        curr = root;
        DEBUG('j', "Thread exiting %d.\n", currentThread->space->pid);
        
        while(curr->child != currentThread->space->pid && curr->next !=NULL){
          curr = curr->next;  // Iterate to find the correct semphore to V
        }
        if(curr->child != currentThread->space->pid){
          DEBUG('a', "How the hell do you break an exit?\n");
        }
        else{
          whence = machine->ReadRegister(4); // whence is the exit value for the thread.
          curr->exit = whence;
          forking->V();
          curr->death->V();
          chillBrother->P();
          currentThread->space->remDiskPages();
          chillBrother->V();
          delete currentThread->space;
          currentThread->Finish();

          DEBUG('a', "Failed to exit.  Machine will now terminate.\n");
        }

         interrupt->Halt();
         break;
      default: ;
    }
    // fprintf(stderr, "I am V\n");
    ((Semaphore *)currentThread->inKernel)->V();
}

#define SC_Dup    10
#define SC_CheckPoint 11
#define SC_Send 12
#define SC_Recv 13
#define SC_GetMailbox 14
#define SC_Migration 15
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

