/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "cake_compat.h"
#include "wide_integer.h"
#include <assert.h>
#include <string.h>

/* 2^WIDE_INTEGER_HALF_BITS, the weight of hi */
#define WIDE_INTEGER_HI_WEIGHT ((long double)ULLONG_MAX + 1.0L)

/* 2^-WIDE_INTEGER_HALF_BITS; halving is exact, and needs no libm (ldexpl is not C89) */
static long double wide_integer_hi_scale(void)
{
    long double scale = 1.0L;
    for (int i = 0; i < WIDE_INTEGER_HALF_BITS; i++)
        scale *= 0.5L;
    return scale;
}

static const struct wide_integer wide_integer_zero = { 0, 0 };

struct wide_integer wide_integer_from_signed(long long value)
{
    struct wide_integer r;
    r.lo = (unsigned long long)value;
    r.hi = value < 0 ? ULLONG_MAX : 0;
    return r;
}

struct wide_integer wide_integer_from_unsigned(unsigned long long value)
{
    struct wide_integer r;
    r.lo = value;
    r.hi = 0;
    return r;
}

struct wide_integer wide_integer_from_long_double(long double value)
{
    if (value < 0)
    {
        return wide_integer_neg(wide_integer_from_long_double(-value));
    }

    /* value / 2^WIDE_INTEGER_HALF_BITS, exact because it is a power of 2 */
    struct wide_integer r;
    r.hi = (unsigned long long)(value * wide_integer_hi_scale());
    r.lo = (unsigned long long)(value - (long double)r.hi * WIDE_INTEGER_HI_WEIGHT);
    return r;
}

long double wide_integer_to_long_double_u(struct wide_integer a)
{
    return (long double)a.hi * WIDE_INTEGER_HI_WEIGHT + (long double)a.lo;
}

long double wide_integer_to_long_double_s(struct wide_integer a)
{
    if (wide_integer_is_negative(a))
    {
        return -wide_integer_to_long_double_u(wide_integer_neg(a));
    }
    return wide_integer_to_long_double_u(a);
}

bool wide_integer_is_zero(struct wide_integer a)
{
    return a.lo == 0 && a.hi == 0;
}

bool wide_integer_is_negative(struct wide_integer a)
{
    return (a.hi >> (WIDE_INTEGER_HALF_BITS - 1)) != 0;
}

bool wide_integer_equal(struct wide_integer a, struct wide_integer b)
{
    return a.lo == b.lo && a.hi == b.hi;
}

int wide_integer_compare_u(struct wide_integer a, struct wide_integer b)
{
    if (a.hi != b.hi)
        return a.hi < b.hi ? -1 : 1;
    if (a.lo != b.lo)
        return a.lo < b.lo ? -1 : 1;
    return 0;
}

int wide_integer_compare_s(struct wide_integer a, struct wide_integer b)
{
    const bool a_negative = wide_integer_is_negative(a);
    const bool b_negative = wide_integer_is_negative(b);

    if (a_negative != b_negative)
        return a_negative ? -1 : 1;

    /* same sign: two's complement keeps the unsigned order */
    return wide_integer_compare_u(a, b);
}

struct wide_integer wide_integer_add(struct wide_integer a, struct wide_integer b)
{
    struct wide_integer r;
    r.lo = a.lo + b.lo;
    r.hi = a.hi + b.hi + (r.lo < a.lo ? 1 : 0);
    return r;
}

struct wide_integer wide_integer_sub(struct wide_integer a, struct wide_integer b)
{
    struct wide_integer r;
    r.lo = a.lo - b.lo;
    r.hi = a.hi - b.hi - (a.lo < b.lo ? 1 : 0);
    return r;
}

struct wide_integer wide_integer_neg(struct wide_integer a)
{
    return wide_integer_sub(wide_integer_zero, a);
}

/* the full product of two unsigned long long, from their halves */
static struct wide_integer wide_integer_mul_half(unsigned long long a, unsigned long long b)
{
    const int quarter = WIDE_INTEGER_HALF_BITS / 2;
    const unsigned long long mask = ULLONG_MAX >> quarter;

