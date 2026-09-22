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

#define FLOW_PARAMETER_OBJECT_INIT_MAX_DEPTH 6


 
static void flow_check_dianostic_suppression(struct flow_ctx* ctx, const struct token* p_token);

static void flow_check_file_scope_objects_at_function_exit(const struct flow_ctx* ctx);

static struct flow_branch_pair flow_visit_expression(struct flow_ctx* ctx, const struct expression* _Opt p_expression);
static void object_static_debug(struct flow_ctx* ctx, const struct object* p_object, struct token* first_token, struct token* last_token);

static void flow_check_object_at_exit(struct flow_ctx* ctx, const struct type* p_type, const struct object* p_obj, const struct marker* marker, const struct token* p_exit_token, bool in_view, const char* _Opt p_root_name_opt);
static void flow_check_arena_objects_at_function_exit(const struct flow_ctx* ctx);
static void flow_check_write_qualified_params_at_exit(struct flow_ctx* ctx, const struct marker* marker, const struct token* p_exit_token);
static void flow_check_discarding_owner_before_overwrite(struct flow_ctx* ctx, const struct expression* p_expression_dest, const struct object* _Opt p_object_dest, const struct marker* marker);
static void flow_seed_member_default(struct flow_ctx* ctx, const struct object* _Opt member_obj, const struct token* _Opt p_token);

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
        enum sizeof_result r = type_get_sizeof(target_type, &width, ctx->ctx->options.target);
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

    char temp[16];
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
            return false;
    }
}

static void flow_predicate_cache_reset(struct flow_ctx* ctx)
{
    ctx->predicate_cache_size = 0;
}

/* Drop any cached predicate that mentions p_obj -- its truth may have changed. */
static void flow_predicate_invalidate(struct flow_ctx* ctx, const struct object* _Opt p_obj)
{
    if (p_obj == NULL)
        return;
    int w = 0;
    for (int r = 0; r < ctx->predicate_cache_size; r++)
    {
        if (ctx->predicate_cache[r].left_obj == p_obj || ctx->predicate_cache[r].right_obj == p_obj)
            continue; /* drop this entry */
        ctx->predicate_cache[w++] = ctx->predicate_cache[r];
    }
    ctx->predicate_cache_size = w;
}

static bool flow_predicate_key(const struct expression* _Opt p_cond,
                               enum expression_type* op,
                               const struct object* _Opt* left_obj,
                               const struct object* _Opt* right_obj,
                               long long* right_const)
{
    if (p_cond == NULL)
        return false;

    switch (p_cond->expression_type)
    {
        case EXPR_EQUALITY_EQUAL:
        case EXPR_EQUALITY_NOT_EQUAL:
        case EXPR_RELATIONAL_BIGGER_THAN:
        case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
        case EXPR_RELATIONAL_LESS_THAN:
        case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
        break;
        default:
            return false;
    }

    if (p_cond->left == NULL || p_cond->right == NULL)
        return false;

    /* object_get_referenced returns its argument when there is no ref, so it
       never yields null -- no guard needed. */
    const struct object* l = object_get_referenced(&p_cond->left->object);

    *op = p_cond->expression_type;
    *left_obj = l;

    if (object_has_known_value(&p_cond->right->object))
    {
        *right_obj = NULL;
        *right_const = object_to_signed_long_long(&p_cond->right->object);
    }
    else
    {
        const struct object* r = object_get_referenced(&p_cond->right->object);
        *right_obj = r;
        *right_const = 0;
    }
    return true;
}

static int flow_predicate_shared_id(struct flow_ctx* ctx, const struct expression* p_cond, int fresh_id)
{
    enum expression_type op = EXPR_INVALID;
    const struct object* _Opt lo = NULL;
    const struct object* _Opt ro = NULL;
    long long rc = 0;
    if (!flow_predicate_key(p_cond, &op, &lo, &ro, &rc))
        return fresh_id;

    for (int i = 0; i < ctx->predicate_cache_size; i++)
    {
        if (ctx->predicate_cache[i].op == op &&
                ctx->predicate_cache[i].left_obj == lo &&
                ctx->predicate_cache[i].right_obj == ro &&
                ctx->predicate_cache[i].right_const == rc)
        {
            return ctx->predicate_cache[i].branch_id; /* reuse */
        }
    }

    if (ctx->predicate_cache_size < (int)(sizeof ctx->predicate_cache / sizeof ctx->predicate_cache[0]))
    {
        struct flow_predicate_entry* e = &ctx->predicate_cache[ctx->predicate_cache_size];
        e->op = op;
        e->left_obj = lo;
        e->right_obj = ro;
        e->right_const = rc;
        e->branch_id = fresh_id;
        ctx->predicate_cache_size++;
    }
    return fresh_id;
}

#define FLOW_BRANCH_PATH_MAX_CHAIN 128

static void flow_diagnose_map_path(const struct flow_ctx* ctx, const struct flow_branch* _Opt map)
{
    if (map == NULL)
        return;

    /* Collect deepest-first then emit backwards, so notes read root-first,
       in the order control flow took them (same shape as
       flow_explain_origin / flow_branch_debug_print). */
    const struct flow_branch* chain[FLOW_BRANCH_PATH_MAX_CHAIN];
    int chain_len = 0;
    for (const struct flow_branch* _Opt cur = map; cur; cur = cur->p_parent_map)
    {
        if (chain_len < FLOW_BRANCH_PATH_MAX_CHAIN)
            chain[chain_len] = cur;
        chain_len++;
    }
    const int collected = chain_len < FLOW_BRANCH_PATH_MAX_CHAIN ? chain_len : FLOW_BRANCH_PATH_MAX_CHAIN;

    /* Dedup (condition, side) pairs already reported: a loop's body is walked more than once, so the same branch decision lands on the chain repeatedly and printed once per visit padded a path with indistinguishable duplicate notes; keyed on expression+kind so a condition genuinely taken both ways still shows both. */
    const struct expression* seen_expr[FLOW_BRANCH_PATH_MAX_CHAIN];
    enum flow_branch_kind seen_kind[FLOW_BRANCH_PATH_MAX_CHAIN];
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

        /* Underline the whole condition (first_token..last_token) rather
           than passing just first_token as the caret: with only the first
           token the marker rendered "~~~" under `ctx` alone for a condition
           like `ctx->current->type == TK_IDENTIFIER`, pointing at a
           sub-expression that is not what the branch turned on. */
        const struct marker branch_marker =
        {
            .p_token_begin = m->p_branch_expr->first_token,
            .p_token_end = m->p_branch_expr->last_token,
        };

        const bool is_true_branch = (m->kind == FLOW_BRANCH_TRUE_BRANCH);

        /* One note per decision, naming what was assumed about the
           condition. Clang's analyzer splits this in two ("Assuming 'p' is
           null" then "Taking false branch"), which was tried here first and
           read as noise: both notes carry the same bit, printed against the
           same caret on the same line, so
             note: Assuming 'x->type==TK_STRING_LITERAL' is true
             note: Taking true branch
           says one thing twice. The assumption is the more informative half
           -- it states nullness for pointers, which is the very fact the
           warning goes on to complain about -- and the branch taken follows
           from it, so only that half is printed. */
        struct osstream cond_ss = { 0 };
        flow_expression_to_string(m->p_branch_expr, &cond_ss);
        const char* cond = cond_ss.c_str ? cond_ss.c_str : "";

        /* Phrase the assumption in terms of the condition's own type. For a
           pointer, "is null"/"is non-null" is what the reader cares about
           and what the resulting warning will talk about; for anything else
           claiming nullness would be wrong, so fall back to true/false. */
        const char* assumption;
        if (type_is_pointer(&m->p_branch_expr->object.type))
            assumption = is_true_branch ? "is non-null" : "is null";
        else
            assumption = is_true_branch ? "is true" : "is false";

        diagnostic(W_LOCATION, ctx->ctx, NULL, &branch_marker,
                   "Assuming '%s' %s%s", cond, assumption,
                   m->is_unreachable ? " (unreachable)" : "");

        ss_close(&cond_ss);
    }
}

static void flow_diagnose_state_origin(const struct flow_ctx* ctx,
                                       const struct flow_alternative* p_alternative,
                                       const struct marker* p_fallback_marker)
{
    if (p_alternative->p_origin_token == NULL)
    {
        return; /* state seeded with nothing to blame */
    }

    const struct token* _Opt p_at = p_fallback_marker->p_token_caret ?
        p_fallback_marker->p_token_caret : p_fallback_marker->p_token_begin;

    if (p_at != NULL && p_at->line == p_alternative->p_origin_token->line)
    {
        return; /* same line as the warning -- says it twice */
    }

    /* A branch map knows the condition the state was established under.
       Underline the whole condition and put the caret on the token itself,
       but only while both sit on one line -- the rendered source line comes
       from the tokens, so a span reaching onto another line would show a
       different line than the caret sits on. */
    const struct expression* _Opt p_state_expr =
        p_alternative->p_origin_map ? p_alternative->p_origin_map->p_branch_expr : NULL;

    const bool span_condition =
        p_state_expr != NULL &&
        p_state_expr->first_token->line == p_alternative->p_origin_token->line &&
        p_state_expr->last_token->line == p_alternative->p_origin_token->line;

    if (p_state_expr)
    {
        const struct marker state_marker =
        {
            .p_token_caret = p_alternative->p_origin_token,
            .p_token_begin = span_condition ? p_state_expr->first_token : p_alternative->p_origin_token,
            .p_token_end = span_condition ? p_state_expr->last_token : p_alternative->p_origin_token,
        };

        diagnostic(W_LOCATION, ctx->ctx, NULL, &state_marker,
                   p_state_expr != NULL ? "the state comes from here, in this branch"
                                        : "the state comes from here ");
    }
}

static void flow_explain_alternative(const struct flow_ctx* ctx,
                                     const struct flow_alternative* p_alternative,
                                     const struct flow_branch* _Opt p_alternative_map,
                                     const struct marker* p_marker)
{
    flow_diagnose_state_origin(ctx, p_alternative, p_marker);
    flow_diagnose_map_path(ctx, p_alternative_map);
}

static struct flow_branch_pair flow_ensure_branch_pair(struct flow_ctx* ctx,
                                                       struct flow_branch* _Opt p_fallback,
                                                       struct flow_branch_pair pair,
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

static struct flow_branch_pair flow_visit_full_expression(struct flow_ctx* ctx, const struct expression* p_expression);


static void flow_exit_block_visit_defer_item(struct flow_ctx* ctx, const struct defer_list_item* p_item, const struct token* position_token)
{
    if (p_item->defer_statement)
    {
        const int error_count = ctx->ctx->p_report->error_count;
        const int warnings_count = ctx->ctx->p_report->warnings_count;
        const int info_count = ctx->ctx->p_report->info_count;

        if (p_item->defer_statement->unlabeled_statement)
            flow_visit_unlabeled_statement(ctx, p_item->defer_statement->unlabeled_statement);

        if (error_count != ctx->ctx->p_report->error_count ||
                warnings_count != ctx->ctx->p_report->warnings_count ||
                info_count != ctx->ctx->p_report->info_count)
        {
            diagnostic(W_LOCATION, ctx->ctx, position_token, NULL, "defer end of scope");
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
                                  p_declarator->name_opt ? p_declarator->name_opt->lexeme : NULL);
    }
}

static void flow_exit_block_visit_defer_list(struct flow_ctx* ctx,
                                             const struct defer_list* p_defer_list,
                                             const struct token* position_token)
{
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
    }
    catch
    {
    }
}

static void flow_defer_list_set_end_of_lifetime(struct flow_ctx* ctx,
                                                const struct defer_list* p_defer_list,
                                                const struct token* position_token)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow_defer_item_set_end_of_lifetime(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow_visit_secondary_block(struct flow_ctx* ctx, struct secondary_block* _Opt p_secondary_block)
{
    /* _Owner _Opt in the AST: an absent secondary block is an empty body,
       so there is simply nothing to visit. Guarded here rather than at each
       of the fourteen call sites. */
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

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

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
            /* Array parameters decay to pointers per C's rule and are always non-null too -- without also checking type_is_array here, an array parameter fell through to the generic ANY seed and false-warned 'possible null pointer' when passed on to a non-optional pointer parameter. User-reported (generate_file_scope_new_name's `char new_name[]`). */
            struct object* _Opt p_pointed = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
            /* `_Opt struct X* p` (qualifier BEFORE the struct-specifier) qualifies
            the POINTEE's type, not the pointer -- p itself stays a guaranteed
            non-null pointer (that's why this whole branch, which only runs for
            a non-_Opt *pointer*, still fires). But the pointee is marked _Opt
            in the same sense a _Dtor pointee is: not guaranteed a fully-formed
            object. So members reached through it should be force_opt-tainted
            just like an _Opt *pointer*'s pointee, even though p can't be null.
            Contrast `struct X* _Opt p` (qualifier AFTER '*'), which makes p
            itself nullable and is handled entirely by the _Opt-pointer branch
            further down -- these are two independent positions for _Opt. */
            bool pointee_is_opt = false;
            if (p_pointed != NULL)
            {
                struct type pointed_type = type_is_array(p_type)
                    ? get_array_item_type(p_type)
                    : type_remove_pointer(p_type);
                pointee_is_opt = type_is_nullable(&pointed_type, nullable_enabled);
                make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
                type_destroy(&pointed_type);
            }

            struct flow_key_alternatives* _Opt ep = flow_branch_find_add(ctx->p_current_flow_branch, p_object);
            if (ep == NULL) throw;
            flow_alternatives_clear(&ep->alternatives);
            /* Single EQUAL alternative carrying the concrete arena pointer.
            * The parameter is non-null on entry and points exactly to this object. */
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
                * _Out parameter: the pointed object is uninitialized on entry —
                * that is the whole purpose of the constructor.  Seed every _Owner
                * member (and scalar leaves) as UNINITIALIZED so constructor
                * writes are treated as first initialization.
                *
                * Non-_Out parameter: seed as ANY (unknown but valid state).
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
                    /* Recurse with the pointee's ACTUAL type (not NULL). Passing
                    NULL here dropped the type, so the non-_Opt-pointer =>
                    non-null rule at the top of this function never fired for
                    the pointee: the *pp of a `T**` parameter (where *pp is a
                    non-_Opt `T*`) was left ANY = possibly-null, producing a
                    false "passing a possible null pointer '(*pp)' to
                    non-nullable pointer parameter" (parser.c:2184). With the
                    real type, a non-_Opt pointee pointer is seeded non-null. */
                    flow_parameter_object_init_r(ctx, p_pointed, &p_pointed->type, p_token, depth + 1, force_opt || pointee_is_opt);
                }
                /* else: depth cap reached (see FLOW_PARAMETER_OBJECT_INIT_MAX_DEPTH) --
                leave p_pointed's members in the ANY state make_object already
                gave them above, without recursing into their own pointer
                members. Prevents unbounded recursion on self-referential
                types (e.g. a linked-list node pointing to itself). */
            }

            return;
        }

        /* Seed an uninitialized _Opt pointer as a correlated null/non-null pair sharing origin maps with its pointee's does-not-exist/valid alternatives, so narrowing (e.g. `if (p)`) keeps both sides in sync. */

        if (relation == FLOW_RELATION_ANY &&
                p_type != NULL &&
                type_is_pointer(p_type) &&
                (type_is_nullable(p_type, nullable_enabled) || force_opt))
        {
            /* Two child maps so alternatives from each arm have distinct origins. */
            struct flow_branch* _Opt p_null_map =
                flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_OPT_NULL);
            struct flow_branch* _Opt p_nonnull_map =
                flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_OPT_NONNULL);

            /* Without both arm maps there is no way to keep the null and
               non-null alternatives on distinct origins. */
            if (p_null_map == NULL || p_nonnull_map == NULL)
            {
                throw;
            }

            /* Allocate and initialise the arena object for the non-null arm. */
            struct object* _Opt p_pointed = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
            if (p_pointed != NULL)
            {
                struct type pointed_type = type_remove_pointer(p_type);
                make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
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

                /* Deliberately not recursing into p_pointed's own members here (tried once, reverted): once p is narrowed non-null, p_not_opt's members follow their own declared nullability as always (pattern2-member-through-narrowed-opt.c), and recursing raced with the eager non-opt-pointer branch on self-referential types, corrupting arena/origin state elsewhere (do-while-nested-if-unrelated-narrow.c). */
            }
        }
        else
        {
            /* A plain non-pointer value (or nullable checking disabled) must be seeded as a genuine-but-unknown value, never left UNINITIALIZED -- fixes false 'possibly uninitialized' on scalar parameters, call results, and struct members reached this way. */
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

    /* Copy a non-constant initializer expression's flow state into its leaf member (relation for integers; init/null/non-null for pointers, including owner members which are marked moved from their source too) -- otherwise a member set this way (e.g. `.p_token_begin = p->first_token`) stayed 'uninitialized' when the aggregate was used. */
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

    if (!type_is_function(&p_init_declarator->p_declarator->object.type))
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

            /* 2. Seed members set by non-constant initializers from those
                  expressions' flow3 values (constant members already seeded). */
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

static void flow_check_condition_known_at_compile_time(struct flow_ctx* ctx,
                                                       const struct expression* p_cond)
{
    if (ctx->iteration_pass != 0)
    {
        /* Inside a loop body the state is one iteration's, and the next
           iteration can contradict it: a counter the body increments, a
           pointer the body advances. Saying "always" there is exactly the
           claim this pass is not entitled to make. */
        return;
    }

    /* Same question compile_assert's check_object_true asks, with a different
       answer shape: that one reports every alternative it cannot prove true,
       one diagnostic each, and explains why; this one stays silent unless the
       whole set agrees. */
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

    /* For a comparison, the fact that settles it lives in the operands, not in
       the folded result: the result's own state was recorded at the condition
       itself, so reporting it would just point back at the line the reader is
       already looking at. Explain the operand instead. */
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

            /* A literal operand has no tracked state; the other side carries
               the explanation. */
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
        /* flow_object_truth found a settled value, so an alternative normally
           exists; if it does not, still report the conclusion. */
        diagnostic(W_FLOW_CONDITION_KNOWN_AT_COMPILE_TIME, ctx->ctx, NULL, &marker,
                   "condition is always %s", known ? "true" : "false");
        return;
    }

    struct osstream name_ss = { 0 };
    flow_expression_to_string(p_explained, &name_ss);

    struct osstream value_ss = { 0 };
    flow_alternative_sprint(&value_ss, p_alternative);

    /* Name the fact, but leave "where it came from" and "which branches were
       assumed" to the notes below -- same shape as every other diagnostic that
       reports a fact carried by one alternative. */
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

        struct flow_branch_pair cond_pair = { 0 };

        if (p_selection_statement->condition &&
                p_selection_statement->condition->expression)
        {
            cond_pair = flow_visit_full_expression(ctx, p_selection_statement->condition->expression);
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

        /* Non-narrowing conditions return the identity pair (or none at all);
           force two distinct arm maps so branch-body writes cannot leak
           unconditionally into the state after the if. */
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
                /* The tokens are only set by the branch above that actually built
                   hidden_expression; without them there is no condition to narrow. */
                p_if_cond = &hidden_expression;
            }
        }
        if (p_if_cond != NULL)
        {
            flow_check_condition_known_at_compile_time(ctx, p_if_cond);
        }

        cond_pair = flow_ensure_branch_pair(ctx, p_before, cond_pair, p_if_cond);

        /* If this exact predicate controlled an earlier branch (and its operands
           weren't written since), reuse that branch's id so values guarded by the
           predicate stay correlated across both branches. */
        if (cond_pair.p_true != NULL && cond_pair.p_false != NULL &&
                cond_pair.p_true != cond_pair.p_false &&
                cond_pair.p_true->branch_id != 0 &&
                p_if_cond != NULL)
        {
            const int fresh = cond_pair.p_true->branch_id;
            const int shared = flow_predicate_shared_id(ctx, p_if_cond, fresh);
            if (shared != fresh)
            {
                cond_pair.p_true->branch_id = shared;
                cond_pair.p_false->branch_id = shared;
            }
        }

        ctx->p_current_flow_branch = cond_pair.p_true;
        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);

        const bool true_reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

        if (p_selection_statement->else_secondary_block_opt)
        {
            ctx->p_current_flow_branch = cond_pair.p_false;
            flow_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);

            const bool false_reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->else_secondary_block_opt);

            /* A null branch map means the arena had no map to give; read it as an
               arm with no outcome to merge and leave p_before as-is. */
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
                /* else: both jump — p_before stays as-is */
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

    if (ctx->p_current_flow_branch == NULL)
        return;

    struct flow_branch* p_before = ctx->p_current_flow_branch;

    /* throw paths are accumulated into this map */
    struct flow_branch* _Opt p_throw_join = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_THROW_JOIN);
    if (p_throw_join == NULL)
        return; /* no map to work with */

    ctx->p_throw_join_map = p_throw_join;

    /* --- visit the try body --- */
    struct flow_branch* _Opt p_try_branch = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_TRY_BRANCH);
    if (p_try_branch == NULL)
        return; /* no map to work with */

    ctx->p_current_flow_branch = p_try_branch;
    flow_visit_secondary_block(ctx, p_try_statement->secondary_block);

    /*
       Restore the enclosing throw-join BEFORE visiting the catch body:
       a throw inside catch is a re-throw that must propagate to the
       *enclosing* try (or nowhere, if there isn't one), not recurse
       into this try's own throw join. That join only exists to collect
       throws out of the TRY body; it is fully populated by this point,
       and any statement in the catch body must resolve throw/goto/break
       targets against the context this whole try-statement was itself
       entered with.
    */
    ctx->p_throw_join_map = p_throw_join_map_old;

    const bool try_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->secondary_block);
    const bool catch_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt);

    if (p_try_statement->catch_secondary_block_opt)
    {
        /* --- visit the catch body, starting from the throw-join map --- */
        struct flow_branch* _Opt p_catch_branch = flow_branch_arena_new(&ctx->flow_branch_arena, p_throw_join, FLOW_BRANCH_CATCH_BRANCH);
        ctx->p_current_flow_branch = p_catch_branch;

        flow_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);

        /* flow_branch_arena_new is _Opt: a null p_catch_branch means the arena
           had no map to give, so there is no catch outcome to merge. Read it
           as an empty catch arm -- the arms below that would have used it are
           skipped, leaving the try arm to stand on its own, which is what the
           "catch jumps away" case already does. */
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
        /* No catch block: `throw` still exits the try body immediately (it
           just has nowhere of its own to land), so the state AT THE THROW
           POINT is exactly as live after the try statement as the state
           from reaching the end of the try body normally -- both are
           possible outcomes the code after the try must account for. Only
           merging p_try_branch here (as if throw simply didn't happen)
           silently dropped whatever the throw path had done up to that
           point, e.g. `int* _Owner p2 = p; if (c) throw; p = 0;` left p
           looking unconditionally null afterward, when the throw path
           actually leaves it MOVED (into p2), never reset. User-reported. */
        /* Pre-filled with p_before so no element is ever indeterminate; only
              the first num_arms entries are read. */
        const struct flow_branch* arms[2] = { p_before, p_before };
        int num_arms = 0;
        if (try_reached_the_end)
        {
            arms[num_arms++] = p_try_branch;
        }
        if (flow_branch_arm_has_entries(p_throw_join, p_before))
        {
            arms[num_arms++] = p_throw_join;
        }
        if (num_arms > 0)
        {
            flow_branch_merge_arms(p_before, arms, num_arms);
        }
    }

    ctx->p_current_flow_branch = p_before;
    /* ctx->p_throw_join_map was already restored before the catch body
       was visited, above. */
}

