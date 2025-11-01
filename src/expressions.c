/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/


#pragma safety enable

#include "ownership.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "expressions.h"
#include <string.h>
#include "parser.h"
#include "type.h"
#include <math.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif


struct expression* _Owner _Opt postfix_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt cast_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt multiplicative_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt unary_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt additive_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt shift_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt relational_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt equality_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt and_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt exclusive_or_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt inclusive_or_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt logical_and_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt logical_or_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt conditional_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);
struct expression* _Owner _Opt conditional_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode);


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

        if (p_current_parameter_type && type_is_void(&p_current_parameter_type->type))
        {
            //(void) function
            p_current_parameter_type = NULL;
        }

        while (p_current_argument && p_current_parameter_type)
        {
            check_assigment(ctx, &p_current_parameter_type->type, p_current_argument->expression, ASSIGMENT_TYPE_PARAMETER);
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }

        if (p_current_argument != NULL && !p_param_list->is_var_args)
        {
            compiler_diagnostic(C_ERROR_TOO_MANY_ARGUMENTS, ctx,
                                        p_current_argument->expression->first_token, NULL,
                                        "too many arguments");
            throw;
        }

        if (p_current_parameter_type != NULL && !p_param_list->is_void)
        {
            if (p_argument_expression_list->tail)
            {
                compiler_diagnostic(C_ERROR_TOO_FEW_ARGUMENTS, ctx,
                                            p_argument_expression_list->tail->expression->first_token,
                    NULL,
                                            "too few arguments");
            }
            else
            {
                compiler_diagnostic(C_ERROR_TOO_FEW_ARGUMENTS, ctx, ctx->current, NULL, "too few arguments");
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
        return false;

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
    if (ctx->current == NULL)
        return false;

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

    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_COMPILER_DECIMAL_CONSTANT ||
        ctx->current->type == TK_COMPILER_OCTAL_CONSTANT ||
        ctx->current->type == TK_COMPILER_HEXADECIMAL_CONSTANT ||
        ctx->current->type == TK_COMPILER_BINARY_CONSTANT;
}

bool is_predefined_constant(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

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
    if (ctx->current == NULL)
        return false;

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

struct generic_association* _Owner _Opt generic_association(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
{
    struct generic_association* _Owner _Opt p_generic_association = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }
        }
        else if (first_of_type_name(ctx))
        {
            bool old = ctx->inside_generic_association;
            ctx->inside_generic_association = true;


            p_generic_association->p_type_name = type_name(ctx);
            if (p_generic_association->p_type_name == NULL) throw;

            assert(p_generic_association->p_type_name->abstract_declarator != NULL);

            ctx->inside_generic_association = old;
            p_generic_association->type = make_type_using_declarator(ctx, p_generic_association->p_type_name->abstract_declarator);
        }
        else
        {
            compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "unexpected");
        }
        if (parser_match_tk(ctx, ':') != 0)
            throw;

        struct expression* _Owner _Opt p_expression_temp = assignment_expression(ctx, eval_mode);
        if (p_expression_temp == NULL)
        {
            throw;
        }

        p_generic_association->expression = p_expression_temp;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
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

struct generic_assoc_list generic_association_list(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
{
    struct generic_assoc_list list = { 0 };
    try
    {
        struct generic_association* _Opt p_default_generic_association = NULL;

        struct generic_association* _Owner _Opt p_generic_association =
            generic_association(ctx, EXPRESSION_EVAL_MODE_TYPE);

        if (p_generic_association == NULL)
            throw;

        if (p_generic_association->first_token->type == TK_KEYWORD_DEFAULT)
        {
            p_default_generic_association = p_generic_association;
        }

        generic_assoc_list_add(&list, p_generic_association);

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        while (ctx->current->type == ',')
        {
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct generic_association* _Owner _Opt p_generic_association2 = generic_association(ctx, eval_mode);
            if (p_generic_association2 == NULL)
                throw;

            if (p_generic_association2->first_token->type == TK_KEYWORD_DEFAULT)
            {
                if (p_default_generic_association != NULL)
                {
                    compiler_diagnostic(C_ERROR_DUPLICATE_DEFAULT_GENERIC_ASSOCIATION,
                        ctx,
                        p_generic_association2->first_token,
                        NULL,
                        "duplicate default generic association.");

                    compiler_diagnostic(W_NOTE,
                        ctx,
                        p_default_generic_association->first_token,
                        NULL,
                        "previous default generic association");
                }
                else
                {
                    p_default_generic_association = p_generic_association2;
                }
            }

            generic_assoc_list_add(&list, p_generic_association2);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }
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

void generic_assoc_list_destroy(_Dtor struct generic_assoc_list* p)
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



struct generic_selection* _Owner _Opt generic_selection(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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
            p_generic_selection->expression = assignment_expression(ctx, eval_mode);
            if (p_generic_selection->expression == NULL)
                throw;
        }

        if (parser_match_tk(ctx, ',') != 0)
            throw;

        p_generic_selection->generic_assoc_list = generic_association_list(ctx, eval_mode);
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
            p_type = &p_generic_selection->type_name->abstract_declarator->type;
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

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }
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

struct expression* _Owner _Opt character_constant_expression(struct parser_ctx* ctx)
{
    struct expression* _Owner _Opt p_expression_node = NULL;

    const unsigned long long
        wchar_max_value = target_unsigned_max(ctx->options.target, get_platform(ctx->options.target)->wchar_t_type);

    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_expression_node = calloc(1, sizeof * p_expression_node);
        if (p_expression_node == NULL)
            throw;

        p_expression_node->expression_type = PRIMARY_EXPRESSION_CHAR_LITERAL;
        p_expression_node->first_token = ctx->current;
        p_expression_node->last_token = p_expression_node->first_token;
        p_expression_node->type.attributes_flags |= CAKE_HIDDEN_ATTRIBUTE_INT_LIKE_CHAR;
        p_expression_node->type.category = TYPE_CATEGORY_ITSELF;

        const unsigned char* _Opt p = (const unsigned char*)ctx->current->lexeme;

        if (p[0] == 'u' && p[1] == '8')
        {
            p++;
            p++;
            p++;

            // A UTF-8 character constant has type char8_t.
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR;

            unsigned int c = 0;
            p = utf8_decode(p, &c);
            if (p == NULL)
            {
                throw;
            }

            if (c == '\\')
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == NULL) throw;
            }

            if (*p != '\'')
            {
                compiler_diagnostic(C_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Unicode character literals may not contain multiple characters.");
            }

            if (c > 0x80)
            {
                compiler_diagnostic(C_CHARACTER_NOT_ENCODABLE_IN_A_SINGLE_CODE_UNIT, ctx, ctx->current, NULL, "character not encodable in a single code unit.");
            }

            p_expression_node->object = object_make_unsigned_char(ctx->options.target, (unsigned char)c);//, ctx->evaluation_is_disabled);
        }
        else if (p[0] == 'u')
        {
            p++;
            p++;

            // A UTF-16 character constant has type char16_t which is an unsigned integer types defined in the <uchar.h> header
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_SHORT;

            unsigned int c = 0;
            p = utf8_decode(p, &c);
            if (p == NULL)
            {
                throw;
            }

            if (c == '\\')
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == NULL) throw;
            }

            if (*p != '\'')
            {
                compiler_diagnostic(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Unicode character literals may not contain multiple characters.");
            }

            if (c > wchar_max_value)
            {
                compiler_diagnostic(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Character too large for enclosing character literal type.");
            }

            p_expression_node->object = object_make_wchar_t(ctx->options.target, c);
        }
        else if (p[0] == 'U')
        {
            p++;
            p++;

            // A UTF-16 character constant has type char16_t which is an unsigned integer types defined in the <uchar.h> header
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT;

            unsigned int c = 0;
            p = utf8_decode(p, &c);
            if (p == NULL)
            {
                throw;
            }

            if (c == '\\')
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == NULL) throw;
            }

            if (*p != '\'')
            {
                compiler_diagnostic(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Unicode character literals may not contain multiple characters.");
            }

            if (c > UINT_MAX)
            {
                compiler_diagnostic(W_MULTICHAR_ERROR, ctx, ctx->current, NULL, "Character too large for enclosing character literal type.");
            }

            p_expression_node->object = object_make_wchar_t(ctx->options.target, c);
        }
        else if (p[0] == 'L')
        {
            // A wchar_t character constant is prefixed by the letter L
            p++;
            p++;

            p_expression_node->type.type_specifier_flags =
                object_type_to_type_specifier(get_platform(ctx->options.target)->wchar_t_type);

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
                unsigned int c = 0;
                p = utf8_decode(p, &c);
                if (p == NULL)
                {
                    throw;
                }

                if (c == '\\')
                {
                    p = escape_sequences_decode_opt(p, &c);
                    if (p == NULL) throw;
                }

                if (c < 0x80)
                {
                    value = value * 256 + c;
                }
                else
                {
                    //decoded
                    value = c;
                }

                if (value > (long long)wchar_max_value)
                {
                    compiler_diagnostic(W_OUT_OF_BOUNDS, ctx, ctx->current, NULL, "character constant too long for its type", ctx->current->lexeme);
                    break;
                }
            }

            p_expression_node->object = object_make_wchar_t(ctx->options.target, (unsigned int)value);
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
                unsigned int c = 0;
                p = utf8_decode(p, &c);
                if (p == NULL)
                {
                    throw;
                }

                if (c == '\\')
                {
                    p = escape_sequences_decode_opt(p, &c);
                    if (p == NULL) throw;
                }

                value = value * 256 + c;
                if (value > INT_MAX)
                {
                    compiler_diagnostic(W_OUT_OF_BOUNDS, ctx, ctx->current, NULL, "character constant too long for its type", ctx->current->lexeme);
                    break;
                }
            }
            p_expression_node->object = object_make_signed_int(ctx->options.target, value);
        }

        parser_match(ctx);
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        // warning: character constant too long for its type
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }
    return p_expression_node;
}