    const unsigned long long a_lo = a & mask;
    const unsigned long long a_hi = a >> quarter;
    const unsigned long long b_lo = b & mask;
    const unsigned long long b_hi = b >> quarter;

    const unsigned long long p0 = a_lo * b_lo;
    const unsigned long long p1 = a_lo * b_hi;
    const unsigned long long p2 = a_hi * b_lo;
    const unsigned long long p3 = a_hi * b_hi;

    /* the middle column, at most 3 * mask, cannot overflow */
    const unsigned long long middle = (p0 >> quarter) + (p1 & mask) + (p2 & mask);

    struct wide_integer r;
    r.lo = (p0 & mask) | (middle << quarter);
    r.hi = p3 + (p1 >> quarter) + (p2 >> quarter) + (middle >> quarter);
    return r;
}

struct wide_integer wide_integer_mul(struct wide_integer a, struct wide_integer b)
{
    /* modulo 2^WIDE_INTEGER_BITS, the hi * hi part is shifted out */
    struct wide_integer r = wide_integer_mul_half(a.lo, b.lo);
    r.hi += a.lo * b.hi + a.hi * b.lo;
    return r;
}

struct wide_integer wide_integer_not(struct wide_integer a)
{
    struct wide_integer r;
    r.lo = ~a.lo;
    r.hi = ~a.hi;
    return r;
}

struct wide_integer wide_integer_and(struct wide_integer a, struct wide_integer b)
{
    struct wide_integer r;
    r.lo = a.lo & b.lo;
    r.hi = a.hi & b.hi;
    return r;
}

struct wide_integer wide_integer_or(struct wide_integer a, struct wide_integer b)
{
    struct wide_integer r;
    r.lo = a.lo | b.lo;
    r.hi = a.hi | b.hi;
    return r;
}

struct wide_integer wide_integer_xor(struct wide_integer a, struct wide_integer b)
{
    struct wide_integer r;
    r.lo = a.lo ^ b.lo;
    r.hi = a.hi ^ b.hi;
    return r;
}

struct wide_integer wide_integer_shl(struct wide_integer a, int count)
{
    assert(count >= 0 && count < WIDE_INTEGER_BITS);

    struct wide_integer r;
    if (count == 0)
    {
        r = a;
    }
    else if (count >= WIDE_INTEGER_HALF_BITS)
    {
        r.hi = a.lo << (count - WIDE_INTEGER_HALF_BITS);
        r.lo = 0;
    }
    else
    {
        r.hi = (a.hi << count) | (a.lo >> (WIDE_INTEGER_HALF_BITS - count));
        r.lo = a.lo << count;
    }
    return r;
}

struct wide_integer wide_integer_shr_u(struct wide_integer a, int count)
{
    assert(count >= 0 && count < WIDE_INTEGER_BITS);

    struct wide_integer r;
    if (count == 0)
    {
        r = a;
    }
    else if (count >= WIDE_INTEGER_HALF_BITS)
    {
        r.lo = a.hi >> (count - WIDE_INTEGER_HALF_BITS);
        r.hi = 0;
    }
    else
    {
        r.lo = (a.lo >> count) | (a.hi << (WIDE_INTEGER_HALF_BITS - count));
        r.hi = a.hi >> count;
    }
    return r;
}

struct wide_integer wide_integer_shr_s(struct wide_integer a, int count)
{
    assert(count >= 0 && count < WIDE_INTEGER_BITS);

    if (!wide_integer_is_negative(a))
    {
        return wide_integer_shr_u(a, count);
    }

    /* ~(~a >> n) shifts in ones, without relying on the host >> of a negative */
    return wide_integer_not(wide_integer_shr_u(wide_integer_not(a), count));
}

