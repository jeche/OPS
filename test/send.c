/* Xkid.c
 *
 * Part of vmtorture suite: output a character 100 times
 * to ConsoleOutput. Message, length, machine, location
 */

#include "syscall.h"

int
main(int argc, char **argv)
{
  int i;
  char args[126];
  int mailbox = GetMailbox();
  for (i = 0; i < 900; i++) {
    Recv(args, 10, mailbox);
    printd(i, ConsoleOutput);
    Write("\n", 1, ConsoleOutput);
    /*Send("Got it!", 7, mailbox, 1, 0);*/
  }
  Write("\nAll done********************************************************\n", 66 ,ConsoleOutput);
  Exit(0);
  /* not reached */
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