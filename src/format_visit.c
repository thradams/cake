#include "ownership.h"
#include "format_visit.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



void ajust_line_and_identation(struct token* token, struct format_visit_ctx* ctx)
{
    /*
    * Before this token we must have a indentation and before indentation a new line.
    * If we don't have it we need to insert.
    */

    if (token && token->level == 0)
    {
        struct token* previous_token = token->prev;
        if (previous_token)
        {
            if (previous_token->type == TK_BLANKS)
            {
                char blanks[50] = { 0 };
                if (ctx->identation > 0)
                    snprintf(blanks, sizeof blanks, "%*c", (ctx->identation * 4), ' ');

                /*only adjust the number of spaces*/
                free(previous_token->lexeme);
                previous_token->lexeme = strdup(blanks);

                struct token* previous_previous_token =
                    previous_token->prev;

                if (previous_previous_token->type != TK_NEWLINE)
                {
                    struct tokenizer_ctx tctx = { 0 };
                    struct token_list list = tokenizer(&tctx, "\n", NULL, 0, TK_FLAG_NONE);
                    token_list_insert_after(&ctx->ast.token_list, previous_previous_token, &list);
                    token_list_destroy(&list);
                }
            }
            else if (previous_token->type != TK_NEWLINE)
            {
                char blanks[50] = {0};
                if (ctx->identation > 0)
                {
                    snprintf(blanks, sizeof blanks, "\n%*c", (ctx->identation * 4), ' ');
                }
                else
                {
                    snprintf(blanks, sizeof blanks, "\n");
                }

                struct tokenizer_ctx tctx = { 0 };
                struct token_list list = tokenizer(&tctx, blanks, NULL, 0, TK_FLAG_NONE);
                token_list_insert_after(&ctx->ast.token_list, previous_token, &list);
                token_list_destroy(&list);
            }
        }
    }
}

void ajust_if_begin(struct token* token, struct format_visit_ctx* ctx)
{
    /*
    * if we have 
      newline blancks
      then we ident
    */
    if (token && token->level == 0)
    {
        struct token* previous_token = token->prev;
        if (previous_token && previous_token->type == TK_BLANKS)
        {
            struct token* previous_previous_token =
                previous_token->prev;
            if (previous_previous_token &&
                previous_previous_token->type == TK_NEWLINE)
            {
                char blanks[50] = { 0 };
                if (ctx->identation > 0)
                    snprintf(blanks, sizeof blanks, "%*c", (ctx->identation * 4), ' ');

                /*only adjust the number of spaces*/
                free(previous_token->lexeme);
                previous_token->lexeme = strdup(blanks);
            }
        }
    }
}

static void format_visit_unlabeled_statement(struct format_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement);
static void format_visit_labeled_statement(struct format_visit_ctx* ctx, struct labeled_statement* p_labeled_statement);

static void format_visit_statement(struct format_visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        format_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        format_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}



static void format_visit_selection_statement(struct format_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    if (p_selection_statement->secondary_block)
    {
        ajust_line_and_identation(p_selection_statement->secondary_block->first_token, ctx);

        if (p_selection_statement->secondary_block &&
            p_selection_statement->secondary_block->statement &&
            p_selection_statement->secondary_block->statement->unlabeled_statement &&
            p_selection_statement->secondary_block->statement->unlabeled_statement->primary_block &&
            p_selection_statement->secondary_block->statement->unlabeled_statement->primary_block->compound_statement)
        {
            format_visit_statement(ctx, p_selection_statement->secondary_block->statement);
        }
        else
        {
            ajust_line_and_identation(p_selection_statement->secondary_block->first_token, ctx);

            format_visit_statement(ctx, p_selection_statement->secondary_block->statement);            
        }        
    }

    if (p_selection_statement->else_secondary_block_opt)
    {
        ajust_line_and_identation(p_selection_statement->else_token_opt, ctx);
        ajust_line_and_identation(p_selection_statement->else_secondary_block_opt->first_token, ctx);

        if (p_selection_statement->else_secondary_block_opt->statement &&
            p_selection_statement->else_secondary_block_opt->statement->unlabeled_statement &&
            p_selection_statement->else_secondary_block_opt->statement->unlabeled_statement->primary_block &&
            p_selection_statement->else_secondary_block_opt->statement->unlabeled_statement->primary_block->compound_statement)
        {
            /*compound statement*/
            format_visit_statement(ctx, p_selection_statement->else_secondary_block_opt->statement);
        }
        else
        {            
            format_visit_statement(ctx, p_selection_statement->else_secondary_block_opt->statement);         
        }
    }

}

