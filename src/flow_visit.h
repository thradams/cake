#pragma once
#include "parser.h"

/*
  To be able to do static analysis with goto jump, we
  need to see full function AST because this affects for 
  instance which scopes we are leaving.
  Because static analysis may change the state we also want
  to check static_assert here.. so better to move all checks to here.
  nullchecks and imaginary flags.
*/

struct flow_visit_ctx
{
    struct parser_ctx *ctx;
    view struct ast ast;    
    struct flow_defer_scope* owner tail_block;
    struct type* view p_return_type;
    int parameter_list;
    struct jump_statement* view p_last_jump_statement;

    bool is_left_expression; //a = b
};


void flow_visit_ctx_destroy(struct flow_visit_ctx* obj_owner p);

void flow_visit_function(struct flow_visit_ctx* ctx, struct declaration* p_declaration);
