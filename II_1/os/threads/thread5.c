#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#define NUM 5

int global = 1;

void *thread_func(void* arg) {

	int local = 2;
	static int static_local = 3;
	const int const_local = 4; 

	//printf("mythread [%d %d %d %ld]: Hello from mythread!\n", getpid(), getppid(), gettid(), pthread_self());	
	printf("mythread [%d %d %d %ld]: Hello from mythread!\nlocal: %p, static local: %p, const local: %p, global: %p\n", getpid(), getppid(), gettid(), pthread_self(), &local, &static_local, &const_local, &global);	
	sleep(5); //для пункта 1.1.е
	local = 10 + gettid();
	global = 20 + gettid();
	printf("addresses of local(%d) and global(%d) variables after changing in thread with tid%ld : %p, %p\n\n", local, global, pthread_self(), &local, &global);

	

	return NULL;
}

int main() {
	pthread_t tids[NUM]; //идентификатор потока
	int err;
                                                 //pid      parent pid   идентификатор текущего потока              
	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	sleep(5); //для пункта 1.1.е

	for(int i = 0; i < NUM; i++)
    {
        err = pthread_create(&tids[i], NULL, thread_func, NULL);
        printf("thread №%d, tid: %ld\n", i, tids[i]);
	}
    sleep(15);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	printf("global variable from main: %d\n", global);
	return 0;
}

