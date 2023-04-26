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
bool direct_declarator_type_is_same(struct direct_declarator_type* a, struct direct_declarator_type* b, bool compare_qualifiers);
struct direct_declarator_type* clone_direct_declarator_to_direct_declarator_type(struct parser_ctx* ctx, struct direct_declarator* p_direct_declarator);

struct direct_declarator_type* direct_declarator_type_copy(struct direct_declarator_type* p_direct_declarator_type_opt);

void direct_declarator_type_destroy(struct direct_declarator_type* p_direct_declarator_type_opt)
{

}

void function_declarator_type_destroy(struct function_declarator_type* p)
{
}


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

    if (e_type_specifier_flags & TYPE_SPECIFIER_INT16)
        print_item(ss, first, "__int16");

    if (e_type_specifier_flags & TYPE_SPECIFIER_INT32)
        print_item(ss, first, "__int32");

    if (e_type_specifier_flags & TYPE_SPECIFIER_INT64)
        print_item(ss, first, "__int64");

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

    if (e_type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T)
        print_item(ss, first, "nullptr_t");

    return first;
}



void print_direct_declarator_type(struct osstream* ss, struct direct_declarator_type* type);

void print_declarator_type(struct osstream* ss, const struct declarator_type* p_declarator_type)
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


void print_params(struct osstream* ss,
    struct params* params, bool is_var_args)
{

    ss_fprintf(ss, "(");
    struct type* param = params->head;
    while (param)
    {
        if (param != params->head)
            ss_fprintf(ss, ",");
        print_type(ss, param);
        param = param->next;
    }
    if (is_var_args)
        ss_fprintf(ss, ",...");

    ss_fprintf(ss, ")");
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
        print_params(ss,
            &p_direct_declarator_type->function_declarator_type->params,
            p_direct_declarator_type->function_declarator_type->is_var_args);
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

void print_type_qualifier_specifiers(struct osstream* ss, const struct type* type)
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

void type_add_const(struct type* p_type)
{
    enum type_category category = type_get_category(p_type);
    switch (category)
    {
    case TYPE_CATEGORY_FUNCTION:
        assert(false);
        break;
    case TYPE_CATEGORY_ARRAY:
        assert(false);
        break;
    case TYPE_CATEGORY_POINTER:
    {
        struct declarator_type* p = find_inner_declarator(p_type->declarator_type);
        if (p)
        {
            p->pointers.head->type_qualifier_flags |= TYPE_QUALIFIER_CONST;
        }
    }
    break;

    case TYPE_CATEGORY_ITSELF:
        p_type->type_qualifier_flags |= TYPE_QUALIFIER_CONST;
        break;
    default:
        break;
    }
}

void type_remove_qualifiers(struct type* p_type)
{
    enum type_category category = type_get_category(p_type);
    switch (category)
    {
    case TYPE_CATEGORY_FUNCTION:
        break;

    case TYPE_CATEGORY_ARRAY:
        /* TODO
         int g(const int a[const 20]) {
            // in this function, a has type const int* const (const pointer to const int)
        }*/
        break;

    case TYPE_CATEGORY_POINTER:
    {
        struct declarator_type* declarator =
            find_inner_declarator(p_type->declarator_type);
        if (declarator != NULL)
        {
            declarator->pointers.tail->type_qualifier_flags = TYPE_QUALIFIER_NONE;
        }
        else {
            assert(false);
        }
        break;
    }
    case TYPE_CATEGORY_ITSELF:
        p_type->type_qualifier_flags = TYPE_QUALIFIER_NONE;
        break;
    default:
        break;
    }
}

struct type type_lvalue_conversion(struct type* p_type)
{

    enum type_category category = type_get_category(p_type);
    switch (category)
    {
    case TYPE_CATEGORY_FUNCTION:
    {
        /*
           "function returning type" is converted to an expression that has type
           "pointer to function returning type".
        */
        struct type t = type_add_pointer(p_type);
        t.attributes_flags &= ~CUSTOM_ATTRIBUTE_PARAM;
        t.category = type_get_category_core(&t);
        return t;
    }

    case TYPE_CATEGORY_ARRAY:
    {
        /*
          An expression that has type "array of type" is converted
          to an expression with type "pointer to type" that points to the initial element
          of the array object and s not an lvalue.
          If the array object has register storage class, the behavior is undefined.
        */
        struct type t = get_array_item_type(p_type);
        struct type t2 = type_add_pointer(&t);

        type_remove_qualifiers(&t2);
        /*
        int g(const int a[const 20]) {
            // in this function, a has type const int* const (const pointer to const int)
            }
        */
        type_destroy(&t);
        t2.attributes_flags &= ~CUSTOM_ATTRIBUTE_PARAM;
        return t2;
    }

    case TYPE_CATEGORY_POINTER:
    case TYPE_CATEGORY_ITSELF:
    default:
        break;
    }

    struct type t = type_copy(p_type);
    type_remove_qualifiers(&t);
    t.attributes_flags &= ~CUSTOM_ATTRIBUTE_PARAM;

