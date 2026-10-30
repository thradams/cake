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
    struct secondary_block* catch_secondary_block_opt;
    struct parser_ctx * _Nonnull const  ctx;
    _View struct ast ast;    
    struct defer_scope* _Owner tail_block;
    int parameter_list;

        
    /* special visit to find labels-- */
    bool searching_label_mode;    
    const char* label_name;
    struct label* p_label;
    /*-------------------------------*/

    struct declaration* p_declaration;

    /* body of a function literal (it has no declaration) */
    struct compound_statement* p_function_literal_body;
};

void defer_visit_ctx_destroy(_Dtor struct defer_visit_ctx* p);
void defer_start_visit_declaration(struct defer_visit_ctx* ctx, struct declaration* p_declaration);
void defer_start_visit_compound_statement(struct defer_visit_ctx* ctx,
                                           struct compound_statement* p_compound_statement,
                                           struct parameter_list* _Nullable p_parameter_list);
