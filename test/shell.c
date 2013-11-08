#ifndef CHANGED
#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i;

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
        Write(prompt, 2, output);

        i = 0;
        
        do {
        
         Read(&buffer[i], 1, input);

        } while( buffer[i++] != '\n' );

        buffer[--i] = '\0';

        if( i > 0 ) {
         newProc = Fork();
          if (newProc == 0) Exec(buffer);
         else Join(newProc);
        }
    }
}

#else


#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    OpenFileId curFile;
    OpenFileId from, to;
    char execBuffer[60][128];
    char prompt[2], ch, buffer[60], fileBuffer[128], file2Buffer[128];
    int i, j, k, m, execCount, outputFile = 0;
    int size, l = 0;

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
        Write(prompt, 2, output);

        i = 0;
        
        do {
        
         Read(&buffer[i], 1, input);

        } while( buffer[i++] != '\n' );

        buffer[--i] = '\0';

        if( i > 0 ) {
            if (buffer[0] == 'c' && buffer[1] == 'a' && buffer[2] == 't' && buffer[3] == ' ') {
                execBuffer[0][0] = 'c';
                execBuffer[0][1] = 'a';
                execBuffer[0][2] = 't';
                execBuffer[0][3] = '\0';
                i = 4;
                j = 0;
                k = 0;
                execCount = 1;
                while (buffer[i] != '\0') {

                    if (buffer[i] == '>') {
                        outputFile = 1;
                        i++;
                        i++;
                    }

                    if (buffer[i] == ' ') {
                        execBuffer[execCount][j] = '\0';
                        execCount++;
                        j = 0;
                        i++;
                    }
                    else {
                        execBuffer[execCount][j] = buffer[i];
                        j++;
                        i++;
                    }  
                }

                execBuffer[execCount][j] =  '\0';
                
                if (!outputFile) { 
                    execCount++;
                    execBuffer[execCount][0] = '\0';
                }

                if (execBuffer[3][0] != '\0') {
                    to = Open(&execBuffer[3][0]);
                    if (to == -1) {
                        Create(&execBuffer[3][0]);
                        to = Open(&execBuffer[3][0]);
                    }
                }
                else {
                    to = output;
                }

                for (m = 1; m <= execCount - 1; m++) {
                    curFile = Open(&execBuffer[m][0]);
                    if (curFile == -1) {
                        Write("Could not open file", 19, output);
                        break;
                        /*Exit(1);*/
                    }
                    while(Read(&ch, 1, curFile) == 1) {
                        Write(&ch, 1, to);
                    }
                    Close(curFile);
                }

            }

            else {
                newProc = Fork();
                if (newProc == 0) {
                    /*Exec(buffer);*/
                }
                else {
                    Join(newProc);
                }
             }


            
        }
    }
}

#endif
