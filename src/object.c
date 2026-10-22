/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable


#include "cake_compat.h"
#include <stdlib.h>
#include "object.h"
#include <limits.h>
#include <stdio.h>
#include <assert.h>
#include "type.h"
#include "error.h"
#include "parser.h"

#include <string.h>
#include <math.h>
#include "fp_to_string.h"
#include "wide_integer.h"

bool object_type_is_unsigned_bitfield(enum object_type t)
{
    return t >= TYPE_UNSIGNED_BITFIELD_1 && t <= TYPE_UNSIGNED_BITFIELD_128;
}

bool object_type_is_signed_bitfield(enum object_type t)
{
    return t >= TYPE_SIGNED_BITFIELD_1 && t <= TYPE_SIGNED_BITFIELD_128;
}

bool object_type_is_bitfield(enum object_type t)
{
    return object_type_is_unsigned_bitfield(t) || object_type_is_signed_bitfield(t);
}

int object_type_bitfield_width(enum object_type t)
{
    if (object_type_is_unsigned_bitfield(t))
        return (int)(t - TYPE_UNSIGNED_BITFIELD_1 + 1);

    if (object_type_is_signed_bitfield(t))
        return (int)(t - TYPE_SIGNED_BITFIELD_1 + 1);

    assert(false);
    return 0;
}

bool object_type_is_unsigned_bitint(enum object_type t)
{
    return t >= TYPE_UNSIGNED_BITINT_1 && t <= TYPE_UNSIGNED_BITINT_128;
}

bool object_type_is_signed_bitint(enum object_type t)
{
    return t >= TYPE_SIGNED_BITINT_1 && t <= TYPE_SIGNED_BITINT_128;
}

bool object_type_is_bitint(enum object_type t)
{
    return object_type_is_unsigned_bitint(t) || object_type_is_signed_bitint(t);
}

int object_type_bitint_width(enum object_type t)
{
    if (object_type_is_unsigned_bitint(t))
        return (int)(t - TYPE_UNSIGNED_BITINT_1 + 1);

    if (object_type_is_signed_bitint(t))
        return (int)(t - TYPE_SIGNED_BITINT_1 + 1);

    assert(false);
    return 0;
}

/*
  "bitsized" = bitfield, bit-precise integer or long long long. All keep an
  N-bit value in the host long long, so every value-level operation treats
  them alike; only promotion and rank tell them apart. The width of
  long long long comes from the target (target_get_num_of_bits).
*/
static bool object_type_is_signed_bitsized(enum object_type t)
{
    return object_type_is_signed_bitfield(t) ||
        object_type_is_signed_bitint(t) ||
        t == TYPE_SIGNED_LONG_LONG_LONG;
}

static bool object_type_is_unsigned_bitsized(enum object_type t)
{
    return object_type_is_unsigned_bitfield(t) ||
        object_type_is_unsigned_bitint(t) ||
        t == TYPE_UNSIGNED_LONG_LONG_LONG;
}

static enum object_type bitfield_to_unsigned(enum object_type t)
{
    if (t == TYPE_SIGNED_LONG_LONG_LONG)
    {
        return TYPE_UNSIGNED_LONG_LONG_LONG;
    }
    if (object_type_is_signed_bitfield(t))
    {
        int width = object_type_bitfield_width(t);
        return (enum object_type)(TYPE_UNSIGNED_BITFIELD_1 + width - 1);
    }
    if (object_type_is_signed_bitint(t))
    {
        int width = object_type_bitint_width(t);
        return (enum object_type)(TYPE_UNSIGNED_BITINT_1 + width - 1);
    }
    return t; /* already unsigned */
}


static long double resize_floating_point(long double value, int bits)
{
    switch (bits)
    {
    case 64:
        //STATIC_ASSERT(sizeof(double) == 8);
        return (double)value;
    case 32:
        //STATIC_ASSERT(sizeof(float) == 4);
        return (float)value;
    }
    return value;
}

static enum object_type to_unsigned(enum object_type t)
{
    if (object_type_is_signed_bitsized(t))
        return bitfield_to_unsigned(t);

    if (object_type_is_unsigned_bitsized(t))
        return t;

    switch (t)
    {
    case TYPE_SIGNED_CHAR: return TYPE_UNSIGNED_CHAR;
    case TYPE_SIGNED_SHORT:return TYPE_UNSIGNED_SHORT;
    case TYPE_SIGNED_INT: return TYPE_UNSIGNED_INT;
    case TYPE_SIGNED_LONG:return TYPE_UNSIGNED_LONG;
    case TYPE_SIGNED_LONG_LONG: return TYPE_UNSIGNED_LONG_LONG;
    case TYPE_SIGNED_LONG_LONG_LONG: return TYPE_UNSIGNED_LONG_LONG_LONG;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        return t;

    default:
        break;
    }

    return t;
}

bool object_type_is_signed_integer(enum object_type type)
{
    if (object_type_is_signed_bitsized(type))
        return true;

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

    default:
        break;
    }
    return false;
}

bool object_type_is_unsigned_integer(enum object_type type)
{
    if (object_type_is_unsigned_bitsized(type))
        return true;

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

    default:
        break;
    }
    return false;
}

/*
  Every integer keeps its value in value.host_wide_integer, wrapped to the width of
  its type (target_get_num_of_bits): sign extended when the type is signed,
  zero extended when it is unsigned. So one wide_integer computation followed by a
  wrap gives the result of any integer type, up to long long long.
*/
static bool object_type_is_integer(enum object_type t)
{
    return object_type_is_signed_integer(t) || object_type_is_unsigned_integer(t);
}

/* wraps v to the width of the integer type of r and stores it */
/*
  v as an integer of 'width' bits. A width of 0 (a zeroed platform, as some
  self tests use) keeps the value as it is.
*/
static struct wide_integer object_wrap_s(struct wide_integer v, int width)
{
    return width >= 1 && width <= WIDE_INTEGER_BITS ? wide_integer_wrap_s(v, width) : v;
}

static struct wide_integer object_wrap_u(struct wide_integer v, int width)
{
    return width >= 1 && width <= WIDE_INTEGER_BITS ? wide_integer_wrap_u(v, width) : v;
}

static void object_set_wide_integer(const struct platform* target, struct object* r, struct wide_integer v)
{
    const int width = (int)target_get_num_of_bits(target, r->value_type);
    r->value.host_wide_integer = object_type_is_signed_integer(r->value_type) ? object_wrap_s(v, width) : object_wrap_u(v, width);
}

/* the value of a, any arithmetic type, as a wide_integer; a floating value is truncated */
static struct wide_integer object_get_wide_integer(const struct object* a)
{
    if (object_type_is_integer(a->value_type))
        return a->value.host_wide_integer;

    return wide_integer_from_long_double(a->value.host_long_double);
}

/* the value of a, any arithmetic type, as a long double */
static long double object_get_long_double(const struct object* a)
{
    if (object_type_is_signed_integer(a->value_type))
        return wide_integer_to_long_double_s(a->value.host_wide_integer);

    if (object_type_is_unsigned_integer(a->value_type))
        return wide_integer_to_long_double_u(a->value.host_wide_integer);

    return a->value.host_long_double;
}

/* the integer of the given width that has value */
static struct wide_integer wide_integer_signed_of_width(long long value, int width)
{
    return object_wrap_s(wide_integer_from_signed(value), width);
}

static struct wide_integer wide_integer_unsigned_of_width(unsigned long long value, int width)
{
    return object_wrap_u(wide_integer_from_unsigned(value), width);
}

NODISCARD
bool unsigned_long_long_mul(_Out unsigned long long* result, unsigned long long a, unsigned long long b)
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

void object_list_push(struct object_list* list, struct object* _Owner pnew)
{
    assert(pnew->next == NULL);

    if (list->head == NULL)
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        assert(list->tail != NULL && list->tail->next == NULL);
        list->tail->next = pnew; //lint 26 33
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
    free((void* _Owner _Opt)p->member_designator);

    struct object* _Owner _Opt item = p->members.head;
    while (item)
    {
        struct object* _Owner _Opt next = item->next;
        item->next = NULL;
        object_delete(item);
        item = next;
    }
} //lint 29 assert line 484

void object_delete(struct object* _Opt _Owner p)
{
    if (p)
    {
        object_destroy(p);
        free(p);
    }
}

bool object_has_all_members_constants(const struct object* object)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }

    if (object->members.head != NULL)
    {
        const struct object* _Opt member = object->members.head;
        while (member)
        {
            if (!object_has_all_members_constants(member))
                return false;
            member = member->next;
        }
        return true; /* all members passed */
    }

    return object_has_constant_value(object);
}

bool object_has_constant_value(const struct object* a)
{
    a = object_get_referenced(a);
    return a->state == CONSTANT_VALUE_STATE_CONSTANT;
}

bool object_has_known_value(const struct object* a)
{
    a = object_get_referenced(a);
    return a->state == CONSTANT_VALUE_STATE_CONSTANT ||
        a->state == CONSTANT_VALUE_EQUAL;
}

struct object object_make_size_t(const struct platform* target, unsigned long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = target->size_t_type;
    const int bits = target_get_num_of_bits(target, r.value_type);
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, bits);
    return r;
}