int convert_to_number(struct parser_ctx* ctx, struct expression* p_expression_node, bool disabled, enum target target)
{
    const unsigned long long unsigned_int_max_value =
        target_unsigned_max(ctx->options.target, TYPE_UNSIGNED_INT);

    const unsigned long long signed_int_max_value =
        target_signed_max(ctx->options.target, TYPE_SIGNED_INT);

    const unsigned long long signed_long_max_value =
        target_signed_max(ctx->options.target, TYPE_SIGNED_LONG);

    const unsigned long long unsigned_long_max_value =
        target_unsigned_max(ctx->options.target, TYPE_UNSIGNED_LONG);

    const unsigned long long signed_long_long_max_value =
        target_signed_max(ctx->options.target, TYPE_SIGNED_LONG_LONG);

    const unsigned long long unsigned_long_long_max_value =
        target_unsigned_max(ctx->options.target, TYPE_UNSIGNED_LONG_LONG);


    if (ctx->current == NULL)
    {
        unexpected_end_of_file(ctx);
        return 1;
    }

    struct token* token = ctx->current;

    /*copy removing separators*/
    // one of the largest buffers needed would be 128 bits binary... 
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

    char errormsg[100] = { 0 };
    char suffix[4] = { 0 };
    enum token_type r = parse_number(buffer, suffix, errormsg);
    if (r == TK_NONE)
    {
        compiler_diagnostic(
            C_INVALID_TOKEN,
            ctx,
            token,
            NULL,
            errormsg);
        return 0;
    }

    switch (token->type)
    {
    case TK_COMPILER_DECIMAL_CONSTANT:
    case TK_COMPILER_OCTAL_CONSTANT:
    case TK_COMPILER_HEXADECIMAL_CONSTANT:
    case TK_COMPILER_BINARY_CONSTANT:
    {
        unsigned long long value = 0;
        switch (token->type)
        {
        case TK_COMPILER_DECIMAL_CONSTANT:
            value = strtoull(buffer, NULL, 10);
            break;
        case TK_COMPILER_OCTAL_CONSTANT:
            if (buffer[1] == 'o' || buffer[1] == 'O')
            {
                //C2Y
                //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3319.htm
                value = strtoull(buffer + 2, NULL, 8);
            }
            else
            {
                value = strtoull(buffer + 1, NULL, 8);
            }
            break;
        case TK_COMPILER_HEXADECIMAL_CONSTANT:
            value = strtoull(buffer + 2, NULL, 16);
            break;
        case TK_COMPILER_BINARY_CONSTANT:
            value = strtoull(buffer + 2, NULL, 2);
            break;
        default:
            break;
        }

        if (value == unsigned_long_long_max_value && errno == ERANGE)
        {
            compiler_diagnostic(
            C_ERROR_LITERAL_OVERFLOW,
            ctx,
            token,
            NULL,
            "integer literal is too large to be represented in any integer type");
        }

        if (suffix[0] == 'U')
        {
            /*fixing the type that fits the size*/
            if (value <= unsigned_int_max_value && suffix[1] != 'L')
            {
                object_destroy(&p_expression_node->object);
                p_expression_node->object = object_make_unsigned_int(ctx->options.target, value);
                p_expression_node->type.type_specifier_flags = (TYPE_SPECIFIER_INT | TYPE_SPECIFIER_UNSIGNED);
            }
            else if (value <= unsigned_long_max_value && suffix[2] != 'L')
            {
                object_destroy(&p_expression_node->object);
                p_expression_node->object = object_make_unsigned_long(target, value);
                p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_UNSIGNED;
            }
            else //if (value <= ULLONG_MAX)
            {
                object_destroy(&p_expression_node->object);
                p_expression_node->object = object_make_unsigned_long_long(ctx->options.target, value);
                p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_LONG_LONG | TYPE_SPECIFIER_UNSIGNED;
            }
        }
        else
        {
            static_assert(NUMBER_OF_TARGETS == 6, "does your target follow the C rules? (MSVC is different)");

            /*fixing the type that fits the size*/
            if (value <= signed_int_max_value && suffix[0] != 'L')
            {
                object_destroy(&p_expression_node->object);
                p_expression_node->object = object_make_signed_int(ctx->options.target, value);
                p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_INT;
            }
            else if (value <= signed_int_max_value && suffix[1] != 'L')
            {
                object_destroy(&p_expression_node->object);
                p_expression_node->object = object_make_signed_long(target, value);
                p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_LONG;
            }
            else if ((target == TARGET_X86_MSVC || target == TARGET_X64_MSVC) &&
                      (value <= unsigned_long_max_value) &&
                      suffix[1] != 'L' /*!= LL*/)
            {
                // ONLY MSVC, NON STANDARD,  uses unsigned long instead of next big signed int
                object_destroy(&p_expression_node->object);
                p_expression_node->object = object_make_unsigned_long(target, value);
                p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG;
            }
            else if (value <= signed_long_max_value && suffix[1] != 'L' /*!= LL*/)
            {
                object_destroy(&p_expression_node->object);
                p_expression_node->object = object_make_signed_long(target, value);
                p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_LONG;
            }
            else if (value <= signed_long_long_max_value)
            {
                object_destroy(&p_expression_node->object);
                p_expression_node->object = object_make_signed_long_long(ctx->options.target, value);
                p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_LONG_LONG;
            }
            else
            {
                compiler_diagnostic(
                    W_IMPLICITLY_UNSIGNED_LITERAL,
                    ctx,
                    token,
                    NULL,
                    "integer literal is too large to be represented in a signed integer type, interpreting as unsigned");

                object_destroy(&p_expression_node->object);
                p_expression_node->object = object_make_unsigned_long_long(ctx->options.target, value);
                p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_LONG_LONG | TYPE_SPECIFIER_UNSIGNED;
            }
        }

    }
    break;

    case TK_COMPILER_DECIMAL_FLOATING_CONSTANT:
    case TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT:
    {
        if (suffix[0] == 'F')
        {
            double value = strtod(buffer, NULL);
            if (isinf(value) && errno == ERANGE)
            {
            }
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_FLOAT;
            object_destroy(&p_expression_node->object);
            p_expression_node->object = object_make_float(ctx->options.target, value);
        }
        else if (suffix[0] == 'L')
        {
            long double value = strtod(buffer, NULL);
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_DOUBLE | TYPE_SPECIFIER_LONG;
            object_destroy(&p_expression_node->object);
            p_expression_node->object = object_make_long_double(ctx->options.target, value);
        }
        else
        {
            double value = strtod(buffer, NULL);
            if (isinf(value) && errno == ERANGE)
            {
            }
            object_destroy(&p_expression_node->object);
            p_expression_node->object = object_make_double(ctx->options.target, value);
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_DOUBLE;
        }
    }
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

struct expression* _Owner _Opt primary_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
    {
        unexpected_end_of_file(ctx);
        return NULL;
    }

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

            struct scope* _Opt p_scope = NULL;
            struct map_entry* _Opt p_entry = find_variables(ctx, ctx->current->lexeme, &p_scope);

            if (p_entry && p_entry->type == TAG_TYPE_ENUMERATOR)
            {
                assert(p_entry->data.p_enumerator != NULL);
                struct enumerator* p_enumerator = p_entry->data.p_enumerator;
                p_expression_node->expression_type = PRIMARY_EXPRESSION_ENUMERATOR;
                p_expression_node->object = p_enumerator->value;

                p_expression_node->type = type_make_enumerator(p_enumerator->enum_specifier);
            }
            else if (p_entry &&
                     (p_entry->type == TAG_TYPE_DECLARATOR || p_entry->type == TAG_TYPE_INIT_DECLARATOR))
            {
                struct declarator* _Opt p_declarator = NULL;
                struct init_declarator* _Opt p_init_declarator = NULL;
                if (p_entry->type == TAG_TYPE_INIT_DECLARATOR)
                {
                    assert(p_entry->data.p_init_declarator != NULL);
                    p_init_declarator = p_entry->data.p_init_declarator;
                    p_declarator = p_init_declarator->p_declarator;
                }
                else
                {
                    p_declarator = p_entry->data.p_declarator;
                }

                assert(p_declarator != NULL);

                if (type_is_deprecated(&p_declarator->type))
                {
                    compiler_diagnostic(W_DEPRECATED, ctx, ctx->current, NULL, "'%s' is deprecated", ctx->current->lexeme);
                }


                if (p_scope->scope_level == 0)
                {
                    //file scope
                }
                else if ((p_declarator->type.storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC) ||
                        (p_declarator->type.storage_class_specifier_flags & STORAGE_SPECIFIER_THREAD_LOCAL))
                {
                    //file scope or thread
                }
                else if (ctx->p_current_function_scope_opt)
                {
                    bool b_type_is_function = type_is_function(&p_declarator->type);
                    if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE && !b_type_is_function)
                    {
                        bool inside_current_function_scope = false;
                        while (p_scope)
                        {
                            if (ctx->p_current_function_scope_opt == p_scope)
                            {
                                inside_current_function_scope = true;
                                break;
                            }
                            p_scope = p_scope->previous;
                        }
                        if (!inside_current_function_scope)
                        {
                            compiler_diagnostic(C_ERROR_OUTER_SCOPE,
                                ctx,
                                ctx->current,
                                NULL,
                                "'%s' cannot be evaluated in this scope", ctx->current->lexeme);
                        }
                    }
                }

                p_declarator->num_uses++;
                p_expression_node->declarator = p_declarator;
                p_expression_node->p_init_declarator = p_init_declarator;

                p_expression_node->expression_type = PRIMARY_EXPRESSION_DECLARATOR;

                p_expression_node->type = type_dup(&p_declarator->type);
                p_expression_node->object = object_make_reference(&p_declarator->object);

            }
            else if (ctx->p_current_function_opt &&
                     strcmp(ctx->current->lexeme, "__func__") == 0)
            {

                const char* func_name = ctx->p_current_function_opt->name_opt ?
                    ctx->p_current_function_opt->name_opt->lexeme :
                    "unnamed";

                p_expression_node->expression_type = PRIMARY_EXPRESSION__FUNC__;
                p_expression_node->first_token = ctx->current;
                p_expression_node->last_token = ctx->current;

                p_expression_node->type = type_make_literal_string((int)strlen(func_name) + 1, TYPE_SPECIFIER_CHAR, TYPE_QUALIFIER_CONST, ctx->options.target);
            }
            else
            {
                compiler_diagnostic(C_ERROR_NOT_FOUND, ctx, ctx->current, NULL, "not found '%s'", ctx->current->lexeme);
                throw;
            }
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }
        }
        else if (ctx->current->type == TK_STRING_LITERAL)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_STRING_LITERAL;
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            enum type_specifier_flags char_type_specifiers = TYPE_SPECIFIER_CHAR;

            bool is_bigger_than_char = false;
            bool is_wide = false;
            bool is_u8 = false;
            bool is_u32 = false;
            bool is_u16 = false;


            if (ctx->current->lexeme[0] == 'L')
            {
                is_wide = true;
                is_bigger_than_char = true;
                char_type_specifiers =
                    object_type_to_type_specifier(get_platform(ctx->options.target)->wchar_t_type);
            }
            else if (ctx->current->lexeme[0] == 'u' &&
                     ctx->current->lexeme[1] == '8')
            {
                is_u8 = true;
                char_type_specifiers = TYPE_SPECIFIER_CHAR;
            }
            else if (ctx->current->lexeme[0] == 'u')
            {
                is_u16 = true;
                is_bigger_than_char = true;
                char_type_specifiers = TYPE_SPECIFIER_UNSIGNED | object_type_to_type_specifier(get_platform(ctx->options.target)->int16_type);
            }
            else if (ctx->current->lexeme[0] == 'U')
            {
                is_u32 = true;
                is_bigger_than_char = true;
                char_type_specifiers = TYPE_SPECIFIER_UNSIGNED | object_type_to_type_specifier(get_platform(ctx->options.target)->int32_type);
            }
            else
            {
                char_type_specifiers = TYPE_SPECIFIER_CHAR;
            }
            /*
              string concatenation should have been done in a previous phase
              but since we keep the source format here it was an alternative
            */

            unsigned int number_of_elements_including_zero = 0;
            struct object* _Opt last = NULL;

            while (ctx->current->type == TK_STRING_LITERAL)
            {
                //"part1" "part2" TODO check different types


                const unsigned char* it = (unsigned char*)ctx->current->lexeme;

                //skip string literal prefix u8, L etc 
                while (*it != '"')
                    it++;

                assert(*it == '"');
                it++; //skip "

                unsigned int value = 0;
                while (it && *it != '"')
                {
                    unsigned int c = 0;

                    if (is_bigger_than_char)
                    {
                        it = utf8_decode(it, &c);
                        if (it == NULL)
                        {
                            throw;
                        }
                    }
                    else
                    {
                        c = *it;
                        it++;
                    }

                    if (c == '\\')
                    {
                        it = escape_sequences_decode_opt(it, &value);
                    }
                    else
                    {
                        value = c;
                    }

                    struct object* _Opt _Owner p_new = calloc(1, sizeof * p_new);
                    if (p_new == NULL) throw;
                    if (is_wide)
                    {
                        *p_new = object_make_wchar_t(ctx->options.target, value);
                    }
                    else if (is_u8)
                    {
                        //C11 u8 is sigend, C23 it is unsigned
                        *p_new = object_make_uint8(ctx->options.target, (uint8_t)value);
                    }
                    else if (is_u16)
                    {
                        *p_new = object_make_uint16(ctx->options.target, (uint16_t)value);
                    }
                    else if (is_u32)
                    {
                        *p_new = object_make_uint32(ctx->options.target, (uint32_t)value);
                    }
                    else
                    {
                        //u8"" also are char not (char8_t)
                        *p_new = object_make_char(ctx->options.target, value);
                    }
                    number_of_elements_including_zero++;
                    if (p_expression_node->object.members.head == NULL)
                    {
                        p_expression_node->object.members.head = p_new;
                    }
                    else
                    {
                        if (last)
                            last->next = p_new;
                    }
                    last = p_new;
                }

                parser_match(ctx);
                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }
            }

            /*
              Appending the last \0
            */
            struct object* _Opt _Owner p_new = calloc(1, sizeof * p_new);
            if (p_new == NULL) throw;

            if (is_wide)
            {
                *p_new = object_make_wchar_t(ctx->options.target, 0);
            }
            else if (is_u8)
            {
                //C11 u8 is sigend, C23 it is unsigned
                *p_new = object_make_uint8(ctx->options.target, (uint8_t)0);
            }
            else if (is_u16)
            {
                *p_new = object_make_uint16(ctx->options.target, 0);
            }
            else if (is_u32)
            {
                *p_new = object_make_uint32(ctx->options.target, 0);
            }
            else
            {
                //u8"" also are char not (char8_t)
                *p_new = object_make_char(ctx->options.target, 0);
            }
            number_of_elements_including_zero++;

            if (last == NULL)
            {
                p_expression_node->object.members.head = p_new;
            }
            else
            {
                last->next = p_new;
            }

            enum type_qualifier_flags lit_flags = ctx->options.const_literal ? TYPE_QUALIFIER_CONST : TYPE_QUALIFIER_NONE;
            p_expression_node->type = type_make_literal_string(number_of_elements_including_zero, char_type_specifiers, lit_flags, ctx->options.target);
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

            p_expression_node->object = object_make_bool(ctx->options.target, ctx->current->type == TK_KEYWORD_TRUE);

            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_BOOL;
            p_expression_node->type.type_qualifier_flags = 0;

            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }
        }
        else if (ctx->current->type == TK_KEYWORD_NULLPTR)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_PREDEFINED_CONSTANT;
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            p_expression_node->object = object_make_nullptr(ctx->options.target);

            /*TODO nullptr type*/
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_NULLPTR_T;
            p_expression_node->type.type_qualifier_flags = 0;

            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }
        }
        else if (is_integer_or_floating_constant(ctx->current->type))
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;

            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;
            p_expression_node->expression_type = PRIMARY_EXPRESSION_NUMBER;

            convert_to_number(ctx, p_expression_node, false /*ctx->evaluation_is_disabled*/, ctx->options.target);

            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }
        }
        else if (ctx->current->type == TK_KEYWORD__GENERIC)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;
            p_expression_node->first_token = ctx->current;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_GENERIC;

            p_expression_node->generic_selection = generic_selection(ctx, eval_mode);
            if (p_expression_node->generic_selection == NULL)
                throw;

            p_expression_node->last_token = p_expression_node->generic_selection->last_token;

            if (p_expression_node->generic_selection->p_view_selected_expression)
            {
                p_expression_node->type = type_dup(&p_expression_node->generic_selection->p_view_selected_expression->type);

                p_expression_node->object = p_expression_node->generic_selection->p_view_selected_expression->object;
            }
            else
            {
                compiler_diagnostic(C_ERROR_NO_MATCH_FOR_GENERIC, ctx, ctx->current, NULL, "no match for generic");
            }
        }
        else if (ctx->current->type == '(')
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL) throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_PARENTESIS;
            p_expression_node->first_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            p_expression_node->right = expression(ctx, eval_mode);
            if (p_expression_node->right == NULL)
                throw;

            p_expression_node->type = type_dup(&p_expression_node->right->type);
            p_expression_node->object = p_expression_node->right->object;

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            p_expression_node->last_token = ctx->current;
            if (parser_match_tk(ctx, ')') != 0)
                throw;
        }
        else
        {
            compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "unexpected");
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

