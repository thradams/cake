/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "ownership.h"

#include <assert.h>
#include <string.h>
#include <assert.h>
#include "visit_defer.h"
#include "expressions.h"
#include "ownership.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>


/*
  We maintain a stack of items—blocks, defers, variables—
  each pointing to its predecessor.
  This stack represents the scopes and the variables that are alive
  at any point in the code.
*/
struct defer_scope
{
    struct declarator* _Opt p_declarator;
    struct defer_statement* _Opt p_defer_statement;
    struct defer_statement* _Opt p_defer_block;
    struct try_statement* _Opt p_try_statement;
    struct selection_statement* _Opt p_selection_statement;
    struct iteration_statement* _Opt p_iteration_statement;
    struct statement* _Opt p_statement;
    struct compound_statement* _Opt p_compound_statement;
    struct compound_statement* _Opt p_function_body;
    struct secondary_block* _Opt p_secondary_block;
    struct primary_block* _Opt p_primary_block;
    struct label* _Owner _Opt label;

    struct defer_scope* _Owner _Opt previous;
};


void defer_visit_declaration(struct defer_visit_ctx* ctx, struct declaration* p_declaration);
static void defer_visit_declarator(struct defer_visit_ctx* ctx, struct declarator* p_declarator);
static void defer_visit_init_declarator_list(struct defer_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list);;
static void defer_visit_secondary_block(struct defer_visit_ctx* ctx, struct secondary_block* p_secondary_block);
static void defer_visit_statement(struct defer_visit_ctx* ctx, struct statement* p_statement);
static void defer_visit_block_item(struct defer_visit_ctx* ctx, struct block_item* p_block_item);
static void defer_visit_compound_statement(struct defer_visit_ctx* ctx, struct compound_statement* p_compound_statement);
static void defer_visit_unlabeled_statement(struct defer_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement);

static struct defer_scope* _Opt defer_visit_ctx_push_child(struct defer_visit_ctx* ctx)
{
    if (ctx->searching_label_mode && ctx->p_label)
    {
        assert(false);
    }

    struct defer_scope* _Owner _Opt child = calloc(1, sizeof * child);
    if (child)
    {
        child->previous = ctx->tail_block;
        ctx->tail_block = child;
    }
    else
    {
        //ops
    }
    return (struct defer_scope* _Opt) child;
}


static struct defer_scope* _Opt defer_visit_ctx_pop_child(struct defer_visit_ctx* ctx)
{
    struct defer_scope* p = ctx->tail_block;
    if (p)
    {
        ctx->tail_block = ctx->tail_block->previous;
    }

    return p;
}

static void defer_visit_ctx_pop_until(struct defer_visit_ctx* ctx, struct defer_scope* _Opt p_defer, struct defer_list* p_defer_list)
{
    if (ctx->searching_label_mode && ctx->p_label)
    {
        return;
    }

    try
    {
        struct defer_scope* _Opt p = ctx->tail_block;

        while (p)
        {
            if (p != p_defer)
            {
                if (p_defer_list && !ctx->searching_label_mode)
                {
                    if (p->p_defer_statement)
                    {
                        struct defer_list_item* item = calloc(1, sizeof * item);
                        if (item == NULL) throw;
                        item->defer_statement = p->p_defer_statement;
                        defer_list_add(p_defer_list, item); //items com inicialzaxao
                    }
                    else if (p->p_declarator)
                    {
                        struct defer_list_item* item = calloc(1, sizeof * item);
                        if (item == NULL) throw;
                        item->declarator = p->p_declarator;
                        defer_list_add(p_defer_list, item); //items com inicialzaxao
                    }
                }
                p = p->previous;

                struct defer_scope* _Opt pDelete = defer_visit_ctx_pop_child(ctx);
                free(pDelete);
            }
            else
            {
                struct defer_scope* _Opt pDelete = defer_visit_ctx_pop_child(ctx);
                free(pDelete);
                break;
            }
        }
    }
    catch
    {
    }
}

static void defer_visit_secondary_block(struct defer_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    defer_visit_statement(ctx, p_secondary_block->statement);
}

