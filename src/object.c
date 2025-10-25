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
#include <math.h>

//static int get_num_of_bits(enum target target, enum object_type type);

static enum object_type to_unsigned(enum object_type t)
{
    switch (t)
    {
    case TYPE_SIGNED_CHAR: return TYPE_UNSIGNED_CHAR;
    case TYPE_SIGNED_SHORT:return TYPE_UNSIGNED_SHORT;
    case TYPE_SIGNED_INT: return TYPE_UNSIGNED_INT;
    case TYPE_SIGNED_LONG:return TYPE_UNSIGNED_LONG;
    case TYPE_SIGNED_LONG_LONG: return TYPE_UNSIGNED_LONG_LONG;
    }
    assert(false);
    return t;
}


long long object_type_get_signed_max(enum  target target, enum object_type type)
{
    int bits = get_num_of_bits(target, type);
    return (1LL << (bits - 1)) - 1; // 2^(bits-1) - 1    
}

unsigned long long object_type_get_unsigned_max(enum  target target, enum object_type type)
{
    int bits = get_num_of_bits(target, type);
    return (1ULL << bits) - 1; // 2^bits - 1
}

static bool object_type_is_signed_integer(enum object_type type)
{
    switch (type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
        return true;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        break;
    }
    return false;
}

static bool object_type_is_unsigned_integer(enum object_type type)
{
    switch (type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
        break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        return true;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        break;
    }
    return false;
}

static bool object_type_is_integer(enum object_type type)
{
    switch (type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        return true;

    }
    return false;
}


#define CAKE_CREATE_MASK(bits) ((uint64_t)((1ULL << (bits)) - 1))

// Macro to emulate casting to N-bit unsigned integer
#define CAKE_CAST_TO_N_BITS_UNSIGNED(value, bits) ((uint64_t)(((uint64_t)(value)) &  CAKE_CREATE_MASK(bits)))

// Macro to sign-extend a number based on N-bit sign bit
#define CAKE_SIGN_EXTEND(num, bits) ((int64_t)((((uint64_t)(num)) & (1ULL << ((bits) - 1))) ? \
    ((num) | ~CAKE_CREATE_MASK(bits)) : (((int64_t)(num)) &  CAKE_CREATE_MASK(bits))))

// Macro to emulate casting to N-bit signed integer
#define CAKE_CAST_TO_N_BITS_SIGNED(value, bits) CAKE_SIGN_EXTEND((int64_t)(((uint64_t)(value)) &  CAKE_CREATE_MASK(bits)), bits)



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

struct object object_make_size_t(enum target target, uint64_t value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = get_platform(target)->size_t_type;
    const unsigned long long bits = get_num_of_bits(target, r.value_type);
    r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, bits);
    return r;
}

struct object object_make_nullptr(enum target target)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = get_platform(target)->size_t_type;
    const unsigned long long bits = get_num_of_bits(target, r.value_type);
    r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(0, bits);
    return r;
}

struct object object_make_char(enum target target, int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = get_platform(target)->char_t_type;

    if (object_type_is_signed_integer(r.value_type))
    {
        r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(value, get_platform(target)->char_n_bits);
    }
    else
    {
        r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, get_platform(target)->char_n_bits);
    }

    return r;
}

struct object object_make_wchar_t(enum target target, int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = get_platform(target)->wchar_t_type;
    unsigned long long bits = get_num_of_bits(target, r.value_type);
    r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, bits);
    return r;
}

struct object object_make_bool(enum target target, bool value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = get_platform(target)->bool_type;
    if (object_type_is_signed_integer(r.value_type))
    {
        r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(value, get_platform(target)->bool_n_bits);
    }
    else
    {
        r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, get_platform(target)->bool_n_bits);
    }
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
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
        snprintf(str, n, "%lld", a->value.host_long_long);
        break;
        break;
    case TYPE_SIGNED_LONG:
        snprintf(str, n, "%lldL", a->value.host_long_long);
        break;
    case TYPE_SIGNED_LONG_LONG:
        snprintf(str, n, "%lldLL", a->value.host_long_long);
        break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
        snprintf(str, n, "%lluU", a->value.host_u_long_long);
        break;

    case TYPE_UNSIGNED_LONG:
        snprintf(str, n, "%lluUL", a->value.host_u_long_long);
        break;

    case TYPE_UNSIGNED_LONG_LONG:

        snprintf(str, n, "%lluULL", a->value.host_u_long_long);
        break;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    {
        snprintf(str, n, "%ff", a->value.host_double);
    }
    break;

    case TYPE_LONG_DOUBLE:
    {
        snprintf(str, n, "%LfLF", a->value.long_double_val);
    }

    break;
    }

    return 0;
}



