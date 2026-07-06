/* Cake x64_msvc */

struct Y {
    int g;
};

union __tag1 {
    unsigned char d;
    short e;
};

union __tag0 {
    char b;
    long c;
    union __tag1 __m0;
    double f;
};

struct X {
    int a;
    union __tag0 __m0;
    struct Y y;
};

static void memset(void *dest, int ch, unsigned long long count);

void f()
{
    struct X s;

    memset(&s, 0, 24);
    s.__m0.f = 1;
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

