
void T13()
{
    struct X {
        int a, b, c;
    };

    constexpr struct X x = { .d = 3 };
#pragma cake diagnostic check "-E720"


    static_assert(x.a == 0);
    static_assert(x.b == 0);
    static_assert(x.c == 0);
}