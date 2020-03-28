
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static int socket_fd = -1;
static int quit = 0;
static const size_t MAX_FRAME_SIZE = 4100;
static const char *tcp_server_ip = "0.0.0.0";
static const int tcp_server_port = 1234;

static void tcp_client_handle_sighup(int sig) {
    //TODO：do something while recv sighup
}

static int8_t client_connet_to_server() {
    struct sigaction sa;
    struct sockaddr_in clientAddr;
    struct sockaddr_in serverAddr;

    if (socket_fd != -1) {
        shutdown(socket_fd, SHUT_RDWR);
        close(socket_fd);
        socket_fd = -1;
    }

    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction() Failed: %d", errno);
        return -1;
    }

    bzero(&clientAddr, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htons(INADDR_ANY);
    clientAddr.sin_port = htons(0);

    //创建client
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        fprintf(stderr, "initial tcp_client create socket failed\n");
        return -1;
    }

    // 3.将socket建立为非阻塞，此时socket被设置为非阻塞模式
    int flags = fcntl(socket_fd, F_GETFL, 0);       //获取建立的sockfd的当前状态（非阻塞）
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);  //将当前sockfd设置为非阻塞

    if (bind(socket_fd, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) == -1) {
        fprintf(stderr, "initial tcp_client bind socket failed\n");
        return -1;
    }

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(tcp_server_ip);
    serverAddr.sin_port = htons(tcp_server_port);

    while (connect(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        static int8_t circleTime = 0;
        ++circleTime;
        if (circleTime >= 20) {
            circleTime = 0;
        }
        if (quit) {
            fprintf(stderr, "tcp client thread terminated\n");
            return -1;
        }

        sleep(2);
        fprintf(stderr, "start to connect the host:%s\n", tcp_server_ip);
    }
    fprintf(stderr, "connect the host:%s success\n", tcp_server_ip);
    return 0;
}

int main(int argc, char **argv) {
    char *buf = "client test";
    char recv_buf[MAX_FRAME_SIZE];
    ssize_t recv_len = 0;
    int i;

    if (client_connet_to_server() == 0) {
        while (1) {
            if (write(socket_fd, buf, strlen(buf)) <= 0) {
                fprintf(stderr, "send %s to server failed!\n", buf);
            }
            recv_len = read(socket_fd, recv_buf, MAX_FRAME_SIZE);
            if (recv_len > 0) {
                //TODO:code the protocol recv here(may have start or end flag)
                fprintf(stderr, "/**************************/\n");
                fprintf(stderr, "%s\n ", recv_buf);
                fprintf(stderr, "/**************************/\n\n");
            }
            sleep(5);
        }
    };
}