//en.cppreference.com/w/c/string/byte/strncpy.html
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
 
int main(void)
{
    char src[] = "hi";
    [[cake::w0052]]
    char dest[6] = "abcdef"; // no null terminator
    
    strncpy(dest, src, 5); // writes five characters 'h', 'i', '\0', '\0', '\0' to dest
    printf("strncpy(dest, src, 5) to a 6-byte dest gives : ");
    for (size_t n = 0; n < sizeof dest; ++n) {
        char c = dest[n];
        c ? printf("'%c' ", c) : printf("'\\0' ");
    }
 
    printf("\nstrncpy(dest2, src, 2) to a 2-byte dst gives : ");
    char dest2[2];
    strncpy(dest2, src, 2); // truncation: writes two characters 'h', 'i', to dest2
    for (size_t n = 0; n < sizeof dest2; ++n) {
        char c = dest2[n];
        c ? printf("'%c' ", c) : printf("'\\0' ");
    }
    printf("\n");
 
#ifdef __STDC_LIB_EXT1__
    set_constraint_handler_s(ignore_handler_s);
    char dst1[6], src1[100] = "hello";
    errno_t r1 = strncpy_s(dst1, 6, src1, 100);  // writes 0 to r1, 6 characters to dst1
    printf("dst1 = \"%s\", r1 = %d\n", dst1,r1); // 'h','e','l','l','o','\0' to dst1
 
    char dst2[5], src2[7] = {'g','o','o','d','b','y','e'};
    errno_t r2 = strncpy_s(dst2, 5, src2, 7);    // copy overflows the destination array
    printf("dst2 = \"%s\", r2 = %d\n", dst2,r2); // writes nonzero to r2,'\0' to dst2[0]
 
    char dst3[5];
    errno_t r3 = strncpy_s(dst3, 5, src2, 4);    // writes 0 to r3, 5 characters to dst3
    printf("dst3 = \"%s\", r3 = %d\n", dst3,r3); // 'g', 'o', 'o', 'd', '\0' to dst3
#endif
}