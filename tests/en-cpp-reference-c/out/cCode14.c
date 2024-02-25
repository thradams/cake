//en.cppreference.com/w/c/chrono/time_t.html
#include <stdio.h>
#include <time.h>
#include <stdint.h>
 
int main(void)
{
    time_t epoch = 0;
    printf("%jd seconds since the epoch began\n", (intmax_t)epoch);
    printf("%s", asctime(gmtime(&epoch)));
}