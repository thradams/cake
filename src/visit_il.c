//TODO 
// - array initialization
// - union
// static static
//
/*
TODO
static char mon[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
*/

/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "ownership.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include "visit_il.h"
#include "expressions.h"



/*
  Moral da historia!
  C precis ver a definicao da funcao que por sua ve precisa ver a definicao dos tipos
  vamos listar todos os tipos primeiro
  depois vamos listar os tipos por ordem de dependencia com umalgorimto que imprime as dependencias primeiro
  depois vamos listar as funcoes.

*/

struct struct_or_union_specifier* get_complete_struct_or_union_specifier2(struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    struct struct_or_union_specifier* _Opt p_complete =
        get_complete_struct_or_union_specifier(p_struct_or_union_specifier);
    if (p_complete == NULL)
        p_complete = p_struct_or_union_specifier;
    return p_complete;
}

struct struct_entry;

struct struct_entry_list
{
    struct struct_entry** data;
    int size;
    int capacity;
};



struct struct_entry
{
    bool done;
    struct struct_or_union_specifier* p_struct_or_union_specifier;
    struct struct_entry_list dependencies;
    struct struct_entry* next;
};

int struct_entry_list_reserve(struct struct_entry_list* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int struct_entry_list_push_back(struct struct_entry_list* p, struct struct_entry* book)
{
    if (p->size == INT_MAX)
    {
        return EOVERFLOW;
    }

    if (p->size + 1 > p->capacity)
    {
        int new_capacity = 0;
        if (p->capacity > (INT_MAX - p->capacity / 2))
        {
            /*overflow*/
            new_capacity = INT_MAX;
        }
        else
        {
            new_capacity = p->capacity + p->capacity / 2;
            if (new_capacity < p->size + 1)
            {
                new_capacity = p->size + 1;
            }
        }

        int error = struct_entry_list_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }



    p->data[p->size] = book; /*VIEW*/
    p->size++;

    return 0;
}

static void object_print_constant_initialization(struct d_visit_ctx* ctx, struct osstream* ss, const struct object* object, bool* first);
static void d_visit_secondary_block(struct d_visit_ctx* ctx, struct osstream* oss, struct secondary_block* p_secondary_block);
static void d_visit_init_declarator(struct d_visit_ctx* ctx, struct osstream* oss, struct init_declarator* p_init_declarator, bool binline);
static void d_visit_init_declarator_list(struct d_visit_ctx* ctx, struct osstream* oss, struct init_declarator_list* p_init_declarator_list, bool binline);
static void d_visit_compound_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct compound_statement* p_compound_statement);
static void d_visit_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct statement* p_statement);
static void d_visit_unlabeled_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct unlabeled_statement* p_unlabeled_statement);
static void object_print_non_constant_initialization(struct d_visit_ctx* ctx, struct osstream* ss, const struct object* object, const char* declarator_name);

static void d_print_type_core(struct d_visit_ctx* ctx, struct osstream* ss, const struct type* p_type0, const char* name_opt);
static void d_print_type(struct d_visit_ctx* ctx,
    struct osstream* ss,
    const struct type* p_type,
    const char* name_opt);

static void print_identation_core(struct osstream* ss, int indentation)
{
    for (int i = 0; i < indentation; i++)
    {
        ss_fprintf(ss, "    ");
    }
}

static void print_identation(const struct d_visit_ctx* ctx, struct osstream* oss)
{
    for (int i = 0; i < ctx->indentation; i++)
    {
        ss_fprintf(oss, "    ");
    }
}


static void il_print_defer_new(struct d_visit_ctx* ctx, struct osstream* oss, struct defer_list_item* p_item)
{
    if (p_item->defer_statement == NULL)
        return;

    d_visit_secondary_block(ctx, oss, p_item->defer_statement->secondary_block);
}

static int il_defer_count(struct defer_list* p_defer_list)
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

static void il_print_defer_list(struct d_visit_ctx* ctx, struct osstream* oss, struct defer_list* p_defer_list)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        il_print_defer_new(ctx, oss, p_item);
        p_item = p_item->next;
    }
}

static void object_print_value(struct osstream* ss, const struct object* a)
{
    a = object_get_referenced(a);

    switch (a->type)
    {

    case TYPE_BOOL:
        //ss_fprintf(ss, "((unsigned char) %s)", a->bool_value ? "1" : "0");
        ss_fprintf(ss, "%d", a->bool_value ? 1 : 0);
        break;

    case TYPE_SIGNED_CHAR:

        //ss_fprintf(ss, "((signed char)%d)", (int)a->signed_char_value);
        ss_fprintf(ss, "%d", (int)a->signed_char_value);
        break;


    case TYPE_UNSIGNED_CHAR:
        //ss_fprintf(ss, "((unsigned char)%d)", (int)a->unsigned_char_value);
        ss_fprintf(ss, "%d", (int)a->unsigned_char_value);
        break;


    case TYPE_SIGNED_SHORT:
        //        ss_fprintf(ss, "((short)%d)", a->signed_short_value);
        ss_fprintf(ss, "%d", a->signed_short_value);
        break;

    case TYPE_UNSIGNED_SHORT:
        //ss_fprintf(ss, "((unsigned short) %d)", a->unsigned_short_value);
        ss_fprintf(ss, "%d", a->unsigned_short_value);
        break;

    case TYPE_SIGNED_INT:
        ss_fprintf(ss, "%d", a->signed_int_value);
        break;

    case TYPE_UNSIGNED_INT:
        ss_fprintf(ss, "%u", a->unsigned_int_value);
        ss_fprintf(ss, "U");
        break;
    case TYPE_SIGNED_LONG:
        ss_fprintf(ss, "%ld", a->signed_long_value);
        ss_fprintf(ss, "L");
        break;
    case TYPE_UNSIGNED_LONG:
        ss_fprintf(ss, "%lu", a->unsigned_long_value);
        ss_fprintf(ss, "UL");
        break;
    case TYPE_SIGNED_LONG_LONG:
        ss_fprintf(ss, "%lld", a->signed_long_long_value);
        ss_fprintf(ss, "LL");
        break;
    case TYPE_UNSIGNED_LONG_LONG:
        ss_fprintf(ss, "%llu", a->unsigned_long_long_value);
        ss_fprintf(ss, "ULL");
        break;
    case TYPE_FLOAT:
        ss_fprintf(ss, "%f", a->float_value);
        ss_fprintf(ss, "f");
        break;
    case TYPE_DOUBLE:
        ss_fprintf(ss, "%lf", a->double_value);
        break;
    case TYPE_LONG_DOUBLE:
        ss_fprintf(ss, "%Lf", a->long_double_value);
        ss_fprintf(ss, "L");
        break;
    }

}
static struct member_declarator* _Opt find_member_declarator_by_index2(struct member_declaration_list* list,
    int member_index,
    char name[100],
    int* p_count)
{
    if (list->head == NULL)
        return NULL;

    int no_name_index = 0;

    struct member_declaration* _Opt p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator* _Opt p_member_declarator = NULL;

        if (p_member_declaration->member_declarator_list_opt)
        {
            p_member_declarator = p_member_declaration->member_declarator_list_opt->head;

            while (p_member_declarator)
            {
                if (p_member_declarator->declarator)
                {
                    if (member_index == *p_count)
                    {
                        snprintf(name, 100, "%s", p_member_declarator->declarator->name_opt->lexeme);
                        return p_member_declarator;
                    }
                }
                (*p_count)++;
                p_member_declarator = p_member_declarator->next;
            }
        }
        else if (p_member_declaration->specifier_qualifier_list)
        {
            if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct struct_or_union_specifier* _Opt p_complete =
                    get_complete_struct_or_union_specifier(p_member_declaration->specifier_qualifier_list->struct_or_union_specifier);


                if (p_complete)
                {
                    char mname[100];
                    p_member_declarator = find_member_declarator_by_index2(&p_complete->member_declaration_list, member_index, mname, p_count);
                    if (p_member_declarator)
                    {
                        snprintf(name, 100, "__m%d.%s", no_name_index++, mname);
                        return p_member_declarator;
                    }
                }
            }
        }

        p_member_declaration = p_member_declaration->next;
    }
    return NULL;
}

