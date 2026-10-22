/*
 * Constant folding of long long long values wider than 64 bits.
 *
 * Every integer constant is held in a struct wide_integer (two unsigned long long),
 * so on the targets where long long long has 128 bits (__SIZEOF_INT128__)
 * the whole range folds exactly. Elsewhere long long long has the size of
 * long long, already covered by parser_long_long_long.c.
 */

#ifdef __SIZEOF_INT128__

#pragma CAKE diagnostic ignored 51 /* unsigned wrap-around is what is tested */

typedef unsigned long long long u128;
typedef long long long i128;

#define TWO_POW_100 ((u128)1 << 100)

/* shifts across the two halves */
static_assert((TWO_POW_100 >> 100) == 1);
static_assert((TWO_POW_100 >> 64) == (1ULL << 36));
static_assert((u128)(TWO_POW_100 >> 100 << 63) == 0x8000000000000000ULL);
static_assert(((i128)-1 >> 100) == -1);
static_assert(((i128)-8 >> 1) == -4);

/* the full range */
static_assert((u128)-1 > (u128)0xFFFFFFFFFFFFFFFFULL);
static_assert(((u128)-1 >> 64) == 0xFFFFFFFFFFFFFFFFULL);
static_assert(((u128)1 << 127) > ((u128)1 << 126));
static_assert((i128)((u128)1 << 127) < 0); /* MIN */
static_assert((u128)-1 + 1 == 0);           /* wraps at 128 bits */

/* (2^64 - 1)^2 = 0xfffffffffffffffe 0000000000000001 */
static_assert((u128)0xFFFFFFFFFFFFFFFFULL * 0xFFFFFFFFFFFFFFFFULL == ((u128)0xFFFFFFFFFFFFFFFEULL << 64) + 1);

/* division and remainder with a 128-bit dividend */
static_assert(TWO_POW_100 / 3 * 3 + TWO_POW_100 % 3 == TWO_POW_100);
static_assert(TWO_POW_100 % 3 == 1);
static_assert((i128)-7 / 2 == -3);
static_assert((i128)-7 % 2 == -1);
static_assert(-((i128)TWO_POW_100 / 2) / 2 == -((i128)TWO_POW_100 >> 2));

/* bitwise */
static_assert((TWO_POW_100 | 1) & 1);
static_assert(((TWO_POW_100 | 1) ^ TWO_POW_100) == 1);
static_assert((~(u128)0 >> 127) == 1);

/* conversions */
static_assert((unsigned long long)(TWO_POW_100 | 5) == 5);       /* truncated to the low half */
static_assert((double)TWO_POW_100 == 1267650600228229401496703205376.0);
static_assert((i128)-1.5 == -1);
static_assert(_Generic(TWO_POW_100 + 1ULL, u128: 1, default: 0));

/* constexpr objects keep the 128 bits */
constexpr u128 big = TWO_POW_100;
static_assert((big >> 96) == 16);
constexpr i128 minus_big = -(i128)big;
static_assert(minus_big < 0 && -minus_big == big);

/* a value wider than the destination is not representable */
constexpr unsigned long long too_big = TWO_POW_100; //lint 1950 error: constant expression is not exactly representable in type
constexpr i128 too_big_signed = (u128)-1;           //lint 1950 error: constant expression is not exactly representable in type
constexpr u128 fits = 0xFFFFFFFFFFFFFFFFULL;
static_assert(fits == 0xFFFFFFFFFFFFFFFFULL);

#endif
