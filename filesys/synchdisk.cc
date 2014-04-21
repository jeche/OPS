#ifndef CHANGED
// synchdisk.cc 
//	Routines to synchronously access the disk.  The physical disk 
//	is an asynchronous device (disk requests return immediately, and
//	an interrupt happens later on).  This is a layer on top of
//	the disk providing a synchronous interface (requests wait until
//	the request completes).
//
//	Use a semaphore to synchronize the interrupt handlers with the
//	pending requests.  And, because the physical disk can only
//	handle one operation at a time, use a lock to enforce mutual
//	exclusion.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchdisk.h"
#include <new>

//----------------------------------------------------------------------
// DiskRequestDone
// 	Disk interrupt handler.  Need this to be a C routine, because 
//	C++ can't handle pointers to member functions.
//----------------------------------------------------------------------

static void
DiskRequestDone (int arg)
{
    SynchDisk* disk = (SynchDisk *)arg;

    disk->RequestDone();
}

//----------------------------------------------------------------------
// SynchDisk::SynchDisk
// 	Initialize the synchronous interface to the physical disk, in turn
//	initializing the physical disk.
//
//	"name" -- UNIX file name to be used as storage for the disk data
//	   (usually, "DISK")
//----------------------------------------------------------------------

SynchDisk::SynchDisk(char* name)
{
    semaphore = new(std::nothrow) Semaphore("synch disk", 0);
    lock = new(std::nothrow) Lock("synch disk lock");
    disk = new(std::nothrow) Disk(name, DiskRequestDone, (int) this);
}

//----------------------------------------------------------------------
// SynchDisk::~SynchDisk
// 	De-allocate data structures needed for the synchronous disk
//	abstraction.
//----------------------------------------------------------------------

SynchDisk::~SynchDisk()
{
    delete disk;
    delete lock;
    delete semaphore;
}

//----------------------------------------------------------------------
// SynchDisk::ReadSector
// 	Read the contents of a disk sector into a buffer.  Return only
//	after the data has been read.
//
//	"sectorNumber" -- the disk sector to read
//	"data" -- the buffer to hold the contents of the disk sector
//----------------------------------------------------------------------

void
SynchDisk::ReadSector(int sectorNumber, char* data)
{
    lock->Acquire();			// only one disk I/O at a time
    disk->ReadRequest(sectorNumber, data);
    semaphore->P();			// wait for interrupt
    lock->Release();
}

//----------------------------------------------------------------------
// SynchDisk::WriteSector
// 	Write the contents of a buffer into a disk sector.  Return only
//	after the data has been written.
//
//	"sectorNumber" -- the disk sector to be written
//	"data" -- the new contents of the disk sector
//----------------------------------------------------------------------

void
SynchDisk::WriteSector(int sectorNumber, char* data)
{
    lock->Acquire();			// only one disk I/O at a time
    disk->WriteRequest(sectorNumber, data);
    semaphore->P();			// wait for interrupt
    lock->Release();
}

//----------------------------------------------------------------------
// SynchDisk::RequestDone
// 	Disk interrupt handler.  Wake up any thread waiting for the disk
//	request to finish.
//----------------------------------------------------------------------

