# Nachos

CSE 120 Project for UCSD

Group 44 - Ming Qin, Xinyu Qian, Evan Carey, Kevin Caasi

---------------------

# Project 3: Virtual Memory

#### Members of Group 44
- Ming Qin    - Addrspace, MemoryManager.
- Xinyu Qian  - Replacement algorithm report, test cases.
- Evan Carey  - BackingStore, test cases, debugging.
- Kevin Caasi - Readme, test cases, debugging.

---------------------
##1. Preliminary Trial

In the class of AddrSpace, several functions were added to support demand paging. AddrSpace::pageFault(int vpn) is called by exception handler. It will then call AddrSpace::LoadPage(int vpn) to copy data from executable file, or BackingStore:PageIn() to load data from swap file.

In the destructor of AddrSpace, unlike project 2, we need to check the valid bit before free up memory in the page table. Just this single bug kept us debugging for two days.


---------------------
##2. Page Replacement

In addition to part 1, function AddrSpace::evictPage(int vpn) is added. It will be called by memory manager when memory is full.

A new class is added, named BackingStore. It copies data from and to the swap file.

The MemoryManager class is enhanced. Now it has variables to keep pointers to AddrSpace. It will figure out which page to evict using designated algorithm, and then call AddrSpace:evictPage().


---------------------
##3. Testing

We tested our virtual memory in this part. With the help of numPageOuts and numPageIns implemented in part 4. We are able to check the correctness of our virtual memory.

Tests:(called from userprog by executing ./nachos -x ../test/{testname})

	pj3testp3part:a test program that only references some of the pages(all the other tests use all of the pages)
	pj3testp3write: to test if we handle the dirty pages correctly. Fisrt create an array larger than the physical memory size, then we write every element of the array one by one for three times. At last we sum the elements of the array up to check the correctness. And we get the expected answer.
	
	tests for different locality£º
	We are using an array and reference to it with different patterns to show the compare the number of faults and pageIns and pageOuts. To guarantee the correctness of the comparison, we set every test with 180 000 references to the same array. And all the tests are generateing the same number of random integers(but only randomLocality uses these numbers). 
	pj3testp3badLocality:We create an array larger than the physical memory size. Then referrence the array elements one by one. To make the case more common, we add two elements that are frequently referenced(to show that LRU works better than others).
	pj3testp3goodLocality:We create an array larger than the physical memory size. Then referrence only three of the elements in the array. So there will be much less page faults than other tests.
	pj3testp3randomLocality:We create an array larger than the physical memory size. Then reference the elements randomly. So two random numbers are generated and used on each reference(the other two tests are also genernating random numbers but not use them). And also there are two elements frequnted referenced. The number of page faults and pageIns is larger than it in goodLocality but less than it in badLocality .

	the test results are in the table test statistics.

---------------------
##4. Replacement Algorithm Report

We implemented numPageOuts and numPageIns to calculate the number of pages writing into BackingStore and the number of pages writing into physical memory. Together with numPageFaults we are able to get an idea of the work load of our virtual memory and furthermore to test the performence of different page replacement algorithms
We tested every algorithms with every test cases. And for most cases LRU performs better than the other two. Random and FIFO's performence are quite the same.

Check test statistics for details.


---------------------
##5. LRU 

We implemented LRU. We set a counter for every physical pages. Each time a page is referenced, the counter of that page is set to zero and all the other counters are increased by one. When we have to evict a page, the page with the largest counter is evicted.
A worse case for LRU is that if a array is just larger than the physical page size and we are referenceing the array one by one recursively. LRU will cause a page fault on every reference, since it evict the least recently used paged and that's the page we are referencing next.
This test is lruWorstCase, in which random works better than LRU, and LRU FIFO perform quite the same(since the least recently used page is the first page comes in in this case). And for other test cases LRU works better than then other two.

The following 3 command switches are used for replacement algorithms.
1. `-fifo` will choose FIFO algorithm, which is also the default option if no option is specified.
2. `-random` : Random replacement algorithm.
3. `-lru` : Least Recently Used algorithm.

There's one extreme condition that will cause the time counter overflow, which is when a program has referenced the memory for "max integer" times but still haven't evicted any page. The maximum 32-bit integer is `2147483647`.

Check test statistics for details.

---------------------
test statistics

Physical memory size: 32 pages.
Page replacement policy: Random.
Program					PageFaults	PageOuts	PageIns
pj3testp3part			23			0			6
pj3testp3write			352			232			273
pj3testp3badLocality	95576		5909		95497						
pj3testp3goodLocality	114			65			35
pj3testp3randomLocality	59493		3645		59414
lruWorstCase			6234		199			6201


