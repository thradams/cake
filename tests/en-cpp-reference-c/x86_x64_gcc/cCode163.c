/* Cake x86_x64_gcc */

extern void *malloc(unsigned int long __size);
extern void free(void * __ptr);
extern void *calloc(unsigned int long __nmemb, unsigned int long __size);
extern void *realloc(void * __ptr, unsigned int long __size);

int main(void)
{
    int * p1;
    int * p2;
    int * p3;

    p1 = malloc(40L);
    free(p1);
    p2 = calloc(10, 4L);
    p3 = realloc(p2, 4000L);
    if (p3)
    {
        free(p3);
    }
    else
    {
        free(p2);
    }
}


