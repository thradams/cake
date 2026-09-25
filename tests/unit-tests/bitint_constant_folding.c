/*
   Constant folding of _BitInt(N): every operation is done in N bits (no
   integer promotion) and the result wraps. bool is compared with
   unsigned _BitInt(1): the conversion to bool is != 0 while the conversion
   to unsigned _BitInt(1) keeps the low bit, and bool promotes to int while
   unsigned _BitInt(1) does not.

   The same values are checked at runtime in
   tests/run-tests/bitint_constexpr_wrap.c.
*/

#pragma CAKE diagnostic ignored 51 /* unsigned wrap-around is what is tested */

/* conversions (casts) */
static_assert((unsigned _BitInt(12)) 4096 == 0);
static_assert((unsigned _BitInt(12)) (4096 + 5) == 5);
static_assert((unsigned _BitInt(12)) -1LL == 4095);
static_assert((unsigned _BitInt(12)) 4095.9 == 4095);
static_assert((_BitInt(8)) 200 == -56);
static_assert((_BitInt(8)) -129 == 127);
static_assert((_BitInt(3)) 4 == -4);
static_assert((_BitInt(3)) 7 == -1);
static_assert((_BitInt(3)) -5 == 3);
static_assert((_BitInt(40)) 0x8000000001LL == 1 - 0x8000000000LL);
static_assert((unsigned _BitInt(63)) -1LL == 0x7fffffffffffffffULL);

/* constexpr objects */
constexpr unsigned _BitInt(12) a = (unsigned _BitInt(12)) 4096;
static_assert(a == 0);
constexpr _BitInt(8) s = (_BitInt(8)) 200;
static_assert(s == -56);

/* arithmetic in N bits */
constexpr unsigned _BitInt(4) n = 15;
constexpr unsigned _BitInt(4) one = 1;
static_assert(n + one == 0);
static_assert((n + one) / 2 == 0);
static_assert(one - n == 2);
static_assert(n * n == 1);
static_assert(~one == 14);
static_assert((unsigned _BitInt(4)) 0 - one == 15);
static_assert(-(unsigned char) 1 == -1);                     /* promoted to int */
static_assert(~(unsigned char) 1 == -2);
static_assert(_Generic(~(unsigned char) 1, int: 1, default: 2) == 1);
static_assert(n / 4 == 3);
static_assert(n % 4 == 3);
static_assert((n & 6) == 6);
static_assert((n ^ one) == 14);
static_assert((one | 8) == 9);

/* shifts: the result has the type of the left operand */
static_assert((one << 4) == 0);
static_assert((one << 3) == 8);
static_assert((n >> 2) == 3);
static_assert(_Generic(one << 1, unsigned _BitInt(4): 1, default: 2) == 1);
static_assert(_Generic(one << 1LL, unsigned _BitInt(4): 1, default: 2) == 1);
static_assert(_Generic((char) 1 << 1LL, int: 1, default: 2) == 1);
static_assert(_Generic(1 >> 1LL, int: 1, default: 2) == 1);
static_assert((unsigned _BitInt(12)) 100 << 6 == 6400 % 4096);
static_assert(((_BitInt(8)) -128 >> 1) == -64);

constexpr unsigned _BitInt(12) x = 100;
static_assert(x * x == 10000 % 4096);

/* exact widths */
static_assert((unsigned _BitInt(32)) 0xFFFFFFFF + (unsigned _BitInt(32)) 1 == 0);
static_assert(0xFFFFFFFFFFFFFFFFuwb + 1uwb == 0);
static_assert((unsigned _BitInt(8)) 255 + (unsigned _BitInt(8)) 1 == 0);
static_assert((unsigned _BitInt(8)) 255 + 1 == 256); /* int outranks _BitInt(8) */

/* bool x unsigned _BitInt(1) */
static_assert((bool) 2 == 1);
static_assert((unsigned _BitInt(1)) 2 == 0);
static_assert((bool) 3 == 1);
static_assert((unsigned _BitInt(1)) 3 == 1);
static_assert((bool) 0.5 == 1);
static_assert((unsigned _BitInt(1)) 0.5 == 0);
static_assert((bool) 4096 == 1);
static_assert((unsigned _BitInt(1)) 4096 == 0);
static_assert((bool) 1 + (bool) 1 == 2);                                      /* promoted to int */
static_assert((unsigned _BitInt(1)) 1 + (unsigned _BitInt(1)) 1 == 0);        /* no promotion */
static_assert(~(unsigned _BitInt(1)) 1 == 0);
static_assert(~(bool) 1 == -2);
static_assert(_Generic((bool) 1 + (bool) 1, int: 1, default: 2) == 1);
static_assert(_Generic((unsigned _BitInt(1)) 1 + (unsigned _BitInt(1)) 1, unsigned _BitInt(1): 1, default: 2) == 1);
static_assert(((unsigned _BitInt(1)) 1 << 1) == 0);
static_assert(((bool) 1 << 1) == 2);
