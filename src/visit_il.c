
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

static void print_initializer(struct d_visit_ctx* ctx,
    struct osstream* oss,
    struct init_declarator* p_init_declarator,
    bool bstatic);

void d_visit_ctx_destroy(_Dtor struct d_visit_ctx* ctx)
{
    hashmap_destroy(&ctx->tag_names);
    hashmap_destroy(&ctx->structs_map);
    hashmap_destroy(&ctx->function_map);
    hashmap_destroy(&ctx->static_declarators);

    ss_close(&ctx->local_declarators);
    ss_close(&ctx->add_this_before);
    ss_close(&ctx->add_this_before_external_decl);

    ss_close(&ctx->data_types);
    ss_close(&ctx->function_types);
}

/*
  Moral da historia!
  C precis ver a definicao da funcao que por sua ve precisa ver a definicao dos tipos
  vamos listar todos os tipos primeiro
  depois vamos listar os tipos por ordem de dependencia com umalgorimto que imprime as dependencias primeiro
  depois vamos listar as funcoes.

*/

static struct struct_or_union_specifier* _Opt get_complete_struct_or_union_specifier2(struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    struct struct_or_union_specifier* _Opt p_complete =
        get_complete_struct_or_union_specifier(p_struct_or_union_specifier);
    if (p_complete == NULL)
        p_complete = p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection;
    return p_complete;
}

struct struct_entry;

struct struct_entry_list
{
    struct struct_entry** _Owner _Opt data;
    int size;
    int capacity;
};



struct struct_entry
{
    bool done;
    struct struct_or_union_specifier* p_struct_or_union_specifier;
    struct struct_entry_list dependencies;
    struct struct_entry* _Opt next;
};

void struct_entry_delete(struct struct_entry* _Opt _Owner p)
{
    if (p)
    {
        free(p->dependencies.data);
        free(p);
    }
}

int struct_entry_list_reserve(struct struct_entry_list* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;
        static_set(p->data, "moved");
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
static void d_visit_init_declarator(struct d_visit_ctx* ctx, struct osstream* oss, struct init_declarator* p_init_declarator, bool binline, bool bstatic);
static void d_visit_init_declarator_list(struct d_visit_ctx* ctx, struct osstream* oss, struct init_declarator_list* p_init_declarator_list, bool binline, bool bstatic);
static void d_visit_compound_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct compound_statement* p_compound_statement);
static void d_visit_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct statement* p_statement);
static void d_visit_unlabeled_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct unlabeled_statement* p_unlabeled_statement);
static void object_print_non_constant_initialization(struct d_visit_ctx* ctx, struct osstream* ss, const struct object* object, const char* declarator_name, bool all);

static void d_print_type_core(struct d_visit_ctx* ctx, struct osstream* ss, const struct type* p_type0, const char* _Opt name_opt);
static void d_print_type(struct d_visit_ctx* ctx,
    struct osstream* ss,
    const struct type* p_type,
    const char* _Opt name_opt);

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

    switch (a->value_type)
    {

    case TYPE_BOOL:
        //ss_fprintf(ss, "((unsigned char) %s)", a->value.bool_value ? "1" : "0");
        ss_fprintf(ss, "%d", a->value.bool_value ? 1 : 0);
        break;

    case TYPE_SIGNED_CHAR:

        //ss_fprintf(ss, "((signed char)%d)", (int)a->value.signed_char_value);
        ss_fprintf(ss, "%d", (int)a->value.signed_char_value);
        break;


    case TYPE_UNSIGNED_CHAR:
        //ss_fprintf(ss, "((unsigned char)%d)", (int)a->value.unsigned_char_value);
        ss_fprintf(ss, "%d", (int)a->value.unsigned_char_value);
        break;


    case TYPE_SIGNED_SHORT:
        //        ss_fprintf(ss, "((short)%d)", a->value.signed_short_value);
        ss_fprintf(ss, "%d", a->value.signed_short_value);
        break;

    case TYPE_UNSIGNED_SHORT:
        //ss_fprintf(ss, "((unsigned short) %d)", a->value.unsigned_short_value);
        ss_fprintf(ss, "%d", a->value.unsigned_short_value);
        break;

    case TYPE_SIGNED_INT:
        ss_fprintf(ss, "%d", a->value.signed_int_value);
        break;

    case TYPE_UNSIGNED_INT:
        ss_fprintf(ss, "%u", a->value.unsigned_int_value);
        ss_fprintf(ss, "U");
        break;
    case TYPE_SIGNED_LONG:
        ss_fprintf(ss, "%ld", a->value.signed_long_value);
        ss_fprintf(ss, "L");
        break;
    case TYPE_UNSIGNED_LONG:
        ss_fprintf(ss, "%lu", a->value.unsigned_long_value);
        ss_fprintf(ss, "UL");
        break;
    case TYPE_SIGNED_LONG_LONG:
        ss_fprintf(ss, "%lld", a->value.signed_long_long_value);
        ss_fprintf(ss, "LL");
        break;
    case TYPE_UNSIGNED_LONG_LONG:
        ss_fprintf(ss, "%llu", a->value.unsigned_long_long_value);
        ss_fprintf(ss, "ULL");
        break;
    case TYPE_FLOAT:
        ss_fprintf(ss, "%f", a->value.float_value);
        ss_fprintf(ss, "f");
        break;
    case TYPE_DOUBLE:
        ss_fprintf(ss, "%lf", a->value.double_value);
        break;
    case TYPE_LONG_DOUBLE:
        ss_fprintf(ss, "%Lf", a->value.long_double_value);
        ss_fprintf(ss, "L");
        break;
    }

}


static void d_visit_expression(struct d_visit_ctx* ctx, struct osstream* oss, struct expression* p_expression);

