/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X;

struct Y {
    struct X * p;
};

struct X {
    int i;
};

static void * memset(void *dest, int ch, unsigned int count);

void *calloc(int i, int sz);
void free(void * p);

int main()
{
    struct Y y;
    struct X * p;

    memset(&y, 0, 4);
    p = calloc(1, 4);
    if (p)
    {
        y.p = p;
    }
    free(y.p);
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
