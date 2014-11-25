
#include <syscall.h>

int main(){
	char buffer[60];
    int i;
	
    i = 0;
    do {
        Read(&buffer[i], 1, ConsoleInput);
    } while (buffer[i++] != '\0');
	
	
	Write("Results: ",9,ConsoleOutput);
	Write(buffer, i-1, ConsoleOutput);
	return 0;
}