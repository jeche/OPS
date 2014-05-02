#ifndef CHANGED
// addrspace.cc 
//  Routines to manage address spaces (executing user programs).
//
//  In order to run a user program, you must:
//
//  1. link with the -N -T 0 option 
//  2. run coff2noff to convert the object file to Nachos format
//      (Nachos object code format is essentially just a simpler
//      version of the UNIX executable object code format)
//  3. load the NOFF file into the Nachos file system
//      (if you haven't implemented the file system yet, you
//      don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include <new>

//----------------------------------------------------------------------
// SwapHeader
//  Do little endian to big endian conversion on the bytes in the 
//  object file header, in case the file was generated on a little
//  endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//  Create an address space to run a user program.
//  Load the program from a file "executable", and set everything
//  up so that we can start executing user instructions.
//
//  Assumes that the object code file is in NOFF format.
//
//  First, set up the translation from program memory to physical 
//  memory.  For now, this is really simple (1:1), since we are
//  only uniprogramming, and we have a single unsegmented page table
//
//  "executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int size;
#ifndef USE_TLB
    unsigned int i;
#endif

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
            + UserStackSize;    // we need to increase the size
                        // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    ASSERT(numPages <= NumPhysPages);       // check we're not trying
                        // to run anything too big --
                        // at least until we have
                        // virtual memory
    // printf("size allocation")

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
                    numPages, size);
#ifndef USE_TLB
// first, set up the translation 
    pageTable = new(std::nothrow) TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
    pageTable[i].virtualPage = i;   // for now, virtual page # = phys page #
    pageTable[i].physicalPage = i;
    pageTable[i].valid = true;
    pageTable[i].use = false;
    pageTable[i].dirty = false;
    pageTable[i].readOnly = false;  // if the code segment was entirely on 
                    // a separate page, we could set its 
                    // pages to be read-only
    }
#endif    

// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    bzero(machine->mainMemory, size);

// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
            noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
            noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
            noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
            noffH.initData.size, noffH.initData.inFileAddr);
    }

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//  Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
#ifndef USE_TLB
   delete pageTable;
#endif
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
//  Set the initial values for the user-level register set.
//
//  We write these directly into the "machine" registers, so
//  that we can immediately jump to user code.  Note that these
//  will be saved/restored into the currentThread->userRegisters
//  when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    
    int i;

    for (i = 0; i < NumTotalRegs; i++)
       machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);   

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!

    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
//  On a context switch, save any machine state, specific
//  to this address space, that needs saving.
//
//  For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//  On a context switch, restore the machine state so that
//  this address space can run.
//
//      For now, tell the machine where to find the page table,
//      IF address translation is done with a page table instead
//      of a hardware TLB.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
#ifndef USE_TLB
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
#endif

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


// addrspace.cc 
//  Routines to manage address spaces (executing user programs).
//
//  In order to run a user program, you must:
//
//  1. link with the -N -T 0 option 
//  2. run coff2noff to convert the object file to Nachos format
//      (Nachos object code format is essentially just a simpler
//      version of the UNIX executable object code format)
//  3. load the NOFF file into the Nachos file system
//      (if you haven't implemented the file system yet, you
//      don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
// #include "exception.cc"
#include <new>


//----------------------------------------------------------------------
// SwapHeader
//  Do little endian to big endian conversion on the bytes in the 
//  object file header, in case the file was generated on a little
//  endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//  Create an address space to run a user program.
//  Load the program from a file "executable", and set everything
//  up so that we can start executing user instructions.
//
//  Assumes that the object code file is in NOFF format.
//
//  First, set up the translation from program memory to physical 
//  memory.  For now, this is really simple (1:1), since we are
//  only uniprogramming, and we have a single unsegmented page table
//
//  "executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable, int PID)
{    NoffHeader noffH;
    unsigned int size;
    unsigned int i;
//*    int j;
    pid=PID;
    enoughSpace = 1;
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
                (WordToHost(noffH.noffMagic) == NOFFMAGIC))
            SwapHeader(&noffH);
    if(noffH.noffMagic != NOFFMAGIC){
        enoughSpace = 0;
    }

    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
                        + UserStackSize ;        // we need to increase the size
                                                // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
    if(numPages > NumSectors){
        enoughSpace = 0;                        // check we're not trying
    }                                           // to run anything too big


    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
                                        numPages, size);
