# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as
GCCDIR = /home/f85/kearns/nachos_compiler/bin/
#LDFLAGS = -T script -n -nostartfiles
LDFLAGS = -T nachos.ld -n -nostartfiles
ASFLAGS =
CPPFLAGS = $(INCDIR) -DCHANGED


# if you aren't cross-compiling:
#GCCDIR =
#LDFLAGS = -N -T 0
#ASFLAGS =
#CPPFLAGS = -P $(INCDIR)


CC = $(GCCDIR)decstation-ultrix-gcc
AS = $(GCCDIR)decstation-ultrix-as
LD = $(GCCDIR)decstation-ultrix-ld

CPP = /usr/bin/cpp -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR) -DCHANGED -nostartfiles

all: halt shell matmult sort fileio hellofile fromcons hellocons newfork argkid arg_seq_child bogus argtest deepfork deepkid1 deepkid2 kid maxfork parent_child seq_child share sharekid fork cat cp echo widefork ckmatmult rematmult vmtoobig huge qmatmult qsort Xkid vmtorture bogus1 atomicfile atomicfile2 test1 test2 test3 addproc cowhammer

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

hellocons.o: hellocons.c
	$(CC) $(CFLAGS) -c hellocons.c
hellocons: hellocons.o start.o
	$(LD) $(LDFLAGS) start.o hellocons.o -o hellocons.coff
	../bin/coff2noff hellocons.coff hellocons

fromcons.o: fromcons.c
	$(CC) $(CFLAGS) -c fromcons.c
fromcons: fromcons.o start.o
	$(LD) $(LDFLAGS) start.o fromcons.o -o fromcons.coff
	../bin/coff2noff fromcons.coff fromcons

hellofile.o: hellofile.c
	$(CC) $(CFLAGS) -c hellofile.c
hellofile: hellofile.o start.o
	$(LD) $(LDFLAGS) start.o hellofile.o -o hellofile.coff
	../bin/coff2noff hellofile.coff hellofile

fileio.o: fileio.c
	$(CC) $(CFLAGS) -c fileio.c
fileio: fileio.o start.o
	$(LD) $(LDFLAGS) start.o fileio.o -o fileio.coff
	../bin/coff2noff fileio.coff fileio

newfork.o: newfork.c
	$(CC) $(CFLAGS) -c newfork.c
newfork: newfork.o start.o
	$(LD) $(LDFLAGS) start.o newfork.o -o newfork.coff
	../bin/coff2noff newfork.coff newfork

argkid.o: argkid.c
	$(CC) $(CFLAGS) -c argkid.c
argkid: argkid.o start.o
	$(LD) $(LDFLAGS) start.o argkid.o -o argkid.coff
	../bin/coff2noff argkid.coff argkid

arg_seq_child.o: arg_seq_child.c
	$(CC) $(CFLAGS) -c arg_seq_child.c
arg_seq_child: arg_seq_child.o start.o
	$(LD) $(LDFLAGS) start.o arg_seq_child.o -o arg_seq_child.coff
	../bin/coff2noff arg_seq_child.coff arg_seq_child

bogus.o: bogus.c
	$(CC) $(CFLAGS) -c bogus.c
bogus: bogus.o start.o
	$(LD) $(LDFLAGS) start.o bogus.o -o bogus.coff
	../bin/coff2noff bogus.coff bogus

argtest.o: argtest.c
	$(CC) $(CFLAGS) -c argtest.c
argtest: argtest.o start.o
	$(LD) $(LDFLAGS) start.o argtest.o -o argtest.coff
	../bin/coff2noff argtest.coff argtest

deepfork.o: deepfork.c
	$(CC) $(CFLAGS) -c deepfork.c
deepfork: deepfork.o start.o
	$(LD) $(LDFLAGS) start.o deepfork.o -o deepfork.coff
	../bin/coff2noff deepfork.coff deepfork

