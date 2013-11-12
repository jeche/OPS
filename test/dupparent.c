#include "syscall.h"

int
main()
{
	int i, kid, joinval;
	OpenFileId f;
	OpenFileId l;
	OpenFileId m;
	OpenFileId n;
	OpenFileId o;
	OpenFileId p;
	char *args[2];

	f = Open("Wawa");
	Close(0);

	l = Dup(f);
	if ((kid = Fork()) == 0) {
	    /* This is the child process */  
	    args[0] = "dupkid";
	    args[1] = (char *) 0;

	    Exec("dupkid", args);
	    Halt();
  }
  joinval = Join(kid);
  prints("PARENT off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
  prints("\n", ConsoleOutput);

  m = Open("Hah");
  Close(1);
  n = Dup(m);

  o = Open("things.txt");
  Close(0);
  p = Dup(o);

  if ((kid = Fork()) == 0) {
  		args[0] = "dupkid";
	    args[1] = (char *) 0;

	    Exec("dupkid", args);
	    Halt();
  }
  joinval = Join(kid);
  prints("PARENT off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
  prints("\n", ConsoleOutput);
  Exit(joinval);
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