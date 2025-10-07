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
              NULL
                |
            [function] (NULL <- child0 <- last_child)
                |
              [try]  (NULL <- child0 <- last_child)
                |
tail_block-> [while] (NULL <- child0 <- last_child)

*/

struct defer_defer_scope
{
    //things must called at end of scope
    struct declarator* _Opt declarator; // declarator
    struct defer_statement* _Opt defer_statement; // defer

    //statements for controlling where jump like break, throw stop.

    struct try_statement* _Opt p_try_statement; //try
    struct selection_statement* _Opt p_selection_statement; //if switch
    struct iteration_statement* _Opt p_iteration_statement; //for do while
    struct statement* _Opt p_statement;//
    struct compound_statement* _Opt p_compound_statement;
    struct compound_statement* _Opt p_function_body;
    struct secondary_block* _Opt p_secondary_block;
    struct primary_block* _Opt p_primary_block;

    struct defer_defer_scope* _Owner _Opt last_child; /**/
    struct defer_defer_scope* _Owner _Opt previous;
};

void defer_visit_declaration(struct defer_visit_ctx* ctx, struct declaration* p_declaration);
static void defer_visit_declarator(struct defer_visit_ctx* ctx, struct declarator* p_declarator);
static void defer_visit_init_declarator_list(struct defer_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list);;
static void defer_visit_secondary_block(struct defer_visit_ctx* ctx, struct secondary_block* p_secondary_block);
static bool defer_find_label_unlabeled_statement(struct defer_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, const char* label);
static void defer_visit_statement(struct defer_visit_ctx* ctx, struct statement* p_statement);

static void defer_visit_block_item(struct defer_visit_ctx* ctx, struct block_item* p_block_item);
static void defer_visit_compound_statement(struct defer_visit_ctx* ctx, struct compound_statement* p_compound_statement);
static bool defer_find_label_statement(struct defer_visit_ctx* ctx, struct statement* statement, const char* label);

struct defer_visit_objects {
    struct defer_defer_scope* current_block;
    struct defer_defer_scope* next_child;
};

static struct defer_defer_scope* _Opt defer_visit_ctx_push_tail_block(struct defer_visit_ctx* ctx)
{
    struct defer_defer_scope* _Owner _Opt p_block = calloc(1, sizeof * p_block);
    if (p_block)
    {
        p_block->previous = ctx->tail_block;
        ctx->tail_block = p_block;
    }
    else
    {
        //ops
    }
    return ctx->tail_block;
}

static struct defer_defer_scope* _Opt defer_visit_ctx_push_child(struct defer_visit_ctx* ctx)
{
    if (ctx->tail_block == NULL)
        return NULL;

    struct defer_defer_scope* _Owner _Opt child = calloc(1, sizeof * child);
    if (child)
    {
        child->previous = ctx->tail_block->last_child;
        ctx->tail_block->last_child = child;
    }
    else
    {
        //ops
    }
    return ctx->tail_block->last_child;
}

static void defer_defer_scope_delete_one(struct defer_defer_scope* _Owner p)
{
    struct defer_defer_scope* _Owner p_block = p;
    struct defer_defer_scope* _Owner _Opt child = p_block->last_child;

    while (child != NULL)
    {
        struct defer_defer_scope* _Owner _Opt previous = child->previous;
        child->previous = NULL;
        defer_defer_scope_delete_one(child);
        child = previous;
    }
    assert(p_block->previous == NULL);
    free(p_block);
}

static void defer_visit_ctx_pop_tail_block(struct defer_visit_ctx* ctx)
{
    if (ctx->tail_block)
    {
        struct defer_defer_scope* _Owner _Opt previous = ctx->tail_block->previous;
        ctx->tail_block->previous = NULL;
        defer_defer_scope_delete_one(ctx->tail_block);
        ctx->tail_block = previous;
    }
}

static void defer_exit_block_visit(struct defer_visit_ctx* ctx,
    struct defer_defer_scope* deferblock,
    struct token* position_token,
    struct defer_list* p_defer_list)
{
    try
    {
        struct defer_defer_scope* _Opt deferchild = deferblock->last_child;
        while (deferchild != NULL)
        {
            if (deferchild->defer_statement)
            {
                struct defer_list_item* _Opt _Owner p_item = calloc(1, sizeof * p_item);
                if (p_item == NULL)
                    throw;

                p_item->defer_statement = deferchild->defer_statement;
                defer_list_add(p_defer_list, p_item);

                defer_visit_secondary_block(ctx, deferchild->defer_statement->secondary_block);
            }
            else if (deferchild->declarator)
            {
                struct defer_list_item* _Opt _Owner p_item = calloc(1, sizeof * p_item);
                if (p_item == NULL)
                    throw;

                p_item->declarator = deferchild->declarator;
                defer_list_add(p_defer_list, p_item);
                //const char* name = p_declarator->name_opt ? p_declarator->name_opt->lexeme : "?";
            }
            deferchild = deferchild->previous;
        }
    }
    catch
    {
    }
}

