#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "expressions.h"
#include "hash.h"
#include <string.h>
#include "console.h"
#include "parser.h"
#include "type.h"

#ifdef _WIN32
#include <crtdbg.h>
#undef assert
#define assert _ASSERTE
#endif




struct expression* postfix_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* cast_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* multiplicative_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* unary_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* additive_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* shift_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* relational_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* equality_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* and_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* exclusive_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* inclusive_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* logical_and_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* logical_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* conditional_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* conditional_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);



int  compare_function_arguments(struct parser_ctx* ctx,
    struct type* p_type,
    struct argument_expression_list* p_argument_expression_list,
    struct error* error)
{
    try
    {
        struct type_list parameter_type = { 0 };


        bool is_var_args = false;
        bool is_void = false;

        if (p_type &&
            p_type->declarator_type &&
            p_type->declarator_type->direct_declarator_type &&
            p_type->declarator_type->direct_declarator_type->array_function_type_list.head)
        {
            parameter_type = p_type->declarator_type->direct_declarator_type->array_function_type_list.head->params;
            is_var_args = p_type->declarator_type->direct_declarator_type->array_function_type_list.head->bVarArg;

            /*detectar que o parametro é (void)*/
            is_void =
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head->params.head &&
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head->params.head->type_specifier_flags == TYPE_SPECIFIER_VOID &&
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head->params.head->declarator_type->pointers.head == NULL;
        }


        struct type* current_parameter_type = parameter_type.head;

        int param_num = 1;
        struct argument_expression* current_argument =
            p_argument_expression_list->head;
        while (current_argument && current_parameter_type)
        {

            if (!type_is_compatible_type_function_call(&current_argument->expression->type, current_parameter_type) != 0)
            {
                parser_seterror_with_token(ctx,
                    current_argument->expression->first,
                    " incompatible types at argument %d", param_num);
            }
            //compare
            current_argument = current_argument->next;
            current_parameter_type = current_parameter_type->next;
            param_num++;
        }

        if (current_argument != NULL && !is_var_args)
        {
            parser_seterror_with_token(ctx,
                p_argument_expression_list->tail->expression->first,
                "too many arguments");
            throw;
        }

        if (current_parameter_type != NULL && !is_void)
        {
            parser_seterror_with_token(ctx,
                p_argument_expression_list->tail->expression->first,
                "too few arguments");

            //seterror(error, "too few parameter_type");
            throw;
        }
    }
    catch
    {}
    return error->code;

}



bool is_enumeration_constant(struct parser_ctx* ctx)
{
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

bool is_first_of_floating_constant(struct parser_ctx* ctx)
{
    /*
     floating-constant:
      decimal-floating-constant
      hexadecimal-floating-constan
    */
    return ctx->current->type == TK_COMPILER_DECIMAL_FLOATING_CONSTANT ||
        ctx->current->type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT;
}

bool is_first_of_integer_constant(struct parser_ctx* ctx)
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

bool is_predefined_constant(struct parser_ctx* ctx)
{
    return ctx->current->type == TK_KEYWORD_TRUE ||
        ctx->current->type == TK_KEYWORD_FALSE ||
        ctx->current->type == TK_KEYWORD_NULLPTR;
}

bool is_first_of_constant(struct parser_ctx* ctx)
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

bool is_first_of_primary_expression(struct parser_ctx* ctx)
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
        ctx->current->type == TK_KEYWORD__GENERIC ||
        ctx->current->type == TK_KEYWORD_TYPEID;
}

struct generic_association* generic_association(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    struct generic_association* p_generic_association = NULL;
    try
    {
        p_generic_association = calloc(1, sizeof * p_generic_association);
        

        /*generic - association:
            type-name : assignment-expression
            default : assignment-expression
            */
        if (ctx->current && ctx->current->type == TK_KEYWORD_DEFAULT)
        {
            parser_match(ctx);
        }
        else if (first_of_type_name(ctx))
        {            
            p_generic_association->p_type_name = type_name(ctx, error);
            p_generic_association->type = make_type_using_declarator(ctx, p_generic_association->p_type_name->declarator);            
        }
        else
        {
            seterror(error, "error");
            error->code = 1;
        }
        parser_match_tk(ctx, ':', error);
        p_generic_association->expression =  assignment_expression(ctx, error, ectx);
    }
    catch{
    }

    return p_generic_association;
}

struct generic_assoc_list generic_association_list(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    struct generic_assoc_list list = { 0 };
    try
    {
        struct generic_association* p_generic_association = 
          generic_association(ctx, error, ectx);
        
        list_add(&list, p_generic_association);
        
        while (error->code == 0 &&
            ctx->current->type == ',')
        {
            parser_match(ctx);

            struct generic_association* p_generic_association2 =
                generic_association(ctx, error, ectx);
            list_add(&list, p_generic_association2);
        }
    }
    catch
    {
    }
    return list;
}

static void print_clean_list(struct token_list* list)
{
    struct token* current = list->head;
    while (current)
    {
        if (current != list->head &&
            (current->flags & TK_FLAG_HAS_SPACE_BEFORE ||
                current->flags & TK_FLAG_HAS_NEWLINE_BEFORE))
        {
            printf(" ");
        }
        printf("%s", current->lexeme);
        if (current == list->tail)
            break;
        current = current->next;
    }
}

struct generic_selection * generic_selection(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
    generic-selection:
    _Generic ( assignment-expression , generic-assoc-ctx )
    */
    struct generic_selection* p_generic_selection = NULL;
    try
    {
        p_generic_selection = calloc(1, sizeof * p_generic_selection);
        
        p_generic_selection->first_token = ctx->current;
        

        parser_match_tk(ctx, TK_KEYWORD__GENERIC, error);
        parser_match_tk(ctx, '(', error);
        struct token_list l = { 0 };
        l.head = ctx->current;
        p_generic_selection->expression = assignment_expression(ctx, error, ectx);
        l.tail = ctx->current->prev;
        
        parser_match_tk(ctx, ',', error);

        p_generic_selection->generic_assoc_list = generic_association_list(ctx, error, ectx);
        
        struct generic_association* current = p_generic_selection->generic_assoc_list.head;
        while (current)
        {
            if (current->p_type_name)
            {
                if (type_is_same(&p_generic_selection->expression->type, &current->type, true))
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
        
        p_generic_selection->last_token= ctx->current;
        parser_match_tk(ctx, ')', error);
    }
    catch
    {
    }
    return p_generic_selection;
}


struct expression* typeid_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = calloc(1, sizeof * p_expression_node);
        p_expression_node->expression_type = TYPEID_EXPRESSION_TYPE;
        p_expression_node->first = ctx->current;

        parser_match_tk(ctx, TK_KEYWORD_TYPEID, error);
        parser_match_tk(ctx, '(', error);
        if (first_of_type_name(ctx))
        {
            
            p_expression_node->type_name = type_name(ctx, error);
            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
            //printf("typeid() = ");
            //print_type(&p_expression_node->type);
            //printf("\n");
        }
        else
        {
        
            bool constant_expr_required_old = ectx->bConstantExpressionRequired;
            ectx->bConstantExpressionRequired = false;
            p_expression_node->right = expression(ctx, error, ectx);

            if (p_expression_node->right == NULL)
                throw;

            ectx->bConstantExpressionRequired = constant_expr_required_old;
            p_expression_node->type = type_copy(&p_expression_node->right->type);

            //printf("typeid() = ");
            //print_type(&p_expression_node->type);
            //printf("\n");
        }

        p_expression_node->last = ctx->current;
        parser_match_tk(ctx, ')', error);
    }
    catch
    {
    }

    return p_expression_node;
}

