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

_Attr(nodiscard)
bool unsigned_long_long_sub(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b)
{
    *result = 0;

    if (a < b)
        return false;

    *result = a - b;
    return true;
}

_Attr(nodiscard)
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

_Attr(nodiscard)
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

_Attr(nodiscard)
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

_Attr(nodiscard)
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

_Attr(nodiscard)
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

void object_list_push(struct object_list* list, struct object* pnew)
{
    if (list->head == NULL)
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        assert(list->tail != NULL);
        list->tail->next = pnew;
        list->tail = pnew;
    }
    list->count++;
}

void object_swap(struct object* a, struct object* b)
{
    struct object temp = *a;
    *a = *b;
    *b = temp;
}

void object_destroy(_Opt _Dtor struct object* p)
{
    assert(p->next == NULL);

    type_destroy(&p->type);
    free((void* _Owner)p->member_designator);

    struct object* _Owner _Opt item = p->members.head;
    while (item)
    {
        struct object* _Owner _Opt next = item->next;
        item->next = NULL;
        object_delete(item);
        item = next;
    }
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
    return a->state == CONSTANT_VALUE_STATE_CONSTANT;
}

void object_to_string(const struct object* a, char buffer[], int sz)
{
    a = object_get_referenced(a);

    buffer[0] = 0;
    switch (a->value_type)
    {

    case TYPE_SIGNED_INT8:
        snprintf(buffer, sz, "%" PRIi8, a->value.signed_int8);
        break;

    case TYPE_UNSIGNED_INT8:
        snprintf(buffer, sz, "%" PRIu8, a->value.unsigned_int8);
        break;

    case TYPE_SIGNED_INT16:
        snprintf(buffer, sz, "%" PRIi16, a->value.signed_int16);
        break;
    case TYPE_UNSIGNED_INT16:
        snprintf(buffer, sz, "%" PRIu16, a->value.signed_int16);
        break;

    case TYPE_SIGNED_INT32:
        snprintf(buffer, sz, "%" PRIi32, a->value.signed_int32);
        break;

    case TYPE_UNSIGNED_INT32:
        snprintf(buffer, sz, "%" PRIu32, a->value.signed_int32);
        break;


    case TYPE_SIGNED_INT64:
        snprintf(buffer, sz, "%" PRIi64, a->value.signed_int64);
        break;

    case TYPE_UNSIGNED_INT64:
        snprintf(buffer, sz, "%" PRIu64, a->value.unsigned_int64);
        break;

    case TYPE_FLOAT32:
        snprintf(buffer, sz, "%f", a->value.float32);
        break;

    case TYPE_FLOAT64:
        snprintf(buffer, sz, "%f", a->value.float64);
        break;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        snprintf(buffer, sz, "%Lf", a->value.float128);
        break;
#endif


    }
}

struct object object_make_size_t(enum target target, uint64_t value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        r.value_type = TYPE_UNSIGNED_INT64;
        r.value.unsigned_int64 = value;
        break;

    case TARGET_X86_MSVC:
        r.value_type = TYPE_UNSIGNED_INT32;
        r.value.unsigned_int32 = (unsigned int)value;
        break;

    case TARGET_X64_MSVC:
        r.value_type = TYPE_UNSIGNED_INT64;
        r.value.unsigned_int64 = value;
        break;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        r.value_type = TYPE_UNSIGNED_INT16;
        r.value.unsigned_int16 = (uint16_t)value;
        break;

    case TARGET_CATALINA:
        r.value_type = TYPE_UNSIGNED_INT32;
        r.value.unsigned_int32 = (unsigned int)value;
        break;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    return r;
}

struct object object_make_nullptr(enum target target)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (target)
    {
    case TARGET_X86_X64_GCC:
        r.value_type = TYPE_UNSIGNED_INT64;
        r.value.unsigned_int64 = 0;
        break;

    case TARGET_X86_MSVC:
        r.value_type = TYPE_UNSIGNED_INT32;
        r.value.unsigned_int32 = 0;
        break;

    case TARGET_X64_MSVC:
        r.value_type = TYPE_UNSIGNED_INT64;
        r.value.unsigned_int64 = 0;
        break;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        r.value_type = TYPE_UNSIGNED_INT16;
        r.value.unsigned_int16 = 0;
        break;

    case TARGET_CATALINA:
        r.value_type = TYPE_UNSIGNED_INT32;
        r.value.unsigned_int32 = 0;
        break;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");


    return r;
}

struct object object_make_wchar_t(enum target target, int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (target)
    {
    case TARGET_X86_X64_GCC:
        r.value_type = TYPE_SIGNED_INT32;
        r.value.signed_int32 = value;
        break;
    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        r.value_type = TYPE_UNSIGNED_INT16;
        r.value.unsigned_int16 = (unsigned short)value;
        break;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        r.value_type = TYPE_UNSIGNED_INT8;
        r.value.unsigned_int8 = (uint8_t)value;
        break;
    case TARGET_CATALINA:
        r.value_type = TYPE_UNSIGNED_INT8;
        r.value.unsigned_int8 = (uint8_t)value;
        break;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");

    return r;
}

struct object object_make_bool(bool value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    r.value_type = TYPE_UNSIGNED_INT8;
    r.value.unsigned_int8 = value;
    return r;
}

#pragma warning( push )
#pragma warning( disable : 4244 )

int object_to_str(const struct object* a, int n, char str[/*n*/])
{
    str[0] = '\0';

    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8:
    case TYPE_SIGNED_INT16:
    case TYPE_SIGNED_INT32:

    case TYPE_SIGNED_INT64:
    {
        long long v = object_to_signed_long_long(a);
        snprintf(str, n, "%lld", v);
    }
    break;

    case TYPE_UNSIGNED_INT8:
    case TYPE_UNSIGNED_INT16:
    case TYPE_UNSIGNED_INT32:

    case TYPE_UNSIGNED_INT64:
    {
        unsigned long long v = object_to_unsigned_long_long(a);
        snprintf(str, n, "%llu", v);
    }
    break;

    case TYPE_FLOAT32:
    case TYPE_FLOAT64:
    {
        long double v = object_to_long_double(a);
        snprintf(str, n, "%Lf", v);
    }
    break;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
    {
        long double v = object_to_long_double(a);
        snprintf(str, n, "%Lf", v);
    }
#endif
    break;
    }

    return 0;
}

void object_set_signed_int(struct object* a, long long value)
{
    a = object_get_non_const_referenced(a);
    a->state = CONSTANT_VALUE_EQUAL;

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8:  a->value.signed_int8 = value; break;
    case TYPE_UNSIGNED_INT8:  a->value.unsigned_int8 = value; break;
    case TYPE_SIGNED_INT16:  a->value.signed_int16 = value; break;
    case TYPE_UNSIGNED_INT16:  a->value.unsigned_int16 = value; break;
    case TYPE_SIGNED_INT32:  a->value.signed_int32 = value; break;
    case TYPE_UNSIGNED_INT32:  a->value.unsigned_int32 = value; break;

    case TYPE_SIGNED_INT64:  a->value.signed_int64 = value; break;
    case TYPE_UNSIGNED_INT64:  a->value.unsigned_int64 = value; break;
    case TYPE_FLOAT32:  a->value.float32 = value; break;
    case TYPE_FLOAT64:  a->value.float64 = value; break;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:  a->value.float128 = value; break;
#endif



    }

}

