/*
   _BitInt(N) conversions and arithmetic on integer constants are wrapped by
   cake at compile time and emitted as a single value. The same operations on
   volatile objects are wrapped by the emitted C, computed in int when N is
   below the int width of the target and in long long otherwise. Both must
   give the same values.
*/
#include <stdio.h>
#include <limits.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

int main(void)
{
    /* constants: signed */
    _BitInt(3) s3;
    s3 = 2;
    CHECK(s3 == 2);
    s3 = 3;
    CHECK(s3 == 3);
    s3 = 4;
    CHECK(s3 == -4);
    s3 = 7;
    CHECK(s3 == -1);
    s3 = -5;
    CHECK(s3 == 3);
    s3 = (_BitInt(3)) 12;
    CHECK(s3 == -4);

    _BitInt(8) s8 = (_BitInt(8)) 200;
    CHECK(s8 == -56);
    s8 = -129;
    CHECK(s8 == 127);
    s8 = 0x7fffffffffffffffLL;
    CHECK(s8 == -1);

    /* constants: unsigned */
    unsigned _BitInt(5) u5 = (unsigned _BitInt(5)) 40;
    CHECK(u5 == 8);
    u5 = -1;
    CHECK(u5 == 31);
    unsigned _BitInt(12) u12 = (unsigned _BitInt(12)) 0xffffffffffffffffULL;
    CHECK(u12 == 4095);

    /* compile time checks of the same values */
    static_assert((_BitInt(3)) 7 == -1);
    static_assert((unsigned _BitInt(5)) 40 == 8);
    static_assert((_BitInt(8)) 200 == -56);
    constexpr _BitInt(3) cs3 = (_BitInt(3)) 4;
    static_assert(cs3 == -4);

    /* runtime, wide widths: computed in long long */
    volatile long long vbig = 0x123456789abcdefLL;
    volatile long long vm1 = -1;
    _BitInt(40) s40 = vbig;
    CHECK(s40 == (_BitInt(40)) 0x123456789abcdefLL);
    CHECK(s40 == 0x6789abcdefLL);
    volatile long long vneg40 = 0x8000000001LL;
    s40 = vneg40;
    CHECK(s40 == 1 - 0x8000000000LL);
    CHECK(s40 == (_BitInt(40)) 0x8000000001LL);
    unsigned _BitInt(40) u40 = vm1;
    CHECK(u40 == 0xffffffffffULL);
    _BitInt(63) s63 = vm1;
    CHECK(s63 == -1);
    unsigned _BitInt(63) u63 = vm1;
    CHECK(u63 == 0x7fffffffffffffffULL);

#if INT_MAX == 0x7fffffff
    /* 32 bit int: _BitInt(31) is the widest computed in int */
    volatile long long v31 = 0x80000000LL + 5;
    _BitInt(31) s31 = v31;
    CHECK(s31 == 5);
    CHECK(s31 == (_BitInt(31)) (0x80000000LL + 5));
    volatile int vi = 0x40000000;
    s31 = vi;
    CHECK(s31 == -0x40000000);
    CHECK(s31 == (_BitInt(31)) 0x40000000);
    unsigned _BitInt(31) u31 = vm1;
    CHECK(u31 == 0x7fffffff);
    CHECK(u31 == (unsigned _BitInt(31)) -1);

    /* _BitInt(33) is the narrowest computed in long long */
    volatile long long v33 = 0x100000000LL;
    _BitInt(33) s33 = v33;
    CHECK(s33 == -0x100000000LL);
    CHECK(s33 == (_BitInt(33)) 0x100000000LL);
#endif

    /*
      runtime floating source goes through long long. An out of range value
      is undefined behavior (no wrap for floating to integer), so only values
      that fit are tested.
    */
    volatile double d = -1.5;
    _BitInt(3) fs = d * 2;
    CHECK(fs == -3);
    volatile double d2 = 4095.9;
    unsigned _BitInt(12) fd = d2;
    CHECK(fd == 4095);

    return failures;
}
