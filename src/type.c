#include "ownership.h"
#include <stdbool.h>
#include "type.h"
#include "parser.h"
#include "error.h"
#include <stdlib.h>
#include <assert.h>
#include "hash.h"
#include <string.h>
#include <stdio.h>

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

    return *first;
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
    p_type->type_qualifier_flags |= TYPE_QUALIFIER_CONST;
}

void type_remove_qualifiers(struct type* p_type)
{
    p_type->type_qualifier_flags = 0;
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
            t.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_PARAMETER;
            t.category = t.category;
            return t;
        }
        break;
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
            t2.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_PARAMETER;
            return t2;
        }
        break;
        case TYPE_CATEGORY_POINTER:
            break;
        case TYPE_CATEGORY_ITSELF:
        default:
            break;
    }

    struct type t = type_dup(p_type);
    type_remove_qualifiers(&t);
    t.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_PARAMETER;

    t.category = type_get_category(&t);

    return t;
}

struct type type_convert_to(const struct type* p_type, enum language_version target)
{
    /*
    * Convert types to previous standard format
    */

    if (type_is_nullptr_t(p_type))
    {
        if (target < LANGUAGE_C2X)
        {
            struct type t = make_void_ptr_type();
            assert(t.name_opt == NULL);
            if (p_type->name_opt)
            {
                t.name_opt = strdup(p_type->name_opt);
            }
            return t;
        }
    }
    else if (type_is_bool(p_type))
    {
        if (target < LANGUAGE_C99)
        {
            struct type t = type_dup(p_type);
            t.type_specifier_flags &= ~TYPE_SPECIFIER_BOOL;
            t.type_specifier_flags |= TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR;
            return t;
        }
    }

    return type_dup(p_type);
}

void print_type_core(struct osstream* ss, const struct type* p_type, bool onlydeclarator)
{
    const struct type* p = p_type;

    while (p)
    {
        if (onlydeclarator && p->next == NULL)
            break;

        switch (p->category)
        {
            case TYPE_CATEGORY_ITSELF:
            {
                struct osstream local = {0};
                bool first = true;

                print_type_qualifier_flags(&local, &first, p->type_qualifier_flags);

                if (p->struct_or_union_specifier)
                {
                    ss_fprintf(&local, "struct %s", p->struct_or_union_specifier->tag_name);
                }
                else if (p->enum_specifier)
                {
                    if (p->enum_specifier->tag_token->lexeme)
                        ss_fprintf(&local, "enum %s", p->enum_specifier->tag_token->lexeme);
                }
                else
                {
                    print_type_specifier_flags(&local, &first, p->type_specifier_flags);
                }



                if (p->name_opt)
                {
                    if (first)
                    {
                        ss_fprintf(ss, " ");
                        first = false;
                    }
                    ss_fprintf(ss, "%s", p->name_opt);
                }

                struct osstream local2 = {0};
                if (ss->c_str)
                    ss_fprintf(&local2, "%s %s", local.c_str, ss->c_str);
                else
                    ss_fprintf(&local2, "%s", local.c_str);

                ss_swap(ss, &local2);
                ss_close(&local);
                ss_close(&local2);
            }
            break;
            case TYPE_CATEGORY_ARRAY:


                if (p->name_opt)
                {
                    //if (first)
                    //{
                      //  ss_fprintf(ss, " ");
                        //first = false;
                    //}
                    ss_fprintf(ss, "%s", p->name_opt);
                }

                ss_fprintf(ss, "[");

                bool b = true;
                if (p->static_array)
                {
                    ss_fprintf(ss, "static");
                    b = false;
                }

                print_type_qualifier_flags(ss, &b, p->type_qualifier_flags);

                if (p->array_size > 0)
                {
                    if (!b)
                        ss_fprintf(ss, " ");

                    ss_fprintf(ss, "%d", p->array_size);
                }
                ss_fprintf(ss, "]");

                break;
            case TYPE_CATEGORY_FUNCTION:

                if (p->name_opt)
                {
                    //if (first)
                    //{
                      //  ss_fprintf(ss, " ");
                        //first = false;
                    //}
                    ss_fprintf(ss, "%s", p->name_opt);
                }
                ss_fprintf(ss, "(");




                struct param* pa = p->params.head;

                while (pa)
                {
                    struct osstream sslocal = {0};
                    print_type(&sslocal, &pa->type);
                    ss_fprintf(ss, "%s", sslocal.c_str);
                    if (pa->next)
                        ss_fprintf(ss, ",");
                    ss_close(&sslocal);
                    pa = pa->next;
                }
                ss_fprintf(ss, ")");
                break;

            case TYPE_CATEGORY_POINTER:
            {
                struct osstream local = {0};
                if (p->next && (
                    (p->next->category == TYPE_CATEGORY_FUNCTION ||
                        p->next->category == TYPE_CATEGORY_ARRAY)))
                {
                    ss_fprintf(&local, "(");
                }

                ss_fprintf(&local, "*");
                bool first = false;
                print_type_qualifier_flags(&local, &first, p->type_qualifier_flags);

                if (p->name_opt)
                {
                    if (!first)
                    {
                        ss_fprintf(ss, " ");
                    }
                    ss_fprintf(ss, "%s", p->name_opt);
                    first = false;
                }

                if (ss->c_str)
                    ss_fprintf(&local, "%s", ss->c_str);

                if (p->next &&
                    (p->next->category == TYPE_CATEGORY_FUNCTION ||
                        p->next->category == TYPE_CATEGORY_ARRAY))
                {
                    ss_fprintf(&local, ")", ss->c_str);
                }

                ss_swap(ss, &local);
                ss_close(&local);
            }
            break;
        }

        p = p->next;

    }
}

void print_type(struct osstream* ss, const struct type* p_type)
{
    print_type_core(ss, p_type, false);
}

void print_type_declarator(struct osstream* ss, const struct type* p_type)
{
    print_type_core(ss, p_type, true);
}

void type_print(const struct type* a)
{
    struct osstream ss = {0};
    print_type(&ss, a);
    puts(ss.c_str);
    puts("\n");
    ss_close(&ss);
}

enum type_category type_get_category(const struct type* p_type)
{
    return p_type->category;
}

void param_list_destroy(struct param_list* obj_owner p)
{
    struct param* owner item = p->head;
    while (item)
    {
        struct param* owner next = item->next;
        type_destroy(&item->type);
        free(item);
        item = next;
    }
}

void type_destroy_one(struct type* obj_owner p_type)
{
    free((void * owner)p_type->name_opt);
    param_list_destroy(&p_type->params);
    assert(p_type->next == NULL);
}

void type_destroy(struct type* obj_owner p_type)
{
    free((void * owner)p_type->name_opt);
    param_list_destroy(&p_type->params);

    struct type* owner item = p_type->next;
    while (item)
    {
        struct type* owner next = item->next;
        item->next = NULL;
        type_destroy_one(item);
        free(item);
        item = next;
    }

}