Physical memory size: 32 pages.
Page replacement policy: FIFO.
Program					PageFaults	PageOuts	PageIns
pj3testp3part			23 			0			6
pj3testp3write			370			248			291
pj3testp3badLocality	98335		5997		98256
pj3testp3goodLocality	108			61			29
pj3testp3randomLocality	59683		3693		59604
lruWorstCase			34279		1071		34246


Physical memory size: 32 pages.
Page replacement policy: LRU.
Program					PageFaults	PageOuts	PageIns
pj3testp3part			26			0			6
pj3testp3write			328			223			249
pj3testp3badLocality	65793		77			65714
pj3testp3goodLocality	93			59			14
pj3testp3randomLocality	41663		77			41584
lruWorstCase			31165		33			31132

---------------------

# Project 2: Multiprogramming

#### Members of Group 44
- Ming Qin - Addrspace, Exec and Exit, pipe, fork and yield, Testing
- Xinyu Qian - MemoryManager, Exec and Exit, Join, Testing
- Evan Carey - Console Read/Write, Exception Handling, Testing
- Kevin Caasi - Exception Handling, Testing
-------------------

##1. Load processes into the memory

We created class MemoryManager based on class BitMap. In addition, we use a Process class to manage operations of exec, exit, join, pipeline and multithreading. The codes in AddrSpace's constructor was moved into AddrSpace::Initialize(), which is called by Process::Load(), and Load is call by exec() and StartProcess().
The functionality of this part is tested in later parts.
---------------------

##2. Exec and Exit

	We implemented Exec and Exit, which create and exit a process. We use processTable to keep track of every process created. Exec returns a process identifier (SpaceId), and Exit will print out the status value passed as the parameter.

	Tests:(called from userprog by executing ./nachos -x ../test/{testname})
		testp2 - the first process calls Exec to create a second process(array) and save the return value of Exec as Exit status value. They get the proper Exit status value which are 2(SpaceId for the second process) for the first and 1128 for the second
		testp2_fileNameNotExist - call Exec with an in file name can not be found. Cause a fault Exec returns 0
		testp2_fileNameTooLong - call Exec with a file name too long(max 256). Cause a fault Exec returns 0
		testp2_fileNameNotEndWithNull - call Exec with a file name not ended in a null character. Since the undefined characters are NULLs, so system will always read a null character at the end. Thus the system regards the filename not ended in null the same as normal ones, if other conditions are the same.
		testp2_sizeTooLarge - call Exec on a process exceeds the physical memory. The system print "run out of physical memory" and Exec returns 0
		testp2_physicalMemoryBoundary - Use this test to check the system is able to load many programs one after another, and the Exit really deletes process. This test calls Exec to creat itself and then exit. So if exit really deletes the process the test will last forever otherwise it will stop when reaching the memory boundary. Press Ctrl+C to exit.
---------------------

##3. Passing arguments in Exec

Two functions are created in exception.cc, which are strUser2Kernel(char* src, char** dst) and strKernel2User(char* src, char* dst, int size). They copy strings between virtual memory and kernel memory. 
After argc and argv are copied into kernel, they are passed to Process::Load(), and then to AddrSpace::Initialize(). In Initialize(), argv is copied right after the 3 segment of code, initdata and uninitdata. The space is enlarged accordingly. The argc and the pointer of argv is then keep in some private variables in class AddrSpace for future initialization.

    Tests:(called from userprog by executing ./nachos -x ../test/testp3)
        testp3 - Generate 3 strings, "1", "3" and "5", and then exec testp3_addition. Here argc is 3.
                In testp3_addition the 3 digits will be summed up and shown in Exit value.
                We can find "Process 2 Exit(9)" in the output, since 1+3+5=9.
---------------------

##4. Synchronized console and Read/Write syscall

We implemented function to synchronously read from and write to the console.This was done by creating a SynchConsole class (/userprog/synchconsole.{cc,h}) that serves as a wrapper to the Nachos built-in asynchronous Console class. 
Reading from the console is done by the Read function, which uses a lock (readLock) and a semaphore (read) to guarantee mutual exclusion and then calls the Console's getChar function, reading from stdin and storing the returned character in the system buffer.
Writing to the console is done by the Write function, which uses a lock (writeLock) and a semaphore (write) to guarantee mutual exclusion and then calls the Console's putChar function, writing characters from the system buffer to stdout. 
If the arguments to Read and Write are invalid, the system calls will return -1 as an error. See tests of part 5.

    Tests:(called from userprog by executing ./nachos -x ../test/{testname})
        testp4_shell -  A simple shell that can execute other programs, for example try to
                type in "../test/snake". In this shell, argument is not surpported.
        snake - A test program that exercises Write system call implementation, provided by Professor Voelker.
        echo - It will repeat what you type in.
