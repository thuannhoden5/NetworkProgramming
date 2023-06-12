#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#define PORT 8888 
#define BACKLOG 5 

int main(int argc, char *argv[]) {

    fd_set masterfds, readfds;
    int maxfds;
    int listener, newfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen;
    char buffer[256];
    int nbytes;

    int client_id = 1;
    char client_name[256];
    char client_id_name[256];

    int client_list[BACKLOG]; 
    char client_info[BACKLOG][256]; 

    int i;
    for (i = 0; i < BACKLOG; i++) {
        client_list[i] = -1;
    }
   
    listener = socket(AF_INET, SOCK_STREAM, 0);

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);

    if (bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(listener, BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&masterfds);
    FD_SET(listener, &masterfds);
    maxfds = listener;

    while(1) {
        readfds = masterfds;
        if (select(maxfds + 1, &readfds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(listener, &readfds)) {
            addrlen = sizeof(clientaddr);
            newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen);

            if (newfd < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            } else {
                printf("New connection from %s on socket %d\n", inet_ntoa(clientaddr.sin_addr), newfd);
                FD_SET(newfd, &masterfds);
                if (newfd > maxfds) {
                    maxfds = newfd;
                }

                // Hỏi tên client
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "Enter your name: ");
                write(newfd, buffer, strlen(buffer));
            }
        }
    }
}