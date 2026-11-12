/*
   The value of a conditional expression with a constant condition is the
   selected operand converted to the common type of the second and third
   operands (6.5.16): (1 ? -1 : 1u) is UINT_MAX, not -1.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static long long file_scope = (1 ? -1 : 1u);

static int id(int x)
{
    return x;
}

int main(void)
{
    long long a = (1 ? -1 : 1u);
    long long b = (0 ? 1u : -1);
    long long c = (id(1) ? -1 : 1u); /* not constant: same result */
    double d = (1 ? 1 : 2.0) / 2;
    long long e = (1 ? -1 : 1u) / 2;

    CHECK(a == 4294967295LL);
    CHECK(b == 4294967295LL);
    CHECK(c == 4294967295LL);
    CHECK(d == 0.5);
    CHECK(e == 2147483647LL);
    CHECK(file_scope == 4294967295LL);

    return failures;
}
