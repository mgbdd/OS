#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thread_function(void* arg) {
    printf("hey, i'm a new thread with tid: %ld\n", pthread_self());
    return NULL; 
}


int main() {
    pthread_t thread; 
    void *return_value; 

    printf("hello from main thread\n");

    while(1)
    {
        if (pthread_create(&thread, NULL, thread_function, NULL) != 0) {
            perror("Failed to create thread");
            return -1;
        }
        //pthread_detach(thread);
    }
    
    
    return 0;
}
