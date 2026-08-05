#pragma safety enable

long long f(long long a, long long b)
{
    if (a > 0 && b > 0)
    {
        return 1;
    }
    if (b > 0)
    {
        return a / b;
    }
    return 0;
}
