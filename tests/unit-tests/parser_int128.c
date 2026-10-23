/*
 * __int128 - the 128-bit integer of gcc and clang, the rank above long long.
 *
 * It exists only on the targets with 128-bit integers, where
 * __SIZEOF_INT128__ is defined; elsewhere it is an error
 * (int128_not_supported_on_target.c).
 */

#ifdef __SIZEOF_INT128__

/* 1. accepted combinations */
__int128 a1;
signed __int128 a2;
unsigned __int128 a3;

static_assert(_Generic(a1, __int128: 1, default: 0));
static_assert(_Generic(a2, __int128: 1, default: 0));
static_assert(_Generic(a3, unsigned __int128: 1, default: 0));

/* 2. size and alignment */
static_assert(__SIZEOF_INT128__ == 16);
static_assert(sizeof(__int128) == 16);
static_assert(sizeof(unsigned __int128) == 16);
static_assert(alignof(__int128) == 16);

/* 3. a type of its own */
static_assert(_Generic((__int128) 0, long long: 1, default: 2) == 2);
static_assert(_Generic((long long) 0, __int128: 1, default: 2) == 2);
static_assert(_Generic((__int128) 0, unsigned __int128: 1, default: 2) == 2);

/* 4. no integer promotion, rank above long long */
static_assert(_Generic((__int128) 1 + 1, __int128: 1, default: 2) == 1);
static_assert(_Generic((__int128) 1 + 1LL, __int128: 1, default: 2) == 1);
static_assert(_Generic((__int128) 1 + 1ULL, __int128: 1, default: 2) == 1); /* holds every unsigned long long */
static_assert(_Generic((unsigned __int128) 1 + 1LL, unsigned __int128: 1, default: 2) == 1);

/* 5. constant folding of values that fit in 64 bits */
static_assert((__int128) 3 + 4 == 7);
static_assert((__int128) -5 * 3 == -15);
static_assert((__int128) 100 / 7 == 14);
static_assert((__int128) 100 % 7 == 2);
static_assert((unsigned __int128) -1 > 0);
static_assert((__int128) -1 < 0);

constexpr __int128 k = 10 * 3;
static_assert(k == 30);

/* 6. the gcc typedefs */
static_assert(sizeof(__int128_t) == 16);
static_assert(sizeof(__uint128_t) == 16);
static_assert(_Generic((__int128_t) 0, __int128: 1, default: 2) == 1);
static_assert(_Generic((__uint128_t) 0, unsigned __int128: 1, default: 2) == 1);

#endif
