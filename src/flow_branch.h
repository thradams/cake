/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once

#include "flow_alternative.h"
#include "osstream.h"

struct flow_key_alternatives
{
    const struct object* p_obj_key; /*key*/
    struct flow_alternatives alternatives;
    struct flow_key_alternatives* _Opt _Owner next;
};

enum flow_branch_kind
{
    FLOW_BRANCH_ROOT,
    FLOW_BRANCH_TRUE_BRANCH,
    FLOW_BRANCH_FALSE_BRANCH,
    FLOW_BRANCH_BREAK_JOIN,
    FLOW_BRANCH_THROW_JOIN,
    FLOW_BRANCH_TRY_BRANCH,
    FLOW_BRANCH_CATCH_BRANCH,
    FLOW_BRANCH_SWITCH_BODY,
    FLOW_BRANCH_OPT_NULL,
    FLOW_BRANCH_OPT_NONNULL,
    FLOW_BRANCH_OPT_MEM_NULL,
    FLOW_BRANCH_OPT_MEM_NONNULL,
    FLOW_BRANCH_CALL_OPT_NULL,
    FLOW_BRANCH_CALL_OPT_NONNULL,
    FLOW_BRANCH_DO_WHILE_BODY_DIAG,
    FLOW_BRANCH_DO_WHILE_FALSE,
    FLOW_BRANCH_FOR_BODY_DIAG,
    FLOW_BRANCH_FOR_BODY_PASS1,
    FLOW_BRANCH_DEFAULT,
    FLOW_BRANCH_CASE,
    FLOW_BRANCH_GOTO_LABEL,
    FLOW_BRANCH_REL_DEAD,
    FLOW_BRANCH_EQ_DEAD,
    FLOW_BRANCH_NOT_DEAD_TRUE,
    FLOW_BRANCH_NOT_DEAD_FALSE,
    FLOW_BRANCH_OR_DEAD_TRUE,
    FLOW_BRANCH_OR_DEAD_FALSE,
    FLOW_BRANCH_AND_DEAD_TRUE,
    FLOW_BRANCH_AND_DEAD_FALSE,
    FLOW_BRANCH_MERGE_TEMP, /* short-lived local scratch map used only inside flow_branch_merge_arms; never printed */
    FLOW_BRANCH_POINTER_TARGET, /* the paths where a pointer with several targets points to one of them */
};

/* A branch side known to hold: the map is on side `kind` of the pair tagged branch_id. */
struct flow_branch_fact
{
    int branch_id;
    enum flow_branch_kind kind;
};

struct flow_branch_implied_facts
{
    struct flow_branch_fact data[8];
    int count;
};

struct flow_branch
{
    struct flow_key_alternatives* _Owner _Opt* _Owner _Opt buckets;
    int num_of_buckets;
    int num_of_entries;

    struct flow_branch* _Opt p_parent_map;
    enum flow_branch_kind kind;
    const struct expression* _Opt p_branch_expr;
    bool is_unreachable;
    int branch_id;
    int child_count;

    /*
      Sides of earlier branch pairs this map is known to be on without being
      under them: a condition whose narrowing kept only values from the true
      side of an earlier `if` can only run after that true side. Checked by
      flow_origins_compatible like the kinds on the parent chain. Full means
      less precision, never a wrong answer.
    */
    struct flow_branch_implied_facts implied_facts;
};

struct flow_branch_arena
{
    struct flow_branch* _Owner _Opt * _Owner _Opt  data;
    int size;
    int capacity;
};

struct flow_allocated_object_arena
{
    struct object*  _Owner _Opt * _Owner _Opt data;
    int size;
    int capacity;
};

void flow_branch_arena_clear(_Clear struct flow_branch_arena* a);
struct flow_branch* _Opt flow_branch_arena_new(struct flow_branch_arena* a, struct flow_branch* _Opt parent, enum flow_branch_kind kind);
struct flow_branch* _Opt flow_branch_arena_new_dead(struct flow_branch_arena* a, struct flow_branch* parent, enum flow_branch_kind kind);

