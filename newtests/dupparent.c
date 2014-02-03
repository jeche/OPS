/* 
The file NewInput MUST exist in order for the test to be successful.

The first half of the test closes standard input and dups the file NewInput to it. 
The child then reads from standard input (which should be the file) and prints the
results to standard output.

The second half of the test closes standard input and dups the file NewInput to it.
It also closes standard out and dups it to a newly created file called OutputHere. 
The child then reads from standard input (which should be NewInput) and prints the 
results to standard output (which should be OutputHere).

Should produce the output 

KID about to read from inherited file
This is input for the child
KID finished reading
PARENT off join with value of 10

The file OutputHere should contain

KID about to read from inherited file
This is input for the child
KID finished reading
PARENT off Join with value of 10

*/

#include "syscall.h"

int
main()
{
	int i, kid, joinval;
	OpenFileId newInput;
	OpenFileId childInput;
  OpenFileId childOutput;
  OpenFileId dupedOutput;
	char *args[2];


  /* ****** First half of test ****** */

  /* Must start with this file already created and with something for the child to read out in it.  If the file has been written to, 
  the child's currency indicator will be at the end of the write*/
	newInput = Open("NewInput");
  /* Close standard input */
	Close(0);

	childInput = Dup(newInput); /* Now the standard input for the child process should be the open file "NewInput" */
	if ((kid = Fork()) == 0) {
	    /* This is the child process */  
	    args[0] = "dupkid";
	    args[1] = (char *) 0;

	    Exec("dupkid", args);
      /* Should not get to here */
	    Halt();
  }
  /* Join with the child */
  joinval = Join(kid);
  prints("PARENT off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
  prints("\n", ConsoleOutput);


  /* ****** Second half of test ****** */

  /* This is where the child's output will be written */
  Create("OutputHere");
  childOutput = Open("OutputHere");
  /* Close standard output */
  Close(1);
  dupedOutput = Dup(childOutput);

  /* This is the file the child will read from.  It must exist before the test is run. */
  newInput = Open("NewInput");
  /* Close standard input */
  Close(0);
  childInput = Dup(newInput);

  if ((kid = Fork()) == 0) {
    args[0] = "dupkid";
    args[1] = (char *) 0;

    Exec("dupkid", args);
    /* Should not get to here */
    Halt();
  }

  joinval = Join(kid);
  prints("PARENT off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
  prints("\n", ConsoleOutput);
  Exit(joinval);
  /* Should not get to here */
  Halt();
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

printd(n,file)
int n;
OpenFileId file;

{

  int i, pos=0, divisor=1000000000, d, zflag=1;
  char c;
  char buffer[11];
  
  if (n < 0) {
    buffer[pos++] = '-';
    n = -n;
  }
  
  if (n == 0) {
    Write("0",1,file);
    return;
  }

  for (i=0; i<10; i++) {
    d = n / divisor; n = n % divisor;
    if (d == 0) {
      if (!zflag) buffer[pos++] =  (char) (d % 10) + '0';
    } else {
      zflag = 0;
      buffer[pos++] =  (char) (d % 10) + '0';
    }
    divisor = divisor/10;
  }
  Write(buffer,pos,file);
}
