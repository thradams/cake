/* Cake 0.14.05 x86_msvc */

struct Y {
    double d;
};

struct X {
    struct Y y;
    int i;
};

static void* memcpy(void * dest, const void * src, unsigned int n);

void T2()
{
    struct Y y;
    struct X x;

    y.d = 3.0;
    memcpy(&x.y, &y, 8);
    x.i = 2;
}

static void* memcpy(void * dest, const void * src, unsigned int n)
{
    char *csrc;
    char *cdest;
    unsigned int i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}

