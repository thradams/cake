/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "cake_compat.h"
#include "integer128.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* the bits used in the top word: 128 - the bits of the words below it */
#define INTEGER128_TOP_BITS (INTEGER128_BITS - (INTEGER128_WORDS - 1) * INTEGER128_WORD_BITS)

/* 2^INTEGER128_WORD_BITS, the weight of one word */
#define INTEGER128_WORD_WEIGHT ((long double)ULLONG_MAX + 1.0L)

static const struct integer128 integer128_zero = { 0 };

static unsigned long long integer128_top_mask(void)
{
    return ULLONG_MAX >> (INTEGER128_WORD_BITS - INTEGER128_TOP_BITS);
}

/* clears the bits above bit 127 */
static struct integer128 integer128_normalize(struct integer128 r)
{
    r.w[INTEGER128_WORDS - 1] &= integer128_top_mask();
    return r;
}

static bool integer128_get_bit(struct integer128 a, int i)
{
    return (a.w[i / INTEGER128_WORD_BITS] >> (i % INTEGER128_WORD_BITS)) & 1;
}

static void integer128_set_bit(struct integer128* a, int i)
{
    a->w[i / INTEGER128_WORD_BITS] |= 1ULL << (i % INTEGER128_WORD_BITS);
}

struct integer128 integer128_from_signed(long long value)
{
    struct integer128 r = integer128_zero;
    r.w[0] = (unsigned long long)value;
    for (int i = 1; i < INTEGER128_WORDS; i++)
        r.w[i] = value < 0 ? ULLONG_MAX : 0;
    return integer128_normalize(r);
}

struct integer128 integer128_from_unsigned(unsigned long long value)
{
    struct integer128 r = integer128_zero;
    r.w[0] = value;
    return integer128_normalize(r);
}

unsigned long long integer128_to_ull(struct integer128 a)
{
    return a.w[0];
}

long long integer128_to_ll(struct integer128 a)
{
    return (long long)a.w[0];
}

bool integer128_fits_ull(struct integer128 a)
{
    return integer128_equal(a, integer128_from_unsigned(integer128_to_ull(a)));
}

bool integer128_fits_ll(struct integer128 a)
{
    return integer128_equal(a, integer128_from_signed(integer128_to_ll(a)));
}

struct integer128 integer128_from_long_double(long double value)
{
    if (value < 0)
    {
        return integer128_neg(integer128_from_long_double(-value));
    }

    /* from the top word down; dividing by a power of 2 is exact, and needs no libm */
    struct integer128 r = integer128_zero;
    for (int i = INTEGER128_WORDS - 1; i >= 0; i--)
    {
        long double weight = 1.0L;
        for (int k = 0; k < i; k++)
            weight *= INTEGER128_WORD_WEIGHT;

        r.w[i] = (unsigned long long)(value / weight); //lint 36 weight is a power of 2, never 0
        value -= (long double)r.w[i] * weight;
    }
    return integer128_normalize(r);
}

long double integer128_to_long_double_u(struct integer128 a)
{
    long double r = 0;
    for (int i = INTEGER128_WORDS - 1; i >= 0; i--)
        r = r * INTEGER128_WORD_WEIGHT + (long double)a.w[i];
    return r;
}

long double integer128_to_long_double_s(struct integer128 a)
{
    if (integer128_is_negative(a))
    {
        return -integer128_to_long_double_u(integer128_neg(a));
    }
    return integer128_to_long_double_u(a);
}

bool integer128_is_zero(struct integer128 a)
{
    for (int i = 0; i < INTEGER128_WORDS; i++)
    {
        if (a.w[i] != 0)
            return false;
    }
    return true;
}

bool integer128_is_negative(struct integer128 a)
{
    return integer128_get_bit(a, INTEGER128_BITS - 1);
}

bool integer128_equal(struct integer128 a, struct integer128 b)
{
    for (int i = 0; i < INTEGER128_WORDS; i++)
    {
        if (a.w[i] != b.w[i])
            return false;
    }
    return true;
}

