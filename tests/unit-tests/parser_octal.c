int main()
{
    static_assert("\100\040\040"[0] == 64);
    static_assert('\100' == 64);
    static_assert('\0' == 0);
    static_assert('\7' == 7);
    static_assert('\10' == 8);
    static_assert('\17' == 15);
    static_assert('\377' == -1);
}