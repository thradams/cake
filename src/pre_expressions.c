/*
  For performance reasons we will separate expression from preprocessor from compiler.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "tokenizer.h"
#include "pre_expressions.h"
#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif


/*contexto expressoes preprocessador*/
struct pre_expression_ctx
{
    /*todas expressões do preprocessador sao calculadas com long long*/
    long long value;
};


static void pre_postfix_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_cast_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_multiplicative_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_unary_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_additive_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_shift_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_relational_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_equality_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_and_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_exclusive_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_inclusive_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_logical_and_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_logical_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_conditional_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
static void pre_conditional_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);



/*
* preprocessor uses long long
*/
static int ppnumber_to_longlong(struct token* token, long long* result)
{

    /*copia removendo os separadores*/
    //um dos maiores buffer necessarios seria 128 bits binario...
    //0xb1'1'1.... 
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

    if (buffer[0] == '0' &&
        buffer[1] == 'x')
    {
        //hex
        *result = strtoll(buffer + 2, 0, 16);
    }
    else if (buffer[0] == '0' &&
        buffer[1] == 'b')
    {
        //binario
        *result = strtoll(buffer + 2, 0, 2);
    }
    else if (buffer[0] == '0')
    {
        //octal
        *result = strtoll(buffer, 0, 8);
    }
    else
    {
        //decimal
        *result = strtoll(buffer, 0, 10);
    }

    
    return 0;
}

/*
  ctx->current and pre_match are used only in preprocessor constant expressions
  (the preprocessor itself uses concept of removing from one list and adding
  into another so the head of the input list is the current.
  We could use the same concept here removing current.
*/
static struct token* pre_match(struct preprocessor_ctx* ctx)
{
    ctx->current = ctx->current->next;

    while (ctx->current && token_is_blank(ctx->current))
    {
        ctx->current = ctx->current->next;
    }

    return ctx->current;
}

static void pre_primary_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        if (ctx->current->type == TK_CHAR_CONSTANT)
        {
            const char* p = ctx->current->lexeme + 1;
            ectx->value = 0;
            int count = 0;
            while (*p != '\'')
            {
                ectx->value = ectx->value * 256 + *p;
                p++;
                count++;
                if (count > 4)
                {
                    preprocessor_set_warning_with_token(W_NONE, ctx, ctx->current, "character constant too long for its type");
                }
            }

            pre_match(ctx);
        }
        else if (ctx->current->type == TK_PPNUMBER)
        {
            ppnumber_to_longlong(ctx->current, &ectx->value);
            pre_match(ctx);
        }
        else if (ctx->current->type == '(')
        {
            pre_match(ctx);
            pre_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;
            if (ctx->current && ctx->current->type != ')')
            {
                preprocessor_set_error_with_token(C_UNEXPECTED, ctx, ctx->current, "expected )");
                throw;
            }
            pre_match(ctx);
        }
        else
        {
            preprocessor_set_error_with_token(C_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS,
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




static void pre_postfix_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        pre_primary_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;
    }
    catch
    {
    }
}


static void pre_unary_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        if (ctx->current->type == '++' || ctx->current->type == '--')
        {
            preprocessor_set_error_with_token(C_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS,
                ctx, 
                ctx->current, 
                "token '%s' is not valid in preprocessor expressions",
                ctx->current->lexeme);
            throw;
        }
        else if (ctx->current != NULL &&
            (ctx->current->type == '&'
                || ctx->current->type == '*'
                || ctx->current->type == '+'
                || ctx->current->type == '-'
                || ctx->current->type == '~'
                || ctx->current->type == '!'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            pre_cast_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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
                preprocessor_set_error_with_token(C_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            }
            else if (op == '&')
            {
                preprocessor_set_error_with_token(C_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            }
            else
            {
                preprocessor_set_error_with_token(C_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            }
        }
        else 
        {
            pre_postfix_expression(ctx,  ectx);
        }
    }
    catch
    {
    }
}

static void pre_cast_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     cast-expression:
      unary-expression
      ( type-name ) cast-expression
    */
    pre_unary_expression(ctx,  ectx);    
}

