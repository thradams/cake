#pragma ownership enable

#include "ownership.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "tokenizer.h"
#include "hashmap.h"
#include "parser.h"
#include <string.h>
#include <stddef.h>
#include "osstream.h"
#include "console.h"
#include "fs.h"
#include <ctype.h>
#include "format_visit.h"
#include "flow_visit.h"
#include <errno.h>
#include "flow_object.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include "version.h"

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif

#include "visit.h"
#include <time.h>

#ifdef PATH_MAX
#define MYMAX_PATH PATH_MAX // Linux uses it in realpath
#else
#define MYMAX_PATH MAX_PATH
#endif


struct defer_statement* owner defer_statement(struct parser_ctx* ctx);

void defer_statement_delete(struct defer_statement* owner opt p)
{
    if (p)
    {
        secondary_block_delete(p->secondary_block);
        free(p);
    }
}

static int s_anonymous_struct_count = 0;

///////////////////////////////////////////////////////////////////////////////
void naming_convention_struct_tag(struct parser_ctx* ctx, struct token* token);
void naming_convention_enum_tag(struct parser_ctx* ctx, struct token* token);
void naming_convention_function(struct parser_ctx* ctx, struct token* token);
void naming_convention_enumerator(struct parser_ctx* ctx, struct token* token);
void naming_convention_struct_member(struct parser_ctx* ctx, struct token* token, struct type* type);
void naming_convention_parameter(struct parser_ctx* ctx, struct token* token, struct type* type);
void naming_convention_global_var(struct parser_ctx* ctx, struct token* token, struct type* type, enum storage_class_specifier_flags storage);
void naming_convention_local_var(struct parser_ctx* ctx, struct token* token, struct type* type);

///////////////////////////////////////////////////////////////////////////////

static bool parser_is_diagnostic_enabled(const struct parser_ctx* ctx, enum diagnostic_id w)
{
    if (w > W_NOTE)
        return true;

    return ((ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors & w) != 0) ||
        ((ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings & w) != 0) ||
        ((ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes & w) != 0);
}

static void check_open_brace_style(struct parser_ctx* ctx, struct token* token)
{
    // token points to {

    if (token->level == 0 &&
        !(token->flags & TK_FLAG_MACRO_EXPANDED) &&
        token->type == '{' &&
        parser_is_diagnostic_enabled(ctx, W_STYLE))
    {
        if (ctx->options.style == STYLE_CAKE)
        {
            if (token->prev->type == TK_BLANKS &&
                token->prev->prev->type == TK_NEWLINE)
            {
            }
            else
            {
                compiler_diagnostic_message(W_STYLE, ctx, token, "not following correct brace style {");
            }
        }
    }
}

static void check_close_brace_style(struct parser_ctx* ctx, struct token* token)
{
    // token points to {

    if (token->level == 0 &&
        !(token->flags & TK_FLAG_MACRO_EXPANDED) &&
        token->type == '}' &&
        parser_is_diagnostic_enabled(ctx, W_STYLE))
    {
        if (ctx->options.style == STYLE_CAKE)
        {
            if (token->prev->type == TK_BLANKS &&
                token->prev->prev->type == TK_NEWLINE)
            {
            }
            else
            {
                compiler_diagnostic_message(W_STYLE, ctx, token, "not following correct close brace style }");
            }
        }
    }
}

static void check_func_open_brace_style(struct parser_ctx* ctx, struct token* token)
{
    // token points to {

    if (token->level == 0 &&
        !(token->flags & TK_FLAG_MACRO_EXPANDED) &&
        token->type == '{' &&
        parser_is_diagnostic_enabled(ctx, W_STYLE))
    {
        if (ctx->options.style == STYLE_CAKE)
        {
            if (token->prev->type == TK_NEWLINE)
            {
            }
            else
            {
                compiler_diagnostic_message(W_STYLE, ctx, token, "not following correct brace style {");
            }
        }
    }
}
/*
static void check_func_close_brace_style(struct parser_ctx* ctx, struct token* token)
{
    //token points to {

    if (token->level == 0 &&
        !(token->flags & TK_FLAG_MACRO_EXPANDED) &&
        token->type == '}' &&
        parser_is_warning_enabled(ctx, W_STYLE))
    {
        if (ctx->options.style == STYLE_CAKE)
        {
            if (token->prev->prev->type == TK_NEWLINE)
            {
            }
            else
            {
                compiler_diagnostic_message(W_STYLE, ctx, token, "not following correct close brace style }");
            }
        }
    }
}
*/

#ifdef TEST
int printf_nothing(const char* fmt, ...) { return 0; }
#endif

void scope_destroy(struct scope* obj_owner p)
{
    hashmap_destroy(&p->tags);
    hashmap_destroy(&p->variables);
}

void scope_list_push(struct scope_list* list, struct scope* pnew)
{
    if (list->tail)
        pnew->scope_level = list->tail->scope_level + 1;

    if (list->head == NULL)
    {
        list->head = pnew;
        list->tail = pnew;
        // pnew->prev = list->tail;
    }
    else
    {
        pnew->previous = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
}

void scope_list_pop(struct scope_list* list)
{

    if (list->head == NULL)
        return;

    struct scope* p = list->tail;
    if (list->head == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {

        list->tail = list->tail->previous;
        if (list->tail == list->head)
        {
            list->tail->next = NULL;
            list->tail->previous = NULL;
        }
    }
    p->next = NULL;
    p->previous = NULL;
}

void parser_ctx_destroy(struct parser_ctx* obj_owner ctx)
{
    if (ctx->sarif_file)
    {
        fclose(ctx->sarif_file);
    }
}

_Bool compiler_diagnostic_message(enum diagnostic_id w,
    struct parser_ctx* ctx,
    const struct token* opt p_token,
    const char* fmt, ...)
{


    bool is_error = false;
    bool is_warning = false;
    bool is_note = false;

    if (w > W_NOTE)
    {
        is_error = true;
    }
    else if (w == W_LOCATION)
    {
        is_note = true;
    }
    else
    {
        is_error =
            (ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors & (1ULL << w)) != 0;

        is_warning =
            (ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings & (1ULL << w)) != 0;

        is_note =
            ((ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes & (1ULL << w)) != 0);
    }

    if (is_error)
    {
        ctx->p_report->error_count++;
    }
    else if (is_warning)
    {
        /*warnings inside headers are ignored*/
        if (p_token->level != 0)
        {
            return false;
        }

        ctx->p_report->warnings_count++;
    }
    else if (is_note)
    {
        /*notes inside headers are ignored*/
        if (p_token->level != 0)
        {
            return false;
        }

        if (w != W_LOCATION)
            ctx->p_report->info_count++;
    }
    else
    {
        return false;
    }

    if (w != W_LOCATION)
        ctx->p_report->last_diagnostic_id = w;

    const char* func_name = "module";
    if (ctx->p_current_function_opt)
    {
        func_name = ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name->lexeme;
    }

    char buffer[200] = { 0 };

    char diagnostic_name[100] = { 0 };
    get_warning_name(w, sizeof diagnostic_name, diagnostic_name);


#ifndef TEST
    if (p_token)
        print_position(p_token->token_origin->lexeme, p_token->line, p_token->col, ctx->options.visual_studio_ouput_format);

    va_list args = { 0 };
    va_start(args, fmt);
    /*int n =*/vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    //bool show_warning_name = w < W_NOTE && w != W_LOCATION;


    if (ctx->options.visual_studio_ouput_format)
    {
        if (is_error)
            printf("error: ");
        else if (is_warning)
            printf("warning: ");
        else if (is_note)
            printf("note: ");

        printf("%s", buffer);

        printf(" [%s]\n", diagnostic_name);
    }
    else
    {
        if (is_error)
        {
            printf(LIGHTRED "error: " WHITE "%s [" LIGHTRED "%s" WHITE "]\n" RESET, buffer, diagnostic_name);
        }
        else if (is_warning)
        {
            printf(LIGHTMAGENTA "warning: " WHITE "%s [" LIGHTMAGENTA "%s" WHITE "]\n" RESET, buffer, diagnostic_name);
        }
        else if (is_note)
        {
            printf(LIGHTCYAN "note: " WHITE "%s [" LIGHTCYAN "%s" WHITE "]\n" RESET, buffer, diagnostic_name);
        }
    }

    print_line_and_token(p_token, ctx->options.visual_studio_ouput_format);
#endif

    if (ctx->sarif_file)
    {


        if (ctx->p_report->error_count + ctx->p_report->warnings_count + ctx->p_report->info_count > 1)
        {
            fprintf(ctx->sarif_file, ",\n");
        }

        fprintf(ctx->sarif_file, "   {\n");
        fprintf(ctx->sarif_file, "     \"ruleId\":\"%s\",\n", diagnostic_name);

        fprintf(ctx->sarif_file, "     \"level\":\"warning\",\n");

        fprintf(ctx->sarif_file, "     \"message\": {\n");
        fprintf(ctx->sarif_file, "            \"text\": \"%s\"\n", buffer);
        fprintf(ctx->sarif_file, "      },\n");
        fprintf(ctx->sarif_file, "      \"locations\": [\n");
        fprintf(ctx->sarif_file, "       {\n");

        fprintf(ctx->sarif_file, "       \"physicalLocation\": {\n");

        fprintf(ctx->sarif_file, "             \"artifactLocation\": {\n");
        fprintf(ctx->sarif_file, "                 \"uri\": \"file:///%s\"\n", p_token->token_origin->lexeme);
        fprintf(ctx->sarif_file, "              },\n");

        fprintf(ctx->sarif_file, "              \"region\": {\n");
        fprintf(ctx->sarif_file, "                  \"startLine\": %d,\n", p_token->line);
        fprintf(ctx->sarif_file, "                  \"startColumn\": %d,\n", p_token->col);
        fprintf(ctx->sarif_file, "                  \"endLine\": %d,\n", p_token->line);
        fprintf(ctx->sarif_file, "                  \"endColumn\": %d\n", p_token->col);
        fprintf(ctx->sarif_file, "               }\n");
        fprintf(ctx->sarif_file, "         },\n");

        fprintf(ctx->sarif_file, "         \"logicalLocations\": [\n");
        fprintf(ctx->sarif_file, "          {\n");

        fprintf(ctx->sarif_file, "              \"fullyQualifiedName\": \"%s\",\n", func_name);
        fprintf(ctx->sarif_file, "              \"decoratedName\": \"%s\",\n", func_name);

        fprintf(ctx->sarif_file, "              \"kind\": \"%s\"\n", "function");
        fprintf(ctx->sarif_file, "          }\n");

        fprintf(ctx->sarif_file, "         ]\n");

        fprintf(ctx->sarif_file, "       }\n");
        fprintf(ctx->sarif_file, "     ]\n");

        fprintf(ctx->sarif_file, "   }\n");
    }

    return 1;
}

void print_scope(struct scope_list* e)
{
    printf("--- begin of scope---\n");
    struct scope* p = e->head;
    int level = 0;
    while (p)
    {
        if (p->variables.table)
        {
            for (int i = 0; i < p->variables.capacity; i++)
            {
                if (p->variables.table[i])
                {
                    for (int k = 0; k < level; k++)
                        printf(" ");
                    printf("%s\n", p->variables.table[i]->key);
                }
            }

            for (int i = 0; i < p->tags.capacity; i++)
            {
                if (p->tags.table[i])
                {
                    for (int k = 0; k < level; k++)
                        printf(" ");
                    printf("tag %s\n", p->tags.table[i]->key);
                }
            }
        }

        level++;
        p = p->next;
    }
    printf("--- end of scope---\n");
}

bool first_of_function_specifier_token(struct token* token)
{
    if (token == NULL)
        return false;

    return token->type == TK_KEYWORD_INLINE ||
        token->type == TK_KEYWORD__NORETURN;
}

bool first_is(struct parser_ctx* ctx, enum token_type type)
{
    return ctx->current != NULL && ctx->current->type == type;
}

bool first_of_function_specifier(struct parser_ctx* ctx)
{
    return first_of_function_specifier_token(ctx->current);
}

bool first_of_enum_specifier_token(struct token* token)
{
    if (token == NULL)
        return false;
    return token->type == TK_KEYWORD_ENUM;
}

bool first_of_enum_specifier(struct parser_ctx* ctx)
{
    return first_of_enum_specifier_token(ctx->current);
}

bool first_of_alignment_specifier(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;
    return ctx->current->type == TK_KEYWORD__ALIGNAS;
}

bool first_of_atomic_type_specifier(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    /*
      If the _Atomic keyword is immediately followed by a left parenthesis, it is interpreted
      as a type specifier (with a type name), not as a type qualifier.
    */

    if (ctx->current->type == TK_KEYWORD__ATOMIC)
    {
        struct token* ahead = parser_look_ahead(ctx);
        if (ahead != NULL)
        {
            return ahead->type == '(';
        }
    }
    return false;
}

bool first_of_storage_class_specifier(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD_TYPEDEF ||
        ctx->current->type == TK_KEYWORD_CONSTEXPR ||
        ctx->current->type == TK_KEYWORD_EXTERN ||
        ctx->current->type == TK_KEYWORD_STATIC ||
        ctx->current->type == TK_KEYWORD__THREAD_LOCAL ||
        ctx->current->type == TK_KEYWORD_AUTO ||
        ctx->current->type == TK_KEYWORD_REGISTER;
}

bool first_of_struct_or_union_token(struct token* token)
{
    return token->type == TK_KEYWORD_STRUCT || token->type == TK_KEYWORD_UNION;
}

bool first_of_struct_or_union(struct parser_ctx* ctx)
{
    return first_of_struct_or_union_token(ctx->current);
}

bool first_of_type_qualifier_token(struct token* p_token)
{
    if (p_token == NULL)
        return false;

    return p_token->type == TK_KEYWORD_CONST ||
        p_token->type == TK_KEYWORD_RESTRICT ||
        p_token->type == TK_KEYWORD_VOLATILE ||
        p_token->type == TK_KEYWORD__ATOMIC ||

        /*extensions*/
        p_token->type == TK_KEYWORD__OUT ||
        p_token->type == TK_KEYWORD__OWNER ||
        p_token->type == TK_KEYWORD__OBJ_OWNER ||
        p_token->type == TK_KEYWORD__VIEW ||
        p_token->type == TK_KEYWORD__OPT;

    //__fastcall
    //__stdcall
}

bool first_of_type_qualifier(struct parser_ctx* ctx)
{
    return first_of_type_qualifier_token(ctx->current);
}

struct map_entry* find_tag(struct parser_ctx* ctx, const char* lexeme)
{
    struct scope* scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry* p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry)
        {
            return p_entry;
        }
        scope = scope->previous;
    }
    return NULL;
}

struct map_entry* find_variables(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt)
{
    if (ppscope_opt != NULL)
        *ppscope_opt = NULL; // out

    struct scope* scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry* p_entry = hashmap_find(&scope->variables, lexeme);
        if (p_entry)
        {
            if (ppscope_opt)
                *ppscope_opt = scope;
            return p_entry;
        }
        scope = scope->previous;
    }
    return NULL;
}

struct enum_specifier* find_enum_specifier(struct parser_ctx* ctx, const char* lexeme)
{
    struct enum_specifier* best = NULL;
    struct scope* scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry* p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry &&
            p_entry->type == TAG_TYPE_ENUN_SPECIFIER)
        {
            best = p_entry->p;
            if (best->enumerator_list.head != NULL)
                return best; // OK bem completo
            else
            {
                // nao eh completo vamos continuar subindo
            }
        }
        scope = scope->previous;
    }
    return best; // mesmo que nao seja tao completo vamos retornar.
}

struct struct_or_union_specifier* find_struct_or_union_specifier(struct parser_ctx* ctx, const char* lexeme)
{
    struct struct_or_union_specifier* p = NULL;
    struct scope* scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry* p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry &&
            p_entry->type == TAG_TYPE_STRUCT_OR_UNION_SPECIFIER)
        {
            p = p_entry->p;
            break;
        }
        scope = scope->previous;
    }
    return p;
}

struct declarator* find_declarator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt)
{
    struct map_entry* p_entry = find_variables(ctx, lexeme, ppscope_opt);

    if (p_entry)
    {
        if (p_entry->type == TAG_TYPE_INIT_DECLARATOR)
        {
            struct init_declarator* p_init_declarator = p_entry->p;
            return (struct declarator*)p_init_declarator->p_declarator;
        }
        else if (p_entry->type == TAG_TYPE_ONLY_DECLARATOR)
        {
            return p_entry->p;
        }
    }

    return NULL;
}

struct enumerator* find_enumerator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt)
{
    struct map_entry* p_entry = find_variables(ctx, lexeme, ppscope_opt);

    if (p_entry && p_entry->type == TAG_TYPE_ENUMERATOR)
        return p_entry->p;

    return NULL;
}

bool first_of_typedef_name(struct parser_ctx* ctx, struct token* p_token)
{
    if (p_token == NULL)
        return false;

    if (p_token->type != TK_IDENTIFIER)
    {
        // nao precisa verificar
        return false;
    }
    if (p_token->flags & TK_FLAG_IDENTIFIER_IS_TYPEDEF)
    {
        // ja foi verificado que eh typedef
        return true;
    }
    if (p_token->flags & TK_FLAG_IDENTIFIER_IS_NOT_TYPEDEF)
    {
        // ja foi verificado que NAO eh typedef
        return false;
    }

    struct declarator* pdeclarator = find_declarator(ctx, p_token->lexeme, NULL);

    // pdeclarator->declaration_specifiers->
    if (pdeclarator &&
        pdeclarator->declaration_specifiers &&
        (pdeclarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF))
    {
        pdeclarator->num_uses++;
        p_token->flags |= TK_FLAG_IDENTIFIER_IS_TYPEDEF;
        return true;
    }
    else
    {
        p_token->flags |= TK_FLAG_IDENTIFIER_IS_NOT_TYPEDEF;
    }
    return false;
}

bool first_of_type_specifier(struct parser_ctx* ctx);
bool first_of_type_specifier_token(struct parser_ctx* ctx, struct token* token);

bool first_of_type_name_ahead(struct parser_ctx* ctx)
{

    if (ctx->current == NULL)
        return false;

    if (ctx->current->type != '(')
        return false;
    struct token* token_ahead = parser_look_ahead(ctx);
    return first_of_type_specifier_token(ctx, token_ahead) ||
        first_of_type_qualifier_token(token_ahead);
}

bool first_of_type_name(struct parser_ctx* ctx)
{
    return first_of_type_specifier(ctx) || first_of_type_qualifier(ctx);
}

bool first_of_type_specifier_token(struct parser_ctx* ctx, struct token* p_token)
{
    if (p_token == NULL)
        return false;

    // if (ctx->)
    return p_token->type == TK_KEYWORD_VOID ||
        p_token->type == TK_KEYWORD_CHAR ||
        p_token->type == TK_KEYWORD_SHORT ||
        p_token->type == TK_KEYWORD_INT ||
        p_token->type == TK_KEYWORD_LONG ||

        // microsoft extension
        p_token->type == TK_KEYWORD__INT8 ||
        p_token->type == TK_KEYWORD__INT16 ||
        p_token->type == TK_KEYWORD__INT32 ||
        p_token->type == TK_KEYWORD__INT64 ||

        // end microsoft

        p_token->type == TK_KEYWORD_FLOAT ||
        p_token->type == TK_KEYWORD_DOUBLE ||
        p_token->type == TK_KEYWORD_SIGNED ||
        p_token->type == TK_KEYWORD_UNSIGNED ||
        p_token->type == TK_KEYWORD__BOOL ||
        p_token->type == TK_KEYWORD__COMPLEX ||
        p_token->type == TK_KEYWORD__DECIMAL32 ||
        p_token->type == TK_KEYWORD__DECIMAL64 ||
        p_token->type == TK_KEYWORD__DECIMAL128 ||
        p_token->type == TK_KEYWORD_TYPEOF ||        // C23
        p_token->type == TK_KEYWORD_TYPEOF_UNQUAL || // C23
        first_of_atomic_type_specifier(ctx) ||
        first_of_struct_or_union_token(p_token) ||
        first_of_enum_specifier_token(p_token) ||
        first_of_typedef_name(ctx, p_token);
}

bool first_of_type_specifier(struct parser_ctx* ctx)
{
    return first_of_type_specifier_token(ctx, ctx->current);
}

bool first_of_type_specifier_qualifier(struct parser_ctx* ctx)
{
    return first_of_type_specifier(ctx) ||
        first_of_type_qualifier(ctx) ||
        first_of_alignment_specifier(ctx);
}

bool first_of_compound_statement(struct parser_ctx* ctx)
{
    return first_is(ctx, '{');
}

bool first_of_jump_statement(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD_GOTO ||
        ctx->current->type == TK_KEYWORD_CONTINUE ||
        ctx->current->type == TK_KEYWORD_BREAK ||
        ctx->current->type == TK_KEYWORD_RETURN ||
        ctx->current->type == TK_KEYWORD_THROW /*extension*/;
}

bool first_of_selection_statement(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD_IF ||
        ctx->current->type == TK_KEYWORD_SWITCH;
}

bool first_of_iteration_statement(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD_WHILE ||
        ctx->current->type == TK_KEYWORD_DO ||
        ctx->current->type == TK_KEYWORD_FOR;
}

bool first_of_label(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    if (ctx->current->type == TK_IDENTIFIER)
    {
        struct token* next = parser_look_ahead(ctx);
        return next && next->type == ':';
    }
    else if (ctx->current->type == TK_KEYWORD_CASE)
    {
        return true;
    }
    else if (ctx->current->type == TK_KEYWORD_DEFAULT)
    {
        return true;
    }

    return false;
}

bool first_of_declaration_specifier(struct parser_ctx* ctx)
{
    /*
    declaration-specifier:
    storage-class-specifier
    type-specifier-qualifier
    function-specifier
    */
    return first_of_storage_class_specifier(ctx) ||
        first_of_function_specifier(ctx) ||
        first_of_type_specifier_qualifier(ctx);
}

bool first_of_pragma_declaration(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_PRAGMA;
}

bool first_of_static_assert_declaration(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD__STATIC_ASSERT ||
        ctx->current->type == TK_KEYWORD_STATIC_DEBUG ||
        ctx->current->type == TK_KEYWORD_STATIC_DEBUG_EX ||
        ctx->current->type == TK_KEYWORD_STATIC_STATE ||
        ctx->current->type == TK_KEYWORD_STATIC_SET;
}

bool first_of_attribute_specifier(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    if (ctx->current->type != '[')
    {
        return false;
    }
    struct token* p_token = parser_look_ahead(ctx);
    return p_token != NULL && p_token->type == '[';
}

bool first_of_labeled_statement(struct parser_ctx* ctx)
{
    return first_of_label(ctx);
}

bool first_of_designator(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == '[' || ctx->current->type == '.';
}

struct token* previous_parser_token(struct token* token)
{
    if (token == NULL)
    {
        return NULL;
    }
    struct token* r = token->prev;
    while (!(r->flags & TK_FLAG_FINAL))
    {
        r = r->prev;
    }

    return r;
}

