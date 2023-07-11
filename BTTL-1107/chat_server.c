#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 2048

typedef struct {
    int socket;
    char nickname[16];
    char room[16];
} Client;

Client clients[MAX_CLIENTS];

void broadcast(char *message, char *room, int sender_socket) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket > 0 && strcmp(clients[i].room, room) == 0 && clients[i].socket != sender_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
}

void send_to(char *message, char *nickname) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket > 0 && strcmp(clients[i].nickname, nickname) == 0) {
            send(clients[i].socket, message, strlen(message), 0);
            break;
        }
    }
}

void join(char *nickname, char *room, int client_socket) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == 0) {
            clients[i].socket = client_socket;
            strcpy(clients[i].nickname, nickname);
            strcpy(clients[i].room, room);
            char message[BUFFER_SIZE];
            sprintf(message, "JOIN %s", nickname);
            broadcast(message, room, client_socket);
            break;
        }
    }
}

void msg(char *nickname, char *room, char *message, int sender_socket) {
    char full_message[BUFFER_SIZE];
    sprintf(full_message, "MSG %s %s: %s", room, nickname, message);
    broadcast(full_message, room, sender_socket);
}

void pmsg(char *nickname, char *message) {
    char full_message[BUFFER_SIZE];
    sprintf(full_message, "PMSG %s: %s", nickname, message);
    send_to(full_message, nickname);
}

void op(char *nickname, int sender_socket) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == sender_socket && strcmp(clients[i].room, "main") == 0) {
            strcpy(clients[i].room, nickname);
            char message[BUFFER_SIZE];
            sprintf(message, "OP %s", nickname);
            broadcast(message, "main", sender_socket);
            break;
        }
    }
}

void kick(char *kicked_nickname, char *op_nickname) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket > 0 && strcmp(clients[i].nickname, kicked_nickname) == 0 && strcmp(clients[i].room, "main") == 0) {
            char message[BUFFER_SIZE];
            sprintf(message, "KICK %s %s", kicked_nickname, op_nickname);
            broadcast(message, "main", clients[i].socket);
            clients[i].socket = 0;
            break;
        }
    }
}

void topic(char *op_nickname, char *topic) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket > 0 && strcmp(clients[i].nickname, op_nickname) == 0 && strcmp(clients[i].room, "main") == 0) {
            char message[BUFFER_SIZE];
            sprintf(message, "TOPIC %s", topic);
            broadcast(message, "main", clients[i].socket);
            break;
        }
    }
}

int quit(char *nickname) {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket > 0 && strcmp(clients[i].nickname, nickname) == 0) {
            char message[BUFFER_SIZE];
            sprintf(message, "QUIT %s", nickname);
            broadcast(message, clients[i].room, clients[i].socket);
            clients[i].socket = 0;
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int server_socket, client_socket, i, opt = 1;
    struct sockaddr_in server, client;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    // Initialize clients

    for (i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket = 0;
    }

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Could not create socket");
        return EXIT_FAILURE;
    }

    // Set socket options
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("Could not set socket options");
        return EXIT_FAILURE;
    }

    // Bind to port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Could not bind to port");
        return EXIT_FAILURE;
    }

    // Listen for connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Could not listen for connections");
        return EXIT_FAILURE;
    }

    printf("Chat server started on port 8888...\n");

    while (1) {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add the server socket to the set
        FD_SET(server_socket, &readfds);

        // Add the client sockets to the set
        int max_socket = server_socket;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].socket > 0) {
                FD_SET(clients[i].socket, &readfds);
                if (clients[i].socket > max_socket) {
                    max_socket = clients[i].socket;
                }
            }
        }

        // Wait for activity on the sockets
        if (select(max_socket + 1, &readfds, NULL, NULL, NULL) == -1) {
            perror("Could not wait for activity on sockets");
            return EXIT_FAILURE;
        }

        // Check for new connections
        if (FD_ISSET(server_socket, &readfds)) {
            int client_address_length = sizeof(client);
            client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&client_address_length);
            if (client_socket == -1) {
                perror("Could not accept connection");
                return EXIT_FAILURE;
            }

            // Send welcome message
            char welcome_message[BUFFER_SIZE];
            sprintf(welcome_message, "Welcome to the chat server. Please enter your nickname: ");
            send(client_socket, welcome_message, strlen(welcome_message), 0);
        }

        // Check for activity on client sockets
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].socket > 0 && FD_ISSET(clients[i].socket, &readfds)) {
                int bytes_received = recv(clients[i].socket, buffer, BUFFER_SIZE, 0);
                if (bytes_received <= 0) {
                    // Client disconnected
                    char message[BUFFER_SIZE];
                    sprintf(message, "QUIT %s", clients[i].nickname);
                    broadcast(message, clients[i].room, clients[i].socket);
                    close(clients[i].socket);
                    clients[i].socket = 0;
                } else {
                    // Process message
                    buffer[bytes_received] = '\0';
                    char *command = strtok(buffer, " ");
                    char *arg1 = strtok(NULL, " ");
                    char *arg2 = strtok(NULL, "\n");
                    if (strcmp(command, "JOIN") == 0) {
                        join(arg1, "main", clients[i].socket);
                    } else if (strcmp(command, "MSG") == 0) {
                        msg(clients[i].nickname, clients[i].room, arg1, clients[i].socket);
                    } else if (strcmp(command, "PMSG") == 0) {
                        pmsg(arg1, arg2);
                    } else if (strcmp(command, "OP") == 0) {
                        op(arg1, clients[i].socket);
                    } else if (strcmp(command, "KICK") == 0) {
                        kick(arg1, arg2);
                    } else if (strcmp(command, "TOPIC") == 0) {
                        topic(clients[i].nickname, arg1);
                    } else if (strcmp(command, "QUIT") == 0) {
                        quit(clients[i].nickname);
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}