    t.category = type_get_category(&t);

    return t;
}

struct type type_convert_to(struct type* p_type, enum language_version target)
{
    /*
    * Convert types to previous standard format
    */
    struct type t = type_copy(p_type);
    if (t.type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T)
    {
        if (target < LANGUAGE_C2X)
        {
            t.type_specifier_flags &= ~TYPE_SPECIFIER_NULLPTR_T;
            t.type_specifier_flags |= TYPE_SPECIFIER_VOID;

            struct pointer* p_pointer = calloc(1, sizeof(struct pointer));
            t.declarator_type = calloc(1, sizeof(struct declarator_type));
            t.declarator_type->direct_declarator_type = calloc(1, sizeof(struct direct_declarator_type));

            if (p_type->declarator_type &&
                p_type->declarator_type->direct_declarator_type &&
                p_type->declarator_type->direct_declarator_type->name_opt)
            {
                /*let´s copy the name*/
                t.declarator_type->direct_declarator_type->name_opt = strdup(p_type->declarator_type->direct_declarator_type->name_opt);
            }

            LIST_ADD(&t.declarator_type->pointers, p_pointer);
        }
    }
    else if (t.type_specifier_flags & TYPE_SPECIFIER_BOOL)
    {
        if (target < LANGUAGE_C99)
        {
            t.type_specifier_flags &= ~TYPE_SPECIFIER_BOOL;
            t.type_specifier_flags |= TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR;
        }
    }

