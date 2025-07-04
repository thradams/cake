
#pragma safety enable


#include <stdio.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>

#include "ownership.h"
#include "console.h"
#include "flow.h"
#include "expressions.h"

struct label_set
{
    char name[50];
    struct flow_set flow_set;
    struct label_set* next;
};

bool object_to_bool_ex(const struct object* a)
{
    a = object_get_referenced(a);
    switch (a->state)
    {
    case CONSTANT_VALUE_STATE_UNINITIALIZED: break;
    case CONSTANT_VALUE_STATE_ANY: break;
    case CONSTANT_VALUE_STATE_CONSTANT:
        return object_to_bool(a);
        //flow analysis
    case CONSTANT_VALUE_NOT_EQUAL:
        return object_to_bool(a);


    case CONSTANT_VALUE_EQUAL:
        return object_to_bool(a);
    }
    return false;
}

void label_set_list_push(struct label_set_list* books, struct label_set* new_book)
{
    assert(books != NULL);
    assert(new_book != NULL);
    assert(new_book->next == NULL);

    if (books->tail == NULL)
    {
        books->head = new_book;
    }
    else
    {
        books->tail->next = new_book;
    }
    books->tail = new_book;
}

void label_set_list_clear(struct label_set_list* list)
{
    //pre condition
    assert(list != NULL);

    struct label_set* it = list->head;
    while (it != NULL)
    {
        struct label_set* next = it->next;
        free(it);
        it = next;
    }
    list->head = NULL;
    list->tail = NULL;
}


struct label_set* label_set_get(struct label_set_list* list, const char* name)
{
    struct label_set* p = list->head;
    while (p)
    {
        if (strcmp(p->name, name) == 0)
            break;
        p = p->next;
    }
    if (p == NULL)
    {
        p = calloc(1, sizeof * p);
        if (p)
        {
            snprintf(p->name, sizeof p->name, "%s", name);
            label_set_list_push(list, p);
        }
    }
    return p;
}


struct true_false_set
{
    struct flow_set true_paths;
    struct flow_set false_paths;
};

static void true_false_set_destroy(struct true_false_set* a)
{
    flow_set_destroy(&a->true_paths);
    flow_set_destroy(&a->false_paths);
}

void c_clrscr();

static void flow_print_token(struct token* p_item, struct token* p_hight_light_token)
{
    if (p_item == p_hight_light_token)
    {
        printf(BK_YELLOW);
        if (p_item->type == TK_NEWLINE)
            printf("\xe2\x86\xb5"); //enter
    }

    if (p_item->flags & TK_FLAG_MACRO_EXPANDED)
    {
        printf(DARKGRAY);
    }
    else if (p_item->type >= TK_KEYWORD_AUTO &&
             p_item->type <= TK_KEYWORD_IS_INTEGRAL)
    {
        printf(BLUE);
    }
    else if (p_item->type == TK_COMMENT ||
             p_item->type == TK_LINE_COMMENT)
    {
        printf(YELLOW);
    }
    printf("%s", p_item->lexeme);



    printf(RESET);
}

static void debug_print_core(struct flow_visit_ctx* ctx,
    const struct flow_set* p_flow_set_true,
    const struct flow_set* p_flow_set_false,
    struct token* p_hight_light_token)
{
    if (!ctx->ctx->options.do_static_debug)
        return;

    if (p_hight_light_token->line > ctx->ctx->options.static_debug_lines)
    {
        ctx->debug_static_started = true;
    }

    if (!ctx->debug_static_started)
        return;

    struct declaration* p_declaration = ctx->p_declaration;
    c_clrscr();

    struct token* _Opt p = p_declaration->first_token;
    if (p)
    {
        if (p_hight_light_token->line == p->line)
            printf(WHITE);
        else
            printf(DARKGRAY);

        printf("%3d ", p->line);
        printf(RESET);
    }

    while (p)
    {
        flow_print_token(p, p_hight_light_token);

        if (p == p_declaration->last_token)
            break;

        if (p->type == TK_NEWLINE)
        {
            if (p_hight_light_token->line == p->line + 1)
                printf(WHITE);
            else
                printf(DARKGRAY);

            printf("%3d ", p->line + 1);
            printf(RESET);
        }
        p = p->next;
    }
    if (p_declaration->function_body)
    {
        p = p_declaration->function_body->first_token->next;
        while (p)
        {
            flow_print_token(p, p_hight_light_token);
            if (p == p_declaration->function_body->last_token)
                break;
            if (p->type == TK_NEWLINE)
            {
                if (p_hight_light_token->line == p->line + 1)
                    printf(WHITE);
                else
                    printf(DARKGRAY);

                printf("%3d ", p->line + 1);
                printf(RESET);
            }
            p = p->next;
        }
    }

    flow_set_print(p_flow_set_true, 1, 50);
    if (p_flow_set_false)
        flow_set_print(p_flow_set_false, 1, 80);

    c_getch();
}

static void debug_print(struct flow_visit_ctx* ctx,
    const struct flow_set* p_flow_set,
    struct token* p_hight_light_token)
{
    debug_print_core(ctx,
    p_flow_set,
    NULL,
    p_hight_light_token);
}

void flow_visit_declaration(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct declaration* p_declaration);
static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct secondary_block* p_secondary_block);
static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void flow_visit_expression(struct flow_visit_ctx* ctx, const struct flow_set* p_flow_set, struct expression* p_expression, struct true_false_set* a);
static void flow_visit_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct statement* p_statement);
static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct enum_specifier* p_enum_specifier);
static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct type_specifier* p_type_specifier);
static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct braced_initializer* p_bracket_initializer_list);
static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct expression_statement* p_expression_statement);
static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct secondary_block* p_secondary_block)
{
    flow_visit_statement(ctx, p_flow_set, p_secondary_block->statement);
}
static void flow_visit_defer_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct defer_statement* p_defer_statement)
{
    /*
      We are not going to visit the secondary block here because
      this is not the place were defer is executed.
    */
}
NODISCARD
static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct initializer* p_initializer);
static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct declarator* p_declarator);
static void flow_visit_init_declarator(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct init_declarator* p_init_declarator)
{
    try
    {
        flow_visit_declarator(ctx, p_flow_set, p_init_declarator->p_declarator);

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                struct true_false_set a = { 0 };
                flow_visit_expression(ctx, p_flow_set, p_init_declarator->initializer->assignment_expression, &a);
                true_false_set_destroy(&a);
            }
            else
            {
                if (p_init_declarator->initializer->braced_initializer)
                {
                    flow_visit_bracket_initializer_list(ctx, p_flow_set,
                        p_init_declarator->initializer->braced_initializer);
                }
                else
                {
                    assert(false);
                }
            }
        }

        if (p_init_declarator->p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
        {
            if (p_init_declarator->initializer &&
                p_init_declarator->initializer->assignment_expression)
            {
            }
            else  if (p_init_declarator->initializer &&
                p_init_declarator->initializer->braced_initializer)
            {
            }
            else
            {

            }
        }
    }
    catch
    {
        //some unexpected error
    }
}
static void flow_visit_init_declarator_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct init_declarator_list* p_init_declarator_list);
static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* _Opt p_type);
static void flow_visit_simple_declaration(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct simple_declaration* p_simple_declaration)
{
    flow_visit_declaration_specifiers(ctx, p_flow_set, p_simple_declaration->p_declaration_specifiers, NULL);
    flow_visit_init_declarator_list(ctx, p_flow_set, &p_simple_declaration->init_declarator_list);
}
static void flow_visit_if_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct selection_statement* p_selection_statement)
{

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_expression_statement)
        flow_visit_expression_statement(ctx, p_flow_set, p_selection_statement->p_init_statement->p_expression_statement);

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_simple_declaration)
        flow_visit_simple_declaration(ctx, p_flow_set, p_selection_statement->p_init_statement->p_simple_declaration);

    /*
      if(int *p = f());
      The hidden expression is p
    */
    _Opt _View struct expression hidden_expression = { 0 };

    struct true_false_set true_false_set = { 0 };

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression)
    {
        flow_visit_expression(ctx, p_flow_set, p_selection_statement->condition->expression, &true_false_set);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_flow_set, p_selection_statement->condition->p_init_declarator);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression == NULL &&
        p_selection_statement->condition->p_init_declarator != NULL)
    {
        hidden_expression.expression_type = PRIMARY_EXPRESSION_DECLARATOR;
        hidden_expression.declarator = p_selection_statement->condition->p_init_declarator->p_declarator;
        assert(p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt != NULL);
        hidden_expression.first_token = p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt;
        hidden_expression.last_token = hidden_expression.first_token;
        flow_visit_expression(ctx, p_flow_set, &hidden_expression, &true_false_set);
    }

    assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);


    flow_set_swap(p_flow_set, &true_false_set.true_paths);

    debug_print(ctx, p_flow_set, p_selection_statement->secondary_block->first_token);

    flow_visit_secondary_block(ctx, p_flow_set, p_selection_statement->secondary_block);
    debug_print(ctx, p_flow_set, p_selection_statement->secondary_block->last_token);

    const bool true_branch_ends_with_jump =
        secondary_block_ends_with_jump(p_selection_statement->secondary_block);

    struct flow_set end_of_if_true = { 0 };
    flow_set_swap(&end_of_if_true, p_flow_set);

    bool false_branch_ends_with_jump = false;

    struct flow_set end_of_if_false = { 0 };



    if (p_selection_statement->else_secondary_block_opt)
    {
        flow_set_swap(p_flow_set, &true_false_set.false_paths);
        false_branch_ends_with_jump = secondary_block_ends_with_jump(p_selection_statement->else_secondary_block_opt);
        if (p_flow_set->size > 0)
        {
            debug_print(ctx, p_flow_set, p_selection_statement->else_secondary_block_opt->first_token);
            flow_visit_secondary_block(ctx, p_flow_set, p_selection_statement->else_secondary_block_opt);
            debug_print(ctx, p_flow_set, p_selection_statement->else_secondary_block_opt->last_token);
            flow_set_swap(&end_of_if_false, p_flow_set);
        }
    }
    else
    {
        flow_set_swap(&end_of_if_false, &true_false_set.false_paths);
    }


    flow_set_destroy(p_flow_set);

    if (!false_branch_ends_with_jump && !true_branch_ends_with_jump)
    {
        *p_flow_set = flow_set_merge(&end_of_if_true, &end_of_if_false);
    }
    else if (true_branch_ends_with_jump)
    {
        *p_flow_set = flow_set_dup(&end_of_if_false);
    }
    else
    {
        *p_flow_set = flow_set_dup(&end_of_if_true);
    }


    flow_set_destroy(&end_of_if_false);

    if (p_selection_statement->else_secondary_block_opt)
        debug_print(ctx, p_flow_set, p_selection_statement->else_secondary_block_opt->last_token->next);
    else
        debug_print(ctx, p_flow_set, p_selection_statement->secondary_block->last_token->next);

    true_false_set_destroy(&true_false_set);
}