int find_member_name(const struct type* p_type, int index, char name[100])
{
    if (!type_is_struct_or_union(p_type))
        return 1;

    struct struct_or_union_specifier* _Opt p_complete =
        get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

    if (p_complete)
    {
        int count = 0;
        struct member_declarator* _Opt p =
            find_member_declarator_by_index2(&p_complete->member_declaration_list, index, name, &count);
        if (p)
        {
            return 0;//p->declarator->name_opt->lexeme;
        }
    }
    return 1;
}

static int il_visit_literal_string(struct token* current, struct osstream* oss)
{

    bool has_u8_prefix =
        current->lexeme[0] == 'u' && current->lexeme[1] == '8';

    unsigned char* psz = (unsigned char*)(current->lexeme + 0);
    if (has_u8_prefix)
    {
        psz = (unsigned char*)(current->lexeme + 2);
    }

    while (*psz)
    {
        if (*psz >= 128)
        {
            ss_fprintf(oss, "\\x%x", *psz);
        }
        else
        {
            ss_fprintf(oss, "%c", *psz);
        }
        psz++;
    }


    return 0;
}

static void d_visit_expression(struct d_visit_ctx* ctx, struct osstream* oss, struct expression* p_expression)
{
    switch (p_expression->expression_type)
    {
    case EXPRESSION_TYPE_INVALID:
        assert(false);
        break;

    case PRIMARY_EXPRESSION__FUNC__:
        break;

    case PRIMARY_EXPRESSION_DECLARATOR:
    {
        ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);

        if (type_is_function(&p_expression->declarator->type) ||
            (p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN))
        {
            const char* func_name = NULL;

            func_name = p_expression->first_token->lexeme;
            struct map_entry* p = hashmap_find(&ctx->function_map, func_name);
            if (p == NULL)
            {
                /*first time we see it*/

                struct hash_item_set i = { 0 };
                i.number = 1;
                hashmap_set(&ctx->function_map, func_name, &i);
                struct osstream ss = { 0 };

                d_print_type(ctx, &ss, &p_expression->type, func_name);

                if (type_is_function(&p_expression->declarator->type))
                {
                    if (p_expression->declarator->function_body &&
                        p_expression->declarator->declaration_specifiers &&
                        p_expression->declarator->declaration_specifiers->function_specifier_flags & FUNCTION_SPECIFIER_INLINE)
                    {
                        ss_fprintf(&ctx->function_types, "\n");
                        struct osstream copy = *oss;
                        oss->c_str = 0;
                        oss->capacity = 0;
                        oss->size = 0;
                        int i2 = ctx->indentation;
                        ctx->indentation = 0;
                        d_visit_compound_statement(ctx, oss, p_expression->declarator->function_body);
                        ctx->indentation = i2;


                        ss_fprintf(&ctx->function_types, "inline %s\n", ss.c_str);
                        ss_fprintf(&ctx->function_types, "%s", oss->c_str);
                        ss_close(oss);
                        *oss = copy;
                    }
                    else
                    {
                        ss_fprintf(&ctx->function_types, "%s;\n", ss.c_str);
                    }
                }
                else
                {
                    ss_fprintf(&ctx->function_types, "%s;\n", ss.c_str);
                }

                ss_close(&ss);
            }
        }
    }
    break;

    case PRIMARY_EXPRESSION_STRING_LITERAL:
        il_visit_literal_string(p_expression->first_token, oss);
        break;

    case PRIMARY_EXPRESSION_ENUMERATOR:
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
    case PRIMARY_EXPRESSION_NUMBER:
    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        object_print_value(oss, &p_expression->object);
        break;

    case PRIMARY_EXPRESSION_PARENTESIS:
        assert(p_expression->right != NULL);
        ss_fprintf(oss, "(");
        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, ")");
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        assert(p_expression->generic_selection != NULL);

        if (p_expression->generic_selection->p_view_selected_expression)
        {
            d_visit_expression(ctx, oss, p_expression->generic_selection->p_view_selected_expression);
        }
        break;

    case POSTFIX_DOT:
    {
        d_visit_expression(ctx, oss, p_expression->left);

        char name[100];
        int r = find_member_name(&p_expression->left->type, p_expression->member_index, name);
        if (r == 0)
        {
            ss_fprintf(oss, ".%s", name);
        }
    }
    break;

    case POSTFIX_ARROW:
    {
        d_visit_expression(ctx, oss, p_expression->left);
        {
            struct type t = type_remove_pointer(&p_expression->left->type);

            char name[100];
            int r = find_member_name(&t, p_expression->member_index, name);
            if (r == 0)
            {
                ss_fprintf(oss, "->%s", name);
            }
            type_destroy(&t);
        }
    }
    break;


    case POSTFIX_INCREMENT:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "++");
        break;

    case POSTFIX_DECREMENT:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "--");
        break;

    case POSTFIX_ARRAY:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "[");
        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, "]");
        break;

    case POSTFIX_FUNCTION_CALL:
    {
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "(");
        struct argument_expression* _Opt arg = p_expression->argument_expression_list.head;
        while (arg)
        {
            d_visit_expression(ctx, oss, arg->expression);
            if (arg->next)
                ss_fprintf(oss, ", ");
            arg = arg->next;
        }
        ss_fprintf(oss, ")");
    }
    break;

    case UNARY_EXPRESSION_ADDRESSOF:
        ss_fprintf(oss, "&");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
    {
        char name[100];
        snprintf(name, sizeof(name), "_lambda_%d", ctx->extern_count++);

        print_identation_core(&ctx->add_this_before, ctx->indentation);

        struct osstream lambda = { 0 };
        ss_fprintf(&lambda, "static ");
        d_print_type(ctx, &lambda, &p_expression->type, name);
        ss_fprintf(&lambda, "\n");
        int current_indentation = ctx->indentation;
        ctx->indentation = 0;
        d_visit_compound_statement(ctx, &lambda, p_expression->compound_statement);
        ctx->indentation = current_indentation;
        ss_fprintf(&ctx->add_this_before_external_decl, "%s\n", lambda.c_str);
        ss_fprintf(oss, "%s", name);
    }
    break;


    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:
    {
        char name[100];
        snprintf(name, sizeof(name), "__cmp_lt_%d", ctx->locals_count++);


        print_identation_core(&ctx->add_this_before, ctx->indentation);
        d_print_type(ctx, &ctx->add_this_before, &p_expression->type, name);
        bool first = true;
        ss_fprintf(&ctx->add_this_before, " = {");
        object_print_constant_initialization(ctx, &ctx->add_this_before, &p_expression->object, &first);
        ss_fprintf(&ctx->add_this_before, "};\n");

        object_print_non_constant_initialization(ctx, &ctx->add_this_before, &p_expression->object, name);

        ss_fprintf(oss, "%s", name);
    }
    break;

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:
        ss_fprintf(oss, "sizeof ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_SIZEOF_TYPE:
    {
        struct osstream local0 = { 0 };
        d_print_type(ctx, &local0, &p_expression->type_name->type, false);
        ss_fprintf(oss, "sizeof (%s)", local0.c_str);
        ss_close(&local0);
    }
    break;

    case UNARY_EXPRESSION_ALIGNOF:
    case UNARY_EXPRESSION_NELEMENTSOF_TYPE:


        object_print_value(oss, &p_expression->object);
        break;

    case UNARY_EXPRESSION_INCREMENT:
        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, " ++");
        assert(false); ///lado?
        break;

    case UNARY_EXPRESSION_DECREMENT:

        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, " --");
        assert(false); ///lado?
        break;

    case UNARY_EXPRESSION_NOT:
        ss_fprintf(oss, " !");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case UNARY_EXPRESSION_BITNOT:
        ss_fprintf(oss, " ~");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case UNARY_EXPRESSION_NEG:
        ss_fprintf(oss, " -");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case UNARY_EXPRESSION_PLUS:
        ss_fprintf(oss, " +");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_CONTENT:
        ss_fprintf(oss, " *");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_ASSERT:
        break;

    case ADDITIVE_EXPRESSION_MINUS:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " - ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case ADDITIVE_EXPRESSION_PLUS:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " + ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case MULTIPLICATIVE_EXPRESSION_MULT:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " * ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case MULTIPLICATIVE_EXPRESSION_DIV:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " / ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case MULTIPLICATIVE_EXPRESSION_MOD:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "%s", " % ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;


    case EXPRESSION_EXPRESSION:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, ", ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case ASSIGNMENT_EXPRESSION:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " = ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case CAST_EXPRESSION:

    {
        struct osstream local2 = { 0 };
        d_print_type(ctx, &local2, &p_expression->type, NULL);
        ss_fprintf(oss, "(%s)", local2.c_str);
        ss_close(&local2);
        d_visit_expression(ctx, oss, p_expression->left);
    }
    break;

    case SHIFT_EXPRESSION_RIGHT:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " >> ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case SHIFT_EXPRESSION_LEFT:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " << ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case RELATIONAL_EXPRESSION_BIGGER_THAN:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " > ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case RELATIONAL_EXPRESSION_LESS_THAN:

        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " < ");
        d_visit_expression(ctx, oss, p_expression->right);

        break;
    case EQUALITY_EXPRESSION_EQUAL:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " == ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case EQUALITY_EXPRESSION_NOT_EQUAL:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " != ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case AND_EXPRESSION:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " & ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case EXCLUSIVE_OR_EXPRESSION:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " ^ ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case INCLUSIVE_OR_EXPRESSION:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " | ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " <= ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " >= ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case LOGICAL_AND_EXPRESSION:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " && ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case LOGICAL_OR_EXPRESSION:
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " || ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_TRAITS:
    case UNARY_EXPRESSION_IS_SAME:
    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
        break;

    case CONDITIONAL_EXPRESSION:

        d_visit_expression(ctx, oss, p_expression->condition_expr);
        ss_fprintf(oss, " ? ");
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " : ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    }
}


