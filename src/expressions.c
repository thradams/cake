/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

//#pragma safety enable

#include "ownership.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "expressions.h"
#include <string.h>
#include "parser.h"
#include "type.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif

struct constant_value make_constant_value_double(double d, bool disabled)
{
    struct constant_value r = { 0 };
    if (disabled)
        return r;
    r.dvalue = d;
    r.type = TYPE_DOUBLE;
    return r;
}

struct constant_value make_constant_value_ull(unsigned long long d, bool disabled)
{
    struct constant_value r = { 0 };
    if (disabled)
        return r;

    r.ullvalue = d;
    r.type = TYPE_UNSIGNED_LONG_LONG;
    return r;
}

struct constant_value make_constant_value_ll(long long d, bool disabled)
{
    struct constant_value r = { 0 };
    if (disabled)
        return r;

    r.llvalue = d;
    r.type = TYPE_LONG_LONG;
    return r;
}

double constant_value_to_double(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:
        break;

    case TYPE_LONG_LONG:
        return (double)a->llvalue;
    case TYPE_DOUBLE:
        return a->dvalue;
    case TYPE_UNSIGNED_LONG_LONG:
        return (double)a->ullvalue;
    }

    return 0;
}

bool constant_value_is_valid(const struct constant_value* a)
{
    return a->type != TYPE_NOT_CONSTANT;
}

void constant_value_to_string(const struct constant_value* a, char buffer[], int sz)
{
    buffer[0] = 0;
    switch (a->type)
    {
    case TYPE_LONG_LONG:
        snprintf(buffer, sz, "%lld", a->llvalue);
        break;
    case TYPE_DOUBLE:
        snprintf(buffer, sz, "%f", a->dvalue);
        break;

    case TYPE_UNSIGNED_LONG_LONG:
        snprintf(buffer, sz, "%llu", a->ullvalue);
        break;
    default:
        return;
    }
}

unsigned long long constant_value_to_ull(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_LONG_LONG:
        return (unsigned long long)a->llvalue;
    case TYPE_DOUBLE:
        return (unsigned long long)a->dvalue;
    case TYPE_UNSIGNED_LONG_LONG:
        return (unsigned long long)a->ullvalue;
    default:
        break;
    }

    return 0;
}
long long constant_value_to_ll(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_LONG_LONG:
        return (long long)a->llvalue;
    case TYPE_DOUBLE:
        return (long long)a->dvalue;
    case TYPE_UNSIGNED_LONG_LONG:
        return (long long)a->ullvalue;
    default:
        break;
    }

    return 0;
}
bool constant_value_to_bool(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_LONG_LONG:
        return a->llvalue != 0;
    case TYPE_DOUBLE:
        return a->dvalue != 0;
    case TYPE_UNSIGNED_LONG_LONG:
        return a->ullvalue != 0;
    default:
        break;
    }

    return 0;
}

struct constant_value constant_value_cast(const struct constant_value* a, enum constant_value_type type)
{
    struct constant_value r = *a;

    if (a->type == TYPE_NOT_CONSTANT)
    {
        return r;
    }

    switch (type)
    {
    case TYPE_NOT_CONSTANT:

    case TYPE_LONG_LONG:
        r.type = TYPE_LONG_LONG;
        r.llvalue = constant_value_to_ll(a);
        break;
    case TYPE_DOUBLE:
        r.type = TYPE_DOUBLE;
        r.dvalue = constant_value_to_double(a);
        break;
    case TYPE_UNSIGNED_LONG_LONG:
        r.type = TYPE_UNSIGNED_LONG_LONG;
        r.ullvalue = constant_value_to_ull(a);
        break;
    }
    return r;
}

struct constant_value constant_value_unary_op(const struct constant_value* a, int op)
{
    struct constant_value r = { 0 };
    if (!constant_value_is_valid(a))
    {
        return r;
    }

    if (a->type == TYPE_DOUBLE)
    {
        r.type = TYPE_DOUBLE;
        switch (op)
        {
        case '!':
            r.dvalue = !a->dvalue;
            break;
            // case '~':r.dvalue = ~ a->dvalue;  break;
        case '+':
            r.dvalue = +a->dvalue;
            break;
        case '-':
            r.dvalue = -a->dvalue;
            break;
        default:
            assert(false);
            break;
        }
        return r;
    }
    else if (a->type == TYPE_UNSIGNED_LONG_LONG)
    {
        r.type = TYPE_UNSIGNED_LONG_LONG;
        switch (op)
        {
        case '!':
            r.ullvalue = !a->ullvalue;
            break;
        case '~':
            r.ullvalue = ~a->ullvalue;
            break;
        case '+':
            r.ullvalue = a->ullvalue;
            break;
            // case '-':r.dvalue = -a->ullvalue;  break;
        case '-':
            r.dvalue = 0; // -a->ullvalue;
            break;
        default:
            assert(false);
            break;
        }
        return r;
    }
    else if (a->type == TYPE_LONG_LONG)
    {
        r.type = TYPE_UNSIGNED_LONG_LONG;
        switch (op)
        {
        case '!':
            r.llvalue = !((long long)a->llvalue);
            break;
        case '~':
            r.llvalue = ~((long long)a->llvalue);
            break;
        case '+':
            r.llvalue = +((long long)a->llvalue);
            break;
        case '-':
            r.llvalue = -((long long)a->llvalue);
            break;
        default:
            assert(false);
            break;
        }
        return r;
    }

    return r;
}

struct constant_value constant_value_op(const struct constant_value* a, const struct constant_value* b, int op)
{
    // TODO https://github.com/thradams/checkedints
    struct constant_value r = { 0 };
    if (!constant_value_is_valid(a) || !constant_value_is_valid(b))
    {
        return r;
    }

    if (a->type == TYPE_DOUBLE || b->type == TYPE_DOUBLE)
    {
        double va = constant_value_to_double(a);
        double vb = constant_value_to_double(b);
        r.type = TYPE_DOUBLE;

        switch (op)
        {
            // Arithmetic Operators
        case '+':
            r.dvalue = va + vb;
            break;
        case '-':
            r.dvalue = va - vb;
            break;
        case '*':
            r.dvalue = va * vb;
            break;
        case '/':
            if (vb != 0)
                r.dvalue = va / vb;
            else
                r.type = TYPE_NOT_CONSTANT;
            break;

            // case '%':r.dvalue = va % vb;  break;

            // Relational Operators
        case '==':
            r.dvalue = va == vb;
            break;
        case '!=':
            r.dvalue = va != vb;
            break;
        case '>':
            r.dvalue = va > vb;
            break;
        case '<':
            r.dvalue = va < vb;
            break;
        case '<=':
            r.dvalue = va <= vb;
            break;
        case '>=':
            r.dvalue = va >= vb;
            break;

            // Logical Operators
        case '&&':
            r.dvalue = va && vb;
            break;
        case '||':
            r.dvalue = va || vb;
            break;

            // Bitwise Operators
            // case '|':r.dvalue = va | vb;  break;
            // case '&':r.dvalue = va & vb;  break;
            // case '^':r.dvalue = va ^ vb;  break;
            // case '>>':r.dvalue = va >> vb;  break;
            // case '<<':r.dvalue = va << vb;  break;

        default:
            assert(false);
            break;
        }

        return r;
    }

    if (a->type == TYPE_UNSIGNED_LONG_LONG || b->type == TYPE_UNSIGNED_LONG_LONG)
    {
        unsigned long long va = constant_value_to_ull(a);
        unsigned long long vb = constant_value_to_ull(b);
        r.type = TYPE_UNSIGNED_LONG_LONG;

        switch (op)
        {
            // Arithmetic Operators
        case '+':
            r.ullvalue = va + vb;
            break;
        case '-':
            r.ullvalue = va - vb;
            break;
        case '*':
            r.ullvalue = va * vb;
            break;
        case '/':
            if (vb != 0)
                r.ullvalue = va / vb;
            else
                r.type = TYPE_NOT_CONSTANT;
            break;

        case '%':
            if (vb != 0)
                r.ullvalue = va % vb;
            else
                r.type = TYPE_NOT_CONSTANT;
            break;

            // Relational Operators
        case '==':
            r.ullvalue = va == vb;
            break;
        case '!=':
            r.ullvalue = va != vb;
            break;
        case '>':
            r.ullvalue = va > vb;
            break;
        case '<':
            r.ullvalue = va < vb;
            break;
        case '<=':
            r.ullvalue = va <= vb;
            break;
        case '>=':
            r.ullvalue = va >= vb;
            break;

            // Logical Operators
        case '&&':
            r.ullvalue = va && vb;
            break;
        case '||':
            r.ullvalue = va || vb;
            break;

            // Bitwise Operators
        case '|':
            r.ullvalue = va | vb;
            break;
        case '&':
            r.ullvalue = va & vb;
            break;
        case '^':
            r.ullvalue = va ^ vb;
            break;
        case '>>':
            r.ullvalue = va >> vb;
            break;
        case '<<':
            r.ullvalue = va << vb;
            break;

        default:
            assert(false);
            break;
        }

        return r;
    }

    unsigned long long va = a->llvalue;
    unsigned long long vb = b->llvalue;
    r.type = TYPE_LONG_LONG;
    switch (op)
    {
        // Arithmetic Operators
    case '+':
        r.llvalue = va + vb;
        break;
    case '-':
        r.llvalue = va - vb;
        break;
    case '*':
        r.llvalue = va * vb;
        break;

    case '/':
        if (vb != 0)
            r.llvalue = va / vb;
        else
            r.type = TYPE_NOT_CONSTANT;
        break;

    case '%':
        if (vb != 0)
            r.llvalue = va % vb;
        else
            r.type = TYPE_NOT_CONSTANT;
        break;

        // Relational Operators
    case '==':
        r.llvalue = va == vb;
        break;
    case '!=':
        r.llvalue = va != vb;
        break;
    case '>':
        r.llvalue = va > vb;
        break;
    case '<':
        r.llvalue = va < vb;
        break;
    case '<=':
        r.llvalue = va <= vb;
        break;
    case '>=':
        r.llvalue = va >= vb;
        break;

        // Logical Operators
    case '&&':
        r.llvalue = va && vb;
        break;
    case '||':
        r.llvalue = va || vb;
        break;

        // Bitwise Operators
    case '|':
        r.llvalue = va | vb;
        break;
    case '&':
        r.llvalue = va & vb;
        break;
    case '^':
        r.llvalue = va ^ vb;
        break;
    case '>>':
        r.llvalue = va >> vb;
        break;
    case '<<':
        r.llvalue = va << vb;
        break;

    default:
        assert(false);
        break;
    }

    return r;
}

struct expression* _Owner _Opt postfix_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt cast_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt multiplicative_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt unary_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt additive_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt shift_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt relational_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt equality_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt and_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt exclusive_or_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt inclusive_or_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt logical_and_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt logical_or_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt conditional_expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt expression(struct parser_ctx* ctx);
struct expression* _Owner _Opt conditional_expression(struct parser_ctx* ctx);

static int compare_function_arguments(struct parser_ctx* ctx,
                                      struct type* p_type,
                                      struct argument_expression_list* p_argument_expression_list)
{
    try
    {
        struct param* _Opt p_current_parameter_type = NULL;

        const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == NULL) throw;

        p_current_parameter_type = p_param_list->head;

        struct argument_expression* _Opt p_current_argument = p_argument_expression_list->head;

        while (p_current_argument && p_current_parameter_type)
        {
            check_assigment(ctx, &p_current_parameter_type->type, p_current_argument->expression, ASSIGMENT_TYPE_PARAMETER);
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }

        if (p_current_argument != NULL && !p_param_list->is_var_args)
        {
            compiler_diagnostic_message(C_ERROR_TOO_MANY_ARGUMENTS, ctx,
                                        p_current_argument->expression->first_token, NULL,
                                        "too many arguments");
            throw;
        }

        if (p_current_parameter_type != NULL && !p_param_list->is_void)
        {
            if (p_argument_expression_list->tail)
            {
                compiler_diagnostic_message(C_ERROR_TOO_FEW_ARGUMENTS, ctx,
                                            p_argument_expression_list->tail->expression->first_token,
                    NULL,
                                            "too few arguments");
            }
            else
            {
                compiler_diagnostic_message(C_ERROR_TOO_FEW_ARGUMENTS, ctx, ctx->current, NULL, "too few arguments");
            }
            throw;
        }
    }
    catch
    {
        return 1; /*error*/
    }

    return 0;
}

bool is_enumeration_constant(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
    {
        return false;
    }

    if (ctx->current->type != TK_IDENTIFIER)
    {
        return false;
    }

    if (ctx->current->flags & TK_FLAG_IDENTIFIER_IS_ENUMERATOR)
        return true;

    if (ctx->current->flags & TK_FLAG_IDENTIFIER_IS_NOT_ENUMERATOR)
        return false;

    const bool is_enumerator = find_enumerator(ctx, ctx->current->lexeme, NULL) != NULL;

    if (is_enumerator)
    {
        ctx->current->flags |= TK_FLAG_IDENTIFIER_IS_ENUMERATOR;
    }
    else
    {
        ctx->current->flags |= TK_FLAG_IDENTIFIER_IS_NOT_ENUMERATOR;
    }

    return is_enumerator;
}

bool is_first_of_floating_constant(const struct parser_ctx* ctx)
{
    /*
     floating-constant:
      decimal-floating-constant
      hexadecimal-floating-constan
    */
    return ctx->current->type == TK_COMPILER_DECIMAL_FLOATING_CONSTANT ||
        ctx->current->type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT;
}

bool is_first_of_integer_constant(const struct parser_ctx* ctx)
{
    /*
     integer-constant:
      decimal-constant integer-suffixopt
      octal-constant integer-suffixopt
      hexadecimal-constant integer-suffixopt
      binary-constant integer-suffixop
    */

    return ctx->current->type == TK_COMPILER_DECIMAL_CONSTANT ||
        ctx->current->type == TK_COMPILER_OCTAL_CONSTANT ||
        ctx->current->type == TK_COMPILER_HEXADECIMAL_CONSTANT ||
        ctx->current->type == TK_COMPILER_BINARY_CONSTANT;
}

bool is_predefined_constant(const struct parser_ctx* ctx)
{
    return ctx->current->type == TK_KEYWORD_TRUE ||
        ctx->current->type == TK_KEYWORD_FALSE ||
        ctx->current->type == TK_KEYWORD_NULLPTR;
}

bool is_first_of_constant(const struct parser_ctx* ctx)
{
    /*
     constant:
      integer-constant
      floating-constant
      enumeration-constant
      character-constant
      predefined-constant
    */
    return is_first_of_integer_constant(ctx) ||
        is_first_of_floating_constant(ctx) ||
        is_enumeration_constant(ctx) ||
        (ctx->current->type == TK_CHAR_CONSTANT) ||
        is_predefined_constant(ctx);
}

bool is_first_of_primary_expression(const struct parser_ctx* ctx)
{
    /*
     primary-expression:
      identifier
      constant
      string-literal
      ( expression )
      generic-selection
      typeid (expression )
    */
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_IDENTIFIER ||
        is_first_of_constant(ctx) ||
        ctx->current->type == TK_STRING_LITERAL ||
        ctx->current->type == '(' ||
        ctx->current->type == TK_KEYWORD__GENERIC;
}