int integer128_compare_u(struct integer128 a, struct integer128 b)
{
    for (int i = INTEGER128_WORDS - 1; i >= 0; i--)
    {
        if (a.w[i] != b.w[i])
            return a.w[i] < b.w[i] ? -1 : 1;
    }
    return 0;
}

int integer128_compare_s(struct integer128 a, struct integer128 b)
{
    const bool a_negative = integer128_is_negative(a);
    const bool b_negative = integer128_is_negative(b);

    if (a_negative != b_negative)
        return a_negative ? -1 : 1;

    /* same sign: two's complement keeps the unsigned order */
    return integer128_compare_u(a, b);
}

struct integer128 integer128_add(struct integer128 a, struct integer128 b)
{
    struct integer128 r = integer128_zero;
    unsigned long long carry = 0;
    for (int i = 0; i < INTEGER128_WORDS; i++)
    {
        const unsigned long long s = a.w[i] + b.w[i];
        const unsigned long long t = s + carry;
        carry = (s < a.w[i] || t < s) ? 1 : 0;
        r.w[i] = t;
    }
    return integer128_normalize(r);
}

struct integer128 integer128_sub(struct integer128 a, struct integer128 b)
{
    struct integer128 r = integer128_zero;
    unsigned long long borrow = 0;
    for (int i = 0; i < INTEGER128_WORDS; i++)
    {
        const unsigned long long d = a.w[i] - b.w[i];
        const unsigned long long t = d - borrow;
        borrow = (a.w[i] < b.w[i] || d < borrow) ? 1 : 0;
        r.w[i] = t;
    }
    return integer128_normalize(r);
}

struct integer128 integer128_neg(struct integer128 a)
{
    return integer128_sub(integer128_zero, a);
}

/* the full product of two words, from their halves */
static void integer128_mul_word(unsigned long long a, unsigned long long b, unsigned long long* lo, unsigned long long* hi)
{
    const int half = INTEGER128_WORD_BITS / 2;
    const unsigned long long mask = ULLONG_MAX >> half;

    const unsigned long long a_lo = a & mask;
    const unsigned long long a_hi = a >> half;
    const unsigned long long b_lo = b & mask;
    const unsigned long long b_hi = b >> half;

    const unsigned long long p0 = a_lo * b_lo;
    const unsigned long long p1 = a_lo * b_hi;
    const unsigned long long p2 = a_hi * b_lo;
    const unsigned long long p3 = a_hi * b_hi;

    /* the middle column, at most 3 * mask, cannot overflow */
    const unsigned long long middle = (p0 >> half) + (p1 & mask) + (p2 & mask);

    *lo = (p0 & mask) | (middle << half);
    *hi = p3 + (p1 >> half) + (p2 >> half) + (middle >> half);
}

/* adds value to the word at index, carrying into the words above; what passes the top is lost */
static void integer128_add_word_at(struct integer128* r, int index, unsigned long long value)
{
    for (int i = index; i < INTEGER128_WORDS && value != 0; i++)
    {
        const unsigned long long s = r->w[i] + value;
        value = s < value ? 1 : 0;
        r->w[i] = s;
    }
}

struct integer128 integer128_mul(struct integer128 a, struct integer128 b)
{
    /* modulo 2^INTEGER128_BITS: the products that land above the top word are dropped */
    struct integer128 r = integer128_zero;
    for (int i = 0; i < INTEGER128_WORDS; i++)
    {
        for (int j = 0; i + j < INTEGER128_WORDS; j++)
        {
            unsigned long long lo = 0;
            unsigned long long hi = 0;
            integer128_mul_word(a.w[i], b.w[j], &lo, &hi);
            integer128_add_word_at(&r, i + j, lo);
            integer128_add_word_at(&r, i + j + 1, hi);
        }
    }
    return integer128_normalize(r);
}

