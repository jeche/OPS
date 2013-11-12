#include "syscall.h"

 int
main(int argc, char **argv)
{
	char c;
    int size;

    if(argc==1){Write("\n", 1, ConsoleOutput);Exit(0);}
    if(argc>=3){Write("Too many arguments\n", 19, ConsoleOutput);Exit(-1);}
    while(argv[1][size]!='\0'){size++;}
    Write(argv[1], size, ConsoleOutput);
	Exit(0);
}