static void flow_visit_switch_statement(struct flow_ctx* ctx, struct selection_statement* p_selection_statement)
{
    /* Saved outside the try so the catch below restores them on every exit. */
    struct flow_branch* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow_branch* _Opt old_p_break_join_map = ctx->p_break_join_map;
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

        ctx->p_initial_map = p_before;
        ctx->p_break_join_map = p_break_join;

        /* record which object is being switched on */
        ctx->p_switch_obj_key = NULL;
        if (p_selection_statement->condition &&
                p_selection_statement->condition->expression)
        {
            ctx->p_switch_obj_key = &p_selection_statement->condition->expression->object;
        }

        ctx->p_current_flow_branch = p_body;
        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);

        bool reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

        {
            /* Pre-filled with p_before so no element is ever indeterminate;
               only the first num_arms entries are read. */
            const struct flow_branch* arms[2] = { p_before, p_before };
            int num_arms = 0;
            if (ctx->p_current_flow_branch == NULL)
            {
                throw;
            }

            if (reached_the_end)
            {
                arms[num_arms++] = ctx->p_current_flow_branch;
            }
            if (flow_branch_arm_has_entries(p_break_join, p_before))
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

/* Collapse a per-leaf diagnostic into one aggregate report: 'all' for UNINITIALIZED/MOVED (can be partial), 'any' for ENDED (a lifetime ends the whole object at once) -- avoids unsuppressable per-member noise (e.g. 20x/21x/222x duplicate warnings for one real issue). */
static bool flow_object_leaves_in_state_2(struct flow_ctx* ctx,
                                          const struct object* p_obj,
                                          enum flow_leaf_state state,
                                          const struct flow_branch* _Opt p_origin_filter,
                                          const struct flow_branch* _Opt p_origin_filter2,
                                          bool require_all,
                                          int* p_line,
                                          const struct flow_branch* _Opt* _Opt pp_origin);

static bool flow_object_leaves_in_state(struct flow_ctx* ctx,
                                        const struct object* p_obj,
                                        enum flow_leaf_state state,
                                        const struct flow_branch* _Opt p_origin_filter,
                                        bool require_all,
                                        int* p_line,
                                        const struct flow_branch* _Opt* _Opt pp_origin)
{
    return flow_object_leaves_in_state_2(ctx, p_obj, state, p_origin_filter, NULL, require_all, p_line, pp_origin);
}

static bool flow_object_leaves_in_state_2(struct flow_ctx* ctx,
                                          const struct object* p_obj,
                                          enum flow_leaf_state state,
                                          const struct flow_branch* _Opt p_origin_filter,
                                          const struct flow_branch* _Opt p_origin_filter2,
                                          bool require_all,
                                          int* p_line,
                                          const struct flow_branch* _Opt* _Opt pp_origin)
{
    if (p_obj->members.head)
    {
        for (const struct object* _Opt m = p_obj->members.head; m; m = m->next)
        {
            const bool hit = flow_object_leaves_in_state_2(ctx, m, state, p_origin_filter, p_origin_filter2, require_all, p_line, pp_origin);
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

        /* Same branch scoping the per-alternative loop applies. Without it a
           fact recorded on one arm leaks onto another: `p = tail(p)` inside a
           loop ends the pre-loop object, and the zero-iteration arm -- whose
           alternative still names that object, never having consumed it -- was
           reported as a use-after-end (samples/flow3/self-consuming-reassignment.c). */
        if (!flow_branch_is_ancestor_or_self(a->p_origin_map, p_origin_filter) &&
                !(p_origin_filter2 != NULL && flow_branch_is_ancestor_or_self(a->p_origin_map, p_origin_filter2)))
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

static struct
{
    const struct object* _Opt p_object; /* NULL marks the end of the used entries */
    int line;
    int diagnostic_id;
} s_reported_findings[256];

static void flow_reported_findings_clear(void)
{
    s_reported_findings[0].p_object = NULL;
}

static bool flow_finding_already_reported(const struct object* _Opt p_object, int line, int diagnostic_id)
{
    const int max = (int)(sizeof(s_reported_findings) / sizeof(s_reported_findings[0]));

    int i = 0;
    while (i < max && s_reported_findings[i].p_object != NULL)
    {
        if (s_reported_findings[i].p_object == p_object &&
                s_reported_findings[i].line == line &&
                s_reported_findings[i].diagnostic_id == diagnostic_id)
        {
            return true;
        }
        i++;
    }

    /* Stop one short of the end so slot max-1 stays NULL and keeps
       terminating the scan above. */
    if (i < max - 1)
    {
        s_reported_findings[i].p_object = p_object;
        s_reported_findings[i].line = line;
        s_reported_findings[i].diagnostic_id = diagnostic_id;
        s_reported_findings[i + 1].p_object = NULL;
    }

    return false;
}

static void flow_check_object_access(struct flow_ctx* ctx,
                                     const char* parent_expression_str,
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
    struct osstream bare_name_ss = { 0 };

    try
    {
        struct marker marker = expression_to_marker(p_expression);

        /* Bare display name for this object itself (used wherever the leaf
           diagnostics below print parent_expression_str with nothing appended):
           "*p" when this is the root pointee of an as-yet-undereferenced pointer
           expression, otherwise parent_expression_str unchanged (it is already
           a complete expression, e.g. "p->text" or "s.a"). */
        ss_fprintf(&bare_name_ss, base_is_ptr ? "*%s" : "%s", parent_expression_str);
        const char* bare_name = bare_name_ss.c_str ? bare_name_ss.c_str : parent_expression_str;

        if (p_object_src->members.head)
        {
            /* A union's members share storage: if ANY member is initialized (e.g.
               after `struct S s = { 0 };` sets the first union member) the whole
               union is initialized, so the sibling members must not be reported as
               "possibly uninitialized". Treating union members like independent
               struct members warned on every member other than the one written. */
            bool members_check_uninit = check_unitialized;
            if (members_check_uninit && flow_union_is_initialized(ctx, p_object_src))
                members_check_uninit = false;

                /* Whole aggregate uninitialized, or wholly moved: report it once, by
               the aggregate's own name, instead of once per leaf. See
               flow_object_leaves_in_state. */
            bool members_check_moved = check_moved;

            if (members_check_uninit)
            {
                int uninit_line = 0;
                const struct flow_branch* _Opt uninit_origin = NULL;
                if (flow_object_leaves_in_state(ctx, p_object_src, FLOW_LEAF_UNINITIALIZED, p_origin_filter, true, &uninit_line, &uninit_origin) &&
                        uninit_line != 0)
                {
                    /* At the aggregate level parent_expression_str is empty (the
                       per-leaf reports built their names by appending the member
                       designator to it), so recover the argument's own text. */
                    struct osstream ss = { 0 };
                    if (parent_expression_str != NULL && parent_expression_str[0] != '\0')
                        ss_fprintf(&ss, "%s", bare_name);
                    else
                        flow_expression_to_string(p_expression, &ss);

                    const bool reported_uninit = diagnostic(W_FLOW_UNINITIALIZED,
                                                            ctx->ctx, NULL, &marker,
                                                            "passing a possible uninitialized object '%s' (see line %d)",
                                                            ss.c_str ? ss.c_str : "", uninit_line);
                    ss_close(&ss);
                    if (reported_uninit)
                        flow_diagnose_map_path(ctx, uninit_origin);
                    members_check_uninit = false;
                }
            }

            /* Whole aggregate's lifetime ended: one report for the object rather
               than one per member. Deleting a struct ends every member at once, so
               the per-member form said the same thing 66 times for one
               `expression_delete(...)` (expressions.c:5667). */
            bool members_check_ended = check_ended;
            if (members_check_ended)
            {
                int ended_line = 0;
                const struct flow_branch* _Opt ended_origin = NULL;
                if (flow_object_leaves_in_state_2(ctx, p_object_src, FLOW_LEAF_ENDED, p_origin_filter, ctx->p_current_flow_branch, false, &ended_line, &ended_origin) &&
                        ended_line != 0)
                {
                    struct osstream ss = { 0 };
                    if (parent_expression_str != NULL && parent_expression_str[0] != '\0')
                        ss_fprintf(&ss, "%s", bare_name);
                    else
                        flow_expression_to_string(p_expression, &ss);

                    const bool reported_ended = diagnostic(W_FLOW_LIFETIME_ENDED,
                                                           ctx->ctx, NULL, &marker,
                                                           "object '%s' lifetime has ended (see line %d)",
                                                           ss.c_str ? ss.c_str : "", ended_line);
                    ss_close(&ss);
                    if (reported_ended)
                        flow_diagnose_map_path(ctx, ended_origin);
                    members_check_ended = false;
                }
            }

            if (members_check_moved)
            {
                int moved_line = 0;
                const struct flow_branch* _Opt moved_origin = NULL;
                if (flow_object_leaves_in_state(ctx, p_object_src, FLOW_LEAF_MOVED, p_origin_filter, true, &moved_line, &moved_origin) &&
                        moved_line != 0)
                {
                    struct osstream ss = { 0 };
                    if (parent_expression_str != NULL && parent_expression_str[0] != '\0')
                        ss_fprintf(&ss, "%s", bare_name);
                    else
                        flow_expression_to_string(p_expression, &ss);

                    const bool reported_moved = diagnostic(W_FLOW_MOVED,
                                                           ctx->ctx, NULL, &marker,
                                                           "object '%s' is moved (see line %d)",
                                                           ss.c_str ? ss.c_str : "", moved_line);
                    ss_close(&ss);
                    if (reported_moved)
                        flow_diagnose_map_path(ctx, moved_origin);
                    members_check_moved = false;
                }
            }

            /* When the destination is an array parameter, its elements are governed
               by the parameter's ELEMENT type (indexed) -- exactly as a struct's
               members are governed by name. Without this, an argument array of
               non-_Opt elements (e.g. `const char* argv[100] = {0}`) passed to a
               `const char* _Opt argv[]` parameter warned once per element even though
               the parameter accepts null elements (compile.c:1006). Derive the
               element governing type once and hand it to each element's check. */
            struct type dest_elem_type = { 0 };
            const struct type* _Opt p_member_governing = NULL;
            if (p_dest_governing_type != NULL && type_is_array(p_dest_governing_type))
            {
                dest_elem_type = get_array_item_type(p_dest_governing_type);
                p_member_governing = &dest_elem_type;
            }

            /* Once inside an array, stay inside it: the elements are usually
               structs, and it is their leaves that would report. */
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

                struct osstream ss = { 0 };
                const char* only_member_name = flow_get_only_member_name(member->member_designator);
                /* base_is_ptr: parent_expression_str is a bare, as-yet-undereferenced
                   pointer expression ("p") -- spell this member access as "p->text"
                   (source-code-accurate), not the old "(*p).text". Otherwise
                   parent_expression_str is already a complete expression (a
                   struct value, e.g. "s" or "p->in"), so a plain "." continues
                   it as before: "s.a", "p->in.a". Either way, the recursive call
                   below is never itself a fresh pointer dereference, so it always
                   passes base_is_ptr = false. */
                if (base_is_ptr)
                    ss_fprintf(&ss, "%s->%s", parent_expression_str,
                               only_member_name[0] == '.' ? only_member_name + 1 : only_member_name);
                else
                    ss_fprintf(&ss, "%s%s", parent_expression_str, only_member_name);
                if (ss.c_str == NULL)
                {
                    ss_close(&ss);
                    type_destroy(&dest_elem_type);
                    throw; /* out of memory */
                }

                flow_check_object_access(ctx, ss.c_str, p_expression, member, members_check_uninit, p_origin_filter, dest_is_dtor, p_member_governing, members_check_moved, members_in_array_element, members_check_ended, false);
                member = member->next;
                ss_close(&ss);
            }

            /* Unconditional: dest_elem_type is zero-initialized, so destroying it
               is a no-op when the array branch above never ran. */
            type_destroy(&dest_elem_type);
        }

        const struct flow_key_alternatives* _Opt p_source_key_alternatives = flow_branch_search_up(ctx->p_current_flow_branch, p_object_src);
        if (p_source_key_alternatives == NULL)
        {
            throw;
        }

        /* An object can carry more than one alternative that independently
           "can be zero" (e.g. an _Opt pointer's null arm plus a second,
           separately-derived possibly-zero fact from a merge). Without this
           guard the loop below reported the same "possible null pointer ...
           (see line N)" diagnostic once per such alternative instead of once
           per call-site -- seen dogfooding flow3 on cake's own expressions.c
           (unary_expression's `p_type` reported twice at several call sites,
           e.g. expressions.c:4149). Scoped per invocation (not static/global):
           each recursive call below (for a member or a REF-resolved object)
           gets its own object and its own fresh guard. */
        bool nullable_reported = false;

        for (int ri = 0; ri < p_source_key_alternatives->alternatives.size; ri++)
        {
            struct flow_alternative* p_alternative = p_source_key_alternatives->alternatives.data[ri];
            /* flow_alternative_print(p_alternative); */
            /* printf("\n"); */

            if (!flow_branch_is_ancestor_or_self(p_alternative->p_origin_map, p_origin_filter))
            {
                /* This alternative was established on a branch that isn't
                   an ancestor of the branch we're checking from -- it
                   belongs to a sibling arm and never actually coexists with
                   the specific pointer alternative that led here. Skip it;
                   see the function comment above. */
                continue;
            }

            if (p_alternative->imaginary == FLOW_IMAGINARY_ENDED && check_ended)
            {
                if (diagnostic(W_FLOW_LIFETIME_ENDED,
                               ctx->ctx, NULL, &marker,
                               "object '%s' lifetime has ended",
                               bare_name))
                {
                    flow_explain_alternative(ctx, p_alternative, p_alternative->p_origin_map, &marker);
                }

                continue; /* no need to print other errors */
            }

            if (p_alternative->value_kind == FLOW_VALUE_KIND_REF)
            {
                if (p_alternative->value.p != p_object_src)
                {
                    flow_check_object_access(ctx,
                                             parent_expression_str,
                                             p_expression,
                                             p_alternative->value.p, check_unitialized, p_origin_filter, dest_is_dtor,
                                             p_dest_governing_type, check_moved, in_array_element, check_ended, base_is_ptr);

                    continue;
                }
            }

            /* Skipped for a _Dtor destination: a destructor must accept a
               partially-created object, so a null member is allowed there. */
            /* The pointer's nullability is governed by the destination (parameter)
                        type when one was threaded in (array-element case above); otherwise by
                        the object's own declared type. */
            const struct type* p_null_type =
                p_dest_governing_type != NULL ? p_dest_governing_type : &p_object_src->type;
            if (!dest_is_dtor &&
                    type_is_pointer(&p_object_src->type) &&
                    !type_is_nullable(p_null_type, ctx->ctx->options.null_checks_enabled) &&
                    flow_alternative_can_be_zero(p_alternative) &&
                    !nullable_reported &&
                    !in_array_element &&
            /* Same fact, reached by another path, already reported for this
                       assignment -- see reported_findings. Evaluated last so it only
                       records findings that would actually have been printed. */
                    !flow_finding_already_reported(p_object_src, flow_alternative_line(p_alternative),
                                                   W_FLOW_NULLABLE_TO_NON_NULLABLE))
            {
                nullable_reported = true;
                /* Two different facts share this call, told apart by whether a destination type was threaded in: with one, %s is a value being read into something else that disallows null (its own type may be _Opt); without one, %s's own declared type is what's non-nullable (the recursive struct-member case). User-reported: the single wording used to get this backwards. */
                bool reported = 0;
                if (p_dest_governing_type != NULL)
                {
                    reported = diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                                          ctx->ctx, NULL, &marker,
                                          "'%s' may be null, but the destination does not allow null",
                                          bare_name);
                }
                else
                {
                    reported = diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                                          ctx->ctx, NULL, &marker,
                                          "'%s' may be null, but is declared non-nullable",
                                          bare_name);
                }

                /* Where the state came from, as its own note rather than a
                   "(see line N)" tail on the warning, plus how control reached
                   the reported line -- "may be null" is exactly the warning a
                   reader tends to believe is impossible. */
                if (reported)
                    flow_explain_alternative(ctx, p_alternative, ctx->p_current_flow_branch, &marker);
            }

            if (p_alternative->imaginary != FLOW_IMAGINARY_ENDED && p_alternative->value_relation == FLOW_RELATION_UNINITIALIZED)
            {
                if (check_unitialized)
                {

                    /* W_FLOW_UNINITIALIZED (30), not the nullability id: reading
                       something never written is a different analysis from passing
                       a null pointer, and 35 cannot distinguish them for `//lint`
                       or -W. flow1 already reports this shape as 30. */
                    if (diagnostic(W_FLOW_UNINITIALIZED,
                                   ctx->ctx, NULL, &marker,
                                   "passing a possible uninitialized object '%s'",
                                   bare_name))
                    {
                        flow_explain_alternative(ctx, p_alternative, p_alternative->p_origin_map, &marker);
                    }
                }
            }

            if (p_alternative->imaginary == FLOW_IMAGINARY_MOVED && check_moved)
            {
                /* W_FLOW_MOVED (32): use-after-move is its own condition, and 31
                   is already carrying "lifetime has ended". flow1 uses 32 here. */
                if (diagnostic(W_FLOW_MOVED,
                               ctx->ctx, NULL, &marker,
                               "object '%s' is moved",
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


static void flow_explain_alternative_not_true(struct osstream* ss, const struct flow_alternative* alt)
{
    if (alt->value_relation == FLOW_RELATION_UNINITIALIZED)
    {
        ss_fprintf(ss, "value is uninitialized");
    }
    else if (alt->value_relation == FLOW_RELATION_ANY)
    {
        ss_fprintf(ss, "value could be any value, including zero");
    }
    else if (alt->value_relation == FLOW_RELATION_EQUAL && flow_value_is_false(alt))
    {
        ss_fprintf(ss, "value equals zero");
    }
    else if (alt->value_relation == FLOW_RELATION_NOT_EQUAL)
    {
        ss_fprintf(ss, "value is only known to differ from a nonzero value, so it could still be zero");
    }
    else
    {
        ss_fprintf(ss, "value cannot be proven nonzero");
    }

    switch (alt->imaginary)
    {
        case FLOW_IMAGINARY_MOVED:
            ss_fprintf(ss, " (object was moved)");
        break;
        case FLOW_IMAGINARY_ENDED:
            ss_fprintf(ss, " (object's lifetime has ended)");
        break;

        default:
        break;
    }

    ss_fprintf(ss, ", set at line %d", flow_alternative_line(alt));

    if (alt->p_origin_map)
    {
        /* The full decision path, not just the name of the one map that
           recorded the fact: on a warning the reader believes is
           impossible, "which conditions were assumed along the way" is the
           question they actually need answered. See
           flow_explain_origin. */
        struct osstream path_ss = flow_explain_origin(alt->p_origin_map);
        ss_fprintf(ss, " in \"%s\"", path_ss.c_str ? path_ss.c_str : "");
        ss_close(&path_ss);
    }
}

/* type_is_pointed_* helpers assume a pointer type and silently answer false for an array parameter (kept as TYPE_CATEGORY_ARRAY, not adjusted to pointer) -- these wrappers check the array's element type directly instead, restoring the C parameter-adjustment equivalence between `char errmsg[]` and `char *errmsg`. */
static bool flow_dest_pointee_is_const(const struct type* p_type)
{
    if (type_is_array(p_type))
    {
        struct type item = get_array_item_type(p_type);
        bool r = type_is_const(&item);
        type_destroy(&item);
        return r;
    }
    return type_is_pointed_const(p_type);
}

static bool flow_dest_pointee_is_ctor(const struct type* p_type)
{
    if (type_is_array(p_type))
    {
        struct type item = get_array_item_type(p_type);
        bool r = type_is_out(&item);
        type_destroy(&item);
        return r;
    }
    return type_is_pointed_out(p_type);
}

static bool flow_dest_pointee_is_dtor(const struct type* p_type)
{
    if (type_is_array(p_type))
    {
        struct type item = get_array_item_type(p_type);
        bool r = type_is_dtor(&item);
        type_destroy(&item);
        return r;
    }
    return type_is_pointed_dtor(p_type);
}

static bool flow_dest_pointee_is_clear(const struct type* p_type)
{
    if (type_is_array(p_type))
    {
        struct type item = get_array_item_type(p_type);
        bool r = type_is_clear(&item);
        type_destroy(&item);
        return r;
    }
    return type_is_pointed_clear(p_type);
}

/* An _Owner member is effectively a plain VIEW when the containing object is
   _View: `_View struct X x` makes every _Owner member of x a view -- assigning
   into it does not move the source, and it is not "owner not moved" at exit.
   The _View qualifier sits on the aggregate while members keep their own _Owner
   type, so detect it by walking the object's parent chain. */
static bool flow_object_under_view(const struct object* obj)
{
    for (const struct object* _Opt p = obj->parent; p != NULL; p = p->parent)
    {
        if (type_is_view(&p->type))
            return true;
    }
    return false;
}

/* Force every leaf member's flow state to exist (via flow_seed_member_default) before a leak check walks them, so a member the source never happens to read isn't silently skipped -- fixes github.com/thradams/cake/issues/459; the older _Opt-only restriction is gone since owner-resource-059.c no longer needs it. */
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
   True if p_obj IS ctx's pending pre-reported object, or is a member of it
   (walking p_obj's own `parent` chain -- bounded, no cycles: struct members
   cannot contain themselves by value). Used only to consume
   ctx->p_pending_ended_report_obj; see the field comment in flow3.h.
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
   parameter `p_object_dest`: record what the call does to it, and check what
   the call reads from it.

   Array parameters are handled the same as pointer parameters via the
   flow_dest_pointee_is_* wrappers, which route array destinations through the
   element type instead of a pointee (type_is_pointed_* alone would silently
   report "false" for every check here, since an array destination's
   type_is_pointer() is false).
*/
static void flow_apply_pointee_param_effect(struct flow_ctx* ctx,
                                            struct expression* p_expression,
                                            const struct object* p_object_dest,
                                            const struct object* pointee,
                                            bool dtor_here,
                                            const struct flow_branch* _Opt p_origin_filter)
{
    enum flow_pointee_effect_kind effect_kind = FLOW_EFFECT_NONE;
    const struct token* p_token = p_expression->first_token;

    if (flow_dest_pointee_is_clear(&p_object_dest->type))
    {
        /* Only _Out _Clear together promises the callee handles a fully
           uninitialized pointee -- plain _Clear may free owned members
           itself before zeroing (issue #468, false positive on
           flow_alternatives_clear otherwise). */
        if (flow_dest_pointee_is_ctor(&p_object_dest->type))
        {
            struct marker clear_marker = expression_to_marker(p_expression);
            flow_check_discarding_owner_before_overwrite(ctx, p_expression, pointee, &clear_marker);
        }
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
                                          ctx->ctx->options.null_checks_enabled);
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
    const bool check_uninitialized = !flow_dest_pointee_is_ctor(&p_object_dest->type) && !source_uninit;
    /* For an array parameter, pass its type so the argument array's
    elements are checked against the parameter's element _Opt. */
    const struct type* _Opt gov =
        type_is_array(&p_object_dest->type) ? &p_object_dest->type : NULL;
    struct osstream arg_ss = { 0 };
    flow_expression_to_string(p_expression, &arg_ss);
    flow_check_object_access(ctx, arg_ss.c_str ? arg_ss.c_str : "", p_expression, pointee, check_uninitialized, p_origin_filter, dtor_here, gov, true, false, true, true);
    ss_close(&arg_ss);
}

static void flow_check_object_init_assigment(struct flow_ctx* ctx,
                                             struct expression* p_expression,
                                             const struct object* _Opt p_object_dest, /* uninitialized always */
                                             const struct object* _Opt p_object_src,
                                             enum init_type init_type,
                                             bool dest_is_dtor,
                                             bool dest_is_view)
{
    try
    {
        if (ctx->p_current_flow_branch == NULL)
            throw;

        if (p_object_src == NULL || p_object_dest == NULL)
            return;

        /* A _Dtor destination (destructor parameter) accepts a partially-created
        object: any of its members may legitimately be null, because a
        constructor that failed halfway must still be destroyable. Once we are
        inside a _Dtor pointee, that permission applies to every member below it,
        so the flag propagates down the recursion. See samples/flow3/dtor_is_opt.c. */
        const bool dtor_here = dest_is_dtor || flow_dest_pointee_is_dtor(&p_object_dest->type);

        /* A _View destination borrows and never takes ownership, so nothing moves
        into it. The qualifier sits on the aggregate (e.g. a `_View struct X`
        parameter) while its members keep their own _Owner types, so it has to
        travel down the recursion just like dest_is_dtor. Without it, passing an
        owner-bearing struct to a _View parameter moved the caller's owners and
        silently dropped the "owner not moved" leak (samples/flow3/ownership.c). */
        const bool view_here = dest_is_view || type_is_view(&p_object_dest->type) ||
            flow_object_under_view(p_object_dest);

        /* Skip only when forwarding an _Out pointee as another call's argument.
           Scoped to INIT_PARAMETER -- it used to fire on `*p = 1;` (INIT_OBJ) too,
           since p_object_dest there IS the _Out-qualified pointee (issue #471). */
        if (init_type == INIT_PARAMETER && type_is_out(&p_object_dest->type))
            return;

        struct marker marker = expression_to_marker(p_expression);

        /* A parameter declared as an array is adjusted to a pointer:
        `void f(int a[2])` IS `void f(int* a)`, so the callee gets a reference
        to the caller's array, not a copy of it. Copying element by element
        (the members path below) left every element holding its pre-call
        state, so `int a[2] = {}; f(a);` still read a[0] == 0 after a call
        that may have written it. Route it through the same pointee-effect
        rules a pointer parameter uses (issue #461). */

        if (init_type == INIT_PARAMETER &&
                type_is_array(&p_object_dest->type) &&
                type_is_array(&p_object_src->type))
        {
            /* p_object_src may be a reference to the caller's array (an
               identifier argument is a reference object), and the effect has
               to land on the array itself: flow_branch_set_object_any_n walks
               members and does not follow p_ref. */
            flow_apply_pointee_param_effect(ctx, p_expression, p_object_dest,
                                            object_get_referenced(p_object_src), dtor_here, NULL);
            return;
        }

        if (p_object_src->members.head && p_object_dest->members.head)
        {
            /* Walking the source's own members is only correct when the source IS
            the object that holds the state. A dereference (`*p`) is not: it is a
            placeholder whose members carry no state, while its real state lives
            in its alternatives (a REF to the pointer's tracked pointee). So when
            the source has alternatives, fall through to the per-alternative loop
            below -- that loop already recurses through each REF into the
            referenced object, which does have the real members and state.
            Short-circuiting here walked the placeholder's empty members instead,
            leaving `struct S temp = *p;` uninitialized and never moving *p's
            owners (dogfooded from object.c object_swap, see
            samples/flow3/swap-through-pointers.c). */
            const struct flow_key_alternatives* _Opt p_src_alts =
                flow_branch_search_up(ctx->p_current_flow_branch, p_object_src);

            /* Inspect EACH alternative: a source that stands for another object
            (a dereference's REF to the pointer's tracked pointee) must be read
            through that object, which holds the real member state. An aggregate
            that owns its state directly (e.g. the result of `x = f()`) has no
            such REF and keeps the member-by-member path below. */
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

            /* A struct-copy source with no tracked state at all (a dereference whose pointee was never fabricated) leaves the destination's members at whatever they had before, e.g. their declaration-time initializer -- since an assignment genuinely happened with an unknown value, set the destination's members to ANY instead. See struct-copy-from-deref-keeps-initializer.c. */
            if (init_type == INIT_OBJ &&
                    !type_is_array(&p_object_src->type) &&
                    !type_is_array(&p_object_dest->type))
            {
                /* Arrays are excluded: a string-literal source has no member
                   state either, but C zero-fills the elements its initializer
                   does not supply, and the caller below relies on that
                   (array-string-init-zero-fill.c). Only a struct/union copy
                   with nothing known about the source lands here. */
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
                                              ctx->ctx->options.null_checks_enabled);
                    return;
                }
            }

            {
                /* A union's members share storage: once any member is initialized the
                whole union is, so recurse only on the initialized member(s) and skip
                the uninitialized siblings. */
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

                /* C zero-fills the trailing elements of an array whose initializer is
                shorter than the array (`char new_file[512] = "";` zeroes [1..511]).
                The loop above copies only the elements the initializer supplies,
                leaving the rest looking uninitialized -- one false "possible
                uninitialized object" per element (codegen.c:5001). Zero the rest. */
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
        /* Array-to-pointer decay: `&arr[0]` is never null, so a pointer
        initialized/assigned from an array is non-null. The array source carries
        no pointer-value alternative to copy (an array identifier expression has
        no tracked pointer value), so without this the destination was left empty
        and read as possibly-null -- e.g. `const char* p = path;` (path is
        `char[400]`) then `*p`, a false "possible null pointer dereference" at
        compile.c:213. Seed the destination as a plain non-null pointer. */
        if (init_type == INIT_OBJ &&
                type_is_array(&p_object_src->type) &&
                type_is_pointer(&p_object_dest->type))
        {
            struct flow_key_alternatives* _Opt e =
                flow_branch_find_add(ctx->p_current_flow_branch, p_object_dest);
            if (e == NULL) throw;
            flow_alternatives_clear(&e->alternatives);
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
            return;
        }

        if (p_object_src->members.head && !p_object_dest->members.head)
        {
            /* Array-to-pointer decay: handled above. */
            /* fall through */
        }

        const struct flow_key_alternatives* _Opt p_src_key_alternatives =
            flow_branch_search_up(ctx->p_current_flow_branch, p_object_src);

        if (p_src_key_alternatives == NULL)
        {
            /* An unknown source ('we know nothing about this value') is not uninitialized -- returning early left a fresh destination still UNINITIALIZED, so record ANY on a real INIT_OBJ assignment instead. Fixes `const bool e = (a || b);` false 'possibly uninitialized'. */
            if (init_type == INIT_OBJ)
            {
                /* Only replace an UNINITIALIZED destination. Specialised paths
                   (array/string zero-fill, array-to-pointer decay) may already
                   have given it a precise state before we got here, and that is
                   strictly better than ANY -- clobbering it broke a
                   compile_assert in array-string-init-zero-fill.c. */
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
           Report each finding once per assignment, not once per source
           alternative -- the same rule flow_check_object_access already
           applies (see nullable_reported there). An object that accumulated
           many alternatives repeats one identical message: `return
           identity_pair;` at flow3.c:8757 emitted the same "possible null
           pointer 'identity_pair'" 174 times, and flow3.c:8754 twice more at
           87 each. The alternatives differ only in provenance, so the extra
           copies carry no information the first one lacks.
        */
        bool nullable_reported = false;
        bool uninitialized_reported = false;
        bool moved_reported = false;
        bool lifetime_ended_reported = false;

        /* Those four flags cover the object being assigned; the recursive
           member checks below run once per source alternative and need the
           same rule applied across calls. Reset here so the scope is exactly
           one assignment. */
        flow_reported_findings_clear();

        for (int ri = 0; ri < p_src_key_alternatives->alternatives.size; ri++)
        {
            struct flow_alternative* p_src_alternative = p_src_key_alternatives->alternatives.data[ri];

            if (p_src_alternative->imaginary == FLOW_IMAGINARY_ENDED &&
                    !flow_branch_is_ancestor_or_self(p_src_alternative->p_origin_map, ctx->p_current_flow_branch))
            {
                /* This ENDED fact's origin is a sibling branch that was never
                   open at the same time as the current path (e.g. a catch arm
                   that released the object and reset the pointer to null,
                   already excluded by a null check ahead of this call) --
                   same class of false positive as flow_check_object_access's
                   already-fixed lifetime check. See
                   deref-after-catch-reset-false-positive.c. */
                continue;
            }

            if (p_src_alternative->imaginary == FLOW_IMAGINARY_ENDED)
            {
                /* Already reported by EXPR_UNARY_CONTENT's own dereference
                   check for this exact object/line, when this same
                   expression was visited just before this call -- see the
                   field comment in flow3.h. Consume it once so it does not
                   suppress an unrelated later report. */
                if (flow_alternative_line(p_src_alternative) == ctx->pending_ended_report_line &&
                        flow_object_is_pending_ended_report(ctx, p_object_src))
                {
                    lifetime_ended_reported = true;
                    ctx->p_pending_ended_report_obj = NULL;
                    continue;
                }

                if (!lifetime_ended_reported)
                {
                    lifetime_ended_reported = true;
                    struct osstream ss = { 0 };
                    flow_expression_to_string(p_expression, &ss);
                    const bool reported_ended = diagnostic(W_FLOW_LIFETIME_ENDED,
                                                           ctx->ctx, NULL, &marker,
                                                           "object '%s' lifetime has ended",
                                                           ss.c_str);
                    ss_close(&ss);
                    if (reported_ended)
                        flow_explain_alternative(ctx, p_src_alternative, p_src_alternative->p_origin_map, &marker);
                }
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
                                                    p_src_alternative->p_origin_map);
                }

            }

            if (init_type == INIT_OBJ && p_dest_key_alternatives != NULL)
            {
                flow_alternatives_add(&p_dest_key_alternatives->alternatives, p_src_alternative);
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

                    /* Returning a pointer doesn't read what it points at, so the pointee's uninitialized state is not a finding on the return path (`return malloc(n);` after declaring malloc _Uninitialized is the ordinary allocator-wrapper idiom) -- INIT_PARAMETER stays checked, since handing that pointer to a callee that may read it is a real bug unless the callee declares _Out. */
                    const bool check_unitialized =
                        !flow_dest_pointee_is_ctor(&p_object_dest->type) &&
                        !source_uninit &&
                        init_type != INIT_RETURN;

                    flow_check_object_access(ctx, ss2.c_str, p_expression, p_src_alternative->value.p,
                                             check_unitialized, p_src_alternative->p_origin_map, dtor_here, NULL, true, false, true, true);
                }

                /* Moving an owner pointer into a void* _Owner destination erases the pointee's type, so its members can never be released individually afterward -- run the same recursive 'owner not moved' check scope-exit uses on the pointee first, for every init kind (parameter/assign/return), so a leaked member is still caught. See void-owner-erasure-leaks-member.c. */
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

                    /* Render the expression being erased (e.g. "p" in
                       `free(p)`) as the root name so the recursive check's
                       message reads "p->integer_type.name_opt" instead of
                       just ".integer_type.name_opt" -- see the
                       p_root_name_opt comment on flow_check_object_at_exit.
                       osstream must outlive the call, so it's closed after. */
                    struct osstream root_name_ss = { 0 };
                    flow_expression_to_string(p_expression, &root_name_ss);
                    flow_check_object_at_exit(ctx,
                                              &p_src_alternative->value.p->type,
                                              p_src_alternative->value.p,
                                              &marker,
                                              p_expression->first_token,
                                              view_here,
                                              root_name_ss.c_str);
                    ss_close(&root_name_ss);
                }

                if (init_type == INIT_PARAMETER)
                {
                    /* Determine write-effect on the pointee (same as above, but for a PTR alternative). */
                    const struct object* pointee = p_src_alternative->value.p;
                    const struct token* p_effect_token = p_expression->first_token;
                    enum flow_pointee_effect_kind effect_kind = FLOW_EFFECT_NONE;

                    if (flow_dest_pointee_is_clear(&p_object_dest->type))
                    {
                        /* Same _Out-only restriction as flow_apply_pointee_param_effect. */
                        if (flow_dest_pointee_is_ctor(&p_object_dest->type))
                        {
                            struct marker clear_marker2 = expression_to_marker(p_expression);
                            flow_check_discarding_owner_before_overwrite(ctx, p_expression, pointee, &clear_marker2);
                        }
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
                                                          ctx->ctx->options.null_checks_enabled);
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

            /* Null-pointer check for non-optional destination.
            Skipped for a _Dtor destination: a destructor must accept a
            partially-created object, so a null member is allowed there. */
            /* An array source decays to a pointer to its first element, which
               can never be null -- `static char buffer[] = ...; return buffer;`
               against a non-_Opt `const char*` return type. The array object
               holds no pointer value of its own, so its alternative reads as
               "could be zero" and used to be reported. (target.c:507) */
            const bool src_is_array = type_is_array(&p_object_src->type);

            if (!dtor_here &&
                    !src_is_array &&
                    type_is_pointer(&p_object_dest->type) &&
                    !type_is_nullable(&p_object_dest->type, ctx->ctx->options.null_checks_enabled) &&
                    flow_alternative_can_be_zero(p_src_alternative) &&
                    !nullable_reported)
            {
                nullable_reported = true;
                struct osstream ss = { 0 };
                flow_expression_to_string(p_expression, &ss);
                /* Always name the DESTINATION's type in this message, never the source expression's own type -- the source may legitimately be _Opt itself; it's the destination that disallows null. User-reported: parser.c:5214, `p_token = ctx->current;` where ctx->current is _Opt but p_token isn't. */
                if (diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                               ctx->ctx, NULL, &marker,
                               "'%s' may be null, but the destination does not allow null",
                               ss.c_str))
                {
                    flow_explain_alternative(ctx, p_src_alternative, ctx->p_current_flow_branch, &marker);
                }
                ss_close(&ss);
            }

            /* Uninitialized check */
            if (p_src_alternative->value_relation == FLOW_RELATION_UNINITIALIZED)
            {
                /* A union member shares storage with its siblings, so if any sibling
                is initialized the storage is initialized and this member is not
                genuinely uninitialized. A BRANCHED write (`if (c) v.i=..; else
                v.u=..`) writes a different member per branch, so after the merge
                each member carries a mixed init/uninit state -- the member-loop's
                union guard (which only skips siblings with NO init state at all)
                lets the uninit alternative through to here. Suppress it when the
                containing union has any initialized member. (object.c:501 /
                samples/flow3/union-branched-member-init.c) */
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
                        !source_uninit && !dest_accepts_uninit &&
                        !uninitialized_reported)
                {
                    uninitialized_reported = true;

                    const char* verb =
                        init_type == INIT_PARAMETER ? "passing" :
                        init_type == INIT_RETURN ? "returning" :
                        "assigning";

                    struct osstream name_ss = { 0 };
                    const char* _Opt object_name = p_object_src->member_designator;
                    if (object_name == NULL || object_name[0] == '\0')
                    {
                        flow_expression_to_string(p_expression, &name_ss);
                        object_name = name_ss.c_str ? name_ss.c_str : "";
                    }

                    const bool reported_uninit = diagnostic(W_FLOW_UNINITIALIZED,
                                                            ctx->ctx, NULL, &marker,
                                                            "%s a possible uninitialized object '%s'",
                                                            verb, object_name);
                    ss_close(&name_ss);
                    if (reported_uninit)
                        flow_explain_alternative(ctx, p_src_alternative, p_src_alternative->p_origin_map, &marker);
                }
            }

            /* Moved check (use-after-move) */
            if (p_src_alternative->imaginary == FLOW_IMAGINARY_MOVED &&
                    type_is_owner(&p_object_dest->type) &&
                    !moved_reported)
            {
                moved_reported = true;
                struct osstream ss = { 0 };
                flow_expression_to_string(p_expression, &ss);
                /* W_FLOW_MOVED (32) -- see the note at the other move site. */
                const bool reported_moved = diagnostic(W_FLOW_MOVED,
                                                       ctx->ctx, NULL, &marker,
                                                       "object '%s' is moved",
                                                       ss.c_str);
                ss_close(&ss);
                if (reported_moved)
                    flow_explain_alternative(ctx, p_src_alternative, p_src_alternative->p_origin_map, &marker);
            }
        }

        /* "A copy is a move": copying an _Owner value out of a source consumes it.
        This is the leaf rule -- an aggregate reaches here once per member via the
        member-by-member recursion above, so `struct S temp = *p;` moves each
        _Owner member of the pointed object one by one (and a plain
        `T* _Owner q = p;` moves the pointer itself). Without it, only the REF
        branch above moved anything, so a struct copied out of a dereference left
        the source's owners looking live -- the false "discards _Owner" /
        "uninitialized" pair on the swap idiom (dogfooded from object.c
        object_swap, see samples/flow3/swap-through-pointers.c). */
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
}

/* Flag an assignment that overwrites a live _Owner member, and also one that is merely ANY/unknown (e.g. fabricated through a plain pointer parameter) -- catching real leaks is worth the known false positive on unverifiable plain-pointer ownership (owner-moved-into-struct-member.c). */
/* Collect owner leaves an overwrite would discard first, report afterwards, so one assignment produces one diagnostic instead of once per leaf -- and name the single leaf individually when there's exactly one, otherwise collapse to the aggregate the assignment actually targets. */
struct flow_discarded_owner_scan
{
    int count;
    const char* _Opt name; /* designator of the first leaf found */
    int line;
};

static void flow_scan_discarded_owners(struct flow_ctx* ctx,
                                       const struct object* _Opt p_object_dest,
                                       struct flow_discarded_owner_scan* scan)
{
    if (p_object_dest == NULL)
    {
        /* A REF alternative's value.p can be NULL when the destination
           pointer (e.g. an _Opt parameter) may itself be null at this
           point -- there is no pointee object to check in that case.
           (The null-dereference itself is reported elsewhere.) */
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

    /* No longer skips synthetic parameter-seed placeholders here, for the same reason the ANY-relation skip was removed: an _Owner member reached through a plain pointer parameter is unknown-but-possibly-live, and silently overwriting it is the same class of leak as overwriting a confirmed-live one. See owner-moved-into-struct-member.c. */

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

        /* An ANY-valued _Owner member (unknown, possibly live) is now flagged on overwrite the same as a confirmed-live one -- previously suppressed to dodge a known false positive through plain pointer parameters, but that silently missed real leaks too; see owner-moved-into-struct-member.c. */

        if (flow_alternative_is_zero(p_alternative))
        {
            continue;
        }

        /* One report per LEAF, not per alternative. Alternatives differ only in
           provenance -- the same rule flow_check_object_access and
           flow_check_object_init_assigment already apply (see
           nullable_reported there). Without the break, an object that
           accumulated many alternatives repeated one identical message once
           for each. */
        if (scan->count == 0)
        {
            scan->name = p_object_dest->member_designator;
            scan->line = flow_alternative_line(p_alternative);
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

    /* One leaf: name it. Several: name the object being assigned, since they
       are all discarded by the same write.

       A top-level destination (a whole object, not a member) carries an EMPTY
       designator rather than a null one -- `*obj = *p;` at object.c:1632 is the
       shape -- so test for content, not for null, and fall back to the first
       leaf's name. Naming one of the discarded members still points at the
       right assignment; an empty string would say nothing at all. */
    const char* _Opt designator = scan.name;
    if (scan.count > 1 &&
            p_object_dest != NULL &&
            p_object_dest->member_designator != NULL &&
            p_object_dest->member_designator[0] != '\0')
    {
        designator = p_object_dest->member_designator;
    }

    /* designator (when present) is relative to the destination expression's
       own object, e.g. ".head" for `*list = new_list;` -- printing that alone
       loses which object it's a member of. Render the destination expression
       itself (e.g. "*list") and splice the two together: "*list" + ".head"
       reads as `list->head`, matching how it would actually be written in
       source, rather than the misleading standalone ".head". */
    struct osstream expr_ss = { 0 };
    flow_expression_to_string(p_expression_dest, &expr_ss);
    const char* base = expr_ss.c_str ? expr_ss.c_str : "";

    struct osstream name_ss = { 0 };
    if (designator != NULL && designator[0] != '\0')
    {
        const char* _Opt member = NULL;
        if (designator[0] == '.')
        {
            member = designator + 1;
        }
        else
        {
            member = designator;
        }
        if (base[0] == '*')
        {
            /* "*list" + ".head" -> "list->head" */
            ss_fprintf(&name_ss, "%s->%s", base + 1, member);
        }
        else
        {
            ss_fprintf(&name_ss, "%s.%s", base, member);
        }
    }
    else
    {
        ss_fprintf(&name_ss, "%s", base);
    }

    diagnostic(W_FLOW_DISCARDING_OWNER,
               ctx->ctx,
               NULL,
               marker,
               "assignment discards _Owner '%s' without releasing it first (see line %d)",
               name_ss.c_str ? name_ss.c_str : "",
               scan.line);

    ss_close(&name_ss);
    ss_close(&expr_ss);
}

/* Apply a _Clear/_Uninitialized allocation contract to the destination's own pointee type (calloc/malloc return typeless void*, which loses the contract at the type conversion) -- _Clear seeds every member 0, _Uninitialized seeds every member uninitialized; using such a member before assigning it now correctly warns. */
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
    if (!want_zero && !want_uninit)
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
            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
            if (want_zero)
                flow_branch_set_object_zero(ctx->p_current_flow_branch, p_pointed, p_token);
            else
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
                /* Defensive: see the matching guard in
                   flow_check_object_init_assigment -- a REF alternative
                   should always carry a real object* alias, but addresses
                   this low can't be valid heap/arena pointers. Found via
                   dogfooding flow3 on cake's own sources; skip rather than
                   crash on what looks like a corrupted/mistagged
                   alternative. */
                continue;
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
        }
    }

    /* Assigning to an owner-typed lvalue moves the source owner out of it.
       The loop above records that whenever the destination object is
       tracked, but a deep member destination whose pointee was not
       synthesized has no tracked object, so the loop does nothing and the
       source would be left as a false "owner not moved" at scope exit.
       Record the move here as well: mark each REF source alternative's
       target moved, the same rule flow_check_object_init_assigment applies.
       Idempotent with the loop (which already sets IMAGINARY_MOVED on the
       same objects). */
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

        /* Collect pointee write-effects instead of applying them per-argument:
           C evaluates ALL arguments first, then the callee runs. Applying them
           immediately would let an earlier argument (e.g. `&x`) invalidate a
           later one (e.g. `x.text`) before it is read. Nested calls in an
           argument save/restore this state so their effects don't leak out. */
        const bool saved_collect = ctx->collect_deferred_effects;
        const int saved_count = ctx->deferred_effects_count;
        ctx->collect_deferred_effects = true;
        ctx->deferred_effects_count = saved_count;

        while (p_current_argument && p_current_parameter_type)
        {
            struct expression* p_arg_expr = p_current_argument->expression;
            const struct type* p_param_type = &p_current_parameter_type->type;

            struct object param_object = { 0 };
            make_object(p_param_type, &param_object, MAKE_STATE_UNITIALIZED, ctx->ctx->options.target);

            flow_visit_full_expression(ctx, p_arg_expr);
            flow_check_object_init_assigment(ctx, p_arg_expr, &param_object, &p_arg_expr->object, INIT_PARAMETER, false, false);

            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
            object_destroy(&param_object);
        }

        /* All arguments evaluated: now apply the deferred write-effects (the
           callee "runs"). Restore the collection state for the caller. */
        ctx->collect_deferred_effects = saved_collect;
        for (int i = saved_count; i < ctx->deferred_effects_count; i++)
        {
            const struct flow_deferred_pointee_effect* e = &ctx->deferred_effects[i];

            /* Copied into a local: the member state of an array element read
               through a variable index is not tracked, so testing e->pointee
               in place would not narrow it. */
            const struct object* _Opt pointee = e->pointee;
            if (pointee == NULL)
            {
                continue; /* no object to apply the effect to */
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
                    flow_branch_set_object_any_n(ctx->p_current_flow_branch, pointee, e->p_token, ctx->ctx->options.null_checks_enabled);
                break;
                case FLOW_EFFECT_DTOR:
                    flow_branch_apply_dtor_or_clear_effect(ctx->p_current_flow_branch, pointee, false, e->p_token);
                break;
                default:
                break;
            }
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
    /* expr is non-null on entry and _Assert keeps it non-null across the
       step, so no null test is needed (and testing it here would leave the
       result looking nullable to the caller). */
    while (expr->expression_type == EXPR_PRIMARY_PARENTHESIS)
    {
        _Assert(expr->right != NULL);
        expr = expr->right;
    }
    return expr;
}

/* A write through an index the analysis cannot pin to one element -- `v[i] = x`
   with `i` unknown -- writes SOME element of v, and nothing says which. Every
   element's tracked value is a guess afterwards, so invalidate the whole array.
   Without this the seeds left by `char buffer[16] = {0};` survived the write
   and a later `buffer[1] == 'O'` folded to always-false (user-reported:
   expressions.c, parse of an octal constant's `o`/`O` prefix). */
static void flow_invalidate_unknown_index_write(struct flow_ctx* ctx,
                                                const struct expression* _Opt p_dest)
{
    if (p_dest == NULL)
    {
        return;
    }

    const struct expression* p_subscript = skip_parenthesis(p_dest);

    if (p_subscript->expression_type != EXPR_POSTFIX_ARRAY ||
            p_subscript->left == NULL ||
            p_subscript->right == NULL)
    {
        return;
    }

    /* Pinned to one element: the subscript visit resolves it to that element's
       object and the normal assignment path updates it -- no need to widen.

       Inside a loop body only a genuine constant expression counts as pinned.
       A flow-derived value does not: the body is analysed with one iteration's
       state, so `buffer[c] = *s;` with `c` at its first-iteration value looks
       like a write to buffer[0] while it really writes a different element on
       every trip. Trusting it left every other element sitting at its
       initializer -- the `char buffer[16] = {0}` case that made a later
       `buffer[1] == 'O'` fold to always-false. */
    const bool index_is_pinned =
        object_has_constant_value(&p_subscript->right->object) ||
        (ctx->iteration_pass == 0 && object_has_known_value(&p_subscript->right->object));

    if (index_is_pinned)
    {
        return;
    }

    /* The subscript visit removes its own operands' temporary entries from the
       map, so the array has to be reached through the object model rather than
       through a REF alternative left behind by the visit. */
    const struct object* p_array =
        object_get_referenced(&skip_parenthesis(p_subscript->left)->object);

    flow_branch_set_object_any_n(ctx->p_current_flow_branch,
                              p_array,
                              p_subscript->first_token,
                              ctx->ctx->options.null_checks_enabled);
}

static void flow_expression_static_debug(struct flow_ctx* ctx, const struct expression* p_expression)
{
    struct token* first_token = p_expression->first_token;
    struct token* last_token = p_expression->last_token;
    p_expression = skip_parenthesis(p_expression);
    object_static_debug(ctx, &p_expression->object, first_token, last_token);
}

static struct flow_branch_pair flow_visit_full_expression(struct flow_ctx* ctx, const struct expression* p_expression)
{
    return flow_visit_expression(ctx, p_expression);
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
                tagged.p_origin_token = p_token;
                flow_alternatives_add(true_alts, &tagged);
            }
            else
            {
                struct flow_alternative tagged = *alt;
                tagged.p_origin_map = origin;
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
                    tagged.p_origin_token = p_token;
                    flow_alternatives_add(false_alts, &tagged);
                }
                else
                {
                    /* != c is true, keep NOT_EQUAL c in true branch only */
                    struct flow_alternative tagged = *alt;
                    tagged.p_origin_map = origin;
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
                        .p_origin_token = p_token
                    };
                    flow_alternatives_add(true_alts, &a_eq);
                    /* For false branch, keep the original NOT_EQUAL val (we lose the info x != c) */
                    struct flow_alternative tagged = *alt;
                    tagged.p_origin_map = origin;
                    tagged.p_origin_token = p_token;
                    flow_alternatives_add(false_alts, &tagged);
                }
                else
                {
                    /* For !=, the condition x != c is not determined by x != val when val != c,
                       so we keep NOT_EQUAL val in both branches */
                    {
                        struct flow_alternative tagged = *alt;
                        tagged.p_origin_map = origin;
                        tagged.p_origin_token = p_token;
                        flow_alternatives_add(true_alts, &tagged);
                    }
                    {
                        struct flow_alternative tagged = *alt;
                        tagged.p_origin_map = origin;
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
                    .p_origin_token = p_token
                };
                struct flow_alternative a_ne =
                {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW_RELATION_NOT_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = origin,
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
                    .p_origin_token = p_token
                };
                struct flow_alternative a_eq =
                {
                    .value_kind = v.value_kind,
                    .value = v.value,
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = alt->imaginary,
                    .p_origin_map = origin,
                    .p_origin_token = p_token
                };
                flow_alternatives_add(true_alts, &a_ne);
                flow_alternatives_add(false_alts, &a_eq);
            }
        }
        else
        {
            /* Relational (half-line) constraint (>, >=, <, <=) meets an
               ==/!= test. The equality branch pins the variable to the
               constant (strictly more precise); the opposite branch keeps
               the range unchanged. This mirrors the ANY handling so that
               e.g. `x > 0 && x == 5` still narrows x to exactly 5 instead
               of losing the `== 5` to the pre-existing `> 0`. */
            struct flow_alternative a_eq =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED,
                .value = {.i = c},
                .value_relation = FLOW_RELATION_EQUAL,
                .imaginary = alt->imaginary,
                .p_origin_map = origin,
                .p_origin_token = p_token
            };
            struct flow_alternative a_range = *alt;
            a_range.p_origin_map = origin;
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

/* Map a relational operator (variable on the LEFT: `var OP c`) to the
   flow_relation recorded for the variable on the requested branch. */
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
            return op;
    }
}

/* Narrow `src` (a variable's alternatives) for the condition `var OP c`,
   producing refined alternatives for the true and false branches.
   - concrete EQUAL values are routed to the branch they satisfy;
   - an unknown scalar (ANY) becomes a half-line constraint on each branch;
   - everything else is carried unchanged into both branches (conservative). */
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
                .p_origin_token = p_token
            };
            struct flow_alternative a_false =
            {
                .value_kind = FLOW_VALUE_KIND_SIGNED,
                .value = {.i = c},
                .value_relation = flow_relation_for_op(op, false),
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = origin,
                .p_origin_token = p_token
            };
            flow_alternatives_add(true_alts, &a_true);
            flow_alternatives_add(false_alts, &a_false);
            continue;
        }

        /* Intersect an existing half-line constraint with a later, different relational condition's range instead of duplicating it unchanged into both branches -- fixes cake's own two-sequential-ifs pattern producing false 'division by zero' (narrow-half-line-against-relational.c); non-numeric cases fall through unchanged. */
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
                tagged.p_origin_token = p_token;
                /* Clip to the intersection -- overlapping is not enough. An
                   alternative already known `<= 0` narrowed by `< 0` must
                   become `< 0` on the true branch; keeping it at `<= 0` left
                   zero live and reported a false division by zero. */
                flow_alt_set_interval(&tagged,
                                      alt_lo > t_lo ? alt_lo : t_lo,
                                      alt_hi < t_hi ? alt_hi : t_hi);
                flow_alternatives_add(true_alts, &tagged);
            }
            if (have_f && alt_lo <= f_hi && f_lo <= alt_hi)
            {
                struct flow_alternative tagged = *alt;
                tagged.p_origin_map = origin;
                tagged.p_origin_token = p_token;
                flow_alt_set_interval(&tagged,
                                      alt_lo > f_lo ? alt_lo : f_lo,
                                      alt_hi < f_hi ? alt_hi : f_hi);
                flow_alternatives_add(false_alts, &tagged);
            }
            continue;
        }

        /* NOT_EQUAL, pointers, or MOVED/ENDED state: keep unchanged in both
           branches (conservative fallback). */
        struct flow_alternative tagged = *alt;
        tagged.p_origin_map = origin;
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

