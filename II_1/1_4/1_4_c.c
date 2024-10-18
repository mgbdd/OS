#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void cleanup(void* arg) {
    char* str = (char*)arg;
    printf("Freeing memory for string '%s'\n", str);
    free(str); 
}

void *thread_function(void *arg)
{
    char *str = malloc(12);
    if(str == NULL)
    {
        perror("Failed tp allocate the memory\n");
        return NULL;
    }
    snprintf(str, 12, "hello world");
    pthread_cleanup_push(cleanup, str);
    while(1)
    {
        printf("%s\n", str);
        sleep(1);
    }
    
    pthread_cleanup_pop(1); 
    return NULL;
}

int main()
{
    pthread_t thread;

    if(pthread_create(&thread, NULL, thread_function, NULL) != 0)
    {
        perror("Failed to create a thread");
        return -1;
    }
    sleep(2);
    printf("After sleep in main thread\n");
    if (pthread_cancel(thread) != 0) {
        perror("Failed to cancel thread");
        return EXIT_FAILURE;
    }

    pthread_join(thread, NULL);
    printf("Thread is cancelled\n");
}