bool wide_integer_divmod_u(struct wide_integer a, struct wide_integer b, _Out struct wide_integer* quotient, _Out struct wide_integer* remainder)
{
    *quotient = wide_integer_zero;
    *remainder = wide_integer_zero;

    if (wide_integer_is_zero(b))
    {
        return false;
    }

    /* restoring division, one bit of the dividend at a time */
    struct wide_integer q = wide_integer_zero;
    struct wide_integer r = wide_integer_zero;
    for (int i = WIDE_INTEGER_BITS - 1; i >= 0; i--)
    {
        r = wide_integer_shl(r, 1);
        r.lo |= wide_integer_shr_u(a, i).lo & 1;
        if (wide_integer_compare_u(r, b) >= 0)
        {
            r = wide_integer_sub(r, b);
            if (i >= WIDE_INTEGER_HALF_BITS)
                q.hi |= 1ULL << (i - WIDE_INTEGER_HALF_BITS);
            else
                q.lo |= 1ULL << i;
        }
    }

    *quotient = q;
    *remainder = r;
    return true;
}

bool wide_integer_divmod_s(struct wide_integer a, struct wide_integer b, _Out struct wide_integer* quotient, _Out struct wide_integer* remainder)
{
    const bool a_negative = wide_integer_is_negative(a);
    const bool b_negative = wide_integer_is_negative(b);

    /* MIN negates to itself, and as unsigned it is still 2^(WIDE_INTEGER_BITS - 1), so this works for it too */
    if (!wide_integer_divmod_u(a_negative ? wide_integer_neg(a) : a,
                        b_negative ? wide_integer_neg(b) : b,
                        quotient,
                        remainder))
    {
        return false;
    }

    if (a_negative != b_negative)
        *quotient = wide_integer_neg(*quotient);

    if (a_negative)
        *remainder = wide_integer_neg(*remainder);

    return true;
}

struct wide_integer wide_integer_wrap_u(struct wide_integer a, int width)
{
    assert(width >= 1 && width <= WIDE_INTEGER_BITS);

    struct wide_integer r = a;
    if (width < WIDE_INTEGER_HALF_BITS)
    {
        r.lo &= (1ULL << width) - 1;
        r.hi = 0;
    }
    else if (width == WIDE_INTEGER_HALF_BITS)
    {
        r.hi = 0;
    }
    else if (width < WIDE_INTEGER_BITS)
    {
        r.hi &= (1ULL << (width - WIDE_INTEGER_HALF_BITS)) - 1;
    }
    return r;
}

struct wide_integer wide_integer_wrap_s(struct wide_integer a, int width)
{
    assert(width >= 1 && width <= WIDE_INTEGER_BITS);

    if (width == WIDE_INTEGER_BITS)
        return a;

    struct wide_integer r = wide_integer_wrap_u(a, width);

    /* bit width - 1 is the sign, copy it into the bits above */
    const bool negative = wide_integer_shr_u(r, width - 1).lo & 1;
    if (negative)
    {
        r = wide_integer_or(r, wide_integer_not(wide_integer_wrap_u(wide_integer_not(wide_integer_zero), width)));
    }
    return r;
}

/* the digits of a; buffer has room for them */
static int wide_integer_write_digits(struct wide_integer a, char* buffer)
{
    /* digits come out from the last one */
    char digits[WIDE_INTEGER_TO_STRING_MIN_SIZE] = { 0 };
    int count = 0;
    const struct wide_integer ten = { 10, 0 };
    do
    {
        struct wide_integer q = { 0 };
        struct wide_integer r = { 0 };
        wide_integer_divmod_u(a, ten, &q, &r);
        digits[count++] = (char)('0' + r.lo);
        a = q;
    }
    while (!wide_integer_is_zero(a));

    for (int i = 0; i < count; i++)
        buffer[i] = digits[count - 1 - i];
    buffer[count] = '\0';
    return count;
}

int wide_integer_to_string_u(struct wide_integer a, char* buffer, int size)
{
    if (size < WIDE_INTEGER_TO_STRING_MIN_SIZE)
    {
        if (size > 0)
            buffer[0] = '\0';
        return 0;
    }
    return wide_integer_write_digits(a, buffer);
}

int wide_integer_to_string_s(struct wide_integer a, char* buffer, int size)
{
    if (size < WIDE_INTEGER_TO_STRING_MIN_SIZE)
    {
        if (size > 0)
            buffer[0] = '\0';
        return 0;
    }

    if (wide_integer_is_negative(a))
    {
        /* MIN negates to itself, and as unsigned it prints right */
        buffer[0] = '-';
        return 1 + wide_integer_write_digits(wide_integer_neg(a), buffer + 1);
    }
    return wide_integer_write_digits(a, buffer);
}