static void flow_visit_block_item(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct block_item* p_block_item);
static void flow_visit_try_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct try_statement* p_try_statement)
{
    try
    {

        struct flow_set end_of_try = { 0 };
        struct flow_set end_of_catch = { 0 };

        struct flow_set old = { 0 };
        flow_set_swap(&old, &ctx->catch_set);


        flow_visit_secondary_block(ctx, p_flow_set, p_try_statement->secondary_block);
        flow_set_swap(p_flow_set, &end_of_try);

        if (p_try_statement->catch_secondary_block_opt)
        {
            //current all possible states of throw
            flow_set_swap(p_flow_set, &ctx->catch_set);

            debug_print(ctx, p_flow_set, p_try_statement->catch_secondary_block_opt->first_token);
            flow_visit_secondary_block(ctx, p_flow_set, p_try_statement->catch_secondary_block_opt);
            //current has the state at the end of catch block
            flow_set_swap(&end_of_catch, p_flow_set);

        }
        flow_set_destroy(p_flow_set);
        *p_flow_set = flow_set_merge(&end_of_catch, &end_of_try);

        if (p_try_statement->catch_secondary_block_opt)
            debug_print(ctx, p_flow_set, p_try_statement->catch_secondary_block_opt->last_token);

        //  flow_set_swap(&old, &ctx->catch_set);

        if (p_try_statement->catch_secondary_block_opt)
            debug_print(ctx, p_flow_set, p_try_statement->catch_secondary_block_opt->last_token->next);
        else
            debug_print(ctx, p_flow_set, p_try_statement->secondary_block->last_token->next);

    }
    catch
    {
    }
}
static void flow_visit_switch_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct selection_statement* p_selection_statement)
{
    struct flow_set old = { 0 };
    flow_set_swap(&old, &ctx->switch_set);
    ctx->switch_set = flow_set_dup(p_flow_set);

    //flow_visit_expression(ctx, p_selection_statement->condition,
    //flow_set_clear(p_flow_set);

    flow_visit_secondary_block(ctx, p_flow_set, p_selection_statement->secondary_block);

    flow_set_swap(p_flow_set, &ctx->break_set);

    flow_set_swap(&old, &ctx->switch_set);
}

static void flow_visit_selection_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct selection_statement* p_selection_statement)
{
    struct selection_statement* old = ctx->p_selection_statement;
    ctx->p_selection_statement = p_selection_statement;

    if (p_selection_statement->first_token->type == TK_KEYWORD_IF)
    {
        flow_visit_if_statement(ctx, p_flow_set, p_selection_statement);
    }
    else if (p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
    {
        flow_visit_switch_statement(ctx, p_flow_set, p_selection_statement);
    }
    else
        assert(false);

    ctx->p_selection_statement = old; //restore
}

static void flow_visit_compound_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct compound_statement* p_compound_statement);
static void flow_visit_initializer_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct initializer_list* p_initializer_list);
static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {

    }
    else
    {
        flow_visit_initializer_list(ctx, p_flow_set, p_bracket_initializer_list->initializer_list);
    }
}
static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct braced_initializer* p_bracket_initializer_list);
static void flow_visit_initializer_list_item(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct initializer_list_item* p_initializer)
{
    assert(p_initializer->initializer != NULL);
    flow_visit_initializer(ctx, p_flow_set, p_initializer->initializer);
}
static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct initializer* p_initializer)
{
    if (p_initializer->assignment_expression)
    {
        struct true_false_set a = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_initializer->assignment_expression, &a);
        true_false_set_destroy(&a);
    }
    else if (p_initializer->braced_initializer)
    {
        flow_visit_bracket_initializer_list(ctx, p_flow_set, p_initializer->braced_initializer);
    }
}
static void flow_visit_initializer_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct initializer_list* p_initializer_list)
{
    struct initializer_list_item* _Opt p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow_visit_initializer_list_item(ctx, p_flow_set, p_initializer);
        p_initializer = p_initializer->next;
    }
}
static void flow_visit_generic_selection(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        struct true_false_set a = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_generic_selection->expression, &a);
        true_false_set_destroy(&a);
    }
}

static void flow_visit_expression(struct flow_visit_ctx* ctx,
    const struct flow_set* p_flow_set,
    struct expression* p_expression,
    struct true_false_set* expr_true_false_set)
{

    switch (p_expression->expression_type)
    {
    case EXPRESSION_TYPE_INVALID:
        assert(false);
        break;

    case PRIMARY_EXPRESSION__FUNC__:
        expr_true_false_set->true_paths = flow_set_dup(p_flow_set);
        break;

    case PRIMARY_EXPRESSION_STRING_LITERAL:
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
    case PRIMARY_EXPRESSION_NUMBER:
    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
    case PRIMARY_EXPRESSION_ENUMERATOR:

        if (object_to_bool(&p_expression->object))
        {
            flow_set_swap(&expr_true_false_set->true_paths, p_flow_set);
            //flow_set_set(&expr_true_false_set->true_paths, &p_expression->object, &p_expression->object);
        }
        else
        {
            flow_set_swap(&expr_true_false_set->false_paths, p_flow_set);
            //flow_set_set(&expr_true_false_set->false_paths, &p_expression->object, &p_expression->object);
        }

        debug_print_core(ctx,
    &expr_true_false_set->true_paths,
    &expr_true_false_set->false_paths,
    p_expression->last_token->next);

        break;

    case PRIMARY_EXPRESSION_DECLARATOR:
    {
        for (int i = 0; i < p_flow_set->size; i++)
        {
            struct flow_path* path = p_flow_set->data[i];
            struct object* o = flow_path_find(path, &p_expression->object);
            if (o == NULL) continue;
            switch (o->state)
            {
            case CONSTANT_VALUE_STATE_UNINITIALIZED:
            {
                struct flow_path* clone1 = calloc(1, sizeof * clone1);
                *clone1 = flow_path_dup(path);

                //flow_path_set(clone1, &p_expression->object, );
                flow_set_push_back(&expr_true_false_set->true_paths, clone1);

                struct flow_path* clone2 = calloc(1, sizeof * clone2);
                *clone2 = flow_path_dup(path);
                flow_set_push_back(&expr_true_false_set->false_paths, clone2);
            }
            break;
            case CONSTANT_VALUE_STATE_ANY:
            {
                struct flow_path* clone1 = calloc(1, sizeof * clone1);
                *clone1 = flow_path_dup(path);

                //flow_path_set(clone1, &p_expression->object, );
                flow_set_push_back(&expr_true_false_set->true_paths, clone1);

                struct flow_path* clone2 = calloc(1, sizeof * clone2);
                *clone2 = flow_path_dup(path);
                flow_set_push_back(&expr_true_false_set->false_paths, clone2);

            }   break;
            case CONSTANT_VALUE_STATE_CONSTANT:
                if (object_to_bool(o))
                {
                    struct flow_path* clone = calloc(1, sizeof * clone);
                    *clone = flow_path_dup(path);
                    flow_set_push_back(&expr_true_false_set->true_paths, clone);
                }
                else
                {
                    struct flow_path* clone = calloc(1, sizeof * clone);
                    *clone = flow_path_dup(path);
                    flow_set_push_back(&expr_true_false_set->false_paths, clone);
                }
                break;
                //flow analysis
            case CONSTANT_VALUE_NOT_EQUAL:
                break;
            case CONSTANT_VALUE_EQUAL:
                if (object_to_bool(o))
                {
                    struct flow_path* clone = calloc(1, sizeof * clone);
                    *clone = flow_path_dup(path);
                    flow_set_push_back(&expr_true_false_set->true_paths, clone);
                }
                else
                {
                    struct flow_path* clone = calloc(1, sizeof * clone);
                    *clone = flow_path_dup(path);
                    flow_set_push_back(&expr_true_false_set->false_paths, clone);
                }
                break;
            }


        }


        debug_print_core(ctx,
            &expr_true_false_set->true_paths,
            &expr_true_false_set->false_paths,
            p_expression->first_token);

#if 0
        if (ctx->p_assigment_object != key)
        {

            struct var_iterator it = flow_set_var_iterator(p_flow_set, key);
            while (var_iterator_next(&it))
            {
                if (it.p_value->state == CONSTANT_VALUE_STATE_UNINITIALIZED)
                {
                    compiler_diagnostic(W_FLOW_UNINITIALIZED, ctx->ctx,
                         p_expression->first_token,
                         NULL,
                         "variable can be uninitialized");
                    break;
                }
            }
        }


#endif

    }
    break;

    case PRIMARY_EXPRESSION_PARENTESIS:
        assert(p_expression->right != NULL);
        flow_visit_expression(ctx, p_flow_set, p_expression->right, expr_true_false_set);
        break;



    case PRIMARY_EXPRESSION_GENERIC:
        assert(p_expression->generic_selection != NULL);
        flow_visit_generic_selection(ctx, p_flow_set, p_expression->generic_selection);
        break;

    case POSTFIX_DOT:
    {
        assert(p_expression->left != NULL);
        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->left, &left_set);
        true_false_set_destroy(&left_set);
    }
    break;

