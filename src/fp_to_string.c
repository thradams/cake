/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "ownership.h"
#include "fp_to_string.h"
#include <stdint.h>
#include <string.h>

/*
 *  The standard-library alternative, in case we ever want it back. Build
 *  with -DSTANDARD_ALTERNATIVE to use it instead of the Grisu2 code below;
 *  the two are interchangeable from the caller's point of view.
 *
 *  It is what cake used before. It is not lossy in the value sense - 17
 *  significant digits always round-trip an IEEE double - but it always spends
 *  all 17 of them, so 0.1 comes out as 0.10000000000000001 even though "0.1"
 *  reads back as the very same double.
 *
 *  Two things it still has to get right, and both are easy to forget:
 *
 *    - %g drops the decimal point for whole numbers, and "2" is an int
 *      constant rather than a double one, so it has to be put back.
 *    - the result must stay inside size.
 *
 *  It also inherits whatever the host libc does, whereas the Grisu2 code
 *  below works from the value's own bits and needs no float formatting at
 *  all.
 */
#ifdef STANDARD_ALTERNATIVE

#include <stdio.h>

int double_to_string(double value, char* buffer, int size)
{
    char* p;
    int has_dot_or_exp = 0;

    if (size < FP_TO_STRING_MIN_SIZE)
    {
        if (size > 0)
            buffer[0] = '\0';
        return 0;
    }

    snprintf(buffer, (size_t)size, "%.17g", value);

    p = buffer;
    while (*p)
    {
        if (*p == 'e' || *p == 'E' || *p == '.')
        {
            has_dot_or_exp = 1;
            break;
        }
        p++;
    }

    if (!has_dot_or_exp)
    {
        /* p is at the NUL; there is room, size is at least 32 */
        *p++ = '.';
        *p++ = '0';
        *p = '\0';
    }

    return (int)strlen(buffer);
}

int float_to_string(float value, char* buffer, int size)
{
    char* p;
    int has_dot_or_exp = 0;

    if (size < FP_TO_STRING_MIN_SIZE)
    {
        if (size > 0)
            buffer[0] = '\0';
        return 0;
    }

    /* 9 significant digits round-trip an IEEE float, the way 17 do a double */
    snprintf(buffer, (size_t)size, "%.9g", (double)value);

    p = buffer;
    while (*p)
    {
        if (*p == 'e' || *p == 'E' || *p == '.')
        {
            has_dot_or_exp = 1;
            break;
        }
        p++;
    }

    if (!has_dot_or_exp)
    {
        /* p is at the NUL; there is room, size is at least 32 */
        *p++ = '.';
        *p++ = '0';
        *p = '\0';
    }

    return (int)strlen(buffer);
}

#else

/*
 *  double_to_string / float_to_string - shortest decimal text that reads
 *  back as the exact same value.
 *
 *  Algorithm: Grisu2, from
 *
 *      Florian Loitsch, "Printing Floating-Point Numbers Quickly and Accurately
 *      with Integers", PLDI 2010.
 *      https://doi.org/10.1145/1806596.1806623
 *      paper: https://www.cs.tufts.edu/~nr/cs257/archive/florian-loitsch/printf.pdf
 *
 *  This is the same variant used by RapidJSON, which is a good cross-check when
 *  reading the code:
 *      https://github.com/Tencent/rapidjson/blob/master/include/rapidjson/internal/dtoa.h
 *
 *  The author's own reference implementation (Grisu2 and Grisu3) lives in
 *      https://github.com/google/double-conversion
 *
 *  GUARANTEE: the string produced, read back by a correctly-rounded strtod,
 *  yields the EXACT same double. Nothing is lost.
 *
 *  NOT guaranteed: minimality. Grisu2 finds the shortest form for ~99.9% of
 *  doubles and emits one extra digit for the rest. It is never wrong, only
 *  occasionally one character longer. Provable minimality needs Grisu3 or Ryu,
 *  which both require a bignum fallback path.
 *
 *  No heap, no locale, no libc float formatting.
 */

