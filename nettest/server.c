/* server.c
 *
 */

#include "syscall.h"

int
main()
{
  char turnChar;
  char c;
  char board[10];
  char buf[11];
  char recvInfo[5];
  char split1[3];
  char split2[3];
  int mPid;
  int wPid;
  int wMbox;
  int mMbox;
  int port;

  int p1;
  int p2;
  int mp1;
  int mp2;
  int fId;
  int pid;

  int turnM;
  int turnB;
  int turnC;
  int turns;

  int l;
  int count;
  int  i;
  int rowlength;

  int row;
  int col;

  

  turnC = 0;

  count = 0;
  i = 1;
  rowlength = 12;
  
  for( i = 0 ; i < 10; i++){
    board[i] = ' ';
  }
  board[9] = 's';
  i = 1;

  mMbox = GetMailbox();

  /* Open a specific mailBox here */
  mPid = 0;
  Create("serve");
  fId = Open("serve");
  printd(mMbox, fId);
  Close(fId);

  Recv(recvInfo, 5, mMbox);
  split1[0] = recvInfo[0];
  split1[1] = recvInfo[1];
  split1[2] = '\0';
  split2[0] = recvInfo[2];
  split2[1] = recvInfo[3];
  split2[2] = '\0';
  p1 = atoi2(split1);
  mp1 = atoi2(split2);
  board[9] = '1';
  Send(board, 10, l, p1, mp1);
  Recv(recvInfo, 5, mMbox);
  split1[0] = recvInfo[0];
  split1[1] = recvInfo[1];
  split1[2] = '\0';
  split2[0] = recvInfo[2];
  split2[1] = recvInfo[3];
  split2[2] = '\0';
  p2 = atoi2(split1);
  mp2 = atoi2(split2);
  prints("\n", ConsoleOutput);
  printd(mp2,ConsoleOutput);
  prints("\n", ConsoleOutput);
  turnM = p1;
  turnB = mp1;
  /*board[9] = '1';
  Send(board, 10, l, p1, mp1);
  prints("Sent\n", ConsoleOutput);*/
  board[9] = '2';
  Send(board, 10, l, p2, mp2);
  prints("Sent\n", ConsoleOutput);
  board[9] = 's';
  turnChar = 'X';
  turns = 0;
  /* Game logic code */
  while(board[9] == 's'){
    Recv(board, 10, mMbox);
    turns += 1;
    if(winningMove('X', board) == 1){
      prints("winning!", ConsoleOutput);
      board[9] = 'X';
      Send(board, 10, mMbox, p1, mp1);
      Recv(board, 10, mMbox);
    } else if (turns >= 9){
      board[9] = 'd';
      Send(board, 10, mMbox, p1, mp1);
      Recv(board, 10, mMbox);
    }
    /* Send information back on how it went */
    
    Send(board, 10, mMbox, p2, mp2);
    Recv(board, 10, mMbox);
    turns += 1;
    if(winningMove('O', board) == 1){
      board[9] = 'O';
      Send(board, 10, mMbox, p2, mp2);
      Recv(board, 10, mMbox);
    } else if (turns >= 9){
      board[9] = 'd';
      Send(board, 10, mMbox, p1, mp1);
      Recv(board, 10, mMbox);
    }
    Send(board, 10, mMbox, p1, mp1);
  }
  
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


/* Print an integer "n" on open file descriptor "file". */
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

/* converts integer to char */
char * itoa2(n, buffer)
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

/* gets the appropriate index on the board */
int getIndex(op, lop)
int op;
int lop;
{
  return 1+ op*24 +lop * 4;
}

/* converts char* to int */
int atoi2(m)
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

/* checks to see if someone has won, return 1 if so, 0 if no one has won */
int winningMove(player, board)
char player;
char* board;
{
  prints(board, ConsoleOutput);
  Write(&player,1, ConsoleOutput);
  Write(board, 3, ConsoleOutput);
  if(board[0] == board[1] && board[2] == board[1] && board[2] == player){
    prints("top", ConsoleOutput);
    /*check top row */
    return 1;
  }
  if(board[0] == board[3] && board[6] == board[3] && board[6] == player){
    /*check first column */
    return 1;
  }
  if(board[0] == board[4] && board[8] == board[4] && board[8] == player){
    /* check from top left corner to bottom right corner diagonal */
    return 1;
  }
  if(board[1] == board[4] && board[7] == board[4] && board[7] == player){
    /* check middle column */
    return 1;
  }
  if(board[2] == board[5] && board[8] == board[5] && board[8] == player){
    /* check last column */
    return 1;
  }
  if(board[2] == board[4] && board[6] == board[4] && board[6] == player){
    /*check top right corner to bottom left corner */
    return 1;
  }
  if(board[3] == board[4] && board[5] == board[4] && board[5] == player){
    /* check middle row */
    return 1;
  }
  if(board[6] == board[7] && board[8] == board[7] && board[8] == player){
    /* check bottom row */
    return 1;
  }
  return 0;

}
