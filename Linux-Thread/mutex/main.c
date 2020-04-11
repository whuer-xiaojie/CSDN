#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//自定义数据结构，用于测试
typedef struct ThreadData_t {
    pthread_t m_pid;
    char* m_threadName;
} ThreadData;

ThreadData threadData;
pthread_mutex_t thread_data_mutex;
pthread_mutex_t static_data_mutex = PTHREAD_MUTEX_INITIALIZER;

static void init_thread_mutex(void) {
    pthread_mutexattr_t mutex_attr;
    //PTHREAD_PROCESS_SHARED
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_PRIVATE);
    pthread_mutexattr_settype(&mutex_attr,PTHREAD_MUTEX_DEFAULT);
    pthread_mutex_init(&thread_data_mutex, &mutex_attr); 
}

static void destory_thread_mutex(void) {
    pthread_mutex_destroy(&thread_data_mutex);
}
//线程函数routine
static void* test_thread1(void* thread_name) {
    fprintf(stderr, "%s try to lock!\n", (char*)thread_name);
    pthread_mutex_lock(&thread_data_mutex);
    threadData.m_pid = pthread_self();
    threadData.m_threadName = (char*)thread_name;
    fprintf(stderr, "%s mutex locked! \n", threadData.m_threadName);
    sleep(5);
    pthread_mutex_unlock(&thread_data_mutex);
    return NULL;
}

static void* test_thread2(void* thread_name) {
    fprintf(stderr, "%s try to lock!\n", (char*)thread_name);

    pthread_mutex_lock(&thread_data_mutex);
    threadData.m_pid = pthread_self();
    threadData.m_threadName = (char*)thread_name;
    fprintf(stderr, "%s mutex locked! \n", threadData.m_threadName);
    sleep(6);
    pthread_mutex_unlock(&thread_data_mutex);

    return NULL;
}
int main(int argc, char** argv) {
    pthread_t pid1, pid2;

    init_thread_mutex();

    if (pthread_create(&pid1, NULL, test_thread1, (void*)"test thread1") != 0) {
        fprintf(stderr, "create thread1 failed\n");
        return -1;
    }
    if (pthread_create(&pid2, NULL, test_thread2, (void*)"test thread2") != 0) {
        fprintf(stderr, "create thread2 failed\n");
        return -1;
    }
    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);

    destory_thread_mutex();
    return 0;
}