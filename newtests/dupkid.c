/*
Kid for the dupparent.c process.  Always reads
from standard input and prints to standard output.  
Those are duped in the parent to get output and produce it in 
different places.
*/

#include "syscall.h"

int
main()
{
	char ch;
	prints("KID about to read from inherited file\n", ConsoleOutput);
	/* Reads from standard input which will be a file dupped in the parent */
	while(Read(&ch, 1, ConsoleInput) == 1) {
		prints(&ch, ConsoleOutput);
	}
	prints("\n", ConsoleOutput);
	prints("KID finished reading\n", ConsoleOutput);
	Exit(10);
}

prints(s,file)
char *s;
OpenFileId file;

{
  while (*s != '\0') {
    Write(s,1,file);
    s++;
  }
}
