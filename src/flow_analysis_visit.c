#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "flow_analysis_visit.h"
#include "expressions.h"
#include "annotations.h"

struct flow_defer_scope
{
    struct declarator* declarator; // declarator 
    struct defer_statement* defer_statement; // defer 
    struct try_statement* p_try_statement; //try
    struct selection_statement* p_selection_statement2; //if swith
    struct iteration_statement* p_iteration_statement; //for do while
    struct statement* p_statement; //
    struct compound_statement* p_function_body;
    struct flow_defer_scope* lastchild;

    struct flow_defer_scope* previous;
};


static void flow_visit_attribute_specifier_sequence(struct flow_analysis_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence);


static void flow_visit_secondary_block(struct flow_analysis_visit_ctx* ctx, struct secondary_block* p_secondary_block);
static void flow_visit_struct_or_union_specifier(struct flow_analysis_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void flow_visit_expression(struct flow_analysis_visit_ctx* ctx, struct expression* p_expression);
static void flow_visit_statement(struct flow_analysis_visit_ctx* ctx, struct statement* p_statement);
static void flow_visit_enum_specifier(struct flow_analysis_visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void flow_visit_type_specifier(struct flow_analysis_visit_ctx* ctx, struct type_specifier* p_type_specifier);


static bool check_defer_and_variables(struct flow_analysis_visit_ctx* ctx,
    struct flow_defer_scope* deferblock,
    struct token* position_token)
{
    bool found_error = false;

    struct flow_defer_scope* deferchild = deferblock->lastchild;
    while (deferchild != NULL)
    {
        if (deferchild->defer_statement)
        {
            flow_visit_secondary_block(ctx, deferchild->defer_statement->secondary_block);
        }
        else if (deferchild->declarator)
        {
            struct declarator* p_declarator = deferchild->declarator;
            /*
                let's print the declarators that were not cleared for these
                flags
              */
            if (p_declarator->declarator_flags & DECLARATOR_MUST_DESTROY)
            {
                const char* name = p_declarator->name ? p_declarator->name->lexeme : "";

                found_error = true;
                compiler_set_error_with_token(C_DESTRUCTOR_MUST_BE_CALLED_BEFORE_END_OF_SCOPE, ctx->ctx,
                    position_token,
                    "variable '%s' was not moved/destroyed",
                    name);
            }

            if (p_declarator->declarator_flags & DECLARATOR_MUST_FREE)
            {
                found_error = true;
                compiler_set_error_with_token(C_FREE_MUST_BE_CALLED_BEFORE_END_OF_SCOPE, ctx->ctx,
                    position_token,
                    "memory pointed by '%s' not moved/deleted",
                    p_declarator->name->lexeme);
            }

        }

        deferchild = deferchild->previous;
    }
    return found_error;
}

static bool check_all_defer_until_try(struct flow_analysis_visit_ctx* ctx, struct flow_defer_scope* deferblock,
    struct token* position_token)
{
    bool found_error = false;
    struct flow_defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        if (check_defer_and_variables(ctx, p_defer, position_token))
            found_error = true;

        if (p_defer->p_try_statement)
        {
            break;
        }

        p_defer = p_defer->previous;
    }
    return found_error;
}

static bool flow_find_label_statement(struct flow_analysis_visit_ctx* ctx, struct statement* statement, const char* label);
static bool flow_find_label_unlabeled_statement(struct flow_analysis_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, const char* label)
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
                    if (flow_find_label_unlabeled_statement(ctx, block_item->unlabeled_statement, label))
                    {
                        return true;
                    }
                }

                block_item = block_item->next;
            }
        }
        else if (p_unlabeled_statement->primary_block->selection_statement)
        {
            if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->selection_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt)
            {
                if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->try_statement)
        {
            if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->try_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt)
            {
                if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->iteration_statement)
        {
            if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->iteration_statement->secondary_block->statement, label))
            {
                return true;
            }
        }
    }
    return false;
}

static bool flow_find_label_statement(struct flow_analysis_visit_ctx* ctx, struct statement* statement, const char* label)
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
        if (flow_find_label_unlabeled_statement(ctx, statement->unlabeled_statement, label))
            return true;
    }
    return false;
}

