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
    curPack = mail->ackHdr.curPack;
}

MailNode::~MailNode(){
    delete cur;
}


MessageNode::MessageNode(Mail *mail){
    head = new (std::nothrow) MailNode(mail);
    msgID = mail->ackHdr.messageID;
    fromMachine = mail->pktHdr.from;
    fromBox = mail->mailHdr.from;
    totalSize = mail->ackHdr.totalSize;
    finished = 0;
}

void
MailNode::Append(MailNode *mn){
    if(cur == NULL){
        cur = mn->cur;
        curPack = mn->curPack;
        return;
    }
    MailNode* temp = this;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = mn;
    mn->prev = temp;

    // if(cur == NULL){
    //     cur = mn->cur;
    // }
    // else{
    //     if(next!=NULL){next->Append(mn);}
    //     else{
    //         mn->prev = this;
    //         next = mn;
    //     }
    // }
}

int MailNode::Find(MailNode *mn){
    if(cur == NULL || mn == NULL || mn->cur == NULL){
        return 0;
    }
    MailNode *temp = this;
    if(mn->curPack == temp->curPack){
        return 1;
    }
    while(temp->next != NULL){
        if(mn->curPack == temp->curPack){
            return 1;
        }
        temp = temp->next;
    }
    
    if(mn->curPack == temp->curPack){
        return 1;
    }    
    return 0;
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


static void SendHelper(int arg)
{ MailBox* po = (MailBox *) arg; po->SendPackets(); }

static void CompleteHelper(int arg)
{ MailBox* po = (MailBox *) arg; po->CompleteMessages(); }

static void AckHelper(int arg)
{ MailBox* po = (MailBox *) arg; po->ackAttackSend(); }

MailBox::MailBox()
{ 
    messages = new(std::nothrow) SynchList();
    unwantedMessages = new(std::nothrow) SynchList(); 
    
    curmsg = NULL; // new(std::nothrow) MailNode(NULL);
    ackLock = new(std::nothrow) Lock("ackLock");
    hasAck = new(std::nothrow) Condition("hasAck");
    head = new(std::nothrow) HistoryNode(-1, -1);
    // notRightAck = new(std::nothrow) SynchList();
    ackList = new(std::nothrow) SynchList();
    retAck = new(std::nothrow) SynchList();
    sendList = new(std::nothrow) SynchList();
    completeList = new(std::nothrow) SynchList();
    tempMessages = new(std::nothrow) SynchList();
    sendThread = new (std::nothrow) Thread("mailbox sender");
    sendThread->Fork(SendHelper,(int) this);
    recvThread = new(std::nothrow) Thread("mailbox receiver");
    recvThread->Fork(CompleteHelper, (int)this);
    ackAttack = new (std::nothrow) Thread("mailbox ackattack sender");
    ackAttack->Fork(AckHelper, (int)this);

}

//----------------------------------------------------------------------
// MailBox::~MailBox
//      De-allocate a single mail box within the post office.
//
//	Just delete the mailbox, and throw away all the queued messages 
//	in the mailbox.
//----------------------------------------------------------------------
void MailBox::ackAttackSend(){
    MailHeader mailHdr;
    PacketHeader pktHdr;
    AckHeader ackHdr;
    Mail* m;
    for(;;){
        m = (Mail *)retAck->Remove();
        mailHdr = m->mailHdr;
        pktHdr = m->pktHdr;
        ackHdr = m->ackHdr;
        char *data = m->data;
        ((PostOffice* )post)->Send(pktHdr, mailHdr, ackHdr, data);
    }
}


void MailBox::SendPackets(){
    MailHeader mailHdr;
    PacketHeader pktHdr;
    AckHeader ackHdr;
    Mail* m;
    for(;;){
        // Remove a message ready for sendings
        m = (Mail *)sendList->Remove();
        // Prep to send it to the post office
        mailHdr = m->mailHdr;
        pktHdr = m->pktHdr;
        ackHdr = m->ackHdr;
        char *data = m->data;
        // GO LITTLE MESSAGE!  BE FREE!
        ((PostOffice* )post)->Send(pktHdr, mailHdr, ackHdr, data);
        // Try to remove an ack.  Looking for my ack.  WHERE IS MY ACK BACK? 
        m = (Mail *) ackList->Remove(); // timeout will add an invalid ack packet
        while(m->mailHdr.length == (unsigned)-1 || m->pktHdr.from != pktHdr.to || m->mailHdr.from != mailHdr.to){
            // keep trying to send same packet until it goes through
            // ASSERT(false);
            // ASSERT(ackHdr.curPack == m->ackHdr.curPack);
            // sending Ack
            ASSERT(m->ackHdr.totalSize == -1);
            ASSERT(mailHdr.length != (unsigned)-1 && ackHdr.totalSize != -1);
            ((PostOffice* )post)->Send(pktHdr, mailHdr, ackHdr, data);
            m = (Mail *) ackList->Remove();
            if(m->pktHdr.from != pktHdr.to && m->mailHdr.from != mailHdr.to &&m->mailHdr.from != -1 &&m->pktHdr.from != -1){
                ackList->Append((void*)m);
            }
        }
        ASSERT(m->pktHdr.from == pktHdr.to && m->mailHdr.from == mailHdr.to);
    }
}


void MailBox::CompleteMessages(){
    Mail *m;
    //MessageNode* temp;
    Mail *temp;
//*    int tempInt;
    int msgComplete;
    MailHeader mailHdr;
    int flag = 0;
    PacketHeader pktHdr;
    AckHeader ackHdr;
//*    Mail *ackMail;
    HistoryNode *kemper;
    HistoryNode *temper;
//*    char* data;
    for(;;){
        m = (Mail *) messages->Remove();
        // flip information for Ack sending
        mailHdr = m->mailHdr;
        pktHdr = m->pktHdr;
        ackHdr = m->ackHdr;
        MailNode * mn = new (std::nothrow) MailNode(m);
        temper = head;
        flag = 0;
        while(temper != NULL){
            if(ackHdr.messageID == temper->msgID && pktHdr.from == temper->machineID && curmsg == NULL){
                flag = 1;
                // ASSERT(false);
                break;
            }
            temper = temper->next;
        } // check and see if in cur msg already.  is this the message we are processing, nad if it is do
        if(curmsg != NULL && m->pktHdr.from == curmsg->fromMachine && m->mailHdr.from == curmsg->fromBox && m->ackHdr.messageID == curmsg->msgID && curmsg->head->Find(mn) == 1){
            flag = 1;
        }
        if(flag == 0){
            if(curmsg == NULL){
                
                // while(ackHdr.curPack != 0){
                //     temper = head;
                //     flag = 0;
                //     while(temper != NULL){
                //         if(ackHdr.messageID == temper->msgID && pktHdr.from == temper->machineID){
                //             flag = 1;
                //             // ASSERT(false);
                //             break;
                //         }
                //         temper = temper->next;
                //     }
                //     if(flag == 0){
                //         unwantedMessages->Append((void *) m);
                //     }
                //     m = (Mail *) messages->Remove();
                //     mailHdr = m->mailHdr;
                //     pktHdr = m->pktHdr;
                //     ackHdr = m->ackHdr;
                // }
                ASSERT(ackHdr.curPack == 0);

                // temper = head;
                // flag = 0;
                // while(temper != NULL){
                //     if(ackHdr.messageID == temper->msgID && pktHdr.from == temper->machineID){
                //         flag = 1;
                //         // ASSERT(false);
                //         break;
                //     }
                //     temper = temper->next;
                // }
                // if(flag == 0){
                curmsg = new (std::nothrow) MessageNode(m);
                curmsg->finished = curmsg->finished + 1;
                // }

            }
            else if(m->pktHdr.from == curmsg->fromMachine && m->mailHdr.from == curmsg->fromBox && m->ackHdr.messageID == curmsg->msgID && curmsg->head->Find(mn) == 0){
                // if this is the correct message to attach to attach
                // MailNode * mn = new (std::nothrow) MailNode(m);
                mn = new (std::nothrow) MailNode(m);
                curmsg->head->Append(mn);
                // add one to the amount we have finished receiving
                curmsg->finished = curmsg->finished + 1;
            } else if (m->pktHdr.from != curmsg->fromMachine && m->mailHdr.from != curmsg->fromBox && m->ackHdr.messageID != curmsg->msgID &&curmsg->head->Find(mn) != 1) {
                temper = head;
                flag = 0;
                while(temper != NULL){
                    if(ackHdr.messageID == temper->msgID && pktHdr.from == temper->machineID){
                        flag = 1;
                        ASSERT(false);
                        break;
                    }
                    temper = temper->next;
                }
                if(flag == 0){
                    unwantedMessages->Append((void *) m);
                }
            } else{
                ASSERT(false);
            }

            if(curmsg != NULL && curmsg->finished >= curmsg->totalSize){
                // temper = head;
                // flag = 0;
                // while(temper != NULL){
                //     if (m->ackHdr.messageID == temper->msgID && m->pktHdr.from == temper->machineID) {//if(ackHdr.messageID == temper->msgID && pktHdr.from == temper->machineID){
                //         flag = 1;
                //         // ASSERT(false);
                //         break;
                //     }
                //     temper = temper->next;
                // }
                // if(flag == 0){
                    completeList->Append((void*) curmsg);
                // }
                
                kemper = head;
                while(kemper->next != NULL){
                    kemper = kemper->next;
                }
                kemper->next = new(std::nothrow) HistoryNode(curmsg->fromMachine, curmsg->msgID);
                msgComplete = 1; // if unwatned list is empty  msgComplete = 0, else msgCompelte = 1
                curmsg = NULL;
                while (msgComplete == 1) {
                    // grab first in the unwatned list (should be where curPack = 0)
                    // setup new curmsg
                    msgComplete = 0;
                    temp = (Mail*) unwantedMessages->Peek();
                    if (temp != NULL && temp->ackHdr.curPack == 0) {
                        ASSERT(false);
                        curmsg = new (std::nothrow) MessageNode(temp);
                        curmsg->finished = curmsg->finished + 1;
                    
                        while (temp != NULL) {
                            
                            
                            ASSERT(curmsg != NULL);
                            mn = new (std::nothrow) MailNode(temp);
                            if (temp->pktHdr.from == curmsg->fromMachine && temp->mailHdr.from == curmsg->fromBox && temp->ackHdr.messageID == curmsg->msgID && curmsg->head->Find(mn) == 0) {
                                curmsg->head->Append(mn);
                                // add one to the amount we have finished receiving
                                curmsg->finished = curmsg->finished + 1;
                            }
                            else {
                                flag = 0;
                                temper = head;
                                while(temper != NULL){
                                    if(temp->ackHdr.messageID == temper->msgID && temp->pktHdr.from == temper->machineID){
                                        flag = 1;
                                        break;

                                    }
                                    temper = temper->next;
                                }
                                if(flag == 0){
                                    tempMessages->Append((void *) temp);
                                }
                            }
                            
                            if(curmsg != NULL && curmsg->finished >= curmsg->totalSize){
                                temper = head;
                                flag = 0;
                                ASSERT(false);
                                while(temper != NULL){
                                    if (temp->ackHdr.messageID == temper->msgID && temp->pktHdr.from == temper->machineID) {//if(ackHdr.messageID == temper->msgID && pktHdr.from == temper->machineID){
                                        flag = 1;
                                        ASSERT(false);
                                        break;
                                    }
                                    temper = temper->next;
                                }
                                if(flag == 0){
                                    completeList->Append((void*) curmsg);
                                }
                                
                                kemper = head;
                                while(kemper->next != NULL){
                                    kemper = kemper->next;
                                }
                                kemper->next = new(std::nothrow) HistoryNode(curmsg->fromMachine, curmsg->msgID);
                               // curmsg = NULL;
                                msgComplete = 1;
                            }
                            temp = (Mail*) unwantedMessages->Peek();
                        }
                        if(msgComplete==1){curmsg = NULL;}
                        temp = (Mail*) tempMessages->Peek();
                        while (temp != NULL) {
                            unwantedMessages->Append((void *) temp);
                            temp = (Mail*) tempMessages->Peek();
                        }
                    }
                    else if(temp != NULL){
                        ASSERT(false);
                        temper = head;
                        flag = 0;
                        while(temper != NULL){
                            if (temp->ackHdr.messageID == temper->msgID && temp->pktHdr.from == temper->machineID) {//if(ackHdr.messageID == temper->msgID && pktHdr.from == temper->machineID){
                                flag = 1;
                                ASSERT(false);
                                break;
                            }
                            temper = temper->next;
                        }
                        if(flag == 0){
                            tempMessages->Append((void *) m);
                        }
                    }// ASSERT(false);}
                }
            }
        }
    }
}



MailBox::~MailBox()
{ 
    delete messages; 
    delete unwantedMessages;
    // delete acks;
    delete ackLock;
    delete hasAck;


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
    // // TODO ******

    messages->Append((void *)mail);	// put on the end of the list of 
				// 	// arrived messages, and wake up 
				// 	// any waiters
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
    // MailNode *mn = new(std::nothrow) MailNode(mail);
    ackList->Append((void*) mail);
    // ackLock->Acquire();
    // acks->Append(mn);
    // hasAck->Broadcast(ackLock);
    // ackLock->Release();
}
/*
int

MailBox::CheckAckMB(int msgID, int fromMach, int toMach, int fromBox, int toBox, int cPack){
    // Mail *temp;
    // MailNode *curMN;
    // curMN  = acks;

    // ASSERT(ackLock->isHeldByCurrentThread());

    // if(curMN->cur == NULL){
    //     curMN = NULL;
    // }

    // while(curMN != NULL){//This could be an issue if the size of acks changes duing th eiteratio thorugh, it shouldn't though
    //     temp = curMN->cur;
    //     if((temp->ackHdr.messageID == msgID) && (temp->mailHdr.from == toBox) &&
    //         (temp->mailHdr.to == fromBox) && 
    //         (temp->pktHdr.from == toMach) && (temp->ackHdr.curPack == cPack)){ // Removed this (temp->pktHdr.to == fromMach) &&
    //         // ackCount++;
    //         curMN->Remove(curMN);
    //         return 1;
    //     }
    //     curMN = curMN->next;
    // }
    // return 0;

}
*/


//----------------------------------------------------------------------
// PostalHelper, ReadAvail, WriteDone
// 	Dummy functions because C++ can't indirectly invoke member functions
//	The first is forked as part of the "postal worker thread; the
//	later two are called by the network interrupt handler.
//
//	"arg" -- pointer to the Post Office managing the Network
//----------------------------------------------------------------------
class postwrap{
public:
    PostOffice* p;
    Mail* m;
    Thread* t3;
};



static void PostalHelper(int arg)
{ PostOffice* po = (PostOffice *) arg; po->PostalDelivery(); }
static void ReadAvail(int arg)
{ PostOffice* po = (PostOffice *) arg; po->IncomingPacket(); }
static void WriteDone(int arg)
{ PostOffice* po = (PostOffice *) arg; po->PacketSent(); }
static void doStuff(int arg)
{ postwrap* po = (postwrap *) arg; po->p->postalDeliverySend((int) po);}
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
    ackCount = 0;
// Second, initialize the mailboxes
    netAddr = addr; 
    numBoxes = nBoxes;
    boxes = new(std::nothrow) MailBox[nBoxes];
    for(int i = 0; i < nBoxes; i++){
        boxes[i].post = (void *) this;
    }

// Third, initialize the network; tell it which interrupt handlers to call
    network = new(std::nothrow) Network(addr, reliability, ReadAvail, WriteDone, (int) this);


// Finally, create a thread whose sole job is to wait for incoming messages,
//   and put them in the right mailbox. 
    t = new(std::nothrow) Thread("postal worker");

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
    delete t;
}