static void flow_narrow_operand_relational(struct flow_ctx* ctx,
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
        narrow_by_relational(&obj_entry->alternatives, c, op,
                             &true_alts, &false_alts, p_true, p_token);
        /* Tag each branch's alternatives with ITS OWN map so join
           correlation can tell them apart (true values belong to p_true,
           false values to p_false -- not both to p_true). */
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
            /* the narrowing helper may have reserved capacity without adding
               any alternative; that buffer is ours to release. */
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
            /* the narrowing helper may have reserved capacity without adding
               any alternative; that buffer is ours to release. */
            flow_alternatives_clear(&false_alts);
        }
    }
}

/* Decide `alt <op> c` for ONE alternative: 1 (holds), 0 (does not hold),
   -1 (undecidable). Split out of the loop below so a single alternative can
   be evaluated directly, without wrapping it in a one-element list. */
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
                /* A NOT_EQUAL pointer alternative is non-null (whether it
                    records `!= null` directly or a concrete non-null target,
                    e.g. the result of array-decay pointer arithmetic) --
                    consistent with flow_alternative_can_be_zero. */
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

/* Is `anc` on the parent chain of `m` (i.e. an ancestor-or-equal map)? */
static bool flow_branch_is_ancestor_or_equal(const struct flow_branch* anc, const struct flow_branch* m)
{
    for (const struct flow_branch* _Opt p = m; p != NULL; p = p->p_parent_map)
        if (p == anc)
        return true;
    return false;
}