enum token_type is_keyword(const char* text)
{
    enum token_type result = 0;
    switch (text[0])
    {
    case 'a':
        if (strcmp("alignof", text) == 0)
            result = TK_KEYWORD__ALIGNOF;
        else if (strcmp("auto", text) == 0)
            result = TK_KEYWORD_AUTO;
        else if (strcmp("alignas", text) == 0)
            result = TK_KEYWORD__ALIGNAS; /*C23 alternate spelling _Alignas*/
        else if (strcmp("alignof", text) == 0)
            result = TK_KEYWORD__ALIGNAS; /*C23 alternate spelling _Alignof*/
        else if (strcmp("assert", text) == 0)
            result = TK_KEYWORD_ASSERT; /*extension*/
        break;
    case 'b':
        if (strcmp("break", text) == 0)
            result = TK_KEYWORD_BREAK;
        else if (strcmp("bool", text) == 0)
            result = TK_KEYWORD__BOOL; /*C23 alternate spelling _Bool*/

        break;
    case 'c':
        if (strcmp("case", text) == 0)
            result = TK_KEYWORD_CASE;
        else if (strcmp("char", text) == 0)
            result = TK_KEYWORD_CHAR;
        else if (strcmp("const", text) == 0)
            result = TK_KEYWORD_CONST;
        else if (strcmp("constexpr", text) == 0)
            result = TK_KEYWORD_CONSTEXPR;
        else if (strcmp("continue", text) == 0)
            result = TK_KEYWORD_CONTINUE;
        else if (strcmp("catch", text) == 0)
            result = TK_KEYWORD_CATCH;
        break;
    case 'd':
        if (strcmp("default", text) == 0)
            result = TK_KEYWORD_DEFAULT;
        else if (strcmp("do", text) == 0)
            result = TK_KEYWORD_DO;
        else if (strcmp("defer", text) == 0)
            result = TK_KEYWORD_DEFER;
        else if (strcmp("double", text) == 0)
            result = TK_KEYWORD_DOUBLE;
        break;
    case 'e':
        if (strcmp("else", text) == 0)
            result = TK_KEYWORD_ELSE;
        else if (strcmp("enum", text) == 0)
            result = TK_KEYWORD_ENUM;
        else if (strcmp("extern", text) == 0)
            result = TK_KEYWORD_EXTERN;
        break;
    case 'f':
        if (strcmp("float", text) == 0)
            result = TK_KEYWORD_FLOAT;
        else if (strcmp("for", text) == 0)
            result = TK_KEYWORD_FOR;
        else if (strcmp("false", text) == 0)
            result = TK_KEYWORD_FALSE;
        break;
    case 'g':
        if (strcmp("goto", text) == 0)
            result = TK_KEYWORD_GOTO;
        break;
    case 'i':
        if (strcmp("if", text) == 0)
            result = TK_KEYWORD_IF;
        else if (strcmp("inline", text) == 0)
            result = TK_KEYWORD_INLINE;
        else if (strcmp("int", text) == 0)
            result = TK_KEYWORD_INT;
        break;
    case 'n':
        if (strcmp("nullptr", text) == 0)
            result = TK_KEYWORD_NULLPTR;
        break;

    case 'l':
        if (strcmp("long", text) == 0)
            result = TK_KEYWORD_LONG;
        break;
    case 'r':
        if (strcmp("register", text) == 0)
            result = TK_KEYWORD_REGISTER;
        else if (strcmp("restrict", text) == 0)
            result = TK_KEYWORD_RESTRICT;
        else if (strcmp("return", text) == 0)
            result = TK_KEYWORD_RETURN;

        break;
    case 's':
        if (strcmp("short", text) == 0)
            result = TK_KEYWORD_SHORT;
        else if (strcmp("signed", text) == 0)
            result = TK_KEYWORD_SIGNED;
        else if (strcmp("sizeof", text) == 0)
            result = TK_KEYWORD_SIZEOF;
        else if (strcmp("static", text) == 0)
            result = TK_KEYWORD_STATIC;
        else if (strcmp("struct", text) == 0)
            result = TK_KEYWORD_STRUCT;
        else if (strcmp("switch", text) == 0)
            result = TK_KEYWORD_SWITCH;
        else if (strcmp("static_assert", text) == 0)
            result = TK_KEYWORD__STATIC_ASSERT; /*C23 alternate spelling _Static_assert*/
        else if (strcmp("static_debug", text) == 0)
            result = TK_KEYWORD_STATIC_DEBUG;
        else if (strcmp("static_debug_ex", text) == 0)
            result = TK_KEYWORD_STATIC_DEBUG_EX;
        else if (strcmp("static_state", text) == 0)
            result = TK_KEYWORD_STATIC_STATE;
        else if (strcmp("static_set", text) == 0)
            result = TK_KEYWORD_STATIC_SET;

        break;
    case 't':
        if (strcmp("typedef", text) == 0)
            result = TK_KEYWORD_TYPEDEF;
        else if (strcmp("typeof", text) == 0)
            result = TK_KEYWORD_TYPEOF; /*C23*/
        else if (strcmp("typeof_unqual", text) == 0)
            result = TK_KEYWORD_TYPEOF_UNQUAL; /*C23*/
        else if (strcmp("true", text) == 0)
            result = TK_KEYWORD_TRUE; /*C23*/
        else if (strcmp("thread_local", text) == 0)
            result = TK_KEYWORD__THREAD_LOCAL; /*C23 alternate spelling _Thread_local*/
        else if (strcmp("try", text) == 0)
            result = TK_KEYWORD_TRY;
        else if (strcmp("throw", text) == 0)
            result = TK_KEYWORD_THROW;
        break;
    case 'u':
        if (strcmp("union", text) == 0)
            result = TK_KEYWORD_UNION;
        else if (strcmp("unsigned", text) == 0)
            result = TK_KEYWORD_UNSIGNED;
        break;
    case 'v':
        if (strcmp("void", text) == 0)
            result = TK_KEYWORD_VOID;
        else if (strcmp("volatile", text) == 0)
            result = TK_KEYWORD_VOLATILE;

        break;
    case 'w':
        if (strcmp("while", text) == 0)
            result = TK_KEYWORD_WHILE;
        break;
    case '_':


        //
        // end microsoft

        /*ownership*/
        if (strcmp("_Out", text) == 0)
            result = TK_KEYWORD__OUT; /*extension*/
        else if (strcmp("_Owner", text) == 0)
            result = TK_KEYWORD__OWNER; /*extension*/
        else if (strcmp("_Obj_owner", text) == 0)
            result = TK_KEYWORD__OBJ_OWNER; /*extension*/
        else if (strcmp("_Opt", text) == 0)
            result = TK_KEYWORD__OPT; /*extension*/

        else if (strcmp("_View", text) == 0)
            result = TK_KEYWORD__VIEW; /*extension*/

        /*EXPERIMENTAL EXTENSION*/
        else if (strcmp("_has_attr", text) == 0)
            result = TK_KEYWORD_ATTR_HAS;
        else if (strcmp("_add_attr", text) == 0)
            result = TK_KEYWORD_ATTR_ADD;
        else if (strcmp("_del_attr", text) == 0)
            result = TK_KEYWORD_ATTR_REMOVE;
        /*EXPERIMENTAL EXTENSION*/

        /*TRAITS EXTENSION*/
        else if (strcmp("_is_lvalue", text) == 0)
            result = TK_KEYWORD_IS_LVALUE;
        else if (strcmp("_is_const", text) == 0)
            result = TK_KEYWORD_IS_CONST;
        else if (strcmp("_is_owner", text) == 0)
            result = TK_KEYWORD_IS_OWNER;
        else if (strcmp("_is_pointer", text) == 0)
            result = TK_KEYWORD_IS_POINTER;
        else if (strcmp("_is_array", text) == 0)
            result = TK_KEYWORD_IS_ARRAY;
        else if (strcmp("_is_function", text) == 0)
            result = TK_KEYWORD_IS_FUNCTION;
        else if (strcmp("_is_arithmetic", text) == 0)
            result = TK_KEYWORD_IS_ARITHMETIC;
        else if (strcmp("_is_floating_point", text) == 0)
            result = TK_KEYWORD_IS_FLOATING_POINT;
        else if (strcmp("_is_integral", text) == 0)
            result = TK_KEYWORD_IS_INTEGRAL;
        else if (strcmp("_is_scalar", text) == 0)
            result = TK_KEYWORD_IS_SCALAR;
        /*TRAITS EXTENSION*/

        else if (strcmp("_Alignof", text) == 0)
            result = TK_KEYWORD__ALIGNOF;
        else if (strcmp("_Alignas", text) == 0)
            result = TK_KEYWORD__ALIGNAS;
        else if (strcmp("_Atomic", text) == 0)
            result = TK_KEYWORD__ATOMIC;
        else if (strcmp("_Bool", text) == 0)
            result = TK_KEYWORD__BOOL;
        else if (strcmp("_Complex", text) == 0)
            result = TK_KEYWORD__COMPLEX;
        else if (strcmp("_Decimal128", text) == 0)
            result = TK_KEYWORD__DECIMAL32;
        else if (strcmp("_Decimal64", text) == 0)
            result = TK_KEYWORD__DECIMAL64;
        else if (strcmp("_Decimal128", text) == 0)
            result = TK_KEYWORD__DECIMAL128;
        else if (strcmp("_Generic", text) == 0)
            result = TK_KEYWORD__GENERIC;
        else if (strcmp("_Imaginary", text) == 0)
            result = TK_KEYWORD__IMAGINARY;
        else if (strcmp("_Noreturn", text) == 0)
            result = TK_KEYWORD__NORETURN; /*_Noreturn deprecated C23*/
        else if (strcmp("_Static_assert", text) == 0)
            result = TK_KEYWORD__STATIC_ASSERT;
        else if (strcmp("_Thread_local", text) == 0)
            result = TK_KEYWORD__THREAD_LOCAL;
        else if (strcmp("_BitInt", text) == 0)
            result = TK_KEYWORD__BITINT; /*(C23)*/
        else if (strcmp("__typeof__", text) == 0)
            result = TK_KEYWORD_TYPEOF; /*(C23)*/
#ifdef  _MSC_VER
        // begin microsoft
        else if (strcmp("__int8", text) == 0)
            result = TK_KEYWORD__INT8;
        else if (strcmp("__int16", text) == 0)
            result = TK_KEYWORD__INT16;
        else if (strcmp("__int32", text) == 0)
            result = TK_KEYWORD__INT32;
        else if (strcmp("__int64", text) == 0)
            result = TK_KEYWORD__INT64;
        else if (strcmp("__forceinline", text) == 0)
            result = TK_KEYWORD_INLINE;
        else if (strcmp("__inline", text) == 0)
            result = TK_KEYWORD_INLINE;
        else if (strcmp("_asm", text) == 0 || strcmp("__asm", text) == 0)
            result = TK_KEYWORD__ASM;
        else if (strcmp("__alignof", text) == 0)
            result = TK_KEYWORD__ALIGNOF;
        else if (strcmp("__restrict", text) == 0)
            result = TK_KEYWORD_RESTRICT;
#endif
        break;
    default:
        break;
    }
    return result;
}


static void token_promote(struct token* token)
{
    if (token->type == TK_IDENTIFIER_RECURSIVE_MACRO)
    {
        // talvez desse para remover antesisso..
        // assim que sai do tetris
        // virou passado
        token->type = TK_IDENTIFIER; /*nao precisamos mais disso*/
    }

    if (token->type == TK_IDENTIFIER)
    {
        enum token_type t = is_keyword(token->lexeme);
        if (t != TK_NONE)
            token->type = t;
    }
    else if (token->type == TK_PPNUMBER)
    {
        token->type = parse_number(token->lexeme, NULL);
    }
}

struct token* parser_look_ahead(struct parser_ctx* ctx)
{
    struct token* p = ctx->current->next;
    while (p && !(p->flags & TK_FLAG_FINAL))
    {
        p = p->next;
    }

    if (p)
    {

        token_promote(p);
    }

    return p;
}

bool is_binary_digit(struct stream* stream)
{
    return stream->current[0] >= '0' && stream->current[0] <= '1';
}

bool is_hexadecimal_digit(struct stream* stream)
{
    return (stream->current[0] >= '0' && stream->current[0] <= '9') ||
        (stream->current[0] >= 'a' && stream->current[0] <= 'f') ||
        (stream->current[0] >= 'A' && stream->current[0] <= 'F');
}

bool is_octal_digit(struct stream* stream)
{
    return stream->current[0] >= '0' && stream->current[0] <= '7';
}

void digit_sequence(struct stream* stream)
{
    while (is_digit(stream))
    {
        stream_match(stream);
    }
}

static void binary_exponent_part(struct stream* stream)
{
    // p signopt digit - sequence
    // P   signopt digit - sequence

    stream_match(stream); // p or P
    if (stream->current[0] == '+' || stream->current[0] == '-')
    {
        stream_match(stream); // p or P
    }
    digit_sequence(stream);
}

void hexadecimal_digit_sequence(struct stream* stream)
{
    /*
     hexadecimal-digit-sequence:
     hexadecimal-digit
     hexadecimal-digit ’opt hexadecimal-digit
    */

    stream_match(stream);
    while (stream->current[0] == '\'' ||
        is_hexadecimal_digit(stream))
    {
        if (stream->current[0] == '\'')
        {
            stream_match(stream);
            if (!is_hexadecimal_digit(stream))
            {
                // erro
            }
            stream_match(stream);
        }
        else
            stream_match(stream);
    }
}

bool first_of_unsigned_suffix(struct stream* stream)
{
    /*
     unsigned-suffix: one of
       u U
     */
    return (stream->current[0] == 'u' ||
        stream->current[0] == 'U');
}

void unsigned_suffix_opt(struct stream* stream)
{
    /*
   unsigned-suffix: one of
     u U
   */
    if (stream->current[0] == 'u' ||
        stream->current[0] == 'U')
    {
        stream_match(stream);
    }
}

void integer_suffix_opt(struct stream* stream, enum type_specifier_flags* flags_opt)
{
    /*
     integer-suffix:
     unsigned-suffix long-suffixopt
     unsigned-suffix long-long-suffix
     long-suffix unsigned-suffixopt
     long-long-suffix unsigned-suffixopt
    */

    if (/*unsigned-suffix*/
        stream->current[0] == 'U' || stream->current[0] == 'u')
    {
        stream_match(stream);

        if (flags_opt)
        {
            *flags_opt |= TYPE_SPECIFIER_UNSIGNED;
        }

        /*long-suffixopt*/
        if (stream->current[0] == 'l' || stream->current[0] == 'L')
        {
            if (flags_opt)
            {
                *flags_opt = *flags_opt & ~TYPE_SPECIFIER_INT;
                *flags_opt |= TYPE_SPECIFIER_LONG;
            }
            stream_match(stream);
        }

        /*long-long-suffix*/
        if (stream->current[0] == 'l' || stream->current[0] == 'L')
        {
            if (flags_opt)
            {
                *flags_opt = *flags_opt & ~TYPE_SPECIFIER_LONG;
                *flags_opt |= TYPE_SPECIFIER_LONG_LONG;
            }

            stream_match(stream);
        }
    }
    else if ((stream->current[0] == 'l' || stream->current[0] == 'L'))
    {
        if (flags_opt)
        {
            *flags_opt = *flags_opt & ~TYPE_SPECIFIER_INT;
            *flags_opt |= TYPE_SPECIFIER_LONG;
        }

        /*long-suffix*/
        stream_match(stream);

        /*long-long-suffix*/
        if ((stream->current[0] == 'l' || stream->current[0] == 'L'))
        {
            if (flags_opt)
            {
                *flags_opt = *flags_opt & ~TYPE_SPECIFIER_LONG;
                *flags_opt |= TYPE_SPECIFIER_LONG_LONG;
            }
            stream_match(stream);
        }

        if (/*unsigned-suffix*/
            stream->current[0] == 'U' || stream->current[0] == 'u')
        {
            if (flags_opt)
            {
                *flags_opt |= TYPE_SPECIFIER_UNSIGNED;
            }
            stream_match(stream);
        }
    }
}

void exponent_part_opt(struct stream* stream)
{
    /*
    exponent-part:
    e signopt digit-sequence
    E signopt digit-sequence
    */
    if (stream->current[0] == 'e' || stream->current[0] == 'E')
    {
        stream_match(stream);

        if (stream->current[0] == '-' || stream->current[0] == '+')
        {
            stream_match(stream);
        }
        digit_sequence(stream);
    }
}

enum type_specifier_flags floating_suffix_opt(struct stream* stream)
{
    enum type_specifier_flags f = TYPE_SPECIFIER_DOUBLE;

    if (stream->current[0] == 'l' || stream->current[0] == 'L')
    {
        f = TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_DOUBLE;
        stream_match(stream);
    }
    else if (stream->current[0] == 'f' || stream->current[0] == 'F')
    {
        f = TYPE_SPECIFIER_FLOAT;
        stream_match(stream);
    }

    return f;
}

bool is_nonzero_digit(struct stream* stream)
{
    return stream->current[0] >= '1' && stream->current[0] <= '9';
}

enum token_type parse_number_core(struct stream* stream, enum type_specifier_flags* flags_opt)
{
    if (flags_opt)
    {
        *flags_opt = TYPE_SPECIFIER_INT;
    }

    enum token_type type = TK_NONE;
    if (stream->current[0] == '.')
    {
        type = TK_COMPILER_DECIMAL_FLOATING_CONSTANT;
        stream_match(stream);
        digit_sequence(stream);
        exponent_part_opt(stream);
        enum type_specifier_flags f = floating_suffix_opt(stream);
        if (flags_opt)
        {
            *flags_opt = f;
        }
    }
    else if (stream->current[0] == '0' && (stream->current[1] == 'x' || stream->current[1] == 'X'))
    {
        type = TK_COMPILER_HEXADECIMAL_CONSTANT;

        stream_match(stream);
        stream_match(stream);
        while (is_hexadecimal_digit(stream))
        {
            stream_match(stream);
        }

        integer_suffix_opt(stream, flags_opt);

        if (stream->current[0] == '.')
        {
            type = TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT;
            hexadecimal_digit_sequence(stream);
        }

        if (stream->current[0] == 'p' ||
            stream->current[0] == 'P')
        {
            type = TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT;
            binary_exponent_part(stream);
        }

        if (type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT)
        {
            enum type_specifier_flags f = floating_suffix_opt(stream);
            if (flags_opt)
            {
                *flags_opt = f;
            }
        }
    }
    else if (stream->current[0] == '0' && (stream->current[1] == 'b' || stream->current[1] == 'B'))
    {
        type = TK_COMPILER_BINARY_CONSTANT;
        stream_match(stream);
        stream_match(stream);
        while (is_binary_digit(stream))
        {
            stream_match(stream);
        }
        integer_suffix_opt(stream, flags_opt);
    }
    else if (stream->current[0] == '0') // octal
    {
        type = TK_COMPILER_OCTAL_CONSTANT;

        stream_match(stream);
        while (is_octal_digit(stream))
        {
            stream_match(stream);
        }
        integer_suffix_opt(stream, flags_opt);

        if (stream->current[0] == '.')
        {
            hexadecimal_digit_sequence(stream);
            enum type_specifier_flags f = floating_suffix_opt(stream);
            if (flags_opt)
            {
                *flags_opt = f;
            }
        }
    }
    else if (is_nonzero_digit(stream)) // decimal
    {
        type = TK_COMPILER_DECIMAL_CONSTANT;

        stream_match(stream);
        while (is_digit(stream))
        {
            stream_match(stream);
        }
        integer_suffix_opt(stream, flags_opt);

        if (stream->current[0] == 'e' || stream->current[0] == 'E')
        {
            exponent_part_opt(stream);
            enum type_specifier_flags f = floating_suffix_opt(stream);
            if (flags_opt)
            {
                *flags_opt = f;
            }
        }
        else if (stream->current[0] == '.')
        {
            stream_match(stream);
            type = TK_COMPILER_DECIMAL_FLOATING_CONSTANT;
            digit_sequence(stream);
            exponent_part_opt(stream);
            enum type_specifier_flags f = floating_suffix_opt(stream);
            if (flags_opt)
            {
                *flags_opt = f;
            }
        }
    }

    return type;
}

enum token_type parse_number(const char* lexeme, enum type_specifier_flags* flags_opt)
{
    struct stream stream = { .source = lexeme, .current = lexeme, .line = 1, .col = 1 };
    return parse_number_core(&stream, flags_opt);
}

static struct token* pragma_match(struct token* current)
{
    current = current->next;
    while (current && current->type == TK_BLANKS)
    {
        current = current->next;
    }
    return current;
}

static void pragma_skip_blanks(struct parser_ctx* ctx)
{
    while (ctx->current && ctx->current->type == TK_BLANKS)
    {
        ctx->current = ctx->current->next;
    }
}

/*
 * Some pragmas needs to be handled by the compiler
 */
static void parse_pragma(struct parser_ctx* ctx, struct token* token)
{
    if (ctx->current->type == TK_PRAGMA)
    {
        ctx->current = ctx->current->next;
        pragma_skip_blanks(ctx);

        if (ctx->current &&
            (strcmp(ctx->current->lexeme, "CAKE") == 0 ||
                strcmp(ctx->current->lexeme, "cake") == 0))
        {
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);
        }

        if (ctx->current && strcmp(ctx->current->lexeme, "nullchecks") == 0)
        {
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);

            // Isso nao esta funcionando pois esta informao precisa estar na AST.
            // pois eh usada em um segundo passo.
            bool onoff = false;
            if (ctx->current && strcmp(ctx->current->lexeme, "ON") == 0)
            {
                onoff = true;
            }
            else if (ctx->current && strcmp(ctx->current->lexeme, "OFF") == 0)
            {
                onoff = false;
            }
            else
            {
                compiler_diagnostic_message(C_ERROR_PRAGMA_ERROR, ctx, ctx->current, "nullchecks pragma needs to use ON OFF");
            }
            ctx->options.null_checks_enabled = onoff;
        }

        if (ctx->current && strcmp(ctx->current->lexeme, "diagnostic") == 0)
        {
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);

            if (ctx->current && strcmp(ctx->current->lexeme, "push") == 0)
            {
                // #pragma GCC diagnostic push
                if (ctx->options.diagnostic_stack_top_index <
                    sizeof(ctx->options.diagnostic_stack) / sizeof(ctx->options.diagnostic_stack[0]))
                {
                    ctx->options.diagnostic_stack_top_index++;
                    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index] =
                        ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index - 1];
                }
                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);
            }
            else if (ctx->current && strcmp(ctx->current->lexeme, "pop") == 0)
            {
                // #pragma CAKE diagnostic pop
                if (ctx->options.diagnostic_stack_top_index > 0)
                {
                    ctx->options.diagnostic_stack_top_index--;
                }
                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);
            }
            else if (ctx->current &&
                (strcmp(ctx->current->lexeme, "error") == 0 ||
                    strcmp(ctx->current->lexeme, "warning") == 0 ||
                    strcmp(ctx->current->lexeme, "note") == 0 ||
                    strcmp(ctx->current->lexeme, "ignored") == 0))
            {
                const bool is_error = strcmp(ctx->current->lexeme, "error") == 0;
                const bool is_warning = strcmp(ctx->current->lexeme, "warning") == 0;
                const bool is_note = strcmp(ctx->current->lexeme, "note") == 0;

                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);

                if (ctx->current && ctx->current->type == TK_STRING_LITERAL)
                {

                    unsigned long long w = get_warning_bit_mask(ctx->current->lexeme + 1 /*+ 2*/);

                    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors &= ~w;
                    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes &= ~w;
                    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings &= ~w;

                    if (is_error)
                        ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors |= w;
                    else if (is_warning)
                        ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings |= w;
                    else if (is_note)
                        ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes |= w;
                }
            }
            else if (ctx->current &&
                (strcmp(ctx->current->lexeme, "check") == 0))
            {
                // TODO better name .  Ack. : means ‘alarm acknowledged’ ?
                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);

                if (ctx->current && ctx->current->type == TK_STRING_LITERAL)
                {
                    enum diagnostic_id id = get_warning(ctx->current->lexeme + 1 + 2);
                    if (ctx->p_report->last_diagnostic_id == id)
                    {
                        // lets remove this error/warning/info from the final report.

                        int t =
                            get_diagnostic_type(&ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index],
                                id);
                        if (t == 3)
                            ctx->p_report->error_count--;
                        else if (t == 2)
                            ctx->p_report->warnings_count--;
                        else if (t == 1)
                            ctx->p_report->info_count--;
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, "pragma check failed");
                    }
                }
            }
            else
            {
                compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, "unknown pragma");
            }
        }
    }
}
static struct token* parser_skip_blanks(struct parser_ctx* ctx)
{
    while (ctx->current && !(ctx->current->flags & TK_FLAG_FINAL))
    {

        if (ctx->current->type == TK_PRAGMA)
        {
            /*only active block have TK_PRAGMA*/
            parse_pragma(ctx, ctx->current);
        }

        if (ctx->current)
            ctx->current = ctx->current->next;
    }

    if (ctx->current)
    {
        token_promote(ctx->current); // transforma para token de parser
    }

    return ctx->current;
}

struct token* parser_match(struct parser_ctx* ctx)
{
    ctx->previous = ctx->current;
    ctx->current = ctx->current->next;
    parser_skip_blanks(ctx);

    return ctx->current;
}

NODISCARD
int parser_match_tk(struct parser_ctx* ctx, enum token_type type)
{
    int error = 0;
    if (ctx->current != NULL)
    {
        if (ctx->current->type != type)
        {
            compiler_diagnostic_message(C_ERROR_UNEXPECTED_TOKEN, ctx, ctx->current, "expected %s", get_token_name(type));
            error = 1;
        }

        ctx->previous = ctx->current;
        ctx->current = ctx->current->next;
        parser_skip_blanks(ctx);
    }
    else
    {
        compiler_diagnostic_message(C_ERROR_UNEXPECTED_TOKEN, ctx, ctx->input_list.tail, "unexpected end of file after");
        error = 1;
    }

    return error;
}

void print_declaration_specifiers(struct osstream* ss, struct declaration_specifiers* p_declaration_specifiers)
{
    bool first = true;
    print_type_qualifier_flags(ss, &first, p_declaration_specifiers->type_qualifier_flags);

    if (p_declaration_specifiers->enum_specifier)
    {

        if (p_declaration_specifiers->enum_specifier->tag_token)
        {
            ss_fprintf(ss, "enum %s", p_declaration_specifiers->enum_specifier->tag_token->lexeme);
        }
        else
        {
            assert(false);
        }
    }
    else if (p_declaration_specifiers->struct_or_union_specifier)
    {
        ss_fprintf(ss, "struct %s", p_declaration_specifiers->struct_or_union_specifier->tag_name);
    }
    else if (p_declaration_specifiers->typedef_declarator)
    {
        print_item(ss, &first, p_declaration_specifiers->typedef_declarator->name->lexeme);
    }
    else
    {
        print_type_specifier_flags(ss, &first, p_declaration_specifiers->type_specifier_flags);
    }
}

bool type_specifier_is_integer(enum type_specifier_flags flags)
{
    if ((flags & TYPE_SPECIFIER_CHAR) ||
        (flags & TYPE_SPECIFIER_SHORT) ||
        (flags & TYPE_SPECIFIER_INT) ||
        (flags & TYPE_SPECIFIER_LONG) ||
        (flags & TYPE_SPECIFIER_INT) ||
        (flags & TYPE_SPECIFIER_INT8) ||
        (flags & TYPE_SPECIFIER_INT16) ||
        (flags & TYPE_SPECIFIER_INT32) ||
        (flags & TYPE_SPECIFIER_INT64) ||
        (flags & TYPE_SPECIFIER_LONG_LONG))
    {
        return true;
    }
    return false;
}

int final_specifier(struct parser_ctx* ctx, enum type_specifier_flags* flags)
{
    if (((*flags) & TYPE_SPECIFIER_UNSIGNED) ||
        ((*flags) & TYPE_SPECIFIER_SIGNED))
    {
        if (!type_specifier_is_integer(*flags))
        {
            // se nao especificou nada vira integer
            (*flags) |= TYPE_SPECIFIER_INT;
        }
    }

    return 0;
}

int add_specifier(struct parser_ctx* ctx,
    enum type_specifier_flags* flags,
    enum type_specifier_flags new_flag)
{
    /*
     * Verifica as combinaçòes possíveis
     */

    if (new_flag & TYPE_SPECIFIER_LONG) // adicionando um long
    {
        if ((*flags) & TYPE_SPECIFIER_LONG_LONG) // ja tinha long long
        {
            compiler_diagnostic_message(C_ERROR_CANNOT_COMBINE_WITH_PREVIOUS_LONG_LONG, ctx, ctx->current, "cannot combine with previous 'long long' declaration specifier");
            return 1;
        }
        else if ((*flags) & TYPE_SPECIFIER_LONG) // ja tinha um long
        {
            (*flags) = (*flags) & ~TYPE_SPECIFIER_LONG;
            (*flags) |= TYPE_SPECIFIER_LONG_LONG;
        }
        else // nao tinha nenhum long
        {
            (*flags) = (*flags) & ~TYPE_SPECIFIER_INT;
            (*flags) |= TYPE_SPECIFIER_LONG;
        }
    }
    else
    {
        (*flags) |= new_flag;
    }
    return 0;
}

void declaration_specifiers_delete(struct declaration_specifiers* owner opt p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);

        struct declaration_specifier* owner item = p->head;
        while (item)
        {
            struct declaration_specifier* owner next = item->next;
            item->next = NULL;
            declaration_specifier_delete(item);
            item = next;
        }
        free(p);


    }
}

void declaration_specifiers_add(struct declaration_specifiers* list, struct declaration_specifier* owner p_item)
{

    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

struct declaration_specifiers* owner declaration_specifiers(struct parser_ctx* ctx,
    enum storage_class_specifier_flags default_storage_flag)
{
    /*
        declaration-specifiers:
          declaration-specifier attribute-specifier-sequence_opt
          declaration-specifier declaration-specifiers
    */

    /*
     Ao fazer parser do segundo o X ja existe mas ele nao deve ser usado
     typedef char X;
     typedef char X;
    */

    struct declaration_specifiers* owner p_declaration_specifiers = calloc(1, sizeof(struct declaration_specifiers));

    try
    {
        if (p_declaration_specifiers == NULL)
            throw;

        static_set(p_declaration_specifiers, "not-null");
        p_declaration_specifiers->first_token = ctx->current;

        while (first_of_declaration_specifier(ctx))
        {
            if (ctx->current->flags & TK_FLAG_IDENTIFIER_IS_TYPEDEF)
            {
                if (p_declaration_specifiers->type_specifier_flags != TYPE_SPECIFIER_NONE)
                {
                    // typedef tem que aparecer sozinho
                    // exemplo Socket eh nome e nao typdef
                    // typedef int Socket;
                    // struct X {int Socket;};
                    break;
                }
            }

            struct declaration_specifier* owner p_declaration_specifier = declaration_specifier(ctx);

            if (p_declaration_specifier->type_specifier_qualifier)
            {
                if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                {
                    if (add_specifier(ctx,
                        &p_declaration_specifiers->type_specifier_flags,
                        p_declaration_specifier->type_specifier_qualifier->type_specifier->flags) != 0)
                    {
                        declaration_specifier_delete(p_declaration_specifier);
                        throw;
                    }

                    if (p_declaration_specifier->type_specifier_qualifier->type_specifier->struct_or_union_specifier)
                    {
                        p_declaration_specifiers->struct_or_union_specifier = p_declaration_specifier->type_specifier_qualifier->type_specifier->struct_or_union_specifier;
                    }
                    else if (p_declaration_specifier->type_specifier_qualifier->type_specifier->enum_specifier)
                    {
                        p_declaration_specifiers->enum_specifier = p_declaration_specifier->type_specifier_qualifier->type_specifier->enum_specifier;
                    }
                    else if (p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier)
                    {
                        p_declaration_specifiers->typeof_specifier = p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier;
                    }
                    else if (p_declaration_specifier->type_specifier_qualifier->type_specifier->token &&
                        p_declaration_specifier->type_specifier_qualifier->type_specifier->token->type == TK_IDENTIFIER)
                    {
                        p_declaration_specifiers->typedef_declarator =
                            find_declarator(ctx,
                                p_declaration_specifier->type_specifier_qualifier->type_specifier->token->lexeme,
                                NULL);

                        // p_declaration_specifiers->typedef_declarator = p_declaration_specifier->type_specifier_qualifier->pType_specifier->token->lexeme;
                    }
                }
                else if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                {
                    p_declaration_specifiers->type_qualifier_flags |= p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;
                }
            }
            else if (p_declaration_specifier->storage_class_specifier)
            {
                p_declaration_specifiers->storage_class_specifier_flags |= p_declaration_specifier->storage_class_specifier->flags;
            }

            declaration_specifiers_add(p_declaration_specifiers, p_declaration_specifier);

            assert(p_declaration_specifiers->p_attribute_specifier_sequence_opt == NULL);
            p_declaration_specifiers->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);

            if (ctx->current->type == TK_IDENTIFIER &&
                p_declaration_specifiers->type_specifier_flags != TYPE_SPECIFIER_NONE)
            {
                // typedef nao pode aparecer com outro especifier
                // entao ja tem tem algo e vier identifier signfica que acabou
                // exemplo
                /*
                 typedef char X;
                 typedef char X;
                */
                break;
            }
        }
        p_declaration_specifiers->last_token = previous_parser_token(ctx->current);
    }
    catch
    {
    }

    if (p_declaration_specifiers)
    {
        // int main() { static int i; } // i is not automatic
        final_specifier(ctx, &p_declaration_specifiers->type_specifier_flags);
    }

    p_declaration_specifiers->storage_class_specifier_flags |= default_storage_flag;

    if (p_declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
    {
        //
        p_declaration_specifiers->storage_class_specifier_flags &= ~STORAGE_SPECIFIER_AUTOMATIC_STORAGE;
    }

    return p_declaration_specifiers;
}

struct declaration* owner declaration_core(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt /*SINK*/,
    bool can_be_function_definition,
    bool* is_function_definition,
    enum storage_class_specifier_flags default_storage_class_specifier_flags,
    bool without_semicolon)
{
    /*
                                  declaration-specifiers init-declarator-list_opt ;
     attribute-specifier-sequence declaration-specifiers init-declarator-list ;
     static_assert-declaration
     attribute-declaration
  */

    struct declaration* owner p_declaration = calloc(1, sizeof(struct declaration));

    try
    {
        if (p_declaration == NULL)
            throw;

        p_declaration->p_attribute_specifier_sequence_opt = p_attribute_specifier_sequence_opt;
        p_attribute_specifier_sequence_opt = NULL; /*MOVED*/

        p_declaration->first_token = ctx->current;

        if (ctx->current->type == ';')
        {
            parser_match(ctx);
            // empty declaration
            return p_declaration;
        }

        if (first_of_static_assert_declaration(ctx))
        {
            p_declaration->static_assert_declaration = static_assert_declaration(ctx);
        }
        else if (first_of_pragma_declaration(ctx))
        {
            p_declaration->pragma_declaration = pragma_declaration(ctx);
        }
        else
        {

            if (first_of_declaration_specifier(ctx))
            {
                p_declaration->declaration_specifiers = declaration_specifiers(ctx, default_storage_class_specifier_flags);

                if (p_declaration->p_attribute_specifier_sequence_opt)
                {
                    p_declaration->declaration_specifiers->attributes_flags =
                        p_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
                }

                if (ctx->current->type != ';')
                {
                    p_declaration->init_declarator_list = init_declarator_list(ctx,
                        p_declaration->declaration_specifiers);
                }

                p_declaration->last_token = ctx->current;

                if (ctx->current->type == '{')
                {
                    if (can_be_function_definition)
                        *is_function_definition = true;
                }
                else
                {

                    if (!without_semicolon && parser_match_tk(ctx, ';') != 0)
                        throw;
                }
            }
            else
            {
                if (ctx->current->type == TK_IDENTIFIER)
                {
                    compiler_diagnostic_message(C_ERROR_INVALID_TYPE, ctx, ctx->current, "invalid type '%s'", ctx->current->lexeme);
                }
                else
                {
                    compiler_diagnostic_message(C_ERROR_EXPECTED_DECLARATION, ctx, ctx->current, "expected declaration not '%s'", ctx->current->lexeme);
                }
                parser_match(ctx); // we need to go ahead
            }
        }
    }
    catch
    {
        declaration_delete(p_declaration);
        p_declaration = NULL;
    }

    attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);

    return p_declaration;
}

