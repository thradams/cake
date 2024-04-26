#pragma safety enable
void f(int  * _Opt p)
{
  static_state(p, "null | not-null");
}
