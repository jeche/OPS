/* cat.c
 *
 * Program to execute cat the way linux does
 *
 */

#include "syscall.h"

 int
main(int argc, char **argv)
{
	int i;
	char ch;
	OpenFileId curFile; 
	for (i = 1; i < argc; i++) {
        curFile = Open(argv[i]);
        if (curFile == -1) {
            Write("Could not open file", 19, ConsoleOutput);
            break;
            /*Exit(1);*/
        }
        while(Read(&ch, 1, curFile) == 1) {
            Write(&ch, 1, ConsoleOutput);
        }
        Close(curFile);
    }

	Exit(0);
}