static void defer_visit_defer_statement(struct defer_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{
    struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
    if (p_defer == NULL)
        return;

    p_defer->p_defer_statement = p_defer_statement;

    struct defer_scope* _Opt p_defer_block = defer_visit_ctx_push_child(ctx);
    if (p_defer_block == NULL)
        return;

    p_defer_block->p_defer_block = p_defer_statement;
    defer_visit_unlabeled_statement(ctx, p_defer_statement->unlabeled_statement);

    defer_visit_ctx_pop_until(ctx, p_defer_block, NULL);
}

static void defer_visit_init_declarator(struct defer_visit_ctx* ctx, struct init_declarator* p_init_declarator)
{
    defer_visit_declarator(ctx, p_init_declarator->p_declarator);
}

static void defer_visit_simple_declaration(struct defer_visit_ctx* ctx, struct simple_declaration* p_simple_declaration)
{
    defer_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}

static void defer_visit_if_statement(struct defer_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    try
    {
        struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
        if (p_defer == NULL) throw;

        p_defer->p_selection_statement = p_selection_statement;

        if (p_selection_statement->p_init_statement &&
            p_selection_statement->p_init_statement->p_simple_declaration)
            defer_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);

        if (p_selection_statement->condition &&
            p_selection_statement->condition->p_init_declarator)
        {
            defer_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
        }

        assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);

        defer_visit_secondary_block(ctx, p_selection_statement->secondary_block);

        if (p_selection_statement->else_secondary_block_opt)
        {
            defer_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);
        }

        defer_visit_ctx_pop_until(ctx, p_defer, &p_selection_statement->defer_list);
    }
    catch
    {
    }
}

static void defer_visit_try_statement(struct defer_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    try
    {
        struct secondary_block* _Opt catch_secondary_block_old = ctx->catch_secondary_block_opt;

        ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;

        struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
        if (p_defer == NULL) throw;

        p_defer->p_try_statement = p_try_statement;

        defer_visit_secondary_block(ctx, p_try_statement->secondary_block);

        if (p_try_statement->catch_secondary_block_opt)
        {
            defer_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
        }
        defer_visit_ctx_pop_until(ctx, p_defer, NULL);
        ctx->catch_secondary_block_opt = catch_secondary_block_old; //restore
    }
    catch
    {
    }
}

static void defer_visit_switch_statement(struct defer_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    try
    {
        struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
        if (p_defer == NULL) throw;

        p_defer->p_selection_statement = p_selection_statement;
        defer_visit_secondary_block(ctx, p_selection_statement->secondary_block);
        defer_visit_ctx_pop_until(ctx, p_defer, &p_selection_statement->defer_list);
    }
    catch
    {
    }
}

static void defer_visit_selection_statement(struct defer_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    if (p_selection_statement->first_token->type == TK_KEYWORD_IF)
    {
        defer_visit_if_statement(ctx, p_selection_statement);
    }
    else if (p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
    {
        defer_visit_switch_statement(ctx, p_selection_statement);
    }
    else
        assert(false);
}

static void defer_visit_block_item_list(struct defer_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* _Opt p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        defer_visit_block_item(ctx, p_block_item);

        if (ctx->searching_label_mode && ctx->p_label)
        {
            //stop search
            break;
        }

        p_block_item = p_block_item->next;
    }
}

static void defer_visit_compound_statement(struct defer_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    try
    {
        struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
        if (p_defer == NULL) throw;

        p_defer->p_compound_statement = p_compound_statement;

        defer_visit_block_item_list(ctx, &p_compound_statement->block_item_list);

        defer_visit_ctx_pop_until(ctx, p_defer, ctx->searching_label_mode ? NULL : &p_compound_statement->defer_list);
    }
    catch
    {
    }
}

static void defer_visit_do_while_statement(struct defer_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);

    try
    {
        struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
        if (p_defer == NULL) throw;

        p_defer->p_iteration_statement = p_iteration_statement;
        defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        defer_visit_ctx_pop_until(ctx, p_defer, &p_iteration_statement->defer_list);
    }
    catch
    {
    }
}

static void defer_visit_while_statement(struct defer_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    try
    {
        assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

        if (p_iteration_statement->expression1 == NULL) throw;

        struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
        if (p_defer == NULL) throw;

        p_defer->p_iteration_statement = p_iteration_statement;

        defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        defer_visit_ctx_pop_until(ctx, p_defer, &p_iteration_statement->defer_list);
    }
    catch
    {
    }
}

