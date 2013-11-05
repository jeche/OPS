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
{    NoffHeader noffH;
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
                        + UserStackSize ;        // we need to increase the size
                                                // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    // size = numPages * PageSize;
    // numPages = 32;
    size = numPages * PageSize;
    ASSERT(numPages <= NumPhysPages);                // check we're not trying
                                                // to run anything too big --
                                                // at least until we have
                                                // virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
                                        numPages, size);
    // bitMap->Mark(1);
    // bitMap->Mark(0);
    // bitMap->Mark(2);
    // bitMap->Mark(3);
    // bitMap->Mark(9);
#ifndef USE_TLB
// first, set up the translation
    pageTable = new(std::nothrow) TranslationEntry[numPages];
    int found = 0;

    for (i = 0; i < numPages; i++) {
        
        found = bitMap->Find();
        // fprintf(stderr, "found %d\n", found);
        if(found == -1){
            i = numPages + 1;
        }
        else{
            bzero( &machine->mainMemory[found*PageSize], PageSize); // If things are funky this is a potential screw up.
            pageTable[i].virtualPage = i;        // for now, virtual page # != phys page #
            
            pageTable[i].physicalPage = found;
            pageTable[i].valid = true;
            pageTable[i].use = false;
            pageTable[i].dirty = false;
            pageTable[i].readOnly = false; // if the code segment was entirely on
                                        // a separate page, we could set its
                                        // pages to be read-only
            bitMap->Mark(found);
            DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d,\n",
                                        i*PageSize,i, found*PageSize, found);
        }
    }
DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d, final space is 0x%x\n",
                                        (i - 1)*PageSize,(i - 1), found*PageSize, found, (found + 1)*PageSize - PageSize - 16);
#endif

