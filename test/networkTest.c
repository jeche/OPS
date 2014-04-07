/* Xkid.c
 *
 * Part of vmtorture suite: output a character 100 times
 * to ConsoleOutput. Message, length, machine, location
 */

#include "syscall.h"

int
main(int argc, char **argv)
{
  int i, mailbox;
  char argl[128];
  /*char* args;*/
  char args[128];
  /*args = "Hello world. I would like to make magic things happen because that would be really nice.  Maybe this actually works, but that\n";*/
  for(i = 0; i < 128; i=i+4){
    args[i] = 'f';
    args[i+1] = 'u';
    args[i+2] = 'c';
    args[i+3] = 'k';
  }
  mailbox = GetMailbox();
  for (i = 0; i < 1000000000; i++) {
    Send(args, 128, mailbox, 0, 0);
    /*Recv(argl, 10, mailbox);*/
/*    Write(argl, 7, ConsoleOutput);
    Write(" ", 1, ConsoleOutput);
    printd(i, ConsoleOutput);
    Write("\n", 1, ConsoleOutput);*/

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

