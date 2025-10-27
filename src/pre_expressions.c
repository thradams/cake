/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

/*
  For performance reasons we will separate expression from preprocessor from compiler.
*/

#include "ownership.h"
#include <stdlib.h>
#include "tokenizer.h"
#include "pre_expressions.h"
#include <locale.h>
#include <assert.h>
#include <limits.h>
#include "object.h"
#include <errno.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif

/*context expressions preprocessor*/
struct pre_expression_ctx
{
    /*all preprocessor expressions are calculated with long long*/
    long long value;
};

static void pre_postfix_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_cast_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_multiplicative_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_unary_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_additive_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_shift_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_relational_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_equality_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_and_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_exclusive_or_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_inclusive_or_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_logical_and_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_logical_or_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_conditional_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);
static void pre_conditional_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx);

//TODO share this with parser!
/*
 * preprocessor uses long long
 */
static int ppnumber_to_longlong(struct preprocessor_ctx* ctx, struct token* token, long long* result, enum target target)
{

    //const long long signed_int_max_value = 
      //  target_signed_max(ctx->options.target, TYPE_SIGNED_INT);

    const long long signed_long_max_value =
        target_signed_max(ctx->options.target, TYPE_SIGNED_LONG);

    const unsigned long long unsigned_long_max_value =
        target_unsigned_max(ctx->options.target, TYPE_UNSIGNED_LONG);

    const long long signed_long_long_max_value =
        target_signed_max(ctx->options.target, TYPE_SIGNED_LONG_LONG);

    //const unsigned long long unsigned_long_long_max_value = 
      //  target_unsigned_max(ctx->options.target, TYPE_UNSIGNED_LONG_LONG);

    /*copy removing the separators*/
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

    char errormsg[100];
    char suffix[4] = { 0 };
    const enum token_type type = parse_number(token->lexeme, suffix, errormsg);
    if (type == TK_NONE)
    {
        preprocessor_diagnostic(
            C_INVALID_TOKEN,
            ctx,
            token,
            "%s",
            errormsg);
        return 0;
    }
    struct object  cv = { 0 };
    switch (type)
    {
    case TK_COMPILER_DECIMAL_CONSTANT:
    case TK_COMPILER_OCTAL_CONSTANT:
    case TK_COMPILER_HEXADECIMAL_CONSTANT:
    case TK_COMPILER_BINARY_CONSTANT:
    {
        unsigned long long value = 0;
        switch (type)
        {
        case TK_COMPILER_DECIMAL_CONSTANT:
            value = strtoull(buffer, NULL, 10);
            break;
        case TK_COMPILER_OCTAL_CONSTANT:
            value = strtoull(buffer + 1, NULL, 8);
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

        if (value == ULLONG_MAX && errno == ERANGE)
        {
            //compiler_diagnostic(
            //C_ERROR_LITERAL_OVERFLOW,
            //ctx,
            //token,
            //NULL,
            //"integer literal is too large to be represented in any integer type");
        }

        if (suffix[0] == 'U')
        {
            /*fixing the type that fits the size*/
            if (value <= UINT_MAX && suffix[1] != 'L')
            {
                cv = object_make_unsigned_int(ctx->options.target, (unsigned int)value);

            }
            else if (value <= unsigned_long_max_value && suffix[2] != 'L')
            {
                cv = object_make_unsigned_long(target, (unsigned long)value);
            }
            else //if (value <= ULLONG_MAX)
            {
                cv = object_make_unsigned_long_long(ctx->options.target, (unsigned long long)value);
            }
        }
        else
        {
            /*fixing the type that fits the size*/
            if (value <= INT_MAX && suffix[0] != 'L')
            {
                cv = object_make_signed_int(ctx->options.target, (int)value);
            }
            else if (value <= (unsigned long long) signed_long_max_value && suffix[1] != 'L' /*!= LL*/)
            {
                cv = object_make_signed_long(target, (long)value);
            }
            else if (value <= (unsigned long long) signed_long_long_max_value)
            {
                cv = object_make_signed_long_long(ctx->options.target, (long long)value);
            }
            else
            {
                cv = object_make_signed_long_long(ctx->options.target, value);
            }
        }

    }
    break;

    case TK_COMPILER_DECIMAL_FLOATING_CONSTANT:
    case TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT:
        //error
        break;

    default:
        assert(false);
    }

    *result = object_to_signed_long_long(&cv);

    return 0;
}

/*
  ctx->current and pre_match are used only in preprocessor constant expressions
  (the preprocessor itself uses concept of removing from one list and adding
  into another so the head of the input list is the current.
  We could use the same concept here removing current.
*/
static struct token* _Opt pre_match(struct preprocessor_ctx* ctx)
{
    if (ctx->current == NULL)
        return NULL;

    ctx->current = ctx->current->next;

    while (ctx->current && token_is_blank(ctx->current))
    {
        ctx->current = ctx->current->next;
    }

    return ctx->current;
}

//TODO share this with parser
static struct object char_constant_to_value(const char* s, char error_message[/*sz*/], int error_message_sz_bytes, enum target target)
{
    error_message[0] = '\0';

    const unsigned char* _Opt p = (const unsigned char*)s;
    const unsigned long long
        wchar_max_value = target_unsigned_max(target, get_platform(target)->wchar_t_type);


    try
    {
        if (p[0] == 'u' && p[1] == '8')
        {
            p++;
            p++;
            p++;

            // A UTF-8 character constant has type char8_t.

            unsigned int c = 0;
            p = utf8_decode(p, &c);
            if (p == NULL)
            {
                throw;
            }

            if (c == '\\')
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == NULL)
                {
                    throw;
                }
            }

            if (*p != '\'')
            {
                snprintf(error_message, error_message_sz_bytes, "Unicode character literals may not contain multiple characters.");
            }

            if (c > 0x80)
            {
                snprintf(error_message, error_message_sz_bytes, "Character too large for enclosing character literal type.");
            }

            return object_make_wchar_t(target, c);//, ctx->evaluation_is_disabled);
        }
        else if (p[0] == 'u')
        {
            p++;
            p++;

            // A UTF-16 character constant has type char16_t which is an unsigned integer types defined in the <uchar.h> header

            unsigned int c = 0;
            p = utf8_decode(p, &c);
            if (p == NULL)
            {
                throw;
            }

            if (c == '\\')
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == NULL)
                {
                    throw;
                }
            }

            if (*p != '\'')
            {
                snprintf(error_message, error_message_sz_bytes, "Unicode character literals may not contain multiple characters.");
            }

            if ((int)c > wchar_max_value)
            {
                snprintf(error_message, error_message_sz_bytes, "Character too large for enclosing character literal type.");
            }

            return object_make_wchar_t(target, c);
        }
        else if (p[0] == 'U')
        {
            p++;
            p++;

            // A UTF-16 character constant has type char16_t which is an unsigned integer types defined in the <uchar.h> header

            unsigned int c = 0;
            p = utf8_decode(p, &c);
            if (p == NULL)
            {
                throw;
            }

            if (c == '\\')
            {
                p = escape_sequences_decode_opt(p, &c);

                if (p == NULL)
                {
                    throw;
                }
            }

            if (*p != '\'')
            {
                snprintf(error_message, error_message_sz_bytes, "Unicode character literals may not contain multiple characters.");
            }

            if (c > UINT_MAX)
            {
                snprintf(error_message, error_message_sz_bytes, "Character too large for enclosing character literal type.");
            }

            return object_make_wchar_t(target, c);
        }
        else if (p[0] == 'L')
        {
            // A wchar_t character constant is prefixed by the letter L
            p++;
            p++;

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
                    if (p == NULL)
                        throw;
                }

                // TODO \u
                value = value * 256 + c;

                if (value > (long long)wchar_max_value)
                {
                    snprintf(error_message, error_message_sz_bytes, "character constant too long for its type");
                    break;
                }
            }

            return object_make_wchar_t(target, (int)value);
        }
        else
        {
            p++;

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
                    if (p == NULL)
                        throw;
                }


                if (c < 0x80)
                {
                    value = value * 256 + c;
                }
                else
                {
                    value = c;
                }
                if (value > INT_MAX)
                {
                    snprintf(error_message, error_message_sz_bytes, "character constant too long for its type");
                    break;
                }
            }
            return object_make_wchar_t(target, (int)value);
        }
    }
    catch
    {
    }

    struct object empty = { 0 };
    return empty;
}

