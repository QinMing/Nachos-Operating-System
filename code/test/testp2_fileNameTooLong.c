#include "syscall.h"
int main()
{
	int result;
	int i;
	char str[264];// ="../test/array";
	for(i=0;i<=256;i=i+8){
	   str[i]='.';
	   str[i+1]='.';
	   str[i+2]='/';
	   str[i+3]='t';
	   str[i+4]='e';
	   str[i+5]='s';
	   str[i+6]='t';
	   str[i+7]='/';
	}
	i=i-8;
	str[i]='a';
	str[i+1]='r';
	str[i+2]='r';
	str[i+3]='a';
	str[i+4]='y';
	str[i+5]='\0';
	result=Exec(str,0,0,0);
	Exit(result);
}
