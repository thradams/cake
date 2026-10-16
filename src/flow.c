
#pragma safety enable


#include "cake_compat.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "flow.h"
#include "expressions.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "console.h"
#include <stdio.h>
#include "osstream.h"

/*
    flow reaches the object an expression stands for through its REF alternatives
    (flow_branch_search_up); p_ref is a parse-time link that is NULL or a
    placeholder for *p, p->x and p[i]. Using it in this file is a build error.
*/
#define object_get_referenced(p_object) object_get_referenced_is_not_used_in_flow_c(p_object)

#define FLOW_PARAMETER_OBJECT_INIT_MAX_DEPTH 6

struct flow_true_false_branches
{
    struct flow_branch* _Opt p_true;
    struct flow_branch* _Opt p_false;
};
 
static void flow_check_dianostic_suppression(struct flow_ctx* ctx, const struct token* p_token);

static void flow_check_file_scope_objects_at_function_exit(const struct flow_ctx* ctx);

static struct flow_true_false_branches flow_visit_expression(struct flow_ctx* ctx, const struct expression* _Opt p_expression);
static void object_static_debug(struct flow_ctx* ctx, const struct object* p_object, struct token* first_token, struct token* last_token);

static void flow_check_object_at_exit(struct flow_ctx* ctx, const struct type* p_type, const struct object* p_obj, const struct marker* marker, const struct token* p_exit_token, bool in_view, const char* _Opt p_root_name_opt, bool root_is_pointer);
static void flow_check_arena_objects_at_function_exit(const struct flow_ctx* ctx);
static void flow_check_write_qualified_params_at_exit(struct flow_ctx* ctx, const struct marker* marker, const struct token* p_exit_token);
static void flow_check_exiting_objects(struct flow_ctx* ctx, const struct expression* _Opt p_return_expression, const struct marker* marker);
static void flow_check_discarding_owner_before_overwrite(struct flow_ctx* ctx, const struct expression* p_expression_dest, const struct object* _Opt p_object_dest, const struct marker* marker);
static void flow_seed_member_default(struct flow_ctx* ctx, const struct object* _Opt member_obj, const struct token* _Opt p_token);

/* p_object now holds exactly one SIGNED value, set here. False when out of memory. */
static bool flow_set_object_signed(struct flow_ctx* ctx,
                                   const struct object* p_object,
                                   long long value,
                                   enum flow_relation relation,
                                   const struct token* _Opt p_token)
{
    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, p_object);
    if (e == NULL)
        return false;
    flow_alternatives_clear(&e->alternatives);
    struct flow_alternative a =
    {
        .value_kind = FLOW_VALUE_KIND_SIGNED,
        .value = {.i = value},
        .value_relation = relation,
        .imaginary = FLOW_IMAGINARY_NONE,
        .p_origin_map = ctx->p_current_flow_branch,
        .p_origin_token = p_token
    };
    flow_alternatives_add(&e->alternatives, &a);
    return true;
}

enum init_type
{
    INIT_PARAMETER,
    INIT_RETURN,
    INIT_OBJ
};

static void flow_check_object_init_assigment(struct flow_ctx* ctx,
                                             struct expression* p_expression,
                                             const struct object* _Opt p_object_dest, /* uninitialized alawys */
                                             const struct object* _Opt p_object_src,
                                             enum init_type init_type,
                                             bool dest_is_dtor,
                                             bool dest_is_view);

static void flow_branch_derive_implied_facts(struct flow_branch* p_arm, struct flow_branch* p_before);
static bool flow_origins_compatible(const struct flow_branch* _Opt o1, const struct flow_branch* _Opt o2);
static struct flow_branch* _Opt flow_branch_new_pointer_target_arm(struct flow_ctx* ctx, struct flow_branch* p_before, const struct flow_branch* _Opt p_target_origin);
/*
    After the write in a target arm, tag the written value with the arm
    (as origin, or as narrowed-from map).
*/
static void flow_branch_tag_pointer_target_arm(const struct flow_branch* p_arm);
static bool flow_target_origins_exclusive(const struct flow_branch* _Opt const target_origins[], int target_count);

static void flow_widen_loop_variant_objects(struct flow_branch* _Opt p_pass1_exit,
                                            struct flow_branch* _Opt p_pass2_exit,
                                            struct flow_branch* _Opt* arms,
                                            int num_arms,
                                            const struct token* _Opt p_token,
                                            bool allow_repeated_value);

static void flow_apply_alloc_contract_to_dest(struct flow_ctx* ctx,
                                              const struct type* _Opt p_dest_type,
                                              const struct object* _Opt p_object_dest,
                                              const struct expression* _Opt p_src_expression);

static bool object_is_file_scope(const struct object* p_object)
{
    const enum storage_class_specifier_flags flags =
        p_object->type.storage_class_specifier_flags;

    if (flags & (STORAGE_SPECIFIER_PARAMETER | STORAGE_SPECIFIER_BLOCK_SCOPE))
    {
        return false;
    }

    if (flags & (STORAGE_SPECIFIER_EXTERN | STORAGE_SPECIFIER_STATIC))
    {
        return true;
    }

    return false;
}

static long long flow_cast_integer_value(const struct flow_ctx* ctx, long long value, const struct type* _Opt target_type)
{
    try
    {
        if (target_type == NULL) return value;
        if (!type_is_integer(target_type)) return value;

        size_t width = 1;
        enum sizeof_result r = type_get_sizeof(target_type, &width, &ctx->ctx->options.platform);
        if (r != SIZEOF_RESULT_OK)
            throw;

        width *= 8; /* type_get_sizeof returns bytes; we need bits */

        if (width >= (int)(sizeof(long long) * 8))
        {
            /* Target can represent the full range of long long, no change. */
            return value;
        }

        unsigned long long mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
        unsigned long long uval = (unsigned long long)value & mask;

        if (type_is_unsigned_integer(target_type))
        {
            return (long long)uval;
        }
        else
        {
            /* Signed: interpret as width-bit two's complement. */
            long long sign_bit = 1LL << (width - 1);
            if (uval & sign_bit)
            {
                return (long long)(uval - (1ULL << width));
            }
            else
            {
                return (long long)uval;
            }
        }
    }
    catch
    {
    }
    return value;
}


void print_object_ptr(struct osstream* ss, const struct object* _Opt p, bool is_parent)
{
    if (p == NULL)
    {
        ss_fprintf(ss, "null");
        return;
    }

    static const char digits[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    uint64_t value = (uint64_t)(uintptr_t)p;

    char temp[16] = { 0 };
    int i = 0;

    while (value > 0 && i < (int)sizeof(temp))
    {
        temp[i++] = digits[value % 62];
        value /= 62;
    }

    if (!is_parent)
    {
        ss_fprintf(ss, "'");
    }
    if (p->parent)
    {
        print_object_ptr(ss, p->parent, true);
    }

    while (i > 0)
    {
        ss_fprintf(ss, "%c", temp[--i]);
    }

    ss_fprintf(ss, ":");
    if (p->member_designator)
    {
        ss_fprintf(ss, "%s", p->member_designator);
    }

    if (!is_parent)
    {
        ss_fprintf(ss, "'");
    }
}

/* Is x OP rhs true for a single scalar x? */
static bool flow_scalar_relation_holds(long long x, enum expression_type op, long long rhs)
{
    switch (op)
    {
        case EXPR_RELATIONAL_BIGGER_THAN:
            return x > rhs;
        case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
            return x >= rhs;
        case EXPR_RELATIONAL_LESS_THAN:
            return x < rhs;
        case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
            return x <= rhs;
        default:
            break;
    }
    return false;
}

#define FLOW_BRANCH_PATH_MAX_CHAIN 128

static void flow_diagnose_map_path(const struct flow_ctx* ctx, const struct flow_branch* _Opt map)
{
    if (map == NULL)
        return;

    /*
        Collect deepest-first then emit backwards, so notes read root-first,
        in the order control flow took them (same shape as
        flow_explain_origin / flow_branch_debug_print).
    */
    const struct flow_branch* chain[FLOW_BRANCH_PATH_MAX_CHAIN];
    int chain_len = 0;
    for (const struct flow_branch* _Opt cur = map; cur; cur = cur->p_parent_map)
    {
        if (chain_len < FLOW_BRANCH_PATH_MAX_CHAIN)
            chain[chain_len] = cur;
        chain_len++;
    }
    const int collected = chain_len < FLOW_BRANCH_PATH_MAX_CHAIN ? chain_len : FLOW_BRANCH_PATH_MAX_CHAIN;

    /*
        Dedup (condition, side) pairs: a loop body is walked more than once.
        Keyed on expression+kind, so a condition taken both ways shows both.
    */
    const struct expression* seen_expr[FLOW_BRANCH_PATH_MAX_CHAIN] = { 0 };
    enum flow_branch_kind seen_kind[FLOW_BRANCH_PATH_MAX_CHAIN] = { 0 };
    int seen_count = 0;

    for (int i = collected - 1; i >= 0; i--)
    {
        const struct flow_branch* m = chain[i];

        if (m->p_branch_expr == NULL)
            continue;

        bool already_reported = false;
        for (int s = 0; s < seen_count; s++)
        {
            if (seen_expr[s] == m->p_branch_expr && seen_kind[s] == m->kind)
            {
                already_reported = true;
                break;
            }
        }
        if (already_reported)
            continue;

        if (seen_count < FLOW_BRANCH_PATH_MAX_CHAIN)
        {
            seen_expr[seen_count] = m->p_branch_expr;
            seen_kind[seen_count] = m->kind;
            seen_count++;
        }

        /* Underline the whole condition, not just its first token. */
        const struct marker branch_marker =
        {
            .p_token_begin = m->p_branch_expr->first_token,
            .p_token_end = m->p_branch_expr->last_token,
        };

        const bool is_true_branch = (m->kind == FLOW_BRANCH_TRUE_BRANCH);

        /* One note per decision: the assumption. The branch taken follows from it. */
        struct osstream cond_ss = { 0 };
        flow_expression_to_string(m->p_branch_expr, &cond_ss);
        const char* cond = cond_ss.c_str ? cond_ss.c_str : "";

        /*
            Phrase the assumption in terms of the condition's own type. For a
            pointer, "is null"/"is non-null" is what the reader cares about
            and what the resulting warning will talk about; for anything else
            claiming nullness would be wrong, so fall back to true/false.
        */
        const char* assumption;
        if (type_is_pointer(&m->p_branch_expr->object.type))
            assumption = is_true_branch ? "is non-null" : "is null";
        else
            assumption = is_true_branch ? "is true" : "is false";

        diagnostic(W_LOCATION, ctx->ctx, NULL, &branch_marker,
                   "Assuming '%s' %s%s", cond, assumption,
                   m->is_unreachable ? " (this path is unreachable)" : "");

        ss_close(&cond_ss);
    }
}

/* the parameter of the current function whose state was seeded at p_token (parameters are seeded at the first token of their declaration specifiers) */
static const struct parameter_declaration* _Opt flow_find_parameter_seeded_at(const struct flow_ctx* ctx, const struct token* p_token)
{
    if (ctx->p_current_function_declaration == NULL ||
        ctx->p_current_function_declaration->init_declarator_list.head == NULL)
    {
        return NULL;
    }

    const struct function_declarator* _Opt p_fd =
        declarator_find_function_declarator(ctx->p_current_function_declaration->init_declarator_list.head->p_declarator);
    if (p_fd == NULL ||
        p_fd->parameter_type_list_opt == NULL ||
        p_fd->parameter_type_list_opt->parameter_list == NULL)
    {
        return NULL;
    }

    for (const struct parameter_declaration* _Opt p_param = p_fd->parameter_type_list_opt->parameter_list->head;
         p_param;
         p_param = p_param->next)
    {
        if (p_param->declaration_specifiers->first_token == p_token)
            return p_param;
    }
    return NULL;
}

static void flow_diagnose_state_origin(const struct flow_ctx* ctx,
                                       const struct flow_alternative* p_alternative,
                                       const struct marker* p_fallback_marker,
                                       enum diagnostic_id id,
                                       int path_number)
{
    if (p_alternative->p_origin_token == NULL)
    {
        return; /* state seeded with nothing to blame */
    }

    const struct token* _Opt p_at = p_fallback_marker->p_token_caret ?
        p_fallback_marker->p_token_caret : p_fallback_marker->p_token_begin;

    if (path_number == 0 && p_at != NULL && p_at->line == p_alternative->p_origin_token->line)
    {
        return; /* same line as the warning -- says it twice; a numbered path still needs its label */
    }

    /*
        Underline the condition and put the caret on the token, only when both
        are on one line (the printed source line comes from the tokens).
    */
    const struct expression* _Opt p_state_expr =
        p_alternative->p_origin_map ? p_alternative->p_origin_map->p_branch_expr : NULL;

    const bool span_condition =
        p_state_expr != NULL &&
        p_state_expr->first_token->line == p_alternative->p_origin_token->line &&
        p_state_expr->last_token->line == p_alternative->p_origin_token->line;

    struct marker state_marker =
    {
        .p_token_caret = p_alternative->p_origin_token,
        .p_token_begin = span_condition && p_state_expr ? p_state_expr->first_token : p_alternative->p_origin_token,
        .p_token_end = span_condition && p_state_expr ? p_state_expr->last_token : p_alternative->p_origin_token,
    };

    /* a parameter is seeded at its declaration: nothing happened there, the value came with the call */
    const struct parameter_declaration* _Opt p_param = flow_find_parameter_seeded_at(ctx, p_alternative->p_origin_token);
    const bool is_parameter = p_param != NULL;
    if (p_param && p_param->declarator && p_param->declarator->name_opt)
    {
        /* point at the parameter's name, not at the start of its type */
        state_marker.p_token_caret = p_param->declarator->name_opt;
        state_marker.p_token_begin = p_param->declarator->name_opt;
        state_marker.p_token_end = p_param->declarator->name_opt;
    }

    /* what happened at the origin depends on what the warning is about */
    const char* what =
        is_parameter ? "value received from the caller" :
        id == W_FLOW_MISSING_DTOR || id == W_FLOW_DISCARDING_OWNER ? "resource acquired here" :
        id == W_FLOW_UNINITIALIZED ? "uninitialized from here" :
        id == W_FLOW_LIFETIME_ENDED ? "lifetime ended here" :
        id == W_FLOW_MOVED || id == W_FLOW_PARAM_OWNER_CONSUMED_AT_EXIT ? "moved here" :
        "value set here";

    /* path_number > 0 numbers the paths when one finding reports several */
    if (path_number > 0)
        diagnostic(W_LOCATION, ctx->ctx, NULL, &state_marker, "%s (path %d)", what, path_number);
    else
        diagnostic(W_LOCATION, ctx->ctx, NULL, &state_marker, "%s", what);
}

static void flow_explain_alternative(const struct flow_ctx* ctx,
                                     const struct flow_alternative* p_alternative,
                                     const struct flow_branch* _Opt p_alternative_map,
                                     const struct marker* p_marker)
{
    flow_diagnose_state_origin(ctx, p_alternative, p_marker, W_LOCATION, 0);
    flow_diagnose_map_path(ctx, p_alternative_map);
}

static struct flow_true_false_branches flow_ensure_branch_pair(struct flow_ctx* ctx,
                                                       struct flow_branch* _Opt p_fallback,
                                                       struct flow_true_false_branches pair,
                                                       const struct expression* _Opt p_expr)
{
    if (pair.p_true == pair.p_false)
    {
        struct flow_branch* _Opt base = pair.p_true ? pair.p_true : p_fallback;

        pair.p_true = flow_branch_arena_new_branch(&ctx->flow_branch_arena, base, true, p_expr);
        pair.p_false = flow_branch_arena_new_branch(&ctx->flow_branch_arena, base, false, p_expr);
        if (pair.p_true == NULL || pair.p_false == NULL)
        {
            /* allocation failure: degrade to the old identity behaviour */
            pair.p_true = base;
            pair.p_false = base;
        }
        flow_tag_branch_pair(pair.p_true, pair.p_false);
        return pair;
    }

    /* When one arm of an already-distinct branch pair still literally IS p_fallback, wrap it in its own fresh child map -- otherwise the branch body mutating it in place (e.g. marking it unreachable) corrupts the shared ancestor p_fallback and misreports unrelated code as unreachable. */
    if (pair.p_true == p_fallback)
    {
        struct flow_branch* _Opt fresh = flow_branch_arena_new_branch(&ctx->flow_branch_arena, pair.p_true, true, p_expr);
        if (fresh != NULL)
        {
            pair.p_true = fresh;
        }
    }
    if (pair.p_false == p_fallback)
    {
        struct flow_branch* _Opt fresh = flow_branch_arena_new_branch(&ctx->flow_branch_arena, pair.p_false, false, p_expr);
        if (fresh != NULL)
        {
            pair.p_false = fresh;
        }
    }
    return pair;
}


static void flow_visit_unlabeled_statement(struct flow_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement);
static void flow_visit_static_assertion(struct flow_ctx* ctx, const struct static_assertion* p_static_assertion);
static void flow_visit_declaration(struct flow_ctx* ctx, struct declaration* p_declaration);
static void flow_visit_secondary_block(struct flow_ctx* ctx, struct secondary_block* _Opt p_secondary_block);
static void flow_visit_struct_or_union_specifier(struct flow_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void flow_visit_statement(struct flow_ctx* ctx, struct statement* p_statement);
static void flow_visit_enum_specifier(struct flow_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void flow_visit_type_specifier(struct flow_ctx* ctx, struct type_specifier* p_type_specifier);
static void flow_visit_bracket_initializer_list(struct flow_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);
static void flow_visit_expression_statement(struct flow_ctx* ctx, const struct expression_statement* p_expression_statement);
static void flow_visit_block_item(struct flow_ctx* ctx, struct block_item* p_block_item);
static void flow_visit_initializer(struct flow_ctx* ctx, struct initializer* p_initializer);
static void flow_visit_declarator(struct flow_ctx* ctx, const struct declarator* p_declarator);
static void flow_visit_label(struct flow_ctx* ctx, const struct label* p_label);

static struct flow_true_false_branches flow_visit_full_expression(struct flow_ctx* ctx, const struct expression* p_expression);
static struct flow_true_false_branches flow_visit_condition(struct flow_ctx* ctx, const struct expression* p_expression);


static void flow_exit_block_visit_defer_item(struct flow_ctx* ctx, const struct defer_list_item* p_item, const struct token* position_token)
{
    if (p_item->defer_statement)
    {
        /*
          Diagnostics are queued, not printed, while flow runs. Each one reported
          inside the defer body gets a note pointing where the defer runs
          (the return, break, goto or end of block), because the same body runs
          at every exit and the problem may exist only at one of them.
        */
        struct diagnostic_queue* q = &ctx->ctx->diagnostic_queue;
        struct diagnostic_item* _Opt p_last_before = q->tail;
        const int count_before = q->count;

        if (p_item->defer_statement->unlabeled_statement)
            flow_visit_unlabeled_statement(ctx, p_item->defer_statement->unlabeled_statement);

        if (q->count > count_before)
        {
            struct diagnostic_item* _Opt p_real_tail = q->tail;
            struct diagnostic_item* _Opt it = p_last_before ? p_last_before->next : q->head;
            while (it)
            {
                /* W_LOCATION attaches to the queue tail */
                q->tail = it;
                diagnostic(W_LOCATION, ctx->ctx, position_token, NULL, "defer executed here");
                it = it->next;
            }
            q->tail = p_real_tail;
        }
    }
    else if (p_item->declarator)
    {
        struct declarator* p_declarator = p_item->declarator;
        struct token* _Opt p_token = NULL;
        if (p_declarator->name_opt)
            p_token = p_declarator->name_opt;
        else if (p_declarator->specifier_qualifier_list)
            p_token = p_declarator->specifier_qualifier_list->first_token;
        else if (p_declarator->declaration_specifiers)
            p_token = p_declarator->declaration_specifiers->first_token;
        else
            _Assert(false);

        const struct marker marker = { .p_token_begin = p_token, .p_token_end = p_token };
        flow_check_object_at_exit(ctx,
                                  &p_declarator->object.type,
                                  &p_declarator->object,
                                  &marker,
                                  position_token,
                                  false,
                                  p_declarator->name_opt ? p_declarator->name_opt->lexeme : NULL,
                                  false);
    }
    else if (p_item->compound_literal)
    {
        struct expression* p_literal = p_item->compound_literal;
        const struct marker marker = { .p_token_begin = p_literal->first_token, .p_token_end = p_literal->last_token };
        flow_check_object_at_exit(ctx,
                                  &p_literal->object.type,
                                  &p_literal->object,
                                  &marker,
                                  position_token,
                                  false,
                                  NULL,
                                  false);
    }
}

static void flow_exit_block_visit_defer_list(struct flow_ctx* ctx,
                                             const struct defer_list* p_defer_list,
                                             const struct token* position_token)
{
    if (ctx->stopped)
        return;

    /*
      { defer free(p); return 0; }
      the end of the block is never reached: the defers already ran at the return
    */
    if (ctx->p_current_flow_branch != NULL && ctx->p_current_flow_branch->is_unreachable)
        return;

    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow_exit_block_visit_defer_item(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow_defer_item_set_end_of_lifetime(struct flow_ctx* ctx, struct defer_list_item* p_item, const struct token* position_token)
{
    try
    {
        if (ctx->p_current_flow_branch == NULL)
        {
            throw;
        }

        if (p_item->defer_statement)
        {
            /* nothing */
        }
        else if (p_item->declarator)
        {
            struct declarator* p_declarator = p_item->declarator;

            if (!is_automatic_variable(p_declarator->object.type.storage_class_specifier_flags))
            {
                /* not local */
                return;
            }

            const struct token* _Opt p_token = position_token;
            flow_branch_set_object_lifetime_ended(ctx->p_current_flow_branch,
                                               &p_declarator->object,
                                               p_token);
        }
        else if (p_item->compound_literal)
        {
            flow_branch_set_object_lifetime_ended(ctx->p_current_flow_branch,
                                               &p_item->compound_literal->object,
                                               position_token);
        }
    }
    catch
    {
    }
}

static void flow_defer_list_set_end_of_lifetime(struct flow_ctx* ctx,
                                                const struct defer_list* p_defer_list,
                                                const struct token* position_token)
{
    if (ctx->stopped)
        return;

    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow_defer_item_set_end_of_lifetime(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow_visit_secondary_block(struct flow_ctx* ctx, struct secondary_block* _Opt p_secondary_block)
{
    /*
        _Owner _Opt in the AST: an absent secondary block is an empty body,
        so there is simply nothing to visit. Guarded here rather than at each
        of the fourteen call sites.
    */
    if (p_secondary_block == NULL)
        return;

    flow_visit_statement(ctx, p_secondary_block->statement);
}

static void flow_object_init(struct flow_ctx* ctx, struct object* p_object, const struct token* _Opt p_token)
{
    if (ctx->p_current_flow_branch == NULL)
    {
        return;
    }

    if (p_object->members.head)
    {
        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, p_object);
        if (e == NULL) return;

        flow_alternatives_clear(&e->alternatives);
        struct flow_alternative a =
        {
            .value_kind = FLOW_VALUE_KIND_REF,
            .value = {.p = p_object},
            .value_relation = FLOW_RELATION_EQUAL,
            .imaginary = FLOW_IMAGINARY_NONE,
            .p_origin_map = ctx->p_current_flow_branch,
            .p_origin_token = p_token
        };
        flow_alternatives_add(&e->alternatives, &a);

        struct object* _Opt  p_object_it = p_object->members.head;
        for (; p_object_it; p_object_it = p_object_it->next)
        {
            flow_object_init(ctx, p_object_it, p_token);
        }

        return;
    }

    enum flow_relation relation = FLOW_RELATION_UNINITIALIZED;

    struct flow_alternative value = { 0 };
    if (type_is_pointer(&p_object->type))
    {
        value.value_kind = FLOW_VALUE_KIND_PTR;
        value.value.p = (void*)(uintptr_t)p_object->value.host_u_long_long;
    }
    else if (type_is_signed(&p_object->type))
    {
        value.value_kind = FLOW_VALUE_KIND_SIGNED;
        value.value.i = p_object->value.host_long_long;
    }
    else
    {
        value.value_kind = FLOW_VALUE_KIND_UNSIGNED;
        value.value.u = p_object->value.host_u_long_long;
    }

    switch (p_object->state)
    {
        case CONSTANT_VALUE_STATE_UNINITIALIZED:
            relation = FLOW_RELATION_UNINITIALIZED;
        break;

        case CONSTANT_VALUE_STATE_CONSTANT:
        case CONSTANT_VALUE_EQUAL:
            relation = FLOW_RELATION_EQUAL;
        break;

        case CONSTANT_VALUE_STATE_ANY:
            relation = FLOW_RELATION_ANY;
        break;
    }

    {
        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, p_object);
        if (e == NULL)
            return; /* no entry to seed */

        flow_alternatives_clear(&e->alternatives);
        struct flow_alternative a =
        {
            .value_kind = value.value_kind,
            .value = value.value,
            .value_relation = relation,
            .imaginary = FLOW_IMAGINARY_NONE,
            .p_origin_map = ctx->p_current_flow_branch,
            .p_origin_token = p_token
        };
        flow_alternatives_add(&e->alternatives, &a);
    }
}

static void flow_parameter_object_init_r(struct flow_ctx* ctx, struct object* p_object, const struct type* p_type, const struct token* _Opt p_token, int depth, bool force_opt);

static void flow_parameter_object_init(struct flow_ctx* ctx, struct object* p_object, const struct type* p_type, const struct token* _Opt p_token)
{
    flow_parameter_object_init_r(ctx, p_object, p_type, p_token, 0, false);
}

static void flow_parameter_object_init_r(struct flow_ctx* ctx, struct object* p_object, const struct type* p_type, const struct token* _Opt p_token, int depth, bool force_opt)
{
    if (ctx->p_current_flow_branch == NULL)
        return;

    const bool nullable_enabled = options_default_nonnull(&ctx->ctx->options);

    try
    {
        if (p_object->members.head)
        {
            struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, p_object);
            if (e == NULL) throw;
            flow_alternatives_clear(&e->alternatives);
            struct flow_alternative a =
            {
                .value_kind = FLOW_VALUE_KIND_REF,
                .value = {.p = p_object},
                .value_relation = FLOW_RELATION_EQUAL,
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = ctx->p_current_flow_branch,
                .p_origin_token = p_token
            };
            flow_alternatives_add(&e->alternatives, &a);

            for (struct object* _Opt p_member = p_object->members.head;
                    p_member != NULL;
                    p_member = p_member->next)
            {
                if (type_is_array(&p_member->type) && p_member->type.array_num_elements > 0 && p_member->members.head == NULL)
                {
                    continue; /* array not made yet (MAKE_STATE_ANY_LAZY_ARRAYS) */
                }
                flow_parameter_object_init_r(ctx, p_member, &p_member->type, p_token, depth, force_opt);
            }
            return;
        }

        enum flow_relation relation = FLOW_RELATION_UNINITIALIZED;

        struct flow_alternative value = { 0 };
        if (type_is_pointer(&p_object->type))
        {
            value.value_kind = FLOW_VALUE_KIND_PTR;
            value.value.p = NULL;
        }
        else if (type_is_signed(&p_object->type))
        {
            value.value_kind = FLOW_VALUE_KIND_SIGNED;
            value.value.i = p_object->value.host_long_long;
        }
        else
        {
            value.value_kind = FLOW_VALUE_KIND_UNSIGNED;
            value.value.u = p_object->value.host_u_long_long;
        }

        switch (p_object->state)
        {
            case CONSTANT_VALUE_STATE_UNINITIALIZED:
                relation = FLOW_RELATION_UNINITIALIZED;
            break;

            case CONSTANT_VALUE_STATE_CONSTANT:
            case CONSTANT_VALUE_EQUAL:
                relation = FLOW_RELATION_EQUAL;
            break;

            case CONSTANT_VALUE_STATE_ANY:
                relation = FLOW_RELATION_ANY;
            break;
        }

        /*parameter are any by default*/
        relation = FLOW_RELATION_ANY;

        /* Pointer parameter initial state: nullable_enabled + non-_Opt => assumed not-null with an arena alias, pointee ANY; nullable_enabled + _Opt => two correlated null/non-null alternatives (below); nullable disabled => ANY (conservative, no null-check enforcement). */
        if (nullable_enabled &&
                p_type != NULL &&
                (type_is_pointer(p_type) || type_is_array(p_type)) &&
                !type_is_nullable(p_type, nullable_enabled) &&
                !force_opt)
        {
            /* Array parameters decay to pointers and are non-null too. */
            /*
                Only the parameter's own pointee is made here; deeper pointees stay
                "!= null" with no target and are made on demand at `->`. An _Out
                pointee is always made: its members start uninitialized.
            */
            struct object* _Opt p_pointed = NULL;
            if (depth == 0 || type_is_pointed_out(p_type))
            {
                p_pointed = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
            }
            /*
                `_Opt struct X* p` qualifies the POINTEE: p is non-null, but members
                reached through it are force_opt, like an _Opt pointer's pointee.
                `struct X* _Opt p` (p itself nullable) is the _Opt-pointer branch below.
            */
            bool pointee_is_opt = false;
            if (p_pointed != NULL)
            {
                struct type pointed_type = type_is_array(p_type)
                    ? get_array_item_type(p_type)
                    : type_remove_pointer(p_type);
                pointee_is_opt = type_is_nullable(&pointed_type, nullable_enabled);
                /* arrays are made at their first constant index, see EXPR_POSTFIX_ARRAY */
                make_object(&pointed_type, p_pointed, type_is_pointed_out(p_type) ? MAKE_STATE_ANY : MAKE_STATE_ANY_LAZY_ARRAYS, &ctx->ctx->options.platform);
                type_destroy(&pointed_type);
            }

            struct flow_key_alternatives* _Opt ep = flow_branch_find_add(ctx->p_current_flow_branch, p_object);
            if (ep == NULL) throw;
            flow_alternatives_clear(&ep->alternatives);
            /*
                Single EQUAL alternative carrying the concrete arena pointer.
                The parameter is non-null on entry and points exactly to this object.
            */
            if (p_pointed != NULL)
            {

                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_PTR,
                    .value = {.p = p_pointed},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&ep->alternatives, &a);

            }
            else
            {

                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_PTR,
                    .value = {.p = NULL},
                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&ep->alternatives, &a);

            }

            if (p_pointed != NULL)
            {
                /*
                    _Out pointee: members start UNINITIALIZED (the callee initializes it).
                    Otherwise: ANY (unknown but valid).
                */
                if (type_is_pointed_out(p_type))
                {
                    /* Mark every leaf _Owner member as uninitialized. */
                    if (p_pointed->members.head)
                    {
                        for (struct object* _Opt m = p_pointed->members.head; m; m = m->next)
                        {

                            struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, m);
                            if (e == NULL) throw;
                            flow_alternatives_clear(&e->alternatives);
                            struct flow_alternative a =
                            {
                                .value_kind = FLOW_VALUE_KIND_SIGNED,
                                .value = {.i = UNINITIALIZED_VALUE},
                                .value_relation = FLOW_RELATION_UNINITIALIZED,
                                .imaginary = FLOW_IMAGINARY_NONE,
                                .p_origin_map = ctx->p_current_flow_branch,
                                .p_origin_token = p_token
                            };
                            flow_alternatives_add(&e->alternatives, &a);

                        }
                        /* Parent node: NOT_APPLICABLE (aggregate, state lives in members). */
                        /* flow_branch_replace_alternatives_i(ctx->p_current_flow_branch, */
                        /* p_pointed, NOT_APPLICABLE_VALUE, FLOW_NOT_APPLICABLE, line); */
                    }
                    else
                    {

                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, p_pointed);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = UNINITIALIZED_VALUE},
                            .value_relation = FLOW_RELATION_UNINITIALIZED,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);

                    }
                }
                else if (depth < FLOW_PARAMETER_OBJECT_INIT_MAX_DEPTH)
                {
                    /*
                        Recurse with the pointee's real type, so a non-_Opt `T*` pointee of a
                        `T**` parameter is seeded non-null.
                    */
                    flow_parameter_object_init_r(ctx, p_pointed, &p_pointed->type, p_token, depth + 1, force_opt || pointee_is_opt);
                }
                /* Depth cap reached: members stay ANY, no recursion (self-referential types). */
            }

            return;
        }

        /* Seed an uninitialized _Opt pointer as a correlated null/non-null pair sharing origin maps with its pointee's does-not-exist/valid alternatives, so narrowing (e.g. `if (p)`) keeps both sides in sync. */

        if (relation == FLOW_RELATION_ANY &&
                p_type != NULL &&
                type_is_pointer(p_type) &&
                ((nullable_enabled && type_is_nullable(p_type, nullable_enabled)) || force_opt))
        {
            /* Two child maps so alternatives from each arm have distinct origins. */
            struct flow_branch* _Opt p_null_map =
                flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_OPT_NULL);
            struct flow_branch* _Opt p_nonnull_map =
                flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_OPT_NONNULL);

            /*
                Without both arm maps there is no way to keep the null and
                non-null alternatives on distinct origins.
            */
            if (p_null_map == NULL || p_nonnull_map == NULL)
            {
                throw;
            }

            /* Allocate and initialise the arena object for the non-null arm. */
            struct object* _Opt p_pointed = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
            if (p_pointed != NULL)
            {
                struct type pointed_type = type_remove_pointer(p_type);
                make_object(&pointed_type, p_pointed, MAKE_STATE_ANY_LAZY_ARRAYS, &ctx->ctx->options.platform);
                type_destroy(&pointed_type);
            }

            /* --- pointer alternatives --- */
            struct flow_key_alternatives* _Opt ep = flow_branch_find_add(ctx->p_current_flow_branch, p_object);
            if (ep == NULL) throw;
            flow_alternatives_clear(&ep->alternatives);
            /* null arm */
            {
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_PTR,
                    .value = {.p = NULL},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = p_null_map,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&ep->alternatives, &a);
            }
            /* non-null arm */
            if (p_pointed != NULL)
            {
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_PTR,
                    .value = {.p = p_pointed},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = p_nonnull_map,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&ep->alternatives, &a);
            }
            else
            {
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_PTR,
                    .value = {.p = NULL},
                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = p_nonnull_map,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&ep->alternatives, &a);
            }

            /* --- pointed-to object alternatives --- */
            if (p_pointed != NULL)
            {
                /* Write both p_pointed alternatives (normal init from p_nonnull_map, does_not_exist from p_null_map) directly and atomically instead of via flow_parameter_object_init, which either loses the origin stamp (NULL type) or overwrites the entry before it's stamped (real type, recursive expansion). */

                struct flow_key_alternatives* _Opt eo = flow_branch_find_add(ctx->p_current_flow_branch, p_pointed);
                if (eo == NULL) throw;
                flow_alternatives_clear(&eo->alternatives);

                /* Non-null arm: ANY state, correlated with p_nonnull_map. */

                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = ANY_VALUE},
                    .value_relation = FLOW_RELATION_ANY,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = p_nonnull_map,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&eo->alternatives, &a);

                /*
                    p_pointed's members are not seeded here: after narrowing they follow their
                    own declared nullability (pattern2-member-through-narrowed-opt.c), and
                    seeding them on self-referential types breaks arena/origin state
                    (do-while-nested-if-unrelated-narrow.c).
                */
            }
        }
        else
        {
            /*
                A plain non-pointer value (or any value with nullable checking
                disabled) is seeded as unknown (ANY), never UNINITIALIZED: scalar
                parameters, call results and members reached this way hold some
                value.
            */
            struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, p_object);
            if (e == NULL) throw;
            flow_alternatives_clear(&e->alternatives);
            struct flow_alternative a =
            {
                .value_kind = value.value_kind,
                .value = value.value,
                .value_relation = FLOW_RELATION_ANY,
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = ctx->p_current_flow_branch,
                .p_origin_token = p_token
            };
            flow_alternatives_add(&e->alternatives, &a);
        }
    }
    catch
    {
    }
}

static void flow_seed_aggregate_from_init_exprs(struct flow_ctx* ctx, struct object* p_object)
{
    if (p_object->members.head)
    {
        for (struct object* _Opt m = p_object->members.head; m != NULL; m = m->next)
            flow_seed_aggregate_from_init_exprs(ctx, m);
        return;
    }

    /*
        Copy a non-constant initializer's flow state into its leaf member
        (owner members are moved from their source), e.g.
        `.p_token_begin = p->first_token`.
    */
    const bool seedable_member =
        type_is_integer(&p_object->type) ||
        type_is_pointer(&p_object->type);

    if (p_object->p_init_expression != NULL &&
            seedable_member &&
            p_object->state != CONSTANT_VALUE_STATE_CONSTANT &&
            p_object->state != CONSTANT_VALUE_EQUAL)
    {
        flow_check_object_init_assigment(ctx,
                                         p_object->p_init_expression,
                                         p_object,
                                         &p_object->p_init_expression->object,
                                         INIT_OBJ,
                                         false,
                                         false);
    }
}

static void flow_visit_init_declarator(struct flow_ctx* ctx, const struct init_declarator* p_init_declarator)
{
    flow_visit_declarator(ctx, p_init_declarator->p_declarator);

    /* like file scope, a static without initializer is seeded as ANY on first use */
    const bool is_static_without_initializer =
        p_init_declarator->initializer == NULL &&
        !is_automatic_variable(p_init_declarator->p_declarator->object.type.storage_class_specifier_flags);

    if (!type_is_function(&p_init_declarator->p_declarator->object.type) &&
            !is_static_without_initializer)
    {
        const struct declaration_specifiers* _Opt p_specifiers =
            p_init_declarator->p_declarator->declaration_specifiers;

        flow_object_init(ctx,
                         &p_init_declarator->p_declarator->object,
                         p_specifiers != NULL ?
                         p_specifiers->first_token :
                         p_init_declarator->p_declarator->first_token_opt);
    }

    if (p_init_declarator->initializer)
    {
        if (p_init_declarator->initializer->assignment_expression)
        {
            struct expression* p_init_expr = p_init_declarator->initializer->assignment_expression;
            flow_visit_full_expression(ctx, p_init_expr);
            flow_check_object_init_assigment(ctx,
                                             p_init_declarator->initializer->assignment_expression,
                                             &p_init_declarator->p_declarator->object,
                                             &p_init_expr->object,
                                             INIT_OBJ,
                                             false,
                                             false
            );

            flow_apply_alloc_contract_to_dest(ctx,
                                              &p_init_declarator->p_declarator->object.type,
                                              &p_init_declarator->p_declarator->object,
                                              p_init_expr);
            /* flow_check_rhs_value(ctx, &p_init_declarator->p_declarator->type, p_init_expr); */
            /* flow_assign_rhs_to_lhs(ctx, &p_init_declarator->p_declarator->object, p_init_expr); */
        }
        else if (p_init_declarator->initializer->braced_initializer)
        {
            struct object* p_aggregate = &p_init_declarator->p_declarator->object;

            /* 1. Visit the whole braced initializer – evaluates all RHS expressions. */
            flow_visit_bracket_initializer_list(ctx, p_init_declarator->initializer->braced_initializer);

            /*
                2. Seed members set by non-constant initializers from those
                expressions' flow3 values (constant members already seeded).
            */
            flow_seed_aggregate_from_init_exprs(ctx, p_aggregate);

            /* 3. Zero‑initialise any scalar members not explicitly set. */
            if (type_is_struct_or_union(&p_aggregate->type))
            {
                /* flow_zero_initialize_uninitialized_members(ctx, p_aggregate, line); */
            }
        }
    }
}

static void flow_visit_init_declarator_list(struct flow_ctx* ctx, struct init_declarator_list* p_init_declarator_list);

static void flow_visit_declaration_specifiers(struct flow_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers);

static void flow_visit_simple_declaration(struct flow_ctx* ctx, struct simple_declaration* p_simple_declaration)
{
    if (p_simple_declaration->p_declaration_specifiers)
    {
        flow_visit_declaration_specifiers(ctx, p_simple_declaration->p_declaration_specifiers);
    }
    flow_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}


static const struct flow_alternative* _Opt flow_find_truth_witness(struct flow_branch* _Opt map,
                                                                   const struct object* p_object,
                                                                   int depth)
{
    if (depth > 10)
    {
        return NULL;
    }

    const struct flow_key_alternatives* _Opt p_entry = flow_branch_search_up(map, p_object);

    if (p_entry == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < p_entry->alternatives.size; i++)
    {
        const struct flow_alternative* alt = p_entry->alternatives.data[i];

        if (flow_alternative_is_dead(alt))
        {
            continue;
        }

        if (alt->value_kind == FLOW_VALUE_KIND_REF)
        {
            if (alt->value.p == NULL)
            {
                continue;
            }
            return flow_find_truth_witness(map, alt->value.p, depth + 1);
        }

        return alt;
    }

    return NULL;
}

/* the value one alternative holds, in words ("null", "not 0", "greater than 3"); no internal state */
static void flow_alternative_describe_value(struct osstream* ss, const struct flow_alternative* alt, bool is_pointer)
{
    if (alt->value_kind == FLOW_VALUE_KIND_PTR || alt->value_kind == FLOW_VALUE_KIND_REF)
    {
        /* a pointer that holds an object's address */
        const bool is_null = alt->value.p == NULL;
        const bool equal = alt->value_relation == FLOW_RELATION_EQUAL;
        ss_fprintf(ss, "%s", (equal == is_null) ? "null" : "not null");
        return;
    }

    char number[32] = { 0 };
    if (alt->value_kind == FLOW_VALUE_KIND_UNSIGNED)
        snprintf(number, sizeof number, "%llu", alt->value.u);
    else
        snprintf(number, sizeof number, "%lld", alt->value.i);

    const bool is_zero = strcmp(number, "0") == 0;
    const char* value = is_pointer && is_zero ? "null" : number;

    switch (alt->value_relation)
    {
        case FLOW_RELATION_EQUAL:         ss_fprintf(ss, "%s", value); break;
        case FLOW_RELATION_NOT_EQUAL:     ss_fprintf(ss, "not %s", value); break;
        case FLOW_RELATION_GREATER:       ss_fprintf(ss, "greater than %s", value); break;
        case FLOW_RELATION_GREATER_EQUAL: ss_fprintf(ss, "greater than or equal to %s", value); break;
        case FLOW_RELATION_LESS:          ss_fprintf(ss, "less than %s", value); break;
        case FLOW_RELATION_LESS_EQUAL:    ss_fprintf(ss, "less than or equal to %s", value); break;
        case FLOW_RELATION_ANY:           ss_fprintf(ss, "unknown"); break;
        case FLOW_RELATION_UNINITIALIZED: ss_fprintf(ss, "uninitialized"); break;
    }
}

static void flow_check_condition_known_at_compile_time(struct flow_ctx* ctx,
                                                       const struct expression* p_cond)
{
    if (ctx->iteration_pass != 0)
    {
        /*
            Inside a loop body the state is one iteration's, and the next
            iteration can contradict it: a counter the body increments, a
            pointer the body advances. Saying "always" there is exactly the
            claim this pass is not entitled to make.
        */
        return;
    }

    /*
        Same question compile_assert's check_object_true asks, with a different
        answer shape: that one reports every alternative it cannot prove true,
        one diagnostic each, and explains why; this one stays silent unless the
        whole set agrees.
    */
    const int known = flow_object_truth(ctx->p_current_flow_branch, &p_cond->object, 0);
    if (known == -1)
    {
        return;
    }

    const struct marker marker =
    {
        .p_token_begin = p_cond->first_token,
        .p_token_end = p_cond->last_token
    };

    /*
        For a comparison, the fact that settles it lives in the operands, not in
        the folded result: the result's own state was recorded at the condition
        itself, so reporting it would just point back at the line the reader is
        already looking at. Explain the operand instead.
    */
    const struct expression* _Opt p_explained = p_cond;

    if (p_cond->expression_type == EXPR_EQUALITY_EQUAL ||
            p_cond->expression_type == EXPR_EQUALITY_NOT_EQUAL ||
            p_cond->expression_type == EXPR_RELATIONAL_BIGGER_THAN ||
            p_cond->expression_type == EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN ||
            p_cond->expression_type == EXPR_RELATIONAL_LESS_THAN ||
            p_cond->expression_type == EXPR_RELATIONAL_LESS_OR_EQUAL_THAN)
    {
        const struct expression* _Opt operands[2] = { p_cond->left, p_cond->right };

        for (int i = 0; i < 2; i++)
        {
            if (operands[i] == NULL)
            {
                continue;
            }

            const struct flow_alternative* _Opt p_operand_alt =
                flow_find_truth_witness(ctx->p_current_flow_branch, &operands[i]->object, 0);

            /*
                A literal operand has no tracked state; the other side carries
                the explanation.
            */
            if (p_operand_alt != NULL &&
                    p_operand_alt->value_relation != FLOW_RELATION_ANY &&
                    p_operand_alt->value_relation != FLOW_RELATION_UNINITIALIZED)
            {
                p_explained = operands[i];
                break;
            }
        }
    }

    const struct flow_alternative* _Opt p_alternative =
        flow_find_truth_witness(ctx->p_current_flow_branch, &p_explained->object, 0);

    if (p_alternative == NULL)
    {
        /*
            flow_object_truth found a settled value, so an alternative normally
            exists; if it does not, still report the conclusion.
        */
        diagnostic(W_FLOW_CONDITION_KNOWN_AT_COMPILE_TIME, ctx->ctx, NULL, &marker,
                   "condition is always %s", known ? "true" : "false");
        return;
    }

    if (p_explained == p_cond)
    {
        /* no operand explains it: "because '<the condition>' is 0" would only repeat the condition */
        if (diagnostic(W_FLOW_CONDITION_KNOWN_AT_COMPILE_TIME, ctx->ctx, NULL, &marker,
                       "condition is always %s", known ? "true" : "false"))
        {
            flow_explain_alternative(ctx, p_alternative, p_alternative->p_origin_map, &marker);
        }
        return;
    }

    struct osstream name_ss = { 0 };
    flow_expression_to_string(p_explained, &name_ss);

    struct osstream value_ss = { 0 };
    flow_alternative_describe_value(&value_ss, p_alternative, type_is_pointer(&p_explained->object.type));

    /*
        Name the fact, but leave "where it came from" and "which branches were
        assumed" to the notes below -- same shape as every other diagnostic that
        reports a fact carried by one alternative.
    */
    const bool reported =
        diagnostic(W_FLOW_CONDITION_KNOWN_AT_COMPILE_TIME, ctx->ctx, NULL, &marker,
                   "condition is always %s because '%s' is %s",
                   known ? "true" : "false",
                   name_ss.c_str ? name_ss.c_str : "",
                   value_ss.c_str ? value_ss.c_str : "");

    ss_close(&name_ss);
    ss_close(&value_ss);

    if (reported)
    {
        flow_explain_alternative(ctx, p_alternative, p_alternative->p_origin_map, &marker);
    }
}

static void flow_check_limits(struct flow_ctx* ctx, const struct token* p_token);

static void flow_visit_if_statement(struct flow_ctx* ctx, struct selection_statement* p_selection_statement)
{
    try
    {
        if (p_selection_statement->last_token == NULL)
            throw;

        if (p_selection_statement->p_init_statement &&
                p_selection_statement->p_init_statement->p_expression_statement)
        {
            flow_visit_expression_statement(ctx, p_selection_statement->p_init_statement->p_expression_statement);
        }

        if (p_selection_statement->p_init_statement &&
                p_selection_statement->p_init_statement->p_simple_declaration)
        {
            flow_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);
        }


        _Opt _View struct expression hidden_expression =
        {
            .first_token = p_selection_statement->first_token,
            .last_token = p_selection_statement->last_token
        };

        struct flow_true_false_branches cond_pair = { 0 };

        if (p_selection_statement->condition &&
                p_selection_statement->condition->expression)
        {
            cond_pair = flow_visit_condition(ctx, p_selection_statement->condition->expression);
        }

        if (p_selection_statement->condition &&
                p_selection_statement->condition->p_init_declarator)
        {
            flow_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
        }

        if (p_selection_statement->condition &&
                p_selection_statement->condition->expression == NULL &&
                p_selection_statement->condition->p_init_declarator != NULL)
        {
            hidden_expression.expression_type = EXPR_PRIMARY_DECLARATOR;
            hidden_expression.declarator = p_selection_statement->condition->p_init_declarator->p_declarator;
            _Assert(p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt != NULL);
            hidden_expression.first_token = p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt;
            hidden_expression.last_token = hidden_expression.first_token;
            cond_pair = flow_visit_full_expression(ctx, &hidden_expression);
        }

        _Assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);

        if (ctx->p_current_flow_branch == NULL)
            return;

        struct flow_branch* p_before = ctx->p_current_flow_branch;

        /*
            Non-narrowing conditions return the identity pair (or none at all);
            force two distinct arm maps so branch-body writes cannot leak
            unconditionally into the state after the if.
        */
        const struct expression* _Opt p_if_cond = NULL;
        if (p_selection_statement->condition != NULL)
        {
            if (p_selection_statement->condition->expression != NULL)
            {
                p_if_cond = p_selection_statement->condition->expression;
            }
            else if (p_selection_statement->condition->p_init_declarator != NULL &&
                     hidden_expression.first_token != NULL &&
                     hidden_expression.last_token != NULL)
            {
                /*
                    The tokens are only set by the branch above that actually built
                    hidden_expression; without them there is no condition to narrow.
                */
                p_if_cond = &hidden_expression;
            }
        }
        if (p_if_cond != NULL)
        {
            flow_check_condition_known_at_compile_time(ctx, p_if_cond);
        }

        cond_pair = flow_ensure_branch_pair(ctx, p_before, cond_pair, p_if_cond);

        /*
            Values the condition kept can say which side of an earlier branch each
            arm is on (same condition tested twice, `if (p)` after `p = c ? &i : 0`),
            so values from the other side stay out of the arm.
        */
        if (cond_pair.p_true != NULL && cond_pair.p_false != NULL &&
                cond_pair.p_true != cond_pair.p_false)
        {
            flow_branch_derive_implied_facts(cond_pair.p_true, p_before);
            flow_branch_derive_implied_facts(cond_pair.p_false, p_before);
        }

        ctx->p_current_flow_branch = cond_pair.p_true;
        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);

        const bool true_reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

        if (p_selection_statement->else_secondary_block_opt)
        {
            ctx->p_current_flow_branch = cond_pair.p_false;
            flow_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);

            /* the merges of a long else-if chain run back to back, with no block item between them */
            flow_check_limits(ctx, p_selection_statement->first_token);
            if (ctx->stopped)
            {
                ctx->p_current_flow_branch = p_before;
                return;
            }

            const bool false_reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->else_secondary_block_opt);

            /*
                A null branch map means the arena had no map to give; read it as an
                arm with no outcome to merge and leave p_before as-is.
            */
            if (cond_pair.p_true != NULL && cond_pair.p_false != NULL)
            {
                if (true_reached_the_end && false_reached_the_end)
                {
                    flow_branch_merge_a_b(p_before, cond_pair.p_true, cond_pair.p_false);
                }
                else if (true_reached_the_end)
                {
                    flow_branch_merge_a_b(p_before, cond_pair.p_true, cond_pair.p_true);
                }
                else if (false_reached_the_end)
                {
                    flow_branch_merge_a_b(p_before, cond_pair.p_false, cond_pair.p_false);
                }
                else
                {
                    /* both jump: nothing after the if is reached from here */
                    p_before->is_unreachable = true;
                }
            }
        }
        else
        {
            /* no else: false path is already narrowed in cond_pair.p_false */
            if (cond_pair.p_true != NULL && cond_pair.p_false != NULL)
            {
                if (true_reached_the_end)
                {
                    flow_branch_merge_a_b(p_before, cond_pair.p_true, cond_pair.p_false);
                }
                else
                {
                    flow_branch_merge_a_b(p_before, cond_pair.p_false, cond_pair.p_false);
                }
            }
        }

        ctx->p_current_flow_branch = p_before;
    }
    catch
    {

    }
}

static void flow_visit_try_statement(struct flow_ctx* ctx, struct try_statement* p_try_statement)
{
    struct flow_branch* _Opt p_throw_join_map_old = ctx->p_throw_join_map;
    const bool throw_join_reached_old = ctx->throw_join_reached;

    if (ctx->p_current_flow_branch == NULL)
        return;

    struct flow_branch* p_before = ctx->p_current_flow_branch;

    /* throw paths are accumulated into this map */
    struct flow_branch* _Opt p_throw_join = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_THROW_JOIN);
    if (p_throw_join == NULL)
        return; /* no map to work with */

    ctx->p_throw_join_map = p_throw_join;
    ctx->throw_join_reached = false;

    /* --- visit the try body --- */
    struct flow_branch* _Opt p_try_branch = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_TRY_BRANCH);
    /*
        Finishing the try and throwing never happen on the same path:
        `try { p = f(); } catch { delete(p); p = 0; } if (p) p->x;` reads x from
        the try side only. Each throw copies its values under the throw join
        (the catch too), so no value there has a try origin.
    */
    flow_tag_branch_pair(p_try_branch, p_throw_join);
    if (p_try_branch == NULL)
        return; /* no map to work with */

    ctx->p_current_flow_branch = p_try_branch;
    flow_visit_secondary_block(ctx, p_try_statement->secondary_block);

    /*
        Restore the enclosing throw-join before the catch body: a throw inside
        catch propagates to the enclosing try, not to this one.
    */
    ctx->p_throw_join_map = p_throw_join_map_old;
    const bool throw_join_reached = ctx->throw_join_reached;
    ctx->throw_join_reached = throw_join_reached_old;

    const bool try_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->secondary_block);
    const bool catch_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt);

    if (p_try_statement->catch_secondary_block_opt)
    {
        /* --- visit the catch body, starting from the throw-join map --- */
        struct flow_branch* _Opt p_catch_branch = flow_branch_arena_new(&ctx->flow_branch_arena, p_throw_join, FLOW_BRANCH_CATCH_BRANCH);
        ctx->p_current_flow_branch = p_catch_branch;

        flow_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);

        /* No catch map (arena empty): treat as an empty catch arm. */
        if (try_reached_the_end && catch_reached_the_end && p_catch_branch != NULL)
        {
            flow_branch_merge_a_b(p_before, p_try_branch, p_catch_branch);
        }
        else if (try_reached_the_end)
        {
            /* catch jumps away (or has no map); only the try-branch outcome matters */
            flow_branch_merge_a_b(p_before, p_try_branch, p_try_branch);
        }
        else if (catch_reached_the_end && p_catch_branch != NULL)
        {
            /* try jumps away; only the catch-branch outcome matters */
            flow_branch_merge_a_b(p_before, p_catch_branch, p_catch_branch);
        }
        /* else: both jump — p_before stays as-is (unreachable continuation) */
    }
    else
    {
        /*
            No catch block: the state at the throw point is a possible outcome
            after the try, as is the end of the try body; both are merged.
        */
        /*
            Pre-filled with p_before so no element is ever indeterminate; only
            the first num_arms entries are read.
        */
        const struct flow_branch* arms[2] = { p_before, p_before };
        int num_arms = 0;
        if (try_reached_the_end)
        {
            arms[num_arms++] = p_try_branch;
        }
        if (throw_join_reached)
        {
            arms[num_arms++] = p_throw_join;
        }
        if (num_arms > 0)
        {
            flow_branch_merge_arms(p_before, arms, num_arms);
        }
    }

    ctx->p_current_flow_branch = p_before;
    /*
        ctx->p_throw_join_map was already restored before the catch body
        was visited, above.
    */
}

static void flow_visit_switch_statement(struct flow_ctx* ctx, struct selection_statement* p_selection_statement)
{
    /* Saved outside the try so the catch below restores them on every exit. */
    struct flow_branch* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow_branch* _Opt old_p_break_join_map = ctx->p_break_join_map;
    const bool old_break_join_reached = ctx->break_join_reached;
    const struct object* _Opt old_p_switch_obj_key = ctx->p_switch_obj_key;

    try
    {
        if (ctx->p_current_flow_branch == NULL)
        {
            throw;
        }

        struct flow_branch* p_before = ctx->p_current_flow_branch;
        struct flow_branch* _Opt p_body = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_SWITCH_BODY);
        struct flow_branch* _Opt p_break_join = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_BREAK_JOIN);
        if (p_body == NULL || p_break_join == NULL)
        {
            throw; /* no map to work with */
        }

        if (p_selection_statement->p_init_statement &&
                p_selection_statement->p_init_statement->p_expression_statement)
        {
            flow_visit_expression_statement(ctx, p_selection_statement->p_init_statement->p_expression_statement);
        }

        if (p_selection_statement->p_init_statement &&
                p_selection_statement->p_init_statement->p_simple_declaration)
        {
            flow_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);
        }

        if (p_selection_statement->condition &&
                p_selection_statement->condition->expression)
        {
            flow_visit_condition(ctx, p_selection_statement->condition->expression);
        }

        if (p_selection_statement->condition &&
                p_selection_statement->condition->p_init_declarator)
        {
            flow_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
        }

        ctx->p_initial_map = p_before;
        ctx->p_break_join_map = p_break_join;
        ctx->break_join_reached = false;

        /* the one object the condition refers to (`x`, `s->kind`); several targets: no narrowing */
        ctx->p_switch_obj_key = NULL;
        if (p_selection_statement->condition &&
                p_selection_statement->condition->expression)
        {
            const struct flow_key_alternatives* _Opt p_condition_entry =
                flow_branch_search_up(ctx->p_current_flow_branch, &p_selection_statement->condition->expression->object);
            if (p_condition_entry != NULL &&
                p_condition_entry->alternatives.size == 1 &&
                p_condition_entry->alternatives.data[0]->value_kind == FLOW_VALUE_KIND_REF &&
                p_condition_entry->alternatives.data[0]->value.p != NULL)
            {
                ctx->p_switch_obj_key = p_condition_entry->alternatives.data[0]->value.p;
            }
        }

        ctx->p_current_flow_branch = p_body;
        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);

        bool reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

        {
            /*
                Pre-filled with p_before so no element is ever indeterminate;
                only the first num_arms entries are read.
            */
            const struct flow_branch* arms[3] = { p_before, p_before, p_before };
            int num_arms = 0;
            if (ctx->p_current_flow_branch == NULL)
            {
                throw;
            }

            /* without default, no case matching skips the body: p_before is an outcome */
            if (case_label_list_find_default(&p_selection_statement->label_list) == NULL)
            {
                arms[num_arms++] = p_before;
            }

            if (reached_the_end)
            {
                arms[num_arms++] = ctx->p_current_flow_branch;
            }
            if (ctx->break_join_reached)
            {
                arms[num_arms++] = p_break_join;
            }
            if (num_arms > 0)
            {
                flow_branch_merge_arms(p_before, arms, num_arms);
            }
        }

        ctx->p_current_flow_branch = p_before;

        if (p_selection_statement->secondary_block != NULL)
        {
            flow_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list,
                                             p_selection_statement->secondary_block->last_token);

            flow_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list,
                                                p_selection_statement->secondary_block->last_token);
        }
    }
    catch
    {
    }

    /* restore */
    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
    ctx->break_join_reached = old_break_join_reached;
    ctx->p_switch_obj_key = old_p_switch_obj_key;
}

static void flow_visit_selection_statement(struct flow_ctx* ctx, struct selection_statement* p_selection_statement)
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
    {
        _Assert(false);
    }

    if (p_selection_statement->lint_token)
    {
        flow_check_dianostic_suppression(ctx, p_selection_statement->lint_token);
    }

}

static void flow_visit_compound_statement(struct flow_ctx* ctx, struct compound_statement* p_compound_statement);
static void flow_visit_compound_statement_core(struct flow_ctx* ctx, struct compound_statement* p_compound_statement);

static void flow_visit_initializer_list(struct flow_ctx* ctx, struct initializer_list* p_initializer_list);

static void flow_visit_bracket_initializer_list(struct flow_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {
    }
    else
    {
        flow_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void flow_visit_initializer_list_item(struct flow_ctx* ctx, struct initializer_list_item* p_initializer)
{
    _Assert(p_initializer->initializer != NULL);
    flow_visit_initializer(ctx, p_initializer->initializer);
}

static void flow_visit_initializer(struct flow_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->assignment_expression)
    {
        flow_visit_full_expression(ctx, p_initializer->assignment_expression);
    }
    else if (p_initializer->braced_initializer)
    {
        flow_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void flow_visit_initializer_list(struct flow_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer_list_item* _Opt p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow_visit_initializer_list_item(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow_visit_generic_selection(struct flow_ctx* ctx, const struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        flow_visit_full_expression(ctx, p_generic_selection->expression);
    }
}

const char* flow_get_only_member_name(const char* member_designator)
{
    const char* s = member_designator;
    const char* dot = s;
    while (*s)
    {
        if (*s == '.')
            dot = s;
        s++;
    }
    return dot;
}
static bool flow_branch_is_ancestor_or_self(const struct flow_branch* _Opt ancestor, const struct flow_branch* _Opt m)
{
    if (ancestor == NULL)
        return true; /* no filter: everything is visible */

    for (const struct flow_branch* _Opt cur = m; cur; cur = cur->p_parent_map)
    {
        if (cur == ancestor)
            return true;
    }
    return false;
}
static bool flow_object_has_initialized_state(struct flow_ctx* ctx, const struct object* obj)
{
    const struct flow_key_alternatives* _Opt e = flow_branch_search_up(ctx->p_current_flow_branch, obj);
    for (int i = 0; e != NULL && i < e->alternatives.size; i++)
    {
        if (e->alternatives.data[i]->value_relation != FLOW_RELATION_UNINITIALIZED)
            return true;
    }
    /* A member that is itself an aggregate has no direct entry; recurse. */
    for (const struct object* _Opt m = obj->members.head; m != NULL; m = m->next)
    {
        if (flow_object_has_initialized_state(ctx, m))
            return true;
    }
    return false;
}
static bool flow_union_is_initialized(struct flow_ctx* ctx, const struct object* p_union)
{
    if (!type_is_union(&p_union->type))
        return false;
    for (const struct object* _Opt m = p_union->members.head; m != NULL; m = m->next)
    {
        if (flow_object_has_initialized_state(ctx, m))
            return true;
    }
    return false;
}

enum flow_leaf_state
{
    FLOW_LEAF_UNINITIALIZED,
    FLOW_LEAF_MOVED,
    FLOW_LEAF_ENDED,
};

/*
    Collapse a per-leaf diagnostic into one aggregate report: 'all' for
    UNINITIALIZED/MOVED (can be partial), 'any' for ENDED (ends the whole
    object). include_current_branch also accepts alternatives whose origin
    is an ancestor of the current branch.
*/
static bool flow_object_leaves_in_state(struct flow_ctx* ctx,
                                        const struct object* p_obj,
                                        enum flow_leaf_state state,
                                        const struct flow_branch* _Opt p_origin_filter,
                                        bool include_current_branch,
                                        bool require_all,
                                        int* p_line,
                                        const struct flow_branch* _Opt* _Opt pp_origin)
{
    /* union members share storage: any member initialized means the union is */
    if (state == FLOW_LEAF_UNINITIALIZED && type_is_union(&p_obj->type) && flow_union_is_initialized(ctx, p_obj))
        return false;

    if (p_obj->members.head)
    {
        for (const struct object* _Opt m = p_obj->members.head; m; m = m->next)
        {
            const bool hit = flow_object_leaves_in_state(ctx, m, state, p_origin_filter, include_current_branch, require_all, p_line, pp_origin);
            if (require_all)
            {
                if (!hit) return false;
            }
            else if (hit)
                return true;
        }
        return require_all;
    }

    const struct flow_key_alternatives* _Opt e =
        flow_branch_search_up(ctx->p_current_flow_branch, p_obj);

    if (e == NULL)
        return false;

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow_alternative* a = e->alternatives.data[i];

        /*
            Same branch scoping as the per-alternative loop: a fact from one arm
            must not reach another (self-consuming-reassignment.c).
        */
        if (!flow_branch_is_ancestor_or_self(a->p_origin_map, p_origin_filter) &&
                !(include_current_branch && ctx->p_current_flow_branch != NULL &&
                  flow_branch_is_ancestor_or_self(a->p_origin_map, ctx->p_current_flow_branch)))
            continue;

        const bool match =
            (state == FLOW_LEAF_UNINITIALIZED)
            ? (a->imaginary != FLOW_IMAGINARY_ENDED &&
               a->value_relation == FLOW_RELATION_UNINITIALIZED)
            : (state == FLOW_LEAF_MOVED)
            ? (a->imaginary == FLOW_IMAGINARY_MOVED)
            : (a->imaginary == FLOW_IMAGINARY_ENDED);

        if (match)
        {
            if (*p_line == 0)
            {
                *p_line = flow_alternative_line(a);
                if (pp_origin != NULL)
                    *pp_origin = a->p_origin_map;
            }
            return true;
        }
    }
    return false;
}

/*
    The entry for (object, line, diagnostic), added when missing; NULL when the
    table is full.
*/
static struct flow_reported_finding* _Opt flow_finding_find_add(struct flow_ctx* ctx,
                                                                const struct object* _Opt p_object,
                                                                int line,
                                                                int diagnostic_id,
                                                                bool* is_new)
{
    const int max = FLOW_MAX_REPORTED_FINDINGS;

    *is_new = false;

    if (ctx->p_reported_findings == NULL)
    {
        ctx->p_reported_findings = calloc(max, sizeof(struct flow_reported_finding));
        if (ctx->p_reported_findings == NULL)
            return NULL;
    }
    struct flow_reported_finding* const findings = ctx->p_reported_findings;

    int i = 0;
    while (i < max && findings[i].p_object != NULL)
    {
        if (findings[i].p_object == p_object &&
                findings[i].line == line &&
                findings[i].diagnostic_id == diagnostic_id)
        {
            return &findings[i];
        }
        i++;
    }

    *is_new = true;

    /*
        Stop one short of the end so slot max-1 stays NULL and keeps
        terminating the scan above.
    */
    if (i >= max - 1)
        return NULL;

    struct flow_reported_finding* p = &findings[i];
    p->p_object = p_object;
    p->line = line;
    p->diagnostic_id = diagnostic_id;
    p->message_offset = -1;
    p->count = 0;
    p->sample_count = 0;
    findings[i + 1].p_object = NULL;
    return p;
}

static bool flow_finding_already_reported(struct flow_ctx* ctx, const struct object* _Opt p_object, int line, int diagnostic_id)
{
    bool is_new = false;
    flow_finding_find_add(ctx, p_object, line, diagnostic_id, &is_new);
    return !is_new;
}

/*
    Accumulate one alternative supporting a finding about p_object at
    p_marker, keyed by line. The first stores the message; all are counted;
    the first FLOW_FINDING_MAX_SAMPLES are kept. Flushed by
    flow_findings_flush.
*/
static void flow_finding_record(struct flow_ctx* ctx,
                                const struct object* p_object,
                                enum diagnostic_id diagnostic_id,
                                const struct marker* p_marker,
                                const struct flow_alternative* p_alternative,
                                const struct flow_branch* _Opt p_path_map,
                                const char* message)
{
    /*
        Off where it was found (e.g. a loop's first pass): what diagnostic()
        would have done there -- report nothing and record nothing, so the
        pass that has it on still reports it.
    */
    if (!is_diagnostic_enabled(&ctx->ctx->options, diagnostic_id))
        return;

    const int line = p_marker->p_token_begin ? p_marker->p_token_begin->line : 0;
    bool is_new = false;
    struct flow_reported_finding* _Opt p = flow_finding_find_add(ctx, p_object, line, diagnostic_id, &is_new);
    if (p == NULL)
    {
        /* no room to accumulate: report it now */
        if (diagnostic(diagnostic_id, ctx->ctx, NULL, p_marker, "%s", message))
            flow_explain_alternative(ctx, p_alternative, p_path_map, p_marker);
        return;
    }

    if (is_new)
    {
        p->message_offset = ctx->findings_text.size;
        ss_fprintf(&ctx->findings_text, "%s", message);
        ss_putc('\0', &ctx->findings_text);
        p->marker = *p_marker;
    }
    else if (p->message_offset < 0)
    {
        return; /* already reported */
    }

    p->count++;
    if (p->sample_count < FLOW_FINDING_MAX_SAMPLES)
    {
        struct flow_finding_sample* sample = &p->samples[p->sample_count++];
        sample->p_origin_token = p_alternative->p_origin_token;
        sample->p_origin_map = p_alternative->p_origin_map;
        sample->p_path_map = p_path_map;
    }
}

/*
    Report every accumulated finding once: "(+N branches)" when more than one
    alternative supports it, explained from its samples only. The entries stay,
    so the same fact is not reported again until the table is cleared.
*/
static void flow_findings_flush(struct flow_ctx* ctx)
{
    if (ctx->findings_depth > 0 || ctx->p_reported_findings == NULL)
        return; /* the outermost assignment flushes */

    for (int i = 0; i < FLOW_MAX_REPORTED_FINDINGS && ctx->p_reported_findings[i].p_object != NULL; i++)
    {
        struct flow_reported_finding* p = &ctx->p_reported_findings[i];
        if (p->message_offset < 0 || ctx->findings_text.c_str == NULL)
            continue;
        const char* message = ctx->findings_text.c_str + p->message_offset;

        const enum diagnostic_id id = (enum diagnostic_id)p->diagnostic_id;
        const bool reported = p->count > 1 ?
            diagnostic(id, ctx->ctx, NULL, &p->marker, "%s (also on %d other %s)", message, p->count - 1, p->count == 2 ? "path" : "paths") :
            diagnostic(id, ctx->ctx, NULL, &p->marker, "%s", message);

        for (int k = 0; reported && k < p->sample_count; k++)
        {
            const struct flow_finding_sample* sample = &p->samples[k];

            /* samples sharing an origin or a path would repeat its notes; numbered paths each keep their label */
            if (k == 0 || p->sample_count > 1 || sample->p_origin_token != p->samples[k - 1].p_origin_token)
            {
                const struct flow_alternative origin =
                {
                    .p_origin_token = sample->p_origin_token,
                    .p_origin_map = sample->p_origin_map
                };
                flow_diagnose_state_origin(ctx, &origin, &p->marker, id, p->sample_count > 1 ? k + 1 : 0);
            }
            if (k == 0 || sample->p_path_map != p->samples[k - 1].p_path_map)
                flow_diagnose_map_path(ctx, sample->p_path_map);
        }

        p->message_offset = -1;
    }
    ss_clear(&ctx->findings_text); /* every pending message was reported */
}

static void flow_reported_findings_clear(struct flow_ctx* ctx)
{
    flow_findings_flush(ctx); /* a pending report is never dropped */
    if (ctx->p_reported_findings != NULL)
        ctx->p_reported_findings[0].p_object = NULL;
}

/*
    A check whose findings are accumulated and reported together. Nested scopes
    (a recursive assignment check) join the outermost one, which starts from an
    empty table and flushes when it ends.
*/
static void flow_findings_begin(struct flow_ctx* ctx)
{
    if (ctx->findings_depth == 0)
        flow_reported_findings_clear(ctx);
    ctx->findings_depth++;
}

/*
    Like flow_findings_begin, but keeps what the table already holds: a check
    that continues the previous one (a call's pointee effects, applied after
    its arguments were checked) must not report their findings again.
*/
static void flow_findings_join(struct flow_ctx* ctx)
{
    ctx->findings_depth++;
}

static void flow_findings_end(struct flow_ctx* ctx)
{
    ctx->findings_depth--;
    flow_findings_flush(ctx);
}

/*
    One member step below the object named by parent_expression_str in
    flow_check_object_access. The recursion links them on the stack and the
    name is only built when a diagnostic needs it.
*/
struct flow_access_name_link
{
    const struct flow_access_name_link* _Opt p_previous;
    const char* member_designator;
};

/* "*p" / "p" for the root, "p->a.b" / "s.a.b" below it */
/* base + member as the user would write it: '&x'+'m' -> 'x.m', '*p'+'m' -> 'p->m', 'x'+'m' -> 'x.m' */
static void flow_join_member_name(struct osstream* ss, const char* base, const char* member)
{
    if (base[0] == '&')
        ss_fprintf(ss, "%s.%s", base + 1, member);
    else if (base[0] == '*')
        ss_fprintf(ss, "%s->%s", base + 1, member);
    else
        ss_fprintf(ss, "%s.%s", base, member);
}

static void flow_access_name_to_string(const char* root_name,
                                       bool root_is_ptr,
                                       const struct flow_access_name_link* _Opt p_link,
                                       struct osstream* ss)
{
    if (root_is_ptr && root_name[0] == '&')
    {
        /* through '&d' is 'd' itself: '&d->m' reads as 'd.m' and '*&d' as 'd' */
        root_name++;
        root_is_ptr = false;
    }

    if (p_link == NULL)
    {
        ss_fprintf(ss, root_is_ptr ? "*%s" : "%s", root_name);
    }
    else
    {
        if (p_link->p_previous == NULL)
        {
            ss_fprintf(ss, "%s", root_name);
        }
        else
        {
            flow_access_name_to_string(root_name, root_is_ptr, p_link->p_previous, ss);
        }

        const char* only_member_name = flow_get_only_member_name(p_link->member_designator);
        if (p_link->p_previous == NULL && root_is_ptr)
        {
            ss_fprintf(ss, "->%s", only_member_name[0] == '.' ? only_member_name + 1 : only_member_name);
        }
        else
        {
            ss_fprintf(ss, "%s", only_member_name);
        }
    }
}

static void flow_check_object_access(struct flow_ctx* ctx,
                                     const char* parent_expression_str,
                                     const struct flow_access_name_link* _Opt p_name_link,
                                     struct expression* p_expression,
                                     const struct object* p_object_src,
                                     bool check_unitialized,
                                     const struct flow_branch* _Opt p_origin_filter,
                                     bool dest_is_dtor,
                                     const struct type* _Opt p_dest_governing_type,
                                     bool check_moved,
                                     bool in_array_element,
                                     bool check_ended,
                                     bool base_is_ptr)
{
    /* Built on first use: most visits report nothing. */
    struct osstream bare_name_ss = { 0 };
    bool bare_name_built = false;

    try
    {
        struct marker marker = expression_to_marker(p_expression);

        /*
            Display name for this object itself: "*p" for the root pointee of an
            as-yet-undereferenced pointer expression, "p->text" or "s.a" below it.
        */
        const char* bare_name = parent_expression_str;

        if (p_object_src->members.head)
        {
            /* Union members share storage: if any member is initialized, the union is. */
            bool members_check_uninit = check_unitialized;
            if (members_check_uninit && flow_union_is_initialized(ctx, p_object_src))
                members_check_uninit = false;

                /*
                    Whole aggregate uninitialized, or wholly moved: report it once, by
                    the aggregate's own name, instead of once per leaf. See
                    flow_object_leaves_in_state.
                */
            bool members_check_moved = check_moved;

            if (members_check_uninit)
            {
                int uninit_line = 0;
                const struct flow_branch* _Opt uninit_origin = NULL;
                if (flow_object_leaves_in_state(ctx, p_object_src, FLOW_LEAF_UNINITIALIZED, p_origin_filter, false, true, &uninit_line, &uninit_origin) &&
                        uninit_line != 0)
                {
                    /*
                        At the aggregate level parent_expression_str is empty (the
                        per-leaf reports built their names by appending the member
                        designator to it), so recover the argument's own text.
                    */
                    struct osstream ss = { 0 };
                    if (p_name_link != NULL || parent_expression_str[0] != '\0')
                        flow_access_name_to_string(parent_expression_str, base_is_ptr, p_name_link, &ss);
                    else
                        flow_expression_to_string(p_expression, &ss);

                    const bool reported_uninit = diagnostic(W_FLOW_UNINITIALIZED,
                                                            ctx->ctx, NULL, &marker,
                                                            "passing a possibly uninitialized object '%s' (see line %d)",
                                                            ss.c_str ? ss.c_str : "", uninit_line);
                    ss_close(&ss);
                    if (reported_uninit)
                        flow_diagnose_map_path(ctx, uninit_origin);
                    members_check_uninit = false;
                }
            }

            /*
                Whole aggregate's lifetime ended: one report for the object rather
                than one per member. Deleting a struct ends every member at once, so
                the per-member form said the same thing 66 times for one
                `expression_delete(...)` (expressions.c:5667).
            */
            bool members_check_ended = check_ended;
            if (members_check_ended)
            {
                int ended_line = 0;
                const struct flow_branch* _Opt ended_origin = NULL;
                if (flow_object_leaves_in_state(ctx, p_object_src, FLOW_LEAF_ENDED, p_origin_filter, true, false, &ended_line, &ended_origin) &&
                        ended_line != 0)
                {
                    struct osstream ss = { 0 };
                    if (p_name_link != NULL || parent_expression_str[0] != '\0')
                        flow_access_name_to_string(parent_expression_str, base_is_ptr, p_name_link, &ss);
                    else
                        flow_expression_to_string(p_expression, &ss);

                    const bool reported_ended = diagnostic(W_FLOW_LIFETIME_ENDED,
                                                           ctx->ctx, NULL, &marker,
                                                           "lifetime of '%s' has ended (see line %d)",
                                                           ss.c_str ? ss.c_str : "", ended_line);
                    ss_close(&ss);
                    if (reported_ended)
                    {
                        flow_diagnose_map_path(ctx, ended_origin);
                        /* so the exiting-object check does not report it again */
                        flow_finding_already_reported(ctx, p_object_src, marker.p_token_begin ? marker.p_token_begin->line : 0, W_FLOW_LIFETIME_ENDED);
                    }
                    members_check_ended = false;
                }
            }

            if (members_check_moved)
            {
                int moved_line = 0;
                const struct flow_branch* _Opt moved_origin = NULL;
                if (flow_object_leaves_in_state(ctx, p_object_src, FLOW_LEAF_MOVED, p_origin_filter, false, true, &moved_line, &moved_origin) &&
                        moved_line != 0)
                {
                    struct osstream ss = { 0 };
                    if (p_name_link != NULL || parent_expression_str[0] != '\0')
                        flow_access_name_to_string(parent_expression_str, base_is_ptr, p_name_link, &ss);
                    else
                        flow_expression_to_string(p_expression, &ss);

                    const bool reported_moved = diagnostic(W_FLOW_MOVED,
                                                           ctx->ctx, NULL, &marker,
                                                           "'%s' was moved (see line %d)",
                                                           ss.c_str ? ss.c_str : "", moved_line);
                    ss_close(&ss);
                    if (reported_moved)
                        flow_diagnose_map_path(ctx, moved_origin);
                    members_check_moved = false;
                }
            }

            /*
                An array parameter's elements are governed by its element type
                (`const char* _Opt argv[]` accepts null elements).
            */
            struct type dest_elem_type = { 0 };
            const struct type* _Opt p_member_governing = NULL;
            if (p_dest_governing_type != NULL && type_is_array(p_dest_governing_type))
            {
                dest_elem_type = get_array_item_type(p_dest_governing_type);
                p_member_governing = &dest_elem_type;
            }

            /*
                Once inside an array, stay inside it: the elements are usually
                structs, and it is their leaves that would report.
            */
            const bool members_in_array_element =
                in_array_element || type_is_array(&p_object_src->type);

            struct object* _Opt member = p_object_src->members.head;
            while (member)
            {
                if (member->member_designator == NULL)
                {
                    /* no name to spell the access with */
                    member = member->next;
                    continue;
                }

                /*
                    The member's name ("p->text" when base_is_ptr, "s.a" otherwise)
                    is spelled by flow_access_name_to_string from these links.
                */
                const struct flow_access_name_link member_link = { .p_previous = p_name_link, .member_designator = member->member_designator };
                flow_check_object_access(ctx, parent_expression_str, &member_link, p_expression, member, members_check_uninit, p_origin_filter, dest_is_dtor, p_member_governing, members_check_moved, members_in_array_element, members_check_ended, base_is_ptr);
                member = member->next;
            }

            /*
                Unconditional: dest_elem_type is zero-initialized, so destroying it
                is a no-op when the array branch above never ran.
            */
            type_destroy(&dest_elem_type);
        }

        const struct flow_key_alternatives* _Opt p_source_key_alternatives = flow_branch_search_up(ctx->p_current_flow_branch, p_object_src);
        if (p_source_key_alternatives == NULL)
        {
            throw;
        }


        for (int ri = 0; ri < p_source_key_alternatives->alternatives.size; ri++)
        {
            struct flow_alternative* p_alternative = p_source_key_alternatives->alternatives.data[ri];
            /* flow_alternative_print(p_alternative); */
            /* printf("\n"); */

            if (!flow_branch_is_ancestor_or_self(p_alternative->p_origin_map, p_origin_filter))
            {
                /* Origin on a sibling arm: never coexists with this pointer alternative. */
                continue;
            }

            if (p_alternative->imaginary == FLOW_IMAGINARY_ENDED && check_ended)
            {
                /*
                    accumulated across alternatives; the entry also keeps the
                    exiting-object check from reporting it again
                */
                if (!bare_name_built)
                {
                    flow_access_name_to_string(parent_expression_str, base_is_ptr, p_name_link, &bare_name_ss);
                    bare_name_built = true;
                    bare_name = bare_name_ss.c_str ? bare_name_ss.c_str : parent_expression_str;
                }
                struct osstream ended_ss = { 0 };
                ss_fprintf(&ended_ss, "lifetime of '%s' has ended", bare_name);
                flow_finding_record(ctx, p_object_src, W_FLOW_LIFETIME_ENDED, &marker,
                                    p_alternative, p_alternative->p_origin_map,
                                    ended_ss.c_str ? ended_ss.c_str : "");
                ss_close(&ended_ss);

                continue; /* no need to print other errors */
            }

            if (p_alternative->value_kind == FLOW_VALUE_KIND_REF)
            {
                if (p_alternative->value.p != p_object_src)
                {
                    flow_check_object_access(ctx,
                                             parent_expression_str,
                                             p_name_link,
                                             p_expression,
                                             p_alternative->value.p, check_unitialized, p_origin_filter, dest_is_dtor,
                                             p_dest_governing_type, check_moved, in_array_element, check_ended, base_is_ptr);

                    continue;
                }
            }

            /*
                Skipped for a _Dtor destination: a destructor must accept a
                partially-created object, so a null member is allowed there.
            */
            /*
                The pointer's nullability is governed by the destination (parameter)
                type when one was threaded in (array-element case above); otherwise by
                the object's own declared type.
            */
            const struct type* p_null_type =
                p_dest_governing_type != NULL ? p_dest_governing_type : &p_object_src->type;
            if (!dest_is_dtor &&
                    type_is_pointer(&p_object_src->type) &&
                    !type_is_nullable(p_null_type, options_default_nonnull(&ctx->ctx->options)) &&
                    flow_alternative_can_be_zero(p_alternative) &&
                    !in_array_element)
            {
                /* Two different facts share this call, told apart by whether a destination type was threaded in: with one, %s is a value being read into something else that disallows null (its own type may be _Opt); without one, %s's own declared type is what's non-nullable (the recursive struct-member case). */
                /*
                    Accumulated: several alternatives may support "can be zero"; one
                    report, with the count and the path that reached it.
                */
                if (!bare_name_built)
                {
                    flow_access_name_to_string(parent_expression_str, base_is_ptr, p_name_link, &bare_name_ss);
                    bare_name_built = true;
                    bare_name = bare_name_ss.c_str ? bare_name_ss.c_str : parent_expression_str;
                }
                struct osstream null_ss = { 0 };
                ss_fprintf(&null_ss,
                           p_dest_governing_type != NULL ?
                               "'%s' may be null, but the elements of the array parameter are not nullable" :
                               "'%s' may be null, but is declared non-nullable",
                           bare_name);
                flow_finding_record(ctx, p_object_src, W_FLOW_NULLABLE_TO_NON_NULLABLE, &marker,
                                    p_alternative, ctx->p_current_flow_branch,
                                    null_ss.c_str ? null_ss.c_str : "");
                ss_close(&null_ss);
            }

            if (p_alternative->imaginary != FLOW_IMAGINARY_ENDED && p_alternative->value_relation == FLOW_RELATION_UNINITIALIZED)
            {
                if (check_unitialized)
                {

                    /*
                        W_FLOW_UNINITIALIZED (30), not the nullability id: a different analysis,
                        separately suppressible.
                    */
                    if (!bare_name_built)
                    {
                        flow_access_name_to_string(parent_expression_str, base_is_ptr, p_name_link, &bare_name_ss);
                        bare_name_built = true;
                        bare_name = bare_name_ss.c_str ? bare_name_ss.c_str : parent_expression_str;
                    }
                    if (diagnostic(W_FLOW_UNINITIALIZED,
                                   ctx->ctx, NULL, &marker,
                                   "passing a possibly uninitialized object '%s'",
                                   bare_name))
                    {
                        flow_explain_alternative(ctx, p_alternative, p_alternative->p_origin_map, &marker);
                    }
                }
            }

            if (p_alternative->imaginary == FLOW_IMAGINARY_MOVED && check_moved)
            {
                /*
                    W_FLOW_MOVED (32): use-after-move is its own condition, and 31
                    is already carrying "lifetime has ended". flow1 uses 32 here.
                */
                if (!bare_name_built)
                {
                    flow_access_name_to_string(parent_expression_str, base_is_ptr, p_name_link, &bare_name_ss);
                    bare_name_built = true;
                    bare_name = bare_name_ss.c_str ? bare_name_ss.c_str : parent_expression_str;
                }
                if (diagnostic(W_FLOW_MOVED,
                               ctx->ctx, NULL, &marker,
                               "'%s' was moved",
                               bare_name))
                {
                    flow_explain_alternative(ctx, p_alternative, p_alternative->p_origin_map, &marker);
                }
            }
        }
    }
    catch
    {
    }

    ss_close(&bare_name_ss);
}


static void flow_explain_alternative_not_true(struct osstream* ss, const struct flow_alternative* alt, int warning_line)
{
    if (alt->imaginary == FLOW_IMAGINARY_MOVED)
    {
        ss_fprintf(ss, "result uses a moved object");
    }
    else if (alt->imaginary == FLOW_IMAGINARY_ENDED)
    {
        ss_fprintf(ss, "result uses an expired object");
    }
    else if (alt->value_relation == FLOW_RELATION_UNINITIALIZED)
    {
        ss_fprintf(ss, "result may be uninitialized");
    }
    else if (alt->value_relation == FLOW_RELATION_EQUAL && flow_value_is_false(alt))
    {
        ss_fprintf(ss, "result is zero");
    }
    else if (alt->value_relation == FLOW_RELATION_ANY ||
             alt->value_relation == FLOW_RELATION_NOT_EQUAL)
    {
        /* NOT_EQUAL: only known to differ from some nonzero value */
        ss_fprintf(ss, "result may be zero");
    }
    else
    {
        ss_fprintf(ss, "result not proven nonzero");
    }

    if (flow_alternative_line(alt) != warning_line)
        ss_fprintf(ss, " (set at line %d)", flow_alternative_line(alt));
}

/*
    type_is_pointed_* answer false for an array parameter; these check the
    element type, so `char e[]` behaves as `char* e`.
*/
static enum type_qualifier_flags flow_dest_pointee_qualifiers(const struct type* p_type)
{
    if (type_is_array(p_type) && p_type->next != NULL)
    {
        /* as get_array_item_type: a const/volatile on the array node belongs to the element (C11 6.7.3p9) */
        return p_type->next->type_qualifier_flags |
            (p_type->type_qualifier_flags & (TYPE_QUALIFIER_CONST | TYPE_QUALIFIER_VOLATILE));
    }
    if (type_is_pointer(p_type) && p_type->next != NULL)
        return p_type->next->type_qualifier_flags;
    return 0;
}

static bool flow_dest_pointee_is_const(const struct type* p_type)
{
    return flow_dest_pointee_qualifiers(p_type) & TYPE_QUALIFIER_CONST;
}

static bool flow_dest_pointee_is_ctor(const struct type* p_type)
{
    return flow_dest_pointee_qualifiers(p_type) & TYPE_QUALIFIER_CAKE_CTOR;
}

/* Without -safety a non-const pointer parameter may only write the pointee (no _Out to say so), so only a const one is known to read it. */
static bool flow_param_reads_pointee(const struct flow_ctx* ctx, const struct type* p_type)
{
    if (flow_dest_pointee_is_ctor(p_type))
        return false;
    return ctx->ctx->options.safety_enabled || flow_dest_pointee_is_const(p_type);
}

static bool flow_dest_pointee_is_dtor(const struct type* p_type)
{
    return flow_dest_pointee_qualifiers(p_type) & TYPE_QUALIFIER_CAKE_DTOR;
}

static bool flow_dest_pointee_is_clear(const struct type* p_type)
{
    return flow_dest_pointee_qualifiers(p_type) & TYPE_QUALIFIER_CAKE_CLEAR;
}

/*
    An _Owner member of a _View object is a view: no move on assignment,
    no "owner not moved" at exit. _View is on the aggregate, so walk parents.
*/
static bool flow_object_under_view(const struct object* obj)
{
    for (const struct object* _Opt p = obj->parent; p != NULL; p = p->parent)
    {
        if (type_is_view(&p->type))
            return true;
    }
    return false;
}

/*
    Every leaf member gets a flow state before a leak check walks them, so a
    member the source never reads is still checked
    (github.com/thradams/cake/issues/459, owner-resource-059.c).
*/
static void flow_seed_all_members_default(struct flow_ctx* ctx, struct object* p_obj, const struct token* _Opt p_token)
{
    if (p_obj->members.head)
    {
        for (struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow_seed_all_members_default(ctx, member, p_token);
        }
        return;
    }
    flow_seed_member_default(ctx, p_obj, p_token);
}

/*
    True if p_obj is ctx's pending pre-reported object or a member of it.
    See the field comment in flow3.h.
*/
static bool flow_object_is_pending_ended_report(const struct flow_ctx* ctx, const struct object* p_obj)
{
    if (ctx->p_pending_ended_report_obj == NULL)
        return false;

    for (const struct object* _Opt cur = p_obj; cur; cur = cur->parent)
    {
        if (cur == ctx->p_pending_ended_report_obj)
            return true;
    }
    return false;
}

/*
    The callee receives a reference to `pointee` through a pointer (or array)
    parameter: record what the call does to it and check what it reads.
    Array destinations go through the element type (flow_dest_pointee_is_*).
*/
static void flow_apply_pointee_param_effect(struct flow_ctx* ctx,
                                            struct expression* p_expression,
                                            const struct object* p_object_dest,
                                            const struct object* pointee,
                                            bool dtor_here,
                                            const struct flow_branch* _Opt p_origin_filter,
                                            const struct object* p_pointer_object)
{
    enum flow_pointee_effect_kind effect_kind = FLOW_EFFECT_NONE;
    const struct token* p_token = p_expression->first_token;

    /*
        _Out promises the callee handles a fully uninitialized pointee: owners
        still alive in it are lost (issue #484). Plain _Clear may free owned
        members itself before zeroing (issue #468).
    */
    if (flow_dest_pointee_is_ctor(&p_object_dest->type))
    {
        struct marker clear_marker = expression_to_marker(p_expression);
        flow_check_discarding_owner_before_overwrite(ctx, p_expression, pointee, &clear_marker);
    }

    if (flow_dest_pointee_is_clear(&p_object_dest->type))
    {
        effect_kind = FLOW_EFFECT_CLEAR; /* zero every member (+ end reachable owner pointees) */
    }
    else if (flow_dest_pointee_is_dtor(&p_object_dest->type))
        effect_kind = FLOW_EFFECT_DTOR; /* uninitialize every member (+ end reachable owner pointees) */
    else if (flow_dest_pointee_is_ctor(&p_object_dest->type))
        effect_kind = FLOW_EFFECT_ANY; /* uninitialized / any (constructor) */
    else if (type_is_owner(&p_object_dest->type))
        effect_kind = FLOW_EFFECT_LIFETIME_ENDED; /* owner takes ownership -> whole object moved/ended */
    else if (!flow_dest_pointee_is_const(&p_object_dest->type))
        effect_kind = FLOW_EFFECT_ANY; /* plain mutable pointer (or array) -> ANY */

    if (effect_kind != FLOW_EFFECT_NONE)
    {
        if (ctx->collect_deferred_effects)
        {
            /* Defer effects until all arguments are evaluated. */
            if (ctx->deferred_effects_count <
                    (int)(sizeof ctx->deferred_effects / sizeof ctx->deferred_effects[0]))
            {
                ctx->deferred_effects[ctx->deferred_effects_count].pointee = pointee;
                ctx->deferred_effects[ctx->deferred_effects_count].kind = effect_kind;
                ctx->deferred_effects[ctx->deferred_effects_count].p_token = p_token;
                ctx->deferred_effects[ctx->deferred_effects_count].p_target_origin = p_origin_filter;
                ctx->deferred_effects[ctx->deferred_effects_count].p_pointer_object = p_pointer_object;
                ctx->deferred_effects[ctx->deferred_effects_count].argument_serial = ctx->argument_serial;
                ctx->deferred_effects_count++;
            }
        }
        else
        {
            switch (effect_kind)
            {
                case FLOW_EFFECT_CLEAR:
                    flow_branch_apply_dtor_or_clear_effect(ctx->p_current_flow_branch, pointee, true, p_token);
                break;
                case FLOW_EFFECT_LIFETIME_ENDED:
                    flow_branch_set_object_lifetime_ended(ctx->p_current_flow_branch, pointee, p_token);
                break;
                case FLOW_EFFECT_ANY:
                    flow_branch_set_object_any_n(ctx->p_current_flow_branch, pointee, p_token,
                                          options_default_nonnull(&ctx->ctx->options));
                break;
                case FLOW_EFFECT_DTOR:
                    flow_branch_apply_dtor_or_clear_effect(ctx->p_current_flow_branch, pointee, false, p_token);
                break;
                default:
                break;
            }
        }
    }

    /* Also check the pointee for uninitialized / moved state (when the argument itself is read). */
    const bool source_uninit = type_is_uninit(&p_expression->object.type) || type_is_pointed_uninit(&p_expression->object.type);
    const bool check_uninitialized = flow_param_reads_pointee(ctx, &p_object_dest->type) && !source_uninit;
    /*
        For an array parameter, pass its type so the argument array's
        elements are checked against the parameter's element _Opt.
    */
    const struct type* _Opt gov =
        type_is_array(&p_object_dest->type) ? &p_object_dest->type : NULL;
    struct osstream arg_ss = { 0 };
    flow_expression_to_string(p_expression, &arg_ss);
    flow_findings_join(ctx);
    /* an array argument names its elements' storage directly: 'buf', not '*buf' */
    const bool base_is_ptr = !type_is_array(&p_expression->object.type);
    flow_check_object_access(ctx, arg_ss.c_str ? arg_ss.c_str : "", NULL, p_expression, pointee, check_uninitialized, p_origin_filter, dtor_here, gov, true, false, true, base_is_ptr);
    flow_findings_end(ctx);
    ss_close(&arg_ss);
}

/*
    Makes the elements of a parameter pointee's array (MAKE_STATE_ANY_LAZY_ARRAYS),
    seeded at the function root as they would have been on entry.
*/
static void flow_make_lazy_array(struct flow_ctx* ctx, struct object* _Opt p_array, const struct token* _Opt p_token)
{
    if (p_array == NULL ||
        !type_is_array(&p_array->type) ||
        p_array->type.array_num_elements == 0 ||
        p_array->members.head != NULL)
    {
        return;
    }

    struct flow_branch* _Opt p_current = ctx->p_current_flow_branch;
    for (struct flow_branch* _Opt p_branch = p_current; p_branch != NULL; p_branch = p_branch->p_parent_map)
    {
        ctx->p_current_flow_branch = p_branch;
    }

    struct type item_type = get_array_item_type(&p_array->type);
    const unsigned long long max_elements = p_array->type.array_num_elements > 1000 ? 1000 : p_array->type.array_num_elements;
    for (unsigned long long k = 0; k < max_elements; k++)
    {
        struct object* _Owner _Opt p_new_element = calloc(1, sizeof(struct object));
        if (p_new_element == NULL)
        {
            break;
        }
        char designator[200] = { 0 };
        snprintf(designator, sizeof designator, "%s[%llu]", p_array->member_designator ? p_array->member_designator : "", k);
        make_object_with_member_designator(&item_type, p_new_element, designator, MAKE_STATE_ANY_LAZY_ARRAYS, &ctx->ctx->options.platform);
        p_new_element->parent = p_array;
        flow_parameter_object_init_r(ctx, p_new_element, &p_new_element->type, p_token, 1, false);
        object_list_push(&p_array->members, p_new_element);
    }
    type_destroy(&item_type);
    ctx->p_current_flow_branch = p_current;
}

static void flow_check_object_init_assigment(struct flow_ctx* ctx,
                                             struct expression* p_expression,
                                             const struct object* _Opt p_object_dest, /* uninitialized always */
                                             const struct object* _Opt p_object_src,
                                             enum init_type init_type,
                                             bool dest_is_dtor,
                                             bool dest_is_view)
{
    bool findings_scope_opened = false;

    try
    {
        if (ctx->p_current_flow_branch == NULL)
            throw;

        if (p_object_src == NULL || p_object_dest == NULL)
            return;

        /*
            A _Dtor destination accepts a partially-created object: members may be
            null. Propagates down the recursion (dtor_is_opt.c).
        */
        const bool dtor_here = dest_is_dtor || flow_dest_pointee_is_dtor(&p_object_dest->type);

        /*
            A _View destination borrows: nothing moves into it. Propagates down the
            recursion like dest_is_dtor (ownership.c).
        */
        const bool view_here = dest_is_view || type_is_view(&p_object_dest->type) ||
            flow_object_under_view(p_object_dest);

        /*
            Skip only when forwarding an _Out pointee as another call's argument.
            Only INIT_PARAMETER: in `*p = 1;` (INIT_OBJ) p_object_dest is also the
               _Out-qualified pointee, and that write is a real assignment (issue #471).
        */
        if (init_type == INIT_PARAMETER && type_is_out(&p_object_dest->type))
            return;

        struct marker marker = expression_to_marker(p_expression);

        /*
            An array parameter is a pointer: the callee gets the caller's array, so
            apply pointer-parameter effects, not an element copy (issue #461).
            Only the argument itself: an array member of a by-value struct is copied.
        */

        if (init_type == INIT_PARAMETER &&
                p_object_src == &p_expression->object &&
                type_is_array(&p_object_dest->type) &&
                type_is_array(&p_object_src->type))
        {
            /*
                Apply the effect to the caller's array via the argument's REF
                alternatives (`f(p->buf)`); without a REF the argument's own object is
                the array.
            */
            bool array_reached_through_ref = false;
            const struct flow_key_alternatives* _Opt p_src_alternatives =
                flow_branch_search_up(ctx->p_current_flow_branch, p_object_src);

            for (int src_index = 0; p_src_alternatives != NULL && src_index < p_src_alternatives->alternatives.size; src_index++)
            {
                const struct flow_alternative* p_src_alternative = p_src_alternatives->alternatives.data[src_index];
                if (p_src_alternative->value_kind == FLOW_VALUE_KIND_REF &&
                        p_src_alternative->value.p != NULL &&
                        p_src_alternative->value.p != p_object_src)
                {
                    flow_apply_pointee_param_effect(ctx, p_expression, p_object_dest,
                                                    p_src_alternative->value.p, dtor_here,
                                                    p_src_alternative->p_origin_map, p_object_src);
                    array_reached_through_ref = true;
                }
            }

            if (!array_reached_through_ref)
            {
                flow_apply_pointee_param_effect(ctx, p_expression, p_object_dest,
                                                p_object_src, dtor_here, NULL, p_object_src);
            }
            return;
        }

        /* a side made without elements (MAKE_STATE_ANY_LAZY_ARRAYS) gets them when the other has them */
        if (p_object_src->members.head && !p_object_dest->members.head)
        {
            flow_make_lazy_array(ctx, (struct object* _Opt)p_object_dest, p_expression->first_token);
        }
        else if (!p_object_src->members.head && p_object_dest->members.head)
        {
            flow_make_lazy_array(ctx, (struct object* _Opt)p_object_src, p_expression->first_token);
        }

        if (p_object_src->members.head && p_object_dest->members.head)
        {
            /*
                A dereference (`*p`) is a placeholder: its state is in its REF
                alternatives, handled by the per-alternative loop below
                (swap-through-pointers.c).
            */
            const struct flow_key_alternatives* _Opt p_src_alts =
                flow_branch_search_up(ctx->p_current_flow_branch, p_object_src);

            /*
                Read each alternative: a REF source is read through the object it names;
                an aggregate with its own state keeps the member path below.
            */
            bool handled_by_reference = false;
            for (int ai = 0; p_src_alts != NULL && ai < p_src_alts->alternatives.size; ai++)
            {
                const struct flow_alternative* a = p_src_alts->alternatives.data[ai];
                if (a->value_kind != FLOW_VALUE_KIND_REF) continue;
                if (a->value.p == NULL || a->value.p == p_object_src) continue;
                if ((uintptr_t)a->value.p < 0x100000) continue; /* defensive, as elsewhere */
                if (!a->value.p->members.head) continue;

                flow_check_object_init_assigment(ctx, p_expression, p_object_dest,
                                                 a->value.p, init_type, dtor_here, view_here);
                handled_by_reference = true;
            }
            if (handled_by_reference)
                return;

            /*
                A struct-copy source with no tracked state: the assignment happened with
                unknown values, so the destination's members become ANY
                (struct-copy-from-deref-keeps-initializer.c).
            */
            if (init_type == INIT_OBJ &&
                    !type_is_array(&p_object_src->type) &&
                    !type_is_array(&p_object_dest->type))
            {
                /*
                    Not arrays: C zero-fills elements the initializer omits
                    (array-string-init-zero-fill.c).
                */
                bool src_member_state_known = false;
                for (const struct object* _Opt m = p_object_src->members.head;
                        m != NULL && !src_member_state_known;
                        m = m->next)
                {
                    const struct flow_key_alternatives* _Opt e =
                        flow_branch_search_up(ctx->p_current_flow_branch, m);
                    if (e != NULL && e->alternatives.size > 0)
                    {
                        src_member_state_known = true;
                    }
                }

                if (!src_member_state_known)
                {
                    flow_branch_set_object_any_n(ctx->p_current_flow_branch, p_object_dest,
                                              p_expression->first_token,
                                              options_default_nonnull(&ctx->ctx->options));
                    return;
                }
            }

            {
                /*
                    A union's members share storage: once any member is initialized the
                    whole union is, so recurse only on the initialized member(s) and skip
                    the uninitialized siblings.
                */
                const bool union_init = flow_union_is_initialized(ctx, p_object_src);

                struct object* _Opt member_dest = p_object_dest->members.head;
                struct object* _Opt member_src = p_object_src->members.head;
                while (member_src && member_dest)
                {
                    if (!(union_init && !flow_object_has_initialized_state(ctx, member_src)))
                        flow_check_object_init_assigment(ctx, p_expression, member_dest, member_src, init_type, dtor_here, view_here);
                    member_src = member_src->next;
                    member_dest = member_dest->next;
                }

                /*
                    C zero-fills the elements an initializer does not supply
                    (`char s[512] = "";`).
                */
                if (init_type == INIT_OBJ && type_is_array(&p_object_dest->type))
                {
                    while (member_dest)
                    {
                        flow_branch_set_object_zero(ctx->p_current_flow_branch, member_dest,
                                                 p_expression->first_token);
                        member_dest = member_dest->next;
                    }
                }
                return;
            }
        }
        /* Array-to-pointer decay: a pointer from an array is non-null. */
        if (init_type == INIT_OBJ &&
                type_is_array(&p_object_src->type) &&
                type_is_pointer(&p_object_dest->type))
        {
            struct flow_key_alternatives* _Opt e =
                flow_branch_find_add(ctx->p_current_flow_branch, p_object_dest);
            if (e == NULL) throw;
            /* like &array[0] when the array is known, so its lifetime can be checked (issue #210) */
            const struct object* _Opt p_array = NULL;
            const struct flow_key_alternatives* _Opt p_src_alternatives = flow_branch_search_up(ctx->p_current_flow_branch, p_object_src);
            for (int i = 0; p_src_alternatives != NULL && i < p_src_alternatives->alternatives.size; i++)
            {
                if (p_src_alternatives->alternatives.data[i]->value_kind == FLOW_VALUE_KIND_REF &&
                        p_src_alternatives->alternatives.data[i]->value.p != NULL)
                {
                    p_array = p_src_alternatives->alternatives.data[i]->value.p;
                    break;
                }
            }

            flow_alternatives_clear(&e->alternatives);
            struct flow_alternative a =
            {
                .value_kind = FLOW_VALUE_KIND_PTR,
                .value = {.p = p_array ? p_array->members.head : NULL},
                .value_relation = (p_array && p_array->members.head) ? FLOW_RELATION_EQUAL : FLOW_RELATION_NOT_EQUAL,
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = ctx->p_current_flow_branch,
                .p_origin_token = p_expression->first_token
            };
            flow_alternatives_add(&e->alternatives, &a);
            return;
        }

        if (p_object_src->members.head && !p_object_dest->members.head)
        {
            /* Array-to-pointer decay: handled above. */
            /* fall through */
        }

        const struct flow_key_alternatives* _Opt p_src_key_alternatives =
            flow_branch_search_up(ctx->p_current_flow_branch, p_object_src);

        /*
            An empty entry is unknown too: `bool n; n = a && b;` when a is not a
            known boolean leaves the result with no values. Not for a pointer:
            ANY would read as "may be null".
        */
        if (p_src_key_alternatives == NULL ||
            (p_src_key_alternatives->alternatives.size == 0 && !type_is_pointer(&p_object_dest->type)))
        {
            /*
                An unknown source ('we know nothing about this value') is not
                uninitialized: a real INIT_OBJ assignment records ANY on the
                destination, e.g. `const bool e = (a || b);`.
            */
            if (init_type == INIT_OBJ)
            {
                /*
                    Only replace an UNINITIALIZED destination: zero-fill / decay paths may
                    already have set a precise state (array-string-init-zero-fill.c).
                */
                const struct flow_key_alternatives* _Opt e_cur =
                    flow_branch_search_up(ctx->p_current_flow_branch, p_object_dest);

                bool dest_is_uninitialized = true;
                if (e_cur != NULL)
                {
                    for (int i = 0; i < e_cur->alternatives.size; i++)
                    {
                        if (e_cur->alternatives.data[i]->value_relation != FLOW_RELATION_UNINITIALIZED)
                        {
                            dest_is_uninitialized = false;
                            break;
                        }
                    }
                }

                struct flow_key_alternatives* _Opt e_any = dest_is_uninitialized
                    ? flow_branch_find_add(ctx->p_current_flow_branch, p_object_dest)
                    : NULL;

                if (e_any != NULL)
                {
                    flow_alternatives_clear(&e_any->alternatives);
                    struct flow_alternative a =
                    {
                        .value_kind = FLOW_VALUE_KIND_SIGNED,
                        .value = {.i = ANY_VALUE},
                        .value_relation = FLOW_RELATION_ANY,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = ctx->p_current_flow_branch,
                        .p_origin_token = p_expression->first_token
                    };
                    flow_alternatives_add(&e_any->alternatives, &a);
                }
            }
            return;
        }

        struct flow_key_alternatives* _Opt p_dest_key_alternatives = NULL;
        if (init_type == INIT_OBJ)
        {
            p_dest_key_alternatives = flow_branch_find_add(ctx->p_current_flow_branch, p_object_dest);
            if (p_dest_key_alternatives == NULL) throw;
            flow_alternatives_clear(&p_dest_key_alternatives->alternatives);
        }

        /*
            One report per finding per assignment (flow_finding_record), not per
            source alternative. The recursive calls (REF sources, members) join the
            outermost scope.
        */
        flow_findings_begin(ctx);
        findings_scope_opened = true;
        const int marker_line = marker.p_token_begin ? marker.p_token_begin->line : 0;

        for (int ri = 0; ri < p_src_key_alternatives->alternatives.size; ri++)
        {
            struct flow_alternative* p_src_alternative = p_src_key_alternatives->alternatives.data[ri];

            if (p_src_alternative->imaginary == FLOW_IMAGINARY_ENDED &&
                    !flow_branch_is_ancestor_or_self(p_src_alternative->p_origin_map, ctx->p_current_flow_branch))
            {
                /*
                    ENDED fact from a sibling branch not open on this path
                    (deref-after-catch-reset-false-positive.c).
                */
                continue;
            }

            if (p_src_alternative->imaginary == FLOW_IMAGINARY_ENDED)
            {
                /*
                    Already reported by EXPR_UNARY_CONTENT for this object/line; consume it
                    once (see the field comment in flow3.h).
                */
                if (flow_alternative_line(p_src_alternative) == ctx->pending_ended_report_line &&
                        flow_object_is_pending_ended_report(ctx, p_object_src))
                {
                    /* an entry with no message: the rest are not reported */
                    flow_finding_already_reported(ctx, p_object_src, marker_line, W_FLOW_LIFETIME_ENDED);
                    ctx->p_pending_ended_report_obj = NULL;
                    continue;
                }

                struct osstream name_ss = { 0 };
                flow_expression_to_string(p_expression, &name_ss);
                struct osstream ss = { 0 };
                ss_fprintf(&ss, "lifetime of '%s' has ended", name_ss.c_str ? name_ss.c_str : "");
                flow_finding_record(ctx, p_object_src, W_FLOW_LIFETIME_ENDED, &marker,
                                    p_src_alternative, p_src_alternative->p_origin_map,
                                    ss.c_str ? ss.c_str : "");
                ss_close(&ss);
                ss_close(&name_ss);
                continue;
            }

            if (p_src_alternative->value_kind == FLOW_VALUE_KIND_REF)
            {
                if (p_src_alternative->value.p != NULL &&
                        (uintptr_t)p_src_alternative->value.p < 0x100000)
                {
                    /* Defensive: skip corrupted REF. */
                    continue;
                }

                if (p_src_alternative->value.p != p_object_src)
                {
                    flow_check_object_init_assigment(ctx,
                                                     p_expression,
                                                     p_object_dest,
                                                     p_src_alternative->value.p,
                                                     init_type,
                                                     dtor_here,
                                                     view_here);

                    if (type_is_owner(&p_object_dest->type) && !view_here)
                    {
                        flow_branch_set_object_moved(ctx->p_current_flow_branch,
                                                  p_src_alternative->value.p,
                                                  p_expression->first_token);
                    }
                    continue;
                }

                if (init_type == INIT_PARAMETER &&
                        type_is_pointer_or_array(&p_object_dest->type) &&
                        p_src_alternative->value.p != NULL)
                {
                    flow_apply_pointee_param_effect(ctx,
                                                    p_expression,
                                                    p_object_dest,
                                                    p_src_alternative->value.p,
                                                    dtor_here,
                                                    p_src_alternative->p_origin_map,
                                                    p_object_src);
                }

            }

            if (init_type == INIT_OBJ && p_dest_key_alternatives != NULL)
            {
                const bool integer_zero =
                    p_src_alternative->value_relation == FLOW_RELATION_EQUAL &&
                    ((p_src_alternative->value_kind == FLOW_VALUE_KIND_SIGNED && p_src_alternative->value.i == 0) ||
                     (p_src_alternative->value_kind == FLOW_VALUE_KIND_UNSIGNED && p_src_alternative->value.u == 0));

                if (integer_zero && type_is_pointer(&p_object_dest->type))
                {
                    /* A null pointer constant assigned to a pointer is a null pointer. */
                    struct flow_alternative null_pointer = *p_src_alternative;
                    null_pointer.value_kind = FLOW_VALUE_KIND_PTR;
                    null_pointer.value.p = NULL;
                    flow_alternatives_add(&p_dest_key_alternatives->alternatives, &null_pointer);
                }
                else
                {
                    flow_alternatives_add(&p_dest_key_alternatives->alternatives, p_src_alternative);
                }
            }

            if (p_src_alternative->value_relation == FLOW_RELATION_EQUAL &&
                    p_src_alternative->value_kind == FLOW_VALUE_KIND_PTR &&
                    p_src_alternative->value.p)
            {
                struct osstream ss = { 0 };
                flow_expression_to_string(p_expression, &ss);
                struct osstream ss2 = { 0 };
                ss_fprintf(&ss2, "%s", ss.c_str);

                if (ss2.c_str == NULL)
                {
                    ss_close(&ss);
                    ss_close(&ss2);
                    throw;
                }

                if (!type_is_void_ptr(&p_object_dest->type))
                {
                    const bool source_uninit = type_is_uninit(&p_expression->object.type) || type_is_pointed_uninit(&p_expression->object.type);

                    /*
                        Returning a pointer does not read its pointee: no uninitialized finding
                        on return. INIT_PARAMETER stays checked unless the callee is _Out.
                    */
                    const bool check_unitialized =
                        !flow_dest_pointee_is_ctor(&p_object_dest->type) &&
                        (init_type != INIT_PARAMETER || flow_param_reads_pointee(ctx, &p_object_dest->type)) &&
                        !source_uninit &&
                        init_type != INIT_RETURN;

                    flow_check_object_access(ctx, ss2.c_str ? ss2.c_str : "", NULL, p_expression, p_src_alternative->value.p,
                                             check_unitialized, p_src_alternative->p_origin_map, dtor_here, NULL, true, false, true, true);
                }

                /*
                    Moving into `void* _Owner` erases the pointee's type: check its owner
                    members now (void-owner-erasure-leaks-member.c).
                */
                if (type_is_owner(&p_object_dest->type) &&
                        type_is_void_ptr(&p_object_dest->type) &&
                        p_src_alternative->value.p != NULL)
                {
                    /* Force every member's flow state to exist before the leak check (else 'no state' reads as 'nothing to report'), and blame the pointer's own acquisition line rather than this call site, since the member itself was never assigned in this function. User-reported. */
                    struct object* _Opt p_pointed_obj =
                        (struct object*)(uintptr_t)p_src_alternative->value.p;
                    if (p_pointed_obj != NULL)
                    {
                        flow_seed_all_members_default(ctx, p_pointed_obj,
                                                      p_src_alternative->p_origin_token);
                    }

                    /* Use the erased expression as root name ("p->m", not ".m"). */
                    struct osstream root_name_ss = { 0 };
                    flow_expression_to_string(p_expression, &root_name_ss);
                    flow_check_object_at_exit(ctx,
                                              &p_src_alternative->value.p->type,
                                              p_src_alternative->value.p,
                                              &marker,
                                              p_expression->first_token,
                                              view_here,
                                              root_name_ss.c_str,
                                              true);
                    ss_close(&root_name_ss);
                }

                if (init_type == INIT_PARAMETER)
                {
                    /* Determine write-effect on the pointee (same as above, but for a PTR alternative). */
                    const struct object* pointee = p_src_alternative->value.p;
                    const struct token* p_effect_token = p_expression->first_token;
                    enum flow_pointee_effect_kind effect_kind = FLOW_EFFECT_NONE;

                    /* Same _Out rule as flow_apply_pointee_param_effect. */
                    if (flow_dest_pointee_is_ctor(&p_object_dest->type))
                    {
                        struct marker clear_marker2 = expression_to_marker(p_expression);
                        flow_check_discarding_owner_before_overwrite(ctx, p_expression, pointee, &clear_marker2);
                    }

                    if (flow_dest_pointee_is_clear(&p_object_dest->type))
                    {
                        effect_kind = FLOW_EFFECT_CLEAR;
                    }
                    else if (flow_dest_pointee_is_dtor(&p_object_dest->type))
                        effect_kind = FLOW_EFFECT_DTOR;
                    else if (flow_dest_pointee_is_ctor(&p_object_dest->type))
                        effect_kind = FLOW_EFFECT_ANY;
                    else if (type_is_owner(&p_object_dest->type))
                        effect_kind = FLOW_EFFECT_LIFETIME_ENDED;
                    else if (!flow_dest_pointee_is_const(&p_object_dest->type) &&
                             pointee != &p_expression->object)
                    {
                        effect_kind = FLOW_EFFECT_ANY;
                    }

                    if (effect_kind != FLOW_EFFECT_NONE)
                    {
                        if (ctx->collect_deferred_effects)
                        {
                            if (ctx->deferred_effects_count <
                                    (int)(sizeof ctx->deferred_effects / sizeof ctx->deferred_effects[0]))
                            {
                                ctx->deferred_effects[ctx->deferred_effects_count].pointee = pointee;
                                ctx->deferred_effects[ctx->deferred_effects_count].kind = effect_kind;
                                ctx->deferred_effects[ctx->deferred_effects_count].p_token = p_effect_token;
                                ctx->deferred_effects[ctx->deferred_effects_count].p_target_origin = p_src_alternative->p_origin_map;
                                ctx->deferred_effects[ctx->deferred_effects_count].p_pointer_object = p_object_src;
                                ctx->deferred_effects[ctx->deferred_effects_count].argument_serial = ctx->argument_serial;
                                ctx->deferred_effects_count++;
                            }
                        }
                        else
                        {
                            switch (effect_kind)
                            {
                                case FLOW_EFFECT_CLEAR:
                                    flow_branch_apply_dtor_or_clear_effect(ctx->p_current_flow_branch, pointee, true, p_effect_token);
                                break;
                                case FLOW_EFFECT_LIFETIME_ENDED:
                                    flow_branch_set_object_lifetime_ended(ctx->p_current_flow_branch, pointee, p_effect_token);
                                break;
                                case FLOW_EFFECT_ANY:
                                    flow_branch_set_object_any_n(ctx->p_current_flow_branch, pointee, p_effect_token,
                                                          options_default_nonnull(&ctx->ctx->options));
                                break;
                                case FLOW_EFFECT_DTOR:
                                    flow_branch_apply_dtor_or_clear_effect(ctx->p_current_flow_branch, pointee, false, p_effect_token);
                                break;
                                default:
                                break;
                            }
                        }
                    }
                }

                ss_close(&ss);
                ss_close(&ss2);
            }

            /*
                Null-pointer check for non-optional destination.
                Skipped for a _Dtor destination: a destructor must accept a
                partially-created object, so a null member is allowed there.
            */
            /* An array decays to a non-null pointer: skip it. */
            const bool src_is_array = type_is_array(&p_object_src->type);

            if (!dtor_here &&
                    !src_is_array &&
                    type_is_pointer(&p_object_dest->type) &&
                    !type_is_nullable(&p_object_dest->type, options_default_nonnull(&ctx->ctx->options)) &&
                    flow_alternative_can_be_zero(p_src_alternative))
            {
                struct osstream name_ss = { 0 };
                flow_expression_to_string(p_expression, &name_ss);
                /* inside a struct copy the null pointer is a member: name 'x.m', not the struct 'x' */
                const char* _Opt member = p_object_src->member_designator;
                if (member && member[0] == '.' && name_ss.c_str &&
                    type_is_struct_or_union(&p_expression->object.type))
                {
                    struct osstream full_ss = { 0 };
                    flow_join_member_name(&full_ss, name_ss.c_str, member + 1);
                    ss_swap(&name_ss, &full_ss);
                    ss_close(&full_ss);
                }
                /* Always name the DESTINATION's type in this message, never the source expression's own type -- the source may legitimately be _Opt itself; it's the destination that disallows null. User-reported: parser.c:5214, `p_token = ctx->current;` where ctx->current is _Opt but p_token isn't. */
                struct osstream ss = { 0 };
                const bool is_null_constant = expression_is_null_pointer_constant(p_expression);
                const char* fmt =
                    init_type == INIT_PARAMETER ?
                        (is_null_constant ? "null pointer '%s' passed to a non-nullable parameter" :
                                            "'%s' may be null, but the parameter is not nullable") :
                    init_type == INIT_RETURN ?
                        (is_null_constant ? "null pointer '%s' returned, but the return type is not nullable" :
                                            "'%s' may be null, but the return type is not nullable") :
                        (is_null_constant ? "null pointer '%s' assigned to a non-nullable pointer" :
                                            "'%s' may be null, but it is assigned to a non-nullable pointer");
                ss_fprintf(&ss, fmt, name_ss.c_str ? name_ss.c_str : "");
                flow_finding_record(ctx, p_object_src, W_FLOW_NULLABLE_TO_NON_NULLABLE, &marker,
                                    p_src_alternative, ctx->p_current_flow_branch,
                                    ss.c_str ? ss.c_str : "");
                ss_close(&ss);
                ss_close(&name_ss);
            }

            /*
                Uninitialized check. Not for a value set on a branch this path did
                not take (`if (c) { d(p); p = 0; } if (p) f(p->x);`), the same check
                the null dereference applies.
            */
            if (p_src_alternative->value_relation == FLOW_RELATION_UNINITIALIZED &&
                flow_origins_compatible(p_src_alternative->p_origin_map, ctx->p_current_flow_branch))
            {
                /*
                    A union member is initialized if any sibling is, also after a branched
                    write (union-branched-member-init.c).
                */
                const bool in_initialized_union =
                    p_object_src->parent != NULL &&
                    flow_union_is_initialized(ctx, p_object_src->parent);

                /* Honour the _Uninitialized contract on the write/return path too (already done on read): a source or destination declared _Uninitialized is not a finding -- e.g. malloc()'s return. Genuinely uninitialized locals still warn. */
                const bool source_uninit =
                    type_is_uninit(&p_expression->object.type) ||
                    type_is_pointed_uninit(&p_expression->object.type);

                const bool dest_accepts_uninit =
                    type_is_uninit(&p_object_dest->type) ||
                    type_is_pointed_uninit(&p_object_dest->type);

                if (!type_is_pointed_out(&p_object_dest->type) && !in_initialized_union &&
                        !source_uninit && !dest_accepts_uninit)
                {
                    const char* verb =
                        init_type == INIT_PARAMETER ? "passing" :
                        init_type == INIT_RETURN ? "returning" :
                        "assigning";

                    struct osstream name_ss = { 0 };
                    const char* _Opt object_name = p_object_src->member_designator;
                    if (object_name == NULL || object_name[0] == '\0' || object_name[0] == '.')
                    {
                        /* a '.member' designator is relative: put the expression that reaches it in front */
                        struct osstream expr_ss = { 0 };
                        flow_expression_to_string(p_expression, &expr_ss);
                        if (object_name != NULL && object_name[0] == '.' && expr_ss.c_str &&
                            type_is_struct_or_union(&p_expression->object.type))
                            flow_join_member_name(&name_ss, expr_ss.c_str, object_name + 1);
                        else
                            ss_fprintf(&name_ss, "%s", expr_ss.c_str ? expr_ss.c_str : "");
                        ss_close(&expr_ss);
                        object_name = name_ss.c_str ? name_ss.c_str : "";
                    }

                    struct osstream ss = { 0 };
                    ss_fprintf(&ss, "%s a possibly uninitialized object '%s'", verb, object_name);
                    flow_finding_record(ctx, p_object_src, W_FLOW_UNINITIALIZED, &marker,
                                        p_src_alternative, p_src_alternative->p_origin_map,
                                        ss.c_str ? ss.c_str : "");
                    ss_close(&ss);
                    ss_close(&name_ss);
                }
            }

            /* Moved check (use-after-move) */
            if (p_src_alternative->imaginary == FLOW_IMAGINARY_MOVED &&
                    type_is_owner(&p_object_dest->type))
            {
                struct osstream name_ss = { 0 };
                flow_expression_to_string(p_expression, &name_ss);
                /* W_FLOW_MOVED (32) -- see the note at the other move site. */
                struct osstream ss = { 0 };
                ss_fprintf(&ss, "'%s' was moved", name_ss.c_str ? name_ss.c_str : "");
                flow_finding_record(ctx, p_object_src, W_FLOW_MOVED, &marker,
                                    p_src_alternative, p_src_alternative->p_origin_map,
                                    ss.c_str ? ss.c_str : "");
                ss_close(&ss);
                ss_close(&name_ss);
            }
        }

        /*
            A copy is a move: copying an _Owner leaf consumes the source. Aggregates
            reach here once per member (`struct S t = *p;`)
            (swap-through-pointers.c).
        */
        if (!view_here &&
                type_is_owner(&p_object_src->type) &&
                type_is_owner(&p_object_dest->type) &&
                !p_object_src->members.head)
        {
            flow_branch_set_object_moved(ctx->p_current_flow_branch,
                                      p_object_src,
                                      p_expression->first_token);
        }
    }
    catch
    {
    }

    if (findings_scope_opened)
        flow_findings_end(ctx);
}

/*
    Overwriting an _Owner member that is live OR unknown (ANY) is a possible
    leak. SPECIAL CASE / known false positive: a member reached through a
    plain pointer parameter is unknown, so it is reported even if the caller
    released it (owner-moved-into-struct-member.c).
*/
/* Collect owner leaves an overwrite would discard first, report afterwards, so one assignment produces one diagnostic instead of once per leaf -- and name the single leaf individually when there's exactly one, otherwise collapse to the aggregate the assignment actually targets. */
struct flow_discarded_owner_scan
{
    int count;
    const char* _Opt name; /* designator of the first leaf found */
    const struct flow_alternative* _Opt p_alternative; /* where the first leaf's resource came from */
};

static void flow_scan_discarded_owners(struct flow_ctx* ctx,
                                       const struct object* _Opt p_object_dest,
                                       struct flow_discarded_owner_scan* scan)
{
    if (p_object_dest == NULL)
    {
        /*
            A REF alternative's value.p can be NULL when the destination
            pointer (e.g. an _Opt parameter) may itself be null at this
            point -- there is no pointee object to check in that case.
            (The null-dereference itself is reported elsewhere.)
        */
        return;
    }

    if (p_object_dest->members.head)
    {
        for (const struct object* _Opt member = p_object_dest->members.head; member; member = member->next)
        {
            flow_scan_discarded_owners(ctx, member, scan);
        }
        return;
    }

    if (!type_is_owner(&p_object_dest->type))
    {
        return;
    }

    /*
        Synthetic parameter-seed placeholders are checked like any other
        member (see the rule above flow_discarded_owner_scan).
    */

    const struct flow_key_alternatives* _Opt e = flow_branch_search_up(ctx->p_current_flow_branch, p_object_dest);
    if (e == NULL)
    {
        return;
    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow_alternative* p_alternative = e->alternatives.data[i];

        if (p_alternative->imaginary == FLOW_IMAGINARY_MOVED ||
                p_alternative->imaginary == FLOW_IMAGINARY_ENDED)
        {
            continue;
        }

        if (p_alternative->value_relation == FLOW_RELATION_UNINITIALIZED)
        {
            continue;
        }

        /* An ANY-valued _Owner member (unknown, possibly live) counts as live. */

        if (flow_alternative_is_zero(p_alternative))
        {
            continue;
        }

        /*
            One report per LEAF, not per alternative. Alternatives differ only in
            provenance -- the same rule flow_check_object_access and
               flow_check_object_init_assigment apply through
               flow_finding_record.
        */
        if (scan->count == 0)
        {
            scan->name = p_object_dest->member_designator;
            scan->p_alternative = p_alternative;
        }
        scan->count++;
        break;
    }
}

static void flow_check_discarding_owner_before_overwrite(struct flow_ctx* ctx,
                                                         const struct expression* p_expression_dest,
                                                         const struct object* _Opt p_object_dest,
                                                         const struct marker* marker)
{
    struct flow_discarded_owner_scan scan = { 0 };
    flow_scan_discarded_owners(ctx, p_object_dest, &scan);

    if (scan.count == 0)
        return;

    /*
        One leaf: name it. Several: name the assigned object. A top-level
        destination has an empty (not null) designator: test for content.
    */
    const char* _Opt designator = scan.name;
    if (scan.count > 1 &&
            p_object_dest != NULL &&
            p_object_dest->member_designator != NULL &&
            p_object_dest->member_designator[0] != '\0')
    {
        designator = p_object_dest->member_designator;
    }

    /*
        The designator is relative to the destination expression: print both
        ("*list" + ".head").
    */
    struct osstream expr_ss = { 0 };
    flow_expression_to_string(p_expression_dest, &expr_ss);
    const char* base = expr_ss.c_str ? expr_ss.c_str : "";

    /* the leaf's designator is a path from the root object; the destination expression already reaches p_object_dest, so only what is below it is appended ('member->key', not 'member->key.key') */
    if (designator != NULL &&
        p_object_dest != NULL &&
        p_object_dest->member_designator != NULL &&
        p_object_dest->member_designator[0] == '.')
    {
        const size_t n = strlen(p_object_dest->member_designator);
        if (strncmp(designator, p_object_dest->member_designator, n) == 0 &&
            (designator[n] == '\0' || designator[n] == '.'))
        {
            designator = designator + n;
        }
    }

    struct osstream name_ss = { 0 };
    if (designator != NULL && designator[0] != '\0' && designator[0] != '.')
    {
        /* without a leading '.' the designator is the whole name, root included ("t.lexeme", "p") */
        ss_fprintf(&name_ss, "%s", designator);
    }
    else if (designator != NULL && designator[0] != '\0')
    {
        flow_join_member_name(&name_ss, base, designator + 1);
    }
    else
    {
        ss_fprintf(&name_ss, "%s", base);
    }

    if (diagnostic(W_FLOW_DISCARDING_OWNER,
               ctx->ctx,
               NULL,
               marker,
               "assignment discards _Owner '%s' without releasing it first",
               name_ss.c_str ? name_ss.c_str : "") &&
        scan.p_alternative != NULL)
    {
        /* where the discarded resource came from, as a note like the other flow diagnostics */
        flow_diagnose_state_origin(ctx, scan.p_alternative, marker, W_FLOW_DISCARDING_OWNER, 0);
    }

    ss_close(&name_ss);
    ss_close(&expr_ss);
}

/*
    Apply a _Clear/_Uninitialized allocation contract to the destination's
    pointee (void* from calloc/malloc loses it at the conversion).
*/
static void flow_apply_alloc_contract_to_dest(struct flow_ctx* ctx,
                                              const struct type* _Opt p_dest_type,
                                              const struct object* _Opt p_object_dest,
                                              const struct expression* _Opt p_src_expression)
{
    if (p_dest_type == NULL || p_object_dest == NULL || p_src_expression == NULL)
        return;

    const struct type* const p_src_type = &p_src_expression->object.type;
    const bool want_zero = type_is_clear(p_src_type) || type_is_pointed_clear(p_src_type);
    const bool want_uninit = type_is_uninit(p_src_type) || type_is_pointed_uninit(p_src_type);
    /* no contract: a void* _Owner (allocation) still gets a typed pointee, in ANY state */
    const bool want_any = !want_zero && !want_uninit && type_is_void_ptr(p_src_type) && type_is_owner(p_src_type);
    if (!want_zero && !want_uninit && !want_any)
        return;

    if (!type_is_pointer(p_dest_type))
        return;

    struct type pointed_type = type_remove_pointer(p_dest_type);
    if (!type_is_void(&pointed_type))
    {
        struct object* _Opt p_pointed = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
        if (p_pointed != NULL)
        {
            const struct token* p_token = p_src_expression->first_token;
            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, &ctx->ctx->options.platform);
            if (want_zero)
                flow_branch_set_object_zero(ctx->p_current_flow_branch, p_pointed, p_token);
            else if (want_uninit)
                flow_branch_set_object_uninitialized(ctx->p_current_flow_branch, p_pointed, p_token);

            /* Repoint the destination's non-null arm; keep its null arm. */
            struct flow_key_alternatives* _Opt e =
                flow_branch_find_add(ctx->p_current_flow_branch, p_object_dest);
            if (e != NULL)
            {
                for (int i = 0; i < e->alternatives.size; i++)
                {
                    struct flow_alternative* a = e->alternatives.data[i];
                    if (a->value_kind != FLOW_VALUE_KIND_PTR) continue;
                    if (flow_alternative_is_zero(a)) continue;
                    a->value.p = p_pointed;
                    a->value_relation = FLOW_RELATION_EQUAL;
                }
            }
        }
    }
    type_destroy(&pointed_type);
}

static void flow_check_assigment(struct flow_ctx* ctx,
                                 const struct expression* p_expression_dest,
                                 struct expression* p_expression_src)
{
    const struct flow_key_alternatives* _Opt p_expression_dest_key_alternatives =
        flow_branch_search_up(ctx->p_current_flow_branch, &p_expression_dest->object);

    /*
        Several possible targets (`*p` after `p = c ? &a : &b`): each is written
        in its own arm and the arms merged. One target: plain overwrite.
    */
    const struct flow_branch* _Opt target_origins[16] = { 0 };
    int target_count = 0;
    for (int i = 0;
            p_expression_dest_key_alternatives != NULL &&
            i < p_expression_dest_key_alternatives->alternatives.size; i++)
    {
        const struct flow_alternative* p_alternative = p_expression_dest_key_alternatives->alternatives.data[i];
        if (p_alternative->value_kind == FLOW_VALUE_KIND_REF &&
                p_alternative->value.p != NULL &&
                (uintptr_t)p_alternative->value.p >= 0x100000)
        {
            if (target_count < (int)(sizeof target_origins / sizeof target_origins[0]))
            {
                target_origins[target_count] = p_alternative->p_origin_map;
            }
            target_count++;
        }
    }

    struct flow_branch* _Opt p_before = ctx->p_current_flow_branch;
    const struct flow_branch* _Opt target_arms[16] = { 0 };
    const bool write_in_arms = p_before != NULL &&
        target_count > 1 &&
        target_count <= (int)(sizeof target_arms / sizeof target_arms[0]) &&
        flow_target_origins_exclusive(target_origins, target_count);
    int target_arm_count = 0;

    for (int i = 0;
            p_expression_dest_key_alternatives != NULL &&
            i < p_expression_dest_key_alternatives->alternatives.size; i++)
    {
        const struct flow_alternative* p_expression_dest_alternative =
            p_expression_dest_key_alternatives->alternatives.data[i];

        if (p_expression_dest_alternative->value_kind == FLOW_VALUE_KIND_REF)
        {
            const struct object* _Opt p_object_dest = p_expression_dest_alternative->value.p;

            if (p_object_dest != NULL && (uintptr_t)p_object_dest < 0x100000)
            {
                /*
                    Defensive, as in flow_check_object_init_assigment: skip a mistagged
                    REF alternative.
                */
                continue;
            }

            struct flow_branch* _Opt p_target_arm = NULL;
            if (write_in_arms && p_before != NULL && p_object_dest != NULL)
            {
                p_target_arm = flow_branch_new_pointer_target_arm(ctx, p_before, p_expression_dest_alternative->p_origin_map);
                if (p_target_arm == NULL)
                {
                    /* never this object on this path */
                    continue;
                }
                ctx->p_current_flow_branch = p_target_arm;
            }

            struct marker marker = expression_to_marker(p_expression_dest);
            flow_check_discarding_owner_before_overwrite(ctx, p_expression_dest, p_object_dest, &marker);

            flow_check_object_init_assigment(ctx,
                                             p_expression_src,
                                             p_object_dest,
                                             &p_expression_src->object,
                                             INIT_OBJ,
                                             false,
                                             false);

            /* Retype a void calloc() pointee on the split declare-then-assign path too (already done for the combined declarator-initializer form) -- otherwise member access through it kept manufacturing fresh ANY alternatives, defeating guards like `if (p->m)`. */
            flow_apply_alloc_contract_to_dest(ctx,
                                              &p_expression_dest->object.type,
                                              p_object_dest,
                                              p_expression_src);

            if (p_target_arm != NULL)
            {
                flow_branch_tag_pointer_target_arm(p_target_arm);
                ctx->p_current_flow_branch = p_before;
                target_arms[target_arm_count] = p_target_arm;
                target_arm_count++;
            }
        }
    }

    if (p_before != NULL && target_arm_count > 0)
    {
        flow_branch_merge_arms(p_before, target_arms, target_arm_count);
    }

    /*
        Assigning to an owner lvalue moves the source. Also done here for an
        untracked deep member destination; idempotent with the loop above.
    */
    if (type_is_owner(&p_expression_dest->object.type))
    {
        const struct flow_key_alternatives* _Opt p_src =
            flow_branch_search_up(ctx->p_current_flow_branch, &p_expression_src->object);

        for (int i = 0; p_src != NULL && i < p_src->alternatives.size; i++)
        {
            const struct flow_alternative* a = p_src->alternatives.data[i];
            if (a->value_kind == FLOW_VALUE_KIND_REF && a->value.p != NULL)
                flow_branch_set_object_moved(ctx->p_current_flow_branch, a->value.p, p_expression_dest->first_token);
        }
    }
}

/*
    Make the pointee of a non-null pointer with unknown target, on demand,
    in the entry holding the pointer's values. Returns that entry, or NULL
    on out of memory.
*/
static const struct flow_key_alternatives* _Opt flow_pointer_pointee_on_demand(struct flow_ctx* ctx,
                                                                              const struct expression* p_pointer_expression,
                                                                              const struct flow_key_alternatives* _Opt p_left_alternatives)
{
    if (ctx->p_current_flow_branch != NULL &&
        p_left_alternatives != NULL &&
        type_is_pointer(&p_pointer_expression->object.type))
    {
            /*
                Fabricate into the variable's entry (via REF), not the temporary's, so
                the same pointee is found on the next read (safety-049).
            */
            const struct object* p_key = &p_pointer_expression->object;
            if (p_left_alternatives->alternatives.size > 0 &&
            p_left_alternatives->alternatives.data[0]->value_kind == FLOW_VALUE_KIND_REF &&
            p_left_alternatives->alternatives.data[0]->value.p != NULL)
            {
                const struct object* p_ref = p_left_alternatives->alternatives.data[0]->value.p;
                const struct flow_key_alternatives* _Opt p_resolved =
                flow_branch_search_up(ctx->p_current_flow_branch, p_ref);
                if (p_resolved != NULL)
                {
                    p_key = p_ref;
                    p_left_alternatives = p_resolved;
                }
            }

            /*
                A void-typed pointee (`obj = calloc(...)`, no cast node) cannot resolve
                members: re-fabricate it with the pointer's real pointed type
                (branch-merge-loses-ref-nonnull.c).
            */
            bool needs_pointee = false;
            for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
            {
                const struct flow_alternative* a = p_left_alternatives->alternatives.data[i];
                if (a->value_kind == FLOW_VALUE_KIND_PTR &&
                a->value.p == NULL &&
                a->value_relation == FLOW_RELATION_NOT_EQUAL)
                {
                    needs_pointee = true;
                    break;
                }
            }

            if (needs_pointee)
            {
                struct type pointed_type = type_remove_pointer(&p_pointer_expression->object.type);
                if (!type_is_void(&pointed_type))
                {
                    struct flow_alternatives rebuilt = { 0 };
                    /* Fabricate one pointee for the whole rebuild pass, written straight into p_key's own map entry and found again via ordinary flow_branch_search_up -- one pointee per pass, not one per matching alternative and not a side-cache, so it stays a single logical pointee (owner-if-else-052.c). */
                    struct object* _Opt p_new_shared = NULL;
                    for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
                    {
                        struct flow_alternative a = *p_left_alternatives->alternatives.data[i];
                        if (a.value_kind == FLOW_VALUE_KIND_PTR &&
                        a.value.p == NULL &&
                        a.value_relation == FLOW_RELATION_NOT_EQUAL)
                        {
                            if (p_new_shared == NULL)
                            {
                                p_new_shared = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
                                if (p_new_shared != NULL)
                                {
                                    make_object(&pointed_type, p_new_shared, MAKE_STATE_ANY, &ctx->ctx->options.platform);
                                }
                            }

                            if (p_new_shared != NULL)
                            {
                                a.value.p = p_new_shared;
                                a.value_relation = FLOW_RELATION_EQUAL;
                            }
                        }
                        flow_alternatives_add(&rebuilt, &a);
                    }

                    struct flow_key_alternatives* _Opt e_base = flow_branch_find_add(ctx->p_current_flow_branch, p_key);
                    if (e_base != NULL)
                    {
                        flow_alternatives_clear(&e_base->alternatives);
                        e_base->alternatives = rebuilt; /* move */
                        p_left_alternatives = e_base;
                    }
                    else
                    {
                        flow_alternatives_clear(&rebuilt);
                        p_left_alternatives = NULL;
                    }
                }
                type_destroy(&pointed_type);
            }
    }
    return p_left_alternatives;
}

static void flow_visit_function_arguments(struct flow_ctx* ctx,
                                          const struct type* p_type,
                                          const struct argument_expression_list* p_argument_expression_list)
{
    try
    {
        if (ctx->p_current_flow_branch == NULL)
            throw;

        const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == NULL)
        {
            throw;
        }

        struct param* _Opt p_current_parameter_type = p_param_list->head;
        struct argument_expression* _Opt p_current_argument = p_argument_expression_list->head;

        /*
            Collect pointee write-effects and apply them after all arguments:
            C evaluates every argument before the call.
        */
        const bool saved_collect = ctx->collect_deferred_effects;
        const int saved_count = ctx->deferred_effects_count;
        ctx->collect_deferred_effects = true;
        ctx->deferred_effects_count = saved_count;

        while (p_current_argument)
        {
            struct expression* p_arg_expr = p_current_argument->expression;
            /* a variadic argument is only read: its own type, as a view */
            struct type variadic_type = { 0 };
            const struct type* p_param_type = &variadic_type;
            if (p_current_parameter_type)
            {
                p_param_type = &p_current_parameter_type->type;
            }
            else
            {
                variadic_type = type_dup(&p_arg_expr->object.type);
                variadic_type.type_qualifier_flags &= ~(TYPE_QUALIFIER_CAKE_OWNER | TYPE_QUALIFIER_CAKE_DTOR |
                                                        TYPE_QUALIFIER_CAKE_CTOR | TYPE_QUALIFIER_CAKE_UNINIT);
            }

            struct object param_object = { 0 };
            make_object(p_param_type, &param_object, MAKE_STATE_UNITIALIZED, &ctx->ctx->options.platform);

            flow_visit_full_expression(ctx, p_arg_expr);

            /* the callee's effects and checks need the pointee as an object */
            if (type_is_pointer(p_param_type) && type_is_pointer(&p_arg_expr->object.type))
            {
                flow_pointer_pointee_on_demand(ctx, p_arg_expr,
                                               flow_branch_search_up(ctx->p_current_flow_branch, &p_arg_expr->object));
            }

            ctx->argument_serial++;
            flow_check_object_init_assigment(ctx, p_arg_expr, &param_object, &p_arg_expr->object, INIT_PARAMETER, false, false);

            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type ? p_current_parameter_type->next : NULL;
            object_destroy(&param_object);
            type_destroy(&variadic_type);
        }

        /*
            All arguments evaluated: now apply the deferred write-effects (the
            callee "runs"). Restore the collection state for the caller.
        */
        ctx->collect_deferred_effects = saved_collect;
        for (int group_start = saved_count; group_start < ctx->deferred_effects_count;)
        {
            /*
                The effects of one pointer's values are alternatives: the pointer
                points to one of them. With several, each is applied in its own
                arm (see flow_branch_new_pointer_target_arm) and the arms merged.
            */
            int group_end = group_start + 1;
            while (group_end < ctx->deferred_effects_count &&
                   ctx->deferred_effects[group_end].p_pointer_object == ctx->deferred_effects[group_start].p_pointer_object &&
                   ctx->deferred_effects[group_end].argument_serial == ctx->deferred_effects[group_start].argument_serial)
            {
                group_end++;
            }

            struct flow_branch* _Opt p_before = ctx->p_current_flow_branch;
            const struct flow_branch* _Opt target_arms[64] = { 0 };
            int target_arm_count = 0;

            const struct flow_branch* _Opt target_origins[64] = { 0 };
            for (int i = group_start; i < group_end; i++)
            {
                target_origins[i - group_start] = ctx->deferred_effects[i].p_target_origin;
            }
            const bool apply_in_arms = group_end - group_start > 1 &&
                flow_target_origins_exclusive(target_origins, group_end - group_start);

            for (int i = group_start; i < group_end; i++)
            {
                const struct flow_deferred_pointee_effect* e = &ctx->deferred_effects[i];

                /*
                    Copied into a local: the member state of an array element read
                    through a variable index is not tracked, so testing e->pointee
                    in place would not narrow it.
                */
                const struct object* _Opt pointee = e->pointee;
                if (pointee == NULL)
                {
                    continue; /* no object to apply the effect to */
                }

                struct flow_branch* _Opt p_target_arm = NULL;
                if (apply_in_arms && p_before != NULL)
                {
                    p_target_arm = flow_branch_new_pointer_target_arm(ctx, p_before, e->p_target_origin);
                    if (p_target_arm == NULL)
                    {
                        continue; /* never this pointee on this path */
                    }
                    ctx->p_current_flow_branch = p_target_arm;
                }

                switch (e->kind)
                {
                    case FLOW_EFFECT_CLEAR:
                        flow_branch_apply_dtor_or_clear_effect(ctx->p_current_flow_branch, pointee, true, e->p_token);
                    break;
                    case FLOW_EFFECT_LIFETIME_ENDED:
                        flow_branch_set_object_lifetime_ended(ctx->p_current_flow_branch, pointee, e->p_token);
                    break;
                    case FLOW_EFFECT_ANY:
                        flow_branch_set_object_any_n(ctx->p_current_flow_branch, pointee, e->p_token, options_default_nonnull(&ctx->ctx->options));
                    break;
                    case FLOW_EFFECT_DTOR:
                        flow_branch_apply_dtor_or_clear_effect(ctx->p_current_flow_branch, pointee, false, e->p_token);
                    break;
                    default:
                    break;
                }

                if (p_target_arm != NULL && p_before != NULL)
                {
                    flow_branch_tag_pointer_target_arm(p_target_arm);
                    ctx->p_current_flow_branch = p_before;
                    target_arms[target_arm_count] = p_target_arm;
                    target_arm_count++;
                }
            }

            if (p_before != NULL && target_arm_count > 0)
            {
                flow_branch_merge_arms(p_before, target_arms, target_arm_count);
            }
            group_start = group_end;
        }
        ctx->deferred_effects_count = saved_count;

        /* Variadic tail – visit remaining arguments with no paired parameter */
        while (p_current_argument)
        {
            flow_visit_full_expression(ctx, p_current_argument->expression);
            p_current_argument = p_current_argument->next;
        }
    }
    catch
    {
    }
}

static void flow_check_dianostic_suppression(struct flow_ctx* ctx, const struct token* p_token)
{
    check_dianostic_suppression_phase(ctx->ctx, p_token, 2);
}

static const struct expression* skip_parenthesis(const struct expression* expr)
{
    /*
        expr is non-null on entry and _Assert keeps it non-null across the
        step, so no null test is needed (and testing it here would leave the
        result looking nullable to the caller).
    */
    while (expr->expression_type == EXPR_PRIMARY_PARENTHESIS)
    {
        _Assert(expr->right != NULL);
        expr = expr->right;
    }
    return expr;
}

/*
    A write through an unknown index writes some element: invalidate the
    whole array. The array is reached through the left operand's REF
    alternatives (pointer into an array: follow object.parent).
    ctx->p_subscript_being_written keeps that entry until the write.
*/
static void flow_invalidate_unknown_index_write(struct flow_ctx* ctx,
                                                const struct expression* _Opt p_dest)
{
    const struct expression* _Opt p_subscript = NULL;
    if (p_dest != NULL)
    {
        p_subscript = skip_parenthesis(p_dest);
    }

    if (p_subscript != NULL &&
            p_subscript->expression_type == EXPR_POSTFIX_ARRAY &&
            p_subscript->left != NULL &&
            p_subscript->right != NULL)
    {
        /*
            Pinned to one element: the normal assignment path updates it. Inside a
            loop body only a constant expression is pinned; a flow-derived index
            changes every iteration.
        */
        const bool index_is_pinned =
            object_has_constant_value(&p_subscript->right->object) ||
            (ctx->iteration_pass == 0 && object_has_known_value(&p_subscript->right->object));

        if (!index_is_pinned)
        {
            const bool left_is_array = type_is_array(&p_subscript->left->object.type);
            const bool nullable_enabled = options_default_nonnull(&ctx->ctx->options);

            /*
                Pass 0 counts the arrays the write can land in, pass 1 invalidates them:
                each in its own arm when there are several; past the arm limit, on every
                path.
            */
            struct flow_branch* _Opt p_before = ctx->p_current_flow_branch;
            const struct flow_branch* _Opt target_arms[16] = { 0 };
            const struct flow_branch* _Opt target_origins[16] = { 0 };
            int target_arm_count = 0;
            int target_count = 0;
            bool write_in_arms = false;

            const struct flow_key_alternatives* _Opt p_left_entry =
                flow_branch_search_up(ctx->p_current_flow_branch, &p_subscript->left->object);

            for (int pass = 0; pass < 2; pass++)
            {
                if (pass == 1)
                {
                    write_in_arms = p_before != NULL &&
                        target_count > 1 &&
                        target_count <= (int)(sizeof target_arms / sizeof target_arms[0]) &&
                        flow_target_origins_exclusive(target_origins, target_count);
                }

                for (int left_index = 0; p_left_entry != NULL && left_index < p_left_entry->alternatives.size; left_index++)
                {
                    const struct flow_alternative* p_left_alternative = p_left_entry->alternatives.data[left_index];

                    /*
                        p[i]: the REF target is the pointer; the write lands in what
                        it points to, the pointer keeps its value
                    */
                    const struct flow_key_alternatives* _Opt p_pointer_entry = NULL;
                    if (!left_is_array &&
                            p_left_alternative->value_kind == FLOW_VALUE_KIND_REF &&
                            p_left_alternative->value.p != NULL)
                    {
                        p_pointer_entry = flow_branch_search_up(p_before, p_left_alternative->value.p);
                    }

                    const int candidate_count = p_pointer_entry != NULL ? p_pointer_entry->alternatives.size : 1;
                    for (int candidate_index = 0; candidate_index < candidate_count; candidate_index++)
                    {
                        const struct flow_alternative* p_candidate =
                            p_pointer_entry != NULL ? p_pointer_entry->alternatives.data[candidate_index] : p_left_alternative;

                        const struct object* _Opt p_target = NULL;
                        if (left_is_array)
                        {
                            /* v[i]: the REF target is the array itself */
                            if (p_candidate->value_kind == FLOW_VALUE_KIND_REF)
                            {
                                p_target = p_candidate->value.p;
                            }
                        }
                        else if (p_candidate->value_kind == FLOW_VALUE_KIND_PTR && p_candidate->value.p != NULL)
                        {
                            /*
                                p[i], or (p + 1)[i] whose operand is a pointer value.
                                `int* p = a;` points at a[0]; p[i] can reach every element of a
                            */
                            p_target = p_candidate->value.p;
                            const struct object* _Opt p_enclosing = p_candidate->value.p->parent;
                            while (p_enclosing != NULL && type_is_array(&p_enclosing->type))
                            {
                                p_target = p_enclosing;
                                p_enclosing = p_enclosing->parent;
                            }
                        }

                        if (p_target != NULL && pass == 0)
                        {
                            if (target_count < (int)(sizeof target_origins / sizeof target_origins[0]))
                            {
                                target_origins[target_count] = p_candidate->p_origin_map;
                            }
                            target_count++;
                        }
                        else if (p_target != NULL)
                        {
                            struct flow_branch* _Opt p_target_arm = NULL;
                            if (write_in_arms && p_before != NULL)
                            {
                                p_target_arm = flow_branch_new_pointer_target_arm(ctx, p_before, p_candidate->p_origin_map);
                            }

                            if (!write_in_arms || p_target_arm != NULL)
                            {
                                if (p_target_arm != NULL)
                                {
                                    ctx->p_current_flow_branch = p_target_arm;
                                }

                                flow_branch_set_object_any_n(ctx->p_current_flow_branch,
                                                             p_target,
                                                             p_subscript->first_token,
                                                             nullable_enabled);

                                if (p_target_arm != NULL && p_before != NULL)
                                {
                                    flow_branch_tag_pointer_target_arm(p_target_arm);
                                    ctx->p_current_flow_branch = p_before;
                                    target_arms[target_arm_count] = p_target_arm;
                                    target_arm_count++;
                                }
                            }
                        }
                    }
                }
            }

            if (p_before != NULL && target_arm_count > 0)
            {
                flow_branch_merge_arms(p_before, target_arms, target_arm_count);
            }
        }

        flow_branch_remove(ctx->p_current_flow_branch, &p_subscript->left->object);
    }
}

static void flow_expression_static_debug(struct flow_ctx* ctx, const struct expression* p_expression)
{
    struct token* first_token = p_expression->first_token;
    struct token* last_token = p_expression->last_token;
    p_expression = skip_parenthesis(p_expression);
    object_static_debug(ctx, &p_expression->object, first_token, last_token);
}

static struct flow_true_false_branches flow_visit_full_expression(struct flow_ctx* ctx, const struct expression* p_expression)
{
    return flow_visit_expression(ctx, p_expression);
}

/*
    Visits an expression whose value is tested (if, loops, ?:, !, &&, ||):
    flow_narrow_map_into then checks it for warning 30.
*/
static struct flow_true_false_branches flow_visit_condition(struct flow_ctx* ctx, const struct expression* p_expression)
{
    const struct expression* _Opt p_previous = ctx->p_condition;
    ctx->p_condition = skip_parenthesis(p_expression);
    struct flow_true_false_branches pair = flow_visit_expression(ctx, p_expression);
    ctx->p_condition = p_previous;
    return pair;
}

enum known_rel_t { KNOWN_EQUAL, KNOWN_NOT_EQUAL, KNOWN_NONE };

static void narrow_by_constant(const struct flow_alternatives* src,
                               long long c,
                               bool is_equal, /* true for ==, false for != */
                               struct flow_alternatives* true_alts,
                               struct flow_alternatives* false_alts,
                               const struct flow_branch* _Opt origin,
                               const struct token* _Opt p_token)
{
    for (int i = 0; i < src->size; i++)
    {
        const struct flow_alternative* alt = src->data[i];
        if (alt->value_relation == FLOW_RELATION_UNINITIALIZED)
            continue;

        /* Extract numeric value from the alternative if possible */
        long long val = 0;
        bool is_ptr = false;
        if (alt->value_kind == FLOW_VALUE_KIND_SIGNED)
            val = alt->value.i;
        else if (alt->value_kind == FLOW_VALUE_KIND_UNSIGNED)
            val = (long long)alt->value.u;
        else if (alt->value_kind == FLOW_VALUE_KIND_PTR)
        {
            val = (uintptr_t)alt->value.p;
            is_ptr = true;
        }
        else if (alt->value_kind == FLOW_VALUE_KIND_REF)
        {
            /* REF should have been resolved before calling this function */
            continue;
        }
        else
        {
            continue;
        }

        if (alt->value_relation == FLOW_RELATION_EQUAL)
        {
            bool result = is_equal ? (val == c) : (val != c);
            if (result)
            {
                struct flow_alternative tagged = *alt;
                tagged.p_origin_map = origin;
                tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
                tagged.p_origin_token = p_token;
                flow_alternatives_add(true_alts, &tagged);
            }
            else
            {
                struct flow_alternative tagged = *alt;
                tagged.p_origin_map = origin;
                tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
                tagged.p_origin_token = p_token;
                flow_alternatives_add(false_alts, &tagged);
            }
        }
        else if (alt->value_relation == FLOW_RELATION_NOT_EQUAL)
        {
            if (val == c)
            {
                /* Already known to be NOT_EQUAL c */
                if (is_equal)
                {
                    /* == c is false, keep NOT_EQUAL c in false branch only */
                    struct flow_alternative tagged = *alt;
                    tagged.p_origin_map = origin;
                    tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
                    tagged.p_origin_token = p_token;
                    flow_alternatives_add(false_alts, &tagged);
                }
                else
                {
                    /* != c is true, keep NOT_EQUAL c in true branch only */
                    struct flow_alternative tagged = *alt;
                    tagged.p_origin_map = origin;
                    tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
                    tagged.p_origin_token = p_token;
                    flow_alternatives_add(true_alts, &tagged);
                }
            }
            else
            {
                /* val != c */
                if (is_equal)
                {
                    /* For true branch of ==, we must enforce x == c, overriding the NOT_EQUAL val */
                    struct flow_alternative v = { 0 };
                    if (is_ptr)
                    {
                        v.value_kind = FLOW_VALUE_KIND_PTR;
                        v.value.p = (const struct object*)(uintptr_t)c;
                    }
                    else
                    {
                        v.value_kind = FLOW_VALUE_KIND_SIGNED;
                        v.value.i = c;
                    }
                    struct flow_alternative a_eq =
                    {
                        .value_kind = v.value_kind,
                        .value = v.value,
                        .value_relation = FLOW_RELATION_EQUAL,
                        .imaginary = alt->imaginary,
                        .p_origin_map = origin,
                        .p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin),
                        .p_origin_token = p_token
                    };
                    flow_alternatives_add(true_alts, &a_eq);
                    /* For false branch, keep the original NOT_EQUAL val (we lose the info x != c) */
                    struct flow_alternative tagged = *alt;
                    tagged.p_origin_map = origin;
                    tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
                    tagged.p_origin_token = p_token;
                    flow_alternatives_add(false_alts, &tagged);
                }
                else
                {
                    /*
                        For !=, the condition x != c is not determined by x != val when val != c,
                        so we keep NOT_EQUAL val in both branches
                    */
                    {
                        struct flow_alternative tagged = *alt;
                        tagged.p_origin_map = origin;
                        tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
                        tagged.p_origin_token = p_token;
                        flow_alternatives_add(true_alts, &tagged);
                    }
                    {
                        struct flow_alternative tagged = *alt;
                        tagged.p_origin_map = origin;
                        tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
                        tagged.p_origin_token = p_token;
                        flow_alternatives_add(false_alts, &tagged);
                    }
                }
            }
        }
        else if (alt->value_relation == FLOW_RELATION_ANY)
        {
            /* Unknown value – refine to EQUAL c for true, NOT_EQUAL c for false */
            struct flow_alternative v = { 0 };
            if (is_ptr)
            {
                v.value_kind = FLOW_VALUE_KIND_PTR;
                v.value.p = (const struct object*)(uintptr_t)c;
            }
            else
            {
                v.value_kind = FLOW_VALUE_KIND_SIGNED;
                v.value.i = c;
            }
            if (is_equal)
            {
                struct flow_alternative a_eq =
                {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = origin,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin),
                    .p_origin_token = p_token
                };
                struct flow_alternative a_ne =
                {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = origin,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin),
                    .p_origin_token = p_token
                };
                flow_alternatives_add(true_alts, &a_eq);
                flow_alternatives_add(false_alts, &a_ne);
            }
            else
            {
                struct flow_alternative a_ne =
                {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = origin,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin),
                    .p_origin_token = p_token
                };
                struct flow_alternative a_eq =
                {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = origin,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin),
                    .p_origin_token = p_token
                };
                flow_alternatives_add(true_alts, &a_ne);
                flow_alternatives_add(false_alts, &a_eq);
            }
        }
        else
        {
            /*
                Half-line constraint meets ==/!=: the equality branch pins the value,
                the other keeps the range (`x > 0 && x == 5`).
            */
            struct flow_alternative a_eq =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED,
                .value = {.i = c},
                .value_relation = FLOW_RELATION_EQUAL,
                .imaginary = alt->imaginary,
                .p_origin_map = origin,
                .p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin),
                .p_origin_token = p_token
            };
            struct flow_alternative a_range = *alt;
            a_range.p_origin_map = origin;
            a_range.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
            a_range.p_origin_token = p_token;
            if (is_equal)
            {
                flow_alternatives_add(true_alts, &a_eq);
                flow_alternatives_add(false_alts, &a_range);
            }
            else
            {
                flow_alternatives_add(true_alts, &a_range);
                flow_alternatives_add(false_alts, &a_eq);
            }
        }
    }
}

/*
    Map a relational operator (variable on the LEFT: `var OP c`) to the
    flow_relation recorded for the variable on the requested branch.
*/
static enum flow_relation flow_relation_for_op(enum expression_type op, bool true_branch)
{
    switch (op)
    {
        case EXPR_RELATIONAL_BIGGER_THAN:
            return true_branch ? FLOW_RELATION_GREATER : FLOW_RELATION_LESS_EQUAL;
        case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
            return true_branch ? FLOW_RELATION_GREATER_EQUAL : FLOW_RELATION_LESS;
        case EXPR_RELATIONAL_LESS_THAN:
            return true_branch ? FLOW_RELATION_LESS : FLOW_RELATION_GREATER_EQUAL;
        case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
            return true_branch ? FLOW_RELATION_LESS_EQUAL : FLOW_RELATION_GREATER;
        default:
            return FLOW_RELATION_ANY;
    }
}

/* `c OP var` is equivalent to `var swapped(OP) c`. */
static enum expression_type flow_swap_relational(enum expression_type op)
{
    switch (op)
    {
        case EXPR_RELATIONAL_BIGGER_THAN:
            return EXPR_RELATIONAL_LESS_THAN;
        case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
            return EXPR_RELATIONAL_LESS_OR_EQUAL_THAN;
        case EXPR_RELATIONAL_LESS_THAN:
            return EXPR_RELATIONAL_BIGGER_THAN;
        case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
            return EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN;
        default:
            break;
    }
    return op;
}

/*
    Narrow `src` for `var OP c` into true/false alternatives: EQUAL values
    go to the branch they satisfy, ANY becomes a half-line, the rest goes
    to both branches.
*/
static void narrow_by_relational(const struct flow_alternatives* src,
                                 long long c,
                                 enum expression_type op, /* var OP c */
                                 struct flow_alternatives* true_alts,
                                 struct flow_alternatives* false_alts,
                                 const struct flow_branch* _Opt origin,
                                 const struct token* _Opt p_token)
{
    for (int i = 0; i < src->size; i++)
    {
        const struct flow_alternative* alt = src->data[i];
        if (alt->value_relation == FLOW_RELATION_UNINITIALIZED)
            continue;

        long long lo = 0, hi = 0;
        if (alt->value_relation == FLOW_RELATION_EQUAL &&
                flow_alt_to_interval(alt, &lo, &hi))
        {
            /* Concrete value: route to whichever branch it satisfies. */
            bool t = flow_scalar_relation_holds(lo, op, c);
            struct flow_alternative tagged = *alt;
            tagged.p_origin_map = origin;
            tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
            tagged.p_origin_token = p_token;
            flow_alternatives_add(t ? true_alts : false_alts, &tagged);
            continue;
        }

        if (alt->value_relation == FLOW_RELATION_ANY &&
                (alt->value_kind == FLOW_VALUE_KIND_SIGNED ||
                    alt->value_kind == FLOW_VALUE_KIND_UNSIGNED) &&
                alt->imaginary == FLOW_IMAGINARY_NONE)
        {
            struct flow_alternative a_true =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED,
                .value = {.i = c},
                .value_relation = flow_relation_for_op(op, true),
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = origin,
                .p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin),
                .p_origin_token = p_token
            };
            struct flow_alternative a_false =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED,
                .value = {.i = c},
                .value_relation = flow_relation_for_op(op, false),
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = origin,
                .p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin),
                .p_origin_token = p_token
            };
            flow_alternatives_add(true_alts, &a_true);
            flow_alternatives_add(false_alts, &a_false);
            continue;
        }

        /*
            An existing half-line constraint is intersected with a later,
            different relational condition's range, not copied unchanged into
            both branches (narrow-half-line-against-relational.c). Non-numeric
            cases fall through unchanged.
        */
        long long alt_lo = 0, alt_hi = 0;
        if (alt->imaginary == FLOW_IMAGINARY_NONE && flow_alt_to_interval(alt, &alt_lo, &alt_hi))
        {
            struct flow_alternative cond_true =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = c},
                .value_relation = flow_relation_for_op(op, true)
            };
            struct flow_alternative cond_false =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = c},
                .value_relation = flow_relation_for_op(op, false)
            };
            long long t_lo = 0, t_hi = 0, f_lo = 0, f_hi = 0;
            bool have_t = flow_alt_to_interval(&cond_true, &t_lo, &t_hi);
            bool have_f = flow_alt_to_interval(&cond_false, &f_lo, &f_hi);

            if (have_t && alt_lo <= t_hi && t_lo <= alt_hi)
            {
                struct flow_alternative tagged = *alt;
                tagged.p_origin_map = origin;
                tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
                tagged.p_origin_token = p_token;
                /*
                    Clip to the intersection -- overlapping is not enough. An
                    alternative already known `<= 0` narrowed by `< 0` must
                    become `< 0` on the true branch; keeping it at `<= 0` left
                    zero live and reported a false division by zero.
                */
                flow_alt_set_interval(&tagged,
                                      alt_lo > t_lo ? alt_lo : t_lo,
                                      alt_hi < t_hi ? alt_hi : t_hi);
                flow_alternatives_add(true_alts, &tagged);
            }
            if (have_f && alt_lo <= f_hi && f_lo <= alt_hi)
            {
                struct flow_alternative tagged = *alt;
                tagged.p_origin_map = origin;
                tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
                tagged.p_origin_token = p_token;
                flow_alt_set_interval(&tagged,
                                      alt_lo > f_lo ? alt_lo : f_lo,
                                      alt_hi < f_hi ? alt_hi : f_hi);
                flow_alternatives_add(false_alts, &tagged);
            }
            continue;
        }

        /*
            NOT_EQUAL, pointers, or MOVED/ENDED state: keep unchanged in both
            branches (conservative fallback).
        */
        struct flow_alternative tagged = *alt;
        tagged.p_origin_map = origin;
        tagged.p_narrowed_from = flow_alternative_narrowed_provenance(alt, origin);
        tagged.p_origin_token = p_token;
        flow_alternatives_add(true_alts, &tagged);
        flow_alternatives_add(false_alts, &tagged);
    }
}

static bool flow_operand_is_single_constant(struct flow_ctx* ctx,
                                            const struct expression* p_expr,
                                            long long* out)
{
    const struct flow_key_alternatives* _Opt entry = flow_branch_search_up(ctx->p_current_flow_branch, &p_expr->object);
    if (entry == NULL)
        return false;

    bool found = false;
    long long val = 0;

    for (int i = 0; i < entry->alternatives.size; i++)
    {
        const struct flow_alternative* alt = entry->alternatives.data[i];
        bool is_ref = (alt->value_kind == FLOW_VALUE_KIND_REF && alt->value.p != NULL);
        const struct flow_key_alternatives* _Opt resolved = entry;
        if (is_ref)
        {
            resolved = flow_branch_search_up(ctx->p_current_flow_branch, alt->value.p);
            if (resolved == NULL) return false;
        }
        int count = is_ref ? resolved->alternatives.size : 1;
        for (int j = 0; j < count; j++)
        {
            const struct flow_alternative* v = is_ref ? resolved->alternatives.data[j] : alt;
            if (v->value_relation != FLOW_RELATION_EQUAL)
                return false;
            long long c = 0;
            if (v->value_kind == FLOW_VALUE_KIND_SIGNED) c = v->value.i;
            else if (v->value_kind == FLOW_VALUE_KIND_UNSIGNED) c = (long long)v->value.u;
            else if (v->value_kind == FLOW_VALUE_KIND_PTR) c = (long long)(uintptr_t)v->value.p;
            else return false;
            if (found && c != val)
                return false; /* more than one distinct value */
            val = c;
            found = true;
        }
    }

    if (!found)
        return false;
    *out = val;
    return true;
}

/*
    Narrow the variable operand `p_expr` for `var OP c` (relational, == or !=),
    writing refined alternatives into p_true/p_false. Per-alternative REF
    pattern: for every REF alternative of the operand, narrow the object it
    references.
*/
static void flow_narrow_operand(struct flow_ctx* ctx,
                                const struct expression* p_expr,
                                long long c,
                                enum expression_type op,
                                struct flow_branch* p_true,
                                struct flow_branch* p_false,
                                const struct token* _Opt p_token)
{
    const struct flow_key_alternatives* _Opt entry = flow_branch_search_up(ctx->p_current_flow_branch, &p_expr->object);
    if (entry == NULL)
        return;

    for (int i = 0; i < entry->alternatives.size; i++)
    {
        const struct flow_alternative* alt = entry->alternatives.data[i];
        if (alt->value_kind != FLOW_VALUE_KIND_REF || alt->value.p == NULL)
            continue;

        const struct object* obj = alt->value.p;
        const struct flow_key_alternatives* _Opt obj_entry =
            flow_branch_search_up(ctx->p_current_flow_branch, obj);
        if (obj_entry == NULL)
            continue;

        struct flow_alternatives true_alts = { 0 }, false_alts = { 0 };
        if (op == EXPR_EQUALITY_EQUAL || op == EXPR_EQUALITY_NOT_EQUAL)
            narrow_by_constant(&obj_entry->alternatives, c, op == EXPR_EQUALITY_EQUAL,
                               &true_alts, &false_alts, p_true, p_token);
        else
            narrow_by_relational(&obj_entry->alternatives, c, op,
                                 &true_alts, &false_alts, p_true, p_token);
        /*
            Tag each branch's alternatives with ITS OWN map so join
            correlation can tell them apart (true values belong to p_true,
            false values to p_false -- not both to p_true).
        */
        for (int k = 0; k < true_alts.size; k++) true_alts.data[k]->p_origin_map = p_true;
        for (int k = 0; k < false_alts.size; k++) false_alts.data[k]->p_origin_map = p_false;
        if (true_alts.size > 0)
        {
            struct flow_key_alternatives* _Opt e = flow_branch_find_add(p_true, obj);
            if (e != NULL)
            {
                flow_alternatives_clear(&e->alternatives);
                e->alternatives = true_alts;
            }
            else
            {
                /* nowhere to move them: release rather than leak */
                flow_alternatives_clear(&true_alts);
            }
        }
        else
        {
            /*
                the narrowing helper may have reserved capacity without adding
                any alternative; that buffer is ours to release.
            */
            flow_alternatives_clear(&true_alts);
        }

        if (false_alts.size > 0)
        {
            struct flow_key_alternatives* _Opt e = flow_branch_find_add(p_false, obj);
            if (e != NULL)
            {
                flow_alternatives_clear(&e->alternatives);
                e->alternatives = false_alts;
            }
            else
            {
                /* nowhere to move them: release rather than leak */
                flow_alternatives_clear(&false_alts);
            }
        }
        else
        {
            /*
                the narrowing helper may have reserved capacity without adding
                any alternative; that buffer is ours to release.
            */
            flow_alternatives_clear(&false_alts);
        }
    }
}

/*
    Decide `alt <op> c` for ONE alternative: 1 (holds), 0 (does not hold),
    -1 (undecidable). Split out of the loop below so a single alternative can
    be evaluated directly, without wrapping it in a one-element list.
*/
static int flow_evaluate_alternative_against_constant(const struct flow_alternative* alt,
                                                      long long c,
                                                      bool is_equal)
{
    bool result = false, known = false;

    if (alt->value_kind == FLOW_VALUE_KIND_PTR)
    {
        if (c == 0)
        {
            if (alt->value_relation == FLOW_RELATION_EQUAL)
            {
                result = is_equal ? (alt->value.p == NULL) : (alt->value.p != NULL);
                known = true;
            }
            else if (alt->value_relation == FLOW_RELATION_NOT_EQUAL)
            {
                /*
                    A NOT_EQUAL pointer alternative is non-null (whether it
                    records `!= null` directly or a concrete non-null target,
                    e.g. the result of array-decay pointer arithmetic) --
                    consistent with flow_alternative_can_be_zero.
                */
                result = is_equal ? false : true;
                known = true;
            }
        }
    }
    else if (alt->value_kind == FLOW_VALUE_KIND_SIGNED || alt->value_kind == FLOW_VALUE_KIND_UNSIGNED)
    {
        long long val = (alt->value_kind == FLOW_VALUE_KIND_SIGNED) ? alt->value.i : (long long)alt->value.u;
        if (alt->value_relation == FLOW_RELATION_EQUAL)
        {
            result = is_equal ? (val == c) : (val != c);
            known = true;
        }
        else if (alt->value_relation == FLOW_RELATION_NOT_EQUAL && val == c)
        {
            result = is_equal ? false : true;
            known = true;
        }
    }
    
    if (!known) return -1;

    return result ? 1 : 0;
}

/*
    Which side of the branch pair branch_id the chain from `chain` to the root is
    on, through a map's own kind or its implied facts. FLOW_BRANCH_ROOT when the
    chain says nothing about that pair.
*/
static enum flow_branch_kind flow_chain_side_of(const struct flow_branch* _Opt chain, int branch_id)
{
    enum flow_branch_kind side = FLOW_BRANCH_ROOT;
    for (const struct flow_branch* _Opt m = chain; m != NULL && side == FLOW_BRANCH_ROOT; m = m->p_parent_map)
    {
        if (m->branch_id == branch_id)
        {
            side = m->kind;
        }
        for (int fact_index = 0; fact_index < m->implied_facts.count && side == FLOW_BRANCH_ROOT; fact_index++)
        {
            if (m->implied_facts.data[fact_index].branch_id == branch_id)
            {
                side = m->implied_facts.data[fact_index].kind;
            }
        }
    }
    return side;
}

static bool flow_origins_compatible(const struct flow_branch* _Opt o1, const struct flow_branch* _Opt o2)
{
    bool compatible = true;
    if (o1 != NULL && o2 != NULL && o1 != o2)
    {
        for (const struct flow_branch* _Opt a = o1; a != NULL && compatible; a = a->p_parent_map)
        {
            /* index -1 is the map's own side, the others its implied facts */
            for (int fact_index = -1; fact_index < a->implied_facts.count && compatible; fact_index++)
            {
                const int branch_id = fact_index < 0 ? a->branch_id : a->implied_facts.data[fact_index].branch_id;
                const enum flow_branch_kind kind = fact_index < 0 ? a->kind : a->implied_facts.data[fact_index].kind;
                if (branch_id != 0)
                {
                    const enum flow_branch_kind other_side = flow_chain_side_of(o2, branch_id);
                    if (other_side != FLOW_BRANCH_ROOT && other_side != kind)
                    {
                        compatible = false;
                    }
                }
            }
        }
    }
    return compatible;
}

/*
    After a condition narrowed its arm, record the earlier branch sides that
    every kept value comes from. In `if (c > 2) p = &i; if (c > 2) *p;` the
    second true arm is on the first true side. A write in between ends the
    correlation. A side is implied only when the object had a value from the
    other side before and the condition dropped all of them.
*/
static void flow_branch_derive_implied_facts(struct flow_branch* p_arm, struct flow_branch* p_before)
{
    struct flow_branch_entries it = flow_branch_entries_begin(p_arm, p_before);
    for (const struct flow_key_alternatives* _Opt p_entry; (p_entry = flow_branch_entries_next(&it)) != NULL;)
    {
        /* a contradicted value exists on no path (see flow_alternative) */
        int first_index = 0;
        while (first_index < p_entry->alternatives.size &&
               p_entry->alternatives.data[first_index]->contradicted)
        {
            first_index++;
        }

        /*
            A value exists only on paths through its origin (and its narrowed-from
            map). Index 0 walks the origin chain, index 1 the narrowed-from chain.
        */
        for (int chain_index = 0; first_index < p_entry->alternatives.size && chain_index < 2; chain_index++)
        {
            const struct flow_alternative* p_first = p_entry->alternatives.data[first_index];
            const struct flow_branch* _Opt p_chain = chain_index == 0 ? p_first->p_origin_map : p_first->p_narrowed_from;
            for (const struct flow_branch* _Opt p_origin = p_chain; p_origin != NULL; p_origin = p_origin->p_parent_map)
            {
                /* index -1 is the map's own side, the others its implied facts */
                for (int fact_index = -1; fact_index < p_origin->implied_facts.count; fact_index++)
                {
                    const int branch_id = fact_index < 0 ? p_origin->branch_id : p_origin->implied_facts.data[fact_index].branch_id;
                    const enum flow_branch_kind kind = fact_index < 0 ? p_origin->kind : p_origin->implied_facts.data[fact_index].kind;

                    bool every_value_from_side = branch_id != 0;
                    for (int alternative_index = 0; every_value_from_side && alternative_index < p_entry->alternatives.size; alternative_index++)
                    {
                        const struct flow_alternative* p_alternative = p_entry->alternatives.data[alternative_index];
                        if (p_alternative->contradicted)
                        {
                            continue;
                        }
                        enum flow_branch_kind side = flow_chain_side_of(p_alternative->p_origin_map, branch_id);
                        if (side == FLOW_BRANCH_ROOT)
                        {
                            side = flow_chain_side_of(p_alternative->p_narrowed_from, branch_id);
                        }
                        if (side != kind)
                        {
                            every_value_from_side = false;
                        }
                    }

                    /* the condition must have dropped values from the other side */
                    bool other_side_dropped = false;
                    const struct flow_key_alternatives* _Opt p_before_entry =
                        every_value_from_side ? flow_branch_search_up(p_before, p_entry->p_obj_key) : NULL;
                    for (int before_index = 0; p_before_entry != NULL && !other_side_dropped && before_index < p_before_entry->alternatives.size; before_index++)
                    {
                        const struct flow_alternative* p_before_alternative = p_before_entry->alternatives.data[before_index];
                        if (p_before_alternative->contradicted)
                        {
                            continue;
                        }
                        enum flow_branch_kind side = flow_chain_side_of(p_before_alternative->p_origin_map, branch_id);
                        if (side == FLOW_BRANCH_ROOT)
                        {
                            side = flow_chain_side_of(p_before_alternative->p_narrowed_from, branch_id);
                        }
                        if (side != FLOW_BRANCH_ROOT && side != kind)
                        {
                            other_side_dropped = true;
                        }
                    }

                    if (every_value_from_side &&
                            other_side_dropped &&
                            flow_chain_side_of(p_arm, branch_id) == FLOW_BRANCH_ROOT &&
                            p_arm->implied_facts.count < (int)(sizeof p_arm->implied_facts.data / sizeof p_arm->implied_facts.data[0]))
                    {
                        p_arm->implied_facts.data[p_arm->implied_facts.count].branch_id = branch_id;
                        p_arm->implied_facts.data[p_arm->implied_facts.count].kind = kind;
                        p_arm->implied_facts.count++;
                    }
                }
            }
        }
    }
}

/*
    Write through a pointer with several targets (`p = c ? &a : &b; *p = 10;`):
    each target is written in its own arm, made from the origin of the value
    that gave that target. NULL when that origin cannot happen here.
*/
static struct flow_branch* _Opt flow_branch_new_pointer_target_arm(struct flow_ctx* ctx, struct flow_branch* p_before, const struct flow_branch* _Opt p_target_origin)
{
    struct flow_branch* _Opt p_arm = NULL;
    if (flow_origins_compatible(p_target_origin, p_before))
    {
        p_arm = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_POINTER_TARGET);
    }

    for (const struct flow_branch* _Opt p_origin = p_target_origin; p_arm != NULL && p_origin != NULL; p_origin = p_origin->p_parent_map)
    {
        /* index -1 is the map's own side, the others its implied facts */
        for (int fact_index = -1; fact_index < p_origin->implied_facts.count; fact_index++)
        {
            const int branch_id = fact_index < 0 ? p_origin->branch_id : p_origin->implied_facts.data[fact_index].branch_id;
            const enum flow_branch_kind kind = fact_index < 0 ? p_origin->kind : p_origin->implied_facts.data[fact_index].kind;
            if (branch_id != 0 &&
                    flow_chain_side_of(p_arm, branch_id) == FLOW_BRANCH_ROOT &&
                    p_arm->implied_facts.count < (int)(sizeof p_arm->implied_facts.data / sizeof p_arm->implied_facts.data[0]))
            {
                p_arm->implied_facts.data[p_arm->implied_facts.count].branch_id = branch_id;
                p_arm->implied_facts.data[p_arm->implied_facts.count].kind = kind;
                p_arm->implied_facts.count++;
            }
        }
    }
    return p_arm;
}

/*
    Split a write into target arms only when every two targets come from
    opposite sides of a branch; otherwise (loop-carried pointer) overwrite
    each target.
*/
static bool flow_target_origins_exclusive(const struct flow_branch* _Opt const target_origins[], int target_count)
{
    bool exclusive = true;
    for (int i = 0; i < target_count && exclusive; i++)
    {
        for (int j = i + 1; j < target_count && exclusive; j++)
        {
            if (flow_origins_compatible(target_origins[i], target_origins[j]))
            {
                exclusive = false;
            }
        }
    }
    return exclusive;
}

static void flow_branch_tag_pointer_target_arm(const struct flow_branch* p_arm)
{
    struct flow_branch_entries it = flow_branch_entries_begin(p_arm, p_arm->p_parent_map);
    for (const struct flow_key_alternatives* _Opt p_entry; (p_entry = flow_branch_entries_next(&it)) != NULL;)
    {
        for (int alternative_index = 0; alternative_index < p_entry->alternatives.size; alternative_index++)
        {
            struct flow_alternative* p_alternative = p_entry->alternatives.data[alternative_index];
            bool origin_on_chain = p_alternative->p_origin_map == NULL;
            for (const struct flow_branch* _Opt m = p_arm; m != NULL && !origin_on_chain; m = m->p_parent_map)
            {
                if (m == p_alternative->p_origin_map)
                {
                    origin_on_chain = true;
                }
            }

            if (origin_on_chain)
            {
                p_alternative->p_origin_map = p_arm;
            }
            else if (p_alternative->p_narrowed_from == NULL)
            {
                p_alternative->p_narrowed_from = p_arm;
            }
        }
    }
}

static const struct flow_branch* _Opt flow_origin_more_specific(const struct flow_branch* _Opt o1,
                                                             const struct flow_branch* _Opt o2)
{
    if (o1 == NULL) return o2;
    if (o2 == NULL) return o1;
    if (o1 == o2) return o1;
    if (flow_branch_is_ancestor_or_self(o1, o2)) return o2; /* o1 is ancestor -> o2 deeper */
    return o1; /* o2 is ancestor -> o1 deeper */
}

static bool flow_alt_concrete_int(const struct flow_alternative* alt, long long* out)
{

    if (alt->value_relation != FLOW_RELATION_EQUAL) return false;
    switch (alt->value_kind)
    {
        case FLOW_VALUE_KIND_SIGNED:
            *out = alt->value.i;
            return true;
        case FLOW_VALUE_KIND_UNSIGNED:
            *out = (long long)alt->value.u;
            return true;
        case FLOW_VALUE_KIND_PTR:
            *out = (long long)(uintptr_t)alt->value.p;
            return true;
        default:
            break;
    }
    return false;
}

static int flow_pair_equality(const struct flow_alternative* lval,
                              const struct flow_alternative* rval,
                              bool is_equal)
{
    long long lc = 0, rc = 0;
    bool lk = flow_alt_concrete_int(lval, &lc);
    bool rk = flow_alt_concrete_int(rval, &rc);

    /*
        Two non-null pointers to DIFFERENT objects may still alias: not
        decidable. Only NULL vs non-null and same-object are. REF values keep
        the exact comparison below.
    */
    if (lval->value_relation == FLOW_RELATION_EQUAL && lval->value_kind == FLOW_VALUE_KIND_PTR && lval->value.p != NULL &&
            rval->value_relation == FLOW_RELATION_EQUAL && rval->value_kind == FLOW_VALUE_KIND_PTR && rval->value.p != NULL &&
            lval->value.p != rval->value.p)
    {
        return -1; /* different non-null pointers: may or may not alias */
    }

    if (lk && rk)
    {
        bool eq = (lc == rc);
        return (is_equal ? eq : !eq) ? 1 : 0;
    }
    if (rk)
    {
        return flow_evaluate_alternative_against_constant(lval, rc, is_equal);
    }
    if (lk)
    {
        return flow_evaluate_alternative_against_constant(rval, lc, is_equal);
    }
    return -1;
}

/*
    Warning 30 for one value of an operand (comparison or arithmetic), recorded
    in the caller's findings scope: every uninitialized value is counted, and the
    operand is reported once.
*/
static void flow_record_uninitialized_operand_value(struct flow_ctx* ctx,
                                                    const struct expression* p_operand,
                                                    const struct flow_alternative* val,
                                                    const struct marker* p_marker)
{
    if (ctx->expression_is_not_evaluated ||
        val->imaginary == FLOW_IMAGINARY_ENDED ||
        val->value_relation != FLOW_RELATION_UNINITIALIZED ||
        type_is_uninit(&p_operand->object.type) ||
        /*
            set on a branch this path did not take (`if (c) { delete(p->x); p = 0; }
            if (p) p->x ...`): the same check the null dereference applies
        */
        !flow_origins_compatible(val->p_origin_map, ctx->p_current_flow_branch))
    {
        return;
    }

    struct osstream name_ss = { 0 };
    flow_expression_to_string(p_operand, &name_ss);
    struct osstream ss = { 0 };
    ss_fprintf(&ss, "using a possibly uninitialized object '%s'", name_ss.c_str ? name_ss.c_str : "");
    flow_finding_record(ctx, &p_operand->object, W_FLOW_UNINITIALIZED, p_marker,
                        val, val->p_origin_map, ss.c_str ? ss.c_str : "");
    ss_close(&ss);
    ss_close(&name_ss);
}

static void flow_narrow_map_into(struct flow_ctx* ctx, const struct expression* _Opt p_read, struct flow_branch* p_dest, struct flow_branch* _Opt p_before, const struct object* p_obj_key, bool true_branch, const struct token* _Opt p_token)
{
    struct flow_key_alternatives* _Opt p_existing = flow_branch_search_up(p_before, p_obj_key);
    if (p_existing == NULL || p_existing->alternatives.size == 0)
    {
        /*
            No known value here (untracked or empty entry): narrow like ANY --
            true arm nonzero, false arm exactly 0. Only the tested object's own
            fact; no ancestor alternatives are pulled in.
        */
        struct flow_key_alternatives* _Opt p_dest_entry0 = flow_branch_find_add(p_dest, p_obj_key);
        if (p_dest_entry0 != NULL)
        {
            flow_alternatives_clear(&p_dest_entry0->alternatives);
            struct flow_alternative a =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED,
                .value = {.i = 0},
                .value_relation = true_branch ? FLOW_RELATION_NOT_EQUAL : FLOW_RELATION_EQUAL,
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = p_dest,
                .p_origin_token = NULL
            };
            flow_alternatives_add(&p_dest_entry0->alternatives, &a);
        }
        return;
    }

    /*
        Only p_obj_key is narrowed. Shared origin ancestry is not a correlation
        between variables, so `if (a) b = 1; if (a) static_debug(b);` does not
        narrow b. Other objects get per-branch values from
        flow_branch_merge_arms's origin re-tagging.
    */

    /* Write the narrowed variable itself into p_dest. */
    struct flow_key_alternatives* _Opt p_dest_entry = flow_branch_find_add(p_dest, p_obj_key);
    if (p_dest_entry == NULL) return;

    flow_alternatives_clear(&p_dest_entry->alternatives);

    /* p_read: the tested expression is a condition, so its value is read */
    const struct marker read_marker = p_read != NULL ? expression_to_marker(p_read) : (struct marker) { 0 };
    if (p_read != NULL)
        flow_findings_begin(ctx);

    for (int k = 0; k < p_existing->alternatives.size; k++)
    {
        struct flow_alternative* alt = p_existing->alternatives.data[k];

        if (alt->value_relation == FLOW_RELATION_ANY)
        {
            if (true_branch)
            {
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = p_dest,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, p_dest),
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            else
            {
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = p_dest,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, p_dest),
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            continue;
        }

        if (alt->value_relation == FLOW_RELATION_NOT_EQUAL && flow_value_is_false(alt))
        {
            if (true_branch)
            {
                /*
                    Keeps the value kind: a pointer "!= null" stays a pointer, so a
                    pointee can still be made for it on demand at `->`.
                */
                struct flow_alternative a =
                {
                    .value_kind = alt->value_kind,
                    .value = alt->value,
                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = p_dest,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, p_dest),
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            else
            {
                /* "!= 0" in the false arm: no path gets here with it */
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = p_dest,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, p_dest),
                    .contradicted = true,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            continue;
        }

        bool keep = false;
        if (alt->value_relation == FLOW_RELATION_EQUAL)
        {
            if (flow_value_is_false(alt) && !true_branch)
            {
                keep = true;
            }
            if (flow_value_is_true(alt) && true_branch)
            {
                keep = true;
            }
            if (flow_value_is_true(alt) && !true_branch)
            {
                /*
                    "== X" (X != 0) on the false arm of a test on the object: the object
                    is 0 here. Record 0; an empty entry would fall through to the ancestor's
                    "== X".
                */
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = p_dest,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, p_dest),
                    .contradicted = true,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&p_dest_entry->alternatives, &a);
                continue;
            }
        }
        else if (alt->value_relation == FLOW_RELATION_UNINITIALIZED)
        {
            keep = false;
            if (p_read != NULL)
                flow_record_uninitialized_operand_value(ctx, p_read, alt, &read_marker);
        }
        else if (alt->value_relation == FLOW_RELATION_NOT_EQUAL)
        {
            /*
                alt->value is a nonzero X ("!= 0" was handled above). True arm: keep
                "!= X". False arm: the value is exactly 0, recorded as 0 so
                flow_alternative_is_zero sees it.
            */
            if (true_branch)
            {
                struct flow_alternative a =
                {
                    .value_kind = alt->value_kind,
                    .value = alt->value,
                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = p_dest,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, p_dest),
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            else
            {
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = 0},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = p_dest,
                    .p_narrowed_from = flow_alternative_narrowed_provenance(alt, p_dest),
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&p_dest_entry->alternatives, &a);
            }
            continue;
        }
        else
        {
            if (true_branch)
            {
                keep = true;
            }
        }

        if (keep)
        {
            flow_alternatives_add(&p_dest_entry->alternatives, alt);
        }
    }

    if (p_read != NULL)
        flow_findings_end(ctx);

    if (p_dest_entry->alternatives.size == 0)
    {
        for (int k = 0; k < p_existing->alternatives.size; k++)
        {
            if (p_existing->alternatives.data[k]->value_relation == FLOW_RELATION_UNINITIALIZED)
            {
                flow_alternatives_add(&p_dest_entry->alternatives, p_existing->alternatives.data[k]);
            }
        }
    }

}

static struct flow_branch* _Opt flow_narrow_map_branch(struct flow_ctx* ctx, const struct expression* _Opt p_read, struct flow_branch_arena* arena, struct flow_branch* _Opt p_before,
                                                    const struct object* p_obj_key, bool true_branch, const struct expression* _Opt p_expr, const struct token* _Opt p_token)
{
    struct flow_branch* _Opt p_dest = flow_branch_arena_new_branch(arena, p_before, true_branch, p_expr);
    if (p_dest == NULL)
        return NULL;

    flow_narrow_map_into(ctx, p_read, p_dest, p_before, p_obj_key, true_branch, p_token);
    return p_dest;
}

const char* obj_display(const struct object* _Opt obj)
{
    if (obj && obj->member_designator && obj->member_designator[0])
        return obj->member_designator;
    static char buf[32] = { 0 };
    snprintf(buf, sizeof(buf), "0x%lx", (unsigned long)(uintptr_t)obj);
    return buf;
}

void print_value(const struct flow_alternative* alt)
{

    switch (alt->value_kind)
    {
        case FLOW_VALUE_KIND_SIGNED:
            printf("%lld", alt->value.i);
        break;
        case FLOW_VALUE_KIND_UNSIGNED:
            printf("%llu", alt->value.u);
        break;
        case FLOW_VALUE_KIND_PTR:
            printf("%s", obj_display(alt->value.p));
        break;
        case FLOW_VALUE_KIND_REF:
            printf("REF->%s", obj_display(alt->value.p));
        break;
        default:
            printf("?");
    }
}

static int flow_interval_relational(long long llo, long long lhi,
                                    long long rlo, long long rhi,
                                    enum expression_type op)
{
    long long worst_l = 0, worst_r = 0, best_l = 0, best_r = 0;
    switch (op)
    {
        case EXPR_RELATIONAL_BIGGER_THAN:
        case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
            /* increasing in l, decreasing in r */
            worst_l = llo;
            worst_r = rhi;
            best_l = lhi;
            best_r = rlo;
        break;
        case EXPR_RELATIONAL_LESS_THAN:
        case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
            /* decreasing in l, increasing in r */
            worst_l = lhi;
            worst_r = rlo;
            best_l = llo;
            best_r = rhi;
        break;
        default:
            return -1;
    }
    if (flow_scalar_relation_holds(worst_l, op, worst_r)) return 1;
    if (!flow_scalar_relation_holds(best_l, op, best_r)) return 0;
    return -1;
}

static int flow_pair_boolean(const struct flow_alternative* lval,
                             const struct flow_alternative* rval,
                             enum expression_type op)
{
    switch (op)
    {
        case EXPR_EQUALITY_EQUAL:
            return flow_pair_equality(lval, rval, true);
        case EXPR_EQUALITY_NOT_EQUAL:
            return flow_pair_equality(lval, rval, false);
        case EXPR_RELATIONAL_BIGGER_THAN:
        case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
        case EXPR_RELATIONAL_LESS_THAN:
        case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
        {
            long long llo = 0, lhi = 0, rlo = 0, rhi = 0;
            if (!flow_alt_to_interval(lval, &llo, &lhi) ||
            !flow_alt_to_interval(rval, &rlo, &rhi))
                return -1;
            return flow_interval_relational(llo, lhi, rlo, rhi, op);
        }
        default:
            return -1;
    }
}


/*
    The comparison's result on each path: one 0/1 alternative per compatible
    pair of operand values. False (out left empty) when some pair is undecidable.
    The same walk reports an uninitialized operand (warning 30) once.
*/
static bool flow_comparison_result_alts(struct flow_ctx* ctx,
                                        const struct expression* p_expression,
                                        struct flow_alternatives* out)
{
    _Assert(p_expression->left != NULL);
    _Assert(p_expression->right != NULL);

    if (ctx->p_current_flow_branch == NULL)
        return false;

    const struct expression* const p_left = p_expression->left;
    const struct expression* const p_right = p_expression->right;
    const enum expression_type op = p_expression->expression_type;
    const struct token* const p_token = p_expression->first_token;
    const struct marker marker = expression_to_marker(p_expression);

    const struct flow_key_alternatives* _Opt left_entry =
        flow_branch_search_up(ctx->p_current_flow_branch, &p_left->object);

    const struct flow_key_alternatives* _Opt right_entry =
        flow_branch_search_up(ctx->p_current_flow_branch, &p_right->object);

    if (left_entry == NULL || right_entry == NULL)
        return false;

    bool any = false;
    /*
        An undecidable pair makes the result unknown, but the walk goes on so
        every operand value is still checked for warning 30.
    */
    bool unknown = false;

    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow_alternative* lalt = left_entry->alternatives.data[li];
        bool l_is_ref = (lalt->value_kind == FLOW_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow_key_alternatives* _Opt left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow_branch_search_up(ctx->p_current_flow_branch, lalt->value.p);
            if (left_resolved == NULL)
            {
                flow_alternatives_clear(out);
                return false;
            }
        }
        int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow_alternative* lval = l_is_ref ? left_resolved->alternatives.data[lvi] : lalt;
            /*
                facts about the right operand alone are recorded once, with the
                first left value, or the count would repeat per left value
            */
            const bool first_left_value = (li == 0 && lvi == 0);
            flow_record_uninitialized_operand_value(ctx, p_left, lval, &marker);

            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow_alternative* ralt = right_entry->alternatives.data[ri];
                bool r_is_ref = (ralt->value_kind == FLOW_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow_key_alternatives* _Opt right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow_branch_search_up(ctx->p_current_flow_branch, ralt->value.p);
                    if (right_resolved == NULL)
                    {
                        flow_alternatives_clear(out);
                        return false;
                    }
                }
                int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow_alternative* rval = r_is_ref ? right_resolved->alternatives.data[rvi] : ralt;
                    if (first_left_value)
                        flow_record_uninitialized_operand_value(ctx, p_right, rval, &marker);
                    if (unknown)
                        continue;
                    if (!flow_origins_compatible(lval->p_origin_map, rval->p_origin_map))
                        continue;

                    int b = flow_pair_boolean(lval, rval, op);
                    if (b == -1)
                    {
                        unknown = true;
                        continue;
                    }

                    struct flow_alternative a =
                    {
                        .value_kind = FLOW_VALUE_KIND_SIGNED,
                        .value = {.i = b ? 1 : 0},
                        .value_relation = FLOW_RELATION_EQUAL,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = flow_origin_more_specific(lval->p_origin_map, rval->p_origin_map),
                        .p_origin_token = p_token
                    };
                    flow_alternatives_add(out, &a);
                    any = true;
                }
            }
        }
    }

    if (unknown || !any)
    {
        flow_alternatives_clear(out);
        return false;
    }
    return true;
}

/*
    Seed the comparison's per-path result (or ANY) and fold it: 1 when it is
    true on every path, 0 when false on every path, -1 otherwise.
*/
static int flow_seed_comparison_result(struct flow_ctx* ctx,
                                       const struct expression* p_expression)
{
    _Assert(p_expression->left != NULL);
    _Assert(p_expression->right != NULL);

    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
    if (e == NULL) return -1;

    flow_alternatives_clear(&e->alternatives);

    int fold = -1;
    struct flow_alternatives per_path = { 0 };
    flow_findings_begin(ctx);
    const bool known = flow_comparison_result_alts(ctx, p_expression, &per_path);
    flow_findings_end(ctx);
    if (known)
    {
        bool all_true = true, all_false = true;
        for (int i = 0; i < per_path.size; i++)
        {
            if (per_path.data[i]->value.i)
                all_false = false;
            else
                all_true = false;
        }
        fold = all_true ? 1 : all_false ? 0 : -1;
        e->alternatives = per_path; /* move */
    }
    else
    {
        /* Not moved into e: release whatever the helper managed to build. */
        flow_alternatives_clear(&per_path);

        struct flow_alternative a =
        {
            .value_kind = FLOW_VALUE_KIND_SIGNED,
            .value = {.i = ANY_VALUE},
            .value_relation = FLOW_RELATION_ANY,
            .imaginary = FLOW_IMAGINARY_NONE,
            .p_origin_map = ctx->p_current_flow_branch,
            .p_origin_token = p_expression->first_token
        };
        flow_alternatives_add(&e->alternatives, &a);
    }
    return fold;
}

static void flow_evaluate_binary_arithmetic(struct flow_ctx* ctx,
                                            const struct expression* p_left,
                                            const struct expression* p_right,
                                            const struct expression* p_result,
                                            char op)
{
    if (ctx->p_current_flow_branch == NULL)
        return;

    const struct flow_key_alternatives* _Opt left_entry = flow_branch_search_up(ctx->p_current_flow_branch, &p_left->object);
    const struct flow_key_alternatives* _Opt right_entry = flow_branch_search_up(ctx->p_current_flow_branch, &p_right->object);

    if (!left_entry || !right_entry)
    {
        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_result->object);
        if (e == NULL) return;

        flow_alternatives_clear(&e->alternatives);
        struct flow_alternative a =
        {
            .value_kind = FLOW_VALUE_KIND_SIGNED,
            .value = {.i = ANY_VALUE},
            .value_relation = FLOW_RELATION_ANY,
            .imaginary = FLOW_IMAGINARY_NONE,
            .p_origin_map = ctx->p_current_flow_branch,
            .p_origin_token = p_result->first_token
        };
        flow_alternatives_add(&e->alternatives, &a);
        return;
    }

    struct flow_alternatives result_alts = { 0 };
    struct marker marker = expression_to_marker(p_result);

    flow_findings_begin(ctx);

    /* ---- Outer loop over left top‑level alternatives ---- */
    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow_alternative* lalt = left_entry->alternatives.data[li];


        /* Resolve left REF if present; any other alternative is its own value */
        const bool l_is_ref = (lalt->value_kind == FLOW_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow_key_alternatives* _Opt left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow_branch_search_up(ctx->p_current_flow_branch, lalt->value.p);
            if (!left_resolved) continue;
        }
        const int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        /* ---- Loop over resolved left values ---- */
        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow_alternative* lval = l_is_ref ? left_resolved->alternatives.data[lvi] : lalt;
            /*
                facts about the right operand alone are recorded once, with the
                first left value, or the count would repeat per left value
            */
            const bool first_left_value = (li == 0 && lvi == 0);

            flow_record_uninitialized_operand_value(ctx, p_left, lval, &marker);


            /* ---- Middle loop over right top‑level alternatives ---- */
            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow_alternative* ralt = right_entry->alternatives.data[ri];


                /* Resolve right REF if present; any other alternative is its own value */
                const bool r_is_ref = (ralt->value_kind == FLOW_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow_key_alternatives* _Opt right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow_branch_search_up(ctx->p_current_flow_branch, ralt->value.p);
                    if (!right_resolved) continue;
                }
                const int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                /* ---- Loop over resolved right values ---- */
                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow_alternative* rval = r_is_ref ? right_resolved->alternatives.data[rvi] : ralt;

                    if (first_left_value)
                        flow_record_uninitialized_operand_value(ctx, p_right, rval, &marker);


                    enum flow_relation result_rel = FLOW_RELATION_ANY;
                    long long result_val = ANY_VALUE;

                    /* Warn if any divisor alternative can be zero, not only for constants. */
                    if ((op == '/' || op == '%') &&
                            first_left_value &&
                            !ctx->expression_is_not_evaluated &&
                            rval->value_relation != FLOW_RELATION_UNINITIALIZED &&
                            flow_alternative_can_be_zero(rval))
                    {
                        /*
                            Not in unevaluated contexts. One report per expression
                            (flow_finding_record).
                        */
                        flow_finding_record(ctx, &p_result->object, W_FLOW_DIVISION_BY_ZERO, &marker,
                                            rval, rval->p_origin_map,
                                            flow_alternative_is_zero(rval) ?
                                                (op == '/' ? "division by zero" : "modulo by zero") :
                                                (op == '/' ? "possible division by zero" : "possible modulo by zero"));
                    }

                    /*
                        Join the computed value only for operands from compatible origins. The
                        divisor check above is independent of this pairing.
                    */
                    if (!flow_origins_compatible(lval->p_origin_map, rval->p_origin_map))
                        continue;

                    /* Only if both are EQUAL and numeric can we compute an exact result */
                    if (lval->value_relation == FLOW_RELATION_EQUAL && rval->value_relation == FLOW_RELATION_EQUAL)
                    {
                        bool lnum = (lval->value_kind == FLOW_VALUE_KIND_SIGNED || lval->value_kind == FLOW_VALUE_KIND_UNSIGNED);
                        bool rnum = (rval->value_kind == FLOW_VALUE_KIND_SIGNED || rval->value_kind == FLOW_VALUE_KIND_UNSIGNED);
                        if (lnum && rnum)
                        {
                            long long l = (lval->value_kind == FLOW_VALUE_KIND_SIGNED) ? lval->value.i : (long long)lval->value.u;
                            long long r = (rval->value_kind == FLOW_VALUE_KIND_SIGNED) ? rval->value.i : (long long)rval->value.u;

                            switch (op)
                            {
                                case '+':
                                    result_val = l + r;
                                    result_rel = FLOW_RELATION_EQUAL;
                                break;
                                case '-':
                                    result_val = l - r;
                                    result_rel = FLOW_RELATION_EQUAL;
                                break;
                                case '*':
                                    result_val = l * r;
                                    result_rel = FLOW_RELATION_EQUAL;
                                break;
                                case '/':
                                    if (r == 0)
                                    {
                                        /*
                                            already warned above by the
                                            possibly-zero-divisor check
                                        */
                                        /* result remains ANY */
                                    }
                                    else
                                    {
                                        result_val = l / r;
                                        result_rel = FLOW_RELATION_EQUAL;
                                    }
                                break;
                                case '%':
                                    if (r == 0)
                                    {
                                        /*
                                            already warned above by the
                                            possibly-zero-divisor check
                                        */
                                        /* result remains ANY */
                                    }
                                    else
                                    {
                                        result_val = l % r;
                                        result_rel = FLOW_RELATION_EQUAL;
                                    }
                                break;
                                case '<': /* << */
                                case '>': /* >> */
                                    if (r < 0 || r >= (long long)(sizeof(long long) * 8))
                                    {
                                        /*
                                            shift count out of range: result is
                                            undefined -- leave ANY.
                                        */
                                    }
                                    else
                                    {
                                        result_val = (op == '<') ? (l << r) : (l >> r);
                                        result_rel = FLOW_RELATION_EQUAL;
                                    }
                                break;
                                default:
                                break;
                            }
                        }
                    }
                    else if ((op == '+' || op == '-') &&
                             type_is_signed_integer(&p_result->object.type) &&
                             rval->value_relation == FLOW_RELATION_EQUAL &&
                             rval->value_kind == FLOW_VALUE_KIND_SIGNED &&
                             lval->value_kind == FLOW_VALUE_KIND_SIGNED &&
                             lval->imaginary == FLOW_IMAGINARY_NONE &&
                             (lval->value_relation == FLOW_RELATION_NOT_EQUAL ||
                              lval->value_relation == FLOW_RELATION_GREATER ||
                              lval->value_relation == FLOW_RELATION_GREATER_EQUAL ||
                              lval->value_relation == FLOW_RELATION_LESS ||
                              lval->value_relation == FLOW_RELATION_LESS_EQUAL))
                    {
                        /* signed overflow is UB, so `k >= c` gives `k + r >= c + r` */
                        result_rel = lval->value_relation;
                        result_val = op == '+' ? lval->value.i + rval->value.i : lval->value.i - rval->value.i;
                    }

                    /* Pointer +/- integer: keep the pointer operand's own alternative instead of falling through to a degraded ANY, which made flow_alternative_can_be_zero treat an offset non-null pointer as possibly zero -- same class of bug already fixed for ++/--/+=/-=. */
                    {
                        /*
                            A pointer base is PTR or a non-null REF (array decay, `arr + 3`):
                            offsetting it never gives null.
                        */
                        /*
                            Whether the other operand is an offset is decided by its type, not its
                            value (narrow-through-pointer-arithmetic.c).
                        */
                        const bool lnum2 = type_is_integer(&p_left->object.type);
                        const bool rnum2 = type_is_integer(&p_right->object.type);
                        /*
                            A pointer re-seeded as SIGNED ANY (after a call) is still a pointer by
                            its static type; a nonzero SIGNED (string literal) is a base too
                            (conditional-literal-pointer-arithmetic.c).
                        */
                        const bool lbase = (lval->value_kind == FLOW_VALUE_KIND_PTR) ||
                            (lval->value_kind == FLOW_VALUE_KIND_REF && lval->value.p != NULL) ||
                            (type_is_pointer(&p_left->object.type) &&
                             (lval->value_relation == FLOW_RELATION_NOT_EQUAL ||
                              (lval->value_relation == FLOW_RELATION_EQUAL &&
                               lval->value_kind == FLOW_VALUE_KIND_SIGNED &&
                               lval->value.i != 0)));
                        const bool rbase = (rval->value_kind == FLOW_VALUE_KIND_PTR) ||
                            (rval->value_kind == FLOW_VALUE_KIND_REF && rval->value.p != NULL) ||
                            (type_is_pointer(&p_right->object.type) &&
                             (rval->value_relation == FLOW_RELATION_NOT_EQUAL ||
                              (rval->value_relation == FLOW_RELATION_EQUAL &&
                               rval->value_kind == FLOW_VALUE_KIND_SIGNED &&
                               rval->value.i != 0)));

                        const struct flow_alternative* _Opt base = NULL;
                        if (lbase && rnum2 && (op == '+' || op == '-'))
                        {
                            base = lval;
                        }
                        else if (rbase && lnum2 && op == '+')
                        {
                            base = rval;
                        }

                        if (base != NULL)
                        {
                            const enum flow_value_kind base_kind = base->value_kind; /* lint 33 68 BUG in flow */

                            if (base_kind == FLOW_VALUE_KIND_PTR)
                            {
                                /* Keep the pointer alternative unchanged. */
                                flow_alternatives_add(&result_alts, base); /* lint 35 BUG in flow */
                            }
                            else
                            {
                                /*
                                    Array-decay / address base: the offset result
                                    is a non-null pointer of unknown exact target.
                                */
                                struct flow_alternative a =
                                {
                                    .value_kind = FLOW_VALUE_KIND_PTR,
                                    .value = {
                                        .p = base_kind == FLOW_VALUE_KIND_REF
                                             ? base->value.p : NULL
                                },
                                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                                    .imaginary = FLOW_IMAGINARY_NONE,
                                    .p_origin_map = flow_origin_more_specific(lval->p_origin_map, rval->p_origin_map),
                                    .p_origin_token = p_result->first_token
                                }; /* lint 33 BUG in flow */
                                flow_alternatives_add(&result_alts, &a);
                            }
                            continue;
                        }
                    }

                    /* Build the result alternative. Tag it with the branch */
                    /* the operand pair belongs to (the more specific of the */
                    /* two compatible origins) so the value stays correlated. */
                    struct flow_alternative a =
                    {
                        .value_kind = FLOW_VALUE_KIND_SIGNED,
                        .value_relation = result_rel,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = flow_origin_more_specific(lval->p_origin_map, rval->p_origin_map),
                        .p_origin_token = p_result->first_token
                    }; /* could be refined based on type */
                    if (result_rel != FLOW_RELATION_ANY)
                    {
                        a.value.i = result_val;
                    }
                    else
                    {
                        a.value.i = ANY_VALUE;
                    }

                    flow_alternatives_add(&result_alts, &a);
                }
            }
        }
    }

    /* ---- Write the computed alternatives into the result expression ---- */
    struct flow_key_alternatives* _Opt dst = flow_branch_find_add(ctx->p_current_flow_branch, &p_result->object);
    if (dst)
    {
        flow_alternatives_clear(&dst->alternatives);
        if (result_alts.size > 0)
        {
            dst->alternatives = result_alts; /* move */
        }
        else
        {
            /* Not moved: release the buffer the loop above may have reserved. */
            flow_alternatives_clear(&result_alts);

            struct flow_alternative a =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED,
                .value = {.i = ANY_VALUE},
                .value_relation = FLOW_RELATION_ANY,
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = ctx->p_current_flow_branch,
                .p_origin_token = p_result->first_token
            };
            flow_alternatives_add(&dst->alternatives, &a);
        }
    }
    else
    {
        flow_alternatives_clear(&result_alts);
    }

    flow_findings_end(ctx);
}

static void flow_seed_constant_result(struct flow_ctx* ctx, const struct expression* p_expression)
{
    if (!object_has_known_value(&p_expression->object))
        return;

    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
    if (e == NULL) return;

    flow_alternatives_clear(&e->alternatives);
    struct flow_alternative a =
    {
        .value_kind = FLOW_VALUE_KIND_UNSIGNED,
        .value = {.u = object_to_unsigned_long_long(&p_expression->object)},
        .value_relation = FLOW_RELATION_EQUAL,
        .imaginary = FLOW_IMAGINARY_NONE,
        .p_origin_map = ctx->p_current_flow_branch,
        .p_origin_token = p_expression->first_token
    };
    flow_alternatives_add(&e->alternatives, &a);
}

static void flow_seed_member_default(struct flow_ctx* ctx, const struct object* _Opt member_obj, const struct token* _Opt p_token)
{
    try
    {
        if (member_obj == NULL)
            return;
        if (flow_branch_search_up(ctx->p_current_flow_branch, member_obj) != NULL)
            return; /* already has flow state */

        const bool nullable_enabled = options_default_nonnull(&ctx->ctx->options);
        if (type_is_pointer(&member_obj->type))
        {
            /*
                Seed an unseeded pointer member from its declared nullability:
                non-_Opt -> non-null, _Opt -> possibly null (never empty).
            */
            const bool is_nullable = type_is_nullable(&member_obj->type, nullable_enabled);
            struct flow_key_alternatives* _Opt me = flow_branch_find_add(ctx->p_current_flow_branch, member_obj);
            if (me == NULL)
                return; /* no entry to seed */

            flow_alternatives_clear(&me->alternatives);

            if (is_nullable)
            {
                /*
                    Model an _Opt member like an _Opt parameter: two correlated arms, the
                    non-null one with a concrete pointee, so narrowing on `a->b` has a
                    stable object to attach to (codegen.c:533).
                */
                struct flow_branch* _Opt p_null_map =
                    flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_OPT_MEM_NULL);
                struct flow_branch* _Opt p_nonnull_map =
                    flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_OPT_MEM_NONNULL);

                /*
                    Without both arm maps the two alternatives would share an
                    origin, which is worse than leaving the member unseeded.
                */
                if (p_null_map == NULL || p_nonnull_map == NULL)
                {
                    throw;
                }

                struct object* _Opt p_pointed = NULL;
                {
                    struct type pointed_type = type_remove_pointer(&member_obj->type);
                    if (!type_is_void(&pointed_type))
                    {
                        p_pointed = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
                        if (p_pointed != NULL)
                            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, &ctx->ctx->options.platform);
                    }
                    type_destroy(&pointed_type);
                }

                struct flow_alternative a_null =
                {
                    .value_kind = FLOW_VALUE_KIND_PTR,
                    .value = {.p = NULL},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = p_null_map,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&me->alternatives, &a_null);

                struct flow_alternative a_nonnull =
                {
                    .value_kind = FLOW_VALUE_KIND_PTR,
                    .value = {.p = p_pointed},
                    .value_relation = p_pointed != NULL ? FLOW_RELATION_EQUAL : FLOW_RELATION_NOT_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = p_nonnull_map,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&me->alternatives, &a_nonnull);
            }
            else
            {
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_PTR,
                    .value = {.p = NULL},
                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(&me->alternatives, &a);
            }
        }
    }
    catch
    {
    }
}

static bool flow_cast_one_value(struct flow_ctx* ctx,
                                const struct flow_alternative* alt,
                                const struct type* p_target_type,
                                struct flow_alternatives* out,
                                const struct flow_branch* _Opt origin,
                                const struct token* _Opt p_token)
{


    if (alt->value_kind == FLOW_VALUE_KIND_PTR && type_is_pointer(p_target_type) &&
            (alt->value_relation == FLOW_RELATION_EQUAL || alt->value_relation == FLOW_RELATION_NOT_EQUAL))
    {
        /*
            A cast keeps "non-null, unknown target"
            (cast-preserves-not-equal-pointer.c).
        */
        struct flow_alternative tagged = *alt;

        /*
            A void-typed fabricated pointee is re-fabricated with the cast's target
            pointee type, so `->member` resolves.
        */
        if (tagged.value.p != NULL && type_is_void(&tagged.value.p->type))
        {
            struct type target_pointee = type_remove_pointer(p_target_type);
            if (!type_is_void(&target_pointee))
            {
                struct object* _Opt p_new = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
                if (p_new != NULL)
                {
                    make_object(&target_pointee, p_new, MAKE_STATE_ANY, &ctx->ctx->options.platform);
                    tagged.value.p = p_new;
                }
            }
            type_destroy(&target_pointee);
        }

        tagged.p_origin_map = origin;
        tagged.p_origin_token = p_token;
        flow_alternatives_add(out, &tagged);
        return true;
    }

    if (alt->value_relation != FLOW_RELATION_EQUAL)
    {
        return false;
    }

    if (alt->value_kind == FLOW_VALUE_KIND_SIGNED || alt->value_kind == FLOW_VALUE_KIND_UNSIGNED)
    {
        long long val = (alt->value_kind == FLOW_VALUE_KIND_SIGNED) ? alt->value.i : (long long)alt->value.u;
        if (type_is_integer(p_target_type))
        {
            struct flow_alternative a =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED,
                .value = {.i = flow_cast_integer_value(ctx, val, p_target_type)},
                .value_relation = FLOW_RELATION_EQUAL,
                .imaginary = alt->imaginary, .p_origin_map = origin, .p_origin_token = p_token
            };
            flow_alternatives_add(out, &a);
            return true;
        }
        if (type_is_pointer(p_target_type) && val == 0)
        {
            /* (T*)0 is the null pointer constant (e.g. NULL == ((void*)0)). */
            struct flow_alternative a =
            {
                .value_kind = FLOW_VALUE_KIND_PTR, .value = {.p = NULL},
                .value_relation = FLOW_RELATION_EQUAL,
                .imaginary = alt->imaginary, .p_origin_map = origin, .p_origin_token = p_token
            };
            flow_alternatives_add(out, &a);
            return true;
        }
        return false; /* nonzero int->ptr, or int->other */
    }

    if (alt->value_kind == FLOW_VALUE_KIND_PTR && type_is_pointer(p_target_type))
    {
        struct flow_alternative tagged = *alt;
        tagged.p_origin_map = origin;
        tagged.p_origin_token = p_token;
        flow_alternatives_add(out, &tagged);
        return true;
    }

    return false;
}

/*
    "pointed object lifetime has ended" for an access, found by an ended leaf
    of the accessed member (flow_object_leaves_in_state): the leaf's line and
    branch stand for the alternative, and the branch path is the explanation.
*/
static void flow_record_ended_pointee(struct flow_ctx* ctx,
                                      const struct expression* p_expression,
                                      const struct marker* p_marker,
                                      int ended_line,
                                      const struct flow_branch* _Opt ended_origin)
{
    const struct flow_alternative ended = { .p_origin_map = ended_origin };
    struct osstream name_ss = { 0 };
    flow_expression_to_string(p_expression, &name_ss);
    struct osstream ss = { 0 };
    ss_fprintf(&ss, "lifetime of the object pointed to by '%s' has ended (see line %d)",
               name_ss.c_str ? name_ss.c_str : "", ended_line);
    flow_finding_record(ctx, &p_expression->object, W_FLOW_LIFETIME_ENDED, p_marker,
                        &ended, ended_origin, ss.c_str ? ss.c_str : "");
    ss_close(&ss);
    ss_close(&name_ss);
}

static struct flow_true_false_branches flow_visit_expression(struct flow_ctx* ctx, const struct expression* _Opt p_expression)
{
    /*
        left/right are _Owner _Opt in the AST, and callers hand them straight
        in; an absent operand is nothing to visit.
    */
    if (p_expression == NULL || ctx->p_current_flow_branch == NULL)
    {
        struct flow_true_false_branches empty = { 0 };
        return empty;
    }

    struct flow_branch* p_before = ctx->p_current_flow_branch;
    (void)p_before; /* used in some cases below */

    /* findings scopes opened below are closed by their case; a throw skips that */
    const int findings_depth = ctx->findings_depth;

    try
    {
        switch (p_expression->expression_type)
        {
            case EXPR_INVALID:
                _Assert(false);
            break;

            case EXPR_PRIMARY__FUNC__:
            break;

            case EXPR_PRIMARY_ENUMERATOR:
                if (object_has_known_value(&p_expression->object))
                if (!flow_set_object_signed(ctx, &p_expression->object, object_to_signed_long_long(&p_expression->object), FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
            break;

            case EXPR_PRIMARY_DECLARATOR:
            {
                _Assert(p_expression->declarator != NULL);
                const struct object* p_obj = &p_expression->declarator->object;
                if (!type_is_function(&p_expression->object.type) &&
                p_obj->state != CONSTANT_VALUE_STATE_CONSTANT &&
                flow_branch_search_up(ctx->p_current_flow_branch, p_obj) == NULL)
                {
                    /*file scope*/
                    /* TODO create flow_set_object_any */
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, p_obj);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);

                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = ANY_VALUE},
                            .value_relation = FLOW_RELATION_ANY,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };

                        /* A pointer global follows its declared nullability (`stdout` is non-null). */
                        if (type_is_pointer(&p_expression->object.type))
                        {
                            a.value_kind = FLOW_VALUE_KIND_PTR;
                            a.value.p = NULL;
                            a.value_relation = type_is_nullable(&p_expression->object.type, options_default_nonnull(&ctx->ctx->options))
                            ? FLOW_RELATION_ANY
                            : FLOW_RELATION_NOT_EQUAL;
                        }

                        flow_alternatives_add(&e->alternatives, &a);
                    }
                }
                else if (!type_is_function(&p_expression->object.type) &&
                 p_obj->state == CONSTANT_VALUE_STATE_CONSTANT &&
                 flow_branch_search_up(ctx->p_current_flow_branch, p_obj) == NULL)
                {
                    /*
                        Compile-time constant not carried from its declaration (each top-level
                        declaration has a fresh map): seed its value.
                    */
                    struct flow_alternative value = { 0 };
                    if (type_is_pointer(&p_obj->type))
                    {
                        value.value_kind = FLOW_VALUE_KIND_PTR;
                        value.value.p = (void*)(uintptr_t)p_obj->value.host_u_long_long;
                    }
                    else if (type_is_signed(&p_obj->type))
                    {
                        value.value_kind = FLOW_VALUE_KIND_SIGNED;
                        value.value.i = p_obj->value.host_long_long;
                    }
                    else
                    {
                        value.value_kind = FLOW_VALUE_KIND_UNSIGNED;
                        value.value.u = p_obj->value.host_u_long_long;
                    }

                    struct flow_key_alternatives* _Opt e1 = flow_branch_find_add(ctx->p_current_flow_branch, p_obj);
                    if (e1 == NULL) throw;
                    flow_alternatives_clear(&e1->alternatives);
                    struct flow_alternative a =
                    {
                        .value_kind = value.value_kind,
                        .value = value.value,
                        .value_relation = FLOW_RELATION_EQUAL,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = ctx->p_current_flow_branch,
                        .p_origin_token = p_expression->first_token
                    };
                    flow_alternatives_add(&e1->alternatives, &a);
                }

                _Assert(p_expression->declarator != NULL);

                
                struct flow_key_alternatives* _Opt e2 = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                if (e2 == NULL) throw;
                flow_alternatives_clear(&e2->alternatives);
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_REF,
                    .value = {.p = p_obj},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_expression->first_token
                };
                flow_alternatives_add(&e2->alternatives, &a);
                

                /* Build true/false branch maps narrowed on this variable. */
                const struct object* p_obj2 = &p_expression->declarator->object;
                struct flow_branch* _Opt p_true = flow_narrow_map_branch(ctx, ctx->p_condition == p_expression ? p_expression : NULL, &ctx->flow_branch_arena, ctx->p_current_flow_branch, p_obj2, true, p_expression, p_expression->first_token);
                struct flow_branch* _Opt p_false = flow_narrow_map_branch(ctx, NULL, &ctx->flow_branch_arena, ctx->p_current_flow_branch, p_obj2, false, p_expression, p_expression->first_token);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow_tag_branch_pair(p_true, p_false);
                return (struct flow_true_false_branches) { p_true, p_false };
            }

            case EXPR_PRIMARY_PARENTHESIS:
            {
                _Assert(p_expression->right != NULL);
                const struct expression* p_inner = skip_parenthesis(p_expression->right);
                struct flow_true_false_branches paren_pair = flow_visit_expression(ctx, p_inner);

                /*
                    Copy the inner value to this node too: a temporary is looked up by its
                    own object, and `(b?1:2)` is a different node from `b?1:2`.
                */
                const struct flow_key_alternatives* _Opt p_inner_entry =
                flow_branch_search_up(ctx->p_current_flow_branch, &p_inner->object);
                if (p_inner_entry)
                {
                    struct flow_key_alternatives* _Opt p_entry = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (p_entry == NULL) throw;
                    flow_alternatives_clear(&p_entry->alternatives);
                    flow_alternatives_append(&p_entry->alternatives, &p_inner_entry->alternatives);
                }

                return paren_pair;
            }

            case EXPR_PRIMARY_STATEMENT_EXPRESSION:
                _Assert(p_expression->compound_statement != NULL);
                flow_visit_compound_statement(ctx, p_expression->compound_statement);
            break;

            case EXPR_PRIMARY_STRING_LITERAL:
            {
                /* Seed a string literal as a definite-nonzero SIGNED placeholder, not FLOW_VALUE_KIND_PTR (which crashes flow_check_object_access on its synthetic char members) -- object_to_signed_long_long has no real integer for an array object, which would read as NULL. */
                if (!flow_set_object_signed(ctx, &p_expression->object, 1, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
            }
            break;

            case EXPR_PRIMARY_CHAR_LITERAL:
            case EXPR_PRIMARY_NUMBER:
            case EXPR_PRIMARY_PREDEFINED_CONSTANT:
            if (!flow_set_object_signed(ctx, &p_expression->object, object_to_signed_long_long(&p_expression->object), FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
            break;

            case EXPR_PRIMARY_GENERIC:
                _Assert(p_expression->generic_selection != NULL);
                flow_visit_generic_selection(ctx, p_expression->generic_selection);
            break;

            case EXPR_POSTFIX_DOT:
            {
                _Assert(p_expression->left != NULL);
                const int member_index = p_expression->member_index;
                flow_visit_expression(ctx, p_expression->left);

                struct flow_key_alternatives* _Opt result_entry = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                if (result_entry == NULL) throw;
                flow_alternatives_clear(&result_entry->alternatives);

                const struct flow_key_alternatives* _Opt p_left_alternatives = flow_branch_search_up(ctx->p_current_flow_branch, &skip_parenthesis(p_expression->left)->object);

                struct flow_branch* _Opt p_true = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, true, p_expression);
                struct flow_branch* _Opt p_false = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, false, p_expression);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow_tag_branch_pair(p_true, p_false);

                for (int i = 0; p_left_alternatives != NULL && i < p_left_alternatives->alternatives.size; i++)
                {
                    const struct flow_alternative* p_left_alternative = p_left_alternatives->alternatives.data[i];

                    if (p_left_alternative->value_relation == FLOW_RELATION_EQUAL &&
                    p_left_alternative->value_kind == FLOW_VALUE_KIND_REF &&
                    p_left_alternative->value.p != NULL)
                    {
                        struct object* _Opt p_member = object_get_member(p_left_alternative->value.p, member_index);

                        /* Seed an unseeded member on `.` access, like `->` does. */
                        flow_seed_member_default(ctx, p_member, p_expression->first_token);

                        {
                            struct flow_alternative a =
                            {
                        .value_kind = FLOW_VALUE_KIND_REF,
                        .value = {.p = p_member},
                        .value_relation = FLOW_RELATION_EQUAL,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = ctx->p_current_flow_branch,
                        .p_origin_token = p_expression->first_token
                            };
                            flow_alternatives_add(&result_entry->alternatives, &a);
                        }

                        if (p_member != NULL)
                        {
                            flow_narrow_map_into(ctx, ctx->p_condition == p_expression ? p_expression : NULL, p_true, ctx->p_current_flow_branch, p_member, true, p_expression->first_token);
                            flow_narrow_map_into(ctx, NULL, p_false, ctx->p_current_flow_branch, p_member, false, p_expression->first_token);
                        }
                    }
                }

                /* Narrow on the member field used as bool. */
                return (struct flow_true_false_branches)
                {
                p_true, p_false
                };
            }

            case EXPR_POSTFIX_ARROW:
            {
                _Assert(p_expression->left != NULL);

                flow_visit_expression(ctx, p_expression->left);

                const int member_index = p_expression->member_index;
                struct marker marker = expression_to_marker(p_expression->left);

                const struct flow_key_alternatives* _Opt p_left_alternatives = flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->left->object);

                struct flow_key_alternatives* _Opt result_entry = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                if (result_entry == NULL) throw;
                flow_alternatives_clear(&result_entry->alternatives);

                struct flow_branch* _Opt p_true = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, true, p_expression);
                struct flow_branch* _Opt p_false = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, false, p_expression);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow_tag_branch_pair(p_true, p_false);

                bool any_member_resolved = false;

                /*
                    One report per access, with the count: a merge can leave several null
                    alternatives, or many aiming at one ended pointee. Keyed by the access
                    expression.
                */
                flow_findings_begin(ctx);

                /* Fabricate an unresolvable pointer's pointee on demand, at the access (not eagerly per call), via make_object rather than flow_object_init (which would wrongly assert every _Opt member as possibly-null) so a guard like `if (pX->p)` can actually narrow it (safety-049). */
                if (p_left_alternatives != NULL)
                {
                    p_left_alternatives = flow_pointer_pointee_on_demand(ctx, p_expression->left, p_left_alternatives);
                    if (p_left_alternatives == NULL)
                        throw;
                }

                if (p_left_alternatives != NULL)
                {
                    for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
                    {
                        const struct flow_alternative* ptr_alt = p_left_alternatives->alternatives.data[i];

                        flow_record_uninitialized_operand_value(ctx, p_expression->left, ptr_alt, &marker);

                        /* Resolve LHS to concrete pointer alternatives */
                        const struct flow_key_alternatives* _Opt p_pointer_alts = NULL;
                        if (ptr_alt->value_relation == FLOW_RELATION_EQUAL &&
                        ptr_alt->value_kind == FLOW_VALUE_KIND_REF &&
                        ptr_alt->value.p != NULL)
                        {
                            p_pointer_alts = flow_branch_search_up(ctx->p_current_flow_branch, ptr_alt->value.p);
                        }
                        else if (ptr_alt->value_kind == FLOW_VALUE_KIND_PTR)
                        {
                            /* Direct pointer – treat as single-alternative */
                            p_pointer_alts = NULL;
                        }
                        else
                        {
                            continue;
                        }

                        /* Process a list of pointer alternatives (resolved from REF) */
                        if (p_pointer_alts != NULL)
                        {
                            for (int k = 0; k < p_pointer_alts->alternatives.size; k++)
                            {
                                const struct flow_alternative* p_pointer_alt = p_pointer_alts->alternatives.data[k];

                                flow_record_uninitialized_operand_value(ctx, p_expression->left, p_pointer_alt, &marker);

                                /*
                                    Null check (skipped in unevaluated contexts like
                                    sizeof/_Alignof: the -> is never applied at runtime).
                                */
                                if (p_pointer_alt->value_relation == FLOW_RELATION_EQUAL &&
                                p_pointer_alt->value_kind == FLOW_VALUE_KIND_PTR &&
                                p_pointer_alt->value.p == NULL)
                                {
                                    /*
                                        A null value that cannot occur on this path (its branch
                                        decisions conflict with where we are) is not a finding --
                                        the same origin check the `*p` dereference applies.
                                    */
                                    if (!ctx->expression_is_not_evaluated &&
                                        flow_origins_compatible(p_pointer_alt->p_origin_map, ctx->p_current_flow_branch))
                                    {
                                        struct osstream name_ss = { 0 };
                                        flow_expression_to_string(p_expression->left, &name_ss);
                                        struct osstream ss = { 0 };
                                        ss_fprintf(&ss, "'%s->' possible null dereference", name_ss.c_str ? name_ss.c_str : "");
                                        flow_finding_record(ctx, &p_expression->object, W_FLOW_NULL_DEREFERENCE, &marker,
                                                            p_pointer_alt, p_pointer_alt->p_origin_map,
                                                            ss.c_str ? ss.c_str : "");
                                        ss_close(&ss);
                                        ss_close(&name_ss);
                                    }
                                    continue;
                                }

                                if (p_pointer_alt->value_kind != FLOW_VALUE_KIND_PTR || p_pointer_alt->value.p == NULL)
                                    continue;

                                const struct object* p_pointed_obj = p_pointer_alt->value.p;

                                struct object* _Opt member_obj = object_get_member(p_pointed_obj, member_index);
                                if (member_obj == NULL)
                                {
                                    /* _Assert(false); */
                                    continue;
                                }

                                /*
                                    Ended-lifetime check on member_obj's own leaves, filtered by the current
                                    branch, not the pointer's origin: an alias can reach an ENDED leaf
                                    unrelated to that origin (aliased-owner-lifetime-not-tracked).
                                */
                                int ended_line = 0;
                                const struct flow_branch* _Opt ended_origin = NULL;
                                if (flow_object_leaves_in_state(ctx, member_obj, FLOW_LEAF_ENDED,
                                                                p_pointer_alt->p_origin_map, true, false, &ended_line, &ended_origin))
                                {
                                    /* Include the expression, so it is clear which `->` is meant. */
                                    flow_record_ended_pointee(ctx, p_expression, &marker, ended_line, ended_origin);
                                }

                                flow_seed_member_default(ctx, member_obj, p_expression->first_token);

                                {
                                    struct flow_alternative a =
                                    {
                                .value_kind = FLOW_VALUE_KIND_REF,
                                .value = {.p = member_obj},
                                .value_relation = FLOW_RELATION_EQUAL,
                                .imaginary = FLOW_IMAGINARY_NONE,
                                .p_origin_map = ctx->p_current_flow_branch,
                                .p_origin_token = p_expression->first_token
                                    };
                                    flow_alternatives_add(&result_entry->alternatives, &a);
                                }

                                flow_narrow_map_into(ctx, ctx->p_condition == p_expression ? p_expression : NULL, p_true, ctx->p_current_flow_branch, member_obj, true, p_expression->first_token);
                                flow_narrow_map_into(ctx, NULL, p_false, ctx->p_current_flow_branch, member_obj, false, p_expression->first_token);

                                any_member_resolved = true;
                            }
                        }
                        else
                        {
                            /* Direct pointer alternative (ptr_alt is FLOW_VALUE_PTR) */
                            if (ptr_alt->value_relation == FLOW_RELATION_EQUAL &&
                            ptr_alt->value_kind == FLOW_VALUE_KIND_PTR &&
                            ptr_alt->value.p == NULL)
                            {
                                /* origin check: see the REF-alternative site above */
                                if (!ctx->expression_is_not_evaluated &&
                                    flow_origins_compatible(ptr_alt->p_origin_map, ctx->p_current_flow_branch))
                                {
                                    struct osstream name_ss = { 0 };
                                    flow_expression_to_string(p_expression->left, &name_ss);
                                    struct osstream ss = { 0 };
                                    ss_fprintf(&ss, "possible null pointer dereference '%s'", name_ss.c_str ? name_ss.c_str : "");
                                    flow_finding_record(ctx, &p_expression->object, W_FLOW_NULL_DEREFERENCE, &marker,
                                                        ptr_alt, ptr_alt->p_origin_map,
                                                        ss.c_str ? ss.c_str : "");
                                    ss_close(&ss);
                                    ss_close(&name_ss);
                                }
                                continue;
                            }

                            if (ptr_alt->value_kind != FLOW_VALUE_KIND_PTR || ptr_alt->value.p == NULL)
                                continue;

                            const struct object* p_pointed_obj = ptr_alt->value.p;

                            struct object* _Opt member_obj = object_get_member(p_pointed_obj, member_index);
                            if (member_obj == NULL)
                            {
                                _Assert(false);
                                continue;
                            }

                            /*
                                "Any leaf ended" on the accessed member, against ptr_alt's origin and
                                the current branch (see the REF-alternative site above).
                            */
                            int ended_line = 0;
                            const struct flow_branch* _Opt ended_origin = NULL;
                            if (flow_object_leaves_in_state(ctx, member_obj, FLOW_LEAF_ENDED,
                                                            ptr_alt->p_origin_map, true, false, &ended_line, &ended_origin))
                            {
                                /*
                                    Include the expression itself -- see the identical fix and
                                    rationale at the REF-alternative site above.
                                */
                                flow_record_ended_pointee(ctx, p_expression, &marker, ended_line, ended_origin);
                            }

                            flow_seed_member_default(ctx, member_obj, p_expression->first_token);

                            {
                                struct flow_alternative a =
                                {
                                    .value_kind = FLOW_VALUE_KIND_REF,
                                    .value = {.p = member_obj},
                                    .value_relation = FLOW_RELATION_EQUAL,
                                    .imaginary = FLOW_IMAGINARY_NONE,
                                    .p_origin_map = ctx->p_current_flow_branch,
                                    .p_origin_token = p_expression->first_token
                                };
                                flow_alternatives_add(&result_entry->alternatives, &a);
                            }

                            flow_narrow_map_into(ctx, ctx->p_condition == p_expression ? p_expression : NULL, p_true, ctx->p_current_flow_branch, member_obj, true, p_expression->first_token);
                            flow_narrow_map_into(ctx, NULL, p_false, ctx->p_current_flow_branch, member_obj, false, p_expression->first_token);

                            any_member_resolved = true;
                        }
                    }
                }

                /* a pointer member's pointee too: `q = p->next` and `p->next` share it */
                if (any_member_resolved && type_is_pointer(&p_expression->object.type))
                {
                    flow_pointer_pointee_on_demand(ctx, p_expression,
                                                   flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->object));
                }

                if (!any_member_resolved)
                {
                    /*
                        No object behind the member: seed from its declared type, never empty
                        (non-_Opt pointer non-null, _Opt possibly null, integer ANY).
                    */
                    const bool nullable_enabled = options_default_nonnull(&ctx->ctx->options);
                    struct flow_key_alternatives* _Opt e_unres = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (e_unres == NULL) throw;
                    if (e_unres != NULL && e_unres->alternatives.size == 0)
                    {
                        struct flow_alternative a =
                        {
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_expression->first_token
                        };
                        if (type_is_pointer(&p_expression->object.type))
                        {
                            a.value_kind = FLOW_VALUE_KIND_PTR;
                            a.value.p = NULL;
                            a.value_relation = type_is_nullable(&p_expression->object.type, nullable_enabled)
                            ? FLOW_RELATION_ANY : FLOW_RELATION_NOT_EQUAL;
                            flow_alternatives_add(&e_unres->alternatives, &a);
                        }
                        else if (type_is_integer(&p_expression->object.type))
                        {
                            a.value_kind = type_is_signed(&p_expression->object.type)
                            ? FLOW_VALUE_KIND_SIGNED : FLOW_VALUE_KIND_UNSIGNED;
                            a.value_relation = FLOW_RELATION_ANY;
                            flow_alternatives_add(&e_unres->alternatives, &a);
                        }
                    }

                    p_true = flow_narrow_map_branch(ctx, ctx->p_condition == p_expression ? p_expression : NULL, &ctx->flow_branch_arena, ctx->p_current_flow_branch, &p_expression->object, true, p_expression, p_expression->first_token);
                    p_false = flow_narrow_map_branch(ctx, NULL, &ctx->flow_branch_arena, ctx->p_current_flow_branch, &p_expression->object, false, p_expression, p_expression->first_token);
                    if (p_true == NULL || p_false == NULL)
                        throw;

                }

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                flow_findings_end(ctx);
                return (struct flow_true_false_branches)
                {
                p_true, p_false
                };
            }

            case EXPR_POSTFIX_ARRAY:
            {
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);
                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);

                /*
                    Bounds check for a flow-derived index (constants are checked by the
                    parser). Warn only when the whole interval is out of bounds.
                */
                {
                    const struct type* p_arr_type = &skip_parenthesis(p_expression->left)->object.type;
                    if (!ctx->expression_is_not_evaluated &&
                    type_is_array(p_arr_type) &&
                    p_arr_type->array_num_elements > 0 &&
                    !object_has_known_value(&p_expression->right->object))
                    {
                        const long long array_num_elements = (long long)p_arr_type->array_num_elements;
                        const struct flow_key_alternatives* _Opt idx_alts =
                        flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->right->object);
                        struct marker idx_marker = expression_to_marker(p_expression);
                        bool warned = false;
                        for (int i = 0; !warned && idx_alts != NULL && i < idx_alts->alternatives.size; i++)
                        {
                            const struct flow_alternative* idx = idx_alts->alternatives.data[i];

                            /*
                                The index expression usually resolves to a REF to the
                                variable object; follow it to the value alternatives.
                            */
                            const struct flow_key_alternatives* _Opt value_alts = NULL;
                            if (idx->value_kind == FLOW_VALUE_KIND_REF && idx->value.p != NULL)
                                value_alts = flow_branch_search_up(ctx->p_current_flow_branch, idx->value.p);

                            const struct flow_alternative* vlist_one = idx;
                            int vcount = 1;
                            /*
                                data is now an array of OWNED POINTERS, not an inline
                                array of structs -- this is itself already the
                                element pointer type, so vlist_many[j] below needs
                                no '&'.
                            */
                            struct flow_alternative* _Owner _Opt* _Opt vlist_many = NULL;
                            if (value_alts != NULL)
                            {
                                vlist_many = value_alts->alternatives.data;
                                vcount = value_alts->alternatives.size;
                            }

                            for (int j = 0; !warned && j < vcount; j++)
                            {
                                const struct flow_alternative* v = vlist_many ? vlist_many[j] : vlist_one;
                                long long lo = 0, hi = 0;
                                if (!flow_alt_to_interval(v, &lo, &hi))
                                    continue;
                                if (lo >= array_num_elements)
                                {
                                    diagnostic(W_FLOW_OUT_OF_BOUNDS, ctx->ctx, NULL, &idx_marker,
                                           "array index is past the end of the array (size %lld)", array_num_elements);
                                    warned = true;
                                }
                                else if (hi < 0)
                                {
                                    diagnostic(W_FLOW_OUT_OF_BOUNDS, ctx->ctx, NULL, &idx_marker,
                                           "array index is negative");
                                    warned = true;
                                }
                            }
                        }
                    }
                }

                /*
                    Constant index: REF to the element object, so element values are
                    tracked (as EXPR_POSTFIX_DOT). Otherwise narrow this expression's own
                    object.
                */
                if (object_has_known_value(&p_expression->right->object))
                {
                    const signed long long index = object_to_signed_long_long(&p_expression->right->object);

                    struct flow_key_alternatives* _Opt result_entry = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (result_entry == NULL) throw;
                    flow_alternatives_clear(&result_entry->alternatives);

                    const struct flow_key_alternatives* _Opt p_left_alternatives =
                    flow_branch_search_up(ctx->p_current_flow_branch, &skip_parenthesis(p_expression->left)->object);

                    struct flow_branch* _Opt p_true = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, true, p_expression);
                    struct flow_branch* _Opt p_false = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, false, p_expression);
                    if (p_true == NULL || p_false == NULL)
                        throw;

                    flow_tag_branch_pair(p_true, p_false);

                    bool any_resolved = false;
                    for (int i = 0; index >= 0 && p_left_alternatives != NULL && i < p_left_alternatives->alternatives.size; i++)
                    {
                        const struct flow_alternative* p_left_alternative = p_left_alternatives->alternatives.data[i];

                        if (p_left_alternative->value_relation == FLOW_RELATION_EQUAL &&
                        p_left_alternative->value_kind == FLOW_VALUE_KIND_REF &&
                        p_left_alternative->value.p != NULL)
                        {
                            flow_make_lazy_array(ctx, (struct object* _Opt)p_left_alternative->value.p, p_expression->first_token);

                            struct object* _Opt p_element = object_get_member(p_left_alternative->value.p, (size_t)index);
                            if (p_element == NULL)
                                continue;

                            struct flow_alternative a =
                            {
                                .value_kind = FLOW_VALUE_KIND_REF,
                                .value = {.p = p_element},
                                .value_relation = FLOW_RELATION_EQUAL,
                                .imaginary = FLOW_IMAGINARY_NONE,
                                .p_origin_map = flow_origin_more_specific(ctx->p_current_flow_branch, p_left_alternative->p_origin_map),
                                .p_origin_token = p_expression->first_token
                            };
                            flow_alternatives_add(&result_entry->alternatives, &a);

                            flow_narrow_map_into(ctx, ctx->p_condition == p_expression ? p_expression : NULL, p_true, ctx->p_current_flow_branch, p_element, true, p_expression->first_token);
                            flow_narrow_map_into(ctx, NULL, p_false, ctx->p_current_flow_branch, p_element, false, p_expression->first_token);
                            any_resolved = true;
                        }
                    }

                    /* the destination of a write keeps its operand for flow_invalidate_unknown_index_write */
                    if (ctx->p_subscript_being_written != p_expression)
                    {
                        flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                    }
                    flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);

                    if (any_resolved)
                        return (struct flow_true_false_branches)
                        {
                    p_true, p_false
                        };

                    /*
                        Nothing resolved (unknown array, out-of-range, etc.): fall through
                        to plain narrowing on this expression's object.
                    */
                }

                /*
                    Seed an unresolved subscript as ANY, never empty: an empty operand folds
                    `v[i] == c` to always true.
                */
                if (type_is_integer(&p_expression->object.type))
                {
                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (e == NULL) throw;
                    if (e->alternatives.size == 0)
                    {
                        struct flow_alternative a =
                        {
                            .value_kind = type_is_signed(&p_expression->object.type)
                                          ? FLOW_VALUE_KIND_SIGNED : FLOW_VALUE_KIND_UNSIGNED,
                            .value_relation = FLOW_RELATION_ANY,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                }
                else if (type_is_pointer(&p_expression->object.type) &&
                 options_default_nonnull(&ctx->ctx->options) &&
                 !type_is_nullable(&p_expression->object.type, options_default_nonnull(&ctx->ctx->options)))
                {
                    /*
                        An unresolved element of a non-_Opt pointer array is non-null
                        (`argv[i]`).
                    */
                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (e == NULL) throw;
                    if (e->alternatives.size == 0)
                    {
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_PTR,
                            .value = {.p = NULL},
                            .value_relation = FLOW_RELATION_NOT_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                }

                /* Array element used as bool (unknown index or unresolved). */
                const struct object* p_obj = &p_expression->object;
                struct flow_branch* _Opt p_true = flow_narrow_map_branch(ctx, ctx->p_condition == p_expression ? p_expression : NULL, &ctx->flow_branch_arena, ctx->p_current_flow_branch, p_obj, true, p_expression, p_expression->first_token);
                struct flow_branch* _Opt p_false = flow_narrow_map_branch(ctx, NULL, &ctx->flow_branch_arena, ctx->p_current_flow_branch, p_obj, false, p_expression, p_expression->first_token);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow_tag_branch_pair(p_true, p_false);
                if (ctx->p_subscript_being_written != p_expression)
                {
                    flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                }
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                return (struct flow_true_false_branches)
                {
                p_true, p_false
                };
            }

            case EXPR_POSTFIX_FUNCTION_CALL:
            {
                _Assert(p_expression->left != NULL);

                flow_visit_expression(ctx, p_expression->left);

                /* fp() with a nullable function pointer */
                if (type_is_pointer(&p_expression->left->object.type) && !ctx->expression_is_not_evaluated)
                {
                    struct marker marker = expression_to_marker(p_expression->left);
                    const struct flow_key_alternatives* _Opt p_callee = flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->left->object);
                    flow_findings_begin(ctx);
                    for (int i = 0; p_callee != NULL && i < p_callee->alternatives.size; i++)
                    {
                        const struct flow_alternative* p_ref = p_callee->alternatives.data[i];
                        const struct flow_key_alternatives* _Opt p_values = p_ref->value_kind == FLOW_VALUE_KIND_REF && p_ref->value.p != NULL ?
                            flow_branch_search_up(ctx->p_current_flow_branch, p_ref->value.p) : NULL;
                        for (int j = 0; p_values != NULL && j < p_values->alternatives.size; j++)
                        {
                            const struct flow_alternative* p_alt = p_values->alternatives.data[j];
                            if (flow_alternative_can_be_zero(p_alt) &&
                                (p_alt->value_relation != FLOW_RELATION_ANY ||
                                 type_is_nullable(&p_expression->left->object.type, options_default_nonnull(&ctx->ctx->options))) &&
                                flow_origins_compatible(p_alt->p_origin_map, ctx->p_current_flow_branch))
                            {
                                struct osstream name_ss = { 0 };
                                flow_expression_to_string(p_expression->left, &name_ss);
                                struct osstream ss = { 0 };
                                ss_fprintf(&ss, "possible null pointer dereference '%s'", name_ss.c_str ? name_ss.c_str : "");
                                flow_finding_record(ctx, &p_expression->left->object, W_FLOW_NULL_DEREFERENCE, &marker,
                                                    p_alt, p_alt->p_origin_map, ss.c_str ? ss.c_str : "");
                                ss_close(&ss);
                                ss_close(&name_ss);
                            }
                        }
                    }
                    flow_findings_end(ctx);
                }

                flow_visit_function_arguments(ctx, &p_expression->left->object.type, &p_expression->argument_expression_list);

                const bool nullable_enabled = options_default_nonnull(&ctx->ctx->options);
                const struct type* p_ret_type = &p_expression->object.type;
                const struct token* p_call_token = p_expression->first_token;
                /*
                    `_Clear` in RETURN position means the returned pointee is all-zero
                    (calloc) -- the return-side reading of the same qualifier that, on a
                    parameter, means "the callee zeroes the pointee".
                */
                const bool ret_zero = type_is_pointer(p_ret_type) &&
                (type_is_clear(p_ret_type) || type_is_pointed_clear(p_ret_type));
                const bool ret_uninit = type_is_pointer(p_ret_type) &&
                (type_is_uninit(p_ret_type) || type_is_pointed_uninit(p_ret_type));

                if (nullable_enabled && type_is_pointer(p_ret_type) && type_is_nullable(p_ret_type, nullable_enabled))
                {
                    struct flow_key_alternatives* _Opt p_result_alternatives = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (p_result_alternatives == NULL) throw;
                    flow_alternatives_clear(&p_result_alternatives->alternatives);

                    struct flow_branch* _Opt p_null_map = flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_CALL_OPT_NULL);
                    if (p_null_map == NULL)
                        throw;

                    {
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_PTR,
                            .value = {.p = NULL},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = p_null_map,
                            .p_origin_token = p_call_token
                        };
                        flow_alternatives_add(&p_result_alternatives->alternatives, &a);
                    }

                    struct flow_branch* _Opt p_nonnull_map = flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_CALL_OPT_NONNULL);
                    if (p_nonnull_map == NULL)
                        throw;

                    struct object* _Opt p_pointed = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
                    if (p_pointed != NULL)
                    {
                        if (ctx->p_current_flow_branch == NULL)
                            throw;

                        struct type pointed_type = type_remove_pointer(p_ret_type);
                        make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, &ctx->ctx->options.platform);

                        struct flow_branch* old = ctx->p_current_flow_branch;
                        ctx->p_current_flow_branch = p_nonnull_map;
                        flow_object_init(ctx, p_pointed, p_call_token);
                        ctx->p_current_flow_branch = old;
                        /*
                            Return-type contract on the pointee: _Clear (calloc) seeds members 0,
                            _Uninitialized (malloc) indeterminate. The qualifier may be on the
                            pointer or the pointee. Seeded in the live parent map, not
                            p_nonnull_map.
                        */
                        if (ret_zero)
                            flow_branch_set_object_zero(ctx->p_current_flow_branch, p_pointed, p_call_token);
                        else if (ret_uninit)
                            flow_branch_set_object_uninitialized(ctx->p_current_flow_branch, p_pointed, p_call_token);
                        type_destroy(&pointed_type);
                    }

                    {
                        /*
                            p_pointed == NULL (allocation failure): fall back to a plain
                            "non-null" alternative with no concrete pointee.
                        */
                        struct flow_alternative a =
                        {
                    .value_kind = FLOW_VALUE_KIND_PTR,
                    .value = {.p = p_pointed},
                    .value_relation = p_pointed != NULL ? FLOW_RELATION_EQUAL : FLOW_RELATION_NOT_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = p_nonnull_map,
                    .p_origin_token = p_call_token
                        };
                        flow_alternatives_add(&p_result_alternatives->alternatives, &a);
                    }

                }
                else if (nullable_enabled && type_is_pointer(p_ret_type))
                {
                    /*
                        Non-_Opt pointer return: non-null. For a `_Clear`/`_Uninitialized`
                        pointee contract, build a concrete pointee so members can be
                        seeded zero/uninitialized (mirrors the _Opt branch above).
                    */
                    struct object* _Opt p_pointed = NULL;
                    if (ret_zero || ret_uninit)
                    {
                        p_pointed = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
                        if (p_pointed != NULL)
                        {
                            struct type pointed_type = type_remove_pointer(p_ret_type);
                            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, &ctx->ctx->options.platform);
                            if (ret_zero)
                                flow_branch_set_object_zero(ctx->p_current_flow_branch, p_pointed, p_call_token);
                            else
                                flow_branch_set_object_uninitialized(ctx->p_current_flow_branch, p_pointed, p_call_token);
                            type_destroy(&pointed_type);
                        }
                    }
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_PTR,
                            .value = {.p = p_pointed},
                            .value_relation = p_pointed != NULL ? FLOW_RELATION_EQUAL : FLOW_RELATION_NOT_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_call_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                }
                else if (!type_is_void(p_ret_type))
                {
                    /*
                        A call's return value is fully initialized: seed it like a parameter
                        (ANY / non-null, recursively).
                    */
                    /*
                        flow_parameter_object_init takes non-const but only reads .members and
                        writes the flow map; cast away const here.
                    */
                    flow_parameter_object_init(ctx, (struct object*)&p_expression->object, p_ret_type, p_call_token);
                }
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
            }
            break;

            case EXPR_POSTFIX_FUNCTION_LITERAL:
            {
                _Assert(p_expression->compound_statement != NULL);

                /*
                    A function literal's body is not reached through flow_visit_declaration:
                    do the per-function setup here, or its _Owner parameters are never
                    seeded.
                */
                const struct direct_declarator* _Opt p_innermost_direct_declarator =
                p_expression->type_name && p_expression->type_name->abstract_declarator ?
                get_innermost_direct_declarator(p_expression->type_name->abstract_declarator->direct_declarator) :
                NULL;

                struct parameter_list* _Opt p_parameter_list =
                p_innermost_direct_declarator &&
                p_innermost_direct_declarator->function_declarator &&
                p_innermost_direct_declarator->function_declarator->parameter_type_list_opt ?
                p_innermost_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list :
                NULL;

                for (struct parameter_declaration* _Opt p_parameter = p_parameter_list ? p_parameter_list->head : NULL;
                     p_parameter;
                     p_parameter = p_parameter->next)
                {
                    if (p_parameter->declarator)
                    {
                        flow_parameter_object_init(ctx,
                                               &p_parameter->declarator->object,
                                               &p_parameter->declarator->object.type,
                                               p_parameter->declaration_specifiers->first_token);
                    }
                }

                struct type* _Opt p_previous_return_type = ctx->p_return_type;
                struct type return_type = get_function_return_type(&p_expression->object.type);
                ctx->p_return_type = &return_type;

                flow_visit_compound_statement_core(ctx, p_expression->compound_statement);

                if (!compound_statement_is_last_item_return(p_expression->compound_statement))
                {
                    flow_exit_block_visit_defer_list(ctx, &p_expression->compound_statement->defer_list, p_expression->compound_statement->last_token);
                    flow_check_arena_objects_at_function_exit(ctx);
                    const struct marker marker =
                    {
                        .p_token_begin = p_expression->compound_statement->last_token,
                        .p_token_end = p_expression->compound_statement->last_token
                    };
                    flow_check_file_scope_objects_at_function_exit(ctx);
                    flow_check_write_qualified_params_at_exit(ctx, &marker, p_expression->compound_statement->last_token);
                    flow_defer_list_set_end_of_lifetime(ctx, &p_expression->compound_statement->defer_list, p_expression->compound_statement->last_token);
                    flow_check_exiting_objects(ctx, NULL, &marker);
                }

                type_destroy(&return_type);
                ctx->p_return_type = p_previous_return_type;

                if (p_expression->compound_statement->lint_token)
                {
                    flow_check_dianostic_suppression(ctx, p_expression->compound_statement->lint_token);
                }

                break;
            }

            case EXPR_POSTFIX_COMPOUND_LITERAL:
            {
                _Assert(p_expression->left == NULL);
                _Assert(p_expression->right == NULL);
                _Assert(p_expression->type_name != NULL);
                _Assert(p_expression->braced_initializer != NULL);

                /* const struct object* p_agg = &p_expression->object; */
                const struct token* p_token = p_expression->first_token;

                /* 1. Evaluate all RHS expressions. */
                flow_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

                /*
                    A compound literal's own object is seeded like a declarator
                    (flow_object_init + flow_seed_aggregate_from_init_exprs), so
                    `x = (struct X){0};` keeps per-member zero/constant info.
                */
                flow_object_init(ctx, (struct object*)&p_expression->object, p_token);
                flow_seed_aggregate_from_init_exprs(ctx, (struct object*)&p_expression->object);
                break;
            }

            case EXPR_UNARY_STATIC_ASSERTION:
                _Assert(p_expression->static_assertion);
                flow_visit_static_assertion(ctx, p_expression->static_assertion);
            break;

            case EXPR_UNARY_ALIGNOF_EXPRESSION:
            {
                _Assert(p_expression->right);
                const bool t2 = ctx->expression_is_not_evaluated;
                ctx->expression_is_not_evaluated = true;
                flow_visit_expression(ctx, p_expression->right);
                ctx->expression_is_not_evaluated = t2;
                flow_seed_constant_result(ctx, p_expression);
                break;
            }

            case EXPR_UNARY_ALIGNOF_TYPE:
                flow_seed_constant_result(ctx, p_expression);
            break;

            case EXPR_UNARY_ASSERT:
                /* _Assert(expr) is `if (!expr) exit();`: only the true branch continues. */
                if (p_expression->right)
                {
                    struct flow_true_false_branches assert_pair = flow_visit_expression(ctx, p_expression->right);

                    /*
                        The false branch is dead (assert would have aborted).
                        Merge only the true outcome back into p_before.
                    */
                    if (assert_pair.p_true != NULL)
                    {
                        flow_branch_merge_a_b(p_before, assert_pair.p_true, assert_pair.p_true);
                    }
                    ctx->p_current_flow_branch = p_before;
                }
            break;

            case EXPR_UNARY_SIZEOF_EXPRESSION:
            {
                _Assert(p_expression->right);

                const bool t2 = ctx->expression_is_not_evaluated;
                ctx->expression_is_not_evaluated = true;
                flow_visit_expression(ctx, p_expression->right);
                ctx->expression_is_not_evaluated = t2;
                flow_seed_constant_result(ctx, p_expression);
                break;
            }

            case EXPR_UNARY_NEG:
            case EXPR_UNARY_PLUS:
                _Assert(p_expression->right != NULL);
                /*
                * Visit the child first so that any sub-expression (e.g. -(a + b))
                * is fully evaluated and its constant value — if any — is propagated
                * into p_expression->right->object before we inspect it.
                */
                flow_visit_expression(ctx, p_expression->right);
                if (object_has_constant_value(&p_expression->right->object))
                {
                    const long long rv = object_to_signed_long_long(&p_expression->right->object);
                    const long long result = (p_expression->expression_type == EXPR_UNARY_NEG) ? -rv : rv;
                    if (!flow_set_object_signed(ctx, &p_expression->object, result, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
                }
                else
                {
                    /*
                        No constant, but maybe a relation (`b < 0`): + keeps it, - mirrors it;
                        otherwise ANY.
                    */
                    const bool is_neg = (p_expression->expression_type == EXPR_UNARY_NEG);
                    const struct flow_key_alternatives* _Opt p_src =
                    flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->right->object);

                    struct flow_alternatives mapped = { 0 };
                    bool all_mapped = (p_src != NULL && p_src->alternatives.size > 0);
                    const struct marker operand_marker = expression_to_marker(p_expression->right);
                    flow_findings_begin(ctx);

                    for (int i = 0; all_mapped && p_src != NULL && i < p_src->alternatives.size; i++)
                    {
                        const struct flow_alternative* a0 = p_src->alternatives.data[i];

                        /*
                            The operand usually resolves to a REF to the variable object;
                            follow it to the actual value alternatives.
                        */
                        const struct flow_key_alternatives* _Opt p_vals = NULL;
                        if (a0->value_kind == FLOW_VALUE_KIND_REF && a0->value.p != NULL)
                            p_vals = flow_branch_search_up(ctx->p_current_flow_branch, a0->value.p);

                        /* data holds pointers: with no REF, use a one-element array holding a0. */
                        struct flow_alternative* _Opt single_list[1];
                        struct flow_alternative* _Opt* _Opt list;
                        if (p_vals != NULL)
                        {
                            list = p_vals->alternatives.data; /* lint 81 */
                        }
                        else
                        {
                            single_list[0] = (struct flow_alternative*)a0;
                            list = single_list;
                        }
                        const int count = p_vals ? p_vals->alternatives.size : 1;
                        if (count == 0)
                        {
                            all_mapped = false;
                            break;
                        }

                        for (int j = 0; j < count; j++)
                        {
                            flow_record_uninitialized_operand_value(ctx, p_expression->right, list[j], &operand_marker);
                            struct flow_alternative out = { 0 };
                            if (is_neg)
                            {
                                if (!flow_alt_negate(list[j], &out))
                                {
                                    all_mapped = false;
                                    break;
                                }
                            }
                            else
                            {
                                out = *list[j];
                                if (out.value_kind != FLOW_VALUE_KIND_SIGNED)
                                {
                                    all_mapped = false;
                                    break;
                                }
                            }
                            out.p_origin_map = ctx->p_current_flow_branch;
                            out.p_origin_token = p_expression->first_token;
                            flow_alternatives_add(&mapped, &out);
                        }
                    }
                    flow_findings_end(ctx);

                    if (all_mapped && mapped.size > 0)
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL)
                        {
                            flow_alternatives_clear(&mapped);
                            throw;
                        }
                        flow_alternatives_clear(&e->alternatives);
                        e->alternatives = mapped;
                        break;
                    }
                    flow_alternatives_clear(&mapped);

                    /* Operand value unknown — result is also unknown. */
                    if (!flow_set_object_signed(ctx, &p_expression->object, ANY_VALUE, FLOW_RELATION_ANY, p_expression->first_token)) throw;
                }
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
            break;

            case EXPR_UNARY_NOT:
            {
                _Assert(p_expression->right != NULL);

                /*
                * Visit the child first so that any sub-expression is fully evaluated
                * and its constant value — if any — is propagated into
                * p_expression->right->object before we inspect it.
                */
                struct flow_true_false_branches child = flow_visit_condition(ctx, p_expression->right);

                if (object_has_constant_value(&p_expression->right->object))
                {
                    /* Both sub-expression and its value are now known: fold directly. */
                    const long long rv = object_to_signed_long_long(&p_expression->right->object);
                    const long long result = rv ? 0 : 1;
                    if (!flow_set_object_signed(ctx, &p_expression->object, result, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
                    flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                    if (result != 0)
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_NOT_DEAD_FALSE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_true_false_branches)
                        {
                        ctx->p_current_flow_branch, p_dead
                        };
                    }
                    else
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_NOT_DEAD_TRUE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_true_false_branches)
                        {
                        p_dead, ctx->p_current_flow_branch
                        };
                    }
                }

                /*
                    Seed the NOT result's OWN value: `!x` yields a boolean (0 or 1) and is
                    always INITIALIZED. Without this, `bool c = !x;` (non-constant x) left
                    c with no value and c was wrongly reported "possibly uninitialized".
                */
                if (!flow_set_object_signed(ctx, &p_expression->object, ANY_VALUE, FLOW_RELATION_ANY, p_expression->first_token)) throw;

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                /* NOT swaps the two branches. */
                return (struct flow_true_false_branches)
                {
                child.p_false, child.p_true
                };
            }

            case EXPR_CHECKED:
            {
                _Assert(p_expression->left != NULL);
                struct flow_true_false_branches checked_pair = flow_visit_expression(ctx, p_expression->left);
                flow_exit_block_visit_defer_list(ctx, &p_expression->defer_list, p_expression->first_token);
                flow_defer_list_set_end_of_lifetime(ctx, &p_expression->defer_list, p_expression->first_token);
                return checked_pair;
            }

            case EXPR_UNARY_SIZEOF_TYPE:
            case EXPR_UNARY_COUNTOF:
                /*
                    Constant when the parser folded it. For a VLA `sizeof` the parser
                    has no constant value, so this seeds nothing and it stays unknown.
                */
                flow_seed_constant_result(ctx, p_expression);
            break;

            case EXPR_UNARY_INCREMENT:
            case EXPR_UNARY_DECREMENT:
            case EXPR_POSTFIX_INCREMENT:
            case EXPR_POSTFIX_DECREMENT:
            {
                struct expression* _Opt p_operand = NULL;

                if (p_expression->expression_type == EXPR_UNARY_INCREMENT ||
                p_expression->expression_type == EXPR_UNARY_DECREMENT)
                {
                    _Assert(p_expression->right != NULL);
                    p_operand = p_expression->right;
                }
                else /* postfix */
                {
                    _Assert(p_expression->left != NULL);
                    p_operand = p_expression->left;
                }

                /* Evaluate the operand first. */
                /* keep the destination subscript's operand for flow_invalidate_unknown_index_write */
                const struct expression* _Opt p_previous_subscript_being_written = ctx->p_subscript_being_written;
                ctx->p_subscript_being_written = skip_parenthesis(p_operand);
                flow_visit_expression(ctx, p_operand);
                ctx->p_subscript_being_written = p_previous_subscript_being_written;

                /* ++v[i] / v[i]-- with an unknown i: no element of v keeps its value. */
                flow_invalidate_unknown_index_write(ctx, p_operand);

                bool is_postfix = (p_expression->expression_type == EXPR_POSTFIX_INCREMENT ||
                           p_expression->expression_type == EXPR_POSTFIX_DECREMENT);
                bool is_increment = (p_expression->expression_type == EXPR_UNARY_INCREMENT ||
                             p_expression->expression_type == EXPR_POSTFIX_INCREMENT);

                /*
                    ++/-- on an _Owner pointer loses the address to free: an error.
                    All four forms land here.
                */
                if (type_is_owner(&p_operand->object.type))
                {
                    diagnostic(is_increment
                       ? C_ERROR_FLOW_OPERATOR_INCREMENT_CANNOT_BE_USED_IN_OWNER
                       : C_ERROR_FLOW_OPERATOR_DECREMENT_CANNOT_BE_USED_IN_OWNER,
                       ctx->ctx,
                       p_operand->first_token, NULL,
                       is_increment
                       ? "operator '++' cannot be used on _Owner pointers"
                       : "operator '--' cannot be used on _Owner pointers");
                }

                /* Resolve the operand's object to its actual alternatives. */
                const struct object* p_obj = &p_operand->object;
                const struct flow_key_alternatives* _Opt p_entry =
                flow_branch_search_up(ctx->p_current_flow_branch, p_obj);
                if (p_entry == NULL)
                {
                    if (!flow_set_object_signed(ctx, p_obj, ANY_VALUE, FLOW_RELATION_ANY, p_expression->first_token)) throw;
                    if (!flow_set_object_signed(ctx, &p_expression->object, ANY_VALUE, FLOW_RELATION_ANY, p_expression->first_token)) throw;
                    break;
                }

                /*
                    Advance every object the operand may name (REF alternatives, e.g.
                    `(*p)++`), each tagged with its reference's branch.
                */
                struct flow_alternatives new_result_alts = { 0 };
                bool advanced_any = false;
                const struct marker operand_marker = expression_to_marker(p_operand);
                flow_findings_begin(ctx);

                for (int ri = 0; ri < p_entry->alternatives.size; ri++)
                {
                    const struct flow_alternative* ref = p_entry->alternatives.data[ri];
                    if (ref->value_kind != FLOW_VALUE_KIND_REF || ref->value.p == NULL)
                        continue;
                    advanced_any = true;

                    const struct object* p_actual_obj = ref->value.p;
                    const struct flow_key_alternatives* _Opt p_resolved =
                    flow_branch_search_up(ctx->p_current_flow_branch, p_actual_obj);
                    struct flow_alternatives new_var_alts = { 0 };
                    int n = p_resolved ? p_resolved->alternatives.size : 0;

                    if (n == 0)
                    {
                        struct flow_alternative a = { .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = ANY_VALUE}, .value_relation = FLOW_RELATION_ANY, .imaginary = FLOW_IMAGINARY_NONE, .p_origin_map = ref->p_origin_map, .p_origin_token = p_expression->first_token };
                        flow_alternatives_add(&new_var_alts, &a);
                        flow_alternatives_add(&new_result_alts, &a);
                    }

                    for (int i = 0; p_resolved != NULL && i < n; i++)
                    {
                        const struct flow_alternative* alt = p_resolved->alternatives.data[i];
                        flow_record_uninitialized_operand_value(ctx, p_operand, alt, &operand_marker);
                        const struct flow_branch* _Opt org = flow_origin_more_specific(alt->p_origin_map, ref->p_origin_map);

                        if (alt->value_relation == FLOW_RELATION_EQUAL &&
                         (alt->value_kind == FLOW_VALUE_KIND_SIGNED || alt->value_kind == FLOW_VALUE_KIND_UNSIGNED))
                        {
                            long long old = (alt->value_kind == FLOW_VALUE_KIND_SIGNED) ? alt->value.i : (long long)alt->value.u;
                            long long new_val = is_increment ? old + 1 : old - 1;
                            struct flow_alternative av = { .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = new_val}, .value_relation = FLOW_RELATION_EQUAL, .imaginary = FLOW_IMAGINARY_NONE, .p_origin_map = org, .p_origin_token = p_expression->first_token };
                            flow_alternatives_add(&new_var_alts, &av);
                            long long result_val = is_postfix ? old : new_val;
                            struct flow_alternative ar = { .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = result_val}, .value_relation = FLOW_RELATION_EQUAL, .imaginary = FLOW_IMAGINARY_NONE, .p_origin_map = org, .p_origin_token = p_expression->first_token };
                            flow_alternatives_add(&new_result_alts, &ar);
                        }
                        else if (alt->value_kind == FLOW_VALUE_KIND_PTR)
                        {
                            /*
                                Advancing a pointer keeps it non-null but moves it to another element:
                                repoint to a fresh ANY pointee, so facts about the old element (`*p == c`)
                                are dropped. A copy taken before the increment keeps the old pointee.
                            */
                            struct flow_alternative a = *alt;
                            if (alt->value_relation == FLOW_RELATION_EQUAL &&
                            alt->value.p != NULL &&
                            type_is_pointer(&p_operand->object.type))
                            {
                                struct object* _Opt p_fresh = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
                                if (p_fresh != NULL)
                                {
                                    struct type pointed_type = type_remove_pointer(&p_operand->object.type);
                                    make_object(&pointed_type, p_fresh, MAKE_STATE_ANY, &ctx->ctx->options.platform);
                                    type_destroy(&pointed_type);
                                    a.value.p = p_fresh;
                                }
                                else
                                {
                                    /*
                                        Can't allocate a fresh pointee: fall back to a
                                        generic non-null pointer (drops the stale value
                                        without inventing a bogus one).
                                    */
                                    a.value.p = NULL;
                                    a.value_relation = FLOW_RELATION_NOT_EQUAL;
                                }
                            }
                            flow_alternatives_add(&new_var_alts, &a);
                            flow_alternatives_add(&new_result_alts, &a);
                        }
                        else if (type_is_pointer(&p_operand->object.type))
                        {
                            /*
                                ANY pointer (not PTR-kind): decided by static type, like `p + n`.
                                null + 1 is UB, so the result is non-null, unknown target.
                            */
                            struct flow_alternative a =
                            {
                                .value_kind = FLOW_VALUE_KIND_PTR,
                                .value = {.p = NULL},
                                .value_relation = FLOW_RELATION_NOT_EQUAL,
                                .imaginary = FLOW_IMAGINARY_NONE,
                                .p_origin_map = org,
                                .p_origin_token = p_expression->first_token
                            };
                            flow_alternatives_add(&new_var_alts, &a);
                            flow_alternatives_add(&new_result_alts, &a);
                        }
                        else if (alt->value_kind == FLOW_VALUE_KIND_SIGNED &&
                                 alt->imaginary == FLOW_IMAGINARY_NONE &&
                                 (alt->value_relation == FLOW_RELATION_NOT_EQUAL ||
                                  alt->value_relation == FLOW_RELATION_GREATER ||
                                  alt->value_relation == FLOW_RELATION_GREATER_EQUAL ||
                                  alt->value_relation == FLOW_RELATION_LESS ||
                                  alt->value_relation == FLOW_RELATION_LESS_EQUAL) &&
                                 type_is_signed_integer(&p_operand->object.type))
                        {
                            /* signed overflow is UB, so `k >= c` becomes `k >= c+1` after ++ */
                            struct flow_alternative av = { .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = is_increment ? alt->value.i + 1 : alt->value.i - 1}, .value_relation = alt->value_relation, .imaginary = FLOW_IMAGINARY_NONE, .p_origin_map = org, .p_origin_token = p_expression->first_token };
                            flow_alternatives_add(&new_var_alts, &av);
                            struct flow_alternative ar = is_postfix ? *alt : av;
                            ar.p_origin_map = org;
                            ar.p_origin_token = p_expression->first_token;
                            flow_alternatives_add(&new_result_alts, &ar);
                        }
                        else
                        {
                            struct flow_alternative a = { .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = ANY_VALUE}, .value_relation = FLOW_RELATION_ANY, .imaginary = FLOW_IMAGINARY_NONE, .p_origin_map = org, .p_origin_token = p_expression->first_token };
                            flow_alternatives_add(&new_var_alts, &a);
                            flow_alternatives_add(&new_result_alts, &a);
                        }
                    }

                    struct flow_key_alternatives* _Opt p_var_entry = flow_branch_find_add(ctx->p_current_flow_branch, p_actual_obj);
                    if (p_var_entry == NULL)
                    {
                        flow_alternatives_clear(&new_var_alts);
                        flow_alternatives_clear(&new_result_alts);
                        throw;
                    }

                    flow_alternatives_clear(&p_var_entry->alternatives);
                    p_var_entry->alternatives = new_var_alts;
                }
                flow_findings_end(ctx);

                if (!advanced_any)
                {
                    struct flow_alternative a = { .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = ANY_VALUE}, .value_relation = FLOW_RELATION_ANY, .imaginary = FLOW_IMAGINARY_NONE, .p_origin_map = ctx->p_current_flow_branch, .p_origin_token = p_expression->first_token };
                    flow_alternatives_add(&new_result_alts, &a);
                }

                {
                    struct flow_key_alternatives* _Opt p_result_entry = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (p_result_entry == NULL)
                    {
                        flow_alternatives_clear(&new_result_alts);
                        throw;
                    }

                    flow_alternatives_clear(&p_result_entry->alternatives);
                    p_result_entry->alternatives = new_result_alts;
                }

                /* Remove the temporary operand object entry. */
                flow_branch_remove(ctx->p_current_flow_branch, &p_operand->object);
            }
            break;

            case EXPR_UNARY_BITNOT:
                _Assert(p_expression->right != NULL);
                /*
        * Visit the child first so that any sub-expression is fully evaluated
        * and its constant value — if any — is propagated into
        * p_expression->right->object before we inspect it.
        */
                flow_visit_expression(ctx, p_expression->right);
                if (object_has_constant_value(&p_expression->right->object))
                {
                    const long long rv = object_to_signed_long_long(&p_expression->right->object);
                    if (!flow_set_object_signed(ctx, &p_expression->object, ~rv, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
                }
                else
                {
                    /* Operand value unknown — result is also unknown. */
                    if (!flow_set_object_signed(ctx, &p_expression->object, ANY_VALUE, FLOW_RELATION_ANY, p_expression->first_token)) throw;
                }
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
            break;

            case EXPR_UNARY_ADDRESSOF:
            {
                _Assert(p_expression->right != NULL);
                flow_visit_expression(ctx, p_expression->right);

                struct flow_key_alternatives* _Opt result_entry = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                if (result_entry == NULL) throw;
                flow_alternatives_clear(&result_entry->alternatives);

                const struct flow_key_alternatives* _Opt p_right_alternatives = flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->right->object);
                if (p_right_alternatives)
                {

                    for (int i = 0; i < p_right_alternatives->alternatives.size; i++)
                    {
                        const struct flow_alternative* p_right_alternative = p_right_alternatives->alternatives.data[i];
                        if (p_right_alternative->value_relation == FLOW_RELATION_EQUAL &&
                        p_right_alternative->value_kind == FLOW_VALUE_KIND_REF &&
                        p_right_alternative->value.p != NULL)
                        {
                            {
                                struct flow_alternative a =
                                {
                                    .value_kind = FLOW_VALUE_KIND_PTR,
                                    .value = {.p = p_right_alternative->value.p},
                                    .value_relation = FLOW_RELATION_EQUAL,
                                    .imaginary = FLOW_IMAGINARY_NONE,
                                    .p_origin_map = ctx->p_current_flow_branch,
                                    .p_origin_token = p_expression->first_token
                                };
                                flow_alternatives_add(&result_entry->alternatives, &a);
                            }
                        }
                    }
                    flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                }
            }

            break;

            case EXPR_UNARY_CONTENT:
            {
                _Assert(p_expression->right != NULL);

                flow_visit_expression(ctx, p_expression->right);

                struct flow_key_alternatives* _Opt result_entry = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                if (result_entry == NULL) throw;
                flow_alternatives_clear(&result_entry->alternatives);
                struct marker marker = expression_to_marker(p_expression);

                const struct flow_key_alternatives* _Opt p_right_alternatives = flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->right->object);

                /* one report per dereference, with the count (see flow_finding_record) */
                flow_findings_begin(ctx);

                for (int i = 0; p_right_alternatives != NULL && i < p_right_alternatives->alternatives.size; i++)
                {
                    const struct flow_alternative* p_right_alt = p_right_alternatives->alternatives.data[i];


                    if (p_right_alt->value_kind == FLOW_VALUE_KIND_REF)
                    {
                        const struct flow_key_alternatives* _Opt p_right_alternatives2 = flow_branch_search_up(ctx->p_current_flow_branch,
                                                                                                        p_right_alt->value.p);

                        if (p_right_alternatives2 == NULL)
                            continue;

                        for (int j = 0; j < p_right_alternatives2->alternatives.size; j++)
                        {
                            const struct flow_alternative* p_right_alt2 = p_right_alternatives2->alternatives.data[j];

                            flow_record_uninitialized_operand_value(ctx, p_expression->right, p_right_alt2, &marker);

                            /*
                                Lifetime check on `*p` after the pointee was freed/moved, like
                                `p->member`, but on the whole pointee.
                            */
                            int ended_line = 0;
                            const struct flow_branch* _Opt ended_origin = NULL;
                            if (p_right_alt2->value_kind == FLOW_VALUE_KIND_PTR &&
                            p_right_alt2->value.p != NULL &&
                            flow_object_leaves_in_state(ctx, p_right_alt2->value.p, FLOW_LEAF_ENDED,
                                                        p_right_alt2->p_origin_map, true, false, &ended_line, &ended_origin))
                            {
                                const struct flow_alternative ended = { .p_origin_map = ended_origin };
                                struct osstream name_ss = { 0 };
                                flow_expression_to_string(p_expression, &name_ss);
                                struct osstream ss = { 0 };
                                ss_fprintf(&ss, "lifetime of the object pointed to by '%s' has ended (see line %d)",
                                           name_ss.c_str ? name_ss.c_str : "", ended_line);
                                flow_finding_record(ctx, &p_expression->object, W_FLOW_LIFETIME_ENDED, &marker,
                                                    &ended, ended_origin, ss.c_str ? ss.c_str : "");
                                ss_close(&ss);
                                ss_close(&name_ss);

                                /*
                                    If this dereference is also an assignment/return/argument source, don't
                                    report it twice (see the field comment in flow3.h).
                                */
                                ctx->p_pending_ended_report_obj = p_right_alt2->value.p;
                                ctx->pending_ended_report_line = ended_line;
                            }

                            /* without default_nonnull an unknown (ANY) pointer is not a null to report, only a null the flow has seen */
                            if (flow_alternative_can_be_zero(p_right_alt2) &&
                            (options_default_nonnull(&ctx->ctx->options) || p_right_alt2->value_relation != FLOW_RELATION_ANY) &&
                            !ctx->expression_is_not_evaluated &&
                            flow_origins_compatible(p_right_alt2->p_origin_map, ctx->p_current_flow_branch))
                            {
                                /*
                                    Unevaluated contexts never dereference. A null value whose origin
                                    conflicts with the current path cannot occur here.
                                */
                                        /* Name the whole dereference ('*p') in the diagnostic. */
                                struct osstream name_ss = { 0 };
                                flow_expression_to_string(p_expression, &name_ss);
                                struct osstream ss = { 0 };
                                ss_fprintf(&ss, "possible null pointer dereference '%s'", name_ss.c_str ? name_ss.c_str : "");
                                flow_finding_record(ctx, &p_expression->object, W_FLOW_NULL_DEREFERENCE, &marker,
                                                    p_right_alt2, p_right_alt2->p_origin_map,
                                                    ss.c_str ? ss.c_str : "");
                                ss_close(&ss);
                                ss_close(&name_ss);
                            }

                            /*
                                Only a PTR alternative with a known target gives a REF; otherwise the
                                result is seeded ANY below.
                            */
                            if (p_right_alt2->value_kind == FLOW_VALUE_KIND_PTR &&
                            p_right_alt2->value.p != NULL)
                            {
                                struct flow_alternative a =
                                {
                            .value_kind = FLOW_VALUE_KIND_REF,
                            .value = {.p = p_right_alt2->value.p},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                                /*
                                    Carry the pointer value's branch origin so a deref
                                    stays correlated: `p = &a@then / &b@else` gives
                                    `*p = ref a@then / ref b@else`.
                                */
                            .p_origin_map = p_right_alt2->p_origin_map,
                            .p_origin_token = p_expression->first_token
                                };
                                flow_alternatives_add(&result_entry->alternatives, &a);
                            }
                        }
                    }
                    else
                    {
                        /* *0 ? */
                        /* *(p ++) */
                    }
                }

                flow_findings_end(ctx);

                /*
                    Nothing resolved (`*get()`, `**pp`): seed ANY, as for an unresolved
                    subscript. No alternatives would leave the destination at its old value.
                */
                if (result_entry->alternatives.size == 0)
                {
                    if (type_is_integer(&p_expression->object.type))
                    {
                        struct flow_alternative a =
                        {
                    .value_kind = type_is_signed(&p_expression->object.type)
                                  ? FLOW_VALUE_KIND_SIGNED : FLOW_VALUE_KIND_UNSIGNED,
                    .value_relation = FLOW_RELATION_ANY,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&result_entry->alternatives, &a);
                    }
                    else if (type_is_pointer(&p_expression->object.type) &&
                     options_default_nonnull(&ctx->ctx->options) &&
                     !type_is_nullable(&p_expression->object.type, options_default_nonnull(&ctx->ctx->options)))
                    {
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_PTR,
                            .value = {.p = NULL},
                            .value_relation = FLOW_RELATION_NOT_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&result_entry->alternatives, &a);
                    }
                }

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
            }
            break;

            case EXPR_ASSIGNMENT_ASSIGN:
            {
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                /* Evaluate both sides (side effects, constant folding) */
                /* keep the destination subscript's operand for flow_invalidate_unknown_index_write */
                const struct expression* _Opt p_previous_subscript_being_written = ctx->p_subscript_being_written;
                ctx->p_subscript_being_written = skip_parenthesis(p_expression->left);
                flow_visit_expression(ctx, p_expression->left);
                ctx->p_subscript_being_written = p_previous_subscript_being_written;
                flow_visit_expression(ctx, p_expression->right);

                flow_check_assigment(ctx, p_expression->left, p_expression->right);

                /* v[i] = x with an unknown i: no element of v keeps its value. */
                flow_invalidate_unknown_index_write(ctx, p_expression->left);

                /* Seed an assignment EXPRESSION's own object (not just its ->left) as a REF to the resolved destination, so a later lookup on the assignment's own node -- e.g. `while ((dp = readdir(dir)) != NULL)` -- finds the narrowed value. */
                {
                    const struct object* p_dest_obj = &p_expression->left->object;
                    const struct flow_key_alternatives* _Opt p_dest_alts =
                    flow_branch_search_up(ctx->p_current_flow_branch, p_dest_obj);
                    if (p_dest_alts &&
                    p_dest_alts->alternatives.size == 1 &&
                    p_dest_alts->alternatives.data[0]->value_relation == FLOW_RELATION_EQUAL &&
                    p_dest_alts->alternatives.data[0]->value_kind == FLOW_VALUE_KIND_REF &&
                    p_dest_alts->alternatives.data[0]->value.p != NULL)
                    {
                        p_dest_obj = p_dest_alts->alternatives.data[0]->value.p;
                    }

                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (e == NULL) throw;
                    flow_alternatives_clear(&e->alternatives);
                    struct flow_alternative a =
                    {
                        .value_kind = FLOW_VALUE_KIND_REF,
                        .value = {.p = p_dest_obj},
                        .value_relation = FLOW_RELATION_EQUAL,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = ctx->p_current_flow_branch,
                        .p_origin_token = p_expression->first_token
                    };
                    flow_alternatives_add(&e->alternatives, &a);
                }

                /* ---- Branch maps for boolean context ---- */
                struct flow_branch* _Opt p_true = flow_narrow_map_branch(ctx, NULL, &ctx->flow_branch_arena,
                                                                  ctx->p_current_flow_branch,
                                                                  &p_expression->left->object,
                                                              true,
                                                                  p_expression,
                                                                  p_expression->first_token);
                if (p_true == NULL)
                    throw;

                struct flow_branch* _Opt p_false = flow_narrow_map_branch(ctx, NULL, &ctx->flow_branch_arena,
                                                                   ctx->p_current_flow_branch,
                                                                   &p_expression->left->object,
                                                               false,
                                                                   p_expression,
                                                                   p_expression->first_token);
                if (p_false == NULL)
                    throw;

                flow_tag_branch_pair(p_true, p_false);

                /* Remove temporary entries for left and right sub‑expressions */
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);

                return (struct flow_true_false_branches)
                {
                p_true, p_false
                };
            }

            case EXPR_ASSIGNMENT_PLUS_ASSIGN:
            case EXPR_ASSIGNMENT_MINUS_ASSIGN:
            case EXPR_ASSIGNMENT_MULTI_ASSIGN:
            case EXPR_ASSIGNMENT_DIV_ASSIGN:
            case EXPR_ASSIGNMENT_MOD_ASSIGN:
            case EXPR_ASSIGNMENT_SHIFT_LEFT_ASSIGN:
            case EXPR_ASSIGNMENT_SHIFT_RIGHT_ASSIGN:
            case EXPR_ASSIGNMENT_AND_ASSIGN:
            case EXPR_ASSIGNMENT_OR_ASSIGN:
            case EXPR_ASSIGNMENT_NOT_ASSIGN:
            {
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                /* keep the destination subscript's operand for flow_invalidate_unknown_index_write */
                const struct expression* _Opt p_previous_subscript_being_written = ctx->p_subscript_being_written;
                ctx->p_subscript_being_written = skip_parenthesis(p_expression->left);
                struct flow_true_false_branches lhs_pair2 = flow_visit_expression(ctx, p_expression->left);
                ctx->p_subscript_being_written = p_previous_subscript_being_written;
                flow_visit_expression(ctx, p_expression->right);

                /* v[i] op= x with an unknown i: no element of v keeps its value. */
                flow_invalidate_unknown_index_write(ctx, p_expression->left);

                /*
                    Fold per LHS alternative, keeping origins (`a += 10` on {1,3} -> {11,13}).
                    Pointers stay as-is. If any can't be folded, the destination is ANY.
                */
                const bool rhs_known = object_has_known_value(&p_expression->right->object);
                const signed long long rv =
                rhs_known ? object_to_signed_long_long(&p_expression->right->object) : 0;

                /*
                    The destination is reached through the left side's REF alternatives,
                    like `=` (`p->x += 10`). Each target folds its own alternatives.
                */
                const struct flow_key_alternatives* _Opt p_left_expression_entry =
                    flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->left->object);
                const int left_alternatives_count =
                    p_left_expression_entry != NULL ? p_left_expression_entry->alternatives.size : 0;

                /*
                    several possible targets: each is folded and written in its own
                    arm, see flow_branch_new_pointer_target_arm
                */
                const struct flow_branch* _Opt target_origins[16] = { 0 };
                int target_count = 0;
                for (int left_index = 0; p_left_expression_entry != NULL && left_index < left_alternatives_count; left_index++)
                {
                    if (p_left_expression_entry->alternatives.data[left_index]->value_kind == FLOW_VALUE_KIND_REF &&
                            p_left_expression_entry->alternatives.data[left_index]->value.p != NULL)
                    {
                        if (target_count < (int)(sizeof target_origins / sizeof target_origins[0]))
                        {
                            target_origins[target_count] = p_left_expression_entry->alternatives.data[left_index]->p_origin_map;
                        }
                        target_count++;
                    }
                }

                struct flow_branch* _Opt p_before_write = ctx->p_current_flow_branch;
                const struct flow_branch* _Opt target_arms[16] = { 0 };
                const bool write_in_arms = p_before_write != NULL &&
                    target_count > 1 &&
                    target_count <= (int)(sizeof target_arms / sizeof target_arms[0]) &&
                    flow_target_origins_exclusive(target_origins, target_count);
                int target_arm_count = 0;

                for (int left_index = 0; left_index < left_alternatives_count; left_index++)
                {
                    /* search again: flow_branch_find_add below may move the entries */
                    p_left_expression_entry =
                        flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->left->object);
                    if (p_left_expression_entry == NULL ||
                            left_index >= p_left_expression_entry->alternatives.size)
                    {
                        break;
                    }

                    const struct flow_alternative* p_left_alternative =
                        p_left_expression_entry->alternatives.data[left_index];
                    if (p_left_alternative->value_kind != FLOW_VALUE_KIND_REF ||
                            p_left_alternative->value.p == NULL)
                    {
                        continue;
                    }

                    const struct object* p_dest_obj = p_left_alternative->value.p;

                    struct flow_branch* _Opt p_target_arm = NULL;
                    if (write_in_arms && p_before_write != NULL)
                    {
                        p_target_arm = flow_branch_new_pointer_target_arm(ctx, p_before_write, p_left_alternative->p_origin_map);
                        if (p_target_arm == NULL)
                        {
                            /* never this object on this path */
                            continue;
                        }
                        ctx->p_current_flow_branch = p_target_arm;
                    }

                    struct flow_key_alternatives* _Opt p_lhs_entry =
                        flow_branch_search_up(ctx->p_current_flow_branch, p_dest_obj);

                    struct flow_alternatives new_alts = { 0 };
                    bool all_handled = (p_lhs_entry != NULL && p_lhs_entry->alternatives.size > 0);

                    for (int i = 0; all_handled && p_lhs_entry != NULL && i < p_lhs_entry->alternatives.size; i++)
                    {
                        const struct flow_alternative* la = p_lhs_entry->alternatives.data[i];

                        if (la->value_kind == FLOW_VALUE_KIND_PTR)
                        {
                            struct flow_alternative a = *la; /* keep pointer alternative */
                            flow_alternatives_add(&new_alts, &a);
                        }
                        else if (rhs_known &&
                         la->value_relation == FLOW_RELATION_EQUAL &&
                         (la->value_kind == FLOW_VALUE_KIND_SIGNED ||
                             la->value_kind == FLOW_VALUE_KIND_UNSIGNED))
                        {
                            const signed long long lv =
                            la->value_kind == FLOW_VALUE_KIND_SIGNED
                            ? la->value.i
                            : (signed long long)la->value.u;
                            signed long long result = lv;

                            switch (p_expression->expression_type)
                            {
                                case EXPR_ASSIGNMENT_PLUS_ASSIGN:
                                    result = lv + rv;
                                break;
                                case EXPR_ASSIGNMENT_MINUS_ASSIGN:
                                    result = lv - rv;
                                break;
                                case EXPR_ASSIGNMENT_MULTI_ASSIGN:
                                    result = lv * rv;
                                break;
                                case EXPR_ASSIGNMENT_DIV_ASSIGN:
                                    result = rv != 0 ? lv / rv : lv;
                                break;
                                case EXPR_ASSIGNMENT_MOD_ASSIGN:
                                    result = rv != 0 ? lv % rv : lv;
                                break;
                                case EXPR_ASSIGNMENT_SHIFT_LEFT_ASSIGN:
                                    result = lv << rv;
                                break;
                                case EXPR_ASSIGNMENT_SHIFT_RIGHT_ASSIGN:
                                    result = lv >> rv;
                                break;
                                case EXPR_ASSIGNMENT_AND_ASSIGN:
                                    result = lv & rv;
                                break;
                                case EXPR_ASSIGNMENT_OR_ASSIGN:
                                    result = lv | rv;
                                break;
                                case EXPR_ASSIGNMENT_NOT_ASSIGN:
                                    result = lv ^ rv;
                                break;
                                default:
                                break;
                            }

                            struct flow_alternative a =
                            {
                                .value_kind = FLOW_VALUE_KIND_SIGNED,
                                .value = {.i = result},
                                .value_relation = FLOW_RELATION_EQUAL,
                                .imaginary = FLOW_IMAGINARY_NONE,
                                .p_origin_map = la->p_origin_map,
                                .p_origin_token = p_expression->right->first_token
                            };
                            flow_alternatives_add(&new_alts, &a);
                        }
                        else if (rhs_known &&
                                 (p_expression->expression_type == EXPR_ASSIGNMENT_PLUS_ASSIGN ||
                                  p_expression->expression_type == EXPR_ASSIGNMENT_MINUS_ASSIGN) &&
                                 type_is_signed_integer(&p_expression->left->object.type) &&
                                 la->value_kind == FLOW_VALUE_KIND_SIGNED &&
                                 la->imaginary == FLOW_IMAGINARY_NONE &&
                                 (la->value_relation == FLOW_RELATION_NOT_EQUAL ||
                                  la->value_relation == FLOW_RELATION_GREATER ||
                                  la->value_relation == FLOW_RELATION_GREATER_EQUAL ||
                                  la->value_relation == FLOW_RELATION_LESS ||
                                  la->value_relation == FLOW_RELATION_LESS_EQUAL))
                        {
                            /* signed overflow is UB, so `k >= c` gives `k += r` >= c + r */
                            struct flow_alternative a =
                            {
                                .value_kind = FLOW_VALUE_KIND_SIGNED,
                                .value = {.i = p_expression->expression_type == EXPR_ASSIGNMENT_PLUS_ASSIGN ? la->value.i + rv : la->value.i - rv},
                                .value_relation = la->value_relation,
                                .imaginary = FLOW_IMAGINARY_NONE,
                                .p_origin_map = la->p_origin_map,
                                .p_origin_token = p_expression->right->first_token
                            };
                            flow_alternatives_add(&new_alts, &a);
                        }
                        else if (p_expression->left != NULL &&
                         type_is_pointer(&p_expression->left->object.type))
                        {
                            /* Like ++/--: `p += n` is never null; decided by static type. */
                            struct flow_alternative a =
                            {
                                .value_kind = FLOW_VALUE_KIND_PTR,
                                .value = {.p = NULL},
                                .value_relation = FLOW_RELATION_NOT_EQUAL,
                                .imaginary = FLOW_IMAGINARY_NONE,
                                .p_origin_map = la->p_origin_map,
                                .p_origin_token = p_expression->right->first_token
                            };
                            flow_alternatives_add(&new_alts, &a);
                        }
                        else
                        {
                            all_handled = false;
                        }
                    }

                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, p_dest_obj);
                        if (e == NULL)
                        {
                            flow_alternatives_clear(&new_alts);
                            throw;
                        }

                        flow_alternatives_clear(&e->alternatives);
                        if (all_handled && new_alts.size > 0)
                        {
                            e->alternatives = new_alts; /* move */
                        }
                        else
                        {
                            flow_alternatives_clear(&new_alts);
                            struct flow_alternative a =
                            {
                                .value_kind = FLOW_VALUE_KIND_SIGNED,
                                .value = {.i = ANY_VALUE},
                                .value_relation = FLOW_RELATION_ANY,
                                .imaginary = FLOW_IMAGINARY_NONE,
                                .p_origin_map = ctx->p_current_flow_branch,
                                .p_origin_token = p_expression->right->first_token
                            };
                            flow_alternatives_add(&e->alternatives, &a);
                        }
                    }

                    if (p_target_arm != NULL && p_before_write != NULL)
                    {
                        flow_branch_tag_pointer_target_arm(p_target_arm);
                        ctx->p_current_flow_branch = p_before_write;
                        target_arms[target_arm_count] = p_target_arm;
                        target_arm_count++;
                    }
                }

                if (p_before_write != NULL && target_arm_count > 0)
                {
                    flow_branch_merge_arms(p_before_write, target_arms, target_arm_count);
                }

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                return lhs_pair2;
            }
            case EXPR_MULTIPLICATIVE_MULT:
            case EXPR_MULTIPLICATIVE_DIV:
            case EXPR_MULTIPLICATIVE_MOD:
            {
                char op = 0;
                switch (p_expression->expression_type)
                {
                    case EXPR_MULTIPLICATIVE_MULT:
                        op = '*';
                    break;
                    case EXPR_MULTIPLICATIVE_DIV:
                        op = '/';
                    break;
                    case EXPR_MULTIPLICATIVE_MOD:
                        op = '%';
                    break;
                    default:
                    break;
                }

                _Assert(p_expression->left != NULL);
                _Assert(p_expression->right != NULL);

                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);

                flow_evaluate_binary_arithmetic(ctx,
                                            p_expression->left,
                                            p_expression->right,
                                            p_expression,
                                            op);

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                break;
            }

            case EXPR_ADDITIVE_PLUS:
            case EXPR_ADDITIVE_MINUS:
            {
                char op = 0;
                switch (p_expression->expression_type)
                {
                    case EXPR_ADDITIVE_PLUS:
                        op = '+';
                    break;
                    case EXPR_ADDITIVE_MINUS:
                        op = '-';
                    break;
                    default:
                    break;
                }

                _Assert(p_expression->left != NULL);
                _Assert(p_expression->right != NULL);

                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);

                flow_evaluate_binary_arithmetic(ctx,
                                            p_expression->left,
                                            p_expression->right,
                                            p_expression,
                                            op);

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                break;
            }

            case EXPR_CAST:
            {
                _Assert(p_expression->left != NULL);

                flow_visit_expression(ctx, p_expression->left);

                const struct type* p_target_type = &p_expression->object.type;

                /*
                    Casting a temporary owner to a non-owner (`(int*) malloc(1)`) loses
                    the ownership: an error.
                */
                if ((p_expression->left->object.type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN) &&
                type_is_owner(&p_expression->left->object.type) &&
                !type_is_owner(p_target_type))
                {
                    diagnostic(W_FLOW_DISCARDING_OWNER,
                           ctx->ctx,
                           p_expression->first_token, NULL,
                           type_is_pointer(&p_expression->left->object.type)
                       ? "discarding _Owner pointer"
                       : "discarding _Owner");
                }

                if (type_is_owner(&p_expression->left->object.type) && type_is_owner(p_target_type))
                {
                    /*
                        Owner-to-owner cast keeps identity: REF to the operand's object, so a
                        later move marks the original (owner-cast-move-through-member.c).
                    */
                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (e == NULL) throw;
                    flow_alternatives_clear(&e->alternatives);
                    struct flow_alternative a =
                    {
                        .value_kind = FLOW_VALUE_KIND_REF,
                        .value = {.p = &p_expression->left->object},
                        .value_relation = FLOW_RELATION_EQUAL,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = ctx->p_current_flow_branch,
                        .p_origin_token = p_expression->first_token
                    };
                    flow_alternatives_add(&e->alternatives, &a);
                    break;
                }

                const struct object* p_src_obj = &p_expression->left->object;
                const struct flow_key_alternatives* _Opt p_src_entry =
                flow_branch_search_up(ctx->p_current_flow_branch, p_src_obj);

                if (p_src_entry == NULL)
                {
                    if (!flow_set_object_signed(ctx, &p_expression->object, ANY_VALUE, FLOW_RELATION_ANY, p_expression->first_token)) throw;
                    break;
                }

                struct flow_alternatives new_alts = { 0 };
                bool all_handled = true;

                /*
                    Cast every source value. Iterate REF alternatives per-alternative
                    (an operand can alias several objects) instead of a size==1 /
                    data[0] shortcut, and keep each value's branch origin so the cast
                    stays correlated.
                */
                for (int i = 0; all_handled && i < p_src_entry->alternatives.size; i++)
                {
                    const struct flow_alternative* src_alt = p_src_entry->alternatives.data[i];
                    if (src_alt->value_kind == FLOW_VALUE_KIND_REF && src_alt->value.p != NULL)
                    {
                        const struct flow_key_alternatives* _Opt resolved =
                        flow_branch_search_up(ctx->p_current_flow_branch, src_alt->value.p);
                        if (resolved == NULL)
                        {
                            all_handled = false;
                            break;
                        }

                        for (int j = 0; j < resolved->alternatives.size; j++)
                        {
                            const struct flow_alternative* v = resolved->alternatives.data[j];
                            if (!flow_cast_one_value(ctx, v, p_target_type, &new_alts,
                                                 flow_origin_more_specific(v->p_origin_map, src_alt->p_origin_map),
                                                 p_expression->first_token))
                            {
                                all_handled = false;
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (!flow_cast_one_value(ctx, src_alt, p_target_type, &new_alts,
                                             src_alt->p_origin_map, p_expression->first_token))
                        {
                            all_handled = false;
                            break;
                        }
                    }
                }

                if (all_handled && new_alts.size > 0)
                {
                    struct flow_key_alternatives* _Opt dst = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (dst == NULL)
                    {
                        flow_alternatives_clear(&new_alts);
                        throw;
                    }

                    flow_alternatives_clear(&dst->alternatives);
                    dst->alternatives = new_alts; /* move */
                }
                else
                {
                    flow_alternatives_clear(&new_alts);
                    if (!flow_set_object_signed(ctx, &p_expression->object, ANY_VALUE, FLOW_RELATION_ANY, p_expression->first_token)) throw;
                }

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                break;
            }

            case EXPR_SHIFT_RIGHT:
            case EXPR_SHIFT_LEFT:
            {
                _Assert(p_expression->left != NULL);
                _Assert(p_expression->right != NULL);

                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);

                /*
                    Fold across all alternatives (per-alternative REF resolution and
                    join correlation), like the other binary arithmetic operators --
                    no size==1 / data[0] shortcut.
                */
                flow_evaluate_binary_arithmetic(ctx, p_expression->left, p_expression->right,
                                            p_expression,
                                            (p_expression->expression_type == EXPR_SHIFT_LEFT) ? '<' : '>');

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                break;
            }


            case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
            case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
            case EXPR_RELATIONAL_BIGGER_THAN:
            case EXPR_RELATIONAL_LESS_THAN:
            {
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);

                /*
                    Seed the per-path boolean result (or ANY) so an enclosing
                    ||/&& / compile_assert can reason per path, and fold it across
                    ALL alternatives of both operands.
                */
                int fold_result = flow_seed_comparison_result(ctx, p_expression);

                if (fold_result != -1)
                {
                    /* Expression always true or always false */
                    if (!flow_set_object_signed(ctx, &p_expression->object, fold_result ? 1 : 0, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
                    if (ctx->p_current_flow_branch == NULL)
                    {
                        throw;
                    }

                    struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena,
                                                                       ctx->p_current_flow_branch,
                                                                       FLOW_BRANCH_REL_DEAD);
                    if (p_dead == NULL)
                        throw;

                    return (struct flow_true_false_branches)
                    {
                    fold_result ? ctx->p_current_flow_branch : p_dead,
                        fold_result ? p_dead : ctx->p_current_flow_branch
                    };
                }

                /*
                    Scalar variable vs constant: narrow it on each branch (true: var OP c,
                    false: var !OP c), also for _Assert.
                */
                {
                    long long cst = 0;
                    const struct expression* _Opt p_var_expr = NULL;
                    enum expression_type narrow_op = p_expression->expression_type;

                    if (flow_operand_is_single_constant(ctx, p_expression->right, &cst))
                    {
                        /* var OP const */
                        p_var_expr = p_expression->left;
                    }
                    else if (flow_operand_is_single_constant(ctx, p_expression->left, &cst))
                    {
                        /* const OP var  ==  var swapped(OP) const */
                        p_var_expr = p_expression->right;
                        narrow_op = flow_swap_relational(p_expression->expression_type);
                    }

                    if (p_var_expr)
                    {
                        struct flow_branch* _Opt p_true = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, true, p_expression);
                        struct flow_branch* _Opt p_false = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, false, p_expression);
                        if (p_true == NULL || p_false == NULL)
                            throw;

                        flow_tag_branch_pair(p_true, p_false);
                        flow_narrow_operand(ctx, p_var_expr, cst, narrow_op,
                                            p_true, p_false, p_expression->first_token);
                        
                        return (struct flow_true_false_branches) { p_true, p_false };
                    }
                }
                return (struct flow_true_false_branches)
                {
                ctx->p_current_flow_branch, ctx->p_current_flow_branch
                };
            }

            case EXPR_EQUALITY_NOT_EQUAL:
            case EXPR_EQUALITY_EQUAL:
            {
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);

                /*
                    Fold across ALL alternatives of both operands. A constant is simply
                    an operand with a single alternative -- no special case.
                */
                int fold = flow_seed_comparison_result(ctx, p_expression);
                if (fold != -1)
                {
                    if (!flow_set_object_signed(ctx, &p_expression->object, fold ? 1 : 0, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
                    if (ctx->p_current_flow_branch == NULL)
                    {
                        throw;
                    }

                    struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena,
                                                                       ctx->p_current_flow_branch,
                                                                       FLOW_BRANCH_EQ_DEAD);
                    if (p_dead == NULL)
                        throw;

                    return (struct flow_true_false_branches)
                    {
                    fold ? ctx->p_current_flow_branch : p_dead,
                        fold ? p_dead : ctx->p_current_flow_branch
                    };
                }

                /*
                    Not foldable: if one operand is a single constant, narrow the other
                    on each branch. (A constant naturally collapses to one value across
                    its alternatives.)
                */
                long long cst = 0;
                const struct expression* _Opt p_var_expr = NULL;
                if (flow_operand_is_single_constant(ctx, p_expression->right, &cst))
                    p_var_expr = p_expression->left;
                else if (flow_operand_is_single_constant(ctx, p_expression->left, &cst))
                    p_var_expr = p_expression->right;

                if (p_var_expr)
                {
                    struct flow_branch* _Opt p_true = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, true, p_expression);
                    struct flow_branch* _Opt p_false = flow_branch_arena_new_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, false, p_expression);
                    if (p_true == NULL || p_false == NULL)
                        throw;

                    flow_tag_branch_pair(p_true, p_false);
                    flow_narrow_operand(ctx, p_var_expr, cst, p_expression->expression_type,
                                        p_true, p_false, p_expression->first_token);
                    return (struct flow_true_false_branches)
                    {
                    p_true, p_false
                    };
                }

                /* -------- Fallback: unknown -------- */
                return (struct flow_true_false_branches)
                {
                ctx->p_current_flow_branch, ctx->p_current_flow_branch
                };
            }
            /* break; */

            case EXPR_LOGICAL_OR:
            {
                /*
                    L || R
                    true  = merge(left_true, right_true_from_left_false)
                    false = right_false_from_left_false
                */
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                if (object_has_constant_value(&p_expression->left->object) &&
                object_has_constant_value(&p_expression->right->object))
                {
                    const long long result = (object_to_signed_long_long(&p_expression->left->object) ||
                                      object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
                    if (!flow_set_object_signed(ctx, &p_expression->object, result, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
                    if (result != 0)
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_OR_DEAD_FALSE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_true_false_branches)
                        {
                        ctx->p_current_flow_branch, p_dead
                        };
                    }
                    else
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_OR_DEAD_TRUE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_true_false_branches)
                        {
                        p_dead, ctx->p_current_flow_branch
                        };
                    }
                }

                struct flow_true_false_branches left_pair = flow_visit_condition(ctx, p_expression->left);

                /* Visit right on the false map of left (right only runs when left is false). */
                /*
                    as for an `if`: in `p == 0 || p->x`, the right side is on the
                    side of earlier branches that p's non-null values come from
                */
                if (left_pair.p_true != NULL && left_pair.p_false != NULL &&
                    left_pair.p_true != left_pair.p_false)
                {
                    flow_branch_derive_implied_facts(left_pair.p_false, p_before);
                }
                ctx->p_current_flow_branch = left_pair.p_false;
                struct flow_true_false_branches right_pair = flow_visit_condition(ctx, p_expression->right);
                ctx->p_current_flow_branch = p_before;

                /*
                * true  = merge(left_true, right_true)
                * false = right_false
                */
                struct flow_branch* _Opt p_or_true = flow_branch_arena_new_branch(&ctx->flow_branch_arena, p_before, true, p_expression);
                if (p_or_true == NULL)
                    throw;

                if (left_pair.p_true != NULL && right_pair.p_true != NULL)
                {
                    flow_branch_merge_a_b(p_or_true, left_pair.p_true, right_pair.p_true);
                }

                /*
                    Per-path value of `L || R`: L if true there, else R. Only when both are
                    clean per-path booleans; otherwise unseeded.
                */
                {
                    const struct flow_key_alternatives* _Opt p_left_entry =
                    flow_branch_search_up(p_before, &p_expression->left->object);
                    const struct flow_key_alternatives* _Opt p_right_entry =
                    flow_branch_search_up(left_pair.p_false, &p_expression->right->object);

                    struct flow_alternatives out = { 0 };
                    bool ok = (p_left_entry != NULL && p_right_entry != NULL);

                    for (int i = 0; ok && p_left_entry != NULL && p_right_entry != NULL && i < p_left_entry->alternatives.size; i++)
                    {
                        const struct flow_alternative* left_alt = p_left_entry->alternatives.data[i];
                        if (flow_alternative_is_dead(left_alt))
                            continue;

                        const int left_truth = flow_alternative_truth(p_before, left_alt, 0);
                        if (left_truth == 1)
                        {
                            struct flow_alternative a =
                            {
                        .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = 1},
                        .value_relation = FLOW_RELATION_EQUAL, .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = left_alt->p_origin_map, .p_origin_token = p_expression->first_token
                            };
                            flow_alternatives_add(&out, &a);
                        }
                        else if (left_truth == 0)
                        {
                            bool matched = false;
                            for (int j = 0; j < p_right_entry->alternatives.size; j++)
                            {
                                const struct flow_alternative* right_alt = p_right_entry->alternatives.data[j];
                                if (flow_alternative_is_dead(right_alt))
                                    continue;

                                if (!flow_origins_compatible(left_alt->p_origin_map, right_alt->p_origin_map))
                                    continue;
                                const int right_truth = flow_alternative_truth(left_pair.p_false, right_alt, 0);
                                bool r_true = (right_truth == 1);
                                if (right_truth == -1)
                                {
                                    ok = false;
                                    break;
                                }
                                struct flow_alternative a =
                                {
                            .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = r_true ? 1 : 0},
                            .value_relation = FLOW_RELATION_EQUAL, .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = flow_origin_more_specific(left_alt->p_origin_map, right_alt->p_origin_map),
                            .p_origin_token = p_expression->first_token
                                };
                                flow_alternatives_add(&out, &a);
                                matched = true;
                            }
                            if (!matched) ok = false;
                        }
                        else
                        {
                            ok = false; /* p_left_entry not a clean boolean on this path */
                        }
                    }

                    if (ok && out.size > 0)
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL)
                        {
                            flow_alternatives_clear(&out);
                            throw;
                        }
                        flow_alternatives_clear(&e->alternatives);
                        e->alternatives = out; /* move */
                    }
                    else
                    {
                        flow_alternatives_clear(&out);

                        /*
                            Unseeded is not unchanged: empty the entry (not ANY) so a stale value
                            from an earlier evaluation is shadowed.
                        */
                        struct flow_key_alternatives* _Opt e =
                        flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e != NULL)
                        {
                            flow_alternatives_clear(&e->alternatives);
                        }
                    }
                }

                return (struct flow_true_false_branches)
                {
                p_or_true, right_pair.p_false
                };
            }

            case EXPR_LOGICAL_AND:
            {
                /*
                    L && R
                    true  = right_true_from_left_true
                    false = merge(left_false, right_false_from_left_true)
                */
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                if (object_has_constant_value(&p_expression->left->object) &&
                object_has_constant_value(&p_expression->right->object))
                {
                    const long long result = (object_to_signed_long_long(&p_expression->left->object) &&
                                      object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
                    if (!flow_set_object_signed(ctx, &p_expression->object, result, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
                    if (result != 0)
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_AND_DEAD_FALSE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_true_false_branches)
                        {
                        ctx->p_current_flow_branch, p_dead
                        };
                    }
                    else
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_AND_DEAD_TRUE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_true_false_branches)
                        {
                        p_dead, ctx->p_current_flow_branch
                        };
                    }
                }

                struct flow_true_false_branches left_pair = flow_visit_condition(ctx, p_expression->left);

                if (object_has_constant_value(&p_expression->left->object) &&
                object_is_true(&p_expression->left->object) == false)
                {
                    /* Left is always false: short-circuit, right never evaluated. */
                    return left_pair;
                }

                /* Visit right on the true map of left (right only runs when left is true). */
                /* as for an `if`: in `p && p->x`, see the `||` case */
                if (left_pair.p_true != NULL && left_pair.p_false != NULL &&
                    left_pair.p_true != left_pair.p_false)
                {
                    flow_branch_derive_implied_facts(left_pair.p_true, p_before);
                }
                ctx->p_current_flow_branch = left_pair.p_true;
                struct flow_true_false_branches right_pair = flow_visit_condition(ctx, p_expression->right);
                ctx->p_current_flow_branch = p_before;

                /*
                * false = merge(left_false, right_false)
                */
                struct flow_branch* _Opt p_and_false = flow_branch_arena_new_branch(&ctx->flow_branch_arena, p_before, false, p_expression);
                if (p_and_false == NULL)
                    throw;

                if (left_pair.p_false != NULL && right_pair.p_false != NULL)
                {
                    flow_branch_merge_a_b(p_and_false, left_pair.p_false, right_pair.p_false);
                }

                /*
                    Per-path value of `L && R` (dual of ||): 0 if L is false there, else R.
                    Only when both are clean per-path booleans; otherwise unseeded.
                */
                {
                    const struct flow_key_alternatives* _Opt p_left_entry =
                    flow_branch_search_up(p_before, &p_expression->left->object);
                    const struct flow_key_alternatives* _Opt p_right_entry =
                    flow_branch_search_up(left_pair.p_true, &p_expression->right->object);

                    struct flow_alternatives out = { 0 };
                    bool ok = (p_left_entry != NULL && p_right_entry != NULL);

                    for (int i = 0; ok && p_left_entry != NULL && p_right_entry != NULL && i < p_left_entry->alternatives.size; i++)
                    {
                        const struct flow_alternative* left_alt = p_left_entry->alternatives.data[i];
                        if (flow_alternative_is_dead(left_alt))
                            continue;

                        const int left_truth = flow_alternative_truth(p_before, left_alt, 0);
                        if (left_truth == 0)
                        {
                            struct flow_alternative a =
                            {
                        .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = 0},
                        .value_relation = FLOW_RELATION_EQUAL, .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = left_alt->p_origin_map, .p_origin_token = p_expression->first_token
                            };
                            flow_alternatives_add(&out, &a);
                        }
                        else if (left_truth == 1)
                        {
                            bool matched = false;
                            for (int j = 0; j < p_right_entry->alternatives.size; j++)
                            {
                                const struct flow_alternative* right_alt = p_right_entry->alternatives.data[j];
                                if (flow_alternative_is_dead(right_alt))
                                    continue;

                                if (!flow_origins_compatible(left_alt->p_origin_map, right_alt->p_origin_map))
                                    continue;
                                const int right_truth = flow_alternative_truth(left_pair.p_true, right_alt, 0);
                                bool r_true = (right_truth == 1);
                                if (right_truth == -1)
                                {
                                    ok = false;
                                    break;
                                }

                                struct flow_alternative a =
                                {
                                    .value_kind = FLOW_VALUE_KIND_SIGNED, .value = {.i = r_true ? 1 : 0},
                                    .value_relation = FLOW_RELATION_EQUAL, .imaginary = FLOW_IMAGINARY_NONE,
                                    .p_origin_map = flow_origin_more_specific(left_alt->p_origin_map, right_alt->p_origin_map),
                                    .p_origin_token = p_expression->first_token
                                };
                                flow_alternatives_add(&out, &a);
                                matched = true;
                            }
                            if (!matched) ok = false;
                        }
                        else
                        {
                            ok = false;
                        }
                    }

                    if (ok && out.size > 0)
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL)
                        {
                            flow_alternatives_clear(&out);
                            throw;
                        }
                        flow_alternatives_clear(&e->alternatives);
                        e->alternatives = out; /* move */
                    }
                    else
                    {
                        flow_alternatives_clear(&out);

                        /*
                            Unseeded is not unchanged: empty the entry (not ANY) so a stale value
                            from an earlier evaluation is shadowed.
                        */
                        struct flow_key_alternatives* _Opt e =
                        flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e != NULL)
                        {
                            flow_alternatives_clear(&e->alternatives);
                        }
                    }
                }

                return (struct flow_true_false_branches) { right_pair.p_true, p_and_false };
            }

            case EXPR_INCLUSIVE_OR:
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);
                if (object_has_constant_value(&p_expression->left->object) &&
                object_has_constant_value(&p_expression->right->object))
                {
                    const long long lv = object_to_signed_long_long(&p_expression->left->object);
                    const long long rv = object_to_signed_long_long(&p_expression->right->object);
                    if (!flow_set_object_signed(ctx, &p_expression->object, lv | rv, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
                    break;
                }
                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);
                if (!flow_set_object_signed(ctx, &p_expression->object, ANY_VALUE, FLOW_RELATION_ANY, p_expression->first_token)) throw;
            break;

            case EXPR_AND:
            case EXPR_EXCLUSIVE_OR:
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);
                if (object_has_constant_value(&p_expression->left->object) &&
                object_has_constant_value(&p_expression->right->object))
                {
                    const long long lv = object_to_signed_long_long(&p_expression->left->object);
                    const long long rv = object_to_signed_long_long(&p_expression->right->object);
                    const long long result = (p_expression->expression_type == EXPR_AND)
                    ? (lv & rv)
                    : (lv ^ rv);
                    if (!flow_set_object_signed(ctx, &p_expression->object, result, FLOW_RELATION_EQUAL, p_expression->first_token)) throw;
                    break;
                }
                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);
                if (!flow_set_object_signed(ctx, &p_expression->object, ANY_VALUE, FLOW_RELATION_ANY, p_expression->first_token)) throw;
            break;

            case EXPR_UNARY_TRAITS:
            break;

            case EXPR_UNARY_IS_SAME:
            break;

            case EXPR_UNARY_DECLARATOR_ATTRIBUTE:
            break;

            case EXPR_EXPRESSION:
            {
                _Assert(p_expression->left != NULL);
                _Assert(p_expression->right != NULL);
                flow_visit_expression(ctx, p_expression->left);

                /* The left operand's value is discarded: `(malloc(1), 0)` loses the owner. */
                if ((p_expression->left->object.type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN) &&
                    type_is_owner(&p_expression->left->object.type))
                {
                    diagnostic(W_FLOW_DISCARDING_OWNER,
                           ctx->ctx,
                           p_expression->left->first_token, NULL,
                           type_is_pointer(&p_expression->left->object.type)
                       ? "discarding _Owner pointer"
                       : "discarding _Owner");
                }

                /* Comma: the value (and branch state) of the right operand is what matters. */
                struct flow_true_false_branches pair = flow_visit_expression(ctx, p_expression->right);

                /*
                    Forward the right operand's value to the comma's own object
                    (`f((p = 0, p))`), as for parentheses.
                */
                const struct expression* p_inner = skip_parenthesis(p_expression->right);
                const struct flow_key_alternatives* _Opt p_inner_entry =
                flow_branch_search_up(ctx->p_current_flow_branch, &p_inner->object);
                if (p_inner_entry)
                {
                    struct flow_key_alternatives* _Opt p_entry = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (p_entry == NULL) throw;
                    flow_alternatives_clear(&p_entry->alternatives);
                    flow_alternatives_append(&p_entry->alternatives, &p_inner_entry->alternatives);
                }
                return pair;
            }

            case EXPR_CONDITIONAL:
            {
                _Assert(p_expression->condition_expr != NULL);
                _Assert(p_expression->right != NULL);

                struct flow_true_false_branches cond_pair = flow_visit_condition(ctx, p_expression->condition_expr);
                cond_pair = flow_ensure_branch_pair(ctx, ctx->p_current_flow_branch, cond_pair,
                                                p_expression->condition_expr);

                /* true branch */
                ctx->p_current_flow_branch = cond_pair.p_true;
                /* Elvis: left==NULL means use condition_expr value on true branch */
                struct expression* p_true_expr = p_expression->left
                    ? p_expression->left
                    : p_expression->condition_expr;
                flow_visit_expression(ctx, p_true_expr);

                /* false branch */
                ctx->p_current_flow_branch = cond_pair.p_false;
                flow_visit_expression(ctx, p_expression->right);

                /*
                    Collect the result: true arm from left (or condition), false arm from
                    right. Done BEFORE the merge, which adds "unknown on the other path"
                    for objects only one arm wrote. A REF is resolved inside its own arm
                    (conditional-operator-null-guard.c).
                */
                struct flow_alternatives result = { 0 };
                {
                    struct
                    {
                        const struct flow_key_alternatives* _Opt entry;
                        struct flow_branch* _Opt map;
                    } arms[2] =
                    {
                    { flow_branch_search_up(cond_pair.p_true, &p_true_expr->object), cond_pair.p_true },
                    { flow_branch_search_up(cond_pair.p_false, &p_expression->right->object), cond_pair.p_false }
                    };

                    for (int ai = 0; ai < 2; ai++)
                    {
                        /*
                            A condition folded at compile time (`(c == 1) ? a : b`
                            with c known) leaves one arm dead, exactly as an `if`
                            does; the value of the dead arm is not a possible
                            result. Same test flow_branch_merge_arms applies.
                        */
                        if (arms[ai].map == NULL || arms[ai].map->is_unreachable)
                            continue;

                        const struct flow_key_alternatives* _Opt e = arms[ai].entry;
                        if (e == NULL || e->alternatives.size == 0)
                            continue;

                        for (int i = 0; i < e->alternatives.size; i++)
                        {
                            const struct flow_alternative* a = e->alternatives.data[i];

                            if (a->value_kind == FLOW_VALUE_KIND_REF &&
                            a->value.p != NULL)
                            {
                                const struct flow_key_alternatives* _Opt p_target =
                                flow_branch_search_up(arms[ai].map, a->value.p);

                                if (p_target != NULL && p_target->alternatives.size > 0)
                                {
                                    flow_alternatives_append(&result, &p_target->alternatives);

                                    /*
                                        An _Owner result takes the owner in its own arm:
                                        `q = p ? p : 0` moves p only where p was chosen
                                        (issue 491).
                                    */
                                    if (type_is_owner(&p_expression->object.type) &&
                                        type_is_owner(&a->value.p->type) &&
                                        a->value.p->members.head == NULL)
                                    {
                                        flow_branch_set_object_moved(arms[ai].map, a->value.p,
                                                                   (ai == 0 ? p_true_expr : p_expression->right)->first_token);
                                    }
                                    continue;
                                }
                            }

                            flow_alternatives_add(&result, a);
                        }
                    }
                }

                /* merge both arms back into p_before */
                if (cond_pair.p_true != NULL && cond_pair.p_false != NULL)
                {
                    flow_branch_merge_a_b(p_before, cond_pair.p_true, cond_pair.p_false);
                }
                ctx->p_current_flow_branch = p_before;

                struct flow_key_alternatives* _Opt p_result_entry = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                if (p_result_entry == NULL)
                {
                    flow_alternatives_clear(&result);
                    throw;
                }
                flow_alternatives_clear(&p_result_entry->alternatives);
                flow_alternatives_append(&p_result_entry->alternatives, &result);
                flow_alternatives_clear(&result);

                if (p_result_entry->alternatives.size == 0)
                {
                    /* neither arm has a known value: record ANY */
                    struct flow_alternative a =
                    {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = ANY_VALUE},
                    .value_relation = FLOW_RELATION_ANY,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_expression->first_token
                    };
                    flow_alternatives_add(&p_result_entry->alternatives, &a);
                }
            }
            break;

            case EXPR_UNARY_GCC__BUILTIN_VA_START:
            case EXPR_UNARY_GCC__BUILTIN_VA_END:
            case EXPR_UNARY_GCC__BUILTIN_VA_COPY:
            case EXPR_UNARY_GCC__BUILTIN_VA_ARG:
            break;
            case EXPR_UNARY_GCC__BUILTIN_OFFSETOF:
            case EXPR_UNARY_CONSTEVAL:
            break;

        }
    }
    catch
    {
        while (ctx->findings_depth > findings_depth)
            flow_findings_end(ctx);
    }

    struct flow_true_false_branches identity_pair = { ctx->p_current_flow_branch, ctx->p_current_flow_branch };

    /* Non-boolean or unhandled expression: both branches are identical (no narrowing). */
    return identity_pair;
}

static void flow_visit_expression_statement(struct flow_ctx* ctx, const struct expression_statement* p_expression_statement)
{
    /*
        Only meant to bridge a report from THIS statement's own expression
        visit into a check running right after it (see the field comment in
        flow3.h) -- must not leak into an unrelated later statement.
    */
    ctx->p_pending_ended_report_obj = NULL;

    if (p_expression_statement->expression_opt)
    {
        flow_visit_full_expression(ctx, p_expression_statement->expression_opt);

        /* before the lint check, so //lint 97 on this statement sees the report */
        flow_check_limits(ctx, p_expression_statement->expression_opt->first_token);
    }

    if (p_expression_statement->p_lint_token)
    {
        flow_check_dianostic_suppression(ctx, p_expression_statement->p_lint_token);
    }
}

static void flow_visit_block_item_list(struct flow_ctx* ctx, struct block_item_list* p_block_item_list);

static void flow_visit_compound_statement_core(struct flow_ctx* ctx, struct compound_statement* p_compound_statement)
{
    flow_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    flow_exit_block_visit_defer_list(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
}

static void flow_visit_compound_statement(struct flow_ctx* ctx, struct compound_statement* p_compound_statement)
{
    flow_visit_compound_statement_core(ctx, p_compound_statement);

    if (p_compound_statement->lint_token)
    {
        flow_check_dianostic_suppression(ctx, p_compound_statement->lint_token);
    }
}

/*
    Merge a loop's exit paths back into p_before: every condition-false arm plus
    break. Loop-variant values are widened on them first; NULL arms (a condition
    that was never evaluated) are skipped.
*/
static void flow_merge_loop_exits(struct flow_ctx* ctx,
                                  struct flow_branch* p_before,
                                  struct flow_branch* _Opt p_pass1_exit,
                                  struct flow_branch* p_break_join,
                                  struct flow_branch* _Opt const false_arms[],
                                  int num_false_arms,
                                  const struct token* p_token)
{
    struct flow_branch* _Opt exit_arms[4] = { 0 };
    const struct flow_branch* arms[4] = { p_before, p_before, p_before, p_before };
    int num_arms = 0;

    _Assert(num_false_arms < 4);
    for (int i = 0; i < num_false_arms; i++)
    {
        exit_arms[i] = false_arms[i];
        if (false_arms[i] != NULL)
            arms[num_arms++] = false_arms[i];
    }
    exit_arms[num_false_arms] = p_break_join;

    flow_widen_loop_variant_objects(p_pass1_exit, ctx->p_current_flow_branch,
                                    exit_arms, num_false_arms + 1, p_token,
                                    false);

    if (ctx->break_join_reached)
    {
        arms[num_arms++] = p_break_join;
    }
    if (num_arms > 0)
    {
        flow_branch_merge_arms(p_before, arms, num_arms);
    }
}


/* true when ++, --, += or -= produced the value (++/-- record it at the variable, +=/-= at the operand) */
static bool flow_alternative_is_step(const struct flow_alternative* alt)
{
    bool is_step = false;
    const struct token* _Opt p_token = alt->p_origin_token;
    if (p_token != NULL)
    {
        if (p_token->type == TK_PLUSPLUS || p_token->type == TK_MINUSMINUS)
        {
            is_step = true; /* ++k, --k */
        }
        else
        {
            const struct token* _Opt p_next = p_token->next;
            while (p_next != NULL && token_is_blank(p_next))
            {
                p_next = p_next->next;
            }
            is_step = p_next != NULL &&
                (p_next->type == TK_PLUSPLUS || p_next->type == TK_MINUSMINUS);

            /* k += c records the value at c: look behind it for the operator */
            const struct token* _Opt p_prev = p_token->prev;
            while (p_prev != NULL && token_is_blank(p_prev))
            {
                p_prev = p_prev->prev;
            }
            if (p_prev != NULL &&
                (p_prev->type == TK_PLUS_ASSIGN || p_prev->type == TK_MINUS_ASSIGN))
            {
                is_step = true;
            }
        }
    }
    return is_step;
}

/*
    The numeric values of e as [*p_min, *p_max]. One value, or (repeated
    values allowed) several: `if (c) n++;` in a loop body leaves n {0, 1} after
    one pass and {0, 1, 2} after two, a range still moving like a single value
    (loop-counter-in-branch-widened.c).
*/
static bool flow_entry_numeric_value(const struct flow_key_alternatives* _Opt e,
                                     long long* p_min,
                                     long long* p_max,
                                     bool allow_repeated_value,
                                     bool* _Opt p_is_step)
{
    if (e == NULL || e->alternatives.size == 0)
    {
        return false;
    }

    bool found = false;
    long long min = 0;
    long long max = 0;

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow_alternative* alt = e->alternatives.data[i];

        if (flow_alternative_is_dead(alt))
        {
            continue;
        }

        if (alt->imaginary != FLOW_IMAGINARY_NONE ||
                alt->value_relation != FLOW_RELATION_EQUAL)
        {
            return false;
        }

        long long this_value = 0;
        if (alt->value_kind == FLOW_VALUE_KIND_SIGNED)
        {
            this_value = alt->value.i;
        }
        else if (alt->value_kind == FLOW_VALUE_KIND_UNSIGNED)
        {
            this_value = (long long)alt->value.u;
        }
        else
        {
            return false;
        }

        if (found && !allow_repeated_value)
        {
            return false;
        }

        if (p_is_step != NULL && !flow_alternative_is_step(alt))
        {
            *p_is_step = false;
        }

        if (!found || this_value < min)
            min = this_value;
        if (!found || this_value > max)
            max = this_value;
        found = true;
    }

    if (!found)
    {
        return false;
    }

    *p_min = min;
    *p_max = max;
    return true;
}


static void flow_widen_loop_variant_objects(
        struct flow_branch* _Opt p_pass1_exit,
        struct flow_branch* _Opt p_pass2_exit,
        struct flow_branch* _Opt* arms,
        int num_arms,
        const struct token* _Opt p_token,
        bool allow_repeated_value)
{
    if (p_pass1_exit == NULL || p_pass2_exit == NULL || num_arms <= 0)
    {
        return;
    }

    struct object_set variants = { 0 };

    /* the widened value of variants.items[i], decided in the walk below */
    struct flow_widen_fact
    {
        enum flow_relation relation;
        long long value;
    };
    struct flow_widen_fact* _Owner _Opt facts = NULL;
    int facts_capacity = 0;

    /* Every key the second pass's body touched. */
    struct flow_branch_entries it = flow_branch_entries_begin(p_pass2_exit, p_pass1_exit);
    for (const struct flow_key_alternatives* _Opt e; (e = flow_branch_entries_next(&it)) != NULL;)
    {
        long long pass1_min = 0, pass1_max = 0;
        long long pass2_min = 0, pass2_max = 0;
        bool is_step = true;

        if (!flow_entry_numeric_value(
            flow_branch_search_up(p_pass1_exit, e->p_obj_key), &pass1_min, &pass1_max,
            allow_repeated_value, NULL))
        {
            continue;
        }

        if (!flow_entry_numeric_value(
            flow_branch_search_up(p_pass2_exit, e->p_obj_key), &pass2_min, &pass2_max,
            allow_repeated_value, &is_step))
        {
            continue;
        }

        if (pass1_min == pass2_min && pass1_max == pass2_max)
        {
            continue;
        }

        const int size_before = variants.size;
        object_set_add(&variants, e->p_obj_key);
        if (variants.size == size_before)
        {
            continue; /* already decided from a map nearer the exit */
        }

        if (variants.size > facts_capacity)
        {
            const int new_capacity = facts_capacity ? facts_capacity * 2 : 8;
            struct flow_widen_fact* _Owner _Opt p_new = realloc(facts, new_capacity * sizeof(struct flow_widen_fact));
            if (p_new == NULL)
            {
                continue; /* past facts_capacity the key widens to ANY */
            }
            facts = p_new; //lint 26  (realloc semantics)
            facts_capacity = new_capacity;
        }

        /* only ++, --, +=, -= keep a direction (signed overflow is UB); anything else widens to ANY */
        struct flow_widen_fact fact = { .relation = FLOW_RELATION_ANY, .value = ANY_VALUE };
        if (is_step && type_is_signed_integer(&e->p_obj_key->type))
        {
            if (pass2_min >= pass1_min && pass2_max > pass1_max)
            {
                fact.relation = FLOW_RELATION_GREATER_EQUAL;
                fact.value = pass1_min;
            }
            else if (pass2_max <= pass1_max && pass2_min < pass1_min)
            {
                fact.relation = FLOW_RELATION_LESS_EQUAL;
                fact.value = pass1_max;
            }
        }
        facts[variants.size - 1] = fact;
    }

    for (int i = 0; i < variants.size; i++)
    {
        const struct object* p_key = variants.items[i];

        long long widen_value = ANY_VALUE;
        enum flow_relation widen_relation = FLOW_RELATION_ANY;
        if (facts != NULL && i < facts_capacity)
        {
            widen_value = facts[i].value;
            widen_relation = facts[i].relation;
        }

        for (int a = 0; a < num_arms; a++)
        {
            if (arms[a] == NULL)
            {
                continue;
            }

            struct flow_key_alternatives* _Opt e = flow_branch_find_add(arms[a], p_key);
            if (e == NULL)
            {
                continue;
            }

            flow_alternatives_clear(&e->alternatives);

            struct flow_alternative any =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED,
                .value = {.i = widen_value},
                .value_relation = widen_relation,
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = arms[a],
                .p_origin_token = p_token
            };
            flow_alternatives_add(&e->alternatives, &any);
        }
    }

    object_set_destroy(&variants);
    free(facts);
}

/* Merge the pre-loop value back in only for keys with no ownership/lifetime facts (those stay exactly as pass 1 computed): a naive union broke the free-and-advance idiom (item->next reading possibly-moved, ~56 false warnings). for-loops use an older, broader join instead. */
static bool flow_alternatives_are_plain_values(const struct flow_alternatives* alts)
{
    for (int i = 0; i < alts->size; i++)
    {
        const struct flow_alternative* alt = alts->data[i];

        if (alt->imaginary != FLOW_IMAGINARY_NONE)
        {
            return false;
        }
    }
    return alts->size > 0;
}

static void flow_join_first_iteration_values(const struct flow_allocated_object_arena* p_arena,
                                             struct flow_branch* _Opt p_body_entry,
                                             struct flow_branch* _Opt p_pass1_exit,
                                             const struct token* _Opt p_token)
{
    if (p_body_entry == NULL || p_pass1_exit == NULL || p_pass1_exit->is_unreachable)
    {
        return;
    }

    struct object_set assigned = { 0 };

    /*
        Every key the body wrote: the maps between the body's exit and its
        entry, exactly the walk flow_widen_loop_variant_objects does.
    */
    struct flow_branch_entries it = flow_branch_entries_begin(p_pass1_exit, p_body_entry);
    for (const struct flow_key_alternatives* _Opt e; (e = flow_branch_entries_next(&it)) != NULL;)
    {
        object_set_add(&assigned, e->p_obj_key);
    }

    for (int i = 0; i < assigned.size; i++)
    {
        const struct object* p_key = assigned.items[i];

        const struct flow_key_alternatives* _Opt p_pre = flow_branch_search_up(p_body_entry, p_key);
        const struct flow_key_alternatives* _Opt p_post = flow_branch_search_up(p_pass1_exit, p_key);

        if (p_pre == NULL || p_post == NULL)
        {
            continue;
        }

        bool post_moved_only = false;
        if (flow_alternatives_are_plain_values(&p_pre->alternatives) &&
                !flow_alternatives_are_plain_values(&p_post->alternatives))
        {
            post_moved_only = true;
            for (int k = 0; k < p_post->alternatives.size; k++)
            {
                if (p_post->alternatives.data[k]->imaginary == FLOW_IMAGINARY_ENDED)
                {
                    post_moved_only = false;
                }
            }
        }

        if (post_moved_only)
        {
            /* a pointee may be a different object on the next iteration: only variables keep the move */
            const struct object* p_root = p_key;
            while (p_root->parent != NULL)
            {
                p_root = p_root->parent;
            }
            for (int k = 0; k < p_arena->size; k++)
            {
                if (p_arena->data[k] == p_root)
                {
                    post_moved_only = false;
                    break;
                }
            }
        }

        if (!post_moved_only &&
                (!flow_alternatives_are_plain_values(&p_pre->alternatives) ||
                 !flow_alternatives_are_plain_values(&p_post->alternatives)))
        {
            /*
                moved/ended/absent on either side: an ownership fact, not a
                value. Leave it to pass 1.
            */
            continue;
        }

        struct flow_alternatives merged = { 0 };
        flow_alternatives_append(&merged, &p_post->alternatives);

        for (int k = 0; k < p_pre->alternatives.size; k++)
        {
            const struct flow_alternative* a = p_pre->alternatives.data[k];

            bool already_there = false;
            for (int m = 0; m < merged.size; m++)
            {
                if (merged.data[m]->value_relation == a->value_relation &&
                        flow_value_is_same(merged.data[m], a))
                {
                    already_there = true;
                    break;
                }
            }

            if (already_there)
            {
                continue;
            }

            struct flow_alternative tagged = *a;
            tagged.p_origin_map = p_body_entry; /* the "first iteration" path */
            tagged.p_origin_token = p_token;
            flow_alternatives_add(&merged, &tagged);
        }

        struct flow_key_alternatives* _Opt e = flow_branch_find_add(p_body_entry, p_key);
        if (e == NULL)
        {
            flow_alternatives_clear(&merged);
            continue;
        }

        flow_alternatives_clear(&e->alternatives);
        e->alternatives = merged; /* move */
    }

    object_set_destroy(&assigned);
}

/* joins falling off the body end with every `continue`; returns the map after the body */
static struct flow_branch* _Opt flow_loop_body_end(struct flow_ctx* ctx,
                                                   struct flow_branch* _Opt p_body_entry,
                                                   const struct flow_branch* _Opt p_continue_join)
{
    struct flow_branch* _Opt p_end = ctx->p_current_flow_branch;

    if (p_end != NULL &&
        p_body_entry != NULL &&
        p_continue_join != NULL &&
        ctx->continue_join_reached)
    {
        /* an unreachable end must not be the parent: everything under it reads as dead */
        struct flow_branch* _Opt p_joined =
            flow_branch_arena_new(&ctx->flow_branch_arena, p_end->is_unreachable ? p_body_entry : p_end, FLOW_BRANCH_FOR_BODY_PASS1);
        if (p_joined != NULL)
        {
            const struct flow_branch* const arms[2] = { p_end, p_continue_join };
            flow_branch_merge_arms(p_joined, arms, 2);
            p_end = p_joined;
        }
    }

    return p_end;
}

/*
    Shared by while, for and do-while; the caller ran the for-init.
    p_condition is NULL for `for (;;)`. condition_first is false for
    do-while. join_zero_iterations: for joins a zero-iterations arm with
    pass 1's exit; while joins only first-iteration values
    (flow_join_first_iteration_values) -- the full join is opt-in via
    warning 85.
*/
static void flow_visit_loop(struct flow_ctx* ctx,
                            struct iteration_statement* p_iteration_statement,
                            const struct expression* _Opt p_condition,
                            const struct expression* _Opt p_next,
                            bool join_zero_iterations,
                            bool condition_first)
{
    struct flow_branch* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow_branch* _Opt old_p_break_join_map = ctx->p_break_join_map;
    struct flow_branch* _Opt old_p_continue_join_map = ctx->p_continue_join_map;
    const bool old_break_join_reached = ctx->break_join_reached;
    const bool old_continue_join_reached = ctx->continue_join_reached;

    if (ctx->p_current_flow_branch == NULL)
        return;

    struct flow_branch* p_before = ctx->p_current_flow_branch;
    struct flow_branch* _Opt p_break_join = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_BREAK_JOIN);
    if (p_break_join == NULL)
        return; /* no map to work with */

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;
    ctx->break_join_reached = false;

    /* First pass — suppress warnings */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    struct flow_true_false_branches pair1 = { 0 };
    if (p_condition && condition_first)
    {
        pair1 = flow_visit_condition(ctx, p_condition);
        pair1 = flow_ensure_branch_pair(ctx, p_before, pair1, p_condition);
        ctx->p_current_flow_branch = pair1.p_true;
    }
    else
    {
        /* do-while and `for (;;)`: body entry is a child of p_before, so joins never write the pre-loop map */
        ctx->p_current_flow_branch =
            flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_FOR_BODY_PASS1);
    }

    /*
        Pass 1 body in its own child map, so flow_widen_loop_variant_objects can
        find what the body assigned (child) and the values before it (parent).
    */
    struct flow_branch* _Opt p_pass1_body_entry = ctx->p_current_flow_branch;
    if (p_pass1_body_entry != NULL)
    {
        struct flow_branch* _Opt p_pass1_body = flow_branch_arena_new(&ctx->flow_branch_arena,
                                                                p_pass1_body_entry, FLOW_BRANCH_FOR_BODY_PASS1);
        if (p_pass1_body != NULL)
        {
            ctx->p_current_flow_branch = p_pass1_body;
        }
    }

    ctx->p_continue_join_map = p_pass1_body_entry == NULL ? NULL :
        flow_branch_arena_new(&ctx->flow_branch_arena, p_pass1_body_entry, FLOW_BRANCH_CONTINUE_JOIN);
    ctx->continue_join_reached = false;

    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    ctx->p_current_flow_branch = flow_loop_body_end(ctx, p_pass1_body_entry, ctx->p_continue_join_map);

    /*
        State after one iteration; compared with the second pass's to spot
        values that are still moving (see flow_widen_loop_variant_objects).
    */
    struct flow_branch* _Opt p_pass1_exit = ctx->p_current_flow_branch;

    /*
        Every path through the body jumps away (map is_unreachable): skip the
        increment and pass 2, as merge_arms skips unreachable arms.
    */
    const bool body_falls_through =
        !(ctx->p_current_flow_branch != NULL && ctx->p_current_flow_branch->is_unreachable);

    if (body_falls_through && p_next)
    {
        flow_visit_full_expression(ctx, p_next);
    }

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    struct flow_true_false_branches pair2 = { 0 };
    struct flow_true_false_branches pair3 = { 0 };

    /*
        Widen body-assigned numeric values before the diagnostic pass, so pass 2
        does not fold on one iteration's concrete value.
    */
    if (body_falls_through && p_pass1_body_entry != NULL && p_pass1_exit != NULL)
    {
        /*
            Widen first (MOVED values like `n++` become ANY), then join (SET values
            like `flag = 1` get the zero-iterations case back). The join destroys
            what widening keys off, so order matters.
        */
        /* do-while: condition visited on pass 1's exit before widening; its false arm keeps one iteration's values */
        if (!condition_first && p_condition)
        {
            ctx->p_current_flow_branch = p_pass1_exit;
            pair2 = flow_visit_condition(ctx, p_condition);
            pair2 = flow_ensure_branch_pair(ctx, p_pass1_exit, pair2, p_condition);
        }

        /* do-while widens only the arm that iterates again; the false arm must stay unwidened */
        struct flow_branch* _Opt widen_arms[1] = { p_pass1_exit };
        if (!condition_first && pair2.p_true != NULL)
        {
            widen_arms[0] = pair2.p_true;
        }
        flow_widen_loop_variant_objects(p_pass1_body_entry, p_pass1_exit,
                                        widen_arms, 1, p_iteration_statement->first_token,
                                        true);

        if (!condition_first)
        {
            /*
                first iteration (unnarrowed) joined with "condition held after an iteration".
                Values only, as while: ownership facts (moved/ended) of pass 1 would mix
                paths, e.g. an object ended where f(ptk) ran with ptk reaching it from
                the path where f did not run.
            */
            struct flow_branch* _Opt p_again = p_condition ? pair2.p_true : p_pass1_exit;

            /* do { } while (0): the body never runs again */
            const bool condition_never_holds = p_condition != NULL &&
                object_has_constant_value(&p_condition->object) &&
                !object_is_true(&p_condition->object);

            if (!condition_never_holds)
            {
                flow_join_first_iteration_values(&ctx->allocated_object_arena, p_pass1_body_entry, p_again,
                                                 p_iteration_statement->first_token);
            }
            ctx->p_current_flow_branch = p_pass1_body_entry;
        }
        else if (join_zero_iterations)
        {
            /*
                "Zero iterations" arm: an empty child of the body-entry state, so it
                contributes the pre-loop value of every key by inheritance.
            */
            struct flow_branch* _Opt p_zero_iterations = flow_branch_arena_new(&ctx->flow_branch_arena,
                                                                         p_pass1_body_entry, FLOW_BRANCH_FOR_BODY_PASS1);
            if (p_zero_iterations != NULL)
            {
                const struct flow_branch* const join_arms[2] = { p_zero_iterations, p_pass1_exit };
                flow_branch_merge_arms(p_pass1_body_entry, join_arms, 2);
                ctx->p_current_flow_branch = p_pass1_body_entry;
            }
        }
        else
        {
            flow_join_first_iteration_values(&ctx->allocated_object_arena, p_pass1_body_entry, p_pass1_exit,
                                             p_iteration_statement->first_token);
            ctx->p_current_flow_branch = p_pass1_body_entry;
        }
    }

    if (body_falls_through)
    {
        if (p_condition && condition_first)
        {
            pair2 = flow_visit_condition(ctx, p_condition);
            pair2 = flow_ensure_branch_pair(ctx, ctx->p_current_flow_branch, pair2, p_condition);
            ctx->p_current_flow_branch = pair2.p_true;
        }

        struct flow_branch* _Opt p_pass2_body_entry = ctx->p_current_flow_branch;
        ctx->p_continue_join_map = p_pass2_body_entry == NULL ? NULL :
            flow_branch_arena_new(&ctx->flow_branch_arena, p_pass2_body_entry, FLOW_BRANCH_CONTINUE_JOIN);
        ctx->continue_join_reached = false;

        ctx->iteration_pass = 2; /*second pass -- see flow_visit_iteration_statement*/
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        ctx->iteration_pass = 1;

        ctx->p_current_flow_branch = flow_loop_body_end(ctx, p_pass2_body_entry, ctx->p_continue_join_map);

        if (p_next)
        {
            flow_visit_full_expression(ctx, p_next);
        }

        /*
            Third look at the condition: the exit after two or more iterations, for
            values the body assigns only from the second iteration on. Diagnostics
            off.
        */
        if (p_condition)
        {
            diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
            pair3 = flow_visit_condition(ctx, p_condition);
            pair3 = flow_ensure_branch_pair(ctx, ctx->p_current_flow_branch, pair3, p_condition);
            diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);
        }
    }
    else
    {
        /*
            Every path diverges: re-visit the body once with diagnostics on (pass 1
            had them off); the map is discarded. Start from the pre-loop state, not
            the body's own unreachable map.
        */
        if (p_condition && condition_first)
        {
            ctx->p_current_flow_branch = p_before;
            struct flow_true_false_branches diag_pair = flow_visit_condition(ctx, p_condition);
            diag_pair = flow_ensure_branch_pair(ctx, p_before, diag_pair, p_condition);
            ctx->p_current_flow_branch = diag_pair.p_true;
        }
        else
        {
            ctx->p_current_flow_branch =
                flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_FOR_BODY_DIAG);
        }
        ctx->p_continue_join_map = NULL; /* this visit only reports */
        ctx->iteration_pass = 2; /*reporting pass -- see flow_visit_iteration_statement*/
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        ctx->iteration_pass = 1;
    }

    if (p_iteration_statement->secondary_block != NULL)
    {
        flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list,
                                         p_iteration_statement->secondary_block->last_token);
    }

    /*
        Loop exit paths: condition false before the first iteration, after one
        iteration, after two or more, or break.
    */
    struct flow_branch* _Opt false_arms[3] = { pair1.p_false, pair2.p_false, pair3.p_false };
    flow_merge_loop_exits(ctx, p_before, p_pass1_exit, p_break_join, false_arms, 3,
                          p_iteration_statement->first_token);

    ctx->p_current_flow_branch = p_before;

    if (p_iteration_statement->secondary_block != NULL)
    {
        flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list,
                                            p_iteration_statement->secondary_block->last_token);
    }

    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
    ctx->p_continue_join_map = old_p_continue_join_map;
    ctx->break_join_reached = old_break_join_reached;
    ctx->continue_join_reached = old_continue_join_reached;
}

static void flow_visit_while_statement(struct flow_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    _Assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    if (p_iteration_statement->expression1 == NULL)
    {
        return;
    }

    flow_visit_loop(ctx, p_iteration_statement, p_iteration_statement->expression1, NULL, false, true);
}

static void flow_visit_do_while_statement(struct flow_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    _Assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);

    flow_visit_loop(ctx, p_iteration_statement, p_iteration_statement->expression1, NULL, false, false);
}

static void flow_visit_for_statement(struct flow_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    _Assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);

    if (p_iteration_statement->declaration &&
            p_iteration_statement->declaration->init_declarator_list.head)
    {
        flow_visit_init_declarator_list(ctx, &p_iteration_statement->declaration->init_declarator_list);
    }
    else if (p_iteration_statement->expression0)
    {
        flow_visit_full_expression(ctx, p_iteration_statement->expression0);
    }

    flow_visit_loop(ctx, p_iteration_statement, p_iteration_statement->expression1,
                    p_iteration_statement->expression2, true, true);
}

static void flow_visit_iteration_statement(struct flow_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    const int outer_iteration_pass = ctx->iteration_pass;
    ctx->iteration_pass = 1; /*first pass over this loop's body*/

    switch (p_iteration_statement->first_token->type)
    {
        case TK_KEYWORD_WHILE:
            flow_visit_while_statement(ctx, p_iteration_statement);
        break;
        case TK_KEYWORD_DO:
            flow_visit_do_while_statement(ctx, p_iteration_statement);
        break;
        case TK_KEYWORD_FOR:
            flow_visit_for_statement(ctx, p_iteration_statement);
        break;
        default:
            _Assert(false);
        break;
    }
    ctx->iteration_pass = outer_iteration_pass; /* restore */

    if (p_iteration_statement->p_lint_token)
    {
        flow_check_dianostic_suppression(ctx, p_iteration_statement->p_lint_token);
    }
}

static void flow_check_arena_objects_at_function_exit(const struct flow_ctx* ctx)
{
    for (int i = 0; i < ctx->allocated_object_arena.size; i++)
    {
        struct object* _Opt p_obj = ctx->allocated_object_arena.data[i];
        if (p_obj == NULL)
        {
            continue;
        }

        /*
         * TODO: check origin tag to distinguish external objects
         * (parameter-seeded, caller-owned) from internal ones
         * (function-created, fully checkable).
         */
    }
}


static void flow_check_file_scope_objects_at_function_exit(const struct flow_ctx* ctx)
{
    /* Build a fast-lookup set of arena object pointers so we can skip them. */
    struct object_set arena_set = { 0 };
    for (int i = 0; i < ctx->allocated_object_arena.size; i++)
    {
        if (ctx->allocated_object_arena.data[i] != NULL)
        {
            object_set_add(&arena_set, ctx->allocated_object_arena.data[i]);
        }
    }

    /* Collect every unique object key present anywhere in the map chain. */
    struct object_set all_keys = { 0 };
    struct flow_branch_entries it = flow_branch_entries_begin(ctx->p_current_flow_branch, NULL);
    for (const struct flow_key_alternatives* _Opt e; (e = flow_branch_entries_next(&it)) != NULL;)
    {
        /* Skip sentinel keys that are not real objects */
        uintptr_t val = (uintptr_t)e->p_obj_key;
        if (val == UNINITIALIZED_VALUE || val == ANY_VALUE)
        {
            continue;
        }
        object_set_add(&all_keys, e->p_obj_key);
    }

    for (int i = 0; i < all_keys.size; i++)
    {
        const struct object* p_obj = all_keys.items[i];

        /* Skip arena (synthetic) objects. */
        bool in_arena = false;
        for (int j = 0; j < arena_set.size; j++)
        {
            if (arena_set.items[j] == p_obj)
            {
                in_arena = true;
                break;
            }
        }
        if (in_arena)
        {
            continue;
        }

        /* Skip non-file-scope objects (parameters, block-scope locals). */
        if (!object_is_file_scope(p_obj))
        {
            continue;
        }
    }

    object_set_destroy(&all_keys);
    object_set_destroy(&arena_set);
}

/* Render a _Clear/_Out/_Dtor exit-check's subject as source code would spell it ('source->tail') instead of '(.tail)' pointee notation -- strip member_designator's leading dot and use '->' for the first hop since param_name is always a pointer, keeping any further dots as-is. User-reported. */
static void flow_param_member_name_to_string(const char* param_name,
                                             const char* _Opt member_designator, struct osstream* ss)
{
    ss_clear(ss);
    ss_fprintf(ss, "%s", param_name);
    if (member_designator != NULL && member_designator[0] != '\0')
    {
        ss_fprintf(ss, "->%s",
                   member_designator[0] == '.' ? member_designator + 1 : member_designator);
    }
}

static void flow_check_clear_object_is_zero_at_exit(struct flow_ctx* ctx,
                                                    const struct object* p_obj,
                                                    const char* param_name,
                                                    const struct marker* marker,
                                                    const struct token* p_exit_token)
{
    if (p_obj->members.head)
    {
        for (const struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow_check_clear_object_is_zero_at_exit(ctx, member, param_name, marker, p_exit_token);
        }
        return;
    }

    const struct flow_key_alternatives* _Opt e = flow_branch_search_up(ctx->p_current_flow_branch, p_obj);
    if (e == NULL || e->alternatives.size == 0)
    {
        /* Never touched at all,  so no evidence of missing clear */
        struct osstream name_ss = { 0 };
        flow_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss);
        diagnostic(W_FLOW_CLEAR_NOT_ZERO_AT_EXIT,
                   ctx->ctx,
                   NULL,
                   marker,
                   "_Clear parameter '%s' is never set to zero",
                   name_ss.c_str ? name_ss.c_str : param_name);
        ss_close(&name_ss);
        return;
    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow_alternative* p_alternative = e->alternatives.data[i];

        if (flow_alternative_is_zero(p_alternative))
        {
            /* OK! */
            continue;
        }
        
        struct osstream name_ss2 = { 0 };
        flow_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss2);
        diagnostic(W_FLOW_CLEAR_NOT_ZERO_AT_EXIT,
                    ctx->ctx,
                    NULL,
                    marker,
                    "_Clear parameter '%s' is not zero at exit (last set at line %d)",
                    name_ss2.c_str ? name_ss2.c_str : param_name,
                    flow_alternative_line(p_alternative));
        ss_close(&name_ss2);        
    }
}

/*
    _Out: every member of the pointee (not only _Owner) must be assigned at
    every exit. flow_parameter_object_init seeds them UNINITIALIZED.
*/
static void flow_check_ctor_object_is_initialized_at_exit(struct flow_ctx* ctx,
                                                          const struct object* p_obj,
                                                          const char* param_name,
                                                          const struct marker* marker,
                                                          const struct token* p_exit_token)
{
    if (p_obj->members.head)
    {
        for (const struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow_check_ctor_object_is_initialized_at_exit(ctx, member, param_name, marker, p_exit_token);
        }
        return;
    }

    /*
        top_level (a bare scalar/pointer pointee, not a struct member) used
        to be skipped here: `*p = expr;` didn't update this object, so it
        always looked UNINITIALIZED. Fixed at the source (issue #471).
    */

    const struct flow_key_alternatives* _Opt e = flow_branch_search_up(ctx->p_current_flow_branch, p_obj);
    if (e == NULL || e->alternatives.size == 0)
    {
        /*
            No entry at all -- never written, still carries the UNINITIALIZED
            seed from function entry.
        */
        struct osstream name_ss = { 0 };
        flow_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss);
        diagnostic(W_FLOW_CTOR_NOT_INITIALIZED_AT_EXIT,
                   ctx->ctx,
                   NULL,
                   marker,
                   "_Out parameter '%s' is never initialized",
                   name_ss.c_str ? name_ss.c_str : param_name);
        
        ss_close(&name_ss);
        return;
    }

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow_alternative* p_alternative = e->alternatives.data[i];

        if (p_alternative->value_relation == FLOW_RELATION_UNINITIALIZED)
        {
            struct osstream name_ss2 = { 0 };
            flow_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss2);
            diagnostic(W_FLOW_CTOR_NOT_INITIALIZED_AT_EXIT,
                       ctx->ctx,
                        NULL,
                        marker,
                        "_Out parameter '%s' is possibly not initialized at exit (last set at line %d)",
                        name_ss2.c_str ? name_ss2.c_str : param_name,
                        flow_alternative_line(p_alternative));
            
            ss_close(&name_ss2);
        }
    }
}

/*
    A plain pointer parameter is a borrow: its _Owner members must be live
    at every exit. Mirror of flow_check_ctor_object_is_initialized_at_exit.
*/
static void flow_check_non_dtor_param_owner_not_consumed_at_exit(struct flow_ctx* ctx,
                                                                 const struct type* p_type,
                                                                 const struct object* p_obj,
                                                                 const char* param_name,
                                                                 const struct marker* marker,
                                                                 const struct token* p_exit_token,
                                                                 bool in_view,
                                                                 bool top_level)
{
    const bool view = in_view || type_is_view(p_type);

    if (p_obj->members.head)
    {
        for (const struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow_check_non_dtor_param_owner_not_consumed_at_exit(ctx,
                                                                 &member->type,
                                                                 member,
                                                                 param_name,
                                                                 marker,
                                                                 p_exit_token,
                                                                 view,
                                                                 false);
        }
        return;
    }

    if (top_level)
    {
        /*
            Same carve-out as flow_check_ctor_object_is_initialized_at_exit:
            a scalar/pointer pointee reached only through a bare `*p = ...`
            write is not yet reliably tracked back onto this exact arena
            object. Only members reached via `->` are checked.
        */
        return;
    }

    if (!type_is_owner(&p_obj->type) || view)
    {
        return;
    }

    const struct flow_key_alternatives* _Opt e = flow_branch_search_up(ctx->p_current_flow_branch, p_obj);
    if (e == NULL)
    {
        return;
    }

    /*
        one report per parameter, with the count; joins the exit's table (see
        flow_check_object_at_exit)
    */
    flow_findings_join(ctx);

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow_alternative* p_alternative = e->alternatives.data[i];

        if (p_alternative->imaginary == FLOW_IMAGINARY_MOVED ||
                p_alternative->imaginary == FLOW_IMAGINARY_ENDED)
        {
            struct osstream name_ss = { 0 };
            flow_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss);
            struct osstream ss = { 0 };
            const char* what = p_alternative->imaginary == FLOW_IMAGINARY_MOVED ? "moved" : "released";
            ss_fprintf(&ss,
                       "parameter '%s' was %s but never reassigned; only a _Dtor or _Owner parameter may leave it %s",
                       name_ss.c_str ? name_ss.c_str : param_name, what, what);
            flow_finding_record(ctx, p_obj, W_FLOW_PARAM_OWNER_CONSUMED_AT_EXIT, marker,
                                p_alternative, p_alternative->p_origin_map,
                                ss.c_str ? ss.c_str : "");
            ss_close(&ss);
            ss_close(&name_ss);
        }
    }

    flow_findings_end(ctx);
}

/* One shared pass over parameters at every exit for _Clear (every member == 0), _Out (every member assigned), and _Dtor (every _Owner member released -- the callee-side half of flow_check_object_init_assigment's caller-side lifetime-ending) (dtor_is_opt.c). */
static void flow_check_write_qualified_params_at_exit(struct flow_ctx* ctx, const struct marker* marker, const struct token* p_exit_token)
{
    if (ctx->p_current_function_declaration == NULL ||
            ctx->p_current_function_declaration->init_declarator_list.head == NULL)
    {
        return;
    }

    struct declarator* p_func_declarator = ctx->p_current_function_declaration->init_declarator_list.head->p_declarator;
    struct function_declarator* _Opt p_fd = declarator_find_function_declarator(p_func_declarator);

    if (p_fd == NULL ||
            p_fd->parameter_type_list_opt == NULL ||
            p_fd->parameter_type_list_opt->parameter_list == NULL)
    {
        return;
    }

    for (struct parameter_declaration* _Opt p_param = p_fd->parameter_type_list_opt->parameter_list->head;
            p_param;
            p_param = p_param->next)
    {
        if (p_param->declarator == NULL)
        {
            continue;
        }

        const struct type* p_param_type = &p_param->declarator->object.type;

        if (!type_is_pointer(p_param_type))
        {
            continue;
        }

        const bool is_clear = type_is_pointed_clear(p_param_type);
        const bool is_dtor = type_is_pointed_dtor(p_param_type);
        const bool is_ctor = type_is_pointed_out(p_param_type);

        /*
            A plain pointer (no _Clear/_Dtor/_Out) is a borrow and gets the exit
            check too -- except an _Owner pointer, checked where it is erased.
        */
        const bool is_owner_param = type_is_owner(p_param_type);

        /*
            Find the concrete pointee: flow_parameter_object_init wired
            this parameter's own alternative to an arena object at
            function entry (see the "non-optional pointer" case there).
        */
        const struct flow_key_alternatives* _Opt e =
            flow_branch_search_up(ctx->p_current_flow_branch, &p_param->declarator->object);

        if (e == NULL)
        {
            continue;
        }

        const char* param_name = p_param->declarator->name_opt ? p_param->declarator->name_opt->lexeme : "";

        for (int i = 0; i < e->alternatives.size; i++)
        {
            const struct flow_alternative* p_alternative = e->alternatives.data[i];

            if (p_alternative->value_kind != FLOW_VALUE_KIND_PTR || p_alternative->value.p == NULL)
            {
                continue;
            }

            if (is_clear)
            {
                flow_check_clear_object_is_zero_at_exit(ctx, p_alternative->value.p, param_name, marker, p_exit_token);
            }

            if (is_dtor)
            {
                flow_check_object_at_exit(ctx,
                                          &p_alternative->value.p->type,
                                          p_alternative->value.p,
                                          marker,
                                          p_exit_token,
                                          false,
                                          param_name,
                                          true);
            }

            if (is_ctor)
            {
                flow_check_ctor_object_is_initialized_at_exit(ctx, p_alternative->value.p, param_name, marker, p_exit_token);
            }

            if (!is_clear && !is_dtor && !is_ctor && !is_owner_param)
            {
                flow_check_non_dtor_param_owner_not_consumed_at_exit(ctx,
                                                                     &p_alternative->value.p->type,
                                                                     p_alternative->value.p,
                                                                     param_name,
                                                                     marker,
                                                                     p_exit_token,
                                                                     false,
                                                                     true);
            }
        }
    }
}

/*
   After the function ends, objects the caller can reach (pointees of non-_Dtor
   pointer parameters, and what the returned value points to) cannot be
   uninitialized, moved or have their lifetime ended. Issue #476.
*/
static void flow_check_exiting_object_r(struct flow_ctx* ctx,
                                        const struct object* p_obj,
                                        struct object_set* p_visited,
                                        const char* root_name,
                                        bool root_is_pointer, /* p_obj is (part of) the pointee of root_name */
                                        bool is_return,
                                        const struct marker* marker)
{
    const int visited_size_before = p_visited->size;
    object_set_add(p_visited, p_obj);

    if (p_visited->size != visited_size_before)
    {
        /* Built on first use: most exiting objects report nothing. */
        struct osstream name_ss = { 0 };
        bool name_built = false;
        const char* name = root_name;

        for (const struct object* _Opt p_member = p_obj->members.head; p_member; p_member = p_member->next)
        {
            flow_check_exiting_object_r(ctx, p_member, p_visited, root_name, root_is_pointer, is_return, marker);
        }

        const struct flow_key_alternatives* _Opt p_alternatives = flow_branch_search_up(ctx->p_current_flow_branch, p_obj);

        for (int i = 0; p_alternatives != NULL && i < p_alternatives->alternatives.size; i++)
        {
            const struct flow_alternative* p_alternative = p_alternatives->alternatives.data[i];
            const struct object* _Opt p_target = p_alternative->value.p;

            /*
                the exiting object may be a local copy, so its own ENDED state does not matter;
                like _Dtor, an _Owner pointer may be left moved or released
            */
            if (p_target == NULL ||
                    p_target == p_obj ||
                    type_is_owner(&p_obj->type) ||
                    p_alternative->imaginary == FLOW_IMAGINARY_MOVED ||
                    (p_alternative->value_kind != FLOW_VALUE_KIND_PTR && p_alternative->value_kind != FLOW_VALUE_KIND_REF))
            {
                continue;
            }

            /* a REF is the object itself, except on an array expression where it decays to a pointer */
            if (p_alternative->value_kind == FLOW_VALUE_KIND_REF && !type_is_array(&p_obj->type))
            {
                flow_check_exiting_object_r(ctx, p_target, p_visited, root_name, root_is_pointer, is_return, marker);
                continue;
            }

            int line = 0;
            enum diagnostic_id id = W_FLOW_LIFETIME_ENDED;
            const char* state = "an object whose lifetime has ended";

            if (!flow_object_leaves_in_state(ctx, p_target, FLOW_LEAF_ENDED, p_alternative->p_origin_map, true, false, &line, NULL))
            {
                /* a buffer (no members) is not tracked per element */
                id = W_FLOW_PARAM_UNINITIALIZED_AT_EXIT;
                state = "a possibly uninitialized object";
                if (p_target->members.head == NULL ||
                        !flow_object_leaves_in_state(ctx, p_target, FLOW_LEAF_UNINITIALIZED, p_alternative->p_origin_map, true, false, &line, NULL))
                {
                    if (!name_built)
                    {
                        if (root_is_pointer)
                        {
                            flow_param_member_name_to_string(root_name, p_obj->member_designator, &name_ss);
                        }
                        else
                        {
                            ss_fprintf(&name_ss, "%s", p_obj->member_designator ? p_obj->member_designator : root_name);
                        }
                        name_built = true;
                        name = name_ss.c_str ? name_ss.c_str : root_name;
                    }
                    flow_check_exiting_object_r(ctx, p_target, p_visited, name, true, is_return, marker);
                    continue;
                }
            }

            const int marker_line = marker->p_token_begin ? marker->p_token_begin->line : 0;
            if (!flow_finding_already_reported(ctx, p_obj, marker_line, id) &&
                    !(is_return && flow_finding_already_reported(ctx, p_target, marker_line, id)))
            {
                if (!name_built)
                {
                    if (root_is_pointer)
                    {
                        flow_param_member_name_to_string(root_name, p_obj->member_designator, &name_ss);
                    }
                    else
                    {
                        ss_fprintf(&name_ss, "%s", p_obj->member_designator ? p_obj->member_designator : root_name);
                    }
                    name_built = true;
                    name = name_ss.c_str ? name_ss.c_str : root_name;
                }
                diagnostic(id, ctx->ctx, NULL, marker,
                           is_return ?
                           "returned '%s' points to %s (see line %d)" :
                           "'%s' points to %s after the function returns (see line %d)",
                           name, state, line);
            }
        }

        ss_close(&name_ss);
    }
}

/* must run after the exit's end of lifetime */
static void flow_check_exiting_objects(struct flow_ctx* ctx,
                                       const struct expression* _Opt p_return_expression,
                                       const struct marker* marker)
{
    if (p_return_expression != NULL)
    {
        struct object_set visited = { 0 };
        struct osstream return_ss = { 0 };
        flow_expression_to_string(p_return_expression, &return_ss);
        const struct marker return_marker = expression_to_marker(p_return_expression);
        flow_check_exiting_object_r(ctx, &p_return_expression->object, &visited, return_ss.c_str ? return_ss.c_str : "", false, true, &return_marker);
        ss_close(&return_ss);
        object_set_destroy(&visited);
    }

    struct function_declarator* _Opt p_fd = NULL;
    if (ctx->p_current_function_declaration != NULL &&
            ctx->p_current_function_declaration->init_declarator_list.head != NULL)
    {
        p_fd = declarator_find_function_declarator(ctx->p_current_function_declaration->init_declarator_list.head->p_declarator);
    }

    if (p_fd != NULL &&
            p_fd->parameter_type_list_opt != NULL &&
            p_fd->parameter_type_list_opt->parameter_list != NULL)
    {
        for (struct parameter_declaration* _Opt p_param = p_fd->parameter_type_list_opt->parameter_list->head;
                p_param;
                p_param = p_param->next)
        {
            if (p_param->declarator == NULL ||
                    !type_is_pointer(&p_param->declarator->object.type) ||
                    type_is_pointed_dtor(&p_param->declarator->object.type) ||
                    type_is_owner(&p_param->declarator->object.type))
            {
                continue;
            }

            const struct type* p_param_type = &p_param->declarator->object.type;
            const char* param_name = p_param->declarator->name_opt ? p_param->declarator->name_opt->lexeme : "";

            const struct flow_key_alternatives* _Opt p_alternatives =
                flow_branch_search_up(ctx->p_current_flow_branch, &p_param->declarator->object);

            for (int i = 0; p_alternatives != NULL && i < p_alternatives->alternatives.size; i++)
            {
                const struct flow_alternative* p_alternative = p_alternatives->alternatives.data[i];
                const struct object* _Opt p_pointee = p_alternative->value.p;
                int line = 0;

                /* p = &local; is not the caller's object */
                if (p_alternative->value_kind != FLOW_VALUE_KIND_PTR ||
                        p_pointee == NULL ||
                        flow_object_leaves_in_state(ctx, p_pointee, FLOW_LEAF_ENDED, ctx->p_current_flow_branch, false, false, &line, NULL))
                {
                    continue;
                }

                /* _Out and _Clear have their own exit checks */
                if (p_pointee->members.head != NULL &&
                        !type_is_pointed_out(p_param_type) &&
                        !type_is_pointed_clear(p_param_type) &&
                        flow_object_leaves_in_state(ctx, p_pointee, FLOW_LEAF_UNINITIALIZED, ctx->p_current_flow_branch, false, false, &line, NULL))
                {
                    diagnostic(W_FLOW_PARAM_UNINITIALIZED_AT_EXIT, ctx->ctx, NULL, marker,
                               "'%s' points to a possibly uninitialized object after the function returns (see line %d)",
                               param_name, line);
                }

                struct object_set visited = { 0 };
                flow_check_exiting_object_r(ctx, p_pointee, &visited, param_name, true, false, marker);
                object_set_destroy(&visited);
            }
        }
    }
}

static void flow_check_function_exit(struct flow_ctx* ctx, const struct jump_statement* p_jump_statement)
{
    if (ctx->stopped)
        return;

    flow_exit_block_visit_defer_list(ctx,
                                     &p_jump_statement->defer_list,
                                     p_jump_statement->first_token);

    flow_check_arena_objects_at_function_exit(ctx); /* TODO do file scope together */

    const struct marker marker =
    {
        .p_token_begin = p_jump_statement->first_token,
        .p_token_end = p_jump_statement->first_token
    };
    flow_check_file_scope_objects_at_function_exit(ctx);

    /* Must run before flow_defer_list_set_end_of_lifetime: the exit's defer list includes the function's own parameters, so ending their lifetime first poisons the lookups this check depends on, silently skipping it on early-return paths (user-reported: a freed p->name left dangling, unreported). */
    flow_check_write_qualified_params_at_exit(ctx, &marker, p_jump_statement->first_token);

    flow_defer_list_set_end_of_lifetime(ctx,
                                        &p_jump_statement->defer_list,
                                        p_jump_statement->first_token);

    flow_check_exiting_objects(ctx, p_jump_statement->expression_opt, &marker);
}

static void flow_visit_jump_statement(struct flow_ctx* ctx, struct jump_statement* p_jump_statement)
{
    
    try
    {
        if (ctx->p_current_flow_branch == NULL)
        {
            bug();
            throw;
        }

        ctx->p_pending_ended_report_obj = NULL;

        if (p_jump_statement->first_token->type == TK_KEYWORD_CAKE_THROW)
        {
            if (ctx->p_throw_join_map == NULL)
            {
                bug();                
                throw;
            }
            
            /* One snapshot map per throw */
            struct flow_branch* _Opt p_throw_snapshot =
                flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_throw_join_map,
                                    FLOW_BRANCH_THROW_JOIN);

            flow_branch_accumulate_into_join(ctx->p_throw_join_map,
                                            ctx->p_current_flow_branch,
                                            p_throw_snapshot, false, ctx->throw_join_reached);
            ctx->throw_join_reached = true;
            
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list,
                                             p_jump_statement->first_token);

            flow_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list,
                                                p_jump_statement->first_token);

            if (ctx->p_current_flow_branch != NULL)
            {
                ctx->p_current_flow_branch->is_unreachable = true;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
        {
            if (p_jump_statement->expression_opt)
            {
                flow_visit_full_expression(ctx, p_jump_statement->expression_opt);

                if (ctx->p_return_type == NULL)
                {
                    bug();
                    throw;
                }

                struct object param_object = { 0 };
                make_object(ctx->p_return_type, &param_object, MAKE_STATE_UNITIALIZED, &ctx->ctx->options.platform);
                flow_check_object_init_assigment(ctx, p_jump_statement->expression_opt, &param_object, &p_jump_statement->expression_opt->object, INIT_RETURN, false, false);
                object_destroy(&param_object);
            }

            flow_check_function_exit(ctx, p_jump_statement);

            if (ctx->p_current_flow_branch != NULL)
            {
                ctx->p_current_flow_branch->is_unreachable = true;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
        {
            if (ctx->p_continue_join_map != NULL)
            {
                /* one snapshot per continue, as for throw: the body map becomes unreachable below */
                struct flow_branch* _Opt p_continue_snapshot =
                    flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_continue_join_map,
                                          FLOW_BRANCH_CONTINUE_JOIN);

                flow_branch_accumulate_into_join(ctx->p_continue_join_map,
                                                 ctx->p_current_flow_branch,
                                                 p_continue_snapshot, false, ctx->continue_join_reached);
                ctx->continue_join_reached = true;
            }

            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            flow_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

            if (ctx->p_current_flow_branch != NULL)
            {
                ctx->p_current_flow_branch->is_unreachable = true;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            if (ctx->p_break_join_map != NULL)
            {
                flow_branch_accumulate_into_join(ctx->p_break_join_map, ctx->p_current_flow_branch, NULL, false, ctx->break_join_reached);
                ctx->break_join_reached = true;
            }

            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            flow_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

            if (ctx->p_current_flow_branch != NULL)
            {
                ctx->p_current_flow_branch->is_unreachable = true;
            }
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
        {
            _Assert(p_jump_statement->label != NULL);

            bool found = false;
            for (int i = 0; i < ctx->labels_size; i++)
            {
                if (strcmp(ctx->labels[i].label_name, p_jump_statement->label->lexeme) == 0)
                {
                    flow_branch_accumulate_into_join(ctx->labels[i].p_flow_branch,
                                                  ctx->p_current_flow_branch, NULL, true, false);
                    found = true;
                    break;
                }
            }

            if (!found &&
                    ctx->labels_size < (int)(sizeof(ctx->labels) / sizeof(ctx->labels[0])))
            {
                struct flow_branch* _Opt p_label_map =
                    flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_GOTO_LABEL);

                if (p_label_map == NULL) throw;
                
                /*
                    Snapshot state at this goto into the label's map, as later jumps do
                    (flow_branch_accumulate_into_join): the parent chain keeps changing.
                */
                flow_branch_accumulate_into_join(p_label_map, ctx->p_current_flow_branch, NULL, true, false);

                ctx->labels[ctx->labels_size].label_name = p_jump_statement->label->lexeme;
                ctx->labels[ctx->labels_size].p_flow_branch = p_label_map;
                ctx->labels_size++;
                
            }

            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
            flow_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

            if (ctx->p_current_flow_branch != NULL)
            {
                ctx->p_current_flow_branch->is_unreachable = true;
            }
        }
        else
        {
            _Assert(false);
        }

        if (p_jump_statement->p_lint_token)
        {
            flow_check_dianostic_suppression(ctx, p_jump_statement->p_lint_token);
        }
    }
    catch
    {
    }    
}

static void flow_visit_labeled_statement(struct flow_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    flow_visit_label(ctx, p_labeled_statement->label);
    flow_visit_statement(ctx, p_labeled_statement->statement);
}

static void flow_visit_primary_block(struct flow_ctx* ctx, struct primary_block* p_primary_block)
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

static void flow_visit_unlabeled_statement(struct flow_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        flow_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        flow_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->defer_statement)
    {
        /* nothing */
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        flow_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        _Assert(false);
    }
}

static void flow_visit_statement(struct flow_ctx* ctx, struct statement* p_statement)
{
    if (ctx->stopped)
        return;

    if (p_statement->labeled_statement)
    {
        flow_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        flow_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void flow_visit_label(struct flow_ctx* ctx, const struct label* p_label)
{
    try
    {
        if (p_label->p_identifier_opt)
        {
            for (int i = 0; i < ctx->labels_size; i++)
            {
                if (strcmp(ctx->labels[i].label_name, p_label->p_identifier_opt->lexeme) == 0)
                {
                    /*
                        A label is reached by goto (already in p_flow_branch) or by fall-through:
                        union both.
                    */
                    flow_branch_accumulate_into_join(ctx->labels[i].p_flow_branch, ctx->p_current_flow_branch, NULL, true, false);
                    ctx->p_current_flow_branch = ctx->labels[i].p_flow_branch;
                    break;
                }
            }
        }
        else
        {
            /* case/default — always start from the pre-switch state */
            if (ctx->p_initial_map == NULL)
            {
                return;
            }

            /*
                The previous case falls through into this label unless it ended with a jump
                (unreachable) or there is no previous case (still the switch body map).
            */
            struct flow_branch* _Opt p_fallthrough = ctx->p_current_flow_branch;
            if (p_fallthrough != NULL &&
                (p_fallthrough->is_unreachable ||
                 (p_fallthrough->kind == FLOW_BRANCH_SWITCH_BODY && p_fallthrough->p_parent_map == ctx->p_initial_map)))
            {
                p_fallthrough = NULL;
            }

            if (p_label->constant_expression == NULL)
            {
                /*
                    `default:` needs its own child map: reusing p_initial_map corrupts it
                    when the body ends with a jump.
                */
                struct flow_branch* _Opt p_default_map =
                    flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_initial_map, FLOW_BRANCH_DEFAULT);
                if (p_default_map != NULL && p_fallthrough != NULL)
                {
                    flow_branch_accumulate_into_join(p_default_map, p_fallthrough, NULL, false, true);
                }
                ctx->p_current_flow_branch = p_default_map;
                return;
            }

            /* case N: — create a child map that records the switch variable == N */
            struct flow_branch* _Opt p_case_map =
                flow_branch_arena_new(&ctx->flow_branch_arena,
                                   ctx->p_initial_map,
                                   FLOW_BRANCH_CASE);

            if (p_case_map == NULL)
            {
                throw;
            }

            /*
                ctx->p_switch_object_key is the object being switched on,
                set when entering the switch statement
            */
            if (ctx->p_switch_obj_key != NULL &&
                    object_has_constant_value(&p_label->constant_expression->object))
            {
                signed long long case_value =
                    object_to_signed_long_long(&p_label->constant_expression->object);

                {
                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(p_case_map, ctx->p_switch_obj_key);
                    if (e == NULL)
                        return; /* no entry to seed for this case */

                    flow_alternatives_clear(&e->alternatives);
                    struct flow_alternative a =
                    {
                        .value_kind = FLOW_VALUE_KIND_SIGNED,
                        .value = {.i = case_value},
                        .value_relation = FLOW_RELATION_EQUAL,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = p_case_map,
                        .p_origin_token = p_label->constant_expression->first_token
                    };
                    flow_alternatives_add(&e->alternatives, &a);
                }
            }

            if (p_fallthrough != NULL)
            {
                flow_branch_accumulate_into_join(p_case_map, p_fallthrough, NULL, false, true);
            }

            ctx->p_current_flow_branch = p_case_map;
        }
    }
    catch
    {
    }
}

/* 'name', or the type when the object has no name (a pointee, a temporary). */
static void flow_print_object_name_or_type(struct osstream* ss, const struct object* _Opt p, const struct platform* target)
{
    if (p == NULL)
        ss_fprintf(ss, "?");
    else if (p->member_designator && p->member_designator[0])
        ss_fprintf(ss, "'%s'", p->member_designator);
    else
    {
        struct osstream type = { 0 };
        print_type_no_names(&type, &p->type, target);
        ss_fprintf(ss, "an object of type '%s'", type.c_str ? type.c_str : "");
        ss_close(&type);
    }
}

/*
    Stops the analysis of the current function when it tracks too many objects or
    values, reporting the object that holds most of them.
*/
static void flow_check_limits(struct flow_ctx* ctx, const struct token* p_token)
{
    enum { FLOW_MAX_TRACKED_OBJECTS = 10000, FLOW_MAX_VALUES = 1000000 };

    if (ctx->stopped)
        return;

    const int values = flow_alternatives_live_count();
    if (values > FLOW_MAX_VALUES)
    {
        const struct object* _Opt p_largest = NULL;
        int largest = 0;
        struct flow_branch_entries it = flow_branch_entries_begin(ctx->p_current_flow_branch, NULL);
        for (const struct flow_key_alternatives* _Opt e; (e = flow_branch_entries_next(&it)) != NULL;)
        {
            if (e->alternatives.size > largest)
            {
                largest = e->alternatives.size;
                p_largest = e->p_obj_key;
            }
        }
        struct osstream name = { 0 };
        flow_print_object_name_or_type(&name, p_largest, &ctx->ctx->options.platform);

        const struct marker m = { .p_token_begin = p_token, .p_token_end = p_token };
        diagnostic(W_FLOW_NOT_DONE, ctx->ctx, NULL, &m,
                   "flow analysis of this function was not done: %d values tracked (limit %d), %s has %d of them here",
                   values, FLOW_MAX_VALUES, name.c_str ? name.c_str : "", largest);
        ss_close(&name);
        ctx->stopped = true;
        return;
    }

    int entries = 0;
    for (const struct flow_branch* _Opt p = ctx->p_current_flow_branch; p; p = p->p_parent_map)
        entries += p->num_of_entries;

    if (entries <= FLOW_MAX_TRACKED_OBJECTS)
        return;

    struct { const struct object* p_root; int count; } roots[16] = { 0 };
    struct flow_branch_entries it = flow_branch_entries_begin(ctx->p_current_flow_branch, NULL);
    for (const struct flow_key_alternatives* _Opt e; (e = flow_branch_entries_next(&it)) != NULL;)
    {
        const struct object* p_root = e->p_obj_key;
        while (p_root->parent)
            p_root = p_root->parent;
        for (int k = 0; k < 16; k++)
        {
            if (roots[k].p_root == NULL)
                roots[k].p_root = p_root;
            if (roots[k].p_root == p_root)
            {
                roots[k].count++;
                break;
            }
        }
    }

    int top = 0;
    for (int k = 1; k < 16; k++)
    {
        if (roots[k].count > roots[top].count)
            top = k;
    }

    struct osstream name = { 0 };
    flow_print_object_name_or_type(&name, roots[top].p_root, &ctx->ctx->options.platform);

    const struct marker m = { .p_token_begin = p_token, .p_token_end = p_token };
    diagnostic(W_FLOW_NOT_DONE, ctx->ctx, NULL, &m,
               "flow analysis of this function was not done: %d objects tracked (limit %d), %d of them are parts of %s",
               entries, FLOW_MAX_TRACKED_OBJECTS, roots[top].count, name.c_str ? name.c_str : "");
    ss_close(&name);
    ctx->stopped = true;
}

static void flow_visit_block_item(struct flow_ctx* ctx, struct block_item* p_block_item)
{
    if (ctx->stopped)
        return;

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

    flow_check_limits(ctx, p_block_item->first_token);
}

static void flow_visit_block_item_list(struct flow_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* _Opt p_block_item = p_block_item_list->head;

    /* Warn once per run of dead code (map is_unreachable until a label). */
    bool warned_unreachable = false;

    while (p_block_item)
    {
        if (!warned_unreachable &&
                p_block_item->label == NULL &&
                ctx->p_current_flow_branch != NULL &&
                ctx->p_current_flow_branch->is_unreachable)
        {
            const struct marker m =
            {
                .p_token_begin = p_block_item->first_token,
                .p_token_end = p_block_item->first_token
            };
            diagnostic(W_FLOW_UNREACHABLE_CODE, ctx->ctx, NULL, &m, "unreachable code");
            warned_unreachable = true;
        }

        flow_visit_block_item(ctx, p_block_item);

        if (p_block_item->label != NULL &&
                ctx->p_current_flow_branch != NULL &&
                !ctx->p_current_flow_branch->is_unreachable)
        {
            /*
                label arrival (goto and/or fall-through) made the map
                live again; a later jump can start a new dead-code run
            */
            warned_unreachable = false;
        }

        p_block_item = p_block_item->next;
    }
}

static void flow_visit_pragma_declaration(struct flow_ctx* ctx, struct pragma_declaration* p_pragma_declaration)
{
    execute_pragma_declaration(ctx->ctx, p_pragma_declaration);
}

static void object_static_debug(struct flow_ctx* ctx,
                                const struct object* p_object,
                                struct token* first_token,
                                struct token* last_token)
{
    const struct object* _Opt member = p_object->members.head;
    if (member)
    {
        while (member)
        {
            object_static_debug(ctx, member, first_token, last_token);
            member = member->next;
        }
        return;
    }

    struct flow_key_alternatives* _Opt p_entry = flow_branch_search_up(ctx->p_current_flow_branch, p_object);
    if (p_entry == NULL)
    {
        const struct marker m =
        {
            .p_token_begin = first_token,
            .p_token_end = last_token
        };

        struct osstream ss = { 0 };
        print_object_ptr(&ss, p_object, false);
        diagnostic(W_LOCATION, ctx->ctx, NULL, &m, "%s", ss.c_str);
        ss_close(&ss);
        return;
    }

    for (int i = 0; i < p_entry->alternatives.size; i++)
    {
        struct flow_alternative* p_flow_alternative = p_entry->alternatives.data[i];


        if (p_flow_alternative->value_kind == FLOW_VALUE_KIND_REF)
        {
            if (p_flow_alternative->value.p != NULL)
            {
                object_static_debug(ctx, p_flow_alternative->value.p, first_token, last_token);
            }
            continue;
        }

        const struct marker m =
        {
            .p_token_begin = first_token,
            .p_token_end = last_token
        };

        struct osstream key_ss = { 0 };

        if (p_entry->p_obj_key->member_designator)
        {
            print_object_ptr(&key_ss, p_entry->p_obj_key, false);
        }

        struct osstream ss = { 0 };
        flow_alternative_sprint(&ss, p_flow_alternative);

        diagnostic(W_LOCATION, ctx->ctx, NULL, &m, "%s %s",
                   key_ss.c_str ? key_ss.c_str : "",
                   ss.c_str);
        ss_close(&key_ss);
        ss_close(&ss);
    }
}

static void check_object_true(struct flow_ctx* ctx, const struct object* p_object, const struct token* p_position_token)
{
    struct flow_key_alternatives* _Opt p_entry = flow_branch_search_up(ctx->p_current_flow_branch, p_object);
    if (p_entry == NULL)
        return;

    for (int i = 0; i < p_entry->alternatives.size; i++)
    {
        const struct flow_alternative* alt = p_entry->alternatives.data[i];
        if (alt->value_kind == FLOW_VALUE_KIND_REF)
        {
            check_object_true(ctx, alt->value.p, p_position_token);
            continue;
        }

        if (!flow_alternative_is_true(alt))
        {
            struct osstream ss = { 0 };
            flow_explain_alternative_not_true(&ss, alt, p_position_token->line);
            /*
                Warning 67 (configurable, suppressible with //lint), not error 1080
                (logical-or-per-path.c).
            */
            /* the conditions assumed along the way are printed as "Assuming ..." notes */
            if (diagnostic(W_COMPILE_ASSERT_UNPROVEM, ctx->ctx,
                           p_position_token, NULL, "compile_assert failed: %s", ss.c_str))
            {
                flow_diagnose_map_path(ctx, alt->p_origin_map);
            }
            ss_close(&ss);
        }
    }
}

static void flow_visit_compile_assert(struct flow_ctx* ctx, const struct static_assertion* p_static_assertion)
{
    check_object_true(ctx,
                      &p_static_assertion->constant_expression->object,
                      p_static_assertion->first_token);
}

static void flow_visit_static_assertion(struct flow_ctx* ctx, const struct static_assertion* p_static_assertion)
{
    if (p_static_assertion->first_token->type == TK_KEYWORD_RUNTIME_ASSERT)
    {
        /*
            _Assert(cond): only the true arm continues (a failed _Assert aborts).
            A keyword, so it narrows in every build. Same as `if (!(cond)) return;`
            in flow_visit_if_statement.
        */
        if (ctx->p_current_flow_branch == NULL)
            return;

        struct flow_branch* p_before = ctx->p_current_flow_branch;
        struct flow_true_false_branches pair = flow_visit_full_expression(ctx, p_static_assertion->constant_expression);
        /*
            As in flow_visit_if_statement: give pair.p_true its own child map when
            it aliases p_before, or merge_arms's cleanup frees ancestor state.
        */
        pair = flow_ensure_branch_pair(ctx, p_before, pair, p_static_assertion->constant_expression);
        if (pair.p_true != NULL)
        {
            flow_branch_merge_a_b(p_before, pair.p_true, pair.p_true);
        }
        ctx->p_current_flow_branch = p_before;
        return;
    }

    const bool t2 = ctx->expression_is_not_evaluated;
    ctx->expression_is_not_evaluated = true;

    flow_visit_full_expression(ctx, p_static_assertion->constant_expression);

    ctx->expression_is_not_evaluated = t2; /* restore */

    /* a loop's first pass only warms up state; print on the reporting pass */
    if ((p_static_assertion->first_token->type == TK_KEYWORD_CAKE_STATIC_DEBUG ||
            p_static_assertion->first_token->type == TK_KEYWORD_CAKE_STATIC_DEBUG_EX) &&
            ctx->iteration_pass != 1)
    {
        if (object_is_zero(&p_static_assertion->constant_expression->object))
        {
            flow_branch_debug_print(ctx->p_current_flow_branch, 0);
        }
        else
        {
            flow_expression_static_debug(ctx, p_static_assertion->constant_expression);
        }
    }
    else if (p_static_assertion->first_token->type == TK_KEYWORD__COMPILE_ASSERT)
    {
        flow_visit_compile_assert(ctx, p_static_assertion);
    }

    /*
        compile_assert's diagnostic is queued above, during flow analysis, so
        its `//lint N` has to be checked here at phase 2. parser.c checks the
        same token at phase 0, which is too early to see it.
    */
    if (p_static_assertion->p_lint_token)
    {
        flow_check_dianostic_suppression(ctx, p_static_assertion->p_lint_token);
    }
}

static void flow_visit_direct_declarator(struct flow_ctx* ctx, const struct direct_declarator* p_direct_declarator)
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
                flow_visit_declaration_specifiers(ctx, parameter->declaration_specifiers);
                flow_visit_declarator(ctx, parameter->declarator);
                flow_parameter_object_init(ctx, &parameter->declarator->object, &parameter->declarator->object.type, parameter->declaration_specifiers->first_token);
            }
            parameter = parameter->next;
        }

        ctx->parameter_list--;
    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            flow_visit_full_expression(ctx, p_direct_declarator->array_declarator->assignment_expression);
        }
    }
}

static void flow_visit_declarator(struct flow_ctx* ctx, const struct declarator* p_declarator)
{
    if (p_declarator->object.type.category != TYPE_CATEGORY_FUNCTION)
    {
        if (ctx->parameter_list > 1)
        {
            /*
                Parameters of a function-pointer parameter are not in scope:
                `void x_destroy(void (*f)(void * _Owner p))` adds f, not p.
            */
            return;
        }
    }

    if (p_declarator->direct_declarator)
    {
        flow_visit_direct_declarator(ctx, p_declarator->direct_declarator);
    }
}

static void flow_visit_init_declarator_list(struct flow_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_init_declarator);
        p_init_declarator = p_init_declarator->next;
    }
}

static void flow_visit_member_declarator(struct flow_ctx* ctx, const struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        flow_visit_declarator(ctx, p_member_declarator->declarator);
    }
}

static void flow_visit_member_declarator_list(struct flow_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* _Opt p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        flow_visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}

static void flow_visit_member_declaration(struct flow_ctx* ctx, struct member_declaration* p_member_declaration)
{
    if (p_member_declaration->member_declarator_list_opt)
    {
        flow_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void flow_visit_member_declaration_list(struct flow_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* _Opt p_member_declaration = p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    flow_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);
}

static void flow_visit_enumerator(struct flow_ctx* ctx, const struct enumerator* p_enumerator)
{
    if (p_enumerator->constant_expression_opt)
    {
        flow_visit_full_expression(ctx, p_enumerator->constant_expression_opt);
    }
}

static void flow_visit_enumerator_list(struct flow_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* _Opt current = p_enumerator_list->head;
    while (current)
    {
        flow_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow_visit_enum_specifier(struct flow_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    flow_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void flow_visit_type_specifier(struct flow_ctx* ctx, struct type_specifier* p_type_specifier)
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

static void flow_visit_type_specifier_qualifier(struct flow_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_specifier)
    {
        flow_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
}

static void flow_visit_declaration_specifier(struct flow_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{
    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);
    }
}

static void flow_visit_declaration_specifiers(struct flow_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers)
{
    struct declaration_specifier* _Opt p_declaration_specifier = p_declaration_specifiers->head;
    while (p_declaration_specifier)
    {
        flow_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}

static void flow_check_object_at_exit(struct flow_ctx* ctx,
                                      const struct type* p_type,
                                      const struct object* p_obj,
                                      const struct marker* marker,
                                      const struct token* p_exit_token,
                                      bool in_view,
                                      const char* _Opt p_root_name_opt,
                                      bool root_is_pointer)
{
    /* Under a _View ancestor, _Owner members are views: not reported at exit. */
    const bool view = in_view || type_is_view(p_type);

    if (p_obj->members.head)
    {
        for (const struct object* _Opt member = p_obj->members.head; member; member = member->next)
        {
            flow_check_object_at_exit(ctx,
                                      &member->type,
                                      member,
                                      marker,
                                      p_exit_token,
                                      view,
                                      p_root_name_opt,
                                      root_is_pointer);
        }
        return;
    }

    const bool is_owner = type_is_owner(&p_obj->type) && !view;

    const struct flow_key_alternatives* _Opt e = flow_branch_search_up(ctx->p_current_flow_branch, p_obj);
    if (e == NULL) return;

    /*
        One report per object at this exit, not per alternative. Joins (does
        not clear) the table: the return's own findings must survive.
    */
    flow_findings_join(ctx);
    const struct marker exit_marker =
    {
        .p_token_caret = p_exit_token,
        .p_token_begin = p_exit_token,
        .p_token_end = p_exit_token,
    };

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow_alternative* p_alternative = e->alternatives.data[i];

        if (p_alternative->value_kind == FLOW_VALUE_KIND_REF)
        {
            /* IS THIS POSSIBLE? */
            if (p_alternative->value.p != p_obj)
            {
                flow_check_object_at_exit(ctx,
                                          p_type,
                                          p_alternative->value.p,
                                          marker,
                                          p_exit_token,
                                          view,
                                          p_root_name_opt,
                                          true);
                continue;
            }
        }

        if (is_owner &&
                p_alternative->imaginary != FLOW_IMAGINARY_MOVED &&
                p_alternative->imaginary != FLOW_IMAGINARY_ENDED)
        {
            /* ENDED (a _Dtor call) resolves an owner, like MOVED. */
            if (flow_alternative_is_zero(p_alternative) ||
                    p_alternative->value_relation == FLOW_RELATION_UNINITIALIZED)
            {
                /* UNINITIALIZED owns nothing: not a leak (owner-resource-074.c). */
            }
            else
            {
                const char* member_suffix = p_obj->member_designator ? p_obj->member_designator : "";
                /* Combine member_designator ('.m') with p_root_name_opt: 'p->m' or 'x.m'. */
                char object_name_buf[256] = { 0 } ;
                const char* object_name;
                if (p_root_name_opt != NULL && p_root_name_opt[0] != 0 &&
                        strcmp(p_root_name_opt, member_suffix) != 0)
                {
                    /*
                        A suffix without leading '.' is the root itself: don't concatenate
                        ('p.p').
                    */
                    if (member_suffix[0] == '.')
                    {
                        snprintf(object_name_buf, sizeof(object_name_buf), "%s%s%s", p_root_name_opt, root_is_pointer ? "->" : ".", member_suffix + 1);
                    }
                    else if (member_suffix[0] != 0)
                    {
                        object_name_buf[0] = 0;
                        snprintf(object_name_buf, sizeof(object_name_buf), "%s", member_suffix);
                    }
                    else
                    {
                        snprintf(object_name_buf, sizeof(object_name_buf), "%s", p_root_name_opt);
                    }
                    object_name = object_name_buf;
                }
                else
                {
                    object_name = member_suffix;
                }
                /*
                    W_FLOW_MISSING_DTOR (29), not 31: a leak is the inverse of
                    "used after lifetime ended", separately suppressible.
                */
                /* Report the leak at the exit point, where the fix goes. */
                struct osstream ss = { 0 };
                if (object_name[0] == '\0')
                    ss_fprintf(&ss, "unnamed parameter lifetime ended while still owning its resource");
                else
                    ss_fprintf(&ss, "'%s' lifetime ended while still owning its resource", object_name);
                flow_finding_record(ctx, p_obj, W_FLOW_MISSING_DTOR, &exit_marker,
                                    p_alternative, p_alternative->p_origin_map,
                                    ss.c_str ? ss.c_str : "");
                ss_close(&ss);
            }
        }

    }

    flow_findings_end(ctx);
}

static void flow_check_write_qualifier_placement(const struct flow_ctx* ctx,
                                                 const struct type* _Opt p_type,
                                                 const struct token* _Opt p_token)
{
    if (p_type == NULL || p_token == NULL)
        return;

    /*
        A function declarator's own type is a function type; its qualifiers
        belong to the return type, which is checked in its own right.
    */
    if (type_is_function(p_type))
        return;

    if (!type_is_pointer(p_type))
    {
        if (type_is_clear(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                       "_Clear qualifies the pointed object: write 'T * _Clear' as '_Clear T *'");
        }
        else if (type_is_dtor(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                       "_Dtor qualifies the pointed object: write 'T * _Dtor' as '_Dtor T *'");
        }
        else if (type_is_out(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                       "_Out qualifies the pointed object: write 'T * _Out' as '_Out T *'");
        }
        else if (type_is_uninit(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                       "_Uninitialized qualifies the pointed object: write 'T * _Uninitialized' as '_Uninitialized T *'");
        }
        return;
    }

    if (!type_is_pointed_const(p_type))
        return;

    if (type_is_pointed_clear(p_type))
    {
        diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
                   "_Clear pointee cannot also be const");
    }
    else if (type_is_pointed_dtor(p_type))
    {
        diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
                   "_Dtor pointee cannot also be const");
    }
    else if (type_is_pointed_out(p_type))
    {
        diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
                   "_Out pointee cannot also be const");
    }
    else if (type_is_pointed_uninit(p_type))
    {
        diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_CANNOT_BE_CONST, ctx->ctx, p_token, NULL,
                   "_Uninitialized pointee cannot also be const");
    }
}

static void flow_check_write_qualifier_parameters(const struct flow_ctx* ctx, struct declarator* p_declarator)
{
    const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(&p_declarator->object.type);
    if (p_param_list == NULL)
    {
        return;
    }

    struct token* _Opt p_token = p_declarator->first_token_opt ? p_declarator->first_token_opt : p_declarator->name_opt;

    for (const struct param* _Opt p_current_parameter_type = p_param_list->head;
            p_current_parameter_type;
            p_current_parameter_type = p_current_parameter_type->next)
    {
        flow_check_write_qualifier_placement(ctx, &p_current_parameter_type->type, p_token);
    }
}

static void flow_check_write_qualifier_declarator(const struct flow_ctx* ctx, struct declarator* p_declarator)
{
    struct token* _Opt p_token = p_declarator->first_token_opt ? p_declarator->first_token_opt : p_declarator->name_opt;
    flow_check_write_qualifier_placement(ctx, &p_declarator->object.type, p_token);
}

void flow_visit_declaration(struct flow_ctx* ctx, struct declaration* p_declaration)
{
    try
    {
        if (p_declaration->static_assertion)
        {
            flow_visit_static_assertion(ctx, p_declaration->static_assertion);
        }

        if (p_declaration->pragma_declaration)
        {
            flow_visit_pragma_declaration(ctx, p_declaration->pragma_declaration);
        }

        if (p_declaration->declaration_specifiers)
        {
            if (p_declaration->init_declarator_list.head)
            {
                flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers);
            }
            else
            {
                flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers);

            }
        }

        if (p_declaration->init_declarator_list.head)
        {
            flow_check_write_qualifier_parameters(ctx, p_declaration->init_declarator_list.head->p_declarator);

            /* ...and the declarators themselves (moved from parser.c). */
            for (struct init_declarator* _Opt p_id = p_declaration->init_declarator_list.head;
                    p_id;
                    p_id = p_id->next)
            {
                flow_check_write_qualifier_declarator(ctx, p_id->p_declarator);
            }
        }

        if (p_declaration->init_declarator_list.head)
        {
            flow_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
        }

        if (p_declaration->function_body)
        {
            _Assert(ctx->p_return_type == NULL);
            if (p_declaration->init_declarator_list.head == NULL) throw;

            struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->object.type);
            ctx->p_return_type = &type;

            ctx->p_current_function_declaration = p_declaration;

            /*
                _core: the body's trailing `//lint N` is checked below instead,
                after the fall-off-the-end exit diagnostics have been queued.
                See flow_visit_compound_statement_core.
            */
            flow_visit_compound_statement_core(ctx, p_declaration->function_body);

            if (!ctx->stopped && !compound_statement_is_last_item_return(p_declaration->function_body))
            {
                /* the body's defers already ran in flow_visit_compound_statement_core */
                /* flow_check_params_at_function_exit(ctx, p_declaration); */
                flow_check_arena_objects_at_function_exit(ctx);
                const struct marker marker =
                {
                    .p_token_begin = p_declaration->function_body->last_token,
                    .p_token_end = p_declaration->function_body->last_token
                };
                flow_check_file_scope_objects_at_function_exit(ctx);
                /*
                    Falling off the end is an exit: check _Clear/_Dtor contracts. Before
                    p_current_function_declaration is cleared, and before
                    flow_defer_list_set_end_of_lifetime (as in flow_check_function_exit).
                */
                flow_check_write_qualified_params_at_exit(ctx, &marker, p_declaration->function_body->last_token);
                flow_defer_list_set_end_of_lifetime(ctx, &p_declaration->function_body->defer_list, p_declaration->function_body->last_token);
                flow_check_exiting_objects(ctx, NULL, &marker);
            }

            if (p_declaration->function_body->lint_token)
            {
                flow_check_dianostic_suppression(ctx, p_declaration->function_body->lint_token);
            }

            ctx->p_current_function_declaration = NULL;
            type_destroy(&type);
            ctx->p_return_type = NULL;
        }

        if (p_declaration->lint_token)
        {
            flow_check_dianostic_suppression(ctx, p_declaration->lint_token);
        }
    }
    catch
    {
    }
}

void flow_start_visit_declaration(struct flow_ctx* ctx, struct declaration* p_declaration)
{
    try
    {
        ctx->labels_size = 0;
        ctx->stopped = false;
        ctx->collect_deferred_effects = false;
        ctx->deferred_effects_count = 0;

        flow_allocated_object_arena_clear(&ctx->allocated_object_arena);
        flow_branch_arena_clear(&ctx->flow_branch_arena);

        ctx->p_current_flow_branch = flow_branch_arena_new(&ctx->flow_branch_arena, NULL, FLOW_BRANCH_ROOT);
        if (ctx->p_current_flow_branch == NULL)
            throw;

        ctx->function_start_time = clock();

        flow_visit_declaration(ctx, p_declaration);

        const long elapsed_ms = (long)((clock() - ctx->function_start_time) * 1000 / CLOCKS_PER_SEC);
        const struct init_declarator* _Opt p_init_declarator = p_declaration->init_declarator_list.head;
        const struct token* _Opt p_name_opt = p_init_declarator ? p_init_declarator->p_declarator->name_opt : NULL;
        if (p_declaration->function_body && p_name_opt && elapsed_ms > 5000)
        {
            const struct marker m =
            {
                .p_token_begin = p_name_opt,
                .p_token_end = p_name_opt
            };
            diagnostic(W_INFO, ctx->ctx, NULL, &m,
                       "flow analysis of '%s' (line %d) took %ld ms", p_name_opt->lexeme, p_name_opt->line, elapsed_ms);
        }

        flow_allocated_object_arena_clear(&ctx->allocated_object_arena);
        flow_branch_arena_clear(&ctx->flow_branch_arena);
        ctx->p_current_flow_branch = NULL;
    }
    catch
    {
    }
}

void flow_visit_ctx_destroy(_Dtor struct flow_ctx* ctx)
{
    ss_close(&ctx->findings_text);
    free(ctx->p_reported_findings);
    flow_allocated_object_arena_clear(&ctx->allocated_object_arena);
    flow_branch_arena_clear(&ctx->flow_branch_arena);
    flow_alternatives_pool_shutdown();
}

#undef object_get_referenced
