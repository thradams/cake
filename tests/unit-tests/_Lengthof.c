
void f(int n)
{
  int v[123][n];
  static_assert(_Lengthof(v) == 123);
}

int main()
{
  int a[7][3];
  int n = _Lengthof(a);
  static_assert(_Lengthof(a) == 7);

  int n2 = _Lengthof(int [7][3]);
  static_assert(_Lengthof(int [2][3]) == 2);
}