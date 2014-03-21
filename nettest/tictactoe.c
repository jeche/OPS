/* ticTacToe.c
 *
 * Kid in simple argument test.
 *
 */

 #include "syscall.h"
int
main(int argc, char **argv)
{
  int i;
  int mPid;
  int l;
  int count;
  int rowlength;
  char c;
  char m;
  char t;
  char buf[11];
  char recvInfo[5];
  char sends[4];
  char board[61];
  char * ans;
  i = 0;
  l = GetMailbox();
  sends[3] = '\0';
  /* send machine ID to known server (0) */
  /* Receive. If ID received is a w then receive until ID and mailbox comes in */
  prints("What is your machine id? ", ConsoleOutput);
  Read(&c, 1, ConsoleInput);
  Read(&m, 1, ConsoleInput);
  mPid = Open("serve");
  Read(recvInfo, 1, mPid);
  i = atoi(recvInfo);
  
  recvInfo[0] = '0';
  recvInfo[1] = c;
  recvInfo[2] = '0';
  ans = itoa(l, buf);
  recvInfo[3] = ans[0];
  recvInfo[4] = '\0';
  prints(recvInfo, ConsoleOutput);
  Send(recvInfo, 5, l, 0, i);
  Recv(board, 61, l);
  m = board[0];
  if(board[0] != 'o'){
    printBoard(board);
  }
  if(m == '1'){
    while(board[0] != 's'){
      prints("Row of move: ", ConsoleOutput);
      prints("\n", ConsoleOutput);
      Read(&c, 1, ConsoleInput);
      Read(&t, 1, ConsoleInput);
    
      sends[0] = m;
      sends[1] = c;

      prints("Col of move: ", ConsoleOutput);
      Read(&c, 1, ConsoleInput);
      Read(&t, 1, ConsoleInput);
      sends[2] = c;
      prints("\n", ConsoleOutput);
      Send(sends, 4, l, 0, i);
      Recv(board, 61, l);
      if(board[0] == 'e'){
        prints("Retry, invalid move\n", ConsoleOutput);
      }
    }
    if(board[0] != 'o'){
      printBoard(board);
    }
  }
  while(board[0] != 'o'){
    prints("Waiting for opponent make a move\n", ConsoleOutput);
    Recv(board, 61, l);
    if(board[0] != 'o'){
      printBoard(board);
    }
    while(board[0] != 's' && board[0] != 'o'){
      prints("Row of move: ", ConsoleOutput);
      prints("\n", ConsoleOutput);
      Read(&c, 1, ConsoleInput);
      Read(&t, 1, ConsoleInput);
    
      sends[0] = m;
      sends[1] = c;

      prints("Col of move: ", ConsoleOutput);
      Read(&c, 1, ConsoleInput);
      Read(&t, 1, ConsoleInput);
      sends[2] = c;    
      prints("\n", ConsoleOutput);
      Send(sends, 4, l, 0, i);
      Recv(board, 61, l);
      if(board[0] == 'e'){
        prints("Retry, invalid move\n", ConsoleOutput);
      }
    }    
    if(board[0] != 'o'){
      printBoard(board);
    }
  }
  if(board[1] == m){
    prints("You won!\n", ConsoleOutput);
  }
  else if(board[1] == 'D'){
    prints("Draw.\n", ConsoleOutput);
  }
  else{
    prints("Loser :P\n", ConsoleOutput);
  }

  Halt();
  /*Send(, 1, 0, i);


  /*Read(&recvInfo, 1, mPid);
  printd(atoi(recvInfo), ConsoleOutput);
  /*Send(args, 6, 1, 1);
  
  while(c != '\n'){

  	i = i * 10;

  	Read(&c, 1, ConsoleInput);
  	prints(c, ConsoleOutput);
  	printd(c, ConsoleOutput);
  	i = i + atoi(c, 1);
  }
  /*printd(i, ConsoleOutput);*/
  Exit(0);
  /* not reached */
}

/* Print a null-terminated string "s" on open file descriptor "file". */

printBoard(b)
char* b;
{
  int polo = 0;
  char bobo[60];
  for(polo; polo < 60; polo++){
    bobo[polo] = b[polo + 1];
  }
  prints(bobo, ConsoleOutput);
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


/* Print an integer "n" on open file descriptor "file". */

printd(num,file)
int num;
OpenFileId file;

{

  int i, pos=0, divisor=1000000000, d, zflag=1;
  char c;
  char buffer[11];
  
  if (num < 0) {
    buffer[pos++] = '-';
    num = -num;
  }
  
  if (num == 0) {
    Write("0",1,file);
    return;
  }

  for (i=0; i<10; i++) {
    d = num / divisor; num = num % divisor;
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

int atoi(m)
char *m;
{
  int count = 0;
  int num = 0;
  int neg = 1;
  if(m[count] == '-'){
    count++;
    neg = -1;
  }
  while(m[count] != '\0'){
    num = num * 10;
    if(m[count] == '0'){
      num = num + 0;
    }
    else if(m[count] == '1'){
      num = num + 1;
    }
    else if(m[count] == '2'){
      num = num + 2;
    }
    else if(m[count] == '3'){
      num = num + 3;
    }
    else if(m[count] == '4'){
      num = num + 4;
    }
    else if(m[count] == '5'){
      num = num + 5;
    }
    else if(m[count] == '6'){
      num = num + 6;
    }
    else if(m[count] == '7'){
      num = num + 7;
    }
    else if(m[count] == '8'){
      num = num + 8;
    }
    else if(m[count] == '9'){
      num = num + 9;
    }
    else{
      /*prints("lefoo", ConsoleOutput);
      prints(m[count], ConsoleOutput);*/
      return -1;
    }
    count++;
  }
  return num;

}

char * itoa(n, buffer)
int n;
char* buffer;
{

  int i, pos=0, divisor=1000000000, d, zflag=1;
  char c;
  
  if (n < 0) {
    buffer[pos++] = '-';
    n = -n;
  }
  
  if (n == 0) {
    return "0";
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
  return buffer;
}