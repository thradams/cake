

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
struct declarator_array;
static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression, struct declarator_array* a);
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
    struct declarator_array_item* owner data;
    int size;
    int capacity;
};

void declarator_array_clear(struct declarator_array* p)
{

    free(p->data);
    p->data = NULL;
    p->size = 0;
    p->capacity = 0;
}

void declarator_array_destroy(struct declarator_array* obj_owner p)
{
    free(p->data);
}

int declarator_array_reserve(struct declarator_array* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* owner pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL)
            return ENOMEM;
        static_set(p->data, "moved");
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int declarator_array_push_back(struct declarator_array* p, const struct declarator_array_item* book)
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

        int error = declarator_array_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = *book; /*COPIED*/


    p->size++;

    return 0;
}

static void declarator_array_invert(struct declarator_array* true_false_sets)
{
    for (int i = 0; i < true_false_sets->size; i++)
    {
        enum boolean_flag temp = true_false_sets->data[i].true_branch_state;
        true_false_sets->data[i].true_branch_state = true_false_sets->data[i].false_branch_state;
        true_false_sets->data[i].false_branch_state = temp;
    }
}

static void declarator_array_swap(struct declarator_array* a, struct declarator_array* b)
{
    struct declarator_array temp = *a;
    *a = *b;
    *b = temp;
}

static int find_item_index_by_expression(struct declarator_array* a, struct expression* p_expression)
{
    for (int i = 0; i < a->size; i++)
    {
        if (a->data[i].p_expression->declarator == p_expression->declarator)
            return i;
    }
    return -1;
}

static bool declarator_array_is_same(struct declarator_array* a, struct declarator_array* b)
{
    if (a->size != b->size)
        return false;
    for (int i = 0; i < a->size; i++)
    {
        if (a->data[i].p_expression != b->data[i].p_expression)
            return false;
        if (a->data[i].false_branch_state != b->data[i].false_branch_state)
            return false;
        if (a->data[i].true_branch_state != b->data[i].true_branch_state)
            return false;
    }
    return true;
}

static void declarator_array_set_objects_to_true_branch(struct flow_visit_ctx* ctx, struct declarator_array* a, bool nullable_enabled)
{
    for (int i = 0; i < a->size; i++)
    {
        if (a->data[i].p_expression != NULL)
        {

            struct object* p_object =
                expression_get_object(ctx, a->data[i].p_expression, nullable_enabled);
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
                    p_object->state &= ~OBJECT_STATE_UNINITIALIZED;
                    if (is_pointer)
                        p_object->state |= (OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL);
                    else
                        p_object->state |= (OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO);
                }
                else if (flag & BOOLEAN_FLAG_FALSE)
                {
                    p_object->state &= ~OBJECT_STATE_UNINITIALIZED;
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
                    p_object->state &= ~OBJECT_STATE_UNINITIALIZED;
                    if (is_pointer)
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_NULL;
                        if (p_object->state & OBJECT_STATE_MOVED)
                        {
                        }
                        else
                            p_object->state |= OBJECT_STATE_NOT_NULL;
                    }
                    else
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_ZERO;
                        p_object->state |= OBJECT_STATE_NOT_ZERO;
                    }
                }
            }
            if (p_object && p_object->is_temporary)
            {
                p_object->state = OBJECT_STATE_LIFE_TIME_ENDED;
            }
        }
    }
}

static void declarator_array_set_objects_to_false_branch(struct flow_visit_ctx* ctx, struct declarator_array* a, bool nullable_enabled)
{
    for (int i = 0; i < a->size; i++)
    {
        if (a->data[i].p_expression != NULL)
        {

            struct object* p_object =
                expression_get_object(ctx, a->data[i].p_expression, nullable_enabled);
            if (p_object)
            {
                const bool is_pointer = type_is_pointer(&a->data[i].p_expression->type);
                const enum boolean_flag flag = a->data[i].false_branch_state;



                if ((flag & BOOLEAN_FLAG_TRUE) && (flag & BOOLEAN_FLAG_FALSE))
                {
                    p_object->state &= ~OBJECT_STATE_UNINITIALIZED;

                    if (is_pointer)
                        p_object->state |= (OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL);
                    else
                        p_object->state |= (OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO);
                }
                else if (flag & BOOLEAN_FLAG_FALSE)
                {
                    p_object->state &= ~OBJECT_STATE_UNINITIALIZED;

                    if (is_pointer)
                    {

                        p_object->state = p_object->state & ~OBJECT_STATE_NOT_NULL;
                        p_object->state = p_object->state & ~OBJECT_STATE_MOVED;
                        p_object->state |= OBJECT_STATE_NULL;
                        //pointed object does not exist. set nothing
                        //See test_18000.c
                        //
                        //object_set_pointed_to_nothing(&a->data[i].p_expression->type, p_object);
                    }
                    else
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_NOT_ZERO;
                        p_object->state |= OBJECT_STATE_ZERO;
                    }
                }
                else if (flag & BOOLEAN_FLAG_TRUE)
                {
                    p_object->state &= ~OBJECT_STATE_UNINITIALIZED;

                    if (is_pointer)
                    {
                        //se era moved nao faz nada!
                        if (p_object->state & OBJECT_STATE_MOVED)
                        {
                        }
                        else
                        {
                            p_object->state &= ~OBJECT_STATE_NULL;
                            p_object->state |= OBJECT_STATE_NOT_NULL;
                        }
                    }
                    else
                    {
                        p_object->state = p_object->state & ~OBJECT_STATE_ZERO;
                        p_object->state |= OBJECT_STATE_NOT_ZERO;
                    }
                }


            }
            //object_destroy(&temp);
        }
    }
}