static bool flow_origins_compatible(const struct flow_branch* _Opt o1, const struct flow_branch* _Opt o2)
{
    if (o1 == NULL || o2 == NULL || o1 == o2)
        return true;
    for (const struct flow_branch* _Opt a = o1; a != NULL; a = a->p_parent_map)
    {
        if (a->branch_id == 0)
            continue;
        for (const struct flow_branch* _Opt b = o2; b != NULL; b = b->p_parent_map)
        {
            if (b->branch_id == a->branch_id && b->kind != a->kind)
                return false;
        }
    }
    return true;
}

static const struct flow_branch* _Opt flow_origin_more_specific(const struct flow_branch* _Opt o1,
                                                             const struct flow_branch* _Opt o2)
{
    if (o1 == NULL) return o2;
    if (o2 == NULL) return o1;
    if (o1 == o2) return o1;
    if (flow_branch_is_ancestor_or_equal(o1, o2)) return o2; /* o1 is ancestor -> o2 deeper */
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
            return false;
    }
}

static int flow_pair_equality(const struct flow_alternative* lval,
                              const struct flow_alternative* rval,
                              bool is_equal)
{
    long long lc = 0, rc = 0;
    bool lk = flow_alt_concrete_int(lval, &lc);
    bool rk = flow_alt_concrete_int(rval, &rc);

    /* Two non-null POINTER values that name DIFFERENT objects cannot be
       concluded unequal: distinct pointer variables may alias at runtime (e.g.
       two _Opt parameters that happen to point at the same object -- their
       non-null arms are separate synthesized pointees, but nothing proves they
       differ). Reporting them as definitely-not-equal wrongly killed branches
       like `if (list->head == list->tail)` after a head!=NULL guard. Only NULL
       vs non-null, and same-object, remain decidable here. REF values (the
       address of a real named object) keep their exact comparison below. */
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

static int flow_evaluate_equality_multi(struct flow_ctx* ctx,
                                        const struct expression* p_left,
                                        const struct expression* p_right,
                                        bool is_equal)
{
    const struct flow_key_alternatives* _Opt left_entry =
        flow_branch_search_up(ctx->p_current_flow_branch, &p_left->object);
    const struct flow_key_alternatives* _Opt right_entry =
        flow_branch_search_up(ctx->p_current_flow_branch, &p_right->object);
    if (left_entry == NULL || right_entry == NULL)
        return -1;

    /* An empty operand (no alternatives) means unknown, not known-zero -- it used to double as calloc's zeroed-member stand-in, which folded `x->i == 0` to always-true and misreported live/unreachable code. calloc now carries _Clear and seeds real zeros, so the vacuous empty-means-zero rule is gone. */
    if (left_entry->alternatives.size == 0 || right_entry->alternatives.size == 0)
        return -1;

    bool all_true = true, all_false = true, any_pair = false, any_skipped = false;

    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow_alternative* lalt = left_entry->alternatives.data[li];
        bool l_is_ref = (lalt->value_kind == FLOW_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow_key_alternatives* _Opt left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow_branch_search_up(ctx->p_current_flow_branch, lalt->value.p);
            if (left_resolved == NULL) return -1;
        }
        int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow_alternative* lval = l_is_ref ? left_resolved->alternatives.data[lvi] : lalt;

            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow_alternative* ralt = right_entry->alternatives.data[ri];
                bool r_is_ref = (ralt->value_kind == FLOW_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow_key_alternatives* _Opt right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow_branch_search_up(ctx->p_current_flow_branch, ralt->value.p);
                    if (right_resolved == NULL) return -1;
                }
                int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow_alternative* rval = r_is_ref ? right_resolved->alternatives.data[rvi] : ralt;
                    if (!flow_origins_compatible(lval->p_origin_map, rval->p_origin_map))
                    {
                        any_skipped = true; /* correlated join: cross-branch pair */
                        continue;
                    }

                    int c = flow_pair_equality(lval, rval, is_equal);
                    if (c == -1) return -1;
                    any_pair = true;
                    if (c)
                    {
                        all_false = false;
                    }
                    else
                    {
                        all_true = false;
                    }
                }
            }
        }
    }

    /* If no pair was decided only because every combination was cross-branch
       (operands non-empty), the result is unknown. A genuinely empty operand
       (no alternatives -- a zero-initialized default) holds vacuously. */
    if (!any_pair && any_skipped)
        return -1;
    if (all_true) return 1;
    if (all_false) return 0;
    return -1;
}

/* Narrow the variable operand `p_expr` for `var == c` / `var != c`, writing
   refined alternatives into p_true/p_false. Per-alternative REF pattern: for
   every REF alternative of the operand, narrow the object it references. */
static void flow_narrow_operand_equality(struct flow_ctx* ctx,
                                         const struct expression* p_expr,
                                         long long c,
                                         bool is_equal,
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

        const struct object* _Opt obj = alt->value.p;
        const struct flow_key_alternatives* _Opt obj_entry =
            flow_branch_search_up(ctx->p_current_flow_branch, obj);
        if (obj_entry == NULL)
            continue;

        struct flow_alternatives true_alts = { 0 }, false_alts = { 0 };
        narrow_by_constant(&obj_entry->alternatives, c, is_equal,
                           &true_alts, &false_alts, p_true, p_token);
        /* Tag each branch's alternatives with its own map (see the relational
           narrow) so join correlation can distinguish them. */
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
            /* the narrowing helper may have reserved capacity without adding
               any alternative; that buffer is ours to release. */
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
            /* the narrowing helper may have reserved capacity without adding
               any alternative; that buffer is ours to release. */
            flow_alternatives_clear(&false_alts);
        }
    }
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

static int flow_evaluate_relational_multi(struct flow_ctx* ctx,
                                          const struct expression* p_left,
                                          const struct expression* p_right,
                                          enum expression_type op)
{
    const struct flow_key_alternatives* _Opt left_entry =
        flow_branch_search_up(ctx->p_current_flow_branch, &p_left->object);

    const struct flow_key_alternatives* _Opt right_entry =
        flow_branch_search_up(ctx->p_current_flow_branch, &p_right->object);

    if (left_entry == NULL || right_entry == NULL)
        return -1;

    bool all_true = true, all_false = true, any = false;

    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow_alternative* lalt = left_entry->alternatives.data[li];
        bool l_is_ref = (lalt->value_kind == FLOW_VALUE_KIND_REF && lalt->value.p != NULL);
        const struct flow_key_alternatives* _Opt left_resolved = left_entry;
        if (l_is_ref)
        {
            left_resolved = flow_branch_search_up(ctx->p_current_flow_branch, lalt->value.p);
            if (left_resolved == NULL) return -1;
        }
        int l_count = l_is_ref ? left_resolved->alternatives.size : 1;

        for (int lvi = 0; lvi < l_count; lvi++)
        {
            const struct flow_alternative* lval = l_is_ref ? left_resolved->alternatives.data[lvi] : lalt;
            long long llo = 0, lhi = 0;
            if (!flow_alt_to_interval(lval, &llo, &lhi))
                return -1;

            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow_alternative* ralt = right_entry->alternatives.data[ri];
                bool r_is_ref = (ralt->value_kind == FLOW_VALUE_KIND_REF && ralt->value.p != NULL);
                const struct flow_key_alternatives* _Opt right_resolved = right_entry;
                if (r_is_ref)
                {
                    right_resolved = flow_branch_search_up(ctx->p_current_flow_branch, ralt->value.p);
                    if (right_resolved == NULL) return -1;
                }
                int r_count = r_is_ref ? right_resolved->alternatives.size : 1;

                for (int rvi = 0; rvi < r_count; rvi++)
                {
                    const struct flow_alternative* rval = r_is_ref ? right_resolved->alternatives.data[rvi] : ralt;
                    if (!flow_origins_compatible(lval->p_origin_map, rval->p_origin_map))
                        continue; /* correlated join: skip cross-branch pairs */
                    long long rlo = 0, rhi = 0;
                    if (!flow_alt_to_interval(rval, &rlo, &rhi))
                        return -1;

                    int c = flow_interval_relational(llo, lhi, rlo, rhi, op);
                    if (c == -1) return -1;
                    any = true;
                    if (c)
                    {
                        all_false = false;
                    }
                    else
                    {
                        all_true = false;
                    }
                }
            }
        }
    }

    if (!any) return -1;
    if (all_true) return 1;
    if (all_false) return 0;
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


