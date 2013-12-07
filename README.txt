----------------------------------------------------------------------------- README -- Nachos 3 
---------------------------------------------------------Aslyn Blohm, Andrew Jones, Jessica Chen
Nachos2 Retesting for Partial Credit
---
We would like to have the tests that ran bogus1 and bogusscript1 on Nachos2 re-run on our Nachos3 implementation. Both should now act appropriately.

Testing Notes
---
	--Please run all tests under stand alone invocation, ../vm/nachos -x <executable>, currently running processes from shell with the time slicing introduced in Nachos2 works most of the time, however occasionally will crash depending on the ordering of execution as well as if it is in a script or not.

	--No Glitter Points for checkpoint, it does not save any openfiles for execution, when a process is re-instantiated from a checkpoint file, all it has is ConsoleInput and ConsoleOutput, it is up to the user to re-open any files used in the program.


Virtual Memory Implementation
---
Page Fault Handler 
------
When we get a page fault, we grab the chillBrother semaphore (which is used to restrict accesses to the ramPages global data structure) and then enter the function that actually deals with a page fault.  We first check to make sure the machine page table doesn't have the page we're faulting for marked as valid.  If it is marked as valid, then we don't need to do anything and we just exit the function and release the semaphore.  However, if the page is marked as invalid then we call the findReplacement() function which returns an integer of the page in ramPages that we are going to replace.  The algorithm done in findReplacement() is discussed in more detail below.  Once we have a victim, we check to see if the the addrSpace in the ramPages entry for the victim is NULL.  If it is, the page cannot be dirty, so we don't need to deal with writing it back to the disk.  However, if the addrSpace is not NULL, then we must make sure that the page is not dirty.  If it is, we write the page to a buffer and write it back to the disk.

Now we must read in the new page.  We read the page on disk into a buffer and then write this buffer into main memory.  With this done, we set the appropriate fields in the ramPages entry.  We change the virtual page number to match the virtual page that caused the page fault, we change the pid to match the pid of the process that caused the fault, we change the valid bit in the currentThread's appropriate page table entry to true and we set the physical page of the currentThread's appropriate page table entry to make its new spot in main memory.  Now that we're done changing things in ramPages, we can set the status of the page we were messing with to be "InUse" so that it can be replaced again.  We leave the function and give up the semaphore and continue on our merry way.

The pageFaultHandler in addrSpace is very similar to the one I just described.  However, there are significant differences.  Since this pageFaultHandler will also be dealing with page faults when a process is exceed and is not yet running in its own thread, we must check things like the page table or the pid of the addrSpace calling the pageFaultHandler rather than checking these things in machine or in the current thread.


Page Replacement Algorithm
------
For our replacement algorithm, we implemented a version of fancy clock.  We begin the replacement algorithm by checking to see if any of the pages in main memory are free.  We start at the location of the commutator (a global value that is only incremented in the findReplacement methods) and sweep all the way around If they are, we select that page for replacement and exit out of the function.  If there are no free pages, we begin the clock algorithm.  We start at the commutator and go through all of the pages, checking first that the addrSpace associated with the page is not NULL (this would only occur if some of the ramPages entries were marked as Free during the deletion of an addrSpace).  If the addrSpace isn't NULL, we then check the use bit and the dirty bit of the page in ramPages.  If the page has not been used (which is to say that the use bit is set to true) and the page is not dirty, then we select that page for replacement.  If none of the pages matches that description, we begin the sweep again, this time checking for a page that has not been used but is dirty.  If we find one of those, we return that page as the one to be replaced.  Otherwise, we set the use bit to true (which would say that the page hasn't been used) and start back at the beginning of the clock part of the algorithm.

Once we've selected a page for replacement, we do one of two things depending on if the addrSpace being pointed to in the ramPages entry is NULL or not.  If it is NULL, we just mark the page as being "MarkedForReplacement" (part of an enum that dictates the different statuses a page can have) and return the page to be replaced to the pageFaultHandler.  If the addrSpace is not NULL, then we need to set the valid bit to false, set the physical page in the addrSpace's pageTable to be a -1 (just to thoroughly invalidate it), and set the status as being "MarkedForReplacement".

We also do one other interesting thing with this algorithm.  We ensure that the replacement algorithm cannot choose the page to replace to be the one the commutator started on at the beginning of the algorithm.  Without this, we were having an issue where running vmtorture would eventually get to a point where we were just continuously replacing the same page.  This was happening because on the first scan (where we check that the use bit is true and the dirty bit is false), every page aside from the one that had just been replaced either had their use bit set to false or their dirty bit set to true.  Having one page constantly being replaced was an issue for our system and we solved it by not allowing the commutator to choose the page that had just been replaced.  This forced the commutator to enter the second loop of the algorithm where we check for the use bit being true and the dirty bit being true.  In this loop, there were many pages that fit this description, so the algorithm chose more effectively.

We tested running vmtorture with both a random and the fancy clock algorithm to see the differences in the number of page faults.  The difference between random and the fancy clock is around 10,000 page faults, thus suggesting that the fancy clock algorithm is far more efficient.


