
#include <syscall.h>

int main(){
	char buffer[60];
    int i;
	
    i = 0;
    do {
        Read(&buffer[i], 1, ConsoleInput);
    } while (buffer[i++] != '\0');
	//now  buffer[i-1]=='\0'
	buffer[i]='\n';
	buffer[i-1]=buffer[i-2]+1;
	
	Write("Results: ",9,ConsoleOutput);
	Write(buffer, i+1, ConsoleOutput);
	Write("(Demo end)\n", 11, ConsoleOutput);
}