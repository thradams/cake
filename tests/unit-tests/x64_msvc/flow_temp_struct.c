/* Cake x64_msvc */

struct X {
    int i;
};

static void memset(void *dest, int ch, unsigned long long count);

struct X f()
{
    struct X x;

    memset(&x, 0, 4);
    return x;
}

int main()
{
    f().i = 1;
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

