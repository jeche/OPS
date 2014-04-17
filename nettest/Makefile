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

all: server tictactoe migrateTest

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

server.o: server.c
	$(CC) $(CFLAGS) -c server.c
server: server.o start.o
	$(LD) $(LDFLAGS) start.o server.o -o server.coff
	../bin/coff2noff server.coff server

tictactoe.o: tictactoe.c
	$(CC) $(CFLAGS) -c tictactoe.c
tictactoe: tictactoe.o start.o
	$(LD) $(LDFLAGS) start.o tictactoe.o -o tictactoe.coff
	../bin/coff2noff tictactoe.coff tictactoe

migrateTest.o: migrateTest.c
	$(CC) $(CFLAGS) -c migrateTest.c
migrateTest: migrateTest.o start.o
	$(LD) $(LDFLAGS) start.o migrateTest.o -o migrateTest.coff
	../bin/coff2noff migrateTest.coff migrateTest