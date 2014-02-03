/* deepkid2.c
 *
 * Second-level child in the deepfork system.
 *
 */

#include "syscall.h"

int
main()
{

  int addr, j, derp;
  addr = 0;
  for( j = 0; j < 10000; j++){
  	addr++;
  	/*for(derp = 0; derp < 200000; derp += 2){derp--;}*/
  }
  Write("A", 1, ConsoleOutput);
  Exit(addr);
    /* not reached */
}