struct generic_association* _Owner _Opt generic_association(struct parser_ctx* ctx)
{
    struct generic_association* _Owner _Opt p_generic_association = NULL;
    try
    {
        if (ctx->current == NULL)
            throw;

        p_generic_association = calloc(1, sizeof * p_generic_association);
        if (p_generic_association == NULL)
            throw;

        p_generic_association->first_token = ctx->current;
        /*generic - association:
            type-name : assignment-expression
            default : assignment-expression
            */
        if (ctx->current->type == TK_KEYWORD_DEFAULT)
        {
            parser_match(ctx);
            if (ctx->current == NULL) throw;
        }
        else if (first_of_type_name(ctx))
        {
            bool old = ctx->inside_generic_association;
            ctx->inside_generic_association = true;


            p_generic_association->p_type_name = type_name(ctx);
            if (p_generic_association->p_type_name == NULL) throw;
            if (p_generic_association->p_type_name->declarator == NULL) throw;

            ctx->inside_generic_association = old;
            p_generic_association->type = make_type_using_declarator(ctx, p_generic_association->p_type_name->declarator);
        }
        else
        {
            compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "unexpected");
        }
        if (parser_match_tk(ctx, ':') != 0)
            throw;

        struct expression* _Owner _Opt p_expression_temp = assignment_expression(ctx);
        if (p_expression_temp == NULL)
        {
            throw;
        }

        p_generic_association->expression = p_expression_temp;

        if (ctx->current == NULL)
        {
            throw;
        }
        p_generic_association->last_token = ctx->current;
    }
    catch
    {
        generic_association_delete(p_generic_association);
        p_generic_association = NULL;
    }

    return p_generic_association;
}

struct generic_assoc_list generic_association_list(struct parser_ctx* ctx)
{
    struct generic_assoc_list list = { 0 };
    try
    {
        struct generic_association* _Owner _Opt p_generic_association =
            generic_association(ctx);

        if (p_generic_association == NULL)
            throw;

        generic_assoc_list_add(&list, p_generic_association);

        while (ctx->current->type == ',')
        {
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            struct generic_association* _Owner _Opt p_generic_association2 = generic_association(ctx);
            if (p_generic_association2 == NULL)
                throw;

            generic_assoc_list_add(&list, p_generic_association2);
        }
    }
    catch
    {
    }
    return list;
}
void generic_association_delete(struct generic_association* _Owner _Opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        type_name_delete(p->p_type_name);
        expression_delete(p->expression);
        type_destroy(&p->type);
        free(p);
    }
}

void generic_assoc_list_add(struct generic_assoc_list* list, struct generic_association* _Owner pitem)
{
    if (list->head == NULL)
    {
        list->head = pitem;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = pitem;
    }
    list->tail = pitem;
}

void generic_assoc_list_destroy(struct generic_assoc_list* _Obj_owner p)
{
    struct generic_association* _Owner _Opt item = p->head;
    while (item)
    {
        struct generic_association* _Owner _Opt next = item->next;
        item->next = NULL;
        generic_association_delete(item);
        item = next;
    }
}
void generic_selection_delete(struct generic_selection* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->expression);
        type_name_delete(p->type_name);
        generic_assoc_list_destroy(&p->generic_assoc_list);
        free(p);
    }
}
struct generic_selection* _Owner _Opt generic_selection(struct parser_ctx* ctx)
{
    /*C23
      generic-selection:
        _Generic ( assignment-expression , generic-assoc-ctx )
    */

    /*
      Extension
      generic-selection:
        "_Generic" ( generic-argument, generic-assoc-list )

        generic-argument:
          assignment-expression
          type-name
    */

    struct generic_selection* _Owner _Opt p_generic_selection = NULL;
    try
    {
        if (ctx->current == NULL) throw;

        p_generic_selection = calloc(1, sizeof * p_generic_selection);
        if (p_generic_selection == NULL)
            throw;

        p_generic_selection->first_token = ctx->current;

        if (parser_match_tk(ctx, TK_KEYWORD__GENERIC) != 0)
            throw;
        if (parser_match_tk(ctx, '(') != 0)
            throw;

        if (first_of_type_name(ctx))
        {
            /*extension*/
            p_generic_selection->type_name = type_name(ctx);
            if (p_generic_selection->type_name == NULL)
                throw;
        }
        else
        {
            p_generic_selection->expression = assignment_expression(ctx);
            if (p_generic_selection->expression == NULL)
                throw;
        }

        if (parser_match_tk(ctx, ',') != 0)
            throw;

        p_generic_selection->generic_assoc_list = generic_association_list(ctx);
        if (p_generic_selection->generic_assoc_list.head == NULL) throw;

        struct type lvalue_type = { 0 };

        struct type* _Opt p_type = NULL;

        if (p_generic_selection->expression)
        {
            p_type = &p_generic_selection->expression->type;

            if (expression_is_subjected_to_lvalue_conversion(p_generic_selection->expression))
            {
                lvalue_type = type_lvalue_conversion(&p_generic_selection->expression->type, ctx->options.null_checks_enabled);
                p_type = &lvalue_type;
            }
        }
        else if (p_generic_selection->type_name)
        {
            p_type = &p_generic_selection->type_name->declarator->type;
        }
        else
        {
            throw;
        }

        struct generic_association* _Opt current = p_generic_selection->generic_assoc_list.head;
        while (current)
        {
            if (current->p_type_name)
            {
                if (type_is_same(p_type, &current->type, true))
                {
                    p_generic_selection->p_view_selected_expression = current->expression;
                    break;
                }
            }
            else
            {
                /*default*/
                p_generic_selection->p_view_selected_expression = current->expression;
            }
            current = current->next;
        }

        type_destroy(&lvalue_type);

        if (ctx->current == NULL) throw;
        p_generic_selection->last_token = ctx->current;

        if (parser_match_tk(ctx, ')') != 0)
        {
            throw;
        }
    }
    catch
    {
        generic_selection_delete(p_generic_selection);
        p_generic_selection = NULL;
    }
    return p_generic_selection;
}

/*
    https://en.wikipedia.org/wiki/UTF-8
    Since the restriction of the Unicode code-space to 21-bit values in 2003,
    UTF-8 is defined to encode code points in one to four bytes, depending on the number
    of significant bits in the numerical value of the code point. The following table shows
    the structure of the encoding. The x characters are replaced by the bits of the code point.

    Code point <->UTF - 8 conversion
    First         | Last           | Byte 1   | Byte 2   | Byte 3   | Byte 4
    --------------| -------------- |----------|----------|----------| ----------
    U+0000      0 | U+007F     127 | 0xxxxxxx |          |          |
    U+0080    128 | U+07FF    2047 | 110xxxxx | 10xxxxxx |          |
    U+0800   2048 | U+FFFF   65535 | 1110xxxx | 10xxxxxx | 10xxxxxx |
    U+10000 65536 | U+10FFFF 69631 | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx
*/

const unsigned char* _Opt utf8_decode(const unsigned char* s, int* c)
{
    if (s[0] == '\0')
    {
        *c = 0;
        return NULL; /*end*/
    }

    const unsigned char* _Opt next = NULL;
    if (s[0] < 0x80)
    {
        *c = s[0];
        assert(*c >= 0x0000 && *c <= 0x007F);
        next = s + 1;
    }
    else if ((s[0] & 0xe0) == 0xc0)
    {
        *c = ((int)(s[0] & 0x1f) << 6) |
            ((int)(s[1] & 0x3f) << 0);
        assert(*c >= 0x0080 && *c <= 0x07FF);
        next = s + 2;
    }
    else if ((s[0] & 0xf0) == 0xe0)
    {
        *c = ((int)(s[0] & 0x0f) << 12) |
            ((int)(s[1] & 0x3f) << 6) |
            ((int)(s[2] & 0x3f) << 0);
        assert(*c >= 0x0800 && *c <= 0xFFFF);
        next = s + 3;
    }
    else if ((s[0] & 0xf8) == 0xf0 && (s[0] <= 0xf4))
    {
        *c = ((int)(s[0] & 0x07) << 18) |
            ((int)(s[1] & 0x3f) << 12) |
            ((int)(s[2] & 0x3f) << 6) |
            ((int)(s[3] & 0x3f) << 0);
        assert(*c >= 0x10000 && *c <= 0x10FFFF);
        next = s + 4;
    }
    else
    {
        *c = -1;      // invalid
        next = s + 1; // skip this byte
    }

    if (*c >= 0xd800 && *c <= 0xdfff)
    {
        *c = -1; // surrogate half
    }

    return next;
}
static bool is_hex_digit(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return true;
    else if (c >= 'a' && c <= 'f')
        return true;
    else if (c >= 'A' && c <= 'F')
        return true;
    return false;
}

static const unsigned char* escape_sequences_decode_opt(const unsigned char* p, int* out_value)
{
    // TODO OVERFLOW CHECK
    if (*p == 'x')
    {
        p++;
        int result = 0;
        while (is_hex_digit(*p))
        {
            int byte = 0;
            if (*p >= '0' && *p <= '9')
                byte = (*p - '0');
            else if (*p >= 'a' && *p <= 'f')
                byte = (*p - 'a') + 10;
            else if (*p >= 'A' && *p <= 'F')
                byte = (*p - 'A') + 10;

            result = (result << 4) | (byte & 0xF);
            p++;
        }

        *out_value = result;
    }
    else if (*p == 'u' || *p == 'U')
    {
        // TODO  assuming input is checked
        // missing tests
        const int num_of_hex_digits = *p == 'U' ? 8 : 4;

        p++;
        unsigned long long result = 0;
        for (int i = 0; i < num_of_hex_digits; i++)
        {
            int byte = 0;
            if (*p >= '0' && *p <= '9')
                byte = (*p - '0');
            else if (*p >= 'a' && *p <= 'f')
                byte = (*p - 'a') + 10;
            else if (*p >= 'A' && *p <= 'F')
                byte = (*p - 'A') + 10;

            result = (result << 4) | (byte & 0xF);
            p++;
        }

        *out_value = (int)result;
    }
    else if (*p == '0')
    {
        // octal digit
        p++;
        int result = 0;
        while ((*p >= '0' && *p <= '7'))
        {
            int byte;
            byte = (*p - '0');
            result = (result << 4) | (byte & 0xF);
            p++;
        }
        *out_value = result;
    }
    else
    {
        switch (*p)
        {
        case 'a':
            *out_value = '\a';
            break;
        case 'b':
            *out_value = '\b';
            break;
        case 'f':
            *out_value = '\f';
            break;
        case 'n':
            *out_value = '\n';
            break;
        case 'r':
            *out_value = '\r';
            break;
            ;
        case 't':
            *out_value = '\t';
            break;
        case '\'':
            *out_value = '\'';
            break;
        case '\\':
            *out_value = '\\';
            break;
        case '"':
            *out_value = '"';
            break;
        default:
            // assume this error is handled at tokenizer
            assert(false);
            break;
        }
        p++;
    }

    return p;
}

struct expression* _Owner _Opt character_constant_expression(struct parser_ctx* ctx)
{
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        if (ctx->current == NULL)
            throw;

        p_expression_node = calloc(1, sizeof * p_expression_node);
        if (p_expression_node == NULL)
            throw;

        p_expression_node->expression_type = PRIMARY_EXPRESSION_CHAR_LITERAL;
        p_expression_node->first_token = ctx->current;
        p_expression_node->last_token = p_expression_node->first_token;
        p_expression_node->type.attributes_flags |= CAKE_HIDDEN_ATTRIBUTE_LIKE_CHAR;
        p_expression_node->type.category = TYPE_CATEGORY_ITSELF;

        const unsigned char* _Opt p = (const unsigned char*)ctx->current->lexeme;

