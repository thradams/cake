#pragma safety enable

int* _Nullable keep;
struct S { int* p; };
struct S gs;

void f1(void)
{
    int x = 0;
    keep = &x;
} //lint 31

void f2(void)
{
    int z = 0;
    gs.p = &z;
} //lint 31

void f3(void)
{
    int x = 0;
    keep = &x;
    keep = 0; /* reset before return */
}

void f4(void)
{
    static int s = 0;
    keep = &s;
}

void f5(int c)
{
    int x = 0;
    if (c)
    {
        keep = &x;
    }
} //lint 31
