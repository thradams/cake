// &a of an array parameter is a pointer to the adjusted pointer (6.7.6.3p7)

void f1(int a[2])
{
    static_assert(_Generic(&a, int**: 1, default: 0));
}

void f2(int a[const 2])
{
    static_assert(_Generic(&a, int* const*: 1, default: 0));
    int* const* p = &a;
    (void)p;
}

void f3(const int a[const 2])
{
    static_assert(_Generic(&a, const int* const*: 1, default: 0));
}

void f4(int a[const][2])
{
    static_assert(_Generic(&a, int (* const*)[2]: 1, default: 0));
}

void f5(int a[const 2])
{
    static_assert(_Generic(typeof(a), int* const: 1, default: 0)); //lint 8
}

void f6(int a[restrict 2])
{
    static_assert(_Generic(&a, int* restrict*: 1, default: 0));
}
