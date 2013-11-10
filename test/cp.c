/* cp.c
 *
 * Parent in simple argument test.
 *
 */

#include "syscall.h"

 int
main(int argc, char **argv)
{
	OpenFileId from;
    OpenFileId to;
    char ch;

	if (argc != 3) {
		Write("Incorrect number of arguments", 29, ConsoleOutput);
		Exit(-1);
	}
	else {
		from = Open(argv[1]);
		if (from == -1) {
			Write("Could not open file", 19, ConsoleOutput);
			Exit(-1);
		}
		else {
			to = Open(argv[2]);
			if (to == -1) {
				Create(argv[2]);
				to = Open(argv[2]);
			}
			while (Read(&ch, 1, from) == 1) {
				Write(&ch, 1, to);
			}
			Close(from); /*Should we close these?*/
			Close(to);
		}
	}
	Exit(0);
}