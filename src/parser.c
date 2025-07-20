/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

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
#include "flow.h"
#include "visit_defer.h"
#include <errno.h>

#ifdef _WIN32
#include <Windows.h>
#endif
#include "version.h"
#include "object.h"

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif

#include "visit_il.h"
#include <time.h>

#ifdef PATH_MAX
#define MYMAX_PATH PATH_MAX // Linux uses it in realpath
#else
#define MYMAX_PATH MAX_PATH
#endif

NODISCARD
int initializer_init_new(struct parser_ctx* ctx,
                       struct type* p_type, /*in (in/out for arrays [])*/
                        struct object* object, /*in (in/out for arrays [])*/
                        struct initializer* initializer, /*rtocar para initializer item??*/
                        bool is_constant,
                        bool requires_constant_initialization);

struct defer_statement* _Owner _Opt defer_statement(struct parser_ctx* ctx);

void defer_statement_delete(struct defer_statement* _Owner _Opt p)
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

    return ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors & w) != 0) ||
        ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings & w) != 0) ||
        ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes & w) != 0);
}

static void check_open_brace_style(struct parser_ctx* ctx, struct token* token)
{
    // token points to {

    if (token->level == 0 &&
        !(token->flags & TK_FLAG_MACRO_EXPANDED) &&
        token->type == '{' &&
        token->prev &&
        parser_is_diagnostic_enabled(ctx, W_STYLE))
    {
        if (ctx->options.style == STYLE_CAKE)
        {
            if (token->prev->type == TK_BLANKS &&
                token->prev->prev &&
                token->prev->prev->type == TK_NEWLINE)
            {
            }
            else
            {
                compiler_diagnostic(W_STYLE, ctx, token, NULL, "not following correct brace style {");
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
        token->prev &&
        token->prev->prev &&
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
                compiler_diagnostic(W_STYLE, ctx, token, NULL, "not following correct close brace style }");
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
        token->prev &&
        parser_is_diagnostic_enabled(ctx, W_STYLE))
    {
        if (ctx->options.style == STYLE_CAKE)
        {
            if (token->prev->type == TK_NEWLINE)
            {
            }
            else
            {
                compiler_diagnostic(W_STYLE, ctx, token, NULL, "not following correct brace style {");
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
                compiler_diagnostic(W_STYLE, ctx, token, "not following correct close brace style }");
            }
        }
    }
}
*/


void scope_destroy(_Dtor struct scope* p)
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
        assert(list->tail != NULL);
        pnew->previous = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
}

void scope_list_pop(struct scope_list* list)
{

    if (list->head == NULL)
        return;
    assert(list->tail != NULL);
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
            assert(list->tail != NULL);
            list->tail->next = NULL;
            list->tail->previous = NULL;
        }
    }
    p->next = NULL;
    p->previous = NULL;
}

void parser_ctx_destroy(_Opt _Dtor struct parser_ctx* ctx)
{
    label_list_clear(&ctx->label_list);
    assert(ctx->label_list.head == NULL);
    assert(ctx->label_list.tail == NULL);

    if (ctx->sarif_file)
    {
        fclose(ctx->sarif_file);
    }
}

static void stringfy(const char* input, char* json_str_message, int output_size)
{
    json_str_message[0] = '\0'; //out

    int k = 0;
    while (*input != '\0')
    {
        if (*input == '\"')
        {
            if (k < output_size)
                json_str_message[k] = '\\';
            k++;
            if (k < output_size)
                json_str_message[k] = '"';
            k++;
            input++;
        }
        else if (*input == '\n')
        {
            if (k < output_size)
                json_str_message[k] = '\\';
            k++;
            if (k < output_size)
                json_str_message[k] = 'n';
            k++;
            input++;
        }
        else
        {
            if (k < output_size)
                json_str_message[k] = *input;
            k++;
            input++;
        }
    }
    if (k < output_size)
        json_str_message[k] = '\0';
    else
        json_str_message[output_size - 1] = '\0';
}

_Bool compiler_diagnostic(enum diagnostic_id w,
    const struct parser_ctx* ctx,
    const struct token* _Opt p_token_opt,
    const struct marker* _Opt p_marker_temp,
    const char* fmt, ...)
{
    bool included_file_location = false;
    struct marker marker = { 0 };
    if (p_marker_temp == NULL)
    {
        if (p_token_opt == NULL) return false;

        marker.file = p_token_opt->token_origin->lexeme;
        marker.line = p_token_opt->line;
        marker.start_col = p_token_opt->col;
        marker.end_col = p_token_opt->col;
        marker.p_token_caret = p_token_opt;
        included_file_location = p_token_opt->level > 0;
    }
    else
    {
        //assert(p_token_opt == NULL);
        marker = *p_marker_temp;
        if (marker.p_token_caret)
            p_token_opt = marker.p_token_caret;
        else if (marker.p_token_begin)
            p_token_opt = marker.p_token_begin;

        if (p_token_opt == NULL) return false;
        marker.file = p_token_opt->token_origin->lexeme;
        included_file_location = p_token_opt->level > 0;

        marker.line = p_token_opt->line;
        marker.start_col = p_token_opt->col;
        marker.end_col = p_token_opt->col;
    }

    bool is_error = false;
    bool is_warning = false;
    bool is_note = false;

    if (is_diagnostic_configurable(w))
    {
        is_error =
            (ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors & (1ULL << w)) != 0;

        is_warning =
            (ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings & (1ULL << w)) != 0;

        is_note =
            ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes & (1ULL << w)) != 0);
    }
    else
    {
        is_note = is_diagnostic_note(w);
        is_error = is_diagnostic_error(w);
        is_warning = is_diagnostic_warning(w);
    }

    if (is_error)
    {
        ctx->p_report->error_count++;
    }
    else if (is_warning)
    {
        /*warnings inside headers are ignored*/
        if (included_file_location)
        {
            return false;
        }

        ctx->p_report->warnings_count++;
    }
    else if (is_note)
    {
        /*notes inside headers are ignored*/
        if (included_file_location)
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
    {
        //index 0 is the most recent
        ctx->p_report->last_diagnostics_ids[1] = ctx->p_report->last_diagnostics_ids[0];
        ctx->p_report->last_diagnostics_ids[0] = w;
    }

    const char* func_name = "module";
    if (ctx->p_current_function_opt)
    {
        if (ctx->p_current_function_opt->name_opt)
            func_name = ctx->p_current_function_opt->name_opt->lexeme;
        else
            func_name = "unnamed";
    }

    char buffer[200] = { 0 };

    char diagnostic_name[100] = { 0 };
    get_warning_name(w, sizeof diagnostic_name, diagnostic_name);




    print_position(marker.file, marker.line, marker.start_col, ctx->options.visual_studio_ouput_format);

#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wnullable-to-non-nullable"
#pragma CAKE diagnostic ignored "-Wanalyzer-null-dereference"

    va_list args = { 0 };
    va_start(args, fmt);
    /*int n =*/vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

#pragma CAKE diagnostic pop

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
            if (w == W_LOCATION)
                printf(LIGHTCYAN "note: " WHITE "%s\n" RESET, buffer);
            else
                printf(LIGHTCYAN "note: " WHITE "%s [" LIGHTCYAN "%s" WHITE "]\n" RESET, buffer, diagnostic_name);
        }
    }

    print_line_and_token(&marker, ctx->options.visual_studio_ouput_format);


    if (ctx->sarif_file)
    {

        char json_str_message[200] = { 0 };
        stringfy(buffer, json_str_message, sizeof json_str_message);

        if (ctx->sarif_entries > 0)
        {
            fprintf(ctx->sarif_file, "   ,\n");
        }

        ((struct parser_ctx*)ctx)->sarif_entries++;

        fprintf(ctx->sarif_file, "   {\n");
        fprintf(ctx->sarif_file, "     \"ruleId\":\"%s\",\n", diagnostic_name);

        if (is_error)
            fprintf(ctx->sarif_file, "     \"level\":\"error\",\n");
        else if (is_warning)
            fprintf(ctx->sarif_file, "     \"level\":\"warning\",\n");
        else if (is_note)
            fprintf(ctx->sarif_file, "     \"level\":\"note\",\n");

        fprintf(ctx->sarif_file, "     \"message\": {\n");
        fprintf(ctx->sarif_file, "            \"text\": \"%s\"\n", json_str_message);
        fprintf(ctx->sarif_file, "      },\n");
        fprintf(ctx->sarif_file, "      \"locations\": [\n");
        fprintf(ctx->sarif_file, "       {\n");

        fprintf(ctx->sarif_file, "       \"physicalLocation\": {\n");

        fprintf(ctx->sarif_file, "             \"artifactLocation\": {\n");
        fprintf(ctx->sarif_file, "                 \"uri\": \"file:///%s\"\n", marker.file);
        fprintf(ctx->sarif_file, "              },\n");

        fprintf(ctx->sarif_file, "              \"region\": {\n");
        fprintf(ctx->sarif_file, "                  \"startLine\": %d,\n", marker.line);
        fprintf(ctx->sarif_file, "                  \"startColumn\": %d,\n", marker.start_col);
        fprintf(ctx->sarif_file, "                  \"endLine\": %d,\n", marker.line);
        fprintf(ctx->sarif_file, "                  \"endColumn\": %d\n", marker.end_col);
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
    struct scope* _Opt p = e->head;
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


bool first_of_function_specifier(const struct parser_ctx* ctx)
{
    struct token* _Opt token = ctx->current;

    if (token == NULL)
        return false;

    return token->type == TK_KEYWORD_INLINE ||
        token->type == TK_KEYWORD__NORETURN;
}

bool first_of_enum_specifier_token(const struct token* token)
{
    return token->type == TK_KEYWORD_ENUM;
}

bool first_of_enum_specifier(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return first_of_enum_specifier_token(ctx->current);
}


bool first_of_alignment_specifier(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;
    return ctx->current->type == TK_KEYWORD__ALIGNAS;
}

bool first_of_atomic_type_specifier(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    /*
      If the _Atomic keyword is immediately followed by a left parenthesis, it is interpreted
      as a type specifier (with a type name), not as a type qualifier.
    */

    if (ctx->current->type == TK_KEYWORD__ATOMIC)
    {
        struct token* _Opt ahead = parser_look_ahead(ctx);
        if (ahead != NULL)
        {
            return ahead->type == '(';
        }
    }
    return false;
}

bool first_of_storage_class_specifier(const struct parser_ctx* ctx)
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

bool first_of_struct_or_union_token(const struct token* token)
{
    return token->type == TK_KEYWORD_STRUCT || token->type == TK_KEYWORD_UNION;
}

bool first_of_struct_or_union(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return first_of_struct_or_union_token(ctx->current);
}

bool first_of_type_qualifier_token(const struct token* p_token)
{

    return p_token->type == TK_KEYWORD_CONST ||
        p_token->type == TK_KEYWORD_RESTRICT ||
        p_token->type == TK_KEYWORD_VOLATILE ||
        p_token->type == TK_KEYWORD__ATOMIC ||

        /*extensions*/
        p_token->type == TK_KEYWORD__CTOR ||
        p_token->type == TK_KEYWORD__OWNER ||
        p_token->type == TK_KEYWORD__DTOR ||
        p_token->type == TK_KEYWORD__VIEW ||
        p_token->type == TK_KEYWORD__OPT;

    //__fastcall
    //__stdcall
}

bool first_of_type_qualifier(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return first_of_type_qualifier_token(ctx->current);
}

struct map_entry* _Opt find_tag(struct parser_ctx* ctx, const char* lexeme)
{
    struct scope* _Opt scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry* _Opt p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry)
        {
            return p_entry;
        }
        scope = scope->previous;
    }
    return NULL;
}

struct map_entry* _Opt find_variables(const struct parser_ctx* ctx, const char* lexeme, struct scope* _Opt* _Opt ppscope_opt)
{
    if (ppscope_opt != NULL)
        *ppscope_opt = NULL; // out

    struct scope* _Opt scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry* _Opt p_entry = hashmap_find(&scope->variables, lexeme);
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

struct enum_specifier* _Opt find_enum_specifier(struct parser_ctx* ctx, const char* lexeme)
{
    struct enum_specifier* _Opt best = NULL;
    struct scope* _Opt scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry* _Opt p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry &&
            p_entry->type == TAG_TYPE_ENUN_SPECIFIER)
        {
            assert(p_entry->data.p_enum_specifier != NULL);

            best = p_entry->data.p_enum_specifier;
            if (best->enumerator_list.head != NULL)
                return best; // OK bem completo
            else
            {
                // it's not complete let's keep going up
            }
        }
        scope = scope->previous;
    }
    return best; // mesmo que nao seja tao completo vamos retornar.
}

struct struct_or_union_specifier* _Opt find_struct_or_union_specifier(const struct parser_ctx* ctx, const char* lexeme)
{
    struct struct_or_union_specifier* _Opt p = NULL;
    struct scope* _Opt scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry* _Opt p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry &&
            p_entry->type == TAG_TYPE_STRUCT_OR_UNION_SPECIFIER)
        {
            assert(p_entry->data.p_struct_or_union_specifier != NULL);
            p = p_entry->data.p_struct_or_union_specifier;
            break;
        }
        scope = scope->previous;
    }
    return p;
}

struct declarator* _Opt find_declarator(const struct parser_ctx* ctx, const char* lexeme, struct scope** _Opt ppscope_opt)
{
    struct map_entry* _Opt p_entry = find_variables(ctx, lexeme, ppscope_opt);

    if (p_entry)
    {
        if (p_entry->type == TAG_TYPE_INIT_DECLARATOR)
        {
            assert(p_entry->data.p_init_declarator != NULL);
            struct init_declarator* p_init_declarator = p_entry->data.p_init_declarator;
            return (struct declarator*)p_init_declarator->p_declarator;
        }
        else if (p_entry->type == TAG_TYPE_DECLARATOR)
        {
            return p_entry->data.p_declarator;
        }
    }

    return NULL;
}

struct enumerator* _Opt find_enumerator(const struct parser_ctx* ctx, const char* lexeme, struct scope** _Opt ppscope_opt)
{
    struct map_entry* _Opt p_entry = find_variables(ctx, lexeme, ppscope_opt);

    if (p_entry && p_entry->type == TAG_TYPE_ENUMERATOR)
        return p_entry->data.p_enumerator;

    return NULL;
}

bool first_of_typedef_name(const struct parser_ctx* ctx, struct token* p_token)
{

    if (p_token->type != TK_IDENTIFIER)
    {
        // no need to check
        return false;
    }
    if (p_token->flags & TK_FLAG_IDENTIFIER_IS_TYPEDEF)
    {
        // it has already been verified that it is a typedef
        return true;
    }
    if (p_token->flags & TK_FLAG_IDENTIFIER_IS_NOT_TYPEDEF)
    {
        // it has already been verified that it is NOT a typedef
        return false;
    }

    struct declarator* _Opt p_declarator = find_declarator(ctx, p_token->lexeme, NULL);

    if (p_declarator &&
        p_declarator->declaration_specifiers &&
        (p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF))
    {
        p_declarator->num_uses++;
        p_token->flags |= TK_FLAG_IDENTIFIER_IS_TYPEDEF;
        return true;
    }
    else
    {
        p_token->flags |= TK_FLAG_IDENTIFIER_IS_NOT_TYPEDEF;
    }
    return false;
}

bool first_of_type_specifier(const struct parser_ctx* ctx);
bool first_of_type_specifier_token(const struct parser_ctx* ctx, struct token* token);

bool first_of_type_name_ahead(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    if (ctx->current->type != '(')
        return false;

    struct token* _Opt token_ahead = parser_look_ahead(ctx);

    if (token_ahead == NULL)
        return false;

    return first_of_type_specifier_token(ctx, token_ahead) ||
        first_of_type_qualifier_token(token_ahead);
}

bool first_of_type_name(const struct parser_ctx* ctx)
{
    return first_of_type_specifier(ctx) || first_of_type_qualifier(ctx);
}

bool first_of_type_specifier_token(const struct parser_ctx* ctx, struct token* p_token)
{
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
        p_token->type == TK_KEYWORD__BITINT ||
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

bool first_of_type_specifier(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;
    return first_of_type_specifier_token(ctx, ctx->current);
}

bool first_of_type_specifier_qualifier(const struct parser_ctx* ctx)
{
    return first_of_type_specifier(ctx) ||
        first_of_type_qualifier(ctx) ||
        first_of_alignment_specifier(ctx);
}

bool first_of_compound_statement(const struct parser_ctx* ctx)
{
    return ctx->current != NULL && ctx->current->type == '{';
}

bool first_of_jump_statement(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD_GOTO ||
        ctx->current->type == TK_KEYWORD_CONTINUE ||
        ctx->current->type == TK_KEYWORD_BREAK ||
        ctx->current->type == TK_KEYWORD_RETURN ||
        ctx->current->type == TK_KEYWORD_THROW /*extension*/;
}

bool first_of_selection_statement(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD_IF ||
        ctx->current->type == TK_KEYWORD_SWITCH;
}

bool first_of_iteration_statement(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD_WHILE ||
        ctx->current->type == TK_KEYWORD_DO ||
        ctx->current->type == TK_KEYWORD_FOR;
}

bool first_of_label(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    if (ctx->current->type == TK_IDENTIFIER)
    {
        struct token* _Opt next = parser_look_ahead(ctx);
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

bool first_of_declaration_specifier(const struct parser_ctx* ctx)
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

bool first_of_pragma_declaration(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_PRAGMA;
}

bool first_of_static_assert_declaration(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD__STATIC_ASSERT ||
        ctx->current->type == TK_KEYWORD_STATIC_DEBUG ||
        ctx->current->type == TK_KEYWORD_STATIC_DEBUG_EX ||
        ctx->current->type == TK_KEYWORD_STATIC_STATE ||
        ctx->current->type == TK_KEYWORD_STATIC_SET;
}

bool first_of_attribute_specifier(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    if (ctx->current->type != '[')
    {
        return false;
    }
    struct token* _Opt p_token = parser_look_ahead(ctx);
    return p_token != NULL && p_token->type == '[';
}

bool first_of_labeled_statement(const struct parser_ctx* ctx)
{
    return first_of_label(ctx);
}

bool first_of_designator(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == '[' || ctx->current->type == '.';
}

struct token* _Opt previous_parser_token(const struct token* token)
{
    if (token->prev == NULL)
    {
        return NULL;
    }

    struct token* _Opt prev = token->prev;
    while (prev && !(prev->flags & TK_FLAG_FINAL))
    {
        prev = prev->prev;
    }

    return prev;
}

enum token_type is_keyword(const char* text)
{
    switch (text[0])
    {
    case 'a':
        if (strcmp("alignof", text) == 0)
            return TK_KEYWORD__ALIGNOF;
        if (strcmp("auto", text) == 0)
            return TK_KEYWORD_AUTO;
        if (strcmp("alignas", text) == 0)
            return TK_KEYWORD__ALIGNAS; /*C23 alternate spelling _Alignas*/
        if (strcmp("alignof", text) == 0)
            return TK_KEYWORD__ALIGNAS; /*C23 alternate spelling _Alignof*/
        if (strcmp("assert", text) == 0)
            return TK_KEYWORD_ASSERT; /*extension*/
        break;

    case 'b':
        if (strcmp("break", text) == 0)
            return TK_KEYWORD_BREAK;
        if (strcmp("bool", text) == 0)
            return TK_KEYWORD__BOOL; /*C23 alternate spelling _Bool*/
        break;

    case 'c':
        if (strcmp("case", text) == 0)
            return TK_KEYWORD_CASE;
        if (strcmp("char", text) == 0)
            return TK_KEYWORD_CHAR;
        if (strcmp("const", text) == 0)
            return TK_KEYWORD_CONST;
        if (strcmp("constexpr", text) == 0)
            return TK_KEYWORD_CONSTEXPR;
        if (strcmp("continue", text) == 0)
            return TK_KEYWORD_CONTINUE;
        if (strcmp("catch", text) == 0)
            return TK_KEYWORD_CATCH;
        break;

    case 'd':
        if (strcmp("default", text) == 0)
            return TK_KEYWORD_DEFAULT;
        if (strcmp("do", text) == 0)
            return TK_KEYWORD_DO;
        if (strcmp("defer", text) == 0)
            return TK_KEYWORD_DEFER;
        if (strcmp("double", text) == 0)
            return TK_KEYWORD_DOUBLE;
        break;

    case 'e':
        if (strcmp("else", text) == 0)
            return TK_KEYWORD_ELSE;
        if (strcmp("enum", text) == 0)
            return TK_KEYWORD_ENUM;
        if (strcmp("extern", text) == 0)
            return TK_KEYWORD_EXTERN;
        break;

    case 'f':
        if (strcmp("float", text) == 0)
            return TK_KEYWORD_FLOAT;
        if (strcmp("for", text) == 0)
            return TK_KEYWORD_FOR;
        if (strcmp("false", text) == 0)
            return TK_KEYWORD_FALSE;
        break;

    case 'g':
        if (strcmp("goto", text) == 0)
            return TK_KEYWORD_GOTO;
        break;

    case 'i':
        if (strcmp("if", text) == 0)
            return TK_KEYWORD_IF;
        if (strcmp("inline", text) == 0)
            return TK_KEYWORD_INLINE;
        if (strcmp("int", text) == 0)
            return TK_KEYWORD_INT;
        break;

    case 'n':
        if (strcmp("nullptr", text) == 0)
            return TK_KEYWORD_NULLPTR;
        break;

    case 'l':
        if (strcmp("long", text) == 0)
            return TK_KEYWORD_LONG;
        break;

    case 'r':
        if (strcmp("register", text) == 0)
            return TK_KEYWORD_REGISTER;
        if (strcmp("restrict", text) == 0)
            return TK_KEYWORD_RESTRICT;
        if (strcmp("return", text) == 0)
            return TK_KEYWORD_RETURN;
        break;

    case 's':
        if (strcmp("short", text) == 0)
            return TK_KEYWORD_SHORT;
        if (strcmp("signed", text) == 0)
            return TK_KEYWORD_SIGNED;
        if (strcmp("sizeof", text) == 0)
            return TK_KEYWORD_SIZEOF;
        if (strcmp("static", text) == 0)
            return TK_KEYWORD_STATIC;
        if (strcmp("struct", text) == 0)
            return TK_KEYWORD_STRUCT;
        if (strcmp("switch", text) == 0)
            return TK_KEYWORD_SWITCH;
        if (strcmp("static_assert", text) == 0)
            return TK_KEYWORD__STATIC_ASSERT; /*C23 alternate spelling _Static_assert*/
        if (strcmp("static_debug", text) == 0)
            return TK_KEYWORD_STATIC_DEBUG;
        if (strcmp("static_debug_ex", text) == 0)
            return TK_KEYWORD_STATIC_DEBUG_EX;
        if (strcmp("static_state", text) == 0)
            return TK_KEYWORD_STATIC_STATE;
        if (strcmp("static_set", text) == 0)
            return TK_KEYWORD_STATIC_SET;
        break;

    case 't':
        if (strcmp("typedef", text) == 0)
            return TK_KEYWORD_TYPEDEF;
        if (strcmp("typeof", text) == 0)
            return TK_KEYWORD_TYPEOF; /*C23*/
        if (strcmp("typeof_unqual", text) == 0)
            return TK_KEYWORD_TYPEOF_UNQUAL; /*C23*/
        if (strcmp("true", text) == 0)
            return TK_KEYWORD_TRUE; /*C23*/
        if (strcmp("thread_local", text) == 0)
            return TK_KEYWORD__THREAD_LOCAL; /*C23 alternate spelling _Thread_local*/
        if (strcmp("try", text) == 0)
            return TK_KEYWORD_TRY;
        if (strcmp("throw", text) == 0)
            return TK_KEYWORD_THROW;
        break;

    case 'u':
        if (strcmp("union", text) == 0)
            return TK_KEYWORD_UNION;
        if (strcmp("unsigned", text) == 0)
            return TK_KEYWORD_UNSIGNED;
        break;

    case 'v':
        if (strcmp("void", text) == 0)
            return TK_KEYWORD_VOID;
        if (strcmp("volatile", text) == 0)
            return TK_KEYWORD_VOLATILE;
        break;

    case 'w':
        if (strcmp("while", text) == 0)
            return TK_KEYWORD_WHILE;
        break;
    case '_':

        /*ownership*/
        if (strcmp("_Ctor", text) == 0)
            return TK_KEYWORD__CTOR; /*extension*/
        if (strcmp("_Owner", text) == 0)
            return TK_KEYWORD__OWNER; /*extension*/
        if (strcmp("_Dtor", text) == 0)
            return TK_KEYWORD__DTOR; /*extension*/
        if (strcmp("_Opt", text) == 0)
            return TK_KEYWORD__OPT; /*extension*/

        if (strcmp("_View", text) == 0)
            return TK_KEYWORD__VIEW; /*extension*/

        if (strcmp("_Countof", text) == 0)
            return TK_KEYWORD__COUNTOF; /*C2Y*/

        /*TRAITS EXTENSION*/
        if (strcmp("_is_lvalue", text) == 0)
            return TK_KEYWORD_IS_LVALUE;
        if (strcmp("_is_const", text) == 0)
            return TK_KEYWORD_IS_CONST;
        if (strcmp("_is_owner", text) == 0)
            return TK_KEYWORD_IS_OWNER;
        if (strcmp("_is_pointer", text) == 0)
            return TK_KEYWORD_IS_POINTER;
        if (strcmp("_is_array", text) == 0)
            return TK_KEYWORD_IS_ARRAY;
        if (strcmp("_is_function", text) == 0)
            return TK_KEYWORD_IS_FUNCTION;
        if (strcmp("_is_arithmetic", text) == 0)
            return TK_KEYWORD_IS_ARITHMETIC;
        if (strcmp("_is_floating_point", text) == 0)
            return TK_KEYWORD_IS_FLOATING_POINT;
        if (strcmp("_is_integral", text) == 0)
            return TK_KEYWORD_IS_INTEGRAL;
        if (strcmp("_is_scalar", text) == 0)
            return TK_KEYWORD_IS_SCALAR;
        /*TRAITS EXTENSION*/

        if (strcmp("_Alignof", text) == 0)
            return TK_KEYWORD__ALIGNOF;
        if (strcmp("_Alignas", text) == 0)
            return TK_KEYWORD__ALIGNAS;
        if (strcmp("_Atomic", text) == 0)
            return TK_KEYWORD__ATOMIC;
        if (strcmp("_Bool", text) == 0)
            return TK_KEYWORD__BOOL;
        if (strcmp("_Complex", text) == 0)
            return TK_KEYWORD__COMPLEX;
        if (strcmp("_Decimal32", text) == 0)
            return TK_KEYWORD__DECIMAL32;
        if (strcmp("_Decimal64", text) == 0)
            return TK_KEYWORD__DECIMAL64;
        if (strcmp("_Decimal128", text) == 0)
            return TK_KEYWORD__DECIMAL128;
        if (strcmp("_Generic", text) == 0)
            return TK_KEYWORD__GENERIC;
        if (strcmp("_Imaginary", text) == 0)
            return TK_KEYWORD__IMAGINARY;
        if (strcmp("_Noreturn", text) == 0)
            return TK_KEYWORD__NORETURN; /*_Noreturn deprecated C23*/
        if (strcmp("_Static_assert", text) == 0)
            return TK_KEYWORD__STATIC_ASSERT;
        if (strcmp("_Thread_local", text) == 0)
            return TK_KEYWORD__THREAD_LOCAL;
        if (strcmp("_BitInt", text) == 0)
            return TK_KEYWORD__BITINT; /*(C23)*/
        if (strcmp("__typeof__", text) == 0)
            return TK_KEYWORD_TYPEOF; /*(C23)*/
#ifdef  _MSC_VER
        // begin microsoft
        if (strcmp("__int8", text) == 0)
            return TK_KEYWORD__INT8;
        if (strcmp("__int16", text) == 0)
            return TK_KEYWORD__INT16;
        if (strcmp("__int32", text) == 0)
            return TK_KEYWORD__INT32;
        if (strcmp("__int64", text) == 0)
            return TK_KEYWORD__INT64;
        if (strcmp("__forceinline", text) == 0)
            return TK_KEYWORD_INLINE;
        if (strcmp("__inline", text) == 0)
            return TK_KEYWORD_INLINE;
        if (strcmp("_asm", text) == 0 || strcmp("__asm", text) == 0)
            return TK_KEYWORD__ASM;
        if (strcmp("__stdcall", text) == 0 || strcmp("_stdcall", text) == 0)
            return TK_KEYWORD__STDCALL;
        if (strcmp("__cdecl", text) == 0)
            return TK_KEYWORD__CDECL;
        if (strcmp("__fastcall", text) == 0)
            return TK_KEYWORD__FASTCALL;
        if (strcmp("__alignof", text) == 0)
            return TK_KEYWORD__ALIGNOF;
        if (strcmp("__restrict", text) == 0)
            return TK_KEYWORD_RESTRICT;
#endif
        break;
    default:
        break;
    }

    return 0;
}


static void token_promote(const struct parser_ctx* ctx, struct token* token)
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
        char errormsg[100] = { 0 };
        char suffix[4] = { 0 };
        token->type = parse_number(token->lexeme, suffix, errormsg);
        if (token->type == TK_NONE)
        {
            compiler_diagnostic(C_INVALID_TOKEN, ctx, token, NULL, errormsg);
        }
    }
}

struct token* _Opt parser_look_ahead(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return NULL;

    struct token* _Opt p = ctx->current->next;
    while (p && !(p->flags & TK_FLAG_FINAL))
    {
        p = p->next;
    }

    if (p)
    {
        token_promote(ctx, p);
    }

    return p;
}


static struct token* _Opt pragma_match(const struct token* p_current)
{
    struct token* _Opt p_token = p_current->next;
    while (p_token && p_token->type == TK_BLANKS)
    {
        p_token = p_token->next;
    }
    return p_token;
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
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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

                // This is not working because this information needs to be in the AST. 
                // because it is used in a second step.
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
                    compiler_diagnostic(C_ERROR_PRAGMA_ERROR, ctx, ctx->current, NULL, "nullchecks pragma needs to use ON OFF");
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
                    if (ctx->options.diagnostic_stack.top_index <
                        sizeof(ctx->options.diagnostic_stack) / sizeof(ctx->options.diagnostic_stack.stack[0]))
                    {
                        ctx->options.diagnostic_stack.top_index++;
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] =
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index - 1];
                    }
                    ctx->current = ctx->current->next;
                    pragma_skip_blanks(ctx);
                }
                else if (ctx->current && strcmp(ctx->current->lexeme, "pop") == 0)
                {
                    // #pragma CAKE diagnostic pop
                    if (ctx->options.diagnostic_stack.top_index > 0)
                    {
                        ctx->options.diagnostic_stack.top_index--;
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

                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;

                        if (is_error)
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors |= w;
                        else if (is_warning)
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
                        else if (is_note)
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes |= w;
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
                        bool found = false;
                        for (int i = 0;
                             i < (int)(sizeof(ctx->p_report->last_diagnostics_ids) / sizeof(ctx->p_report->last_diagnostics_ids[0]));
                             i++)
                        {
                            if (ctx->p_report->last_diagnostics_ids[i] == 0) break;

                            if (ctx->p_report->last_diagnostics_ids[i] == id)
                            {
                                found = true;
                                // lets remove this error/warning/info from the final report.

                                int t =
                                    get_diagnostic_type(&ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index],
                                        id);
                                if (t == 3)
                                    ctx->p_report->error_count--;
                                else if (t == 2)
                                    ctx->p_report->warnings_count--;
                                else if (t == 1)
                                    ctx->p_report->info_count--;

                                break;
                            }
                        }

                        if (!found)
                        {
                            compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "pragma check failed");
                        }
                    }
                }
                else
                {
                    compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "unknown pragma");
                }
            }
        }
    }
    catch
    {
    }
}

