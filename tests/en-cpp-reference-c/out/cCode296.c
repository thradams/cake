struct _iobuf {
    void * _Placeholder;
};


int __cdecl puts(char * _Buffer);
int __cdecl fflush(struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void f1(void)
{
    puts("pushed first");
    fflush((__acrt_iob_func(1)));
}

void f2(void)
{
    puts("pushed second");
}

int __cdecl at_quick_exit(void (__cdecl *)(void));
void __cdecl quick_exit(int _Code);

int main(void)
{
    at_quick_exit(f1);
    at_quick_exit(f2);
    quick_exit(0);
}

