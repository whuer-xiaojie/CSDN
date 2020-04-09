#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static void *test_thread_fun(void *arg) {
    while (1) {
        //在这里添加需要执行的代码
        fprintf(stderr, "%s(): thread is running!\n", __FUNCTION__);
        sleep(1);
    }

    fprintf(stderr, "%s(): thread terminated!\n", __FUNCTION__);
}

int main(int argc, char **argv) {
    pthread_attr_t attr;
    pthread_t pid;
    struct sched_param param;
    int i = 20;

    pthread_attr_init(&attr);
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = i;

    pthread_attr_setschedparam(&attr, &param);
    //创建一个非绑定，非分离，优先级为20的线程
    pthread_create(&pid, &attr, test_thread_fun, NULL);
    sleep(20);
    pthread_kill(pid);
    pthread_join(pid,NULL);
    return 0;
}