void
PostOffice::postalDeliverySend(int p) {
    // postwrap* pp = (postwrap*) p;
    // Mail* mail = pp->m;
    // Thread* t = pp->t3;
    // MailHeader mailHdr = mail->mailHdr;
    // PacketHeader pktHdr = mail->pktHdr;
    // AckHeader ackHdr = mail->ackHdr;
    // char *data = mail->data;
    // ackHdr.totalSize = -1;
    // pp->p->Send(pktHdr, mailHdr, ackHdr, data);
    // t->Finish();
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
    AckHeader muckingWithWit;
    MailHeader ducking;
    PacketHeader pucking;
    char *buffer = new(std::nothrow) char[MaxPacketSize];
    Mail* ackMail;

    for (;;) {
        // first, wait for a message
        messageAvailable->P();	
        pktHdr = network->Receive(buffer);

        mailHdr = *(MailHeader *)buffer;
        ackHdr = *(AckHeader *)(buffer + sizeof(struct MailHeader));  // not sure if this is how you would do this...
        
        if (DebugIsEnabled('n')) {
    	    printf("Putting mail into mailbox: ");
    	    PrintHeader(pktHdr, mailHdr);
            fprintf(stderr, "\ncurpack %d\n", ackHdr.curPack);
        }

	// check that arriving message is legal!
    	ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
    	ASSERT(mailHdr.length <= MaxMailSize);

    	// put into mailbox
        if(ackHdr.totalSize != -1){
            // /*Need to Ack-Back*/
            muckingWithWit.totalSize = ackHdr.totalSize;
            muckingWithWit.curPack = ackHdr.curPack;
            muckingWithWit.messageID = ackHdr.messageID;

            ducking.to = mailHdr.from;
            ducking.from = mailHdr.to;
            ducking.length = mailHdr.length;

            pucking.to =pktHdr.from;
            pucking.from =pktHdr.to;
            pucking.length =pktHdr.length;

            // int pktTemp = pktHdr.to;
            // pktHdr.to = pktHdr.from;
            // //pktHdr.from = NULL;

            // int tempSize = ackHdr.totalSize; // Need to save this variable in order pull it into a buffer of the right size on the recieve
            // ackHdr.totalSize = -1;
            int mailTemp = mailHdr.to;
            // mailHdr.to = mailHdr.from;
            // mailHdr.from = mailTemp;
            // Mail *mail2 = new (std::nothrow) Mail(pktHdr, mailHdr, ackHdr, buffer + sizeof(MailHeader) + sizeof(AckHeader));
            // Thread *t2 = new(std::nothrow) Thread("ackSender");
            // postwrap* p = new(std::nothrow) postwrap();
            // p->p = this;
            // p->m = mail2;
            // p->t3 = t2;
            // t2->Fork(doStuff, (int) p);
            // // fprintf(stderr, "sent magic message %d %d\n", ackHdr.messageID, ackHdr.curPack);
            // //this->Send(pktHdr, mailHdr, ackHdr, buffer + sizeof(MailHeader) + sizeof(AckHeader));
            // // Reset the variables for to put in the mailbox
            // // mailHdr.from = mailHdr.to;
            // // mailHdr.to = mailTemp;
            // // pktHdr.from = pktHdr.to;
            // // pktHdr.to = pktTemp;
            boxes[mailTemp].Put(pktHdr, mailHdr, ackHdr, buffer + sizeof(MailHeader) + sizeof(AckHeader));
        //     tempInt = pktHdr.to;
        // pktHdr.to = pktHdr.from;
        // pktHdr.from = tempInt;
        // tempInt = mailHdr.to;
        // mailHdr.to = mailHdr.from;
        // mailHdr.from = tempInt;
        muckingWithWit.totalSize = -1;
        // data = m->data;
        // set up ackMail
        ackMail = new(std::nothrow) Mail(pucking, ducking, muckingWithWit, buffer + sizeof(MailHeader) + sizeof(AckHeader));
        boxes[mailHdr.to].retAck->Append((void*)ackMail);
            // //This should be done in a separate thread....

            // /*Signal the appropriate condition variable
            //     **REMEMBER YOU HAVE TO GRAB THE APPROPRIATE LOCK THEN YOU CAN USE THE COND VAR
            // */
            // //boxes[box].hasAck->Signal(boxes[box].ackLock);

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
    pktHdr.from = netAddr;
    pktHdr.length = mailHdr.length + sizeof(MailHeader) + sizeof(AckHeader);
    if (DebugIsEnabled('n')) {
	printf("Post send: ");
	PrintHeader(pktHdr, mailHdr);
    }
    ASSERT(mailHdr.length <= MaxMailSize);
    ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
    
    // fill in pktHdr, for the Network layer

    if (DebugIsEnabled('n')) {
    printf("Packet Length: %d", pktHdr.length);
    
    }
    // concatenate MailHeader and data
    bcopy((char *) &mailHdr, buffer, sizeof(MailHeader));
    bcopy((char *) &ackHdr, buffer + sizeof(MailHeader), sizeof(AckHeader));
    bcopy(data, buffer + sizeof(MailHeader) + sizeof(AckHeader), mailHdr.length);
    sendLock->Acquire();   		// only one message can be sent
			// to the network at any one time
    //fprintf(stderr, "here %s\n", data);

    network->Send(pktHdr, buffer);
    messageSent->P();   
    		// wait for interrupt to tell us
					// ok to send the next message
    sendLock->Release();

    delete [] buffer;			// we've sent the message, so
					// we can delete our buffer
    // ackLockAcquire(mailHdr.from);
    // hasAckSignal(mailHdr.from);
    // ackLockRelease(mailHdr.from);
}


void PostOffice::SendThings(Mail *mail, int box){
    boxes[box].sendList->Append((void *)mail);
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
MessageNode* PostOffice::GrabMessage(int box){
    return (MessageNode*) boxes[box].completeList->Remove();
}


void
PostOffice::Receive(int box, PacketHeader *pktHdr, 
				MailHeader *mailHdr, AckHeader *ackHdr, char* data)
{
    ASSERT((box >= 0) && (box < numBoxes));

    // boxes[box].Get(pktHdr, mailHdr, ackHdr, data);


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
    Mail *mail = (Mail *) boxes[box].unwantedMessages->Peek();
    while (mail != NULL) {
        fprintf(stderr, "Putting mail back!!!\n");
        boxes[box].Put(mail->pktHdr, mail->mailHdr, mail->ackHdr, mail->data);
        mail = (Mail *) boxes[box].unwantedMessages->Peek();
    }
}
/*
int
PostOffice::CheckAckPO(int box, int msgID, int fromMach, int toMach, int fromBox, int toBox, int cPack){
    return boxes[box].CheckAckMB(msgID, fromMach, toMach, fromBox, toBox, cPack);
}
*/
void
PostOffice::hasAckWait(int box){
    boxes[box].hasAck->Wait(boxes[box].ackLock);
}

void 
PostOffice::hasAckSignal(int box){
    boxes[box].hasAck->Broadcast(boxes[box].ackLock);
}

void
PostOffice::ackLockAcquire(int box){
    boxes[box].ackLock->Acquire();
}
void
PostOffice::ackLockRelease(int box){
    boxes[box].ackLock->Release();
}

void PostOffice::KaputTime(){
    PacketHeader pktHdr;
    AckHeader ackHdr;
    MailHeader mailHdr;
    Mail * mail;
    ackHdr.totalSize = -1;
    ackHdr.curPack = -1;
    ackHdr.messageID = -1;

    mailHdr.to = -1;
    mailHdr.from = -1;
    mailHdr.length = 1;

    pktHdr.to = -1;
    pktHdr.from = -1;
    pktHdr.length = -1;

    char data;
    for(int i = 0; i < numBoxes; i++){
            mail = new(std::nothrow) Mail(pktHdr, mailHdr, ackHdr, &data); 
        mail->mailHdr.length = -1;
        boxes[i].ackList->Append((void *) mail);
    }

}
