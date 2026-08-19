/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "parser.h"


struct flow_label_state
{
    const char* label_name;
    struct flow_map* p_flow_map;
};

struct flow_map_arena
{
    struct flow_map* _Owner _Opt * _Owner _Opt  data;
    int size;
    int capacity;
};

struct flow_allocated_object_arena
{
    struct object*  _Owner _Opt * _Owner _Opt data;
    int size;
    int capacity;
};

/* One recorded comparison predicate (see same-predicate branch correlation in
   flow3.c): a comparison of the same operands controlling more than one branch
   reuses the branch id first assigned to it. */
struct flow_predicate_entry
{
    enum expression_type op;
    const struct object* _Opt left_obj;
    const struct object* _Opt right_obj;   /* NULL when the right side is a constant */
    long long right_const;
    int branch_id;
};

/* A pointee write-effect of a function call, deferred until after all
   arguments are evaluated (C evaluates arguments first, then the callee runs). */
enum flow_pointee_effect_kind
{
    FLOW_EFFECT_NONE = 0,             /* no write-effect on the pointee */
    FLOW_EFFECT_CLEAR,                /* _Clear: every member set to zero
                                           (+ reachable owner pointees ended) */
    FLOW_EFFECT_LIFETIME_ENDED,       /* plain _Owner parameter consuming the
                                           whole object: every member ended */
    FLOW_EFFECT_ANY,                  /* _Out, or a plain mutable pointer:
                                           possibly-modified/unknown */
    FLOW_EFFECT_DTOR                  /* _Dtor: every member set to
                                           uninitialized (+ reachable owner
                                           pointees ended) */
};

struct flow_deferred_pointee_effect
{
    const struct object* _Opt pointee;
    enum flow_pointee_effect_kind kind;
    const struct token* _Opt p_token; /* where the call is, for the state it sets */
};

struct flow_visit_ctx
{
    struct parser_ctx* const ctx;

    struct type* _Opt p_return_type;
    int parameter_list;

    bool expression_is_not_evaluated; //true when is expression for sizeof, missing state_set, typeof

    /*avoid messages like always something, because in loop the same expression is visited in diferent states*/
    bool inside_loop;

    struct flow_map* _Opt p_throw_join_map;  /*map where throws are joined*/
    struct flow_map* _Opt p_break_join_map;  /*map where breaks are joined*/
    struct flow_map* _Opt p_initial_map;     /*map snapshot of the original state*/
    const struct object* _Opt p_switch_obj_key;
    

    struct flow_label_state labels[100]; //max 100 labels in a function (case not included)
    int labels_size;

    /* Same-predicate branch correlation cache; reset per function. */
    struct flow_predicate_entry predicate_cache[128];
    int predicate_cache_size;

    /* Function-call pointee write-effects, deferred until after all arguments
       are evaluated (see flow_visit_function_arguments). */
    struct flow_deferred_pointee_effect deferred_effects[64];
    int deferred_effects_count;
    bool collect_deferred_effects;

    struct flow_allocated_object_arena allocated_object_arena;
    struct flow_map_arena flow_map_arena;
    struct flow_map* _Opt p_current_flow_map;

    /*
     * Set while visiting a function body so that flow_visit_jump_statement
     * can call flow_check_params_at_function_exit on every return path.
     * NULL outside of a function body.
     */
    struct declaration* _Opt p_current_function_declaration;

    /*
     * Set by EXPR_UNARY_CONTENT's own "dereference of an ended pointee"
     * check right after it reports one, so flow_check_object_init_assigment
     * -- called right afterward when that same dereference is ALSO used as
     * an assignment/return/argument source -- can tell it was already
     * reported and skip its own, otherwise-duplicate, report of the exact
     * same fact. NULL/0 when nothing is pending consumption. See the
     * comment above the EXPR_UNARY_CONTENT lifetime check itself.
     */
    const struct object* _Opt p_pending_ended_report_obj;
    int pending_ended_report_line;
};

void flow_visit_ctx_destroy(_Dtor struct flow_visit_ctx* p);
void flow_start_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration);