static void defer_check_all_defer_until_try(struct defer_visit_ctx* ctx, struct defer_defer_scope* deferblock,
    struct token* position_token,
    struct defer_list* p_defer_list)
{
    struct defer_defer_scope* _Opt p_defer = deferblock;
    while (p_defer != NULL)
    {
        defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);

        if (p_defer->p_try_statement)
        {
            break;
        }

        p_defer = p_defer->previous;
    }
}

static bool defer_find_label_block_item_list(struct defer_visit_ctx* ctx, struct block_item_list* p_block_item, const char* label)
{
    struct block_item* _Opt block_item = p_block_item->head;
    while (block_item)
    {
        if (block_item->label &&
            block_item->label->p_identifier_opt &&
            strcmp(block_item->label->p_identifier_opt->lexeme, label) == 0)
        {
            return true;
        }
        else if (block_item->unlabeled_statement)
        {
            if (defer_find_label_unlabeled_statement(ctx, block_item->unlabeled_statement, label))
            {
                return true;
            }
        }

        block_item = block_item->next;
    }

    return false;
}

static bool defer_find_label_unlabeled_statement(struct defer_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, const char* label)
{
    if (p_unlabeled_statement->primary_block)
    {
        if (p_unlabeled_statement->primary_block->compound_statement)
        {
            if (defer_find_label_block_item_list(ctx,
                &p_unlabeled_statement->primary_block->compound_statement->block_item_list,
                label))
            {
                return true;
            }
        }
        else if (p_unlabeled_statement->primary_block->selection_statement)
        {
            if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->selection_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt)
            {
                if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->try_statement)
        {
            if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->try_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt)
            {
                if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->iteration_statement)
        {
            if (defer_find_label_statement(ctx, p_unlabeled_statement->primary_block->iteration_statement->secondary_block->statement, label))
            {
                return true;
            }
        }
    }
    return false;
}

static bool defer_find_label_statement(struct defer_visit_ctx* ctx, struct statement* statement, const char* label)
{
    if (statement->labeled_statement &&
        statement->labeled_statement->label->p_identifier_opt)
    {
        if (/*statement->labeled_statement->label &&*/
            strcmp(statement->labeled_statement->label->p_identifier_opt->lexeme, label) == 0)
        {
            return true;
        }
    }
    else if (statement->unlabeled_statement)
    {
        if (defer_find_label_unlabeled_statement(ctx, statement->unlabeled_statement, label))
            return true;
    }
    return false;
}

static bool defer_find_label_scope(struct defer_visit_ctx* ctx, struct defer_defer_scope* deferblock, const char* label)
{
    if (deferblock->p_iteration_statement)
    {
        if (defer_find_label_statement(ctx, deferblock->p_iteration_statement->secondary_block->statement, label))
            return true;
    }
    else if (deferblock->p_selection_statement)
    {
        if (defer_find_label_statement(ctx, deferblock->p_selection_statement->secondary_block->statement, label))
            return true;

        if (deferblock->p_selection_statement->else_secondary_block_opt)
        {
            if (defer_find_label_statement(ctx, deferblock->p_selection_statement->else_secondary_block_opt->statement, label))
                return true;
        }
    }
    else if (deferblock->p_try_statement)
    {
        if (defer_find_label_statement(ctx, deferblock->p_try_statement->secondary_block->statement, label))
            return true;

        if (deferblock->p_try_statement->catch_secondary_block_opt)
        {
            if (defer_find_label_statement(ctx, deferblock->p_try_statement->catch_secondary_block_opt->statement, label))
                return true;
        }
    }
    else if (deferblock->p_statement)
    {
        if (defer_find_label_statement(ctx, deferblock->p_statement, label))
            return true;
    }
    else if (deferblock->p_function_body)
    {
        if (defer_find_label_block_item_list(ctx,
            &deferblock->p_function_body->block_item_list,
            label))
        {
            return true;
        }
    }
    else if (deferblock->p_compound_statement)
    {
        if (defer_find_label_block_item_list(ctx,
            &deferblock->p_compound_statement->block_item_list,
            label))
        {
            return true;
        }
    }
    return false;
}