static int wide_integer_digit_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

struct wide_integer wide_integer_from_digits(const char* text, int base, _Out bool* overflow)
{
    assert(base == 2 || base == 8 || base == 10 || base == 16);

    *overflow = false;

    const struct wide_integer wide_base = wide_integer_from_unsigned((unsigned long long)base);
    struct wide_integer r = wide_integer_zero;

    for (const char* p = text; *p; p++)
    {
        const int digit = wide_integer_digit_value(*p);
        if (digit < 0 || digit >= base)
            break;

        /* r * base + digit did not fit when going back does not give r */
        const struct wide_integer next =
            wide_integer_add(wide_integer_mul(r, wide_base), wide_integer_from_unsigned((unsigned long long)digit));

        struct wide_integer quotient = { 0 };
        struct wide_integer remainder = { 0 };
        wide_integer_divmod_u(next, wide_base, &quotient, &remainder);
        if (!wide_integer_equal(quotient, r) || remainder.lo != (unsigned long long)digit)
            *overflow = true;

        r = next;
    }
    return r;
}

#ifdef TEST
#include <stdio.h>
#include "unit_test.h"

static struct wide_integer wide_integer_make(unsigned long long hi, unsigned long long lo)
{
    struct wide_integer r;
    r.hi = hi;
    r.lo = lo;
    return r;
}

/* the most negative value, only the sign bit */
static struct wide_integer wide_integer_min(void)
{
    return wide_integer_make(1ULL << (WIDE_INTEGER_HALF_BITS - 1), 0);
}

void wide_integer_arithmetic_test(void)
{
    const struct wide_integer one = wide_integer_from_unsigned(1);
    const struct wide_integer minus_one = wide_integer_from_signed(-1);
    const struct wide_integer max_half = wide_integer_from_unsigned(ULLONG_MAX);

    assert(wide_integer_equal(minus_one, wide_integer_make(ULLONG_MAX, ULLONG_MAX)));
    assert(wide_integer_equal(wide_integer_add(max_half, one), wide_integer_make(1, 0)));
    assert(wide_integer_equal(wide_integer_sub(wide_integer_make(1, 0), one), max_half));
    assert(wide_integer_equal(wide_integer_add(minus_one, one), wide_integer_from_unsigned(0)));
    assert(wide_integer_equal(wide_integer_neg(one), minus_one));

    /* (2^h - 1)^2 = (2^h - 2) * 2^h + 1 */
    assert(wide_integer_equal(wide_integer_mul(max_half, max_half), wide_integer_make(ULLONG_MAX - 1, 1)));
    assert(wide_integer_equal(wide_integer_mul(wide_integer_from_signed(-3), wide_integer_from_signed(7)), wide_integer_from_signed(-21)));
    assert(wide_integer_equal(wide_integer_mul(wide_integer_make(1, 0), wide_integer_make(1, 0)), wide_integer_from_unsigned(0))); /* 2^WIDE_INTEGER_BITS wraps */
}

void wide_integer_shift_test(void)
{
    const struct wide_integer one = wide_integer_from_unsigned(1);
    const int n = WIDE_INTEGER_HALF_BITS + WIDE_INTEGER_HALF_BITS / 2;

    assert(wide_integer_equal(wide_integer_shl(one, n), wide_integer_make(1ULL << (n - WIDE_INTEGER_HALF_BITS), 0)));
    assert(wide_integer_equal(wide_integer_shl(one, WIDE_INTEGER_HALF_BITS), wide_integer_make(1, 0)));
    assert(wide_integer_equal(wide_integer_shl(one, 0), one));
    assert(wide_integer_equal(wide_integer_shr_u(wide_integer_make(1ULL << (n - WIDE_INTEGER_HALF_BITS), 0), n), one));
    assert(wide_integer_equal(wide_integer_shr_u(wide_integer_from_signed(-1), WIDE_INTEGER_BITS - 1), one));
    assert(wide_integer_equal(wide_integer_shr_s(wide_integer_from_signed(-1), WIDE_INTEGER_BITS - 1), wide_integer_from_signed(-1)));
    assert(wide_integer_equal(wide_integer_shr_s(wide_integer_from_signed(-8), 1), wide_integer_from_signed(-4)));
    assert(wide_integer_equal(wide_integer_shr_s(wide_integer_from_signed(-7), 1), wide_integer_from_signed(-4)));
}

