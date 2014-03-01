/* Xkid.c
 *
 * Part of vmtorture suite: output a character 100 times
 * to ConsoleOutput. Message, length, machine, location
 */

#include "syscall.h"

int
main(int argc, char **argv)
{
  char *args;
  Recv(args, 6, 1);
  Write(args, 6, ConsoleOutput);
  Halt();
  /* not reached */
}
