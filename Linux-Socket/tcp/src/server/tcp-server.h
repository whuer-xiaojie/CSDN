#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <limits.h>    /* PATH_MAX */
#include <sys/types.h> /* ssize_t */

#define LISTEN_BACKLOG 32

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

typedef struct device_s {
    int fd;
    int state;
    unsigned char *recv_buf;
    size_t recv_cnt;
    unsigned char *data;
    size_t data_len;
    ssize_t (*dev_send)(int, const void *, size_t);
} device_t;

#endif