static bool flow_comparison_result_alts(struct flow_ctx* ctx,
                                        const struct expression* p_left,
                                        const struct expression* p_right,
                                        enum expression_type op,
                                        struct flow_alternatives* out,
                                        const struct token* _Opt p_token)
{
    if (ctx->p_current_flow_branch == NULL)
        return false;

    const struct flow_key_alternatives* _Opt left_entry =
        flow_branch_search_up(ctx->p_current_flow_branch, &p_left->object);

    const struct flow_key_alternatives* _Opt right_entry =
        flow_branch_search_up(ctx->p_current_flow_branch, &p_right->object);

    if (left_entry == NULL || right_entry == NULL)
        return false;

    bool any = false;

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
                    if (!flow_origins_compatible(lval->p_origin_map, rval->p_origin_map))
                        continue;

                    int b = flow_pair_boolean(lval, rval, op);
                    if (b == -1)
                    {
                        flow_alternatives_clear(out);
                        return false;
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

    if (!any)
    {
        flow_alternatives_clear(out);
        return false;
    }
    return true;
}

static void flow_seed_comparison_result(struct flow_ctx* ctx,
                                        const struct expression* p_expression)
{
    _Assert(p_expression->left != NULL);
    _Assert(p_expression->right != NULL);

    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
    if (e == NULL) return;

    flow_alternatives_clear(&e->alternatives);

    struct flow_alternatives per_path = { 0 };
    if (flow_comparison_result_alts(ctx, p_expression->left, p_expression->right,
                                    p_expression->expression_type, &per_path,
                                    p_expression->first_token))
    {
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

    /* Emit division/modulo-by-zero at most once per expression, even
       though the divisor may be checked against several left-hand
       alternatives below. */
    bool zero_divisor_warned = false;

    bool left_uninit_warned = false;
    bool right_uninit_warned = false;

    /* ---- Outer loop over left top‑level alternatives ---- */
    for (int li = 0; li < left_entry->alternatives.size; li++)
    {
        const struct flow_alternative* lalt = left_entry->alternatives.data[li];


        /* Resolve left REF if present */
        const struct flow_key_alternatives* _Opt left_resolved = left_entry;
        if (lalt->value_kind == FLOW_VALUE_KIND_REF && lalt->value.p != NULL)
        {
            left_resolved = flow_branch_search_up(ctx->p_current_flow_branch, lalt->value.p);
            if (!left_resolved) continue;
        }

        /* ---- Loop over resolved left values ---- */
        for (int lvi = 0; lvi < left_resolved->alternatives.size; lvi++)
        {
            const struct flow_alternative* lval = left_resolved->alternatives.data[lvi];

            if (!left_uninit_warned &&
                !ctx->expression_is_not_evaluated &&
                lval->imaginary != FLOW_IMAGINARY_ENDED &&
                lval->value_relation == FLOW_RELATION_UNINITIALIZED &&
                !type_is_uninit(&p_left->object.type))
            {
                struct osstream left_name_ss = { 0 };
                flow_expression_to_string(p_left, &left_name_ss);
                const bool reported_left_uninit = diagnostic(W_FLOW_UNINITIALIZED,
                                                             ctx->ctx, NULL, &marker,
                                                             "using a possible uninitialized object '%s'",
                                                             left_name_ss.c_str ? left_name_ss.c_str : "");
                if (reported_left_uninit)
                    flow_explain_alternative(ctx, lval, lval->p_origin_map, &marker);
                ss_close(&left_name_ss);
                left_uninit_warned = true;
            }


            /* ---- Middle loop over right top‑level alternatives ---- */
            for (int ri = 0; ri < right_entry->alternatives.size; ri++)
            {
                const struct flow_alternative* ralt = right_entry->alternatives.data[ri];


                /* Resolve right REF if present */
                const struct flow_key_alternatives* _Opt right_resolved = right_entry;
                if (ralt->value_kind == FLOW_VALUE_KIND_REF && ralt->value.p != NULL)
                {
                    right_resolved = flow_branch_search_up(ctx->p_current_flow_branch, ralt->value.p);
                    if (!right_resolved) continue;
                }

                /* ---- Loop over resolved right values ---- */
                for (int rvi = 0; rvi < right_resolved->alternatives.size; rvi++)
                {
                    const struct flow_alternative* rval = right_resolved->alternatives.data[rvi];

                    if (!right_uninit_warned &&
                        !ctx->expression_is_not_evaluated &&
                        rval->imaginary != FLOW_IMAGINARY_ENDED &&
                        rval->value_relation == FLOW_RELATION_UNINITIALIZED &&
                        !type_is_uninit(&p_right->object.type))
                    {
                        struct osstream right_name_ss = { 0 };
                        flow_expression_to_string(p_right, &right_name_ss);
                        const bool reported_right_uninit = diagnostic(W_FLOW_UNINITIALIZED,
                                                                      ctx->ctx, NULL, &marker,
                                                                      "using a possible uninitialized object '%s'",
                                                                      right_name_ss.c_str ? right_name_ss.c_str : "");
                        if (reported_right_uninit)
                            flow_explain_alternative(ctx, rval, rval->p_origin_map, &marker);
                        ss_close(&right_name_ss);
                        right_uninit_warned = true;
                    }


                    enum flow_relation result_rel = FLOW_RELATION_ANY;
                    long long result_val = ANY_VALUE;

                    /*
                       Divisor may be zero even when its exact value isn't
                       known (ANY, or NOT_EQUAL to some nonzero constant
                       still doesn't rule out 0). Warn as soon as we see a
                       divisor alternative that could be zero, not only
                       when both operands are known constants.
                    */
                    if ((op == '/' || op == '%') &&
                            !zero_divisor_warned &&
                            !ctx->expression_is_not_evaluated &&
                            rval->value_relation != FLOW_RELATION_UNINITIALIZED &&
                            flow_alternative_can_be_zero(rval))
                    {
                        /* Unevaluated contexts (sizeof/_Alignof, static asserts)
                           never perform the division at runtime. */
                        diagnostic(W_FLOW_DIVISION_BY_ZERO, ctx->ctx, NULL, &marker,
                                   op == '/' ? "division by zero" : "modulo by zero");
                        zero_divisor_warned = true;
                    }

                    /* Correlate the join for the computed VALUE: only combine
                       operand values that can coexist on the same path
                       (compatible origins). The divisor-zero check above is a
                       property of the divisor alone and runs for every rval,
                       independent of this pairing. */
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
                                        /* already warned above by the
                                   possibly-zero-divisor check */
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
                                        /* already warned above by the
                                   possibly-zero-divisor check */
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
                                        /* shift count out of range: result is
                                   undefined -- leave ANY. */
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

                    /* Pointer +/- integer: keep the pointer operand's own alternative instead of falling through to a degraded ANY, which made flow_alternative_can_be_zero treat an offset non-null pointer as possibly zero -- same class of bug already fixed for ++/--/+=/-=. */
                    {
                        /* A "pointer base" is either a real pointer value (PTR)
                           or a non-null address (REF) -- the latter is how an
                           array-decay operand like `int arr[10]` shows up
                           (`arr + 3`). Offsetting either by an integer can never
                           produce a null pointer, so keep the result non-null
                           instead of degrading to a plain SIGNED ANY. Without
                           the REF case, `arr + 3` was wrongly treated as
                           possibly null. */
                        /* Whether the other operand is an OFFSET is a property
                              of its type, not of whatever value the flow analysis
                              currently holds for it. Passing the containing struct
                              to a function invalidates the tracked value of an
                              integer member (`st->size` after `reserve(st, n)`),
                              and that must not cost the pointer operand its
                              non-null guarantee -- see
                              tests/unit-tests/flow3/narrow-through-pointer-arithmetic.c */
                        const bool lnum2 = type_is_integer(&p_left->object.type);
                        const bool rnum2 = type_is_integer(&p_right->object.type);
                        /* A pointer whose containing object was invalidated by
                           a call is re-seeded with a plain SIGNED ANY, so a
                           later `!= 0` narrowing yields {SIGNED, NOT_EQUAL}
                           rather than a pointer-kind alternative. Recognise
                           that as a base too -- via the operand's static type,
                           which no invalidation can change -- so the guard's
                           non-nullness survives the arithmetic. */
                        const bool lbase = (lval->value_kind == FLOW_VALUE_KIND_PTR) ||
                            (lval->value_kind == FLOW_VALUE_KIND_REF && lval->value.p != NULL) ||
                            (type_is_pointer(&p_left->object.type) &&
                             lval->value_relation == FLOW_RELATION_NOT_EQUAL);
                        const bool rbase = (rval->value_kind == FLOW_VALUE_KIND_PTR) ||
                            (rval->value_kind == FLOW_VALUE_KIND_REF && rval->value.p != NULL) ||
                            (type_is_pointer(&p_right->object.type) &&
                             rval->value_relation == FLOW_RELATION_NOT_EQUAL);

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
                                /* Array-decay / address base: the offset result
                                   is a non-null pointer of unknown exact target. */
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
                    if (result_rel == FLOW_RELATION_EQUAL)
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

        const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
        if (type_is_pointer(&member_obj->type))
        {
            /* Seed an unseeded pointer member from its declared nullability:
                 non-_Opt member -> non-null (!= null) by contract;
                 _Opt member     -> possibly-null (ANY).
               Leaving an _Opt member EMPTY was wrong: a later `m != NULL` then
               found no null alternative and treated the pointer as definitely
               non-null, killing the else branch (e.g. `list->head = old->next;`
               where next is _Opt, then `if (list->head != NULL) ... else ...`). */
            const bool is_nullable = type_is_nullable(&member_obj->type, nullable_enabled);
            struct flow_key_alternatives* _Opt me = flow_branch_find_add(ctx->p_current_flow_branch, member_obj);
            if (me == NULL)
                return; /* no entry to seed */

            flow_alternatives_clear(&me->alternatives);

            if (is_nullable)
            {
                /* Model an _Opt member the same way an _Opt PARAMETER is modeled:
                   two correlated arms, and give the non-null arm a CONCRETE pointee.

                   A single `{PTR, ANY, p == NULL}` says "may be null" but points at
                   nothing, so `a->b->c` had to invent a pointee at the access and
                   the narrowing from `if (a->b)` was fragile -- it survived a
                   straight-line guard but not a guard inside a loop, whose body is
                   re-analysed and re-merged (codegen.c:533). Giving the non-null arm
                   a real object up front makes the deeper member a stable object
                   that narrowing can attach to. */
                struct flow_branch* _Opt p_null_map =
                    flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_OPT_MEM_NULL);
                struct flow_branch* _Opt p_nonnull_map =
                    flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_current_flow_branch, FLOW_BRANCH_OPT_MEM_NONNULL);

                /* Without both arm maps the two alternatives would share an
                   origin, which is worse than leaving the member unseeded. */
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
                            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
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
        /* A pointer known only as "non-null, unknown target" (NOT_EQUAL,
           the ordinary state of a non-_Opt pointer) still has that fact
           preserved across a cast -- casting doesn't change whether a
           pointer is null. Handling only the EQUAL case here (e.g. a
           literal address, or the null constant) dropped this common
           case to the "can't handle" fallback below, which reseeds the
           result as fully unknown and false-warns it as possibly null
           at its next use (see samples/flow3/cast-preserves-not-equal-pointer.c). */
        struct flow_alternative tagged = *alt;

        /* A void*-returning allocator's fabricated pointee stays void-typed even after a cast to a concrete pointer type, so later `->member` access silently failed to resolve and looked like it carried no fact at all -- re-fabricate the pointee typed to the CAST's target pointee type instead of carrying the stale void one forward. */
        if (tagged.value.p != NULL && type_is_void(&tagged.value.p->type))
        {
            struct type target_pointee = type_remove_pointer(p_target_type);
            if (!type_is_void(&target_pointee))
            {
                struct object* _Opt p_new = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
                if (p_new != NULL)
                {
                    make_object(&target_pointee, p_new, MAKE_STATE_ANY, ctx->ctx->options.target);
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

static struct flow_branch_pair flow_visit_expression(struct flow_ctx* ctx, const struct expression* _Opt p_expression)
{
    /* left/right are _Owner _Opt in the AST, and callers hand them straight
       in; an absent operand is nothing to visit. */
    if (p_expression == NULL || ctx->p_current_flow_branch == NULL)
    {
        struct flow_branch_pair empty = { 0 };
        return empty;
    }

    struct flow_branch* p_before = ctx->p_current_flow_branch;
    (void)p_before; /* used in some cases below */

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
                {
                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                    if (e == NULL) throw;
                    flow_alternatives_clear(&e->alternatives);
                    struct flow_alternative a =
                    {
                        .value_kind = FLOW_VALUE_KIND_SIGNED,
                        .value = {.i = object_to_signed_long_long(&p_expression->object)},
                        .value_relation = FLOW_RELATION_EQUAL,
                        .imaginary = FLOW_IMAGINARY_NONE,
                        .p_origin_map = ctx->p_current_flow_branch,
                        .p_origin_token = p_expression->first_token
                    };
                    flow_alternatives_add(&e->alternatives, &a);
                }
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

                        /* A pointer global respects its declared nullability, just like
                a parameter or member: a non-_Opt global pointer is non-null
                (e.g. `stdout`), an _Opt one is possibly-null. Without this a
                plain `FILE* stdout` read as ANY and passing it to a
                non-nullable parameter falsely warned. */
                        if (type_is_pointer(&p_expression->object.type))
                        {
                            a.value_kind = FLOW_VALUE_KIND_PTR;
                            a.value.p = NULL;
                            a.value_relation = type_is_nullable(&p_expression->object.type, ctx->ctx->options.null_checks_enabled)
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
                    /* Compile-time constant (e.g. constexpr) whose value was not
                        carried over from its own declaration analysis (each top-level
                        declaration gets a fresh flow map). Seed it with its real,
                        unchanging value instead of leaving it untracked. */
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
                struct flow_branch* _Opt p_true = flow_narrow_map_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, p_obj2, true, p_expression, p_expression->first_token);
                struct flow_branch* _Opt p_false = flow_narrow_map_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, p_obj2, false, p_expression, p_expression->first_token);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow_tag_branch_pair(p_true, p_false);
                return (struct flow_branch_pair) { p_true, p_false };
            }

            case EXPR_PRIMARY_PARENTHESIS:
            {
                _Assert(p_expression->right != NULL);
                const struct expression* p_inner = skip_parenthesis(p_expression->right);
                struct flow_branch_pair paren_pair = flow_visit_expression(ctx, p_inner);

                /* Copy the inner expression's computed value forward to its own node too: narrowing already flows correctly, but a synthesized temporary (e.g. a parenthesized nested ternary) is looked up by its own distinct &object, and `(b?1:2)` vs `b?1:2` are different nodes -- without the copy a caller keying off this node's address finds nothing. */
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
                /* Seed a string literal as a definite-nonzero SIGNED placeholder, not FLOW_VALUE_KIND_PTR (which crashes flow_check_object_access on its synthetic char members) -- object_to_signed_long_long has no real integer for an array object, so it used to silently look identical to NULL. */
                struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                if (e == NULL) throw;
                flow_alternatives_clear(&e->alternatives);
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = 1},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_expression->first_token
                };
                flow_alternatives_add(&e->alternatives, &a);
            }
            break;

            case EXPR_PRIMARY_CHAR_LITERAL:
            case EXPR_PRIMARY_NUMBER:
            case EXPR_PRIMARY_PREDEFINED_CONSTANT:
            {
                struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                if (e == NULL) throw;
                flow_alternatives_clear(&e->alternatives);
                struct flow_alternative a =
                {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = object_to_signed_long_long(&p_expression->object)},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_expression->first_token
                };
                flow_alternatives_add(&e->alternatives, &a);
            }
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

                        /* Seed an unseeded member's default flow state on `.` access too, exactly like EXPR_POSTFIX_ARROW already does for `->` -- otherwise a member reached via `.` never got its null/non-null pair, so re-evaluating the same `a.b` a second time (e.g. an initializer RHS) lost the first evaluation's narrowing. */
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
                            flow_narrow_map_into(p_true, ctx->p_current_flow_branch, p_member, true, p_expression->first_token);
                            flow_narrow_map_into(p_false, ctx->p_current_flow_branch, p_member, false, p_expression->first_token);
                        }
                    }
                }

                /* Narrow on the member field used as bool. */
                return (struct flow_branch_pair)
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

                /* A merge (e.g. a while-loop's "ran and exited null" vs "never
        entered, started null" paths) can leave the base pointer with
        more than one alternative that is independently null. Without
        this guard, the loop below over alternatives reported the same
        "operator -> applied to a possible null pointer" diagnostic once
        per null alternative instead of once per access
        (null-narrow-while-traverse-post-loop.c warned twice on one
        line). */
                bool null_deref_reported = false;

                /* Same one-report-per-access rule for the pointee's lifetime:
        a merged pointer can carry many alternatives aiming at the
        same pointee, and without this the identical "operator ->:
        pointed object lifetime has ended" was emitted once per
        (pointer alternative x pointee alternative) pair -- 30 times
        on a single line of cake's own parser.c. */
                bool lifetime_ended_reported = false;

                /* Fabricate an unresolvable pointer's pointee on demand, at the access (not eagerly per call), via make_object rather than flow_object_init (which would wrongly assert every _Opt member as possibly-null) so a guard like `if (pX->p)` can actually narrow it (safety-049). */
                if (p_left_alternatives != NULL && type_is_pointer(&p_expression->left->object.type))
                {
                    /* The PTR alternatives usually live on the VARIABLE object, reached
            through a REF from this expression's temporary. Fabricate into
            that entry -- keying off the temporary would mint a fresh pointee
            on every read, so a guard narrowed on one read would not be
            visible on the next (safety-049). */
                    const struct object* p_key = &p_expression->left->object;
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
            A pointee can also be present but USELESS: `obj = calloc(...)`
            is an implicit void*->T* conversion with no EXPR_CAST node, so
            the fabricated pointee behind obj stays typed `void`.
            object_get_member(void_obj, ...) then always returns NULL, so
            every `obj->m` falls back to seeding an untyped ANY -- member
            writes and narrowings on it go nowhere, and the member reads as
            possibly-null forever. Re-fabricate with the base pointer's real
            pointed type in that case, exactly as for a missing pointee.
            See samples/flow3/branch-merge-loses-ref-nonnull.c.
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
                        struct type pointed_type = type_remove_pointer(&p_expression->left->object.type);
                        if (!type_is_void(&pointed_type))
                        {
                            struct flow_alternatives rebuilt = { 0 };
                            /* Fabricate one pointee for the whole rebuild pass, written straight into p_key's own map entry and found again via ordinary flow_branch_search_up -- not one per matching alternative, and not via a side-cache, both of which used to fragment or stale a single logical pointee (owner-if-else-052.c). */
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
                                            make_object(&pointed_type, p_new_shared, MAKE_STATE_ANY, ctx->ctx->options.target);
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
                                type_destroy(&pointed_type);
                                throw;
                            }
                        }
                        type_destroy(&pointed_type);
                    }
                }

                if (p_left_alternatives != NULL)
                {
                    for (int i = 0; i < p_left_alternatives->alternatives.size; i++)
                    {
                        const struct flow_alternative* ptr_alt = p_left_alternatives->alternatives.data[i];



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



                                /* Null check (skipped in unevaluated contexts like
                        sizeof/_Alignof: the -> is never applied at runtime). */
                                if (p_pointer_alt->value_relation == FLOW_RELATION_EQUAL &&
                                p_pointer_alt->value_kind == FLOW_VALUE_KIND_PTR &&
                                p_pointer_alt->value.p == NULL)
                                {
                                    if (!ctx->expression_is_not_evaluated && !null_deref_reported)
                                    {
                                        null_deref_reported = true;
                                        struct osstream ss = { 0 };
                                        flow_expression_to_string(p_expression->left, &ss);
                                        const bool reported_null = diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                                                          "'%s->' possible null dereference",
                                                                          ss.c_str ? ss.c_str : "");
                                        ss_close(&ss);
                                        if (reported_null)
                                            flow_explain_alternative(ctx, p_pointer_alt, p_pointer_alt->p_origin_map, &marker);
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

                                /* Walk only member_obj's own leaves (not the whole object) for an ended-lifetime check, filtered by ctx->p_current_flow_branch rather than the pointer's own origin -- an aliased pointer can reach a leaf whose ENDED fact has no ancestor relation to the pointer's origin but is still visible here (aliased-owner-lifetime-not-tracked). */
                                int ended_line = 0;
                                const struct flow_branch* _Opt ended_origin = NULL;
                                if (!lifetime_ended_reported &&
                                flow_object_leaves_in_state_2(ctx, member_obj, FLOW_LEAF_ENDED,
                                                              p_pointer_alt->p_origin_map, ctx->p_current_flow_branch, false, &ended_line, &ended_origin))
                                {
                                    lifetime_ended_reported = true;
                                    /* Include the expression itself, matching the null-deref
                            sites' "operator -> applied to a possible null pointer
                            '%s'" -- without it there was no way to tell which
                            access, in a function with more than one `->`, the
                            diagnostic was even about. User-reported. */
                                    struct osstream ss = { 0 };
                                    flow_expression_to_string(p_expression, &ss);
                                    const bool reported_ended = diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker,
                                                                       "'%s': pointed object lifetime has ended (see line %d)",
                                                                       ss.c_str ? ss.c_str : "", ended_line);
                                    ss_close(&ss);
                                    if (reported_ended)
                                        flow_diagnose_map_path(ctx, ended_origin);
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

                                flow_narrow_map_into(p_true, ctx->p_current_flow_branch, member_obj, true, p_expression->first_token);
                                flow_narrow_map_into(p_false, ctx->p_current_flow_branch, member_obj, false, p_expression->first_token);

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
                                if (!ctx->expression_is_not_evaluated && !null_deref_reported)
                                {
                                    null_deref_reported = true;
                                    struct osstream ss = { 0 };
                                    flow_expression_to_string(p_expression->left, &ss);
                                    const bool reported_null = diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                                                      "possible null pointer '%s' dereference",
                                                                      ss.c_str ? ss.c_str : "");
                                    ss_close(&ss);
                                    if (reported_null)
                                        flow_explain_alternative(ctx, ptr_alt, ptr_alt->p_origin_map, &marker);
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

                            /* "Any leaf ended" walk, scoped to the ACCESSED MEMBER, checking
                    against BOTH ptr_alt's own origin and ctx->p_current_flow_branch
                    -- see the identical fix and rationale (aliased-owner-
                    lifetime-not-tracked) at the REF-alternative site above. */
                            int ended_line = 0;
                            const struct flow_branch* _Opt ended_origin = NULL;
                            if (!lifetime_ended_reported &&
                            flow_object_leaves_in_state_2(ctx, member_obj, FLOW_LEAF_ENDED,
                                                          ptr_alt->p_origin_map, ctx->p_current_flow_branch, false, &ended_line, &ended_origin))
                            {
                                lifetime_ended_reported = true;
                                /* Include the expression itself -- see the identical fix and
                        rationale at the REF-alternative site above. */
                                struct osstream ss = { 0 };
                                flow_expression_to_string(p_expression, &ss);
                                const bool reported_ended = diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker,
                                                                   "'%s': pointed object lifetime has ended (see line %d)",
                                                                   ss.c_str ? ss.c_str : "", ended_line);
                                ss_close(&ss);
                                if (reported_ended)
                                    flow_diagnose_map_path(ctx, ended_origin);
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

                            flow_narrow_map_into(p_true, ctx->p_current_flow_branch, member_obj, true, p_expression->first_token);
                            flow_narrow_map_into(p_false, ctx->p_current_flow_branch, member_obj, false, p_expression->first_token);

                            any_member_resolved = true;
                        }
                    }
                }

                if (!any_member_resolved)
                {
                    /*
            The member has no concrete object behind it (base pointer has no
            modeled pointee). Seed the result from the member's DECLARED type
            rather than leaving it EMPTY -- empty now means "unknown", and an
            unknown operand must not be mistaken for a proof.

            non-_Opt pointer -> non-null (its type guarantees it)
            _Opt pointer     -> possibly null
            integer          -> ANY
            */
                    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
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

                    p_true = flow_narrow_map_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, &p_expression->object, true, p_expression, p_expression->first_token);
                    p_false = flow_narrow_map_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, &p_expression->object, false, p_expression, p_expression->first_token);
                    if (p_true == NULL || p_false == NULL)
                        throw;

                }

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                return (struct flow_branch_pair)
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

                /* Bounds check for a FLOW-DERIVED index (a narrowed range or a
        branch-constant). A literal/constant-folded index is already checked
        by the parser (object_has_known_value is true for those), so we skip
        it here to avoid double-warning. We warn only when the index is
        PROVABLY out of bounds on some path -- i.e. its whole interval lies
        past the end (lo >= N) or below zero (hi < 0) -- so an ordinary
        unknown index (ANY, no interval) is never flagged. */
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

                            /* The index expression usually resolves to a REF to the
                    variable object; follow it to the value alternatives. */
                            const struct flow_key_alternatives* _Opt value_alts = NULL;
                            if (idx->value_kind == FLOW_VALUE_KIND_REF && idx->value.p != NULL)
                                value_alts = flow_branch_search_up(ctx->p_current_flow_branch, idx->value.p);

                            const struct flow_alternative* vlist_one = idx;
                            int vcount = 1;
                            /* data is now an array of OWNED POINTERS, not an inline
                    array of structs -- this is itself already the
                    element pointer type, so vlist_many[j] below needs
                    no '&'. */
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

                /* For a CONSTANT index, resolve v[i] to the array's element object and
        seed this expression as a REF to it -- so element values/relations
        (including initializers) are tracked, mirroring EXPR_POSTFIX_DOT.
        A non-constant index can't be pinned to one element, so we fall back
        to narrowing on this expression's own object. */
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

                            flow_narrow_map_into(p_true, ctx->p_current_flow_branch, p_element, true, p_expression->first_token);
                            flow_narrow_map_into(p_false, ctx->p_current_flow_branch, p_element, false, p_expression->first_token);
                            any_resolved = true;
                        }
                    }

                    flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                    flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);

                    if (any_resolved)
                        return (struct flow_branch_pair)
                        {
                    p_true, p_false
                        };

                    /* Nothing resolved (unknown array, out-of-range, etc.): fall through
            to plain narrowing on this expression's object. */
                }

                /* Seed the (unresolved) subscript result as an ANY value of its
        element type, so it is never an EMPTY operand. An empty operand
        makes flow_evaluate_equality_multi fold `v[i] == c` to "always
        true" (the vacuous-empty rule), which marked the else branch of
        e.g. `if (s->current[0] == '\n')` as unreachable code. `*p`
        (EXPR_UNARY *) already seeds ANY; subscript did not. */
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
                 ctx->ctx->options.null_checks_enabled &&
                 !type_is_nullable(&p_expression->object.type, ctx->ctx->options.null_checks_enabled))
                {
                    /* An unresolved element of a non-_Opt pointer array is non-null by
            the non-_Opt => non-null rule -- e.g. `argv[i]` for
            `char** argv` (argv[0..argc-1] are non-null per the C standard).
            Seed NOT_EQUAL null, not ANY: seeding ANY here would (re)introduce
            ~28 false "possible null" -- the reason pointer elements were left
            unseeded originally. Non-null is the correct, narrower state and
            keeps `argv[i]` (and `argv[i] + n`) off the possibly-null path. */
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
                struct flow_branch* _Opt p_true = flow_narrow_map_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, p_obj, true, p_expression, p_expression->first_token);
                struct flow_branch* _Opt p_false = flow_narrow_map_branch(&ctx->flow_branch_arena, ctx->p_current_flow_branch, p_obj, false, p_expression, p_expression->first_token);
                if (p_true == NULL || p_false == NULL)
                    throw;

                flow_tag_branch_pair(p_true, p_false);
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                return (struct flow_branch_pair)
                {
                p_true, p_false
                };
            }

            case EXPR_POSTFIX_FUNCTION_CALL:
            {
                _Assert(p_expression->left != NULL);

                flow_visit_expression(ctx, p_expression->left);
                flow_visit_function_arguments(ctx, &p_expression->left->object.type, &p_expression->argument_expression_list);

                const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
                const struct type* p_ret_type = &p_expression->object.type;
                const struct token* p_call_token = p_expression->first_token;
                /* `_Clear` in RETURN position means the returned pointee is all-zero
                    (calloc) -- the return-side reading of the same qualifier that, on a
                    parameter, means "the callee zeroes the pointee". */
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
                        make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);

                        struct flow_branch* old = ctx->p_current_flow_branch;
                        ctx->p_current_flow_branch = p_nonnull_map;
                        flow_object_init(ctx, p_pointed, p_call_token);
                        ctx->p_current_flow_branch = old;
                        /* Return-type contract on the pointee: `_Clear` (e.g. calloc)
                means the returned region is all-zero -- seed each member
                EQUAL 0 so `p->m == 0` is concretely true; `_Uninitialized`
                (e.g. malloc) means the contents are indeterminate. The
                qualifier may sit on the returned pointer (`T* _Opt _Clear f()`,
                next to _Owner/_Opt) or on the pointee (`_Clear T* f()`).
                Seed into the live (parent) map -- not p_nonnull_map, whose
                member state is a sibling branch that execution never enters
                after the call; p_pointed is only ever reached through the
                surviving non-null alternative, so the parent map is where a
                later `x->m` read resolves it. */
                        if (ret_zero)
                            flow_branch_set_object_zero(ctx->p_current_flow_branch, p_pointed, p_call_token);
                        else if (ret_uninit)
                            flow_branch_set_object_uninitialized(ctx->p_current_flow_branch, p_pointed, p_call_token);
                        type_destroy(&pointed_type);
                    }

                    {
                        /* p_pointed == NULL (allocation failure): fall back to a plain
                "non-null" alternative with no concrete pointee. */
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
                    /* Non-_Opt pointer return: non-null. For a `_Clear`/`_Uninitialized`
            pointee contract, build a concrete pointee so members can be
            seeded zero/uninitialized (mirrors the _Opt branch above). */
                    struct object* _Opt p_pointed = NULL;
                    if (ret_zero || ret_uninit)
                    {
                        p_pointed = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
                        if (p_pointed != NULL)
                        {
                            struct type pointed_type = type_remove_pointer(p_ret_type);
                            make_object(&pointed_type, p_pointed, MAKE_STATE_ANY, ctx->ctx->options.target);
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
                    /* A call's return value is always fully initialized by the time it comes back, even for a non-pointer type or a pointer with nullable checks disabled -- reuse flow_parameter_object_init to seed it ANY/non-null recursively, fixing false 'possibly uninitialized' on scalar returns and their members (errcode, x.a). */
                    /* p_expression is const here (flow_visit_expression's own
                        parameter), so &p_expression->object is a const struct
                        object* -- but flow_parameter_object_init's signature
                        (shared with the parameter-seeding call site) takes a
                        non-const struct object*, matching every other call site
                        where the object being seeded belongs to a non-const
                        declarator. It only ever reads this object's own
                        .members list (to recurse) and writes into the flow map
                        keyed by its address; it never mutates the object itself.
                        Cast away const explicitly rather than relaxing the
                        shared signature for every other caller. */
                    flow_parameter_object_init(ctx, (struct object*)&p_expression->object, p_ret_type, p_call_token);
                }
                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->left->object);
            }
            break;

            case EXPR_POSTFIX_FUNCTION_LITERAL:
            {
                _Assert(p_expression->compound_statement != NULL);

                /* A function literal's body is never reached through
                   flow_visit_declaration (it has no enclosing struct declaration --
                   its compound_statement hangs off this expression instead), so
                   none of the per-function setup/teardown that macro normally
                   provides happens for it automatically. Without this, a literal's
                   _Owner parameters are never seeded by flow_parameter_object_init,
                   so passing/leaking a resource through them goes uncaught -- same
                   root cause as the defer-not-generated bug in defer.c fixed for
                   issue #269, just in the ownership checker instead of codegen. */
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

                /* Seed a compound literal's own object via flow_object_init + flow_seed_aggregate_from_init_exprs, same as a declarator, instead of blindly marking it ANY -- fixes `x = (struct X){0};` losing per-member zero/constant info that the plain declaration form kept. User-reported. */
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
                /*
                * _Assert(expr) is equivalent to:
                *   if (!expr) exit();   // exit does not return
                *
                * So after assert, only the TRUE branch of expr is reachable.
                * We apply the true-branch refinements to the current map and
                * discard the false branch (it is a dead end, like exit()).
                */
                if (p_expression->right)
                {
                    struct flow_branch_pair assert_pair = flow_visit_expression(ctx, p_expression->right);

                    /* The false branch is dead (assert would have aborted).
            Merge only the true outcome back into p_before. */
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
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = result},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                }
                else
                {
                    /* Operand has no constant value, but it may still carry a RELATION
                        (e.g. `b < 0` narrowed by an enclosing if). Carry that through:
                        unary + preserves it, unary - mirrors it. Only if nothing can be
                        mapped do we fall back to a plain ANY. */
                    const bool is_neg = (p_expression->expression_type == EXPR_UNARY_NEG);
                    const struct flow_key_alternatives* _Opt p_src =
                    flow_branch_search_up(ctx->p_current_flow_branch, &p_expression->right->object);

                    struct flow_alternatives mapped = { 0 };
                    bool all_mapped = (p_src != NULL && p_src->alternatives.size > 0);

                    for (int i = 0; all_mapped && p_src != NULL && i < p_src->alternatives.size; i++)
                    {
                        const struct flow_alternative* a0 = p_src->alternatives.data[i];

                        /* The operand usually resolves to a REF to the variable object;
                            follow it to the actual value alternatives. 
                         */
                        const struct flow_key_alternatives* _Opt p_vals = NULL;
                        if (a0->value_kind == FLOW_VALUE_KIND_REF && a0->value.p != NULL)
                            p_vals = flow_branch_search_up(ctx->p_current_flow_branch, a0->value.p);

                        /* data is an array of pointers now, so list[j] is already a
                           struct flow_alternative* -- when there's no REF to follow,
                           use a synthetic one-element array holding a0 itself instead
                           of treating a0 (one alternative) as if it were the array. */
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
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
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
                        flow_alternatives_add(&e->alternatives, &a);
                    }
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
                struct flow_branch_pair child = flow_visit_expression(ctx, p_expression->right);

                if (object_has_constant_value(&p_expression->right->object))
                {
                    /* Both sub-expression and its value are now known: fold directly. */
                    const long long rv = object_to_signed_long_long(&p_expression->right->object);
                    const long long result = rv ? 0 : 1;
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = result},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                    if (result != 0)
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_NOT_DEAD_FALSE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_branch_pair)
                        {
                        ctx->p_current_flow_branch, p_dead
                        };
                    }
                    else
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_NOT_DEAD_TRUE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_branch_pair)
                        {
                        p_dead, ctx->p_current_flow_branch
                        };
                    }
                }

                /* Seed the NOT result's OWN value: `!x` yields a boolean (0 or 1) and is
                    always INITIALIZED. Without this, `bool c = !x;` (non-constant x) left
                    c with no value and c was wrongly reported "possibly uninitialized". */
                {
                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
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
                    flow_alternatives_add(&e->alternatives, &a);
                }

                flow_branch_remove(ctx->p_current_flow_branch, &p_expression->right->object);
                /* NOT swaps the two branches. */
                return (struct flow_branch_pair)
                {
                child.p_false, child.p_true
                };
            }

            case EXPR_CHECKED:
            {
                _Assert(p_expression->left != NULL);
                struct flow_branch_pair checked_pair = flow_visit_expression(ctx, p_expression->left);
                flow_exit_block_visit_defer_list(ctx, &p_expression->defer_list, p_expression->first_token);
                flow_defer_list_set_end_of_lifetime(ctx, &p_expression->defer_list, p_expression->first_token);
                return checked_pair;
            }

            case EXPR_UNARY_SIZEOF_TYPE:
            case EXPR_UNARY_COUNTOF:
                /* Constant when the parser folded it. For a VLA `sizeof` the parser
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
                flow_visit_expression(ctx, p_operand);

                /* Mutating the operand invalidates any predicate over it. */
                flow_predicate_invalidate(ctx, object_get_referenced(&p_operand->object));

                /* ++v[i] / v[i]-- with an unknown i: no element of v keeps its value. */
                flow_invalidate_unknown_index_write(ctx, p_operand);

                bool is_postfix = (p_expression->expression_type == EXPR_POSTFIX_INCREMENT ||
                           p_expression->expression_type == EXPR_POSTFIX_DECREMENT);
                bool is_increment = (p_expression->expression_type == EXPR_UNARY_INCREMENT ||
                             p_expression->expression_type == EXPR_POSTFIX_INCREMENT);

                /*
                ++ / -- are disallowed on an _Owner pointer: advancing it loses the
                very address that has to be freed, so the allocation could never be
                released through it.

                Moved here from expressions.c so that every diagnostic mentioning
                _Owner lives in flow3 -- and extended while moving: the parser only
                checked the POSTFIX forms, so `++p` / `--p` on an owner went
                completely unreported. All four forms land in this case.
                */
                if (type_is_owner(&p_operand->object.type))
                {
                    diagnostic(is_increment
                       ? C_ERROR_FLOW_OPERATOR_INCREMENT_CANNOT_BE_USED_IN_OWNER
                       : C_ERROR_FLOW_OPERATOR_DECREMENT_CANNOT_BE_USED_IN_OWNER,
                       ctx->ctx,
                       p_operand->first_token, NULL,
                       is_increment
                       ? "operator ++ cannot be used in _Owner pointers"
                       : "operator -- cannot be used in _Owner pointers");
                }

                /* Resolve the operand's object to its actual alternatives. */
                const struct object* p_obj = &p_operand->object;
                const struct flow_key_alternatives* _Opt p_entry =
                flow_branch_search_up(ctx->p_current_flow_branch, p_obj);
                if (p_entry == NULL)
                {
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
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
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
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    break;
                }

                /* Advance the object(s) the operand names. An lvalue may alias
                    several objects -- e.g. `(*p)++` where p can point to a or b -- so
                    iterate its REF alternatives the way flow_check_assigment handles
                    an assignment destination, rather than a size==1 / data[0] shortcut.
                    Each referenced object's values are advanced, tagged with the branch
                    the reference belongs to so the update stays correlated. */
                struct flow_alternatives new_result_alts = { 0 };
                bool advanced_any = false;

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
                            /* Advancing a pointer preserves its null-ness (it still
                                points within the same object/array, so non-null stays
                                non-null) but moves it to a DIFFERENT element -- the
                                pointed-to VALUE is now unknown. Keeping the SAME pointee
                                object would leave a stale fact like `*p == c` (from an
                                earlier narrowing, e.g. a `while (*p != '"') p++;` loop
                                exit) attached to the advanced pointer, which wrongly
                                folded `*p != c` to false and reported dead code
                                (tokenizer.c). Repoint to a fresh ANY pointee; a pointer
                                copied off BEFORE the increment keeps the old pointee, so
                                its knowledge of `*q` is correctly preserved. */
                            struct flow_alternative a = *alt;
                            if (alt->value_relation == FLOW_RELATION_EQUAL &&
                            alt->value.p != NULL &&
                            type_is_pointer(&p_operand->object.type))
                            {
                                struct object* _Opt p_fresh = flow_allocated_object_arena_new(&ctx->allocated_object_arena);
                                if (p_fresh != NULL)
                                {
                                    struct type pointed_type = type_remove_pointer(&p_operand->object.type);
                                    make_object(&pointed_type, p_fresh, MAKE_STATE_ANY, ctx->ctx->options.target);
                                    type_destroy(&pointed_type);
                                    a.value.p = p_fresh;
                                }
                                else
                                {
                                    /* Can't allocate a fresh pointee: fall back to a
                            generic non-null pointer (drops the stale value
                            without inventing a bogus one). */
                                    a.value.p = NULL;
                                    a.value_relation = FLOW_RELATION_NOT_EQUAL;
                                }
                            }
                            flow_alternatives_add(&new_var_alts, &a);
                            flow_alternatives_add(&new_result_alts, &a);
                        }
                        else if (type_is_pointer(&p_operand->object.type))
                        {
                            /* A pointer whose tracked value is ANY (a merge, or a
                                member seeded without a concrete target) is not PTR-kind, so
                                it used to fall into the generic branch below and come back
                                as a SIGNED ANY -- which "could be zero", making the very
                                next use report "may be null". Advancing a pointer is the
                                one case where nullness cannot be introduced: null + 1 is
                                undefined behaviour, not a null result. Decide it from the
                                operand's static type, the same way the binary `p + n` form
                                does, and keep the result non-null with no known target. */
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
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = ~rv},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                }
                else
                {
                    /* Operand value unknown — result is also unknown. */
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
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
                        flow_alternatives_add(&e->alternatives, &a);
                    }
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

                bool content_lifetime_ended_reported = false;

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

                            /* Lifetime check: `*p` after p's pointee was freed/moved
                                (e.g. consumed by an _Owner parameter, or _Dtor'd)
                                mirrors the same check EXPR_POSTFIX_ARROW does for
                                `p->member` -- without it, `*p = 0;` after `consume(p)`
                                (p an _Owner pointer parameter, no member access
                                involved) went entirely unchecked. See the two-origin
                                rationale on flow_object_leaves_in_state_2 above:
                                same shape applies here, just checking the WHOLE
                                pointee rather than one member (there's no member
                                index for `*p`, only a value it derefs to). */
                            int ended_line = 0;
                            const struct flow_branch* _Opt ended_origin = NULL;
                            if (p_right_alt2->value_kind == FLOW_VALUE_KIND_PTR &&
                            p_right_alt2->value.p != NULL &&
                            !content_lifetime_ended_reported &&
                            flow_object_leaves_in_state_2(ctx, p_right_alt2->value.p, FLOW_LEAF_ENDED,
                                                          p_right_alt2->p_origin_map, ctx->p_current_flow_branch, false, &ended_line, &ended_origin))
                            {
                                content_lifetime_ended_reported = true;
                                struct osstream ss = { 0 };
                                flow_expression_to_string(p_expression, &ss);
                                const bool reported_ended = diagnostic(W_FLOW_LIFETIME_ENDED, ctx->ctx, NULL, &marker,
                                                                   "dereference of '%s': pointed object lifetime has ended (see line %d)",
                                                                   ss.c_str ? ss.c_str : "", ended_line);
                                ss_close(&ss);
                                if (reported_ended)
                                    flow_diagnose_map_path(ctx, ended_origin);

                                /* If this same dereference is ALSO used as
                                an assignment/return/argument source,
                                flow_check_object_init_assigment runs
                                right after and would otherwise report
                                this identical fact a second time -- see
                                the field comment in flow3.h. */
                                ctx->p_pending_ended_report_obj = p_right_alt2->value.p;
                                ctx->pending_ended_report_line = ended_line;
                            }

                            if (flow_alternative_can_be_zero(p_right_alt2) &&
                            !ctx->expression_is_not_evaluated &&
                            flow_origins_compatible(p_right_alt2->p_origin_map, ctx->p_current_flow_branch))
                            {
                                /* The operand of sizeof/_Alignof (and other unevaluated
                                contexts) is never dereferenced at runtime -- only its
                                type is needed -- so a possibly-null pointer there is
                                not an actual null dereference.

                                The origin check drops a null value that cannot occur
                                on the current path: if its branch decisions conflict
                                with where we are (e.g. it is the "else" value of a
                                condition whose "then" branch we are inside), the
                                dereference is safe here. */
                                        /* Include the whole dereference expression ('*p', not just the pointer operand) in a null-dereference diagnostic so the reader knows which pointer it's about in a function with more than one -- user-requested, and specifically '*p' here unlike `->` sites, which print just the pointer since the operator name already says what's happening. */
                                struct osstream ss = { 0 };
                                flow_expression_to_string(p_expression, &ss);
                                const bool reported_null = diagnostic(W_FLOW_NULL_DEREFERENCE, ctx->ctx, NULL, &marker,
                                                                  "possible null pointer dereference '%s'",
                                                                  ss.c_str ? ss.c_str : "");
                                ss_close(&ss);
                                if (reported_null)
                                    flow_explain_alternative(ctx, p_right_alt2, p_right_alt2->p_origin_map, &marker);
                            }

                            /* Only a pointer alternative that names its target
                               says what `*p` is. A "not null, target unknown"
                               alternative (value.p == NULL) used to be turned into
                               a REF to nothing, which counted as information here
                               and blocked the ANY seeding below -- `e = **pp;`
                               then left e at its previous value. */
                            if (p_right_alt2->value_kind == FLOW_VALUE_KIND_PTR &&
                            p_right_alt2->value.p != NULL)
                            {
                                struct flow_alternative a =
                                {
                            .value_kind = FLOW_VALUE_KIND_REF,
                            .value = {.p = p_right_alt2->value.p},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                                /* Carry the pointer value's branch origin so a deref
                            stays correlated: `p = &a@then / &b@else` gives
                            `*p = ref a@then / ref b@else`. */
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

                /* Nothing resolved -- the operand is not a tracked pointer, as in
                   `*get()` or `**pp`, where no REF alternative leads anywhere with
                   state. Leaving the result with NO alternatives at all reads as
                   "no information" further up, and an assignment from it left the
                   destination sitting at its previous value: `e = 0; if (cond) e =
                   *get(); if (e == 0)` folded to always-true (compile.c:287, where
                   the source was `error = errno`). Seed the same ANY the subscript
                   path seeds for an unresolved element. */
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
                     ctx->ctx->options.null_checks_enabled &&
                     !type_is_nullable(&p_expression->object.type, ctx->ctx->options.null_checks_enabled))
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
                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);

                flow_check_assigment(ctx, p_expression->left, p_expression->right);

                /* Writing the destination invalidates any predicate over it. */
                flow_predicate_invalidate(ctx, object_get_referenced(&p_expression->left->object));

                /* v[i] = x with an unknown i: no element of v keeps its value. */
                flow_invalidate_unknown_index_write(ctx, p_expression->left);

                /* Seed an assignment EXPRESSION's own object (not just its ->left) as a REF to the resolved destination, so a later lookup on the assignment's own node -- e.g. `while ((dp = readdir(dir)) != NULL)` -- finds the narrowed value; previously only the destination variable itself got updated. */
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
                struct flow_branch* _Opt p_true = flow_narrow_map_branch(&ctx->flow_branch_arena,
                                                                  ctx->p_current_flow_branch,
                                                                  &p_expression->left->object,
                                                              true,
                                                                  p_expression,
                                                                  p_expression->first_token);
                if (p_true == NULL)
                    throw;

                struct flow_branch* _Opt p_false = flow_narrow_map_branch(&ctx->flow_branch_arena,
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

                return (struct flow_branch_pair)
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

                struct flow_branch_pair lhs_pair2 = flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);

                const struct object* p_left_obj = object_get_referenced(&p_expression->left->object);
                struct flow_key_alternatives* _Opt p_lhs_entry = flow_branch_search_up(ctx->p_current_flow_branch, p_left_obj);

                /* Writing the destination invalidates any predicate over it. */
                flow_predicate_invalidate(ctx, p_left_obj);

                /* v[i] op= x with an unknown i: no element of v keeps its value. */
                flow_invalidate_unknown_index_write(ctx, p_expression->left);

                /* Compound assignment folds per LHS alternative, so a correlated
                    join survives it (e.g. `if(c)a=1;else a=3; a+=10;` -> {11,13}).
                    Iterate every alternative -- never data[0] -- keeping each value's
                    branch origin. A pointer alternative (p += n / p -= n) is kept as-is:
                    arithmetic can't turn a valid pointer into a null one. If any
                    alternative can't be folded, degrade the whole destination to ANY. */
                const bool rhs_known = object_has_known_value(&p_expression->right->object);
                const signed long long rv =
                rhs_known ? object_to_signed_long_long(&p_expression->right->object) : 0;

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
                    else if (p_expression->left != NULL &&
                     type_is_pointer(&p_expression->left->object.type))
                    {
                        /* Same rule as ++/--: `p += n` cannot produce a null
                   pointer, so a pointer whose tracked value is ANY must
                   not degrade the destination to a plain unknown that
                   "could be zero". Decided from the static type, since an
                   ANY alternative is not PTR-kind. */
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
                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, p_left_obj);
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

                /* Casting a TEMPORARY owner (a function return value) to a non-owner
                    throws the ownership away with nothing left holding it -- e.g.
                    `(int*) malloc(1)`. Moved here from expressions.c so that every
                    diagnostic mentioning _Owner lives in flow3. */
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
                    /* An owner-to-owner cast (e.g. `(void* _Owner)p->member`) doesn't change identity -- model its result as a REF to the left operand's own object, not an independent derived value, so a later move of the cast result (e.g. free()) correctly marks the ORIGINAL object moved too. See owner-cast-move-through-member.c. */
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
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
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
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    break;
                }

                struct flow_alternatives new_alts = { 0 };
                bool all_handled = true;

                /* Cast every source value. Iterate REF alternatives per-alternative
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
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
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
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                }

                /* Casting an owner to an _Owner target transfers ownership: the source
                    is moved into the cast result. Without this, `free((void* _Owner)s)`
                    freed the cast temporary but left the original `s` looking un-moved,
                    producing a false "owner object 's' not moved" leak warning. 
                */
                if (type_is_owner(p_target_type) && type_is_owner(&p_expression->left->object.type))
                {
                    const struct object* p_src_var = object_get_referenced(&p_expression->left->object);
                    flow_branch_set_object_moved(ctx->p_current_flow_branch, p_src_var, p_expression->first_token);
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

                /* Fold across all alternatives (per-alternative REF resolution and
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

                /* Attempt to fold the relational expression across ALL alternatives */
                /* of both operands (per-alternative REF resolution; no size==1). */
                int fold_result = flow_evaluate_relational_multi(ctx,
                                                             p_expression->left,
                                                             p_expression->right,
                                                             p_expression->expression_type);

                if (fold_result != -1)
                {
                    /* Expression always true or always false */
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;

                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = fold_result ? 1 : 0},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    if (ctx->p_current_flow_branch == NULL)
                    {
                        throw;
                    }

                    struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena,
                                                                       ctx->p_current_flow_branch,
                                                                       FLOW_BRANCH_REL_DEAD);
                    if (p_dead == NULL)
                        throw;

                    return (struct flow_branch_pair)
                    {
                    fold_result ? ctx->p_current_flow_branch : p_dead,
                        fold_result ? p_dead : ctx->p_current_flow_branch
                    };
                }

                /* Not foldable: seed the per-path boolean result (or ANY) so an */
                /* enclosing ||/&& / compile_assert can reason per path. */
                flow_seed_comparison_result(ctx, p_expression);

                /* ... but if this compares a scalar variable against a constant, we
                    can still narrow the variable on each branch (true: var OP c,
                    false: var !OP c). This is what lets `if (a > 0)` -- and, via the
                    EXPR_UNARY_ASSERT true-branch merge, `_Assert(a > 0)` -- record the
                    half-line fact so a later compile_assert(a > 0) can prove it. 
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
                        flow_narrow_operand_relational(ctx, p_var_expr, cst, narrow_op,
                                                   p_true, p_false, p_expression->first_token);
                        
                        return (struct flow_branch_pair) { p_true, p_false };
                    }
                }
                return (struct flow_branch_pair)
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

                struct flow_key_alternatives* _Opt p_result_alternatives = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                if (p_result_alternatives == NULL) throw;
                flow_alternatives_clear(&p_result_alternatives->alternatives);

                const bool is_equal_op = (p_expression->expression_type == EXPR_EQUALITY_EQUAL);

                /* Fold across ALL alternatives of both operands. A constant is simply
                    an operand with a single alternative -- no special case. 
                */
                int fold = flow_evaluate_equality_multi(ctx, p_expression->left, p_expression->right, is_equal_op);
                if (fold != -1)
                {
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = fold ? 1 : 0},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    if (ctx->p_current_flow_branch == NULL)
                    {
                        throw;
                    }

                    struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena,
                                                                       ctx->p_current_flow_branch,
                                                                       FLOW_BRANCH_EQ_DEAD);
                    if (p_dead == NULL)
                        throw;

                    return (struct flow_branch_pair)
                    {
                    fold ? ctx->p_current_flow_branch : p_dead,
                        fold ? p_dead : ctx->p_current_flow_branch
                    };
                }

                /* Not foldable: if one operand is a single constant, narrow the other
                    on each branch. (A constant naturally collapses to one value across
                    its alternatives.) */
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
                    flow_narrow_operand_equality(ctx, p_var_expr, cst, is_equal_op,
                                             p_true, p_false, p_expression->first_token);
                    flow_seed_comparison_result(ctx, p_expression);
                    return (struct flow_branch_pair)
                    {
                    p_true, p_false
                    };
                }

                /* -------- Fallback: unknown -------- */
                flow_seed_comparison_result(ctx, p_expression);
                return (struct flow_branch_pair)
                {
                ctx->p_current_flow_branch, ctx->p_current_flow_branch
                };
            }
            /* break; */

            case EXPR_LOGICAL_OR:
            {
                /*
                * L || R
                *   true  = merge(left_true, right_true_from_left_false)
                *           (left was true, OR left was false but right was true)
                *   false = right_false_from_left_false
                *           (both were false)
                */
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                if (object_has_constant_value(&p_expression->left->object) &&
                object_has_constant_value(&p_expression->right->object))
                {
                    const long long result = (object_to_signed_long_long(&p_expression->left->object) ||
                                      object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = result},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    if (result != 0)
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_OR_DEAD_FALSE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_branch_pair)
                        {
                        ctx->p_current_flow_branch, p_dead
                        };
                    }
                    else
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_OR_DEAD_TRUE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_branch_pair)
                        {
                        p_dead, ctx->p_current_flow_branch
                        };
                    }
                }

                struct flow_branch_pair left_pair = flow_visit_expression(ctx, p_expression->left);

                /* Visit right on the false map of left (right only runs when left is false). */
                ctx->p_current_flow_branch = left_pair.p_false;
                struct flow_branch_pair right_pair = flow_visit_expression(ctx, p_expression->right);
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

                /* Seed this OR's per-path boolean value. For each path (identified by
                    origin), `L || R` is true if L is true there, else R's value there.
                    L was evaluated on p_before; R on left's false map. Only applied
                    when both sides are clean per-path booleans -- otherwise the result
                    is left unseeded (previous behavior). This lets compile_assert see
                    a 0 exactly on a path where neither disjunct holds.
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

                        /* Unseeded is not the same as unchanged: an earlier evaluation of this node (a loop's suppressed first pass, say) may have left a now-stale value here -- empty the entry (not an explicit ANY) so it shadows the old value without asserting a value nothing can prove, matching how consumers like compile_assert read 'no value' vs ANY differently. */
                        struct flow_key_alternatives* _Opt e =
                        flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e != NULL)
                        {
                            flow_alternatives_clear(&e->alternatives);
                        }
                    }
                }

                return (struct flow_branch_pair)
                {
                p_or_true, right_pair.p_false
                };
            }

            case EXPR_LOGICAL_AND:
            {
                /*
                * L && R
                *   true  = right_true_from_left_true
                *           (both were true)
                *   false = merge(left_false, right_false_from_left_true)
                *           (left was false, OR left was true but right was false)
                */
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);

                if (object_has_constant_value(&p_expression->left->object) &&
                object_has_constant_value(&p_expression->right->object))
                {
                    const long long result = (object_to_signed_long_long(&p_expression->left->object) &&
                                      object_to_signed_long_long(&p_expression->right->object)) ? 1 : 0;
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = result},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    if (result != 0)
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_AND_DEAD_FALSE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_branch_pair)
                        {
                        ctx->p_current_flow_branch, p_dead
                        };
                    }
                    else
                    {
                        struct flow_branch* _Opt p_dead = flow_branch_arena_new_dead(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_AND_DEAD_TRUE);
                        if (p_dead == NULL)
                            throw;

                        return (struct flow_branch_pair)
                        {
                        p_dead, ctx->p_current_flow_branch
                        };
                    }
                }

                struct flow_branch_pair left_pair = flow_visit_expression(ctx, p_expression->left);

                if (object_has_constant_value(&p_expression->left->object) &&
                object_is_true(&p_expression->left->object) == false)
                {
                    /* Left is always false: short-circuit, right never evaluated. */
                    return left_pair;
                }

                /* Visit right on the true map of left (right only runs when left is true). */
                ctx->p_current_flow_branch = left_pair.p_true;
                struct flow_branch_pair right_pair = flow_visit_expression(ctx, p_expression->right);
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

                /* Seed this AND's per-path boolean value (dual of ||): for each path,
                    `L && R` is 0 if L is false there, else R's value there. L was
                    evaluated on p_before; R on left's true map. Only when both sides
                    are clean per-path booleans; otherwise leave unseeded (previous
                    behavior). Lets compile_assert see a 0 where either side fails. 
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

                        /* Unseeded is not the same as unchanged: an earlier evaluation of this node (a loop's suppressed first pass, say) may have left a now-stale value here -- empty the entry (not an explicit ANY) so it shadows the old value without asserting a value nothing can prove, matching how consumers like compile_assert read 'no value' vs ANY differently. */
                        struct flow_key_alternatives* _Opt e =
                        flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e != NULL)
                        {
                            flow_alternatives_clear(&e->alternatives);
                        }
                    }
                }

                return (struct flow_branch_pair) { right_pair.p_true, p_and_false };
            }

            case EXPR_INCLUSIVE_OR:
                _Assert(p_expression->right != NULL);
                _Assert(p_expression->left != NULL);
                if (object_has_constant_value(&p_expression->left->object) &&
                object_has_constant_value(&p_expression->right->object))
                {
                    const long long lv = object_to_signed_long_long(&p_expression->left->object);
                    const long long rv = object_to_signed_long_long(&p_expression->right->object);
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                            .value_kind = FLOW_VALUE_KIND_SIGNED,
                            .value = {.i = lv | rv},
                            .value_relation = FLOW_RELATION_EQUAL,
                            .imaginary = FLOW_IMAGINARY_NONE,
                            .p_origin_map = ctx->p_current_flow_branch,
                            .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    break;
                }
                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);
                {
                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
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
                    flow_alternatives_add(&e->alternatives, &a);
                }
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
                    {
                        struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
                        if (e == NULL) throw;
                        flow_alternatives_clear(&e->alternatives);
                        struct flow_alternative a =
                        {
                    .value_kind = FLOW_VALUE_KIND_SIGNED,
                    .value = {.i = result},
                    .value_relation = FLOW_RELATION_EQUAL,
                    .imaginary = FLOW_IMAGINARY_NONE,
                    .p_origin_map = ctx->p_current_flow_branch,
                    .p_origin_token = p_expression->first_token
                        };
                        flow_alternatives_add(&e->alternatives, &a);
                    }
                    break;
                }
                flow_visit_expression(ctx, p_expression->left);
                flow_visit_expression(ctx, p_expression->right);
                {
                    struct flow_key_alternatives* _Opt e = flow_branch_find_add(ctx->p_current_flow_branch, &p_expression->object);
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
                    flow_alternatives_add(&e->alternatives, &a);
                }
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
                /* Comma: the value (and branch state) of the right operand is what matters. */
                struct flow_branch_pair pair = flow_visit_expression(ctx, p_expression->right);

                /* Forward the right operand's value to the comma's OWN object, so a
                    consumer that reads this node (e.g. a function-argument check) sees
                    the comma's result -- otherwise `f((p = 0, p))` found no value on the
                    comma node and missed that p was just set to null. Mirrors the value
                    forwarding done for EXPR_PRIMARY_PARENTHESIS. 
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

                struct flow_branch_pair cond_pair = flow_visit_expression(ctx, p_expression->condition_expr);
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
                * Collect the result value of the conditional expression: the
                * true arm carries the value from left (or condition_expr), the
                * false arm from right. Append both so downstream consumers
                * (static_debug, assert_state, etc.) can see it.
                *
                * This happens BEFORE the arms are merged back into p_before.
                * The merge frees each arm's own entries, so a later
                * flow_branch_search_up through an arm lands on the merged copy
                * in p_before -- and that copy also carries the "unknown on the
                * other path" alternative flow_branch_merge_arms contributes for
                * an object only one arm wrote (which is every arm temporary).
                * Reading after the merge made `c ? "a" : "b"` look possibly
                * null.
                *
                * A REF alternative is resolved inside the arm that produced it
                * for the same reason: `p ? p : ""` was otherwise discarding the
                * true arm's narrowing. See conditional-operator-null-guard.c.
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
                        /* A condition folded at compile time (`(c == 1) ? a : b`
                           with c known) leaves one arm dead, exactly as an `if`
                           does; the value of the dead arm is not a possible
                           result. Same test flow_branch_merge_arms applies. */
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
    }

    struct flow_branch_pair identity_pair = { ctx->p_current_flow_branch, ctx->p_current_flow_branch };

    /* Non-boolean or unhandled expression: both branches are identical (no narrowing). */
    return identity_pair;
}

