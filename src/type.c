#include <stdbool.h>
#include "type.h"
#include "parser.h"
#include "error.h"
#include <stdlib.h>
#include <assert.h>
#include "hash.h"
#include <string.h>
#include <stdio.h>
struct declarator* find_declarator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt);
bool direct_declarator_type_is_same(struct direct_declarator_type* a, struct direct_declarator_type* b);
struct direct_declarator_type* clone_direct_declarator_to_direct_declarator_type(struct parser_ctx* ctx, struct direct_declarator* p_direct_declarator);

struct direct_declarator_type* direct_declarator_type_copy(struct direct_declarator_type* p_direct_declarator_type_opt);

void print_item(struct osstream* ss, bool* first, const char* item)
{
    if (!(*first))
        ss_fprintf(ss, " ");
    ss_fprintf(ss, "%s", item);
    *first = false;

}

bool print_type_specifier_flags(struct osstream* ss, bool* first, enum type_specifier_flags e_type_specifier_flags)
{
    if (e_type_specifier_flags & TYPE_SPECIFIER_VOID)
        print_item(ss, first, "void");

    if (e_type_specifier_flags & TYPE_SPECIFIER_SIGNED)
        print_item(ss, first, "signed");

    if (e_type_specifier_flags & TYPE_SPECIFIER_UNSIGNED)
        print_item(ss, first, "unsigned");

    if (e_type_specifier_flags & TYPE_SPECIFIER_INT)
        print_item(ss, first, "int");

    if (e_type_specifier_flags & TYPE_SPECIFIER_SHORT)
        print_item(ss, first, "short");

    if (e_type_specifier_flags & TYPE_SPECIFIER_LONG)
        print_item(ss, first, "long");

    if (e_type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        print_item(ss, first, "long long");

    if (e_type_specifier_flags & TYPE_SPECIFIER_CHAR)
        print_item(ss, first, "char");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        print_item(ss, first, "double");

    if (e_type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        print_item(ss, first, "float");

    if (e_type_specifier_flags & TYPE_SPECIFIER_BOOL)
        print_item(ss, first, "_Bool");

    if (e_type_specifier_flags & TYPE_SPECIFIER_COMPLEX)
        print_item(ss, first, "_Complex");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DECIMAL32)
        print_item(ss, first, "_Decimal32");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DECIMAL64)
        print_item(ss, first, "_Decimal64");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DECIMAL128)
        print_item(ss, first, "_Decimal128");

    return first;
}



void print_direct_declarator_type(struct osstream* ss, struct direct_declarator_type* type);

void print_declarator_type(struct osstream* ss, struct declarator_type* p_declarator_type)
{
    if (p_declarator_type == NULL)
    {
        return;
    }

    bool first = false;
    struct pointer_type* pointer = p_declarator_type->pointers.head;
    while (pointer)
    {
        ss_fprintf(ss, "*");
        print_type_qualifier_flags(ss, &first, pointer->type_qualifier_flags);

        pointer = pointer->next;
    }

    if (p_declarator_type->direct_declarator_type)
    {        
        print_direct_declarator_type(ss, p_declarator_type->direct_declarator_type);
    }


}

/*in case we need to print in console to debug*/
void debug_print_declarator_type(struct declarator_type* p_declarator_type)
{
    struct osstream ss = { 0 };
    print_declarator_type(&ss, p_declarator_type);
    printf("%s\n", ss.c_str);
    ss_close(&ss);
}

void print_direct_declarator_type(struct osstream* ss,
    struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type == NULL)
        return;

    if (p_direct_declarator_type->name_opt)
    {
        ss_fprintf(ss, " %s", p_direct_declarator_type->name_opt);
    }
    if (p_direct_declarator_type->declarator_opt)
    {
        if (p_direct_declarator_type->declarator_opt->pointers.head == NULL &&
            p_direct_declarator_type->declarator_opt->direct_declarator_type->array_declarator_type == NULL &&
            p_direct_declarator_type->declarator_opt->direct_declarator_type->function_declarator_type == NULL &&
            p_direct_declarator_type->declarator_opt->direct_declarator_type->declarator_opt == NULL)
        {
           /*no need ()*/            
            print_declarator_type(ss, p_direct_declarator_type->declarator_opt);            
        }
        else
        {
            ss_fprintf(ss, "(");
            print_declarator_type(ss, p_direct_declarator_type->declarator_opt);
            ss_fprintf(ss, ")");
        }
    }
    

    if (p_direct_declarator_type->function_declarator_type)
    {
        print_direct_declarator_type(ss,
            p_direct_declarator_type->function_declarator_type->direct_declarator_type);

        ss_fprintf(ss, "(");
        struct type* param = p_direct_declarator_type->function_declarator_type->params.head;
        while (param)
        {
            if (param != p_direct_declarator_type->function_declarator_type->params.head)
                ss_fprintf(ss, ",");
            print_type(ss, param);
            param = param->next;
        }
        if (p_direct_declarator_type->function_declarator_type->is_var_args)
            ss_fprintf(ss, ",...");

        ss_fprintf(ss, ")");
    }

    if (p_direct_declarator_type->array_declarator_type)
    {
        if (p_direct_declarator_type->array_declarator_type->direct_declarator_type)
        {
            print_direct_declarator_type(ss,
                p_direct_declarator_type->array_declarator_type->direct_declarator_type);
        }
        else
        {
            //int [2]
            //assert(false);
        }
        ss_fprintf(ss, "[%d]", p_direct_declarator_type->array_declarator_type->constant_size);
        
    }
}


void print_type_qualifier_flags(struct osstream* ss, bool* first, enum type_qualifier_flags e_type_qualifier_flags)
{

    if (e_type_qualifier_flags & TYPE_QUALIFIER_CONST)
        print_item(ss, first, "const");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_RESTRICT)
        print_item(ss, first, "restrict");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_VOLATILE)
        print_item(ss, first, "volatile");

}