/*---------------------------------------------------------------------------
 *  diy_fp: a 64-bit significand with a binary exponent.  value = f * 2^e
 *-------------------------------------------------------------------------*/

struct fp_value
{
    uint64_t f;
    int e;
};

#define DP_SIGNIFICAND_SIZE  52
#define DP_EXPONENT_BIAS     (0x3FF + DP_SIGNIFICAND_SIZE)
#define DP_DENORMAL_EXPONENT (-DP_EXPONENT_BIAS + 1)
#define DP_EXPONENT_MASK     0x7FF0000000000000ULL
#define DP_SIGNIFICAND_MASK  0x000FFFFFFFFFFFFFULL
#define DP_HIDDEN_BIT        0x0010000000000000ULL

static struct fp_value fp_from_double(double d)
{
    struct fp_value r;
    uint64_t u;
    int biased_e;
    uint64_t significand;

    memcpy(&u, &d, sizeof u);

    biased_e = (int)((u & DP_EXPONENT_MASK) >> DP_SIGNIFICAND_SIZE);
    significand = u & DP_SIGNIFICAND_MASK;

    if (biased_e != 0)
    {
        r.f = significand + DP_HIDDEN_BIT;
        r.e = biased_e - DP_EXPONENT_BIAS;
    }
    else
    {
        r.f = significand;
        r.e = DP_DENORMAL_EXPONENT;
    }
    return r;
}

#define SP_SIGNIFICAND_SIZE  23
#define SP_EXPONENT_BIAS     (0x7F + SP_SIGNIFICAND_SIZE)
#define SP_DENORMAL_EXPONENT (-SP_EXPONENT_BIAS + 1)
#define SP_EXPONENT_MASK     0x7F800000u
#define SP_SIGNIFICAND_MASK  0x007FFFFFu
#define SP_HIDDEN_BIT        0x00800000u

static struct fp_value fp_from_float(float d)
{
    struct fp_value r;
    uint32_t u;
    int biased_e;
    uint32_t significand;

    memcpy(&u, &d, sizeof u);

    biased_e = (int)((u & SP_EXPONENT_MASK) >> SP_SIGNIFICAND_SIZE);
    significand = u & SP_SIGNIFICAND_MASK;

    if (biased_e != 0)
    {
        r.f = significand + SP_HIDDEN_BIT;
        r.e = biased_e - SP_EXPONENT_BIAS;
    }
    else
    {
        r.f = significand;
        r.e = SP_DENORMAL_EXPONENT;
    }
    return r;
}

/* x - y. Requires equal exponents and x >= y. */
static struct fp_value fp_minus(struct fp_value x, struct fp_value y)
{
    struct fp_value r;
    r.f = x.f - y.f;
    r.e = x.e;
    return r;
}

/* x * y, keeping the high 64 bits, rounded. */
static struct fp_value fp_multiply(struct fp_value x, struct fp_value y)
{
    struct fp_value r;
    const uint64_t M32 = 0xFFFFFFFFULL;
    const uint64_t a = x.f >> 32;
    const uint64_t b = x.f & M32;
    const uint64_t c = y.f >> 32;
    const uint64_t d = y.f & M32;
    const uint64_t ac = a * c;
    const uint64_t bc = b * c;
    const uint64_t ad = a * d;
    const uint64_t bd = b * d;
    uint64_t tmp = (bd >> 32) + (ad & M32) + (bc & M32);

    tmp += 1ULL << 31; /* round */

    r.f = ac + (ad >> 32) + (bc >> 32) + (tmp >> 32);
    r.e = x.e + y.e + 64;
    return r;
}

/* shift left until the top bit is set */
static struct fp_value fp_normalize(struct fp_value x)
{
    while (!(x.f & (1ULL << 63)))
    {
        x.f <<= 1;
        x.e--;
    }
    return x;
}

