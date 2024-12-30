/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "parser.h"
#include "object_flow.h"

struct label_state
{
    const char * label_name;
    int state_number;
};

struct flow_visit_ctx
{
    struct secondary_block* _Opt catch_secondary_block_opt;

    struct parser_ctx *ctx;
    _View struct ast ast;    
 
    struct type* _Opt p_return_type;
    int parameter_list;
    
    int state_number_generator;
    bool expression_is_not_evaluated; //true when is expression for sizeof, missing state_set, typeof
    bool inside_assert;
    bool inside_contract;

    int throw_join_state; /*state where throws are joined*/
    int break_join_state; /*state where breaks are joined*/
    int initial_state;    /*used to keep the original state*/

    struct flow_objects arena;

    struct label_state labels[100]; //max 100 labels in a function (case not included)
    int labels_size;
};

struct flow_object* _Opt arena_new_object(struct flow_visit_ctx* ctx);

void flow_visit_ctx_destroy(struct flow_visit_ctx* _Obj_owner p);

void flow_start_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration);
void print_arena(struct flow_visit_ctx* ctx);
