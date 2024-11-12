#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/mman.h> 
#include <sched.h> 
#define PAGE 4096
#define STACK_SIZE 3*PAGE

typedef  void *(*start_routine_t) (void *);

 typedef struct mythread{
    int mythread_id; 
    start_routine_t start_routine;
    int finished;
    int joined;
    void *arg;
    void *retval;
    
}mythread_struct_t;

typedef mythread_struct_t* mythread_t;

int thread_startup(void *arg);
void *create_stack(off_t size, int mythread_id);
int mythread_create(mythread_t *thread, void *(*start_routine) (void*), void *arg);
void mythread_join(mythread_t mytid, void **retval);
void *mythread_function(void *arg);
