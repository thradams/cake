
#pragma safety enable

void f(int * _Nullable p)
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