int char_constant_to_int(const char* text)
{
    const char* p = text + 1;
    int value = 0;
    while (*p != '\'')
    {
        value = value * 256 + *p;
        p++;
    }

    return value;
}


//enum token_type parse_number(const char* lexeme, enum type_specifier_flags* flags, struct error* error);

int convert_to_number(struct token* token, struct expression* p_expression_node, struct error* error)
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
    enum type_specifier_flags  flags = 0;
    parse_number(buffer, &flags, error);
    p_expression_node->type.type_specifier_flags = flags;

    switch (token->type)
    {
    case TK_COMPILER_DECIMAL_CONSTANT:

        if (flags && TYPE_SPECIFIER_UNSIGNED)
        {
            p_expression_node->constant_value = strtoll(buffer, 0, 10);
        }
        else
        {
            p_expression_node->constant_value = strtoll(buffer, 0, 10);
        }

        break;
    case TK_COMPILER_OCTAL_CONSTANT:
        p_expression_node->constant_value = strtoll(buffer, 0, 8);
        break;
    case TK_COMPILER_HEXADECIMAL_CONSTANT:
        p_expression_node->constant_value = strtoll(buffer + 2, 0, 16);
        break;
    case TK_COMPILER_BINARY_CONSTANT:
        p_expression_node->constant_value = strtoll(buffer + 2, 0, 2);
        break;
    case TK_COMPILER_DECIMAL_FLOATING_CONSTANT:
        //p_expression_node->type.type_specifier_flags |= TYPE_SPECIFIER_DOUBLE;
        //result = atof(buffer, 0, 10); 
        //assert(false);
        break;
    case TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT:
        //p_expression_node->type.type_specifier_flags |= TYPE_SPECIFIER_DOUBLE;
        //assert(false);
        break;
    default:
        assert(false);
    }

    return error->code;
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


struct expression* primary_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
     primary-expression:
      identifier
      constant
      string-literal
      ( expression )
      generic-selection
    */
    struct expression* p_expression_node = NULL;
    try
    {
        if (ctx->current->type == TK_IDENTIFIER)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);

            if (p_expression_node == NULL) throw;
            
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            struct enumerator* p_enumerator = find_enumerator(ctx, ctx->current->lexeme, NULL);
            if (p_enumerator)
            {
                p_expression_node->expression_type = PRIMARY_EXPRESSION_ENUMERATOR;
                p_expression_node->constant_value = p_enumerator->value;
                type_set_int(&p_expression_node->type);
            }
            else
            {
                if (ectx->bConstantExpressionRequired)
                {
                    parser_seterror_with_token(ctx, ctx->current, "not constant");
                    error->code = 1;
                    throw;
                }
                struct declarator* p_declarator = find_declarator(ctx, ctx->current->lexeme, NULL);
                if (p_declarator == NULL)
                {
                    parser_seterror_with_token(ctx, ctx->current, "not found '%s'\n", ctx->current->lexeme);
                    error->code = 1;
                    throw;
                }
                else
                {
                    p_declarator->num_uses++;
                    p_expression_node->declarator = p_declarator;
                    p_expression_node->expression_type = PRIMARY_EXPRESSION_DECLARATOR;

                    assert(p_declarator->type.type_specifier_flags != 0);
                    p_expression_node->type = type_copy(&p_declarator->type);
                }
            }
            parser_match(ctx);
        }
        else if (ctx->current->type == TK_STRING_LITERAL)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_STRING_LITERAL;
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            p_expression_node->type.type_qualifier_flags = TYPE_QUALIFIER_CONST;
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_CHAR;
            
            struct declarator_type* p_declarator_type = calloc(1, sizeof * p_declarator_type);
            struct array_function_type* p_array_function_type = calloc(1, sizeof * p_array_function_type);
            struct direct_declarator_type* p_direct_declarator_type = calloc(1, sizeof * p_direct_declarator_type);

            p_declarator_type->direct_declarator_type = p_direct_declarator_type;
            
            //TODO decode before get size
            p_array_function_type->array_size = strlen(ctx->current->lexeme) - 2 /*2 quotes*/ + 1 /*\0*/;

            p_array_function_type->bIsArray = true;
            list_add(&p_declarator_type->direct_declarator_type->array_function_type_list, p_array_function_type);
            
            p_expression_node->type.declarator_type = p_declarator_type;
            
            //type_print(&p_expression_node->type);
            //printf("TODO warning correct type for literals not implemented yet\n");
            //DECLARE_AND_CREATE_STRUCT_POINTER(direct_declarator_type);
            //p_expression_node->type.declarator_type->direct_declarator_type.array_function_type_list;// = direct_declarator_type;

            //DECLARE_AND_CREATE_STRUCT_POINTER(direct_declarator_type);


            if (ectx->bConstantExpressionRequired)
            {
                parser_seterror_with_token(ctx, ctx->current, "not constant");
                error->code = 1;
                throw;
            }
            else
            {
                parser_match(ctx);
            }
            /*
            string concatenation deveria ser em uma phase anterior
            mas como mantemos as forma do fonte aqui foi uma alternativa
            */
            while (ctx->current &&
                error->code == 0 &&
                ctx->current->type == TK_STRING_LITERAL)
            {
                //TODO montar estring toda
                parser_match(ctx);
            }
        }
        else if (ctx->current->type == TK_CHAR_CONSTANT)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->constant_value = char_constant_to_int(ctx->current->lexeme);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_CHAR_LITERAL;
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            type_set_int(&p_expression_node->type);

            parser_match(ctx);
        }

        else if (ctx->current->type == TK_KEYWORD_TRUE ||
            ctx->current->type == TK_KEYWORD_FALSE)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_PREDEFINED_CONSTANT;
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            p_expression_node->constant_value =
                ctx->current->type == TK_KEYWORD_TRUE ? 1 : 0;

            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_BOOL;
            p_expression_node->type.type_qualifier_flags = 0;
            p_expression_node->type.declarator_type = NULL;


            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD_NULLPTR)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_PREDEFINED_CONSTANT;
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            p_expression_node->constant_value = 0;
            
            /*TODO nullptr type*/
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_LONG;
            p_expression_node->type.type_qualifier_flags = 0;
            p_expression_node->type.declarator_type = NULL;


            parser_match(ctx);
        }
        else if (is_integer_or_floating_constant(ctx->current->type))
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;
            p_expression_node->expression_type = PRIMARY_EXPRESSION_NUMBER;
            convert_to_number(ctx->current, p_expression_node, error);
            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD__GENERIC)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_GENERIC;

            if (ectx->bConstantExpressionRequired)
            {
                parser_seterror_with_token(ctx, ctx->current, "not constant");
                error->code = 1;
            }
            else
            {
                
                p_expression_node->generic_selection = generic_selection(ctx, error, ectx);
                p_expression_node->first = p_expression_node->generic_selection->first_token;
                p_expression_node->last = p_expression_node->generic_selection->last_token;

                if (p_expression_node->generic_selection->p_view_selected_expression)
                {
                    p_expression_node->type = type_copy(&p_expression_node->generic_selection->p_view_selected_expression->type);
                }
                else
                {
                    parser_seterror_with_token(ctx, ctx->current, "no match for generic");
                    error->code = 1;
                }
            }
        }
        else if (ctx->current->type == TK_KEYWORD_TYPEID)
        {
            p_expression_node = typeid_expression(ctx, error, ectx);
        }
        else if (ctx->current->type == '(')
        {
            parser_match(ctx);
            p_expression_node = expression(ctx, error, ectx);
            if (error->code != 0)
                throw;
            parser_match_tk(ctx, ')', error);
        }
        else
        {
            parser_seterror_with_token(ctx, ctx->current, "unexpected");
        }
    }
    catch
    {
    }

    /*generic - assoc - ctx:
    generic-association
    generic-assoc-ctx , generic-association
    */
    //assert(p_expression_node&& p_expression_node->first);
    //assert(p_expression_node&& p_expression_node->last);
    
    return p_expression_node;
}