struct integer128 integer128_not(struct integer128 a)
{
    struct integer128 r = integer128_zero;
    for (int i = 0; i < INTEGER128_WORDS; i++)
        r.w[i] = ~a.w[i];
    return integer128_normalize(r);
}

struct integer128 integer128_and(struct integer128 a, struct integer128 b)
{
    struct integer128 r = integer128_zero;
    for (int i = 0; i < INTEGER128_WORDS; i++)
        r.w[i] = a.w[i] & b.w[i];
    return r;
}

struct integer128 integer128_or(struct integer128 a, struct integer128 b)
{
    struct integer128 r = integer128_zero;
    for (int i = 0; i < INTEGER128_WORDS; i++)
        r.w[i] = a.w[i] | b.w[i];
    return r;
}

struct integer128 integer128_xor(struct integer128 a, struct integer128 b)
{
    struct integer128 r = integer128_zero;
    for (int i = 0; i < INTEGER128_WORDS; i++)
        r.w[i] = a.w[i] ^ b.w[i];
    return r;
}

struct integer128 integer128_shl(struct integer128 a, int count)
{
    assert(count >= 0 && count < INTEGER128_BITS);

    const int words = count / INTEGER128_WORD_BITS;
    const int bits = count % INTEGER128_WORD_BITS;

    struct integer128 r = integer128_zero;
    for (int i = INTEGER128_WORDS - 1; i >= 0; i--)
    {
        const int from = i - words;
        unsigned long long v = 0;
        if (from >= 0)
        {
            v = a.w[from] << bits;
            if (bits != 0 && from - 1 >= 0)
                v |= a.w[from - 1] >> (INTEGER128_WORD_BITS - bits);
        }
        r.w[i] = v;
    }
    return integer128_normalize(r);
}

struct integer128 integer128_shr_u(struct integer128 a, int count)
{
    assert(count >= 0 && count < INTEGER128_BITS);

    const int words = count / INTEGER128_WORD_BITS;
    const int bits = count % INTEGER128_WORD_BITS;

    struct integer128 r = integer128_zero;
    for (int i = 0; i < INTEGER128_WORDS; i++)
    {
        const int from = i + words;
        unsigned long long v = 0;
        if (from < INTEGER128_WORDS)
        {
            v = a.w[from] >> bits;
            if (bits != 0 && from + 1 < INTEGER128_WORDS)
                v |= a.w[from + 1] << (INTEGER128_WORD_BITS - bits);
        }
        r.w[i] = v;
    }
    return r;
}

struct integer128 integer128_shr_s(struct integer128 a, int count)
{
    assert(count >= 0 && count < INTEGER128_BITS);

    if (!integer128_is_negative(a))
    {
        return integer128_shr_u(a, count);
    }

    /* ~(~a >> n) shifts in ones, without relying on the host >> of a negative */
    return integer128_not(integer128_shr_u(integer128_not(a), count));
}

bool integer128_divmod_u(struct integer128 a, struct integer128 b, _Out struct integer128* quotient, _Out struct integer128* remainder)
{
    *quotient = integer128_zero;
    *remainder = integer128_zero;

    if (integer128_is_zero(b))
    {
        return false;
    }

    /* restoring division, one bit of the dividend at a time */
    struct integer128 q = integer128_zero;
    struct integer128 r = integer128_zero;
    for (int i = INTEGER128_BITS - 1; i >= 0; i--)
    {
        r = integer128_shl(r, 1);
        if (integer128_get_bit(a, i))
            r.w[0] |= 1;
        if (integer128_compare_u(r, b) >= 0)
        {
            r = integer128_sub(r, b);
            integer128_set_bit(&q, i);
        }
    }

    *quotient = q;
    *remainder = r;
    return true;
}

