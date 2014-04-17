#include "syscall.h"

int
main()
{
	char c;

	prints("Press enter to initiate migrateTest", ConsoleOutput);
  	Read(&c, 1, ConsoleInput);
  	Migration(1, 2);
  	prints("Finished", ConsoleOutput);
  	Exit(0);
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