struct argument_expression_list argument_expression_list(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
     argument-expression-list:
      assignment-expression
      argument-expression-ctx , assignment-expression
    */
    struct argument_expression_list list = { 0 };

    struct argument_expression* p_argument_expression = calloc(1, sizeof(struct argument_expression));
    p_argument_expression->expression = assignment_expression(ctx, error, ectx);
    list_add(&list, p_argument_expression);

    while (error->code == 0 &&
        ctx->current->type == ',')
    {
        parser_match(ctx);

        struct argument_expression* p_argument_expression_2 = calloc(1, sizeof * p_argument_expression_2);
        p_argument_expression_2->expression = assignment_expression(ctx, error, ectx);
        list_add(&list, p_argument_expression_2);


    }

    return list;
}

bool first_of_postfix_expression(struct parser_ctx* ctx)
{
    //( type-name )  postfix confunde com (expression) primary
    if (first_of_type_name_ahead(ctx))
        return true; //acho q  nao precisa pq primary tb serve p postif
    return is_first_of_primary_expression(ctx);
}


struct expression* postfix_expression_tail(struct parser_ctx* ctx,
    struct error* error,
    struct expression* p_expression_node,
    struct expression_ctx* ectx)
{
    try
    {
        while (error->code == 0 && ctx->current != NULL)
        {
            if (ctx->current->type == '[')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_ARRAY;
                p_expression_node_new->left = p_expression_node;

                //TODO verificar se eh ponteiro ou array
                // 
                //if (!type_is_function_or_function_pointer(&p_expression_node->type))
                //{
                    //seterror_with_token(ctx, ctx->current,
                  //      "called object is not a function or function pointer");
                    //throw;
                //}
                if (type_is_pointer(&p_expression_node->type))
                {
                    p_expression_node_new->type = get_pointer_content_type(&p_expression_node->type);

                }
                else if (type_is_array(&p_expression_node->type))
                {
                    p_expression_node_new->type = get_array_item_type(&p_expression_node->type);
                }

                parser_match(ctx);
                /*contem a expresao de dentro do  [ ] */
                p_expression_node_new->right = expression(ctx, error, ectx);
                if (error->code != 0) throw;
                parser_match_tk(ctx, ']', error);
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '(')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_FUNCTION_CALL;
                p_expression_node_new->left = p_expression_node;


                if (!type_is_function_or_function_pointer(&p_expression_node->type))
                {
                    parser_seterror_with_token(ctx, ctx->current,
                        "called object is not a function or function pointer");
                    throw;
                }

                p_expression_node_new->type = get_function_return_type(&p_expression_node->type);


                parser_match(ctx);
                if (ctx->current->type != ')')
                {
                    p_expression_node_new->argument_expression_list = argument_expression_list(ctx, error, ectx);
                    if (error->code != 0) throw;
                }
                parser_match_tk(ctx, ')', error);

                //Agora vamos comparar os argumentos...
                struct error local = { 0 };
                if (compare_function_arguments(ctx, &p_expression_node->type, &p_expression_node_new->argument_expression_list, &local) != 0)
                {


                    throw;
                }

                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '.')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_DOT;
                p_expression_node_new->left = p_expression_node;

                parser_match(ctx);
                if (p_expression_node->type.type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
                {
                    struct struct_or_union_specifier* p =
                        find_struct_or_union_specifier(ctx,
                            p_expression_node->type.struct_or_union_specifier->tag_name);
                    if (p)
                    {
                        struct member_declarator* p_member_declarator =
                            find_member_declarator(&p->member_declaration_list, ctx->current->lexeme);
                        if (p_member_declarator)
                        {
                            p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                        }
                        else
                        {

                            parser_seterror_with_token(ctx, ctx->current, "struct member '%s' not found in '%s'",
                                ctx->current->lexeme,
                                p_expression_node->type.struct_or_union_specifier->tag_name);
                        }
                    }
                    else
                    {
                        print_scope(&ctx->scopes);
                    }
                    parser_match_tk(ctx, TK_IDENTIFIER, error);
                }
                else
                {
                    parser_seterror_with_token(ctx, ctx->current, "structure or union required");
                }
                //todo apontar pro nome?
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '->')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_ARROW;
                p_expression_node_new->left = p_expression_node;

