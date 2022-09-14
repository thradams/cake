#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "visit.h"
#include "expressions.h"



struct token* next_parser_token(struct token* token)
{
    struct token* r = token->next;
    while (!(r->flags & TK_FLAG_FINAL))
    {
        r = r->next;
    }
    return r;
}

struct defer_scope* get_last_defer_before_try(struct defer_scope* block)
{
    struct defer_scope* p_defer = block;
    while (p_defer)
    {
        if (p_defer->lastchild != NULL)
        {
            /*achou*/
            return p_defer->lastchild;
        }

        if (p_defer->p_selection_statement2 &&
            p_defer->p_selection_statement2->first_token->type == TK_KEYWORD_TRY)
        {
            /*chegamos no try sem achar nenhum defer*/
            break;
        }

        p_defer = p_defer->previous;
    }

    return NULL;
}

struct defer_scope* get_last_defer_before_try_up(struct defer_scope* block)
{
    struct defer_scope* p_defer = block->previous;
    while (p_defer)
    {
        if (p_defer->lastchild != NULL)
        {
            /*achou*/
            return p_defer->lastchild;
        }

        if (p_defer->p_selection_statement2 &&
            p_defer->p_selection_statement2->first_token->type == TK_KEYWORD_TRY)
        {
            /*chegamos no try sem achar nenhum defer*/
            break;
        }

        p_defer = p_defer->previous;
    }

    return NULL;
}


static void visit_struct_or_union_specifier(struct visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier, struct error* error);
static void visit_expression(struct visit_ctx* ctx, struct expression* p_expression, struct error* error);
static void visit_statement(struct visit_ctx* ctx, struct statement* p_statement, struct error* error);
static void visit_enum_specifier(struct visit_ctx* ctx, struct enum_specifier* p_enum_specifier, struct error* error);
static void visit_type_specifier(struct visit_ctx* ctx, struct type_specifier* p_type_specifier, struct error* error);

void convert_if_statement(struct visit_ctx* ctx, struct selection_statement* p_selection_statement, struct error* error)
{
    if (p_selection_statement->init_declarator == NULL ||
        p_selection_statement->first_token->type != TK_KEYWORD_IF)
    {
        return;
    }

    struct token* before_first_token =
        p_selection_statement->first_token->prev;
    struct token* token =
        next_parser_token(p_selection_statement->first_token);
    token = next_parser_token(token);
    struct token* first = token;
    while (token)
    {
        if (token->type == ';')
            break;
        token = next_parser_token(token);
    }
    struct token_list list =
        token_list_remove(&ctx->ast.token_list, first, token);
    token_list_insert_after(&ctx->ast.token_list,
        before_first_token,
        &list);
    struct token_list list1 = tokenizer("{", "", 0, TK_FLAG_NONE, error);
    token_list_insert_after(&ctx->ast.token_list,
        before_first_token,
        &list1);
    struct token_list list2 = tokenizer("}", "", 0, TK_FLAG_NONE, error);
    token_list_insert_after(&ctx->ast.token_list,
        previous_parser_token(p_selection_statement->last_token),
        &list2);
}

void print_block_defer(struct defer_scope* deferblock, struct osstream* ss, bool bHide)
{
    struct defer_scope* deferchild = deferblock->lastchild;
    while (deferchild != NULL)
    {
        struct token_list l = { 0 };
        l.head = deferchild->defer_statement->firsttoken;
        l.tail = deferchild->defer_statement->lasttoken;

        l.head->flags |= TK_FLAG_HIDE;
        const char* s = get_code_as_compiler_see(&l);
        assert(s != NULL);
        if (bHide)
            token_range_add_flag(l.head, l.tail, TK_FLAG_HIDE);

        ss_fprintf(ss, "%s", s);
        free(s);
        deferchild = deferchild->previous;
    }
}


void hide_block_defer(struct defer_scope* deferblock)
{
    struct defer_scope* deferchild = deferblock->lastchild;
    while (deferchild != NULL)
    {
        struct token_list l = { 0 };
        l.head = deferchild->defer_statement->firsttoken;
        l.tail = deferchild->defer_statement->lasttoken;          
        token_range_add_flag(l.head, l.tail, TK_FLAG_HIDE);    
        deferchild = deferchild->previous;
    }
}


