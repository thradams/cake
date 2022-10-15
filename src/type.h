#pragma once
#include <stdbool.h>

#include "osstream.h"
#include "error.h"

struct parser_ctx;

#include "annotations.h"

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
    STD_ATTRIBUTE_REPRODUCIBLE = 1 << 6
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

    TYPE_SPECIFIER_NULLPTR = 1 << 24,
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
};

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


struct type
{    
    enum attribute_flags  attributes_flags;
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
   
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;
    
    struct declarator_type* declarator_type;

    /*for linked list*/
    struct type* next;
};

void print_type(struct osstream* ss, struct type* type);
void print_item(struct osstream* ss, bool* first, const char* item);
struct type type_copy(struct type* p_type);

struct declarator_type* declarator_type_copy(struct declarator_type* p_declarator_type);
void debug_print_declarator_type(struct declarator_type* p_declarator_type);
void print_declarator_type(struct osstream* ss, struct declarator_type* p_declarator_type);

struct type get_function_return_type(struct type* p_type);
struct type type_common(struct type* p_type1, struct type* p_type2, struct error* error);
struct type get_array_item_type(struct type* p_type);
struct type get_pointer_content_type(struct type* p_type);
bool type_is_array(struct type* p_type);
bool type_is_pointer(struct type* p_type);
bool type_is_integer(struct type* p_type);
bool type_is_floating_point(struct type* p_type);
bool type_is_void(struct type* p_type);
bool type_is_arithmetic(struct type* p_type);
bool type_is_compatible(struct type* a, struct type* b);
bool type_is_compatible_type_function_call(struct type* a, struct type* b);
bool type_is_function_or_function_pointer(struct type* p_type);
bool type_is_function(struct type* p_type);
bool type_is_nodiscard(struct type* p_type);
bool type_is_deprecated(struct type* p_type);
bool type_is_maybe_unused(struct type* p_type);
struct  function_declarator_type* get_function_declarator_type(struct type* p_type);

struct type get_pointer_content_type(struct type* p_type);
struct type get_array_item_type(struct type* p_type);


void print_declarator_description(struct osstream* ss, struct declarator_type* declarator);
struct type get_function_return_type(struct type* p_type);
bool type_is_pointer_or_array(struct type* p_type);
int type_get_rank(struct type* p_type1, struct error* error);
void type_set_int(struct type* p_type);
int type_get_sizeof(struct parser_ctx* ctx, struct type* p_type, struct error* error);
unsigned int type_get_hashof(struct parser_ctx* ctx, struct type* p_type, struct error* error);
bool type_is_same(struct type* a, struct type* b, bool compare_qualifiers);
struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type);
struct type get_address_of_type(struct type* p_type);
void type_print(struct type* a);
bool type_is_scalar(struct type* p_type);
enum type_category find_type_category(const struct type* p_type);
void print_type_qualifier_specifiers(struct osstream* ss, struct type* type);
void declarator_type_merge(struct declarator_type* p_declarator_typet1, struct declarator_type* p_typedef_decl);
void declarator_type_clear_name(struct declarator_type* p_declarator_type);

struct declarator;
struct declarator_type* clone_declarator_to_declarator_type(struct parser_ctx* ctx, struct declarator* p_declarator);
