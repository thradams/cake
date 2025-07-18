struct _struct_tag_6 {
    void * _Opaque;
};

struct _struct_tag_3 {
    void * _Handle;
    unsigned int _Tid;
};


int __cdecl puts(char * _Buffer);

void do_once(void)
{
    puts("called once");
}

static struct _struct_tag_6  flag = {0};
void __cdecl call_once(struct _struct_tag_6 * _Flag, void (* _Func)(void));

int func(void * data)
{
    call_once(&flag, do_once);
}

int __cdecl thrd_create(struct _struct_tag_3 * _Thr, int (__cdecl * _Func)(void *), void * _Arg);
int __cdecl thrd_join(struct _struct_tag_3  _Thr, int * _Res);

int main(void)
{
    struct _struct_tag_3  t1;
    struct _struct_tag_3  t2;
    struct _struct_tag_3  t3;
    struct _struct_tag_3  t4;

    thrd_create(&t1, func, 0U);
    thrd_create(&t2, func, 0U);
    thrd_create(&t3, func, 0U);
    thrd_create(&t4, func, 0U);
    thrd_join(t1, 0U);
    thrd_join(t2, 0U);
    thrd_join(t3, 0U);
    thrd_join(t4, 0U);
}

