
#include "tcp-server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>


static const size_t MAX_FRAME_SIZE = 4100;
static const int PORT = 1234;

static device_t tcp_device;
static int listen_fd = -1;


static void __sleep(unsigned long ms) {
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = ms % 1000 * 1000;
    select(0, NULL, NULL, NULL, &tv);
}

static int tcp_server_init(device_t *dev) {
    struct sockaddr_in addr;
    //创建socket，获取一个socket描述符
    if ((listen_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "%s(): cannot create socket", __FUNCTION__);
        return -1;
    }
    //设置server的地址同通信协议
    memset(&addr, 0, sizeof(struct sockaddr_in));
    //协议族: AF_INET AF_INET6 AF_LOCAL
    addr.sin_family = AF_INET;
    //取本地任意一个地址进行通信
    // uint32_t htonl(uint32_t hostlong) 将主机无符号长整型转化为网络字节
    //INADDR_ANY=0.0.0.0
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // uint16_t htons(uint16_t hostshort) 将hostshort 转化为网络字节序
    //网络字节采用大端模式（big-ending）,高位存放在低地址
    addr.sin_port = htons(PORT);
    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "%s(): bind() failed", __FUNCTION__);
        goto failed;
    }
    if (listen(listen_fd, LISTEN_BACKLOG) != 0) {
        fprintf(stderr, "%s(): listen() failed", __FUNCTION__);
        goto failed;
    }
    fprintf(stderr, "%s(): tcp_server running on %s port=%d\n", __FUNCTION__, inet_ntoa(addr.sin_addr), PORT);
    dev->recv_buf = (unsigned char *)malloc(MAX_FRAME_SIZE);
    dev->recv_cnt = 0;
    if (dev->recv_buf == NULL) {
        fprintf(stderr, "%s(): not enough memory", __FUNCTION__);
        goto failed;
    }

    dev->dev_send = write;
    return 0;
failed:
    close(listen_fd);
    listen_fd = -1;
    return -1;
}

static int tcp_server_close(device_t *dev) {
    int ret = 0;
    if (listen_fd != -1) {
        if (close(listen_fd) != 0)
            ret = -1;
    }

    if (dev && dev->recv_buf != NULL) {
        free(dev->recv_buf);
        dev->recv_buf = NULL;
    }

    return ret;
}
static ssize_t tcp_server_recv(device_t *dev) {
    unsigned char buf[MAX_FRAME_SIZE];
    ssize_t n_read;
    char *test_buf="server test";
    if ((dev->recv_cnt = read(dev->fd, dev->recv_buf, MAX_FRAME_SIZE)) < 0) /* error */
        return -1;
    //TODO:code the protocol recv here(may have start or end flag)
    fprintf(stderr, "/**************************/\n");
    fprintf(stderr, "%s\n", dev->recv_buf);
    fprintf(stderr, "/**************************/\n\n");

    return dev->dev_send(dev->fd, test_buf, strlen(test_buf));
}

static void tcp_server_handle_sighup(int sig) {
    //TODO：do something while recv sighup
}

static int tcp_server_main(int argc, char **argv) {
    //使用fd_set以及select实现非阻塞模式
    fd_set rfds, afds;
    int max_fd = 0, fd;
    int i;
    struct sigaction sa;

    //清空文件描述符集合
    FD_ZERO(&afds);
    if (listen_fd != -1) {
        FD_SET(listen_fd, &afds);
        max_fd = MAX(max_fd, listen_fd);
    }
    if (max_fd == 0) {
        fprintf(stderr, "%s(): terminated\n", __FUNCTION__);
        return -1;
    }

    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = tcp_server_handle_sighup;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        fprintf(stderr, "%s(): sigaction() error: %d", __FUNCTION__, errno);
        return -1;
    }

    while (1) {
        rfds = afds; /* structure assignment */

        //监听所有文件描述符
        if (select(max_fd + 1, &rfds, NULL, NULL, NULL) < 0) {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "%s(): select() error: %d", __FUNCTION__, errno);
            break;
        }

        if (FD_ISSET(listen_fd, &rfds)) { /* client connected */
            struct sockaddr_in addr;
            socklen_t addr_len;

            addr_len = sizeof(addr);
            fd = accept(listen_fd, (struct sockaddr *)&addr, &addr_len);
            if (fd != -1) {
                //把建议链接的客户端文件描述符添加到监听集合
                FD_SET(fd, &afds);
                fprintf(stderr, "%s(): client %s connented\n\n", __FUNCTION__, inet_ntoa(addr.sin_addr));
                max_fd = MAX(max_fd, fd);
            }
        }
        //遍历所有文件描述符，接收客户端的数据
        for (fd = 0; fd <= max_fd; fd++) {
            if (fd == listen_fd || !FD_ISSET(fd, &rfds)) {
                continue;
            }
            //XXX：这里把每一个客户端的信息都存在一个结构里面，应该为每一个链接创建不同的链接来进行通信
            tcp_device.fd = fd;
            if (tcp_server_recv(&tcp_device) <= 0) {
                /* client closed */
                close(fd);
                FD_CLR(fd, &afds);
            }
        }
        __sleep(30L);
    }
    tcp_server_close(&tcp_device);
    return 0;
}

int main(int argc, char **argv) {
    if (tcp_server_init(&tcp_device) != 0) {
        fprintf(stderr, "%s(): tcp_server_init failed!\n", __FUNCTION__);
        return -1;
    }

    tcp_server_main(argc, argv);

    return 0;
}