    case POSTFIX_ARROW:
    {
        assert(p_expression->left != NULL);
        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->left, &left_set);
        struct flow_set m = flow_set_merge(&left_set.true_paths, &left_set.false_paths);

        for (int i = 0; i < m.size; i++)
        {
            struct flow_path* path = m.data[i];
            struct object* o = flow_path_find(path, &p_expression->object);
            if (o == NULL) continue;
            switch (o->state)
            {
            case CONSTANT_VALUE_STATE_UNINITIALIZED:
            {
                struct flow_path* clone1 = calloc(1, sizeof * clone1);
                *clone1 = flow_path_dup(path);

                //flow_path_set(clone1, &p_expression->object, );
                flow_set_push_back(&expr_true_false_set->true_paths, clone1);

                struct flow_path* clone2 = calloc(1, sizeof * clone2);
                *clone2 = flow_path_dup(path);
                flow_set_push_back(&expr_true_false_set->false_paths, clone2);
            }
            break;
            case CONSTANT_VALUE_STATE_ANY:
            {
                struct flow_path* clone1 = calloc(1, sizeof * clone1);
                *clone1 = flow_path_dup(path);

                //flow_path_set(clone1, &p_expression->object, );
                flow_set_push_back(&expr_true_false_set->true_paths, clone1);

                struct flow_path* clone2 = calloc(1, sizeof * clone2);
                *clone2 = flow_path_dup(path);
                flow_set_push_back(&expr_true_false_set->false_paths, clone2);

            }   break;
            case CONSTANT_VALUE_STATE_CONSTANT:
                if (object_to_bool(o))
                {
                    struct flow_path* clone = calloc(1, sizeof * clone);
                    *clone = flow_path_dup(path);
                    flow_set_push_back(&expr_true_false_set->true_paths, clone);
                }
                else
                {
                    struct flow_path* clone = calloc(1, sizeof * clone);
                    *clone = flow_path_dup(path);
                    flow_set_push_back(&expr_true_false_set->false_paths, clone);
                }
                break;
                //flow analysis
            case CONSTANT_VALUE_NOT_EQUAL:
                break;
            case CONSTANT_VALUE_EQUAL:
                if (object_to_bool(o))
                {
                    struct flow_path* clone = calloc(1, sizeof * clone);
                    *clone = flow_path_dup(path);
                    flow_set_push_back(&expr_true_false_set->true_paths, clone);
                }
                else
                {
                    struct flow_path* clone = calloc(1, sizeof * clone);
                    *clone = flow_path_dup(path);
                    flow_set_push_back(&expr_true_false_set->false_paths, clone);
                }
                break;
            }


        }
        true_false_set_destroy(&left_set);
    }
    break;

    case POSTFIX_INCREMENT:
    case POSTFIX_DECREMENT:
    {
        assert(p_expression->left != NULL);
        flow_visit_expression(ctx, p_flow_set, p_expression->left, expr_true_false_set);
    }
    break;

    case POSTFIX_ARRAY:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow_visit_expression(ctx, p_flow_set, p_expression->left, expr_true_false_set);
        flow_visit_expression(ctx, p_flow_set, p_expression->right, expr_true_false_set);
    }
    break;

    case POSTFIX_FUNCTION_CALL:
    {
        if (type_is_pointer(&p_expression->type))
        {
            if (type_is_opt(&p_expression->type, true))
            {
                struct flow_set dup = flow_set_dup(p_flow_set);

                struct object null_object = object_make_null_pointer();
                flow_set_set(p_flow_set, &p_expression->object, &null_object);
                flow_set_swap(&expr_true_false_set->false_paths, p_flow_set);

                struct object* heap1 = calloc(1, sizeof * heap1);
                objects_push(&ctx->heap_objects, heap1);

                struct type t = type_remove_pointer(&p_expression->type);
                struct object heap = { 0 };
                make_object(&t, &heap);
                heap1->debug_name = strdup("heap");
                *heap1 = heap;

                struct object pointer_to = { 0 };
                //if (pnew == NULL) throw;
                pointer_to.value_type = TYPE_VOID_PTR;
                pointer_to.value.void_pointer = heap1;
                pointer_to.state = CONSTANT_VALUE_EQUAL;
                flow_set_set(&dup, &p_expression->object, &pointer_to);
                // flow_set_set(&dup, &p_expression->object, heap1);

                flow_set_swap(&expr_true_false_set->true_paths, &dup);
                debug_print(ctx, &expr_true_false_set->false_paths, p_expression->last_token);
                debug_print(ctx, &expr_true_false_set->true_paths, p_expression->last_token);
            }
            else
            {
                //struct object* heap1 = calloc(1, sizeof * heap1);
                //objects_push(&ctx->heap_objects, heap1);

                //struct type t = type_remove_pointer(&p_expression->type);
                //struct object heap = { 0 };
                //make_object(&t, &heap);
                //heap1->debug_name = strdup("heap");
                //*heap1 = heap;

                //struct object pointer_to = { 0 };
                //if (pnew == NULL) throw;
                //pointer_to.value_type = TYPE_VOID_PTR;
                //pointer_to.value.void_pointer = heap1;
                //pointer_to.state = CONSTANT_VALUE_EQUAL;

                //flow_set_swap(&expr_true_false_set->true_paths, &dup);                
            }
        }
        else
        {
        }


        struct type t = get_function_return_type(&p_expression->type);
        if (type_is_scalar(&t) && type_is_opt(&t, ctx->ctx->options.null_checks_enabled))
        {
            expr_true_false_set->true_paths = flow_set_dup(p_flow_set);
            expr_true_false_set->false_paths = flow_set_dup(p_flow_set);
        }
        type_destroy(&t);

        struct true_false_set left_local = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->left, &left_local);
        true_false_set_destroy(&left_local);
    }
    break;

    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
        assert(p_expression->compound_statement != NULL);
        flow_visit_compound_statement(ctx, p_flow_set, p_expression->compound_statement);
        break;

    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);
        assert(p_expression->type_name != NULL);
        assert(p_expression->braced_initializer != NULL);
        flow_visit_bracket_initializer_list(ctx, p_flow_set, p_expression->braced_initializer);
        break;

    case UNARY_EXPRESSION_ALIGNOF:

        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_flow_set, p_expression->right, expr_true_false_set);
        }
        break;

    case UNARY_EXPRESSION_ASSERT:

        if (p_expression->right)
        {
            struct true_false_set true_false_set4 = { 0 };
            flow_visit_expression(ctx, p_flow_set, p_expression->right, &true_false_set4); //assert(p == 0);            
            true_false_set_destroy(&true_false_set4);
        }

        break;

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:

        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_flow_set, p_expression->right, expr_true_false_set);
        }
        break;

    case UNARY_EXPRESSION_NEG:
    case UNARY_EXPRESSION_PLUS:
        assert(p_expression->right != NULL);
        flow_visit_expression(ctx, p_flow_set, p_expression->right, expr_true_false_set);
        break;

    case UNARY_EXPRESSION_NOT:
        assert(p_expression->right != NULL);
        flow_visit_expression(ctx, p_flow_set, p_expression->right, expr_true_false_set);
        break;

    case UNARY_EXPRESSION_SIZEOF_TYPE:
    case UNARY_EXPRESSION_NELEMENTSOF_TYPE:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:
    case UNARY_EXPRESSION_BITNOT:
        break;

    case UNARY_EXPRESSION_ADDRESSOF:
        struct object* obj = object_get_referenced(&p_expression->right->object);
        p_expression->object = object_make_pointer(obj);
        break;

    case UNARY_EXPRESSION_CONTENT:
    {
        assert(p_expression->right != NULL);
        struct true_false_set local_true_false = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->right, &local_true_false);
        /*empty set*/
        true_false_set_destroy(&local_true_false);
    }
    break;

    case ASSIGNMENT_EXPRESSION_ASSIGN:
    case ASSIGNMENT_EXPRESSION_PLUS_ASSIGN:
    case ASSIGNMENT_EXPRESSION_MINUS_ASSIGN:
    case ASSIGNMENT_EXPRESSION_MULTI_ASSIGN:
    case ASSIGNMENT_EXPRESSION_DIV_ASSIGN:
    case ASSIGNMENT_EXPRESSION_MOD_ASSIGN:
    case ASSIGNMENT_EXPRESSION_SHIFT_LEFT_ASSIGN:
    case ASSIGNMENT_EXPRESSION_SHIFT_RIGHT_ASSIGN:
    case ASSIGNMENT_EXPRESSION_AND_ASSIGN:
    case ASSIGNMENT_EXPRESSION_OR_ASSIGN:
    case ASSIGNMENT_EXPRESSION_NOT_ASSIGN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);



        struct true_false_set right_set = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->right, &right_set);
        struct flow_set m = flow_set_merge(&right_set.true_paths, &right_set.false_paths);

        struct object* p_old_assigment_object = ctx->p_assigment_object;
        ctx->p_assigment_object = object_get_referenced(&p_expression->left->object);

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, &m, p_expression->left, &left_set);

        ctx->p_assigment_object = p_old_assigment_object; //no uninitialized warnings

        struct flow_set m2 = flow_set_merge(&left_set.true_paths, &left_set.false_paths);


        for (int i = 0; i < m2.size; i++)
        {
            struct flow_path* path = m2.data[i];
            struct object* left = flow_path_find(path, &p_expression->left->object);
            struct object* right = flow_path_find(path, &p_expression->right->object);
            if (left == NULL || right == NULL) continue;
            flow_path_set(path, &p_expression->left->object, right);

            struct flow_path* clone = calloc(1, sizeof * clone);
            *clone = flow_path_dup(path);

            if (object_to_bool_ex(right))
            {
                flow_set_push_back(&expr_true_false_set->true_paths, clone);
            }
            else
            {

                flow_set_push_back(&expr_true_false_set->false_paths, clone);
            }
        }


        true_false_set_destroy(&left_set);
        true_false_set_destroy(&right_set);
    }
    break;

    case MULTIPLICATIVE_EXPRESSION_DIV:
    {

        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        struct true_false_set right_set = { 0 };

        flow_visit_expression(ctx, p_flow_set, p_expression->left, &left_set);
        flow_visit_expression(ctx, p_flow_set, p_expression->right, &right_set);

        struct object* variables[2] = { 0 };
        int n = expression_get_variables(p_expression->right, _Countof(variables), variables);

        if (n > 0)
        {
            struct object* var = variables[0];
            struct var_iterator it = flow_set_var_iterator(p_flow_set, var);
            while (var_iterator_next(&it))
            {
                struct object copy = { 0 };
                object_swap(&copy, var);
                object_swap(var, it.p_value);
                struct object r = expression_eval(p_expression->right);
                if (object_has_constant_value(&r))
                {
                    if (object_to_bool(&r) == false)
                    {
                        compiler_diagnostic(W_FLOW_DIVIZION_BY_ZERO,
                            ctx->ctx,
                            p_expression->right->first_token,
                            NULL,
                            "division by zero");
                    }
                }
                object_swap(&copy, var); //restore
            }
        }
        true_false_set_destroy(&left_set);
        true_false_set_destroy(&right_set);
    }
    break;
    case CAST_EXPRESSION:
    case MULTIPLICATIVE_EXPRESSION_MULT:
    case MULTIPLICATIVE_EXPRESSION_MOD:
    case ADDITIVE_EXPRESSION_PLUS:
    case ADDITIVE_EXPRESSION_MINUS:
    case SHIFT_EXPRESSION_RIGHT:
    case SHIFT_EXPRESSION_LEFT:
    {
        if (p_expression->left)
        {
            struct true_false_set left_set = { 0 };
            flow_visit_expression(ctx, p_flow_set, p_expression->left, &left_set);
            true_false_set_destroy(&left_set);
        }

        if (p_expression->right)
        {
            struct true_false_set right_set = { 0 };
            flow_visit_expression(ctx, p_flow_set, p_expression->right, &right_set);
            true_false_set_destroy(&right_set);
        }
    }
    break;

    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_BIGGER_THAN:
    case RELATIONAL_EXPRESSION_LESS_THAN:
    {
        struct true_false_set true_false_set = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->left, &true_false_set);
        flow_visit_expression(ctx, p_flow_set, p_expression->right, &true_false_set);
        true_false_set_destroy(&true_false_set);
    }
    break;

    case EQUALITY_EXPRESSION_NOT_EQUAL:
    case EQUALITY_EXPRESSION_EQUAL:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);


        struct true_false_set right_true_false_set = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->right, &right_true_false_set);

        struct flow_set m = flow_set_merge(&right_true_false_set.true_paths, &right_true_false_set.false_paths);

        struct true_false_set left_true_false_set = { 0 };
        flow_visit_expression(ctx, &m, p_expression->left, &left_true_false_set);

        struct flow_set m2 = flow_set_merge(&left_true_false_set.true_paths, &left_true_false_set.false_paths);


        for (int i = 0; i < m2.size; i++)
        {
            struct flow_path* current_path = m2.data[i];
            struct object* left = flow_path_find(current_path, &p_expression->left->object);
            struct object* right = flow_path_find(current_path, &p_expression->right->object);
            if (left == NULL || right == NULL) continue;

            struct flow_path* path = calloc(1, sizeof * path);
            *path = flow_path_dup(current_path);

            if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL)
            {
                if (object_equal(left, right))
                {
                    flow_set_push_back(&expr_true_false_set->true_paths, path);
                }
                else
                {
                    flow_set_push_back(&expr_true_false_set->false_paths, path);
                }
            }
            else if (p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL)
            {
                if (object_not_equal(left, right))
                {
                    flow_set_push_back(&expr_true_false_set->true_paths, path);
                }
                else
                {
                    flow_set_push_back(&expr_true_false_set->false_paths, path);
                }
            }
        }

        debug_print_core(ctx,
            &expr_true_false_set->true_paths,
            &expr_true_false_set->false_paths,
            p_expression->last_token->next);
    }
    break;

    case LOGICAL_OR_EXPRESSION:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->left, &left_set);


        //Set all variables to false state, because otherwise, the right branch
        //would not be evaluated

        //struct flow_set previous = { 0 };
        //flow_set_swap(p_flow_set, &previous);
        //ctx->flow_set = flow_set_dup(&left_set.false_paths);
        //flow_set_i_know_this_is(ctx->flow_set, )

        //debug_print(ctx, p_flow_set,  p_expression->right->first_token);
        struct true_false_set right_set = { 0 };

        flow_visit_expression(ctx, &left_set.false_paths, p_expression->right, &right_set);


        // a      ||     b
        // |             |   ( a false, b true)  |
        // v             v                       v
        // false        true                  a false, b false
        // 

        expr_true_false_set->true_paths = flow_set_merge(&left_set.true_paths, &right_set.true_paths);
        flow_set_i_know_this_is(&expr_true_false_set->true_paths, &p_expression->right->object, true);



        //Anything that makes left false or right false also makes left && right false.

        expr_true_false_set->false_paths = flow_set_dup(&right_set.false_paths);

        //Se o B for garantidamente true, nao existe possibilite de false branch e ele deveria ser fazio.
        flow_set_i_know_this_is(&expr_true_false_set->false_paths, &p_expression->right->object, false);
        debug_print(ctx, &expr_true_false_set->false_paths, p_expression->right->last_token);


        true_false_set_destroy(&left_set);
        true_false_set_destroy(&right_set);
    }
    break;

    case LOGICAL_AND_EXPRESSION:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);
        struct true_false_set left_set = { 0 };
        struct true_false_set right_set = { 0 };

        flow_visit_expression(ctx, p_flow_set, p_expression->left, &left_set);

        //flow_set_destroy(p_flow_set);
        //ctx->flow_set = flow_set_dup(&left_set.true_paths);

        flow_visit_expression(ctx, &left_set.true_paths, p_expression->right, &right_set);


        expr_true_false_set->true_paths = flow_set_dup(&right_set.true_paths);
        expr_true_false_set->false_paths = flow_set_merge(&left_set.false_paths, &right_set.false_paths);

        true_false_set_destroy(&left_set);
        true_false_set_destroy(&right_set);
    }
    break;

    case INCLUSIVE_OR_EXPRESSION:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        // A | B
        struct true_false_set true_false_set = { 0 };

        flow_visit_expression(ctx, p_flow_set, p_expression->left, &true_false_set);
        flow_visit_expression(ctx, p_flow_set, p_expression->right, &true_false_set);

        true_false_set_destroy(&true_false_set);
    }
    break;

    case AND_EXPRESSION:
    case EXCLUSIVE_OR_EXPRESSION:

        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);
        flow_visit_expression(ctx, p_flow_set, p_expression->left, expr_true_false_set);
        flow_visit_expression(ctx, p_flow_set, p_expression->right, expr_true_false_set);
        break;

    case UNARY_EXPRESSION_TRAITS:
        break;

    case UNARY_EXPRESSION_IS_SAME:
        break;

    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
        break;

    case EXPRESSION_EXPRESSION:
        break;

    case CONDITIONAL_EXPRESSION:
    {
        assert(p_expression->condition_expr != NULL);
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);
        struct true_false_set true_false_set = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->condition_expr, &true_false_set);


        struct true_false_set set = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->left, &set);

        struct flow_set end_of_if_true = { 0 };
        flow_set_swap(&end_of_if_true, p_flow_set);


        true_false_set_destroy(&set);

        flow_set_swap(p_flow_set, &true_false_set.false_paths);
        debug_print(ctx, p_flow_set, p_expression->right->first_token);

        struct true_false_set set2 = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression->right, &set2);
        true_false_set_destroy(&set2);

        struct flow_set end_of_if_false = { 0 };
        flow_set_swap(&end_of_if_false, &true_false_set.false_paths);

        //ctx->flow_set = flow_set_merge(&end_of_if_true, &end_of_if_false);
        //debug_print(ctx, p_flow_set,  p_expression->right->last_token);        
        flow_set_destroy(&end_of_if_false);

        true_false_set_destroy(&true_false_set);
    }
    break;

    }
}
static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct expression_statement* p_expression_statement)
{

    if (p_expression_statement->expression_opt)
    {
        struct true_false_set d = { 0 };
        flow_visit_expression(ctx, p_flow_set, p_expression_statement->expression_opt, &d);

        flow_set_destroy(p_flow_set);
        *p_flow_set = flow_set_merge(&d.true_paths, &d.false_paths);

        debug_print(ctx, p_flow_set, p_expression_statement->expression_opt->last_token);
        true_false_set_destroy(&d);
    }

}
static void flow_visit_block_item_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct block_item_list* p_block_item_list);
static void flow_visit_compound_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct compound_statement* p_compound_statement)
{
    flow_visit_block_item_list(ctx, p_flow_set, &p_compound_statement->block_item_list);
}
static void flow_visit_do_while_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);

    struct true_false_set true_false_set = { 0 };

    try
    {
        if (p_iteration_statement->expression1)
        {
            flow_visit_expression(ctx, p_flow_set, p_iteration_statement->expression1, &true_false_set);
        }

        flow_visit_secondary_block(ctx, p_flow_set, p_iteration_statement->secondary_block);

        secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

    }
    catch
    {
    }

    true_false_set_destroy(&true_false_set);
}

