#include "ownership.h"

#include <assert.h>
#include <string.h>
#include <assert.h>
#include "flow_visit.h"
#include "expressions.h"
#include "ownership.h"
#include <ctype.h>
#include <stdlib.h>


/*
              NULL
                |
            [function] (NULL <- child0 <- last_child)
                |
              [try]  (NULL <- child0 <- last_child)
                |
tail_block-> [while] (NULL <- child0 <- last_child)

*/

struct flow_defer_scope
{

    //things must called at end of scope
    struct declarator* declarator; // declarator
    struct defer_statement* defer_statement; // defer

    //statements for controling where jump like break, throw stop.

    struct try_statement* p_try_statement; //try
    struct selection_statement* p_selection_statement; //if switch
    struct iteration_statement* p_iteration_statement; //for do while
    struct statement* p_statement;//
    struct compound_statement* p_compound_statement;
    struct compound_statement* p_function_body;
    struct secondary_block* p_secondary_block;
    struct primary_block* p_primary_block;

    struct flow_defer_scope* owner last_child; /**/
    struct flow_defer_scope* owner previous;
};

void flow_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration);
static void flow_visit_attribute_specifier_sequence(struct flow_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence);

static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct secondary_block* p_secondary_block);
static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression);
static void flow_visit_statement(struct flow_visit_ctx* ctx, struct statement* p_statement);
static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct type_specifier* p_type_specifier);
static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);
static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct expression_statement* p_expression_statement);

enum boolean_flag
{
    BOOLEAN_FLAG_TRUE = 1 << 0,
    BOOLEAN_FLAG_FALSE = 1 << 1,
};

struct declarator_array_item
{
    struct expression* p_expression;
    enum boolean_flag true_branch_state;
    enum boolean_flag false_branch_state;
};


struct declarator_array
{
    //see also
    //  declarator_array_set_objects_to_false_branch
    //  declarator_array_set_objects_to_true_branch
    // 
    //The number of variables determines the possible number of combinations.
    //10 variables requires 2^10 = 1024 evaluations.
    struct declarator_array_item data[10];
    int n;
};

static void declarator_array_set_objects_to_true_branch(struct flow_visit_ctx* ctx, struct declarator_array* a, bool nullable_enabled)
{
    for (int i = 0; i < a->n; i++)
    {
        if (a->data[i].p_expression != NULL)
        {
            struct object temp = { 0 };
            struct object* p_object =
                expression_get_object(a->data[i].p_expression, &temp, nullable_enabled);
            if (p_object)
            {
                const bool is_pointer = type_is_pointer(&a->data[i].p_expression->type);
                const enum boolean_flag flag = a->data[i].true_branch_state;
#if 0
                //TODO this test cannot be here..it must be only were variable is used as bool
                if (p == p2) // p is always not null does not help here!
                    if (p == NULL) // but it helps here...move to expression

                        if (is_pointer)
                        {
                            if (is_null(p_object2->state))
                            {
                                compiler_diagnostic_message(W_NON_NULL,
                                        ctx->ctx,
                                        a->data[i].p_expression->first_token,
                                        "expression is always null");
                                continue;
                            }
                            else if (is_not_null(p_object2->state))
                            {
                                compiler_diagnostic_message(W_NON_NULL,
                                        ctx->ctx,
                                        a->data[i].p_expression->first_token,
                                        "expression is always not-null");
                                continue;
                            }
                        }
#endif

                if ((flag & BOOLEAN_FLAG_TRUE) && (flag & BOOLEAN_FLAG_FALSE)
                    )
                {
                    if (is_pointer)
                        p_object->state |= (OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL);
                    else
                        p_object->state |= (OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO);
                }
                else if (flag & BOOLEAN_FLAG_FALSE)
                {
                    if (is_pointer)
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_NOT_NULL;
                        p_object->state |= OBJECT_STATE_NULL;
                    }
                    else
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_NOT_ZERO;
                        p_object->state |= OBJECT_STATE_ZERO;
                    }
                }
                else if (flag & BOOLEAN_FLAG_TRUE)
                {
                    if (is_pointer)
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_NULL;
                        p_object->state |= OBJECT_STATE_NOT_NULL;
                    }
                    else
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_ZERO;
                        p_object->state |= OBJECT_STATE_NOT_ZERO;
                    }
                }
            }
            object_destroy(&temp);
        }
    }
}

static void declarator_array_set_objects_to_false_branch(struct flow_visit_ctx* ctx, struct declarator_array* a, bool nullable_enabled)
{
    for (int i = 0; i < a->n; i++)
    {
        if (a->data[i].p_expression != NULL)
        {
            struct object temp = { 0 };
            struct object* p_object =
                expression_get_object(a->data[i].p_expression, &temp, nullable_enabled);
            if (p_object)
            {
                const bool is_pointer = type_is_pointer(&a->data[i].p_expression->type);
                const enum boolean_flag flag = a->data[i].false_branch_state;



                if ((flag & BOOLEAN_FLAG_TRUE) && (flag & BOOLEAN_FLAG_FALSE))
                {
                    if (is_pointer)
                        p_object->state |= (OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL);
                    else
                        p_object->state |= (OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO);
                }
                else if (flag & BOOLEAN_FLAG_FALSE)
                {
                    if (is_pointer)
                    {

                        p_object->state = p_object->state & ~OBJECT_STATE_NOT_NULL;
                        p_object->state |= OBJECT_STATE_NULL;

                    }
                    else
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_NOT_ZERO;
                        p_object->state |= OBJECT_STATE_ZERO;
                    }
                }
                else if (flag & BOOLEAN_FLAG_TRUE)
                {
                    if (is_pointer)
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_NULL;
                        p_object->state |= OBJECT_STATE_NOT_NULL;
                    }
                    else
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_ZERO;
                        p_object->state |= OBJECT_STATE_NOT_ZERO;
                    }
                }


            }
            object_destroy(&temp);
        }
    }
}


static void flow_visit_expression_to_collect_objects(struct expression* expression, struct declarator_array* a)
{
    if (expression == NULL)
        return;
    /*
       The objective of this function is collect expressions that refers to objects
       The result of these expression will be simulated.
    */

    if (constant_value_is_valid(&expression->constant_value))
    {
        //constant expression don't need to be simulated...they already have a value
        return;
    }

    switch (expression->expression_type)
    {
    case PRIMARY_IDENTIFIER:break;
    case PRIMARY_EXPRESSION_ENUMERATOR:break;

    case PRIMARY_EXPRESSION_DECLARATOR:

        if (expression->declarator)
        {
            // if (is_null(expression->declarator->object.state) ||
              //   is_zero(expression->declarator->object.state))
             //{
                 //during the simulated evaluation ()
               //  expression->value_emulation = 0;
             //}
             //else if (is_not_null(expression->declarator->object.state) ||
                  //    is_not_zero(expression->declarator->object.state))
             //{
               //  expression->value_emulation = 1;
             //}
             //else
            {
                if (a->n == sizeof(a->data) / sizeof(a->data[0]) - 1)
                {
                    return /*error*/;
                }

                bool found = false;
                for (int i = 0; i < a->n; i++)
                {
                    if (a->data[a->n].p_expression == expression)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    a->data[a->n].p_expression = expression;
                    a->n++;
                }

                //we simulate true  and false null only if the object
                //can be both

            }
        }
        break;


    case PRIMARY_EXPRESSION_STRING_LITERAL: break;
    case PRIMARY_EXPRESSION__FUNC__: break;

    case PRIMARY_EXPRESSION_CHAR_LITERAL:
        break;

    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        break;
    case PRIMARY_EXPRESSION_NUMBER:
        break;
    case PRIMARY_EXPRESSION_PARENTESIS:
        break;

    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
        break;
    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:
        break;

    case POSTFIX_FUNCTION_CALL:
        break;// ( ) :break;

    case POSTFIX_ARRAY:
    case POSTFIX_DOT:
    case POSTFIX_ARROW:
        //arrow expression are emulated as one object
        //a->b->c->d
        if (a->n == sizeof(a->data) / sizeof(a->data[0]) - 1)
        {
            return /*error*/;
        }

        assert(a->n < 10);
        bool found = false;
        for (int i = 0; i < a->n; i++)
        {
            if (a->data[a->n].p_expression == expression)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            a->data[a->n].p_expression = expression;
            a->n++;
        }
        break;// .

    case POSTFIX_INCREMENT:
        break;
    case POSTFIX_DECREMENT:
        break;


    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:
    case UNARY_EXPRESSION_SIZEOF_TYPE:
    case UNARY_EXPRESSION_TRAITS:
    case UNARY_EXPRESSION_IS_SAME:
    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
    case UNARY_EXPRESSION_ALIGNOF:
    case UNARY_EXPRESSION_ASSERT:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:
    case UNARY_EXPRESSION_NOT:
    case UNARY_EXPRESSION_BITNOT:
    case UNARY_EXPRESSION_NEG:
    case UNARY_EXPRESSION_PLUS:
    case UNARY_EXPRESSION_CONTENT:
    case UNARY_EXPRESSION_ADDRESSOF:
        flow_visit_expression_to_collect_objects(expression->left, a);
        break;


    case CAST_EXPRESSION:
        break;

    case MULTIPLICATIVE_EXPRESSION_MULT:
    case MULTIPLICATIVE_EXPRESSION_DIV:
    case MULTIPLICATIVE_EXPRESSION_MOD:break;

    case ADDITIVE_EXPRESSION_PLUS:
    case ADDITIVE_EXPRESSION_MINUS:
    case SHIFT_EXPRESSION_RIGHT:
    case SHIFT_EXPRESSION_LEFT:
    case RELATIONAL_EXPRESSION_BIGGER_THAN:
    case RELATIONAL_EXPRESSION_LESS_THAN:
    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:

    case EQUALITY_EXPRESSION_NOT_EQUAL:
    case AND_EXPRESSION:
    case EXCLUSIVE_OR_EXPRESSION:
    case INCLUSIVE_OR_EXPRESSION:

    case LOGICAL_AND_EXPRESSION:
    case LOGICAL_OR_EXPRESSION:
    case ASSIGNMENT_EXPRESSION:
    case EQUALITY_EXPRESSION_EQUAL:
        flow_visit_expression_to_collect_objects(expression->left, a);
        flow_visit_expression_to_collect_objects(expression->right, a);
        break;

    case CONDITIONAL_EXPRESSION:break;

    }
    //assert(false);
}