                parser_match(ctx);
                if (p_expression_node->type.type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
                {
                    struct struct_or_union_specifier* p = find_struct_or_union_specifier(ctx,
                        p_expression_node->type.struct_or_union_specifier->tag_name);

                    if (p)
                    {
                        struct member_declarator* p_member_declarator =
                            find_member_declarator(&p->member_declaration_list, ctx->current->lexeme);
                        if (p_member_declarator)
                        {
                            p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                        }
                        else
                        {
                            parser_seterror_with_token(ctx,
                                ctx->current,
                                "struct member '%s' not found in '%s'",
                                ctx->current->lexeme, p_expression_node->type.struct_or_union_specifier->tag_name);
                        }
                    }
                    else
                    {
                        print_scope(&ctx->scopes);
                    }
                    parser_match_tk(ctx, TK_IDENTIFIER, error);
                }
                else
                {
                    parser_seterror_with_token(ctx, ctx->current, "structure or union required");
                }
                //todo apontar pro nome?
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '++')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_INCREMENT;
                p_expression_node_new->left = p_expression_node;
                p_expression_node_new->type = type_copy(&p_expression_node->type);
                parser_match(ctx);
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '--')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_DECREMENT;
                p_expression_node_new->left = p_expression_node;
                parser_match(ctx);
                p_expression_node = p_expression_node_new;
            }
            else
            {
                break;
            }
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* postfix_expression_type_name(struct parser_ctx* ctx, struct type_name* p_type_name, struct error* error, struct expression_ctx* ectx)
{
    /*
        ( type-name ) { initializer-ctx }
        ( type-name ) { initializer-ctx , }

        //My extension : if type-name is function then follow is compound-statement
        ( type-name ) compound-statement

    */

    struct expression* p_expression_node = calloc(1, sizeof * p_expression_node);

    assert(p_expression_node->type_name == NULL);
    p_expression_node->first = ctx->current;
    p_expression_node->type_name = p_type_name;
    p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
    bool is_function_type = false;
    if (p_expression_node->type.declarator_type->direct_declarator_type->array_function_type_list.head &&
        p_expression_node->type.declarator_type->direct_declarator_type->array_function_type_list.head->bIsFunction)
    {
        if (p_expression_node->type.declarator_type->direct_declarator_type->declarator_opt == NULL)
        {
            is_function_type = true;
        }
        else
        {
            /*funtion pointer*/
        }
    }
    if (is_function_type)
    {
        p_expression_node->expression_type = POSTFIX_EXPRESSION_FUNCTION_LITERAL;

        struct scope* parameters_scope =
            &p_expression_node->type_name->declarator->direct_declarator->array_function_list.head->function_declarator->parameters_scope;

        scope_list_push(&ctx->scopes, parameters_scope);
        p_expression_node->compound_statement = function_body(ctx, error);
        scope_list_pop(&ctx->scopes);

        p_expression_node->last = p_expression_node->compound_statement->last_token;
    }
    else
    {
        p_expression_node->expression_type = POSTFIX_EXPRESSION_COMPOUND_LITERAL;
        p_expression_node->braced_initializer = braced_initializer(ctx, error);
    }

    p_expression_node = postfix_expression_tail(ctx,
        error,
        p_expression_node,
        ectx);

    return p_expression_node;
}

struct expression* postfix_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
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
    struct expression* p_expression_node = NULL;
    try
    {
        if (first_of_type_name_ahead(ctx)) //aqui preciso ver se nao eh primary
        {
            assert(false); //este caso esta pegando lá dentro deo cast expression.
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->first = ctx->current;
            parser_match_tk(ctx, '(', error);
            p_expression_node->type_name = type_name(ctx, error);
            if (error->code != 0) throw;

            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
            parser_match_tk(ctx, ')', error);
            //printf("\n");
            //print_type(&p_expression_node->type);
            bool is_function_type = false;
            if (p_expression_node->type.declarator_type->direct_declarator_type->array_function_type_list.head &&
                p_expression_node->type.declarator_type->direct_declarator_type->array_function_type_list.head->bIsFunction)
            {
                if (p_expression_node->type.declarator_type->direct_declarator_type->declarator_opt == NULL)
                {
                    is_function_type = true;
                }
                else
                {
                    /*funtion pointer*/
                }
            }
            if (is_function_type)
            {
                p_expression_node->expression_type = POSTFIX_EXPRESSION_FUNCTION_LITERAL;
                p_expression_node->compound_statement = compound_statement(ctx, error);
                if (error->code != 0) throw;
                p_expression_node->last = p_expression_node->compound_statement->last_token;
            }
            else
            {
                p_expression_node->expression_type = POSTFIX_EXPRESSION_COMPOUND_LITERAL;
                p_expression_node->braced_initializer = braced_initializer(ctx, error);
                p_expression_node->last = ctx->current;
            }
        }
        else
        {
            p_expression_node = primary_expression(ctx, error, ectx);
        }
        
            p_expression_node = postfix_expression_tail(ctx,
                error,
                p_expression_node,
                ectx);
        
    }
    catch
    {
    }
    return p_expression_node;
}


bool is_first_of_unary_expression(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;
    return  first_of_postfix_expression(ctx) ||
        ctx->current->type == '++' ||
        ctx->current->type == '--' ||
        ctx->current->type == '&' ||
        ctx->current->type == '*' ||
        ctx->current->type == '+' ||
        ctx->current->type == '-' ||
        ctx->current->type == '~' ||
        ctx->current->type == '!' ||
        ctx->current->type == TK_KEYWORD_SIZEOF ||
        ctx->current->type == TK_KEYWORD_HASHOF ||
        ctx->current->type == TK_KEYWORD__ALIGNOF;
}

