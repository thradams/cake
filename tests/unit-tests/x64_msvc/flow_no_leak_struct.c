/* Cake x64_msvc */

struct X {
    char * text;
};

static void memset(void *dest, int ch, unsigned long long count);

char *strdup(char * s);
void free(void * p);

void f(int a)
{
    struct X x;

    memset(&x, 0, 8);
    x.text = strdup("a");
    free(x.text);
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

