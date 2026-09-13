/*
 * C23 _BitInt(N) - bit-precise integers (N <= 64)
 *
 * The width is part of the type, there is no integer promotion, the
 * conversion rank sits just below the first standard type that holds N bits,
 * and constant folding wraps at N bits. sizeof/alignof are the ones of the
 * standard integer the type is lowered to.
 */

/* 1. size and alignment follow the smallest standard integer that holds N bits */
static_assert(sizeof(_BitInt(7)) == 1);
static_assert(sizeof(_BitInt(8)) == 1);
static_assert(sizeof(_BitInt(12)) == 2);
static_assert(sizeof(_BitInt(16)) == 2);
static_assert(sizeof(_BitInt(24)) == 4);
static_assert(sizeof(_BitInt(32)) == 4);
static_assert(sizeof(_BitInt(40)) == 8);
static_assert(sizeof(unsigned _BitInt(64)) == 8);
static_assert(alignof(_BitInt(12)) == 2);
static_assert(alignof(_BitInt(33)) == 8);

/* 2. the width is part of the type */
static_assert(_Generic((_BitInt(12)) 0, _BitInt(12): 1, default: 2) == 1);
static_assert(_Generic((_BitInt(12)) 0, _BitInt(13): 1, default: 2) == 2);
static_assert(_Generic((_BitInt(12)) 0, unsigned _BitInt(12): 1, default: 2) == 2);
static_assert(_Generic((signed _BitInt(12)) 0, _BitInt(12): 1, default: 2) == 1);
static_assert(_Generic((_BitInt(32)) 0, int: 1, default: 2) == 2);

/* 3. no integer promotion (6.3.1.1) */
static_assert(_Generic((_BitInt(8)) 1 + (_BitInt(8)) 1, _BitInt(8): 1, default: 2) == 1);
static_assert(_Generic(-(_BitInt(4)) 1, _BitInt(4): 1, default: 2) == 1);
static_assert(_Generic(~(unsigned _BitInt(4)) 1, unsigned _BitInt(4): 1, default: 2) == 1);

/* 4. usual arithmetic conversions by rank */
static_assert(_Generic((_BitInt(12)) 1 + 1, int: 1, default: 2) == 1);           /* int outranks _BitInt(12)     */
static_assert(_Generic((_BitInt(40)) 1 + 1, _BitInt(40): 1, default: 2) == 1);   /* _BitInt(40) outranks int      */
static_assert(_Generic((_BitInt(40)) 1 + (_BitInt(24)) 1, _BitInt(40): 1, default: 2) == 1);
static_assert(_Generic((_BitInt(48)) 1 + (unsigned _BitInt(40)) 1, _BitInt(48): 1, default: 2) == 1); /* 48 bits hold every 40-bit unsigned */
static_assert(_Generic((_BitInt(40)) 1 + (unsigned _BitInt(40)) 1, unsigned _BitInt(40): 1, default: 2) == 1);
static_assert(_Generic((_BitInt(40)) 1 + 1LL, long long: 1, default: 2) == 1);   /* long long outranks _BitInt(40) */

/* 5. literals: wb / uwb pick the smallest width that holds the value */
static_assert(_Generic(0wb, _BitInt(2): 1, default: 2) == 1);
static_assert(_Generic(1wb, _BitInt(2): 1, default: 2) == 1);
static_assert(_Generic(2wb, _BitInt(3): 1, default: 2) == 1);
static_assert(_Generic(0uwb, unsigned _BitInt(1): 1, default: 2) == 1);
static_assert(_Generic(255uwb, unsigned _BitInt(8): 1, default: 2) == 1);
static_assert(_Generic(255WB, _BitInt(9): 1, default: 2) == 1);
static_assert(_Generic(0xFFwbu, unsigned _BitInt(8): 1, default: 2) == 1);
static_assert(_Generic(0b1010UWB, unsigned _BitInt(4): 1, default: 2) == 1);
static_assert(_Generic(0xFFFFFFFFFFFFFFFFuwb, unsigned _BitInt(64): 1, default: 2) == 1);

/* 6. constant folding wraps at N bits */
constexpr unsigned _BitInt(12) c1 = (unsigned _BitInt(12)) 4096;
static_assert(c1 == 0);
constexpr _BitInt(8) c2 = (_BitInt(8)) 200;
static_assert(c2 == -56);
constexpr unsigned _BitInt(3) c4 = ~(unsigned _BitInt(3)) 0;
static_assert(c4 == 7);
constexpr _BitInt(40) c5 = (_BitInt(40)) 0x7FFFFFFFFF;
static_assert(c5 == 549755813887);
constexpr unsigned _BitInt(64) c6 = 0xFFFFFFFFFFFFFFFFuwb;
static_assert(c6 == 18446744073709551615ULL);
constexpr _BitInt(40) c7 = -1;
static_assert(c7 < 0);
static_assert((_BitInt(12)) 100 * 3 == 300);
static_assert(((_BitInt(12)) 100 << 4) / 16 == 100);

/* 7. typedef, struct member, array */
typedef unsigned _BitInt(24) u24;
static_assert(sizeof(u24) == 4);
static_assert(_Generic((u24) 0, unsigned _BitInt(24): 1, default: 2) == 1);

struct S
{
    _BitInt(12) m;
    unsigned _BitInt(3) n : 2;
    _BitInt(8) a[4];
};
static_assert(sizeof(struct S) == 8);
static_assert(offsetof(struct S, a) == 3);

constexpr struct S s = { .m = 5, .n = 3, .a = { 1, 2, 3, 4 } };
static_assert(s.m == 5);
static_assert(s.n == 3);
static_assert(s.a[3] == 4);

int main(void)
{
    _BitInt(12) a = 100;
    unsigned _BitInt(12) b = 4095uwb;
    signed _BitInt(40) c = -1;
    return a + b + c;
}
