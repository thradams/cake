/* Cake x64_msvc */

struct X;

struct Y {
    struct X * p;
};

struct X {
    int i;
};

static void memset(void *dest, int ch, unsigned long long count);

void *calloc(int i, int sz);
void free(void * p);

int main()
{
    struct Y y;
    struct X * p;

    memset(&y, 0, 8);
    p = calloc(1, 4ULL);
    if (p)
    {
        y.p = p;
    }
    free(y.p);
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

