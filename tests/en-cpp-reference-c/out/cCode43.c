struct _iobuf {
    void * _Placeholder;
};


int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
void __cdecl perror(char * _ErrorMessage);

int main(void)
{
    int rc;

    rc = fputs("Hello World", (__acrt_iob_func(1)));
    if (rc == -1)
    {
        perror("fputs()");
    }
}

