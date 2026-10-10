#pragma safety enable

/*
  A string literal reached through ?: is still non-null after pointer
  arithmetic. Reduced from codegen.c codegen_print_designator.
*/

#include <string.h>

struct O {
  const char * _Opt md;
};

void f(const struct O * o, const char * b)
{
    const char * d = o->md ? o->md : "";
    size_t n = strlen(b);
    const char * s = strncmp(d, b, n) == 0 ? d + n : d;
    (void)s;
}

void g(const struct O * o, unsigned long n)
{
    const char * d = o->md ? o->md : "";
    const char * s = d + n;
    (void)s;
}
