void f(void)
{
    constexpr struct { int a[2]; } c = { 0 };
    (static void (void)) {
        const int* p = c.a; //lint 950 the usage of 'c.a' would require access to the object from the enclosing function
        (void)p;
    }();
}

void g(void)
{
    constexpr struct { int a[2]; } c = { 0 };
    static void local(void)
    {
        const int* p = c.a; //lint 950 the usage of 'c.a' would require access to the object from the enclosing function
        (void)p;
    }
    local();
}
