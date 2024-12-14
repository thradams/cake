#pragma safety enable
void f(int  *p)
{
  static_state(p, "not-null");
}