struct expression* unary_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;


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
    struct expression* p_expression_node = NULL;
    try
    {
        if (ctx->current->type == '++' || ctx->current->type == '--')
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);

            if (ctx->current->type == '++')
                new_expression->expression_type = UNARY_EXPRESSION_INCREMENT;
            else
                new_expression->expression_type = UNARY_EXPRESSION_DECREMENT;
            parser_match(ctx);
            new_expression->right = unary_expression(ctx, error, ectx);
            if (error->code != 0) throw;

            new_expression->type = type_copy(&new_expression->right->type);
            p_expression_node = new_expression;
        }
        else if (ctx->current != NULL &&
            (ctx->current->type == '&'
                || ctx->current->type == '*'
                || ctx->current->type == '+'
                || ctx->current->type == '-'
                || ctx->current->type == '~'
                || ctx->current->type == '!'))
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->first = ctx->current;

          

            

            struct token* op_position = ctx->current; //marcar posicao
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->right = cast_expression(ctx, error, ectx);
            if (error->code != 0) throw;
            new_expression->last = new_expression->right->last;
            if (op == '!')
            {
                new_expression->expression_type = UNARY_EXPRESSION_NOT;
                new_expression->constant_value = !new_expression->right->constant_value;
                new_expression->type = type_copy(&new_expression->right->type);
            }
            else if (op == '~')
            {
                new_expression->expression_type = UNARY_EXPRESSION_BITNOT;
                new_expression->constant_value = ~new_expression->right->constant_value;
                new_expression->type = type_copy(&new_expression->right->type);
            }
            else if (op == '-')
            {
                new_expression->expression_type = UNARY_EXPRESSION_NEG;
                new_expression->constant_value = -new_expression->right->constant_value;
                new_expression->type = type_copy(&new_expression->right->type);
            }
            else if (op == '+')
            {
                new_expression->expression_type = UNARY_EXPRESSION_PLUS;
                new_expression->constant_value = +new_expression->right->constant_value;
                new_expression->type = type_copy(&new_expression->right->type);
            }
            else if (op == '*')
            {
                new_expression->expression_type = UNARY_EXPRESSION_CONTENT;
                if (!type_is_pointer(&new_expression->right->type))
                {
                    parser_seterror_with_token(ctx, op_position, "indirection requires pointer operand");
                }
                new_expression->type = get_pointer_content_type(&new_expression->right->type);                
            }
            else if (op == '&')
            {
                new_expression->expression_type = UNARY_EXPRESSION_ADDRESSOF;
                //TODO nao tem como tirar endereco de uma constante
                new_expression->type = get_address_of_type(&new_expression->right->type);
            }
            else
            {
                parser_seterror_with_token(ctx, ctx->current, "invalid token");
                throw;
            }
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_SIZEOF)
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            if (first_of_type_name_ahead(ctx))
            {
                new_expression->expression_type = UNARY_EXPRESSION_SIZEOF_TYPE;
                parser_match_tk(ctx, '(', error);
                new_expression->type_name = type_name(ctx, error);
                new_expression->type = make_type_using_declarator(ctx, new_expression->type_name->declarator);
                parser_match_tk(ctx, ')', error);
                new_expression->constant_value = type_get_sizeof(ctx, &new_expression->type, error);
            }
            else
            {
                bool old = ectx->bConstantExpressionRequired;
                ectx->bConstantExpressionRequired = false;
                new_expression->right = unary_expression(ctx, error, ectx);
                ectx->bConstantExpressionRequired = old;

                if (error->code != 0)
                    throw;

                new_expression->expression_type = UNARY_EXPRESSION_SIZEOF_EXPRESSION;
                new_expression->constant_value = type_get_sizeof(ctx, &new_expression->right->type, error);
            }
            type_set_int(&new_expression->type); //resultado sizeof
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_HASHOF)
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->first = ctx->current;

            parser_match(ctx);

            if (first_of_type_name_ahead(ctx))
            {
                new_expression->expression_type = UNARY_EXPRESSION_HASHOF_TYPE;
                parser_match_tk(ctx, '(', error);
                new_expression->type_name = type_name(ctx, error);
                new_expression->type = make_type_using_declarator(ctx, new_expression->type_name->declarator);

                new_expression->last = ctx->current;

                parser_match_tk(ctx, ')', error);
                new_expression->constant_value = type_get_hashof(ctx, &new_expression->type, error);
            }
            else
            {
                bool old = ectx->bConstantExpressionRequired;
                ectx->bConstantExpressionRequired = false;
                new_expression->right = unary_expression(ctx, error, ectx);
                ectx->bConstantExpressionRequired = old;

                if (error->code != 0)
                    throw;

                new_expression->expression_type = UNARY_EXPRESSION_HASHOF_TYPE;
                new_expression->constant_value = type_get_hashof(ctx, &new_expression->right->type, error);
                new_expression->last = previous_parser_token(ctx->current);
            }

            type_set_int(&new_expression->type); //resultado sizeof
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD__ALIGNOF)
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);


            new_expression->expression_type = UNARY_EXPRESSION_ALIGNOF;
            parser_match_tk(ctx, '(', error);
            new_expression->type_name = type_name(ctx, error);
            new_expression->type = make_type_using_declarator(ctx, new_expression->type_name->declarator);
            parser_match_tk(ctx, ')', error);
            new_expression->constant_value = type_get_sizeof(ctx, &new_expression->type, error);


            type_set_int(&new_expression->type); //resultado sizeof
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD__ALIGNAS)
        {
        }
        else //if (is_first_of_primary_expression(ctx))
        {
            p_expression_node = postfix_expression(ctx, error, ectx);
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* cast_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{



    /*
     cast-expression:
      unary-expression
      ( type-name ) cast-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        if (error->code != 0)
            throw;


        if (first_of_type_name_ahead(ctx))
        {

            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->first = ctx->current;
            p_expression_node->expression_type = CAST_EXPRESSION;
            parser_match_tk(ctx, '(', error);
            p_expression_node->type_name = type_name(ctx, error);
            if (error->code != 0)
                throw;

            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
            //type_set_int(&ectx->result_type);
            //print_type_name(p_cast_expression->type_name);
            parser_match_tk(ctx, ')', error);
            //struct token_list r = copy_replacement_list(&l);
            //pop_f
            if (ctx->current->type == '{')
            {
                // Achar que era um cast_expression foi um engano...
                // porque apareceu o { então é compound literal que eh postfix.
                struct expression* new_expression = postfix_expression_type_name(ctx, p_expression_node->type_name, error, ectx);

                new_expression->first = p_expression_node->first;

                free(p_expression_node);
                p_expression_node = new_expression;
            }
            else
            {
                p_expression_node->left = cast_expression(ctx, error, ectx);
                p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
            }
            //token_list_destroy(&ectx->type);
            //ectx->type = r;
            //print_list(&ectx->type);
        }
        else if (is_first_of_unary_expression(ctx))
        {
            p_expression_node = unary_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;
        }
        else
        {
            parser_seterror_with_token(ctx, ctx->current, "unexpected");
        }
    }
    catch
    {
    }
    if (p_expression_node && ctx->current)
      p_expression_node->last = previous_parser_token(ctx->current);
    return p_expression_node;
}

struct expression* multiplicative_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{


    /*
     multiplicative-expression:
    cast-expression
    multiplicative-expression * cast-expression
    multiplicative-expression / cast-expression
    multiplicative-expression % cast-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        if (error->code != 0)
            throw;

        p_expression_node = cast_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '*' ||
                ctx->current->type == '/' ||
                ctx->current->type == '%'))
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->left = p_expression_node;
            new_expression->right = cast_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            if (op == '*')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MULT;
                new_expression->constant_value = (new_expression->left->constant_value * new_expression->right->constant_value);
            }
            else if (op == '/')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_DIV;
                if (new_expression->right->constant_value != 0)
                {
                    new_expression->constant_value = (new_expression->left->constant_value / new_expression->right->constant_value);
                }
                else
                {
                }
            }
            else if (op == '%')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MOD;
                if (new_expression->right->constant_value != 0)
                    new_expression->constant_value = (new_expression->left->constant_value % new_expression->right->constant_value);
            }

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }
    return p_expression_node;
}

struct expression* additive_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
     additive-expression:
    multiplicative-expression
    additive-expression + multiplicative-expression
    additive-expression - multiplicative-expression
    */

    struct expression* p_expression_node = NULL;
    struct expression* new_expression = NULL;

    try
    {
        if (error->code != 0)
            throw;
        p_expression_node = multiplicative_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;


        while (ctx->current != NULL &&
            (ctx->current->type == '+' ||
                ctx->current->type == '-'))
        {
            struct token* operator_position = ctx->current;

            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->left = p_expression_node;

            static int count = 0;
            count++;
            new_expression->right = multiplicative_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            if (!type_is_scalar(&new_expression->left->type))
            {
                parser_seterror_with_token(ctx, operator_position, "left operator is not scalar");
            }
            if (!type_is_scalar(&new_expression->right->type))
            {
                parser_seterror_with_token(ctx, operator_position, "right operator is not scalar");
            }

            if (op == '+')
            {
                new_expression->expression_type = ADDITIVE_EXPRESSION_PLUS;
                new_expression->constant_value = (new_expression->left->constant_value + new_expression->right->constant_value);


            }
            else if (op == '-')
            {
                new_expression->expression_type = ADDITIVE_EXPRESSION_MINUS;
                new_expression->constant_value = (new_expression->left->constant_value - new_expression->right->constant_value);
            }

            if (type_is_pointer(&new_expression->left->type))
            {
                /*
                 pointer +- integer
                */
                if (type_is_integer(&new_expression->right->type))
                {
                    new_expression->type = type_copy(&new_expression->left->type);
                }
                else if (type_is_pointer_or_array(&new_expression->right->type))
                {
                    //tem que ser do mesmo tipo..
                    if (op == '-')
                    {
                        if (type_is_same(&new_expression->left->type, &new_expression->right->type, true))
                        {
                            type_set_int(&new_expression->type);//
                        }
                        else
                        {
                            parser_seterror_with_token(ctx, ctx->current, "incompatible types +-");
                        }
                    }
                    else
                    {
                        parser_seterror_with_token(ctx, ctx->current, "invalid operands of types");
                    }
                }
                else
                {
                    parser_seterror_with_token(ctx, ctx->current, "sorry not implemented yet...");
                }
            }
            else
            {
                struct error localerror = { 0 };
                new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
                if (localerror.code != 0)
                {
                    parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                    throw;
                }
            }

            if (error->code != 0)
                throw;

            p_expression_node = new_expression;
            new_expression = NULL; /*MOVED*/
        }
    }
    catch
    {
        if (p_expression_node)
        {
            //expression_node_delete(p_expression_node);
        }
        if (new_expression)
        {
            //expression_node_delete(p_expression_node);
        }
    }



    return p_expression_node;
}

struct expression* shift_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
     shift-expression:
       additive-expression
       shift-expression << additive-expression
       shift-expression >> additive-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = additive_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '>>' ||
                ctx->current->type == '<<'))
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->left = p_expression_node;
            new_expression->right = multiplicative_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            if (op == '>>')
            {
                new_expression->expression_type = SHIFT_EXPRESSION_RIGHT;
                new_expression->constant_value = (new_expression->left->constant_value >> new_expression->right->constant_value);
            }
            else if (op == '<<')
            {
                new_expression->expression_type = SHIFT_EXPRESSION_LEFT;
                new_expression->constant_value = (new_expression->left->constant_value << new_expression->right->constant_value);
            }

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* relational_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
    relational-expression:
    shift-expression
    relational-expression < shift-expression
    relational-expression > shift-expression
    relational-expression <= shift-expression
    relational-expression >= shift-expression
    */
    struct expression* p_expression_node = shift_expression(ctx, error, ectx);
    if (error->code != 0)
        return NULL;

    while (ctx->current != NULL &&
        (ctx->current->type == '>' ||
            ctx->current->type == '<' ||
            ctx->current->type == '>=' ||
            ctx->current->type == '<='))
    {
        struct expression* new_expression = calloc(1, sizeof * new_expression);
        enum token_type op = ctx->current->type;
        parser_match(ctx);
        new_expression->left = p_expression_node;
        new_expression->right = shift_expression(ctx, error, ectx);
        if (error->code != 0)
            break;

        if (op == '>')
        {
            new_expression->expression_type = RELATIONAL_EXPRESSION_BIGGER_THAN;
            new_expression->constant_value = (new_expression->left->constant_value > new_expression->right->constant_value);
        }
        else if (op == '<')
        {
            new_expression->expression_type = RELATIONAL_EXPRESSION_LESS_THAN;
            new_expression->constant_value = (new_expression->left->constant_value < new_expression->right->constant_value);
        }
        else if (op == '>=')
        {
            new_expression->expression_type = RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN;
            new_expression->constant_value = (new_expression->left->constant_value >= new_expression->right->constant_value);
        }
        else if (op == '<=')
        {
            new_expression->expression_type = RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN;
            new_expression->constant_value = (new_expression->left->constant_value <= new_expression->right->constant_value);
        }

        type_set_int(&new_expression->type);

        p_expression_node = new_expression;
    }
    return p_expression_node;
}

