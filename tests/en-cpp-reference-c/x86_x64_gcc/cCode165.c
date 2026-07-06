/* Cake x86_x64_gcc */

extern void *malloc(unsigned int long __size);
extern int printf(char * __format, ...);
extern void free(void * __ptr);

int main(void)
{
    int * p1;
    int * p2;
    int * p3;

    p1 = malloc(16L);
    p2 = malloc(16L);
    p3 = malloc(16L);
    if (p1)
    {
        {
            int n;

            n = 0;
            for (; n < 4; ++n)
            p1[n] = n * n;
        }
        {
            int n;

            n = 0;
            for (; n < 4; ++n)
            printf("p1[%d] == %d\n", n, p1[n]);
        }
    }
    free(p1);
    free(p2);
    free(p3);
}


