/* Cake 0.12.26 x86_msvc */
struct __tag6 {
    void * _Opaque;
};

struct __tag3 {
    void * _Handle;
    unsigned int _Tid;
};


int __cdecl puts(char * _Buffer);

void do_once(void)
{
    puts("called once");
}


void __cdecl call_once(struct __tag6 * _Flag, void (* _Func)(void));
static struct __tag6  flag = {0};


int func(void * data)
{
    call_once(&flag, do_once);
}


int __cdecl thrd_create(struct __tag3 * _Thr, int (__cdecl * _Func)(void *), void * _Arg);
int __cdecl thrd_join(struct __tag3  _Thr, int * _Res);

int main(void)
{
    struct __tag3  t1;
    struct __tag3  t2;
    struct __tag3  t3;
    struct __tag3  t4;

    thrd_create(&t1, func, 0);
    thrd_create(&t2, func, 0);
    thrd_create(&t3, func, 0);
    thrd_create(&t4, func, 0);
    thrd_join(t1, 0);
    thrd_join(t2, 0);
    thrd_join(t3, 0);
    thrd_join(t4, 0);
}