static void format_visit_jump_statement(struct format_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{



    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW ||
        p_jump_statement->first_token->type == TK_KEYWORD_RETURN ||
        p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE ||
        p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {
        ajust_line_and_identation(p_jump_statement->first_token, ctx);
    }
    else
    {
        assert(false);
    }
}

static void format_visit_compound_statement(struct format_visit_ctx* ctx, struct compound_statement* p_compound_statement);


static void format_visit_secondary_block(struct format_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    format_visit_statement(ctx, p_secondary_block->statement);
}

static void format_visit_iteration_statement(struct format_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    ajust_line_and_identation(p_iteration_statement->first_token, ctx);

    if (p_iteration_statement->expression1)
    {
        //format_visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->expression2)
    {
        //format_visit_expression(ctx, p_iteration_statement->expression2);
    }

    if (p_iteration_statement->first_token->type == TK_KEYWORD_DO)
    {
        ajust_line_and_identation(p_iteration_statement->second_token, ctx);
    }

    if (p_iteration_statement->secondary_block)
    {
        format_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }
}

static void format_visit_try_statement(struct format_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    if (p_try_statement->secondary_block)
        format_visit_secondary_block(ctx, p_try_statement->secondary_block);

    if (p_try_statement->catch_secondary_block_opt)
    {
        format_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
    }
}


static void format_visit_primary_block(struct format_visit_ctx* ctx, struct primary_block* p_primary_block)
{

    if (p_primary_block->compound_statement)
    {
        format_visit_compound_statement(ctx, p_primary_block->compound_statement);
    }
    else if (p_primary_block->iteration_statement)
    {
        format_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
    }
    else if (p_primary_block->selection_statement)
    {
        format_visit_selection_statement(ctx, p_primary_block->selection_statement);
    }
    else if (p_primary_block->defer_statement)
    {
        //visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else if (p_primary_block->try_statement)
    {
        format_visit_try_statement(ctx, p_primary_block->try_statement);
    }
    else
    {
        assert(false);
    }
}


static void format_visit_expression_statement(struct format_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
    {
        //ajust_line_and_identation(p_expression_statement->first_token, ctx);
    }
}

static void format_visit_labeled_statement(struct format_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    ajust_line_and_identation(p_labeled_statement->label->name, ctx);

    if (p_labeled_statement->statement)
        format_visit_statement(ctx, p_labeled_statement->statement);
}

static void format_visit_unlabeled_statement(struct format_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        format_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        format_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        format_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}

static void format_visit_block_item(struct format_visit_ctx* ctx, struct block_item* p_block_item)
{
    ajust_line_and_identation(p_block_item->first_token, ctx);

    if (p_block_item->declaration)
    {
        //visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        format_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        //visit_labeled_statement(ctx, p_block_item->labeled_statement);
    }
}

static void format_visit_block_item_list(struct format_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        format_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void format_visit_compound_statement(struct format_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    ajust_line_and_identation(p_compound_statement->first_token, ctx);

    ctx->identation++;
    format_visit_block_item_list(ctx, &p_compound_statement->block_item_list);

    ctx->identation++;
    /*fix comments anything that is not part of AST*/
    struct token* tk = p_compound_statement->first_token;
    while (tk)
    {
        if (tk->type == TK_LINE_COMMENT ||
            tk->type == TK_COMMENT)
        {
            ajust_if_begin(tk, ctx);
        }
        tk = tk->next;
    }
    ctx->identation--;

    ctx->identation--;

    ajust_line_and_identation(p_compound_statement->last_token, ctx);
}

static void format_visit_declaration(struct format_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        //format_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }

    if (p_declaration->declaration_specifiers)
    {
        //format_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers);

    }

    if (p_declaration->init_declarator_list.head)
    {
        //format_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        format_visit_compound_statement(ctx, p_declaration->function_body);
    }
}

void format_visit(struct format_visit_ctx* ctx)
{
    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        format_visit_declaration(ctx, p_declaration);
        p_declaration = p_declaration->next;
    }
}