void print_type_qualifier_specifiers(struct osstream* ss, struct type* type)
{
    bool first = true;
    print_type_qualifier_flags(ss, &first, type->type_qualifier_flags);

    if (type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
    {
        print_item(ss, &first, "struct ");
        ss_fprintf(ss, "%s", type->struct_or_union_specifier->tag_name);
    }
    else if (type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
    {
        print_item(ss, &first, "enum ");
        if (type->enum_specifier->tag_token)
            ss_fprintf(ss, "%s", type->enum_specifier->tag_token->lexeme);

    }
    else if (type->type_specifier_flags & TYPE_SPECIFIER_TYPEDEF)
    {
        assert(false);
    }
    else
    {
        print_type_specifier_flags(ss, &first, type->type_specifier_flags);
    }
}

void print_type(struct osstream* ss, struct type* type)
{
    print_type_qualifier_specifiers(ss, type);
    print_declarator_type(ss, type->declarator_type);// type->declarator_name_opt);
}

void type_print(struct type* a) {
    struct osstream ss = { 0 };
    print_type(&ss, a);
    puts(ss.c_str);
    puts("\n");
    ss_close(&ss);
}

void pointer_type_list_pop_front(struct pointer_type_list* list)
{
    if (list->head != NULL)
    {
        struct pointer_type* p = list->head;
        if (list->head == list->tail)
        {
            list->head = list->tail = NULL;
        }
        else
        {
            list->head = list->head->next;
        }
        p->next = NULL;
    }
}

struct pointer_type_list pointer_type_list_copy(struct pointer_type_list* p_pointer_type_list)
{
    struct pointer_type_list list = { 0 };
    struct pointer_type* p = p_pointer_type_list->head;
    while (p)
    {
        struct pointer_type* p_pointer_type = calloc(1, sizeof * p_pointer_type);
        p_pointer_type->type_qualifier_flags = p->type_qualifier_flags;
        list_add(&list, p_pointer_type);
        p = p->next;
    }
    return list;
}


struct params params_copy(struct params* input)
{
    struct params r = { 0 };
    struct type* p_param_type = input->head;
    while (p_param_type)
    {
        struct type* par = calloc(1, sizeof * par);
        *par = type_copy(p_param_type);
        list_add(&r, par);
        p_param_type = p_param_type->next;
    }
    return r;
}

struct array_declarator_type* array_declarator_type_copy(struct array_declarator_type* parray_declarator_type)
{
    if (parray_declarator_type == NULL)
        return NULL;

    struct array_declarator_type* p_array_declarator_type =
        calloc(1, sizeof(struct function_declarator_type));

    p_array_declarator_type->constant_size = parray_declarator_type->constant_size;
    p_array_declarator_type->direct_declarator_type = direct_declarator_type_copy(parray_declarator_type->direct_declarator_type);
    //p_array_declarator_type->params = params_copy(&parray_declarator_type->params);

    return p_array_declarator_type;
}

struct function_declarator_type* function_declarator_type_copy(struct function_declarator_type* pfunction_declarator_type)
{
    if (pfunction_declarator_type == NULL)
        return NULL;

    struct function_declarator_type* p_function_declarator_type =
        calloc(1, sizeof(struct function_declarator_type));

    p_function_declarator_type->is_var_args = pfunction_declarator_type->is_var_args;
    p_function_declarator_type->direct_declarator_type = direct_declarator_type_copy(pfunction_declarator_type->direct_declarator_type);
    p_function_declarator_type->params = params_copy(&pfunction_declarator_type->params);

    return p_function_declarator_type;
}


struct direct_declarator_type* direct_declarator_type_copy(struct direct_declarator_type* p_direct_declarator_type_opt)
{
    if (p_direct_declarator_type_opt == NULL)
        return NULL;
    struct direct_declarator_type* p_direct_declarator_type = calloc(1, sizeof(struct direct_declarator_type));
    
    p_direct_declarator_type->declarator_opt = declarator_type_copy(p_direct_declarator_type_opt->declarator_opt);
    p_direct_declarator_type->function_declarator_type = function_declarator_type_copy(p_direct_declarator_type_opt->function_declarator_type);
    p_direct_declarator_type->array_declarator_type = array_declarator_type_copy(p_direct_declarator_type_opt->array_declarator_type);
    
    if (p_direct_declarator_type_opt->name_opt)
      p_direct_declarator_type->name_opt = strdup(p_direct_declarator_type_opt->name_opt);

    return p_direct_declarator_type;
}

struct declarator_type* declarator_type_copy(struct declarator_type* p_declarator_type_opt)
{
    if (p_declarator_type_opt == NULL)
        return NULL;
    struct declarator_type* p_declarator_type = calloc(1, sizeof(struct declarator_type));
    p_declarator_type->pointers = pointer_type_list_copy(&p_declarator_type_opt->pointers);
    p_declarator_type->direct_declarator_type = direct_declarator_type_copy(p_declarator_type_opt->direct_declarator_type);
    
    return p_declarator_type;
}

struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type);


void visit_declarator_get(enum type_category* type_category, struct declarator_type* declarator);
void visit_direct_declarator_get(enum type_category* type_category, struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt)
        visit_declarator_get(type_category, p_direct_declarator_type->declarator_opt);

    if (p_direct_declarator_type->function_declarator_type)
    {
        if (p_direct_declarator_type->function_declarator_type->direct_declarator_type)
        {
            visit_direct_declarator_get(type_category, p_direct_declarator_type->function_declarator_type->direct_declarator_type);
        }

        if (*type_category == TYPE_CATEGORY_ITSELF)
        {
            *type_category = TYPE_CATEGORY_FUNCTION;         
        }
    }
    
    if (p_direct_declarator_type->array_declarator_type)
    {
        if (p_direct_declarator_type->array_declarator_type->direct_declarator_type)
        {
            visit_direct_declarator_get(type_category, p_direct_declarator_type->array_declarator_type->direct_declarator_type);
        }

        if (*type_category == TYPE_CATEGORY_ITSELF)
        {
            *type_category = TYPE_CATEGORY_ARRAY; /*array*/            
        }
    }
}

void visit_declarator_get(enum type_category* type_category, struct declarator_type* declarator)
{
    if (declarator == NULL)
        return;

    if (declarator->direct_declarator_type)
        visit_direct_declarator_get(type_category, declarator->direct_declarator_type);

    if (*type_category == TYPE_CATEGORY_ITSELF)
    {
        if (declarator->pointers.head)
        {
            *type_category = TYPE_CATEGORY_POINTER;
        }
    }
}

enum type_category find_type_category(const struct type* p_type)
{
    enum type_category type_category = TYPE_CATEGORY_ITSELF;
    visit_declarator_get(&type_category, p_type->declarator_type);
    return type_category;
}

void type_destroy(struct type* p_type)
{
    //TODO
}

bool type_has_attribute(struct type* p_type, enum attribute_flags attributes)
{
    if (p_type->attributes_flags & attributes)
    {
        /*like 
          [[maybe_unused]] int i;
        */
        return true;
    }

    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt = NULL;


    if (p_type->struct_or_union_specifier)
    {
        /*like
          struct [[maybe_unused]] X { }
          struct X x;
        */
        p_attribute_specifier_sequence_opt = p_type->struct_or_union_specifier->attribute_specifier_sequence_opt;
        if (p_attribute_specifier_sequence_opt == NULL &&
            p_type->struct_or_union_specifier->complete_struct_or_union_specifier)
        {
            p_attribute_specifier_sequence_opt = p_type->struct_or_union_specifier->complete_struct_or_union_specifier->attribute_specifier_sequence_opt;
        }
    }
    else if (p_type->enum_specifier)
    {
        p_attribute_specifier_sequence_opt = p_type->enum_specifier->attribute_specifier_sequence_opt;
        if (p_attribute_specifier_sequence_opt == NULL &&
            p_type->enum_specifier->complete_enum_specifier)
        {
            p_attribute_specifier_sequence_opt = p_type->enum_specifier->complete_enum_specifier->attribute_specifier_sequence_opt;
        }
    }

    if (p_attribute_specifier_sequence_opt &&
        p_attribute_specifier_sequence_opt->attributes_flags & attributes)
    {
        return true;
    }

    return false;
}

