#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "chat.h"

#define BUFFER_SIZE 1024

SSL_CTX *ssl_ctx;
SSL *ssl;

void *receive_messages(void *unused) {
    char buffer[BUFFER_SIZE];
    int len;
    FILE *log_file = fopen("chat_log.txt", "a");

    while ((len = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[len] = '\0';
        printf("\n>> %s\n", buffer);
        fprintf(log_file, ">> %s\n", buffer);
        fflush(log_file);
        printf("You: ");
        fflush(stdout);
    }

    fclose(log_file);
    printf("\nRozmówca się rozłączył.\n");
    exit(0);
}

void *send_messages(void *unused) {
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + MAX_NICK + 5];

    while (1) {
        printf("You: ");
        fflush(stdout);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "/quit") == 0) {
            printf("Rozłączanie...\n");
            SSL_shutdown(ssl);
            SSL_free(ssl);
            SSL_CTX_free(ssl_ctx);
            exit(0);
        }

        snprintf(message, sizeof(message), "[%s] %s", user_nick, buffer);
        SSL_write(ssl, message, strlen(message));
    }
}

void chat_session() {
    pthread_t recv_thread, send_thread;

    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_create(&send_thread, NULL, send_messages, NULL);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);
}

void init_openssl() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
}

SSL_CTX *create_context(int is_server) {
    const SSL_METHOD *method = is_server ? TLS_server_method() : TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Nie można utworzyć kontekstu TLS");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

void tls_connect_client(int sock) {
    init_openssl();
    ssl_ctx = create_context(0);
    ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    printf("Połączono bezpiecznie (TLS)\n");
}

void tls_accept_server(int sock) {
    init_openssl();
    ssl_ctx = create_context(1);

    if (SSL_CTX_use_certificate_file(ssl_ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ssl_ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    printf("Ustanowiono bezpieczne połączenie TLS\n");
}
