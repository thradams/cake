//en.cppreference.com/w/c/experimental/dynamic/asprintf.html
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
 
void test(const char *fmt, ...)
{
    char* dyn_buf;
 
    printf("Demo asprintf:\n");
    const int written_1 = asprintf(&dyn_buf, "%s", fmt);
    printf("dyn_buf: \"%s\"; %i chars were written\n", dyn_buf, written_1);
    free(dyn_buf);
 
    printf("Demo vasprintf:\n");
    va_list args;
    va_start(args, fmt);
    const int written_2 = vasprintf(&dyn_buf, fmt, args);
    va_end(args);
    printf("dyn_buf: \"%s\"; %i chars were written\n", dyn_buf, written_2);
    free(dyn_buf);
}
 
int main(void)
{
    test("Testing... %d, %d, %d", 1, 2, 3);
}