static void d_visit_declaration(struct d_visit_ctx* ctx, struct osstream* oss, struct declaration* p_declaration);

static void d_visit_declarator(struct d_visit_ctx* ctx, struct osstream* oss, struct declarator* p_declarator)
{

}

static void d_visit_expression_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct expression_statement* p_expression_statement)
{

    ss_clear(&ctx->add_this_before);
    struct osstream local = { 0 };

    print_identation(ctx, &local);
    if (p_expression_statement->expression_opt)
        d_visit_expression(ctx, &local, p_expression_statement->expression_opt);


    if (ctx->add_this_before.size > 0)
    {
        ss_fprintf(oss, "%s", ctx->add_this_before.c_str);
        ss_clear(&ctx->add_this_before);

    }

    ss_fprintf(oss, "%s;\n", local.c_str);
    ss_close(&local);
}

static void d_visit_jump_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct jump_statement* p_jump_statement)
{



    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
    {
        il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
        print_identation(ctx, oss);
        ss_fprintf(oss, "goto _catch_label_%d;\n", p_jump_statement->try_catch_block_index);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {

        //if the return does not have expression, or if it returns a constant
        //we know that is safe do execute defer before returning
        const bool constant_expression_or_void =
            p_jump_statement->expression_opt == NULL ||
            object_has_constant_value(&p_jump_statement->expression_opt->object) ||
            il_defer_count(&p_jump_statement->defer_list) == 0;

        if (constant_expression_or_void)
        {
            il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
            print_identation(ctx, oss);
            ss_fprintf(oss, "return");

            if (p_jump_statement->expression_opt)
            {
                ss_fprintf(oss, " ");
                d_visit_expression(ctx, oss, p_jump_statement->expression_opt);
            }
            ss_fprintf(oss, ";\n");
        }
        else
        {
            //Interleaved return like in
            //
            // int f() {
            //    int i = 1;
            //    defer { i = 0; }
            //    return i++;
            // }

            char name[100] = { 0 };
            snprintf(name, sizeof name, "__ck%d", ctx->locals_count++);


            print_identation(ctx, oss);
            d_print_type(ctx, oss, &p_jump_statement->expression_opt->type, name);
            ss_fprintf(oss, " = ");
            if (p_jump_statement->expression_opt)
                d_visit_expression(ctx, oss, p_jump_statement->expression_opt);
            ss_fprintf(oss, ";\n");

            il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
            print_identation(ctx, oss);

            ss_fprintf(oss, "return %s;\n", name);
        }
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {
        il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
        print_identation(ctx, oss);

        if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
            ss_fprintf(oss, "break;\n");
        else
            ss_fprintf(oss, "continue;\n");
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {
        il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
        print_identation(ctx, oss);
        ss_fprintf(oss, "goto %s;\n", p_jump_statement->label->lexeme);
    }
    else
    {
        assert(false);
    }
}

static void d_visit_labeled_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct labeled_statement* p_labeled_statement)
{
    if (p_labeled_statement->label)
    {
        //p_labeled_statement->label->constant_expression
    }
    d_visit_statement(ctx, oss, p_labeled_statement->statement);
}

static void d_visit_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        d_visit_labeled_statement(ctx, oss, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        d_visit_unlabeled_statement(ctx, oss, p_statement->unlabeled_statement);
    }
}

static void d_visit_secondary_block(struct d_visit_ctx* ctx, struct osstream* oss, struct secondary_block* p_secondary_block)
{
    d_visit_statement(ctx, oss, p_secondary_block->statement);
}

static void d_visit_iteration_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct iteration_statement* p_iteration_statement)
{
    print_identation(ctx, oss);
    if (p_iteration_statement->first_token->type == TK_KEYWORD_WHILE)
    {
        ss_fprintf(oss, "while ("); //one statement per line

        if (p_iteration_statement->expression1)
            d_visit_expression(ctx, oss, p_iteration_statement->expression1);

        ss_fprintf(oss, ")\n");
        d_visit_secondary_block(ctx, oss, p_iteration_statement->secondary_block);
    }
    else if (p_iteration_statement->first_token->type == TK_KEYWORD_DO)
    {
        ss_fprintf(oss, "do\n");

        d_visit_secondary_block(ctx, oss, p_iteration_statement->secondary_block);

        print_identation(ctx, oss);
        ss_fprintf(oss, "while (");

        d_visit_expression(ctx, oss, p_iteration_statement->expression1);

        ss_fprintf(oss, ");\n");
    }
    else if (p_iteration_statement->first_token->type == TK_KEYWORD_FOR)
    {
        if (p_iteration_statement->declaration)
        {
            ss_fprintf(oss, "{\n");
            ctx->indentation++;

            d_visit_declaration(ctx, oss, p_iteration_statement->declaration);

            print_identation(ctx, oss);
            ss_fprintf(oss, "for (");

        }
        else if (p_iteration_statement->expression0)
        {
            ss_fprintf(oss, "for (");
            d_visit_expression(ctx, oss, p_iteration_statement->expression0);
        }
        else
        {
            ss_fprintf(oss, "for (");
        }

        ss_fprintf(oss, "; ");

        if (p_iteration_statement->expression1)
            d_visit_expression(ctx, oss, p_iteration_statement->expression1);

        ss_fprintf(oss, "; ");

        if (p_iteration_statement->expression2)
            d_visit_expression(ctx, oss, p_iteration_statement->expression2);

        ss_fprintf(oss, ")\n");

        d_visit_secondary_block(ctx, oss, p_iteration_statement->secondary_block);

        if (p_iteration_statement->declaration)
        {
            ctx->indentation--;
            print_identation(ctx, oss);
            ss_fprintf(oss, "}\n");
        }
    }
}

