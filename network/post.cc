// post.cc 
// 	Routines to deliver incoming network messages to the correct
//	"address" -- a mailbox, or a holding area for incoming messages.
//	This module operates just like the US postal service (in other
//	words, it works, but it's slow, and you can't really be sure if
//	your mail really got through!).
//
//	Note that once we prepend the MailHdr to the outgoing message data,
//	the combination (MailHdr plus data) looks like "data" to the Network 
//	device.
//
// 	The implementation synchronizes incoming messages with threads
//	waiting for those messages.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "post.h"

//----------------------------------------------------------------------
// Mail::Mail
//      Initialize a single mail message, by concatenating the headers to
//	the data.
//
//	"pktH" -- source, destination machine ID's
//	"mailH" -- source, destination mailbox ID's
//	"data" -- payload data
//----------------------------------------------------------------------

Mail::Mail(PacketHeader pktH, MailHeader mailH, AckHeader ackH, char *msgData)
{
    ASSERT(mailH.length <= MaxMailSize);
    pktHdr = pktH;
    mailHdr = mailH;
    ackHdr = ackH;
    bcopy(msgData, data, mailHdr.length);
}


MailNode::MailNode(Mail *mail){
    cur = mail;
    next = NULL;
    prev = NULL;
}

MailNode::~MailNode(){
    delete cur;
}

void
MailNode::Append(MailNode *mn){
    if(cur == NULL){
        cur = mn->cur;
    }
    else{
        if(next!=NULL){next->Append(mn);}
        else{
            next = mn;
        }
    }
}
void
MailNode::Remove(MailNode *mn){
    if( mn->prev != NULL && mn->next == NULL){
        mn->prev->next = NULL;
        delete mn;
    }
    else if(mn->prev != NULL && mn->next != NULL){
        mn->prev->next = mn->next;
        mn->next->prev = mn->prev;
        delete mn;
    }
}

//----------------------------------------------------------------------
// MailBox::MailBox
//      Initialize a single mail box within the post office, so that it
//	can receive incoming messages.
//
//	Just initialize a list of messages, representing the mailbox.
//----------------------------------------------------------------------


MailBox::MailBox()
{ 
    messages = new(std::nothrow) SynchList();
    unwantedMessages = new(std::nothrow) SynchList(); 
    acks = new(std::nothrow) MailNode(NULL);
    ackLock = new(std::nothrow) Lock("ackLock");
    ackProcLock = new(std::nothrow) Lock("ackProcLock");
    hasAck = new(std::nothrow) Condition("hasAck");
    ackResolved = new(std::nothrow) Condition("ackResolved");
}

//----------------------------------------------------------------------
// MailBox::~MailBox
//      De-allocate a single mail box within the post office.
//
//	Just delete the mailbox, and throw away all the queued messages 
//	in the mailbox.
//----------------------------------------------------------------------

MailBox::~MailBox()
{ 
    delete messages; 
    delete unwantedMessages;
    delete acks;
    delete ackLock;
    delete ackProcLock;
    delete hasAck;
    delete ackResolved;
}

//----------------------------------------------------------------------
// PrintHeader
// 	Print the message header -- the destination machine ID and mailbox
//	#, source machine ID and mailbox #, and message length.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//----------------------------------------------------------------------

static void 
PrintHeader(PacketHeader pktHdr, MailHeader mailHdr)
{
    printf("From (%d, %d) to (%d, %d) bytes %d\n",
    	    pktHdr.from, mailHdr.from, pktHdr.to, mailHdr.to, mailHdr.length);
}

//----------------------------------------------------------------------
// MailBox::Put
// 	Add a message to the mailbox.  If anyone is waiting for message
//	arrival, wake them up!
//
//	We need to reconstruct the Mail message (by concatenating the headers
//	to the data), to simplify queueing the message on the SynchList.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//	"data" -- payload message data
//----------------------------------------------------------------------

void 
MailBox::Put(PacketHeader pktHdr, MailHeader mailHdr, AckHeader ackHdr, char *data)
{ 
    Mail *mail = new(std::nothrow) Mail(pktHdr, mailHdr, ackHdr, data); 
    // TODO ******

    messages->Append((void *)mail);	// put on the end of the list of 
					// arrived messages, and wake up 
					// any waiters
}

