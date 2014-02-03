/* test1.c
 *
 * such test. many covered cases. very thorough. wow.
 *
 */

#include "syscall.h"

int
main()
{
	int pidArr[5];
	int joinArr[5];
	char *args[2];


	int i;
  	
  	args[1] = (char *)0;


	if((pidArr[0] = Fork()) == 0){
		args[0] = "f1";
  		Exec("cowhammer", args);
      	print("ERROR: exec failed\n");
     	Halt();
  	}
  	if((pidArr[1] = Fork()) == 0){
  		args[0] = "f2";
  		Exec("cowhammer", args);
      	print("ERROR: exec failed\n");
     	Halt();
  	}
  	if((pidArr[2] = Fork()) == 0){
  		args[0] = "f3";
  		Exec("cowhammer", args);
      	print("ERROR: exec failed\n");
     	Halt();
  	}
  	if((pidArr[3] = Fork()) == 0){
  		args[0] = "f4";
  		Exec("cowhammer", args);
      	print("ERROR: exec failed\n");
     	Halt();
  	}
  	if((pidArr[4] = Fork()) == 0){
  		args[0] = "f5";
  		Exec("cowhammer", args);
      	print("ERROR: exec failed\n");
     	Halt();
  	}
  	for( i = 0; i < 5; i++ ){
  		joinArr[i] = Join(pidArr[i]);
  	}
  	prints("\nFinished Joining\nExit Value Validation\n", ConsoleOutput);
  	for( i = 0; i < 5; i++ ){
  		if(joinArr[i] == 1){prints("G", ConsoleOutput);}
  		else{prints("B", ConsoleOutput);}
  	}
  	/*Exit Value Validation*/
  	Exit(1);
  	prints("Exit Failed", ConsoleOutput);
  	Halt();
    /* not reached */
}

/* Print a null-terminated string "s" on open file descriptor "file". */

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


/* Print an integer "n" on open file descriptor "file". */

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

/* Print a null-terminated string "s" on ConsoleOutput. */

print(s)
char *s;

{
  prints(s, ConsoleOutput);
}


