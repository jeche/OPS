// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"
#include <string.h>
#include <new>

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

void
StartProcess(char *filename)
{
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }
    space = new(std::nothrow) AddrSpace(executable, 0);  

    currentThread->space = space;

    delete executable;			// close file

    

    space->InitRegisters();		// set the initial register values

    space->RestoreState();		// load page table register
    int addr[1];
    int sp = machine->ReadRegister(StackReg);
    int len = strlen(filename);

    sp -= len;
    int i;
    for(i = 0; i < len; i++){
        space->WriteMem(sp + i, sizeof(char), filename[i]);
    }
    addr[0] = sp;
    sp = sp & ~3;
    sp -= sizeof(int);
    space->WriteMem(sp, sizeof(int), addr[0]);
    


    machine->WriteRegister(4, 1);
    machine->WriteRegister(5, sp);

    machine->WriteRegister(StackReg, sp - 8);
    //fprintf(stderr, "finished with progtest\n");
    machine->Run();			// jump to the user progam
    ASSERT(false);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int) { readAvail->V(); }
static void WriteDone(int) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new(std::nothrow) Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new(std::nothrow) Semaphore("read avail", 0);
    writeDone = new(std::nothrow) Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}
