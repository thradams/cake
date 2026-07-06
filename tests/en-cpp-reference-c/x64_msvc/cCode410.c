/* Cake x64_msvc */

union __m256i {
    char m256i_i8[32];
    short m256i_i16[16];
    int m256i_i32[8];
    long long m256i_i64[4];
    unsigned char m256i_u8[32];
    unsigned short m256i_u16[16];
    unsigned int m256i_u32[8];
    unsigned long long m256i_u64[4];
};

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

union __m128i {
    char m128i_i8[16];
    short m128i_i16[8];
    int m128i_i32[4];
    long long m128i_i64[2];
    unsigned char m128i_u8[16];
    unsigned short m128i_u16[8];
    unsigned int m128i_u32[4];
    unsigned long long m128i_u64[2];
};
__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;

unsigned short __cdecl putwchar(unsigned short _Character);
static int __cdecl wmemcmp(unsigned short * _S1, unsigned short * _S2, unsigned long long _N);
extern int _Avx2WmemEnabled;
extern union __m256i __cdecl _mm256_loadu_si256(union __m256i *);
extern union __m256i __cdecl _mm256_cmpeq_epi16(union __m256i, union __m256i);
extern int __cdecl _mm256_movemask_epi8(union __m256i);
unsigned char _BitScanForward(unsigned long * _Index, unsigned long _Mask);
extern union __m128i _mm_loadu_si128(union __m128i * _P);
extern union __m128i _mm_cmpeq_epi16(union __m128i _A, union __m128i _B);
extern int _mm_movemask_epi8(union __m128i _A);
unsigned char _BitScanForward64(unsigned long * _Index, unsigned long long _Mask);
static int __cdecl wprintf(unsigned short * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfwprintf(unsigned long long _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void demo(unsigned short * lhs, unsigned short * rhs, unsigned long long sz)
{
    int rc;

    {
        unsigned long long n;

        n = 0;
        for (; n < sz; ++n)
        {
            putwchar(lhs[n]);
        }
    }
    rc = wmemcmp(lhs, rhs, sz);
    if (rc == 0)
    {
        wprintf(L" compares equal to ");
    }
    else
    {
        if (rc < 0)
        {
            wprintf(L" precedes ");
        }
        else
        {
            if (rc > 0)
            {
                wprintf(L" follows ");
            }
        }
    }
    {
        unsigned long long n;

        n = 0;
        for (; n < sz; ++n)
        {
            putwchar(rhs[n]);
        }
    }
    wprintf(L" in lexicographical order\n");
}

static int __cdecl wmemcmp(unsigned short * _S1, unsigned short * _S2, unsigned long long _N)
{
    unsigned long long __count;
    unsigned long __index;
    unsigned short * __s1;
    unsigned short * __s2;

    __count = 0;
    __index = 0;
    __s1 = _S1;
    __s2 = _S2;
    if (_Avx2WmemEnabled)
    {
        while (__count + 16 <= _N)
        {
            union __m256i __v1;
            union __m256i __v2;
            unsigned int __mask;

            __v1 = _mm256_loadu_si256((union __m256i *)__s1);
            __v2 = _mm256_loadu_si256((union __m256i *)__s2);
            __v1 = _mm256_cmpeq_epi16(__v1, __v2);
            __mask = (unsigned int)_mm256_movemask_epi8(__v1);
            if (__mask != 4294967295)
            {
                _BitScanForward(&__index, ~__mask);
                __index >>= 1;
                return _S1[__count + __index] < _S2[__count + __index] ? -1 : 1;
            }
            __count += 16;
            __s1 += 16;
            __s2 += 16;
        }
    }
    while (__count + 8 <= _N)
    {
        union __m128i __v1;
        union __m128i __v2;
        unsigned short __mask;

        __v1 = _mm_loadu_si128((union __m128i *)__s1);
        __v2 = _mm_loadu_si128((union __m128i *)__s2);
        __v1 = _mm_cmpeq_epi16(__v1, __v2);
        __mask = (unsigned short)_mm_movemask_epi8(__v1);
        if (__mask != 65535)
        {
            _BitScanForward(&__index, (unsigned long)~__mask);
            __index >>= 1;
            return _S1[__count + __index] < _S2[__count + __index] ? -1 : 1;
        }
        __count += 8;
        __s1 += 8;
        __s2 += 8;
    }
    if (__count + 4 <= _N)
    {
        unsigned long long __v1;
        unsigned long long __v2;

        __v1 = *(unsigned long long *)__s1;
        __v2 = *(unsigned long long *)__s2;
        if (__v1 != __v2)
        {
            _BitScanForward64(&__index, (__v1 ^ __v2));
            __index >>= 4;
            return _S1[__count + __index] < _S2[__count + __index] ? -1 : 1;
        }
        __count += 4;
    }
    for (; __count < _N; ++__count)
    {
        if (_S1[__count] != _S2[__count])
        {
            return _S1[__count] < _S2[__count] ? -1 : 1;
        }
    }
    return 0;
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
}

static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl wprintf(unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfwprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    unsigned short a1[3];
    unsigned short a2[3];
    unsigned long long sz;

    setlocale(0, "en_US.utf8");
    a1[0] = 945;
    a1[1] = 946;
    a1[2] = 947;
    a2[0] = 945;
    a2[1] = 946;
    a2[2] = 948;
    sz = 3ULL;
    demo(a1, a2, sz);
    demo(a2, a1, sz);
    demo(a1, a1, sz);
}