        if (p[0] == 'u' && p[1] == '8')
        {
            p++;
            p++;
            p++;

            // A UTF-8 character constant has type char8_t.
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR;

            int c = 0;
            p = utf8_decode(p, &c);
            if (p == NULL)
            {
                throw;
            }

            if (c == '\\')
                p = escape_sequences_decode_opt(p, &c);

            if (*p != '\'')
            {
                compiler_diagnostic_message(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Unicode character literals may not contain multiple characters.");
            }

            if (c > 0x80)
            {
                compiler_diagnostic_message(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Character too large for enclosing character literal type.");
            }

            p_expression_node->constant_value = make_constant_value_ll(c, ctx->evaluation_is_disabled);
        }
        else if (p[0] == 'u')
        {
            p++;
            p++;

            // A UTF-16 character constant has type char16_t which is an unsigned integer types defined in the <uchar.h> header
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_SHORT;

            int c = 0;
            p = utf8_decode(p, &c);
            if (p == NULL)
            {
                throw;
            }

            if (c == '\\')
                p = escape_sequences_decode_opt(p, &c);

            if (*p != '\'')
            {
                compiler_diagnostic_message(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Unicode character literals may not contain multiple characters.");
            }

            if (c > USHRT_MAX)
            {
                compiler_diagnostic_message(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Character too large for enclosing character literal type.");
            }

            p_expression_node->constant_value = make_constant_value_ll(c, ctx->evaluation_is_disabled);
        }
        else if (p[0] == 'U')
        {
            p++;
            p++;

            // A UTF-16 character constant has type char16_t which is an unsigned integer types defined in the <uchar.h> header
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT;

            int c = 0;
            p = utf8_decode(p, &c);
            if (p == NULL)
            {
                throw;
            }

            if (c == '\\')
                p = escape_sequences_decode_opt(p, &c);

            if (*p != '\'')
            {
                compiler_diagnostic_message(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Unicode character literals may not contain multiple characters.");
            }

            if (c > UINT_MAX)
            {
                compiler_diagnostic_message(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Character too large for enclosing character literal type.");
            }

            p_expression_node->constant_value = make_constant_value_ll(c, ctx->evaluation_is_disabled);
        }
        else if (p[0] == 'L')
        {
            // A wchar_t character constant is prefixed by the letter L
            p++;
            p++;

            p_expression_node->type.type_specifier_flags = CAKE_WCHAR_T_TYPE_SPECIFIER;

            /*
             wchar_t character constant prefixed by the letter L has type wchar_t, an integer type defined in
             the <stddef.h> header. The value of a wchar_t character constant containing a single multibyte
             character that maps to a single member of the extended execution character set is the wide character
             corresponding to that multibyte character in the implementation-defined wide literal encoding
             (6.2.9). The value of a wchar_t character constant containing more than one multibyte character or a
             single multibyte character that maps to multiple members of the extended execution character set,
             or containing a multibyte character or escape sequence not represented in the extended execution
             character set, is implementation-defined.
            */
            long long value = 0;
            while (*p != '\'')
            {
                int c = 0;
                p = utf8_decode(p, &c);
                if (p == NULL)
                {
                    throw;
                }
                if (c == '\\')
                    p = escape_sequences_decode_opt(p, &c);

                if (p == 0)
                    break;
                // TODO \u
                value = value * 256 + c;
#ifdef _WIN32
                if (value > USHRT_MAX)
                {
                    compiler_diagnostic_message(W_OUT_OF_BOUNDS, ctx, ctx->current, NULL, "character constant too long for its type", ctx->current->lexeme);
                    break;
                }
#else
                if (value > UINT_MAX)
                {
                    compiler_diagnostic_message(W_OUT_OF_BOUNDS, ctx, ctx->current, NULL, "character constant too long for its type", ctx->current->lexeme);
                    break;
                }
#endif
            }

            p_expression_node->constant_value = make_constant_value_ll(value, false /*ctx->evaluation_is_disabled*/);
        }
        else
        {
            p++;
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_INT;

            /*
              An integer character constant has type int. The value of an integer character constant containing
              a single character that maps to a single value in the literal encoding (6.2.9) is the numerical value
              of the representation of the mapped character in the literal encoding interpreted as an integer.
              The value of an integer character constant containing more than one character (e.g., ’ab’), or
              containing a character or escape sequence that does not map to a single value in the literal encoding,
              is implementation-defined. If an integer character constant contains a single character or escape
              sequence, its value is the one that results when an object with type char whose value is that of the
              single character or escape sequence is converted to type int.
            */
            long long value = 0;
            while (*p != '\'')
            {
                int c = 0;
                p = utf8_decode(p, &c);
                if (p == NULL)
                {
                    throw;
                }

                if (c == '\\')
                    p = escape_sequences_decode_opt(p, &c);
                if (p == 0)
                    break;
                value = value * 256 + c;
                if (value > INT_MAX)
                {
                    compiler_diagnostic_message(W_OUT_OF_BOUNDS, ctx, ctx->current, NULL, "character constant too long for its type", ctx->current->lexeme);
                    break;
                }
            }
            p_expression_node->constant_value = make_constant_value_ll(value, false /*ctx->evaluation_is_disabled*/);
        }

        parser_match(ctx);
        if (ctx->current == NULL) throw;

        // warning: character constant too long for its type
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }
    return p_expression_node;
}

int convert_to_number(struct token* token, struct expression* p_expression_node, bool disabled)
{

    /*copia removendo os separadores*/
    // um dos maiores buffer necessarios seria 128 bits binario...
    // 0xb1'1'1....
    int c = 0;
    char buffer[128 * 2 + 4] = { 0 };
    const char* s = token->lexeme;
    while (*s)
    {
        if (*s != '\'')
        {
            buffer[c] = *s;
            c++;
        }
        s++;
    }
    enum type_specifier_flags flags = 0;
    parse_number(buffer, &flags);
    p_expression_node->type.type_specifier_flags = flags;

    switch (token->type)
    {
    case TK_COMPILER_DECIMAL_CONSTANT:

        if (flags & TYPE_SPECIFIER_UNSIGNED)
        {
            p_expression_node->constant_value = make_constant_value_ull(strtoull(buffer, NULL, 10), disabled);
        }
        else
        {
            p_expression_node->constant_value = make_constant_value_ll(strtoll(buffer, NULL, 10), disabled);
        }

        break;
    case TK_COMPILER_OCTAL_CONSTANT:
        p_expression_node->constant_value = make_constant_value_ll(strtoll(buffer, NULL, 8), disabled);

        break;
    case TK_COMPILER_HEXADECIMAL_CONSTANT:
        p_expression_node->constant_value = make_constant_value_ll(strtoll(buffer + 2, NULL, 16), disabled);

        break;
    case TK_COMPILER_BINARY_CONSTANT:
        p_expression_node->constant_value = make_constant_value_ll(strtoll(buffer + 2, NULL, 2), disabled);
        break;
    case TK_COMPILER_DECIMAL_FLOATING_CONSTANT:
        p_expression_node->constant_value = make_constant_value_double(strtod(buffer, NULL), disabled);
        break;
    case TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT:
        p_expression_node->constant_value = make_constant_value_double(strtod(buffer + 2, NULL), disabled);
        break;
    default:
        assert(false);
    }

    return 0;
}

static bool is_integer_or_floating_constant(enum token_type type)
{
    return type == TK_COMPILER_DECIMAL_CONSTANT ||
        type == TK_COMPILER_OCTAL_CONSTANT ||
        type == TK_COMPILER_HEXADECIMAL_CONSTANT ||
        type == TK_COMPILER_BINARY_CONSTANT ||
        type == TK_COMPILER_DECIMAL_FLOATING_CONSTANT ||
        type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT;
}

struct expression* _Owner _Opt primary_expression(struct parser_ctx* ctx)
{
    /*
     primary-expression:
      identifier
      constant
      string-literal
      ( expression )
      generic-selection
    */

    if (ctx->current == NULL)
        return NULL;

    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        if (ctx->current->type == TK_IDENTIFIER)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);

            if (p_expression_node == NULL)
                throw;

            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            struct map_entry* _Opt p_entry = find_variables(ctx, ctx->current->lexeme, NULL);

            if (p_entry && p_entry->type == TAG_TYPE_ENUMERATOR)
            {
                struct enumerator* p_enumerator = p_entry->p;
                p_expression_node->expression_type = PRIMARY_EXPRESSION_ENUMERATOR;
                p_expression_node->constant_value = make_constant_value_ll(p_enumerator->value, ctx->evaluation_is_disabled);

                p_expression_node->type = type_make_enumerator(p_enumerator->enum_specifier);
            }
            else if (p_entry &&
                     (p_entry->type == TAG_TYPE_ONLY_DECLARATOR || p_entry->type == TAG_TYPE_INIT_DECLARATOR))
            {
                struct declarator* _Opt p_declarator = NULL;
                struct init_declarator* _Opt p_init_declarator = NULL;
                if (p_entry->type == TAG_TYPE_INIT_DECLARATOR)
                {
                    p_init_declarator = p_entry->p;
                    p_declarator = p_init_declarator->p_declarator;
                }
                else
                {
                    p_declarator = p_entry->p;
                }

                if (type_is_deprecated(&p_declarator->type))
                {
                    compiler_diagnostic_message(W_DEPRECATED, ctx, ctx->current, NULL, "'%s' is deprecated", ctx->current->lexeme);
                }

                p_declarator->num_uses++;
                p_expression_node->declarator = p_declarator;
                p_expression_node->expression_type = PRIMARY_EXPRESSION_DECLARATOR;

                p_expression_node->type = type_dup(&p_declarator->type);
                if (p_init_declarator)
                {
                    if (/*
                         p_init_declarator->p_declarator && */
                         p_init_declarator->p_declarator->declaration_specifiers &&
                         p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR)
                    {
                        if (p_init_declarator->initializer &&
                            p_init_declarator->initializer->assignment_expression)
                        {
                            p_expression_node->constant_value =
                                p_init_declarator->initializer->assignment_expression->constant_value;
                        }
                    }
                }
            }
            else if (ctx->p_current_function_opt &&
                     strcmp(ctx->current->lexeme, "__func__") == 0)
            {

                const char* func_str = "?";


                if (ctx->p_current_function_opt->init_declarator_list.head != NULL &&
                    ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name_opt)
                {
                    func_str =
                        ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name_opt->lexeme;
                }


                p_expression_node->expression_type = PRIMARY_EXPRESSION__FUNC__;
                p_expression_node->first_token = ctx->current;
                p_expression_node->last_token = ctx->current;

                p_expression_node->type = type_make_literal_string(strlen(func_str) + 1, TYPE_SPECIFIER_CHAR);
            }
            else
            {
                compiler_diagnostic_message(C_ERROR_NOT_FOUND, ctx, ctx->current, NULL, "not found '%s'", ctx->current->lexeme);
                throw;
            }
            parser_match(ctx);
            if (ctx->current == NULL) throw;
        }
        else if (ctx->current->type == TK_STRING_LITERAL)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_STRING_LITERAL;
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            enum type_specifier_flags char_type = TYPE_SPECIFIER_CHAR;

            if (get_char_type(ctx->current->lexeme) == 2)
            {
                /*
                   automatically finding out the type of wchar_t to copy
                   GCC or MSVC.
                   windows it is short linux is
                */
                char_type = CAKE_WCHAR_T_TYPE_SPECIFIER;
            }

            p_expression_node->type = type_make_literal_string(string_literal_byte_size(ctx->current->lexeme), char_type);

            parser_match(ctx);
            if (ctx->current == NULL) throw;

            /*
            string concatenation deveria ser em uma phase anterior
            mas como mantemos as forma do fonte aqui foi uma alternativa
            */
            while (ctx->current &&
                   ctx->current->type == TK_STRING_LITERAL)
            {
                parser_match(ctx);
                if (ctx->current == NULL) throw;
            }
        }
        else if (ctx->current->type == TK_CHAR_CONSTANT)
        {
            p_expression_node = character_constant_expression(ctx);
        }

        else if (ctx->current->type == TK_KEYWORD_TRUE ||
                 ctx->current->type == TK_KEYWORD_FALSE)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_PREDEFINED_CONSTANT;
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            p_expression_node->constant_value =
                make_constant_value_ll(ctx->current->type == TK_KEYWORD_TRUE ? 1 : 0, false /*ctx->evaluation_is_disabled*/);

            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_BOOL;
            p_expression_node->type.type_qualifier_flags = 0;

            parser_match(ctx);
            if (ctx->current == NULL) throw;
        }
        else if (ctx->current->type == TK_KEYWORD_NULLPTR)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_PREDEFINED_CONSTANT;
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            p_expression_node->constant_value = make_constant_value_ll(0, false /*ctx->evaluation_is_disabled*/);

            /*TODO nullptr type*/
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_NULLPTR_T;
            p_expression_node->type.type_qualifier_flags = 0;

            parser_match(ctx);
            if (ctx->current == NULL) throw;
        }
        else if (is_integer_or_floating_constant(ctx->current->type))
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;

            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;
            p_expression_node->expression_type = PRIMARY_EXPRESSION_NUMBER;

            convert_to_number(ctx->current, p_expression_node, false /*ctx->evaluation_is_disabled*/);

            parser_match(ctx);
            if (ctx->current == NULL) throw;
        }
        else if (ctx->current->type == TK_KEYWORD__GENERIC)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;
            p_expression_node->first_token = ctx->current;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_GENERIC;

            p_expression_node->generic_selection = generic_selection(ctx);
            if (p_expression_node->generic_selection == NULL)
                throw;

            p_expression_node->last_token = p_expression_node->generic_selection->last_token;

            if (p_expression_node->generic_selection->p_view_selected_expression)
            {
                p_expression_node->type = type_dup(&p_expression_node->generic_selection->p_view_selected_expression->type);

                p_expression_node->constant_value = p_expression_node->generic_selection->p_view_selected_expression->constant_value;
            }
            else
            {
                compiler_diagnostic_message(C_ERROR_NO_MATCH_FOR_GENERIC, ctx, ctx->current, NULL, "no match for generic");
            }
        }
        else if (ctx->current->type == '(')
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL) throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_PARENTESIS;
            p_expression_node->first_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            p_expression_node->right = expression(ctx);
            if (p_expression_node->right == NULL)
                throw;

            p_expression_node->type = type_dup(&p_expression_node->right->type);
            p_expression_node->constant_value = p_expression_node->right->constant_value;

            if (ctx->current == NULL) throw;

            p_expression_node->last_token = ctx->current;
            if (parser_match_tk(ctx, ')') != 0)
                throw;
        }
        else
        {
            compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "unexpected");
            throw;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    assert(p_expression_node == NULL || (p_expression_node->first_token && p_expression_node->last_token));

    return p_expression_node;
}

void argument_expression_delete(struct argument_expression* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->expression);
        assert(p->next == NULL);
        free(p);
    }
}

struct argument_expression_list argument_expression_list(struct parser_ctx* ctx)
{
    /*
     argument-expression-list:
      assignment-expression
      argument-expression-ctx , assignment-expression
    */

    /*
     argument-expression-list: (extended)
      assignment-expression
      move assignment-expression
      argument-expression-ctx , assignment-expression
      argument-expression-ctx , assignment-expression
    */

    struct argument_expression_list list = { 0 };
    struct argument_expression* _Owner _Opt p_argument_expression = NULL;

    try
    {
        p_argument_expression = calloc(1, sizeof(struct argument_expression));
        if (p_argument_expression == NULL)
            throw;

        struct expression* _Owner _Opt p_assignment_expression = assignment_expression(ctx);
        if (p_assignment_expression == NULL)
        {
            argument_expression_delete(p_argument_expression);
            throw;
        }

        p_argument_expression->expression = p_assignment_expression;
        argument_expression_list_push(&list, p_argument_expression);


        while (ctx->current->type == ',')
        {
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            struct argument_expression* _Owner _Opt p_argument_expression_2 = calloc(1, sizeof * p_argument_expression_2);
            if (p_argument_expression_2 == NULL)
                throw;
            struct expression* _Owner _Opt p_assignment_expression_2 = assignment_expression(ctx);
            if (p_assignment_expression_2 == NULL)
            {
                argument_expression_delete(p_argument_expression_2);
                throw;
            }
            p_argument_expression_2->expression = p_assignment_expression_2;
            if (p_argument_expression_2->expression == NULL)
            {
                argument_expression_delete(p_argument_expression_2);
                throw;
            }

            argument_expression_list_push(&list, p_argument_expression_2);
        }
    }
    catch
    {
    }
    return list;
}

bool first_of_postfix_expression(struct parser_ctx* ctx)
{
    //( type-name )  postfix confunde com (expression) primary
    if (first_of_type_name_ahead(ctx))
        return true;// I don't think it's necessary because primary also works for postfix
    return is_first_of_primary_expression(ctx);
}

static void fix_member_type(struct type* p_type, const struct type* struct_type, const struct type* member_type)
{
    if (struct_type->type_qualifier_flags & TYPE_QUALIFIER_CONST)
    {
        /*
          struct X { int i; };
          const struct X x;
          x.i ;//x.i is const
        */
        p_type->type_qualifier_flags |= TYPE_QUALIFIER_CONST;
    }

    /*
          struct X { int i; };
          const struct X x;
          x.i ;//x.i is also local, or parameter etc.
    */
    p_type->storage_class_specifier_flags = struct_type->storage_class_specifier_flags;

    if (struct_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW)
    {
        /*
          struct X { _Owner int i; };
          _View struct X x;
          x.i ;//is is not _Owner
        */
        p_type->type_qualifier_flags &= ~TYPE_QUALIFIER_OWNER;
    }

    if (struct_type->type_qualifier_flags & TYPE_QUALIFIER_NULLABLE)
    {
        /*
          struct X { _Owner int i; };
          _View struct X x;
          x.i ;//is is not _Owner
        */
        p_type->type_qualifier_flags |= TYPE_QUALIFIER_NULLABLE;
    }

}

static void fix_arrow_member_type(struct type* p_type, const struct type* left, const struct type* member_type)
{
    struct type t = type_remove_pointer(left);

    if (t.type_qualifier_flags & TYPE_QUALIFIER_CONST)
    {
        /*
           const struct X * p;
        */

        p_type->type_qualifier_flags |= TYPE_QUALIFIER_CONST;
    }

    if (t.type_qualifier_flags & TYPE_QUALIFIER_NULLABLE)
    {
        /*
           const struct X * p;
        */

        p_type->type_qualifier_flags |= TYPE_QUALIFIER_NULLABLE;
    }

    if (t.type_qualifier_flags & TYPE_QUALIFIER_VIEW)
    {
        /*
          _View struct X * p;
        */
        p_type->type_qualifier_flags &= ~TYPE_QUALIFIER_OWNER;
    }

    type_destroy(&t);
}

