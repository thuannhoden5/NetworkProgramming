#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket[MAX_CLIENTS];
    struct sockaddr_in server_address, client_address;
    int max_clients = MAX_CLIENTS;
    int activity, i, valread, sd;
    int max_sd;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    // Tạo socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("cannot create server");
        exit(EXIT_FAILURE);
    }

    // Thiết lập thông tin địa chỉ và cổng của server
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // Gán địa chỉ và cổng cho server socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("cannot assign address to server");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe các kết nối đến
    if (listen(server_socket, 3) < 0) {
        perror("fail when listen to connect");
        exit(EXIT_FAILURE);
    }

    printf("Telnet server ready\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

       
        if (FD_ISSET(server_socket, &readfds)) {
            int new_socket;
            int client_address_length = sizeof(client_address);
            if ((new_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_length)) < 0) {
                perror("Fail when new connect");
                exit(EXIT_FAILURE);
            }
            printf("New connected\n");

           
            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }
            }
        }

       
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                // Đọc dữ liệu từ client
                memset(buffer, 0, sizeof(buffer));
                valread = read(sd, buffer, sizeof(buffer));

                // Xử lý yêu cầu đăng nhập
                if (strstr(buffer, "user") != NULL && strstr(buffer, "pass") != NULL) {
                    char user[50];
                    char pass[50];
                    sscanf(buffer, "user %s pass %s", user, pass);

                    // So sánh với file cơ sở dữ liệu
                    FILE *database = fopen("database.txt", "r");
                    char line[100];
                    int found = 0;
                    while (fgets(line, sizeof(line), database) != NULL) {
                        char db_user[50];
                        char db_pass[50];
                        sscanf(line, "%s %s", db_user, db_pass);
                        if (strcmp(user, db_user) == 0 && strcmp(pass, db_pass) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    fclose(database);

                    // Gửi kết quả đăng nhập cho client
                    if (found) {
                        send(sd, "Login sucess\n", strlen("Login sucess\n"), 0);
                    } else {
                        send(sd, "Cannot login\n", strlen("Error login\n"), 0);
                    }
                }
                // Xử lý lệnh từ client
                else {

                   
                    FILE *output_file = fopen("out.txt", "w"); // Mở file out.txt để lưu kết quả lệnh
                    if (output_file == NULL) {
                        perror("Cannot open file");
                        exit(EXIT_FAILURE);
                    } else printf("Sucess open file\n");

                    
                    // Lưu kết quả vào file out.txt
                    snprintf(output_file, sizeof(buffer), "%s\n", buffer);

                    printf("Get the requirement!\n");


                   
                    send(sd, "Sent sucess\n", strlen("sent to success!\n"), 0);
                }
            }
        }
    }

return 0;
}