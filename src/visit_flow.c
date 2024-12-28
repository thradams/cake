/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "ownership.h"

#include <assert.h>
#include <string.h>
#include <assert.h>
#include "visit_flow.h"
#include "expressions.h"
#include "ownership.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>


void flow_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration);


static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct secondary_block* p_secondary_block);
static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
struct true_false_set;
static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression, struct true_false_set* a);
static void flow_visit_statement(struct flow_visit_ctx* ctx, struct statement* p_statement);
static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct type_specifier* p_type_specifier);
static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);
static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct expression_statement* p_expression_statement);

enum boolean_flag
{
    BOOLEAN_FLAG_NONE = 0,
    BOOLEAN_FLAG_TRUE = 1 << 0,
    BOOLEAN_FLAG_FALSE = 1 << 1,
};

struct true_false_set_item
{
    struct expression* p_expression;
    enum boolean_flag true_branch_state;
    enum boolean_flag false_branch_state;
};

struct true_false_set
{
    struct true_false_set_item* _Owner _Opt data;
    int size;
    int capacity;
};

static void true_false_set_clear(struct true_false_set* p)
{
    free(p->data);
    p->data = NULL;
    p->size = 0;
    p->capacity = 0;
}

enum merge_options
{
    MERGE_OPTIONS_A_TRUE = 1 << 0,
    MERGE_OPTIONS_A_FALSE = 1 << 1,
    MERGE_OPTIONS_B_TRUE = 1 << 2,
    MERGE_OPTIONS_B_FALSE = 1 << 3
};

void true_false_set_destroy(struct true_false_set* _Obj_owner p)
{
    free(p->data);
}

static int true_false_set_reserve(struct true_false_set* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL)
            return ENOMEM;
        static_set(p->data, "moved");
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

static int true_false_set_push_back(struct true_false_set* p, const struct true_false_set_item* book)
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

        int error = true_false_set_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = *book; /*COPIED*/


    p->size++;

    return 0;
}

static void true_false_set_invert(struct true_false_set* true_false_set)
{
    for (int i = 0; i < true_false_set->size; i++)
    {
        enum boolean_flag temp = true_false_set->data[i].true_branch_state;
        true_false_set->data[i].true_branch_state = true_false_set->data[i].false_branch_state;
        true_false_set->data[i].false_branch_state = temp;
    }
}

static void true_false_set_swap(struct true_false_set* a, struct true_false_set* b)
{
    struct true_false_set temp = *a;
    *a = *b;
    *b = temp;
}

static int find_item_index_by_expression(const struct true_false_set* a, const struct expression* p_expression)
{
    for (int i = 0; i < a->size; i++)
    {
        if (a->data[i].p_expression == p_expression)
            return i;
    }
    return -1;
}


//1
// 
//true true, false false
//true true, false false
static void true_false_set_merge(struct true_false_set* result,
    struct true_false_set* a,
    struct true_false_set* b,
    enum merge_options options_true,
    enum merge_options options_false)
{

    for (int i = 0; i < a->size; i++)
    {
        const struct true_false_set_item* p_item_a = &a->data[i];

        _Opt struct true_false_set_item new_item = { 0 };
        new_item.p_expression = p_item_a->p_expression;

        if (options_true & MERGE_OPTIONS_A_TRUE)
            new_item.true_branch_state |= p_item_a->true_branch_state;

        if (options_true & MERGE_OPTIONS_A_FALSE)
            new_item.true_branch_state |= p_item_a->false_branch_state;

        if (options_false & MERGE_OPTIONS_A_TRUE)
            new_item.false_branch_state |= p_item_a->true_branch_state;

        if (options_false & MERGE_OPTIONS_A_FALSE)
            new_item.true_branch_state |= p_item_a->false_branch_state;


        true_false_set_push_back(result, &new_item);
    }

    for (int k = 0; k < b->size; k++)
    {
        const struct true_false_set_item* p_item_b = &b->data[k];

        int index = find_item_index_by_expression(result, p_item_b->p_expression);
        if (index == -1)
        {
            index = result->size;
            _Opt struct true_false_set_item item2 = { 0 };
            item2.p_expression = p_item_b->p_expression;
            true_false_set_push_back(result, &item2);
        }

        struct true_false_set_item* p_item_result = &result->data[index];

        if (options_true & MERGE_OPTIONS_B_TRUE)
            p_item_result->true_branch_state |= p_item_b->true_branch_state;

        if (options_true & MERGE_OPTIONS_B_FALSE)
            p_item_result->true_branch_state |= p_item_b->false_branch_state;

        if (options_false & MERGE_OPTIONS_B_TRUE)
            p_item_result->false_branch_state |= p_item_b->true_branch_state;

        if (options_false & MERGE_OPTIONS_B_FALSE)
            p_item_result->true_branch_state |= p_item_b->false_branch_state;
    }
}

static void true_false_set_set_objects_to_core_branch(struct flow_visit_ctx* ctx,
    struct true_false_set* a,
    bool nullable_enabled,
    bool true_branch)
{
    for (int i = 0; i < a->size; i++)
    {
        if (a->data[i].p_expression != NULL)
        {

            struct flow_object* _Opt p_object =
                expression_get_flow_object(ctx, a->data[i].p_expression, nullable_enabled);

            if (p_object)
            {
                if (p_object->current.state == FLOW_OBJECT_STATE_NOT_NULL ||
                    p_object->current.state == FLOW_OBJECT_STATE_NULL ||
                    p_object->current.state == FLOW_OBJECT_STATE_MOVED ||
                    p_object->current.state == FLOW_OBJECT_STATE_ZERO ||
                    p_object->current.state == FLOW_OBJECT_STATE_NOT_ZERO ||
                    p_object->current.state == FLOW_OBJECT_STATE_LIFE_TIME_ENDED)
                {
                    continue;
                }

                const enum boolean_flag flag =
                    true_branch ?
                    a->data[i].true_branch_state :
                    a->data[i].false_branch_state;



                if ((flag & BOOLEAN_FLAG_TRUE) && (flag & BOOLEAN_FLAG_FALSE))
                {
                }
                else if (flag & BOOLEAN_FLAG_FALSE)
                {
                    p_object->current.state &= ~FLOW_OBJECT_STATE_NOT_NULL;
                    p_object->current.state &= ~FLOW_OBJECT_STATE_MOVED;
                }
                else if (flag & BOOLEAN_FLAG_TRUE)
                {
                    p_object->current.state &= ~FLOW_OBJECT_STATE_NULL;
                    p_object->current.state &= ~FLOW_OBJECT_STATE_ZERO;

                }


            }
        }
    }
}

static void true_false_set_set_objects_to_true_branch(struct flow_visit_ctx* ctx, struct true_false_set* a, bool nullable_enabled)
{
    true_false_set_set_objects_to_core_branch(ctx, a, nullable_enabled, true);
}

static void true_false_set_set_objects_to_false_branch(struct flow_visit_ctx* ctx, struct true_false_set* a, bool nullable_enabled)
{
    true_false_set_set_objects_to_core_branch(ctx, a, nullable_enabled, false);
}

static int arena_add_copy_of_current_state(struct flow_visit_ctx* ctx, const char* name);


static void arena_remove_state(struct flow_visit_ctx* ctx, int state_number);

struct visit_objects {
    struct flow_defer_scope* current_block;
    struct flow_defer_scope* next_child;
};


static void flow_exit_block_visit_defer_item(struct flow_visit_ctx* ctx,
    struct defer_list_item* p_item,
    struct token* position_token)
{
    if (p_item->defer_statement)
    {
        const int error_count = ctx->ctx->p_report->error_count;
        const int warnings_count = ctx->ctx->p_report->warnings_count;
        const int info_count = ctx->ctx->p_report->info_count;

        flow_visit_secondary_block(ctx, p_item->defer_statement->secondary_block);

        if (error_count != ctx->ctx->p_report->error_count ||
            warnings_count != ctx->ctx->p_report->warnings_count ||
            info_count != ctx->ctx->p_report->info_count)
        {
            compiler_diagnostic_message(W_LOCATION, ctx->ctx, position_token, NULL, "defer end of scope");
        }
    }
    else if (p_item->declarator)
    {
        struct declarator* p_declarator = p_item->declarator;
        const char* name = p_declarator->name_opt ? p_declarator->name_opt->lexeme : "?";

        if (p_declarator->p_flow_object)
        {
            flow_end_of_block_visit(ctx,
                &p_declarator->type,
                type_is_view(&p_declarator->type),
                p_declarator->p_flow_object,
                position_token,
                name);
        }
        else
        {
            assert(0);
        }
    }
}

static void flow_exit_block_visit_defer_list(struct flow_visit_ctx* ctx,
    struct defer_list* p_defer_list,
    struct token* position_token)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow_exit_block_visit_defer_item(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow_defer_item_set_end_of_lifetime(struct flow_visit_ctx* ctx,
    struct defer_list_item* p_item,
    struct token* position_token)
{
    if (p_item->defer_statement)
    {
        //?
    }
    else if (p_item->declarator)
    {
        struct declarator* p_declarator = p_item->declarator;
        //const char* name = p_declarator->name_opt ? p_declarator->name_opt->lexeme : "?";

        if (p_declarator->p_flow_object)
        {
            flow_object_set_end_of_lifetime(&p_declarator->type, p_declarator->p_flow_object);
        }
        else
        {
            assert(0);
        }
    }
}

static void flow_defer_list_set_end_of_lifetime(struct flow_visit_ctx* ctx,
    struct defer_list* p_defer_list,
    struct token* position_token)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow_defer_item_set_end_of_lifetime(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    flow_visit_statement(ctx, p_secondary_block->statement);
}

static void flow_visit_defer_statement(struct flow_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{
    /*
      We are not going to visit the secondary block here because
      this is not the place were defer is executed.
    */
}

NODISCARD
static int arena_add_copy_of_current_state(struct flow_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow_object_add_new_state_as_a_copy_of_current_state(ctx->arena.data[i], name, state_number);
    }
    return state_number;
}

static int arena_add_empty_state(struct flow_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;

    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p_object = ctx->arena.data[i];
        _Opt struct flow_object_state* _Owner _Opt p_flow_object_state = calloc(1, sizeof * p_flow_object_state);
        if (p_flow_object_state)
        {
            p_flow_object_state->dbg_name = name;
            p_flow_object_state->state_number = state_number;
            flow_object_add_state(p_object, p_flow_object_state);
        }

    }
    return state_number;
}

