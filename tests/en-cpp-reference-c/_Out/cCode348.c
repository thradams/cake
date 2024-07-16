//en.cppreference.com/w/c/string/byte/strtol.html
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    // parsing with error handling
    const char *p = "10 200000000000000000000000000000 30 -40 junk";
    printf("Parsing '%s':\n", p);
 
    for (;;)
    {
        // errno can be set to any non-zero value by a library function call
        // regardless of whether there was an error, so it needs to be cleared
        // in order to check the error set by strtol
        errno = 0;
        char *end;
        const long i = strtol(p, &end, 10);
        if (p == end)
            break;
 
        const bool range_error = errno == ERANGE;
        printf("Extracted '%.*s', strtol returned %ld.", (int)(end-p), p, i);
        p = end;
 
        if (range_error)
            printf("\n --> Range error occurred.");
 
        putchar('\n');
    }
 
    printf("Unextracted leftover: '%s'\n\n", p);
 
    // parsing without error handling
    printf("\"1010\" in binary  --> %ld\n", strtol("1010", NULL, 2));
    printf("\"12\"   in octal   --> %ld\n", strtol("12",   NULL, 8));
    printf("\"A\"    in hex     --> %ld\n", strtol("A",    NULL, 16));
    printf("\"junk\" in base-36 --> %ld\n", strtol("junk", NULL, 36));
    printf("\"012\"  in auto-detected base --> %ld\n", strtol("012",  NULL, 0));
    printf("\"0xA\"  in auto-detected base --> %ld\n", strtol("0xA",  NULL, 0));
    printf("\"junk\" in auto-detected base --> %ld\n", strtol("junk", NULL, 0));
}