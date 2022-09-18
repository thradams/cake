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



void print_direct_declarator_type(struct osstream* ss, struct direct_declarator_type* type, const char* name);

void print_declarator_type(struct osstream* ss, struct declarator_type* p_declarator_type, const char* name)
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
        print_direct_declarator_type(ss, p_declarator_type->direct_declarator_type, name);
    }


}

/*in case we need to print in console to debug*/
void debug_print_declarator_type(struct declarator_type* p_declarator_type, const char* name)
{
    struct osstream ss = { 0 };
    print_declarator_type(&ss, p_declarator_type, name);
    printf("%s\n", ss.c_str);
    ss_close(&ss);
}

void print_direct_declarator_type(struct osstream* ss,
    struct direct_declarator_type* p_direct_declarator_type, 
    const char* name_opt)
{
    if (p_direct_declarator_type->declarator_opt)
    {
        ss_fprintf(ss, "(");
        print_declarator_type(ss, p_direct_declarator_type->declarator_opt, name_opt);
        ss_fprintf(ss, ")");
    }
    else
    {
        if (name_opt)
            ss_fprintf(ss, "%s", name_opt);
    }
    struct array_function_type* p_array_function_type =
        p_direct_declarator_type->array_function_type_list.head;
    for (; p_array_function_type; p_array_function_type = p_array_function_type->next)
    {
        if (p_array_function_type->bIsArray)
        {
            ss_fprintf(ss, "[%d]", p_array_function_type->array_size);
        }
        else if (p_array_function_type->bIsFunction)
        {
            ss_fprintf(ss, "(");
            struct type* param = p_array_function_type->params.head;
            while (param)
            {
                if (param != p_array_function_type->params.head)
                    ss_fprintf(ss, ",");
                print_type(ss, param);
                param = param->next;
            }
            if (p_array_function_type->bVarArg)
                ss_fprintf(ss, ",...");

            ss_fprintf(ss, ")");
        }
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
    print_declarator_type(ss, type->declarator_type, /*name*/NULL);
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



struct type_list params_copy(struct type_list* input)
{
    struct type_list r = { 0 };
    struct type* p_param_type = input->head;
    while (p_param_type)
    {
        struct type* par = calloc(1, sizeof * par);
        par->type_qualifier_flags = p_param_type->type_qualifier_flags;
        par->type_specifier_flags = p_param_type->type_specifier_flags;

        par->enum_specifier = p_param_type->enum_specifier;
        par->struct_or_union_specifier = p_param_type->struct_or_union_specifier;
        par->declarator_type = declarator_type_copy(p_param_type->declarator_type);
        list_add(&r, par);
        p_param_type = p_param_type->next;
    }
    return r;
}

struct array_function_type_list array_function_type_list_copy(struct array_function_type_list* p_array_function_type_list)
{
    struct array_function_type_list list = { 0 };
    struct array_function_type* p = p_array_function_type_list->head;
    while (p)
    {
        struct array_function_type* p_array_function_type = calloc(1, sizeof(struct array_function_type));
        p_array_function_type->array_size = p->array_size;
        p_array_function_type->bIsArray = p->bIsArray;
        p_array_function_type->bIsFunction = p->bIsFunction;
        p_array_function_type->bVarArg = p->bVarArg;
        p_array_function_type->params = params_copy(&p->params);
        list_add(&list, p_array_function_type);
        p = p->next;
    }
    return list;
}



struct direct_declarator_type* direct_declarator_type_copy(struct direct_declarator_type* p_direct_declarator_type_opt)
{
    if (p_direct_declarator_type_opt == NULL)
        return NULL;
    struct direct_declarator_type* p_direct_declarator_type = calloc(1, sizeof(struct direct_declarator_type));
    p_direct_declarator_type->declarator_opt = declarator_type_copy(p_direct_declarator_type_opt->declarator_opt);
    p_direct_declarator_type->array_function_type_list = array_function_type_list_copy(&p_direct_declarator_type_opt->array_function_type_list);
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

    struct array_function_type* p = p_direct_declarator_type->array_function_type_list.head;
    while (p)
    {
        if (p->bIsFunction)
        {
            if (*type_category == TYPE_CATEGORY_ITSELF)
            {
                *type_category = TYPE_CATEGORY_FUNCTION;
                break;
            }
        }
        else if (p->bIsArray)
        {
            if (*type_category == TYPE_CATEGORY_ITSELF)
            {
                *type_category = TYPE_CATEGORY_ARRAY; /*array*/
                break;
            }
        }
        p = p->next;
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

/*
  The type char, the signed and unsigned integer types, and the enumerated types are collectively
  called integer types. The integer and real floating types are collectively called real types
*/
bool type_is_integer(struct type* p_type)
{
    if (find_type_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

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

bool type_is_function_or_function_pointer(struct type* p_type)
{
    if (p_type->declarator_type == NULL)
        return false;


    if (
        p_type->declarator_type->direct_declarator_type &&
        p_type->declarator_type->direct_declarator_type->array_function_type_list.head &&
        p_type->declarator_type->direct_declarator_type->array_function_type_list.head->bIsFunction)
    {
        return true;
    }

    return false;
}

struct type get_address_of_type(struct type* p_type)
{
    struct type r = type_copy(p_type);
    struct declarator_type* p = find_inner_declarator(r.declarator_type);
    if (p == NULL)
    {
        assert(false);
    }

    if (p->direct_declarator_type &&
        p->direct_declarator_type->array_function_type_list.head &&
        (p->direct_declarator_type->array_function_type_list.head->bIsArray ||
            p->direct_declarator_type->array_function_type_list.head->bIsFunction))
    {
        struct declarator_type* p2 = calloc(1, sizeof * p);
        p->direct_declarator_type->declarator_opt = p2;

        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        list_add(&p2->pointers, p_pointer_type);
    }
    else
    {
        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        list_add(&p->pointers, p_pointer_type);
    }



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

    struct type r = { 0 };
    r.type_qualifier_flags = p_type->type_qualifier_flags;
    r.type_specifier_flags = p_type->type_specifier_flags;

    r.struct_or_union_specifier = p_type->struct_or_union_specifier;
    r.enum_specifier = p_type->enum_specifier;



    if (p_type->declarator_type)
    {
        r.declarator_type = declarator_type_copy(p_type->declarator_type);
        array_function_type_list_pop_back(&r.declarator_type->direct_declarator_type->array_function_type_list);
    }

    return r;
}

void print_declarator_description(struct osstream* ss, struct declarator_type* declarator);
void print_direct_declarator_description(struct osstream* ss, struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt)
        print_declarator_description(ss, p_direct_declarator_type->declarator_opt);

    struct array_function_type* p = p_direct_declarator_type->array_function_type_list.head;
    while (p)
    {
        if (p->bIsFunction)
        {
            ss_fprintf(ss, " function returning ");
        }
        else if (p->bIsArray)
        {
            ss_fprintf(ss, "array [%d] of ", p->array_size);
        }
        p = p->next;
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



void function_result_declarator(int* pop, struct declarator_type* declarator);
void function_result_ddeclarator(int* pop, struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt)
        function_result_declarator(pop, p_direct_declarator_type->declarator_opt);

    struct array_function_type* p = p_direct_declarator_type->array_function_type_list.head;
    while (p)
    {
        if (p->bIsFunction)
        {
            if (*pop == 0)
            {
                *pop = 1;
                struct array_function_type* removed =
                    array_function_type_list_pop_back(&p_direct_declarator_type->array_function_type_list);
                //array_function_type_delete(removed);
                break;
            }
        }
        p = p->next;
    }
}



void function_result_declarator(int* pop, struct declarator_type* declarator)
{
    if (declarator->direct_declarator_type)
        function_result_ddeclarator(pop, declarator->direct_declarator_type);
}

struct type get_function_return_type(struct type* p_type)
{
    struct type r = type_copy(p_type);
    int pop = 0;
    function_result_declarator(&pop, r.declarator_type);
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

    if (p_inner_declarator->direct_declarator_type->array_function_type_list.tail)
    {
        if (p_inner_declarator->direct_declarator_type->array_function_type_list.tail->bIsArray)
        {
            /*arrays sao equivalentes a ponteiros em C*/
            return true;
        }
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
    r.type_qualifier_flags = p_type->type_qualifier_flags;
    r.type_specifier_flags = p_type->type_specifier_flags;
    r.struct_or_union_specifier = p_type->struct_or_union_specifier;
    r.enum_specifier = p_type->enum_specifier;


    if (p_type->declarator_type)
    {
        r.declarator_type = declarator_type_copy(p_type->declarator_type);
    }
    return r;
}


struct array_function_type* array_function_type_list_pop_back(struct array_function_type_list* list)
{
    if (list == NULL)
        return NULL;
    if (list->head == list->tail)
    {
        struct array_function_type* p = list->head;
        list->head = NULL;
        list->tail = NULL;
        return p;
    }
    struct array_function_type* before_last = list->head;
    while (before_last->next->next)
    {
        before_last = before_last->next;
    }
    struct array_function_type* p = before_last->next;
    before_last->next = NULL;
    return p;
}

int type_get_sizeof(struct parser_ctx* ctx, struct type* p_type, struct error* error)
{
    size_t size = 0;

    try
    {
        if (p_type->declarator_type &&
            p_type->declarator_type->pointers.head != NULL)
        {
            size = sizeof(void*);
        }
        else {

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
                size = sizeof(long);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT16)
            {
                size = sizeof(short);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT8)
            {
                size = sizeof(char);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
            {
                size = sizeof(double);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
            {
                size = 1;       //TODO
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
            {
                size = sizeof(int);
            }
            else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
            {
                seterror(error, "type information is missing");
                throw;
            }
            else if (p_type->type_specifier_flags == TYPE_SPECIFIER_TYPEOF)
            {
                size = 1; //TODO
                //assert(false);
                //;; size =
                    //  type_get_sizeof(ctx, struct type* p_type, struct error* error)
            }
            else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
            {
                //
            }
            else
            {
                assert(false);
            }

        }

        //Multiplica size pelo numero de elementos
        if (p_type->declarator_type &&
            p_type->declarator_type->direct_declarator_type)
        {
            struct array_function_type* p_array_function_type =
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head;
            while (p_array_function_type)
            {
                if (p_array_function_type->bIsArray)
                {
                    size = size * p_array_function_type->array_size;
                }
                p_array_function_type = p_array_function_type->next;
            }
        }
    }
    catch
    {
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

            struct token* current = p_struct_or_union_specifier->first;
            for (;
                current != p_struct_or_union_specifier->last->next;
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

        if (p_type->declarator_type &&
            p_type->declarator_type->direct_declarator_type)
        {
            struct array_function_type* p_array_function_type =
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head;
            while (p_array_function_type)
            {
                if (p_array_function_type->bIsArray)
                {
                    //size = size * p_array_function_type->constant_size;
                }
                p_array_function_type = p_array_function_type->next;
            }
        }
    }
    catch
    {
    }

    return hash;
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

struct declarator_type* clone_declarator_to_declarator_type(struct parser_ctx* ctx, struct declarator* p_declarator);

struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);

struct direct_declarator_type* clone_direct_declarator_to_direct_declarator_type(struct parser_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator == NULL)
    {
        return NULL;
    }
    struct direct_declarator_type* p_direct_declarator_type = calloc(1, sizeof(struct direct_declarator_type));
    if (p_direct_declarator->declarator)
    {
        p_direct_declarator_type->declarator_opt = clone_declarator_to_declarator_type(ctx, p_direct_declarator->declarator);
    }
    struct array_function* p_array_function = p_direct_declarator->array_function_list.head;
    while (p_array_function)
    {
        struct array_function_type* p_array_function_type = calloc(1, sizeof(struct array_function_type));
        if (p_array_function->function_declarator)
        {
            /*copy function argument type into params*/
            p_array_function_type->bIsFunction = true;



            struct parameter_declaration* p_parameter_declaration = NULL;

            if (p_array_function->function_declarator->parameter_type_list_opt != NULL)
            {
                p_array_function_type->bVarArg =
                    p_array_function->function_declarator->parameter_type_list_opt->is_var_args;

                p_parameter_declaration =
                    p_array_function->function_declarator->parameter_type_list_opt->parameter_list->head;

            }
            while (p_parameter_declaration)
            {
                struct type* p_type = calloc(1, sizeof(struct type));
                *p_type = make_type_using_declarator(ctx, p_parameter_declaration->declarator);
                //print_type(p_type);
                list_add(&p_array_function_type->params, p_type);
                p_parameter_declaration = p_parameter_declaration->next;
            }
        }
        else if (p_array_function->array_declarator)
        {
            p_array_function_type->array_size = p_array_function->array_declarator->constant_size;
            p_array_function_type->bIsArray = true;
        }
        else
        {
            assert(false);
        }
        list_add(&p_direct_declarator_type->array_function_type_list, p_array_function_type);
        p_array_function = p_array_function->next;
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
    //type_set_qualifiers_using_declarator(p_declarator_type, pdeclarator);
    //type_set_specifiers_using_declarator(declarator_type, pdeclarator);
    p_declarator_type->direct_declarator_type = clone_direct_declarator_to_direct_declarator_type(ctx, p_declarator->direct_declarator);
    p_declarator_type->pointers = clone_pointer_to_pointer_type_list(p_declarator->pointer);
    return p_declarator_type;
}
bool is_empty_declarator_type(struct declarator_type* p_declarator_type)
{
    return
        p_declarator_type->pointers.head == NULL &&
        p_declarator_type->direct_declarator_type->declarator_opt == NULL &&
        p_declarator_type->direct_declarator_type->array_function_type_list.head == NULL;
}

void declarator_type_merge(struct declarator_type* p_declarator_typet1, struct declarator_type* p_typedef_decl0)
{
    //struct type typedef_type_copy = type_copy(typedef_type);
   /*acho o nucleo do typedef*/
    struct declarator_type* p_typedef_decl = p_typedef_decl0;
    while (p_typedef_decl)
    {
        if (p_typedef_decl->direct_declarator_type->declarator_opt)
            p_typedef_decl = p_typedef_decl->direct_declarator_type->declarator_opt;
        else
            break;
    }

    //    typedef_type_copy->type_qualifier_flags = t1->type_qualifier_flags;

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
            p_typedef_decl->direct_declarator_type->array_function_type_list.head)
        {            
            p_typedef_decl->direct_declarator_type->declarator_opt =
                declarator_type_copy(p_declarator_typet1);
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

            struct array_function_type* p_array_function_type =
                copy->direct_declarator_type->array_function_type_list.head;

            while (p_array_function_type)
            {
                struct array_function_type* next = p_array_function_type->next;
                list_add(&p_typedef_decl->direct_declarator_type->array_function_type_list, p_array_function_type);
                p_array_function_type = next;
            }

            p_typedef_decl->direct_declarator_type->declarator_opt = copy->direct_declarator_type->declarator_opt;
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
    memset(&t, 0, sizeof t);

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
                struct declarator_type* dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
                declarator_type_merge(dectype, t.declarator_type);
            }
            else if (pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name)
            {
                t = type_copy(&pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name->declarator->type);
                
                struct declarator_type *dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
                declarator_type_merge(dectype, t.declarator_type);
                //declarator_type_delete(dectype);
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
    t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator);
    return t;
}

struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type)
{
    struct declarator_type* p = p_declarator_type;
    while (p)
    {
        if (p->direct_declarator_type &&
            p->direct_declarator_type->declarator_opt)
            p = p->direct_declarator_type->declarator_opt;
        else
            break;
    }
    return p;
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
bool type_list_is_same(struct type_list* a, struct type_list* b)
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

bool array_function_type_is_same(struct array_function_type* a, struct array_function_type* b)
{
    if (a && b)
    {
        if (a->array_size != b->array_size)
            return false;
        if (a->bIsArray != b->bIsArray)
            return false;
        if (a->bIsFunction != b->bIsFunction)
            return false;
        if (a->bVarArg && b->bVarArg)
            return false;

        if (a->array_size != b->array_size)
            return false;

        if (!type_list_is_same(&a->params, &b->params))
            return false;

        return true;
    }
    return a == NULL && b == NULL;
}

bool array_function_type_list_is_same(struct array_function_type_list* a, struct array_function_type_list* b)
{
    if (a && b)
    {
        struct array_function_type* pa = a->head;
        struct array_function_type* pb = b->head;
        if (pa && pb)
        {
            while (pa && pb)
            {
                if (!array_function_type_is_same(pa, pb))
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

    if (a->array_function_type_list.head != NULL)
    {
        return false;
    }

    return true;
}
bool direct_declarator_type_is_same(struct direct_declarator_type* a, struct direct_declarator_type* b)
{
    if (!direct_declarator_type_is_empty(a) && !direct_declarator_type_is_empty(b))
    {

        if (!array_function_type_list_is_same(&a->array_function_type_list, &b->array_function_type_list))
        {
            return false;
        }

        if (!declarator_type_is_same(a->declarator_opt, b->declarator_opt))
        {
            return false;
        }

        return true;
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
        if (a->direct_declarator_type->array_function_type_list.head != NULL ||
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

