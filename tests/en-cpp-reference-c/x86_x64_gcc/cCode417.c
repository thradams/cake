/* Cake x86_x64_gcc */
struct timespec {
    int long tv_sec;
    int long tv_nsec;
};


extern double difftime(int long __time1, int long __time0);

double usdiff(struct timespec  s, struct timespec  e)
{
    double sdiff;
    long nsdiff;

    sdiff = difftime(e.tv_sec, s.tv_sec);
    nsdiff = e.tv_nsec - s.tv_nsec;
    if (nsdiff < 0)
    {
        return 1000000 * (sdiff - 1) + (1000000000L + nsdiff) / 1000.0;
    }
    else
    {
        return 1000000 * (sdiff) + nsdiff / 1000.0;
    }
}


extern int timespec_get(struct timespec * __ts, int __base);
extern void thrd_yield(void);

void sleep_100us()
{
    struct timespec  start;
    struct timespec  end;

    timespec_get(&start, 1);
    do
    {
        thrd_yield();
        timespec_get(&end, 1);
    }
    while (usdiff(start, end) < 100.0);
}


extern int printf(char * __format, ...);

int main()
{
    struct timespec  start;
    struct timespec  end;

    timespec_get(&start, 1);
    sleep_100us();
    timespec_get(&end, 1);
    printf("Waited for %.3f us\n", usdiff(start, end));
}


