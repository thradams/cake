struct _iobuf {
    void * _Placeholder;
};


int _Avx2WmemEnabledWeakValue = 0;
char *__cdecl setlocale(int _Category, char * _Locale);
int __cdecl fputws(unsigned short * _Buffer, struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
void __cdecl perror(char * _ErrorMessage);

int main(void)
{
    int rc;

    setlocale(0, "en_US.utf8");
    rc = fputws(L"\xe5\xbe\xa1\xe4\xbc\x91\xe3\x81\xbf\xe3\x81\xaa\xe3\x81\x95\xe3\x81\x84", (__acrt_iob_func(1)));
    if (rc == -1)
    {
        perror("fputws()");
    }
}

