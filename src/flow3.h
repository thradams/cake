/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "parser.h"


struct flow3_label_state
{
    const char* label_name;
    struct flow3_map* p_flow3_map;
};

struct flow3_map_arena
{
    struct flow3_map* _Owner _Opt * _Owner _Opt  data;
    int size;
    int capacity;
};

struct flow3_allocated_object_arena
{
    struct object*  _Owner _Opt * _Owner _Opt data;
    int size;
    int capacity;
};

/* One recorded comparison predicate (see same-predicate branch correlation in
   flow3.c): a comparison of the same operands controlling more than one branch
   reuses the branch id first assigned to it. */
struct flow3_predicate_entry
{
    enum expression_type op;
    const struct object* _Opt left_obj;
    const struct object* _Opt right_obj;   /* NULL when the right side is a constant */
    long long right_const;
    int branch_id;
};

/* A pointee write-effect of a function call, deferred until after all
   arguments are evaluated (C evaluates arguments first, then the callee runs). */
enum flow3_pointee_effect_kind
{
    FLOW3_EFFECT_NONE = 0,             /* no write-effect on the pointee */
    FLOW3_EFFECT_CLEAR,                /* _Clear: every member set to zero
                                           (+ reachable owner pointees ended) */
    FLOW3_EFFECT_LIFETIME_ENDED,       /* plain _Owner parameter consuming the
                                           whole object: every member ended */
    FLOW3_EFFECT_ANY,                  /* _Ctor, or a plain mutable pointer:
                                           possibly-modified/unknown */
    FLOW3_EFFECT_DTOR                  /* _Dtor: every member set to
                                           uninitialized (+ reachable owner
                                           pointees ended) */
};

struct flow3_deferred_pointee_effect
{
    const struct object* _Opt pointee;
    enum flow3_pointee_effect_kind kind;
    int line;
};

struct flow3_visit_ctx
{
    struct parser_ctx* const ctx;

    struct type* _Opt p_return_type;
    int parameter_list;

    bool expression_is_not_evaluated; //true when is expression for sizeof, missing state_set, typeof

    /*avoid messages like always something, because in loop the same expression is visited in diferent states*/
    bool inside_loop;

    struct flow3_map* _Opt p_throw_join_map;  /*map where throws are joined*/
    struct flow3_map* _Opt p_break_join_map;  /*map where breaks are joined*/
    struct flow3_map* _Opt p_initial_map;     /*map snapshot of the original state*/
    const struct object* _Opt p_switch_obj_key;
    

    struct flow3_label_state labels[100]; //max 100 labels in a function (case not included)
    int labels_size;

    /* Same-predicate branch correlation cache; reset per function. */
    struct flow3_predicate_entry predicate_cache[128];
    int predicate_cache_size;

    /* Function-call pointee write-effects, deferred until after all arguments
       are evaluated (see flow3_visit_function_arguments). */
    struct flow3_deferred_pointee_effect deferred_effects[64];
    int deferred_effects_count;
    bool collect_deferred_effects;

    struct flow3_allocated_object_arena allocated_object_arena;
    struct flow3_map_arena flow3_map_arena;
    struct flow3_map* _Opt p_current_flow3_map;

    /*
     * Set while visiting a function body so that flow3_visit_jump_statement
     * can call flow3_check_params_at_function_exit on every return path.
     * NULL outside of a function body.
     */
    struct declaration* _Opt p_current_function_declaration;

    /*
     * Set by EXPR_UNARY_CONTENT's own "dereference of an ended pointee"
     * check right after it reports one, so flow3_check_object_init_assigment
     * -- called right afterward when that same dereference is ALSO used as
     * an assignment/return/argument source -- can tell it was already
     * reported and skip its own, otherwise-duplicate, report of the exact
     * same fact. NULL/0 when nothing is pending consumption. See the
     * comment above the EXPR_UNARY_CONTENT lifetime check itself.
     */
    const struct object* _Opt p_pending_ended_report_obj;
    int pending_ended_report_line;
};

void flow3_visit_ctx_destroy(_Dtor struct flow3_visit_ctx* p);
void flow3_start_visit_declaration(struct flow3_visit_ctx* ctx, struct declaration* p_declaration);
