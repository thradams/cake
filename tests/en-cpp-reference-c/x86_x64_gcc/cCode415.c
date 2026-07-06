/* Cake x86_x64_gcc */
struct __tag5 {
    int __data;
};


extern int puts(char * __s);

void do_once(void)
{
    puts("called once");
}


extern void call_once(struct __tag5 * __flag, void (* __func)(void));
static struct __tag5  flag = {0};


int func(void * data)
{
    call_once(&flag, do_once);
}


extern int thrd_create(unsigned int long * __thr, int (* __func)(void *), void * __arg);
extern int thrd_join(unsigned int long __thr, int * __res);

int main(void)
{
    unsigned int long t1;
    unsigned int long t2;
    unsigned int long t3;
    unsigned int long t4;

    thrd_create(&t1, func, 0);
    thrd_create(&t2, func, 0);
    thrd_create(&t3, func, 0);
    thrd_create(&t4, func, 0);
    thrd_join(t1, 0);
    thrd_join(t2, 0);
    thrd_join(t3, 0);
    thrd_join(t4, 0);
}


