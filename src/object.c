/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "ownership.h"
#include "object.h"
#include <limits.h>
#include <stdio.h>
#include <assert.h>
#include "type.h"
#include "error.h"
#include "parser.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

bool unsigned_long_long_sub(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b)
{
    *result = 0;

    if (a < b)
        return false;

    *result = a - b;
    return true;
}

bool unsigned_long_long_mul(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b)
{
    *result = 0;

    if (b == 0)
    {
        /*
          b cannot be zero in the next test
          so we solve this case here
        */
        *result = 0;
        return true;
    }

    if (a > ULLONG_MAX / b)
        return false;

    *result = a * b;
    return true;
}

bool unsigned_long_long_add(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b)
{
    *result = 0;

    if (a > ULLONG_MAX - b)
    {
        //a=2
        //b=254
        return false;
    }
    *result = a + b;
    return true;
}

bool signed_long_long_sub(_Ctor signed long long* result, signed long long a, signed long long b)
{
    *result = 0;

    if (a >= 0 && b >= 0)
    {
    }
    else if (a < 0 && b < 0)
    {
    }
    else
    {
        if (a < 0)
        {
            if (a < LLONG_MIN + b)
            {
                // (-128) - (-1)
                return false;
            }
        }
        else
        {
            if (b == LLONG_MIN)
            {
                // 0 - (-128)                
                return false;
            }

            if (a > LLONG_MAX - (-b))
            {
                /*
                *  1 - (-127)
                *  2 - (-126)
                */
                return false;
            }
        }
    }

    *result = a - b;
    return true;
}

bool signed_long_long_add(_Ctor signed long long* result, signed long long a, signed long long b)
{
    *result = 0;

    if (a >= 0 && b >= 0)
    {
        /*both positive*/
        if (a > LLONG_MAX - b)
        {
            //2+126
            return false;
        }
    }
    else if (a < 0 && b < 0)
    {

        if (a == LLONG_MIN || b == LLONG_MIN)
        {
            //(-128) + (-128)
            return false;
        }

        if (a < LLONG_MIN - b)
        {
            // (-127) + (-127)
            return false;
        }
    }
    else
    {
        /*one positive another negative*/

    }

    *result = a + b;
    return true;
}

bool signed_long_long_mul(_Ctor signed long long* result, signed long long a, signed long long b)
{
    *result = 0;

    if (a > 0 && b > 0)
    {
        if (a > LLONG_MAX / b)
        {
            //2*64
            return false;
        }
    }
    else if (a < 0 && b < 0)
    {

        if (a == LLONG_MIN || b == LLONG_MIN)
        {
            //(-128)*(-128)
            return false;
        }

        if (-a > LLONG_MAX / -b)
        {
            //-127 * -127
            return false;
        }
    }
    else
    {
        if (a == 0 || b == 0)
        {
            *result = 0;
            return true;
        }
        if (b > 0)
        {
            if (a < LLONG_MIN / b)
                //(-127) * (2)
                return false;
        }
        else
        {
            if (b < LLONG_MIN / a)
            {
                //2*(-128)
                return false;
            }
        }
    }

    *result = a * b;
    return true;
}

void object_destroy(_Opt _Dtor struct object* p)
{
    type_destroy(&p->type);
    free(p->debug_name);
}

void object_delete(struct object* _Opt _Owner p)
{
    if (p)
    {
        object_destroy(p);
        free(p);
    }
}

bool object_has_constant_value(const struct object* a)
{
    a = object_get_referenced(a);
    return a->state == CONSTANT_VALUE_STATE_CONSTANT_EXACT;
}

void object_to_string(const struct object* a, char buffer[], int sz)
{
    a = object_get_referenced(a);

    buffer[0] = 0;
    switch (a->value_type)
    {
    case TYPE_BOOL:
        snprintf(buffer, sz, "%s", a->value.signed_char_value ? "true" : "false");
        break;

    case TYPE_SIGNED_CHAR:
        snprintf(buffer, sz, "%c", a->value.signed_char_value);
        break;

    case TYPE_UNSIGNED_CHAR:
        snprintf(buffer, sz, "%c", a->value.unsigned_char_value);
        break;

    case TYPE_SIGNED_SHORT:
        snprintf(buffer, sz, "%c", a->value.signed_short_value);
        break;
    case TYPE_UNSIGNED_SHORT:
        snprintf(buffer, sz, "%c", a->value.signed_short_value);
        break;

    case TYPE_SIGNED_INT:
        snprintf(buffer, sz, "%d", a->value.signed_int_value);
        break;

    case TYPE_UNSIGNED_INT:
        snprintf(buffer, sz, "%u", a->value.signed_int_value);
        break;

    case TYPE_SIGNED_LONG:
        snprintf(buffer, sz, "%ld", a->value.signed_long_value);
        break;

    case TYPE_UNSIGNED_LONG:
        break;

    case TYPE_SIGNED_LONG_LONG:
        snprintf(buffer, sz, "%lud", a->value.signed_long_value);
        break;

    case TYPE_UNSIGNED_LONG_LONG:
        snprintf(buffer, sz, "%llu", a->value.signed_long_long_value);
        break;

    case TYPE_FLOAT:
        snprintf(buffer, sz, "%f", a->value.float_value);
        break;

    case TYPE_DOUBLE:
        snprintf(buffer, sz, "%f", a->value.double_value);
        break;

    case TYPE_LONG_DOUBLE:
        snprintf(buffer, sz, "%Lf", a->value.long_double_value);
        break;
    }
}