void wide_integer_compare_test(void)
{
    assert(wide_integer_compare_s(wide_integer_from_signed(-1), wide_integer_from_signed(0)) < 0);
    assert(wide_integer_compare_u(wide_integer_from_signed(-1), wide_integer_from_signed(0)) > 0);
    assert(wide_integer_compare_s(wide_integer_min(), wide_integer_from_signed(-1)) < 0);
    assert(wide_integer_compare_u(wide_integer_make(1, 0), wide_integer_make(0, ULLONG_MAX)) > 0);
    assert(wide_integer_compare_s(wide_integer_from_signed(5), wide_integer_from_signed(5)) == 0);
}

void wide_integer_divmod_test(void)
{
    struct wide_integer q = { 0 };
    struct wide_integer r = { 0 };

    /* 2^n = 3 * q + r */
    const struct wide_integer n = wide_integer_shl(wide_integer_from_unsigned(1), WIDE_INTEGER_BITS - 28);
    assert(wide_integer_divmod_u(n, wide_integer_from_unsigned(3), &q, &r));
    assert(wide_integer_compare_u(r, wide_integer_from_unsigned(3)) < 0);
    assert(wide_integer_equal(wide_integer_add(wide_integer_mul(q, wide_integer_from_unsigned(3)), r), n));

    assert(wide_integer_divmod_s(wide_integer_from_signed(-7), wide_integer_from_signed(2), &q, &r));
    assert(wide_integer_equal(q, wide_integer_from_signed(-3)));
    assert(wide_integer_equal(r, wide_integer_from_signed(-1)));

    assert(wide_integer_divmod_s(wide_integer_from_signed(7), wide_integer_from_signed(-2), &q, &r));
    assert(wide_integer_equal(q, wide_integer_from_signed(-3)));
    assert(wide_integer_equal(r, wide_integer_from_signed(1)));

    /* MIN / -1 wraps to MIN */
    assert(wide_integer_divmod_s(wide_integer_min(), wide_integer_from_signed(-1), &q, &r));
    assert(wide_integer_equal(q, wide_integer_min()));
    assert(wide_integer_is_zero(r));

    assert(!wide_integer_divmod_u(n, wide_integer_from_unsigned(0), &q, &r));
    assert(wide_integer_is_zero(q) && wide_integer_is_zero(r));
}

void wide_integer_wrap_test(void)
{
    const int w = WIDE_INTEGER_HALF_BITS + 36;

    assert(wide_integer_equal(wide_integer_wrap_s(wide_integer_from_unsigned(0xFF), 8), wide_integer_from_signed(-1)));
    assert(wide_integer_equal(wide_integer_wrap_u(wide_integer_from_signed(-1), 8), wide_integer_from_unsigned(0xFF)));
    assert(wide_integer_equal(wide_integer_wrap_u(wide_integer_from_signed(-1), w), wide_integer_make((1ULL << 36) - 1, ULLONG_MAX)));
    assert(wide_integer_equal(wide_integer_wrap_s(wide_integer_make(1ULL << 35, 0), w), wide_integer_make(ULLONG_MAX << 35, 0)));
    assert(wide_integer_equal(wide_integer_wrap_u(wide_integer_from_signed(-1), WIDE_INTEGER_HALF_BITS), wide_integer_from_unsigned(ULLONG_MAX)));
    assert(wide_integer_equal(wide_integer_wrap_s(wide_integer_from_unsigned(ULLONG_MAX), WIDE_INTEGER_HALF_BITS), wide_integer_from_signed(-1)));
    assert(wide_integer_equal(wide_integer_wrap_s(wide_integer_from_signed(-1), WIDE_INTEGER_BITS), wide_integer_from_signed(-1)));
}

