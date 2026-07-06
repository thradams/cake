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

static void memcpy2(void * dest, const void * src, unsigned long long n);
__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;

static unsigned short *__cdecl wmemchr(unsigned short * _S, unsigned short _C, unsigned long long _N);
extern int _Avx2WmemEnabled;
extern union __m256i __cdecl _mm256_broadcastw_epi16(union __m128i);
extern union __m128i _mm_cvtsi32_si128(int _A);
extern union __m256i __cdecl _mm256_loadu_si256(union __m256i *);
extern union __m256i __cdecl _mm256_cmpeq_epi16(union __m256i, union __m256i);
extern int __cdecl _mm256_movemask_epi8(union __m256i);
unsigned char _BitScanForward(unsigned long * _Index, unsigned long _Mask);
extern union __m128i _mm_set1_epi16(short _W);
extern union __m128i _mm_loadu_si128(union __m128i * _P);
extern union __m128i _mm_cmpeq_epi16(union __m128i _A, union __m128i _B);
extern int _mm_movemask_epi8(union __m128i _A);
extern union __m128i _mm_loadl_epi64(union __m128i * _P);
char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    unsigned short str[23];
    unsigned long long sz;
    unsigned short target;
    unsigned short * result;

    memcpy2(str, L"\xe8\xaf\xba\xe4\xb8\x8d\xe8\xbd\xbb\xe4\xbf\xa1\xef\xbc\x8c\xe6\x95\x85\xe4\xba\xba\xe4\xb8\x8d\xe8\xb4\x9f\xe6\x88\x91\0\xe8\xaf\xba\xe4\xb8\x8d\xe8\xbd\xbb\xe8\xae\xb8\xef\xbc\x8c\xe6\x95\x85\xe6\x88\x91\xe4\xb8\x8d\xe8\xb4\x9f\xe4\xba\xba\xe3\x80\x82", 23);
    sz = 23ULL;
    target = 35768;
    result = wmemchr(str, target, sz);
    if (result)
    {
        setlocale(0, "en_US.utf8");
        printf("Found '%lc' at position %td\n", target, result - str);
    }
}

static unsigned short *__cdecl wmemchr(unsigned short * _S, unsigned short _C, unsigned long long _N)
{
    unsigned long long __count;
    unsigned long __index;
    unsigned short * __s;

    __count = 0;
    __index = 0;
    __s = _S;
    if (_Avx2WmemEnabled && _N >= 16)
    {
        union __m256i __v2;

        __v2 = _mm256_broadcastw_epi16(_mm_cvtsi32_si128(_C));
        do
        {
            union __m256i __v1;
            unsigned int __mask;

            __v1 = _mm256_loadu_si256((union __m256i *)__s);
            __v1 = _mm256_cmpeq_epi16(__v1, __v2);
            __mask = (unsigned int)_mm256_movemask_epi8(__v1);
            if (__mask != 0)
            {
                _BitScanForward(&__index, __mask);
                __index >>= 1;
                return (unsigned short *)&_S[__count + __index];
            }
            __count += 16;
            __s += 16;
        }
        while (__count + 16 <= _N);
    }
    if (__count + 4 <= _N)
    {
        union __m128i __v2;

        __v2 = _mm_set1_epi16((short)_C);
        while (__count + 8 <= _N)
        {
            union __m128i __v1;
            unsigned short __mask;

            __v1 = _mm_loadu_si128((union __m128i *)__s);
            __v1 = _mm_cmpeq_epi16(__v1, __v2);
            __mask = (unsigned short)_mm_movemask_epi8(__v1);
            if (__mask != 0)
            {
                _BitScanForward(&__index, __mask);
                __index >>= 1;
                return (unsigned short *)&_S[__count + __index];
            }
            __count += 8;
            __s += 8;
        }
        if (__count + 4 <= _N)
        {
            union __m128i __v1;
            unsigned char __mask;

            __v1 = _mm_loadl_epi64((union __m128i *)(__s));
            __v1 = _mm_cmpeq_epi16(__v1, __v2);
            __mask = (unsigned char)_mm_movemask_epi8(__v1);
            if (__mask != 0)
            {
                _BitScanForward(&__index, __mask);
                __index >>= 1;
                return (unsigned short *)&_S[__count + __index];
            }
            __count += 4;
        }
    }
    for (; __count < _N; ++__count)
    {
        if (_S[__count] == _C)
        {
            return (unsigned short *)&_S[__count];
        }
    }
    return 0;
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static void memcpy2(void * dest, const void * src, unsigned long long n)
{
    char *csrc;
    char *cdest;
    unsigned long long i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}

