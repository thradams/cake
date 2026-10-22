/*
 * long long long - cake extension, the rank above long long
 *
 * It is never narrower than long long; the target gives its size
 * (platform::long_long_long_n_bits): 128 bits on gcc/clang x64 and arm64,
 * the size of long long elsewhere. __int128 is an alias of it, like __int64
 * is on msvc, and __SIZEOF_INT128__ is defined only when it has 128 bits.
 *
 * Constant values wider than 64 bits are not folded yet.
 */

/* 1. accepted combinations, all the same type */
long long long a1;
signed long long long a2;
long long long int a3;
signed long long long int a4;
unsigned long long long a5;
unsigned long long long int a6;

static_assert(_Generic(a1, long long long: 1, default: 0));
static_assert(_Generic(a2, long long long: 1, default: 0));
static_assert(_Generic(a3, long long long: 1, default: 0));
static_assert(_Generic(a4, long long long: 1, default: 0));
static_assert(_Generic(a5, unsigned long long long: 1, default: 0));
static_assert(_Generic(a6, unsigned long long long: 1, default: 0));

/* 2. never narrower than long long */
static_assert(sizeof(long long long) >= sizeof(long long));
static_assert(sizeof(unsigned long long long) == sizeof(long long long));
static_assert(alignof(long long long) >= alignof(long long));

/* 3. a type of its own, even when it has the size of long long */
static_assert(_Generic((long long long) 0, long long: 1, default: 2) == 2);
static_assert(_Generic((long long) 0, long long long: 1, default: 2) == 2);
static_assert(_Generic((long long long) 0, unsigned long long long: 1, default: 2) == 2);

/* 4. __int128 is an alias */
static_assert(_Generic((__int128) 0, long long long: 1, default: 2) == 1);
static_assert(_Generic((unsigned __int128) 0, unsigned long long long: 1, default: 2) == 1);
static_assert(_Generic((signed __int128) 0, long long long: 1, default: 2) == 1);

/* 5. no integer promotion, rank above long long */
static_assert(_Generic((long long long) 1 + 1, long long long: 1, default: 2) == 1);
static_assert(_Generic((long long long) 1 + 1LL, long long long: 1, default: 2) == 1);
static_assert(_Generic((unsigned long long long) 1 + 1LL, unsigned long long long: 1, default: 2) == 1);

/*
   long long long + unsigned long long: when both have the same size the signed
   one cannot hold every unsigned value, so the result is unsigned long long long;
   when long long long is wider it holds them, so it stays long long long.
*/
static_assert(_Generic((long long long) 1 + 1ULL,
                       unsigned long long long: 1,
                       long long long: 2,
                       default: 0) == (sizeof(long long long) == sizeof(long long) ? 1 : 2));

/* 6. constant folding of values that fit in 64 bits */
static_assert((long long long) 3 + 4 == 7);
static_assert((long long long) -5 * 3 == -15);
static_assert((long long long) 100 / 7 == 14);
static_assert((long long long) 100 % 7 == 2);
static_assert((unsigned long long long) -1 > 0);
static_assert((long long long) -1 < 0);

constexpr long long long k = 10 * 3;
static_assert(k == 30);

/* 7. 128 bits */
#ifdef __SIZEOF_INT128__
static_assert(__SIZEOF_INT128__ == 16);
static_assert(sizeof(__int128) == 16);
static_assert(alignof(__int128) == 16);
static_assert(sizeof(__int128_t) == 16);
static_assert(sizeof(__uint128_t) == 16);
static_assert(_Generic((__int128_t) 0, long long long: 1, default: 2) == 1);
static_assert(_Generic((__uint128_t) 0, unsigned long long long: 1, default: 2) == 1);
#else
static_assert(sizeof(long long long) == sizeof(long long));
#endif
