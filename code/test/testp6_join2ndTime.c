#include "syscall.h"

int main()
{
	int result;
	//int id;
	char* argv[1];

	Exec("../test/array", 0, 0, 1);
	argv[0] = "2";
	Exec("../test/testp6_joinTwice", 1, argv, 0);
	result = Join(2); //the exit status of process #2 is passed to result

	Exit(result);
}
