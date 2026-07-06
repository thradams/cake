/* Cake 0.13.29 x86_msvc */

static void memcpy2(void * dest, const void * src, unsigned int n);
__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;

static unsigned short *__cdecl wmemset(unsigned short * _S, unsigned short _C, unsigned int _N);
char *__cdecl setlocale(int _Category, char * _Locale);
unsigned short __cdecl putwchar(unsigned short _Character);

int main(void)
{
    unsigned short ar[10];

    memcpy2(ar, L"1234567890", 10);
    wmemset(ar, 62284, 5);
    wmemset(ar + 5, 34121, 5);
    setlocale(0, "en_US.utf8");
    {
        unsigned int n;

        n = 0;
        for (; n < 10; ++n)
        {
            putwchar(ar[n]);
        }
    }
    putwchar(10);
}

static unsigned short *__cdecl wmemset(unsigned short * _S, unsigned short _C, unsigned int _N)
{
    unsigned short * _Su;

    _Su = _S;
    for (; 0 < _N; ++_Su, --_N)
    {
        *_Su = _C;
    }
    return _S;
}

static void memcpy2(void * dest, const void * src, unsigned int n)
{
    char *csrc;
    char *cdest;
    unsigned int i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}

