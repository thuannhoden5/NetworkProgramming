#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000); 

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("connect() failed");
        return 1;
    }
        
    char tenmt[14];
    char hoten[64];
    char ns[12];
    float dtb;
    char buf[256];
    int odia = 0;
    char tenodia[20];
    int kichthuoc[20];
    while (1)
    {
        printf("Nhap thong tin may tinh.\n");
        printf("Nhap ten may tinh: ");
        fgets(tenmt, sizeof(tenmt), stdin)


        for(int i=0; i < odia; i++){
            printf("Nhap ten o dia 1");
            scanf("%c", &tenodia[i]);
            printf("Nhap dung luong");
            scanf("%d", &kichthuoc[i]);
        }


        printf("%s\n", buf);

        send(client, buf, strlen(buf), 0);
    }

    // Ket thuc, dong socket
    close(client);

    return 0;
}