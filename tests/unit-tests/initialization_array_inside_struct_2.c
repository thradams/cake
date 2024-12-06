void T1()
{
    struct X
    {
        int a;
        int b;
    };

    struct Y
    {
        int i;
        struct X x;
        struct X x2;
    };

    constexpr struct Y y = { 1, {1, 2}, 3 };
    static_assert(y.i == 1);

    static_assert(y.x.a == 1);
    static_assert(y.x.b == 2);

    static_assert(y.x2.a == 3);
}