static void expression_to_bool_value(struct d_visit_ctx* ctx, struct osstream* oss, struct expression* p_expression)
{
    if (object_has_constant_value(&p_expression->object))
    {
        if (object_to_bool(&p_expression->object))
        {
            ss_fprintf(oss, "1");
        }
        else
        {
            ss_fprintf(oss, "0");
        }
    }
    else
    {
        if (type_is_bool(&p_expression->type))
        {
            d_visit_expression(ctx, oss, p_expression);
        }
        else
        {
            ss_fprintf(oss, "!!(");
            d_visit_expression(ctx, oss, p_expression);
            ss_fprintf(oss, ")");
        }
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
                    char mname[100] = { 0 };
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

static int find_member_name(const struct type* p_type, int index, char name[100])
{
    if (!type_is_struct_or_union(p_type))
        return 1;

    assert(p_type->struct_or_union_specifier != NULL);

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

static const char* get_op_by_expression_type(enum expression_type type)
{
    switch (type)
    {
    case ASSIGNMENT_EXPRESSION_ASSIGN:
        return "=";
    case ASSIGNMENT_EXPRESSION_PLUS_ASSIGN:
        return "+=";
    case ASSIGNMENT_EXPRESSION_MINUS_ASSIGN:
        return "-=";
    case ASSIGNMENT_EXPRESSION_MULTI_ASSIGN:
        return "*=";
    case ASSIGNMENT_EXPRESSION_DIV_ASSIGN:
        return "/=";
    case ASSIGNMENT_EXPRESSION_MOD_ASSIGN:
        return "%=";
    case ASSIGNMENT_EXPRESSION_SHIFT_LEFT_ASSIGN:
        return "<<=";
    case ASSIGNMENT_EXPRESSION_SHIFT_RIGHT_ASSIGN:
        return ">>=";
    case ASSIGNMENT_EXPRESSION_AND_ASSIGN:
        return "&=";
    case ASSIGNMENT_EXPRESSION_OR_ASSIGN:
        return "|=";
    case ASSIGNMENT_EXPRESSION_NOT_ASSIGN:
        return "^=";
    default:
        break;
    }
    assert(false);
    return "";
}

static void d_visit_expression(struct d_visit_ctx* ctx, struct osstream* oss, struct expression* p_expression)
{

    if (object_has_constant_value(&p_expression->object))
    {
        object_print_value(oss, &p_expression->object);
        return;
    }

    switch (p_expression->expression_type)
    {
    case EXPRESSION_TYPE_INVALID:
        assert(false);
        break;

    case PRIMARY_EXPRESSION__FUNC__:
    {
        assert(ctx->p_current_function_opt);

        char func_name[200] = { 0 };
        char name[100] = { 0 };
        if (ctx->p_current_function_opt->name_opt)
        {
            snprintf(func_name, sizeof func_name, "%s", ctx->p_current_function_opt->name_opt->lexeme);
            snprintf(name, sizeof(name), "__cake_func_%s", func_name);
        }
        else
        {
            snprintf(func_name, sizeof func_name, "unnamed");
            snprintf(name, sizeof(name), "__cake_func_%p", ctx->p_current_function_opt);
        }



        if (!ctx->is__func__predefined_identifier_added)
        {
            assert(ctx->p_current_function_opt);

            ctx->is__func__predefined_identifier_added = true;
            ss_fprintf(&ctx->add_this_before_external_decl, "static const char %s[] = \"%s\";\n", name, func_name);
        }
        ss_fprintf(oss, "%s", name);

    }
    break;

    case PRIMARY_EXPRESSION_DECLARATOR:
    {

        if (type_is_function(&p_expression->declarator->type) ||
            (p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN))
        {
            ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);

            const char* _Opt func_name = NULL;

            func_name = p_expression->first_token->lexeme;
            struct map_entry* _Opt p = hashmap_find(&ctx->function_map, func_name);
            if (p == NULL)
            {
                /*first time we see it*/

                struct hash_item_set i = { 0 };
                i.number = 1;
                hashmap_set(&ctx->function_map, func_name, &i);
                hash_item_set_destroy(&i);

                struct osstream ss = { 0 };

                d_print_type(ctx, &ss, &p_expression->type, func_name);

                if (p_expression->p_init_declarator &&
                    p_expression->p_init_declarator->initializer)
                {
                    /*
                       we include in the map to avoid export it
                       TODO it could be a flag
                    */


                    print_initializer(ctx, oss, p_expression->p_init_declarator, true);
                }

                if (type_is_function(&p_expression->declarator->type))
                {
                    assert(p_expression->declarator != NULL);

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

                        ss_swap(oss, &copy);
                        ss_close(&copy);
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
        else if (!type_is_function(&p_expression->declarator->type) &&
                 p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
        {

            /*
               local static declarators are generated on-demand.
            */

            char newname[200] = { 0 };
            snprintf(newname, sizeof newname, "%p", p_expression->declarator);

            void* _Opt p = hashmap_find(&ctx->static_declarators, newname);
            if (p == NULL)
            {
                /*
                  first time, let´s generate it
                */

                /*
                   insert in a map to avoid generating it again
                */
                struct hash_item_set i = { 0 };
                i.number = 1;
                hashmap_set(&ctx->static_declarators, newname, &i);

                assert(p_expression->declarator);
                assert(p_expression->declarator->name_opt);

                /*
                   we may have the same name used in other local scope so we need
                   a unique name
                */
                ctx->extern_count++;
                snprintf(newname, sizeof newname, "__ck_%s%d", p_expression->declarator->name_opt->lexeme, ctx->extern_count);
                free(p_expression->declarator->name_opt->lexeme);
                p_expression->declarator->name_opt->lexeme = strdup(newname);

                struct osstream ss = { 0 };

                d_print_type(ctx, &ss, &p_expression->type, newname);

                if (p_expression->p_init_declarator &&
                    p_expression->p_init_declarator->initializer)
                {
                    print_initializer(ctx, &ss, p_expression->p_init_declarator, true);
                    ss_fprintf(&ctx->function_types, "%s\n", ss.c_str);
                }
                else
                    ss_fprintf(&ctx->function_types, "%s;\n", ss.c_str);

                ss_close(&ss);
                hash_item_set_destroy(&i);
            }

            ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
        }
        else
        {
            ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
        }
    }
    break;

    case PRIMARY_EXPRESSION_STRING_LITERAL:
    {
        struct token* _Opt ptk = p_expression->first_token;
        do
        {
            if (ptk->type == TK_STRING_LITERAL)
                il_visit_literal_string(ptk, oss);

            if (ptk == p_expression->last_token)
                break;

            ptk = ptk->next;
        } while (ptk);
    }
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
        assert(p_expression->left != NULL);

        d_visit_expression(ctx, oss, p_expression->left);

        char name[100] = { 0 };
        int r = find_member_name(&p_expression->left->type, p_expression->member_index, name);
        if (r == 0)
        {
            ss_fprintf(oss, ".%s", name);
        }
    }
    break;

    case POSTFIX_ARROW:
    {
        assert(p_expression->left != NULL);


        d_visit_expression(ctx, oss, p_expression->left);
        {
            struct type t = type_remove_pointer(&p_expression->left->type);

            char name[100] = { 0 };
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
        assert(p_expression->left != NULL);

        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "++");
        break;

    case POSTFIX_DECREMENT:
        assert(p_expression->left != NULL);

        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "--");
        break;

    case POSTFIX_ARRAY:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "[");
        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, "]");
        break;

    case POSTFIX_FUNCTION_CALL:
    {
        assert(p_expression->left != NULL);


        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "(");
        struct argument_expression* _Opt arg = p_expression->argument_expression_list.head;
        while (arg)
        {
            //TODO convert int to bool parameter
            d_visit_expression(ctx, oss, arg->expression);
            if (arg->next)
                ss_fprintf(oss, ", ");
            arg = arg->next;
        }
        ss_fprintf(oss, ")");
    }
    break;

    case UNARY_EXPRESSION_ADDRESSOF:

        assert(p_expression->right != NULL);
        ss_fprintf(oss, "&");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
    {
        char name[100] = { 0 };
        snprintf(name, sizeof(name), "_lambda_%d", ctx->extern_count++);

        print_identation_core(&ctx->add_this_before, ctx->indentation);

        struct osstream lambda = { 0 };
        ss_fprintf(&lambda, "static ");
        d_print_type(ctx, &lambda, &p_expression->type, name);
        ss_fprintf(&lambda, "\n");
        int current_indentation = ctx->indentation;
        ctx->indentation = 0;
        assert(p_expression->compound_statement != NULL);

        struct declarator* _Opt p_current_function_opt = ctx->p_current_function_opt;
        ctx->p_current_function_opt = p_expression->type_name->abstract_declarator;

        d_visit_compound_statement(ctx, &lambda, p_expression->compound_statement);
        ctx->indentation = current_indentation;
        ctx->p_current_function_opt = p_current_function_opt;

        ss_fprintf(&ctx->add_this_before_external_decl, "%s\n", lambda.c_str);
        ss_fprintf(oss, "%s", name);
        ss_close(&lambda);
    }
    break;


    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:
    {
        char name[100] = { 0 };
        snprintf(name, sizeof(name), "__cmp_lt_%d", ctx->locals_count++);



        if (ctx->is_local)
        {
            struct osstream local = { 0 };
            ss_swap(&ctx->local_declarators, &local);
            print_identation_core(&local, ctx->indentation);
            d_print_type(ctx, &local, &p_expression->type, name);
            ss_fprintf(&local, ";\n", name);
            ss_fprintf(&ctx->local_declarators, "%s", local.c_str);

            ss_clear(&local);

            //bool first = true;
            object_print_non_constant_initialization(ctx, &local, &p_expression->object, name, true);
            ss_fprintf(&ctx->add_this_before, "%s", local.c_str);
            ss_close(&local);
            ss_fprintf(oss, "%s", name);
        }
        else
        {
            struct osstream local = { 0 };
            print_identation_core(&local, ctx->indentation);
            d_print_type(ctx, &local, &p_expression->type, name);
            bool first = true;
            ss_fprintf(&local, " = {");
            object_print_constant_initialization(ctx, &local, &p_expression->object, &first);
            ss_fprintf(&local, "};\n");
            ss_fprintf(&ctx->add_this_before, "%s", local.c_str);
            ss_close(&local);
            ss_fprintf(oss, "%s", name);

        }
    }
    break;

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:
        object_print_value(oss, &p_expression->object);
        break;

    case UNARY_EXPRESSION_SIZEOF_TYPE:
        object_print_value(oss, &p_expression->object);
        break;

    case UNARY_EXPRESSION_ALIGNOF:
    case UNARY_EXPRESSION_NELEMENTSOF_TYPE:
        object_print_value(oss, &p_expression->object);
        break;

    case UNARY_EXPRESSION_INCREMENT:
        assert(p_expression->right != NULL);
        ss_fprintf(oss, "++");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_DECREMENT:
        assert(p_expression->right != NULL);
        ss_fprintf(oss, "--");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_NOT:
        assert(p_expression->right != NULL);
        ss_fprintf(oss, "!");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_BITNOT:
        assert(p_expression->right != NULL);
        ss_fprintf(oss, "~");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_NEG:

        assert(p_expression->right != NULL);
        ss_fprintf(oss, "-");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case UNARY_EXPRESSION_PLUS:

        assert(p_expression->right != NULL);
        ss_fprintf(oss, "+");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_CONTENT:

        assert(p_expression->right != NULL);
        ss_fprintf(oss, "*");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_ASSERT:
        break;

    case ADDITIVE_EXPRESSION_MINUS:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " - ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case ADDITIVE_EXPRESSION_PLUS:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " + ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case MULTIPLICATIVE_EXPRESSION_MULT:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " * ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    case MULTIPLICATIVE_EXPRESSION_DIV:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " / ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case MULTIPLICATIVE_EXPRESSION_MOD:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, "%s", " % ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;


    case EXPRESSION_EXPRESSION:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, ", ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;


    case ASSIGNMENT_EXPRESSION_ASSIGN:

        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " %s ", get_op_by_expression_type(p_expression->expression_type));

        if (type_is_bool(&p_expression->left->type))
        {
            expression_to_bool_value(ctx, oss, p_expression->right);
        }
        else
        {
            d_visit_expression(ctx, oss, p_expression->right);
        }

        break;

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
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " %s ", get_op_by_expression_type(p_expression->expression_type));
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case CAST_EXPRESSION:
    {
        assert(p_expression->left != NULL);

        struct osstream local2 = { 0 };
        d_print_type(ctx, &local2, &p_expression->type, NULL);
        ss_fprintf(oss, "(%s)", local2.c_str);
        ss_close(&local2);
        d_visit_expression(ctx, oss, p_expression->left);
    }
    break;

    case SHIFT_EXPRESSION_RIGHT:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " >> ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case SHIFT_EXPRESSION_LEFT:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " << ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case RELATIONAL_EXPRESSION_BIGGER_THAN:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " > ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case RELATIONAL_EXPRESSION_LESS_THAN:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " < ");
        d_visit_expression(ctx, oss, p_expression->right);

        break;
    case EQUALITY_EXPRESSION_EQUAL:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " == ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case EQUALITY_EXPRESSION_NOT_EQUAL:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " != ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case AND_EXPRESSION:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " & ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case EXCLUSIVE_OR_EXPRESSION:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " ^ ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case INCLUSIVE_OR_EXPRESSION:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " | ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " <= ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " >= ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case LOGICAL_AND_EXPRESSION:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " && ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case LOGICAL_OR_EXPRESSION:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " || ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;

    case UNARY_EXPRESSION_TRAITS:
    case UNARY_EXPRESSION_IS_SAME:
    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
        break;

    case CONDITIONAL_EXPRESSION:
        assert(p_expression->condition_expr != NULL);
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        d_visit_expression(ctx, oss, p_expression->condition_expr);
        ss_fprintf(oss, " ? ");
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, " : ");
        d_visit_expression(ctx, oss, p_expression->right);
        break;
    }
}