struct expression* _Owner _Opt postfix_expression_tail(struct parser_ctx* ctx, struct expression* _Owner p_expression_node_param)
{

    /*
       argument cannot be null, but here p_expression_node can be null
    */
    struct expression* _Owner _Opt p_expression_node = p_expression_node_param;

    try
    {
        while (ctx->current != NULL)
        {
            if (ctx->current->type == '[')
            {
                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL) throw;

                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = POSTFIX_ARRAY;
                // the result of the subscription operator ([])

                if (!type_is_pointer_or_array(&p_expression_node->type))
                {
                    compiler_diagnostic_message(C_ERROR_SUBSCRIPTED_VALUE_IS_NEITHER_ARRAY_NOR_POINTER,
                                                ctx,
                                                ctx->current, NULL,
                                                "subscripted value is neither array nor pointer");
                }

                if (type_is_pointer(&p_expression_node->type))
                {
                    p_expression_node_new->type = type_remove_pointer(&p_expression_node->type);
                }
                else if (type_is_array(&p_expression_node->type))
                {
                    p_expression_node_new->type = get_array_item_type(&p_expression_node->type);
                }

                parser_match(ctx);
                if (ctx->current == NULL)
                {
                    expression_delete(p_expression_node_new);
                    throw;
                }

                /*contem a expresao de dentro do  [ ] */
                p_expression_node_new->right = expression(ctx);
                if (p_expression_node_new->right == NULL)
                {
                    expression_delete(p_expression_node_new);
                    throw;
                }

                if (constant_value_is_valid(&p_expression_node_new->right->constant_value))
                {
                    unsigned long long index =
                        constant_value_to_ull(&p_expression_node_new->right->constant_value);
                    if (type_is_array(&p_expression_node->type))
                    {
                        if (p_expression_node->type.num_of_elements > 0)
                        {
                            if (index >= (unsigned long long)p_expression_node->type.num_of_elements)
                            {
                                compiler_diagnostic_message(W_OUT_OF_BOUNDS,
                                                            ctx,
                                                            ctx->current, NULL,
                                                            "index %d is past the end of the array", index);
                            }
                        }
                    }
                }
                if (parser_match_tk(ctx, ']') != 0)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '(')
            {
                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL) throw;

                p_expression_node_new->first_token = p_expression_node->first_token;
                p_expression_node_new->expression_type = POSTFIX_FUNCTION_CALL;

                if (!type_is_function_or_function_pointer(&p_expression_node->type))
                {
                    compiler_diagnostic_message(C_ERROR_CALLED_OBJECT_IS_NOT_FUNCTION_OR_FUNCTION_POINTER,
                                                ctx,
                                                ctx->current,
                                                NULL,
                                                "called object is not attr function or function pointer");
                }

                p_expression_node_new->type = get_function_return_type(&p_expression_node->type);

                parser_match(ctx);
                if (ctx->current == NULL)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                if (ctx->current->type != ')')
                {
                    p_expression_node_new->argument_expression_list = argument_expression_list(ctx);
                }
                if (parser_match_tk(ctx, ')') != 0)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                compare_function_arguments(ctx, &p_expression_node->type, &p_expression_node_new->argument_expression_list);

                if (ctx->previous == NULL)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                p_expression_node_new->last_token = ctx->previous;

                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '.')
            {
                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL) throw;

                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = POSTFIX_DOT;
                p_expression_node_new->left = p_expression_node;
                p_expression_node = NULL; /*MOVED*/

                p_expression_node_new->declarator = p_expression_node_new->left->declarator;

                parser_match(ctx);
                if (ctx->current == NULL)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                if (p_expression_node_new->left->type.type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
                {
                    assert(p_expression_node_new->left->type.struct_or_union_specifier != NULL);

                    struct struct_or_union_specifier* _Opt p =
                        find_struct_or_union_specifier(ctx, p_expression_node_new->left->type.struct_or_union_specifier->tag_name);

                    if (p)
                        p = get_complete_struct_or_union_specifier(p);

                    if (p)
                    {
                        assert(ctx->current != NULL);

                        int member_index = 0;
                        struct member_declarator* _Opt p_member_declarator =
                            find_member_declarator(&p->member_declaration_list, ctx->current->lexeme, &member_index);

                        if (p_member_declarator)
                        {
                            p_expression_node_new->member_index = member_index;

                            if (p_member_declarator->declarator)
                            {
                                p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                            }
                            else
                            {
                                /*
                                struct X {
                                    int : 1;
                                };
                                */
                            }

                            if (p_member_declarator->declarator != NULL)
                            {
                                fix_member_type(&p_expression_node_new->type,
                                                &p_expression_node_new->left->type,
                                                &p_member_declarator->declarator->type);
                            }
                        }
                        else
                        {
                            compiler_diagnostic_message(C_ERROR_STRUCT_MEMBER_NOT_FOUND,
                                                    ctx,
                                                    ctx->current, NULL,
                                                    "member '%s' not found in 'struct %s'",
                                                    ctx->current->lexeme,
                                                    p->tag_name);
                        }
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_STRUCT_MEMBER_NOT_FOUND,
                                                 ctx,
                                                 ctx->current, NULL,
                                                 "incomplete struct type '%s'",
                                                 p_expression_node_new->left->type.struct_or_union_specifier->tag_name);
                        //print_scope(&ctx->scopes);
                    }
                    if (parser_match_tk(ctx, TK_IDENTIFIER) != 0)
                    {
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = NULL;
                        throw;
                    }
                }
                else
                {
                    compiler_diagnostic_message(C_ERROR_STRUCTURE_OR_UNION_REQUIRED,
                                                ctx,
                                                ctx->current, NULL,
                                                "structure or union required");
                }
                // todo apontar pro nome?
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '->')
            {
                /*
                             p_expression_node_new
                           (left)   /  \  (right)
                                   /    \
                  p_expression_node      NULL
                */

                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL) throw;

                p_expression_node->last_token = ctx->current;
                p_expression_node_new->first_token = p_expression_node->first_token;
                p_expression_node_new->last_token = ctx->current;
                p_expression_node_new->expression_type = POSTFIX_ARROW;

                // the result of a member access through pointer -> operator is lvalue

                parser_match(ctx);
                if (ctx->current == NULL)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                if (type_is_pointer_or_array(&p_expression_node->type))
                {
                    struct type item_type = { 0 };
                    if (type_is_array(&p_expression_node->type))
                    {
                        compiler_diagnostic_message(W_ARRAY_INDIRECTION, ctx, ctx->current, NULL, "using indirection '->' in array");
                        item_type = get_array_item_type(&p_expression_node->type);
                    }
                    else
                    {
                        item_type = type_remove_pointer(&p_expression_node->type);
                    }

                    if (type_is_struct_or_union(&item_type))
                    {
                        assert(p_expression_node->type.next);
                        assert(p_expression_node->type.next->struct_or_union_specifier);

                        struct struct_or_union_specifier* _Opt p_complete =
                            get_complete_struct_or_union_specifier(p_expression_node->type.next->struct_or_union_specifier);

                        if (p_complete)
                        {
                            int member_index = 0;
                            struct member_declarator* _Opt p_member_declarator =
                                find_member_declarator(&p_complete->member_declaration_list, ctx->current->lexeme, &member_index);

                            if (p_member_declarator)
                            {
                                if (p_member_declarator->declarator)
                                {
                                    p_expression_node_new->member_index = member_index;
                                    p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                                    fix_arrow_member_type(&p_expression_node_new->type, &p_expression_node->type, &p_expression_node_new->type);
                                }
                                else
                                {
                                    assert(false); //TODO
                                }
                            }
                            else
                            {
                                compiler_diagnostic_message(C_ERROR_STRUCT_MEMBER_NOT_FOUND,
                                                            ctx,
                                                            ctx->current, NULL,
                                                            "member '%s' not found in struct '%s'",
                                                            ctx->current->lexeme,
                                                            p_expression_node->type.next->struct_or_union_specifier->tag_name);
                            }
                        }
                        else
                        {
                            compiler_diagnostic_message(C_ERROR_STRUCT_IS_INCOMPLETE,
                                                        ctx,
                                                        ctx->current, NULL,
                                                        "struct '%s' is incomplete.",
                                                        ctx->current->lexeme);
                        }
                        if (parser_match_tk(ctx, TK_IDENTIFIER) != 0)
                        {
                            type_destroy(&item_type);
                            expression_delete(p_expression_node_new);
                            p_expression_node_new = NULL;
                            throw;
                        }
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_STRUCTURE_OR_UNION_REQUIRED,
                                                    ctx,
                                                    ctx->current, NULL,
                                                    "structure or union required");
                    }
                    type_destroy(&item_type);
                }
                else
                {
                    compiler_diagnostic_message(C_ERROR_STRUCTURE_OR_UNION_REQUIRED,
                                                ctx,
                                                ctx->current, NULL,
                                                "structure or union required");
                }

                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '++')
            {
                if (type_is_owner(&p_expression_node->type))
                {
                    compiler_diagnostic_message(C_ERROR_OPERATOR_INCREMENT_CANNOT_BE_USED_IN_OWNER,
                                                ctx,
                                                p_expression_node->first_token, NULL,
                                                "operator ++ cannot be used in _Owner pointers");
                }

                if (!expression_is_lvalue(p_expression_node))
                {
                    compiler_diagnostic_message(C_ERROR_OPERATOR_NEEDS_LVALUE,
                                                ctx,
                                                p_expression_node->first_token, NULL,
                                                "lvalue required as increment operand");
                }

                if (ctx->current == NULL) throw;

                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL) throw;

                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = POSTFIX_INCREMENT;

                p_expression_node_new->type = type_dup(&p_expression_node->type);
                parser_match(ctx);
                if (ctx->current == NULL)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '--')
            {
                if (type_is_owner(&p_expression_node->type))
                {
                    compiler_diagnostic_message(C_ERROR_OPERATOR_DECREMENT_CANNOT_BE_USED_IN_OWNER,
                                                ctx,
                                                p_expression_node->first_token, NULL,
                                                "operator -- cannot be used in owner pointers");
                }

                if (!expression_is_lvalue(p_expression_node))
                {
                    compiler_diagnostic_message(C_ERROR_OPERATOR_NEEDS_LVALUE,
                                                ctx,
                                                p_expression_node->first_token, NULL,
                                                "lvalue required as decrement operand");
                }

                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL) throw;

                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = POSTFIX_DECREMENT;

                p_expression_node_new->type = type_dup(&p_expression_node->type);
                parser_match(ctx);
                if (ctx->current == NULL)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else
            {
                if (ctx->current == NULL)
                    throw;

                struct token* _Opt p_last = previous_parser_token(ctx->current);
                if (p_last == NULL)
                    throw; //unexpected

                p_expression_node->last_token = p_last;
                break;
            }
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* _Owner _Opt postfix_expression_type_name(struct parser_ctx* ctx, struct type_name* _Owner p_type_name_par)
{
    /*
        ( type-name ) { initializer-ctx }
        ( type-name ) { initializer-ctx , }

        //My extension : if type-name is function then follow is compound-statement
        ( type-name ) compound-statement

    */
    struct type_name* _Owner _Opt p_type_name = p_type_name_par; //MOVED
    struct expression* _Owner _Opt p_expression_node = NULL;

    try
    {
        p_expression_node = calloc(1, sizeof * p_expression_node);
        if (p_expression_node == NULL)
            throw;

        assert(p_expression_node->type_name == NULL);

        struct token* _Opt p_previous = previous_parser_token(p_type_name->first_token);
        if (p_previous == NULL)
            throw;

        p_expression_node->first_token = p_previous;
        assert(p_expression_node->first_token->type == '(');

        p_expression_node->type_name = p_type_name; /*MOVED*/
        p_type_name = NULL; /*MOVED*/
        p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);

        if (type_is_function(&p_expression_node->type_name->declarator->type))
        {
            p_expression_node->expression_type = POSTFIX_EXPRESSION_FUNCTION_LITERAL;

            struct scope* parameters_scope =
                &p_expression_node->type_name->declarator->direct_declarator->function_declarator->parameters_scope;

            scope_list_push(&ctx->scopes, parameters_scope);
            p_expression_node->compound_statement = function_body(ctx);
            scope_list_pop(&ctx->scopes);
        }
        else
        {
            p_expression_node->expression_type = POSTFIX_EXPRESSION_COMPOUND_LITERAL;
            p_expression_node->braced_initializer = braced_initializer(ctx);
        }

        if (ctx->previous == NULL)
            throw;

        p_expression_node->last_token = ctx->previous;

        p_expression_node = postfix_expression_tail(ctx, p_expression_node);
        if (p_expression_node == NULL)
            throw;
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }
    type_name_delete(p_type_name);
    return p_expression_node;
}

struct expression* _Owner _Opt postfix_expression(struct parser_ctx* ctx)
{
    /*
      postfix-expression:
        primary-expression
        postfix-expression [ expression ]
        postfix-expression ( argument-expression-list_opt)
        postfix-expression . identifier
        postfix-expression -> identifier
        postfix-expression ++
        postfix-expression --
        ( type-name ) { initializer-ctx }
        ( type-name ) { initializer-ctx , }

        //My extension : if type-name is function then follow is compound-statement
        ( type-name ) compound-statement

        */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {


        if (first_of_type_name_ahead(ctx)) // aqui preciso ver se nao eh primary
        {
            assert(false); // este caso esta pegando lá dentro deo cast expression.
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;

            
            assert(ctx->current != NULL);
            p_expression_node->first_token = ctx->current;
            if (parser_match_tk(ctx, '(') != 0)
                throw;

            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == NULL)
                throw;

            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);

            if (parser_match_tk(ctx, ')') != 0)
                throw;
            // printf("\n");
            // print_type(&p_expression_node->type);
            bool is_function_type = type_is_function(&p_expression_node->type);

            if (is_function_type)
            {
                p_expression_node->expression_type = POSTFIX_EXPRESSION_FUNCTION_LITERAL;
                p_expression_node->compound_statement = compound_statement(ctx);
                if (p_expression_node->compound_statement == NULL)
                    throw;

                p_expression_node->last_token = p_expression_node->compound_statement->last_token;
            }
            else
            {
                p_expression_node->expression_type = POSTFIX_EXPRESSION_COMPOUND_LITERAL;
                p_expression_node->braced_initializer = braced_initializer(ctx);
                p_expression_node->last_token = ctx->current;
            }
        }
        else
        {
            p_expression_node = primary_expression(ctx);
            if (p_expression_node == NULL)
                throw;
        }

        p_expression_node = postfix_expression_tail(ctx, p_expression_node);
        if (p_expression_node == NULL)
            throw;
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }
    return p_expression_node;
}

