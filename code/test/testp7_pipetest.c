/* 
 * pipetest.c
 *
 *	Simple program to test pipe system call.  Note that
 * you will have to write "produce" and "consume".  Produce writes
 * output and consume reads it.
 */

#include <syscall.h>

int
main()
{
	OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
	
    Exec("../test/testp7_pipetest_produce", 0, 0, 2);
	Exec("../test/testp7_pipetest_mid", 0, 0, 6);
	Exec("../test/testp7_pipetest_mid", 0, 0, 6);
    SpaceId pid = Exec("../test/testp7_pipetest_consume", 0, 0, 4);
	
	Write("Enter a letter. For example: B<enter>. >>    ", 45, output);
	//do{//dead loop
	//	pid=1;
	//}while(pid==1);

	char debug;
	debug = (char) pid + 48;
	Write(&debug,sizeof(int),output);
	Join(pid);

	//Yield();
	return 0;
}