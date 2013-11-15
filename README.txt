README.txt
Aslyn Blohm, Andrew Jones, Jessica Chen



system.h
--------
In system.h changes made included declaring two classes, FamilyNode and SynchConsole, as well as several global variables used later.  The SynchConsole class creates a global console for the system to write to.  It basically serves as a wrapper class for the already defined console.  There is a lock around methods that the kernel class so that no more than one process can access the SynchConsole to read to it or write from it at once.  The FamilyNode class is basically a node class for a linked list that will contain all family relations in the system.  It has two integers, one for the parent pid and one for the child pid.  It also has an integer to store an exit value.  It lastly has a pointer to another FamilyNode which will serve as a link to other nodes that might need to be accessed.  Finally it has a semaphore, death, which is used per relation for children to notify parents that they have exited, and force the parents to wait if the children have not exited.

Other global variables that were declared include a global pid to be bumped as new processes are forked.  Currently if anyone tries to allocate more than INT_MAX processes the system will have undefined behavior (so please do not try to do this).  There is also a bitmap that is declared to keep track of which pages are free, and which are currently allocated to a process.  There is also a global timer declared which results in random time slicing.  Finally there is a semaphore called forking which is used to ensure singluar access to most of the global objects in the system.

system.cc
---------
Intializes the pid to 0.  Intializes the forking semaphore for accessing gloabl values in the system with a value of 1.  Initializes the root node for the family tree (which is a global value) with an initial node that has the same parent as it does child.  Initializes the bitmap with the number of pages the system has.

MAJOR DESIGN DECISIONS:  We made the "FamilyNode" essentially a linked list of pids with semaphores stored in each node for parents and children to communicate with each other.  The linked list works because each pid is unique so each node should have a unique relation of parent and child pids.  This way no nodes need ever be deleted for multiprogramming to correctly occur.  Nodes are only ever added to the end.

The SynchConsole is essentially a wrapper for Console with a lock around the methods of PutChar and GetChar, and a Wait that forces processes to wait until the SynchConsole is not in use.  After a read or write is finished it Broadcasts to a condition to allow waiting processes to continue.  This was done for testing purposes before we added a semaphore to atomize read and write syscalls.  It was never removed due to sheer laziness on the part of Jessica.



addrspace.h
-----------
In addrspace.h a class for protecting each file in the file descriptor is declared.  The class is referred to as FileShield.  Each FileShield has a reference count, a pointer to a file, and an integer to check if ConsoleInput or ConsoleOutput have been dup-ed into that particular spot.  Each FileShield object has a method CopyFile() which is called everytime a new reference to that file is created (mostly for use in Fork).  Each FileShield also has a CloseFile() which decrements the reference count and returns the new reference count to the caller.  In the AddrSpace declaration 4 new variables related to each space are declared.  An integer to store a pid, a pointer to pointers for FileShields, referred to as fileDescriptors.  There is also an integer, enoughSpace which is used to determine if the AddrSpace has been created correctly.

addrspace.cc
------------
In the constructor the changes made include removing all ASSERTs and instead when they fail those sanity checks setting the value of enoughSpace to 0 (It is initially 1 because it assumes that the address space will allocate correctly).  It then interates through in a for loop in an attempt to find pages for the address space.  If it is ever unable to find a page it immediately exits the loop and sets enoughSpace to 0 denoting that the allocation of the address space failed.  For every page found the value for the virtual page is set to the physical page that has been found to be free from the bitMap.  After trying to find all the pages the fileDescriptors array is initialized with a limit of 16 files, including ConsoleInput and ConsoleOutput.  After that assuming all the other steps in the constructor have been successful it writes the code and global information to the executable in a byte by byte fashion using the virtual addresses and translation specific to that address space.  It then also initializes the pid.

The methods from translate.cc were copied for ReadMem(), WriteMem(), and Translate() verbatim into the AddrSpace object to allow for their usage when trying to determine the correct physical page from virtual pages.  This was done out of sheer laziness on the part of Jessica.

Clean():
An unimportant remnant of days where we did things in an ugly fashion and where everything was stored in the AddrSpace.  This is not used anymore, but has not yet been deleted because we don't want to break something. (Not that it actualy should be capable of doing so, but we seem to be capable of breaking things in strange ways sometimes so we are leaving it alone for now).