static void flow_object_set_state_from_current(struct flow_object* object, int state_number)
{
    struct flow_object_state* _Opt p_flow_object_state = object->current.next;
    while (p_flow_object_state)
    {
        if (p_flow_object_state->state_number == state_number)
        {
            flow_object_state_copy(p_flow_object_state, &object->current);
            break;
        }
        p_flow_object_state = p_flow_object_state->next;
    }
}

static void arena_set_state_from_current(struct flow_visit_ctx* ctx, int number_state)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p_object = ctx->arena.data[i];
        flow_object_set_state_from_current(p_object, number_state);
    }
}

static int flow_object_merge_current_with_state(struct flow_visit_ctx* ctx, struct flow_object* object, int state_number)
{
    try
    {
        struct flow_object_state* _Opt it = object->current.next;
        while (it)
        {
            if (it->state_number == state_number)
            {
                it->state |= object->current.state;
                if (object->current.pointed == NULL && it->pointed == NULL)
                {
                    //nothing
                }
                else if (object->current.pointed == NULL && it->pointed != NULL)
                {
                    //mesmo
                }
                else if (object->current.pointed != NULL && it->pointed == NULL)
                {
                    it->pointed = object->current.pointed;
                }
                else if (object->current.pointed != it->pointed)
                {
                    assert(object->current.pointed != NULL);
                    assert(it->pointed != NULL);

                    struct flow_object* _Opt p_new_object = arena_new_object(ctx);
                    if (p_new_object == NULL) throw;

                    struct flow_object_state* _Owner _Opt p_new_state = calloc(1, sizeof * p_new_state);
                    if (p_new_state == NULL) throw;

                    p_new_state->dbg_name = "merged";
                    p_new_state->state_number = state_number;
                    objects_view_push_back(&p_new_state->alternatives, object->current.pointed);
                    objects_view_push_back(&p_new_state->alternatives, it->pointed);
                    flow_object_add_state(p_new_object, p_new_state);

                    int n_childs_1 = object->current.pointed->members.size;
                    int n_childs_2 = it->pointed->members.size;
                    if (n_childs_1 == n_childs_2)
                    {

                        for (int j = 0; j < n_childs_1; j++)
                        {
                            struct flow_object* _Opt p_new_child = arena_new_object(ctx);
                            if (p_new_child == NULL) throw;

                            p_new_child->parent = p_new_object;

                            struct flow_object* child1 = object->current.pointed->members.data[j];
                            struct flow_object* child2 = it->pointed->members.data[j];

                            struct flow_object_state* _Owner _Opt p_child_new_state = calloc(1, sizeof * p_child_new_state);
                            if (p_child_new_state == NULL) throw;

                            p_child_new_state->dbg_name = "merged";
                            p_child_new_state->state_number = state_number;
                            p_child_new_state->state = child1->current.state | it->state;
                            objects_view_push_back(&p_child_new_state->alternatives, child1);
                            objects_view_push_back(&p_child_new_state->alternatives, child2);
                            flow_object_add_state(p_new_child, p_child_new_state);
                            objects_view_push_back(&p_new_object->members, p_new_child);
                        }
                    }
                    else
                    {
                        //ops
                        //dbg_break();
                    }

                    it->pointed = p_new_object;
                }
                break;
            }
            it = it->next;
        }
    }
    catch
    {
    }
    return 0;
}
static void arena_merge_current_state_with_state_number(struct flow_visit_ctx* ctx, int number_state)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow_object_merge_current_with_state(ctx, ctx->arena.data[i], number_state);
    }
}

static void object_restore_current_state_from(struct flow_visit_ctx* ctx, struct flow_object* object, int state_number)
{
    struct flow_object_state* _Opt it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            flow_object_state_copy(&object->current, it);
            break;
        }
        it = it->next;
    }
}

static void arena_restore_current_state_from(struct flow_visit_ctx* ctx, int number_state)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        object_restore_current_state_from(ctx, ctx->arena.data[i], number_state);
    }
}

static void arena_remove_state(struct flow_visit_ctx* ctx, int state_number)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow_object_remove_state(ctx->arena.data[i], state_number);
    }
}

static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct initializer* p_initializer);
static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct declarator* p_declarator);

static void braced_initializer_set_object(struct braced_initializer* p, struct type* type, struct flow_object* object)
{
    if (p->initializer_list == NULL)
    {
    }
    //TODO currently it is zero

    flow_object_set_zero(type, object);
}

static void braced_initializer_flow_core(struct flow_visit_ctx* ctx, struct object* obj, struct flow_object* flow_obj)
{
    /*
       We use the obj to get the expression that initializes it.
    */

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (obj->p_init_expression)
    {
        struct flow_object* _Opt p_right_object =
            expression_get_flow_object(ctx, obj->p_init_expression, nullable_enabled);

        if (p_right_object)
        {
            struct marker a_marker = {
                .p_token_begin = NULL,
                .p_token_end = NULL,
            };

            struct marker b_marker = {
                .p_token_begin = obj->p_init_expression->first_token,
                .p_token_end = obj->p_init_expression->last_token,
            };

            flow_check_assignment(ctx,
                                obj->p_init_expression->first_token,
                                &a_marker,
                                &b_marker,
                                ASSIGMENT_TYPE_OBJECTS,
                                false,
                                type_is_view(&obj->type),
                                type_is_nullable(&obj->type, ctx->ctx->options.null_checks_enabled),
                                &obj->type,
                                flow_obj,
                                &obj->p_init_expression->type,
                                p_right_object,
                                NULL);
        }
    }
    else
    {
        /*default initialization*/
        flow_object_set_zero(&obj->type, flow_obj);
    }

    if (flow_obj->members.size > 0)
    {
        /*flow_object and object have the same number of members*/
        int i = 0;

        struct object* _Opt m = obj->members;
        while (m)
        {
            braced_initializer_flow_core(ctx, m, flow_obj->members.data[i]);
            m = m->next;
            i++;
        }
    }
}

static void braced_initializer_flow(struct flow_visit_ctx* ctx, struct object* obj, struct flow_object* flow_obj)
{
    try
    {
        braced_initializer_flow_core(ctx, obj, flow_obj);

        if (flow_obj->p_declarator_origin == NULL)
        {
            throw;
        }

        /*
           Let´s check if the object has been initialized correctly
        */

        bool is_nullable = type_is_nullable(&obj->type, ctx->ctx->options.null_checks_enabled);

        struct marker a_marker = {
                          .p_token_begin = flow_obj->p_declarator_origin->first_token_opt,
                          .p_token_end = flow_obj->p_declarator_origin->last_token_opt,
        };
        checked_read_object(ctx,
        &obj->type,
        is_nullable,
        flow_obj,
        flow_obj->p_declarator_origin->first_token_opt,
        &a_marker,
        false);
    }
    catch
    {
    }
}


static void flow_visit_init_declarator(struct flow_visit_ctx* ctx, struct init_declarator* p_init_declarator)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    try
    {
        flow_visit_declarator(ctx, p_init_declarator->p_declarator);

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                struct true_false_set a = { 0 };
                flow_visit_expression(ctx, p_init_declarator->initializer->assignment_expression, &a);
                true_false_set_destroy(&a);
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
                struct flow_object* _Opt p_right_object =
                    expression_get_flow_object(ctx, p_init_declarator->initializer->assignment_expression, nullable_enabled);

                if (p_right_object)
                {
                    struct marker a_marker = {
                        .p_token_begin = p_init_declarator->p_declarator->first_token_opt,
                        .p_token_end = p_init_declarator->p_declarator->last_token_opt,
                    };

                    struct marker b_marker = {
                        .p_token_begin = p_init_declarator->initializer->assignment_expression->first_token,
                        .p_token_end = p_init_declarator->initializer->assignment_expression->last_token,
                    };

                    assert(p_init_declarator->p_declarator->p_flow_object != NULL);

                    flow_check_assignment(ctx,
                                        p_init_declarator->initializer->assignment_expression->first_token,
                                        &a_marker,
                                        &b_marker,
                                        ASSIGMENT_TYPE_OBJECTS,
                                        false,
                                        type_is_view(&p_init_declarator->p_declarator->type),
                                        type_is_nullable(&p_init_declarator->p_declarator->type, ctx->ctx->options.null_checks_enabled),
                                        &p_init_declarator->p_declarator->type,
                                        p_init_declarator->p_declarator->p_flow_object,
                                        &p_init_declarator->initializer->assignment_expression->type,
                                        p_right_object,
                                        NULL);
                }
                //cast?
                if (expression_is_malloc(p_init_declarator->initializer->assignment_expression))
                {
                    struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                    struct flow_object* _Opt po = make_flow_object(ctx, &t, p_init_declarator->p_declarator, NULL);
                    if (po == NULL)
                    {
                        type_destroy(&t);
                        throw;
                    }

                    object_set_pointer(p_init_declarator->p_declarator->p_flow_object, po);
                    type_destroy(&t);
                    p_init_declarator->p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
                }
                else if (expression_is_calloc(p_init_declarator->initializer->assignment_expression))
                {
                    struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                    struct flow_object* _Opt pointed_calloc_object = make_flow_object(ctx, &t, p_init_declarator->p_declarator, NULL);
                    if (pointed_calloc_object == NULL)
                    {
                        type_destroy(&t);
                        throw;
                    }

                    flow_object_set_zero(&t, pointed_calloc_object);
                    object_set_pointer(p_init_declarator->p_declarator->p_flow_object, pointed_calloc_object);

                    struct marker a_marker = {
                      .p_token_begin = p_init_declarator->p_declarator->first_token_opt,
                      .p_token_end = p_init_declarator->p_declarator->last_token_opt,
                    };

                    checked_read_object(ctx,
                        &t,
                        type_is_nullable(&t, ctx->ctx->options.null_checks_enabled),
                        pointed_calloc_object,
                        p_init_declarator->p_declarator->first_token_opt,
                        &a_marker,
                        false);

                    type_destroy(&t);
                    p_init_declarator->p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
                }
            }
            else  if (p_init_declarator->initializer &&
                p_init_declarator->initializer->braced_initializer)
            {
                braced_initializer_flow(ctx,
                    &p_init_declarator->p_declarator->object,
                    p_init_declarator->p_declarator->p_flow_object);
            }
            else
            {
                if (p_init_declarator->p_declarator->declaration_specifiers &&
                    (
                        (!(p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTOMATIC_STORAGE)) ||
                        (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
                        )
                    )
                {
                    flow_object_set_zero(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_flow_object);
                }
                else
                {
                    flow_object_set_uninitialized(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_flow_object);
                }
            }
        }
    }
    catch
    {
        //some unexpected error
    }
}


