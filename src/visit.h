/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 *
 *  This visit "edit" the original tokens hidden/editing/adding some new 
 *  tokens to convert from one C++ version to another.
 *
*/

#pragma once
#include "parser.h"
#include "ownership.h"


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
  
};

void visit(struct visit_ctx* ctx);
void visit_ctx_destroy( struct visit_ctx* _Obj_owner ctx);