static void d_visit_declaration(struct d_visit_ctx* ctx, struct osstream* oss, struct declaration* p_declaration);


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
        ss_fprintf(oss, "/*throw*/ goto _CKL%d;\n", p_jump_statement->label_id);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        struct type return_type = get_function_return_type(&ctx->p_current_function_opt->type);

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

                if (type_is_bool(&return_type))
                {
                    expression_to_bool_value(ctx, oss, p_jump_statement->expression_opt);
                }
                else
                {
                    d_visit_expression(ctx, oss, p_jump_statement->expression_opt);
                }
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
            d_print_type(ctx, oss, &return_type, name);
            ss_fprintf(oss, " = ");
            if (p_jump_statement->expression_opt)
            {
                if (type_is_bool(&return_type))
                {
                    expression_to_bool_value(ctx, oss, p_jump_statement->expression_opt);
                }
                else
                {
                    d_visit_expression(ctx, oss, p_jump_statement->expression_opt);
                }
            }
            ss_fprintf(oss, ";\n");

            il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
            print_identation(ctx, oss);

            ss_fprintf(oss, "return %s;\n", name);
        }

        type_destroy(&return_type);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {
        il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
        print_identation(ctx, oss);

        if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            if (ctx->break_reference.p_selection_statement)
            {
                ss_fprintf(oss, "/*break*/ goto _CKL%d;\n\n", ctx->break_reference.p_selection_statement->label_id);
            }
            else
            {
                ss_fprintf(oss, "break;\n");
            }
        }
        else
        {
            ss_fprintf(oss, "continue;\n");
        }
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
    assert(p_labeled_statement->label != NULL);
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
    struct break_reference old = ctx->break_reference;
    ctx->break_reference.p_iteration_statement = p_iteration_statement;
    ctx->break_reference.p_selection_statement = NULL;

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

        assert(p_iteration_statement->expression1 != NULL);
        d_visit_expression(ctx, oss, p_iteration_statement->expression1);


        ss_fprintf(oss, ");\n");
    }
    else if (p_iteration_statement->first_token->type == TK_KEYWORD_FOR)
    {
        if (p_iteration_statement->declaration)
        {
            ss_fprintf(oss, "{\n");
            ctx->indentation++;

            struct osstream local_declarators = { 0 };
            ss_swap(&local_declarators, &ctx->local_declarators);

            struct osstream local = { 0 };
            d_visit_declaration(ctx, &local, p_iteration_statement->declaration);

            ss_fprintf(oss, "%s", ctx->local_declarators.c_str);
            if (local.c_str)
                ss_fprintf(oss, "%s", local.c_str);

            ss_swap(&local_declarators, &ctx->local_declarators);


            print_identation(ctx, oss);
            ss_fprintf(oss, "for (");
            ss_close(&local_declarators);
            ss_close(&local);

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

    //restore
    ctx->break_reference = old;
}

static void d_visit_simple_declaration(struct d_visit_ctx* ctx, struct osstream* oss, struct simple_declaration* p_simple_declaration)
{
    d_visit_init_declarator_list(ctx, oss, &p_simple_declaration->init_declarator_list, false, false);
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
        d_visit_init_declarator(ctx, oss, p_condition->p_init_declarator, false, false);

    if (p_condition->expression)
        d_visit_expression(ctx, oss, p_condition->expression);
}

static bool is_compound_statement(struct secondary_block* p_secondary_block)
{
    assert(p_secondary_block->statement != NULL);

    if (p_secondary_block->statement->unlabeled_statement &&
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
        struct break_reference old = ctx->break_reference;
        ctx->break_reference.p_iteration_statement = NULL;
        ctx->break_reference.p_selection_statement = p_selection_statement;
        assert(p_selection_statement->condition != NULL);

        struct osstream ss = { 0 };


        ss_fprintf(&ss, "/*switch*/\n");
        print_identation(ctx, &ss);
        ss_fprintf(&ss, "{\n");
        ctx->indentation++;


        char name[100] = { 0 };
        snprintf(name, sizeof(name), "_R%d", ctx->locals_count++);

        print_identation(ctx, &ss);
        ss_fprintf(&ss, "register ");
        d_print_type(ctx, &ss, &p_selection_statement->condition->expression->type, name);

        ss_fprintf(&ss, " = ");

        assert(p_selection_statement->condition != NULL);
        d_visit_condition(ctx, &ss, p_selection_statement->condition);
        ss_fprintf(&ss, ";\n");

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
                print_identation(ctx, &ss);
                if (p_label->constant_expression_end == NULL)
                {
                    char str[50] = { 0 };
                    object_to_str(&p_label->constant_expression->object, 50, str);
                    ss_fprintf(&ss, "if (%s == %s) goto _CKL%d; /*case %s*/\n", name, str, p_label->label_id, str);

                }
                else
                {
                    char str_begin[50] = { 0 };
                    object_to_str(&p_label->constant_expression->object, 50, str_begin);
                    char str_end[50] = { 0 };
                    object_to_str(&p_label->constant_expression_end->object, 50, str_end);
                    ss_fprintf(&ss, "if (%s >= %s && %s <= %s) goto _CKL%d; /*case %s ... %s*/\n", name, str_begin, name, str_end, p_label->label_id, str_begin, str_end);
                }
            }

            p_label = p_label->next;
        }

        print_identation(ctx, &ss);

        if (p_label_default)
        {
            ss_fprintf(&ss, "goto /*default*/ _CKL%d;\n", p_label_default->label_id);
        }
        else
        {
            ss_fprintf(&ss, "goto _CKL%d;\n", p_selection_statement->label_id);
        }

        ss_fprintf(&ss, "\n");

        d_visit_secondary_block(ctx, &ss, p_selection_statement->secondary_block);

        print_identation(ctx, &ss);
        ss_fprintf(&ss, "_CKL%d:;\n", ctx->break_reference.p_selection_statement->label_id);

        ctx->indentation--;
        print_identation(ctx, &ss);
        ss_fprintf(&ss, "}\n");

        ss_fprintf(oss, "%s", ss.c_str);
        ss_close(&ss);
        //restore
        ctx->break_reference = old;
    }
    else if (p_selection_statement->first_token->type == TK_KEYWORD_IF)
    {
        bool addclose = false;
        if (p_selection_statement->p_init_statement)
        {
            ss_fprintf(oss, "{\n");
            addclose = true;
            ctx->indentation++;
            struct osstream local_declarators = { 0 };
            ss_swap(&local_declarators, &ctx->local_declarators);

            struct osstream local2 = { 0 };
            d_visit_init_statement(ctx, &local2, p_selection_statement->p_init_statement);
            ss_fprintf(oss, "%s", ctx->local_declarators.c_str);
            ss_fprintf(oss, "\n");
            ss_fprintf(oss, "%s", local2.c_str);
            ss_close(&local2);
            print_identation(ctx, oss);
            ss_swap(&local_declarators, &ctx->local_declarators);
            ss_close(&local_declarators);
        }

        if (p_selection_statement->condition)
        {
            if (p_selection_statement->condition->p_init_declarator)
            {
                ss_fprintf(oss, "{\n");
                ctx->indentation++;
                addclose = true;
                struct osstream local_declarators = { 0 };
                ss_swap(&local_declarators, &ctx->local_declarators);

                struct osstream local2 = { 0 };
                d_visit_init_declarator(ctx, &local2, p_selection_statement->condition->p_init_declarator, false, false);
                ss_fprintf(oss, "%s", ctx->local_declarators.c_str);
                ss_fprintf(oss, "\n");
                ss_fprintf(oss, "%s", local2.c_str);
                ss_close(&local2);
                ss_swap(&local_declarators, &ctx->local_declarators);

                print_identation(ctx, oss);
                ss_fprintf(oss, "if (");
                ss_fprintf(oss, "%s", p_selection_statement->condition->p_init_declarator->p_declarator->name_opt->lexeme);
                ss_fprintf(oss, ")\n");
                ss_close(&local_declarators);
            }
            else if (p_selection_statement->condition->expression)
            {
                ss_fprintf(oss, "if (");
                d_visit_expression(ctx, oss, p_selection_statement->condition->expression);
                ss_fprintf(oss, ")\n");
            }

        }

        assert(p_selection_statement->secondary_block != NULL);

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
    ss_fprintf(oss, "/*try*/ if (1)\n");

    d_visit_secondary_block(ctx, oss, p_try_statement->secondary_block);

    print_identation(ctx, oss);
    ss_fprintf(oss, "/*catch*/ else _CKL%d:\n", p_try_statement->catch_label_id);

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
}

