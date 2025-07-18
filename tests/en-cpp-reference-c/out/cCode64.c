struct _iobuf {
    void * _Placeholder;
};

struct timespec {
    __int64 tv_sec;
    long tv_nsec;
};

struct _timespec64 {
    __int64 tv_sec;
    long tv_nsec;
};


void __cdecl setbuf(struct _iobuf * _Stream, char * _Buffer);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl putchar(int _Character);

int __cdecl _thrd_sleep64(struct _timespec64 * duration, struct _timespec64 * remaining);
inline static int __cdecl thrd_sleep(struct timespec * duration, struct timespec * remaining)
{
    return _thrd_sleep64((struct _timespec64 *)duration, (struct _timespec64 *)remaining);
}

int main(void)
{
    struct timespec  __cmp_lt_0;

    setbuf((__acrt_iob_func(1)), 0U);
    putchar(97);
    __cmp_lt_0.tv_sec = 1;
    __cmp_lt_0.tv_nsec = 0;
    thrd_sleep(&__cmp_lt_0, 0U);
    putchar(98);
}

