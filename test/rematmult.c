/* rematmult.c 
 *
 *    Reinstantiate checkpoint file ck1. Should be matmult
 *    at the 15th outer iteration. Note that the Exec, if
 *    successful, blows away this program and replaces it
 *    with the reinstantiated checkpoint.
 *
 */

#include "syscall.h"

int
main()
{

  char *args[2];   /* Dummy, just to keep Exec happy */
  int restarted;

  prints("REMATMULT begins\n\n", ConsoleOutput);
  restarted = Exec("ck1", args);  /* args should be ignored in the kernel */
  prints("\n\nREMATMULT failed\n\n", ConsoleOutput);

  Halt();
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