static int flow_run_simulated_evaluation(struct expression* expression, int* error)
{
    if (constant_value_is_valid(&expression->constant_value))
    {
        return constant_value_to_bool(&expression->constant_value);
    }

    switch (expression->expression_type)
    {
    case PRIMARY_IDENTIFIER:break;
    case PRIMARY_EXPRESSION_ENUMERATOR:break;
    case PRIMARY_EXPRESSION_DECLARATOR:
    {
        return expression->value_emulation;
    }
    case PRIMARY_EXPRESSION_STRING_LITERAL: return true;
    case PRIMARY_EXPRESSION__FUNC__: return true;

    case PRIMARY_EXPRESSION_CHAR_LITERAL:
        return constant_value_to_bool(&expression->constant_value);

    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        return constant_value_to_bool(&expression->constant_value);

    case PRIMARY_EXPRESSION_GENERIC:
        break;
    case PRIMARY_EXPRESSION_NUMBER:
        return constant_value_to_bool(&expression->constant_value);
        break;
    case PRIMARY_EXPRESSION_PARENTESIS:
        break;

    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
        break;
    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:
        break;

    case POSTFIX_FUNCTION_CALL:

        // if (f() && p) { }
        return expression->value_emulation;

        //break;// ( ) :break;
    case POSTFIX_ARRAY:
        break;// [ ]:break;
    case POSTFIX_DOT:
    case POSTFIX_ARROW:
        //arrow expressions are emulated all in one
        //a->b->c->d becomes one result
        return expression->value_emulation;

    case POSTFIX_INCREMENT:
        break;
    case POSTFIX_DECREMENT:
        break;


    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:
        break;
    case UNARY_EXPRESSION_SIZEOF_TYPE:
        break;

    case UNARY_EXPRESSION_TRAITS:
        break;
    case UNARY_EXPRESSION_IS_SAME:
        break;
    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
        break;
    case UNARY_EXPRESSION_ALIGNOF:break;
    case UNARY_EXPRESSION_ASSERT:break;

    case UNARY_EXPRESSION_INCREMENT:break;
    case UNARY_EXPRESSION_DECREMENT:break;

    case UNARY_EXPRESSION_NOT:
        return !flow_run_simulated_evaluation(expression->right, error);


    case UNARY_EXPRESSION_BITNOT:break;
    case UNARY_EXPRESSION_NEG:break;
    case UNARY_EXPRESSION_PLUS:break;
    case UNARY_EXPRESSION_CONTENT:
        break;

    case UNARY_EXPRESSION_ADDRESSOF:
        return true;


    case CAST_EXPRESSION:
        break;

    case MULTIPLICATIVE_EXPRESSION_MULT:
        return
            flow_run_simulated_evaluation(expression->left, error) *
            flow_run_simulated_evaluation(expression->right, error);

    case MULTIPLICATIVE_EXPRESSION_DIV:
        return
            flow_run_simulated_evaluation(expression->left, error) /
            flow_run_simulated_evaluation(expression->right, error);

    case MULTIPLICATIVE_EXPRESSION_MOD:break;

    case ADDITIVE_EXPRESSION_PLUS:
        return
            flow_run_simulated_evaluation(expression->left, error) +
            flow_run_simulated_evaluation(expression->right, error);

    case ADDITIVE_EXPRESSION_MINUS:
        return
            flow_run_simulated_evaluation(expression->left, error) -
            flow_run_simulated_evaluation(expression->right, error);

    case SHIFT_EXPRESSION_RIGHT:
        return
            flow_run_simulated_evaluation(expression->left, error) >>
            flow_run_simulated_evaluation(expression->right, error);

    case SHIFT_EXPRESSION_LEFT:
        return
            flow_run_simulated_evaluation(expression->left, error) <<
            flow_run_simulated_evaluation(expression->right, error);

    case RELATIONAL_EXPRESSION_BIGGER_THAN:
        return
            flow_run_simulated_evaluation(expression->left, error) >
            flow_run_simulated_evaluation(expression->right, error);

    case RELATIONAL_EXPRESSION_LESS_THAN:
        return
            flow_run_simulated_evaluation(expression->left, error) <
            flow_run_simulated_evaluation(expression->right, error);

    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
        return
            flow_run_simulated_evaluation(expression->left, error) >=
            flow_run_simulated_evaluation(expression->right, error);

    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
        return
            flow_run_simulated_evaluation(expression->left, error) <=
            flow_run_simulated_evaluation(expression->right, error);

    case EQUALITY_EXPRESSION_EQUAL:
        return
            flow_run_simulated_evaluation(expression->left, error) ==
            flow_run_simulated_evaluation(expression->right, error);

    case EQUALITY_EXPRESSION_NOT_EQUAL:
        return
            flow_run_simulated_evaluation(expression->left, error) !=
            flow_run_simulated_evaluation(expression->right, error);

    case AND_EXPRESSION:

        return
            flow_run_simulated_evaluation(expression->left, error) &
            flow_run_simulated_evaluation(expression->right, error);


    case EXCLUSIVE_OR_EXPRESSION:
        return
            flow_run_simulated_evaluation(expression->left, error) |
            flow_run_simulated_evaluation(expression->right, error);

    case INCLUSIVE_OR_EXPRESSION:
        return
            flow_run_simulated_evaluation(expression->left, error) ||
            flow_run_simulated_evaluation(expression->right, error);



    case LOGICAL_AND_EXPRESSION:
        return
            flow_run_simulated_evaluation(expression->left, error) &&
            flow_run_simulated_evaluation(expression->right, error);

    case LOGICAL_OR_EXPRESSION:break;
    case ASSIGNMENT_EXPRESSION:break;

    case CONDITIONAL_EXPRESSION:break;

    }
    *error = 1;
    //assert(false);
    return true;
}

static int flow_simulated_evaluation(int k, struct expression* expression, struct declarator_array* a, int* error)
{

    if (k == a->n)
    {
        //we have a combination, now we check the evaluation with these variables
        bool r = flow_run_simulated_evaluation(expression, error);
        if (*error != 0)
            return *error;

        for (int i = 0; i < a->n; i++)
        {
            if (r)
            {
                a->data[i].true_branch_state |=
                    (a->data[i].p_expression->value_emulation ? BOOLEAN_FLAG_TRUE : BOOLEAN_FLAG_FALSE);
            }
            else
            {
                a->data[i].false_branch_state |=
                    (a->data[i].p_expression->value_emulation ? BOOLEAN_FLAG_TRUE : BOOLEAN_FLAG_FALSE);
            }
        }

        return 0;
    }

    a->data[k].p_expression->value_emulation = 1;
    flow_simulated_evaluation(k + 1, expression, a, error);
    if (*error != 0)
        return *error;

    a->data[k].p_expression->value_emulation = 0;
    flow_simulated_evaluation(k + 1, expression, a, error);
    if (*error != 0)
        return *error;

    return 0;
}

/*
   This function computes the possible values for the expression to be true or false.
   Anwser is stored at declarator_array
*/
static int compute_true_false_sets(struct expression* expression, struct declarator_array* a)
{
    int error = 0;
    assert(a->n == 0);

    //We collect expressions that may be null/not-null or zero/not-zero    
    flow_visit_expression_to_collect_objects(expression, a);

    flow_simulated_evaluation(0, expression, a, &error);

    if (error != 0)
    {
        //TODO we can assume worst scenario
         // Sample f() > 2.0 is not possible to evaluate (f return double)
    }
    return error;
}

struct visit_objects {
    struct flow_defer_scope* current_block;
    struct flow_defer_scope* next_child;
};

static struct object* visit_objects_next(struct visit_objects* visit_objects)
{
    while (visit_objects->current_block)
    {
        while (visit_objects->next_child)
        {
            if (visit_objects->next_child->declarator)
            {
                struct object* p = &visit_objects->next_child->declarator->object;
                visit_objects->next_child = visit_objects->next_child->previous;
                return p;
            }
            else
            {
                visit_objects->next_child = visit_objects->next_child->previous;
            }
        }

        visit_objects->current_block = visit_objects->current_block->previous;
        if (visit_objects->current_block)
            visit_objects->next_child = visit_objects->current_block->last_child;

    }

    return NULL;
}

static struct flow_defer_scope* flow_visit_ctx_push_tail_block(struct flow_visit_ctx* ctx)
{
    struct flow_defer_scope* owner p_block = calloc(1, sizeof * p_block);
    p_block->previous = ctx->tail_block;
    ctx->tail_block = p_block;
    return ctx->tail_block;
}

