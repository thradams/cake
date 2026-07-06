/* Cake x86_x64_gcc */

extern int long clock(void);
extern int printf(char * __format, ...);

int main(void)
{
    int long t;
    double d;
    volatile double vd;

    t = clock();
    d = 0.0;
    {
        int n;

        n = 0;
        for (; n < 10000; ++n)
        {
            int m;

            m = 0;
            for (; m < 10000; ++m)
            d += d * n * m;
        }
    }
    printf("Modified a non-volatile variable 100m times. Time used: %.2f seconds\n", (double)(clock() - t) / 1000000L);
    t = clock();
    vd = 0.0;
    {
        int n;

        n = 0;
        for (; n < 10000; ++n)
        {
            int m;

            m = 0;
            for (; m < 10000; ++m)
            {
                double prod;

                prod = vd * n * m;
                vd += prod;
            }
        }
    }
    printf("Modified a volatile variable 100m times. Time used: %.2f seconds\n", (double)(clock() - t) / 1000000L);
}