    return t;
}

void print_type(struct osstream* ss, const struct type* type)
{
    print_type_qualifier_specifiers(ss, type);
    print_declarator_type(ss, type->declarator_type);// type->declarator_name_opt);
}

void type_print(const struct type* a) {
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
        LIST_ADD(&list, p_pointer_type);
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
        LIST_ADD(&r, par);
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

enum type_category type_get_category_core(const struct type* p_type)
{
    enum type_category type_category = TYPE_CATEGORY_ITSELF;
    visit_declarator_get(&type_category, p_type->declarator_type);
    return type_category;
}

enum type_category type_get_category(const struct type* p_type)
{

    /*better performance*/
#if 0 
    /*
    * Initialy category was always computed, searched.
    * then I decided to keep it calculated on p_type->category
    * but in case some place is not doing this we have this print
    */
    enum type_category c = type_get_category_core(p_type);
    if (c != p_type->category)
    {
        static int ops = 0;
        printf("******************************** ops %d\n", ops++);
        ((struct type*)p_type)->category = c;
    }
#endif

    return p_type->category;
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

        struct struct_or_union_specifier* p_complete =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_attribute_specifier_sequence_opt == NULL && p_complete)
        {
            p_attribute_specifier_sequence_opt = p_complete->attribute_specifier_sequence_opt;
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

/*used to get arguments from function or function pointer*/
struct  function_declarator_type* get_function_declarator_type(struct type* p_type)
{
    struct direct_declarator_type* p_direct_declarator_type =
        find_inner_function(p_type);
    if (p_direct_declarator_type)
    {
        assert(p_direct_declarator_type->function_declarator_type);
        return p_direct_declarator_type->function_declarator_type;
    }
    return NULL;
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
    return type_get_category(p_type) == TYPE_CATEGORY_ARRAY;
}


bool type_is_const(struct type* p_type)
{
    enum type_category category = type_get_category(p_type);
    switch (category)
    {
    case TYPE_CATEGORY_ITSELF:
        return p_type->type_qualifier_flags & TYPE_QUALIFIER_CONST;

    case TYPE_CATEGORY_FUNCTION:
        return false;

    case TYPE_CATEGORY_ARRAY:
        return false; //?

    case TYPE_CATEGORY_POINTER:
    {
        struct declarator_type* declarator =
            find_inner_declarator(p_type->declarator_type);
        if (declarator != NULL)
        {
            return
                declarator->pointers.tail->type_qualifier_flags & TYPE_QUALIFIER_CONST;
        }
        else {
            assert(false);
        }
    }
    break;
    default:
        break;
    }


    return false;
}

bool direct_declarator_type_is_empty(struct direct_declarator_type* p_direct_declarator_type_opt)
{
    if (p_direct_declarator_type_opt == NULL)
        return true;

    return
        p_direct_declarator_type_opt->array_declarator_type == NULL &&
        p_direct_declarator_type_opt->declarator_opt == NULL &&
        p_direct_declarator_type_opt->function_declarator_type == NULL;
}

bool type_is_void_ptr(const struct type* p_type)
{
    if (p_type->declarator_type &&
        p_type->declarator_type->pointers.head &&
        p_type->declarator_type->pointers.head->next == NULL)
    {
        if (direct_declarator_type_is_empty(p_type->declarator_type->direct_declarator_type))
        {
            return p_type->type_specifier_flags & TYPE_SPECIFIER_VOID;
        }
    }

    return false;
}

bool type_is_void(const struct type* p_type)
{
    if (p_type->declarator_type == NULL)
    {
        return p_type->type_specifier_flags & TYPE_SPECIFIER_VOID;
    }

    return false;
}

bool type_is_nullptr_t(const struct type* p_type)
{
    if (p_type->declarator_type == NULL)
    {
        return p_type->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T;
    }

    return false;
}


bool type_is_pointer(struct type* p_type)
{
    enum type_category category = type_get_category(p_type);
    if (category == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags == TYPE_SPECIFIER_NULLPTR_T)
    {
        return true;
    }
    return category == TYPE_CATEGORY_POINTER;
}


bool type_is_enum(struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM;
}

bool type_is_struct_or_union(struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION;
}

/*
  The three types
  char, signed char, and unsigned char
  are collectively called the character types.
*/
bool type_is_character(struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR;
}

bool type_is_bool(struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL;
}


/*
 There are three standard floating types, designated as
 float, double, and long double.

 There are three decimal floating types, designated as _Decimal32, _Decimal64, and _Decimal128.
*/
bool type_is_floating_point(struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_DOUBLE |
            TYPE_SPECIFIER_FLOAT);
}


/*
  The type char, the signed and unsigned integer types,
  and the enumerated types
  are collectively  called integer types.
*/
bool type_is_integer(struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {
        /*we cannot check long without check double*/
        //long double
        return false;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
    {
        return true;
    }

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_CHAR |
            TYPE_SPECIFIER_SHORT |
            TYPE_SPECIFIER_INT |

            TYPE_SPECIFIER_INT16 |
            TYPE_SPECIFIER_INT32 |
            TYPE_SPECIFIER_INT64 |

            TYPE_SPECIFIER_INT |
            TYPE_SPECIFIER_LONG |
            TYPE_SPECIFIER_SIGNED |
            TYPE_SPECIFIER_UNSIGNED |
            TYPE_SPECIFIER_INT8 |
            TYPE_SPECIFIER_INT16 |
            TYPE_SPECIFIER_INT64 |
            TYPE_SPECIFIER_LONG_LONG |
            TYPE_SPECIFIER_BOOL);
}

/*
* Integer and floating types are collectively called arithmetic types.
*/
bool type_is_arithmetic(struct type* p_type)
{
    return type_is_integer(p_type) || type_is_floating_point(p_type);
}

/*
 Arithmetic types, pointer types, and the nullptr_t type are collectively
 called scalar types.
*/
bool type_is_scalar(struct type* p_type)
{
    //TODO we need two concepts...is_scalar on real type or is_scalar after lvalue converison

    if (type_is_arithmetic(p_type))
        return true;

    if (type_is_pointer_or_array(p_type))
        return true;

    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;


    if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        return true;
    if (p_type->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T)
        return true;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        return true;

    return false;
}

bool type_is_compatible(struct type* expression_type, struct type* return_type)
{
    //TODO

    //if (!type_is_same(a, b))
      //  return false;

    return true;
}

void check_function_argument_and_parameter(struct parser_ctx* ctx,
    struct argument_expression* current_argument,
    struct type* paramer_type,
    int param_num)
{
    struct type* argument_type = &current_argument->expression->type;
    bool is_zero = false;
    
    if (constant_value_is_valid(&current_argument->expression->constant_value) &&
        constant_value_to_ull(&current_argument->expression->constant_value) == 0)
    {
        is_zero = true;
    }

    struct type t1 = { 0 };
    struct type t2 = { 0 };
    /*
       less generic tests are first
    */
    if (type_is_enum(argument_type) && type_is_enum(paramer_type))
    {
        if (!type_is_same(argument_type, paramer_type, false))
        {
            parser_seterror_with_token(ctx,
                current_argument->expression->first_token,
                " incompatible types at argument %d", param_num);
        }
        goto continuation;
    }

    if (type_is_arithmetic(argument_type) && type_is_arithmetic(paramer_type))
    {
        goto continuation;
    }

    if (is_zero && type_is_pointer_or_array(paramer_type))
    {
        goto continuation;
    }

    if (type_is_pointer_or_array(argument_type) && type_is_pointer_or_array(paramer_type))
    {
        if (argument_type->type_specifier_flags & TYPE_SPECIFIER_VOID)
        {
            /*void pointer can be converted to any type*/
            goto continuation;
        }

        if (paramer_type->type_specifier_flags & TYPE_SPECIFIER_VOID)
        {
            /*void pointer can be converted to any type*/
            goto continuation;
        }


        //TODO  lvalue

        if (type_is_array(paramer_type))
        {
            t2 = type_lvalue_conversion(paramer_type);
        }
        else
        {
            t2 = type_copy(paramer_type);
        }

        if (expression_is_subjected_to_lvalue_conversion(current_argument->expression))
        {
            t1 = type_lvalue_conversion(argument_type);
        }
        else
        {
            t1 = type_copy(argument_type);
        }


        if (!type_is_same(&t1, &t2, false))
        {
            //type_print(&t1);
            //type_print(&t2);

            parser_seterror_with_token(ctx,
                current_argument->expression->first_token,
                " incompatible types at argument %d", param_num);
            //disabled for now util it works correctly
            //return false;
        }
        //return true;
    }

continuation:
    type_destroy(&t1);
    type_destroy(&t2);
}

bool type_is_function(struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_FUNCTION;
}

bool type_is_function_or_function_pointer(struct type* p_type)
{

    if (type_is_function(p_type))
        return true;

    if (type_is_pointer(p_type))
    {
        //TODO not optimized
        struct type t = type_remove_pointer(p_type);
        bool r = type_is_function(&t);
        type_destroy(&t);
        return r;
    }

    return false;
}

struct type type_add_pointer(struct type* p_type)
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
        LIST_ADD(&p2->pointers, p_pointer_type);
        pdirect_declarator_type->declarator_opt = p2;

        p->direct_declarator_type->function_declarator_type->direct_declarator_type = pdirect_declarator_type;
    }
    else if (p->direct_declarator_type && p->direct_declarator_type->array_declarator_type)
    {
        struct direct_declarator_type* pdirect_declarator_type = calloc(1, sizeof * pdirect_declarator_type);
        struct declarator_type* p2 = calloc(1, sizeof * p);
        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        LIST_ADD(&p2->pointers, p_pointer_type);

        pdirect_declarator_type->declarator_opt = p2;

        p->direct_declarator_type->array_declarator_type->direct_declarator_type = pdirect_declarator_type;
    }
    else
    {
        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        LIST_ADD(&p->pointers, p_pointer_type);
    }