static struct flow_defer_scope* flow_visit_ctx_push_child(struct flow_visit_ctx* ctx)
{
    struct flow_defer_scope* owner child = calloc(1, sizeof * child);
    child->previous = ctx->tail_block->last_child;
    ctx->tail_block->last_child = child;
    return ctx->tail_block->last_child;
}

static void flow_defer_scope_delete_one(struct flow_defer_scope* owner p)
{
    struct flow_defer_scope* owner p_block = p;
    if (p_block != NULL)
    {
        struct flow_defer_scope* owner child = p_block->last_child;
        while (child != NULL)
        {
            struct flow_defer_scope* owner previous = child->previous;
            child->previous = NULL;
            flow_defer_scope_delete_one(child);
            child = previous;
        }
        assert(p_block->previous == NULL);
        free(p_block);
    }
}


static void flow_visit_ctx_pop_tail_block(struct flow_visit_ctx* ctx)
{
    if (ctx->tail_block)
    {
        struct flow_defer_scope* owner previous = ctx->tail_block->previous;
        ctx->tail_block->previous = NULL;
        flow_defer_scope_delete_one(ctx->tail_block);
        ctx->tail_block = previous;
    }
}

static bool check_defer_and_variables(struct flow_visit_ctx* ctx,
    struct flow_defer_scope* deferblock,
    struct token* position_token)
{
    bool found_error = false;

    struct flow_defer_scope* deferchild = deferblock->last_child;
    while (deferchild != NULL)
    {
        if (deferchild->defer_statement)
        {
            const int error_count = ctx->ctx->p_report->error_count;
            const int warnings_count = ctx->ctx->p_report->warnings_count;
            const int info_count = ctx->ctx->p_report->info_count;

            flow_visit_secondary_block(ctx, deferchild->defer_statement->secondary_block);

            if (error_count != ctx->ctx->p_report->error_count ||
                warnings_count != ctx->ctx->p_report->warnings_count ||
                info_count != ctx->ctx->p_report->info_count)
            {
                compiler_diagnostic_message(W_LOCATION, ctx->ctx, position_token, "defer end of scope");
            }
        }
        else if (deferchild->declarator)
        {
            struct declarator* p_declarator = deferchild->declarator;
            const char* name = p_declarator->name ? p_declarator->name->lexeme : "?";

            end_of_storage_visit(ctx->ctx,
                &p_declarator->type,
                type_is_view(&p_declarator->type),
                &p_declarator->object,
                position_token,
                name);
            //visit_object(ctx->ctx, &p_declarator->type, &p_declarator->object, position_token, name, false);

        }
        deferchild = deferchild->previous;
    }
    return found_error;
}
static bool flow_find_label_unlabeled_statement(struct flow_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, const char* label);

static bool check_all_defer_until_try(struct flow_visit_ctx* ctx, struct flow_defer_scope* deferblock,
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

static bool flow_find_label_block_item_list(struct flow_visit_ctx* ctx, struct block_item_list* p_block_item, const char* label)
{
    struct block_item* block_item = p_block_item->head;
    while (block_item)
    {
        if (block_item->label &&
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

    return false;
}

static bool flow_find_label_statement(struct flow_visit_ctx* ctx, struct statement* statement, const char* label);
static bool flow_find_label_unlabeled_statement(struct flow_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, const char* label)
{
    if (p_unlabeled_statement->primary_block)
    {
        if (p_unlabeled_statement->primary_block->compound_statement)
        {
            if (flow_find_label_block_item_list(ctx,
                &p_unlabeled_statement->primary_block->compound_statement->block_item_list,
                label))
            {
                return true;
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

static bool flow_find_label_statement(struct flow_visit_ctx* ctx, struct statement* statement, const char* label)
{
    if (statement->labeled_statement)
    {
        if (statement->labeled_statement->label &&
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

static bool flow_find_label_scope(struct flow_visit_ctx* ctx, struct flow_defer_scope* deferblock, const char* label)
{
    if (deferblock->p_iteration_statement)
    {
        if (deferblock->p_iteration_statement->secondary_block)
        {
            if (flow_find_label_statement(ctx, deferblock->p_iteration_statement->secondary_block->statement, label))
                return true;
        }
    }
    else if (deferblock->p_selection_statement)
    {
        if (deferblock->p_selection_statement->secondary_block)
        {
            if (flow_find_label_statement(ctx, deferblock->p_selection_statement->secondary_block->statement, label))
                return true;
        }

        if (deferblock->p_selection_statement->else_secondary_block_opt)
        {
            if (flow_find_label_statement(ctx, deferblock->p_selection_statement->else_secondary_block_opt->statement, label))
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
    else if (deferblock->p_function_body)
    {
        if (flow_find_label_block_item_list(ctx,
            &deferblock->p_function_body->block_item_list,
            label))
        {
            return true;
        }
    }
    else if (deferblock->p_compound_statement)
    {
        if (flow_find_label_block_item_list(ctx,
            &deferblock->p_compound_statement->block_item_list,
            label))
        {
            return true;
        }
    }
    return false;
}

static bool check_all_defer_until_label(struct flow_visit_ctx* ctx, struct flow_defer_scope* deferblock, const char* label,
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

static bool check_all_defer_until_iter(struct flow_visit_ctx* ctx,
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

static bool check_all_defer_until_iteration_or_selection_statement(struct flow_visit_ctx* ctx,
    struct flow_defer_scope* deferblock,
    struct token* position_token)
{
    bool error_found = false;
    struct flow_defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        if (check_defer_and_variables(ctx, p_defer, position_token))
            error_found = true;
        if (p_defer->p_iteration_statement || p_defer->p_selection_statement)
        {
            //break pode ser usado em loops or switch
            break;
        }
        p_defer = p_defer->previous;
    }
    return error_found;
}

static bool check_all_defer_until_end(struct flow_visit_ctx* ctx, struct flow_defer_scope* deferblock, struct token* position_token)
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



static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{

    flow_visit_statement(ctx, p_secondary_block->statement);

}

static void flow_visit_defer_statement(struct flow_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{
    struct flow_defer_scope* p_defer = flow_visit_ctx_push_child(ctx);
    p_defer->defer_statement = p_defer_statement;
    /*
      We are not going to visit the secondary block here because
      this is not the place were defer is executed.
    */
}

static int push_copy_of_current_state(struct flow_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;
    struct visit_objects v1 = { .current_block = ctx->tail_block,
                                  .next_child = ctx->tail_block->last_child };

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_push_copy_current_state(p_object, name, state_number);
        p_object = visit_objects_next(&v1);
    }
    return state_number;
}


static int  ctx_push_empty_state(struct flow_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;

    struct visit_objects v1 = { .current_block = ctx->tail_block,
                                  .next_child = ctx->tail_block->last_child };

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_push_empty(p_object, name, state_number);
        p_object = visit_objects_next(&v1);
    }
    return state_number;
}


static void ctx_object_set_state_from_current(struct flow_visit_ctx* ctx, int number_state)
{
    struct visit_objects v1 = { .current_block = ctx->tail_block,
                               .next_child = ctx->tail_block->last_child };

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_set_state_from_current(p_object, number_state);
        p_object = visit_objects_next(&v1);
    };
}

static void ctx_object_merge_current_state_with_state_number(struct flow_visit_ctx* ctx, int number_state)
{
    struct visit_objects v1 = { .current_block = ctx->tail_block,
                               .next_child = ctx->tail_block->last_child };

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_merge_current_state_with_state_number(p_object, number_state);
        p_object = visit_objects_next(&v1);
    };
}

static void ctx_object_merge_current_state_with_state_number_or(struct flow_visit_ctx* ctx, int number_state)
{
    struct visit_objects v1 = { .current_block = ctx->tail_block,
                               .next_child = ctx->tail_block->last_child };

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_merge_current_state_with_state_number_or(p_object, number_state);
        p_object = visit_objects_next(&v1);
    };
}


static void ctx_object_restore_current_state_from(struct flow_visit_ctx* ctx, int number_state)
{
    struct visit_objects v1 = { .current_block = ctx->tail_block,
                               .next_child = ctx->tail_block->last_child };

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_restore_current_state_from(p_object, number_state);
        p_object = visit_objects_next(&v1);
    };
}


static void pop_states(struct flow_visit_ctx* ctx, int n)
{
    struct visit_objects v1 = { .current_block = ctx->tail_block,
                               .next_child = ctx->tail_block->last_child };

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_pop_states(p_object, n);
        p_object = visit_objects_next(&v1);
    };
}
static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct initializer* p_initializer);
static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct declarator* p_declarator);


static void flow_visit_init_declarator_new(struct flow_visit_ctx* ctx, struct init_declarator* p_init_declarator)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (p_init_declarator->p_declarator)
    {
        flow_visit_declarator(ctx, p_init_declarator->p_declarator);
    }

    if (p_init_declarator->initializer)
    {
        if (p_init_declarator->initializer->assignment_expression)
        {
            flow_visit_expression(ctx, p_init_declarator->initializer->assignment_expression);
        }
        else
        {
            if (p_init_declarator->initializer->braced_initializer)
            {
                flow_visit_bracket_initializer_list(ctx,
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

            struct object temp_obj = { 0 };
            struct object* p_right_object =
                expression_get_object(p_init_declarator->initializer->assignment_expression, &temp_obj, nullable_enabled);

            //cast?
            if (expression_is_malloc(p_init_declarator->initializer->assignment_expression))
            {
                struct object* owner po = calloc(1, sizeof * po);


                struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                struct object o = make_object(&t, p_init_declarator->p_declarator, NULL);
                *po = o; //MOVED
                //p_init_declarator->p_declarator->object.pointed_ref = po; //MOVED                    
                object_set_pointer(&p_init_declarator->p_declarator->object, po);
                type_destroy(&t);
                p_init_declarator->p_declarator->object.state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
                objects_push_back(&ctx->arena, po);
            }
            else if (expression_is_calloc(p_init_declarator->initializer->assignment_expression))
            {
                struct object* owner po = calloc(1, sizeof * po);


                struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                struct object o = make_object(&t, p_init_declarator->p_declarator, NULL);
                object_set_zero(&t, &o);
                *po = o; //MOVED
                //p_init_declarator->p_declarator->object.pointed_ref = po; //MOVED                    
                object_set_pointer(&p_init_declarator->p_declarator->object, po);
                type_destroy(&t);
                p_init_declarator->p_declarator->object.state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
                objects_push_back(&ctx->arena, po);
            }
            else
            {
                const struct token* const token_position =
                    p_init_declarator->p_declarator->name ?
                    p_init_declarator->p_declarator->name :
                    p_init_declarator->p_declarator->first_token
                    ;

                if (p_right_object)
                {
                    object_assignment3(ctx->ctx,
                           token_position,
                           ASSIGMENT_TYPE_OBJECTS,
                           false,
                           type_is_view(&p_init_declarator->p_declarator->type),
                           &p_init_declarator->p_declarator->type,
                           &p_init_declarator->p_declarator->object,
                        &p_init_declarator->initializer->assignment_expression->type,
                           p_right_object

                    );
                }
                else
                {
                    //provisory handling too deep -> -> -> indirection
                    object_set_unknown(&p_init_declarator->p_declarator->type,
                          &p_init_declarator->p_declarator->object, nullable_enabled);
                }

            }

            object_destroy(&temp_obj);
        }
        else  if (p_init_declarator->initializer &&
            p_init_declarator->initializer->braced_initializer)
        {
            bool is_zero_initialized = false;
            if (p_init_declarator->initializer->braced_initializer->initializer_list == NULL)
            {
                is_zero_initialized = true;
            }
            else
            {
                if (p_init_declarator->initializer->braced_initializer->initializer_list->size == 1 &&
                    p_init_declarator->initializer->braced_initializer->initializer_list->head->assignment_expression)
                {
                    struct constant_value* p_constant_value =
                        &p_init_declarator->initializer->braced_initializer->initializer_list->head->assignment_expression->constant_value;

                    if (constant_value_is_valid(p_constant_value) &&
                        constant_value_to_ull(p_constant_value) == 0)
                    {
                        is_zero_initialized = true;
                    }

                }
            }

            if (is_zero_initialized)
            {
                object_set_zero(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object);
            }
            else
            {
                object_set_zero(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object);
            }
        }
        else
        {
            if (p_init_declarator->p_declarator->declaration_specifiers &&
                (
                    (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN) ||
                    (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
                    )
                )
            {
                object_set_zero(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object);
            }
            else
            {
                object_set_uninitialized(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object);
            }



        }
    }


    //if (p_init_declarator->initializer)
      //  flow_visit_initializer(ctx, p_init_declarator->initializer);

    //if (p_init_declarator->p_declarator)
      //  flow_visit_declarator(ctx, p_init_declarator->p_declarator);
}


static void flow_visit_init_declarator(struct flow_visit_ctx* ctx, struct init_declarator* p_init_declarator)
{
    flow_visit_init_declarator_new(ctx, p_init_declarator);
}

static void flow_visit_init_declarator_list(struct flow_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list);

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* p_type_opt);

static void flow_visit_simple_declaration(struct flow_visit_ctx* ctx, struct simple_declaration* p_simple_declaration)
{
    flow_visit_declaration_specifiers(ctx, p_simple_declaration->p_declaration_specifiers, NULL);
    flow_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}

static void flow_check_pointer_used_as_bool(struct flow_visit_ctx* ctx, struct expression* p_expression);

static void flow_visit_if_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
    p_defer->p_selection_statement = p_selection_statement;

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_expression_statement)
        flow_visit_expression_statement(ctx, p_selection_statement->p_init_statement->p_expression_statement);

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_simple_declaration)
        flow_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);


    struct declarator_array declarator_array = { 0 };

    if (p_selection_statement->condition->expression)
    {
        flow_check_pointer_used_as_bool(ctx, p_selection_statement->condition->expression);

        flow_visit_expression(ctx, p_selection_statement->condition->expression);
        compute_true_false_sets(p_selection_statement->condition->expression, &declarator_array);
    }

    if (p_selection_statement->condition->p_init_declarator)
        flow_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);


    assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);


    /*
       This index is from the end of top of stack going to base of statck
    */
    int before_if = push_copy_of_current_state(ctx, "before-if");

    declarator_array_set_objects_to_true_branch(ctx, &declarator_array, nullable_enabled);
    if (p_selection_statement->secondary_block)
    {
        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    }


    const bool true_branch_ends_with_jump =
        secondary_block_ends_with_jump(p_selection_statement->secondary_block);


    /*let's make a copy of the state we left true branch*/
    //const int true_branch = 1;
    int left_true_branch = push_copy_of_current_state(ctx, "left-true-branch");

    ctx_object_restore_current_state_from(ctx, before_if);


    if (p_selection_statement->else_secondary_block_opt)
    {
        declarator_array_set_objects_to_false_branch(ctx, &declarator_array,nullable_enabled);
        flow_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);
    }
    else
    {
        //we emulate a empty else {}
        declarator_array_set_objects_to_false_branch(ctx, &declarator_array, nullable_enabled);
    }

    bool else_ends_with_jump =
        p_selection_statement->else_secondary_block_opt ?
        secondary_block_ends_with_jump(p_selection_statement->else_secondary_block_opt) :
        false;


    if (true_branch_ends_with_jump)
    {
        if (else_ends_with_jump)
        {
            /*
               if (){
                 return
               }
               else
               {
                 return;
               }
            */
            /*no merge*/
        }
        else
        {
            /*
               if (){
                 return
               }
               else
               {

               }
            */

        }
    }
    else
    {
        if (else_ends_with_jump)
        {
            /*
               if (){

               }
               else
               {
                 return;
               }
            */

            ctx_object_restore_current_state_from(ctx, left_true_branch);
        }
        else
        {
            /*
               if (){
               }
               else {

               }
            */

            ctx_object_merge_current_state_with_state_number(ctx, left_true_branch);
            ctx_object_restore_current_state_from(ctx, left_true_branch);
        }
    }

    check_defer_and_variables(ctx, p_defer, p_selection_statement->last_token);
    flow_visit_ctx_pop_tail_block(ctx);

    pop_states(ctx, 2);


}
static void flow_visit_block_item(struct flow_visit_ctx* ctx, struct block_item* p_block_item);


