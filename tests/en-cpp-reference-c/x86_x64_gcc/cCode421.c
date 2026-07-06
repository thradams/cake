/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    printf("CHAR_BIT       = %d\n", 8);
    printf("MB_LEN_MAX     = %d\n\n", 16);
    printf("CHAR_MIN       = %+d\n", -128);
    printf("CHAR_MAX       = %+d\n", 127);
    printf("SCHAR_MIN      = %+d\n", -128);
    printf("SCHAR_MAX      = %+d\n", 127);
    printf("UCHAR_MAX      = %u\n\n", 255);
    printf("SHRT_MIN       = %+d\n", -32768);
    printf("SHRT_MAX       = %+d\n", 32767);
    printf("USHRT_MAX      = %u\n\n", 65535);
    printf("INT_MIN        = %+d\n", -2147483648);
    printf("INT_MAX        = %+d\n", 2147483647);
    printf("UINT_MAX       = %u\n\n", 4294967295);
    printf("LONG_MIN       = %+ld\n", -9223372036854775808L);
    printf("LONG_MAX       = %+ld\n", 9223372036854775807L);
    printf("ULONG_MAX      = %lu\n\n", 18446744073709551615L);
    printf("LLONG_MIN      = %+lld\n", -9223372036854775808LL);
    printf("LLONG_MAX      = %+lld\n", 9223372036854775807LL);
    printf("ULLONG_MAX     = %llu\n\n", 18446744073709551615ULL);
    printf("PTRDIFF_MIN    = %td\n", -9223372036854775808L);
    printf("PTRDIFF_MAX    = %+td\n", 9223372036854775807L);
    printf("SIZE_MAX       = %zu\n", 18446744073709551615L);
    printf("SIG_ATOMIC_MIN = %+jd\n", -2147483648L);
    printf("SIG_ATOMIC_MAX = %+jd\n", 2147483647L);
    printf("WCHAR_MIN      = %+jd\n", -2147483648L);
    printf("WCHAR_MAX      = %+jd\n", 2147483647L);
    printf("WINT_MIN       = %jd\n", 0L);
    printf("WINT_MAX       = %jd\n", 4294967295L);
}


