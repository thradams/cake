/*
   C23 _BitInt(N) is lowered to the smallest standard integer type that holds
   N bits (char/short/int/long/long long). This checks that the generated
   code keeps the values, the sizes, the wb/uwb literals and the constant
   folded results computed by cake. Arithmetic that overflows N bits at
   runtime is not wrapped by the generated code, so it is not tested here.
*/
#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

typedef unsigned _BitInt(24) u24;

struct S
{
    _BitInt(12) m;
    unsigned _BitInt(3) n : 2;
    _BitInt(8) a[4];
};

static _BitInt(40) add40(_BitInt(40) a, _BitInt(40) b)
{
    return a + b;
}

int main(void)
{
    _BitInt(12) a = 100;
    unsigned _BitInt(12) b = 4095uwb;
    signed _BitInt(40) c = -1;
    unsigned _BitInt(64) d = 0xFFFFFFFFFFFFFFFFuwb;
    _BitInt(8) e = 1wb;
    u24 f = 0xFFFFFF;
    struct S s = { .m = 5, .n = 3, .a = { 1, 2, 3, 4 } };

    static_assert(sizeof(a) == 2);
    static_assert(sizeof(b) == 2);
    static_assert(sizeof(c) == 8);
    static_assert(sizeof(d) == 8);
    static_assert(sizeof(e) == 1);
    static_assert(sizeof(f) == 4);
    static_assert(sizeof(struct S) == 8);

    CHECK(a == 100);
    CHECK(b == 4095);
    CHECK(c == -1);
    CHECK(d == 18446744073709551615ULL);
    CHECK(e == 1);
    CHECK(f == 0xFFFFFF);
    CHECK(s.m == 5);
    CHECK(s.n == 3);
    CHECK(s.a[3] == 4);

    CHECK(add40(0x7FFFFFFFF, 1) == 0x800000000);

    /* folded by cake at N bits, the generated code stores the wrapped value */
    unsigned _BitInt(12) w = (unsigned _BitInt(12)) 4096;
    _BitInt(8) y = (_BitInt(8)) 200;
    CHECK(w == 0);
    CHECK(y == -56);

    return failures;
}