void wide_integer_conversion_test(void)
{
    assert(wide_integer_equal(wide_integer_from_long_double(-3.9L), wide_integer_from_signed(-3)));
    assert(wide_integer_equal(wide_integer_from_long_double(WIDE_INTEGER_HI_WEIGHT), wide_integer_make(1, 0)));
    assert(wide_integer_to_long_double_u(wide_integer_make(1, 0)) == WIDE_INTEGER_HI_WEIGHT);
    assert(wide_integer_to_long_double_s(wide_integer_from_signed(-5)) == -5.0L);

    char buffer[WIDE_INTEGER_TO_STRING_MIN_SIZE] = { 0 };

    wide_integer_to_string_s(wide_integer_from_unsigned(0), buffer, sizeof buffer);
    assert(strcmp(buffer, "0") == 0);

    wide_integer_to_string_s(wide_integer_from_signed(-42), buffer, sizeof buffer);
    assert(strcmp(buffer, "-42") == 0);

    wide_integer_to_string_u(wide_integer_from_unsigned(1234567890), buffer, sizeof buffer);
    assert(strcmp(buffer, "1234567890") == 0);

#if ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
    wide_integer_to_string_u(wide_integer_from_signed(-1), buffer, sizeof buffer);
    assert(strcmp(buffer, "340282366920938463463374607431768211455") == 0);

    wide_integer_to_string_s(wide_integer_min(), buffer, sizeof buffer);
    assert(strcmp(buffer, "-170141183460469231731687303715884105728") == 0);
#endif

    char small[8] = { 0 };
    assert(wide_integer_to_string_u(wide_integer_from_unsigned(1), small, sizeof small) == 0);
    assert(small[0] == '\0');
}

void wide_integer_from_digits_test(void)
{
    bool overflow = true;

    assert(wide_integer_equal(wide_integer_from_digits("12345", 10, &overflow), wide_integer_from_unsigned(12345)));
    assert(!overflow);

    assert(wide_integer_equal(wide_integer_from_digits("ff", 16, &overflow), wide_integer_from_unsigned(255)));
    assert(wide_integer_equal(wide_integer_from_digits("777", 8, &overflow), wide_integer_from_unsigned(511)));
    assert(wide_integer_equal(wide_integer_from_digits("101", 2, &overflow), wide_integer_from_unsigned(5)));

    /* stops at the first character that is not a digit of the base */
    assert(wide_integer_equal(wide_integer_from_digits("12uwb", 10, &overflow), wide_integer_from_unsigned(12)));
    assert(wide_integer_equal(wide_integer_from_digits("129", 8, &overflow), wide_integer_from_unsigned(10)));

    /* 2^HALF_BITS, one past an unsigned long long */
    char text[WIDE_INTEGER_BITS + 2] = { 0 };
    text[0] = '1';
    for (int i = 1; i <= WIDE_INTEGER_HALF_BITS; i++)
        text[i] = '0';
    assert(wide_integer_equal(wide_integer_from_digits(text, 2, &overflow), wide_integer_make(1, 0)));
    assert(!overflow);

    /* WIDE_INTEGER_BITS ones fit, one more does not */
    for (int i = 0; i < WIDE_INTEGER_BITS; i++)
        text[i] = '1';
    text[WIDE_INTEGER_BITS] = '\0';
    assert(wide_integer_equal(wide_integer_from_digits(text, 2, &overflow), wide_integer_from_signed(-1)));
    assert(!overflow);

    text[WIDE_INTEGER_BITS] = '1';
    text[WIDE_INTEGER_BITS + 1] = '\0';
    wide_integer_from_digits(text, 2, &overflow);
    assert(overflow);

#if ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
    assert(wide_integer_equal(wide_integer_from_digits("340282366920938463463374607431768211455", 10, &overflow), wide_integer_from_signed(-1)));
    assert(!overflow);
    wide_integer_from_digits("340282366920938463463374607431768211456", 10, &overflow);
    assert(overflow);
#endif
}

#endif
