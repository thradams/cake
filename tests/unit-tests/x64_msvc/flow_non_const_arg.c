/* Cake x64_msvc */

struct X {
    void * text;
};

static void memset(void *dest, int ch, unsigned long long count);

void x_change(struct X * list);

int main()
{
    struct X x;

    memset(&x, 0, 8);
    x_change(&x);
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