static int push_copy_of_current_state(struct flow_visit_ctx* ctx, const char* name);


void ctx_remove_state(struct flow_visit_ctx* ctx, int state_number);

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
                struct object* p = visit_objects->next_child->declarator->p_object;
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

            end_of_storage_visit(ctx,
                &p_declarator->type,
                type_is_view(&p_declarator->type),
                p_declarator->p_object,
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

NODISCARD
static int push_copy_of_current_state(struct flow_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;
    struct visit_objects v1 = { .current_block = ctx->tail_block,
                                  .next_child = ctx->tail_block->last_child };

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_add_copy_current_state(p_object, name, state_number);
        p_object = visit_objects_next(&v1);
    }
    return state_number;
}


static int ctx_add_empty_state(struct flow_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;

    struct visit_objects v1 = { .current_block = ctx->tail_block,
                                  .next_child = ctx->tail_block->last_child };

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_add_empty_state(p_object, name, state_number);
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

void ctx_remove_state(struct flow_visit_ctx* ctx, int state_number)
{
    struct visit_objects v1 = { 0 };

    v1.current_block = ctx->tail_block;
    v1.next_child = ctx->tail_block->last_child;

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_remove_state(p_object, state_number);
        p_object = visit_objects_next(&v1);
    };
}



static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct initializer* p_initializer);
static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct declarator* p_declarator);

static void braced_initializer_set_object(struct braced_initializer* p, struct type* type, struct object* object)
{
    if (p->initializer_list == NULL)
    {
    }
    //TODO currently it is zero
    
    object_set_zero(type, object);
}

static void flow_visit_init_declarator(struct flow_visit_ctx* ctx, struct init_declarator* p_init_declarator)
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
            struct declarator_array a = { 0 };
            flow_visit_expression(ctx, p_init_declarator->initializer->assignment_expression, &a);
            declarator_array_destroy(&a);
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


            struct object* p_right_object =
                expression_get_object(ctx, p_init_declarator->initializer->assignment_expression, nullable_enabled);

            if (p_right_object)
            {
                flow_assignment(ctx,
                                    p_init_declarator->initializer->assignment_expression->first_token,
                                    ASSIGMENT_TYPE_OBJECTS,
                                    false,
                                    type_is_view(&p_init_declarator->p_declarator->type),
                                    type_is_nullable(&p_init_declarator->p_declarator->type, ctx->ctx->options.null_checks_enabled),
                                    &p_init_declarator->p_declarator->type,
                                    p_init_declarator->p_declarator->p_object,
                                    &p_init_declarator->initializer->assignment_expression->type,
                                    p_right_object);
            }
            //cast?
            if (expression_is_malloc(p_init_declarator->initializer->assignment_expression))
            {
                struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                struct object* po = make_object(ctx, &t, p_init_declarator->p_declarator, NULL);
                object_set_pointer(p_init_declarator->p_declarator->p_object, po);
                type_destroy(&t);
                p_init_declarator->p_declarator->p_object->state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
            }
            else if (expression_is_calloc(p_init_declarator->initializer->assignment_expression))
            {

                struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                struct object* po = make_object(ctx, &t, p_init_declarator->p_declarator, NULL);
                object_set_zero(&t, po);
                object_set_pointer(p_init_declarator->p_declarator->p_object, po);
                type_destroy(&t);
                p_init_declarator->p_declarator->p_object->state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
            }

            //object_destroy(&temp_obj);
        }
        else  if (p_init_declarator->initializer &&
            p_init_declarator->initializer->braced_initializer)
        {
            struct object* po = make_object(ctx, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator, NULL);

            braced_initializer_set_object(p_init_declarator->initializer->braced_initializer,
                &p_init_declarator->p_declarator->type,
                po);

            struct object* p_right_object = po;
            flow_assignment(ctx,
                                   p_init_declarator->p_declarator->first_token,
                                   ASSIGMENT_TYPE_OBJECTS,
                                   false,
                                   type_is_view(&p_init_declarator->p_declarator->type),
                                   type_is_nullable(&p_init_declarator->p_declarator->type, ctx->ctx->options.null_checks_enabled),
                                   &p_init_declarator->p_declarator->type,
                                   p_init_declarator->p_declarator->p_object,
                                   &p_init_declarator->p_declarator->type,
                                   p_right_object);
            //object_destroy(&o);
        }
        else
        {
            //struct object * po = make_object(ctx, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator, NULL);

            if (p_init_declarator->p_declarator->declaration_specifiers &&
                (
                    (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN) ||
                    (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
                    )
                )
            {
                object_set_zero(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_object);
            }
            else
            {
                object_set_uninitialized(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_object);
            }
            //struct object* p_right_object = po;
            //object_assignment3(ctx,
              //                     p_init_declarator->p_declarator->first_token,
                //                   ASSIGMENT_TYPE_OBJECTS,
                  //                 false,
                    //               type_is_view(&p_init_declarator->p_declarator->type),
                      //             type_is_nullable(&p_init_declarator->p_declarator->type, ctx->ctx->options.null_checks_enabled),
                        //           &p_init_declarator->p_declarator->type,
                          //         p_init_declarator->p_declarator->p_object,
                            //       &p_init_declarator->p_declarator->type,
                              //     p_right_object);
            //object_destroy(&o);
        }
    }

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
        flow_visit_expression(ctx, p_selection_statement->condition->expression, &declarator_array);
    }

    if (p_selection_statement->condition->p_init_declarator)
        flow_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);


    assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);


    /*
       This index is from the end of top of stack going to base of statck
    */
    const int before_if_state_number = push_copy_of_current_state(ctx, "before-if");

    declarator_array_set_objects_to_true_branch(ctx, &declarator_array, nullable_enabled);
    if (p_selection_statement->secondary_block)
    {
        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);
    }


    const bool true_branch_ends_with_jump =
        secondary_block_ends_with_jump(p_selection_statement->secondary_block);


    /*let's make a copy of the state we left true branch*/
    //const int true_branch = 1;
    const int left_true_branch_state_number = push_copy_of_current_state(ctx, "left-true-branch");

    ctx_object_restore_current_state_from(ctx, before_if_state_number);


    if (p_selection_statement->else_secondary_block_opt)
    {
        declarator_array_set_objects_to_false_branch(ctx, &declarator_array, nullable_enabled);
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

            ctx_object_restore_current_state_from(ctx, left_true_branch_state_number);
        }
        else
        {
            /*
               if (){
               }
               else {

               }
            */

            ctx_object_merge_current_state_with_state_number(ctx, left_true_branch_state_number);
            ctx_object_restore_current_state_from(ctx, left_true_branch_state_number);
        }
    }

    check_defer_and_variables(ctx, p_defer, p_selection_statement->last_token);
    flow_visit_ctx_pop_tail_block(ctx);

    ctx_remove_state(ctx, before_if_state_number);
    ctx_remove_state(ctx, left_true_branch_state_number);
    declarator_array_destroy(&declarator_array);
}