/*
   The midpoints between v and its two neighbours, brought to a common
   exponent. Any decimal strictly inside (m_minus, m_plus) reads back as v.
*/
static void fp_boundaries(struct fp_value v,
                              uint64_t hidden_bit,
                              _Out struct fp_value* m_minus,
                              _Out struct fp_value* m_plus)
{
    struct fp_value pl = { 0 };
    struct fp_value mi = { 0 };

    pl.f = (v.f << 1) + 1;
    pl.e = v.e - 1;
    pl = fp_normalize(pl);

    if (v.f == hidden_bit)
    {
        /* v is a power of two: the gap below it is half the gap above */
        mi.f = (v.f << 2) - 1;
        mi.e = v.e - 2;
    }
    else
    {
        mi.f = (v.f << 1) - 1;
        mi.e = v.e - 1;
    }

    mi.f <<= mi.e - pl.e;
    mi.e = pl.e;

    /*
      Member-wise rather than `*m_plus = pl;`: under #pragma safety enable the
      flow analysis does not currently count a whole-struct assignment as
      initializing an _Out pointee, and reports warning 71.
    */
    m_plus->f = pl.f;
    m_plus->e = pl.e;
    m_minus->f = mi.f;
    m_minus->e = mi.e;
}

/*---------------------------------------------------------------------------
 *  Cached powers of ten: 10^k for k = -348, -340, ... 340, as normalized
 *  diy_fp. Generated with exact integer arithmetic.
 *-------------------------------------------------------------------------*/

static const uint64_t g_fp_cached_power_f[87] = {
    0xFA8FD5A0081C0288ULL, 0xBAAEE17FA23EBF76ULL, 0x8B16FB203055AC76ULL, 0xCF42894A5DCE35EAULL,
    0x9A6BB0AA55653B2DULL, 0xE61ACF033D1A45DFULL, 0xAB70FE17C79AC6CAULL, 0xFF77B1FCBEBCDC4FULL,
    0xBE5691EF416BD60CULL, 0x8DD01FAD907FFC3CULL, 0xD3515C2831559A83ULL, 0x9D71AC8FADA6C9B5ULL,
    0xEA9C227723EE8BCBULL, 0xAECC49914078536DULL, 0x823C12795DB6CE57ULL, 0xC21094364DFB5637ULL,
    0x9096EA6F3848984FULL, 0xD77485CB25823AC7ULL, 0xA086CFCD97BF97F4ULL, 0xEF340A98172AACE5ULL,
    0xB23867FB2A35B28EULL, 0x84C8D4DFD2C63F3BULL, 0xC5DD44271AD3CDBAULL, 0x936B9FCEBB25C996ULL,
    0xDBAC6C247D62A584ULL, 0xA3AB66580D5FDAF6ULL, 0xF3E2F893DEC3F126ULL, 0xB5B5ADA8AAFF80B8ULL,
    0x87625F056C7C4A8BULL, 0xC9BCFF6034C13053ULL, 0x964E858C91BA2655ULL, 0xDFF9772470297EBDULL,
    0xA6DFBD9FB8E5B88FULL, 0xF8A95FCF88747D94ULL, 0xB94470938FA89BCFULL, 0x8A08F0F8BF0F156BULL,
    0xCDB02555653131B6ULL, 0x993FE2C6D07B7FACULL, 0xE45C10C42A2B3B06ULL, 0xAA242499697392D3ULL,
    0xFD87B5F28300CA0EULL, 0xBCE5086492111AEBULL, 0x8CBCCC096F5088CCULL, 0xD1B71758E219652CULL,
    0x9C40000000000000ULL, 0xE8D4A51000000000ULL, 0xAD78EBC5AC620000ULL, 0x813F3978F8940984ULL,
    0xC097CE7BC90715B3ULL, 0x8F7E32CE7BEA5C70ULL, 0xD5D238A4ABE98068ULL, 0x9F4F2726179A2245ULL,
    0xED63A231D4C4FB27ULL, 0xB0DE65388CC8ADA8ULL, 0x83C7088E1AAB65DBULL, 0xC45D1DF942711D9AULL,
    0x924D692CA61BE758ULL, 0xDA01EE641A708DEAULL, 0xA26DA3999AEF774AULL, 0xF209787BB47D6B85ULL,
    0xB454E4A179DD1877ULL, 0x865B86925B9BC5C2ULL, 0xC83553C5C8965D3DULL, 0x952AB45CFA97A0B3ULL,
    0xDE469FBD99A05FE3ULL, 0xA59BC234DB398C25ULL, 0xF6C69A72A3989F5CULL, 0xB7DCBF5354E9BECEULL,
    0x88FCF317F22241E2ULL, 0xCC20CE9BD35C78A5ULL, 0x98165AF37B2153DFULL, 0xE2A0B5DC971F303AULL,
    0xA8D9D1535CE3B396ULL, 0xFB9B7CD9A4A7443CULL, 0xBB764C4CA7A44410ULL, 0x8BAB8EEFB6409C1AULL,
    0xD01FEF10A657842CULL, 0x9B10A4E5E9913129ULL, 0xE7109BFBA19C0C9DULL, 0xAC2820D9623BF429ULL,
    0x80444B5E7AA7CF85ULL, 0xBF21E44003ACDD2DULL, 0x8E679C2F5E44FF8FULL, 0xD433179D9C8CB841ULL,
    0x9E19DB92B4E31BA9ULL, 0xEB96BF6EBADF77D9ULL, 0xAF87023B9BF0EE6BULL
};

