/* Cake x64_msvc */

struct X {
    int i;
    void * p;
};

static void memset(void *dest, int ch, unsigned long long count);

int main()
{
    struct X x;
    struct X __a;

    memset(&__a, 0, 16);
    x = __a;
}

static void memset(void *dest, int ch, unsigned long long count)
{
    unsigned char *ptr;

    ptr = (unsigned char*)dest;
    while (count-- > 0)
    {
       *ptr++ = 0;
    }
}

