int main()
{
    int a[] = {1, 2, 3};
    static_assert(sizeof(a) == sizeof(int) * 3);
}
