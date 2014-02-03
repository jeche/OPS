/* cowhammer.c
 *
 * Process Designed to Hammer on Cow Implementation
 *
 */

#include "syscall.h"

int
main(int argc, char **argv)
{
  
	int arr[10];
	char str[51];
	int pid, joinval, calc;
	int i;
	OpenFileId file;
	if(argc != 1){
		prints("Incorrect Number of Args\n", ConsoleOutput);
		Exit(-1);
	}
	pid = Fork();
	calc = 0;
	file = Open(argv[0]);
	for( i = 1; i <= 10; i ++){
		if( i > 0 )
	    	arr[i] =  arr[i-1]*(i+1);
		else{
			arr[i] = i+1;
		}
	}
	
	if( arr[9] == 7862400){
		calc++;
	}

	for( i = 0; i < 1000; i++ ){

	}
	if(pid == 0){
		for( i = 0; i < 50; i++ ){
			Write(argv[0], 2, file);
		}
		Exit(calc);
	}
	else{
		for( i = 0; i < 50; i++ ){
			Read(str+i, 1, file);
		}
		str[50]=(char*)0;
		prints(str, ConsoleOutput);
	}
	joinval = Join(pid);
	if(joinval != calc){
		Exit(-1);
	}
	Exit(1);
  
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