bool type_has_attribute(const struct type* p_type, enum attribute_flags attributes)
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

bool type_is_maybe_unused(const struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_MAYBE_UNUSED);
}

bool type_is_deprecated(const struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_DEPRECATED);
}

bool type_is_nodiscard(const struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_NODISCARD);
}

bool type_is_array(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ARRAY;
}

bool type_is_lvalue(const struct type* p_type)
{
    return p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_LVALUE;
}

bool type_is_any_owner(const struct type* p_type)
{
    if (type_is_owner(p_type))
    {
        return true;
    }
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_OBJ_OWNER;
}

bool type_is_pointer_to_owner(const struct type* p_type)
{
    return type_is_owner(p_type->next);
}

bool type_is_obj_owner(const struct type* p_type)
{
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_OBJ_OWNER;
}

bool type_is_owner(const struct type* p_type)
{

    if (p_type->struct_or_union_specifier)
    {
        if (p_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW)
            return false;

        struct struct_or_union_specifier* p_complete =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_complete && p_complete->is_owner)
        {
            //The objective here is fix a type later.
            /*
             struct X;
             struct X f(); //X is owner?
             struct X { char * owner p; };
             int main()
             {
               struct X x = 1 ? f() : f();
             }
            */

            return true;
        }
    }

    return p_type->type_qualifier_flags & TYPE_QUALIFIER_OWNER;
}

bool type_is_out(const struct type* p_type)
{
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_OUT;
}

bool type_is_const(const struct type* p_type)
{
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_CONST;
}

bool type_is_pointer_to_const(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_POINTER)
    {
        if (p_type->next)
        {
            return p_type->next->type_qualifier_flags & TYPE_QUALIFIER_CONST;
        }
    }
    return false;
}

bool type_is_void_ptr(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_POINTER)
    {
        if (p_type->next)
        {
            return p_type->next->type_specifier_flags & TYPE_SPECIFIER_VOID;
        }
    }
    return false;
}

bool type_is_void(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_ITSELF)
    {
        return p_type->type_specifier_flags & TYPE_SPECIFIER_VOID;
    }

    return false;
}

bool type_is_nullptr_t(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_ITSELF)
    {
        return p_type->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T;
    }

    return false;
}

bool type_is_pointer(const struct type* p_type)
{
    return p_type->category == TYPE_CATEGORY_POINTER;
}

bool type_is_enum(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM;
}

bool type_is_struct_or_union(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION;
}

/*
  The three types
  char, signed char, and unsigned char
  are collectively called the character types.
*/
bool type_is_character(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR;
}

bool type_is_bool(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL;
}

/*
 There are three standard floating types, designated as
 float, double, and long double.

 There are three decimal floating types, designated as _Decimal32, _Decimal64, and _Decimal128.
*/
bool type_is_floating_point(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_DOUBLE |
            TYPE_SPECIFIER_FLOAT);
}

bool type_is_unsigned_integer(const struct type* p_type)
{
    if (type_is_integer(p_type) &&
        (p_type->type_specifier_flags & TYPE_SPECIFIER_UNSIGNED))
    {
        return true;
    }

    return false;
}
/*
  The type char, the signed and unsigned integer types,
  and the enumerated types
  are collectively  called integer types.
*/
bool type_is_integer(const struct type* p_type)
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
bool type_is_arithmetic(const struct type* p_type)
{
    return type_is_integer(p_type) || type_is_floating_point(p_type);
}

/*
 Arithmetic types, pointer types, and the nullptr_t type are collectively
 called scalar types.
*/
bool type_is_scalar(const struct type* p_type)
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


const struct param_list* type_get_func_or_func_ptr_params(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_FUNCTION)
    {
        return &p_type->params;
    }
    else if (p_type->category == TYPE_CATEGORY_POINTER)
    {
        if (p_type->next &&
            p_type->next->category == TYPE_CATEGORY_FUNCTION)
        {
            return &p_type->next->params;
        }
    }
    return NULL;
}

