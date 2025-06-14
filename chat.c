#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

void *receive_messages(void *socket_desc) {
    int sock = *(int *)socket_desc;
    char buffer[BUFFER_SIZE];
    int len;

    while ((len = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[len] = '\0';
        printf("\n>> %s\n", buffer);
        printf("You: ");
        fflush(stdout);
    }

    printf("\nRozmówca się rozłączył.\n");
    exit(0);
}

void *send_messages(void *socket_desc) {
    int sock = *(int *)socket_desc;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("You: ");
        fflush(stdout);
        fgets(buffer, sizeof(buffer), stdin);
        send(sock, buffer, strlen(buffer), 0);
    }
}

void chat_session(int sock) {
    pthread_t recv_thread, send_thread;

    pthread_create(&recv_thread, NULL, receive_messages, &sock);
    pthread_create(&send_thread, NULL, send_messages, &sock);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);
}

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

    chat_session(client_fd);
}

void connect_to_peer(char *ip, int port) {
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
    chat_session(sock);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // tryb serwera
        start_server(5000);
    } else if (argc == 3) {
        // tryb klienta
        connect_to_peer(argv[1], atoi(argv[2]));
    } else {
        printf("Użycie:\n");
        printf("  %s               # uruchom jako serwer (port 5000)\n", argv[0]);
        printf("  %s IP PORT       # połącz się jako klient\n", argv[0]);
        return 1;
    }

    return 0;
}

