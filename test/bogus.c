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



/* char *t2 = "global string"; */

int
main()
{
	char *s = "Hello, console.\n";
	/*int x;*/
	prints(s, ConsoleOutput);
	/*x = 2    */
    
   /*char * s1 = "This is a string";*/
	/*if(x == 2){
		prints(s, ConsoleOutput);
	}
	prints(s, ConsoleOutput);*/
	
    
    /* not reached */
}

prints(s,file)
char *s;
OpenFileId file;

{
  int count = 0;
  char *p;

  p = s;
  while (*p++ != '\0') count++;
  Write(s, count, file);  

}