static void flow_visit_init_declarator_list(struct flow_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list);

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* _Opt p_type);


static void flow_visit_simple_declaration(struct flow_visit_ctx* ctx, struct simple_declaration* p_simple_declaration)
{
    flow_visit_declaration_specifiers(ctx, p_simple_declaration->p_declaration_specifiers, NULL);
    flow_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}

static void flow_check_pointer_used_as_bool(struct flow_visit_ctx* ctx, struct expression* p_expression);

void print_arena(struct flow_visit_ctx* ctx)
{
    int extra_cols = 0;
    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p_object = ctx->arena.data[i];
        struct flow_object_state* _Opt p_state = p_object->current.next;
        int count = 0;
        while (p_state)
        {
            count++;
            p_state = p_state->next;
        }
        if (count > extra_cols)
            extra_cols = count;
    }


    //┐
    printf("\n");
    printf("┌──┬──────────────────┬─────────────────────────");
    if (extra_cols > 0)
    {
        for (int i = 0; i < extra_cols; i++)
        {
            if (i < extra_cols - 1)
                printf("┬─────────────────────────");
            else
                printf("┬─────────────────────────");
        }
    }

    printf("┐");

    printf("\n");

    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p = ctx->arena.data[i];
        print_object_line(p, extra_cols);
    }
    printf("└──┴──────────────────┴─────────────────────────");
    if (extra_cols > 0)
    {
        for (int i = 0; i < extra_cols; i++)
        {
            if (i < extra_cols - 1)
                printf("┴─────────────────────────");
            else
                printf("┴─────────────────────────");
        }
    }

    printf("┘");

    printf("\n");
    printf("\n");
}

static void flow_visit_if_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_expression_statement)
        flow_visit_expression_statement(ctx, p_selection_statement->p_init_statement->p_expression_statement);

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_simple_declaration)
        flow_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);


    /*
      if(int *p = f());
      The hidden expression is p
    */
    struct expression hidden_expression = { 0 };

    struct true_false_set true_false_set = { 0 };

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression)
    {
        flow_check_pointer_used_as_bool(ctx, p_selection_statement->condition->expression);
        flow_visit_expression(ctx, p_selection_statement->condition->expression, &true_false_set);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression == NULL)
    {
        hidden_expression.expression_type = PRIMARY_EXPRESSION_DECLARATOR;
        hidden_expression.declarator = p_selection_statement->condition->p_init_declarator->p_declarator;
        flow_visit_expression(ctx, &hidden_expression, &true_false_set);
    }

    assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);

    /*
       This index is from the end of top of stack going to base of statck
    */
    const int before_if_state_number = arena_add_copy_of_current_state(ctx, "before-if");

    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);

    const bool true_branch_ends_with_jump =
        secondary_block_ends_with_jump(p_selection_statement->secondary_block);


    /*let's make a copy of the state we left true branch*/
    //const int true_branch = 1;
    const int left_true_branch_state_number = arena_add_copy_of_current_state(ctx, "left-true-branch");

    arena_restore_current_state_from(ctx, before_if_state_number);


    if (p_selection_statement->else_secondary_block_opt)
    {
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        flow_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);
    }
    else
    {
        //we emulate a empty else {}
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
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

            arena_restore_current_state_from(ctx, left_true_branch_state_number);
        }
        else
        {
            /*
               if (){
               }
               else {

               }
            */

            arena_merge_current_state_with_state_number(ctx, left_true_branch_state_number);

            arena_restore_current_state_from(ctx, left_true_branch_state_number);
        }
    }

    flow_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list, p_selection_statement->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list, p_selection_statement->last_token);

    arena_remove_state(ctx, before_if_state_number);
    arena_remove_state(ctx, left_true_branch_state_number);
    true_false_set_destroy(&true_false_set);

}

static void flow_visit_block_item(struct flow_visit_ctx* ctx, struct block_item* p_block_item);


static void flow_visit_try_statement(struct flow_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    try
    {
        const int throw_join_state_old = ctx->throw_join_state;
        struct secondary_block* _Opt catch_secondary_block_old = ctx->catch_secondary_block_opt;

        ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;

        ctx->throw_join_state = arena_add_empty_state(ctx, "try");

        const int original_state_number = arena_add_copy_of_current_state(ctx, "original");


        flow_visit_secondary_block(ctx, p_try_statement->secondary_block);
        arena_set_state_from_current(ctx, original_state_number); //state of end of secondary block

        if (p_try_statement->catch_secondary_block_opt)
        {
            //current all possible states of throw
            arena_restore_current_state_from(ctx, ctx->throw_join_state);
            flow_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
            //current has the state at the end of catch block
        }

        bool try_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->secondary_block);
        bool catch_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt);

        if (try_reached_the_end && catch_reached_the_end)
        {
            //we could merge directly at current
            arena_merge_current_state_with_state_number(ctx, original_state_number);
            arena_restore_current_state_from(ctx, original_state_number);
        }
        else if (try_reached_the_end)
        {
            arena_restore_current_state_from(ctx, original_state_number);
        }
        else if (catch_reached_the_end)
        {
            //ctx_object_restore_current_state_from(ctx, orignial);       
        }


        arena_remove_state(ctx, original_state_number);
        arena_remove_state(ctx, ctx->throw_join_state);

        ctx->throw_join_state = throw_join_state_old; //restore
        ctx->catch_secondary_block_opt = catch_secondary_block_old; //restore
    }
    catch
    {
    }
}

static void flow_visit_switch_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = arena_add_empty_state(ctx, "break join");



    flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);

    bool reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

    if (!reached_the_end)
    {
        arena_restore_current_state_from(ctx, ctx->break_join_state);
    }

    flow_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list, p_selection_statement->secondary_block->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list, p_selection_statement->secondary_block->last_token);

    arena_remove_state(ctx, ctx->initial_state);
    arena_remove_state(ctx, ctx->break_join_state);

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

static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);

static void flow_visit_initializer_list_item(struct flow_visit_ctx* ctx, struct initializer_list_item* p_initializer)
{
    assert(p_initializer->initializer != NULL);
    flow_visit_initializer(ctx, p_initializer->initializer);
}

static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->assignment_expression)
    {
        struct true_false_set a = { 0 };
        flow_visit_expression(ctx, p_initializer->assignment_expression, &a);
        true_false_set_destroy(&a);
    }
    else if (p_initializer->braced_initializer)
    {
        flow_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void flow_visit_initializer_list(struct flow_visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer_list_item* _Opt p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow_visit_initializer_list_item(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow_visit_type_name(struct flow_visit_ctx* ctx, struct type_name* p_type_name)
{
    flow_visit_declarator(ctx, p_type_name->abstract_declarator);
}

static void flow_visit_generic_selection(struct flow_visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        struct true_false_set a = { 0 };
        flow_visit_expression(ctx, p_generic_selection->expression, &a);
        true_false_set_destroy(&a);
    }
    else if (p_generic_selection->type_name)
    {
        flow_visit_type_name(ctx, p_generic_selection->type_name);
    }
}


static void flow_compare_function_arguments(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct argument_expression_list* p_argument_expression_list)
{
    try
    {
        const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

        const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == NULL) throw;

        struct param* _Opt p_current_parameter_type = p_param_list->head;
        struct argument_expression* _Opt p_current_argument = p_argument_expression_list->head;

        while (p_current_argument && p_current_parameter_type)
        {

            struct true_false_set a = { 0 };

            struct diagnostic temp =
                ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index];

            //we don´t report W_FLOW_UNINITIALIZED here because it is checked next.. (TODO parts of expression)
            diagnostic_remove(&ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index], W_FLOW_UNINITIALIZED);

            flow_visit_expression(ctx, p_current_argument->expression, &a);

            ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index] = temp;

            true_false_set_destroy(&a);

            struct flow_object* _Opt p_argument_object =
                expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);

            if (p_argument_object)
            {
                struct flow_object* _Opt parameter_object = make_flow_object(ctx, &p_current_parameter_type->type, NULL, p_current_argument->expression);
                if (parameter_object == NULL) throw;

                flow_object_set_uninitialized(&p_current_parameter_type->type, parameter_object);

                struct marker a_marker = {
                    .p_token_begin = p_current_argument->expression->first_token,
                    .p_token_end = p_current_argument->expression->last_token
                };

                //TODO pass real function to get location
                struct marker b_marker = {
                    .p_token_begin = p_current_argument->expression->first_token,
                    .p_token_end = p_current_argument->expression->last_token
                };

                flow_check_assignment(ctx,
                  p_current_argument->expression->first_token,
                  &a_marker,
                  &b_marker,
                  ASSIGMENT_TYPE_PARAMETER,
                  true,
                  type_is_view(&p_current_parameter_type->type),
                  type_is_nullable(&p_current_parameter_type->type, ctx->ctx->options.null_checks_enabled),
                  &p_current_parameter_type->type,
                  parameter_object, /*dest object*/

                  &p_current_argument->expression->type,
                  p_argument_object,
                  &p_current_argument->set_unkown
                );
            }
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }

        while (p_current_argument)
        {
            /*
               We have more argument than parameters, this happens with variadic functions
            */

            struct flow_object* _Opt p_argument_object =
                expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);

            if (p_argument_object)
            {
                struct marker marker = {
                    .p_token_begin = p_current_argument->expression->first_token,
                    .p_token_end = p_current_argument->expression->last_token
                };
                checked_read_object(ctx,
                    &p_current_argument->expression->type,
                    type_is_nullable(&p_current_argument->expression->type, ctx->ctx->options.null_checks_enabled),
                    p_argument_object,
                    p_current_argument->expression->first_token,
                    &marker,
                    false);
            }
            else
            {
                //
            }
            p_current_argument = p_current_argument->next;
        }

        //////////////////////////// SECOND PASS ////////////////////////////
        /*
            //consider this sample...
            void f(struct X *p,  int * p);

            int main()
            {
                struct X *  pX = make();
                if (pX->p)
                {
                   //cake is making pX->p  unknown before function call..it must be after
                   f(pX, pX->p);
                }
            }
        */

        /*struct param* */ p_current_parameter_type = p_param_list->head;
        /*struct argument_expression* */ p_current_argument = p_argument_expression_list->head;


        while (p_current_argument && p_current_parameter_type)
        {
            if (p_current_argument->set_unkown &&
                type_is_pointer(&p_current_argument->expression->type))
            {
                struct type pointed_type = type_remove_pointer(&p_current_argument->expression->type);

                struct flow_object* _Opt p_argument_object =
                    expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);


                if (p_argument_object)
                {
                    const bool argument_type_is_nullable =
                        type_is_nullable(&pointed_type, ctx->ctx->options.null_checks_enabled);

                    if (p_argument_object->current.pointed)
                    {
                        flow_object_set_unknown(&pointed_type,
                                           argument_type_is_nullable,
                                           p_argument_object->current.pointed,
                                           ctx->ctx->options.null_checks_enabled);
                    }
                    else
                    {
                        //assert(false);
                    }
                }

                type_destroy(&pointed_type);
            }
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }

        while (p_current_argument)
        {
            /*
               We have more argument than parameters, this happens with variadic functions
            */

            struct flow_object* _Opt p_argument_object =
                expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);

            if (p_argument_object)
            {
                //??
            }
            else
            {
                //??
            }
            p_current_argument = p_current_argument->next;
        }
    }
    catch
    {
    }
}

