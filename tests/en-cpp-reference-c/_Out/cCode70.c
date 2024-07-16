//en.cppreference.com/w/c/io/vfprintf.html
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
 
void debug_log(const char *fmt, ...)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char time_buf[100];
    size_t rc = strftime(time_buf, sizeof time_buf, "%D %T", gmtime(&ts.tv_sec));
    snprintf(time_buf + rc, sizeof time_buf - rc, ".%06ld UTC", ts.tv_nsec / 1000);
 
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    char buf[1+vsnprintf(NULL, 0, fmt, args1)];
    va_end(args1);
    vsnprintf(buf, sizeof buf, fmt, args2);
    va_end(args2);
 
    printf("%s [debug]: %s\n", time_buf, buf);
}
 
int main(void)
{
    debug_log("Logging, %d, %d, %d", 1, 2, 3);
}