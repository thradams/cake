
#pragma safety enable
#include "version.h"
#include "ownership.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include "visit_il.h"
#include "expressions.h"

#if SIZE_MAX > UINT_MAX
#define SIZE_T_TYPE_STR "unsigned long long"
#else
#define SIZE_T_TYPE_STR "unsigned int"
#endif

#define CAKE_PREFIX_FOR_CODE_GENERATION "__Ck" 


static void print_initializer(struct d_visit_ctx* ctx,
    struct osstream* oss,
    struct init_declarator* p_init_declarator,
    bool bstatic);

void d_visit_ctx_destroy(_Dtor struct d_visit_ctx* ctx)
{
    hashmap_destroy(&ctx->tag_names);
    hashmap_destroy(&ctx->structs_map);
    hashmap_destroy(&ctx->file_scope_declarator_map);
    hashmap_destroy(&ctx->instantiated_lambdas);
    ss_close(&ctx->block_scope_declarators);
    ss_close(&ctx->add_this_before);
    ss_close(&ctx->add_this_before_external_decl);
    ss_close(&ctx->add_this_after_external_decl);
}

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

static void d_visit_function_body(struct d_visit_ctx* ctx,
    struct osstream* oss,
    const struct declarator* function_definition);

static void object_print_constant_initialization(struct d_visit_ctx* ctx, struct osstream* ss, const struct object* object, bool* first);
static void d_visit_secondary_block(struct d_visit_ctx* ctx, struct osstream* oss, struct secondary_block* p_secondary_block);
static void d_visit_init_declarator(struct d_visit_ctx* ctx, struct osstream* oss, struct init_declarator* p_init_declarator, enum function_specifier_flags function_specifier_flags, enum storage_class_specifier_flags storage_class_specifier_flags);
static void d_visit_init_declarator_list(struct d_visit_ctx* ctx, struct osstream* oss, struct init_declarator_list* p_init_declarator_list, enum function_specifier_flags function_specifier_flags, enum storage_class_specifier_flags storage_class_specifier_flags);
static void d_visit_compound_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct compound_statement* p_compound_statement);
static void d_visit_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct statement* p_statement);
static void d_visit_unlabeled_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct unlabeled_statement* p_unlabeled_statement);
static void object_print_non_constant_initialization(struct d_visit_ctx* ctx, struct osstream* ss, const struct object* object,
    const char* declarator_name,
    bool all,
    bool initialize_objects_that_does_not_have_initializer);

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
        if (type_is_bool(&p_expression->type) ||
            type_is_essential_bool(&p_expression->type))
        {
            d_visit_expression(ctx, oss, p_expression);
        }
        else
        {
            switch (p_expression->expression_type)
            {
                //Operators with lower precedence than != are:
                //&& ||  ? assignments(=, +=, …) ,
            case LOGICAL_OR_EXPRESSION:
            case LOGICAL_AND_EXPRESSION:
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
            case EXPRESSION_EXPRESSION:
            case CONDITIONAL_EXPRESSION:
                ss_fprintf(oss, "((");
                d_visit_expression(ctx, oss, p_expression);
                ss_fprintf(oss, ") != 0)");
                break;
            default:
                //we are taking some risks eliminating () to make it easier to read.
                //external ()  are optional, but I think it makes it clear.
                ss_fprintf(oss, "(");
                d_visit_expression(ctx, oss, p_expression);
                ss_fprintf(oss, " != 0)");
                break;
            }

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

static int il_visit_literal_string2(struct token* current, struct osstream* oss)
{

    const bool has_u8_prefix =
        current->lexeme[0] == 'u' && current->lexeme[1] == '8';

    const bool has_wide_prefix =
        current->lexeme[0] == 'L';

    unsigned char* psz = (unsigned char*)(current->lexeme + 0);
    if (has_u8_prefix)
    {
        psz = (unsigned char*)(current->lexeme + 2 + 1);
    }
    else if (has_wide_prefix)
    {
        psz = (unsigned char*)(current->lexeme + 1 + 1);
    }
    else
    {
        psz = (unsigned char*)(current->lexeme + 1);
    }

    while (*psz)
    {
        if (*(psz + 1) == '\0')
            break;

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

static void il_print_string(struct token* first_token, struct token* last_token, struct osstream* oss)
{
    bool opened = false;

    struct token* _Opt ptk = first_token;
    do
    {
        if (ptk == NULL)
            break;

        if ((ptk->flags & TK_FLAG_FINAL) &&
            ptk->type == TK_STRING_LITERAL)
        {
            if (!opened)
            {
                if (ptk->lexeme[0] == 'L')
                    ss_fprintf(oss, "L");
                ss_fprintf(oss, "\"");
                opened = true;
            }
            il_visit_literal_string2(ptk, oss);
        }

        if (ptk == last_token)
            break;

        ptk = ptk->next;
    } while (ptk);
    ss_fprintf(oss, "\"");

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
        object_print_value(oss, &p_expression->object, ctx->options.target);
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
        assert(p_expression->declarator != NULL);

        const char* declarator_name = "";
        if (p_expression->declarator->name_opt)
            declarator_name = p_expression->declarator->name_opt->lexeme;

        const bool is_function = type_is_function(&p_expression->declarator->type);
        bool is_local_function_definition = false;
        if (is_function)
        {
            is_local_function_definition =
                p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_BLOCK_SCOPE &&
                p_expression->declarator->function_body != NULL;
        }

        const bool is_static =
            p_expression->declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC;

        const bool is_inline =
            p_expression->declarator->declaration_specifiers->function_specifier_flags & FUNCTION_SPECIFIER_INLINE;

        const bool is_extern = p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN;

        const bool is_local =
            (!is_static && !is_extern) &&
            p_expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_BLOCK_SCOPE;

        if (is_function)
        {
            ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);

            struct map_entry* _Opt p = hashmap_find(&ctx->file_scope_declarator_map, declarator_name);
            if (p == NULL)
            {
                /*first time we see it*/

                struct hash_item_set i = { 0 };
                i.number = 1;
                hashmap_set(&ctx->file_scope_declarator_map, declarator_name, &i);
                hash_item_set_destroy(&i);

                struct osstream ss = { 0 };

                if ((is_inline || is_local_function_definition) && !is_static)
                {
                    ss_fprintf(&ss, "static ");
                }
                d_print_type(ctx, &ss, &p_expression->type, declarator_name);
                ss_fprintf(&ctx->add_this_before_external_decl, "%s", ss.c_str);
                ss_fprintf(&ctx->add_this_before_external_decl, ";\n");


                assert(p_expression->declarator != NULL);

                const struct declarator* _Opt p_function_defined
                    = declarator_get_function_definition(p_expression->declarator);

                if (p_function_defined && (is_static || is_inline || is_local_function_definition))
                {
                    //We need to find the function..

                    struct osstream local3 = { 0 };
                    struct osstream local4 = { 0 };
                    d_print_type(ctx, &local4, &p_function_defined->type, declarator_name);

                    if (!is_static)
                        ss_fprintf(&local3, "static ");

                    ss_fprintf(&local3, "%s\n", local4.c_str);

                    d_visit_function_body(ctx, &local3, p_function_defined);

                    assert(ss.c_str);
                    assert(oss->c_str);

                    ss_fprintf(&ctx->add_this_after_external_decl, "\n");
                    ss_fprintf(&ctx->add_this_after_external_decl, "%s", local3.c_str);

                    ss_close(&local3);
                    ss_close(&local4);
                }

                ss_close(&ss);
            }
        }
        else if (is_local)
        {
            ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
        }
        else
        {

            /*
               local static declarators are generated on-demand.
            */

            void* _Opt p = hashmap_find(&ctx->file_scope_declarator_map, declarator_name);
            if (p == NULL && !ctx->checking_lambda)
            {
                /*
                  first time, let´s generate it
                */

                /*
                   insert in a map to avoid generating it again
                */
                struct hash_item_set i = { 0 };
                i.number = 1;
                hashmap_set(&ctx->file_scope_declarator_map, declarator_name, &i);

                assert(p_expression->declarator);
                assert(p_expression->declarator->name_opt);


                struct osstream ss = { 0 };

                d_print_type(ctx, &ss, &p_expression->type, declarator_name);

                if (p_expression->p_init_declarator &&
                    p_expression->p_init_declarator->initializer)
                {
                    print_initializer(ctx, &ss, p_expression->p_init_declarator, true);
                    assert(ss.c_str);
                    ss_fprintf(&ctx->add_this_before_external_decl, "%s\n", ss.c_str);
                }
                else
                {
                    assert(ss.c_str);
                    ss_fprintf(&ctx->add_this_before_external_decl, "%s;\n", ss.c_str);
                }

                ss_close(&ss);
                hash_item_set_destroy(&i);
            }


            ss_fprintf(oss, "%s", p_expression->declarator->name_opt->lexeme);
        }

    }
    break;

    case PRIMARY_EXPRESSION_STRING_LITERAL:
        il_print_string(p_expression->first_token, p_expression->last_token, oss);
        break;

    case PRIMARY_EXPRESSION_ENUMERATOR:
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
    case PRIMARY_EXPRESSION_NUMBER:
    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        object_print_value(oss, &p_expression->object, ctx->options.target);
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

    case UNARY_EXPRESSION_GCC__BUILTIN_OFFSETOF:
        ss_fprintf(oss, "__builtin_offsetof(");

        if (p_expression->type_name)
        {
            d_print_type(ctx, oss, &p_expression->type_name->type, NULL);
        }
        ss_fprintf(oss, ", ");
        ss_fprintf(oss, "%s", p_expression->offsetof_member_designator->lexeme);
        ss_fprintf(oss, ")");
        break;

    case UNARY_EXPRESSION_GCC__BUILTIN_VA_START:
        assert(p_expression->left != NULL);

        if (p_expression->right != NULL)
        {
            ss_fprintf(oss, "__builtin_va_start(");
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, ", ");
            d_visit_expression(ctx, oss, p_expression->right);
            ss_fprintf(oss, ")");
        }
        else
        {
            //first argument is optional in C23
            ss_fprintf(oss, "__builtin_c23_va_start(");
            d_visit_expression(ctx, oss, p_expression->left);
            ss_fprintf(oss, ")");
            //TODO to convert to C89 we need to insert the first parameter
            //at the caller and at the implementation
            //ss_fprintf(oss, ", __first_va_arg");            
        }
        break;

    case UNARY_EXPRESSION_GCC__BUILTIN_VA_END:
        assert(p_expression->left != NULL);
        ss_fprintf(oss, "__builtin_va_end(");
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, ")");
        break;

    case UNARY_EXPRESSION_GCC__BUILTIN_VA_COPY:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);

        ss_fprintf(oss, "__builtin_va_copy(");
        d_visit_expression(ctx, oss, p_expression->left);
        ss_fprintf(oss, ", ");
        d_visit_expression(ctx, oss, p_expression->right);
        ss_fprintf(oss, ")");
        break;

    case UNARY_EXPRESSION_GCC__BUILTIN_VA_ARG:
        assert(p_expression->left != NULL);

        ss_fprintf(oss, "__builtin_va_arg(");
        d_visit_expression(ctx, oss, p_expression->left);

        if (p_expression->type_name)
        {
            ss_fprintf(oss, ", ");
            d_print_type(ctx, oss, &p_expression->type_name->type, NULL);
        }
        ss_fprintf(oss, ")");
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

        struct param* param = p_expression->left->type.params.head;

        ss_fprintf(oss, "(");
        struct argument_expression* _Opt arg = p_expression->argument_expression_list.head;
        while (arg)
        {
            bool to_bool =
                param &&
                type_is_bool(&param->type) &&
                !(type_is_bool(&arg->expression->type) ||
                  type_is_essential_bool(&arg->expression->type));

            if (to_bool)
            {
                expression_to_bool_value(ctx, oss, arg->expression);
            }
            else
            {
                d_visit_expression(ctx, oss, arg->expression);
            }

            if (param)
                param = param->next;

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
        assert(p_expression->type_name != NULL);

        char name[100] = { 0 };

        print_identation_core(&ctx->add_this_before, ctx->indentation);

        struct osstream lambda_nameless = { 0 };
        d_print_type(ctx, &lambda_nameless, &p_expression->type, NULL);

        const int current_indentation = ctx->indentation;
        ctx->indentation = 0;
        assert(p_expression->compound_statement != NULL);

        const struct declarator* _Opt p_current_function_opt = ctx->p_current_function_opt;
        ctx->p_current_function_opt = p_expression->type_name->abstract_declarator;

        const unsigned int current_cake_declarator_number = ctx->cake_declarator_number;
		const bool current_checking_lambda = ctx->checking_lambda;
		ctx->cake_declarator_number = 0; // this makes statics inside of the function literals actually work
		ctx->checking_lambda = true;
		
        d_visit_compound_statement(ctx, &lambda_nameless, p_expression->compound_statement);
		
        ctx->cake_declarator_number = current_cake_declarator_number;
		ctx->checking_lambda = current_checking_lambda;

        assert(lambda_nameless.c_str);

        struct map_entry* _Opt l = hashmap_find(&ctx->instantiated_lambdas, lambda_nameless.c_str);
        if (l != NULL && !ctx->checking_lambda)
        {
            snprintf(name, sizeof(name), CAKE_PREFIX_FOR_CODE_GENERATION "%d_flit", l->data.number);
        }
        else
        {
            snprintf(name, sizeof(name), CAKE_PREFIX_FOR_CODE_GENERATION "%d_flit", ctx->cake_declarator_number++);

			if(!ctx->checking_lambda)
            {
				struct osstream lambda_inner = { 0 };
				struct osstream lambda_sig = { 0 };
				d_print_type(ctx, &lambda_sig, &p_expression->type, name);

				d_visit_compound_statement(ctx, &lambda_inner, p_expression->compound_statement);

				struct hash_item_set i = { 0 };
				i.number = current_cake_declarator_number;
				hashmap_set(&ctx->instantiated_lambdas, lambda_nameless.c_str, &i);
				hash_item_set_destroy(&i);

				ss_fprintf(&ctx->add_this_before_external_decl, "static %s\n%s", lambda_sig.c_str, lambda_inner.c_str);
				ss_close(&lambda_sig);
				ss_close(&lambda_inner);
			}
        }
        ctx->p_current_function_opt = p_current_function_opt;
        ctx->indentation = current_indentation;

        ss_fprintf(oss, "%s", name);
        ss_close(&lambda_nameless);
    }
    break;


    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:
    {
        char name[100] = { 0 };
        snprintf(name, sizeof(name), CAKE_PREFIX_FOR_CODE_GENERATION "%d_compound_lit", ctx->cake_declarator_number++);



        if (ctx->is_local)
        {
            struct osstream local = { 0 };
            ss_swap(&ctx->block_scope_declarators, &local);
            print_identation_core(&local, ctx->indentation);
            d_print_type(ctx, &local, &p_expression->type, name);
            ss_fprintf(&local, ";\n", name);
            ss_fprintf(&ctx->block_scope_declarators, "%s", local.c_str);

            ss_clear(&local);

            //bool first = true;
            object_print_non_constant_initialization(ctx, &local, &p_expression->object, name, true, true);

            assert(local.c_str);
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
        object_print_value(oss, &p_expression->object, ctx->options.target);
        break;

    case UNARY_EXPRESSION_SIZEOF_TYPE:
        object_print_value(oss, &p_expression->object, ctx->options.target);
        break;

    case UNARY_EXPRESSION_ALIGNOF_EXPRESSION:
    case UNARY_EXPRESSION_ALIGNOF_TYPE:
    case UNARY_EXPRESSION_COUNTOF:
        object_print_value(oss, &p_expression->object, ctx->options.target);
        break;

    case UNARY_EXPRESSION_CONSTEVAL:
        d_visit_expression(ctx, oss, p_expression->right);
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
    if (p_jump_statement->first_token->type == TK_KEYWORD_CAKE_THROW)
    {
        il_print_defer_list(ctx, oss, &p_jump_statement->defer_list);
        print_identation(ctx, oss);
        ss_fprintf(oss, "goto _CKL%d;/*throw*/\n", p_jump_statement->label_id);
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
            snprintf(name, sizeof name, CAKE_PREFIX_FOR_CODE_GENERATION "%d_tmp", ctx->cake_declarator_number++);


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
                ss_fprintf(oss, "goto _CKL%d; /*break*/\n\n", ctx->break_reference.p_selection_statement->label_id);
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

static void d_visit_label(struct d_visit_ctx* ctx, struct osstream* oss, struct label* p_label);

static void d_visit_labeled_statement(struct d_visit_ctx* ctx, struct osstream* oss, struct labeled_statement* p_labeled_statement)
{
    assert(p_labeled_statement->label != NULL);

    d_visit_label(ctx, oss, p_labeled_statement->label);


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

            struct osstream block_scope_declarators = { 0 };
            ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);

            struct osstream local = { 0 };
            d_visit_declaration(ctx, &local, p_iteration_statement->declaration);

            if (ctx->block_scope_declarators.c_str)
            {
                ss_fprintf(oss, "%s", ctx->block_scope_declarators.c_str);
                ss_fprintf(oss, "\n");
            }

            if (local.c_str)
                ss_fprintf(oss, "%s", local.c_str);

            ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);


            print_identation(ctx, oss);
            ss_fprintf(oss, "for (");
            ss_close(&block_scope_declarators);
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
        snprintf(name, sizeof(name), CAKE_PREFIX_FOR_CODE_GENERATION "%d_temp", ctx->cake_declarator_number++);

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
            ss_fprintf(&ss, "goto _CKL%d;/*default*/\n", p_label_default->label_id);
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
            struct osstream block_scope_declarators = { 0 };
            ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);

            struct osstream local2 = { 0 };

            print_identation(ctx, &local2);
            d_visit_init_statement(ctx, &local2, p_selection_statement->p_init_statement);

            if (ctx->block_scope_declarators.c_str)
            {
                ss_fprintf(oss, "%s", ctx->block_scope_declarators.c_str);
                ss_fprintf(oss, "\n");
            }

            ss_fprintf(oss, "%s", local2.c_str);
            ss_close(&local2);
            print_identation(ctx, oss);
            ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);
            ss_close(&block_scope_declarators);
        }

        if (p_selection_statement->condition)
        {
            if (p_selection_statement->condition->p_init_declarator)
            {
                ss_fprintf(oss, "{\n");
                ctx->indentation++;
                addclose = true;
                struct osstream block_scope_declarators = { 0 };
                ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);


                struct osstream local2 = { 0 };
                print_identation(ctx, &local2);
                d_visit_init_declarator(ctx, &local2, p_selection_statement->condition->p_init_declarator, false, false);

                if (ctx->block_scope_declarators.c_str)
                {
                    ss_fprintf(oss, "%s", ctx->block_scope_declarators.c_str);
                    ss_fprintf(oss, "\n");
                }

                ss_fprintf(oss, "%s", local2.c_str);
                ss_close(&local2);
                ss_swap(&block_scope_declarators, &ctx->block_scope_declarators);

                print_identation(ctx, oss);
                ss_fprintf(oss, "if (");
                ss_fprintf(oss, "%s", p_selection_statement->condition->p_init_declarator->p_declarator->name_opt->lexeme);
                ss_fprintf(oss, ")\n");
                ss_close(&block_scope_declarators);
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
    ss_fprintf(oss, "if (1) /*try*/\n");

    d_visit_secondary_block(ctx, oss, p_try_statement->secondary_block);

    print_identation(ctx, oss);
    ss_fprintf(oss, "else _CKL%d: /*catch*/ \n", p_try_statement->catch_label_id);

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
        ss_fprintf(oss, "_CKL%d:", p_label->label_id);

        char str[50] = { 0 };
        object_to_str(&p_label->constant_expression->object, 50, str);
        if (p_label->constant_expression_end == NULL)
        {
            ss_fprintf(oss, " /*case %s*/ ", str);
        }
        else
        {
            char str2[50] = { 0 };
            object_to_str(&p_label->constant_expression_end->object, 50, str2);
            ss_fprintf(oss, " /*case %s ... %s*/ ", str, str2);
        }

        ss_fprintf(oss, "\n");
    }
    else if (p_label->p_first_token->type == TK_IDENTIFIER)
    {
        print_identation(ctx, oss);
        ss_fprintf(oss, "%s:\n", p_label->p_first_token->lexeme);
    }
    else if (p_label->p_first_token->type == TK_KEYWORD_DEFAULT)
    {
        print_identation(ctx, oss);
        ss_fprintf(oss, "_CKL%d: /*default*/ \n", p_label->label_id);
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

    struct osstream block_scope_declarators = { 0 };
    ss_swap(&ctx->block_scope_declarators, &block_scope_declarators);

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

    if (ctx->block_scope_declarators.c_str)
    {
        ss_fprintf(oss, "%s", ctx->block_scope_declarators.c_str);
        ss_fprintf(oss, "\n");
    }

    if (local.c_str)
        ss_fprintf(oss, "%s", local.c_str);

    print_identation(ctx, oss);
    ss_fprintf(oss, "}\n");
    ctx->is_local = is_local; //restore
    ss_swap(&ctx->block_scope_declarators, &block_scope_declarators);
    ss_close(&block_scope_declarators);
    ss_close(&local);
}

