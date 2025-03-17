
void f(int n)
{
  int v[123][n];
  static_assert(_Countof(v) == 123);
}

int main()
{
  int a[7][3];
  int n = _Countof(a);
  static_assert(_Countof(a) == 7);

  int n2 = _Countof(int [7][3]);
  static_assert(_Countof(int [2][3]) == 2);
}