struct object object_make_nullptr(const struct platform* target)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = target->size_t_type;
    const int bits = target_get_num_of_bits(target, r.value_type);
    r.value.host_wide_integer = wide_integer_unsigned_of_width(0, bits);
    return r;
}

struct object object_make_char(const struct platform* target, int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = target->char_t_type;

    if (object_type_is_signed_integer(r.value_type))
    {
        r.value.host_wide_integer = wide_integer_signed_of_width(value, target->char_n_bits);
    }
    else
    {
        r.value.host_wide_integer = wide_integer_unsigned_of_width(value, target->char_n_bits);
    }

    return r;
}

struct object object_make_wchar_t(const struct platform* target, int value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = target->wchar_t_type;
    const int bits = target_get_num_of_bits(target, r.value_type);
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, bits);
    return r;
}

struct object object_make_bool(const struct platform* target, bool value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = target->bool_type;
    if (object_type_is_signed_integer(r.value_type))
    {
        r.value.host_wide_integer = wide_integer_signed_of_width(value, target->bool_n_bits);
    }
    else
    {
        r.value.host_wide_integer = wide_integer_unsigned_of_width(value, target->bool_n_bits);
    }
    return r;
}

int object_to_str(const struct object* a, int n, char str[ /*n*/])
{
    str[0] = '\0';

    a = object_get_referenced(a);

    if (object_type_is_integer(a->value_type))
    {
        char digits[WIDE_INTEGER_TO_STRING_MIN_SIZE] = { 0 };
        if (object_type_is_signed_integer(a->value_type))
            wide_integer_to_string_s(a->value.host_wide_integer, digits, sizeof digits);
        else
            wide_integer_to_string_u(a->value.host_wide_integer, digits, sizeof digits);

        /* the suffix of the standard types; long long long is written like long long */
        const char* suffix = "";
        switch (a->value_type)
        {
        case TYPE_SIGNED_LONG: suffix = "L"; break;
        case TYPE_SIGNED_LONG_LONG: suffix = "LL"; break;
        case TYPE_SIGNED_LONG_LONG_LONG: suffix = "LL"; break;
        case TYPE_UNSIGNED_CHAR: suffix = "U"; break;
        case TYPE_UNSIGNED_SHORT: suffix = "U"; break;
        case TYPE_UNSIGNED_INT: suffix = "U"; break;
        case TYPE_UNSIGNED_LONG: suffix = "UL"; break;
        case TYPE_UNSIGNED_LONG_LONG: suffix = "ULL"; break;
        case TYPE_UNSIGNED_LONG_LONG_LONG: suffix = "ULL"; break;
        default:
            break;
        }

        snprintf(str, n, "%s%s", digits, suffix);
        return 0;
    }

    switch (a->value_type)
    {
    case TYPE_FLOAT:
        snprintf(str, n, "%Lff", a->value.host_long_double);
        break;
    case TYPE_DOUBLE:
        snprintf(str, n, "%Lf", a->value.host_long_double);
        break;

    case TYPE_LONG_DOUBLE:
        snprintf(str, n, "%LfLF", a->value.host_long_double);
        break;

    default:
        break;
    }

    return 0;
}

bool object_is_true(const struct object* a)
{
    a = object_get_referenced(a);

    if (object_type_is_integer(a->value_type))
        return !wide_integer_is_zero(a->value.host_wide_integer);

    return a->value.host_long_double != 0;
}

struct object object_make_signed_char(signed char value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_CHAR;
    r.value.host_wide_integer = wide_integer_signed_of_width(value, 8);
    return r;
}

bool object_increment_value(const struct platform* target, struct object* a)
{
    if (object_type_is_integer(a->value_type))
    {
        const struct wide_integer prev = a->value.host_wide_integer;
        object_set_wide_integer(target, a, wide_integer_add(prev, wide_integer_from_unsigned(1)));

        /* true when it wrapped around */
        return object_type_is_signed_integer(a->value_type) ?
            wide_integer_compare_s(prev, a->value.host_wide_integer) > 0 :
            wide_integer_compare_u(prev, a->value.host_wide_integer) > 0;
    }

    a->value.host_long_double++;
    a->value.host_long_double = resize_floating_point(a->value.host_long_double, target_get_num_of_bits(target, a->value_type));
    return false;
}

struct object object_make_unsigned_char(const struct platform* target, unsigned char value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_CHAR;
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, target->char_n_bits);

    return r;
}

struct object object_make_signed_short(signed short value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_SHORT;
    r.value.host_wide_integer = wide_integer_signed_of_width(value, 16);

    return r;
}

struct object object_make_uint8(const struct platform* target, uint8_t value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = to_unsigned(target->int8_type);
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, 8);
    return r;
}
struct object object_make_uint16(const struct platform* target, uint16_t value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = to_unsigned(target->int16_type);
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, 16);
    return r;
}
struct object object_make_uint32(const struct platform* target, uint32_t value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = to_unsigned(target->int32_type);
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, 32);
    return r;
}

struct object object_make_signed_int(const struct platform* target, long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_INT;
    r.value.host_wide_integer = wide_integer_signed_of_width(value, target->int_n_bits);
    return r;
}

struct object object_make_unsigned_int(const struct platform* target, unsigned long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_INT;
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, target->int_n_bits);
    return r;
}

struct object object_make_signed_long(const struct platform* target, signed long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_LONG;
    r.value.host_wide_integer = wide_integer_signed_of_width(value, target->long_n_bits);
    return r;
}

struct object object_make_unsigned_long(const struct platform* target, unsigned long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_LONG;
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, target->long_n_bits);
    return r;
}

struct object object_make_signed_long_long(const struct platform* target, signed long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_SIGNED_LONG_LONG;

    r.value.host_wide_integer = wide_integer_signed_of_width(value, target->long_long_n_bits);
    return r;
}

signed long long object_to_signed_long_long(const struct object* a)
{
    a = object_get_referenced(a);

    /* the low half; a value wider than a long long is truncated */
    if (object_type_is_integer(a->value_type))
        return (signed long long)a->value.host_wide_integer.lo;

    return (long long)a->value.host_long_double;
}
struct object object_make_unsigned_long_long( unsigned long long value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_UNSIGNED_LONG_LONG;
    r.value.host_wide_integer = wide_integer_from_unsigned(value);
    return r;
}

unsigned long long object_to_unsigned_long_long(const struct object* a)
{
    a = object_get_referenced(a);

    /* the low half; a value wider than a long long is truncated */
    if (object_type_is_integer(a->value_type))
        return a->value.host_wide_integer.lo;

    return (unsigned long long) a->value.host_long_double;
}

struct object object_make_float(const struct platform* target, long double value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_FLOAT;
    r.value.host_long_double = resize_floating_point(value, target_get_num_of_bits(target, TYPE_FLOAT));
    return r;
}

struct object object_make_double(const struct platform* target, long double value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_DOUBLE;
    r.value.host_long_double = resize_floating_point(value, target_get_num_of_bits(target, TYPE_DOUBLE));
    return r;
}

struct object object_make_long_double(const struct platform* target, long double value)
{
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = TYPE_LONG_DOUBLE;
    r.value.host_long_double = resize_floating_point(value, target_get_num_of_bits(target, TYPE_LONG_DOUBLE));
    return r;
}

struct object object_make_signed_bitfield(int width, long long value)
{
    assert(width >= 1 && width <= 128);
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = (enum object_type)(TYPE_SIGNED_BITFIELD_1 + width - 1);
    r.value.host_wide_integer = wide_integer_signed_of_width(value, width);
    return r;
}

struct object object_make_unsigned_bitfield(int width, unsigned long long value)
{
    assert(width >= 1 && width <= 128);
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = (enum object_type)(TYPE_UNSIGNED_BITFIELD_1 + width - 1);
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, width);
    return r;
}

struct object object_make_signed_bitint(int width, long long value)
{
    assert(width >= 2 && width <= WIDE_INTEGER_BITS);
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = (enum object_type)(TYPE_SIGNED_BITINT_1 + width - 1);
    r.value.host_wide_integer = wide_integer_signed_of_width(value, width);
    return r;
}

struct object object_make_unsigned_bitint(int width, unsigned long long value)
{
    assert(width >= 1 && width <= WIDE_INTEGER_BITS);
    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = (enum object_type)(TYPE_UNSIGNED_BITINT_1 + width - 1);
    r.value.host_wide_integer = wide_integer_unsigned_of_width(value, width);
    return r;
}

struct object object_make_pointer(struct object* object)
{
    object = object_get_non_const_referenced(object);

    struct object r = { 0 };
    r.state = CONSTANT_VALUE_EQUAL;
    r.value_type = TYPE_UNSIGNED_LONG_LONG;
    r.value.host_wide_integer = wide_integer_from_unsigned((uint64_t)object);

    return r;
}

struct object object_make_reference(struct object* object)
{
    object = object_get_non_const_referenced(object);

    struct object r = { 0 };
    r.state = CONSTANT_VALUE_EQUAL;
    r.value_type = TYPE_UNSIGNED_LONG_LONG;
    r.value.host_wide_integer = wide_integer_from_unsigned((uint64_t)object);
    r.p_ref = object;
    return r;
}

struct object object_cast(const struct platform* target, enum object_type dest_type, const struct object* v)
{
    v = object_get_referenced(v);

    //No changes
    if (v->value_type == dest_type)
        return object_dup(v);

    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = dest_type; /*dest type*/