#ifndef USE_TLB
// first, set up the translation
    pageTable = new(std::nothrow) TranslationEntry[numPages];
    revPageTable = new(std::nothrow) TranslationEntry[numPages];
    int found = 0;
    for (i = 0; i < numPages; i++) {

        if(server != -1){
            found = synchDisk->AllocPage();
        } else{
           found = diskBitMap->Find(); 
        }

        if(found == -1){
            numPages = i + 1;
            i = numPages + 1;
            enoughSpace = 0;
        }
        else{
            revPageTable[i].virtualPage = i;        
            revPageTable[i].physicalPage = found;
            revPageTable[i].valid = true;
            revPageTable[i].use = false;
            revPageTable[i].dirty = false;
            revPageTable[i].readOnly = false;
            chillBrother->P();
            pageTable[i].virtualPage = i;        
            pageTable[i].physicalPage = -1;
            pageTable[i].valid = false;
            pageTable[i].use = false;
            pageTable[i].dirty = false;
            pageTable[i].readOnly = false; 
            //fprintf(stderr, "DiskPageOrig: %d\n", found);
            
            if(diskPages[found]->getStatus() == Free){
                //diskPages[found]->setStatus(InUse);
                diskPages[found]->addAddr(this);
            }
            else{
                if(diskPages[found]->getStatus() == InUse){
                    fprintf(stderr, "Poop %d\n", found);
                }
                fprintf(stderr, "diskPages not correctly set up\n");
                DEBUG('a', "Tried to use a diskPage already in use\n");
            }
            chillBrother->V();
            // diskBitMap->Mark(found);
            DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d,\n",
                                        i*PageSize,i, found*PageSize, found);
        }
    }
DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d, final space is 0x%x\n",
                                        (i - 1)*PageSize,(i - 1), found*PageSize, found, (found + 1)*PageSize - PageSize - 16);
#endif

    fileDescriptors = new (std::nothrow) FileShield*[16];
    // Specially creates the fileShields for ConsoleInput and ConsoleOutput and sets their values to -1 for checking in Read and Write
    fileDescriptors[0] = new FileShield();
    fileDescriptors[1] = new FileShield();
    fileDescriptors[0]->inOut = -1;
    fileDescriptors[1]->inOut = -1; 
    fileDescriptors[0]->CopyFile();
    fileDescriptors[1]->CopyFile();
    for(i = 2; i < 16; i++){
        fileDescriptors[i] = NULL;
    }

    

    if(enoughSpace == 1){
        char strbuf[128];
        memset(strbuf, '\0', sizeof(strbuf));
        unsigned int count = 0;
        int page = 0;
        int m;
        bool lastToWrite = false;
        IntStatus oldLevel = interrupt->SetLevel(IntOff);
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
            noffH.code.virtualAddr, noffH.code.size);
            DEBUG('p', "StartOfCodeAndInit");
        for(m = 0; m < noffH.code.size; m++){
            executable->ReadAt(&strbuf[count], sizeof(char), noffH.code.inFileAddr + (m* sizeof(char)));
            count++;
            if(count == 128){
                    for(i = 0; i < 128; i++ ){
                        //DEBUG('p', "%c", strbuf[i]);
                    }
                pageTable[page].readOnly = false;//I was TRUE!!!!
                synchDisk->WriteSector(revPageTable[page].physicalPage, strbuf);
                // fprintf(stderr, "pageNum %d\n", page);
                page++;

                count = 0;
                memset(strbuf, '\0', sizeof(strbuf));
            }
        }
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
            noffH.initData.virtualAddr, noffH.initData.size);
//*        unsigned int poo;
//*        poo = count;
        
        for(m = 0; m < noffH.initData.size; m++){
            executable->ReadAt(&strbuf[count], sizeof(char), noffH.initData.inFileAddr+ m * sizeof(char));
            count++;
            if(count == 128){
                for(i = 0; i < 128; i++ ){
                   DEBUG('p', "%c", strbuf[i]);
               }                
                pageTable[page].readOnly = false;
                synchDisk->WriteSector(revPageTable[page].physicalPage, strbuf);
                page++;
                count = 0;
                memset(strbuf, '\0', sizeof(strbuf));
                lastToWrite = true;
//*                poo = 0;
            }
            else{            
                lastToWrite = false;
            }
        }
        if(lastToWrite == false){
           for(i = 0; i < count; i++ ){
                DEBUG('p', "%c", strbuf[i]);
            }
            synchDisk->WriteSector(revPageTable[page].physicalPage, strbuf);
            page++;
            count = 0;
            memset(strbuf, '\0', sizeof(strbuf));
        }
        DEBUG('p', "EndOfCodeAndInit");
        interrupt->SetLevel(oldLevel);
    //delete strbuf;
    }
    clean = false;

}

