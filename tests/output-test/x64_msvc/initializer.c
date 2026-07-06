/* Cake x64_msvc */

struct A {
    int x;
    int y;
};

struct B {
    struct A a;
};

static void memcpy(void * dest, const void * src, unsigned long long n);

int main(void)
{
    struct A ia;
    struct B b;

    ia.x = 1;
    ia.y = 2;
    memcpy(b, ia, 8);
    b.a.y = 42;
}

static void memcpy(void * dest, const void * src, unsigned long long n)
{
    char *csrc;
    char *cdest;
    unsigned long long i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}