static void flow_visit_block_item(struct flow_visit_ctx* ctx, struct block_item* p_block_item);


static void flow_visit_try_statement(struct flow_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    const int throw_join_state_old = ctx->throw_join_state;
    struct secondary_block* catch_secondary_block_old = ctx->catch_secondary_block_opt;


    ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;

    ctx->throw_join_state = ctx_add_empty_state(ctx, "try");

    const int original_state_number = push_copy_of_current_state(ctx, "original");

    struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
    p_defer->p_try_statement = p_try_statement;

    if (p_try_statement->secondary_block)
    {
        flow_visit_secondary_block(ctx, p_try_statement->secondary_block);
        ctx_object_set_state_from_current(ctx, original_state_number); //state of end of secondary block
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
        ctx_object_merge_current_state_with_state_number_or(ctx, original_state_number);
        ctx_object_restore_current_state_from(ctx, original_state_number);
    }
    else if (try_reached_the_end)
    {
        ctx_object_restore_current_state_from(ctx, original_state_number);
    }
    else if (catch_reached_the_end)
    {
        //ctx_object_restore_current_state_from(ctx, orignial);       
    }


    check_defer_and_variables(ctx, p_defer, p_try_statement->secondary_block->last_token);


    flow_visit_ctx_pop_tail_block(ctx);

    ctx_remove_state(ctx, original_state_number);
    ctx_remove_state(ctx, ctx->throw_join_state);

    ctx->throw_join_state = throw_join_state_old; //restore
    ctx->catch_secondary_block_opt = catch_secondary_block_old; //restore
}

static void flow_visit_switch_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = push_copy_of_current_state(ctx, "original");
    ctx->break_join_state = ctx_add_empty_state(ctx, "break join");

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
    //pop_states(ctx, 2);

    ctx_remove_state(ctx, ctx->initial_state);
    ctx_remove_state(ctx, ctx->break_join_state);

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
        struct declarator_array a = { 0 };
        flow_visit_expression(ctx, p_initializer->assignment_expression, &a);
        declarator_array_destroy(&a);
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
        struct declarator_array a = { 0 };
        flow_visit_expression(ctx, p_argument_expression->expression, &a);
        p_argument_expression = p_argument_expression->next;
        declarator_array_destroy(&a);
    }
}

static void flow_visit_generic_selection(struct flow_visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        struct declarator_array a = { 0 };
        flow_visit_expression(ctx, p_generic_selection->expression, &a);
        declarator_array_destroy(&a);
    }
    else if (p_generic_selection->type_name)
    {
        flow_visit_type_name(ctx, p_generic_selection->type_name);
    }
}