static const int16_t g_fp_cached_power_e[87] = {
    -1220, -1193, -1166, -1140, -1113, -1087, -1060, -1034, -1007, -980, -954, -927, -901, -874,
    -847, -821, -794, -768, -741, -715, -688, -661, -635, -608, -582, -555, -529, -502, -475,
    -449, -422, -396, -369, -343, -316, -289, -263, -236, -210, -183, -157, -130, -103, -77,
    -50, -24, 3, 30, 56, 83, 109, 136, 162, 189, 216, 242, 269, 295, 322, 348, 375, 402, 428,
    455, 481, 508, 534, 561, 588, 614, 641, 667, 694, 720, 747, 774, 800, 827, 853, 880, 907,
    933, 960, 986, 1013, 1039, 1066
};

/* pick the 10^-k that lands the scaled value in a workable binary range */
static struct fp_value fp_get_cached_power(int e, _Out int* K)
{
    struct fp_value r;
    /* 1/lg(10) = 0.30102999566398114 */
    const double dk = (-61 - e) * 0.30102999566398114 + 347;
    int k = (int)dk;
    unsigned index;

    if (dk - k > 0.0)
        k++;   /* ceil */

    index = (unsigned)((k >> 3) + 1);
    *K = -(-348 + (int)(index << 3));

    r.f = g_fp_cached_power_f[index];
    r.e = g_fp_cached_power_e[index];
    return r;
}

/*---------------------------------------------------------------------------
 *  Digit generation
 *-------------------------------------------------------------------------*/

static const uint32_t g_fp_pow10[10] = {
    1, 10, 100, 1000, 10000, 100000,
    1000000, 10000000, 100000000, 1000000000
};

static int fp_count_decimal_digit32(uint32_t n)
{
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    return 10;
}

/*
   Walk the last digit back down while the result stays inside the
   round-trip interval and moves closer to the true value.
*/
static void fp_round_last_digit(char* buffer, int len, uint64_t delta,
                        uint64_t rest, uint64_t ten_kappa, uint64_t wp_w)
{
    while (rest < wp_w &&
           delta - rest >= ten_kappa &&
           (rest + ten_kappa < wp_w ||
            wp_w - rest > rest + ten_kappa - wp_w))
    {
        buffer[len - 1]--;
        rest += ten_kappa;
    }
}