static bool flow_find_label_scope(struct flow_analysis_visit_ctx* ctx, struct flow_defer_scope* deferblock, const char* label)
{
    if (deferblock->p_iteration_statement)
    {
        if (deferblock->p_iteration_statement->secondary_block)
        {
            if (flow_find_label_statement(ctx, deferblock->p_iteration_statement->secondary_block->statement, label))
                return true;
        }
    }
    else if (deferblock->p_selection_statement2)
    {
        if (deferblock->p_selection_statement2->secondary_block)
        {
            if (flow_find_label_statement(ctx, deferblock->p_selection_statement2->secondary_block->statement, label))
                return true;
        }

        if (deferblock->p_selection_statement2->else_secondary_block_opt)
        {
            if (flow_find_label_statement(ctx, deferblock->p_selection_statement2->else_secondary_block_opt->statement, label))
                return true;
        }
    }
    else if (deferblock->p_try_statement)
    {
        if (deferblock->p_try_statement->secondary_block)
        {
            if (flow_find_label_statement(ctx, deferblock->p_try_statement->secondary_block->statement, label))
                return true;
        }

        if (deferblock->p_try_statement->catch_secondary_block_opt)
        {
            if (flow_find_label_statement(ctx, deferblock->p_try_statement->catch_secondary_block_opt->statement, label))
                return true;
        }
    }
    else if (deferblock->p_statement)
    {
        if (flow_find_label_statement(ctx, deferblock->p_statement, label))
            return true;
    }
    return false;
}

static bool check_all_defer_until_label(struct flow_analysis_visit_ctx* ctx, struct flow_defer_scope* deferblock, const char* label,
    struct token* position_token)
{
    bool error_found = false;
    /*
    * Precisamos saber quantos escopos nós saimos até achar o label.
    * Para isso procuramos no escopo atual aonde aparede o goto.
    * Se o label não esta diretamente neste escopo ou dentro de algum escopo interno
    * Não nós imprimos os defers pois estamos saindo do escopo e vamos para o escopo
    * de cima. Assim vamos repetindo em cada saida de escopo imprimos o defer.
    */
    struct flow_defer_scope* p_defer = deferblock;

    while (p_defer != NULL)
    {
        if (!flow_find_label_scope(ctx, p_defer, label))
        {
            error_found = error_found || check_defer_and_variables(ctx, p_defer, position_token);
        }
        else
        {
            break;
        }
        p_defer = p_defer->previous;
    }
    return error_found;
}

static bool check_all_defer_until_iter(struct flow_analysis_visit_ctx* ctx,
    struct flow_defer_scope* deferblock,
    struct token* position_token)
{
    bool error_found = false;
    struct flow_defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        if (check_defer_and_variables(ctx, p_defer, position_token))
            error_found = true;
        if (p_defer->p_iteration_statement)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
    return error_found;
}

static bool check_all_defer_until_end(struct flow_analysis_visit_ctx* ctx, struct flow_defer_scope* deferblock, struct token* position_token)
{
    bool found_found = false;
    struct flow_defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        found_found = found_found || check_defer_and_variables(ctx, p_defer, position_token);
        p_defer = p_defer->previous;
    }
    return found_found;
}

static void flow_visit_secondary_block(struct flow_analysis_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    flow_visit_statement(ctx, p_secondary_block->statement);
}