struct expression* equality_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

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
    struct expression* p_expression_node = relational_expression(ctx, error, ectx);
    if (error->code != 0)
        return NULL;


    while (ctx->current != NULL &&
        (ctx->current->type == '==' ||
            ctx->current->type == '!='))
    {
        struct expression* new_expression = calloc(1, sizeof * new_expression);
        enum token_type op = ctx->current->type;
        parser_match(ctx);
        new_expression->left = p_expression_node;
        new_expression->right = relational_expression(ctx, error, ectx);
        if (error->code != 0)
            break;

        if (op == '==')
        {
            new_expression->expression_type = EQUALITY_EXPRESSION_EQUAL;

            if (new_expression->left->expression_type == TYPEID_EXPRESSION_TYPE ||
                new_expression->right->expression_type == TYPEID_EXPRESSION_TYPE)
            {
                new_expression->constant_value = type_is_same(&new_expression->left->type, &new_expression->right->type, true);
            }
            else
            {
                new_expression->constant_value = (new_expression->left->constant_value == new_expression->right->constant_value);
            }
        }
        else if (op == '!=')
        {
            new_expression->expression_type = EQUALITY_EXPRESSION_EQUAL;

            if (new_expression->left->expression_type == TYPEID_EXPRESSION_TYPE ||
                new_expression->right->expression_type == TYPEID_EXPRESSION_TYPE)
            {
                new_expression->constant_value = !type_is_same(&new_expression->left->type, &new_expression->right->type, true);
            }
            else
            {
                new_expression->constant_value = (new_expression->left->constant_value != new_expression->right->constant_value);
            }
        }
        else
        {
            assert(false);
        }
        type_set_int(&new_expression->type);
        p_expression_node = new_expression;
    }
    return p_expression_node;
}