    //type_print(&r);
    r.category = type_get_category_core(&r);
    return r;
}

struct type type_remove_pointer(struct type* p_type)
{
    struct type r = type_copy(p_type);
    struct declarator_type* p_inner_declarator = find_inner_declarator(r.declarator_type);
    if (p_inner_declarator && p_inner_declarator->pointers.head != NULL)
    {
        pointer_type_list_pop_front(&p_inner_declarator->pointers);
    }
    else
    {
        //parser_seterror_with_token(ctx, ctx->current, "indirection requires pointer operand");
    }
    r.category = type_get_category_core(&r);
    return r;
}

void array_declarator_type_delete(struct array_declarator_type* p)
{
    if (p)
    {
        //TODO recursive free
        //p->direct_declarator_type
        free(p);
    }
}

static void visit_declarator_to_remove_array(int* removed, struct declarator_type* declarator);
static void visit_direct_declarator_to_remove_array(int* removed, struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt)
        visit_declarator_to_remove_array(removed, p_direct_declarator_type->declarator_opt);

    if (p_direct_declarator_type->function_declarator_type)
    {
        if (p_direct_declarator_type->function_declarator_type->direct_declarator_type)
        {
            visit_direct_declarator_to_remove_array(removed, p_direct_declarator_type->function_declarator_type->direct_declarator_type);
        }
    }

    if (p_direct_declarator_type->array_declarator_type)
    {
        if (p_direct_declarator_type->array_declarator_type->direct_declarator_type)
        {
            visit_direct_declarator_to_remove_array(removed, p_direct_declarator_type->array_declarator_type->direct_declarator_type);
        }

        if (*removed == false)
        {
            array_declarator_type_delete(p_direct_declarator_type->array_declarator_type);
            p_direct_declarator_type->array_declarator_type = NULL;
            *removed = true;
        }
    }
}

static void visit_declarator_to_remove_array(int* removed, struct declarator_type* declarator)
{
    if (declarator == NULL)
        return;

    if (declarator->direct_declarator_type)
        visit_direct_declarator_to_remove_array(removed, declarator->direct_declarator_type);
}


struct type get_array_item_type(struct type* p_type)
{
    assert(type_is_array(p_type));
    struct type r = type_copy(p_type);
    int removed = false;
    visit_declarator_to_remove_array(&removed, r.declarator_type);
    assert(removed);
    r.category = type_get_category_core(&r);
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



bool type_is_pointer_or_array(struct type* p_type)
{
    const enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER ||
        category == TYPE_CATEGORY_ARRAY)
        return true;

    if (category == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags == TYPE_SPECIFIER_NULLPTR_T)
    {
        return true;
    }

    return false;
}


//See 6.3.1.1
int type_get_rank(struct type* p_type1)
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
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T))
    {
        rank = 50; //?
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
        return -1;
        //seterror(error, "internal error - struct is not valid for rank");
    }
    else
    {
        return -2;
        //seterror(error, "unexpected type for rank");
    }
    return rank;
}

