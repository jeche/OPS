/* ticTacToe.c
 *
 * Kid in simple argument test.
 *
 */

 #include "syscall.h"
int
main(int argc, char **argv)
{
  int i;
  char c;
  
  i = 0;
  /* send machine ID to known server (0) */
  /* Receive. If ID received is a w then receive until ID and mailbox comes in */
  prints("Enter a machine to play with: ", ConsoleOutput);
  while(c != '\n'){

  	i = i * 10;

  	Read(&c, 1, ConsoleInput);
  	prints(c, ConsoleOutput);
  	printd(c, ConsoleOutput);
  	i = i + atoi(c, 1);
  }
  /*printd(i, ConsoleOutput);*/
  Exit(0);
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


int atoi(size, m)
int size;
char *m;
{
  int count = 0;
  int num = 0;
  int neg = 1;
  if(m[count] == '-'){
    count++;
    neg = -1;
  }
  while(count < size){
  	num = num * 10;
    if(m[count] == '0'){
    	num = num + 0;

    }
    else if(m[count] == '1'){
    	num = num + 1;
    }
    else if(m[count] == '2'){
    	num = num + 2;
    }
    else if(m[count] == '3'){
    	num = num + 3;
    }
    else if(m[count] == '4'){
    	num = num + 4;
    }
    else if(m[count] == '5'){
    	num = num + 5;
    }
    else if(m[count] == '6'){
    	num = num + 6;
    }
    else if(m[count] == '7'){
    	num = num + 7;
    }
    else if(m[count] == '8'){
    	num = num + 8;
    }
    else if(m[count] == '9'){
    	num = num + 9;
    }
    else{
    	/*prints("lefoo", ConsoleOutput);
    	prints(m[count], ConsoleOutput);*/
      return -1;
    }
    count++;
  }
  return num;
}