/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * name;
};

static void * memset(void *dest, int ch, unsigned int count);

int rand();
struct X f();
void x_destroy(struct X * p);

void g()
{
    {
        struct X x;

        memset(&x, 0, 4);
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

static void * memset(void *ptr, int value, unsigned int count)
{
    unsigned char *p;
    unsigned char v;

    p = (unsigned char *) ptr;
    v = (unsigned char) value;
    while (count--)
    {
        *p++ = v;
    }

    return ptr;
}
