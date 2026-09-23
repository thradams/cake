
#include <stdio.h>

int main()
{
  bool b = true;
  b = false;
  static_assert(1 == true);
  static_assert(0 == false);

  printf("%s", _Generic(true, bool : "bool"));
  printf("%s", _Generic(false, bool : "bool"));

  printf("%s", _Generic(b, bool : "bool"));

  auto b2 = true;
  printf("%s", _Generic(b2, bool : "bool"));
}
