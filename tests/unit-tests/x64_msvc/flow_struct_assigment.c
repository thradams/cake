/* Cake x64_msvc */

struct X {
    char * name;
};

struct Y {
    int i;
};

static void memset(void *dest, int ch, unsigned long long count);

int main()
{
    struct Y y1;
    struct Y y2;
    struct X x1;
    struct X x2;

    memset(&y1, 0, 4);
    memset(&y2, 0, 4);
    y1 = y2;
    memset(&x1, 0, 8);
    memset(&x2, 0, 8);
    x1 = x2;
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