static void compare_function_arguments3(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct argument_expression_list* p_argument_expression_list)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct param* p_current_parameter_type = NULL;

    const struct param_list* p_param_list = type_get_func_or_func_ptr_params(p_type);

    if (p_param_list)
    {
        p_current_parameter_type = p_param_list->head;
    }


    struct argument_expression* p_current_argument = p_argument_expression_list->head;

    while (p_current_argument && p_current_parameter_type)
    {

        struct object* p_argument_object =
            expression_get_object(ctx, p_current_argument->expression, nullable_enabled);

        if (p_argument_object)
        {
            struct object* parameter_object = make_object(ctx, &p_current_parameter_type->type, NULL, p_current_argument->expression);
            object_set_uninitialized(&p_current_parameter_type->type, parameter_object);
            flow_assignment(ctx,
              p_current_argument->expression->first_token,
              ASSIGMENT_TYPE_PARAMETER,
              true,
              type_is_view(&p_current_parameter_type->type),
              type_is_nullable(&p_current_parameter_type->type, ctx->ctx->options.null_checks_enabled),
              &p_current_parameter_type->type,
              parameter_object, /*dest object*/

              &p_current_argument->expression->type,
              p_argument_object
            );

            //object_destroy(&parameter_object);

        }
        p_current_argument = p_current_argument->next;
        p_current_parameter_type = p_current_parameter_type->next;
        //object_destroy(&temp_obj1);
    }

    while (p_current_argument)
    {
        /*
           We have more argument than parameters, this happens with variadic functions
        */


        struct object* p_argument_object =
            expression_get_object(ctx, p_current_argument->expression, nullable_enabled);
        if (p_argument_object)
        {
            checked_read_object(ctx,
                &p_current_argument->expression->type,
                type_is_nullable(&p_current_argument->expression->type, ctx->ctx->options.null_checks_enabled),
                p_argument_object,
                p_current_argument->expression->first_token,
                false);
        }
        else
        {
            //
        }
        p_current_argument = p_current_argument->next;
        //object_destroy(&temp_obj);
    }

}

static void check_uninitialized(struct flow_visit_ctx* ctx, struct expression* p_expression)
{
    if (p_expression->is_assigment_expression)
        return;

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;


    struct object* p_object = expression_get_object(ctx, p_expression, nullable_enabled);

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
    //object_destroy(&temp_obj);
}