static void check_uninitialized(struct flow_visit_ctx* ctx, struct expression* p_expression)
{
    if (p_expression->is_assignment_expression)
        return;

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (type_is_function(&p_expression->type) ||
        type_is_void(&p_expression->type) ||
        type_is_nullptr_t(&p_expression->type))
    {
        return;
    }

    struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_expression, nullable_enabled);

    if (!ctx->expression_is_not_evaluated)
    {
        if (p_object && p_object->current.state == FLOW_OBJECT_STATE_UNINITIALIZED)
        {
            if (p_expression->expression_type == PRIMARY_EXPRESSION_DECLARATOR &&
                p_expression->declarator &&
                p_expression->declarator->name_opt)
            {
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->first_token, NULL, "using a uninitialized object '%s'", p_expression->declarator->name_opt->lexeme);
            }
            else
            {
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->first_token, NULL, "using a uninitialized object");
            }
        }
        else if (p_object && p_object->current.state & FLOW_OBJECT_STATE_UNINITIALIZED)
        {
            if (ctx->ctx->options.ownership_enabled)
            {
                if (p_expression->declarator && p_expression->declarator->name_opt)
                {
                    compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        p_expression->first_token,
                        NULL,
                        "object '%s' can be uninitialized ",
                        p_expression->declarator->name_opt->lexeme);
                }
                else
                {
                    compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        p_expression->first_token, NULL, "maybe using a uninitialized object");
                }
            }
        }
    }
}

void flow_object_push_states_from(const struct flow_object* p_object_from, struct flow_object* p_object_to)
{
    struct flow_object_state* _Opt it_from = p_object_from->current.next;
    while (it_from)
    {
#if 0
        flow_object_add_state(
        p_object_to,
        p_object_to->current.state,
        &p_object_to->current.ref,
        it_from->dbg_name,
        it_from->state_number);
#endif

        it_from = it_from->next;
    }

    for (int i = 0; i < p_object_to->members.size; i++)
    {
        flow_object_push_states_from(p_object_from, p_object_to->members.data[i]);
    }
}


static void flow_check_pointer_used_as_bool(struct flow_visit_ctx* ctx, struct expression* p_expression)
{
    //when pointer as used as bool, like if (p) or p && p->i we check
    //if the pointer has a flow condition always true or false.

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (type_is_pointer(&p_expression->type))
    {

        struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_expression, nullable_enabled);
        if (p_object)
        {
            struct marker marker = {
                 .p_token_begin = p_expression->first_token,
                 .p_token_end = p_expression->last_token
            };

            if (flow_object_is_null(p_object))
            {
                compiler_diagnostic_message(W_FLOW_NON_NULL,
                        ctx->ctx,
                        NULL,
                        &marker,
                        "pointer is always null");

            }
            else if (flow_object_is_not_null(p_object))
            {
                compiler_diagnostic_message(W_FLOW_NON_NULL,
                        ctx->ctx,
                        NULL,
                        &marker,
                        "pointer is always not-null");
            }
        }
    }
}

static void arena_broadcast_change(struct flow_visit_ctx* ctx, struct flow_object* p)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p_obj = ctx->arena.data[i];
        if (p_obj)
        {
            for (int j = 0; j < p_obj->current.alternatives.size; j++)
            {
                if (p_obj->current.alternatives.data[j] == p)
                {
                    flow_object_update_current(p_obj);
                    break;
                }
            }
        }
    }
}

static struct argument_expression* _Opt param_list_find_argument_by_name(struct param_list* p_param_list,
    struct argument_expression_list* list,
    const char* name)
{
    struct param* _Opt p_param = p_param_list->head;
    struct argument_expression* _Opt p_argument_expression = list->head;
    while (p_param && p_argument_expression)
    {
        if (strcmp(p_param->type.name_opt, name) == 0)
        {
            return p_argument_expression;
        }
        p_argument_expression = p_argument_expression->next;
        p_param = p_param->next;
    }
    return NULL;
}

static struct expression* _Opt _Owner expression_replace(struct flow_visit_ctx* ctx,
    struct expression* p_expression,
    struct param_list* p_param_list,
    struct argument_expression_list* list)
{

    if (p_expression->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
    {
        const char* name = p_expression->declarator->name_opt->lexeme;
        struct argument_expression* _Opt p_argument_expression =
            param_list_find_argument_by_name(p_param_list, list, name);

        if (p_argument_expression)
            return expression_replace(ctx, p_argument_expression->expression, p_param_list, list);

        struct expression* _Opt _Owner p_expression_new = calloc(1, sizeof * p_expression_new);
        if (p_expression_new == NULL)
            return NULL;

        *p_expression_new = *p_expression;
        return p_expression_new;
    }
#if 0
    else if (p_expression->expression_type == POSTFIX_FUNCTION_CALL)
    {
        if (type_is_function(&p_expression->left->declarator->type))
        {
            struct expression* _Opt _Owner p_expression_new = calloc(1, sizeof * p_expression_new);
            if (p_expression_new == NULL)
                return NULL;

            *p_expression_new = *p_expression;

            struct argument_expression* _Opt p_current_argument =
                p_expression->argument_expression_list.head;

            while (p_current_argument)
            {
                p_current_argument->expression = expression_replace(ctx,
                            p_current_argument->expression,
                            &p_expression->left->declarator->type.params,
                            &p_expression->argument_expression_list);

                p_current_argument = p_current_argument->next;
            }

            if (p_expression->left->declarator->p_expression_true)
            {
                return
                    expression_replace(ctx,
                        p_expression->left->declarator->p_expression_true,
                        &p_expression->left->declarator->type.params,
                        &p_expression->argument_expression_list);
            }
        }
    }
#endif

    struct expression* _Opt _Owner p_expression_new = calloc(1, sizeof * p_expression_new);
    if (p_expression_new == NULL)
        return NULL;

    *p_expression_new = *p_expression;

    if (p_expression->left)
        p_expression_new->left = expression_replace(ctx, p_expression->left, p_param_list, list);
    if (p_expression->right)
        p_expression_new->right = expression_replace(ctx, p_expression->right, p_param_list, list);

    return p_expression_new;
}

static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression, struct true_false_set* expr_true_false_set)
{
    true_false_set_clear(expr_true_false_set); //_Out

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    switch (p_expression->expression_type)
    {
    case EXPRESSION_TYPE_INVALID:
        assert(false);
        break;

    case PRIMARY_EXPRESSION__FUNC__:
        break;

    case PRIMARY_EXPRESSION_ENUMERATOR:

        break;
    case PRIMARY_EXPRESSION_DECLARATOR:
    {
        _Opt struct true_false_set_item item = { 0 };
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        true_false_set_push_back(expr_true_false_set, &item);
        check_uninitialized(ctx, p_expression);
    }
    break;

    case PRIMARY_EXPRESSION_PARENTESIS:
        assert(p_expression->right != NULL);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        break;

    case PRIMARY_EXPRESSION_STRING_LITERAL:
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
    case PRIMARY_EXPRESSION_NUMBER:
    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        assert(p_expression->generic_selection != NULL);
        flow_visit_generic_selection(ctx, p_expression->generic_selection);
        break;

    case POSTFIX_DOT:
    {
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);

        true_false_set_destroy(&left_set);

        struct true_false_set_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        true_false_set_push_back(expr_true_false_set, &item);
    }
    break;

    case POSTFIX_ARROW:
    {
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);
        true_false_set_destroy(&left_set);

        struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);

        if (p_object != NULL)
        {
            if (flow_object_can_be_null(p_object))
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    struct marker marker = { 0 };
                    marker.p_token_begin = p_expression->left->first_token;
                    marker.p_token_end = p_expression->left->last_token;
                    compiler_diagnostic_message(W_FLOW_NULL_DEREFERENCE,
                            ctx->ctx,
                            NULL,
                            &marker,
                           "pointer may be null");
                }
            }
            else if (flow_object_can_be_uninitialized(p_object))
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    compiler_diagnostic_message(W_FLOW_NULL_DEREFERENCE,
                            ctx->ctx,
                            p_expression->left->first_token, NULL, "object is possibly uninitialized");
                }
            }
            else if (flow_object_can_have_its_lifetime_ended(p_object))
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    compiler_diagnostic_message(W_FLOW_LIFETIME_ENDED,
                            ctx->ctx,
                            p_expression->left->first_token, NULL, "object lifetime ended");
                }
            }
        }

        if (!ctx->expression_is_not_evaluated)
        {
            struct flow_object* _Opt p_object2 = expression_get_flow_object(ctx, p_expression, nullable_enabled);
            if (p_object2 && flow_object_can_have_its_lifetime_ended(p_object2))
            {
                struct marker marker = {
                    .p_token_begin = p_expression->first_token,
                    .p_token_end = p_expression->last_token
                };
                compiler_diagnostic_message(W_FLOW_LIFETIME_ENDED,
                        ctx->ctx,
                        NULL,
                        &marker,
                        "object lifetime ended");
            }
        }

        struct true_false_set_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        true_false_set_push_back(expr_true_false_set, &item);
    }
    break;

    case POSTFIX_INCREMENT:
    case POSTFIX_DECREMENT:
    {
        assert(p_expression->left != NULL);

        struct flow_object* const _Opt p_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
        if (p_object)
        {
            if (flow_object_is_null(p_object))
            {
                //p_object->current.state &= ~FLOW_OBJECT_STATE_NULL;
                p_object->current.state = FLOW_OBJECT_STATE_NOT_NULL;
            }
            else if (flow_object_is_zero(p_object))
            {
                //p_object->current.state &= ~FLOW_OBJECT_STATE_ZERO;
                p_object->current.state = FLOW_OBJECT_STATE_NOT_ZERO;
            }
        }
        flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
    }
    break;

    case POSTFIX_ARRAY:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        true_false_set_clear(expr_true_false_set);

        struct true_false_set_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        true_false_set_push_back(expr_true_false_set, &item);

    }
    break;

    case POSTFIX_FUNCTION_CALL:
    {
        assert(p_expression->left != NULL);

        struct true_false_set left_local = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_local);

        flow_compare_function_arguments(ctx, &p_expression->left->type, &p_expression->argument_expression_list);
        true_false_set_destroy(&left_local);
        //////////////////////////////////////////////////////////////////////////////////////////////////////
