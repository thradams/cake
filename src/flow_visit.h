#pragma once
#include "parser.h"
#include "flow_object.h"

/*
  To be able to do static analysis with goto jump, we
  need to see full function AST because this affects for 
  instance which scopes we are leaving.
  Because static analysis may change the state we also want
  to check static_assert here.. so better to move all checks to here.
  nullchecks and imaginary flags.
*/

struct label_state
{
    const char * label_name;
    int state_number;
};

struct flow_visit_ctx
{
    struct secondary_block* catch_secondary_block_opt;

    struct parser_ctx *ctx;
    view struct ast ast;    
    struct flow_defer_scope* owner tail_block;
    struct type* view p_return_type;
    int parameter_list;
    
    int state_number_generator;
    bool expression_is_not_evaluated; //true when is expression for sizeof, missing state_set, typeof
    
    int throw_join_state; /*state where throws are joined*/
    int break_join_state; /*state where breaks are joined*/
    int initial_state;    /*used to keep the original state*/

    struct objects arena;

    struct label_state labels[100]; //max 100 labels in a function (case not included)
    int labels_size;
};


void flow_visit_ctx_destroy(struct flow_visit_ctx* obj_owner p);

void flow_start_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration);
