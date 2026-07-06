/* Cake x64_msvc */

struct X {
    char * text;
};

static void memset(void *dest, int ch, unsigned long long count);

void free(void * p);

void x_destroy(struct X * x)
{
    free(x->text);
}

int main()
{
    struct X x;

    memset(&x, 0, 8);
    x_destroy(&x);
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