void print_all_defer_until_try(struct defer_scope* deferblock, struct osstream* ss)
{
    struct defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        print_block_defer(p_defer, ss, false);
        if (p_defer->p_selection_statement2 &&
            p_defer->p_selection_statement2->first_token->type == TK_KEYWORD_TRY)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

bool find_label_statement(struct statement* statement, const char* label);
bool find_label_unlabeled_statement(struct unlabeled_statement* p_unlabeled_statement, const char* label)
{
    if (p_unlabeled_statement->primary_block)
    {
        if (p_unlabeled_statement->primary_block->compound_statement)
        {            
            struct block_item* block_item =
                p_unlabeled_statement->primary_block->compound_statement->block_item_list.head;
            while (block_item)
            {
                if (block_item->label &&
                    strcmp(block_item->label->name->lexeme, label) == 0)
                {
                    /*achou*/
                    return true;
                }
                else if (block_item->unlabeled_statement)
                {
                    if (find_label_unlabeled_statement(block_item->unlabeled_statement, label))
                    {
                        return true;
                    }
                }
                else if (block_item->labeled_statement)
                {
                    if (find_label_statement(block_item->labeled_statement->statement, label))
                    {
                        return true;
                    }

                }
                block_item = block_item->next;
            }
        }
        else if (p_unlabeled_statement->primary_block->selection_statement)
        {            
            if (find_label_statement(p_unlabeled_statement->primary_block->selection_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->selection_statement->else_secondary_block)
            {
                if (find_label_statement(p_unlabeled_statement->primary_block->selection_statement->else_secondary_block->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->iteration_statement)
        {
            if (find_label_statement(p_unlabeled_statement->primary_block->iteration_statement->secondary_block->statement, label))
            {
                return true;
            }            
        }
    }
    return false;
}

bool find_label_statement(struct statement* statement, const char* label)
{
    if (statement->labeled_statement)
    {
        if (statement->labeled_statement &&
            statement->labeled_statement->label &&
            strcmp(statement->labeled_statement->label->name->lexeme, label) == 0)
        {
            /*achou*/
            return true;
        }
    }
    else if (statement->unlabeled_statement)
    {
        if (find_label_unlabeled_statement(statement->unlabeled_statement, label))
            return true;
    }
    return false;
}

static bool find_label_scope(struct defer_scope* deferblock, const char* label)
{
    if (deferblock->p_iteration_statement)
    {
        if (deferblock->p_iteration_statement->secondary_block)
        {
            if (find_label_statement(deferblock->p_iteration_statement->secondary_block->statement, label))
                return true;
        }
    }
    else if (deferblock->p_selection_statement2)
    {
        if (deferblock->p_selection_statement2->secondary_block)
        {
            if (find_label_statement(deferblock->p_selection_statement2->secondary_block->statement, label))
                return true;
        }
        
        if (deferblock->p_selection_statement2->else_secondary_block)
        {
            if (find_label_statement(deferblock->p_selection_statement2->else_secondary_block->statement, label))
                return true;
        }
    }
    else if (deferblock->p_statement)
    {
        if (find_label_statement(deferblock->p_statement, label))
            return true;
    }
    return false;
}

void print_all_defer_until_label(struct defer_scope* deferblock, const char* label, struct osstream* ss)
{
    /*
    * Precisamos saber quantos escopos nós saimos até achar o label.
    * Para isso procuramos no escopo atual aonde aparede o goto.
    * Se o label não esta diretamente neste escopo ou dentro de algum escopo interno
    * Não nós imprimos os defers pois estamos saindo do escopo e vamos para o escopo
    * de cima. Assim vamos repetindo em cada saida de escopo imprimos o defer.
    */
    struct defer_scope* p_defer = deferblock;

    while (p_defer != NULL)
    {
        if (!find_label_scope(p_defer, label))
        {
            print_block_defer(p_defer, ss, false);            
        }
        else
        {
            break;
        }
        p_defer = p_defer->previous;
    }
    
}

void print_all_defer_until_iter(struct defer_scope* deferblock, struct osstream* ss)
{
    struct defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        print_block_defer(p_defer, ss, false);
        if (p_defer->p_iteration_statement)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

void print_all_defer_until_end(struct defer_scope* deferblock, struct osstream* ss)
{
    struct defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        print_block_defer(p_defer, ss, false);
        p_defer = p_defer->previous;
    }
}

static void visit_secondary_block(struct visit_ctx* ctx, struct secondary_block* p_secondary_block, struct error* error)
{
    visit_statement(ctx, p_secondary_block->statement, error);
}

static void visit_defer_statement(struct visit_ctx* ctx, struct defer_statement* p_defer_statement, struct error* error)
{


    if (ctx->lambda_step == 1)
    {

        //adiciona como filho do ultimo bloco
        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->defer_statement = p_defer_statement;


        p_defer->previous = ctx->tail_block->lastchild;
        ctx->tail_block->lastchild = p_defer;


        if (p_defer_statement->secondary_block)
        {
            ctx->bInsideDefer = true;
            visit_secondary_block(ctx, p_defer_statement->secondary_block, error);
            ctx->bInsideDefer = false;
        }
    }
    else if (ctx->lambda_step == 2)
    {
        if (p_defer_statement->secondary_block)
            visit_secondary_block(ctx, p_defer_statement->secondary_block, error);
    }
}

static void visit_selection_statement(struct visit_ctx* ctx, struct selection_statement* p_selection_statement, struct error* error)
{
    convert_if_statement(ctx, p_selection_statement, error);

    if (p_selection_statement->first_token->type == TK_KEYWORD_TRY &&
        ctx->lambda_step == 1)
    {


        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->previous = ctx->tail_block;
        ctx->tail_block = p_defer;
        p_defer->p_selection_statement2 = p_selection_statement;

        if (p_selection_statement->secondary_block)
            visit_secondary_block(ctx, p_selection_statement->secondary_block, error);


        struct osstream ss = { 0 };
        //defer_print(defer, &ss, ctx->bHasThrowWithVariable, ctx->bHasBreakWithVariable, ctx->bHasReturnWithVariable);
        print_block_defer(p_defer, &ss, true);
        struct token_list l = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);
        token_list_insert_after(&ctx->ast.token_list, p_selection_statement->secondary_block->last->prev, &l);


        if (ctx->tail_block)
        {
            //POP
            ctx->tail_block = ctx->tail_block->previous;
        }

        free(p_selection_statement->first_token->lexeme);

        p_selection_statement->first_token->lexeme = strdup("if (1) /*try*/");


        char buffer[50] = { 0 };
        if (p_selection_statement->else_secondary_block)
        {

            snprintf(buffer, sizeof buffer, "else _catch_label_%d:", p_selection_statement->try_catch_block_index);

            free(p_selection_statement->else_catch_token->lexeme);
            p_selection_statement->else_catch_token->lexeme = strdup(buffer);

            visit_secondary_block(ctx, p_selection_statement->else_secondary_block, error);
        }
        else
        {


            snprintf(buffer, sizeof buffer, "%s _catch_label_%d:",
                p_selection_statement->else_catch_token->lexeme,
                p_selection_statement->try_catch_block_index
            );

            free(p_selection_statement->else_catch_token->lexeme);
            p_selection_statement->else_catch_token->lexeme = strdup(buffer);
        }
    }
    else
    {
        //PUSH
        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->previous = ctx->tail_block;
        ctx->tail_block = p_defer;
        p_defer->p_selection_statement2 = p_selection_statement;

        if (p_selection_statement->secondary_block)
            visit_secondary_block(ctx, p_selection_statement->secondary_block, error);

        struct osstream ss = { 0 };
        //defer_print(defer, &ss, ctx->bHasThrowWithVariable, ctx->bHasBreakWithVariable, ctx->bHasReturnWithVariable);
        print_block_defer(p_defer, &ss, true);

        //if (ctx->bHasThrowWithVariable)
        //{
            //se chegou no fim do scopo do try devido a um jump do throw
            //entao pulamos para o catch
            //if (defer->lastchild)
            //{
              //  ss_fprintf(&ss, "\n/*if block exit*/ if (_up) goto _catch_label_%d;\n", p_selection_statement->try_catch_block_index);
            //}
        //}
        if (ss.size > 0)
        {
            struct token_list l = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);
            token_list_insert_after(&ctx->ast.token_list, p_selection_statement->secondary_block->last->prev, &l);
        }
        //POP
        ctx->tail_block = ctx->tail_block->previous;


        if (p_selection_statement->else_secondary_block)
            visit_secondary_block(ctx, p_selection_statement->else_secondary_block, error);
    }
}

static void visit_compound_statement(struct visit_ctx* ctx, struct compound_statement* p_compound_statement, struct error* error);



char* remove_char(char* input, char ch)
{
    if (input == NULL)
        return NULL;
    char* pWrite = input;
    const char* p = input;
    while (*p)
    {
        if (p[0] == ch)
        {
            p++;
        }
        else
        {
            *pWrite = *p;
            p++;
            pWrite++;
        }
    }
    *pWrite = 0;
    return input;
}

static void visit_initializer_list(struct visit_ctx* ctx, struct initializer_list* p_initializer_list, struct error* error);

static void visit_bracket_initializer_list(struct visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list, struct error* error)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {
        if (ctx->target < LANGUAGE_C2X)
        {
            assert(p_bracket_initializer_list->first->type == '{');

            //Criar token 0
            struct token_list list2 = tokenizer("0", NULL, 0, TK_FLAG_NONE, error);

            //inserir na frente
            token_list_insert_after(&ctx->ast.token_list, p_bracket_initializer_list->first, &list2);
        }
    }
    else
    {
        visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list, error);
    }
}

static void visit_designation(struct visit_ctx* ctx, struct designation* p_designation, struct error* error)
{
}

static void visit_initializer(struct visit_ctx* ctx, struct initializer* p_initializer, struct error* error)
{
    if (p_initializer->designation)
    {
        visit_designation(ctx, p_initializer->designation, error);
    }

    if (p_initializer->assignment_expression)
    {
        visit_expression(ctx, p_initializer->assignment_expression, error);
    }
    else if (p_initializer->braced_initializer)
    {
        visit_bracket_initializer_list(ctx, p_initializer->braced_initializer, error);
    }
}

static void visit_initializer_list(struct visit_ctx* ctx, struct initializer_list* p_initializer_list, struct error* error)
{
    struct initializer* p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        visit_initializer(ctx, p_initializer, error);
        if (error->code != 0)
            break;
        p_initializer = p_initializer->next;
    }
}

