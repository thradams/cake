
#pragma safety enable
#include "version.h"
#include "ownership.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include "visit_asm.h"
#include "expressions.h"

/*
 * x86-64 assembly code generation visitor (GAS/AT&T syntax).
 * Supports both System V AMD64 and Windows x64 calling conventions.
 */

/* Calling convention register tables */
static const char* sysv_arg_regs_64[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };
static const char* sysv_arg_regs_32[] = { "%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d" };
static const int sysv_max_reg_args = 6;

static const char* win64_arg_regs_64[] = { "%rcx", "%rdx", "%r8", "%r9" };
static const char* win64_arg_regs_32[] = { "%ecx", "%edx", "%r8d", "%r9d" };
static const int win64_max_reg_args = 4;

/* Shadow space size for Windows x64 calling convention */
#define WIN64_SHADOW_SPACE 32

/* =========================================================================
 *  Context lifecycle
 * ========================================================================= */

void asm_visit_ctx_destroy(_Dtor struct asm_visit_ctx* ctx)
{
    hashmap_destroy(&ctx->local_vars);
    hashmap_destroy(&ctx->file_scope_declarator_map);
    hashmap_destroy(&ctx->string_literals_map);
    ss_close(&ctx->rodata_section);
    ss_close(&ctx->data_section);
    ss_close(&ctx->bss_section);
}

/* =========================================================================
 *  Helpers
 * ========================================================================= */

static int asm_new_label(struct asm_visit_ctx* ctx)
{
    return ctx->label_counter++;
}

static void emit(struct osstream* oss, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ss_vafprintf(oss, fmt, args);
    va_end(args);
}

static void emit_line(struct osstream* oss, const char* fmt, ...)
{
    ss_fprintf(oss, "    ");
    va_list args;
    va_start(args, fmt);
    ss_vafprintf(oss, fmt, args);
    va_end(args);
    ss_fprintf(oss, "\n");
}

static void emit_label(struct osstream* oss, int label_id)
{
    ss_fprintf(oss, ".L%d:\n", label_id);
}

static void emit_named_label(struct osstream* oss, const char* name)
{
    ss_fprintf(oss, "%s:\n", name);
}

static void emit_comment(struct osstream* oss, const char* fmt, ...)
{
    ss_fprintf(oss, "    # ");
    va_list args;
    va_start(args, fmt);
    ss_vafprintf(oss, fmt, args);
    va_end(args);
    ss_fprintf(oss, "\n");
}

static bool asm_is_win64(const struct asm_visit_ctx* ctx)
{
    return ctx->calling_convention == ASM_CONV_WIN64;
}

static const char** asm_arg_regs_64(const struct asm_visit_ctx* ctx)
{
    return asm_is_win64(ctx) ? win64_arg_regs_64 : sysv_arg_regs_64;
}

static const char** asm_arg_regs_32(const struct asm_visit_ctx* ctx)
{
    return asm_is_win64(ctx) ? win64_arg_regs_32 : sysv_arg_regs_32;
}

static int asm_max_reg_args(const struct asm_visit_ctx* ctx)
{
    return asm_is_win64(ctx) ? win64_max_reg_args : sysv_max_reg_args;
}

/* =========================================================================
 *  Type sizing helpers
 * ========================================================================= */

static int asm_type_size(const struct asm_visit_ctx* ctx, const struct type* p_type)
{
    size_t sz = 0;
    if (type_get_sizeof(p_type, &sz, ctx->options.target) != 0)
        return 8; /* default to pointer size */
    return (int)sz;
}

/*
 * Returns the GAS instruction suffix for a given byte size.
 *   1 -> 'b', 2 -> 'w', 4 -> 'l', 8 -> 'q'
 */
static char asm_size_suffix(int size)
{
    switch (size)
    {
    case 1: return 'b';
    case 2: return 'w';
    case 4: return 'l';
    case 8: return 'q';
    default: return 'q';
    }
}

/*
 * Returns the appropriate %rax sub-register name for a given byte size.
 */
static const char* asm_rax_for_size(int size)
{
    switch (size)
    {
    case 1: return "%al";
    case 2: return "%ax";
    case 4: return "%eax";
    case 8: return "%rax";
    default: return "%rax";
    }
}

static const char* asm_rcx_for_size(int size)
{
    switch (size)
    {
    case 1: return "%cl";
    case 2: return "%cx";
    case 4: return "%ecx";
    case 8: return "%rcx";
    default: return "%rcx";
    }
}

static const char* asm_rdx_for_size(int size)
{
    switch (size)
    {
    case 1: return "%dl";
    case 2: return "%dx";
    case 4: return "%edx";
    case 8: return "%rdx";
    default: return "%rdx";
    }
}

static bool asm_type_is_floating(const struct type* p_type)
{
    return type_is_floating_point(p_type);
}

/* Align value up to alignment boundary */
static int align_up(int value, int alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

/* =========================================================================
 *  Stack allocation for local variables
 * ========================================================================= */

static int asm_stack_alloc(struct asm_visit_ctx* ctx, int size)
{
    if (size < 1) size = 1;
    int aligned_size = align_up(size, 8);
    ctx->current_stack_offset -= aligned_size;
    return ctx->current_stack_offset;
}

static int asm_get_local_offset(struct asm_visit_ctx* ctx, const char* name)
{
    struct map_entry* _Opt p = hashmap_find(&ctx->local_vars, name);
    if (p != NULL)
    {
        return (int) p->data.number;
    }
    /* Should not happen - variable not found */
    assert(false);
    return 0;
}

static void asm_set_local_offset(struct asm_visit_ctx* ctx, const char* name, int offset)
{
    struct hash_item_set i = { 0 };
    i.number = offset;
    hashmap_set(&ctx->local_vars, name, &i);
    hash_item_set_destroy(&i);
}

/* =========================================================================
 *  String literal management
 * ========================================================================= */

static int asm_emit_string_literal(struct asm_visit_ctx* ctx, struct token* first_token, struct token* last_token)
{
    /* Build the string content */
    struct osstream content = { 0 };
    struct token* _Opt ptk = first_token;
    do
    {
        if (ptk == NULL) break;
        if ((ptk->flags & TK_FLAG_FINAL) && ptk->type == TK_STRING_LITERAL)
        {
            /* Extract string content without quotes */
            const char* lex = ptk->lexeme;
            /* Skip prefix (L, u8, etc) and opening quote */
            if (lex[0] == 'L') lex++;
            else if (lex[0] == 'u' && lex[1] == '8') lex += 2;
            if (*lex == '"') lex++;
            while (*lex && *(lex + 1) != '\0')
            {
                ss_putc(*lex, &content);
                lex++;
            }
        }
        if (ptk == last_token) break;
        ptk = ptk->next;
    } while (ptk);

    /* Check if we already emitted this string */
    if (content.c_str)
    {
        struct map_entry* _Opt p = hashmap_find(&ctx->string_literals_map, content.c_str);
        if (p != NULL)
        {
            int label = (int) p->data.number;
            ss_close(&content);
            return label;
        }
    }

    int label = ctx->string_literal_counter++;

    /* Record in map */
    if (content.c_str)
    {
        struct hash_item_set i = { 0 };
        i.number = label;
        hashmap_set(&ctx->string_literals_map, content.c_str, &i);
        hash_item_set_destroy(&i);
    }

    /* Emit in .rodata section */
    ss_fprintf(&ctx->rodata_section, ".LC%d:\n", label);
    ss_fprintf(&ctx->rodata_section, "    .string \"");

    /* Re-emit the string with proper escaping for GAS */
    ptk = first_token;
    do
    {
        if (ptk == NULL) break;
        if ((ptk->flags & TK_FLAG_FINAL) && ptk->type == TK_STRING_LITERAL)
        {
            const char* lex = ptk->lexeme;
            if (lex[0] == 'L') lex++;
            else if (lex[0] == 'u' && lex[1] == '8') lex += 2;
            if (*lex == '"') lex++;
            while (*lex)
            {
                if (*(lex + 1) == '\0') break; /* skip closing quote */
                unsigned char ch = (unsigned char)*lex;
                if (ch >= 128)
                    ss_fprintf(&ctx->rodata_section, "\\%03o", ch);
                else
                    ss_putc(ch, &ctx->rodata_section);
                lex++;
            }
        }
        if (ptk == last_token) break;
        ptk = ptk->next;
    } while (ptk);

    ss_fprintf(&ctx->rodata_section, "\"\n");

    ss_close(&content);
    return label;
}

/* =========================================================================
 *  Member offset computation
 * ========================================================================= */

static int asm_get_member_offset(const struct asm_visit_ctx* ctx,
    const struct type* p_struct_type,
    int member_index)
{
    if (!type_is_struct_or_union(p_struct_type))
        return 0;

    assert(p_struct_type->struct_or_union_specifier != NULL);
    struct struct_or_union_specifier* _Opt p_complete =
        get_complete_struct_or_union_specifier(p_struct_type->struct_or_union_specifier);

    if (p_complete == NULL)
        return 0;

    /* Walk members to accumulate offset */
    int offset = 0;
    int idx = 0;
    struct member_declaration* _Opt md = p_complete->member_declaration_list.head;
    while (md)
    {
        if (md->member_declarator_list_opt)
        {
            struct member_declarator* _Opt decl = md->member_declarator_list_opt->head;
            while (decl)
            {
                if (decl->declarator)
                {
                    if (idx == member_index)
                    {
                        /* Use type_get_offsetof if available, else compute manually */
                        return offset;
                    }
                    size_t member_size = 0;
                    type_get_sizeof(&decl->declarator->type, &member_size, ctx->options.target);
                    int alignment = (int)member_size;
                    if (alignment > 8) alignment = 8;
                    if (alignment < 1) alignment = 1;
                    offset = align_up(offset, alignment);
                    offset += (int)member_size;
                    idx++;
                }
                decl = decl->next;
            }
        }
        md = md->next;
    }
    return 0;
}

static int asm_find_member_name(const struct type* p_type, int index, char name[100])
{
    if (!type_is_struct_or_union(p_type))
        return 1;

    assert(p_type->struct_or_union_specifier != NULL);
    struct struct_or_union_specifier* _Opt p_complete =
        get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

    if (p_complete)
    {
        int idx = 0;
        struct member_declaration* _Opt md = p_complete->member_declaration_list.head;
        while (md)
        {
            if (md->member_declarator_list_opt)
            {
                struct member_declarator* _Opt decl = md->member_declarator_list_opt->head;
                while (decl)
                {
                    if (decl->declarator && decl->declarator->name_opt)
                    {
                        if (idx == index)
                        {
                            snprintf(name, 100, "%s", decl->declarator->name_opt->lexeme);
                            return 0;
                        }
                        idx++;
                    }
                    decl = decl->next;
                }
            }
            md = md->next;
        }
    }
    return 1;
}

/* =========================================================================
 *  Forward declarations
 * ========================================================================= */

static void asm_visit_expression(struct asm_visit_ctx* ctx, struct osstream* oss, struct expression* p_expression);
static void asm_visit_expression_addr(struct asm_visit_ctx* ctx, struct osstream* oss, struct expression* p_expression);
static void asm_visit_declaration(struct asm_visit_ctx* ctx, struct osstream* oss, struct declaration* p_declaration);
static void asm_visit_compound_statement(struct asm_visit_ctx* ctx, struct osstream* oss, struct compound_statement* p_compound_statement);
static void asm_visit_statement(struct asm_visit_ctx* ctx, struct osstream* oss, struct statement* p_statement);
static void asm_visit_unlabeled_statement(struct asm_visit_ctx* ctx, struct osstream* oss, struct unlabeled_statement* p_unlabeled_statement);
static void asm_visit_secondary_block(struct asm_visit_ctx* ctx, struct osstream* oss, struct secondary_block* p_secondary_block);
static void asm_visit_init_declarator(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct init_declarator* p_init_declarator,
    enum function_specifier_flags function_specifier_flags,
    enum storage_class_specifier_flags storage_class_specifier_flags);
static void asm_visit_function_body(struct asm_visit_ctx* ctx, struct osstream* oss,
    const struct declarator* function_definition);

/* =========================================================================
 *  Defer handling (mirrors visit_il.c pattern)
 * ========================================================================= */

static int asm_defer_count(struct defer_list* p_defer_list)
{
    int count = 0;
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        if (p_item->defer_statement) count++;
        p_item = p_item->next;
    }
    return count;
}

