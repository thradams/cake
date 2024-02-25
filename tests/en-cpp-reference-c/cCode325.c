//en.cppreference.com/w/c/string/byte/memcpy.html
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
 
int main(void)
{
    // simple usage
    char source[] = "once upon a midnight dreary...", dest[4];
    memcpy(dest, source, sizeof dest);
    for(size_t n = 0; n < sizeof dest; ++n)
        putchar(dest[n]);
 
    // setting effective type of allocated memory to be int
    int *p = malloc(3*sizeof(int));   // allocated memory has no effective type
    int arr[3] = {1,2,3};
    memcpy(p,arr,3*sizeof(int));      // allocated memory now has an effective type
 
    // reinterpreting data
    double d = 0.1;
//    int64_t n = *(int64_t*)(&d); // strict aliasing violation
    int64_t n;
    memcpy(&n, &d, sizeof d); // OK
    printf("\n%a is %" PRIx64 " as an int64_t\n", d, n);
 
#ifdef __STDC_LIB_EXT1__
    set_constraint_handler_s(ignore_handler_s);
    char src[] = "aaaaaaaaaa";
    char dst[] = "xyxyxyxyxy";
    int r = memcpy_s(dst,sizeof dst,src,5);
    printf("dst = \"%s\", r = %d\n", dst,r);
    r = memcpy_s(dst,5,src,10);            //  count is greater than destsz  
    printf("dst = \"");
    for(size_t ndx=0; ndx<sizeof dst; ++ndx) {
        char c = dst[ndx];
        c ? printf("%c", c) : printf("\\0");
    }
    printf("\", r = %d\n", r);
#endif
}