static void visit_type_qualifier(struct visit_ctx* ctx, struct type_qualifier* p_type_qualifier, struct error* error)
{
    //p_type_qualifier->
}

static void visit_specifier_qualifier(struct visit_ctx* ctx, struct specifier_qualifier* p_specifier_qualifier, struct error* error)
{
    if (p_specifier_qualifier->type_specifier)
        visit_type_specifier(ctx, p_specifier_qualifier->type_specifier, error);

    if (p_specifier_qualifier->type_qualifier)
        visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier, error);
}

static void visit_specifier_qualifier_list(struct visit_ctx* ctx, struct specifier_qualifier_list* p_specifier_qualifier_list_opt, struct error* error)
{
    if (p_specifier_qualifier_list_opt == NULL)
        return;

    if (p_specifier_qualifier_list_opt->struct_or_union_specifier)
    {
        visit_struct_or_union_specifier(ctx, p_specifier_qualifier_list_opt->struct_or_union_specifier, error);
    }
    else if (p_specifier_qualifier_list_opt->enum_specifier)
    {
        visit_enum_specifier(ctx, p_specifier_qualifier_list_opt->enum_specifier, error);
    }
    else if (p_specifier_qualifier_list_opt->typedef_declarator)
    {
        //typedef name
    }
    //else if (p_specifier_qualifier_list->p_typeof_expression_opt)
    //{
      //  visit_expression(ctx, p_specifier_qualifier_list->p_typeof_expression_opt, error);
    //}
    else
    {
        struct specifier_qualifier* p_specifier_qualifier = p_specifier_qualifier_list_opt->head;
        while (p_specifier_qualifier)
        {
            visit_specifier_qualifier(ctx, p_specifier_qualifier, error);
            p_specifier_qualifier = p_specifier_qualifier->next;
        }
    }
}

static void visit_type_name(struct visit_ctx* ctx, struct type_name* p_type_name, struct error* error)
{
    visit_specifier_qualifier_list(ctx, p_type_name->specifier_qualifier_list, error);
    //visit_declarator
}

#pragma warning(default : 4061)


static void visit_argument_expression_list(struct visit_ctx* ctx, struct argument_expression_list* p_argument_expression_list, struct error* error)
{
    struct argument_expression* p_argument_expression =
        p_argument_expression_list->head;
    while (p_argument_expression)
    {
        visit_expression(ctx, p_argument_expression->expression, error);
        p_argument_expression = p_argument_expression->next;
    }
}

static void visit_generic_selection(struct visit_ctx* ctx, struct generic_selection* p_generic_selection, struct error* error)
{
    visit_expression(ctx, p_generic_selection->expression, error);
    if (ctx->target < LANGUAGE_C11)
    {
        token_range_add_flag(p_generic_selection->first_token, p_generic_selection->last_token, TK_FLAG_HIDE);
        if (p_generic_selection->p_view_selected_expression)
        {
            token_range_remove_flag(p_generic_selection->p_view_selected_expression->first,
                p_generic_selection->p_view_selected_expression->last,
                TK_FLAG_HIDE);
        }
    }

}


