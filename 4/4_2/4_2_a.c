#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	pid_t pid = getpid();
	printf("pid: %d\n", pid);
	sleep(1);
	execvp("./1", (char *[]){"./1", NULL});
	printf("Hello world\n");
	
}
