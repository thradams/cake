/*
 * C23 _BitInt(N) wider than 64 bits.
 *
 * __BITINT_MAXWIDTH__ is the width of __int128 on the targets that have it
 * (gcc/clang x64 and arm64), else the width of long long. The wider _BitInt
 * are lowered to __int128 and their constants fold exactly.
 */

static_assert(__BITINT_MAXWIDTH__ >= 64);
static_assert(__BITINT_MAXWIDTH__ >= sizeof(long long) * 8);

#if __BITINT_MAXWIDTH__ >= 128

#pragma CAKE diagnostic ignored 51 /* unsigned wrap-around is what is tested */

typedef unsigned __int128 u128;

/* 1. size and alignment of the lowered type */
static_assert(sizeof(_BitInt(65)) == sizeof(__int128));
static_assert(sizeof(unsigned _BitInt(100)) == sizeof(__int128));
static_assert(sizeof(_BitInt(128)) == sizeof(__int128));
static_assert(alignof(_BitInt(100)) == alignof(__int128));

/* 2. the width is part of the type */
static_assert(_Generic((_BitInt(100)) 0, _BitInt(100): 1, default: 2) == 1);
static_assert(_Generic((_BitInt(100)) 0, _BitInt(101): 1, default: 2) == 2);
static_assert(_Generic((_BitInt(128)) 0, __int128: 1, default: 2) == 2);

/* 3. no promotion; rank between long long and __int128 */
static_assert(_Generic((_BitInt(100)) 1 + (_BitInt(100)) 1, _BitInt(100): 1, default: 2) == 1);
static_assert(_Generic((_BitInt(100)) 1 + 1LL, _BitInt(100): 1, default: 2) == 1);
static_assert(_Generic((_BitInt(100)) 1 + 1ULL, _BitInt(100): 1, default: 2) == 1); /* holds every unsigned long long */
static_assert(_Generic((_BitInt(100)) 1 + (__int128) 1, __int128: 1, default: 2) == 1);
static_assert(_Generic((unsigned _BitInt(100)) 1 + (__int128) 1, __int128: 1, default: 2) == 1);

/* 4. conversions wrap at N bits */
static_assert((unsigned _BitInt(100)) -1 == ((u128)1 << 100) - 1);
static_assert(((unsigned _BitInt(100)) -1 >> 64) == (1ULL << 36) - 1);
static_assert((_BitInt(100)) ((u128)1 << 99) < 0);
static_assert((_BitInt(100)) ((u128)1 << 100) == 0);
static_assert((_BitInt(100)) (((u128)1 << 100) - 1) == -1);
static_assert((unsigned _BitInt(65)) ((u128)1 << 65) == 0);
static_assert((_BitInt(65)) 0xFFFFFFFFFFFFFFFFULL == 0xFFFFFFFFFFFFFFFFULL);

/* 5. arithmetic in N bits */
constexpr unsigned _BitInt(100) max100 = (unsigned _BitInt(100)) -1;
constexpr unsigned _BitInt(100) one100 = 1;
static_assert(max100 + one100 == 0);
static_assert(one100 - max100 == 2);
static_assert(max100 * max100 == 1);
static_assert(~one100 == max100 - 1);
static_assert((one100 << 99) >> 99 == 1);
static_assert((one100 << 99) / 3 * 3 + (one100 << 99) % 3 == (one100 << 99));

constexpr _BitInt(100) minus_one = -1;
static_assert(minus_one >> 99 == -1);
static_assert(minus_one / 2 == 0);
static_assert(-minus_one == 1);

/* 6. the widest */
static_assert((unsigned _BitInt(128)) -1 == (u128) -1);
static_assert((_BitInt(128)) ((u128)1 << 127) < 0);

_BitInt(129) too_wide; //lint 2060 _BitInt width 129 exceeds the maximum supported width 128

/* 7. wb and uwb literals wider than an unsigned long long */
static_assert(_Generic(0x10000000000000000uwb, unsigned _BitInt(65): 1, default: 0));
static_assert(_Generic(0x10000000000000000wb, _BitInt(66): 1, default: 0));
static_assert(0x10000000000000000uwb == (u128)1 << 64);
static_assert(1267650600228229401496703205376uwb == (u128)1 << 100); /* decimal 2^100 */
static_assert(0b1'0000000000000000000000000000000000000000000000000000000000000000'0000uwb == (u128)1 << 68);
static_assert(_Generic(0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFuwb, unsigned _BitInt(128): 1, default: 0));
static_assert(0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFuwb == (u128)-1);
static_assert(-0x10000000000000000wb < 0);

constexpr unsigned _BitInt(100) literal100 = 0xFFFFFFFFFFFFFFFFFFFFFFFFFuwb;
static_assert(literal100 == ((u128)1 << 100) - 1);

/* a signed 128-bit value needs 129 bits */
int too_wide_literal = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFwb; //lint 1350 integer literal is too large to be represented in a signed _BitInt of the supported width

#endif
