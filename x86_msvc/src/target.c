/* Cake 0.12.26 x86_msvc */
struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct platform {
    char * name;
    char * thread_local_attr;
    char * alignas_fmt_must_have_one_percent_d;
    int bool_n_bits;
    int bool_aligment;
    int bool_type;
    int char_n_bits;
    int char_t_type;
    int char_aligment;
    int short_n_bits;
    int short_aligment;
    int int_n_bits;
    int int_aligment;
    int long_n_bits;
    int long_aligment;
    int long_long_n_bits;
    int long_long_aligment;
    int float_n_bits;
    int float_aligment;
    int double_n_bits;
    int double_aligment;
    int long_double_n_bits;
    int long_double_aligment;
    int pointer_n_bits;
    int pointer_aligment;
    int wchar_t_type;
    int int8_type;
    int int16_type;
    int int32_type;
    int int64_type;
    int size_t_type;
    int ptrdiff_type;
};


char * TARGET_X86_X64_GCC_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define __linux__\n#define __x86_64__ 1\n#define __CHAR_BIT__ 8\n#define __SIZE_TYPE__ long unsigned int\n#define __PTRDIFF_TYPE__ long int\n#define __WCHAR_TYPE__ int\n#define __WINT_TYPE__ unsigned int\n#define __INTMAX_TYPE__ long int\n#define __UINTMAX_TYPE__ long unsigned int\n#define __SIG_ATOMIC_TYPE__ int\n#define __INT8_TYPE__ signed char\n#define __INT16_TYPE__ short int\n#define __INT32_TYPE__ int\n#define __INT64_TYPE__ long int\n#define __UINT8_TYPE__ unsigned char\n#define __UINT16_TYPE__ short unsigned int\n#define __UINT32_TYPE__ unsigned int\n#define __UINT64_TYPE__ long unsigned int\n#define __INT_LEAST8_TYPE__ signed char\n#define __INT_LEAST16_TYPE__ short int\n#define __INT_LEAST32_TYPE__ int\n#define __INT_LEAST64_TYPE__ long int\n#define __UINT_LEAST8_TYPE__ unsigned char\n#define __UINT_LEAST16_TYPE__ short unsigned int\n#define __UINT_LEAST32_TYPE__ unsigned int\n#define __UINT_LEAST64_TYPE__ long unsigned int\n#define __INT_FAST8_TYPE__ signed char\n#define __INT_FAST16_TYPE__ long int\n#define __INT_FAST32_TYPE__ long int\n#define __INT_FAST64_TYPE__ long int\n#define __UINT_FAST8_TYPE__ unsigned char\n#define __UINT_FAST16_TYPE__ long unsigned int\n#define __UINT_FAST32_TYPE__ long unsigned int\n#define __UINT_FAST64_TYPE__ long unsigned int\n#define __INTPTR_TYPE__ long int\n#define __UINTPTR_TYPE__ long unsigned int\n#define __DBL_MAX__ ((double)1.79769313486231570814527423731704357e+308L)\n#define __DBL_MIN__ ((double)2.22507385850720138309023271733240406e-308L)\n#define __FLT_RADIX__ 2\n#define __FLT_EPSILON__ 1.19209289550781250000000000000000000e-7F\n#define __DBL_EPSILON__ ((double)2.22044604925031308084726333618164062e-16L)\n#define __LDBL_EPSILON__ 1.08420217248550443400745280086994171e-19L\n#define __DBL_DECIMAL_DIG__ 17\n#define __FLT_EVAL_METHOD__ 0\n#define __FLT_RADIX__ 2\n#define __DBL_MAX_EXP__ 1024\n#define __DECIMAL_DIG__ 21\n#define __FLT_DECIMAL_DIG__ 9\n#define __FLT_MIN_10_EXP__ (-37)\n#define __FLT_MIN__ 1.17549435082228750796873653722224568e-38F\n#define __FLT_MAX__ 3.40282346638528859811704183484516925e+38F\n#define __FLT_EPSILON__ 1.19209289550781250000000000000000000e-7F\n#define __FLT_DIG__ 6\n#define __FLT_MANT_DIG__ 24\n#define __FLT_MIN_EXP__ (-125)\n#define __FLT_MAX_10_EXP__ 38\n#define __FLT_ROUNDS__ __FLT_ROUNDS__\n#define __FLT_EVAL_METHOD__ 0\n#define __FLT_HAS_SUBNORM__ __FLT_HAS_SUBNORM__\n#define __FLT_MAX_EXP__ 128\n#define __FLT_HAS_DENORM__ 1\n#define __SCHAR_MAX__ 0x7f\n#define __WCHAR_MAX__ 0x7fffffff\n#define __SHRT_MAX__ 0x7fff\n#define __INT_MAX__ 0x7fffffff\n#define __LONG_MAX__ 0x7fffffffffffffffL\n#define __LONG_LONG_MAX__ 0x7fffffffffffffffLL\n#define __WINT_MAX__ 0xffffffffU\n#define __SIZE_MAX__ 0xffffffffffffffffUL\n#define __PTRDIFF_MAX__ 0x7fffffffffffffffL\n#define __INTMAX_MAX__ 0x7fffffffffffffffL\n#define __UINTMAX_MAX__ 0xffffffffffffffffUL\n#define __SIG_ATOMIC_MAX__ 0x7fffffff\n#define __INT8_MAX__ 0x7f\n#define __INT16_MAX__ 0x7fff\n#define __INT32_MAX__ 0x7fffffff\n#define __INT64_MAX__ 0x7fffffffffffffffL\n#define __UINT8_MAX__ 0xff\n#define __UINT16_MAX__ 0xffff\n#define __UINT32_MAX__ 0xffffffffU\n#define __UINT64_MAX__ 0xffffffffffffffffUL\n#define __INT_LEAST8_MAX__ 0x7f\n#define __INT_LEAST16_MAX__ 0x7fff\n#define __INT_LEAST32_MAX__ 0x7fffffff\n#define __INT_LEAST64_MAX__ 0x7fffffffffffffffL\n#define __UINT_LEAST8_MAX__ 0xff\n#define __UINT_LEAST16_MAX__ 0xffff\n#define __UINT_LEAST32_MAX__ 0xffffffffU\n#define __UINT_LEAST64_MAX__ 0xffffffffffffffffUL\n#define __INT_FAST8_MAX__ 0x7f\n#define __INT_FAST16_MAX__ 0x7fffffffffffffffL\n#define __INT_FAST32_MAX__ 0x7fffffffffffffffL\n#define __INT_FAST64_MAX__ 0x7fffffffffffffffL\n#define __UINT_FAST8_MAX__ 0xff\n#define __UINT_FAST16_MAX__ 0xffffffffffffffffUL\n#define __UINT_FAST32_MAX__ 0xffffffffffffffffUL\n#define __UINT_FAST64_MAX__ 0xffffffffffffffffUL\n#define __INTPTR_MAX__ 0x7fffffffffffffffL\n#define __UINTPTR_MAX__ 0xffffffffffffffffUL\n#define __WCHAR_MIN__ (-0x7fffffff - 1)\n#define __WINT_MIN__ 0U\n#define __SIG_ATOMIC_MIN__ (-0x7fffffff - 1)\n#define __INT8_C (-0x7fffffff - 1)\n#define __INT16_C __INT16_C\n#define __INT32_C __INT32_C\n#define __INT64_C __INT64_C\n#define __UINT8_C __UINT8_C\n#define __UINT16_C __UINT16_C\n#define __UINT32_C __UINT32_C\n#define __UINT64_C __UINT64_C\n#define __INTMAX_C __INTMAX_C\n#define __UINTMAX_C __UINTMAX_C\n#define __SCHAR_WIDTH__ 8\n#define __SHRT_WIDTH__ 16\n#define __INT_WIDTH__ 32\n#define __LONG_WIDTH__ 64\n#define __LONG_LONG_WIDTH__ 64\n#define __PTRDIFF_WIDTH__ 64\n#define __SIG_ATOMIC_WIDTH__ 32\n#define __SIZE_WIDTH__ 64\n#define __WCHAR_WIDTH__ 32\n#define __WINT_WIDTH__ 32\n#define __INT_LEAST8_WIDTH__ 8\n#define __INT_LEAST16_WIDTH__ 16\n#define __INT_LEAST32_WIDTH__ 32\n#define __INT_LEAST64_WIDTH__ 64\n#define __INT_FAST8_WIDTH__ 8\n#define __INT_FAST16_WIDTH__ 64\n#define __INT_FAST32_WIDTH__ 64\n#define __INT_FAST64_WIDTH__ 64\n#define __INTPTR_WIDTH__ 64\n#define __INTMAX_WIDTH__ 64\n#define __SIZEOF_INT__ 4\n#define __SIZEOF_LONG__ 8\n#define __SIZEOF_LONG_LONG__ 8\n#define __SIZEOF_SHORT__ 2\n#define __SIZEOF_POINTER__ 8\n#define __SIZEOF_FLOAT__ 4\n#define __SIZEOF_DOUBLE__ 8\n#define __SIZEOF_LONG_DOUBLE__ 16\n#define __SIZEOF_SIZE_T__ 8\n#define __SIZEOF_WCHAR_T__ 4\n#define __SIZEOF_WINT_T__ 4\n#define __SIZEOF_PTRDIFF_T__ 8\n";
char * TARGET_X86_MSVC_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define _WIN32 1\n#define _INTEGRAL_MAX_BITS 64\n#define _MSC_VER 1944\n#define _M_IX86 600\n#define __pragma(a)\n\n";
char * TARGET_X64_MSVC_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define _WIN32 1\n#define _WIN64 1\n#define _INTEGRAL_MAX_BITS 64\n#define _MSC_VER 1944\n#define _M_X64 100\n#define __pragma(a)\n\n";
char * TARGET_CCU8_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define __EI()\n#define __DI()\n#define __asm()\n#define __SEGBASE_N()\n#define __SEGBASE_F()\n#define __SEGSIZE()\n#define __near\n#define __far\n#define __huge\n#define __PACKED\n#define __UNPACKED\n#define __noreg\n#define __STDC__\n#define __CCU8__\n#define __VERSION__\n#define __ARCHITECTURE__\n#define __DEBUG__\n#define __MS__\n#define __ML__\n#define __ML__\n#define __UNSIGNEDCHAR__\n#define __NOFAR__\n#define __LCCU16__\n#define __LAPISOMF__\n\n";
char * TARGET_LCCU16_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define __EI()\n#define __DI()\n#define __asm()\n#define __SEGBASE_N()\n#define __SEGBASE_F()\n#define __SEGSIZE()\n#define __near\n#define __far\n#define __huge\n#define __PACKED\n#define __UNPACKED\n#define __noreg\n#define __STDC__\n#define __CCU8__\n#define __VERSION__\n#define __ARCHITECTURE__\n#define __DEBUG__\n#define __MS__\n#define __ML__\n#define __ML__\n#define __UNSIGNEDCHAR__\n#define __NOFAR__\n#define __LCCU16__\n#define __LAPISOMF__\n\n";
char * TARGET_CATALINA_PREDEFINED_MACROS = "#define __CAKE__ 202311L\n#define __STDC_VERSION__ 202311L\n#define __FILE__ \"__FILE__\"\n#define __LINE__ 0\n#define __COUNTER__ 0\n#define _CONSOLE\n#define __STDC_OWNERSHIP__ 1\n#define __STDC_NO_ATOMICS__ 1\n#define __STDC_NO_COMPLEX__  1\n#define __STDC_NO_THREADS__   __STDC_NO_THREADS__\n#define __STDC_NO_VLA__    1\n#define ___CATALINA__\n#define ___CATALYST__\n\n";
int __cdecl strcmp(char * _Str1, char * _Str2);
static struct platform *platforms[6] = {0 /*?*/,0 /*?*/,0 /*?*/,0 /*?*/,0 /*?*/,0 /*?*/};


