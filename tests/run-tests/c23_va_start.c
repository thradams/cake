/*
   C23 7.16.1.4: the second argument of va_start is optional. The generated
   code targets older compilers, so cake inserts the last named parameter.
*/
#include <stdarg.h>
#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static int sum(int count, ...)
{
    int result = 0;
    va_list args;
    va_start(args);
    for (int i = 0; i < count; i++)
        result += va_arg(args, int);
    va_end(args);
    return result;
}

/* the last named parameter is not the first one */
static double sum_d(const char* label, int count, ...)
{
    double result = 0;
    va_list args;
    va_start(args);
    for (int i = 0; i < count; i++)
        result += va_arg(args, double);
    va_end(args);
    return label[0] == 'x' ? result : -1;
}

/* the old form still works */
static int sum_old(int count, ...)
{
    int result = 0;
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++)
        result += va_arg(args, int);
    va_end(args);
    return result;
}

int main(void)
{
    CHECK(sum(4, 25, 25, 50, 50) == 150);
    CHECK(sum(0) == 0);
    CHECK(sum_d("x", 2, 1.5, 2.5) == 4.0);
    CHECK(sum_old(3, 1, 2, 3) == 6);
    return failures;
}