static void pre_primary_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
     primary-expression:
      identifier
      constant
      string-literal
      ( expression )
      generic-selection
    */
    try
    {
        if (ctx->current == NULL)
        {
            pre_unexpected_end_of_file(ctx->input_list.tail, ctx);
            throw;
        }

        if (ctx->current->type == TK_CHAR_CONSTANT)
        {
            const char* p = ctx->current->lexeme + 1;
            char errmsg[200] = { 0 };
            struct object v = char_constant_to_value(p, errmsg, sizeof errmsg, ctx->options.target);
            if (errmsg[0] != '\0')
            {
                preprocessor_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, "%s", errmsg);
            }
            ectx->value = object_to_signed_long_long(&v);
            pre_match(ctx);
            object_destroy(&v);
        }
        else if (ctx->current->type == TK_PPNUMBER)
        {
            ppnumber_to_longlong(ctx, ctx->current, &ectx->value, ctx->options.target);
            pre_match(ctx);
        }
        else if (ctx->current->type == '(')
        {
            pre_match(ctx);
            pre_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;
            if (ctx->current && ctx->current->type != ')')
            {
                preprocessor_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, "expected )");
                throw;
            }
            pre_match(ctx);
        }
        else
        {
            preprocessor_diagnostic(C_ERROR_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS,
                                              ctx,
                                              ctx->current,
                                              "token '%s' is not valid in preprocessor expressions",
                                              ctx->current->lexeme);
            throw;
        }
    }
    catch
    {
    }
}