struct argument_expression_list argument_expression_list(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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

        struct expression* _Owner _Opt p_assignment_expression = assignment_expression(ctx, eval_mode);
        if (p_assignment_expression == NULL)
        {
            argument_expression_delete(p_argument_expression);
            throw;
        }

        p_argument_expression->expression = p_assignment_expression;
        argument_expression_list_push(&list, p_argument_expression);

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        while (ctx->current->type == ',')
        {
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct argument_expression* _Owner _Opt p_argument_expression_2 = calloc(1, sizeof * p_argument_expression_2);
            if (p_argument_expression_2 == NULL)
                throw;
            struct expression* _Owner _Opt p_assignment_expression_2 = assignment_expression(ctx, eval_mode);
            if (p_assignment_expression_2 == NULL)
            {
                argument_expression_delete(p_argument_expression_2);
                throw;
            }
            p_argument_expression_2->expression = p_assignment_expression_2;

            argument_expression_list_push(&list, p_argument_expression_2);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }
        }
    }
    catch
    {
    }
    return list;
}

bool first_of_postfix_expression(const struct parser_ctx* ctx)
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

    if (struct_type->type_qualifier_flags & TYPE_QUALIFIER_OPT)
    {
        /*
          struct X { _Owner int i; };
          _View struct X x;
          x.i ;//is is not _Owner
        */
        p_type->type_qualifier_flags |= TYPE_QUALIFIER_OPT;
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

    if (t.type_qualifier_flags & TYPE_QUALIFIER_OPT)
    {
        /*
           const struct X * p;
        */

        p_type->type_qualifier_flags |= TYPE_QUALIFIER_OPT;
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

struct expression* _Owner _Opt postfix_expression_tail(struct parser_ctx* ctx, struct expression* _Owner p_expression_node_param, enum expression_eval_mode eval_mode)
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
                p_expression_node->last_token = ctx->current;
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = POSTFIX_ARRAY;
                // the result of the subscription operator ([])

                if (!type_is_pointer_or_array(&p_expression_node->type))
                {
                    compiler_diagnostic(C_ERROR_SUBSCRIPTED_VALUE_IS_NEITHER_ARRAY_NOR_POINTER,
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

                    unexpected_end_of_file(ctx);

                    expression_delete(p_expression_node_new);
                    throw;
                }

                /*contem a expresao de dentro do  [ ] */
                p_expression_node_new->right = expression(ctx, eval_mode);
                if (p_expression_node_new->right == NULL)
                {
                    expression_delete(p_expression_node_new);
                    throw;
                }

                if (!type_is_integer(&p_expression_node_new->right->type))
                {
                    compiler_diagnostic(C_ERROR_SUBSCRIPT_IS_NOT_AN_INTEGER,
                                            ctx,
                                            p_expression_node_new->right->first_token,
                                            NULL,
                                            "array subscript is not an integer");

                }

                if (object_has_constant_value(&p_expression_node_new->right->object))
                {
                    unsigned long long index =
                        object_to_unsigned_long_long(&p_expression_node_new->right->object);
                    if (type_is_array(&p_expression_node->type))
                    {
                        if (p_expression_node->type.num_of_elements > 0)
                        {
                            if (index >= (unsigned long long)p_expression_node->type.num_of_elements)
                            {
                                compiler_diagnostic(W_OUT_OF_BOUNDS,
                                                            ctx,
                                                            ctx->current, NULL,
                                                            "index %d is past the end of the array", index);
                            }


                            struct object* _Opt it = object_get_member(&p_expression_node->object, (int)index);

                            if (it != NULL)
                                p_expression_node_new->object = object_make_reference(it);
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
                p_expression_node->last_token = ctx->current;
                p_expression_node_new->first_token = p_expression_node->first_token;
                p_expression_node_new->expression_type = POSTFIX_FUNCTION_CALL;

                if (!type_is_function_or_function_pointer(&p_expression_node->type))
                {
                    compiler_diagnostic(C_ERROR_CALLED_OBJECT_IS_NOT_FUNCTION_OR_FUNCTION_POINTER,
                                                ctx,
                                                ctx->current,
                                                NULL,
                                                "called object is not attr function or function pointer");
                }

                p_expression_node_new->type = get_function_return_type(&p_expression_node->type);

                parser_match(ctx);
                if (ctx->current == NULL)
                {

                    unexpected_end_of_file(ctx);

                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                if (ctx->current->type != ')')
                {
                    p_expression_node_new->argument_expression_list = argument_expression_list(ctx, eval_mode);
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

                make_object(&p_expression_node_new->type, &p_expression_node_new->object, ctx->options.target);
                p_expression_node_new->last_token = ctx->previous;
                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '.')
            {
                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL) throw;
                p_expression_node->last_token = ctx->current;
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = POSTFIX_DOT;
                p_expression_node_new->left = p_expression_node;
                p_expression_node = NULL; /*MOVED*/

                p_expression_node_new->declarator = p_expression_node_new->left->declarator;

                parser_match(ctx);
                if (ctx->current == NULL)
                {

                    unexpected_end_of_file(ctx);

                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                if (p_expression_node_new->left->type.type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
                {
                    assert(p_expression_node_new->left->type.struct_or_union_specifier != NULL);

                    struct struct_or_union_specifier* _Opt p_complete =
                        find_struct_or_union_specifier(ctx, p_expression_node_new->left->type.struct_or_union_specifier->tag_name);

                    if (p_complete)
                        p_complete = get_complete_struct_or_union_specifier(p_complete);

                    if (p_complete)
                    {
                        assert(ctx->current != NULL);

                        int member_index = 0;
                        struct member_declarator* _Opt p_member_declarator =
                            find_member_declarator(&p_complete->member_declaration_list, ctx->current->lexeme, &member_index);

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

                            struct object* object = find_object_declarator_by_index(&p_expression_node_new->left->object, &p_complete->member_declaration_list, member_index);

                            if (object)
                            {
                                p_expression_node_new->object = object_make_reference(object);
                            }
                            else
                            {
                                //not fixed yet
                                 //assert(false);                                    
                            }
                        }
                        else
                        {
                            compiler_diagnostic(C_ERROR_STRUCT_MEMBER_NOT_FOUND,
                                                    ctx,
                                                    ctx->current, NULL,
                                                    "member '%s' not found in 'struct %s'",
                                                    ctx->current->lexeme,
                                                    p_complete->tag_name);
                        }
                    }
                    else
                    {
                        compiler_diagnostic(C_ERROR_STRUCT_MEMBER_NOT_FOUND,
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
                    compiler_diagnostic(C_ERROR_STRUCTURE_OR_UNION_REQUIRED,
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
                        compiler_diagnostic(W_ARRAY_INDIRECTION, ctx, ctx->current, NULL, "using indirection '->' in array");
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
                                compiler_diagnostic(C_ERROR_STRUCT_MEMBER_NOT_FOUND,
                                                            ctx,
                                                            ctx->current, NULL,
                                                            "member '%s' not found in struct '%s'",
                                                            ctx->current->lexeme,
                                                            p_expression_node->type.next->struct_or_union_specifier->tag_name);
                            }
                        }
                        else
                        {
                            compiler_diagnostic(C_ERROR_STRUCT_IS_INCOMPLETE,
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
                        compiler_diagnostic(C_ERROR_STRUCTURE_OR_UNION_REQUIRED,
                                                    ctx,
                                                    ctx->current, NULL,
                                                    "structure or union required");
                    }
                    type_destroy(&item_type);
                }
                else
                {
                    compiler_diagnostic(C_ERROR_STRUCTURE_OR_UNION_REQUIRED,
                                                ctx,
                                                ctx->current, NULL,
                                                "structure or union required");
                }

                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '++')
            {
                p_expression_node->last_token = ctx->current;

                if (type_is_owner(&p_expression_node->type))
                {
                    compiler_diagnostic(C_ERROR_OPERATOR_INCREMENT_CANNOT_BE_USED_IN_OWNER,
                                                ctx,
                                                p_expression_node->first_token, NULL,
                                                "operator ++ cannot be used in _Owner pointers");
                }

                if (!expression_is_lvalue(p_expression_node))
                {
                    compiler_diagnostic(C_ERROR_OPERATOR_NEEDS_LVALUE,
                                                ctx,
                                                p_expression_node->first_token, NULL,
                                                "lvalue required as increment operand");
                }


                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL) throw;

                p_expression_node->last_token = ctx->current;
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = POSTFIX_INCREMENT;

                p_expression_node_new->type = type_dup(&p_expression_node->type);
                parser_match(ctx);
                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '--')
            {
                p_expression_node->last_token = ctx->current;

                if (type_is_owner(&p_expression_node->type))
                {
                    compiler_diagnostic(C_ERROR_OPERATOR_DECREMENT_CANNOT_BE_USED_IN_OWNER,
                                                ctx,
                                                p_expression_node->first_token, NULL,
                                                "operator -- cannot be used in owner pointers");
                }

                if (!expression_is_lvalue(p_expression_node))
                {
                    compiler_diagnostic(C_ERROR_OPERATOR_NEEDS_LVALUE,
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
                    unexpected_end_of_file(ctx);
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = NULL;
                    throw;
                }

                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else
            {
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

struct expression* _Owner _Opt postfix_expression_type_name(struct parser_ctx* ctx, struct type_name* _Owner p_type_name_par, enum expression_eval_mode eval_mode)
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
        p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);

        if (type_is_function(&p_expression_node->type_name->abstract_declarator->type))
        {
            //this keep the typedef for function out.. we must have the function declarator
            const struct declarator* inner = declarator_get_innert_function_declarator(p_expression_node->type_name->abstract_declarator);
            if (inner->direct_declarator->function_declarator == NULL)
            {
                compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, p_expression_node->type_name->first_token, NULL, "missing function declarator");
                throw;
            }

            p_expression_node->expression_type = POSTFIX_EXPRESSION_FUNCTION_LITERAL;



            struct scope* parameters_scope =
                &p_expression_node->type_name->abstract_declarator->direct_declarator->function_declarator->parameters_scope;

            scope_list_push(&ctx->scopes, parameters_scope);

            struct declarator* _Opt p_current_function_opt = ctx->p_current_function_opt;
            ctx->p_current_function_opt = p_expression_node->type_name->abstract_declarator;

            struct scope* p_current_function_scope_opt = ctx->p_current_function_scope_opt;
            ctx->p_current_function_scope_opt = ctx->scopes.tail;

            p_expression_node->compound_statement = function_body(ctx);

            scope_list_pop(&ctx->scopes);
            ctx->p_current_function_opt = p_current_function_opt; //restore
            ctx->p_current_function_scope_opt = p_current_function_scope_opt; //restore

        }
        else
        {
            p_expression_node->expression_type = POSTFIX_EXPRESSION_COMPOUND_LITERAL;
            p_expression_node->braced_initializer = braced_initializer(ctx);
            p_expression_node->type = type_dup(&p_expression_node->type_name->type);
            //TODO

            if (p_expression_node->type.storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF)
            {
            }
            else
            {
                int er = make_object(&p_expression_node->type, &p_expression_node->object, ctx->options.target);
                if (er != 0)
                {
                    compiler_diagnostic(C_ERROR_STRUCT_IS_INCOMPLETE, ctx, p_expression_node->first_token, NULL, "incomplete struct/union type");
                    throw;
                }
            }

            const bool is_constant = type_is_const_or_constexpr(&p_expression_node->type);

            object_default_initialization(&p_expression_node->object, is_constant);

            struct initializer initializer = { 0 };
            initializer.braced_initializer = p_expression_node->braced_initializer;
            initializer.first_token = p_expression_node->first_token;

            const bool requires_constant_initialization = false;

            initializer_init_new(ctx,
                         &p_expression_node->type,
                         &p_expression_node->object,
                         &initializer,
                         is_constant,
                         requires_constant_initialization);

            //initializer_destroy(&initializer); explodindo
        }

        if (ctx->previous == NULL)
            throw;

        p_expression_node->last_token = ctx->previous;

        p_expression_node = postfix_expression_tail(ctx, p_expression_node, eval_mode);
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

struct expression* _Owner _Opt postfix_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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

            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);

            if (parser_match_tk(ctx, ')') != 0)
                throw;
            // printf("\n");
            // print_type(&p_expression_node->type);

            if (type_is_function(&p_expression_node->type))
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
                if (p_expression_node->braced_initializer == NULL) throw;
                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }

                p_expression_node->last_token = ctx->current;
            }
        }
        else
        {
            p_expression_node = primary_expression(ctx, eval_mode);
            if (p_expression_node == NULL)
                throw;
        }

        p_expression_node = postfix_expression_tail(ctx, p_expression_node, eval_mode);
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

    if (ctx->current->type == TK_KEYWORD_CONST)
    {
        struct token* _Opt ahead = parser_look_ahead(ctx);
        if (ahead && ahead->type == '(')
            return true; //const(expr)
    }

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
        ctx->current->type == TK_KEYWORD__COUNTOF ||
        ctx->current->type == TK_KEYWORD__ALIGNOF ||

        ctx->current->type == TK_KEYWORD_GCC__BUILTIN_VA_END ||
        ctx->current->type == TK_KEYWORD_GCC__BUILTIN_VA_ARG ||
        ctx->current->type == TK_KEYWORD_GCC__BUILTIN_C23_VA_START ||
        ctx->current->type == TK_KEYWORD_GCC__BUILTIN_VA_COPY ||
        ctx->current->type == TK_KEYWORD_GCC__BUILTIN_OFFSETOF ||

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
            compiler_diagnostic(C_ERROR_STRUCT_IS_INCOMPLETE,
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

            compiler_diagnostic(W_SIZEOF_ARRAY_ARGUMENT,
                                        ctx,
                                        p_expression->first_token,
                                        NULL,
                                        "sizeof applied to array function parameter");

        }
    }

    return 0; //ok
}

struct expression* _Owner _Opt unary_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
{
    /*
    unary-expression:
        postfix-expression
        ++ unary-expression
        -- unary-expression
        unary-operator cast-expression
        sizeof unary-expression
        sizeof ( type-name )
        _Countof unary-expression   //C2Y
        _Countof ( type-name )      //C2Y
        alignof ( type-name )
    */

    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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

                unexpected_end_of_file(ctx);

                expression_delete(new_expression);
                throw;
            }
            new_expression->right = unary_expression(ctx, eval_mode);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->type = type_dup(&new_expression->right->type);
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == '&' ||
                 ctx->current->type == '*' ||
                 ctx->current->type == '+' ||
                 ctx->current->type == '-' ||
                 ctx->current->type == '~' ||
                 ctx->current->type == '!')
        {

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;

            struct token* op_position = ctx->current; // marcar posicao
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }
#if 0
            //visual studio formater is adding spaces..
            if (style_has_space(ctx->current))
            {
                compiler_diagnostic(W_STYLE, ctx, ctx->current, "don't use spaces");
            }
#endif

            new_expression->right = cast_expression(ctx, eval_mode);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;
            if (op == '!')
            {
                new_expression->expression_type = UNARY_EXPRESSION_NOT;
                if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE &&
                    object_has_constant_value(&new_expression->right->object))
                {
                    char warning_message[200] = { 0 };
                    new_expression->object =
                        object_logical_not(ctx->options.target, &new_expression->right->object, warning_message);
                }
                new_expression->type = type_make_int_bool_like();
            }
            else if (op == '~')
            {
                if (!type_is_integer(&new_expression->right->type))
                {
                    compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_INTEGER,
                                               ctx,
                                               op_position,
                                               NULL,
                                               "requires integer type");

                    expression_delete(new_expression);
                    throw;
                }

                new_expression->expression_type = UNARY_EXPRESSION_BITNOT;

                /*
                The result of the ~ operator is the bitwise complement of its (promoted) operand (that is, each bit in
                the result is set if and only if the corresponding bit in the converted operand is not set). The integer
                promotions are performed on the operand, and the result has the promoted type. If the promoted
                type is an unsigned type, the expression ~E is equivalent to the maximum value representable in
                that type minus E.
                */
                struct type promoted = type_dup(&new_expression->right->type);
                type_integer_promotion(&promoted);
                new_expression->type = promoted;

                if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE &&
                  object_has_constant_value(&new_expression->right->object))
                {
                    char warning_message[200] = { 0 };
                    new_expression->object =
                        object_bitwise_not(ctx->options.target, &new_expression->right->object, warning_message);
                }
            }
            else if (op == '-' || op == '+')
            {
                if (op == '-')
                    new_expression->expression_type = UNARY_EXPRESSION_NEG;
                else
                    new_expression->expression_type = UNARY_EXPRESSION_PLUS;

                //promote
                new_expression->type = type_common(&new_expression->right->type, &new_expression->right->type, ctx->options.target);

                if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE &&
                    object_has_constant_value(&new_expression->right->object))
                {
                    char warning_message[200] = { 0 };
                    if (op == '-')
                    {
                        new_expression->object =
                            object_unary_minus(ctx->options.target, &new_expression->right->object, warning_message);
                    }
                    else if (op == '+')
                    {
                        new_expression->object =
                            object_unary_plus(ctx->options.target, &new_expression->right->object, warning_message);
                    }
                }
            }
            else if (op == '*')
            {
                new_expression->expression_type = UNARY_EXPRESSION_CONTENT;
                // the result of the indirection(unary*) operator applied to a pointer to object

                if (!type_is_pointer_or_array(&new_expression->right->type))
                {
                    compiler_diagnostic(C_ERROR_INDIRECTION_REQUIRES_POINTER_OPERAND,
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
                    compiler_diagnostic(W_ARRAY_INDIRECTION,
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
                    compiler_diagnostic(C_ERROR_ADDRESS_OF_REGISTER,
                                                ctx,
                                                new_expression->right->first_token,
                                                NULL,
                                                "lvalue required as unary '&' operand");
                }

                if (new_expression->right->type.storage_class_specifier_flags & STORAGE_SPECIFIER_REGISTER)
                {
                    const char* variable_name = "?";

                    if (new_expression->right->declarator &&
                        new_expression->right->declarator->name_opt)
                    {
                        variable_name = new_expression->right->declarator->name_opt->lexeme;
                    }

                    compiler_diagnostic(C_ERROR_ADDRESS_OF_REGISTER,
                                                ctx,
                                                new_expression->right->first_token,
                                                NULL,
                                                "address of register variable 'x' requested",
                                                variable_name);

                }

                new_expression->type = type_add_pointer(&new_expression->right->type, ctx->options.null_checks_enabled);
                new_expression->type.address_of = true;
            }
            else
            {
                expression_delete(new_expression);
                compiler_diagnostic(C_ERROR_INVALID_TOKEN,
                                            ctx,
                                            ctx->current,
                    NULL,
                                            "invalid token");
                throw;
            }
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_GCC__BUILTIN_C23_VA_START)
        {


            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;
            new_expression->first_token = ctx->current;
            new_expression->expression_type = UNARY_EXPRESSION_GCC__BUILTIN_VA_START;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, '(') != 0)
            {
                expression_delete(new_expression);
                throw;
            }
            new_expression->left = unary_expression(ctx, eval_mode);
            if (new_expression->left == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            if (ctx->current->type == ',')
            {
                parser_match(ctx);
                new_expression->right = unary_expression(ctx, eval_mode);
                if (new_expression->right == NULL)
                {
                    expression_delete(new_expression);
                    throw;
                }
            }

            if (parser_match_tk(ctx, ')') != 0)
            {
                expression_delete(new_expression);
                throw;
            }
            new_expression->type = make_void_type();
            return new_expression;

        }
        else if (ctx->current->type == TK_KEYWORD_GCC__BUILTIN_VA_END)
        {


            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;
            new_expression->first_token = ctx->current;
            new_expression->expression_type = UNARY_EXPRESSION_GCC__BUILTIN_VA_END;

            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, '(') != 0)
            {
                expression_delete(new_expression);
                throw;
            }
            new_expression->left = unary_expression(ctx, eval_mode);
            if (new_expression->left == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, ')') != 0)
            {
                expression_delete(new_expression);
                throw;
            }
            new_expression->type = make_void_type();
            return new_expression;

        }
        else if (ctx->current->type == TK_KEYWORD_GCC__BUILTIN_VA_ARG)
        {
            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;
            new_expression->first_token = ctx->current;
            new_expression->expression_type = UNARY_EXPRESSION_GCC__BUILTIN_VA_ARG;

            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, '(') != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->left = unary_expression(ctx, eval_mode);
            if (new_expression->left == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, ',') != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->type_name = type_name(ctx);
            if (new_expression->type_name == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, ')') != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->type = type_dup(&new_expression->type_name->type);
            return new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_GCC__BUILTIN_VA_COPY)
        {
            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;
            new_expression->first_token = ctx->current;
            new_expression->expression_type = UNARY_EXPRESSION_GCC__BUILTIN_VA_COPY;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, '(') != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->left = unary_expression(ctx, eval_mode);
            if (new_expression->left == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, ',') != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->right = unary_expression(ctx, eval_mode);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, ')') != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->type = make_void_type();
            return new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_GCC__BUILTIN_OFFSETOF)
        {
            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;
            new_expression->first_token = ctx->current;
            new_expression->expression_type = UNARY_EXPRESSION_GCC__BUILTIN_OFFSETOF;

            parser_match(ctx);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, '(') != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->type_name = type_name(ctx);
            if (new_expression->type_name == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            if (parser_match_tk(ctx, ',') != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            if (ctx->current->type != TK_IDENTIFIER)
            {
                //TODO
                //https://gcc.gnu.org/onlinedocs/gcc/Offsetof.html#Offsetof
                expression_delete(new_expression);
                throw;
            }
            new_expression->offsetof_member_designator = ctx->current;

            parser_match(ctx);

            if (parser_match_tk(ctx, ')') != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->type = make_size_t_type(ctx->options.target);

            size_t offsetof = 0;

            enum sizeof_error e =
                type_get_offsetof(&new_expression->type_name->type, new_expression->offsetof_member_designator->lexeme, &offsetof, ctx->options.target);

            if (e != 0)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->object = object_make_size_t(ctx->options.target, offsetof);

            return new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_SIZEOF)
        {
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;

            if (first_of_type_name_ahead(ctx))
            {
                new_expression->expression_type = UNARY_EXPRESSION_SIZEOF_TYPE;
                if (parser_match_tk(ctx, '(') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }
                new_expression->type_name = type_name(ctx);
                if (new_expression->type_name == NULL)
                {
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->type = make_size_t_type(ctx->options.target);

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->last_token = ctx->current;

                if (parser_match_tk(ctx, ')') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }

                if (check_sizeof_argument(ctx, new_expression, &new_expression->type_name->type) != 0)
                {
                    //not fatal error
                    //fatal will be if someone need the sizeof at compile time
                    //but we don't have the object set here
                }
                else
                {
                    if (type_is_vla(&new_expression->type_name->abstract_declarator->type))
                    {
                        //not constant
                    }
                    else
                    {
                        size_t type_sizeof = 0;
                        if (type_get_sizeof(&new_expression->type_name->abstract_declarator->type, &type_sizeof, ctx->options.target) != 0)
                        {
                            expression_delete(new_expression);
                            throw;
                        }

                        new_expression->object = object_make_size_t(ctx->options.target, type_sizeof);
                    }
                }
            }
            else
            {
                new_expression->right = unary_expression(ctx, EXPRESSION_EVAL_MODE_TYPE);
                if (new_expression->right == NULL)
                {
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
                    size_t sz = 0;
                    if (type_get_sizeof(&new_expression->right->type, &sz, ctx->options.target) != 0)
                    {
                        expression_delete(new_expression);
                        throw;
                    }
                    new_expression->object = object_make_size_t(ctx->options.target, sz);
                }
            }

            type_destroy(&new_expression->type);
            new_expression->type = type_make_size_t(ctx->options.target);
            p_expression_node = new_expression;

        }
        else if (ctx->current->type == TK_KEYWORD__COUNTOF)//C2Y
        {
            // defer would be nice here...


            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;

            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            new_expression->expression_type = UNARY_EXPRESSION_COUNTOF;

            if (first_of_type_name_ahead(ctx))
            {
                if (parser_match_tk(ctx, '(') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }
                new_expression->type_name = type_name(ctx);
                if (new_expression->type_name == NULL)
                {
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->type = make_size_t_type(ctx->options.target);

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->last_token = ctx->current;

                if (parser_match_tk(ctx, ')') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }

                if (type_is_enum(&new_expression->type_name->abstract_declarator->type))
                {
                    const struct enum_specifier* _Opt p_enum_specifier =
                        get_complete_enum_specifier(new_expression->type_name->type.enum_specifier);
                    size_t nelements = 0;
                    if (p_enum_specifier)
                    {
                        struct enumerator* _Owner _Opt p =
                            p_enum_specifier->enumerator_list.head;
                        while (p)
                        {
                            nelements++;
                            p = p->next;
                        }
                    }
                    new_expression->object = object_make_size_t(ctx->options.target, nelements);
                }
                else if (type_is_array(&new_expression->type_name->abstract_declarator->type))
                {
                    size_t nelements = new_expression->type_name->abstract_declarator->type.num_of_elements;
                    if (nelements > 0)
                        new_expression->object = object_make_size_t(ctx->options.target, nelements);
                }
                else
                {
                    compiler_diagnostic(C_INVALID_ARGUMENT_NELEMENTSOF,
                                        ctx,
                                        new_expression->type_name->first_token,
                                        NULL,
                                        "argument of _Countof must be an array");

                    expression_delete(new_expression);
                    throw;
                }
            }
            else
            {
                if (parser_match_tk(ctx, '(') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->right = unary_expression(ctx, EXPRESSION_EVAL_MODE_TYPE);

                if (new_expression->right == NULL)
                {
                    expression_delete(new_expression);
                    throw;
                }

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->last_token = ctx->current;

                if (parser_match_tk(ctx, ')') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }

                if (type_is_enum(&new_expression->right->type))
                {
                    const struct enum_specifier* _Opt p_enum_specifier =
                        get_complete_enum_specifier(new_expression->right->type.enum_specifier);
                    size_t nelements = 0;
                    if (p_enum_specifier)
                    {
                        struct enumerator* _Owner _Opt p =
                            p_enum_specifier->enumerator_list.head;
                        while (p)
                        {
                            nelements++;
                            p = p->next;
                        }
                    }
                    new_expression->object = object_make_size_t(ctx->options.target, nelements);

                }
                else if (type_is_array(&new_expression->right->type))
                {
                    size_t nelements = new_expression->right->type.num_of_elements;
                    if (nelements > 0)
                    {
                        new_expression->object = object_make_size_t(ctx->options.target, nelements);
                    }
                    else
                    {
                        //vla [n][2] but not vla[2][n]
                    }
                }
                else
                {
                    compiler_diagnostic(C_INVALID_ARGUMENT_NELEMENTSOF,
                        ctx,
                        new_expression->right->first_token,
                        NULL,
                        "argument of _Countof must be an array");

                    expression_delete(new_expression);
                    throw;
                }
            }

            type_destroy(&new_expression->type);
            new_expression->type = type_make_size_t(ctx->options.target);
            p_expression_node = new_expression;

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
                throw;
            }
            new_expression->right = expression(ctx, eval_mode);

            if (parser_match_tk(ctx, ')') != 0)
            {
                expression_delete(new_expression);
                throw;
            }
            return new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_CONST) /*new keyword consteval?? */
        {
            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->expression_type = UNARY_EXPRESSION_CONSTEVAL;
            new_expression->first_token = ctx->current;

            parser_match(ctx);

            if (ctx->current == NULL || parser_match_tk(ctx, '(') != 0)
            {
                expression_delete(new_expression);
                throw;
            }
            new_expression->right = constant_expression(ctx, true, eval_mode);

            if (parser_match_tk(ctx, ')') != 0)
            {
                expression_delete(new_expression);
                throw;
            }
            return new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD__ALIGNOF)
        {


            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;

            if (first_of_type_name_ahead(ctx))
            {
                new_expression->expression_type = UNARY_EXPRESSION_ALIGNOF_TYPE;
                if (parser_match_tk(ctx, '(') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }
                new_expression->type_name = type_name(ctx);
                if (new_expression->type_name == NULL)
                {
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->type = make_size_t_type(ctx->options.target);

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->last_token = ctx->current;

                if (parser_match_tk(ctx, ')') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }

                if (check_sizeof_argument(ctx, new_expression, &new_expression->type_name->type) != 0)
                {
                    //not fatal error
                    //fatal will be if someone need the sizeof at compile time
                    //but we don't have the object set here
                }
                else
                {
                    if (type_is_vla(&new_expression->type_name->abstract_declarator->type))
                    {
                        //not constant
                    }
                    else
                    {
                        size_t type_alignof = 0;
                        type_alignof = type_get_alignof(&new_expression->type_name->abstract_declarator->type, ctx->options.target);

                        new_expression->object = object_make_size_t(ctx->options.target, type_alignof);
                    }
                }
            }
            else
            {
                new_expression->right = unary_expression(ctx, EXPRESSION_EVAL_MODE_TYPE);
                if (new_expression->right == NULL)
                {
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->expression_type = UNARY_EXPRESSION_ALIGNOF_EXPRESSION;

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
                    size_t sz = 0;
                    sz = type_get_alignof(&new_expression->right->type, ctx->options.target);
                    new_expression->object = object_make_size_t(ctx->options.target, sz);
                }
            }

            type_destroy(&new_expression->type);
            new_expression->type = type_make_size_t(ctx->options.target);
            p_expression_node = new_expression;


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



            struct token* traits_token = ctx->current;

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = UNARY_EXPRESSION_TRAITS;

            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            struct type* _Opt p_type = NULL;
            if (first_of_type_name_ahead(ctx))
            {
                if (parser_match_tk(ctx, '(') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }
                new_expression->type_name = type_name(ctx);
                if (new_expression->type_name == NULL)
                {
                    expression_delete(new_expression);
                    throw;
                }

                if (ctx->current == NULL)
                {

                    unexpected_end_of_file(ctx);

                    expression_delete(new_expression);
                    throw;
                }

                new_expression->last_token = ctx->current;
                if (parser_match_tk(ctx, ')') != 0)
                {
                    expression_delete(new_expression);
                    throw;
                }
                p_type = &new_expression->type_name->abstract_declarator->type;
            }
            else
            {

                new_expression->right = unary_expression(ctx, EXPRESSION_EVAL_MODE_TYPE);
                if (new_expression->right == NULL)
                {
                    expression_delete(new_expression);
                    throw;
                }

                p_type = &new_expression->right->type;
                if (ctx->previous == NULL)
                {
                    expression_delete(new_expression);
                    throw;
                }

                new_expression->last_token = ctx->previous;
            }

            switch (traits_token->type)
            {
            case TK_KEYWORD_IS_LVALUE:
                assert(new_expression->right != NULL);
                new_expression->object = object_make_signed_int(ctx->options.target, expression_is_lvalue(new_expression->right));
                break;

            case TK_KEYWORD_IS_CONST:
                new_expression->object = object_make_signed_int(ctx->options.target, type_is_const(p_type));
                break;
            case TK_KEYWORD_IS_OWNER:
                new_expression->object = object_make_signed_int(ctx->options.target, type_is_owner(p_type));
                break;

            case TK_KEYWORD_IS_POINTER:
                new_expression->object = object_make_signed_int(ctx->options.target, type_is_pointer(p_type));

                break;
            case TK_KEYWORD_IS_FUNCTION:
                new_expression->object = object_make_signed_int(ctx->options.target, type_is_function(p_type));

                break;
            case TK_KEYWORD_IS_ARRAY:
                new_expression->object = object_make_signed_int(ctx->options.target, type_is_array(p_type));

                break;
            case TK_KEYWORD_IS_ARITHMETIC:
                new_expression->object = object_make_signed_int(ctx->options.target, type_is_arithmetic(p_type));

                break;
            case TK_KEYWORD_IS_SCALAR:
                new_expression->object = object_make_signed_int(ctx->options.target, type_is_scalar(p_type));

                break;
            case TK_KEYWORD_IS_FLOATING_POINT:
                new_expression->object = object_make_signed_int(ctx->options.target, type_is_floating_point(p_type));

                break;
            case TK_KEYWORD_IS_INTEGRAL:
                new_expression->object = object_make_signed_int(ctx->options.target, type_is_integer(p_type));

                break;

            default:
                assert(false);
            }

            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
        }
        else // if (is_first_of_primary_expression(ctx, eval_mode))
        {
            p_expression_node = postfix_expression(ctx, eval_mode);
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

struct expression* _Owner _Opt cast_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (first_of_type_name_ahead(ctx))
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL)
                throw;

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


            if (parser_match_tk(ctx, ')') != 0)
                throw;

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            if (ctx->current->type == '{')
            {
                // Thinking it was a cast expression was a mistake... 
                // because the { appeared then it is a compound literal which is a postfix.
                struct expression* _Owner _Opt new_expression = postfix_expression_type_name(ctx, p_expression_node->type_name, eval_mode);
                p_expression_node->type_name = NULL; // MOVED

                if (new_expression == NULL) throw;

                expression_delete(p_expression_node);
                p_expression_node = new_expression;
            }
            else if (is_first_of_unary_expression(ctx))
            {
                p_expression_node->left = cast_expression(ctx, eval_mode);
                if (p_expression_node->left == NULL)
                {
                    expression_delete(p_expression_node);
                    p_expression_node = NULL;
                    throw;
                }

                if (type_is_floating_point(&p_expression_node->type) &&
                    type_is_pointer(&p_expression_node->left->type))
                {
                    compiler_diagnostic(C_ERROR_POINTER_TO_FLOATING_TYPE,
                        ctx,
                        p_expression_node->first_token,
                        NULL,
                        "pointer type cannot be converted to any floating type");
                }
                else if (type_is_pointer(&p_expression_node->type) &&
                         type_is_floating_point(&p_expression_node->left->type))
                {
                    compiler_diagnostic(C_ERROR_FLOATING_TYPE_TO_POINTER,
                     ctx,
                     p_expression_node->first_token,
                     NULL,
                     "A floating type cannot be converted to any pointer type");
                }
                else if (type_is_nullptr_t(&p_expression_node->left->type))
                {
                    if (type_is_void(&p_expression_node->type) ||
                        type_is_bool(&p_expression_node->type) ||
                        type_is_pointer(&p_expression_node->type))
                    {
                        /*
                          The type nullptr_t shall not be converted to any type other than
                          void, bool or a pointer type
                        */
                    }
                    else
                    {
                        compiler_diagnostic(C_ERROR_NULLPTR_CAST_ERROR,
                        ctx,
                        p_expression_node->first_token,
                        NULL,
                        "cannot cast nullptr_t to this type");
                    }
                }
                else if (type_is_nullptr_t(&p_expression_node->type))
                {
                    /*
                      If the target type is nullptr_t, the cast expression shall
                      be a null pointer constant or have type nullptr_t.
                    */

                    if (expression_is_null_pointer_constant(p_expression_node->left) ||
                        type_is_nullptr_t(&p_expression_node->left->type))
                    {
                        //ok
                    }
                    else
                    {
                        compiler_diagnostic(C_ERROR_NULLPTR_CAST_ERROR,
                        ctx,
                        p_expression_node->left->first_token,
                        NULL,
                        "cannot cast this expression to nullptr_t");
                    }
                }

                if (p_expression_node->left->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN &&
                    type_is_owner(&p_expression_node->left->type))
                {
                    if (!type_is_owner(&p_expression_node->type))
                    {
                        if (type_is_pointer(&p_expression_node->left->type))
                        {
                            //(int*) malloc(1)
                            compiler_diagnostic(W_OWNERSHIP_DISCARDING_OWNER, ctx, p_expression_node->first_token, NULL, "discarding _Owner pointer");
                        }
                        else
                        {
                            compiler_diagnostic(W_OWNERSHIP_DISCARDING_OWNER, ctx, p_expression_node->first_token, NULL, "discarding _Owner");
                        }
                    }
                }

                type_destroy(&p_expression_node->type);
                p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);

                if (type_is_same(&p_expression_node->type, &p_expression_node->left->type, true))
                {
                    if (p_expression_node->first_token->flags & TK_FLAG_MACRO_EXPANDED)
                    {
                        /*
                             not a warning when used inside macros
                        */
                    }
                    else
                    {
                        if (
                             (p_expression_node->type.storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF) ||
                             (p_expression_node->left->type.storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF)
                            )
                        {
                            /*
                            if any of them are typedef, no warning
                            */
                        }
                        else
                        {
                            //compiler_diagnostic(W_CAST_TO_SAME_TYPE,
                              //                  ctx,
                                //                p_expression_node->first_token,
                                  //              NULL,
                                    //            "casting to the same type");
                        }
                    }
                }

                if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE &&
                    object_has_constant_value(&p_expression_node->left->object))
                {
                    enum object_type vt = type_to_object_type(&p_expression_node->type, ctx->options.target);
                    p_expression_node->object = object_cast(ctx->options.target, vt, &p_expression_node->left->object);
                }

                p_expression_node->type.storage_class_specifier_flags =
                    p_expression_node->left->type.storage_class_specifier_flags;
            }
            else
            {
                compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "expected expression");
            }
        }
        else if (is_first_of_unary_expression(ctx))
        {
            p_expression_node = unary_expression(ctx, eval_mode);
            if (p_expression_node == NULL)
            {
                throw;
            }
        }
        else
        {
            compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "expected expression");
            assert(p_expression_node == NULL);
            throw;
        }

        if (ctx->current == NULL || ctx->previous == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_expression_node->last_token = ctx->previous;
    }
    catch
    {
        expression_delete(p_expression_node);
        p_expression_node = NULL;
    }

    return p_expression_node;
}