static void visit_expression(struct visit_ctx* ctx, struct expression* p_expression, struct error* error)
{
    switch (p_expression->expression_type)
    {
    case PRIMARY_IDENTIFIER:
        break;
    case TYPEID_EXPRESSION_TYPE:
        break;
    case PRIMARY_EXPRESSION_ENUMERATOR:
        break;
    case PRIMARY_EXPRESSION_DECLARATOR:
        //p_expression->declarator->name
    {
    }
    break;
    case PRIMARY_EXPRESSION_STRING_LITERAL:
        break;
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
        break;
    case PRIMARY_EXPRESSION_NUMBER:
        break;

    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        visit_generic_selection(ctx, p_expression->generic_selection, error);
        break;

    case POSTFIX_DOT:
        break;
    case POSTFIX_ARROW:
        break;
    case POSTFIX_INCREMENT:
        break;
    case POSTFIX_DECREMENT:
        break;
    case POSTFIX_ARRAY:
        //visit_expression(ctx, p_expression->left, error);
        break;
    case POSTFIX_FUNCTION_CALL:
        visit_expression(ctx, p_expression->left, error);
        visit_argument_expression_list(ctx, &p_expression->argument_expression_list, error);
        break;
    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
    {
        /*
          Indica que foi encontrado expressões lambda/function literal
          no corpo da funcao.
        */
        ctx->bHasLambda = true;
        ctx->bInsideLambda = true;
        visit_compound_statement(ctx, p_expression->compound_statement, error);
        ctx->bInsideLambda = false;

        if (ctx->lambda_step == 2)
        {
            /*no segundo passo nós copiamos a funcao*/
            char name[100] = { 0 };
            snprintf(name, sizeof name, " _lit_func_%d", ctx->lambdasindex);
            ctx->lambdasindex++;

            struct osstream ss = { 0 };

            bool bFirst = true;
            print_specifier_qualifier_list(&ss, &bFirst, p_expression->type_name->specifier_qualifier_list);
            ss_fprintf(&ss, "%s", name);
            print_declarator(&ss, p_expression->type_name->declarator, false);

            struct token_list l1 = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);

            token_list_append_list(&ctx->insert_before_declaration, &l1);
            ss_close(&ss);

            for (struct token* current = p_expression->compound_statement->first_token;
                current != p_expression->compound_statement->last_token->next;
                current = current->next)
            {
                token_list_clone_and_add(&ctx->insert_before_declaration, current);
            }

            token_range_add_flag(p_expression->first, p_expression->last, TK_FLAG_HIDE);

            struct token_list l3 = tokenizer("\n\n", NULL, 0, TK_FLAG_NONE, error);
            token_list_append_list(&ctx->insert_before_declaration, &l3);


            struct token_list l2 = tokenizer(name, NULL, 0, TK_FLAG_FINAL, error);
            token_list_insert_after(&ctx->ast.token_list, p_expression->last, &l2);
        }
    }
    break;

    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:
        visit_bracket_initializer_list(ctx, p_expression->braced_initializer, error);


        if (p_expression->left)
        {
            visit_expression(ctx, p_expression->left, error);
        }
        if (p_expression->right)
        {
            visit_expression(ctx, p_expression->right, error);
        }
        if (p_expression->type_name)
        {
            visit_type_name(ctx, p_expression->type_name, error);
        }
        break;

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:
    case UNARY_EXPRESSION_SIZEOF_TYPE:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:
        break;

    case UNARY_EXPRESSION_HASHOF_TYPE:

        token_range_add_flag(p_expression->first, p_expression->last, TK_FLAG_HIDE);

        char buffer[30] = { 0 };
        snprintf(buffer, sizeof buffer, "%lld", p_expression->constant_value);

        struct token_list l3 = tokenizer(buffer, NULL, 0, TK_FLAG_NONE, error);
        token_list_insert_after(&ctx->ast.token_list, p_expression->last, &l3);

        break;

    case UNARY_EXPRESSION_ALIGNOF:
    case CAST_EXPRESSION:
    case ASSIGNMENT_EXPRESSION:
    case MULTIPLICATIVE_EXPRESSION_MULT:
    case MULTIPLICATIVE_EXPRESSION_DIV:
    case MULTIPLICATIVE_EXPRESSION_MOD:
    case ADDITIVE_EXPRESSION_PLUS:
    case ADDITIVE_EXPRESSION_MINUS:
    case SHIFT_EXPRESSION_RIGHT:
    case SHIFT_EXPRESSION_LEFT:
    case RELATIONAL_EXPRESSION_BIGGER_THAN:
    case RELATIONAL_EXPRESSION_LESS_THAN:
    case EQUALITY_EXPRESSION_EQUAL:
    case EQUALITY_EXPRESSION_NOT_EQUAL:
    case AND_EXPRESSION:
    case EXCLUSIVE_OR_EXPRESSION:
    case INCLUSIVE_OR_EXPRESSION:
    case INCLUSIVE_AND_EXPRESSION:
    case LOGICAL_OR_EXPRESSION:
    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:

        if (p_expression->left)
        {
            visit_expression(ctx, p_expression->left, error);
        }
        if (p_expression->right)
        {
            visit_expression(ctx, p_expression->right, error);
        }
        if (p_expression->type_name)
        {
            visit_type_name(ctx, p_expression->type_name, error);
        }
        break;
    default:
        break;
    }
}

static void visit_expression_statement(struct visit_ctx* ctx, struct expression_statement* p_expression_statement, struct error* error)
{
    if (p_expression_statement->expression)
        visit_expression(ctx, p_expression_statement->expression, error);
}

static void visit_block_item_list(struct visit_ctx* ctx, struct block_item_list* p_block_item_list, struct error* error);

static void visit_compound_statement(struct visit_ctx* ctx, struct compound_statement* p_compound_statement, struct error* error)
{
    ctx->bInsideCompoundStatement = true;
    
    visit_block_item_list(ctx, &p_compound_statement->block_item_list, error);
    
    ctx->bInsideCompoundStatement = false;
}

//
static void visit_iteration_statement(struct visit_ctx* ctx, struct iteration_statement* p_iteration_statement, struct error* error)
{

    if (p_iteration_statement->expression1)
    {
        visit_expression(ctx, p_iteration_statement->expression1, error);
    }

    if (p_iteration_statement->expression2)
    {
        visit_expression(ctx, p_iteration_statement->expression2, error);
    }

    if (p_iteration_statement->token->type == TK_KEYWORD_REPEAT)
    {
        free(p_iteration_statement->token->lexeme);
        p_iteration_statement->token->lexeme = strdup("for(;;)/*repeat*/");
    }

    if (p_iteration_statement->secondary_block)
    {
        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->previous = ctx->tail_block;
        ctx->tail_block = p_defer;
        p_defer->p_iteration_statement = p_iteration_statement;

        visit_secondary_block(ctx, p_iteration_statement->secondary_block, error);

        struct osstream ss = { 0 };
        //defer_print(defer, &ss, ctx->bHasThrowWithVariable, ctx->bHasBreakWithVariable, ctx->bHasReturnWithVariable);
        print_block_defer(p_defer, &ss, true);

        struct token_list l = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);
        token_list_insert_after(&ctx->ast.token_list, p_iteration_statement->secondary_block->last->prev, &l);


        if (ctx->tail_block)
        {
            //POP
            ctx->tail_block = ctx->tail_block->previous;
        }

    }
}