void object_push_states_from(const struct object* p_object_from, struct object* p_object_to)
{
    for (int i = 0; i < p_object_from->object_state_stack.size; i++)
    {
        object_state_stack_push_back(
            &p_object_to->object_state_stack,
            p_object_to->state,
            &p_object_to->pointed,
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

        struct object* p_object = expression_get_object(ctx, p_expression, nullable_enabled);
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
        //object_destroy(&temp);
    }
}

static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression, struct declarator_array* d)
{
    if (p_expression == NULL)
        return;
    declarator_array_clear(d); //out

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
    {
        struct declarator_array_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        declarator_array_push_back(d, &item);
        check_uninitialized(ctx, p_expression);
    }
    break;

    case PRIMARY_EXPRESSION_PARENTESIS:
        flow_visit_expression(ctx, p_expression->right, d);
        break;

    case PRIMARY_EXPRESSION_STRING_LITERAL:
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
    case PRIMARY_EXPRESSION_NUMBER:
    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        flow_visit_generic_selection(ctx, p_expression->generic_selection);
        break;

    case POSTFIX_DOT:
    {
        struct declarator_array left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);
        struct declarator_array right_set = { 0 };
        flow_visit_expression(ctx, p_expression->right, &right_set);

        declarator_array_destroy(&left_set);
        declarator_array_destroy(&right_set);


        struct declarator_array_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        declarator_array_push_back(d, &item);
    }
    break;

    case POSTFIX_ARROW:
    {
        struct declarator_array left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);
        declarator_array_destroy(&left_set);

        struct object* p_object = expression_get_object(ctx, p_expression->left, nullable_enabled);

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
        }

        struct declarator_array_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        declarator_array_push_back(d, &item);
    }
    break;

    case POSTFIX_INCREMENT:
        break;
    case POSTFIX_DECREMENT:
        break;
    case POSTFIX_ARRAY:
    {
        flow_visit_expression(ctx, p_expression->left, d);
        flow_visit_expression(ctx, p_expression->right, d);
        declarator_array_clear(d);

        struct declarator_array_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        declarator_array_push_back(d, &item);

    }
    break;

    case POSTFIX_FUNCTION_CALL:

        flow_visit_expression(ctx, p_expression->left, d);

        flow_visit_argument_expression_list(ctx, &p_expression->argument_expression_list);
        //new function waiting all test to pass to become active
        compare_function_arguments3(ctx, &p_expression->left->type, &p_expression->argument_expression_list);

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

        struct object* temp2 = make_object(ctx, &p_expression->type, NULL, p_expression);
        object_swap(temp2, p_expression->type_name->declarator->p_object);
        //object_destroy(&temp2);

        //TODO the state of object depends of the initializer
        set_direct_state(&p_expression->type, p_expression->type_name->declarator->p_object, OBJECT_STATE_ZERO);


        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);

        break;

    case UNARY_EXPRESSION_ALIGNOF:

        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_expression->right, d);
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
            struct declarator_array declarator_array4 = { 0 };
            flow_visit_expression(ctx, p_expression->right, &declarator_array4); //assert(p == 0);
            declarator_array_set_objects_to_true_branch(ctx, &declarator_array4, nullable_enabled);
            declarator_array_destroy(&declarator_array4);
        }

        break;

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:

        if (p_expression->right)
        {
            const bool t2 = ctx->expression_is_not_evaluated;
            ctx->expression_is_not_evaluated = true;
            flow_visit_expression(ctx, p_expression->right, d);
            ctx->expression_is_not_evaluated = t2;
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            flow_visit_type_name(ctx, p_expression->type_name);
        }


        break;

    case UNARY_EXPRESSION_NOT:
        flow_visit_expression(ctx, p_expression->right, d);
        declarator_array_invert(d);
        break;

    case UNARY_EXPRESSION_SIZEOF_TYPE:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:

    case UNARY_EXPRESSION_BITNOT:
    case UNARY_EXPRESSION_NEG:
    case UNARY_EXPRESSION_PLUS:

    case UNARY_EXPRESSION_ADDRESSOF:
    {
        if (p_expression->right)
        {
            check_uninitialized(ctx, p_expression->right);

            struct object* p_object2 = expression_get_object(ctx, p_expression->right, nullable_enabled);

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

            flow_visit_expression(ctx, p_expression->right, d);
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            flow_visit_type_name(ctx, p_expression->type_name);
        }
    }
    break;

    case UNARY_EXPRESSION_CONTENT:
    {
        struct object* p_object0 = expression_get_object(ctx, p_expression->right, nullable_enabled);

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


        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_expression->right, d);
        }
    }
    break;


    case ASSIGNMENT_EXPRESSION:
    {
        struct declarator_array left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);
        declarator_array_swap(d, &left_set);
        declarator_array_destroy(&left_set);

        struct declarator_array right_set = { 0 };
        flow_visit_expression(ctx, p_expression->right, &right_set);
        declarator_array_destroy(&right_set);

        //struct object temp_obj1 = { 0 };
        struct object* const p_right_object = expression_get_object(ctx, p_expression->right, nullable_enabled);

        //struct object temp_obj2 = { 0 };
        struct object* const p_dest_object = expression_get_object(ctx, p_expression->left, nullable_enabled);

        flow_assignment(ctx,
            p_expression->left->first_token,
            ASSIGMENT_TYPE_OBJECTS,
            true,
            type_is_view(&p_expression->left->type), /*dest type*/
            type_is_nullable(&p_expression->left->type, ctx->ctx->options.null_checks_enabled), /*dest type*/
            &p_expression->left->type, /*dest type*/
            p_dest_object, /*dest object*/
            &p_expression->right->type, /*source type*/
            p_right_object /*source*/);

        /*
          built-in malloc, calloc assignment
        */
        if (expression_is_malloc(p_expression->right))
        {
            struct type t = type_remove_pointer(&p_expression->left->type);
            struct object* po = make_object(ctx, &t, NULL, p_expression->left);
            object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
        }
        else if (expression_is_calloc(p_expression->right))
        {
            struct type t = type_remove_pointer(&p_expression->left->type);
            struct object* po = make_object(ctx, &t, NULL, p_expression->left);
            object_set_zero(&t, po);
            object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
        }

        //object_destroy(&temp_obj1);
        //object_destroy(&temp_obj2);
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
    {
        if (p_expression->left)
        {
            struct declarator_array left_set = { 0 };
            flow_visit_expression(ctx, p_expression->left, &left_set);
            declarator_array_destroy(&left_set);
        }

        if (p_expression->right)
        {
            struct declarator_array right_set = { 0 };
            flow_visit_expression(ctx, p_expression->right, &right_set);
            declarator_array_destroy(&right_set);
        }
    }
    break;
    case EQUALITY_EXPRESSION_EQUAL:
    {
        const int left_value = constant_value_is_valid(&p_expression->left->constant_value) ?
            constant_value_to_ull(&p_expression->left->constant_value) :
            -1;

        const int right_value = constant_value_is_valid(&p_expression->right->constant_value) ?
            constant_value_to_ull(&p_expression->right->constant_value) :
            -1;

        if (left_value == 0 || left_value == 1)
        {
            struct declarator_array true_false_set_right = { 0 };
            flow_visit_expression(ctx, p_expression->right, &true_false_set_right);
            //0 == expression            
            //1 == expression            
            declarator_array_swap(d, &true_false_set_right);
            if (left_value == 0)
            {
                declarator_array_invert(d);
            }
            declarator_array_destroy(&true_false_set_right);
        }

        else if (right_value == 0 || right_value == 1)
        {
            struct declarator_array true_false_set_left3 = { 0 };
            flow_visit_expression(ctx, p_expression->left, &true_false_set_left3);

            //expression == 0
            //expression == 1
            declarator_array_swap(d, &true_false_set_left3);
            if (right_value == 0)
            {
                declarator_array_invert(d);
            }
            declarator_array_destroy(&true_false_set_left3);
        }
        else
        {
        }
    }
    break;

    case EQUALITY_EXPRESSION_NOT_EQUAL:
    {
        const int left_value = constant_value_is_valid(&p_expression->left->constant_value) ?
            constant_value_to_ull(&p_expression->left->constant_value) :
            -1;
        const int right_value = constant_value_is_valid(&p_expression->right->constant_value) ?
            constant_value_to_ull(&p_expression->right->constant_value) :
            -1;

        if (left_value == 0 || left_value == 1)
        {
            struct declarator_array true_false_set_right = { 0 };
            flow_visit_expression(ctx, p_expression->right, &true_false_set_right);
            //0 != expression            
            //1 != expression            
            declarator_array_swap(d, &true_false_set_right);
            if (left_value == 1)
            {
                declarator_array_invert(d);
            }
            declarator_array_destroy(&true_false_set_right);
        }

        else if (right_value == 0 || right_value == 1)
        {
            struct declarator_array true_false_set_left2 = { 0 };
            flow_visit_expression(ctx, p_expression->left, &true_false_set_left2);

            //expression != 0
            //expression != 1
            declarator_array_swap(d, &true_false_set_left2);
            if (right_value == 1)
            {
                declarator_array_invert(d);
            }
            declarator_array_destroy(&true_false_set_left2);
        }
        else
        {

        }
    }
    break;

    case LOGICAL_OR_EXPRESSION:
    {
        flow_check_pointer_used_as_bool(ctx, p_expression->left);
        flow_check_pointer_used_as_bool(ctx, p_expression->right);


        struct declarator_array left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);

        const int original_state_number = push_copy_of_current_state(ctx, "original");


        //Set all variables to false state, because otherwise, the right branch
        // would not be evaluated
        declarator_array_set_objects_to_false_branch(ctx, &left_set, nullable_enabled);

        struct declarator_array right_set = { 0 };
        flow_visit_expression(ctx, p_expression->right, &right_set);
        ctx_object_restore_current_state_from(ctx, original_state_number);

        //Tudo que faz left ser true ou right ser true

        for (int i = 0; i < left_set.size; i++)
        {
            struct declarator_array_item item5;

            item5.p_expression = left_set.data[i].p_expression;
            item5.true_branch_state |= (left_set.data[i].true_branch_state | left_set.data[i].false_branch_state);
            item5.false_branch_state |= left_set.data[i].false_branch_state;
            declarator_array_push_back(d, &item5);
        }

        for (int k = 0; k < right_set.size; k++)
        {
            int index =
                find_item_index_by_expression(d, right_set.data[k].p_expression);
            if (index == -1)
            {
                index = d->size;
                struct declarator_array_item item4 = { 0 };
                declarator_array_push_back(d, &item4);
            }

            //Tudo que faz left true e right true faz expressao se true
            d->data[index].p_expression = right_set.data[k].p_expression;
            //d->data[index].true_branch_state |= right_set.data[k].true_branch_state;
            //Tudo que faz left true ou left false, e right false faz ser false
            d->data[index].false_branch_state |= right_set.data[k].false_branch_state;

            //No path true seria possivel nao ser feito o right
            d->data[index].true_branch_state |= (BOOLEAN_FLAG_TRUE | BOOLEAN_FLAG_FALSE);

        }

        ctx_remove_state(ctx, original_state_number);
        declarator_array_destroy(&left_set);
        declarator_array_destroy(&right_set);
    }
    break;

    case LOGICAL_AND_EXPRESSION:
    {
        //flow_check_pointer_used_as_bool(ctx, p_expression->left);
        //flow_check_pointer_used_as_bool(ctx, p_expression->right);

        struct declarator_array left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);

        const int original_state_number = push_copy_of_current_state(ctx, "original");


        //Set all variables to true state, because otherwise, the right branch
        // would not be evaluated
        declarator_array_set_objects_to_true_branch(ctx, &left_set, nullable_enabled);

        struct declarator_array right_set = { 0 };
        flow_visit_expression(ctx, p_expression->right, &right_set);
        ctx_object_restore_current_state_from(ctx, original_state_number);

        //Tudo que faz left_true e right_true  também fazem esta expressao ser true

        for (int i = 0; i < left_set.size; i++)
        {
            struct declarator_array_item item3 = { 0 };

            //Tudo que faz left true e right true faz expressao se true
            item3.p_expression = left_set.data[i].p_expression;
            item3.true_branch_state |= left_set.data[i].true_branch_state;

            //Tudo que faz left true ou left false, e right false faz ser false
            item3.false_branch_state |= left_set.data[i].true_branch_state |
                left_set.data[i].false_branch_state;

            declarator_array_push_back(d, &item3);
        }

        for (int k = 0; k < right_set.size; k++)
        {
            int index =
                find_item_index_by_expression(d, right_set.data[k].p_expression);
            if (index == -1)
            {
                index = d->size;
                struct declarator_array_item item2 = { 0 };
                declarator_array_push_back(d, &item2);
            }

            //Tudo que faz left true e right true faz expressao se true
            d->data[index].p_expression = right_set.data[k].p_expression;
            d->data[index].true_branch_state |= right_set.data[k].true_branch_state;
            //Tudo que faz left true ou left false, e right false faz ser false
            d->data[index].false_branch_state |= right_set.data[k].false_branch_state;

            //right expression may not be evaluated, in this case all previous states are also valid
            //so if the variable could be true and false then we need to add            
            d->data[index].false_branch_state |= (BOOLEAN_FLAG_TRUE | BOOLEAN_FLAG_FALSE);

        }

        ctx_remove_state(ctx, original_state_number);
        declarator_array_destroy(&left_set);
        declarator_array_destroy(&right_set);
    }
    break;

    case AND_EXPRESSION:
    case EXCLUSIVE_OR_EXPRESSION:

    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:

        if (p_expression->left)
        {
            flow_visit_expression(ctx, p_expression->left, d);
        }
        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_expression->right, d);
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
        //TODO
        if (p_expression->condition_expr)
        {
            flow_visit_expression(ctx, p_expression->condition_expr, d);
        }

        if (p_expression->left)
        {
            flow_visit_expression(ctx, p_expression->left, d);
        }
        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_expression->right, d);
        }

        break;

    default:
        break;
    }
}