bool object_is_true(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
        return a->value.host_long_long;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        return a->value.host_u_long_long;

    case TYPE_FLOAT: return a->value.host_float;
    case TYPE_DOUBLE: return a->value.host_double;
    case TYPE_LONG_DOUBLE: return a->value.long_double_val;
    }
    assert(0);
    return 0;
}

struct object object_make_signed_char(signed char value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_CHAR;
    r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(value, get_platform(TARGET_X86_MSVC)->char_n_bits);
    return r;
}

void object_increment_value(enum target target, struct object* a)
{
    switch (a->value_type)
    {

    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:

        a->value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(a->value.host_long_long + 1, get_num_of_bits(target, a->value_type));
        break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        a->value.host_u_long_long = CAKE_CAST_TO_N_BITS_SIGNED(a->value.host_u_long_long + 1, get_num_of_bits(target, a->value_type));
        break;

    case TYPE_FLOAT:
        a->value.host_float++;
        break;
    case TYPE_DOUBLE:
        a->value.host_double++;
        break;
    case TYPE_LONG_DOUBLE:
        a->value.long_double_val++;
        break;
    }
}

struct object object_make_unsigned_char(enum target target, unsigned char value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_CHAR;
    r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, get_platform(target)->char_n_bits);
    //assert(false);
    return r;
}


struct object object_make_signed_short(signed short value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_SHORT;
    r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(value, 16);
    //RTODO
    return r;
}


struct object object_make_uint8(enum target target, uint8_t value)
{
    struct object r = { 0 };
    r.value_type = to_unsigned(get_platform(target)->int8_type);
    r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, 8);
    return r;
}
struct object object_make_uint16(enum target target, uint16_t value)
{
    struct object r = { 0 };
    r.value_type = to_unsigned(get_platform(target)->int16_type);
    r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, 16);
    return r;
}
struct object object_make_uint32(enum target target, uint32_t value)
{
    struct object r = { 0 };
    r.value_type = to_unsigned(get_platform(target)->int32_type);
    r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, 32);
    return r;
}

struct object object_make_signed_int(enum  target target, long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_INT;
    r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(value, get_platform(target)->int_n_bits);
    return r;
}

struct object object_make_unsigned_int(enum target target, unsigned int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_INT;
    r.value.host_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, get_platform(target)->int_n_bits);
    return r;
}


struct object object_make_signed_long(enum target target, signed long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_INT; //RTODO
    r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(value, get_platform(target)->long_n_bits);
    return r;
}


struct object object_make_unsigned_long(enum target target, unsigned long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_LONG;
    r.value.host_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(value, get_platform(target)->long_n_bits);
    return r;
}

struct object object_make_signed_long_long(enum target target, signed long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_LONG_LONG;

    r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(value, get_platform(target)->long_long_n_bits);
    return r;
}

signed long long object_to_signed_long_long(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
        return a->value.host_long_long;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        return a->value.host_u_long_long;

    case TYPE_FLOAT: return a->value.host_float;
    case TYPE_DOUBLE: return a->value.host_double;
    case TYPE_LONG_DOUBLE: return a->value.long_double_val;
    }
    assert(0);
    return 0;
}
struct object object_make_unsigned_long_long(enum target target, unsigned long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_LONG_LONG;
    r.value.host_u_long_long = value;
    return r;
}

unsigned long long object_to_unsigned_long_long(const struct object* a)
{
    a = object_get_referenced(a);


    switch (a->value_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
        return a->value.host_long_long;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        return a->value.host_u_long_long;

    case TYPE_FLOAT: return a->value.host_float;
    case TYPE_DOUBLE: return a->value.host_double;
    case TYPE_LONG_DOUBLE: return a->value.long_double_val;
    }
    assert(0);
    return 0;
}

struct object object_make_float(float value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_FLOAT;
    r.value.host_float = value;
    return r;
}

struct object object_make_double(double value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_DOUBLE;
    r.value.host_double = value;
    return r;
}

struct object object_make_pointer(struct object* object)
{
    object = object_get_non_const_referenced(object);

    struct object r = { 0 };
    r.state = CONSTANT_VALUE_EQUAL;
    r.value_type = TYPE_UNSIGNED_LONG_LONG;
    r.value.host_u_long_long = (uint64_t)object;

    return r;
}

struct object object_make_reference(struct object* object)
{
    object = object_get_non_const_referenced(object);

    struct object r = { 0 };
    r.state = CONSTANT_VALUE_EQUAL;
    r.value_type = TYPE_UNSIGNED_LONG_LONG;
    r.value.host_u_long_long = (uint64_t)object;
    r.p_ref = object;
    return r;
}

struct object object_make_long_double(long double value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_LONG_DOUBLE;
    r.value.long_double_val = value;
    return r;
}


#pragma warning( pop )



struct object object_cast(enum target target, enum object_type dest_type, const struct object* v)
{
    v = object_get_referenced(v);

    //No changes
    if (v->value_type == dest_type)
        return *v;

