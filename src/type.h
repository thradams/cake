#pragma once
#include <stdbool.h>

#include "osstream.h"
#include "error.h"

struct parser_ctx;


enum type_specifier_flags
{
    type_specifier_none = 0,
    type_specifier_void = 1 << 0,
    type_specifier_char = 1 << 1,
    type_specifier_short = 1 << 2,
    type_specifier_int = 1 << 3,
    type_specifier_long = 1 << 4,

    type_specifier_float = 1 << 5,
    type_specifier_double = 1 << 6,
    type_specifier_signed = 1 << 7,
    type_specifier_unsigned = 1 << 8,
    type_specifier_bool = 1 << 9,
    type_specifier_complex = 1 << 10,
    type_specifier_decimal32 = 1 << 11,
    type_specifier_decimal64 = 1 << 12,
    type_specifier_decimal128 = 1 << 13,
    type_specifier_atomic = 1 << 14,
    type_specifier_struct_or_union = 1 << 15,
    type_specifier_enum = 1 << 16,
    type_specifier_typedef = 1 << 17,

    //microsoft
    type_specifier_int8 = 1 << 18,
    type_specifier_int16 = 1 << 19,
    type_specifier_int32 = 1 << 20,
    type_specifier_int64 = 1 << 21,

    type_specifier_long_long = 1 << 22,
    type_specifier_typeof = 1 << 23,
};

enum type_qualifier_flags
{
    type_qualifier_none,
    type_qualifier_const = 1 << 0,
    type_qualifier_restrict = 1 << 1,
    type_qualifier_volatile = 1 << 2,
    type_qualifier__Atomic = 1 << 3,
};


struct type_list
{
    struct type* head;
    struct type* tail;
};

struct array_function_type
{
    int constant_size;

    struct type_list params; //se for funcao lista de tipos usado por cada parametro
    bool bVarArg; //true se for funcao ....
    bool bIsArray;
    bool bIsFunction;
    int array_size;

    struct array_function_type* next;
};

struct array_function_type_list
{
    struct array_function_type* head;
    struct array_function_type* tail;
};

struct array_function_type* array_function_type_list_pop_back(struct array_function_type_list* list);

struct direct_declarator_type
{
    struct declarator_type* declarator_opt; //nao nulo se tiver (declarator )
    struct array_function_type_list array_function_type_list;
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
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
   
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;
    
    struct declarator_type* declarator_type;

    struct type* next; //se quiser usar lista  ligada
};
void print_type(struct osstream* ss, struct type* type);

struct type type_copy(struct type* p_type);

struct declarator_type* declarator_type_copy(struct declarator_type* p_declarator_type);


struct type get_function_return_type(struct type* p_type);
struct type type_common(struct type* p_type1, struct type* p_type2, struct error* error);
struct type get_array_item_type(struct type* p_type);
struct type get_pointer_content_type(struct type* p_type);
bool type_is_array(struct type* p_type);
bool type_is_pointer(struct type* p_type);
bool type_is_integer(struct type* p_type);
bool type_is_arithmetic(struct type* p_type);
bool type_is_compatible_type_function_call(struct type* a, struct type* b);
bool type_is_function_or_function_pointer(struct type* p_type);
struct type get_pointer_content_type(struct type* p_type);
struct type get_array_item_type(struct type* p_type);


void print_declarator_description(struct osstream* ss, struct declarator_type* declarator);
struct type get_function_return_type(struct type* p_type);
bool type_is_pointer_or_array(struct type* p_type);
int type_get_rank(struct type* p_type1, struct error* error);
void type_set_int(struct type* p_type);
int type_get_sizeof(struct parser_ctx* ctx, struct type* p_type, struct error* error);
unsigned int type_get_hashof(struct parser_ctx* ctx, struct type* p_type, struct error* error);
bool type_is_same(struct type* a, struct type* b);
struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type);
struct type get_address_of_type(struct type* p_type);
void type_print(struct type* a);
