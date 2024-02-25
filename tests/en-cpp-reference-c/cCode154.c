//en.cppreference.com/w/c/language/variadic.html
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
 
void tlog(const char* fmt,...)
{
    char msg[50];
    strftime(msg, sizeof msg, "%T", localtime(&(time_t){time(NULL)}));
    printf("[%s] ", msg);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
 
int main(void)
{
   tlog("logging %d %d %d...\n", 1, 2, 3);
}