static void visit_jump_statement(struct visit_ctx* ctx, struct jump_statement* p_jump_statement, struct error* error)
{

    if (p_jump_statement->token->type == TK_KEYWORD_THROW)
    {
        struct osstream ss0 = { 0 };
        print_all_defer_until_try(ctx->tail_block, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "goto _catch_label_%d;", p_jump_statement->try_catch_block_index);
            ss_fprintf(&ss, "}");
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;
            p_jump_statement->lasttoken->flags |= TK_FLAG_HIDE;
        }
        else
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "goto _catch_label_%d", p_jump_statement->try_catch_block_index);
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;
        }
    }
    else if (p_jump_statement->token->type == TK_KEYWORD_RETURN)
    {
        struct osstream ss0 = { 0 };
        print_all_defer_until_end(ctx->tail_block, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "return");
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;
            free(p_jump_statement->lasttoken->lexeme);
            p_jump_statement->lasttoken->lexeme = strdup(";}");
        }
    }
    else if (p_jump_statement->token->type == TK_KEYWORD_BREAK ||
             p_jump_statement->token->type == TK_KEYWORD_CONTINUE)
    {
        struct osstream ss0 = { 0 };

        print_all_defer_until_iter(ctx->tail_block, &ss0);
        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "break;");
            ss_fprintf(&ss, "}");
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;  /*MOVED*/
            p_jump_statement->lasttoken->flags |= TK_FLAG_HIDE;
        }
    }
    else if (p_jump_statement->token->type == TK_KEYWORD_GOTO)
    {
        struct osstream ss0 = { 0 };
        print_all_defer_until_label(ctx->tail_block, p_jump_statement->label->lexeme, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "goto");
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;
            free(p_jump_statement->lasttoken->lexeme);
            p_jump_statement->lasttoken->lexeme = strdup(";}");
        }
    }
    else
    {
        assert(false);
    }
}


static void visit_labeled_statement(struct visit_ctx* ctx, struct labeled_statement* p_labeled_statement, struct error* error)
{
    //p_labeled_statement->label

    if (p_labeled_statement->statement)
    {
        visit_statement(ctx, p_labeled_statement->statement, error);
    }
}

static void visit_primary_block(struct visit_ctx* ctx, struct primary_block* p_primary_block, struct error* error)
{



    if (p_primary_block->defer_statement)
    {
        visit_defer_statement(ctx, p_primary_block->defer_statement, error);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            visit_compound_statement(ctx, p_primary_block->compound_statement, error);
        }
        else if (p_primary_block->iteration_statement)
        {
            visit_iteration_statement(ctx, p_primary_block->iteration_statement, error);
        }
        else if (p_primary_block->selection_statement)
        {
            visit_selection_statement(ctx, p_primary_block->selection_statement, error);
        }
    }

}

static void visit_unlabeled_statement(struct visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, struct error* error)
{
    if (p_unlabeled_statement->primary_block)
    {
        visit_primary_block(ctx, p_unlabeled_statement->primary_block, error);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        visit_expression_statement(ctx, p_unlabeled_statement->expression_statement, error);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        visit_jump_statement(ctx, p_unlabeled_statement->jump_statement, error);
    }
    else
    {
        assert(false);
    }
}

static void visit_declaration(struct visit_ctx* ctx, struct declaration* p_declaration, struct error* error);

static void visit_statement(struct visit_ctx* ctx, struct statement* p_statement, struct error* error)
{
    if (p_statement->labeled_statement)
    {
        visit_labeled_statement(ctx, p_statement->labeled_statement, error);
    }
    else if (p_statement->unlabeled_statement)
    {
        visit_unlabeled_statement(ctx, p_statement->unlabeled_statement, error);
    }
}


static void visit_block_item(struct visit_ctx* ctx, struct block_item* p_block_item, struct error* error)
{
    if (p_block_item->declaration)
    {
        visit_declaration(ctx, p_block_item->declaration, error);
    }
    else if (p_block_item->unlabeled_statement)
    {
        visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement, error);
    }
    else if (p_block_item->labeled_statement)
    {
        visit_labeled_statement(ctx, p_block_item->labeled_statement, error);
    }
    if (ctx->insert_before_block_item.head != NULL)
    {
        //token_list_insert_after(&ctx->ast.token_list, p_statement->first_token->prev, &ctx->insert_before_statement);
        token_list_insert_after(&ctx->ast.token_list, p_block_item->first_token->prev, &ctx->insert_before_block_item);
        ctx->insert_before_block_item.head = NULL;
        ctx->insert_before_block_item.tail = NULL;
    }
}

static void visit_block_item_list(struct visit_ctx* ctx, struct block_item_list* p_block_item_list, struct error* error)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (error->code == 0 && p_block_item)
    {
        visit_block_item(ctx, p_block_item, error);
        p_block_item = p_block_item->next;
    }
}

static void visit_static_assert_declaration(struct visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration, struct error* error)
{
    visit_expression(ctx, p_static_assert_declaration->constant_expression, error);

    if (ctx->target < LANGUAGE_C11)
    {
        /*
        * Vamos apagar a parte do static assert. Não adianta so commentar
        * pq poderia ter um comentario dentro. (so se verificar que nao tem)
        */
        for (struct token* p = p_static_assert_declaration->first_token;
            p != p_static_assert_declaration->last_token->next;
            p = p->next)
        {
            /*
             Se eu colocar como sendo macro expanded vai esconder
            */
            p->flags |= TK_FLAG_MACRO_EXPANDED;
        }
    }
    else if (ctx->target == LANGUAGE_C11)
    {
        if (p_static_assert_declaration->string_literal_opt == NULL)
        {
            struct token* rp = previous_parser_token(p_static_assert_declaration->last_token);
            rp = previous_parser_token(rp);

            struct token_list list1 = tokenizer(", \"error\"", "", 0, TK_FLAG_NONE, error);
            token_list_insert_after(&ctx->ast.token_list, rp, &list1);
        }
        if (strcmp(p_static_assert_declaration->first_token->lexeme, "static_assert") == 0)
        {
            /*C23 has static_assert but C11 _Static_assert*/
            free(p_static_assert_declaration->first_token->lexeme);
            p_static_assert_declaration->first_token->lexeme = strdup("_Static_assert");
        }
    }
}


