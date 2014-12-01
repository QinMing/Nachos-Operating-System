
#include <syscall.h>

int main(){
    char buffer[60];

    Read(&buffer[0], 1, ConsoleInput);
	buffer[1]=buffer[0]+1;
	buffer[2]='\0';
	Write(buffer, 3, ConsoleOutput);

	return 0;
}