static void asm_emit_defer_item(struct asm_visit_ctx* ctx, struct osstream* oss, struct defer_list_item* p_item)
{
    if (p_item->defer_statement == NULL)
        return;
    asm_visit_unlabeled_statement(ctx, oss, p_item->defer_statement->unlabeled_statement);
}

static void asm_emit_defer_list(struct asm_visit_ctx* ctx, struct osstream* oss, struct defer_list* p_defer_list)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        asm_emit_defer_item(ctx, oss, p_item);
        p_item = p_item->next;
    }
}

/* =========================================================================
 *  Expression: compute address of an lvalue into %rax
 * ========================================================================= */

static void asm_visit_expression_addr(struct asm_visit_ctx* ctx, struct osstream* oss, struct expression* p_expression)
{
    switch (p_expression->expression_type)
    {
    case PRIMARY_EXPRESSION_DECLARATOR:
    {
        assert(p_expression->declarator != NULL);
        const char* name = "";
        if (p_expression->declarator->name_opt)
            name = p_expression->declarator->name_opt->lexeme;

        const bool is_local =
            (p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_BLOCK_SCOPE) &&
            !(p_expression->declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC) &&
            !(p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN);

        if (is_local)
        {
            int offset = asm_get_local_offset(ctx, name);
            emit_line(oss, "leaq %d(%%rbp), %%rax", offset);
        }
        else
        {
            /* Global/static - use RIP-relative addressing */
            emit_line(oss, "leaq %s(%%rip), %%rax", name);
        }
    }
    break;

    case UNARY_EXPRESSION_CONTENT:
        /* *p - the address of *p is just the value of p */
        assert(p_expression->right != NULL);
        asm_visit_expression(ctx, oss, p_expression->right);
        break;

    case POSTFIX_DOT:
    {
        assert(p_expression->left != NULL);
        asm_visit_expression_addr(ctx, oss, p_expression->left);
        int member_offset = asm_get_member_offset(ctx, &p_expression->left->type, p_expression->member_index);
        if (member_offset != 0)
        {
            emit_line(oss, "addq $%d, %%rax", member_offset);
        }
    }
    break;

    case POSTFIX_ARROW:
    {
        assert(p_expression->left != NULL);
        asm_visit_expression(ctx, oss, p_expression->left);
        int member_offset = asm_get_member_offset(ctx, &p_expression->left->type, p_expression->member_index);
        if (member_offset != 0)
        {
            emit_line(oss, "addq $%d, %%rax", member_offset);
        }
    }
    break;

    case POSTFIX_ARRAY:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        /* address = base + index * element_size */
        asm_visit_expression(ctx, oss, p_expression->right);
        int elem_size = asm_type_size(ctx, &p_expression->type);
        if (elem_size != 1)
        {
            emit_line(oss, "imulq $%d, %%rax, %%rax", elem_size);
        }
        emit_line(oss, "pushq %%rax");
        if (type_is_array(&p_expression->left->type))
            asm_visit_expression_addr(ctx, oss, p_expression->left);
        else
            asm_visit_expression(ctx, oss, p_expression->left);
        emit_line(oss, "popq %%rcx");
        emit_line(oss, "addq %%rcx, %%rax");
    }
    break;

    default:
        /* Fallback: evaluate expression - it should produce a pointer */
        asm_visit_expression(ctx, oss, p_expression);
        break;
    }
}

/* =========================================================================
 *  Expression visitor: result in %rax (integer/ptr) or %xmm0 (float)
 * ========================================================================= */