static void flow_visit_expression_statement(struct flow_ctx* ctx, const struct expression_statement* p_expression_statement)
{
    /* Only meant to bridge a report from THIS statement's own expression
       visit into a check running right after it (see the field comment in
       flow3.h) -- must not leak into an unrelated later statement. */
    ctx->p_pending_ended_report_obj = NULL;

    if (p_expression_statement->expression_opt)
    {
        flow_visit_full_expression(ctx, p_expression_statement->expression_opt);
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

static void flow_visit_do_while_statement(struct flow_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    _Assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);

    /* Saved outside the try so the catch below restores them on every exit. */
    struct flow_branch* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow_branch* _Opt old_p_break_join_map = ctx->p_break_join_map;

    try
    {
        if (ctx->p_current_flow_branch == NULL)
            return;

        struct flow_branch* p_before = ctx->p_current_flow_branch;
        struct flow_branch* _Opt p_break_join = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_BREAK_JOIN);
        if (p_break_join == NULL)
            return; /* no map to work with */

        ctx->p_initial_map = p_before;
        ctx->p_break_join_map = p_break_join;

        struct flow_branch_pair do_cond_pair1 = { 0 };

        /*
         * do-while: the body executes first, then the condition is checked.
         * First pass — suppress warnings (warm up state for second pass).
         */
        diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

        /* Two levels, as in flow_visit_for_statement: an empty body-entry map
           whose keys are the pre-loop ones by inheritance, and a child of it
           the body writes into, so "what the body assigned" stays recoverable
           for flow_widen_loop_variant_objects and for the join below. */
        struct flow_branch* _Opt p_pass1_body_entry =
            flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_FOR_BODY_PASS1);
        if (p_pass1_body_entry != NULL)
        {
            struct flow_branch* _Opt p_pass1_body = flow_branch_arena_new(&ctx->flow_branch_arena,
                                                                    p_pass1_body_entry, FLOW_BRANCH_FOR_BODY_PASS1);
            if (p_pass1_body != NULL)
            {
                ctx->p_current_flow_branch = p_pass1_body;
            }
        }

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        /* State after one iteration; see flow_widen_loop_variant_objects. */
        struct flow_branch* _Opt p_pass1_exit = ctx->p_current_flow_branch;

        /*
           If every path through the first pass's body unconditionally
           transferred control away, ctx->p_current_flow_branch is now marked
           is_unreachable (see flow_visit_jump_statement); there is no sound
           "condition after one iteration" state to compute, and running the
           second pass on top of it would just build on dead/dead-code-
           polluted state (see the identical reasoning in
           flow_visit_while_statement).
        */
        const bool body_falls_through =
            !(ctx->p_current_flow_branch != NULL && ctx->p_current_flow_branch->is_unreachable);

        /*
           Put the first iteration back alongside the second before the
           diagnostic pass runs, the way flow_visit_for_statement does. Pass 1
           leaves body-assigned values at the result of exactly ONE iteration,
           so without this the pass that actually reports runs from "the body
           already ran once" and contradicts what the first iteration sees.
        */
        if (body_falls_through && p_pass1_body_entry != NULL && p_pass1_exit != NULL)
        {
            struct flow_branch* _Opt widen_arms[1] = { p_pass1_exit };
            flow_widen_loop_variant_objects(p_pass1_body_entry, p_pass1_exit,
                                            widen_arms, 1, p_iteration_statement->first_token,
                                            true);

            /* "First iteration" arm: an empty child of the body-entry state, so
               it contributes the pre-loop value of every key by inheritance. */
            struct flow_branch* _Opt p_first_iteration = flow_branch_arena_new(&ctx->flow_branch_arena,
                                                                         p_pass1_body_entry, FLOW_BRANCH_FOR_BODY_PASS1);
            if (p_first_iteration != NULL)
            {
                const struct flow_branch* const join_arms[2] = { p_first_iteration, p_pass1_exit };
                flow_branch_merge_arms(p_pass1_body_entry, join_arms, 2);
                ctx->p_current_flow_branch = p_pass1_body_entry;
            }
        }

        if (body_falls_through && p_iteration_statement->expression1)
        {
            do_cond_pair1 = flow_visit_full_expression(ctx, p_iteration_statement->expression1);
            /* Narrow to true branch before running second-pass body, mirroring while. */
            ctx->p_current_flow_branch = do_cond_pair1.p_true;
        }

        /* Second pass — warnings on */
        diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

        if (body_falls_through)
        {
            ctx->iteration_pass = 2; /*second pass -- see flow_visit_iteration_statement*/
            flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
            ctx->iteration_pass = 1;
        }
        else
        {
            /*
               Every path through the body diverges, so there is nothing
               sound to compute for a second iteration -- but the only pass
               that visited this body ran with diagnostics suppressed. Give
               it one more visit, fresh from p_before, purely so real
               diagnostics can fire; the result is discarded (see the
               identical reasoning in flow_visit_while_statement).
            */
            ctx->p_current_flow_branch =
                flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_DO_WHILE_BODY_DIAG);
            flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        }

        if (ctx->p_current_flow_branch == NULL)
            return;

        struct flow_branch* p_after_body = ctx->p_current_flow_branch;
        struct flow_branch_pair do_cond_pair2 = { 0 };
        if (body_falls_through && p_iteration_statement->expression1)
        {
            do_cond_pair2 = flow_visit_full_expression(ctx, p_iteration_statement->expression1);
        }

        if (p_iteration_statement->secondary_block != NULL)
        {
            flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        }

        struct flow_branch* _Opt p_false_branch_dw = do_cond_pair2.p_false ? do_cond_pair2.p_false
            : flow_branch_arena_new(&ctx->flow_branch_arena, p_after_body, FLOW_BRANCH_DO_WHILE_FALSE);

        if (p_false_branch_dw == NULL)
        {
            throw;
        }

        {
            /* Loop exit paths: condition false after an iteration, or break. */
            /* Pre-filled with p_before so no element is ever indeterminate;
               only the first num_arms entries are read. */
            struct flow_branch* _Opt exit_arms[2] = { p_false_branch_dw, p_break_join };
            flow_widen_loop_variant_objects(p_pass1_exit, ctx->p_current_flow_branch,
                                            exit_arms, 2, p_iteration_statement->first_token,
                                             false);

            const struct flow_branch* arms[2] = { p_before, p_before };
            int num_arms = 0;
            if (body_falls_through)
            {
                arms[num_arms++] = p_false_branch_dw;
            }
            if (flow_branch_arm_has_entries(p_break_join, p_before))
            {
                arms[num_arms++] = p_break_join;
            }
            if (num_arms > 0)
            {
                flow_branch_merge_arms(p_before, arms, num_arms);
            }
        }

        ctx->p_current_flow_branch = p_before;

        if (p_iteration_statement->secondary_block != NULL)
        {
            flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        }

    }
    catch
    {
    }

    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
}

