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

    //TODO decide attribute or not
    CAKE_ATTRIBUTE_CTOR = 1 << 7,
    CAKE_ATTRIBUTE_DTOR = 1 << 8,

    /*
     1 == 2 results in int in C
     lets add extra flag here
     not sure what is the best place to put in
     type specifier my generate some error
    */
    CAKE_HIDDEN_ATTRIBUTE_LIKE_BOOL = 1 << 25,
    // 'a'
    CAKE_HIDDEN_ATTRIBUTE_INT_LIKE_CHAR = 1 << 26,

    /*
       Storing calling convention on attributes to consuming less memory
    */
    CAKE_ATTRIBUTE_FASTCALL = 1 << 27,
    CAKE_ATTRIBUTE_STDCALL = 1 << 28,
    CAKE_ATTRIBUTE_CDECL = 1 << 29,

    CAKE_ATTRIBUTE_LEAK = 1 << 30,

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

   
    TYPE_SPECIFIER_LONG_LONG = 1 << 22,

    TYPE_SPECIFIER_TYPEOF = 1 << 23,

    TYPE_SPECIFIER_NULLPTR_T = 1 << 24,

    TYPE_SPECIFIER_GCC__BUILTIN_VA_LIST = 1 << 25
};


enum type_qualifier_flags
{
    TYPE_QUALIFIER_NONE,
    TYPE_QUALIFIER_CONST = 1 << 0,
    TYPE_QUALIFIER_RESTRICT = 1 << 1,
    TYPE_QUALIFIER_VOLATILE = 1 << 2,
    TYPE_QUALIFIER__ATOMIC = 1 << 3,



    /*ownership extensions*/
    TYPE_QUALIFIER_OWNER = 1 << 4,
    TYPE_QUALIFIER_VIEW = 1 << 5,
    TYPE_QUALIFIER_OPT = 1 << 6,

    /*function contract*/
    TYPE_QUALIFIER_DTOR = 1 << 7,
    TYPE_QUALIFIER_CTOR = 1 << 8,

    TYPE_QUALIFIER_MSVC_PTR32 = 1 << 9,
    TYPE_QUALIFIER_MSVC_PTR64 = 1 << 10,

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
    STORAGE_SPECIFIER_BLOCK_SCOPE = 1 << 12,

    STORAGE_SPECIFIER_FUNCTION_RETURN = 1 << 13,
    STORAGE_SPECIFIER_FUNCTION_RETURN_NODISCARD = 1 << 14,
};

bool is_automatic_variable(enum storage_class_specifier_flags f);
enum function_specifier_flags
{
    FUNCTION_SPECIFIER_NONE = 0,
    FUNCTION_SPECIFIER_INLINE = 1 << 0,
    FUNCTION_SPECIFIER_NORETURN = 1 << 1,
};

enum alignment_specifier_flags
{
    ALIGNMENT_SPECIFIER_NONE = 0,
    ALIGNMENT_SPECIFIER_8_FLAGS = 1 << 0,
    ALIGNMENT_SPECIFIER_16_FLAGS = 1 << 1,
    ALIGNMENT_SPECIFIER_32_FLAGS = 1 << 2,
    ALIGNMENT_SPECIFIER_64_FLAGS = 1 << 3,
    ALIGNMENT_SPECIFIER_128_FLAGS = 1 << 4,
};

enum msvc_declspec_flags
{
    //https://learn.microsoft.com/en-us/cpp/cpp/declspec?view=msvc-170
    MSVC_DECLSPEC_NONE = 0,
    MSVC_DECLSPEC_ALIGN_8_FLAG = 1 << 1, //( NUMBER )
    MSVC_DECLSPEC_ALIGN_16_FLAG = 1 << 2, //( NUMBER )
    MSVC_DECLSPEC_ALIGN_32_FLAG = 1 << 3, //( NUMBER )
    MSVC_DECLSPEC_ALIGN_64_FLAG = 1 << 4, //( NUMBER )