//----------------------------------------------------------------------
// MailBox::Get
// 	Get a message from a mailbox, parsing it into the packet header,
//	mailbox header, and data. 
//
//	The calling thread waits if there are no messages in the mailbox.
//
//	"pktHdr" -- address to put: source, destination machine ID's
//	"mailHdr" -- address to put: source, destination mailbox ID's
//	"data" -- address to put: payload message data
//----------------------------------------------------------------------

void 
MailBox::Get(PacketHeader *pktHdr, MailHeader *mailHdr, AckHeader *ackHdr, char *data) 
{ 
    DEBUG('n', "Waiting for mail in mailbox\n");

    Mail *mail = (Mail *) messages->Remove();	// remove message from list;
						// will wait if list is empty

    *pktHdr = mail->pktHdr;
    *mailHdr = mail->mailHdr;
    *ackHdr = mail->ackHdr;
    if (DebugIsEnabled('n')) {
	printf("Got mail from mailbox: ");
	PrintHeader(*pktHdr, *mailHdr);
    }
    bcopy(mail->data, data, mail->mailHdr.length);
					// copy the message data into
					// the caller's buffer
    delete mail;			// we've copied out the stuff we
					// need, we can now discard the message
}


void
MailBox::PutAck(PacketHeader pktHdr, MailHeader mailHdr, AckHeader ackHdr, char *data){
    Mail *mail = new(std::nothrow) Mail(pktHdr, mailHdr, ackHdr, data); 
    MailNode *mn = new(std::nothrow) MailNode(mail);
    acks->Append(mn);
    ackLock->Acquire();
    hasAck->Broadcast(ackLock);
    ackLock->Release();
}

int

MailBox::CheckAckMB(int msgID, int fromMach, int toMach, int fromBox, int toBox, int cPack){
    Mail *temp;
    MailNode *curMN;
    curMN  = acks;

    ASSERT(ackLock->isHeldByCurrentThread());

    if(curMN->cur == NULL){
        curMN = NULL;
    }

    while(curMN != NULL){//This could be an issue if the size of acks changes duing th eiteratio thorugh, it shouldn't though
        temp = curMN->cur;
        if((temp->ackHdr.messageID == msgID) && (temp->mailHdr.from == toBox) &&
            (temp->mailHdr.to == fromBox) && 
            (temp->pktHdr.from == toMach) && (temp->ackHdr.curPack == cPack)){ // Removed this (temp->pktHdr.to == fromMach) &&
            curMN->Remove(curMN);
            return 1;
        }
        curMN = curMN->next;
    }
    hasAck->Signal(ackLock);
    return 0;

}



//----------------------------------------------------------------------
// PostalHelper, ReadAvail, WriteDone
// 	Dummy functions because C++ can't indirectly invoke member functions
//	The first is forked as part of the "postal worker thread; the
//	later two are called by the network interrupt handler.
//
//	"arg" -- pointer to the Post Office managing the Network
//----------------------------------------------------------------------

static void PostalHelper(int arg)
{ PostOffice* po = (PostOffice *) arg; po->PostalDelivery(); }
static void ReadAvail(int arg)
{ PostOffice* po = (PostOffice *) arg; po->IncomingPacket(); }
static void WriteDone(int arg)
{ PostOffice* po = (PostOffice *) arg; po->PacketSent(); }

//----------------------------------------------------------------------
// PostOffice::PostOffice
// 	Initialize a post office as a collection of mailboxes.
//	Also initialize the network device, to allow post offices
//	on different machines to deliver messages to one another.
//
//      We use a separate thread "the postal worker" to wait for messages 
//	to arrive, and deliver them to the correct mailbox.  Note that
//	delivering messages to the mailboxes can't be done directly
//	by the interrupt handlers, because it requires a Lock.
//
//	"addr" is this machine's network ID 
//	"reliability" is the probability that a network packet will
//	  be delivered (e.g., reliability = 1 means the network never
//	  drops any packets; reliability = 0 means the network never
//	  delivers any packets)
//	"nBoxes" is the number of mail boxes in this Post Office
//----------------------------------------------------------------------

PostOffice::PostOffice(NetworkAddress addr, double reliability, int nBoxes)
{
// First, initialize the synchronization with the interrupt handlers
    messageAvailable = new(std::nothrow) Semaphore("message available", 0);
    messageSent = new(std::nothrow) Semaphore("message sent", 0);
    sendLock = new(std::nothrow) Lock("message send lock");

// Second, initialize the mailboxes
    netAddr = addr; 
    numBoxes = nBoxes;
    boxes = new(std::nothrow) MailBox[nBoxes];

// Third, initialize the network; tell it which interrupt handlers to call
    network = new(std::nothrow) Network(addr, reliability, ReadAvail, WriteDone, (int) this);


// Finally, create a thread whose sole job is to wait for incoming messages,
//   and put them in the right mailbox. 
    Thread *t = new(std::nothrow) Thread("postal worker");

    t->Fork(PostalHelper, (int) this);
}

