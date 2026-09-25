#pragma safety enable

void use(int i);

void f(int c, int a, int b)
{
    bool n;
    if (c)
    {
        n = a && (b < 0);
    }
    else
    {
        n = false;
    }
    if (!n) /* no warning 30 */
    {
        use(1);
    }
}