static void visit_init_declarator_list(struct visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list, struct error* error)
{
    struct init_declarator* p_init_declarator = p_init_declarator_list->head;


    while (p_init_declarator)
    {
        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                visit_expression(ctx, p_init_declarator->initializer->assignment_expression, error);
            }
            else
            {
                assert(p_init_declarator->initializer->braced_initializer != NULL);
                if (p_init_declarator->initializer->braced_initializer)
                {
                    visit_bracket_initializer_list(ctx,
                        p_init_declarator->initializer->braced_initializer, error);
                }

            }
        }

        p_init_declarator = p_init_declarator->next;
    }
}


static void visit_member_declarator(struct visit_ctx* ctx, struct member_declarator* p_member_declarator, struct error* error)
{
    if (p_member_declarator->declarator)
    {

    }
}

static void visit_member_declarator_list(struct visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list, struct error* error)
{
    struct member_declarator* p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        visit_member_declarator(ctx, p_member_declarator, error);
        p_member_declarator = p_member_declarator->next;
    }
}
static void visit_member_declaration(struct visit_ctx* ctx, struct member_declaration* p_member_declaration, struct error* error)
{
    //p_member_declaration->specifier_qualifier_list        

    visit_specifier_qualifier_list(ctx, p_member_declaration->specifier_qualifier_list, error);

    if (p_member_declaration->member_declarator_list_opt)
    {
        visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt, error);
    }
}

static void visit_member_declaration_list(struct visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list, struct error* error)
{
    struct member_declaration* p_member_declaration =
        p_member_declaration_list->head;
    while (p_member_declaration)
    {
        visit_member_declaration(ctx, p_member_declaration, error);
        p_member_declaration = p_member_declaration->next;
    }
}

static void visit_attribute_specifier(struct visit_ctx* ctx, struct attribute_specifier* p_attribute_specifier, struct error* error)
{
    if (ctx->target < LANGUAGE_C2X)
    {
        token_range_add_flag(p_attribute_specifier->first, p_attribute_specifier->last, TK_FLAG_HIDE);
    }
}

static void visit_attribute_specifier_sequence(struct visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence, struct error* error)
{
    struct attribute_specifier* current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        visit_attribute_specifier(ctx, current, error);
        current = current->next;
    }
}

static void visit_struct_or_union_specifier(struct visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier, struct error* error)
{

    if (p_struct_or_union_specifier->attribute_specifier_sequence)
        visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence, error);

    if (p_struct_or_union_specifier->complete_struct_or_union_specifier)
    {
        if (ctx->bInsideLambda && ctx->lambda_step == 1)
        {
            /*
              Na primeira passada marcamos os tipos que são renomeados
            */
            if (p_struct_or_union_specifier->complete_struct_or_union_specifier->scope_level >
                p_struct_or_union_specifier->scope_level &&
                p_struct_or_union_specifier->complete_struct_or_union_specifier->visit_moved == 0)
            {
                char newtag[200];
                snprintf(newtag, sizeof newtag, "_%s%d", p_struct_or_union_specifier->tag_name, ctx->captureindex);
                ctx->captureindex++;

                free(p_struct_or_union_specifier->complete_struct_or_union_specifier->tagtoken->lexeme);
                p_struct_or_union_specifier->complete_struct_or_union_specifier->tagtoken->lexeme =
                    strdup(newtag);

                p_struct_or_union_specifier->complete_struct_or_union_specifier->visit_moved = 1;
            }
        }
        else if (ctx->lambda_step == 2)
        {
            /*
             Na segunda passada vou renomear quem usa este tag exporado
            */
            if (p_struct_or_union_specifier->complete_struct_or_union_specifier->visit_moved == 1)
            {
                free(p_struct_or_union_specifier->tagtoken->lexeme);
                p_struct_or_union_specifier->tagtoken->lexeme =
                    strdup(p_struct_or_union_specifier->complete_struct_or_union_specifier->tagtoken->lexeme);
            }
        }
    }



    visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list, error);

}

static void visit_enumerator(struct visit_ctx* ctx, struct enumerator* p_enumerator, struct error* error)
{
    if (p_enumerator->constant_expression_opt)
        visit_expression(ctx, p_enumerator->constant_expression_opt, error);

}

//struct enumerator_list* enumerator_list;
static void visit_enumerator_list(struct visit_ctx* ctx, struct enumerator_list* p_enumerator_list, struct error* error)
{
    struct enumerator* current = p_enumerator_list->head;
    while (error->code == 0 && current)
    {
        visit_enumerator(ctx, current, error);
        current = current->next;
    }
}

static void visit_enum_specifier(struct visit_ctx* ctx, struct enum_specifier* p_enum_specifier, struct error* error)
{
    
    visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list, error);
    
}


