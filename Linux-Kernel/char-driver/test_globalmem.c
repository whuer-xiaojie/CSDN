#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEVICE_NAME "/dev/globalmem"
#define MAX_SIZE 1020
#define MEM_CLEAR 0x01

static void usage(char *name) {
    fprintf(stderr, "Usage:%s [message]\n", name);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return -1;
    }
    char *msg = argv[1];
    char buf[MAX_SIZE];

    //open the file
    int fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "open %s failed\n", DEVICE_NAME);
        return -1;
    }

    //clear the file
    if (ioctl(fd, 1, 0) < 0) {
        fprintf(stderr, "clear %s filed\n", DEVICE_NAME);
        close(fd);
        return -1;
    }

    //write message to file
    if (write(fd, msg, strlen(msg)) <= 0) {
        fprintf(stderr, "write %s error\n", DEVICE_NAME);
    }

    if (lseek(fd, -strlen(msg), SEEK_CUR) < 0) {
        fprintf(stderr, "seek %s error\n", DEVICE_NAME);
    }
    
    //read form file
    if (read(fd, buf, MAX_SIZE) <= 0) {
        fprintf(stderr, "read %s failed!\n", DEVICE_NAME);
    } else {
        fprintf(stderr, "read suceess: buf=%s\n", buf);
    }

    close(fd);
    return 0;
}