static void d_visit_label(struct d_visit_ctx* ctx, struct osstream* oss, struct label* p_label)
{
    if (p_label->p_first_token->type == TK_KEYWORD_CASE)
    {
        print_identation(ctx, oss);
        char str[50] = { 0 };
        object_to_str(&p_label->constant_expression->object, 50, str);
        if (p_label->constant_expression_end == NULL)
        {
            ss_fprintf(oss, "/*case %s*/ ", str);
        }
        else
        {
            char str2[50] = { 0 };
            object_to_str(&p_label->constant_expression_end->object, 50, str2);
            ss_fprintf(oss, "/*case %s ... %s*/ ", str, str2);
        }
        ss_fprintf(oss, "_CKL%d:\n", p_label->label_id);
    }
    else if (p_label->p_first_token->type == TK_IDENTIFIER)
    {
        print_identation(ctx, oss);
        ss_fprintf(oss, "%s:\n", p_label->p_first_token->lexeme);
    }
    else if (p_label->p_first_token->type == TK_KEYWORD_DEFAULT)
    {
        print_identation(ctx, oss);
        ss_fprintf(oss, "/*default*/ _CKL%d:\n", p_label->label_id);
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
    bool is_local = ctx->is_local;
    ctx->is_local = true;

    struct osstream local_declarators = { 0 };
    ss_swap(&ctx->local_declarators, &local_declarators);

    struct osstream local = { 0 };

    ctx->indentation++;
    d_visit_block_item_list(ctx, &local, &p_compound_statement->block_item_list);


    bool ends_with_jump = false;

    if (p_compound_statement->block_item_list.tail &&
        p_compound_statement->block_item_list.tail->unlabeled_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != NULL)
    {
        ends_with_jump = true;
    }

    if (!ends_with_jump)
        il_print_defer_list(ctx, &local, &p_compound_statement->defer_list);

    ctx->indentation--;

    print_identation(ctx, oss);
    ss_fprintf(oss, "{\n");

    if (ctx->local_declarators.c_str)
    {
        ss_fprintf(oss, "%s", ctx->local_declarators.c_str);
        ss_fprintf(oss, "\n");
    }

    if (local.c_str)
        ss_fprintf(oss, "%s", local.c_str);

    print_identation(ctx, oss);
    ss_fprintf(oss, "}\n");
    ctx->is_local = is_local; //restore
    ss_swap(&ctx->local_declarators, &local_declarators);
    ss_close(&local_declarators);
    ss_close(&local);
}


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
                        char name[100] = { 0 };
                        snprintf(name, sizeof name, "%p", (void*)p_complete);

                        struct map_entry* _Opt p = hashmap_find(&ctx->structs_map, name);
                        if (p == NULL)
                        {
                            //vamos ver se ja existe alguma com o mesmo tag?
                            //se existir vamos mudar o tagname desta nova adicinando um numero.

                            struct map_entry* _Opt p_name = hashmap_find(&ctx->tag_names, p_complete->tag_name);
                            if (p_name != NULL)
                            {
                                //ja existe uma com este nome
                                char new_name[100] = { 0 };
                                snprintf(new_name, sizeof name, "%s%d", p_complete->tag_name, ctx->tag_name_count++);
                                snprintf(p_complete->tag_name, sizeof p_complete->tag_name, "%s", new_name);

                                struct hash_item_set i = { 0 };
                                i.number = 1;
                                hashmap_set(&ctx->tag_names, new_name, &i);
                                hash_item_set_destroy(&i);
                            }
                            else
                            {
                                struct hash_item_set i = { 0 };
                                i.number = 1;
                                hashmap_set(&ctx->tag_names, p_complete->tag_name, &i);
                                hash_item_set_destroy(&i);
                            }

                            _Opt struct struct_entry* _Opt _Owner p_struct_entry = calloc(1, sizeof * p_struct_entry);
                            if (p_struct_entry == NULL)
                                throw;

                            p_struct_entry->p_struct_or_union_specifier = p_complete;

                            {
                                struct hash_item_set i = { 0 };
                                i.p_struct_entry = p_struct_entry;
                                hashmap_set(&ctx->structs_map, name, &i);
                                hash_item_set_destroy(&i);
                            }

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
                                        if (member_declarator->declarator)
                                        {
                                            if (type_is_struct_or_union(&member_declarator->declarator->type))
                                            {
                                                assert(member_declarator->declarator->type.struct_or_union_specifier != NULL);

                                                struct struct_or_union_specifier* _Opt p_complete_member =
                                                    get_complete_struct_or_union_specifier(member_declarator->declarator->type.struct_or_union_specifier);

                                                if (p_complete_member == NULL)
                                                    throw;

                                                char name2[100] = { 0 };
                                                snprintf(name2, sizeof name2, "%p", (void*)p_complete_member);

                                                register_struct_types_and_functions(ctx, &member_declarator->declarator->type);
                                                struct map_entry* _Opt p2 = hashmap_find(&ctx->structs_map, name2);
                                                if (p2 != NULL)
                                                {
                                                    assert(p2->data.p_struct_entry != NULL);
                                                    struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                                }
                                            }
                                            if (type_is_array(&member_declarator->declarator->type))
                                            {
                                                struct type t = get_array_item_type(&member_declarator->declarator->type);
                                                if (type_is_struct_or_union(&t))
                                                {
                                                    assert(t.struct_or_union_specifier != NULL);

                                                    struct struct_or_union_specifier* _Opt p_complete_member =
                                                        p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);

                                                    char name2[100] = { 0 };
                                                    snprintf(name2, sizeof name2, "%p", (void* _Opt)p_complete_member);

                                                    register_struct_types_and_functions(ctx, &t);
                                                    struct map_entry* _Opt p2 = hashmap_find(&ctx->structs_map, name2);
                                                    if (p2 != NULL)
                                                    {
                                                        assert(p2->data.p_struct_entry != NULL);
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

                                            char name2[100] = { 0 };
                                            snprintf(name2, sizeof name2, "%p", (void* _Opt)p_complete_member);

                                            register_struct_types_and_functions(ctx, &t);
                                            struct map_entry* _Opt p2 = hashmap_find(&ctx->structs_map, name2);
                                            if (p2 != NULL)
                                            {
                                                assert(p2->data.p_struct_entry != NULL);
                                                struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                            }
                                        }
                                        if (type_is_array(&t))
                                        {
                                            struct type t2 = get_array_item_type(&t);
                                            if (type_is_struct_or_union(&t2))
                                            {
                                                struct struct_or_union_specifier* _Opt p_complete_member =
                                                    p_complete_member = get_complete_struct_or_union_specifier(t.struct_or_union_specifier);

                                                if (p_complete_member == NULL)
                                                {
                                                    type_destroy(&t2);
                                                    throw;
                                                }

                                                char name2[100] = { 0 };
                                                snprintf(name2, sizeof name2, "%p", (void*)p_complete_member);

                                                register_struct_types_and_functions(ctx, &t);
                                                struct map_entry* _Opt p2 = hashmap_find(&ctx->structs_map, name2);
                                                if (p2 != NULL)
                                                {
                                                    assert(p2->data.p_struct_entry != NULL);
                                                    struct_entry_list_push_back(&p_struct_entry->dependencies, p2->data.p_struct_entry);
                                                }
                                            }
                                            else
                                            {
                                                register_struct_types_and_functions(ctx, &t);
                                            }
                                            type_destroy(&t2);
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
    const char* _Opt name_opt)
{
    const struct type* _Opt p_type = p_type0;

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
                enum type_specifier_flags enum_type_specifier_flags =
                    get_enum_type_specifier_flags(p_type->enum_specifier);

                print_type_specifier_flags(&local, &first, enum_type_specifier_flags);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
            {
                ss_fprintf(&local, "unsigned char ");
            }
            else
            {
                print_type_specifier_flags(&local, &first, p_type->type_specifier_flags);
            }

            if (name_opt) /*flow loop bug*/
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
            ss_close(&local);
        }

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

                ss_fprintf(ss, "%zu", p_type->num_of_elements);
            }
            else
            {
                //vlz
            }
            ss_fprintf(ss, "]");

            break;

        case TYPE_CATEGORY_FUNCTION:

            if (name_opt)
            {
                //if (first)
                //{
                  //  ss_fprintf(ss, " ");
                    //first = false;
                //}
                if (p_type->attributes_flags & CAKE_ATTRIBUTE_STDCALL)
                {
                    ss_fprintf(ss, "__stdcall ");
                }
                else if (p_type->attributes_flags & CAKE_ATTRIBUTE_FASTCALL)
                {
                    ss_fprintf(ss, "__fastcall ");
                }
                else if (p_type->attributes_flags & CAKE_ATTRIBUTE_CDECL)
                {
                    ss_fprintf(ss, "__cdecl ");
                }
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
                ss_close(&local2);
            }

            if (p_type->params.is_var_args)
            {
                if (p_type->params.head)
                    ss_fprintf(ss, ", ");
                ss_fprintf(ss, "...");
            }

            ss_fprintf(ss, ")");


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

            if (p_type->attributes_flags & CAKE_ATTRIBUTE_STDCALL)
            {
                ss_fprintf(&local, "__stdcall ");
            }
            else if (p_type->attributes_flags & CAKE_ATTRIBUTE_FASTCALL)
            {
                ss_fprintf(&local, "__fastcall ");
            }
            else if (p_type->attributes_flags & CAKE_ATTRIBUTE_CDECL)
            {
                ss_fprintf(&local, "__cdecl ");
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

        }
        break;
        }

        p_type = p_type->next;

    }
}