    const enum object_type source_type = v->value_type;

    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = dest_type; /*dest type*/
    const int dest_n_bits = get_num_of_bits(target, dest_type);


    if (object_type_is_signed_integer(source_type))
    {
        if (object_type_is_signed_integer(dest_type))
        {
            r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(v->value.host_long_long, dest_n_bits);
        }
        else if (object_type_is_unsigned_integer(dest_type))
        {
            r.value.host_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(v->value.host_long_long, dest_n_bits);
        }
        else if (dest_type == TYPE_FLOAT)
        {
            r.value.host_float = (float)v->value.host_long_long;
        }
        else if (dest_type == TYPE_DOUBLE)
        {
            r.value.host_double = (double)v->value.host_long_long;
        }
        else if (dest_type == TYPE_LONG_DOUBLE)
        {
            r.value.long_double_val = (long double)v->value.host_long_long;
        }
        else
        {
            assert(false);
        }
    }
    else if (object_type_is_unsigned_integer(source_type))
    {
        if (object_type_is_signed_integer(dest_type))
        {
            r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED(v->value.host_u_long_long, get_num_of_bits(TARGET_X86_MSVC, dest_type));
        }
        else if (object_type_is_unsigned_integer(dest_type))
        {
            r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED(v->value.host_u_long_long, get_num_of_bits(TARGET_X86_MSVC, dest_type));
        }
        else if (dest_type == TYPE_FLOAT)
        {
            r.value.host_float = (float)v->value.host_u_long_long;
        }
        else if (dest_type == TYPE_DOUBLE)
        {
            r.value.host_double = (double)v->value.host_u_long_long;
        }
        else if (dest_type == TYPE_LONG_DOUBLE)
        {
            r.value.long_double_val = (long double)v->value.host_u_long_long;
        }
        else
        {
            assert(false);
        }
    }
    else if (source_type == TYPE_FLOAT)
    {
        if (object_type_is_signed_integer(dest_type))
        {
            r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED((long long)v->value.host_float, get_num_of_bits(TARGET_X86_MSVC, dest_type));
        }
        else if (object_type_is_unsigned_integer(dest_type))
        {
            r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED((long long)v->value.host_float, get_num_of_bits(TARGET_X86_MSVC, dest_type));
        }
        else if (dest_type == TYPE_FLOAT)
        {
            r.value.host_float = (float)v->value.host_float;
        }
        else if (dest_type == TYPE_DOUBLE)
        {
            r.value.host_double = v->value.host_float;
        }
        else if (dest_type == TYPE_LONG_DOUBLE)
        {
            r.value.long_double_val = (long double)v->value.host_float;
        }
        else
        {
            assert(false);
        }
    }
    else if (source_type == TYPE_DOUBLE)
    {
        if (object_type_is_signed_integer(dest_type))
        {
            r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED((long long)v->value.host_double, get_num_of_bits(TARGET_X86_MSVC, dest_type));
        }
        else if (object_type_is_unsigned_integer(dest_type))
        {
            r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED((long long)v->value.host_double, get_num_of_bits(TARGET_X86_MSVC, dest_type));
        }
        else if (dest_type == TYPE_FLOAT)
        {
            r.value.host_float = (float)v->value.host_double;
        }
        else if (dest_type == TYPE_DOUBLE)
        {
            r.value.host_double = (double)v->value.host_double;
        }
        else if (dest_type == TYPE_LONG_DOUBLE)
        {
            r.value.long_double_val = (long double)v->value.host_double;
        }
        else
        {
            assert(false);
        }
    }
    else if (source_type == TYPE_LONG_DOUBLE)
    {
        if (object_type_is_signed_integer(dest_type))
        {
            r.value.host_long_long = CAKE_CAST_TO_N_BITS_SIGNED((long long)v->value.long_double_val, get_num_of_bits(TARGET_X86_MSVC, dest_type));
        }
        else if (object_type_is_unsigned_integer(dest_type))
        {
            r.value.host_u_long_long = CAKE_CAST_TO_N_BITS_UNSIGNED((long long)v->value.long_double_val, get_num_of_bits(TARGET_X86_MSVC, dest_type));
        }
        else if (dest_type == TYPE_FLOAT)
        {
            r.value.host_float = (float)v->value.long_double_val;
        }
        else if (dest_type == TYPE_DOUBLE)
        {
            r.value.host_double = (double)v->value.long_double_val;
        }
        else if (dest_type == TYPE_LONG_DOUBLE)
        {
            r.value.long_double_val = (long double)v->value.long_double_val;
        }
        else
        {
            assert(false);
        }
    }
    else
    {
        assert(false);
    }

    return r;

}