struct declaration* owner function_definition_or_declaration(struct parser_ctx* ctx)
{
    /*
     function-definition:
        attribute-specifier-sequence opt declaration-specifiers declarator function-body
    */

    /*
      declaration:
        declaration-specifiers                              init-declarator-list opt ;
        attribute-specifier-sequence declaration-specifiers init-declarator-list ;
        static_assert-declaration
        attribute-declaration
    */
    struct declaration* owner p_declaration = NULL;
    try
    {
        struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);

        bool is_function_definition = false;

        p_declaration = declaration_core(ctx, p_attribute_specifier_sequence_opt, true, &is_function_definition, STORAGE_SPECIFIER_EXTERN, false);
        if (p_declaration == NULL)
            throw;

        if (is_function_definition)
        {

            ctx->p_current_function_opt = p_declaration;
            // tem que ter 1 so
            // tem 1 que ter  1 cara e ser funcao
            assert(p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator);

            /*
                scope of parameters is the inner declarator

                void (*f(int i))(void) {
                    i = 1;
                    return 0;
                }
            */

            struct declarator* inner = p_declaration->init_declarator_list.head->p_declarator;
            for (;;)
            {
                if (inner->direct_declarator &&
                    inner->direct_declarator->function_declarator &&
                    inner->direct_declarator->function_declarator->direct_declarator &&
                    inner->direct_declarator->function_declarator->direct_declarator->declarator)
                {
                    inner = inner->direct_declarator->function_declarator->direct_declarator->declarator;
                }
                else
                    break;
            }

            struct scope* parameters_scope = &inner->direct_declarator->function_declarator->parameters_scope;
            scope_list_push(&ctx->scopes, parameters_scope);

            check_func_open_brace_style(ctx, ctx->current);

            struct diagnostic before_function_diagnostics = ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index];

            assert(p_declaration->function_body == NULL);
            p_declaration->function_body = function_body(ctx);
            if (p_declaration->function_body == NULL)
                throw;

            p_declaration->init_declarator_list.head->p_declarator->function_body = p_declaration->function_body;

            if (ctx->options.flow_analysis)
            {
                /*
                 Now we have the full function AST let´s visit to analise
                 jumps
                */

                /*we are going to visit the function again.. lets put the same diagnostic state*/
                ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index] = before_function_diagnostics;

                struct flow_visit_ctx ctx2 = { 0 };
                ctx2.ctx = ctx;
                flow_start_visit_declaration(&ctx2, p_declaration);
                flow_visit_ctx_destroy(&ctx2);
            }

            struct parameter_declaration* parameter = NULL;

            if (p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator &&
                p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt &&
                p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
            {
                parameter = p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
            }

            /*parametros nao usados*/
            while (parameter)
            {
                if (!type_is_maybe_unused(&parameter->declarator->type) &&
                    parameter->declarator->num_uses == 0)
                {
                    if (parameter->declarator->name &&
                        parameter->declarator->name->level == 0 /*direct source*/
                        )
                    {
                        compiler_diagnostic_message(W_UNUSED_PARAMETER,
                            ctx,
                            parameter->declarator->name,
                            "'%s': unreferenced formal parameter",
                            parameter->declarator->name->lexeme);
                    }
                }
                parameter = parameter->next;
            }

            scope_list_pop(&ctx->scopes);
            ctx->p_current_function_opt = NULL;
        }
        else
        {
            if (ctx->options.flow_analysis)
            {
                /*
                 *  The objetive of this visit is to initialize global objects.
                 *  It also executes static_debug
                 */
                struct flow_visit_ctx ctx2 = { 0 };
                ctx2.ctx = ctx;
                flow_start_visit_declaration(&ctx2, p_declaration);
                flow_visit_ctx_destroy(&ctx2);
            }
        }
    }
    catch
    {
        declaration_delete(p_declaration);
        p_declaration = NULL;
    }

    return p_declaration;
}

struct simple_declaration* owner simple_declaration(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt,
    bool ignore_semicolon)
{
    enum storage_class_specifier_flags storage_specifier_flags = STORAGE_SPECIFIER_AUTOMATIC_STORAGE;
    /*
      simple-declaration:
      declaration-specifiers init-declarator-list opt ;
      attribute-specifier-sequence declaration-specifiers init-declarator-list ;
    */

    struct simple_declaration* owner p_simple_declaration = calloc(1, sizeof(struct simple_declaration));

    try
    {
        if (p_simple_declaration == NULL)
            throw;


        p_simple_declaration->first_token = ctx->current;

        p_simple_declaration->p_attribute_specifier_sequence_opt = p_attribute_specifier_sequence_opt;
        p_attribute_specifier_sequence_opt = NULL; /*MOVED*/


        p_simple_declaration->p_declaration_specifiers = declaration_specifiers(ctx, storage_specifier_flags);

        if (p_simple_declaration->p_attribute_specifier_sequence_opt)
        {
            p_simple_declaration->p_declaration_specifiers->attributes_flags =
                p_simple_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
        }

        p_simple_declaration->init_declarator_list = init_declarator_list(ctx, p_simple_declaration->p_declaration_specifiers);

        p_simple_declaration->last_token = previous_parser_token(ctx->current);

        if (!ignore_semicolon && parser_match_tk(ctx, ';') != 0) throw;
    }
    catch
    {
        simple_declaration_delete(p_simple_declaration);
        p_simple_declaration = NULL;
    }

    attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);

    return p_simple_declaration;
}

struct declaration* owner declaration(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt,
    enum storage_class_specifier_flags storage_specifier_flags)
{
    bool is_function_definition = false;
    return declaration_core(ctx, p_attribute_specifier_sequence_opt, false, &is_function_definition, storage_specifier_flags, false);
}

//(6.7) declaration-specifiers:
// declaration-specifier attribute-specifier-sequenceopt
// declaration-specifier declaration-specifiers

void declaration_specifier_delete(struct declaration_specifier* owner opt p)
{
    if (p)
    {
        free(p->function_specifier);
        type_specifier_qualifier_delete(p->type_specifier_qualifier);
        free(p->storage_class_specifier);
        assert(p->next == NULL);
        free(p);
    }
}

struct declaration_specifier* owner declaration_specifier(struct parser_ctx* ctx)
{
    //    storage-class-specifier
    //    type-specifier-qualifier
    //    function-specifier
    struct declaration_specifier* owner p_declaration_specifier = calloc(1, sizeof * p_declaration_specifier);
    if (first_of_storage_class_specifier(ctx))
    {
        p_declaration_specifier->storage_class_specifier = storage_class_specifier(ctx);
    }
    else if (first_of_type_specifier_qualifier(ctx))
    {
        p_declaration_specifier->type_specifier_qualifier = type_specifier_qualifier(ctx);
    }
    else if (first_of_function_specifier(ctx))
    {
        p_declaration_specifier->function_specifier = function_specifier(ctx);
    }
    else
    {
        compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, "unexpected");
    }
    return p_declaration_specifier;
}

void init_declarator_delete(struct init_declarator* owner opt p)
{
    if (p)
    {
        initializer_delete(p->initializer);
        declarator_delete(p->p_declarator);
        assert(p->next == NULL);
        free(p);
    }
}
struct init_declarator* owner init_declarator(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers)
{
    /*
     init-declarator:
       declarator
       declarator = initializer
    */
    struct init_declarator* owner p_init_declarator = calloc(1, sizeof(struct init_declarator));
    try
    {
        struct token* tkname = 0;
        p_init_declarator->p_declarator = declarator(ctx,
            NULL,
            p_declaration_specifiers,
            false,
            &tkname);

        if (p_init_declarator->p_declarator == NULL)
            throw;
        p_init_declarator->p_declarator->name = tkname;

        if (tkname == NULL)
        {
            compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, "empty declarator name?? unexpected");
            return p_init_declarator;
        }

        p_init_declarator->p_declarator->declaration_specifiers = p_declaration_specifiers;
        p_init_declarator->p_declarator->name = tkname;

        if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
        {
            /*
              auto requires we find the type after initializer
            */
        }
        else
        {
#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"
            assert(p_init_declarator->p_declarator->type.type_specifier_flags == 0);
            p_init_declarator->p_declarator->type = make_type_using_declarator(ctx, p_init_declarator->p_declarator);
#pragma cake diagnostic pop
        }

        const char* name = p_init_declarator->p_declarator->name->lexeme;
        if (name)
        {
            /*
              Checking naming conventions
            */
            if (ctx->scopes.tail->scope_level == 0)
            {
                if (type_is_function(&p_init_declarator->p_declarator->type))
                {
                    naming_convention_global_var(ctx,
                        tkname,
                        &p_init_declarator->p_declarator->type,
                        p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags);
                }
                else
                {
                    naming_convention_global_var(ctx,
                        tkname,
                        &p_init_declarator->p_declarator->type,
                        p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags);
                }
            }

            struct scope* out_scope = NULL;
            struct declarator* previous = find_declarator(ctx, name, &out_scope);
            if (previous)
            {
                if (out_scope->scope_level == ctx->scopes.tail->scope_level)
                {
                    if (out_scope->scope_level == 0)
                    {
                        /*file scope*/
                        if (!type_is_same(&previous->type, &p_init_declarator->p_declarator->type, true))
                        {
                            // TODO failing on windows headers
                            // parser_seterror_with_token(ctx, p_init_declarator->declarator->name, "redeclaration of  '%s' with diferent types", previous->name->lexeme);
                            // parser_set_info_with_token(ctx, previous->name, "previous declaration");
                        }
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_REDECLARATION, ctx, ctx->current, "redeclaration");
                        compiler_diagnostic_message(W_NOTE, ctx, previous->name, "previous declaration");
                    }
                }
                else
                {
                    hashmap_set(&ctx->scopes.tail->variables, name, p_init_declarator, TAG_TYPE_INIT_DECLARATOR);

                    /*global scope no warning...*/
                    if (out_scope->scope_level != 0)
                    {
                        /*but redeclaration at function scope we show warning*/
                        if (compiler_diagnostic_message(W_DECLARATOR_HIDE, ctx, p_init_declarator->p_declarator->first_token, "declaration of '%s' hides previous declaration", name))
                        {
                            compiler_diagnostic_message(W_NOTE, ctx, previous->first_token, "previous declaration is here");
                        }
                    }
                }
            }
            else
            {
                /*first time we see this declarator*/
                hashmap_set(&ctx->scopes.tail->variables, name, p_init_declarator, TAG_TYPE_INIT_DECLARATOR);
            }
        }
        else
        {
            assert(false);
        }

        if (ctx->current && ctx->current->type == '=')
        {
            parser_match(ctx);
            p_init_declarator->initializer = initializer(ctx);
            if (p_init_declarator->initializer->braced_initializer)
            {
                if (type_is_array(&p_init_declarator->p_declarator->type))
                {
                    const int sz = p_init_declarator->p_declarator->type.num_of_elements;
                    if (sz == 0)
                    {
                        /*int a[] = {1, 2, 3}*/
                        const int braced_initializer_size =
                            p_init_declarator->initializer->braced_initializer->initializer_list->size;
                        p_init_declarator->p_declarator->type.num_of_elements = braced_initializer_size;
                    }
                }

                /*
                  Fixing the type of auto declarator
                  ??
                */
            }
            else if (p_init_declarator->initializer->assignment_expression)
            {

                if (p_init_declarator->initializer->assignment_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL)
                {
                    /*char a[] = "ab"*/
                    if (type_is_array(&p_init_declarator->p_declarator->type))
                    {
                        const int array_size_elements = p_init_declarator->p_declarator->type.num_of_elements;
                        if (array_size_elements == 0)
                        {
                            p_init_declarator->p_declarator->type.num_of_elements =
                                p_init_declarator->initializer->assignment_expression->type.num_of_elements;
                        }
                    }
                }

                /*
                  Fixing the type of auto declarator
                */
                if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
                {
                    struct type t = { 0 };

                    if (p_init_declarator->initializer->assignment_expression->expression_type == UNARY_EXPRESSION_ADDRESSOF)
                    {
                        t = type_dup(&p_init_declarator->initializer->assignment_expression->type);
                    }
                    else
                    {
                        struct type t2 = type_lvalue_conversion(&p_init_declarator->initializer->assignment_expression->type, ctx->options.null_checks_enabled);
                        type_swap(&t2, &t);
                        type_destroy(&t2);
                    }

                    type_remove_names(&t);
                    assert(t.name_opt == NULL);
                    t.name_opt = strdup(p_init_declarator->p_declarator->name->lexeme);

                    type_set_qualifiers_using_declarator(&t, p_init_declarator->p_declarator);

                    type_visit_to_mark_anonymous(&t);
                    type_swap(&p_init_declarator->p_declarator->type, &t);
                    type_destroy(&t);
                }

                check_assigment(ctx, &p_init_declarator->p_declarator->type, p_init_declarator->initializer->assignment_expression, ASSIGMENT_TYPE_OBJECTS);
            }
        }
    }
    catch
    {
    }

    /*
       checking usage of [static ] other than in function arguments
    */
    if (p_init_declarator->p_declarator)
    {
        if (type_is_array(&p_init_declarator->p_declarator->type))
            if (p_init_declarator->p_declarator->type.type_qualifier_flags != 0 ||
                p_init_declarator->p_declarator->type.static_array)
            {
                compiler_diagnostic_message(C_ERROR_STATIC_OR_TYPE_QUALIFIERS_NOT_ALLOWED_IN_NON_PARAMETER,
                    ctx,
                    p_init_declarator->p_declarator->first_token,
                    "static or type qualifiers are not allowed in non-parameter array declarator");
            }

        if (!type_is_pointer(&p_init_declarator->p_declarator->type) &&
            p_init_declarator->p_declarator->type.type_qualifier_flags & TYPE_QUALIFIER_OBJ_OWNER)
        {
            compiler_diagnostic_message(C_ERROR_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER,
                ctx,
                p_init_declarator->p_declarator->first_token,
                "obj_owner qualifier can only be used with pointers");
        }
    }

    if (
        !(p_init_declarator->p_declarator->type.storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF) &&
        !type_is_function(&p_init_declarator->p_declarator->type))
    {
        int sz = type_get_sizeof(&p_init_declarator->p_declarator->type);

        if (sz == -3)
        {
            /*type_get_sizeof returns -3 for VLAs*/
        }
        else if (sz < 0)
        {
            // clang warning: array 'c' assumed to have one element
            // gcc "error: storage size of '%s' isn't known"
            compiler_diagnostic_message(C_ERROR_STORAGE_SIZE,
                ctx,
                p_init_declarator->p_declarator->name,
                "storage size of '%s' isn't known",
                p_init_declarator->p_declarator->name->lexeme);
        }
        else
        {
            // ok
        }
    }

    return p_init_declarator;
}