static void pre_postfix_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
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
    try
    {
        pre_primary_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;
    }
    catch
    {
    }
}

static void pre_unary_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
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
    try
    {
        if (ctx->current && (ctx->current->type == '++' || ctx->current->type == '--'))
        {
            preprocessor_diagnostic(C_ERROR_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS,
                                              ctx,
                                              ctx->current,
                                              "token '%s' is not valid in preprocessor expressions",
                                              ctx->current->lexeme);
            throw;
        }
        else if (ctx->current != NULL &&
                 (ctx->current->type == '&' || ctx->current->type == '*' || ctx->current->type == '+' || ctx->current->type == '-' || ctx->current->type == '~' || ctx->current->type == '!'))
        {
            const struct token* const p_old = ctx->current;
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            pre_cast_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;

            if (op == '!')
                ectx->value = !ectx->value;
            else if (op == '~')
                ectx->value = ~ectx->value;
            else if (op == '-')
                ectx->value = -ectx->value;
            else if (op == '+')
                ectx->value = +ectx->value;
            else if (op == '*')
            {
                preprocessor_diagnostic(C_ERROR_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS, ctx, p_old, "token '%s' is not valid in preprocessor expressions", p_old->lexeme);
            }
            else if (op == '&')
            {
                preprocessor_diagnostic(C_ERROR_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS, ctx, p_old, "token '%s' is not valid in preprocessor expressions", p_old->lexeme);
            }
            else
            {
                preprocessor_diagnostic(C_ERROR_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS, ctx, p_old, "token '%s' is not valid in preprocessor expressions", p_old->lexeme);
            }
        }
        else
        {
            pre_postfix_expression(ctx, ectx);
        }
    }
    catch
    {
    }
}

static void pre_cast_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
     cast-expression:
      unary-expression
      ( type-name ) cast-expression
    */
    pre_unary_expression(ctx, ectx);
}

static void pre_multiplicative_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
     multiplicative-expression:
    cast-expression
    multiplicative-expression * cast-expression
    multiplicative-expression / cast-expression
    multiplicative-expression % cast-expression
    */
    try
    {
        pre_cast_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '*' ||
                   ctx->current->type == '/' ||
                   ctx->current->type == '%'))
        {
            struct token* op_token = ctx->current;
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_cast_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;

            if (op == '*')
            {
                ectx->value = (left_value * ectx->value);
            }
            else if (op == '/')
            {
                if (ectx->value == 0)
                {
                    preprocessor_diagnostic(C_PRE_DIVISION_BY_ZERO, ctx, op_token, "division by zero");
                    throw;
                }
                else
                {
                    ectx->value = (left_value / ectx->value);
                }
            }
            else if (op == '%')
            {
                ectx->value = (left_value % ectx->value);
            }
        }
    }
    catch
    {
    }
}

static void pre_additive_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
     additive-expression:
       multiplicative-expression
       additive-expression + multiplicative-expression
       additive-expression - multiplicative-expression
    */
    try
    {
        pre_multiplicative_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '+' ||
                   ctx->current->type == '-'))
        {
            const struct token* p_op_token = ctx->current;
            pre_match(ctx);
            if (ctx->current == NULL)
            {
                pre_unexpected_end_of_file(ctx->input_list.tail, ctx);
                throw;
            }
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;

            if (p_op_token->type == '+')
            {
                ectx->value = left_value + ectx->value;
            }
            else if (p_op_token->type == '-')
            {
                ectx->value = left_value - ectx->value;
            }
            else
            {
                throw;
            }
        }
    }
    catch
    {
    }
}

static void pre_shift_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
     shift-expression:
       additive-expression
       shift-expression << additive-expression
       shift-expression >> additive-expression
    */
    try
    {
        pre_additive_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '>>' ||
                   ctx->current->type == '<<'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;

            if (op == '>>')
            {
                ectx->value = left_value >> ectx->value;
            }
            else if (op == '<<')
            {
                ectx->value = left_value << ectx->value;
            }
        }
    }
    catch
    {
    }
}

