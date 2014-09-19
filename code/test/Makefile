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

# UCSD OSTL (Linux)
# GCCDIR = /home/ostl/cs120f/public/mips-x86.linux-xgcc/
# UCSD uAPE (Sparc/Solaris)
# GCCDIR = /home/solaris/ieng9/cs120f/public/mips-sparc.solaris-xgcc/

# GCCDIR = $(CSE120_GCC)
GCCDIR = mips-xgcc/

LDFLAGS = -T script -N
ASFLAGS = -mips2
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)

PATH = $(GCCDIR):/lib:/usr/bin:/bin

CC = $(GCCDIR)gcc
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = gcc -E
INCDIR =-I../userprog -I../threads
# CFLAGS = -G 0 -c $(INCDIR)
CFLAGS = -G 0 -c $(INCDIR)

CFILES = $(wildcard *.c)
OFILES = $(patsubst %.c,_/%.o,$(CFILES))
TARGETS = $(patsubst %.c,_/%,$(CFILES))

.PRECIOUS: $(OFILES)

all: $(TARGETS)

_/start.o: start.cc ../userprog/syscall.h
	@mkdir -p _
	$(CPP) $(CPPFLAGS) start.cc > _/strt.s
	$(AS) $(ASFLAGS) -o _/start.o _/strt.s

_/%: _/start.o _/%.o
	@mkdir -p _
	$(LD) $(LDFLAGS) $^ -o $@.coff
	../bin/coff2noff $@.coff $@ > $@.log
	@cp $@ $(notdir $@)

_/%.o: %.c
	@mkdir -p _
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -rf _
