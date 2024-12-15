/*
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
    int indentation;
    int locals_count;
    int extern_count;

    int tag_name_count;
    struct hash_map tag_names;
    struct hash_map structs_map;
    struct hash_map function_map;

    struct osstream local_declarators;
    struct osstream add_this_before;
    struct osstream add_this_before_external_decl;
    bool is_local;
    struct osstream data_types;
    struct osstream function_types;
    bool zero_mem_used;
    _View struct ast ast;    
};

void d_visit(struct d_visit_ctx* ctx, struct osstream* oss);
void d_visit_ctx_destroy( struct d_visit_ctx* _Obj_owner ctx);
