//https://open-std.org/jtc1/sc22/wg14/www/docs/n3715.pdf

#include <stdio.h>

#define BIT(n) ( \
  static_assert(n >= 0), \
  static_assert(n < 32), \
  1U << (n) \
)

int main()
{
    static_assert(1);
    printf("%u", BIT(1));
    return 0;
}
