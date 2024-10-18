#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int global = 6;

int main()
{
	pid_t pid;
	pid = getpid();
	printf("pid: %d\n\n", pid);

        int local = 3;
	printf("local address: %p\n", &local);
	printf("local value: %i\n", local);

	printf("global address: %p\n", &global);
	printf("global value: %i\n\n", global);
	
	
	pid_t pid2 = fork();
	if(pid2 < 0)
	{
		printf("Failed to crate a process\n");
		return 1;
	}
	else if(pid2 == 0) //дочерний процесс
	{
		
		char mapsPath[100];
		snprintf(mapsPath, sizeof(mapsPath), "/proc/%d/status", getpid());
       		FILE *mapsFile = fopen(mapsPath, "r");

		if(mapsFile == NULL)
		{
			printf("File opening error\n");
			return 0;
		}

		char *line = malloc(256);
		while(fgets(line, sizeof(line), mapsFile) != NULL)
		{
			printf("%s", line);
		}
		fclose(mapsFile);

	 	printf("second pid: %d\n", pid2);
		printf("parent pid: %d\n\n", pid);
		
		printf("local address from child proc: %p\n", &local);
		printf("local value from child proc: %i\n", local);

		printf("global address from child proc: %p\n", &global);
		printf("global value from child proc: %i\n\n", global);

		/*char mapsPath1[100];
		snprintf(mapsPath1, sizeof(mapsPath1), "/proc/%d/maps", getpid());
       		FILE *mapsFile1 = fopen(mapsPath1, "r");

		if(mapsFile1 == NULL)
		{
			printf("File opening error\n");
			return 0;
		}

		char *line1 = malloc(256);
		while(fgets(line1, sizeof(line1), mapsFile1) != NULL)
		{
			printf("%s", line1);
		}
		fclose(mapsFile1);*/


		local = 4;
		global = 10;


		/*char mapsPath2[100];
		snprintf(mapsPath2, sizeof(mapsPath2), "/proc/%d/maps", getpid());
       		FILE *mapsFile2 = fopen(mapsPath2, "r");

		if(mapsFile2 == NULL)
		{
			printf("File opening error\n");
			return 0;
		}

		char *line2 = malloc(256);
		while(fgets(line2, sizeof(line2), mapsFile2) != NULL)
		{
			printf("%s", line2);
		}
		fclose(mapsFile2);*/


		printf("local value from child proc: %i\n", local);
		printf("global value from child proc: %i\n\n", global);
		exit(5);
		
		}
	else
	{
		printf("child pid from parent pid: %d\n", pid2);

		printf("local value from parent proc: %i\n", local);
		printf("global value from parent proc: %i\n", global);
		sleep(30);


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
	}

	return 0;
}