static void flow_visit_defer_statement(struct flow_analysis_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{

    struct flow_defer_scope* p_defer = calloc(1, sizeof * p_defer);
    p_defer->defer_statement = p_defer_statement;
    p_defer->previous = ctx->tail_block->lastchild;
    ctx->tail_block->lastchild = p_defer;

    /*
      We are not going to visit the secondary block here because
      this is not the place were defer is executed.
    */
}

static void flow_visit_try_statement(struct flow_analysis_visit_ctx* ctx, struct try_statement* p_try_statement)
{

    struct flow_defer_scope* p_defer = calloc(1, sizeof * p_defer);
    p_defer->previous = ctx->tail_block;
    ctx->tail_block = p_defer;
    p_defer->p_try_statement = p_try_statement;

    if (p_try_statement->secondary_block)
        flow_visit_secondary_block(ctx, p_try_statement->secondary_block);



    check_defer_and_variables(ctx, p_defer, p_try_statement->secondary_block->last_token);



    if (ctx->tail_block)
    {
        //POP
        ctx->tail_block = ctx->tail_block->previous;
    }

    free(p_try_statement->first_token->lexeme);
    p_try_statement->first_token->lexeme = strdup("if (1) /*try*/");


    char buffer[50] = {0};
    if (p_try_statement->catch_secondary_block_opt)
    {

        snprintf(buffer, sizeof buffer, "else _catch_label_%d:", p_try_statement->try_catch_block_index);

        free(p_try_statement->catch_token_opt->lexeme);
        p_try_statement->catch_token_opt->lexeme = strdup(buffer);

        flow_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
    }
    else
    {
        snprintf(buffer, sizeof buffer, "} else {_catch_label_%d:;}", p_try_statement->try_catch_block_index);
        free(p_try_statement->last_token->lexeme);
        p_try_statement->last_token->lexeme = strdup(buffer);
    }
}

static void flow_visit_selection_statement(struct flow_analysis_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{

    //PUSH
    struct flow_defer_scope* p_defer = calloc(1, sizeof * p_defer);
    p_defer->previous = ctx->tail_block;
    ctx->tail_block = p_defer;
    p_defer->p_selection_statement2 = p_selection_statement;

    if (p_selection_statement->secondary_block)
        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);


    check_defer_and_variables(ctx, p_defer, p_selection_statement->secondary_block->last_token);


    //POP
    ctx->tail_block = ctx->tail_block->previous;

    if (p_selection_statement->else_secondary_block_opt)
        flow_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);


}

static void flow_visit_compound_statement(struct flow_analysis_visit_ctx* ctx, struct compound_statement* p_compound_statement);

static void flow_visit_initializer_list(struct flow_analysis_visit_ctx* ctx, struct initializer_list* p_initializer_list);