static void visit_type_specifier(struct visit_ctx* ctx, struct type_specifier* p_type_specifier, struct error* error)
{
    if (p_type_specifier->flags & TYPE_SPECIFIER_TYPEOF)
    {
        if (ctx->target < LANGUAGE_C2X)
        {
            if (p_type_specifier->typeof_specifier)
            {
                /*
                * TODO é muito mais completo que isto..
                * anonomous struct etc...
                */
                /*
                * Na transformacao de codigo vou evitar remover toksn
                * ao inves disso vamos deixar eles inviisveis
                * assim nao quebramos as declaracoes que apontam para estes tokens
                * e somente inserimos tokens novos
                */


                token_range_add_flag(p_type_specifier->typeof_specifier->token,
                    p_type_specifier->typeof_specifier->endtoken,
                    TK_FLAG_HIDE);


                bool bHasPointers = false;
                struct osstream ss = { 0 };

                if (p_type_specifier->typeof_specifier->typeof_specifier_argument->expression)
                {
                    
                    if (p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.declarator_type)
                    {
                        bHasPointers =
                            p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.declarator_type->pointers.head != NULL;
                    }
                    

                    if (bHasPointers)
                        ss_fprintf(&ss, "typedef ");
                    print_type(&ss, &p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type);


                    //anominous struct
                    if (p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier &&
                        p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier->has_anonymous_tag)
                    {
                        /*
                          por se tratar de uma struct anomina vou ligar o tag gerado que estava escondido
                          adiconando ele depois da struct e um espaco antes.
                          so que tenho que fazerisso uma vez so.
                        */

                        /*para nao fazer 2 vezes*/
                        p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier->has_anonymous_tag = false;

                        struct token* first = p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier->first;

                        const char* tag = p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier->tag_name;
                        char buffer[200] = { 0 };
                        snprintf(buffer, sizeof buffer, " %s", tag);
                        struct token_list l2 = tokenizer(buffer, NULL, 0, TK_FLAG_NONE, error);
                        token_list_insert_after(&ctx->ast.token_list, first, &l2);

                    }

                }
                else if (p_type_specifier->typeof_specifier->typeof_specifier_argument->type_name)
                {
                    bHasPointers =
                        p_type_specifier->typeof_specifier->typeof_specifier_argument->type_name->declarator->pointer != NULL;

                    if (bHasPointers)
                        ss_fprintf(&ss, "typedef ");

                    print_type_name(&ss, p_type_specifier->typeof_specifier->typeof_specifier_argument->type_name);
                }
                else
                {
                    assert(false);
                }


                //TODO se nao tem ponteiro poderia ser adicionado direto ali
                //typeof(int*) p1, p2;
                if (bHasPointers)
                {
                    ss_fprintf(&ss, " _typeof_type_%d;\n", ctx->typeofindex);
                    struct token_list list0 = tokenizer(ss.c_str, NULL, 0, TK_FLAG_NONE, error);
                    token_list_append_list(&ctx->insert_before_block_item, &list0);
                    ss_clear(&ss);
                    //este cara tem que ser em cima do statement 
                    ss_fprintf(&ss, "_typeof_type_%d ", ctx->typeofindex);
                    ctx->typeofindex++;
                }
                else
                {
                }



                struct token_list list = tokenizer(ss.c_str, NULL, 0, TK_FLAG_NONE, error);
                for (struct token* current = list.head;
                    current &&
                    current != list.tail->next;
                    current = current->next)
                {
                    current->flags |= TK_FLAG_FINAL;
                }
                ss_close(&ss);
                token_list_insert_after(&ctx->ast.token_list, p_type_specifier->typeof_specifier->endtoken, &list);

                //vou trocar tods os token por estes
            }
        }
    }
}

static void visit_type_specifier_qualifier(struct visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier, struct error* error)
{
    if (p_type_specifier_qualifier->type_qualifier)
    {
    }
    else if (p_type_specifier_qualifier->type_specifier)
    {
        visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier, error);
    }
    else if (p_type_specifier_qualifier->alignment_specifier)
    {
    }
}
static void visit_declaration_specifier(struct visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier, struct error* error)
{

    if (p_declaration_specifier->function_specifier)
    {
    }

    if (p_declaration_specifier->storage_class_specifier)
    {
    }

    if (p_declaration_specifier->type_specifier_qualifier)
    {
        visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier, error);

    }

}

static void visit_declaration_specifiers(struct visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers, struct error* error)
{
    //p_declaration_specifiers->head->type_specifier_qualifier->pType_specifier->p_typeof_expression_opt

    if (p_declaration_specifiers->struct_or_union_specifier)
    {
        visit_struct_or_union_specifier(ctx, p_declaration_specifiers->struct_or_union_specifier, error);
    }
    else if (p_declaration_specifiers->enum_specifier)
    {
        visit_enum_specifier(ctx, p_declaration_specifiers->enum_specifier, error);
    }
    else if (p_declaration_specifiers->typedef_declarator)
    {
        //typedef name
    }
    else if (p_declaration_specifiers->p_typeof_expression_opt)
    {
        visit_expression(ctx, p_declaration_specifiers->p_typeof_expression_opt, error);
    }
    else
    {
        struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;
        while (p_declaration_specifier)
        {
            visit_declaration_specifier(ctx, p_declaration_specifier, error);
            p_declaration_specifier = p_declaration_specifier->next;
        }
    }
}

/*
* retorna true se o ultimo item for um return
*/
static bool is_last_item_return(struct compound_statement* p_compound_statement)
{
    if (p_compound_statement &&        
        p_compound_statement->block_item_list.tail &&
        p_compound_statement->block_item_list.tail->unlabeled_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->token &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->token->type == TK_KEYWORD_RETURN)
    {
        return true;
    }
    return false;
}

static void visit_declaration(struct visit_ctx* ctx, struct declaration* p_declaration, struct error* error)
{
    if (p_declaration->static_assert_declaration)
    {
        visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration, error);
    }

    if (p_declaration->declaration_specifiers)
    {
        visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, error);

    }

    if (ctx->lambda_step == 2)
    {

        if (p_declaration->declaration_specifiers &&
            p_declaration->declaration_specifiers->type_specifier_flags == TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            if (p_declaration->declaration_specifiers->struct_or_union_specifier->visit_moved == 1)
            {
                struct token_list list0 = tokenizer("struct ", NULL, 0, TK_FLAG_FINAL, error);
                token_list_append_list(&ctx->insert_before_declaration, &list0);

                struct token_list list = tokenizer(p_declaration->declaration_specifiers->struct_or_union_specifier->tagtoken->lexeme, NULL, 0, TK_FLAG_FINAL, error);
                token_list_append_list(&ctx->insert_before_declaration, &list);


                //struct token_list list3 = tokenizer("{", NULL, 0, TK_FLAG_FINAL, error);
                //token_list_append_list(&ctx->insert_before_declaration, &list3);



                struct token* first = p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.first;
                struct token* last = p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.last;
                for (struct token* current = first;
                    current != last->next;
                    current = current->next)
                {
                    token_list_clone_and_add(&ctx->insert_before_declaration, current);
                    //current->flags |= TK_FLAG_FINAL;
                }

                struct token_list list3 = tokenizer(";\n", NULL, 0, TK_FLAG_FINAL, error);
                token_list_append_list(&ctx->insert_before_declaration, &list3);


                if (p_declaration->init_declarator_list.head == NULL)
                {
                    token_range_add_flag(p_declaration->declaration_specifiers->struct_or_union_specifier->first,
                        p_declaration->declaration_specifiers->struct_or_union_specifier->last,
                        TK_FLAG_HIDE);
                }
                else
                {
                    token_range_add_flag(p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.first,
                        p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.last,
                        TK_FLAG_HIDE);
                }
            }
        }
    }


    if (p_declaration->init_declarator_list.head)
    {
        visit_init_declarator_list(ctx, &p_declaration->init_declarator_list, error);
    }

    if (p_declaration->function_body)
    {

        ctx->bHasLambda = false;
        ctx->lambda_step = 1;


        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        ctx->tail_block = p_defer;
        p_defer->p_function_body = p_declaration->function_body;

        visit_compound_statement(ctx, p_declaration->function_body, error);

        if (!is_last_item_return(p_declaration->function_body))
        {
            struct osstream ss = { 0 };
            print_block_defer(p_defer, &ss, true);
            if (ss.size > 0)
            {
                struct token_list l = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);
                token_list_insert_after(&ctx->ast.token_list, p_declaration->function_body->last_token->prev, &l);
            }

        }
        else
        {
            //ja tem um return antes que chama defer
            hide_block_defer(p_defer);
        }

        
        ctx->tail_block = NULL;


        if (ctx->bHasLambda)
        {
            ctx->lambda_step = 2;
            visit_compound_statement(ctx, p_declaration->function_body, error);
        }
    }
}