    if (object_type_is_integer(dest_type))
    {
        /* the value wrapped to the destination; a floating one is truncated first */
        object_set_wide_integer(target, &r, object_get_wide_integer(v));
    }
    else
    {
        r.value.host_long_double = resize_floating_point(object_get_long_double(v), target_get_num_of_bits(target, dest_type));
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
        p_object->value.host_wide_integer.lo = 0;
        p_object->value.host_wide_integer.hi = 0;
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

static int get_rank(enum object_type t)
{
    //https://cigix.me/c23#6.3.1.1

    if (object_type_is_bitfield(t))
    {
        //TODO promote to long?
        /* Bitfields promote to int (rank 40) */
        return 40;
    }

    /*
      Bit-precise integers rank below any standard type with at least as many
      bits and above the ones with fewer, and among themselves by width. Ranks
      of standard types are spaced by 10, and the widths that can end up here
      are limited by object_make_*_bitint, so 'standard rank - 10 + width/16'
      keeps the ordering without touching the standard values.
    */
    if (object_type_is_bitint(t))
    {
        const int width = object_type_bitint_width(t);
        if (width > 64)
        {
            return 90 - 10 + width / 16; /* between long long and long long long */
        }
        if (width > 32)
        {
            return 80 - 10 + width / 16; /* between long and long long */
        }
        if (width > 16)
        {
            return 40 - 10 + width / 16; /* between short and int */
        }
        if (width > 8)
        {
            return 30 - 10 + width / 16; /* between char and short */
        }
        return 20 - 10 + width / 16; /* below char */
    }

    if (t == TYPE_SIGNED_LONG_LONG_LONG ||
        t == TYPE_UNSIGNED_LONG_LONG_LONG)
    {
        return 90;
    }
    else if (t == TYPE_SIGNED_LONG_LONG ||
        t == TYPE_UNSIGNED_LONG_LONG)
    {
        return 80;
    }
    else if (t == TYPE_SIGNED_LONG ||
        t == TYPE_UNSIGNED_LONG)
    {
        return 60;
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

int target_sizeof(const struct platform* target, enum object_type t)
{
    return target_get_num_of_bits(target, t) / 8;
}

bool is_signed(enum object_type t)
{
    if (object_type_is_signed_bitsized(t)) return true;

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
    if (object_type_is_unsigned_bitsized(t)) return true;

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
    p_object = object_get_referenced(p_object);

    if (!object_has_constant_value(p_object))
        return false;

    if (object_type_is_integer(p_object->value_type))
        return wide_integer_is_zero(p_object->value.host_wide_integer);

    return p_object->value.host_long_double == 0.0;
}

bool object_is_one(const struct object* p_object)
{
    p_object = object_get_referenced(p_object);

    if (!object_has_constant_value(p_object))
        return false;

    if (object_type_is_integer(p_object->value_type))
        return wide_integer_equal(p_object->value.host_wide_integer, wide_integer_from_unsigned(1));

    return p_object->value.host_long_double == 1.0;
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

const struct object* _Opt object_get_member_const(const struct object* p_object, size_t index)
{
    p_object = object_get_referenced(p_object);

    if (p_object->members.head == NULL)
        return NULL; //tODO

    const struct object* _Opt it = p_object->members.head;
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

struct object* _Opt object_get_member(const struct object* p_object, size_t index)
{
    return (struct object* _Opt)object_get_member_const(p_object, index);
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
            const struct object* _Opt it_from = from->members.head;

            while (it_from && it_to)
            {
                if (object_set(ctx, it_to, NULL, it_from, is_constant, requires_constant_initialization) != 0)
                    throw;
                it_to = it_to->next;
                it_from = it_from->next;
            }
        }
        else
        {
            assert(to->members.head == NULL);

            to->state = from->state;

            {
                struct object temp = object_cast(&ctx->options.platform, to->value_type, from);
                to->value = temp.value;
                object_destroy(&temp);
            }

            if (from->state == CONSTANT_VALUE_STATE_CONSTANT &&
                (object_type_is_signed_integer(from->value_type) || object_type_is_unsigned_integer(from->value_type)) &&
                (object_type_is_signed_integer(to->value_type) || object_type_is_unsigned_integer(to->value_type)))
            {
                bool representable = true;

                /*
                  compare the source's mathematical value against the exact
                  range of the destination type (rather than round-tripping
                  through a wrapping cast, which is not reliable: e.g. a
                  same-width signed<->unsigned cast is a bijection, so a
                  wrapped-around value can cast back to the original bit
                  pattern even though the value was not representable)
                */
                const int dest_n_bits = target_get_num_of_bits(&ctx->options.platform, to->value_type);
                const bool dest_is_signed = object_type_is_signed_integer(to->value_type);
                const struct wide_integer v = from->value.host_wide_integer;

                /* the top bit of an unsigned value of the full wide_integer width is not a sign */
                const bool v_is_negative = object_type_is_signed_integer(from->value_type) && wide_integer_is_negative(v);
                const bool v_uses_top_bit = !object_type_is_signed_integer(from->value_type) && wide_integer_is_negative(v);

                if (type_is_bool(&to->type))
                    representable = !v_is_negative && wide_integer_compare_u(v, wide_integer_from_unsigned(1)) <= 0;
                else if (v_is_negative)
                    representable = dest_is_signed && wide_integer_equal(object_wrap_s(v, dest_n_bits), v);
                else if (v_uses_top_bit)
                    representable = !dest_is_signed && dest_n_bits == WIDE_INTEGER_BITS;
                else
                    /* v is in the range when wrapping it to the destination keeps it */
                    representable = wide_integer_equal(dest_is_signed ? object_wrap_s(v, dest_n_bits) : object_wrap_u(v, dest_n_bits), v);

                if (!representable && p_init_expression)
                {
                    const enum diagnostic_id id =
                        to->type.storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR ?
                        C_ERROR_CONSTANT_VALUE_NOT_REPRESENTABLE :
                        W_CONSTANT_VALUE_NOT_REPRESENTABLE;

                    {
                        char value_buf[64] = { 0 };
                        object_to_str(from, sizeof value_buf, value_buf);
                        struct osstream ss = { 0 };
                        print_type_no_names(&ss, &to->type, &ctx->options.platform);
                        if (type_is_bitfield(&to->type))
                            ss_fprintf(&ss, " : %d", type_get_bitfield_width(&to->type));
                        const struct token* p_first = p_init_expression->first_token;
                        size_t digits = strlen(value_buf);
                        while (digits > 0 && (value_buf[digits - 1] == 'U' || value_buf[digits - 1] == 'L'))
                            digits--;
                        const bool same_as_written =
                            strlen(p_first->lexeme) == digits && strncmp(p_first->lexeme, value_buf, digits) == 0;
                        if (p_first == p_init_expression->last_token && !same_as_written)
                            diagnostic(id, ctx, p_first, NULL,
                                       "constant expression %s (value %s) is not exactly representable in '%s'", p_first->lexeme, value_buf, ss.c_str ? ss.c_str : "");
                        else
                            diagnostic(id, ctx, p_init_expression->first_token, NULL,
                                       "constant expression %s is not exactly representable in '%s'", same_as_written ? p_first->lexeme : value_buf, ss.c_str ? ss.c_str : "");
                        ss_close(&ss);
                    }
                }
            }
            else if (from->state == CONSTANT_VALUE_STATE_CONSTANT &&
                (from->value_type == TYPE_FLOAT || from->value_type == TYPE_DOUBLE || from->value_type == TYPE_LONG_DOUBLE) &&
                (to->value_type == TYPE_FLOAT || to->value_type == TYPE_DOUBLE || to->value_type == TYPE_LONG_DOUBLE))
            {
                const int dest_n_bits = target_get_num_of_bits(&ctx->options.platform, to->value_type);
                const long double narrowed = resize_floating_point(from->value.host_long_double, dest_n_bits);

                /* NaN compares unequal to itself, but converting it keeps it a NaN */
                if (!isnan(from->value.host_long_double) &&
                    narrowed != from->value.host_long_double && p_init_expression)
                {
                    const enum diagnostic_id id =
                        to->type.storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR ?
                        C_ERROR_CONSTANT_VALUE_NOT_REPRESENTABLE :
                        W_CONSTANT_VALUE_NOT_REPRESENTABLE;

                    char exact_buf[64] = { 0 };
                    char rounded_buf[64] = { 0 };
                    const char* dest_type_name =
                        to->value_type == TYPE_FLOAT ? "float" :
                        to->value_type == TYPE_DOUBLE ? "double" : "long double";

                    snprintf(exact_buf, sizeof exact_buf, "%.17g", (double)from->value.host_long_double);

                    if (to->value_type == TYPE_FLOAT)
                        snprintf(rounded_buf, sizeof rounded_buf, "%.9g", (double)(float)narrowed);
                    else
                        snprintf(rounded_buf, sizeof rounded_buf, "%.17g", (double)narrowed);

                    diagnostic(id,
                        ctx,
                        p_init_expression->first_token,
                        NULL,
                        "constant expression %s is not exactly representable in '%s'; nearest '%s' value is %s",
                        exact_buf,
                        dest_type_name,
                        dest_type_name,
                        rounded_buf);
                }
            }
            else if (from->state == CONSTANT_VALUE_STATE_CONSTANT &&
                is_constant &&
                (from->value_type == TYPE_FLOAT || from->value_type == TYPE_DOUBLE || from->value_type == TYPE_LONG_DOUBLE) &&
                (object_type_is_signed_integer(to->value_type) || object_type_is_unsigned_integer(to->value_type)) &&
                p_init_expression)
            {
                /*
                  constexpr int i = 1.5;  error, value changes when converted
                  const int k = 1.5;      warning
                  constexpr int j = 2.0;  OK
                */
                const long double v = from->value.host_long_double;
                const int dest_n_bits = target_get_num_of_bits(&ctx->options.platform, to->value_type);
                const bool dest_is_signed = object_type_is_signed_integer(to->value_type);

                /* 2^(n - 1) or 2^n, the first value out of the range */
                long double limit = 1.0L;
                for (int i = 0; i < (dest_is_signed ? dest_n_bits - 1 : dest_n_bits); i++)
                    limit *= 2.0L;

                const bool in_range =
                    type_is_bool(&to->type) ? (v >= 0.0L && v <= 1.0L) :
                    dest_is_signed ? (v >= -limit && v < limit) :
                    (v >= 0.0L && v < limit);

                /* range is checked before the conversion, so it is defined;
                   it truncates toward zero (avoids truncl/libm, not C89) */
                if (isnan(v) || !in_range ||
                    (dest_is_signed ?
                        wide_integer_to_long_double_s(wide_integer_from_long_double(v)) :
                        wide_integer_to_long_double_u(wide_integer_from_long_double(v))) != v)
                {
                    char value_buf[64] = { 0 };
                    snprintf(value_buf, sizeof value_buf, "%.17g", (double)v);
                    struct osstream ss = { 0 };
                    print_type_no_names(&ss, &to->type, &ctx->options.platform);
                    const enum diagnostic_id id =
                        to->type.storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR ?
                        C_ERROR_CONSTANT_VALUE_NOT_REPRESENTABLE :
                        W_CONSTANT_VALUE_NOT_REPRESENTABLE;
                    diagnostic(id,
                        ctx,
                        p_init_expression->first_token,
                        NULL,
                        "constant expression %s is not exactly representable in '%s'",
                        value_buf,
                        ss.c_str ? ss.c_str : "");
                    ss_close(&ss);
                }
            }

            else if (from->state == CONSTANT_VALUE_STATE_CONSTANT &&
                is_constant &&
                (object_type_is_signed_integer(from->value_type) || object_type_is_unsigned_integer(from->value_type)) &&
                (to->value_type == TYPE_FLOAT || to->value_type == TYPE_DOUBLE || to->value_type == TYPE_LONG_DOUBLE) &&
                p_init_expression)
            {
                /*
                  constexpr float f = 16777217;  error, nearest float is 16777216
                  const float g = 16777217;      warning
                */
                const int dest_n_bits = target_get_num_of_bits(&ctx->options.platform, to->value_type);
                bool representable;
                char value_buf[64] = { 0 };

                if (object_type_is_signed_integer(from->value_type))
                {
                    const signed long long v = object_to_signed_long_long(from);
                    const long double narrowed = resize_floating_point((long double)v, dest_n_bits);
                    /* 2^63 does not fit back into long long */
                    representable = narrowed < 9223372036854775808.0L && (signed long long)narrowed == v;
                    snprintf(value_buf, sizeof value_buf, "%lld", v);
                }
                else
                {
                    const unsigned long long v = object_to_unsigned_long_long(from);
                    const long double narrowed = resize_floating_point((long double)v, dest_n_bits);
                    /* 2^64 does not fit back into unsigned long long */
                    representable = narrowed < 18446744073709551616.0L && (unsigned long long)narrowed == v;
                    snprintf(value_buf, sizeof value_buf, "%llu", v);
                }

                if (!representable)
                {
                    const enum diagnostic_id id =
                        to->type.storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR ?
                        C_ERROR_CONSTANT_VALUE_NOT_REPRESENTABLE :
                        W_CONSTANT_VALUE_NOT_REPRESENTABLE;
                    struct osstream ss = { 0 };
                    print_type_no_names(&ss, &to->type, &ctx->options.platform);
                    diagnostic(id,
                        ctx,
                        p_init_expression->first_token,
                        NULL,
                        "constant expression %s is not exactly representable in '%s'",
                        value_buf,
                        ss.c_str ? ss.c_str : "");
                    ss_close(&ss);
                }
            }

            if ((to->type.storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR) &&
                type_is_pointer(&to->type) &&
                p_init_expression &&
                !expression_is_null_pointer_constant(p_init_expression))
            {
                /*
                  constexpr int* p = &g; the only pointer value allowed is null
                */
                diagnostic(C_ERROR_CONSTEXPR_POINTER_NOT_NULL,
                    ctx,
                    p_init_expression->first_token,
                    NULL,
                    "a constexpr pointer must be initialized with a null pointer constant");
            }

            if (requires_constant_initialization &&
                !object_has_constant_value(from))
            {
                if (p_init_expression &&
                    !type_is_pointer_or_array(&p_init_expression->object.type) &&
                    !type_is_function(&p_init_expression->object.type))
                {
                    struct token* _Opt tk = p_init_expression->first_token;

                    diagnostic(C_ERROR_REQUIRES_COMPILE_TIME_VALUE,
                        ctx,
                        tk,
                        NULL,
                        "initializer must be a constant expression");                    
                }
            }

            if (is_constant && from->state == CONSTANT_VALUE_STATE_CONSTANT)
            {
                /*
                  struct point { int x; } p = { .x = 1 };
                  const int i = p.x; //not a exactly constant
                  int j = i;}
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

struct object* _Owner _Opt make_object_ptr_core(const struct type* p_type,
    const char* member_designator,
    enum make_state make_state,
    const struct platform* target)
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
            p_object->member_designator = strdup(member_designator); //lint 26 assert line 1777

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

            if (p_type->array_num_elements > 0 && make_state != MAKE_STATE_ANY_LAZY_ARRAYS)
            {
                struct type array_item_type = get_array_item_type(p_type);

                //too big..
                const unsigned long long max_elements = p_type->array_num_elements > 1000 ? 1000 : p_type->array_num_elements;

                for (unsigned long long i = 0; i < max_elements; i++)
                {
                    char buffer[200] = { 0 };
                    snprintf(buffer, sizeof buffer, "%s[%llu]", member_designator, i);
                    struct object* _Owner _Opt p_member_obj = make_object_ptr_core(&array_item_type, buffer, make_state, target);
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
            if (p_type->enum_specifier && p_type->type_specifier_flags == TYPE_SPECIFIER_ENUM)
            {
                const struct enum_specifier* _Opt p_complete_enum = get_complete_enum_specifier(p_type->enum_specifier);
                if (p_complete_enum == NULL)
                {
                    throw;
                }

                p_type = &p_complete_enum->integer_type;
            }

            p_object = calloc(1, sizeof * p_object);
            if (p_object == NULL)
                throw;

            switch (make_state)
            {
            case MAKE_STATE_ZERO_CONSTANT:
                p_object->state = CONSTANT_VALUE_STATE_CONSTANT;
                p_object->value.host_wide_integer = wide_integer_from_signed(0);
                break;

            case MAKE_STATE_ZERO:
                p_object->state = CONSTANT_VALUE_EQUAL;
                p_object->value.host_wide_integer = wide_integer_from_signed(0);
                break;

            case MAKE_STATE_UNITIALIZED:
                p_object->state = CONSTANT_VALUE_STATE_UNINITIALIZED;
                p_object->value.host_wide_integer = wide_integer_from_signed(-1);
                break;
            case MAKE_STATE_ANY:
            case MAKE_STATE_ANY_LAZY_ARRAYS:
                p_object->state = CONSTANT_VALUE_STATE_ANY;
                p_object->value.host_wide_integer = wide_integer_from_signed(-1);
                break;
            }

            p_object->value_type = type_to_object_type(p_type, target);

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
                        if (p_member_declarator->declarator->name_opt)
                        {
                            snprintf(buffer, sizeof buffer, "%s.%s", member_designator, p_member_declarator->declarator->name_opt->lexeme);
                        }
                        else
                        {
                            /*
                            unnamed bit-fields allow for padding
                            */
                        }

                        struct object* _Owner _Opt p_member_obj = make_object_ptr_core(&p_member_declarator->declarator->object.type, buffer, make_state, target);
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
                    /* anonymous members are accessed as members of the parent */
                    snprintf(buffer, sizeof buffer, "%s", member_designator);

                    struct object* _Owner _Opt p_member_obj = make_object_ptr_core(&t, buffer, make_state, target);
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

struct object* _Owner _Opt make_object_ptr(const struct type* p_type, enum make_state make_state, const struct platform* target)
{
    return make_object_ptr_core(p_type, "", make_state, target);
}

int make_object_with_member_designator(const struct type* p_type,
    struct object* obj,
    const char* name,
    enum make_state make_state,
    const struct platform* target)
{
    /*
      p_type may alias &obj->type (this is common now that a declarator's
      or expression's own type lives inside its object). Take an
      independent copy before destroying/resetting obj, otherwise
      object_destroy(obj) below frees the very type p_type points to.
    */
    struct type type_copy = type_dup(p_type);

    object_destroy(obj);
    *obj = (struct object){ 0 };

    assert(obj->members.head == NULL);
    assert(obj->next == NULL);

    struct object* _Owner _Opt p = make_object_ptr_core(&type_copy, name, make_state, target);
    if (p)
    {
        *obj = *p; //not an error    
        object_fix_parent(obj, obj);
        free(p);

        /*
          make_object_ptr_core lowers an enum to its underlying integer type,
          which is what the *value* representation needs (see value_type), but
          it must not become the type of the declarator or expression this
          object belongs to: that would drop the declared name and the enum
          specifier. Keep exactly the type that was asked for.
        */
        type_destroy(&obj->type);
        obj->type = type_copy; /*moved*/
        return 0;
    }

    /*
        The build failed (an incomplete type, typically). The object stays
        empty, but the type must still be the one that was asked for: it is
        the declarator's or expression's own type, and dropping it would hide
        the very incompleteness that made this fail.
    */
    type_destroy(&obj->type);
    obj->type = type_copy; /*moved*/
    return 1;
}

struct object object_dup(const struct object* src)
{
    struct object result = { 0 };

    try
    {
        result.state = src->state;
        result.value_type = src->value_type;
        result.type = type_dup(&src->type);
        result.member_designator = src->member_designator ? strdup(src->member_designator) : NULL;
        result.value = src->value;
        result.parent = NULL;
        /* the copy shares the AST links of src, it does not change them */
        result.p_init_expression = (struct expression* _Opt)src->p_init_expression;
        result.p_ref = (struct object* _Opt)src->p_ref;
        result.next = NULL;

        for (const struct object* _Opt p = src->members.head; p; p = p->next)
        {
            struct object* _Opt _Owner pnew = calloc(1, sizeof * pnew);
            if (pnew == NULL) throw;
            *pnew = object_dup(p);
            object_list_push(&result.members, pnew);
        }
    }
    catch
    {
    }
    return result;
}

int make_object(const struct type* p_type, struct object* obj, enum make_state make_state, const struct platform* target)
{
    return make_object_with_member_designator(p_type, obj, "", make_state, target);
}

enum type_specifier_flags object_type_to_type_specifier(enum object_type type)
{
    /* the width is not representable in the flags, see type_make_bitint */
    if (object_type_is_signed_bitint(type))
        return TYPE_SPECIFIER_BITINT;

    if (object_type_is_unsigned_bitint(type))
        return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_BITINT;

    if (object_type_is_signed_bitfield(type))
        return TYPE_SPECIFIER_INT;

    if (object_type_is_unsigned_bitfield(type))
        return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT;

    switch (type)
    {
    case TYPE_SIGNED_CHAR:
        return TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_CHAR;
    case TYPE_UNSIGNED_CHAR: return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR;

    case TYPE_SIGNED_SHORT: return TYPE_SPECIFIER_SHORT;
    case TYPE_UNSIGNED_SHORT: return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_SHORT;

    case TYPE_SIGNED_INT: return TYPE_SPECIFIER_INT;
    case TYPE_UNSIGNED_INT: return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT;

    case TYPE_SIGNED_LONG: return TYPE_SPECIFIER_LONG;
    case TYPE_UNSIGNED_LONG: return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG;

    case TYPE_SIGNED_LONG_LONG: return TYPE_SPECIFIER_LONG_LONG;
    case TYPE_UNSIGNED_LONG_LONG: return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG_LONG;

    case TYPE_SIGNED_LONG_LONG_LONG: return TYPE_SPECIFIER_LONG_LONG_LONG;
    case TYPE_UNSIGNED_LONG_LONG_LONG: return TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG_LONG_LONG;

    case TYPE_FLOAT: return TYPE_SPECIFIER_FLOAT;
    case TYPE_DOUBLE: return TYPE_SPECIFIER_DOUBLE;
    case TYPE_LONG_DOUBLE: return TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_DOUBLE;

    default:
        break;
    }

    return TYPE_SPECIFIER_NONE;
}

enum object_type type_specifier_to_object_type(const enum type_specifier_flags type_specifier_flags, const struct platform* target)
{

    if (type_specifier_flags & TYPE_SPECIFIER_BOOL)
        return target->bool_type;

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

    /*
        Widest specifier first. `long long int` carries TYPE_SPECIFIER_INT as
        well as TYPE_SPECIFIER_LONG_LONG, so testing INT before LONG_LONG made
        every explicitly-spelled `long long int` (uint64_t among them) an int:
        `long long int a = 5000000000;` was reported as not representable, and
        "%lld" was reported as the wrong conversion for it.
    */
    if (type_specifier_flags & TYPE_SPECIFIER_UNSIGNED)
    {
        if (type_specifier_flags & TYPE_SPECIFIER_CHAR)
            return TYPE_UNSIGNED_CHAR;
        if (type_specifier_flags & TYPE_SPECIFIER_SHORT)
            return TYPE_UNSIGNED_SHORT;

        if (type_specifier_flags & TYPE_SPECIFIER_LONG_LONG_LONG)
            return TYPE_UNSIGNED_LONG_LONG_LONG;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
            return TYPE_UNSIGNED_LONG_LONG;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
            return TYPE_UNSIGNED_LONG;

        if (type_specifier_flags & TYPE_SPECIFIER_INT)
            return TYPE_UNSIGNED_INT;
    }
    else
    {
        if (type_specifier_flags & TYPE_SPECIFIER_CHAR)
        {
            /* plain char has the signedness of the target */
            if (type_specifier_flags & TYPE_SPECIFIER_SIGNED)
                return TYPE_SIGNED_CHAR;
            return target->char_t_type;
        }
        if (type_specifier_flags & TYPE_SPECIFIER_SHORT)
            return TYPE_SIGNED_SHORT;

        if (type_specifier_flags & TYPE_SPECIFIER_LONG_LONG_LONG)
            return TYPE_SIGNED_LONG_LONG_LONG;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
            return TYPE_SIGNED_LONG_LONG;
        if (type_specifier_flags & TYPE_SPECIFIER_LONG)
            return TYPE_SIGNED_LONG;

        if (type_specifier_flags & TYPE_SPECIFIER_INT)
            return TYPE_SIGNED_INT;
    }
    return TYPE_SIGNED_INT;
}

enum object_type type_to_object_type(const struct type* type, const struct platform* target)
{
    if (type_is_pointer(type))
    {
        return target->size_t_type;
    }

    /*
     * Bitfield members store their width in type->array_num_elements
     * (set in expressions.c when STORAGE_SPECIFIER_BITFIELD is propagated).
     * Map them into the TYPE_[UN]SIGNED_BITFIELD_N ranges.
     */
    if (type->storage_class_specifier_flags & STORAGE_SPECIFIER_BITFIELD)
    {
        int width = (int)type->array_num_elements;
        if (width < 1) width = 1;
        if (width > 128) width = 128;

        bool is_unsigned_base = false;

        if (type_is_bool(type))
            is_unsigned_base = true;
        else
            is_unsigned_base = !!(type->type_specifier_flags & TYPE_SPECIFIER_UNSIGNED);

        if (is_unsigned_base)
            return (enum object_type)(TYPE_UNSIGNED_BITFIELD_1 + (width - 1)); /* TYPE_UNSIGNED_BITFIELD_N */
        else
            return (enum object_type)(TYPE_SIGNED_BITFIELD_1 + (width - 1)); /* TYPE_SIGNED_BITFIELD_N  */
    }

    if (type->type_specifier_flags & TYPE_SPECIFIER_BITINT)
    {
        int width = type->bitint_width;
        if (width < 1) width = 1;
        if (width > WIDE_INTEGER_BITS) width = WIDE_INTEGER_BITS;

        if (type->type_specifier_flags & TYPE_SPECIFIER_UNSIGNED)
            return (enum object_type)(TYPE_UNSIGNED_BITINT_1 + (width - 1));
        else
            return (enum object_type)(TYPE_SIGNED_BITINT_1 + (width - 1));
    }

    return type_specifier_to_object_type(type->type_specifier_flags, target);
}

void object_print_value_debug(const struct object* a)
{
    a = object_get_referenced(a);

    if (object_type_is_integer(a->value_type))
    {
        char digits[WIDE_INTEGER_TO_STRING_MIN_SIZE] = { 0 };
        if (object_type_is_signed_integer(a->value_type))
            wide_integer_to_string_s(a->value.host_wide_integer, digits, sizeof digits);
        else
            wide_integer_to_string_u(a->value.host_wide_integer, digits, sizeof digits);

        if (object_type_is_signed_bitint(a->value_type))
        {
            printf("%s (_BitInt(%d))", digits, object_type_bitint_width(a->value_type));
        }
        else if (object_type_is_unsigned_bitint(a->value_type))
        {
            printf("%s (unsigned _BitInt(%d))", digits, object_type_bitint_width(a->value_type));
        }
        else if (object_type_is_signed_bitfield(a->value_type))
        {
            printf("%s (signed bitfield %d)", digits, object_type_bitfield_width(a->value_type));
        }
        else if (object_type_is_unsigned_bitfield(a->value_type))
        {
            printf("%s (unsigned bitfield %d)", digits, object_type_bitfield_width(a->value_type));
        }
        else
        {
            const char* name = "";
            switch (a->value_type)
            {
            case TYPE_SIGNED_CHAR: name = "signed char"; break;
            case TYPE_UNSIGNED_CHAR: name = "unsigned char"; break;
            case TYPE_SIGNED_SHORT: name = "short"; break;
            case TYPE_UNSIGNED_SHORT: name = "unsigned short"; break;
            case TYPE_SIGNED_INT: name = "int"; break;
            case TYPE_UNSIGNED_INT: name = "unsigned int"; break;
            case TYPE_SIGNED_LONG: name = "long"; break;
            case TYPE_UNSIGNED_LONG: name = "unsigned long"; break;
            case TYPE_SIGNED_LONG_LONG: name = "long long"; break;
            case TYPE_UNSIGNED_LONG_LONG: name = "unsigned long long"; break;
            case TYPE_SIGNED_LONG_LONG_LONG: name = "long long long"; break;
            case TYPE_UNSIGNED_LONG_LONG_LONG: name = "unsigned long long long"; break;
            default:
                break;
            }
            printf("%s (%s)", digits, name);
        }
        return;
    }

    switch (a->value_type)
    {
    case TYPE_FLOAT:
        printf("%Lf (float)", a->value.host_long_double);
        break;

    case TYPE_DOUBLE:
        printf("%Lf (double)", a->value.host_long_double);
        break;

    case TYPE_LONG_DOUBLE:
        printf("%Lf (long double)", a->value.host_long_double);
        break;

    default:
        break;
    }

}

void object_print_to_debug_core(const struct object* object, int n, const struct platform* target)
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

        const struct object* _Opt member = object->members.head;
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

void object_print_to_debug(const struct object* object, const struct platform* target)
{
    int n = 0;
    object_print_to_debug_core(object, n, target);
}

/*
   extends the array to the max_index returning the added item.
*/
struct object* _Opt object_extend_array_to_index(const struct type* p_type, struct object* a, size_t max_index, bool is_constant, const struct platform* target)
{
    try
    {
        for (size_t count = a->members.count; count < (max_index + 1); count++)
        {
            char name[50] = { 0 };
            snprintf(name, sizeof name, "[%zu]", count);

            struct object* _Owner _Opt p = make_object_ptr_core(p_type,
                name,
                is_constant ? MAKE_STATE_ZERO_CONSTANT : MAKE_STATE_ZERO,
                target);
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
    if (object_type_is_bitfield(a->value_type))
    {
        return true;
    }

    /* bit-precise integers are never promoted (C23 6.3.1.1) */
    if (object_type_is_bitint(a->value_type))
    {
        return false;
    }

    if (a->value_type == TYPE_SIGNED_CHAR ||
        a->value_type == TYPE_UNSIGNED_CHAR ||
        a->value_type == TYPE_SIGNED_SHORT ||
        a->value_type == TYPE_UNSIGNED_SHORT)
    {
        return true;
    }
    return false;
}

enum object_type object_common(const struct platform* target, const struct object* a, const struct object* b)
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

    enum object_type signed_promoted = is_signed(a_type) ? a_type : b_type;
    enum object_type unsigned_promoted = is_unsigned(a_type) ? a_type : b_type;

    if (get_rank(unsigned_promoted) >= get_rank(signed_promoted))
    {
        return unsigned_promoted;
    }

    /*
      Otherwise, if the type of the operand with signed integer type can represent all the values
      of the type of the operand with unsigned integer type, then the operand with unsigned
      integer type is converted to the type of the operand with signed integer type
    */

    if (target_sizeof(target, signed_promoted) > target_sizeof(target, unsigned_promoted))
    {
        return signed_promoted;
    }

    /*
      Otherwise, both operands are converted to the unsigned integer type corresponding to
      the type of the operand with signed integer type
    */

    return to_unsigned(signed_promoted);

}

void object_print_value(const struct platform* target, struct osstream* ss, const struct object* a)
{
    a = object_get_referenced(a);

    /* long long long holds its value like long long, so it is written like one */
    enum object_type value_type = a->value_type;
    if (value_type == TYPE_SIGNED_LONG_LONG_LONG)
        value_type = TYPE_SIGNED_LONG_LONG;
    else if (value_type == TYPE_UNSIGNED_LONG_LONG_LONG)
        value_type = TYPE_UNSIGNED_LONG_LONG;

    if (object_type_is_integer(a->value_type))
    {
        /*
          A value that does not fit in a long long has no literal. It is built
          from its halves, as the 128-bit type of the targets that have one.
        */
        const struct wide_integer v = a->value.host_wide_integer;
        const bool fits = object_type_is_signed_integer(a->value_type) ?
            wide_integer_equal(v, wide_integer_from_signed((long long)v.lo)) :
            v.hi == 0;
        if (!fits)
        {
            if (object_type_is_signed_integer(a->value_type))
                ss_fprintf(ss, "((__int128)((unsigned __int128)0x%llxULL << %d | 0x%llxULL))", v.hi, WIDE_INTEGER_HALF_BITS, v.lo);
            else
                ss_fprintf(ss, "((unsigned __int128)0x%llxULL << %d | 0x%llxULL)", v.hi, WIDE_INTEGER_HALF_BITS, v.lo);
            return;
        }
    }

    /* the value fits in a long long */
    const long long sv = (long long)a->value.host_wide_integer.lo;
    const unsigned long long uv = a->value.host_wide_integer.lo;

    if (object_type_is_signed_bitsized(value_type))
    {
        ss_fprintf(ss, "%lld", sv);
        return;
    }
    if (object_type_is_unsigned_bitint(a->value_type))
    {
        /* an unsigned _BitInt(64) value may not fit any signed type, so keep it unsigned */
        if (uv > target_unsigned_max(target, TYPE_UNSIGNED_INT))
        {
            ss_fprintf(ss, "%lluULL", uv);
        }
        else
        {
            ss_fprintf(ss, "%lluU", uv);
        }
        return;
    }
    if (object_type_is_unsigned_bitfield(a->value_type))
    {
        ss_fprintf(ss, "%llu", uv);
        return;
    }

    switch (value_type)
    {
    case TYPE_SIGNED_CHAR:
        ss_fprintf(ss, "%lld", sv);
        break;

    case TYPE_UNSIGNED_CHAR:
        ss_fprintf(ss, "%llu", uv);
        break;

    case TYPE_SIGNED_SHORT:
        ss_fprintf(ss, "%lld", sv);
        break;

    case TYPE_UNSIGNED_SHORT:
        ss_fprintf(ss, "%llu", uv);
        break;

    /*
      The minimum of a signed type has no literal of its own: -2147483648 is
      -(2147483648), and the positive part overflows the type. Emit it the way
      <limits.h> does, (-MAX - 1), so the literal keeps the intended type.
    */
    case TYPE_SIGNED_INT:
        if (sv == target_signed_min(target, TYPE_SIGNED_INT))
        {
            ss_fprintf(ss, "(-%lld - 1)", target_signed_max(target, TYPE_SIGNED_INT));
        }
        else
        {
            ss_fprintf(ss, "%lld", sv);
        }
        break;

    case TYPE_SIGNED_LONG:
        if (sv == target_signed_min(target, TYPE_SIGNED_LONG))
        {
            ss_fprintf(ss, "(-%lldL - 1)", target_signed_max(target, TYPE_SIGNED_LONG));
        }
        else
        {
            ss_fprintf(ss, "%lldL", sv);
        }
        break;

    case TYPE_UNSIGNED_LONG:
        ss_fprintf(ss, "%lluUL", uv);
        break;

    case TYPE_UNSIGNED_INT:
        ss_fprintf(ss, "%lluU", uv);
        break;

    case TYPE_SIGNED_LONG_LONG:
        if (sv == target_signed_min(target, TYPE_SIGNED_LONG_LONG))
        {
            ss_fprintf(ss, "(-%lldLL - 1)", target_signed_max(target, TYPE_SIGNED_LONG_LONG));
        }
        else
        {
            ss_fprintf(ss, "%lldLL", sv);
        }
        break;

    case TYPE_UNSIGNED_LONG_LONG:
        ss_fprintf(ss, "%lluULL", uv);
        break;

    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_LONG_DOUBLE:
        if (isinf(a->value.host_long_double) || isnan(a->value.host_long_double))
        {
            /*
              There is no literal for infinity/nan, so we build one the same way
              MSVC's <math.h> does: an overflowing product, cast to the wanted
              type. The cast (rather than a f/L suffix) matters because 1e+300f
              does not fit in a float and would be rejected on its own.

              This form is portable - it compiles and folds correctly on msvc,
              gcc, clang and tcc - so it is what we emit for every target.

              TODO decide whether to use the per-target alternatives instead.
              For the gcc/clang targets we could emit the builtins:

                  __builtin_inff() / __builtin_inf()  / __builtin_infl()
                  __builtin_nanf("") / __builtin_nan("") / __builtin_nanl("")

              They are constant expressions, keep the sign and payload explicit,
              and avoid the -Woverflow warning that gcc may emit for the
              overflowing product. The cost is that the generated code stops
              being compiler neutral, which is why we have `target` here.
            */
            const char* cast = "";
            if (a->value_type == TYPE_FLOAT)
                cast = "(float)";
            else if (a->value_type == TYPE_LONG_DOUBLE)
                cast = "(long double)";

            if (isinf(a->value.host_long_double))
            {
                ss_fprintf(ss, "%s(%s(1e+300 * 1e+300))",
                    a->value.host_long_double < 0 ? "-" : "",
                    cast);
            }
            else
            {
                ss_fprintf(ss, "(%s((1e+300 * 1e+300) * 0.0))", cast);
            }
            break;
        }
        else
        {
            /*
              Shortest round-trip decimal: the text we emit reads back as the
              exact same value, and 0.1 stays "0.1" instead of turning into
              0.10000000000000001. See fp_to_string.h.
            */
            char temp[64] = { 0 };

            if (a->value_type == TYPE_FLOAT)
                float_to_string((float)a->value.host_long_double, temp, sizeof temp);
            else if (a->value_type == TYPE_DOUBLE)
                double_to_string((double)a->value.host_long_double, temp, sizeof temp);
            else if (target->long_double_n_bits == 64)
            {
                /*
                  On this target long double is just a double (every msvc
                  target, and gcc/clang on arm64), so the exact printer applies.
                */
                double_to_string((double)a->value.host_long_double, temp, sizeof temp);
            }
            else
            {
                /*
                  Target long double is wider than a double - the 80-bit x87
                  format on gcc/clang x86, or 128-bit. Its significand does not
                  fit the printer above, so fall back to the host library and
                  accept the loss. Documented in manual.md, section 7.5.
                */
                snprintf(temp, sizeof temp, "%.21Lg", a->value.host_long_double);

                /*
                  %g drops the decimal point for whole numbers, and
                  9223372036854775808L would be an integer constant.
                */
                if (strpbrk(temp, ".eE") == NULL)
                {
                    const size_t len = strlen(temp);
                    if (len + 2 < sizeof temp)
                    {
                        temp[len] = '.';
                        temp[len + 1] = '0';
                        temp[len + 2] = '\0';
                    }
                }
            }

            ss_fprintf(ss, "%s", temp);
        }

        if (a->value_type == TYPE_FLOAT)
            ss_fprintf(ss, "f");
        else if (a->value_type == TYPE_LONG_DOUBLE)
            ss_fprintf(ss, "L");
        break;

    default:
        break;
    }

}

enum object_operator
{
    OBJECT_OPERATOR_EQUAL,
    OBJECT_OPERATOR_NOT_EQUAL,
    OBJECT_OPERATOR_GREATER_THAN_OR_EQUAL,
    OBJECT_OPERATOR_GREATER_THAN,
    OBJECT_OPERATOR_SMALLER_THAN_OR_EQUAL,
    OBJECT_OPERATOR_SMALLER_THAN,
    OBJECT_OPERATOR_ADD,
    OBJECT_OPERATOR_SUB,
    OBJECT_OPERATOR_MUL,
    OBJECT_OPERATOR_DIV,
    OBJECT_OPERATOR_MOD,
    OBJECT_OPERATOR_BITWISE_AND,
    OBJECT_OPERATOR_BITWISE_OR,
    OBJECT_OPERATOR_BITWISE_XOR,
    OBJECT_OPERATOR_SHIFT_LEFT,
    OBJECT_OPERATOR_SHIFT_RIGHT,
};

static bool object_operator_is_comparison(enum object_operator op)
{
    return op <= OBJECT_OPERATOR_SMALLER_THAN;
}

static bool object_operator_is_shift(enum object_operator op)
{
    return op == OBJECT_OPERATOR_SHIFT_LEFT || op == OBJECT_OPERATOR_SHIFT_RIGHT;
}

static bool object_comparison_result(enum object_operator op, int compare)
{
    switch (op)
    {
    case OBJECT_OPERATOR_EQUAL: return compare == 0;
    case OBJECT_OPERATOR_NOT_EQUAL: return compare != 0;
    case OBJECT_OPERATOR_GREATER_THAN_OR_EQUAL: return compare >= 0;
    case OBJECT_OPERATOR_GREATER_THAN: return compare > 0;
    case OBJECT_OPERATOR_SMALLER_THAN_OR_EQUAL: return compare <= 0;
    case OBJECT_OPERATOR_SMALLER_THAN: return compare < 0;
    default:
        break;
    }
    assert(false);
    return false;
}

/*
  x op y in the integer type 'type'; both already have it, except the shift
  count y, that keeps its own value. The result is wrapped to the width of
  the type, and warning_message says when that changed the exact result.
*/
static void object_binary_integer(const struct platform* target,
    enum object_operator op,
    struct wide_integer x,
    struct wide_integer y,
    enum object_type type,
    struct object* r,
    char warning_message[200])
{
    const bool is_signed = object_type_is_signed_integer(type);
    const int width = (int)target_get_num_of_bits(target, type);

    if (object_operator_is_comparison(op))
    {
        const int compare = is_signed ? wide_integer_compare_s(x, y) : wide_integer_compare_u(x, y);
        object_set_wide_integer(target, r, wide_integer_from_signed(object_comparison_result(op, compare)));
        return;
    }

    struct wide_integer v = { 0 };

    switch (op)
    {
    case OBJECT_OPERATOR_ADD:
        v = wide_integer_add(x, y);
        break;
    case OBJECT_OPERATOR_SUB:
        v = wide_integer_sub(x, y);
        break;
    case OBJECT_OPERATOR_MUL:
        v = wide_integer_mul(x, y);
        break;

    case OBJECT_OPERATOR_DIV:
    case OBJECT_OPERATOR_MOD:
    {
        struct wide_integer quotient = { 0 };
        struct wide_integer remainder = { 0 };
        const bool divided = is_signed ?
            wide_integer_divmod_s(x, y, &quotient, &remainder) :
            wide_integer_divmod_u(x, y, &quotient, &remainder);
        if (!divided)
        {
            snprintf(warning_message, 200, "division by zero");
            return;
        }
        v = op == OBJECT_OPERATOR_DIV ? quotient : remainder;
    }
    break;

    case OBJECT_OPERATOR_BITWISE_AND:
        v = wide_integer_and(x, y);
        break;
    case OBJECT_OPERATOR_BITWISE_OR:
        v = wide_integer_or(x, y);
        break;
    case OBJECT_OPERATOR_BITWISE_XOR:
        v = wide_integer_xor(x, y);
        break;

    case OBJECT_OPERATOR_SHIFT_LEFT:
    case OBJECT_OPERATOR_SHIFT_RIGHT:
    {
        /* a count out of the width of the type is undefined, 0 is used */
        const long long count = (long long)y.lo;
        if (wide_integer_compare_s(y, wide_integer_from_signed(0)) >= 0 && wide_integer_compare_s(y, wide_integer_from_signed(width)) < 0)
        {
            if (op == OBJECT_OPERATOR_SHIFT_LEFT)
                v = wide_integer_shl(x, (int)count);
            else
                v = is_signed ? wide_integer_shr_s(x, (int)count) : wide_integer_shr_u(x, (int)count);
        }
    }
    break;

    default:
        assert(false);
        break;
    }

    /* x and y are in the range of width, so the exact v of + - fits in a wide_integer */
    const struct wide_integer wrapped = is_signed ? object_wrap_s(v, width) : object_wrap_u(v, width);

    bool overflow = false;
    if (op == OBJECT_OPERATOR_ADD)
    {
        overflow = is_signed ?
            (wide_integer_is_negative(x) == wide_integer_is_negative(y) && wide_integer_is_negative(wrapped) != wide_integer_is_negative(x)) :
            wide_integer_compare_u(wrapped, x) < 0;
    }
    else if (op == OBJECT_OPERATOR_SUB)
    {
        overflow = is_signed ?
            (wide_integer_is_negative(x) != wide_integer_is_negative(y) && wide_integer_is_negative(wrapped) != wide_integer_is_negative(x)) :
            wide_integer_compare_u(x, y) < 0;
    }
    else if (op == OBJECT_OPERATOR_MUL && !wide_integer_is_zero(x))
    {
        /* the product wrapped when dividing it back does not give y */
        struct wide_integer quotient = { 0 };
        struct wide_integer remainder = { 0 };
        if (is_signed)
        {
            wide_integer_divmod_s(wrapped, x, &quotient, &remainder);
            overflow = !wide_integer_equal(quotient, y) ||
                /* -1 * MIN gives MIN back, and MIN / -1 gives MIN again */
                (wide_integer_equal(x, wide_integer_from_signed(-1)) && wide_integer_is_negative(y) && wide_integer_equal(wrapped, y));
        }
        else
        {
            wide_integer_divmod_u(wrapped, x, &quotient, &remainder);
            overflow = !wide_integer_equal(quotient, y);
        }
    }
    else if (op == OBJECT_OPERATOR_DIV && is_signed)
    {
        /* MIN / -1 */
        overflow = wide_integer_equal(y, wide_integer_from_signed(-1)) && wide_integer_is_negative(x) && wide_integer_equal(wrapped, x);
    }

    if (overflow)
    {
        /*
          v is the exact result while it fits in a wide_integer: always for
          + - and /, and for * up to the half of its width (two long long).
        */
        const bool exact_is_known =
            op == OBJECT_OPERATOR_MUL ? 2 * width <= WIDE_INTEGER_BITS : width < WIDE_INTEGER_BITS;

        char text[WIDE_INTEGER_TO_STRING_MIN_SIZE] = { 0 };
        char exact[WIDE_INTEGER_TO_STRING_MIN_SIZE] = { 0 };

        if (is_signed)
            wide_integer_to_string_s(wrapped, text, sizeof text);
        else
            wide_integer_to_string_u(wrapped, text, sizeof text);

        /* the exact result of an unsigned subtraction is negative */
        if (is_signed || op == OBJECT_OPERATOR_SUB)
            wide_integer_to_string_s(v, exact, sizeof exact);
        else
            wide_integer_to_string_u(v, exact, sizeof exact);

        if (exact_is_known)
            snprintf(warning_message, 200, "integer %s results in '%s'; the exact result is '%s'",
                is_signed ? "overflow" : "wrap-around", text, exact);
        else
            snprintf(warning_message, 200, "integer %s results in '%s'",
                is_signed ? "overflow" : "wrap-around", text);
    }

    object_set_wide_integer(target, r, wrapped);
}

/* x op y in the floating type 'type' */
static void object_binary_floating(const struct platform* target,
    enum object_operator op,
    long double x,
    long double y,
    enum object_type type,
    struct object* r,
    char warning_message[200])
{
    if (object_operator_is_comparison(op))
    {
        const int compare = x < y ? -1 : x > y ? 1 : 0;
        /* a NaN compares unequal and unordered */
        const bool unordered = isnan(x) || isnan(y);
        const bool result = unordered ? op == OBJECT_OPERATOR_NOT_EQUAL : object_comparison_result(op, compare);
        object_set_wide_integer(target, r, wide_integer_from_signed(result));
        return;
    }

    long double v = 0;
    switch (op)
    {
    case OBJECT_OPERATOR_ADD:
        v = x + y;
        break;
    case OBJECT_OPERATOR_SUB:
        v = x - y;
        break;
    case OBJECT_OPERATOR_MUL:
        v = x * y;
        break;
    case OBJECT_OPERATOR_DIV:
        v = x / y; //lint 36 div by zero, we want it here
        break;
    default:
        assert(false);
        snprintf(warning_message, 200, " invalid operands");
        return;
    }
    r->value.host_long_double = resize_floating_point(v, target_get_num_of_bits(target, type));
}

/*
  a op b after the usual arithmetic conversions (6.3.1.8). A shift has the
  type of the promoted left operand and keeps the value of the count.
  A comparison is an int.
*/
static struct object object_binary(const struct platform* target,
    enum object_operator op,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    warning_message[0] = '\0';
    a = object_get_referenced(a);
    b = object_get_referenced(b);

    const enum object_type common_type =
        object_operator_is_shift(op) ?
        (object_is_promoted(a) ? TYPE_SIGNED_INT : a->value_type) :
        object_common(target, a, b);

    struct object a0 = object_cast(target, common_type, a);
    struct object b0 = object_cast(target, object_operator_is_shift(op) ? TYPE_SIGNED_LONG_LONG : common_type, b);

    struct object r = { 0 };
    r.value_type = object_operator_is_comparison(op) ? TYPE_SIGNED_INT : common_type;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    if (object_type_is_integer(common_type))
    {
        object_binary_integer(target, op, a0.value.host_wide_integer, b0.value.host_wide_integer, common_type, &r, warning_message);
    }
    else
    {
        object_binary_floating(target, op, a0.value.host_long_double, b0.value.host_long_double, common_type, &r, warning_message);
    }

    object_destroy(&a0);
    object_destroy(&b0);
    return r;
}

struct object object_equal(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_EQUAL, a, b, warning_message);
}

struct object object_not_equal(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_NOT_EQUAL, a, b, warning_message);
}

struct object object_greater_than_or_equal(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_GREATER_THAN_OR_EQUAL, a, b, warning_message);
}

struct object object_greater_than(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_GREATER_THAN, a, b, warning_message);
}

struct object object_smaller_than_or_equal(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_SMALLER_THAN_OR_EQUAL, a, b, warning_message);
}

struct object object_smaller_than(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_SMALLER_THAN, a, b, warning_message);
}

struct object object_add(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_ADD, a, b, warning_message);
}

struct object object_sub(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_SUB, a, b, warning_message);
}

