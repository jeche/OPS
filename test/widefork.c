/* deepfork.c
 *
 * Parent fork/exec/joins kid who fork/exec/joins kid.
 *
 */

#include "syscall.h"

int
main()
{

  SpaceId kid[16];
  int joinval;
  char *args[1];
  int i;

  args[0] = (char *)0;

  print("PARENT exists\n");
  for(i=0;i<16;i++){

    if ((kid[i]=Fork()) == 0) {
      Exec("deepfork", args);
      print("ERROR: exec failed\n");
      Halt();
    }
    if(kid[i]==-1){Halt();}
  }
  for(i=0;i<16;i++){
    print("PARENT about to Join kid\n");
    joinval = Join(kid[i]);
    print("PARENT off Join with value of ");
    printd(joinval, ConsoleOutput);
    print("\n");
  }
  
  Exit(10);
  Halt();
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