static void flow_visit_try_statement(struct flow_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    const int throw_join_state_old = ctx->throw_join_state;
    struct secondary_block* catch_secondary_block_old = ctx->catch_secondary_block_opt;


    ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;

    ctx->throw_join_state = ctx_push_empty_state(ctx, "try");

    int orignial = push_copy_of_current_state(ctx, "original");

    struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
    p_defer->p_try_statement = p_try_statement;

    if (p_try_statement->secondary_block)
    {
        flow_visit_secondary_block(ctx, p_try_statement->secondary_block);
        ctx_object_set_state_from_current(ctx, orignial); //state of end of secondary block
    }

    if (p_try_statement->catch_secondary_block_opt)
    {
        //current all possible states of throw
        ctx_object_restore_current_state_from(ctx, ctx->throw_join_state);
        flow_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
        //current has the state at the end of catch block
    }

    bool try_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->secondary_block);
    bool catch_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt);

    if (try_reached_the_end && catch_reached_the_end)
    {
        //merge current with orignial
        ctx_object_merge_current_state_with_state_number_or(ctx, orignial);
        ctx_object_restore_current_state_from(ctx, orignial);
    }
    else if (try_reached_the_end)
    {
        ctx_object_restore_current_state_from(ctx, orignial);
    }
    else if (catch_reached_the_end)
    {
        //ctx_object_restore_current_state_from(ctx, orignial);       
    }


    check_defer_and_variables(ctx, p_defer, p_try_statement->secondary_block->last_token);


    flow_visit_ctx_pop_tail_block(ctx);
    pop_states(ctx, 2);
    ctx->throw_join_state = throw_join_state_old; //restore
    ctx->catch_secondary_block_opt = catch_secondary_block_old; //restore
}

static void flow_visit_switch_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = push_copy_of_current_state(ctx, "original");
    ctx->break_join_state = ctx_push_empty_state(ctx, "break join");

    struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
    p_defer->p_selection_statement = p_selection_statement;

    if (p_selection_statement->secondary_block)
    {
        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    }

    bool reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

    if (!reached_the_end)
    {
        ctx_object_restore_current_state_from(ctx, ctx->break_join_state);
    }
    check_defer_and_variables(ctx, p_defer, p_selection_statement->secondary_block->last_token);
    flow_visit_ctx_pop_tail_block(ctx);
    pop_states(ctx, 2);

    //restore
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
}

static void flow_visit_selection_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{

    if (p_selection_statement->first_token->type == TK_KEYWORD_IF)
    {
        flow_visit_if_statement(ctx, p_selection_statement);
    }
    else if (p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
    {
        flow_visit_switch_statement(ctx, p_selection_statement);
    }
    else
        assert(false);


}

static void flow_visit_compound_statement(struct flow_visit_ctx* ctx, struct compound_statement* p_compound_statement);

static void flow_visit_initializer_list(struct flow_visit_ctx* ctx, struct initializer_list* p_initializer_list);

static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {

    }
    else
    {
        flow_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void flow_visit_designation(struct flow_visit_ctx* ctx, struct designation* p_designation)
{}

static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);

static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct initializer* p_initializer)
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

