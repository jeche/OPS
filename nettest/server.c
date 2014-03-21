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

  int count;
  int  i;
  int rowlength;

  int row;
  int col;
  char board[61];
  char buf[11];
  char recvInfo[5];
  char split1[3];
  char split2[3];
  char c;

  turnC = 0;

  count = 0;
  i = 1;
  rowlength = 12;
  board[0] = 's';
  /* Generates a blank tic tac toe board for sending back and forth */
  while(count < 5){
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
  }

  mMbox = GetMailbox();

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
  board[0] = '1';
  Send(board, 61, p1, mp1);
  board[0] = '2';
  Send(board, 61, p2, mp2);
  board[0] = 's';
  while(board[0] != 'o'){
    /* Receive move from tictactoe process */
    Recv(recvInfo, 4, mMbox);
    i = 0;
    if(turnM == p1 && recvInfo[0] == '1'){
      c = recvInfo[1];
      row = atoi(&c);
      c = recvInfo[2];
      col = atoi(&c);

      if(board[1+ row*24 +col * 4] == ' '){
        board[0] = 's';
        board[1+ row*24 +col * 4] = 'X';
        /* check for winning move */
        if(getIndex(row - 1, col - 1) < 61 && getIndex(row - 1, col - 1) >= 0 && board[getIndex(row - 1, col - 1)] == 'X'){
          if(getIndex(row + 1, col + 1) < 61 && getIndex(row + 1, col + 1) >= 0 && board[getIndex(row + 1, col + 1)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
          else if(getIndex(row - 2, col - 2) < 61 && getIndex(row - 2, col - 2) >= 0 && board[getIndex(row - 2, col - 2)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
        }
        if(getIndex(row + 1, col + 1) < 61 && getIndex(row + 1, col + 1) >= 0 && board[getIndex(row + 1, col + 1)] == 'X'){
          if(getIndex(row - 1, col - 1) < 61 && getIndex(row - 1, col - 1) >= 0 && board[getIndex(row - 1, col - 1)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
          else if(getIndex(row + 2, col + 2) < 61 && getIndex(row + 2, col + 2) >= 0 && board[getIndex(row + 2, col + 2)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
        }
        if (getIndex(row + 1, col - 1) < 61 && getIndex(row + 1, col - 1) >= 0 && board[getIndex(row + 1, col - 1)] == 'X'){
          if(getIndex(row + 2, col - 2) < 61 && getIndex(row + 2, col - 2) >= 0 && board[getIndex(row + 2, col - 2)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
          else if(getIndex(row - 1, col + 1) < 61 && getIndex(row - 1, col + 1) >= 0 && board[getIndex(row - 1, col + 1)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
        }
        if (getIndex(row - 1, col + 1) < 61 && getIndex(row - 1, col + 1) >= 0 && board[getIndex(row - 1, col + 1)] == 'X'){
          if(getIndex(row - 2, col + 2) < 61 && getIndex(row - 2, col + 2) >= 0 && board[getIndex(row - 2, col + 2)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
          else if(getIndex(row + 1, col - 1) < 61 && getIndex(row + 1, col - 1) >= 0 && board[getIndex(row + 1, col - 1)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
        }
        if (getIndex(row, col + 1) < 61 && getIndex(row, col + 1) >= 0 && board[getIndex(row, col + 1)] == 'X'){
          if(getIndex(row, col + 2) < 61 && getIndex(row, col + 2) >= 0 && board[getIndex(row, col + 2)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
          else if(getIndex(row, col - 1) < 61 && getIndex(row, col - 1) >= 0 && board[getIndex(row, col - 1)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
        }
        if (getIndex(row, col  - 1 ) < 61 && getIndex(row, col  - 1 ) >= 0 && board[getIndex(row, col  - 1 )] == 'X'){
          if(getIndex(row, col - 2) < 61 && getIndex(row, col - 2) >= 0 && board[getIndex(row, col - 2)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
          else if(getIndex(row, col  + 1) < 61 && getIndex(row, col  + 1) >= 0 && board[getIndex(row, col  + 1)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
        }
        if (getIndex(row - 1, col ) < 61 && getIndex(row - 1, col) >= 0 && board[getIndex(row - 1, col)] == 'X'){
          if(getIndex(row - 2, col) < 61 && getIndex(row - 2, col) >= 0 && board[getIndex(row - 2, col)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
          else if(getIndex(row + 1, col ) < 61 && getIndex(row + 1, col) >= 0 && board[getIndex(row + 1, col)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
        }        
        if (getIndex(row + 1, col ) < 61 && getIndex(row + 1, col) >= 0 && board[getIndex(row + 1, col)] == 'X'){
          if(getIndex(row + 2, col) < 61 && getIndex(row + 2, col) >= 0 && board[getIndex(row + 2, col)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
          else if(getIndex(row - 1, col ) < 61 && getIndex(row - 1, col) >= 0 && board[getIndex(row - 1, col)] == 'X'){
            board[0] = 'o';
            board[1] = '1';
          }
        }
        if(turnC >= 8 && board[0] != 'o'){
          board[0] = 'o';
          board[1] = 'D';
        }  
        turnM = p2;
        turnB = mp2;
        turnC = turnC + 1;
        Send(board, 61, p1, mp1);
        if(board[0] != 'o'){
          board[0] = 'u';
        }
        Send(board, 61, p2, mp2);
        if(board[0] != 'o'){
          board[0] = 's';
        }
      }
      else{
        prints("\n", ConsoleOutput);
        prints("TURNM:  ", ConsoleOutput);
        printd(row, ConsoleOutput);
        prints("\n", ConsoleOutput);
        prints("FIRST  IFFFFERRR   RECVINFO:  ", ConsoleOutput);
        prints(recvInfo, ConsoleOutput);
        prints("\n Board SPot:  ", ConsoleOutput);
        c = board[1+ row*24 +col * 4];
        printd(col, ConsoleOutput);
        prints(c, ConsoleOutput);
        prints("\n", ConsoleOutput);
        prints("\n", ConsoleOutput);
        prints("\n", ConsoleOutput);
        prints(board, ConsoleOutput);

        board[0] = 'e';
        Send(board, 61, p1, mp1);
      }  
    }
    else if (turnM == p2 && recvInfo[0] == '2'){
      c = recvInfo[1];
      row = atoi(&c);
      c = recvInfo[2];
      col = atoi(&c);

      if(board[1+ row*24 +col * 4] == ' '){
        board[0] = 's';
        board[1+ row*24 +col * 4] = 'O';
        /* check for winning move */
        if(getIndex(row - 1, col - 1) < 61 && getIndex(row - 1, col - 1) >= 0 && board[getIndex(row - 1, col - 1)] == 'O'){
          if(getIndex(row + 1, col + 1) < 61 && getIndex(row + 1, col + 1) >= 0 && board[getIndex(row + 1, col + 1)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
          else if(getIndex(row - 2, col - 2) < 61 && getIndex(row - 2, col - 2) >= 0 && board[getIndex(row - 2, col - 2)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
        }
        if(getIndex(row + 1, col + 1) < 61 && getIndex(row + 1, col + 1) >= 0 && board[getIndex(row + 1, col + 1)] == 'O'){
          if(getIndex(row - 1, col - 1) < 61 && getIndex(row - 1, col - 1) >= 0 && board[getIndex(row - 1, col - 1)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
          else if(getIndex(row + 2, col + 2) < 61 && getIndex(row + 2, col + 2) >= 0 && board[getIndex(row + 2, col + 2)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
        }
        if (getIndex(row + 1, col - 1) < 61 && getIndex(row + 1, col - 1) >= 0 && board[getIndex(row + 1, col - 1)] == 'O'){
          if(getIndex(row + 2, col - 2) < 61 && getIndex(row + 2, col - 2) >= 0 && board[getIndex(row + 2, col - 2)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
          else if(getIndex(row - 1, col + 1) < 61 && getIndex(row - 1, col + 1) >= 0 && board[getIndex(row - 1, col + 1)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
        }
        if (getIndex(row - 1, col + 1) < 61 && getIndex(row - 1, col + 1) >= 0 && board[getIndex(row - 1, col + 1)] == 'O'){
          if(getIndex(row - 2, col + 2) < 61 && getIndex(row - 2, col + 2) >= 0 && board[getIndex(row - 2, col + 2)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
          else if(getIndex(row + 1, col - 1) < 61 && getIndex(row + 1, col - 1) >= 0 && board[getIndex(row + 1, col - 1)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
        }
        if (getIndex(row, col + 1) < 61 && getIndex(row, col + 1) >= 0 && board[getIndex(row, col + 1)] == 'O'){
          if(getIndex(row, col + 2) < 61 && getIndex(row, col + 2) >= 0 && board[getIndex(row, col + 2)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
          else if(getIndex(row, col - 1) < 61 && getIndex(row, col - 1) >= 0 && board[getIndex(row, col - 1)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
        }
        if (getIndex(row, col  - 1 ) < 61 && getIndex(row, col  - 1 ) >= 0 && board[getIndex(row, col  - 1 )] == 'O'){
          if(getIndex(row, col - 2) < 61 && getIndex(row, col - 2) >= 0 && board[getIndex(row, col - 2)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
          else if(getIndex(row, col  + 1) < 61 && getIndex(row, col  + 1) >= 0 && board[getIndex(row, col  + 1)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
        }
        if (getIndex(row - 1, col ) < 61 && getIndex(row - 1, col) >= 0 && board[getIndex(row - 1, col)] == 'O'){
          if(getIndex(row - 2, col) < 61 && getIndex(row - 2, col) >= 0 && board[getIndex(row - 2, col)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
          else if(getIndex(row + 1, col ) < 61 && getIndex(row + 1, col) >= 0 && board[getIndex(row + 1, col)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
        }        
        if (getIndex(row + 1, col ) < 61 && getIndex(row + 1, col) >= 0 && board[getIndex(row + 1, col)] == 'O'){
          if(getIndex(row + 2, col) < 61 && getIndex(row + 2, col) >= 0 && board[getIndex(row + 2, col)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
          else if(getIndex(row - 1, col ) < 61 && getIndex(row - 1, col) >= 0 && board[getIndex(row - 1, col)] == 'O'){
            board[0] = 'o';
            board[1] = '2';
          }
        }

        if(turnC >= 8 && board[0] != 'o'){
          board[0] = 'o';
          board[1] = 'D';
        }

        turnM = p1;
        turnB = mp1;
        turnC = turnC + 1;
        Send(board, 61, p2, mp2);
        if(board[0] != 'o'){
          board[0] = 'u';
        }
        Send(board, 61, p1, mp1);
        if(board[0] != 'o'){
          board[0] = 's';
        }
      } else{
        prints("\n", ConsoleOutput);
        prints("TURNM:  ", ConsoleOutput);
        printd(turnM, ConsoleOutput);
        prints("\n", ConsoleOutput);
        prints("RECVINFO:  ", ConsoleOutput);
        prints(recvInfo, ConsoleOutput);
        board[0] = 'e';
        Send(board, 61, p2, mp2);
      }      
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

int getIndex(op, lop)
int op;
int lop;
{
  return 1+ op*24 +lop * 4;
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