#if CONTRACTS
        if (p_expression->left->declarator &&
            type_is_function(&p_expression->left->declarator->type))
        {
            struct type return_type = get_function_return_type(&p_expression->left->declarator->type);
            if (p_expression->left->declarator->p_expression_true)
            {
                struct expression* p_expression_true =
                    expression_replace(ctx,
                        p_expression->left->declarator->p_expression_true,
                        &p_expression->left->declarator->type.params,
                        &p_expression->argument_expression_list);

                if (type_is_scalar(&return_type))
                {
                    struct true_false_set local = { 0 };

                    flow_visit_expression(ctx, p_expression_true, &local);

                    for (int i = 0; i < local.size; i++)
                    {
                        struct true_false_set_item item5 = { 0 };
                        item5.p_expression = local.data[i].p_expression;
                        item5.true_branch_state |= local.data[i].true_branch_state;
                        //item5.false_branch_state |= local.data[i].false_branch_state;
                        true_false_set_push_back(expr_true_false_set, &item5);
                    }
                    true_false_set_destroy(&local);
                }
                else
                {
                    struct true_false_set true_false_set4 = { 0 };
                    bool old = ctx->inside_assert;
                    ctx->inside_assert = true;
                    flow_visit_expression(ctx, p_expression_true, &true_false_set4); //assert(p == 0);            
                    ctx->inside_assert = old;
                    true_false_set_set_objects_to_true_branch(ctx, &true_false_set4, nullable_enabled);
                    true_false_set_destroy(&true_false_set4);
                }
            }

            if (p_expression->left->declarator->p_expression_false)
            {
                struct expression* p_expression_false =
                    expression_replace(ctx,
                        p_expression->left->declarator->p_expression_false,
                        &p_expression->left->declarator->type.params,
                        &p_expression->argument_expression_list);


                struct true_false_set local = { 0 };

                flow_visit_expression(ctx, p_expression_false, &local);

                for (int i = 0; i < local.size; i++)
                {
                    int index =
                        find_item_index_by_expression(expr_true_false_set, local.data[i].p_expression);
                    if (index == -1)
                    {
                        struct true_false_set_item item5 = { 0 };
                        item5.p_expression = local.data[i].p_expression;
                        //item5.true_branch_state |= local.data[i].true_branch_state;
                        item5.false_branch_state |= local.data[i].true_branch_state;
                        true_false_set_push_back(expr_true_false_set, &item5);
                    }
                    else
                    {
                        local.data[index].false_branch_state |= local.data[i].false_branch_state;
                    }

                }
                true_false_set_destroy(&local);

            }
            type_destroy(&return_type);

        }
