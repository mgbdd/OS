#include "pcreate_lib.h"


int thread_startup(void *arg)
{
    mythread_t thread = (mythread_t) arg;
    printf("thread_startup: starting to execute thread function for the thread %d\n", thread->mythread_id);
    thread->retval = thread->start_routine(thread->arg);
    thread->finished = 1;
    printf("thread_startup: waiting for join() the thread %d\n", thread->mythread_id);
    while(!thread->joined)
    {
        sleep(1);
    }
    printf("thread_startup: the thread function finished for the thread %d\n", thread->mythread_id);
    return 0;
}

void *create_stack(off_t size, int mythread_id)
{
    char stack_file[128];
    snprintf(stack_file, sizeof(stack_file), "stack-%d", mythread_id);
    int stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    ftruncate(stack_fd, 0); //гарантирует, что файл не содержит старых данных 
    ftruncate(stack_fd, size);

    void *stack = mmap(NULL, size, PROT_NONE, MAP_SHARED, stack_fd, 0);
    close(stack_fd);
    return stack;
}

int mythread_create(mythread_t *mytid, void *(*start_routine) (void*), void *arg)
{
    static int mythread_id = 0; 
    mythread_t thread;
    int child_pid;
    void *child_stack;

    mythread_id++;

    printf("mythread_create: creating thread %d\n", mythread_id);

    child_stack = create_stack(STACK_SIZE, mythread_id);
    mprotect(child_stack + PAGE, STACK_SIZE - PAGE, PROT_READ|PROT_WRITE); //отступаем защитную страничку
    memset(child_stack + PAGE, 0x7f, STACK_SIZE - PAGE);
    thread = (mythread_t)(child_stack + STACK_SIZE - sizeof(mythread_struct_t));
    thread->mythread_id = mythread_id;
    thread->arg = arg;
    thread->start_routine = start_routine;
    thread->retval = NULL;
    thread->joined = 0;
    thread->finished = 0;

    child_stack = (void*)thread;
    printf("child stack %p; mythread_struct %p; \n", child_stack, thread);

    child_pid = clone(thread_startup, child_stack, 
                  CLONE_VM | CLONE_FILES | CLONE_THREAD |  CLONE_SIGHAND, child_stack);
    if(child_pid == -1)
    {
        printf("Failed to clone: %s\n", strerror(errno));
        return -1;
    }
    *mytid = thread;
    return 0;
}

void mythread_join(mythread_t mytid, void **retval)
{
    mythread_t thread = mytid;
    printf("thread_join: waiting for the thread %d to finish\n", thread->mythread_id);
    while(!thread->finished)
    {
        usleep(1);
    }
    printf("thread_join: the thread %d finished\n", thread->mythread_id);
    *retval = thread->retval;
    thread->joined = 1;
}

void *mythread_function(void *arg)
{
    char *str = (char*)arg;
    for(int i = 0; i < 10; i++)
    {
        printf("hello from my thread '%s'\n", str);
        sleep(1);
    }
    return "bye";
}


//посмотреть можно ли чтото вместо usleep












//PROT_NONE -  выделенная область памяти не будет доступна для чтения или записи