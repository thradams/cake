/* Cake 0.13.29 x86_msvc */

struct _iobuf {
    void * _Placeholder;
};
__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;

char *__cdecl setlocale(int _Category, char * _Locale);
int *__cdecl _errno(void);
unsigned short __cdecl fputwc(unsigned short _Character, struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl puts(char * _Buffer);

int main(void)
{
    setlocale(0, "en_US.utf8");
    (*_errno()) = 0;
    if (fputwc(62284, (__acrt_iob_func(1))) == 65535)
    {
        if ((*_errno()) == 42)
        {
            puts("Encoding error in fputwc.");
        }
        else
        {
            puts("I/O error in fputwc.");
        }
        return 1;
    }
}
