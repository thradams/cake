/* Cake x86_x64_gcc */

volatile double sink;
extern int long clock(void);
extern double sin(double __x);
extern int printf(char * __format, ...);

int main(void)
{
    int long start;
    int long end;
    double cpu_time_used;

    start = clock();
    {
        unsigned int long i;

        i = 0;
        for (; i < 3141592; ++i)
        sink += sin(i);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / 1000000L;
    printf("for loop took %f seconds to execute \n", cpu_time_used);
}