bool is_first_of_compiler_function(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return
        // traits
        ctx->current->type == TK_KEYWORD_IS_LVALUE ||
        ctx->current->type == TK_KEYWORD_IS_OWNER ||
        ctx->current->type == TK_KEYWORD_IS_CONST ||
        ctx->current->type == TK_KEYWORD_IS_POINTER ||
        ctx->current->type == TK_KEYWORD_IS_ARRAY ||
        ctx->current->type == TK_KEYWORD_IS_FUNCTION ||

        ctx->current->type == TK_KEYWORD_ASSERT ||

        ctx->current->type == TK_KEYWORD_IS_SCALAR ||
        ctx->current->type == TK_KEYWORD_IS_ARITHMETIC ||
        ctx->current->type == TK_KEYWORD_IS_FLOATING_POINT ||
        ctx->current->type == TK_KEYWORD_IS_INTEGRAL;
}

bool is_first_of_unary_expression(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;
    return first_of_postfix_expression(ctx) ||
        ctx->current->type == '++' ||
        ctx->current->type == '--' ||
        ctx->current->type == '&' ||
        ctx->current->type == '*' ||
        ctx->current->type == '+' ||
        ctx->current->type == '-' ||
        ctx->current->type == '~' ||
        ctx->current->type == '!' ||
        ctx->current->type == TK_KEYWORD_SIZEOF ||
        ctx->current->type == TK_KEYWORD__ALIGNOF ||
        is_first_of_compiler_function(ctx);
}

static int check_sizeof_argument(struct parser_ctx* ctx,
    const struct expression* p_expression,
    const struct type* const p_type)
{
    //sizeof(type)  p_expression is the sizeof expression
    //sizeof(expression) p_expression is expression


    enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_FUNCTION)
    {
        //In GCC returns 1

        //The sizeof operator shall not be applied to an expression that has function type or an incomplete type
    }
    else if (category == TYPE_CATEGORY_ITSELF &&
            p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
    {
        assert(p_type->struct_or_union_specifier);

        struct struct_or_union_specifier* _Opt p_complete =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_complete == NULL)
        {
            //The sizeof operator shall not be applied to an expression that has function type or an incomplete type
            compiler_diagnostic_message(C_ERROR_STRUCT_IS_INCOMPLETE,
                                       ctx,
                                       p_expression->first_token,
                                       NULL,
                                       "struct is incomplete type");
            return -1;
        }
    }
    else if (category == TYPE_CATEGORY_ARRAY)
    {
        if (type_is_vla(p_type))
        {
            return 0;
        }

        if (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            //GCC
            //<source>:4:21: warning: 'sizeof' on array function parameter 'a' will return size of 'int *' [-Wsizeof-array-argument]
            //CLANG
            //<source>:4:21: warning: sizeof on array function parameter will return size of 'int *' instead of 'int[]' [-Wsizeof-array-argument]

            compiler_diagnostic_message(W_SIZEOF_ARRAY_ARGUMENT,
                                        ctx,
                                        p_expression->first_token,
                                        NULL,
                                        "sizeof applied to array function parameter");

        }
    }

    return 0; //ok
}

struct expression* _Owner _Opt unary_expression(struct parser_ctx* ctx)
{
    /*
    unary-expression:
      postfix-expression
      ++ unary-expression
      -- unary-expression

      one of (& * + - ~ !) cast-expression

      sizeof unary-expression
      sizeof ( type-name )
      _Alignof ( type-name )



      */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        if (ctx->current->type == '++' || ctx->current->type == '--')
        {
            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;

            if (ctx->current->type == '++')
                new_expression->expression_type = UNARY_EXPRESSION_INCREMENT;
            else
                new_expression->expression_type = UNARY_EXPRESSION_DECREMENT;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                expression_delete(new_expression);
                throw;
            }
            new_expression->right = unary_expression(ctx);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->type = type_dup(&new_expression->right->type);
            p_expression_node = new_expression;
        }
        else if (ctx->current != NULL &&
                 (ctx->current->type == '&' || ctx->current->type == '*' || ctx->current->type == '+' || ctx->current->type == '-' || ctx->current->type == '~' || ctx->current->type == '!'))
        {

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;

            struct token* op_position = ctx->current; // marcar posicao
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                expression_delete(new_expression);
                throw;
            }
#if 0
            //visual studio formater is adding spaces..
            if (style_has_space(ctx->current))
            {
                compiler_diagnostic_message(W_STYLE, ctx, ctx->current, "don't use spaces");
            }
#endif

            new_expression->right = cast_expression(ctx);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;
            if (op == '!')
            {
                new_expression->expression_type = UNARY_EXPRESSION_NOT;
                new_expression->constant_value = constant_value_unary_op(&new_expression->right->constant_value, op);

                // same as v == 0

                new_expression->type = type_make_int_bool_like();
            }
            else if (op == '~')
            {
                new_expression->expression_type = UNARY_EXPRESSION_BITNOT;
                new_expression->constant_value = constant_value_unary_op(&new_expression->right->constant_value, op);
                new_expression->type = type_dup(&new_expression->right->type);
                type_integer_promotion(&new_expression->type);
            }
            else if (op == '-')
            {
                new_expression->expression_type = UNARY_EXPRESSION_NEG;

                new_expression->constant_value = constant_value_unary_op(&new_expression->right->constant_value, op);

                new_expression->type = type_dup(&new_expression->right->type);
                type_integer_promotion(&new_expression->type);
            }
            else if (op == '+')
            {
                new_expression->expression_type = UNARY_EXPRESSION_PLUS;

                new_expression->constant_value = constant_value_unary_op(&new_expression->right->constant_value, op);

                new_expression->type = type_dup(&new_expression->right->type);
                type_integer_promotion(&new_expression->type);
            }
            else if (op == '*')
            {
                new_expression->expression_type = UNARY_EXPRESSION_CONTENT;
                // the result of the indirection(unary*) operator applied to a pointer to object

                if (!type_is_pointer_or_array(&new_expression->right->type))
                {
                    compiler_diagnostic_message(C_ERROR_INDIRECTION_REQUIRES_POINTER_OPERAND,
                                                ctx,
                                                op_position,
                        NULL,
                                                "indirection requires pointer operand");
                }
                if (type_is_pointer(&new_expression->right->type))
                {
                    new_expression->type = type_remove_pointer(&new_expression->right->type);
                }
                else
                {
                    compiler_diagnostic_message(W_ARRAY_INDIRECTION,
                                                ctx,
                                                op_position, NULL,
                                                "array indirection");
                    new_expression->type = get_array_item_type(&new_expression->right->type);
                }
            }
            else if (op == '&')
            {
                /*
                  The result of the unary & operator is a pointer to the object referred to by the lvalue-expression.
                  If the type of the lvalue-expression is " . . . ", the type of the result is
                  "pointer to . . ."
                */
                new_expression->expression_type = UNARY_EXPRESSION_ADDRESSOF;

                if (!expression_is_lvalue(new_expression->right))
                {
                    compiler_diagnostic_message(C_ERROR_ADDRESS_OF_REGISTER,
                                                ctx,
                                                new_expression->right->first_token,
                                                NULL,
                                                "lvalue required as unary '&' operand");
                }

                if (new_expression->right->type.storage_class_specifier_flags & STORAGE_SPECIFIER_REGISTER)
                {
                    if (new_expression->right->declarator)
                    {
                        compiler_diagnostic_message(C_ERROR_ADDRESS_OF_REGISTER,
                                                    ctx,
                                                    new_expression->right->first_token,
                            NULL,
                                                    "address of register variable 'x' requested",
                                                    new_expression->right->declarator->name_opt->lexeme);
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_ADDRESS_OF_REGISTER,
                                                    ctx,
                                                    new_expression->right->first_token,
                            NULL,
                                                    "address of register variable requested - declarator?");
                    }
                }

                new_expression->type = type_add_pointer(&new_expression->right->type, ctx->options.null_checks_enabled);
                new_expression->type.address_of = true;
            }
            else
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(C_ERROR_INVALID_TOKEN,
                                            ctx,
                                            ctx->current,
                    NULL,
                                            "invalid token");
                throw;
            }
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_SIZEOF)
        {
            const bool disable_evaluation_copy = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = true;
            // defer would be nice here...

            parser_match(ctx);
            if (ctx->current == NULL) throw;

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;

            if (first_of_type_name_ahead(ctx))
            {
                new_expression->expression_type = UNARY_EXPRESSION_SIZEOF_TYPE;
                if (parser_match_tk(ctx, '(') != 0)
                {
                    expression_delete(new_expression);
                    new_expression = NULL;
                    throw;
                }
                new_expression->type_name = type_name(ctx);
                if (new_expression->type_name == NULL)
                {
                    expression_delete(new_expression);
                    new_expression = NULL;
                    throw;
                }

                new_expression->type = make_size_t_type();

                if (parser_match_tk(ctx, ')') != 0)
                {
                    expression_delete(new_expression);
                    new_expression = NULL;
                    throw;
                }

                if (check_sizeof_argument(ctx, new_expression, &new_expression->type_name->type) != 0)
                {
                    //not fatal error
                    //fatal will be if someone need the sizeof at compile time
                    //but we don't have the constant_value set here
                }
                else
                {
                    if (type_is_vla(&new_expression->type_name->declarator->type))
                    {
                        //not constant
                    }
                    else
                    {
                        new_expression->constant_value = make_constant_value_ll(type_get_sizeof(&new_expression->type_name->declarator->type), false);
                    }
                }
            }
            else
            {
                new_expression->right = unary_expression(ctx);
                if (new_expression->right == NULL)
                {
                    /*restore*/
                    ctx->evaluation_is_disabled = disable_evaluation_copy;
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->expression_type = UNARY_EXPRESSION_SIZEOF_EXPRESSION;

                if (check_sizeof_argument(ctx, new_expression->right, &new_expression->right->type) != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }

                if (type_is_vla(&new_expression->right->type))
                {
                    //not constant
                }
                else
                {
                    new_expression->constant_value = make_constant_value_ll(type_get_sizeof(&new_expression->right->type), false);
                }
            }

            type_destroy(&new_expression->type);
            new_expression->type = type_make_size_t();
            p_expression_node = new_expression;

            /*restore*/
            ctx->evaluation_is_disabled = disable_evaluation_copy;
        }
        else if (
            ctx->current->type == TK_KEYWORD_IS_LVALUE ||
            ctx->current->type == TK_KEYWORD_IS_OWNER ||
            ctx->current->type == TK_KEYWORD_IS_CONST ||
            ctx->current->type == TK_KEYWORD_IS_POINTER ||
            ctx->current->type == TK_KEYWORD_IS_ARRAY ||
            ctx->current->type == TK_KEYWORD_IS_FUNCTION ||
            ctx->current->type == TK_KEYWORD_IS_ARITHMETIC ||
            ctx->current->type == TK_KEYWORD_IS_SCALAR ||
            ctx->current->type == TK_KEYWORD_IS_FLOATING_POINT ||
            ctx->current->type == TK_KEYWORD_IS_INTEGRAL)
        {
            const bool disable_evaluation_copy = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = true;


            struct token* traits_token = ctx->current;

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = UNARY_EXPRESSION_TRAITS;

            parser_match(ctx);
            if (ctx->current == NULL)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }

            struct type* _Opt p_type = NULL;
            if (first_of_type_name_ahead(ctx))
            {
                if (parser_match_tk(ctx, '(') != 0)
                {
                    expression_delete(new_expression);
                    new_expression = NULL;
                    throw;
                }
                new_expression->type_name = type_name(ctx);
                if (new_expression->type_name == NULL)
                {
                    expression_delete(new_expression);
                    new_expression = NULL;
                    throw;
                }

                if (ctx->current == NULL)
                {
                    expression_delete(new_expression);
                    new_expression = NULL;
                    throw;
                }

                new_expression->last_token = ctx->current;
                if (parser_match_tk(ctx, ')') != 0)
                {
                    expression_delete(new_expression);
                    new_expression = NULL;
                    throw;
                }
                p_type = &new_expression->type_name->declarator->type;
            }
            else
            {

                new_expression->right = unary_expression(ctx);
                if (new_expression->right == NULL)
                {
                    /*restore*/
                    ctx->evaluation_is_disabled = disable_evaluation_copy;
                    expression_delete(new_expression);
                    throw;
                }

                p_type = &new_expression->right->type;
                if (ctx->previous == NULL)
                {
                    expression_delete(new_expression);
                    new_expression = NULL;
                    throw;
                }

                new_expression->last_token = ctx->previous;
            }

            switch (traits_token->type)
            {
            case TK_KEYWORD_IS_LVALUE:
                new_expression->constant_value = make_constant_value_ll(expression_is_lvalue(new_expression->right), false);
                break;

            case TK_KEYWORD_IS_CONST:
                new_expression->constant_value = make_constant_value_ll(type_is_const(p_type), false);
                break;
            case TK_KEYWORD_IS_OWNER:
                new_expression->constant_value = make_constant_value_ll(type_is_owner(p_type), false);
                break;

            case TK_KEYWORD_IS_POINTER:
                new_expression->constant_value = make_constant_value_ll(type_is_pointer(p_type), false);

                break;
            case TK_KEYWORD_IS_FUNCTION:
                new_expression->constant_value = make_constant_value_ll(type_is_function(p_type), false);

                break;
            case TK_KEYWORD_IS_ARRAY:
                new_expression->constant_value = make_constant_value_ll(type_is_array(p_type), false);

                break;
            case TK_KEYWORD_IS_ARITHMETIC:
                new_expression->constant_value = make_constant_value_ll(type_is_arithmetic(p_type), false);

                break;
            case TK_KEYWORD_IS_SCALAR:
                new_expression->constant_value = make_constant_value_ll(type_is_scalar(p_type), false);

                break;
            case TK_KEYWORD_IS_FLOATING_POINT:
                new_expression->constant_value = make_constant_value_ll(type_is_floating_point(p_type), false);

                break;
            case TK_KEYWORD_IS_INTEGRAL:
                new_expression->constant_value = make_constant_value_ll(type_is_integer(p_type), false);

                break;

            default:
                assert(false);
            }

            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
            /*restore*/
            ctx->evaluation_is_disabled = disable_evaluation_copy;
        }
        else if (ctx->current->type == TK_KEYWORD_ASSERT)
        {
            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->expression_type = UNARY_EXPRESSION_ASSERT;
            new_expression->first_token = ctx->current;

            parser_match(ctx);

            if (ctx->current == NULL || parser_match_tk(ctx, '(') != 0)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }
            new_expression->right = expression(ctx);

            /*if (constant_value_is_valid(&new_expression->right->constant_value) &&
                !constant_value_to_bool(&new_expression->right->constant_value))
            {
                compiler_diagnostic_message(C_ERROR_STATIC_ASSERT_FAILED, ctx,
                    new_expression->right->first_token, "assert failed");
            }*/

            if (parser_match_tk(ctx, ')') != 0)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }
            return new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD__ALIGNOF)
        {
            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->expression_type = UNARY_EXPRESSION_ALIGNOF;
            new_expression->first_token = ctx->current;
            parser_match(ctx);

            if (ctx->current == NULL || parser_match_tk(ctx, '(') != 0)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }
            new_expression->type_name = type_name(ctx);
            if (parser_match_tk(ctx, ')') != 0)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }

            if (!ctx->evaluation_is_disabled)
            {
                new_expression->constant_value = make_constant_value_ll(type_get_alignof(&new_expression->type_name->type), ctx->evaluation_is_disabled);
            }
            new_expression->type = type_make_int();

            assert(ctx->previous != NULL);
            new_expression->last_token = ctx->previous;

            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD__ALIGNAS)
        {
        }
        else // if (is_first_of_primary_expression(ctx))
        {
            p_expression_node = postfix_expression(ctx);
            if (p_expression_node == NULL)
                throw;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

struct expression* _Owner _Opt cast_expression(struct parser_ctx* ctx)
{
    /*
     cast-expression:
      unary-expression
      ( type-name ) cast-expression


      ( type-name ) //<- extension void value
    */

    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        if (first_of_type_name_ahead(ctx))
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;


            assert(ctx->current != NULL); //by first_of_type_name_ahead

            p_expression_node->first_token = ctx->current;
            p_expression_node->expression_type = CAST_EXPRESSION;
            if (parser_match_tk(ctx, '(') != 0)
                throw;

            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == NULL)
            {
                expression_delete(p_expression_node);
                p_expression_node = NULL;
                throw;
            }

            p_expression_node->type = type_dup(&p_expression_node->type_name->type);

            // type_set_int(&ctx->result_type);
            // print_type_name(p_cast_expression->type_name);
            if (parser_match_tk(ctx, ')') != 0)
                throw;
            // struct token_list r = copy_replacement_list(&l);
            // pop_f
            if (ctx->current->type == '{')
            {
                // Thinking it was a cast expression was a mistake... 
                // because the { appeared then it is a compound literal which is a postfix.
                struct expression* _Owner _Opt new_expression = postfix_expression_type_name(ctx, p_expression_node->type_name);
                if (new_expression == NULL) throw;

                p_expression_node->type_name = NULL; // MOVED

                expression_delete(p_expression_node);
                p_expression_node = new_expression;
            }
            else if (is_first_of_unary_expression(ctx))
            {
                p_expression_node->left = cast_expression(ctx);
                if (p_expression_node->left == NULL)
                {
                    expression_delete(p_expression_node);
                    p_expression_node = NULL;
                    throw;
                }

                if (p_expression_node->left->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN &&
                    type_is_owner(&p_expression_node->left->type))
                {
                    if (!type_is_owner(&p_expression_node->type))
                    {
                        if (type_is_pointer(&p_expression_node->left->type))
                        {
                            //(int*) malloc(1)
                            compiler_diagnostic_message(W_OWNERSHIP_DISCARDING_OWNER, ctx, p_expression_node->first_token, NULL, "discarding _Owner pointer");
                        }
                        else
                        {
                            compiler_diagnostic_message(W_OWNERSHIP_DISCARDING_OWNER, ctx, p_expression_node->first_token, NULL, "discarding _Owner");
                        }
                    }
                }

                p_expression_node->constant_value = p_expression_node->left->constant_value;

                type_destroy(&p_expression_node->type);
                p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
                if (type_is_floating_point(&p_expression_node->type))
                {
                    p_expression_node->constant_value =
                        constant_value_cast(&p_expression_node->constant_value, TYPE_DOUBLE);
                }
                else if (type_is_integer(&p_expression_node->type))
                {
                    if (type_is_unsigned_integer(&p_expression_node->type))
                    {
                        p_expression_node->constant_value =
                            constant_value_cast(&p_expression_node->constant_value, TYPE_UNSIGNED_LONG_LONG);
                    }
                    else
                    {
                        p_expression_node->constant_value =
                            constant_value_cast(&p_expression_node->constant_value, TYPE_LONG_LONG);
                    }
                }

                p_expression_node->type.storage_class_specifier_flags =
                    p_expression_node->left->type.storage_class_specifier_flags;
            }
            else
            {
                compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "expected expression");
            }
        }
        else if (is_first_of_unary_expression(ctx))
        {

            p_expression_node = unary_expression(ctx);
            if (p_expression_node == NULL)
            {
                throw;
            }
        }
        else
        {
            compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "expected expression");
            assert(p_expression_node == NULL);
            throw;
        }

        if (ctx->current == NULL || ctx->previous == NULL)
            throw;

        p_expression_node->last_token = ctx->previous;
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

