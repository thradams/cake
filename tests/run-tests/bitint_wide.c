/*
   _BitInt(N) wider than 64 bits in the generated code: it is lowered to
   long long long (__int128), and every result is wrapped to N bits with
   masks built from two halves, since there are no 128-bit literals.
*/
#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

#pragma CAKE diagnostic ignored 51 /* unsigned wrap-around is what is tested */

#if __BITINT_MAXWIDTH__ >= 128

static unsigned _BitInt(100) add100(unsigned _BitInt(100) a, unsigned _BitInt(100) b)
{
    return a + b;
}

static _BitInt(100) to_signed100(unsigned _BitInt(100) v)
{
    return (_BitInt(100))v;
}

#endif

int main(void)
{
#if __BITINT_MAXWIDTH__ >= 128
    /* unsigned wraps at 100 bits */
    unsigned _BitInt(100) u = 0;
    u = u - 1;
    CHECK((unsigned long long)(u >> 64) == (1ULL << 36) - 1);
    CHECK((unsigned long long)u == 0xFFFFFFFFFFFFFFFFULL);

    u = add100(u, 2);
    CHECK(u == 1);

    /* a value with bit 99 set is negative as a signed _BitInt(100) */
    unsigned _BitInt(100) top = 1;
    top = top << 99;
    _BitInt(100) s = to_signed100(top);
    CHECK(s < 0);
    CHECK(to_signed100(top - 1) > 0);
    CHECK(to_signed100((unsigned _BitInt(100))0 - 1) == -1);

    /* multiplication keeps the low 100 bits */
    unsigned _BitInt(100) big = 1;
    big = big << 60;
    big = big * big; /* 2^120 wraps to 0 */
    CHECK(big == 0);

    /* conversions from wider and narrower types */
    unsigned long long long wide = (unsigned long long long)-1;
    unsigned _BitInt(65) narrow = (unsigned _BitInt(65))wide;
    CHECK((unsigned long long)(narrow >> 64) == 1);
    CHECK((unsigned long long)narrow == 0xFFFFFFFFFFFFFFFFULL);

    _BitInt(70) negative = -5;
    long long back = (long long)negative;
    CHECK(back == -5);

    /* constants wider than 64 bits written in the generated code */
    unsigned _BitInt(100) constant = (unsigned _BitInt(100))-1;
    CHECK((unsigned long long)(constant >> 64) == (1ULL << 36) - 1);

    /* a uwb literal wider than an unsigned long long */
    unsigned _BitInt(100) literal = 0x123456789ABCDEF0123456789uwb;
    CHECK((unsigned long long)(literal >> 64) == 0x123456789ULL);
    CHECK((unsigned long long)literal == 0xABCDEF0123456789ULL);
#endif

    return failures;
}