static void flow_visit_while_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    if (p_iteration_statement->expression1 == NULL)
        return;

    struct true_false_set true_false_set = { 0 };

    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
    flow_visit_expression(ctx, p_flow_set, p_iteration_statement->expression1, &true_false_set);

    flow_visit_secondary_block(ctx, p_flow_set, p_iteration_statement->secondary_block);
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    struct true_false_set true_false_set2 = { 0 };
    flow_visit_expression(ctx, p_flow_set, p_iteration_statement->expression1, &true_false_set2);
    true_false_set_destroy(&true_false_set2);

    flow_visit_secondary_block(ctx, p_flow_set, p_iteration_statement->secondary_block);

    secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

    true_false_set_destroy(&true_false_set);
}

static void flow_visit_for_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct iteration_statement* p_iteration_statement)
{
    /*
      TODO
       {
        int i = 0;
        if (condition)
        {
            //second-block
            i++;
            if (condition)
            {
                //second-block
                i++;
                //recursive
            }
        }
    }
    */

    assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);

    struct true_false_set d = { 0 };

    try
    {
        if (p_iteration_statement->declaration &&
            p_iteration_statement->declaration->init_declarator_list.head)
        {
            flow_visit_init_declarator_list(ctx, p_flow_set, &p_iteration_statement->declaration->init_declarator_list);
        }

        if (p_iteration_statement->expression0)
        {
            flow_visit_expression(ctx, p_flow_set, p_iteration_statement->expression0, &d);
        }

        if (p_iteration_statement->expression1)
        {
            flow_visit_expression(ctx, p_flow_set, p_iteration_statement->expression1, &d);
        }


        diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
        flow_visit_secondary_block(ctx, p_flow_set, p_iteration_statement->secondary_block);
        diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

        if (p_iteration_statement->expression2)
        {
            flow_visit_expression(ctx, p_flow_set, p_iteration_statement->expression2, &d);
        }
        const bool b_secondary_block_ends_with_jump =
            secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

        /*we visit again*/
        if (!b_secondary_block_ends_with_jump)
        {
            flow_visit_secondary_block(ctx, p_flow_set, p_iteration_statement->secondary_block);
        }
    }
    catch
    {
    }
    true_false_set_destroy(&d);
}

