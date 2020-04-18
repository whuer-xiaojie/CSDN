#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void test_ctime(void) {
    time_t local_time;
    char *str_time1, *str_time2;

    //get current local time in seconeds
    local_time = time(NULL);
    str_time1 = ctime(&local_time);
    fprintf(stderr, "%s():local time str1 is:%s\n", __FUNCTION__, str_time1);

    local_time -= 60 * 60 * 24;
    str_time2 = ctime(&local_time);
    fprintf(stderr, "%s():local time str1 is:%s\n", __FUNCTION__, str_time1);
}

static void test_asctime(void) {
    time_t local_time;
    struct tm *ptm;
    char *str_time1, *str_time2;

    //get current local time in seconeds
    local_time = time(NULL);
    //get the `struct tm' representation of *TIMER in the local timezone.
    ptm = localtime(&local_time);

    str_time1 = asctime(ptm);
    fprintf(stderr, "%s():local time str1 is:%s\n", __FUNCTION__, str_time1);

    local_time -= 60 * 60 * 24;
    ptm = localtime(&local_time);
    str_time2 = asctime(ptm);
    fprintf(stderr, "%s():local time str2 is:%s\n", __FUNCTION__, str_time2);
}

int main(int argc, char **argv) {
    test_ctime();
    test_asctime();
    return 0;
}