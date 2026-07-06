/* Cake x64_msvc */

struct X {
    char * name;
};

static void memset(void *dest, int ch, unsigned long long count);

char *strdup(char * s);

struct X make()
{
    struct X x;

    memset(&x, 0, 8);
    x.name = strdup("text");
    return x;
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