//----------------------------------------------------------------------
// PostOffice::~PostOffice
// 	De-allocate the post office data structures.
//----------------------------------------------------------------------

PostOffice::~PostOffice()
{
    delete network;
    delete [] boxes;
    delete messageAvailable;
    delete messageSent;
    delete sendLock;
}

//----------------------------------------------------------------------
// PostOffice::PostalDelivery
// 	Wait for incoming messages, and put them in the right mailbox.
//
//      Incoming messages have had the PacketHeader stripped off,
//	but the MailHeader is still tacked on the front of the data.
//----------------------------------------------------------------------

void
PostOffice::PostalDelivery()
{
    PacketHeader pktHdr;
    MailHeader mailHdr;
    AckHeader ackHdr;
    char *buffer = new(std::nothrow) char[MaxPacketSize];

    for (;;) {
        // first, wait for a message
        messageAvailable->P();	
        pktHdr = network->Receive(buffer);

        mailHdr = *(MailHeader *)buffer;
        ackHdr = *(AckHeader *)(buffer + sizeof(MailHeader));  // not sure if this is how you would do this...
        
        if (DebugIsEnabled('n')) {
    	    printf("Putting mail into mailbox: ");
    	    PrintHeader(pktHdr, mailHdr);
            fprintf(stderr, "\ncurpack %d\n", ackHdr.curPack);
        }

	// check that arriving message is legal!
    	ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
    	ASSERT(mailHdr.length <= MaxMailSize);

    	// put into mailbox
        if(ackHdr.totalSize!=-1){
            /*Need to Ack-Back*/
            int pktTemp = pktHdr.to;
            pktHdr.to = pktHdr.from;
            //pktHdr.from = NULL;

            int tempSize = ackHdr.totalSize; // Need to save this variable in order pull it into a buffer of the right size on the recieve
            ackHdr.totalSize = -1;
            int mailTemp = mailHdr.to;
            mailHdr.to = mailHdr.from;
            mailHdr.from = mailTemp;
            // fprintf(stderr, "sent magic message %d %d\n", ackHdr.messageID, ackHdr.curPack);
            this->Send(pktHdr, mailHdr, ackHdr, buffer + sizeof(MailHeader) + sizeof(ackHdr));
            // Reset the variables for to put in the mailbox
            ackHdr.totalSize = tempSize; 
            mailHdr.from = mailHdr.to;
            mailHdr.to = mailTemp;
            pktHdr.from = pktHdr.to;
            pktHdr.to = pktTemp;
            boxes[mailTemp].Put(pktHdr, mailHdr, ackHdr, buffer + sizeof(MailHeader) + sizeof(AckHeader));
            //This should be done in a separate thread....

            /*Signal the appropriate condition variable
                **REMEMBER YOU HAVE TO GRAB THE APPROPRIATE LOCK THEN YOU CAN USE THE COND VAR
            */
            //boxes[box].hasAck->Signal(boxes[box].ackLock);
        }
        else{
            boxes[mailHdr.to].PutAck(pktHdr, mailHdr, ackHdr, buffer + sizeof(MailHeader) + sizeof(AckHeader));

        }
    }
}

//----------------------------------------------------------------------
// PostOffice::Send
// 	Concatenate the MailHeader to the front of the data, and pass 
//	the result to the Network for delivery to the destination machine.
//
//	Note that the MailHeader + data looks just like normal payload
//	data to the Network.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//	"data" -- payload message data
//----------------------------------------------------------------------