static void asm_visit_expression(struct asm_visit_ctx* ctx, struct osstream* oss, struct expression* p_expression)
{
    /* If the expression has a compile-time constant value, emit it directly */
    if (object_has_constant_value(&p_expression->object))
    {
        if (type_is_void_ptr(&p_expression->type) ||
            type_is_nullptr_t(&p_expression->type))
        {
            if (object_is_zero(&p_expression->object))
            {
                emit_line(oss, "xorq %%rax, %%rax");
                return;
            }
        }
        else if (type_is_arithmetic(&p_expression->type))
        {
            if (asm_type_is_floating(&p_expression->type))
            {
                /* For float/double constants we emit in .rodata and load */
                /* Simplified: use integer representation for now */
                struct osstream val_ss = { 0 };
                object_print_value(&val_ss, &p_expression->object, ctx->options.target);
                if (val_ss.c_str)
                {
                    int lbl = ctx->string_literal_counter++;
                    if (type_is_float(&p_expression->type))
                    {
                        ss_fprintf(&ctx->rodata_section, "    .align 4\n");
                        ss_fprintf(&ctx->rodata_section, ".LC%d:\n", lbl);
                        ss_fprintf(&ctx->rodata_section, "    .float %s\n", val_ss.c_str);
                        emit_line(oss, "movss .LC%d(%%rip), %%xmm0", lbl);
                    }
                    else
                    {
                        ss_fprintf(&ctx->rodata_section, "    .align 8\n");
                        ss_fprintf(&ctx->rodata_section, ".LC%d:\n", lbl);
                        ss_fprintf(&ctx->rodata_section, "    .double %s\n", val_ss.c_str);
                        emit_line(oss, "movsd .LC%d(%%rip), %%xmm0", lbl);
                    }
                }
                ss_close(&val_ss);
                return;
            }
            else
            {
                struct osstream val_ss = { 0 };
                object_print_value(&val_ss, &p_expression->object, ctx->options.target);
                if (val_ss.c_str)
                {
                    emit_line(oss, "movq $%s, %%rax", val_ss.c_str);
                }
                ss_close(&val_ss);
                return;
            }
        }
    }

    switch (p_expression->expression_type)
    {
    case EXPRESSION_TYPE_INVALID:
        assert(false);
        break;

    case PRIMARY_EXPRESSION__FUNC__:
    {
        /* __func__ is a string literal with the function name */
        assert(ctx->p_current_function_opt);
        const char* func_name = "unknown";
        if (ctx->p_current_function_opt->name_opt)
            func_name = ctx->p_current_function_opt->name_opt->lexeme;

        int lbl = ctx->string_literal_counter++;
        ss_fprintf(&ctx->rodata_section, ".LC%d:\n", lbl);
        ss_fprintf(&ctx->rodata_section, "    .string \"%s\"\n", func_name);
        emit_line(oss, "leaq .LC%d(%%rip), %%rax", lbl);
    }
    break;

    case PRIMARY_EXPRESSION_DECLARATOR:
    {
        assert(p_expression->declarator != NULL);
        const char* name = "";
        if (p_expression->declarator->name_opt)
            name = p_expression->declarator->name_opt->lexeme;

        const bool is_function = type_is_function(&p_expression->declarator->type);
        const bool is_local =
            (p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_BLOCK_SCOPE) &&
            !(p_expression->declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC) &&
            !(p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN);

        if (is_function)
        {
            /* Function reference - load address */
            emit_line(oss, "leaq %s(%%rip), %%rax", name);
        }
        else if (type_is_array(&p_expression->declarator->type))
        {
            /* Array decays to pointer - load address */
            if (is_local)
            {
                int offset = asm_get_local_offset(ctx, name);
                emit_line(oss, "leaq %d(%%rbp), %%rax", offset);
            }
            else
            {
                emit_line(oss, "leaq %s(%%rip), %%rax", name);
            }
        }
        else if (is_local)
        {
            int offset = asm_get_local_offset(ctx, name);
            int size = asm_type_size(ctx, &p_expression->type);

            if (asm_type_is_floating(&p_expression->type))
            {
                if (type_is_float(&p_expression->type))
                    emit_line(oss, "movss %d(%%rbp), %%xmm0", offset);
                else
                    emit_line(oss, "movsd %d(%%rbp), %%xmm0", offset);
            }
            else if (type_is_struct_or_union(&p_expression->type))
            {
                /* For structs, load address into %rax */
                emit_line(oss, "leaq %d(%%rbp), %%rax", offset);
            }
            else
            {
                /* Scalar load with appropriate size */
                if (size <= 4)
                {
                    bool is_unsigned = (p_expression->type.type_specifier_flags & TYPE_SPECIFIER_UNSIGNED) != 0;
                    if (size == 1)
                        emit_line(oss, "%s %d(%%rbp), %%eax", is_unsigned ? "movzbl" : "movsbl", offset);
                    else if (size == 2)
                        emit_line(oss, "%s %d(%%rbp), %%eax", is_unsigned ? "movzwl" : "movswl", offset);
                    else
                        emit_line(oss, "movl %d(%%rbp), %%eax", offset);
                }
                else
                {
                    emit_line(oss, "movq %d(%%rbp), %%rax", offset);
                }
            }
        }
        else
        {
            /* Global/static variable */
            int size = asm_type_size(ctx, &p_expression->type);
            if (asm_type_is_floating(&p_expression->type))
            {
                if (type_is_float(&p_expression->type))
                    emit_line(oss, "movss %s(%%rip), %%xmm0", name);
                else
                    emit_line(oss, "movsd %s(%%rip), %%xmm0", name);
            }
            else if (type_is_struct_or_union(&p_expression->type))
            {
                emit_line(oss, "leaq %s(%%rip), %%rax", name);
            }
            else
            {
                char suffix = asm_size_suffix(size);
                emit_line(oss, "mov%c %s(%%rip), %s", suffix, name, asm_rax_for_size(size));
            }
        }
    }
    break;

    case PRIMARY_EXPRESSION_STRING_LITERAL:
    {
        int lbl = asm_emit_string_literal(ctx, p_expression->first_token, p_expression->last_token);
        emit_line(oss, "leaq .LC%d(%%rip), %%rax", lbl);
    }
    break;

    case PRIMARY_EXPRESSION_ENUMERATOR:
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
    case PRIMARY_EXPRESSION_NUMBER:
    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
    {
        struct osstream val_ss = { 0 };
        object_print_value(&val_ss, &p_expression->object, ctx->options.target);
        if (val_ss.c_str)
        {
            emit_line(oss, "movq $%s, %%rax", val_ss.c_str);
        }
        ss_close(&val_ss);
    }
    break;

    case PRIMARY_EXPRESSION_PARENTHESIS:
        assert(p_expression->right != NULL);
        asm_visit_expression(ctx, oss, p_expression->right);
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        assert(p_expression->generic_selection != NULL);
        if (p_expression->generic_selection->p_view_selected_expression)
        {
            asm_visit_expression(ctx, oss, p_expression->generic_selection->p_view_selected_expression);
        }
        break;

    /* ---- Unary expressions ---- */

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:
    case UNARY_EXPRESSION_SIZEOF_TYPE:
    {
        int sz = 0;
        if (p_expression->right)
            sz = asm_type_size(ctx, &p_expression->right->type);
        else if (p_expression->type_name)
            sz = asm_type_size(ctx, &p_expression->type_name->type);
        emit_line(oss, "movq $%d, %%rax", sz);
    }
    break;

    case UNARY_EXPRESSION_ALIGNOF_TYPE:
    case UNARY_EXPRESSION_ALIGNOF_EXPRESSION:
    {
        /* Simplified - use size as alignment proxy */
        struct osstream val_ss = { 0 };
        object_print_value(&val_ss, &p_expression->object, ctx->options.target);
        if (val_ss.c_str)
            emit_line(oss, "movq $%s, %%rax", val_ss.c_str);
        else
            emit_line(oss, "movq $8, %%rax");
        ss_close(&val_ss);
    }
    break;

    case UNARY_EXPRESSION_INCREMENT:
        /* ++x: load address, increment, store, result = new value */
        assert(p_expression->right != NULL);
    {
        int size = asm_type_size(ctx, &p_expression->right->type);
        char suf = asm_size_suffix(size);
        asm_visit_expression_addr(ctx, oss, p_expression->right);
        emit_line(oss, "pushq %%rax");
        emit_line(oss, "mov%c (%%rax), %s", suf, asm_rax_for_size(size));
        if (type_is_pointer(&p_expression->right->type))
        {
            struct type pointed = type_remove_pointer(&p_expression->right->type);
            int elem_sz = asm_type_size(ctx, &pointed);
            emit_line(oss, "addq $%d, %%rax", elem_sz);
            type_destroy(&pointed);
        }
        else
        {
            emit_line(oss, "add%c $1, %s", suf, asm_rax_for_size(size));
        }
        emit_line(oss, "popq %%rcx");
        emit_line(oss, "mov%c %s, (%%rcx)", suf, asm_rax_for_size(size));
    }
    break;

    case UNARY_EXPRESSION_DECREMENT:
        assert(p_expression->right != NULL);
    {
        int size = asm_type_size(ctx, &p_expression->right->type);
        char suf = asm_size_suffix(size);
        asm_visit_expression_addr(ctx, oss, p_expression->right);
        emit_line(oss, "pushq %%rax");
        emit_line(oss, "mov%c (%%rax), %s", suf, asm_rax_for_size(size));
        if (type_is_pointer(&p_expression->right->type))
        {
            struct type pointed = type_remove_pointer(&p_expression->right->type);
            int elem_sz = asm_type_size(ctx, &pointed);
            emit_line(oss, "subq $%d, %%rax", elem_sz);
            type_destroy(&pointed);
        }
        else
        {
            emit_line(oss, "sub%c $1, %s", suf, asm_rax_for_size(size));
        }
        emit_line(oss, "popq %%rcx");
        emit_line(oss, "mov%c %s, (%%rcx)", suf, asm_rax_for_size(size));
    }
    break;

    case POSTFIX_INCREMENT:
        /* x++: load old value, store incremented, result = old value */
        assert(p_expression->left != NULL);
    {
        int size = asm_type_size(ctx, &p_expression->left->type);
        char suf = asm_size_suffix(size);
        asm_visit_expression_addr(ctx, oss, p_expression->left);
        emit_line(oss, "pushq %%rax"); /* save address */
        emit_line(oss, "mov%c (%%rax), %s", suf, asm_rax_for_size(size));
        emit_line(oss, "pushq %%rax"); /* save old value */
        if (type_is_pointer(&p_expression->left->type))
        {
            struct type pointed = type_remove_pointer(&p_expression->left->type);
            int elem_sz = asm_type_size(ctx, &pointed);
            emit_line(oss, "addq $%d, %%rax", elem_sz);
            type_destroy(&pointed);
        }
        else
        {
            emit_line(oss, "add%c $1, %s", suf, asm_rax_for_size(size));
        }
        emit_line(oss, "movq %%rax, %%rdx"); /* new value in rdx */
        emit_line(oss, "movq 8(%%rsp), %%rcx"); /* address */
        emit_line(oss, "mov%c %s, (%%rcx)", suf, asm_rdx_for_size(size));
        emit_line(oss, "popq %%rax"); /* restore old value */
        emit_line(oss, "addq $8, %%rsp"); /* pop address */
    }
    break;

    case POSTFIX_DECREMENT:
        assert(p_expression->left != NULL);
    {
        int size = asm_type_size(ctx, &p_expression->left->type);
        char suf = asm_size_suffix(size);
        asm_visit_expression_addr(ctx, oss, p_expression->left);
        emit_line(oss, "pushq %%rax");
        emit_line(oss, "mov%c (%%rax), %s", suf, asm_rax_for_size(size));
        emit_line(oss, "pushq %%rax");
        if (type_is_pointer(&p_expression->left->type))
        {
            struct type pointed = type_remove_pointer(&p_expression->left->type);
            int elem_sz = asm_type_size(ctx, &pointed);
            emit_line(oss, "subq $%d, %%rax", elem_sz);
            type_destroy(&pointed);
        }
        else
        {
            emit_line(oss, "sub%c $1, %s", suf, asm_rax_for_size(size));
        }
        emit_line(oss, "movq %%rax, %%rdx");
        emit_line(oss, "movq 8(%%rsp), %%rcx");
        emit_line(oss, "mov%c %s, (%%rcx)", suf, asm_rdx_for_size(size));
        emit_line(oss, "popq %%rax");
        emit_line(oss, "addq $8, %%rsp");
    }
    break;

    case UNARY_EXPRESSION_NOT:
        assert(p_expression->right != NULL);
        asm_visit_expression(ctx, oss, p_expression->right);
        emit_line(oss, "testq %%rax, %%rax");
        emit_line(oss, "sete %%al");
        emit_line(oss, "movzbl %%al, %%eax");
        break;

    case UNARY_EXPRESSION_BITNOT:
        assert(p_expression->right != NULL);
        asm_visit_expression(ctx, oss, p_expression->right);
        emit_line(oss, "notq %%rax");
        break;

    case UNARY_EXPRESSION_NEG:
        assert(p_expression->right != NULL);
        asm_visit_expression(ctx, oss, p_expression->right);
        if (asm_type_is_floating(&p_expression->type))
        {
            /* Negate float/double by XOR with sign bit */
            int lbl = ctx->string_literal_counter++;
            if (type_is_float(&p_expression->type))
            {
                ss_fprintf(&ctx->rodata_section, "    .align 16\n.LC%d:\n    .long 0x80000000\n    .long 0\n    .long 0\n    .long 0\n", lbl);
                emit_line(oss, "xorps .LC%d(%%rip), %%xmm0", lbl);
            }
            else
            {
                ss_fprintf(&ctx->rodata_section, "    .align 16\n.LC%d:\n    .quad 0x8000000000000000\n    .quad 0\n", lbl);
                emit_line(oss, "xorpd .LC%d(%%rip), %%xmm0", lbl);
            }
        }
        else
        {
            emit_line(oss, "negq %%rax");
        }
        break;

    case UNARY_EXPRESSION_PLUS:
        assert(p_expression->right != NULL);
        asm_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_CONTENT:
        /* Dereference: *ptr */
        assert(p_expression->right != NULL);
        asm_visit_expression(ctx, oss, p_expression->right);
        if (!type_is_struct_or_union(&p_expression->type) &&
            !type_is_array(&p_expression->type))
        {
            int size = asm_type_size(ctx, &p_expression->type);
            if (asm_type_is_floating(&p_expression->type))
            {
                if (type_is_float(&p_expression->type))
                    emit_line(oss, "movss (%%rax), %%xmm0");
                else
                    emit_line(oss, "movsd (%%rax), %%xmm0");
            }
            else
            {
                if (size < 8)
                {
                    bool is_unsigned = (p_expression->type.type_specifier_flags & TYPE_SPECIFIER_UNSIGNED) != 0;
                    if (size == 1)
                        emit_line(oss, "%s (%%rax), %%eax", is_unsigned ? "movzbl" : "movsbl");
                    else if (size == 2)
                        emit_line(oss, "%s (%%rax), %%eax", is_unsigned ? "movzwl" : "movswl");
                    else
                        emit_line(oss, "movl (%%rax), %%eax");
                }
                else
                {
                    emit_line(oss, "movq (%%rax), %%rax");
                }
            }
        }
        /* else: struct/array - address is already the result */
        break;

    case UNARY_EXPRESSION_ADDRESSOF:
        assert(p_expression->right != NULL);
        asm_visit_expression_addr(ctx, oss, p_expression->right);
        break;

    /* ---- Cast ---- */

    case CAST_EXPRESSION:
        assert(p_expression->left != NULL);
        asm_visit_expression(ctx, oss, p_expression->left);
        /* Integer truncation/extension handled implicitly by register sizes */
        /* Float <-> int conversions: */
        if (p_expression->type_name)
        {
            bool dst_float = type_is_floating_point(&p_expression->type_name->type);
            bool src_float = type_is_floating_point(&p_expression->left->type);
            if (!src_float && dst_float)
            {
                /* int -> float/double */
                if (type_is_float(&p_expression->type_name->type))
                {
                    emit_line(oss, "cvtsi2ssl %%eax, %%xmm0");
                }
                else
                {
                    emit_line(oss, "cvtsi2sdq %%rax, %%xmm0");
                }
            }
            else if (src_float && !dst_float)
            {
                /* float/double -> int */
                if (type_is_float(&p_expression->left->type))
                    emit_line(oss, "cvttss2siq %%xmm0, %%rax");
                else
                    emit_line(oss, "cvttsd2siq %%xmm0, %%rax");
            }
            else if (src_float && dst_float)
            {
                /* float <-> double */
                if (type_is_float(&p_expression->left->type) && type_is_double(&p_expression->type_name->type))
                    emit_line(oss, "cvtss2sd %%xmm0, %%xmm0");
                else if (type_is_double(&p_expression->left->type) && type_is_float(&p_expression->type_name->type))
                    emit_line(oss, "cvtsd2ss %%xmm0, %%xmm0");
            }
        }
        break;

    /* ---- Postfix expressions ---- */

    case POSTFIX_FUNCTION_CALL:
    {
        assert(p_expression->left != NULL);
        emit_comment(oss, "function call");

        /* Evaluate arguments right to left, push to stack for extras */
        int arg_count = 0;
        struct argument_expression* _Opt arg = p_expression->argument_expression_list.head;
        /* Count args */
        while (arg)
        {
            arg_count++;
            arg = arg->next;
        }

        int max_regs = asm_max_reg_args(ctx);

        /* Allocate space for stack arguments (if any) and shadow space (win64) */
        int stack_args = (arg_count > max_regs) ? (arg_count - max_regs) : 0;
        int stack_space = stack_args * 8;
        if (asm_is_win64(ctx))
        {
            stack_space += WIN64_SHADOW_SPACE;
        }

        /* Align stack to 16 bytes */
        int total_push = stack_space;
        if (total_push % 16 != 0)
            total_push += (16 - total_push % 16);

        if (total_push > 0)
        {
            emit_line(oss, "subq $%d, %%rsp", total_push);
        }

        /* Evaluate each argument and place in register or on stack */
        arg = p_expression->argument_expression_list.head;

        /* We need to evaluate all args first, saving them on stack,
           then move to registers to avoid clobbering */
        /* Simple approach: evaluate each arg, push result */
        struct argument_expression* _Opt arg2 = p_expression->argument_expression_list.head;
        while (arg2)
        {
            asm_visit_expression(ctx, oss, arg2->expression);
            emit_line(oss, "pushq %%rax");
            arg2 = arg2->next;
        }

        /* Now pop into registers (reverse order) */
        const char** regs64 = asm_arg_regs_64(ctx);
        for (int i = arg_count - 1; i >= 0; i--)
        {
            if (i < max_regs)
            {
                emit_line(oss, "popq %s", regs64[i]);
            }
            else
            {
                /* Stack argument */
                int stack_pos = (i - max_regs) * 8;
                if (asm_is_win64(ctx))
                    stack_pos += WIN64_SHADOW_SPACE;
                emit_line(oss, "popq %%rax");
                emit_line(oss, "movq %%rax, %d(%%rsp)", stack_pos);
            }
        }

        /* Call the function */
        if (p_expression->left->expression_type == PRIMARY_EXPRESSION_DECLARATOR &&
            p_expression->left->declarator &&
            p_expression->left->declarator->name_opt)
        {
            emit_line(oss, "call %s", p_expression->left->declarator->name_opt->lexeme);
        }
        else
        {
            /* Indirect call */
            /* First save the args, evaluate function pointer */
            asm_visit_expression(ctx, oss, p_expression->left);
            emit_line(oss, "call *%%rax");
        }

        if (total_push > 0)
        {
            emit_line(oss, "addq $%d, %%rsp", total_push);
        }
        /* Result is in %rax (or %xmm0 for float) */
    }
    break;

    case POSTFIX_ARRAY:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        /* array[index] -> load from address */
        asm_visit_expression_addr(ctx, oss, p_expression);
        if (!type_is_struct_or_union(&p_expression->type) &&
            !type_is_array(&p_expression->type))
        {
            int size = asm_type_size(ctx, &p_expression->type);
            if (asm_type_is_floating(&p_expression->type))
            {
                if (type_is_float(&p_expression->type))
                    emit_line(oss, "movss (%%rax), %%xmm0");
                else
                    emit_line(oss, "movsd (%%rax), %%xmm0");
            }
            else
            {
                if (size < 8)
                {
                    bool is_unsigned = (p_expression->type.type_specifier_flags & TYPE_SPECIFIER_UNSIGNED) != 0;
                    if (size == 1)
                        emit_line(oss, "%s (%%rax), %%eax", is_unsigned ? "movzbl" : "movsbl");
                    else if (size == 2)
                        emit_line(oss, "%s (%%rax), %%eax", is_unsigned ? "movzwl" : "movswl");
                    else
                        emit_line(oss, "movl (%%rax), %%eax");
                }
                else
                {
                    emit_line(oss, "movq (%%rax), %%rax");
                }
            }
        }
    }
    break;

    case POSTFIX_DOT:
    {
        assert(p_expression->left != NULL);
        int member_offset = asm_get_member_offset(ctx, &p_expression->left->type, p_expression->member_index);
        asm_visit_expression_addr(ctx, oss, p_expression->left);
        if (member_offset != 0)
            emit_line(oss, "addq $%d, %%rax", member_offset);

        if (!type_is_struct_or_union(&p_expression->type) &&
            !type_is_array(&p_expression->type))
        {
            int size = asm_type_size(ctx, &p_expression->type);
            if (asm_type_is_floating(&p_expression->type))
            {
                if (type_is_float(&p_expression->type))
                    emit_line(oss, "movss (%%rax), %%xmm0");
                else
                    emit_line(oss, "movsd (%%rax), %%xmm0");
            }
            else
            {
                char suf = asm_size_suffix(size);
                emit_line(oss, "mov%c (%%rax), %s", suf, asm_rax_for_size(size));
            }
        }
    }
    break;

    case POSTFIX_ARROW:
    {
        assert(p_expression->left != NULL);
        asm_visit_expression(ctx, oss, p_expression->left);
        int member_offset = asm_get_member_offset(ctx, &p_expression->left->type, p_expression->member_index);
        if (member_offset != 0)
            emit_line(oss, "addq $%d, %%rax", member_offset);

        if (!type_is_struct_or_union(&p_expression->type) &&
            !type_is_array(&p_expression->type))
        {
            int size = asm_type_size(ctx, &p_expression->type);
            if (asm_type_is_floating(&p_expression->type))
            {
                if (type_is_float(&p_expression->type))
                    emit_line(oss, "movss (%%rax), %%xmm0");
                else
                    emit_line(oss, "movsd (%%rax), %%xmm0");
            }
            else
            {
                char suf = asm_size_suffix(size);
                emit_line(oss, "mov%c (%%rax), %s", suf, asm_rax_for_size(size));
            }
        }
    }
    break;

    /* ---- Binary arithmetic ---- */

    case MULTIPLICATIVE_EXPRESSION_MULT:
    case MULTIPLICATIVE_EXPRESSION_DIV:
    case MULTIPLICATIVE_EXPRESSION_MOD:
    case ADDITIVE_EXPRESSION_PLUS:
    case ADDITIVE_EXPRESSION_MINUS:
    case SHIFT_EXPRESSION_LEFT:
    case SHIFT_EXPRESSION_RIGHT:
    case AND_EXPRESSION:
    case EXCLUSIVE_OR_EXPRESSION:
    case INCLUSIVE_OR_EXPRESSION:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        bool is_float_op = asm_type_is_floating(&p_expression->type);

        if (is_float_op)
        {
            /* Float binary ops: left in xmm0, right in xmm1 */
            asm_visit_expression(ctx, oss, p_expression->left);
            emit_line(oss, "subq $8, %%rsp");
            if (type_is_float(&p_expression->type))
                emit_line(oss, "movss %%xmm0, (%%rsp)");
            else
                emit_line(oss, "movsd %%xmm0, (%%rsp)");

            asm_visit_expression(ctx, oss, p_expression->right);
            emit_line(oss, "movapd %%xmm0, %%xmm1");
            if (type_is_float(&p_expression->type))
                emit_line(oss, "movss (%%rsp), %%xmm0");
            else
                emit_line(oss, "movsd (%%rsp), %%xmm0");
            emit_line(oss, "addq $8, %%rsp");

            const char* insn = "";
            bool is_f = type_is_float(&p_expression->type);
            switch (p_expression->expression_type)
            {
            case ADDITIVE_EXPRESSION_PLUS:  insn = is_f ? "addss" : "addsd"; break;
            case ADDITIVE_EXPRESSION_MINUS: insn = is_f ? "subss" : "subsd"; break;
            case MULTIPLICATIVE_EXPRESSION_MULT: insn = is_f ? "mulss" : "mulsd"; break;
            case MULTIPLICATIVE_EXPRESSION_DIV:  insn = is_f ? "divss" : "divsd"; break;
            default: break;
            }
            if (insn[0])
                emit_line(oss, "%s %%xmm1, %%xmm0", insn);
        }
        else
        {
            /* Integer: evaluate left, push, evaluate right, pop left to rcx */
            asm_visit_expression(ctx, oss, p_expression->left);
            emit_line(oss, "pushq %%rax");
            asm_visit_expression(ctx, oss, p_expression->right);
            emit_line(oss, "movq %%rax, %%rcx"); /* right in rcx */
            emit_line(oss, "popq %%rax");         /* left in rax */

            /* Handle pointer arithmetic: ptr + int, ptr - int */
            if (type_is_pointer(&p_expression->left->type) &&
                (p_expression->expression_type == ADDITIVE_EXPRESSION_PLUS ||
                    p_expression->expression_type == ADDITIVE_EXPRESSION_MINUS))
            {
                struct type pointed = type_remove_pointer(&p_expression->left->type);
                int elem_sz = asm_type_size(ctx, &pointed);
                if (elem_sz > 1)
                    emit_line(oss, "imulq $%d, %%rcx, %%rcx", elem_sz);
                type_destroy(&pointed);
            }

            switch (p_expression->expression_type)
            {
            case ADDITIVE_EXPRESSION_PLUS:
                emit_line(oss, "addq %%rcx, %%rax");
                break;
            case ADDITIVE_EXPRESSION_MINUS:
                emit_line(oss, "subq %%rcx, %%rax");
                /* If pointer - pointer, divide by element size */
                if (type_is_pointer(&p_expression->left->type) &&
                    type_is_pointer(&p_expression->right->type))
                {
                    struct type pointed = type_remove_pointer(&p_expression->left->type);
                    int elem_sz = asm_type_size(ctx, &pointed);
                    if (elem_sz > 1)
                    {
                        emit_line(oss, "cqto");
                        emit_line(oss, "movq $%d, %%rcx", elem_sz);
                        emit_line(oss, "idivq %%rcx");
                    }
                    type_destroy(&pointed);
                }
                break;
            case MULTIPLICATIVE_EXPRESSION_MULT:
                emit_line(oss, "imulq %%rcx, %%rax");
                break;
            case MULTIPLICATIVE_EXPRESSION_DIV:
                emit_line(oss, "cqto");
                emit_line(oss, "idivq %%rcx");
                break;
            case MULTIPLICATIVE_EXPRESSION_MOD:
                emit_line(oss, "cqto");
                emit_line(oss, "idivq %%rcx");
                emit_line(oss, "movq %%rdx, %%rax"); /* remainder */
                break;
            case SHIFT_EXPRESSION_LEFT:
                emit_line(oss, "salq %%cl, %%rax");
                break;
            case SHIFT_EXPRESSION_RIGHT:
            {
                bool is_unsigned = (p_expression->left->type.type_specifier_flags & TYPE_SPECIFIER_UNSIGNED) != 0;
                emit_line(oss, "%s %%cl, %%rax", is_unsigned ? "shrq" : "sarq");
            }
            break;
            case AND_EXPRESSION:
                emit_line(oss, "andq %%rcx, %%rax");
                break;
            case EXCLUSIVE_OR_EXPRESSION:
                emit_line(oss, "xorq %%rcx, %%rax");
                break;
            case INCLUSIVE_OR_EXPRESSION:
                emit_line(oss, "orq %%rcx, %%rax");
                break;
            default:
                break;
            }
        }
    }
    break;

    /* ---- Comparison ---- */

    case RELATIONAL_EXPRESSION_BIGGER_THAN:
    case RELATIONAL_EXPRESSION_LESS_THAN:
    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
    case EQUALITY_EXPRESSION_EQUAL:
    case EQUALITY_EXPRESSION_NOT_EQUAL:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        if (asm_type_is_floating(&p_expression->left->type))
        {
            /* Float comparison */
            asm_visit_expression(ctx, oss, p_expression->left);
            emit_line(oss, "subq $8, %%rsp");
            if (type_is_float(&p_expression->left->type))
                emit_line(oss, "movss %%xmm0, (%%rsp)");
            else
                emit_line(oss, "movsd %%xmm0, (%%rsp)");
            asm_visit_expression(ctx, oss, p_expression->right);
            emit_line(oss, "movapd %%xmm0, %%xmm1");
            if (type_is_float(&p_expression->left->type))
            {
                emit_line(oss, "movss (%%rsp), %%xmm0");
                emit_line(oss, "ucomiss %%xmm1, %%xmm0");
            }
            else
            {
                emit_line(oss, "movsd (%%rsp), %%xmm0");
                emit_line(oss, "ucomisd %%xmm1, %%xmm0");
            }
            emit_line(oss, "addq $8, %%rsp");
        }
        else
        {
            /* Integer comparison */
            asm_visit_expression(ctx, oss, p_expression->left);
            emit_line(oss, "pushq %%rax");
            asm_visit_expression(ctx, oss, p_expression->right);
            emit_line(oss, "movq %%rax, %%rcx");
            emit_line(oss, "popq %%rax");
            emit_line(oss, "cmpq %%rcx, %%rax");
        }

        bool is_unsigned = (p_expression->left->type.type_specifier_flags & TYPE_SPECIFIER_UNSIGNED) != 0 ||
            type_is_pointer(&p_expression->left->type);
        const char* set_insn = "";
        switch (p_expression->expression_type)
        {
        case RELATIONAL_EXPRESSION_BIGGER_THAN:
            set_insn = is_unsigned ? "seta" : "setg";
            break;
        case RELATIONAL_EXPRESSION_LESS_THAN:
            set_insn = is_unsigned ? "setb" : "setl";
            break;
        case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
            set_insn = is_unsigned ? "setae" : "setge";
            break;
        case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
            set_insn = is_unsigned ? "setbe" : "setle";
            break;
        case EQUALITY_EXPRESSION_EQUAL:
            set_insn = "sete";
            break;
        case EQUALITY_EXPRESSION_NOT_EQUAL:
            set_insn = "setne";
            break;
        default:
            break;
        }
        emit_line(oss, "%s %%al", set_insn);
        emit_line(oss, "movzbl %%al, %%eax");
    }
    break;

    /* ---- Logical AND/OR (short-circuit) ---- */

    case LOGICAL_AND_EXPRESSION:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        int label_false = asm_new_label(ctx);
        int label_end = asm_new_label(ctx);

        asm_visit_expression(ctx, oss, p_expression->left);
        emit_line(oss, "testq %%rax, %%rax");
        emit_line(oss, "je .L%d", label_false);

        asm_visit_expression(ctx, oss, p_expression->right);
        emit_line(oss, "testq %%rax, %%rax");
        emit_line(oss, "je .L%d", label_false);

        emit_line(oss, "movl $1, %%eax");
        emit_line(oss, "jmp .L%d", label_end);

        emit_label(oss, label_false);
        emit_line(oss, "xorl %%eax, %%eax");

        emit_label(oss, label_end);
    }
    break;

    case LOGICAL_OR_EXPRESSION:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        int label_true = asm_new_label(ctx);
        int label_end = asm_new_label(ctx);

        asm_visit_expression(ctx, oss, p_expression->left);
        emit_line(oss, "testq %%rax, %%rax");
        emit_line(oss, "jne .L%d", label_true);

        asm_visit_expression(ctx, oss, p_expression->right);
        emit_line(oss, "testq %%rax, %%rax");
        emit_line(oss, "jne .L%d", label_true);

        emit_line(oss, "xorl %%eax, %%eax");
        emit_line(oss, "jmp .L%d", label_end);

        emit_label(oss, label_true);
        emit_line(oss, "movl $1, %%eax");

        emit_label(oss, label_end);
    }
    break;

    /* ---- Conditional (ternary) ---- */

    case CONDITIONAL_EXPRESSION:
    {
        assert(p_expression->condition_expr != NULL);
        assert(p_expression->right != NULL);

        int label_false = asm_new_label(ctx);
        int label_end = asm_new_label(ctx);

        asm_visit_expression(ctx, oss, p_expression->condition_expr);
        emit_line(oss, "testq %%rax, %%rax");
        emit_line(oss, "je .L%d", label_false);

        if (p_expression->left != NULL)
        {
            /* Normal ternary: evaluate the explicit true-branch expression */
            asm_visit_expression(ctx, oss, p_expression->left);
        }
        /* Elvis (left == NULL): condition result already in %rax, nothing to do */
        emit_line(oss, "jmp .L%d", label_end);

        emit_label(oss, label_false);
        asm_visit_expression(ctx, oss, p_expression->right);

        emit_label(oss, label_end);
    }
    break;

    /* ---- Assignment ---- */

    case ASSIGNMENT_EXPRESSION_ASSIGN:
    case ASSIGNMENT_EXPRESSION_PLUS_ASSIGN:
    case ASSIGNMENT_EXPRESSION_MINUS_ASSIGN:
    case ASSIGNMENT_EXPRESSION_MULTI_ASSIGN:
    case ASSIGNMENT_EXPRESSION_DIV_ASSIGN:
    case ASSIGNMENT_EXPRESSION_MOD_ASSIGN:
    case ASSIGNMENT_EXPRESSION_SHIFT_LEFT_ASSIGN:
    case ASSIGNMENT_EXPRESSION_SHIFT_RIGHT_ASSIGN:
    case ASSIGNMENT_EXPRESSION_AND_ASSIGN:
    case ASSIGNMENT_EXPRESSION_OR_ASSIGN:
    case ASSIGNMENT_EXPRESSION_NOT_ASSIGN:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        int size = asm_type_size(ctx, &p_expression->left->type);

        if (p_expression->expression_type == ASSIGNMENT_EXPRESSION_ASSIGN)
        {
            if (type_is_struct_or_union(&p_expression->left->type))
            {
                /* Struct assignment: memcpy */
                asm_visit_expression_addr(ctx, oss, p_expression->right);
                emit_line(oss, "pushq %%rax"); /* src */
                asm_visit_expression_addr(ctx, oss, p_expression->left);
                emit_line(oss, "movq %%rax, %%rdi"); /* dst */
                emit_line(oss, "popq %%rsi");         /* src */
                emit_line(oss, "movq $%d, %%rdx", size);
                emit_line(oss, "call memcpy");
            }
            else
            {
                asm_visit_expression(ctx, oss, p_expression->right);
                emit_line(oss, "pushq %%rax");
                asm_visit_expression_addr(ctx, oss, p_expression->left);
                emit_line(oss, "popq %%rcx");
                char suf = asm_size_suffix(size);
                emit_line(oss, "mov%c %s, (%%rax)", suf, asm_rcx_for_size(size));
                emit_line(oss, "movq %%rcx, %%rax"); /* result = assigned value */
            }
        }
        else
        {
            /* Compound assignment: load addr, load old value, compute, store */
            asm_visit_expression(ctx, oss, p_expression->right);
            emit_line(oss, "pushq %%rax"); /* rhs */
            asm_visit_expression_addr(ctx, oss, p_expression->left);
            emit_line(oss, "pushq %%rax"); /* address of lhs */

            char suf = asm_size_suffix(size);
            emit_line(oss, "mov%c (%%rax), %s", suf, asm_rax_for_size(size));
            /* rax = old value, rhs is at 8(%rsp) */
            emit_line(oss, "movq 8(%%rsp), %%rcx"); /* rhs */

            switch (p_expression->expression_type)
            {
            case ASSIGNMENT_EXPRESSION_PLUS_ASSIGN:
                emit_line(oss, "addq %%rcx, %%rax"); break;
            case ASSIGNMENT_EXPRESSION_MINUS_ASSIGN:
                emit_line(oss, "subq %%rcx, %%rax"); break;
            case ASSIGNMENT_EXPRESSION_MULTI_ASSIGN:
                emit_line(oss, "imulq %%rcx, %%rax"); break;
            case ASSIGNMENT_EXPRESSION_DIV_ASSIGN:
                emit_line(oss, "cqto");
                emit_line(oss, "idivq %%rcx"); break;
            case ASSIGNMENT_EXPRESSION_MOD_ASSIGN:
                emit_line(oss, "cqto");
                emit_line(oss, "idivq %%rcx");
                emit_line(oss, "movq %%rdx, %%rax"); break;
            case ASSIGNMENT_EXPRESSION_SHIFT_LEFT_ASSIGN:
                emit_line(oss, "salq %%cl, %%rax"); break;
            case ASSIGNMENT_EXPRESSION_SHIFT_RIGHT_ASSIGN:
                emit_line(oss, "sarq %%cl, %%rax"); break;
            case ASSIGNMENT_EXPRESSION_AND_ASSIGN:
                emit_line(oss, "andq %%rcx, %%rax"); break;
            case ASSIGNMENT_EXPRESSION_OR_ASSIGN:
                emit_line(oss, "orq %%rcx, %%rax"); break;
            case ASSIGNMENT_EXPRESSION_NOT_ASSIGN:
                emit_line(oss, "xorq %%rcx, %%rax"); break;
            default: break;
            }

            /* Store back */
            emit_line(oss, "popq %%rcx"); /* address */
            emit_line(oss, "mov%c %s, (%%rcx)", suf, asm_rax_for_size(size));
            emit_line(oss, "addq $8, %%rsp"); /* pop rhs */
        }
    }
    break;

    /* ---- Comma expression ---- */

    case EXPRESSION_EXPRESSION:
    {
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        asm_visit_expression(ctx, oss, p_expression->left);
        asm_visit_expression(ctx, oss, p_expression->right);
    }
    break;

    /* ---- Builtins ---- */
    case UNARY_EXPRESSION_GCC__BUILTIN_VA_START:
    case UNARY_EXPRESSION_GCC__BUILTIN_VA_END:
    case UNARY_EXPRESSION_GCC__BUILTIN_VA_COPY:
    case UNARY_EXPRESSION_GCC__BUILTIN_VA_ARG:
        emit_comment(oss, "va_start/va_end/va_copy/va_arg - TODO");
        break;

    case UNARY_EXPRESSION_GCC__BUILTIN_OFFSETOF:
    {
        struct osstream val_ss = { 0 };
        object_print_value(&val_ss, &p_expression->object, ctx->options.target);
        if (val_ss.c_str)
            emit_line(oss, "movq $%s, %%rax", val_ss.c_str);
        ss_close(&val_ss);
    }
    break;

    case UNARY_EXPRESSION_COUNTOF:
    case UNARY_EXPRESSION_TRAITS:
    case UNARY_EXPRESSION_IS_SAME:
    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
    case UNARY_EXPRESSION_ASSERT:
    case UNARY_EXPRESSION_CONSTEVAL:
    {
        /* Compile-time expressions - emit constant value */
        struct osstream val_ss = { 0 };
        object_print_value(&val_ss, &p_expression->object, ctx->options.target);
        if (val_ss.c_str)
            emit_line(oss, "movq $%s, %%rax", val_ss.c_str);
        else
            emit_line(oss, "xorq %%rax, %%rax");
        ss_close(&val_ss);
    }
    break;

    case PRIMARY_EXPRESSION_STATEMENT_EXPRESSION:
        emit_comment(oss, "statement expression - TODO");
        break;

    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
        emit_comment(oss, "function literal - TODO");
        break;

    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:
    {
        /* Allocate stack space, initialize, return address */
        int size = asm_type_size(ctx, &p_expression->type);
        int offset = asm_stack_alloc(ctx, size);

        /* Zero initialize */
        emit_line(oss, "leaq %d(%%rbp), %%rdi", offset);
        emit_line(oss, "xorl %%esi, %%esi");
        emit_line(oss, "movl $%d, %%edx", size);
        emit_line(oss, "call memset");

        /* TODO: actual initializer values */
        emit_line(oss, "leaq %d(%%rbp), %%rax", offset);
    }
    break;

    default:
        emit_comment(oss, "unhandled expression type %d", (int)p_expression->expression_type);
        break;
    }
}

