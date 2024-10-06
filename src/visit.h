/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "parser.h"
#include "ownership.h"

struct defer_scope
{
    struct defer_statement* _Opt defer_statement; // defer 
    struct try_statement* _Opt p_try_statement; //try
    struct selection_statement* _Opt p_selection_statement; //if swith
    struct iteration_statement* _Opt p_iteration_statement; //for do while
    struct statement* _Opt p_statement; //
    struct compound_statement* _Opt p_function_body;
    
    struct defer_scope* _Owner _Opt lastchild;
    struct defer_scope* _Owner _Opt previous;
};

struct visit_ctx
{
    /*
    * It is necessary two passes to generate lambdas expressions
    * because some types maybe needs to be "globalized"
    * is_second_pass is true if the compiler is at second pass
    */
    bool is_second_pass;
   
    bool has_lambda;

    bool is_inside_lambda;
    bool hide_non_used_declarations;

    /*these indexes are used to generate unique names at file scope*/
    int capture_index;    
    int lambdas_index;
    
    struct token_list insert_before_declaration;
    struct token_list insert_before_block_item;
    _View struct ast ast;
    enum language_version target;
    struct defer_scope* _Owner _Opt tail_block;
};

void visit(struct visit_ctx* ctx);
void visit_ctx_destroy( struct visit_ctx* _Obj_owner ctx);
