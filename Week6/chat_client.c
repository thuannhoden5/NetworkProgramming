#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1" 
#define SERVER_PORT 8888 

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Không thể tạo socket.\n");
        exit(1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        printf("Cannot connect to server chat\n");
        exit(1);
    }

    printf("Connect to server success\n");

    char client_name[50];
    printf("Enter your name ");
    fgets(client_name, 50, stdin);

    char message[1000];
    while (1) {
        printf("Bạn: ");
        fgets(message, 1000, stdin);

        if (send(sock, message, strlen(message), 0) == -1) {
            printf("Cannot send message.\n");
            exit(1);
        }

        if (strcmp(message, "exit\n") == 0) {
            printf("Exit chat\n");
            break;
        }
    }

    close(sock);
    return 0;
}