static void defer_visit_for_statement(struct defer_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);

    try
    {
        if (p_iteration_statement->declaration &&
            p_iteration_statement->declaration->init_declarator_list.head)
        {
            defer_visit_init_declarator_list(ctx, &p_iteration_statement->declaration->init_declarator_list);
        }

        struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
        if (p_defer == NULL) throw;

        p_defer->p_iteration_statement = p_iteration_statement;
        defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }
    catch
    {
    }
}

static void defer_visit_iteration_statement(struct defer_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    switch (p_iteration_statement->first_token->type)
    {
    case  TK_KEYWORD_WHILE:
        defer_visit_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_DO:
        defer_visit_do_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_FOR:
        defer_visit_for_statement(ctx, p_iteration_statement);
        break;
    default:
        assert(false);
        break;
    }
}

static struct defer_scope* find_common_defer_scope(struct defer_scope* p_label_list, struct defer_scope* p_goto_list)
{
    //we have two scope lists, one at the goto point and another at the label point.
    // this list must have common prefix, in the  worst  case is the function scope.

    //A B C D E F G
    //A B 1 2 
    struct defer_scope* p1 = p_label_list;
    while (p_label_list)
    {
        struct defer_scope* p = p_goto_list;
        while (p)
        {
            if (p->p_declarator == p1->p_declarator &&
                p->p_defer_statement == p1->p_defer_statement &&
                p->p_defer_block == p1->p_defer_block &&
                p->p_try_statement == p1->p_try_statement &&
                p->p_selection_statement == p1->p_selection_statement &&
                p->p_iteration_statement == p1->p_iteration_statement &&
                p->p_statement == p1->p_statement &&
                p->p_compound_statement == p1->p_compound_statement &&
                p->p_function_body == p1->p_function_body &&
                p->p_secondary_block == p1->p_secondary_block &&
                p->p_primary_block == p1->p_primary_block &&
                p->label == p1->label)
            {
                return p;
            }

            p = p->previous;
        }

        p1 = p1->previous;
    }
    return NULL;
}

