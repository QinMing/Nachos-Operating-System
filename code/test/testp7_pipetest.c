/* 
 * pipetest.c
 *
 *	Simple program to test pipe system call.  Note that
 * you will have to write "produce" and "consume".  Produce writes
 * output and consume reads it.
 */

#include <syscall.h>


int main()
{
	OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
	
    Exec("../test/testp7_pipetest_produce", 0, 0, 2);
	Exec("../test/testp7_pipetest_mid", 0, 0, 6);
	Exec("../test/testp7_pipetest_mid", 0, 0, 6);
	SpaceId pid = Exec("../test/testp7_pipetest_consume", 0, 0, 0x4|0x01);
	Yield(); 
	int i;
	i = 2;
	Yield();
	Write("Enter a letter or a digit >> ", 29, output);
	
	int result;
	result=Join(pid);

	return result;
}
