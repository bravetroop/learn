#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEST_PORT 8000
#define DSET_IP_ADDRESS  "192.168.217.131"

int main()
{
    int send_num;
    int recv_num;
    int sock_fd;
    char send_buf[20] = "hey, who are you?";
    char recv_buf[20];

    socklen_t addrLen;
    struct sockaddr_in addr_serv;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        perror("socket");
        return -1;
    }

    if (sock_fd == -1) {
        printf("create socket error\n");
        return -1;
    }

    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(DSET_IP_ADDRESS);
    addr_serv.sin_port = htons(DEST_PORT);

    while(1) {
        sleep(1);
        send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr*)&addr_serv, sizeof(addr_serv));

        if(send_num < 0) {
            perror("sendto error:");
            continue;
        }

        recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&addr_serv, &addrLen);

        if(recv_num < 0) {
            perror("recvfrom error:");
            continue;
        }
    
        recv_buf[recv_num] = '\0';
        printf("client receive %d bytes: %s\n", recv_num, recv_buf);
    }

    close(sock_fd);

    return 0;
}