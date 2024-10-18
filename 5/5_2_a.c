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
    printf("local address: %p\n", (void*)&local);
    printf("local value: %i\n", local);

    printf("global address: %p\n", (void*)&global);
    printf("global value: %i\n\n", global);

    pid_t pid2 = fork();
    if(pid2 < 0)
    {
        printf("Failed to create a process\n");
        return 1;
    }
    else if(pid2 == 0) // Дочерний процесс
    {
        printf("Second pid: %d\n", pid2);
        printf("Parent pid: %d\n\n", pid);

        printf("Local address from child proc: %p\n", (void*)&local);
        printf("Local value from child proc: %i\n", local);

        printf("Global address from child proc: %p\n", (void*)&global);
        printf("Global value from child proc: %i\n\n", global);

        local = 4;
        global = 10;

        printf("Local value from child proc: %i\n", local);
        printf("Global value from child proc: %i\n\n", global);

        exit(5);
    }
    else
    {
        printf("Child pid from parent pid: %d\n", pid2);

        printf("Local value from parent proc: %i\n", local);
        printf("Global value from parent proc: %i\n", global);

     
        sleep(30);

       

        return 0;
    }
}
