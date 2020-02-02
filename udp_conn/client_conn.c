#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LOCAL_PORT 32000
#define DEST_PORT 8000
#define DSET_IP_ADDRESS  "192.168.217.131"

struct sockaddr_in addr_serv;

int bindLocal(int sock_fd, int port)
{
    int ret = 0;
    socklen_t sock_len;
    struct sockaddr_in localAddr = {0};

    ret = getsockname(sock_fd, (struct sockaddr*)&localAddr, &sock_len);
    if (ret != 0) {
        printf("get sock name failed, errno:[%d]\n", errno);
    }

    localAddr.sin_port = htons(port);
    ret = bind(sock_fd, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if(ret != 0) {
        printf("bind local port failed, errno:[%d]\n", errno);
    }

    return 0;
}

int connectServer(int sock_fd, int family)
{
    bindLocal(sock_fd, LOCAL_PORT);

    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = family;
    addr_serv.sin_addr.s_addr = inet_addr(DSET_IP_ADDRESS);
    addr_serv.sin_port = htons(DEST_PORT);

    if(connect(sock_fd, (struct sockaddr*)&addr_serv, sizeof(addr_serv)) == -1) {
        printf("connect server error:%d\n", errno);
        perror("connect server error");
        close(sock_fd);
        return -1;
    }

    return 0;
}

int createFd()
{
    int sock_fd;
    /* 设置address */

    /* 建立udp socket */
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        perror("socket");
        return -1;
    }

    connectServer(sock_fd, AF_INET);

    return sock_fd;
}

int main()
{
    int times = 0;
    int conn_status = 1;
    int send_num;
    int recv_num;
    socklen_t recv_len = 0;
    char send_buf[20] = "hey, who are you?";
    char recv_buf[20];

    /* socket文件描述符 */
    int sock_fd;

    sock_fd = createFd();
    if (sock_fd == -1) {
        printf("create socket error\n");
        return 1;
    }

    while(1) {
        sleep(1);
        if (conn_status) {
            send_num = send(sock_fd, send_buf, strlen(send_buf), 0);
        } else {
            send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0,
                          (struct sockaddr*)&addr_serv, sizeof(addr_serv));
        }

        if(send_num < 0) {
            perror("sendto error:");
        }

        ++times;
        if (times % 20 == 0) {
            connectServer(sock_fd, AF_INET);
            printf("reconnect\n");
            conn_status = 1;
        } else if(times % 10 == 0) {
            connectServer(sock_fd, AF_UNSPEC);
            printf("disconnect\n");
            conn_status = 0;
        }

        if (conn_status) {
            recv_num = recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
        } else {
            recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0,
                          (struct sockaddr*)&addr_serv, &recv_len);
        }

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