AddrSpace::AddrSpace(OpenFile *chkpt, int numpages, int PID){
    unsigned int i, readnum;
    int found;
    char sectorBuf[128];
    pid=PID;
    enoughSpace=1;
    numPages=numpages;
    pageTable = new(std::nothrow) TranslationEntry[numPages];
    revPageTable = new(std::nothrow) TranslationEntry[numPages];
    for(i = 0; i<numPages; i++){
        PacketHeader outPktHdr;
        MailHeader outMailHdr;
        AckHeader outAckHdr;
        Mail* mail;
        char data[2];
        int msgID;
        if(server != -1){
            found = synchDisk->AllocPage();
           // msgCTR->P();
           // msgctr++;
           // msgID=msgctr;
           // msgCTR->V(); 
           // outPktHdr.to = server;   
           // outMailHdr.to = netname;
           // //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
           // outMailHdr.from = 0;//1; 
           // // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
           // outMailHdr.length = 2; // had a plus 1 here before?????????
           // outAckHdr.totalSize = 1;// size/MaxMailSize ; 
           // outAckHdr.curPack = 0;
           // outAckHdr.messageID = msgID;
           // outAckHdr.pageID = 0;
           // mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, data);
           // postOffice->SendThings(mail, 0);
           // // postOffice->Send(outPktHdr, outMailHdr, outAckHdr, mail->data);
           // // delete mail;
           // // fprintf(stderr, "Write Before\n");
           // MessageNode* message = postOffice->GrabMessage(0);
           // MailNode* curNode = message->head;
           // Mail* curMail = curNode->cur;
           // found = curMail->ackHdr.pageID;
           // delete curNode;
           // delete message;
        } else{
           found = diskBitMap->Find(); 
        }
        if(found == -1){
            numPages = i+1;
            i=numPages+1;
            enoughSpace=0;
        }
        else{
            revPageTable[i].virtualPage = i;       
            revPageTable[i].physicalPage = found;
            revPageTable[i].valid = true;
            revPageTable[i].use = false;
            revPageTable[i].dirty = false;
            revPageTable[i].readOnly = false;

            pageTable[i].virtualPage = i;       
            pageTable[i].physicalPage = -1;
            pageTable[i].valid = false;
            pageTable[i].use = false;
            pageTable[i].dirty = false;
            pageTable[i].readOnly = false;

            if(diskPages[found]->getStatus() == Free){
                //diskPages[found]->setStatus(InUse);
                diskPages[found]->addAddr(this);
            }
            else{
                fprintf(stderr, "diskPages not correctly set up\n");
                DEBUG('a', "Tried to use a diskPage already in use\n");
            }
        }
    }
    fileDescriptors = new (std::nothrow) FileShield*[16];
    // Specially creates the fileShields for ConsoleInput and ConsoleOutput and sets their values to -1 for checking in Read and Write
    fileDescriptors[0] = new FileShield();
    fileDescriptors[1] = new FileShield();
    fileDescriptors[0]->inOut = -1;
    fileDescriptors[1]->inOut = -1; 
    fileDescriptors[0]->CopyFile();
    fileDescriptors[1]->CopyFile();
    for(i = 2; i < 16; i++){
        fileDescriptors[i] = NULL;
    }
    /*Sector Replacement Start*/
    for(i=0; i<numPages;i++){
        // fprintf(stderr, "count %d\n", i);
        readnum = chkpt->Read(sectorBuf, 128);
        if(readnum!=128){fprintf(stderr, "Incomplete Read count %d numPages %d\n", i, numPages);ASSERT(false);enoughSpace=0;break;}
        synchDisk->WriteSector(revPageTable[i].physicalPage, sectorBuf);
    }
    /*Sector Replacement End*/
    // delete sectorBuf;
    clean = false;

}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//  Used by newSpace to properly initialize a new address space for a 
//  forked process
//----------------------------------------------------------------------
AddrSpace::AddrSpace(TranslationEntry *newPageTable, TranslationEntry *newRevPageTable, FileShield** avengers, int newNumPages, int newEnoughSpace, bool isCow, int PID){
    numPages = newNumPages;
    revPageTable = newRevPageTable;
    pageTable = newPageTable;
    fileDescriptors = avengers;
    enoughSpace = newEnoughSpace;
    clean = false;
    pid = PID;
    //fprintf(stderr, "numPages: %d\n", numPages);
    //cow=isCow;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//  Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
#ifndef USE_TLB
    chillBrother->P();
    AddrSpace *other;
    //fprintf(stderr, "dying, my pid was: %d\n", pid);
    
    for(unsigned int i = 0; i < numPages; i++){
        if(pageTable[i].readOnly){
            other = diskPages[revPageTable[i].physicalPage]->otherAddr(this);
            if(other != NULL){
                other->pageTable[i].readOnly = false;
                if(other->pageTable[i].valid &&ramPages[other->pageTable[i].physicalPage]->getStatus() == CowPage){
                    ramPages[other->pageTable[i].physicalPage]->setStatus(InUse);
                    ramPages[other->pageTable[i].physicalPage]->head = other;
                    ramPages[other->pageTable[i].physicalPage]->pid = other->pid;
                }
            }
            else{ASSERT(false);} //********************************************
        }else{
            if(pageTable[i].valid){
                ramPages[pageTable[i].physicalPage]->setStatus(Free);
                ramPages[pageTable[i].physicalPage]->head = NULL;
                ramPages[pageTable[i].physicalPage]->pid = -1;
            }
            if (server != -1){
                synchDisk->ClearPage(revPageTable[i].physicalPage);
            } else{
                diskBitMap->Clear(revPageTable[i].physicalPage);
            }
        }
        diskPages[revPageTable[i].physicalPage]->setStatus(Free);
        diskPages[revPageTable[i].physicalPage]->removeAddr(this);
    }
    delete[] fileDescriptors;
    delete[] pageTable;
    delete[] revPageTable;
    chillBrother->V();

#endif
}

