
//OK
void T3()
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

    constexpr struct X x = { .i = 1 };
    static_assert(x.i == 1);
    static_assert(x.y.d == 0);
}
