Nachos Networking

All changes that were made in relation to this project occurred either in userprog, filesys, or threads.

For setting up the network paging, changes were made to create a thread on the server and allocate a mailbox on the server (the client's machine number) for each client that would handle all interactions with that client.  Note that we have a bitmap keeping track of which mailboxes are in use so that if the clients try to do any sort of network communication between each other they will not use the ones previously designated for paging or migrating.  The thread on the server listens for requests from its specified client(the client is specified when it is originally forked off). It utilizes the mailHdr.length to determine what kind of request the client is making. One of the requests clients can make related to paging is reading a page (mailHdr.length == 1), the page being requested is denoted by an integer in the ackHdr (a header specifically designed for us to easily pass more information through mail) of the request. The server then accesses its synchDisk to read the specified page out into a buffer and replies to the client with a piece of mail that contains the page requested in the data. The next request possible is writing a page back, when doing so the length of the mail is denoted to be the pageSize (128) due to the only data in the mail being all 128 characters for the dirty page, the server processes the request by accessing its synchDisk and writing the page to the disk. It then replies back to the client to let the client know it has finished. Another request related to paging is that clients may also request a page be allocated to them as well as release a page from the server. The server has a bitmap which keeps track of the pages currently free. When the client sends a request for a page to either be cleared or allocated the server updates the bitmap accordingly and then replies to the client with confirmation, or the number of the allocated page. Clients always make and receive replies for requests related to remote paging locally (to the client) on mailbox 0.  

For the migration the mailbox that has the same number as the server's machine number is used to communicate with clients. The clients use their own mailbox 1 to communicate back to the server. The clients all have an additional thread that listens for the server attempting to tell it that something related to migration should happen. Another integer value located in the ackHdr denotes what kind of communication related to migration the message received is. On a migrate the server sends a message to the victim (the loser of a process) telling it to grab a user thread (migrateFlag == 0). The victim-client grabs a user thread by stealing it off the readyToRun list in the scheduler. The vicitm it selects must be a userthread, not have been previously migrated, cannot have forked any kids into existence, and cannot be the first process started up on the machine(the -x process). If there are no processes that are on the readyList that fit these criteria, then it returns a NULL which is dealt with in the migrationHandler where an error message is sent back to the server. If this is not the case, it then creates the migckpt file to write out all of the victim user thread's information to the file. After doing so we destroy the addrspace of the stolen thread and delete it. We then perform a Murder on the victim user thread, Murder is a new function that can be performed on a thread which tells it to destroy itself. The victim client then replies to the server allowing the server to notify immigrant acceptor client that it may now recreate the immigrant(migrateFlag == 1). The immigrant is reinstantiated as a new process from a checkpoint on the immigrant acceptor machine.  Note that also when a process is migrated the server instantiates a new ForeignThreadNode which tracks the pid of the processs on the original machine, the machine number of the original machine, the pid of the process on new machine, the machine number of the new machine, and the pid of the parent process on the original machine.  These nodes are later used for exitting and joining.

For network exits the server thread for each client and the migration thread on each client are stolen and hacked away at in order to allow for easy communication. When a process exits, if it was a process that has been migrated from another machine, it sends its exit value up to the server (to the mailbox/thread that listens for disk requests). The server then finds out where that process originated from using the foreignThreadNode linked list and sends a notification to the appropriate client via the migration handling thread/mailbox on that client. On a receive of an exit that client then V()'s on the appropriate semaphore denoting a child's death and places the exit value which has been passed along throughout in it's appropriate spot to allow for the parent to get what it needs when it needs it.

For ease we decided to never allow the clients to migrate threads that are currently parents nor allow for a process to migrated more than once.

For automatic load balancing, we have gerryrigged it into our current timeouthandler and our pagefaults, in the server and client respectively. From the point of view of the client, the system has a value numReadyProcs that is the number of processes on the readyList at any time. It is updated in the scheduler anytime a thread is added or removed from the readyList(ReadyToRun and FindNextToRun). Whenever a user process pagefaults, it sends whatever the current value of numReadyProcs is to the server in the migrateFlag field in the AckHeader. From this point on the server handles the rest of automatic load balancing. The server has a float array, clientLoad, with a spot for each individual client's load(Max 10 clients). As the server handles readRequests from clients, it sums the value at clientLoad[client] and the recieved value by weighting the current value and new recieved value. Currently the equation is clientLoad[client] = .8 * clientLoad[client] + .2 * currentClientLoad. For the automatic load balancing aspect, inside the current timeouthandler it iterates through the clientLoad array to see if there is an imbalance among the active clients. Currently it finds the max and min load among the clients and if the difference of the two is greater than 4, it will fork off a thread to do a migration. After the check for an imbalance, it then goes and multiplies all active clientLoad values by .8. This is done for a variety of reasons. First, the main issue with passing the load values through the page fault handling system is if there are no running user threads on a system, it will still have it's load value on the server since it is skewed toward history rather than current. This way, if a machine never pagefaults, eventually the load value will be the minimum load so it will have processes migrated to it. Another reason why this is done is if network reliability is not set to 1, because timeouts on packets sent will occur frequently, it acts as a natural deterant to the system automatically doing a migration that would prevent other machines from sending and recieving page fault message, optimizing overall system performance. Another aspect of automatic load balancing ties into the restrictions as to who can and cannot be migrated. Since it is likely that a system may become composed entirely of processes that cannot be migrated, if we send a request for a thread and the machine cannot give one up, it sends back an error case and the server immediately stops trying to migrate a process that timeout round.

Current Issues:  Note for that more robust tests like VM Torture after several migrates occasionally there is an Illegal Instruction Exception or Address Error Exception.  This is a problem that has been prevalent throughout the semester ever since we started working with page faulting.

Test Case 1 (Remote Paging, no Migration)

This is a simple test to see if remote paging works, as well as the server does not try to migrate processes(the machines are balanced enough). To run, start the server and clients up with shell, run matmult on both clients, then a halt on the clients when they finish. Take note of the statistics following the halt, they show that only the server is doing disk I/O and the clients and server have network I/O, which must be due to the system as shell, matmult, and halt have no network operations within.

	Server:

	atjones@me:/home/scratch/atjones/OSNet/test> ../network/nachos -x shell -m 0 -C "1,2"
	--clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: -1.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.180389
	clientLoad[2]: -1.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.144311
	clientLoad[2]: 0.214748
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.000000
	clientLoad[2]: 0.171799
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.000000
	clientLoad[2]: 0.053692
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.000009
	clientLoad[2]: 0.000066
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.000000
	clientLoad[2]: 0.006732
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.000000
	clientLoad[2]: 0.000256
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.000000
	clientLoad[2]: 0.021475
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.999917
	clientLoad[2]: 0.017180
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.636954
	clientLoad[2]: 0.631665
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.509564
	clientLoad[2]: 0.505332
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	halclientLoad[0]: -1.000000
	clientLoad[1]: 0.407651
	clientLoad[2]: 0.404266
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	t
	Machine halting!

	Ticks: total 1438144224, idle 1399934435, system 38206070, user 3719
	Disk I/O: reads 43642, writes 991
	Console I/O: reads 5, writes 2
	Paging: faults 43
	Network I/O: packets received 128688, sent 128688

	Cleaning up...


	Client 1 (Matmult):

	atjones@me:/home/scratch/atjones/OSNet/test> ../network/nachos -x shell -m 1 -S 0
	--matmult
	Starting matmult
	Initialization Complete
	i = 0
	i = 1
	i = 2
	i = 3
	i = 4
	i = 5
	i = 6
	i = 7
	i = 8
	i = 9
	i = 10
	i = 11
	i = 12
	i = 13
	i = 14
	i = 15
	i = 16
	i = 17
	i = 18
	i = 19
	i = 20
	i = 21
	i = 22
	i = 23
	i = 24
	i = 25
	i = 26
	i = 27
	i = 28
	i = 29
	C[29,29] = 25230

	Process Exited: 10

	--halt
	Machine halting!

	Ticks: total 1548316211, idle 1523570162, system 22360460, user 2385589
	Disk I/O: reads 0, writes 0
	Console I/O: reads 13, writes 283
	Paging: faults 21618
	Network I/O: packets received 77969, sent 77969

	Cleaning up...

	Client 2 (Matmult):

	atjones@me:/home/scratch/atjones/OSNet/test> ../network/nachos -x shell -m 2 -S 0
	--matmult
	Starting matmult
	Initialization Complete
	i = 0
	i = 1
	i = 2
	i = 3
	i = 4
	i = 5
	i = 6
	i = 7
	i = 8
	i = 9
	i = 10
	i = 11
	i = 12
	i = 13
	i = 14
	i = 15
	i = 16
	i = 17
	i = 18
	i = 19
	i = 20
	i = 21
	i = 22
	i = 23
	i = 24
	i = 25
	i = 26
	i = 27
	i = 28
	i = 29
	C[29,29] = 25230

	Process Exited: 10

	--halt
	Machine halting!

	Ticks: total 1586542897, idle 1564663418, system 19493890, user 2385589
	Disk I/O: reads 0, writes 0
	Console I/O: reads 13, writes 283
	Paging: faults 21618
	Network I/O: packets received 50719, sent 50719

	Cleaning up...

Test Case 2 (Migration Test)

	A simple test to ensure that migration will occur, and works, when an imbalence among clients is detected. To run, start up the server and client 1 with shell and client 2 with matmult(It normally would be run with shell, however the synchconsole would be stuck on a read and nothing would be printed out until the user hits enter). On client 1 start up maxfork, try to do this while matmult is still running so there is some user process interweaving. In this run, there were 3 migrations from machine 1 to machine 2, seen in the migration log of the server and print out of exit values of the migrated processes on machine 2.

	Server:

	atjones@me:/home/scratch/atjones/OSNet/test> ../network/nachos -x shell -m 0 -C "1,2"
	--clientLoad[0]: -1.000000
	clientLoad[1]: -1.000000
	clientLoad[2]: -1.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.214748
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.021329
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 5.924216
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	I want to migrate a process from machine 1 to 2
	clientLoad[0]: -1.000000
	clientLoad[1]: 8.547765
	clientLoad[2]: 0.000002
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	I want to migrate a process from machine 1 to 2
	clientLoad[0]: -1.000000
	clientLoad[1]: 7.431688
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	I want to migrate a process from machine 1 to 2
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.049380
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.439504
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 1.951603
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 1.561283
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.473732
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.778986
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.223189
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	halt
	Machine halting!

	Ticks: total 1374856224, idle 1352907425, system 21945080, user 3719
	Disk I/O: reads 23550, writes 1479
	Console I/O: reads 5, writes 2
	Paging: faults 43
	Network I/O: packets received 59483, sent 59483

	Cleaning up...

	Client 1 (Running shell and a maxfork)

	atjones@me:/home/scratch/atjones/OSNet/test> ../network/nachos -x shell -m 1 -S 0
	--maxfork
	PARENT exists
	Kid 0 created. pid=2
	KID exists
	Kid 1 created. pid=3
	KID exists
	Kid 2 created. pid=4
	KID exists
	Kid 3 created. pid=5
	KID exists
	Kid 4 created. pid=6
	KID exists
	Kid 5 created. pid=7
	KID exists
	Kid 6 created. pid=8
	KID exists
	Kid 7 created. pid=9
	KID exists
	Kid 8 created. pid=10
	KID exists
	Kid 9 created. pid=11
	KID exists
	Kid 10 created. pid=12
	KID exists
	Kid 11 created. pid=13
	KID exists
	Kid 12 created. pid=14
	KID exists
	Kid 13 created. pid=15
	KID exists
	Kid 14 created. pid=16
	PARENT about to Join kids
	KID exists
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	PARENT off Join with value of 17
	kid array:
	     kid[0]=2
	     kid[1]=3
	     kid[2]=4
	     kid[3]=5
	     kid[4]=6
	     kid[5]=7
	     kid[6]=8
	     kid[7]=9
	     kid[8]=10
	     kid[9]=11
	     kid[10]=12
	     kid[11]=13
	     kid[12]=14
	     kid[13]=15
	     kid[14]=16

	Process Exited: 10

	--halt
	Machine halting!

	Ticks: total 1043083478, idle 1008798168, system 11019400, user 23265910
	Disk I/O: reads 0, writes 0
	Console I/O: reads 13, writes 1285
	Paging: faults 461
	Network I/O: packets received 13844, sent 13844

	Cleaning up...

	Client 2 (Running matmult)

	atjones@me:/home/scratch/atjones/OSNet/test> ../network/nachos -x matmult -m 2 -S 0
	Starting matmult
	Initialization Complete
	i = 0
	i = 1
	i = 2
	i = 3
	i = 4
	i = 5
	i = 6
	i = 7
	i = 8
	i = 9
	i = 10
	i = 11
	i = 12
	i = 13
	i = 14
	i = 15
	i = 16
	i = 17
	exit value: 17
	i = 18
	i = 19
	i = 20
	i = 21
	exit value: 17
	i = 22
	i = 23
	i = 24
	exit value: 17
	i = 25
	i = 26
	i = 27
	i = 28
	i = 29
	C[29,29] = 25230
	^C
	Cleaning up...

Test Case 3 (Remote Paging and Migration with vmtorture and matmult)

	More of a non-trivial version of test 2, set up is the same, however vmtorture is run stand alone on machine 1 and not through shell. Note, this test will take a while due to the large number of threads and since 2 matmults will be run. In this run 2 of the Xkid threads were migrated over to machine 2. Occasionally this test causes illegal instruction errors or address exception errors on machine 2, we are not entirely sure of the source of the issue.
	
	Server (Shell):
	atjones@me:/home/scratch/atjones/OSNet/test> ../network/nachos -x shell -m 0 -C "1,2"
	--clientLoad[0]: -1.000000
	clientLoad[1]: -1.000000
	clientLoad[2]: -1.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 4.484591
	clientLoad[2]: -1.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 4.073094
	clientLoad[2]: 0.003691
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	I want to migrate a process from machine 1 to 2
	clientLoad[0]: -1.000000
	clientLoad[1]: 5.390948
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	I want to migrate a process from machine 1 to 2
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.294235
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.451899
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.294657
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.997253
	clientLoad[2]: 0.000083
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.965867
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.492989
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.417868
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.175240
	clientLoad[2]: 0.000317
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.420348
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.543820
	clientLoad[2]: 0.017592
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.713041
	clientLoad[2]: 0.000014
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.542956
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.345621
	clientLoad[2]: 0.000203
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.218474
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.328830
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.503653
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.464117
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.442064
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.109343
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.177093
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.445549
	clientLoad[2]: 0.014074
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.225995
	clientLoad[2]: 0.104858
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.321094
	clientLoad[2]: 0.000130
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.998765
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.540804
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.517760
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.565692
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.493962
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.490125
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.249315
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.262108
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.388362
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.961517
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.302608
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.458639
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.490159
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.270713
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.384577
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.658550
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.410933
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.495733
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.657192
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.233287
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.533239
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.533045
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.417401
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.609933
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.401420
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.567161
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.393504
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.612068
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.552426
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.240907
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.266152
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.751360
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 3.340376
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.287312
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000006
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.400396
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.001889
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000000
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 2.000083
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 1.916128
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 1.532902
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 1.226322
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.981058
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.784846
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.627877
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.502301
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.401841
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	clientLoad[0]: -1.000000
	clientLoad[1]: 0.321473
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	halclientLoad[0]: -1.000000
	clientLoad[1]: 0.257178
	clientLoad[2]: 0.000000
	clientLoad[3]: -1.000000
	clientLoad[4]: -1.000000
	clientLoad[5]: -1.000000
	clientLoad[6]: -1.000000
	clientLoad[7]: -1.000000
	clientLoad[8]: -1.000000
	clientLoad[9]: -1.000000
	No Migration
	t
	Machine halting!

	Ticks: total 8728282724, idle 8672311525, system 55967480, user 3719
	Disk I/O: reads 60871, writes 14155
	Console I/O: reads 5, writes 2
	Paging: faults 43
	Network I/O: packets received 194631, sent 194631

	Cleaning up...

	Client 1 (vmtorture):

	atjones@me:/home/scratch/atjones/OSNet/test> ../network/nachos -x vmtorture -m 1 -S 0
	VMTORTURE beginning
	QMATMULT
	QSORT
	Console kids starting
	AAAAAAAAAAAAAAAABABABABABABABABABABABBBABBABABABABBABABABABAABABBABABABABABABABAABABABBBAAABABABABABBABABABBAABABACBACBACBACABACBACBACACABCBBABCACABCACACABCBCBCBCBCBCBCBCBCBCBCBCBCBCBCBCBCBCBBCBBCBCBCBBBBCBBBCBBCBBCCBCCCBCBCCCCCDCDCDCCDCDCDCCDCDCDCDCDCDCDDCDCDCDCDCEDCEDEDCDECDECDECECEDCCEDCDCCEDEECECECDEDCDECCDCEDCECECECECECECECECECECCECEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE

	Kids done

	Awaiting qmatmult
	qmatmult Exit value is 25230
	Awaiting qsort
	qsort Exit value is 1

	VMTORTURE terminating normally
	Machine halting!

	Ticks: total 10619798544, idle 10538597606, system 43286090, user 37914848
	Disk I/O: reads 0, writes 0
	Console I/O: reads 0, writes 597
	Paging: faults 38406
	Network I/O: packets received 145362, sent 145362

	Cleaning up...

	Client 2 (matmult):

	atjones@me:/home/scratch/atjones/OSNet/test> ../network/nachos -x matmult -m 2 -S 0
	Starting matmult
	Initialization Complete
	i = 0
	i = 1
	AAAAAAAAAAAAAAAAAAAAAAexit value: 0 
	i = 2
	DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDi = 3
	DDDDDDDDDDDDDDDDDexit value: 0 
	i = 4
	i = 5
	i = 6
	i = 7
	i = 8
	i = 9
	i = 10
	i = 11
	i = 12
	i = 13
	i = 14
	i = 15
	i = 16
	i = 17
	i = 18
	i = 19
	i = 20
	i = 21
	i = 22
	i = 23
	i = 24
	i = 25
	i = 26
	i = 27
	i = 28
	i = 29
	C[29,29] = 25230
	^C
	Cleaning up...