static void parser_skip_blanks(struct parser_ctx* ctx)
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
        token_promote(ctx, ctx->current); // transform to parser token
    }
}

void parser_match(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return;

    ctx->previous = ctx->current;
    ctx->current = ctx->current->next;
    parser_skip_blanks(ctx);
}

void unexpected_end_of_file(struct parser_ctx* ctx)
{
    compiler_diagnostic(C_ERROR_UNEXPECTED_TOKEN, ctx, ctx->input_list.tail, NULL, "unexpected end of file");
}

NODISCARD
int parser_match_tk(struct parser_ctx* ctx, enum token_type type)
{
    int error = 0;
    if (ctx->current != NULL)
    {
        if (ctx->current->type != type)
        {
            compiler_diagnostic(C_ERROR_UNEXPECTED_TOKEN, ctx, ctx->current, NULL, "expected %s", get_token_name(type));
            error = 1;
        }

        ctx->previous = ctx->current;
        ctx->current = ctx->current->next;
        parser_skip_blanks(ctx);
    }
    else
    {
        compiler_diagnostic(C_ERROR_UNEXPECTED_TOKEN, ctx, ctx->input_list.tail, NULL, "unexpected end of file after");
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
        if (p_declaration_specifiers->typedef_declarator->name_opt)
            print_item(ss, &first, p_declaration_specifiers->typedef_declarator->name_opt->lexeme);
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
        //TODO leave as it is..and check at comparison 
        if (!type_specifier_is_integer(*flags))
        {
            // if you didn't specify anything, it becomes integer
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
        transform the sequence of two longs
        in
        TYPE_SPECIFIER_LONG_LONG
    */
    if (new_flag & TYPE_SPECIFIER_LONG) // adding a long
    {
        if ((*flags) & TYPE_SPECIFIER_LONG_LONG) // ja tinha long long
        {
            compiler_diagnostic(C_ERROR_CANNOT_COMBINE_WITH_PREVIOUS_LONG_LONG, ctx, ctx->current, NULL, "cannot combine with previous 'long long' declaration specifier");
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

    //Following 6.7.2 we check possible combinations
    switch ((unsigned int)*flags)
    {
    case TYPE_SPECIFIER_VOID:  //void
    case TYPE_SPECIFIER_CHAR:  //char
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_CHAR:  //signed char
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR:  //unsigned char
    case TYPE_SPECIFIER_SHORT:  //short
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_SHORT:  //signed short
    case TYPE_SPECIFIER_SHORT | TYPE_SPECIFIER_INT:  //short int
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_SHORT | TYPE_SPECIFIER_INT:  //signed short int
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_SHORT:  //unsigned short 
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_SHORT | TYPE_SPECIFIER_INT:  //unsigned short int
    case TYPE_SPECIFIER_INT:  //int
    case TYPE_SPECIFIER_SIGNED:  //signed
    case TYPE_SPECIFIER_INT | TYPE_SPECIFIER_SIGNED:  //int signed
    case TYPE_SPECIFIER_UNSIGNED:  //signed
    case TYPE_SPECIFIER_INT | TYPE_SPECIFIER_UNSIGNED:  //int unsigned
    case TYPE_SPECIFIER_LONG:  //long
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_LONG:  //signed long
    case TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_INT:  //long int
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_INT:  //signed long int
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG:  //unsigned long
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_INT:  //unsigned long int
    case TYPE_SPECIFIER_LONG_LONG:  //long long
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_LONG_LONG:  //signed long long
    case TYPE_SPECIFIER_LONG_LONG | TYPE_SPECIFIER_INT:  //long long int
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_LONG_LONG | TYPE_SPECIFIER_INT:  //signed long long
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG_LONG:  //unsigned long long
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG_LONG | TYPE_SPECIFIER_INT:  //unsigned long long int
        // _BitInt constant-expression, or signed _BitInt constant-expression        
        // unsigned _BitInt constant-expression
    case TYPE_SPECIFIER_FLOAT:  //float
    case TYPE_SPECIFIER_DOUBLE:  //double
    case TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_DOUBLE:  //long double
    case TYPE_SPECIFIER_DECIMAL32:  //_Decimal32
    case TYPE_SPECIFIER_DECIMAL64:  //_Decimal64
    case TYPE_SPECIFIER_DECIMAL128:  //_Decimal128
    case TYPE_SPECIFIER_BOOL:  //bool
    case TYPE_SPECIFIER_COMPLEX | TYPE_SPECIFIER_FLOAT:  //complex float
    case TYPE_SPECIFIER_COMPLEX | TYPE_SPECIFIER_DOUBLE:  //complex double
    case TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_COMPLEX | TYPE_SPECIFIER_DOUBLE:  //complex long double        
    case TYPE_SPECIFIER_ATOMIC:  //complex long double
    case TYPE_SPECIFIER_STRUCT_OR_UNION:  //complex long double
    case TYPE_SPECIFIER_ENUM:  //complex long double
    case TYPE_SPECIFIER_TYPEOF:  //typeof        
    case TYPE_SPECIFIER_TYPEDEF:

    case TYPE_SPECIFIER_INT8:
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT8:

    case TYPE_SPECIFIER_INT16:
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT16:
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_INT16:
    case TYPE_SPECIFIER_INT32:
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT32:
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_INT32:
    case TYPE_SPECIFIER_INT64:
    case TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT64:
    case TYPE_SPECIFIER_SIGNED | TYPE_SPECIFIER_INT64:
        //VALID
        break;
    default:
        compiler_diagnostic(C_ERROR_TWO_OR_MORE_SPECIFIERS, ctx, ctx->current, NULL, "incompatible specifiers");
        return 1;
    }

    return 0;
}

void declaration_specifiers_delete(struct declaration_specifiers* _Owner _Opt p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);

        struct declaration_specifier* _Owner _Opt item = p->head;
        while (item)
        {
            struct declaration_specifier* _Owner _Opt next = item->next;
            item->next = NULL;
            declaration_specifier_delete(item);
            item = next;
        }
        free(p);
    }
}

void declaration_specifiers_add(struct declaration_specifiers* list, struct declaration_specifier* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

struct declaration_specifiers* _Owner _Opt declaration_specifiers(struct parser_ctx* ctx,
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

    if (ctx->current == NULL)
        return NULL;

    _Opt struct declaration_specifiers* _Owner _Opt p_declaration_specifiers = calloc(1, sizeof(struct declaration_specifiers));

    try
    {
        if (p_declaration_specifiers == NULL)
            throw;

        p_declaration_specifiers->first_token = ctx->current;

        while (first_of_declaration_specifier(ctx))
        {
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

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

            struct declaration_specifier* _Owner _Opt p_declaration_specifier = declaration_specifier(ctx);
            if (p_declaration_specifier == NULL) throw;

            if (p_declaration_specifier->type_specifier_qualifier)
            {
                if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                {
                    if (add_specifier(ctx,
                        &p_declaration_specifiers->type_specifier_flags,
                        p_declaration_specifier->type_specifier_qualifier->type_specifier->flags) != 0)
                    {
                        //not fatal error
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
                    else if (p_declaration_specifier->type_specifier_qualifier->type_specifier->token->type == TK_IDENTIFIER)
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
            else if (p_declaration_specifier->function_specifier)
            {
                p_declaration_specifiers->function_specifier_flags |= p_declaration_specifier->function_specifier->flags;
            }

            declaration_specifiers_add(p_declaration_specifiers, p_declaration_specifier);

            assert(p_declaration_specifiers->p_attribute_specifier_sequence_opt == NULL);
            p_declaration_specifiers->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

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

        struct token* _Opt prev = previous_parser_token(ctx->current);
        if (prev == NULL)
            throw;

        p_declaration_specifiers->last_token = prev;

        // int main() { static int i; } // i is not automatic
        final_specifier(ctx, &p_declaration_specifiers->type_specifier_flags);

        p_declaration_specifiers->storage_class_specifier_flags |= default_storage_flag;

        if (p_declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
        {
            //
            p_declaration_specifiers->storage_class_specifier_flags &= ~STORAGE_SPECIFIER_AUTOMATIC_STORAGE;
        }
    }
    catch
    {
        declaration_specifiers_delete(p_declaration_specifiers);
        p_declaration_specifiers = NULL;
    }
    return p_declaration_specifiers;
}

struct declaration* _Owner _Opt declaration_core(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt /*SINK*/,
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

    struct declaration* _Owner _Opt p_declaration = NULL;

    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_declaration = calloc(1, sizeof(struct declaration));
        if (p_declaration == NULL)
        {
            throw;
        }

        p_declaration->p_attribute_specifier_sequence_opt = p_attribute_specifier_sequence_opt;
        p_attribute_specifier_sequence_opt = NULL; /*MOVED*/

        p_declaration->first_token = ctx->current;

        if (ctx->current->type == ';')
        {
            p_declaration->last_token = ctx->current;
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
                if (p_declaration->declaration_specifiers == NULL) throw;

                if (p_declaration->p_attribute_specifier_sequence_opt)
                {
                    p_declaration->declaration_specifiers->attributes_flags =
                        p_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
                }

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }

                if (ctx->current->type != ';')
                {
                    p_declaration->init_declarator_list = init_declarator_list(ctx,
                        p_declaration->declaration_specifiers);

                    if (p_declaration->init_declarator_list.head == NULL)
                        throw;
                }

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }

                p_declaration->last_token = ctx->current;

                if (ctx->current->type == '{')
                {
                    if (can_be_function_definition)
                        *is_function_definition = true;
                }
#if EXPERIMENTAL_CONTRACTS
                else if (ctx->current->type == TK_KEYWORD_TRUE ||
                         ctx->current->type == TK_KEYWORD_FALSE ||
                         ctx->current->type == TK_IDENTIFIER)
                {
                    if (can_be_function_definition)
                        *is_function_definition = true;
                }
#endif
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
                    compiler_diagnostic(C_ERROR_INVALID_TYPE, ctx, ctx->current, NULL, "invalid type '%s'", ctx->current->lexeme);
                }
                else
                {
                    compiler_diagnostic(C_ERROR_EXPECTED_DECLARATION, ctx, ctx->current, NULL, "expected declaration not '%s'", ctx->current->lexeme);
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

struct declaration* _Owner _Opt declaration(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt,
    enum storage_class_specifier_flags storage_specifier_flags,
    bool extern_declaration);

struct declaration* _Owner _Opt function_definition_or_declaration(struct parser_ctx* ctx)
{
    return declaration(ctx, NULL, STORAGE_SPECIFIER_NONE, true);
    /*
     function-definition:
        attribute-specifier-sequence _Opt declaration-specifiers declarator function-body
    */

    /*
      declaration:
        declaration-specifiers                              init-declarator-list opt ;
        attribute-specifier-sequence declaration-specifiers init-declarator-list ;
        static_assert-declaration
        attribute-declaration
    */

}

struct simple_declaration* _Owner _Opt simple_declaration(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt,
    bool ignore_semicolon)
{

    if (ctx->current == NULL)
    {
        unexpected_end_of_file(ctx);
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
        return NULL;
    }

    enum storage_class_specifier_flags storage_specifier_flags = STORAGE_SPECIFIER_AUTOMATIC_STORAGE;
    /*
      simple-declaration:
      declaration-specifiers init-declarator-list _Opt ;
      attribute-specifier-sequence declaration-specifiers init-declarator-list ;
    */

    struct simple_declaration* _Owner _Opt p_simple_declaration = calloc(1, sizeof(struct simple_declaration));

    try
    {
        if (p_simple_declaration == NULL)
            throw;

        p_simple_declaration->first_token = ctx->current;

        p_simple_declaration->p_attribute_specifier_sequence_opt = p_attribute_specifier_sequence_opt;
        p_attribute_specifier_sequence_opt = NULL; /*MOVED*/

        struct declaration_specifiers* _Owner _Opt ptemp = declaration_specifiers(ctx, storage_specifier_flags);
        if (ptemp == NULL) throw;

        p_simple_declaration->p_declaration_specifiers = ptemp;

        if (p_simple_declaration->p_attribute_specifier_sequence_opt)
        {
            p_simple_declaration->p_declaration_specifiers->attributes_flags =
                p_simple_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
        }

        p_simple_declaration->init_declarator_list = init_declarator_list(ctx, p_simple_declaration->p_declaration_specifiers);

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        struct token* _Opt prev = previous_parser_token(ctx->current);
        if (prev == NULL) throw;

        p_simple_declaration->last_token = prev;

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

static void check_unused_parameters(struct parser_ctx* ctx, struct parameter_list* parameter_list)
{
    struct parameter_declaration* _Opt parameter = NULL;
    parameter = parameter_list->head;

    while (parameter)
    {
        if (!type_is_maybe_unused(&parameter->declarator->type) &&
            parameter->declarator &&
            parameter->declarator->num_uses == 0)
        {
            if (parameter->declarator->name_opt &&
                parameter->declarator->name_opt->level == 0 /*direct source*/
                )
            {
                compiler_diagnostic(W_UNUSED_PARAMETER,
                    ctx,
                    parameter->declarator->name_opt, NULL,
                    "'%s': unreferenced formal parameter",
                    parameter->declarator->name_opt->lexeme);
            }
        }
        parameter = parameter->next;
    }
}

struct declaration* _Owner _Opt declaration(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt00,
    enum storage_class_specifier_flags storage_specifier_flags,
    bool extern_declaration)
{
    struct declaration* _Owner _Opt p_declaration = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);

        bool is_function_definition = false;

        p_declaration = declaration_core(ctx, p_attribute_specifier_sequence_opt, true, &is_function_definition, storage_specifier_flags, false);
        if (p_declaration == NULL)
            throw;

        if (is_function_definition)
        {

            if (p_declaration->init_declarator_list.head == NULL ||
                p_declaration->init_declarator_list.head->p_declarator->direct_declarator == NULL ||
                p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator == NULL)
            {
                throw; //unexpected
            }

            struct declarator* p_declarator =
                p_declaration->init_declarator_list.head->p_declarator;


            //ctx->p_current_function_opt = p_declaration->init_declarator_list.head->p_declarator;

            /*
                scope of parameters is the inner declarator

                void (*f(int i))(void) {
                    i = 1;
                    return 0;
                }
            */

            assert(p_declaration->init_declarator_list.head != NULL); //because functions definitions have names

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


            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            check_func_open_brace_style(ctx, ctx->current);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;

            }
            struct diagnostic before_function_diagnostics = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];
#if EXPERIMENTAL_CONTRACTS


            if (ctx->current->type == TK_KEYWORD_TRUE ||
                ctx->current->type == TK_KEYWORD_FALSE ||
                ctx->current->type == TK_IDENTIFIER)
            {
                for (;;)
                {
                    if (ctx->current == NULL)
                    {
                        unexpected_end_of_file(ctx);
                        throw;
                    }

                    enum token_type type = ctx->current->type;
                    if (type != TK_KEYWORD_TRUE &&
                        type != TK_KEYWORD_FALSE &&
                        type != TK_IDENTIFIER)
                    {
                        throw;
                    }
                    parser_match(ctx); //true
                    parser_match(ctx); //(

                    if (type != TK_KEYWORD_FALSE)
                    {
                        assert(p_declarator->p_expression_true == NULL);
                        p_declarator->p_expression_true = expression(ctx);
                    }
                    else
                    {
                        assert(p_declarator->p_expression_false == NULL);
                        p_declarator->p_expression_false = expression(ctx);
                    }
                    parser_match(ctx); //)

                    if (ctx->current == NULL)
                    {
                        unexpected_end_of_file(ctx);
                        throw;
                    }

                    if (ctx->current->type != ',')
                        break;

                    parser_match(ctx); //)
                }
            }
#endif
            struct declarator* _Opt p_current_function_opt = ctx->p_current_function_opt;
            ctx->p_current_function_opt = p_declarator;

            struct scope * p_current_function_scope_opt = ctx->p_current_function_scope_opt;
            ctx->p_current_function_scope_opt =  ctx->scopes.tail;

            struct scope* parameters_scope = &inner->direct_declarator->function_declarator->parameters_scope;
            scope_list_push(&ctx->scopes, parameters_scope);

            struct compound_statement* _Owner _Opt p_function_body = function_body(ctx);

            ctx->p_current_function_scope_opt =  p_current_function_scope_opt; //restore
            ctx->p_current_function_opt = p_current_function_opt; //restore
            scope_list_pop(&ctx->scopes);

            if (p_function_body == NULL)
                throw;

            assert(p_declaration->function_body == NULL);
            p_declaration->function_body = p_function_body;
            p_declaration->init_declarator_list.head->p_declarator->function_body = p_declaration->function_body;

            if (p_declaration->init_declarator_list.head &&
                p_declaration->init_declarator_list.head->p_declarator->direct_declarator &&
                p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator &&
                p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt &&
                p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
            {
                check_unused_parameters(ctx, p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list);
            }

        
            struct defer_visit_ctx ctx2 = { .ctx = ctx };
            defer_start_visit_declaration(&ctx2, p_declaration);
            defer_visit_ctx_destroy(&ctx2);


            if (ctx->options.flow_analysis)
            {
                /*
                 Now we have the full function AST let´s visit to Analise
                 jumps
                */

                /*we are going to visit the function again.. lets put the same diagnostic state*/
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = before_function_diagnostics;

                struct flow_visit_ctx ctx2 = { 0 };
                ctx2.ctx = ctx;
                flow_start_visit_declaration(&ctx2, p_declaration);
                flow_visit_ctx_destroy(&ctx2);
            }

        }
        else
        {
            if (ctx->options.flow_analysis)
            {

                if (p_declaration && p_declaration->declaration_specifiers)
                {
                    if (!(p_declaration->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTOMATIC_STORAGE))
                    {
                        //USE AFTER FREE, IF USED IN LOCAL VARIABLES!
                        /*
                         *  The objective of this visit is to initialize global flow_objects.
                         *  It also executes static_debug
                         */
                        _Opt struct flow_visit_ctx ctx2 = { 0 };
                        ctx2.ctx = ctx;
                        flow_start_visit_declaration(&ctx2, p_declaration);
                        flow_visit_ctx_destroy(&ctx2);
                    }

                }
                else if (extern_declaration)
                {
                    //pragma static assert etc..
                    _Opt struct flow_visit_ctx ctx2 = { 0 };
                    ctx2.ctx = ctx;
                    flow_start_visit_declaration(&ctx2, p_declaration);
                    flow_visit_ctx_destroy(&ctx2);

                }
            }
        }
    }
    catch
    {
        declaration_delete(p_declaration);
        p_declaration = NULL;
    }

    return p_declaration;

    //bool is_function_definition = false;
    //return declaration_core(ctx, p_attribute_specifier_sequence_opt, false, &is_function_definition, storage_specifier_flags, false);
}

//(6.7) declaration-specifiers:
// declaration-specifier attribute-specifier-sequenceopt
// declaration-specifier declaration-specifiers

void declaration_specifier_delete(struct declaration_specifier* _Owner _Opt p)
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

struct declaration_specifier* _Owner _Opt declaration_specifier(struct parser_ctx* ctx)
{
    //    storage-class-specifier
    //    type-specifier-qualifier
    //    function-specifier

    struct declaration_specifier* _Owner _Opt p_declaration_specifier = NULL;
    try
    {
        p_declaration_specifier = calloc(1, sizeof * p_declaration_specifier);
        if (p_declaration_specifier == NULL)
            throw;

        if (first_of_storage_class_specifier(ctx))
        {
            p_declaration_specifier->storage_class_specifier = storage_class_specifier(ctx);
            if (p_declaration_specifier->storage_class_specifier == NULL) throw;
        }
        else if (first_of_type_specifier_qualifier(ctx))
        {
            p_declaration_specifier->type_specifier_qualifier = type_specifier_qualifier(ctx);
            if (p_declaration_specifier->type_specifier_qualifier == NULL) throw;
        }
        else if (first_of_function_specifier(ctx))
        {
            p_declaration_specifier->function_specifier = function_specifier(ctx);
            if (p_declaration_specifier->function_specifier == NULL) throw;
        }
        else
        {
            compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "unexpected");
        }
    }
    catch
    {
        declaration_specifier_delete(p_declaration_specifier);
        p_declaration_specifier = NULL;
    }

    return p_declaration_specifier;
}

struct init_declarator* _Owner init_declarator_add_ref(struct init_declarator* p)
{
    p->has_shared_ownership = true;
    return (struct init_declarator* _Owner)p;
}


void init_declarator_sink(struct init_declarator* _Owner _Opt p) {}

void init_declarator_delete(struct init_declarator* _Owner _Opt p)
{
    if (p)
    {
        if (p->has_shared_ownership)
        {
            p->has_shared_ownership = false;
            init_declarator_sink(p);
            return;
        }

        initializer_delete(p->initializer);
        declarator_delete(p->p_declarator);
        assert(p->next == NULL);
        free(p);
    }
}


struct init_declarator* _Owner _Opt init_declarator(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers)
{
    /*
     init-declarator:
       declarator
       declarator = initializer
    */

    struct init_declarator* _Owner _Opt p_init_declarator = NULL;
    try
    {
        p_init_declarator = calloc(1, sizeof(struct init_declarator));
        if (p_init_declarator == NULL)
            throw;

        struct token* _Opt tkname = NULL;

        {
            struct declarator* _Owner _Opt p_temp_declarator = declarator(ctx,
                NULL,
                p_declaration_specifiers,
                false,
                &tkname);
            if (p_temp_declarator == NULL) throw;
            p_init_declarator->p_declarator = p_temp_declarator;
        }

        if (tkname == NULL)
        {
            compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, NULL, "init declarator must have a name");
            throw;
        }

        p_init_declarator->p_declarator->declaration_specifiers = p_declaration_specifiers;
        p_init_declarator->p_declarator->name_opt = tkname;

        if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
        {
            /*
              auto requires we find the type after initializer
            */
        }
        else
        {
            assert(p_init_declarator->p_declarator->type.type_specifier_flags == 0);
            p_init_declarator->p_declarator->type = make_type_using_declarator(ctx, p_init_declarator->p_declarator);
        }

        assert(p_init_declarator->p_declarator->declaration_specifiers != NULL);

        assert(ctx->scopes.tail != NULL);

        /*
          Checking naming conventions
        */
        if (ctx->scopes.tail->scope_level == 0)
        {
            naming_convention_global_var(ctx,
                tkname,
                &p_init_declarator->p_declarator->type,
                p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags);
        }

        /////////////////////////////////////////////////////////////////////////////
        const char* name = p_init_declarator->p_declarator->name_opt->lexeme;
        struct scope* _Opt out_scope = NULL;
        struct declarator* _Opt previous = find_declarator(ctx, name, &out_scope);
        if (previous)
        {
            assert(out_scope != NULL);
            assert(ctx->scopes.tail != NULL);

            if (out_scope->scope_level == ctx->scopes.tail->scope_level)
            {
                if (out_scope->scope_level == 0)
                {
                    /*
                    __C_ASSERT__ is failing..maybe because __builtin_offsetof is not implemented
                    */
                    if (strcmp(name, "__C_ASSERT__") != 0)
                    {
                        //TODO type_is_same needs changes see #164
                        if (!type_is_same(&previous->type, &p_init_declarator->p_declarator->type, false))
                        {
                            struct osstream ss = { 0 };
                            print_type_no_names(&ss, &previous->type);

                            compiler_diagnostic(
                                C_ERROR_REDECLARATION,
                                ctx,
                                ctx->current,
                                NULL,
                                "conflicting types for '%s' (%s)", name, ss.c_str);

                            ss_clear(&ss);
                            print_type_no_names(&ss, &p_init_declarator->p_declarator->type);

                            compiler_diagnostic(C_ERROR_REDECLARATION,
                                ctx,
                                previous->name_opt,
                                NULL,
                                "previous declaration (%s)", ss.c_str);
                            ss_close(&ss);
                        }
                    }
                }
                else
                {
                    compiler_diagnostic(C_ERROR_REDECLARATION, ctx, ctx->current, NULL, "redeclaration");
                    compiler_diagnostic(W_NOTE, ctx, previous->name_opt, NULL, "previous declaration");
                }
            }
            else
            {
                struct hash_item_set item = { 0 };
                item.p_init_declarator = init_declarator_add_ref(p_init_declarator);
                hashmap_set(&ctx->scopes.tail->variables, name, &item);
                hash_item_set_destroy(&item);

                /*global scope no warning...*/
                if (out_scope->scope_level != 0)
                {
                    /*but redeclaration at function scope we show warning*/
                    if (compiler_diagnostic(W_DECLARATOR_HIDE, ctx, p_init_declarator->p_declarator->first_token_opt, NULL, "declaration of '%s' hides previous declaration", name))
                    {
                        compiler_diagnostic(W_NOTE, ctx, previous->first_token_opt, NULL, "previous declaration is here");
                    }
                }
            }
        }
        else
        {
            /*first time we see this declarator*/
            struct hash_item_set item = { 0 };
            item.p_init_declarator = init_declarator_add_ref(p_init_declarator);
            hashmap_set(&ctx->scopes.tail->variables, name, &item);
            hash_item_set_destroy(&item);
        }
        /////////////////////////////////////////////////////////////////////////////


        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == '=')
        {
            const bool requires_constant_initialization =
                (ctx->p_current_function_opt == NULL) ||
                (p_declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC);

            parser_match(ctx);

            assert(p_init_declarator->initializer == NULL);
            p_init_declarator->initializer = initializer(ctx);

            if (p_init_declarator->initializer == NULL)
            {
                throw;
            }

            if (p_init_declarator->initializer->braced_initializer)
            {
                if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
                {
                    compiler_diagnostic(C_ERROR_AUTO_NEEDS_SINGLE_DECLARATOR, ctx, p_init_declarator->p_declarator->first_token_opt, NULL, "'auto' requires a plain identifier");
                    throw;
                }


                int er = make_object(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object);
                if (er != 0)
                {
                    compiler_diagnostic(C_ERROR_STRUCT_IS_INCOMPLETE, ctx, p_init_declarator->p_declarator->first_token_opt, NULL, "incomplete struct/union type");
                    throw;
                }


                bool is_constant = type_is_const(&p_init_declarator->p_declarator->type) ||
                    p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR;


                if (initializer_init_new(ctx,
                    &p_init_declarator->p_declarator->type,
                    &p_init_declarator->p_declarator->object,
                    p_init_declarator->initializer,
                    is_constant,
                    requires_constant_initialization) != 0)
                {
                    throw;
                }

                p_init_declarator->p_declarator->object.type.num_of_elements =
                    p_init_declarator->p_declarator->type.num_of_elements;
                //fixing the name of members?
            }
            else if (p_init_declarator->initializer->assignment_expression)
            {
                if (type_is_array(&p_init_declarator->p_declarator->type))
                {
                    const unsigned long long array_size_elements = p_init_declarator->p_declarator->type.num_of_elements;
                    if (array_size_elements == 0)
                    {
                        p_init_declarator->p_declarator->type.num_of_elements =
                            p_init_declarator->initializer->assignment_expression->type.num_of_elements;
                    }
                    else
                    {
                        if (p_init_declarator->initializer->assignment_expression->type.num_of_elements > array_size_elements)
                        {
                            if (p_init_declarator->p_declarator->first_token_opt)
                            {
                                compiler_diagnostic(W_ARRAY_SIZE, ctx, p_init_declarator->p_declarator->first_token_opt, NULL, "initializer for array is too long");
                            }
                        }
                    }
                }

                /*
                  Fixing the type of auto declarator
                */
                assert(p_init_declarator->p_declarator->declaration_specifiers != NULL);

                if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
                {

                    if (p_init_declarator->p_declarator->direct_declarator &&
                        (p_init_declarator->p_declarator->direct_declarator->array_declarator != NULL ||
                            p_init_declarator->p_declarator->direct_declarator->function_declarator != NULL))
                    {
                        compiler_diagnostic(C_ERROR_AUTO_NEEDS_SINGLE_DECLARATOR, ctx, p_init_declarator->p_declarator->first_token_opt, NULL, "'auto' requires a plain identifier");
                        throw;
                    }

                    if (p_init_declarator->p_declarator->pointer != NULL)
                    {
                        compiler_diagnostic(C_ERROR_AUTO_NEEDS_SINGLE_DECLARATOR, ctx, p_init_declarator->p_declarator->first_token_opt, NULL, "'auto' requires a plain identifier");
                    }

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
                    t.name_opt = strdup(p_init_declarator->p_declarator->name_opt->lexeme);

                    type_set_qualifiers_using_declarator(&t, p_init_declarator->p_declarator);

                    type_visit_to_mark_anonymous(&t);
                    type_swap(&p_init_declarator->p_declarator->type, &t);
                    type_destroy(&t);
                }

                check_assigment(ctx, &p_init_declarator->p_declarator->type, p_init_declarator->initializer->assignment_expression, ASSIGMENT_TYPE_INIT);

                const char* name2 = p_init_declarator->p_declarator->name_opt ?
                    p_init_declarator->p_declarator->name_opt->lexeme : "";

                int er = make_object_with_name(&p_init_declarator->p_declarator->type,
                    &p_init_declarator->p_declarator->object, name2);

                if (er != 0)
                {
                    throw;
                }

                bool is_constant = type_is_const(&p_init_declarator->p_declarator->type) ||
                    p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR;


                //intf("\n");                

                if (initializer_init_new(ctx,
                    &p_init_declarator->p_declarator->type,
                    &p_init_declarator->p_declarator->object,
                    p_init_declarator->initializer,
                    is_constant,
                    requires_constant_initialization) != 0)
                {
                    throw;
                }
                //object_print_to_debug(&p_init_declarator->p_declarator->object);
            }
        }
        else
        {
            if (p_init_declarator->p_declarator->type.category != TYPE_CATEGORY_FUNCTION &&
                !(p_init_declarator->p_declarator->type.storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF))
            {
                const char* name2 = p_init_declarator->p_declarator->name_opt ?
                    p_init_declarator->p_declarator->name_opt->lexeme : "";

                int er = make_object_with_name(&p_init_declarator->p_declarator->type,
                    &p_init_declarator->p_declarator->object,
                    name2);

                if (er != 0)
                {
                    if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN)
                    {
                        //extern struct X x;
                    }
                    else
                    {
                        compiler_diagnostic(C_ERROR_STRUCT_IS_INCOMPLETE, ctx, p_init_declarator->p_declarator->first_token_opt, NULL, "incomplete struct/union type");
                        throw;
                    }
                }

                if (type_is_const(&p_init_declarator->p_declarator->type))
                {
                    if (p_declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF)
                    {
                        //no warning on typedefs
                    }
                    else
                    {
                        compiler_diagnostic(W_CONST_NOT_INITIALIZED,
                            ctx,
                            p_init_declarator->p_declarator->first_token_opt, NULL,
                            "const object should be initialized");
                    }
                }
            }
        }

        /*
           checking usage of [static ] other than in function arguments
        */
        if (p_init_declarator->p_declarator)
        {
            if (type_is_array(&p_init_declarator->p_declarator->type))
                if (p_init_declarator->p_declarator->type.type_qualifier_flags != 0 ||
                    p_init_declarator->p_declarator->type.has_static_array_size)
                {
                    if (p_init_declarator->p_declarator->first_token_opt)
                    {
                        compiler_diagnostic(C_ERROR_STATIC_OR_TYPE_QUALIFIERS_NOT_ALLOWED_IN_NON_PARAMETER,
                            ctx,
                            p_init_declarator->p_declarator->first_token_opt, NULL,
                            "static or type qualifiers are not allowed in non-parameter array declarator");
                    }
                    else if (p_init_declarator->initializer)
                    {
                        compiler_diagnostic(C_ERROR_STATIC_OR_TYPE_QUALIFIERS_NOT_ALLOWED_IN_NON_PARAMETER,
                        ctx,
                        p_init_declarator->initializer->first_token, NULL,
                        "static or type qualifiers are not allowed in non-parameter array declarator");
                    }
                }

            if (!type_is_pointer(&p_init_declarator->p_declarator->type) &&
                p_init_declarator->p_declarator->type.type_qualifier_flags & TYPE_QUALIFIER_DTOR)
            {
                if (p_init_declarator->p_declarator->first_token_opt)
                {
                    compiler_diagnostic(C_ERROR_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER,
                        ctx,
                        p_init_declarator->p_declarator->first_token_opt, NULL,
                        "_Dtor qualifier can only be used with pointers");
                }
                else if (p_init_declarator->initializer)
                {
                    compiler_diagnostic(C_ERROR_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER,
                    ctx,
                    p_init_declarator->initializer->first_token, NULL,
                    "_Dtor qualifier can only be used with pointers");
                }
            }
        }

        if (
            !(p_init_declarator->p_declarator->type.storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF) &&
            !type_is_function(&p_init_declarator->p_declarator->type))
        {

            if (type_is_vla(&p_init_declarator->p_declarator->type))
            {
            }
            else if (type_is_function(&p_init_declarator->p_declarator->type))
            {
                compiler_diagnostic(C_ERROR_STORAGE_SIZE,
                  ctx,
                  p_init_declarator->p_declarator->name_opt, NULL,
                  "invalid application of 'sizeof' to a function type");
            }
            else
            {
                size_t sz = 0;
                enum sizeof_error size_result = type_get_sizeof(&p_init_declarator->p_declarator->type, &sz);

                if (size_result == ESIZEOF_NONE)
                {
                    //ok
                }
                else if (size_result == ESIZEOF_INCOMPLETE)
                {
                    if (p_init_declarator->p_declarator->type.storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN)
                    {
                        //this is not a problem for extern variables
                    }
                    else
                    {
                        // clang warning: array 'c' assumed to have one element
                        // gcc "error: storage size of '%s' isn't known"
                        compiler_diagnostic(C_ERROR_STORAGE_SIZE,
                            ctx,
                            p_init_declarator->p_declarator->name_opt, NULL,
                            "storage size of '%s' isn't known",
                            p_init_declarator->p_declarator->name_opt->lexeme);
                    }
                }
                else if (size_result == ESIZEOF_OVERLOW)
                {
                    compiler_diagnostic(C_ERROR_STORAGE_SIZE,
                            ctx,
                            p_init_declarator->p_declarator->name_opt, NULL,
                            "sizeof '%s' is too large",
                            p_init_declarator->p_declarator->name_opt->lexeme);
                }
                else
                {
                    compiler_diagnostic(C_ERROR_STORAGE_SIZE,
                        ctx,
                        p_init_declarator->p_declarator->name_opt, NULL,
                        "storage size of '%s' isn't known",
                        p_init_declarator->p_declarator->name_opt->lexeme);
                }
            }
        }
    }
    catch
    {
        init_declarator_delete(p_init_declarator);
        p_init_declarator = NULL;
    }

    return p_init_declarator;
}

void init_declarator_list_add(struct init_declarator_list* list, struct init_declarator* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void init_declarator_list_destroy(_Dtor struct init_declarator_list* p)
{
    struct init_declarator* _Owner _Opt item = p->head;
    while (item)
    {
        struct init_declarator* _Owner _Opt next = item->next;
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
    struct init_declarator* _Owner _Opt p_init_declarator = NULL;

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

void storage_class_specifier_delete(struct storage_class_specifier* _Owner _Opt p)
{
    if (p)
    {
        free(p);
    }
}

struct storage_class_specifier* _Owner _Opt storage_class_specifier(struct parser_ctx* ctx)
{
    struct storage_class_specifier* _Owner _Opt p_storage_class_specifier = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_storage_class_specifier = calloc(1, sizeof(struct storage_class_specifier));
        if (p_storage_class_specifier == NULL)
            throw;

        p_storage_class_specifier->token = ctx->current;
        switch (ctx->current->type)
        {
        case TK_KEYWORD_TYPEDEF:
            p_storage_class_specifier->flags = STORAGE_SPECIFIER_TYPEDEF;
            break;
        case TK_KEYWORD_EXTERN:
            p_storage_class_specifier->flags = STORAGE_SPECIFIER_EXTERN;
            break;
        case TK_KEYWORD_CONSTEXPR:

            p_storage_class_specifier->flags = STORAGE_SPECIFIER_CONSTEXPR;
            if (ctx->scopes.tail && ctx->scopes.tail->scope_level == 0)
                p_storage_class_specifier->flags |= STORAGE_SPECIFIER_CONSTEXPR_STATIC;
            break;
        case TK_KEYWORD_STATIC:
            p_storage_class_specifier->flags = STORAGE_SPECIFIER_STATIC;
            break;
        case TK_KEYWORD__THREAD_LOCAL:
            p_storage_class_specifier->flags = STORAGE_SPECIFIER_THREAD_LOCAL;
            break;
        case TK_KEYWORD_AUTO:
            p_storage_class_specifier->flags = STORAGE_SPECIFIER_AUTO;
            break;
        case TK_KEYWORD_REGISTER:
            p_storage_class_specifier->flags = STORAGE_SPECIFIER_REGISTER;
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
    }
    catch
    {
        storage_class_specifier_delete(p_storage_class_specifier);
        p_storage_class_specifier = NULL;
    }

    return p_storage_class_specifier;
}

struct typeof_specifier_argument* _Owner _Opt typeof_specifier_argument(struct parser_ctx* ctx)
{
    struct typeof_specifier_argument* _Owner _Opt new_typeof_specifier_argument = NULL;
    try
    {
        new_typeof_specifier_argument = calloc(1, sizeof(struct typeof_specifier_argument));
        if (new_typeof_specifier_argument == NULL)
            throw;

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
        typeof_specifier_argument_delete(new_typeof_specifier_argument);
        new_typeof_specifier_argument = NULL;
    }

    return new_typeof_specifier_argument;
}

bool first_of_typeof_specifier(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD_TYPEOF ||
        ctx->current->type == TK_KEYWORD_TYPEOF_UNQUAL;
}

struct typeof_specifier* _Owner _Opt  typeof_specifier(struct parser_ctx* ctx)
{
    struct typeof_specifier* _Owner _Opt p_typeof_specifier = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_typeof_specifier = calloc(1, sizeof(struct typeof_specifier));
        if (p_typeof_specifier == NULL)
            throw;

        p_typeof_specifier->first_token = ctx->current;

        const bool is_typeof_unqual = ctx->current->type == TK_KEYWORD_TYPEOF_UNQUAL;
        parser_match(ctx);
        if (parser_match_tk(ctx, '(') != 0)
        {
            throw;
        }

        struct typeof_specifier_argument* _Owner _Opt p_typeof_specifier_argument =
            typeof_specifier_argument(ctx);

        if (p_typeof_specifier_argument == NULL)
        {
            throw;
        }

        p_typeof_specifier->typeof_specifier_argument = p_typeof_specifier_argument;

        if (p_typeof_specifier->typeof_specifier_argument->expression)
        {
            p_typeof_specifier->type = type_dup(&p_typeof_specifier->typeof_specifier_argument->expression->type);
        }
        else if (p_typeof_specifier->typeof_specifier_argument->type_name)
        {
            p_typeof_specifier->type = type_dup(&p_typeof_specifier->typeof_specifier_argument->type_name->abstract_declarator->type);
        }

        if (p_typeof_specifier->type.storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            compiler_diagnostic(W_TYPEOF_ARRAY_PARAMETER, ctx, ctx->current, NULL, "typeof used in array arguments");

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

        free((void* _Owner)p_typeof_specifier->type.name_opt);
        p_typeof_specifier->type.name_opt = NULL;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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

void typeof_specifier_argument_delete(struct typeof_specifier_argument* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->expression);
        type_name_delete(p->type_name);
        free(p);
    }
}

void typeof_specifier_delete(struct typeof_specifier* _Owner _Opt p)
{
    if (p)
    {
        typeof_specifier_argument_delete(p->typeof_specifier_argument);
        type_destroy(&p->type);
        free(p);
    }
}

void type_specifier_delete(struct type_specifier* _Owner _Opt p)
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
struct type_specifier* _Owner _Opt type_specifier(struct parser_ctx* ctx)
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

    struct type_specifier* _Owner _Opt p_type_specifier = NULL;

    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_type_specifier = calloc(1, sizeof * p_type_specifier);
        if (p_type_specifier == NULL)
            throw;

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
            if (p_type_specifier->typeof_specifier == NULL)
            {
                type_specifier_delete(p_type_specifier);
                return NULL;
            }

        }
        else if (first_of_atomic_type_specifier(ctx))
        {
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = TYPE_SPECIFIER_ATOMIC;
            p_type_specifier->atomic_type_specifier = atomic_type_specifier(ctx);
            if (p_type_specifier->atomic_type_specifier == NULL)
            {
                type_specifier_delete(p_type_specifier);
                return NULL;
            }
        }
        else if (first_of_struct_or_union(ctx))
        {
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = TYPE_SPECIFIER_STRUCT_OR_UNION;
            p_type_specifier->struct_or_union_specifier = struct_or_union_specifier(ctx);
            if (p_type_specifier->struct_or_union_specifier == NULL)
            {
                type_specifier_delete(p_type_specifier);
                return NULL;
            }
        }
        else if (first_of_enum_specifier(ctx))
        {
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = TYPE_SPECIFIER_ENUM;
            p_type_specifier->enum_specifier = enum_specifier(ctx);
            if (p_type_specifier->enum_specifier == NULL)
            {
                type_specifier_delete(p_type_specifier);
                return NULL;
            }
        }
        else if (ctx->current->type == TK_KEYWORD__BITINT)
        {
            //TODO
            type_specifier_delete(p_type_specifier);
            return NULL;
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

    }
    catch
    {
        type_specifier_delete(p_type_specifier);
        p_type_specifier = NULL;
    }
    return p_type_specifier;
}

enum type_specifier_flags get_enum_type_specifier_flags(const struct enum_specifier* p_enum_specifier)
{
    if (p_enum_specifier->specifier_qualifier_list)
    {
        return p_enum_specifier->specifier_qualifier_list->type_specifier_flags;
    }

    if (p_enum_specifier->p_complete_enum_specifier &&
        p_enum_specifier->p_complete_enum_specifier->specifier_qualifier_list)
    {
        return p_enum_specifier->p_complete_enum_specifier->specifier_qualifier_list->type_specifier_flags;
    }
    else if (p_enum_specifier->p_complete_enum_specifier &&
        p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier &&
        p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier->specifier_qualifier_list)
    {
        return p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier->specifier_qualifier_list->type_specifier_flags;
    }

    return TYPE_SPECIFIER_INT;
}

const struct enum_specifier* _Opt get_complete_enum_specifier(const struct enum_specifier* p_enum_specifier)
{
    /*
      The way cake find the complete struct is using one pass.. for this task is uses double indirection.
      Then the result will be there at end of first pass.
      This crazy code finds the complete definition of the struct if exists.
    */
    if (p_enum_specifier->enumerator_list.head)
    {
        /*p_struct_or_union_specifier is complete*/
        return p_enum_specifier;
    }
    else if (p_enum_specifier->p_complete_enum_specifier &&
        p_enum_specifier->p_complete_enum_specifier->enumerator_list.head)
    {
        /*p_struct_or_union_specifier is the first seem tag tag points directly to complete*/
        return p_enum_specifier->p_complete_enum_specifier;
    }
    else if (p_enum_specifier->p_complete_enum_specifier &&
        p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier &&
        p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier->enumerator_list.head)
    {
        /* all others points to the first seem that points to the complete*/
        return p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier;
    }

    return NULL;
}

struct struct_or_union_specifier* _Opt get_complete_struct_or_union_specifier(const struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    /*
      The way cake find the complete struct is using one pass.. for this task is uses double indirection.
      Then the result will be there at end of first pass.
      This crazy code finds the complete definition of the struct if exists.
    */


    if (p_struct_or_union_specifier->member_declaration_list.head)
    {
        /*p_struct_or_union_specifier is complete*/
        return (struct struct_or_union_specifier* _Opt) p_struct_or_union_specifier;
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

struct struct_or_union_specifier* _Owner struct_or_union_specifier_add_ref(struct struct_or_union_specifier* p)
{
    p->has_shared_ownership = true;
    return (struct struct_or_union_specifier* _Owner) p;
}


void struct_or_union_specifier_sink(struct struct_or_union_specifier* _Owner _Opt p) {}

bool struct_or_union_specifier_is_union(const struct struct_or_union_specifier* p)
{
    return p->first_token->type == TK_KEYWORD_UNION;
}

void struct_or_union_specifier_delete(struct struct_or_union_specifier* _Owner _Opt p)
{
    if (p)
    {
        if (p->has_shared_ownership > 0)
        {
            p->has_shared_ownership = false;
            struct_or_union_specifier_sink(p);
            return;
        }

        member_declaration_list_destroy(&p->member_declaration_list);
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        free(p);
    }
}

struct struct_or_union_specifier* _Owner _Opt struct_or_union_specifier(struct parser_ctx* ctx)
{

    struct struct_or_union_specifier* _Owner _Opt p_struct_or_union_specifier = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_struct_or_union_specifier = calloc(1, sizeof * p_struct_or_union_specifier);
        if (p_struct_or_union_specifier == NULL)
            throw;

        if (ctx->current->type == TK_KEYWORD_STRUCT ||
            ctx->current->type == TK_KEYWORD_UNION)
        {
            p_struct_or_union_specifier->first_token = ctx->current;
            parser_match(ctx);
        }
        else
        {
            throw;
        }

        p_struct_or_union_specifier->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);

        struct struct_or_union_specifier* p_first_tag_in_this_scope = NULL;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == TK_IDENTIFIER)
        {
            p_struct_or_union_specifier->tagtoken = ctx->current;
            /*
             Structure, union, and enumeration tags have scope that begins just after the
             appearance of the tag in a type specifier that declares the tag.
            */

            snprintf(p_struct_or_union_specifier->tag_name, sizeof p_struct_or_union_specifier->tag_name, "%s", ctx->current->lexeme);

            struct map_entry* _Opt p_entry = hashmap_find(&ctx->scopes.tail->tags, ctx->current->lexeme);
            if (p_entry)
            {
                /*this tag already exist in this scope*/
                if (p_entry->type == TAG_TYPE_STRUCT_OR_UNION_SPECIFIER)
                {
                    assert(p_entry->data.p_struct_or_union_specifier != NULL);
                    p_first_tag_in_this_scope = p_entry->data.p_struct_or_union_specifier;
                    p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection = p_first_tag_in_this_scope;
                }
                else
                {
                    compiler_diagnostic(C_ERROR_TAG_TYPE_DOES_NOT_MATCH_PREVIOUS_DECLARATION,
                        ctx,
                        ctx->current, NULL,
                        "use of '%s' with tag type that does not match previous declaration.",
                        ctx->current->lexeme);
                }
            }
            else
            {
                /*tag does not exist in the current scope, let search on upper scopes*/
                struct struct_or_union_specifier* _Opt p_first_tag_previous_scopes = find_struct_or_union_specifier(ctx, ctx->current->lexeme);
                if (p_first_tag_previous_scopes == NULL)
                {
                    /*tag not found, so it is the first appearance*/

                    p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;

                    struct hash_item_set item = { 0 };
                    item.p_struct_or_union_specifier = struct_or_union_specifier_add_ref(p_struct_or_union_specifier);
                    hashmap_set(&ctx->scopes.tail->tags, ctx->current->lexeme, &item);
                    hash_item_set_destroy(&item);
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
            snprintf(p_struct_or_union_specifier->tag_name, sizeof p_struct_or_union_specifier->tag_name, "_struct_tag_%d", s_anonymous_struct_count);
            s_anonymous_struct_count++;
            p_struct_or_union_specifier->has_anonymous_tag = true;
            p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;


            struct hash_item_set item = { 0 };
            item.p_struct_or_union_specifier = struct_or_union_specifier_add_ref(p_struct_or_union_specifier);
            hashmap_set(&ctx->scopes.tail->tags, p_struct_or_union_specifier->tag_name, &item);
            hash_item_set_destroy(&item);
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == '{')
        {
            /*
            this is the complete struct
            */

            struct struct_or_union_specifier* _Opt first = find_struct_or_union_specifier(ctx, p_struct_or_union_specifier->tag_name);

            if (first)
            {
                /*
                   The first tag (will the one at symbol table) will point to the complete struct
                */
                first->complete_struct_or_union_specifier_indirection = p_struct_or_union_specifier;
            }

            if (p_struct_or_union_specifier->tagtoken)
                naming_convention_struct_tag(ctx, p_struct_or_union_specifier->tagtoken);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct token* firsttoken = ctx->current;
            parser_match(ctx);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            if (ctx->current->type != '}') /*not official extensions yet..missing sizeof etc*/
            {
#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"
                p_struct_or_union_specifier->member_declaration_list = member_declaration_list(ctx, p_struct_or_union_specifier);
#pragma cake diagnostic pop

                //we cannot have an empty struct
                if (p_struct_or_union_specifier->member_declaration_list.head == NULL) throw;
            }

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
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

        struct struct_or_union_specifier* _Opt p_complete =
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
                    compiler_diagnostic(W_DEPRECATED, ctx, p_struct_or_union_specifier->first_token, NULL, "'%s' is deprecated", p_struct_or_union_specifier->tagtoken->lexeme);
                }
                else
                {
                    compiler_diagnostic(W_DEPRECATED, ctx, p_struct_or_union_specifier->first_token, NULL, "deprecated");
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

struct member_declarator* _Owner _Opt member_declarator(
    struct parser_ctx* ctx,
    struct struct_or_union_specifier* p_struct_or_union_specifier, /*not const*/
    const struct specifier_qualifier_list* p_specifier_qualifier_list)
{
    /*
    member-declarator:
     declarator
     declaratoropt : constant-expression
    */
    struct member_declarator* _Owner _Opt p_member_declarator = NULL;
    try
    {
        p_member_declarator = calloc(1, sizeof(struct member_declarator));
        if (p_member_declarator == NULL) throw;

        struct token* _Opt p_token_name = NULL;

        p_member_declarator->declarator = declarator(ctx, p_specifier_qualifier_list, /*declaration_specifiers*/ NULL, false, &p_token_name);
        if (p_member_declarator->declarator == NULL) throw;

        p_member_declarator->declarator->name_opt = p_token_name;
        p_member_declarator->declarator->specifier_qualifier_list = p_specifier_qualifier_list;

#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"    
        p_member_declarator->declarator->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
#pragma cake diagnostic pop

        if (type_is_function(&p_member_declarator->declarator->type))
        {
            //A structure or union shall not contain a member with incomplete 
            // or function type 

            struct token* p_token =
                p_member_declarator->declarator->first_token_opt;
            if (p_token == NULL)
                p_token = ctx->current;

            compiler_diagnostic(C_ERROR_FUNCTION_CANNOT_BE_MEMBER,
                ctx,
                p_token,
                NULL,
                "members having a function type are not allowed");

            throw;
        }

        /*extension*/
        if (type_is_owner(&p_member_declarator->declarator->type))
        {
            /*having at least 1 _Owner member, the struct type is _Owner by default*/
            p_struct_or_union_specifier->is_owner = true;
        }

        if (p_member_declarator->declarator->name_opt)
            naming_convention_struct_member(ctx, p_member_declarator->declarator->name_opt, &p_member_declarator->declarator->type);

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == ':')
        {
            parser_match(ctx);
            p_member_declarator->constant_expression = constant_expression(ctx, true);
        }
    }
    catch
    {
        member_declarator_delete(p_member_declarator);
        p_member_declarator = NULL;
    }
    return p_member_declarator;
}

void member_declarator_delete(struct member_declarator* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        assert(p->next == NULL);
        declarator_delete(p->declarator);
        free(p);
    }
}

void member_declarator_list_add(struct member_declarator_list* list, struct member_declarator* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void member_declarator_list_delete(struct member_declarator_list* _Owner _Opt p)
{
    if (p)
    {
        struct member_declarator* _Owner _Opt item = p->head;
        while (item)
        {
            struct member_declarator* _Owner _Opt next = item->next;
            item->next = NULL;
            member_declarator_delete(item);
            item = next;
        }
        free(p);
    }
}

struct member_declarator_list* _Owner _Opt member_declarator_list(
    struct parser_ctx* ctx,
    struct struct_or_union_specifier* p_struct_or_union_specifier,
    const struct specifier_qualifier_list* p_specifier_qualifier_list)
{
    struct member_declarator_list* _Owner _Opt p_member_declarator_list = calloc(1, sizeof(struct member_declarator_list));
    try
    {
        if (p_member_declarator_list == NULL) throw;

        struct member_declarator* _Opt _Owner p_member_declarator = member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list);
        if (p_member_declarator == NULL) throw;

        member_declarator_list_add(p_member_declarator_list, p_member_declarator);

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        while (ctx->current->type == ',')
        {
            parser_match(ctx);

            struct member_declarator* _Opt _Owner p_member_declarator2 = member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list);
            if (p_member_declarator2 == NULL) throw;

            member_declarator_list_add(p_member_declarator_list, p_member_declarator2);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }
        }
    }
    catch
    {
        member_declarator_list_delete(p_member_declarator_list);
        p_member_declarator_list = NULL;
    }

    return p_member_declarator_list;
}

void member_declaration_list_add(struct member_declaration_list* list, struct member_declaration* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void member_declaration_list_destroy(_Opt _Dtor struct member_declaration_list* p)
{
    struct member_declaration* _Owner _Opt item = p->head;
    while (item)
    {
        struct member_declaration* _Owner _Opt next = item->next;
        item->next = NULL;
        member_declaration_delete(item);
        item = next;
    }
}

struct member_declaration_list member_declaration_list(struct parser_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    _Opt struct member_declaration_list list = { 0 };
    // member_declaration
    // member_declaration_list member_declaration

    struct member_declaration* _Owner _Opt p_member_declaration = NULL;

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
        member_declaration_list_destroy(&list);
        list = (struct member_declaration_list){ 0 };
    }

    return list;
}

void member_declaration_delete(struct member_declaration* _Owner _Opt p)
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

struct member_declaration* _Owner _Opt member_declaration(struct parser_ctx* ctx,
    struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    struct member_declaration* _Owner _Opt p_member_declaration = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_member_declaration = calloc(1, sizeof(struct member_declaration));
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

            if (p_member_declaration->specifier_qualifier_list == NULL)
            {
                throw;
            }

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            if (ctx->current->type != ';')
            {
                p_member_declaration->member_declarator_list_opt = member_declarator_list(ctx,
                    p_struct_or_union_specifier,
                    p_member_declaration->specifier_qualifier_list);
                if (p_member_declaration->member_declarator_list_opt == NULL) throw;
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

struct member_declarator* _Opt find_member_declarator(struct member_declaration_list* list,
    const char* name,
    int* p_member_index)
{
    if (list->head == NULL)
        return NULL;

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
                    if (p_member_declarator->declarator->name_opt && strcmp(p_member_declarator->declarator->name_opt->lexeme, name) == 0)
                    {
                        return p_member_declarator;
                    }
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
                struct struct_or_union_specifier* _Opt p_complete =
                    get_complete_struct_or_union_specifier(p_member_declaration->specifier_qualifier_list->struct_or_union_specifier);

                if (p_complete)
                {
                    p_member_declarator = find_member_declarator(&p_complete->member_declaration_list, name, p_member_index);
                    if (p_member_declarator)
                        return p_member_declarator;
                }
            }

        }

        p_member_declaration = p_member_declaration->next;
    }
    return NULL;
}

static struct member_declarator* _Opt find_member_declarator_by_index_core(struct member_declaration_list* list, int member_index, int* p_count)
{
    if (list->head == NULL)
        return NULL;


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
                        return p_member_declarator;
                    }
                }
                (*p_count)++;
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

                p_member_declarator = find_member_declarator_by_index_core(p_member_declaration_list, member_index, p_count);
                if (p_member_declarator)
                    return p_member_declarator;
            }
        }

        p_member_declaration = p_member_declaration->next;
    }
    return NULL;
}

