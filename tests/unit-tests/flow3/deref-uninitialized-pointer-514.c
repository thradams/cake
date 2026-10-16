#pragma safety enable

/* issue #514 */
struct S { int a; };

void f1(void)
{
    int* p;
    *p = 1; //lint 30 using a possibly uninitialized object 'p'
}

int f2(void)
{
    struct S* s;
    return s->a; //lint 30 using a possibly uninitialized object 's'
}

int f3(int c)
{
    int x = 0;
    int* p;
    if (c) p = &x;
    return *p; //lint 30 using a possibly uninitialized object 'p'
}
