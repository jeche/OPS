/* ticTacToe.c
 *
 * Kid in simple argument test.
 *
 */

 #include "syscall.h"
int
main(int argc, char **argv)
{
  char c;
  char m;
  char t;
  char playerChar;
  char otherChar;
  char stupid[2];
  char recvInfo[5];
  char sends[4];
  char board[10];
  char buf[11];
  int i;
  int mPid;
  int l;
  int count;
  int row;
  int col;
  int rowlength;
  char * ans;
  i = 0;
  l = GetMailbox();
  sends[3] = '\0';
  recvInfo[0] = '\0';
  recvInfo[1] = '\0';
  recvInfo[2] = '\0';
  recvInfo[3] = '\0';
  recvInfo[4] = '\0';
  /* send machine ID to known server (0) */
  /* Receive. If ID received is a w then receive until ID and mailbox comes in */
  prints("What is your machine id? ", ConsoleOutput);
  Read(&c, 1, ConsoleInput);
  Read(&m, 1, ConsoleInput);
  mPid = Open("serve");
  Read(recvInfo, 1, mPid);
  i = atoi2(recvInfo);
  recvInfo[0] = '0';
  recvInfo[1] = c;
  recvInfo[2] = '0';
  ans = itoa3(l, buf);
  recvInfo[3] = ans[0];
  recvInfo[4] = '\0';
  Send(recvInfo, 5, l, 0, i);
  Recv(board, 10, l);
  m = board[9];
  printBoard(board);

  if(m == '1'){
    playerChar = 'X';
    otherChar = 'O';
    board[9] = 's';
    while(board[9] == 's'){
      prints("Row of move (0 - 2): ", ConsoleOutput);
      Read(&c, 1, ConsoleInput);
      Read(&t, 1, ConsoleInput);
      stupid[0] = c;
      stupid[1] = '\0';
      row = atoi2(stupid);

      prints("Col of move (0 - 2): ", ConsoleOutput);
      Read(&c, 1, ConsoleInput);
      Read(&t, 1, ConsoleInput);
      stupid[0] = c;
      stupid[1] = '\0';
      col = atoi2(stupid);

      while(checkValidMove(row, col, board) != 1) {
        prints("Retry, invalid move\n", ConsoleOutput);
        prints("Row of move (0 - 2): ", ConsoleOutput);
        Read(&c, 1, ConsoleInput);
        Read(&t, 1, ConsoleInput);
        stupid[0] = c;
        stupid[1] = '\0';
        row = atoi2(stupid);

        prints("Col of move (0 - 2): ", ConsoleOutput);
        Read(&c, 1, ConsoleInput);
        Read(&t, 1, ConsoleInput);
        stupid[0] = c;
        stupid[1] = '\0';
        col = atoi2(stupid);
      }
      board[row * 3 + col] = playerChar;
      printBoard(board);
      Send(board, 10, l, 0, i);
      Recv(board, 10, l);
      printBoard(board);
    }

  } 

  else{
    playerChar = 'O';
    otherChar = 'X';
    board[9] = 's';
    Recv(board, 10, l);
    while(board[9] == 's'){
      if(board[9] != 's'){
        break;
      }
      else{
        printBoard(board);
        prints("Row of move (0 - 2): ", ConsoleOutput);
        Read(&c, 1, ConsoleInput);
        Read(&t, 1, ConsoleInput);
        stupid[0] = c;
        stupid[1] = '\0';
        row = atoi2(stupid);

        prints("Col of move (0 - 2): ", ConsoleOutput);
        Read(&c, 1, ConsoleInput);
        Read(&t, 1, ConsoleInput);
        stupid[0] = c;
        stupid[1] = '\0';
        col = atoi2(stupid);

        while(checkValidMove(row, col, board) != 1) {
          prints("Retry, invalid move\n", ConsoleOutput);
          prints("Row of move (0 - 2): ", ConsoleOutput);
          Read(&c, 1, ConsoleInput);
          Read(&t, 1, ConsoleInput);
          stupid[0] = c;
          stupid[1] = '\0';
          row = atoi2(stupid);

          prints("Col of move (0 - 2): ", ConsoleOutput);
          Read(&c, 1, ConsoleInput);
          Read(&t, 1, ConsoleInput);
          stupid[0] = c;
          stupid[1] = '\0';
          col = atoi2(stupid);
        }
        board[row * 3 + col] = playerChar;
        printBoard(board);
        Send(board, 10, l, 0, i);
        Recv(board, 10, l);
      }
    }
  }
  if((board[9] == 'X' && playerChar == 'X') || (board[9] == 'O' && playerChar == 'O')){
    prints("You won!\n", ConsoleOutput);
    Send(board, 10, l, 0, i);
  }
  else if(board[9] == 'd'){
    prints("Draw.\n", ConsoleOutput);
    Send(board, 10, l, 0, i);
  }
  else{
    prints("Loser :P\n", ConsoleOutput);
    Send(board, 10, l, 0, i);
  }
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
  int polo;
  int timeT;
  char bobo[61];
  polo =  0;
  timeT = 0;
  bobo[0] = ' ';
  /*prints(b, ConsoleOutput);*/
  for(polo; polo < 60; polo++){
    if((polo / 12 % 2) == 0){
      bobo[polo] = ' ';
      bobo[polo + 1] = b[timeT];
      bobo[polo + 2] = ' ';
      bobo[polo + 3] = '|';
      timeT = timeT + 1;
      if(timeT > 9){
        Halt();
      }
    } else{
      bobo[polo] = '-';
      bobo[polo + 1] = '-';
      bobo[polo + 2] = '-';
      bobo[polo + 3] = '-';
    }
    polo = polo + 3;

  }
  bobo[11] = '\n';
  bobo[23] = '\n';
  bobo[35] = '\n';
  bobo[47] = '\n';
  bobo[59] = '\n';
  bobo[60] = '\0';
  prints("\n", ConsoleOutput);
  prints(bobo, ConsoleOutput);
  prints("\n*******\n\n", ConsoleOutput);
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

char* itoa3(n, buffer)
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
