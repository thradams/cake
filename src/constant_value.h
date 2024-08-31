#pragma once

#include <stdbool.h>
#include <wchar.h>

enum constant_value_type {
    TYPE_NOT_CONSTANT,
    TYPE_BOOL,

    TYPE_SIGNED_CHAR,
    TYPE_UNSIGNED_CHAR,

    TYPE_SIGNED_SHORT,
    TYPE_UNSIGNED_SHORT,

    TYPE_SIGNED_INT,
    TYPE_UNSIGNED_INT,

    TYPE_SIGNED_LONG,
    TYPE_UNSIGNED_LONG,

    TYPE_SIGNED_LONG_LONG,
    TYPE_UNSIGNED_LONG_LONG,

    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_LONG_DOUBLE
};

struct constant_value {
    enum constant_value_type type;
    union {
        _Bool bool_value;

        signed char signed_char_value;
        unsigned char unsigned_char_value;

        signed short signed_short_value;
        unsigned short unsigned_short_value;

        signed int signed_int_value;
        unsigned int unsigned_int_value;

        signed long signed_long_value;
        unsigned long unsigned_long_value;

        signed long long signed_long_long_value;
        unsigned long long unsigned_long_long_value;

        float float_value;
        double double_value;
        long double long_double_value;
    };
};

bool constant_value_is_valid(const struct constant_value* a);
void constant_value_to_string(const struct constant_value* a, char buffer[], int sz);

//Make constant value
struct constant_value            constant_value_make_wchar_t(wchar_t value);
struct constant_value             constant_value_make_size_t(size_t value);
struct constant_value               constant_value_make_bool(bool value);
struct constant_value            constant_value_make_nullptr();
struct constant_value        constant_value_make_signed_char(signed char value);
struct constant_value      constant_value_make_unsigned_char(unsigned char value);
struct constant_value       constant_value_make_signed_short(signed short value);
struct constant_value     constant_value_make_unsigned_short(unsigned short value);
struct constant_value         constant_value_make_signed_int(signed int value);
struct constant_value       constant_value_make_unsigned_int(unsigned int value);
struct constant_value        constant_value_make_signed_long(signed long value);
struct constant_value      constant_value_make_unsigned_long(unsigned long value);
struct constant_value   constant_value_make_signed_long_long(signed long long value);
struct constant_value constant_value_make_unsigned_long_long(unsigned long long value);
struct constant_value              constant_value_make_float(float value);
struct constant_value             constant_value_make_double(double value);
struct constant_value        constant_value_make_long_double(long double value);

//dynamic cast
struct constant_value constant_value_cast(enum constant_value_type e, const struct constant_value* a);

//static cast
signed char constant_value_to_signed_char(const struct constant_value* a);
unsigned char constant_value_to_unsigned_char(const struct constant_value* a);
signed short constant_value_to_signed_short(const struct constant_value* a);
unsigned short constant_value_to_unsigned_short(const struct constant_value* a);
signed int constant_value_to_signed_int(const struct constant_value* a);
unsigned int constant_value_to_unsigned_int(const struct constant_value* a);
signed long constant_value_to_signed_long(const struct constant_value* a);
unsigned long constant_value_to_unsigned_long(const struct constant_value* a);
signed long long constant_value_to_signed_long_long(const struct constant_value* a);
unsigned long long constant_value_to_unsigned_long_long(const struct constant_value* a);
float constant_value_to_float(const struct constant_value* a);
double constant_value_to_double(const struct constant_value* a);
long double constant_value_to_long_double(const struct constant_value* a);
bool constant_value_to_bool(const struct constant_value* a);

//Oveflow checks
bool unsigned_long_long_sub(_Out unsigned long long* result, unsigned long long a, unsigned long long b);
bool unsigned_long_long_mul(_Out unsigned long long* result, unsigned long long a, unsigned long long b);
bool unsigned_long_long_add(_Out unsigned long long* result, unsigned long long a, unsigned long long b);
bool signed_long_long_sub(_Out signed long long* result, signed long long a, signed long long b);
bool signed_long_long_add(_Out signed long long* result, signed long long a, signed long long b);
bool signed_long_long_mul(_Out signed long long* result, signed long long a, signed long long b);