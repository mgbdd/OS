#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFFER_SIZE 4096*1024

int iteration = 0;

void function()
{
	printf("iteration: %i\n", iteration);
	iteration++;
	char arr[4096 * 128];
	sleep(1);
	function();
}

int main()
{
	pid_t pid = getpid();
	printf("pid: %d\n", pid);
	sleep(10);

	//function();

	while (1) {
        // Выделяем память на куче
        void* buffer = malloc(BUFFER_SIZE);
	printf("%d bytes was allocated\n", BUFFER_SIZE);
        if (buffer == NULL) 
	{
            fprintf(stderr, "Failed to allocate memory\n");
            return 1;
       	}
	
	printf("sleep before free\n");
	sleep(5);
        free(buffer);
	printf("buffer was freed\n");
        sleep(1);
    }
}
