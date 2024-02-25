//en.cppreference.com/w/c/chrono/tm.html
#include <stdio.h>
#include <time.h>
 
int main(void)
{
    struct tm start = {.tm_year=2022-1900, .tm_mday=1};
    mktime(&start);
    printf("%s", asctime(&start));
}