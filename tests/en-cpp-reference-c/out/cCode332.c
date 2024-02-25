//en.cppreference.com/w/c/string/byte/strcpy.html
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    const char *src = "Take the test.";
//  src[0] = 'M' ; // this would be undefined behavior
    char dst[strlen(src) + 1]; // +1 to accommodate for the null terminator
    strcpy(dst, src);
    dst[0] = 'M'; // OK
    printf("src = %s\ndst = %s\n", src, dst);
 
#ifdef __STDC_LIB_EXT1__
    set_constraint_handler_s(ignore_handler_s);
    int r = strcpy_s(dst, sizeof dst, src);
    printf("dst = \"%s\", r = %d\n", dst, r);
    r = strcpy_s(dst, sizeof dst, "Take even more tests.");
    printf("dst = \"%s\", r = %d\n", dst, r);
#endif
}