void init_declarator_list_add(struct init_declarator_list* list, struct init_declarator* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void init_declarator_list_destroy(struct init_declarator_list* obj_owner p)
{
    struct init_declarator* owner item = p->head;
    while (item)
    {
        struct init_declarator* owner next = item->next;
        item->next = NULL;
        init_declarator_delete(item);
        item = next;
    }
}

struct init_declarator_list init_declarator_list(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers)
{
    /*
    init-declarator-list:
      init-declarator
      init-declarator-list , init-declarator
    */
    struct init_declarator_list init_declarator_list = { 0 };
    struct init_declarator* owner p_init_declarator = NULL;

    try
    {
        p_init_declarator = init_declarator(ctx, p_declaration_specifiers);

        if (p_init_declarator == NULL)
            throw;
        init_declarator_list_add(&init_declarator_list, p_init_declarator);
        p_init_declarator = NULL; /*MOVED*/

        while (ctx->current != NULL && ctx->current->type == ',')
        {
            parser_match(ctx);
            p_init_declarator = init_declarator(ctx, p_declaration_specifiers);
            if (p_init_declarator == NULL)
                throw;
            init_declarator_list_add(&init_declarator_list, p_init_declarator);
            p_init_declarator = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return init_declarator_list;
}

void storage_class_specifier_delete(struct storage_class_specifier* owner opt p)
{
    if (p)
    {
        free(p);
    }
}

struct storage_class_specifier* owner storage_class_specifier(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return NULL;

    struct storage_class_specifier* owner new_storage_class_specifier = calloc(1, sizeof(struct storage_class_specifier));
    if (new_storage_class_specifier == NULL)
        return NULL;

    new_storage_class_specifier->token = ctx->current;
    switch (ctx->current->type)
    {
    case TK_KEYWORD_TYPEDEF:
        new_storage_class_specifier->flags = STORAGE_SPECIFIER_TYPEDEF;
        break;
    case TK_KEYWORD_EXTERN:
        new_storage_class_specifier->flags = STORAGE_SPECIFIER_EXTERN;
        break;
    case TK_KEYWORD_CONSTEXPR:

        new_storage_class_specifier->flags = STORAGE_SPECIFIER_CONSTEXPR;
        if (ctx->scopes.tail->scope_level == 0)
            new_storage_class_specifier->flags |= STORAGE_SPECIFIER_CONSTEXPR_STATIC;
        break;
    case TK_KEYWORD_STATIC:
        new_storage_class_specifier->flags = STORAGE_SPECIFIER_STATIC;
        break;
    case TK_KEYWORD__THREAD_LOCAL:
        new_storage_class_specifier->flags = STORAGE_SPECIFIER_THREAD_LOCAL;
        break;
    case TK_KEYWORD_AUTO:
        new_storage_class_specifier->flags = STORAGE_SPECIFIER_AUTO;
        break;
    case TK_KEYWORD_REGISTER:
        new_storage_class_specifier->flags = STORAGE_SPECIFIER_REGISTER;
        break;
    default:
        assert(false);
    }

    /*
     TODO
     thread_local may appear with static or extern,
     auto may appear with all the others except typedef138), and
     constexpr may appear with auto, register, or static.
    */

    parser_match(ctx);
    return new_storage_class_specifier;
}

struct typeof_specifier_argument* owner typeof_specifier_argument(struct parser_ctx* ctx)
{
    struct typeof_specifier_argument* owner new_typeof_specifier_argument = calloc(1, sizeof(struct typeof_specifier_argument));
    if (new_typeof_specifier_argument == NULL)
        return NULL;
    try
    {
        if (first_of_type_name(ctx))
        {
            new_typeof_specifier_argument->type_name = type_name(ctx);
        }
        else
        {
            const bool disable_evaluation_copy = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = true;
            new_typeof_specifier_argument->expression = expression(ctx);
            /*restore*/
            ctx->evaluation_is_disabled = disable_evaluation_copy;

            if (new_typeof_specifier_argument->expression == NULL)
                throw;

            // declarator_type_clear_name(new_typeof_specifier_argument->expression->type.declarator_type);
        }
    }
    catch
    {
    }
    return new_typeof_specifier_argument;
}

bool first_of_typeof_specifier(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD_TYPEOF ||
        ctx->current->type == TK_KEYWORD_TYPEOF_UNQUAL;
}

struct typeof_specifier* owner typeof_specifier(struct parser_ctx* ctx)
{
    struct typeof_specifier* owner p_typeof_specifier = NULL;
    try
    {
        p_typeof_specifier = calloc(1, sizeof(struct typeof_specifier));
        if (p_typeof_specifier == NULL)
            throw;

        p_typeof_specifier->first_token = ctx->current;

        const bool is_typeof_unqual = ctx->current->type == TK_KEYWORD_TYPEOF_UNQUAL;
        parser_match(ctx);
        if (parser_match_tk(ctx, '(') != 0)
            throw;

        p_typeof_specifier->typeof_specifier_argument = typeof_specifier_argument(ctx);
        if (p_typeof_specifier->typeof_specifier_argument == NULL)
            throw;

        if (p_typeof_specifier->typeof_specifier_argument->expression)
        {
            p_typeof_specifier->type = type_dup(&p_typeof_specifier->typeof_specifier_argument->expression->type);
        }
        else if (p_typeof_specifier->typeof_specifier_argument->type_name)
        {
            p_typeof_specifier->type = type_dup(&p_typeof_specifier->typeof_specifier_argument->type_name->declarator->type);
        }

        if (p_typeof_specifier->type.storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            compiler_diagnostic_message(W_TYPEOF_ARRAY_PARAMETER, ctx, ctx->current, "typeof used in array arguments");

            if (type_is_array(&p_typeof_specifier->type))
            {
                struct type t = type_param_array_to_pointer(&p_typeof_specifier->type, ctx->options.null_checks_enabled);
                type_swap(&t, &p_typeof_specifier->type);
                type_destroy(&t);
            }
        }

        if (is_typeof_unqual)
        {
            type_remove_qualifiers(&p_typeof_specifier->type);
        }

        type_visit_to_mark_anonymous(&p_typeof_specifier->type);

        free((void* owner)p_typeof_specifier->type.name_opt);
        p_typeof_specifier->type.name_opt = NULL;

        p_typeof_specifier->last_token = ctx->current;
        if (parser_match_tk(ctx, ')') != 0) throw;
    }
    catch
    {
        typeof_specifier_delete(p_typeof_specifier);
        p_typeof_specifier = NULL;
    }

    return p_typeof_specifier;
}

void typeof_specifier_argument_delete(struct typeof_specifier_argument* owner opt p)
{
    if (p)
    {
        expression_delete(p->expression);
        type_name_delete(p->type_name);
        free(p);
    }
}

void typeof_specifier_delete(struct typeof_specifier* owner opt p)
{
    if (p)
    {
        typeof_specifier_argument_delete(p->typeof_specifier_argument);
        type_destroy(&p->type);
        free(p);
    }
}
void type_specifier_delete(struct type_specifier* owner opt p)
{
    if (p)
    {
        struct_or_union_specifier_delete(p->struct_or_union_specifier);
        typeof_specifier_delete(p->typeof_specifier);
        enum_specifier_delete(p->enum_specifier);
        atomic_type_specifier_delete(p->atomic_type_specifier);
        free(p);
    }
}
struct type_specifier* owner type_specifier(struct parser_ctx* ctx)
{
    /*
     type-specifier:
       void
       char
       short
       int
       long
       float
       double
       signed
       unsigned
       _BitInt ( constant-expression )
       bool                                  C23
       _Complex
       _Decimal32
       _Decimal64
       _Decimal128
       atomic-type-specifier
       struct-or-union-specifier
       enum-specifier
       typedef-name
       typeof-specifier                      C23
    */

    struct type_specifier* owner p_type_specifier = calloc(1, sizeof * p_type_specifier);
    if (p_type_specifier == NULL)
        return NULL;

    switch (ctx->current->type)
    {
    case TK_KEYWORD_VOID:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_VOID;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD_CHAR:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_CHAR;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD_SHORT:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_SHORT;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD_INT:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_INT;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

        // microsoft
    case TK_KEYWORD__INT8:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_INT8;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD__INT16:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_INT16;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;
    case TK_KEYWORD__INT32:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_INT32;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;
    case TK_KEYWORD__INT64:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_INT64;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;
        // end microsoft

    case TK_KEYWORD_LONG:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_LONG;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD_FLOAT:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_FLOAT;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD_DOUBLE:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_DOUBLE;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD_SIGNED:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_SIGNED;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD_UNSIGNED:

        p_type_specifier->flags = TYPE_SPECIFIER_UNSIGNED;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD__BOOL:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_BOOL;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD__COMPLEX:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_COMPLEX;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD__DECIMAL32:
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_DECIMAL32;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD__DECIMAL64:

        p_type_specifier->flags = TYPE_SPECIFIER_DECIMAL64;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    case TK_KEYWORD__DECIMAL128:
        p_type_specifier->flags = TYPE_SPECIFIER_DECIMAL128;
        p_type_specifier->token = ctx->current;
        parser_match(ctx);
        return p_type_specifier;

    default:
        // Do nothing
        break;
    }

    if (first_of_typeof_specifier(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_TYPEOF;
        p_type_specifier->typeof_specifier = typeof_specifier(ctx);
    }
    else if (first_of_atomic_type_specifier(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_ATOMIC;
        p_type_specifier->atomic_type_specifier = atomic_type_specifier(ctx);
    }
    else if (first_of_struct_or_union(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_STRUCT_OR_UNION;
        p_type_specifier->struct_or_union_specifier = struct_or_union_specifier(ctx);
    }
    else if (first_of_enum_specifier(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_ENUM;
        p_type_specifier->enum_specifier = enum_specifier(ctx);
    }
    else if (ctx->current->type == TK_IDENTIFIER)
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_TYPEDEF;

        p_type_specifier->typedef_declarator =
            find_declarator(ctx, ctx->current->lexeme, NULL);

        // Ser chegou aqui já tem que exitir (reaprovecitar?)
        assert(p_type_specifier->typedef_declarator != NULL);

        parser_match(ctx);
    }

    return p_type_specifier;
}

const struct enum_specifier* get_complete_enum_specifier(const struct enum_specifier* p_enum_specifier)
{
    /*
      The way cake find the complete struct is using one pass.. for this task is uses double indirection.
      Then the result will be there at end of first pass.
      This crazy code finds the complete definition of the struct if exists.
    */
    if (p_enum_specifier == NULL)
        return NULL;

    if (p_enum_specifier->enumerator_list.head)
    {
        /*p_struct_or_union_specifier is complete*/
        return p_enum_specifier;
    }
    else if (p_enum_specifier->complete_enum_specifier2 &&
        p_enum_specifier->complete_enum_specifier2->enumerator_list.head)
    {
        /*p_struct_or_union_specifier is the first seem tag tag points directly to complete*/
        return p_enum_specifier->complete_enum_specifier2;
    }
    else if (p_enum_specifier->complete_enum_specifier2 &&
        p_enum_specifier->complete_enum_specifier2->complete_enum_specifier2 &&
        p_enum_specifier->complete_enum_specifier2->complete_enum_specifier2->enumerator_list.head)
    {
        /* all others points to the first seem that points to the complete*/
        return p_enum_specifier->complete_enum_specifier2->complete_enum_specifier2;
    }

    return NULL;
}

struct struct_or_union_specifier* get_complete_struct_or_union_specifier(struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    /*
      The way cake find the complete struct is using one pass.. for this task is uses double indirection.
      Then the result will be there at end of first pass.
      This crazy code finds the complete definition of the struct if exists.
    */
    if (p_struct_or_union_specifier == NULL)
        return NULL;

    if (p_struct_or_union_specifier->member_declaration_list.head)
    {
        /*p_struct_or_union_specifier is complete*/
        return p_struct_or_union_specifier;
    }
    else if (p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection &&
        p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection->member_declaration_list.head)
    {
        /*p_struct_or_union_specifier is the first seem tag tag points directly to complete*/
        return p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection;
    }
    else if (p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection &&
        p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection->complete_struct_or_union_specifier_indirection &&
        p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection->complete_struct_or_union_specifier_indirection->member_declaration_list.head)
    {
        /* all others points to the first seem that points to the complete*/
        return p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection->complete_struct_or_union_specifier_indirection;
    }

    return NULL;
}

bool struct_or_union_specifier_is_complete(struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    return get_complete_struct_or_union_specifier(p_struct_or_union_specifier) != NULL;
}

void struct_or_union_specifier_delete(struct struct_or_union_specifier* owner opt p)
{
    if (p)
    {
        member_declaration_list_destroy(&p->member_declaration_list);
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        free(p);
    }
}

struct struct_or_union_specifier* owner struct_or_union_specifier(struct parser_ctx* ctx)
{
    struct struct_or_union_specifier* owner p_struct_or_union_specifier = calloc(1, sizeof * p_struct_or_union_specifier);
    try
    {
        if (p_struct_or_union_specifier == NULL)
            throw;

        static_set(*p_struct_or_union_specifier, "zero");

        if (ctx->current->type == TK_KEYWORD_STRUCT ||
            ctx->current->type == TK_KEYWORD_UNION)
        {
            p_struct_or_union_specifier->first_token = ctx->current;
            parser_match(ctx);
        }
        else
        {
            assert(false);
        }

        p_struct_or_union_specifier->attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);

        struct struct_or_union_specifier* p_first_tag_in_this_scope = NULL;

        if (ctx->current->type == TK_IDENTIFIER)
        {
            p_struct_or_union_specifier->tagtoken = ctx->current;
            /*
             Structure, union, and enumeration tags have scope that begins just after the
             appearance of the tag in a type specifier that declares the tag.
            */

            snprintf(p_struct_or_union_specifier->tag_name, sizeof p_struct_or_union_specifier->tag_name, "%s", ctx->current->lexeme);

            struct map_entry* p_entry = hashmap_find(&ctx->scopes.tail->tags, ctx->current->lexeme);
            if (p_entry)
            {
                /*this tag already exist in this scope*/
                if (p_entry->type == TAG_TYPE_STRUCT_OR_UNION_SPECIFIER)
                {
                    p_first_tag_in_this_scope = p_entry->p;
                    p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection = p_first_tag_in_this_scope;
                }
                else
                {
                    compiler_diagnostic_message(C_ERROR_TAG_TYPE_DOES_NOT_MATCH_PREVIOUS_DECLARATION,
                        ctx,
                        ctx->current,
                        "use of '%s' with tag type that does not match previous declaration.",
                        ctx->current->lexeme);
                }
            }
            else
            {
                /*tag does not exist in the current scope, let search on upper scopes*/
                struct struct_or_union_specifier* p_first_tag_previous_scopes = find_struct_or_union_specifier(ctx, ctx->current->lexeme);
                if (p_first_tag_previous_scopes == NULL)
                {
                    /*tag not found, so it is the first appearence*/
                    p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
                    hashmap_set(&ctx->scopes.tail->tags, ctx->current->lexeme, p_struct_or_union_specifier, TAG_TYPE_STRUCT_OR_UNION_SPECIFIER);
                }
                else
                {
                    /*tag already exists in some scope*/
                    p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection = p_first_tag_previous_scopes;
                }
            }

            parser_match(ctx);
        }
        else
        {
            /*struct without a tag, in this case we make one*/
            snprintf(p_struct_or_union_specifier->tag_name, sizeof p_struct_or_union_specifier->tag_name, "_anonymous_struct_%d", s_anonymous_struct_count);
            s_anonymous_struct_count++;
            p_struct_or_union_specifier->has_anonymous_tag = true;
            p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
            hashmap_set(&ctx->scopes.tail->tags, p_struct_or_union_specifier->tag_name, p_struct_or_union_specifier, TAG_TYPE_STRUCT_OR_UNION_SPECIFIER);
        }

        if (ctx->current->type == '{')
        {
            /*
            this is the complete struct
            */

            struct struct_or_union_specifier* first = find_struct_or_union_specifier(ctx, p_struct_or_union_specifier->tag_name);

            if (first)
            {
                /*
                   The first tag (will the one at symbol table) will point to the complete struct
                */
                first->complete_struct_or_union_specifier_indirection = p_struct_or_union_specifier;
            }

            if (p_struct_or_union_specifier->tagtoken)
                naming_convention_struct_tag(ctx, p_struct_or_union_specifier->tagtoken);

            struct token* firsttoken = ctx->current;
            parser_match(ctx);
            if (ctx->current->type != '}') /*not official extensions yet..missing sizeof etc*/
            {
#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"
                p_struct_or_union_specifier->member_declaration_list = member_declaration_list(ctx, p_struct_or_union_specifier);
#pragma cake diagnostic pop

            }
            p_struct_or_union_specifier->member_declaration_list.first_token = firsttoken;
            p_struct_or_union_specifier->last_token = ctx->current;
            p_struct_or_union_specifier->member_declaration_list.last_token = ctx->current;
            if (parser_match_tk(ctx, '}') != 0)
                throw;
        }
        else
        {
            p_struct_or_union_specifier->last_token = ctx->current;
        }

        struct struct_or_union_specifier* p_complete =
            get_complete_struct_or_union_specifier(p_struct_or_union_specifier);

        /*check if complete struct is deprecated*/
        if (p_complete)
        {
            if (p_complete->attribute_specifier_sequence_opt &&
                p_complete->attribute_specifier_sequence_opt->attributes_flags & STD_ATTRIBUTE_DEPRECATED)
            {
                if (p_struct_or_union_specifier->tagtoken)
                {
                    // TODO add deprecated message
                    compiler_diagnostic_message(W_DEPRECATED, ctx, p_struct_or_union_specifier->first_token, "'%s' is deprecated", p_struct_or_union_specifier->tagtoken->lexeme);
                }
                else
                {
                    compiler_diagnostic_message(W_DEPRECATED, ctx, p_struct_or_union_specifier->first_token, "deprecated");
                }
            }
        }
    }
    catch
    {
        struct_or_union_specifier_delete(p_struct_or_union_specifier);
        p_struct_or_union_specifier = NULL;
    }
    return p_struct_or_union_specifier;
}

struct member_declarator* owner member_declarator(
    struct parser_ctx* ctx,
    struct struct_or_union_specifier* p_struct_or_union_specifier, /*not const*/
    const struct specifier_qualifier_list* p_specifier_qualifier_list)
{
    /*
    member-declarator:
     declarator
     declaratoropt : constant-expression
    */
    struct member_declarator* owner p_member_declarator = calloc(1, sizeof(struct member_declarator));

    struct token* p_token_name = NULL;

    p_member_declarator->declarator = declarator(ctx, p_specifier_qualifier_list, /*declaration_specifiers*/ NULL, false, &p_token_name);
    p_member_declarator->declarator->name = p_token_name;
    p_member_declarator->declarator->specifier_qualifier_list = p_specifier_qualifier_list;

#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"    
    p_member_declarator->declarator->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
#pragma cake diagnostic pop

    /*extension*/
    if (type_is_owner(&p_member_declarator->declarator->type))
    {
        /*having at least 1 owner member, the struct type is owner by default*/
        p_struct_or_union_specifier->is_owner = true;
    }

    if (p_member_declarator->declarator->name)
        naming_convention_struct_member(ctx, p_member_declarator->declarator->name, &p_member_declarator->declarator->type);

    if (ctx->current->type == ':')
    {
        parser_match(ctx);
        p_member_declarator->constant_expression = constant_expression(ctx, true);
    }
    return p_member_declarator;
}

void member_declarator_delete(struct member_declarator* owner opt p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        assert(p->next == NULL);
        declarator_delete(p->declarator);
        free(p);
    }
}

void member_declarator_list_add(struct member_declarator_list* list, struct member_declarator* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void member_declarator_list_delete(struct member_declarator_list* owner opt p)
{
    if (p)
    {
        struct member_declarator* owner item = p->head;
        while (item)
        {
            struct member_declarator* owner next = item->next;
            item->next = NULL;
            member_declarator_delete(item);
            item = next;
        }
        free(p);
    }
}
struct member_declarator_list* owner member_declarator_list(
    struct parser_ctx* ctx,
    struct struct_or_union_specifier* p_struct_or_union_specifier,
    const struct specifier_qualifier_list* p_specifier_qualifier_list)
{
    struct member_declarator_list* owner p_member_declarator_list = calloc(1, sizeof(struct member_declarator_list));
    member_declarator_list_add(p_member_declarator_list, member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list));
    while (ctx->current->type == ',')
    {
        parser_match(ctx);
        member_declarator_list_add(p_member_declarator_list, member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list));
    }
    return p_member_declarator_list;
}

void member_declaration_list_add(struct member_declaration_list* list, struct member_declaration* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}
void member_declaration_list_destroy(struct member_declaration_list* obj_owner p)
{
    struct member_declaration* owner item = p->head;
    while (item)
    {
        struct member_declaration* owner next = item->next;
        item->next = NULL;
        member_declaration_delete(item);
        item = next;
    }
}

struct member_declaration_list member_declaration_list(struct parser_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    struct member_declaration_list list = { 0 };
    // member_declaration
    // member_declaration_list member_declaration

    struct member_declaration* owner p_member_declaration = NULL;

    try
    {
        p_member_declaration = member_declaration(ctx, p_struct_or_union_specifier);

        if (p_member_declaration == NULL)
            throw;
        member_declaration_list_add(&list, p_member_declaration);
        p_member_declaration = NULL; /*MOVED*/

        while (ctx->current && ctx->current->type != '}')
        {
            p_member_declaration = member_declaration(ctx, p_struct_or_union_specifier);
            if (p_member_declaration == NULL)
                throw;
            member_declaration_list_add(&list, p_member_declaration);
            p_member_declaration = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return list;
}

void member_declaration_delete(struct member_declaration* owner opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        specifier_qualifier_list_delete(p->specifier_qualifier_list);
        member_declarator_list_delete(p->member_declarator_list_opt);
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        static_assert_declaration_delete(p->static_assert_declaration);
        pragma_declaration_delete(p->pragma_declaration);
        free(p);
    }
}
struct member_declaration* owner member_declaration(struct parser_ctx* ctx,
    struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    struct member_declaration* owner p_member_declaration = calloc(1, sizeof(struct member_declaration));
    try
    {
        if (p_member_declaration == NULL)
            throw;
        // attribute_specifier_sequence_opt specifier_qualifier_list member_declarator_list_opt ';'
        // static_assert_declaration
        if (ctx->current->type == TK_KEYWORD__STATIC_ASSERT)
        {
            p_member_declaration->static_assert_declaration = static_assert_declaration(ctx);
        }
        else if (ctx->current->type == TK_PRAGMA)
        {
            p_member_declaration->pragma_declaration = pragma_declaration(ctx);
        }
        else
        {
            p_member_declaration->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);

            p_member_declaration->specifier_qualifier_list = specifier_qualifier_list(ctx);
            if (ctx->current->type != ';')
            {
                p_member_declaration->member_declarator_list_opt = member_declarator_list(ctx,
                    p_struct_or_union_specifier,
                    p_member_declaration->specifier_qualifier_list);
            }
            if (parser_match_tk(ctx, ';') != 0)
                throw;
        }
    }
    catch
    {
        member_declaration_delete(p_member_declaration);
        p_member_declaration = NULL;
    }
    return p_member_declaration;
}

struct member_declarator* find_member_declarator(struct member_declaration_list* list, const char* name, int* p_member_index)
{
    struct member_declaration* p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator* p_member_declarator = NULL;

        if (p_member_declaration->member_declarator_list_opt)
        {
            p_member_declarator = p_member_declaration->member_declarator_list_opt->head;

            while (p_member_declarator)
            {
                if (p_member_declarator->declarator->name && strcmp(p_member_declarator->declarator->name->lexeme, name) == 0)
                {
                    return p_member_declarator;
                }

                (*p_member_index)++;
                p_member_declarator = p_member_declarator->next;
            }
        }
        else
        {
            if (p_member_declaration->specifier_qualifier_list &&
                p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct member_declaration_list* p_member_declaration_list =
                    &p_member_declaration->specifier_qualifier_list->struct_or_union_specifier->member_declaration_list;

                p_member_declarator = find_member_declarator(p_member_declaration_list, name, p_member_index);
                if (p_member_declarator)
                    return p_member_declarator;
            }
        }

        p_member_declaration = p_member_declaration->next;
    }
    return NULL;
}

void print_specifier_qualifier_list(struct osstream* ss, bool* first, struct specifier_qualifier_list* p_specifier_qualifier_list)
{

    print_type_qualifier_flags(ss, first, p_specifier_qualifier_list->type_qualifier_flags);

    if (p_specifier_qualifier_list->enum_specifier)
    {

        // TODO
        assert(false);
    }
    else if (p_specifier_qualifier_list->struct_or_union_specifier)
    {
        ss_fprintf(ss, "struct %s", p_specifier_qualifier_list->struct_or_union_specifier->tag_name);
    }
    else if (p_specifier_qualifier_list->typedef_declarator)
    {
        print_item(ss, first, p_specifier_qualifier_list->typedef_declarator->name->lexeme);
    }
    else
    {
        print_type_specifier_flags(ss, first, p_specifier_qualifier_list->type_specifier_flags);
    }
}


void specifier_qualifier_list_add(struct specifier_qualifier_list* list, struct type_specifier_qualifier* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void specifier_qualifier_list_delete(struct specifier_qualifier_list* owner opt p)
{
    if (p)
    {
        struct type_specifier_qualifier* owner item = p->head;
        while (item)
        {
            struct type_specifier_qualifier* owner next = item->next;
            item->next = NULL;
            type_specifier_qualifier_delete(item);
            item = next;
        }

        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence);

        free(p);
    }
}

struct specifier_qualifier_list* owner specifier_qualifier_list(struct parser_ctx* ctx)
{
    struct specifier_qualifier_list* owner p_specifier_qualifier_list = calloc(1, sizeof(struct specifier_qualifier_list));
    if (p_specifier_qualifier_list == NULL)
        return NULL;

    static_set(*p_specifier_qualifier_list, "zero");

    /*
      type_specifier_qualifier attribute_specifier_sequence_opt
      type_specifier_qualifier specifier_qualifier_list
    */
    try
    {
        p_specifier_qualifier_list->first_token = ctx->current;

        while (ctx->current != NULL &&
            (first_of_type_specifier(ctx) ||
                first_of_type_qualifier(ctx)))
        {

            if (ctx->current->flags & TK_FLAG_IDENTIFIER_IS_TYPEDEF)
            {
                if (p_specifier_qualifier_list->type_specifier_flags != TYPE_SPECIFIER_NONE)
                {
                    // typedef tem que aparecer sozinho
                    // exemplo Socket eh nome e nao typdef
                    // typedef int Socket;
                    // struct X {int Socket;};
                    break;
                }
            }

            struct type_specifier_qualifier* owner p_type_specifier_qualifier = type_specifier_qualifier(ctx);

            if (p_type_specifier_qualifier->type_specifier)
            {
                if (add_specifier(ctx,
                    &p_specifier_qualifier_list->type_specifier_flags,
                    p_type_specifier_qualifier->type_specifier->flags) != 0)
                {
                    type_specifier_qualifier_delete(p_type_specifier_qualifier);
                    throw;
                }

                if (p_type_specifier_qualifier->type_specifier->struct_or_union_specifier)
                {
                    p_specifier_qualifier_list->struct_or_union_specifier = p_type_specifier_qualifier->type_specifier->struct_or_union_specifier;
                }
                else if (p_type_specifier_qualifier->type_specifier->enum_specifier)
                {
                    p_specifier_qualifier_list->enum_specifier = p_type_specifier_qualifier->type_specifier->enum_specifier;
                }
                else if (p_type_specifier_qualifier->type_specifier->typeof_specifier)
                {
                    p_specifier_qualifier_list->typeof_specifier = p_type_specifier_qualifier->type_specifier->typeof_specifier;
                }
                else if (p_type_specifier_qualifier->type_specifier->token->type == TK_IDENTIFIER)
                {
                    p_specifier_qualifier_list->typedef_declarator =
                        find_declarator(ctx,
                            p_type_specifier_qualifier->type_specifier->token->lexeme,
                            NULL);
                }
            }
            else if (p_type_specifier_qualifier->type_qualifier)
            {
                p_specifier_qualifier_list->type_qualifier_flags |= p_type_specifier_qualifier->type_qualifier->flags;
            }

            p_specifier_qualifier_list->p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);

            specifier_qualifier_list_add(p_specifier_qualifier_list, p_type_specifier_qualifier);
        }
    }
    catch
    {
    }

    final_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags);
    p_specifier_qualifier_list->last_token = previous_parser_token(ctx->current);
    return p_specifier_qualifier_list;
}

void type_qualifier_delete(struct type_qualifier* owner opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        free(p);
    }
}

void type_specifier_qualifier_delete(struct type_specifier_qualifier* owner opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        if (p->type_qualifier)
        {
            assert(p->type_qualifier->next == NULL);
            free(p->type_qualifier);
        }
        alignment_specifier_delete(p->alignment_specifier);

        type_specifier_delete(p->type_specifier);

        //ATENTION LIST?? SEE ABOVE TODO
        //type_qualifier_delete(p->type_qualifier);

        free(p);
    }
}
struct type_specifier_qualifier* owner type_specifier_qualifier(struct parser_ctx* ctx)
{
    struct type_specifier_qualifier* owner type_specifier_qualifier = calloc(1, sizeof * type_specifier_qualifier);
    // type_specifier
    // type_qualifier
    // alignment_specifier
    if (first_of_type_specifier(ctx))
    {
        type_specifier_qualifier->type_specifier = type_specifier(ctx);
    }
    else if (first_of_type_qualifier(ctx))
    {
        type_specifier_qualifier->type_qualifier = type_qualifier(ctx);
    }
    else if (first_of_alignment_specifier(ctx))
    {
        type_specifier_qualifier->alignment_specifier = alignment_specifier(ctx);
    }
    else
    {
        assert(false);
    }
    return type_specifier_qualifier;
}

void enum_specifier_delete(struct enum_specifier* owner opt p)
{
    if (p)
    {
        specifier_qualifier_list_delete(p->specifier_qualifier_list);
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        enumerator_list_destroy(&p->enumerator_list);
        free(p);
    }
}

struct enum_specifier* owner enum_specifier(struct parser_ctx* ctx)
{
    /*
     enum-type-specifier:
     : specifier-qualifier-list
    */

    /*
        enum-specifier:

        "enum" attribute-specifier-sequence opt identifier opt enum-type-specifier opt
        { enumerator-list }

        "enum" attribute-specifier-sequence opt identifier opt enum-type-specifier opt
        { enumerator-list , }
        enum identifier enum-type-specifier opt
    */
    struct enum_specifier* owner p_enum_specifier = calloc(1, sizeof * p_enum_specifier);
    try
    {
        if (p_enum_specifier == NULL)
            throw;

        p_enum_specifier->first_token = ctx->current;
        if (parser_match_tk(ctx, TK_KEYWORD_ENUM) != 0)
            throw;

        p_enum_specifier->attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);


        if (ctx->current->type == TK_IDENTIFIER)
        {
            snprintf(p_enum_specifier->tag_name, sizeof p_enum_specifier->tag_name, "%s", ctx->current->lexeme);


            p_enum_specifier->tag_token = ctx->current;
            parser_match(ctx);
        }
        else
        {
            snprintf(p_enum_specifier->tag_name, sizeof p_enum_specifier->tag_name, "_anonymous_struct_%d", s_anonymous_struct_count);
            s_anonymous_struct_count++;
        }

        if (ctx->current->type == ':')
        {
            if (!ctx->inside_generic_association)
            {
                /*C23*/
                parser_match(ctx);
                p_enum_specifier->specifier_qualifier_list = specifier_qualifier_list(ctx);
            }
            else
            {
                //TODO
            }
        }



        if (ctx->current->type == '{')
        {
            if (p_enum_specifier->tag_token)
                naming_convention_enum_tag(ctx, p_enum_specifier->tag_token);

            /*points to itself*/
            p_enum_specifier->complete_enum_specifier2 = p_enum_specifier;

            if (parser_match_tk(ctx, '{') != 0)
                throw;
            p_enum_specifier->enumerator_list = enumerator_list(ctx, p_enum_specifier);
            if (ctx->current->type == ',')
            {
                parser_match(ctx);
            }
            if (parser_match_tk(ctx, '}') != 0)
                throw;

            hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, p_enum_specifier, TAG_TYPE_ENUN_SPECIFIER);
            p_enum_specifier->complete_enum_specifier2 = p_enum_specifier;
        }
        else
        {
            struct enum_specifier* p_existing_enum_specifier = find_enum_specifier(ctx, p_enum_specifier->tag_token->lexeme);
            if (p_existing_enum_specifier)
            {
                //p_existing_enum_specifier->complete_enum_specifier2 = p_enum_specifier;
                //ja existe
                //verificar o caso de ser outro tag no memso escopo
                p_enum_specifier->complete_enum_specifier2 = p_existing_enum_specifier;
            }
            else
            {
                //nao existe lugar nenhum vamos adicionar
                hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, p_enum_specifier, TAG_TYPE_ENUN_SPECIFIER);
                p_enum_specifier->complete_enum_specifier2 = p_enum_specifier;
            }

            //if (!has_identifier)
            //{
              //  compiler_diagnostic_message(C_ERROR_MISSING_ENUM_TAG_NAME, ctx, ctx->current, "missing enum tag name");
                //throw;
            //}
        }


    }
    catch
    {
    }

    return p_enum_specifier;
}

void enumerator_list_add(struct enumerator_list* list, struct enumerator* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void enumerator_list_destroy(struct enumerator_list* obj_owner p)
{
    struct enumerator* owner item = p->head;
    while (item)
    {
        struct enumerator* owner next = item->next;
        item->next = NULL;
        enumerator_delete(item);
        item = next;
    }
}

struct enumerator_list enumerator_list(struct parser_ctx* ctx, const struct enum_specifier* p_enum_specifier)
{

    /*
       enumerator
        enumerator_list ',' enumerator
     */
    long long next_enumerator_value = 0;

    struct enumerator_list enumeratorlist = { 0 };
    struct enumerator* owner p_enumerator = NULL;
    try
    {
        p_enumerator = enumerator(ctx, p_enum_specifier, &next_enumerator_value);
        if (p_enumerator == NULL)
            throw;

        enumerator_list_add(&enumeratorlist, p_enumerator);

        while (ctx->current != NULL && ctx->current->type == ',')
        {
            parser_match(ctx); /*pode ter uma , vazia no fim*/

            if (ctx->current && ctx->current->type != '}')
            {
                p_enumerator = enumerator(ctx, p_enum_specifier, &next_enumerator_value);
                if (p_enumerator == NULL)
                    throw;
                enumerator_list_add(&enumeratorlist, p_enumerator);
            }
        }
    }
    catch
    {
    }

    return enumeratorlist;
}

void enumerator_delete(struct enumerator* owner opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        expression_delete(p->constant_expression_opt);

        free(p);
    }
}
struct enumerator* owner enumerator(struct parser_ctx* ctx,
    const struct enum_specifier* p_enum_specifier,
    long long* p_next_enumerator_value)
{
    // TODO VALUE
    struct enumerator* owner p_enumerator = calloc(1, sizeof(struct enumerator));
    try
    {
        if (p_enumerator == NULL)
            throw;

        p_enumerator->enum_specifier = p_enum_specifier;
        struct token* name = ctx->current;

        naming_convention_enumerator(ctx, name);

        if (parser_match_tk(ctx, TK_IDENTIFIER) != 0)
            throw;

        p_enumerator->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);

        p_enumerator->token = name;
        hashmap_set(&ctx->scopes.tail->variables, p_enumerator->token->lexeme, p_enumerator, TAG_TYPE_ENUMERATOR);

        if (ctx->current->type == '=')
        {
            parser_match(ctx);
            p_enumerator->constant_expression_opt = constant_expression(ctx, true);
            p_enumerator->value = constant_value_to_ll(&p_enumerator->constant_expression_opt->constant_value);
            *p_next_enumerator_value = p_enumerator->value;
            (*p_next_enumerator_value)++; // TODO overflow  and size check
        }
        else
        {
            p_enumerator->value = *p_next_enumerator_value;
            (*p_next_enumerator_value)++; // TODO overflow  and size check
        }
    }
    catch
    {
        enumerator_delete(p_enumerator);
        p_enumerator = NULL;
    }
    return p_enumerator;
}

void alignment_specifier_delete(struct alignment_specifier* owner opt p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        type_name_delete(p->type_name);
        free(p);
    }
}

struct alignment_specifier* owner alignment_specifier(struct parser_ctx* ctx)
{
    struct alignment_specifier* owner alignment_specifier = calloc(1, sizeof * alignment_specifier);
    try
    {
        if (alignment_specifier == NULL)
            throw;

        alignment_specifier->token = ctx->current;
        if (parser_match_tk(ctx, TK_KEYWORD__ALIGNAS) != 0)
            throw;
        if (parser_match_tk(ctx, '(') != 0)
            throw;
        if (first_of_type_name(ctx))
        {
            alignment_specifier->type_name = type_name(ctx);
            if (alignment_specifier->type_name == NULL)
                throw;
        }
        else
        {
            alignment_specifier->constant_expression = constant_expression(ctx, true);
            if (alignment_specifier->constant_expression == NULL)
                throw;
        }
        if (parser_match_tk(ctx, ')') != 0)
            throw;
    }
    catch
    {
    }
    return alignment_specifier;
}

void atomic_type_specifier_delete(struct atomic_type_specifier* owner opt p)
{
    if (p)
    {
        type_name_delete(p->type_name);
        free(p);
    }
}
struct atomic_type_specifier* owner atomic_type_specifier(struct parser_ctx* ctx)
{
    //'_Atomic' '(' type_name ')'
    struct atomic_type_specifier* owner p = calloc(1, sizeof * p);
    try
    {
        if (p == NULL)
            throw;
        p->token = ctx->current;
        if (parser_match_tk(ctx, TK_KEYWORD__ATOMIC) != 0)
            throw;
        if (parser_match_tk(ctx, '(') != 0)
            throw;
        p->type_name = type_name(ctx);
        if (parser_match_tk(ctx, ')') != 0)
            throw;
    }
    catch
    {
        atomic_type_specifier_delete(p);
        p = NULL;
    }
    return p;
}


struct type_qualifier* owner type_qualifier(struct parser_ctx* ctx)
{
    struct type_qualifier* owner p_type_qualifier = calloc(1, sizeof * p_type_qualifier);

    switch (ctx->current->type)
    {
    case TK_KEYWORD_CONST:
        p_type_qualifier->flags = TYPE_QUALIFIER_CONST;
        break;
    case TK_KEYWORD_RESTRICT:
        p_type_qualifier->flags = TYPE_QUALIFIER_RESTRICT;
        break;
    case TK_KEYWORD_VOLATILE:
        p_type_qualifier->flags = TYPE_QUALIFIER_VOLATILE;
        break;
    case TK_KEYWORD__ATOMIC:
        p_type_qualifier->flags = TYPE_QUALIFIER__ATOMIC;
        break;

    default:
        // do nothing
        break;
    }

