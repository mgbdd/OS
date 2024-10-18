#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_function(void *arg)
{
    //int i = 0; // 1_4_b
    while(1)
    {
        printf("hello \n");  // 1_4_а 
        //sleep(1);
        //i++;                   // 1_4_b
        //pthread_testcancel();  // 1_4_b
    }
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
    //sleep(2);
    printf("After sleep in main thread\n");
    if (pthread_cancel(thread) != 0) {
        perror("Failed to cancel thread");
        return EXIT_FAILURE;
    }


    pthread_join(thread, NULL);
    printf("Thread is cancelled\n");
}



//  pthread_cancel - send a cancellation request to a thread
/*

pthread_exit() используется для завершения текущего потока. Она позволяет потоку завершить свою работу корректно, освобождая ресурсы и возвращая значение.
При вызове pthread_exit(), поток завершает выполнение и может вернуть значение, которое будет доступно другим потокам, которые его ожидают с помощью pthread_join()
При этом поток не завершает весь процесс, а только завершает себя. Другие потоки продолжают работу как обычно.





*/


// как был бы устроен асинхронный и отложенный
// почему с printf без sleep так долго работает