struct member_declarator* _Opt find_member_declarator_by_index(struct member_declaration_list* list, int member_index)
{
    int count = 0;
    return find_member_declarator_by_index_core(list, member_index, &count);
}

static struct object* _Opt find_object_declarator_by_index_core(struct object* p_object0, struct member_declaration_list* list, int member_index, int* p_count)
{
    const struct object* p_object = object_is_reference(p_object0) ? object_get_referenced(p_object0) : p_object0;

    if (list->head == NULL)
        return NULL;

    if (p_object->members == NULL)
    {
        return NULL;
    }

    struct object* _Opt p_member_object = p_object->members;

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
                        return p_member_object;
                    }
                }
                (*p_count)++;
                p_member_declarator = p_member_declarator->next;
                if (p_member_object == NULL)
                {
                    //BUG
                    return NULL;
                }
                p_member_object = p_member_object->next;
            }
        }
        else if (p_member_declaration->specifier_qualifier_list)
        {
            if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct member_declaration_list* p_member_declaration_list =
                    &p_member_declaration->specifier_qualifier_list->struct_or_union_specifier->member_declaration_list;

                struct object* _Opt p_member_object2 = find_object_declarator_by_index_core(p_member_object, p_member_declaration_list, member_index, p_count);
                if (p_member_object2)
                    return p_member_object2;

            }
            p_member_object = p_member_object->next;
        }

        p_member_declaration = p_member_declaration->next;
    }
    return NULL;
}

