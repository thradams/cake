
void T5()
{
    constexpr int a[2][3] = { 1, 2, 3, 4, 5, 6 };

    static_assert(a[0][0] == 1);
    static_assert(a[0][1] == 2);
    static_assert(a[0][2] == 3);
    static_assert(a[1][0] == 4);
    static_assert(a[1][1] == 5);
    static_assert(a[1][2] == 6);
}