bool integer128_divmod_s(struct integer128 a, struct integer128 b, _Out struct integer128* quotient, _Out struct integer128* remainder)
{
    const bool a_negative = integer128_is_negative(a);
    const bool b_negative = integer128_is_negative(b);

    /* MIN negates to itself, and as unsigned it is still 2^(INTEGER128_BITS - 1), so this works for it too */
    if (!integer128_divmod_u(a_negative ? integer128_neg(a) : a,
                        b_negative ? integer128_neg(b) : b,
                        quotient,
                        remainder))
    {
        return false;
    }

    if (a_negative != b_negative)
        *quotient = integer128_neg(*quotient);

    if (a_negative)
        *remainder = integer128_neg(*remainder);

    return true;
}

struct integer128 integer128_wrap_u(struct integer128 a, int width)
{
    assert(width >= 1 && width <= INTEGER128_BITS);

    struct integer128 r = a;
    for (int i = 0; i < INTEGER128_WORDS; i++)
    {
        const int first = i * INTEGER128_WORD_BITS;
        if (first >= width)
            r.w[i] = 0;
        else if (width - first < INTEGER128_WORD_BITS)
            r.w[i] &= (1ULL << (width - first)) - 1;
    }
    return r;
}

struct integer128 integer128_wrap_s(struct integer128 a, int width)
{
    assert(width >= 1 && width <= INTEGER128_BITS);

    if (width == INTEGER128_BITS)
        return a;

    struct integer128 r = integer128_wrap_u(a, width);

    /* bit width - 1 is the sign, copy it into the bits above */
    if (integer128_get_bit(r, width - 1))
    {
        r = integer128_or(r, integer128_not(integer128_wrap_u(integer128_not(integer128_zero), width)));
    }
    return r;
}

/* the digits of a; buffer has room for them */
static int integer128_write_digits(struct integer128 a, char* buffer)
{
    /* digits come out from the last one */
    char digits[INTEGER128_TO_STRING_MIN_SIZE] = { 0 };
    int count = 0;
    const struct integer128 ten = integer128_from_unsigned(10);
    do
    {
        struct integer128 q = { 0 };
        struct integer128 r = { 0 };
        integer128_divmod_u(a, ten, &q, &r);
        digits[count++] = (char)('0' + r.w[0]);
        a = q;
    }
    while (!integer128_is_zero(a));

    for (int i = 0; i < count; i++)
        buffer[i] = digits[count - 1 - i];
    buffer[count] = '\0';
    return count;
}

int integer128_to_string_u(struct integer128 a, char* buffer, int size)
{
    if (size < INTEGER128_TO_STRING_MIN_SIZE)
    {
        if (size > 0)
            buffer[0] = '\0';
        return 0;
    }
    return integer128_write_digits(a, buffer);
}

int integer128_to_string_s(struct integer128 a, char* buffer, int size)
{
    if (size < INTEGER128_TO_STRING_MIN_SIZE)
    {
        if (size > 0)
            buffer[0] = '\0';
        return 0;
    }

    if (integer128_is_negative(a))
    {
        /* MIN negates to itself, and as unsigned it prints right */
        buffer[0] = '-';
        return 1 + integer128_write_digits(integer128_neg(a), buffer + 1);
    }
    return integer128_write_digits(a, buffer);
}

/* the hex digits of the 64 bits of a starting at bit 'first', without leading zeros */
static void integer128_hex64(struct integer128 a, int first, char* buffer)
{
    int n = 0;
    for (int k = 15; k >= 0; k--)
    {
        const int digit = (int)(integer128_to_ull(integer128_shr_u(a, first + 4 * k)) & 0xF);
        if (digit != 0 || n > 0 || k == 0)
            buffer[n++] = "0123456789abcdef"[digit];
    }
    buffer[n] = '\0';
}

int integer128_to_c_text(struct integer128 a, char* buffer, int size)
{
    if (size < INTEGER128_TO_C_TEXT_MIN_SIZE)
    {
        if (size > 0)
            buffer[0] = '\0';
        return 0;
    }

    /* the targets that have __int128 have a 64-bit unsigned long long, whatever the host has */
    char hi[17] = { 0 };
    char lo[17] = { 0 };
    integer128_hex64(a, 64, hi);
    integer128_hex64(a, 0, lo);
    return snprintf(buffer, (size_t)size, "((unsigned __int128)0x%sULL << 64 | 0x%sULL)", hi, lo);
}

