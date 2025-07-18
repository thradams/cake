struct _iobuf {
    void * _Placeholder;
};


struct _iobuf *__cdecl tmpfile(void);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);
void __cdecl rewind(struct _iobuf * _Stream);
int __cdecl fgetc(struct _iobuf * _Stream);
int __cdecl putchar(int _Character);
int __cdecl puts(char * _Buffer);
void __cdecl clearerr(struct _iobuf * _Stream);
int __cdecl feof(struct _iobuf * _Stream);

int main(void)
{
    struct _iobuf * tmpf;

    tmpf = tmpfile();
    fputs("cppreference.com\n", tmpf);
    rewind(tmpf);
    {
        int ch;
        for (; (ch = fgetc(tmpf)) != -1; putchar(ch))
        {
        }
    }
    ;
    puts("End of file reached");
    clearerr(tmpf);
    puts(feof(tmpf) ? "EOF indicator set" : "EOF indicator cleared");
}