struct expression* _Owner _Opt multiplicative_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        p_expression_node = cast_expression(ctx, eval_mode);
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
            const enum token_type op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            switch (op)
            {
            case '*':
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MULT;
                break;
            case '/':
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_DIV;
                break;
            case '%':
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MOD;
                break;
            default:
                assert(false);
                break;
            }


            new_expression->left = p_expression_node;
            p_expression_node = NULL; // MOVED

            new_expression->right = cast_expression(ctx, eval_mode);

            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }
            new_expression->last_token = new_expression->right->last_token;


            if (op == '%')
            {
                /*The operands of the % operator shall have integer type*/
                if (!type_is_integer(&new_expression->left->type))
                {
                    compiler_diagnostic(C_ERROR_LEFT_IS_NOT_INTEGER,
                        ctx,
                        new_expression->left->first_token,
                        NULL,
                        "left is not an integer type");
                }

                if (!type_is_integer(&new_expression->right->type))
                {
                    compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_INTEGER,
                        ctx,
                        new_expression->right->first_token,
                        NULL,
                        "right is not an integer type");
                }
            }
            else
            {
                /*Each of the operands shall have arithmetic type.*/
                if (!type_is_arithmetic(&new_expression->left->type))
                {
                    compiler_diagnostic(C_ERROR_LEFT_IS_NOT_ARITHMETIC,
                        ctx,
                        new_expression->left->first_token,
                        NULL,
                        "left is not an arithmetic type");
                }

                if (!type_is_arithmetic(&new_expression->right->type))
                {
                    compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_ARITHMETIC,
                        ctx,
                        new_expression->right->first_token,
                        NULL,
                        "right is not an arithmetic type");
                }
            }
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);

            if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE)
            {
                if (object_has_constant_value(&new_expression->left->object) &&
                    object_has_constant_value(&new_expression->right->object))
                {
                    const struct marker m =
                    {
                        .p_token_begin = new_expression->left->first_token,
                        .p_token_end = new_expression->right->last_token
                    };

                    char warning_message[200] = { 0 };
                    if (op == '*')
                    {
                        new_expression->object = object_mul(ctx->options.target,
                                             &new_expression->left->object,
                                             &new_expression->right->object,
                                             warning_message);

                        if (warning_message[0] != '\0')
                        {
                            compiler_diagnostic(W_INTEGER_OVERFLOW,
                                ctx,
                                NULL,
                                &m,
                                "%s",
                                warning_message);
                        }
                    }
                    else if (op == '/')
                    {
                        new_expression->object = object_div(ctx->options.target,
                                             &new_expression->left->object,
                                             &new_expression->right->object,
                                             warning_message);

                        if (warning_message[0] != '\0')
                        {
                            compiler_diagnostic(W_DIVIZION_BY_ZERO,
                                ctx,
                                NULL,
                                &m,
                                "%s",
                                warning_message);
                        }
                    }
                    else if (op == '%')
                    {
                        new_expression->object = object_mod(ctx->options.target,
                                             &new_expression->left->object,
                                             &new_expression->right->object,
                                             warning_message);

                        if (warning_message[0] != '\0')
                        {
                            compiler_diagnostic(W_DIVIZION_BY_ZERO,
                                ctx,
                                NULL,
                                &m,
                                "%s",
                                warning_message);
                        }
                    }
                }
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


struct expression* _Owner _Opt additive_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        p_expression_node = multiplicative_expression(ctx, eval_mode);
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
                compiler_diagnostic(C_ERROR_OUT_OF_MEM, ctx, ctx->current, NULL, "out of mem");
                throw;
            }

            new_expression->first_token = ctx->current;

            enum token_type op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                throw;
            }

            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/


            new_expression->right = multiplicative_expression(ctx, eval_mode);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;



            if (!type_is_scalar(&new_expression->left->type) && !type_is_array(&new_expression->left->type))
            {
                compiler_diagnostic(C_ERROR_LEFT_IS_NOT_SCALAR, ctx, operator_position, NULL, "left operator is not scalar");
            }
            if (!type_is_scalar(&new_expression->right->type) && !type_is_array(&new_expression->right->type))
            {
                compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_SCALAR, ctx, operator_position, NULL, "right operator is not scalar");
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
                    new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);

                    if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE)
                    {
                        if (object_has_constant_value(&new_expression->left->object) &&
                            object_has_constant_value(&new_expression->right->object))
                        {
                            const struct marker m =
                            {
                                .p_token_begin = new_expression->left->first_token,
                                .p_token_end = new_expression->right->last_token
                            };

                            char warning_message[200] = { 0 };

                            new_expression->object = object_add(ctx->options.target,
                                                 &new_expression->left->object,
                                                 &new_expression->right->object,
                                                 warning_message);

                            if (warning_message[0] != '\0')
                            {
                                compiler_diagnostic(W_INTEGER_OVERFLOW,
                                    ctx,
                                    NULL,
                                    &m,
                                    "%s",
                                    warning_message);
                            }
                        }
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
                            compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "expected integer type on right");
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
                            compiler_diagnostic(C_ERROR_LEFT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "expected integer type on left");
                        }
                    }
                    else
                    {
                        compiler_diagnostic(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid types additive expression");
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
                    new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);

                    if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE)
                    {

                        if (object_has_constant_value(&new_expression->left->object) &&
                            object_has_constant_value(&new_expression->right->object))
                        {
                            const struct marker m =
                            {
                                .p_token_begin = new_expression->left->first_token,
                                .p_token_end = new_expression->right->last_token
                            };

                            char warning_message[200] = { 0 };

                            new_expression->object = object_sub(ctx->options.target,
                                                 &new_expression->left->object,
                                                 &new_expression->right->object,
                                                 warning_message);

                            if (warning_message[0] != '\0')
                            {
                                compiler_diagnostic(W_INTEGER_OVERFLOW,
                                    ctx,
                                    NULL,
                                    &m,
                                    "%s",
                                    warning_message);
                            }
                        }
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
                                compiler_diagnostic(C_ERROR_INCOMPATIBLE_POINTER_TYPES, ctx, ctx->current, NULL, "incompatible pointer types");
                            }

                            new_expression->type = type_make_ptrdiff_t(ctx->options.target);
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
                                compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "right must be integer type");
                            }
                        }
                    }
                    else
                    {
                        compiler_diagnostic(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid types for operator -");
                    }
                }
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

    return p_expression_node;
}

