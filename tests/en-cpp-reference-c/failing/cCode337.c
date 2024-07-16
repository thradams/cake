//en.cppreference.com/w/c/string/byte/strncat.html
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    char str[50] = "Hello ";
    char str2[50] = "World!";
    strcat(str, str2);
    strncat(str, " Goodbye World!", 3);
    puts(str);
 
#ifdef __STDC_LIB_EXT1__
    set_constraint_handler_s(ignore_handler_s);
    char s1[100] = "good";
    char s5[1000] = "bye";
    int r1 = strncat_s(s1, 100, s5, 1000); // r1 is 0, s1 holds "goodbye\0"
    printf("s1 = %s, r1 = %d\n", s1, r1);
    char s2[6] = "hello";
    int r2 = strncat_s(s2, 6, "", 1); // r2 is 0, s2 holds "hello\0"
    printf("s2 = %s, r2 = %d\n", s2, r2);
    char s3[6] = "hello";
    int r3 = strncat_s(s3, 6, "X", 2); // r3 is non-zero, s3 holds "\0"
    printf("s3 = %s, r3 = %d\n", s3, r3);
    // the strncat_s truncation idiom:
    char s4[7] = "abc";
    int r4 = strncat_s(s4, 7, "defghijklmn", 3); // r4 is 0, s4 holds "abcdef\0"
    printf("s4 = %s, r4 = %d\n", s4, r4);
#endif
}