static void defer_visit_jump_statement(struct defer_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    if (ctx->searching_label_mode)
        return;

    assert(ctx->tail_block != NULL);

    try
    {
        if (p_jump_statement->first_token->type == TK_KEYWORD_CAKE_THROW)
        {
            struct defer_scope* _Opt p = ctx->tail_block;
            while (p)
            {
                if (p->p_try_statement)
                    break;

                if (p->p_declarator)
                {
                    struct defer_list_item* item = calloc(1, sizeof * item);
                    if (item == NULL) throw;

                    item->declarator = p->p_declarator;
                    defer_list_add(&p_jump_statement->defer_list, item);
                    
                }
                else if (p->p_defer_statement)
                {
                    struct defer_list_item* item = calloc(1, sizeof * item);
                    if (item == NULL) throw;

                    item->defer_statement = p->p_defer_statement;
                    defer_list_add(&p_jump_statement->defer_list, item);                    
                }
                if (p->p_defer_block)
                {
                    compiler_diagnostic(C_ERROR_EXIT_DEFER, ctx->ctx, p->p_defer_block->first_token, NULL, "is jumping out of defer");
                }
                p = p->previous;
            }

        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
        {
            struct defer_scope* _Opt p = ctx->tail_block;
            while (p)
            {
                if (p->p_declarator)
                {
                    struct defer_list_item* item = calloc(1, sizeof * item);
                    if (item == NULL) throw;

                    item->declarator = p->p_declarator;
                    defer_list_add(&p_jump_statement->defer_list, item);                    
                }
                else if (p->p_defer_statement)
                {
                    struct defer_list_item* item = calloc(1, sizeof * item);
                    if (item == NULL) throw;

                    item->defer_statement = p->p_defer_statement;
                    defer_list_add(&p_jump_statement->defer_list, item);                    
                }

                if (p->p_defer_block)
                {
                    compiler_diagnostic(C_ERROR_EXIT_DEFER, ctx->ctx, p->p_defer_block->first_token, NULL, "is jumping out of defer");
                }
                p = p->previous;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE ||
                 p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            const bool break_jump = p_jump_statement->first_token->type == TK_KEYWORD_BREAK;

            struct defer_scope* _Opt p = ctx->tail_block;
            while (p)
            {

                if (p->p_iteration_statement)
                    break;

                //break  also works in switch
                if (break_jump &&
                    p->p_selection_statement &&
                    p->p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
                {
                    break;
                }

                if (p->p_declarator)
                {
                    struct defer_list_item* item = calloc(1, sizeof * item);
                    if (item == NULL) throw;

                    item->declarator = p->p_declarator;
                    defer_list_add(&p_jump_statement->defer_list, item);
                }
                else if (p->p_defer_statement)
                {
                    struct defer_list_item* item = calloc(1, sizeof * item);
                    if (item == NULL) throw;

                    item->defer_statement = p->p_defer_statement;
                    defer_list_add(&p_jump_statement->defer_list, item);
                }

                if (p->p_defer_block)
                {
                    if (break_jump)
                        compiler_diagnostic(C_ERROR_EXIT_DEFER, ctx->ctx, p_jump_statement->first_token, NULL, "break cannot jump out of defer");
                    else
                        compiler_diagnostic(C_ERROR_EXIT_DEFER, ctx->ctx, p_jump_statement->first_token, NULL, "continue cannot jump out of defer");                    
                }
                p = p->previous;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
        {
            //Visit to find the route until label
            struct defer_visit_ctx label_ctx = { 0 };
            label_ctx.searching_label_mode = true;
            label_ctx.label_name = p_jump_statement->label->lexeme;
            defer_start_visit_declaration(&label_ctx, ctx->p_declaration);

            struct defer_scope* p_common =
                find_common_defer_scope(label_ctx.tail_block /*label*/, ctx->tail_block /*goto*/);

            if (p_common == NULL)
            {
                //should be not null. However, test-mode code 
                //"eats" errors and we can be in this situation.
                //when the label is not found (test that checks for the error label is not found)
            }

            struct defer_scope* _Owner _Opt p1 = label_ctx.tail_block;
            while (p1)
            {
                if (p1->p_defer_statement)
                {
                    bool found = false;
                    struct defer_scope* _Owner _Opt p0 = ctx->tail_block;
                    while (p0)
                    {
                        if (p0->p_defer_statement == p1->p_defer_statement)
                        {
                            found = true; //quando acha 1 acha todos??
                            break;
                        }
                        p0 = p0->previous;
                    }

                    if (!found)
                    {
                        compiler_diagnostic(C_ERROR_EXIT_DEFER, ctx->ctx, p_jump_statement->first_token, NULL, "jumping over defer. from here");
                        compiler_diagnostic(W_LOCATION, ctx->ctx, label_ctx.p_label->p_first_token, NULL, "to here");
                        compiler_diagnostic(W_LOCATION, ctx->ctx, p1->p_defer_statement->first_token, NULL, "defer");
                    }
                }
                else if (p1->p_defer_block)
                {
                    bool found = false;
                    struct defer_scope* _Owner _Opt p0 = ctx->tail_block;
                    while (p0)
                    {
                        if (p0->p_defer_block == p1->p_defer_block)
                        {
                            found = true; //quando acha 1 acha todos??
                            break;
                        }
                        p0 = p0->previous;
                    }
                    if (!found)
                    {
                        compiler_diagnostic(C_ERROR_EXIT_DEFER, ctx->ctx, p_jump_statement->first_token, NULL, "jumping into defer. from here");
                        compiler_diagnostic(W_LOCATION, ctx->ctx, label_ctx.p_label->p_first_token, NULL, "to here");
                        //compiler_diagnostic(W_LOCATION, ctx->ctx, p1->p_defer_block->first_token, NULL, "defer");
                    }
                }
                p1 = p1->previous;
            }

            const char* label = p_jump_statement->label->lexeme;
            struct defer_scope* _Opt p = ctx->tail_block;
            struct defer_scope* _Opt start = p;
            while (p)
            {
                if (p_common == p)
                {
                    //we are in the common, we dont need to up anymore.
                    break;
                }

                if (p->p_compound_statement ||
                    p->p_iteration_statement ||
                    p->p_primary_block ||
                    p->p_secondary_block)
                {
                    //start -> p
                    struct defer_scope* _Opt p2 = start;
                    while (p2 != p)
                    {
                        if (p2->p_declarator)
                        {
                            struct defer_list_item* item = calloc(1, sizeof * item);
                            if (item == NULL) throw;
                            item->declarator = p2->p_declarator;
                            defer_list_add(&p_jump_statement->defer_list, item);                            
                        }
                        else if (p2->p_defer_statement)
                        {
                            struct defer_list_item* item = calloc(1, sizeof * item);
                            if (item == NULL) throw;
                            item->defer_statement = p2->p_defer_statement;
                            defer_list_add(&p_jump_statement->defer_list, item);                            
                        }

                        if (p->p_defer_block)
                        {
                            compiler_diagnostic(C_ERROR_EXIT_DEFER, ctx->ctx, p_jump_statement->first_token, NULL, "jumping out of defer.");
                            compiler_diagnostic(W_LOCATION, ctx->ctx, label_ctx.p_label->p_first_token, NULL, "to here");
                            // compiler_diagnostic(W_LOCATION, ctx->ctx, p1->p_defer_statement->first_token, NULL, "defer");                            
                        }
                        p2 = p2->previous;
                    }
                    start = p;
                }
                else if (p->label &&
                         p->label->p_first_token &&
                         strcmp(p->label->p_first_token->lexeme, label) == 0)
                {
                    struct defer_scope* _Opt p2 = start;
                    while (p2 != p)
                    {
                        if (p2->p_declarator)
                        {
                            struct defer_list_item* item = calloc(1, sizeof * item);
                            if (item == NULL) throw;
                            item->declarator = p2->p_declarator;
                            defer_list_add(&p_jump_statement->defer_list, item);
                        }
                        else if (p2->p_defer_statement)
                        {
                            struct defer_list_item* item = calloc(1, sizeof * item);
                            if (item == NULL) throw;
                            item->defer_statement = p2->p_defer_statement;
                            defer_list_add(&p_jump_statement->defer_list, item);
                        }
                        p2 = p2->previous;
                    }
                    break; //pode parar de subir
                }

                if (p->p_defer_block)
                {
                    compiler_diagnostic(C_ERROR_EXIT_DEFER, ctx->ctx, p_jump_statement->first_token, NULL, "goto is jumping out of defer.");
                    compiler_diagnostic(W_LOCATION, ctx->ctx, label_ctx.p_label->p_first_token, NULL, "to here");
                }

                p = p->previous;
            }
        }
        else
        {
            assert(false);
            throw;
        }
    }
    catch
    {
    }
}

static void defer_visit_labeled_statement(struct defer_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    defer_visit_statement(ctx, p_labeled_statement->statement);
}

static void defer_visit_primary_block(struct defer_visit_ctx* ctx, struct primary_block* p_primary_block)
{
    if (p_primary_block->compound_statement)
    {
        defer_visit_compound_statement(ctx, p_primary_block->compound_statement);
    }
    else if (p_primary_block->iteration_statement)
    {
        defer_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
    }
    else if (p_primary_block->selection_statement)
    {
        defer_visit_selection_statement(ctx, p_primary_block->selection_statement);
    }
    else if (p_primary_block->try_statement)
    {
        defer_visit_try_statement(ctx, p_primary_block->try_statement);
    }
}

static void defer_visit_expression(struct defer_visit_ctx* ctx, struct expression* p_expression)
{
    /*
        Literal functions need to build defer_list
    */

    if (p_expression->condition_expr)
    {
        defer_visit_expression(ctx, p_expression->condition_expr);
    }

    if (p_expression->left)
    {
        defer_visit_expression(ctx, p_expression->left);
    }

    if (p_expression->right)
    {
        defer_visit_expression(ctx, p_expression->right);
    }

    switch (p_expression->expression_type)
    {
    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
    {
        assert(p_expression->compound_statement != NULL);

        //TODO missing parameters of literal functions
        //without it static analysis will not work
        defer_visit_compound_statement(ctx, p_expression->compound_statement);
        //assert(ctx->tail_block == NULL);
        //struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
        //if (p_defer == NULL)
        //{
          //  return;
        //}
        //p_defer->p_function_body = p_declaration->function_body;

        //defer_visit_typen(ctx, p_declaration);
        //assert(p_declaration->function_body != NULL); //defer_visit_declaration does not change this

        //parameters
        //if (ctx->tail_block)
        //{
          //  //exit_block_visit(ctx,
          //      ctx->tail_block,
          //      p_expression->compound_statement->last_token,
          //      &p_expression->defer_list);
        //}

        //
    }
    break;
    default:
        break;
    }
}

static void defer_visit_expression_statement(struct defer_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
        defer_visit_expression(ctx, p_expression_statement->expression_opt);
}

static void defer_visit_unlabeled_statement(struct defer_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        defer_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        defer_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        defer_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else if (p_unlabeled_statement->defer_statement)
    {
        defer_visit_defer_statement(ctx, p_unlabeled_statement->defer_statement);
    }
    else
    {
        assert(false);
    }
}

static void defer_visit_statement(struct defer_visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        defer_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        defer_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void defer_visit_block_item(struct defer_visit_ctx* ctx, struct block_item* p_block_item)
{
    try
    {
        if (p_block_item->declaration)
        {
            defer_visit_declaration(ctx, p_block_item->declaration);
        }
        else if (p_block_item->unlabeled_statement)
        {
            defer_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
        }
        else if (p_block_item->label)
        {
            if (ctx->searching_label_mode)
            {
                assert(ctx->label_name != NULL);

                if (strcmp(ctx->label_name, p_block_item->label->p_first_token->lexeme) == 0)
                {
                    ctx->p_label = p_block_item->label;
                    return;
                }
            }

            struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
            if (p_defer == NULL) throw;
            p_defer->label = p_block_item->label;
        }
    }
    catch
    {
    }
}

static void defer_visit_direct_declarator(struct defer_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration* _Opt parameter = NULL;

        if (p_direct_declarator->function_declarator->parameter_type_list_opt &&
            p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }

        ctx->parameter_list++;

        while (parameter)
        {
            if (parameter->declarator)
            {
                defer_visit_declarator(ctx, parameter->declarator);
            }

            parameter = parameter->next;
        }

        ctx->parameter_list--;
    }
    else if (p_direct_declarator->array_declarator)
    {
    }
}

static void defer_visit_declarator(struct defer_visit_ctx* ctx, struct declarator* p_declarator)
{
    try
    {
        if (ctx->tail_block &&
            p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
        {
            if (ctx->parameter_list > 1)
            {
                /*
                 The objective here is to avoid including the arguments
                 of function pointers inside the scope.
                 Sample
                 void x_destroy(void (*f)(void * _Owner p))
                 We add f but not p.
                */
                return;
            }

            struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
            if (p_defer == NULL) throw;
            p_defer->p_declarator = p_declarator;
        }

        if (p_declarator->direct_declarator)
        {
            defer_visit_direct_declarator(ctx, p_declarator->direct_declarator);
        }
    }
    catch
    {
    }
}

static void defer_visit_init_declarator_list(struct defer_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        defer_visit_init_declarator(ctx, p_init_declarator);
        p_init_declarator = p_init_declarator->next;
    }
}

void defer_visit_declaration(struct defer_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->init_declarator_list.head)
    {
        defer_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        defer_visit_compound_statement(ctx, p_declaration->function_body);
    }
}

void defer_start_visit_declaration(struct defer_visit_ctx* ctx, struct declaration* p_declaration)
{
    ctx->p_declaration = p_declaration;

    try
    {

        if (p_declaration->function_body)
        {
            assert(ctx->tail_block == NULL);
            struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
            if (p_defer == NULL)
            {
                return;
            }
            p_defer->p_function_body = p_declaration->function_body;

            defer_visit_declaration(ctx, p_declaration);
            assert(p_declaration->function_body != NULL); //defer_visit_declaration does not change this
            defer_visit_ctx_pop_until(ctx, p_defer, &p_declaration->function_body->defer_list);

            //parameters
            if (ctx->tail_block)
            {
                //exit_block_visit(ctx,
                  //  ctx->tail_block,
                    //p_declaration->function_body->last_token,
                    //&p_declaration->defer_list); //maybe use the same defer_list from body??
            }
        }
        else
        {
            struct defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
            if (p_defer == NULL) throw;            
            defer_visit_declaration(ctx, p_declaration);
        }
    }
    catch 
    {
    }
}

void defer_visit_ctx_destroy(_Dtor struct defer_visit_ctx* p)
{
    if (p->tail_block != NULL)
    {        
        assert(p->tail_block->previous == NULL);
        free(p->tail_block);
    }
}

