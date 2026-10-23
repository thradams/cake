/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once

#include "cake_compat.h"
#include <stdbool.h>
#include <limits.h>

/*
 *  integer128: a 128-bit integer, whatever the widths of the host are. It
 *  is an array of unsigned long long words, as many as needed to hold 128
 *  bits (2 when unsigned long long has 64 bits, 1 when it has 128 or more);
 *  the bits of the top word above bit 127 are always 0. cake keeps the
 *  constant value of every integer in one
 *  (struct object::value.host_integer128), up to __int128.
 *
 *  struct integer128 is only the bits, in two's complement. Like in the
 *  hardware, the same bits are a signed or an unsigned value: the
 *  operations where that matters have a _s and a _u version.
 *
 *  Shift counts are 0..INTEGER128_BITS - 1 and widths are 1..INTEGER128_BITS;
 *  anything else is a caller error (asserted).
 */

#define INTEGER128_BITS 128
#define INTEGER128_WORD_BITS ((int)(sizeof(unsigned long long) * CHAR_BIT))
#define INTEGER128_WORDS ((INTEGER128_BITS + INTEGER128_WORD_BITS - 1) / INTEGER128_WORD_BITS)

struct integer128
{
    unsigned long long w[INTEGER128_WORDS]; /* w[0] is the lowest */
};

struct integer128 integer128_from_signed(long long value);
struct integer128 integer128_from_unsigned(unsigned long long value);

/*
 *  The value of the digits of text in base (2, 8, 10 or 16), up to the
 *  first character that is not one. *overflow tells it did not fit in
 *  INTEGER128_BITS (the result is then the value modulo 2^INTEGER128_BITS).
 */
struct integer128 integer128_from_digits(const char* text, int base, _Out bool* overflow);

/* truncates toward zero; the value must fit in INTEGER128_BITS */
struct integer128 integer128_from_long_double(long double value);
long double integer128_to_long_double_s(struct integer128 a);
long double integer128_to_long_double_u(struct integer128 a);

bool integer128_is_zero(struct integer128 a);
bool integer128_is_negative(struct integer128 a);
bool integer128_equal(struct integer128 a, struct integer128 b);

/* -1, 0 or 1 */
int integer128_compare_s(struct integer128 a, struct integer128 b);
int integer128_compare_u(struct integer128 a, struct integer128 b);

/* these wrap around modulo 2^INTEGER128_BITS */
struct integer128 integer128_add(struct integer128 a, struct integer128 b);
struct integer128 integer128_sub(struct integer128 a, struct integer128 b);
struct integer128 integer128_neg(struct integer128 a);
struct integer128 integer128_mul(struct integer128 a, struct integer128 b);

struct integer128 integer128_not(struct integer128 a);
struct integer128 integer128_and(struct integer128 a, struct integer128 b);
struct integer128 integer128_or(struct integer128 a, struct integer128 b);
struct integer128 integer128_xor(struct integer128 a, struct integer128 b);

struct integer128 integer128_shl(struct integer128 a, int count);
struct integer128 integer128_shr_u(struct integer128 a, int count); /* fills with 0 */
struct integer128 integer128_shr_s(struct integer128 a, int count); /* fills with the sign bit */

/*
 *  Quotient truncated toward zero, remainder with the sign of the dividend
 *  (C semantics). Return false, leaving *quotient and *remainder 0, when the
 *  divisor is 0. The signed MIN / -1 wraps to MIN with remainder 0.
 */
bool integer128_divmod_s(struct integer128 a, struct integer128 b, _Out struct integer128* quotient, _Out struct integer128* remainder);
bool integer128_divmod_u(struct integer128 a, struct integer128 b, _Out struct integer128* quotient, _Out struct integer128* remainder);

/*
 *  Keep the low 'width' bits. _u fills the bits above with 0, _s copies
 *  bit width - 1 into them, so the result is the value as a 'width' bit
 *  integer seen in INTEGER128_BITS.
 */
struct integer128 integer128_wrap_u(struct integer128 a, int width);
struct integer128 integer128_wrap_s(struct integer128 a, int width);

/*
 *  Decimal text, NUL-terminated, returns its length. buffer must hold at
 *  least INTEGER128_TO_STRING_MIN_SIZE bytes (the digits of 2^INTEGER128_BITS, a sign
 *  and the NUL); a smaller size writes an empty string and returns 0.
 */
#define INTEGER128_TO_STRING_MIN_SIZE (INTEGER128_BITS * 302 / 1000 + 3)

int integer128_to_string_s(struct integer128 a, char* buffer, int size);
int integer128_to_string_u(struct integer128 a, char* buffer, int size);

/* the low bits that fit, like a conversion of the value to unsigned long long or long long */
unsigned long long integer128_to_ull(struct integer128 a);
long long integer128_to_ll(struct integer128 a);

/* true when the value is the same as an unsigned long long (_u) or a long long (_s) */
bool integer128_fits_ull(struct integer128 a);
bool integer128_fits_ll(struct integer128 a);

/*
 *  The value as a C expression of type unsigned __int128 built from the
 *  words, for values that have no literal: ((unsigned __int128)0x1ULL << 64 | 0x0ULL).
 *  Returns the length, or 0 (empty string) when size is too small.
 */
#define INTEGER128_TO_C_TEXT_MIN_SIZE (INTEGER128_WORDS * 48 + 32)
int integer128_to_c_text(struct integer128 a, char* buffer, int size);