//----------------------------------------------------------------------
// --DEPRICATED--
// AddrSpace::Clean
//  Goes through a process's virtual space and clears out the bit 
//  map for the pages the process had.
// --DEPRICATED--
//----------------------------------------------------------------------
// --DEPRICATED--
void AddrSpace::Clean()
{
    fprintf(stderr, "Warning: Use of Depricated Method: AddrSpace::Clean\n");
    clean = true;
    for(unsigned int i = 0; i < numPages; i++){
        bitMap->Clear(pageTable[i].physicalPage);
    }
}
// --DEPRICATED--

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
//  Set the initial values for the user-level register set.
//
//  We write these directly into the "machine" registers, so
//  that we can immediately jump to user code.  Note that these
//  will be saved/restored into the currentThread->userRegisters
//  when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{

    int i;

    for (i = 0; i < NumTotalRegs; i++)
    machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);   

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);

}

//----------------------------------------------------------------------
// AddrSpace::SaveState
//  On a context switch, save any machine state, specific
//  to this address space, that needs saving.
//
//  For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{

}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//  On a context switch, restore the machine state so that
//  this address space can run.
//
//      For now, tell the machine where to find the page table,
//      IF address translation is done with a page table instead
//      of a hardware TLB.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
#ifndef USE_TLB
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
#endif
}

//----------------------------------------------------------------------
// AddrSpace::ReadMem
//  Our version of ReadMem.
//
//  Read "size" (1, 2, or 4) bytes of virtual memory at "addr" into 
//  the location pointed to by "value".
//
//      Returns FALSE if the translation step from virtual to physical memory
//      failed.
//
//  "addr" -- the virtual address to read from
//  "size" -- the number of bytes to read (1, 2, or 4)
//  "value" -- the place to write the result 
//----------------------------------------------------------------------

bool
AddrSpace::ReadMem(int addr, int size, int *value)
{
    int data;
//    ExceptionType Exception;
    int physicalAddress;
    
    DEBUG('a', "Reading VA 0x%x, size %d\n", addr, size);
    
    /*Exception = */Translate(addr, &physicalAddress, size, false);

    switch (size) {
      case 1:
    data = machine->mainMemory[physicalAddress];
    *value = data;
    break;
    
      case 2:
    data = *(unsigned short *) &machine->mainMemory[physicalAddress];
    *value = ShortToHost(data);
    break;
    
      case 4:
    data = *(unsigned int *) &machine->mainMemory[physicalAddress];
    *value = WordToHost(data);
    break;

      default: ASSERT(false);
    }
    
    DEBUG('a', "\tvalue read = %8.8x\n", *value);
    return (true);
}

//----------------------------------------------------------------------
// AddrSpace::WriteMem
//   Our version of WriteMem
//
//      Write "size" (1, 2, or 4) bytes of the contents of "value" into
//  virtual memory at location "addr".
//
//      Returns FALSE if the translation step from virtual to physical memory
//      failed.
//
//  "addr" -- the virtual address to write to
//  "size" -- the number of bytes to be written (1, 2, or 4)
//  "value" -- the data to be written
//----------------------------------------------------------------------

bool
AddrSpace::WriteMem(int addr, int size, int value)
{
//*    ExceptionType Exception;
    int physicalAddress;
     
    DEBUG('a', "Writing VA 0x%x, size %d, value 0x%x\n", addr, size, value);

    /*Exception = */Translate(addr, &physicalAddress, size, true);

    switch (size) {
      case 1:
    machine->mainMemory[physicalAddress] = (unsigned char) (value & 0xff);
    break;

      case 2:
    *(unsigned short *) &machine->mainMemory[physicalAddress]
        = ShortToMachine((unsigned short) (value & 0xffff));
    break;
      
      case 4:
    *(unsigned int *) &machine->mainMemory[physicalAddress]
        = WordToMachine((unsigned int) value);
    break;
    
      default: ASSERT(false);
    }

    return true;
}

//----------------------------------------------------------------------
// AddrSpace::Translate
//  Our version of Translate.
//
//  Translate a virtual address into a physical address, using 
//  either a page table or a TLB.  Check for alignment and all sorts 
//  of other errors, and if everything is ok, set the use/dirty bits in 
//  the translation table entry, and store the translated physical 
//  address in "physAddr".  If there was an error, returns the type
//  of the exception.
//
//  "virtAddr" -- the virtual address to translate
//  "physAddr" -- the place to store the physical address
//  "size" -- the amount of memory being read or written
//  "writing" -- if TRUE, check the "read-only" bit in the TLB
//----------------------------------------------------------------------