void object_set_unsigned_int(struct object* a, unsigned long long value)
{
    a = object_get_non_const_referenced(a);
    a->state = CONSTANT_VALUE_EQUAL;

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8:  a->value.signed_int8 = value; break;
    case TYPE_UNSIGNED_INT8:  a->value.unsigned_int8 = value; break;
    case TYPE_SIGNED_INT16:  a->value.signed_int16 = value; break;
    case TYPE_UNSIGNED_INT16:  a->value.unsigned_int16 = value; break;
    case TYPE_SIGNED_INT32:  a->value.signed_int32 = value; break;
    case TYPE_UNSIGNED_INT32:  a->value.unsigned_int32 = value; break;

    case TYPE_SIGNED_INT64:  a->value.signed_int64 = value; break;
    case TYPE_UNSIGNED_INT64:  a->value.unsigned_int64 = value; break;
    case TYPE_FLOAT32:  a->value.float32 = value; break;
    case TYPE_FLOAT64:  a->value.float64 = value; break;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:  a->value.float128 = value; break;
#endif

    }

}

bool object_to_bool(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif

    }
    assert(0);
    return 0;
}
struct object object_make_signed_char(signed char value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_INT8;
    r.value.signed_int8 = value;
    return r;
}

errno_t object_increment_value(struct object* a)
{
    a = object_get_non_const_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8:
        a->value.signed_int8++;
        break;
    case TYPE_UNSIGNED_INT8:
        a->value.unsigned_int8++;
        break;
    case TYPE_SIGNED_INT16:
        a->value.signed_int16++;
        break;
    case TYPE_UNSIGNED_INT16:
        a->value.unsigned_int16++;
        break;
    case TYPE_SIGNED_INT32:
        a->value.signed_int32++;
        break;
    case TYPE_UNSIGNED_INT32:
        a->value.unsigned_int32++;
        break;

    case TYPE_SIGNED_INT64:
        a->value.signed_int64++;
        break;
    case TYPE_UNSIGNED_INT64:
        a->value.unsigned_int64++;
        break;
    case TYPE_FLOAT32:
        a->value.float32++;
        break;
    case TYPE_FLOAT64:
        a->value.float64++;
        break;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        a->value.float128++;
        break;
#endif


    default:
        return 1;
    }

    return 0;
}

signed char object_to_signed_char(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}

struct object object_make_unsigned_char(unsigned char value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_INT8;
    r.value.unsigned_int8 = value;
    return r;
}

unsigned char object_to_unsigned_char(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}
struct object object_make_signed_short(signed short value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_INT16;
    r.value.signed_int16 = value;
    return r;
}

signed short object_to_signed_short(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif

    }
    assert(0);
    return 0;
}
struct object object_make_unsigned_short(unsigned short value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_INT16;
    r.value.unsigned_int16 = value;
    return r;
}

unsigned short object_to_unsigned_short(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}
struct object object_make_signed_int(signed int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_INT32;
    r.value.signed_int32 = value;
    return r;
}

signed int object_to_signed_int(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}
struct object object_make_unsigned_int(unsigned int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_INT32;
    r.value.unsigned_int32 = value;
    return r;
}

unsigned int object_to_unsigned_int(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif

    }
    assert(0);
    return 0;
}
struct object object_make_signed_long(signed long long value, enum target target)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        r.value_type = TYPE_SIGNED_INT64;
        r.value.signed_int64 = value;
        break;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        r.value_type = TYPE_SIGNED_INT32;
        r.value.signed_int32 = value;
        break;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        r.value_type = TYPE_SIGNED_INT32;
        r.value.signed_int32 = value;
        break;

    case TARGET_CATALINA:
        r.value_type = TYPE_SIGNED_INT32;
        r.value.signed_int32 = value;
        break;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    return r;
}

signed long object_to_signed_long(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;
    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}
struct object object_make_unsigned_long(unsigned long long value, enum target target)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (target)
    {
    case TARGET_X86_X64_GCC:
        r.value_type = TYPE_UNSIGNED_INT64;
        r.value.unsigned_int64 = value;
        break;
    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        r.value_type = TYPE_UNSIGNED_INT32;
        r.value.unsigned_int32 = value;
        break;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        r.value_type = TYPE_UNSIGNED_INT32;
        r.value.unsigned_int32 = value;
        break;

    case TARGET_CATALINA:
        r.value_type = TYPE_UNSIGNED_INT32;
        r.value.unsigned_int32 = value;
        break;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    return r;
}

unsigned long object_to_unsigned_long(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}
struct object object_make_signed_long_long(signed long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_INT64;
    r.value.signed_int64 = value;
    return r;
}

signed long long object_to_signed_long_long(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}
struct object object_make_unsigned_long_long(unsigned long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_INT64;
    r.value.unsigned_int64 = value;
    return r;
}

unsigned long long object_to_unsigned_long_long(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}
struct object object_make_float(float value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_FLOAT32;
    r.value.float32 = value;
    return r;
}

float object_to_float(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}
struct object object_make_double(double value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_FLOAT64;
    r.value.float64 = value;
    return r;
}

double object_to_double(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
    }
    assert(0);
    return 0;
}


struct object object_make_pointer(struct object* object)
{
    object = object_get_non_const_referenced(object);

    struct object r = { 0 };
    r.state = CONSTANT_VALUE_EQUAL;
    r.value_type = TYPE_UNSIGNED_INT64;
    r.value.unsigned_int64 = (uint64_t)object;

    return r;
}

struct object object_make_reference(struct object* object)
{
    object = object_get_non_const_referenced(object);

    struct object r = { 0 };
    r.state = CONSTANT_VALUE_EQUAL;
    r.value_type = TYPE_UNSIGNED_INT64;
    r.value.unsigned_int64 = (uint64_t)object;
    r.p_ref = object;
    return r;
}

struct object object_make_long_double(long double value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
#ifdef CAKE_FLOAT128_DEFINED
    r.value_type = TYPE_FLOAT128;
    r.value.float128 = value;
#else
    r.value_type = TYPE_FLOAT64;
    r.value.float64 = value;
#endif
    return r;
}

