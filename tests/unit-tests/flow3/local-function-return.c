#pragma safety enable

int main(void)
{
    static int f(int a)
    {
        return a;
    }
    int i = f(1);
    int j = i; /* must not be reported as unreachable */
    return j;
}