ExceptionType
AddrSpace::Translate(int virtAddr, int* physAddr, int size, bool writing) 
{

    //int i;
    unsigned int vpn, offset;
    TranslationEntry *entry;
    unsigned int pageFrame;

    DEBUG('a', "\tTranslate 0x%x, %s: ", virtAddr, writing ? "write" : "read");

    // check for alignment errors
    if (((size == 4) && (virtAddr & 0x3)) || ((size == 2) && (virtAddr & 0x1))){
        DEBUG('a', "alignment problem at %d, size %d!\n", virtAddr, size);
        return AddressErrorException;
    }
     
    ASSERT(pageTable != NULL);   

    // calculate the virtual page number, and offset within the page,
    // from the virtual address
    vpn = (unsigned) virtAddr / PageSize;
    offset = (unsigned) virtAddr % PageSize;
    if(offset + size == 128){
        //************************************************************** What should be done here?
    }
    
    // => page table => vpn is index into table
    if (vpn >= numPages * PageSize) {
        DEBUG('y', "Machine AddressErrorException");
        DEBUG('a', "virtual page # %d, %d too large for page table size %d!\n", 
            virtAddr, virtAddr, numPages * PageSize);
        return AddressErrorException;
    } else if (!pageTable[vpn].valid) {
        DEBUG('a', "Page table miss, virtual address  %d!\n", 
            virtAddr);
        chillBrother->P();
        pageFaultHandler2(virtAddr);
        chillBrother->V();
    }
    entry = &(this->pageTable[vpn]);
    
    if (entry == NULL) {                // not found
            DEBUG('a', "*** no valid TLB entry found for this virtual page!\n");
            ASSERT(false);
            interrupt->Halt();
            ASSERT(false);
    }
    

    if (entry->readOnly && writing) {   // trying to write to a read-only page
        chillBrother->P();
        this->copyCowPage(virtAddr);
        chillBrother->V();
        ASSERT(false);
        interrupt->Halt();
        return ReadOnlyException;
    }
    pageFrame = entry->physicalPage;

    // if the pageFrame is too big, there is something really wrong! 
    // An invalid translation was loaded into the page table or TLB. 
    if (pageFrame >= NumPhysPages) { 
    DEBUG('a', "ADDRSPACE ");
    DEBUG('a', "*** frame %d > %d!\n", pageFrame, NumPhysPages);
    ASSERT(false);
    interrupt->Halt();
    return BusErrorException;
    }

    entry->use = false;     // set the use, dirty bits
    if (writing)
    entry->dirty = true;
    *physAddr = pageFrame * PageSize + offset;
    ASSERT((*physAddr >= 0) && ((*physAddr + size) <= MemorySize));
    DEBUG('a', "phys addr = 0x%x\n", *physAddr);
    return NoException;
}

unsigned int AddrSpace::getNumPages(){
    return numPages;
}

