#include "ownership.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "visit.h"
#include "expressions.h"
#include "ownership.h"

void defer_scope_delete_all(struct defer_scope* owner p);

void visit_ctx_destroy(struct visit_ctx* obj_owner ctx)
{
    defer_scope_delete_all(ctx->tail_block);
    token_list_destroy(&ctx->insert_before_declaration);
    token_list_destroy(&ctx->insert_before_block_item);
}

static void visit_attribute_specifier_sequence(struct visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence);

struct token* next_parser_token(struct token* token)
{
    struct token* r = token->next;
    while (!(r->flags & TK_FLAG_FINAL))
    {
        r = r->next;
    }
    return r;
}

static void visit_struct_or_union_specifier(struct visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void visit_expression(struct visit_ctx* ctx, struct expression* p_expression);
static void visit_statement(struct visit_ctx* ctx, struct statement* p_statement);
static void visit_enum_specifier(struct visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void visit_type_specifier(struct visit_ctx* ctx, struct type_specifier* p_type_specifier);

void convert_if_statement(struct visit_ctx* ctx, struct selection_statement* p_selection_statement)
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
        token_list_remove_get(&ctx->ast.token_list, first, token);

    token_list_insert_after(&ctx->ast.token_list,
        before_first_token,
        &list);

    struct tokenizer_ctx tctx = {0};
    struct token_list list1 = tokenizer(&tctx, "{", "", 0, TK_FLAG_FINAL);
    token_list_insert_after(&ctx->ast.token_list,
        before_first_token,
        &list1);
    struct token_list list2 = tokenizer(&tctx, "}", "", 0, TK_FLAG_FINAL);
    token_list_insert_after(&ctx->ast.token_list,
        p_selection_statement->last_token,
        &list2);
    token_list_destroy(&list2);
    token_list_destroy(&list1);
    token_list_destroy(&list);
}

void print_block_defer(struct defer_scope* defer_block, struct osstream* ss, bool hide_tokens)
{
    struct defer_scope* defer_child = defer_block->lastchild;
    while (defer_child != NULL)
    {
        view struct token_list l = {0};

        l.head = defer_child->defer_statement->first_token;
        l.tail = defer_child->defer_statement->last_token;

        l.head->flags |= TK_FLAG_HIDE;
        const char* owner s = get_code_as_compiler_see(&l);
        assert(s != NULL);
        if (hide_tokens)
            token_range_add_flag(l.head, l.tail, TK_FLAG_HIDE);

        ss_fprintf(ss, "%s", s);
        free((void* owner)s);
        defer_child = defer_child->previous;
    }
}


void hide_block_defer(struct defer_scope* deferblock)
{
    struct defer_scope* deferchild = deferblock->lastchild;
    while (deferchild != NULL)
    {
        struct token* head = deferchild->defer_statement->first_token;
        struct token* tail = deferchild->defer_statement->last_token;
        token_range_add_flag(head, tail, TK_FLAG_HIDE);
        deferchild = deferchild->previous;
    }
}


void print_all_defer_until_try(struct defer_scope* deferblock, struct osstream* ss)
{
    struct defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        print_block_defer(p_defer, ss, false);

        if (p_defer->p_try_statement)
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
                if (block_item &&
                    block_item->label &&
                    block_item->label->name &&
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

                block_item = block_item->next;
            }
        }
        else if (p_unlabeled_statement->primary_block->selection_statement)
        {
            if (find_label_statement(p_unlabeled_statement->primary_block->selection_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt)
            {
                if (find_label_statement(p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->try_statement)
        {
            if (find_label_statement(p_unlabeled_statement->primary_block->try_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt)
            {
                if (find_label_statement(p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt->statement, label))
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

        if (deferblock->p_selection_statement2->else_secondary_block_opt)
        {
            if (find_label_statement(deferblock->p_selection_statement2->else_secondary_block_opt->statement, label))
                return true;
        }
    }
    else if (deferblock->p_try_statement)
    {
        if (deferblock->p_try_statement->secondary_block)
        {
            if (find_label_statement(deferblock->p_try_statement->secondary_block->statement, label))
                return true;
        }

        if (deferblock->p_try_statement->catch_secondary_block_opt)
        {
            if (find_label_statement(deferblock->p_try_statement->catch_secondary_block_opt->statement, label))
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

static void visit_secondary_block(struct visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    visit_statement(ctx, p_secondary_block->statement);
}
struct defer_scope* visit_ctx_push_tail_child(struct visit_ctx* ctx)
{
    struct defer_scope* owner p_defer = calloc(1, sizeof * p_defer);
    p_defer->previous = ctx->tail_block->lastchild;
    ctx->tail_block->lastchild = p_defer;

    return ctx->tail_block->lastchild;
}


struct defer_scope* visit_ctx_push_tail_block(struct visit_ctx* ctx)
{
    struct defer_scope* owner p_defer = calloc(1, sizeof * p_defer);
    p_defer->previous = ctx->tail_block;
    ctx->tail_block = p_defer;

    return ctx->tail_block;
}
static void visit_defer_statement(struct visit_ctx* ctx, struct defer_statement* p_defer_statement)
{
    if (!ctx->is_second_pass)
    {
        //adiciona como filho do ultimo bloco
        struct defer_scope* p_defer = visit_ctx_push_tail_child(ctx);
        p_defer->defer_statement = p_defer_statement;


        if (p_defer_statement->secondary_block)
        {
            visit_secondary_block(ctx, p_defer_statement->secondary_block);
        }
    }
    else //if (ctx->is_second_pass)
    {
        if (p_defer_statement->secondary_block)
            visit_secondary_block(ctx, p_defer_statement->secondary_block);
    }
}



void defer_scope_delete_one(struct defer_scope* owner p_block);

void visit_ctx_pop_tail_block(struct visit_ctx* ctx)
{
    if (ctx->tail_block)
    {
        struct defer_scope* owner previous = ctx->tail_block->previous;
        ctx->tail_block->previous = NULL;
        defer_scope_delete_one(ctx->tail_block);
        ctx->tail_block = previous;
    }
}

static void visit_try_statement(struct visit_ctx* ctx, struct try_statement* p_try_statement)
{
    if (!ctx->is_second_pass)
    {
        struct defer_scope* p_defer = visit_ctx_push_tail_block(ctx);
        p_defer->p_try_statement = p_try_statement;

        if (p_try_statement->secondary_block)
            visit_secondary_block(ctx, p_try_statement->secondary_block);


        struct osstream ss = {0};

        print_block_defer(p_defer, &ss, true);
        struct tokenizer_ctx tctx = {0};
        struct token_list l = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, p_try_statement->secondary_block->last_token->prev, &l);


        visit_ctx_pop_tail_block(ctx);


        free(p_try_statement->first_token->lexeme);
        p_try_statement->first_token->lexeme = strdup("if (1) /*try*/");


        char buffer[50] = {0};
        if (p_try_statement->catch_secondary_block_opt)
        {

            snprintf(buffer, sizeof buffer, "else _catch_label_%d:", p_try_statement->try_catch_block_index);

            free(p_try_statement->catch_token_opt->lexeme);
            p_try_statement->catch_token_opt->lexeme = strdup(buffer);

            visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
        }
        else
        {
            snprintf(buffer, sizeof buffer, "} else {_catch_label_%d:;}", p_try_statement->try_catch_block_index);
            free(p_try_statement->last_token->lexeme);
            p_try_statement->last_token->lexeme = strdup(buffer);
        }

        ss_close(&ss);
        token_list_destroy(&l);
    }
}

static void visit_selection_statement(struct visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    convert_if_statement(ctx, p_selection_statement);

    //PUSH
    struct defer_scope* p_defer = visit_ctx_push_tail_block(ctx);
    p_defer->p_selection_statement2 = p_selection_statement;

    if (p_selection_statement->secondary_block)
        visit_secondary_block(ctx, p_selection_statement->secondary_block);

    struct osstream ss = {0};
    print_block_defer(p_defer, &ss, true);

    if (ss.size > 0)
    {
        struct tokenizer_ctx tctx = {0};
        struct token_list l = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, p_selection_statement->secondary_block->last_token->prev, &l);
        token_list_destroy(&l);
    }

    visit_ctx_pop_tail_block(ctx);

    if (p_selection_statement->else_secondary_block_opt)
        visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);

    ss_close(&ss);
}

static void visit_compound_statement(struct visit_ctx* ctx, struct compound_statement* p_compound_statement);



char* remove_char(char* input, char ch)
{
    if (input == NULL)
        return NULL;
    char* p_write = input;
    const char* p = input;
    while (*p)
    {
        if (p[0] == ch)
        {
            p++;
        }
        else
        {
            *p_write = *p;
            p++;
            p_write++;
        }
    }
    *p_write = 0;
    return input;
}

static void visit_initializer_list(struct visit_ctx* ctx, struct initializer_list* p_initializer_list);

static void visit_bracket_initializer_list(struct visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {
        if (ctx->target < LANGUAGE_C2X)
        {
            assert(p_bracket_initializer_list->first_token->type == '{');

            const int level = p_bracket_initializer_list->first_token->level;
            //Criar token 0
            struct tokenizer_ctx tctx = {0};
            struct token_list list2 = tokenizer(&tctx, "0", NULL, level, TK_FLAG_FINAL);

            //inserir na frente
            token_list_insert_after(&ctx->ast.token_list, p_bracket_initializer_list->first_token, &list2);
            token_list_destroy(&list2);
        }
    }
    else
    {
        visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void visit_designation(struct visit_ctx* ctx, struct designation* p_designation)
{
}

static void visit_initializer(struct visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->p_attribute_specifier_sequence_opt)
    {
        visit_attribute_specifier_sequence(ctx, p_initializer->p_attribute_specifier_sequence_opt);
    }

    if (p_initializer->designation)
    {
        visit_designation(ctx, p_initializer->designation);
    }

    if (p_initializer->assignment_expression)
    {
        visit_expression(ctx, p_initializer->assignment_expression);
    }
    else if (p_initializer->braced_initializer)
    {
        visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void visit_initializer_list(struct visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer* p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        visit_initializer(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void visit_type_qualifier(struct visit_ctx* ctx, struct type_qualifier* p_type_qualifier)
{
    if (p_type_qualifier->token)
    {
        if (ctx->target < LANGUAGE_C99 && p_type_qualifier->token->type == TK_KEYWORD_RESTRICT)
        {
            free(p_type_qualifier->token->lexeme);
            p_type_qualifier->token->lexeme = strdup("/*restrict*/");
        }

        if (p_type_qualifier->token->type == TK_KEYWORD__OWNER ||
            p_type_qualifier->token->type == TK_KEYWORD__OBJ_OWNER ||
            p_type_qualifier->token->type == TK_KEYWORD__VIEW)
        {
            char temp[100] = {0};
            snprintf(temp, sizeof temp, "/*%s*/", p_type_qualifier->token->lexeme);
            free(p_type_qualifier->token->lexeme);
            p_type_qualifier->token->lexeme = strdup(temp);
        }
    }
}

static void visit_specifier_qualifier(struct visit_ctx* ctx, struct type_specifier_qualifier* p_specifier_qualifier)
{
    if (p_specifier_qualifier->type_specifier)
        visit_type_specifier(ctx, p_specifier_qualifier->type_specifier);

    if (p_specifier_qualifier->type_qualifier)
        visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier);
}

static void visit_specifier_qualifier_list(struct visit_ctx* ctx, struct specifier_qualifier_list* p_specifier_qualifier_list_opt,
    struct type* p_type)
{

    //(typeof(int[2])*)
    // 
    //TODO se tiver typeof em qualquer parte tem que imprimir todo  tipo
    // tem que refazer
    if (p_specifier_qualifier_list_opt->type_specifier_flags & TYPE_SPECIFIER_TYPEOF)
    {
        const int level = p_specifier_qualifier_list_opt->first_token->level;

        token_range_add_flag(p_specifier_qualifier_list_opt->first_token,
            p_specifier_qualifier_list_opt->last_token, TK_FLAG_HIDE);

        struct osstream ss = {0};
        print_type(&ss, type_get_specifer_part(p_type));


        struct tokenizer_ctx tctx = {0};
        struct token_list l2 = tokenizer(&tctx, ss.c_str, NULL, level, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, p_specifier_qualifier_list_opt->last_token, &l2);

        ss_close(&ss);
        token_list_destroy(&l2);
    }


    if (p_specifier_qualifier_list_opt == NULL)
        return;

    if (p_specifier_qualifier_list_opt->struct_or_union_specifier)
    {
        visit_struct_or_union_specifier(ctx, p_specifier_qualifier_list_opt->struct_or_union_specifier);
    }
    else if (p_specifier_qualifier_list_opt->enum_specifier)
    {
        visit_enum_specifier(ctx, p_specifier_qualifier_list_opt->enum_specifier);
    }
    else if (p_specifier_qualifier_list_opt->typedef_declarator)
    {
        //typedef name
    }
    //else if (p_specifier_qualifier_list->p_typeof_expression_opt)
    //{
      //  visit_expression(ctx, p_specifier_qualifier_list->p_typeof_expression_opt);
    //}
    else
    {
        struct type_specifier_qualifier* p_specifier_qualifier = p_specifier_qualifier_list_opt->head;
        while (p_specifier_qualifier)
        {
            visit_specifier_qualifier(ctx, p_specifier_qualifier);
            p_specifier_qualifier = p_specifier_qualifier->next;
        }
    }
}
static void visit_declarator(struct visit_ctx* ctx, struct declarator* p_declarator);
static void visit_type_name(struct visit_ctx* ctx, struct type_name* p_type_name)
{

    visit_specifier_qualifier_list(ctx, p_type_name->specifier_qualifier_list, &p_type_name->type);
    visit_declarator(ctx, p_type_name->declarator);


    /*
    * Vamos esconder tudo e gerar um novo
    *  Exemplo
    *  (const typeof(int (*)())) -> *  ( int (*const )() )
    */
}



static void visit_argument_expression_list(struct visit_ctx* ctx, struct argument_expression_list* p_argument_expression_list)
{
    struct argument_expression* p_argument_expression =
        p_argument_expression_list->head;
    while (p_argument_expression)
    {
        visit_expression(ctx, p_argument_expression->expression);
        p_argument_expression = p_argument_expression->next;
    }
}

static void visit_generic_selection(struct visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        visit_expression(ctx, p_generic_selection->expression);
    }
    else if (p_generic_selection->type_name)
    {
        visit_type_name(ctx, p_generic_selection->type_name);
    }

    struct generic_association* p = p_generic_selection->generic_assoc_list.head;
    while (p)
    {
        visit_type_name(ctx, p->p_type_name);
        visit_expression(ctx, p->expression);
        p = p->next;
    }

    if (ctx->target < LANGUAGE_C11)
    {
        //TODO lambada inside teh expression
        /*let's hide everything first*/
        token_range_add_flag(p_generic_selection->first_token, p_generic_selection->last_token, TK_FLAG_HIDE);

        if (p_generic_selection->p_view_selected_expression)
        {
            /*then let's show up the selected expression*/
            token_range_remove_flag(p_generic_selection->p_view_selected_expression->first_token,
                p_generic_selection->p_view_selected_expression->last_token,
                TK_FLAG_HIDE);
        }
    }

}


static void visit_expression(struct visit_ctx* ctx, struct expression* p_expression)
{
    switch (p_expression->expression_type)
    {
        case PRIMARY_EXPRESSION__FUNC__:
            break;
        case PRIMARY_IDENTIFIER:
            break;
        case PRIMARY_EXPRESSION_ENUMERATOR:
            if (ctx->target < LANGUAGE_C2X)
            {
                struct type t = type_get_enum_type(&p_expression->type);
                if (t.type_specifier_flags != TYPE_SPECIFIER_INT)
                {
                    struct osstream ss0 = {0};
                    print_type(&ss0, &t);
                    struct osstream ss = {0};
                    ss_fprintf(&ss, "((%s)%s)", ss0.c_str, p_expression->first_token->lexeme);

                    free(p_expression->first_token->lexeme);
                    p_expression->first_token->lexeme = ss.c_str;
                    ss.c_str = NULL; /*MOVED*/
                    ss_close(&ss);
                    ss_close(&ss0);
                }
                type_destroy(&t);
            }
            break;
        case PRIMARY_EXPRESSION_DECLARATOR:

            if (ctx->target < LANGUAGE_C2X)
            {
                if (constant_value_is_valid(&p_expression->constant_value))
                {
                    free((void* owner)p_expression->type.name_opt);
                    p_expression->type.name_opt = NULL;

                    struct osstream ss1 = {0};
                    struct osstream ss = {0};
                    print_type(&ss, &p_expression->type);
                    /*
                      this is the way we handle constexpr, replacing the declarator
                      for it's number and changing the expression type
                      we are not handling &a at this moment
                    */
                    char buffer[40] = {0};
                    constant_value_to_string(&p_expression->constant_value, buffer, sizeof buffer);
                    free(p_expression->first_token->lexeme);

                    ss_fprintf(&ss1, "((%s)%s)", ss.c_str, buffer);

                    p_expression->first_token->lexeme = ss1.c_str;
                    ss1.c_str = NULL;// MOVED
                    p_expression->expression_type = PRIMARY_EXPRESSION_NUMBER;


                    ss_close(&ss);
                    ss_close(&ss1);
                }
            }

            break;
        case PRIMARY_EXPRESSION_STRING_LITERAL:
            break;
        case PRIMARY_EXPRESSION_CHAR_LITERAL:
            break;
        case PRIMARY_EXPRESSION_NUMBER:
            break;

        case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
            if (p_expression->first_token &&
                p_expression->first_token->type == TK_KEYWORD_NULLPTR)
            {
                if (ctx->target < LANGUAGE_C2X)
                {
                    free(p_expression->first_token->lexeme);
                    p_expression->first_token->lexeme = strdup("((void*)0)");
                }
            }
            else if (p_expression->first_token &&
                p_expression->first_token->type == TK_KEYWORD_TRUE)
            {
                if (ctx->target < LANGUAGE_C99)
                {
                    free(p_expression->first_token->lexeme);
                    p_expression->first_token->lexeme = strdup("1");
                }
                else if (ctx->target < LANGUAGE_C2X)
                {
                    free(p_expression->first_token->lexeme);
                    p_expression->first_token->lexeme = strdup("((_Bool)1)");
                }
            }
            else if (p_expression->first_token &&
                p_expression->first_token->type == TK_KEYWORD_FALSE)
            {
                if (ctx->target < LANGUAGE_C99)
                {
                    free(p_expression->first_token->lexeme);
                    p_expression->first_token->lexeme = strdup("0");
                }
                else if (ctx->target < LANGUAGE_C2X)
                {
                    free(p_expression->first_token->lexeme);
                    p_expression->first_token->lexeme = strdup("((_Bool)0)");
                }
            }
            break;

        case PRIMARY_EXPRESSION_PARENTESIS:
            visit_expression(ctx, p_expression->right);
            break;

        case PRIMARY_EXPRESSION_GENERIC:
            visit_generic_selection(ctx, p_expression->generic_selection);
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
            //visit_expression(ctx, p_expression->left);
            break;
        case POSTFIX_FUNCTION_CALL:
            visit_expression(ctx, p_expression->left);
            visit_argument_expression_list(ctx, &p_expression->argument_expression_list);
            break;
        case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
        {
            ctx->has_lambda = true;
            ctx->is_inside_lambda = true;
            visit_type_name(ctx, p_expression->type_name);
            visit_compound_statement(ctx, p_expression->compound_statement);
            ctx->is_inside_lambda = false;

            if (ctx->is_second_pass)
            {
                /*no segundo passo nós copiamos a funcao*/
                char name[100] = {0};
                snprintf(name, sizeof name, " _lit_func_%d", ctx->lambdas_index);
                ctx->lambdas_index++;

                struct osstream ss = {0};



                bool is_first = true;
                print_type_qualifier_flags(&ss, &is_first, p_expression->type_name->declarator->type.type_qualifier_flags);
                print_type_specifier_flags(&ss, &is_first, p_expression->type_name->declarator->type.type_specifier_flags);


                free((void* owner)p_expression->type_name->declarator->type.name_opt);
                p_expression->type_name->declarator->type.name_opt = strdup(name);

                struct osstream ss0 = {0};

                print_type(&ss0, &p_expression->type_name->declarator->type);
                ss_fprintf(&ss, "static %s", ss0.c_str);

                ss_close(&ss0);

                struct tokenizer_ctx tctx = {0};
                struct token_list l1 = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);

                token_list_append_list(&ctx->insert_before_declaration, &l1);
                ss_close(&ss);
                token_list_destroy(&l1);

                for (struct token* current = p_expression->compound_statement->first_token;
                    current != p_expression->compound_statement->last_token->next;
                    current = current->next)
                {
                    token_list_clone_and_add(&ctx->insert_before_declaration, current);
                }

                token_range_add_flag(p_expression->first_token, p_expression->last_token, TK_FLAG_HIDE);


                struct token_list l3 = tokenizer(&tctx, "\n\n", NULL, 0, TK_FLAG_NONE);
                token_list_append_list(&ctx->insert_before_declaration, &l3);
                token_list_destroy(&l3);

                struct token_list l2 = tokenizer(&tctx, name, NULL, 0, TK_FLAG_FINAL);
                token_list_insert_after(&ctx->ast.token_list, p_expression->last_token, &l2);
                token_list_destroy(&l2);
            }
        }
        break;

        case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

            if (p_expression->type_name)
            {
                visit_type_name(ctx, p_expression->type_name);
            }

            visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

            assert(p_expression->left == NULL);
            assert(p_expression->right == NULL);

            break;

        case UNARY_EXPRESSION_ALIGNOF:

            if (ctx->target < LANGUAGE_C11)
            {
                const int level = p_expression->first_token->level;
                token_range_add_flag(p_expression->first_token, p_expression->last_token, TK_FLAG_HIDE);
                char buffer[30] = {0};
                snprintf(buffer, sizeof buffer, "%lld", constant_value_to_ll(&p_expression->constant_value));
                struct tokenizer_ctx tctx = {0};
                struct token_list l3 = tokenizer(&tctx, buffer, NULL, level, TK_FLAG_FINAL);
                l3.head->flags = p_expression->last_token->flags;
                token_list_insert_after(&ctx->ast.token_list, p_expression->last_token, &l3);
                token_list_destroy(&l3);
            }

            if (p_expression->right)
            {
                visit_expression(ctx, p_expression->right);
            }

            if (p_expression->type_name)
            {
                /*sizeof*/
                visit_type_name(ctx, p_expression->type_name);
            }
            break;

        case UNARY_EXPRESSION_SIZEOF_EXPRESSION:
        case UNARY_EXPRESSION_SIZEOF_TYPE:
        case UNARY_EXPRESSION_INCREMENT:
        case UNARY_EXPRESSION_DECREMENT:

        case UNARY_EXPRESSION_NOT:
        case UNARY_EXPRESSION_BITNOT:
        case UNARY_EXPRESSION_NEG:
        case UNARY_EXPRESSION_PLUS:
        case UNARY_EXPRESSION_CONTENT:
        case UNARY_EXPRESSION_ADDRESSOF:
            if (p_expression->right)
            {
                visit_expression(ctx, p_expression->right);
            }

            if (p_expression->type_name)
            {
                /*sizeof*/
                visit_type_name(ctx, p_expression->type_name);
            }

            break;




        case ASSIGNMENT_EXPRESSION:
        case CAST_EXPRESSION:
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
                visit_expression(ctx, p_expression->left);
            }
            if (p_expression->right)
            {
                visit_expression(ctx, p_expression->right);
            }
            if (p_expression->type_name)
            {
                visit_type_name(ctx, p_expression->type_name);
            }


            break;

        case UNARY_EXPRESSION_TRAITS:
        {
            if (ctx->target < LANGUAGE_CXX)
            {
                struct tokenizer_ctx tctx = {0};
                struct token_list l2 = {0};

                if (constant_value_to_bool(&p_expression->constant_value))
                    l2 = tokenizer(&tctx, "1", NULL, 0, TK_FLAG_FINAL);
                else
                    l2 = tokenizer(&tctx, "0", NULL, 0, TK_FLAG_FINAL);


                token_list_insert_after(&ctx->ast.token_list,
                    p_expression->last_token,
                    &l2);

                token_range_add_flag(p_expression->first_token,
                    p_expression->last_token,
                    TK_FLAG_HIDE);

                token_list_destroy(&l2);
            }
        }
        break;

        case UNARY_EXPRESSION_IS_SAME:
            break;

        case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
            break;

        case CONDITIONAL_EXPRESSION:
            if (p_expression->condition_expr)
            {
                visit_expression(ctx, p_expression->condition_expr);
            }

            if (p_expression->left)
            {
                visit_expression(ctx, p_expression->left);
            }
            if (p_expression->right)
            {
                visit_expression(ctx, p_expression->right);
            }

            break;

        default:
            break;
    }
}

static void visit_expression_statement(struct visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
        visit_expression(ctx, p_expression_statement->expression_opt);
}

static void visit_block_item_list(struct visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void visit_compound_statement(struct visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    visit_block_item_list(ctx, &p_compound_statement->block_item_list);
}

//
static void visit_iteration_statement(struct visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{

    if (p_iteration_statement->expression1)
    {
        visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->expression2)
    {
        visit_expression(ctx, p_iteration_statement->expression2);
    }
    

    if (p_iteration_statement->secondary_block)
    {
        struct defer_scope* p_defer = visit_ctx_push_tail_block(ctx);
        p_defer->p_iteration_statement = p_iteration_statement;

        visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        struct osstream ss = {0};
        //defer_print(defer, &ss, ctx->bHasThrowWithVariable, ctx->bHasBreakWithVariable, ctx->bHasReturnWithVariable);
        print_block_defer(p_defer, &ss, true);

        struct tokenizer_ctx tctx = {0};
        struct token_list l = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, p_iteration_statement->secondary_block->last_token->prev, &l);


        visit_ctx_pop_tail_block(ctx);


        ss_close(&ss);
        token_list_destroy(&l);
    }
}



static void visit_jump_statement(struct visit_ctx* ctx, struct jump_statement* p_jump_statement)
{

    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
    {
        struct osstream ss0 = {0};
        print_all_defer_until_try(ctx->tail_block, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = {0};
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "goto _catch_label_%d;", p_jump_statement->try_catch_block_index);
            ss_fprintf(&ss, "}");

            free(p_jump_statement->first_token->lexeme);
            p_jump_statement->first_token->lexeme = ss.c_str;


            p_jump_statement->last_token->flags |= TK_FLAG_HIDE;

        }
        else
        {
            struct osstream ss = {0};
            ss_fprintf(&ss, "goto _catch_label_%d", p_jump_statement->try_catch_block_index);
            free(p_jump_statement->first_token->lexeme);
            p_jump_statement->first_token->lexeme = ss.c_str; /*MOVED*/
        }

        ss_close(&ss0);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        struct osstream ss0 = {0};
        print_all_defer_until_end(ctx->tail_block, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = {0};
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "return");
            free(p_jump_statement->first_token->lexeme);

            p_jump_statement->first_token->lexeme = ss.c_str;
            ss.c_str = NULL; /*MOVED*/

            free(p_jump_statement->last_token->lexeme);
            p_jump_statement->last_token->lexeme = strdup(";}");
            ss_close(&ss);
        }
        ss_close(&ss0);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {
        struct osstream ss0 = {0};

        print_all_defer_until_iter(ctx->tail_block, &ss0);
        if (ss0.size > 0)
        {
            struct osstream ss = {0};
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "break;");
            ss_fprintf(&ss, "}");
            free(p_jump_statement->first_token->lexeme);
            p_jump_statement->first_token->lexeme = ss.c_str;
            ss.c_str = NULL;

            p_jump_statement->last_token->flags |= TK_FLAG_HIDE;
            ss_close(&ss);
        }

        ss_close(&ss0);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {
        struct osstream ss0 = {0};
        print_all_defer_until_label(ctx->tail_block, p_jump_statement->label->lexeme, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = {0};
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "goto");
            free(p_jump_statement->first_token->lexeme);
            p_jump_statement->first_token->lexeme = ss.c_str;
            ss.c_str = NULL; /*MOVED*/
            free(p_jump_statement->last_token->lexeme);
            p_jump_statement->last_token->lexeme = strdup(";}");
            ss_close(&ss);
        }

        ss_close(&ss0);
    }
    else
    {
        assert(false);
    }
}


static void visit_labeled_statement(struct visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    //p_labeled_statement->label

    if (p_labeled_statement->statement)
    {
        visit_statement(ctx, p_labeled_statement->statement);
    }
}

static void visit_primary_block(struct visit_ctx* ctx, struct primary_block* p_primary_block)
{



    if (p_primary_block->defer_statement)
    {
        visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            visit_compound_statement(ctx, p_primary_block->compound_statement);
        }
        else if (p_primary_block->iteration_statement)
        {
            visit_iteration_statement(ctx, p_primary_block->iteration_statement);
        }
        else if (p_primary_block->selection_statement)
        {
            visit_selection_statement(ctx, p_primary_block->selection_statement);
        }
        else if (p_primary_block->try_statement)
        {
            visit_try_statement(ctx, p_primary_block->try_statement);
        }
    }

}

static void visit_unlabeled_statement(struct visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}

static void visit_declaration(struct visit_ctx* ctx, struct declaration* p_declaration);

static void visit_statement(struct visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void visit_label(struct visit_ctx* ctx, struct label* p_label)
{
    //p_label->name
}
static void visit_block_item(struct visit_ctx* ctx, struct block_item* p_block_item)
{
    if (p_block_item->declaration)
    {
        visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        visit_label(ctx, p_block_item->label);
    }
    if (ctx->insert_before_block_item.head != NULL)
    {
        token_list_insert_after(&ctx->ast.token_list, p_block_item->first_token->prev, &ctx->insert_before_block_item);
    }
}

static void visit_block_item_list(struct visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void visit_static_assert_declaration(struct visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration)
{
    visit_expression(ctx, p_static_assert_declaration->constant_expression);

    if (ctx->target < LANGUAGE_C11)
    {
        /*let's hide everything first*/
        token_range_add_flag(p_static_assert_declaration->first_token,
            p_static_assert_declaration->last_token,
            TK_FLAG_HIDE);
    }
    else if (ctx->target == LANGUAGE_C11)
    {
        if (p_static_assert_declaration->string_literal_opt == NULL)
        {
            struct token* rp = previous_parser_token(p_static_assert_declaration->last_token);
            rp = previous_parser_token(rp);

            struct tokenizer_ctx tctx = {0};
            struct token_list list1 = tokenizer(&tctx, ", \"error\"", "", 0, TK_FLAG_FINAL);
            token_list_insert_after(&ctx->ast.token_list, rp, &list1);
            token_list_destroy(&list1);
        }
        if (strcmp(p_static_assert_declaration->first_token->lexeme, "static_assert") == 0)
        {
            /*C23 has static_assert but C11 _Static_assert*/
            free(p_static_assert_declaration->first_token->lexeme);
            p_static_assert_declaration->first_token->lexeme = strdup("_Static_assert");
        }
    }
    else
    {
        free(p_static_assert_declaration->first_token->lexeme);
        p_static_assert_declaration->first_token->lexeme = strdup("static_assert");
    }
}

static void visit_declaration_specifiers(struct visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* p_type);


static void visit_direct_declarator(struct visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration* parameter = NULL;

        if (p_direct_declarator->function_declarator->parameter_type_list_opt)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }

        while (parameter)
        {
            if (parameter->implicit_token)
            {
                free(parameter->implicit_token->lexeme);
                parameter->implicit_token->lexeme = strdup("/*_Implicit*/");
            }

            if (parameter->attribute_specifier_sequence_opt)
            {
                visit_attribute_specifier_sequence(ctx, parameter->attribute_specifier_sequence_opt);
            }

            visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);
            visit_declarator(ctx, parameter->declarator);
            parameter = parameter->next;
        }

    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            visit_expression(ctx, p_direct_declarator->array_declarator->assignment_expression);
        }

        if (ctx->target < LANGUAGE_C99)
        {
            /*static and type qualifiers in parameter array declarators where added in C99*/
            if (p_direct_declarator->array_declarator->static_token_opt)
            {
                p_direct_declarator->array_declarator->static_token_opt->flags |= TK_FLAG_HIDE;

                if (p_direct_declarator->array_declarator->type_qualifier_list_opt)
                {
                    struct type_qualifier* p_type_qualifier =
                        p_direct_declarator->array_declarator->type_qualifier_list_opt->head;

                    while (p_type_qualifier)
                    {
                        p_type_qualifier->token->flags |= TK_FLAG_HIDE;
                        p_type_qualifier = p_type_qualifier->next;
                    }
                }
            }
        }
    }
}

static void visit_declarator(struct visit_ctx* ctx, struct declarator* p_declarator)
{
    bool need_transformation = false;

    if (p_declarator->pointer)
    {
        struct pointer* p = p_declarator->pointer;
        while (p)
        {
            if (p->type_qualifier_list_opt)
            {
                struct type_qualifier* current = p->type_qualifier_list_opt->head;
                while (current)
                {
                    visit_type_qualifier(ctx, current);
                    current = current->next;
                }
            }
            p = p->pointer;
        }
    }

    if (ctx->target < LANGUAGE_C2X)
    {
        if (p_declarator->declaration_specifiers)
        {
            if (p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
            {
                need_transformation = true;
            }
            if (p_declarator->declaration_specifiers->type_specifier_flags & TYPE_SPECIFIER_TYPEOF)
            {
                need_transformation = true;
            }
        }

        if (p_declarator->specifier_qualifier_list &&
            p_declarator->specifier_qualifier_list->type_specifier_flags & TYPE_SPECIFIER_TYPEOF)
        {
            need_transformation = true;
        }
    }


    //we may have a diference type from the current syntax 
    if (need_transformation)
    {

        struct osstream ss = {0};

        /*types like nullptr are converted to other types like void* */
        struct type new_type = type_convert_to(&p_declarator->type, ctx->target);

        type_remove_names(&new_type);
        if (p_declarator->name)
        {
            free((void * owner)new_type.name_opt);
            new_type.name_opt = strdup(p_declarator->name->lexeme);
        }

        print_type_declarator(&ss, &new_type);

        if (ss.c_str != NULL)
        {
            const int level = p_declarator->first_token ? p_declarator->first_token->level : 0;
            struct tokenizer_ctx tctx = {0};
            struct token_list l2 = tokenizer(&tctx, ss.c_str, NULL, level, TK_FLAG_FINAL);


            /*let's hide the old declarator*/
            if (p_declarator->first_token != NULL &&
                p_declarator->first_token != p_declarator->last_token)
            {
                l2.head->flags = p_declarator->first_token->flags;
                token_list_insert_after(&ctx->ast.token_list, p_declarator->last_token, &l2);
                token_range_add_flag(p_declarator->first_token, p_declarator->last_token, TK_FLAG_HIDE);
            }
            else
            {

                if (p_declarator->first_token == NULL)
                {
                    l2.head->flags = p_declarator->last_token->flags;
                    /*it is a empty declarator, so first_token is not part of declarator it only marks de position*/
                    token_list_insert_after(&ctx->ast.token_list, p_declarator->last_token->prev, &l2);
                }
                else
                {
                    l2.head->flags = p_declarator->first_token->flags;
                    /*it is a empty declarator, so first_token is not part of declarator it only marks de position*/
                    token_list_insert_after(&ctx->ast.token_list, p_declarator->last_token, &l2);
                    token_range_add_flag(p_declarator->first_token, p_declarator->last_token, TK_FLAG_HIDE);
                }

            }
            token_list_destroy(&l2);
        }

        type_destroy(&new_type);
        ss_close(&ss);
    }


    if (p_declarator->direct_declarator)
    {
        visit_direct_declarator(ctx, p_declarator->direct_declarator);
    }
}

static void visit_init_declarator_list(struct visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* p_init_declarator = p_init_declarator_list->head;



    while (p_init_declarator)
    {
        if (p_init_declarator->p_declarator)
        {
            visit_declarator(ctx, p_init_declarator->p_declarator);
        }

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                visit_expression(ctx, p_init_declarator->initializer->assignment_expression);
            }
            else
            {
                assert(p_init_declarator->initializer->braced_initializer != NULL);
                if (p_init_declarator->initializer->braced_initializer)
                {
                    visit_bracket_initializer_list(ctx,
                        p_init_declarator->initializer->braced_initializer);
                }

            }
        }

        p_init_declarator = p_init_declarator->next;
    }
}



static void visit_member_declarator(struct visit_ctx* ctx, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        visit_declarator(ctx, p_member_declarator->declarator);
    }
}

static void visit_member_declarator_list(struct visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}
static void visit_member_declaration(struct visit_ctx* ctx, struct member_declaration* p_member_declaration)
{
    if (p_member_declaration->member_declarator_list_opt)
    {
        visit_specifier_qualifier_list(ctx,
            p_member_declaration->specifier_qualifier_list,
            &p_member_declaration->member_declarator_list_opt->head->declarator->type); /*se nao tem?*/
    }

    if (p_member_declaration->member_declarator_list_opt)
    {
        visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void visit_member_declaration_list(struct visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* p_member_declaration =
        p_member_declaration_list->head;
    while (p_member_declaration)
    {
        visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void visit_attribute_specifier(struct visit_ctx* ctx, struct attribute_specifier* p_attribute_specifier)
{
    if (ctx->target < LANGUAGE_C2X)
    {
        token_range_add_flag(p_attribute_specifier->first_token, p_attribute_specifier->last_token, TK_FLAG_HIDE);
    }
}

static void visit_attribute_specifier_sequence(struct visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence)
{
    struct attribute_specifier* current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        visit_attribute_specifier(ctx, current);
        current = current->next;
    }
}

static void visit_struct_or_union_specifier(struct visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{

    if (p_struct_or_union_specifier->attribute_specifier_sequence_opt)
        visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence_opt);

    struct struct_or_union_specifier* p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);

    if (p_struct_or_union_specifier->show_anonymous_tag && !ctx->is_second_pass)
    {
        struct token* first = p_struct_or_union_specifier->first_token;

        const char* tag = p_struct_or_union_specifier->tag_name;
        char buffer[200] = {0};
        snprintf(buffer, sizeof buffer, " %s", tag);
        struct tokenizer_ctx tctx = {0};
        struct token_list l2 = tokenizer(&tctx, buffer, NULL, 0, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, first, &l2);
        token_list_destroy(&l2);
    }

    if (p_complete)
    {
        if (ctx->is_inside_lambda && !ctx->is_second_pass)
        {
            /*
              Na primeira passada marcamos os tipos que são renomeados
            */
            if (p_complete->scope_level >
                p_struct_or_union_specifier->scope_level &&
                p_complete->visit_moved == 0)
            {
                char newtag[212] = {0};
                snprintf(newtag, sizeof newtag, "_%s%d", p_struct_or_union_specifier->tag_name, ctx->capture_index);
                ctx->capture_index++;

                free(p_complete->tagtoken->lexeme);
                p_complete->tagtoken->lexeme = strdup(newtag);
                p_complete->visit_moved = 1;
            }
        }
        else if (ctx->is_second_pass)
        {
            /*
             Na segunda passada vou renomear quem usa este tag exporado
            */
            if (p_complete->visit_moved == 1)
            {
                if (p_struct_or_union_specifier != p_complete)
                {
                    free(p_struct_or_union_specifier->tagtoken->lexeme);
                    p_struct_or_union_specifier->tagtoken->lexeme = strdup(p_complete->tagtoken->lexeme);
                }
            }
        }
    }



    visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);

}

static void visit_enumerator(struct visit_ctx* ctx, struct enumerator* p_enumerator)
{
    if (p_enumerator->constant_expression_opt)
        visit_expression(ctx, p_enumerator->constant_expression_opt);

}

//struct enumerator_list* enumerator_list;
static void visit_enumerator_list(struct visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* current = p_enumerator_list->head;
    while (current)
    {
        visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void visit_enum_specifier(struct visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    if (ctx->target < LANGUAGE_C2X)
    {
        if (p_enum_specifier->specifier_qualifier_list)
        {
            struct token* tk = p_enum_specifier->specifier_qualifier_list->first_token;
            while (tk)
            {
                if (tk->type == ':')
                    break;
                tk = tk->prev;
            }
            token_range_add_flag(tk,
                p_enum_specifier->specifier_qualifier_list->last_token,
                TK_FLAG_HIDE);
        }


        if (p_enum_specifier->complete_enum_specifier != NULL &&
            p_enum_specifier != p_enum_specifier->complete_enum_specifier &&
            p_enum_specifier->complete_enum_specifier->specifier_qualifier_list)
        {
            p_enum_specifier->first_token->flags |= TK_FLAG_HIDE;

            if (p_enum_specifier->tag_token)
                p_enum_specifier->tag_token->flags |= TK_FLAG_HIDE;

            struct osstream ss = {0};
            bool b_first = true;

            print_type_qualifier_flags(&ss, &b_first, p_enum_specifier->complete_enum_specifier->specifier_qualifier_list->type_qualifier_flags);
            print_type_specifier_flags(&ss, &b_first, p_enum_specifier->complete_enum_specifier->specifier_qualifier_list->type_specifier_flags);

            struct tokenizer_ctx tctx = {0};
            struct token_list l2 = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_NONE);

            token_list_insert_after(&ctx->ast.token_list,
                p_enum_specifier->tag_token,
                &l2);

            ss_close(&ss);
            token_list_destroy(&l2);
        }

    }

    if (p_enum_specifier->attribute_specifier_sequence_opt)
    {
        visit_attribute_specifier_sequence(ctx, p_enum_specifier->attribute_specifier_sequence_opt);
    }

    visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void visit_typeof_specifier(struct visit_ctx* ctx, struct typeof_specifier* p_typeof_specifier)
{
}

static void visit_type_specifier(struct visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->typeof_specifier)
    {
        visit_typeof_specifier(ctx, p_type_specifier->typeof_specifier);
    }

    if (p_type_specifier->struct_or_union_specifier)
    {
        visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }


    if (p_type_specifier->atomic_type_specifier)
    {
        //visit_deped(ctx, p_type_specifier->enum_specifier);
    }

    if (p_type_specifier->flags & TYPE_SPECIFIER_BOOL)
    {
        if (ctx->target < LANGUAGE_C99)
        {
            free(p_type_specifier->token->lexeme);
            p_type_specifier->token->lexeme = strdup("unsigned char");
        }
        else
        {
            if (ctx->target < LANGUAGE_C2X)
            {
                if (strcmp(p_type_specifier->token->lexeme, "bool") == 0)
                {
                    free(p_type_specifier->token->lexeme);
                    p_type_specifier->token->lexeme = strdup("_Bool");
                }
            }
            else
            {
                free(p_type_specifier->token->lexeme);
                p_type_specifier->token->lexeme = strdup("bool");
            }
        }
    }
}

static void visit_type_specifier_qualifier(struct visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_qualifier)
    {
    }
    else if (p_type_specifier_qualifier->type_specifier)
    {
        visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
    else if (p_type_specifier_qualifier->alignment_specifier)
    {
    }
}

static void visit_storage_class_specifier(struct visit_ctx* ctx, struct storage_class_specifier* p_storage_class_specifier)
{
    if (p_storage_class_specifier->flags & STORAGE_SPECIFIER_AUTO)
    {
        if (ctx->target < LANGUAGE_C2X)
        {
            p_storage_class_specifier->token->flags |= TK_FLAG_HIDE;
        }
    }
}

static void visit_declaration_specifier(struct visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{

    if (p_declaration_specifier->function_specifier)
    {
        if (p_declaration_specifier->function_specifier->token->type == TK_KEYWORD__NORETURN)
        {
            if (ctx->target < LANGUAGE_C11)
            {
                free(p_declaration_specifier->function_specifier->token->lexeme);
                p_declaration_specifier->function_specifier->token->lexeme = strdup("/*[[noreturn]]*/");
            }
            else if (ctx->target == LANGUAGE_C11)
            {
                /*nothing*/
            }
            else if (ctx->target > LANGUAGE_C11)
            {
                /*use attributes*/
                free(p_declaration_specifier->function_specifier->token->lexeme);
                p_declaration_specifier->function_specifier->token->lexeme = strdup("[[noreturn]]");
            }

        }
    }


    if (p_declaration_specifier->storage_class_specifier)
    {
        visit_storage_class_specifier(ctx, p_declaration_specifier->storage_class_specifier);

    }

    if (p_declaration_specifier->type_specifier_qualifier)
    {
        visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);

    }

}

static void visit_declaration_specifiers(struct visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* p_type_opt)
{
    /*
        * Se tiver typeof ou auto vamos apagar todos type specifiers.
        * e trocar por um novo
        * const typeof(int (*)()) a;
           //a = 1;
          auto p = (const typeof(int (*)())) 0;

          TODO esconder os type spefiver e qualifider , esconder auto.
          o resto tipo static deixar.

        */
        //
    if (!ctx->is_second_pass &&
        ctx->target < LANGUAGE_C2X &&
        (p_declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO ||
            p_declaration_specifiers->type_specifier_flags & TYPE_SPECIFIER_TYPEOF))
    {

        struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;
        while (p_declaration_specifier)
        {
            if (p_declaration_specifier->function_specifier)
            {
            }
            if (p_declaration_specifier->storage_class_specifier)
            {
            }
            if (p_declaration_specifier->type_specifier_qualifier)
            {
                if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                {
                    p_declaration_specifier->type_specifier_qualifier->type_qualifier->token->flags |= TK_FLAG_HIDE;
                }
                if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                {
                    if (p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier)
                    {
                        token_range_add_flag(p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier->first_token,
                            p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier->last_token,
                            TK_FLAG_HIDE);
                    }
                    p_declaration_specifier->type_specifier_qualifier->type_specifier->token->flags |= TK_FLAG_HIDE;
                }
            }
            p_declaration_specifier = p_declaration_specifier->next;
        }


        /*now we print new specifiers then convert to tokens*/
        struct osstream ss0 = {0};
        struct type new_type = {0};

        if (p_type_opt)
            new_type = type_convert_to(p_type_opt, ctx->target);

        const struct type* p = type_get_specifer_part(&new_type);
        print_type_qualifier_specifiers(&ss0, p);

        const int level = p_declaration_specifiers->last_token->level;
        struct tokenizer_ctx tctx = {0};
        struct token_list l2 = tokenizer(&tctx, ss0.c_str, NULL, level, TK_FLAG_FINAL);

        token_list_insert_after(&ctx->ast.token_list, p_declaration_specifiers->last_token, &l2);


        type_destroy(&new_type);
        ss_close(&ss0);
        token_list_destroy(&l2);
    }

    struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;

    struct declaration_specifier* p_constexpr_declaration_specifier = NULL;
    while (p_declaration_specifier)
    {
        if (p_declaration_specifier->storage_class_specifier &&
            p_declaration_specifier->storage_class_specifier->flags & STORAGE_SPECIFIER_CONSTEXPR)
        {
            p_constexpr_declaration_specifier = p_declaration_specifier;
        }

        visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }


    if (ctx->target < LANGUAGE_C2X)
    {
        /*
          fixing constexpr, we add static const if necessary
        */
        if (p_constexpr_declaration_specifier &&
            p_declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR)
        {
            struct osstream ss = {0};
            const bool is_file_scope =
                p_declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR_STATIC;

            const bool has_static =
                p_declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC;

            const bool has_const =
                p_declaration_specifiers->type_qualifier_flags & TYPE_QUALIFIER_CONST;


            if (is_file_scope && !has_static)
            {
                ss_fprintf(&ss, "static");
                if (!has_const)
                {
                    ss_fprintf(&ss, " const");
                }
            }
            else
            {
                if (!has_const)
                {
                    ss_fprintf(&ss, "const");
                }
                else
                {
                    ss_fprintf(&ss, " ");
                }
            }

            free(p_constexpr_declaration_specifier->storage_class_specifier->token->lexeme);
            p_constexpr_declaration_specifier->storage_class_specifier->token->lexeme = ss.c_str;
            ss.c_str = NULL; /*MOVED*/
            ss_close(&ss);
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
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        return true;
    }
    return false;
}

void defer_scope_delete_one(struct defer_scope* owner p_block)
{
    if (p_block != NULL)
    {
        struct defer_scope* owner child = p_block->lastchild;
        while (child != NULL)
        {
            struct defer_scope* owner prev = child->previous;

            child->previous = NULL;
            defer_scope_delete_one(child);

            child = prev;
        }

        assert(p_block->previous == NULL);
        free(p_block);
    }
}

void defer_scope_delete_all(struct defer_scope* owner p)
{
    struct defer_scope* owner p_block = p;
    while (p_block != NULL)
    {
        struct defer_scope* owner prev_block = p_block->previous;
        p_block->previous = NULL;
        defer_scope_delete_one(p_block);
        p_block = prev_block;
    }
}

static void visit_declaration(struct visit_ctx* ctx, struct declaration* p_declaration)
{



    if (p_declaration->static_assert_declaration)
    {
        visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
        visit_attribute_specifier_sequence(ctx, p_declaration->p_attribute_specifier_sequence_opt);
    }


    if (p_declaration->declaration_specifiers)
    {

        if (p_declaration->init_declarator_list.head)
        {
            visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers,
                &p_declaration->init_declarator_list.head->p_declarator->type);
        }
        else
        {
            visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, NULL);
        }

    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
        if (!ctx->is_second_pass)
        {
            token_range_add_flag(p_declaration->p_attribute_specifier_sequence_opt->first_token,
                p_declaration->p_attribute_specifier_sequence_opt->last_token,
                TK_FLAG_HIDE);

        }
    }
    if (ctx->is_second_pass)
    {

        if (p_declaration->declaration_specifiers &&
            p_declaration->declaration_specifiers->type_specifier_flags == TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            if (p_declaration->declaration_specifiers->struct_or_union_specifier->visit_moved == 1)
            {
                struct tokenizer_ctx tctx = {0};
                struct token_list list0 = tokenizer(&tctx, "struct ", NULL, 0, TK_FLAG_FINAL);
                token_list_append_list(&ctx->insert_before_declaration, &list0);
                token_list_destroy(&list0);


                struct token_list list = tokenizer(&tctx, p_declaration->declaration_specifiers->struct_or_union_specifier->tagtoken->lexeme, NULL, 0, TK_FLAG_FINAL);
                token_list_append_list(&ctx->insert_before_declaration, &list);
                token_list_destroy(&list);

                //struct token_list list3 = tokenizer("{", NULL, 0, TK_FLAG_FINAL);
                //token_list_append_list(&ctx->insert_before_declaration, &list3);



                struct token* first = p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.first_token;
                struct token* last = p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.last_token;
                for (struct token* current = first;
                    current != last->next;
                    current = current->next)
                {
                    token_list_clone_and_add(&ctx->insert_before_declaration, current);
                    //current->flags |= TK_FLAG_FINAL;
                }

                struct token_list list3 = tokenizer(&tctx, ";\n", NULL, 0, TK_FLAG_FINAL);
                token_list_append_list(&ctx->insert_before_declaration, &list3);


                if (p_declaration->init_declarator_list.head == NULL)
                {
                    token_range_add_flag(p_declaration->declaration_specifiers->struct_or_union_specifier->first_token,
                        p_declaration->declaration_specifiers->struct_or_union_specifier->last_token,
                        TK_FLAG_HIDE);
                }
                else
                {
                    token_range_add_flag(p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.first_token,
                        p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.last_token,
                        TK_FLAG_HIDE);
                }
                token_list_destroy(&list3);
            }
        }
    }


    if (p_declaration->init_declarator_list.head)
    {
        visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        ctx->has_lambda = false;
        ctx->is_second_pass = false;


        struct defer_scope* p_defer = visit_ctx_push_tail_block(ctx);
        p_defer->p_function_body = p_declaration->function_body;

        visit_compound_statement(ctx, p_declaration->function_body);

        if (!is_last_item_return(p_declaration->function_body))
        {
            struct osstream ss = {0};
            print_block_defer(p_defer, &ss, true);
            if (ss.size > 0)
            {
                struct tokenizer_ctx tctx = {0};
                struct token_list l = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
                token_list_insert_after(&ctx->ast.token_list, p_declaration->function_body->last_token->prev, &l);
                token_list_destroy(&l);
            }
            ss_close(&ss);
        }
        else
        {
            //ja tem um return antes que chama defer
            hide_block_defer(p_defer);
        }

        visit_ctx_pop_tail_block(ctx);



        if (ctx->has_lambda)
        {
            /*functions with lambdas requires two phases*/
            ctx->is_second_pass = true;
            visit_compound_statement(ctx, p_declaration->function_body);
        }
    }


    /*
       In direct mode, we hide non used declarations (just to make the result smaller)
    */
    if (ctx->hide_non_used_declarations &&
        p_declaration->init_declarator_list.head)
    {
        if (p_declaration->init_declarator_list.head->p_declarator &&
            p_declaration->init_declarator_list.head->p_declarator->num_uses == 0 &&
            p_declaration->init_declarator_list.head->p_declarator->function_body == NULL)
        {
            /*
              transformations must keep first_token and last_token correct - updated
            */
            token_range_add_flag(p_declaration->first_token, p_declaration->last_token, TK_FLAG_HIDE);
        }
    }
}

int visit_literal_string(struct visit_ctx* ctx, struct token* current)
{
    bool has_u8_prefix =
        current->lexeme[0] == 'u' && current->lexeme[1] == '8';

    if (has_u8_prefix && ctx->target < LANGUAGE_C11)
    {
        struct osstream ss = {0};
        unsigned char* psz = (unsigned char*) (current->lexeme + 2);

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

    return 0;
}

int visit_tokens(struct visit_ctx* ctx)
{
    struct token* current = ctx->ast.token_list.head;
    while (current)
    {

        if (current->type == TK_STRING_LITERAL)
        {
            visit_literal_string(ctx, current);
        }

        if (ctx->target < LANGUAGE_C2X)
        {
            if (current->type == TK_LINE_COMMENT)
            {
                if (ctx->target < LANGUAGE_C99)
                {
                    struct osstream ss = {0};
                    //TODO  check /* inside
                    ss_fprintf(&ss, "/*%s*/", current->lexeme + 2);
                    free(current->lexeme);
                    current->lexeme = ss.c_str;
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
                    current->next->lexeme = strdup("elif ! defined ");
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
                char buffer[33 + 2] = {'0', 'x'};
                snprintf(buffer, sizeof buffer, "0x%x", value);
                free(current->lexeme);
                current->lexeme = strdup(buffer);
            }
            else if (current->type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT)
            {
                remove_char(current->lexeme, '\'');

                if (ctx->target < LANGUAGE_C99)
                {
                    /*
                     * C99 Hexadecimal floating constants to C89.
                     */
                    long double d = strtold(current->lexeme, 0);
                    char buffer[50] = {0};
                    snprintf(buffer, sizeof buffer, "%Lg", d);

                    free(current->lexeme);
                    current->lexeme = strdup(buffer);
                }
            }
            else if (current->type == TK_COMPILER_DECIMAL_CONSTANT ||
                current->type == TK_COMPILER_OCTAL_CONSTANT ||
                current->type == TK_COMPILER_HEXADECIMAL_CONSTANT ||
                current->type == TK_COMPILER_DECIMAL_FLOATING_CONSTANT ||
                current->type == TK_PPNUMBER)
            {
                /*remove digit separators*/
                remove_char(current->lexeme, '\'');
            }
        }

        current = current->next;
    }
    return 0;
}

void visit(struct visit_ctx* ctx)
{
    ctx->capture_index = 0;
    ctx->lambdas_index = 0;

    if (visit_tokens(ctx) != 0)
        return;

    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        visit_declaration(ctx, p_declaration);

        if (ctx->insert_before_block_item.head != NULL)
        {
            token_list_insert_after(&ctx->ast.token_list, p_declaration->first_token->prev, &ctx->insert_before_block_item);
        }

        /*
        * Tem que inserir algo antes desta declaracao?
        */
        if (ctx->insert_before_declaration.head != NULL)
        {
            token_list_insert_after(&ctx->ast.token_list, p_declaration->first_token->prev, &ctx->insert_before_declaration);

        }

        p_declaration = p_declaration->next;
    }
    //if (ctx->instanciations.head != NULL)
    //{
    //    token_list_append_list(&ctx->ast.token_list, &ctx->instanciations);
    //}
}