struct expression* _Owner _Opt multiplicative_expression(struct parser_ctx* ctx)
{

    /*
     multiplicative-expression:
    cast-expression
    multiplicative-expression * cast-expression
    multiplicative-expression / cast-expression
    multiplicative-expression % cast-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        p_expression_node = cast_expression(ctx);
        if (p_expression_node == NULL)
        {
            throw;
        }

        while (ctx->current != NULL &&
               (ctx->current->type == '*' ||
                   ctx->current->type == '/' ||
                   ctx->current->type == '%'))
        {
            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
            {
                expression_delete(p_expression_node);
                p_expression_node = NULL;
                throw;
            }
            
            new_expression->first_token = ctx->current;
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->left = p_expression_node;
            p_expression_node = NULL; // MOVED

            new_expression->right = cast_expression(ctx);

            if (new_expression->left == NULL ||
                new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;

            if (op == '*')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MULT;

                if (!type_is_arithmetic(&new_expression->left->type))
                {
                    compiler_diagnostic_message(C_ERROR_LEFT_IS_NOT_ARITHMETIC, ctx, ctx->current, NULL, "left * is not arithmetic");
                }
                if (!type_is_arithmetic(&new_expression->right->type))
                {
                    compiler_diagnostic_message(C_ERROR_RIGHT_IS_NOT_ARITHMETIC, ctx, ctx->current, NULL, "right * is not arithmetic");
                }
            }
            else if (op == '/')
            {

                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_DIV;

                new_expression->constant_value =
                    constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '/');

                if (ctx->evaluation_is_disabled)
                {
                    //sizeof(1/0)
                }
                else
                {
                    if (constant_value_is_valid(&new_expression->right->constant_value) &&
                        constant_value_to_ll(&new_expression->right->constant_value) == 0)
                    {
                        compiler_diagnostic_message(W_DIVIZION_BY_ZERO, ctx, ctx->current, NULL, "division by zero");
                    }
                }

                if (!type_is_arithmetic(&new_expression->left->type))
                {
                    compiler_diagnostic_message(C_ERROR_LEFT_IS_NOT_ARITHMETIC, ctx, ctx->current, NULL, "left / is not arithmetic");
                }
                if (!type_is_arithmetic(&new_expression->right->type))
                {
                    compiler_diagnostic_message(C_ERROR_RIGHT_IS_NOT_ARITHMETIC, ctx, ctx->current, NULL, "right / is not arithmetic");
                }
            }
            else if (op == '%')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MOD;

                if (!type_is_integer(&new_expression->left->type))
                {
                    compiler_diagnostic_message(C_ERROR_LEFT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "left is not integer");
                }
                if (!type_is_integer(&new_expression->right->type))
                {
                    compiler_diagnostic_message(C_ERROR_RIGHT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "right is not integer");
                }

                if (constant_value_is_valid(&new_expression->right->constant_value) &&
                    constant_value_to_ll(&new_expression->right->constant_value) == 0)
                {
                    compiler_diagnostic_message(W_DIVIZION_BY_ZERO, ctx, ctx->current, NULL, "divizion by zero");
                }
            }

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, op);

            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid type multiplicative expression");
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }
    return p_expression_node;
}

struct expression* _Owner _Opt additive_expression(struct parser_ctx* ctx)
{
    /*
     additive-expression:
    multiplicative-expression
    additive-expression + multiplicative-expression
    additive-expression - multiplicative-expression
    */

    struct expression* _Owner _Opt p_expression_node = NULL;

    try
    {
        p_expression_node = multiplicative_expression(ctx);
        if (p_expression_node == NULL)
        {
            throw;
        }

        while (ctx->current != NULL &&
               (ctx->current->type == '+' ||
                   ctx->current->type == '-'))
        {
            struct token* operator_position = ctx->current;

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
            {
                compiler_diagnostic_message(C_ERROR_OUT_OF_MEM, ctx, ctx->current, NULL, "_Out of mem");
                throw;
            }

            new_expression->first_token = ctx->current;

            enum token_type op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }

            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/


            new_expression->right = multiplicative_expression(ctx);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;

            if (!type_is_scalar(&new_expression->left->type))
            {
                compiler_diagnostic_message(C_ERROR_LEFT_IS_NOT_SCALAR, ctx, operator_position, NULL, "left operator is not scalar");
            }
            if (!type_is_scalar(&new_expression->right->type))
            {
                compiler_diagnostic_message(C_ERROR_RIGHT_IS_NOT_SCALAR, ctx, operator_position, NULL, "right operator is not scalar");
            }

            const bool b_left_is_arithmetic = type_is_arithmetic(&new_expression->left->type);
            const bool b_right_is_arithmetic = type_is_arithmetic(&new_expression->right->type);

            const enum type_category left_category = type_get_category(&new_expression->left->type);
            const enum type_category right_category = type_get_category(&new_expression->right->type);

            if (op == '+')
            {
                new_expression->expression_type = ADDITIVE_EXPRESSION_PLUS;

                /*
                 For addition, either both operands shall have arithmetic type,
                 or one operand shall be a pointer to a complete object type and
                 the other shall have integer type. (Incrementing is equivalent to adding 1.)
                */
                if (b_left_is_arithmetic && b_right_is_arithmetic)
                {
                    int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
                    if (code != 0)
                    {
                        expression_delete(new_expression);
                        new_expression = NULL;
                        compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "internal error");
                        throw;
                    }
                }
                else
                {
                    if (left_category == TYPE_CATEGORY_POINTER || left_category == TYPE_CATEGORY_ARRAY /* || left_category == TYPE_CATEGORY_FUNCTION*/)
                    {
                        if (type_is_integer(&new_expression->right->type))
                        {
                            if (left_category == TYPE_CATEGORY_ARRAY)
                            {
                                struct type t = get_array_item_type(&new_expression->left->type);
                                new_expression->type = type_add_pointer(&t, ctx->options.null_checks_enabled);
                                //new_expression->type.type_qualifier_flags |= not_nu;
                                type_destroy(&t);
                            }
                            else
                            {
                                new_expression->type = type_dup(&new_expression->left->type);
                            }
                        }
                        else
                        {
                            compiler_diagnostic_message(C_ERROR_RIGHT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "expected integer type on right");
                        }
                    }
                    else if (right_category == TYPE_CATEGORY_POINTER || right_category == TYPE_CATEGORY_ARRAY /*|| right_category == TYPE_CATEGORY_FUNCTION*/)
                    {
                        if (type_is_integer(&new_expression->left->type))
                        {
                            if (right_category == TYPE_CATEGORY_ARRAY)
                            {
                                new_expression->type = get_array_item_type(&new_expression->right->type);
                            }
                            else
                            {
                                new_expression->type = type_dup(&new_expression->right->type);
                            }
                        }
                        else
                        {
                            compiler_diagnostic_message(C_ERROR_LEFT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "expected integer type on left");
                        }
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid types additive expression");
                    }
                }
            }
            else if (op == '-')
            {
                new_expression->expression_type = ADDITIVE_EXPRESSION_MINUS;

                /*
                 For subtraction, one of the following shall hold:
                    — both operands have arithmetic type;
                    — both operands are pointers to qualified or unqualified versions of compatible complete object
                    types; or
                    — the left operand is a pointer to a complete object type and the right operand has integer type.
                    (Decrementing is equivalent to subtracting 1.)
                */
                if (b_left_is_arithmetic && b_right_is_arithmetic)
                {
                    // — both operands have arithmetic type;
                    int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
                    if (code != 0)
                    {
                        expression_delete(new_expression);
                        new_expression = NULL;

                        compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "internal error type_common");
                        throw;
                    }
                }
                else
                {
                    if (left_category == TYPE_CATEGORY_POINTER || left_category == TYPE_CATEGORY_ARRAY)
                    {
                        if (right_category == TYPE_CATEGORY_POINTER || right_category == TYPE_CATEGORY_ARRAY)
                        {
                            // — both operands are pointers to qualified or unqualified versions of compatible complete object
                            //  types;
                            struct type t1 = type_lvalue_conversion(&new_expression->left->type, ctx->options.null_checks_enabled);
                            struct type t2 = type_lvalue_conversion(&new_expression->right->type, ctx->options.null_checks_enabled);

                            if (!type_is_same(&t1, &t2, false))
                            {
                                compiler_diagnostic_message(C_ERROR_INCOMPATIBLE_POINTER_TYPES, ctx, ctx->current, NULL, "incompatible pointer types");
                            }

                            new_expression->type = type_make_int();
                            type_destroy(&t1);
                            type_destroy(&t2);
                        }
                        else
                        {
                            if (type_is_integer(&new_expression->right->type))
                            {
                                //- the left operand is a pointer to a complete object typeand the right operand has integer type.
                                new_expression->type = type_dup(&new_expression->left->type);
                            }
                            else
                            {
                                compiler_diagnostic_message(C_ERROR_RIGHT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "right must be integer type");
                            }
                        }
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid types for operator -");
                    }
                }
            }

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, op);

            p_expression_node = new_expression;
            new_expression = NULL; /*MOVED*/
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

struct expression* _Owner _Opt shift_expression(struct parser_ctx* ctx)
{
    /*
     shift-expression:
       additive-expression
       shift-expression << additive-expression
       shift-expression >> additive-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        p_expression_node = additive_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '>>' ||
                   ctx->current->type == '<<'))
        {
            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;
            

            enum token_type op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }

            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = multiplicative_expression(ctx);
            if (new_expression->left == NULL || new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;

            if (op == '>>')
            {
                new_expression->expression_type = SHIFT_EXPRESSION_RIGHT;
            }
            else if (op == '<<')
            {
                new_expression->expression_type = SHIFT_EXPRESSION_LEFT;
            }

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, op);

            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid type shift expression");
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

struct expression* _Owner _Opt relational_expression(struct parser_ctx* ctx)
{
    /*
    relational-expression:
        shift-expression
        relational-expression < shift-expression
        relational-expression > shift-expression
        relational-expression <= shift-expression
        relational-expression >= shift-expression
    */

