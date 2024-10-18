#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8889
#define MAX_CLIENTS 1
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t recv_size;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv_size = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

        if (recv_size == 0) {
            // Клиент закрыл соединение
            printf("Клиент закрыл соединение\n");
            close(client_socket);
            break;
        } else if (recv_size < 0) {
            perror("Ошибка чтения из сокета");
            close(client_socket);
            break;
        }

        printf("Получено от клиента: %s\n", buffer);
        send(client_socket, buffer, strlen(buffer), 0);
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    pid_t child_pid;

    // Создание TCP-сокета
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Заполнение структуры адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);//присваиваем ip адрес
    server_addr.sin_port = htons(SERVER_PORT);

    // Привязка сокета к адресу
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка привязки сокета");
        exit(EXIT_FAILURE);
    }

    // Прослушивание входящих соединений
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Ошибка прослушивания");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на %s:%d\n", SERVER_IP, SERVER_PORT);

    while (1) {
        client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_socket < 0) {
            perror("Ошибка принятия соединения");
            continue;
        }

        printf("Новое соединение от %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        child_pid = fork();
        if (child_pid == 0) {
            // Дочерний процесс
            close(server_socket); // Закрываем серверный сокет в дочернем процессе
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else if (child_pid > 0) {
            // Родительский процесс
	    printf("Процесс клиента: %d\n", child_pid);
            close(client_socket); // Закрываем клиентский сокет в родительском процессе
        } else {
            perror("Ошибка создания дочернего процесса");
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}