static int integer128_digit_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

struct integer128 integer128_from_digits(const char* text, int base, _Out bool* overflow)
{
    assert(base == 2 || base == 8 || base == 10 || base == 16);

    *overflow = false;

    const struct integer128 wide_base = integer128_from_unsigned((unsigned long long)base);
    struct integer128 r = integer128_zero;

    for (const char* p = text; *p; p++)
    {
        const int digit = integer128_digit_value(*p);
        if (digit < 0 || digit >= base)
            break;

        /* r * base + digit did not fit when going back does not give r */
        const struct integer128 next =
            integer128_add(integer128_mul(r, wide_base), integer128_from_unsigned((unsigned long long)digit));

        struct integer128 quotient = { 0 };
        struct integer128 remainder = { 0 };
        integer128_divmod_u(next, wide_base, &quotient, &remainder);
        if (!integer128_equal(quotient, r) || remainder.w[0] != (unsigned long long)digit)
            *overflow = true;

        r = next;
    }
    return r;
}

#ifdef TEST
#include <stdio.h>
#include "unit_test.h"

/* the tests are written for 64-bit halves */
#define INTEGER128_HALF_BITS 64
#define INTEGER128_HI_WEIGHT ((long double)ULLONG_MAX + 1.0L)

static struct integer128 integer128_make(unsigned long long hi, unsigned long long lo)
{
    return integer128_or(integer128_shl(integer128_from_unsigned(hi), INTEGER128_HALF_BITS), integer128_from_unsigned(lo));
}

/* the most negative value, only the sign bit */
static struct integer128 integer128_min(void)
{
    return integer128_make(1ULL << (INTEGER128_HALF_BITS - 1), 0);
}

void integer128_arithmetic_test(void)
{
    const struct integer128 one = integer128_from_unsigned(1);
    const struct integer128 minus_one = integer128_from_signed(-1);
    const struct integer128 max_half = integer128_from_unsigned(ULLONG_MAX);

    assert(integer128_equal(minus_one, integer128_make(ULLONG_MAX, ULLONG_MAX)));
    assert(integer128_equal(integer128_add(max_half, one), integer128_make(1, 0)));
    assert(integer128_equal(integer128_sub(integer128_make(1, 0), one), max_half));
    assert(integer128_equal(integer128_add(minus_one, one), integer128_from_unsigned(0)));
    assert(integer128_equal(integer128_neg(one), minus_one));

    /* (2^h - 1)^2 = (2^h - 2) * 2^h + 1 */
    assert(integer128_equal(integer128_mul(max_half, max_half), integer128_make(ULLONG_MAX - 1, 1)));
    assert(integer128_equal(integer128_mul(integer128_from_signed(-3), integer128_from_signed(7)), integer128_from_signed(-21)));
    assert(integer128_equal(integer128_mul(integer128_make(1, 0), integer128_make(1, 0)), integer128_from_unsigned(0))); /* 2^INTEGER128_BITS wraps */
}

void integer128_shift_test(void)
{
    const struct integer128 one = integer128_from_unsigned(1);
    const int n = INTEGER128_HALF_BITS + INTEGER128_HALF_BITS / 2;

    assert(integer128_equal(integer128_shl(one, n), integer128_make(1ULL << (n - INTEGER128_HALF_BITS), 0)));
    assert(integer128_equal(integer128_shl(one, INTEGER128_HALF_BITS), integer128_make(1, 0)));
    assert(integer128_equal(integer128_shl(one, 0), one));
    assert(integer128_equal(integer128_shr_u(integer128_make(1ULL << (n - INTEGER128_HALF_BITS), 0), n), one));
    assert(integer128_equal(integer128_shr_u(integer128_from_signed(-1), INTEGER128_BITS - 1), one));
    assert(integer128_equal(integer128_shr_s(integer128_from_signed(-1), INTEGER128_BITS - 1), integer128_from_signed(-1)));
    assert(integer128_equal(integer128_shr_s(integer128_from_signed(-8), 1), integer128_from_signed(-4)));
    assert(integer128_equal(integer128_shr_s(integer128_from_signed(-7), 1), integer128_from_signed(-4)));
}