struct object object_mul(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_MUL, a, b, warning_message);
}

struct object object_div(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_DIV, a, b, warning_message);
}

struct object object_mod(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_MOD, a, b, warning_message);
}

int object_is_equal(const struct platform* target, const struct object* a, const struct object* b)
{
    char message[200] = { 0 };
    struct object r = object_equal(target, a, b, message);
    int i = !wide_integer_is_zero(r.value.host_wide_integer);
    object_destroy(&r);
    return i;
}

int object_is_not_equal(const struct platform* target, const struct object* a, const struct object* b)
{
    char message[200] = { 0 };
    struct object r = object_not_equal(target, a, b, message);
    int i = !wide_integer_is_zero(r.value.host_wide_integer);
    object_destroy(&r);

    return i;
}

int object_is_greater_than_or_equal(const struct platform* target, const struct object* a, const struct object* b)
{
    char message[200] = { 0 };
    struct object r = object_greater_than_or_equal(target, a, b, message);
    int i = !wide_integer_is_zero(r.value.host_wide_integer);
    object_destroy(&r);
    return i;
}

int object_is_smaller_than_or_equal(const struct platform* target, const struct object* a, const struct object* b)
{
    char message[200] = { 0 };
    struct object r = object_smaller_than_or_equal(target, a, b, message);
    int i = !wide_integer_is_zero(r.value.host_wide_integer);
    object_destroy(&r);
    return i;
}

