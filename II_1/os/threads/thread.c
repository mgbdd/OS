#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
	printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());	
	return NULL;
}

int main() {
	pthread_t tid; //идентификатор потока
	int err;
                                                 //pid      parent pid   идентификатор текущего потока              
	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
	if (pthread_join(tid, NULL) != 0) {
        perror("Failed to join thread");
        return -1;
    }
	return 0;
}

