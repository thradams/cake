struct _iobuf {
    void * _Placeholder;
};


int _Avx2WmemEnabledWeakValue = 0;
char *__cdecl setlocale(int _Category, char * _Locale);
struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
void __cdecl perror(char * _ErrorMessage);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
int *__cdecl _errno(void);
unsigned short __cdecl fgetwc(struct _iobuf * _Stream);
unsigned short __cdecl putwchar(unsigned short _Character);
int __cdecl ferror(struct _iobuf * _Stream);
int __cdecl puts(char * _Buffer);
int __cdecl feof(struct _iobuf * _Stream);

int main(void)
{
    struct _iobuf * fp;
    unsigned short wc;

    setlocale(0, "en_US.utf8");
    fp = fopen("fgetwc.dat", "w");
    if (!fp)
    {
        perror("Can't open file for writing");
        return 1;
    }
    fputs("\xd0\xba\xd0\xbe\xd1\x88\xd0\xba\xd0\xb0\n", fp);
    fclose(fp);
    fp = fopen("fgetwc.dat", "r");
    if (!fp)
    {
        perror("Can't open file for reading");
        return 1;
    }
    (*_errno()) = 0;
    while ((wc = fgetwc(fp)) != 65535)
    putwchar(wc);
    if (ferror(fp))
    {
        if ((*_errno()) == 42)
        {
            puts("Character encoding error while reading.");
        }
        else
        {
            puts("I/O error when reading");
        }
    }
    else
    {
        if (feof(fp))
        {
            puts("End of file reached successfully");
        }
    }
    fclose(fp);
}