struct expression* and_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
     AND-expression:
      equality-expression
      AND-expression & equality-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = equality_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '&'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = AND_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = equality_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            new_expression->constant_value = (new_expression->left->constant_value & new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* exclusive_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
     exclusive-OR-expression:
      AND-expression
     exclusive-OR-expression ^ AND-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = and_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '^'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = EXCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = and_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            new_expression->constant_value = (new_expression->left->constant_value ^ new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* inclusive_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
    inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = exclusive_or_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '|'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);

            new_expression->expression_type = INCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = exclusive_or_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            new_expression->constant_value = (new_expression->left->constant_value | new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }
            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* logical_and_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
    logical-AND-expression:
     inclusive-OR-expression
     logical-AND-expression && inclusive-OR-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = inclusive_or_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '&&'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = INCLUSIVE_AND_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = inclusive_or_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            new_expression->constant_value = (new_expression->left->constant_value && new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }
            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* logical_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
      logical-OR-expression:
       logical-AND-expression
       logical-OR-expression || logical-AND-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        if (error->code != 0)
            throw;

        p_expression_node = logical_and_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '||'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = LOGICAL_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = logical_and_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            //TODO converter cada lado para bool


            new_expression->constant_value = (new_expression->left->constant_value || new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }
            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}



struct expression* assignment_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

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
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = conditional_expression(ctx, error, ectx);
        if (error->code != 0)
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
            parser_match(ctx);

            if (ectx->bConstantExpressionRequired)
            {
                parser_seterror_with_token(ctx, ctx->current, "assignment is not an constant expression");
                error->code = 1;
                throw;
            }

            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = ASSIGNMENT_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = assignment_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;


            new_expression->type = type_copy(&new_expression->right->type);

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }
    return p_expression_node;
}

struct expression* expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*expression:
      assignment-expression
      expression, assignment-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = assignment_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        struct expression* p_expression_node_tail = p_expression_node;
        while (ctx->current->type == ',')
        {
            parser_match(ctx);
            p_expression_node_tail->right = expression(ctx, error, ectx);
            if (error->code != 0)
                throw;
            p_expression_node_tail = p_expression_node_tail->right;
        }
    }
    catch
    {
    }
    return p_expression_node;
}

bool is_first_of_conditional_expression(struct parser_ctx* ctx)
{
    return is_first_of_unary_expression(ctx) ||
        is_first_of_primary_expression(ctx);
}

struct expression* conditional_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
      conditional-expression:
      logical-OR-expression
      logical-OR-expression ? expression : conditional-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = logical_or_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;


        if (ctx->current && ctx->current->type == '?')
        {
            //TODO VERIFICAR
            parser_match(ctx);
            if (p_expression_node->constant_value)
            {
                p_expression_node->left = expression(ctx, error, ectx);
                if (error->code != 0)
                    throw;

                parser_match(ctx); //:
                struct expression_ctx temp = { 0 };
                conditional_expression(ctx, error, &temp);
            }
            else
            {
                struct expression_ctx temp = { 0 };
                p_expression_node->left = expression(ctx, error, &temp);
                if (error->code != 0)
                    throw;

                parser_match(ctx); //:
                p_expression_node->right = conditional_expression(ctx, error, ectx);
                if (error->code != 0)
                    throw;
            }
        }
    }
    catch
    {
    }
    return p_expression_node;
}

struct expression* constant_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    ectx->bConstantExpressionRequired = true;
    return conditional_expression(ctx, error, ectx);
}


#ifdef TEST
#include "unit_test.h"

struct type type_make_using_string(const char* expr)
{
    struct error error = { 0 };

    struct token_list list = tokenizer(expr, "", 0, TK_FLAG_NONE, &error);
    struct preprocessor_ctx preprocessor_ctx = { 0 };
    struct token_list input = preprocessor(&preprocessor_ctx, &list, 0, &error);


    struct scope file_scope = { 0 };
    struct parser_ctx parser_ctx = { 0 };
    scope_list_push(&parser_ctx.scopes, &file_scope);
    parser_ctx.input_list = input;
    parser_ctx.current = parser_ctx.input_list.head;
    parser_skip_blanks(&parser_ctx);

    struct expression_ctx expression_ctx = { .bConstantExpressionRequired = true };
    struct expression* expression = conditional_expression(&parser_ctx, &error, &expression_ctx);
    assert(error.code == 0);
    return expression->type;
}

int test_constant_expression(const char* expr, int result)
{
    struct error error = { 0 };

    struct token_list list = tokenizer(expr, "", 0, TK_FLAG_NONE, &error);
    struct preprocessor_ctx preprocessor_ctx = { 0 };
    struct token_list input = preprocessor(&preprocessor_ctx, &list, 0, &error);


    struct scope file_scope = { 0 };
    struct parser_ctx parser_ctx = { 0 };
    scope_list_push(&parser_ctx.scopes, &file_scope);
    parser_ctx.input_list = input;
    parser_ctx.current = parser_ctx.input_list.head;
    parser_skip_blanks(&parser_ctx);

    struct expression_ctx expression_ctx = { .bConstantExpressionRequired = true };
    struct expression* expression = constant_expression(&parser_ctx, &error, &expression_ctx);

    return expression->constant_value == result ? 0 : 1;
}

void test_compiler_constant_expression()
{
    assert(test_constant_expression("sizeof(char)", sizeof(char)) == 0);
    assert(test_constant_expression("'A'", 'A') == 0);
    assert(test_constant_expression("'ab'", 'ab') == 0);
    assert(test_constant_expression("true", true) == 0);
    assert(test_constant_expression("false", false) == 0);
    assert(test_constant_expression("10+2*3/4", 10 + 2 * 3 / 4) == 0);
    assert(test_constant_expression("sizeof(int)", sizeof(int)) == 0);
    assert(test_constant_expression("sizeof(double)", sizeof(double)) == 0);
    assert(test_constant_expression("sizeof(unsigned long)", sizeof(unsigned long)) == 0);
}

void is_pointer_test()
{
    struct type t1 = type_make_using_string("(int *)0");
    assert(type_is_pointer(&t1));

    struct type t2 = type_make_using_string("1");
    assert(type_is_integer(&t2));

    struct type t3 = type_make_using_string("(void (*)(void))0");
    assert(type_is_pointer(&t3));
}

void sizeoftest1()
{
    const char* source =
        "char a[10];"
        "static_assert(sizeof(a) == 10);"
        "char *p[10];"
        "static_assert(sizeof(p) == 40);"
        "static_assert(sizeof(int) == 4);"
        "static_assert(sizeof(long) == 4);"
        "static_assert(sizeof(char) == 1);"
        "static_assert(sizeof(short) == 4);"
        "static_assert(sizeof(unsigned int) == 4);"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}


void sizeof_struct_test()
{
    const char* source =
        "struct X { int i; char c; };"
        "_Static_assert(sizeof(struct X) == 4);"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code != 0);
}