static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    struct declarator_array d = { 0 };
    if (p_expression_statement->expression_opt)
        flow_visit_expression(ctx, p_expression_statement->expression_opt, &d);
    declarator_array_destroy(&d);

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
        //compute_true_false_sets(p_iteration_statement->expression1, &declarator_array);
        flow_visit_expression(ctx, p_iteration_statement->expression1, &declarator_array);
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
    declarator_array_destroy(&declarator_array);
}

static void flow_visit_while_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = push_copy_of_current_state(ctx, "original");
    ctx->break_join_state = ctx_add_empty_state(ctx, "break join");


    struct declarator_array declarator_array = { 0 };


    if (p_iteration_statement->expression1)
    {
        //compute_true_false_sets(p_iteration_statement->expression1, &declarator_array);

        //We do a visit but this is not conclusive..so we ignore warnings
        ctx->ctx->options.diagnostic_stack_top_index++;
        ctx->ctx->options.diagnostic_stack[ctx->ctx->options.diagnostic_stack_top_index].warnings = 0;
        ctx->ctx->options.diagnostic_stack[ctx->ctx->options.diagnostic_stack_top_index].errors = 0;
        ctx->ctx->options.diagnostic_stack[ctx->ctx->options.diagnostic_stack_top_index].notes = 0;
        flow_visit_expression(ctx, p_iteration_statement->expression1, &declarator_array);
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


        //pop_states(ctx, 2);
        flow_visit_ctx_pop_tail_block(ctx);
    }

    ctx_remove_state(ctx, ctx->initial_state);
    ctx_remove_state(ctx, ctx->break_join_state);

    //Now we visit the expression again because we have the states
    //at end of while that will be used again for the expression.
    //At this time we print warnings
    struct declarator_array declarator_array2 = { 0 };
    flow_visit_expression(ctx, p_iteration_statement->expression1, &declarator_array2);

    //restore
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
    declarator_array_destroy(&declarator_array);
    declarator_array_destroy(&declarator_array2);

}

