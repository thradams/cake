/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once

#include "cake_compat.h"
#include <stdbool.h>
#include <limits.h>

/*
 *  wide_integer: an integer with the double of the bits of an unsigned
 *  long long of the host, made of two of them, so it works on hosts that
 *  have nothing wider (msvc). cake keeps the constant value of every
 *  integer in one (struct object::value.host_wide_integer), up to a long
 *  long long.
 *
 *  struct wide_integer is only the bits, in two's complement. Like in the
 *  hardware, the same bits are a signed or an unsigned value: the
 *  operations where that matters have a _s and a _u version.
 *
 *  Shift counts are 0..WIDE_INTEGER_BITS - 1 and widths are 1..WIDE_INTEGER_BITS;
 *  anything else is a caller error (asserted).
 */

#define WIDE_INTEGER_HALF_BITS ((int)(sizeof(unsigned long long) * CHAR_BIT))
#define WIDE_INTEGER_BITS (2 * WIDE_INTEGER_HALF_BITS)

struct wide_integer
{
    unsigned long long lo;
    unsigned long long hi;
};

struct wide_integer wide_integer_from_signed(long long value);
struct wide_integer wide_integer_from_unsigned(unsigned long long value);

/*
 *  The value of the digits of text in base (2, 8, 10 or 16), up to the
 *  first character that is not one. *overflow tells it did not fit in
 *  WIDE_INTEGER_BITS (the result is then the value modulo 2^WIDE_INTEGER_BITS).
 */
struct wide_integer wide_integer_from_digits(const char* text, int base, _Out bool* overflow);

/* truncates toward zero; the value must fit in WIDE_INTEGER_BITS */
struct wide_integer wide_integer_from_long_double(long double value);
long double wide_integer_to_long_double_s(struct wide_integer a);
long double wide_integer_to_long_double_u(struct wide_integer a);

bool wide_integer_is_zero(struct wide_integer a);
bool wide_integer_is_negative(struct wide_integer a);
bool wide_integer_equal(struct wide_integer a, struct wide_integer b);

/* -1, 0 or 1 */
int wide_integer_compare_s(struct wide_integer a, struct wide_integer b);
int wide_integer_compare_u(struct wide_integer a, struct wide_integer b);

/* these wrap around modulo 2^WIDE_INTEGER_BITS */
struct wide_integer wide_integer_add(struct wide_integer a, struct wide_integer b);
struct wide_integer wide_integer_sub(struct wide_integer a, struct wide_integer b);
struct wide_integer wide_integer_neg(struct wide_integer a);
struct wide_integer wide_integer_mul(struct wide_integer a, struct wide_integer b);

struct wide_integer wide_integer_not(struct wide_integer a);
struct wide_integer wide_integer_and(struct wide_integer a, struct wide_integer b);
struct wide_integer wide_integer_or(struct wide_integer a, struct wide_integer b);
struct wide_integer wide_integer_xor(struct wide_integer a, struct wide_integer b);

struct wide_integer wide_integer_shl(struct wide_integer a, int count);
struct wide_integer wide_integer_shr_u(struct wide_integer a, int count); /* fills with 0 */
struct wide_integer wide_integer_shr_s(struct wide_integer a, int count); /* fills with the sign bit */

/*
 *  Quotient truncated toward zero, remainder with the sign of the dividend
 *  (C semantics). Return false, leaving *quotient and *remainder 0, when the
 *  divisor is 0. The signed MIN / -1 wraps to MIN with remainder 0.
 */
bool wide_integer_divmod_s(struct wide_integer a, struct wide_integer b, _Out struct wide_integer* quotient, _Out struct wide_integer* remainder);
bool wide_integer_divmod_u(struct wide_integer a, struct wide_integer b, _Out struct wide_integer* quotient, _Out struct wide_integer* remainder);

/*
 *  Keep the low 'width' bits. _u fills the bits above with 0, _s copies
 *  bit width - 1 into them, so the result is the value as a 'width' bit
 *  integer seen in WIDE_INTEGER_BITS.
 */
struct wide_integer wide_integer_wrap_u(struct wide_integer a, int width);
struct wide_integer wide_integer_wrap_s(struct wide_integer a, int width);

/*
 *  Decimal text, NUL-terminated, returns its length. buffer must hold at
 *  least WIDE_INTEGER_TO_STRING_MIN_SIZE bytes (the digits of 2^WIDE_INTEGER_BITS, a sign
 *  and the NUL); a smaller size writes an empty string and returns 0.
 */
#define WIDE_INTEGER_TO_STRING_MIN_SIZE (WIDE_INTEGER_BITS * 302 / 1000 + 3)

int wide_integer_to_string_s(struct wide_integer a, char* buffer, int size);
int wide_integer_to_string_u(struct wide_integer a, char* buffer, int size);