struct expression* _Owner _Opt shift_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        p_expression_node = additive_expression(ctx, eval_mode);
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
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }

            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = multiplicative_expression(ctx, eval_mode);
            if (new_expression->right == NULL)
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

            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);

            //Each of the operands shall have integer type.
            if (!type_is_integer(&new_expression->left->type))
            {
                compiler_diagnostic(C_ERROR_LEFT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "left type must be an integer type");
                throw;
            }

            if (!type_is_integer(&new_expression->right->type))
            {
                compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "right type must be an integer type");
                throw;
            }

            if (object_has_constant_value(&new_expression->left->object) &&
                object_has_constant_value(&new_expression->right->object))
            {
                char warning_message[200] = { 0 };

                if (op == '<<')
                {
                    new_expression->object = object_shift_left(ctx->options.target,
                    &new_expression->left->object,
                    &new_expression->right->object, warning_message);
                }
                else
                {
                    new_expression->object = object_shift_right(ctx->options.target,
                      &new_expression->left->object,
                      &new_expression->right->object, warning_message);
                }
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

static void check_comparison(struct parser_ctx* ctx,
    struct expression* p_a_expression,
    struct expression* p_b_expression,
    const struct token* op_token)
{
    //TODO more checks unsigned < 0
    bool equal_not_equal =
        op_token->type == '!=' ||
        op_token->type == '==';

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
            compiler_diagnostic(W_ENUN_CONVERSION,
                                        ctx,
                                        op_token, NULL,
                                        "comparison between pointer and integer");
        }
    }

    if (type_is_bool(p_a_type) &&
        !(type_is_bool(p_b_type) || type_is_essential_bool(p_b_type)))
    {
        if (equal_not_equal && (object_is_zero(&p_b_expression->object) ||
            object_is_one(&p_b_expression->object)))
        {
            //no warning when comparing == 0 == 1 != 0 != 0
        }
        else
        {
            compiler_diagnostic(W_BOOL_COMPARISON,
                                 ctx,
                                 op_token, NULL,
                                 "comparison bool with non bool");
        }
    }

    if (type_is_bool(p_b_type) &&
        !(type_is_bool(p_a_type) || type_is_essential_bool(p_a_type))
        )
    {
        if (equal_not_equal &&
            (object_is_zero(&p_a_expression->object) ||
                object_is_one(&p_a_expression->object)))
        {
            //no warning when comparing == 0 == 1 != 0 != 0
        }
        else
        {
            compiler_diagnostic(W_BOOL_COMPARISON,
                                 ctx,
                                 op_token, NULL,
                                 "comparison bool with non bool");
        }
    }

    check_diferent_enuns(ctx,
                         op_token,
                         p_a_expression,
                         p_b_expression,
                         "comparing different enums.");
}