struct  function_declarator_type* get_function_declarator_type(struct type* p_type)
{
    assert(type_is_function_or_function_pointer(p_type));
    struct declarator_type* inner = p_type->declarator_type;
    for (;;)
    {
        if (inner->direct_declarator_type &&
            inner->direct_declarator_type->function_declarator_type &&
            inner->direct_declarator_type->function_declarator_type->direct_declarator_type &&
            inner->direct_declarator_type->function_declarator_type->direct_declarator_type->declarator_opt)
        {
            inner = inner->direct_declarator_type->function_declarator_type->direct_declarator_type->declarator_opt;
        }
        else
            break;
    }
    return inner->direct_declarator_type->function_declarator_type;
}

bool type_is_maybe_unused(struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_MAYBE_UNUSED);
}

bool type_is_deprecated(struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_DEPRECATED);
}

bool type_is_nodiscard(struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_NODISCARD);    
}

bool type_is_destroy(struct type* p_type)
{
    return type_has_attribute(p_type, CUSTOM_ATTRIBUTE_DESTROY);
}

bool type_is_array(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ARRAY;
}




bool type_is_pointer(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_POINTER;
}


bool type_is_enum(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ITSELF &&
           p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM;
}

bool type_is_struct_or_union(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION;
}

/*
* The three types char, signed char, and unsigned char are collectively called the character types.
The implementation shall define char to have the same range, representation, and behavior as either
signed char or unsigned char.50)
*/

bool type_is_character(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR;
}

bool type_is_bool(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL;
}