/* K is in/out here: fp_get_cached_power set it, this adds the digit count */
static void fp_digit_gen(struct fp_value W, struct fp_value Mp, uint64_t delta,
                      char* buffer, _Out int* len, int* K)
{
    struct fp_value one;
    struct fp_value wp_w;
    uint32_t p1;
    uint64_t p2;
    int kappa;

    one.f = 1ULL << -Mp.e;
    one.e = Mp.e;

    wp_w = fp_minus(Mp, W);

    p1 = (uint32_t)(Mp.f >> -one.e); /* integer part */
    p2 = Mp.f & (one.f - 1);         /* fractional part */

    kappa = fp_count_decimal_digit32(p1);
    *len = 0;

    while (kappa > 0)
    {
        /*
          kappa is 1..10 here, so this is g_fp_pow10[0..9] and never zero. The
          local makes that visible to the flow analysis, which cannot know the
          contents of the table.
        */
        const uint32_t ten_k = (kappa >= 1 && kappa <= 10) ? g_fp_pow10[kappa - 1] : 1;
        uint32_t d;
        uint64_t tmp;

        if (ten_k == 0)
            break; /* unreachable */

        d = p1 / ten_k;
        p1 %= ten_k;

        if (d != 0 || *len != 0)
            buffer[(*len)++] = (char)('0' + d);

        kappa--;

        tmp = ((uint64_t)p1 << -one.e) + p2;
        if (tmp <= delta)
        {
            *K += kappa;
            fp_round_last_digit(buffer, *len, delta, tmp,
                        (uint64_t)g_fp_pow10[kappa] << -one.e, wp_w.f);
            return;
        }
    }

    for (;;)
    {
        char d;

        p2 *= 10;
        delta *= 10;

        d = (char)(p2 >> -one.e);
        if (d != 0 || *len != 0)
            buffer[(*len)++] = (char)('0' + d);

        p2 &= one.f - 1;
        kappa--;

        if (p2 < delta)
        {
            const int index = -kappa;
            *K += kappa;
            fp_round_last_digit(buffer, *len, delta, p2, one.f,
                        wp_w.f * (index < 9 ? g_fp_pow10[index] : 0));
            return;
        }
    }
}

/* on return: value == 0.<buffer> * 10^(K + length) */
static void fp_shortest_digits(double value, char* buffer, _Out int* length, _Out int* K)
{
    struct fp_value v = fp_from_double(value);
    struct fp_value w_m;
    struct fp_value w_p;
    struct fp_value c_mk;
    struct fp_value W;
    struct fp_value Wp;
    struct fp_value Wm;

    fp_boundaries(v, DP_HIDDEN_BIT, &w_m, &w_p);

    c_mk = fp_get_cached_power(w_p.e, K);

    W = fp_multiply(fp_normalize(v), c_mk);
    Wp = fp_multiply(w_p, c_mk);
    Wm = fp_multiply(w_m, c_mk);

    Wm.f++; /* shrink the interval so both endpoints are excluded */
    Wp.f--;

    fp_digit_gen(W, Wp, Wp.f - Wm.f, buffer, length, K);
}

/* same, but the round-trip interval is the wider one of a float */
static void fp_shortest_digits_float(float value, char* buffer, _Out int* length, _Out int* K)
{
    struct fp_value v = fp_from_float(value);
    struct fp_value w_m;
    struct fp_value w_p;
    struct fp_value c_mk;
    struct fp_value W;
    struct fp_value Wp;
    struct fp_value Wm;

    fp_boundaries(v, SP_HIDDEN_BIT, &w_m, &w_p);

    c_mk = fp_get_cached_power(w_p.e, K);

    W = fp_multiply(fp_normalize(v), c_mk);
    Wp = fp_multiply(w_p, c_mk);
    Wm = fp_multiply(w_m, c_mk);

    Wm.f++;
    Wp.f--;

    fp_digit_gen(W, Wp, Wp.f - Wm.f, buffer, length, K);
}

/*---------------------------------------------------------------------------
 *  Formatting
 *-------------------------------------------------------------------------*/

static char* fp_write_exponent(int K, char* buffer)
{
    if (K < 0)
    {
        *buffer++ = '-';
        K = -K;
    }
    else
    {
        *buffer++ = '+';
    }

    if (K >= 100)
    {
        *buffer++ = (char)('0' + K / 100);
        K %= 100;
        *buffer++ = (char)('0' + K / 10);
        *buffer++ = (char)('0' + K % 10);
    }
    else if (K >= 10)
    {
        *buffer++ = (char)('0' + K / 10);
        *buffer++ = (char)('0' + K % 10);
    }
    else
    {
        *buffer++ = (char)('0' + K);
    }
    return buffer;
}