struct expression* _Owner _Opt relational_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        p_expression_node = shift_expression(ctx, eval_mode);
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
            struct token* optk = ctx->current;
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = shift_expression(ctx, eval_mode);
            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = NULL;
                throw;
            }

            check_comparison(ctx, new_expression->left, new_expression->right, optk);

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

            //Each of the operands shall have arithmetic type
            if (type_is_arithmetic(&new_expression->left->type) &&
                type_is_arithmetic(&new_expression->right->type))
            {
                new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);

                if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE)
                {
                    if (object_has_constant_value(&new_expression->left->object) &&
                        object_has_constant_value(&new_expression->right->object))
                    {

                        const struct marker m =
                        {
                            .p_token_begin = new_expression->left->first_token,
                            .p_token_end = new_expression->right->last_token
                        };



                        char warning_message[200] = { 0 };
                        enum diagnostic_id warning_id = 0;

                        if (op == '>=')
                        {
                            new_expression->object = object_greater_than_or_equal(ctx->options.target,
                                                 &new_expression->left->object,
                                                 &new_expression->right->object,
                                                 warning_message);
                            warning_id = W_INTEGER_OVERFLOW;
                            if (warning_message[0] != '\0')
                            {
                                compiler_diagnostic(warning_id,
                                    ctx,
                                    NULL,
                                    &m,
                                    "%s",
                                    warning_message);
                            }
                           
                        }
                        else if (op == '<=')
                        {
                            new_expression->object = object_smaller_than_or_equal(ctx->options.target,
                                                 &new_expression->left->object,
                                                 &new_expression->right->object,
                                                 warning_message);
                            warning_id = W_INTEGER_OVERFLOW;
                            if (warning_message[0] != '\0')
                            {
                                compiler_diagnostic(warning_id,
                                    ctx,
                                    NULL,
                                    &m,
                                    "%s",
                                    warning_message);
                            }
                          
                        }
                        else if (op == '>')
                        {
                            new_expression->object= object_greater_than(ctx->options.target,
                                                 &new_expression->left->object,
                                                 &new_expression->right->object,
                                                 warning_message);
                            warning_id = W_INTEGER_OVERFLOW;
                            if (warning_message[0] != '\0')
                            {
                                compiler_diagnostic(warning_id,
                                    ctx,
                                    NULL,
                                    &m,
                                    "%s",
                                    warning_message);
                            }
                         
                        }
                        else if (op == '<')
                        {
                            new_expression->object= object_smaller_than(ctx->options.target,
                                                 &new_expression->left->object,
                                                 &new_expression->right->object,
                                                 warning_message);
                            warning_id = W_INTEGER_OVERFLOW;
                            if (warning_message[0] != '\0')
                            {
                                compiler_diagnostic(warning_id,
                                    ctx,
                                    NULL,
                                    &m,
                                    "%s",
                                    warning_message);
                            }
                     
                        }

                    }
                }
            }

            type_destroy(&new_expression->type);
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
        assert(left->type.enum_specifier);
        assert(right->type.enum_specifier);

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

            compiler_diagnostic(W_ENUN_CONVERSION,
                                        ctx,
                                        operator_token, NULL,
                                        finalmessage,
                                        lefttag,
                                        righttag);
        }
    }
}

struct expression* _Owner _Opt equality_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        p_expression_node = relational_expression(ctx, eval_mode);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '==' ||
                   ctx->current->type == '!='))
        {
            struct token* p_token_operator = ctx->current;
            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;

            struct token* operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            if (operator_token->type == '==')
                new_expression->expression_type = EQUALITY_EXPRESSION_EQUAL;
            else
                new_expression->expression_type = EQUALITY_EXPRESSION_NOT_EQUAL;

            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = relational_expression(ctx, eval_mode);
            if (new_expression->right == NULL)
                throw;

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            check_comparison(ctx, new_expression->left, new_expression->right, p_token_operator);

            new_expression->last_token = new_expression->right->last_token;
            new_expression->first_token = operator_token;

            if (object_has_constant_value(&new_expression->left->object) &&
                object_has_constant_value(&new_expression->right->object))
            {
                if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE)
                {
                    char warning_message[200] = { 0 };
                    if (p_token_operator->type == '==')
                    {
                        new_expression->object = object_equal(ctx->options.target,
                         &new_expression->left->object,
                         &new_expression->right->object,
                         warning_message);
                    }
                    else
                    {
                        new_expression->object = object_not_equal(ctx->options.target,
                                                                  &new_expression->left->object,
                                                                  &new_expression->right->object,
                                                                  warning_message);
                    }
                }
            }

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

