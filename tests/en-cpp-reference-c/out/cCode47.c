struct _iobuf {
    void * _Placeholder;
};


int __cdecl puts(char * _Buffer);
struct _iobuf *__cdecl freopen(char * _FileName, char * _Mode, struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
void __cdecl perror(char * _ErrorMessage);
int __cdecl fclose(struct _iobuf * _Stream);

int main(void)
{
    puts("stdout is printed to console");
    if (freopen("redir.txt", "w", (__acrt_iob_func(1))) == 0U)
    {
        perror("freopen() failed");
        return 1;
    }
    puts("stdout is redirected to a file");
    fclose((__acrt_iob_func(1)));
    return 0;
}

