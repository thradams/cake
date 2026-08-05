
#pragma flow enable

void f(int * p)
{
    if (p)
    {
        compile_assert(p != 0);
    }
    else
    {
        compile_assert(p == 0);
    }
}
