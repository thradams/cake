void f(void)
{
    constexpr struct { int a[2]; } c = { 0 };
    (static void (void)) {
        const int* p = c.a; //lint 1870
        (void)p;
    }();
}

void g(void)
{
    constexpr struct { int a[2]; } c = { 0 };
    static void local(void)
    {
        const int* p = c.a; //lint 1870
        (void)p;
    }
    local();
}
