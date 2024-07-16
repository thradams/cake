//en.cppreference.com/w/c/io/vfwprintf.html
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <stdarg.h>
#include <stddef.h>
#include <wchar.h>
 
void debug_wlog(const wchar_t *fmt, ...)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char time_buf[100];
    size_t rc = strftime(time_buf, sizeof time_buf, "%D %T", gmtime(&ts.tv_sec));
    snprintf(time_buf + rc, sizeof time_buf - rc, ".%06ld UTC", ts.tv_nsec / 1000);
 
    va_list args;
    va_start(args, fmt);
    wchar_t buf[1024];
    int rc2 = vswprintf(buf, sizeof buf / sizeof *buf, fmt, args);
    va_end(args);
 
    if(rc2 > 0)
       wprintf(L"%s [debug]: %ls\n", time_buf, buf);
    else
       wprintf(L"%s [debug]: (string too long)\n", time_buf);
}
 
int main(void)
{
    setlocale(LC_ALL, "");
    debug_wlog(L"Logging, %d, %d, %d", 1, 2, 3);
}