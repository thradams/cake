// Cake 0.12.05 target=x86_msvc
struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


char *target_name[5] = {"x86_x64_gcc", "x86_msvc", "x64_msvc", "ccu8", "catalina"};
int __cdecl strcmp(char * _Str1, char * _Str2);

int parse_target(char * targetstr, int * target)
{
    {
        int i;

        i = 0;
        for (; i < 5; i++)
        {
            if (strcmp(targetstr + 8, target_name[i]) == 0)
            {
                *target = i;
                return 0;
            }
        }
    }
    return 1;
}


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __Ck0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void print_target_options()
{
    {
        int i;

        i = 0;
        for (; i < 5; i++)
        {
            printf("%s ", target_name[i]);
        }
    }
    printf("\n");
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__Ck0__OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
char *target_intN_suffix(int target, int size)
{
    /*switch*/
    {
        register int __Ck1_temp = target;
        if (__Ck1_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck1_temp == 1) goto _CKL3; /*case 1*/
        if (__Ck1_temp == 2) goto _CKL4; /*case 2*/
        if (__Ck1_temp == 3) goto _CKL6; /*case 3*/
        if (__Ck1_temp == 4) goto _CKL9; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            if (size == 64)
            {
                return "";
            }
            goto _CKL0; /*break*/

            _CKL3: /*case 1*/ 
            _CKL4: /*case 2*/ 
            if (size == 64)
            {
                return "LL";
            }
            goto _CKL0; /*break*/

            _CKL6: /*case 3*/ 
            if (size == 32)
            {
                return "L";
            }
            if (size == 64)
            {
                return "LL";
            }
            goto _CKL0; /*break*/

            _CKL9: /*case 4*/ 
            if (size == 64)
            {
                return "LL";
            }
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return "";
}


char *target_uintN_suffix(int target, int size)
{
    /*switch*/
    {
        register int __Ck2_temp = target;
        if (__Ck2_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck2_temp == 1) goto _CKL4; /*case 1*/
        if (__Ck2_temp == 2) goto _CKL5; /*case 2*/
        if (__Ck2_temp == 3) goto _CKL8; /*case 3*/
        if (__Ck2_temp == 4) goto _CKL11; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            if (size == 32)
            {
                return "U";
            }
            if (size == 64)
            {
                return "U";
            }
            goto _CKL0; /*break*/

            _CKL4: /*case 1*/ 
            _CKL5: /*case 2*/ 
            if (size == 32)
            {
                return "U";
            }
            if (size == 64)
            {
                return "ULL";
            }
            goto _CKL0; /*break*/

            _CKL8: /*case 3*/ 
            if (size == 32)
            {
                return "UL";
            }
            if (size == 64)
            {
                return "ULL";
            }
            goto _CKL0; /*break*/

            _CKL11: /*case 4*/ 
            if (size == 64)
            {
                return "ULL";
            }
            goto _CKL0; /*break*/

        }
        _CKL0:;
    }
    return "";
}


char *target_to_string(int target)
{
    if (target >= 0 && target < 5)
    {
        return target_name[(int)target];
    }
    ;
    return "";
}


unsigned int target_get_wchar_max(int target)
{
    /*switch*/
    {
        register int __Ck3_temp = target;
        if (__Ck3_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck3_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck3_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck3_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck3_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return 2147483647;
            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            return 65535;
            _CKL4: /*case 3*/ 
            return 255;
            _CKL5: /*case 4*/ 
            return 255;
        }
        _CKL0:;
    }
    ;
    return 0;
}


long long target_get_signed_long_max(int target)
{
    /*switch*/
    {
        register int __Ck4_temp = target;
        if (__Ck4_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck4_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck4_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck4_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck4_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return 9223372036854775807LL;
            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            return 2147483647LL;
            _CKL4: /*case 3*/ 
            return 2147483647LL;
            _CKL5: /*case 4*/ 
            return 2147483647LL;
        }
        _CKL0:;
    }
    ;
    return 0;
}


long long target_get_signed_long_long_max(int target)
{
    /*switch*/
    {
        register int __Ck5_temp = target;
        if (__Ck5_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck5_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck5_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck5_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck5_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return 9223372036854775807LL;
            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            return 9223372036854775807LL;
            _CKL4: /*case 3*/ 
            return 2147483647LL;
            _CKL5: /*case 4*/ 
            return 2147483647LL;
        }
        _CKL0:;
    }
    ;
    return 0;
}


unsigned long long target_get_unsigned_long_long_max(int target)
{
    /*switch*/
    {
        register int __Ck6_temp = target;
        if (__Ck6_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck6_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck6_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck6_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck6_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return 18446744073709551615ULL;
            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            return 18446744073709551615ULL;
            _CKL4: /*case 3*/ 
            return 4294967295ULL;
            _CKL5: /*case 4*/ 
            return 4294967295ULL;
        }
        _CKL0:;
    }
    ;
    return 0;
}


unsigned long long target_get_unsigned_long_max(int target)
{
    /*switch*/
    {
        register int __Ck7_temp = target;
        if (__Ck7_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck7_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck7_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck7_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck7_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return 18446744073709551615ULL;
            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            return 4294967295U;
            _CKL4: /*case 3*/ 
            return 4294967295ULL;
            _CKL5: /*case 4*/ 
            return 4294967295U;
        }
        _CKL0:;
    }
    ;
    return 0;
}


long long target_get_signed_int_max(int target)
{
    /*switch*/
    {
        register int __Ck8_temp = target;
        if (__Ck8_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck8_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck8_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck8_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck8_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return 2147483647LL;
            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            return 2147483647LL;
            _CKL4: /*case 3*/ 
            return 32767ULL;
            _CKL5: /*case 4*/ 
            return 2147483647LL;
        }
        _CKL0:;
    }
    ;
    return 0;
}


unsigned long long target_get_unsigned_int_max(int target)
{
    /*switch*/
    {
        register int __Ck9_temp = target;
        if (__Ck9_temp == 0) goto _CKL1; /*case 0*/
        if (__Ck9_temp == 1) goto _CKL2; /*case 1*/
        if (__Ck9_temp == 2) goto _CKL3; /*case 2*/
        if (__Ck9_temp == 3) goto _CKL4; /*case 3*/
        if (__Ck9_temp == 4) goto _CKL5; /*case 4*/
        goto _CKL0;

        {
            _CKL1: /*case 0*/ 
            return 4294967295ULL;
            _CKL2: /*case 1*/ 
            _CKL3: /*case 2*/ 
            return 4294967295ULL;
            _CKL4: /*case 3*/ 
            return 65535ULL;
            _CKL5: /*case 4*/ 
            return 4294967295ULL;
        }
        _CKL0:;
    }
    ;
    return 0;
}


char * TARGET_X86_X64_GCC_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define __linux__\n#define __GNUC__  16\n#define __x86_64__ 1\n#define __CHAR_BIT__ 8\n#define __SIZE_TYPE__ long unsigned int\n#define __PTRDIFF_TYPE__ long int\n#define __WCHAR_TYPE__ int\n#define __WINT_TYPE__ unsigned int\n#define __INTMAX_TYPE__ long int\n#define __UINTMAX_TYPE__ long unsigned int\n#define __SIG_ATOMIC_TYPE__ int\n#define __INT8_TYPE__ signed char\n#define __INT16_TYPE__ short int\n#define __INT32_TYPE__ int\n#define __INT64_TYPE__ long int\n#define __UINT8_TYPE__ unsigned char\n#define __UINT16_TYPE__ short unsigned int\n#define __UINT32_TYPE__ unsigned int\n#define __UINT64_TYPE__ long unsigned int\n#define __INT_LEAST8_TYPE__ signed char\n#define __INT_LEAST16_TYPE__ short int\n#define __INT_LEAST32_TYPE__ int\n#define __INT_LEAST64_TYPE__ long int\n#define __UINT_LEAST8_TYPE__ unsigned char\n#define __UINT_LEAST16_TYPE__ short unsigned int\n#define __UINT_LEAST32_TYPE__ unsigned int\n#define __UINT_LEAST64_TYPE__ long unsigned int\n#define __INT_FAST8_TYPE__ signed char\n#define __INT_FAST16_TYPE__ long int\n#define __INT_FAST32_TYPE__ long int\n#define __INT_FAST64_TYPE__ long int\n#define __UINT_FAST8_TYPE__ unsigned char\n#define __UINT_FAST16_TYPE__ long unsigned int\n#define __UINT_FAST32_TYPE__ long unsigned int\n#define __UINT_FAST64_TYPE__ long unsigned int\n#define __INTPTR_TYPE__ long int\n#define __UINTPTR_TYPE__ long unsigned int\n#define __DBL_MAX__ ((double)1.79769313486231570814527423731704357e+308L)\n#define __DBL_MIN__ ((double)2.22507385850720138309023271733240406e-308L)\n#define __FLT_RADIX__ 2\n#define __FLT_EPSILON__ 1.19209289550781250000000000000000000e-7F\n#define __DBL_EPSILON__ ((double)2.22044604925031308084726333618164062e-16L)\n#define __LDBL_EPSILON__ 1.08420217248550443400745280086994171e-19L\n#define __DBL_DECIMAL_DIG__ 17\n#define __FLT_EVAL_METHOD__ 0\n#define __FLT_RADIX__ 2\n#define __DBL_MAX_EXP__ 1024\n#define __DECIMAL_DIG__ 21\n#define __FLT_DECIMAL_DIG__ 9\n#define __FLT_MIN_10_EXP__ (-37)\n#define __FLT_MIN__ 1.17549435082228750796873653722224568e-38F\n#define __FLT_MAX__ 3.40282346638528859811704183484516925e+38F\n#define __FLT_EPSILON__ 1.19209289550781250000000000000000000e-7F\n#define __FLT_DIG__ 6\n#define __FLT_MANT_DIG__ 24\n#define __FLT_MIN_EXP__ (-125)\n#define __FLT_MAX_10_EXP__ 38\n#define __FLT_ROUNDS__ __FLT_ROUNDS__\n#define __FLT_EVAL_METHOD__ 0\n#define __FLT_HAS_SUBNORM__ __FLT_HAS_SUBNORM__\n#define __FLT_MAX_EXP__ 128\n#define __FLT_HAS_DENORM__ 1\n#define __SCHAR_MAX__ 0x7f\n#define __WCHAR_MAX__ 0x7fffffff\n#define __SHRT_MAX__ 0x7fff\n#define __INT_MAX__ 0x7fffffff\n#define __LONG_MAX__ 0x7fffffffffffffffL\n#define __LONG_LONG_MAX__ 0x7fffffffffffffffLL\n#define __WINT_MAX__ 0xffffffffU\n#define __SIZE_MAX__ 0xffffffffffffffffUL\n#define __PTRDIFF_MAX__ 0x7fffffffffffffffL\n#define __INTMAX_MAX__ 0x7fffffffffffffffL\n#define __UINTMAX_MAX__ 0xffffffffffffffffUL\n#define __SIG_ATOMIC_MAX__ 0x7fffffff\n#define __INT8_MAX__ 0x7f\n#define __INT16_MAX__ 0x7fff\n#define __INT32_MAX__ 0x7fffffff\n#define __INT64_MAX__ 0x7fffffffffffffffL\n#define __UINT8_MAX__ 0xff\n#define __UINT16_MAX__ 0xffff\n#define __UINT32_MAX__ 0xffffffffU\n#define __UINT64_MAX__ 0xffffffffffffffffUL\n#define __INT_LEAST8_MAX__ 0x7f\n#define __INT_LEAST16_MAX__ 0x7fff\n#define __INT_LEAST32_MAX__ 0x7fffffff\n#define __INT_LEAST64_MAX__ 0x7fffffffffffffffL\n#define __UINT_LEAST8_MAX__ 0xff\n#define __UINT_LEAST16_MAX__ 0xffff\n#define __UINT_LEAST32_MAX__ 0xffffffffU\n#define __UINT_LEAST64_MAX__ 0xffffffffffffffffUL\n#define __INT_FAST8_MAX__ 0x7f\n#define __INT_FAST16_MAX__ 0x7fffffffffffffffL\n#define __INT_FAST32_MAX__ 0x7fffffffffffffffL\n#define __INT_FAST64_MAX__ 0x7fffffffffffffffL\n#define __UINT_FAST8_MAX__ 0xff\n#define __UINT_FAST16_MAX__ 0xffffffffffffffffUL\n#define __UINT_FAST32_MAX__ 0xffffffffffffffffUL\n#define __UINT_FAST64_MAX__ 0xffffffffffffffffUL\n#define __INTPTR_MAX__ 0x7fffffffffffffffL\n#define __UINTPTR_MAX__ 0xffffffffffffffffUL\n#define __WCHAR_MIN__ (-0x7fffffff - 1)\n#define __WINT_MIN__ 0U\n#define __SIG_ATOMIC_MIN__ (-0x7fffffff - 1)\n#define __INT8_C (-0x7fffffff - 1)\n#define __INT16_C __INT16_C\n#define __INT32_C __INT32_C\n#define __INT64_C __INT64_C\n#define __UINT8_C __UINT8_C\n#define __UINT16_C __UINT16_C\n#define __UINT32_C __UINT32_C\n#define __UINT64_C __UINT64_C\n#define __INTMAX_C __INTMAX_C\n#define __UINTMAX_C __UINTMAX_C\n#define __SCHAR_WIDTH__ 8\n#define __SHRT_WIDTH__ 16\n#define __INT_WIDTH__ 32\n#define __LONG_WIDTH__ 64\n#define __LONG_LONG_WIDTH__ 64\n#define __PTRDIFF_WIDTH__ 64\n#define __SIG_ATOMIC_WIDTH__ 32\n#define __SIZE_WIDTH__ 64\n#define __WCHAR_WIDTH__ 32\n#define __WINT_WIDTH__ 32\n#define __INT_LEAST8_WIDTH__ 8\n#define __INT_LEAST16_WIDTH__ 16\n#define __INT_LEAST32_WIDTH__ 32\n#define __INT_LEAST64_WIDTH__ 64\n#define __INT_FAST8_WIDTH__ 8\n#define __INT_FAST16_WIDTH__ 64\n#define __INT_FAST32_WIDTH__ 64\n#define __INT_FAST64_WIDTH__ 64\n#define __INTPTR_WIDTH__ 64\n#define __INTMAX_WIDTH__ 64\n#define __SIZEOF_INT__ 4\n#define __SIZEOF_LONG__ 8\n#define __SIZEOF_LONG_LONG__ 8\n#define __SIZEOF_SHORT__ 2\n#define __SIZEOF_POINTER__ 8\n#define __SIZEOF_FLOAT__ 4\n#define __SIZEOF_DOUBLE__ 8\n#define __SIZEOF_LONG_DOUBLE__ 16\n#define __SIZEOF_SIZE_T__ 8\n#define __SIZEOF_WCHAR_T__ 4\n#define __SIZEOF_WINT_T__ 4\n#define __SIZEOF_PTRDIFF_T__ 8\n";
char * TARGET_X86_MSVC_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define _WIN32 1\n#define _INTEGRAL_MAX_BITS 64\n#define _MSC_VER 1944\n#define _M_IX86 600\n#define __pragma(a)\n\n";
char * TARGET_X64_MSVC_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define _WIN32 1\n#define _WIN64 1\n#define _INTEGRAL_MAX_BITS 64\n#define _MSC_VER 1944\n#define _M_X64 100\n#define __pragma(a)\n\n";
char * TARGET_CCU8_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n\n";
char * TARGET_CATALINA_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define ___CATALINA__\n#define ___CATALYST__\n\n";
