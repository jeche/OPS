#include "synchconsole.h"
#include <new>

static void
ReadAvail (int arg)
{
    SynchConsole* console = (SynchConsole *)arg;

    Console->CheckCharAvaliable();
}

static void
WriteDone (int arg)
{
    SynchConsole* console = (SynchConsole *)arg;

    Console->WriteDone();
}

SynchConsole::SynchConsole(char* name)
{
    //readAvail = new(std::nothrow) Semaphore("readAvail", 0);
    //writeDone
    //lock = new(std::nothrow) Lock("synch disk lock");
    console = new(std::nothrow) Console(NULL, NULL, ReadAvail, WriteDone, (int) this);
}

SynchConsole::~SynchConsole()
{
    delete console;
    //delete lock;
    //delete semaphore;
}

void
SynchConsole::SynchPutChar(char ch)
{
    //lock->Acquire();			// only one disk I/O at a time
    console->PutChar(ch);
    //semaphore->P();			// wait for interrupt
    //lock->Release();
}

char
SynchConsole::SynchGetChar()
{
    //lock->Acquire();			// only one disk I/O at a time
    char c = console->GetChar();
    //semaphore->P();			// wait for interrupt
    //lock->Release();
    return c;
}