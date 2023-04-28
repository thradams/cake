#pragma once
#include <stdbool.h>

#include "osstream.h"
#include "error.h"
#include "options.h"

struct parser_ctx;

#include "annotations.h"

enum type_category
{
    TYPE_CATEGORY_ITSELF,
    TYPE_CATEGORY_FUNCTION,
    TYPE_CATEGORY_ARRAY,
    TYPE_CATEGORY_POINTER,
};

enum static_analisys_flags
{
    ISVALID = 1 << 1,
    UNINITIALIZED = 1 << 2,
    MUST_DESTROY = 1 << 3,
    MUST_FREE = 1 << 4
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

    CUSTOM_ATTRIBUTE_FREE = 1 << 7,
    CUSTOM_ATTRIBUTE_DESTROY = 1 << 8,

    /*
     Used to detect argument type
    */
    CUSTOM_ATTRIBUTE_PARAM = 1 << 9
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

enum type_qualifier_flags
{
    TYPE_QUALIFIER_NONE,
    TYPE_QUALIFIER_CONST = 1 << 0,
    TYPE_QUALIFIER_RESTRICT = 1 << 1,
    TYPE_QUALIFIER_VOLATILE = 1 << 2,
    TYPE_QUALIFIER__ATOMIC = 1 << 3,
};


struct params
{
    struct type* head;
    struct type* tail;
};


struct function_declarator_type
{
    /*
     function-declarator:
       direct-declarator ( parameter-type-list opt )
    */
    struct direct_declarator_type* direct_declarator_type;    
    struct params params;
    bool is_var_args;
};

struct array_declarator_type
{
    /*
     array-declarator:
        direct-declarator [ type-qualifier-list opt assignment-expression opt ]
        direct-declarator [ "static" type-qualifier-list opt assignment-expression ]
        direct-declarator [ type-qualifier-list "static" assignment-expression ]
        direct-declarator [ type-qualifier-list opt * ]
    */
    struct direct_declarator_type* direct_declarator_type;
    unsigned long long constant_size;
    enum type_qualifier_flags flags;
    bool has_static;
};

void array_declarator_type_delete(struct array_declarator_type* p);

struct direct_declarator_type
{
    const char* name_opt;
    struct declarator_type* declarator_opt;
    struct array_declarator_type* array_declarator_type;
    struct function_declarator_type* function_declarator_type;    
};


struct pointer_type
{
    enum type_qualifier_flags type_qualifier_flags;
    struct pointer_type* next;
};

struct pointer_type_list
{
    struct pointer_type* head;
    struct pointer_type* tail;
};

void pointer_type_list_pop_front(struct pointer_type_list* list);

struct declarator_type
{
    struct pointer_type_list pointers;
    struct direct_declarator_type* direct_declarator_type;
};


struct _destroy type
{    
    enum attribute_flags  attributes_flags;
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
   
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;
    
    struct declarator_type* declarator_type;

    enum type_category category;

    /*for linked list*/
    struct type* next;
};

void print_type(struct osstream* ss, const  struct type* type);
void print_item(struct osstream* ss, bool* first, const char* item);
struct type type_dup(const struct type* p_type);
void type_destroy(_destroy struct type* p_type);
struct declarator_type* declarator_type_copy(struct declarator_type* p_declarator_type);
void debug_print_declarator_type(struct declarator_type* p_declarator_type);
void print_declarator_type(struct osstream* ss, const struct declarator_type* p_declarator_type);

struct type get_function_return_type(struct type* p_type);

int type_common(struct type* p_type1, struct type* p_type2, struct type* out);
struct type get_array_item_type(struct type* p_type);
struct type type_remove_pointer(struct type* p_type);
int type_get_array_size(const struct type* p_type);
int type_set_array_size(struct type* p_type, int size);

bool type_is_enum(const struct type * p_type);
bool type_is_array(const struct type * p_type);
bool type_is_const(const struct type * p_type);
bool type_is_pointer(const struct type* p_type);
bool type_is_nullptr_t(const struct type* p_type);
bool type_is_void_ptr(const struct type* p_type);
bool type_is_integer(const struct type* p_type);
bool type_is_floating_point(const struct type* p_type);

bool type_is_arithmetic(const struct type* p_type);
bool type_is_compatible(const struct type* a, struct type* b);
bool type_is_struct_or_union(const struct type* p_type);
bool type_is_void(const struct type* p_type);
bool type_is_function_or_function_pointer(const struct type* p_type);
bool type_is_function(const struct type* p_type);
bool type_is_nodiscard(const struct type* p_type);
bool type_is_destroy(const struct type* p_type);
bool type_is_deprecated(const struct type* p_type);
bool type_is_maybe_unused(const struct type* p_type);
bool type_is_pointer_or_array(const struct type* p_type);
bool type_is_same(const struct type* a, const struct type* b, bool compare_qualifiers);
bool type_is_scalar(const struct type* p_type);

struct argument_expression;
void check_function_argument_and_parameter(struct parser_ctx* ctx,
    struct argument_expression* current_argument,
    struct type* paramer_type,
    int param_num);

struct type type_convert_to(struct type* p_type, enum language_version target);
struct type type_lvalue_conversion(struct type* p_type);
void type_remove_qualifiers(struct type* p_type);
void type_add_const(struct type* p_type);
void type_swap(struct type* a, struct type* b);
struct direct_declarator_type* find_inner_function(struct type* p_type);
struct  function_declarator_type* get_function_declarator_type(struct type* p_type);

struct type type_remove_pointer(struct type* p_type);
struct type get_array_item_type(struct type* p_type);

struct type type_param_array_to_pointer(const struct type* p_type);

struct type type_make_literal_string(int size, enum type_specifier_flags chartype);
struct type type_make_int();
struct type type_make_size_t();
struct type type_make_enumerator(struct enum_specifier* enum_specifier);
struct type make_void_type();

void print_declarator_description(struct osstream* ss, struct declarator_type* declarator);
struct type get_function_return_type(struct type* p_type);
int type_get_rank(struct type* p_type1);

int type_get_sizeof(struct type* p_type);
int type_get_alignof(struct type* p_type);
unsigned int type_get_hashof(struct parser_ctx* ctx, struct type* p_type);
struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type);
struct type type_add_pointer(struct type* p_type);
void type_print(const struct type* a);
enum type_category type_get_category(const struct type* p_type);
void print_type_qualifier_specifiers(struct osstream* ss, const struct type* type);
void declarator_type_merge(struct declarator_type* p_declarator_typet1, struct declarator_type* p_typedef_decl);
void declarator_type_clear_name(struct declarator_type* p_declarator_type);

struct declarator;
struct declarator_type* clone_declarator_to_declarator_type(struct parser_ctx* ctx, struct declarator* p_declarator);
enum type_category type_get_category_core(const struct type* p_type);
void type_print_data(const struct type* p_type);
void type_visit_to_mark_anonymous(struct type* p_type);
