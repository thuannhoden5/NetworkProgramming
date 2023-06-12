#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080       
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];


    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Fail create socket");
        exit(EXIT_FAILURE);
    }

   
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);


    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Fail connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected server. Enter username and password\n");

    // Nhập tên người dùng và mật khẩu từ bàn phím
    char username[20];
    char password[20];
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;  // Xóa ký tự newline từ chuỗi
    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;  // Xóa ký tự newline từ chuỗi

    // Gửi tên người dùng và mật khẩu tới server
    snprintf(buffer, sizeof(buffer), "user %s pass %s", username, password);
    send(client_socket, buffer, strlen(buffer), 0);


    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Response from server: %s\n", buffer);

 
    if (strcmp(buffer, "Login success\n") == 0) {
       
        printf("Enter code ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;  
        send(client_socket, buffer, strlen(buffer), 0);

       
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Result: %s\n", buffer);
    }

 
    close(client_socket);

    return 0;
}