int AddrSpace::findReplacement2(){
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
      if (ramPages[commutator]->getStatus() == Free) {
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
              if (ramPages[commutator]->head == NULL) {
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
              if (ramPages[commutator]->head == NULL) {
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
        if(ramPages[commutator]->getStatus() != MarkedForReplacement){
          found = commutator;
          ramPages[found]->head->pageTable[ramPages[found]->vPage].valid = false;
          ramPages[found]->head->pageTable[ramPages[found]->vPage].physicalPage = -1; 
          AddrSpace *other = diskPages[ramPages[found]->head->revPageTable[ramPages[found]->vPage].physicalPage]->otherAddr(ramPages[found]->head);
          if(other!= NULL){
            other->pageTable[ramPages[found]->vPage].valid = false;
            other->pageTable[ramPages[found]->vPage].physicalPage = -1;
          }
          ASSERT(ramPages[found]->head->isCowAddr());
          ramPages[found]->setStatus(MarkedForReplacement);
          return found; 
        }
    }
}

void AddrSpace::pageFaultHandler2(int badVAddr) {
  char* stringArg;
  stringArg = new(std::nothrow) char[128];
  int vpn = badVAddr / PageSize;
  stats->numPageFaults++;
  if(!machine->pageTable[vpn].valid){
    int victim = findReplacement2();
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
    synchDisk->ReadSector(this->revPageTable[vpn].physicalPage, stringArg);
    for(int q = 0; q < PageSize; q++){
      machine->mainMemory[victim * PageSize + q] = stringArg[q];
    }
    ramPages[victim]->vPage = vpn;
    ramPages[victim]->pid = this->pid;
    ramPages[victim]->head = this;
    
    if (diskPages[this->revPageTable[vpn].physicalPage]->getStatus() == CowPage || pageTable[vpn].readOnly) {
        AddrSpace *other = diskPages[this->revPageTable[vpn].physicalPage]->otherAddr(this);
        other->pageTable[vpn].valid = true;
        other->pageTable[vpn].physicalPage = victim;
        ramPages[victim]->setStatus(CowPage);
    }else{
        ramPages[victim]->setStatus(InUse);
    }
    this->pageTable[vpn].valid = true;
    this->pageTable[vpn].physicalPage = victim;
            // fprintf(stderr, "Oh poop\n");
    DEBUG('j', "PageFaultHandled\n");
  } else{
    if(diskPages[currentThread->space->revPageTable[vpn].physicalPage]->getStatus() != CowPage){
      fprintf(stderr, "I'm a goofy goober yeah.  You're a goofy goober yeah.  We're all goofy goober yeah.  Goofy goobers yeah yeah\n");
      ASSERT(false);  
    }
  }
  delete[] stringArg;
}

//----------------------------------------------------------------------
// AddrSpace::newSpace
//  Creates a new address space for a process that has been forked
//  If there is not enough space to create the new process, the 
//  addrspace returns a -1 as the number of pages.  The created 
//  addrspace will be deleted in the Fork if there is not enough space.
//----------------------------------------------------------------------

AddrSpace* AddrSpace::newSpace(int PID){
    TranslationEntry *pageTable2 = new(std::nothrow) TranslationEntry[numPages];
    TranslationEntry *revPageTable2 = new(std::nothrow) TranslationEntry[numPages];
    FileShield** fileDescriptors2 = new (std::nothrow) FileShield*[16];
    AddrSpace* NewSpace;
    int found = 0;
    unsigned int i;
    unsigned int j;

    if (diskBitMap->NumClear() < (signed)numPages) {//if issues then we have a lot of numPages
        // We don't have enough pages to make a new address space, return and address space with a -1 for numPages
        return new(std::nothrow) AddrSpace(pageTable2, revPageTable2, fileDescriptors2, numPages, 0, false, PID);
    }

    for (i = 0; i < numPages; i++) {
        PacketHeader outPktHdr;
        MailHeader outMailHdr;
        AckHeader outAckHdr;
        Mail* mail;
        int msgID;
        char data[2];
        if(server != -1){
            found = synchDisk->AllocPage();
           // msgCTR->P();
           // msgctr++;
           // msgID=msgctr;
           // msgCTR->V(); 
           // outPktHdr.to = server;   
           // outMailHdr.to = netname;
           // //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
           // outMailHdr.from = 0;//1; 
           // // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
           // outMailHdr.length = 2; // had a plus 1 here before?????????
           // outAckHdr.totalSize = 1;// size/MaxMailSize ; 
           // outAckHdr.curPack = 0;
           // outAckHdr.messageID = msgID;
           // outAckHdr.pageID = 0;
           // mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, data);
           // postOffice->SendThings(mail, 0);
           // // postOffice->Send(outPktHdr, outMailHdr, outAckHdr, mail->data);
           // // delete mail;
           // // fprintf(stderr, "Write Before\n");
           // MessageNode* message = postOffice->GrabMessage(0);
           // MailNode* curNode = message->head;
           // Mail* curMail = curNode->cur;
           // found = curMail->ackHdr.pageID;
           // delete curNode;
           // delete message;
        } else{
           found = diskBitMap->Find(); 
        }

        if(found == -1){
            i = numPages + 1;
            enoughSpace = 0;
        }
        else{

            char pagebuf[128];

            revPageTable2[i].virtualPage = i;        
            revPageTable2[i].physicalPage = found;
            revPageTable2[i].valid = true;
            revPageTable2[i].use = false;
            revPageTable2[i].dirty = false;
            revPageTable2[i].readOnly = false;

            // IntStatus oldLevel;
            // oldLevel = interrupt->SetLevel(IntOff); // disable interrupts
            chillBrother->P();
            if(pageTable[i].dirty == true && pageTable[i].valid == true){
                for(j = 0; j < PageSize; j ++){
                    pagebuf[j] = machine->mainMemory[pageTable[i].physicalPage * PageSize + j];
                }    
            }else{
                synchDisk->ReadSector(revPageTable[i].physicalPage, pagebuf); 
                // interrupt->SetLevel(IntOff);   
            }
            synchDisk->WriteSector(revPageTable2[i].physicalPage, pagebuf);
            chillBrother->V();
            // interrupt->SetLevel(IntOff);
            // (void) interrupt->SetLevel(oldLevel); // re-enable interrupts

            pageTable2[i].virtualPage = i;       
            pageTable2[i].physicalPage = -1;
            pageTable2[i].valid = false;
            pageTable2[i].use = false;
            pageTable2[i].dirty = false;
            pageTable2[i].readOnly = false; 
            // diskBitMap->Mark(found);
            DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d,\n",
                                        i*PageSize,i, found*PageSize, found);
             //delete pagebuf;
        }
    }
    DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d, final space is 0x%x\n",
                                        (i - 1)*PageSize,(i - 1), found*PageSize, found, (found + 1)*PageSize - PageSize - 16);
    
    // Copies all the open file descriptors from parent to child
    for(int k = 0; k < 16; k++){
        if(fileDescriptors[k] != NULL){
            fileDescriptors[k]->CopyFile(); // Increases the reference count for the file
            fileDescriptors2[k] = fileDescriptors[k];
            fileDescriptors2[k]->inOut = fileDescriptors[k]->inOut;
        }else{
            fileDescriptors2[k] = NULL;
        }
    }
    if(numPages == 23){
	   DEBUG('r', "NumPages is 23 for pid\n");
    }else{
        DEBUG('r', "NumPages is %d\n", numPages);
    }
    NewSpace = new(std::nothrow) AddrSpace(pageTable2, revPageTable2, fileDescriptors2, numPages, enoughSpace, false, PID);
    for(i = 0; i < numPages; i++){
        //diskPages[revPageTable2[i].physicalPage]->setStatus(InUse);
        diskPages[revPageTable2[i].physicalPage]->addAddr(NewSpace);
    }
    return NewSpace;
}