--------------------

##5. Exception Handling

We added functionality to the ExceptionHandler function in /userprog/exception.cc to handle user exceptions. When one of these exceptions {PageFaultException, ReadOnlyException, BusErrorException, AddressErrorException, OverflowException, IllegalInstrException} occurs, the triggering process is terminated, but other processes (and Nachos) are unaffected.

    Tests: (called from userprog by executing ./nachos -x ../test/{testname})
        testp5 - Triggers an IllegalInstrException by attempting to divide by zero.
        testp5_exec - Triggers an IllegalInstrException in a process created by the Exec system call
        testp5_2_exec - Executes 3 programs that will trigger AddressErrorException. They are:
            1. testp5_2_Normal - Directly access an invalid address. The kernel will kill this program by force.
            2. testp5_2_Write - Writes to console using invalid buffer address. The system call will return -1 and the program will Exit normally.
            3. testp5_2_Read - Similar to Write.
--------------------

##6. Join

We implemented join system call. It takes one argument, the SpaceId of the process that the caller wants to wait for. The process to be Joined must have been Exec'ed with its willJoin argument's last bit set to 1(sharing willJoin with pipe part7). Join returns the process status (Exit) code of the process that was Joined. Return -65535 from Join to signify an error.We implemented 4 errors,each with a test.

	Tests:(called from userprog by executing ./nachos -x ../test/{testname})
		testp6 - first process calls Join on the second process(array) and receives the second process' exit status(1128)
		testp6_join2ndTime - two processes calls Join on the same process and one of the caller result in a fault(Return value -65535)
		testp6_joinOnItself - the process calls Join on itself and result in a fault(Return value -65535)
		testp6_joinOnNotToJoined - the process calls Join on a process whose argument willJoin's last bit not set to 1, result in a fault(Return value -65535)
		testp6_joinOnInvalidPID - the process calls Join on a process whose Space Id can't not be found. Result in a fault(Return value -65535)
--------------------

##7. Pipe

We created the Pipe class. The parent process maintains a list of pipe line. Everytime it exec a new process with appropriate pipecrtl, it adds a new pipe to the list and set the inputPipe and outputPipe of the new process accordingly. 

	Tests:(called from userprog by executing ./nachos -x ../test/{testname})
        testp7_pipetest - Execute 4 process as follows: one pipetest_produce, two pipetest_mid and one pipetest_consume.
            The first process pipetest_produce will read a letter from the console, and pass it to the
            next process. Every process will add one letter to the string. The last process, 
            pipetest_consume, will print the string to the console.		
--------------------

##8. Multithreading

We provide support to multi-thread programs. Now the Exit system call implies that the calling thread is finished, and that if it's the last thread inside the process, the process is finished as well. The return value of the process depends on the last thread to call Exit. As an convention, We need user program to explicitly call Exit when the forked thread is finished.
In detail, when Fork is called, the kernel will add another user stack at the end of address space, then map the new space into physical memory. The machine registers will be set slightly different when initializing the new thread.

    Tests:(called from userprog by executing ./nachos -x ../test/{testname})
        testp8_multi - Forks a thread and ping-pong between the two threads.
--------------------

##9. Other

There's a test program named "sort". To run this test, please increase the UserStackNumPage in addrspace.h.


---------------------
# Project 1: Threads

#### Members of Group 44
- Ming Qin - Join, Mailbox, Testing, Priority
- Xinyu Qian - Whales, CV, Mailbox, Testing
- Evan Carey - Join, CV, Locks, Testing
- Kevin Caasi - Join, CV, Locks, Testing
-------------------

###Locks And Conditions

We implemented acquire and release using interrupts, asserts, wait and
ready queue. Similarly, condition variables use interrupts, wait and ready
queues, and locks. Instead of appending to the wait queue, we use
SortedInsert() to prioritize the threads.

    Test switch case numbers used:
        2: Expects to work successfully
        3: Assert should fail: Cannot delete a held lock by own thread
        4: Assert should fail: Cannot delete a held lock by another thread
        5: Assert Should fail: Cannot wait without holding a lock
        6: Expects to work successfully
        7: Expects to work successfully
        8: Assert fails: Cannot delete while threads on queue