static void flow_visit_for_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);


    struct declarator_array d = { 0 };
    if (p_iteration_statement->expression0)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression0, &d);
    }

    if (p_iteration_statement->expression1)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression1, &d);
    }
    if (p_iteration_statement->expression2)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression2, &d);
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

    declarator_array_destroy(&d);
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
            struct declarator_array d = { 0 };
            flow_visit_expression(ctx, p_jump_statement->expression_opt, &d);
            declarator_array_destroy(&d);
        }

        /*
          returning a declarator will move the onwership
        */
        if (p_jump_statement->expression_opt)
        {
            struct object* p_object =
                expression_get_object(ctx, p_jump_statement->expression_opt, nullable_enabled);

            if (p_object)
            {
                struct object* p_dest_object =
                    make_object(ctx, ctx->p_return_type, NULL, p_jump_statement->expression_opt);

                object_set_zero(ctx->p_return_type, p_dest_object);

                flow_assignment(ctx,
                 p_jump_statement->expression_opt->first_token,
                 ASSIGMENT_TYPE_RETURN,
                 true,
                    type_is_view(ctx->p_return_type), /*dest type*/
                    type_is_nullable(ctx->p_return_type, ctx->ctx->options.null_checks_enabled), /*dest type*/
                    ctx->p_return_type, /*dest type*/
                    p_dest_object, /*dest object*/
                    &p_jump_statement->expression_opt->type, /*source type*/
                    p_object /*source*/
                );

                p_dest_object->state = OBJECT_STATE_LIFE_TIME_ENDED;
            }

            if (p_object && p_object->is_temporary)
            {
                //a + b
                p_object->state = OBJECT_STATE_LIFE_TIME_ENDED;
            }

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
        int label_state_number = -1;
        for (int i = 0; i < ctx->labels_size; i++)
        {
            if (ctx->labels[i].state_number == label_state_number)
            {
                break; //already exist
            }
        }
        if (label_state_number == -1)
        {
            label_state_number = ctx_add_empty_state(ctx, p_jump_statement->label->lexeme);
            ctx->labels[ctx->labels_size].state_number = label_state_number;
            ctx->labels[ctx->labels_size].label_name = p_jump_statement->label->lexeme;
            ctx->labels_size++;
        }
        ctx_object_merge_current_state_with_state_number(ctx, label_state_number);
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

    if (p_label->name)
    {
        for (int i = 0; i < ctx->labels_size; i++)
        {
            if (strcmp(ctx->labels[i].label_name, p_label->name->lexeme) == 0)
            {
                ctx_object_restore_current_state_from(ctx, ctx->labels[i].state_number);
                break; //already exist
            }
        }
    }
    else
    {
        //case, default
        ctx_object_restore_current_state_from(ctx, ctx->initial_state);
    }
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

    struct declarator_array a = { 0 };
    flow_visit_expression(ctx, p_static_assert_declaration->constant_expression, &a);

    ctx->expression_is_not_evaluated = t2; //restore


    if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG ||
        p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG_EX)
    {
        bool ex = p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG_EX;

        compiler_diagnostic_message(W_LOCATION, ctx->ctx, p_static_assert_declaration->first_token, "static_debug");


        struct object* p_obj =
            expression_get_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);

        if (p_obj)
        {
            print_object(&p_static_assert_declaration->constant_expression->type, p_obj, !ex);
        }

        if (p_obj->is_temporary)
        {
            p_obj->state = OBJECT_STATE_LIFE_TIME_ENDED;
        }

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

            struct object* p_obj =
                expression_get_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);
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

            if (p_obj && p_obj->is_temporary)
            {
                p_obj->state = OBJECT_STATE_LIFE_TIME_ENDED;
            }

        }
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_SET)
    {


        struct object* p_obj =
            expression_get_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);

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
        if (p_obj->is_temporary)
        {
            p_obj->state = OBJECT_STATE_LIFE_TIME_ENDED;
        }

    }

    declarator_array_destroy(&a);
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
            struct declarator_array a = { 0 };
            flow_visit_expression(ctx, p_direct_declarator->array_declarator->assignment_expression, &a);
            declarator_array_destroy(&a);
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


        p_declarator->p_object = make_object(ctx, &p_declarator->type, p_declarator, NULL);
        object_set_uninitialized(&p_declarator->type, p_declarator->p_object);


        if (p_declarator->declaration_specifiers &&
            p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            if (type_is_pointer(&p_declarator->type))
            {
                if (type_is_nullable(&p_declarator->type, ctx->ctx->options.null_checks_enabled))
                {
                    p_declarator->p_object->state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
                }
                else
                {
                    p_declarator->p_object->state = OBJECT_STATE_NOT_NULL;
                }

                if (type_is_pointer_to_out(&p_declarator->type))
                {
                    struct type t = type_remove_pointer(&p_declarator->type);
                    struct object* po = make_object(ctx, &t, p_declarator, NULL);
                    object_set_uninitialized(&t, po);
                    object_set_pointer(p_declarator->p_object, po); //MOVED                    
                    type_destroy(&t);
                }
                else if (type_is_any_owner(&p_declarator->type))
                {
                    struct type t = type_remove_pointer(&p_declarator->type);
                    struct object* po = make_object(ctx, &t, p_declarator, NULL);
                    const bool t_is_nullable = type_is_nullable(&t, ctx->ctx->options.null_checks_enabled);
                    object_set_unknown(&t, t_is_nullable, po, nullable_enabled);
                    object_set_pointer(p_declarator->p_object, po); //MOVED                    
                    type_destroy(&t);
                }
            }
            else if (type_is_struct_or_union(&p_declarator->type))
            {
                const bool is_nullable = type_is_nullable(&p_declarator->type, nullable_enabled);
                object_set_unknown(&p_declarator->type, is_nullable, p_declarator->p_object, nullable_enabled);
            }
            else if (type_is_array(&p_declarator->type))
            {
                // assert(false);//TODO
                 //object_set_unknown(&p_declarator->type, &p_declarator->object);
                p_declarator->p_object->state = OBJECT_STATE_NOT_ZERO;
            }
            else
            {
                p_declarator->p_object->state = OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO;
            }


#if 0
            if (type_is_pointer(&p_declarator->type))
            {
                //TODO necessary?
                struct type t2 = type_remove_pointer(&p_declarator->type);
                if (p_declarator->p_object->pointed)
                {
                    set_object(&t2, p_declarator->p_object->pointed, (OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL));
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
    struct declarator_array a = { 0 };
    if (p_enumerator->constant_expression_opt)
        flow_visit_expression(ctx, p_enumerator->constant_expression_opt, &a);
    declarator_array_destroy(&a);

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
    ctx->labels_size = 0;
    objects_clear(&ctx->arena);


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
    objects_destroy(&p->arena);
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

