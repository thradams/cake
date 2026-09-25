/*
   _BitInt(N) is lowered to a wider standard integer, so the generated code
   must wrap values to N bits itself: on arithmetic results, on conversions
   (assignment, initialization, cast, argument passing, return, compound
   assignment, ++/--). Values come from volatile objects so nothing is
   folded by cake and the wrapping is done by the emitted C.

   bool has the same problem (it is lowered to unsigned char): (bool)256,
   b += 2, b |= 256 must give 1, not 0.
*/
#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static unsigned _BitInt(12) pass12(unsigned _BitInt(12) v)
{
    return v;
}

static unsigned _BitInt(12) ret12(int v)
{
    return v; /* converted on return */
}

static _BitInt(8) ret8(int v)
{
    return v;
}

int main(void)
{
    volatile int big = 4096;
    volatile int v100 = 100;
    volatile int v200 = 200;
    volatile long long vneg = -1;
    volatile double d = 4095.9;             /* floating to integer does not wrap, out of range is UB */

    /* conversions */
    unsigned _BitInt(12) a = big;             /* init */
    CHECK(a == 0);
    a = big + 5;                              /* assignment */
    CHECK(a == 5);
    CHECK((unsigned _BitInt(12)) big == 0);   /* cast */
    CHECK(pass12(big + 1) == 1);              /* argument */
    CHECK(ret12(big + 2) == 2);               /* return */
    unsigned _BitInt(12) neg = vneg;
    CHECK(neg == 4095);
    unsigned _BitInt(12) fromd = d;
    CHECK(fromd == 4095);

    _BitInt(8) s = v200;                      /* signed wrap on conversion */
    CHECK(s == -56);
    CHECK((_BitInt(8)) v200 == -56);
    CHECK(ret8(v200) == -56);
    _BitInt(8) s2 = (_BitInt(8)) -129;
    CHECK(s2 == 127);

    /* arithmetic results are wrapped, no promotion to int */
    unsigned _BitInt(4) n = 15;
    unsigned _BitInt(4) one = 1;
    CHECK((n + one) / 2 == 0);                /* 16 wraps to 0 before the division */
    CHECK(n * n == 1);                        /* 225 & 0xF */
    CHECK((one << 4) == 0);
    CHECK(~one == 14);
    CHECK(one - n == 2);                      /* -14 mod 16 */

    unsigned _BitInt(12) x = v100;
    CHECK(x * x == 10000 % 4096);
    CHECK(x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x + x == 4200 - 4096);

    _BitInt(8) sa = 100;
    _BitInt(8) sb = 27;
    CHECK(sa + sb == 127);                    /* largest value, signed overflow is UB and not tested */
    unsigned _BitInt(8) ua = 200;
    unsigned _BitInt(8) ub = 100;
    CHECK(ua + ub == 44);                     /* unsigned wraps: 300 - 256 */

    /* compound assignment and ++/-- */
    unsigned _BitInt(4) c = 15;
    c++;
    CHECK(c == 0);
    c--;
    CHECK(c == 15);
    CHECK(c++ == 15);
    CHECK(c == 0);
    CHECK(--c == 15);
    c += 3;
    CHECK(c == 2);
    c -= 5;
    CHECK(c == 13);
    c *= 2;
    CHECK(c == 10);
    c <<= 1;
    CHECK(c == 4);
    c /= vneg;                                /* 4 / -1 in int, converted back */
    CHECK(c == 12);

    /* exact widths need no wrap but must still work */
    unsigned _BitInt(32) e32 = 0xFFFFFFFF;
    e32 += 1;
    CHECK(e32 == 0);
    unsigned _BitInt(64) e64 = 0xFFFFFFFFFFFFFFFFuwb;
    e64 += 1;
    CHECK(e64 == 0);
    unsigned _BitInt(8) e8 = 255;
    unsigned _BitInt(8) one8 = 1;
    CHECK((e8 + one8) == 0);                  /* narrower than int: promotion must not leak */
    CHECK((e8 + 1) == 256);                   /* int outranks _BitInt(8): computed in int */

    /* bool: same lowering problem */
    bool b = big;
    CHECK(b == 1);
    CHECK((bool) big == 1);
    b = 0;
    b += 2;
    CHECK(b == 1);
    b = 0;
    b |= big;
    CHECK(b == 1);

    return failures;
}