static void flow_visit_iteration_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct iteration_statement* p_iteration_statement)
{
    struct flow_set old_continue_set = { 0 };
    flow_set_swap(&ctx->continue_set, &old_continue_set);

    struct flow_set old_break_set = { 0 };
    flow_set_swap(&ctx->break_set, &old_break_set);

    switch (p_iteration_statement->first_token->type)
    {
    case  TK_KEYWORD_WHILE:
        flow_visit_while_statement(ctx, p_flow_set, p_iteration_statement);
        break;
    case TK_KEYWORD_DO:
        flow_visit_do_while_statement(ctx, p_flow_set, p_iteration_statement);
        break;
    case TK_KEYWORD_FOR:
        flow_visit_for_statement(ctx, p_flow_set, p_iteration_statement);
        break;
    default:
        assert(false);
        break;
    }

    flow_set_swap(&ctx->continue_set, &old_continue_set); //restore
    flow_set_swap(&ctx->break_set, &old_break_set); //restore
    flow_set_destroy(&old_continue_set);
    flow_set_destroy(&old_break_set);
}

static void flow_visit_jump_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct jump_statement* p_jump_statement)
{
    try
    {
        if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
        {
            struct flow_set s = { 0 };
            flow_set_swap(&s, &ctx->catch_set);
            ctx->catch_set = flow_set_merge(&s, p_flow_set);
            flow_set_destroy(&s);
            flow_set_clear(p_flow_set);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
        {
            if (p_jump_statement->expression_opt)
            {
                struct true_false_set d = { 0 };
                flow_visit_expression(ctx, p_flow_set, p_jump_statement->expression_opt, &d);
                true_false_set_destroy(&d);
            }
            flow_set_clear(p_flow_set);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
        {
            struct flow_set s = { 0 };
            flow_set_swap(&s, &ctx->continue_set);
            ctx->continue_set = flow_set_merge(&s, p_flow_set);
            flow_set_destroy(&s);
            flow_set_clear(p_flow_set);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            struct flow_set s = { 0 };
            flow_set_swap(&s, &ctx->break_set);
            ctx->break_set = flow_set_merge(&s, p_flow_set);
            flow_set_destroy(&s);
            flow_set_clear(p_flow_set);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
        {
            //goto needs a label
            assert(p_jump_statement->label != NULL);
            struct label_set* p = label_set_get(&ctx->labels, p_jump_statement->label->lexeme);
            if (p == NULL) throw;
            struct flow_set s = { 0 };
            flow_set_swap(&s, &p->flow_set);
            p->flow_set = flow_set_merge(&s, p_flow_set);
            flow_set_destroy(&s);
            flow_set_clear(p_flow_set);
        }
        else
        {
            assert(false);
        }
    }
    catch
    {
    }
}
static void flow_visit_label(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct label* p_label);
static void flow_visit_labeled_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct labeled_statement* p_labeled_statement)
{
    flow_visit_label(ctx, p_flow_set, p_labeled_statement->label);
    flow_visit_statement(ctx, p_flow_set, p_labeled_statement->statement);
}
static void flow_visit_primary_block(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct primary_block* p_primary_block)
{

    if (p_primary_block->defer_statement)
    {
        flow_visit_defer_statement(ctx, p_flow_set, p_primary_block->defer_statement);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            flow_visit_compound_statement(ctx, p_flow_set, p_primary_block->compound_statement);
        }
        else if (p_primary_block->iteration_statement)
        {
            flow_visit_iteration_statement(ctx, p_flow_set, p_primary_block->iteration_statement);
        }
        else if (p_primary_block->selection_statement)
        {
            flow_visit_selection_statement(ctx, p_flow_set, p_primary_block->selection_statement);
        }
        else if (p_primary_block->try_statement)
        {
            flow_visit_try_statement(ctx, p_flow_set, p_primary_block->try_statement);
        }
    }
}
static void flow_visit_unlabeled_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        flow_visit_primary_block(ctx, p_flow_set, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        flow_visit_expression_statement(ctx, p_flow_set, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        flow_visit_jump_statement(ctx, p_flow_set, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}
static void flow_visit_statement(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        flow_visit_labeled_statement(ctx, p_flow_set, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        flow_visit_unlabeled_statement(ctx, p_flow_set, p_statement->unlabeled_statement);
    }
}
static void flow_visit_label(struct flow_visit_ctx* ctx,
    struct flow_set* p_flow_set,
    struct label* p_label)
{
    try
    {

        if (p_label->p_first_token->type == TK_KEYWORD_CASE ||
            p_label->p_first_token->type == TK_KEYWORD_DEFAULT)
        {

            //case 1:
            // break; // flow_set is empty
            //case 2:
            //  ...

            //case 1:
            //case 2: //// flow_set not empty
            //  ...

            //bool flow_set_is_empty = (p_flow_set->size == 0);
            struct flow_set s = { 0 };
            flow_set_swap(&s, p_flow_set);
            *p_flow_set = flow_set_merge(&ctx->switch_set, &s);
            flow_set_destroy(&s);


            if (p_label->p_first_token->type == TK_KEYWORD_CASE)
            {
                flow_set_set(p_flow_set, &ctx->p_selection_statement->condition->expression->object, &p_label->constant_expression->object);
            }

            if (p_label->constant_expression && p_label->constant_expression->last_token)
              debug_print(ctx, p_flow_set, p_label->constant_expression->last_token->next);
        }
        else if (p_label->p_identifier_opt)
        {
            struct label_set* p = label_set_get(&ctx->labels, p_label->p_identifier_opt->lexeme);
            if (p == NULL) throw;

            if (p->flow_set.size > 0)
            {
                struct flow_set s = { 0 };
                flow_set_swap(&s, p_flow_set);
                *p_flow_set = flow_set_merge(&s, &p->flow_set);
                flow_set_destroy(&s);
                debug_print(ctx, p_flow_set, p_label->p_identifier_opt->next);
            }
        }
    }
    catch
    {
    }
}
static void flow_visit_block_item(struct flow_visit_ctx* ctx,
    struct flow_set* p_flow_set,
    struct block_item* p_block_item)
{
    if (p_block_item->declaration)
    {
        flow_visit_declaration(ctx, p_flow_set, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        flow_visit_unlabeled_statement(ctx, p_flow_set, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        flow_visit_label(ctx, p_flow_set, p_block_item->label);
    }
}
static void flow_visit_block_item_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct block_item_list* p_block_item_list)
{
    struct block_item* _Opt p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow_visit_block_item(ctx, p_flow_set, p_block_item);
        p_block_item = p_block_item->next;
    }
}
static void flow_visit_pragma_declaration(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct pragma_declaration* p_pragma_declaration)
{
    execute_pragma(ctx->ctx, p_pragma_declaration, true);
}
static void flow_visit_static_assert_declaration(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct static_assert_declaration* p_static_assert_declaration)
{
    struct true_false_set a = { 0 };
    flow_visit_expression(ctx, p_flow_set, p_static_assert_declaration->constant_expression, &a);

    if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG ||
        p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG_EX)
    {

        compiler_diagnostic(W_LOCATION, ctx->ctx, p_static_assert_declaration->first_token, NULL, "static_debug");
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_STATE)
    {
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_SET)
    {
    }

    struct token* position = p_static_assert_declaration->first_token;
    if (p_static_assert_declaration->first_token->type == TK_KEYWORD__STATIC_ASSERT)
    {
        if (object_has_constant_value(&p_static_assert_declaration->constant_expression->object))
        {
            //in this case it was evaluated at parser phase.
        }
        else
        {
            struct object* variables[2] = { 0 };
            int n = expression_get_variables(p_static_assert_declaration->constant_expression,
                _Countof(variables),
                variables);

            if (n > 0)
            {
                struct object* var = variables[0];
                struct var_iterator it = flow_set_var_iterator(p_flow_set, var);
                while (var_iterator_next(&it))
                {
                    struct object copy = { 0 };
                    object_swap(&copy, var);
                    object_swap(var, it.p_value);
                    struct object r = expression_eval(p_static_assert_declaration->constant_expression);
                    if (object_has_constant_value(&r))
                    {
                        if (!object_to_bool(&r))
                        {
                            if (p_static_assert_declaration->string_literal_opt)
                            {
                                compiler_diagnostic(C_ERROR_STATIC_ASSERT_FAILED, ctx->ctx, position, NULL, "static_assert failed %s\n",
                                    p_static_assert_declaration->string_literal_opt->lexeme);
                            }
                            else
                            {
                                compiler_diagnostic(C_ERROR_STATIC_ASSERT_FAILED, ctx->ctx, position, NULL, "static_assert failed");
                            }
                        }
                    }
                    else
                    {
                        compiler_diagnostic(C_ERROR_STATIC_ASSERT_FAILED,
                            ctx->ctx,
                            position,
                            NULL,
                            "expression does not evaluate to known value");
                    }
                    object_swap(&copy, var); //restore
                }
            }
        }

    }

    true_false_set_destroy(&a);
}
static void flow_visit_direct_declarator(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration* _Opt parameter = NULL;

        if (p_direct_declarator->function_declarator->parameter_type_list_opt &&
            p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }



        while (parameter)
        {


            flow_visit_declaration_specifiers(ctx, p_flow_set, parameter->declaration_specifiers, &parameter->declarator->type);

            if (parameter->declarator)
            {
                flow_visit_declarator(ctx, p_flow_set, parameter->declarator);
            }

            parameter = parameter->next;
        }
    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            struct true_false_set a = { 0 };
            flow_visit_expression(ctx, p_flow_set, p_direct_declarator->array_declarator->assignment_expression, &a);
            true_false_set_destroy(&a);
        }

    }
}