    if (ctx->options.ownership_enabled)
    {
        switch (ctx->current->type)
        {
        case TK_KEYWORD__OUT:
            p_type_qualifier->flags = TYPE_QUALIFIER_OUT;
            break;

        case TK_KEYWORD__OWNER:
            p_type_qualifier->flags = TYPE_QUALIFIER_OWNER;
            break;

        case TK_KEYWORD__OBJ_OWNER:
            p_type_qualifier->flags = TYPE_QUALIFIER_OBJ_OWNER;
            break;
        case TK_KEYWORD__VIEW:
            p_type_qualifier->flags = TYPE_QUALIFIER_VIEW;
            break;
        default:
            // do nothing
            break;
        }
    }

    if (ctx->options.null_checks_enabled)
    {
        switch (ctx->current->type)
        {
        case TK_KEYWORD__OPT:
            p_type_qualifier->flags = TYPE_QUALIFIER_NULLABLE;
            break;

        default:
            // do nothing
            break;
        }
    }

    p_type_qualifier->token = ctx->current;

    //'const'
    //'restrict'
    //'volatile'
    //'_Atomic'
    parser_match(ctx);
    return p_type_qualifier;
}
//

struct type_qualifier* owner type_qualifier_opt(struct parser_ctx* ctx)
{
    if (first_of_type_qualifier(ctx))
    {
        return type_qualifier(ctx);
    }
    return NULL;
}

void function_specifier_delete(struct function_specifier* owner opt p)
{
    if (p)
    {
        free(p);
    }
}

struct function_specifier* owner function_specifier(struct parser_ctx* ctx)
{
    if (ctx->current->type == TK_KEYWORD__NORETURN)
    {
        compiler_diagnostic_message(W_STYLE, ctx, ctx->current, "_Noreturn is deprecated use attributes");
    }

    struct function_specifier* owner p_function_specifier = NULL;
    try
    {
        p_function_specifier = calloc(1, sizeof * p_function_specifier);
        if (p_function_specifier == NULL)
            throw;

        p_function_specifier->token = ctx->current;
        parser_match(ctx);
    }
    catch
    {
    }

    return p_function_specifier;
}

void declarator_delete(struct declarator* owner opt p)
{
    if (p)
    {
        type_destroy(&p->type);
        direct_declarator_delete(p->direct_declarator);
        pointer_delete(p->pointer);
        free(p);
    }
}
struct declarator* owner declarator(struct parser_ctx* ctx,
    const struct specifier_qualifier_list* p_specifier_qualifier_list,
    struct declaration_specifiers* p_declaration_specifiers,
    bool abstract_acceptable,
    struct token** pp_token_name)
{
    /*
      declarator:
      pointer_opt direct-declarator
    */
    struct declarator* owner p_declarator = calloc(1, sizeof(struct declarator));
    p_declarator->first_token = ctx->current;
    p_declarator->pointer = pointer_opt(ctx);
    p_declarator->direct_declarator = direct_declarator(ctx, p_specifier_qualifier_list, p_declaration_specifiers, abstract_acceptable, pp_token_name);

    if (ctx->current != p_declarator->first_token)
    {
        p_declarator->last_token = previous_parser_token(ctx->current);
    }
    else
    {
        /*empty declarator*/

        p_declarator->last_token = p_declarator->first_token;
        p_declarator->first_token = NULL; /*this is the way we can know...first is null*/
    }

    return p_declarator;
}

const char* declarator_get_name(struct declarator* p_declarator)
{
    if (p_declarator->direct_declarator)
    {
        if (p_declarator->direct_declarator->name_opt)
            return p_declarator->direct_declarator->name_opt->lexeme;
    }

    return NULL;
}

bool declarator_is_function(struct declarator* p_declarator)
{
    return (p_declarator->direct_declarator &&
        p_declarator->direct_declarator->function_declarator != NULL);
}

struct array_declarator* owner array_declarator(struct direct_declarator* owner p_direct_declarator, struct parser_ctx* ctx);
struct function_declarator* owner function_declarator(struct direct_declarator* owner p_direct_declarator, struct parser_ctx* ctx);

void function_declarator_delete(struct function_declarator* owner opt p)
{
    if (p)
    {
        direct_declarator_delete(p->direct_declarator);
        scope_destroy(&p->parameters_scope);
        parameter_type_list_delete(p->parameter_type_list_opt);
        free(p);
    }
}
void direct_declarator_delete(struct direct_declarator* owner opt p)
{
    if (p)
    {
        declarator_delete(p->declarator);
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        array_declarator_delete(p->array_declarator);
        function_declarator_delete(p->function_declarator);

        free(p);
    }
}
struct direct_declarator* owner direct_declarator(struct parser_ctx* ctx,
    const struct specifier_qualifier_list* p_specifier_qualifier_list,
    struct declaration_specifiers* p_declaration_specifiers,
    bool abstract_acceptable,
    struct token** pptoken_name)
{
    /*
    direct-declarator:
     identifier attribute-specifier-sequenceopt
     ( declarator )

     array-declarator attribute-specifier-sequenceopt
     function-declarator attribute-specifier-sequenceopt
    */
    struct direct_declarator* owner p_direct_declarator = calloc(1, sizeof(struct direct_declarator));
    try
    {
        if (ctx->current == NULL)
            throw;

        struct token* p_token_ahead = parser_look_ahead(ctx);
        if (ctx->current->type == TK_IDENTIFIER)
        {
            p_direct_declarator->name_opt = ctx->current;
            if (pptoken_name != NULL)
            {
                *pptoken_name = ctx->current;
            }

            parser_match(ctx);
            p_direct_declarator->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        }
        else if (ctx->current->type == '(')
        {
            struct token* ahead = parser_look_ahead(ctx);

            if (!first_of_type_specifier_token(ctx, p_token_ahead) &&
                !first_of_type_qualifier_token(p_token_ahead) &&
                ahead->type != ')' &&
                ahead->type != '...')
            {
                // look ahead para nao confundir (declarator) com parametros funcao ex void (int)
                // or function int ()

                parser_match(ctx);

                p_direct_declarator->declarator = declarator(ctx,
                    p_specifier_qualifier_list,
                    p_declaration_specifiers,
                    abstract_acceptable,
                    pptoken_name);

                parser_match(ctx); //)
            }
        }

        while (ctx->current != NULL &&
            (ctx->current->type == '[' || ctx->current->type == '('))
        {
            struct direct_declarator* owner p_direct_declarator2 = calloc(1, sizeof(struct direct_declarator));
            static_set(*p_direct_declarator2, "zero");
            if (ctx->current->type == '[')
            {
                p_direct_declarator2->array_declarator = array_declarator(p_direct_declarator, ctx);
            }
            else
            {
                p_direct_declarator2->function_declarator = function_declarator(p_direct_declarator, ctx);
            }
            p_direct_declarator = p_direct_declarator2;
        }
    }
    catch
    {
    }

    return p_direct_declarator;
}

void array_declarator_delete(struct array_declarator* owner opt p)
{
    if (p)
    {
        expression_delete(p->assignment_expression);
        direct_declarator_delete(p->direct_declarator);
        expression_delete(p->expression);
        type_qualifier_list_delete(p->type_qualifier_list_opt);

        free(p);
    }
}
unsigned long long array_declarator_get_size(struct array_declarator* p_array_declarator)
{
    if (p_array_declarator->assignment_expression)
    {
        if (constant_value_is_valid(&p_array_declarator->assignment_expression->constant_value))
        {
            return constant_value_to_ull(&p_array_declarator->assignment_expression->constant_value);
        }
    }
    return 0;
}

struct array_declarator* owner array_declarator(struct direct_declarator* owner p_direct_declarator, struct parser_ctx* ctx)
{
    // direct_declarator '['          type_qualifier_list_opt           assignment_expression_opt ']'
    // direct_declarator '[' 'static' type_qualifier_list_opt           assignment_expression     ']'
    // direct_declarator '['          type_qualifier_list      'static' assignment_expression     ']'
    // direct_declarator '['          type_qualifier_list_opt  '*'           ']'

    struct array_declarator* owner p_array_declarator = NULL;
    try
    {
        p_array_declarator = calloc(1, sizeof * p_array_declarator);
        if (p_array_declarator == NULL)
            throw;

        p_array_declarator->direct_declarator = p_direct_declarator;
        p_direct_declarator = NULL; /*MOVED*/

        if (parser_match_tk(ctx, '[') != 0)
            throw;

        bool has_static = false;
        if (ctx->current->type == TK_KEYWORD_STATIC)
        {
            p_array_declarator->static_token_opt = ctx->current;
            parser_match(ctx);
            has_static = true;
        }

        if (first_of_type_qualifier(ctx))
        {
            p_array_declarator->type_qualifier_list_opt = type_qualifier_list(ctx);
        }

        if (!has_static)
        {
            if (ctx->current->type == TK_KEYWORD_STATIC)
            {
                parser_match(ctx);
                has_static = true;
            }
        }

        if (has_static)
        {
            // tem que ter..

            const bool evaluation_is_disabled = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = false;
            p_array_declarator->assignment_expression = assignment_expression(ctx);
            /*restore*/
            ctx->evaluation_is_disabled = evaluation_is_disabled;

            if (p_array_declarator->assignment_expression == NULL)
                throw;
        }
        else
        {
            // opcional
            if (ctx->current->type == '*')
            {
                parser_match(ctx);
            }
            else if (ctx->current->type != ']')
            {
                const bool evaluation_is_disabled = ctx->evaluation_is_disabled;
                ctx->evaluation_is_disabled = false;
                p_array_declarator->assignment_expression = assignment_expression(ctx);

                /*restore*/
                ctx->evaluation_is_disabled = evaluation_is_disabled;
                if (p_array_declarator->assignment_expression == NULL)
                    throw;
            }
            else
            {
            }
        }

        if (parser_match_tk(ctx, ']') != 0)
            throw;
    }
    catch
    {
        if (p_array_declarator)
        {
        }
    }

    direct_declarator_delete(p_direct_declarator);

    return p_array_declarator;
}

struct function_declarator* owner function_declarator(struct direct_declarator* owner p_direct_declarator, struct parser_ctx* ctx)
{
    struct function_declarator* owner p_function_declarator = calloc(1, sizeof(struct function_declarator));
    try
    {
        if (p_function_declarator == NULL)
        {
            throw;
        }

        // faz um push da funcion_scope_declarator_list que esta vivendo mais em cima
        // eh feito o pop mais em cima tb.. aqui dentro do decide usar.
        // ctx->funcion_scope_declarator_list->outer_scope = ctx->current_scope;
        // ctx->current_scope = ctx->funcion_scope_declarator_list;
        // direct_declarator '(' parameter_type_list_opt ')'

        p_function_declarator->direct_declarator = p_direct_declarator;
        p_direct_declarator = NULL; /*MOVED*/
        p_function_declarator->parameters_scope.scope_level = ctx->scopes.tail->scope_level + 1;
        p_function_declarator->parameters_scope.variables.capacity = 5;
        p_function_declarator->parameters_scope.tags.capacity = 1;

        if (parser_match_tk(ctx, '(') != 0)
            throw;

        if (ctx->current->type != ')')
        {
            scope_list_push(&ctx->scopes, &p_function_declarator->parameters_scope);
            p_function_declarator->parameter_type_list_opt = parameter_type_list(ctx);
            scope_list_pop(&ctx->scopes);
        }
        if (parser_match_tk(ctx, ')') != 0)
            throw;

    }
    catch
    {
        function_declarator_delete(p_function_declarator);
        p_function_declarator = NULL;
    }

    direct_declarator_delete(p_direct_declarator);
    return p_function_declarator;
}

void pointer_delete(struct pointer* owner opt p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        type_qualifier_list_delete(p->type_qualifier_list_opt);

        struct pointer* owner item = p->pointer;
        while (item)
        {
            struct pointer* owner next = item->pointer;
            attribute_specifier_sequence_delete(item->attribute_specifier_sequence_opt);
            type_qualifier_list_delete(item->type_qualifier_list_opt);
            free(item);
            item = next;
        }

        free(p);
    }
}
struct pointer* owner pointer_opt(struct parser_ctx* ctx)
{
    struct pointer* owner p = NULL;
    struct pointer* owner p_pointer = NULL;
    try
    {
        while (ctx->current != NULL && ctx->current->type == '*')
        {
            p_pointer = calloc(1, sizeof(struct pointer));
            if (p_pointer == NULL)
                throw;

            static_set(*p_pointer, "zero");

            p = p_pointer;
            parser_match(ctx);

            p_pointer->attribute_specifier_sequence_opt =
                attribute_specifier_sequence_opt(ctx);

            if (first_of_type_qualifier(ctx))
            {
                p_pointer->type_qualifier_list_opt = type_qualifier_list(ctx);
            }

            while (ctx->current != NULL && ctx->current->type == '*')
            {
                p_pointer->pointer = pointer_opt(ctx);
                if (p_pointer->pointer == NULL)
                    throw;
            }
        }
    }
    catch
    {
    }

    //'*' attribute_specifier_sequence_opt type_qualifier_list_opt
    //'*' attribute_specifier_sequence_opt type_qualifier_list_opt pointer
    return p;
}

void type_qualifier_list_add(struct type_qualifier_list* list, struct type_qualifier* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void type_qualifier_list_delete(struct type_qualifier_list* owner opt p)
{
    if (p)
    {
        struct type_qualifier* owner item = p->head;
        while (item)
        {
            struct type_qualifier* owner next = item->next;
            item->next = NULL;
            free(item);
            item = next;
        }
        free(p);
    }
}

struct type_qualifier_list* owner type_qualifier_list(struct parser_ctx* ctx)
{
    // type_qualifier
    // type_qualifier_list type_qualifier

    struct type_qualifier_list* owner p_type_qualifier_list = NULL;
    struct type_qualifier* owner p_type_qualifier = NULL;

    try
    {
        p_type_qualifier_list = calloc(1, sizeof(struct type_qualifier_list));
        if (p_type_qualifier_list == NULL)
            throw;

        p_type_qualifier = type_qualifier(ctx);
        if (p_type_qualifier == NULL)
            throw;

        p_type_qualifier_list->flags |= p_type_qualifier->flags;
        type_qualifier_list_add(p_type_qualifier_list, p_type_qualifier);
        p_type_qualifier = NULL; /*MOVED*/

        while (ctx->current != NULL && first_of_type_qualifier(ctx))
        {
            p_type_qualifier = type_qualifier(ctx);
            if (p_type_qualifier == NULL)
                throw;

            p_type_qualifier_list->flags |= p_type_qualifier->flags;
            type_qualifier_list_add(p_type_qualifier_list, p_type_qualifier);
            p_type_qualifier = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return p_type_qualifier_list;
}

void parameter_type_list_delete(struct parameter_type_list* owner opt p)
{
    if (p)
    {
        parameter_list_delete(p->parameter_list);
        free(p);
    }
}

struct parameter_type_list* owner parameter_type_list(struct parser_ctx* ctx)
{
    struct parameter_type_list* owner p_parameter_type_list = calloc(1, sizeof(struct parameter_type_list));
    // parameter_list
    // parameter_list ',' '...'
    p_parameter_type_list->parameter_list = parameter_list(ctx);

    if (p_parameter_type_list->parameter_list->head ==
        p_parameter_type_list->parameter_list->tail)
    {
        if (type_is_void(&p_parameter_type_list->parameter_list->head->declarator->type))
        {
            p_parameter_type_list->is_void = true;
        }
    }

    /*ja esta saindo com a virgula consumida do parameter_list para evitar ahead*/
    if (ctx->current->type == '...')
    {
        parser_match(ctx);
        // parser_match_tk(ctx, '...');
        p_parameter_type_list->is_var_args = true;
    }
    return p_parameter_type_list;
}

void parameter_list_add(struct parameter_list* list, struct parameter_declaration* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void parameter_list_delete(struct parameter_list* owner p)
{
    if (p)
    {
        struct parameter_declaration* owner item = p->head;
        while (item)
        {
            struct parameter_declaration* owner next = item->next;
            item->next = NULL;
            parameter_declaration_delete(item);
            item = next;
        }
        free(p);
    }
}
struct parameter_list* owner parameter_list(struct parser_ctx* ctx)
{
    /*
      parameter_list
      parameter_declaration
      parameter_list ',' parameter_declaration
    */
    struct parameter_list* owner p_parameter_list = NULL;
    struct parameter_declaration* owner p_parameter_declaration = NULL;
    try
    {
        p_parameter_list = calloc(1, sizeof(struct parameter_list));
        if (p_parameter_list == NULL)
            throw;

        p_parameter_declaration = parameter_declaration(ctx);
        if (p_parameter_declaration == NULL)
            throw;

        parameter_list_add(p_parameter_list, p_parameter_declaration);
        p_parameter_declaration = NULL; /*MOVED*/

        while (ctx->current != NULL && ctx->current->type == ',')
        {
            parser_match(ctx);
            if (ctx->current->type == '...')
            {
                // follow
                break;
            }

            p_parameter_declaration = parameter_declaration(ctx);
            if (p_parameter_declaration == NULL)
                throw;

            parameter_list_add(p_parameter_list, p_parameter_declaration);
            p_parameter_declaration = NULL; /*MOVED*/
        }
    }
    catch
    {
    }
    return p_parameter_list;
}

void parameter_declaration_delete(struct parameter_declaration* owner opt p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        declaration_specifiers_delete(p->declaration_specifiers);
        declarator_delete(p->declarator);

        assert(p->next == NULL);
        free(p);
    }
}
struct parameter_declaration* owner parameter_declaration(struct parser_ctx* ctx)
{
    struct parameter_declaration* owner p_parameter_declaration = calloc(1, sizeof(struct parameter_declaration));

    p_parameter_declaration->attribute_specifier_sequence_opt =
        attribute_specifier_sequence_opt(ctx);

    p_parameter_declaration->declaration_specifiers = declaration_specifiers(ctx, STORAGE_SPECIFIER_PARAMETER);

    // talvez no ctx colocar um flag que esta em argumentos
    // TODO se tiver uma struct tag novo...
    // warning: declaration of 'struct X' will not be visible outside of this function [-Wvisibility]
    struct token* p_token_name = 0;
    p_parameter_declaration->declarator = declarator(ctx,
        /*specifier_qualifier_list*/ NULL,
        p_parameter_declaration->declaration_specifiers,
        true /*can be abstract*/,
        &p_token_name);
    p_parameter_declaration->declarator->name = p_token_name;

    p_parameter_declaration->declarator->declaration_specifiers = p_parameter_declaration->declaration_specifiers;

#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"        
    p_parameter_declaration->declarator->type = make_type_using_declarator(ctx, p_parameter_declaration->declarator);
#pragma cake diagnostic pop

    if (p_parameter_declaration->attribute_specifier_sequence_opt)
    {
        p_parameter_declaration->declarator->type.attributes_flags |=
            p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags;
    }

    p_parameter_declaration->declarator->type.storage_class_specifier_flags |= STORAGE_SPECIFIER_PARAMETER;

    if (p_parameter_declaration->implicit_token)
    {
        p_parameter_declaration->declarator->type.attributes_flags |= CAKE_ATTRIBUTE_IMPLICT;
    }

    if (p_parameter_declaration->declarator->name)
        naming_convention_parameter(ctx, p_parameter_declaration->declarator->name, &p_parameter_declaration->declarator->type);

    // coloca o pametro no escpo atual que deve apontar para escopo paramtros
    //  da funcao .
    //
    // assert ctx->current_scope->variables parametrosd
    if (p_parameter_declaration->declarator->name)
    {
        // parametro void nao te name
        hashmap_set(&ctx->scopes.tail->variables,
            p_parameter_declaration->declarator->name->lexeme,
            p_parameter_declaration->declarator,
            TAG_TYPE_ONLY_DECLARATOR);
        // print_scope(ctx->current_scope);
    }
    return p_parameter_declaration;
}

struct specifier_qualifier_list* owner copy(struct declaration_specifiers* p_declaration_specifiers)
{
    struct specifier_qualifier_list* owner p_specifier_qualifier_list = calloc(1, sizeof(struct specifier_qualifier_list));

    p_specifier_qualifier_list->type_qualifier_flags = p_declaration_specifiers->type_qualifier_flags;
    p_specifier_qualifier_list->type_specifier_flags = p_declaration_specifiers->type_specifier_flags;

    struct declaration_specifier* p_declaration_specifier =
        p_declaration_specifiers->head;

    while (p_declaration_specifier)
    {
        if (p_declaration_specifier->type_specifier_qualifier)
        {
            struct type_specifier_qualifier* owner p_specifier_qualifier = calloc(1, sizeof(struct type_specifier_qualifier));

            if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
            {
                struct type_qualifier* owner p_type_qualifier = calloc(1, sizeof(struct type_qualifier));

                p_type_qualifier->flags = p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;

                p_type_qualifier->token = p_declaration_specifier->type_specifier_qualifier->type_qualifier->token;
                p_specifier_qualifier->type_qualifier = p_type_qualifier;
            }
            else if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
            {
                struct type_specifier* owner p_type_specifier = calloc(1, sizeof(struct type_specifier));

                p_type_specifier->flags = p_declaration_specifier->type_specifier_qualifier->type_specifier->flags;

                // todo
                assert(p_declaration_specifier->type_specifier_qualifier->type_specifier->struct_or_union_specifier == NULL);

                p_type_specifier->token = p_declaration_specifier->type_specifier_qualifier->type_specifier->token;
                p_specifier_qualifier->type_specifier = p_type_specifier;
            }

            specifier_qualifier_list_add(p_specifier_qualifier_list, p_specifier_qualifier);
        }
        p_declaration_specifier = p_declaration_specifier->next;
    }
    return p_specifier_qualifier_list;
}

void print_declarator(struct osstream* ss, struct declarator* p_declarator, bool is_abstract);

void print_direct_declarator(struct osstream* ss, struct direct_declarator* p_direct_declarator, bool is_abstract)
{
    if (p_direct_declarator->declarator)
    {
        ss_fprintf(ss, "(");
        print_declarator(ss, p_direct_declarator->declarator, is_abstract);
        ss_fprintf(ss, ")");
    }

    if (p_direct_declarator->name_opt && !is_abstract)
    {
        // Se is_abstract for true é pedido para nao imprimir o nome do indentificador
        ss_fprintf(ss, "%s", p_direct_declarator->name_opt->lexeme);
    }

    if (p_direct_declarator->function_declarator)
    {
        print_direct_declarator(ss, p_direct_declarator->function_declarator->direct_declarator, is_abstract);

        ss_fprintf(ss, "(");
        struct parameter_declaration* p_parameter_declaration =
            p_direct_declarator->function_declarator->parameter_type_list_opt ? p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head : NULL;

        while (p_parameter_declaration)
        {
            if (p_parameter_declaration != p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head)
                ss_fprintf(ss, ",");

            print_declaration_specifiers(ss, p_parameter_declaration->declaration_specifiers);
            ss_fprintf(ss, " ");
            print_declarator(ss, p_parameter_declaration->declarator, is_abstract);

            p_parameter_declaration = p_parameter_declaration->next;
        }
        //... TODO
        ss_fprintf(ss, ")");
    }
    if (p_direct_declarator->array_declarator)
    {
        // TODO
        ss_fprintf(ss, "[]");
    }
}

enum type_specifier_flags declarator_get_type_specifier_flags(const struct declarator* p)
{
    if (p->declaration_specifiers)
        return p->declaration_specifiers->type_specifier_flags;
    if (p->specifier_qualifier_list)
        return p->specifier_qualifier_list->type_specifier_flags;
    return 0;
}
void print_declarator(struct osstream* ss, struct declarator* p_declarator, bool is_abstract)
{
    bool first = true;
    if (p_declarator->pointer)
    {
        struct pointer* p = p_declarator->pointer;
        while (p)
        {
            if (p->type_qualifier_list_opt)
            {
                print_type_qualifier_flags(ss, &first, p->type_qualifier_list_opt->flags);
            }
            ss_fprintf(ss, "*");
            p = p->pointer;
        }
    }
    print_direct_declarator(ss, p_declarator->direct_declarator, is_abstract);
}

void print_type_name(struct osstream* ss, struct type_name* p)
{
    bool first = true;
    print_specifier_qualifier_list(ss, &first, p->specifier_qualifier_list);
    print_declarator(ss, p->declarator, true);
}

void type_name_delete(struct type_name* owner opt p)
{
    if (p)
    {
        specifier_qualifier_list_delete(p->specifier_qualifier_list);
        type_destroy(&p->type);
        declarator_delete(p->declarator);
        free(p);
    }
}
struct type_name* owner opt type_name(struct parser_ctx* ctx)
{
    struct type_name* owner p_type_name = calloc(1, sizeof(struct type_name));

    p_type_name->first_token = ctx->current;

    p_type_name->specifier_qualifier_list = specifier_qualifier_list(ctx);

    p_type_name->declarator = declarator(ctx,
        p_type_name->specifier_qualifier_list, //??
        /*declaration_specifiers*/ NULL,
        true /*DEVE SER TODO*/,
        NULL);
    p_type_name->declarator->specifier_qualifier_list = p_type_name->specifier_qualifier_list;

#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"    
    p_type_name->declarator->type = make_type_using_declarator(ctx, p_type_name->declarator);
#pragma cake diagnostic pop

    p_type_name->last_token = ctx->current->prev;
    p_type_name->type = type_dup(&p_type_name->declarator->type);

    return p_type_name;
}

void braced_initializer_delete(struct braced_initializer* owner opt p)
{
    if (p)
    {
        initializer_list_delete(p->initializer_list);
        free(p);
    }
}
struct braced_initializer* owner braced_initializer(struct parser_ctx* ctx)
{
    /*
     { }
     { initializer-list }
     { initializer-list , }
    */

    struct braced_initializer* owner p_bracket_initializer_list = calloc(1, sizeof(struct braced_initializer));
    try
    {
        if (p_bracket_initializer_list == NULL)
            throw;
        p_bracket_initializer_list->first_token = ctx->current;
        if (parser_match_tk(ctx, '{') != 0)
            throw;
        if (ctx->current->type != '}')
        {
            p_bracket_initializer_list->initializer_list = initializer_list(ctx);
        }
        if (parser_match_tk(ctx, '}') != 0)
            throw;
    }
    catch
    {
        braced_initializer_delete(p_bracket_initializer_list);
        p_bracket_initializer_list = NULL;
    }
    return p_bracket_initializer_list;
}

void initializer_delete(struct initializer* owner opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        expression_delete(p->assignment_expression);
        braced_initializer_delete(p->braced_initializer);
        designation_delete(p->designation);
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        free(p);
    }
}

struct initializer* owner initializer(struct parser_ctx* ctx)
{
    /*
    initializer:
      assignment-expression
      braced-initializer
    */

    struct initializer* owner p_initializer = calloc(1, sizeof(struct initializer));

    p_initializer->first_token = ctx->current;

    if (ctx->current->type == '{')
    {
        p_initializer->braced_initializer = braced_initializer(ctx);
    }
    else
    {
        p_initializer->p_attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);

        p_initializer->assignment_expression = assignment_expression(ctx);
    }
    return p_initializer;
}

void initializer_list_add(struct initializer_list* list, struct initializer* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void initializer_list_delete(struct initializer_list* owner opt p)
{
    if (p)
    {
        struct initializer* owner item = p->head;
        while (item)
        {
            struct initializer* owner next = item->next;
            item->next = NULL;
            initializer_delete(item);
            item = next;
        }
        free(p);
    }
}
struct initializer_list* owner initializer_list(struct parser_ctx* ctx)
{
    /*
    initializer-list:
       designation opt initializer
       initializer-list , designation opt initializer
    */

    struct initializer_list* owner p_initializer_list = calloc(1, sizeof(struct initializer_list));

    p_initializer_list->first_token = ctx->current;

    struct designation* owner p_designation = NULL;
    if (first_of_designator(ctx))
    {
        p_designation = designation(ctx);
    }
    struct initializer* owner p_initializer = initializer(ctx);

    assert(p_initializer->designation == NULL);
    p_initializer->designation = p_designation;

    initializer_list_add(p_initializer_list, p_initializer);
    p_initializer_list->size++;

    while (ctx->current != NULL && ctx->current->type == ',')
    {
        parser_match(ctx);
        if (ctx->current->type == '}')
            break; // follow

        struct designation* owner p_designation2 = NULL;
        if (first_of_designator(ctx))
        {
            p_designation2 = designation(ctx);
        }
        struct initializer* owner p_initializer2 = initializer(ctx);

        assert(p_initializer2->designation == NULL);
        p_initializer2->designation = p_designation2;

        initializer_list_add(p_initializer_list, p_initializer2);
        p_initializer_list->size++;
    }

    return p_initializer_list;
}

void designation_delete(struct designation* owner opt p)
{
    if (p)
    {
        designator_list_delete(p->designator_list);
        free(p);
    }
}
struct designation* owner designation(struct parser_ctx* ctx)
{
    // designator_list '='
    struct designation* owner p_designation = calloc(1, sizeof(struct designation));
    try
    {
        p_designation->designator_list = designator_list(ctx);
        if (parser_match_tk(ctx, '=') != 0)
            throw;
    }
    catch
    {
        designation_delete(p_designation);
        p_designation = NULL;
    }
    return p_designation;
}


void designator_list_add(struct designator_list* list, struct designator* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void designator_list_delete(struct designator_list* owner opt p)
{
    if (p)
    {
        struct designator* owner item = p->head;
        while (item)
        {
            struct designator* owner next = item->next;
            item->next = NULL;
            designator_delete(item);
            item = next;
        }
        free(p);
    }
}
struct designator_list* owner designator_list(struct parser_ctx* ctx)
{
    // designator
    // designator_list designator
    struct designator_list* owner p_designator_list = NULL;
    struct designator* owner p_designator = NULL;
    try
    {
        p_designator_list = calloc(1, sizeof(struct designator_list));
        if (p_designator_list == NULL)
            throw;

        p_designator = designator(ctx);
        if (p_designator == NULL)
            throw;
        designator_list_add(p_designator_list, p_designator);
        p_designator = NULL; /*MOVED*/

        while (ctx->current != NULL && first_of_designator(ctx))
        {
            p_designator = designator(ctx);
            if (p_designator == NULL)
                throw;
            designator_list_add(p_designator_list, p_designator);
            p_designator = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return p_designator_list;
}

void designator_delete(struct designator* owner opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        expression_delete(p->constant_expression_opt);
        free(p);
    }
}
struct designator* owner designator(struct parser_ctx* ctx)
{
    //'[' constant_expression ']'
    //'.' identifier
    struct designator* owner p_designator = calloc(1, sizeof(struct designator));
    try
    {
        if (p_designator == NULL)
            throw;
        if (ctx->current->type == '[')
        {
            if (parser_match_tk(ctx, '[') != 0)
                throw;
            p_designator->constant_expression_opt = constant_expression(ctx, true);
            if (parser_match_tk(ctx, ']') != 0)
                throw;
        }
        else if (ctx->current->type == '.')
        {
            parser_match(ctx);
            if (parser_match_tk(ctx, TK_IDENTIFIER) != 0)
                throw;
        }
    }
    catch
    {
        designator_delete(p_designator);
        p_designator = NULL;
    }
    return p_designator;
}

void static_assert_declaration_delete(struct static_assert_declaration* owner opt p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        free(p);
    }
}

void pragma_declaration_delete(struct pragma_declaration* owner opt p)
{
    if (p)
    {
        free(p);
    }
}

void execute_pragma(struct parser_ctx* ctx, struct pragma_declaration* p_pragma, bool on_flow_analysis)
{
    struct token* p_pragma_token = p_pragma->first_token;

    if (p_pragma_token->type != TK_PRAGMA)
    {
        assert(false);
        return;
    }

    p_pragma_token = pragma_match(p_pragma_token);

    if (p_pragma_token &&
        (strcmp(p_pragma_token->lexeme, "CAKE") == 0 ||
            strcmp(p_pragma_token->lexeme, "cake") == 0))
    {
        p_pragma_token = pragma_match(p_pragma_token);
    }

    if (p_pragma_token && strcmp(p_pragma_token->lexeme, "diagnostic") == 0)
    {
        p_pragma_token = pragma_match(p_pragma_token);

        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "push") == 0)
        {
            // #pragma GCC diagnostic push
            if (ctx->options.diagnostic_stack_top_index <
                sizeof(ctx->options.diagnostic_stack) / sizeof(ctx->options.diagnostic_stack[0]))
            {
                ctx->options.diagnostic_stack_top_index++;
                ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index] =
                    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index - 1];
            }
            p_pragma_token = p_pragma_token->next;
        }
        else if (p_pragma_token && strcmp(p_pragma_token->lexeme, "pop") == 0)
        {
            // #pragma CAKE diagnostic pop
            if (ctx->options.diagnostic_stack_top_index > 0)
            {
                ctx->options.diagnostic_stack_top_index--;
            }
            p_pragma_token = pragma_match(p_pragma_token);
        }
        else if (p_pragma_token &&
            (strcmp(p_pragma_token->lexeme, "error") == 0 ||
                strcmp(p_pragma_token->lexeme, "warning") == 0 ||
                strcmp(p_pragma_token->lexeme, "note") == 0 ||
                strcmp(p_pragma_token->lexeme, "ignored") == 0))
        {
            const bool is_error = strcmp(p_pragma_token->lexeme, "error") == 0;
            const bool is_warning = strcmp(p_pragma_token->lexeme, "warning") == 0;
            const bool is_note = strcmp(p_pragma_token->lexeme, "note") == 0;

            p_pragma_token = pragma_match(p_pragma_token);

            if (p_pragma_token && p_pragma_token->type == TK_STRING_LITERAL)
            {

                unsigned long long w = get_warning_bit_mask(p_pragma_token->lexeme + 1 /*+ 2*/);

                ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors &= ~w;
                ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes &= ~w;
                ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings &= ~w;

                if (is_error)
                    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors |= w;
                else if (is_warning)
                    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings |= w;
                else if (is_note)
                    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes |= w;
            }
        }
        else if (p_pragma_token &&
            (strcmp(p_pragma_token->lexeme, "check") == 0))
        {
            p_pragma_token = pragma_match(p_pragma_token);

            if (p_pragma_token && p_pragma_token->type == TK_STRING_LITERAL)
            {
                enum diagnostic_id id = get_warning(p_pragma_token->lexeme + 1);

                if ((!on_flow_analysis && get_diagnostic_phase(id) != 2) ||
                    (on_flow_analysis && get_diagnostic_phase(id) == 2))
                {
                    if (ctx->p_report->last_diagnostic_id == id)
                    {
                        // lets remove this error/warning/info from the final report.

                        int t =
                            get_diagnostic_type(&ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index],
                                id);
                        if (t == 3)
                            ctx->p_report->error_count--;
                        else if (t == 2)
                            ctx->p_report->warnings_count--;
                        else if (t == 1)
                            ctx->p_report->info_count--;
                    }
                    else
                    {
                        compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, p_pragma_token, "pragma check failed");
                    }
                }
            }
        }
        else
        {
            compiler_diagnostic_message(C_ERROR_UNEXPECTED, ctx, p_pragma_token, "unknown pragma");
        }
    }
    else if (p_pragma_token && strcmp(p_pragma_token->lexeme, "nullable") == 0)
    {
        //see
        //https://learn.microsoft.com/en-us/dotnet/csharp/nullable-references
        p_pragma_token = pragma_match(p_pragma_token);

        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "enable") == 0)
        {
            unsigned long long w = NULLABLE_DISABLE_REMOVED_WARNINGS;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings &= ~w;

            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings |= w;
            ctx->options.null_checks_enabled = true;
            ctx->options.flow_analysis = true; //also enable flow analysis
        }
        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
        {
            unsigned long long w = NULLABLE_DISABLE_REMOVED_WARNINGS;

            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings &= ~w;


            // Dereference warnings : Disabled
            // Assignment warnings : Disabled
            // Pointer types : All are nullable
            ctx->options.null_checks_enabled = false;
        }
    }
    else if (p_pragma_token && strcmp(p_pragma_token->lexeme, "ownership") == 0)
    {
        //see
        //https://learn.microsoft.com/en-us/dotnet/csharp/nullable-references
        p_pragma_token = pragma_match(p_pragma_token);

        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "enable") == 0)
        {
            unsigned long long w = OWNERSHIP_DISABLE_REMOVED_WARNINGS;

            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings &= ~w;

            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings |= w;

            ctx->options.ownership_enabled = true;
            ctx->options.flow_analysis = true; //also enable flow analysis

        }
        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
        {
            unsigned long long w = OWNERSHIP_DISABLE_REMOVED_WARNINGS;

            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings &= ~w;

            ctx->options.ownership_enabled = false;
        }
    }
    else if (p_pragma_token && strcmp(p_pragma_token->lexeme, "flow") == 0)
    {
        p_pragma_token = pragma_match(p_pragma_token);

        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "enable") == 0)
        {
            ctx->options.flow_analysis = true;
        }
        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
        {
            ctx->options.flow_analysis = false;
        }
    }
    else if (p_pragma_token && strcmp(p_pragma_token->lexeme, "safety") == 0)
    {
        p_pragma_token = pragma_match(p_pragma_token);

        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "enable") == 0)
        {
            unsigned long long w = NULLABLE_DISABLE_REMOVED_WARNINGS | OWNERSHIP_DISABLE_REMOVED_WARNINGS;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings &= ~w;

            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings |= w;
            ctx->options.null_checks_enabled = true;
            ctx->options.flow_analysis = true; //also enable flow analysis

            ctx->options.ownership_enabled = true;
        }
        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
        {
            unsigned long long w = NULLABLE_DISABLE_REMOVED_WARNINGS | OWNERSHIP_DISABLE_REMOVED_WARNINGS;

            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].errors &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].notes &= ~w;
            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings &= ~w;

            ctx->options.null_checks_enabled = false;
            ctx->options.ownership_enabled = false;
            ctx->options.flow_analysis = false;
        }
    }

}