struct expression* _Owner _Opt and_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        p_expression_node = equality_expression(ctx, eval_mode);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL && ctx->current->type == '&')
        {
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = AND_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = equality_expression(ctx, eval_mode);
            if (new_expression->right == NULL)
                throw;

            new_expression->last_token = new_expression->right->last_token;

            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);

            //Each of the operands shall have integer type.
            if (!type_is_integer(&new_expression->left->type))
            {
                compiler_diagnostic(C_ERROR_LEFT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "left type must be an integer type");
                throw;
            }

            if (!type_is_integer(&new_expression->right->type))
            {
                compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "right type must be an integer type");
                throw;
            }

            if (object_has_constant_value(&new_expression->left->object) &&
                object_has_constant_value(&new_expression->right->object))
            {
                char warning_message[200] = { 0 };
                new_expression->object = object_bitwise_and(ctx->options.target,
                    &new_expression->left->object,
                    &new_expression->right->object, warning_message);
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

struct expression* _Owner _Opt  exclusive_or_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        p_expression_node = and_expression(ctx, eval_mode);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '^'))
        {
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = EXCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = and_expression(ctx, eval_mode);
            if (new_expression->right == NULL)
                throw;

            new_expression->last_token = new_expression->right->last_token;
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);

            //Each of the operands shall have integer type.
            if (!type_is_integer(&new_expression->left->type))
            {
                compiler_diagnostic(C_ERROR_LEFT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "left type must be an integer type");
                throw;
            }

            if (!type_is_integer(&new_expression->right->type))
            {
                compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "right type must be an integer type");
                throw;
            }

            if (object_has_constant_value(&new_expression->left->object) &&
                object_has_constant_value(&new_expression->right->object))
            {
                char warning_message[200] = { 0 };
                new_expression->object = object_bitwise_xor(ctx->options.target,
                    &new_expression->left->object,
                    &new_expression->right->object, warning_message);
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



struct expression* _Owner _Opt inclusive_or_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
{
    /*
    inclusive-OR-expression:
        exclusive-OR-expression
        inclusive-OR-expression | exclusive-OR-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        p_expression_node = exclusive_or_expression(ctx, eval_mode);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '|'))
        {
            struct token* operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;


            new_expression->first_token = ctx->current;
            new_expression->expression_type = INCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            new_expression->right = exclusive_or_expression(ctx, eval_mode);
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
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);

            if (!type_is_integer(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(C_ERROR_LEFT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "left type must be an integer type");
                throw;
            }

            if (!type_is_integer(&new_expression->right->type))
            {
                compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_INTEGER, ctx, ctx->current, NULL, "right type must be an integer type");
                throw;
            }

            if (object_has_constant_value(&new_expression->left->object) &&
                object_has_constant_value(&new_expression->right->object))
            {
                char warning_message[200] = { 0 };
                new_expression->object = object_bitwise_or(ctx->options.target,
                &new_expression->left->object,
                &new_expression->right->object, warning_message);

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

struct expression* _Owner _Opt logical_and_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
{
    /*
    logical-AND-expression:
     inclusive-OR-expression
     logical-AND-expression && inclusive-OR-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        p_expression_node = inclusive_or_expression(ctx, eval_mode);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '&&'))
        {
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = LOGICAL_AND_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            bool right_evaluation_is_disabled = false;

            if (object_has_constant_value(&new_expression->left->object))
            {
                if (!object_is_true(&new_expression->left->object))
                {
                    right_evaluation_is_disabled = true;
                }
            }


            new_expression->right = inclusive_or_expression(ctx,
                right_evaluation_is_disabled ? EXPRESSION_EVAL_MODE_TYPE :
                                               EXPRESSION_EVAL_MODE_VALUE_AND_TYPE);


            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }
            new_expression->last_token = new_expression->right->last_token;

            if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE)
            {
                if (object_has_constant_value(&new_expression->left->object))
                {
                    bool a = object_is_true(&new_expression->left->object);
                    if (a == 0)
                    {
                        // 0 && something
                        new_expression->object = object_make_signed_int(ctx->options.target, 0);
                    }
                    else
                    {
                        // 1 && something
                        if (object_has_constant_value(&new_expression->right->object))
                        {
                            bool b = object_is_true(&new_expression->right->object);
                            new_expression->object = object_make_signed_int(ctx->options.target, a && b);
                        }
                    }
                }
            }

            //Each of the operands shall have scalar type
            if (!type_is_scalar(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(C_ERROR_LEFT_IS_NOT_SCALAR, ctx, ctx->current, NULL, "left type is not scalar for or expression");
                throw;
            }

            if (!type_is_scalar(&new_expression->right->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_SCALAR, ctx, ctx->current, NULL, "right type is not scalar for or expression");
                throw;
            }

            //The result has type int
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

struct expression* _Owner _Opt logical_or_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
{
    /*
      logical-OR-expression:
       logical-AND-expression
       logical-OR-expression || logical-AND-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {


        p_expression_node = logical_and_expression(ctx, eval_mode);
        if (p_expression_node == NULL)
            throw;

        enum expression_eval_mode current_eval_mode = eval_mode;

        while (ctx->current != NULL &&
               (ctx->current->type == '||'))
        {
            parser_match(ctx);
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;
            new_expression->expression_type = LOGICAL_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            bool right_evaluation_is_disabled = false;

            if (object_has_constant_value(&new_expression->left->object))
            {
                if (object_is_true(&new_expression->left->object))
                {
                    /*
                      If the first operand compares unequal to 0, the second operand is not evaluated.
                    */
                    right_evaluation_is_disabled = true;
                }
            }
            switch (current_eval_mode)
            {
            case EXPRESSION_EVAL_MODE_NONE:
            case EXPRESSION_EVAL_MODE_TYPE:
                break;
            case EXPRESSION_EVAL_MODE_VALUE_AND_TYPE:
                if (right_evaluation_is_disabled)
                {
                    current_eval_mode = EXPRESSION_EVAL_MODE_TYPE;
                }
                break;
            }

            new_expression->right = logical_and_expression(ctx, current_eval_mode);

            if (new_expression->right == NULL)
            {
                expression_delete(new_expression);
                throw;
            }

            new_expression->last_token = new_expression->right->last_token;

            if (eval_mode == EXPRESSION_EVAL_MODE_VALUE_AND_TYPE)
            {
                if (object_has_constant_value(&new_expression->left->object))
                {
                    bool a = object_is_true(&new_expression->left->object);
                    if (a == 1)
                    {
                        // 1 || something
                        new_expression->object = object_make_signed_int(ctx->options.target, 1);
                    }
                    else
                    {
                        // 0 || something
                        if (object_has_constant_value(&new_expression->right->object))
                        {
                            bool b = object_is_true(&new_expression->right->object);
                            new_expression->object = object_make_signed_int(ctx->options.target, a || b);
                        }
                    }
                }
            }


            //Each of the operands shall have scalar type
            if (!type_is_scalar(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(C_ERROR_LEFT_IS_NOT_SCALAR, ctx, ctx->current, NULL, "left type is not scalar for or expression");
                throw;
            }

            if (!type_is_scalar(&new_expression->right->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(C_ERROR_RIGHT_IS_NOT_SCALAR, ctx, ctx->current, NULL, "right type is not scalar for or expression");
                throw;
            }

            //The result has type int
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

struct expression* _Owner _Opt assignment_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        p_expression_node = conditional_expression(ctx, eval_mode);
        if (p_expression_node == NULL)
            throw;

        assert(p_expression_node->expression_type != EXPRESSION_TYPE_INVALID);

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
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct expression* _Owner _Opt new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL)
                throw;

            new_expression->first_token = ctx->current;

            switch (op_token->type)
            {
            case '=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_ASSIGN;
                break;
            case '*=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_MULTI_ASSIGN;
                break;
            case '/=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_DIV_ASSIGN;
                break;
            case '%=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_MOD_ASSIGN;
                break;
            case '+=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_PLUS_ASSIGN;
                break;
            case '-=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_MINUS_ASSIGN;
                break;
            case '<<=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_SHIFT_LEFT_ASSIGN;
                break;
            case '>>=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_SHIFT_RIGHT_ASSIGN;
                break;
            case '&=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_AND_ASSIGN;
                break;
            case '^=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_NOT_ASSIGN;
                break;
            case '|=':
                new_expression->expression_type = ASSIGNMENT_EXPRESSION_OR_ASSIGN;
                break;
            default:
                assert(false);
                break;
            }


            new_expression->left = p_expression_node;
            p_expression_node = NULL; // MOVED

            const struct marker left_operand_marker = {
                      .p_token_begin = new_expression->left->first_token,
                      .p_token_end = new_expression->left->last_token,
            };

            if (type_is_function(&new_expression->left->type))
            {
                compiler_diagnostic(C_ERROR_ASSIGNMENT_OF_FUNCTION,
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
                    compiler_diagnostic(W_ASSIGNMENT_OF_ARRAY_PARAMETER,
                        ctx,
                        NULL,
                        &left_operand_marker,
                        "assignment to array parameter");
                }
                else
                {
                    compiler_diagnostic(C_ERROR_ASSIGNMENT_TO_EXPRESSION_WITH_ARRAY_TYPE,
                        ctx,
                        NULL,
                        &left_operand_marker,
                        "assignment to expression with array type");
                }
            }

            if (type_is_const(&new_expression->left->type))
            {
                compiler_diagnostic(C_ERROR_ASSIGNMENT_OF_READ_ONLY_OBJECT,
                    ctx,
                    NULL,
                    &left_operand_marker,
                    "assignment of read-only object");
            }


            if (!expression_is_lvalue(new_expression->left))
            {
                compiler_diagnostic(C_ERROR_OPERATOR_NEEDS_LVALUE,
                                            ctx,
                                            NULL,
                                            &left_operand_marker,
                                            "lvalue required as left operand of assignment");
            }

            new_expression->right = assignment_expression(ctx, eval_mode);
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

void argument_expression_list_destroy(_Dtor struct argument_expression_list* p)
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

        //explodindo
        //object_destroy(&p->object);

        free(p);
    }
}
struct expression* _Owner _Opt expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
{
    /*expression:
      assignment-expression
      expression, assignment-expression
    */
    struct expression* _Owner _Opt p_expression_node = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_expression_node = assignment_expression(ctx, eval_mode);
        if (p_expression_node == NULL)
            throw;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == ',')
        {
            while (ctx->current->type == ',')
            {
                parser_match(ctx);
                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }

                struct expression* _Owner _Opt p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL)
                    throw;

                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = EXPRESSION_EXPRESSION;
                p_expression_node_new->left = p_expression_node;
                p_expression_node = NULL; /*MOVED*/

                p_expression_node_new->right = expression(ctx, eval_mode);
                if (p_expression_node_new->right == NULL)
                {
                    expression_delete(p_expression_node_new);
                    throw;
                }
                p_expression_node_new->object = object_dup(&p_expression_node_new->right->object);
                p_expression_node_new->left->last_token = p_expression_node_new->right->last_token;

                p_expression_node = p_expression_node_new;

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }
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
        return (object_has_constant_value(&expression->object) &&
                object_is_one(&expression->object));
    }
    return false;
}

bool expression_is_zero(const struct expression* expression)
{
    if (expression->expression_type == PRIMARY_EXPRESSION_NUMBER)
    {
        return (object_has_constant_value(&expression->object) &&
                object_is_zero(&expression->object));
    }
    return false;
}

bool expression_is_null_pointer_constant(const struct expression* expression)
{
    /*
      An integer constant expression with the value 0,
      such an expression cast to type void *, or the
      predefined constant nullptr is called a null pointer constant.57)
    */
    if (type_is_integer(&expression->type) &&
        object_has_constant_value(&expression->object) &&
        object_is_zero(&expression->object))
    {
        return true;
    }
    if (type_is_void_ptr(&expression->type) &&
        object_has_constant_value(&expression->object) &&
        object_is_zero(&expression->object))
    {
        return true;
    }

    if (type_is_nullptr_t(&expression->type))
    {
        return true;
    }

    return false;
}