void object_default_initialization(struct object* p_object, bool is_constant)
{
    if (p_object->members.head == NULL)
    {
        if (is_constant)
            p_object->state = CONSTANT_VALUE_STATE_CONSTANT;
        else
            p_object->state = CONSTANT_VALUE_EQUAL;
        p_object->value.host_u_long_long = 0;
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


int get_rank(enum object_type t)
{
    //https://cigix.me/c23#6.3.1.1
    if (t == TYPE_SIGNED_LONG_LONG ||
        t == TYPE_UNSIGNED_LONG_LONG)
    {
        return 80;
    }

    else if (t == TYPE_SIGNED_INT ||
             t == TYPE_UNSIGNED_INT)
    {
        return 40;
    }
    else if (t == TYPE_SIGNED_SHORT ||
             t == TYPE_UNSIGNED_SHORT)
    {
        return 30;
    }
    else if (t == TYPE_SIGNED_CHAR ||
             t == TYPE_UNSIGNED_CHAR)
    {
        return 20;
    }
    return 0;
}


int get_size(enum object_type t)
{
    if (t == TYPE_SIGNED_LONG_LONG ||
        t == TYPE_UNSIGNED_LONG_LONG)
    {
        return sizeof(long long);
    }

    else if (t == TYPE_SIGNED_INT ||
             t == TYPE_UNSIGNED_INT)
    {
        return sizeof(int);
    }
    else if (t == TYPE_SIGNED_SHORT ||
             t == TYPE_UNSIGNED_SHORT)
    {
        return sizeof(short);
    }
    else if (t == TYPE_SIGNED_CHAR ||
             t == TYPE_UNSIGNED_CHAR)
    {
        return sizeof(char);
    }

    return 1;
}

bool is_signed(enum object_type t)
{
    switch (t)
    {

    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:

    case TYPE_SIGNED_LONG_LONG:
    case TYPE_DOUBLE:
        return true;


    case TYPE_LONG_DOUBLE:
        return true;


    default:
        break;
    }
    return false;
}


bool is_unsigned(enum object_type t)
{
    switch (t)
    {

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
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


    switch (p_object->value_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
        return p_object->value.host_long_long == 0;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        return p_object->value.host_u_long_long == 0;

    case TYPE_FLOAT: return p_object->value.host_float == 0.0;
    case TYPE_DOUBLE: return p_object->value.host_double == 0.0;
    case TYPE_LONG_DOUBLE: return p_object->value.long_double_val == 0.0;
    }
    assert(0);
    return 0;
}


bool object_is_one(const struct object* p_object)
{
    p_object = (struct object* _Opt) object_get_referenced(p_object);

    if (!object_has_constant_value(p_object))
        return false;


    switch (p_object->value_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
        return p_object->value.host_long_long == 1;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        return p_object->value.host_u_long_long == 1;

    case TYPE_FLOAT: return p_object->value.host_float == 1.0;
    case TYPE_DOUBLE: return p_object->value.host_double == 1.0;
    case TYPE_LONG_DOUBLE: return p_object->value.long_double_val == 1.0;
    }
    assert(0);
    return 0;
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
            to->value = object_cast(ctx->options.target, to->value_type, from).value;

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
            p_object->value.host_long_long = -1;
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

enum type_specifier_flags object_type_to_type_specifier(enum object_type type)
{
    switch (type)
    {
    case TYPE_SIGNED_CHAR:
        return TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_CHAR;
    case TYPE_UNSIGNED_CHAR: return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR;

    case TYPE_SIGNED_SHORT:return  TYPE_SPECIFIER_SHORT;
    case TYPE_UNSIGNED_SHORT:return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_SHORT;

    case TYPE_SIGNED_INT:return TYPE_SPECIFIER_INT;
    case TYPE_UNSIGNED_INT:return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT;

    case TYPE_SIGNED_LONG:return TYPE_SPECIFIER_LONG;
    case TYPE_UNSIGNED_LONG:return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG;

    case TYPE_SIGNED_LONG_LONG:return TYPE_SPECIFIER_LONG_LONG;
    case TYPE_UNSIGNED_LONG_LONG:return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG_LONG;

    case TYPE_FLOAT:return TYPE_SPECIFIER_FLOAT;
    case TYPE_DOUBLE:return TYPE_SPECIFIER_DOUBLE;
    case TYPE_LONG_DOUBLE:return TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_DOUBLE;
    }
    assert(false);
    return 0;
}

enum object_type  type_specifier_to_object_type(const enum type_specifier_flags type_specifier_flags, enum target target)
{

    if (type_specifier_flags & TYPE_SPECIFIER_BOOL)
        return get_platform(target)->bool_type;

    if (type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        return TYPE_FLOAT;

    if (type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {
        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            return TYPE_LONG_DOUBLE;
        }
        return TYPE_DOUBLE;
    }


    if (type_specifier_flags & TYPE_SPECIFIER_UNSIGNED)
    {
        if (type_specifier_flags & TYPE_SPECIFIER_CHAR)
            return TYPE_UNSIGNED_CHAR;
        if (type_specifier_flags & TYPE_SPECIFIER_SHORT)
            return TYPE_UNSIGNED_SHORT;

        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
            return TYPE_UNSIGNED_LONG;

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
            return TYPE_SIGNED_LONG;
        if (type_specifier_flags & TYPE_SPECIFIER_INT)
            return TYPE_SIGNED_INT;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
            return TYPE_SIGNED_LONG_LONG;
    }
    return TYPE_SIGNED_INT;
}

enum object_type type_to_object_type(const struct type* type, enum target target)
{
    if (type_is_pointer(type))
    {
        return  get_platform(target)->size_t_type;
    }

    return type_specifier_to_object_type(type->type_specifier_flags, target);
}


void object_print_value_debug(const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {
    case TYPE_SIGNED_CHAR:
        printf("%lld (signed char)", a->value.host_long_long);
        break;


    case TYPE_UNSIGNED_CHAR:
        printf("%llu (unsigned char)", a->value.host_u_long_long);
        break;


    case TYPE_SIGNED_SHORT:
        printf("%lld (short)", a->value.host_long_long);
        break;

    case TYPE_UNSIGNED_SHORT:
        printf("%llu (unsigned short)", a->value.host_u_long_long);
        break;

    case TYPE_SIGNED_INT:
        printf("%lld (int)", a->value.host_long_long);
        break;
    case TYPE_UNSIGNED_INT:
        printf("%llu (unsigned int)", a->value.host_u_long_long);
        break;

    case TYPE_SIGNED_LONG_LONG:
        printf("%lld (long long)", a->value.host_long_long);
        break;
    case TYPE_UNSIGNED_LONG_LONG:
        printf("%llu (unsigned long long)", a->value.host_u_long_long);
        break;
    case TYPE_FLOAT:
        printf("%f (float)", a->value.host_float);
        break;

    case TYPE_DOUBLE:
        printf("%lf (double)", a->value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        printf("%Lf (long double)", a->value.long_double_val);
        break;

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
            char name[50] = { 0 };
            snprintf(name, sizeof name, "[%zu]", count);

            struct object* _Owner _Opt p = make_object_ptr_core(p_type, name, target);
            if (p == NULL)
                throw;

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
    if ((a->value_type == TYPE_SIGNED_CHAR) ||
        (a->value_type == TYPE_UNSIGNED_CHAR) ||
        (a->value_type == TYPE_SIGNED_SHORT) ||
        a->value_type == TYPE_UNSIGNED_SHORT)
    {
        return true;
    }
    return false;
}

enum object_type object_common(const struct object* a, const struct object* b)
{

    enum object_type a_type = a->value_type;
    enum object_type b_type = b->value_type;

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

    if (a_type == TYPE_LONG_DOUBLE || b_type == TYPE_LONG_DOUBLE)
    {
        return TYPE_LONG_DOUBLE;
    }


    /*
      Otherwise, if the corresponding real type of either operand is double,
      the other operand is converted, without change of type domain, to a type
      whose corresponding real type is double.
    */

    if (a_type == TYPE_DOUBLE || b_type == TYPE_DOUBLE)
    {
        return TYPE_DOUBLE;
    }

    /*
      Otherwise, if the corresponding real type of either operand is float,
      the other operand is converted, without change of type domain,
      to a type whose corresponding real type is float
    */
    if (a_type == TYPE_FLOAT || b_type == TYPE_FLOAT)
    {
        return TYPE_FLOAT;
    }


    /*
     Otherwise, if any of the two types is an enumeration, it is converted to its underlying type.
    */

    /*
      Then, the integer promotions are performed on both operands.
    */


    if (object_is_promoted(a))
    {
        a_type = TYPE_SIGNED_INT;
    }

    if (object_is_promoted(b))
    {
        b_type = TYPE_SIGNED_INT;
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


    enum object_type  signed_promoted = is_signed(a_type) ? a_type : b_type;
    enum object_type  unsigned_promoted = is_unsigned(a_type) ? a_type : b_type;


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
        arr->capacity = (int)new_capacity;
    }
    arr->items[arr->size++] = obj;
    return 0;
}



void object_print_value(struct osstream* ss, const struct object* a, enum target target)
{
    a = object_get_referenced(a);

    switch (a->value_type)
    {
    case TYPE_SIGNED_CHAR:
        ss_fprintf(ss, "%lld", a->value.host_long_long);
        break;

    case TYPE_UNSIGNED_CHAR:
        ss_fprintf(ss, "%llu", a->value.host_u_long_long);
        break;


    case TYPE_SIGNED_SHORT:
        ss_fprintf(ss, "%lld", a->value.host_long_long);
        break;

    case TYPE_UNSIGNED_SHORT:
        ss_fprintf(ss, "%llu", a->value.host_u_long_long);
        break;

    case TYPE_SIGNED_INT:
        ss_fprintf(ss, "%lld", a->value.host_long_long);
        break;
    case TYPE_SIGNED_LONG:
        ss_fprintf(ss, "%lldL", a->value.host_long_long);
        break;

    case TYPE_UNSIGNED_LONG:
        ss_fprintf(ss, "%lluL", a->value.host_u_long_long);
        break;

    case TYPE_UNSIGNED_INT:
        ss_fprintf(ss, "%llu", a->value.host_u_long_long);
        break;

    case TYPE_SIGNED_LONG_LONG:
        ss_fprintf(ss, "%lldLL", a->value.host_long_long);
        break;

    case TYPE_UNSIGNED_LONG_LONG:
        ss_fprintf(ss, "%lluULL", a->value.host_u_long_long);
        break;

    case TYPE_FLOAT:
        if (isinf(a->value.host_float))
        {
            assert(false); //TODO
            ss_fprintf(ss, "%f", a->value.host_float);
        }
        else
        {
            ss_fprintf(ss, "%f", a->value.host_float);
        }
        ss_fprintf(ss, "f");
        break;

    case TYPE_DOUBLE:
        if (isinf(a->value.host_double))
        {
            assert(false);//TODO we dont want inf to be printed.
            ss_fprintf(ss, "%.17g", a->value.host_double);
        }
        else
        {
            char temp[64] = { 0 };
            snprintf(temp, sizeof temp, "%.17g", a->value.host_double);

            /*
              This format is good but not adding . in some cases
            */
            char* p = temp;
            bool dot_found = false;

            while (*p)
            {
                if (*p == '.')
                {
                    dot_found = true;
                    break;
                }
                p++;
            }

            if (!dot_found)
            {
                *p = '.'; p++;
                *p = '0'; p++;
                *p = '\0';
            }

            ss_fprintf(ss, "%s", temp);
        }
        break;

    case TYPE_LONG_DOUBLE:
        ss_fprintf(ss, "%Lf", a->value.host_double);
        ss_fprintf(ss, "L");
        break;

    }

}



struct object object_equal(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = TYPE_SIGNED_INT;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long == b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long == b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_u_long_long = (a0.value.host_float == b0.value.host_float);
        break;
    case TYPE_DOUBLE:
        r.value.host_u_long_long = (a0.value.host_double == b0.value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        r.value.host_u_long_long = (a0.value.long_double_val == b0.value.long_double_val);
        break;
    }

    return r;
}


struct object object_not_equal(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long != b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long != b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_u_long_long = (a0.value.host_float != b0.value.host_float);
        break;
    case TYPE_DOUBLE:
        r.value.host_u_long_long = (a0.value.host_double != b0.value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        r.value.host_u_long_long = (a0.value.long_double_val != b0.value.long_double_val);
        break;
    }

    return r;
}



struct object object_greater_than_or_equal(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long >= b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long >= b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_u_long_long = (a0.value.host_float >= b0.value.host_float);
        break;
    case TYPE_DOUBLE:
        r.value.host_u_long_long = (a0.value.host_double >= b0.value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        r.value.host_u_long_long = (a0.value.long_double_val >= b0.value.long_double_val);
        break;
    }

    return r;
}


struct object object_greater_than(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long > b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long > b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_u_long_long = (a0.value.host_float > b0.value.host_float);
        break;
    case TYPE_DOUBLE:
        r.value.host_u_long_long = (a0.value.host_double > b0.value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        r.value.host_u_long_long = (a0.value.long_double_val > b0.value.long_double_val);
        break;
    }

    return r;
}

struct object object_smaller_than_or_equal(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long <= b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long <= b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_u_long_long = (a0.value.host_float <= b0.value.host_float);
        break;
    case TYPE_DOUBLE:
        r.value.host_u_long_long = (a0.value.host_double <= b0.value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        r.value.host_u_long_long = (a0.value.long_double_val <= b0.value.long_double_val);
        break;
    }

    return r;
}


struct object object_smaller_than(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long < b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long < b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_u_long_long = (a0.value.host_float < b0.value.host_float);
        break;
    case TYPE_DOUBLE:
        r.value.host_u_long_long = (a0.value.host_double < b0.value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        r.value.host_u_long_long = (a0.value.long_double_val < b0.value.long_double_val);
        break;
    }

    return r;
}


struct object object_add(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long + b0.value.host_long_long, get_num_of_bits(target, common_type));

        signed long long exact_result;
        if (signed_long_long_add(&exact_result, a0.value.host_long_long, b0.value.host_long_long))
        {
            if (r.value.host_long_long != exact_result)
            {
                snprintf(warning_message,
                        200,
                        "integer overflow results in '%lld'. The exact result is '%lld'.", r.value.host_long_long, exact_result);
            }
        }
        else
        {
            snprintf(warning_message,
                    200,
                    "integer overflow");
        }
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long + b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        unsigned long long exact_result;
        if (unsigned_long_long_add(&exact_result, a0.value.host_u_long_long, b0.value.host_u_long_long))
        {
            if (r.value.host_u_long_long != exact_result)
            {
                snprintf(warning_message,
                        200,
                        "integer wrap-around results in '%llu'. The exact result is '%llu'.", r.value.host_u_long_long, exact_result);
            }
        }
        else
        {
            snprintf(warning_message,
                    200,
                    "integer wrap-around results in '%llu'. ", r.value.host_u_long_long);
        }
        break;

    case TYPE_FLOAT:
        r.value.host_float = a0.value.host_float + b0.value.host_float;
        break;
    case TYPE_DOUBLE:
        r.value.host_double = a0.value.host_float + b0.value.host_double;
        break;

    case TYPE_LONG_DOUBLE:
        r.value.long_double_val = a0.value.long_double_val + b0.value.long_double_val;
    }

    return r;
}


struct object object_sub(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long - b0.value.host_long_long, get_num_of_bits(target, common_type));

        signed long long exact_result;
        if (signed_long_long_sub(&exact_result, a0.value.host_long_long, b0.value.host_long_long))
        {
            if (r.value.host_long_long != exact_result)
            {
                snprintf(warning_message,
                        200,
                        "integer overflow results in '%lld'. The exact result is '%lld'.", r.value.host_long_long, exact_result);
            }
        }
        else
        {
            snprintf(warning_message,
                    200,
                    "integer overflow");
        }
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long - b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        unsigned long long exact_result;
        if (unsigned_long_long_sub(&exact_result, a0.value.host_u_long_long, b0.value.host_u_long_long))
        {
            if (r.value.host_u_long_long != exact_result)
            {
                snprintf(warning_message,
                        200,
                        "integer wrap-around results in '%llu'. The exact result is '%llu'.", r.value.host_u_long_long, exact_result);
            }
        }
        else
        {
            snprintf(warning_message,
                    200,
                    "integer wrap-around results in '%llu'. ", r.value.host_u_long_long);
        }
        break;

    case TYPE_FLOAT:
        r.value.host_float = a0.value.host_float - b0.value.host_float;
        break;
    case TYPE_DOUBLE:
        r.value.host_double = a0.value.host_float - b0.value.host_double;
        break;

    case TYPE_LONG_DOUBLE:
        r.value.long_double_val = a0.value.long_double_val - b0.value.long_double_val;
    }

    return r;
}



struct object object_mul(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long * b0.value.host_long_long, get_num_of_bits(target, common_type));

        signed long long exact_result;
        if (signed_long_long_mul(&exact_result, a0.value.host_long_long, b0.value.host_long_long))
        {
            if (r.value.host_long_long != exact_result)
            {
                snprintf(warning_message,
                        200,
                        "integer overflow results in '%lld'. The exact result is '%lld'.", r.value.host_long_long, exact_result);
            }
        }
        else
        {
            snprintf(warning_message,
                    200,
                    "integer overflow");
        }
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long * b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        unsigned long long exact_result;
        if (unsigned_long_long_mul(&exact_result, a0.value.host_u_long_long, b0.value.host_u_long_long))
        {
            if (r.value.host_u_long_long != exact_result)
            {
                snprintf(warning_message,
                        200,
                        "integer wrap-around results in '%llu'. The exact result is '%llu'.", r.value.host_u_long_long, exact_result);
            }
        }
        else
        {
            snprintf(warning_message,
                    200,
                    "integer wrap-around results in '%llu'. ", r.value.host_u_long_long);
        }
        break;

    case TYPE_FLOAT:
        r.value.host_float = a0.value.host_float * b0.value.host_float;
        break;
    case TYPE_DOUBLE:
        r.value.host_double = a0.value.host_float * b0.value.host_double;
        break;

    case TYPE_LONG_DOUBLE:
        r.value.long_double_val = a0.value.long_double_val * b0.value.long_double_val;
    }

    return r;
}



struct object object_div(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        if (b0.value.host_long_long == 0)
        {
            snprintf(warning_message, 200, "division by zero");
            return r;
        }

        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long / b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        if (b0.value.host_u_long_long == 0)
        {
            snprintf(warning_message, 200, "division by zero");
            return r;
        }

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long / b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_float = a0.value.host_float / b0.value.host_float;
        break;
    case TYPE_DOUBLE:
        r.value.host_double = a0.value.host_float / b0.value.host_double;
        break;

    case TYPE_LONG_DOUBLE:
        r.value.long_double_val = a0.value.long_double_val / b0.value.long_double_val;
    }

    return r;
}


