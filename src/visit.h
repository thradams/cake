#pragma once
#include "parser.h"
#include "ownership.h"

struct defer_scope
{
    struct defer_statement* defer_statement; // defer 
    struct try_statement* p_try_statement; //try
    struct selection_statement* p_selection_statement2; //if swith
    struct iteration_statement* p_iteration_statement; //for do while
    struct statement* p_statement; //
    struct compound_statement* p_function_body;
    struct defer_scope* owner lastchild;

    struct defer_scope* owner previous;
};
void defer_scope_delete(struct defer_scope * owner opt p);
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
    view struct ast ast;
    enum language_version target;
    struct defer_scope* owner tail_block;
};

void visit(struct visit_ctx* ctx);
void visit_ctx_destroy( struct visit_ctx* obj_owner ctx);
