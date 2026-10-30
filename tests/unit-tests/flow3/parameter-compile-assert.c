void f(int i compile_assert(i > 0));

void g(int x)
{
    f(1);
    f(0); //lint 67
    f(x); //lint 67
    if (x > 0)
    {
        compile_assert(x > 0);
        f(x);
    }
}
