#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct MyStruct MyStruct;
struct MyStruct{
    int a;
    char *str;
};

void *thread_function(void *arg)
{
    MyStruct *exe = (MyStruct*)arg;
    printf("int value: %d, string: %s\n", exe->a, exe->str);
    free(exe);
    return NULL;
}

int main()
{
    MyStruct *example = malloc(sizeof(MyStruct));
    example->a = 4;
    example->str = "hi hi hi";

    pthread_t thread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if(pthread_create(&thread, &attr, thread_function, (void*)example) != 0)
    {
        perror("Failed to create a thread");
        free(example);
        return -1;
    }

    printf("hello from main thread\n");
    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}


// что за сегмент между стеками без прав 
/*
это какой-то разделитель
что будет если стек будет расти, что будет если залезем в жту область памяти или на чужой стек

*/