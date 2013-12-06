#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"

class SynchConsole {
  public:
    SynchConsole(char* name);    		// Initialize a synchronous console,
					// by initializing the raw console.
    ~SynchConsole();			// De-allocate the synch console data
    
    void SynchPutChar(char ch);
    					//output ch on console; delay if busy
    char SynchGetChar();
    					//return character input on console; if none avaliable, delay until it is input

  private:
    Console *console;		  		// Raw console
    //Semaphore *readAvail; 		// To synchronize requesting thread 
					// with the interrupt handler
    //Semaphore *writeDone
    //Lock *lock;		  		// Only one read/write request
					// can be sent to the disk at a time
};

#endif // SYNCHCONSOLE_H




class SynchConsole {
    private:
    Console *console;   
    public:         // Raw console
    Semaphore *readAvail;       // To synchronize requesting thread 
                    // with the interrupt handler
    Semaphore *writeDone;
    //Lock *lock;               // Only one read/write request
                    // can be sent to the disk at a time

  public:
    static void ReadAvail(int arg) { 
        SynchConsole* cons = (SynchConsole*)arg;
        cons->readAvail->V(); 
    };
    static void WriteDone(int arg) {
        SynchConsole* cons = (SynchConsole*)arg; 
        cons->writeDone->V(); 
    };

    SynchConsole(char* name){
        //readAvail = new(std::nothrow) Semaphore("readAvail", 0);
        //writeDone
        //lock = new(std::nothrow) Lock("synch disk lock");
        console = new(std::nothrow) Console(NULL, NULL, ReadAvail, WriteDone, (int) this);
        fprintf(stderr, "hi\n");
        readAvail = new(std::nothrow) Semaphore("read avail", 0);
        writeDone = new(std::nothrow) Semaphore("write done", 0);

    }; 
    ~SynchConsole(){
        delete console;
        //delete lock;
        //delete semaphore;
    };          
    
    void SynchPutChar(char ch){
        //lock->Acquire();          
        console->PutChar(ch);
        //semaphore->P();           // wait for interrupt
        //lock->Release();
    };
                        //output ch on console; delay if busy
    char SynchGetChar(){
        //lock->Acquire();          // only one disk I/O at a time
        char c = console->GetChar();
        //semaphore->P();           // wait for interrupt
        //lock->Release();
        return c;
    };
                        //return character input on console; if none avaliable, delay until it is input
    void CheckCharAvailable(){

    };

    void WriteDone(){

    };


  
};