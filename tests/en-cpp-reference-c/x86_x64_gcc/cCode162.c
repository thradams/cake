/* Cake x86_x64_gcc */

extern void *calloc(unsigned int long __nmemb, unsigned int long __size);
extern int printf(char * __format, ...);
extern void free(void * __ptr);

int main(void)
{
    int * p1;
    int * p2;
    int * p3;

    p1 = calloc(4, 4L);
    p2 = calloc(1, 16L);
    p3 = calloc(4, 4L);
    if (p2)
    {
        {
            int n;

            n = 0;
            for (; n < 4; ++n)
            printf("p2[%d] == %d\n", n, p2[n]);
        }
    }
    free(p1);
    free(p2);
    free(p3);
}


