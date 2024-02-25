//en.cppreference.com/w/c/variadic.html
#include <stdio.h>
#include <stdarg.h>
 
void simple_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
 
    while (*fmt != '\0') {
        if (*fmt == 'd') {
            int i = va_arg(args, int);
            printf("%d\n", i);
        } else if (*fmt == 'c') {
            // A 'char' variable will be promoted to 'int'
            // A character literal in C is already 'int' by itself
            int c = va_arg(args, int);
            printf("%c\n", c);
        } else if (*fmt == 'f') {
            double d = va_arg(args, double);
            printf("%f\n", d);
        }
        ++fmt;
    }
 
    va_end(args);
}
 
int main(void)
{
    simple_printf("dcff", 3, 'a', 1.999, 42.5); 
}