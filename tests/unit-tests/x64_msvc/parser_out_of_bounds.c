/* Cake x64_msvc */

static void memset(void *dest, int ch, unsigned long long count);

int main()
{
    int a[5];
    int i;

    memset(&a, 0, 20);
    i = a[5];
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