static void flow_visit_initializer_list(struct flow_visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer* p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow_visit_initializer(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow_visit_type_qualifier(struct flow_visit_ctx* ctx, struct type_qualifier* p_type_qualifier)
{

}

static void flow_visit_specifier_qualifier(struct flow_visit_ctx* ctx, struct type_specifier_qualifier* p_specifier_qualifier)
{
    if (p_specifier_qualifier->type_specifier)
        flow_visit_type_specifier(ctx, p_specifier_qualifier->type_specifier);

    if (p_specifier_qualifier->type_qualifier)
        flow_visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier);
}

static void flow_visit_specifier_qualifier_list(struct flow_visit_ctx* ctx, struct specifier_qualifier_list* p_specifier_qualifier_list_opt,
    struct type* p_type)
{
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


static void flow_visit_type_name(struct flow_visit_ctx* ctx, struct type_name* p_type_name)
{

    flow_visit_specifier_qualifier_list(ctx, p_type_name->specifier_qualifier_list, &p_type_name->type);
    flow_visit_declarator(ctx, p_type_name->declarator);


    /*
    * Vamos esconder tudo e gerar um novo
    *  Exemplo
    *  (const typeof(int (*)())) -> *  ( int (*const )() )
    */
}

static void flow_visit_argument_expression_list(struct flow_visit_ctx* ctx, struct argument_expression_list* p_argument_expression_list)
{
    struct argument_expression* p_argument_expression =
        p_argument_expression_list->head;
    while (p_argument_expression)
    {
        flow_visit_expression(ctx, p_argument_expression->expression);
        p_argument_expression = p_argument_expression->next;
    }
}

static void flow_visit_generic_selection(struct flow_visit_ctx* ctx, struct generic_selection* p_generic_selection)
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


static void compare_function_arguments3(struct parser_ctx* ctx,
    struct type* p_type,
    struct argument_expression_list* p_argument_expression_list)
{
    const bool nullable_enabled = ctx->options.null_checks_enabled;

    struct param* p_current_parameter_type = NULL;

    const struct param_list* p_param_list = type_get_func_or_func_ptr_params(p_type);

    if (p_param_list)
    {
        p_current_parameter_type = p_param_list->head;
    }


    struct argument_expression* p_current_argument = p_argument_expression_list->head;

    while (p_current_argument && p_current_parameter_type)
    {
        struct object temp_obj1 = { 0 };

        struct object* p_argument_object =
            expression_get_object(p_current_argument->expression, &temp_obj1, nullable_enabled);

        if (p_argument_object)
        {
            struct object parameter_object = make_object(&p_current_parameter_type->type, NULL, p_current_argument->expression);
            object_set_uninitialized(&p_current_parameter_type->type, &parameter_object);
            object_assignment3(ctx,
              p_current_argument->expression->first_token,
              ASSIGMENT_TYPE_PARAMETER,
              true,
              type_is_view(&p_current_parameter_type->type),
              &p_current_parameter_type->type,
              &parameter_object, /*dest object*/

              &p_current_argument->expression->type,
              p_argument_object
            );

            object_destroy(&parameter_object);

        }
        p_current_argument = p_current_argument->next;
        p_current_parameter_type = p_current_parameter_type->next;
        object_destroy(&temp_obj1);
    }

    while (p_current_argument)
    {
        /*
           We have more argument than parameters, this happens with variadic functions
        */
        struct object temp_obj = { 0 };

        struct object* p_argument_object =
            expression_get_object(p_current_argument->expression, &temp_obj, nullable_enabled);
        if (p_argument_object)
        {
            checked_read_object(ctx,
                &p_current_argument->expression->type,
                p_argument_object,
                p_current_argument->expression->first_token,
                false);
        }
        else
        {
            //
        }
        p_current_argument = p_current_argument->next;
        object_destroy(&temp_obj);
    }

}

static void check_uninitialized(struct flow_visit_ctx* ctx, struct expression* p_expression)
{
    if (p_expression->is_assigment_expression)
        return;

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct object temp_obj = { 0 };
    struct object* p_object = expression_get_object(p_expression, &temp_obj, nullable_enabled);

    if (!ctx->expression_is_not_evaluated)
    {
        if (p_object && p_object->state == OBJECT_STATE_UNINITIALIZED)
        {
            if (p_expression->expression_type == PRIMARY_EXPRESSION_DECLARATOR &&
                p_expression->declarator &&
                p_expression->declarator->name)
            {
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->first_token, "using a uninitialized object '%s'", p_expression->declarator->name->lexeme);
            }
            else
            {
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->first_token, "using a uninitialized object");
            }
        }
        else if (p_object && p_object->state & OBJECT_STATE_UNINITIALIZED)
        {
            if (p_expression->declarator && p_expression->declarator->name)
            {
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->declarator->name, "object '%s' can be uninitialized ", p_expression->declarator->name->lexeme);
            }
            else
            {
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->first_token, "maybe using a uninitialized object");
            }
        }
    }
    object_destroy(&temp_obj);
}

static void object_push_states_from(const struct object* p_object_from, struct object* p_object_to)
{
    for (int i = 0; i < p_object_from->object_state_stack.size; i++)
    {
        object_state_stack_push_back(
            &p_object_to->object_state_stack,
            p_object_to->state,
            &p_object_to->ref,
            p_object_from->object_state_stack.data[i].name,
            p_object_from->object_state_stack.data[i].state_number);
    }
    for (int i = 0; i < p_object_to->members.size; i++)
    {
        object_push_states_from(p_object_from, p_object_to->members.data[i]);
    }
}


static void flow_check_pointer_used_as_bool(struct flow_visit_ctx* ctx, struct expression* p_expression)
{
    //when pointer as used as bool, like if (p) or p && p->i we check
    //if the pointer has a flow condition always true or false.

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (type_is_pointer(&p_expression->type))
    {
        struct object temp = { 0 };
        struct object* p_object = expression_get_object(p_expression, &temp, nullable_enabled);
        if (p_object)
        {
            if (is_null(p_object->state))
            {
                compiler_diagnostic_message(W_FLOW_NON_NULL,
                        ctx->ctx,
                        p_expression->first_token,
                        "pointer is always null");

            }
            else if (is_not_null(p_object->state))
            {
                compiler_diagnostic_message(W_FLOW_NON_NULL,
                        ctx->ctx,
                        p_expression->first_token,
                        "pointer is always not-null");
            }
        }
        object_destroy(&temp);
    }
}

static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression)
{
    if (p_expression == NULL)
        return;

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;


    switch (p_expression->expression_type)
    {
    case PRIMARY_EXPRESSION__FUNC__:
        break;
    case PRIMARY_IDENTIFIER:
        break;
    case PRIMARY_EXPRESSION_ENUMERATOR:

        break;
    case PRIMARY_EXPRESSION_DECLARATOR:

        check_uninitialized(ctx, p_expression);
        break;

    case PRIMARY_EXPRESSION_PARENTESIS:
        flow_visit_expression(ctx, p_expression->right);
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
        flow_visit_expression(ctx, p_expression->left);
        flow_visit_expression(ctx, p_expression->right);
        break;

    case POSTFIX_ARROW:

        flow_visit_expression(ctx, p_expression->left);

        struct object temp_obj = { 0 };
        struct object* p_object = expression_get_object(p_expression->left, &temp_obj, nullable_enabled);

        if (p_object != NULL)
        {
            if (p_object->state & OBJECT_STATE_NULL)
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {                    
                    compiler_diagnostic_message(W_FLOW_NULL_DEREFERENCE,
                            ctx->ctx,
                            p_expression->left->first_token, "object is possibly null");
                }
            }
            else if (p_object->state & OBJECT_STATE_UNINITIALIZED)
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    compiler_diagnostic_message(W_FLOW_NULL_DEREFERENCE,
                            ctx->ctx,
                            p_expression->left->first_token, "object is possibly uninitialized");
                }
            }
            else if (p_object->state & OBJECT_STATE_LIFE_TIME_ENDED)
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    compiler_diagnostic_message(W_FLOW_NULL_DEREFERENCE,
                            ctx->ctx,
                            p_expression->left->first_token, "object lifetime ended");
                }
            }

            if (p_object->state != OBJECT_STATE_NULL &&
                p_object->state != OBJECT_STATE_UNINITIALIZED &&
                p_object->ref.size == 0)
            {
                struct type t2 = type_remove_pointer(&p_expression->left->type);
                if (!type_is_void(&t2))
                {
                    struct object* owner p_object2 = calloc(1, sizeof(struct object));

                    *p_object2 = make_object(&t2, NULL, p_expression->left);
                    object_set_unknown(&t2, p_object2, nullable_enabled);
                    object_set_pointer(p_object, p_object2);////obj.pointed2 = p_object;
                    object_push_states_from(p_object, p_object2);
                    objects_push_back(&ctx->arena, p_object2);
                }
                type_destroy(&t2);
            }


        }

        object_destroy(&temp_obj);

        flow_visit_expression(ctx, p_expression->right);
        break;

    case POSTFIX_INCREMENT:
        break;
    case POSTFIX_DECREMENT:
        break;
    case POSTFIX_ARRAY:

        flow_visit_expression(ctx, p_expression->left);
        flow_visit_expression(ctx, p_expression->right);


        break;

    case POSTFIX_FUNCTION_CALL:

        flow_visit_expression(ctx, p_expression->left);

        flow_visit_argument_expression_list(ctx, &p_expression->argument_expression_list);
        //new function waiting all test to pass to become active
        compare_function_arguments3(ctx->ctx, &p_expression->left->type, &p_expression->argument_expression_list);

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

        struct object temp2 = make_object(&p_expression->type, NULL, p_expression);
        object_swap(&temp2, &p_expression->type_name->declarator->object);
        object_destroy(&temp2);

        //TODO the state of object depends of the initializer
        set_direct_state(&p_expression->type, &p_expression->type_name->declarator->object, OBJECT_STATE_ZERO);


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

    case UNARY_EXPRESSION_ASSERT:

        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_expression->right);

            struct declarator_array declarator_array = { 0 };
            compute_true_false_sets(p_expression->right, &declarator_array);
            declarator_array_set_objects_to_true_branch(ctx, &declarator_array, nullable_enabled);
        }

        break;

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:

        if (p_expression->right)
        {
            const bool t2 = ctx->expression_is_not_evaluated;
            ctx->expression_is_not_evaluated = true;
            flow_visit_expression(ctx, p_expression->right);
            ctx->expression_is_not_evaluated = t2;
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            flow_visit_type_name(ctx, p_expression->type_name);
        }


        break;

    case UNARY_EXPRESSION_SIZEOF_TYPE:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:
    case UNARY_EXPRESSION_NOT:
    case UNARY_EXPRESSION_BITNOT:
    case UNARY_EXPRESSION_NEG:
    case UNARY_EXPRESSION_PLUS:

    case UNARY_EXPRESSION_ADDRESSOF:
    {
        if (p_expression->right)
        {
            check_uninitialized(ctx, p_expression->right);
            struct object temp_obj2 = { 0 };
            struct object* p_object2 = expression_get_object(p_expression->right, &temp_obj2,nullable_enabled);

            if (!ctx->expression_is_not_evaluated)
            {
                if (p_object2 && p_object2->state == OBJECT_STATE_UNINITIALIZED)
                {
                    compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        p_expression->right->first_token, "using a uninitialized object");
                }
                else if (p_object2 && p_object2->state & OBJECT_STATE_UNINITIALIZED)
                {
                    compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        p_expression->right->first_token, "maybe using a uninitialized object");
                }
            }


            flow_visit_expression(ctx, p_expression->right);
            object_destroy(&temp_obj2);
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            flow_visit_type_name(ctx, p_expression->type_name);
        }
    }
    break;