void flow_allocated_object_arena_clear(_Clear struct flow_allocated_object_arena* a);
struct object* _Opt flow_allocated_object_arena_new(struct flow_allocated_object_arena* a);

struct flow_key_alternatives* _Opt flow_branch_search_up(struct flow_branch* _Opt m, const struct object* obj);
struct flow_key_alternatives* _Opt flow_branch_find_add(struct flow_branch* _Opt m, const struct object* obj);
void flow_branch_remove(struct flow_branch* _Opt m, const struct object* obj);

void flow_branch_set_object_moved(struct flow_branch* _Opt m, const struct object* obj, const struct token* _Opt p_token);
void flow_branch_set_object_zero(struct flow_branch* _Opt m, const struct object* obj, const struct token* _Opt p_token);
void flow_branch_set_object_uninitialized(struct flow_branch* _Opt m, const struct object* obj, const struct token* _Opt p_token);
void flow_branch_set_object_any_n(struct flow_branch* _Opt m, const struct object* obj, const struct token* _Opt p_token, bool nullable_enabled);
void flow_branch_set_object_lifetime_ended(struct flow_branch* m, const struct object* obj, const struct token* _Opt p_token);
void flow_branch_apply_dtor_or_clear_effect(struct flow_branch* _Opt m, const struct object* obj, bool is_clear, const struct token* _Opt p_token);

bool flow_branch_arm_has_entries(const struct flow_branch* arm, const struct flow_branch* parent);
void flow_branch_merge_arms(struct flow_branch* parent, const struct flow_branch* const arms[], int num_arms);
void flow_branch_merge_a_b(struct flow_branch* parent, const struct flow_branch* a, const struct flow_branch* b);
void flow_branch_accumulate_into_join(struct flow_branch* p_join, struct flow_branch* _Opt p_src,
                                    const struct flow_branch* _Opt p_retag_origin);

void flow_branch_name_to_string(const struct flow_branch* _Opt map, struct osstream* ss);
void flow_branch_debug_print(const struct flow_branch* _Opt map, int indent);
int flow_alternative_truth(struct flow_branch* _Opt map, const struct flow_alternative* alt, int depth);
int flow_object_truth(struct flow_branch* _Opt map, const struct object* p_object, int depth);
void flow_narrow_map_into(struct flow_branch* p_dest, struct flow_branch* _Opt p_before, const struct object* p_obj_key, bool true_branch, const struct token* _Opt p_token);

struct flow_branch_pair
{
    struct flow_branch* _Opt p_true;
    struct flow_branch* _Opt p_false;
};

struct osstream flow_explain_origin(const struct flow_branch* _Opt map);

struct flow_branch* _Opt flow_branch_arena_new_branch(struct flow_branch_arena* a, struct flow_branch* _Opt parent, bool is_true, const struct expression* _Opt p_expr);
struct flow_branch* _Opt flow_narrow_map_branch(struct flow_branch_arena* arena, struct flow_branch* _Opt p_before, const struct object* p_obj_key, bool true_branch, const struct expression* _Opt p_expr, const struct token* _Opt p_token);
const struct flow_branch* _Opt flow_alternative_narrowed_provenance(const struct flow_alternative* alt, const struct flow_branch* _Opt new_origin);
void flow_tag_branch_pair(struct flow_branch* _Opt p_true, struct flow_branch* _Opt p_false);



struct object_set
{
    const struct object** _Owner _Opt items;
    int size;
    int capacity;

    /* Open-addressing hash index (pointer-keyed, NULL = empty slot) used only
       to make membership checks O(1) average -- `items` above stays the dense,
       insertion-ordered array every caller already iterates over; this table
       exists purely so object_set_add doesn't have to linearly rescan it.
       See the comment on object_set_add for why this exists. */
    const struct object* _Opt* _Owner _Opt table;
    int table_capacity;
};

void object_set_add(struct object_set* l, const struct object* obj);
void object_set_destroy(_Dtor struct object_set* l);