/*
CowSpace What it needs to do
    copy page tables from parent*
    make parent a cow*
    add addrspace to the diskpage*
    increase refcount for all the diskpages*
    mark pages as read only*
    change all pages in main mem to cow pages*
    make status of new addrspace cow*
    create a new addrspace and return it*
*/
AddrSpace* AddrSpace::cowSpace(int PID){
    //return newSpace(PID);

    TranslationEntry *pageTable2 = new(std::nothrow) TranslationEntry[numPages];
    TranslationEntry *revPageTable2 = new(std::nothrow) TranslationEntry[numPages];
    FileShield** fileDescriptors2 = new (std::nothrow) FileShield*[16];
    AddrSpace* CowAddrSpace = NULL;
    
    // if (diskBitMap->NumClear() < numPages) {
    //     // We don't have enough pages to make a new address space, return and address space with a -1 for numPages
    //     return new(std::nothrow) AddrSpace(pageTable2, revPageTable2, fileDescriptors2, numPages, 0, false);
    // }
    //Copy the pageTable, revPageTable, and fileDescriptors
    for(unsigned int i = 0; i < numPages; i++){
        revPageTable2[i].virtualPage = revPageTable[i].virtualPage;        
        revPageTable2[i].physicalPage = revPageTable[i].physicalPage;
        revPageTable2[i].valid = revPageTable[i].valid;
        revPageTable2[i].use = revPageTable[i].use;
        revPageTable2[i].dirty = revPageTable[i].dirty;
        revPageTable2[i].readOnly = revPageTable[i].readOnly;
        if(revPageTable[i].readOnly){fprintf(stderr, "BadThings, Cow aspects in non cow proc\n");}
        //Set both revPageTables to ReadOnly

        //revPageTable2[i].readOnly = true;
        //revPageTable[i].readOnly = true;

        pageTable2[i].virtualPage = pageTable[i].virtualPage;       
        pageTable2[i].physicalPage = pageTable[i].physicalPage;
        pageTable2[i].valid = pageTable[i].valid;
        pageTable2[i].use = pageTable[i].use;
        pageTable2[i].dirty = pageTable[i].dirty;
        if(pageTable[i].readOnly){fprintf(stderr, "BadThings, Cow aspects in non cow proc\n");}
        //Set both Pagetables to ReadOnly //???
        pageTable2[i].readOnly = true;
        pageTable[i].readOnly = true;

        // if(pageTable[i].dirty && pageTable[i].valid){
        //     char pageBuf[128];
        //     for(int q = 0; q < PageSize; q++){
        //            pageBuf[q] = machine->mainMemory[pageTable[i].physicalPage * PageSize + q];
        //     }
        //     synchDisk->WriteSector(revPageTable[i].physicalPage, pageBuf);
        //     pageTable[i].dirty = false;
        //     pageTable2[i].dirty = false;
        // }
        if(pageTable[i].valid){
            ramPages[pageTable[i].physicalPage]->setStatus(CowPage);
        }
    }

    // Copies all the open file descriptors from parent to child
    for(int k = 0; k < 16; k++){
        if(fileDescriptors[k] != NULL){
            fileDescriptors[k]->CopyFile(); // Increases the reference count for the file
            fileDescriptors2[k] = fileDescriptors[k];
            fileDescriptors2[k]->inOut = fileDescriptors[k]->inOut;
        }else{
            fileDescriptors2[k] = NULL;
        }
    }
    //Changing the Status to Cow pages in RamPages
    for(int i = 0; i<NumPhysPages;i++){
        if(pageTable[ramPages[i]->vPage].valid == true && ramPages[i]->pid == this->pid){
            ramPages[i]->setStatus(CowPage);
        }
    }
    enoughSpace=1;
    //End of cowSpace, setting parent to be a cow and returning the new addrspace with isCow true
    
    CowAddrSpace = new(std::nothrow) AddrSpace(pageTable2, revPageTable2, fileDescriptors2, numPages, enoughSpace, true, PID);
    //Adding the CowAddrSpace to the addrs associatied with diskPages
    for(unsigned int i = 0; i < numPages; i++){
        //fprintf(stderr, "DiskPageCow: %d\n", revPageTable[i].physicalPage);
        //diskPages[revPageTable[i].physicalPage]->setStatus(CowPage);
        diskPages[revPageTable[i].physicalPage]->addAddr(CowAddrSpace);
        if(diskPages[revPageTable[i].physicalPage]->getStatus()!=CowPage){fprintf(stderr, "DiskPages not in sync, Cowspace\n");}
    }
    return CowAddrSpace;
}
void AddrSpace::remDiskPages(){
    
    //fprintf(stdout, "just removed pid: %d\n", this->pid);
    //printAllDiskPages();

}

bool AddrSpace::writeBackDirty(){
    unsigned int i;
    char pageBuf[128];
    
    for(i=0;i<NumPhysPages;i++){
        if(pageTable[ramPages[i]->vPage].valid == true && pageTable[ramPages[i]->vPage].dirty == true){
            if(ramPages[i]->pid == /*currentThread->space->pid*/this->pid ){
                ramPages[i]->setStatus(MarkedForReplacement);
                ramPages[i]->head->pageTable[ramPages[i]->vPage].dirty = false;
                for(int q = 0; q < PageSize; q++){
                   pageBuf[q] = machine->mainMemory[i * PageSize + q];

                }
                allThreads->Mapcar((VoidFunctionPtr) ThreadPrint);
                //ASSERT(false);

                synchDisk->WriteSector(ramPages[i]->head->revPageTable[ramPages[i]->vPage].physicalPage, pageBuf);
                ramPages[i]->setStatus(InUse);

            }
        }
        fprintf(stderr, "page: %d\n", i);
    }
    
    return true;
}


