Nachos 2
========
For the Nachos 2 project the 10 syscalls as defined in syscall.h were defined as follows:

### Syscalls
#####0. [SC\_Halt](#sc_halt) 
Stop Nachos, and print out performance stats
#####1. [SC\_Exit](#sc_exit) 
This user program is done (status = 0 means exited normally).
#####2. [SC\_Exec](#sc_exec)  
Run the executable, stored in the Nachos file "name", in the context of the current address space. Should not return unless there is an error, in which case a -1 is returned.
#####3. [SC\_Join](#sc_join)  
Only return once the the user program "id" has finished.  Return the exit status.
#####4. [SC\_Create](#sc_create)  
Create a Nachos file, with "name" 
#####5. [SC\_Open](#sc_open)  
Open the Nachos file "name", and return an "OpenFileId" that can be used to read and write to the file.
#####6. [SC\_Read](#sc_read)  
Read "size" bytes from the open file into "buffer".  Return the number of bytes actually read -- if the open file isn't long enough, or if it is an I/O device, and there aren't enough characters to read, return whatever is available (for I/O devices, you should always wait until you can return at least one character).
#####7. [SC\_Write](#sc_write)  
Write "size" bytes from "buffer" to the open file.
#####8. [SC\_Close](#sc_close)  
Close the file, we're done reading and writing to it.
#####9. [SC\_Fork](#sc_fork)  
Fork creates a clone (the child) of the calling user process (the parent). The parent gets the SpaceId of the child as the return value of the Fork; the child gets a 0 return value. If there is an error that prevents the creation of the child, the parent gets a -1 return value.
#####10. [SC\_Dup](#sc_dup)  
Returns a new OpenFileId referring to the same file as denoted by the argument



<a name="sc_halt"/>
SC_Halt
---------------------------
Calls interrupt->Halt() forcing the machine to stop.

<a name="sc_exit"/>
SC_Exit
---------------------------
The exit syscall begins by iterating through the linked list of all processes/threads that have ever been created and continues until it finds a node where the pid of the child is equal to the current thread's pid.  This should work because each child forked should have a unique node.  Once it finds that it sets the exit value to the value that has been read out of register 4.  It then also does a V() on the semaphore within the node that is specified for the child.  This way if the parent is currently stuck at a P() on the semaphore from the SC_Join, the parent will know that its child has exited.  If the parent has not yet had a chance to join on the child, then if it does later it will be able to proceed past the node's semaphore because the child has already done the V().  The exiting process then deletes its address space and calls a Thread->Finish() to clean itself up.  The node related to the child is never affected.

<a name="sc_exec"/>
SC_Exec
---------------------------
In the exec syscall the name is read out of register 4 and a new address space is initialized using the specified file by name.  The syscall then also handles copying out all of the old file descriptors of the previous address into the new address space.  After the new AddrSpace is created, the registers for that space are initialized, and then argument reading begins.  The stack pointer is pushed down by the length of the file name and then that file name is written above the stack pointer in the new AddrSpace and its location is recorded in the initial position of argvAddr.  This process of reading out the argument, decrementing the stack pointer, writing the argument to the stack and recording the location in argvAddr is repeated for all of the arguments.  After that, we move the stack pointer again to make space to write in all the values in argvAddr.  We write all of those values into the new AddrSpace and then remove the old one and replace it with the one we've created.  We restore the state of the AddrSpace and then write the argcount and the location of argvAddr[0] to the correct registers.  We also write the new stack pointer (with a little extra buffer) back to the appropriate register.  With all this complete, we run our thread again.

<a name="sc_join"/>
SC_Join
---------------------------
The join syscall begins by iterating through the linked list of all processes/threads that have ever been created and continues until it finds a node where the pid of the child is equal to the argument passed into register 4 and the parent pid in the node is equal to the current thread's pid.  If a node is never found that matches the description, join exits with a return value of -1.  Otherwise the parent waits for a V() on the semaphore for the found node.  Once it gets past the semaphore it retrieves the exit value from the node and returns it in register 2.

<a name="sc_dup"/>
SC_Dup
---------------------------
The dup syscall begins by reading out the file to be duped and making sure that it is valid.  Once this is done, we iterate through the file descriptor buffer, looking for the first open spot.  If there is not one, we return a -1.  If there is one, we set the found slot equal to the slot of the file we wish to dup.  We increase the reference count for the file and note that if the slot previously pointed to ConsoleInput or ConsoleOutput, now it points to a file.  After this we return the slot the file was duped into and incease the PC registers appropriately.