static void flow_visit_bracket_initializer_list(struct flow_analysis_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {

    }
    else
    {
        flow_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void flow_visit_designation(struct flow_analysis_visit_ctx* ctx, struct designation* p_designation)
{
}

static void flow_visit_initializer(struct flow_analysis_visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->p_attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_initializer->p_attribute_specifier_sequence_opt);
    }

    if (p_initializer->designation)
    {
        flow_visit_designation(ctx, p_initializer->designation);
    }

    if (p_initializer->assignment_expression)
    {
        flow_visit_expression(ctx, p_initializer->assignment_expression);
    }
    else if (p_initializer->braced_initializer)
    {
        flow_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void flow_visit_initializer_list(struct flow_analysis_visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer* p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow_visit_initializer(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow_visit_type_qualifier(struct flow_analysis_visit_ctx* ctx, struct type_qualifier* p_type_qualifier)
{

}

static void flow_visit_specifier_qualifier(struct flow_analysis_visit_ctx* ctx, struct type_specifier_qualifier* p_specifier_qualifier)
{
    if (p_specifier_qualifier->type_specifier)
        flow_visit_type_specifier(ctx, p_specifier_qualifier->type_specifier);

    if (p_specifier_qualifier->type_qualifier)
        flow_visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier);
}

static void flow_visit_specifier_qualifier_list(struct flow_analysis_visit_ctx* ctx, struct specifier_qualifier_list* p_specifier_qualifier_list_opt,
    struct type* p_type)
{

    //(typeof(int[2])*)
    // 
    //TODO se tiver typeof em qualquer parte tem que imprimir todo  tipo
    // tem que refazer
    if (p_specifier_qualifier_list_opt->type_specifier_flags & TYPE_SPECIFIER_TYPEOF)
    {
        token_range_add_flag(p_specifier_qualifier_list_opt->first_token,
            p_specifier_qualifier_list_opt->last_token, TK_FLAG_HIDE);

        struct osstream ss = {0};
        print_type(&ss, type_get_specifer_part(p_type));

        struct tokenizer_ctx tctx = {0};
        struct token_list l2 = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, p_specifier_qualifier_list_opt->last_token, &l2);

        ss_close(&ss);
    }

    if (p_specifier_qualifier_list_opt == NULL)
        return;

    if (p_specifier_qualifier_list_opt->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_specifier_qualifier_list_opt->struct_or_union_specifier);
    }
    else if (p_specifier_qualifier_list_opt->enum_specifier)
    {
        flow_visit_enum_specifier(ctx, p_specifier_qualifier_list_opt->enum_specifier);
    }
    else if (p_specifier_qualifier_list_opt->typedef_declarator)
    {
        //typedef name
    }
    //else if (p_specifier_qualifier_list->p_typeof_expression_opt)
    //{
      //  flow_visit_expression(ctx, p_specifier_qualifier_list->p_typeof_expression_opt);
    //}
    else
    {
        struct type_specifier_qualifier* p_specifier_qualifier = p_specifier_qualifier_list_opt->head;
        while (p_specifier_qualifier)
        {
            flow_visit_specifier_qualifier(ctx, p_specifier_qualifier);
            p_specifier_qualifier = p_specifier_qualifier->next;
        }
    }
}
static void flow_visit_declarator(struct flow_analysis_visit_ctx* ctx, struct declarator* p_declarator);
static void flow_visit_type_name(struct flow_analysis_visit_ctx* ctx, struct type_name* p_type_name)
{

    flow_visit_specifier_qualifier_list(ctx, p_type_name->specifier_qualifier_list, &p_type_name->type);
    flow_visit_declarator(ctx, p_type_name->declarator);


    /*
    * Vamos esconder tudo e gerar um novo
    *  Exemplo
    *  (const typeof(int (*)())) -> *  ( int (*const )() )
    */
}



static void flow_visit_argument_expression_list(struct flow_analysis_visit_ctx* ctx, struct argument_expression_list* p_argument_expression_list)
{
    struct argument_expression* p_argument_expression =
        p_argument_expression_list->head;
    while (p_argument_expression)
    {
        flow_visit_expression(ctx, p_argument_expression->expression);
        p_argument_expression = p_argument_expression->next;
    }
}

static void flow_visit_generic_selection(struct flow_analysis_visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        flow_visit_expression(ctx, p_generic_selection->expression);
    }
    else if (p_generic_selection->type_name)
    {
        flow_visit_type_name(ctx, p_generic_selection->type_name);
    }



}

static int compare_function_arguments2(struct parser_ctx* ctx,
    struct type* p_type,
    struct argument_expression_list* p_argument_expression_list)
{
    try
    {
        struct param* p_current_parameter_type = NULL;

        const struct param_list* p_param_list = type_get_func_or_func_ptr_params(p_type);

        if (p_param_list)
        {
            p_current_parameter_type = p_param_list->head;
        }

        int param_num = 1;
        struct argument_expression* p_current_argument = p_argument_expression_list->head;

        while (p_current_argument && p_current_parameter_type)
        {
            //check_function_argument_and_parameter(ctx, p_current_argument, p_current_parameter_type->type, param_num);




            if (p_current_parameter_type->type->type_qualifier_flags & TYPE_QUALIFIER_OWNER ||
                p_current_parameter_type->type->type_qualifier_flags & TYPE_QUALIFIER_OBJ_OWNER)
            {

                struct declarator* const p_argument_declarator =
                    expression_get_declarator(p_current_argument->expression);
                /*
                *

                (type_has_attribute(p_current_parameter_type->type->next, CAKE_ATTRIBUTE_DESTROY) ||
                    type_has_attribute(p_current_parameter_type->type->next, CAKE_ATTRIBUTE_FREE) ||
                    type_has_attribute(p_current_parameter_type->type->next, CAKE_ATTRIBUTE_MOVE))
                )
                */

                /*
                  design must decide if we want to destroy partially, like independent
                  destroy and free.
                */
                if (p_argument_declarator)
                    p_argument_declarator->declarator_flags = DECLARATOR_UNINITIALIZED;
            }



            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
            param_num++;
        }

        //if (p_current_argument != NULL && !p_param_list->is_var_args)
        //{
          //  compiler_set_error_with_token(C_TOO_MANY_ARGUMENTS, ctx,
            //    p_argument_expression_list->tail->expression->first_token,
              //  "too many arguments");
            //throw;
        //}

        //if (p_current_parameter_type != NULL && !p_param_list->is_void)
        //{
          //  if (p_argument_expression_list->tail)
            //{
              //  compiler_set_error_with_token(C_TOO_FEW_ARGUMENTS, ctx,
                //    p_argument_expression_list->tail->expression->first_token,
                  //  "too few arguments");
            //}
            //else
            //{
              //  compiler_set_error_with_token(C_TOO_FEW_ARGUMENTS, ctx, ctx->current, "too few arguments");
            //}
            //throw;
        //}
    }
    catch
    {
        return 1; /*error*/
    }

