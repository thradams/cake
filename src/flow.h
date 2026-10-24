/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "parser.h"
#include "flow_branch.h"
#include <time.h>

struct flow_label_state
{
    const char* label_name;
    struct flow_branch* p_flow_branch;
};

/*
    A pointee write-effect of a function call, deferred until after all
    arguments are evaluated (C evaluates arguments first, then the callee runs).
*/
enum flow_pointee_effect_kind
{
    FLOW_EFFECT_NONE = 0,             /* no write-effect on the pointee */
    FLOW_EFFECT_CLEAR,                /* _Clear: every member set to zero
                                           (+ reachable owner pointees ended) */
    FLOW_EFFECT_LIFETIME_ENDED,       /* plain _Owner parameter consuming the
                                           whole object: every member ended */
    FLOW_EFFECT_ANY,                  /* _Out, or a plain mutable pointer:
                                           possibly-modified/unknown */
    FLOW_EFFECT_DTOR,                 /* _Dtor: every member set to
                                           uninitialized (+ reachable owner
                                           pointees ended) */
    FLOW_EFFECT_REACHABLE_ANY         /* const pointer (const is shallow): the
                                           pointee is kept, what its pointer
                                           members point to becomes ANY
                                           (flow.md 6.3) */
};

struct flow_deferred_pointee_effect
{
    const struct object* _Opt pointee;
    enum flow_pointee_effect_kind kind;
    const struct token* _Opt p_token; /* where the call is, for the state it sets */

    /*
        The pointer value the pointee came from: its origin, the object whose
        values it is one of, and the argument. Effects with the same object and
        argument are alternatives (the pointer points to one of them) and are
        applied in pointer target arms; the others all happen.
    */
    const struct flow_branch* _Opt p_target_origin;
    const struct object* _Opt p_pointer_object;
    int argument_serial;
};

/* A pointee passed to a call that has a restrict parameter (see flow_check_restrict_arguments). */
struct flow_hypothesis;

struct flow_restrict_argument
{
    const struct object* _Opt pointee;
    int argument_serial;
    bool is_restrict; /* the parameter receiving it is restrict */
    bool is_owner; /* the parameter receiving it is _Owner */
    const struct expression* _Opt p_argument;
    const struct flow_hypothesis* _Opt p_hypothesis; /* the pointee is the argument's only under it */
};

#define FLOW_MAX_RESTRICT_ARGUMENTS 64


/* Entries in flow_ctx.p_reported_findings. */
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
/* "in case 'a' and 'b' point to the same object": the values of p_branch and its descendants exist only then */
struct flow_hypothesis
{
    const struct flow_branch* _Opt p_branch;
    bool is_parameter; /* reported as W_FLOW_PARAM_MAY_ALIAS */
    char text[160];
};

#define FLOW_MAX_HYPOTHESES 128

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
    const struct flow_hypothesis* _Opt p_hypothesis; /* the hypothesis of the alternatives, when all have one */
    bool has_plain; /* an alternative with no hypothesis supports it */
};

struct flow_ctx
{
    struct parser_ctx* const ctx;

    /*
        Arguments whose call may have changed what is reachable from them
        (flow.md 6.3). A state set there is explained as a call effect, not
        as "value set here". Reset for each function.
    */
    const struct expression* call_effect_arguments[64];
    int call_effect_arguments_count;

    struct type* _Opt p_return_type;
    int parameter_list;

    bool expression_is_not_evaluated; //true when is expression for sizeof, missing state_set, typeof

    /* the expression being visited as a condition (see flow_visit_condition) */
    const struct expression* _Opt p_condition;

    /* the expression whose value an _Owner destination takes: a ?: moves its owner arm only then */
    const struct expression* _Opt p_owner_sink;

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

    clock_t function_start_time; /* see flow_start_visit_declaration */
    bool stopped; /* the analysis of the current function was stopped, see flow_check_limits */

    struct flow_branch* _Opt p_throw_join_map;  /*map where throws are joined*/
    bool throw_join_reached;    /* a throw reached p_throw_join_map */
    bool break_join_reached;    /* a break reached p_break_join_map */
    bool continue_join_reached; /* a continue reached p_continue_join_map */
    struct flow_branch* _Opt p_break_join_map;  /*map where breaks are joined*/
    struct flow_branch* _Opt p_continue_join_map;  /*map where continues are joined*/
    struct flow_branch* _Opt p_initial_map;     /*map snapshot of the original state*/
    const struct object* _Opt p_switch_obj_key;
    bool switch_is_constant;                       /* the switch condition is known at compile time */
    const struct label* _Opt p_switch_live_label;  /* the label a constant switch jumps to */
    bool switch_case_dead;                         /* the current case of a constant switch is never reached */
    

    struct flow_label_state labels[100]; //max 100 labels in a function (case not included)
    int labels_size;


    /*
        Function-call pointee write-effects, deferred until after all arguments
        are evaluated (see flow_visit_function_arguments).
    */
    struct flow_deferred_pointee_effect deferred_effects[64];
    int deferred_effects_count;
    bool collect_deferred_effects;
    int argument_serial; /* the argument being checked, see flow_deferred_pointee_effect */

    /* the call whose arguments are checked has a restrict parameter: its arguments' pointees are recorded */
    bool call_has_restrict;
    struct flow_restrict_argument restrict_arguments[FLOW_MAX_RESTRICT_ARGUMENTS];
    int restrict_arguments_count;

    struct flow_allocated_object_arena allocated_object_arena;
    struct flow_branch_arena flow_branch_arena;
    struct flow_branch* _Opt p_current_flow_branch;

    /*
        Findings accumulated or already reported, so one fact is reported once
        (see flow_finding_record); cleared per assignment.
    */
    struct flow_reported_finding* _Owner _Opt p_reported_findings; /* FLOW_MAX_REPORTED_FINDINGS entries, allocated on first use */
    int findings_depth; /* nested assignment checks sharing p_reported_findings */
    struct osstream findings_text; /* pending messages, each ending in '\0' */

    /*
     * Set while visiting a function body so that flow_visit_jump_statement
     * can call flow_check_params_at_function_exit on every return path.
     * NULL outside of a function body.
     */
    struct declaration* _Opt p_current_function_declaration;

    /* pointees of the restrict parameters of the function: the result cannot point into them */
    const struct object* _Opt restrict_pointees[16];
    const struct declarator* _Opt restrict_parameters[16];
    int restrict_pointees_count;

    /* hypotheses of the function, see struct flow_hypothesis */
    struct flow_hypothesis hypotheses[FLOW_MAX_HYPOTHESES];
    int hypotheses_count;

    /*
     * Set by the `*` and `->` "dereference of an ended pointee"
     * checks right after they report one, so flow_check_object_init_assigment
     * -- called right afterward when that same dereference is ALSO used as
     * an assignment/return/argument source -- can tell it was already
     * reported and skip its own, otherwise-duplicate, report of the exact
     * same fact. NULL/0 when nothing is pending consumption. See the
     * comment above the EXPR_UNARY_CONTENT lifetime check itself.
     */
    const struct object* _Opt p_pending_ended_report_obj;

    /* origin of the REF being followed as an assignment source; its values must be compatible */
    const struct flow_branch* _Opt p_source_ref_origin;

    /* inside `a = b`: an unknown b overwrites the old value of a with ANY */
    bool assignment_overwrites;

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
