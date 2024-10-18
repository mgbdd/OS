#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* thread_function_1(void* arg) {
    printf("hey, i'm a new thread\n");
    int *result = malloc(sizeof(int)); 
    *result = 42;
    return result; 
}

void* thread_function_2(void* arg) {
    printf("hey, i'm a new thread\n");
    char *str = malloc(13 * sizeof(char));
    strcpy(str, "hello world");
    return (void*)str; 
}

int main() {
    pthread_t thread; 
    void *return_value; 

    printf("hello from main thread\n");

    if (pthread_create(&thread, NULL, thread_function_1, NULL) != 0) {
        perror("Failed to create thread");
        return -1;
    }  

    if (pthread_join(thread, &return_value) != 0) {
        perror("Failed to join thread");
        return -1;
    }

    printf("The value from thread function: %d\n", *(int*)return_value);
    //printf("The string from thread function: %s\n", (char*)return_value);
    free(return_value);
    return 0;
}












//return_value хранит адрес переменной статуса завершения потока, поточная функция возвращает этот адрес, поэтому надо выделить память под эту переменную на куче