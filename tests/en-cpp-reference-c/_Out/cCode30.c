//en.cppreference.com/w/c/experimental/dynamic/strdup.html
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif
 
#include <string.h>
#include <assert.h>
#include <stdlib.h>
 
int main(void)
{
    const char *s1 = "String";
    char *s2 = strdup(s1);
    assert(strcmp(s1, s2) == 0);
    free(s2);
}