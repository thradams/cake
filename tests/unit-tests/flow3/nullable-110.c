#pragma nullable disable
void f(int  *p)
{
    compile_assert(p == 0 || p != 0);
}
