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
    struct flow3_map** data;
    int size;
    int capacity;
};

struct flow3_allocated_object_arena
{
    struct object** data;
    int size;
    int capacity;
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
    const struct object* p_switch_obj_key;
    

    struct flow3_label_state labels[100]; //max 100 labels in a function (case not included)
    int labels_size;

    
    struct flow3_allocated_object_arena allocated_object_arena;
    struct flow3_map_arena flow3_map_arena;
    struct flow3_map* p_current_flow3_map;

    /*
     * Set while visiting a function body so that flow3_visit_jump_statement
     * can call flow3_check_params_at_function_exit on every return path.
     * NULL outside of a function body.
     */
    struct declaration* _Opt p_current_function_declaration;
};

void flow3_visit_ctx_destroy(_Dtor struct flow3_visit_ctx* p);
void flow3_start_visit_declaration(struct flow3_visit_ctx* ctx, struct declaration* p_declaration);
