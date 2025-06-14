#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "network.h"
#include "chat.h"

void start_server(int port) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    printf("Serwer nasłuchuje na porcie %d...\n", port);

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    printf("Połączono z %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    tls_accept_server(client_fd); 

    chat_session();  
}

int connect_to_peer(char *ip, int port) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Błąd połączenia");
        exit(1);
    }

    printf("Połączono z %s:%d\n", ip, port);


    tls_connect_client(sock); 
    chat_session();  
}