    MSVC_DECLSPEC_ALLOCATE_FLAG = 1 << 5, //(" SEGNAME ")
    MSVC_DECLSPEC_ALLOCATOR_FLAG = 1 << 6, //
    MSVC_DECLSPEC_APPDOMAIN_FLAG = 1 << 7, //
    MSVC_DECLSPEC_CODE_SEG_FLAG = 1 << 8, //(" SEGNAME ")
    MSVC_DECLSPEC_DEPRECATED_FLAG = 1 << 9, //
    MSVC_DECLSPEC_DLLIMPORT_FLAG = 1 << 10, //
    MSVC_DECLSPEC_DLLEXPORT_FLAG = 1 << 11, //
    MSVC_DECLSPEC_EMPTY_BASES_FLAG = 1 << 12, //
    MSVC_DECLSPEC_HYBRID_PATCHABLE_FLAG = 1 << 13, //
    MSVC_DECLSPEC_JITINTRINSIC_FLAG = 1 << 14, //
    MSVC_DECLSPEC_NAKED_FLAG = 1 << 15, //
    MSVC_DECLSPEC_NOALIAS_FLAG = 1 << 16, //
    MSVC_DECLSPEC_NOINLINE_FLAG = 1 << 17, //
    MSVC_DECLSPEC_NORETURN_FLAG = 1 << 18, //
    MSVC_DECLSPEC_NOTHROW_FLAG = 1 << 19, //
    MSVC_DECLSPEC_NOVTABLE_FLAG = 1 << 20, //
    MSVC_DECLSPEC_NO_SANITIZE_ADDRESS_FLAG = 1 << 21, //
    MSVC_DECLSPEC_PROCESS_FLAG = 1 << 22, //
    MSVC_DECLSPEC_PROPERTY_FLAG = 1 << 23, //( { GET=GET-FUNC-NAME | ,PUT=PUT-FUNC-NAME } )
    MSVC_DECLSPEC_RESTRICT_FLAG = 1 << 24, //
    MSVC_DECLSPEC_SAFEBUFFERS_FLAG = 1 << 25, //
    MSVC_DECLSPEC_SELECTANY_FLAG = 1 << 26, //
    MSVC_DECLSPEC_SPECTRE_FLAG = 1 << 27, //(NOMITIGATION)
    MSVC_DECLSPEC_THREAD_FLAG = 1 << 28, //
    MSVC_DECLSPEC_UUID_FLAG = 1 << 29, //(" COMOBJECTGUID ")
};

struct declarator;
struct type;

enum assigment_type
{
    ASSIGMENT_TYPE_RETURN,    // T f() { return b; }
    ASSIGMENT_TYPE_PARAMETER, // void f(T a); f(b);
    ASSIGMENT_TYPE_OBJECTS,   // a = b
    ASSIGMENT_TYPE_INIT,   // T a = b
};


struct type_list
{
    struct type* _Owner _Opt head;
    struct type* _Opt tail;
};

void type_list_destroy(_Dtor struct type_list* p_type_list);
void type_list_push_back(struct type_list* books, struct type* _Owner new_book);
void type_list_push_front(struct type_list* books, struct type* _Owner new_book);


struct param;

struct param_list
{
    bool is_var_args;
    bool is_void;
    struct param* _Owner _Opt head;
    struct param* _Opt tail;
};

void param_list_destroy(_Dtor struct param_list* p);
void param_list_add(struct param_list* p, struct param* _Owner p_item);

struct type
{
    enum type_category category;

    enum attribute_flags  attributes_flags;
    enum msvc_declspec_flags msvc_declspec_flags;
    enum alignment_specifier_flags alignment_specifier_flags;
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
    enum storage_class_specifier_flags storage_class_specifier_flags;

    const char* _Owner _Opt name_opt;

    struct struct_or_union_specifier* _Opt struct_or_union_specifier;
    const struct enum_specifier* _Opt enum_specifier;

    //Expression used as array size. Can be constant or not constant (VLA)
    const struct expression* _Opt array_num_elements_expression;

    size_t num_of_elements;
    bool has_static_array_size;
    

    /*
      address_of is true when the type is created by address of operator.
      This is used to create _Dtor pointer.
    */
    bool address_of;

    struct param_list params;
    struct type* _Owner _Opt next;
};

const struct param_list* _Opt type_get_func_or_func_ptr_params(const struct type* p_type);

struct param
{
    struct type type;
    struct param* _Owner _Opt next;
};

void print_type(struct osstream* ss, const  struct type* type, enum target target);
void print_type_no_names(struct osstream* ss, const struct type* p_type, enum target target);

void print_item(struct osstream* ss, bool* first, const char* item);
struct type type_dup(const struct type* p_type);
void type_set(struct type* a, const struct type* b);
void type_destroy(_Opt _Dtor struct type* p_type);

struct type type_common(const struct type* p_type1, const struct type* p_type2, enum target target);
struct type get_array_item_type(const struct type* p_type);
struct type type_remove_pointer(const struct type* p_type);

