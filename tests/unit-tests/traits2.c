#define _is_same(T1, T2) _Generic(T1, T2: 1, default: 0)

int main()
{
  int i;
  static_assert(_is_integral(i));
  static_assert(_is_floating_point(double) && _is_floating_point(float));
  static_assert(_is_function(main));

  char *p;
  static_assert(_is_scalar(p));
  static_assert(_is_scalar(nullptr));

  int a[10];
  static_assert(_is_array(a));

  
  static_assert(_is_same(int, typeof(i)));

  static_assert(_is_const(const int));
  static_assert(!_is_const(const int *));
  static_assert(_is_const(int *const));
}
