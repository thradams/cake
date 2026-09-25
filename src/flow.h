/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "parser.h"
#include "flow_branch.h"

struct flow_label_state
{
    const char* label_name;
    struct flow_branch* p_flow_branch;
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

    /* The pointer value the pointee came from: its origin, the object whose
       values it is one of, and the argument. Effects with the same object and
       argument are alternatives (the pointer points to one of them) and are
       applied in pointer target arms; the others all happen. */
    const struct flow_branch* _Opt p_target_origin;
    const struct object* _Opt p_pointer_object;
    int argument_serial;
};

/* Entries in flow_ctx.reported_findings. */
#define FLOW_MAX_REPORTED_FINDINGS 256

/* Branches explained for one finding; the others are only counted. */
#define FLOW_FINDING_MAX_SAMPLES 3

/* Where one alternative supporting a finding came from (see flow_explain_alternative). */
struct flow_finding_sample
{
    const struct token* _Opt p_origin_token;
    const struct flow_branch* _Opt p_origin_map;
    const struct flow_branch* _Opt p_path_map; /* the branch path explained */
};

/*
   One fact (object, line, diagnostic), accumulated across every alternative
   and every path that supports it and reported once by flow_findings_flush:
   "'p' may be null (+2 branches)", explained from a few samples only.
   An entry with no pending message only deduplicates
   (flow_finding_already_reported), and so does an entry already flushed.
*/
struct flow_reported_finding
{
    const struct object* _Opt p_object; /* NULL marks the end of the used entries */
    int line;
    int diagnostic_id;

    int message_offset; /* pending report in flow_ctx.findings_text; -1 when none */
    struct marker marker;
    int count; /* alternatives supporting the finding */
    int sample_count;
    struct flow_finding_sample samples[FLOW_FINDING_MAX_SAMPLES];
};

struct flow_ctx
{
    struct parser_ctx* const ctx;

    struct type* _Opt p_return_type;
    int parameter_list;

    bool expression_is_not_evaluated; //true when is expression for sizeof, missing state_set, typeof

    /* the expression being visited as a condition (see flow_visit_condition) */
    const struct expression* _Opt p_condition;

    /*
       Which pass over the innermost enclosing loop body is running:
       0 outside any loop (so a zero-initialized context starts out correct),
       1 while visiting it the first time, 2 the second, and so on. Restored
       to the enclosing loop's value when a nested loop finishes.

       Diagnostics that claim a value is settled ("condition is always true")
       must stay quiet whenever this is not 0: the same expression is visited
       in one iteration's state, and the next iteration can contradict it.
    */
    int iteration_pass;

    struct flow_branch* _Opt p_throw_join_map;  /*map where throws are joined*/
    struct flow_branch* _Opt p_break_join_map;  /*map where breaks are joined*/
    struct flow_branch* _Opt p_continue_join_map;  /*map where continues are joined*/
    struct flow_branch* _Opt p_initial_map;     /*map snapshot of the original state*/
    const struct object* _Opt p_switch_obj_key;
    

    struct flow_label_state labels[100]; //max 100 labels in a function (case not included)
    int labels_size;


    /* Function-call pointee write-effects, deferred until after all arguments
       are evaluated (see flow_visit_function_arguments). */
    struct flow_deferred_pointee_effect deferred_effects[64];
    int deferred_effects_count;
    bool collect_deferred_effects;
    int argument_serial; /* the argument being checked, see flow_deferred_pointee_effect */

    struct flow_allocated_object_arena allocated_object_arena;
    struct flow_branch_arena flow_branch_arena;
    struct flow_branch* _Opt p_current_flow_branch;

    /* Findings accumulated or already reported, so one fact is reported once
       (see flow_finding_record); cleared per assignment. */
    struct flow_reported_finding reported_findings[FLOW_MAX_REPORTED_FINDINGS];
    int findings_depth; /* nested assignment checks sharing reported_findings */
    struct osstream findings_text; /* pending messages, each ending in '\0' */

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

    /*
     * The subscript expression an assignment, compound assignment or ++/--
     * is about to write, set while its left operand is visited. The subscript
     * visit invalidates the array for an unknown index while the REF
     * alternatives of its own operand are still in the map.
     */
    const struct expression* _Opt p_subscript_being_written;
    int pending_ended_report_line;
};

void flow_visit_ctx_destroy(_Dtor struct flow_ctx* p);
void flow_start_visit_declaration(struct flow_ctx* ctx, struct declaration* p_declaration);
