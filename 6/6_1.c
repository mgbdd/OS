#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>

sem_t barrier;
sem_t access_control; // Дополнительный счетный семафор для контроля доступа к барьеру

int main() {
    const int PAGE_SIZE = getpagesize();
    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    unsigned int* shared_mem = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    // Инициализация барьера и семафора доступа
    sem_init(&barrier, 1, 0);
    sem_init(&access_control, 1, 1);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Процесс-писатель
        unsigned int value = 0;
        while (1) {
            sem_wait(&access_control); // Захват семафора доступа
            for (int i = 0; i < PAGE_SIZE / sizeof(unsigned int); i++) {
                shared_mem[i] = value++;
                if (value == UINT_MAX) {
                    value = 0;
                }
            }
            sem_post(&barrier); // Разрешение барьера
            sem_post(&access_control); // Освобождение семафора доступа
            sem_wait(&barrier); // Ожидание барьера
        }
    } else {
        // Процесс-читатель
        unsigned int expected_value = 0;
        while (1) {
            sem_wait(&access_control); // Захват семафора доступа
            sem_wait(&barrier); // Ожидание барьера
            for (int i = 0; i < PAGE_SIZE / sizeof(unsigned int); i++) {
                if (shared_mem[i] != expected_value) {
                    printf("Ошибка последовательности: ожидалось %u, получено %u\n", expected_value, shared_mem[i]);
                    exit(EXIT_FAILURE);
                }
                expected_value++;
                if (expected_value == UINT_MAX) {
                    expected_value = 0;
                }
            }
            sem_post(&barrier); // Разрешение барьера
            sem_post(&access_control); // Освобождение семафора доступа
        }
    }

    return 0;
}