struct pragma_declaration* owner pragma_declaration(struct parser_ctx* ctx)
{
    assert(ctx->current->type == TK_PRAGMA);
    struct pragma_declaration* owner p_pragma_declaration = NULL;
    try
    {
        p_pragma_declaration = calloc(1, sizeof(struct pragma_declaration));
        if (p_pragma_declaration == NULL)
            throw;
        p_pragma_declaration->first_token = ctx->current;
        parser_match(ctx);
        while (ctx->current && ctx->current->type != TK_PRAGMA_END)
        {
            ctx->current = ctx->current->next;
        }
        p_pragma_declaration->last_token = ctx->current;
        parser_match(ctx);
    }
    catch
    {
    }
    if (p_pragma_declaration)
        execute_pragma(ctx, p_pragma_declaration, false);

    return p_pragma_declaration;
}

struct static_assert_declaration* owner static_assert_declaration(struct parser_ctx* ctx)
{

    /*
     static_assert-declaration:
      "static_assert" ( constant-expression , string-literal ) ;
      "static_assert" ( constant-expression ) ;
    */

    struct static_assert_declaration* owner p_static_assert_declaration = NULL;
    try
    {
        p_static_assert_declaration = calloc(1, sizeof(struct static_assert_declaration));
        if (p_static_assert_declaration == NULL)
            throw;

        p_static_assert_declaration->first_token = ctx->current;
        struct token* position = ctx->current;

        parser_match(ctx);

        if (parser_match_tk(ctx, '(') != 0)
            throw;

        /*
         When flow analysis is enabled static assert is evaluated there
        */
        bool show_error_if_not_constant = false;
        if (p_static_assert_declaration->first_token->type == TK_KEYWORD__STATIC_ASSERT)
        {
            show_error_if_not_constant = true;
        }

        p_static_assert_declaration->constant_expression = constant_expression(ctx, show_error_if_not_constant);
        if (p_static_assert_declaration->constant_expression == NULL)
            throw;

        if (ctx->current->type == ',')
        {
            parser_match(ctx);
            p_static_assert_declaration->string_literal_opt = ctx->current;
            if (parser_match_tk(ctx, TK_STRING_LITERAL) != 0)
                throw;
        }

        if (parser_match_tk(ctx, ')') != 0)
            throw;
        p_static_assert_declaration->last_token = ctx->current;
        if (parser_match_tk(ctx, ';') != 0)
            throw;

        if (position->type == TK_KEYWORD__STATIC_ASSERT)
        {
            if (!constant_value_to_bool(&p_static_assert_declaration->constant_expression->constant_value))
            {
                if (p_static_assert_declaration->string_literal_opt)
                {
                    compiler_diagnostic_message(C_ERROR_STATIC_ASSERT_FAILED, ctx, position, "_Static_assert failed %s\n",
                        p_static_assert_declaration->string_literal_opt->lexeme);
                }
                else
                {
                    compiler_diagnostic_message(C_ERROR_STATIC_ASSERT_FAILED, ctx, position, "_Static_assert failed");
                }
            }
        }
    }
    catch
    {
    }

    return p_static_assert_declaration;
}

void attribute_specifier_sequence_add(struct attribute_specifier_sequence* list, struct attribute_specifier* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void attribute_specifier_sequence_delete(struct attribute_specifier_sequence* owner opt p)
{
    if (p)
    {
        struct attribute_specifier* owner item = p->head;
        while (item)
        {
            struct attribute_specifier* owner next = item->next;
            item->next = NULL;
            attribute_specifier_delete(item);
            item = next;
        }
        free(p);
    }
}

struct attribute_specifier_sequence* owner attribute_specifier_sequence_opt(struct parser_ctx* ctx)
{
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence = NULL;

    if (first_of_attribute_specifier(ctx))
    {
        p_attribute_specifier_sequence = calloc(1, sizeof(struct attribute_specifier_sequence));

        p_attribute_specifier_sequence->first_token = ctx->current;

        while (ctx->current != NULL &&
            first_of_attribute_specifier(ctx))
        {
            struct attribute_specifier* owner p_attribute_specifier = attribute_specifier(ctx);

            p_attribute_specifier_sequence->attributes_flags |=
                p_attribute_specifier->attribute_list->attributes_flags;

            attribute_specifier_sequence_add(p_attribute_specifier_sequence, p_attribute_specifier);
        }
        p_attribute_specifier_sequence->last_token = ctx->previous;
    }

    return p_attribute_specifier_sequence;
}

struct attribute_specifier_sequence* owner attribute_specifier_sequence(struct parser_ctx* ctx)
{
    // attribute_specifier_sequence_opt attribute_specifier
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence = calloc(1, sizeof(struct attribute_specifier_sequence));
    while (ctx->current != NULL && first_of_attribute_specifier(ctx))
    {
        attribute_specifier_sequence_add(p_attribute_specifier_sequence, attribute_specifier(ctx));
    }
    return p_attribute_specifier_sequence;
}

void attribute_specifier_delete(struct attribute_specifier* owner opt p)
{
    if (p)
    {
        attribute_list_delete(p->attribute_list);
        assert(p->next == NULL);
        free(p);
    }
}
struct attribute_specifier* owner attribute_specifier(struct parser_ctx* ctx)
{
    struct attribute_specifier* owner p_attribute_specifier = calloc(1, sizeof(struct attribute_specifier));
    try
    {
        if (p_attribute_specifier == NULL)
            throw;

        p_attribute_specifier->first_token = ctx->current;

        //'[' '[' attribute_list ']' ']'
        if (parser_match_tk(ctx, '[') != 0)
            throw;
        if (parser_match_tk(ctx, '[') != 0)
            throw;
        p_attribute_specifier->attribute_list = attribute_list(ctx);
        if (parser_match_tk(ctx, ']') != 0)
            throw;
        p_attribute_specifier->last_token = ctx->current;
        if (parser_match_tk(ctx, ']') != 0)
            throw;
    }
    catch
    {
        attribute_specifier_delete(p_attribute_specifier);
        p_attribute_specifier = NULL;
    }
    return p_attribute_specifier;
}

void attribute_delete(struct attribute* owner opt p)
{
    if (p)
    {
        attribute_token_delete(p->attribute_token);
        attribute_argument_clause_delete(p->attribute_argument_clause);
        assert(p->next == NULL);
        free(p);
    }
}

void attribute_list_add(struct attribute_list* list, struct attribute* owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void attribute_list_destroy(struct attribute_list* obj_owner p)
{
    struct attribute* owner item = p->head;
    while (item)
    {
        struct attribute* owner next = item->next;
        item->next = NULL;
        attribute_delete(item);
        item = next;
    }
}
void attribute_list_delete(struct attribute_list* owner p)
{
    if (p)
    {
        attribute_list_destroy(p);
        free(p);
    }
}

struct attribute_list* owner attribute_list(struct parser_ctx* ctx)
{
    struct attribute_list* owner p_attribute_list = calloc(1, sizeof(struct attribute_list));
    //
    // attribute_list ',' attribute_opt
    while (ctx->current != NULL && (first_of_attribute(ctx) ||
        ctx->current->type == ','))
    {
        if (first_of_attribute(ctx))
        {
            struct attribute* owner p_attribute = attribute(ctx);
            p_attribute_list->attributes_flags |= p_attribute->attributes_flags;
            attribute_list_add(p_attribute_list, p_attribute);
        }
        if (ctx->current->type == ',')
        {
            parser_match(ctx);
        }
    }
    return p_attribute_list;
}

bool first_of_attribute(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;
    return ctx->current->type == TK_IDENTIFIER;
}

struct attribute* owner attribute(struct parser_ctx* ctx)
{
    struct attribute* owner p_attribute = calloc(1, sizeof(struct attribute));
    // attribute_token attribute_argument_clause_opt
    p_attribute->attribute_token = attribute_token(ctx);
    p_attribute->attributes_flags = p_attribute->attribute_token->attributes_flags;
    if (ctx->current->type == '(') // first
    {
        p_attribute->attribute_argument_clause = attribute_argument_clause(ctx);
    }
    return p_attribute;
}

void attribute_token_delete(struct attribute_token* owner opt p)
{
    if (p)
    {
        free(p);
    }
}
struct attribute_token* owner attribute_token(struct parser_ctx* ctx)
{
    struct attribute_token* owner p_attribute_token = calloc(1, sizeof(struct attribute_token));

    struct token* attr_token = ctx->current;

    bool is_standard_attribute = false;
    if (strcmp(attr_token->lexeme, "deprecated") == 0)
    {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_DEPRECATED;
    }
    else if (strcmp(attr_token->lexeme, "fallthrough") == 0)
    {
        is_standard_attribute = true;
    }
    else if (strcmp(attr_token->lexeme, "maybe_unused") == 0)
    {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_MAYBE_UNUSED;
    }
    else if (strcmp(attr_token->lexeme, "noreturn") == 0)
    {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_NORETURN;
    }
    else if (strcmp(attr_token->lexeme, "reproducible") == 0)
    {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_REPRODUCIBLE;
    }
    else if (strcmp(attr_token->lexeme, "unsequenced") == 0)
    {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_UNSEQUENCED;
    }
    else if (strcmp(attr_token->lexeme, "nodiscard") == 0)
    {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_NODISCARD;
    }

    const bool is_cake_attr =
        strcmp(attr_token->lexeme, "cake") == 0;

    if (token_is_identifier_or_keyword(ctx->current->type))
    {
        parser_match(ctx);
    }
    else
    {
        compiler_diagnostic_message(C_ERROR_UNEXPECTED_TOKEN, ctx, attr_token, "expected identifier");
    }

    if (ctx->current->type == '::')
    {
        parser_match(ctx);
        if (is_cake_attr)
        {
            compiler_diagnostic_message(W_ATTRIBUTES, ctx, attr_token, "warning '%s' is not an cake attribute", ctx->current->lexeme);
        }
        if (token_is_identifier_or_keyword(ctx->current->type))
        {
            parser_match(ctx);
        }
        else
        {
            compiler_diagnostic_message(C_ERROR_UNEXPECTED_TOKEN, ctx, attr_token, "expected identifier");
        }
    }
    else
    {
        /*
         * Each implementation should choose a distinctive name for the attribute prefix in an attribute
         * prefixed token. Implementations should not define attributes without an attribute prefix unless it is
         * a standard attribute as specified in this document.
         */
        if (!is_standard_attribute)
        {
            compiler_diagnostic_message(W_ATTRIBUTES, ctx, attr_token, "warning '%s' is not an standard attribute", attr_token->lexeme);
        }
    }
    return p_attribute_token;
}

void attribute_argument_clause_delete(struct attribute_argument_clause* owner opt p)
{
    if (p)
    {
        balanced_token_sequence_delete(p->p_balanced_token_sequence);
        free(p);
    }
}
struct attribute_argument_clause* owner attribute_argument_clause(struct parser_ctx* ctx)
{
    struct attribute_argument_clause* owner p_attribute_argument_clause = calloc(1, sizeof(struct attribute_argument_clause));
    try
    {
        if (p_attribute_argument_clause == NULL)
            throw;
        //'(' balanced_token_sequence_opt ')'
        if (parser_match_tk(ctx, '(') != 0)
            throw;
        p_attribute_argument_clause->p_balanced_token_sequence = balanced_token_sequence_opt(ctx);
        if (parser_match_tk(ctx, ')') != 0)
            throw;
    }
    catch
    {
        attribute_argument_clause_delete(p_attribute_argument_clause);
        p_attribute_argument_clause = NULL;
    }
    return p_attribute_argument_clause;
}

void balanced_token_sequence_delete(struct balanced_token_sequence* owner opt p)
{
    if (p)
    {
        struct balanced_token* owner item = p->head;
        while (item)
        {
            struct balanced_token* owner next = item->next;
            free(item);
            item = next;
        }
        free(p);
    }
}
struct balanced_token_sequence* owner balanced_token_sequence_opt(struct parser_ctx* ctx)
{
    struct balanced_token_sequence* owner p_balanced_token_sequence = calloc(1, sizeof(struct balanced_token_sequence));
    // balanced_token
    // balanced_token_sequence balanced_token
    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    for (; ctx->current;)
    {
        if (ctx->current->type == '(')
            count1++;
        else if (ctx->current->type == '[')
            count2++;
        else if (ctx->current->type == '{')
            count3++;
        else if (ctx->current->type == ')')
        {
            if (count1 == 0)
            {
                // parser_match(ctx);
                break;
            }
            count1--;
        }
        else if (ctx->current->type == '[')
            count2--;
        else if (ctx->current->type == '{')
            count3--;
        parser_match(ctx);
    }
    if (count2 != 0)
    {
        compiler_diagnostic_message(C_ERROR_ATTR_UNBALANCED, ctx, ctx->current, "expected ']' before ')'");
    }
    if (count3 != 0)
    {
        compiler_diagnostic_message(C_ERROR_ATTR_UNBALANCED, ctx, ctx->current, "expected '}' before ')'");
    }
    return p_balanced_token_sequence;
}

void statement_delete(struct statement* owner opt p)
{
    if (p)
    {
        labeled_statement_delete(p->labeled_statement);
        unlabeled_statement_delete(p->unlabeled_statement);
        free(p);
    }
}
struct statement* owner statement(struct parser_ctx* ctx)
{
    struct statement* owner p_statement = calloc(1, sizeof(struct statement));
    try
    {
        if (p_statement == NULL)
            throw;

        if (first_of_labeled_statement(ctx))
        {
            p_statement->labeled_statement = labeled_statement(ctx);
            if (p_statement->labeled_statement == NULL)
                throw;
        }
        else
        {
            p_statement->unlabeled_statement = unlabeled_statement(ctx);
            if (p_statement->unlabeled_statement == NULL)
                throw;
        }
    }
    catch
    {
        statement_delete(p_statement);
        p_statement = NULL;
    }
    return p_statement;
}

struct primary_block* owner primary_block(struct parser_ctx* ctx)
{
    assert(ctx->current != NULL);
    struct primary_block* owner p_primary_block = calloc(1, sizeof(struct primary_block));
    try
    {
        if (p_primary_block == NULL)
            throw;

        if (first_of_compound_statement(ctx))
        {
            p_primary_block->compound_statement = compound_statement(ctx);
            if (p_primary_block->compound_statement == NULL)
                throw;
        }
        else if (first_of_selection_statement(ctx))
        {
            p_primary_block->selection_statement = selection_statement(ctx);
            if (p_primary_block->selection_statement == NULL)
                throw;
        }
        else if (first_of_iteration_statement(ctx))
        {
            p_primary_block->iteration_statement = iteration_statement(ctx);
            if (p_primary_block->iteration_statement == NULL)
                throw;
        }
        else if (ctx->current->type == TK_KEYWORD_DEFER)
        {
            p_primary_block->defer_statement = defer_statement(ctx);
            if (p_primary_block->defer_statement == NULL)
                throw;
        }
        else if (ctx->current->type == TK_KEYWORD_TRY)
        {
            p_primary_block->try_statement = try_statement(ctx);
            if (p_primary_block->try_statement == NULL)
                throw;
        }
        else
        {
            compiler_diagnostic_message(C_ERROR_UNEXPECTED_TOKEN, ctx, ctx->current, "unexpected token");
        }
    }
    catch
    {
        primary_block_delete(p_primary_block);
        p_primary_block = NULL;
    }
    return p_primary_block;
}

struct secondary_block* owner secondary_block(struct parser_ctx* ctx)
{
    check_open_brace_style(ctx, ctx->current);

    struct secondary_block* owner p_secondary_block = calloc(1, sizeof(struct secondary_block));
    try
    {
        if (p_secondary_block == NULL)
            throw;

        p_secondary_block->first_token = ctx->current;

        p_secondary_block->statement = statement(ctx);
        if (p_secondary_block->statement == NULL)
            throw;

        p_secondary_block->last_token = ctx->previous;

        check_close_brace_style(ctx, p_secondary_block->last_token);
    }
    catch
    {
        secondary_block_delete(p_secondary_block);
        p_secondary_block = NULL;
    }
    return p_secondary_block;
}

bool unlabeled_statement_ends_with_jump(struct unlabeled_statement* p_unlabeled_statement)
{
    struct expression* p_expression = NULL;

    if (p_unlabeled_statement->expression_statement)
    {
        p_expression = p_unlabeled_statement->expression_statement->expression_opt;
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        return true;
    }
    else if (p_unlabeled_statement->primary_block &&
        p_unlabeled_statement->primary_block->compound_statement &&
        p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail &&
        p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement &&
        p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement->expression_statement)
    {
        p_expression =
            p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement->expression_statement->expression_opt;
    }

    if (p_expression)
    {
        return p_expression->type.attributes_flags & STD_ATTRIBUTE_NORETURN;
    }

    if (p_unlabeled_statement->primary_block &&
        p_unlabeled_statement->primary_block->compound_statement &&
        p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail &&
        p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement)
    {
        return
            p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != NULL;
    }
    
    return false;
}

bool secondary_block_ends_with_jump(struct secondary_block* p_secondary_block)
{
    if (p_secondary_block &&
        p_secondary_block->statement &&
        p_secondary_block->statement->unlabeled_statement)
    {
        return unlabeled_statement_ends_with_jump(p_secondary_block->statement->unlabeled_statement);
    }
    return false;
}

void secondary_block_delete(struct secondary_block* owner opt p)
{
    if (p)
    {
        statement_delete(p->statement);
        free(p);
    }
}
void primary_block_delete(struct primary_block* owner opt p)
{
    if (p)
    {
        compound_statement_delete(p->compound_statement);
        defer_statement_delete(p->defer_statement);
        iteration_statement_delete(p->iteration_statement);
        selection_statement_delete(p->selection_statement);
        try_statement_delete(p->try_statement);
        free(p);
    }
}
bool first_of_primary_block(struct parser_ctx* ctx)
{
    if (first_of_compound_statement(ctx) ||
        first_of_selection_statement(ctx) ||
        first_of_iteration_statement(ctx) ||
        ctx->current->type == TK_KEYWORD_DEFER /*extension*/ ||
        ctx->current->type == TK_KEYWORD_TRY /*extension*/
        )
    {
        return true;
    }
    return false;
}

void unlabeled_statement_delete(struct unlabeled_statement* owner opt p)
{
    if (p)
    {
        expression_statement_delete(p->expression_statement);
        jump_statement_delete(p->jump_statement);
        primary_block_delete(p->primary_block);
        free(p);
    }
}
struct unlabeled_statement* owner unlabeled_statement(struct parser_ctx* ctx)
{
    /*
     unlabeled-statement:
       expression-statement
       attribute-specifier-sequence opt primary-block
       attribute-specifier-sequence opt jump-statement
    */
    struct unlabeled_statement* owner p_unlabeled_statement = calloc(1, sizeof(struct unlabeled_statement));
    try
    {
        if (first_of_primary_block(ctx))
        {
            p_unlabeled_statement->primary_block = primary_block(ctx);
            if (p_unlabeled_statement->primary_block == NULL)
                throw;
        }
        else if (first_of_jump_statement(ctx))
        {
            p_unlabeled_statement->jump_statement = jump_statement(ctx);
            if (p_unlabeled_statement->jump_statement == NULL)
                throw;
        }
        else
        {
            p_unlabeled_statement->expression_statement = expression_statement(ctx, false);
            if (p_unlabeled_statement->expression_statement == NULL)
                throw;

            if (p_unlabeled_statement->expression_statement->expression_opt)
            {
                if (!type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type) &&
                    type_is_nodiscard(&p_unlabeled_statement->expression_statement->expression_opt->type) &&
                    p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
                {

                    if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                    {
                        compiler_diagnostic_message(W_ATTRIBUTES, ctx,
                            p_unlabeled_statement->expression_statement->expression_opt->first_token,
                            "ignoring return value of function declared with 'nodiscard' attribute");
                    }
                }
                if (type_is_owner(&p_unlabeled_statement->expression_statement->expression_opt->type) &&
                    p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
                {

                    if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                    {
                        compiler_diagnostic_message(W_ATTRIBUTES, ctx,
                            p_unlabeled_statement->expression_statement->expression_opt->first_token,
                            "ignoring the result of owner type ");
                    }
                }
            }
            if (p_unlabeled_statement->expression_statement->expression_opt &&
                p_unlabeled_statement->expression_statement->expression_opt->expression_type == POSTFIX_FUNCTION_CALL)
            {
            }
            else
            {
                /*
                 *  The objective here is to detect expression with not effect
                 *  a == b; etc
                 */
                if (p_unlabeled_statement != NULL &&
                    p_unlabeled_statement->jump_statement == NULL &&
                    p_unlabeled_statement->expression_statement != NULL &&
                    p_unlabeled_statement->expression_statement->expression_opt &&
                    !type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type) &&
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != ASSIGNMENT_EXPRESSION &&
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != POSTFIX_FUNCTION_CALL &&
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != POSTFIX_INCREMENT &&
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != POSTFIX_DECREMENT &&
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != UNARY_EXPRESSION_INCREMENT &&
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != UNARY_EXPRESSION_DECREMENT &&
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != UNARY_DECLARATOR_ATTRIBUTE_EXPR &&
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != UNARY_EXPRESSION_ASSERT)
                {
                    if (ctx->current &&
                        ctx->current->level == 0)
                    {
#if 0
                        //too many false..alerts.
                        //make list of for sure ...
                        compiler_diagnostic_message(W_UNUSED_VALUE,
                            ctx,
                            p_unlabeled_statement->expression_statement->expression_opt->first_token,
                            "expression not used");
#endif
                    }
                }
            }
                    }
                }
    catch
    {
        unlabeled_statement_delete(p_unlabeled_statement);
        p_unlabeled_statement = NULL;
    }

    return p_unlabeled_statement;
            }

void label_delete(struct label* owner opt p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        free(p);
    }
}
struct label* owner label(struct parser_ctx* ctx)
{
    struct label* owner p_label = calloc(1, sizeof(struct label));
    try
    {
        if (ctx->current->type == TK_IDENTIFIER)
        {
            p_label->name = ctx->current;
            parser_match(ctx);
            if (parser_match_tk(ctx, ':') != 0)
                throw;
        }
        else if (ctx->current->type == TK_KEYWORD_CASE)
        {
            parser_match(ctx);
            p_label->constant_expression = constant_expression(ctx, true);
            if (parser_match_tk(ctx, ':') != 0)
                throw;
        }
        else if (ctx->current->type == TK_KEYWORD_DEFAULT)
        {
            parser_match(ctx);
            if (parser_match_tk(ctx, ':') != 0)
                throw;
        }
        // attribute_specifier_sequence_opt identifier ':'
        // attribute_specifier_sequence_opt 'case' constant_expression ':'
        // attribute_specifier_sequence_opt 'default' ':'
    }
    catch
    {
        label_delete(p_label);
        p_label = NULL;
    }
    return p_label;
}

void labeled_statement_delete(struct labeled_statement* owner opt p)
{
    if (p)
    {
        label_delete(p->label);
        statement_delete(p->statement);
        free(p);
    }
}
struct labeled_statement* owner labeled_statement(struct parser_ctx* ctx)
{
    struct labeled_statement* owner p_labeled_statement = calloc(1, sizeof(struct labeled_statement));
    // label statement
    p_labeled_statement->label = label(ctx);
    p_labeled_statement->statement = statement(ctx);
    return p_labeled_statement;
}

void compound_statement_delete(struct compound_statement* owner opt p)
{
    if (p)
    {
        block_item_list_destroy(&p->block_item_list);
        free(p);
    }
}

struct compound_statement* owner compound_statement(struct parser_ctx* ctx)
{

    //'{' block_item_list_opt '}'
    struct compound_statement* owner p_compound_statement = calloc(1, sizeof(struct compound_statement));
    struct scope block_scope = { .variables.capacity = 10 };

    try
    {
        if (p_compound_statement == NULL)
            throw;

        p_compound_statement->diagnostic_flags = ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index];

        scope_list_push(&ctx->scopes, &block_scope);

        p_compound_statement->first_token = ctx->current;
        if (parser_match_tk(ctx, '{') != 0)
            throw;

        if (ctx->current->type != '}')
        {
            bool berror = false;
            p_compound_statement->block_item_list = block_item_list(ctx, &berror);
            if (berror)
                throw;
        }

        p_compound_statement->last_token = ctx->current;
        if (parser_match_tk(ctx, '}') != 0)
            throw;

        // TODO ver quem nao foi usado.

        for (int i = 0; i < block_scope.variables.capacity; i++)
        {
            if (block_scope.variables.table == NULL)
                continue;
            struct map_entry* entry = block_scope.variables.table[i];
            while (entry)
            {

                if (entry->type != TAG_TYPE_ONLY_DECLARATOR &&
                    entry->type != TAG_TYPE_INIT_DECLARATOR)
                {
                    entry = entry->next;
                    continue;
                }

                struct declarator* p_declarator = NULL;
                struct init_declarator* p_init_declarator = NULL;
                if (entry->type == TAG_TYPE_INIT_DECLARATOR)
                {
                    p_init_declarator = entry->p;
                    p_declarator = p_init_declarator->p_declarator;
                }
                else
                {
                    p_declarator = entry->p;
                }

                if (p_declarator)
                {

                    if (!type_is_maybe_unused(&p_declarator->type) &&
                        p_declarator->num_uses == 0)
                    {
                        if (p_declarator->name->token_origin->level == 0)
                        {
                            compiler_diagnostic_message(W_UNUSED_VARIABLE,
                                ctx,
                                p_declarator->name,
                                "'%s': unreferenced declarator",
                                p_declarator->name->lexeme);
                        }
                    }
                }

                entry = entry->next;
            }
        }
    }
    catch
    {
        compound_statement_delete(p_compound_statement);
        p_compound_statement = NULL;
    }
    scope_list_pop(&ctx->scopes);

    scope_destroy(&block_scope);

    return p_compound_statement;
}