static int expression_type(const char* expression, const char* result)
{
    char source[200] = { 0 };
    snprintf(source, sizeof source, "_Static_assert(typeid(%s) == typeid(%s));", expression, result);
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    return error.code;
}

void test_expressions()
{
    const char* source =
        "\n"
        "\n"
        "struct X {\n"
        "    int i;\n"
        "};\n"
        "\n"
        "struct Y {\n"
        "    double d;\n"
        "};\n"
        "\n"
        "enum E { A = 1 };\n"
        "enum E e1;\n"
        "struct X* F() { return 0; }\n"
        "\n"
        "int main()\n"
        "{\n"
        "    enum E { B } e2; \n"
        "    static_assert(typeid(e2) == typeid(enum E));\n"
        "    static_assert(typeid(e2) != typeid(e1));\n"
        "\n"
        "    struct X x;\n"
        "    struct Y y;\n"
        "\n"
        "    static_assert(typeid(x) == typeid(struct X));\n"
        "    static_assert(typeid(x) != typeid(struct Y));\n"
        "\n"
        "    static_assert(typeid(int(double)) != typeid(int()));\n"
        "    int aa[10];\n"
        "\n"
        "    static_assert(typeid(*F()) == typeid(struct X));\n"
        "    static_assert(typeid(&aa) == typeid(int(*)[10]));\n"
        "\n"
        "    int* p = 0;\n"
        "    static_assert(typeid(*(p + 1)) == typeid(int));\n"
        "    \n"
        "    static_assert(1 == typeid(int));\n"
        "\n"
        "    static_assert(typeid(main) == typeid(int()));\n"
        "    \n"
        "    \n"
        "    static_assert(typeid(main) != typeid(int(double)));\n"
        "    static_assert(typeid(main) != typeid(int));\n"
        "\n"
        "\n"
        "    struct X x;\n"
        "    enum E e;\n"
        "    static_assert(typeid(e) == typeid(enum E));\n"
        "    static_assert(typeid(x) == typeid(struct X));\n"
        "    static_assert(typeid(e) != typeid(struct X));\n"
        "\n"
        "    \n"
        "        \n"
        "    static_assert(1L == typeid(long));\n"
        "    static_assert(1UL == typeid(unsigned long));\n"
        "    static_assert(1ULL == typeid(unsigned long long));\n"
        "    static_assert(A == typeid(int));\n"
        "    static_assert(1.0 == typeid(double));\n"
        "    static_assert(1.0f == typeid(float));\n"
        "    static_assert(1.0L == typeid(long double));\n"
        "\n"
        "    static_assert(typeid((((int*)0) + 1))  == typeid(int*));\n"
        "    static_assert(typeid(*(((int*)0) + 1))  == typeid(int));\n"
        "  \n"
        "}\n"
        "\n"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);    
}

void literal_string_type()
{
    const char* source =
        "\n"
        "    static_assert(typeid(\"A\") == typeid(const char [2]);\n"
        "    static_assert(typeid(\"A\\n\") != typeid(const char [3]));\n"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}
void type_suffix_test()
{

    const char* source =
        "_Static_assert("
        "1 == typeid(int) && "
        "1L == typeid(long) && "
        "1LL == typeid(long long)  && "
        "1U == typeid(unsigned int) && "
        "1ULL == typeid(unsigned long long) &&"
        "1 == typeid(int) && "
        "1l == typeid(long) && "
        "1ll == typeid(long long)  && "
        "1u == typeid(unsigned int) && "
        "1ull == typeid(unsigned long long) && "
        "0x1 == typeid(int) && "
        "0x1L == typeid(long) && "
        "0x1LL == typeid(long long)  && "
        "0x1U == typeid(unsigned int) && "
        "0x1ULL == typeid(unsigned long long) &&"
        "0x1 == typeid(int)  &&"
        "0x1l == typeid(long)  && "
        "0x1ll == typeid(long long)  && "
        "0x1u == typeid(unsigned int) && "
        "0x1ull == typeid(unsigned long long) &&"
        "0b1 == typeid(int)  &&"
        "0b1L == typeid(long) && "
        "0b1LL == typeid(long long) &&  "
        "0b1U == typeid(unsigned int) && "
        "0b1ULL == typeid(unsigned long long) &&"
        "0b1l == typeid(long) && "
        "0b1ll == typeid(long long)   &&"
        "0b1ul == typeid(unsigned long)  &&"
        "0b1ull == typeid(unsigned long long) && "
        "1.0f == typeid(float) && "
        "1.0 == typeid(double) "        
        ");"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void type_test()
{
    const char* source =
        "int * p = 0;"
        "_Static_assert(typeid(*(p + 1)) == typeid(int));"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void digit_separator_test()
{
    const char* source =
        "_Static_assert(1'00'00 == 10000);"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void numbers_test()
{
    const char* source =
        "#if 0xA1 == 161\n"
        "_Static_assert(0xA1 == 161); \n"
        "#endif"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void binary_digits_test()
{
    const char* source =
        "_Static_assert(0b101010 == 42);"
        "_Static_assert(0b1010'10 == 42);"
        "_Static_assert(052 == 42);"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void is_arithmetic_test()
{
    const char* source =
        "int (*d1)(int i)[10];"
        "int d2;"
        "long double d3;"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);

    struct declarator* d1 = ast.declaration_list.head->init_declarator_list.head->declarator;
    struct declarator* d2 = ast.declaration_list.head->next->init_declarator_list.head->declarator;
    struct declarator* d3 = ast.declaration_list.head->next->next->init_declarator_list.head->declarator;



    assert(!type_is_array(&d1->type));
    assert(type_is_pointer(&d1->type));
    assert(type_is_scalar(&d1->type));
    assert(!type_is_integer(&d1->type));

    assert(!type_is_array(&d2->type));
    assert(!type_is_pointer(&d2->type));
    assert(type_is_arithmetic(&d2->type));
    assert(type_is_integer(&d2->type));

    assert(!type_is_integer(&d3->type));

}

void type_is_pointer_test()
{
    const char* source =
        "const char* d1[10];\n"
        "char* (*f)(void);\n"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);

    struct declarator* d1 = ast.declaration_list.head->init_declarator_list.head->declarator;
    struct declarator* d2 = ast.declaration_list.head->next->init_declarator_list.head->declarator;

    assert(!type_is_pointer(&d1->type));
    assert(type_is_array(&d1->type));

    assert(type_is_pointer(&d2->type));
    assert(type_is_function_or_function_pointer(&d2->type));

}

void params_test()
{
    const char* source =
        "void f1();"
        "void f2(void);"
        "void f3(char * s, ...);"
        "int main()"
        "{"
        "  f1();"
        "  f2();"
        "  f3(\"\");"
        "  f3(\"\", 1, 2, 3);"
        "}"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}
#endif
