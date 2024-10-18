#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define SERVER_PORT 8888

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if (argc < 2) {
        printf("Сообщение не введено\n");
        exit(EXIT_FAILURE);
    }

    // Создание UDP-сокета
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Заполнение структуры адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);

    // Отправка данных серверу
    int send_len = sendto(client_fd, argv[1], strlen(argv[1]), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (send_len < 0) {
        perror("Ошибка отправки данных");
        exit(EXIT_FAILURE);
    }

    memset(buffer, 0, BUFFER_SIZE);
    int recv_len = recvfrom(client_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (recv_len < 0) {
        perror("Ошибка приема данных");
        exit(EXIT_FAILURE);
    }

    printf("Ответ от сервера: %s\n", buffer);

    close(client_fd);
    return 0;
}
