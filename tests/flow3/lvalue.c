
#pragma flow enable
struct X {
    int a, b;
};
void f(int c)
{
    struct X x;
    struct X y;
    struct X *p;

    x.a = 1;
    x.b = 2;


    y.a = 3;
    y.b = 4;
    
    if (c)
    {
          p = &x;
    }
    else
    {
          p = &y;

    }
    static_debug(p);
    static_debug(p->a);
}