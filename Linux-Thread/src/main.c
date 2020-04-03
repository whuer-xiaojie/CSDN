#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "keyboard.h"

#define MAX_THREAD_COUNT 10

typedef struct thread_s {
    pthread_t thread;                //线程标志符
    void *(*start_routine)(void *);  //函数指针，需要执行的函数
    void *arg;                       //指针函数的参数
    int blocked;                     //线程运行状态标志
    int started;                     //现场启动标志
} thread_t;

static thread_t threads[MAX_THREAD_COUNT];  //定义一个静态数组存放线程信息
static int thread_cnt = 0;                  //已有线程的数据

static int quit = 0;  //标记是否退出线程

static void add_thread(void *(*start_routine)(void *), void *arg, int blocked) {
    if (thread_cnt >= MAX_THREAD_COUNT)
        return;
    threads[thread_cnt].start_routine = start_routine;
    threads[thread_cnt].arg = arg;
    threads[thread_cnt].blocked = blocked;
    threads[thread_cnt].started = 0;
    thread_cnt++;
}

static void start_thread(void) {
    int i;
    for (i = 0; i < thread_cnt; i++) {
        //创建一个线程
        if (pthread_create(&threads[i].thread, NULL, threads[i].start_routine,
                           threads[i].arg) == 0) {
            threads[i].started = 1;
        } else {
            fprintf(stderr, "%s(): cannot create thread %d\n", __FUNCTION__, i);
        }
    }
}

static void exit_thread(void) {
    int i;
    for (i = 0; i < thread_cnt; i++) {
        if (threads[i].started) {
            if (threads[i].blocked)
                pthread_kill(threads[i].thread, SIGHUP);
            pthread_join(threads[i].thread, NULL);
        }
    }
}

static void *test_thread_a(void *arg) {
    while (!quit) {
        //在这里添加需要执行的代码
        fprintf(stderr, "%s(): thread is running!\n", __FUNCTION__);
        sleep(2);
    }

    fprintf(stderr, "%s(): thread terminated!\n", __FUNCTION__);
}

static void *test_thread_b(void *arg) {
    while (1) {
        fprintf(stderr, "%s(): thread is running!\n", __FUNCTION__);
        sleep(3);
    }
    fprintf(stderr, "%s(): thread terminated!\n", __FUNCTION__);
}

int main(int argc, char **argv) {
    int _char;
    add_thread(test_thread_a, NULL, 0);
    add_thread(test_thread_b, NULL, 1);

    start_thread();

    //初始化键盘输入
    if (init_keyboard() != 0) {
        fprintf(stderr, "init_keyboard failed\n");
    } else {
        fprintf(stderr, "Press Q or q quit\n");
    }

    while (!quit) {
        //接收所有键盘输入的字符，如果有Q或者q就退出程序
        while ((_char = __getch()) >= 0) {
            if (_char == 'Q' || _char == 'q') {
                quit = 1;
                break;
            }
        }
        sleep(2);
    }

    close_keyboard();
    exit_thread();
    return 0;
}