void
PostOffice::Send(PacketHeader pktHdr, MailHeader mailHdr, AckHeader ackHdr, char* data)
{
    char *buffer = new(std::nothrow) char[MaxPacketSize];	// space to hold concatenated
						// mailHdr + data
    if (DebugIsEnabled('n')) {
	printf("Post send: ");
	PrintHeader(pktHdr, mailHdr);
    }
    ASSERT(mailHdr.length <= MaxMailSize);
    ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
    
    // fill in pktHdr, for the Network layer
    pktHdr.from = netAddr;
    pktHdr.length = mailHdr.length + sizeof(MailHeader) + sizeof(AckHeader);

    // concatenate MailHeader and data
    bcopy((char *) &mailHdr, buffer, sizeof(MailHeader));
    bcopy((char *) &ackHdr, buffer + sizeof(MailHeader), sizeof(AckHeader));
    bcopy(data, buffer + sizeof(MailHeader) + sizeof(AckHeader), mailHdr.length);
    sendLock->Acquire();   		// only one message can be sent
			// to the network at any one time
    //fprintf(stderr, "here %s\n", data);
    network->Send(pktHdr, buffer);
    messageSent->P();			// wait for interrupt to tell us
					// ok to send the next message
    sendLock->Release();

    delete [] buffer;			// we've sent the message, so
					// we can delete our buffer
    ackLockAcquire(mailHdr.from);
    hasAckSignal(mailHdr.from);
    ackLockRelease(mailHdr.from);
}

//----------------------------------------------------------------------
// PostOffice::Send
// 	Retrieve a message from a specific box if one is available, 
//	otherwise wait for a message to arrive in the box.
//
//	Note that the MailHeader + data looks just like normal payload
//	data to the Network.
//
//
//	"box" -- mailbox ID in which to look for message
//	"pktHdr" -- address to put: source, destination machine ID's
//	"mailHdr" -- address to put: source, destination mailbox ID's
//	"data" -- address to put: payload message data
//----------------------------------------------------------------------

void
PostOffice::Receive(int box, PacketHeader *pktHdr, 
				MailHeader *mailHdr, AckHeader *ackHdr, char* data)
{
    ASSERT((box >= 0) && (box < numBoxes));

    boxes[box].Get(pktHdr, mailHdr, ackHdr, data);

    ASSERT(mailHdr->length <= MaxMailSize);
}

//----------------------------------------------------------------------
// PostOffice::IncomingPacket
// 	Interrupt handler, called when a packet arrives from the network.
//
//	Signal the PostalDelivery routine that it is time to get to work!
//----------------------------------------------------------------------

void
PostOffice::IncomingPacket()
{ 
    messageAvailable->V(); 
}

//----------------------------------------------------------------------
// PostOffice::PacketSent
// 	Interrupt handler, called when the next packet can be put onto the 
//	network.
//
//	The name of this routine is a misnomer; if "reliability < 1",
//	the packet could have been dropped by the network, so it won't get
//	through.
//----------------------------------------------------------------------

void 
PostOffice::PacketSent()
{ 
    messageSent->V();
}

void
PostOffice::PutUnwanted(int box, PacketHeader pktHdr, MailHeader mailHdr, AckHeader ackHdr, char *data)
{ 
    Mail *mail = new(std::nothrow) Mail(pktHdr, mailHdr, ackHdr, data); 
    // TODO ******

    boxes[box].unwantedMessages->Append((void *)mail); // put on the end of the list of 
                    // arrived messages, and wake up 
                    // any waiters
}

void
PostOffice::RestoreUnwanted(int box)
{
    while (boxes[box].unwantedMessages->Peek() != 0) {
        Mail *mail = (Mail *) boxes[box].unwantedMessages->Remove();
        fprintf(stderr, "Putting mail back!!!\n");
        boxes[box].Put(mail->pktHdr, mail->mailHdr, mail->ackHdr, mail->data);
    }
}

int
PostOffice::CheckAckPO(int box, int msgID, int fromMach, int toMach, int fromBox, int toBox, int cPack){
    return boxes[box].CheckAckMB(msgID, fromMach, toMach, fromBox, toBox, cPack);
}

void
PostOffice::hasAckWait(int box){
    boxes[box].hasAck->Wait(boxes[box].ackLock);
}

void 
PostOffice::hasAckSignal(int box){
    boxes[box].hasAck->Broadcast(boxes[box].ackLock);
}

void 
PostOffice::ackResolvedWait(int box){
    boxes[box].ackResolved->Wait(boxes[box].ackProcLock);
}

void 
PostOffice::ackResolvedSignal(int box){
    boxes[box].ackResolved->Signal(boxes[box].ackProcLock);

}

void
PostOffice::ackLockAcquire(int box){
    boxes[box].ackLock->Acquire();
}
void
PostOffice::ackLockRelease(int box){
    boxes[box].ackLock->Release();
}
void
PostOffice::ackProcLockAcquire(int box){
    boxes[box].ackProcLock->Acquire();
}
void
PostOffice::ackProcLockRelease(int box){
    boxes[box].ackProcLock->Release();
}
