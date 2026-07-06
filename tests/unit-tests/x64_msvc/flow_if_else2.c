/* Cake x64_msvc */

struct X {
    char * name;
};

static void memset(void *dest, int ch, unsigned long long count);

int rand();
struct X f();
void x_destroy(struct X * p);

void g()
{
    {
        struct X x;

        memset(&x, 0, 8);
        if (rand())
        {
            x = f();
        }
        else
        {
            x = f();
        }
        x_destroy(&x);
    }
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

