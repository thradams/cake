/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include <stdbool.h>
#include <stddef.h>

#include "osstream.h"
#include "options.h"

struct parser_ctx;

#include "ownership.h"

enum type_category
{
    TYPE_CATEGORY_ITSELF,
    TYPE_CATEGORY_FUNCTION,
    TYPE_CATEGORY_ARRAY,
    TYPE_CATEGORY_POINTER,
};


enum attribute_flags
{
    STD_ATTRIBUTE_NONE = 0,
    STD_ATTRIBUTE_DEPRECATED = 1 << 0,
    STD_ATTRIBUTE_FALLTHROUGH = 1 << 1,
    STD_ATTRIBUTE_MAYBE_UNUSED = 1 << 2,
    STD_ATTRIBUTE_NODISCARD = 1 << 3,
    STD_ATTRIBUTE_NORETURN = 1 << 4,
    STD_ATTRIBUTE_UNSEQUENCED = 1 << 5,
    STD_ATTRIBUTE_REPRODUCIBLE = 1 << 6,
    
    /*
     1 == 2 results in int in C
     lets add extra flag here
     not sure what is the best place to put in
     type specifier my generate some error
    */
    CAKE_HIDDEN_ATTRIBUTE_LIKE_BOOL = 1 << 25,
    // 'a'
    CAKE_HIDDEN_ATTRIBUTE_INT_LIKE_CHAR = 1 << 26
};

enum type_specifier_flags
{
    TYPE_SPECIFIER_NONE = 0,
    TYPE_SPECIFIER_VOID = 1 << 0,
    TYPE_SPECIFIER_CHAR = 1 << 1,
    TYPE_SPECIFIER_SHORT = 1 << 2,
    TYPE_SPECIFIER_INT = 1 << 3,
    TYPE_SPECIFIER_LONG = 1 << 4,

    TYPE_SPECIFIER_FLOAT = 1 << 5,
    TYPE_SPECIFIER_DOUBLE = 1 << 6,
    TYPE_SPECIFIER_SIGNED = 1 << 7,
    TYPE_SPECIFIER_UNSIGNED = 1 << 8,
    TYPE_SPECIFIER_BOOL = 1 << 9,
    TYPE_SPECIFIER_COMPLEX = 1 << 10,
    TYPE_SPECIFIER_DECIMAL32 = 1 << 11,
    TYPE_SPECIFIER_DECIMAL64 = 1 << 12,
    TYPE_SPECIFIER_DECIMAL128 = 1 << 13,
    TYPE_SPECIFIER_ATOMIC = 1 << 14,
    TYPE_SPECIFIER_STRUCT_OR_UNION = 1 << 15,
    TYPE_SPECIFIER_ENUM = 1 << 16,
    TYPE_SPECIFIER_TYPEDEF = 1 << 17,

    //MICROSOFT
    TYPE_SPECIFIER_INT8 = 1 << 18,
    TYPE_SPECIFIER_INT16 = 1 << 19,
    TYPE_SPECIFIER_INT32 = 1 << 20,
    TYPE_SPECIFIER_INT64 = 1 << 21,

    TYPE_SPECIFIER_LONG_LONG = 1 << 22,

    TYPE_SPECIFIER_TYPEOF = 1 << 23,

    TYPE_SPECIFIER_NULLPTR_T = 1 << 24,
};

#ifdef _WIN32

    #define CAKE_WCHAR_T_TYPE_SPECIFIER (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_SHORT)

    #ifdef _WIN64
    #define  CAKE_SIZE_T_TYPE_SPECIFIER (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT64)    
    #else
    #define  CAKE_SIZE_T_TYPE_SPECIFIER (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT)    
    #endif

#else 

    #define CAKE_WCHAR_T_TYPE_SPECIFIER (TYPE_SPECIFIER_INT)

    #ifdef __x86_64__
    /* 64-bit */
    #define  CAKE_SIZE_T_TYPE_SPECIFIER (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG)    
    #else
    #define  CAKE_SIZE_T_TYPE_SPECIFIER (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT)    
    #endif


#endif


enum type_qualifier_flags
{
    TYPE_QUALIFIER_NONE,
    TYPE_QUALIFIER_CONST = 1 << 0,
    TYPE_QUALIFIER_RESTRICT = 1 << 1,
    TYPE_QUALIFIER_VOLATILE = 1 << 2,
    TYPE_QUALIFIER__ATOMIC = 1 << 3,