static void defer_check_all_defer_until_label(struct defer_visit_ctx* ctx,
    struct defer_defer_scope* deferblock,
    const char* label,
    struct token* position_token,
    struct defer_list* p_defer_list)
{
    /*
    * We need to know how many scopes we exited until we found the label.
    * To do this, we look in the current scope for where the goto appears.
    * If the label is not directly in this scope or within some internal scope
    * No, we print the defers because we are exiting the scope and going to the scope
    * above. So we repeat this at each scope exit, printing the defer.
    */
    struct defer_defer_scope* _Opt p_defer = deferblock;

    while (p_defer != NULL)
    {
        if (!defer_find_label_scope(ctx, p_defer, label))
        {
            defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);
        }
        else
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

static void defer_check_all_defer_until_iter(struct defer_visit_ctx* ctx,
    struct defer_defer_scope* deferblock,
    struct token* position_token,
    struct defer_list* p_defer_list)
{
    struct defer_defer_scope* _Opt p_defer = deferblock;
    while (p_defer != NULL)
    {
        defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);
        if (p_defer->p_iteration_statement)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

static void defer_exit_iteration_or_switch_statement_visit(struct defer_visit_ctx* ctx,
    struct defer_defer_scope* deferblock,
    struct token* position_token,
    struct defer_list* p_defer_list)
{
    struct defer_defer_scope* _Opt p_defer = deferblock;
    while (p_defer != NULL)
    {
        defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);

        if (p_defer->p_iteration_statement)
        {
            //break using in for,do , while
            break;
        }

        if (p_defer->p_selection_statement &&
            p_defer->p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
        {
            //break switch case
            break;
        }

        p_defer = p_defer->previous;
    }
}

static void defer_exit_function_visit(struct defer_visit_ctx* ctx,
    struct defer_defer_scope* deferblock,
    struct token* position_token,
    struct defer_list* p_defer_list)
{
    struct defer_defer_scope* _Opt p_defer = deferblock;
    while (p_defer != NULL)
    {
        defer_exit_block_visit(ctx, p_defer, position_token, p_defer_list);
        p_defer = p_defer->previous;
    }
}

static void defer_visit_secondary_block(struct defer_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    defer_visit_statement(ctx, p_secondary_block->statement);
}

static void defer_visit_defer_statement(struct defer_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{
    struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
    if (p_defer)
    {
        p_defer->defer_statement = p_defer_statement;
    }
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
    struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
    if (p_defer == NULL)
        return;

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

    defer_exit_block_visit(ctx, p_defer, p_selection_statement->last_token, &p_selection_statement->defer_list);
    defer_visit_ctx_pop_tail_block(ctx);
}

static void defer_visit_try_statement(struct defer_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    try
    {
        struct secondary_block* _Opt catch_secondary_block_old = ctx->catch_secondary_block_opt;

        ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;

        struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == NULL)
        {
            //TODO restore
            throw;
        }

        p_defer->p_try_statement = p_try_statement;

        defer_visit_secondary_block(ctx, p_try_statement->secondary_block);

        if (p_try_statement->catch_secondary_block_opt)
        {
            defer_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
        }

        //defer_exit_block_visit(ctx, p_defer, p_try_statement->secondary_block->last_token, p_defer_list);
        defer_visit_ctx_pop_tail_block(ctx);
        ctx->catch_secondary_block_opt = catch_secondary_block_old; //restore
    }
    catch
    {
    }
}

static void defer_visit_switch_statement(struct defer_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
    if (p_defer == NULL)
    {
        return;
    }
    p_defer->p_selection_statement = p_selection_statement;
    defer_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    defer_exit_block_visit(ctx, p_defer, p_selection_statement->secondary_block->last_token, &p_selection_statement->defer_list);
    defer_visit_ctx_pop_tail_block(ctx);
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
        p_block_item = p_block_item->next;
    }
}

static void defer_visit_compound_statement(struct defer_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
    if (p_defer == NULL)
    {
        return;
    }
    p_defer->p_compound_statement = p_compound_statement;

    defer_visit_block_item_list(ctx, &p_compound_statement->block_item_list);

    defer_exit_block_visit(ctx, p_defer, p_compound_statement->last_token, &p_compound_statement->defer_list);

    defer_visit_ctx_pop_tail_block(ctx);
}

static void defer_visit_do_while_statement(struct defer_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);
    //const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
    try
    {
        struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == NULL) throw;
        p_defer->p_iteration_statement = p_iteration_statement;
        defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        defer_exit_block_visit(ctx, p_defer, p_iteration_statement->secondary_block->last_token, &p_iteration_statement->defer_list);
        defer_visit_ctx_pop_tail_block(ctx);
    }
    catch
    {
    }
}