/*
   Lay the digits out as a valid C floating literal: a '.' or an 'e' is always
   present, so the result can never be mistaken for an integer constant.
*/
static char* fp_prettify(char* buffer, int length, int k)
{
    const int kk = length + k; /* the decimal point sits after kk digits */

    if (0 <= k && kk <= 21)
    {
        /* 1234e7 -> 12340000000.0 */
        int i;
        for (i = length; i < kk; i++)
            buffer[i] = '0';
        buffer[kk] = '.';
        buffer[kk + 1] = '0';
        return &buffer[kk + 2];
    }

    if (0 < kk && kk <= 21)
    {
        /* 1234e-2 -> 12.34 */
        memmove(&buffer[kk + 1], &buffer[kk], (size_t)(length - kk));
        buffer[kk] = '.';
        return &buffer[length + 1];
    }

    if (-6 < kk && kk <= 0)
    {
        /* 1234e-6 -> 0.001234 */
        const int offset = 2 - kk;
        int i;
        memmove(&buffer[offset], &buffer[0], (size_t)length);
        buffer[0] = '0';
        buffer[1] = '.';
        for (i = 2; i < offset; i++)
            buffer[i] = '0';
        return &buffer[length + offset];
    }

    if (length == 1)
    {
        /* 1e30 -> 1e+30 */
        buffer[1] = 'e';
        return fp_write_exponent(kk - 1, &buffer[2]);
    }

    /* 1234e30 -> 1.234e+33 */
    memmove(&buffer[2], &buffer[1], (size_t)(length - 1));
    buffer[1] = '.';
    buffer[length + 1] = 'e';
    return fp_write_exponent(kk - 1, &buffer[length + 2]);
}

/*---------------------------------------------------------------------------
 *  Public entry point.
 *
 *  See fp_to_string.h for the contract.
 *-------------------------------------------------------------------------*/


int double_to_string(double value, char* buffer, int size)
{
    char* p = buffer;
    int length;
    int K;

    if (size < FP_TO_STRING_MIN_SIZE)
    {
        if (size > 0)
            buffer[0] = '\0';
        return 0;
    }

    if (value == 0.0)
    {
        uint64_t u;
        memcpy(&u, &value, sizeof u);
        if (u >> 63) /* keep -0.0 distinguishable */
            *p++ = '-';
        *p++ = '0';
        *p++ = '.';
        *p++ = '0';
        *p = '\0';
        return (int)(p - buffer);
    }

    if (value < 0)
    {
        *p++ = '-';
        value = -value;
    }

    fp_shortest_digits(value, p, &length, &K);
    p = fp_prettify(p, length, K);
    *p = '\0';
    return (int)(p - buffer);
}

/*
 *  As above, for float. Emitting the float's own shortest form matters: the
 *  shortest form of the double it widens to is correct but far longer
 *  (0.1f would come out as 0.10000000149011612).
 */
int float_to_string(float value, char* buffer, int size)
{
    char* p = buffer;
    int length;
    int K;

    if (size < FP_TO_STRING_MIN_SIZE)
    {
        if (size > 0)
            buffer[0] = '\0';
        return 0;
    }

    if (value == 0.0f)
    {
        uint32_t u;
        memcpy(&u, &value, sizeof u);
        if (u >> 31)
            *p++ = '-';
        *p++ = '0';
        *p++ = '.';
        *p++ = '0';
        *p = '\0';
        return (int)(p - buffer);
    }

    if (value < 0)
    {
        *p++ = '-';
        value = -value;
    }

    fp_shortest_digits_float(value, p, &length, &K);
    p = fp_prettify(p, length, K);
    *p = '\0';
    return (int)(p - buffer);
}

#endif /* STANDARD_ALTERNATIVE */
