#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//声明一个全局变量key,该key对所有线程可见，
//但是该key具体指向的内容各个线程不一样
//可以理解为一个二级指针
pthread_key_t key;
//用于标识key,key只创建一次
pthread_once_t once_create = PTHREAD_ONCE_INIT;

//自定义数据结构，用于测试
typedef struct ThreadData_t {
    pthread_t m_pid;
    char* m_threadName;
} ThreadData;

//释放线程自定义数据，即每个线程使用key具体指向的数据
//这里其实就是ThreadData
static void free_thread_data(void* pData) {
    fprintf(stderr, "%s():addr= %p\n", __FUNCTION__, pData);
    free(pData);
}

//创建key
static void create_my_key(void) {
    fprintf(stderr, "%s():called\n", __FUNCTION__);
    pthread_key_create(&key, free_thread_data);
}

static void test_get_thread_data(void) {
    ThreadData* pData = (ThreadData*)pthread_getspecific(key);
    fprintf(stderr, "%s():%s get data by key addr=%p\n",
            __FUNCTION__, pData->m_threadName, pData);
    return;
}

//线程函数routine
static void* test_thread(void* thread_name) {
    //如果key没有被创建，就创建key，被创建了就不会再创建
    pthread_once(&once_create, create_my_key);

    //申请一个线程数据结构，该数据只在该线程内可见，其他线程不可见，
    //但是该线程的其他函数可以获取到该数据
    ThreadData* pData = (ThreadData*)malloc(sizeof(ThreadData));
    if (pData == NULL) {
        fprintf(stderr, "%s():not enough memory\n", __FUNCTION__);
        return NULL;
    }

    pData->m_pid = pthread_self();
    pData->m_threadName = (char*)thread_name;
    fprintf(stderr, "thread %s malloc data addr=%p\n",
            pData->m_threadName, pData);

    pthread_setspecific(key, pData);
    sleep(5);
    test_get_thread_data();
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t pid1, pid2;

    if (pthread_create(&pid1, NULL, test_thread, (void*)"test thread1") != 0) {
        fprintf(stderr, "create thread1 failed\n");
        return -1;
    }
    if (pthread_create(&pid2, NULL, test_thread, (void*)"test thread2") != 0) {
        fprintf(stderr, "create thread2 failed\n");
        return -1;
    }
    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);

    pthread_key_delete(key);
    return 0;
}