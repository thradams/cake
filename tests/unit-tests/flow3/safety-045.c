#pragma safety enable

struct X {
    struct X* _Opt next;
};


void f(struct X* p);
struct X* _Opt get();

int c;

int main()
{
    struct X* _Opt p = get();
    for (; p; p = p->next)
    {
        compile_assert(p != 0);
    }
    compile_assert(p == 0);

    for (p = get(); p; p = p->next)
    {
        compile_assert(p != 0);
    }
    compile_assert(p == 0);

    for (p = get(); p; )
    {
        compile_assert(p != 0);
        p = p->next;
    }
    compile_assert(p == 0);

    for (p = get(); p; )
    {
        if (c)
        {
            compile_assert(p != 0);
            break;
        }
        p = p->next;
    }
   
    for (struct X* _Opt p2 = get(); p2; )
    {
        compile_assert(p2 != 0);
        f(p2);
    }

    
    struct X* _Opt p3 = get();
    for (;;)
    {
        break;
    }
    
}
