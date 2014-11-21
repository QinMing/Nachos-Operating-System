/*
 * echo.c
 *
 * Echo lines of input to the output.  Terminate on a ".".
 */

#include <syscall.h>

int
main ()
{
    char buffer[80];
    char prompt[3];
    int i;

    prompt[0] = '-';
    prompt[1] = '>';
    prompt[2] = ' ';

    while (1) {
       // print the prompt
       Write(prompt, 3, ConsoleOutput);

       // read the input terminated by a newline
       i = 0;
       do {
	   Read(&buffer[i], 1, ConsoleInput);
       } while (buffer[i++] != '\n');
       buffer[i] = '\0';

       // if the input is just a period, then exit
       if (buffer[0] == '.' &&
	   buffer[1] == '\n') {
	   return 0;
       }

       // echo the input to the output
       Write(buffer, i, ConsoleOutput);
   }
}
