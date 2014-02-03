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

all: dupparent dupkid test1 test2 test3 addproc cowhammer

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

dupparent.o: dupparent.c
	$(CC) $(CFLAGS) -c dupparent.c
dupparent: dupparent.o start.o
	$(LD) $(LDFLAGS) start.o dupparent.o -o dupparent.coff
	../bin/coff2noff dupparent.coff dupparent

dupkid.o: dupkid.c
	$(CC) $(CFLAGS) -c dupkid.c
dupkid: dupkid.o start.o
	$(LD) $(LDFLAGS) start.o dupkid.o -o dupkid.coff
	../bin/coff2noff dupkid.coff dupkid

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