void
SynchDisk::RequestDone()
{ 
    semaphore->V();
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
// synchdisk.cc 
//  Routines to synchronously access the disk.  The physical disk 
//  is an asynchronous device (disk requests return immediately, and
//  an interrupt happens later on).  This is a layer on top of
//  the disk providing a synchronous interface (requests wait until
//  the request completes).
//
//  Use a semaphore to synchronize the interrupt handlers with the
//  pending requests.  And, because the physical disk can only
//  handle one operation at a time, use a lock to enforce mutual
//  exclusion.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchdisk.h"
#include <new>
#include "system.h"

//----------------------------------------------------------------------
// DiskRequestDone
//  Disk interrupt handler.  Need this to be a C routine, because 
//  C++ can't handle pointers to member functions.
//----------------------------------------------------------------------

static void
DiskRequestDone (int arg)
{
    SynchDisk* disk = (SynchDisk *)arg;

    disk->RequestDone();
}

//----------------------------------------------------------------------
// SynchDisk::SynchDisk
//  Initialize the synchronous interface to the physical disk, in turn
//  initializing the physical disk.
//
//  "name" -- UNIX file name to be used as storage for the disk data
//     (usually, "DISK")
//----------------------------------------------------------------------

SynchDisk::SynchDisk(char* name)
{
    semaphore = new(std::nothrow) Semaphore("synch disk", 0);
    lock = new(std::nothrow) Lock("synch disk lock");        
    if(server == -1){
        disk = new(std::nothrow) Disk(name, DiskRequestDone, (int) this);
    }

    
}

//----------------------------------------------------------------------
// SynchDisk::~SynchDisk
//  De-allocate data structures needed for the synchronous disk
//  abstraction.
//----------------------------------------------------------------------

SynchDisk::~SynchDisk()
{
    if(server != -1){
        delete disk;
    }
    delete lock;
    delete semaphore;
}

//----------------------------------------------------------------------
// SynchDisk::ReadSector
//  Read the contents of a disk sector into a buffer.  Return only
//  after the data has been read.
//
//  "sectorNumber" -- the disk sector to read
//  "data" -- the buffer to hold the contents of the disk sector
//----------------------------------------------------------------------

void
SynchDisk::ReadSector(int sectorNumber, char* data)
{
    PacketHeader outPktHdr;
    MailHeader outMailHdr;
    AckHeader outAckHdr;

    Mail* mail;
    int msgID;
    lock->Acquire();            // only one disk I/O at a time
    if(server != -1){
        msgCTR->P();
        msgctr++;
        msgID=msgctr;
        msgCTR->V(); 
        outPktHdr.to = server;   
        outMailHdr.to = netname;
        //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
        outMailHdr.from = 0;//1; 
        // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
        outMailHdr.length = 1; // had a plus 1 here before?????????
        outAckHdr.totalSize = 1;// size/MaxMailSize ; 
        outAckHdr.curPack = 0;
        outAckHdr.messageID = msgID;
        outAckHdr.pageID = sectorNumber;
        mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, data);
        postOffice->SendThings(mail, 0);
        fprintf(stderr, "Read Before %d\n", msgID);
        MessageNode* message = postOffice->GrabMessage(0);
        fprintf(stderr, "Read After\n");
        MailNode* curNode = message->head;
        Mail* curMail = curNode->cur;
        for(int i = 0; i < 128; i++){
            data[i] = curMail->data[i];
        }

    }
    else{
        
        disk->ReadRequest(sectorNumber, data);
        semaphore->P();         // wait for interrupt
        
    }
    lock->Release();
}

//----------------------------------------------------------------------
// SynchDisk::WriteSector
//  Write the contents of a buffer into a disk sector.  Return only
//  after the data has been written.
//
//  "sectorNumber" -- the disk sector to be written
//  "data" -- the new contents of the disk sector
//----------------------------------------------------------------------

void
SynchDisk::WriteSector(int sectorNumber, char* data)
{
    PacketHeader outPktHdr;
    MailHeader outMailHdr;
    AckHeader outAckHdr;
    Mail* mail;
    int msgID;
    lock->Acquire();            // only one disk I/O at a time
    if(server != -1){
        msgCTR->P();
        msgctr++;
        msgID=msgctr;
        msgCTR->V(); 
        outPktHdr.to = server;   
        outMailHdr.to = netname;
        //fprintf(stderr, "mailheader.to %d\n", outMailHdr.to);
        outMailHdr.from = 0;//1; 
        // fprintf(stderr, "add something to addrspace to denote which mailbox belongs to which process\n"); 
        outMailHdr.length = 128; // had a plus 1 here before?????????
        outAckHdr.totalSize = 1;// size/MaxMailSize ; 
        outAckHdr.curPack = 0;
        outAckHdr.messageID = msgID;
        outAckHdr.pageID = sectorNumber;
        mail = new(std::nothrow) Mail(outPktHdr, outMailHdr, outAckHdr, data);
        postOffice->SendThings(mail, 0);
        fprintf(stderr, "Write Before\n");
        MessageNode* message = postOffice->GrabMessage(0);
        fprintf(stderr, "Write After\n");
    }
    else{
        
        disk->WriteRequest(sectorNumber, data);
        semaphore->P();         // wait for interrupt
        
    }
    lock->Release();

}

//----------------------------------------------------------------------
// SynchDisk::RequestDone
//  Disk interrupt handler.  Wake up any thread waiting for the disk
//  request to finish.
//----------------------------------------------------------------------

void
SynchDisk::RequestDone()
{ 
    semaphore->V();
}
#endif