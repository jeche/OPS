/* server.c
 *
 */

#include "syscall.h"

int
main()
{
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
  char turnChar;
  char board[10];
  char buf[11];
  char recvInfo[5];
  char split1[3];
  char split2[3];
  char c;

  turnC = 0;

  count = 0;
  i = 1;
  rowlength = 12;
  
  for( i = 0 ; i < 10; i++){
    board[i] = ' ';
  }
  board[9] = 's';
  i = 1;
  /* Generates a blank tic tac toe board for sending back and forth */
  /*while(count < 5){
    count = count + 1;
    if(count % 2 == 1){
      for(i; i < count * rowlength; i++){
        board[i] = ' ';
        i = i + 1;
        board[i] = ' ';
        i = i + 1;
        board[i] = ' ';
        i = i + 1;
        board[i] = '|';
      }
    }
    else{
      for(i; i < count * rowlength; i++){
        board[i] = '-';
      }
    }
    board[i - 1] = '\n';
  }*/

  mMbox = GetMailbox();
  l = mMbox;

  /* Open a specific mailBox here */
  mPid = 0;
  Create("serve");
  fId = Open("serve");
  printd(mMbox, fId);
  Close(fId);
  printd(mMbox, ConsoleOutput);
  prints("\n", ConsoleOutput);
  prints(itoa(mMbox, buf), ConsoleOutput);

  Recv(recvInfo, 5, mMbox);
  prints(recvInfo, ConsoleOutput);
  split1[0] = recvInfo[0];
  split1[1] = recvInfo[1];
  split1[2] = '\0';
  split2[0] = recvInfo[2];
  split2[1] = recvInfo[3];
  split2[2] = '\0';
  p1 = atoi(split1);
  mp1 = atoi(split2);
  printd(p1,ConsoleOutput);
  prints("\n", ConsoleOutput);
  printd(mp1,ConsoleOutput);
  prints("\n", ConsoleOutput);
  Recv(recvInfo, 5, mMbox);
  prints(recvInfo, ConsoleOutput);
  split1[0] = recvInfo[0];
  split1[1] = recvInfo[1];
  split1[2] = '\0';
  split2[0] = recvInfo[2];
  split2[1] = recvInfo[3];
  split2[2] = '\0';
  p2 = atoi(split1);
  mp2 = atoi(split2);
  printd(p2,ConsoleOutput);
  prints("\n", ConsoleOutput);
  printd(mp2,ConsoleOutput);
  prints("\n", ConsoleOutput);
  turnM = p1;
  turnB = mp1;
  board[9] = '1';
  Send(board, 10, l, p1, mp1);
  board[9] = '2';
  Send(board, 10, l, p2, mp2);
  board[9] = 's';
  turnChar = 'X';
  turns = 0;
  /* Game logic code */
  while(board[9] == 's'){
    /* Receive move from tictactoe process */
    Recv(recvInfo, 4, mMbox);
    c = recvInfo[1];
    row = atoi(&c);
    c = recvInfo[2];
    col = atoi(&c);
    turns = turns + 1;
    while(checkValidMove(row, col, turnChar, board) != 1){
      /* keep attempting to recv until you get the correct move */
      board[9] = 'e';
      Send(board, 10, l, turnM, turnB);
      Recv(recvInfo, 4, mMbox);
      c = recvInfo[1];
      row = atoi(&c);
      c = recvInfo[2];
      col = atoi(&c);
    }
    board[9] = 's';
    board[row * 3 + col] = turnChar;
    if(winningMove(row, col, turnChar, board) == 1){
      /* if this is the winning move change the board to reflect it */
      board[9] = 'o';
    } else if (turns >= 9){
      /* draw case */
      board[9] = 'd';
    }
    /* Send information back on how it went */
    if(board[9] == 'o'){
      board[9] = 'w';
    }
    Send(board, 10, l, turnM, turnB);
    /* flip the turn here */
    if(turnM == p1){
      turnM = p2;
      turnB = mp2;
      turnChar = 'O';
    } else{
      turnM = p1;
      turnB = mp1;
      turnChar = 'X';
    }
    if(board[9] == 'w'){
      /* send l so that machine knows it lost */
      board[9] = 'l';
    }
    Send(board, 10, l, turnM, turnB);
  }
  
  }

  /* Open a specific mailbox here */
  

  /*save for later*/
  /*mMbox = GetMailbox();
  printd(mMbox, fId);
  Recv(&recvInfo, 2, mMbox);
  prints(recvInfo, ConsoleOutput);
  Close(fId);
  while(1){
    Recv(&recvInfo, 3, mMbox);
  }




  /*while(1){
    /* Receive connections of machine id and mailbox number*/
    /* Mark a machine as waiting, if one is not waiting, if not waiting send back machine to connect to with mailbox
    char *args;*/
    /* Send back another machine who wants to play 
  Send(args, 6, 1, 1);
  Recv(args, 6, 1);*/
  /*}
  */

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

/* gets the appropriate index on the board */
int getIndex(op, lop)
int op;
int lop;
{
  return 1+ op*24 +lop * 4;
}

/* converts char* to int */
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

/* valid move returns 1, invalid returns 0 */
int checkValidMove(row, col, board)
int row;
int col;
char* board;
{
  if(row < 0 || col < 0 || row > 2 || col > 2){
    return 0;
  }
  if(board[row * 3 + col] != ' '){
    return 0;
  }
  return 1;
}

/* checks to see if someone has won, return 1 if so, 0 if no one has won */
int winningMove(row, col, player, board)
int row;
int col;
char player;
char* board;
{
  if(board[0] == board [1] == board[2] == player){
    /*check top row */
    return 1;
  }
  if(board[0] == board[3] == board[6] == player){
    /*check first column */
    return 1;
  }
  if(board[0] == board[4] == board[8] == player){
    /* check from top left corner to bottom right corner diagonal */
    return 1;
  }
  if(board[1] == board[4] == board[7] == player){
    /* check middle column */
    return 1;
  }
  if(board[2] == board[5] == board[8] == player){
    /* check last column */
    return 1;
  }
  if(board[2] == board[4] == board[6] == player){
    /*check top right corner to bottom left corner */
    return 1;
  }
  if(board[3] == board[4] == board [5] == player){
    /* check middle row */
    return 1;
  }
  if(board[6] == board[7] == board[8] == player){
    /* check bottom row */
    return 1;
  }
  return 0;

}