static void d_visit_simple_declaration(struct d_visit_ctx* ctx, struct osstream* oss, struct simple_declaration* p_simple_declaration)
{
    d_visit_init_declarator_list(ctx, oss, &p_simple_declaration->init_declarator_list, false);
}

static void d_visit_init_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct init_statement* p_init_statement)
{
    if (p_init_statement->p_expression_statement)
        d_visit_expression_statement(ctx, oss, p_init_statement->p_expression_statement);
    if (p_init_statement->p_simple_declaration)
        d_visit_simple_declaration(ctx, oss, p_init_statement->p_simple_declaration);
}

static void d_visit_condition(struct d_visit_ctx* ctx, struct osstream* oss, struct condition* p_condition)
{
    if (p_condition->p_init_declarator)
        d_visit_init_declarator(ctx, oss, p_condition->p_init_declarator, false);

    if (p_condition->expression)
        d_visit_expression(ctx, oss, p_condition->expression);
}

static bool is_compound_statement(struct secondary_block* p_secondary_block)
{

    if (p_secondary_block->statement &&
        p_secondary_block->statement->unlabeled_statement &&
        p_secondary_block->statement->unlabeled_statement->primary_block &&
        p_secondary_block->statement->unlabeled_statement->primary_block->compound_statement)
    {
        return true;
    }

    return false;
}

static void d_visit_selection_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct selection_statement* p_selection_statement)
{
    print_identation(ctx, oss);
    if (p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
    {
        ss_fprintf(oss, "switch (");
        d_visit_condition(ctx, oss, p_selection_statement->condition);
        ss_fprintf(oss, ")\n");
        d_visit_secondary_block(ctx, oss, p_selection_statement->secondary_block);
    }
    else if (p_selection_statement->first_token->type == TK_KEYWORD_IF)
    {
        bool addclose = false;
        if (p_selection_statement->p_init_statement)
        {
            ss_fprintf(oss, "{\n");
            addclose = true;
            ctx->indentation++;
            d_visit_init_statement(ctx, oss, p_selection_statement->p_init_statement);
            print_identation(ctx, oss);
        }

        if (p_selection_statement->condition)
        {
            if (p_selection_statement->condition->p_init_declarator)
            {
                ss_fprintf(oss, "{\n");
                ctx->indentation++;
                addclose = true;
                d_visit_init_declarator(ctx, oss, p_selection_statement->condition->p_init_declarator, false);
                print_identation(ctx, oss);
                ss_fprintf(oss, "if (");
                ss_fprintf(oss, "%s", p_selection_statement->condition->p_init_declarator->p_declarator->name_opt->lexeme);
                ss_fprintf(oss, ")\n");

            }
            else if (p_selection_statement->condition->expression)
            {
                ss_fprintf(oss, "if (");
                d_visit_expression(ctx, oss, p_selection_statement->condition->expression);
                ss_fprintf(oss, ")\n");
            }

        }

        if (p_selection_statement->secondary_block)
        {
            if (is_compound_statement(p_selection_statement->secondary_block))
            {
                d_visit_secondary_block(ctx, oss, p_selection_statement->secondary_block);
            }
            else
            {
                print_identation(ctx, oss);
                ss_fprintf(oss, "{\n");
                ctx->indentation++;
                d_visit_secondary_block(ctx, oss, p_selection_statement->secondary_block);
                ctx->indentation--;
                print_identation(ctx, oss);
                ss_fprintf(oss, "}\n");
            }
        }

        if (p_selection_statement->else_secondary_block_opt)
        {
            print_identation(ctx, oss);
            ss_fprintf(oss, "else\n");

            if (is_compound_statement(p_selection_statement->else_secondary_block_opt))
            {
                d_visit_secondary_block(ctx, oss, p_selection_statement->else_secondary_block_opt);
            }
            else
            {
                print_identation(ctx, oss);
                ss_fprintf(oss, "{\n");
                ctx->indentation++;
                d_visit_secondary_block(ctx, oss, p_selection_statement->else_secondary_block_opt);
                ctx->indentation--;
                print_identation(ctx, oss);
                ss_fprintf(oss, "}\n");
            }
        }

        if (addclose)
        {
            ctx->indentation--;
            print_identation(ctx, oss);
            ss_fprintf(oss, "}\n");

        }
    }


}

static void d_visit_try_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct try_statement* p_try_statement)
{
    print_identation(ctx, oss);
    ss_fprintf(oss, "if (1)\n");

    d_visit_secondary_block(ctx, oss, p_try_statement->secondary_block);

    print_identation(ctx, oss);
    ss_fprintf(oss, "else _catch_label_%d:\n", p_try_statement->try_catch_block_index);

    if (p_try_statement->catch_secondary_block_opt)
    {
        d_visit_secondary_block(ctx, oss, p_try_statement->catch_secondary_block_opt);
    }
}