static void d_print_type(struct d_visit_ctx* ctx,
    struct osstream* ss,
    const struct type* p_type,
    const char* _Opt name_opt)
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
            if (object_to_bool(&object->p_init_expression->object) != 0)
            {
                return false;
            }
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
        if (type_is_union(&object->type))
        {
            //In c89 only the first member can be initialized
            //we could make the first member be array of unsigned int
            //then initialize it
            struct object* _Opt member = object->members;
            object_print_constant_initialization(ctx, ss, member, first);
        }
        else
        {
            struct object* _Opt member = object->members;
            while (member)
            {
                object_print_constant_initialization(ctx, ss, member, first);
                member = member->next;
            }
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

static void object_print_non_constant_initialization(struct d_visit_ctx* ctx,
    struct osstream* ss,
    const struct object* object,
    const char* declarator_name,
    bool all)
{

    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }

    if (object->members != NULL)
    {
        if (type_is_union(&object->type))
        {
            //In c89 only the first member can be initialized
            struct object* _Opt member = object->members;

            if (member->p_init_expression &&
                object_has_constant_value(&member->p_init_expression->object) &&
                !all)
            {
                //already initialized
            }
            else
            {
                //TODO external declarations bug
                /*
                */
                while (member)
                {
                    if (member->p_init_expression)
                    {
                        //object_print_non_constant_initialization(ctx, ss, member, declarator_name);
                        print_identation_core(ss, ctx->indentation);
                        ss_fprintf(ss, "%s%s = ", declarator_name, member->debug_name);
                        struct osstream local = { 0 };
                        d_visit_expression(ctx, &local, member->p_init_expression);
                        ss_fprintf(ss, "%s", local.c_str);
                        ss_fprintf(ss, ";\n");
                        ss_close(&local);
                        break;
                    }
                    else if (all)
                    {
                        print_identation_core(ss, ctx->indentation);
                        ss_fprintf(ss, "%s%s = 0;\n", declarator_name, member->debug_name);
                    }
                    member = member->next;
                }
            }
        }
        else
        {
            if (object->p_init_expression &&
                object->p_init_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL &&
                type_is_array(&object->type))
            {
                //char b[] = "abc";
                print_identation_core(ss, ctx->indentation);
                ss_fprintf(ss, "_cake_memcpy(%s%s, ", declarator_name, object->debug_name);
                struct osstream local = { 0 };
                d_visit_expression(ctx, &local, object->p_init_expression);
                ss_fprintf(ss, "%s, %d", local.c_str, object->type.num_of_elements);

                ss_fprintf(ss, ");\n");
                ss_close(&local);
                ctx->memcpy_used = true;
            }
            else
            {
                struct object* _Opt member = object->members;
                while (member)
                {
                    object_print_non_constant_initialization(ctx, ss, member, declarator_name, all);
                    member = member->next;
                }
            }
        }
    }
    else
    {
        if (object->p_init_expression)
        {
            if (!all)
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
            else
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
        else if (all)
        {
            print_identation_core(ss, ctx->indentation);
            ss_fprintf(ss, "%s%s = 0;\n", declarator_name, object->debug_name);
        }
    }
}


static void print_initializer(struct d_visit_ctx* ctx,
    struct osstream* oss,
    struct init_declarator* p_init_declarator,
    bool bstatic)
{
    assert(p_init_declarator->initializer != NULL);

    try
    {
        const bool is_local = ctx->is_local;

        if (p_init_declarator->initializer->assignment_expression)
        {
            if (is_local && !bstatic)
            {
                if (p_init_declarator->initializer->assignment_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL &&
                    type_is_array(&p_init_declarator->p_declarator->type))
                {
                    print_identation_core(oss, ctx->indentation);
                    ss_fprintf(oss, "_cake_memcpy(%s, ", p_init_declarator->p_declarator->name_opt->lexeme);
                    struct osstream local = { 0 };
                    d_visit_expression(ctx, &local, p_init_declarator->initializer->assignment_expression);
                    ss_fprintf(oss, "%s, %d", local.c_str, p_init_declarator->p_declarator->type.num_of_elements);

                    ss_fprintf(oss, ");\n");
                    ss_close(&local);
                    ctx->memcpy_used = true;
                }
                else
                {
                    //ss_fprintf(oss, ";\n");
                    print_identation_core(oss, ctx->indentation);
                    ss_fprintf(oss, "%s%s = ", p_init_declarator->p_declarator->name_opt->lexeme, "");

                    if (type_is_bool(&p_init_declarator->p_declarator->type))
                    {
                        expression_to_bool_value(ctx, oss, p_init_declarator->initializer->assignment_expression);
                    }
                    else
                    {
                        d_visit_expression(ctx, oss, p_init_declarator->initializer->assignment_expression);
                    }
                    ss_fprintf(oss, ";\n");
                }
            }
            else
            {
                print_identation_core(oss, ctx->indentation);
                ss_fprintf(oss, " = ");//, p_init_declarator->p_declarator->name_opt->lexeme, "");
                d_visit_expression(ctx, oss, p_init_declarator->initializer->assignment_expression);
                ss_fprintf(oss, ";\n");
            }
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
                        if (is_local && !bstatic)
                        {
                            size_t sz = 0;
                            if (type_get_sizeof(&p_init_declarator->p_declarator->type, &sz) != 0)
                            {
                                throw;
                            }

                            // ss_fprintf(oss, ";\n");
                            print_identation_core(oss, ctx->indentation);
                            ss_fprintf(oss, "_cake_zmem(&%s, %zu);\n",
                            p_init_declarator->p_declarator->name_opt->lexeme,
                            sz);
                            ctx->zero_mem_used = true;
                        }
                        else
                        {
                            ss_fprintf(oss, " = ");
                            ss_fprintf(oss, "{0};\n");
                        }
                    }
                    else
                    {

                        bool first = true;
                        if (!is_local || bstatic)
                        {
                            ss_fprintf(oss, " = ");
                            ss_fprintf(oss, "{");
                            object_print_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, &first);
                            ss_fprintf(oss, "}");
                            ss_fprintf(oss, ";\n");
                        }
                        else
                        {
                            //ss_fprintf(oss, ";\n");
                            object_print_non_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, p_init_declarator->p_declarator->name_opt->lexeme, true);
                        }
                    }
                }
                else
                {
                    if (is_local && !bstatic)
                    {
                        size_t sz = 0;
                        if (type_get_sizeof(&p_init_declarator->p_declarator->type, &sz) != 0)
                        {
                            throw;
                        }

                        //ss_fprintf(oss, ";\n");
                        print_identation_core(oss, ctx->indentation);
                        ss_fprintf(oss, "_cake_zmem(&%s, %zu);\n",
                        p_init_declarator->p_declarator->name_opt->lexeme,
                        sz);
                        ctx->zero_mem_used = true;
                    }
                    else
                    {
                        ss_fprintf(oss, " = ");
                        ss_fprintf(oss, "{0};\n");
                    }
                    //ss_fprintf(oss, "{0};\n");
                }
            }
        }
    }
    catch
    {
    }
}


