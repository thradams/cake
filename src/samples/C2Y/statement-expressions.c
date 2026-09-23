
//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3643.htm

#include <stdio.h>

#define maxint(a,b)   ({int _a = (a), _b = (b); _a > _b ? _a : _b; })

int main()
{
  printf("%d", maxint(1, 2));
}
