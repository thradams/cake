void T10()
{
    constexpr int a[3] = { [2] = 1,[1] = 2,[0] = 3 };
    static_assert(a[0] == 3);
    static_assert(a[1] == 2);
    static_assert(a[2] == 1);
}

