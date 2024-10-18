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
    return NULL;
}

int main()
{
    MyStruct example;
    example.a = 4;
    example.str = "hi hi hi";

    pthread_t thread;
    if(pthread_create(&thread, NULL, thread_function, (void*)&example) != 0)
    {
        perror("Failed to create a thread");
        return -1;
    }
    if (pthread_join(thread, NULL) != 0) {
        perror("Failed to join thread");
        return -1;
    }
    return 0;
}