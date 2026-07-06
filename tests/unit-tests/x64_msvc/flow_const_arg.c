/* Cake x64_msvc */

struct X {
    void * text;
};

static void memset(void *dest, int ch, unsigned long long count);

void f(struct X * list);

int main()
{
    struct X x;

    memset(&x, 0, 8);
    f(&x);
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

