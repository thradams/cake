//en.cppreference.com/w/c/string/byte/strtoimax.html
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
 
int main(void)
{
    char* endptr;
 
    printf("%ld\n", strtoimax(" -123junk",&endptr,10)); /* base 10                    */
    printf("%ld\n", strtoimax("11111111",&endptr,2));   /* base 2                     */
    printf("%ld\n", strtoimax("XyZ",&endptr,36));       /* base 36                    */
    printf("%ld\n", strtoimax("010",&endptr,0));        /* octal auto-detection       */
    printf("%ld\n", strtoimax("10",&endptr,0));         /* decimal auto-detection     */
    printf("%ld\n", strtoimax("0x10",&endptr,0));       /* hexadecimal auto-detection */
 
    /* range error             */
    /* LONG_MAX+1 --> LONG_MAX */
    errno = 0;
    printf("%ld\n", strtoimax("9223372036854775808",&endptr,10));
    printf("%s\n", strerror(errno));
 
    return 0;
}