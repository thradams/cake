int main()
{
    //warning: division by zero
    [[cake::w37]]
    int a = 2 / 0;

    [[cake::w37]]
    static_assert((-2147483647-1)/-1 == -2147483648);

}
