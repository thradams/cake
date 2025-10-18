/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 *  This visit generates a new and preprocessed C89 code from the AST
 */
 
#pragma once
#include "parser.h"
#include "ownership.h"
#include "osstream.h"
#include "hashmap.h"

struct d_visit_ctx
{
    struct options options;
    int indentation;
    bool print_qualifiers;

    /*
       File scope counter to generate unique names
    */
    unsigned int cake_file_scope_declarator_number;

    /*
       This counter is reset in each function
    */
    unsigned int cake_local_declarator_number;

    
    struct hash_map tag_names;
    struct hash_map structs_map;
    struct hash_map file_scope_declarator_map;
    struct hash_map instantiated_function_literals;
    
    struct osstream block_scope_declarators;
    struct osstream add_this_before;
    struct osstream add_this_before_external_decl;
    struct osstream add_this_after_external_decl;
    bool is_local;
    
    bool zero_mem_used;
    bool memcpy_used;

    bool define_nullptr;
    bool null_pointer_constant_used;

    bool address_of_argument;
        
    /*
    * Points to the function we're in. Or null in file scope.
    */
    const struct declarator* _Opt p_current_function_opt;

    struct break_reference
    {
      struct selection_statement * _Opt p_selection_statement;
      struct iteration_statement * _Opt p_iteration_statement;
    } break_reference;

    bool is__func__predefined_identifier_added;

    _View struct ast ast;    
};

void d_visit(struct d_visit_ctx* ctx, struct osstream* oss);
void d_visit_ctx_destroy( _Dtor struct d_visit_ctx* ctx);