static void add_object(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct object* p_object)
{
    p_object = object_get_referenced(p_object);

    if (p_object->members != NULL)
    {
        struct object* _Opt p = p_object->members;
        while (p)
        {
            add_object(ctx, p_flow_set, p);
            p = p->next;
        }
    }
    else
    {
        flow_set_set(p_flow_set, p_object, p_object);
    }
}

static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct declarator* p_declarator)
{
    try
    {
        if (p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
        {
            add_object(ctx, p_flow_set, &p_declarator->object);
            debug_print(ctx, p_flow_set, p_declarator->first_token_opt);
        }

        if (p_declarator->direct_declarator)
        {
            flow_visit_direct_declarator(ctx, p_flow_set, p_declarator->direct_declarator);
        }
    }
    catch
    {
    }
}
static void flow_visit_init_declarator_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_flow_set, p_init_declarator);
        p_init_declarator = p_init_declarator->next;
    }
}
static void flow_visit_member_declarator(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        flow_visit_declarator(ctx, p_flow_set, p_member_declarator->declarator);
    }
}
static void flow_visit_member_declarator_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* _Opt p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        flow_visit_member_declarator(ctx, p_flow_set, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}
static void flow_visit_member_declaration(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct member_declaration* p_member_declaration)
{
    if (p_member_declaration->member_declarator_list_opt)
    {
        flow_visit_member_declarator_list(ctx, p_flow_set, p_member_declaration->member_declarator_list_opt);
    }
}
static void flow_visit_member_declaration_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* _Opt p_member_declaration = p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow_visit_member_declaration(ctx, p_flow_set, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    flow_visit_member_declaration_list(ctx, p_flow_set, &p_struct_or_union_specifier->member_declaration_list);
}

static void flow_visit_enumerator(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct enumerator* p_enumerator)
{
    struct true_false_set a = { 0 };
    if (p_enumerator->constant_expression_opt)
        flow_visit_expression(ctx, p_flow_set, p_enumerator->constant_expression_opt, &a);
    true_false_set_destroy(&a);
}

static void flow_visit_enumerator_list(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* _Opt current = p_enumerator_list->head;
    while (current)
    {
        flow_visit_enumerator(ctx, p_flow_set, current);
        current = current->next;
    }
}

static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct enum_specifier* p_enum_specifier)
{
    flow_visit_enumerator_list(ctx, p_flow_set, &p_enum_specifier->enumerator_list);
}
static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_flow_set, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        flow_visit_enum_specifier(ctx, p_flow_set, p_type_specifier->enum_specifier);
    }
}