struct object object_logical_not(const struct platform* target, const struct object* a, char warning_message[200])
{
    warning_message[0] = '\0';
    a = object_get_referenced(a);

    struct object r = { 0 };
    r.value_type = TYPE_SIGNED_INT;
    r.state = CONSTANT_VALUE_STATE_CONSTANT;

    const bool is_zero = object_type_is_integer(a->value_type) ?
        wide_integer_is_zero(a->value.host_wide_integer) :
        a->value.host_long_double == 0;

    object_set_wide_integer(target, &r, wide_integer_from_signed(is_zero));
    return r;
}

enum object_unary_operator
{
    OBJECT_UNARY_OPERATOR_MINUS,
    OBJECT_UNARY_OPERATOR_PLUS,
    OBJECT_UNARY_OPERATOR_BITWISE_NOT,
};

/* op a, after the integer promotions (bool, char and short become int, _BitInt is not promoted) */
static struct object object_unary(const struct platform* target,
    enum object_unary_operator op,
    const struct object* a,
    char warning_message[200])
{
    warning_message[0] = '\0';
    a = object_get_referenced(a);

    const enum object_type common_type = object_is_promoted(a) ? TYPE_SIGNED_INT : a->value_type;
    struct object a0 = object_cast(target, common_type, a);

    struct object r = { 0 };
    r.state = CONSTANT_VALUE_STATE_CONSTANT;
    r.value_type = common_type;

    if (object_type_is_integer(common_type))
    {
        const struct wide_integer x = a0.value.host_wide_integer;
        switch (op)
        {
        case OBJECT_UNARY_OPERATOR_MINUS:
            object_set_wide_integer(target, &r, wide_integer_neg(x));
            break;
        case OBJECT_UNARY_OPERATOR_PLUS:
            object_set_wide_integer(target, &r, x);
            break;
        case OBJECT_UNARY_OPERATOR_BITWISE_NOT:
            object_set_wide_integer(target, &r, wide_integer_not(x));
            break;
        }
    }
    else
    {
        const long double x = a0.value.host_long_double;
        switch (op)
        {
        case OBJECT_UNARY_OPERATOR_MINUS:
            r.value.host_long_double = resize_floating_point(-x, target_get_num_of_bits(target, common_type));
            break;
        case OBJECT_UNARY_OPERATOR_PLUS:
            r.value.host_long_double = resize_floating_point(+x, target_get_num_of_bits(target, common_type));
            break;
        case OBJECT_UNARY_OPERATOR_BITWISE_NOT:
            snprintf(warning_message, 200, "invalid operand");
            break;
        }
    }

    object_destroy(&a0);
    return r;
}

