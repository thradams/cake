/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

unsigned char init(int * a)
{
    *a = 3;
    return 1;
}

int main()
{
    int a;

    if (init(&a) || a == 0)
    {
        return a;
    }
    if (init(&a) && a == 0)
    {
        return a;
    }
    return a;
}
