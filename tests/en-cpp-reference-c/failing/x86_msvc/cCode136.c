/* Cake 0.13.29 x86_msvc */

union __tag0 {
    int x;
    double d;
};

struct s {
    int x;
};

static void * memset(void *dest, int ch, unsigned int count);

struct s f(void)
{
    struct s __a;

    __a.x = 1;
    return __a;
}

int main(void)
{
    struct s s;
    int n;
    struct s sc;
    union __tag0 u;

    s.x = 1;
    n = f().x;
    memset(&sc, 0, 4);
    u.x = 1;
    u.d = 0.10000000000000001;
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