struct object object_bitwise_not(const struct platform* target, const struct object* a, char warning_message[200])
{
    return object_unary(target, OBJECT_UNARY_OPERATOR_BITWISE_NOT, a, warning_message);
}

struct object object_unary_minus(const struct platform* target, const struct object* a, char warning_message[200])
{
    return object_unary(target, OBJECT_UNARY_OPERATOR_MINUS, a, warning_message);
}

struct object object_unary_plus(const struct platform* target, const struct object* a, char warning_message[200])
{
    /*
       char  c = -5;
       int   i = +c; //it just perform integer promotion
     */
    return object_unary(target, OBJECT_UNARY_OPERATOR_PLUS, a, warning_message);
}

struct object object_bitwise_xor(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_BITWISE_XOR, a, b, warning_message);
}

struct object object_bitwise_or(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_BITWISE_OR, a, b, warning_message);
}

struct object object_bitwise_and(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_BITWISE_AND, a, b, warning_message);
}

struct object object_shift_left(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_SHIFT_LEFT, a, b, warning_message);
}

struct object object_shift_right(const struct platform* target,
    const struct object* a,
    const struct object* b,
    char warning_message[200])
{
    return object_binary(target, OBJECT_OPERATOR_SHIFT_RIGHT, a, b, warning_message);
}