struct object object_make_size_t(size_t value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;

#if defined(_WIN64) || defined(__x86_64__) 
    r.value_type = TYPE_UNSIGNED_LONG_LONG;
    r.value.unsigned_long_long_value = value;
#else
    r.value_type = TYPE_UNSIGNED_INT;
    r.value.unsigned_int_value = value;
#endif

    return r;
}

struct object object_make_nullptr()
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;

    r.value_type = TYPE_SIGNED_INT;
    r.value.signed_short_value = 0;
    return r;
}

struct object object_make_wchar_t(wchar_t value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;

#ifdef _WIN32
    static_assert(_Generic(L' ', unsigned short : 1), "");
    r.value_type = TYPE_SIGNED_SHORT;
    r.value.signed_short_value = value;
#else
    static_assert(_Generic(L' ', int : 1), "");
    r.value_type = TYPE_SIGNED_INT;
    r.value.signed_int_value = value;
#endif

    return r;
}

struct object object_make_bool(bool value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;

    r.value_type = TYPE_BOOL;
    r.value.bool_value = value;
    return r;
}

#pragma warning( push )
#pragma warning( disable : 4244 )

bool object_to_bool(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_signed_char(signed char value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_SIGNED_CHAR;
    r.value.signed_char_value = value;
    return r;
}

errno_t object_increment_value(struct object* a)
{
    a = object_get_non_const_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL:
        return a->value.bool_value++;
    case TYPE_SIGNED_CHAR:
        return a->value.signed_char_value++;
    case TYPE_UNSIGNED_CHAR:
        return a->value.unsigned_char_value++;
    case TYPE_SIGNED_SHORT:
        return a->value.signed_short_value++;
    case TYPE_UNSIGNED_SHORT:
        return a->value.unsigned_short_value++;
    case TYPE_SIGNED_INT:
        return a->value.signed_int_value++;
    case TYPE_UNSIGNED_INT:
        return a->value.unsigned_int_value++;
    case TYPE_SIGNED_LONG:
        return a->value.signed_long_value++;
    case TYPE_UNSIGNED_LONG:
        return a->value.unsigned_long_value++;
    case TYPE_SIGNED_LONG_LONG:
        return a->value.signed_long_long_value++;
    case TYPE_UNSIGNED_LONG_LONG:
        return a->value.unsigned_long_long_value++;
    case TYPE_FLOAT:
        return a->value.float_value++;
    case TYPE_DOUBLE:
        return a->value.double_value++;
    case TYPE_LONG_DOUBLE:
        return a->value.long_double_value++;
    }
    assert(0);
    return 0;
}

signed char object_to_signed_char(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}

struct object object_make_unsigned_char(unsigned char value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_UNSIGNED_CHAR;
    r.value.unsigned_char_value = value;
    return r;
}

