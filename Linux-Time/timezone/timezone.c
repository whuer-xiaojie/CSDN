#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

static void second_to_tm(void) {
    time_t timestamp;
    struct tm* gm_tm;
    struct tm* local_tm;

    timestamp = time(NULL);

    local_tm = localtime(&timestamp);
    fprintf(stderr, "%s():local time is:%s", __FUNCTION__, asctime(local_tm));

    //time_t to struct tm*
    gm_tm = gmtime(&timestamp);
    fprintf(stderr, "%s():gm time is:%s", __FUNCTION__, asctime(gm_tm));
}

static void tm_to_second(void) {
    struct timeval tv;
    struct tm* local_tm;
    time_t local_t, gm_t;

    if (gettimeofday(&tv, NULL) == 0) {
        local_tm = localtime(&tv.tv_sec);

        local_t = mktime(local_tm);
        fprintf(stderr, "%s():mktime time_t is:%ld\n", __FUNCTION__, local_t);

        local_t = timelocal(local_tm);
        fprintf(stderr, "%s():timelocal time_t is:%ld\n", __FUNCTION__, local_t);

        gm_t = timegm(local_tm);
        fprintf(stderr, "%s():timegm time_t is:%ld\n", __FUNCTION__, gm_t);
    }
}

int main(int argc, char** argv) {
    second_to_tm();
    tm_to_second();
    return 0;
}