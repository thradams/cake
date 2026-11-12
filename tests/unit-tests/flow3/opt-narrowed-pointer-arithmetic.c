#pragma safety enable

void f(const char* _Opt s, int len)
{
    if (s && len >= 2)
    {
        compile_assert(s != 0);
        compile_assert(s + 1 != 0);
    }
}