struct expression* _Owner _Opt conditional_expression(struct parser_ctx* ctx, enum expression_eval_mode eval_mode)
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
        p_expression_node = logical_or_expression(ctx, eval_mode);
        if (p_expression_node == NULL)
            throw;

        enum expression_eval_mode current_eval_mode = eval_mode;

        if (ctx->current && ctx->current->type == '?')
        {
            struct expression* _Owner _Opt p_conditional_expression = calloc(1, sizeof(struct expression));
            if (p_conditional_expression == NULL) throw;


            p_conditional_expression->first_token = ctx->current;
            p_conditional_expression->expression_type = CONDITIONAL_EXPRESSION;
            p_conditional_expression->condition_expr = p_expression_node;
            p_expression_node = NULL; /*MOVED*/

            parser_match(ctx); //?
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                throw;
            }

            bool constant_expression_is_true = false;
            bool has_constant_expression = false;

            if (object_has_constant_value(&p_conditional_expression->condition_expr->object))
            {
                has_constant_expression = true;
                if (object_is_true(&p_conditional_expression->condition_expr->object))
                {
                    constant_expression_is_true = true;
                }
            }

            switch (current_eval_mode)
            {
            case EXPRESSION_EVAL_MODE_NONE:
            case EXPRESSION_EVAL_MODE_TYPE:
                break;
            case EXPRESSION_EVAL_MODE_VALUE_AND_TYPE:
                current_eval_mode = has_constant_expression && !constant_expression_is_true;
                break;
            }

            struct expression* _Owner _Opt p_left = expression(ctx, current_eval_mode);

            if (p_left == NULL)
            {
                expression_delete(p_conditional_expression);
                throw;
            }
            p_conditional_expression->left = p_left;


            if (parser_match_tk(ctx, TK_COLON) != 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                throw;
            }

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                throw;
            }

            enum expression_eval_mode new_eval_mode = EXPRESSION_EVAL_MODE_VALUE_AND_TYPE;
            switch (eval_mode)
            {
            case EXPRESSION_EVAL_MODE_NONE:
            case EXPRESSION_EVAL_MODE_TYPE:
                new_eval_mode = eval_mode;
                break;

            case EXPRESSION_EVAL_MODE_VALUE_AND_TYPE:
                new_eval_mode = eval_mode;
                if (has_constant_expression && constant_expression_is_true)
                {
                    new_eval_mode = EXPRESSION_EVAL_MODE_TYPE;
                }
                break;
            }

            struct expression* _Owner _Opt p_right = conditional_expression(ctx, new_eval_mode);

            if (p_right == NULL)
            {
                expression_delete(p_conditional_expression);
                throw;
            }
            p_conditional_expression->right = p_right;

            if (object_has_constant_value(&p_conditional_expression->condition_expr->object))
            {
                if (object_is_true(&p_conditional_expression->condition_expr->object))
                {
                    p_conditional_expression->object = object_make_reference(&p_conditional_expression->left->object);
                }
                else
                {
                    p_conditional_expression->object = object_make_reference(&p_conditional_expression->right->object);
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
                compiler_diagnostic(C_ERROR_CONDITION_MUST_HAVE_SCALAR_TYPE, ctx, ctx->current, NULL, "condition must have scalar type");
            }
            else if (type_is_arithmetic(&left_type) &&
                     type_is_arithmetic(&right_type))
            {
                /*
                 *  both operands have arithmetic type;
                */
                type_destroy(&p_conditional_expression->type);
                p_conditional_expression->type = type_common(&left_type, &right_type, ctx->options.target);
            }
            else if (type_is_struct_or_union(&left_type) && type_is_struct_or_union(&right_type))
            {
                /*
                 *  both operands have compatible structure or union type;
                 */
                if (!type_is_same(&left_type, &right_type, true))
                {
                    compiler_diagnostic(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types");
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
                        compiler_diagnostic(C_ERROR_INCOMPATIBLE_TYPES, ctx, ctx->current, NULL, "incompatible types");
                    }
                    else
                    {
                        type_swap(&p_conditional_expression->type, &right_type);
                    }
                }
                else
                {
                    compiler_diagnostic(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types");
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
                        compiler_diagnostic(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types");
                    }
                    else
                    {
                        type_swap(&p_conditional_expression->type, &right_type);
                    }
                }
                else
                {
                    compiler_diagnostic(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types");
                }
            }
            else
            {
                compiler_diagnostic(C_ERROR_INCOMPATIBLE_TYPES, ctx, p_conditional_expression->condition_expr->first_token, NULL, "incompatible types");
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

struct expression* _Owner _Opt constant_expression(struct parser_ctx* ctx, bool show_error_if_not_constant, enum expression_eval_mode eval_mode)
{
    struct expression* _Owner _Opt p_expression = conditional_expression(ctx, eval_mode);

    if (show_error_if_not_constant &&
        p_expression &&
        !object_has_constant_value(&p_expression->object))
    {
        compiler_diagnostic(C_ERROR_EXPECTED_CONSTANT_EXPRESSION, ctx, ctx->current, NULL, "expected constant expression");
    }

    return p_expression;
}

bool expression_get_variables(struct expression* expr, int n, struct object* variables[/*n*/])
{
    int count = 0;
    switch (expr->expression_type)
    {

    case EXPRESSION_TYPE_INVALID:  break;

    case PRIMARY_EXPRESSION_ENUMERATOR:  break;
    case PRIMARY_EXPRESSION_DECLARATOR:
        if (!object_has_constant_value(&expr->object))
        {
            if (count < n)
            {
                variables[count] = object_get_non_const_referenced(&expr->object);
                count++;
            }

        }
        break;

    case PRIMARY_EXPRESSION_STRING_LITERAL:  break;
    case PRIMARY_EXPRESSION__FUNC__:  break; /*predefined identifier __func__ */
    case PRIMARY_EXPRESSION_CHAR_LITERAL:  break;
    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:  break; /*true false*/
    case PRIMARY_EXPRESSION_GENERIC:  break;
    case PRIMARY_EXPRESSION_NUMBER:  break;

    case PRIMARY_EXPRESSION_PARENTESIS:
        assert(expr->right != NULL);
        count += expression_get_variables(expr->right, n, variables);
        break;

    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:  break;
    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:  break;

    case POSTFIX_FUNCTION_CALL:  break; // ( ) 
    case POSTFIX_ARRAY:  break; // [ ]
    case POSTFIX_DOT:  break; // .
    case POSTFIX_ARROW:  break; // .
    case POSTFIX_INCREMENT:  break;
    case POSTFIX_DECREMENT:  break;


    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:  break;
    case UNARY_EXPRESSION_SIZEOF_TYPE:  break;
    case UNARY_EXPRESSION_COUNTOF:  break;

    case UNARY_EXPRESSION_TRAITS:  break;
    case UNARY_EXPRESSION_IS_SAME:  break;
    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:  break;

    case UNARY_EXPRESSION_ALIGNOF_TYPE:  break;
    case UNARY_EXPRESSION_ALIGNOF_EXPRESSION:  break;

    case UNARY_EXPRESSION_ASSERT:  break;

    case UNARY_EXPRESSION_INCREMENT:  break;
    case UNARY_EXPRESSION_DECREMENT:  break;

    case UNARY_EXPRESSION_NOT:  break;
    case UNARY_EXPRESSION_BITNOT:  break;
    case UNARY_EXPRESSION_NEG:  break;
    case UNARY_EXPRESSION_PLUS:  break;
    case UNARY_EXPRESSION_CONTENT:  break;
    case UNARY_EXPRESSION_ADDRESSOF:  break;

    case CAST_EXPRESSION:  break;

    case MULTIPLICATIVE_EXPRESSION_MULT:
    case MULTIPLICATIVE_EXPRESSION_DIV:
    case MULTIPLICATIVE_EXPRESSION_MOD:
        assert(expr->left != NULL);
        assert(expr->right != NULL);
        count += expression_get_variables(expr->left, n, variables);
        count += expression_get_variables(expr->right, n, variables);
        break;

    case ADDITIVE_EXPRESSION_PLUS:
    case ADDITIVE_EXPRESSION_MINUS:
        assert(expr->left != NULL);
        assert(expr->right != NULL);

        count += expression_get_variables(expr->left, n, variables);
        count += expression_get_variables(expr->right, n, variables);
        break;


    case SHIFT_EXPRESSION_RIGHT:
    case SHIFT_EXPRESSION_LEFT:

    case RELATIONAL_EXPRESSION_BIGGER_THAN:
    case RELATIONAL_EXPRESSION_LESS_THAN:
    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
    case EQUALITY_EXPRESSION_EQUAL:
    case EQUALITY_EXPRESSION_NOT_EQUAL:
        assert(expr->left != NULL);
        assert(expr->right != NULL);
        count += expression_get_variables(expr->left, n, variables);
        count += expression_get_variables(expr->right, n, variables);
        break;

    case AND_EXPRESSION:  break;
    case EXCLUSIVE_OR_EXPRESSION:  break;
    case INCLUSIVE_OR_EXPRESSION:  break;

    case LOGICAL_OR_EXPRESSION:
    case LOGICAL_AND_EXPRESSION:
        assert(expr->left != NULL);
        assert(expr->right != NULL);
        count += expression_get_variables(expr->left, n, variables);
        count += expression_get_variables(expr->right, n, variables);
        break; //&&

    case ASSIGNMENT_EXPRESSION_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_PLUS_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_MINUS_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_MULTI_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_DIV_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_MOD_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_SHIFT_LEFT_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_SHIFT_RIGHT_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_AND_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_OR_ASSIGN:  break;
    case ASSIGNMENT_EXPRESSION_NOT_ASSIGN:  break;


    case EXPRESSION_EXPRESSION:  break;

    case CONDITIONAL_EXPRESSION:  break;

    case UNARY_EXPRESSION_GCC__BUILTIN_VA_START:
    case UNARY_EXPRESSION_GCC__BUILTIN_VA_END:
    case UNARY_EXPRESSION_GCC__BUILTIN_VA_COPY:
    case UNARY_EXPRESSION_GCC__BUILTIN_VA_ARG:
    case UNARY_EXPRESSION_GCC__BUILTIN_OFFSETOF:
    case UNARY_EXPRESSION_CONSTEVAL:
        break;
    }

    return count;
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



void check_assigment(struct parser_ctx* ctx,
    const struct type* p_a_type, /*this is not expression because function parameters*/
    const struct expression* p_b_expression,
    enum assigment_type assignment_type)
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
                    compiler_diagnostic(W_STYLE, ctx, p_b_expression->first_token, NULL, "use NULL instead of 0");
                }
                else
                {
                    //Everything else is unusual
                    // p = false;
                    // p = 1-1;
                    // p = '\0';
                    compiler_diagnostic(W_UNSUAL_NULL_POINTER_CONSTANT, ctx, p_b_expression->first_token, NULL, "unusual expression/type used as null pointer constant");
                }
            }
            else
            {
                compiler_diagnostic(C_ERROR_INT_TO_POINTER, ctx, p_b_expression->first_token, NULL, "non-pointer to pointer");
            }
        }
    }

    if (type_is_bool(p_a_type) && type_is_nullptr_t(p_b_type))
    {
        struct marker marker = {
        .p_token_begin = p_b_expression->first_token,
        .p_token_end = p_b_expression->first_token
        };

        compiler_diagnostic(W_NULL_CONVERTION,
        ctx,
        NULL,
        &marker,
        "implicit conversion of nullptr constant to 'bool'");
    }

    struct type b_type_lvalue = { 0 };

    if (expression_is_subjected_to_lvalue_conversion(p_b_expression))
    {
        b_type_lvalue = type_lvalue_conversion(p_b_type, ctx->options.null_checks_enabled);
    }
    else
    {
        b_type_lvalue = type_dup(p_b_type);
    }


    if (type_is_owner(p_a_type) && !type_is_owner(&p_b_expression->type))
    {
        if (!is_null_pointer_constant)
        {
            compiler_diagnostic(W_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN, ctx, p_b_expression->first_token, NULL, "cannot assign a non-owner to owner");
            type_destroy(&b_type_lvalue);
            //type_destroy(&t2);
            return;
        }
    }

    if (!type_is_owner(p_a_type) && type_is_owner_or_pointer_to_dtor(&p_b_expression->type))
    {
        if (p_b_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
        {
            compiler_diagnostic(W_OWNERSHIP_USING_TEMPORARY_OWNER,
                ctx,
                p_b_expression->first_token, NULL,
                "cannot assign a temporary owner to non-owner object.");
            type_destroy(&b_type_lvalue);
            //type_destroy(&t2);
            return;
        }
    }

    if (assignment_type == ASSIGMENT_TYPE_RETURN)
    {
        if (!type_is_owner(p_a_type) && type_is_owner_or_pointer_to_dtor(&p_b_expression->type))
        {
            if (is_automatic_variable(p_b_expression->type.storage_class_specifier_flags))
            {
                compiler_diagnostic(C_ERROR_RETURN_LOCAL_OWNER_TO_NON_OWNER,
                    ctx,
                    p_b_expression->first_token, NULL,
                    "cannot return a automatic storage duration _Owner to non-owner");
                type_destroy(&b_type_lvalue);
                // type_destroy(&t2);
                return;
            }
        }
    }
#if 1
    /*
    TODO REMOVE THiS
    #pragma safety enable

      #include <stdlib.h>

      struct X {
       struct Y * _Owner p;
      };

      void x_destroy(_Dtor struct X * p);

      void f(struct X * x) {
        x_destroy(x);
        static_debug(*x);
      }
    */
    if (type_is_pointed_dtor(p_a_type) && type_is_pointer(p_a_type))
    {
        if (type_is_owner(p_b_type))
        {
        }
        else if (!p_b_type->address_of)
        {
            compiler_diagnostic(W_MUST_USE_ADDRESSOF,
                       ctx,
                       p_b_expression->first_token, NULL,
                       "source expression of _Dtor must be addressof");
        }
    }
#endif


    if (type_is_pointer(p_a_type) &&
        !type_is_opt(p_a_type, ctx->options.null_checks_enabled) &&
        is_null_pointer_constant)
    {

        compiler_diagnostic(W_NULLABLE_TO_NON_NULLABLE,
            ctx,
            p_b_expression->first_token, NULL,
            "cannot convert a null pointer constant to non-nullable pointer");

        type_destroy(&b_type_lvalue);
        //type_destroy(&t2);

        return;

    }



    /*
       less generic tests are first
    */
    if (type_is_enum(p_b_type) && type_is_enum(p_a_type))
    {
        if (!type_is_same(p_b_type, p_a_type, false))
        {
            compiler_diagnostic(W_INCOMPATIBLE_ENUN_TYPES, ctx,
                p_b_expression->first_token, NULL,
                " incompatible types ");
        }


        type_destroy(&b_type_lvalue);
        // type_destroy(&t2);
        return;
    }

    if (type_is_arithmetic(p_b_type) && type_is_arithmetic(p_a_type))
    {

        type_destroy(&b_type_lvalue);
        //type_destroy(&t2);
        return;
    }

    if (is_null_pointer_constant && type_is_pointer(p_a_type))
    {
        //TODO void F(int * [[_Opt]] p)
        // F(0) when passing null we will check if the parameter
        //have the anotation [[_Opt]]

        /*can be converted to any type*/

        type_destroy(&b_type_lvalue);
        //type_destroy(&t2);
        return;
    }

    if (is_null_pointer_constant && type_is_array(p_a_type))
    {
        compiler_diagnostic(W_PASSING_NULL_AS_ARRAY,
            ctx,
            p_b_expression->first_token, NULL,
            " passing null as array");


        type_destroy(&b_type_lvalue);
        //type_destroy(&t2);
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

            type_destroy(&b_type_lvalue);
            //type_destroy(&t2);
            return;
        }

        if (type_is_void_ptr(p_a_type))
        {
            /*any pointer can be converted to void* */

            type_destroy(&b_type_lvalue);
            // type_destroy(&t2);
            return;
        }


        //TODO  lvalue

        struct type a_type_lvalue = { 0 };

        if (type_is_array(p_a_type))
        {
            if (assignment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                size_t parameter_array_size = p_a_type->num_of_elements;
                if (type_is_array(p_b_type))
                {
                    size_t argument_array_size = p_b_type->num_of_elements;
                    if (parameter_array_size != 0 &&
                        argument_array_size < parameter_array_size)
                    {
                        compiler_diagnostic(C_ERROR_ARGUMENT_SIZE_SMALLER_THAN_PARAMETER_SIZE, ctx,
                            p_b_expression->first_token, NULL,
                            " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                    }
                }
                else if (is_null_pointer_constant || type_is_nullptr_t(p_b_type))
                {
                    compiler_diagnostic(W_PASSING_NULL_AS_ARRAY, ctx,
                        p_b_expression->first_token, NULL,
                        " passing null as array");
                }
            }
            a_type_lvalue = type_lvalue_conversion(p_a_type, ctx->options.null_checks_enabled);
        }
        else
        {
            a_type_lvalue = type_dup(p_a_type);
        }



        if (!type_is_same(&b_type_lvalue, &a_type_lvalue, false))
        {
            type_print(&b_type_lvalue, ctx->options.target);
            type_print(&a_type_lvalue, ctx->options.target);

            compiler_diagnostic(W_ERROR_INCOMPATIBLE_TYPES, ctx,
                p_b_expression->first_token, NULL,
                " incompatible types");
        }


        if (assignment_type == ASSIGMENT_TYPE_PARAMETER)
        {
            if (type_is_pointer(&b_type_lvalue) && type_is_pointer(&a_type_lvalue))
            {
                //parameter pointer do non const
                //argument const.
                struct type b_pointed_type_lvalue = type_remove_pointer(&b_type_lvalue);
                struct type a_lvalue_pointed_type = type_remove_pointer(&a_type_lvalue);
                if (type_is_const(&b_pointed_type_lvalue) && !type_is_const(&a_lvalue_pointed_type))
                {
                    compiler_diagnostic(W_DISCARDED_QUALIFIERS, ctx,
                        p_b_expression->first_token, NULL,
                        " discarding const at argument ");
                }
                type_destroy(&b_pointed_type_lvalue);
                type_destroy(&a_lvalue_pointed_type);
            }
        }
        else
        {
            if (type_is_pointer(p_a_type) && type_is_pointer(&b_type_lvalue))
            {
                struct type b_pointed_type = type_remove_pointer(&b_type_lvalue);
                struct type a_pointed_type = type_remove_pointer(p_a_type);
                if (type_is_const(&b_pointed_type) && !type_is_const(&a_pointed_type))
                {
                    compiler_diagnostic(W_DISCARDED_QUALIFIERS, ctx,
                        p_b_expression->first_token, NULL,
                        " discarding const");
                }
                type_destroy(&b_pointed_type);
                type_destroy(&a_pointed_type);
            }
        }

        //return true;
        type_destroy(&a_type_lvalue);
    }

    if (!type_is_same(p_a_type, &b_type_lvalue, false))
    {
        // compiler_diagnostic(C_ERROR_INCOMPATIBLE_TYPES,
        //     ctx,
        //       p_b_expression->first_token, 
        //       NULL,
        //       " incompatible types ");
    }


    type_destroy(&b_type_lvalue);


}
