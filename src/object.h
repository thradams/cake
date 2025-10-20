
/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 *
 *  struct object is used to compute the compile time expressions (including constexpr)
 *
*/

#pragma once

#include "ownership.h"
#include <stdbool.h>
#include <wchar.h>
#include "error.h"
#include "type.h"
#include <inttypes.h>
struct parser_ctx;

#ifdef __GNUC__
#define CAKE_FLOAT128_DEFINED 1
#endif


enum object_value_type
{
    TYPE_SIGNED_INT8,
    TYPE_UNSIGNED_INT8,

    TYPE_SIGNED_INT16,
    TYPE_UNSIGNED_INT16,

    TYPE_SIGNED_INT32,
    TYPE_UNSIGNED_INT32,

    TYPE_SIGNED_INT64,
    TYPE_UNSIGNED_INT64,

    TYPE_FLOAT32,
    TYPE_FLOAT64,

#ifdef CAKE_FLOAT128_DEFINED
    TYPE_FLOAT128
#endif
};

enum object_value_state
{
    CONSTANT_VALUE_STATE_UNINITIALIZED,
    CONSTANT_VALUE_STATE_ANY,
    CONSTANT_VALUE_STATE_CONSTANT,
    //flow analysis
    CONSTANT_VALUE_NOT_EQUAL,
    CONSTANT_VALUE_EQUAL,
};

struct object_list
{
    struct object* _Owner _Opt head, * _Opt tail;
    size_t count;
};

void object_list_push(struct object_list* list, struct object* item);

struct object
{
    enum object_value_state state;
    enum object_value_type value_type;
    struct type type; //TODO to be removed we have 2 types in two places.

    const char* _Opt _Owner member_designator;

    union {

        int8_t signed_int8;
        uint8_t unsigned_int8;

        int16_t signed_int16;
        uint16_t unsigned_int16;

        int32_t signed_int32;
        uint32_t unsigned_int32;

        int64_t  signed_int64;
        int64_t  unsigned_int64;

        float float32;
        double float64;

#ifdef CAKE_FLOAT128_DEFINED
        long double float128;
#endif

    } value;
    struct object* _Opt parent; //to be removed
    struct object* _Opt p_ref;
    struct expression* _Opt p_init_expression;

    struct object_list members;
    struct object* _Opt _Owner next;
};

void object_swap(struct object* a, struct object* b);
void object_print_value_debug(const struct object* a);
void object_destroy(_Opt _Dtor struct object* p);
void object_delete(struct object* _Opt _Owner p);
bool object_has_constant_value(const struct object* a);
void object_to_string(const struct object* a, char buffer[], int sz);


//Make constant value
struct object            object_make_char(enum target target, int value);
struct object            object_make_wchar_t(enum target target, int value);
struct object             object_make_size_t(enum target target, uint64_t value);
struct object               object_make_bool(bool value);
struct object            object_make_nullptr(enum target target);
struct object        object_make_signed_char(signed char value);
struct object      object_make_unsigned_char(unsigned char value);
struct object       object_make_signed_short(signed short value);
struct object     object_make_unsigned_short(unsigned short value);
struct object         object_make_signed_int(signed int value);
struct object       object_make_unsigned_int(unsigned int value);

struct object        object_make_signed_long(signed long long value, enum target target);
struct object      object_make_unsigned_long(unsigned long long value, enum target target);

struct object   object_make_signed_long_long(signed long long value);
struct object object_make_unsigned_long_long(unsigned long long value);
struct object              object_make_float(float value);
struct object             object_make_double(double value);
struct object        object_make_long_double(long double value);
struct object        object_make_reference(struct object* object);


struct object     object_make_uint8(uint8_t value);
struct object     object_make_uint16(uint16_t value);
struct object     object_make_uint32(uint32_t value);


//dynamic cast
void object_set_signed_int(struct object* a, long long value);
void object_set_unsigned_int(struct object* a, unsigned long long value);

struct object object_cast(enum object_value_type e, const struct object* a);
enum object_value_type  type_specifier_to_object_type(const enum type_specifier_flags type_specifier_flags, enum target target);

errno_t object_increment_value(struct object* a);

//static cast
signed char object_to_signed_char(const struct object* a);
unsigned char object_to_unsigned_char(const struct object* a);
signed short object_to_signed_short(const struct object* a);
unsigned short object_to_unsigned_short(const struct object* a);
signed int object_to_signed_int(const struct object* a);
unsigned int object_to_unsigned_int(const struct object* a);
signed long object_to_signed_long(const struct object* a);
unsigned long object_to_unsigned_long(const struct object* a);
signed long long object_to_signed_long_long(const struct object* a);
unsigned long long object_to_unsigned_long_long(const struct object* a);
float object_to_float(const struct object* a);
double object_to_double(const struct object* a);
long double object_to_long_double(const struct object* a);
bool object_to_bool(const struct object* a);

int object_to_str(const struct object* a, int n, char str[/*n*/]);

int object_greater_than_or_equal(const struct object* a, const struct object* b);
int object_smaller_than_or_equal(const struct object* a, const struct object* b);
int object_equal(const struct object* a, const struct object* b);
int object_not_equal(const struct object* a, const struct object* b);
struct object object_add(const struct object* a, const struct object* b);
struct object object_sub(const struct object* a, const struct object* b);


//Overflow checks
bool unsigned_long_long_sub(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b);
bool unsigned_long_long_mul(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b);
bool unsigned_long_long_add(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b);
bool signed_long_long_sub(_Ctor signed long long* result, signed long long a, signed long long b);
bool signed_long_long_add(_Ctor signed long long* result, signed long long a, signed long long b);
bool signed_long_long_mul(_Ctor signed long long* result, signed long long a, signed long long b);

void object_default_initialization(struct object* p_object, bool is_constant);

struct object* _Opt object_get_member(struct object* p_object, size_t index);

int make_object_with_member_designator(const struct type* p_type, struct object* obj, const char* member_designator, enum target target);
int make_object(const struct type* p_type, struct object* obj, enum target target);
struct object object_dup(const struct object* src);

bool object_is_reference(const struct object* p_object);
bool object_is_derived(const struct object* p_object);
bool object_is_signed(const struct object* p_object);
void object_set_any(struct object* p_object);

bool object_is_one(const struct object* p_object);
bool object_is_zero(const struct object* p_object);

const struct object* object_get_referenced(const struct object* p_object);

_Attr(nodiscard)
int object_set(
    struct parser_ctx* ctx,
    struct object* to,
    struct expression* _Opt init_expression,
    const struct object* from,
    bool is_constant,
    bool requires_constant_initialization);

struct type;

enum object_value_type type_to_object_type(const struct type* type, enum target target);

void object_print_to_debug(const struct object* object, enum target target);

struct object* object_extend_array_to_index(const struct type* p_type, struct object* a, size_t n, bool is_constant, enum target target);
struct object* object_get_non_const_referenced(struct object* p_object);



struct objects
{
    struct object** _Opt items;
    int size;
    int capacity;
};

void objects_destroy(struct objects* arr);
int objects_push(struct objects* arr, struct object* obj); // returns 0 on success, ENOMEM on alloc fail

void object_print_value(struct osstream* ss, const struct object* a, enum target target);
