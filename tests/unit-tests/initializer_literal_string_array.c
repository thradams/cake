void T6()
{
    constexpr char s[] = "123";
    constexpr char s2[] = s;
    static_assert(s2[0] == '1');
}