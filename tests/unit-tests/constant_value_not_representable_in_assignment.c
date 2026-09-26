struct S { int b : 1; unsigned int u : 2; };

void f(void)
{
    struct S s;
    s.b = 1;    //lint 74 warning: constant expression is not exactly representable in type
    s.b = -1;
    s.b = 0;
    s.u = 3;
    s.u = 5;    //lint 74 warning: constant expression is not exactly representable in type

    unsigned char uc;
    uc = 255;
    uc = 300;   //lint 74 warning: constant expression is not exactly representable in type
    (void)uc;
    (void)s;
}
