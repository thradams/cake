#pragma safety enable
void f(int  *p)
{
  compile_assert(p != 0);
}
