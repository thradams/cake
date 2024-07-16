//en.cppreference.com/w/c/string/byte/memmove.html
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
 
int main(void)
{
    char str[] = "1234567890";
    puts(str);
    memmove(str+4, str+3, 3); // copy from [4,5,6] to [5,6,7]
    puts(str);
 
    // setting effective type of allocated memory to be int
    int *p = malloc(3*sizeof(int));   // allocated memory has no effective type
    int arr[3] = {1,2,3};
    memmove(p,arr,3*sizeof(int));     // allocated memory now has an effective type
 
    // reinterpreting data
    double d = 0.1;
//    int64_t n = *(int64_t*)(&d); // strict aliasing violation
    int64_t n;
    memmove(&n, &d, sizeof d); // OK
    printf("%a is %" PRIx64 " as an int64_t\n", d, n);
 
#ifdef __STDC_LIB_EXT1__
    set_constraint_handler_s(ignore_handler_s);
    char src[] = "aaaaaaaaaa";
    char dst[] = "xyxyxyxyxy";
    int r = memmove_s(dst,sizeof dst,src,5);
    printf("dst = \"%s\", r = %d\n", dst,r);
    r = memmove_s(dst,5,src,10);            //  count is greater than destsz  
    printf("dst = \"");
    for(size_t ndx=0; ndx<sizeof dst; ++ndx) {
        char c = dst[ndx];
        c ? printf("%c", c) : printf("\\0");
    }
    printf("\", r = %d\n", r);
#endif
}