static void d_visit_primary_block(struct d_visit_ctx* ctx, struct osstream* oss, struct primary_block* p_primary_block)
{
    if (p_primary_block->defer_statement)
    {
        //visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else if (p_primary_block->compound_statement)
    {
        d_visit_compound_statement(ctx, oss, p_primary_block->compound_statement);
    }
    else if (p_primary_block->iteration_statement)
    {
        d_visit_iteration_statement(ctx, oss, p_primary_block->iteration_statement);
    }
    else if (p_primary_block->selection_statement)
    {
        d_visit_selection_statement(ctx, oss, p_primary_block->selection_statement);
    }
    else if (p_primary_block->try_statement)
    {
        d_visit_try_statement(ctx, oss, p_primary_block->try_statement);
    }

}

static void d_visit_unlabeled_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct unlabeled_statement* p_unlabeled_statement)
{
    //print_identation(ctx, oss);
    if (p_unlabeled_statement->primary_block)
    {
        d_visit_primary_block(ctx, oss, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        d_visit_expression_statement(ctx, oss, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        d_visit_jump_statement(ctx, oss, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
    //ss_fprintf(oss, "\n"); //one statement per line
}

static void d_visit_label(struct d_visit_ctx* ctx, struct osstream* oss, struct label* p_label)
{
    if (p_label->p_first_token->type == TK_KEYWORD_CASE)
    {
        print_identation(ctx, oss);
        ss_fprintf(oss, "case ");

        object_print_value(oss, &p_label->constant_expression->object);

        ss_fprintf(oss, " :\n");
    }
    else if (p_label->p_first_token->type == TK_IDENTIFIER)
    {
        print_identation(ctx, oss);
        ss_fprintf(oss, "%s:\n", p_label->p_first_token->lexeme);
    }
    else if (p_label->p_first_token->type == TK_KEYWORD_DEFAULT)
    {
        print_identation(ctx, oss);
        ss_fprintf(oss, "default:\n");
    }

}

static void d_visit_block_item(struct d_visit_ctx* ctx, struct osstream* oss, struct block_item* p_block_item)
{
    struct osstream ss0 = { 0 };
    ss_swap(oss, &ss0);

    ss_clear(&ctx->add_this_before);

    if (p_block_item->declaration)
    {
        d_visit_declaration(ctx, oss, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        d_visit_unlabeled_statement(ctx, oss, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        d_visit_label(ctx, oss, p_block_item->label);
    }

    if (ctx->add_this_before.size > 0)
    {
        ss_fprintf(&ss0, "%s", ctx->add_this_before.c_str);
        ss_clear(&ctx->add_this_before);

    }
    if (oss->c_str)
    {
        ss_fprintf(&ss0, "%s", oss->c_str);
    }
    ss_swap(oss, &ss0);
    ss_close(&ss0);
}

static void d_visit_block_item_list(struct d_visit_ctx* ctx, struct osstream* oss, struct block_item_list* p_block_item_list)
{
    struct block_item* _Opt p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        d_visit_block_item(ctx, oss, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void d_visit_compound_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct compound_statement* p_compound_statement)
{
    print_identation(ctx, oss);
    ss_fprintf(oss, "{\n");
    ctx->indentation++;
    d_visit_block_item_list(ctx, oss, &p_compound_statement->block_item_list);


    bool ends_with_jump = false;

    if (p_compound_statement->block_item_list.tail &&
        p_compound_statement->block_item_list.tail->unlabeled_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != NULL)
    {
        ends_with_jump = true;
    }

    if (!ends_with_jump)
        il_print_defer_list(ctx, oss, &p_compound_statement->defer_list);

    ctx->indentation--;
    print_identation(ctx, oss);
    ss_fprintf(oss, "}\n");
}

//struct struct_entry
static void register_struct_types_and_functions(struct d_visit_ctx* ctx, const struct type* p_type0)
{
    try
    {
        const struct type* _Opt p_type = p_type0;

        while (p_type)
        {
            switch (p_type->category)
            {
            case TYPE_CATEGORY_ITSELF:
            {
                if (p_type->struct_or_union_specifier)
                {
                    struct struct_or_union_specifier* _Opt p_complete =
                        get_complete_struct_or_union_specifier2(p_type->struct_or_union_specifier);


                    if (p_complete)
                    {
                        char name[100];
                        snprintf(name, sizeof name, "%p", (void*)p_complete);

                        struct map_entry* p = hashmap_find(&ctx->structs_map, name);
                        if (p == NULL)
                        {
                            //vamos ver se ja existe alguma com o mesmo tag?
                            //se existir vamos mudar o tagname desta nova adicinando um numero.

                            struct map_entry* p_name = hashmap_find(&ctx->tag_names, p_complete->tag_name);
                            if (p_name != NULL)
                            {
                                //ja existe uma com este nome
                                char new_name[100];
                                snprintf(new_name, sizeof name, "%s%d", p_complete->tag_name, ctx->tag_name_count++);
                                snprintf(p_complete->tag_name, sizeof p_complete->tag_name, "%s", new_name);

                                struct hash_item_set i = { 0 };
                                i.number = 1;
                                hashmap_set(&ctx->tag_names, new_name, &i);
                            }
                            else
                            {
                                struct hash_item_set i = { 0 };
                                i.number = 1;
                                hashmap_set(&ctx->tag_names, p_complete->tag_name, &i);
                            }

                            struct struct_entry* _Opt _Owner p_struct_entry = calloc(1, sizeof * p_struct_entry);
                            if (p_struct_entry == NULL)
                                throw;

                            p_struct_entry->p_struct_or_union_specifier = p_complete;

                            struct hash_item_set i = { 0 };
                            i.p_struct_entry = p_struct_entry;
                            hashmap_set(&ctx->structs_map, name, &i);

                            struct member_declaration* _Opt member_declaration =
                                p_complete->member_declaration_list.head;

                            while (member_declaration)
                            {
                                struct member_declarator* _Opt member_declarator = NULL;

                                if (member_declaration->member_declarator_list_opt)
                                {
                                    member_declarator = member_declaration->member_declarator_list_opt->head;
                                    while (member_declarator)
                                    {
                                        if (type_is_struct_or_union(&member_declarator->declarator->type))
                                        {
                                            struct struct_or_union_specifier* _Opt p_complete_member =
                                                p_complete_member = get_complete_struct_or_union_specifier(member_declarator->declarator->type.struct_or_union_specifier);

                                            char name2[100];
                                            snprintf(name2, sizeof name2, "%p", (void*)p_complete_member);

                                            register_struct_types_and_functions(ctx, &member_declarator->declarator->type);
                                            struct map_entry* p2 = hashmap_find(&ctx->structs_map, name2);
                                            if (p2 != NULL)
                                            {
                                                struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                            }
                                        }
                                        if (type_is_array(&member_declarator->declarator->type))
                                        {
                                            struct type t = get_array_item_type(&member_declarator->declarator->type);
                                            if (type_is_struct_or_union(&t))
                                            {
                                                struct struct_or_union_specifier* _Opt p_complete_member =
                                                    p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);

                                                char name2[100];
                                                snprintf(name2, sizeof name2, "%p", (void*)p_complete_member);

                                                register_struct_types_and_functions(ctx, &t);
                                                struct map_entry* p2 = hashmap_find(&ctx->structs_map, name2);
                                                if (p2 != NULL)
                                                {
                                                    struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                                }
                                            }
                                            else
                                            {
                                                register_struct_types_and_functions(ctx, &member_declarator->declarator->type);
                                            }
                                            type_destroy(&t);
                                        }
                                        else
                                        {
                                            register_struct_types_and_functions(ctx, &member_declarator->declarator->type);
                                        }

                                        member_declarator = member_declarator->next;
                                    }
                                }
                                else if (member_declaration->specifier_qualifier_list != NULL)
                                {
                                    if (member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                                    {
                                        struct type t = { 0 };
                                        t.category = TYPE_CATEGORY_ITSELF;
                                        t.struct_or_union_specifier = member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                                        //char buffer[200] = { 0 };
                                        //snprintf(buffer, sizeof buffer, ".%s", name);

                                        if (type_is_struct_or_union(&t))
                                        {
                                            struct struct_or_union_specifier* _Opt p_complete_member =
                                                p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);

                                            char name2[100];
                                            snprintf(name2, sizeof name2, "%p", (void*)p_complete_member);

                                            register_struct_types_and_functions(ctx, &t);
                                            struct map_entry* p2 = hashmap_find(&ctx->structs_map, name2);
                                            if (p2 != NULL)
                                            {
                                                struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                            }
                                        }
                                        if (type_is_array(&t))
                                        {
                                            struct type t = get_array_item_type(&t);
                                            if (type_is_struct_or_union(&t))
                                            {
                                                struct struct_or_union_specifier* _Opt p_complete_member =
                                                    p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);

                                                char name2[100];
                                                snprintf(name2, sizeof name2, "%p", (void*)p_complete_member);

                                                register_struct_types_and_functions(ctx, &t);
                                                struct map_entry* p2 = hashmap_find(&ctx->structs_map, name2);
                                                if (p2 != NULL)
                                                {
                                                    struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                                }
                                            }
                                            else
                                            {
                                                register_struct_types_and_functions(ctx, &t);
                                            }
                                            type_destroy(&t);
                                        }
                                        else
                                        {
                                            register_struct_types_and_functions(ctx, &t);
                                        }

                                        type_destroy(&t);
                                    }
                                }

                                member_declaration = member_declaration->next;
                            }
                        }
                    }
                }
                else
                {
                }
            }
            break;

            case TYPE_CATEGORY_FUNCTION:
            {
                struct param* _Opt pa = p_type->params.head;
                while (pa)
                {
                    register_struct_types_and_functions(ctx, &pa->type);
                    pa = pa->next;
                }
            }
            break;

            case TYPE_CATEGORY_ARRAY:
            case TYPE_CATEGORY_POINTER:

                break;
            }
            p_type = p_type->next;
        }
    }
    catch
    {
    }
}

static void d_print_type_core(struct d_visit_ctx* ctx,
    struct osstream* ss,
    const struct type* p_type0,
    const char* name_opt)
{
    const struct type* _Opt p_type = p_type0;


    bool previous_was_pointer_to = false;

    while (p_type)
    {
        switch (p_type->category)
        {
        case TYPE_CATEGORY_ITSELF:
        {
            struct osstream local = { 0 };
            bool first = true;

            if (p_type->struct_or_union_specifier)
            {
                if (struct_or_union_specifier_is_union(p_type->struct_or_union_specifier))
                    ss_fprintf(&local, "union %s", p_type->struct_or_union_specifier->tag_name);
                else
                    ss_fprintf(&local, "struct %s", p_type->struct_or_union_specifier->tag_name);
            }
            else if (p_type->enum_specifier)
            {

                const struct enum_specifier* p_enum_specifier =
                    get_complete_enum_specifier(p_type->enum_specifier);

                if (p_enum_specifier &&
                    p_enum_specifier->specifier_qualifier_list)
                {
                    print_type_specifier_flags(&local,
                        &first,
                        p_enum_specifier->specifier_qualifier_list->type_specifier_flags);
                }
                else
                {
                    ss_fprintf(&local, "int ");
                }
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
            {
                ss_fprintf(&local, "unsigned char ");
            }
            else
            {
                print_type_specifier_flags(&local, &first, p_type->type_specifier_flags);
            }

            if (name_opt)
            {
                if (first)
                {
                    ss_fprintf(ss, " ");
                    first = false;
                }
                ss_fprintf(ss, "%s", name_opt);
                name_opt = NULL;
            }

            struct osstream local2 = { 0 };
            if (ss->c_str)
                ss_fprintf(&local2, "%s %s", local.c_str, ss->c_str);
            else
                ss_fprintf(&local2, "%s", local.c_str);

            ss_swap(ss, &local2);
            //ss_close(&local);
            ss_close(&local2);
        }
        previous_was_pointer_to = false;
        break;
        case TYPE_CATEGORY_ARRAY:


            if (name_opt)
            {
                //if (first)
                //{
                  //  ss_fprintf(ss, " ");
                    //first = false;
                //}
                ss_fprintf(ss, "%s", name_opt);
                name_opt = NULL;
            }

            ss_fprintf(ss, "[");

            bool b = true;


            //print_type_qualifier_flags(ss, &b, p_type->type_qualifier_flags);

            if (p_type->num_of_elements > 0)
            {
                if (!b)
                    ss_fprintf(ss, " ");

                ss_fprintf(ss, "%d", p_type->num_of_elements);
            }
            ss_fprintf(ss, "]");
            previous_was_pointer_to = false;
            break;

        case TYPE_CATEGORY_FUNCTION:

            if (name_opt)
            {
                //if (first)
                //{
                  //  ss_fprintf(ss, " ");
                    //first = false;
                //}
                ss_fprintf(ss, "%s", name_opt);
                name_opt = NULL;
            }
            ss_fprintf(ss, "(");

            struct param* _Opt pa = p_type->params.head;

            while (pa)
            {
                struct osstream sslocal = { 0 };
                struct osstream local2 = { 0 };
                d_print_type_core(ctx, &local2, &pa->type, pa->type.name_opt);

                ss_fprintf(&sslocal, "%s", local2.c_str);
                ss_fprintf(ss, "%s", sslocal.c_str);
                if (pa->next)
                    ss_fprintf(ss, ", ");
                ss_close(&sslocal);
                pa = pa->next;
            }

            if (p_type->params.is_var_args)
            {
                if (p_type->params.head)
                    ss_fprintf(ss, ", ");
                ss_fprintf(ss, "...");
            }

            ss_fprintf(ss, ")");

            previous_was_pointer_to = false;
            break;

        case TYPE_CATEGORY_POINTER:
        {
            struct osstream local = { 0 };
            if (p_type->next && (
                (p_type->next->category == TYPE_CATEGORY_FUNCTION ||
                    p_type->next->category == TYPE_CATEGORY_ARRAY)))
            {
                ss_fprintf(&local, "(");
            }

            ss_fprintf(&local, "*");
            bool first = false;

            if (name_opt)
            {
                if (!first)
                {
                    ss_fprintf(ss, " ");
                }
                ss_fprintf(ss, "%s", name_opt);
                name_opt = NULL;
                first = false;
            }

            if (ss->c_str)
                ss_fprintf(&local, "%s", ss->c_str);

            if (p_type->next &&
                (p_type->next->category == TYPE_CATEGORY_FUNCTION ||
                    p_type->next->category == TYPE_CATEGORY_ARRAY))
            {
                ss_fprintf(&local, ")", ss->c_str);
            }

            ss_swap(ss, &local);
            ss_close(&local);
            previous_was_pointer_to = true;
        }
        break;
        }

        p_type = p_type->next;

    }
}

static void d_print_type(struct d_visit_ctx* ctx,
    struct osstream* ss,
    const struct type* p_type,
    const char* name_opt)
{

    //Register structs

    register_struct_types_and_functions(ctx, p_type);

    struct osstream local = { 0 };


    d_print_type_core(ctx,
    &local,
    p_type,
    name_opt);


    if (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN)
        ss_fprintf(ss, "extern ");

    if (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
    {
        ss_fprintf(ss, "static ");
    }


    ss_fprintf(ss, "%s", local.c_str);

    ss_close(&local);
}

//return true se todas as expressoes constantes sao 0, as nao constantes nao sao consideradas
static bool is_all_zero(const struct object* object)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }

    if (object->members != NULL)
    {
        struct object* _Opt member = object->members;
        while (member)
        {
            if (!is_all_zero(member))
                return false;
            member = member->next;
        }
    }

    if (object->p_init_expression)
    {
        if (object_has_constant_value(&object->p_init_expression->object))
        {
            if (object->p_init_expression->object.bool_value != 0)
                return false;
        }
        else
            return false;
    }

    return true;
}


static void object_print_constant_initialization(struct d_visit_ctx* ctx, struct osstream* ss, const struct object* object, bool* first)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }

    //printf("\n");
      //             object_print_to_debug(object);

    if (object->p_init_expression &&
        object->p_init_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL)
    {
        if (!(*first))
            ss_fprintf(ss, ", ");

        *first = false;

        il_visit_literal_string(object->p_init_expression->first_token, ss);
        return;
    }

    if (object->members != NULL)
    {
        struct object* _Opt member = object->members;
        while (member)
        {
            object_print_constant_initialization(ctx, ss, member, first);
            member = member->next;
        }
    }
    else
    {
        if (!(*first))
            ss_fprintf(ss, ", ");

        *first = false;

        if (object->p_init_expression)
        {
            if (object_has_constant_value(&object->p_init_expression->object))
            {
                object_print_value(ss, &object->p_init_expression->object);
            }
            else if (object->p_init_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL)
            {
                //literals also can be used in c89 initializers
                il_visit_literal_string(object->p_init_expression->first_token, ss);
            }
            else
            {
                ss_fprintf(ss, "0"); /*provisory*/
            }
        }
        else
        {
            ss_fprintf(ss, "0");
        }
    }
}

static void object_print_non_constant_initialization(struct d_visit_ctx* ctx, struct osstream* ss, const struct object* object, const char* declarator_name)
{

    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }

    if (object->members != NULL)
    {
        struct object* _Opt member = object->members;
        while (member)
        {
            object_print_non_constant_initialization(ctx, ss, member, declarator_name);
            member = member->next;
        }
    }
    else
    {
        if (object->p_init_expression)
        {
            if (object->p_init_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL)
            {
                //skip
            }
            else if (!object_has_constant_value(&object->p_init_expression->object))
            {
                print_identation_core(ss, ctx->indentation);
                ss_fprintf(ss, "%s%s = ", declarator_name, object->debug_name);
                struct osstream local = { 0 };
                d_visit_expression(ctx, &local, object->p_init_expression);
                ss_fprintf(ss, "%s", local.c_str);
                ss_fprintf(ss, ";\n");
                ss_close(&local);
            }
        }
    }
}

static void d_visit_init_declarator(struct d_visit_ctx* ctx, struct osstream* oss, struct init_declarator* p_init_declarator, bool binline)
{

    if (type_is_function(&p_init_declarator->p_declarator->type) && p_init_declarator->p_declarator->function_body == NULL)
    {
        //function declarations are on-demand
    }
    else if ((p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN) ||
             (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF))
    {
        //function declarations are on-demand
    }
    else
    {
        print_identation(ctx, oss);

        if (binline)
            ss_fprintf(oss, "__inline ");

        struct osstream ss = { 0 };
        d_print_type(ctx, &ss,
            &p_init_declarator->p_declarator->type,
            p_init_declarator->p_declarator->name_opt->lexeme);

        ss_fprintf(oss, "%s", ss.c_str);

        if (p_init_declarator->initializer)
        {
            ss_fprintf(oss, " = ");
            if (p_init_declarator->initializer->assignment_expression)
            {
                d_visit_expression(ctx, oss, p_init_declarator->initializer->assignment_expression);
                ss_fprintf(oss, ";\n");
            }
            else
            {
                if (p_init_declarator->initializer->braced_initializer)
                {
                    if (p_init_declarator->initializer->braced_initializer->initializer_list &&
                        p_init_declarator->initializer->braced_initializer->initializer_list->head)
                    {
                        if (is_all_zero(&p_init_declarator->p_declarator->object))
                        {
                            ss_fprintf(oss, "{0};\n");
                        }
                        else
                        {
                            bool first = true;
                            ss_fprintf(oss, "{");
                            object_print_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, &first);
                            ss_fprintf(oss, "}");
                            ss_fprintf(oss, ";\n");
                            object_print_non_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, p_init_declarator->p_declarator->name_opt->lexeme);
                        }
                    }
                    else
                    {
                        ss_fprintf(oss, "{0};\n");
                    }
                }
            }
        }
        else
        {
            if (p_init_declarator->p_declarator->function_body == NULL)
                ss_fprintf(oss, ";\n");
        }

        if (p_init_declarator->p_declarator->function_body)
        {
            struct hash_item_set i = { 0 };
            i.number = 1;
            hashmap_set(&ctx->function_map, p_init_declarator->p_declarator->name_opt->lexeme, &i);
            struct osstream ss = { 0 };

            ss_fprintf(oss, "\n");
            d_visit_compound_statement(ctx, oss, p_init_declarator->p_declarator->function_body);
            ss_fprintf(oss, "\n");
        }
    }
}