struct object object_mod(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        if (b0.value.host_long_long == 0)
        {
            snprintf(warning_message, 200, "division by zero");
            return r;
        }

        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long % b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        if (b0.value.host_u_long_long == 0)
        {
            snprintf(warning_message, 200, "division by zero");
            return r;
        }

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long % b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        assert(false);
        snprintf(warning_message, 200, " invalid operands for");
        break;
    }

    return r;
}

int object_is_equal(enum target target, const struct object* a, const struct object* b)
{
    char message[200];
    struct object r = object_equal(target, a, b, message);
    return r.value.host_long_long != 0;
}

int object_is_not_equal(enum target target, const struct object* a, const struct object* b)
{
    char message[200];
    struct object r = object_not_equal(target, a, b, message);
    return r.value.host_long_long != 0;
}

int object_is_greater_than_or_equal(enum target target, const struct object* a, const struct object* b)
{
    char message[200];
    struct object r = object_greater_than_or_equal(target, a, b, message);
    return r.value.host_long_long != 0;
}

int object_is_smaller_than_or_equal(enum target target, const struct object* a, const struct object* b)
{
    char message[200];
    struct object r = object_smaller_than_or_equal(target, a, b, message);
    return r.value.host_long_long != 0;
}


struct object object_logical_not(enum target target, const struct object* a, char warning_message[200])
{
    a = object_get_referenced(a);

