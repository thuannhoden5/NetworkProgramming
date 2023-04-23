#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9999
#define MAX_SIZE 2048
#define SUB_STRING "0123456789"

int main()
{
    // Initializing socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd == -1) {
        perror("[**ERROR] - socket()");
        exit(1);
    }

    // Updating address info
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr(SRV_ADDR); 
    addr.sin_port = htons(PORT);

    // Binding socket
    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("[**ERROR] - bind()");
        exit(1);
    }

    // Listening for connection
    if (listen(sock_fd, 5)) {
        perror("[**ERROR] - listen()");
        exit(1);
    }
    printf("Listening on port %d...\n", PORT);

    // Accepting connection
    // Without Client IP
    int acpt_client = accept(sock_fd, NULL, NULL);
    puts("New client connected."); 
    puts("Data received from client:");

    // Receiving from Client
    char buff[MAX_SIZE];
    int occurrence = 0;
    while(1) {
        int rcv_byte = recv(acpt_client, buff, MAX_SIZE, 0);
        if (rcv_byte <= 0) {
            printf("Connection closed.\n");
            break;
        }
        if (rcv_byte < MAX_SIZE) 
            buff[rcv_byte] = 0;

        // Parsing and Counting
        puts("--------------------------------------------------");
        // Word wrap for Output
        for (unsigned int iChar = 0; iChar < strlen(buff); ++iChar) {
            putchar(buff[iChar]);
            if ((iChar + 1) % 50 == 0) {    // 50 characters per line
                putchar('\n');
            }
        }
        putchar('\n');
        puts("--------------------------------------------------");
        char * tmp = strstr(buff, SUB_STRING);
        occurrence++;
        // Next test string starts at `tmp + 1` to avoid overlapping
        while ((tmp = strstr(tmp + 1, SUB_STRING))) {
            occurrence++;
        }
        printf("- Substring: %s\n", SUB_STRING);
        printf("- Substring occurrences: %d\n", occurrence);
    }
    
    // Closing connection
    close(acpt_client);
    close(sock_fd);
}

