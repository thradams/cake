/* Cake x64_msvc */

struct _timespec64 {
    long long tv_sec;
    long tv_nsec;
};

struct timespec {
    long long tv_sec;
    long tv_nsec;
};

struct _iobuf {
    void * _Placeholder;
};

void __cdecl setbuf(struct _iobuf * _Stream, char * _Buffer);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl putchar(int _Character);
static int __cdecl thrd_sleep(struct timespec * duration, struct timespec * remaining);
int __cdecl _thrd_sleep64(struct _timespec64 * duration, struct _timespec64 * remaining);

int main(void)
{
    struct timespec __a;

    setbuf((__acrt_iob_func(1)), 0);
    putchar(97);
    __a.tv_sec = 1;
    __a.tv_nsec = 0;
    thrd_sleep(&__a, 0);
    putchar(98);
}

static int __cdecl thrd_sleep(struct timespec * duration, struct timespec * remaining)
{
    return _thrd_sleep64((struct _timespec64 *)duration, (struct _timespec64 *)remaining);
}