/* =========================================================================
 *  Statement visitors
 * ========================================================================= */

static void asm_visit_expression_statement(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
    {
        asm_visit_expression(ctx, oss, p_expression_statement->expression_opt);
    }
}

static void asm_visit_jump_statement(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct jump_statement* p_jump_statement)
{
    if (p_jump_statement->first_token->type == TK_KEYWORD_CAKE_THROW)
    {
        asm_emit_defer_list(ctx, oss, &p_jump_statement->defer_list);
        emit_line(oss, "jmp .L%d", p_jump_statement->label_id);
        emit_comment(oss, "throw");
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        if (p_jump_statement->expression_opt)
        {
            struct type return_type = get_function_return_type(&ctx->p_current_function_opt->type);

            if (asm_type_is_floating(&return_type))
            {
                asm_visit_expression(ctx, oss, p_jump_statement->expression_opt);
                /* Result already in %xmm0 */
            }
            else if (type_is_struct_or_union(&return_type))
            {
                /* Return struct by hidden pointer (first arg) */
                emit_comment(oss, "return struct - TODO: hidden pointer");
                asm_visit_expression(ctx, oss, p_jump_statement->expression_opt);
            }
            else
            {
                asm_visit_expression(ctx, oss, p_jump_statement->expression_opt);
                /* Result in %rax */
            }
            type_destroy(&return_type);
        }

        asm_emit_defer_list(ctx, oss, &p_jump_statement->defer_list);
        emit_line(oss, "leave");
        emit_line(oss, "ret");
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
    {
        asm_emit_defer_list(ctx, oss, &p_jump_statement->defer_list);
        if (ctx->break_reference.break_label >= 0)
        {
            emit_line(oss, "jmp .L%d", ctx->break_reference.break_label);
            emit_comment(oss, "break");
        }
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {
        asm_emit_defer_list(ctx, oss, &p_jump_statement->defer_list);
        if (ctx->break_reference.continue_label >= 0)
        {
            emit_line(oss, "jmp .L%d", ctx->break_reference.continue_label);
            emit_comment(oss, "continue");
        }
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {
        asm_emit_defer_list(ctx, oss, &p_jump_statement->defer_list);
        emit_line(oss, "jmp %s", p_jump_statement->label->lexeme);
    }
}

static void asm_visit_label(struct asm_visit_ctx* ctx, struct osstream* oss, struct label* p_label)
{
    if (p_label->p_first_token->type == TK_KEYWORD_CASE)
    {
        emit_label(oss, p_label->label_id);
        emit_comment(oss, "case");
    }
    else if (p_label->p_first_token->type == TK_IDENTIFIER)
    {
        emit_named_label(oss, p_label->p_first_token->lexeme);
    }
    else if (p_label->p_first_token->type == TK_KEYWORD_DEFAULT)
    {
        emit_label(oss, p_label->label_id);
        emit_comment(oss, "default");
    }
}

static void asm_visit_labeled_statement(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct labeled_statement* p_labeled_statement)
{
    assert(p_labeled_statement->label != NULL);
    asm_visit_label(ctx, oss, p_labeled_statement->label);
    asm_visit_statement(ctx, oss, p_labeled_statement->statement);
}

static void asm_visit_statement(struct asm_visit_ctx* ctx, struct osstream* oss, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        asm_visit_labeled_statement(ctx, oss, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        asm_visit_unlabeled_statement(ctx, oss, p_statement->unlabeled_statement);
    }
}

static void asm_visit_selection_statement(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct selection_statement* p_selection_statement)
{
    if (p_selection_statement->first_token->type == TK_KEYWORD_IF)
    {
        assert(p_selection_statement->condition != NULL);

        /* Handle init statement (C2Y) */
        if (p_selection_statement->p_init_statement)
        {
            if (p_selection_statement->p_init_statement->p_expression_statement)
                asm_visit_expression_statement(ctx, oss, p_selection_statement->p_init_statement->p_expression_statement);
            if (p_selection_statement->p_init_statement->p_simple_declaration)
            {
                /* Visit simple declaration init */
                struct init_declarator* _Opt id = p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head;
                while (id)
                {
                    asm_visit_init_declarator(ctx, oss, id, 0, 0);
                    id = id->next;
                }
            }
        }

        int label_else = asm_new_label(ctx);
        int label_end = asm_new_label(ctx);

        /* Evaluate condition */
        if (p_selection_statement->condition->expression)
        {
            asm_visit_expression(ctx, oss, p_selection_statement->condition->expression);
            emit_line(oss, "testq %%rax, %%rax");
            emit_line(oss, "je .L%d", p_selection_statement->else_secondary_block_opt ? label_else : label_end);
        }
        else if (p_selection_statement->condition->p_init_declarator)
        {
            asm_visit_init_declarator(ctx, oss, p_selection_statement->condition->p_init_declarator, 0, 0);
            const char* name = p_selection_statement->condition->p_init_declarator->p_declarator->name_opt->lexeme;
            int offset = asm_get_local_offset(ctx, name);
            emit_line(oss, "movq %d(%%rbp), %%rax", offset);
            emit_line(oss, "testq %%rax, %%rax");
            emit_line(oss, "je .L%d", p_selection_statement->else_secondary_block_opt ? label_else : label_end);
        }

        /* If body */
        asm_visit_secondary_block(ctx, oss, p_selection_statement->secondary_block);

        if (p_selection_statement->else_secondary_block_opt)
        {
            emit_line(oss, "jmp .L%d", label_end);
            emit_label(oss, label_else);
            asm_visit_secondary_block(ctx, oss, p_selection_statement->else_secondary_block_opt);
        }

        emit_label(oss, label_end);
    }
    else if (p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
    {
        assert(p_selection_statement->condition != NULL);

        struct asm_break_reference old = ctx->break_reference;
        int break_label = asm_new_label(ctx);
        ctx->break_reference.p_selection_statement = p_selection_statement;
        ctx->break_reference.p_iteration_statement = NULL;
        ctx->break_reference.break_label = break_label;

        emit_comment(oss, "switch");

        /* Evaluate switch expression */
        if (p_selection_statement->condition->expression)
            asm_visit_expression(ctx, oss, p_selection_statement->condition->expression);

        emit_line(oss, "movq %%rax, %%rdx"); /* save switch value */

        /* Emit comparisons for each case label */
        struct label* _Opt p_label = p_selection_statement->label_list.head;
        struct label* _Opt p_label_default = NULL;
        while (p_label)
        {
            if (p_label->p_first_token->type == TK_KEYWORD_DEFAULT)
            {
                p_label_default = p_label;
            }
            else
            {
                struct osstream val_ss = { 0 };
                object_to_str(&p_label->constant_expression->object, 50, val_ss.c_str ? val_ss.c_str : "");
                char str[50] = { 0 };
                object_to_str(&p_label->constant_expression->object, 50, str);

                emit_line(oss, "cmpq $%s, %%rdx", str);
                emit_line(oss, "je .L%d", p_label->label_id);

                if (p_label->constant_expression_end)
                {
                    /* Range case */
                    char str_end[50] = { 0 };
                    object_to_str(&p_label->constant_expression_end->object, 50, str_end);
                    emit_line(oss, "cmpq $%s, %%rdx", str);
                    emit_line(oss, "jl .L_skip_%d", p_label->label_id);
                    emit_line(oss, "cmpq $%s, %%rdx", str_end);
                    emit_line(oss, "jle .L%d", p_label->label_id);
                    ss_fprintf(oss, ".L_skip_%d:\n", p_label->label_id);
                }

                ss_close(&val_ss);
            }
            p_label = p_label->next;
        }

        /* Jump to default or end */
        if (p_label_default)
            emit_line(oss, "jmp .L%d", p_label_default->label_id);
        else
            emit_line(oss, "jmp .L%d", break_label);

        /* Visit the switch body */
        asm_visit_secondary_block(ctx, oss, p_selection_statement->secondary_block);

        emit_label(oss, break_label);
        emit_comment(oss, "end switch");

        ctx->break_reference = old;
    }
}

static void asm_visit_iteration_statement(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct iteration_statement* p_iteration_statement)
{
    struct asm_break_reference old = ctx->break_reference;
    int continue_label = asm_new_label(ctx);
    int break_label = asm_new_label(ctx);
    ctx->break_reference.p_iteration_statement = p_iteration_statement;
    ctx->break_reference.p_selection_statement = NULL;
    ctx->break_reference.break_label = break_label;
    ctx->break_reference.continue_label = continue_label;

    if (p_iteration_statement->first_token->type == TK_KEYWORD_WHILE)
    {
        emit_label(oss, continue_label);
        if (p_iteration_statement->expression1)
        {
            asm_visit_expression(ctx, oss, p_iteration_statement->expression1);
            emit_line(oss, "testq %%rax, %%rax");
            emit_line(oss, "je .L%d", break_label);
        }

        asm_visit_secondary_block(ctx, oss, p_iteration_statement->secondary_block);
        emit_line(oss, "jmp .L%d", continue_label);
        emit_label(oss, break_label);
    }
    else if (p_iteration_statement->first_token->type == TK_KEYWORD_DO)
    {
        int top_label = asm_new_label(ctx);
        emit_label(oss, top_label);

        asm_visit_secondary_block(ctx, oss, p_iteration_statement->secondary_block);

        emit_label(oss, continue_label);
        assert(p_iteration_statement->expression1 != NULL);
        asm_visit_expression(ctx, oss, p_iteration_statement->expression1);
        emit_line(oss, "testq %%rax, %%rax");
        emit_line(oss, "jne .L%d", top_label);
        emit_label(oss, break_label);
    }
    else if (p_iteration_statement->first_token->type == TK_KEYWORD_FOR)
    {
        /* Init */
        if (p_iteration_statement->declaration)
        {
            asm_visit_declaration(ctx, oss, p_iteration_statement->declaration);
        }
        else if (p_iteration_statement->expression0)
        {
            asm_visit_expression(ctx, oss, p_iteration_statement->expression0);
        }

        int cond_label = asm_new_label(ctx);
        emit_label(oss, cond_label);

        /* Condition */
        if (p_iteration_statement->expression1)
        {
            asm_visit_expression(ctx, oss, p_iteration_statement->expression1);
            emit_line(oss, "testq %%rax, %%rax");
            emit_line(oss, "je .L%d", break_label);
        }

        /* Body */
        asm_visit_secondary_block(ctx, oss, p_iteration_statement->secondary_block);

        /* Continue label (before increment) */
        emit_label(oss, continue_label);

        /* Increment */
        if (p_iteration_statement->expression2)
        {
            asm_visit_expression(ctx, oss, p_iteration_statement->expression2);
        }

        emit_line(oss, "jmp .L%d", cond_label);
        emit_label(oss, break_label);
    }

    ctx->break_reference = old;
}

static void asm_visit_try_statement(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct try_statement* p_try_statement)
{
    /* Transform try/catch to labels, similar to visit_il.c */
    asm_visit_secondary_block(ctx, oss, p_try_statement->secondary_block);

    if (p_try_statement->catch_secondary_block_opt)
    {
        int end_label = asm_new_label(ctx);
        emit_line(oss, "jmp .L%d", end_label);
        emit_label(oss, p_try_statement->catch_label_id);
        asm_visit_secondary_block(ctx, oss, p_try_statement->catch_secondary_block_opt);
        emit_label(oss, end_label);
    }
}

static void asm_visit_primary_block(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct primary_block* p_primary_block)
{
    if (p_primary_block->compound_statement)
    {
        asm_visit_compound_statement(ctx, oss, p_primary_block->compound_statement);
    }
    else if (p_primary_block->iteration_statement)
    {
        asm_visit_iteration_statement(ctx, oss, p_primary_block->iteration_statement);
    }
    else if (p_primary_block->selection_statement)
    {
        asm_visit_selection_statement(ctx, oss, p_primary_block->selection_statement);
    }
    else if (p_primary_block->try_statement)
    {
        asm_visit_try_statement(ctx, oss, p_primary_block->try_statement);
    }
    else if (p_primary_block->asm_statement)
    {
        /* Inline assembly - emit as-is */
        struct token* p = p_primary_block->asm_statement->p_first_token;
        while (p)
        {
            ss_fprintf(oss, "%s", p->lexeme);
            if (p == p_primary_block->asm_statement->p_last_token)
                break;
            p = p->next;
        }
        ss_fprintf(oss, "\n");
    }
}

static void asm_visit_unlabeled_statement(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        asm_visit_primary_block(ctx, oss, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        asm_visit_expression_statement(ctx, oss, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        asm_visit_jump_statement(ctx, oss, p_unlabeled_statement->jump_statement);
    }
    else if (p_unlabeled_statement->defer_statement)
    {
        /* Defer body is emitted later at scope exit - nothing here */
    }
}

static void asm_visit_secondary_block(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct secondary_block* p_secondary_block)
{
    if (p_secondary_block->statement &&
        p_secondary_block->statement->unlabeled_statement &&
        p_secondary_block->statement->unlabeled_statement->defer_statement)
    {
        asm_visit_unlabeled_statement(ctx, oss,
            p_secondary_block->statement->unlabeled_statement->defer_statement->unlabeled_statement);
    }
    else
    {
        asm_visit_statement(ctx, oss, p_secondary_block->statement);
    }
}

static void asm_visit_block_item(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct block_item* p_block_item)
{
    if (p_block_item->declaration)
    {
        asm_visit_declaration(ctx, oss, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        asm_visit_unlabeled_statement(ctx, oss, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        asm_visit_label(ctx, oss, p_block_item->label);
    }
}

static void asm_visit_block_item_list(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct block_item_list* p_block_item_list)
{
    struct block_item* _Opt p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        asm_visit_block_item(ctx, oss, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void asm_visit_compound_statement(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct compound_statement* p_compound_statement)
{
    int saved_stack_offset = ctx->current_stack_offset;

    asm_visit_block_item_list(ctx, oss, &p_compound_statement->block_item_list);

    /* Emit defer cleanup at end of scope */
    bool ends_with_jump = false;
    if (p_compound_statement->block_item_list.tail &&
        p_compound_statement->block_item_list.tail->unlabeled_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != NULL)
    {
        ends_with_jump = true;
    }

    if (!ends_with_jump)
        asm_emit_defer_list(ctx, oss, &p_compound_statement->defer_list);

    /* Restore stack offset (local scope variables freed) */
    ctx->current_stack_offset = saved_stack_offset;
}

/* =========================================================================
 *  Function body
 * ========================================================================= */

static void asm_visit_function_body(struct asm_visit_ctx* ctx, struct osstream* oss,
    const struct declarator* function_definition)
{
    if (function_definition->function_body == NULL)
    {
        assert(false);
        return;
    }

    const struct declarator* _Opt previous_func = ctx->p_current_function_opt;
    ctx->p_current_function_opt = function_definition;
    ctx->is_in_function = true;

    /* Reset local state */
    hashmap_destroy(&ctx->local_vars);
    ctx->local_vars.capacity = 100;
    ctx->current_stack_offset = 0;

    /* Two-pass approach: first pass to collect body, second to finalize stack */
    struct osstream body = { 0 };

    /* Allocate stack space for function parameters */
    struct type func_type = function_definition->type;
    const char** regs64 = asm_arg_regs_64(ctx);
    int max_regs = asm_max_reg_args(ctx);
    int param_idx = 0;

    if (func_type.category == TYPE_CATEGORY_FUNCTION ||
        (func_type.next && func_type.next->category == TYPE_CATEGORY_FUNCTION))
    {
        /* Navigate to the function type */
        const struct type* _Opt ft = &func_type;
        while (ft && ft->category != TYPE_CATEGORY_FUNCTION)
            ft = ft->next;

        if (ft)
        {
            struct param* _Opt pa = ft->params.head;
            while (pa)
            {
                const char* param_name = pa->type.name_opt;
                if (param_name)
                {
                    int size = asm_type_size(ctx, &pa->type);
                    int offset = asm_stack_alloc(ctx, size);
                    asm_set_local_offset(ctx, param_name, offset);

                    /* Store parameter from register or stack into local */
                    if (param_idx < max_regs)
                    {
                        int store_size = (size <= 4) ? 4 : 8;
                        if (store_size <= 4)
                            emit_line(&body, "movl %s, %d(%%rbp)", asm_arg_regs_32(ctx)[param_idx], offset);
                        else
                            emit_line(&body, "movq %s, %d(%%rbp)", regs64[param_idx], offset);
                    }
                    else
                    {
                        /* Parameter was passed on the stack */
                        int param_stack_offset = 16 + (param_idx - max_regs) * 8;
                        if (asm_is_win64(ctx))
                            param_stack_offset = 16 + param_idx * 8; /* Win64 all args have home space */
                        emit_line(&body, "movq %d(%%rbp), %%rax", param_stack_offset);
                        emit_line(&body, "movq %%rax, %d(%%rbp)", offset);
                    }
                }
                param_idx++;
                pa = pa->next;
            }
        }
    }

    /* Visit function body */
    asm_visit_compound_statement(ctx, &body, function_definition->function_body);

    /* Now emit prologue with correct stack size */
    int stack_size = -ctx->current_stack_offset;
    stack_size = align_up(stack_size, 16);
    if (stack_size == 0) stack_size = 16; /* minimum frame */

    emit_line(oss, "pushq %%rbp");
    emit_line(oss, "movq %%rsp, %%rbp");
    emit_line(oss, "subq $%d, %%rsp", stack_size);

    /* Emit body */
    if (body.c_str)
        ss_fprintf(oss, "%s", body.c_str);
    ss_close(&body);

    /* Implicit return (for void functions or fall-through) */
    emit_line(oss, "leave");
    emit_line(oss, "ret");

    type_destroy(&func_type);

    ctx->p_current_function_opt = previous_func;
    ctx->is_in_function = (previous_func != NULL);
}

/* =========================================================================
 *  Declaration visitors
 * ========================================================================= */

static void asm_visit_init_declarator(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct init_declarator* p_init_declarator,
    enum function_specifier_flags function_specifier_flags,
    enum storage_class_specifier_flags storage_class_specifier_flags)
{
    const bool is_function = type_is_function(&p_init_declarator->p_declarator->type);
    const bool is_typedef = (storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF);
    const bool is_extern = (storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN);
    const bool is_static = (storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC);
    const bool is_block_scope = (storage_class_specifier_flags & STORAGE_SPECIFIER_BLOCK_SCOPE);
    const bool is_function_body = p_init_declarator->p_declarator->function_body != NULL;


    if (is_typedef) return;
    if (is_extern && !is_function_body) return; /* extern declarations - linker resolves */

    const char* name = "";
    if (p_init_declarator->p_declarator->name_opt)
        name = p_init_declarator->p_declarator->name_opt->lexeme;


    if (is_function && is_function_body)
    {
        /* Function definition */
        void* _Opt p = hashmap_find(&ctx->file_scope_declarator_map, name);
        if (p != NULL) return; /* Already emitted */

        struct hash_item_set i = { 0 };
        i.number = 1;
        hashmap_set(&ctx->file_scope_declarator_map, name, &i);
        hash_item_set_destroy(&i);

        ss_fprintf(oss, "\n");
        if (!is_static)
        {
            ss_fprintf(oss, "    .globl %s\n", name);
        }
        ss_fprintf(oss, "    .type %s, @function\n", name);
        emit_named_label(oss, name);

        asm_visit_function_body(ctx, oss, p_init_declarator->p_declarator);

        ss_fprintf(oss, "    .size %s, .-%s\n", name, name);
    }
    else if (is_block_scope && !is_static)
    {
        /* Local variable declaration */
        int size = asm_type_size(ctx, &p_init_declarator->p_declarator->type);
        int offset = asm_stack_alloc(ctx, size);
        asm_set_local_offset(ctx, name, offset);

        /* Initialize if has initializer */
        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                if (type_is_struct_or_union(&p_init_declarator->p_declarator->type))
                {
                    /* Struct initialization */
                    asm_visit_expression(ctx, oss, p_init_declarator->initializer->assignment_expression);
                    /* Source address in %rax */
                    emit_line(oss, "leaq %d(%%rbp), %%rdi", offset);
                    emit_line(oss, "movq %%rax, %%rsi");
                    emit_line(oss, "movq $%d, %%rdx", size);
                    emit_line(oss, "call memcpy");
                }
                else if (asm_type_is_floating(&p_init_declarator->p_declarator->type))
                {
                    asm_visit_expression(ctx, oss, p_init_declarator->initializer->assignment_expression);
                    if (type_is_float(&p_init_declarator->p_declarator->type))
                        emit_line(oss, "movss %%xmm0, %d(%%rbp)", offset);
                    else
                        emit_line(oss, "movsd %%xmm0, %d(%%rbp)", offset);
                }
                else
                {
                    asm_visit_expression(ctx, oss, p_init_declarator->initializer->assignment_expression);
                    char suf = asm_size_suffix(size > 8 ? 8 : size);
                    emit_line(oss, "mov%c %s, %d(%%rbp)", suf, asm_rax_for_size(size > 8 ? 8 : size), offset);
                }
            }
            else if (p_init_declarator->initializer->braced_initializer)
            {
                /* Zero-initialize first */
                emit_line(oss, "leaq %d(%%rbp), %%rdi", offset);
                emit_line(oss, "xorl %%esi, %%esi");
                emit_line(oss, "movl $%d, %%edx", size);
                emit_line(oss, "call memset");
                /* TODO: fill in non-zero initializer values */
            }
        }
    }
    else if (!is_block_scope && !is_function)
    {
        /* Global variable */
        void* _Opt p = hashmap_find(&ctx->file_scope_declarator_map, name);
        if (p != NULL) return;

        struct hash_item_set i = { 0 };
        i.number = 1;
        hashmap_set(&ctx->file_scope_declarator_map, name, &i);
        hash_item_set_destroy(&i);

        int size = asm_type_size(ctx, &p_init_declarator->p_declarator->type);

        if (p_init_declarator->initializer &&
            p_init_declarator->initializer->assignment_expression &&
            object_has_constant_value(&p_init_declarator->initializer->assignment_expression->object))
        {
            /* Initialized global -> .data section */
            if (!is_static)
                ss_fprintf(&ctx->data_section, "    .globl %s\n", name);
            ss_fprintf(&ctx->data_section, "    .align %d\n", size > 16 ? 16 : size);
            ss_fprintf(&ctx->data_section, "    .type %s, @object\n", name);
            ss_fprintf(&ctx->data_section, "    .size %s, %d\n", name, size);
            ss_fprintf(&ctx->data_section, "%s:\n", name);

            struct osstream val_ss = { 0 };
            object_print_value(&val_ss, &p_init_declarator->initializer->assignment_expression->object, ctx->options.target);
            if (val_ss.c_str)
            {
                switch (size)
                {
                case 1: ss_fprintf(&ctx->data_section, "    .byte %s\n", val_ss.c_str); break;
                case 2: ss_fprintf(&ctx->data_section, "    .value %s\n", val_ss.c_str); break;
                case 4: ss_fprintf(&ctx->data_section, "    .long %s\n", val_ss.c_str); break;
                case 8: ss_fprintf(&ctx->data_section, "    .quad %s\n", val_ss.c_str); break;
                default: ss_fprintf(&ctx->data_section, "    .zero %d\n", size); break;
                }
            }
            ss_close(&val_ss);
        }
        else if (!p_init_declarator->initializer)
        {
            /* Uninitialized global -> .bss section */
            if (!is_static)
                ss_fprintf(&ctx->bss_section, "    .globl %s\n", name);
            ss_fprintf(&ctx->bss_section, "    .align %d\n", size > 16 ? 16 : size);
            ss_fprintf(&ctx->bss_section, "    .type %s, @object\n", name);
            ss_fprintf(&ctx->bss_section, "    .size %s, %d\n", name, size);
            ss_fprintf(&ctx->bss_section, "%s:\n", name);
            ss_fprintf(&ctx->bss_section, "    .zero %d\n", size);
        }
        else
        {
            /* Non-constant initialized global - needs runtime init, put in .data with zero */
            if (!is_static)
                ss_fprintf(&ctx->data_section, "    .globl %s\n", name);
            ss_fprintf(&ctx->data_section, "    .align %d\n", size > 16 ? 16 : size);
            ss_fprintf(&ctx->data_section, "    .type %s, @object\n", name);
            ss_fprintf(&ctx->data_section, "    .size %s, %d\n", name, size);
            ss_fprintf(&ctx->data_section, "%s:\n", name);
            ss_fprintf(&ctx->data_section, "    .zero %d\n", size);
        }
    }
}

static void asm_visit_init_declarator_list(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct init_declarator_list* p_init_declarator_list,
    enum function_specifier_flags function_specifier_flags,
    enum storage_class_specifier_flags storage_class_specifier_flags)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        asm_visit_init_declarator(ctx, oss, p_init_declarator, function_specifier_flags, storage_class_specifier_flags);
        p_init_declarator = p_init_declarator->next;
    }
}

static void asm_visit_declaration(struct asm_visit_ctx* ctx, struct osstream* oss,
    struct declaration* p_declaration)
{
    if (p_declaration->init_declarator_list.head)
    {
        assert(p_declaration->declaration_specifiers != NULL);

        enum storage_class_specifier_flags storage_class_specifier_flags =
            p_declaration->declaration_specifiers->storage_class_specifier_flags;

        enum function_specifier_flags function_specifier_flags =
            p_declaration->declaration_specifiers->function_specifier_flags;

        asm_visit_init_declarator_list(ctx, oss, &p_declaration->init_declarator_list,
            function_specifier_flags, storage_class_specifier_flags);
    }
}

/* =========================================================================
 *  Main entry point
 * ========================================================================= */

void asm_visit(struct asm_visit_ctx* ctx, struct osstream* oss)
{
    /* Determine calling convention from target */
    switch (ctx->options.target)
    {
    case TARGET_X64_MSVC:
    case TARGET_X86_MSVC:
        ctx->calling_convention = ASM_CONV_WIN64;
        break;
    default:
        ctx->calling_convention = ASM_CONV_SYSV_AMD64;
        break;
    }

    ctx->break_reference.break_label = -1;
    ctx->break_reference.continue_label = -1;

    /* File header */
    ss_fprintf(oss, "# Generated by Cake %s\n", get_platform(ctx->options.target)->name);
    ss_fprintf(oss, "    .text\n");

    /* Walk all declarations - functions go to .text, globals to .data/.bss */
    struct declaration* _Opt p_declaration = ctx->ast.declaration_list.head;
    int decl_count = 0;
    while (p_declaration)
    {
        decl_count++;        
        asm_visit_declaration(ctx, oss, p_declaration);
        p_declaration = p_declaration->next;
    }

    /* Emit .rodata section (string literals, float constants) */
    if (ctx->rodata_section.size > 0)
    {
        ss_fprintf(oss, "\n    .section .rodata\n");
        ss_fprintf(oss, "%s", ctx->rodata_section.c_str);
    }

    /* Emit .data section (initialized globals) */
    if (ctx->data_section.size > 0)
    {
        ss_fprintf(oss, "\n    .data\n");
        ss_fprintf(oss, "%s", ctx->data_section.c_str);
    }

    /* Emit .bss section (uninitialized globals) */
    if (ctx->bss_section.size > 0)
    {
        ss_fprintf(oss, "\n    .bss\n");
        ss_fprintf(oss, "%s", ctx->bss_section.c_str);
    }

    ss_fprintf(oss, "\n");
}