deepkid1.o: deepkid1.c
	$(CC) $(CFLAGS) -c deepkid1.c
deepkid1: deepkid1.o start.o
	$(LD) $(LDFLAGS) start.o deepkid1.o -o deepkid1.coff
	../bin/coff2noff deepkid1.coff deepkid1

deepkid2.o: deepkid2.c
	$(CC) $(CFLAGS) -c deepkid2.c
deepkid2: deepkid2.o start.o
	$(LD) $(LDFLAGS) start.o deepkid2.o -o deepkid2.coff
	../bin/coff2noff deepkid2.coff deepkid2

kid.o: kid.c
	$(CC) $(CFLAGS) -c kid.c
kid: kid.o start.o
	$(LD) $(LDFLAGS) start.o kid.o -o kid.coff
	../bin/coff2noff kid.coff kid

maxfork.o: maxfork.c
	$(CC) $(CFLAGS) -c maxfork.c
maxfork: maxfork.o start.o
	$(LD) $(LDFLAGS) start.o maxfork.o -o maxfork.coff
	../bin/coff2noff maxfork.coff maxfork

parent_child.o: parent_child.c
	$(CC) $(CFLAGS) -c parent_child.c
parent_child: parent_child.o start.o
	$(LD) $(LDFLAGS) start.o parent_child.o -o parent_child.coff
	../bin/coff2noff parent_child.coff parent_child

seq_child.o: seq_child.c
	$(CC) $(CFLAGS) -c seq_child.c
seq_child: seq_child.o start.o
	$(LD) $(LDFLAGS) start.o seq_child.o -o seq_child.coff
	../bin/coff2noff seq_child.coff seq_child

share.o: share.c
	$(CC) $(CFLAGS) -c share.c
share: share.o start.o
	$(LD) $(LDFLAGS) start.o share.o -o share.coff
	../bin/coff2noff share.coff share

sharekid.o: sharekid.c
	$(CC) $(CFLAGS) -c sharekid.c
sharekid: sharekid.o start.o
	$(LD) $(LDFLAGS) start.o sharekid.o -o sharekid.coff
	../bin/coff2noff sharekid.coff sharekid

fork.o: fork.c
	$(CC) $(CFLAGS) -c fork.c
fork: fork.o start.o
	$(LD) $(LDFLAGS) start.o fork.o -o fork.coff
	../bin/coff2noff fork.coff fork

cat.o: cat.c
	$(CC) $(CFLAGS) -c cat.c
cat: cat.o start.o
	$(LD) $(LDFLAGS) start.o cat.o -o cat.coff
	../bin/coff2noff cat.coff cat

cp.o: cp.c
	$(CC) $(CFLAGS) -c cp.c
cp: cp.o start.o
	$(LD) $(LDFLAGS) start.o cp.o -o cp.coff
	../bin/coff2noff cp.coff cp

echo.o: echo.c
	$(CC) $(CFLAGS) -c echo.c
echo: echo.o start.o
	$(LD) $(LDFLAGS) start.o echo.o -o echo.coff
	../bin/coff2noff echo.coff echo

widefork.o: widefork.c
	$(CC) $(CFLAGS) -c widefork.c
widefork: widefork.o start.o
	$(LD) $(LDFLAGS) start.o widefork.o -o widefork.coff
	../bin/coff2noff widefork.coff widefork

ckmatmult.o: ckmatmult.c
	$(CC) $(CFLAGS) -c ckmatmult.c
ckmatmult: ckmatmult.o start.o
	$(LD) $(LDFLAGS) start.o ckmatmult.o -o ckmatmult.coff
	../bin/coff2noff ckmatmult.coff ckmatmult

rematmult.o: rematmult.c
	$(CC) $(CFLAGS) -c rematmult.c
rematmult: rematmult.o start.o
	$(LD) $(LDFLAGS) start.o rematmult.o -o rematmult.coff
	../bin/coff2noff rematmult.coff rematmult