#if 1
    case UNARY_EXPRESSION_CONTENT:
    {



        struct object temp_obj3 = { 0 };
        struct object* p_object0 = expression_get_object(p_expression->right, &temp_obj3,nullable_enabled);

        if (p_object0 && p_object0->state == OBJECT_STATE_UNINITIALIZED)
        {
            if (!ctx->expression_is_not_evaluated)
            {
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->right->first_token, "using a uninitialized object");
            }
        }
        else if (p_object0 && maybe_is_null(p_object0->state))
        {
            /*
              *p = 1*
            */
            if (!ctx->expression_is_not_evaluated)
            {
                compiler_diagnostic_message(W_FLOW_NULL_DEREFERENCE,
                    ctx->ctx,
                    p_expression->right->first_token, "dereference a NULL object");
            }
        }
        object_destroy(&temp_obj3);

        if (p_object0->state != OBJECT_STATE_NULL &&
                p_object0->state != OBJECT_STATE_UNINITIALIZED &&
                p_object0->ref.size == 0)
        {
            struct type t2 = type_remove_pointer(&p_expression->right->type);
            if (!type_is_void(&t2))
            {
                struct object* owner p_object2 = calloc(1, sizeof(struct object));

                *p_object2 = make_object(&t2, NULL, p_expression->right);
                object_set_unknown(&t2, p_object2, nullable_enabled);
                object_set_pointer(p_object0, p_object2);////obj.pointed2 = p_object;
                object_push_states_from(p_object0, p_object2);
                objects_push_back(&ctx->arena, p_object2);
            }
            type_destroy(&t2);
        }

        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_expression->right);
        }
    }
    break;
#endif




    case ASSIGNMENT_EXPRESSION:
    {
        flow_visit_expression(ctx, p_expression->left);

        flow_visit_expression(ctx, p_expression->right);

        struct object temp_obj1 = { 0 };
        struct object* const p_right_object = expression_get_object(p_expression->right, &temp_obj1, nullable_enabled);

        struct object temp_obj2 = { 0 };
        struct object* const p_dest_object = expression_get_object(p_expression->left, &temp_obj2, nullable_enabled);



        bool bool_source_zero_value = constant_value_is_valid(&p_expression->right->constant_value) &&
            constant_value_to_ull(&p_expression->right->constant_value) == 0;

        if (p_expression->right &&
            p_expression->right->expression_type == POSTFIX_FUNCTION_CALL)
        {
            if (p_expression->right->left &&
                p_expression->right->left->declarator &&
                p_expression->right->left->declarator->name &&
                strcmp(p_expression->right->left->declarator->name->lexeme, "calloc") == 0)
            {
                bool_source_zero_value = true;
            }
        }

        object_assignment3(ctx->ctx,
            p_expression->left->first_token,
            ASSIGMENT_TYPE_OBJECTS,
            true,
            type_is_view(&p_expression->left->type), /*dest type*/
            &p_expression->left->type, /*dest type*/
            p_dest_object, /*dest object*/
            &p_expression->right->type, /*source type*/
            p_right_object /*source*/);

        /*
          built-in malloc, calloc assignment
        */
        if (expression_is_malloc(p_expression->right))
        {
            struct object* owner po = calloc(1, sizeof * po);


            struct type t = type_remove_pointer(&p_expression->left->type);
            struct object o = make_object(&t, NULL, p_expression->left);
            *po = o; //MOVED
            //p_init_declarator->p_declarator->object.pointed_ref = po; //MOVED                    
            object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
            objects_push_back(&ctx->arena, po);
        }
        else if (expression_is_calloc(p_expression->right))
        {
            struct object* owner po = calloc(1, sizeof * po);


            struct type t = type_remove_pointer(&p_expression->left->type);
            struct object o = make_object(&t, NULL, p_expression->left);
            object_set_zero(&t, &o);
            *po = o; //MOVED
            //p_init_declarator->p_declarator->object.pointed_ref = po; //MOVED                    
            object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
            objects_push_back(&ctx->arena, po);
        }

        object_destroy(&temp_obj1);
        object_destroy(&temp_obj2);
    }
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
        flow_visit_expression(ctx, p_expression->left);
        flow_visit_expression(ctx, p_expression->right);

        break;

    case EQUALITY_EXPRESSION_NOT_EQUAL:
        flow_visit_expression(ctx, p_expression->left);
        flow_visit_expression(ctx, p_expression->right);
        break;

    case LOGICAL_OR_EXPRESSION:
    {
        flow_check_pointer_used_as_bool(ctx, p_expression->left);
        flow_check_pointer_used_as_bool(ctx, p_expression->right);

        flow_visit_expression(ctx, p_expression->left);


        const int original = push_copy_of_current_state(ctx, "original");
        struct declarator_array d = { 0 };
        compute_true_false_sets(p_expression->left, &d);

        //Set all variables to false state, because otherwise, the right branch
        // would not be evaluated
        declarator_array_set_objects_to_false_branch(ctx, &d, nullable_enabled);
        flow_visit_expression(ctx, p_expression->right);

        //Restore previous state "original"
        ctx_object_restore_current_state_from(ctx, original);

        //Pop original state
        pop_states(ctx, 1);
    }
    break;

    case LOGICAL_AND_EXPRESSION:
    {
        flow_check_pointer_used_as_bool(ctx, p_expression->left);
        flow_check_pointer_used_as_bool(ctx, p_expression->right);

        flow_visit_expression(ctx, p_expression->left);

        const int original = push_copy_of_current_state(ctx, "original");

        struct declarator_array d = { 0 };
        compute_true_false_sets(p_expression->left, &d);

        //Set all variables to true state, because otherwise, the right branch
        // would not be evaluated
        declarator_array_set_objects_to_true_branch(ctx, &d, nullable_enabled);

        flow_visit_expression(ctx, p_expression->right);
        ctx_object_restore_current_state_from(ctx, original);
        pop_states(ctx, 1); //pop original
    }
    break;

    case AND_EXPRESSION:
    case EXCLUSIVE_OR_EXPRESSION:

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

static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
        flow_visit_expression(ctx, p_expression_statement->expression_opt);
}