    /*ownership extensions*/
    TYPE_QUALIFIER_OWNER = 1 << 4,
    TYPE_QUALIFIER_OBJ_OWNER = 1 << 5,
    TYPE_QUALIFIER_VIEW = 1 << 6,
    TYPE_QUALIFIER_NULLABLE = 1 << 7,
    
    TYPE_QUALIFIER_OUT = 1 << 9,
};

enum storage_class_specifier_flags
{
    STORAGE_SPECIFIER_NONE = 0,
    STORAGE_SPECIFIER_TYPEDEF = 1 << 0,
    STORAGE_SPECIFIER_EXTERN = 1 << 1,
    STORAGE_SPECIFIER_STATIC = 1 << 2,
    STORAGE_SPECIFIER_THREAD_LOCAL = 1 << 3,
    STORAGE_SPECIFIER_AUTO = 1 << 4,
    STORAGE_SPECIFIER_REGISTER = 1 << 5,
    STORAGE_SPECIFIER_CONSTEXPR = 1 << 6,

    /*extra flag just to annotate this*/
    STORAGE_SPECIFIER_CONSTEXPR_STATIC = 1 << 7,

    /*it is a function parameter*/
    STORAGE_SPECIFIER_PARAMETER = 1 << 11,

    STORAGE_SPECIFIER_AUTOMATIC_STORAGE = 1 << 12,
    STORAGE_SPECIFIER_FUNCTION_RETURN = 1 << 13,
    STORAGE_SPECIFIER_FUNCTION_RETURN_NODISCARD = 1 << 14,
};

enum function_specifier_flags
{
    FUNCTION_SPECIFIER_NONE = 0,
    FUNCTION_SPECIFIER_INLINE = 1 << 0,    
    FUNCTION_SPECIFIER_NORETURN = 1 << 1,    
};

struct declarator;
struct type;

enum assigment_type
{
    ASSIGMENT_TYPE_RETURN,    // T f() { return b; }
    ASSIGMENT_TYPE_PARAMETER, // void f(T a); f(b);
    ASSIGMENT_TYPE_OBJECTS,   // a = b
};


struct type_list {
    struct type* _Owner _Opt head;
    struct type* _Opt tail;
};

void type_list_destroy(struct type_list* _Obj_owner p_type_list);
void type_list_push_back(struct type_list* books, struct type* _Owner new_book);
void type_list_push_front(struct type_list* books, struct type* _Owner new_book);


struct param;

struct param_list {
    bool is_var_args;
    bool is_void;
    struct param* _Owner _Opt head;
    struct param* _Opt tail;
};

void param_list_destroy(struct param_list* _Obj_owner p);
void param_list_add(struct param_list*  p, struct param* _Owner p_item);

struct type
{
    enum type_category category;

    enum attribute_flags  attributes_flags;
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
    enum storage_class_specifier_flags storage_class_specifier_flags;

    const char* _Owner _Opt name_opt;

    struct struct_or_union_specifier* _Opt struct_or_union_specifier;
    const struct enum_specifier* _Opt enum_specifier;

    //Expression used as array size. Can be constant or not constant (VLA)
    const struct expression* _Opt array_num_elements_expression;

    int num_of_elements;
    bool static_array;

    /*
      address_of is true when the type is created by address of operator.
      This is used to create _Obj_owner pointer.
    */
    bool address_of;

    struct param_list params;
    struct type* _Owner _Opt next;
};

const struct param_list* _Opt type_get_func_or_func_ptr_params(const struct type* p_type);

struct param {
    struct type type;
    struct param* _Owner _Opt next;
};

void print_type(struct osstream* ss, const  struct type* type);
void print_type_no_names(struct osstream* ss, const struct type* p_type);

void print_item(struct osstream* ss, bool* first, const char* item);
struct type type_dup(const struct type* p_type);
void type_set(struct type* a, const struct type* b);
void type_destroy(_Opt struct type* _Obj_owner p_type);

struct type type_common(const struct type* p_type1, const struct type* p_type2);
struct type get_array_item_type(const struct type* p_type);
struct type type_remove_pointer(const struct type* p_type);