static bool flow_entry_numeric_value(const struct flow_key_alternatives* _Opt e,
                                     long long* out,
                                     bool allow_repeated_value)
{
    if (e == NULL || e->alternatives.size == 0)
    {
        return false;
    }

    bool found = false;
    long long value = 0;

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

        if (found && (!allow_repeated_value || this_value != value))
        {
            return false;
        }

        value = this_value;
        found = true;
    }

    if (!found)
    {
        return false;
    }

    *out = value;
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

    /* Every key the second pass's body touched. */
    for (struct flow_branch* _Opt cur = p_pass2_exit;
            cur != NULL && cur != p_pass1_exit;
            cur = cur->p_parent_map)
    {
        if (cur->buckets == NULL)
        {
            continue;
        }

        for (int i = 0; i < cur->num_of_buckets; i++)
        {
            for (const struct flow_key_alternatives* _Opt e = cur->buckets[i]; e; e = e->next)
            {
                long long pass1_value = 0;
                long long pass2_value = 0;

                if (!flow_entry_numeric_value(
                    flow_branch_search_up(p_pass1_exit, e->p_obj_key), &pass1_value,
                    allow_repeated_value))
                {
                    continue;
                }

                if (!flow_entry_numeric_value(
                    flow_branch_search_up(p_pass2_exit, e->p_obj_key), &pass2_value,
                    allow_repeated_value))
                {
                    continue;
                }

                if (pass1_value != pass2_value)
                {
                    object_set_add(&variants, e->p_obj_key);
                }
            }
        }
    }

    for (int i = 0; i < variants.size; i++)
    {
        const struct object* p_key = variants.items[i];

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
                .value = {.i = ANY_VALUE},
                .value_relation = FLOW_RELATION_ANY,
                .imaginary = FLOW_IMAGINARY_NONE,
                .p_origin_map = arms[a],
                .p_origin_token = p_token
            };
            flow_alternatives_add(&e->alternatives, &any);
        }
    }

    object_set_destroy(&variants);
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

static void flow_join_first_iteration_values(struct flow_branch* _Opt p_body_entry,
                                             struct flow_branch* _Opt p_pass1_exit,
                                             const struct token* _Opt p_token)
{
    if (p_body_entry == NULL || p_pass1_exit == NULL || p_pass1_exit->is_unreachable)
    {
        return;
    }

    struct object_set assigned = { 0 };

    /* Every key the body wrote: the maps between the body's exit and its
       entry, exactly the walk flow_widen_loop_variant_objects does. */
    for (const struct flow_branch* _Opt cur = p_pass1_exit;
            cur != NULL && cur != p_body_entry;
            cur = cur->p_parent_map)
    {
        if (cur->buckets == NULL)
        {
            continue;
        }
        for (int i = 0; i < cur->num_of_buckets; i++)
        {
            for (const struct flow_key_alternatives* _Opt e = cur->buckets[i]; e; e = e->next)
            {
                object_set_add(&assigned, e->p_obj_key);
            }
        }
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

        if (!flow_alternatives_are_plain_values(&p_pre->alternatives) ||
                !flow_alternatives_are_plain_values(&p_post->alternatives))
        {
            /* moved/ended/absent on either side: an ownership fact, not a
               value. Leave it to pass 1. */
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

static void flow_visit_while_statement(struct flow_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    _Assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    if (p_iteration_statement->expression1 == NULL)
    {
        return;
    }

    struct flow_branch* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow_branch* _Opt old_p_break_join_map = ctx->p_break_join_map;

    if (ctx->p_current_flow_branch == NULL)
        return;

    struct flow_branch* p_before = ctx->p_current_flow_branch;
    struct flow_branch* _Opt p_break_join = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_BREAK_JOIN);
    if (p_break_join == NULL)
        return; /* no map to work with */

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;

    /* First pass — suppress warnings */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    struct flow_branch_pair w_pair1 = flow_visit_full_expression(ctx, p_iteration_statement->expression1);
    w_pair1 = flow_ensure_branch_pair(ctx, p_before, w_pair1,
                                      p_iteration_statement->expression1);
    ctx->p_current_flow_branch = w_pair1.p_true;

    /* Run the first pass's body in its own child map, for the reason
       flow_visit_for_statement gives at the same point: the body writes in
       place, so without a child map "what the body assigned" is not
       recoverable and flow_widen_loop_variant_objects collects nothing. */
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

    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    /* State after one iteration; compared with the second pass's to spot
       values that are still moving (see flow_widen_loop_variant_objects). */
    struct flow_branch* _Opt p_pass1_exit = ctx->p_current_flow_branch;

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    /*
       If every path through the first pass's body unconditionally
       transferred control away (return/break/continue/goto/throw),
       ctx->p_current_flow_branch is now marked is_unreachable by
       flow_visit_jump_statement, and there is no sound "after one
       iteration, re-check the condition" state to compute: the second
       pass would just build on top of dead (possibly dead-code-polluted)
       state. Skip the second pass entirely in that case, exactly the
       way merge_arms already skips is_unreachable arms elsewhere.
    */
    const bool body_falls_through =
        !(ctx->p_current_flow_branch != NULL && ctx->p_current_flow_branch->is_unreachable);

    /* Widen body-assigned numeric values before the diagnostic pass (like the for-statement does), but deliberately skip its 'zero iterations' join here: joining broke the free-and-advance idiom (`item->next` reading possibly-moved) with ~56 false warnings across cake's own sources; the narrow case it would fix is opt-in via warning 85. */
    if (body_falls_through && p_pass1_body_entry != NULL && p_pass1_exit != NULL)
    {
        struct flow_branch* _Opt widen_arms[1] = { p_pass1_exit };
        flow_widen_loop_variant_objects(p_pass1_body_entry, p_pass1_exit,
                                        widen_arms, 1, p_iteration_statement->first_token,
                                        true);

        flow_join_first_iteration_values(p_pass1_body_entry, p_pass1_exit,
                                         p_iteration_statement->first_token);
        ctx->p_current_flow_branch = p_pass1_body_entry;
    }

    struct flow_branch_pair w_pair2 = { 0 };
    struct flow_branch_pair w_pair3 = { 0 };
    if (body_falls_through)
    {
        w_pair2 = flow_visit_full_expression(ctx, p_iteration_statement->expression1);
        w_pair2 = flow_ensure_branch_pair(ctx, ctx->p_current_flow_branch, w_pair2,
                                          p_iteration_statement->expression1);

        ctx->p_current_flow_branch = w_pair2.p_true;

        ctx->iteration_pass = 2; /*second pass -- see flow_visit_iteration_statement*/
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        ctx->iteration_pass = 1;

        /* A third look at the condition, for the "left the loop after TWO or
           more iterations" exit. Without it the exit state reflects at most
           one iteration, so anything the body only assigns from the second
           iteration onward -- the `if (a == 0) a = p; else if (b == 0) b = p;`
           shape, where pass 1 always takes the first arm -- keeps its pre-loop
           value after the loop (tokenizer.c control_line's #line handling).
           Diagnostics off: this visit only computes state, and the condition
           has already been reported on twice. */
        diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
        w_pair3 = flow_visit_full_expression(ctx, p_iteration_statement->expression1);
        w_pair3 = flow_ensure_branch_pair(ctx, ctx->p_current_flow_branch, w_pair3,
                                          p_iteration_statement->expression1);
        diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);
    }
    else
    {
        /*
           Every path through the body diverges (return/break/continue/
           goto/throw), so there is no "after one iteration" state worth
           computing -- but the only pass that visited this body ran
           with diagnostics suppressed (it was just warming up state).
           Re-visit once more, fresh from the original true-branch entry
           point, purely so real diagnostics (e.g. unreachable-code) can
           fire; the resulting map is discarded, not merged anywhere,
           since this arm still does not fall through.
        */
        struct flow_branch_pair w_pair1_diag = { 0 };
        /* Re-evaluate from the pre-loop state, not from whatever pass 1 left
           in ctx->p_current_flow_branch: that map is the body's own (now
           unreachable) state, and narrowing done inside the body would
           otherwise be carried into the condition -- `while (n) { if (p) return;
           break; }` would see the `p == 0` its own body established. */
        ctx->p_current_flow_branch = p_before;
        w_pair1_diag = flow_visit_full_expression(ctx, p_iteration_statement->expression1);
        w_pair1_diag = flow_ensure_branch_pair(ctx, p_before, w_pair1_diag,
                                               p_iteration_statement->expression1);
        ctx->p_current_flow_branch = w_pair1_diag.p_true;
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }

    if (p_iteration_statement->secondary_block != NULL)
    {
        flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list,
                                         p_iteration_statement->secondary_block->last_token);
    }

    {
        /* Loop exit paths: condition false before the first iteration
           (w_pair1.p_false), condition false after an iteration
           (w_pair2.p_false), or break. */
        /* Pre-filled with p_before so no element is ever indeterminate; only
              the first num_arms entries are read. */
        struct flow_branch* _Opt exit_arms[4] = { w_pair1.p_false, w_pair2.p_false, w_pair3.p_false, p_break_join };
        flow_widen_loop_variant_objects(p_pass1_exit, ctx->p_current_flow_branch,
                                        exit_arms, 4, p_iteration_statement->first_token,
                                         false);

        const struct flow_branch* arms[4] = { p_before, p_before, p_before, p_before };
        int num_arms = 0;
        if (w_pair1.p_false != NULL)
        {
            arms[num_arms++] = w_pair1.p_false;
        }
        if (body_falls_through && w_pair2.p_false != NULL)
        {
            arms[num_arms++] = w_pair2.p_false;
        }
        if (body_falls_through && w_pair3.p_false != NULL)
        {
            arms[num_arms++] = w_pair3.p_false;
        }
        if (flow_branch_arm_has_entries(p_break_join, p_before))
        {
            arms[num_arms++] = p_break_join;
        }
        if (num_arms > 0)
        {
            flow_branch_merge_arms(p_before, arms, num_arms);
        }
    }

    ctx->p_current_flow_branch = p_before;

    if (p_iteration_statement->secondary_block != NULL)
    {
        flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list,
                                            p_iteration_statement->secondary_block->last_token);
    }

    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
}