static void d_visit_init_declarator_list(struct d_visit_ctx* ctx,
    struct osstream* oss,
    struct init_declarator_list* p_init_declarator_list,
    bool binline)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;

    while (p_init_declarator)
    {
        d_visit_init_declarator(ctx, oss, p_init_declarator, binline);
        p_init_declarator = p_init_declarator->next;
    }
}


static void d_visit_declaration(struct d_visit_ctx* ctx, struct osstream* oss, struct declaration* p_declaration)
{
    bool binline = false;

    if (p_declaration->declaration_specifiers &&
        p_declaration->declaration_specifiers->head)
    {
        struct declaration_specifier* _Opt  p = p_declaration->declaration_specifiers->head;
        while (p)
        {
            if (p->function_specifier &&
                p->function_specifier->token->type == TK_KEYWORD_INLINE)
            {
                binline = true;//ss_fprintf(oss, "__inline ");
            }
            p = p->next;
        }
    }

    if (p_declaration->init_declarator_list.head)
    {
        if (!binline)
            d_visit_init_declarator_list(ctx, oss, &p_declaration->init_declarator_list, binline);
    }
}

void print_complete_struct(struct d_visit_ctx* ctx, struct osstream* ss, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    struct struct_or_union_specifier* _Opt p_complete =
        get_complete_struct_or_union_specifier2(p_struct_or_union_specifier);


    struct member_declaration* _Opt member_declaration =
        p_complete->member_declaration_list.head;

    if (struct_or_union_specifier_is_union(p_complete))
    {
        ss_fprintf(ss, "union %s", p_complete->tag_name);
    }
    else
    {
        ss_fprintf(ss, "struct %s", p_complete->tag_name);
    }

    if (p_complete->member_declaration_list.head)
    {
        ss_fprintf(ss, " ");
        ss_fprintf(ss, "{\n");
    }

    int no_name_index = 0;

    while (member_declaration)
    {
        struct member_declarator* _Opt member_declarator = NULL;

        if (member_declaration->member_declarator_list_opt)
        {
            member_declarator = member_declaration->member_declarator_list_opt->head;

            while (member_declarator)
            {
                if (member_declarator->declarator->name_opt)
                {
                    ss_fprintf(ss, "    ");
                    d_print_type(ctx, ss, &member_declarator->declarator->type, member_declarator->declarator->name_opt->lexeme);
                    ss_fprintf(ss, ";\n");
                }
                member_declarator = member_declarator->next;
            }
        }
        else if (member_declaration->specifier_qualifier_list != NULL)
        {
            if (member_declaration->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct type t = { 0 };
                t.category = TYPE_CATEGORY_ITSELF;
                t.struct_or_union_specifier = member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                char name[100];
                snprintf(name, sizeof name, "__m%d", no_name_index++);
                ss_fprintf(ss, "    ");
                d_print_type(ctx, ss, &t, name);
                ss_fprintf(ss, ";\n");
                type_destroy(&t);
            }
        }


        member_declaration = member_declaration->next;
    }
    if (p_complete->member_declaration_list.head)
        ss_fprintf(ss, "};\n\n");
    else
        ss_fprintf(ss, ";\n");
}

