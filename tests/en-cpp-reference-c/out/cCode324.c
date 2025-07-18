struct _iobuf {
    void * _Placeholder;
};


int __cdecl putchar(int _Character);
int __cdecl memcmp(void * _Buf1, void * _Buf2, unsigned int _Size);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl puts(char * _Buffer);

void demo(char * lhs, char * rhs, unsigned int sz)
{
    int rc;
    char * rel;

    {
        unsigned int n;
        n = 0;
        for (; n < sz; ++n)
        putchar(lhs[n]);
    }
    rc = memcmp(lhs, rhs, sz);
    rel = rc < 0 ? " precedes " : rc > 0 ? " follows " : " compares equal ";
    fputs(rel, (__acrt_iob_func(1)));
    {
        unsigned int n;
        n = 0;
        for (; n < sz; ++n)
        putchar(rhs[n]);
    }
    puts(" in lexicographical order");
}

int main(void)
{
    char a1[3];
    char a2[3];

    a1[0] = 97;
    a1[1] = 98;
    a1[2] = 99;
    a2[0] = 97;
    a2[1] = 98;
    a2[2] = 100;
    demo(a1, a2, 3U);
    demo(a2, a1, 3U);
    demo(a1, a1, 3U);
}

