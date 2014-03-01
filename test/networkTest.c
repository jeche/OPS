/* Xkid.c
 *
 * Part of vmtorture suite: output a character 100 times
 * to ConsoleOutput. Message, length, machine, location
 */

#include "syscall.h"

int
main(int argc, char **argv)
{
  char* args = "Hello\n";
  Send(args, 6, 1, 1);
  Halt();
  /* not reached */
}
