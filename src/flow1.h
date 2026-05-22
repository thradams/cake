/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "parser.h"

struct flow1_visit_ctx;

enum flow1_state
{
    /*
       Not applicable. The state cannot be used.
       struct...
       TODO we need empty state when object does not exist
       and the merge it is not a problem
       merge with nothing then
    */
    FLOW1_OBJECT_STATE_NOT_APPLICABLE = 0,

    FLOW1_OBJECT_STATE_UNINITIALIZED = 1 << 0,

    /*
      The only reason we have null and zero is because
      of non pointer references -1 for instance can be the "null"
    */
    FLOW1_OBJECT_STATE_NULL = 1 << 1,
    FLOW1_OBJECT_STATE_NOT_NULL = 1 << 2,

    //means not-null moved at same time
    FLOW1_OBJECT_STATE_MOVED = 1 << 3,

    FLOW1_OBJECT_STATE_ZERO = 1 << 5,
    FLOW1_OBJECT_STATE_NOT_ZERO = 1 << 6,

    FLOW1_OBJECT_STATE_LIFE_TIME_ENDED = 1 << 7,

    FLOW1_OBJECT_STATE_DANGLING = 1 << 8
};


struct flow1_objects
{
    struct flow1_object* _Owner* _Owner _Opt data;
    int size;
    int capacity;
};

struct flow1_objects_view
{
    struct flow1_object** _Owner _Opt data;
    int size;
    int capacity;
};

struct flow1_object_state
{
    const char* dbg_name;
    int state_number;

    struct flow1_object* _Opt pointed;
    enum flow1_state state;
    struct flow1_objects_view alternatives;
    struct flow1_object_state* _Owner _Opt next;
};

/*
  Used in flow analysis to represent the object instance
*/
struct flow1_object
{
    //used to avoid infinite recursion
    unsigned int visit_number;

    struct flow1_object* _Opt parent;

    /*object are the result of expressions or they are declarators*/
    const struct declarator* _Opt p_declarator_origin;
    const struct expression* _Opt p_expression_origin;

    struct flow1_objects_view members;

    struct flow1_object_state current;

    int id; //helps debugging
    bool is_temporary;
};

struct flow1_label_state
{
    const char* label_name;
    int state_number;
};

struct flow1_visit_ctx
{
    struct secondary_block* _Opt catch_secondary_block_opt;

    struct parser_ctx* const ctx;

    struct type* _Opt p_return_type;
    int parameter_list;

    int state_number_generator;
    bool expression_is_not_evaluated; //true when is expression for sizeof, missing state_set, typeof
    bool inside_assert;
    bool inside_contract;

    /*avoid messages like always something, because in loop the same expression is visited in diferent states*/
    bool inside_loop;

    int throw_join_state; /*state where throws are joined*/
    int break_join_state; /*state where breaks are joined*/
    int initial_state;    /*used to keep the original state*/

    struct flow1_objects arena;

    struct flow1_label_state labels[100]; //max 100 labels in a function (case not included)
    int labels_size;
};

void flow1_visit_ctx_destroy(_Dtor struct flow1_visit_ctx* p);
void flow1_start_visit_declaration(struct flow1_visit_ctx* ctx, struct declaration* p_declaration);