#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "pcreate_lib.h"

int main()
{
    mythread_t mytid1;
    mythread_t mytid2;
    void *retval1;
    void *retval2;

    printf("main [%d %d %d]\n", getpid(), getppid(), gettid());
    if(mythread_create(&mytid1, mythread_function, "hiiii") != 0)
    {
        perror("Failed to create mythread");
        return -1;
    }
    if(mythread_create(&mytid2, mythread_function, "heyyy") != 0)
    {
        perror("Failed tocreate mythread");
        return -1;
    }

    mythread_join(mytid1, &retval1);
    mythread_join(mytid2, &retval2);
    printf("retval: %s\n", (char*)retval1);

    return 0;
}