static void flow_visit_for_statement(struct flow_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    _Assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);

    struct expression* _Opt p_next = p_iteration_statement->expression2;
    struct expression* _Opt p_condition = p_iteration_statement->expression1;

    if (p_iteration_statement->declaration &&
            p_iteration_statement->declaration->init_declarator_list.head)
    {
        flow_visit_init_declarator_list(ctx, &p_iteration_statement->declaration->init_declarator_list);
    }
    else if (p_iteration_statement->expression0)
    {
        flow_visit_full_expression(ctx, p_iteration_statement->expression0);
    }

    struct flow_branch* _Opt old_p_initial_map = ctx->p_initial_map;
    struct flow_branch* _Opt old_p_break_join_map = ctx->p_break_join_map;

    if (ctx->p_current_flow_branch == NULL)
        return;

    struct flow_branch* p_before = ctx->p_current_flow_branch;
    struct flow_branch* _Opt p_break_join = flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_BREAK_JOIN);
    if (p_break_join == NULL)
        return; /* no map to work with */

    ctx->p_initial_map = p_before;
    ctx->p_break_join_map = p_break_join;

    /* First pass — suppress warnings */
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    struct flow_branch_pair for_pair1 = { 0 };
    if (p_condition)
    {
        for_pair1 = flow_visit_full_expression(ctx, p_condition);
        for_pair1 = flow_ensure_branch_pair(ctx, ctx->p_current_flow_branch, for_pair1,
                                            p_condition);
        ctx->p_current_flow_branch = for_pair1.p_true;
    }

    /*
       Run the first pass's body in its own child map.

       The body writes state IN PLACE, so without this the map before the body
       and the map after it are the same pointer -- and
       flow_widen_loop_variant_objects, which walks the parent chain from the
       later state up to the earlier one to find the keys the body touched,
       terminates immediately and collects nothing. Giving the body its own map
       makes "what the body assigned" recoverable: those keys land in the child,
       and their pre-body values remain reachable via the parent.
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

    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    /* State after one iteration; see flow_widen_loop_variant_objects. */
    struct flow_branch* _Opt p_pass1_exit = ctx->p_current_flow_branch;

    /*
       If every path through the first pass's body unconditionally
       transferred control away, ctx->p_current_flow_branch is now marked
       is_unreachable (see flow_visit_jump_statement). The loop's increment
       expression (p_next) and a second pass would then just build on
       top of dead/dead-code-polluted state — skip both, mirroring
       flow_visit_while_statement / flow_visit_do_while_statement.
    */
    const bool body_falls_through =
        !(ctx->p_current_flow_branch != NULL && ctx->p_current_flow_branch->is_unreachable);

    if (body_falls_through && p_next)
    {
        flow_visit_full_expression(ctx, p_next);
    }

    /* Second pass — warnings on */
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    /* Widen body-assigned numeric values before the diagnostic pass runs: pass 1 leaves a value at one iteration's concrete result (`flag = 1`), so pass 2's `flag == 0` folds false and misreports a branch the first iteration actually reaches as unreachable -- same reasoning as flow_widen_loop_variant_objects. */
    if (body_falls_through && p_pass1_body_entry != NULL && p_pass1_exit != NULL)
    {
        /* Widen first (a MOVED value like `chain_len++` becomes ANY, or a later bound check folds to false and misreports the body unreachable), then join (a merely-SET value like `flag = 1` needs the zero-iterations case put back, or the first iteration's reachable branch folds to unreachable) -- order matters because the join destroys the value widening keys off. */
        struct flow_branch* _Opt widen_arms[1] = { p_pass1_exit };
        flow_widen_loop_variant_objects(p_pass1_body_entry, p_pass1_exit,
                                        widen_arms, 1, p_iteration_statement->first_token,
                                         true);

        /* "Zero iterations" arm: an empty child of the body-entry state, so it
           contributes the pre-loop value of every key by inheritance. */
        struct flow_branch* _Opt p_zero_iterations = flow_branch_arena_new(&ctx->flow_branch_arena,
                                                                     p_pass1_body_entry, FLOW_BRANCH_FOR_BODY_PASS1);
        if (p_zero_iterations != NULL)
        {
            const struct flow_branch* const join_arms[2] = { p_zero_iterations, p_pass1_exit };
            flow_branch_merge_arms(p_pass1_body_entry, join_arms, 2);
            ctx->p_current_flow_branch = p_pass1_body_entry;
        }
    }

    /* struct flow_branch* p_after_body = ctx->p_current_flow_branch; */
    struct flow_branch_pair for_pair2 = { 0 };
    struct flow_branch_pair for_pair3 = { 0 };
    if (body_falls_through && p_condition)
    {
        for_pair2 = flow_visit_full_expression(ctx, p_condition);
        for_pair2 = flow_ensure_branch_pair(ctx, ctx->p_current_flow_branch, for_pair2,
                                            p_condition);
        ctx->p_current_flow_branch = for_pair2.p_true;
    }

    if (body_falls_through)
    {
        ctx->iteration_pass = 2; /*second pass -- see flow_visit_iteration_statement*/
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
        ctx->iteration_pass = 1;

        if (p_next)
        {
            flow_visit_full_expression(ctx, p_next);
        }

        /* Third look at the condition -- see the same step in
           flow_visit_while_statement. */
        if (p_condition)
        {
            diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
            for_pair3 = flow_visit_full_expression(ctx, p_condition);
            for_pair3 = flow_ensure_branch_pair(ctx, ctx->p_current_flow_branch, for_pair3,
                                                p_condition);
            diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);
        }
    }
    else
    {
        /*
           Every path through the body diverges, so there is nothing
           sound to compute for a second iteration -- but the only pass
           that visited this body ran with diagnostics suppressed. Give
           it one more visit, fresh from p_before, purely so real
           diagnostics can fire; the result is discarded (see the
           identical reasoning in flow_visit_while_statement).
        */
        ctx->p_current_flow_branch =
            flow_branch_arena_new(&ctx->flow_branch_arena, p_before, FLOW_BRANCH_FOR_BODY_DIAG);
        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }

    if (p_iteration_statement->secondary_block != NULL)
    {
        flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list,
                                         p_iteration_statement->secondary_block->last_token);
    }

    {
        /* Loop exit paths: condition false before the first iteration,
           condition false after an iteration, or break. */
        /* Pre-filled with p_before so no element is ever indeterminate; only
              the first num_arms entries are read. */
        struct flow_branch* _Opt exit_arms[4] = { for_pair1.p_false, for_pair2.p_false, for_pair3.p_false, p_break_join };
        flow_widen_loop_variant_objects(p_pass1_exit, ctx->p_current_flow_branch,
                                        exit_arms, 4, p_iteration_statement->first_token,
                                         false);

        const struct flow_branch* arms[4] = { p_before, p_before, p_before, p_before };
        int num_arms = 0;
        if (p_condition)
        {
            if (for_pair1.p_false != NULL)
            {
                arms[num_arms++] = for_pair1.p_false;
            }
            if (body_falls_through && for_pair2.p_false != NULL)
            {
                arms[num_arms++] = for_pair2.p_false;
            }
            if (body_falls_through && for_pair3.p_false != NULL)
            {
                arms[num_arms++] = for_pair3.p_false;
            }
        }
        if (flow_branch_arm_has_entries(p_break_join, p_before))
        {
            arms[num_arms++] = p_break_join;
        }
        if (num_arms > 0)
        {
            flow_branch_merge_arms(p_before, arms, num_arms);
        }
    }

    ctx->p_current_flow_branch = p_before;

    if (p_iteration_statement->secondary_block != NULL)
    {
        flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list,
                                            p_iteration_statement->secondary_block->last_token);
    }

    ctx->p_initial_map = old_p_initial_map;
    ctx->p_break_join_map = old_p_break_join_map;
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
    for (const struct flow_branch* _Opt cur = ctx->p_current_flow_branch; cur != NULL; cur = cur->p_parent_map)
    {
        if (!cur->buckets)
        {
            continue;
        }
        for (int i = 0; i < cur->num_of_buckets; i++)
        {
            for (const struct flow_key_alternatives* _Opt e = cur->buckets[i]; e; e = e->next)
            {
                /* Skip sentinel keys that are not real objects */
                uintptr_t val = (uintptr_t)e->p_obj_key;
                if (val == UNINITIALIZED_VALUE || val == ANY_VALUE)
                {
                    continue;
                }
                object_set_add(&all_keys, e->p_obj_key);
            }
        }
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
                    "_Clear parameter '%s' is not zero at exit (see line %d)",
                    name_ss2.c_str ? name_ss2.c_str : param_name,
                    flow_alternative_line(p_alternative));
        ss_close(&name_ss2);        
    }
}

/*
   _Out requires EVERY member of the pointee -- not just _Owner ones -- to
   have been given a real value by every exit point: the same "definite
   assignment" obligation C#'s `out` enforces, for every field. A plain
   scalar member left uninitialized is just as much a violation as an
   untouched _Owner member; the caller is trusting that the whole object is
   now well-formed, not only its owned resources. flow_parameter_object_init
   seeds every member of a _Out pointee as UNINITIALIZED at function entry
   (see the _Out branch there); this is the check that verifies every one
   of them left that state behind.
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

    /* top_level (a bare scalar/pointer pointee, not a struct member) used
       to be skipped here: `*p = expr;` didn't update this object, so it
       always looked UNINITIALIZED. Fixed at the source (issue #471). */

    const struct flow_key_alternatives* _Opt e = flow_branch_search_up(ctx->p_current_flow_branch, p_obj);
    if (e == NULL || e->alternatives.size == 0)
    {
        /* No entry at all -- never written, still carries the UNINITIALIZED
           seed from function entry. */
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
                        "_Out parameter '%s' is possibly not initialized at exit (see line %d)",
                        name_ss2.c_str ? name_ss2.c_str : param_name,
                        flow_alternative_line(p_alternative));
            
            ss_close(&name_ss2);
        }
    }
}

/* A plain (non-_Dtor/_Out/_Owner) pointer parameter is a borrow: its _Owner members must still be live at every exit -- otherwise a member freed but not restored (e.g. on an early return) leaks silently with no diagnostic. Mirror of flow_check_ctor_object_is_initialized_at_exit, but checking consumption instead of assignment. */
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
        /* Same carve-out as flow_check_ctor_object_is_initialized_at_exit:
           a scalar/pointer pointee reached only through a bare `*p = ...`
           write is not yet reliably tracked back onto this exact arena
           object. Only members reached via `->` are checked. */
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

    bool consumed_reported = false;

    for (int i = 0; i < e->alternatives.size; i++)
    {
        const struct flow_alternative* p_alternative = e->alternatives.data[i];

        if (!consumed_reported &&
                (p_alternative->imaginary == FLOW_IMAGINARY_MOVED ||
                    p_alternative->imaginary == FLOW_IMAGINARY_ENDED))
        {
            consumed_reported = true;
            struct osstream name_ss = { 0 };
            flow_param_member_name_to_string(param_name, p_obj->member_designator, &name_ss);
            diagnostic(W_FLOW_PARAM_OWNER_CONSUMED_AT_EXIT,
                           ctx->ctx,
                           NULL,
                           marker,
                           "parameter '%s' was moved/released here (see line %d) but never reassigned -- only a _Dtor or _Owner parameter may leave the caller's object consumed",
                           name_ss.c_str ? name_ss.c_str : param_name,
                           flow_alternative_line(p_alternative));
            ss_close(&name_ss);
        }
    }
}

/* One shared pass over parameters at every exit for _Clear (every member == 0), _Out (every member assigned), and _Dtor (every _Owner member released -- the callee-side half of flow_check_object_init_assigment's caller-side lifetime-ending); _Dtor's obligation used to go unchecked entirely (dtor_is_opt.c). */
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

        /* A plain pointer (none of _Clear/_Dtor/_Out) is a borrow and now gets the same exit-point check as those -- except when the POINTER ITSELF is _Owner, which legitimately transfers the whole pointee and is already checked at its point of erasure; treating it as a borrow too double-reported the same finding. */
        const bool is_owner_param = type_is_owner(p_param_type);

        /* Find the concrete pointee: flow_parameter_object_init wired
           this parameter's own alternative to an arena object at
           function entry (see the "non-optional pointer" case there). */
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
                                          param_name);
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

static void flow_check_function_exit(struct flow_ctx* ctx, const struct jump_statement* p_jump_statement)
{
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
                                            p_throw_snapshot);
            
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
                make_object(ctx->p_return_type, &param_object, MAKE_STATE_UNITIALIZED, ctx->ctx->options.target);
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
                flow_branch_accumulate_into_join(ctx->p_break_join_map, ctx->p_current_flow_branch, NULL);
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
                                                  ctx->p_current_flow_branch, NULL);
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
                
                /* Eagerly snapshot state at this goto into the label's own map entries, the same way later jumps to it do via flow_branch_accumulate_into_join -- otherwise the label map starts empty and falls back to its parent chain, which keeps mutating in place and no longer reflects what existed at this goto by the time it's reached. */
                flow_branch_accumulate_into_join(p_label_map, ctx->p_current_flow_branch, NULL);

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
                       A label can be reached two ways: by a goto (already
                       folded into ctx->labels[i].p_flow_branch above) or by
                       simply falling through from the previous statement
                       (ctx->p_current_flow_branch, right now). Union the
                       fall-through arrival into the label's accumulated
                       state instead of discarding it by overwriting
                       ctx->p_current_flow_branch outright -- otherwise
                       whichever gotos arrived first would be silently lost
                       the moment control also reaches the label normally.
                    */
                    flow_branch_accumulate_into_join(ctx->labels[i].p_flow_branch, ctx->p_current_flow_branch, NULL);
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

            if (p_label->constant_expression == NULL)
            {
                /* A `default:` label still needs its own fresh child map, not ctx->p_initial_map directly -- reusing the pre-switch map corrupts it when the body ends with an unconditional jump, the same aliasing bug fixed for `&&` in flow_ensure_branch_pair. Concretely: `switch(e){...;default:break;} return 0;` falsely warned the return unreachable. */
                ctx->p_current_flow_branch =
                    flow_branch_arena_new(&ctx->flow_branch_arena, ctx->p_initial_map, FLOW_BRANCH_DEFAULT);
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

            /* ctx->p_switch_object_key is the object being switched on,
               set when entering the switch statement */
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

            ctx->p_current_flow_branch = p_case_map;
        }
    }
    catch
    {
    }
}

static void flow_visit_block_item(struct flow_ctx* ctx, struct block_item* p_block_item)
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

static void flow_visit_block_item_list(struct flow_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* _Opt p_block_item = p_block_item_list->head;

    /*
       Warn once per run of dead code. ctx->p_current_flow_branch is
       marked is_unreachable by flow_visit_jump_statement right when an
       unconditional return/break/continue/goto/throw is processed, so
       any block item visited afterward -- until a label makes the map
       live again via a goto arrival -- can never actually execute.
    */
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
            /* label arrival (goto and/or fall-through) made the map
               live again; a later jump can start a new dead-code run */
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
            flow_explain_alternative_not_true(&ss, alt);
            /* Use warning 67 (W_COMPILE_ASSERT_UNPROVEM), not flow1's error 1080: 1080 is an unconfigurable error id whose phase-0 check ran before flow analysis queued anything, so `//lint 1080` failed to suppress it. 67 is a configurable phase-2 warning, so `//lint 67` actually works. See logical-or-per-path.c. */
            diagnostic(W_COMPILE_ASSERT_UNPROVEM, ctx->ctx,
                       p_position_token, NULL, "compile_assert failed: %s", ss.c_str);
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
        /* _Assert(cond) narrows like assert: after it, only the branch
           where cond is true is reachable (a failed _Assert aborts).
           Visit the condition EVALUATED so it narrows, then keep only the true
           arm. Unlike the `assert` macro this is a keyword, so it is never
           stripped and narrows in every build, including release.
           This mirrors exactly what `if (!(cond)) return;` does in
           flow_visit_if_statement's no-else, "only the false arm reaches
           the end" case: merge_a_b(p_before, X, X) then continue in
           p_before -- so it relies on flow_branch_merge_arms's cleanup loop
           not over-freeing shared ancestor maps (see the ancestor-check
           there) rather than needing any special-casing here. */
        if (ctx->p_current_flow_branch == NULL)
            return;

        struct flow_branch* p_before = ctx->p_current_flow_branch;
        struct flow_branch_pair pair = flow_visit_full_expression(ctx, p_static_assertion->constant_expression);
        /* Same as flow_visit_if_statement: force pair.p_true into its own
           fresh child map when it aliases p_before (or any intermediate
           arm of the condition aliases the map it was branched from) --
           otherwise flow_branch_merge_arms's cleanup loop, walking from an
           arm that IS an ancestor of p_before back up looking for
           p_before, never finds it (p_before is below, not above) and
           keeps climbing past it, wiping real ancestor state other
           already-evaluated operands still depend on. */
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

    if (p_static_assertion->first_token->type == TK_KEYWORD_CAKE_STATIC_DEBUG ||
            p_static_assertion->first_token->type == TK_KEYWORD_CAKE_STATIC_DEBUG_EX)
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

    /* compile_assert's diagnostic is queued above, during flow analysis, so
       its `//lint N` has to be checked here at phase 2. parser.c checks the
       same token at phase 0, which is too early to see it. */
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
             The objective here is to avoid including the arguments
             of function pointers inside the scope.
             Sample
             void x_destroy(void (*f)(void * _Owner p))
             We add f but not p.
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
                                      const char* _Opt p_root_name_opt)
{
    /* A `_View` object views its members as non-owner: `_View struct X x`
       treats every _Owner member of x as a plain view, so those members are not
       owned here and must not be reported "owner not moved" at scope exit
       (expressions.c:2706, `_View struct initializer initializer`). Once any
       ancestor is _View the whole subtree is a view. */
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
                                      p_root_name_opt);
        }
        return;
    }

    const bool is_owner = type_is_owner(&p_obj->type) && !view;

    const struct flow_key_alternatives* _Opt e = flow_branch_search_up(ctx->p_current_flow_branch, p_obj);
    if (e == NULL) return;

        /* One report per object at this exit, not one per alternative: a leaked
       owner is a property of the object, and repeating it once per accumulated
       alternative only pads the output -- parser.c:6295 reported each of six
       owner members ten times over. */
    bool not_moved_reported = false;

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
                                          p_root_name_opt);
                continue;
            }
        }

        if (is_owner &&
                !not_moved_reported &&
                p_alternative->imaginary != FLOW_IMAGINARY_MOVED &&
                p_alternative->imaginary != FLOW_IMAGINARY_ENDED)
        {
            /* FLOW_IMAGINARY_ENDED is how a _Dtor call resolves an owner
               (flow_branch_set_object_lifetime_ended, called from
               flow_check_object_init_assigment for type_is_pointed_dtor
               parameters) -- just as legitimate a resolution as MOVED.
               Before this fix, passing an _Owner struct member to a
               _Dtor function (e.g. `hash_item_set_destroy(&item);`)
               correctly ended its lifetime (confirmed via static_debug:
               the alternative really was ANY/ENDED at this point) but
               this check still reported "owner object not moved",
               because it only ever recognized MOVED. Found via a
               user-reported false positive on real code. */
            if (flow_alternative_is_zero(p_alternative) ||
                    p_alternative->value_relation == FLOW_RELATION_UNINITIALIZED)
            {
                /* An owner that was never assigned a value owns nothing to leak -- UNINITIALIZED means 'never told to hold anything', not 'holds something unreleased'; only an actually-assigned value (real or ANY) is a possible leak. Matters for void-erasure member checks: `malloc` then `free` with untouched owner members is not a leak (owner-resource-074.c). */
            }
            else
            {
                not_moved_reported = true;
                const char* member_suffix = p_obj->member_designator ? p_obj->member_designator : "";
                /* member_designator is attached to the object, not the expression reaching it, so it reads as bare '.member' with no leading name -- combine it with p_root_name_opt (the original declarator's name, threaded through every recursive call) so the message reads as 'p->member', matching what the user would type to fix it. */
                char object_name_buf[256] = { 0 } ;
                const char* object_name;
                if (p_root_name_opt != NULL && p_root_name_opt[0] != 0 &&
                        strcmp(p_root_name_opt, member_suffix) != 0)
                {
                    /* member_suffix with no leading '.' names the ROOT object itself, not something reached through it -- covers a root object whose own name differs from the caller-supplied display name (e.g. the void-erasure call site rendering a full expression). Concatenating here would repeat the name twice ('p.p'). */
                    if (member_suffix[0] == '.')
                    {
                        snprintf(object_name_buf, sizeof(object_name_buf), "%s->%s", p_root_name_opt, member_suffix + 1);
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
                   W_FLOW_MISSING_DTOR (29), not W_FLOW_LIFETIME_ENDED (31).
                   This is the inverse condition of the rest of 31: those mean
                   "an object whose lifetime already ended was used", this means
                   "an owner whose lifetime never ended was leaked". Sharing one
                   id made `//lint 31` blind a file to both at once. flow1 already
                   reports this shape as 29 ("object referenced by owner '%s' was
                   not released").
                */
                /* Report the leak at the exit point, where the fix actually belongs, not at the declaration/origin -- one diagnostic instead of a separate exit-point note. Wording says 'lifetime ended' (matching W_FLOW_LIFETIME_ENDED), not 'not moved'/'goes out of scope', wrong for non-block exits. User-reported twice. */
                diagnostic(W_FLOW_MISSING_DTOR,
                           ctx->ctx,
                           p_exit_token,
                           NULL,
                           "'%s' lifetime ended while still owning its resource (acquired at line %d)",
                           object_name,
                           flow_alternative_line(p_alternative));
            }
        }

    }
}

static void flow_check_write_qualifier_placement(const struct flow_ctx* ctx,
                                                 const struct type* _Opt p_type,
                                                 const struct token* _Opt p_token)
{
    if (p_type == NULL || p_token == NULL)
        return;

    /* A function declarator's own type is a function type; its qualifiers
       belong to the return type, which is checked in its own right. */
    if (type_is_function(p_type))
        return;

    if (!type_is_pointer(p_type))
    {
        if (type_is_clear(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                       "_Clear must be used only at the pointed object");
        }
        else if (type_is_dtor(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                       "_Dtor must be used only at the pointed object");
        }
        else if (type_is_out(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                       "_Out must be used only at the pointed object");
        }
        else if (type_is_uninit(p_type))
        {
            diagnostic(C_ERROR_FLOW_WRITE_QUALIFIER_MUST_QUALIFY_POINTEE, ctx->ctx, p_token, NULL,
                       "_Uninitialized must be used only at the pointed object");
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

            /* _core: the body's trailing `//lint N` is checked below instead,
            after the fall-off-the-end exit diagnostics have been queued.
            See flow_visit_compound_statement_core. */
            flow_visit_compound_statement_core(ctx, p_declaration->function_body);

            if (!compound_statement_is_last_item_return(p_declaration->function_body))
            {
                flow_exit_block_visit_defer_list(ctx, &p_declaration->function_body->defer_list, p_declaration->function_body->last_token);
                /* flow_check_params_at_function_exit(ctx, p_declaration); */
                flow_check_arena_objects_at_function_exit(ctx);
                const struct marker marker =
                {
                    .p_token_begin = p_declaration->function_body->last_token,
                    .p_token_end = p_declaration->function_body->last_token
                };
                flow_check_file_scope_objects_at_function_exit(ctx);
                /* Falling off the end of the function is an exit point too --
                a _Clear/_Dtor parameter's contract must hold here just as
                much as at an explicit return
                (ctx->p_current_function_declaration is still set at this
                point, so this must run before it's cleared below).

                Must run BEFORE flow_defer_list_set_end_of_lifetime -- see
                the identical reordering (and the false-negative it fixes)
                in flow_check_function_exit, just above. */
                flow_check_write_qualified_params_at_exit(ctx, &marker, p_declaration->function_body->last_token);
                flow_defer_list_set_end_of_lifetime(ctx, &p_declaration->function_body->defer_list, p_declaration->function_body->last_token);
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
        flow_predicate_cache_reset(ctx);
        ctx->collect_deferred_effects = false;
        ctx->deferred_effects_count = 0;

        flow_allocated_object_arena_clear(&ctx->allocated_object_arena);
        flow_branch_arena_clear(&ctx->flow_branch_arena);

        ctx->p_current_flow_branch = flow_branch_arena_new(&ctx->flow_branch_arena, NULL, FLOW_BRANCH_ROOT);
        if (ctx->p_current_flow_branch == NULL)
            throw;

        flow_visit_declaration(ctx, p_declaration);

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
    flow_allocated_object_arena_clear(&ctx->allocated_object_arena);
    flow_branch_arena_clear(&ctx->flow_branch_arena);
    flow_alternatives_pool_shutdown();
}