#endif
    }
    break;

    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
        assert(p_expression->compound_statement != NULL);
        flow_visit_compound_statement(ctx, p_expression->compound_statement);

        break;

    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);
        assert(p_expression->type_name != NULL);
        assert(p_expression->braced_initializer != NULL);

        flow_visit_type_name(ctx, p_expression->type_name);

        flow_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

        struct flow_object* _Opt temp2 = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
        if (temp2 == NULL)
        {
            return;
        }

        if (p_expression->type_name->abstract_declarator->p_flow_object)
        {
            flow_object_swap(temp2, p_expression->type_name->abstract_declarator->p_flow_object);

            //TODO the state of object depends of the initializer        
            flow_object_set_zero(&p_expression->type, p_expression->type_name->abstract_declarator->p_flow_object);
        }

        break;

    case UNARY_EXPRESSION_ALIGNOF:

        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
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
            struct true_false_set true_false_set4 = { 0 };
            bool old = ctx->inside_assert;
            ctx->inside_assert = true;
            flow_visit_expression(ctx, p_expression->right, &true_false_set4); //assert(p == 0);            
            ctx->inside_assert = old;
            true_false_set_set_objects_to_true_branch(ctx, &true_false_set4, nullable_enabled);
            true_false_set_destroy(&true_false_set4);
        }

        break;

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:

        if (p_expression->right)
        {
            const bool t2 = ctx->expression_is_not_evaluated;
            ctx->expression_is_not_evaluated = true;
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
            ctx->expression_is_not_evaluated = t2;
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            flow_visit_type_name(ctx, p_expression->type_name);
        }


        break;

    case UNARY_EXPRESSION_NEG:
    case UNARY_EXPRESSION_PLUS:
        assert(p_expression->right != NULL);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        break;

    case UNARY_EXPRESSION_NOT:
        assert(p_expression->right != NULL);
        flow_check_pointer_used_as_bool(ctx, p_expression->right);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        true_false_set_invert(expr_true_false_set);
        break;

    case UNARY_EXPRESSION_SIZEOF_TYPE:
    case UNARY_EXPRESSION_NELEMENTSOF_TYPE:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:

    case UNARY_EXPRESSION_BITNOT:


    case UNARY_EXPRESSION_ADDRESSOF:
    {

        if (p_expression->type_name)
        {
            /*sizeof*/
            flow_visit_type_name(ctx, p_expression->type_name);
        }
    }
    break;

    case UNARY_EXPRESSION_CONTENT:
    {
        assert(p_expression->right != NULL);

        struct flow_object* _Opt p_object0 = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);

        if (p_object0 && p_object0->current.state == FLOW_OBJECT_STATE_UNINITIALIZED)
        {
            if (!ctx->expression_is_not_evaluated)
            {
                struct marker marker = { 0 };
                marker.p_token_begin = p_expression->right->first_token;
                marker.p_token_end = p_expression->right->last_token;
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    NULL, &marker, "using a uninitialized object");
            }
        }
        else if (p_object0 && flow_object_can_be_null(p_object0))
        {
            /*
              *p = 1*
            */
            if (!ctx->expression_is_not_evaluated)
            {
                compiler_diagnostic_message(W_FLOW_NULL_DEREFERENCE,
                    ctx->ctx,
                    p_expression->right->first_token, NULL, "dereference a NULL object");
            }
        }

        struct true_false_set local_true_false = { 0 };
        flow_visit_expression(ctx, p_expression->right, &local_true_false);
        /*empty set*/
        true_false_set_destroy(&local_true_false);
    }
    break;


    case ASSIGNMENT_EXPRESSION:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);
        true_false_set_swap(expr_true_false_set, &left_set);
        true_false_set_destroy(&left_set);

        struct true_false_set right_set = { 0 };
        flow_visit_expression(ctx, p_expression->right, &right_set);
        true_false_set_destroy(&right_set);

        //struct object temp_obj1 = { 0 };
        struct flow_object* const _Opt p_right_object = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);

        //struct object temp_obj2 = { 0 };
        struct flow_object* const _Opt p_dest_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);

        if (p_dest_object == NULL || p_right_object == NULL)
        {
            return;
        }
        //TODO

        struct marker a_marker = {
          .p_token_begin = p_expression->left->first_token,
          .p_token_end = p_expression->left->last_token
        };

        struct marker b_marker = {
          .p_token_begin = p_expression->right->first_token,
          .p_token_end = p_expression->right->last_token
        };

        flow_check_assignment(ctx,
            p_expression->left->first_token,
            &a_marker,
            &b_marker,
            ASSIGMENT_TYPE_OBJECTS,
            true,
            type_is_view(&p_expression->left->type), /*dest type*/
            type_is_nullable(&p_expression->left->type, ctx->ctx->options.null_checks_enabled), /*dest type*/
            &p_expression->left->type, /*dest type*/
            p_dest_object, /*dest object*/
            &p_expression->right->type, /*source type*/
            p_right_object /*source*/,
            NULL);

        //we could havea arena_broadcast
        /*
          built-in malloc, calloc assignment
        */
        arena_broadcast_change(ctx, p_dest_object);

        if (expression_is_malloc(p_expression->right))
        {
            struct type t = type_remove_pointer(&p_expression->left->type);
            struct flow_object* _Opt po = make_flow_object(ctx, &t, NULL, p_expression->left);
            if (po == NULL)
            {
                type_destroy(&t);
                return;
            }
            object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
        }
        else if (expression_is_calloc(p_expression->right))
        {
            struct type t = type_remove_pointer(&p_expression->left->type);
            struct flow_object* _Opt po = make_flow_object(ctx, &t, NULL, p_expression->left);
            if (po == NULL)
            {
                type_destroy(&t);
                return;
            }
            flow_object_set_zero(&t, po);
            object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
        }
    }
    break;
    case MULTIPLICATIVE_EXPRESSION_DIV:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        struct true_false_set right_set = { 0 };

        flow_visit_expression(ctx, p_expression->left, &left_set);

        struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_expression->right, ctx->ctx->options.null_checks_enabled);
        if (p_object)
        {
            if (flow_object_can_be_zero(p_object))
            {
                compiler_diagnostic_message(W_FLOW_DIVIZION_BY_ZERO, ctx->ctx, p_expression->right->first_token, NULL, "possible division by zero");
            }
        }

        /*
                               true_set               false_set
             b / a             b_true_set a_true_set  a_true_set
             0 / a             -                      a_true_set a_true_set
        */

        flow_visit_expression(ctx, p_expression->right, &right_set);
        true_false_set_merge(expr_true_false_set, &left_set, &right_set,
            MERGE_OPTIONS_A_TRUE | MERGE_OPTIONS_B_TRUE,
            MERGE_OPTIONS_A_TRUE | MERGE_OPTIONS_B_TRUE);

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
            flow_visit_expression(ctx, p_expression->left, &left_set);
            true_false_set_destroy(&left_set);
        }

        if (p_expression->right)
        {
            struct true_false_set right_set = { 0 };
            flow_visit_expression(ctx, p_expression->right, &right_set);
            true_false_set_destroy(&right_set);
        }
    }
    break;

    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_BIGGER_THAN:
    case RELATIONAL_EXPRESSION_LESS_THAN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        //TODO a > 1
        //     a > -1 etc...

        /*
                     true_set         false_set
           a > 0     a_true           a_true a_false
           a < 0     a_true           a_true a_false


           a >= 0    a_true a_false   a_true
           b <= 0    a_true a_false   a_true
        */

        const bool left_is_constant = object_has_constant_value(&p_expression->left->object);
        const bool right_is_constant = object_has_constant_value(&p_expression->right->object);

        if (left_is_constant)
        {
            const long long left_value = object_to_signed_long_long(&p_expression->left->object);

            struct true_false_set true_false_set_right = { 0 };
            flow_visit_expression(ctx, p_expression->right, &true_false_set_right);
            if (left_value == 0)
            {
                true_false_set_swap(expr_true_false_set, &true_false_set_right);
                for (int i = 0; i < expr_true_false_set->size; i++)
                {
                    struct true_false_set_item* item = &expr_true_false_set->data[i];
                    item->false_branch_state |= item->true_branch_state;
                }

                if (p_expression->expression_type == RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN ||
                    p_expression->expression_type == RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN)
                {
                    true_false_set_invert(expr_true_false_set);
                }
            }
            true_false_set_destroy(&true_false_set_right);
        }

        else if (right_is_constant)
        {
            const long long right_value = object_to_signed_long_long(&p_expression->right->object);
            struct true_false_set true_false_set_left3 = { 0 };
            flow_visit_expression(ctx, p_expression->left, &true_false_set_left3);
            if (right_value == 0)
            {
                true_false_set_swap(expr_true_false_set, &true_false_set_left3);
                for (int i = 0; i < expr_true_false_set->size; i++)
                {
                    struct true_false_set_item* item = &expr_true_false_set->data[i];
                    item->false_branch_state |= item->true_branch_state;
                }
                if (p_expression->expression_type == RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN ||
                    p_expression->expression_type == RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN)
                {
                    true_false_set_invert(expr_true_false_set);
                }
            }
            true_false_set_destroy(&true_false_set_left3);
        }
        else
        {
            struct true_false_set true_false_set = { 0 };
            flow_visit_expression(ctx, p_expression->left, &true_false_set);
            flow_visit_expression(ctx, p_expression->right, &true_false_set);
            true_false_set_destroy(&true_false_set);
        }
    }
    break;

    case EQUALITY_EXPRESSION_NOT_EQUAL:
    case EQUALITY_EXPRESSION_EQUAL:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        long long value = 0;
        struct expression* _Opt p_comp_expression = NULL;
        if (object_has_constant_value(&p_expression->left->object) &&
            !object_has_constant_value(&p_expression->right->object))
        {
            value = object_to_signed_long_long(&p_expression->left->object);
            p_comp_expression = p_expression->right;
        }
        else if (object_has_constant_value(&p_expression->right->object) &&
                !object_has_constant_value(&p_expression->left->object))
        {
            value = object_to_signed_long_long(&p_expression->right->object);
            p_comp_expression = p_expression->left;
        }


        if (p_comp_expression)
        {
            struct true_false_set true_false_set = { 0 };
            flow_visit_expression(ctx, p_comp_expression, &true_false_set);

            //constant == p_comp_expression  |  p_comp_expression == constant
            //constant != p_comp_expression  |  p_comp_expression != constant

            struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_comp_expression, nullable_enabled);
            if (p_object)
            {
                struct marker marker = {
                     .p_token_begin = p_comp_expression->first_token,
                     .p_token_end = p_comp_expression->last_token
                };


                if ((flow_object_is_null(p_object) || flow_object_is_zero(p_object)) && value == 0)
                {
                    if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL)
                    {
                        if (ctx->inside_assert)
                        {
                            /*
                             assert checks in runtime the same state we have at compile time
                             assert(p == NULL);
                            */
                        }
                        else
                        {
                            //if (p == NULL) { } //warning  p is always null
                            if (type_is_pointer(&p_comp_expression->type))
                                compiler_diagnostic_message(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always null");
                            else
                                compiler_diagnostic_message(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always zero");
                        }
                    }
                    else if (p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL)
                    {
                        /*
                           runtime check is diferent from static state
                           assert(p != NULL);
                        */
                        if (type_is_pointer(&p_comp_expression->type))
                            compiler_diagnostic_message(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always null");
                        else
                            compiler_diagnostic_message(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always zero");
                    }
                }
                else if ((flow_object_is_not_null(p_object) || flow_object_is_not_zero(p_object)) && value == 0)
                {
                    if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL)
                    {
                        /*
                           runtime check is diferent from static state
                           assert(p == NULL);
                        */
                        if (type_is_pointer(&p_comp_expression->type))
                            compiler_diagnostic_message(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always non-null");
                        else
                            compiler_diagnostic_message(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always non-zero");
                    }
                    else if (p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL)
                    {
                        /*
                           assert checks in runtime the same state we have at compile time
                           assert(p != NULL);
                        */
                        if (ctx->inside_assert)
                        {
                            /*
                             assert checks in runtime the same state we have at compile time
                             assert(p == NULL);
                            */
                        }
                        else
                        {
                            if (type_is_pointer(&p_comp_expression->type))
                                compiler_diagnostic_message(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always non-null");
                            else
                                compiler_diagnostic_message(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always non-zero");
                        }
                    }
                }
            }

            true_false_set_swap(expr_true_false_set, &true_false_set);
            if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL && value == 0)
            {
                true_false_set_invert(expr_true_false_set);
            }
            else if (p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL && value != 0)
            {
                true_false_set_invert(expr_true_false_set);
            }
            true_false_set_destroy(&true_false_set);
        }
        else
        {
            struct true_false_set true_false_set = { 0 };
            flow_visit_expression(ctx, p_expression->left, &true_false_set);
            flow_visit_expression(ctx, p_expression->right, &true_false_set);
            true_false_set_destroy(&true_false_set);
        }
    }
    break;

    case LOGICAL_OR_EXPRESSION:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow_check_pointer_used_as_bool(ctx, p_expression->left);
        flow_check_pointer_used_as_bool(ctx, p_expression->right);

        const int original_state_number = arena_add_copy_of_current_state(ctx, "original");

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);



        //Set all variables to false state, because otherwise, the right branch
        // would not be evaluated
        true_false_set_set_objects_to_false_branch(ctx, &left_set, nullable_enabled);

        struct true_false_set right_set = { 0 };
        flow_visit_expression(ctx, p_expression->right, &right_set);

        //  true_false_set_merge(expr_true_false_set, &left_set, &right_set,
        //         MERGE_OPTIONS_A_TRUE | MERGE_OPTIONS_A_FALSE | MERGE_OPTIONS_B_TRUE | MERGE_OPTIONS_B_FALSE,
        //         MERGE_OPTIONS_A_FALSE| MERGE_OPTIONS_B_FALSE);

        //Tudo que faz left ser true ou right ser true

        for (int i = 0; i < left_set.size; i++)
        {
            struct true_false_set_item item5;

            item5.p_expression = left_set.data[i].p_expression;
            item5.true_branch_state |= (left_set.data[i].true_branch_state | left_set.data[i].false_branch_state);
            item5.false_branch_state |= left_set.data[i].false_branch_state;
            true_false_set_push_back(expr_true_false_set, &item5);
        }

        for (int k = 0; k < right_set.size; k++)
        {
            int index =
                find_item_index_by_expression(expr_true_false_set, right_set.data[k].p_expression);
            if (index == -1)
            {
                index = expr_true_false_set->size;
                _Opt struct true_false_set_item item4 = { 0 };
                true_false_set_push_back(expr_true_false_set, &item4);
            }

            //Tudo que faz left true e right true faz expressao se true
            expr_true_false_set->data[index].p_expression = right_set.data[k].p_expression;
            //d->data[index].true_branch_state |= right_set.data[k].true_branch_state;
            //Tudo que faz left true ou left false, e right false faz ser false
            expr_true_false_set->data[index].false_branch_state |= right_set.data[k].false_branch_state;

            //No path true seria possivel nao ser feito o right
            expr_true_false_set->data[index].true_branch_state |= (BOOLEAN_FLAG_TRUE | BOOLEAN_FLAG_FALSE);

        }

        arena_restore_current_state_from(ctx, original_state_number);
        arena_remove_state(ctx, original_state_number);
        true_false_set_destroy(&left_set);
        true_false_set_destroy(&right_set);
    }
    break;

    case LOGICAL_AND_EXPRESSION:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow_check_pointer_used_as_bool(ctx, p_expression->left);
        flow_check_pointer_used_as_bool(ctx, p_expression->right);

        const int original_state_number = arena_add_copy_of_current_state(ctx, "original");

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);


        //Set all variables to true state, because otherwise, the right branch
        // would not be evaluated
        true_false_set_set_objects_to_true_branch(ctx, &left_set, nullable_enabled);

        struct true_false_set right_set = { 0 };
        flow_visit_expression(ctx, p_expression->right, &right_set);
        //arena_restore_current_state_from(ctx, original_state_number);

       //Anything that makes left and right true also makes left && right true. 
       //Anything that makes left false or right false also makes left && right false.

        for (int i = 0; i < left_set.size; i++)
        {
            const struct true_false_set_item* p_item_left = &left_set.data[i];

            _Opt struct true_false_set_item left_and_right = { 0 };
            left_and_right.p_expression = p_item_left->p_expression;

            left_and_right.true_branch_state |= p_item_left->true_branch_state;
            left_and_right.false_branch_state |= p_item_left->true_branch_state | p_item_left->false_branch_state;

            true_false_set_push_back(expr_true_false_set, &left_and_right);
        }

        for (int k = 0; k < right_set.size; k++)
        {
            const struct true_false_set_item* p_item_right = &right_set.data[k];

            int index = find_item_index_by_expression(expr_true_false_set, p_item_right->p_expression);
            if (index == -1)
            {
                index = expr_true_false_set->size;
                struct true_false_set_item item2 = { 0 };
                //item2.p_expression //????
                true_false_set_push_back(expr_true_false_set, &item2);
            }

            //Tudo que faz left true e right true faz expressao se true
            expr_true_false_set->data[index].p_expression = p_item_right->p_expression;
            expr_true_false_set->data[index].true_branch_state |= p_item_right->true_branch_state;
            //Tudo que faz left true ou left false, e right false faz ser false
            expr_true_false_set->data[index].false_branch_state |= p_item_right->false_branch_state;

            //right expression may not be evaluated, in this case all previous states are also valid
            //so if the variable could be true and false then we need to add            
            expr_true_false_set->data[index].false_branch_state |= (BOOLEAN_FLAG_TRUE | BOOLEAN_FLAG_FALSE);

        }

        arena_restore_current_state_from(ctx, original_state_number);
        arena_remove_state(ctx, original_state_number);
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

        flow_visit_expression(ctx, p_expression->left, &true_false_set);
        flow_visit_expression(ctx, p_expression->right, &true_false_set);

        true_false_set_destroy(&true_false_set);
    }
    break;

    case AND_EXPRESSION:
    case EXCLUSIVE_OR_EXPRESSION:

        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);


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
    {
        assert(p_expression->condition_expr != NULL);
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);


        struct true_false_set true_false_set = { 0 };

        flow_check_pointer_used_as_bool(ctx, p_expression->condition_expr);
        flow_visit_expression(ctx, p_expression->condition_expr, &true_false_set);

        const int before_if_state_number = arena_add_copy_of_current_state(ctx, "before-if");

        true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);

        struct true_false_set set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &set);
        true_false_set_destroy(&set);


        const int left_true_branch_state_number = arena_add_copy_of_current_state(ctx, "left-true-branch");

        arena_restore_current_state_from(ctx, before_if_state_number);

        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);

        struct true_false_set set2 = { 0 };
        flow_visit_expression(ctx, p_expression->right, &set2);
        true_false_set_destroy(&set2);

        arena_merge_current_state_with_state_number(ctx, left_true_branch_state_number);

        arena_restore_current_state_from(ctx, left_true_branch_state_number);
        arena_remove_state(ctx, before_if_state_number);
        arena_remove_state(ctx, left_true_branch_state_number);
        true_false_set_destroy(&true_false_set);
    }
    break;

    }
}