void check_function_argument_and_parameter2(struct parser_ctx* ctx,
    struct argument_expression* current_argument,
    struct type* paramer_type,
    int param_num)
{
    //            owner     obj_owner  view parameter
    // owner      OK                   OK
    // obj_owner  X         OK         OK
    // view       X (NULL)  X          OK

    const bool paramer_is_obj_owner = type_is_obj_owner(paramer_type);
    const bool paramer_is_owner = type_is_owner(paramer_type);
    const bool paramer_is_view = !paramer_is_obj_owner && !paramer_is_owner;

    const struct type* const argument_type = &current_argument->expression->type;
    const bool argument_is_owner = type_is_owner(&current_argument->expression->type);
    const bool argument_is_obj_owner = type_is_obj_owner(&current_argument->expression->type);
    const bool argument_is_view = !argument_is_owner && !argument_is_obj_owner;

    if (argument_is_owner && paramer_is_owner)
    {
        //ok
    }
    else if (argument_is_owner && paramer_is_obj_owner)
    {
        //ok
    }
    else if (argument_is_owner && paramer_is_view)
    {
        //ok
        if (current_argument->expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
        {
            compiler_set_error_with_token(C_OWNERSHIP_USING_TEMPORARY_OWNER,
                ctx,
                current_argument->expression->first_token,
                "passing a temporary owner to a view");
        }

    }////////////////////////////////////////////////////////////
    else if (argument_is_obj_owner && paramer_is_owner)
    {
        compiler_set_error_with_token(C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
            ctx,
            current_argument->expression->first_token,
            "cannot move obj_owner to owner");
    }
    else if (argument_is_obj_owner && paramer_is_obj_owner)
    {
        //ok
    }
    else if (argument_is_obj_owner && paramer_is_view)
    {
        //ok
        //ok
        if (current_argument->expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
        {
            compiler_set_error_with_token(C_OWNERSHIP_USING_TEMPORARY_OWNER,
                ctx,
                current_argument->expression->first_token,
                "passing a temporary owner to a view");
        }


    }///////////////////////////////////////////////////////////////
    else if (argument_is_view && paramer_is_owner)
    {
        if (!expression_is_zero(current_argument->expression))
        {
            compiler_set_error_with_token(C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                ctx,
                current_argument->expression->first_token,
                "passing a view argument to a owner parameter");
        }
    }
    else if (argument_is_view && paramer_is_obj_owner)
    {
        //check if the contented of pointer is owner.
        if (type_is_pointer(argument_type))
        {
            struct type t2 = type_remove_pointer(argument_type);
            if (!type_is_owner(&t2))
            {

                compiler_set_error_with_token(C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                    ctx,
                    current_argument->expression->first_token,
                    "pointed object is not owner");

            }
            else
            {
                //pointer object is owner 
                if (!argument_type->address_of)
                {
                    //we need something created with address of.
                    compiler_set_error_with_token(C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                        ctx,
                        current_argument->expression->first_token,
                        "obj_owner pointer must be created using address of operator &");
                }
            }

            type_destroy(&t2);
        }
        else
        {
            if (!expression_is_zero(current_argument->expression))
            {
                compiler_set_error_with_token(C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                    ctx,
                    current_argument->expression->first_token,
                    "passing a view argument to a obj_owner parameter");
            }
        }

    }
    else if (argument_is_view && paramer_is_view)
    {
        //ok
    }///////////////////////////////////////////////////////////////
}

void check_function_argument_and_parameter(struct parser_ctx* ctx,
    struct argument_expression* current_argument,
    struct type* paramer_type,
    int param_num)
{

    if (type_is_any_owner(paramer_type))
    {
        if (type_is_obj_owner(paramer_type))
        {
            if (current_argument->expression->type.category == TYPE_CATEGORY_POINTER)
            {
                if (type_is_pointer(&current_argument->expression->type) &&
                    !type_is_pointer_to_owner(&current_argument->expression->type))
                {
                    compiler_set_error_with_token(C_OWNERSHIP_NOT_OWNER, ctx,
                        current_argument->expression->first_token,
                        "parameter %d requires a pointer to owner object",
                        param_num);
                }
            }
            else
            {
                compiler_set_error_with_token(C_OWNERSHIP_NOT_OWNER, ctx,
                    current_argument->expression->first_token,
                    "parameter %d requires a pointer to owner type",
                    param_num);
            }
        }
    }
    struct type* argument_type = &current_argument->expression->type;
    bool is_null_pointer_constant = false;

    if (current_argument)
    {
        if (type_is_nullptr_t(&current_argument->expression->type) ||
            (constant_value_is_valid(&current_argument->expression->constant_value) &&
                constant_value_to_ull(&current_argument->expression->constant_value) == 0))
        {
            is_null_pointer_constant = true;
        }
    }


    struct type parameter_type_converted = (type_is_array(paramer_type)) ?
        type_lvalue_conversion(paramer_type) :
        type_dup(paramer_type);


    struct type argument_type_converted =
        expression_is_subjected_to_lvalue_conversion(current_argument->expression) ?
        type_lvalue_conversion(argument_type) :
        type_dup(argument_type);


    /*
       less generic tests are first
    */
    if (type_is_enum(argument_type) && type_is_enum(paramer_type))
    {
        if (!type_is_same(argument_type, paramer_type, false))
        {
            compiler_set_error_with_token(C_INCOMPATIBLE_TYPES, ctx,
                current_argument->expression->first_token,
                " incompatible types at argument %d", param_num);
        }

        check_function_argument_and_parameter2(ctx,
            current_argument,
            paramer_type,
            param_num);

        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);

        return;
    }

    if (type_is_arithmetic(argument_type) && type_is_arithmetic(paramer_type))
    {
        check_function_argument_and_parameter2(ctx,
            current_argument,
            paramer_type,
            param_num);

        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);

        return;
    }

    if (is_null_pointer_constant && type_is_pointer(paramer_type))
    {
        //TODO void F(int * [[opt]] p)
        // F(0) when passing null we will check if the parameter 
        //have the anotation [[opt]]

        /*can be converted to any type*/
        check_function_argument_and_parameter2(ctx,
            current_argument,
            paramer_type,
            param_num);

        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);

        return;
    }

    if (is_null_pointer_constant && type_is_array(paramer_type))
    {
        compiler_set_warning_with_token(W_NON_NULL,
            ctx,
            current_argument->expression->first_token,
            " passing null as array");

        check_function_argument_and_parameter2(ctx,
            current_argument,
            paramer_type,
            param_num);

        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);

        return;
    }

    /*
       We have two pointers or pointer/array combination
    */
    if (type_is_pointer_or_array(argument_type) && type_is_pointer_or_array(paramer_type))
    {
        if (type_is_void_ptr(argument_type))
        {
            /*void pointer can be converted to any type*/
            check_function_argument_and_parameter2(ctx,
                current_argument,
                paramer_type,
                param_num);

            type_destroy(&parameter_type_converted);
            type_destroy(&argument_type_converted);

            return;
        }

        if (type_is_void_ptr(paramer_type))
        {
            /*any pointer can be converted to void* */
            check_function_argument_and_parameter2(ctx,
                current_argument,
                paramer_type,
                param_num);

            type_destroy(&parameter_type_converted);
            type_destroy(&argument_type_converted);

            return;
        }


        //TODO  lvalue

        if (type_is_array(paramer_type))
        {
            int parameter_array_size = type_get_array_size(paramer_type);
            if (type_is_array(argument_type))
            {
                int argument_array_size = type_get_array_size(argument_type);
                if (parameter_array_size != 0 &&
                    argument_array_size < parameter_array_size)
                {
                    compiler_set_error_with_token(C_ARGUMENT_SIZE_SMALLER_THAN_PARAMETER_SIZE,
                        ctx,
                        current_argument->expression->first_token,
                        " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                }
            }
            else if (is_null_pointer_constant || type_is_nullptr_t(argument_type))
            {
                compiler_set_error_with_token(C_PASSING_NULL_AS_ARRAY,
                    ctx,
                    current_argument->expression->first_token,
                    " passing null as array");
            }
        }



        if (!type_is_same(&argument_type_converted, &parameter_type_converted, false))
        {
            type_print(&argument_type_converted);
            type_print(&parameter_type_converted);

            compiler_set_error_with_token(C_INCOMPATIBLE_TYPES, ctx,
                current_argument->expression->first_token,
                " incompatible types at argument %d", param_num);
            //disabled for now util it works correctly
            //return false;
        }

        if (type_is_pointer(&argument_type_converted) && type_is_pointer(&parameter_type_converted))
        {
            //parameter pointer do non const
            //argument const.
            struct type argument_pointer_to = type_remove_pointer(&argument_type_converted);
            struct type parameter_pointer_to = type_remove_pointer(&parameter_type_converted);
            if (type_is_const(&argument_pointer_to) &&
                !type_is_const(&parameter_pointer_to) &&
                !type_is_any_owner(&parameter_pointer_to))
            {
                compiler_set_error_with_token(C_DISCARDING_CONST_AT_ARGUMENT, ctx,
                    current_argument->expression->first_token,
                    " discarding const at argument %d", param_num);
            }
            type_destroy(&argument_pointer_to);
            type_destroy(&parameter_pointer_to);
        }
        //return true;
    }

    //TODO
    //if (!type_is_same(paramer_type, &current_argument->expression->type, false))
    //{
    //    compiler_set_error_with_token(C1, ctx,
    //        current_argument->expression->first_token,
    //        " incompatible types at argument %d ", param_num);
    //}

    check_function_argument_and_parameter2(ctx,
        current_argument,
        paramer_type,
        param_num);



    type_destroy(&argument_type_converted);
    type_destroy(&parameter_type_converted);
}

void check_owner_rules_assigment(struct parser_ctx* ctx,
    struct type* left_type,
    struct expression* right,
    bool return_assignment)
{

    struct type* p_right_type = &right->type;
    bool is_null_pointer_constant = false;

    if (type_is_nullptr_t(&right->type) ||
        (constant_value_is_valid(&right->constant_value) &&
            constant_value_to_ull(&right->constant_value) == 0))
    {
        is_null_pointer_constant = true;
    }

    struct type lvalue_right_type = {0};
    struct type t2 = {0};

    if (expression_is_subjected_to_lvalue_conversion(right))
    {
        lvalue_right_type = type_lvalue_conversion(p_right_type);
    }
    else
    {
        lvalue_right_type = type_dup(p_right_type);
    }

    if (return_assignment)
    {
        if (type_is_pointer(&right->type) &&
            right->type.next &&
            right->type.next->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTOMATIC_STORAGE)
        {
            compiler_set_warning_with_token(W_RETURN_LOCAL_ADDR,
                ctx,
                right->first_token,
                "function returns address of local variable");
        }

        /*              return | non owner  | owner
            non owner          | OK         | if external, or param
            owner          |   | ERROR      | explicit if local, non explicit if external or param
        */

        if (type_is_owner(&right->type))
        {
            if (type_is_owner(left_type))
            {
                //returning a owning variable to a owner result                                
                // * explicit if local variable                
                // * non explicit if param or external
                // ok if external



            }
            else
            {
                //returning a owning variable to a non owner result
                // * ok if external or param
                if (right->type.storage_class_specifier_flags & STORAGE_SPECIFIER_AUTOMATIC_STORAGE)
                {
                    compiler_set_error_with_token(C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                        ctx,
                        right->first_token,
                        "returning a owner variable to a non owner result");
                }
            }
        }
        else
        {
            if (type_is_owner(left_type))
            {
                if (type_is_pointer(left_type) &&
                    constant_value_is_valid(&right->constant_value) &&
                    constant_value_to_bool(&right->constant_value) == false)
                {
                    //ok
                }
                else
                {
                    //returning a non owning variable to owner
                    compiler_set_error_with_token(C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                        ctx,
                        right->first_token,
                        "returning a non owner variable to a owner");
                }
            }
            else
            {
                //returning a non owning variable to non owner
                //ok
            }
        }
    }
    else
    {
        if (type_is_owner(left_type))
        {
            if (type_is_owner(&right->type))
            {
                //owner = owner

            }
            else
            {
                //owner = non-owner
                if (!is_null_pointer_constant)
                {
                    compiler_set_error_with_token(C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                        ctx,
                        right->first_token,
                        "move assignment needs a owner type on right side");
                }
            }
        }
        else
        {
            if (type_is_owner(&right->type))
            {
                //non owner = owner

                if (right->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
                {
                    //non owner = (owner) f()
                    compiler_set_error_with_token(C_OWNERSHIP_NON_OWNER_MOVE,
                        ctx,
                        right->first_token,
                        "cannot move a temporary owner to non-owner");
                }



            }
            else
            {
                //non owner = non owner


            }
        }
    }
    if (right->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER)
    {
        if (right->expression_type == POSTFIX_FUNCTION_CALL)
        {
            //p = f();
            if (!type_is_owner(left_type))
            {
                compiler_set_error_with_token(C_OWNERSHIP_MISSING_OWNER_QUALIFIER, ctx, right->first_token, "left type must be owner qualified ");
            }
        }
    }



    type_destroy(&lvalue_right_type);
    type_destroy(&t2);

}

void check_assigment(struct parser_ctx* ctx,
    struct type* left_type,
    struct expression* right,
    bool return_assignment)
{

    struct type* p_right_type = &right->type;
    bool is_null_pointer_constant = false;

    if (type_is_nullptr_t(&right->type) ||
        (constant_value_is_valid(&right->constant_value) &&
            constant_value_to_ull(&right->constant_value) == 0))
    {
        is_null_pointer_constant = true;
    }

    struct type lvalue_right_type = {0};
    struct type t2 = {0};

    if (expression_is_subjected_to_lvalue_conversion(right))
    {
        lvalue_right_type = type_lvalue_conversion(p_right_type);
    }
    else
    {
        lvalue_right_type = type_dup(p_right_type);
    }


    if (!(type_is_owner(&right->type)) && type_is_owner(left_type))
    {
        if (!is_null_pointer_constant)
        {
            compiler_set_error_with_token(C_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN, ctx, right->first_token, "cannot assign a non owner to owner");

            check_owner_rules_assigment(ctx,
                left_type,
                right,
                return_assignment);

            type_destroy(&lvalue_right_type);
            type_destroy(&t2);

            return;
        }
    }


    /*
       less generic tests are first
    */
    if (type_is_enum(p_right_type) && type_is_enum(left_type))
    {
        if (!type_is_same(p_right_type, left_type, false))
        {
            compiler_set_error_with_token(C_INCOMPATIBLE_ENUN_TYPES, ctx,
                right->first_token,
                " incompatible types ");
        }
        check_owner_rules_assigment(ctx,
            left_type,
            right,
            return_assignment);

        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }

    if (type_is_arithmetic(p_right_type) && type_is_arithmetic(left_type))
    {
        check_owner_rules_assigment(ctx,
            left_type,
            right,
            return_assignment);
        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }

    if (is_null_pointer_constant && type_is_pointer(left_type))
    {
        //TODO void F(int * [[opt]] p)
        // F(0) when passing null we will check if the parameter 
        //have the anotation [[opt]]

        /*can be converted to any type*/
        check_owner_rules_assigment(ctx,
            left_type,
            right,
            return_assignment);
        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }

    if (is_null_pointer_constant && type_is_array(left_type))
    {
        compiler_set_warning_with_token(W_NON_NULL,
            ctx,
            right->first_token,
            " passing null as array");

        check_owner_rules_assigment(ctx,
            left_type,
            right,
            return_assignment);
        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }

    /*
       We have two pointers or pointer/array combination
    */
    if (type_is_pointer_or_array(p_right_type) && type_is_pointer_or_array(left_type))
    {
        if (type_is_void_ptr(p_right_type))
        {
            /*void pointer can be converted to any type*/
            check_owner_rules_assigment(ctx,
                left_type,
                right,
                return_assignment);
            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }

        if (type_is_void_ptr(left_type))
        {
            /*any pointer can be converted to void* */
            check_owner_rules_assigment(ctx,
                left_type,
                right,
                return_assignment);
            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }


        //TODO  lvalue

        if (type_is_array(left_type))
        {
            int parameter_array_size = type_get_array_size(left_type);
            if (type_is_array(p_right_type))
            {
                int argument_array_size = type_get_array_size(p_right_type);
                if (parameter_array_size != 0 &&
                    argument_array_size < parameter_array_size)
                {
                    compiler_set_error_with_token(C_ARGUMENT_SIZE_SMALLER_THAN_PARAMETER_SIZE, ctx,
                        right->first_token,
                        " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                }
            }
            else if (is_null_pointer_constant || type_is_nullptr_t(p_right_type))
            {
                compiler_set_error_with_token(C_PASSING_NULL_AS_ARRAY, ctx,
                    right->first_token,
                    " passing null as array");
            }
            t2 = type_lvalue_conversion(left_type);
        }
        else
        {
            t2 = type_dup(left_type);
        }



        if (!type_is_same(&lvalue_right_type, &t2, false))
        {
            type_print(&lvalue_right_type);
            type_print(&t2);

            compiler_set_error_with_token(C_INCOMPATIBLE_TYPES, ctx,
                right->first_token,
                " incompatible types at argument ");
            //disabled for now util it works correctly
            //return false;
        }

        if (type_is_pointer(&lvalue_right_type) && type_is_pointer(&t2))
        {
            //parameter pointer do non const
            //argument const.
            struct type argument_pointer_to = type_remove_pointer(&lvalue_right_type);
            struct type parameter_pointer_to = type_remove_pointer(&t2);
            if (type_is_const(&argument_pointer_to) && !type_is_const(&parameter_pointer_to))
            {
                compiler_set_error_with_token(C_DISCARDING_CONST_AT_ARGUMENT, ctx,
                    right->first_token,
                    " discarding const at argument ");
            }
            type_destroy(&argument_pointer_to);
            type_destroy(&parameter_pointer_to);
        }
        //return true;
    }

    if (!type_is_same(left_type, &lvalue_right_type, false))
    {
        //TODO more rules..but it is good to check worst case!
        // 
        //  compiler_set_error_with_token(C1, ctx,
        //      right->first_token,
        //      " incompatible types ");
    }

    check_owner_rules_assigment(ctx,
        left_type,
        right,
        return_assignment);



    type_destroy(&lvalue_right_type);
    type_destroy(&t2);

}

bool type_is_function(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_FUNCTION;
}

bool type_is_function_or_function_pointer(const struct type* p_type)
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

struct type type_add_pointer(const struct type* p_type)
{
    struct type r = type_dup(p_type);

    struct type* owner p = calloc(1, sizeof(struct type));
    *p = r;
    r = (struct type){0};
    r.next = p;
    r.category = TYPE_CATEGORY_POINTER;

    r.storage_class_specifier_flags = p_type->storage_class_specifier_flags;

    return r;
}

struct type type_remove_pointer(const struct type* p_type)
{
    assert(type_is_pointer(p_type));

    struct type r = type_dup(p_type);

    assert(r.next);
    if (r.next)
    {
        struct type next = *r.next;
        /*
          we have moved the contents of r.next, but we also need to delete it's memory
        */
        free(r.next);
        r.next = NULL;
        type_destroy_one(&r);
        r = next;
    }

    r.storage_class_specifier_flags = p_type->next->storage_class_specifier_flags;
    r.type_qualifier_flags = p_type->next->type_qualifier_flags;

    return r;
}


struct type get_array_item_type(const struct type* p_type)
{
    struct type r = type_dup(p_type);

    struct type r2 = *r.next;

    free(r.next);
    free((void * owner) r.name_opt);
    param_list_destroy(&r.params);

    return r2;
}

struct type type_param_array_to_pointer(const struct type* p_type)
{
    assert(type_is_array(p_type));
    struct type t = get_array_item_type(p_type);
    struct type t2 = type_add_pointer(&t);

    if (p_type->type_qualifier_flags & TYPE_QUALIFIER_CONST)
    {
        /*
         void F(int a[static const 5]) {
          static_assert((typeof(a)) == (int* const));
        }
        */
        t2.type_qualifier_flags |= TYPE_QUALIFIER_CONST;
    }

    type_destroy(&t);
    t2.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_PARAMETER;

    return t2;
}

bool type_is_pointer_or_array(const struct type* p_type)
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

int type_common(struct type* p_type1, struct type* p_type2, struct type* out_type)
{
    struct type t0 = {0};
    try
    {
        int rank_left = type_get_rank(p_type1);
        if (rank_left < 0) throw;

        int rank_right = type_get_rank(p_type2);
        if (rank_right < 0) throw;

        if (rank_left >= rank_right)
            t0 = type_dup(p_type1);
        else
            t0 = type_dup(p_type2);

        /*
           The result of expression +,- * / etc are not lvalue
        */
        out_type->storage_class_specifier_flags &= ~STORAGE_SPECIFIER_LVALUE;
    }
    catch
    {
        return 1;
    }

    type_swap(out_type, &t0);
    type_destroy(&t0);

    return 0;
}

void type_set(struct type* a, const struct type* b)
{
    struct type t = type_dup(b);
    type_swap(&t, a);
    type_destroy(&t);
}

struct type type_dup(const struct type* p_type)
{
    struct type_list l = {0};
    const struct type* p = p_type;
    while (p)
    {
        struct type* owner p_new = calloc(1, sizeof(struct type));
        *p_new = *p;

        //actually I was not the owner of p_new->next
        static_set(p_new->next, "uninitialized");
        p_new->next = NULL;

        if (p->name_opt)
        {
            //actually p_new->name_opt was not mine..
            static_set(p_new->name_opt, "uninitialized");
            p_new->name_opt = strdup(p->name_opt);
        }

        if (p->category == TYPE_CATEGORY_FUNCTION)
        {
            //actually p_new->params.head  p_new->params.tail and was not mine..
            static_set(p_new->params.head, "uninitialized");
            p_new->params.head = NULL;
            static_set(p_new->params.tail, "uninitialized");
            p_new->params.tail = NULL;

            struct param* p_param = p->params.head;
            while (p_param)
            {
                struct param* owner p_new_param = calloc(1, sizeof * p_new_param);
                p_new_param->type = type_dup(&p_param->type);

                LIST_ADD(&p_new->params, p_new_param);
                p_param = p_param->next;
            }
        }

        type_list_push_back(&l, p_new);
        p = p->next;
    }

    struct type r = *l.head;
    /*
       we have moved the content of l.head
       but we also need to delete the memory
    */
    free(l.head);

    return r;
}


int type_get_array_size(const struct type* p_type)
{
    return p_type->array_size;
}


int type_set_array_size(struct type* p_type, int size)
{
    p_type->array_size = size;
    return 0;
}
int type_get_num_members(const struct type* type);
int type_get_struct_num_members(struct struct_or_union_specifier* complete_struct_or_union_specifier)
{
    int count = 0;
    struct member_declaration* d = complete_struct_or_union_specifier->member_declaration_list.head;
    while (d)
    {
        if (d->member_declarator_list_opt)
        {
            struct member_declarator* md = d->member_declarator_list_opt->head;
            while (md)
            {
                count += 1;
                md = md->next;
            }
        }
        d = d->next;
    }

    return count;
}

int type_get_sizeof(const struct type* p_type);
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
        else
        {
            if (d->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct type t = {0};
                t.category = TYPE_CATEGORY_ITSELF;
                t.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;
                t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                int align = type_get_alignof(&t);

                if (align > maxalign)
                {
                    maxalign = align;
                }
                if (size % align != 0)
                {
                    size += align - (size % align);
                }
                size += type_get_sizeof(&t);
                type_destroy(&t);
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
int type_get_alignof(const struct type* p_type);
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

            struct type type = {0};
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

int type_get_alignof(const struct type* p_type)
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

int type_get_num_members(const struct type* p_type)
{
    enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        return 0;
    }
    else if (category == TYPE_CATEGORY_FUNCTION)
    {
        return 0;
    }
    else if (category == TYPE_CATEGORY_ITSELF)
    {
        if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            struct struct_or_union_specifier* p_complete =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);


            if (p_complete)
            {
                return type_get_struct_num_members(p_complete);
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else if (category == TYPE_CATEGORY_ARRAY)
    {
        //int arraysize = type_get_array_size(p_type);
        //struct type type = get_array_item_type(p_type);
        //int sz = type_get_sizeof(&type);
        //size = sz * arraysize;
        //type_destroy(&type);
        //assert(false);
        return 1;
    }
    return 0;
}

int type_get_sizeof(const struct type* p_type)
{
    int size = 0;

    enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        size = (int) sizeof(void*);
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
            size = (int) sizeof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        {
            size = (int) sizeof(_Bool);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        {
            size = (int) sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT)
        {
            size = (int) sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            //TODO enum type
            size = (int) sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            size = (int) sizeof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        {
            size = (int) sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT64)
        {
            size = (int) sizeof(long long);
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
            size = (int) sizeof(float);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        {
            size = (int) sizeof(double);
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
            size = (int) sizeof(int);
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
        int arraysize = type_get_array_size(p_type);
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
    if (type_is_struct_or_union(p_type))
    {
        struct osstream ss = {0};
        struct struct_or_union_specifier* p_complete =
            p_type->struct_or_union_specifier->complete_struct_or_union_specifier_indirection;
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

        hash = string_hash(ss.c_str);
        ss_close(&ss);
    }
    else if (type_is_enum(p_type))
    {
        struct osstream ss = {0};

        struct enum_specifier* p_complete =
            p_type->enum_specifier->complete_enum_specifier;


        if (p_complete)
        {
            //struct token* current = p_complete->first_token;
           // for (;
             //   current != p_complete->last_token->next;
               // current = current->next)
            //{
              //  if (current->flags & TK_FLAG_FINAL)
                //{
                  //  ss_fprintf(&ss, "%s", current->lexeme);
//
  //              }
    //        }
        }

        hash = string_hash(ss.c_str);
        ss_close(&ss);
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







struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);


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




struct type get_function_return_type(const struct type* p_type)
{

    if (type_is_pointer(p_type))
    {
        /*pointer to function returning ... */
        struct type r = type_dup(p_type->next->next);
        return r;
    }


    /*function returning ... */
    struct type r = type_dup(p_type->next);
    return r;
}


void type_set_int(struct type* p_type)
{
    p_type->type_specifier_flags = TYPE_SPECIFIER_INT;
    p_type->type_qualifier_flags = 0;
    p_type->category = TYPE_CATEGORY_ITSELF;
}

struct type type_make_enumerator(const struct enum_specifier* enum_specifier)
{
    struct type t = {0};
    t.type_specifier_flags = TYPE_SPECIFIER_ENUM;
    t.enum_specifier = enum_specifier;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}
struct type type_get_enum_type(const struct type* p_type)
{
    assert(p_type->enum_specifier != NULL);

    if (p_type->enum_specifier->complete_enum_specifier &&
        p_type->enum_specifier->complete_enum_specifier->specifier_qualifier_list)
    {
        struct type t = {0};
        t.type_qualifier_flags = p_type->enum_specifier->complete_enum_specifier->specifier_qualifier_list->type_qualifier_flags;
        t.type_specifier_flags = p_type->enum_specifier->complete_enum_specifier->specifier_qualifier_list->type_specifier_flags;
        return t;
    }

    struct type t = {0};
    t.type_specifier_flags = TYPE_SPECIFIER_INT;
    return t;
}

struct type type_make_size_t()
{
    struct type t = {0};

#ifdef _WIN64
    t.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT64;
#else
    t.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT;
#endif

    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type make_void_ptr_type()
{
    struct type t = {0};
    t.category = TYPE_CATEGORY_POINTER;

    struct type* owner p = calloc(1, sizeof * p);
    p->category = TYPE_CATEGORY_ITSELF;
    p->type_specifier_flags = TYPE_SPECIFIER_VOID;
    t.next = p;

    return t;
}

struct type make_void_type()
{
    struct type t = {0};
    t.type_specifier_flags = TYPE_SPECIFIER_VOID;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_int_bool_like()
{
    struct type t = {0};
    t.type_specifier_flags = TYPE_SPECIFIER_INT;
    t.attributes_flags = CAKE_HIDDEN_ATTRIBUTE_LIKE_BOOL;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_int()
{
    struct type t = {0};
    t.type_specifier_flags = TYPE_SPECIFIER_INT;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_literal_string(int size, enum type_specifier_flags chartype)
{
    struct type t = {0};
    t.category = TYPE_CATEGORY_ARRAY;
    t.array_size = size;
    struct type* owner p2 = calloc(1, sizeof(struct type));
    p2->category = TYPE_CATEGORY_ITSELF;
    p2->type_specifier_flags = chartype;
    t.next = p2;
    return t;
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




bool type_is_same(const struct type* a, const struct type* b, bool compare_qualifiers)
{
    const struct type* pa = a;
    const struct type* pb = b;


    while (pa && pb)
    {


        if (pa->array_size != pb->array_size) return false;

        if (pa->category != pb->category) return false;

        if (pa->enum_specifier &&
            pb->enum_specifier &&
            pa->enum_specifier->complete_enum_specifier !=
            pb->enum_specifier->complete_enum_specifier)
        {
            return false;
        }


        if (pa->enum_specifier && !pb->enum_specifier)
        {
            //TODO enum with types
            //enum  x int
           //return false;
        }

        if (!pa->enum_specifier && pb->enum_specifier)
        {
            //TODO enum with types
            //int x enum
            //return false;
        }

        //if (pa->name_opt != pb->name_opt) return false;
        if (pa->static_array != pb->static_array) return false;

        if (pa->category == TYPE_CATEGORY_FUNCTION)
        {

            if (pa->params.is_var_args != pb->params.is_var_args)
            {
                return false;
            }

            if (pa->params.is_void != pb->params.is_void)
            {
                return false;
            }

            struct param* p_param_a = pa->params.head;
            struct param* p_param_b = pb->params.head;
            while (p_param_a && p_param_b)
            {
                if (!type_is_same(&p_param_a->type, &p_param_b->type, true))
                {
                    return false;
                }
                p_param_a = p_param_a->next;
                p_param_b = p_param_b->next;
            }
            return p_param_a == NULL && p_param_b == NULL;
        }

        if (pa->struct_or_union_specifier &&
            pb->struct_or_union_specifier)
        {

            if (pa->struct_or_union_specifier->complete_struct_or_union_specifier_indirection !=
                pb->struct_or_union_specifier->complete_struct_or_union_specifier_indirection)
            {
                //this should work but it is not... 
            }

            if (strcmp(pa->struct_or_union_specifier->tag_name, pb->struct_or_union_specifier->tag_name) != 0)
            {
                return false;
            }
        }

        if (compare_qualifiers && pa->type_qualifier_flags != pb->type_qualifier_flags)
        {
            return false;
        }

        if (pa->type_specifier_flags != pb->type_specifier_flags)
        {
            return false;
        }


        pa = pa->next;
        pb = pb->next;
    }
    return pa == NULL && pb == NULL;
}


void type_swap(view struct type* a, view struct type* b)
{
    view struct type temp = *a;
    *a = *b;
    *b = temp;
}


void type_visit_to_mark_anonymous(struct type* p_type)
{
    //TODO better visit?
    if (p_type->struct_or_union_specifier != NULL &&
        p_type->struct_or_union_specifier->has_anonymous_tag)
    {
        if (p_type->struct_or_union_specifier->complete_struct_or_union_specifier_indirection)
        {
            p_type->struct_or_union_specifier->complete_struct_or_union_specifier_indirection->show_anonymous_tag = true;
        }
        p_type->struct_or_union_specifier->show_anonymous_tag = true;
    }

}


void type_merge_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    
    enum type_qualifier_flags type_qualifier_flags = 0;
    if (pdeclarator->declaration_specifiers)
    {
        type_qualifier_flags = pdeclarator->declaration_specifiers->type_qualifier_flags;
        
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        type_qualifier_flags = pdeclarator->specifier_qualifier_list->type_qualifier_flags;
        
    }

    p_type->type_qualifier_flags |= type_qualifier_flags;




}


void type_set_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    
    enum type_qualifier_flags type_qualifier_flags = 0;
    if (pdeclarator->declaration_specifiers)
    {
        type_qualifier_flags = pdeclarator->declaration_specifiers->type_qualifier_flags;
     
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        type_qualifier_flags = pdeclarator->specifier_qualifier_list->type_qualifier_flags;
     
    }

    p_type->type_qualifier_flags = type_qualifier_flags;



}

void type_set_storage_specifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    /*if we have a declarator then we have a lvalue*/
    p_type->storage_class_specifier_flags |= STORAGE_SPECIFIER_LVALUE;

    if (pdeclarator->declaration_specifiers)
    {
        p_type->storage_class_specifier_flags |=
            pdeclarator->declaration_specifiers->storage_class_specifier_flags;
    }
    else
    {
        //struct member
        //assert(false);
        /*
           where we don't have specifiers?
        */
        //p_type->storage_class_specifier_flags |= STORAGE_SPECIFIER_AUTO;
    }
}


void type_set_specifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
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

void type_set_attributes_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        if (pdeclarator->declaration_specifiers->attributes_flags & STD_ATTRIBUTE_NODISCARD)
        {
            p_type->storage_class_specifier_flags |= STORAGE_SPECIFIER_FUNCTION_RETURN_NODISCARD;
        }
        p_type->attributes_flags =
            pdeclarator->declaration_specifiers->attributes_flags;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        //p_type->attributes_flags =
          //  pdeclarator->specifier_qualifier_list->attributes_flags;
    }
}


void type_list_push_front(struct type_list* books, struct type* owner new_book)
{
    assert(books != NULL);
    assert(new_book != NULL);
    assert(new_book->next == NULL);

    if (books->head == NULL)
    {
        books->head = new_book;
        books->tail = new_book;
    }
    else
    {
        new_book->next = books->head;
        books->head = new_book;
    }
}


void type_list_push_back(struct type_list* type_list, struct type* owner new_book)
{
    assert(type_list != NULL);
    assert(new_book != NULL);

    if (type_list->tail == NULL)
    {
        assert(type_list->head == NULL);
        type_list->head = new_book;
    }
    else
    {
        assert(type_list->tail->next == NULL);
        type_list->tail->next = new_book;
    }

    type_list->tail = new_book;
}

void make_type_using_declarator_core(struct parser_ctx* ctx, struct declarator* pdeclarator, char** ppname, struct type_list* list);

void  make_type_using_direct_declarator(struct parser_ctx* ctx,
    struct direct_declarator* pdirectdeclarator,
    char** ppname,
    struct type_list* list)
{
    if (pdirectdeclarator->declarator)
    {
        make_type_using_declarator_core(ctx, pdirectdeclarator->declarator, ppname, list);
    }

    else if (pdirectdeclarator->function_declarator)
    {
        if (pdirectdeclarator->function_declarator->direct_declarator)
        {
            make_type_using_direct_declarator(ctx,
                pdirectdeclarator->function_declarator->direct_declarator,
                ppname,
                list);
        }

        struct type* owner p_func = calloc(1, sizeof(struct type));
        p_func->category = TYPE_CATEGORY_FUNCTION;


        if (pdirectdeclarator->function_declarator->parameter_type_list_opt &&
            pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list)
        {

            struct parameter_declaration* p =
                pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list->head;

            p_func->params.is_var_args = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_var_args;
            p_func->params.is_void = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_void;

            while (p)
            {
                struct param* owner p_new_param = calloc(1, sizeof(struct param));
                p_new_param->type = type_dup(&p->declarator->type);
                assert(p->declarator->type.category != TYPE_CATEGORY_NONE);
                LIST_ADD(&p_func->params, p_new_param);
                p = p->next;
            }
        }


        type_list_push_back(list, p_func);
    }
    else if (pdirectdeclarator->array_declarator)
    {

        if (pdirectdeclarator->array_declarator->direct_declarator)
        {
            make_type_using_direct_declarator(ctx,
                pdirectdeclarator->array_declarator->direct_declarator,
                ppname,
                list);
        }

        struct type* owner p = calloc(1, sizeof(struct type));
        p->category = TYPE_CATEGORY_ARRAY;

        p->array_size =
            (int) array_declarator_get_size(pdirectdeclarator->array_declarator);


        if (pdirectdeclarator->array_declarator->static_token_opt)
        {
            p->static_array = true;
        }

        if (pdirectdeclarator->array_declarator->type_qualifier_list_opt)
        {
            p->type_qualifier_flags = pdirectdeclarator->array_declarator->type_qualifier_list_opt->flags;
        }

        type_list_push_back(list, p);

        // if (pdirectdeclarator->name_opt)
         //{
           //  p->name_opt = strdup(pdirectdeclarator->name_opt->lexeme);
         //}
    }

    if (pdirectdeclarator->name_opt)
    {
        *ppname = pdirectdeclarator->name_opt->lexeme;
    }


}

void make_type_using_declarator_core(struct parser_ctx* ctx, struct declarator* pdeclarator,
    char** ppname, struct type_list* list)
{
    struct type_list pointers = {0};
    struct pointer* pointer = pdeclarator->pointer;
    while (pointer)
    {
        struct type* owner p_flat = calloc(1, sizeof(struct type));

        if (pointer->type_qualifier_list_opt)
        {
            p_flat->type_qualifier_flags = pointer->type_qualifier_list_opt->flags;
        }

        if (pointer->attribute_specifier_sequence_opt)
        {
            p_flat->attributes_flags |= pointer->attribute_specifier_sequence_opt->attributes_flags;
        }
        p_flat->category = TYPE_CATEGORY_POINTER;
        type_list_push_front(&pointers, p_flat); /*invertido*/
        pointer = pointer->pointer;
    }

    if (pdeclarator->direct_declarator)
    {
        make_type_using_direct_declarator(ctx, pdeclarator->direct_declarator, ppname, list);
        if (list->head &&
            list->head->category == TYPE_CATEGORY_FUNCTION)
        {
            if (pointers.head)
            {
                pointers.head->storage_class_specifier_flags |= STORAGE_SPECIFIER_FUNCTION_RETURN;
            }
        }
    }

    while (pointers.head)
    {
        struct type* owner p = pointers.head;
        pointers.head = p->next;
        p->next = NULL;
        type_list_push_back(list, p);
    }

}

struct enum_specifier* declarator_get_enum_specifier(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers &&
        pdeclarator->declaration_specifiers->enum_specifier)
    {
        return pdeclarator->declaration_specifiers->enum_specifier;
    }
    if (pdeclarator->specifier_qualifier_list &&
        pdeclarator->specifier_qualifier_list->enum_specifier)
    {
        return pdeclarator->specifier_qualifier_list->enum_specifier;
    }
    return NULL;
}


struct struct_or_union_specifier* declarator_get_struct_or_union_specifier(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers &&
        pdeclarator->declaration_specifiers->struct_or_union_specifier)
    {
        return pdeclarator->declaration_specifiers->struct_or_union_specifier;
    }
    if (pdeclarator->specifier_qualifier_list &&
        pdeclarator->specifier_qualifier_list->struct_or_union_specifier)
    {
        return pdeclarator->specifier_qualifier_list->struct_or_union_specifier;
    }
    return NULL;
}

struct typeof_specifier* declarator_get_typeof_specifier(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        return pdeclarator->declaration_specifiers->typeof_specifier;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        return pdeclarator->specifier_qualifier_list->typeof_specifier;
    }
    return NULL;
}

struct declarator* declarator_get_typedef_declarator(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        return pdeclarator->declaration_specifiers->typedef_declarator;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        return pdeclarator->specifier_qualifier_list->typedef_declarator;
    }

    return NULL;
}

struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator)
{
    struct type_list list = {0};
    char* name = 0;
    make_type_using_declarator_core(ctx, pdeclarator, &name, &list);

    //type_print(list.head);

    if (declarator_get_typeof_specifier(pdeclarator))
    {
        struct type nt =
            type_dup(&declarator_get_typeof_specifier(pdeclarator)->type);

        struct type* owner p_nt = calloc(1, sizeof(struct type));
        *p_nt = nt;

        bool head = list.head != NULL;

        if (head)
            type_set_qualifiers_using_declarator(list.head, pdeclarator);

        if (list.tail)
        {
            assert(list.tail->next == NULL);
            list.tail->next = p_nt;
        }
        else
        {
            type_list_push_back(&list, p_nt);
        }
    }
    else if (declarator_get_typedef_declarator(pdeclarator))
    {
        struct declarator* p_typedef_declarator =
            declarator_get_typedef_declarator(pdeclarator);

        struct type nt =
            type_dup(&p_typedef_declarator->type);

        struct type* owner p_nt = calloc(1, sizeof(struct type));
        *p_nt = nt;


        /*
          maybe typedef already has const qualifier
          so we cannot override
        */
        type_merge_qualifiers_using_declarator(p_nt, pdeclarator);

        if (list.tail)
        {
            assert(list.tail->next == 0);
            list.tail->next = p_nt;
        }
        else
        {
            type_list_push_back(&list, p_nt);
        }
    }
    else
    {
        struct type* owner p = calloc(1, sizeof(struct type));
        p->category = TYPE_CATEGORY_ITSELF;


        type_set_specifiers_using_declarator(p, pdeclarator);
        type_set_attributes_using_declarator(p, pdeclarator);


        type_set_qualifiers_using_declarator(p, pdeclarator);

        if (list.tail &&
            list.tail->category == TYPE_CATEGORY_FUNCTION)
        {
            p->storage_class_specifier_flags |= STORAGE_SPECIFIER_FUNCTION_RETURN;
        }



        type_list_push_back(&list, p);

        type_set_storage_specifiers_using_declarator(list.head, pdeclarator);


        //if (name)
        //{
          //  if (list.head->name_opt == NULL)
            //{
              //  list.head->name_opt = strdup(name);
            //}
        //}
        //type_set_qualifiers_using_declarator(list.tail, pdeclarator);
    }


    //type_set_qualifiers_using_declarator(list.head, pdeclarator);


#if 0
    if (list.head->category == TYPE_CATEGORY_FUNCTION)
    {
        if (list.head->next)
        {
            if (!type_is_void(list.head->next))
            {
                list.head->next->attributes_flags |= CAKE_HIDDEN_ATTRIBUTE_FUNC_RESULT;
            }
        }
    }
#endif

    if (pdeclarator->name)
    {
        free((void* owner) list.head->name_opt);
        list.head->name_opt = strdup(pdeclarator->name->lexeme);
    }

    struct type r = *list.head;
    /*
      we moved the contents of head
      but we also need to delete the memory
    */
    free(list.head);
    return r;
    }

void type_remove_names(struct type* p_type)
{
    /*
      function parameters names are preserved
    */
    struct type* p = p_type;

    while (p)
    {
        if (p->name_opt)
        {
            free((void* owner) p->name_opt);
            p->name_opt = NULL;
        }
        p = p->next;
    }
}

const struct type* type_get_specifer_part(const struct type* p_type)
{
    /*
     last part is the specifier
    */
    const struct type* p = p_type;
    while (p->next) p = p->next;
    return p;
}