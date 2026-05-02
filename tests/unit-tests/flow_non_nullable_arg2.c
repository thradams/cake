#pragma safety enable
void f(int  *p)
{
  assert_state(p, "not-null");
}