newSpace():
Added within AddrSpace to allow for the forking of children.  Initializes a new AddrSpace with the same values that the AddrSpace that called it holds.  A new page table is allocated, but in the new page table the physical pages that the virtual pages point to are different than the original's.  It then also copies over the FileShield object into a new fileDescriptors array.  As it copies pointers to the actual FileShield objects into the new array it increments each reference count by calling CopyFile().  newSpace then returns a new AddrSpace with the newly initialized values. Design decision:  We created newSpace so that technically every file would only ever have access to its own pageTable.

The deconstructor of AddrSpace was also modified to notify the bitMap to release pages held in the pageTable, delete the fileDescriptors array while closing any files it still has open, and then finally delete the pageTable.

MAJOR DESIGN DECISIONS:  The FileShield object is basically a wrapper class with a reference count.  It ensures the file doesn't get deleted.  In the array we just set the pointer to a specific object to NULL unless if the reference count was decremented to 0, in which case we just delete the file after calling CloseFile in exception.cc.  This was an easy way for us to ensure children recieved their parents files, and we could think of no other way to do this.



bitmap.h
--------
Intialized a Semaphore monitor with the value of 1.

bitmap.cc
---------
All the changes that were made to bitMap.cc were simply to ensure only one process can access it at a time.  Semaphores were placed around all accessors forcing each process attempting to access to P() on monitor and then V() when it exits.

MAJOR DESIGN DECISIONS:  We made bitmap a monitor so we wouldn't have to worry as much about concurrency when allocating an AddrSpace.



exception.cc
------------
For all syscalls that necessitate reading as an argument, we read the string out of the appropriate register and place it in a buffer called stringArg.  The buffer has a size of 128 characters.  We allow the user to input up to 127 characters, the 128th character is always forcibly a null character.  This way if a user gives an input string for an argument that is longer than 127 characters it is simply truncated to 127 characters.


SC_Halt
---------------------------
Calls interrupt->Halt() forcing the machine to stop.

SC_Exit
---------------------------
The exit syscall begins by iterating through the linked list of all processes/threads that have ever been created and continues until it finds a node where the pid of the child is equal to the current thread's pid.  This should work because each child forked should have a unique node.  Once it finds that it sets the exit value to the value that has been read out of register 4.  It then also does a V() on the semaphore within the node that is specified for the child.  This way if the parent is currently stuck at a P() on the semaphore from the SC_Join, the parent will know that its child has exited.  If the parent has not yet had a chance to join on the child, then if it does later it will be able to proceed past the node's semaphore because the child has already done the V().  The exiting process then deletes its address space and calls a Thread->Finish() to clean itself up.  The node related to the child is never affected.

SC_Exec
---------------------------
In the exec syscall the name is read out of register 4 and a new address space is initialized using the specified file by name.  The syscall then also handles copying out all of the old file descriptors of the previous address into the new address space.  After the new AddrSpace is created, the registers for that space are initialized, and then argument reading begins.  The stack pointer is pushed down by the length of the file name and then that file name is written above the stack pointer in the new AddrSpace and its location is recorded in the initial position of argvAddr.  This process of reading out the argument, decrementing the stack pointer, writing the argument to the stack and recording the location in argvAddr is repeated for all of the arguments.  After that, we move the stack pointer again to make space to write in all the values in argvAddr.  We write all of those values into the new AddrSpace and then remove the old one and replace it with the one we've created.  We restore the state of the AddrSpace and then write the argcount and the location of argvAddr[0] to the correct registers.  We also write the new stack pointer (with a little extra buffer) back to the appropriate register.  With all this complete, we run our thread again.

SC_Join
---------------------------
The join syscall begins by iterating through the linked list of all processes/threads that have ever been created and continues until it finds a node where the pid of the child is equal to the argument passed into register 4 and the parent pid in the node is equal to the current thread's pid.  If a node is never found that matches the description, join exits with a return value of -1.  Otherwise the parent waits for a V() on the semaphore for the found node.  Once it gets past the semaphore it retrieves the exit value from the node and returns it in register 2.

