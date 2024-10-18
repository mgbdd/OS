#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define SERVER_PORT 8888

int main() {
    int server_fd; //дескриптор сокета сервера 
    int client_len; //размер структуры адреса клиента
    struct sockaddr_in server_addr, client_addr; //структуры дя хранения адерсов сервера и клиента
    char buffer[BUFFER_SIZE];

    // Создание UDP-сокета
    server_fd = socket(AF_INET, SOCK_DGRAM, 0); 
    //af_inet указывает на семейство адресов, работающее с ipv4, sock_dgram - датаграмный сокет
    //датаграмные сокеты работают без установления соединения между отправитлелем и получателем 
    if (server_fd < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Заполнение структуры адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;//семейство ipv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // устанавливаем ip адрес сервера, INADDR_ANY - любой доступный адрес на данном хосте 
    server_addr.sin_port = htons(SERVER_PORT);

    // Связывание сокета с адресом и портом
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка связывания");
        exit(EXIT_FAILURE);
    }

    printf("UDP-сервер запущен на порту %d\n", SERVER_PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        client_len = sizeof(client_addr);

        // Получение данных от клиента                               полуаем адрес клиента для последующей отправки ответа
        int recv_len = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) {
            perror("Ошибка приема данных");
            continue;
        }

        printf("Получено от клиента: %s\n", buffer);

        // Отправка данных обратно клиенту
        sendto(server_fd, buffer, recv_len, 0, (struct sockaddr *)&client_addr, client_len);
    }

    close(server_fd);
    return 0;
}
