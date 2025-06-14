#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "chat.h"

char user_nick[MAX_NICK];  

int main(int argc, char *argv[]) {
    printf("Podaj swój nick: ");
    fgets(user_nick, sizeof(user_nick), stdin);
    user_nick[strcspn(user_nick, "\n")] = '\0'; 

    if (argc == 1) {
        // tryb serwera
        start_server(5000);
    } else if (argc == 3) {
        // tryb klienta
        connect_to_peer(argv[1], atoi(argv[2]));
    } else {
        printf("Użycie:\n");
        printf("  %s               # uruchom jako serwer\n", argv[0]);
        printf("  %s IP PORT       # połącz się jako klient\n", argv[0]);
        return 1;
    }

    return 0;
}
