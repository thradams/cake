/* Cake x86_x64_gcc */

extern unsigned int putwchar(int __wc);
extern int wmemcmp(int * __s1, int * __s2, unsigned int long __n);
extern int wprintf(int * __format, ...);

void demo(int * lhs, int * rhs, unsigned int long sz)
{
    int rc;

    {
        unsigned int long n;

        n = 0;
        for (; n < sz; ++n)
        putwchar(lhs[n]);
    }
    rc = wmemcmp(lhs, rhs, sz);
    if (rc == 0)
    {
        wprintf(L" compares equal to ");
    }
    else
    {
        if (rc < 0)
        {
            wprintf(L" precedes ");
        }
        else
        {
            if (rc > 0)
            {
                wprintf(L" follows ");
            }
        }
    }
    {
        unsigned int long n;

        n = 0;
        for (; n < sz; ++n)
        putwchar(rhs[n]);
    }
    wprintf(L" in lexicographical order\n");
}


extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int a1[3];
    int a2[3];
    unsigned int long sz;

    setlocale(6, "en_US.utf8");
    a1[0] = 945;
    a1[1] = 946;
    a1[2] = 947;
    a2[0] = 945;
    a2[1] = 946;
    a2[2] = 948;
    sz = 3L;
    demo(a1, a2, sz);
    demo(a2, a1, sz);
    demo(a1, a1, sz);
}


