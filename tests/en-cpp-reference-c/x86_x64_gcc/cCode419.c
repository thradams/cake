/* Cake x86_x64_gcc */
struct S;


extern int printf(char * __format, ...);
extern void *malloc(unsigned int long __size);
extern void free(void * __ptr);

int main(void)
{
    int * p;
    struct S * s;
    void (* f)(int, double);
    char * ptr;

    p = 0;
    s = 0;
    f = 0;
    printf("%p %p %p\n", (void *)p, (void *)s, (void *)(long)f);
    ptr = malloc(15ULL);
    if (ptr == 0)
    {
        printf("Out of memory");
    }
    else
    {
        printf("ptr = %#lx\n", (unsigned int long)ptr);
    }
    free(ptr);
}