    return 0;
}

static void flow_visit_expression(struct flow_analysis_visit_ctx* ctx, struct expression* p_expression)
{
    switch (p_expression->expression_type)
    {
        case PRIMARY_EXPRESSION__FUNC__:
            break;
        case PRIMARY_IDENTIFIER:
            break;
        case PRIMARY_EXPRESSION_ENUMERATOR:

            break;
        case PRIMARY_EXPRESSION_DECLARATOR:




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
            flow_visit_generic_selection(ctx, p_expression->generic_selection);
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
            flow_visit_expression(ctx, p_expression->left);
            flow_visit_argument_expression_list(ctx, &p_expression->argument_expression_list);
            compare_function_arguments2(ctx->ctx, &p_expression->left->type, &p_expression->argument_expression_list);
            break;
        case POSTFIX_EXPRESSION_FUNCTION_LITERAL:


            flow_visit_compound_statement(ctx, p_expression->compound_statement);


            break;

        case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

            if (p_expression->type_name)
            {
                flow_visit_type_name(ctx, p_expression->type_name);
            }

            flow_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

            assert(p_expression->left == NULL);
            assert(p_expression->right == NULL);

            break;

        case UNARY_EXPRESSION_ALIGNOF:

            if (p_expression->right)
            {
                flow_visit_expression(ctx, p_expression->right);
            }

            if (p_expression->type_name)
            {
                /*sizeof*/
                flow_visit_type_name(ctx, p_expression->type_name);
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
                flow_visit_expression(ctx, p_expression->right);
            }

            if (p_expression->type_name)
            {
                /*sizeof*/
                flow_visit_type_name(ctx, p_expression->type_name);
            }

            break;

        case UNARY_EXPRESSION_HASHOF_TYPE:
            break;


        case ASSIGNMENT_EXPRESSION:

            if (p_expression->move_assignment)
            {
                struct declarator* const p_left_declarator = expression_get_declarator(p_expression->left);
                struct declarator* const p_right_declarator = expression_get_declarator(p_expression->right);
                if (p_left_declarator && p_right_declarator)
                {
                    p_left_declarator->declarator_flags = p_right_declarator->declarator_flags;
                    p_right_declarator->declarator_flags = DECLARATOR_UNINITIALIZED;
                }
            }
            flow_visit_expression(ctx, p_expression->left);
            flow_visit_expression(ctx, p_expression->right);
            break;

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
                flow_visit_expression(ctx, p_expression->left);
            }
            if (p_expression->right)
            {
                flow_visit_expression(ctx, p_expression->right);
            }
            if (p_expression->type_name)
            {
                flow_visit_type_name(ctx, p_expression->type_name);
            }
            break;

        case UNARY_EXPRESSION_TRAITS:
        {

        }
        break;

        case UNARY_EXPRESSION_IS_SAME:
            break;

        case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
            break;

        case CONDITIONAL_EXPRESSION:
            if (p_expression->condition_expr)
            {
                flow_visit_expression(ctx, p_expression->condition_expr);
            }

            if (p_expression->left)
            {
                flow_visit_expression(ctx, p_expression->left);
            }
            if (p_expression->right)
            {
                flow_visit_expression(ctx, p_expression->right);
            }

            break;

        default:
            break;
    }
}

static void flow_visit_expression_statement(struct flow_analysis_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
        flow_visit_expression(ctx, p_expression_statement->expression_opt);
}

