#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1" // địa chỉ IP của máy chủ chat
#define SERVER_PORT 8888 // cổng mặc định của máy chủ chat

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
        printf("Cannot connect to server\n");
        exit(1);
    }

    printf("Connect success\n");

    char client_name[50];
    printf("Enter your name ");
    fgets(client_name, 50, stdin);

    char message[1000];
    while (1) {
        printf("Your: ");
        fgets(message, 1000, stdin);

        if (send(sock, message, strlen(message), 0) == -1) {
            printf("Cannot send message to server\n");
            exit(1);
        }

        if (strcmp(message, "exit\n") == 0) {
            printf("Leave room.\n");
            break;
        }
    }

    close(sock);
    return 0;
}