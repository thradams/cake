/* Cake x86_x64_gcc */
struct timespec {
    int long tv_sec;
    int long tv_nsec;
};


extern int printf(char * __format, ...);
extern char *ctime(int long * __timer);
extern int long time(int long * __timer);
extern int thrd_sleep(struct timespec * __time_point, struct timespec * __remaining);

int main(void)
{
    int long __v0;
    struct timespec  __v1;
    int long __v2;

    __v0(null) = time(0);
    printf("Time: %s", ctime(&__v0));
    __v1.tv_sec = 1;
    __v1.tv_nsec = 0;
    thrd_sleep(&__v1, 0);
    __v2(null) = time(0);
    printf("Time: %s", ctime(&__v2));
}