static void flow_visit_block_item_list(struct flow_analysis_visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void flow_visit_compound_statement(struct flow_analysis_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    flow_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
}

//
static void flow_visit_iteration_statement(struct flow_analysis_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{

    if (p_iteration_statement->expression1)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->expression2)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression2);
    }

    if (p_iteration_statement->first_token->type == TK_KEYWORD_REPEAT)
    {
        free(p_iteration_statement->first_token->lexeme);
        p_iteration_statement->first_token->lexeme = strdup("for(;;)/*repeat*/");
    }

    if (p_iteration_statement->secondary_block)
    {
        struct flow_defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->previous = ctx->tail_block;
        ctx->tail_block = p_defer;
        p_defer->p_iteration_statement = p_iteration_statement;

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);




        check_defer_and_variables(ctx, p_defer, p_iteration_statement->secondary_block->last_token);


        if (ctx->tail_block)
        {
            //POP
            ctx->tail_block = ctx->tail_block->previous;
        }


    }
}



static void flow_visit_jump_statement(struct flow_analysis_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{

    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
    {
        check_all_defer_until_try(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        /*
          returning a declarator will move the onwership
        */
        if (p_jump_statement->expression_opt &&
            p_jump_statement->expression_opt->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
        {
            /*
               returning a declarator will remove the flags must destroy or must free,
               similar of moving
            */
            p_jump_statement->expression_opt->declarator->declarator_flags &= ~(DECLARATOR_MUST_DESTROY | DECLARATOR_MUST_FREE);
        }



        check_all_defer_until_end(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {
        check_all_defer_until_iter(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {
        check_all_defer_until_label(ctx, ctx->tail_block, p_jump_statement->label->lexeme, p_jump_statement->first_token);
    }
    else
    {
        assert(false);
    }
}


static void flow_visit_labeled_statement(struct flow_analysis_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    //p_labeled_statement->label

    if (p_labeled_statement->statement)
    {
        flow_visit_statement(ctx, p_labeled_statement->statement);
    }
}

static void flow_visit_primary_block(struct flow_analysis_visit_ctx* ctx, struct primary_block* p_primary_block)
{



    if (p_primary_block->defer_statement)
    {
        flow_visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            flow_visit_compound_statement(ctx, p_primary_block->compound_statement);
        }
        else if (p_primary_block->iteration_statement)
        {
            flow_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
        }
        else if (p_primary_block->selection_statement)
        {
            flow_visit_selection_statement(ctx, p_primary_block->selection_statement);
        }
        else if (p_primary_block->try_statement)
        {
            flow_visit_try_statement(ctx, p_primary_block->try_statement);
        }
    }

}

static void flow_visit_unlabeled_statement(struct flow_analysis_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        flow_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        flow_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        flow_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}


static void flow_visit_statement(struct flow_analysis_visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        flow_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        flow_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void flow_visit_label(struct flow_analysis_visit_ctx* ctx, struct label* p_label)
{
    //p_label->name
}
static void flow_visit_block_item(struct flow_analysis_visit_ctx* ctx, struct block_item* p_block_item)
{
    if (p_block_item->declaration)
    {
        flow_analysis_visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        flow_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        flow_visit_label(ctx, p_block_item->label);
    }

}

static void flow_visit_block_item_list(struct flow_analysis_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void flow_visit_static_assert_declaration(struct flow_analysis_visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration)
{
    flow_visit_expression(ctx, p_static_assert_declaration->constant_expression);
}

static void flow_visit_declaration_specifiers(struct flow_analysis_visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* p_type);


static void flow_visit_direct_declarator(struct flow_analysis_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
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
            if (parameter->attribute_specifier_sequence_opt)
            {
                flow_visit_attribute_specifier_sequence(ctx, parameter->attribute_specifier_sequence_opt);
            }

            flow_visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);
            flow_visit_declarator(ctx, parameter->declarator);
            parameter = parameter->next;
        }

    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            flow_visit_expression(ctx, p_direct_declarator->array_declarator->assignment_expression);
        }

    }
}

static void flow_visit_declarator(struct flow_analysis_visit_ctx* ctx, struct declarator* p_declarator)
{
    if (ctx->tail_block)
    {
        if (p_declarator->name != NULL)
        {
            struct flow_defer_scope* p_defer = calloc(1, sizeof * p_defer);
            p_defer->declarator = p_declarator;
            p_defer->previous = ctx->tail_block->lastchild;
            ctx->tail_block->lastchild = p_defer;
        }
        else
        {
            //can be a visit in type name abstract declarator
            //TODO better way to check?
        }
    }

    if (p_declarator->pointer)
    {
        struct pointer* p = p_declarator->pointer;
        while (p)
        {
            p = p->pointer;
        }
    }

    if (p_declarator->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER)
    {
        p_declarator->declarator_flags = DECLARATOR_MUST_DESTROY | DECLARATOR_ISVALID;
    }


    if (p_declarator->direct_declarator)
    {
        flow_visit_direct_declarator(ctx, p_declarator->direct_declarator);
    }
}

static void flow_visit_init_declarator_list(struct flow_analysis_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* p_init_declarator = p_init_declarator_list->head;



    while (p_init_declarator)
    {
        if (p_init_declarator->p_declarator)
        {
            flow_visit_declarator(ctx, p_init_declarator->p_declarator);
        }

        if (p_init_declarator->initializer)
        {

            if (p_init_declarator->initializer->assignment_expression)
            {

                if (p_init_declarator->p_declarator->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER)
                {
                    if (type_is_pointer(&p_init_declarator->p_declarator->type))
                    {
                        p_init_declarator->p_declarator->declarator_flags |= DECLARATOR_MUST_FREE;

                        p_init_declarator->p_declarator->declarator_flags |= DECLARATOR_MUST_DESTROY;
                    }
                    else
                    {
                        p_init_declarator->p_declarator->declarator_flags |= DECLARATOR_MUST_DESTROY;
                    }
                }

                //if (p_init_declarator->initializer->assignment_expression->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER)
                //{
                  //  p_init_declarator->p_declarator->declarator_flags |= DECLARATOR_MUST_FREE;
                //}


                if (p_init_declarator->initializer->move_token)
                {
                    struct declarator* const p_right_declarator = expression_get_declarator(p_init_declarator->initializer->assignment_expression);
                    if (p_right_declarator)
                    {
                        p_init_declarator->p_declarator->declarator_flags = p_right_declarator->declarator_flags;
                        p_right_declarator->declarator_flags = DECLARATOR_UNINITIALIZED;
                    }
                }

                flow_visit_expression(ctx, p_init_declarator->initializer->assignment_expression);
            }
            else
            {
                assert(p_init_declarator->initializer->braced_initializer != NULL);
                if (p_init_declarator->initializer->braced_initializer)
                {
                    flow_visit_bracket_initializer_list(ctx,
                        p_init_declarator->initializer->braced_initializer);
                }

            }
        }

        p_init_declarator = p_init_declarator->next;
    }
}



static void flow_visit_member_declarator(struct flow_analysis_visit_ctx* ctx, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        flow_visit_declarator(ctx, p_member_declarator->declarator);
    }
}

static void flow_visit_member_declarator_list(struct flow_analysis_visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        flow_visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}

static void flow_visit_member_declaration(struct flow_analysis_visit_ctx* ctx, struct member_declaration* p_member_declaration)
{
    if (p_member_declaration->member_declarator_list_opt)
    {
        flow_visit_specifier_qualifier_list(ctx,
            p_member_declaration->specifier_qualifier_list,
            &p_member_declaration->member_declarator_list_opt->head->declarator->type); /*se nao tem?*/
    }

    if (p_member_declaration->member_declarator_list_opt)
    {
        flow_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void flow_visit_member_declaration_list(struct flow_analysis_visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* p_member_declaration =
        p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow_visit_attribute_specifier(struct flow_analysis_visit_ctx* ctx, struct attribute_specifier* p_attribute_specifier)
{

}

static void flow_visit_attribute_specifier_sequence(struct flow_analysis_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence)
{
    struct attribute_specifier* current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        flow_visit_attribute_specifier(ctx, current);
        current = current->next;
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_analysis_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{

    if (p_struct_or_union_specifier->attribute_specifier_sequence_opt)
        flow_visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence_opt);

    //struct struct_or_union_specifier* p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);


    flow_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);

}

static void flow_visit_enumerator(struct flow_analysis_visit_ctx* ctx, struct enumerator* p_enumerator)
{
    if (p_enumerator->constant_expression_opt)
        flow_visit_expression(ctx, p_enumerator->constant_expression_opt);

}

//struct enumerator_list* enumerator_list;
static void flow_visit_enumerator_list(struct flow_analysis_visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* current = p_enumerator_list->head;
    while (current)
    {
        flow_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow_visit_enum_specifier(struct flow_analysis_visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    if (p_enum_specifier->attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_enum_specifier->attribute_specifier_sequence_opt);
    }

    flow_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void flow_visit_typeof_specifier(struct flow_analysis_visit_ctx* ctx, struct typeof_specifier* p_typeof_specifier)
{
}

static void flow_visit_type_specifier(struct flow_analysis_visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->typeof_specifier)
    {
        flow_visit_typeof_specifier(ctx, p_type_specifier->typeof_specifier);
    }

    if (p_type_specifier->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        flow_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }


    if (p_type_specifier->atomic_type_specifier)
    {
        //visit_deped(ctx, p_type_specifier->enum_specifier);
    }

}

static void flow_visit_type_specifier_qualifier(struct flow_analysis_visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_qualifier)
    {
    }
    else if (p_type_specifier_qualifier->type_specifier)
    {
        flow_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
    else if (p_type_specifier_qualifier->alignment_specifier)
    {
    }
}

static void flow_visit_storage_class_specifier(struct flow_analysis_visit_ctx* ctx, struct storage_class_specifier* p_storage_class_specifier)
{
    if (p_storage_class_specifier->flags & STORAGE_SPECIFIER_AUTO)
    {

    }
}

static void flow_visit_declaration_specifier(struct flow_analysis_visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{

    if (p_declaration_specifier->function_specifier)
    {

    }


    if (p_declaration_specifier->storage_class_specifier)
    {
        flow_visit_storage_class_specifier(ctx, p_declaration_specifier->storage_class_specifier);

    }

    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);

    }

}

static void flow_visit_declaration_specifiers(struct flow_analysis_visit_ctx* ctx,
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

    struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;

    struct declaration_specifier* p_constexpr_declaration_specifier = NULL;
    while (p_declaration_specifier)
    {
        if (p_declaration_specifier->storage_class_specifier &&
            p_declaration_specifier->storage_class_specifier->flags & STORAGE_SPECIFIER_CONSTEXPR)
        {
            p_constexpr_declaration_specifier = p_declaration_specifier;
        }

        flow_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}

/*
* retorna true se o ultimo item for um return
*/
static bool flow_is_last_item_return(struct compound_statement* p_compound_statement)
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

void flow_analysis_visit_declaration(struct flow_analysis_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        flow_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_declaration->p_attribute_specifier_sequence_opt);
    }


    if (p_declaration->declaration_specifiers)
    {
        if (p_declaration->init_declarator_list.head)
        {
            flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers,
                &p_declaration->init_declarator_list.head->p_declarator->type);
        }
        else
        {
            flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, NULL);

        }

    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {

    }


    if (p_declaration->init_declarator_list.head)
    {
        flow_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {


        struct flow_defer_scope* p_defer = calloc(1, sizeof * p_defer);
        ctx->tail_block = p_defer;
        p_defer->p_function_body = p_declaration->function_body;

        flow_visit_compound_statement(ctx, p_declaration->function_body);

        if (!flow_is_last_item_return(p_declaration->function_body))
        {
            check_defer_and_variables(ctx, p_defer, p_declaration->function_body->last_token);
        }
        else
        {


        }


        ctx->tail_block = NULL;



    }
}

void flow_analysis_visit(struct flow_analysis_visit_ctx* ctx)
{
    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        flow_analysis_visit_declaration(ctx, p_declaration);
        p_declaration = p_declaration->next;
    }
}