static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    struct true_false_set d = { 0 };
    if (p_expression_statement->expression_opt)
        flow_visit_expression(ctx, p_expression_statement->expression_opt, &d);
    true_false_set_destroy(&d);
}

static void flow_visit_block_item_list(struct flow_visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void flow_visit_compound_statement(struct flow_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    flow_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    flow_exit_block_visit_defer_list(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
}

static void flow_visit_do_while_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct true_false_set true_false_set = { 0 };

    try
    {
        if (p_iteration_statement->expression1)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set);
        }

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

        bool was_last_statement_inside_true_branch_return =
            secondary_block_ends_with_jump(p_iteration_statement->secondary_block);


        if (was_last_statement_inside_true_branch_return)
        {
            //restore_state(ctx, 0);
            //if (p_object_compared_with_not_null)
            //{
                //do {}  while (p);
              //  p_object_compared_with_not_null->state = FLOW_OBJECT_STATE_NULL;
            //}
        }
        else
        {
            //do { } while (p);
            true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        }
    }
    catch
    {
    }

    true_false_set_destroy(&true_false_set);
}

static void flow_visit_while_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    if (p_iteration_statement->expression1 == NULL)
        return;

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = arena_add_empty_state(ctx, "break join");

    struct true_false_set true_false_set = { 0 };

    /*
        we do like this to acumulate states.

        if (expression)
        {
           statements...
           if (expression)
           {
             statements...
           }
        }
        break_exit:
    */

    //We do a visit but this is not conclusive..so we ignore warnings
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
    flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set);


    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);

    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    //Second pass warning is ON
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    struct true_false_set true_false_set2 = { 0 };
    flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set2);
    true_false_set_destroy(&true_false_set2);

    //visit secondary_block again
    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    const bool was_last_statement_inside_true_branch_return =
        secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

    if (was_last_statement_inside_true_branch_return)
    {
        /*
           while (p) { return; }
        */
        arena_restore_current_state_from(ctx, ctx->initial_state);
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
    }
    else
    {
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        arena_merge_current_state_with_state_number(ctx, ctx->break_join_state);
        arena_restore_current_state_from(ctx, ctx->break_join_state);
    }

    flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    arena_remove_state(ctx, ctx->initial_state);
    arena_remove_state(ctx, ctx->break_join_state);


    //restore
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
    true_false_set_destroy(&true_false_set);
}

static void flow_visit_for_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
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
            flow_visit_init_declarator_list(ctx, &p_iteration_statement->declaration->init_declarator_list);
        }

        if (p_iteration_statement->expression0)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression0, &d);
        }

        if (p_iteration_statement->expression1)
        {
            flow_check_pointer_used_as_bool(ctx, p_iteration_statement->expression1);
            flow_visit_expression(ctx, p_iteration_statement->expression1, &d);
        }

        //TODO we need to merge states inside loops

        //Disable warning because the state is temporary..missing a visit
        diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);


        if (p_iteration_statement->expression2)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression2, &d);
        }
        const bool b_secondary_block_ends_with_jump =
            secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

        /*we visit again*/
        if (!b_secondary_block_ends_with_jump)
        {
            flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
            flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
            flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        }

    }
    catch
    {
    }
    true_false_set_destroy(&d);
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
    try
    {
        if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
        {
            arena_merge_current_state_with_state_number(ctx, ctx->throw_join_state);

            //check_all_defer_until_try(ctx, ctx->tail_block, p_jump_statement->first_token);
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
        {
            if (ctx->p_return_type == NULL)
            {
                //we must be inside a function and we need this return set.
                throw;
            }

            if (p_jump_statement->expression_opt)
            {
                struct true_false_set d = { 0 };
                flow_visit_expression(ctx, p_jump_statement->expression_opt, &d);
                true_false_set_destroy(&d);
            }

            /*
              returning a declarator will move the ownership
            */
            if (p_jump_statement->expression_opt)
            {
                struct flow_object* _Opt p_object =
                    expression_get_flow_object(ctx, p_jump_statement->expression_opt, nullable_enabled);

                if (p_object)
                {
                    assert(ctx->p_return_type != NULL);
                    struct flow_object* _Opt p_dest_object =
                        make_flow_object(ctx, ctx->p_return_type, NULL, p_jump_statement->expression_opt);

                    if (p_dest_object == NULL)
                    {
                        throw;
                    }

                    assert(ctx->p_return_type != NULL);
                    flow_object_set_zero(ctx->p_return_type, p_dest_object);

                    struct marker a_marker = {
                       .p_token_begin = p_jump_statement->expression_opt->first_token,
                       .p_token_end = p_jump_statement->expression_opt->last_token,
                    };
                    struct marker b_marker = {
                       .p_token_begin = p_jump_statement->expression_opt->first_token,
                       .p_token_end = p_jump_statement->expression_opt->last_token,
                    };

                    assert(ctx->p_return_type != NULL);

                    flow_check_assignment(ctx,
                     p_jump_statement->expression_opt->first_token,
                     &a_marker,
                     &b_marker,
                     ASSIGMENT_TYPE_RETURN,
                     true,
                        type_is_view(ctx->p_return_type), /*dest type*/
                        type_is_nullable(ctx->p_return_type, ctx->ctx->options.null_checks_enabled), /*dest type*/
                        ctx->p_return_type, /*dest type*/
                        p_dest_object, /*dest object*/
                        &p_jump_statement->expression_opt->type, /*source type*/
                        p_object, /*source*/
                        NULL
                    );

                    const int state_before_return = arena_add_copy_of_current_state(ctx, "before-return");

                    // Simulate the function's state as if it had already returned. 
                    // This allows us to verify whether the returned object points to any 
                    // local variables.

                    flow_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

                    if (ctx->p_return_type == NULL)
                        throw;

                    checked_read_object(ctx,
                                        ctx->p_return_type,
                                        type_is_nullable(ctx->p_return_type, ctx->ctx->options.null_checks_enabled),
                                        p_dest_object,
                                        NULL,
                                        &a_marker,
                                        true);

                    //then we restore the state
                    arena_restore_current_state_from(ctx, state_before_return);
                }

                if (p_object && p_object->is_temporary)
                {
                    //a + b
                    p_object->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
                }

            }

            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
        {
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            arena_merge_current_state_with_state_number(ctx, ctx->break_join_state);
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
        {
            //goto needs a label
            assert(p_jump_statement->label != NULL);

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
                label_state_number = arena_add_empty_state(ctx, p_jump_statement->label->lexeme);
                ctx->labels[ctx->labels_size].state_number = label_state_number;
                ctx->labels[ctx->labels_size].label_name = p_jump_statement->label->lexeme;
                ctx->labels_size++;
            }

            arena_merge_current_state_with_state_number(ctx, label_state_number);
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
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

static void flow_visit_label(struct flow_visit_ctx* ctx, struct label* p_label);

static void flow_visit_labeled_statement(struct flow_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    flow_visit_label(ctx, p_labeled_statement->label);
    flow_visit_statement(ctx, p_labeled_statement->statement);
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
    if (p_label->p_identifier_opt)
    {
        for (int i = 0; i < ctx->labels_size; i++)
        {
            if (strcmp(ctx->labels[i].label_name, p_label->p_identifier_opt->lexeme) == 0)
            {

                arena_restore_current_state_from(ctx, ctx->labels[i].state_number);

                break; //already exist
            }
        }
    }
    else
    {
        //case, default
        arena_restore_current_state_from(ctx, ctx->initial_state);
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
    struct block_item* _Opt p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static enum flow_state parse_string_state(const char* s, bool* invalid)
{
    //TODO faling with _
    *invalid = false;

    enum flow_state e = 0;
    const char* p = s;

    while (*p)
    {
        if (isalpha(*p))
        {
            int sz = 0;
            const char* start = p;
            while (isalpha(*p) || *p == '-')
            {
                sz++;
                p++;
            }

            if (strncmp(start, "moved", sz) == 0)
                e |= FLOW_OBJECT_STATE_MOVED;

            else if (strncmp(start, "null", sz) == 0)
                e |= FLOW_OBJECT_STATE_NULL;
            else if (strncmp(start, "not-null", sz) == 0)
                e |= FLOW_OBJECT_STATE_NOT_NULL;
            else if (strncmp(start, "maybe-null", sz) == 0)
                e |= (FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL);
            else if (strncmp(start, "uninitialized", sz) == 0)
                e |= FLOW_OBJECT_STATE_UNINITIALIZED;
            else if (strncmp(start, "zero", sz) == 0)
                e |= FLOW_OBJECT_STATE_ZERO;
            else if (strncmp(start, "not-zero", sz) == 0)
                e |= FLOW_OBJECT_STATE_NOT_ZERO;
            else if (strncmp(start, "any", sz) == 0)
                e |= (FLOW_OBJECT_STATE_NOT_ZERO | FLOW_OBJECT_STATE_ZERO);
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

    struct true_false_set a = { 0 };
    flow_visit_expression(ctx, p_static_assert_declaration->constant_expression, &a);

    ctx->expression_is_not_evaluated = t2; //restore


    if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG ||
        p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG_EX)
    {
        bool ex = p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG_EX;

        compiler_diagnostic_message(W_LOCATION, ctx->ctx, p_static_assert_declaration->first_token, NULL, "static_debug");

        struct flow_object* _Opt p_obj =
            expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);

        if (p_obj)
        {
            print_flow_object(&p_static_assert_declaration->constant_expression->type, p_obj, !ex);
            if (p_obj->is_temporary)
            {
                p_obj->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
            }
        }

        if (ex)
        {
            print_arena(ctx);
        }
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_STATE)
    {
        /*TODO
           check state

        */

        bool is_invalid = false;
        enum flow_state e = 0;
        if (p_static_assert_declaration->string_literal_opt)
            e = parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);
        if (is_invalid)
        {
            compiler_diagnostic_message(C_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assert_declaration->first_token, NULL, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
        }
        else
        {

            struct flow_object* _Opt p_obj =
                expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);
            if (p_obj)
            {


                if (e != p_obj->current.state)
                {
                    compiler_diagnostic_message(C_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assert_declaration->first_token, NULL, "static_state failed");
                    if (p_static_assert_declaration->string_literal_opt)
                        printf("expected :%s\n", p_static_assert_declaration->string_literal_opt->lexeme);
                    printf("current  :");
                    flow_object_print_state(p_obj);
                    printf("\n");
                }
            }
            else
            {
                if (e != FLOW_OBJECT_STATE_NOT_APPLICABLE)
                {
                    compiler_diagnostic_message(C_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assert_declaration->first_token, NULL, "static_state failed");
                }
            }

            if (p_obj && p_obj->is_temporary)
            {
                p_obj->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
            }

        }
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_SET)
    {
        struct flow_object* _Opt p_obj =
            expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);

        if (p_obj)
        {
            if (p_static_assert_declaration->string_literal_opt)
            {
                const char* lexeme =
                    p_static_assert_declaration->string_literal_opt->lexeme;

                if (strcmp(lexeme, "\"zero\"") == 0)
                {
                    //gives the semantics of {0} or calloc
                    flow_object_set_zero(&p_static_assert_declaration->constant_expression->type, p_obj);
                }
                else
                {
                    bool is_invalid = false;
                    enum flow_state e =
                        parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);

                    if (!is_invalid)
                    {
                        if (p_obj->members.size > 0)
                        {
                            compiler_diagnostic_message(C_ERROR_STATIC_SET, ctx->ctx, p_static_assert_declaration->first_token, NULL, "use only for non agregates");
                        }
                        p_obj->current.state = e;
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_STATIC_SET, ctx->ctx, p_static_assert_declaration->first_token, NULL, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
                    }
                }
            }

        }
        if (p_obj && p_obj->is_temporary)
        {
            p_obj->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
        }

    }

    true_false_set_destroy(&a);
}

static void flow_visit_direct_declarator(struct flow_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
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

            flow_visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);

            if (parameter->declarator)
            {
                flow_visit_declarator(ctx, parameter->declarator);
            }

            parameter = parameter->next;
        }

        ctx->parameter_list--;

    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            struct true_false_set a = { 0 };
            flow_visit_expression(ctx, p_direct_declarator->array_declarator->assignment_expression, &a);
            true_false_set_destroy(&a);
        }

    }
}