static void pre_multiplicative_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        pre_cast_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '*' ||
                ctx->current->type == '/' ||
                ctx->current->type == '%'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_cast_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

            if (op == '*')
            {
                ectx->value = (left_value * ectx->value);
            }
            else if (op == '/')
            {
                ectx->value = (left_value / ectx->value);
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

static void pre_additive_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     additive-expression:
       multiplicative-expression
       additive-expression + multiplicative-expression
       additive-expression - multiplicative-expression
    */
    try
    {
        pre_multiplicative_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '+' ||
                ctx->current->type == '-'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            if (ctx->current == NULL)
            {
                preprocessor_set_error_with_token(C_UNEXPECTED_END_OF_FILE, ctx, ctx->current, "unexpected end of file");
                throw;
            }
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

            if (op == '+')
            {
                ectx->value = left_value + ectx->value;
            }
            else if (op == '-')
            {
                ectx->value = left_value - ectx->value;
            }
        }
    }
    catch
    {
    }
}

static void pre_shift_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     shift-expression:
       additive-expression
       shift-expression << additive-expression
       shift-expression >> additive-expression
    */
    try
    {
        pre_additive_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '>>' ||
                ctx->current->type == '<<'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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

static void pre_relational_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        pre_shift_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '>' ||
                ctx->current->type == '<' ||
                ctx->current->type == '>=' ||
                ctx->current->type == '<='))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_shift_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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

static void pre_equality_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        pre_relational_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '==' ||
                ctx->current->type == '!='))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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

static void pre_and_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     AND-expression:
      equality-expression
      AND-expression & equality-expression
    */
    try
    {
        pre_equality_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;
        while (ctx->current != NULL &&
            (ctx->current->type == '&'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_equality_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;
            ectx->value = left_value & ectx->value;
        }
    }
    catch
    {
    }
}

static void pre_exclusive_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     exclusive-OR-expression:
      AND-expression
     exclusive-OR-expression ^ AND-expression
    */
    try
    {
        pre_and_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        
        while (ctx->current != NULL &&
            (ctx->current->type == '^'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_and_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;
            ectx->value = left_value ^ ectx->value;
        }
    }
    catch
    {
    }
}

static void pre_inclusive_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
    inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */
    try
    {
        pre_exclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '|'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_exclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0) throw;

            ectx->value = left_value | ectx->value;
        }
    }
    catch
    {
    }
}

static void pre_logical_and_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
    logical-AND-expression:
     inclusive-OR-expression
     logical-AND-expression && inclusive-OR-expression
    */
    try
    {
        pre_inclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '&&'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_inclusive_or_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

            ectx->value = left_value && ectx->value;
        }
    }
    catch
    {
    }
}

static void pre_logical_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
      logical-OR-expression:
       logical-AND-expression
       logical-OR-expression || logical-AND-expression
    */
    try
    {
        pre_logical_and_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '||'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_logical_and_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

            ectx->value = left_value || ectx->value;

        }
    }
    catch
    {
    }
}



static void pre_assignment_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
       //aqui eh duvidoso mas conditional faz a unary tb.
       //a diferenca q nao eh qualquer expressao
       //que pode ser de atribuicao
    try
    {
        pre_conditional_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        
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
            preprocessor_set_error_with_token(C_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS, ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            throw;
        }
    }
    catch
    {
    }
}

static void pre_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*expression:
      assignment-expression
      expression, assignment-expression
    */
    try
    {
        pre_assignment_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;
        
        while (ctx->current->type == ',')
        {
            pre_match(ctx);
            pre_expression(ctx, ectx);
            if (ctx->n_errors > 0) throw;            
        }
    }
    catch
    {
    }
}



static void pre_conditional_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
      conditional-expression:
      logical-OR-expression
      logical-OR-expression ? expression : conditional-expression
    */
    try
    {
        pre_logical_or_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        if (ctx->current && ctx->current->type == '?')
        {
            pre_match(ctx);
            if (ectx->value)
            {
                pre_expression(ctx, ectx);
                if (ctx->n_errors > 0) throw;

                pre_match(ctx); //:
                struct pre_expression_ctx temp = {0};
                pre_conditional_expression(ctx, &temp);                
                if (ctx->n_errors > 0) throw;
            }
            else
            {
                struct pre_expression_ctx temp = {0};
                pre_expression(ctx, &temp);
                if (ctx->n_errors > 0) throw;

                pre_match(ctx); //:
                pre_conditional_expression(ctx, ectx);
                if (ctx->n_errors > 0) throw;
            }
        }
    }
    catch
    {
    }
}

int pre_constant_expression(struct preprocessor_ctx* ctx,long long* pvalue)
{
    struct pre_expression_ctx ectx = { 0 };
    pre_conditional_expression(ctx, &ectx);
    *pvalue = ectx.value;
    return ctx->n_errors > 0;
}