static void d_visit_function_body(struct d_visit_ctx* ctx,
    struct osstream* oss,
    const struct declarator* function_definition)
{
    if (function_definition->function_body == NULL)
    {
        assert(false);
        return;
    }
    int indentation = ctx->indentation;
    ctx->indentation = 0;
    const struct declarator* _Opt previous_func = ctx->p_current_function_opt;
    ctx->p_current_function_opt = function_definition;
    d_visit_compound_statement(ctx, oss, function_definition->function_body);
    ctx->p_current_function_opt = previous_func;//restore
    ctx->indentation = indentation; //restore
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
                                snprintf(new_name, sizeof name, "%s%d", p_complete->tag_name, ctx->cake_tag_count++);
                                snprintf(p_complete->tag_name, sizeof p_complete->tag_name, "%s", new_name);

                                struct hash_item_set i = { 0 };
                                i.number = 1;
                                hashmap_set(&ctx->tag_names, new_name, &i);
                                hash_item_set_destroy(&i);

                                //break;
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
                switch (ctx->options.target)
                {
                case TARGET_X86_X64_GCC:
                case TARGET_X86_MSVC:
                case TARGET_X64_MSVC:
                    ss_fprintf(&local, "unsigned char");
                    first = false;
                }
            }
            else
            {
                print_type_alignment_flags(&local, &first, p_type->alignment_specifier_flags, ctx->options.target);
                print_msvc_declspec(&local, &first, p_type->msvc_declspec_flags);
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

    if (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_THREAD_LOCAL)
    {
        switch (ctx->options.target)
        {
        case TARGET_X86_MSVC:
        case TARGET_X64_MSVC:
            ss_fprintf(ss, "__declspec(thread) ");
            break;

        case TARGET_X86_X64_GCC:
            ss_fprintf(ss, "__thread ");
            break;
        }
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

        il_print_string(object->p_init_expression->first_token, object->p_init_expression->last_token, ss);
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
                object_print_value(ss, &object->p_init_expression->object, ctx->options.target);
            }
            else if (object->p_init_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL)
            {
                //literals also can be used in c89 initializers
                il_print_string(object->p_init_expression->first_token, object->p_init_expression->last_token, ss);

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
    bool all,
    bool initialize_objects_that_does_not_have_initializer)
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
                        if (initialize_objects_that_does_not_have_initializer)
                        {
                            print_identation_core(ss, ctx->indentation);
                            ss_fprintf(ss, "%s%s = 0;\n", declarator_name, member->debug_name);
                        }
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
                ss_fprintf(ss, "%s, %d", local.c_str, object->type.num_of_elements);//TODO size of?

                ss_fprintf(ss, ");\n");
                ss_close(&local);
                ctx->memcpy_used = true;
            }
            else if (object->p_init_expression)
            {
                /*
                       struct A { int x, y; };
                       struct B { struct A a; };

                       int main(void)
                       {
                         struct A ia = { 1, 2 };
                         struct B b = { .a = ia, .a.y = 42 };
                       }
                */
                print_identation_core(ss, ctx->indentation);
                ss_fprintf(ss, "_cake_memcpy(&%s%s, ", declarator_name, object->debug_name);
                struct osstream local = { 0 };
                d_visit_expression(ctx, &local, object->p_init_expression);
                size_t sz = 0;
                type_get_sizeof(&object->type, &sz, ctx->options.target);
                ss_fprintf(ss, "&%s, %d", local.c_str, sz);

                ss_fprintf(ss, ");\n");
                ss_close(&local);
                ctx->memcpy_used = true;

                struct object* _Opt member = object->members;
                while (member)
                {
                    object_print_non_constant_initialization(ctx, ss, member, declarator_name, all, false);
                    member = member->next;
                }

            }
            else
            {
                struct object* _Opt member = object->members;
                while (member)
                {
                    object_print_non_constant_initialization(ctx, ss, member, declarator_name, all, true);
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
            if (initialize_objects_that_does_not_have_initializer)
            {
                print_identation_core(ss, ctx->indentation);
                ss_fprintf(ss, "%s%s = 0;\n", declarator_name, object->debug_name);
            }
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
                            if (type_get_sizeof(&p_init_declarator->p_declarator->type, &sz, ctx->options.target) != 0)
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
                            object_print_non_constant_initialization(ctx, oss, &p_init_declarator->p_declarator->object, p_init_declarator->p_declarator->name_opt->lexeme, true, true);
                        }
                    }
                }
                else
                {
                    if (is_local && !bstatic)
                    {
                        size_t sz = 0;
                        if (type_get_sizeof(&p_init_declarator->p_declarator->type, &sz, ctx->options.target) != 0)
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


static void d_visit_init_declarator(struct d_visit_ctx* ctx,
    struct osstream* oss0,
    struct init_declarator* p_init_declarator,
    enum function_specifier_flags function_specifier_flags,
    enum storage_class_specifier_flags storage_class_specifier_flags)
{
    try
    {
        const char* declarator_name = "";
        if (p_init_declarator->p_declarator->name_opt)
            declarator_name = p_init_declarator->p_declarator->name_opt->lexeme;

        const bool is_function = type_is_function(&p_init_declarator->p_declarator->type);
        const bool is_inline = (function_specifier_flags & FUNCTION_SPECIFIER_INLINE);

        const bool is_block_scope = (storage_class_specifier_flags & STORAGE_SPECIFIER_BLOCK_SCOPE);
        const bool is_typedef = (storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF);
        const bool is_extern = (storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN);

        const bool is_function_body = p_init_declarator->p_declarator->function_body != NULL;

        if (is_typedef)
            return;
        const bool is_static = (storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC);

        /*
         int i;               | !is_extern !is_block_scope !is_is_inline !is_static !is_function !is_function_body   |  action = ACTION_DECLARE;
         {int i;           }  | !is_extern  is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_DECLARE;
         extern void f() {}   |  is_extern !is_block_scope !is_inline !is_static  is_function  is_function_body   |  action = ACTION_DECLARE;
         void f() {}          | !is_extern !is_block_scope !is_inline !is_static  is_function  is_function_body   |  action = ACTION_DECLARE;
         static int i;        | !is_extern !is_block_scope !is_inline  is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         extern int i;        |  is_extern !is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         void f();            |  is_extern !is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         extern void f();     |  is_extern !is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         inline void f();     |  is_extern !is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         static void f();     |  is_extern !is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         inline void f() {}   |  is_extern !is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         static void f() {}   |  is_extern !is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         {extern int i;    }  |  is_extern  is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         {void f();        }  |  is_extern  is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         {extern void f(); }  |  is_extern  is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         {static int i;    }  |                                                                                   |  action = ACTION_ON_DEMAND_;/rename
         {inline void f() {}} |  |                                                                                |  action = ACTION_ON_DEMAND_;/rename
         {static void f() {}} |                                                                                   |  action = ACTION_ON_DEMAND_;/rename
         {inline void f();  } |  is_extern  is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         {static void f();  } |  is_extern  is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;
         {void f() {}       } |  is_extern  is_block_scope !is_inline !is_static !is_function !is_function_body   |  action = ACTION_ON_DEMAND_;/rename
         {extern void f() {}} |  is_extern  is_block_scope !is_inline !is_static !is_function !is_function_body   |  error
        */


        if (!is_extern && !is_block_scope && !is_inline && !is_static && !is_function && !is_function_body)
        {
            //int i;

            struct hash_item_set i = { 0 };
            i.number = 1;
            hashmap_set(&ctx->file_scope_declarator_map, p_init_declarator->p_declarator->name_opt->lexeme, &i);

            struct osstream ss = { 0 };
            d_print_type(ctx, &ss,
               &p_init_declarator->p_declarator->type,
               p_init_declarator->p_declarator->name_opt->lexeme
            );

            ss_fprintf(oss0, "%s", ss.c_str);

            if (p_init_declarator->initializer)
            {
                print_initializer(ctx, oss0, p_init_declarator, true);
            }
            else
                ss_fprintf(oss0, ";\n");

            ss_close(&ss);
            return;
        }
        else if (!is_extern && is_block_scope && !is_inline && !is_static && !is_function && !is_function_body)
        {
            //{ int i; }
            struct osstream ss = { 0 };
            d_print_type(ctx, &ss,
              &p_init_declarator->p_declarator->type,
              p_init_declarator->p_declarator->name_opt->lexeme);

            print_identation(ctx, &ctx->block_scope_declarators);

            ss_fprintf(&ctx->block_scope_declarators, "%s;\n", ss.c_str);
            ss_close(&ss);

            if (p_init_declarator->initializer)
            {
                //print_identation(ctx, &ctx->block_scope_declarators);

                print_initializer(ctx, oss0, p_init_declarator, false);
            }

            return;
        }
        else if (!is_block_scope && !is_inline && !is_static && is_function && is_function_body)
        {
            // extern void f() {} 
            //        void f() {} 
            //action = ACTION_DECLARE;
            struct osstream ss = { 0 };
            d_print_type(ctx, &ss,
               &p_init_declarator->p_declarator->type,
               p_init_declarator->p_declarator->name_opt->lexeme);

            struct hash_item_set i = { 0 };
            i.number = 1;
            hashmap_set(&ctx->file_scope_declarator_map, p_init_declarator->p_declarator->name_opt->lexeme, &i);

            ss_fprintf(&ss, "\n");
            d_visit_function_body(ctx, &ss, p_init_declarator->p_declarator);
            ss_fprintf(&ss, "\n");

            ss_fprintf(oss0, "%s\n", ss.c_str);
            ss_close(&ss);
            return;
        }
        else
        {
            bool rename_declarator = false;
            if (!is_extern && is_block_scope && !is_inline && is_static && !is_function && !is_function_body)
            {
                //{ static int i; }
                rename_declarator = true;
            }
            else if (!is_extern && is_block_scope && is_inline && !is_static && is_function && is_function_body)
            {
                //{inline void f() {}}

                rename_declarator = true;
            }
            else if (!is_extern && is_block_scope && !is_inline && is_static && is_function && is_function_body)
            {
                //{static void f() {}}
                rename_declarator = true;
            }
            else if (!is_extern && is_block_scope && !is_inline && !is_static && is_function && is_function_body)
            {
                //{void f() {}       }
                rename_declarator = true;
            }

            if (rename_declarator)
            {
                if (!p_init_declarator->p_declarator->declarator_renamed)
                {
                    p_init_declarator->p_declarator->declarator_renamed = true;
                    char name[100] = { 0 };
                    snprintf(name, sizeof(name),
                        CAKE_PREFIX_FOR_CODE_GENERATION "%d_%s",
                        ctx->cake_declarator_number++,
                        p_init_declarator->p_declarator->name_opt->lexeme
                    );

                    free(p_init_declarator->p_declarator->name_opt->lexeme);
                    p_init_declarator->p_declarator->name_opt->lexeme = strdup(name);
                }
            }
            return;
        }
    }
    catch
    {
    }
}

static void d_visit_init_declarator_list(struct d_visit_ctx* ctx,
    struct osstream* oss,
    struct init_declarator_list* p_init_declarator_list,
    enum function_specifier_flags function_specifier_flags,
    enum storage_class_specifier_flags storage_class_specifier_flags)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;

    while (p_init_declarator)
    {
        d_visit_init_declarator(ctx, oss, p_init_declarator, function_specifier_flags, storage_class_specifier_flags);
        p_init_declarator = p_init_declarator->next;
    }
}


static void d_visit_declaration(struct d_visit_ctx* ctx, struct osstream* oss, struct declaration* p_declaration)
{
    if (p_declaration->init_declarator_list.head)
    {
        assert(p_declaration->declaration_specifiers != NULL);

        enum storage_class_specifier_flags storage_class_specifier_flags =
            p_declaration->declaration_specifiers->storage_class_specifier_flags;

        enum function_specifier_flags function_specifier_flags =
            p_declaration->declaration_specifiers->function_specifier_flags;

        d_visit_init_declarator_list(ctx, oss, &p_declaration->init_declarator_list, function_specifier_flags, storage_class_specifier_flags);
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

    ss_fprintf(oss, "// Cake %s target=%s\n", CAKE_VERSION, target_to_string(ctx->options.target));

    ctx->indentation = 0;
    struct declaration* _Opt p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        ss_clear(&ctx->add_this_after_external_decl);
        ss_clear(&ctx->add_this_before_external_decl);


        struct osstream declaration = { 0 };
        d_visit_declaration(ctx, &declaration, p_declaration);


        if (ctx->add_this_before.size > 0)
        {
            ss_fprintf(oss, "%s", ctx->add_this_before.c_str);
            ss_clear(&ctx->add_this_before);

        }
        if (ctx->add_this_before_external_decl.size > 0)
        {
            ss_fprintf(&declarations, "%s\n", ctx->add_this_before_external_decl.c_str);
            ss_clear(&ctx->add_this_before_external_decl);
        }
        if (declaration.size > 0)
            ss_fprintf(&declarations, "%s", declaration.c_str);

        if (ctx->add_this_after_external_decl.c_str)
            ss_fprintf(&declarations, "%s", ctx->add_this_after_external_decl.c_str);

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
            "static void _cake_zmem(void *dest, register " SIZE_T_TYPE_STR " len)\n"
            "{\n"
            "  register unsigned char *ptr = (unsigned char*)dest;\n"
            "  while (len-- > 0) *ptr++ = 0;\n"
            "}\n\n";
        ss_fprintf(oss, "%s", str);
    }

    if (ctx->memcpy_used)
    {
        const char* str =
            "static void _cake_memcpy(void * dest, const void * src, " SIZE_T_TYPE_STR " n)\n"
            "{\n"
            "  char *csrc = (char *)src;\n"
            "  char *cdest = (char *)dest;\n"
            "  " SIZE_T_TYPE_STR " i; \n"
            "  for (i = 0; i < n; i++) cdest[i] = csrc[i]; \n"
            "}\n\n";
        ss_fprintf(oss, "%s", str);
    }


    if (declarations.c_str)
    {
        ss_fprintf(oss, "%s", declarations.c_str);
    }


    ss_close(&declarations);
}