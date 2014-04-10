/* Xkid.c
 *
 * Part of vmtorture suite: output a character 100 times
 * to ConsoleOutput. Message, length, machine, location
 */

#include "syscall.h"

int
main(int argc, char **argv)
{
  int mailbox;
  int i;
  char letters[200];

  mailbox = GetMailbox();
  for (i = 0; i < 200; i++) {
    letters[i] = 'a';
  }
  for (i = 0; i < 200; i = i + 4) {
    Send(letters, 200, mailbox, 0, 0);
    Recv(letters, 200, mailbox);
    prints(letters, ConsoleOutput);
    Send(letters, 200, mailbox, 0, 0);
    Recv(letters, 200, mailbox);
    prints(letters, ConsoleOutput);
    Send(letters, 200, mailbox, 0, 0);
    Recv(letters, 200, mailbox);
    prints(letters, ConsoleOutput);
    Send(letters, 200, mailbox, 0, 0);
    Recv(letters, 200, mailbox);
    prints(letters, ConsoleOutput);
  }
  prints("*******************************************************************ALL DONE******************************************************************", ConsoleOutput);
  Exit(0);
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
