
//This is a multithreaded user program

#include<syscall.h>
#define Length 8
char str[Length + 2];


//void nothing()
//{
//	//Write("+", 1, ConsoleOutput);
//	//Exit(456);
//	int i;
//	for (i = 0; i < 3; i++) {
//		char ch = i + 48;
//		Write(&ch, 1, ConsoleOutput);
//	}
//	//Exit(455);
//}
//
//int main() {
//	Fork(nothing);
//}

void ping()
{
	int i;
	for (i = 0; i < 5; i++) {
		str[1] = 'i';
		if (i < 10)
			str[5] = i + 48;
		else
			str[5] = '*';

		Write(str, Length, ConsoleOutput);
		Yield();
	}
	Exit(222);
}

void pong()
{
	int i;
	for (i = 0; i < 5; i++) {
		str[1] = 'o';
		if (i < 10)
			str[5] = i + 48;
		else
			str[5] = '*';

		Write(str, Length, ConsoleOutput);
		Yield();
	}
	Write("\n(Demo end)\n", 12, ConsoleOutput);
	Exit(333);
}


void main()
{
	str[0] = 'p';
	str[1] = '?';
	str[2] = 'n';
	str[3] = 'g';
	str[4] = '-';
	str[5] = '?';
	str[6] = ' ';
	str[7] = ' ';
	str[8] = '\0';

	Fork(pong);
	ping();
}