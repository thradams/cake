void T2()
{

    struct Y
    {
        double d;
    };
    struct X
    {
        struct Y y;
        int i;
    };
    constexpr struct Y y = { 3.0 };
    constexpr struct X x = { y, 2 };
    static_assert(x.y.d == 3.0);
    static_assert(x.i == 2);

}