struct object* _Opt find_object_declarator_by_index(struct object* p_object, struct member_declaration_list* list, int member_index)
{
    int count = 0;
    return find_object_declarator_by_index_core(p_object, list, member_index, &count);
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
        if (p_specifier_qualifier_list->typedef_declarator->name_opt)
            print_item(ss, first, p_specifier_qualifier_list->typedef_declarator->name_opt->lexeme);
    }
    else
    {
        print_type_specifier_flags(ss, first, p_specifier_qualifier_list->type_specifier_flags);
    }
}


void specifier_qualifier_list_add(struct specifier_qualifier_list* list, struct type_specifier_qualifier* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void specifier_qualifier_list_delete(struct specifier_qualifier_list* _Owner _Opt p)
{
    if (p)
    {
        struct type_specifier_qualifier* _Owner _Opt item = p->head;
        while (item)
        {
            struct type_specifier_qualifier* _Owner _Opt next = item->next;
            item->next = NULL;
            type_specifier_qualifier_delete(item);
            item = next;
        }

        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence);

        free(p);
    }
}

struct specifier_qualifier_list* _Owner _Opt specifier_qualifier_list(struct parser_ctx* ctx)
{
    struct specifier_qualifier_list* _Owner _Opt p_specifier_qualifier_list = NULL;
    try
    {
        if (!first_of_type_specifier_qualifier(ctx))
        {
            compiler_diagnostic(C_ERROR_MISSING_ENUM_TAG_NAME, ctx, ctx->current, NULL, "type specifier or qualifier expected");
            throw;
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_specifier_qualifier_list = calloc(1, sizeof(struct specifier_qualifier_list));
        if (p_specifier_qualifier_list == NULL)
            throw;

        /*
          type_specifier_qualifier attribute_specifier_sequence_opt
          type_specifier_qualifier specifier_qualifier_list
        */

        p_specifier_qualifier_list->first_token = ctx->current;

        while (ctx->current != NULL &&
            (first_of_type_specifier_qualifier(ctx)))
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

            struct type_specifier_qualifier* _Owner _Opt p_type_specifier_qualifier = type_specifier_qualifier(ctx);
            if (p_type_specifier_qualifier == NULL)
            {
                throw;
            }

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

            assert(p_specifier_qualifier_list->p_attribute_specifier_sequence == NULL);
            p_specifier_qualifier_list->p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);

            specifier_qualifier_list_add(p_specifier_qualifier_list, p_type_specifier_qualifier);
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        final_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags);
        struct token* _Opt p_previous_parser_token = previous_parser_token(ctx->current);
        if (p_previous_parser_token == NULL) throw;

        p_specifier_qualifier_list->last_token = p_previous_parser_token;
    }
    catch
    {
        specifier_qualifier_list_delete(p_specifier_qualifier_list);
        p_specifier_qualifier_list = NULL;
    }

    return p_specifier_qualifier_list;
}

void type_qualifier_delete(struct type_qualifier* _Owner _Opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        free(p);
    }
}

void type_specifier_qualifier_delete(struct type_specifier_qualifier* _Owner _Opt p)
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

struct type_specifier_qualifier* _Owner _Opt type_specifier_qualifier(struct parser_ctx* ctx)
{
    struct type_specifier_qualifier* _Owner _Opt type_specifier_qualifier = NULL;
    try
    {
        type_specifier_qualifier = calloc(1, sizeof * type_specifier_qualifier);
        if (type_specifier_qualifier == NULL) throw;

        // type_specifier
        // type_qualifier
        // alignment_specifier
        if (first_of_type_specifier(ctx))
        {
            type_specifier_qualifier->type_specifier = type_specifier(ctx);
            if (type_specifier_qualifier->type_specifier == NULL) throw;
        }
        else if (first_of_type_qualifier(ctx))
        {
            type_specifier_qualifier->type_qualifier = type_qualifier(ctx);
            if (type_specifier_qualifier->type_qualifier == NULL) throw;
        }
        else if (first_of_alignment_specifier(ctx))
        {
            type_specifier_qualifier->alignment_specifier = alignment_specifier(ctx);
            if (type_specifier_qualifier->alignment_specifier == NULL) throw;
        }
        else
        {
            assert(false);
        }
    }
    catch
    {
        type_specifier_qualifier_delete(type_specifier_qualifier);
        type_specifier_qualifier = NULL;
    }
    return type_specifier_qualifier;
}

const struct enumerator* _Opt find_enumerator_by_value(const struct enum_specifier* p_enum_specifier, const struct object* object)
{
    if (p_enum_specifier->enumerator_list.head == NULL)
    {
        return NULL;
    }

    struct enumerator* _Opt p = p_enum_specifier->enumerator_list.head;
    while (p)
    {
        if (object_equal(&p->value, object))
            return p;
        p = p->next;
    }
    return NULL;
}

struct enum_specifier* _Owner enum_specifier_add_ref(struct enum_specifier* p)
{
    p->has_shared_ownership = true;
    return (struct enum_specifier* _Owner)p;
}

void enum_specifier_delete_sink(struct enum_specifier* _Owner _Opt p) {}

void enum_specifier_delete(struct enum_specifier* _Owner _Opt p)
{
    if (p)
    {
        if (p->has_shared_ownership > 0)
        {
            p->has_shared_ownership = false;
            enum_specifier_delete_sink(p);
            return;
        }

        specifier_qualifier_list_delete(p->specifier_qualifier_list);
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        enumerator_list_destroy(&p->enumerator_list);
        free(p);
    }
}

bool enum_specifier_has_fixed_underlying_type(const struct enum_specifier* p_enum_specifier)
{
    return p_enum_specifier->specifier_qualifier_list != NULL;
}

struct enum_specifier* _Owner _Opt enum_specifier(struct parser_ctx* ctx)
{
    /*
     enum-type-specifier:
     : specifier-qualifier-list
    */

    /*
        enum-specifier:

        "enum" attribute-specifier-sequence _Opt identifier _Opt enum-type-specifier _Opt
        { enumerator-list }

        "enum" attribute-specifier-sequence _Opt identifier _Opt enum-type-specifier _Opt
        { enumerator-list , }
        enum identifier enum-type-specifier _Opt
    */

    struct enum_specifier* _Owner _Opt p_enum_specifier = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_enum_specifier = calloc(1, sizeof * p_enum_specifier);
        if (p_enum_specifier == NULL)
            throw;

        p_enum_specifier->first_token = ctx->current;
        if (parser_match_tk(ctx, TK_KEYWORD_ENUM) != 0)
            throw;

        p_enum_specifier->attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == ':')
        {
            if (!ctx->inside_generic_association)
            {
                /*C23*/
                parser_match(ctx);
                p_enum_specifier->specifier_qualifier_list = specifier_qualifier_list(ctx);
                if (p_enum_specifier->specifier_qualifier_list == NULL)
                    throw;

                struct type enum_underline_type =
                    make_with_type_specifier_flags(p_enum_specifier->specifier_qualifier_list->type_specifier_flags);

                if (!type_is_integer(&enum_underline_type))
                {
                    type_destroy(&enum_underline_type);
                    compiler_diagnostic(C_ERROR_NON_INTEGRAL_ENUM_TYPE,
                        ctx,
                        p_enum_specifier->specifier_qualifier_list->first_token,
                        NULL,
                        "expected an integer type");

                    throw;
                }
                type_destroy(&enum_underline_type);
            }
            else
            {
            }
        }


        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == '{')
        {
            if (p_enum_specifier->tag_token)
                naming_convention_enum_tag(ctx, p_enum_specifier->tag_token);

            /*points to itself*/
            p_enum_specifier->p_complete_enum_specifier = p_enum_specifier;

            if (parser_match_tk(ctx, '{') != 0)
                throw;

            p_enum_specifier->enumerator_list = enumerator_list(ctx, p_enum_specifier);

            if (p_enum_specifier->enumerator_list.head == NULL)
                throw;

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            if (ctx->current->type == ',')
            {
                parser_match(ctx);
            }

            if (parser_match_tk(ctx, '}') != 0)
                throw;

            struct hash_item_set item = { 0 };
            item.p_enum_specifier = enum_specifier_add_ref(p_enum_specifier);
            hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, &item);
            p_enum_specifier->p_complete_enum_specifier = p_enum_specifier;
            hash_item_set_destroy(&item);
        }
        else
        {
            struct enum_specifier* _Opt p_existing_enum_specifier = find_enum_specifier(ctx, p_enum_specifier->tag_token->lexeme);
            if (p_existing_enum_specifier)
            {
                //p_existing_enum_specifier->p_complete_enum_specifier = p_enum_specifier;
                //ja existe
                //verificar o caso de ser outro tag no memso escopo
                p_enum_specifier->p_complete_enum_specifier = p_existing_enum_specifier;
            }
            else
            {
                //nao existe lugar nenhum vamos adicionar
                struct hash_item_set item = { 0 };
                item.p_enum_specifier = enum_specifier_add_ref(p_enum_specifier);
                hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, &item);
                p_enum_specifier->p_complete_enum_specifier = p_enum_specifier;
                hash_item_set_destroy(&item);
            }

            //if (!has_identifier)
            //{
              //  compiler_diagnostic(C_ERROR_MISSING_ENUM_TAG_NAME, ctx, ctx->current, "missing enum tag name");
                //throw;
            //}
        }
    }
    catch
    {
        enum_specifier_delete(p_enum_specifier);
        p_enum_specifier = NULL;
    }

    return p_enum_specifier;
}

void enumerator_list_add(struct enumerator_list* list, struct enumerator* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void enumerator_list_destroy(_Dtor struct enumerator_list* p)
{
    struct enumerator* _Owner _Opt item = p->head;
    while (item)
    {
        struct enumerator* _Owner _Opt next = item->next;
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


    struct object next_enumerator_value = object_make_signed_int(0);

    if (p_enum_specifier->specifier_qualifier_list)
    {
        enum object_value_type vt = type_specifier_to_object_type(p_enum_specifier->specifier_qualifier_list->type_specifier_flags);
        next_enumerator_value = object_cast(vt, &next_enumerator_value);
    }

    struct enumerator_list enumeratorlist = { 0 };
    struct enumerator* _Owner _Opt p_enumerator = NULL;
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
        enumerator_list_destroy(&enumeratorlist);
        enumeratorlist.head = NULL;
        enumeratorlist.tail = NULL;
    }

    return enumeratorlist;
}

struct enumerator* _Owner enumerator_add_ref(struct enumerator* p)
{
    p->has_shared_ownership = true;
    return (struct enumerator* _Owner) p;
}


void enumerator_sink(struct enumerator* _Owner _Opt p) {}

void enumerator_delete(struct enumerator* _Owner _Opt p)
{
    if (p)
    {
        if (p->has_shared_ownership > 0)
        {
            p->has_shared_ownership = false;
            enumerator_sink(p);
            return;
        }

        assert(p->next == NULL);
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        expression_delete(p->constant_expression_opt);

        free(p);
    }
}

struct enumerator* _Owner _Opt enumerator(struct parser_ctx* ctx,
    const struct enum_specifier* p_enum_specifier,
    struct object* p_next_enumerator_value)
{
    // TODO VALUE    
    struct enumerator* _Owner _Opt p_enumerator = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_enumerator = calloc(1, sizeof(struct enumerator));
        if (p_enumerator == NULL)
            throw;

        p_enumerator->enum_specifier = p_enum_specifier;
        struct token* name = ctx->current;

        naming_convention_enumerator(ctx, name);

        if (parser_match_tk(ctx, TK_IDENTIFIER) != 0)
            throw;

        p_enumerator->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);

        p_enumerator->token = name;

        struct hash_item_set item = { 0 };
        item.p_enumerator = enumerator_add_ref(p_enumerator);
        hashmap_set(&ctx->scopes.tail->variables, p_enumerator->token->lexeme, &item);
        hash_item_set_destroy(&item);

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == '=')
        {
            parser_match(ctx);
            assert(p_enumerator->constant_expression_opt == NULL);
            p_enumerator->constant_expression_opt = constant_expression(ctx, true);
            if (p_enumerator->constant_expression_opt == NULL) throw;

            if (enum_specifier_has_fixed_underlying_type(p_enum_specifier))
            {

            }
            else
            {
                //if the value is bigger than int the enum whould type must be fixed
            }
            p_enumerator->value = p_enumerator->constant_expression_opt->object;

            //fixes #257
            *p_next_enumerator_value = *object_get_referenced(&p_enumerator->value);

            if (object_increment_value(p_next_enumerator_value) != 0)
            {
                //overflow TODO
            }
        }
        else
        {
            p_enumerator->value = *p_next_enumerator_value;
            if (object_increment_value(p_next_enumerator_value) != 0)
            {
                //overflow
            }
        }
    }
    catch
    {
        enumerator_delete(p_enumerator);
        p_enumerator = NULL;
    }
    return p_enumerator;
}

void alignment_specifier_delete(struct alignment_specifier* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        type_name_delete(p->type_name);
        free(p);
    }
}

struct alignment_specifier* _Owner _Opt alignment_specifier(struct parser_ctx* ctx)
{
    struct alignment_specifier* _Owner _Opt alignment_specifier = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        alignment_specifier = calloc(1, sizeof * alignment_specifier);

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

void atomic_type_specifier_delete(struct atomic_type_specifier* _Owner _Opt p)
{
    if (p)
    {
        type_name_delete(p->type_name);
        free(p);
    }
}

struct atomic_type_specifier* _Owner _Opt atomic_type_specifier(struct parser_ctx* ctx)
{
    //'_Atomic' '(' type_name ')'
    struct atomic_type_specifier* _Owner _Opt p_atomic_type_specifier = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_atomic_type_specifier = calloc(1, sizeof * p_atomic_type_specifier);
        if (p_atomic_type_specifier == NULL)
            throw;

        p_atomic_type_specifier->token = ctx->current;
        if (parser_match_tk(ctx, TK_KEYWORD__ATOMIC) != 0)
            throw;
        if (parser_match_tk(ctx, '(') != 0)
            throw;

        struct type_name* _Owner _Opt ptemp = type_name(ctx);
        if (ptemp == NULL)
        {
            throw;
        }

        p_atomic_type_specifier->type_name = ptemp;
        if (parser_match_tk(ctx, ')') != 0)
            throw;
    }
    catch
    {
        atomic_type_specifier_delete(p_atomic_type_specifier);
        p_atomic_type_specifier = NULL;
    }
    return p_atomic_type_specifier;
}


struct type_qualifier* _Owner _Opt type_qualifier(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
    {
        unexpected_end_of_file(ctx);
        return NULL;
    }

    struct type_qualifier* _Owner _Opt p_type_qualifier = calloc(1, sizeof * p_type_qualifier);

    if (p_type_qualifier == NULL)
        return NULL;

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
        case TK_KEYWORD__CTOR:
            p_type_qualifier->flags = TYPE_QUALIFIER_CTOR;
            break;

        case TK_KEYWORD__DTOR:
            p_type_qualifier->flags = TYPE_QUALIFIER_DTOR;
            break;

        case TK_KEYWORD__OWNER:
            p_type_qualifier->flags = TYPE_QUALIFIER_OWNER;
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
            p_type_qualifier->flags = TYPE_QUALIFIER_OPT;
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

struct type_qualifier* _Owner _Opt type_qualifier_opt(struct parser_ctx* ctx)
{
    if (first_of_type_qualifier(ctx))
    {
        return type_qualifier(ctx);
    }
    return NULL;
}

void function_specifier_delete(struct function_specifier* _Owner _Opt p)
{
    if (p)
    {
        free(p);
    }
}

struct function_specifier* _Owner _Opt function_specifier(struct parser_ctx* ctx)
{
    struct function_specifier* _Owner _Opt p_function_specifier = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }


        p_function_specifier = calloc(1, sizeof * p_function_specifier);
        if (p_function_specifier == NULL)
            throw;

        if (ctx->current->type == TK_KEYWORD__NORETURN)
        {
            compiler_diagnostic(W_STYLE, ctx, ctx->current, NULL, "_Noreturn is deprecated use attributes");
        }

        if (ctx->current->type == TK_KEYWORD_INLINE)
        {
            p_function_specifier->flags |= FUNCTION_SPECIFIER_INLINE;
        }

        p_function_specifier->token = ctx->current;
        parser_match(ctx);
    }
    catch
    {
        function_specifier_delete(p_function_specifier);
        p_function_specifier = NULL;
    }

    return p_function_specifier;
}

struct declarator* _Owner declarator_add_ref(struct declarator* p)
{
    p->has_shared_ownership = true;
    return (struct declarator* _Owner)p;
}

void declarator_sink(struct declarator* _Owner _Opt p) {}

void declarator_delete(struct declarator* _Owner _Opt p)
{
    if (p)
    {
        if (p->has_shared_ownership > 0)
        {
            p->has_shared_ownership = false;
            declarator_sink(p);
            return;
        }

        type_destroy(&p->type);
        direct_declarator_delete(p->direct_declarator);
        pointer_delete(p->pointer);
        free(p);
    }
}

struct declarator* _Owner _Opt declarator(struct parser_ctx* ctx,
    const struct specifier_qualifier_list* _Opt p_specifier_qualifier_list_opt,
    struct declaration_specifiers* _Opt p_declaration_specifiers_opt,
    bool abstract_acceptable,
    struct token** _Opt pp_token_name_opt)
{
    /*
      declarator:
      pointer_opt direct-declarator
    */

    struct declarator* _Owner _Opt p_declarator = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_declarator = calloc(1, sizeof(struct declarator));
        if (p_declarator == NULL) throw;

        p_declarator->first_token_opt = ctx->current;
        p_declarator->pointer = pointer_opt(ctx);
        p_declarator->direct_declarator = direct_declarator(ctx, p_specifier_qualifier_list_opt, p_declaration_specifiers_opt, abstract_acceptable, pp_token_name_opt);
        if (p_declarator->direct_declarator == NULL) throw;

        if (pp_token_name_opt && *pp_token_name_opt)
        {
            free((void*)p_declarator->object.debug_name);
            p_declarator->object.debug_name = strdup((*pp_token_name_opt)->lexeme);
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current != p_declarator->first_token_opt)
        {
            p_declarator->last_token_opt = previous_parser_token(ctx->current);
        }
        else
        {
            /*empty declarator*/
            p_declarator->last_token_opt = p_declarator->first_token_opt;
            p_declarator->first_token_opt = NULL; /*this is the way we can know...first is null*/
        }
    }
    catch
    {
        declarator_delete(p_declarator);
        p_declarator = NULL;
    }


    return p_declarator;
}

const char* _Opt declarator_get_name(struct declarator* p_declarator)
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

struct array_declarator* _Owner _Opt array_declarator(struct direct_declarator* _Owner p_direct_declarator, struct parser_ctx* ctx);
struct function_declarator* _Owner _Opt function_declarator(struct direct_declarator* _Owner p_direct_declarator, struct parser_ctx* ctx);

void function_declarator_delete(struct function_declarator* _Owner _Opt p)
{
    if (p)
    {
        direct_declarator_delete(p->direct_declarator);
        scope_destroy(&p->parameters_scope);
        parameter_type_list_delete(p->parameter_type_list_opt);
        free(p);
    }
}

void direct_declarator_delete(struct direct_declarator* _Owner _Opt p)
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

struct direct_declarator* _Owner _Opt direct_declarator(struct parser_ctx* ctx,
    const struct specifier_qualifier_list* _Opt p_specifier_qualifier_list,
    struct declaration_specifiers* _Opt p_declaration_specifiers,
    bool abstract_acceptable,
    struct token** _Opt pp_token_name_opt)
{
    /*
    direct-declarator:
     identifier attribute-specifier-sequence opt
     ( declarator )

     array-declarator attribute-specifier-sequence opt
     function-declarator attribute-specifier-sequence opt
    */


    struct direct_declarator* _Owner _Opt p_direct_declarator = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_direct_declarator = calloc(1, sizeof(struct direct_declarator));
        if (p_direct_declarator == NULL) throw;

        struct token* _Opt p_token_ahead = parser_look_ahead(ctx);
        if (p_token_ahead == NULL)
            throw;

        if (ctx->current->type == TK_KEYWORD__STDCALL ||
            ctx->current->type == TK_KEYWORD__CDECL ||
            ctx->current->type == TK_KEYWORD__FASTCALL)
        {
            /*
              int __fastcall add(int a, int b);
            */
            p_direct_declarator->p_calling_convention = ctx->current;
            parser_match(ctx);
        }

        if (ctx->current->type == TK_IDENTIFIER)
        {
            p_direct_declarator->name_opt = ctx->current;
            if (pp_token_name_opt != NULL)
            {
                *pp_token_name_opt = ctx->current;
            }

            parser_match(ctx);
            p_direct_declarator->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        }
        else if (ctx->current->type == '(')
        {
            struct token* _Opt ahead = parser_look_ahead(ctx);
            if (ahead == NULL)
                throw;

            if (!first_of_type_specifier_token(ctx, p_token_ahead) &&
                !first_of_type_qualifier_token(p_token_ahead) &&
                ahead->type != ')' &&
                ahead->type != '...')
            {
                // look ahead para nao confundir (declarator) com parametros funcao ex void (int)
                // or function int ()

                parser_match(ctx);

                struct declarator* _Owner _Opt p_declarator_temp =
                    declarator(ctx,
                        p_specifier_qualifier_list,
                        p_declaration_specifiers,
                        abstract_acceptable,
                        pp_token_name_opt);

                if (p_declarator_temp == NULL)
                    throw;

                p_direct_declarator->declarator = p_declarator_temp;
                parser_match(ctx); //)
            }
        }

        while (ctx->current != NULL &&
            (ctx->current->type == '[' || ctx->current->type == '('))
        {
            struct direct_declarator* _Owner _Opt p_direct_declarator2 = calloc(1, sizeof(struct direct_declarator));
            if (p_direct_declarator2 == NULL) throw;

            if (ctx->current->type == '[')
            {
                p_direct_declarator2->array_declarator = array_declarator(p_direct_declarator, ctx);
                p_direct_declarator = NULL; //MOVED
                if (p_direct_declarator2->array_declarator == NULL)
                {
                    direct_declarator_delete(p_direct_declarator2);
                    throw;
                }
            }
            else
            {
                p_direct_declarator2->function_declarator = function_declarator(p_direct_declarator, ctx);
                p_direct_declarator = NULL; //MOVED
                if (p_direct_declarator2->function_declarator == NULL)
                {
                    direct_declarator_delete(p_direct_declarator2);
                    throw;
                }
            }
            p_direct_declarator = p_direct_declarator2;
        }
    }
    catch
    {
        direct_declarator_delete(p_direct_declarator);
        p_direct_declarator = NULL;
    }

    return p_direct_declarator;
}

void array_declarator_delete(struct array_declarator* _Owner _Opt p)
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

size_t array_declarator_get_size(const struct array_declarator* p_array_declarator)
{
    if (p_array_declarator->assignment_expression)
    {
        if (object_has_constant_value(&p_array_declarator->assignment_expression->object))
        {
            return (size_t)object_to_unsigned_long_long(&p_array_declarator->assignment_expression->object);
        }
    }
    return 0;
}

