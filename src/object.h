
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


long long target_signed_max(enum  target target, enum object_type type);
unsigned long long target_unsigned_max(enum  target target, enum object_type type);


enum object_value_state
{
    CONSTANT_VALUE_STATE_UNINITIALIZED,
    CONSTANT_VALUE_STATE_ANY,
    CONSTANT_VALUE_STATE_CONSTANT,
    
    /*flow analysis*/
    CONSTANT_VALUE_NOT_EQUAL,
    CONSTANT_VALUE_EQUAL,
};

struct object_list
{
    struct object* _Owner _Opt head, * _Opt tail;
    size_t count;
};

void object_list_push(struct object_list* list, struct object* _Owner item);


struct object
{
    enum object_value_state state;
    enum object_type value_type;
    struct type type; //TODO to be removed we have 2 types in two places.

    const char* _Opt _Owner member_designator;

    union 
    {
        signed long long  host_long_long;
        unsigned long long  host_u_long_long;
        long double host_long_double;
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


struct object            object_make_char(enum target target, int value);
struct object            object_make_wchar_t(enum target target, int value);
struct object             object_make_size_t(enum target target, unsigned long long value);
struct object               object_make_bool(enum target target, bool value);
struct object            object_make_nullptr(enum target target);

struct object      object_make_unsigned_char(enum target target, unsigned char value);

struct object         object_make_signed_int(enum target target, long long value);
struct object       object_make_unsigned_int(enum target target, unsigned long long  value);

struct object        object_make_signed_long(enum target target, signed long long value);
struct object      object_make_unsigned_long(enum target target, unsigned long long value);

struct object   object_make_signed_long_long(enum target target, signed long long value);
struct object object_make_unsigned_long_long(enum target target, unsigned long long value);
struct object              object_make_float(enum target target, long double value);
struct object             object_make_double(enum target target, long double value);
struct object        object_make_long_double(enum target target, long double value);
struct object        object_make_reference(struct object* object);


struct object     object_make_uint8(enum target target, uint8_t value);
struct object     object_make_uint16(enum target target, uint16_t value);
struct object     object_make_uint32(enum target target, uint32_t value);



struct object object_cast(enum target target, enum object_type e, const struct object* a);
enum object_type  type_specifier_to_object_type(const enum type_specifier_flags type_specifier_flags, enum target target);
enum type_specifier_flags object_type_to_type_specifier(enum object_type type);


void object_increment_value(enum target target, struct object* a);


signed long long object_to_signed_long_long(const struct object* a);
unsigned long long object_to_unsigned_long_long(const struct object* a);


bool object_is_true(const struct object* a);

int object_to_str(const struct object* a, int n, char str[/*n*/]);

int object_is_greater_than_or_equal(enum target target, const struct object* a, const struct object* b);
int object_is_smaller_than_or_equal(enum target target, const struct object* a, const struct object* b);
int object_is_equal(enum target target, const struct object* a, const struct object* b);
int object_is_not_equal(enum target target, const struct object* a, const struct object* b);


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

enum object_type type_to_object_type(const struct type* type, enum target target);

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

struct object object_add(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_sub(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_mul(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);


struct object object_div(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);



struct object object_mod(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_equal(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);


struct object object_not_equal(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);


struct object object_greater_than_or_equal(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_smaller_than_or_equal(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);



struct object object_greater_than(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_smaller_than(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_logical_not(enum target target, const struct object* a, char warning_message[200]);
struct object object_unary_minus(enum target target, const struct object* a, char warning_message[200]);
struct object object_unary_plus(enum target target, const struct object* a, char warning_message[200]);
struct object object_bitwise_not(enum target target, const struct object* a, char warning_message[200]);

struct object object_bitwise_xor(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_bitwise_or(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_bitwise_and(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_shift_left(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);

struct object object_shift_right(enum target target,
    const struct object* a,
    const struct object* b,
    char warning_message[200]);
