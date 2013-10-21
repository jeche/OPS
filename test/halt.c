/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"

int
main()
{
	int i, j;
	char c[3];
	Create("Wawa");
	i = Open("Wawa");
	Write("Hi!", 4, i);
	Close(i);
	i = Open("Wawa");
	Read(c, 2, i);

	Create("WawaBuffer");
	j = Open("WawaBuffer");
	Write(c, 2, j);
	Close(i);
	Close(j);
	/*Write("Hi!", 4, i);*/
    Halt();
    /* not reached */
}
