int main()
{
    const int a[3] = {1,2, 3};
    &a[1+1];
    static_assert(a[0] == 1);
}