---------------

###Mailbox

We used locks, and condition variables canSend and canReceive as well as a
bool named full. we ensure interleaving by inversion calling the condition
variables in both send and receive. Variable buff holds the word, and full
ensures that only one sender interacts with one receiver.

    Test switch case numbers used:
        9: Expects to work succesfully as sender comes first
        10:Expects to work succesffully as receiver comes first
        11:Only sender is available. Synchronization can not be achieved.
        12:Only receiver is available. Synchronization can not be achieved.
        13:Expects to work. Interleaving between senders and receivers
---------------

###Join
We added several member variables to the Thread class, including booleans 
that indicate whether the thread has forked (hasForked), whether the thread is 
joinable (willBeJoined - set based on constructor argument), and whether the thread 
has already been joined (hasJoined), as well as a lock and a condition variable.
When a parent invokes Join() on a child, the child acquires the lock and puts the 
parent to sleep until the child thread has finished.
Once a thread finishes, via a call to Finish(), it checks its hasJoined and willBeJoined status. 
If it is going to be joined but hasn't yet, it will wait for Join to be invoked and returned. 
Afterward, it will set itself as ready to be destroyed.

    Test switch case numbers used:
        21: a thread that will be joined only is destroyed once Join has been called on it
        22: if a parent calls Join on a child and the child is still executing, the parent waits
        23: if a parent calls Join on a child and the child has finished executing, the parent does not block
        24: a thread does not call join on itself 
        25: join is only invoked on threads created to be joined
        26: join is only called on a thread that has forked
        27: join is not called more than once on a thread
----------

###Priorities

We worked on priorities by utilizing the SortedInsert() function in list. However, this function originally sorts by ascending order. We want descending order. For this, we simply pass in a negation of the priority to the function to achieve the expected results. Later on, we modified Thread::Yield() so that high-priority thread will come back to itself if no other thread has higher priority.

    Test switch case numbers used:
       19: Expects to work successfully. Outputs for all test cases will be display on 
the screen after running "./nachos -q 19".

Those test cases include:

       <1> Priority test for scheduler. When Thread::Yield() is called, the scheduler 
              always run the thread with the highest priority, or switch between threads that share 
              the highest priority.
       <2> Priority test for synch primiives Lock.
       <3> Priority test for Semaphore.
       <4> Priority test for Condition Var, waked up by Signal();
       <5> Priority test for Condition Var, waked up by Broadcast();
       <6> [Extra Credit] There are 5 threads in this case. The lock-holding thread is successfully promoted, 
              so that it exits before all mid-priority threads finish. 
       <7> [Extra Credit] There are 4 threads in this case. The joinee is successfully promoted, so that it 
              exits before all mid-priority threads finish.
       <8> [Extra Credit] There are 6 threads in this case. When Join() is called, the joinee (it is also the lock 
              waiter) is promoted, as well as the lock holder. So that they exit before all mid-priority threads.

------------

###Whales
We used 4 semaphores to implement whale matching. There are 3 functions with similar codes male, female and matchmaker representing whales from each class. There are four semaphores thus four wait lists, one for each whale class to make sure only one whale from each class is waiting to be matched, and another for whales to wait for being matched. Only when 3 whales from each of the 3 classes come, can they form a match and then return, otherwise wait. To make things clear, we add variables to indicate the number of match and whales from each classes, when a male whale comes or returns, the program will print "one male whale come(return from match NO.#)" and then print out the number of male whales waiting to be matched, when female and matchmaker come or return, the program will print similar message. And when a match 
completed it will print "match NO.# completed"

    Test switch case numbers used:
         30: 9 whales in 3 pairs, 3 male, 3 female, 3 matchmaker, three matches succeed
         31: 8 whales , 3 male, 3 female, 2 matchmaker, only two matches succeed
         32: 7 whales , 3 male, 1 female, 3 matchmaker, only one match succeeds
         33: 9 whales in 3 pairs, 3 male, 3 female, 3 matchmaker, in a different order, three matches succeed
         
	 we also tried -rs while testing and our program works fine in random switch circumstance
         use -d w and -d x to see debug information

-------------------

## Nachos Installation Guide

### Install on a 64-bit Ubuntu 

Nachos requires 32-bit headers and libraries. To install these on
a Ubuntu 14.4 LTS 64-bit:

```
$ sudo apt-get install gcc-multilib g++-multilib lib32stdc++6
```