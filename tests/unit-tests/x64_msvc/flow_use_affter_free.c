/* Cake x64_msvc */

struct X {
    char * name;
};

static void memset(void *dest, int ch, unsigned long long count);

void free(void * ptr);

void x_destroy(struct X * p)
{
    free(p->name);
}

void x_print(struct X * p)
{
}

char *strdup(char * s);

int main()
{
    struct X x;

    memset(&x, 0, 8);
    x.name = strdup("a");
    x_destroy(&x);
    x_print(&x);
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

