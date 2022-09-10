#pragma once
#include "parser.h"


struct defer_scope
{
    struct defer_statement* defer_statement; // defer 
    struct selection_statement* p_selection_statement2; //if swith try
    struct iteration_statement* p_iteration_statement; //for do while
    struct statement* p_statement; //
    struct compound_statement* p_function_body;
    struct defer_scope* lastchild;

    struct defer_scope* previous;
};

struct visit_ctx
{
    /*
    * Funcoes que tem lambdas dentro precisam de 2 passadas
    * uma para descobrir se tem alguem que precisa ser globalizado
    * e depois para renomar as referencias para o globalizado
    */
    int lambda_step;
    
    /*
    * true se precisa executar segundo passo
    */
    bool bHasLambda;
    bool bInsideLambda;
    bool bInsideDefer;
    bool bInsideCompoundStatement;

    int captureindex;
    int typeofindex;
    int lambdasindex;
    

    struct hash_map   instanciations_map;
    struct token_list insert_before_declaration;
    struct token_list insert_before_block_item;
    struct token_list instanciations;
    struct ast ast;
    enum language_version target;
    struct defer_scope* tail_block;
};

void visit(struct visit_ctx* ctx, struct error* error);
