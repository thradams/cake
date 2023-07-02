#pragma once
#include "parser.h"


struct defer_scope
{
    struct defer_statement* defer_statement; // defer 
    struct try_statement* p_try_statement; //try
    struct selection_statement* p_selection_statement2; //if swith
    struct iteration_statement* p_iteration_statement; //for do while
    struct statement* p_statement; //
    struct compound_statement* p_function_body;
    struct defer_scope* lastchild;

    struct defer_scope* previous;
};

struct owner visit_ctx
{
    /*
    * It is necessary two passes to generate lambdas expressions
    * because some types maybe needs to be "globalized"
    * is_second_pass is true if the compiler is at second pass
    */
    bool is_second_pass;
   
    bool has_lambda;

    bool is_inside_lambda;

    /*these indexes are used to generate unique names at file scope*/
    int capture_index;    
    int lambdas_index;
    
    struct token_list insert_before_declaration;
    struct token_list insert_before_block_item;
    struct ast ast;
    enum language_version target;
    struct defer_scope* tail_block;
};

void visit(struct visit_ctx* ctx);
void visit_ctx_destroy(implicit struct visit_ctx* obj_owner ctx);