struct array_declarator* _Owner _Opt array_declarator(struct direct_declarator* _Owner p_direct_declarator, struct parser_ctx* ctx)
{
    // direct_declarator '['          type_qualifier_list_opt           assignment_expression_opt ']'
    // direct_declarator '[' 'static' type_qualifier_list_opt           assignment_expression     ']'
    // direct_declarator '['          type_qualifier_list      'static' assignment_expression     ']'
    // direct_declarator '['          type_qualifier_list_opt  '*'           ']'

    struct array_declarator* _Owner _Opt p_array_declarator = NULL;
    try
    {
        p_array_declarator = calloc(1, sizeof * p_array_declarator);
        if (p_array_declarator == NULL)
        {
            throw;
        }

        p_array_declarator->direct_declarator = p_direct_declarator;
        p_direct_declarator = NULL; /*MOVED*/

        if (parser_match_tk(ctx, '[') != 0)
            throw;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
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
            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            // optional
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
        array_declarator_delete(p_array_declarator);
        p_array_declarator = NULL;

    }

    direct_declarator_delete(p_direct_declarator);

    return p_array_declarator;
}

struct function_declarator* _Owner _Opt function_declarator(struct direct_declarator* _Owner p_direct_declarator_arg, struct parser_ctx* ctx)
{
    struct direct_declarator* _Owner _Opt p_direct_declarator = p_direct_declarator_arg; //MOVED

    struct function_declarator* _Owner _Opt p_function_declarator = calloc(1, sizeof(struct function_declarator));
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

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type != ')')
        {
            scope_list_push(&ctx->scopes, &p_function_declarator->parameters_scope);
            p_function_declarator->parameter_type_list_opt = parameter_type_list(ctx);
            scope_list_pop(&ctx->scopes);
            if (p_function_declarator->parameter_type_list_opt == NULL)
                throw;
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

void pointer_delete(struct pointer* _Owner _Opt p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        type_qualifier_list_delete(p->type_qualifier_list_opt);

        struct pointer* _Owner _Opt item = p->pointer;
        while (item)
        {
            struct pointer* _Owner _Opt next = item->pointer;
            attribute_specifier_sequence_delete(item->attribute_specifier_sequence_opt);
            type_qualifier_list_delete(item->type_qualifier_list_opt);
            free(item);
            item = next;
        }

        free(p);
    }
}