static void d_visit_init_declarator(struct d_visit_ctx* ctx, struct osstream* oss0, struct init_declarator* p_init_declarator, bool binline, bool bstatic)
{
    struct osstream* _Opt oss = NULL;
    try
    {
        const bool is_local = ctx->is_local;
        const bool is_static_not_function = bstatic &&
            !type_is_function(&p_init_declarator->p_declarator->type);

        oss = is_static_not_function ? &ctx->add_this_before_external_decl : oss0;

        if (type_is_function(&p_init_declarator->p_declarator->type) && p_init_declarator->p_declarator->function_body == NULL)
        {
            //function declarations are created on-demand
            return;
        }

        if (
                 p_init_declarator->p_declarator->declaration_specifiers && (
                     (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN) ||
                     (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF))
            )
        {
            //function declarations are on-demand
            return;
        }

        if (is_local && is_static_not_function)
        {
            //local static non functions declarations are created on-demand
            return;
        }

        if (p_init_declarator->p_declarator->name_opt == NULL)
            throw;

        if (binline)
            ss_fprintf(oss, "__inline ");

        struct osstream ss = { 0 };


        if (is_local && !bstatic)
        {
            d_print_type(ctx, &ss,
                &p_init_declarator->p_declarator->type,
                p_init_declarator->p_declarator->name_opt->lexeme);

            print_identation(ctx, &ctx->local_declarators);

            ss_fprintf(&ctx->local_declarators, "%s;\n", ss.c_str);
            ss_clear(&ss);
        }
        else
        {
            if (is_local)
                print_identation(ctx, &ss);


            d_print_type(ctx, &ss,
                &p_init_declarator->p_declarator->type,
                p_init_declarator->p_declarator->name_opt->lexeme);


            if (p_init_declarator->initializer == NULL &&
                p_init_declarator->p_declarator->function_body == NULL)
            {
                ss_fprintf(oss, "%s;\n", ss.c_str);
            }
            else
                ss_fprintf(oss, "%s", ss.c_str);
        }

        if (p_init_declarator->initializer)
        {
            char newname[200] = { 0 };
            snprintf(newname, sizeof newname, "%p", p_init_declarator->p_declarator);
            struct hash_item_set i = { 0 };
            i.number = 1;
            hashmap_set(&ctx->static_declarators, newname, &i);
            print_initializer(ctx, oss, p_init_declarator, bstatic);
            hash_item_set_destroy(&i);
        }
        else
        {
            //if (p_init_declarator->p_declarator->function_body == NULL)
              //  ss_fprintf(oss, ";\n");
        }

        if (p_init_declarator->p_declarator->function_body)
        {
            assert(p_init_declarator->p_declarator->name_opt != NULL);

            ctx->is__func__predefined_identifier_added = false;
            ctx->p_current_function_opt = p_init_declarator->p_declarator;

            struct hash_item_set i = { 0 };
            i.number = 1;
            hashmap_set(&ctx->function_map, p_init_declarator->p_declarator->name_opt->lexeme, &i);

            ss_fprintf(oss, "\n");
            d_visit_compound_statement(ctx, oss, p_init_declarator->p_declarator->function_body);
            ss_fprintf(oss, "\n");
            hash_item_set_destroy(&i);
        }

        ss_close(&ss);
    }
    catch
    {
    }
}

