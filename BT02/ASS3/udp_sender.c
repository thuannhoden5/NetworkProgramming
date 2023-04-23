#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define RECEIVER_ADDR "127.0.0.1"
#define PORT 9999
#define MAX_SIZE 65535

/** @function: VerifyAddress()
* @brief: Checking IPv4 address
* @param cmd_address: Command line argument for server IPv4 address
*
* @return: 0 if valid
*          1 if invalid.
**/
int VerifyAddress (const char * cmd_address) 
{
    char buff[32], sub_buff[8];
    char * octet_str;
    
    strcpy(buff, cmd_address);
    octet_str = strtok(buff, ".");        // Parse octets

    while (octet_str != NULL) {
        strcpy(sub_buff, octet_str);

        unsigned int oclen = strlen(octet_str);       // Check octet string length
        if(oclen > 3) {
            return 1;
        }
        for (unsigned int iDigit = 0; iDigit < oclen; ++iDigit) {
            int tmp = sub_buff[iDigit] - '0';     // Number only
            if (tmp < 0 || tmp > 9) {
                return 1;
            }
        }

        octet_str = strtok(NULL, ".");
    }
    
    return 0;
}

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
    if (argc != 4) {
        puts("[**ERROR]: Invalid Comamnd.");
        puts("[USAGE]: ./udp_sender receiver_IPv4 port send_file");
        exit(1);
    }

    // Verifying Address
    if (VerifyAddress(argv[1])) {
        puts("[**ERROR]: Invalid IPv4 Address");
        exit(1);
    }

    // Verifying Port
    int port = VerifyPort(argv[2]);
    if (port == -1) {
        puts("[**ERROR]: Invalid Port.");
        exit(1);
    }

    // Initializing socket
    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Updating address info
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(port);

    // Sending datagram
    char buff[MAX_SIZE];
    FILE * f_handler;
    f_handler = fopen(argv[3], "r");
    if (f_handler == NULL) {
        puts("[**ERROR]: File does not exist");
        exit(1);
    }
    puts("Sending Datagram");

    strncpy(buff, argv[3], strlen(argv[3]));
    strcat(buff, " ");
    int str_ptr = strlen(buff);

    fread(buff + str_ptr, MAX_SIZE, 1, f_handler);
    fclose(f_handler);

    puts(buff);
    sendto(sock_fd, buff, strlen(buff), 0, (struct sockaddr *)&addr, sizeof(addr));
    
}