static void pre_relational_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
    relational-expression:
      shift-expression
      relational-expression < shift-expression
      relational-expression > shift-expression
      relational-expression <= shift-expression
      relational-expression >= shift-expression
    */
    try
    {
        pre_shift_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '>' ||
                   ctx->current->type == '<' ||
                   ctx->current->type == '>=' ||
                   ctx->current->type == '<='))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_shift_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;

            if (op == '>')
            {
                ectx->value = left_value > ectx->value;
            }
            else if (op == '<')
            {
                ectx->value = left_value < ectx->value;
            }
            else if (op == '>=')
            {
                ectx->value = left_value >= ectx->value;
            }
            else if (op == '<=')
            {
                ectx->value = left_value <= ectx->value;
            }
        }
    }
    catch
    {
    }
}

static void pre_equality_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
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
    try
    {
        pre_relational_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '==' ||
                   ctx->current->type == '!='))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;

            if (op == '==')
            {
                ectx->value = left_value == ectx->value;
            }
            else if (op == '!=')
            {
                ectx->value = left_value != ectx->value;
            }
        }
    }
    catch
    {
    }
}

static void pre_and_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
     AND-expression:
      equality-expression
      AND-expression & equality-expression
    */
    try
    {
        pre_equality_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;
        while (ctx->current != NULL &&
               (ctx->current->type == '&'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_equality_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;
            ectx->value = left_value & ectx->value;
        }
    }
    catch
    {
    }
}

static void pre_exclusive_or_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
     exclusive-OR-expression:
      AND-expression
     exclusive-OR-expression ^ AND-expression
    */
    try
    {
        pre_and_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '^'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_and_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;
            ectx->value = left_value ^ ectx->value;
        }
    }
    catch
    {
    }
}

static void pre_inclusive_or_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
    inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */
    try
    {
        pre_exclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '|'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_exclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;

            ectx->value = left_value | ectx->value;
        }
    }
    catch
    {
    }
}

static void pre_logical_and_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
    logical-AND-expression:
     inclusive-OR-expression
     logical-AND-expression && inclusive-OR-expression
    */
    try
    {
        pre_inclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '&&'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_inclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;

            ectx->value = left_value && ectx->value;
        }
    }
    catch
    {
    }
}

static void pre_logical_or_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
      logical-OR-expression:
       logical-AND-expression
       logical-OR-expression || logical-AND-expression
    */
    try
    {
        pre_logical_and_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '||'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_logical_and_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;

            ectx->value = left_value || ectx->value;
        }
    }
    catch
    {
    }
}

static void pre_assignment_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
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
       // here it is doubtful but conditional does unary too. 
       // the difference is that it is not just any expression 
       // which can be an assignment
    try
    {
        pre_conditional_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current != NULL &&
               (ctx->current->type == '=' ||
                   ctx->current->type == '*=' ||
                   ctx->current->type == '/=' ||
                   ctx->current->type == '+=' ||
                   ctx->current->type == '-=' ||
                   ctx->current->type == '<<=' ||
                   ctx->current->type == '>>=' ||
                   ctx->current->type == '&=' ||
                   ctx->current->type == '^=' ||
                   ctx->current->type == '|='))
        {
            preprocessor_diagnostic(C_ERROR_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            throw;
        }
    }
    catch
    {
    }
}

static void pre_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*expression:
      assignment-expression
      expression, assignment-expression
    */
    try
    {
        pre_assignment_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        while (ctx->current && ctx->current->type == ',')
        {
            pre_match(ctx);
            pre_expression(ctx, ectx);
            if (ctx->n_errors > 0)
                throw;
        }
    }
    catch
    {
    }
}

static void pre_conditional_expression(struct preprocessor_ctx* ctx, struct pre_expression_ctx* ectx)
{
    /*
      conditional-expression:
      logical-OR-expression
      logical-OR-expression ? expression : conditional-expression
    */
    try
    {
        pre_logical_or_expression(ctx, ectx);
        if (ctx->n_errors > 0)
            throw;

        if (ctx->current && ctx->current->type == '?')
        {
            pre_match(ctx);
            if (ectx->value)
            {
                pre_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                    throw;

                pre_match(ctx); //:
                struct pre_expression_ctx temp = { 0 };
                pre_conditional_expression(ctx, &temp);
                if (ctx->n_errors > 0)
                    throw;
            }
            else
            {
                struct pre_expression_ctx temp = { 0 };
                pre_expression(ctx, &temp);
                if (ctx->n_errors > 0)
                    throw;

                pre_match(ctx); //:
                pre_conditional_expression(ctx, ectx);
                if (ctx->n_errors > 0)
                    throw;
            }
        }
    }
    catch
    {
    }
}

int pre_constant_expression(struct preprocessor_ctx* ctx, long long* pvalue)
{
    struct pre_expression_ctx ectx = { 0 };
    pre_conditional_expression(ctx, &ectx);
    *pvalue = ectx.value;
    return ctx->n_errors > 0;
}

