
void f(int n)
{
  int v[123][n];
  static_assert(nelementsof(v) == 123);
}

int main()
{
  int a[7][3];
  int n = nelementsof(a);
  static_assert(nelementsof(a) == 7);

  int n2 = nelementsof(int [7][3]);
  static_assert(nelementsof(int [2][3]) == 2);
}