//en.cppreference.com/w/c/io/std_streams.html
#include <stdarg.h>
#include <stdio.h>
 
int my_printf(const char * restrict fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    int ret = vfprintf(stdout, fmt, vl);
    va_end(vl);
    return ret;
}
 
int main(void)
{
    my_printf("Rounding:\t%f %.0f %.32f\n", 1.5, 1.5, 1.3);
    my_printf("Padding:\t%05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
    my_printf("Scientific:\t%E %e\n", 1.5, 1.5);
    my_printf("Hexadecimal:\t%a %A\n", 1.5, 1.5);
}