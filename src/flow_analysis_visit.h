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

struct flow_analysis_visit_ctx
{
    struct parser_ctx *ctx;
    struct ast ast;
    enum language_version target;
    struct flow_defer_scope* tail_block;
};

void flow_analysis_visit(struct flow_analysis_visit_ctx* ctx);
void flow_analysis_visit_declaration(struct flow_analysis_visit_ctx* ctx, struct declaration* p_declaration);