bool type_is_essential_bool(const struct type* p_type);
bool type_is_essential_char(const struct type* p_type);

bool type_is_enum(const struct type* p_type);
bool type_is_array(const struct type* p_type);

bool type_is_ctor(const struct type* p_type);
bool type_is_const(const struct type* p_type);
bool type_is_constexpr(const struct type* p_type);
bool type_is_const_or_constexpr(const struct type* p_type);
bool type_is_opt(const struct type* p_type, bool nullable_enabled);
bool type_is_view(const struct type* p_type);

bool type_is_owner(const struct type* p_type);
bool type_is_pointed_dtor(const struct type* p_type);
bool type_is_owner_or_pointer_to_dtor(const struct type* p_type);

bool type_is_pointer_to_const(const struct type* p_type);
bool type_is_pointer(const struct type* p_type);
bool type_is_pointer_to_out(const struct type* p_type);

bool type_is_nullptr_t(const struct type* p_type);
bool type_is_void_ptr(const struct type* p_type);
bool type_is_integer(const struct type* p_type);
bool type_is_char(const struct type* p_type);
bool type_is_array_of_char(const struct type* p_type);
bool type_is_unsigned_integer(const struct type* p_type);
bool type_is_signed_integer(const struct type* p_type);
bool type_is_floating_point(const struct type* p_type);
int type_get_integer_rank(const struct type* p_type1);

bool type_is_arithmetic(const struct type* p_type);

bool type_is_struct_or_union(const struct type* p_type);
bool type_is_union(const struct type* p_type);

bool type_is_void(const struct type* p_type);
bool type_is_function_or_function_pointer(const struct type* p_type);
bool type_is_function(const struct type* p_type);
bool type_is_nodiscard(const struct type* p_type);

bool type_is_deprecated(const struct type* p_type);
bool type_is_maybe_unused(const struct type* p_type);
bool type_is_pointer_or_array(const struct type* p_type);
bool type_is_same(const struct type* a, const struct type* b, bool compare_qualifiers);
bool type_is_compatible(const struct type* a, const struct type* b);
bool type_is_scalar(const struct type* p_type);
bool type_is_scalar_decay(const struct type* p_type);
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
bool type_is_empty(const struct type* p_type);

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
void type_clear(struct type* a);
void type_integer_promotion(struct type* a);


struct type type_remove_pointer(const struct type* p_type);
struct type get_array_item_type(const struct type* p_type);

struct type type_param_array_to_pointer(const struct type* p_type, bool null_checks_enabled);

struct type type_make_literal_string(int size, enum type_specifier_flags chartype, enum type_qualifier_flags qualifiers, enum target target);
struct type type_make_int();
struct type type_make_int_bool_like();
struct type type_make_size_t(enum target target);
struct type type_make_ptrdiff_t(enum target target);

struct type type_make_long_double();
struct type type_make_double();
struct type type_make_float();


struct type type_make_enumerator(const struct enum_specifier* enum_specifier);
struct type make_void_type();
struct type make_void_ptr_type();
struct type make_size_t_type(enum target target);
struct type make_with_type_specifier_flags(enum type_specifier_flags f);


struct type get_function_return_type(const struct type* p_type);
bool function_returns_void(const struct type* p_type);


enum sizeof_error
{
    ESIZEOF_NONE = 0,
    ESIZEOF_OVERLOW,
    ESIZEOF_VLA,
    ESIZEOF_INCOMPLETE,
    ESIZEOF_FUNCTION
};

enum sizeof_error type_get_sizeof(const struct type* p_type, size_t* size, enum target target);
enum sizeof_error type_get_offsetof(const struct type* p_type, const char* member, size_t* size, enum target target);

size_t type_get_alignof(const struct type* p_type, enum target target);

struct type type_add_pointer(const struct type* p_type, bool null_checks_enabled);
void type_print(const struct type* a, enum target target);
void type_println(const struct type* a, enum target target);

enum type_category type_get_category(const struct type* p_type);
void print_type_qualifier_specifiers(struct osstream* ss, const struct type* type, enum target target);

void type_visit_to_mark_anonymous(struct type* p_type);

void type_set_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator);
void type_set_storage_specifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator);
void type_merge_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator);

void print_type_declarator(struct osstream* ss, const struct type* p_type, enum target target);
void type_remove_names(struct type* p_type);
const struct type* type_get_specifer_part(const struct type* p_type);
void print_msvc_declspec(struct osstream* ss, bool* first, enum msvc_declspec_flags  msvc_declspec_flags);