static void flow_visit_type_specifier_qualifier(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_specifier)
    {
        flow_visit_type_specifier(ctx, p_flow_set, p_type_specifier_qualifier->type_specifier);
    }
}

static void flow_visit_declaration_specifier(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct declaration_specifier* p_declaration_specifier)
{
    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow_visit_type_specifier_qualifier(ctx, p_flow_set, p_declaration_specifier->type_specifier_qualifier);
    }
}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* _Opt p_type_opt)
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

    struct declaration_specifier* _Opt p_declaration_specifier = p_declaration_specifiers->head;


    while (p_declaration_specifier)
    {
        flow_visit_declaration_specifier(ctx, p_flow_set, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}

void flow_visit_declaration(struct flow_visit_ctx* ctx, struct flow_set* p_flow_set, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        flow_visit_static_assert_declaration(ctx, p_flow_set, p_declaration->static_assert_declaration);
    }

    if (p_declaration->pragma_declaration)
    {
        flow_visit_pragma_declaration(ctx, p_flow_set, p_declaration->pragma_declaration);
    }


    if (p_declaration->declaration_specifiers)
    {
        if (p_declaration->init_declarator_list.head)
        {
            flow_visit_declaration_specifiers(ctx, p_flow_set, p_declaration->declaration_specifiers,
                &p_declaration->init_declarator_list.head->p_declarator->type);
        }
        else
        {
            flow_visit_declaration_specifiers(ctx, p_flow_set, p_declaration->declaration_specifiers, NULL);

        }
    }

    if (p_declaration->init_declarator_list.head)
    {
        flow_visit_init_declarator_list(ctx, p_flow_set, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        flow_visit_compound_statement(ctx, p_flow_set, p_declaration->function_body);
        type_destroy(&type);
    }
}

void flow_start_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration)
{
    ctx->p_declaration = p_declaration;
    if (p_declaration->function_body)
    {
        struct flow_set flow_set = { 0 };
        flow_visit_declaration(ctx, &flow_set, p_declaration);
        assert(p_declaration->function_body != NULL); //flow_visit_declaration does not change this
    }
    else
    {
        //disabled TODO
       // flow_visit_declaration(ctx, p_flow_set,  p_declaration);
    }
}

void flow_visit_ctx_destroy(_Dtor struct flow_visit_ctx* p)
{

}


#pragma region


int flow_path_value_release(struct flow_path_value* p)
{
    assert(p->ref_count > 0);

    p->ref_count--;
    if (p->ref_count == 0)
    {
        free(p);
        return 0;
    }
    return p->ref_count;
}


void flow_path_destroy(struct flow_path* arr)
{
    for (int i = 0; i < arr->size; ++i)
    {
        flow_path_value_release(arr->data[i]);
    }
    free(arr->data);
}

void flow_path_delete(struct flow_path* p)
{
    if (p)
    {
        flow_path_destroy(p);
        free(p);
    }
}

// Helper function to find the insertion index to keep data sorted by address
[[nodiscard]]
static int find_insert_index(struct flow_path* arr, const struct object* key)
{
    int left = 0;
    int right = arr->size;

    while (left < right)
    {
        int mid = left + (right - left) / 2;
        if (arr->data[mid]->key < key)
            left = mid + 1;
        else
            right = mid;
    }
    return left;
}

[[nodiscard]]
struct object* flow_path_find(const struct flow_path* arr, const struct object* key)
{
    key = object_get_referenced(key);
    
    if (object_has_constant_value(key))
    {
        return key;
    }

    int left = 0;
    int right = arr->size - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        if (arr->data[mid]->key == key)
        {
            return (struct object*)object_get_referenced(&arr->data[mid]->value);
        }
        else if (arr->data[mid]->key < key)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return NULL; // Not found
}

struct flow_path flow_path_dup(const struct flow_path* source)
{
    struct flow_path result = {
        .capacity = source->size,
        .size = source->size,
        .data = malloc(sizeof(struct flow_path_value*) * source->size)
    };

    if (!result.data)
    {
        result.capacity = 0;
        result.size = 0;
        return result;
    }

    for (int i = 0; i < source->size; i++)
    {
        struct flow_path_value* p = source->data[i];
        result.data[i] = p;
        p->ref_count++;
    }

    return result;
}

struct flow_path* flow_path_clone(const struct flow_path* source)
{
    struct flow_path* result = 0;
    try
    {
        result = calloc(1, sizeof * result);
        if (result == NULL)
            throw;
        *result = flow_path_dup(source);
    }
    catch
    {
    }
    return result;
}