long double object_to_long_double(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {


    case TYPE_SIGNED_INT8: return a->value.signed_int8;
    case TYPE_UNSIGNED_INT8: return a->value.unsigned_int8;
    case TYPE_SIGNED_INT16: return a->value.signed_int16;
    case TYPE_UNSIGNED_INT16: return a->value.unsigned_int16;
    case TYPE_SIGNED_INT32: return a->value.signed_int32;
    case TYPE_UNSIGNED_INT32: return a->value.unsigned_int32;

    case TYPE_SIGNED_INT64: return a->value.signed_int64;
    case TYPE_UNSIGNED_INT64: return a->value.unsigned_int64;
    case TYPE_FLOAT32: return a->value.float32;
    case TYPE_FLOAT64: return a->value.float64;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128: return a->value.float128;
#endif
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

                {"signed char", "signed_char", "TYPE_SIGNED_INT8"},
                {"unsigned char", "unsigned_char", "TYPE_UNSIGNED_INT8"},
                {"signed short", "signed_short", "TYPE_SIGNED_INT16"},
                {"unsigned short", "unsigned_short", "TYPE_UNSIGNED_INT16"},
                {"signed int", "signed_int", "TYPE_SIGNED_INT32"},
                {"unsigned int", "unsigned_int", "TYPE_UNSIGNED_INT32"},
                {"signed long", "signed_long", "TYPE_SIGNED_LONG"},
                {"unsigned long", "unsigned_long", "TYPE_UNSIGNED_LONG"},
                {"signed long long", "signed_long_long", "TYPE_SIGNED_INT64"},
                {"unsigned long long", "unsigned_long_long", "TYPE_UNSIGNED_INT64"},
                {"float", "float", "TYPE_FLOAT32"},
                {"double", "double", "TYPE_FLOAT64"},
                {"long double", "long_double", "TYPE_FLOAT128"}
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
    if (t == TYPE_SIGNED_INT8)
    {
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_signed_char((signed char)v->value.unsigned_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_signed_char((signed char)v->value.signed_int16);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_signed_char((signed char)v->value.unsigned_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_signed_char((signed char)v->value.signed_int32);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_signed_char((signed char)v->value.unsigned_int32);
        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_signed_char((signed char)v->value.signed_int64);
        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_signed_char((signed char)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_signed_char((signed char)v->value.float32);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_signed_char((signed char)v->value.float64);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_signed_char((signed char)v->value.float128);
#endif
    }
    if (t == TYPE_UNSIGNED_INT8)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_unsigned_char((unsigned char)v->value.signed_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_unsigned_char((unsigned char)v->value.signed_int16);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_unsigned_char((unsigned char)v->value.unsigned_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_unsigned_char((unsigned char)v->value.signed_int32);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_unsigned_char((unsigned char)v->value.unsigned_int32);

        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_unsigned_char((unsigned char)v->value.signed_int64);
        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_unsigned_char((unsigned char)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_unsigned_char((unsigned char)v->value.float32);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_unsigned_char((unsigned char)v->value.float64);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_unsigned_char((unsigned char)v->value.float128);
#endif
    }
    if (t == TYPE_SIGNED_INT16)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_signed_short((signed short)v->value.signed_int8);
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_signed_short((signed short)v->value.unsigned_int8);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_signed_short((signed short)v->value.unsigned_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_signed_short((signed short)v->value.signed_int32);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_signed_short((signed short)v->value.unsigned_int32);


        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_signed_short((signed short)v->value.signed_int64);
        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_signed_short((signed short)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_signed_short((signed short)v->value.float32);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_signed_short((signed short)v->value.float64);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_signed_short((signed short)v->value.float128);
#endif
    }
    if (t == TYPE_UNSIGNED_INT16)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_unsigned_short((unsigned short)v->value.signed_int8);
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_unsigned_short((unsigned short)v->value.unsigned_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_unsigned_short((unsigned short)v->value.signed_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_unsigned_short((unsigned short)v->value.signed_int32);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_unsigned_short((unsigned short)v->value.unsigned_int32);

        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_unsigned_short((unsigned short)v->value.signed_int64);
        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_unsigned_short((unsigned short)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_unsigned_short((unsigned short)v->value.float32);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_unsigned_short((unsigned short)v->value.float64);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_unsigned_short((unsigned short)v->value.float128);
#endif
    }
    if (t == TYPE_SIGNED_INT32)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_signed_int((signed int)v->value.signed_int8);
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_signed_int((signed int)v->value.unsigned_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_signed_int((signed int)v->value.signed_int16);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_signed_int((signed int)v->value.unsigned_int16);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_signed_int((signed int)v->value.unsigned_int32);

        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_signed_int((signed int)v->value.signed_int64);
        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_signed_int((signed int)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_signed_int((signed int)v->value.float32);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_signed_int((signed int)v->value.float64);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_signed_int((signed int)v->value.float128);
#endif
    }
    if (t == TYPE_UNSIGNED_INT32)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_unsigned_int((unsigned int)v->value.signed_int8);
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_unsigned_int((unsigned int)v->value.unsigned_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_unsigned_int((unsigned int)v->value.signed_int16);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_unsigned_int((unsigned int)v->value.unsigned_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_unsigned_int((unsigned int)v->value.signed_int32);

        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_unsigned_int((unsigned int)v->value.signed_int64);
        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_unsigned_int((unsigned int)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_unsigned_int((unsigned int)v->value.float32);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_unsigned_int((unsigned int)v->value.float64);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_unsigned_int((unsigned int)v->value.float128);
#endif
    }
    if (t == TYPE_SIGNED_INT64)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_signed_long_long((signed long long)v->value.signed_int8);
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_signed_long_long((signed long long)v->value.unsigned_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_signed_long_long((signed long long)v->value.signed_int16);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_signed_long_long((signed long long)v->value.unsigned_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_signed_long_long((signed long long)v->value.signed_int32);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_signed_long_long((signed long long)v->value.unsigned_int32);


        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_signed_long_long((signed long long)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_signed_long_long((signed long long)v->value.float32);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_signed_long_long((signed long long)v->value.float64);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_signed_long_long((signed long long)v->value.float128);
#endif
    }
    if (t == TYPE_UNSIGNED_INT64)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int8);
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int16);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int32);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_unsigned_long_long((unsigned long long)v->value.unsigned_int32);

        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_unsigned_long_long((unsigned long long)v->value.signed_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_unsigned_long_long((unsigned long long)v->value.float32);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_unsigned_long_long((unsigned long long)v->value.float64);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_unsigned_long_long((unsigned long long)v->value.float128);
#endif
    }
    if (t == TYPE_FLOAT32)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_float((float)v->value.signed_int8);
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_float((float)v->value.unsigned_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_float((float)v->value.signed_int16);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_float((float)v->value.unsigned_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_float((float)v->value.signed_int32);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_float((float)v->value.unsigned_int32);

        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_float((float)v->value.signed_int64);
        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_float((float)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_float((float)v->value.float64);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_float((float)v->value.float128);
#endif
    }
    if (t == TYPE_FLOAT64)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_double((double)v->value.signed_int8);
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_double((double)v->value.unsigned_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_double((double)v->value.signed_int16);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_double((double)v->value.unsigned_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_double((double)v->value.signed_int32);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_double((double)v->value.unsigned_int32);

        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_double((double)v->value.signed_int64);
        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_double((double)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_double((double)v->value.float32);
#ifdef CAKE_FLOAT128_DEFINED
        if (v->value_type == TYPE_FLOAT128)
            return object_make_double((double)v->value.float128);
#endif
    }
#ifdef CAKE_FLOAT128_DEFINED
    if (t == TYPE_FLOAT128)
    {

        if (v->value_type == TYPE_SIGNED_INT8)
            return object_make_long_double((long double)v->value.signed_int8);
        if (v->value_type == TYPE_UNSIGNED_INT8)
            return object_make_long_double((long double)v->value.unsigned_int8);
        if (v->value_type == TYPE_SIGNED_INT16)
            return object_make_long_double((long double)v->value.signed_int16);
        if (v->value_type == TYPE_UNSIGNED_INT16)
            return object_make_long_double((long double)v->value.unsigned_int16);
        if (v->value_type == TYPE_SIGNED_INT32)
            return object_make_long_double((long double)v->value.signed_int32);
        if (v->value_type == TYPE_UNSIGNED_INT32)
            return object_make_long_double((long double)v->value.unsigned_int32);

        if (v->value_type == TYPE_SIGNED_INT64)
            return object_make_long_double((long double)v->value.signed_int64);
        if (v->value_type == TYPE_UNSIGNED_INT64)
            return object_make_long_double((long double)v->value.unsigned_int64);
        if (v->value_type == TYPE_FLOAT32)
            return object_make_long_double((long double)v->value.float32);
        if (v->value_type == TYPE_FLOAT64)
            return object_make_long_double((long double)v->value.float64);
    }
#endif
    struct object empty = { 0 };
    return empty;

}

void object_default_initialization(struct object* p_object, bool is_constant)
{
    if (p_object->members.head == NULL)
    {
        if (is_constant)
            p_object->state = CONSTANT_VALUE_STATE_CONSTANT;
        else
            p_object->state = CONSTANT_VALUE_EQUAL;
        p_object->value.unsigned_int64 = 0;
    }

    if (type_is_union(&p_object->type))
    {
        struct object* _Opt p = p_object->members.head;
        if (p)
        {
            object_default_initialization(p, is_constant);
        }
    }
    else
    {
        struct object* _Opt p = p_object->members.head;
        while (p)
        {
            object_default_initialization(p, is_constant);
            p = p->next;
        }
    }
}

struct object* object_get_non_const_referenced(struct object* p_object)
{
    if (p_object->p_ref != NULL)
    {
        p_object = p_object->p_ref;
    }

    return p_object;
}


const struct object* object_get_referenced(const struct object* p_object)
{
    if (p_object->p_ref != NULL)
    {
        p_object = p_object->p_ref;
    }

    return p_object;
}


int get_rank(enum object_value_type t)
{
    //https://cigix.me/c23#6.3.1.1
    if (t == TYPE_SIGNED_INT64 ||
        t == TYPE_UNSIGNED_INT64)
    {
        return 80;
    }

    else if (t == TYPE_SIGNED_INT32 ||
             t == TYPE_UNSIGNED_INT32)
    {
        return 40;
    }
    else if (t == TYPE_SIGNED_INT16 ||
             t == TYPE_UNSIGNED_INT16)
    {
        return 30;
    }
    else if (t == TYPE_SIGNED_INT8 ||
             t == TYPE_UNSIGNED_INT8)
    {
        return 20;
    }
    return 0;
}


int get_size(enum object_value_type t)
{
    if (t == TYPE_SIGNED_INT64 ||
        t == TYPE_UNSIGNED_INT64)
    {
        return sizeof(long long);
    }

    else if (t == TYPE_SIGNED_INT32 ||
             t == TYPE_UNSIGNED_INT32)
    {
        return sizeof(int);
    }
    else if (t == TYPE_SIGNED_INT16 ||
             t == TYPE_UNSIGNED_INT16)
    {
        return sizeof(short);
    }
    else if (t == TYPE_SIGNED_INT8 ||
             t == TYPE_UNSIGNED_INT8)
    {
        return sizeof(char);
    }

    return 1;
}

bool is_signed(enum object_value_type t)
{
    switch (t)
    {

    case TYPE_SIGNED_INT8:
    case TYPE_SIGNED_INT16:
    case TYPE_SIGNED_INT32:

    case TYPE_SIGNED_INT64:
    case TYPE_FLOAT64:
        return true;

#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        return true;
#endif

    default:
        break;
    }
    return false;
}

enum object_value_type to_unsigned(enum object_value_type t)
{
    switch (t)
    {
    case TYPE_SIGNED_INT8: return TYPE_UNSIGNED_INT8;
    case TYPE_SIGNED_INT16:return TYPE_UNSIGNED_INT16;
    case TYPE_SIGNED_INT32: return TYPE_UNSIGNED_INT32;
    case TYPE_SIGNED_INT64: return TYPE_UNSIGNED_INT64;
    default:
        break;
    }
    return t;
}

bool is_unsigned(enum object_value_type t)
{
    switch (t)
    {

    case TYPE_UNSIGNED_INT8:
    case TYPE_UNSIGNED_INT16:
    case TYPE_UNSIGNED_INT32:
    case TYPE_UNSIGNED_INT64:
        return true;
    default:
        break;
    }
    return false;
}

void object_set_any(struct object* p_object)
{
    p_object = object_get_non_const_referenced(p_object);
    p_object->state = CONSTANT_VALUE_STATE_ANY;
    struct object* _Opt p = p_object->members.head;
    while (p)
    {
        object_set_any(p);
        p = p->next;
    }
}

bool object_is_zero(const struct object* p_object)
{
    p_object = (struct object* _Opt) object_get_referenced(p_object);

    if (!object_has_constant_value(p_object))
        return false;


    if (object_is_signed(p_object))
    {
        signed long long r = object_to_signed_long_long(p_object);
        return r == 0;
    }

    unsigned long long r = object_to_unsigned_long_long(p_object);
    return r == 0;
}


bool object_is_one(const struct object* p_object)
{
    p_object = (struct object* _Opt) object_get_referenced(p_object);

    if (!object_has_constant_value(p_object))
        return false;

    if (object_is_signed(p_object))
    {
        signed long long r = object_to_signed_long_long(p_object);
        return r == 1;
    }

    unsigned long long r = object_to_unsigned_long_long(p_object);
    return r == 1;
}

bool object_is_signed(const struct object* p_object)
{
    p_object = (struct object* _Opt) object_get_referenced(p_object);
    return is_signed(p_object->value_type);
}

bool object_is_derived(const struct object* p_object)
{
    if (p_object->p_ref != NULL)
        return false;

    return p_object->members.head != NULL;
}

bool object_is_reference(const struct object* p_object)
{
    return p_object->p_ref != NULL;
}

static void object_fix_parent(struct object* p_object, struct object* parent)
{
    struct object* _Opt it = p_object->members.head;
    while (it)
    {
        it->parent = parent;
        it = it->next;
    }
}

struct object* _Opt object_get_member(struct object* p_object, size_t index)
{
    p_object = (struct object* _Opt) object_get_referenced(p_object);

    if (p_object->members.head == NULL)
        return NULL; //tODO

    struct object* _Opt it = p_object->members.head;
    size_t count = 0;
    while (it)
    {
        if (index == count)
            return it;
        count++;
        it = it->next;
    }

    return NULL;
}

int object_set(
    struct parser_ctx* ctx,
    struct object* to,
    struct expression* _Opt p_init_expression,
    const struct object* from,
    bool is_constant,
    bool requires_constant_initialization)
{
    try
    {
        from = object_get_referenced(from);

        to->p_init_expression = p_init_expression;

        if (object_is_derived(to))
        {
            struct object* _Opt it_to = to->members.head;
            struct object* _Opt it_from = from->members.head;

            while (it_from && it_to)
            {
                object_set(ctx, it_to, NULL, it_from, is_constant, requires_constant_initialization);
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
            assert(to->members.head == NULL);

            to->state = from->state;
            to->value = object_cast(to->value_type, from).value;

            if (requires_constant_initialization &&
                !object_has_constant_value(from))
            {
                if (!type_is_pointer_or_array(&p_init_expression->type) &&
                    !type_is_function(&p_init_expression->type))
                {
                    struct token* _Opt tk = p_init_expression ?
                        p_init_expression->first_token : ctx->current;

                    compiler_diagnostic(C_ERROR_REQUIRES_COMPILE_TIME_VALUE,
                        ctx,
                        tk,
                        NULL,
                        "requires a compile time object");

                    throw;
                }
            }

            if (is_constant && from->state == CONSTANT_VALUE_STATE_CONSTANT)
            {
                /*
                  struct point { int x; } p = { .x = 1 };
                  const int i = p.x; //not a exactly constant
                  int j = i;}
                */

                //TODO
                /*
                      struct X {int x;};
                      int main() { constexpr struct X x = (struct X){ .x = 50 };}
                */
                to->state = CONSTANT_VALUE_STATE_CONSTANT;
            }
            else
            {
                if (to->state == CONSTANT_VALUE_STATE_CONSTANT)
                {
                    //Sample int i = 1; 1 is constant but i will not be
                    to->state = CONSTANT_VALUE_EQUAL;
                }
            }
        }
    }
    catch
    {
        return 1;
    }

    return 0;
}

struct object* _Owner _Opt make_object_ptr_core(const struct type* p_type, const char* member_designator, enum target target)
{
    struct object* _Owner _Opt p_object = NULL;

    try
    {
        if (p_type->category == TYPE_CATEGORY_FUNCTION)
        {
            p_object = calloc(1, sizeof * p_object);
            if (p_object == NULL)
                throw;
            p_object->member_designator = strdup(member_designator);
            p_object->type = type_dup(p_type);
            return p_object;
        }

        if (p_type->category == TYPE_CATEGORY_POINTER)
        {
            p_object = calloc(1, sizeof * p_object);
            if (p_object == NULL)
                throw;

            *p_object = object_make_nullptr(target);
            p_object->state = CONSTANT_VALUE_STATE_UNINITIALIZED;
            assert(p_object->member_designator == NULL);
            p_object->member_designator = strdup(member_designator);

            type_destroy(&p_object->type);
            p_object->type = type_dup(p_type);

            return p_object;
        }

        if (p_type->category == TYPE_CATEGORY_ARRAY)
        {
            p_object = calloc(1, sizeof * p_object);
            if (p_object == NULL)
                throw;
            p_object->type = type_dup(p_type);
            p_object->member_designator = strdup(member_designator);

            if (p_type->num_of_elements > 0)
            {
                struct type array_item_type = get_array_item_type(p_type);

                //too big..
                const unsigned long long max_elements = p_type->num_of_elements > 1000 ? 1000 : p_type->num_of_elements;


                for (unsigned long long i = 0; i < max_elements; i++)
                {
                    char buffer[200] = { 0 };
                    snprintf(buffer, sizeof buffer, "%s[%llu]", member_designator, i);
                    struct object* _Owner _Opt p_member_obj = make_object_ptr_core(&array_item_type, buffer, target);
                    if (p_member_obj == NULL)
                    {
                        type_destroy(&array_item_type);
                        throw;
                    }
                    p_member_obj->parent = p_object;

                    free((void* _Owner)p_member_obj->member_designator);
                    p_member_obj->member_designator = strdup(buffer);
                    object_list_push(&p_object->members, p_member_obj);
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
            p_object->value_type = type_to_object_type(p_type, target);
            p_object->value.signed_int64 = -1;
            p_object->member_designator = strdup(member_designator);
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

        p_object->member_designator = strdup(member_designator);
        p_object->type = type_dup(p_type);

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
                        snprintf(buffer, sizeof buffer, "%s.%s", member_designator, p_member_declarator->declarator->name_opt->lexeme);


                        struct object* _Owner _Opt p_member_obj = make_object_ptr_core(&p_member_declarator->declarator->type, buffer, target);
                        if (p_member_obj == NULL)
                            throw;

                        p_member_obj->parent = p_object;

                        free((void* _Owner)p_member_obj->member_designator);
                        p_member_obj->member_designator = strdup(buffer);
                        object_list_push(&p_object->members, p_member_obj);
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
                    snprintf(buffer, sizeof buffer, ".%s", member_designator);


                    struct object* _Owner _Opt p_member_obj = make_object_ptr_core(&t, buffer, target);
                    if (p_member_obj == NULL)
                        throw;

                    free((void* _Owner)p_member_obj->member_designator);
                    p_member_obj->member_designator = strdup(buffer);

                    p_member_obj->parent = p_object;

                    object_list_push(&p_object->members, p_member_obj);
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

struct object* _Owner _Opt make_object_ptr(const struct type* p_type, enum target target)
{
    return make_object_ptr_core(p_type, "", target);
}

int make_object_with_member_designator(const struct type* p_type, struct object* obj, const char* name, enum target target)
{
    struct object* _Owner _Opt p = make_object_ptr_core(p_type, name, target);
    if (p)
    {
        *obj = *p;
        object_fix_parent(obj, obj);
        free(p);
        return 0;
    }
    return 1;
}

struct object object_dup(const struct object* src)
{
    assert(src->members.head == NULL);
    //assert(src->next == NULL); ??

    struct object result = *src;
    result.type = type_dup(&src->type);

    if (src->member_designator)
        result.member_designator = strdup(src->member_designator);

    result.next = NULL;

    return result;
}

int make_object(const struct type* p_type, struct object* obj, enum target target)
{
    return make_object_with_member_designator(p_type, obj, "", target);
}


enum object_value_type  type_specifier_to_object_type(const enum type_specifier_flags type_specifier_flags, enum target target)
{

    if (type_specifier_flags & TYPE_SPECIFIER_BOOL)
        return TYPE_UNSIGNED_INT8;

    if (type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        return TYPE_FLOAT32;

    if (type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {

        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
#ifdef CAKE_FLOAT128_DEFINED
            return TYPE_FLOAT128;
#else
            return TYPE_FLOAT64;
#endif
        }

        return TYPE_FLOAT64;
    }


    if (type_specifier_flags & TYPE_SPECIFIER_UNSIGNED)
    {
        if (type_specifier_flags & TYPE_SPECIFIER_CHAR)
            return TYPE_UNSIGNED_INT8;
        if (type_specifier_flags & TYPE_SPECIFIER_SHORT)
            return TYPE_UNSIGNED_INT16;

        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            switch (target)
            {
            case TARGET_X86_X64_GCC:
                return TYPE_UNSIGNED_INT64; /*check before int*/

            case TARGET_X86_MSVC:
            case TARGET_X64_MSVC:
                return TYPE_UNSIGNED_INT32; /*check before int*/

            case TARGET_LCCU16:
            case TARGET_CCU8:
                return TYPE_UNSIGNED_INT32;

            case TARGET_CATALINA:
                return TYPE_UNSIGNED_INT32;
            }
            static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
        }

        if (type_specifier_flags & TYPE_SPECIFIER_INT)
            return TYPE_UNSIGNED_INT32;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
            return TYPE_UNSIGNED_INT64;
    }
    else
    {
        if (type_specifier_flags & TYPE_SPECIFIER_CHAR)
            return TYPE_SIGNED_INT8;
        if (type_specifier_flags & TYPE_SPECIFIER_SHORT)
            return TYPE_SIGNED_INT16;

        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            switch (target)
            {
            case TARGET_X86_X64_GCC:
                return TYPE_SIGNED_INT64;

            case TARGET_X86_MSVC:
            case TARGET_X64_MSVC:
                return TYPE_SIGNED_INT32;

            case TARGET_LCCU16:
            case TARGET_CCU8:
                return TYPE_SIGNED_INT32;

            case TARGET_CATALINA:
                return TYPE_SIGNED_INT32;
            }
            static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
        }

        if (type_specifier_flags & TYPE_SPECIFIER_INT)
            return TYPE_SIGNED_INT32;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
            return TYPE_SIGNED_INT64;
    }
    return TYPE_SIGNED_INT32;
}

enum object_value_type type_to_object_type(const struct type* type, enum target target)
{
    if (type_is_pointer(type))
    {
        switch (target)
        {
        case TARGET_X86_X64_GCC:
            return TYPE_UNSIGNED_INT64;

        case TARGET_X86_MSVC:
            return TYPE_UNSIGNED_INT32;

        case TARGET_X64_MSVC:
            return TYPE_UNSIGNED_INT64;

        case TARGET_LCCU16:
        case TARGET_CCU8:
            return TYPE_UNSIGNED_INT16;

        case TARGET_CATALINA:
            return TYPE_UNSIGNED_INT32;
        }
        static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    }

    return type_specifier_to_object_type(type->type_specifier_flags, target);
}



void object_print_value_debug(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {

    case TYPE_SIGNED_INT8:

        printf("% " PRIi8 " (i8)", (int)a->value.signed_int8);
        break;


    case TYPE_UNSIGNED_INT8:
        printf("%" PRIu8 " (u8)", (int)a->value.unsigned_int8);
        break;


    case TYPE_SIGNED_INT16:
        printf("%" PRIi16 " (i16)", a->value.signed_int16);
        break;

    case TYPE_UNSIGNED_INT16:
        printf("%" PRIu16 " (u16)", a->value.unsigned_int16);
        break;

    case TYPE_SIGNED_INT32:
        printf("%" PRIi32 " (i32)", a->value.signed_int32);
        break;
    case TYPE_UNSIGNED_INT32:
        printf("%" PRIu32 " (u32)", a->value.unsigned_int32);
        break;

    case TYPE_SIGNED_INT64:
        printf("%" PRIi64 " (i64)", a->value.signed_int64);
        break;
    case TYPE_UNSIGNED_INT64:
        printf("%" PRIu64 " (u64)", a->value.unsigned_int64);
        break;
    case TYPE_FLOAT32:
        printf("%f (float)", a->value.float32);
        break;
    case TYPE_FLOAT64:
        printf("%lf (double)", a->value.float64);
        break;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        printf("%Lf (long double)", a->value.float128);
        break;
#endif
    }

}

void object_print_to_debug_core(const struct object* object, int n, enum target target)
{

    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }


    for (int i = 0; i < n; i++) printf("  ");
    if (object->member_designator)
        printf("%s ", object->member_designator);

    if (object->members.head != NULL)
    {

        type_print(&object->type, target);

        printf(" {\n");

        struct object* _Opt member = object->members.head;
        while (member)
        {
            object_print_to_debug_core(member, n + 1, target);
            member = member->next;
        }

        for (int i = 0; i < n; i++) printf("  ");
        printf("}\n");
    }
    else
    {



        type_print(&object->type, target);


        printf(" = ");

        object_print_value_debug(object);

        switch (object->state)
        {
        case CONSTANT_VALUE_STATE_UNINITIALIZED: printf(" uninitialized "); break;
        case CONSTANT_VALUE_STATE_ANY:printf(" unknown "); break;
        case CONSTANT_VALUE_EQUAL:printf(" exact "); break;
        case CONSTANT_VALUE_STATE_CONSTANT:printf(" constant_exact "); break;

        }

        printf("\n");
    }

}

void object_print_to_debug(const struct object* object, enum target target)
{
    int n = 0;
    object_print_to_debug_core(object, n, target);
}


/*
   extends the array to the max_index returning the added item.
*/
struct object* object_extend_array_to_index(const struct type* p_type, struct object* a, size_t max_index, bool is_constant, enum target target)
{
    try
    {
        for (size_t count = a->members.count; count < (max_index + 1); count++)
        {
            struct object* _Owner _Opt p = make_object_ptr(p_type, target);
            if (p == NULL)
                throw;

            char name[100] = { 0 };
            snprintf(name, sizeof name, "[%d]", count);
            free((void* _Owner)p->member_designator);
            p->member_designator = strdup(name);
            p->parent = a;
            object_default_initialization(p, is_constant);
            object_list_push(&a->members, p);
        }
    }
    catch
    {

    }

    return a->members.tail;
}


bool object_is_promoted(const struct object* a)
{
    /*
      types smaller than int are promoted to int
    */
    if ((a->value_type == TYPE_SIGNED_INT8) ||
        (a->value_type == TYPE_UNSIGNED_INT8) ||
        (a->value_type == TYPE_SIGNED_INT16) ||
        a->value_type == TYPE_UNSIGNED_INT16)
    {
        return true;
    }
    return false;
}

enum object_value_type object_common(const struct object* a, const struct object* b)
{

    enum object_value_type a_type = a->value_type;
    enum object_value_type b_type = b->value_type;

    //See 6.3.1.8 Usual arithmetic conversions


    /*
       First, if the type of either operand is _Decimal128,
       the other operand is converted to _Decimal128.
    */

    /*
      Otherwise, if the type of either operand is _Decimal64,
      the other operand is converted to _Decimal64
    */

    /*
      Otherwise, if the type of either operand is _Decimal32,
      the other operand is converted to _Decimal32.
    */

    /*
      Otherwise, if the corresponding real type of either operand is long double,
      the other operand is converted, without change of type domain, to a type whose
      corresponding real type is long double
    */
#if CAKE_FLOAT128_DEFINED
    if (a_type == TYPE_FLOAT128 || b_type == TYPE_FLOAT128)
    {
        return TYPE_FLOAT128;
    }
#endif

    /*
      Otherwise, if the corresponding real type of either operand is double,
      the other operand is converted, without change of type domain, to a type
      whose corresponding real type is double.
    */

    if (a_type == TYPE_FLOAT64 || b_type == TYPE_FLOAT64)
    {
        return TYPE_FLOAT64;
    }

    /*
      Otherwise, if the corresponding real type of either operand is float,
      the other operand is converted, without change of type domain,
      to a type whose corresponding real type is float
    */
    if (a_type == TYPE_FLOAT32 || b_type == TYPE_FLOAT32)
    {
        return TYPE_FLOAT32;
    }


    /*
     Otherwise, if any of the two types is an enumeration, it is converted to its underlying type.
    */

    /*
      Then, the integer promotions are performed on both operands.
    */


    if (object_is_promoted(a))
    {
        a_type = TYPE_SIGNED_INT32;
    }

    if (object_is_promoted(b))
    {
        b_type = TYPE_SIGNED_INT32;
    }


    /*
      Next, the following rules are applied to the promoted operands
      if both operands have the same type, then no further conversion is needed
    */
    if (a_type == b_type)
    {
        return a_type;
    }

    /*
     Otherwise, if both operands have signed integer types or both have unsigned integer
     types, the operand with the type of lesser integer conversion rank is converted to the type
     of the operand with greater rank.
    */

    if (is_signed(a_type) == is_signed(b_type))
    {
        if (get_rank(a_type) > get_rank(b_type))
        {
            return a_type;
        }

        return b_type;
    }


    /*
     Otherwise, if the operand that has unsigned integer type has rank greater or equal to
     the rank of the type of the other operand, then the operand with signed integer type is
     converted to the type of the operand with unsigned integer type.
    */


    enum object_value_type  signed_promoted = is_signed(a_type) ? a_type : b_type;
    enum object_value_type  unsigned_promoted = is_unsigned(a_type) ? a_type : b_type;


    if (get_rank(unsigned_promoted) >= get_rank(signed_promoted))
    {
        return unsigned_promoted;
    }

    /*
      Otherwise, if the type of the operand with signed integer type can represent all the values
      of the type of the operand with unsigned integer type, then the operand with unsigned
      integer type is converted to the type of the operand with signed integer type
    */

    if (get_size(signed_promoted) > get_size(unsigned_promoted))
    {
        return signed_promoted;
    }

    /*
      Otherwise, both operands are converted to the unsigned integer type corresponding to
      the type of the operand with signed integer type
    */

    return to_unsigned(signed_promoted);

}

int object_greater_than_or_equal(const struct object* a, const struct object* b)
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_value_type common_type = object_common(a, b);

    switch (common_type)
    {
    case TYPE_SIGNED_INT32:
        return object_to_signed_int(a) >= object_to_signed_int(b);

    case TYPE_UNSIGNED_INT32:
        return object_to_unsigned_int(a) >= object_to_unsigned_int(b);

    case TYPE_SIGNED_INT8:
        return object_to_signed_char(a) >= object_to_signed_char(b);

        break;
    case TYPE_UNSIGNED_INT8:
        return object_to_unsigned_char(a) >= object_to_unsigned_char(b);

    case TYPE_SIGNED_INT16:
        return object_to_signed_short(a) >= object_to_signed_short(b);

    case TYPE_UNSIGNED_INT16:
        return object_to_unsigned_short(a) >= object_to_unsigned_short(b);



    case TYPE_SIGNED_INT64:
        return object_to_signed_long_long(a) >= object_to_signed_long_long(b);

    case TYPE_UNSIGNED_INT64:
        return object_to_unsigned_long_long(a) >= object_to_unsigned_long_long(b);

    case TYPE_FLOAT32:
        return object_to_float(a) >= object_to_float(b);

    case TYPE_FLOAT64:
        return object_to_double(a) >= object_to_double(b);

#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        return object_to_long_double(a) >= object_to_long_double(b);
#endif
    }

    assert(false);
    return object_to_unsigned_long_long(a) >= object_to_unsigned_long_long(b);

}

int object_smaller_than_or_equal(const struct object* a, const struct object* b)
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_value_type common_type = object_common(a, b);

    switch (common_type)
    {
    case TYPE_SIGNED_INT32:
        return object_to_signed_int(a) <= object_to_signed_int(b);

    case TYPE_UNSIGNED_INT32:
        return object_to_unsigned_int(a) <= object_to_unsigned_int(b);


    case TYPE_SIGNED_INT8:
        return object_to_signed_char(a) <= object_to_signed_char(b);

        break;
    case TYPE_UNSIGNED_INT8:
        return object_to_unsigned_char(a) <= object_to_unsigned_char(b);

    case TYPE_SIGNED_INT16:
        return object_to_signed_short(a) <= object_to_signed_short(b);

    case TYPE_UNSIGNED_INT16:
        return object_to_unsigned_short(a) <= object_to_unsigned_short(b);



    case TYPE_SIGNED_INT64:
        return object_to_signed_long_long(a) <= object_to_signed_long_long(b);

    case TYPE_UNSIGNED_INT64:
        return object_to_unsigned_long_long(a) <= object_to_unsigned_long_long(b);

    case TYPE_FLOAT32:
        return object_to_float(a) <= object_to_float(b);

    case TYPE_FLOAT64:
        return object_to_double(a) <= object_to_double(b);
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        return object_to_long_double(a) <= object_to_long_double(b);
#endif

    }

    assert(false);
    return object_to_unsigned_long_long(a) <= object_to_unsigned_long_long(b);
}

struct object object_add(const struct object* a, const struct object* b)
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_value_type common_type = object_common(a, b);

    switch (common_type)
    {
    case TYPE_SIGNED_INT32:
        return object_make_signed_int(object_to_signed_int(a) + object_to_signed_int(b));

    case TYPE_UNSIGNED_INT32:
        return object_make_unsigned_int(object_to_unsigned_int(a) + object_to_unsigned_int(b));


        //case TYPE_SIGNED_INT8:
          //  return object_make_signed_char(object_to_signed_char(a) == object_to_signed_char(b);

          //  break;
        //case TYPE_UNSIGNED_INT8:
    //        return object_to_unsigned_char(a) == object_to_unsigned_char(b);

        //case TYPE_SIGNED_INT16:
          //  return object_to_signed_short(a) == object_to_signed_short(b);

        //case TYPE_UNSIGNED_INT16:
          //  return object_to_unsigned_short(a) == object_to_unsigned_short(b);



    case TYPE_SIGNED_INT64:
        return object_make_signed_long_long(object_to_signed_long_long(a) + object_to_signed_long_long(b));

    case TYPE_UNSIGNED_INT64:
        return object_make_unsigned_long_long(object_to_unsigned_long_long(a) + object_to_unsigned_long_long(b));

    case TYPE_FLOAT32:
        return object_make_float(object_to_float(a) + object_to_float(b));

    case TYPE_FLOAT64:
        return object_make_double(object_to_double(a) + object_to_double(b));
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        return object_make_long_double(object_to_long_double(a) + object_to_long_double(b));
#endif

    }

    assert(false);
    struct object o = { 0 };
    return o;
}


struct object object_sub(const struct object* a, const struct object* b)
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_value_type common_type = object_common(a, b);

    switch (common_type)
    {
    case TYPE_SIGNED_INT32:
        return object_make_signed_int(object_to_signed_int(a) - object_to_signed_int(b));

    case TYPE_UNSIGNED_INT32:
        return object_make_unsigned_int(object_to_unsigned_int(a) - object_to_unsigned_int(b));


        //case TYPE_SIGNED_INT8:
          //  return object_make_signed_char(object_to_signed_char(a) == object_to_signed_char(b);

          //  break;
        //case TYPE_UNSIGNED_INT8:
    //        return object_to_unsigned_char(a) == object_to_unsigned_char(b);

        //case TYPE_SIGNED_INT16:
          //  return object_to_signed_short(a) == object_to_signed_short(b);

        //case TYPE_UNSIGNED_INT16:
          //  return object_to_unsigned_short(a) == object_to_unsigned_short(b);


    case TYPE_SIGNED_INT64:
        return object_make_signed_long_long(object_to_signed_long_long(a) - object_to_signed_long_long(b));

    case TYPE_UNSIGNED_INT64:
        return object_make_unsigned_long_long(object_to_unsigned_long_long(a) - object_to_unsigned_long_long(b));

    case TYPE_FLOAT32:
        return object_make_float(object_to_float(a) - object_to_float(b));

    case TYPE_FLOAT64:
        return object_make_double(object_to_double(a) - object_to_double(b));
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        return object_make_long_double(object_to_long_double(a) - object_to_long_double(b));
#endif
    }

    assert(false);
    struct object o = { 0 };
    return o;
}


int object_equal(const struct object* a, const struct object* b)
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_value_type common_type = object_common(a, b);

    switch (common_type)
    {
    case TYPE_SIGNED_INT32:
        return object_to_signed_int(a) == object_to_signed_int(b);

    case TYPE_UNSIGNED_INT32:
        return object_to_unsigned_int(a) == object_to_unsigned_int(b);


    case TYPE_SIGNED_INT8:
        return object_to_signed_char(a) == object_to_signed_char(b);

        break;
    case TYPE_UNSIGNED_INT8:
        return object_to_unsigned_char(a) == object_to_unsigned_char(b);

    case TYPE_SIGNED_INT16:
        return object_to_signed_short(a) == object_to_signed_short(b);

    case TYPE_UNSIGNED_INT16:
        return object_to_unsigned_short(a) == object_to_unsigned_short(b);



    case TYPE_SIGNED_INT64:
        return object_to_signed_long_long(a) == object_to_signed_long_long(b);

    case TYPE_UNSIGNED_INT64:
        return object_to_unsigned_long_long(a) == object_to_unsigned_long_long(b);

    case TYPE_FLOAT32:
        return object_to_float(a) == object_to_float(b);

    case TYPE_FLOAT64:
        return object_to_double(a) == object_to_double(b);
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        return object_to_long_double(a) == object_to_long_double(b);
#endif

    }

    assert(false);
    return object_to_unsigned_long_long(a) == object_to_unsigned_long_long(b);
}


int object_not_equal(const struct object* a, const struct object* b)
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_value_type common_type = object_common(a, b);

    switch (common_type)
    {
    case TYPE_SIGNED_INT32:
        return object_to_signed_int(a) != object_to_signed_int(b);

    case TYPE_UNSIGNED_INT32:
        return object_to_unsigned_int(a) != object_to_unsigned_int(b);


    case TYPE_SIGNED_INT8:
        return object_to_signed_char(a) != object_to_signed_char(b);

        break;
    case TYPE_UNSIGNED_INT8:
        return object_to_unsigned_char(a) != object_to_unsigned_char(b);

    case TYPE_SIGNED_INT16:
        return object_to_signed_short(a) != object_to_signed_short(b);

    case TYPE_UNSIGNED_INT16:
        return object_to_unsigned_short(a) != object_to_unsigned_short(b);



    case TYPE_SIGNED_INT64:
        return object_to_signed_long_long(a) != object_to_signed_long_long(b);

    case TYPE_UNSIGNED_INT64:
        return object_to_unsigned_long_long(a) != object_to_unsigned_long_long(b);

    case TYPE_FLOAT32:
        return object_to_float(a) != object_to_float(b);

    case TYPE_FLOAT64:
        return object_to_double(a) != object_to_double(b);

#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        return object_to_long_double(a) != object_to_long_double(b);
#endif

    }

    assert(false);
    return object_to_unsigned_long_long(a) != object_to_unsigned_long_long(b);
}


#define OBJECTS_INITIAL_CAPACITY 8

void objects_destroy(struct objects* arr)
{
    free(arr->items);
}

int objects_push(struct objects* arr, struct object* obj)
{
    if (arr->items == NULL)
    {
        arr->items = malloc(OBJECTS_INITIAL_CAPACITY * sizeof(struct object*));
        if (!arr->items)
        {
            arr->size = 0;
            arr->capacity = 0;
            return ENOMEM;
        }
        arr->size = 0;
        arr->capacity = OBJECTS_INITIAL_CAPACITY;
    }
    if (arr->size == arr->capacity)
    {
        size_t new_capacity = arr->capacity ? arr->capacity * 2 : OBJECTS_INITIAL_CAPACITY;
        struct object** _Opt _Owner new_items = realloc(arr->items, new_capacity * sizeof(struct object*));
        if (!new_items) return ENOMEM;
        arr->items = new_items;
        arr->capacity = new_capacity;
    }
    arr->items[arr->size++] = obj;
    return 0;
}



void object_print_value(struct osstream* ss, const struct object* a, enum target target)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {
    case TYPE_SIGNED_INT8:
        ss_fprintf(ss, "%" PRIi8, (int)a->value.signed_int8);
        break;


    case TYPE_UNSIGNED_INT8:
        ss_fprintf(ss, "%" PRIu8, (int)a->value.unsigned_int8);
        break;


    case TYPE_SIGNED_INT16:
        ss_fprintf(ss, "%" PRIi16, a->value.signed_int16);
        break;

    case TYPE_UNSIGNED_INT16:
        ss_fprintf(ss, "%" PRIu16, a->value.unsigned_int16);
        break;

    case TYPE_SIGNED_INT32:
        ss_fprintf(ss, "%" PRIi32, a->value.signed_int32);
        break;

    case TYPE_UNSIGNED_INT32:
        ss_fprintf(ss, "%" PRIu32, a->value.unsigned_int32);
        ss_fprintf(ss, target_uintN_suffix(target, 32));
        break;

    case TYPE_SIGNED_INT64:
        ss_fprintf(ss, "%" PRIi64, a->value.signed_int64);
        ss_fprintf(ss, target_intN_suffix(target, 64));
        break;

    case TYPE_UNSIGNED_INT64:
        ss_fprintf(ss, "%" PRIu64, a->value.unsigned_int64);
        ss_fprintf(ss, target_uintN_suffix(target, 64));
        break;

    case TYPE_FLOAT32:
        ss_fprintf(ss, "%f", a->value.float32);
        ss_fprintf(ss, "f");
        break;

    case TYPE_FLOAT64:
        ss_fprintf(ss, "%lf", a->value.float64);
        break;
#ifdef CAKE_FLOAT128_DEFINED
    case TYPE_FLOAT128:
        ss_fprintf(ss, "%Lf", a->value.float64);
        ss_fprintf(ss, "L");
        break;
#endif
    }

}