static void flow_visit_block_item_list(struct flow_visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void flow_visit_compound_statement(struct flow_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{

    struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
    p_defer->p_compound_statement = p_compound_statement;

    flow_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    check_defer_and_variables(ctx, p_defer, p_compound_statement->last_token);

    flow_visit_ctx_pop_tail_block(ctx);

}

static void flow_visit_do_while_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct declarator_array declarator_array = { 0 };

    if (p_iteration_statement->expression1)
    {
        compute_true_false_sets(p_iteration_statement->expression1, &declarator_array);
        flow_visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->secondary_block)
    {
        struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        p_defer->p_iteration_statement = p_iteration_statement;

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        check_defer_and_variables(ctx, p_defer, p_iteration_statement->secondary_block->last_token);

        flow_visit_ctx_pop_tail_block(ctx);

        bool was_last_statement_inside_true_branch_return =
            secondary_block_ends_with_jump(p_iteration_statement->secondary_block);


        if (was_last_statement_inside_true_branch_return)
        {
            //restore_state(ctx, 0);
            //if (p_object_compared_with_not_null)
            //{
                //do {}  while (p);
              //  p_object_compared_with_not_null->state = OBJECT_STATE_NULL;
            //}
        }
        else
        {
            //do { } while (p);
            declarator_array_set_objects_to_false_branch(ctx, &declarator_array, nullable_enabled);
        }
    }
}

static void flow_visit_while_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = push_copy_of_current_state(ctx, "original");
    ctx->break_join_state = ctx_push_empty_state(ctx, "break join");


    struct declarator_array declarator_array = { 0 };


    if (p_iteration_statement->expression1)
    {
        compute_true_false_sets(p_iteration_statement->expression1, &declarator_array);

        //We do a visit but this is not conclusive..so we ignore warnings
        ctx->ctx->options.diagnostic_stack_top_index++;
        ctx->ctx->options.diagnostic_stack[ctx->ctx->options.diagnostic_stack_top_index].warnings = 0;
        ctx->ctx->options.diagnostic_stack[ctx->ctx->options.diagnostic_stack_top_index].errors = 0;
        ctx->ctx->options.diagnostic_stack[ctx->ctx->options.diagnostic_stack_top_index].notes = 0;
        flow_visit_expression(ctx, p_iteration_statement->expression1);
        ctx->ctx->options.diagnostic_stack_top_index--;
    }

    if (p_iteration_statement->secondary_block)
    {

        struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        p_defer->p_iteration_statement = p_iteration_statement;
        declarator_array_set_objects_to_true_branch(ctx, &declarator_array, nullable_enabled);

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        check_defer_and_variables(ctx, p_defer, p_iteration_statement->secondary_block->last_token);



        bool was_last_statement_inside_true_branch_return =
            secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

        if (was_last_statement_inside_true_branch_return)
        {

            /*
            while (p)
            {
              return;
            }
            */
            //restore original
            //ctx->initial_state = push_copy_of_current_state(ctx, "original");
            ctx_object_restore_current_state_from(ctx, ctx->initial_state);
            declarator_array_set_objects_to_false_branch(ctx, &declarator_array, nullable_enabled);
        }
        else
        {
            declarator_array_set_objects_to_false_branch(ctx, &declarator_array, nullable_enabled);
            ctx_object_merge_current_state_with_state_number(ctx, ctx->break_join_state);
            ctx_object_restore_current_state_from(ctx, ctx->break_join_state);
        }

        pop_states(ctx, 2);
        flow_visit_ctx_pop_tail_block(ctx);
    }

    //Now we visit the expression again because we have the states
    //at end of while that will be used again for the expression.
    //At this time we print warnings
    flow_visit_expression(ctx, p_iteration_statement->expression1);

    //restore
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;

}

static void flow_visit_for_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);


    if (p_iteration_statement->expression0)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression0);
    }

    if (p_iteration_statement->expression1)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression1);
    }
    if (p_iteration_statement->expression2)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression2);
    }


    if (p_iteration_statement->secondary_block)
    {
        struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        p_defer->p_iteration_statement = p_iteration_statement;
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        check_defer_and_variables(ctx, p_defer, p_iteration_statement->secondary_block->last_token);
        flow_visit_ctx_pop_tail_block(ctx);
    }

    /*we visit again*/
    if (p_iteration_statement->secondary_block)
    {
        struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        p_defer->p_iteration_statement = p_iteration_statement;
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        check_defer_and_variables(ctx, p_defer, p_iteration_statement->secondary_block->last_token);
        flow_visit_ctx_pop_tail_block(ctx);
    }
}


static void flow_visit_iteration_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    switch (p_iteration_statement->first_token->type)
    {
    case  TK_KEYWORD_WHILE:
        flow_visit_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_DO:
        flow_visit_do_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_FOR:
        flow_visit_for_statement(ctx, p_iteration_statement);
        break;
    default:
        assert(false);
        break;
    }
}

