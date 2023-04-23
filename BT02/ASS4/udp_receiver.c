#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9999
#define MAX_SIZE 2048
#define LOG_DIR "./received_dir/"

/** @function: VerifyPort()
* @brief: Checking port
* @param cmd_port: Command line argument for port
*
* @return: Port number if valid.
*          -1 if invalid       
**/
int VerifyPort (const char * cmd_port)
{
    char buff[10];
    strcpy(buff, cmd_port);
    for(unsigned int iBuff = 0; iBuff < strlen(buff); ++iBuff) {
        int tmp = buff[iBuff] - '0';
        if (tmp < 0 || tmp > 9) {       // Number only
            return -1;
        }
    }
    int port = atoi(cmd_port);
    if(port < 1 || port > 65535) {      // Port range
        return -1;
    }

    return port;
}

int main(int argc, char * argv[])
{
    // Verifying Command
    if (argc != 2) {
        puts("[**ERROR]: Invalid Command");
        puts("[USAGE]: ./udp_receiver port");
        exit(1);
    }

    // Verifying Port
    int port = VerifyPort(argv[1]);
    if (port == -1) {
        puts("[**ERROR]: Invalid Port.");
        exit(1);
    }

    // Initializing socket
    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  
    // Updating address info
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    // Binding socket
    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("[**ERROR] - bind()");
        exit(1);
    }
    printf("Listening on port %d\n", port);
    
    // Receiving datagram
    char buff[MAX_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    while (1) {
        int recv_byte = recvfrom(sock_fd, buff, sizeof(buff), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (recv_byte == -1) {
            puts("recvfrom() failed.");
            break;
        } else {
            buff[recv_byte] = 0;
            char file_path[64];
            char * client_ip = inet_ntoa(client_addr.sin_addr);
            int client_port = ntohs(client_addr.sin_port);

            printf("Received from: %s:%d\n", client_ip, client_port);

            // Appending string to make path to save files
            strncpy(file_path, LOG_DIR, strlen(LOG_DIR));       // Directory path
            strncat(file_path, client_ip, strlen(client_ip));   // IP
            strcat(file_path, ":");                             // Separator
            sprintf(file_path + strlen(file_path), "%d", client_port);      // Port
            strcat(file_path, ".log");                          // Extension

            FILE * f_handler;
            f_handler = fopen(file_path, "a");
            fprintf(f_handler, "%s", buff); 
            fclose(f_handler);

            memset(file_path, 0, sizeof(file_path));
            puts("------");
        }
    }
}