int type_common(struct type* p_type1, struct type* p_type2, struct type* out)
{
    try
    {
        type_destroy(out);

        int rank_left = type_get_rank(p_type1);
        if (rank_left < 0) throw;

        int rank_right = type_get_rank(p_type2);
        if (rank_right < 0) throw;

        if (rank_left >= rank_right)
            *out = type_copy(p_type1);
        else
            *out = type_copy(p_type2);
    }
    catch
    {
        return 1;
    }

    return 0;
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
    r.category = p_type->category;

    //if (p_type->declarator_name_opt)
      //r.declarator_name_opt = strdup(p_type->declarator_name_opt);

    if (p_type->declarator_type)
    {
        r.declarator_type = declarator_type_copy(p_type->declarator_type);
    }
    return r;
}


static void visit_declarator_to_get_array_size(int* array_size, struct declarator_type* declarator);
static void visit_direct_declarator_to_get_array_size(int* array_size, struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt)
    {
        visit_declarator_to_get_array_size(array_size, p_direct_declarator_type->declarator_opt);
    }

    if (p_direct_declarator_type->function_declarator_type)
    {
        if (p_direct_declarator_type->function_declarator_type->direct_declarator_type)
        {
            visit_direct_declarator_to_get_array_size(array_size, p_direct_declarator_type->function_declarator_type->direct_declarator_type);
        }
    }

    if (p_direct_declarator_type->array_declarator_type)
    {
        if (p_direct_declarator_type->array_declarator_type->direct_declarator_type)
        {
            visit_direct_declarator_to_get_array_size(array_size, p_direct_declarator_type->array_declarator_type->direct_declarator_type);
        }

        if (*array_size == 0)
        {
            //TODO maybe array does not have size?
            *array_size = (int)p_direct_declarator_type->array_declarator_type->constant_size;
        }
    }
}
static void visit_declarator_to_get_array_size(int* array_size, struct declarator_type* declarator)
{
    if (declarator == NULL)
        return;

    if (declarator->direct_declarator_type)
        visit_direct_declarator_to_get_array_size(array_size, declarator->direct_declarator_type);
}
int get_array_size(struct type* p_type)
{
    if (type_is_array(p_type))
    {
        int sz = 0;
        visit_declarator_to_get_array_size(&sz, p_type->declarator_type);

        return sz;
    }
    else
    {
        assert(false);
    }
    return 0;
}


static void visit_declarator_to_set_array_size(int* array_size, struct declarator_type* declarator, int size);
static void visit_direct_declarator_to_set_array_size(int* array_size, struct direct_declarator_type* p_direct_declarator_type, int size)
{
    if (p_direct_declarator_type->declarator_opt)
    {
        visit_declarator_to_set_array_size(array_size, p_direct_declarator_type->declarator_opt, size);
    }

    if (p_direct_declarator_type->function_declarator_type)
    {
        if (p_direct_declarator_type->function_declarator_type->direct_declarator_type)
        {
            visit_direct_declarator_to_set_array_size(array_size, p_direct_declarator_type->function_declarator_type->direct_declarator_type, size);
        }
    }

    if (p_direct_declarator_type->array_declarator_type)
    {
        if (p_direct_declarator_type->array_declarator_type->direct_declarator_type)
        {
            visit_direct_declarator_to_set_array_size(array_size, p_direct_declarator_type->array_declarator_type->direct_declarator_type, size);
        }

        if (*array_size == 0)
        {
            *array_size = 1;
            p_direct_declarator_type->array_declarator_type->constant_size = size;
        }
    }
}
static void visit_declarator_to_set_array_size(int* array_size, struct declarator_type* declarator, int size)
{
    if (declarator == NULL)
        return;

    if (declarator->direct_declarator_type)
        visit_direct_declarator_to_set_array_size(array_size, declarator->direct_declarator_type, size);
}

int set_array_size(struct type* p_type, int size)
{
    if (type_is_array(p_type))
    {
        int sz = 0;
        visit_declarator_to_set_array_size(&sz, p_type->declarator_type, size);

        return sz;
    }
    else
    {
        assert(false);
    }
    return 0;
}

int type_get_sizeof(struct type* p_type);
int get_sizeof_struct(struct struct_or_union_specifier* complete_struct_or_union_specifier)
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
                int align = type_get_alignof(&md->declarator->type);

                if (align > maxalign)
                {
                    maxalign = align;
                }
                if (size % align != 0)
                {
                    size += align - (size % align);
                }

                size += type_get_sizeof(&md->declarator->type);
                md = md->next;
            }
        }
        d = d->next;
    }
    if (maxalign != 0)
    {
        if (size % maxalign != 0)
        {
            size += maxalign - (size % maxalign);
        }
    }
    else
    {
        assert(false);
    }

    return size;
}
int type_get_alignof(struct type* p_type);
int get_alignof_struct(struct struct_or_union_specifier* complete_struct_or_union_specifier)
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
                int temp_align = type_get_alignof(&md->declarator->type);
                if (temp_align > align)
                {
                    align = temp_align;
                }
                md = md->next;
            }
        }
        else
        {
            /*We don't have the declarator like in */
            /*
              struct X {
                union {
                    struct {
                        int Zone;
                    };
                    int Value;
                };
            };
            static_assert(alignof(struct X) == 1);
            */

            /*so we create a type using only specifiers*/

            struct type type = { 0 };
            if (d->specifier_qualifier_list)
            {
                type.type_specifier_flags =
                    d->specifier_qualifier_list->type_specifier_flags;

                type.enum_specifier = d->specifier_qualifier_list->enum_specifier;
                type.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;

            }

            int temp_align = type_get_alignof(&type);
            if (temp_align > align)
            {
                align = temp_align;
            }

            type_destroy(&type);
        }
        d = d->next;
    }
    assert(align != 0);
    return align;
}

