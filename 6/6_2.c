#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

int main() {
    int pipefd[2];
    // pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // процесс-писатель
        close(pipefd[0]); // читающий конец pipe
        unsigned int value = 0;
        while (1) {
            if (write(pipefd[1], &value, sizeof(unsigned int)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            if (value == UINT_MAX) {
                value = 0;
            } else {
                value++;
            }
        }
        close(pipefd[1]); // закрываем пишущий конец pipe
    } else {
        // процесс-читатель
        close(pipefd[1]); // закрываем пишущий конец pipe
        unsigned int expected_value = 0;
        unsigned int curr_value;

        while (1) {
            ssize_t bytes_read = read(pipefd[0], &curr_value, sizeof(unsigned int));
            if (bytes_read == 0) {
                // писатель закрыл свой конец pipe, данных больше не будет
                break;
            } else if (bytes_read == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            if (curr_value != expected_value) {
                printf("Ошибка последовательности: ожидалось %u, получено %u\n", expected_value, curr_value);
                exit(EXIT_FAILURE);
            }

            expected_value++;
            if (expected_value == UINT_MAX) {
                expected_value = 0;
            }
        }
        close(pipefd[0]); // закрываем читающий конец pipe
    }

    return 0;
}