static void defer_visit_while_statement(struct defer_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    if (p_iteration_statement->expression1 == NULL)
        return;

    struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
    if (p_defer == NULL)
    {
        return;
    }

    p_defer->p_iteration_statement = p_iteration_statement;

    defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    defer_exit_block_visit(ctx, p_defer, p_iteration_statement->secondary_block->last_token, &p_iteration_statement->defer_list);
    defer_visit_ctx_pop_tail_block(ctx);
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

        struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == NULL)
            throw;

        p_defer->p_iteration_statement = p_iteration_statement;
        defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        defer_visit_ctx_pop_tail_block(ctx);

        const bool b_secondary_block_ends_with_jump =
            secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

        /*we visit again*/
        if (!b_secondary_block_ends_with_jump)
        {
            struct defer_defer_scope* _Opt p_defer2 = defer_visit_ctx_push_tail_block(ctx);
            if (p_defer2 == NULL) throw;

            p_defer2->p_iteration_statement = p_iteration_statement;
            defer_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
            defer_exit_block_visit(ctx, p_defer2, p_iteration_statement->secondary_block->last_token, &p_iteration_statement->defer_list);
            defer_visit_ctx_pop_tail_block(ctx);
        }
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

static void defer_visit_jump_statement(struct defer_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    assert(ctx->tail_block != NULL);

    try
    {
        if (p_jump_statement->first_token->type == TK_KEYWORD_CAKE_THROW)
        {
            defer_check_all_defer_until_try(ctx, ctx->tail_block, p_jump_statement->first_token, &p_jump_statement->defer_list);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
        {
            defer_exit_function_visit(ctx, ctx->tail_block, p_jump_statement->first_token, &p_jump_statement->defer_list);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
        {
            defer_check_all_defer_until_iter(ctx, ctx->tail_block, p_jump_statement->first_token, &p_jump_statement->defer_list);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            defer_exit_iteration_or_switch_statement_visit(ctx, ctx->tail_block, p_jump_statement->first_token, &p_jump_statement->defer_list);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
        {
            assert(p_jump_statement->label != NULL);
            defer_check_all_defer_until_label(ctx,
                ctx->tail_block,
                p_jump_statement->label->lexeme,
                p_jump_statement->first_token,
                &p_jump_statement->defer_list);
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
    if (p_primary_block->defer_statement)
    {
        defer_visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else if (p_primary_block->compound_statement)
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
        //struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
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
          //  defer_exit_block_visit(ctx,
          //      ctx->tail_block,
          //      p_expression->compound_statement->last_token,
          //      &p_expression->defer_list);
        //}

        //defer_visit_ctx_pop_tail_block(ctx);
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

            struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_child(ctx);
            if (p_defer == NULL) throw;
            p_defer->declarator = p_declarator;
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
    if (p_declaration->function_body)
    {
        assert(ctx->tail_block == NULL);
        struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == NULL)
        {
            return;
        }
        p_defer->p_function_body = p_declaration->function_body;

        defer_visit_declaration(ctx, p_declaration);
        assert(p_declaration->function_body != NULL); //defer_visit_declaration does not change this

        //parameters
        if (ctx->tail_block)
        {
            defer_exit_block_visit(ctx,
                ctx->tail_block,
                p_declaration->function_body->last_token,
                &p_declaration->defer_list); //maybe use the same defer_list from body??
        }

        defer_visit_ctx_pop_tail_block(ctx);
    }
    else
    {
        struct defer_defer_scope* _Opt p_defer = defer_visit_ctx_push_tail_block(ctx);
        if (p_defer == NULL)
        {
            return;
        }
        defer_visit_declaration(ctx, p_declaration);
        defer_visit_ctx_pop_tail_block(ctx);
    }
}

void defer_visit_ctx_destroy(_Dtor struct defer_visit_ctx* p)
{
    assert(p->tail_block == NULL);
}

