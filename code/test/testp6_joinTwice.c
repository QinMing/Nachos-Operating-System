#include "syscall.h"

int main(int argc, char *argv[])
{
	int result;
	int id;

	id = (int)argv[0][0] - 48;
	
	result = Join(id); //the exit status of process #2 is passed to result

	Exit(result);
}