void block_item_list_add(struct block_item_list* list, struct block_item* owner p_item)
{

    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;

}

void block_item_list_destroy(struct block_item_list* obj_owner list)
{
    struct block_item* owner item = list->head;
    while (item)
    {
        struct block_item* owner next = item->next;
        item->next = NULL;
        block_item_delete(item);
        item = next;
    }
}
struct block_item_list block_item_list(struct parser_ctx* ctx, bool* error)
{
    /*
      block_item_list:
      block_item
      block_item_list block_item
    */
    *error = false;
    struct block_item_list block_item_list = { 0 };
    struct block_item* owner p_block_item = NULL;
    try
    {
        p_block_item = block_item(ctx);
        if (p_block_item == NULL)
            throw;

        block_item_list_add(&block_item_list, p_block_item);

        p_block_item = NULL; /*MOVED*/

        while (ctx->current != NULL && ctx->current->type != '}') // follow
        {
            p_block_item = block_item(ctx);
            if (p_block_item == NULL)
                throw;
            block_item_list_add(&block_item_list, p_block_item);
            p_block_item = NULL; /*MOVED*/
        }
    }
    catch
    {
        *error = true;
    }

    return block_item_list;
}

void block_item_delete(struct block_item* owner opt p)
{
    if (p)
    {
        declaration_delete(p->declaration);
        label_delete(p->label);
        unlabeled_statement_delete(p->unlabeled_statement);
        assert(p->next == NULL);
        free(p);
    }
}

struct block_item* owner block_item(struct parser_ctx* ctx)
{
    //   declaration
    //     unlabeled_statement
    //   label
    struct block_item* owner p_block_item = calloc(1, sizeof(struct block_item));

    try
    {
        /*
         * Attributes can be first of declaration, labels etc..
         * so it is better to parse it in advance.
         */
        struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);

        p_block_item->first_token = ctx->current;

        if (ctx->current->type == TK_KEYWORD__ASM)
        { /*
       asm-block:
       __asm assembly-instruction ;opt
       __asm { assembly-instruction-list } ;opt

   assembly-instruction-list:
       assembly-instruction ;opt
       assembly-instruction ; assembly-instruction-list ;opt
       */

            parser_match(ctx);
            if (ctx->current->type == '{')
            {
                parser_match(ctx);
                while (ctx->current->type != '}')
                {
                    parser_match(ctx);
                }
                parser_match(ctx);
            }
            else
            {
                while (ctx->current->type != TK_NEWLINE)
                {
                    ctx->current = ctx->current->next;
                }
                parser_match(ctx);
            }
            if (ctx->current->type == ';')
                parser_match(ctx);
        }
        else if (first_of_declaration_specifier(ctx) ||
            first_of_static_assert_declaration(ctx) ||
            first_of_pragma_declaration(ctx))
        {
            p_block_item->declaration = declaration(ctx, p_attribute_specifier_sequence_opt, STORAGE_SPECIFIER_AUTOMATIC_STORAGE);
            if (p_block_item->declaration == NULL)
                throw;
            p_attribute_specifier_sequence_opt = NULL; /*MOVED*/

            struct init_declarator* p = p_block_item->declaration->init_declarator_list.head;
            while (p)
            {
                if (p->p_declarator && p->p_declarator->name)
                {
                    naming_convention_local_var(ctx, p->p_declarator->name, &p->p_declarator->type);
                }
                p = p->next;
            }
        }
        else if (first_of_label(ctx))
        {
            // so identifier confunde com expression
            p_block_item->label = label(ctx);
            if (p_block_item->label == NULL)
            {
                attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                throw;
            }
        }
        else
        {
            p_block_item->unlabeled_statement = unlabeled_statement(ctx);
            if (p_block_item->unlabeled_statement == NULL)
            {
                attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                throw;
            }
        }
        /*
                                               declaration-specifiers init-declarator-list_opt;
                  attribute-specifier-sequence declaration-specifiers init-declarator-list;
                  static_assert-declaration attribute_declaration
        */
        /*
        unlabeled-statement:
         expression-statement
         attribute-specifier-sequenceopt compound-statement
         attribute-specifier-sequenceopt selection-statement
         attribute-specifier-sequenceopt iteration-statement
         attribute-specifier-sequenceopt jump-statement

        label:
        attribute-specifier-sequenceopt identifier :
        attribute-specifier-sequenceopt case constant-expression :
        attribute-specifier-sequenceopt default :
        */

        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
    }
    catch
    {
        block_item_delete(p_block_item);
        p_block_item = NULL;
    }
    return p_block_item;
}

void try_statement_delete(struct try_statement* owner opt p)
{
    if (p)
    {
        secondary_block_delete(p->catch_secondary_block_opt);
        secondary_block_delete(p->secondary_block);
        free(p);
    }
}
struct try_statement* owner try_statement(struct parser_ctx* ctx)
{
    struct try_statement* owner p_try_statement = calloc(1, sizeof(struct try_statement));
    try
    {
        p_try_statement->first_token = ctx->current;

        assert(ctx->current->type == TK_KEYWORD_TRY);
        const struct try_statement* try_statement_copy_opt = ctx->p_current_try_statement_opt;
        ctx->p_current_try_statement_opt = p_try_statement;
        ctx->try_catch_block_index++;
        p_try_statement->try_catch_block_index = ctx->try_catch_block_index;


        if (parser_match_tk(ctx, TK_KEYWORD_TRY) != 0)
            throw;

#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"    
        p_try_statement->secondary_block = secondary_block(ctx);
#pragma cake diagnostic pop

        /*restores the previous one*/
        ctx->p_current_try_statement_opt = try_statement_copy_opt;

        if (ctx->current->type == TK_KEYWORD_CATCH)
        {
            p_try_statement->catch_token_opt = ctx->current;
            parser_match(ctx);

            assert(p_try_statement->catch_secondary_block_opt == NULL);
            p_try_statement->catch_secondary_block_opt = secondary_block(ctx);
        }
        p_try_statement->last_token = ctx->previous;
    }
    catch
    {
        try_statement_delete(p_try_statement);
        p_try_statement = NULL;
    }
    return p_try_statement;
}

void selection_statement_delete(struct selection_statement* owner opt p)
{
    if (p)
    {
        secondary_block_delete(p->secondary_block);
        secondary_block_delete(p->else_secondary_block_opt);

        condition_delete(p->condition);
        init_statement_delete(p->p_init_statement);
        free(p);
    }
}
struct selection_statement* owner selection_statement(struct parser_ctx* ctx)
{
    /*
    selection-statement: //C23
        "if" ( expression ) secondary-block
        "if" ( expression ) secondary-block else secondary-block
        "switch" ( expression ) secondary-block
    */

    /* Extension

    selection-statement: C2Y?
       "if" ( init-statement opt condition ) secondary-block
       "if" ( init-statement opt condition ) secondary-block "else" secondary-block
       switch ( init-statement opt condition ) secondary-block
    */

    struct scope if_scope = { 0 };
    scope_list_push(&ctx->scopes, &if_scope);

    struct selection_statement* owner p_selection_statement = calloc(1, sizeof(struct selection_statement));
    try
    {
        if (p_selection_statement == NULL)
            throw;

        p_selection_statement->first_token = ctx->current;

        const bool is_if = (ctx->current->type == TK_KEYWORD_IF);
        if (ctx->current->type == TK_KEYWORD_IF ||
            ctx->current->type == TK_KEYWORD_SWITCH)
        {
            parser_match(ctx);
        }
        else
        {
            throw;
        }

        if (!(ctx->current->flags & TK_FLAG_MACRO_EXPANDED) && !style_has_one_space(ctx->current))
        {
            compiler_diagnostic_message(W_STYLE, ctx, ctx->current, "one space");
        }

        p_selection_statement->open_parentesis_token = ctx->current;


        if (parser_match_tk(ctx, '(') != 0)
            throw;

        /*
           init-statement and condition are  almost the same.
           the diference is that init-statement has ;
        */
        p_selection_statement->p_init_statement = init_statement(ctx, true);
        if (p_selection_statement->p_init_statement == NULL)
            throw;

        if (ctx->current->type == ';')
        {
            /*
              We only know if we are at init-statement if we find ;
            */

            //  fixing the last_token            
            if (p_selection_statement->p_init_statement->p_simple_declaration)
                p_selection_statement->p_init_statement->p_simple_declaration->last_token = ctx->current;
            else if (p_selection_statement->p_init_statement->p_expression_statement)
                p_selection_statement->p_init_statement->p_simple_declaration->last_token = ctx->current;

            parser_match(ctx);
            p_selection_statement->condition = condition(ctx);
        }
        else if (ctx->current->type == ')')
        {
            /*
              We only know if we are at init-statement if we find ;
              In this case it is NOT.
              So we copy parts of init-statement to condition
            */

            /*ignore that init_statement*/
            p_selection_statement->condition = calloc(1, sizeof * p_selection_statement->condition);
            if (p_selection_statement->condition == NULL) throw;

            //steal expression
            if (p_selection_statement->p_init_statement->p_expression_statement)
            {
                p_selection_statement->condition->first_token = p_selection_statement->p_init_statement->p_expression_statement->expression_opt->first_token;
                p_selection_statement->condition->last_token = p_selection_statement->p_init_statement->p_expression_statement->expression_opt->last_token;

                p_selection_statement->condition->expression =
                    p_selection_statement->p_init_statement->p_expression_statement->expression_opt;
                p_selection_statement->p_init_statement->p_expression_statement->expression_opt = NULL;
            }

            if (p_selection_statement->p_init_statement->p_simple_declaration)
            {
                p_selection_statement->condition->first_token = p_selection_statement->p_init_statement->p_simple_declaration->first_token;
                p_selection_statement->condition->last_token = p_selection_statement->p_init_statement->p_simple_declaration->last_token;

                if (p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head !=
                    p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.tail)
                {
                    //tODO only 1
                    assert(false);
                    throw;
                }
                p_selection_statement->condition->p_init_declarator =
                    p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head;

                p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head = NULL;
                p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.tail = NULL;

                p_selection_statement->condition->p_declaration_specifiers =
                    p_selection_statement->p_init_statement->p_simple_declaration->p_declaration_specifiers;
                p_selection_statement->p_init_statement->p_simple_declaration->p_declaration_specifiers = NULL;
            }


            init_statement_delete(p_selection_statement->p_init_statement);
            p_selection_statement->p_init_statement = NULL;
        }

        p_selection_statement->close_parentesis_token = ctx->current;
        if (parser_match_tk(ctx, ')') != 0)
            throw;

        //if (constant_value_is_valid(&p_selection_statement->init_statement_expression->constant_value))
        //{
            //compiler_diagnostic_message(W_CONDITIONAL_IS_CONSTANT, ctx, p_selection_statement->init_statement_expression->first_token, "conditional expression is constant");
        //}

        p_selection_statement->secondary_block = secondary_block(ctx);
        if (p_selection_statement->secondary_block == NULL)
            throw;

        if (is_if && ctx->current && ctx->current->type == TK_KEYWORD_ELSE)
        {
            p_selection_statement->else_token_opt = ctx->current;
            parser_match(ctx);
            p_selection_statement->else_secondary_block_opt = secondary_block(ctx);
        }


        p_selection_statement->last_token = previous_parser_token(ctx->current);
    }
    catch
    {
        selection_statement_delete(p_selection_statement);
        p_selection_statement = NULL;
    }
    scope_list_pop(&ctx->scopes);
    scope_destroy(&if_scope);

    return p_selection_statement;
}

struct defer_statement* owner defer_statement(struct parser_ctx* ctx)
{
    struct defer_statement* owner p_defer_statement = calloc(1, sizeof(struct defer_statement));
    if (ctx->current->type == TK_KEYWORD_DEFER)
    {
        p_defer_statement->first_token = ctx->current;
        parser_match(ctx);
        p_defer_statement->secondary_block = secondary_block(ctx);
        p_defer_statement->last_token = ctx->previous;
    }
    return p_defer_statement;
}

void iteration_statement_delete(struct iteration_statement* owner opt p)
{
    if (p)
    {
        expression_delete(p->expression0);
        expression_delete(p->expression1);
        expression_delete(p->expression2);
        declaration_delete(p->declaration);
        secondary_block_delete(p->secondary_block);
        free(p);
    }
}
struct iteration_statement* owner iteration_statement(struct parser_ctx* ctx)
{
    /*
    iteration-statement:
      while ( expression ) statement
      do statement while ( expression ) ;
      for ( expressionopt ; expressionopt ; expressionopt ) statement
      for ( declaration expressionopt ; expressionopt ) statement
    */
    struct iteration_statement* owner p_iteration_statement = calloc(1, sizeof(struct iteration_statement));
    try
    {

        p_iteration_statement->first_token = ctx->current;
        if (ctx->current->type == TK_KEYWORD_DO)
        {
            parser_match(ctx);
            p_iteration_statement->secondary_block = secondary_block(ctx);
            p_iteration_statement->second_token = ctx->current;

            if (parser_match_tk(ctx, TK_KEYWORD_WHILE) != 0)
                throw;
            if (parser_match_tk(ctx, '(') != 0)
                throw;

            p_iteration_statement->expression1 = expression(ctx);
            if (parser_match_tk(ctx, ')') != 0)
                throw;
            if (parser_match_tk(ctx, ';') != 0)
                throw;
        }
        else if (ctx->current->type == TK_KEYWORD_WHILE)
        {
            parser_match(ctx);
            if (parser_match_tk(ctx, '(') != 0)
                throw;

            p_iteration_statement->expression1 = expression(ctx);
            if (parser_match_tk(ctx, ')') != 0)
                throw;
            p_iteration_statement->secondary_block = secondary_block(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD_FOR)
        {
            parser_match(ctx);
            if (parser_match_tk(ctx, '(') != 0)
                throw;
            if (first_of_declaration_specifier(ctx))
            {
                struct scope for_scope = { 0 };
                scope_list_push(&ctx->scopes, &for_scope);

                p_iteration_statement->declaration = declaration(ctx, NULL, STORAGE_SPECIFIER_AUTOMATIC_STORAGE);
                if (ctx->current->type != ';')
                {
                    p_iteration_statement->expression1 = expression(ctx);
                    if (p_iteration_statement->expression1 == NULL)
                    {
                        scope_list_pop(&ctx->scopes);
                        scope_destroy(&for_scope);
                        throw;
                    }
                }

                if (parser_match_tk(ctx, ';') != 0)
                {
                    scope_destroy(&for_scope);
                    throw;
                }

                if (ctx->current->type != ')')
                    p_iteration_statement->expression2 = expression(ctx);

                if (parser_match_tk(ctx, ')') != 0)
                {
                    scope_destroy(&for_scope);
                    throw;
                }

                p_iteration_statement->secondary_block = secondary_block(ctx);

                scope_list_pop(&ctx->scopes);

                scope_destroy(&for_scope);
            }
            else
            {
                /*
                 *   int i;
                 *   for (i = 0; i < 10; i++)
                 *   {
                 *   }
                 */

                if (ctx->current->type != ';')
                    p_iteration_statement->expression0 = expression(ctx);
                if (parser_match_tk(ctx, ';') != 0)
                    throw;
                if (ctx->current->type != ';')
                    p_iteration_statement->expression1 = expression(ctx);
                if (parser_match_tk(ctx, ';') != 0)
                    throw;
                if (ctx->current->type != ')')
                    p_iteration_statement->expression2 = expression(ctx);
                if (parser_match_tk(ctx, ')') != 0)
                    throw;

                p_iteration_statement->secondary_block = secondary_block(ctx);
            }
        }
    }
    catch
    {
        iteration_statement_delete(p_iteration_statement);
        p_iteration_statement = NULL;
    }
    return p_iteration_statement;
}

void jump_statement_delete(struct jump_statement* owner opt p)
{
    if (p)
    {
        expression_delete(p->expression_opt);
        free(p);
    }
}
struct jump_statement* owner jump_statement(struct parser_ctx* ctx)
{
    /*
      jump-statement:
            goto identifier ;
            continue ;
            break ;
            return expressionopt ;
    */

    /*
       throw; (extension)
    */

    struct jump_statement* owner p_jump_statement = calloc(1, sizeof(struct jump_statement));
    try
    {
        if (p_jump_statement == NULL)
            throw;

        p_jump_statement->first_token = ctx->current;

        if (ctx->current->type == TK_KEYWORD_GOTO)
        {
            parser_match(ctx);
            p_jump_statement->label = ctx->current;
            if (parser_match_tk(ctx, TK_IDENTIFIER) != 0)
                throw;
        }
        else if (ctx->current->type == TK_KEYWORD_CONTINUE)
        {
            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD_BREAK)
        {
            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD_THROW)
        {
            if (ctx->p_current_try_statement_opt == NULL)
            {

                compiler_diagnostic_message(C_ERROR_THROW_STATEMENT_NOT_WITHIN_TRY_BLOCK, ctx, ctx->current, "throw statement not within try block");
            }
            else
            {
                p_jump_statement->try_catch_block_index = ctx->p_current_try_statement_opt->try_catch_block_index;
            }

            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD_RETURN)
        {
            const struct token* const p_return_token = ctx->current;
            parser_match(ctx);

            if (ctx->current->type != ';')
            {
                p_jump_statement->expression_opt = expression(ctx);

                if (p_jump_statement->expression_opt)
                {
                    /*
                     * Check is return type is compatible with function return
                     */
                    struct type return_type =
                        get_function_return_type(&ctx->p_current_function_opt->init_declarator_list.head->p_declarator->type);

                    if (type_is_void(&return_type))
                    {
                        compiler_diagnostic_message(C_ERROR_VOID_FUNCTION_SHOULD_NOT_RETURN_VALUE,
                            ctx,
                            p_return_token,
                            "void function '%s' should not return a value",
                            ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name->lexeme);
                    }
                    else
                    {
                        check_assigment(ctx,
                            &return_type,
                            p_jump_statement->expression_opt,
                            ASSIGMENT_TYPE_RETURN);
                    }

                    type_destroy(&return_type);
                }
            }
        }
        else
        {
            assert(false);
        }
        p_jump_statement->last_token = ctx->current;
        if (parser_match_tk(ctx, ';') != 0)
            throw;
    }
    catch
    {
        jump_statement_delete(p_jump_statement);
        p_jump_statement = NULL;
    }
    return p_jump_statement;
}

void expression_statement_delete(struct expression_statement* owner opt p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        expression_delete(p->expression_opt);
        free(p);
    }
}

struct expression_statement* owner expression_statement(struct parser_ctx* ctx, bool ignore_semicolon)
{
    struct expression_statement* owner p_expression_statement = calloc(1, sizeof(struct expression_statement));
    try
    {
        /*
         expression-statement:
           expression opt ;
           attribute-specifier-sequence expression ;
        */
        if (p_expression_statement == NULL)
            throw;

        p_expression_statement->p_attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);

        if (ctx->current->type != ';')
        {
            p_expression_statement->expression_opt = expression(ctx);
            if (p_expression_statement->expression_opt == NULL)
                throw;
        }

        if (!ignore_semicolon && parser_match_tk(ctx, ';') != 0)
            throw;
    }
    catch
    {
        expression_statement_delete(p_expression_statement);
        p_expression_statement = NULL;
    }
    return p_expression_statement;
}

void declaration_list_add(struct declaration_list* list, struct declaration* owner p_declaration)
{
    if (list->head == NULL)
    {
        list->head = p_declaration;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_declaration;
    }
    list->tail = p_declaration;
}

void simple_declaration_delete(struct simple_declaration* owner opt p_simple_declaration)
{
    if (p_simple_declaration)
    {
        declaration_specifiers_delete(p_simple_declaration->p_declaration_specifiers);
        init_declarator_list_destroy(&p_simple_declaration->init_declarator_list);
        attribute_specifier_sequence_delete(p_simple_declaration->p_attribute_specifier_sequence_opt);

        free(p_simple_declaration);
    }
}



void condition_delete(struct condition* owner opt p_condition)
{
    if (p_condition)
    {
        init_declarator_delete(p_condition->p_init_declarator);
        expression_delete(p_condition->expression);

        attribute_specifier_sequence_delete(p_condition->p_attribute_specifier_sequence_opt);
        declaration_specifiers_delete(p_condition->p_declaration_specifiers);
        free(p_condition);
    }
}

struct condition* owner condition(struct parser_ctx* ctx)
{

    /*
    condition :
       expression
       attribute-specifier-seq opt decl-specifier-seq declarator initializer
    */
    struct condition* owner p_condition = calloc(1, sizeof * p_condition);
    try
    {
        if (p_condition == NULL) throw;
        p_condition->first_token = ctx->current;
        if (first_of_declaration_specifier(ctx))
        {
            p_condition->p_attribute_specifier_sequence_opt = attribute_specifier_sequence(ctx);
            p_condition->p_declaration_specifiers = declaration_specifiers(ctx, STORAGE_SPECIFIER_AUTOMATIC_STORAGE);
            p_condition->p_init_declarator = init_declarator(ctx, p_condition->p_declaration_specifiers);
        }
        else
        {
            p_condition->expression = expression(ctx);
            if (p_condition->expression == NULL) throw;
        }
        p_condition->last_token = previous_parser_token(ctx->current);
    }
    catch
    {
        condition_delete(p_condition);
        p_condition = NULL;
    }
    return p_condition;
}

