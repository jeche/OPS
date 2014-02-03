/* test2.c
 *
 * Primarily Tests Cow Implementation. Syscalls as well.
 * Also heavily tests exit values, does an end validation of the exit values.
 *
 */

#include "syscall.h"

int
main()
{
  
  int answerArr[50][4];
  int pidArr[4];
  char *args[1];

  int i, j, k, l, m, derp, pid, pid2;
  int addr, sub, mul, divi;


  addr = 0;
  mul = 1;
  args[0] = (char *)0;
/* Really long loop, with loops inside, really tests paging */
  if((pid = Fork())==0){
    pid2 = Fork();
  }
  else{
    pid2 = -1;
  }

  for( i = 0; i < 50; i++ ){
  	/*prints("Iteration ", ConsoleOutput);
  	printd(i, ConsoleOutput);
  	prints("\n", ConsoleOutput);*/
  	
  	if((pidArr[0] = Fork()) == 0){
  		Exec("addproc", args);
      	print("ERROR: exec failed\n");
     	Halt();
  	}
  	if((pidArr[1] = Fork()) == 0){
  		Exec("addproc", args);
      	print("ERROR: exec failed\n");
     	Halt();
  	}
  	if((pidArr[2] = Fork()) == 0){
  		Exec("addproc", args);
      	print("ERROR: exec failed\n");
     	Halt();
  	}
  	if((pidArr[3] = Fork()) == 0){
  		Exec("addproc", args);
      	print("ERROR: exec failed\n");
     	Halt();
  	}
    /*print("JoiningThreads", ConsoleOutput);*/
  	answerArr[i][0]=Join(pidArr[0]);
  	answerArr[i][1]=Join(pidArr[1]);
  	answerArr[i][2]=Join(pidArr[2]);
  	answerArr[i][3]=Join(pidArr[3]);
  	
  	
  }
  prints("\nValidation\n", ConsoleOutput);
  /*VALIDATION WOOO!!!*/
  for(i = 0; i < 50; i++){
  	if(answerArr[i][0]==10000){print("G");}
  	else{print("B");}
  	if(answerArr[i][1]==10000){print("G");}
  	else{print("B");}
  	if(answerArr[i][2]==10000){print("G");}
  	else{print("B");}
  	if(answerArr[i][3]==10000){print("G");}
  	else{print("B");}
  }
  if(pid2 == 0){
    print("Child2 Exiting\n");
    Exit(2);
  }
  if(pid == 0){
    Join(pid2);
    print("Child1 Exiting\n");
  	Exit(1);
  	prints("Failed To Exit(Child)\n", ConsoleOutput);
  	Halt();
  }
  Join(pid);
  prints("Joined Identical Forked Proc(Exit Val == 1): ", ConsoleOutput);
  printd(pid);
  Exit(1);
  prints("Exit Failed\n", ConsoleOutput);
  Halt();
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
