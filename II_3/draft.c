#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define BUFFER_SIZE 8192
#define PORT 80

int proxy_socket;

void sigint_handler(int sig) 
{
    printf("\nSIGINT received\n");
    close(proxy_socket); 
    exit(0);
}


int get_host(char *request, char *resolved_host, int before_host_flag) {
    char *host_start;
    const char *host_end;
    if (before_host_flag == 0) 
    {
        host_start = strstr(request, "http://");
        if (host_start == NULL) 
        {
            return -1;
        }
        host_start += strlen("http://");
        host_end = strchr(host_start, '/');
        if(host_end == NULL) {
             host_end = strchr(host_start, ' ');
        }

    } else {
        host_start = strstr(request, "Host:");
        if (host_start == NULL) {
            return -1;
        }
        host_start += strlen("Host:");


        while (*host_start == ' ') {
            host_start++;
        }

        host_end = strpbrk(host_start, " \r\n");
    }

    if (host_end == NULL) {

        host_end = host_start + strlen(host_start);
    }

    size_t host_length = host_end - host_start;

    if (host_length >= BUFFER_SIZE)
    {
        return -1;
    }    

    if (host_length <= 0) {
        return -1;
    }

    strncpy(resolved_host, host_start, host_length);
    resolved_host[host_length] = '\0';
    return 0;
}

void *handle_client(void *arg) {
    int client_socket = *(int*)arg;
    char buffer[BUFFER_SIZE], host[BUFFER_SIZE];
    int bytes_read, bytes_sent;
    int target_socket; 
    struct sockaddr_in target_addr; 
    struct hostent *he; // хранит инфу о хосте 

    bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0); // получаем http запрос от клиента
    if (bytes_read < 0) 
    {
        perror("Error reading client request");
        close(client_socket);
        return NULL;
    }
    //printf("%s\n", buffer);
    
    if (get_host(buffer, host, 0) != 0) 
    {
        if (get_host(buffer, host, 1) != 0) 
        {
            close(client_socket);
            return NULL;
        }
    }
    printf("Host: %s\n", host);
    
    he = gethostbyname(host);
    if (he == NULL) {
        herror("gethostbyname");
        close(client_socket);
        return NULL;
    }
    
    target_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (target_socket < 0) 
    {
        perror("Error creating server socket");
        close(client_socket);
        return NULL;
    }
        
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(PORT);
    memcpy(&target_addr.sin_addr, he->h_addr_list[0], he->h_length);


    if (connect(target_socket, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) 
    {
        perror("Error connecting to target server");
        close(client_socket);
        close(target_socket);
        return NULL;
    }

    send(target_socket, buffer, bytes_read, 0); 

    fd_set readfds; 
    int max_fd = (client_socket > target_socket) ? client_socket : target_socket;
    int client_closed = 0, server_closed =0;


    while (!client_closed && !server_closed) 
    {
        FD_ZERO(&readfds); 
        if (!client_closed) FD_SET(client_socket, &readfds);
        if (!server_closed) FD_SET(target_socket, &readfds);

        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        bytes_read = 0;



        //обработка данных от клиента
        if (!client_closed && FD_ISSET(client_socket, &readfds)) 
        {
            bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_read == 0)
            {
                client_closed = 1;
                //printf("Client closed connection\n");
                perror("Client closed connection\n");
                continue; 
            }
            else if (bytes_read == -1)
            {
                client_closed = 1;
                //printf("Error reading from client\n");
                perror("Error reading from client\n");
                continue; 
            }

            bytes_sent = 0;
            int total_sent = 0;

            while(total_sent < bytes_read)
            { 
                bytes_sent = send(target_socket, buffer+total_sent, bytes_read-total_sent , 0); 
                if (bytes_sent <= 0)
                {
                    server_closed = 1;
                    perror("Error sending to server");
                    break;
                }
                total_sent += bytes_sent;
            }
        }
        //обработка данных от сервера
        if (!server_closed && FD_ISSET(target_socket, &readfds)) 
        {
            bytes_read = 0;
            bytes_read = recv(target_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_read == 0)
            { 
                server_closed = 1;
                //printf("Server closed connection\n");
                perror("Server closed connection\n");
                continue; 
            }
            else if (bytes_read == -1)
            { 
                server_closed = 1;
                //printf("Error reading from server\n");
                perror("Error reading from server\n");
                continue; 
            }
            bytes_sent = 0;
            int total_sent = 0;
            while(total_sent < bytes_read)
            { 
                bytes_sent = send(client_socket, buffer+total_sent, bytes_read - total_sent, 0); 
                if (bytes_sent <= 0)
                {
                    client_closed = 1;
                    perror("Error sending to client");
                    break;
                }
                total_sent += bytes_sent;
            }
        }  
    }

    close(client_socket);
    close(target_socket);
    return NULL;

}

int main() {
    signal(SIGINT, sigint_handler);

    int client_socket; 
    struct sockaddr_in proxy_addr;
    pthread_t thread_id;

    proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxy_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET; //ipv4
    proxy_addr.sin_addr.s_addr = INADDR_ANY;
    proxy_addr.sin_port = htons(PORT);

    if (bind(proxy_socket, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0) 
    {
        perror("Error binding socket");
        close(proxy_socket);
        exit(1);
    }

    if (listen(proxy_socket, SOMAXCONN) == -1) 
    {
        perror("Error listening for connections");
        close(proxy_socket);
        exit(1);
    }

    printf("HTTP proxy server listening on port 80\n");

    while (1) 
    {
        int client_socket; 
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        client_socket = accept(proxy_socket, (struct sockaddr *)&client_addr, &client_addr_len); // принимаем соединение от клиента
        if (client_socket < 0) 
        {
            perror("Error accepting connection");
            continue;
        }

        if (pthread_create(&thread_id, NULL, handle_client, (void *)&client_socket) < 0) 
        {
            perror("Error creating thread");
            close(client_socket);
            continue;
        }
        pthread_detach(thread_id);
    }

    close(proxy_socket);
    return 0;
}