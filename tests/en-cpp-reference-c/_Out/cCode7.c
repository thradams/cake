//en.cppreference.com/w/c/chrono/ctime.html
#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>
#include <stdio.h>
 
int main(void)
{
    time_t result = time(NULL);
    printf("%s", ctime(&result));
 
#ifdef __STDC_LIB_EXT1__
    char str[26];
    ctime_s(str,sizeof str,&result);
    printf("%s", str);
#endif
}