static void d_visit_init_declarator_list(struct d_visit_ctx* ctx,
    struct osstream* oss,
    struct init_declarator_list* p_init_declarator_list,
    bool binline,
    bool bstatic)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;

    while (p_init_declarator)
    {
        d_visit_init_declarator(ctx, oss, p_init_declarator, binline, bstatic);
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
        assert(p_declaration->declaration_specifiers != NULL);

        bool is_static = p_declaration->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC;

        if (!binline)
            d_visit_init_declarator_list(ctx, oss, &p_declaration->init_declarator_list, binline, is_static);
    }
}

static void print_complete_struct(struct d_visit_ctx* ctx, struct osstream* ss, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    try
    {
        struct struct_or_union_specifier* _Opt p_complete =
            get_complete_struct_or_union_specifier2(p_struct_or_union_specifier);

        if (p_complete == NULL)
            throw;

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
                    if (member_declarator->declarator &&
                        member_declarator->declarator->name_opt)
                    {
                        ss_fprintf(ss, "    ");

                        if (type_is_array(&member_declarator->declarator->type) &&
                            member_declarator->declarator->type.num_of_elements == 0)
                        {
                            //Flexible array members - we print as [1] instead 
                            // of [0] or []
                            //sizeof is not used in generated code, so this will not cause 
                            //problems
                            member_declarator->declarator->type.num_of_elements = 1;

                            d_print_type(ctx,
                             ss,
                             &member_declarator->declarator->type,
                             member_declarator->declarator->name_opt->lexeme);

                            member_declarator->declarator->type.num_of_elements = 0; //restore
                        }
                        else
                        {
                            d_print_type(ctx,
                                ss,
                                &member_declarator->declarator->type,
                                member_declarator->declarator->name_opt->lexeme);
                        }
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

                    char name[100] = { 0 };
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
    catch
    {
    }
}

void print_complete_structs(struct d_visit_ctx* ctx, struct osstream* ss, struct struct_entry* p_struct_entry)
{
    if (p_struct_entry->done)
        return;

    for (int i = 0; i < p_struct_entry->dependencies.size; i++)
    {
        struct struct_entry* p_struct_entry_item = p_struct_entry->dependencies.data[i];
        print_complete_structs(ctx, ss, p_struct_entry_item);
    }

    if (!p_struct_entry->done)
    {
        p_struct_entry->done = true;
        struct osstream local = { 0 };
        print_complete_struct(ctx, &local, p_struct_entry->p_struct_or_union_specifier);
        if (local.c_str)
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
            assert(entry->data.p_struct_entry != NULL);
            print_complete_structs(ctx, oss, entry->data.p_struct_entry);
            entry = entry->next;
        }
    }
    ss_fprintf(oss, "\n");

    if (ctx->zero_mem_used)
    {
        const char* str =
            "static void _cake_zmem(void *dest, register unsigned int len)\n"
            "{\n"
            "  register unsigned char *ptr = (unsigned char*)dest;\n"
            "  while (len-- > 0) *ptr++ = 0;\n"
            "}\n\n";
        ss_fprintf(oss, "%s", str);
    }

    if (ctx->memcpy_used)
    {
        const char* str =
            "static void _cake_memcpy(void * dest, const void * src, unsigned long n)\n"
            "{\n"
            "  char *csrc = (char *)src;\n"
            "  char *cdest = (char *)dest;\n"
            "  for (int i=0; i<n; i++) cdest[i] = csrc[i]; \n"
            "}\n\n";
        ss_fprintf(oss, "%s", str);
    }


    if (declarations.c_str)
    {
        ss_fprintf(oss, "%s", declarations.c_str);
    }
    ss_close(&declarations);
}

