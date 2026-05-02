#pragma nullable disable
void f(int  *p)
{
  assert_state(p, "null | not-null");
}
