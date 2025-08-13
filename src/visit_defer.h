/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake 
 *
 *  The objective of this visit is to build the "defer list" on AST
 *  The defer list is the list of items that will go out of scope.
 *  Each item can point to a declarator or defer.
 *  It is complicated algorithm we make it ready to use on AST
*/

#pragma once
#include "expressions.h"
#include "parser.h"

struct defer_visit_ctx
{
    struct secondary_block* _Opt catch_secondary_block_opt;
    struct parser_ctx *ctx;
    _View struct ast ast;    
    struct defer_defer_scope* _Owner _Opt tail_block;
    int parameter_list;
};

void defer_visit_ctx_destroy(_Dtor struct defer_visit_ctx* p);
void defer_start_visit_declaration(struct defer_visit_ctx* ctx, struct declaration* p_declaration);
