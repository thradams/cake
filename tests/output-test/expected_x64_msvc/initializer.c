/* Cake 0.14.03 x86_msvc */

struct A {
    int x;
    int y;
};

struct B {
    struct A a;
};

static void* memcpy(void * dest, const void * src, unsigned int n);

int main(void)
{
    struct A ia;
    struct B b;

    ia.x = 1;
    ia.y = 2;
    memcpy(&b.a, &ia, 8);
    b.a.y = 42;
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

