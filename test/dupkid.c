#include "syscall.h"

int
main()
{
	char ch;
	prints("KID about to read from inherited file\n", ConsoleOutput);
	while(Read(&ch, 1, ConsoleInput) == 1) {
		prints(&ch, ConsoleOutput);
	}
	prints("\n", ConsoleOutput);
	Exit(10);
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