void AddrSpace::printAllPages(){
    unsigned int i;
    char pageBuf[128];

    for(i=0;i<numPages;i++){
        synchDisk->ReadSector(revPageTable[i].physicalPage, pageBuf);
        for(int j = 0; j < 128; j++){
         DEBUG('y', "%c", pageBuf[j]);   
        }
    }
     //delete pageBuf;
}
void AddrSpace::printAllDiskPages(){
    for(int i = 0; i < 35; i++){
        fprintf(stdout, "diskPage[%d]\n", i);
        diskPages[i]->displayPage();
    }

}

int AddrSpace::copyCowPage(int rOPage){
       // printAllDiskPages();
    //fprintf(stdout, "Am i the only one? pid: %d\n", this->pid);
    int vpn = rOPage / PageSize;
    int found = 0;
     PacketHeader outPktHdr;
     MailHeader outMailHdr;
     AckHeader outAckHdr;
     Mail* mail;
     int msgID;
     char data[2];
     if(server != -1){
        found = synchDisk->AllocPage();
        // msgCTR->P();
        // msgctr++;
        // msgID=msgctr;
        // msgCTR->V(); 
        // outPktHdr.to = server;   
        // outMailHdr.to = netname;
        // //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
        // outMailHdr.from = 0;//1; 
        // // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
        // outMailHdr.length = 2; // had a plus 1 here before?????????
        // outAckHdr.totalSize = 1;// size/MaxMailSize ; 
        // outAckHdr.curPack = 0;
        // outAckHdr.messageID = msgID;
        // outAckHdr.pageID = 0;
        // mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, data);
        // postOffice->SendThings(mail, 0);
        // // postOffice->Send(outPktHdr, outMailHdr, outAckHdr, mail->data);
        // // delete mail;
        // // fprintf(stderr, "Write Before\n");
        // MessageNode* message = postOffice->GrabMessage(0);
        // MailNode* curNode = message->head;
        // Mail* curMail = curNode->cur;


        // found = curMail->ackHdr.pageID;
        // delete curNode;
        // delete message;
     } else{
        found = diskBitMap->Find(); 
     }
    //fprintf(stdout, "vpn: %d\n", vpn);
    ASSERT(pageTable[vpn].readOnly);
    if(found == -1){
        return 0;
    }
    char pagebuf[128];

    //Changing in the Other Process
    AddrSpace *other = diskPages[revPageTable[vpn].physicalPage]->otherAddr(this);
    if(other->pid==this->pid){return 0;} //********************************************************************

    if(pageTable[vpn].dirty && pageTable[vpn].valid){
            // char pageBuf[128];
        for(int q = 0; q < PageSize; q++){
                   pagebuf[q] = machine->mainMemory[ pageTable[vpn].physicalPage * PageSize + q];
        }
        // synchDisk->WriteSector(revPageTable[vpn].physicalPage, pagebuf);
        pageTable[vpn].dirty = false;
        // ASSERT(false);
    } else if(pageTable[vpn].valid){
        for(int q = 0; q < PageSize; q++){
                   pagebuf[q] = machine->mainMemory[ pageTable[vpn].physicalPage * PageSize + q];
        }
    }else{
        synchDisk->ReadSector(revPageTable[vpn].physicalPage, pagebuf); 
    }
    synchDisk->WriteSector(found, pagebuf);
    diskPages[revPageTable[vpn].physicalPage]->removeAddr(this);
    // ASSERT(pageTable[vpn].valid);
    // ASSERT(other->pageTable[vpn].valid);
    // ASSERT(!other->pageTable[vpn].dirty);
    // ASSERT(!pageTable[vpn].dirty);
    revPageTable[vpn].physicalPage = found;
    other->pageTable[vpn].readOnly = false;
    pageTable[vpn].valid = false;
    // other->pageTable[vpn].valid = false;
    
    pageTable[vpn].readOnly = false; 
    // diskBitMap->Mark(found);
    diskPages[found]->addAddr(this);
    if(other->pageTable[vpn].valid){
        ramPages[pageTable[vpn].physicalPage]->head = other;
        ramPages[pageTable[vpn].physicalPage]->pid = other->pid;
        ramPages[pageTable[vpn].physicalPage]->setStatus(InUse);
    }
    // memset(pagebuf, '\0', sizeof(pagebuf));
    ASSERT(revPageTable[vpn].physicalPage != other->revPageTable[vpn].physicalPage);
    // fprintf(stderr, "Faulted pid newguy %d, still valid %d, vpn: %d\n", pid, other->pid, vpn);
    //printAllDiskPages();
    return 1;

}
bool AddrSpace::isCowAddr(){
    for(unsigned int i = 0; i < numPages; i++){
        if(diskPages[revPageTable[i].physicalPage]->getStatus() == CowPage || pageTable[i].readOnly){return true;}
    }
    return false;
}




#endif // CHANGED
