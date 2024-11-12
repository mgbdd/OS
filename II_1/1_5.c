#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sigint_handler(int signo) {
    printf("Received SIGINT in thread %ld. Exiting\n", pthread_self());
    pthread_exit(0);

}

void* block_signals_thread(void* arg) {
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    while (1) {
        printf("Signal blocking thread %ld is running\n", pthread_self());
        sleep(2);
    }
    return NULL;
}

void* sigint_handler_thread(void* arg) {
    struct sigaction sa;
    sa.sa_handler = sigint_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    while (1) {
        printf("SIGINT handler thread %ld is running\n", pthread_self());
        sleep(2); 
    }
    return NULL;
}

void* sigquit_handler_thread(void* arg) {
    sigset_t mask;
    int sig;
    sigemptyset(&mask); 
    sigaddset(&mask, SIGQUIT);

    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    while (1) {
        printf("Waiting for SIGQUIT in thread %ld\n", pthread_self());
        sigwait(&mask, &sig);
        printf("Received SIGQUIT in thread %ld. Exiting\n", pthread_self());
        break; 
    }
    return NULL;
}

int is_blocked(int signo) {
    sigset_t current_mask;

    if (sigprocmask(SIG_BLOCK, NULL, &current_mask) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    return sigismember(&current_mask, signo);
}

void block_signal(int signo)
{
    sigset_t mask;
    sigemptyset(&mask); 
    sigaddset(&mask, signo);

    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    printf("\nSIGNAL № %d was blocked in main thread\n\n", signo);

}

int main() {

    printf("Main thread: %ld\n", pthread_self());

    if (is_blocked(SIGQUIT)) {
        printf("Signal SIGQUIT is blocked in the main thread.\n");
    } else {
        printf("Signal SIGQUIT is NOT blocked in the main thread.\n");
        block_signal(SIGQUIT);
    }

    /*if (is_blocked(SIGINT)) {
        printf("Signal SIGINT is blocked in the main thread.\n");
    }  else {
        printf("Signal SIGINT is NOT blocked in the main thread.\n");
        block_signal(SIGINT);
    }*/


    pthread_t tid1, tid2, tid3;

    pthread_create(&tid1, NULL, block_signals_thread, NULL);
    pthread_create(&tid2, NULL, sigint_handler_thread, NULL);
    pthread_create(&tid3, NULL, sigquit_handler_thread, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    return 0;
}


//SIGINT что-то странное, он должен видимо останавливать только 1 поток, а останавливает главный тоже

