#include "syscall.h"
int main()
{
	int result;
	int i;
	char str[13];
	
	/*char *background[300];
	for (i = 0; i < 300; i++){
		background[i] = '3';
	}
	str = &background[10];
	*/

	str[0] = '.';
	str[1] = '.';
	str[2] = '/';
	str[3] = 't';
	str[4] = 'e';
	str[5] = 's';
	str[6] = 't';
	str[7] = '/';
	str[8] = 'a';
	str[9] = 'r';
	str[10] = 'r';
	str[11] = 'a';
	str[12] = 'y';

	result=Exec(str,0,0,0);
	Exit(result);
}