bool type_is_void(struct type* p_type)
{
    if (find_type_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags & TYPE_SPECIFIER_VOID;
}

/*
  The type char, the signed and unsigned integer types, and the enumerated types are collectively
  called integer types. The integer and real floating types are collectively called real types
*/
bool type_is_floating_point(struct type* p_type)
{
    if (find_type_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_DOUBLE |
            TYPE_SPECIFIER_FLOAT);
}


/*
  The type char, the signed and unsigned integer types, and the enumerated types are collectively
  called integer types. The integer and real floating types are collectively called real types
*/
bool type_is_integer(struct type* p_type)
{
    if (find_type_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {
        /*we cannot check long without check double*/
        //long double
        return false;
    }

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_CHAR |
            TYPE_SPECIFIER_SHORT |
            TYPE_SPECIFIER_INT |
            TYPE_SPECIFIER_LONG |
            TYPE_SPECIFIER_SIGNED |
            TYPE_SPECIFIER_UNSIGNED |
            TYPE_SPECIFIER_INT8 |
            TYPE_SPECIFIER_INT16 |
            TYPE_SPECIFIER_INT64 |
            TYPE_SPECIFIER_LONG_LONG);
}

/*
* Integer and floating types are collectively called arithmetic types.
*/
bool type_is_arithmetic(struct type* p_type)
{
    if (find_type_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;


    return p_type->type_specifier_flags &
        (           
            TYPE_SPECIFIER_FLOAT |
            TYPE_SPECIFIER_DOUBLE |

            TYPE_SPECIFIER_DECIMAL32 |
            TYPE_SPECIFIER_DECIMAL64 |
            TYPE_SPECIFIER_DECIMAL128 |

            TYPE_SPECIFIER_CHAR |
            TYPE_SPECIFIER_SHORT |
            
            TYPE_SPECIFIER_INT |            
            TYPE_SPECIFIER_ENUM | /*work as int*/

            TYPE_SPECIFIER_LONG |
            TYPE_SPECIFIER_SIGNED |
            TYPE_SPECIFIER_UNSIGNED |
            TYPE_SPECIFIER_INT8 |
            TYPE_SPECIFIER_INT16 |
            TYPE_SPECIFIER_INT64 |
            TYPE_SPECIFIER_LONG_LONG
            );
}

/*
 Arithmetic types, pointer types, and the nullptr_t type are collectively called scalar types. Array
and structure types are collectively called aggregate types51).
*/
bool type_is_scalar(struct type* p_type)
{

    if (type_is_arithmetic(p_type))
        return true;

    if (type_is_pointer_or_array(p_type))
        return true;

    if (find_type_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags & TYPE_SPECIFIER_NULLPTR;
}

bool type_is_compatible(struct type* expression_type, struct type* return_type)
{
    //TODO

    //if (!type_is_same(a, b))
      //  return false;

    return true;
}

bool type_is_compatible_type_function_call(struct type* argument_type, struct type* paramer_type)
{
    if (type_is_arithmetic(argument_type) && type_is_arithmetic(paramer_type))
    {
        return true;
    }

    if (type_is_arithmetic(argument_type) && type_is_bool(paramer_type))
    {
        /*passar 0 or 1 to bool*/
        return true;
    }


    if (type_is_bool(argument_type) && type_is_bool(paramer_type))
    {
        return true;
    }

    if (type_is_pointer(argument_type) && type_is_pointer(paramer_type))
    {
        if (argument_type->type_specifier_flags & TYPE_SPECIFIER_VOID)
        {
            /*void pointer can be converted to any type*/
            return true;
        }

        if (!type_is_same(argument_type, paramer_type, false))
        {
            //disabled for now util it works correctly
            //return false;
        }
        return true;
    }

    if (type_is_array(argument_type) && type_is_pointer(paramer_type))
    {
        //TODO
        return true;
    }

    if (type_is_pointer(argument_type) && type_is_array(paramer_type))
    {
        //TODO
        return true;
    }

    if (type_is_enum(argument_type) && type_is_enum(paramer_type))
    {
        if (!type_is_same(argument_type, paramer_type, false))
        {
            //disabled for now util it works correctly
            //return false;
        }
        return true;
    }

    if (type_is_arithmetic(argument_type) && type_is_enum(paramer_type))
    {
        return true;
    }

    if (type_is_enum(argument_type) && type_is_arithmetic(paramer_type))
    {
        return true;
    }


    //disabled for now util it works correctly
    //return false;
    return true;
}

bool type_is_function(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_FUNCTION;        
}

bool type_is_function_or_function_pointer(struct type* p_type)
{
    if (p_type->declarator_type == NULL)
        return false;


    if (
        p_type->declarator_type->direct_declarator_type &&
        p_type->declarator_type->direct_declarator_type->function_declarator_type)
    {
        return true;
    }

    return false;
}

struct type get_address_of_type(struct type* p_type)
{
    //type_print(p_type);
    struct type r = type_copy(p_type);
    struct declarator_type* p = find_inner_declarator(r.declarator_type);
    if (p == NULL)
    {
        assert(false);
    }

    if (p->direct_declarator_type && p->direct_declarator_type->function_declarator_type)
    {   
        struct direct_declarator_type* pdirect_declarator_type = calloc(1, sizeof * pdirect_declarator_type);
        struct declarator_type* p2 = calloc(1, sizeof * p);        
        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        list_add(&p2->pointers, p_pointer_type);
        pdirect_declarator_type->declarator_opt = p2;

        p->direct_declarator_type->function_declarator_type->direct_declarator_type = pdirect_declarator_type;
    }
    else if (p->direct_declarator_type && p->direct_declarator_type->array_declarator_type)
    {
        struct direct_declarator_type* pdirect_declarator_type = calloc(1, sizeof * pdirect_declarator_type);
        struct declarator_type* p2 = calloc(1, sizeof * p);
        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        list_add(&p2->pointers, p_pointer_type);

        pdirect_declarator_type->declarator_opt = p2;

        p->direct_declarator_type->array_declarator_type->direct_declarator_type = pdirect_declarator_type;
    }
    else
    {
        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        list_add(&p->pointers, p_pointer_type);
    }

    //type_print(&r);

    return r;
}

struct type get_pointer_content_type(struct type* p_type)
{
    struct type r = type_copy(p_type);
    struct declarator_type* p_inner_declarator = find_inner_declarator(r.declarator_type);
    if (p_inner_declarator->pointers.head != NULL)
    {
        pointer_type_list_pop_front(&p_inner_declarator->pointers);
    }
    else
    {
        //parser_seterror_with_token(ctx, ctx->current, "indirection requires pointer operand");
    }
    return r;
}


struct type get_array_item_type(struct type* p_type)
{
    assert(type_is_array(p_type));

    struct type r = type_copy(p_type);

    if (r.declarator_type)
    {
        struct array_declarator_type* removed = r.declarator_type->direct_declarator_type->array_declarator_type;
        r.declarator_type->direct_declarator_type = removed->direct_declarator_type;
        removed->direct_declarator_type = NULL; /*MOVED*/
        //array_declarator_type
    }

    return r;
}


void print_declarator_description(struct osstream* ss, struct declarator_type* declarator);
void print_direct_declarator_description(struct osstream* ss, struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt)
        print_declarator_description(ss, p_direct_declarator_type->declarator_opt);

    if (p_direct_declarator_type->function_declarator_type)
    {
        ss_fprintf(ss, " function returning ");
        print_direct_declarator_description(ss, p_direct_declarator_type->function_declarator_type->direct_declarator_type);
    }
    else if (p_direct_declarator_type->array_declarator_type)
    {
        ss_fprintf(ss, "array [%d] of ", p_direct_declarator_type->array_declarator_type->constant_size);
        print_direct_declarator_description(ss, p_direct_declarator_type->array_declarator_type->direct_declarator_type);
    }
}

void print_declarator_description(struct osstream* ss, struct declarator_type* declarator)
{
    if (declarator->direct_declarator_type)
        print_direct_declarator_description(ss, declarator->direct_declarator_type);

    struct pointer_type* p = declarator->pointers.head;
    while (p)
    {
        ss_fprintf(ss, " pointer to ");
        p = p->next;
    }
}



struct type get_function_return_type(struct type* p_type)
{
    //assert(type_is_function(p_type));

    struct type r = type_copy(p_type);

    if (r.declarator_type)
    {
        struct function_declarator_type* removed = r.declarator_type->direct_declarator_type->function_declarator_type;

        r.declarator_type->direct_declarator_type =
            r.declarator_type->direct_declarator_type->function_declarator_type->direct_declarator_type;        
    }

    return r;
}


bool type_is_pointer_or_array(struct type* p_type)
{
    if (p_type->declarator_type == NULL)
        return false;

    struct declarator_type* p_inner_declarator = find_inner_declarator(p_type->declarator_type);

    if (p_inner_declarator &&
        p_inner_declarator->pointers.head != NULL)
    {
        return true;
    }

    if (p_inner_declarator->direct_declarator_type &&
        p_inner_declarator->direct_declarator_type->array_declarator_type)
    {        
            /*arrays sao equivalentes a ponteiros em C*/
        
            return true;        
    }

    if (p_inner_declarator->direct_declarator_type &&
        p_inner_declarator->direct_declarator_type->function_declarator_type)
    {
        /*funcoes sao equivalentes a ponteiros em C*/
        return true;
    }

    return false;
}


int type_get_rank(struct type* p_type1, struct error* error)
{
    if (type_is_pointer_or_array(p_type1))
    {
        return 40;
    }

    int rank = 0;
    if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_BOOL))
    {
        rank = 10;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_CHAR) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_INT8))
    {
        rank = 20;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_SHORT) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_INT16))
    {
        rank = 30;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_INT) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_ENUM))
    {
        rank = 40;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_LONG) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_INT32))
    {
        rank = 50;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_FLOAT))
    {
        rank = 60;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_DOUBLE))
    {
        rank = 70;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_INT64))
    {
        rank = 80;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION))
    {
        seterror(error, "internal error - struct is not valid for rank");
    }
    else
    {
        seterror(error, "unexpected type for rank");
    }
    return rank;
}

struct type type_common(struct type* p_type1, struct type* p_type2, struct error* error)
{
    try
    {
        int rank_left = type_get_rank(p_type1, error);
        if (error) throw;

        int rank_right = type_get_rank(p_type1, error);
        if (error) throw;


        if (rank_left >= rank_right)
            return type_copy(p_type1);
        else
            return type_copy(p_type2);
    }
    catch
    {
    }

    return type_copy(p_type1);
}

/*retorna uma cópia do tipo*/
struct type type_copy(struct type* p_type)
{
    struct type r = { 0 };
    r.attributes_flags = p_type->attributes_flags;
    r.type_qualifier_flags = p_type->type_qualifier_flags;
    r.type_specifier_flags = p_type->type_specifier_flags;
    r.struct_or_union_specifier = p_type->struct_or_union_specifier;
    r.enum_specifier = p_type->enum_specifier;
    //if (p_type->declarator_name_opt)
      //r.declarator_name_opt = strdup(p_type->declarator_name_opt);

    if (p_type->declarator_type)
    {
        r.declarator_type = declarator_type_copy(p_type->declarator_type);
    }
    return r;
}