SC_Create
---------------------------
The create syscall reads a string for the name of the file out of register 4.  It passes that name to the file system and asks the file system to create the file.  If the file is successfully created the system continues on its merry way.  If it is not the system places a -1 in the return register, however since Create does not return anything this is virtually useless, and only there to denote the difference between a successful call and unsuccessful call.  The limit on the file name size is a total of 127 characters, not including the null character at the end of a string.  If the user attmepts to create a file with a name that is longer than 127 characters the system will truncate it to 127 characters with a null character at the end.

SC_Open
---------------------------
The open syscall reads a string for the name of the file out of register 4.  After reading the string out it does a sanity check to make sure the user entered a string with size greater than 0 for the name.  It then makes a call to the file system to open the file.  If the call was unsuccessful the return value is NULL.  If the returned value is null the kernel cleans up information from the syscall, increments the PCRegs, and returns a -1 in register 2.  If the returned file was valid it iterates through the current thread's file descriptor array.  If it finds an open spot (denoted by a NULL) in the array it creates a new File Shield in that spot and places the open file in that spot.  When it finds a spot the loop then terminates and forces the value of i to 18.  At the end if the value of i was 18 it will return the found descriptor, if no open spot was found it returns a -1.  In the open syscall the user is never allowed to open a file in the spots reserved for ConsoleInput and ConsoleOutput.  If a user intends to place files in those spots in the file descriptor array the Close and Dup syscalls should be used in the appropriate fashion.

SC_Read
---------------------------
The read syscall starts by grabbing the global semaphore that is used to atomize reads and writes.  It then reads out the size the user wants to read and ensures it is greater than 0.  It then checks where the call wants to read from by getting the descriptor out of register 6.  The validity of the descriptor is checked, as well as if there is currently a file at that descriptor.  Assuming the file descriptor is legitimate, if the process was instrcuted to read from a file it attempts to read that from the file into a buffer called stringArg. (Maximum size of a read is limited by the size of the buffer stringArg which is 128 characters with a forced '\0' at the 128th character).  It then writes the characters stored in stringArg to mainMemory at the specified address that is read out of register 4.  The Read call on the file returns an integer of how many characters were read.  That integer is placed into register 2 on a successful read.  If it is a read from ConsoleInput it instead calls GetChar() on the synchConsole in a for loop that occurs as many times as the size of the read.  As it reads character by character out of the synchConsole it writes each character into memory at the appropriately incremented address retrieved from register 4 and stored in whence.  It then writes the number of times it read from consoleInput to register 2.  At the end it calls a V() on the semaphore used to atmoize the reads and writes and then increments the PC Regs.  If at any point a check was failed or something went wrong a -1 is written to register 2 to show in the return that something went wrong.

SC_Write
---------------------------
The write syscall starts by grabbing the global semaphore on a P() that is used to atomize reads and writes.  It then checks to make sure the size it writes is greater than 0.  It reads the characters to be written out of memory at the address retrieved from register 4 and into the buffer stringArg.  (Again Write is limited to 128 characters maximum)  After that it checks to make sure the write is being called on a valid filed descriptor that has been read out of register 6.  If it is it calls write on the file and gives it the size to write as well as the buffer stringArg to write from.  If the write is to consoleOutput it calls a putChar() on the synchConsole as it reads characters out of memory in a for loop.  We do not allow anyone to write to consoleInput.  If at anypoint anything goes wrong a -1 is written into register 2 (even though Write technically does not return anything).  Before the write syscall exits it V()s the global semaphore and increments the PC Regs.

SC_Close
---------------------------
The close syscall starts by reading the file descriptor id to be closed out of register 4.  It then checks to make sure it is within the range of valid files.  If the descriptor is invalid it returns a -1.  If the file descriptor is a valid file descriptor it attempts to CloseFile() on the fileShield in that thread's space's fileDescriptors array.  If the CloseFile() call returns a 0 it deletes the file.  After closing the file, no matter what the return value was of the CloseFile() call it sets that spot in the fileDescriptors array to point at NULL so it is no longer pointing to the fileShield object that was there previously.  If for some reason the CloseFile() returns a nubmer that is less than 0 Close will return a -1.