int parse_target(char * targetstr, int * target)
{
    {
        int i;

        i = 0;
        for (; i < 6; i++)
        {
            if (strcmp(targetstr, platforms[i]->name) == 0)
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
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void print_target_options()
{
    {
        int i;

        i = 0;
        for (; i < 6; i++)
        {
            printf("%s ", platforms[i]->name);
        }
    }
    printf("\n");
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}
struct platform *get_platform(int target)
{
    return platforms[target];
}


int get_num_of_bits(int target, int type)
{
    /*switch*/
    {
        int __v0 = type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 1) goto __L2; /*case 1*/
        if (__v0 == 2) goto __L3; /*case 2*/
        if (__v0 == 3) goto __L4; /*case 3*/
        if (__v0 == 4) goto __L5; /*case 4*/
        if (__v0 == 6) goto __L6; /*case 6*/
        if (__v0 == 5) goto __L7; /*case 5*/
        if (__v0 == 7) goto __L8; /*case 7*/
        if (__v0 == 8) goto __L9; /*case 8*/
        if (__v0 == 9) goto __L10; /*case 9*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 1*/ 
            return get_platform(target)->char_n_bits;
            __L3: /*case 2*/ 
            __L4: /*case 3*/ 
            return get_platform(target)->short_n_bits;
            __L5: /*case 4*/ 
            __L6: /*case 6*/ 
            __L7: /*case 5*/ 
            __L8: /*case 7*/ 
            return get_platform(target)->int_n_bits;
            __L9: /*case 8*/ 
            __L10: /*case 9*/ 
            return get_platform(target)->long_long_n_bits;
        }
        __L0:;
    }
    return 0;
}


char *target_get_predefined_macros(int e)
{
    /*switch*/
    {
        int __v0 = e;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 1) goto __L2; /*case 1*/
        if (__v0 == 2) goto __L3; /*case 2*/
        if (__v0 == 3) goto __L4; /*case 3*/
        if (__v0 == 4) goto __L5; /*case 4*/
        if (__v0 == 5) goto __L6; /*case 5*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            return TARGET_X86_X64_GCC_PREDEFINED_MACROS;
            __L2: /*case 1*/ 
            return TARGET_X86_MSVC_PREDEFINED_MACROS;
            __L3: /*case 2*/ 
            return TARGET_X64_MSVC_PREDEFINED_MACROS;
            __L4: /*case 3*/ 
            return TARGET_CCU8_PREDEFINED_MACROS;
            __L5: /*case 4*/ 
            return TARGET_LCCU16_PREDEFINED_MACROS;
            __L6: /*case 5*/ 
            return TARGET_CATALINA_PREDEFINED_MACROS;
        }
        __L0:;
    }
    return "";
}