static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct declarator* p_declarator)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
    try
    {
        if (
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


            p_declarator->p_flow_object = make_flow_object(ctx, &p_declarator->type, p_declarator, NULL);
            if (p_declarator->p_flow_object == NULL)
            {
                throw;
            }

            flow_object_set_uninitialized(&p_declarator->type, p_declarator->p_flow_object);


            if (p_declarator->declaration_specifiers &&
                p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
            {
                if (type_is_pointer(&p_declarator->type))
                {
                    if (type_is_nullable(&p_declarator->type, ctx->ctx->options.null_checks_enabled))
                    {
                        p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
                    }
                    else
                    {
                        p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_NULL;
                    }

                    if (type_is_pointer_to_out(&p_declarator->type))
                    {
                        struct type t = type_remove_pointer(&p_declarator->type);
                        struct flow_object* _Opt po = make_flow_object(ctx, &t, p_declarator, NULL);
                        if (po == NULL)
                        {
                            type_destroy(&t);
                            throw;
                        }
                        flow_object_set_uninitialized(&t, po);
                        object_set_pointer(p_declarator->p_flow_object, po); //MOVED                    
                        type_destroy(&t);
                    }
                    else if (type_is_any_owner(&p_declarator->type))
                    {
                        struct type t = type_remove_pointer(&p_declarator->type);
                        struct flow_object* _Opt po = make_flow_object(ctx, &t, p_declarator, NULL);
                        if (po == NULL)
                        {
                            type_destroy(&t);
                            throw;
                        }
                        const bool t_is_nullable = type_is_nullable(&t, ctx->ctx->options.null_checks_enabled);
                        flow_object_set_unknown(&t, t_is_nullable, po, nullable_enabled);
                        object_set_pointer(p_declarator->p_flow_object, po); //MOVED                    
                        type_destroy(&t);
                    }
                }
                else if (type_is_struct_or_union(&p_declarator->type))
                {
                    const bool is_nullable = type_is_nullable(&p_declarator->type, nullable_enabled);
                    flow_object_set_unknown(&p_declarator->type, is_nullable, p_declarator->p_flow_object, nullable_enabled);
                }
                else if (type_is_array(&p_declarator->type))
                {
                    // assert(false);//TODO
                     //flow_object_set_unknown(&p_declarator->type, &p_declarator->object);
                    p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_ZERO;
                }
                else
                {
                    p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_ZERO | FLOW_OBJECT_STATE_NOT_ZERO;
                }


#if 0
                if (type_is_pointer(&p_declarator->type))
                {
                    //TODO necessary?
                    struct type t2 = type_remove_pointer(&p_declarator->type);
                    if (p_declarator->p_object->pointed)
                    {
                        set_object(&t2, p_declarator->p_object->pointed, (FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL));
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
    catch
    {
    }
}

static void flow_visit_init_declarator_list(struct flow_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;
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
    struct member_declarator* _Opt p_member_declarator = p_member_declarator_list->head;
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
        flow_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void flow_visit_member_declaration_list(struct flow_visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* _Opt p_member_declaration = p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    flow_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);
}

static void flow_visit_enumerator(struct flow_visit_ctx* ctx, struct enumerator* p_enumerator)
{
    struct true_false_set a = { 0 };
    if (p_enumerator->constant_expression_opt)
        flow_visit_expression(ctx, p_enumerator->constant_expression_opt, &a);
    true_false_set_destroy(&a);
}

static void flow_visit_enumerator_list(struct flow_visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* _Opt current = p_enumerator_list->head;
    while (current)
    {
        flow_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    flow_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}


static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        flow_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }
}

static void flow_visit_type_specifier_qualifier(struct flow_visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_specifier)
    {
        flow_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
}

static void flow_visit_declaration_specifier(struct flow_visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{
    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);
    }
}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx,
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
        flow_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}

/*
* retorna true se o ultimo item for um return
*/
static bool flow_is_last_item_return(struct compound_statement* p_compound_statement)
{
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wflow-not-null"

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

#pragma CAKE diagnostic pop
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

    if (p_declaration->init_declarator_list.head)
    {
        flow_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        assert(ctx->p_return_type == NULL);

        struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        ctx->p_return_type = &type;

        flow_visit_compound_statement(ctx, p_declaration->function_body);
        type_destroy(&type);
        ctx->p_return_type = NULL;
    }
}

void flow_start_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration)
{
    ctx->labels_size = 0;
    flow_objects_clear(&ctx->arena);

    ctx->state_number_generator = 1; //reserva 0 p current

    if (p_declaration->function_body)
    {
        flow_visit_declaration(ctx, p_declaration);
        assert(p_declaration->function_body != NULL); //flow_visit_declaration does not change this

        if (!flow_is_last_item_return(p_declaration->function_body))
        {
            flow_exit_block_visit_defer_list(ctx, &p_declaration->defer_list, p_declaration->function_body->last_token);
        }
    }
    else
    {
        flow_visit_declaration(ctx, p_declaration);
    }

    flow_objects_clear(&ctx->arena);
}

#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wanalyzer-maybe-uninitialized" 

_Opt struct flow_object* _Opt arena_new_object(struct flow_visit_ctx* ctx)
{
    _Opt struct flow_object* _Owner _Opt p = calloc(1, sizeof * p);
    if (p != NULL)
    {
        p->current.dbg_name = "current";
        p->id = ctx->arena.size + 1;
        if (flow_objects_push_back(&ctx->arena, p) != 0)
        {
            p = NULL;
        }
    }
    return (struct flow_object* _Opt)p; //warning removed
}

#pragma CAKE diagnostic pop


void flow_visit_ctx_destroy(struct flow_visit_ctx* _Obj_owner p)
{
    flow_objects_destroy(&p->arena);
}

static void flow_analysis_visit(struct flow_visit_ctx* ctx)
{
    struct declaration* _Opt p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        flow_visit_declaration(ctx, p_declaration);
        p_declaration = p_declaration->next;
    }
}

