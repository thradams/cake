//en.cppreference.com/w/c/language/while.html
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum { SIZE = 8 };
int main(void)
{
    // trivial example
    int array[SIZE], n = 0;
    while(n < SIZE) array[n++] = rand() % 2;
    puts("Array filled!");
    n = 0;
    while(n < SIZE) printf("%d ", array[n++]);
    printf("\n");
 
    // classic strcpy() implementation
    // (copies a null-terminated string from src to dst)
    char src[] = "Hello, world", dst[sizeof src], *p = dst, *q = src;
    while((*p++ = *q++)) // double parentheses (that are not strictly necessary)
                         // used to suppress warnings, ensuring that this is an
                         // assignment (as opposed to a comparison) by intention,
                         // whose result is used as a truth value
        ; // null statement
    puts(dst);
}