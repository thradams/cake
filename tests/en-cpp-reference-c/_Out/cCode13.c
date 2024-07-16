//en.cppreference.com/w/c/chrono/time.html
#include <stdio.h>
#include <time.h>
#include <stdint.h>
 
int main(void)
{
    time_t result = time(NULL);
    if(result != (time_t)(-1))
        printf("The current time is %s(%jd seconds since the Epoch)\n",
               asctime(gmtime(&result)), (intmax_t)result);
}