    struct expression* _Owner _Opt p_expression_node = NULL;
    struct expression* _Owner _Opt new_expression = NULL;
    try
    {
        p_expression_node = shift_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '>' ||
                   ctx->current->type == '<' ||
                   ctx->current->type == '>=' ||
                   ctx->current->type == '<='))
        {
            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
            {
                throw;
            }
            new_expression->first_token = ctx->current;
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = shift_expression(ctx);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;
            if (ctx->current == NULL)
            {
                throw;
            }

            check_comparison(ctx,
              new_expression->left,
              new_expression->right,
              ctx->current);

            if (op == '>')
            {
                new_expression->expression_type = RELATIONAL_EXPRESSION_BIGGER_THAN;
            }
            else if (op == '<')
            {
                new_expression->expression_type = RELATIONAL_EXPRESSION_LESS_THAN;
            }
            else if (op == '>=')
            {
                new_expression->expression_type = RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN;
            }
            else if (op == '<=')
            {
                new_expression->expression_type = RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN;
            }

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, op);

            new_expression->type = type_make_int_bool_like();

            p_expression_node = new_expression;
            new_expression = NULL; /*MOVED*/
        }
    }
    catch
    {
        expression_delete(new_expression);
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

void check_diferent_enuns(struct parser_ctx* ctx,
                                 const struct token* operator_token,
                                 const struct expression* left,
                                 const struct expression* right,
                                 const char* message)
{
    if (left->type.type_specifier_flags & TYPE_SPECIFIER_ENUM &&
        right->type.type_specifier_flags & TYPE_SPECIFIER_ENUM)
    {
        if (get_complete_enum_specifier(left->type.enum_specifier) !=
            get_complete_enum_specifier(right->type.enum_specifier))
        {
            assert(left->type.enum_specifier != NULL);
            assert(right->type.enum_specifier != NULL);

            const char* lefttag = "";
            if (left->type.enum_specifier->tag_token)
                lefttag = left->type.enum_specifier->tag_token->lexeme;

            const char* righttag = "";
            if (right->type.enum_specifier->tag_token)
                righttag = right->type.enum_specifier->tag_token->lexeme;

            char finalmessage[200] = { 0 };
            snprintf(finalmessage,
                sizeof finalmessage,
                "%s (enum %s, enum %s)",
                message,
                lefttag,
                righttag);

            compiler_diagnostic_message(W_ENUN_CONVERSION,
                                        ctx,
                                        operator_token, NULL,
                                        finalmessage,
                                        lefttag,
                                        righttag);
        }
    }
}
void expression_evaluate_equal_not_equal(const struct expression* left,
                                         const struct expression* right,
                                         struct expression* result,
                                         int op,
                                         bool disabled)
{
    assert(op == '==' || op == '!=');
    result->constant_value =
        constant_value_op(&left->constant_value, &right->constant_value, op);
}

struct expression* _Owner _Opt equality_expression(struct parser_ctx* ctx)
{
    /*
      equality-expression:
       relational-expression
       equality-expression == relational-expression
       equality-expression != relational-expression
    */
    /*
    * Equality operators
    One of the following shall hold:
    — both operands have arithmetic type;
    — both operands are pointers to qualified or unqualified versions of compatible types;
    — one operand is a pointer to an object type and the other is a pointer to a qualified or unqualified
    version of void; or
    — one operand is a pointer and the other is a null pointer constant.
    */
    struct expression* _Owner _Opt  p_expression_node = NULL;
    struct expression* _Owner _Opt new_expression = NULL;

    try
    {
        p_expression_node = relational_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '==' ||
                   ctx->current->type == '!='))
        {
            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            
            struct token* operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = relational_expression(ctx);
            if (new_expression->right == NULL)
                throw;

            if (ctx->current == NULL)
            {
                throw;
            }

            check_comparison(ctx,
              new_expression->left,
              new_expression->right,
              ctx->current);

            new_expression->last_token = new_expression->right->last_token;


            new_expression->first_token = operator_token;

            if (operator_token->type == '==')
            {
                new_expression->expression_type = EQUALITY_EXPRESSION_EQUAL;
                expression_evaluate_equal_not_equal(new_expression->left,
                                                    new_expression->right,
                                                    new_expression,
                                                    '==',
                                                    ctx->evaluation_is_disabled);
            }
            else if (operator_token->type == '!=')
            {
                new_expression->expression_type = EQUALITY_EXPRESSION_NOT_EQUAL;
                expression_evaluate_equal_not_equal(new_expression->left,
                                                    new_expression->right,
                                                    new_expression,
                                                    '!=',
                                                    ctx->evaluation_is_disabled);
            }
            else
            {
                assert(false);
            }

            type_destroy(&new_expression->type);
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
            new_expression = NULL; /*MOVED*/
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    expression_delete(new_expression);
    return p_expression_node;
}

struct expression* _Owner _Opt and_expression(struct parser_ctx* ctx)
{
    /*
     AND-expression:
      equality-expression
      AND-expression & equality-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    struct expression* _Owner _Opt new_expression = NULL;

    try
    {
        p_expression_node = equality_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL && ctx->current->type == '&')
        {
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = AND_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = equality_expression(ctx);
            if (new_expression->right == NULL)
                throw;

            new_expression->last_token = new_expression->right->last_token;
            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '&');

            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid types and expression");
                throw;
            }

            p_expression_node = new_expression;
            new_expression = NULL; /*MOVED*/
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    expression_delete(new_expression);
    return p_expression_node;
}

struct expression* _Owner _Opt  exclusive_or_expression(struct parser_ctx* ctx)
{
    /*
     exclusive-OR-expression:
      AND-expression
      exclusive-OR-expression ^ AND-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    struct expression* _Owner _Opt new_expression = NULL;

    try
    {
        p_expression_node = and_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '^'))
        {
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = EXCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = and_expression(ctx);
            if (new_expression->right == NULL)
                throw;

            new_expression->last_token = new_expression->right->last_token;
            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '^');

            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid types or expression");
                throw;
            }

            p_expression_node = new_expression;
            new_expression = NULL;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    expression_delete(new_expression);
    return p_expression_node;
}

struct expression* _Owner _Opt inclusive_or_expression(struct parser_ctx* ctx)
{
    /*
    inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        p_expression_node = exclusive_or_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '|'))
        {
            struct token* operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            
            new_expression->first_token = ctx->current;
            new_expression->expression_type = INCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = exclusive_or_expression(ctx);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            check_diferent_enuns(ctx,
                                operator_token,
                                new_expression->left,
                                new_expression->right,
                                "operator '|' between enumerations of different types.");

            new_expression->last_token = new_expression->right->last_token;
            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '|');

            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid types inclusive or expression");
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

struct expression* _Owner _Opt logical_and_expression(struct parser_ctx* ctx)
{
    /*
    logical-AND-expression:
     inclusive-OR-expression
     logical-AND-expression && inclusive-OR-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        p_expression_node = inclusive_or_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '&&'))
        {
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = LOGICAL_AND_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = inclusive_or_expression(ctx);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }
            new_expression->last_token = new_expression->right->last_token;
            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '&&');

            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {

                type_print(&new_expression->left->type);
                type_print(&new_expression->right->type);
                compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid types logical and expression");
                expression_delete(new_expression);
                throw;
            }
            p_expression_node = new_expression;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

struct expression* _Owner _Opt logical_or_expression(struct parser_ctx* ctx)
{
    /*
      logical-OR-expression:
       logical-AND-expression
       logical-OR-expression || logical-AND-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        p_expression_node = logical_and_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '||'))
        {
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = LOGICAL_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = logical_and_expression(ctx);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;
            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '||');

            if (!type_is_scalar(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(C_ERROR_LEFT_IS_NOT_SCALAR, ctx, ctx->current, NULL, "left type is not scalar for or expression");
                throw;
            }

            if (!type_is_scalar(&new_expression->right->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(C_ERROR_RIGHT_IS_NOT_SCALAR, ctx, ctx->current, NULL, "right type is not scalar for or expression");
                throw;
            }

            new_expression->type = type_make_int_bool_like();

            p_expression_node = new_expression;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

struct expression* _Owner _Opt assignment_expression(struct parser_ctx* ctx)
{
    /*
    assignment-expression:
       conditional-expression
       unary-expression assignment-operator assignment-expression
       */
       /*
          assignment-operator: one of
          = *= /= %= += -= <<= >>= &= ^= |=
       */
       // aqui eh duvidoso mas conditional faz a unary tb.
       // a diferenca q nao eh qualquer expressao
       // que pode ser de atribuicao
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        p_expression_node = conditional_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '=' ||
                   ctx->current->type == '*=' ||
                   ctx->current->type == '/=' ||
                   ctx->current->type == '%=' ||
                   ctx->current->type == '+=' ||
                   ctx->current->type == '-=' ||
                   ctx->current->type == '<<=' ||
                   ctx->current->type == '>>=' ||
                   ctx->current->type == '&=' ||
                   ctx->current->type == '^=' ||
                   ctx->current->type == '|='))
        {

            const struct token* const op_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == NULL) throw;

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = ASSIGNMENT_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; // MOVED

            assert(new_expression->left != NULL);

            const struct marker left_operand_marker = {
                      .p_token_begin = new_expression->left->first_token,
                      .p_token_end = new_expression->left->last_token,
            };

            if (type_is_function(&new_expression->left->type))
            {
                compiler_diagnostic_message(C_ERROR_ASSIGNMENT_OF_FUNCTION,
                    ctx,
                    NULL,
                    &left_operand_marker,
                    "assignment of function");
            }
            else if (type_is_array(&new_expression->left->type))
            {
                if (new_expression->left->type.storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
                {
                    /*
                      assignment of array parameter
                    */
                    compiler_diagnostic_message(W_ASSIGNMENT_OF_ARRAY_PARAMETER,
                        ctx,
                        NULL,
                        &left_operand_marker,
                        "assignment to array parameter");
                }
                else
                {
                    compiler_diagnostic_message(C_ERROR_ASSIGNMENT_TO_EXPRESSION_WITH_ARRAY_TYPE,
                        ctx,
                        NULL,
                        &left_operand_marker,
                        "assignment to expression with array type");
                }
            }

            if (type_is_const(&new_expression->left->type))
            {
                compiler_diagnostic_message(C_ERROR_ASSIGNMENT_OF_READ_ONLY_OBJECT,
                    ctx,
                    NULL,
                    &left_operand_marker,
                    "assignment of read-only object");
            }


            if (!expression_is_lvalue(new_expression->left))
            {
                compiler_diagnostic_message(C_ERROR_OPERATOR_NEEDS_LVALUE,
                                            ctx,
                                            NULL,
                                            &left_operand_marker,
                                            "lvalue required as left operand of assignment");
            }

            new_expression->right = assignment_expression(ctx);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            if (op_token->type == '=')
            {
                check_assigment(ctx, &new_expression->left->type, new_expression->right, ASSIGMENT_TYPE_OBJECTS);
            }

            new_expression->last_token = new_expression->right->last_token;

            new_expression->type = type_dup(&new_expression->left->type);

            new_expression->type.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_FUNCTION_RETURN;
            new_expression->type.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_FUNCTION_RETURN_NODISCARD;

            check_diferent_enuns(ctx,
                op_token,
                new_expression->left,
                new_expression->right,
                "assignment of different enums.");

            new_expression->left->is_assignment_expression = true;
            if (new_expression->left->left)
                new_expression->left->left->is_assignment_expression = true;
            p_expression_node = new_expression;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

void argument_expression_list_push(struct argument_expression_list* list, struct argument_expression* _Owner pitem)
{
    if (list->head == NULL)
    {
        list->head = pitem;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = pitem;
    }
    list->tail = pitem;
}

void argument_expression_list_destroy(struct argument_expression_list* _Obj_owner p)
{
    struct argument_expression* _Owner _Opt item = p->head;
    while (item)
    {
        struct argument_expression* _Owner _Opt next = item->next;
        item->next = NULL;
        argument_expression_delete(item);
        item = next;
    }
}

bool expression_is_malloc(const struct expression* p)
{
    if (p->expression_type == POSTFIX_FUNCTION_CALL &&
        p->left &&
        p->left->declarator &&
        p->left->declarator->name_opt)
    {
        if (strcmp(p->left->declarator->name_opt->lexeme, "malloc") == 0)
            return true;
    }
    return false;
}

bool expression_is_calloc(const struct expression* p)
{
    if (p->expression_type == POSTFIX_FUNCTION_CALL &&
        p->left &&
        p->left->declarator &&
        p->left->declarator->name_opt)
    {
        if (strcmp(p->left->declarator->name_opt->lexeme, "calloc") == 0)
            return true;
    }
    return false;
}

void expression_delete(struct expression* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->condition_expr);
        compound_statement_delete(p->compound_statement);
        type_name_delete(p->type_name);

        expression_delete(p->right);
        expression_delete(p->left);
        braced_initializer_delete(p->braced_initializer);
        generic_selection_delete(p->generic_selection);
        type_destroy(&p->type);
        argument_expression_list_destroy(&p->argument_expression_list);
        free(p);
    }
}
struct expression* _Owner _Opt expression(struct parser_ctx* ctx)
{
    /*expression:
      assignment-expression
      expression, assignment-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        p_expression_node = assignment_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        if (ctx->current->type == ',')
        {
            while (ctx->current->type == ',')
            {
                parser_match(ctx);
                if (ctx->current == NULL) throw;

                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL)
                    throw;


                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = ASSIGNMENT_EXPRESSION;
                p_expression_node_new->left = p_expression_node;
                p_expression_node = NULL; /*MOVED*/

                p_expression_node_new->right = expression(ctx);
                if (p_expression_node_new->right == NULL)
                {
                    expression_delete(p_expression_node_new);
                    throw;
                }
                p_expression_node_new->left->last_token = p_expression_node_new->right->last_token;

                p_expression_node = p_expression_node_new;
            }

            if (p_expression_node->right == NULL)
            {
                throw;
            }

            /*same type of the last expression*/
            type_destroy(&p_expression_node->type);
            p_expression_node->type = type_dup(&p_expression_node->right->type);
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }
    return p_expression_node;
}

bool is_first_of_conditional_expression(struct parser_ctx* ctx)
{
    return is_first_of_unary_expression(ctx) ||
        is_first_of_primary_expression(ctx);
}

bool expression_is_one(const struct expression* expression)
{
    if (expression->expression_type == PRIMARY_EXPRESSION_NUMBER)
    {
        return (constant_value_is_valid(&expression->constant_value) &&
            constant_value_to_ull(&expression->constant_value) == 1);
    }
    return false;
}

bool expression_is_zero(const struct expression* expression)
{
    if (expression->expression_type == PRIMARY_EXPRESSION_NUMBER)
    {
        return (constant_value_is_valid(&expression->constant_value) &&
            constant_value_to_ull(&expression->constant_value) == 0);
    }
    return false;
}

bool expression_is_null_pointer_constant(const struct expression* expression)
{

    if (type_is_nullptr_t(&expression->type) ||
        (constant_value_is_valid(&expression->constant_value) &&
            constant_value_to_ull(&expression->constant_value) == 0))
    {
        return true;
    }

    return false;
}

