/* test1.c
 *
 * such test. many covered cases. very thorough. wow.
 *
 */

#include "syscall.h"

int
main()
{


  int answerArr[50][4];
  int i, j, k;
  int l, m, derp;
  int addr, sub, mul, divi;


  addr = 0;
  mul = 1;
/* Really long loop, with loops inside, really tests paging */
  for( i = 0; i < 50; i++ ){
  	prints("Iteration ", ConsoleOutput);
  	printd(i, ConsoleOutput);
  	prints("\n", ConsoleOutput);
  	
  	addr = 0;
  	for( j = 0; j < 1000; j++){
  		addr++;
  		
  	}
  	
  	sub = addr;
  	answerArr[i][0]=addr;
  	prints("A", ConsoleOutput);
  	for( k = 1000; k >=0; k--){
  		sub--;
  		
  	}
  	prints("S", ConsoleOutput);
  	answerArr[i][1]=sub;
  	
  	mul = 1;
  	for( l = 0; l < 19; l++){
  		mul*=2;
  		
  	}
  	prints("M", ConsoleOutput);
  	answerArr[i][2]=mul;
  	
  	divi = mul;
  	for( m = 19; m >= 0; m--){
  		divi/=2;
  		
  	}
  	prints("D", ConsoleOutput);
  	answerArr[i][3]=divi;
  	
  }
  prints("\nValidation\n", ConsoleOutput);
  /*VALIDATION WOOO!!!*/
  for(i = 0; i < 50; i++){
  	if(answerArr[i][0]==1000){print("G");}
  	else{print("B");}
  	if(answerArr[i][1]==-1){print("G");}
  	else{print("B");}
  	if(answerArr[i][2]==524288){print("G");}
  	else{print("B");}
  	if(answerArr[i][3]=1){print("G\n");}
  	else{print("B\n");}
  }
  Exit(1);
  prints("Exit Failed\n");
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