bool type_is_essential_bool(const struct type* p_type);
bool type_is_essential_char(const struct type* p_type);

bool type_is_enum(const struct type* p_type);
bool type_is_array(const struct type* p_type);

bool type_is_out(const struct type* p_type);
bool type_is_const(const struct type* p_type);
bool type_is_nullable(const struct type* p_type, bool nullable_enabled);
bool type_is_view(const struct type* p_type);

bool type_is_owner(const struct type* p_type);
bool type_is_obj_owner(const struct type* p_type);
bool type_is_any_owner(const struct type* p_type);

bool type_is_pointer_to_const(const struct type* p_type);
bool type_is_pointer(const struct type* p_type);
bool type_is_pointer_to_out(const struct type* p_type);
bool type_is_out(const struct type* p_type);
bool type_is_nullptr_t(const struct type* p_type);
bool type_is_void_ptr(const struct type* p_type);
bool type_is_integer(const struct type* p_type);
bool type_is_unsigned_integer(const struct type* p_type);
bool type_is_signed_integer(const struct type* p_type);
bool type_is_floating_point(const struct type* p_type);
int type_get_integer_rank(const struct type* p_type1);

bool type_is_arithmetic(const struct type* p_type);

bool type_is_struct_or_union(const struct type* p_type);
bool type_is_void(const struct type* p_type);
bool type_is_function_or_function_pointer(const struct type* p_type);
bool type_is_function(const struct type* p_type);
bool type_is_nodiscard(const struct type* p_type);

bool type_is_deprecated(const struct type* p_type);
bool type_is_maybe_unused(const struct type* p_type);
bool type_is_pointer_or_array(const struct type* p_type);
bool type_is_same(const struct type* a, const struct type* b, bool compare_qualifiers);
bool type_is_scalar(const struct type* p_type);
bool type_has_attribute(const struct type* p_type, enum attribute_flags attributes);
bool type_is_bool(const struct type* p_type);
bool type_is_decimal128(const struct type* p_type);
bool type_is_decimal64(const struct type* p_type);
bool type_is_decimal32(const struct type* p_type);
bool type_is_long_double(const struct type* p_type);
bool type_is_double(const struct type* p_type);
bool type_is_float(const struct type* p_type);
bool type_is_int(const struct type* p_type);
bool type_is_unsigned_int(const struct type* p_type);

bool type_is_vla(const struct type* p_type);

struct type type_get_enum_type(const struct type* p_type);

struct argument_expression;
void check_argument_and_parameter(struct parser_ctx* ctx,
    struct argument_expression* current_argument,
    struct type* paramer_type,
    int param_num);

struct type type_convert_to(const struct type* p_type, enum language_version target);
struct type type_lvalue_conversion(const struct type* p_type, bool nullchecks_enabled);
void type_remove_qualifiers(struct type* p_type);
void type_add_const(struct type* p_type);
void type_swap(struct type* a, struct type* b);
void type_integer_promotion(struct type* a);


struct type type_remove_pointer(const struct type* p_type);
struct type get_array_item_type(const struct type* p_type);

struct type type_param_array_to_pointer(const struct type* p_type, bool null_checks_enabled);

struct type type_make_literal_string(int size, enum type_specifier_flags chartype);
struct type type_make_int();
struct type type_make_int_bool_like();
struct type type_make_size_t();
struct type type_make_long_double();
struct type type_make_double();
struct type type_make_float();


struct type type_make_enumerator(const struct enum_specifier* enum_specifier);
struct type make_void_type();
struct type make_void_ptr_type();
struct type make_size_t_type();

struct type get_function_return_type(const struct type* p_type);

size_t type_get_sizeof(const struct type* p_type);

size_t type_get_alignof(const struct type* p_type);

struct type type_add_pointer(const struct type* p_type, bool null_checks_enabled);
void type_print(const struct type* a);
void type_println(const struct type* a);

enum type_category type_get_category(const struct type* p_type);
void print_type_qualifier_specifiers(struct osstream* ss, const struct type* type);

void type_visit_to_mark_anonymous(struct type* p_type);

void type_set_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator);
void type_merge_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator);

void print_type_declarator(struct osstream* ss, const struct type* p_type);
void type_remove_names(struct type* p_type);
const struct type* type_get_specifer_part(const struct type* p_type);