unsigned char object_to_unsigned_char(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_signed_short(signed short value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_SIGNED_SHORT;
    r.value.signed_short_value = value;
    return r;
}

signed short object_to_signed_short(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_unsigned_short(unsigned short value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_UNSIGNED_SHORT;
    r.value.unsigned_short_value = value;
    return r;
}

unsigned short object_to_unsigned_short(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_signed_int(signed int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_SIGNED_INT;
    r.value.signed_int_value = value;
    return r;
}

signed int object_to_signed_int(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_unsigned_int(unsigned int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_UNSIGNED_INT;
    r.value.unsigned_int_value = value;
    return r;
}

unsigned int object_to_unsigned_int(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_signed_long(signed long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_SIGNED_LONG;
    r.value.signed_long_value = value;
    return r;
}

signed long object_to_signed_long(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_unsigned_long(unsigned long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_UNSIGNED_LONG;
    r.value.unsigned_long_value = value;
    return r;
}

unsigned long object_to_unsigned_long(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_signed_long_long(signed long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_SIGNED_LONG_LONG;
    r.value.signed_long_long_value = value;
    return r;
}

signed long long object_to_signed_long_long(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_unsigned_long_long(unsigned long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_UNSIGNED_LONG_LONG;
    r.value.unsigned_long_long_value = value;
    return r;
}

unsigned long long object_to_unsigned_long_long(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_float(float value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_FLOAT;
    r.value.float_value = value;
    return r;
}

float object_to_float(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}
struct object object_make_double(double value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_DOUBLE;
    r.value.double_value = value;
    return r;
}

double object_to_double(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}



struct object object_make_reference(struct object* object)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_REFERENCE;
    r.value_type = TYPE_SIGNED_INT;
    r.value.signed_int_value = 0;

    //referenced object is stored at members

    //If state is CONSTANT_VALUE_STATE_REFERENCE then  members is not owner
    r.members = object_get_non_const_referenced(object);

    return r;
}

struct object object_make_long_double(long double value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
    r.value_type = TYPE_LONG_DOUBLE;
    r.value.long_double_value = value;
    return r;
}

long double object_to_long_double(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL: return a->value.bool_value;
    case TYPE_SIGNED_CHAR: return a->value.signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->value.unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->value.signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->value.unsigned_short_value;
    case TYPE_SIGNED_INT: return a->value.signed_int_value;
    case TYPE_UNSIGNED_INT: return a->value.unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->value.signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->value.unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->value.signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->value.unsigned_long_long_value;
    case TYPE_FLOAT: return a->value.float_value;
    case TYPE_DOUBLE: return a->value.double_value;
    case TYPE_LONG_DOUBLE: return a->value.long_double_value;
    }
    assert(0);
    return 0;
}



#pragma warning( pop )



struct object object_cast(enum object_value_type t, const struct object* v)
{
    v = object_get_referenced(v);

    //No changes
    if (v->value_type == t)
        return *v;


    //This function is generated by this program
    /*
            struct type
            {
                const char * type;
                const char * name;
                const char * value_type;
            };
            struct type types[] =
            {
                {"bool", "bool", "TYPE_BOOL"},
                {"signed char", "signed_char", "TYPE_SIGNED_CHAR"},
                {"unsigned char", "unsigned_char", "TYPE_UNSIGNED_CHAR"},
                {"signed short", "signed_short", "TYPE_SIGNED_SHORT"},
                {"unsigned short", "unsigned_short", "TYPE_UNSIGNED_SHORT"},
                {"signed int", "signed_int", "TYPE_SIGNED_INT"},
                {"unsigned int", "unsigned_int", "TYPE_UNSIGNED_INT"},
                {"signed long", "signed_long", "TYPE_SIGNED_LONG"},
                {"unsigned long", "unsigned_long", "TYPE_UNSIGNED_LONG"},
                {"signed long long", "signed_long_long", "TYPE_SIGNED_LONG_LONG"},
                {"unsigned long long", "unsigned_long_long", "TYPE_UNSIGNED_LONG_LONG"},
                {"float", "float", "TYPE_FLOAT"},
                {"double", "double", "TYPE_DOUBLE"},
                {"long double", "long_double", "TYPE_LONG_DOUBLE"}
            };


            int main()
            {
                FILE * f = fopen("imp.c", "w");
                if (f == NULL)
                    return;

                fprintf(f, "struct object cast(enum object_value_type t, struct object * v)\n");
                fprintf(f, "{\n");
                for (int i = 0; i < sizeof(types) / sizeof(types[0]); i++)
                {
                    fprintf(f, "if (t == %s)\n", types[i].value_type);
                    fprintf(f, "{\n");
                    for (int j = 0; j < sizeof(types) / sizeof(types[0]); j++)
                    {
                        if (i == j)
                            continue;
                        fprintf(f, "if (v->type == %s)\n", types[j].value_type);
                        fprintf(f, " return object_make_%s((%s)v->value.%s_value);\n", types[i].name, types[i].type, types[j].name);
                    }
                    fprintf(f, "}\n");
                }
                fprintf(f, "}\n");

                fclose(f);
            }
    */
    if (t == TYPE_BOOL)
    {
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_bool((bool)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_bool((bool)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_bool((bool)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_bool((bool)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_bool((bool)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_bool((bool)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_bool((bool)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_bool((bool)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_bool((bool)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_bool((bool)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_bool((bool)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_bool((bool)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_bool((bool)v->value.long_double_value);
    }
    if (t == TYPE_SIGNED_CHAR)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_signed_char((signed char)v->value.bool_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_signed_char((signed char)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_signed_char((signed char)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_signed_char((signed char)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_signed_char((signed char)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_signed_char((signed char)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_signed_char((signed char)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_signed_char((signed char)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_signed_char((signed char)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_signed_char((signed char)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_signed_char((signed char)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_signed_char((signed char)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_signed_char((signed char)v->value.long_double_value);
    }
    if (t == TYPE_UNSIGNED_CHAR)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_unsigned_char((unsigned char)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_unsigned_char((unsigned char)v->value.signed_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_unsigned_char((unsigned char)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_unsigned_char((unsigned char)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_unsigned_char((unsigned char)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_unsigned_char((unsigned char)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_unsigned_char((unsigned char)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_unsigned_char((unsigned char)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_unsigned_char((unsigned char)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_unsigned_char((unsigned char)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_unsigned_char((unsigned char)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_unsigned_char((unsigned char)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_unsigned_char((unsigned char)v->value.long_double_value);
    }
    if (t == TYPE_SIGNED_SHORT)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_signed_short((signed short)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_signed_short((signed short)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_signed_short((signed short)v->value.unsigned_char_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_signed_short((signed short)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_signed_short((signed short)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_signed_short((signed short)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_signed_short((signed short)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_signed_short((signed short)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_signed_short((signed short)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_signed_short((signed short)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_signed_short((signed short)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_signed_short((signed short)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_signed_short((signed short)v->value.long_double_value);
    }
    if (t == TYPE_UNSIGNED_SHORT)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_unsigned_short((unsigned short)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_unsigned_short((unsigned short)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_unsigned_short((unsigned short)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_unsigned_short((unsigned short)v->value.signed_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_unsigned_short((unsigned short)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_unsigned_short((unsigned short)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_unsigned_short((unsigned short)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_unsigned_short((unsigned short)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_unsigned_short((unsigned short)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_unsigned_short((unsigned short)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_unsigned_short((unsigned short)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_unsigned_short((unsigned short)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_unsigned_short((unsigned short)v->value.long_double_value);
    }
    if (t == TYPE_SIGNED_INT)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_signed_int((signed int)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_signed_int((signed int)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_signed_int((signed int)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_signed_int((signed int)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_signed_int((signed int)v->value.unsigned_short_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_signed_int((signed int)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_signed_int((signed int)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_signed_int((signed int)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_signed_int((signed int)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_signed_int((signed int)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_signed_int((signed int)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_signed_int((signed int)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_signed_int((signed int)v->value.long_double_value);
    }
    if (t == TYPE_UNSIGNED_INT)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_unsigned_int((unsigned int)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_unsigned_int((unsigned int)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_unsigned_int((unsigned int)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_unsigned_int((unsigned int)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_unsigned_int((unsigned int)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_unsigned_int((unsigned int)v->value.signed_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_unsigned_int((unsigned int)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_unsigned_int((unsigned int)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_unsigned_int((unsigned int)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_unsigned_int((unsigned int)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_unsigned_int((unsigned int)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_unsigned_int((unsigned int)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_unsigned_int((unsigned int)v->value.long_double_value);
    }
    if (t == TYPE_SIGNED_LONG)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_signed_long((signed long)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_signed_long((signed long)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_signed_long((signed long)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_signed_long((signed long)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_signed_long((signed long)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_signed_long((signed long)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_signed_long((signed long)v->value.unsigned_int_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_signed_long((signed long)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_signed_long((signed long)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_signed_long((signed long)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_signed_long((signed long)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_signed_long((signed long)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_signed_long((signed long)v->value.long_double_value);
    }
    if (t == TYPE_UNSIGNED_LONG)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_unsigned_long((unsigned long)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_unsigned_long((unsigned long)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_unsigned_long((unsigned long)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_unsigned_long((unsigned long)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_unsigned_long((unsigned long)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_unsigned_long((unsigned long)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_unsigned_long((unsigned long)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_unsigned_long((unsigned long)v->value.signed_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_unsigned_long((unsigned long)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_unsigned_long((unsigned long)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_unsigned_long((unsigned long)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_unsigned_long((unsigned long)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_unsigned_long((unsigned long)v->value.long_double_value);
    }
    if (t == TYPE_SIGNED_LONG_LONG)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_signed_long_long((signed long long)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_signed_long_long((signed long long)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_signed_long_long((signed long long)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_signed_long_long((signed long long)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_signed_long_long((signed long long)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_signed_long_long((signed long long)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_signed_long_long((signed long long)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_signed_long_long((signed long long)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_signed_long_long((signed long long)v->value.unsigned_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_signed_long_long((signed long long)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_signed_long_long((signed long long)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_signed_long_long((signed long long)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_signed_long_long((signed long long)v->value.long_double_value);
    }
    if (t == TYPE_UNSIGNED_LONG_LONG)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_unsigned_long_long((unsigned long long)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_unsigned_long_long((unsigned long long)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_unsigned_long_long((unsigned long long)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_unsigned_long_long((unsigned long long)v->value.long_double_value);
    }
    if (t == TYPE_FLOAT)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_float((float)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_float((float)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_float((float)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_float((float)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_float((float)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_float((float)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_float((float)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_float((float)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_float((float)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_float((float)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_float((float)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_float((float)v->value.double_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_float((float)v->value.long_double_value);
    }
    if (t == TYPE_DOUBLE)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_double((double)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_double((double)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_double((double)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_double((double)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_double((double)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_double((double)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_double((double)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_double((double)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_double((double)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_double((double)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_double((double)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_double((double)v->value.float_value);
        if (v->value_type == TYPE_LONG_DOUBLE)
            return object_make_double((double)v->value.long_double_value);
    }
    if (t == TYPE_LONG_DOUBLE)
    {
        if (v->value_type == TYPE_BOOL)
            return object_make_long_double((long double)v->value.bool_value);
        if (v->value_type == TYPE_SIGNED_CHAR)
            return object_make_long_double((long double)v->value.signed_char_value);
        if (v->value_type == TYPE_UNSIGNED_CHAR)
            return object_make_long_double((long double)v->value.unsigned_char_value);
        if (v->value_type == TYPE_SIGNED_SHORT)
            return object_make_long_double((long double)v->value.signed_short_value);
        if (v->value_type == TYPE_UNSIGNED_SHORT)
            return object_make_long_double((long double)v->value.unsigned_short_value);
        if (v->value_type == TYPE_SIGNED_INT)
            return object_make_long_double((long double)v->value.signed_int_value);
        if (v->value_type == TYPE_UNSIGNED_INT)
            return object_make_long_double((long double)v->value.unsigned_int_value);
        if (v->value_type == TYPE_SIGNED_LONG)
            return object_make_long_double((long double)v->value.signed_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG)
            return object_make_long_double((long double)v->value.unsigned_long_value);
        if (v->value_type == TYPE_SIGNED_LONG_LONG)
            return object_make_long_double((long double)v->value.signed_long_long_value);
        if (v->value_type == TYPE_UNSIGNED_LONG_LONG)
            return object_make_long_double((long double)v->value.unsigned_long_long_value);
        if (v->value_type == TYPE_FLOAT)
            return object_make_long_double((long double)v->value.float_value);
        if (v->value_type == TYPE_DOUBLE)
            return object_make_long_double((long double)v->value.double_value);
    }
    struct object empty = { 0 };
    return empty;
}

void object_default_initialization(struct object* p_object, bool is_constant)
{
    if (p_object->members == NULL)
    {
        if (is_constant)
            p_object->state = CONSTANT_VALUE_STATE_CONSTANT_EXACT;
        else
            p_object->state = CONSTANT_VALUE_STATE_EXACT;
        p_object->value.unsigned_long_long_value = 0;
    }

    if (type_is_union(&p_object->type))
    {
        struct object* _Opt p = p_object->members;
        if (p)
        {
            object_default_initialization(p, is_constant);
        }
    }
    else
    {
        struct object* _Opt p = p_object->members;
        while (p)
        {
            object_default_initialization(p, is_constant);
            p = p->next;
        }
    }
}

struct object* object_get_non_const_referenced(struct object* p_object)
{
    if (p_object->state == CONSTANT_VALUE_STATE_REFERENCE)
    {
        assert(p_object->members != NULL);
        p_object = p_object->members;
    }

    assert(p_object->state != CONSTANT_VALUE_STATE_REFERENCE);

    return p_object;
}


const struct object* object_get_referenced(const struct object* p_object)
{
    if (p_object->state == CONSTANT_VALUE_STATE_REFERENCE)
    {
        assert(p_object->members != NULL);
        p_object = p_object->members;
    }

    assert(p_object->state != CONSTANT_VALUE_STATE_REFERENCE);

    return p_object;
}

bool object_is_derived(const struct object* p_object)
{
    if (p_object->state == CONSTANT_VALUE_STATE_REFERENCE)
        return false;

    return p_object->members != NULL;
}

bool object_is_reference(const struct object* p_object)
{
    return p_object->state == CONSTANT_VALUE_STATE_REFERENCE;
}

static void object_fix_parent(struct object* p_object, struct object* parent)
{
    struct object* _Opt it = p_object->members;
    while (it)
    {
        it->parent = parent;
        it = it->next;
    }
}

struct object* _Opt object_get_member(struct object* p_object, int index)
{
    p_object = (struct object* _Opt) object_get_referenced(p_object);

    if (p_object->members == NULL)
        return NULL; //tODO

    struct object* _Opt it = p_object->members;
    int count = 0;
    while (it)
    {
        if (index == count)
            return it;
        count++;
        it = it->next;
    }

    return NULL;
}

void object_set(struct object* to, struct expression* _Opt p_init_expression, const struct object* from, bool is_constant)
{
    from = object_get_referenced(from);

    to->p_init_expression = p_init_expression;

    if (object_is_derived(to))
    {
        struct object* _Opt it_to = to->members;
        struct object* _Opt it_from = from->members;

        while (it_from && it_to)
        {
            object_set(it_to, NULL, it_from, is_constant);
            it_to = it_to->next;
            it_from = it_from->next;
        }
        if (it_from != NULL || it_to != NULL)
        {
            //TODO  in dev
          // assert(false);//TODO
        }
        //assert(it_from == NULL);
        //assert(it_to == NULL);
    }
    else
    {
        assert(to->members == NULL);

        to->state = from->state;
        to->value = object_cast(to->value_type, from).value;

        if (is_constant)
        {
            if (to->state == CONSTANT_VALUE_STATE_CONSTANT_EXACT ||
                to->state == CONSTANT_VALUE_STATE_EXACT)
            {
                /*
                struct X {int x;};
                int main() { constexpr struct X x = (struct X){ .x = 50 };}*/
                to->state = CONSTANT_VALUE_STATE_CONSTANT_EXACT ;
            }
        }
        else
        {
            if (to->state == CONSTANT_VALUE_STATE_CONSTANT_EXACT)
            {
                //Sample int i = 1; 1 is constant but i will not be
                to->state = CONSTANT_VALUE_STATE_EXACT;
            }
        }


    }
}

struct object* _Owner _Opt make_object_ptr_core(const struct type* p_type, const char* name)
{
    struct object* _Owner _Opt p_object = NULL;

    try
    {
        if (p_type->category == TYPE_CATEGORY_FUNCTION)
        {
            p_object = calloc(1, sizeof * p_object);
            if (p_object == NULL)
                throw;
            p_object->debug_name = strdup(name);
            p_object->type = type_dup(p_type);
            return p_object;
        }

        if (p_type->category == TYPE_CATEGORY_POINTER)
        {
            p_object = calloc(1, sizeof * p_object);
            if (p_object == NULL)
                throw;

            *p_object = object_make_nullptr();
            p_object->state = CONSTANT_VALUE_STATE_UNINITIALIZED;
            assert(p_object->debug_name == NULL);
            p_object->debug_name = strdup(name);
            p_object->type = type_dup(p_type);

            return p_object;
        }

        if (p_type->category == TYPE_CATEGORY_ARRAY)
        {
            p_object = calloc(1, sizeof * p_object);
            if (p_object == NULL)
                throw;
            p_object->type = type_dup(p_type);
            p_object->debug_name = strdup(name);

            if (p_type->num_of_elements > 0)
            {
                struct type array_item_type = get_array_item_type(p_type);

                struct object* _Opt p_tail_object = NULL;
                for (int i = 0; i < p_type->num_of_elements; i++)
                {
                    char buffer[200] = { 0 };
                    snprintf(buffer, sizeof buffer, "%s[%d]", name, i);
                    struct object* _Owner _Opt p_member_obj = make_object_ptr_core(&array_item_type, buffer);
                    if (p_member_obj == NULL)
                    {
                        type_destroy(&array_item_type);
                        throw;
                    }
                    p_member_obj->parent = p_object;
                    p_member_obj->debug_name = strdup(buffer);
                    if (p_tail_object == NULL)
                    {
                        assert(p_object->members == NULL);
                        p_object->members = p_member_obj;
                    }
                    else
                    {
                        assert(p_object->next == NULL);
                        p_tail_object->next = p_member_obj;
                    }

                    p_tail_object = p_member_obj;
                }
                type_destroy(&array_item_type);
            }

            return p_object;
        }


        if (p_type->struct_or_union_specifier == NULL)
        {
            p_object = calloc(1, sizeof * p_object);
            if (p_object == NULL)
                throw;


            p_object->state = CONSTANT_VALUE_STATE_UNINITIALIZED;
            p_object->value_type = type_to_object_type(p_type);
            p_object->value.signed_long_long_value = -1;
            p_object->debug_name = strdup(name);
            p_object->type = type_dup(p_type);

            return p_object;
        }


        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier == NULL)
        {
            //incomplete
            throw;
        }

        p_object = calloc(1, sizeof * p_object);
        if (p_object == NULL)
            throw;

        p_object->debug_name = strdup(name);
        p_object->type = type_dup(p_type);

        struct object* _Opt p_last_member_obj = NULL;

        struct member_declaration* _Opt p_member_declaration =
            p_struct_or_union_specifier->member_declaration_list.head;

        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator* _Opt p_member_declarator =
                    p_member_declaration->member_declarator_list_opt->head;

                while (p_member_declarator)
                {
                    if (p_member_declarator->declarator)
                    {
                        char buffer[200] = { 0 };
                        snprintf(buffer, sizeof buffer, "%s.%s", name, p_member_declarator->declarator->name_opt->lexeme);


                        struct object* _Owner _Opt p_member_obj = make_object_ptr_core(&p_member_declarator->declarator->type, buffer);
                        if (p_member_obj == NULL)
                            throw;

                        p_member_obj->parent = p_object;
                        p_member_obj->debug_name = strdup(buffer);

                        if (p_object->members == NULL)
                        {
                            p_object->members = p_member_obj;
                        }
                        else
                        {
                            //assert(p_last_member_obj->next != NULL);
                            p_last_member_obj->next = p_member_obj;
                        }
                        p_last_member_obj = p_member_obj;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else if (p_member_declaration->specifier_qualifier_list != NULL)
            {
                if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                {
                    struct type t = { 0 };
                    t.category = TYPE_CATEGORY_ITSELF;
                    t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                    char buffer[200] = { 0 };
                    snprintf(buffer, sizeof buffer, ".%s", name);


                    struct object* _Owner _Opt p_member_obj = make_object_ptr_core(&t, buffer);
                    if (p_member_obj == NULL)
                        throw;

                    p_member_obj->debug_name = strdup(buffer);
                    p_member_obj->parent = p_object;
                    if (p_last_member_obj == NULL)
                    {
                        assert(p_object->members == NULL);
                        p_object->members = p_member_obj;
                    }
                    else
                    {
                        p_last_member_obj->next = p_member_obj;
                    }
                    p_last_member_obj = p_member_obj;

                    type_destroy(&t);
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return p_object;
    }
    catch
    {
        object_delete(p_object);
        p_object = NULL;
    }

    return NULL;

}

struct object* _Owner _Opt make_object_ptr(const struct type* p_type)
{
    return make_object_ptr_core(p_type, "");
}

int make_object(const struct type* p_type, struct object* obj)
{
    struct object* _Owner _Opt p = make_object_ptr_core(p_type, "");
    if (p)
    {
        *obj = *p;
        object_fix_parent(obj, obj);
        free(p);
        return 0;
    }
    return 1;
}


enum object_value_type  type_specifier_to_object_type(const enum type_specifier_flags type_specifier_flags)
{

    if (type_specifier_flags & TYPE_SPECIFIER_BOOL)
        return TYPE_BOOL;

    if (type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        return TYPE_FLOAT;

    if (type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {
        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
            return TYPE_LONG_DOUBLE;
        return TYPE_DOUBLE;
    }


    if (type_specifier_flags & TYPE_SPECIFIER_UNSIGNED)
    {
        if (type_specifier_flags & TYPE_SPECIFIER_CHAR)
            return TYPE_UNSIGNED_CHAR;
        if (type_specifier_flags & TYPE_SPECIFIER_SHORT)
            return TYPE_UNSIGNED_SHORT;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
            return TYPE_UNSIGNED_LONG; /*check before int*/
        if (type_specifier_flags & TYPE_SPECIFIER_INT)
            return TYPE_UNSIGNED_INT;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
            return TYPE_UNSIGNED_LONG_LONG;
    }
    else
    {
        if (type_specifier_flags & TYPE_SPECIFIER_CHAR)
            return TYPE_SIGNED_CHAR;
        if (type_specifier_flags & TYPE_SPECIFIER_SHORT)
            return TYPE_SIGNED_SHORT;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
            return TYPE_SIGNED_LONG; /*check before int*/
        if (type_specifier_flags & TYPE_SPECIFIER_INT)
            return TYPE_SIGNED_INT;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
            return TYPE_SIGNED_LONG_LONG;
    }
    return TYPE_SIGNED_INT;
}

enum object_value_type type_to_object_type(const struct type* type)
{
    if (type_is_pointer(type))
    {
#if defined(_WIN64) || defined(__x86_64__) 
        return TYPE_UNSIGNED_LONG_LONG;
#else
        return TYPE_UNSIGNED_INT;
#endif
    }

    return type_specifier_to_object_type(type->type_specifier_flags);
}



void object_print_value_debug(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_BOOL:
        printf("%s (bool)", a->value.bool_value ? "true" : "false");
        break;

    case TYPE_SIGNED_CHAR:

        printf("%d (signed char)", (int)a->value.signed_char_value);
        break;


    case TYPE_UNSIGNED_CHAR:
        printf("%d (unsigned char)", (int)a->value.unsigned_char_value);
        break;


    case TYPE_SIGNED_SHORT:
        printf("%d (short)", a->value.signed_short_value);
        break;

    case TYPE_UNSIGNED_SHORT:
        printf("%d (unsigned short)", a->value.unsigned_short_value);
        break;

    case TYPE_SIGNED_INT:
        printf("%d (int)", a->value.signed_int_value);
        break;
    case TYPE_UNSIGNED_INT:
        printf("%du (unsigned int)", a->value.unsigned_int_value);
        break;
    case TYPE_SIGNED_LONG:
        printf("%ld (long)", a->value.signed_long_value);
        break;
    case TYPE_UNSIGNED_LONG:
        printf("%lu (unsigned long)", a->value.unsigned_long_value);
        break;
    case TYPE_SIGNED_LONG_LONG:
        printf("%lld (long long)", a->value.signed_long_long_value);
        break;
    case TYPE_UNSIGNED_LONG_LONG:
        printf("%llu (unsigned long long)", a->value.unsigned_long_long_value);
        break;
    case TYPE_FLOAT:
        printf("%f (float)", a->value.float_value);
        break;
    case TYPE_DOUBLE:
        printf("%lf (double)", a->value.double_value);
        break;
    case TYPE_LONG_DOUBLE:
        printf("%Lf (long double)", a->value.long_double_value);
        break;
    }

}

void object_print_to_debug_core(const struct object* object, int n)
{

    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }


    for (int i = 0; i < n; i++) printf("  ");
    if (object->debug_name)
        printf("%s ", object->debug_name);

    if (object->members != NULL)
    {

        type_print(&object->type);

        printf(" {\n");

        struct object* _Opt member = object->members;
        while (member)
        {
            object_print_to_debug_core(member, n + 1);
            member = member->next;
        }

        for (int i = 0; i < n; i++) printf("  ");
        printf("}\n");
    }
    else
    {



        type_print(&object->type);


        printf(" = ");

        object_print_value_debug(object);

        switch (object->state)
        {
        case CONSTANT_VALUE_STATE_UNINITIALIZED: printf(" uninitialized "); break;
        case CONSTANT_VALUE_STATE_UNKNOWN:printf(" unknown "); break;
        case CONSTANT_VALUE_STATE_EXACT:printf(" exact "); break;
        case CONSTANT_VALUE_STATE_CONSTANT_EXACT:printf(" constant_exact "); break;
        case CONSTANT_VALUE_STATE_REFERENCE: assert(false); break;
        }

        printf("\n");
    }

}

void object_print_to_debug(const struct object* object)
{
    int n = 0;
    object_print_to_debug_core(object, n);
}


/*
   extends the array to the max_index returning the added item.
*/
struct object* object_extend_array_to_index(const struct type* p_type, struct object* a, int max_index, bool is_constant)
{
    struct object* _Opt it = a->members;

    try
    {
        int count = 0;
        while (it)
        {
            count++;
            if (it->next == NULL)
                break;
            it = it->next;
        }

        while (count < (max_index + 1))
        {
            if (it == NULL)
            {
                assert(a->members == NULL);
                a->members = make_object_ptr(p_type);
                if (a->members == NULL)
                    throw;

                char name[100]={0};
                snprintf(name, sizeof name, "[%d]", count);
                a->members->debug_name = strdup(name);

                object_default_initialization(a->members, is_constant);

                it = a->members;
                it->parent = a;
                count++;
            }
            else
            {
                struct object* _Owner _Opt p = make_object_ptr(p_type);
                if (p == NULL)
                    throw;
                char name[100]={0};
                snprintf(name, sizeof name, "[%d]", count);
                p->debug_name = strdup(name);


                p->parent = a;
                object_default_initialization(p, is_constant);

                assert(it->next == NULL);
                it->next = p;

                it = p;
                count++;
            }
        }
    }
    catch
    {
    }
    return it;
}