static void flow_visit_jump_statement(struct flow_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
    {
        ctx_object_merge_current_state_with_state_number(ctx, ctx->throw_join_state);
        check_all_defer_until_try(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        if (p_jump_statement->expression_opt)
        {
            flow_visit_expression(ctx, p_jump_statement->expression_opt);
        }

        /*
          returning a declarator will move the onwership
        */
        if (p_jump_statement->expression_opt)
        {
            struct object temp_obj = { 0 };
            struct object* p_object =
                expression_get_object(p_jump_statement->expression_opt, &temp_obj, nullable_enabled);

            if (p_object)
            {
                checked_read_object(ctx->ctx,
                    &p_jump_statement->expression_opt->type,
                    p_object,
                    p_jump_statement->expression_opt->first_token,
                    true);

                struct object dest_object =
                    make_object(ctx->p_return_type, NULL, p_jump_statement->expression_opt);

                object_set_zero(ctx->p_return_type, &dest_object);

                object_assignment3(ctx->ctx,
                 p_jump_statement->expression_opt->first_token,
                 ASSIGMENT_TYPE_RETURN,
                 true,
                    type_is_view(ctx->p_return_type), /*dest type*/
                    ctx->p_return_type, /*dest type*/
                    &dest_object, /*dest object*/
                    &p_jump_statement->expression_opt->type, /*source type*/
                    p_object /*source*/
                );

                object_destroy(&dest_object);
            }

            object_destroy(&temp_obj);
        }
        check_all_defer_until_end(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {
        check_all_defer_until_iter(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
    {
        ctx_object_merge_current_state_with_state_number(ctx, ctx->break_join_state);
        check_all_defer_until_iteration_or_selection_statement(ctx, ctx->tail_block, p_jump_statement->first_token);
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

static void flow_visit_label(struct flow_visit_ctx* ctx, struct label* p_label);

static void flow_visit_labeled_statement(struct flow_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    if (p_labeled_statement->label)
    {
        flow_visit_label(ctx, p_labeled_statement->label);
    }

    if (p_labeled_statement->statement)
    {
        flow_visit_statement(ctx, p_labeled_statement->statement);
    }
}

static void flow_visit_primary_block(struct flow_visit_ctx* ctx, struct primary_block* p_primary_block)
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

static void flow_visit_unlabeled_statement(struct flow_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
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

static void flow_visit_statement(struct flow_visit_ctx* ctx, struct statement* p_statement)
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

static void flow_visit_label(struct flow_visit_ctx* ctx, struct label* p_label)
{
    ctx_object_restore_current_state_from(ctx, ctx->initial_state);
}

static void flow_visit_block_item(struct flow_visit_ctx* ctx, struct block_item* p_block_item)
{

    if (p_block_item->declaration)
    {
        flow_visit_declaration(ctx, p_block_item->declaration);
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

static void flow_visit_block_item_list(struct flow_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static enum object_state parse_string_state(const char* s, bool* invalid)
{
    //TODO faling with _
    *invalid = false;

    enum object_state e = 0;
    const char* p = s;

    while (*p)
    {
        if (isalpha(*p))
        {
            const char* start = p;
            while (isalpha(*p) || *p == '-')
            {
                p++;
            }

            if (strncmp(start, "moved", p - start) == 0)
                e |= OBJECT_STATE_MOVED;

            else if (strncmp(start, "null", p - start) == 0)
                e |= OBJECT_STATE_NULL;
            else if (strncmp(start, "not-null", p - start) == 0)
                e |= OBJECT_STATE_NOT_NULL;
            else if (strncmp(start, "maybe-null", p - start) == 0)
                e |= (OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL);
            else if (strncmp(start, "uninitialized", p - start) == 0)
                e |= OBJECT_STATE_UNINITIALIZED;
            else if (strncmp(start, "zero", p - start) == 0)
                e |= OBJECT_STATE_ZERO;
            else if (strncmp(start, "not-zero", p - start) == 0)
                e |= OBJECT_STATE_NOT_ZERO;
            else if (strncmp(start, "any", p - start) == 0)
                e |= (OBJECT_STATE_NOT_ZERO | OBJECT_STATE_ZERO);
            //else if (strncmp(start, "moved", p - start) == 0)
                //e |= OBJECT_STATE_MOVED;
            else if (strncmp(start, "or", p - start) == 0) //or obsolte
            {
                //skiped
            }
            else
            {
                *invalid = true;
                return 0; //error invalid name
            }
        }
        else
        {
            p++;
        }
    }

    return e;

}


static void flow_visit_pragma_declaration(struct flow_visit_ctx* ctx, struct pragma_declaration* p_pragma_declaration)
{
    execute_pragma(ctx->ctx, p_pragma_declaration, true);
}

static void flow_visit_static_assert_declaration(struct flow_visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration)
{
    const bool t2 = ctx->expression_is_not_evaluated;
    ctx->expression_is_not_evaluated = true;
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    flow_visit_expression(ctx, p_static_assert_declaration->constant_expression);

    ctx->expression_is_not_evaluated = t2; //restore


    if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG ||
        p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG_EX)
    {
        bool ex = p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG_EX;

        compiler_diagnostic_message(W_LOCATION, ctx->ctx, p_static_assert_declaration->first_token, "static_debug");

        struct object temp_obj = { 0 };
        struct object* p_obj =
            expression_get_object(p_static_assert_declaration->constant_expression,
                &temp_obj, nullable_enabled);

        if (p_obj)
        {
            print_object(&p_static_assert_declaration->constant_expression->type, p_obj, !ex);
        }

        object_destroy(&temp_obj);
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_STATE)
    {
        /*TODO
           check state

        */

        bool is_invalid = false;
        enum object_state e = 0;
        if (p_static_assert_declaration->string_literal_opt)
            e = parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);
        if (is_invalid)
        {
            compiler_diagnostic_message(C_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assert_declaration->first_token, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
        }
        else
        {
            struct object temp_obj = { 0 };
            struct object* p_obj =
                expression_get_object(p_static_assert_declaration->constant_expression, &temp_obj, nullable_enabled);
            if (p_obj)
            {


                if (e != p_obj->state)
                {
                    compiler_diagnostic_message(C_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assert_declaration->first_token, "static_state failed");
                    printf("expected :%s\n", p_static_assert_declaration->string_literal_opt->lexeme);
                    printf("current  :");
                    object_state_to_string(p_obj->state);
                    printf("\n");
                }
            }
            else
            {
                if (e != OBJECT_STATE_NOT_APPLICABLE)
                {
                    compiler_diagnostic_message(C_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assert_declaration->first_token, "static_state failed");
                }
            }

            object_destroy(&temp_obj);
        }
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_SET)
    {

        struct object temp_obj = { 0 };
        struct object* p_obj =
            expression_get_object(p_static_assert_declaration->constant_expression,
                &temp_obj, nullable_enabled);

        if (p_obj)
        {
            if (p_static_assert_declaration->string_literal_opt)
            {

                const char* lexeme =
                    p_static_assert_declaration->string_literal_opt->lexeme;

                if (strcmp(lexeme, "\"zero\"") == 0)
                {
                    //gives the semantics of {0} or calloc
                    object_set_zero(&p_static_assert_declaration->constant_expression->type, p_obj);
                }
                else
                {
                    bool is_invalid = false;
                    enum object_state e =
                        parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);

                    if (!is_invalid)
                    {
                        set_object(&p_static_assert_declaration->constant_expression->type, p_obj, e);
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_STATIC_SET, ctx->ctx, p_static_assert_declaration->first_token, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
                    }
                }
            }

        }
        object_destroy(&temp_obj);
    }
}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* p_type);


static void flow_visit_direct_declarator(struct flow_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration* parameter = NULL;

        if (p_direct_declarator->function_declarator->parameter_type_list_opt)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }


        ctx->parameter_list++;

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

        ctx->parameter_list--;

    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            flow_visit_expression(ctx, p_direct_declarator->array_declarator->assignment_expression);
        }

    }
}

static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct declarator* p_declarator)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (ctx->tail_block &&
        p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
    {

        if (ctx->parameter_list > 1)
        {
            /*
             The objective here is to avoid including the arguments
             of function pointers inside the scope.
             Sample
             void x_destroy(void (*f)(void * owner p))
             We add f but not p.
            */
            return;
        }

        struct flow_defer_scope* p_defer = flow_visit_ctx_push_child(ctx);
        p_defer->declarator = p_declarator;


        struct object temp = make_object(&p_declarator->type, p_declarator, NULL);
        object_set_uninitialized(&p_declarator->type, &temp);
        object_swap(&temp, &p_declarator->object);
        object_destroy(&temp);

        if (p_declarator->declaration_specifiers &&
            p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            if (type_is_pointer(&p_declarator->type))
            {
                if (type_is_nullable(&p_declarator->type, ctx->ctx->options.null_checks_enabled))
                {
                    p_declarator->object.state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
                }
                else
                {
                    p_declarator->object.state = OBJECT_STATE_NOT_NULL;
                }

                if (type_is_pointer_to_out(&p_declarator->type))
                {
                    //
                    struct object* owner po = calloc(1, sizeof * po);


                    struct type t = type_remove_pointer(&p_declarator->type);
                    struct object o = make_object(&t, p_declarator, NULL);
                    object_set_uninitialized(&t, &o);
                    *po = o; //MOVED
                    object_set_pointer(&p_declarator->object, po); //MOVED                    
                    type_destroy(&t);
                    objects_push_back(&ctx->arena, po);
                    //p_declarator->object.state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
                }
                else if (type_is_any_owner(&p_declarator->type))
                {
                    //
                    struct object* owner po = calloc(1, sizeof * po);


                    struct type t = type_remove_pointer(&p_declarator->type);
                    struct object o = make_object(&t, p_declarator, NULL);
                    object_set_unknown(&t, &o, nullable_enabled);
                    *po = o; //MOVED
                    object_set_pointer(&p_declarator->object, po); //MOVED                    
                    type_destroy(&t);
                    objects_push_back(&ctx->arena, po);
                    //p_declarator->object.state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
                }
            }
            else if (type_is_struct_or_union(&p_declarator->type))
            {
                object_set_unknown(&p_declarator->type, &p_declarator->object, nullable_enabled);
            }
            else if (type_is_array(&p_declarator->type))
            {
                // assert(false);//TODO
                 //object_set_unknown(&p_declarator->type, &p_declarator->object);
                p_declarator->object.state = OBJECT_STATE_NOT_ZERO;
            }
            else
            {
                p_declarator->object.state = OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO;
            }


#if 0
            if (type_is_pointer(&p_declarator->type))
            {
                //TODO necessary?
                struct type t2 = type_remove_pointer(&p_declarator->type);
                if (p_declarator->object.pointed)
                {
                    set_object(&t2, p_declarator->object.pointed, (OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL));
                }
                type_destroy(&t2);
        }
#endif
    }
}

    /*if (p_declarator->pointer)
    {
        struct pointer* p = p_declarator->pointer;
        while (p)
        {
            p = p->pointer;
        }
    }*/


    if (p_declarator->direct_declarator)
    {
        flow_visit_direct_declarator(ctx, p_declarator->direct_declarator);
    }
}

static void flow_visit_init_declarator_list(struct flow_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_init_declarator);
        p_init_declarator = p_init_declarator->next;
    }
}

static void flow_visit_member_declarator(struct flow_visit_ctx* ctx, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        flow_visit_declarator(ctx, p_member_declarator->declarator);
    }
}

static void flow_visit_member_declarator_list(struct flow_visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        flow_visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}

static void flow_visit_member_declaration(struct flow_visit_ctx* ctx, struct member_declaration* p_member_declaration)
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

static void flow_visit_member_declaration_list(struct flow_visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* p_member_declaration =
        p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow_visit_attribute_specifier(struct flow_visit_ctx* ctx, struct attribute_specifier* p_attribute_specifier)
{

}

static void flow_visit_attribute_specifier_sequence(struct flow_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence)
{
    struct attribute_specifier* current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        flow_visit_attribute_specifier(ctx, current);
        current = current->next;
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{

    if (p_struct_or_union_specifier->attribute_specifier_sequence_opt)
        flow_visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence_opt);

    //struct struct_or_union_specifier* p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);


    flow_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);

}

static void flow_visit_enumerator(struct flow_visit_ctx* ctx, struct enumerator* p_enumerator)
{
    if (p_enumerator->constant_expression_opt)
        flow_visit_expression(ctx, p_enumerator->constant_expression_opt);

}

static void flow_visit_enumerator_list(struct flow_visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* current = p_enumerator_list->head;
    while (current)
    {
        flow_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    if (p_enum_specifier->attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_enum_specifier->attribute_specifier_sequence_opt);
    }

    flow_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void flow_visit_typeof_specifier(struct flow_visit_ctx* ctx, struct typeof_specifier* p_typeof_specifier)
{}

static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct type_specifier* p_type_specifier)
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

static void flow_visit_type_specifier_qualifier(struct flow_visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
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

static void flow_visit_storage_class_specifier(struct flow_visit_ctx* ctx, struct storage_class_specifier* p_storage_class_specifier)
{
    if (p_storage_class_specifier->flags & STORAGE_SPECIFIER_AUTO)
    {

    }
}

static void flow_visit_declaration_specifier(struct flow_visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
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

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx,
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


    while (p_declaration_specifier)
    {
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

void flow_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        flow_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }

    if (p_declaration->pragma_declaration)
    {
        flow_visit_pragma_declaration(ctx, p_declaration->pragma_declaration);
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

        assert(ctx->p_return_type == NULL);

        struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        ctx->p_return_type = &type;

        //struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        //p_defer->p_statement = p_declaration->function_body;

        flow_visit_compound_statement(ctx, p_declaration->function_body);
        type_destroy(&type);
        ctx->p_return_type = NULL;

        //flow_visit_ctx_pop_tail_block(ctx);
    }

}

void flow_start_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration)
{
    s_object_id_generator = 0; //reset object id, id are local

    ctx->state_number_generator = 0;
    if (p_declaration->function_body)
    {

        assert(ctx->tail_block == NULL);
        struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        if (p_defer == NULL)
        {
            return;
        }
        p_defer->p_function_body = p_declaration->function_body;

        flow_visit_declaration(ctx, p_declaration);

        if (!flow_is_last_item_return(p_declaration->function_body))
        {
            check_defer_and_variables(ctx, p_defer, p_declaration->function_body->last_token);
        }

        flow_visit_ctx_pop_tail_block(ctx);
    }
    else
    {
        struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        if (p_defer == NULL)
        {
            return;
        }
        flow_visit_declaration(ctx, p_declaration);
        flow_visit_ctx_pop_tail_block(ctx);
    }


    objects_clear(&ctx->arena);
}

void flow_visit_ctx_destroy(struct flow_visit_ctx* obj_owner p)
{
    assert(p->tail_block == NULL);
}

void flow_analysis_visit(struct flow_visit_ctx* ctx)
{
    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        flow_visit_declaration(ctx, p_declaration);
        p_declaration = p_declaration->next;
    }
}

