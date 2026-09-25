/*
   Runtime values of folded _BitInt(N) and bool constants: cake computes them
   at compile time (checked by tests/unit-tests/bitint_constant_folding.c) and
   the generated code must hold the same values.

   bool is compared with unsigned _BitInt(1): the conversion to bool is != 0,
   the conversion to unsigned _BitInt(1) keeps the low bit. Both are checked
   on constants (folded by cake) and on volatile values (converted by the
   generated code).
*/
#include <stdio.h>

#pragma CAKE diagnostic ignored 44 51 /* constant conditions and unsigned wrap-around are what is tested */

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

int main(void)
{
    /* conversions */
    constexpr unsigned _BitInt(12) a = (unsigned _BitInt(12)) 4096;
    CHECK(a == 0);
    constexpr unsigned _BitInt(12) neg = (unsigned _BitInt(12)) -1LL;
    CHECK(neg == 4095);
    constexpr unsigned _BitInt(12) fromd = (unsigned _BitInt(12)) 4095.9;
    CHECK(fromd == 4095);
    constexpr _BitInt(8) s = (_BitInt(8)) 200;
    CHECK(s == -56);
    constexpr _BitInt(3) s3 = (_BitInt(3)) 4;
    CHECK(s3 == -4);
    CHECK((_BitInt(3)) 7 == -1);
    CHECK((_BitInt(40)) 0x8000000001LL == 1 - 0x8000000000LL);
    CHECK((unsigned _BitInt(63)) -1LL == 0x7fffffffffffffffULL);

    /* arithmetic in N bits */
    constexpr unsigned _BitInt(4) n = 15;
    constexpr unsigned _BitInt(4) one = 1;
    CHECK(n + one == 0);
    CHECK((n + one) / 2 == 0);
    CHECK(one - n == 2);
    CHECK(n * n == 1);
    CHECK(~one == 14);
    CHECK((one << 4) == 0);
    CHECK((n >> 2) == 3);
    constexpr unsigned _BitInt(4) sum = n + one;
    CHECK(sum == 0);
    constexpr unsigned _BitInt(12) xx = (unsigned _BitInt(12)) 100 * (unsigned _BitInt(12)) 100;
    CHECK(xx == 10000 % 4096);

    /* promotions of narrow standard types */
    CHECK(~(unsigned char) 1 == -2);
    CHECK(-(unsigned char) 1 == -1);

    /* bool x unsigned _BitInt(1), constants */
    constexpr bool b2 = (bool) 2;
    constexpr unsigned _BitInt(1) u2 = (unsigned _BitInt(1)) 2;
    CHECK(b2 == 1);
    CHECK(u2 == 0);
    CHECK((bool) 0.5 == 1);
    CHECK((unsigned _BitInt(1)) 0.5 == 0);
    CHECK((bool) 1 + (bool) 1 == 2);
    CHECK((unsigned _BitInt(1)) 1 + (unsigned _BitInt(1)) 1 == 0);
    CHECK(~(bool) 1 == -2);
    CHECK(~(unsigned _BitInt(1)) 1 == 0);

    /* bool x unsigned _BitInt(1), runtime */
    volatile int v2 = 2;
    volatile int v3 = 3;
    volatile double vhalf = 0.5;
    CHECK((bool) v2 == 1);
    CHECK((unsigned _BitInt(1)) v2 == 0);
    CHECK((bool) v3 == 1);
    CHECK((unsigned _BitInt(1)) v3 == 1);
    CHECK((bool) vhalf == 1);
    CHECK((unsigned _BitInt(1)) vhalf == 0);
    bool vb = v2;
    unsigned _BitInt(1) vu = v2;
    CHECK(vb == 1);
    CHECK(vu == 0);
    unsigned _BitInt(1) vone = 1;
    CHECK(vone + vone == 0);
    CHECK(vb + vb == 2);
    CHECK(~vone == 0);
    CHECK(~vb == -2);

    return failures;
}