[[nodiscard]]
static int flow_path_reserve(struct flow_path* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

void flow_path_print(struct flow_path* map, int line, int col)
{
    printf(RESET);
    for (int i = 0; i < map->size; i++)
    {
        c_gotoxy(col, line + i);

        struct flow_path_value* entry = map->data[i];

        printf("%p: ", entry->key);

        if (entry->key->debug_name)
        {
            printf("%s", entry->key->debug_name);
        }

        switch (entry->value.state)
        {
        case CONSTANT_VALUE_STATE_UNINITIALIZED:
            printf(RED);
            printf(" uninitialized ");
            printf(RESET);
            break;

        case CONSTANT_VALUE_EQUAL:
        case CONSTANT_VALUE_STATE_CONSTANT:
            printf(" == ");
            break;
        case CONSTANT_VALUE_NOT_EQUAL:
            printf(" != ");
            break;

        case CONSTANT_VALUE_STATE_ANY:
            printf(" any ");
            break;
        }
        if (entry->value.state != CONSTANT_VALUE_STATE_UNINITIALIZED &&
            entry->value.state != CONSTANT_VALUE_STATE_ANY)
        {
            char buffer[200] = { 0 };
            object_to_string(&entry->value, buffer, 200);
            printf("%s", buffer);
        }
        //c_gotoxy(left_table + 15, line);
    }
}

[[nodiscard]]
int flow_path_add_path_value(struct flow_path* arr, const struct object* key, struct flow_path_value* p_new)
{
    key = object_get_referenced(key);
    //assert(key->state != CONSTANT_VALUE_STATE_CONSTANT);

    int er = 0;
    try
    {
        int idx = find_insert_index(arr, key);
        if (idx < arr->size)
        {
            if (arr->data[idx]->key == key)
            {
                flow_path_value_release(arr->data[idx]);
                arr->data[idx] = p_new;
                p_new->ref_count++;
            }
            else
            {
                er = flow_path_reserve(arr, arr->size + 1);
                if (er != 0)
                    throw;

                memmove(&arr->data[idx + 1], &arr->data[idx], (arr->size - idx) * sizeof(struct flow_path_value*));

                arr->data[idx] = p_new;
                p_new->ref_count++;
                arr->size++;
            }
        }
        else
        {
            er = flow_path_reserve(arr, arr->size + 1);
            if (er != 0)
                throw;

            assert(idx == arr->size);
            arr->data[idx] = p_new;
            p_new->ref_count++;
            arr->size++;
        }
    }
    catch
    {
    }

    return er;
}

[[nodiscard]]
int flow_path_set(struct flow_path* arr, const struct object* key, const struct object* value)
{
    key = object_get_referenced(key);
    assert(key->state != CONSTANT_VALUE_STATE_CONSTANT);

    int er = 0;
    try
    {
        struct flow_path_value* p_new = malloc(sizeof * p_new);
        if (p_new == NULL)
        {
            er = ENOMEM;
            throw;
        }

        p_new->ref_count = 1;
        p_new->key = key;
        p_new->value = object_dup(value);

        int idx = find_insert_index(arr, key);
        if (idx < arr->size)
        {
            if (arr->data[idx]->key == key)
            {
                flow_path_value_release(arr->data[idx]);
                arr->data[idx] = p_new;
                p_new->ref_count++;
            }
            else
            {
                er = flow_path_reserve(arr, arr->size + 1);
                if (er != 0)
                    throw;
                memmove(&arr->data[idx + 1], &arr->data[idx], (arr->size - idx) * sizeof(struct flow_path_value*));
                flow_path_value_release(arr->data[idx]);
                arr->data[idx] = p_new;
                p_new->ref_count++;
                arr->size++;
            }
        }
        else
        {
            er = flow_path_reserve(arr, arr->size + 1);
            if (er != 0)
                throw;

            assert(idx == arr->size);
            arr->data[idx] = p_new;
            p_new->ref_count++;
            arr->size++;
        }
    }
    catch
    {
    }
    return er;
}

[[nodiscard]]
static int flow_set_reserve(struct flow_set* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int flow_set_remove(struct flow_set* arr, int index)
{
    if (index < 0 || index >= arr->size)
        return EINVAL;

    struct flow_path* map = arr->data[index];

    if (arr->size > 1 && index < arr->size - 1)
    {
        memmove(&arr->data[index], &arr->data[index + 1], (arr->size - index - 1) * sizeof(struct flow_path_value*));
    }
    arr->size--;
    flow_path_delete(map);

    return 0;
}

[[nodiscard]]
int flow_set_push_back(struct flow_set* p, struct flow_path* book)
{
    if (p->size == INT_MAX)
    {
        return EOVERFLOW;
    }

    if (p->size + 1 > p->capacity)
    {
        int new_capacity = 0;
        if (p->capacity > (INT_MAX - p->capacity / 2))
        {
            /*overflow*/
            new_capacity = INT_MAX;
        }
        else
        {
            new_capacity = p->capacity + p->capacity / 2;
            if (new_capacity < p->size + 1)
            {
                new_capacity = p->size + 1;
            }
        }

        int error = flow_set_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = book; /*VIEW*/
    p->size++;

    return 0;
}

void flow_set_clear(struct flow_set* flow)
{
    flow_set_destroy(flow);
    flow->capacity = 0;
    flow->data = NULL;
    flow->size = 0;
}

void flow_set_destroy(struct flow_set* flow)
{
    for (int i = 0; i < flow->size; i++)
    {
        flow_path_delete(flow->data[i]);
    }
    free(flow->data);
}

struct flow_set flow_set_dup(const struct flow_set* flow)
{
    struct flow_set result = { 0 };
    try
    {
        for (int i = 0; i < flow->size; i++)
        {
            struct flow_path* clone = flow_path_clone(flow->data[i]);
            if (clone == 0)
                throw;
            flow_set_push_back(&result, clone);
        }
    }
    catch
    {
    }

    return result;
}

int flow_set_set(struct flow_set* flow, const struct object* key, const struct object* value)
{
    key = object_get_referenced(key);
    value = object_get_referenced(value);

    ///assert(key->state != CONSTANT_VALUE_STATE_CONSTANT);

    /*
    *  Os objectos são colocados individualmente no mapa.
    */
    assert(key->members == NULL);
    assert(value->members == NULL);


    int er = 0;
    try
    {
        if (flow->size == 0)
        {
            struct flow_path* new_map = calloc(1, sizeof * new_map);
            if (new_map == NULL)
            {
                er = ENOMEM;
                throw;
            }

            er = flow_set_push_back(flow, new_map);
            if (er != 0)
            {
                throw;
            }
        }

        struct flow_path_value* p_new = malloc(sizeof * p_new);
        if (p_new == NULL)
        {
            er = ENOMEM;
            throw;
        }

        p_new->ref_count = 1;
        p_new->key = key;
        p_new->value = object_dup(value);

        for (int i = 0; i < flow->size; i++)
        {
            int er = flow_path_add_path_value(flow->data[i], key, p_new);
            if (er != 0)
                throw;
        }

        flow_path_value_release(p_new);
    }
    catch
    {
    }

    return er;
}

struct object* flow_set_has_known_value(const struct flow_set* src, const struct object* key)
{
    key = object_get_referenced(key);

    struct object* p_reference = NULL;

    for (size_t i = 0; i < src->size; ++i)
    {
        struct object* p = flow_path_find(src->data[i], key);
        if (p)
        {
            if (p->state == CONSTANT_VALUE_EQUAL)
            {
                if (p_reference == NULL)
                {
                    p_reference = p;
                }
                else
                {
                    //if (object_same_value(candidate, p))
                    //{
                        //return NULL;
                    //}
                }
            }
            else
            {
                return NULL;
            }
        }
    }
    return p_reference;
}


bool flow_set_can_be_uninitialized(const struct flow_set* src, const struct object* key)
{
    for (size_t i = 0; i < src->size; ++i)
    {
        struct object* p = flow_path_find(src->data[i], key);
        if (p)
        {
            if (p->state == CONSTANT_VALUE_STATE_UNINITIALIZED)
                return true;
        }
    }
    return false;
}

void flow_set_swap(struct flow_set* a, struct flow_set* b)
{
    struct flow_set temp = *a;
    *a = *b;
    *b = temp;
}


struct flow_set flow_set_merge(const struct flow_set* a, const struct flow_set* b)
{
    int er = 0;
    struct flow_set result = { 0 };
    try
    {
        for (int i = 0; i < a->size; i++)
        {
            struct flow_path* clone = flow_path_clone(a->data[i]);
            if (clone == NULL) throw;
            er = flow_set_push_back(&result, clone);
            if (er != 0) throw;
        }
        for (int i = 0; i < b->size; i++)
        {
            struct flow_path* clone = flow_path_clone(b->data[i]);
            if (clone == NULL) throw;
            er = flow_set_push_back(&result, clone);
            if (er != 0) throw;
        }
    }
    catch
    {

    }
    return result;
}



void flow_set_print(const struct flow_set* src, int line, int col)
{
    for (size_t i = 0; i < src->size; ++i)
    {
        flow_path_print(src->data[i], line, col);
        line += src->data[i]->size + 1;
    }
}

static void flow_set_remove_states_where_object_is_(const struct flow_set* src, struct object* key, bool remove_if_true)
{
    for (size_t i = 0; i < src->size; /*empty*/)
    {
        struct object* p = flow_path_find(src->data[i], key);

        if (p == NULL)
            continue;

        bool remove = false;

        if (p->state == CONSTANT_VALUE_EQUAL)
        {
            if (remove_if_true)
                remove = object_to_bool(p);
            else
                remove = !object_to_bool(p);
        }

        if (remove)
        {
            flow_set_remove(src, i);
        }
        else
        {
            i++;
        }
    }
}


void flow_set_remove_states_where_object_is_true(const struct flow_set* src, const struct object* key)
{
    flow_set_remove_states_where_object_is_(src, key, true);
}

void flow_set_remove_states_where_object_is_false(const struct flow_set* src, const struct object* key)
{
    flow_set_remove_states_where_object_is_(src, key, false);
}

bool flow_set_eval(const struct flow_set* src, struct object* key, int op, struct object* b, bool* result)
{
    bool r = false;

    for (size_t i = 0; i < src->size; i++)
    {

        struct object* p = flow_path_find(src->data[i], key);
        if (p == NULL)
            return false;



        if (p->state == CONSTANT_VALUE_EQUAL)
        {

        }
        else if (p->state == CONSTANT_VALUE_NOT_EQUAL && op == '!=')
        {
            if (i == 0)
            {
                r = object_equal(p, b);
                *result = r;
            }
            else
            {
                r = r && object_not_equal(p, b);
            }
        }
        else if (p->state == CONSTANT_VALUE_EQUAL)

        {
            if (op == '==')
                r = object_equal(p, b);
            else if (op == '!=')
                r = object_not_equal(p, b);
            *result = r;
        }
        else
        {
            return false;
        }
        if (remove)
        {
            flow_set_remove(src, i);
        }
        else
        {
            i++;
        }
    }
    return true;
}

bool flow_set_i_know_this_is(struct flow_set* src, const struct object* key, bool istrue)
{
    key = object_get_referenced(key);

    for (size_t i = 0; i < src->size; )
    {
        struct object* p = flow_path_find(src->data[i], key);
        if (p == NULL)
        {
            i++;
            continue;
        }

        bool remove_path = false;


        switch (p->state)
        {
        case CONSTANT_VALUE_STATE_UNINITIALIZED:
            break;

        case CONSTANT_VALUE_STATE_ANY:
        {
            struct object zero = object_dup(p);
            object_set_signed_int(&zero, 0);
            zero.state = istrue ? CONSTANT_VALUE_NOT_EQUAL : CONSTANT_VALUE_EQUAL;
            flow_path_set(src->data[i], key, &zero);
        }
        break;


        case CONSTANT_VALUE_NOT_EQUAL:
        {
            //const bool current_value_is_true = object_to_bool(p);

            struct object zero = object_dup(p);
            object_set_signed_int(&zero, 0);
            zero.state = istrue ? CONSTANT_VALUE_NOT_EQUAL : CONSTANT_VALUE_EQUAL;
            flow_path_set(src->data[i], key, &zero);

        }
        break;

        case CONSTANT_VALUE_STATE_CONSTANT:
        case CONSTANT_VALUE_EQUAL:
            remove_path = istrue ? !object_to_bool(p) : object_to_bool(p);
            break;
        }

        if (remove_path)
        {
            flow_set_remove(src, i);
        }
        else
        {
            i++;
        }
    }
    return true;
}

bool var_iterator_next(struct var_iterator* p_it)
{
    do
    {
        if (p_it->next_index >= p_it->p_flow_set->size)
            return false;
        if (object_has_constant_value(p_it->key))
            p_it->p_value = p_it->key;
        else
            p_it->p_value = flow_path_find(p_it->p_flow_set->data[p_it->next_index], p_it->key);

        p_it->next_index++;

    } while (p_it->p_value == NULL);

    return true;
}

struct var_iterator flow_set_var_iterator(const struct flow_set* src, struct object* key)
{
    struct var_iterator it = {
     .next_index = 0,
     .p_flow_set = src,
     .p_value = NULL,
     .key = key
    };

    return it;
}

#pragma endregion
