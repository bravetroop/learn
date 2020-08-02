/*
 * Copyright: Co., Ltd. 2018-2020. All rights reserved.
 * Description: 
 * Author: bravetroop
 * Create: 2020-07-12
 * Notes: 
 * History: 
 */ 
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
#define DSET_IP_ADDRESS  "192.168.153.142"

int main()
{
    int send_num;
    int recv_num;
    int sock_fd;
    char send_buf[20] = "hey, who are you?";
    char recv_buf[20];

    socklen_t addrLen;
    struct sockaddr_in addr_serv;

    socklen_t client_addr_len = 0;
    struct sockaddr_in client_addr;

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

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = 0;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));

    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(DSET_IP_ADDRESS);
    addr_serv.sin_port = htons(DEST_PORT);

    getsockname(sock_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    printf("port:[%u]\n", client_addr.sin_port);
    send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr*)&addr_serv, sizeof(addr_serv));
    getsockname(sock_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    printf("port:[%u]\n", client_addr.sin_port);

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