struct pointer* _Owner _Opt pointer_opt(struct parser_ctx* ctx)
{
    struct pointer* _Owner _Opt p = NULL;
    struct pointer* _Owner _Opt p_pointer = NULL;
    try
    {
        struct token* _Opt calling_convention = NULL;
        struct token* _Opt ahead = parser_look_ahead(ctx);
        if (ahead != NULL && ahead->type == '*')
        {
            /*
              typedef void (__fastcall *pf)();
            */
            if (ctx->current->type == TK_KEYWORD__STDCALL ||
                ctx->current->type == TK_KEYWORD__CDECL ||
                ctx->current->type == TK_KEYWORD__FASTCALL)
            {
                calling_convention = ctx->current;
                parser_match(ctx);
            }
        }

        while (ctx->current != NULL && ctx->current->type == '*')
        {
            p_pointer = calloc(1, sizeof(struct pointer));
            if (p_pointer == NULL)
                throw;

            p_pointer->calling_convention = calling_convention;
            calling_convention = NULL;

            p = p_pointer;
            parser_match(ctx);

            p_pointer->attribute_specifier_sequence_opt =
                attribute_specifier_sequence_opt(ctx);

            if (first_of_type_qualifier(ctx))
            {
                assert(ctx->current != NULL);

                if (ctx->current->type == TK_KEYWORD__VIEW)
                {
                    compiler_diagnostic(C_ERROR_INVALID_QUALIFIER_FOR_POINTER,
                                ctx,
                                ctx->current,
                                NULL,
                                "invalid qualifier for pointer");
                }
                else
                {
                    p_pointer->type_qualifier_list_opt = type_qualifier_list(ctx);
                }
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

void type_qualifier_list_add(struct type_qualifier_list* list, struct type_qualifier* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void type_qualifier_list_delete(struct type_qualifier_list* _Owner _Opt p)
{
    if (p)
    {
        struct type_qualifier* _Owner _Opt item = p->head;
        while (item)
        {
            struct type_qualifier* _Owner _Opt next = item->next;
            item->next = NULL;
            free(item);
            item = next;
        }
        free(p);
    }
}

struct type_qualifier_list* _Owner _Opt type_qualifier_list(struct parser_ctx* ctx)
{
    // type_qualifier
    // type_qualifier_list type_qualifier

    struct type_qualifier_list* _Owner _Opt p_type_qualifier_list = NULL;
    struct type_qualifier* _Owner _Opt p_type_qualifier = NULL;

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

void parameter_type_list_delete(struct parameter_type_list* _Owner _Opt p)
{
    if (p)
    {
        parameter_list_delete(p->parameter_list);
        free(p);
    }
}

struct parameter_type_list* _Owner _Opt parameter_type_list(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return NULL;

    struct parameter_type_list* _Owner _Opt p_parameter_type_list = calloc(1, sizeof(struct parameter_type_list));

    try
    {
        if (p_parameter_type_list == NULL)
            throw;

        // parameter_list
        // parameter_list ',' '...'
        p_parameter_type_list->parameter_list = parameter_list(ctx);
        if (p_parameter_type_list->parameter_list == NULL) throw;

        if (p_parameter_type_list->parameter_list->head ==
            p_parameter_type_list->parameter_list->tail)
        {
            if (type_is_void(&p_parameter_type_list->parameter_list->head->declarator->type))
            {
                p_parameter_type_list->is_void = true;
            }
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        /*ja esta saindo com a virgula consumida do parameter_list para evitar ahead*/
        if (ctx->current->type == '...')
        {
            parser_match(ctx);
            // parser_match_tk(ctx, '...');
            p_parameter_type_list->is_var_args = true;
        }
    }
    catch
    {
        parameter_type_list_delete(p_parameter_type_list);
        p_parameter_type_list = NULL;
    }

    return p_parameter_type_list;
}

void parameter_list_add(struct parameter_list* list, struct parameter_declaration* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void parameter_list_delete(struct parameter_list* _Owner _Opt p)
{
    if (p)
    {
        struct parameter_declaration* _Owner _Opt item = p->head;
        while (item)
        {
            struct parameter_declaration* _Owner _Opt next = item->next;
            item->next = NULL;
            parameter_declaration_delete(item);
            item = next;
        }
        free(p);
    }
}

struct parameter_list* _Owner _Opt parameter_list(struct parser_ctx* ctx)
{
    /*
      parameter_list
      parameter_declaration
      parameter_list ',' parameter_declaration
    */
    struct parameter_list* _Owner _Opt p_parameter_list = NULL;
    struct parameter_declaration* _Owner _Opt p_parameter_declaration = NULL;
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

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

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
        parameter_list_delete(p_parameter_list);
        p_parameter_list = NULL;
    }
    return p_parameter_list;
}

void parameter_declaration_delete(struct parameter_declaration* _Owner _Opt p)
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

struct parameter_declaration* _Owner _Opt parameter_declaration(struct parser_ctx* ctx)
{


    struct parameter_declaration* _Owner _Opt p_parameter_declaration = calloc(1, sizeof(struct parameter_declaration));
    try
    {
        if (p_parameter_declaration == NULL)
            throw;

        p_parameter_declaration->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);

        struct declaration_specifiers* _Owner _Opt p_declaration_specifiers =
            declaration_specifiers(ctx, STORAGE_SPECIFIER_PARAMETER | STORAGE_SPECIFIER_AUTOMATIC_STORAGE);

        if (p_declaration_specifiers == NULL)
        {
            throw;
        }

        if (p_parameter_declaration->attribute_specifier_sequence_opt)
        {
            if (p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags & CAKE_ATTRIBUTE_CTOR)
            {
                p_declaration_specifiers->type_qualifier_flags |= TYPE_QUALIFIER_CTOR;
            }
            else if (p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags & CAKE_ATTRIBUTE_DTOR)
            {
                p_declaration_specifiers->type_qualifier_flags |= TYPE_QUALIFIER_DTOR;
            }
        }
        p_parameter_declaration->declaration_specifiers = p_declaration_specifiers;


        struct token* _Opt p_token_name = NULL;

        p_parameter_declaration->declarator = declarator(ctx,
            /*specifier_qualifier_list*/ NULL,
            p_parameter_declaration->declaration_specifiers,
            true /*can be abstract*/,
            &p_token_name);

        if (p_parameter_declaration->declarator == NULL) throw;

        p_parameter_declaration->declarator->name_opt = p_token_name;
        p_parameter_declaration->declarator->declaration_specifiers = p_parameter_declaration->declaration_specifiers;

#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"        
        p_parameter_declaration->declarator->type = make_type_using_declarator(ctx, p_parameter_declaration->declarator);
#pragma cake diagnostic pop

        if (p_parameter_declaration->declarator->type.storage_class_specifier_flags & STORAGE_SPECIFIER_TYPEDEF)
        {
        }
        else
        {
            int er = make_object(&p_parameter_declaration->declarator->type, &p_parameter_declaration->declarator->object);
            if (er != 0)
            {
                //compiler_diagnostic(C_ERROR_STRUCT_IS_INCOMPLETE, ctx, p_init_declarator->p_declarator->first_token_opt, NULL, "incomplete struct/union type");
                throw;
            }
        }

        if (p_parameter_declaration->declarator->name_opt)
        {
            free((void*)p_parameter_declaration->declarator->object.debug_name);
            p_parameter_declaration->declarator->object.debug_name = strdup(p_parameter_declaration->declarator->name_opt->lexeme);
        }

        object_set_any(&p_parameter_declaration->declarator->object);


        if (p_parameter_declaration->attribute_specifier_sequence_opt)
        {
            p_parameter_declaration->declarator->type.attributes_flags |=
                p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags;
        }

        p_parameter_declaration->declarator->type.storage_class_specifier_flags |= STORAGE_SPECIFIER_PARAMETER;

        if (p_parameter_declaration->declarator->name_opt)
            naming_convention_parameter(ctx, p_parameter_declaration->declarator->name_opt, &p_parameter_declaration->declarator->type);

        // coloca o pametro no escpo atual que deve apontar para escopo paramtros
        //  da funcao .
        //
        // assert ctx->current_scope->variables parametrosd
        if (p_parameter_declaration->declarator->name_opt)
        {
            struct hash_item_set item = { 0 };
            item.p_declarator = declarator_add_ref(p_parameter_declaration->declarator);

            assert(p_parameter_declaration->declarator->name_opt != NULL); //add_ref will not change that

            // parametro void nao te name
            hashmap_set(&ctx->scopes.tail->variables,
                p_parameter_declaration->declarator->name_opt->lexeme,
                &item);

            // print_scope(ctx->current_scope);
            hash_item_set_destroy(&item);
        }
    }
    catch
    {
        parameter_declaration_delete(p_parameter_declaration);
        p_parameter_declaration = NULL;
    }

    return p_parameter_declaration;
}

struct specifier_qualifier_list* _Owner _Opt copy(struct declaration_specifiers* p_declaration_specifiers)
{
    struct specifier_qualifier_list* _Owner _Opt p_specifier_qualifier_list = calloc(1, sizeof(struct specifier_qualifier_list));

    try
    {
        if (p_specifier_qualifier_list == NULL)
            throw;

        p_specifier_qualifier_list->type_qualifier_flags = p_declaration_specifiers->type_qualifier_flags;
        p_specifier_qualifier_list->type_specifier_flags = p_declaration_specifiers->type_specifier_flags;

        struct declaration_specifier* _Opt p_declaration_specifier = p_declaration_specifiers->head;

        while (p_declaration_specifier)
        {
            if (p_declaration_specifier->type_specifier_qualifier)
            {
                struct type_specifier_qualifier* _Owner _Opt p_specifier_qualifier = calloc(1, sizeof(struct type_specifier_qualifier));

                if (p_specifier_qualifier == NULL)
                {
                    throw;
                }

                if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                {
                    struct type_qualifier* _Owner _Opt p_type_qualifier = calloc(1, sizeof(struct type_qualifier));
                    if (p_type_qualifier == NULL)
                    {
                        type_specifier_qualifier_delete(p_specifier_qualifier);
                        throw;
                    }

                    p_type_qualifier->flags = p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;

                    p_type_qualifier->token = p_declaration_specifier->type_specifier_qualifier->type_qualifier->token;
                    p_specifier_qualifier->type_qualifier = p_type_qualifier;
                }
                else if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                {
                    struct type_specifier* _Owner _Opt p_type_specifier = calloc(1, sizeof(struct type_specifier));
                    if (p_type_specifier == NULL)
                    {
                        type_specifier_qualifier_delete(p_specifier_qualifier);
                        throw;
                    }

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
    }
    catch
    {
        specifier_qualifier_list_delete(p_specifier_qualifier_list);
        p_specifier_qualifier_list = NULL;
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
        assert(p_direct_declarator->function_declarator->direct_declarator != NULL);

        print_direct_declarator(ss, p_direct_declarator->function_declarator->direct_declarator, is_abstract);

        ss_fprintf(ss, "(");
        struct parameter_declaration* _Opt p_parameter_declaration =
            p_direct_declarator->function_declarator->parameter_type_list_opt ? p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head : NULL;

        while (p_parameter_declaration)
        {
            if (p_parameter_declaration != p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head)
                ss_fprintf(ss, ",");

            print_declaration_specifiers(ss, p_parameter_declaration->declaration_specifiers);
            ss_fprintf(ss, " ");

            if (p_parameter_declaration->declarator)
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
        struct pointer* _Opt p = p_declarator->pointer;
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
    if (p_declarator->direct_declarator)
        print_direct_declarator(ss, p_declarator->direct_declarator, is_abstract);
}

void print_type_name(struct osstream* ss, struct type_name* p)
{
    bool first = true;
    print_specifier_qualifier_list(ss, &first, p->specifier_qualifier_list);
    print_declarator(ss, p->abstract_declarator, true);
}

void type_name_delete(struct type_name* _Owner _Opt p)
{
    if (p)
    {
        specifier_qualifier_list_delete(p->specifier_qualifier_list);
        type_destroy(&p->type);
        declarator_delete(p->abstract_declarator);
        free(p);
    }
}

struct type_name* _Owner _Opt type_name(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return NULL;

    struct type_name* _Owner _Opt p_type_name = calloc(1, sizeof(struct type_name));
    try
    {
        if (p_type_name == NULL) throw;

        p_type_name->first_token = ctx->current;

        struct specifier_qualifier_list* _Owner _Opt p_specifier_qualifier_list =
            specifier_qualifier_list(ctx);

        if (p_specifier_qualifier_list == NULL)
        {
            throw;
        }

        p_type_name->specifier_qualifier_list = p_specifier_qualifier_list;

        struct declarator* _Owner _Opt p_declarator = declarator(ctx,
            p_type_name->specifier_qualifier_list, //??
            /*declaration_specifiers*/ NULL,
            true /*DEVE SER TODO*/,
            NULL);

        if (p_declarator == NULL)
            throw;

        p_type_name->abstract_declarator = p_declarator;

        p_type_name->abstract_declarator->specifier_qualifier_list = p_type_name->specifier_qualifier_list;

#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"    
        p_type_name->abstract_declarator->type = make_type_using_declarator(ctx, p_type_name->abstract_declarator);
#pragma cake diagnostic pop

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->prev == NULL)
            throw;

        p_type_name->last_token = ctx->current->prev;
        p_type_name->type = type_dup(&p_type_name->abstract_declarator->type);

    }
    catch
    {
        type_name_delete(p_type_name);
        p_type_name = NULL;
    }

    return p_type_name;
}

void braced_initializer_delete(struct braced_initializer* _Owner _Opt p)
{
    if (p)
    {
        initializer_list_delete(p->initializer_list);
        free(p);
    }
}

struct braced_initializer* _Owner _Opt braced_initializer(struct parser_ctx* ctx)
{
    /*
     { }
     { initializer-list }
     { initializer-list , }
    */

    struct braced_initializer* _Owner _Opt p_bracket_initializer_list = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_bracket_initializer_list = calloc(1, sizeof(struct braced_initializer));

        if (p_bracket_initializer_list == NULL)
            throw;

        p_bracket_initializer_list->first_token = ctx->current;
        if (parser_match_tk(ctx, '{') != 0)
            throw;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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

void initializer_delete(struct initializer* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->assignment_expression);
        braced_initializer_delete(p->braced_initializer);
        free(p);
    }
}

struct initializer* _Owner _Opt initializer(struct parser_ctx* ctx)
{
    /*
    initializer:
      assignment-expression
      braced-initializer
    */

    struct initializer* _Owner _Opt p_initializer = NULL;

    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_initializer = calloc(1, sizeof(struct initializer));
        if (p_initializer == NULL)
            throw;

        p_initializer->first_token = ctx->current;

        if (ctx->current->type == '{')
        {
            p_initializer->braced_initializer = braced_initializer(ctx);
            if (p_initializer->braced_initializer == NULL) throw;
        }
        else
        {
            p_initializer->assignment_expression = assignment_expression(ctx);
            if (p_initializer->assignment_expression == NULL) throw;
        }
    }
    catch
    {
        initializer_delete(p_initializer);
        p_initializer = NULL;
    }
    return p_initializer;
}

void defer_list_add(struct defer_list* list, struct defer_list_item* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void defer_list_destroy(_Dtor struct defer_list* p)
{

    struct defer_list_item* _Owner _Opt item = p->head;
    while (item)
    {
        struct defer_list_item* _Owner _Opt next = item->next;
        item->next = NULL;
        free(item);
        item = next;
    }
    free(p);

}


void initializer_list_add(struct initializer_list* list, struct initializer_list_item* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void initializer_list_item_delete(struct initializer_list_item* _Owner _Opt p)
{
    if (p)
    {
        designation_delete(p->designation);
        initializer_delete(p->initializer);
        free(p);
    }
}

void initializer_list_delete(struct initializer_list* _Owner _Opt p)
{
    if (p)
    {
        struct initializer_list_item* _Owner _Opt item = p->head;
        while (item)
        {
            struct initializer_list_item* _Owner _Opt next = item->next;
            item->next = NULL;
            initializer_list_item_delete(item);
            item = next;
        }
        free(p);
    }
}
struct initializer_list* _Owner _Opt initializer_list(struct parser_ctx* ctx)
{
    /*
    initializer-list:
       designation _Opt initializer
       initializer-list , designation _Opt initializer
    */

    struct initializer_list* _Owner _Opt p_initializer_list = NULL;

    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            return NULL;
        }


        p_initializer_list = calloc(1, sizeof(struct initializer_list));
        if (p_initializer_list == NULL)
            throw;

        p_initializer_list->first_token = ctx->current;


        struct initializer_list_item* _Owner _Opt p_initializer_list_item = calloc(1, sizeof * p_initializer_list_item);
        if (p_initializer_list_item == NULL)
            throw;


        struct designation* _Owner _Opt p_designation = NULL;
        if (first_of_designator(ctx))
        {
            p_designation = designation(ctx);
            if (p_designation == NULL)
                throw;
            p_initializer_list_item->designation = p_designation;
        }

        struct initializer* _Owner _Opt p_initializer = initializer(ctx);

        if (p_initializer == NULL)
        {
            designation_delete(p_designation);
            throw;
        }

        p_initializer_list_item->initializer = p_initializer;


        initializer_list_add(p_initializer_list, p_initializer_list_item);
        p_initializer_list_item = NULL; //MOVED
        //p_initializer_list->size++;

        while (ctx->current != NULL && ctx->current->type == ',')
        {
            parser_match(ctx);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            if (ctx->current->type == '}')
                break; // follow

            p_initializer_list_item = calloc(1, sizeof * p_initializer_list_item);
            if (p_initializer_list_item == NULL)
                throw;

            struct designation* _Owner _Opt p_designation2 = NULL;
            if (first_of_designator(ctx))
            {
                p_designation2 = designation(ctx);
                if (p_designation2 == NULL)
                    throw;
            }
            p_initializer_list_item->designation = p_designation2;

            struct initializer* _Owner _Opt p_initializer2 = initializer(ctx);
            if (p_initializer2 == NULL)
            {
                designation_delete(p_designation2);
                throw;
            }
            p_initializer_list_item->initializer = p_initializer2;

            initializer_list_add(p_initializer_list, p_initializer_list_item);
            p_initializer_list_item = NULL; //MOVED
            p_initializer_list->size++;
        }
    }
    catch
    {
        initializer_list_delete(p_initializer_list);
        p_initializer_list = NULL;
    }
    return p_initializer_list;
}

void designation_delete(struct designation* _Owner _Opt p)
{
    if (p)
    {
        designator_list_delete(p->designator_list);
        free(p);
    }
}

struct designation* _Owner _Opt designation(struct parser_ctx* ctx)
{
    // designator_list '='
    struct designation* _Owner _Opt p_designation = NULL;
    try
    {
        p_designation = calloc(1, sizeof(struct designation));
        if (p_designation == NULL)
            throw;

        struct designator_list* _Owner _Opt p_designator_list = designator_list(ctx);

        if (p_designator_list == NULL)
            throw;

        p_designation->designator_list = p_designator_list;

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


void designator_list_add(struct designator_list* list, struct designator* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void designator_list_delete(struct designator_list* _Owner _Opt p)
{
    if (p)
    {
        struct designator* _Owner _Opt  item = p->head;
        while (item)
        {
            struct designator* _Owner _Opt next = item->next;
            item->next = NULL;
            designator_delete(item);
            item = next;
        }
        free(p);
    }
}

struct designator_list* _Owner _Opt designator_list(struct parser_ctx* ctx)
{
    // designator
    // designator_list designator
    struct designator_list* _Owner _Opt p_designator_list = NULL;
    struct designator* _Owner _Opt p_designator = NULL;
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
        designator_list_delete(p_designator_list);
        p_designator_list = NULL;
    }

    return p_designator_list;
}

void designator_delete(struct designator* _Owner _Opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        expression_delete(p->constant_expression_opt);
        free(p);
    }
}

struct designator* _Owner _Opt designator(struct parser_ctx* ctx)
{
    //'[' constant_expression ']'
    //'.' identifier
    struct designator* _Owner _Opt p_designator = NULL;

    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_designator = calloc(1, sizeof(struct designator));
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

            p_designator->token = ctx->current;
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

void static_assert_declaration_delete(struct static_assert_declaration* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        free(p);
    }
}

void pragma_declaration_delete(struct pragma_declaration* _Owner _Opt p)
{
    if (p)
    {
        free(p);
    }
}

void execute_pragma(struct parser_ctx* ctx, struct pragma_declaration* p_pragma, bool on_flow_analysis)
{
    struct token* _Opt p_pragma_token = p_pragma->first_token;

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
            if (ctx->options.diagnostic_stack.top_index <
                sizeof(ctx->options.diagnostic_stack) / sizeof(ctx->options.diagnostic_stack.stack[0]))
            {
                ctx->options.diagnostic_stack.top_index++;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] =
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index - 1];
            }
            p_pragma_token = p_pragma_token->next;
        }
        else if (p_pragma_token && strcmp(p_pragma_token->lexeme, "pop") == 0)
        {
            // #pragma CAKE diagnostic pop
            if (ctx->options.diagnostic_stack.top_index > 0)
            {
                ctx->options.diagnostic_stack.top_index--;
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

                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;

                if (is_error)
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors |= w;
                else if (is_warning)
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
                else if (is_note)
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes |= w;
            }
        }
        else if (p_pragma_token &&
            (strcmp(p_pragma_token->lexeme, "check") == 0))
        {
            p_pragma_token = pragma_match(p_pragma_token);

            if (p_pragma_token && p_pragma_token->type == TK_STRING_LITERAL)
            {
                enum diagnostic_id id = get_warning(p_pragma_token->lexeme + 1);
                //warnings errors are removed on demand..

                if ((!on_flow_analysis && get_diagnostic_phase(id) != 2) ||
                    (on_flow_analysis && get_diagnostic_phase(id) == 2))
                {
                    bool found = false;
                    for (int i = 0;
                         i < (int)(sizeof(ctx->p_report->last_diagnostics_ids) / sizeof(ctx->p_report->last_diagnostics_ids[0]));
                         i++)
                    {
                        if (ctx->p_report->last_diagnostics_ids[i] == 0) break;

                        if (ctx->p_report->last_diagnostics_ids[i] == id)
                        {
                            // lets remove this error/warning/info from the final report.
                            found = true;
                            int t =
                                get_diagnostic_type(&ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index],
                                    id);
                            if (t == 3)
                                ctx->p_report->error_count--;
                            else if (t == 2)
                                ctx->p_report->warnings_count--;
                            else if (t == 1)
                                ctx->p_report->info_count--;

                            break;
                        }
                    }

                    if (!found)
                    {
                        //is fatal error?
                        //fatal errors are kept here and checked at end
                        ctx->p_report->fatal_error_expected = atoi(p_pragma_token->lexeme + 3);

                        compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, p_pragma_token, NULL, "pragma check failed");
                    }
                }
            }
        }
        else
        {
            compiler_diagnostic(C_ERROR_UNEXPECTED, ctx, p_pragma_token, NULL, "unknown pragma");
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
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;

            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
            ctx->options.null_checks_enabled = true;
            ctx->options.flow_analysis = true; //also enable flow analysis
        }
        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
        {
            unsigned long long w = NULLABLE_DISABLE_REMOVED_WARNINGS;

            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;


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

            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;

            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;

            ctx->options.ownership_enabled = true;
            ctx->options.flow_analysis = true; //also enable flow analysis

        }
        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
        {
            unsigned long long w = OWNERSHIP_DISABLE_REMOVED_WARNINGS;

            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;

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
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;

            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
            ctx->options.null_checks_enabled = true;
            ctx->options.flow_analysis = true; //also enable flow analysis

            ctx->options.ownership_enabled = true;
        }
        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
        {
            unsigned long long w = NULLABLE_DISABLE_REMOVED_WARNINGS | OWNERSHIP_DISABLE_REMOVED_WARNINGS;

            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;

            ctx->options.null_checks_enabled = false;
            ctx->options.ownership_enabled = false;
            ctx->options.flow_analysis = false;
        }
    }
}

struct pragma_declaration* _Owner _Opt pragma_declaration(struct parser_ctx* ctx)
{
    struct pragma_declaration* _Owner _Opt p_pragma_declaration = NULL;
    try
    {
        if (ctx->current == NULL || ctx->current->type != TK_PRAGMA)
        {
            assert(false);
            throw;
        }

        p_pragma_declaration = calloc(1, sizeof(struct pragma_declaration));
        if (p_pragma_declaration == NULL)
            throw;

        p_pragma_declaration->first_token = ctx->current;
        parser_match(ctx);

        while (ctx->current && ctx->current->type != TK_PRAGMA_END)
        {
            ctx->current = ctx->current->next;
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
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

struct static_assert_declaration* _Owner _Opt static_assert_declaration(struct parser_ctx* ctx)
{

    /*
     static_assert-declaration:
      "static_assert" ( constant-expression , string-literal ) ;
      "static_assert" ( constant-expression ) ;
    */

    struct static_assert_declaration* _Owner _Opt p_static_assert_declaration = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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

        if (ctx->options.flow_analysis)
            show_error_if_not_constant = false;

        struct expression* _Owner _Opt p_constant_expression = constant_expression(ctx, show_error_if_not_constant);
        if (p_constant_expression == NULL)
            throw;

        p_static_assert_declaration->constant_expression = p_constant_expression;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == ',')
        {
            parser_match(ctx);
            p_static_assert_declaration->string_literal_opt = ctx->current;
            if (parser_match_tk(ctx, TK_STRING_LITERAL) != 0)
                throw;
        }

        if (parser_match_tk(ctx, ')') != 0)
            throw;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_static_assert_declaration->last_token = ctx->current;
        if (parser_match_tk(ctx, ';') != 0)
            throw;

        if (position->type == TK_KEYWORD__STATIC_ASSERT)
        {
            if (object_has_constant_value(&p_static_assert_declaration->constant_expression->object) &&
                !object_to_bool(&p_static_assert_declaration->constant_expression->object))
            {
                if (p_static_assert_declaration->string_literal_opt)
                {
                    compiler_diagnostic(C_ERROR_STATIC_ASSERT_FAILED, ctx, position, NULL, "static_assert failed %s\n",
                        p_static_assert_declaration->string_literal_opt->lexeme);
                }
                else
                {
                    compiler_diagnostic(C_ERROR_STATIC_ASSERT_FAILED, ctx, position, NULL, "static_assert failed");
                }
            }
        }
    }
    catch
    {
    }

    return p_static_assert_declaration;
}

void attribute_specifier_sequence_add(struct attribute_specifier_sequence* list, struct attribute_specifier* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void attribute_specifier_sequence_delete(struct attribute_specifier_sequence* _Owner _Opt p)
{
    if (p)
    {
        struct attribute_specifier* _Owner _Opt item = p->head;
        while (item)
        {
            struct attribute_specifier* _Owner _Opt next = item->next;
            item->next = NULL;
            attribute_specifier_delete(item);
            item = next;
        }
        free(p);
    }
}

struct attribute_specifier_sequence* _Owner _Opt attribute_specifier_sequence_opt(struct parser_ctx* ctx)
{
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (first_of_attribute_specifier(ctx))
        {
            p_attribute_specifier_sequence = calloc(1, sizeof(struct attribute_specifier_sequence));
            if (p_attribute_specifier_sequence == NULL)
                throw;

            p_attribute_specifier_sequence->first_token = ctx->current;

            while (ctx->current != NULL &&
                first_of_attribute_specifier(ctx))
            {
                struct attribute_specifier* _Owner _Opt p_attribute_specifier = attribute_specifier(ctx);
                if (p_attribute_specifier == NULL)
                    throw;

                p_attribute_specifier_sequence->attributes_flags |=
                    p_attribute_specifier->attribute_list->attributes_flags;

                attribute_specifier_sequence_add(p_attribute_specifier_sequence, p_attribute_specifier);
            }

            if (ctx->previous == NULL)
                throw;

            p_attribute_specifier_sequence->last_token = ctx->previous;
        }
    }
    catch
    {
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = NULL;
    }

    return p_attribute_specifier_sequence;
}

static struct attribute_specifier_sequence* _Owner _Opt attribute_specifier_sequence(struct parser_ctx* ctx)
{
    // attribute_specifier_sequence_opt attribute_specifier
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence = NULL;
    try
    {
        p_attribute_specifier_sequence = calloc(1, sizeof(struct attribute_specifier_sequence));
        if (p_attribute_specifier_sequence == NULL)
            throw;

        while (first_of_attribute_specifier(ctx))
        {
            struct attribute_specifier* _Owner _Opt p_attribute_specifier = attribute_specifier(ctx);
            if (p_attribute_specifier == NULL) throw;

            attribute_specifier_sequence_add(p_attribute_specifier_sequence, p_attribute_specifier);
        }
    }
    catch
    {
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = NULL;
    }

    return p_attribute_specifier_sequence;
}

void attribute_specifier_delete(struct attribute_specifier* _Owner _Opt p)
{
    if (p)
    {
        attribute_list_delete(p->attribute_list);
        assert(p->next == NULL);
        free(p);
    }
}

struct attribute_specifier* _Owner _Opt attribute_specifier(struct parser_ctx* ctx)
{
    struct attribute_specifier* _Owner _Opt p_attribute_specifier = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_attribute_specifier = calloc(1, sizeof(struct attribute_specifier));
        if (p_attribute_specifier == NULL)
            throw;

        p_attribute_specifier->first_token = ctx->current;

        //'[' '[' attribute_list ']' ']'
        if (parser_match_tk(ctx, '[') != 0)
            throw;
        if (parser_match_tk(ctx, '[') != 0)
            throw;

        struct attribute_list* _Owner _Opt p_attribute_list = attribute_list(ctx);

        if (p_attribute_list == NULL)
            throw;

        p_attribute_specifier->attribute_list = p_attribute_list;
        if (parser_match_tk(ctx, ']') != 0)
            throw;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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

void attribute_delete(struct attribute* _Owner _Opt p)
{
    if (p)
    {
        attribute_token_delete(p->attribute_token);
        attribute_argument_clause_delete(p->attribute_argument_clause);
        assert(p->next == NULL);
        free(p);
    }
}

void attribute_list_add(struct attribute_list* list, struct attribute* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void attribute_list_destroy(_Dtor struct attribute_list* p)
{
    struct attribute* _Owner _Opt item = p->head;
    while (item)
    {
        struct attribute* _Owner _Opt next = item->next;
        item->next = NULL;
        attribute_delete(item);
        item = next;
    }
}

void attribute_list_delete(struct attribute_list* _Owner _Opt p)
{
    if (p)
    {
        attribute_list_destroy(p);
        free(p);
    }
}

struct attribute_list* _Owner _Opt attribute_list(struct parser_ctx* ctx)
{
    struct attribute_list* _Owner _Opt p_attribute_list = NULL;
    try
    {
        p_attribute_list = calloc(1, sizeof(struct attribute_list));
        if (p_attribute_list == NULL)
            throw;

        //
        // attribute_list ',' attribute_opt
        while (ctx->current != NULL && (first_of_attribute(ctx) ||
            ctx->current->type == ','))
        {
            if (first_of_attribute(ctx))
            {
                struct attribute* _Owner _Opt p_attribute = attribute(ctx);
                if (p_attribute == NULL) throw;

                p_attribute_list->attributes_flags |= p_attribute->attributes_flags;
                attribute_list_add(p_attribute_list, p_attribute);
            }

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            if (ctx->current->type == ',')
            {
                parser_match(ctx);
            }
        }
    }
    catch
    {
        attribute_list_delete(p_attribute_list);
        p_attribute_list = NULL;
    }

    return p_attribute_list;
}

bool first_of_attribute(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    if (ctx->current->type == TK_IDENTIFIER)
        return true;

    if (is_keyword(ctx->current->lexeme) != 0)
        return true;

    return false;
}

struct attribute* _Owner _Opt attribute(struct parser_ctx* ctx)
{
    struct attribute* _Owner _Opt p_attribute = NULL;
    try
    {
        p_attribute = calloc(1, sizeof(struct attribute));
        if (p_attribute == NULL)
            throw;

        struct attribute_token* _Owner _Opt p_attribute_token = attribute_token(ctx);

        if (p_attribute_token == NULL)
            throw;

        // attribute_token attribute_argument_clause_opt
        p_attribute->attribute_token = p_attribute_token;
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_attribute->attributes_flags = p_attribute->attribute_token->attributes_flags;
        if (ctx->current->type == '(') // first
        {
            struct attribute_argument_clause* _Owner _Opt  p_attribute_argument_clause =
                attribute_argument_clause(ctx);
            if (p_attribute_argument_clause == NULL)
                throw;
            p_attribute->attribute_argument_clause = p_attribute_argument_clause;
        }
    }
    catch
    {
        attribute_delete(p_attribute);
        p_attribute = NULL;
    }
    return p_attribute;
}

void attribute_token_delete(struct attribute_token* _Owner _Opt p)
{
    if (p)
    {
        free(p);
    }
}

struct attribute_token* _Owner _Opt attribute_token(struct parser_ctx* ctx)
{
    struct attribute_token* _Owner _Opt p_attribute_token = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_attribute_token = calloc(1, sizeof(struct attribute_token));
        if (p_attribute_token == NULL)
            throw;

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
        else if (strcmp(attr_token->lexeme, "dtor") == 0)
        {
            is_standard_attribute = true;
            p_attribute_token->attributes_flags = CAKE_ATTRIBUTE_DTOR;
        }
        else if (strcmp(attr_token->lexeme, "ctor") == 0)
        {
            is_standard_attribute = true;
            p_attribute_token->attributes_flags = CAKE_ATTRIBUTE_CTOR;
        }

        const bool is_cake_attr = strcmp(attr_token->lexeme, "cake") == 0;

        if (token_is_identifier_or_keyword(ctx->current->type))
        {
            parser_match(ctx);
        }
        else
        {
            compiler_diagnostic(C_ERROR_UNEXPECTED_TOKEN, ctx, attr_token, NULL, "expected identifier");
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == '::')
        {
            parser_match(ctx);
            if (is_cake_attr)
            {
                compiler_diagnostic(W_ATTRIBUTES, ctx, attr_token, NULL, "warning '%s' is not an cake attribute", ctx->current->lexeme);
            }

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            if (token_is_identifier_or_keyword(ctx->current->type))
            {
                parser_match(ctx);
            }
            else
            {
                compiler_diagnostic(C_ERROR_UNEXPECTED_TOKEN, ctx, attr_token, NULL, "expected identifier");
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
                compiler_diagnostic(W_ATTRIBUTES, ctx, attr_token, NULL, "warning '%s' is not an standard attribute", attr_token->lexeme);
            }
        }
    }
    catch
    {
        attribute_token_delete(p_attribute_token);
        p_attribute_token = NULL;
    }

    return p_attribute_token;
}

void attribute_argument_clause_delete(struct attribute_argument_clause* _Owner _Opt p)
{
    if (p)
    {
        balanced_token_sequence_delete(p->p_balanced_token_sequence);
        free(p);
    }
}

struct attribute_argument_clause* _Owner _Opt  attribute_argument_clause(struct parser_ctx* ctx)
{
    struct attribute_argument_clause* _Owner _Opt p_attribute_argument_clause = calloc(1, sizeof(struct attribute_argument_clause));
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

void balanced_token_sequence_delete(struct balanced_token_sequence* _Owner _Opt p)
{
    if (p)
    {
        struct balanced_token* _Owner _Opt item = p->head;
        while (item)
        {
            struct balanced_token* _Owner _Opt  next = item->next;
            free(item);
            item = next;
        }
        free(p);
    }
}

struct balanced_token_sequence* _Owner _Opt balanced_token_sequence_opt(struct parser_ctx* ctx)
{
    struct balanced_token_sequence* _Owner _Opt p_balanced_token_sequence = calloc(1, sizeof(struct balanced_token_sequence));
    try
    {
        if (p_balanced_token_sequence == NULL)
            throw;

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
            compiler_diagnostic(C_ERROR_ATTR_UNBALANCED, ctx, ctx->current, NULL, "expected ']' before ')'");
        }
        if (count3 != 0)
        {
            compiler_diagnostic(C_ERROR_ATTR_UNBALANCED, ctx, ctx->current, NULL, "expected '}' before ')'");
        }
    }
    catch
    {
        balanced_token_sequence_delete(p_balanced_token_sequence);
        p_balanced_token_sequence = NULL;
    }
    return p_balanced_token_sequence;
}

void statement_delete(struct statement* _Owner _Opt p)
{
    if (p)
    {
        labeled_statement_delete(p->labeled_statement);
        unlabeled_statement_delete(p->unlabeled_statement);
        free(p);
    }
}

struct statement* _Owner _Opt statement(struct parser_ctx* ctx)
{
    struct statement* _Owner _Opt p_statement = calloc(1, sizeof(struct statement));
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

struct primary_block* _Owner _Opt primary_block(struct parser_ctx* ctx)
{
    assert(ctx->current != NULL);
    struct primary_block* _Owner _Opt p_primary_block = calloc(1, sizeof(struct primary_block));
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
            compiler_diagnostic(C_ERROR_UNEXPECTED_TOKEN, ctx, ctx->current, NULL, "unexpected token");
        }
    }
    catch
    {
        primary_block_delete(p_primary_block);
        p_primary_block = NULL;
    }
    return p_primary_block;
}

struct secondary_block* _Owner _Opt secondary_block(struct parser_ctx* ctx)
{
    struct secondary_block* _Owner _Opt p_secondary_block = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        check_open_brace_style(ctx, ctx->current);

        p_secondary_block = calloc(1, sizeof(struct secondary_block));
        if (p_secondary_block == NULL)
            throw;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_secondary_block->first_token = ctx->current;

        struct statement* _Owner _Opt p_statement = statement(ctx);
        if (p_statement == NULL)
            throw;

        p_secondary_block->statement = p_statement;

        if (ctx->previous == NULL)
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
    struct expression* _Opt p_expression = NULL;

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

bool secondary_block_ends_with_jump(struct secondary_block* _Opt p_secondary_block)
{
    if (p_secondary_block &&
        p_secondary_block->statement->unlabeled_statement)
    {
        return unlabeled_statement_ends_with_jump(p_secondary_block->statement->unlabeled_statement);
    }
    return false;
}

void secondary_block_delete(struct secondary_block* _Owner _Opt p)
{
    if (p)
    {
        statement_delete(p->statement);
        free(p);
    }
}

void primary_block_delete(struct primary_block* _Owner _Opt p)
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

static bool first_of_primary_block(const struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

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

void unlabeled_statement_delete(struct unlabeled_statement* _Owner _Opt p)
{
    if (p)
    {
        expression_statement_delete(p->expression_statement);
        jump_statement_delete(p->jump_statement);
        primary_block_delete(p->primary_block);
        free(p);
    }
}

struct unlabeled_statement* _Owner _Opt unlabeled_statement(struct parser_ctx* ctx)
{
    /*
     unlabeled-statement:
       expression-statement
       attribute-specifier-sequence _Opt primary-block
       attribute-specifier-sequence _Opt jump-statement
    */
    struct unlabeled_statement* _Owner _Opt p_unlabeled_statement = calloc(1, sizeof(struct unlabeled_statement));
    try
    {
        if (p_unlabeled_statement == NULL)
            throw;

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
                        compiler_diagnostic(W_ATTRIBUTES, ctx,
                            p_unlabeled_statement->expression_statement->expression_opt->first_token, NULL,
                            "ignoring return value of function declared with 'nodiscard' attribute");
                    }
                }
                if (type_is_owner(&p_unlabeled_statement->expression_statement->expression_opt->type) &&
                    p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
                {

                    if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                    {
                        compiler_diagnostic(W_ATTRIBUTES, ctx,
                            p_unlabeled_statement->expression_statement->expression_opt->first_token, NULL,
                            "ignoring the result of _Owner type ");
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
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != ASSIGNMENT_EXPRESSION_ASSIGN &&
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
                        compiler_diagnostic(W_UNUSED_VALUE,
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

void label_delete(struct label* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        free(p);
    }
}

struct label* _Owner _Opt label(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return NULL;

    struct label* _Owner _Opt p_label = calloc(1, sizeof(struct label));
    try
    {
        if (p_label == NULL)
            throw;

        p_label->label_id = ctx->label_id++;

        p_label->p_first_token = ctx->current;

        if (ctx->current->type == TK_IDENTIFIER)
        {
            struct label_list_item* _Opt p_label_list_item =
                label_list_find(&ctx->label_list, ctx->current->lexeme);

            if (p_label_list_item == NULL)
            {
                struct label_list_item* _Owner _Opt p_label_list_item_new = calloc(1, sizeof * p_label_list_item_new);
                if (p_label_list_item_new)
                {
                    p_label_list_item_new->p_defined = ctx->current;
                    label_list_push(&ctx->label_list, p_label_list_item_new);
                }
            }
            else
            {
                if (p_label_list_item->p_defined)
                {
                    //already defined
                    compiler_diagnostic(C_ERROR_DUPLICATED_LABEL, ctx, ctx->current, NULL, "duplicated label '%s'", ctx->current->lexeme);
                    compiler_diagnostic(W_NOTE, ctx, p_label_list_item->p_defined, NULL, "previous definition of '%s'", ctx->current->lexeme);
                }
                else
                {
                    p_label_list_item->p_defined = ctx->current;
                }
            }

            p_label->p_identifier_opt = ctx->current;
            parser_match(ctx);
            if (parser_match_tk(ctx, ':') != 0)
                throw;
        }
        else if (ctx->current->type == TK_KEYWORD_CASE)
        {

            if (ctx->p_current_selection_statement == NULL ||
                ctx->p_current_selection_statement->condition == NULL)
            {
                //unexpected because we are in case
                throw;
            }


            parser_match(ctx);
            p_label->constant_expression = constant_expression(ctx, true);
            if (p_label->constant_expression == NULL)
                throw;

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            if (ctx->current->type == '...')
            {
                parser_match(ctx);
                p_label->constant_expression_end = constant_expression(ctx, true);
                if (p_label->constant_expression_end == NULL)
                    throw;
                /*
                n3550.pdf
                If the arithmetic value of the first constant expression is
                greater than the one of the second, the range
                described by the constant range expression is empty.
                */

                struct label* _Opt p_existing_label = case_label_list_find_range(&ctx->p_current_selection_statement->label_list,
                    &p_label->constant_expression->object,
                    &p_label->constant_expression_end->object);


                if (p_existing_label)
                {  //we have a case with a single value that is inside this begin...end
                    char str1[50];
                    object_to_str(&p_label->constant_expression->object, 50, str1);

                    char str2[50];
                    object_to_str(&p_label->constant_expression_end->object, 50, str2);


                    //current range x existing range
                    //current range x existing single value

                    compiler_diagnostic(C_ERROR_DUPLICATED_CASE,
                            ctx,
                            p_label->constant_expression->first_token, NULL,
                            "case '%s' ... '%s' is duplicating values", str1, str2);

                    assert(p_label->constant_expression != NULL); //because case have values
                    compiler_diagnostic(W_LOCATION,
                        ctx,
                        p_existing_label->constant_expression->first_token, NULL, "previous case");
                }
            }
            else
            {
                struct label* _Opt p_existing_label = case_label_list_find(&ctx->p_current_selection_statement->label_list, &p_label->constant_expression->object);
                if (p_existing_label)
                {

                    //current single value x existing single value
                    //current single value x existing range
                    char str[50];
                    object_to_str(&p_label->constant_expression->object, 50, str);

                    compiler_diagnostic(C_ERROR_DUPLICATED_CASE,
                            ctx,
                            p_label->constant_expression->first_token, NULL,
                            "duplicate case '%s'", str);

                    assert(p_label->constant_expression != NULL); //because case have values
                    compiler_diagnostic(W_LOCATION,
                        ctx,
                        p_existing_label->constant_expression->first_token, NULL, "previous declaration");
                }
            }

            if (parser_match_tk(ctx, ':') != 0)
                throw;

            case_label_list_push(&ctx->p_current_selection_statement->label_list, p_label);

            if (ctx->p_current_selection_statement &&
                ctx->p_current_selection_statement->condition &&
                ctx->p_current_selection_statement->condition->expression)
            {
                if (type_is_enum(&ctx->p_current_selection_statement->condition->expression->type))
                {
                    if (type_is_enum(&p_label->constant_expression->type))
                    {
                        check_diferent_enuns(ctx,
                                    p_label->constant_expression->first_token,
                                    p_label->constant_expression,
                                    ctx->p_current_selection_statement->condition->expression,
                                    "mismatch in enumeration types");
                    }
                    else
                    {
                        //enum and something else...
                    }
                }

                if (ctx->p_current_selection_statement == NULL)
                {
                    //unexpected because we have case inside switch
                    throw;
                }

                const struct enum_specifier* _Opt p_enum_specifier = NULL;

                if (ctx->p_current_selection_statement->condition &&
                    ctx->p_current_selection_statement->condition->expression &&
                    ctx->p_current_selection_statement->condition->expression->type.enum_specifier)
                {
                    p_enum_specifier = get_complete_enum_specifier(ctx->p_current_selection_statement->condition->expression->type.enum_specifier);
                }

                if (p_enum_specifier)
                {
                    const struct enumerator* _Opt p_enumerator = find_enumerator_by_value(p_enum_specifier, &p_label->constant_expression->object);
                    if (p_enumerator == NULL)
                    {
                        char str[50];
                        object_to_str(&p_label->constant_expression->object, 50, str);

                        compiler_diagnostic(W_ENUN_CONVERSION,
                                        ctx,
                                        p_label->constant_expression->first_token, NULL,
                                        "case value '%s' not in enumerated type 'enum %s'",
                                        str,
                                        p_enum_specifier->tag_name);
                    }
                    else
                    {

                    }
                }
            }

        }
        else if (ctx->current->type == TK_KEYWORD_DEFAULT)
        {
            struct label* _Opt p_existing_default_label = case_label_list_find_default(&ctx->p_current_selection_statement->label_list);

            if (p_existing_default_label)
            {
                compiler_diagnostic(C_ERROR_MULTIPLE_DEFAULT_LABELS_IN_ONE_SWITCH,
                    ctx,
                    p_label->p_first_token,
                    NULL,
                    "multiple default labels in one switch");

                compiler_diagnostic(W_NOTE,
                    ctx,
                    p_existing_default_label->p_first_token,
                    NULL,
                    "previous default");

                throw;
            }

            parser_match(ctx);
            if (parser_match_tk(ctx, ':') != 0)
                throw;

            case_label_list_push(&ctx->p_current_selection_statement->label_list, p_label);
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


struct label* _Opt case_label_list_find_default(const struct case_label_list* list)
{
    struct label* _Opt p = list->head;
    while (p)
    {
        if (p->p_first_token->type == TK_KEYWORD_DEFAULT)
            return p;
        p = p->next;
    }
    return NULL;
}


struct label* _Opt case_label_list_find_range(const struct case_label_list* list, const struct object* begin, const struct object* end)
{
    struct label* _Opt p = list->head;
    while (p)
    {
        if (p->constant_expression_end == NULL)
        {
            if (object_greater_than_or_equal(&p->constant_expression->object, begin) &&
                object_smaller_than_or_equal(&p->constant_expression_end->object, end))
            {
                return p;
            }
        }
        else
        {
            //range with range intersection
            if (object_smaller_than_or_equal(&p->constant_expression->object, end) &&
                object_smaller_than_or_equal(begin, &p->constant_expression_end->object))
                return p;
        }
        p = p->next;
    }
    return NULL;
}

struct label* _Opt case_label_list_find(const struct case_label_list* list, const struct object* object)
{
    struct label* _Opt p = list->head;
    while (p)
    {
        if (p->constant_expression_end == NULL)
        {
            if (object_equal(&p->constant_expression->object, object))
                return p;
        }
        else
        {
            if (object_greater_than_or_equal(object, &p->constant_expression->object) &&
                object_smaller_than_or_equal(object, &p->constant_expression_end->object))
                return p;
        }
        p = p->next;
    }
    return NULL;
}

void case_label_list_push(struct case_label_list* list, struct label* pnew)
{
    if (list->head == NULL)
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        assert(list->tail != NULL);
        list->tail->next = pnew;
        list->tail = pnew;
    }
}

void labeled_statement_delete(struct labeled_statement* _Owner _Opt p)
{
    if (p)
    {
        label_delete(p->label);
        statement_delete(p->statement);
        free(p);
    }
}

struct labeled_statement* _Owner _Opt labeled_statement(struct parser_ctx* ctx)
{
    struct labeled_statement* _Owner _Opt p_labeled_statement = calloc(1, sizeof(struct labeled_statement));
    try
    {
        if (p_labeled_statement == NULL)
            throw;

        // label statement
        struct label* _Owner _Opt p_label = label(ctx);

        if (p_label == NULL)
            throw;

        p_labeled_statement->label = p_label;

        struct statement* _Owner _Opt p_statement = statement(ctx);
        if (p_statement == NULL)
            throw;

        p_labeled_statement->statement = p_statement;
    }
    catch
    {
        labeled_statement_delete(p_labeled_statement);
        p_labeled_statement = NULL;
    }
    return p_labeled_statement;
}

void compound_statement_delete(struct compound_statement* _Owner _Opt p)
{
    if (p)
    {
        block_item_list_destroy(&p->block_item_list);
        free(p);
    }
}

struct compound_statement* _Owner _Opt compound_statement(struct parser_ctx* ctx)
{

    struct scope block_scope = { .variables.capacity = 10 };

    //'{' block_item_list_opt '}'
    struct compound_statement* _Owner _Opt p_compound_statement = NULL;

    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_compound_statement = calloc(1, sizeof(struct compound_statement));

        if (p_compound_statement == NULL)
            throw;

        p_compound_statement->diagnostic_flags = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];

        scope_list_push(&ctx->scopes, &block_scope);

        p_compound_statement->first_token = ctx->current;
        if (parser_match_tk(ctx, '{') != 0)
            throw;

        if (ctx->current == NULL)
        {
            compiler_diagnostic(C_ERROR_UNEXPECTED_TOKEN, ctx, ctx->input_list.tail, NULL, "unexpected end of file");
            throw;
        }

        if (ctx->current->type != '}')
        {
            bool berror = false;
            p_compound_statement->block_item_list = block_item_list(ctx, &berror);
            if (berror)
                throw;
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
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
            struct map_entry* _Opt entry = block_scope.variables.table[i];
            while (entry)
            {

                if (entry->type != TAG_TYPE_DECLARATOR &&
                    entry->type != TAG_TYPE_INIT_DECLARATOR)
                {
                    entry = entry->next;
                    continue;
                }

                struct declarator* _Opt p_declarator = NULL;
                struct init_declarator* _Opt p_init_declarator = NULL;
                if (entry->type == TAG_TYPE_INIT_DECLARATOR)
                {
                    assert(entry->data.p_init_declarator != NULL);
                    p_init_declarator = entry->data.p_init_declarator;
                    p_declarator = p_init_declarator->p_declarator;
                }
                else
                {
                    p_declarator = entry->data.p_declarator;
                }

                if (p_declarator)
                {

                    if (!type_is_maybe_unused(&p_declarator->type) &&
                        p_declarator->num_uses == 0)
                    {
                        if (p_declarator->name_opt && p_declarator->name_opt->token_origin->level == 0)
                        {
                            compiler_diagnostic(W_UNUSED_VARIABLE,
                                ctx,
                                p_declarator->name_opt, NULL,
                                "'%s': unreferenced declarator",
                                p_declarator->name_opt->lexeme);
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

void block_item_list_add(struct block_item_list* list, struct block_item* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;

}

void block_item_list_destroy(_Dtor struct block_item_list* list)
{
    struct block_item* _Owner _Opt item = list->head;
    while (item)
    {
        struct block_item* _Owner _Opt next = item->next;
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
    struct block_item* _Owner _Opt p_block_item = NULL;
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

void block_item_delete(struct block_item* _Owner _Opt p)
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

struct block_item* _Owner _Opt block_item(struct parser_ctx* ctx)
{
    //   declaration
    //     unlabeled_statement
    //   label


    struct block_item* _Owner _Opt p_block_item = NULL;

    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_block_item = calloc(1, sizeof(struct block_item));
        if (p_block_item == NULL)
            throw;

        /*
         * Attributes can be first of declaration, labels etc..
         * so it is better to parse it in advance.
         */
        struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
            throw;
        }

        p_block_item->first_token = ctx->current;

        if (ctx->current->type == TK_KEYWORD__ASM)
        { /*
       asm-block:
       __asm assembly-instruction ;_Opt
       __asm { assembly-instruction-list } ;_Opt

   assembly-instruction-list:
       assembly-instruction ;_Opt
       assembly-instruction ; assembly-instruction-list ;_Opt
       */

            parser_match(ctx);

            if (ctx->current == NULL)
            {
                attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                unexpected_end_of_file(ctx);
                throw;
            }

            if (ctx->current->type == '{')
            {
                parser_match(ctx);

                while (ctx->current && ctx->current->type != '}')
                {
                    parser_match(ctx);
                }

                parser_match(ctx);
            }
            else
            {

                while (ctx->current && ctx->current->type != TK_NEWLINE)
                {
                    ctx->current = ctx->current->next;
                }

                parser_match(ctx);
            }

            if (ctx->current == NULL)
            {
                attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                unexpected_end_of_file(ctx);
                throw;
            }

            if (ctx->current->type == ';')
                parser_match(ctx);
        }
        else if (first_of_declaration_specifier(ctx) ||
            first_of_static_assert_declaration(ctx) ||
            first_of_pragma_declaration(ctx))
        {
            p_block_item->declaration = declaration(ctx, p_attribute_specifier_sequence_opt, STORAGE_SPECIFIER_AUTOMATIC_STORAGE, false);
            if (p_block_item->declaration == NULL)
                throw;
            p_attribute_specifier_sequence_opt = NULL; /*MOVED*/

            struct init_declarator* _Opt p = p_block_item->declaration->init_declarator_list.head;
            while (p)
            {
                if (p->p_declarator->name_opt)
                {
                    naming_convention_local_var(ctx, p->p_declarator->name_opt, &p->p_declarator->type);
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

void try_statement_delete(struct try_statement* _Owner _Opt p)
{
    if (p)
    {
        secondary_block_delete(p->catch_secondary_block_opt);
        secondary_block_delete(p->secondary_block);
        free(p);
    }
}

struct try_statement* _Owner _Opt try_statement(struct parser_ctx* ctx)
{
    struct try_statement* _Owner _Opt p_try_statement = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_try_statement = calloc(1, sizeof(struct try_statement));

        if (p_try_statement == NULL)
            throw;

        p_try_statement->first_token = ctx->current;

        assert(ctx->current->type == TK_KEYWORD_TRY);
        const struct try_statement* _Opt try_statement_copy_opt = ctx->p_current_try_statement_opt;
        ctx->p_current_try_statement_opt = p_try_statement;

        p_try_statement->catch_label_id = ctx->label_id++;


        if (parser_match_tk(ctx, TK_KEYWORD_TRY) != 0)
            throw;

#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wmissing-destructor"    

        struct secondary_block* _Owner _Opt p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == NULL) throw;

        p_try_statement->secondary_block = p_secondary_block;
#pragma cake diagnostic pop

        /*restores the previous one*/
        ctx->p_current_try_statement_opt = try_statement_copy_opt;

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type == TK_KEYWORD_CATCH)
        {
            p_try_statement->catch_token_opt = ctx->current;
            parser_match(ctx);

            assert(p_try_statement->catch_secondary_block_opt == NULL);


            p_try_statement->catch_secondary_block_opt = secondary_block(ctx);
            if (p_try_statement->catch_secondary_block_opt == NULL) throw;

        }
        if (ctx->previous == NULL)
            throw;

        p_try_statement->last_token = ctx->previous;
    }
    catch
    {
        try_statement_delete(p_try_statement);
        p_try_statement = NULL;
    }
    return p_try_statement;
}

void selection_statement_delete(struct selection_statement* _Owner _Opt p)
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

struct selection_statement* _Owner _Opt selection_statement(struct parser_ctx* ctx)
{
    /*
    selection-statement: //C23
        "if" ( expression ) secondary-block
        "if" ( expression ) secondary-block else secondary-block
        "switch" ( expression ) secondary-block
    */

    /* Extension

    selection-statement: C2Y?
       "if" ( init-statement _Opt condition ) secondary-block
       "if" ( init-statement _Opt condition ) secondary-block "else" secondary-block
       switch ( init-statement _Opt condition ) secondary-block
    */

    struct scope if_scope = { 0 };
    scope_list_push(&ctx->scopes, &if_scope);

    struct selection_statement* _Owner _Opt p_selection_statement = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_selection_statement = calloc(1, sizeof(struct selection_statement));

        if (p_selection_statement == NULL)
        {
            throw;
        }

        p_selection_statement->label_id = ctx->label_id++;
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

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (!(ctx->current->flags & TK_FLAG_MACRO_EXPANDED) && !style_has_one_space(ctx->current))
        {
            compiler_diagnostic(W_STYLE, ctx, ctx->current, NULL, "one space");
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

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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
            if (p_selection_statement->condition == NULL)
                throw;

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

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_selection_statement->close_parentesis_token = ctx->current;
        if (parser_match_tk(ctx, ')') != 0)
            throw;

        //if (object_has_constant_value(&p_selection_statement->init_statement_expression->object))
        //{
            //compiler_diagnostic(W_CONDITIONAL_IS_CONSTANT, ctx, p_selection_statement->init_statement_expression->first_token, "conditional expression is constant");
        //}

        if (!is_if && p_selection_statement && p_selection_statement->condition)
        {
            if (type_is_bool(&p_selection_statement->condition->expression->type) ||
                type_is_essential_bool(&p_selection_statement->condition->expression->type))
            {
                compiler_diagnostic(W_SWITCH,
                            ctx,
                            p_selection_statement->condition->first_token,
                            NULL,
                            "switch condition has boolean value"); //[-Wswitch-bool]
            }
        }

        const struct selection_statement* _Opt previous = ctx->p_current_selection_statement;
        ctx->p_current_selection_statement = p_selection_statement;

        struct secondary_block* _Owner _Opt p_secondary_block = secondary_block(ctx);

        if (p_secondary_block == NULL)
        {
            throw;
        }

        if (p_secondary_block->statement->unlabeled_statement &&
            p_secondary_block->statement->unlabeled_statement->expression_statement &&
            p_secondary_block->statement->unlabeled_statement->expression_statement->expression_opt == NULL)
        {
            compiler_diagnostic(W_SWITCH,
                            ctx,
                            p_secondary_block->first_token,
                            NULL,
                            "empty controlled statement found; is this the intent?");

        }


        assert(p_selection_statement->secondary_block == NULL);
        p_selection_statement->secondary_block = p_secondary_block;

        if (p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
        {
            //switch of enum without default, then we check if all items were used
            if (case_label_list_find_default(&p_selection_statement->label_list) == NULL)
            {
                const struct enum_specifier* _Opt p_enum_specifier = NULL;

                if (ctx->p_current_selection_statement &&
                    ctx->p_current_selection_statement->condition &&
                    ctx->p_current_selection_statement->condition->expression &&
                    ctx->p_current_selection_statement->condition->expression->type.enum_specifier)
                {
                    p_enum_specifier = get_complete_enum_specifier(ctx->p_current_selection_statement->condition->expression->type.enum_specifier);
                }

                if (p_enum_specifier)
                {
                    struct enumerator* _Opt p = p_enum_specifier->enumerator_list.head;
                    while (p)
                    {
                        struct label* _Opt p_used = case_label_list_find(&p_selection_statement->label_list, &p->value);

                        if (p_used == NULL)
                        {
                            compiler_diagnostic(W_SWITCH,
                                ctx,
                                ctx->current, NULL,
                                "enumeration '%s' not handled in switch", p->token->lexeme);
                        }
                        p = p->next;
                    }
                }
            }
        }

        ctx->p_current_selection_statement = previous;




        if (is_if && ctx->current && ctx->current->type == TK_KEYWORD_ELSE)
        {
            p_selection_statement->else_token_opt = ctx->current;
            parser_match(ctx);
            assert(p_selection_statement->else_secondary_block_opt == NULL);

            struct secondary_block* _Owner _Opt p_secondary_block2 = secondary_block(ctx);
            if (p_secondary_block2 == NULL) throw;

            p_selection_statement->else_secondary_block_opt = p_secondary_block2;
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        struct token* _Opt p_tk = previous_parser_token(ctx->current);
        if (p_tk == NULL)
        {
            throw;
        }

        p_selection_statement->last_token = p_tk;
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

struct defer_statement* _Owner _Opt defer_statement(struct parser_ctx* ctx)
{
    struct defer_statement* _Owner _Opt p_defer_statement = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        if (ctx->current->type != TK_KEYWORD_DEFER)
            throw;

        p_defer_statement = calloc(1, sizeof(struct defer_statement));

        if (p_defer_statement == NULL)
            throw;

        p_defer_statement->first_token = ctx->current;
        parser_match(ctx);

        const struct defer_statement* _Opt p_previous_defer_statement_opt =
            ctx->p_current_defer_statement_opt;

        ctx->p_current_defer_statement_opt = p_defer_statement;

        struct secondary_block* _Owner _Opt p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == NULL) throw;

        p_defer_statement->secondary_block = p_secondary_block;
        if (ctx->previous == NULL) throw;

        p_defer_statement->last_token = ctx->previous;

        ctx->p_current_defer_statement_opt = p_previous_defer_statement_opt;
    }
    catch
    {
        defer_statement_delete(p_defer_statement);
        p_defer_statement = NULL;
    }
    return p_defer_statement;
}

void iteration_statement_delete(struct iteration_statement* _Owner _Opt p)
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

struct iteration_statement* _Owner _Opt iteration_statement(struct parser_ctx* ctx)
{
    /*
    iteration-statement:
      while ( expression ) statement
      do statement while ( expression ) ;
      for ( expressionopt ; expressionopt ; expressionopt ) statement
      for ( declaration expressionopt ; expressionopt ) statement
    */
    struct iteration_statement* _Owner _Opt p_iteration_statement = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_iteration_statement = calloc(1, sizeof(struct iteration_statement));

        if (p_iteration_statement == NULL)
            throw;

        p_iteration_statement->first_token = ctx->current;
        if (ctx->current->type == TK_KEYWORD_DO)
        {
            parser_match(ctx);

            struct secondary_block* _Owner _Opt p_secondary_block = secondary_block(ctx);
            if (p_secondary_block == NULL) throw;

            p_iteration_statement->secondary_block = p_secondary_block;

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

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
            struct secondary_block* _Owner _Opt p_secondary_block = secondary_block(ctx);
            if (p_secondary_block == NULL) throw;
            p_iteration_statement->secondary_block = p_secondary_block;
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

                p_iteration_statement->declaration = declaration(ctx, NULL, STORAGE_SPECIFIER_AUTOMATIC_STORAGE, false);

                if (ctx->current == NULL)
                {

                    unexpected_end_of_file(ctx);

                    scope_list_pop(&ctx->scopes);
                    scope_destroy(&for_scope);
                    throw;
                }

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
                    scope_list_pop(&ctx->scopes);
                    scope_destroy(&for_scope);
                    throw;
                }

                if (ctx->current == NULL)
                {

                    unexpected_end_of_file(ctx);

                    scope_list_pop(&ctx->scopes);
                    scope_destroy(&for_scope);
                    throw;
                }

                if (ctx->current->type != ')')
                    p_iteration_statement->expression2 = expression(ctx);

                if (parser_match_tk(ctx, ')') != 0)
                {
                    scope_list_pop(&ctx->scopes);
                    scope_destroy(&for_scope);
                    throw;
                }

                struct secondary_block* _Owner _Opt p_secondary_block = secondary_block(ctx);
                if (p_secondary_block == NULL)
                {
                    scope_list_pop(&ctx->scopes);
                    scope_destroy(&for_scope);
                    throw;
                }

                p_iteration_statement->secondary_block = p_secondary_block;

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

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }

                if (ctx->current->type != ';')
                    p_iteration_statement->expression0 = expression(ctx);
                if (parser_match_tk(ctx, ';') != 0)
                    throw;

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }

                if (ctx->current->type != ';')
                    p_iteration_statement->expression1 = expression(ctx);

                if (parser_match_tk(ctx, ';') != 0)
                    throw;

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }

                if (ctx->current->type != ')')
                    p_iteration_statement->expression2 = expression(ctx);

                if (parser_match_tk(ctx, ')') != 0)
                    throw;

                if (ctx->current == NULL)
                {
                    unexpected_end_of_file(ctx);
                    throw;
                }

                struct secondary_block* _Owner _Opt p_secondary_block = secondary_block(ctx);
                if (p_secondary_block == NULL) throw;

                p_iteration_statement->secondary_block = p_secondary_block;
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

void jump_statement_delete(struct jump_statement* _Owner _Opt p)
{
    if (p)
    {
        expression_delete(p->expression_opt);
        free(p);
    }
}

struct jump_statement* _Owner _Opt jump_statement(struct parser_ctx* ctx)
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

    struct jump_statement* _Owner _Opt p_jump_statement = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_jump_statement = calloc(1, sizeof(struct jump_statement));

        if (p_jump_statement == NULL)
            throw;

        p_jump_statement->first_token = ctx->current;

        if (ctx->current->type == TK_KEYWORD_GOTO)
        {
            parser_match(ctx);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            struct label_list_item* _Opt p_label_list_item =
                label_list_find(&ctx->label_list, ctx->current->lexeme);

            if (p_label_list_item == NULL)
            {
                struct label_list_item* _Owner _Opt p_label_list_item_new = calloc(1, sizeof * p_label_list_item_new);
                if (p_label_list_item_new)
                {
                    p_label_list_item_new->p_last_usage = ctx->current;
                    label_list_push(&ctx->label_list, p_label_list_item_new);
                }
            }
            else
            {
                p_label_list_item->p_last_usage = ctx->current;
            }

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
                compiler_diagnostic(C_ERROR_THROW_STATEMENT_NOT_WITHIN_TRY_BLOCK, ctx, ctx->current, NULL, "throw statement not within try block");
            }
            else
            {
                p_jump_statement->label_id = ctx->p_current_try_statement_opt->catch_label_id;
            }

            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD_RETURN)
        {
            if (ctx->p_current_defer_statement_opt != NULL)
            {
                compiler_diagnostic(C_ERROR_RETURN_CANNOT_BE_USED_INSIDE_DEFER,
                             ctx,
                             ctx->current,
                             NULL,
                             "%s",
                             "return cannot be used inside defer statement");
                throw;
            }

            const struct token* const p_return_token = ctx->current;
            parser_match(ctx);

            if (ctx->current == NULL)
            {
                unexpected_end_of_file(ctx);
                throw;
            }

            /*
                     * Check is return type is compatible with function return
                     */
            struct type return_type =
                get_function_return_type(&ctx->p_current_function_opt->type);

            if (ctx->current->type != ';')
            {
                p_jump_statement->expression_opt = expression(ctx);

                if (p_jump_statement->expression_opt)
                {
                    if (type_is_void(&return_type))
                    {
                        const char* func_name =
                            ctx->p_current_function_opt->name_opt ?
                            ctx->p_current_function_opt->name_opt->lexeme :
                            "unnamed";

                        compiler_diagnostic(C_ERROR_VOID_FUNCTION_SHOULD_NOT_RETURN_VALUE,
                            ctx,
                            p_return_token, NULL,
                            "void function '%s' should not return a value",
                            func_name);
                    }
                    else
                    {
                        check_assigment(ctx,
                            &return_type,
                            p_jump_statement->expression_opt,
                            ASSIGMENT_TYPE_RETURN);
                    }
                }
            }
            else
            {
                if (!type_is_void(&return_type))
                {
                    const char* func_name =
                        ctx->p_current_function_opt->name_opt ?
                        ctx->p_current_function_opt->name_opt->lexeme :
                        "unnamed";

                    compiler_diagnostic(C_ERROR_NON_VOID_FUNCTION_SHOULD_RETURN_VALUE,
                        ctx,
                        p_return_token, NULL,
                        "non void function '%s' should return a value",
                        func_name);
                }
            }
            type_destroy(&return_type);
        }
        else
        {
            assert(false);
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
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

void expression_statement_delete(struct expression_statement* _Owner _Opt p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        expression_delete(p->expression_opt);
        free(p);
    }
}

struct expression_statement* _Owner _Opt  expression_statement(struct parser_ctx* ctx, bool ignore_semicolon)
{
    struct expression_statement* _Owner _Opt p_expression_statement = calloc(1, sizeof(struct expression_statement));
    try
    {
        /*
         expression-statement:
           expression _Opt ;
           attribute-specifier-sequence expression ;
        */
        if (p_expression_statement == NULL)
            throw;

        p_expression_statement->p_attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

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

void declaration_list_add(struct declaration_list* list, struct declaration* _Owner p_declaration)
{
    if (list->head == NULL)
    {
        list->head = p_declaration;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_declaration;
    }
    list->tail = p_declaration;
}

void simple_declaration_delete(struct simple_declaration* _Owner _Opt p_simple_declaration)
{
    if (p_simple_declaration)
    {
        declaration_specifiers_delete(p_simple_declaration->p_declaration_specifiers);
        init_declarator_list_destroy(&p_simple_declaration->init_declarator_list);
        attribute_specifier_sequence_delete(p_simple_declaration->p_attribute_specifier_sequence_opt);

        free(p_simple_declaration);
    }
}

void condition_delete(struct condition* _Owner _Opt p_condition)
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

struct condition* _Owner _Opt condition(struct parser_ctx* ctx)
{
    /*
    condition :
       expression
       attribute-specifier-seq opt decl-specifier-seq declarator initializer
    */
    struct condition* _Owner _Opt p_condition = NULL;
    try
    {
        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        p_condition = calloc(1, sizeof * p_condition);
        if (p_condition == NULL)
            throw;

        p_condition->first_token = ctx->current;
        if (first_of_declaration_specifier(ctx))
        {
            p_condition->p_attribute_specifier_sequence_opt = attribute_specifier_sequence(ctx);

            p_condition->p_declaration_specifiers = declaration_specifiers(ctx, STORAGE_SPECIFIER_AUTOMATIC_STORAGE);
            if (p_condition->p_declaration_specifiers == NULL)
                throw;

            struct init_declarator* _Owner _Opt p_init_declarator =
                init_declarator(ctx, p_condition->p_declaration_specifiers);

            if (p_init_declarator == NULL)
                throw;

            p_condition->p_init_declarator = p_init_declarator;
        }
        else
        {
            p_condition->expression = expression(ctx);
            if (p_condition->expression == NULL)
                throw;
        }

        if (ctx->current == NULL)
        {
            unexpected_end_of_file(ctx);
            throw;
        }

        struct token* _Opt previous = previous_parser_token(ctx->current);

        if (previous)
        {
            //shoult never be null
            p_condition->last_token = previous;
        }
    }
    catch
    {
        condition_delete(p_condition);
        p_condition = NULL;
    }
    return p_condition;
}

void init_statement_delete(struct init_statement* _Owner _Opt p_init_statement)
{
    if (p_init_statement)
    {
        expression_statement_delete(p_init_statement->p_expression_statement);
        simple_declaration_delete(p_init_statement->p_simple_declaration);
        free(p_init_statement);
    }
}

struct init_statement* _Owner _Opt init_statement(struct parser_ctx* ctx, bool ignore_semicolon)
{
    /*
       expression-statement
       simple-declaration
    */
    struct init_statement* _Owner _Opt p_init_statement = NULL;
    try
    {
        p_init_statement = calloc(1, sizeof * p_init_statement);

        if (p_init_statement == NULL)
            throw;

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

void declaration_delete(struct declaration* _Owner _Opt p)
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

void declaration_list_destroy(_Dtor struct declaration_list* list)
{
    struct declaration* _Owner _Opt p = list->head;
    while (p)
    {
        struct declaration* _Owner _Opt next = p->next;
        p->next = NULL;
        declaration_delete(p);
        p = next;
    }
}

static void check_unused_static_declarators(struct parser_ctx* ctx, struct declaration_list* declaration_list)
{
    struct declaration* _Opt p = declaration_list->head;
    while (p)
    {
        if (p->declaration_specifiers &&
            p->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
        {
            if (p->init_declarator_list.head &&
                p->init_declarator_list.head->p_declarator)
            {
                struct map_entry* _Opt p_entry = find_variables(ctx, p->init_declarator_list.head->p_declarator->name_opt->lexeme, NULL);
                if (p_entry && (p_entry->type == TAG_TYPE_DECLARATOR || p_entry->type == TAG_TYPE_INIT_DECLARATOR))
                {
                    /*
                       Consider
                       static void f();
                       static void f(){}
                       int main(){
                         f();
                       }
                       num_uses if incremented only at the pointer returned by find_variables
                    */
                    struct declarator* p_declarator_local = NULL;
                    if (p_entry->type == TAG_TYPE_INIT_DECLARATOR)
                    {
                        p_declarator_local = p_entry->data.p_init_declarator->p_declarator;
                    }
                    else
                    {
                        p_declarator_local = p_entry->data.p_declarator;

                    }
                    int num_uses = p_declarator_local->num_uses;
                    if (num_uses == 0)
                    {
                        if (type_is_function(&p->init_declarator_list.head->p_declarator->type))
                        {
                            compiler_diagnostic(W_UNUSED_FUNCTION,
                            ctx,
                            p->init_declarator_list.head->p_declarator->name_opt,
                            NULL,
                            "warning: static function '%s' not used.",
                            p->init_declarator_list.head->p_declarator->name_opt->lexeme);
                        }
                        else
                        {
                            compiler_diagnostic(W_UNUSED_VARIABLE,
                            ctx,
                            p->init_declarator_list.head->p_declarator->name_opt,
                            NULL,
                            "warning: '%s' not used.",
                            p->init_declarator_list.head->p_declarator->name_opt->lexeme);
                        }
                    }
                }
            }
        }
        p = p->next;
    }
}

struct declaration* _Owner _Opt external_declaration(struct parser_ctx* ctx)
{
    /*
     function_definition
     declaration
     */
    return function_definition_or_declaration(ctx);
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
            struct declaration* _Owner _Opt p = external_declaration(ctx);
            if (p == NULL)
                throw;
            declaration_list_add(&declaration_list, p);
        }

        check_unused_static_declarators(ctx, &declaration_list);
    }
    catch
    {
        *berror = true;
    }
    return declaration_list;
}



struct label_list_item* _Opt label_list_find(struct label_list* list, const char* label_name)
{
    struct label_list_item* _Opt item = list->head;
    while (item)
    {
        if (item->p_defined && strcmp(item->p_defined->lexeme, label_name) == 0)
        {
            return item;
        }
        else if (item->p_last_usage && strcmp(item->p_last_usage->lexeme, label_name) == 0)
        {
            return item;
        }

        item = item->next;
    }
    return NULL;
}

void label_list_swap(struct label_list* a, struct label_list* b)
{
    struct label_list temp = *a;
    *a = *b;
    *b = temp;
}
void label_list_clear(struct label_list* list)
{
    struct label_list_item* _Owner _Opt item = list->head;
    while (item)
    {
        struct label_list_item* _Owner _Opt next = item->next;
        item->next = NULL;
        free(item);
        item = next;
    }
    list->head = NULL;
    list->tail = NULL;
}

void label_list_push(struct label_list* list, struct label_list_item* _Owner pitem)
{
    if (list->head == NULL)
    {
        list->head = pitem;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = pitem;
    }
    list->tail = pitem;
}


void check_labels(struct parser_ctx* ctx)
{
    struct label_list_item* _Opt item = ctx->label_list.head;
    while (item)
    {
        if (item->p_defined == NULL && item->p_last_usage != NULL)
        {
            compiler_diagnostic(C_ERROR_LABEL_NOT_DEFINED, ctx, item->p_last_usage, NULL, "label '%s' used but not defined", item->p_last_usage->lexeme);
        }
        else if (item->p_defined != NULL && item->p_last_usage == NULL)
        {
            compiler_diagnostic(W_UNUSED_LABEL, ctx, item->p_defined, NULL, "label '%s' defined but not used", item->p_defined->lexeme);
        }
        item = item->next;
    }
}

struct compound_statement* _Owner _Opt function_body(struct parser_ctx* ctx)
{

    /*
     * Used to give an unique index (inside the function)
     * for try-catch blocks
     */

    const struct try_statement* _Opt p_current_try_statement_opt = ctx->p_current_try_statement_opt;
    ctx->p_current_try_statement_opt = NULL;

    int label_id = ctx->label_id;
    ctx->label_id = 0; /*reset*/

    struct defer_statement* _Opt p_current_defer_statement_opt = ctx->p_current_defer_statement_opt;
    ctx->p_current_defer_statement_opt = NULL;

    struct selection_statement* _Opt p_current_selection_statement = ctx->p_current_selection_statement;
    ctx->p_current_selection_statement = NULL;

    struct label_list label_list = { 0 };
    label_list_swap(&label_list, &ctx->label_list);

    struct compound_statement* _Owner _Opt p_compound_statement = compound_statement(ctx);
    if (p_compound_statement)
    {
        check_labels(ctx);
    }

    label_list_swap(&label_list, &ctx->label_list);

    ctx->label_id = label_id;
    ctx->p_current_try_statement_opt = p_current_try_statement_opt;
    ctx->p_current_defer_statement_opt = p_current_defer_statement_opt;
    ctx->p_current_selection_statement = p_current_selection_statement;

    label_list_clear(&label_list);

    return p_compound_statement;
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

WINBASEAPI unsigned long WINAPI GetEnvironmentVariableA(const char* name,
char* buffer,
unsigned long size);

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


int generate_config_file(const char* configpath)
{
    FILE* _Owner _Opt outfile = NULL;
    int error = 0;
    try
    {
        outfile = fopen(configpath, "w");
        if (outfile == NULL)
        {
            printf("Cannot open the file '%s' for writing.\n", configpath);
            error = errno;
            throw;
        }

        fprintf(outfile, "//This was generated by running cake -autoconfig \n");


#ifdef __linux__

        fprintf(outfile, "This file was generated reading the output of\n");
        fprintf(outfile, "//echo | gcc -v -E - 2>&1\n");
        fprintf(outfile, "\n");

        char path[400] = { 0 };
        char* command = "echo | gcc -v -E - 2>&1";
        int in_include_section = 0;

        // Open the command for reading
        FILE* fp = popen(command, "r");
        if (fp == NULL)
        {
            fprintf(stderr, "Failed to run command\n");
            error = errno;
            throw;
        }

        // Read the output a line at a time
        while (fgets(path, sizeof(path), fp) != NULL)
        {
            // Check if we are in the "#include <...> search starts here:" section
            if (strstr(path, "#include <...> search starts here:") != NULL)
            {
                in_include_section = 1;
                continue;
            }
            // Check if we have reached the end of the include section
            if (in_include_section && strstr(path, "End of search list.") != NULL)
            {
                break;
            }
            // Print the include directories
            if (in_include_section)
            {
                const char* p = path;
                while (*p == ' ') p++;

                int len = strlen(path);
                if (path[len - 1] == '\n')
                    path[len - 1] = '\0';

                fprintf(outfile, "#pragma dir \"%s\"\n", p);
            }
        }

        fprintf(outfile, "\n");

        // Close the command stream
        pclose(fp);

#endif

#ifdef _WIN32
        char env[2000] = { 0 };
        int n = GetEnvironmentVariableA("INCLUDE", env, sizeof(env));

        if (n <= 0)
        {
            printf("INCLUDE not found.\nPlease, run cake -autoconfig inside visual studio command prompty.\n");
            error = 1;
            throw;
        }

        fprintf(outfile, "//This file was generated reading the variable INCLUDE inside Visual Studio Command Prompt.\n");
        fprintf(outfile, "//echo %%INCLUDE%% \n");

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
                char* pch = filename_local;
                while (*pch)
                {
                    if (*pch == '\\')
                        *pch = '/';
                    pch++;
                }

                fprintf(outfile, "#pragma dir \"%s\"\n", filename_local);
            }
            if (*p == '\0')
            {
                break;
            }
            p++;
        }
#endif
    }
    catch
    {
    }
    if (outfile)
        fclose(outfile);

    if (error == 0)
    {
        printf("file '%s'\n", configpath);
        printf("successfully generated\n");
    }
    return error;
}

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

    if (include_config_header(&prectx, file_name) != 0)
    {
        //cakeconfig.h is optional               
    }
    // print_all_macros(&prectx);

    struct ast ast = { 0 };

    const char* _Owner _Opt s = NULL;

    _Opt struct parser_ctx ctx = { 0 };

    struct tokenizer_ctx tctx = { 0 };
    struct token_list tokens = { 0 };

    ctx.options = *options;
    ctx.p_report = report;
    char* _Owner _Opt content = NULL;

    try
    {
        //-D , -I etc..
        if (fill_preprocessor_options(argc, argv, &prectx) != 0)
        {
            throw;
        }

        prectx.options = *options;
        append_msvc_include_dir(&prectx);

        content = read_file(file_name, true /*append new line*/);
        if (content == NULL)
        {
            report->error_count++;
            printf("file not found '%s'\n", file_name);
            throw;
        }

        if (options->sarif_output)
        {
            char sarif_file_name[260] = { 0 };
            if (options->sarifpath[0] != '\0')
            {
                mkdir(options->sarifpath, 0777);
                snprintf(sarif_file_name, sizeof sarif_file_name, "%s/%s.cake.sarif", options->sarifpath, basename(file_name));
            }
            else
            {
                snprintf(sarif_file_name, sizeof sarif_file_name, "%s.cake.sarif", file_name);
            }

            ctx.sarif_file = (FILE * _Owner _Opt) fopen(sarif_file_name, "w");
            if (ctx.sarif_file)
            {
                const char* begin_sarif =
                    "{\n"
                    "  \"version\": \"2.1.0\",\n"
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
                printf("cannot open Sarif output file '%s'\n", sarif_file_name);
                throw;
            }
        }

        tokens = tokenizer(&tctx, content, file_name, 0, TK_FLAG_NONE);

        if (tctx.n_errors > 0)
            throw;

        if (options->dump_tokens)
        {
            print_tokens(tokens.head);
        }

        prectx.options.diagnostic_stack.stack[prectx.options.diagnostic_stack.top_index].notes |= (1ULL << W_NOTE);
        ast.token_list = preprocessor(&prectx, &tokens, 0);

        report->warnings_count += prectx.n_warnings;
        report->error_count += prectx.n_errors;

        if (prectx.n_errors > 0)
        {
            throw;
        }

        if (options->dump_pptokens)
        {
            if (ast.token_list.head != NULL)
                print_tokens(ast.token_list.head);
        }

        if (options->preprocess_only)
        {
            const char* _Owner _Opt s2 = print_preprocessed_to_string2(ast.token_list.head);
            printf("%s", s2);
            free((void* _Owner _Opt)s2);
        }
        else
        {
            bool berror = false;
            ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
            if (berror || report->error_count > 0)
                throw;

            if (!options->no_output)
            {
                if (options->target == TARGET_C89_IL)
                {
                    struct osstream ss = { 0 };
                    struct d_visit_ctx ctx2 = { 0 };
                    ctx2.ast = ast;
                    d_visit(&ctx2, &ss);
                    s = ss.c_str; //MOVE
                    d_visit_ctx_destroy(&ctx2);
                }

                FILE* _Owner _Opt outfile = fopen(out_file_name, "w");
                if (outfile)
                {
                    if (s)
                        fprintf(outfile, "%s", s);

                    fclose(outfile);
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

#define SARIF_FOOTER                                                             \
    "      ],\n"                                                        \
    "      \"tool\": {\n"                                               \
    "        \"driver\": {\n"                                           \
    "          \"name\": \"cake\",\n"                                   \
    "          \"fullName\": \"cake code analysis\",\n"                 \
    "          \"version\": \"" CAKE_VERSION  "\",\n"                   \
    "          \"informationUri\": \"https://https://github.com/thradams/cake\"\n" \
    "        }\n"                                                       \
    "      }\n"                                                         \
    "    }\n"                                                           \
    "  ]\n"                                                             \
    "}\n"                                                               \
    "\n"
            fprintf(ctx.sarif_file, "%s", SARIF_FOOTER);
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
        snprintf(buf, sizeof buf, "%s_expected.c", file_name);

        char* _Owner _Opt content_expected = read_file(buf, false /*append new line*/);
        if (content_expected)
        {
            if (s && strcmp(content_expected, s) != 0)
            {
                printf("different");
                report->error_count++;
            }
            free(content_expected);
        }

        if (report->fatal_error_expected != 0)
        {
            if (report->last_diagnostics_ids[0] == report->fatal_error_expected)
            {
                report->error_count--;
            }
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

    parser_ctx_destroy(&ctx);
    free((void* _Owner _Opt)s);
    free(content);
    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);

    return report->error_count > 0;
}

static int compile_many_files(const char* file_name,
    struct options* options,
    const char* out_file_name,
    int argc,
    const char** argv,
    struct report* report)
{
    const char* const file_name_name = basename(file_name);
    const char* _Opt const file_name_extension = strrchr((char*)file_name_name, '.');

    if (file_name_extension == NULL)
    {
        assert(false);
    }

    int num_files = 0;

    char path[MYMAX_PATH] = { 0 };
    snprintf(path, sizeof path, "%s", file_name);
    dirname(path);
    DIR* _Owner _Opt dir = opendir(path);

    if (dir == NULL)
    {
        return errno;
    }

    struct dirent* _Opt dp;
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
            const char* _Opt const file_extension = strrchr((char*)file_name_iter, '.');

            if (file_name_extension &&
                file_extension &&
                strcmp(file_name_extension, file_extension) == 0)
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


                report->fatal_error_expected = report_local.fatal_error_expected;
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

    char executable_path[MAX_PATH - sizeof(CAKE_CFG_FNAME)] = { 0 };
    get_self_path(executable_path, sizeof(executable_path));
    dirname(executable_path);
    char cakeconfig_path[MAX_PATH] = { 0 };
    snprintf(cakeconfig_path, sizeof cakeconfig_path, "%s" CAKE_CFG_FNAME, executable_path);

    if (options.auto_config) //-autoconfig
    {
        report->ignore_this_report = true;
        return generate_config_file(cakeconfig_path);
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
        if (strcmp(argv[i], "-o") == 0 ||
            strcmp(argv[i], "-sarif-path") == 0)
        {
            //consumes next
            i++;
            continue;
        }

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
            struct report report_local = { 0 };
            compile_one_file(fullpath, &options, output_file, argc, argv, &report_local);

            report->fatal_error_expected = report_local.fatal_error_expected;
            report->error_count += report_local.error_count;
            report->warnings_count += report_local.warnings_count;
            report->info_count += report_local.info_count;
            report->test_succeeded += report_local.test_succeeded;
            report->test_failed += report_local.test_failed;
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

    _Opt struct parser_ctx ctx = { 0 };
    ctx.p_report = report;

    try
    {
        prectx.options = *options;
        prectx.macros.capacity = 5000;

        add_standard_macros(&prectx);

        ast.token_list = preprocessor(&prectx, &list, 0);
        if (prectx.n_errors != 0)
            throw;

        ctx.options = *options;

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
* given a string s, produce argv by modifying the input string
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

const char* _Owner _Opt compile_source(const char* pszoptions, const char* content, struct report* report)
{
    const char* argv[100] = { 0 };
    char string[200] = { 0 };
    snprintf(string, sizeof string, "exepath %s", pszoptions);

    const int argc = strtoargv(string, 10, argv);

    const char* _Owner _Opt s = NULL;

    struct preprocessor_ctx prectx = { 0 };
    struct ast ast = { 0 };
    struct options options = { .input = LANGUAGE_CAK };


    try
    {
        if (fill_options(&options, argc, argv) != 0)
        {
            throw;
        }

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

            if (options.target == TARGET_C89_IL)
            {
                struct osstream ss = { 0 };
                struct d_visit_ctx ctx2 = { 0 };
                ctx2.ast = ast;
                d_visit(&ctx2, &ss);
                s = ss.c_str; //MOVED                
                //ss.c_str = NULL;
                //ss_close(&ss);
                d_visit_ctx_destroy(&ctx2);
            }

        }
    }
    catch
    {
    }

    preprocessor_ctx_destroy(&prectx);

    ast_destroy(&ast);

    return s;
}

char* _Owner _Opt CompileText(const char* pszoptions, const char* content)
{
    /*
      This function is called by the web playground
    */
    printf(WHITE "Cake " CAKE_VERSION RESET "\n");
    printf(WHITE "cake %s main.c\n", pszoptions);
    struct report report = { 0 };
    return (char* _Owner _Opt)compile_source(pszoptions, content, &report);
}

void ast_destroy(_Dtor struct ast* ast)
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
            compiler_diagnostic(W_STYLE, ctx, token, NULL, "use snake_case for struct/union tags");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_pascal_case(token->lexeme))
        {
            compiler_diagnostic(W_STYLE, ctx, token, NULL, "use camelCase for struct/union tags");
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
            compiler_diagnostic(W_STYLE, ctx, token, NULL, "use snake_case for enum tags");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_pascal_case(token->lexeme))
        {
            compiler_diagnostic(W_STYLE, ctx, token, NULL, "use PascalCase for enum tags");
        }
    }
}

void naming_convention_function(struct parser_ctx* ctx, struct token* token)
{
    if (!parser_is_diagnostic_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (ctx->options.style == STYLE_CAKE)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(W_STYLE, ctx, token, NULL, "use snake_case for functions");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_pascal_case(token->lexeme))
        {
            compiler_diagnostic(W_STYLE, ctx, token, NULL, "use PascalCase for functions");
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
            if (type_is_const(type))
            {
                //all upper
            }
            else
            {
                if (token->lexeme[0] != 's' || token->lexeme[1] != '_')
                {
                    compiler_diagnostic(W_STYLE, ctx, token, NULL, "use prefix s_ for static global variables");
                }
            }
        }
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(W_STYLE, ctx, token, NULL, "use snake_case global variables");
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
            compiler_diagnostic(W_STYLE, ctx, token, NULL, "use snake_case for local variables");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_camel_case(token->lexeme))
        {
            compiler_diagnostic(W_STYLE, ctx, token, NULL, "use camelCase for local variables");
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
        compiler_diagnostic(W_STYLE, ctx, token, NULL, "use UPPERCASE for enumerators");
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
        compiler_diagnostic(W_STYLE, ctx, token, NULL, "use snake_case for struct members");
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
        compiler_diagnostic(W_STYLE, ctx, token, NULL, "use snake_case for arguments");
    }
}

static struct object* _Opt find_first_subobject_old(struct type* p_type_not_used, struct object* p_object, struct type* p_type_out, bool* sub_object_of_union)
{
    p_object = (struct object* _Opt) object_get_referenced(p_object);

    if (p_object->members == NULL)
    {
        *sub_object_of_union = false;

        struct type t = type_dup(&p_object->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);

        return p_object; //tODO
    }

    *sub_object_of_union = type_is_union(&p_object->type);
    struct type t = type_dup(&p_object->members->type);
    type_swap(&t, p_type_out);
    type_destroy(&t);

    return p_object->members; //tODO
}

static struct object* _Opt find_first_subobject(struct type* p_type_not_used, struct object* p_object, struct type* p_type_out, bool* sub_object_of_union)
{
    return find_first_subobject_old(p_type_not_used, p_object, p_type_out, sub_object_of_union);
}

static struct object* _Opt find_last_suboject_of_suboject_old(struct type* p_type_not_used, struct object* p_object, struct type* p_type_out)
{
    p_object = (struct object* _Opt) object_get_referenced(p_object);

    if (p_object->members == NULL)
    {
        struct type t = type_dup(&p_object->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
        return p_object; //tODO
    }

    struct object* _Opt it = p_object->members;

    while (it)
    {
        if (it->next == NULL)
            return find_last_suboject_of_suboject_old(p_type_not_used, it, p_type_out);

        it = it->next;
    }

    struct type t = type_dup(&p_object->type);
    type_swap(&t, p_type_out);
    type_destroy(&t);


    return p_object;
}

static struct object* _Opt find_last_suboject_of_suboject(struct type* p_type_not_used, struct object* p_object, struct type* p_type_out)
{
    return find_last_suboject_of_suboject_old(p_type_not_used, p_object, p_type_out);
}


static struct object* _Opt find_next_subobject_old(struct type* p_top_object_not_used,
    struct object* current_object,
    struct object* it,
    struct type* p_type_out,
    bool* sub_object_of_union)
{
    type_clear(p_type_out);

    if (it == NULL)
        return NULL;


    if (it->members)
    {
        *sub_object_of_union = type_is_union(&it->type);

        it = it->members;

        struct type t = type_dup(&it->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);

        return it;
    }

    for (;;)
    {
        if (it == NULL)
            break;

        struct object* next = it->next;

        if (next != NULL)
        {
            if (it->parent)
                *sub_object_of_union = type_is_union(&it->parent->type);
            it = next;
            break;
        }

        it = it->parent;
    }

    if (it != NULL)
    {
        struct type t = type_dup(&it->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
    }

    return it;
}


static struct object* _Opt find_next_subobject(struct type* p_top_object_not_used,
    struct object* current_object,
    struct object* it,
    struct type* p_type_out,
    bool* sub_object_of_union)
{
    return find_next_subobject_old(p_top_object_not_used,
    current_object,
    it,
    p_type_out,
    sub_object_of_union);
}
struct find_object_result
{
    struct object* _Opt object;
    struct type type;
};

static bool find_next_subobject_core(const struct type* p_type, struct object* obj, struct object* subobj, struct find_object_result* result)
{
    try
    {
        if (type_is_scalar(p_type))
        {
            if (result->object != NULL)
            {
                result->object = obj;
                result->type = type_dup(p_type);
                return true;
            }

        }

        if (subobj == obj)
        {
            result->object = obj;
            return false;
        }

        if (type_is_array(p_type))
        {
            struct type item_type = get_array_item_type(p_type);
            struct object* _Opt it = obj->members;
            for (; it; it = it->next)
            {
                if (find_next_subobject_core(&item_type, it, subobj, result))
                {
                    type_destroy(&item_type);
                    return true;
                }
            }
            type_destroy(&item_type);
            return false;
        }

        if (p_type->struct_or_union_specifier)
        {
            struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            if (p_struct_or_union_specifier == NULL)
            {
                //incomplete
                throw;
            }


            if (subobj == obj)
            {
                result->object = obj;
                //continua para achar o proximo
            }

            struct member_declaration* _Opt p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            struct object* _Opt member_object = obj->members;

            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (find_next_subobject_core(&p_member_declarator->declarator->type, member_object, subobj, result))
                            {
                                return true;
                            }

                            member_object = member_object->next;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type t = { 0 };
                        t.category = TYPE_CATEGORY_ITSELF;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;


                        if (find_next_subobject_core(&t, member_object, subobj, result))
                        {
                            return true;
                        }

                        type_destroy(&t);
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
        }
    }
    catch
    {
    }
    return false;
}

static struct object* _Opt find_designated_subobject(struct parser_ctx* ctx,
    struct type* p_current_object_type,
    struct object* current_object,
    struct designator* p_designator,
    bool is_constant,
    struct type* p_type_out2,
    bool not_error)
{
    try
    {
        if (type_is_struct_or_union(p_current_object_type))
        {
            assert(p_current_object_type->struct_or_union_specifier);

            struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
                get_complete_struct_or_union_specifier(p_current_object_type->struct_or_union_specifier);

            if (p_struct_or_union_specifier == NULL)
            {
                throw;
            }

            struct member_declaration* _Opt p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            struct member_declarator* _Opt p_member_declarator = NULL;

            const char* name = p_designator->token->lexeme;
            struct object* _Opt p_member_object = current_object->members;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    p_member_declarator = p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_member_declarator->declarator->name_opt &&
                                strcmp(p_member_declarator->declarator->name_opt->lexeme, name) == 0)
                            {
                                if (p_designator->next != NULL)
                                    return find_designated_subobject(ctx, &p_member_declarator->declarator->type, p_member_object, p_designator->next, is_constant, p_type_out2, false);
                                else
                                {
                                    struct type t = type_dup(&p_member_declarator->declarator->type);
                                    type_swap(&t, p_type_out2);
                                    type_destroy(&t);

                                    return p_member_object;
                                }
                            }
                        }
                        p_member_object = p_member_object->next;
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
                            struct type t = { 0 };
                            t.category = TYPE_CATEGORY_ITSELF;
                            t.struct_or_union_specifier = p_complete;
                            t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                            struct object* _Opt p = find_designated_subobject(ctx,
                                                                         &t,
                                                                         p_member_object,
                                                                         p_designator,
                                                                         is_constant,
                                                                         p_type_out2,
                                                                         true);
                            if (p)
                            {
                                type_destroy(&t);
                                return p;
                            }
                            p_member_object = p_member_object->next;
                            type_destroy(&t);
                        }
                    }
                }
                else
                {
                    //static_assert pragma...
                }
                p_member_declaration = p_member_declaration->next;
            }

            if (!not_error)
            {
                compiler_diagnostic(
                                              C_ERROR_STRUCT_MEMBER_NOT_FOUND,
                                              ctx,
                                              p_designator->token,
                                              NULL,
                                              "member '%s' not found in '%s'", name, p_struct_or_union_specifier->tag_name);
            }
            return NULL;
        }
        else if (type_is_array(p_current_object_type))
        {
            const bool compute_array_size = p_current_object_type->array_num_elements_expression == NULL;
            long long index = -1;
            long long max_index = -1;
            struct type array_item_type = get_array_item_type(p_current_object_type);

            struct object* _Opt member_obj = current_object->members;

            if (p_designator->constant_expression_opt)
            {
                index = object_to_signed_long_long(&p_designator->constant_expression_opt->object);

                if (index > max_index)
                {
                    max_index = index;
                    if (compute_array_size)
                    {
                        member_obj = object_extend_array_to_index(&array_item_type, current_object, max_index, is_constant);
                    }
                }

                member_obj = object_get_member(current_object, index);
                if (member_obj == NULL)
                {
                    if (index < 0)
                    {
                        compiler_diagnostic(
                                                  C_ERROR_STRUCT_MEMBER_NOT_FOUND,
                                                  ctx,
                                                  p_designator->constant_expression_opt->first_token,
                                                  NULL,
                                                  "array designator value '%d' is negative", index);
                    }
                    else if (index > p_current_object_type->num_of_elements)
                    {
                        compiler_diagnostic(
                                                  C_ERROR_STRUCT_MEMBER_NOT_FOUND,
                                                  ctx,
                                                  p_designator->constant_expression_opt->first_token,
                                                  NULL,
                                                  "array index '%d' in initializer exceeds array bounds", index);
                    }

                    type_destroy(&array_item_type);
                    return NULL;
                }


                if (p_designator->next != NULL)
                {
                    struct object* _Opt p =
                        find_designated_subobject(ctx, &array_item_type, member_obj, p_designator->next, is_constant, p_type_out2, false);

                    type_destroy(&array_item_type);
                    return p;
                }
                else
                {
                    type_swap(p_type_out2, &array_item_type);
                    type_destroy(&array_item_type);
                }

                return member_obj;
            }
            else
            {
                //oops
            }
            type_destroy(&array_item_type);
        }
    }
    catch
    {

    }
    return NULL;
}

int initializer_init_new(struct parser_ctx* ctx,
                        struct type* p_type, /*in (in/out for arrays [])*/
                        struct object* object, /*in (in/out for arrays [])*/
                        struct initializer* initializer, /*rtocar para initializer item??*/
                        bool is_constant,
                        bool requires_constant_initialization);

static struct initializer_list_item* _Opt find_innner_initializer_list_item(struct braced_initializer* braced_initializer)
{
    assert(braced_initializer->initializer_list);

    struct initializer_list_item* _Opt p_initializer_list_item = braced_initializer->initializer_list->head;

    while (p_initializer_list_item->initializer->braced_initializer)
    {
        //int i = {{1}};
        p_initializer_list_item = p_initializer_list_item->initializer->braced_initializer->initializer_list->head;

        if (p_initializer_list_item == NULL)
        {
            assert(false);
            return NULL;
        }

        if (p_initializer_list_item->next == NULL)
            return p_initializer_list_item;

        p_initializer_list_item = p_initializer_list_item->next;
    }

    return p_initializer_list_item;
}

NODISCARD
static int braced_initializer_new(struct parser_ctx* ctx,
                                  struct type* p_current_object_type,
                                  struct object* current_object,
                                  struct braced_initializer* braced_initializer,
                                  bool is_constant,
                                  bool requires_constant_initialization)
{
    try
    {
        if (braced_initializer->initializer_list == NULL)
        {
            object_default_initialization(current_object, is_constant);
            return 0;
        }

        if (!type_is_union(p_current_object_type))
        {
            object_default_initialization(current_object, is_constant);
        }

        if (type_is_scalar(p_current_object_type))
        {
            struct initializer_list_item* _Opt p_initializer_list_item =
                find_innner_initializer_list_item(braced_initializer);

            if (p_initializer_list_item == NULL)
            {
                return 0;
            }

            if (p_initializer_list_item->initializer->assignment_expression != NULL)
            {
                if (object_set(ctx,
                    current_object, p_initializer_list_item->initializer->assignment_expression,
                    &p_initializer_list_item->initializer->assignment_expression->object,
                    is_constant,
                    requires_constant_initialization) != 0)
                {
                    throw;
                }
            }

            p_initializer_list_item = p_initializer_list_item->next;

            if (p_initializer_list_item != NULL)
            {
                compiler_diagnostic(W_TO_MANY_INITIALIZERS,
                                            ctx,
                                            p_initializer_list_item->initializer->first_token,
                                            NULL,
                                            "warning: excess elements in initializer");
            }
            return 0;
        }


        //TODO Array char

        struct object* const _Opt parent_copy = current_object->parent;
        current_object->parent = NULL; //to be only here
        struct initializer_list_item* _Opt p_initializer_list_item = braced_initializer->initializer_list->head;
        int array_to_expand_index = -1;
        int array_to_expand_max_index = -1;
        bool compute_array_size = false;
        struct type array_item_type = { 0 };
        if (type_is_array(p_current_object_type))
        {
            array_item_type = get_array_item_type(p_current_object_type);
            compute_array_size = p_current_object_type->array_num_elements_expression == NULL;
            if (type_is_char(&array_item_type))
            {
                struct initializer_list_item* _Opt p_initializer_list_item2 = find_innner_initializer_list_item(braced_initializer);
                if (p_initializer_list_item2 == NULL)
                {
                    type_destroy(&array_item_type);
                    return 0;
                }
                if (p_initializer_list_item2->initializer->assignment_expression != NULL)
                {
                    if (p_initializer_list_item2->initializer->assignment_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL)
                    {
                        size_t num_of_elements =
                            p_initializer_list_item2->initializer->assignment_expression->type.num_of_elements;

                        if (compute_array_size)
                        {
                            object_extend_array_to_index(&array_item_type, current_object, num_of_elements - 1, is_constant);
                        }

                        if (object_set(ctx,
                            current_object,
                            p_initializer_list_item2->initializer->assignment_expression,
                            &p_initializer_list_item2->initializer->assignment_expression->object,
                            is_constant,
                            requires_constant_initialization) != 0)
                        {
                            type_destroy(&array_item_type);
                            throw;
                        }
                        //current_object->type2.num_of_elements = num_of_elements;
                        p_current_object_type->num_of_elements = num_of_elements;

                        //printf("\n");
                        //object_print_to_debug(current_object);
                        type_destroy(&array_item_type);
                        return 0;
                    }
                }
            }
        }

        struct object* _Opt p_subobject = NULL;

        for (;;)
        {
            bool is_subobject_of_union = false;
            struct type subobject_type = { 0 };

            if (p_initializer_list_item == NULL)
            {
                break;
            }

            if (p_initializer_list_item->designation)
            {
                if (compute_array_size)
                {
                    array_to_expand_index = object_to_signed_long_long(&p_initializer_list_item->designation->designator_list->head->constant_expression_opt->object);

                    if (array_to_expand_index > array_to_expand_max_index)
                        array_to_expand_max_index = array_to_expand_index;

                    object_extend_array_to_index(&array_item_type, current_object, array_to_expand_max_index, is_constant);
                }
                is_subobject_of_union = type_is_union(&subobject_type);
                p_subobject = find_designated_subobject(ctx, p_current_object_type, current_object, p_initializer_list_item->designation->designator_list->head, is_constant, &subobject_type, false);
                if (p_subobject == NULL)
                {
                    // already have the error, need not say that it was not consumed
                    p_initializer_list_item = p_initializer_list_item->next;
                    type_destroy(&subobject_type);
                    break;
                }
            }
            else
            {
                if (compute_array_size)
                {

                    struct object* _Opt po = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                    if (po == NULL)
                    {
                        array_to_expand_index++;
                        if (array_to_expand_index > array_to_expand_max_index)
                            array_to_expand_max_index = array_to_expand_index;

                        object_extend_array_to_index(&array_item_type, current_object, array_to_expand_max_index, is_constant);
                    }
                }

                if (p_subobject == NULL)
                {

                    p_subobject = find_first_subobject(p_current_object_type, current_object, &subobject_type, &is_subobject_of_union);
                }
                else
                {
                    p_subobject = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                }
            }

            if (p_subobject == NULL)
            {
                type_destroy(&subobject_type);
                break;
            }



            if (p_initializer_list_item->initializer->braced_initializer)
            {
                if (braced_initializer_new(ctx,
                    &subobject_type,
                    p_subobject,
                    p_initializer_list_item->initializer->braced_initializer,
                    is_constant,
                    requires_constant_initialization) != 0)
                {
                    type_destroy(&array_item_type);
                    type_destroy(&subobject_type);
                    throw;
                }

                struct type t = { 0 };

                is_subobject_of_union = type_is_union(&subobject_type);
                p_subobject = find_last_suboject_of_suboject(&subobject_type, p_subobject, &t);
                type_swap(&t, &subobject_type);
                type_destroy(&t);
            }
            else if (p_initializer_list_item->initializer->assignment_expression)
            {
                bool entire_object_initialized = false;

                if (type_is_array_of_char(&subobject_type) &&
                    p_initializer_list_item->initializer->assignment_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL)
                {
                    /*
                    struct X { int i; char text[4]; };
                    constexpr struct X x = {1, "abc"};
                    */
                    entire_object_initialized = true;
                }
                else if (type_is_array(&subobject_type))
                {
                    while (type_is_array(&subobject_type))
                    {
                        /*
                          struct X { int i[2]; };
                          int a[2]={};
                          struct  X b = { a };    //error
                        */
                        //sub_object_of_union = false;
                        p_subobject = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                    }
                }
                else if (type_is_struct_or_union(&subobject_type))
                {
                    if (type_is_struct_or_union(&p_initializer_list_item->initializer->assignment_expression->type))
                    {
                        //mesmo tipo
                        entire_object_initialized = true;
                    }
                    else
                    {
                        p_subobject = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                    }
                }

                if (object_set(ctx,
                    p_subobject,
                    p_initializer_list_item->initializer->assignment_expression,
                    &p_initializer_list_item->initializer->assignment_expression->object,
                    is_constant,
                    requires_constant_initialization) != 0)
                {
                    type_destroy(&array_item_type);
                    type_destroy(&subobject_type);
                    throw;
                }


                if (is_subobject_of_union)
                {
                    assert(p_subobject);
                    assert(p_subobject->parent);
                    struct type t = { 0 };
                    is_subobject_of_union = true;
                    p_subobject = find_last_suboject_of_suboject(&p_subobject->parent->type,
                                                                 p_subobject->parent,
                                                                 &t);
                    type_swap(&t, &subobject_type);
                    type_destroy(&t);
                    if (p_subobject)
                    {
                        type_destroy(&subobject_type);
                        subobject_type = type_dup(&p_subobject->type);
                    }

                }
                else if (entire_object_initialized)
                {
                    assert(p_subobject);

                    struct type t = { 0 };
                    is_subobject_of_union = type_is_union(p_current_object_type);
                    p_subobject = find_last_suboject_of_suboject(&subobject_type, p_subobject, &t);
                    type_swap(&t, &subobject_type);
                    type_destroy(&t);
                    if (p_subobject)
                    {
                        type_destroy(&subobject_type);
                        subobject_type = type_dup(&p_subobject->type);
                    }
                }
            }
            p_initializer_list_item = p_initializer_list_item->next;
            type_destroy(&subobject_type);
        }

        if (p_initializer_list_item != NULL)
        {
            compiler_diagnostic(W_TO_MANY_INITIALIZERS,
                                        ctx,
                                        p_initializer_list_item->initializer->first_token,
                                        NULL,
                                        "warning: excess elements in initializer");

        }
        if (compute_array_size)
        {
            current_object->type.num_of_elements = array_to_expand_max_index + 1;
            p_current_object_type->num_of_elements = array_to_expand_max_index + 1;
        }

        current_object->parent = parent_copy; //restore
        type_destroy(&array_item_type);
    }
    catch
    {
        return 1;
    }

    return 0;
}

int initializer_init_new(struct parser_ctx* ctx,
                        struct type* p_type,
                        struct object* object, /*in (in/out for arrays [])*/
                        struct initializer* initializer, /*rtocar para initializer item??*/
                        bool is_constant,
                        bool requires_constant_initialization)
{
    try
    {
        if (initializer->assignment_expression != NULL)
        {
            //types must be compatible
            if (object_set(ctx,
                object,
                initializer->assignment_expression,
                &initializer->assignment_expression->object,
                is_constant,
                requires_constant_initialization) != 0)
            {
                throw;
            }
        }
        else if (initializer->braced_initializer)
        {
            if (braced_initializer_new(ctx,
                p_type,
                object,
                initializer->braced_initializer,
                is_constant,
                requires_constant_initialization) != 0)
            {
                throw;
            }
        }
    }
    catch
    {
        return 1;
    }

    return 0;
}