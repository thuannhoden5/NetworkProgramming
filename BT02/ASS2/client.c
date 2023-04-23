#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SRV_ADDR "127.0.0.1"
#define PORT 9999
#define MAX_SIZE 2048

int main()
{
    // Initializing socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Updating address info
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SRV_ADDR);
    // inet_aton(argv[1], &addr.sin_addr);
    addr.sin_port = htons(PORT);

    // Connecting to the server
    int c_state = connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (c_state == -1) {
        perror("[**ERROR] - connect()");
        exit(1);
    }

    char buff[MAX_SIZE];
    FILE * f_handler;
    f_handler = fopen("data.txt", "r");
    while ( !feof(f_handler) ) {
        fscanf(f_handler, "%s", buff);      // Write from file to buffer
    }
    puts(buff);
    send(sock_fd, buff, strlen(buff), 0);

    // Closing connection
    close(sock_fd);
}
