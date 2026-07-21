//en.cppreference.com/w/c/thread/thrd_sleep.html
/* <threads.h> is not provided by the macOS SDK */
#ifndef __APPLE__
#include <threads.h>
#include <time.h>
#include <stdio.h>
 
int main(void)
{
    printf("Time: %s", ctime(&(time_t){time(NULL)}));
    thrd_sleep(&(struct timespec){.tv_sec=1}, NULL); // sleep 1 sec
    printf("Time: %s", ctime(&(time_t){time(NULL)}));
}
#endif /* !__APPLE__ */