void integer128_compare_test(void)
{
    assert(integer128_compare_s(integer128_from_signed(-1), integer128_from_signed(0)) < 0);
    assert(integer128_compare_u(integer128_from_signed(-1), integer128_from_signed(0)) > 0);
    assert(integer128_compare_s(integer128_min(), integer128_from_signed(-1)) < 0);
    assert(integer128_compare_u(integer128_make(1, 0), integer128_make(0, ULLONG_MAX)) > 0);
    assert(integer128_compare_s(integer128_from_signed(5), integer128_from_signed(5)) == 0);
}

void integer128_divmod_test(void)
{
    struct integer128 q = { 0 };
    struct integer128 r = { 0 };

    /* 2^n = 3 * q + r */
    const struct integer128 n = integer128_shl(integer128_from_unsigned(1), INTEGER128_BITS - 28);
    assert(integer128_divmod_u(n, integer128_from_unsigned(3), &q, &r));
    assert(integer128_compare_u(r, integer128_from_unsigned(3)) < 0);
    assert(integer128_equal(integer128_add(integer128_mul(q, integer128_from_unsigned(3)), r), n));

    assert(integer128_divmod_s(integer128_from_signed(-7), integer128_from_signed(2), &q, &r));
    assert(integer128_equal(q, integer128_from_signed(-3)));
    assert(integer128_equal(r, integer128_from_signed(-1)));

    assert(integer128_divmod_s(integer128_from_signed(7), integer128_from_signed(-2), &q, &r));
    assert(integer128_equal(q, integer128_from_signed(-3)));
    assert(integer128_equal(r, integer128_from_signed(1)));

    /* MIN / -1 wraps to MIN */
    assert(integer128_divmod_s(integer128_min(), integer128_from_signed(-1), &q, &r));
    assert(integer128_equal(q, integer128_min()));
    assert(integer128_is_zero(r));

    assert(!integer128_divmod_u(n, integer128_from_unsigned(0), &q, &r));
    assert(integer128_is_zero(q) && integer128_is_zero(r));
}

void integer128_wrap_test(void)
{
    const int w = INTEGER128_HALF_BITS + 36;

    assert(integer128_equal(integer128_wrap_s(integer128_from_unsigned(0xFF), 8), integer128_from_signed(-1)));
    assert(integer128_equal(integer128_wrap_u(integer128_from_signed(-1), 8), integer128_from_unsigned(0xFF)));
    assert(integer128_equal(integer128_wrap_u(integer128_from_signed(-1), w), integer128_make((1ULL << 36) - 1, ULLONG_MAX)));
    assert(integer128_equal(integer128_wrap_s(integer128_make(1ULL << 35, 0), w), integer128_make(ULLONG_MAX << 35, 0)));
    assert(integer128_equal(integer128_wrap_u(integer128_from_signed(-1), INTEGER128_HALF_BITS), integer128_from_unsigned(ULLONG_MAX)));
    assert(integer128_equal(integer128_wrap_s(integer128_from_unsigned(ULLONG_MAX), INTEGER128_HALF_BITS), integer128_from_signed(-1)));
    assert(integer128_equal(integer128_wrap_s(integer128_from_signed(-1), INTEGER128_BITS), integer128_from_signed(-1)));
}

