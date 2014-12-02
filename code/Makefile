# Copyright (c) 1992 The Regents of the University of California.
# All rights reserved.  See copyright.h for copyright notice and limitation 
# of liability and disclaimer of warranty provisions.

.PHONY: clean tags fmt ubuntu print turnin

MAKE=make --no-print-directory
LPR=lpr

all: 
	@ echo "## threads"
	@ $(MAKE) -C threads
	@ echo "## userprog"
	@ $(MAKE) -C userprog
	@ echo "## vm"
	@ $(MAKE) -C vm
	@ echo "## filesys"
	@ $(MAKE) -C filesys
	@ echo "## network"
	@ $(MAKE) -C network
	@ echo "## bin"
	@ $(MAKE) -C bin
	@ echo "## test"
	@ $(MAKE) -C test

# don't delete executables in "test" in case there is no cross-compiler
clean:
	#original
	 rm -f *~ */{core,nachos,DISK,*.o,swtch.s,*~} test/{*.coff} bin/{coff2flat,coff2noff,disassemble,out}
	#Ming 
	#rm -f *~ */{core,nachos,DISK,*.o,swtch.s,*~} test/{*.coff} bin/{coff2flat,disassemble,out}

.PHONY: tags fmt ubuntu

tags:
	ctags -h=.cc.h -R

fmt:
	astyle `find . -name *.c` `find . -name *.h` `find . -name *.cc` -nQ

ubuntu:
	sudo apt-get install gcc-multilib g++-multilib lib32stdc++6 

print:
	$(LPR) Makefile* */Makefile
	$(LPR) threads/*.h threads/*.cc threads/*.s
	$(LPR) userprog/*.h userprog/*.cc
	$(LPR) filesys/*.h filesys/*.cc
	$(LPR) network/*.h network/*.cc 
	$(LPR) machine/*.h machine/*.cc
	$(LPR) bin/noff.h bin/coff.h bin/coff2noff.c
	$(LPR) test/*.h test/*.c test/*.s

turnin: clean
	cd .. && tar czf code.tgz code
	turnin ../code.tgz