void print_complete_structs(struct d_visit_ctx* ctx, struct osstream* ss, struct struct_entry* p_struct_entry)
{
    if (p_struct_entry->done)
        return;

    for (int i = 0; i < p_struct_entry->dependencies.size; i++)
    {
        struct struct_entry* p_struct_entry_item = p_struct_entry->dependencies.data[i];
        print_complete_structs(ctx, ss, p_struct_entry_item);
        p_struct_entry_item = p_struct_entry_item->next;
    }

    if (!p_struct_entry->done)
    {
        p_struct_entry->done = true;
        struct osstream local = { 0 };
        print_complete_struct(ctx, &local, p_struct_entry->p_struct_or_union_specifier);
        ss_fprintf(ss, local.c_str);
        ss_close(&local);
    }
}

void d_visit(struct d_visit_ctx* ctx, struct osstream* oss)
{
    struct osstream declarations = { 0 };

    ctx->indentation = 0;
    struct declaration* _Opt p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        ss_clear(&ctx->add_this_before_external_decl);
        ss_clear(&ctx->function_types);

        struct osstream declaration = { 0 };
        d_visit_declaration(ctx, &declaration, p_declaration);

        if (ctx->function_types.size > 0)
        {
            ss_fprintf(&declarations, "%s\n", ctx->function_types.c_str);
            ss_clear(&ctx->function_types);
        }

        if (ctx->add_this_before.size > 0)
        {
            ss_fprintf(oss, "%s", ctx->add_this_before.c_str);
            ss_clear(&ctx->add_this_before);

        }
        if (ctx->add_this_before_external_decl.size > 0)
        {
            ss_fprintf(&declarations, "%s", ctx->add_this_before_external_decl.c_str);
            ss_clear(&ctx->add_this_before_external_decl);
        }
        if (declaration.size > 0)
            ss_fprintf(&declarations, "%s", declaration.c_str);

        ss_close(&declaration);

        p_declaration = p_declaration->next;
    }

#if 0
    const char* str
        =
        "/* Generated by Cake - C89 compliant source code \n"
        "   PREVIEW - WORK IN PROGRESS \n"
        "*/\n\n";

    ss_fprintf(oss, "%s", str);
#endif

    if (ctx->data_types.c_str)
    {
        ss_fprintf(oss, "%s", ctx->data_types.c_str);
        ss_fprintf(oss, "\n");
    }

    for (int i = 0; i < ctx->structs_map.capacity; i++)
    {
        struct map_entry* _Opt entry = ctx->structs_map.table[i];
        while (entry)
        {
            print_complete_structs(ctx, oss, entry->data.p_struct_entry);
            entry = entry->next;
        }
    }
    ss_fprintf(oss, "\n");

    if (declarations.c_str)
    {
        ss_fprintf(oss, "%s", declarations.c_str);
    }
    ss_close(&declarations);
}