int get_array_size(struct type* p_type, struct error* error)
{
    if (type_is_array(p_type))
    {
        return p_type->declarator_type->direct_declarator_type->array_declarator_type->constant_size;
    }
    else
    {
        assert(false);
    }
    return 0;
}
int type_get_sizeof(struct type* p_type, struct error* error);
int get_sizeof_struct(struct struct_or_union_specifier* complete_struct_or_union_specifier, struct error* error)
{
    int maxalign = 0;
    int size = 0;
    struct member_declaration* d = complete_struct_or_union_specifier->member_declaration_list.head;
    while (d)
    {
        if (d->member_declarator_list_opt)
        {
            struct member_declarator* md = d->member_declarator_list_opt->head;
            while (md)
            {                
                int align = type_get_alignof(&md->declarator->type, error);

                if (align > maxalign)
                {
                    maxalign = align;
                }
                if (size % align != 0)
                {
                    size += align - (size % align);
                }

                size += type_get_sizeof(&md->declarator->type, error);                
                md = md->next;
            }
        }
        d = d->next;
    }

    if (size % maxalign != 0)
    {
        size += maxalign - (size % maxalign);
    }

    return size;
}
int type_get_alignof(struct type* p_type, struct error* error);
int get_alignof_struct(struct struct_or_union_specifier* complete_struct_or_union_specifier, struct error* error)
{
    int align = 0;
    struct member_declaration* d = complete_struct_or_union_specifier->member_declaration_list.head;
    while (d)
    {
        if (d->member_declarator_list_opt)
        {
            struct member_declarator* md = d->member_declarator_list_opt->head;
            while (md)
            {
                //TODO padding
                int temp_align = type_get_alignof(&md->declarator->type, error);
                if (temp_align > align)
                {
                    align = temp_align;
                }
                md = md->next;
            }
        }
        d = d->next;
    }
    return align;
}

