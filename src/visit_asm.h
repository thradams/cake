/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 *  This visit generates x86-64 assembly (GAS/AT&T syntax) from the AST
 */

#pragma once
#include "parser.h"
#include "ownership.h"
#include "osstream.h"
#include "hashmap.h"

/*
 * Calling convention identifiers
 */
enum asm_calling_convention
{
    ASM_CONV_SYSV_AMD64,   /* Linux/macOS: RDI, RSI, RDX, RCX, R8, R9 */
    ASM_CONV_WIN64,        /* Windows x64: RCX, RDX, R8, R9 + 32-byte shadow */
};

/*
 * Tracks a local variable's location on the stack
 */
struct asm_local_var
{
    int stack_offset;   /* negative offset from %rbp */
    int size;           /* byte size */
};

struct asm_visit_ctx
{
    struct options options;

    /*
     * Label generation counters
     */
    int label_counter;
    int string_literal_counter;

    /*
     * Stack frame management for the current function
     */
    int current_stack_offset;   /* next available negative offset from %rbp */
    struct hash_map local_vars; /* name -> asm_local_var (via hash_item_set) */

    /*
     * Function context
     */
    const struct declarator* _Opt p_current_function_opt;
    bool is_in_function;

    /*
     * Calling convention for the current target
     */
    enum asm_calling_convention calling_convention;

    /*
     * Defer and control flow
     */
    struct asm_break_reference
    {
        struct selection_statement* _Opt p_selection_statement;
        struct iteration_statement* _Opt p_iteration_statement;
        int break_label;
        int continue_label;
    } break_reference;

    /*
     * Section buffers - collected during traversal, emitted at the end
     */
    struct osstream rodata_section;  /* .rodata (string literals, constants) */
    struct osstream data_section;    /* .data (initialized globals) */
    struct osstream bss_section;     /* .bss (uninitialized globals) */

    /*
     * Tracking maps
     */
    struct hash_map file_scope_declarator_map;
    struct hash_map string_literals_map; /* string content -> label name */

    _View struct ast ast;
};

void asm_visit(struct asm_visit_ctx* ctx, struct osstream* oss);
void asm_visit_ctx_destroy(_Dtor struct asm_visit_ctx* ctx);
