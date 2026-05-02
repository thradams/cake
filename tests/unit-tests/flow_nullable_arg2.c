#pragma safety enable
void f(int  * _Opt p)
{
  assert_state(p, "null | not-null");
}