int type_get_alignof(struct type* p_type, struct error* error)
{
    size_t align = 0;

    enum type_category category = find_type_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        align = _Alignof(void*);
    }
    else if (category == TYPE_CATEGORY_FUNCTION)
    {
        seterror(error, "sizeof function");
    }
    else if (category == TYPE_CATEGORY_ITSELF)
    {
        if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
        {
            align = _Alignof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        {
            align = _Alignof(_Bool);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        {
            align = _Alignof(short);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT)
        {
            align = _Alignof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            //TODO enum type
            align = _Alignof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            align = _Alignof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        {
            align = _Alignof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT64)
        {
            align = _Alignof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT32)
        {
            align = _Alignof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT16)
        {
            align = _Alignof(short);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT8)
        {
            align = _Alignof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        {
            align = _Alignof(float);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        {
            align = _Alignof(double);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            align = 1;
            if (p_type->struct_or_union_specifier->complete_struct_or_union_specifier)
            {
                align = get_alignof_struct(p_type->struct_or_union_specifier->complete_struct_or_union_specifier, error);
            }
            else
            {
                seterror(error, "invalid application of 'sizeof' to incomplete type 'struct %s'", p_type->struct_or_union_specifier->tag_name);
            }
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            align = _Alignof(int);
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            seterror(error, "type information is missing");
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
        {
            align = 1;
        }
        else
        {
            assert(false);
        }
    }
    else if (category == TYPE_CATEGORY_ARRAY)
    {
        
        struct type type = get_array_item_type(p_type);
        align = type_get_alignof(&type, error);
        type_destroy(&type);
    }

    return align;
}

int type_get_sizeof( struct type* p_type, struct error* error)
{
    size_t size = 0;

    enum type_category category = find_type_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        size = sizeof(void*);
    }
    else if (category == TYPE_CATEGORY_FUNCTION)
    {
        seterror(error, "sizeof function");
    }
    else if (category == TYPE_CATEGORY_ITSELF)
    {
        if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
        {
            size = sizeof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        {
            size = sizeof(_Bool);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        {
            size = sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT)
        {
            size = sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            //TODO enum type
            size = sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            size = sizeof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        {
            size = sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT64)
        {
            size = sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT32)
        {
            size = 4;
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT16)
        {
            size = 2;
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT8)
        {
            size = 1;
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        {
            size = sizeof(float);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        {
            size = sizeof(double);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            size = 1;
            if (p_type->struct_or_union_specifier->complete_struct_or_union_specifier)
            {
                size = get_sizeof_struct(p_type->struct_or_union_specifier->complete_struct_or_union_specifier, error);
            }
            else
            {
                seterror(error, "invalid application of 'sizeof' to incomplete type 'struct %s'", p_type->struct_or_union_specifier->tag_name);
            }
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            size = sizeof(int);
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            seterror(error, "type information is missing");            
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
        {
            size = 1;
        }
        else
        {
            assert(false);
        }
    }
    else if (category == TYPE_CATEGORY_ARRAY)
    {
        int arraysize = get_array_size(p_type, error);
        struct type type = get_array_item_type(p_type);
        int sz = type_get_sizeof(&type, error);
        size = sz * arraysize;
        type_destroy(&type);
    }

    return size;
}

unsigned int type_get_hashof(struct parser_ctx* ctx, struct type* p_type, struct error* error)
{
    unsigned int hash = 0;

    try
    {

        if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
        {
            //size = sizeof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        {
            //size = sizeof(_Bool);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        {
            //size = sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT)
        {
            //size = sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            //size = sizeof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        {
            //size = sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT64)
        {
            //size = sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT32)
        {
            //size = sizeof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT16)
        {
            //size = sizeof(short);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT8)
        {
            //size = sizeof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        {
            //size = sizeof(double);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            struct osstream ss = { 0 };

            struct struct_or_union_specifier* p_struct_or_union_specifier =
                p_type->struct_or_union_specifier;

            if (p_struct_or_union_specifier->member_declaration_list.head == NULL)
            {
                p_struct_or_union_specifier =
                    p_type->struct_or_union_specifier->complete_struct_or_union_specifier;
            }

            struct token* current = p_struct_or_union_specifier->first_token;
            for (;
                current != p_struct_or_union_specifier->last_token->next;
                current = current->next)
            {
                if (current->flags & TK_FLAG_FINAL)
                {
                    ss_fprintf(&ss, "%s", current->lexeme);

                }
            }
            hash = stringhash(ss.c_str);
            ss_close(&ss);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            //size = sizeof(int);
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            seterror(error, "type information is missing");
            throw;
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_TYPEOF)
        {
            //s//ize = 1; //TODO
            //assert(false);
            //;; size =
                //  type_get_sizeof(ctx, struct type* p_type, struct error* error)
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
        {
            if (p_type->declarator_type &&
                p_type->declarator_type->pointers.head != NULL)
            {
                // size = sizeof(void*);
            }
            else
            {
                seterror(error, "invalid application of 'sizeof' to a void type");
                throw;
            }
        }
        else
        {
            assert(false);
        }        
    }
    catch
    {
    }

    return hash;
}


void type_set_attributes(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->attributes_flags =
            pdeclarator->declaration_specifiers->attributes_flags;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        //p_type->type_qualifier_flags =
          //  pdeclarator->specifier_qualifier_list->ATR;
    }
}


void type_set_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->type_qualifier_flags =
            pdeclarator->declaration_specifiers->type_qualifier_flags;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        p_type->type_qualifier_flags =
            pdeclarator->specifier_qualifier_list->type_qualifier_flags;
    }
}

void type_set_specifiers_using_declarator(struct parser_ctx* ctx, struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->type_specifier_flags =
            pdeclarator->declaration_specifiers->type_specifier_flags;

        p_type->enum_specifier = pdeclarator->declaration_specifiers->enum_specifier;
        p_type->struct_or_union_specifier = pdeclarator->declaration_specifiers->struct_or_union_specifier;

    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        p_type->type_specifier_flags =
            pdeclarator->specifier_qualifier_list->type_specifier_flags;
        p_type->enum_specifier = pdeclarator->specifier_qualifier_list->enum_specifier;
        p_type->struct_or_union_specifier = pdeclarator->specifier_qualifier_list->struct_or_union_specifier;

    }
}


struct pointer_type_list clone_pointer_to_pointer_type_list(struct pointer* p_pointer)
{
    struct pointer_type_list r = { 0 };
    if (p_pointer == NULL)
    {
        return r;
    }
    struct pointer* p = p_pointer;
    while (p)
    {
        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        if (p_pointer->type_qualifier_list_opt)
            p_pointer_type->type_qualifier_flags = p_pointer->type_qualifier_list_opt->flags;
        list_add(&r, p_pointer_type);
        p = p->pointer;
    }
    return r;
}



struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);

struct array_declarator_type* clone_array_declarator_to_array_declarator_type(struct parser_ctx* ctx, struct array_declarator* p_array_declarator)
{
    struct array_declarator_type* p_array_declarator_type = calloc(1, sizeof(struct array_declarator_type));

    p_array_declarator_type->direct_declarator_type =
        clone_direct_declarator_to_direct_declarator_type(ctx, p_array_declarator->direct_declarator);

    //struct parameter_declaration* p_parameter_declaration = NULL;


    p_array_declarator_type->constant_size = p_array_declarator->constant_size;

    return p_array_declarator_type;
}

struct function_declarator_type* clone_function_declarator_to_function_declarator_type(struct parser_ctx* ctx, struct function_declarator* p_function_declarator)
{
    struct function_declarator_type* p_function_declarator_type = calloc(1, sizeof(struct function_declarator_type));

    p_function_declarator_type->direct_declarator_type =
        clone_direct_declarator_to_direct_declarator_type(ctx, p_function_declarator->direct_declarator);

    struct parameter_declaration* p_parameter_declaration = NULL;
    if (p_function_declarator->parameter_type_list_opt)
    {
        p_parameter_declaration = p_function_declarator->parameter_type_list_opt->parameter_list->head;
    }


    if (p_function_declarator->parameter_type_list_opt)
    {
        p_function_declarator_type->is_var_args = p_function_declarator->parameter_type_list_opt->is_var_args;
    }
    
    //TODO ...
    

    while (p_parameter_declaration)
    {
        struct type* p_type = calloc(1, sizeof(struct type));
        *p_type = make_type_using_declarator(ctx, p_parameter_declaration->declarator);

        /*name of the argument*/
        //free(p_type->declarator_name_opt);
        //if (p_parameter_declaration->name)
          //  p_type->declarator_name_opt = strdup(p_parameter_declaration->name->lexeme);

        list_add(&p_function_declarator_type->params, p_type);
        p_parameter_declaration = p_parameter_declaration->next;
    }

    
    return p_function_declarator_type;
}

struct direct_declarator_type* clone_direct_declarator_to_direct_declarator_type(struct parser_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator == NULL)
    {
        return NULL;
    }

    struct direct_declarator_type* p_direct_declarator_type = calloc(1, sizeof(struct direct_declarator_type));

    if (p_direct_declarator->name_opt)
    {
        p_direct_declarator_type->name_opt = strdup(p_direct_declarator->name_opt->lexeme);
    }

    if (p_direct_declarator->declarator)
    {
        p_direct_declarator_type->declarator_opt = clone_declarator_to_declarator_type(ctx, p_direct_declarator->declarator);
    }

    if (p_direct_declarator->array_declarator)
    {
        p_direct_declarator_type->array_declarator_type =
            clone_array_declarator_to_array_declarator_type(ctx, p_direct_declarator->array_declarator);
    }
    
    if (p_direct_declarator->function_declarator)
    {
        p_direct_declarator_type->function_declarator_type=
            clone_function_declarator_to_function_declarator_type(ctx, p_direct_declarator->function_declarator);
    }


    return p_direct_declarator_type;
}

struct declarator_type* clone_declarator_to_declarator_type(struct parser_ctx* ctx, struct declarator* p_declarator)
{
    if (p_declarator == NULL)
    {
        return NULL;
    }
    struct declarator_type* p_declarator_type = calloc(1, sizeof(struct declarator_type));
    
    p_declarator_type->pointers = clone_pointer_to_pointer_type_list(p_declarator->pointer);
    p_declarator_type->direct_declarator_type = clone_direct_declarator_to_direct_declarator_type(ctx, p_declarator->direct_declarator);

    return p_declarator_type;
}
bool is_empty_declarator_type(struct declarator_type* p_declarator_type)
{
    return
        p_declarator_type->pointers.head == NULL &&
        p_declarator_type->direct_declarator_type->declarator_opt == NULL &&
        p_declarator_type->direct_declarator_type->function_declarator_type == NULL &&
        p_declarator_type->direct_declarator_type->array_declarator_type == NULL;
}

static void direct_declarator_type_clear_name(struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type == NULL)
        return;

    if (p_direct_declarator_type->declarator_opt)
    {
        direct_declarator_type_clear_name(p_direct_declarator_type->declarator_opt->direct_declarator_type);
    }

    if (p_direct_declarator_type->name_opt)
    {
        free((void*)p_direct_declarator_type->name_opt);
        p_direct_declarator_type->name_opt = NULL;
    }
    else if (p_direct_declarator_type->array_declarator_type)
    {
        direct_declarator_type_clear_name(p_direct_declarator_type->array_declarator_type->direct_declarator_type);
    }
    else if (p_direct_declarator_type->function_declarator_type)
    {
        direct_declarator_type_clear_name(p_direct_declarator_type->function_declarator_type->direct_declarator_type);
    }

}

void declarator_type_clear_name(struct declarator_type* p_declarator_type_opt)
{           
    if (p_declarator_type_opt == NULL)
        return;

     direct_declarator_type_clear_name(p_declarator_type_opt->direct_declarator_type);
}


void declarator_type_merge(struct declarator_type* p_declarator_typet1, struct declarator_type* p_typedef_decl0)
{
    struct declarator_type* p_typedef_decl = find_inner_declarator(p_typedef_decl0);

    if (p_typedef_decl)
    {
        /*
        typedef int A[2];
        typedef A *B [1];
        =>int (* [1]) [2]


        typedef int A[2];
        typedef A *B ;
        =>int (*) [2]

        typedef int *A;
        typedef A *B;
        =>int**

        typedef int *A;
        typedef A B[1] ;
        =>int* [1]

        */
        
        if (p_typedef_decl->direct_declarator_type &&
            (p_typedef_decl->direct_declarator_type->array_declarator_type ||
            p_typedef_decl->direct_declarator_type->function_declarator_type))
        {            
            struct declarator_type* copy =
                declarator_type_copy(p_declarator_typet1);

            p_typedef_decl->direct_declarator_type->declarator_opt = copy;                
        }
        else
        {
            struct declarator_type* copy = declarator_type_copy(p_declarator_typet1);
            
            struct pointer_type* p = copy->pointers.head;
            while (p)
            {
                struct pointer_type* next = p->next;
                list_add(&p_typedef_decl->pointers, p);
                p = next;
            }

            //free(copy->direct_declarator_type->name_opt);
            //copy->direct_declarator_type->name_opt = NULL;

            p_typedef_decl->direct_declarator_type = copy->direct_declarator_type;
            copy->direct_declarator_type = NULL; //mOVED?

            
            //p_typedef_de//cl->direct_declarator_type->declarator_opt = copy->direct_declarator_type->declarator_opt;
        }


        //p_typedef_decl->direct_declarator_type->declarator_opt =
          //  declarator_type_copy(t1->declarator_type);
    }
}

#if 0    
/*this sample is useful to try in compiler explorer*/
#include <stdio.h>
#include <typeinfo>
#include <cxxabi.h>

int status;
#define TYPE(EXPR) \
 printf("%s=", #EXPR); \
 printf("%s\n", abi::__cxa_demangle(typeid(typeof(EXPR)).name(),0,0,&status))


typedef char* T1;
typedef const T1 CONST_T1;
typedef CONST_T1 T2[5];
typedef T2 T3[2];

int main()
{
    TYPE(T1);    
}
#endif
/*

typedef char *T;
T a[2]; //char * [2]

typedef char *T[1];
T a[2]; // char* [2][1]

typedef char (*PF)(void);
PF a[2]; //char (* [2])()

typedef char *T;
T (*a)(void); //char* (*)()

typedef char const *T;
T (*a)(void); //char const* (*)()

typedef char (*PF)(void);
    PF (*a)(void); //char (*(*)())()

typedef char (*PF)(double);
    PF (*a)(int); //char (*(*)(int))(double)

 typedef char (*PF)(double);
 const PF (*a)(int); //char (* const (*)(int))(double)

*/

struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator)
{
    struct type t = { 0 };

    //if (pdeclarator->name)
    //{
      //  t.declarator_name_opt = strdup(pdeclarator->name->lexeme);
    //}

    if (pdeclarator->specifier_qualifier_list)
    {

        

        if (pdeclarator->specifier_qualifier_list->typeof_specifier)
        {
         
            if (pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->expression)
            {
                t = type_copy(&pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->expression->type);
                struct declarator_type* dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
                declarator_type_merge(dectype, t.declarator_type);
                
            }
            else if (pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->type_name)
            {
                t = type_copy(&pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->type_name->declarator->type);
                struct declarator_type* dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
                declarator_type_merge(dectype, t.declarator_type);

                
            }
        }
        else  if (pdeclarator->specifier_qualifier_list->typedef_declarator)
        {
            t = type_copy(&pdeclarator->specifier_qualifier_list->typedef_declarator->type);
            struct declarator_type* dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
            declarator_type_merge(dectype, t.declarator_type);

        }
        else
        {
            type_set_attributes(&t, pdeclarator);
            type_set_qualifiers_using_declarator(&t, pdeclarator);
            type_set_specifiers_using_declarator(ctx, &t, pdeclarator);
            t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator);
        }
    }
    else if (pdeclarator->declaration_specifiers)
    {
        if (pdeclarator->declaration_specifiers->typeof_specifier)
        {
            if (pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->expression)
            {
                t = type_copy(&pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->expression->type);                
                
                declarator_type_clear_name(t.declarator_type);

                struct declarator_type* dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
                
                if (t.declarator_type != NULL) /*expression it may be null*/
                {
                    
                    declarator_type_merge(dectype, t.declarator_type);
                }
                else
                {
                    t.declarator_type = dectype;
                }          
            }
            else if (pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name)
            {
                t = type_copy(&pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name->declarator->type);
                
                struct declarator_type *dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
                
                if (t.declarator_type != NULL) /*expression it may be null*/
                {
                    declarator_type_merge(dectype, t.declarator_type);
                }
                else
                {
                    /*works but unexpected*/                    
                    t.declarator_type = dectype;
                    assert(false);
                }
            }
        }
        else if (pdeclarator->declaration_specifiers->typedef_declarator)
        {
            t = type_copy(&pdeclarator->declaration_specifiers->typedef_declarator->type);

            //t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name->declarator);
            struct declarator_type* dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
            declarator_type_merge(dectype, t.declarator_type);

            //type_set_qualifiers_using_declarator(&t, pdeclarator->declaration_specifiers->typedef_declarator);
            //type_set_specifiers_using_declarator(ctx, &t, pdeclarator->declaration_specifiers->typedef_declarator);

            //t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator->declaration_specifiers->typedef_declarator);
            //struct declarator_type* dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
            //declarator_type_merge(dectype, t.declarator_type);
            //declarator_type_delete(dectype);
            //debug_print_declarator_type(t.declarator_type, 0);
        }
        else
        {
            type_set_attributes(&t, pdeclarator);
            type_set_qualifiers_using_declarator(&t, pdeclarator);
            type_set_specifiers_using_declarator(ctx, &t, pdeclarator);
            t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator);
        }
    }



    return t;
}

struct type make_type_using_declarator_do_not_expand(struct parser_ctx* ctx, struct declarator* pdeclarator)
{
    struct type t = { 0 };
    memset(&t, 0, sizeof t);
    type_set_qualifiers_using_declarator(&t, pdeclarator);
    type_set_specifiers_using_declarator(ctx, &t, pdeclarator);
    type_set_attributes(&t, pdeclarator);
    t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator);
    return t;
}

struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type);

struct declarator_type* direct_declarator_find_inner_declarator(struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type)
    {
        if (p_direct_declarator_type->declarator_opt)
        {
            return find_inner_declarator(p_direct_declarator_type->declarator_opt);
        }
        else if (p_direct_declarator_type->function_declarator_type)
        {
            return direct_declarator_find_inner_declarator(p_direct_declarator_type->function_declarator_type->direct_declarator_type);
        }
        else if (p_direct_declarator_type->array_declarator_type)
        {
            return direct_declarator_find_inner_declarator(p_direct_declarator_type->array_declarator_type->direct_declarator_type);
        }
    }
    return NULL;// p_direct_declarator_type->declarator_opt;
}

struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type)
{

    if (p_declarator_type == NULL ||
        p_declarator_type->direct_declarator_type == NULL)
        return p_declarator_type;

    if (p_declarator_type->direct_declarator_type->declarator_opt == NULL &&
        p_declarator_type->direct_declarator_type->function_declarator_type == NULL &&
        p_declarator_type->direct_declarator_type->array_declarator_type == NULL)
    {
        assert(p_declarator_type != NULL);
        return p_declarator_type;
    }

    struct declarator_type* p2 = direct_declarator_find_inner_declarator(p_declarator_type->direct_declarator_type);
    if (p2 != NULL)
    {
        p_declarator_type = p2;        
    }

    return p_declarator_type;
}

void type_set_int(struct type* p_type)
{
    p_type->type_specifier_flags = TYPE_SPECIFIER_INT;
    p_type->type_qualifier_flags = 0;
    p_type->declarator_type = NULL;
}


bool pointer_type_is_same(struct pointer_type* a, struct pointer_type* b)
{
    if (a && b)
    {
        if (a->type_qualifier_flags != b->type_qualifier_flags)
            return false;

        return true;
    }
    return a == NULL && b == NULL;
}

bool pointer_type_list_is_same(struct pointer_type_list* a, struct pointer_type_list* b)
{
    if (a && b)
    {
        struct pointer_type* pa = a->head;
        struct pointer_type* pb = b->head;

        if (pa && pb)
        {
            while (pa && pb)
            {
                if (!pointer_type_is_same(pa, pb))
                    return false;
                pa = pa->next;
                pb = pb->next;
            }
            return pa == NULL && pb == NULL;
        }
        return true;
    }

    return a == NULL && b == NULL;
}

bool type_is_same(struct type* a, struct type* b, bool compare_qualifiers);
bool type_list_is_same(struct params* a, struct params* b)
{
    if (a && b)
    {
        struct type* pa = a->head;
        struct type* pb = b->head;

        if (pa && pb)
        {
            while (pa && pb)
            {
                if (!type_is_same(pa, pb, true))
                    return false;
                pa = pa->next;
                pb = pb->next;
            }
            return pa == NULL && pb == NULL;
        }
        return pa == NULL && pb == NULL;

    }
    return a == NULL && b == NULL;
}



bool declarator_type_is_same(struct declarator_type* a, struct declarator_type* b);

bool direct_declarator_type_is_empty(struct direct_declarator_type* a)
{
    if (a == NULL)
        return true;

    if (a->declarator_opt != NULL)
    {
        return false;
    }

    if (a->array_declarator_type != NULL)
    {
        return false;
    }

    if (a->function_declarator_type != NULL)
    {
        return false;
    }

    return true;
}



bool array_declarator_type_is_same(struct array_declarator_type* a, struct array_declarator_type* b)
{
    if (a && b)
    {
        if (!direct_declarator_type_is_same(a->direct_declarator_type, b->direct_declarator_type))
            return false;

        return a->constant_size == b->constant_size;
    }
    return a == b;
}

bool function_declarator_type_is_same(struct function_declarator_type* a, struct function_declarator_type* b)
{
    if (a && b)
    {
        if (!direct_declarator_type_is_same(a->direct_declarator_type, b->direct_declarator_type))
            return false;

        if (!type_list_is_same(&a->params, &b->params))
            return false;

        return a->is_var_args == b->is_var_args;
    }

    return a == b;
}

bool direct_declarator_type_is_same(struct direct_declarator_type* a, struct direct_declarator_type* b)
{
    if (a && b)
    {
        if (!direct_declarator_type_is_empty(a) && !direct_declarator_type_is_empty(b))
        {

            if (!array_declarator_type_is_same(a->array_declarator_type, b->array_declarator_type))
                return false;


            if (!function_declarator_type_is_same(a->function_declarator_type, b->function_declarator_type))
                return false;


            if (!declarator_type_is_same(a->declarator_opt, b->declarator_opt))
            {
                return false;
            }

            return true;
        }
        
    }
    return direct_declarator_type_is_empty(a) && direct_declarator_type_is_empty(b);
}

bool struct_or_union_specifier_is_same(struct struct_or_union_specifier* a, struct struct_or_union_specifier* b)
{
    if (a && b)
    {
        if (a->complete_struct_or_union_specifier != NULL && b->complete_struct_or_union_specifier != NULL)
        {
            if (a->complete_struct_or_union_specifier != b->complete_struct_or_union_specifier)
            {
                return false;
            }
            return true;
        }
        else
        {
            /*both incomplete then we compare tag names*/
            if (a->tagtoken != NULL && b->tagtoken != NULL)
            {
                if (strcmp(a->tagtoken->lexeme, b->tagtoken->lexeme) == 0)
                    return true;
            }
        }
        return a->complete_struct_or_union_specifier == NULL &&
            b->complete_struct_or_union_specifier == NULL;
    }
    return a == NULL && b == NULL;
}

bool enum_specifier_is_same(struct enum_specifier* a, struct enum_specifier* b)
{
    if (a && b)
    {
        if (a->complete_enum_specifier && b->complete_enum_specifier)
        {
            if (a->complete_enum_specifier != b->complete_enum_specifier)
                return false;
            return true;
        }
        return a->complete_enum_specifier == NULL &&
            b->complete_enum_specifier == NULL;
    }
    return a == NULL && b == NULL;
}

bool declarator_type_is_empty(struct declarator_type* a)
{
    if (a == NULL)
        return true;

    if (a->direct_declarator_type)
    {
        if (a->direct_declarator_type->array_declarator_type != NULL ||
            a->direct_declarator_type->function_declarator_type != NULL ||
            a->direct_declarator_type->declarator_opt != NULL)
        {
            return false;
        }
    }
    if (a->pointers.head != NULL)
    {
        return false;
    }

    return true;
}

bool declarator_type_is_same(struct declarator_type* a, struct declarator_type* b)
{
    if (!declarator_type_is_empty(a) && !declarator_type_is_empty(b))
    {
        if (!pointer_type_list_is_same(&a->pointers, &b->pointers))
            return false;
        if (!direct_declarator_type_is_same(a->direct_declarator_type, b->direct_declarator_type))
            return false;

        return true;
    }
    return declarator_type_is_empty(a) && declarator_type_is_empty(b);
}



bool type_is_same(struct type* a, struct type* b, bool compare_qualifiers)
{
    //debug
    //type_print(a);
    //type_print(b);
    //TODO compare_qualifiers inner declarator
    compare_qualifiers;

    if (a && b)
    {
        if (a->type_qualifier_flags != b->type_qualifier_flags)
        {
            return false;
        }

        if (a->type_specifier_flags != b->type_specifier_flags)
        {
            return false;
        }

        if (!declarator_type_is_same(a->declarator_type, b->declarator_type))
        {
            return false;
        }

        if (!enum_specifier_is_same(a->enum_specifier, b->enum_specifier))
        {
            return false;
        }

        if (!struct_or_union_specifier_is_same(a->struct_or_union_specifier, b->struct_or_union_specifier))
        {
            return false;
        }

        return true;
    }

    return a == NULL && b == NULL;
}

