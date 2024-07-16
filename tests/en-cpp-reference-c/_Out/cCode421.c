//en.cppreference.com/w/c/types/limits.html
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
 
int main(void)
{
    printf("CHAR_BIT       = %d\n", CHAR_BIT);
    printf("MB_LEN_MAX     = %d\n\n", MB_LEN_MAX);
 
    printf("CHAR_MIN       = %+d\n", CHAR_MIN);
    printf("CHAR_MAX       = %+d\n", CHAR_MAX);
    printf("SCHAR_MIN      = %+d\n", SCHAR_MIN);
    printf("SCHAR_MAX      = %+d\n", SCHAR_MAX);
    printf("UCHAR_MAX      = %u\n\n", UCHAR_MAX);
 
    printf("SHRT_MIN       = %+d\n", SHRT_MIN);
    printf("SHRT_MAX       = %+d\n", SHRT_MAX);
    printf("USHRT_MAX      = %u\n\n", USHRT_MAX);
 
    printf("INT_MIN        = %+d\n", INT_MIN);
    printf("INT_MAX        = %+d\n", INT_MAX);
    printf("UINT_MAX       = %u\n\n", UINT_MAX);
 
    printf("LONG_MIN       = %+ld\n", LONG_MIN);
    printf("LONG_MAX       = %+ld\n", LONG_MAX);
    printf("ULONG_MAX      = %lu\n\n", ULONG_MAX);
 
    printf("LLONG_MIN      = %+lld\n", LLONG_MIN);
    printf("LLONG_MAX      = %+lld\n", LLONG_MAX);
    printf("ULLONG_MAX     = %llu\n\n", ULLONG_MAX);
 
    printf("PTRDIFF_MIN    = %td\n", PTRDIFF_MIN);
    printf("PTRDIFF_MAX    = %+td\n", PTRDIFF_MAX);
    printf("SIZE_MAX       = %zu\n", SIZE_MAX);
    printf("SIG_ATOMIC_MIN = %+jd\n",(intmax_t)SIG_ATOMIC_MIN);
    printf("SIG_ATOMIC_MAX = %+jd\n",(intmax_t)SIG_ATOMIC_MAX);
    printf("WCHAR_MIN      = %+jd\n",(intmax_t)WCHAR_MIN);
    printf("WCHAR_MAX      = %+jd\n",(intmax_t)WCHAR_MAX);
    printf("WINT_MIN       = %jd\n", (intmax_t)WINT_MIN);
    printf("WINT_MAX       = %jd\n", (intmax_t)WINT_MAX);
}