void integer128_conversion_test(void)
{
    assert(integer128_equal(integer128_from_long_double(-3.9L), integer128_from_signed(-3)));
    assert(integer128_equal(integer128_from_long_double(INTEGER128_HI_WEIGHT), integer128_make(1, 0)));
    assert(integer128_to_long_double_u(integer128_make(1, 0)) == INTEGER128_HI_WEIGHT);
    assert(integer128_to_long_double_s(integer128_from_signed(-5)) == -5.0L);

    char buffer[INTEGER128_TO_STRING_MIN_SIZE] = { 0 };

    integer128_to_string_s(integer128_from_unsigned(0), buffer, sizeof buffer);
    assert(strcmp(buffer, "0") == 0);

    integer128_to_string_s(integer128_from_signed(-42), buffer, sizeof buffer);
    assert(strcmp(buffer, "-42") == 0);

    integer128_to_string_u(integer128_from_unsigned(1234567890), buffer, sizeof buffer);
    assert(strcmp(buffer, "1234567890") == 0);

#if ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
    integer128_to_string_u(integer128_from_signed(-1), buffer, sizeof buffer);
    assert(strcmp(buffer, "340282366920938463463374607431768211455") == 0);

    integer128_to_string_s(integer128_min(), buffer, sizeof buffer);
    assert(strcmp(buffer, "-170141183460469231731687303715884105728") == 0);
#endif

    char small[8] = { 0 };
    assert(integer128_to_string_u(integer128_from_unsigned(1), small, sizeof small) == 0);
    assert(small[0] == '\0');
}

void integer128_to_c_text_test(void)
{
    char buffer[INTEGER128_TO_C_TEXT_MIN_SIZE] = { 0 };
    integer128_to_c_text(integer128_shl(integer128_from_unsigned(1), 100), buffer, sizeof buffer);
    assert(strcmp(buffer, "((unsigned __int128)0x1000000000ULL << 64 | 0x0ULL)") == 0);
    integer128_to_c_text(integer128_from_signed(-1), buffer, sizeof buffer);
    assert(strcmp(buffer, "((unsigned __int128)0xffffffffffffffffULL << 64 | 0xffffffffffffffffULL)") == 0);
}

void integer128_from_digits_test(void)
{
    bool overflow = true;

    assert(integer128_equal(integer128_from_digits("12345", 10, &overflow), integer128_from_unsigned(12345)));
    assert(!overflow);

    assert(integer128_equal(integer128_from_digits("ff", 16, &overflow), integer128_from_unsigned(255)));
    assert(integer128_equal(integer128_from_digits("777", 8, &overflow), integer128_from_unsigned(511)));
    assert(integer128_equal(integer128_from_digits("101", 2, &overflow), integer128_from_unsigned(5)));

    /* stops at the first character that is not a digit of the base */
    assert(integer128_equal(integer128_from_digits("12uwb", 10, &overflow), integer128_from_unsigned(12)));
    assert(integer128_equal(integer128_from_digits("129", 8, &overflow), integer128_from_unsigned(10)));

    /* 2^HALF_BITS, one past an unsigned long long */
    char text[INTEGER128_BITS + 2] = { 0 };
    text[0] = '1';
    for (int i = 1; i <= INTEGER128_HALF_BITS; i++)
        text[i] = '0';
    assert(integer128_equal(integer128_from_digits(text, 2, &overflow), integer128_make(1, 0)));
    assert(!overflow);

    /* INTEGER128_BITS ones fit, one more does not */
    for (int i = 0; i < INTEGER128_BITS; i++)
        text[i] = '1';
    text[INTEGER128_BITS] = '\0';
    assert(integer128_equal(integer128_from_digits(text, 2, &overflow), integer128_from_signed(-1)));
    assert(!overflow);

    text[INTEGER128_BITS] = '1';
    text[INTEGER128_BITS + 1] = '\0';
    integer128_from_digits(text, 2, &overflow);
    assert(overflow);

#if ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
    assert(integer128_equal(integer128_from_digits("340282366920938463463374607431768211455", 10, &overflow), integer128_from_signed(-1)));
    assert(!overflow);
    integer128_from_digits("340282366920938463463374607431768211456", 10, &overflow);
    assert(overflow);
#endif
}

#endif