void init_statement_delete(struct init_statement* owner opt p_init_statement)
{
    if (p_init_statement)
    {
        expression_statement_delete(p_init_statement->p_expression_statement);
        simple_declaration_delete(p_init_statement->p_simple_declaration);
        free(p_init_statement);
    }
}

struct init_statement* owner init_statement(struct parser_ctx* ctx, bool ignore_semicolon)
{
    /*
       expression-statement
       simple-declaration
    */
    struct init_statement* owner p_init_statement = calloc(1, sizeof * p_init_statement);
    try
    {
        if (p_init_statement == NULL) throw;
        if (first_of_declaration_specifier(ctx))
        {
            p_init_statement->p_simple_declaration = simple_declaration(ctx, NULL, ignore_semicolon);
        }
        else
        {
            p_init_statement->p_expression_statement = expression_statement(ctx, ignore_semicolon);
        }
    }
    catch
    {
        init_statement_delete(p_init_statement);
        p_init_statement = NULL;
    }
    return p_init_statement;
}

void declaration_delete(struct declaration* owner opt p)
{
    if (p)
    {

        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        static_assert_declaration_delete(p->static_assert_declaration);

        declaration_specifiers_delete(p->declaration_specifiers);

        compound_statement_delete(p->function_body);
        pragma_declaration_delete(p->pragma_declaration);

        init_declarator_list_destroy(&p->init_declarator_list);
        assert(p->next == NULL);
        free(p);
    }
}

void declaration_list_destroy(struct declaration_list* obj_owner list)
{
    struct declaration* owner p = list->head;
    while (p)
    {
        struct declaration* owner next = p->next;
        p->next = NULL;
        declaration_delete(p);
        p = next;
    }
}

struct declaration_list translation_unit(struct parser_ctx* ctx, bool* berror)
{
    *berror = false;
    struct declaration_list declaration_list = { 0 };
    /*
      translation_unit:
      external_declaration
      translation_unit external_declaration
    */
    try
    {
        while (ctx->current != NULL)
        {
            struct declaration* owner p = external_declaration(ctx);
            if (p == NULL)
                throw;
            declaration_list_add(&declaration_list, p);
        }
    }
    catch
    {
        *berror = true;
    }
    return declaration_list;
}

struct declaration* owner external_declaration(struct parser_ctx* ctx)
{
    /*
     function_definition
     declaration
     */
    return function_definition_or_declaration(ctx);
}

struct compound_statement* owner function_body(struct parser_ctx* ctx)
{

    /*
     * Used to give an unique index (inside the function)
     * for try-catch blocks
     */
    ctx->try_catch_block_index = 0;
    ctx->p_current_try_statement_opt = NULL;
    return compound_statement(ctx);
}

static void show_unused_file_scope(struct parser_ctx* ctx)
{

    for (int i = 0; i < ctx->scopes.head->variables.capacity; i++)
    {
        if (ctx->scopes.head->variables.table == NULL)
            continue;
        struct map_entry* entry = ctx->scopes.head->variables.table[i];
        while (entry)
        {

            if (entry->type != TAG_TYPE_ONLY_DECLARATOR &&
                entry->type != TAG_TYPE_INIT_DECLARATOR)
            {
                entry = entry->next;
                continue;
            }

            struct declarator* p_declarator = NULL;
            struct init_declarator* p_init_declarator = NULL;
            if (entry->type == TAG_TYPE_INIT_DECLARATOR)
            {
                p_init_declarator = entry->p;
                p_declarator = p_init_declarator->p_declarator;
            }
            else
            {
                p_declarator = entry->p;
            }

            if (p_declarator &&
                p_declarator->first_token &&
                p_declarator->first_token->level == 0 &&
                declarator_is_function(p_declarator) &&
                (p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC))
            {
                if (!type_is_maybe_unused(&p_declarator->type) &&
                    p_declarator->num_uses == 0)
                {
                    compiler_diagnostic_message(W_UNUSED_VARIABLE,
                        ctx,
                        p_declarator->name,
                        "declarator '%s' not used", p_declarator->name->lexeme);
                }
            }

            entry = entry->next;
        }
    }
}

struct declaration_list parse(struct parser_ctx* ctx, struct token_list* list, bool* berror)
{
    *berror = false;
    s_anonymous_struct_count = 0;
    struct declaration_list l = { 0 };
    struct scope file_scope = { 0 };
    try
    {
        scope_list_push(&ctx->scopes, &file_scope);
        ctx->input_list = *list;
        ctx->current = ctx->input_list.head;
        parser_skip_blanks(ctx);

        bool local_error = false;
        l = translation_unit(ctx, &local_error);
        if (local_error)
            throw;
        show_unused_file_scope(ctx); // cannot be executed on error becase scope have dangling pointers
    }
    catch
    {
        *berror = true;
    }
    scope_destroy(&file_scope);

    return l;
}

int fill_preprocessor_options(int argc, const char** argv, struct preprocessor_ctx* prectx)
{
    /*first loop used to collect options*/
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            continue;

        if (argv[i][1] == 'I')
        {
            include_dir_add(&prectx->include_dir, argv[i] + 2);
            continue;
        }
        if (argv[i][1] == 'D')
        {
            char buffer[200] = { 0 };
            snprintf(buffer, sizeof buffer, "#define %s \n", argv[i] + 2);

            /*TODO make it more precise*/
            char* p = &buffer[7];
            while (*p)
            {
                if (*p == '=')
                {
                    *p = ' ';
                    break;
                }
                p++;
            }

            struct tokenizer_ctx tctx = { 0 };
            struct token_list l1 = tokenizer(&tctx, buffer, "", 0, TK_FLAG_NONE);
            struct token_list r = preprocessor(prectx, &l1, 0);
            token_list_destroy(&l1);
            token_list_destroy(&r);
            continue;
        }
    }
    return 0;
}

#ifdef _WIN32
unsigned long __stdcall GetEnvironmentVariableA(
    const char* lpname,
    char* lpbuffer,
    unsigned long nsize);
#else

unsigned long GetEnvironmentVariableA(
    const char* lpname,
    char* lpbuffer,
    unsigned long nsize)
{
    return 0;
}
#endif

void append_msvc_include_dir(struct preprocessor_ctx* prectx)
{

#ifdef _WIN32
    char env[2000] = { 0 };
    int n = GetEnvironmentVariableA("INCLUDE", env, sizeof(env));

    if (n > 0)
    {

        const char* p = env;
        for (;;)
        {
            if (*p == '\0')
            {
                break;
            }
            char filename_local[500] = { 0 };
            int count = 0;
            while (*p != '\0' && (*p != ';' && *p != '\n'))
            {
                filename_local[count] = *p;
                p++;
                count++;
            }
            filename_local[count] = 0;
            if (count > 0)
            {
                strcat(filename_local, "/");
                include_dir_add(&prectx->include_dir, filename_local);
            }
            if (*p == '\0')
            {
                break;
            }
            p++;
        }
    }
#endif
        }

const char* owner format_code(struct options* options, const char* content)
{
    struct ast ast = { 0 };
    const char* owner s = NULL;

    struct preprocessor_ctx prectx = { 0 };

    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx);

    struct report report = { 0 };
    struct parser_ctx ctx = { 0 };
    ctx.options = *options;
    ctx.p_report = &report;
    struct tokenizer_ctx tctx = { 0 };
    struct token_list tokens = { 0 };

    try
    {
        prectx.options = *options;
        append_msvc_include_dir(&prectx);

        tokens = tokenizer(&tctx, content, "", 0, TK_FLAG_NONE);
        ast.token_list = preprocessor(&prectx, &tokens, 0);
        if (prectx.n_errors != 0)
            throw;

        bool berror = false;
        ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
        if (berror || report.error_count > 0)
            throw;

        struct format_visit_ctx visit_ctx = { 0 };
        visit_ctx.ast = ast;
        format_visit(&visit_ctx);

        if (options->direct_compilation)
            s = get_code_as_compiler_see(&visit_ctx.ast.token_list);
        else
            s = get_code_as_we_see(&visit_ctx.ast.token_list, options->remove_comments);
    }
    catch
    {
    }

    token_list_destroy(&tokens);

    parser_ctx_destroy(&ctx);
    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);
    return s;
}

void ast_format_visit(struct ast* ast)
{
    /*format input source before transformation*/
    struct format_visit_ctx visit_ctx = { 0 };
    visit_ctx.ast = *ast;
    format_visit(&visit_ctx);
}

void c_visit(struct ast* ast)
{}

int compile_one_file(const char* file_name,
    struct options* options,
    const char* out_file_name,
    int argc,
    const char** argv,
    struct report* report)
{
    printf("%s\n", file_name);
    struct preprocessor_ctx prectx = { 0 };

    prectx.macros.capacity = 5000;

    add_standard_macros(&prectx);

    include_config_header(&prectx);
    // print_all_macros(&prectx);

    struct ast ast = { 0 };

    const char* owner s = NULL;

    struct parser_ctx ctx = { 0 };
    struct visit_ctx visit_ctx = { 0 };
    struct tokenizer_ctx tctx = { 0 };
    struct token_list tokens = { 0 };

    ctx.options = *options;
    ctx.p_report = report;
    char* owner content = NULL;

    try
    {
        if (fill_preprocessor_options(argc, argv, &prectx) != 0)
        {
            throw;
        }

        prectx.options = *options;
        append_msvc_include_dir(&prectx);

        content = read_file(file_name);
        if (content == NULL)
        {
            report->error_count++;
            printf("file not found '%s'\n", file_name);
            throw;
        }

        if (options->sarif_output)
        {
            char sarif_file_name[260] = { 0 };
            strcpy(sarif_file_name, file_name);
            strcat(sarif_file_name, ".sarif");
            ctx.sarif_file = (FILE * owner) fopen(sarif_file_name, "w");
            if (ctx.sarif_file)
            {
                const char* begin_sarif =
                    "{\n"
                    "  \"version\": \"2.1.0\",\n"
                    //"  \"$schema\": \"https://raw.githubusercontent.com/oasis-tcs/sarif-spec/master/Schemata/sarif-schema-2.1.0.json\",\n"
                    "  \"$schema\": \"https://schemastore.azurewebsites.net/schemas/json/sarif-2.1.0-rtm.5.json\",\n"
                    "  \"runs\": [\n"
                    "    {\n"
                    "      \"results\": [\n"
                    "\n";

                fprintf(ctx.sarif_file, "%s", begin_sarif);
            }
            else
            {
                report->error_count++;
                printf("cannot open sarif output file '%s'\n", sarif_file_name);
                throw;
            }
        }

        tokens = tokenizer(&tctx, content, file_name, 0, TK_FLAG_NONE);

        if (options->dump_tokens)
        {
            print_tokens(tokens.head);
        }

        ast.token_list = preprocessor(&prectx, &tokens, 0);
        if (prectx.n_errors > 0)
            throw;

        if (options->dump_pptokens)
        {
            print_tokens(ast.token_list.head);
        }

        if (options->preprocess_only)
        {
            const char* owner s2 = print_preprocessed_to_string2(ast.token_list.head);
            printf("%s", s2);
            free((void* owner)s2);
        }
        else
        {
            bool berror = false;
            ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
            if (berror || report->error_count > 0)
                throw;

            // ast_wasm_visit(&ast);

            if (!options->no_output)
            {
                if (options->format_input)
                {
                    struct format_visit_ctx f = { .ast = ast, .indentation = 4 };
                    format_visit(&f);
                }

                visit_ctx.target = options->target;
                visit_ctx.hide_non_used_declarations = options->direct_compilation;

                visit_ctx.ast = ast;
                visit(&visit_ctx);

                if (options->direct_compilation)
                    s = get_code_as_compiler_see(&visit_ctx.ast.token_list);
                else
                    s = get_code_as_we_see(&visit_ctx.ast.token_list, options->remove_comments);

                if (options->format_ouput)
                {
                    /*re-parser ouput and format*/
                    const char* owner s2 = format_code(options, s);
                    free((void* owner)s);
                    s = s2;
                }

                FILE* owner outfile = fopen(out_file_name, "w");
                if (outfile)
                {
                    if (s)
                        fprintf(outfile, "%s", s);

                    fclose(outfile);
                    // printf("%-30s ", path);
                }
                else
                {
                    report->error_count++;
                    printf("cannot open output file '%s' - %s\n", out_file_name, get_posix_error_message(errno));
                    throw;
                }
            }
        }

        if (ctx.sarif_file)
        {

#define END                                                             \
    "      ],\n"                                                        \
    "      \"tool\": {\n"                                               \
    "        \"driver\": {\n"                                           \
    "          \"name\": \"cake\",\n"                                   \
    "          \"fullName\": \"cake code analysis\",\n"                 \
    "          \"version\": \"0.5\",\n"                                 \
    "          \"informationUri\": \"https://github.com/cake-build\"\n" \
    "        }\n"                                                       \
    "      }\n"                                                         \
    "    }\n"                                                           \
    "  ]\n"                                                             \
    "}\n"                                                               \
    "\n"
            fprintf(ctx.sarif_file, "%s", END);
            fclose(ctx.sarif_file);
            ctx.sarif_file = NULL;
        }
    }
    catch
    {
        // printf("Error %s\n", error->message);
    }

    if (ctx.options.test_mode)
    {
        //lets check if the generated file is the expected
        char buf[MYMAX_PATH] = { 0 };
        snprintf(buf, sizeof buf, "%s.txt", file_name);
        char* owner content_expected = read_file(buf);
        if (content_expected)
        {
            if (s && strcmp(content_expected, s) != 0)
            {
                printf("diferent");
                report->error_count++;
            }
            free(content_expected);
        }

        if (report->error_count > 0 || report->warnings_count > 0)
        {
            printf("-------------------------------------------\n");
            printf("%s", content);
            printf("\n-------------------------------------------\n");
            printf(LIGHTRED "TEST FAILED" RESET " : error=%d, warnings=%d\n", report->error_count, report->warnings_count);
            printf("\n\n");
            report->test_failed++;
        }
        else
        {
            report->test_succeeded++;
            printf(LIGHTGREEN "TEST OK\n" RESET);
        }
    }

    token_list_destroy(&tokens);
    visit_ctx_destroy(&visit_ctx);
    parser_ctx_destroy(&ctx);
    free((void* owner)s);
    free(content);
    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);

    return report->error_count > 0;
}

int compile_many_files(const char* file_name,
    struct options* options,
    const char* out_file_name,
    int argc,
    const char** argv,
    struct report* report)
{
    const char* const file_name_name = basename(file_name);
    const char* const file_name_extension = strrchr(file_name_name, '.');

    int num_files = 0;

    char path[MYMAX_PATH] = { 0 };
    snprintf(path, sizeof path, "%s", file_name);
    dirname(path);
    DIR* owner dir = opendir(path);

    if (dir == NULL)
    {
        return errno;
    }

    struct dirent* dp;
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            /* skip self and parent */
            continue;
        }

        char fromlocal[257] = { 0 };
        snprintf(fromlocal, sizeof fromlocal, "%s/%s", "", dp->d_name);

        if (dp->d_type & DT_DIR)
        {

        }
        else
        {
            const char* const file_name_iter = basename(dp->d_name);
            const char* const file_extension = strrchr(file_name_iter, '.');

            if (strcmp(file_name_extension, file_extension) == 0)
            {
                //Fixes the output file name replacing the current name
                char out_file_name_final[MYMAX_PATH] = { 0 };
                strcpy(out_file_name_final, out_file_name);
                dirname(out_file_name_final);
                strcat(out_file_name_final, "/");
                strcat(out_file_name_final, file_name_iter);

                char in_file_name_final[MYMAX_PATH] = { 0 };
                strcpy(in_file_name_final, file_name);
                dirname(in_file_name_final);
                strcat(in_file_name_final, "/");
                strcat(in_file_name_final, file_name_iter);


                struct report report_local = { 0 };
                report_local.test_mode = report->test_mode;
                compile_one_file(in_file_name_final,
                                 options,
                                 out_file_name_final,
                                 argc,
                                 argv,
                                 &report_local);


                report->error_count += report_local.error_count;
                report->warnings_count += report_local.warnings_count;
                report->info_count += report_local.info_count;
                report->test_succeeded += report_local.test_succeeded;
                report->test_failed += report_local.test_failed;
                num_files++;
            }
        }
    }

    closedir(dir);
    return num_files;
}

static void longest_common_path(int argc, const char** argv, char root_dir[MYMAX_PATH])
{
    /*
     find the longest common path
    */
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
            continue;

        char fullpath_i[MYMAX_PATH] = { 0 };
        realpath(argv[i], fullpath_i);
        strcpy(root_dir, fullpath_i);
        dirname(root_dir);

        for (int k = 0; k < MYMAX_PATH; k++)
        {
            const char ch = fullpath_i[k];
            for (int j = 2; j < argc; j++)
            {
                if (argv[j][0] == '-')
                    continue;

                char fullpath_j[MYMAX_PATH] = { 0 };
                realpath(argv[j], fullpath_j);
                if (fullpath_j[k] != ch)
                {
                    strncpy(root_dir, fullpath_j, k);
                    root_dir[k] = '\0';
                    dirname(root_dir);
                    goto exit;
                }
            }
            if (ch == '\0')
                break;
        }
    }
exit:;
}

static int create_multiple_paths(const char* root, const char* outdir)
{
    /*
       This function creates all dirs (folder1, forder2 ..) after root
       root   : C:/folder
       outdir : C:/folder/folder1/folder2 ...
    */
#if !defined __EMSCRIPTEN__
    const char* p = outdir + strlen(root) + 1;
    for (;;)
    {
        if (*p != '\0' && *p != '/' && *p != '\\')
        {
            p++;
            continue;
        }

        char temp[MYMAX_PATH] = { 0 };
        strncpy(temp, outdir, p - outdir);

        int er = mkdir(temp, 0777);
        if (er != 0)
        {
            er = errno;
            if (er != EEXIST)
            {
                printf("error creating output folder '%s' - %s\n", temp, get_posix_error_message(er));
                return er;
            }
        }
        if (*p == '\0')
            break;
        p++;
    }
    return 0;
#else
    return -1;
#endif
}

int compile(int argc, const char** argv, struct report* report)
{
    struct options options = { 0 };
    if (fill_options(&options, argc, argv) != 0)
    {
        return 1;
    }


    report->test_mode = options.test_mode;

    clock_t begin_clock = clock();
    int no_files = 0;

    char root_dir[MYMAX_PATH] = { 0 };

    if (!options.no_output)
    {
        longest_common_path(argc, argv, root_dir);
    }

    const int root_dir_len = strlen(root_dir);

    /*second loop to compile each file*/
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
            continue;
        no_files++;
        char output_file[MYMAX_PATH] = { 0 };

        if (!options.no_output)
        {
            if (no_files == 1 && options.output[0] != '\0')
            {
                /*
                   -o outputname
                   works when we compile just one file
                */
                strcat(output_file, options.output);
            }
            else
            {
                char fullpath[MYMAX_PATH] = { 0 };
                realpath(argv[i], fullpath);

                strcpy(output_file, root_dir);
                strcat(output_file, "/out");

                strcat(output_file, fullpath + root_dir_len);

                char outdir[MYMAX_PATH] = { 0 };
                strcpy(outdir, output_file);
                dirname(outdir);
                if (create_multiple_paths(root_dir, outdir) != 0)
                {
                    return 1;
                }
            }
        }

        char fullpath[MYMAX_PATH] = { 0 };
        realpath(argv[i], fullpath);

        const char* file_extension = basename(fullpath);

        if (file_extension[0] == '*')
        {
            no_files--; //does not count *.c 
            no_files += compile_many_files(fullpath, &options, output_file, argc, argv, report);
        }
        else
        {
            compile_one_file(fullpath, &options, output_file, argc, argv, report);
        }
    }

    clock_t end_clock = clock();
    double cpu_time_used = ((double)(end_clock - begin_clock)) / CLOCKS_PER_SEC;
    report->no_files = no_files;
    report->cpu_time_used_sec = cpu_time_used;
    return 0;
}

struct ast get_ast(struct options* options,
    const char* filename,
    const char* source,
    struct report* report)
{
    struct ast ast = { 0 };
    struct tokenizer_ctx tctx = { 0 };

    struct token_list list = tokenizer(&tctx, source, filename, 0, TK_FLAG_NONE);

    struct preprocessor_ctx prectx = { 0 };

    struct parser_ctx ctx = { 0 };
    try
    {
        prectx.options = *options;
        prectx.macros.capacity = 5000;

        add_standard_macros(&prectx);

        ast.token_list = preprocessor(&prectx, &list, 0);
        if (prectx.n_errors != 0)
            throw;

        ctx.options = *options;
        ctx.p_report = report;
        bool berror = false;
        ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
        if (berror)
            throw;
    }
    catch
    {
    }
    parser_ctx_destroy(&ctx);
    token_list_destroy(&list);
    preprocessor_ctx_destroy(&prectx);

    return ast;
}

/*
 * dada uma string s produz o argv modificando a string de entrada
 * return argc
 */
int strtoargv(char* s, int n, const char* argv[/*n*/])
{
    int argvc = 0;
    char* p = s;
    while (*p)
    {
        while (*p == ' ')
            p++;
        if (*p == 0)
            break;
        argv[argvc] = p;
        argvc++;
        while (*p != ' ' && *p != '\0')
            p++;
        if (*p == 0)
            break;
        *p = 0;
        p++;
        if (argvc >= n)
            break; /*nao tem mais lugares*/
    }
    return argvc;
}

const char* owner compile_source(const char* pszoptions, const char* content, struct report* report)
{
    const char* argv[100] = { 0 };
    char string[200] = { 0 };
    snprintf(string, sizeof string, "exepath %s", pszoptions);

    const int argc = strtoargv(string, 10, argv);

    const char* owner s = NULL;

    struct preprocessor_ctx prectx = { 0 };
    struct ast ast = { 0 };
    struct options options = { .input = LANGUAGE_CXX };

    struct visit_ctx visit_ctx = { 0 };
    try
    {
        if (fill_options(&options, argc, argv) != 0)
        {
            throw;
        }

        visit_ctx.target = options.target;
        visit_ctx.hide_non_used_declarations = options.direct_compilation;
        prectx.options = options;
        add_standard_macros(&prectx);

        if (options.preprocess_only)
        {
            struct tokenizer_ctx tctx = { 0 };
            struct token_list tokens = tokenizer(&tctx, content, "c:/main.c", 0, TK_FLAG_NONE);

            struct token_list token_list = preprocessor(&prectx, &tokens, 0);
            if (prectx.n_errors == 0)
            {
                s = print_preprocessed_to_string2(token_list.head);
            }

            token_list_destroy(&tokens);
            token_list_destroy(&token_list);
        }
        else
        {

            ast = get_ast(&options, "c:/main.c", content, report);
            if (report->error_count > 0)
                throw;

            visit_ctx.ast = ast;
            visit(&visit_ctx);

            if (options.direct_compilation)
            {
                s = get_code_as_compiler_see(&visit_ctx.ast.token_list);
            }
            else
            {
                s = get_code_as_we_see(&visit_ctx.ast.token_list, options.remove_comments);
            }
            if (options.format_ouput)
            {

                /*re-parser ouput and format*/
                const char* owner s2 = format_code(&options, s);
                free((void* owner)s);
                s = s2;
            }
        }
    }
    catch
    {
    }

    preprocessor_ctx_destroy(&prectx);
    visit_ctx_destroy(&visit_ctx);
    ast_destroy(&ast);

    return s;
}

char* owner CompileText(const char* pszoptions, const char* content)
{
    /*
      This function is called by the web playground
    */
    printf(WHITE "Cake " CAKE_VERSION RESET "\n");
    printf(WHITE "cake %s main.c\n", pszoptions);
    struct report report = { 0 };
    return (char* owner)compile_source(pszoptions, content, &report);
}

void ast_destroy(struct ast* obj_owner ast)
{
    token_list_destroy(&ast->token_list);
    declaration_list_destroy(&ast->declaration_list);
}

static bool is_all_upper(const char* text)
{
    const char* p = text;
    while (*p)
    {
        if (*p != toupper(*p))
        {
            return false;
        }
        p++;
    }
    return true;
}

static bool is_snake_case(const char* text)
{
    if (text == NULL)
        return true;

    if (!(*text >= 'a' && *text <= 'z'))
    {
        return false;
    }

    while (*text)
    {
        if ((*text >= 'a' && *text <= 'z') ||
            *text == '_' ||
            (*text >= '0' && *text <= '9'))

        {
            // ok
        }
        else
            return false;
        text++;
    }

    return true;
}

static bool is_camel_case(const char* text)
{
    if (text == NULL)
        return true;

    if (!(*text >= 'a' && *text <= 'z'))
    {
        return false;
    }

    while (*text)
    {
        if ((*text >= 'a' && *text <= 'z') ||
            (*text >= 'A' && *text <= 'Z') ||
            (*text >= '0' && *text <= '9'))
        {
            // ok
        }
        else
            return false;
        text++;
    }

    return true;
}

static bool is_pascal_case(const char* text)
{
    if (text == NULL)
        return true;

    if (!(text[0] >= 'A' && text[0] <= 'Z'))
    {
        /*first letter uppepr case*/
        return false;
    }

    while (*text)
    {
        if ((*text >= 'a' && *text <= 'z') ||
            (*text >= 'A' && *text <= 'Z') ||
            (*text >= '0' && *text <= '9'))
        {
            // ok
        }
        else
            return false;
        text++;
    }

    return true;
}

/*
 * This naming conventions are not ready yet...
 * but not dificult to implement.maybe options to choose style
 */
void naming_convention_struct_tag(struct parser_ctx* ctx, struct token* token)
{
    if (!parser_is_diagnostic_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (ctx->options.style == STYLE_CAKE)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(W_STYLE, ctx, token, "use snake_case for struct/union tags");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_pascal_case(token->lexeme))
        {
            compiler_diagnostic_message(W_STYLE, ctx, token, "use camelCase for struct/union tags");
        }
    }
}

void naming_convention_enum_tag(struct parser_ctx* ctx, struct token* token)
{
    if (!parser_is_diagnostic_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (ctx->options.style == STYLE_CAKE)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(W_STYLE, ctx, token, "use snake_case for enum tags");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_pascal_case(token->lexeme))
        {
            compiler_diagnostic_message(W_STYLE, ctx, token, "use PascalCase for enum tags");
        }
    }
}

void naming_convention_function(struct parser_ctx* ctx, struct token* token)
{
    if (token == NULL)
        return;

    if (!parser_is_diagnostic_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (ctx->options.style == STYLE_CAKE)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(W_STYLE, ctx, token, "use snake_case for functions");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_pascal_case(token->lexeme))
        {
            compiler_diagnostic_message(W_STYLE, ctx, token, "use PascalCase for functions");
        }
    }
}

void naming_convention_global_var(struct parser_ctx* ctx, struct token* token, struct type* type, enum storage_class_specifier_flags storage)
{
    if (!parser_is_diagnostic_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (!type_is_function_or_function_pointer(type))
    {
        if (storage & STORAGE_SPECIFIER_STATIC)
        {
            if (token->lexeme[0] != 's' || token->lexeme[1] != '_')
            {
                compiler_diagnostic_message(W_STYLE, ctx, token, "use prefix s_ for static global variables");
            }
        }
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(W_STYLE, ctx, token, "use snake_case global variables");
        }
    }
}

void naming_convention_local_var(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!parser_is_diagnostic_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (ctx->options.style == STYLE_CAKE)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(W_STYLE, ctx, token, "use snake_case for local variables");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_camel_case(token->lexeme))
        {
            compiler_diagnostic_message(W_STYLE, ctx, token, "use camelCase for local variables");
        }
    }
}

void naming_convention_enumerator(struct parser_ctx* ctx, struct token* token)
{
    if (!parser_is_diagnostic_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (!is_all_upper(token->lexeme))
    {
        compiler_diagnostic_message(W_STYLE, ctx, token, "use UPPERCASE for enumerators");
    }
}

void naming_convention_struct_member(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!parser_is_diagnostic_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (!is_snake_case(token->lexeme))
    {
        compiler_diagnostic_message(W_STYLE, ctx, token, "use snake_case for struct members");
    }
}

void naming_convention_parameter(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!parser_is_diagnostic_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (!is_snake_case(token->lexeme))
    {
        compiler_diagnostic_message(W_STYLE, ctx, token, "use snake_case for arguments");
    }
}