vmtoobig.o: vmtoobig.c
	$(CC) $(CFLAGS) -c vmtoobig.c
vmtoobig: vmtoobig.o start.o
	$(LD) $(LDFLAGS) start.o vmtoobig.o -o vmtoobig.coff
	../bin/coff2noff vmtoobig.coff vmtoobig

huge.o: huge.c
	$(CC) $(CFLAGS) -c huge.c
huge: huge.o start.o
	$(LD) $(LDFLAGS) start.o huge.o -o huge.coff
	../bin/coff2noff huge.coff huge

qmatmult.o: qmatmult.c
	$(CC) $(CFLAGS) -c qmatmult.c
qmatmult: qmatmult.o start.o
	$(LD) $(LDFLAGS) start.o qmatmult.o -o qmatmult.coff
	../bin/coff2noff qmatmult.coff qmatmult

qsort.o: qsort.c
	$(CC) $(CFLAGS) -c qsort.c
qsort: qsort.o start.o
	$(LD) $(LDFLAGS) start.o qsort.o -o qsort.coff
	../bin/coff2noff qsort.coff qsort

Xkid.o: Xkid.c
	$(CC) $(CFLAGS) -c Xkid.c
Xkid: Xkid.o start.o
	$(LD) $(LDFLAGS) start.o Xkid.o -o Xkid.coff
	../bin/coff2noff Xkid.coff Xkid

vmtorture.o: vmtorture.c
	$(CC) $(CFLAGS) -c vmtorture.c
vmtorture: vmtorture.o start.o
	$(LD) $(LDFLAGS) start.o vmtorture.o -o vmtorture.coff
	../bin/coff2noff vmtorture.coff vmtorture

bogus1.o: bogus1.c
	$(CC) $(CFLAGS) -c bogus1.c
bogus1: bogus1.o start.o
	$(LD) $(LDFLAGS) start.o bogus1.o -o bogus1.coff
	../bin/coff2noff bogus1.coff bogus1

atomicfile.o: atomicfile.c
	$(CC) $(CFLAGS) -c atomicfile.c
atomicfile: atomicfile.o start.o
	$(LD) $(LDFLAGS) start.o atomicfile.o -o atomicfile.coff
	../bin/coff2noff atomicfile.coff atomicfile

atomicfile2.o: atomicfile2.c
	$(CC) $(CFLAGS) -c atomicfile2.c
atomicfile2: atomicfile2.o start.o
	$(LD) $(LDFLAGS) start.o atomicfile2.o -o atomicfile2.coff
	../bin/coff2noff atomicfile2.coff atomicfile2

test1.o: test1.c
	$(CC) $(CFLAGS) -c test1.c
test1: test1.o start.o
	$(LD) $(LDFLAGS) start.o test1.o -o test1.coff
	../bin/coff2noff test1.coff test1

test2.o: test2.c
	$(CC) $(CFLAGS) -c test2.c
test2: test2.o start.o
	$(LD) $(LDFLAGS) start.o test2.o -o test2.coff
	../bin/coff2noff test2.coff test2

test3.o: test3.c
	$(CC) $(CFLAGS) -c test3.c
test3: test3.o start.o
	$(LD) $(LDFLAGS) start.o test3.o -o test3.coff
	../bin/coff2noff test3.coff test3

addproc.o: addproc.c
	$(CC) $(CFLAGS) -c addproc.c
addproc: addproc.o start.o
	$(LD) $(LDFLAGS) start.o addproc.o -o addproc.coff
	../bin/coff2noff addproc.coff addproc

cowhammer.o: cowhammer.c
	$(CC) $(CFLAGS) -c cowhammer.c
cowhammer: cowhammer.o start.o
	$(LD) $(LDFLAGS) start.o cowhammer.o -o cowhammer.coff
	../bin/coff2noff cowhammer.coff cowhammer