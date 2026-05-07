
void T13()
{
    struct X {
        int a, b, c;
    };

    //error: member 'd' not found in 'X' 

    constexpr struct X x = {.d = 3}; //lint 720

    static_assert(x.a == 0);
    static_assert(x.b == 0);
    static_assert(x.c == 0);
}