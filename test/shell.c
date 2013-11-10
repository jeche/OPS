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
    char *args[50];

    args[0]=(char *)0;

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
          if (newProc == 0) Exec(buffer, args);
         else Join(newProc);
        }
    }
}


#else
/* 
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
*/

#include "syscall.h"

int
main(int argc, char*argv[])
{

    /*Write("hi", 2, ConsoleOutput);*/
/*
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i;
    char *args[50];
*/

    SpaceId newProc;
    OpenFileId fd, out;
    OpenFileId input;
    OpenFileId output; 

    char filename[30];
    char redirFile[30];
    char execBuffer[16][30];
    char scriptBuf[8];

    char prompt[2], buffer[120];
    int i, j, script, redir;
    int exitval;
    int argcount;  
    char *args[16];
    
    input = ConsoleInput;
    output = ConsoleOutput;
    script=0;
    redir=0;
    argcount=0;
    prompt[0] = '-';
    prompt[1] = '-';


    if(argc==2 && argv[1][0]=='s'&& argv[1][1]=='m'&& argv[1][2]=='o'&& argv[1][3]=='d'&& argv[1][4]=='e'){
        script=1;
    }
    while( 1 ){
        if(!script){
            Write(prompt, 2, output);
        }
        i = 0;
        redir=0;
        script=0;
        do {
        
         Read(&buffer[i], 1, input);

        } while( buffer[i++] != '\n' );

        buffer[--i] = '\0';
        /*prints(buffer, output);
        prints("\n", output);*/

        argcount=0;
        /*Check for a # and treat as Comment as in do not proceed*/
        if(buffer[0] != '#'){
        
        /* Get the Process Name */
            i=0;
            while(buffer[i]!=' ' && buffer[i]!='\0'){
                filename[i] = buffer[i];
                i++;
            }
            filename[i] = '\0';
           

            /* Open the File and see if it is a script */
            fd = Open(filename);
            if(fd == -1){
                prints("<", output);
                prints(filename, output);
                prints(">: ", output);
                prints("Executable File or Script does not Exist\n", output);
            }
            else{
                j = Read(scriptBuf, 7, fd);
                if(j==7 && scriptBuf[0] == '#' && scriptBuf[1] == 'S' && scriptBuf[2] == 'C' && scriptBuf[3] == 'R' && scriptBuf[4] == 'I' && scriptBuf[5] == 'P' && scriptBuf[6] == 'T' ){/* Scripting Section */
                    /*   If the file is a script then we want to */
                    /* dup the file into the input and exec a    */
                    /* new shell process with a flag telling it  */
                    /* is a script                               */
                    
                    
                    newProc = Fork();
                    if(newProc == 0){
                        j=0;
                       /* if(buffer[i]!='\0'){
                                while(buffer[i]==' '){i++;}
                                if(buffer[i]=='>'){
                                    i++;
                                    while(buffer[i]==' '){i++;}
                                    while(buffer[i]!= '\0'){
                                        redirFile[j]=buffer[i];
                                        i++;j++;
                                    }
                                    redirFile[j]='\0';
                                    out = Open(redirFile);
                                    prints("I am here\n", output);
                                    if(out==-1){
                                        Create(redirFile);
                                        out = Open(redirFile);
                                    }
                                    prints("now here\n", output);
                                    Close(ConsoleOutput);
                                    Dup(out);
                                    prints("lslsdkld", 1);
                                    Close(out);

                                }
                                
                        }*/
                        Close(fd);
                        fd = Open(filename);
                        Close(ConsoleInput);
                        Dup(fd);
                        Close(fd);
                        args[0]="smode";
                        args[1]=(char*)0;
                        Exec("shell", args);
                    }
                    else if(newProc == -1){
                        prints("well fuck\n", output);
                    }
                    else{
                        prints("waiting for the script to finish", output);
                        Join(newProc);

                    }
                    prints("heyo\n", output);
                    Close(fd);
                }
                else {/* Exec With Args */
                    Close(fd);
                    newProc = Fork();
                    if (newProc == 0) {      
                        i=0;j=0;                               
                        while(buffer[i] != '\0'){/* Gets the Args and puts them in the argv */
/*                            Write("<", 1, output);
                            Write(&buffer[i], 1, output);
                            Write(">", 1, output);*/
                            if(buffer[i]==' '||buffer[i+1]=='\0'){
                                if(buffer[i+1]=='\0'){execBuffer[argcount][j] = buffer[i];j++;}
                                execBuffer[argcount][j] = '\0';
                                while(buffer[i+1]==' '){i++;}/* Nasty Stuff, strips off extra spaces and increments i */
                                /*if(buffer[i+1]=='>'){i++;redir=1;while(buffer[i+1]==' '){i++;}i++;}*/
                                argcount++;
                                if(argcount>15){Write("\nToo Many Arguments\n\n", 21, output ); Exit(-1);}
                                j=0;
                                if(redir){break;}
                            }
                            else{
                                execBuffer[argcount][j] = buffer[i];
                                j++;
                            }
                            i++;
                        }
                        /*if(redir){
                            j=0;
                            while(buffer[i]!= '\0'){
                                redirFile[j]=buffer[i];
                                i++;j++;
                            }
                            redirFile[j]='\0';
                            prints(redirFile, output);
                            out = Open(redirFile);
                            prints("I am here\n", output);
                            if(out==-1){
                                Create(redirFile);out = Open(redirFile);
                            }
                            prints("now here\n", output);
                            Close(ConsoleOutput);
                            Dup(out);
                            prints("lslsdkld", 1);
                            Close(out);
                        }
                        redir=0;
                        */
                        for(i=0;i<=argcount;i++){/*Put the args into the *char[] to pass into exec*/
                            args[i]=execBuffer[i];
                        }
                        args[argcount]=(char *)0;/*Put a \0 in the last arg spot*/
                        prints("I say heyhey", 1);
                        Exec(args[0], &args[1]);
                        Halt();
                        
                        
                    }
                     else if(newProc == -1){
                        prints("well fuck\n", output);
                    }
                    else {
                        prints("parent waiting\n", output);
                        exitval = Join(newProc);
                        Write("\nProcess Exited: ", 17, output);
                        printd(exitval, output);
                        Write("\n\n", 2, output);
                        if(script){Exit(0);}

                    }
                } 
            }            
        }
    }
}
printd(n,file)
int n;
OpenFileId file;

{

  int i, pos=0, divisor=1000000000, d, zflag=1;
  char c;
  char buffer[11];
  
  if (n < 0) {
    buffer[pos++] = '-';
    n = -n;
  }
  
  if (n == 0) {
    Write("0",1,file);
    return;
  }

  for (i=0; i<10; i++) {
    d = n / divisor; n = n % divisor;
    if (d == 0) {
      if (!zflag) buffer[pos++] =  (char) (d % 10) + '0';
    } else {
      zflag = 0;
      buffer[pos++] =  (char) (d % 10) + '0';
    }
    divisor = divisor/10;
  }
  Write(buffer,pos,file);
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

#endif
/*Cat Stuff*/
/*        if( i > 0 ) {
            Write("h2", 2, output);
            if (buffer[0] == 'c' && buffer[1] == 'a' && buffer[2] == 't' && buffer[3] == ' ') {
                Write("h5", 2, output);
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
                    }
                    while(Read(&ch, 1, curFile) == 1) {
                        Write(&ch, 1, to);
                    }
                    Close(curFile);
                }
            }
*/