CheckPoint Implementation
---

Progression when CheckPoint(char *name) is called
------
Under the usage pattern below, the progression of a call to CheckPoint is as follows.
	
	if (CheckPoint("ckptfile")) {
	/* stuff to do if being re-instantiated */
	. . .
	} else {
	/* stuff to do if taking the checkpoint */
	. . .
	}

When the syscall to CheckPoint is called it initially increments the PrevPc, PC, and NextPC registers, reads in string argument for the filename, creates the checkpoint file with the string argment as the name, and opens that file for edits. Assuming the file opens correctly, CheckPoint calls the writeBackDirty method on the currentThreads addrspace. This writes all of the processes dirty pages back to memory as when it copies the processes pages, it does it from disk, and we want the most recent versions of all the pages. From this point on it begins the process of writing the CheckPoint file. First we write a 1 to the syscall return register(register 2), so when the CheckPoint file is re-instantiated, it is as if it just returned from the Checkpoint call with a 1. Before it writes anything to the CheckPoint file it writes #CheckPoint at the top to designate it as a checkpoint file to exec. Next it writes all the registers to the file, followed by the number of pages the process requires, finished with the contents of all the processes pages on Disk. The format of the CheckPoint file is as follows, where anything inside <logicalvalue> denotes what value is written and not the literal string, <logicalvalue>, and [...] denotes skipping repetitive iteration to save space.

#Checkpoint\n
<Register(0)>\n
<Register(1)>\n
[...]
<Register(NumTotalRegs-1)>\n
<NumPages>\n
<Contents of Disk Sector associated with vPage 0><Contents of Disk Sector associated with vPage 1>[...]<Contents of Disk Sector associated vPage NumPages-1>

After it has finished writing to the checkpoint file it then puts a 0 in the syscall return register(2), breaks out of the exception.cc switch statement, and returns to where CheckPoint was called with a return value of 0.


Progression when a CheckPoint file "ckpt" is exec'd
------
When an exec with the filename to a CheckPoint file as the argument is called, it follows the same progression to exec as before, however several aspects were changed to handle this case. When it enters into Exec, it does the same initial section of reading in the Exec string argument and opening it. After it does this, it now reads in the first 12 characters and compares what it gets to the string "#Checkpoint\n", if it matches it continues with the new case for exec'ing a checkpoint file, otherwise it reopens the file, which resets the currency indicator(yes there probably is a better way to do this, but it works), and does a 'normal' exec with args. If the first 12 bytes matched "#Checkpoint\n", it then begins reading in the CheckPoint file to begin the re-instantiation. First before anything, it saves the current user state so it can be brought back if something goes wrong, using currentThread->SaveUserState(). It then reads in the register values, going byte by byte, with '\n' as the delimiter between register values, reading into a buffer then calling atoi on that buffer, and then writes the values to the correct register. Since atoi was used, mostly out of laziness and it was used in other portions of nachos, there is the weakness of if non number values are in the buffer, so any changes to the checkpoint file made by the user that introduces any whitespace or other mucking about will cause bad things to happen. It then reads in the numPages value in the same manner, and passes it and the openfile object to the new AddrSpace constructor, AddrSpace(OpenFile *chkpt, int numpages). In this constructor, it functions similarly to the default AddrSpace constructor, except instead of relying on the noff file for the number of pages needed it is supplied that from the incoming arguments, sets up ConsoleInput and ConsoleOutput(No other OpenFiles), and reads in the pages in 128 byte chunks from the checkpoint file and writes them to disk using the revPageTable set up when it grabbed the pages from the diskBitMap. Afterward it returns a new AddrSpace which matches the AddrSpace of the CheckPoint'ed process at the time of the CheckPoint. Returning back to CheckPoint in exception.cc, it checks the enoughSpace flag from the newly constructed AddrSpace, as well as the flag, flag(in the Exec), to see if any errors occurred. If any did, it restores the old user state and wries a -1 to Exec's return. Otherwise, assuming everything happened correctly, it then transferes the pid over, sets the currentThread's addrspace to the new space, calls RestoreState() on the new space to make the machine's pagetable it's own, and finally it calls machine->Run(); 



Non-Trivial Modifications to Files to Implement
---------

addrspace.h 
------------
	-Added new Constructor
		- AddrSpace(OpenFile *chkpt, int numpages);
	-Added class methods: 
		- void printAllPages();
		- bool writeBackDirty();

addrspace.cc
------------
	- AddrSpace(OpenFile *chkpt, int numpages);
		New AddrSpace constructor to be used when recreating a process from a CheckPoint file.
	- void printAllPages();
		Debug method to print the status of all a processes pages on disk.
	- bool writeBackDirty();
		Used to forcibly write all of a processes dirty pages back to disk. Used prior to writing a checkpoint file, as it is written based off of the processes pages on disk.

exception.cc
------------
	-Modifications to Exec Syscall
		Changed to be able to exec a CheckPoint file
	-CheckPoint Syscall
		Creates a CheckPoint file based off of the calling processes current state, sets return value to -1 on error, 0 if you called CheckPoint, 1 if you have been restored from a CheckPoint file.

