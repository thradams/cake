/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

static void * memset(void *dest, int ch, unsigned int count);

int main()
{
    int a[5];
    int i;

    memset(&a, 0, 20);
    i = a[5];
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
