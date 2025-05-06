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

struct parser_ctx;

enum object_value_type 
{

    TYPE_SIGNED_INT,
    TYPE_UNSIGNED_INT,

    TYPE_BOOL,

    TYPE_SIGNED_CHAR,
    TYPE_UNSIGNED_CHAR,

    TYPE_SIGNED_SHORT,
    TYPE_UNSIGNED_SHORT,

    TYPE_SIGNED_LONG,
    TYPE_UNSIGNED_LONG,

    TYPE_SIGNED_LONG_LONG,
    TYPE_UNSIGNED_LONG_LONG,

    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_LONG_DOUBLE
};

enum object_value_state
{
    /*object has a uninitialized value*/
    CONSTANT_VALUE_STATE_UNINITIALIZED,
    
    /*object has a unknown value*/
    CONSTANT_VALUE_STATE_UNKNOWN,

    /*
       Object has a exactly but not constant value
       I believe this can be used to execute code at compile time.(like c++ consteval)    
    */
    CONSTANT_VALUE_STATE_EXACT,

    /*object has a exactly and immutable value*/
    CONSTANT_VALUE_STATE_CONSTANT_EXACT,

    /*indirect*/
    CONSTANT_VALUE_STATE_REFERENCE,
};

struct object
{    
    enum object_value_state state;
    enum object_value_type value_type;
    struct type type; //TODO to be removed

    const char* _Opt _Owner debug_name; //TODO we can remove this passing tthe type to print function

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
    } value;
    struct object* _Opt parent; //to be removed
    struct expression * _Opt p_init_expression;
    struct object* _Opt _Owner members;
    struct object* _Opt _Owner next;
};

void object_destroy(_Opt _Dtor struct object* p);
void object_delete(struct object* _Opt _Owner p);
bool object_has_constant_value(const struct object* a);
void object_to_string(const struct object* a, char buffer[], int sz);

//Make constant value
struct object            object_make_wchar_t(wchar_t value);
struct object             object_make_size_t(size_t value);
struct object               object_make_bool(bool value);
struct object            object_make_nullptr();
struct object        object_make_signed_char(signed char value);
struct object      object_make_unsigned_char(unsigned char value);
struct object       object_make_signed_short(signed short value);
struct object     object_make_unsigned_short(unsigned short value);
struct object         object_make_signed_int(signed int value);
struct object       object_make_unsigned_int(unsigned int value);

struct object        object_make_signed_long(signed long value);
struct object      object_make_unsigned_long(unsigned long value);
struct object   object_make_signed_long_long(signed long long value);
struct object object_make_unsigned_long_long(unsigned long long value);
struct object              object_make_float(float value);
struct object             object_make_double(double value);
struct object        object_make_long_double(long double value);
struct object        object_make_reference(struct object* object);


//dynamic cast
struct object object_cast(enum object_value_type e, const struct object* a);
enum object_value_type  type_specifier_to_object_type(const enum type_specifier_flags type_specifier_flags);

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

//Overflow checks
bool unsigned_long_long_sub(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b);
bool unsigned_long_long_mul(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b);
bool unsigned_long_long_add(_Ctor unsigned long long* result, unsigned long long a, unsigned long long b);
bool signed_long_long_sub(_Ctor signed long long* result, signed long long a, signed long long b);
bool signed_long_long_add(_Ctor signed long long* result, signed long long a, signed long long b);
bool signed_long_long_mul(_Ctor signed long long* result, signed long long a, signed long long b);

void object_default_initialization(struct object* p_object, bool is_constant);

struct object* _Opt object_get_member(struct object* p_object, int index);

int make_object(const struct type* p_type, struct object* obj);

bool object_is_reference(const struct object* p_object);
bool object_is_derived(const struct object* p_object);

const struct object* object_get_referenced(const struct object* p_object);

NODISCARD
int object_set(
    struct parser_ctx* ctx, 
    struct object* to,
    struct expression* _Opt init_expression, 
    const struct object* from, 
    bool is_constant,
    bool requires_constant_initialization);

struct type;

enum object_value_type type_to_object_type(const struct type* type);

void object_print_to_debug(const struct object* object);

struct object* object_extend_array_to_index(const struct type* p_type, struct object* a, int n, bool is_constant);
struct object* object_get_non_const_referenced(struct object* p_object);
