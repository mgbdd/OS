#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t childPid = fork();

	printf("parent pid: %d\n", getpid());
	printf("child pid: %d\n", childPid);

    if (childPid < 0)
    {
        printf("Failed to create child proc\n");
        return 1;
    } else if (childPid == 0) {
        printf("child proc\n");
        
	pid_t grandChildPid = fork();

	printf("grandchild pid: %d\n", grandChildPid);

	if(grandChildPid < 0)
	{
		printf("Failed to create child proc\n");
        	return 1;
	}
	else if(grandChildPid == 0)
	{
		printf("grandchild proc\n");
		sleep(20);
		printf("grandchild proc was finished\n");
	}
	else
	{
		exit(0);
	}
	

        printf("child proc finished\n");
    } else //parent 
    {
	int status;
	wait(&status);

	if(WIFEXITED(status))
	{
		printf("child proc is finished, code: %d\n", WEXITSTATUS(status));
	}
	else if(WIFSIGNALED(status))
	{
		printf("child proc was finished with a signal: %d\n", WTERMSIG(status));
	}
	else
	{
		printf("child proc is finished with error\n");
	}


        printf("parent proc\n");
        return 0;
    }

    return 0;
}
