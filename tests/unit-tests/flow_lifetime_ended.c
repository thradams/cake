#pragma safety enable

struct X {
    int i;
};

void f2()
{
    struct X* _Opt p = 0;
    {
        struct X x = {0};
        p = &x;
    }

    if (p->i)  //lint 31  object lifetime ended 
    {
    }
}