    struct object r = { 0 };
    r.value_type = TYPE_SIGNED_INT;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    enum object_type common_type = a->value_type;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(!a->value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(!a->value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_u_long_long = (!a->value.host_float);
        break;
    case TYPE_DOUBLE:
        r.value.host_u_long_long = (!a->value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        r.value.host_u_long_long = (!a->value.long_double_val);
        break;
    }

    return r;
}


struct object object_bitwise_not(enum target target, const struct object* a, char warning_message[200])
{
    a = object_get_referenced(a);

    struct object r = { 0 };

    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    enum object_type common_type = a->value_type;
    r.value_type = common_type;
    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(~a->value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(~a->value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        snprintf(warning_message, 200, "invalid operand");
        break;
    }

    return r;
}

struct object object_unary_minus(enum target target, const struct object* a, char warning_message[200])
{
    a = object_get_referenced(a);

    struct object r = { 0 };

    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    enum object_type common_type = a->value_type;
    r.value_type = common_type;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(-(a->value.host_long_long), get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(-(a->value.host_u_long_long), get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_float = -(a->value.host_float);
        break;
    case TYPE_DOUBLE:
        r.value.host_double = -(a->value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        r.value.long_double_val = -(a->value.long_double_val);
        break;
    }

    return r;
}

struct object object_unary_plus(enum target target, const struct object* a, char warning_message[200])
{
    /*
       char  c = -5;
       int   i = +c; //it just perform integer promotion
     */
    a = object_get_referenced(a);

    struct object r = { 0 };

    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    enum object_type common_type = a->value_type;
    r.value_type = common_type;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(+(a->value.host_long_long), get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(+(a->value.host_u_long_long), get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
        r.value.host_float = +(a->value.host_float);
        break;
    case TYPE_DOUBLE:
        r.value.host_double = +(a->value.host_double);
        break;

    case TYPE_LONG_DOUBLE:
        r.value.long_double_val = +(a->value.long_double_val);
        break;
    }

    return r;
}


struct object object_bitwise_xor(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long ^ b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long ^ b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        assert(false);
        snprintf(warning_message, 200, " invalid operands");
        break;
    }

    return r;
}

struct object object_bitwise_or(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long | b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long | b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        assert(false);
        snprintf(warning_message, 200, " invalid operands");
        break;
    }

    return r;
}


struct object object_bitwise_and(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long & b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long & b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        assert(false);
        snprintf(warning_message, 200, " invalid operands");
        break;
    }

    return r;
}

struct object object_shift_left(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long << b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long << b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        assert(false);
        snprintf(warning_message, 200, " invalid operands");
        break;
    }

    return r;
}


struct object object_shift_right(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    enum object_type common_type = object_common(a, b);
    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, common_type, b);
    struct object r = { 0 };
    r.value_type = common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    switch (common_type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_SIGNED_SHORT:
    case TYPE_SIGNED_INT:
    case TYPE_SIGNED_LONG:
    case TYPE_SIGNED_LONG_LONG:
    {
        r.value.host_long_long =
            CAKE_CAST_TO_N_BITS_SIGNED(a0.value.host_long_long >> b0.value.host_long_long, get_num_of_bits(target, common_type));
    }
    break;

    case TYPE_UNSIGNED_CHAR:
    case TYPE_UNSIGNED_SHORT:
    case TYPE_UNSIGNED_INT:
    case TYPE_UNSIGNED_LONG:
    case TYPE_UNSIGNED_LONG_LONG:

        r.value.host_u_long_long =
            CAKE_CAST_TO_N_BITS_UNSIGNED(a0.value.host_u_long_long >> b0.value.host_u_long_long, get_num_of_bits(target, common_type));

        break;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        assert(false);
        snprintf(warning_message, 200, " invalid operands");
        break;
    }

    return r;
}