// zero out the entire address space, to zero the unitialized data segment
// and the stack segment
    // bzero(machine->mainMemory, size);
    // machine->pageTable = pageTable;
    // machine->pageTableSize = numPages;
    fileDescriptors = new (std::nothrow) FileShield*[16];

    int babyAddr = 0;
    // int virtAddr, int* physAddr, int size, bool writing
    // int offset;
    // then, copy in the code and data segments into memory


    if (noffH.code.size > 0) {
        Translate(noffH.code.virtualAddr, &babyAddr, 1, false);
        // offset = (unsigned) noff.code.virtualAddr % PageSize;
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
                        noffH.code.virtualAddr, noffH.code.size);
        for(i = 0; i < noffH.code.size; i++){
            // if(j % PageSize == 0){
            //     j = 0;
            //     Translate(noffH.code.virtualAddr + PageSize * (i / PageSize), &(babyAddr), 1, false);

            // }
            Translate(noffH.code.virtualAddr + i*sizeof(char), &babyAddr, sizeof(char), false);
            executable->ReadAt(&(machine->mainMemory[babyAddr]),
                        sizeof(char), noffH.code.inFileAddr+i*sizeof(char));

        }
    }
    if (noffH.initData.size > 0) {
        Translate(noffH.initData.virtualAddr, &babyAddr, sizeof(char), false);
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
                        noffH.initData.virtualAddr, noffH.initData.size);
        for(i = 0; i < noffH.initData.size; i++){
            
            Translate(noffH.initData.virtualAddr + i*sizeof(char), &babyAddr, sizeof(char), false);
            executable->ReadAt(&(machine->mainMemory[babyAddr]),
                        sizeof(char), noffH.initData.inFileAddr+i*sizeof(char));
        }
    }

    // if (noffH.code.size > 0) {
    //     Translate(noffH.code.virtualAddr, &babyAddr, noffH.code.size, false);
    //     // offset = (unsigned) noff.code.virtualAddr % PageSize;
    //     DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
    //                     noffH.code.virtualAddr, noffH.code.size);
    //     executable->ReadAt(&(machine->mainMemory[babyAddr]),
    //                     noffH.code.size, noffH.code.inFileAddr);
    // }
    // if (noffH.initData.size > 0) {
    //     Translate(noffH.initData.virtualAddr, &babyAddr, noffH.initData.size, false);
    //     DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
    //                     noffH.initData.virtualAddr, noffH.initData.size);
    //     executable->ReadAt(&(machine->mainMemory[babyAddr]),
    //                     noffH.initData.size, noffH.initData.inFileAddr);
    // }



    
    death = (int)new(std::nothrow) Semaphore("death", 0);

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//  Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------
AddrSpace::AddrSpace(TranslationEntry *newPageTable, FileShield** avengers, int newNumPages){
    numPages = newNumPages;
    pageTable = newPageTable;
    fileDescriptors = avengers;
    death = (int)new(std::nothrow) Semaphore("death", 0);
    // // pageTable = new(std::nothrow) TranslationEntry[numPages];
    // int found = 0;
    // int i;
    // for (i = 0; i < numPagesNeeded; i++) {
        
    //     found = bitMap->Find();
    //     fprintf(stderr, "found %d\n", found);
    //     if(found == -1){
    //         i = numPages + 1;
    //     }
    //     else{
    //         bzero( &machine->mainMemory[found*PageSize], PageSize); // If things are funky this is a potential screw up.
    //         pageTable[i].virtualPage = i;        // for now, virtual page # != phys page #
            
    //         pageTable[i].physicalPage = found;
    //         pageTable[i].valid = true;
    //         pageTable[i].use = false;
    //         pageTable[i].dirty = false;
    //         pageTable[i].readOnly = false; // if the code segment was entirely on
    //                                     // a separate page, we could set its
    //                                     // pages to be read-only
    //         bitMap->Mark(found);
    //         DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d,\n",
    //                                     i*PageSize,i, found*PageSize, found);
    //     }
    // }
    // DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d, final space is 0x%x\n",
    //                                     (i - 1)*PageSize,(i - 1), found*PageSize, found, (found + 1)*PageSize - PageSize - 16);
    // fileDescriptors = new (std::nothrow) FileShield*[16];
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//  Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
#ifndef USE_TLB
    delete fileDescriptors;
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
{
    // currentThread->SaveUserState();
    // fprintf(stderr, "ohshoot\n");

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
    // fprintf(stderr, "so amaze\n");
    // currentThread->RestoreUserState();
#endif
}

bool
AddrSpace::ReadMem(int addr, int size, int *value)
{
    // fprintf(stderr, "Memory read: %d", addr);
    int data;
    ExceptionType Exception;
    int physicalAddress;
    
    DEBUG('a', "Reading VA 0x%x, size %d\n", addr, size);
    // fprintf(stderr, "ohshit %d\n", size);
    
    Exception = Translate(addr, &physicalAddress, size, false);
    if (Exception != NoException) {
    machine->RaiseException(Exception, addr);
    return false;
    }
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

bool
AddrSpace::WriteMem(int addr, int size, int value)
{
    ExceptionType Exception;
    int physicalAddress;
     
    DEBUG('a', "Writing VA 0x%x, size %d, value 0x%x\n", addr, size, value);

    Exception = Translate(addr, &physicalAddress, size, true);
    if (Exception != NoException) {
    machine->RaiseException(Exception, addr);
    return false;
    }
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

ExceptionType
AddrSpace::Translate(int virtAddr, int* physAddr, int size, bool writing) 
{
    int i;
    unsigned int vpn, offset;
    TranslationEntry *entry;
    unsigned int pageFrame;

    DEBUG('a', "\tTranslate 0x%x, %s: ", virtAddr, writing ? "write" : "read");

// check for alignment errors
    if (((size == 4) && (virtAddr & 0x3)) || ((size == 2) && (virtAddr & 0x1))){
    DEBUG('a', "alignment problem at %d, size %d!\n", virtAddr, size);
    return AddressErrorException;
    }
    
    // we must have either a TLB or a page table, but not both!
    // ASSERT(tlb == NULL || pageTable == NULL);   
    ASSERT(pageTable != NULL);   

// calculate the virtual page number, and offset within the page,
// from the virtual address
    vpn = (unsigned) virtAddr / PageSize;
    offset = (unsigned) virtAddr % PageSize;
    
    // => page table => vpn is index into table
    if (vpn >= numPages * PageSize) {
        fprintf(stderr, "virtual page # %d, %d too large for page table size %d!\n", 
            virtAddr, virtAddr, numPages * PageSize);
        // fprintf(stderr, "hohoho");
        return AddressErrorException;
    } else if (!pageTable[vpn].valid) {
        DEBUG('a', "Page table miss, virtual address  %d!\n", 
            virtAddr);
        return PageFaultException;
    }
    entry = &pageTable[vpn];
    
    if (entry == NULL) {                // not found
            DEBUG('a', "*** no valid TLB entry found for this virtual page!\n");
            return PageFaultException;      // really, this is a TLB fault,
                        // the page may be in memory,
                        // but not in the TLB
    }
    

    if (entry->readOnly && writing) {   // trying to write to a read-only page
    DEBUG('a', "%d mapped read-only at %d in TLB!\n", virtAddr, i);
    return ReadOnlyException;
    }
    pageFrame = entry->physicalPage;

    // if the pageFrame is too big, there is something really wrong! 
    // An invalid translation was loaded into the page table or TLB. 
    if (pageFrame >= NumPhysPages) { 
    DEBUG('a', "*** frame %d > %d!\n", pageFrame, NumPhysPages);
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

AddrSpace* AddrSpace::newSpace(){
    TranslationEntry *pageTable2 = new(std::nothrow) TranslationEntry[numPages];
    int found = 0;
    int i;
    // TODO: Check to make sure enough pages.
    // currentThread->SaveState();
    for (i = 0; i < numPages; i++) {
        
        found = bitMap->Find();
        // fprintf(stderr, "found %d\n", found);
        if(found == -1){
            i = numPages + 1;
        }
        else{
            bzero( &machine->mainMemory[found*PageSize], PageSize); // If things are funky this is a potential screw up.
            pageTable2[i].virtualPage = i;        // for now, virtual page # != phys page #
            
            pageTable2[i].physicalPage = found;
            for(int j = 0; j < PageSize; j ++){
                machine->mainMemory[pageTable2[i].physicalPage * PageSize + j] = machine->mainMemory[pageTable[i].physicalPage * PageSize + j];
            }
            pageTable2[i].valid = true;
            pageTable2[i].use = false;
            pageTable2[i].dirty = false;
            pageTable2[i].readOnly = false; // if the code segment was entirely on
                                        // a separate page, we could set its
                                        // pages to be read-only
            bitMap->Mark(found);
            DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d,\n",
                                        i*PageSize,i, found*PageSize, found);
        }
    }
    DEBUG('a', "Initializing address space, 0x%x virtual page %d,0x%x phys page %d, final space is 0x%x\n",
                                        (i - 1)*PageSize,(i - 1), found*PageSize, found, (found + 1)*PageSize - PageSize - 16);
    FileShield** fileDescriptors2 = new (std::nothrow) FileShield*[16];
    for(int k = 0; k < 16; k++){
        if(fileDescriptors[k] != NULL){
            fileDescriptors2[k] = fileDescriptors[k];
        }
    }
    return new(std::nothrow) AddrSpace(pageTable2, fileDescriptors2, numPages);

}

#endif // CHANGED