int visit_literal_string(struct visit_ctx* ctx, struct token* current, struct error* error)
{
    bool bUtf8Prefix =
        current->lexeme[0] == 'u' && current->lexeme[1] == '8';

    if (bUtf8Prefix && ctx->target < LANGUAGE_C11)
    {
        struct osstream ss = { 0 };
        unsigned char* psz = current->lexeme + 2;

        while (*psz)
        {
            if (*psz >= 128)
            {
                ss_fprintf(&ss, "\\x%x", *psz);
            }
            else
            {
                ss_fprintf(&ss, "%c", *psz);
            }
            psz++;
        }

        free(current->lexeme);
        current->lexeme = ss.c_str;
        ss.c_str = NULL;
        ss_close(&ss);
    }

    return error->code;
}

int visit_tokens(struct visit_ctx* ctx, struct error* error)
{
    struct token* current = ctx->ast.token_list.head;
    while (current)
    {

        if (current->type == TK_STRING_LITERAL)
        {
            visit_literal_string(ctx, current, error);
        }

        if (ctx->target < LANGUAGE_C2X)
        {
            if (current->type == TK_KEYWORD_TRUE)
            {
                free(current->lexeme);
                current->lexeme = strdup("((_Bool)1)");
            }
            else if (current->type == TK_KEYWORD_FALSE)
            {
                free(current->lexeme);
                current->lexeme = strdup("((_Bool)0)");
            }
            else if (current->type == TK_KEYWORD_NULLPTR)
            {
                free(current->lexeme);
                current->lexeme = strdup("((void*)0)");
            }
            else if (current->type == TK_KEYWORD__BOOL)
            {
                if (strcmp(current->lexeme, "bool") == 0)
                {
                    free(current->lexeme);
                    current->lexeme = strdup("_Bool");
                }

            }
            else if (current->type == TK_PREPROCESSOR_LINE)
            {
                while (current->next && current->next->type == TK_BLANKS)
                    current = current->next;

                /*
                  Trocar C23 #elifdef e #elifndef
                */
                if (current->next && strcmp(current->next->lexeme, "elifdef") == 0)
                {
                    free(current->next->lexeme);
                    current->next->lexeme = strdup("elif defined ");
                    current = current->next->next;
                    continue;
                }
                else if (current->next && strcmp(current->next->lexeme, "elifndef") == 0)
                {
                    free(current->next->lexeme);
                    current->lexeme = strdup("elif ! defined ");
                    current = current->next->next;
                    continue;
                }
            }

            if (
                (current->type == TK_COMPILER_BINARY_CONSTANT) ||
                (current->type == TK_PPNUMBER && current->lexeme[0] == '0' && (current->lexeme[1] == 'b' || current->lexeme[1] == 'B')) /*dentro macros*/
                )
            {
                /*remove digit separators*/
                remove_char(current->lexeme, '\'');

                /*
                * Binary literals were added into C23.
                * We are converting to C99 hex.
                */
                current->type = TK_COMPILER_HEXADECIMAL_CONSTANT;
                int value = strtol(current->lexeme + 2, 0, 2);
                char buffer[33 + 2] = { '0', 'x' };
                snprintf(buffer, sizeof buffer, "0x%x", value);
                free(current->lexeme);
                current->lexeme = strdup(buffer);
            }
            else if (current->type == TK_COMPILER_DECIMAL_CONSTANT ||
                current->type == TK_COMPILER_OCTAL_CONSTANT ||
                current->type == TK_COMPILER_HEXADECIMAL_CONSTANT ||
                current->type == TK_COMPILER_DECIMAL_FLOATING_CONSTANT ||
                current->type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT ||
                current->type == TK_PPNUMBER)
            {
                /*remove digit separators*/
                remove_char(current->lexeme, '\'');
            }
        }

        current = current->next;
    }
    return error->code;
}

void visit(struct visit_ctx* ctx, struct error* error)
{
    ctx->captureindex = 0;
    ctx->lambdasindex = 0;
    ctx->typeofindex = 0;

    if (visit_tokens(ctx, error) != 0)
        return;

    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (error->code == 0 && p_declaration)
    {
        visit_declaration(ctx, p_declaration, error);

        if (ctx->insert_before_block_item.head != NULL)
        {
            //token_list_insert_after(&ctx->ast.token_list, p_statement->first_token->prev, &ctx->insert_before_statement);
            token_list_insert_after(&ctx->ast.token_list, p_declaration->first_token->prev, &ctx->insert_before_block_item);
            ctx->insert_before_block_item.head = NULL;
            ctx->insert_before_block_item.tail = NULL;
        }


        /*
        * Tem que inserir algo antes desta declaracao?
        */
        if (ctx->insert_before_declaration.head != NULL)
        {
            token_list_insert_after(&ctx->ast.token_list, p_declaration->first_token->prev, &ctx->insert_before_declaration);
            ctx->insert_before_declaration.head = NULL;
            ctx->insert_before_declaration.tail = NULL;
        }

        p_declaration = p_declaration->next;
    }
    if (ctx->instanciations.head != NULL)
    {
        token_list_append_list(&ctx->ast.token_list, &ctx->instanciations);
    }
}