int type_get_alignof(struct type* p_type)
{
    int align = 0;

    enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        align = _Alignof(void*);
    }
    else if (category == TYPE_CATEGORY_FUNCTION)
    {
        align = -1;
        //seterror(error, "sizeof function");
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
            struct struct_or_union_specifier* p_complete =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            align = 1;
            if (p_complete)
            {
                align = get_alignof_struct(p_complete);
            }
            else
            {
                align = -2;
                //seterror(error, "invalid application of 'sizeof' to incomplete type 'struct %s'", p_type->struct_or_union_specifier->tag_name);
            }
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            align = _Alignof(int);
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            align = -3;
            //seterror(error, "type information is missing");
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
        align = type_get_alignof(&type);
        type_destroy(&type);
    }
    assert(align > 0);
    return align;
}


int type_get_sizeof(struct type* p_type)
{
    int size = 0;

    enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        size = (int)sizeof(void*);
    }
    else if (category == TYPE_CATEGORY_FUNCTION)
    {
        size = -1;
        //seterror(error, "sizeof function");
    }
    else if (category == TYPE_CATEGORY_ITSELF)
    {
        if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
        {
            size = (int)sizeof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        {
            size = (int)sizeof(_Bool);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        {
            size = (int)sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT)
        {
            size = (int)sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            //TODO enum type
            size = (int)sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            size = (int)sizeof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        {
            size = (int)sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT64)
        {
            size = (int)sizeof(long long);
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
            size = (int)sizeof(float);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        {
            size = (int)sizeof(double);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            struct struct_or_union_specifier* p_complete =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            size = 1;
            if (p_complete)
            {
                size = get_sizeof_struct(p_complete);
            }
            else
            {
                size = -2;
                //seterror(error, "invalid application of 'sizeof' to incomplete type 'struct %s'", p_type->struct_or_union_specifier->tag_name);
            }
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            size = (int)sizeof(int);
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            size = -3;
            //seterror(error, "type information is missing");
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
        int arraysize = get_array_size(p_type);
        struct type type = get_array_item_type(p_type);
        int sz = type_get_sizeof(&type);
        size = sz * arraysize;
        type_destroy(&type);
    }

    return size;
}

unsigned int type_get_hashof(struct parser_ctx* ctx, struct type* p_type)
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

            struct struct_or_union_specifier* p_complete =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            if (p_complete)
            {
                struct token* current = p_complete->first_token;
                for (;
                    current != p_complete->last_token->next;
                    current = current->next)
                {
                    if (current->flags & TK_FLAG_FINAL)
                    {
                        ss_fprintf(&ss, "%s", current->lexeme);

                    }
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
            parser_set_info_with_token(ctx, ctx->current, "type information is missing");
            throw;
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_TYPEOF)
        {
            //s//ize = 1; //TODO
            //assert(false);
            //;; size =
                //  type_get_sizeof(ctx, struct type* p_type)
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
                parser_seterror_with_token(ctx,
                    ctx->current,
                    "invalid application of 'sizeof' to a void type");
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
        if (p->type_qualifier_list_opt)
            p_pointer_type->type_qualifier_flags = p->type_qualifier_list_opt->flags;
        LIST_ADD(&r, p_pointer_type);
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

        LIST_ADD(&p_function_declarator_type->params, p_type);
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
        p_direct_declarator_type->function_declarator_type =
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


        if (
            p_typedef_decl0->direct_declarator_type &&
            p_typedef_decl0->direct_declarator_type->function_declarator_type &&
            p_typedef_decl0->direct_declarator_type->function_declarator_type->direct_declarator_type &&
            p_typedef_decl0->direct_declarator_type->function_declarator_type->direct_declarator_type->name_opt)
        {
            /*
              extern int func(void);
              auto f = func;
              was repeating "func"
              int  (* f) func(void) = func;
            */
            free((void*)p_typedef_decl0->direct_declarator_type->function_declarator_type->direct_declarator_type->name_opt);
            p_typedef_decl0->direct_declarator_type->function_declarator_type->direct_declarator_type->name_opt = NULL;
        }

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
                LIST_ADD(&p_typedef_decl->pointers, p);
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
            t = type_copy(&pdeclarator->specifier_qualifier_list->typeof_specifier->type);
            struct declarator_type* dectype = clone_declarator_to_declarator_type(ctx, pdeclarator);
            declarator_type_merge(dectype, t.declarator_type);
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

            t = type_copy(&pdeclarator->declaration_specifiers->typeof_specifier->type);

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


    t.category = type_get_category_core(&t);


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
    t.category = type_get_category_core(&t);
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

struct direct_declarator_type* direct_declarator_type_find_inner_function(struct direct_declarator_type* p_direct_declarator_type);

struct direct_declarator_type* declarator_type_find_inner_function(struct declarator_type* p_declarator_type)
{
    struct direct_declarator_type* p = NULL;
    if (p_declarator_type->direct_declarator_type)
    {
        p = direct_declarator_type_find_inner_function(p_declarator_type->direct_declarator_type);
    }
    return p;
}


struct direct_declarator_type* direct_declarator_type_find_inner_function(struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt == NULL)
    {
        if (p_direct_declarator_type->function_declarator_type)
        {
            if (p_direct_declarator_type->function_declarator_type->direct_declarator_type)
            {
                struct direct_declarator_type* p = direct_declarator_type_find_inner_function(p_direct_declarator_type->function_declarator_type->direct_declarator_type);
                if (p == NULL)
                {
                    return p_direct_declarator_type;
                }
                return p;
            }

            return p_direct_declarator_type;
        }
        return NULL;
    }

    return  declarator_type_find_inner_function(p_direct_declarator_type->declarator_opt);
}

/*works for function and pointer to function*/
struct direct_declarator_type* find_inner_function(struct type* p_type)
{
    return direct_declarator_type_find_inner_function(p_type->declarator_type->direct_declarator_type);
}

struct type get_function_return_type(struct type* p_type)
{
    struct type r = type_copy(p_type);

    struct direct_declarator_type* p_direct_declarator_type =
        find_inner_function(&r);

    if (p_direct_declarator_type)
    {
        /*lets delete the function part*/
        function_declarator_type_destroy(p_direct_declarator_type->function_declarator_type);
        p_direct_declarator_type->function_declarator_type = NULL;
    }

    r.category = type_get_category_core(&r);
    return r;
}


void type_set_int(struct type* p_type)
{
    p_type->type_specifier_flags = TYPE_SPECIFIER_INT;
    p_type->type_qualifier_flags = 0;
    p_type->declarator_type = NULL;
    p_type->category = TYPE_CATEGORY_ITSELF;
}

struct type type_make_enumerator(struct enum_specifier* enum_specifier)
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_ENUM;
    t.enum_specifier = enum_specifier;
    t.category = TYPE_CATEGORY_ITSELF;

    return t;
}

struct type type_make_size_t()
{
    struct type t = { 0 };

#ifdef _WIN64
    t.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT64;
#else
    t.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT;
#endif

    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type make_void_type()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_VOID;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}
struct type type_make_int()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_INT;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_literal_string(int size, enum type_specifier_flags chartype)
{
    struct type t = { 0 };
    t.type_specifier_flags = chartype;// TYPE_SPECIFIER_CHAR;

    struct declarator_type* p_declarator_type = calloc(1, sizeof * p_declarator_type);
    struct array_declarator_type* array_declarator_type = calloc(1, sizeof * array_declarator_type);
    struct direct_declarator_type* p_direct_declarator_type = calloc(1, sizeof * p_direct_declarator_type);
    struct direct_declarator_type* p_direct_declarator_type2 = calloc(1, sizeof * p_direct_declarator_type);

    p_declarator_type->direct_declarator_type = p_direct_declarator_type;

    array_declarator_type->constant_size = size;
    array_declarator_type->direct_declarator_type = p_direct_declarator_type2; /*abstract*/
    p_direct_declarator_type->array_declarator_type = array_declarator_type;

    t.declarator_type = p_declarator_type;
    t.category = TYPE_CATEGORY_ARRAY;
    return t;
}

bool pointer_type_is_same(struct pointer_type* a, struct pointer_type* b, bool compare_qualifiers)
{
    if (a && b)
    {
        if (compare_qualifiers)
        {
            if (a->type_qualifier_flags != b->type_qualifier_flags)
                return false;
        }

        return true;
    }
    return a == NULL && b == NULL;
}

bool pointer_type_list_is_same(struct pointer_type_list* a, struct pointer_type_list* b, bool compare_qualifiers)
{
    if (a && b)
    {
        struct pointer_type* pa = a->head;
        struct pointer_type* pb = b->head;

        if (pa && pb)
        {
            while (pa && pb)
            {
                if (!pointer_type_is_same(pa, pb, compare_qualifiers))
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



bool declarator_type_is_same(struct declarator_type* a, struct declarator_type* b, bool compare_qualifiers);


bool array_declarator_type_is_same(struct array_declarator_type* a, struct array_declarator_type* b)
{
    if (a && b)
    {
        if (!direct_declarator_type_is_same(a->direct_declarator_type, b->direct_declarator_type, false))
            return false;

        return a->constant_size == b->constant_size;
    }
    return a == b;
}

bool function_declarator_type_is_same(struct function_declarator_type* a, struct function_declarator_type* b)
{
    if (a && b)
    {
        if (!direct_declarator_type_is_same(a->direct_declarator_type, b->direct_declarator_type, false))
            return false;

        if (!type_list_is_same(&a->params, &b->params))
            return false;

        return a->is_var_args == b->is_var_args;
    }

    return a == b;
}

bool direct_declarator_type_is_same(struct direct_declarator_type* a, struct direct_declarator_type* b, bool compare_qualifiers)
{
    if (a && b)
    {
        if (!direct_declarator_type_is_empty(a) && !direct_declarator_type_is_empty(b))
        {

            if (!array_declarator_type_is_same(a->array_declarator_type, b->array_declarator_type))
                return false;


            if (!function_declarator_type_is_same(a->function_declarator_type, b->function_declarator_type))
                return false;


            if (!declarator_type_is_same(a->declarator_opt, b->declarator_opt, compare_qualifiers))
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
        struct struct_or_union_specifier* p_complete_a = get_complete_struct_or_union_specifier(a);
        struct struct_or_union_specifier* p_complete_b = get_complete_struct_or_union_specifier(b);

        if (p_complete_a != NULL && p_complete_b != NULL)
        {
            if (p_complete_a != p_complete_b)
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
        return p_complete_a == NULL && p_complete_b == NULL;
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

bool declarator_type_is_same(struct declarator_type* a, struct declarator_type* b, bool compare_qualifiers)
{
    if (!declarator_type_is_empty(a) && !declarator_type_is_empty(b))
    {
        if (!pointer_type_list_is_same(&a->pointers, &b->pointers, compare_qualifiers))
            return false;

        if (!direct_declarator_type_is_same(a->direct_declarator_type, b->direct_declarator_type, compare_qualifiers))
            return false;

        return true;
    }
    return declarator_type_is_empty(a) && declarator_type_is_empty(b);
}



bool type_is_same(struct type* a, struct type* b, bool compare_qualifiers)
{
    if (a && b)
    {
        if (compare_qualifiers)
        {
            if (a->type_qualifier_flags != b->type_qualifier_flags)
            {
                return false;
            }
        }

        if (a->type_specifier_flags != b->type_specifier_flags)
        {
            return false;
        }

        if (!declarator_type_is_same(a->declarator_type, b->declarator_type, compare_qualifiers))
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

void type_swap(struct type* a, struct type* b)
{
    struct type temp = *a;
    *a = *b;
    *b = temp;
    _del_attr(temp, MUST_DESTROY);
}

void declarator_type_print_data(int n, struct declarator_type* p_declarator_type);

void printdots(int n) {
    for (int i = 0; i < n * 2; i++) printf(" ");
    printf("|");
}
void direct_declarator_type_print_data(int n, struct direct_declarator_type* p)
{
    if (p == NULL) return;

    if (p->name_opt)
    {
        printdots(n);
        printf("name '%s'", p->name_opt);
    }

    if (p->array_declarator_type)
    {
        printdots(n);
        printf("array_declarator_type\n");
        direct_declarator_type_print_data(n + 1, p->array_declarator_type->direct_declarator_type);

        //printf("[]\n");
    }
    else if (p->function_declarator_type)
    {
        printdots(n);

        printf("function_declarator_type %p", p->function_declarator_type);
        struct osstream ss = { 0 };
        print_params(&ss, &p->function_declarator_type->params,
            p->function_declarator_type->is_var_args);
        printf("%s\n", ss.c_str);
        ss_close(&ss);


        direct_declarator_type_print_data(n + 1, p->function_declarator_type->direct_declarator_type);

        //printf("()");
    }
    if (p->declarator_opt)
    {
        printdots(n);
        printf("declarator %p\n", p->declarator_opt);
        declarator_type_print_data(n + 1, p->declarator_opt);
    }
}

void declarator_type_print_data(int n, struct declarator_type* p_declarator_type)
{

    struct pointer_type* p = p_declarator_type->pointers.head;
    if (p) {
        printdots(n);

        while (p)
        {
            printf("*");
            p = p->next;
        }
        printf("\n");
    }

    if (p_declarator_type->direct_declarator_type)
    {
        printdots(n);
        printf("direct-declarator %p\n", p_declarator_type->direct_declarator_type);
        direct_declarator_type_print_data(n + 1, p_declarator_type->direct_declarator_type);
    }


}

void type_print_data(const struct type* p_type)
{
    type_print(p_type);


    int n = 0;
    if (p_type->struct_or_union_specifier)
        printf("struct_or_union_specifier: %p\n", p_type->struct_or_union_specifier);

    if (p_type->enum_specifier)
        printf("enum_specifier %p\n", p_type->enum_specifier);

    if (p_type->declarator_type)
    {
        printf("declarator %p\n", p_type->declarator_type);
        declarator_type_print_data(n + 1, p_type->declarator_type);
    }
    printf("\n");
}