SC_Fork
---------------------------
The fork syscall starts by iterating through the familyTree to find the end of it.  It bumps the global pid and then attempts to create a new AddrSpace for the child by copying the parent's using the AddrSpace function newSpace().  After attempting to create the new AddrSpace it checks to see if there was enough space to create the AddrSpace.  If there was not it writes a -1 to register 2 and decrements the global pid.  It also deletes the partially created AddrSpace.  If there was a enough space it creates a new thread, and a new FamilyNode to add to the end of the family linked list.  It gives the new thread the copy AddrSpace that was created.  The new AddrSpace is given the pid so that the thread can identify itself later.  It then calls SaveUserState() on the new thread to write all the current machine registers to the userRegisters for the child.  It then Forks the new process with the function CopyRegs.  CopyRegs restores the child's registers, restores the page table for the child, increments the PC Regs, and finally writes a 0 to register 2 before calling machine->Run() to start the execution of that thread.  The parent continues its execution by writing the bumped pid to register 2 and saving the state just in case we have problems later.  It then increments its PC Regs and resumes execution.

SC_Dup
---------------------------
The dup syscall begins by reading out the file to be duped and making sure that it is valid.  Once this is done, we iterate through the file descriptor buffer, looking for the first open spot.  If there is not one, we return a -1.  If there is one, we set the found slot equal to the slot of the file we wish to dup.  We increase the reference count for the file and note that if the slot previously pointed to ConsoleInput or ConsoleOutput, now it points to a file.  After this we return the slot the file was duped into and incease the PC registers appropriately.



syscall.h
---------
Added the Dup syscall with a #define value of 10.

test/start.s
------------
Added the Dup syscall.

shell.c
-------
The Shell can be invoked with the following command, < ../userprog/nachos -x shell >. From there you are brought to a prompt where you as the user can invoke nachos userland programs and Scripts. There are 3 utility programs, cat, cp, and echo. "cat" takes in single or multiple filenames as arguments and prints them out. "cp" will take the contents of the first given file and copy them to the second. "echo" takes in a single argument(note arguments are separated as spaces so really just a single word unless you use '-' or '_' for spaces) and prints out the input. This is primarily useful for scripts to see where you are in the execution of a script without having to modify the programs it is calling. Any program can be called with at most 14 arguments, each argument being up to 30 chars in length, scripts can not take any arguments(but any program calls within a script can take 14 arguments). Each line of input prior to the user hitting enter or a '\n' in a script may be at most 120 characters, excluding comments. A script must start with #SCRIPT being in the first line of the file with no spaces, tabs, or newlines before. Comments are defined as any text following a '#' up until a newline, but there must be a space or tab before the '#' if it is in the same line as an invocation of a program or script. The Shell handles irregular tabbing and spacing before a program or script call and between arguments. When a program finishes, the Shell will print out "Process Exited: " with the exit value returned from the program. When a script is invoked, the '--' and "Process Exited: " will not be printed out, once the script has finished the shell will print out "Script Finished" with no exit value and a "--" waiting for user input. The shell also handles for file redirection. To redirect the output of the shell to a file, after the script or program call with arguments has been input into the shell, put a '>' followed by the file you wish to output to. If the file has not been created, the shell will create a new file by the input name, and if it already exists then it will overwrite char by char. Note: if you do redirect to a file that already exists that contains a greater number of characters than you are writing, there will be remnants of the old file after the redirection.

cat.c
---------------------------------------------
The cat program takes in a list of files as arguments from the shell that exec-ed it.  It iterates through these files and tries to open them.  If open ever returns a -1, cat simply prints a failure message to the console and continues iterating through the files.  Once a file has been opened, cat reads the one character from the file and writes it to ConsoleOutput until Read returns no more characters.  Cat then closes the opened file and moves on to the next one.

cp.c
--------------------------------------
The cp program takes in two arguments aside from "cp".  One is the file to read from and the other is the file to write to.  If the number of arguments is incorrect or the file to be read from cannot be opened, cp exits with a -1 value.  Otherwise, cp tries to open the file to write to.  If it cannot, it creates the file and then opens it.  Once we have an open file, cp reads a character from the first file and writes it to the second file until the read returns no more characters.  Once cp has finished, it closes both files and exits.  It should be noted that, like shell, if the file to be written into contains more characters than are being written from the first file, then there will be remnants of the old file after the copy has completed.