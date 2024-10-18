#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thread_function(void* arg) {
    printf("hey, i'm a new thread with tid: %ld\n", pthread_self());
    return NULL; 
}

int main() {
    pthread_t thread; 
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    printf("hello from main thread\n");

    while (1) {
        if (pthread_create(&thread, &attr, thread_function, NULL) != 0) {
            perror("Failed to create thread");
            return -1;
        }
        
    }

    pthread_attr_destroy(&attr); 

    return 0;
}
