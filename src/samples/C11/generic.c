#include <math.h>

#define cbrt(X) _Generic((X), \
                  double: cbrtl, \
                  float: cbrtf ,\
                  default: cbrtl  \
              )(X)

int main(void)
{
    cbrt(1.0);

    const int * const p = 0;
    _Static_assert(_Generic(p, const int * : 1 ), "");
    _Static_assert(_Generic(&p, const int * const * : 1 ), "");
    _Static_assert(_Generic(main, int (*)(void) : 1 ), "");

    const int * const p2 = 0;
    static_assert(_Generic(p2, const int *: 1));

    static_assert(_Generic("abc",  char *: 1));    
}
