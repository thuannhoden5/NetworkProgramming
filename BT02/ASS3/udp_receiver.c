#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9999
#define MAX_SIZE 65535
#define SAVE_FILE_PATH "udp_file_receiver.txt" 

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
    printf("Listening on port %d\n", PORT);
    
    // Receiving datagram
    char buff[MAX_SIZE];
    // int name_flag = 0;   
    while (1) {
        int recv_byte = recvfrom(sock_fd, buff, sizeof(buff), 0, NULL, NULL);        
        if (recv_byte == -1) {
            puts("recvfrom() failed.");
            break;
        } else {
            buff[recv_byte] = 0;
            char rcv_filename[30];
            int str_ptr = strcspn(buff, " ");

            strncpy(rcv_filename, buff, str_ptr);

            FILE * f_handler;
            f_handler = fopen(SAVE_FILE_PATH, "w");
            puts("--------------------------------");
            printf("File received from Client: %s\n", rcv_filename);;
            puts("Saved to 'receiver_datagram.txt'.");
            puts("--------------------------------");
            puts(buff + str_ptr + 1);
            // fwrite(buff, strlen(buff), 1, f_handler);
            fprintf(f_handler, "%s", buff + str_ptr + 1);
            fclose(f_handler);

            memset(buff, 0, sizeof(buff));
        }
    }
}