struct expression* _Owner _Opt conditional_expression(struct parser_ctx* ctx)
{
    /*
      conditional-expression:
      logical-OR-expression
      logical-OR-expression ? expression : conditional-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    struct type left_type = { 0 };
    struct type right_type = { 0 };
    try
    {
        p_expression_node = logical_or_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        if (ctx->current && ctx->current->type == '?')
        {
            struct expression* _Owner _Opt p_conditional_expression = calloc(1, sizeof * p_conditional_expression);
            if (p_conditional_expression == NULL) throw;


            p_conditional_expression->first_token = ctx->current;
            p_conditional_expression->expression_type = CONDITIONAL_EXPRESSION;
            p_conditional_expression->condition_expr = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            parser_match(ctx); //?
            if (ctx->current == NULL)
            {
                expression_delete(p_conditional_expression);
                throw;
            }

            struct expression* _Owner _Opt p_left = expression(ctx);
            if (p_left == NULL)
            {
                expression_delete(p_conditional_expression);
                throw;
            }
            p_conditional_expression->left = p_left;

            parser_match(ctx); //:
            if (ctx->current == NULL)
            {
                expression_delete(p_conditional_expression);
                throw;
            }

            struct expression* _Owner _Opt p_right = conditional_expression(ctx);
            if (p_right == NULL)
            {
                expression_delete(p_conditional_expression);
                throw;
            }
            p_conditional_expression->right = p_right;

            if (constant_value_is_valid(&p_conditional_expression->condition_expr->constant_value))
            {
                if (constant_value_to_bool(&p_conditional_expression->condition_expr->constant_value))
                {
                    /*this is an extensions.. in constant expression we can mix types!*/
                    p_conditional_expression->type = type_dup(&p_conditional_expression->left->type);
                    p_conditional_expression->constant_value = p_conditional_expression->left->constant_value;
                }
                else
                {
                    /*this is an extensions.. in constant expression we can mix types!*/
                    p_conditional_expression->type = type_dup(&p_conditional_expression->right->type);
                    p_conditional_expression->constant_value = p_conditional_expression->right->constant_value;
                }
            }

            if (expression_is_subjected_to_lvalue_conversion(p_conditional_expression->left))
            {
                left_type = type_lvalue_conversion(&p_conditional_expression->left->type, ctx->options.null_checks_enabled);
            }
            else
            {
                left_type = type_dup(&p_conditional_expression->left->type);
            }

            if (expression_is_subjected_to_lvalue_conversion(p_conditional_expression->right))
            {
                right_type = type_lvalue_conversion(&p_conditional_expression->right->type, ctx->options.null_checks_enabled);
            }
            else
            {
                right_type = type_dup(&p_conditional_expression->right->type);
            }

            /*The first operand shall have scalar type*/
            if (!type_is_scalar(&p_conditional_expression->condition_expr->type))
            {
                compiler_diagnostic_message(C_ERROR_CONDITION_MUST_HAVE_SCALAR_TYPE, ctx, ctx->current, NULL, "condition must have scalar type");
            }
            else if (type_is_arithmetic(&left_type) &&
                     type_is_arithmetic(&right_type))
            {
                /*
                 *  both operands have arithmetic type;
                 */
                type_common(&left_type, &right_type, &p_conditional_expression->type);
            }
            else if (type_is_struct_or_union(&left_type) && type_is_struct_or_union(&right_type))
            {
                /*
                 *  both operands have compatible structure or union type;
                 */
                if (!type_is_same(&left_type, &right_type, true))
                {
                    compiler_diagnostic_message(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types");
                }
                type_swap(&p_conditional_expression->type, &right_type);
            }
            else if (type_is_void(&left_type) && type_is_void(&right_type))
            {
                /*
                 *  both operands have void type;
                 */
                type_swap(&p_conditional_expression->type, &right_type);
            }
            else if (type_is_nullptr_t(&left_type) && type_is_nullptr_t(&right_type))
            {
                /* both operands have nullptr_t type;*/
                type_swap(&p_conditional_expression->type, &right_type);
            }
            else if (type_is_pointer(&left_type))
            {
                /*
                — one operand is a pointer and the other is a null pointer constant or has type nullptr_t; or
                    — one operand is a pointer to an object type and the other is a pointer to a qualified or unqualified
                    version of void
            */
                if (expression_is_null_pointer_constant(p_conditional_expression->right) ||
                    type_is_nullptr_t(&right_type) ||
                    type_is_void_ptr(&right_type))
                {
                    type_swap(&p_conditional_expression->type, &left_type);
                }
                else if (type_is_pointer(&right_type))
                {
                    if (type_is_nullptr_t(&left_type) || type_is_void_ptr(&left_type))
                    {
                        type_swap(&p_conditional_expression->type, &left_type);
                    }
                    else if (!type_is_same(&left_type, &right_type, false))
                    {
                        // type_print(&left_type);
                        // type_print(&right_type);
                        compiler_diagnostic_message(C_ERROR_INCOMPATIBLE_TYPES, ctx, ctx->current, NULL, "incompatible types");
                    }
                    else
                    {
                        type_swap(&p_conditional_expression->type, &right_type);
                    }
                }
                else
                {
                    compiler_diagnostic_message(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types");
                }
            }
            else if (type_is_pointer(&right_type))
            {
                if (expression_is_null_pointer_constant(p_conditional_expression->left) ||
                    type_is_nullptr_t(&left_type) ||
                    type_is_void_ptr(&left_type))
                {
                    type_swap(&p_conditional_expression->type, &right_type);
                }
                else if (type_is_pointer(&left_type))
                {
                    if (type_is_nullptr_t(&left_type) || type_is_void_ptr(&left_type))
                    {
                        type_swap(&p_conditional_expression->type, &right_type);
                    }
                    else if (!type_is_same(&left_type, &right_type, false))
                    {
                        compiler_diagnostic_message(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types");
                    }
                    else
                    {
                        type_swap(&p_conditional_expression->type, &right_type);
                    }
                }
                else
                {
                    compiler_diagnostic_message(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types");
                }
            }
            else
            {
                compiler_diagnostic_message(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types??");
                assert(false);
            }
            p_expression_node = p_conditional_expression;
        }
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    type_destroy(&left_type);
    type_destroy(&right_type);

    return p_expression_node;
}

struct expression* _Owner _Opt constant_expression(struct parser_ctx* ctx, bool show_error_if_not_constant)
{
    struct expression* _Owner _Opt p_expression = conditional_expression(ctx);

    if (show_error_if_not_constant &&
        p_expression &&
        !constant_value_is_valid(&p_expression->constant_value))
    {
        compiler_diagnostic_message(C_ERROR_EXPECTED_CONSTANT_EXPRESSION, ctx, ctx->current, NULL, "expected constant expression");
    }

    return p_expression;
}

bool expression_is_lvalue(const struct expression* expr)
{
    // https://en.cppreference.com/w/c/language/value_category

    switch (expr->expression_type)
    {
    case PRIMARY_EXPRESSION_DECLARATOR:
    case PRIMARY_EXPRESSION__FUNC__:
    case PRIMARY_EXPRESSION_STRING_LITERAL:
    case POSTFIX_ARRAY:
    case POSTFIX_ARROW:
    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:
    case UNARY_EXPRESSION_CONTENT:
        return true;
    default:
        break;
    }

    if (expr->expression_type == PRIMARY_EXPRESSION_PARENTESIS &&
        expr->right)
    {
        return expression_is_lvalue(expr->right);
    }
    else if (expr->expression_type == POSTFIX_DOT &&
             expr->left)
    {
        return expression_is_lvalue(expr->left);
    }

    return false;
}


/*
 * Returns true if the type of expression is subjected to type_lvalue_conversion
 */
bool expression_is_subjected_to_lvalue_conversion(const struct expression* expression)
{

    switch (expression->expression_type)
    {
    case UNARY_EXPRESSION_ADDRESSOF:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:
    case POSTFIX_INCREMENT:
    case POSTFIX_DECREMENT:
        return false;
    default:
        if (expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
            return true;
    }

    return true;
}

void check_comparison(struct parser_ctx* ctx,
    struct expression* p_a_expression,
    struct expression* p_b_expression,
    const struct token* op_token)
{
    //TODO more checks unsigned < 0

    struct type* p_a_type = &p_a_expression->type;
    struct type* p_b_type = &p_b_expression->type;

    if (type_is_pointer(p_a_type) && type_is_integer(p_b_type))
    {
        if (expression_is_zero(p_b_expression))
        {
            // p == 0
            //style warning
        }
        else
        {
            //array functions..
            compiler_diagnostic_message(W_ENUN_CONVERSION,
                                        ctx,
                                        op_token, NULL,
                                        "comparison between pointer and integer");
        }
    }

    check_diferent_enuns(ctx,
                         op_token,
                         p_a_expression,
                         p_b_expression,
                         "comparing different enums.");
}

void check_assigment(struct parser_ctx* ctx,
    const struct type* p_a_type, /*this is not expression because function parameters*/
    const struct expression* p_b_expression,
    enum assigment_type assignment_type /*ASSIGMENT_TYPE_RETURN, ASSIGMENT_TYPE_PARAMETER, ASSIGMENT_TYPE_OBJECTS*/)
{
    const struct type* const p_b_type = &p_b_expression->type;

    const bool is_null_pointer_constant = expression_is_null_pointer_constant(p_b_expression);

    if (type_is_pointer(p_a_type))
    {
        if (!type_is_nullptr_t(p_b_type) &&
            !type_is_pointer_or_array(p_b_type) &&
            !type_is_function(p_b_type))
        {
            if (is_null_pointer_constant)
            {
                if (p_b_expression->expression_type == PRIMARY_EXPRESSION_NUMBER)
                {
                    // This is the only exception.
                    // p = 0;
                    compiler_diagnostic_message(W_STYLE, ctx, p_b_expression->first_token, NULL, "use NULL instead of 0");
                }
                else
                {
                    //Everything else is unusual
                    // p = false;
                    // p = 1-1;
                    // p = '\0';
                    compiler_diagnostic_message(W_UNSUAL_NULL_POINTER_CONSTANT, ctx, p_b_expression->first_token, NULL, "unusual expression/type used as null pointer constant");
                }
            }
            else
            {
                compiler_diagnostic_message(C_ERROR_INT_TO_POINTER, ctx, p_b_expression->first_token, NULL, "non-pointer to pointer");
            }
        }
    }

    if (type_is_bool(p_a_type) && type_is_nullptr_t(p_b_type))
    {
        struct marker marker = {
        .p_token_begin = p_b_expression->first_token,
        .p_token_end = p_b_expression->first_token
        };

        compiler_diagnostic_message(W_NULL_CONVERTION,
        ctx,
        NULL,
        &marker,
        "implicit conversion of nullptr constant to 'bool'");
    }

    struct type lvalue_right_type = { 0 };
    struct type t2 = { 0 };

    if (expression_is_subjected_to_lvalue_conversion(p_b_expression))
    {
        lvalue_right_type = type_lvalue_conversion(p_b_type, ctx->options.null_checks_enabled);
    }
    else
    {
        lvalue_right_type = type_dup(p_b_type);
    }


    if (type_is_owner(p_a_type) && !type_is_owner(&p_b_expression->type))
    {
        if (!is_null_pointer_constant)
        {
            compiler_diagnostic_message(W_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN, ctx, p_b_expression->first_token, NULL, "cannot assign a non-owner to owner");
            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }
    }

    if (!type_is_owner(p_a_type) && type_is_any_owner(&p_b_expression->type))
    {
        if (p_b_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
        {
            compiler_diagnostic_message(W_OWNERSHIP_USING_TEMPORARY_OWNER,
                ctx,
                p_b_expression->first_token, NULL,
                "cannot assign a temporary _Owner to no-_Owner object.");
            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }
    }

    if (assignment_type == ASSIGMENT_TYPE_RETURN)
    {
        if (!type_is_owner(p_a_type) && type_is_any_owner(&p_b_expression->type))
        {
            if (p_b_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_AUTOMATIC_STORAGE)
            {
                compiler_diagnostic_message(C_ERROR_RETURN_LOCAL_OWNER_TO_NON_OWNER,
                    ctx,
                    p_b_expression->first_token, NULL,
                    "cannot return a automatic storage duration _Owner to non-owner");
                type_destroy(&lvalue_right_type);
                type_destroy(&t2);
                return;
            }
        }
    }

    if (type_is_obj_owner(p_a_type) && type_is_pointer(p_a_type))
    {
        if (type_is_owner(p_b_type))
        {
        }
        else if (!p_b_type->address_of)
        {
            compiler_diagnostic_message(W_MUST_USE_ADDRESSOF,
                       ctx,
                       p_b_expression->first_token, NULL,
                       "source expression of _Obj_owner must be addressof");
        }
    }


    if (type_is_pointer(p_a_type) &&
        !type_is_nullable(p_a_type, ctx->options.null_checks_enabled) &&
        is_null_pointer_constant)
    {

        compiler_diagnostic_message(W_FLOW_NULLABLE_TO_NON_NULLABLE,
            ctx,
            p_b_expression->first_token, NULL,
            "cannot convert a null pointer constant to non-nullable pointer");

        type_destroy(&lvalue_right_type);
        type_destroy(&t2);

        return;

    }



    /*
       less generic tests are first
    */
    if (type_is_enum(p_b_type) && type_is_enum(p_a_type))
    {
        if (!type_is_same(p_b_type, p_a_type, false))
        {
            compiler_diagnostic_message(W_INCOMPATIBLE_ENUN_TYPES, ctx,
                p_b_expression->first_token, NULL,
                " incompatible types ");
        }


        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }

    if (type_is_arithmetic(p_b_type) && type_is_arithmetic(p_a_type))
    {

        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }

    if (is_null_pointer_constant && type_is_pointer(p_a_type))
    {
        //TODO void F(int * [[_Opt]] p)
        // F(0) when passing null we will check if the parameter
        //have the anotation [[_Opt]]

        /*can be converted to any type*/

        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }

    if (is_null_pointer_constant && type_is_array(p_a_type))
    {
        compiler_diagnostic_message(W_FLOW_NON_NULL,
            ctx,
            p_b_expression->first_token, NULL,
            " passing null as array");


        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }

    /*
       We have two pointers or pointer/array combination
    */
    if (type_is_pointer_or_array(p_b_type) && type_is_pointer_or_array(p_a_type))
    {
        if (type_is_void_ptr(p_b_type))
        {
            /*void pointer can be converted to any type*/

            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }

        if (type_is_void_ptr(p_a_type))
        {
            /*any pointer can be converted to void* */

            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }


        //TODO  lvalue

        if (type_is_array(p_a_type))
        {
            int parameter_array_size = p_a_type->num_of_elements;
            if (type_is_array(p_b_type))
            {
                int argument_array_size = p_b_type->num_of_elements;
                if (parameter_array_size != 0 &&
                    argument_array_size < parameter_array_size)
                {
                    compiler_diagnostic_message(C_ERROR_ARGUMENT_SIZE_SMALLER_THAN_PARAMETER_SIZE, ctx,
                        p_b_expression->first_token, NULL,
                        " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                }
            }
            else if (is_null_pointer_constant || type_is_nullptr_t(p_b_type))
            {
                compiler_diagnostic_message(W_PASSING_NULL_AS_ARRAY, ctx,
                    p_b_expression->first_token, NULL,
                    " passing null as array");
            }
            t2 = type_lvalue_conversion(p_a_type, ctx->options.null_checks_enabled);
        }
        else
        {
            t2 = type_dup(p_a_type);
        }



        if (!type_is_same(&lvalue_right_type, &t2, false))
        {
            type_print(&lvalue_right_type);
            type_print(&t2);

            compiler_diagnostic_message(C_ERROR_INCOMPATIBLE_TYPES, ctx,
                p_b_expression->first_token, NULL,
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
                compiler_diagnostic_message(W_DISCARDED_QUALIFIERS, ctx,
                    p_b_expression->first_token, NULL,
                    " discarding const at argument ");
            }
            type_destroy(&argument_pointer_to);
            type_destroy(&parameter_pointer_to);
        }
        //return true;
    }

    if (!type_is_same(p_a_type, &lvalue_right_type, false))
    {
        //TODO more rules..but it is good to check worst case!
        //
        //  compiler_diagnostic_message(C1, ctx,
        //      right->first_token,
        //      " incompatible types ");
    }





    type_destroy(&lvalue_right_type);
    type_destroy(&t2);

}
