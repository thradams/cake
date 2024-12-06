void T11()
{
    struct X {
        int a, b, c;
    };

    constexpr struct X x = { .b = 2, 3 };
    static_assert(x.a == 0);
    static_assert(x.b == 2);
    static_assert(x.c == 3);
}
