
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
#include "wasm_visit.h"
#include "flow_visit.h"
#include <errno.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif


#include "visit.h"
#include <time.h>




const char* object_state_to_string(enum object_state e)
{
    switch (e)
    {
        case OBJECT_STATE_STRUCT: return "";
        case OBJECT_STATE_UNINITIALIZED: return "UNINITIALIZED";
        case OBJECT_STATE_ZERO: return "ZERO";
        case OBJECT_STATE_UNKNOWN: return "UNKNOWN";
        case OBJECT_STATE_NOT_ZERO: return "NOT_ZERO";
        case OBJECT_STATE_MOVED: return "MOVED";
        case OBJECT_STATE_NULL_OR_UNINITIALIZED: return "NULL_OR_UNINITIALIZED";
    }
    return "";
}
struct defer_statement* owner defer_statement(struct parser_ctx* ctx);


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

static bool parser_is_warning_enabled(const struct parser_ctx* ctx, enum warning w)
{
    return
        (ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index] & w) != 0;

}

static void check_open_brace_style(struct parser_ctx* ctx, struct token* token)
{
    //token points to {

    if (token->level == 0 &&
        !(token->flags & TK_FLAG_MACRO_EXPANDED) &&
        token->type == '{' &&
        parser_is_warning_enabled(ctx, W_STYLE))
    {
        if (ctx->options.style == STYLE_CAKE)
        {
            if (token->prev->type == TK_BLANKS &&
                token->prev->prev->type == TK_NEWLINE)
            {
            }
            else
            {
                compiler_set_info_with_token(W_STYLE, ctx, token, "not following correct brace style {");
            }
        }
    }
}

static void check_close_brace_style(struct parser_ctx* ctx, struct token* token)
{
    //token points to {

    if (token->level == 0 &&
        !(token->flags & TK_FLAG_MACRO_EXPANDED) &&
        token->type == '}' &&
        parser_is_warning_enabled(ctx, W_STYLE))
    {
        if (ctx->options.style == STYLE_CAKE)
        {
            if (token->prev->type == TK_BLANKS &&
                token->prev->prev->type == TK_NEWLINE)
            {
            }
            else
            {
                compiler_set_info_with_token(W_STYLE, ctx, token, "not following correct close brace style }");
            }
        }
    }
}

static void check_func_open_brace_style(struct parser_ctx* ctx, struct token* token)
{
    //token points to {

    if (token->level == 0 &&
        !(token->flags & TK_FLAG_MACRO_EXPANDED) &&
        token->type == '{' &&
        parser_is_warning_enabled(ctx, W_STYLE))
    {
        if (ctx->options.style == STYLE_CAKE)
        {
            if (token->prev->type == TK_NEWLINE)
            {
            }
            else
            {
                compiler_set_info_with_token(W_STYLE, ctx, token, "not following correct brace style {");
            }
        }
    }
}

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
                compiler_set_info_with_token(W_STYLE, ctx, token, "not following correct close brace style }");
            }
        }
    }
}


#ifdef TEST
int printf_nothing(const char* fmt, ...) { return 0; }
#endif

void scope_destroy(implicit struct scope* obj_owner p)
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
        //pnew->prev = list->tail;
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


void parser_ctx_destroy(implicit struct parser_ctx* obj_owner ctx)
{

}



void compiler_set_error_with_token(enum error error, struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    ctx->p_report->error_count++;
    ctx->p_report->last_error = error;
    char buffer[200] = {0};

#ifndef TEST

    if (p_token)
        print_position(p_token->token_origin->lexeme, p_token->line, p_token->col);


    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    printf(LIGHTRED "error: " WHITE "%s\n", buffer);

    print_line_and_token(p_token);
#endif
    const char* func_name = "module";
    if (ctx->p_current_function_opt)
    {
        func_name = ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name->lexeme;
    }

    if (ctx->sarif_file)
    {
        const char* file_name = "?";
        int line = 0;
        int col = 0;
        if (p_token)
        {
            file_name = p_token->token_origin->lexeme;
            line = p_token->line;
            col = p_token->col;
        }

        if (ctx->p_report->error_count + ctx->p_report->warnings_count + ctx->p_report->info_count > 1)
        {
            fprintf(ctx->sarif_file, ",\n");
        }

        fprintf(ctx->sarif_file, "   {\n");
        fprintf(ctx->sarif_file, "     \"ruleId\":\"%s\",\n", "error");
        fprintf(ctx->sarif_file, "     \"level\":\"error\",\n");
        fprintf(ctx->sarif_file, "     \"message\": {\n");
        fprintf(ctx->sarif_file, "            \"text\": \"%s\"\n", buffer);
        fprintf(ctx->sarif_file, "      },\n");
        fprintf(ctx->sarif_file, "      \"locations\": [\n");
        fprintf(ctx->sarif_file, "       {\n");

        fprintf(ctx->sarif_file, "       \"physicalLocation\": {\n");

        fprintf(ctx->sarif_file, "             \"artifactLocation\": {\n");
        fprintf(ctx->sarif_file, "                 \"uri\": \"file:///%s\"\n", file_name);
        fprintf(ctx->sarif_file, "              },\n");

        fprintf(ctx->sarif_file, "              \"region\": {\n");
        fprintf(ctx->sarif_file, "                  \"startLine\": %d,\n", line);
        fprintf(ctx->sarif_file, "                  \"startColumn\": %d,\n", col);
        fprintf(ctx->sarif_file, "                  \"endLine\": %d,\n", line);
        fprintf(ctx->sarif_file, "                  \"endColumn\": %d\n", col);
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

}


_Bool compiler_set_warning_with_token(enum warning w, struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    if (w != W_NONE)
    {
        if (p_token && p_token->level != 0)
        {
            /*we dont warning code inside includes*/
            return false;
        }

        if (!parser_is_warning_enabled(ctx, w))
        {
            return false;
        }
    }

    ctx->p_report->warnings_count++;
    ctx->p_report->last_warning |= w;

    const char* func_name = "module";
    if (ctx->p_current_function_opt)
    {
        func_name = ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name->lexeme;
    }

    char buffer[200] = {0};

#ifndef TEST
    print_position(p_token->token_origin->lexeme, p_token->line, p_token->col);


    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (w != W_NONE)
    {
        printf(LIGHTMAGENTA "warning: " WHITE "%s [" LIGHTMAGENTA "-W%s" WHITE "]\n" RESET, buffer, get_warning_name(w));
    }
    else
    {
        printf(LIGHTMAGENTA "warning: " WHITE "%s\n" RESET, buffer);
    }
    print_line_and_token(p_token);
#endif

    if (ctx->sarif_file)
    {
        if (ctx->p_report->error_count + ctx->p_report->warnings_count + ctx->p_report->info_count > 1)
        {
            fprintf(ctx->sarif_file, ",\n");
        }

        fprintf(ctx->sarif_file, "   {\n");
        fprintf(ctx->sarif_file, "     \"ruleId\":\"%s\",\n", get_warning_name(w));
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


void compiler_set_info_with_token(enum warning w, struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    if (w != W_NONE)
    {
        if (p_token->level != 0)
        {
            /*we dont warning code inside includes*/
            return;
        }

        if (!parser_is_warning_enabled(ctx, w))
        {
            return;
        }
    }
    const char* func_name = "module";
    if (ctx->p_current_function_opt)
    {
        func_name = ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name->lexeme;
    }
    ctx->p_report->info_count++;
    ctx->p_report->last_warning |= w;
    char buffer[200] = {0};

#ifndef TEST
    print_position(p_token->token_origin->lexeme, p_token->line, p_token->col);

    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    printf(LIGHTCYAN "note: " WHITE "%s\n", buffer);
    print_line_and_token(p_token);
#endif // !TEST


    if (ctx->sarif_file)
    {
        if (ctx->p_report->error_count + ctx->p_report->warnings_count + ctx->p_report->info_count > 1)
        {
            fprintf(ctx->sarif_file, ",\n");
        }

        fprintf(ctx->sarif_file, "   {\n");
        fprintf(ctx->sarif_file, "     \"ruleId\":\"%s\",\n", "info");
        fprintf(ctx->sarif_file, "     \"level\":\"note\",\n");
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

}


void print_scope(struct scope_list* e)
{
    printf("--- begin of scope---\n");
    struct scope* p = e->head;
    int level = 0;
    while (p)
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

bool  first_of_struct_or_union_token(struct token* token)
{
    return token->type == TK_KEYWORD_STRUCT || token->type == TK_KEYWORD_UNION;
}

bool  first_of_struct_or_union(struct parser_ctx* ctx)
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
        p_token->type == TK_KEYWORD__OWNER ||
        p_token->type == TK_KEYWORD__OBJ_OWNER ||
        p_token->type == TK_KEYWORD__VIEW;

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
        *ppscope_opt = NULL; //out

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
                return best; //OK bem completo
            else
            {
                //nao eh completo vamos continuar subindo
            }

        }
        scope = scope->previous;
    }
    return best; //mesmo que nao seja tao completo vamos retornar.    
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
            return (struct declarator*) p_init_declarator->p_declarator;
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
        //nao precisa verificar
        return false;
    }
    if (p_token->flags & TK_FLAG_IDENTIFIER_IS_TYPEDEF)
    {
        //ja foi verificado que eh typedef
        return true;
    }
    if (p_token->flags & TK_FLAG_IDENTIFIER_IS_NOT_TYPEDEF)
    {
        //ja foi verificado que NAO eh typedef
        return false;
    }


    struct declarator* pdeclarator = find_declarator(ctx, p_token->lexeme, NULL);

    //pdeclarator->declaration_specifiers->
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

    //if (ctx->)
    return p_token->type == TK_KEYWORD_VOID ||
        p_token->type == TK_KEYWORD_CHAR ||
        p_token->type == TK_KEYWORD_SHORT ||
        p_token->type == TK_KEYWORD_INT ||
        p_token->type == TK_KEYWORD_LONG ||

        //microsoft extension
        p_token->type == TK_KEYWORD__INT8 ||
        p_token->type == TK_KEYWORD__INT16 ||
        p_token->type == TK_KEYWORD__INT32 ||
        p_token->type == TK_KEYWORD__INT64 ||
        //end microsoft

        p_token->type == TK_KEYWORD_FLOAT ||
        p_token->type == TK_KEYWORD_DOUBLE ||
        p_token->type == TK_KEYWORD_SIGNED ||
        p_token->type == TK_KEYWORD_UNSIGNED ||
        p_token->type == TK_KEYWORD__BOOL ||
        p_token->type == TK_KEYWORD__COMPLEX ||
        p_token->type == TK_KEYWORD__DECIMAL32 ||
        p_token->type == TK_KEYWORD__DECIMAL64 ||
        p_token->type == TK_KEYWORD__DECIMAL128 ||
        p_token->type == TK_KEYWORD_TYPEOF || //C23
        p_token->type == TK_KEYWORD_TYPEOF_UNQUAL || //C23
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

    return
        ctx->current->type == TK_KEYWORD_REPEAT || /*extension*/
        ctx->current->type == TK_KEYWORD_WHILE ||
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



bool first_of_static_assert_declaration(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_KEYWORD__STATIC_ASSERT;
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
            if (strcmp("alignof", text) == 0) result = TK_KEYWORD__ALIGNOF;
            else if (strcmp("auto", text) == 0) result = TK_KEYWORD_AUTO;
            else if (strcmp("alignas", text) == 0) result = TK_KEYWORD__ALIGNAS; /*C23 alternate spelling _Alignas*/
            else if (strcmp("alignof", text) == 0) result = TK_KEYWORD__ALIGNAS; /*C23 alternate spelling _Alignof*/
            break;
        case 'b':
            if (strcmp("break", text) == 0) result = TK_KEYWORD_BREAK;
            else if (strcmp("bool", text) == 0) result = TK_KEYWORD__BOOL; /*C23 alternate spelling _Bool*/

            break;
        case 'c':
            if (strcmp("case", text) == 0) result = TK_KEYWORD_CASE;
            else if (strcmp("char", text) == 0) result = TK_KEYWORD_CHAR;
            else if (strcmp("const", text) == 0) result = TK_KEYWORD_CONST;
            else if (strcmp("constexpr", text) == 0) result = TK_KEYWORD_CONSTEXPR;
            else if (strcmp("continue", text) == 0) result = TK_KEYWORD_CONTINUE;
            else if (strcmp("catch", text) == 0) result = TK_KEYWORD_CATCH;
            break;
        case 'd':
            if (strcmp("default", text) == 0) result = TK_KEYWORD_DEFAULT;
            else if (strcmp("do", text) == 0) result = TK_KEYWORD_DO;
            else if (strcmp("defer", text) == 0) result = TK_KEYWORD_DEFER;
            else if (strcmp("double", text) == 0) result = TK_KEYWORD_DOUBLE;
            break;
        case 'e':
            if (strcmp("else", text) == 0) result = TK_KEYWORD_ELSE;
            else if (strcmp("enum", text) == 0) result = TK_KEYWORD_ENUM;
            else if (strcmp("extern", text) == 0) result = TK_KEYWORD_EXTERN;
            break;
        case 'f':
            if (strcmp("float", text) == 0) result = TK_KEYWORD_FLOAT;
            else if (strcmp("for", text) == 0) result = TK_KEYWORD_FOR;
            else if (strcmp("false", text) == 0) result = TK_KEYWORD_FALSE;
            break;
        case 'g':
            if (strcmp("goto", text) == 0) result = TK_KEYWORD_GOTO;
            break;
        case 'i':
            if (strcmp("if", text) == 0) result = TK_KEYWORD_IF;
            else if (strcmp("inline", text) == 0) result = TK_KEYWORD_INLINE;
            else if (strcmp("int", text) == 0) result = TK_KEYWORD_INT;
            break;
        case 'n':
            if (strcmp("nullptr", text) == 0) result = TK_KEYWORD_NULLPTR;
            break;
        case 'l':
            if (strcmp("long", text) == 0) result = TK_KEYWORD_LONG;
            break;
        case 'r':
            if (strcmp("register", text) == 0) result = TK_KEYWORD_REGISTER;
            else if (strcmp("restrict", text) == 0) result = TK_KEYWORD_RESTRICT;
            else if (strcmp("return", text) == 0) result = TK_KEYWORD_RETURN;
            else if (strcmp("repeat", text) == 0) result = TK_KEYWORD_REPEAT;
            break;
        case 's':
            if (strcmp("short", text) == 0) result = TK_KEYWORD_SHORT;
            else if (strcmp("signed", text) == 0) result = TK_KEYWORD_SIGNED;
            else if (strcmp("sizeof", text) == 0) result = TK_KEYWORD_SIZEOF;
            else if (strcmp("static", text) == 0) result = TK_KEYWORD_STATIC;
            else if (strcmp("struct", text) == 0) result = TK_KEYWORD_STRUCT;
            else if (strcmp("switch", text) == 0) result = TK_KEYWORD_SWITCH;
            else if (strcmp("static_assert", text) == 0) result = TK_KEYWORD__STATIC_ASSERT; /*C23 alternate spelling _Static_assert*/
            else if (strcmp("static_debug", text) == 0) result = TK_KEYWORD_STATIC_DEBUG;

            break;
        case 't':
            if (strcmp("typedef", text) == 0) result = TK_KEYWORD_TYPEDEF;
            else if (strcmp("typeof", text) == 0) result = TK_KEYWORD_TYPEOF; /*C23*/
            else if (strcmp("typeof_unqual", text) == 0) result = TK_KEYWORD_TYPEOF_UNQUAL; /*C23*/
            else if (strcmp("true", text) == 0) result = TK_KEYWORD_TRUE; /*C23*/
            else if (strcmp("thread_local", text) == 0) result = TK_KEYWORD__THREAD_LOCAL; /*C23 alternate spelling _Thread_local*/
            else if (strcmp("try", text) == 0) result = TK_KEYWORD_TRY;
            else if (strcmp("throw", text) == 0) result = TK_KEYWORD_THROW;
            break;
        case 'u':
            if (strcmp("union", text) == 0) result = TK_KEYWORD_UNION;
            else if (strcmp("unsigned", text) == 0) result = TK_KEYWORD_UNSIGNED;
            break;
        case 'v':
            if (strcmp("void", text) == 0) result = TK_KEYWORD_VOID;
            else if (strcmp("volatile", text) == 0) result = TK_KEYWORD_VOLATILE;
            break;
        case 'w':
            if (strcmp("while", text) == 0) result = TK_KEYWORD_WHILE;
            break;
        case '_':

            //begin microsoft
            if (strcmp("__int8", text) == 0) result = TK_KEYWORD__INT8;
            else if (strcmp("__int16", text) == 0) result = TK_KEYWORD__INT16;
            else if (strcmp("__int32", text) == 0) result = TK_KEYWORD__INT32;
            else if (strcmp("__int64", text) == 0) result = TK_KEYWORD__INT64;
            else if (strcmp("__forceinline", text) == 0) result = TK_KEYWORD_INLINE;
            else if (strcmp("__inline", text) == 0) result = TK_KEYWORD_INLINE;
            else if (strcmp("_asm", text) == 0 || strcmp("__asm", text) == 0) result = TK_KEYWORD__ASM;
            else if (strcmp("__alignof", text) == 0) result = TK_KEYWORD__ALIGNOF;
            //
            //end microsoft

            /*EXPERIMENTAL EXTENSION*/
            else if (strcmp("_has_attr", text) == 0) result = TK_KEYWORD_ATTR_HAS;
            else if (strcmp("_add_attr", text) == 0) result = TK_KEYWORD_ATTR_ADD;
            else if (strcmp("_del_attr", text) == 0) result = TK_KEYWORD_ATTR_REMOVE;
            /*EXPERIMENTAL EXTENSION*/

            /*TRAITS EXTENSION*/
            else if (strcmp("_is_lvalue", text) == 0) result = TK_KEYWORD_IS_LVALUE;
            else if (strcmp("_is_const", text) == 0) result = TK_KEYWORD_IS_CONST;
            else if (strcmp("_is_owner", text) == 0) result = TK_KEYWORD_IS_OWNER;
            else if (strcmp("_is_pointer", text) == 0) result = TK_KEYWORD_IS_POINTER;
            else if (strcmp("_is_array", text) == 0) result = TK_KEYWORD_IS_ARRAY;
            else if (strcmp("_is_function", text) == 0) result = TK_KEYWORD_IS_FUNCTION;
            else if (strcmp("_is_arithmetic", text) == 0) result = TK_KEYWORD_IS_ARITHMETIC;
            else if (strcmp("_is_floating_point", text) == 0) result = TK_KEYWORD_IS_FLOATING_POINT;
            else if (strcmp("_is_integral", text) == 0) result = TK_KEYWORD_IS_INTEGRAL;
            else if (strcmp("_is_scalar", text) == 0) result = TK_KEYWORD_IS_SCALAR;
            /*TRAITS EXTENSION*/

            else if (strcmp("_is_same", text) == 0) result = TK_KEYWORD_IS_SAME;
            else if (strcmp("_Alignof", text) == 0) result = TK_KEYWORD__ALIGNOF;
            else if (strcmp("_Alignas", text) == 0) result = TK_KEYWORD__ALIGNAS;
            else if (strcmp("_Atomic", text) == 0) result = TK_KEYWORD__ATOMIC;
            else if (strcmp("_Bool", text) == 0) result = TK_KEYWORD__BOOL;
            else if (strcmp("_Complex", text) == 0) result = TK_KEYWORD__COMPLEX;
            else if (strcmp("_Decimal128", text) == 0) result = TK_KEYWORD__DECIMAL32;
            else if (strcmp("_Decimal64", text) == 0) result = TK_KEYWORD__DECIMAL64;
            else if (strcmp("_Decimal128", text) == 0) result = TK_KEYWORD__DECIMAL128;
            else if (strcmp("_Generic", text) == 0) result = TK_KEYWORD__GENERIC;
            else if (strcmp("_Imaginary", text) == 0) result = TK_KEYWORD__IMAGINARY;
            else if (strcmp("_Noreturn", text) == 0) result = TK_KEYWORD__NORETURN; /*_Noreturn deprecated C23*/
            else if (strcmp("_Static_assert", text) == 0) result = TK_KEYWORD__STATIC_ASSERT;
            else if (strcmp("_Thread_local", text) == 0) result = TK_KEYWORD__THREAD_LOCAL;
            else if (strcmp("_BitInt", text) == 0) result = TK_KEYWORD__BITINT; /*(C23)*/

            else if (strcmp("_Owner", text) == 0) result = TK_KEYWORD__OWNER; /*extension*/
            else if (strcmp("_Obj_owner", text) == 0) result = TK_KEYWORD__OBJ_OWNER; /*extension*/
            else if (strcmp("_View", text) == 0) result = TK_KEYWORD__VIEW; /*extension*/
            else if (strcmp("_Move", text) == 0) result = TK_KEYWORD__MOVE; /*extension*/
            else if (strcmp("_Implicit", text) == 0) result = TK_KEYWORD__IMPLICIT; /*extension*/


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
        //talvez desse para remover antesisso..
        //assim que sai do tetris
        //virou passado
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
    //p signopt digit - sequence
    //P   signopt digit - sequence

    stream_match(stream); //p or P
    if (stream->current[0] == '+' || stream->current[0] == '-')
    {
        stream_match(stream); //p or P
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
                //erro
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
    else if (stream->current[0] == '0') //octal
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
    else if (is_nonzero_digit(stream)) //decimal
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
    struct stream stream = {.source = lexeme, .current = lexeme, .line = 1, .col = 1};
    return parse_number_core(&stream, flags_opt);
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

        if (ctx->current && strcmp(ctx->current->lexeme, "CAKE") == 0)
        {
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);
        }

        if (ctx->current && strcmp(ctx->current->lexeme, "diagnostic") == 0)
        {
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);

            if (ctx->current && strcmp(ctx->current->lexeme, "push") == 0)
            {
                //#pragma GCC diagnostic push
                if (ctx->options.enabled_warnings_stack_top_index <
                    sizeof(ctx->options.enabled_warnings_stack) / sizeof(ctx->options.enabled_warnings_stack[0]))
                {
                    ctx->options.enabled_warnings_stack_top_index++;
                    ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index] =
                        ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index - 1];
                }
                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);
            }
            else if (ctx->current && strcmp(ctx->current->lexeme, "pop") == 0)
            {
                //#pragma CAKE diagnostic pop
                if (ctx->options.enabled_warnings_stack_top_index > 0)
                {
                    ctx->options.enabled_warnings_stack_top_index--;
                }
                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);
            }
            else if (ctx->current && strcmp(ctx->current->lexeme, "warning") == 0)
            {
                //#pragma CAKE diagnostic warning "-Wenum-compare"

                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);

                if (ctx->current && ctx->current->type == TK_STRING_LITERAL)
                {
                    enum warning  w = get_warning_flag(ctx->current->lexeme + 1 + 2);
                    ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index] |= w;
                }
            }
            else if (ctx->current && strcmp(ctx->current->lexeme, "ignore") == 0)
            {
                //#pragma CAKE diagnostic ignore "-Wenum-compare"

                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);

                if (ctx->current && ctx->current->type == TK_STRING_LITERAL)
                {
                    enum warning  w = get_warning_flag(ctx->current->lexeme + 1 + 2);
                    ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index] &= ~w;
                }
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
        token_promote(ctx->current); //transforma para token de parser
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



int parser_match_tk(struct parser_ctx* ctx, enum token_type type)
{
    int error = 0;
    if (ctx->current != NULL)
    {
        if (ctx->current->type != type)
        {
            compiler_set_error_with_token(C_UNEXPECTED_TOKEN, ctx, ctx->current, "expected %s", get_token_name(type));
            error = 1;
        }

        ctx->previous = ctx->current;
        ctx->current = ctx->current->next;
        parser_skip_blanks(ctx);
    }
    else
    {
        compiler_set_error_with_token(C_UNEXPECTED_TOKEN, ctx, ctx->input_list.tail, "unexpected end of file after");
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
            ss_fprintf(ss, "enum %s", p_declaration_specifiers->enum_specifier->tag_token->lexeme);
        else
            assert(false);
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
            //se nao especificou nada vira integer
            (*flags) |= TYPE_SPECIFIER_INT;
        }
    }

    return 0;
}

int add_specifier(struct parser_ctx* ctx,
    enum type_specifier_flags* flags,
    enum type_specifier_flags new_flag
)
{
    /*
    * Verifica as combinaçòes possíveis
    */

    if (new_flag & TYPE_SPECIFIER_LONG) //adicionando um long
    {
        if ((*flags) & TYPE_SPECIFIER_LONG_LONG) //ja tinha long long
        {
            compiler_set_error_with_token(C_CANNOT_COMBINE_WITH_PREVIOUS_LONG_LONG, ctx, ctx->current, "cannot combine with previous 'long long' declaration specifier");
            return 1;
        }
        else if ((*flags) & TYPE_SPECIFIER_LONG) //ja tinha um long
        {
            (*flags) = (*flags) & ~TYPE_SPECIFIER_LONG;
            (*flags) |= TYPE_SPECIFIER_LONG_LONG;
        }
        else //nao tinha nenhum long
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

        p_declaration_specifiers->first_token = ctx->current;

        while (first_of_declaration_specifier(ctx))
        {
            if (ctx->current->flags & TK_FLAG_IDENTIFIER_IS_TYPEDEF)
            {
                if (p_declaration_specifiers->type_specifier_flags != TYPE_SPECIFIER_NONE)
                {
                    //typedef tem que aparecer sozinho
                    //exemplo Socket eh nome e nao typdef
                    //typedef int Socket;
                    //struct X {int Socket;}; 
                    break;
                }
            }

            struct declaration_specifier* owner p_declaration_specifier = declaration_specifier(ctx);

            if (p_declaration_specifier->type_specifier_qualifier)
            {
                if (p_declaration_specifier->type_specifier_qualifier)
                {
                    if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                    {

                        if (add_specifier(ctx,
                            &p_declaration_specifiers->type_specifier_flags,
                            p_declaration_specifier->type_specifier_qualifier->type_specifier->flags) != 0)
                        {
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

                            //p_declaration_specifiers->typedef_declarator = p_declaration_specifier->type_specifier_qualifier->pType_specifier->token->lexeme;
                        }
                    }
                    else if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                    {
                        p_declaration_specifiers->type_qualifier_flags |= p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;

                    }
                }
            }
            else if (p_declaration_specifier->storage_class_specifier)
            {
                p_declaration_specifiers->storage_class_specifier_flags |= p_declaration_specifier->storage_class_specifier->flags;
            }

            LIST_ADD(p_declaration_specifiers, p_declaration_specifier);
            //attribute_specifier_sequence_opt(ctx);

            if (ctx->current->type == TK_IDENTIFIER &&
                p_declaration_specifiers->type_specifier_flags != TYPE_SPECIFIER_NONE)
            {
                //typedef nao pode aparecer com outro especifier
                //entao ja tem tem algo e vier identifier signfica que acabou 
                //exemplo
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
        //int main() { static int i; } // i is not automatic
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
    enum storage_class_specifier_flags default_storage_class_specifier_flags
)
{
    /*
                                  declaration-specifiers init-declarator-list_opt ;
     attribute-specifier-sequence declaration-specifiers init-declarator-list ;
     static_assert-declaration
     attribute-declaration
  */


    struct declaration* owner p_declaration = calloc(1, sizeof(struct declaration));

    p_declaration->p_attribute_specifier_sequence_opt = move p_attribute_specifier_sequence_opt;

    p_declaration->first_token = ctx->current;

    if (ctx->current->type == ';')
    {
        parser_match_tk(ctx, ';');
        //declaracao vazia nao eh erro
        return p_declaration;
    }

    if (first_of_static_assert_declaration(ctx))
    {
        p_declaration->static_assert_declaration = move static_assert_declaration(ctx);
    }
    else
    {

        if (first_of_declaration_specifier(ctx))
        {
            p_declaration->declaration_specifiers = move declaration_specifiers(ctx, default_storage_class_specifier_flags);

            if (p_declaration->p_attribute_specifier_sequence_opt)
            {
                p_declaration->declaration_specifiers->attributes_flags =
                    p_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
            }

            if (ctx->current->type != ';')
            {
                p_declaration->init_declarator_list = move init_declarator_list(ctx,
                    p_declaration->declaration_specifiers,
                    p_declaration->p_attribute_specifier_sequence_opt);
            }


            p_declaration->last_token = ctx->current;

            if (ctx->current->type == '{')
            {
                if (can_be_function_definition)
                    *is_function_definition = true;
            }
            else
                parser_match_tk(ctx, ';');
        }
        else
        {
            if (ctx->current->type == TK_IDENTIFIER)
            {
                compiler_set_error_with_token(C_INVALID_TYPE, ctx, ctx->current, "invalid type '%s'", ctx->current->lexeme);
            }
            else
            {
                compiler_set_error_with_token(C_EXPECTED_DECLARATION, ctx, ctx->current, "expected declaration not '%s'", ctx->current->lexeme);
            }
            parser_match(ctx); //we need to go ahead
        }
    }


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

    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt =
        move attribute_specifier_sequence_opt(ctx);


    bool is_function_definition = false;
    struct declaration* owner p_declaration = declaration_core(ctx, move p_attribute_specifier_sequence_opt, true, &is_function_definition, STORAGE_SPECIFIER_EXTERN);
    if (is_function_definition)
    {

        ctx->p_current_function_opt = p_declaration;
        //tem que ter 1 so
        //tem 1 que ter  1 cara e ser funcao
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

        //o function_prototype_scope era um block_scope
        p_declaration->function_body = move function_body(ctx);
        p_declaration->init_declarator_list.head->p_declarator->function_body = p_declaration->function_body;


        if (ctx->options.flow_analysis)
        {
            /*
             Now we have the full function AST let´s visit to analise
             jumps
            */

            struct flow_visit_ctx ctx2 = {0};
            ctx2.ctx = ctx;
            flow_visit_function(&ctx2, p_declaration);
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
                    compiler_set_warning_with_token(W_UNUSED_PARAMETER,
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


    return p_declaration;
}

struct declaration* owner declaration(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt,
    enum storage_class_specifier_flags storage_specifier_flags
)
{
    bool is_function_definition;
    return declaration_core(ctx, move p_attribute_specifier_sequence_opt, false, &is_function_definition, storage_specifier_flags);
}


//(6.7) declaration-specifiers:
//declaration-specifier attribute-specifier-sequenceopt
//declaration-specifier declaration-specifiers



struct declaration_specifier* owner declaration_specifier(struct parser_ctx* ctx)
{
    //    storage-class-specifier
    //    type-specifier-qualifier
    //    function-specifier
    struct declaration_specifier* owner p_declaration_specifier = calloc(1, sizeof * p_declaration_specifier);
    if (first_of_storage_class_specifier(ctx))
    {
        p_declaration_specifier->storage_class_specifier = move storage_class_specifier(ctx);
    }
    else if (first_of_type_specifier_qualifier(ctx))
    {
        p_declaration_specifier->type_specifier_qualifier = move type_specifier_qualifier(ctx);
    }
    else if (first_of_function_specifier(ctx))
    {
        p_declaration_specifier->function_specifier = move function_specifier(ctx);
    }
    else
    {
        compiler_set_error_with_token(C_UNEXPECTED, ctx, ctx->current, "unexpected");
    }
    return p_declaration_specifier;
}


struct init_declarator* owner init_declarator(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt)
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
        p_init_declarator->p_declarator = move declarator(ctx,
            NULL,
            p_declaration_specifiers,
            false,
            &tkname);

        if (p_init_declarator->p_declarator == NULL) throw;


        p_init_declarator->p_declarator->name = tkname;


        if (tkname == NULL)
        {
            compiler_set_error_with_token(C_UNEXPECTED, ctx, ctx->current, "empty declarator name?? unexpected");

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
            assert(p_init_declarator->p_declarator->type.type_specifier_flags == 0);
            p_init_declarator->p_declarator->type = move make_type_using_declarator(ctx, p_init_declarator->p_declarator);
        }

        const char* name = p_init_declarator->p_declarator->name->lexeme;
        if (name)
        {
            if (tkname)
            {
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
            }

            struct scope* out = NULL;
            struct declarator* previous = find_declarator(ctx, name, &out);
            if (previous)
            {
                if (out->scope_level == ctx->scopes.tail->scope_level)
                {
                    if (out->scope_level == 0)
                    {
                        /*file scope*/
                        if (!type_is_same(&previous->type, &p_init_declarator->p_declarator->type, true))
                        {
                            //TODO failing on windows headers
                            //parser_seterror_with_token(ctx, p_init_declarator->declarator->name, "redeclaration of  '%s' with diferent types", previous->name->lexeme);
                            //parser_set_info_with_token(ctx, previous->name, "previous declaration");
                        }
                    }
                    else
                    {
                        compiler_set_error_with_token(C_REDECLARATION, ctx, ctx->current, "redeclaration");
                        compiler_set_info_with_token(W_NONE, ctx, previous->name, "previous declaration");
                    }
                }
                else
                {
                    hashmap_set(&ctx->scopes.tail->variables, name, p_init_declarator, TAG_TYPE_INIT_DECLARATOR);

                    /*global scope no warning...*/
                    if (out->scope_level != 0)
                    {
                        /*but redeclaration at function scope we show warning*/
                        if (compiler_set_warning_with_token(W_DECLARATOR_HIDE, ctx, p_init_declarator->p_declarator->first_token, "declaration of '%s' hides previous declaration", name))
                        {
                            compiler_set_info_with_token(W_NONE, ctx, previous->first_token, "previous declaration is here");
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

            struct token* move_token = NULL;
            if (ctx->current->type == TK_KEYWORD__MOVE)
            {
                move_token = ctx->current;
                parser_match(ctx);

            }

            p_init_declarator->initializer = move initializer(ctx);
            p_init_declarator->initializer->move_token = move_token;



            if (p_init_declarator->initializer->braced_initializer)
            {
                if (type_is_array(&p_init_declarator->p_declarator->type))
                {
                    const int sz = type_get_array_size(&p_init_declarator->p_declarator->type);
                    if (sz == 0)
                    {
                        /*int a[] = {1, 2, 3}*/
                        const int braced_initializer_size =
                            p_init_declarator->initializer->braced_initializer->initializer_list->size;

                        type_set_array_size(&p_init_declarator->p_declarator->type, braced_initializer_size);
                    }
                }
            }
            else if (p_init_declarator->initializer->assignment_expression)
            {

            }
            /*
               auto requires we find the type after initializer
            */
            if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
            {
                if (p_init_declarator->initializer &&
                    p_init_declarator->initializer->assignment_expression)
                {
                    struct type t = {0};

                    if (p_init_declarator->initializer->assignment_expression->expression_type == UNARY_EXPRESSION_ADDRESSOF)
                    {
                        t = move type_dup(&p_init_declarator->initializer->assignment_expression->type);
                    }
                    else
                    {
                        struct type t2 = type_lvalue_conversion(&p_init_declarator->initializer->assignment_expression->type);
                        type_swap(&t2, &t);
                        type_destroy(&t2);
                    }

                    type_remove_names(&t);
                    t.name_opt = move strdup(p_init_declarator->p_declarator->name->lexeme);

                    type_set_qualifiers_using_declarator(&t, p_init_declarator->p_declarator);
                    //storage qualifiers?

                    type_visit_to_mark_anonymous(&t);
                    type_swap(&p_init_declarator->p_declarator->type, &t);
                    type_destroy(&t);
                }
            }

            if (p_init_declarator->initializer &&
                p_init_declarator->initializer->assignment_expression &&
                type_is_pointer_to_const(&p_init_declarator->initializer->assignment_expression->type))
            {
                if (p_init_declarator->p_declarator &&
                    !type_is_pointer_to_const(&p_init_declarator->p_declarator->type))
                {
                    compiler_set_warning_with_token(W_DISCARDED_QUALIFIERS, ctx, ctx->current, "const qualifier discarded");
                }
            }

        }

    }
    catch
    {
    }

    if (p_init_declarator->initializer &&
        p_init_declarator->initializer->assignment_expression)
    {
        if (p_init_declarator->initializer->assignment_expression->type.type_qualifier_flags &
            TYPE_QUALIFIER_OWNER)
        {
            if (p_init_declarator->initializer->assignment_expression->expression_type == POSTFIX_FUNCTION_CALL)
            {
                //type * p = f();
                if (!(p_init_declarator->p_declarator->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER))
                {
                    compiler_set_error_with_token(C_MISSING_OWNER, ctx, p_init_declarator->p_declarator->first_token, "missing owner qualifier");
                }
            }
        }
    }

    if (p_init_declarator->p_declarator)
    {
        if (type_is_array(&p_init_declarator->p_declarator->type))
            if (p_init_declarator->p_declarator->type.type_qualifier_flags != 0 ||
                p_init_declarator->p_declarator->type.static_array)
            {
                compiler_set_error_with_token(C_STATIC_OR_TYPE_QUALIFIERS_NOT_ALLOWED_IN_NON_PARAMETER,
                    ctx,
                    p_init_declarator->p_declarator->first_token,
                    "static or type qualifiers are not allowed in non-parameter array declarator");
            }


        if (!type_is_pointer(&p_init_declarator->p_declarator->type) &&
            p_init_declarator->p_declarator->type.type_qualifier_flags & TYPE_QUALIFIER_OBJ_OWNER)
        {
            compiler_set_error_with_token(C_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER,
                ctx,
                p_init_declarator->p_declarator->first_token,
                "_Obj_owner qualifier can only be used with pointers");
        }
    }

    return p_init_declarator;
}

struct init_declarator_list init_declarator_list(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt)
{
    /*
    init-declarator-list:
      init-declarator
      init-declarator-list , init-declarator
    */
    struct init_declarator_list init_declarator_list = {0};
    struct init_declarator* owner p_init_declarator = NULL;

    try
    {
        p_init_declarator = move init_declarator(ctx,
            p_declaration_specifiers,
            p_attribute_specifier_sequence_opt);
        if (p_init_declarator == NULL) throw;
        LIST_ADD(&init_declarator_list, p_init_declarator);
        p_init_declarator = NULL; /*MOVED*/

        while (ctx->current != NULL && ctx->current->type == ',')
        {
            parser_match(ctx);
            p_init_declarator = move init_declarator(ctx, p_declaration_specifiers, p_attribute_specifier_sequence_opt);
            if (p_init_declarator == NULL) throw;
            LIST_ADD(&init_declarator_list, p_init_declarator);
            p_init_declarator = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return init_declarator_list;
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
            new_typeof_specifier_argument->type_name = move type_name(ctx);
        }
        else
        {
            new_typeof_specifier_argument->expression = move expression(ctx);
            if (new_typeof_specifier_argument->expression == NULL) throw;

            //declarator_type_clear_name(new_typeof_specifier_argument->expression->type.declarator_type);
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
        p_typeof_specifier = move calloc(1, sizeof(struct typeof_specifier));
        if (p_typeof_specifier == NULL) throw;

        p_typeof_specifier->first_token = ctx->current;

        const bool is_typeof_unqual = ctx->current->type == TK_KEYWORD_TYPEOF_UNQUAL;
        parser_match(ctx);
        if (parser_match_tk(ctx, '(') != 0) throw;

        p_typeof_specifier->typeof_specifier_argument = move typeof_specifier_argument(ctx);
        if (p_typeof_specifier->typeof_specifier_argument == NULL) throw;

        if (p_typeof_specifier->typeof_specifier_argument->expression)
        {
            p_typeof_specifier->type = move type_dup(&p_typeof_specifier->typeof_specifier_argument->expression->type);
        }
        else if (p_typeof_specifier->typeof_specifier_argument->type_name)
        {
            p_typeof_specifier->type = move type_dup(&p_typeof_specifier->typeof_specifier_argument->type_name->declarator->type);
        }

        if (p_typeof_specifier->type.storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            compiler_set_warning_with_token(W_TYPEOF_ARRAY_PARAMETER, ctx, ctx->current, "typeof used in array arguments");

            if (type_is_array(&p_typeof_specifier->type))
            {
                struct type t = type_param_array_to_pointer(&p_typeof_specifier->type);
                type_swap(&t, &p_typeof_specifier->type);
                type_destroy(&t);
            }

        }

        if (is_typeof_unqual)
        {
            type_remove_qualifiers(&p_typeof_specifier->type);
        }

        type_visit_to_mark_anonymous(&p_typeof_specifier->type);

        p_typeof_specifier->last_token = ctx->current;
        parser_match_tk(ctx, ')');
    }
    catch
    {
    }

    return p_typeof_specifier;
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




    //typeof (expression)
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

            //microsoft
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
            //end microsoft

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


    }

    if (first_of_typeof_specifier(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_TYPEOF;
        p_type_specifier->typeof_specifier = move typeof_specifier(ctx);
    }
    else if (first_of_atomic_type_specifier(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_ATOMIC;
        p_type_specifier->atomic_type_specifier = move atomic_type_specifier(ctx);
    }
    else if (first_of_struct_or_union(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_STRUCT_OR_UNION;
        p_type_specifier->struct_or_union_specifier = move struct_or_union_specifier(ctx);
    }
    else if (first_of_enum_specifier(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_ENUM;
        p_type_specifier->enum_specifier = move enum_specifier(ctx);
    }
    else if (ctx->current->type == TK_IDENTIFIER)
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_TYPEDEF;

        p_type_specifier->typedef_declarator =
            find_declarator(ctx, ctx->current->lexeme, NULL);

        //Ser chegou aqui já tem que exitir (reaprovecitar?)
        assert(p_type_specifier->typedef_declarator != NULL);

        parser_match(ctx);
    }
    return p_type_specifier;
}



struct struct_or_union_specifier* get_complete_struct_or_union_specifier(struct struct_or_union_specifier* p_struct_or_union_specifier)
{

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
    return
        get_complete_struct_or_union_specifier(p_struct_or_union_specifier) != NULL;
}

struct struct_or_union_specifier* owner struct_or_union_specifier(struct parser_ctx* ctx)
{
    struct struct_or_union_specifier* owner p_struct_or_union_specifier = calloc(1, sizeof * p_struct_or_union_specifier);

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
        move attribute_specifier_sequence_opt(ctx);

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
                compiler_set_error_with_token(C_TAG_TYPE_DOES_NOT_MATCH_PREVIOUS_DECLARATION,
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
        p_struct_or_union_specifier->member_declaration_list = move member_declaration_list(ctx, p_struct_or_union_specifier);
        p_struct_or_union_specifier->member_declaration_list.first_token = firsttoken;
        p_struct_or_union_specifier->last_token = ctx->current;
        p_struct_or_union_specifier->member_declaration_list.last_token = ctx->current;
        parser_match_tk(ctx, '}');

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
                //TODO add deprecated message
                compiler_set_warning_with_token(W_DEPRECATED, ctx, p_struct_or_union_specifier->first_token, "'%s' is deprecated", p_struct_or_union_specifier->tagtoken->lexeme);
            }
            else
            {
                compiler_set_warning_with_token(W_DEPRECATED, ctx, p_struct_or_union_specifier->first_token, "deprecated");
            }
        }
    }

    return p_struct_or_union_specifier;
}

struct member_declarator* owner member_declarator(
    struct parser_ctx* ctx,
    struct struct_or_union_specifier* p_struct_or_union_specifier,
    struct specifier_qualifier_list* p_specifier_qualifier_list
)
{
    /*
    member-declarator:
     declarator
     declaratoropt : constant-expression
    */
    struct member_declarator* owner p_member_declarator = calloc(1, sizeof(struct member_declarator));

    struct token* p_token_name = NULL;

    p_member_declarator->declarator = move declarator(ctx, p_specifier_qualifier_list, /*declaration_specifiers*/NULL, false, &p_token_name);
    p_member_declarator->declarator->name = p_token_name;
    p_member_declarator->declarator->specifier_qualifier_list = p_specifier_qualifier_list;

    p_member_declarator->declarator->type = move make_type_using_declarator(ctx, p_member_declarator->declarator);

    /*extension*/
    if (p_member_declarator->declarator->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER)
    {
        /*having at least 1 owner member, the struct type is owner by default*/
        p_struct_or_union_specifier->is_owner = true;
    }

    if (p_member_declarator->declarator->name)
        naming_convention_struct_member(ctx, p_member_declarator->declarator->name, &p_member_declarator->declarator->type);

    if (ctx->current->type == ':')
    {
        parser_match(ctx);
        p_member_declarator->constant_expression = move constant_expression(ctx);
    }
    return p_member_declarator;
}

struct member_declarator_list* owner member_declarator_list(
    struct parser_ctx* ctx,
    struct struct_or_union_specifier* p_struct_or_union_specifier,
    struct specifier_qualifier_list* p_specifier_qualifier_list)
{
    struct member_declarator_list* owner p_member_declarator_list = calloc(1, sizeof(struct member_declarator_list));
    LIST_ADD(p_member_declarator_list, member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list));
    while (ctx->current->type == ',')
    {
        parser_match(ctx);
        LIST_ADD(p_member_declarator_list, member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list));
    }
    return p_member_declarator_list;
}


struct member_declaration_list member_declaration_list(struct parser_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    struct member_declaration_list list = {0};
    //member_declaration
    //member_declaration_list member_declaration

    struct member_declaration* owner p_member_declaration = NULL;

    try
    {
        p_member_declaration = move member_declaration(ctx,
            p_struct_or_union_specifier);

        if (p_member_declaration == NULL) throw;
        LIST_ADD(&list, p_member_declaration);
        p_member_declaration = NULL; /*MOVED*/

        while (ctx->current && ctx->current->type != '}')
        {
            p_member_declaration = move member_declaration(ctx, p_struct_or_union_specifier);
            if (p_member_declaration == NULL) throw;
            LIST_ADD(&list, p_member_declaration);
            p_member_declaration = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return list;
}

struct member_declaration* owner member_declaration(struct parser_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    struct member_declaration* owner p_member_declaration = calloc(1, sizeof(struct member_declaration));
    //attribute_specifier_sequence_opt specifier_qualifier_list member_declarator_list_opt ';'
    //static_assert_declaration
    if (ctx->current->type == TK_KEYWORD__STATIC_ASSERT)
    {
        p_member_declaration->static_assert_declaration = move static_assert_declaration(ctx);
    }
    else
    {
        p_member_declaration->p_attribute_specifier_sequence_opt = move attribute_specifier_sequence_opt(ctx);

        p_member_declaration->specifier_qualifier_list = move specifier_qualifier_list(ctx);
        if (ctx->current->type != ';')
        {
            p_member_declaration->member_declarator_list_opt = move member_declarator_list(ctx,
                p_struct_or_union_specifier,
                p_member_declaration->specifier_qualifier_list);
        }
        parser_match_tk(ctx, ';');
    }
    return p_member_declaration;
}

struct member_declarator* find_member_declarator(struct member_declaration_list* list, const char* name, int* p_member_index)
{
    int member_index = 0;

    struct member_declaration* p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator* p_member_declarator = NULL;

        if (p_member_declaration->member_declarator_list_opt)
        {
            p_member_declarator = p_member_declaration->member_declarator_list_opt->head;

            while (p_member_declarator)
            {
                if (strcmp(p_member_declarator->declarator->name->lexeme, name) == 0)
                {
                    *p_member_index = member_index;
                    return p_member_declarator;
                }

                member_index++;
                p_member_declarator = p_member_declarator->next;
            }
        }
        else if (p_member_declaration->specifier_qualifier_list)
        {
            /*
             struct X {
                union  {
                  unsigned char       Byte[16];
                  unsigned short      Word[8];
                  };
            };

            struct X* a;
            a.Byte[0] & 0xe0;
            */

            if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct member_declaration_list* p_member_declaration_list =
                    &p_member_declaration->specifier_qualifier_list->struct_or_union_specifier->member_declaration_list;
                int inner_member_index = 0;
                struct member_declarator* p = find_member_declarator(p_member_declaration_list, name, &inner_member_index);
                if (p)
                {
                    *p_member_index = member_index + inner_member_index;
                    return p;
                }
            }
        }
        else
        {
            assert(p_member_declaration->static_assert_declaration != NULL);
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

        //TODO
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



struct specifier_qualifier_list* owner specifier_qualifier_list(struct parser_ctx* ctx)
{
    struct specifier_qualifier_list* owner p_specifier_qualifier_list = calloc(1, sizeof(struct specifier_qualifier_list));
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
                    //typedef tem que aparecer sozinho
                    //exemplo Socket eh nome e nao typdef
                    //typedef int Socket;
                    //struct X {int Socket;}; 
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

            LIST_ADD(p_specifier_qualifier_list, p_type_specifier_qualifier);
            p_specifier_qualifier_list->p_attribute_specifier_sequence = move attribute_specifier_sequence_opt(ctx);
        }
    }
    catch
    {
    }

    final_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags);
    p_specifier_qualifier_list->last_token = previous_parser_token(ctx->current);
    return p_specifier_qualifier_list;
}

struct type_specifier_qualifier* owner type_specifier_qualifier(struct parser_ctx* ctx)
{
    struct type_specifier_qualifier* owner type_specifier_qualifier = calloc(1, sizeof * type_specifier_qualifier);
    //type_specifier
    //type_qualifier
    //alignment_specifier
    if (first_of_type_specifier(ctx))
    {
        type_specifier_qualifier->type_specifier = move type_specifier(ctx);
    }
    else if (first_of_type_qualifier(ctx))
    {
        type_specifier_qualifier->type_qualifier = move type_qualifier(ctx);
    }
    else if (first_of_alignment_specifier(ctx))
    {
        type_specifier_qualifier->alignment_specifier = move alignment_specifier(ctx);
    }
    else
    {
        assert(false);
    }
    return type_specifier_qualifier;
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
    struct enum_specifier* owner p_enum_specifier = NULL;
    try
    {
        p_enum_specifier = move calloc(1, sizeof * p_enum_specifier);

        p_enum_specifier->first_token = ctx->current;
        parser_match_tk(ctx, TK_KEYWORD_ENUM);

        p_enum_specifier->attribute_specifier_sequence_opt =
            move attribute_specifier_sequence_opt(ctx);


        struct enum_specifier* p_previous_tag_in_this_scope = NULL;
        bool has_identifier = false;
        if (ctx->current->type == TK_IDENTIFIER)
        {
            has_identifier = true;
            p_enum_specifier->tag_token = ctx->current;
            parser_match(ctx);
        }

        if (ctx->current->type == ':')
        {
            /*C23*/
            parser_match(ctx);
            p_enum_specifier->specifier_qualifier_list = move specifier_qualifier_list(ctx);
        }

        if (ctx->current->type == '{')
        {
            if (p_enum_specifier->tag_token)
                naming_convention_enum_tag(ctx, p_enum_specifier->tag_token);

            /*points to itself*/
            p_enum_specifier->complete_enum_specifier = p_enum_specifier;

            parser_match_tk(ctx, '{');
            p_enum_specifier->enumerator_list = move enumerator_list(ctx, p_enum_specifier);
            if (ctx->current->type == ',')
            {
                parser_match(ctx);
            }
            parser_match_tk(ctx, '}');
        }
        else
        {
            if (!has_identifier)
            {
                compiler_set_error_with_token(C_MISSING_ENUM_TAG_NAME, ctx, ctx->current, "missing enum tag name");
                throw;
            }
        }

        /*
        * Let's search for this tag at current scope only
        */
        struct map_entry* p_entry = NULL;

        if (p_enum_specifier->tag_token &&
            p_enum_specifier->tag_token->lexeme)
        {
            p_entry = hashmap_find(&ctx->scopes.tail->tags, p_enum_specifier->tag_token->lexeme);
        }
        if (p_entry)
        {
            /*
               ok.. we have this tag at this scope
            */
            if (p_entry->type == TAG_TYPE_ENUN_SPECIFIER)
            {
                p_previous_tag_in_this_scope = p_entry->p;

                if (p_previous_tag_in_this_scope->enumerator_list.head != NULL &&
                    p_enum_specifier->enumerator_list.head != NULL)
                {
                    compiler_set_error_with_token(C_MULTIPLE_DEFINITION_ENUM,
                        ctx,
                        p_enum_specifier->tag_token,
                        "multiple definition of 'enum %s'",
                        p_enum_specifier->tag_token->lexeme);
                }
                else if (p_previous_tag_in_this_scope->enumerator_list.head != NULL)
                {
                    p_enum_specifier->complete_enum_specifier = p_previous_tag_in_this_scope;
                }
                else if (p_enum_specifier->enumerator_list.head != NULL)
                {
                    p_previous_tag_in_this_scope->complete_enum_specifier = p_enum_specifier;
                }
            }
            else
            {
                compiler_set_error_with_token(C_TAG_TYPE_DOES_NOT_MATCH_PREVIOUS_DECLARATION,
                    ctx,
                    ctx->current, "use of '%s' with tag type that does not match previous declaration.",
                    ctx->current->lexeme);
                throw;
            }
        }
        else
        {
            /*
            * we didn't find at current scope let's search in previous scopes
            */
            struct enum_specifier* p_other = NULL;

            if (p_enum_specifier->tag_token)
            {
                p_other = find_enum_specifier(ctx, p_enum_specifier->tag_token->lexeme);
            }

            if (p_other == NULL)
            {
                /*
                 * we didn't find, so this is the first time this tag is used
                */
                if (p_enum_specifier->tag_token)
                {
                    hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_token->lexeme, p_enum_specifier, TAG_TYPE_ENUN_SPECIFIER);
                }
                else
                {
                    //make a name?
                }
            }
            else
            {


                /*
                 * we found this enum tag in previous scopes
                */

                if (p_enum_specifier->enumerator_list.head != NULL)
                {
                    /*it is a new definition - itself*/
                    //p_enum_specifier->complete_enum_specifier = p_enum_specifier;
                }
                else if (p_other->enumerator_list.head != NULL)
                {
                    /*previous enum is complete*/
                    p_enum_specifier->complete_enum_specifier = p_other;
                }
            }
        }

    }
    catch
    {}
    return p_enum_specifier;
}

struct enumerator_list enumerator_list(struct parser_ctx* ctx, struct enum_specifier* p_enum_specifier)
{

    /*
       enumerator
        enumerator_list ',' enumerator
     */

    struct enumerator_list enumeratorlist = {0};
    struct enumerator* owner p_enumerator = NULL;
    try
    {


        p_enumerator = move enumerator(ctx, p_enum_specifier);
        if (p_enumerator == NULL) throw;
        LIST_ADD(&enumeratorlist, p_enumerator);


        while (ctx->current != NULL && ctx->current->type == ',')
        {
            parser_match(ctx);  /*pode ter uma , vazia no fim*/

            if (ctx->current && ctx->current->type != '}')
            {
                p_enumerator = move enumerator(ctx, p_enum_specifier);
                if (p_enumerator == NULL) throw;
                LIST_ADD(&enumeratorlist, p_enumerator);
            }
        }
    }
    catch
    {
    }

    return enumeratorlist;
}

struct enumerator* owner enumerator(struct parser_ctx* ctx,
    struct enum_specifier* p_enum_specifier)
{
    //TODO VALUE
    struct enumerator* owner p_enumerator = calloc(1, sizeof(struct enumerator));
    p_enumerator->enum_specifier = p_enum_specifier;
    struct token* name = ctx->current;

    naming_convention_enumerator(ctx, name);

    parser_match_tk(ctx, TK_IDENTIFIER);

    p_enumerator->attribute_specifier_sequence_opt = move attribute_specifier_sequence_opt(ctx);

    p_enumerator->token = name;
    hashmap_set(&ctx->scopes.tail->variables, p_enumerator->token->lexeme, p_enumerator, TAG_TYPE_ENUMERATOR);

    if (ctx->current->type == '=')
    {
        parser_match(ctx);
        p_enumerator->constant_expression_opt = move constant_expression(ctx);
        p_enumerator->value = constant_value_to_ll(&p_enumerator->constant_expression_opt->constant_value);
    }

    return p_enumerator;
}


struct alignment_specifier* owner alignment_specifier(struct parser_ctx* ctx)
{
    struct alignment_specifier* owner alignment_specifier = calloc(1, sizeof * alignment_specifier);
    alignment_specifier->token = ctx->current;
    parser_match_tk(ctx, TK_KEYWORD__ALIGNAS);
    parser_match_tk(ctx, '(');
    if (first_of_type_name(ctx))
    {
        alignment_specifier->type_name = move type_name(ctx);
    }
    else
    {
        alignment_specifier->constant_expression = move constant_expression(ctx);
    }
    parser_match_tk(ctx, ')');
    return alignment_specifier;
}



struct atomic_type_specifier* owner atomic_type_specifier(struct parser_ctx* ctx)
{
    //'_Atomic' '(' type_name ')'
    struct atomic_type_specifier* owner p = calloc(1, sizeof * p);
    p->token = ctx->current;
    parser_match_tk(ctx, TK_KEYWORD__ATOMIC);
    parser_match_tk(ctx, '(');
    p->type_name = move type_name(ctx);
    parser_match_tk(ctx, ')');
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

            /*
              ownership extensions
            */

        case TK_KEYWORD__OWNER:
            p_type_qualifier->flags = TYPE_QUALIFIER_OWNER;
            break;
        case TK_KEYWORD__OBJ_OWNER:
            p_type_qualifier->flags = TYPE_QUALIFIER_OBJ_OWNER;
            break;
        case TK_KEYWORD__VIEW:
            p_type_qualifier->flags = TYPE_QUALIFIER_VIEW;
            break;

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


struct function_specifier* owner function_specifier(struct parser_ctx* ctx)
{
    if (ctx->current->type == TK_KEYWORD__NORETURN)
    {
        compiler_set_info_with_token(W_STYLE, ctx, ctx->current, "_Noreturn is deprecated use attributes");
    }

    struct function_specifier* owner p_function_specifier = NULL;
    try
    {
        p_function_specifier = move calloc(1, sizeof * p_function_specifier);
        if (p_function_specifier == NULL) throw;

        p_function_specifier->token = ctx->current;
        parser_match(ctx);

    }
    catch
    {
    }

    return p_function_specifier;
}


struct declarator* owner declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* p_specifier_qualifier_list,
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
    p_declarator->pointer = move pointer_opt(ctx);
    p_declarator->direct_declarator = move direct_declarator(ctx, p_specifier_qualifier_list, p_declaration_specifiers, abstract_acceptable, pp_token_name);

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

struct direct_declarator* owner direct_declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* p_specifier_qualifier_list,
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
        if (ctx->current == NULL) throw;

        struct token* p_token_ahead = parser_look_ahead(ctx);
        if (ctx->current->type == TK_IDENTIFIER)
        {
            p_direct_declarator->name_opt = ctx->current;
            if (pptoken_name != NULL)
            {
                *pptoken_name = ctx->current;
            }


            parser_match(ctx);
            p_direct_declarator->p_attribute_specifier_sequence_opt = move attribute_specifier_sequence_opt(ctx);
        }
        else if (ctx->current->type == '(')
        {
            struct token* ahead = parser_look_ahead(ctx);

            if (!first_of_type_specifier_token(ctx, p_token_ahead) &&
                !first_of_type_qualifier_token(p_token_ahead) &&
                ahead->type != ')' &&
                ahead->type != '...')
            {
                //look ahead para nao confundir (declarator) com parametros funcao ex void (int)
                //or function int ()

                parser_match(ctx);

                p_direct_declarator->declarator = move declarator(ctx,
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

            if (ctx->current->type == '[')
            {
                p_direct_declarator2->array_declarator = move array_declarator(move p_direct_declarator, ctx);
            }
            else
            {
                p_direct_declarator2->function_declarator = move function_declarator(move p_direct_declarator, ctx);
            }
            p_direct_declarator = move p_direct_declarator2;
        }
    }
    catch
    {
    }

    return p_direct_declarator;
}


unsigned long long array_declarator_get_size(struct array_declarator* p_array_declarator)
{
    if (p_array_declarator->assignment_expression)
    {
        if (constant_value_is_valid(&p_array_declarator->assignment_expression->constant_value))
        {
            return
                constant_value_to_ull(&p_array_declarator->assignment_expression->constant_value);
        }
    }
    return 0;
}

struct array_declarator* owner array_declarator(struct direct_declarator* owner p_direct_declarator, struct parser_ctx* ctx)
{
    //direct_declarator '['          type_qualifier_list_opt           assignment_expression_opt ']'
    //direct_declarator '[' 'static' type_qualifier_list_opt           assignment_expression     ']'
    //direct_declarator '['          type_qualifier_list      'static' assignment_expression     ']'
    //direct_declarator '['          type_qualifier_list_opt  '*'           ']'

    struct array_declarator* owner p_array_declarator = NULL;
    try
    {
        p_array_declarator = move calloc(1, sizeof * p_array_declarator);
        if (p_array_declarator == NULL) throw;

        p_array_declarator->direct_declarator = move p_direct_declarator;
        parser_match_tk(ctx, '[');

        bool has_static = false;
        if (ctx->current->type == TK_KEYWORD_STATIC)
        {
            p_array_declarator->static_token_opt = ctx->current;
            parser_match(ctx);
            has_static = true;
        }

        if (first_of_type_qualifier(ctx))
        {
            p_array_declarator->type_qualifier_list_opt = move type_qualifier_list(ctx);
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
            //tem que ter..

            p_array_declarator->assignment_expression = move assignment_expression(ctx);
            if (p_array_declarator->assignment_expression == NULL) throw;
        }
        else
        {
            //opcional
            if (ctx->current->type == '*')
            {
                parser_match(ctx);
            }
            else if (ctx->current->type != ']')
            {
                p_array_declarator->assignment_expression = move assignment_expression(ctx);
                if (p_array_declarator->assignment_expression == NULL) throw;
            }
            else
            {
            }
        }

        parser_match_tk(ctx, ']');
    }
    catch
    {
        if (p_array_declarator)
        {
        }
    }



    return p_array_declarator;
}


struct function_declarator* owner function_declarator(struct direct_declarator* owner p_direct_declarator, struct parser_ctx* ctx)
{
    struct function_declarator* owner p_function_declarator = calloc(1, sizeof(struct function_declarator));
    //faz um push da funcion_scope_declarator_list que esta vivendo mais em cima
    //eh feito o pop mais em cima tb.. aqui dentro do decide usar.
    //ctx->funcion_scope_declarator_list->outer_scope = ctx->current_scope;
    //ctx->current_scope = ctx->funcion_scope_declarator_list;
    //direct_declarator '(' parameter_type_list_opt ')'


    p_function_declarator->direct_declarator = move p_direct_declarator;
    p_function_declarator->parameters_scope.scope_level = ctx->scopes.tail->scope_level + 1;
    p_function_declarator->parameters_scope.variables.capacity = 5;
    p_function_declarator->parameters_scope.tags.capacity = 1;


    scope_list_push(&ctx->scopes, &p_function_declarator->parameters_scope);

    //print_scope(&ctx->scopes);

    parser_match_tk(ctx, '(');
    if (ctx->current->type != ')')
    {
        p_function_declarator->parameter_type_list_opt = move parameter_type_list(ctx);
    }
    parser_match_tk(ctx, ')');

    //print_scope(&ctx->scopes);

    scope_list_pop(&ctx->scopes);

    //print_scope(&ctx->scopes);


    return p_function_declarator;
}


struct pointer* owner pointer_opt(struct parser_ctx* ctx)
{
    struct pointer* owner p = NULL;
    struct pointer* owner p_pointer = NULL;
    try
    {
        while (ctx->current != NULL && ctx->current->type == '*')
        {
            p_pointer = move calloc(1, sizeof(struct pointer));
            if (p_pointer == NULL) throw;
            p = move p_pointer;
            parser_match(ctx);

            p_pointer->attribute_specifier_sequence_opt =
                move attribute_specifier_sequence_opt(ctx);

            if (first_of_type_qualifier(ctx))
            {
                p_pointer->type_qualifier_list_opt = move type_qualifier_list(ctx);
            }

            while (ctx->current != NULL && ctx->current->type == '*')
            {
                p_pointer->pointer = move pointer_opt(ctx);
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


struct type_qualifier_list* owner type_qualifier_list(struct parser_ctx* ctx)
{
    //type_qualifier
    //type_qualifier_list type_qualifier

    struct type_qualifier_list* owner p_type_qualifier_list = NULL;
    struct type_qualifier* owner p_type_qualifier = NULL;

    try
    {
        p_type_qualifier_list = move calloc(1, sizeof(struct type_qualifier_list));
        if (p_type_qualifier_list == NULL) throw;


        p_type_qualifier = move type_qualifier(ctx);
        if (p_type_qualifier == NULL) throw;

        p_type_qualifier_list->flags |= p_type_qualifier->flags;
        LIST_ADD(p_type_qualifier_list, p_type_qualifier);
        p_type_qualifier = NULL; /*MOVED*/

        while (ctx->current != NULL && first_of_type_qualifier(ctx))
        {
            p_type_qualifier = move type_qualifier(ctx);
            if (p_type_qualifier == NULL) throw;

            p_type_qualifier_list->flags |= p_type_qualifier->flags;
            LIST_ADD(p_type_qualifier_list, p_type_qualifier);
            p_type_qualifier = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return p_type_qualifier_list;
}


struct parameter_type_list* owner parameter_type_list(struct parser_ctx* ctx)
{
    struct parameter_type_list* owner p_parameter_type_list = calloc(1, sizeof(struct parameter_type_list));
    //parameter_list
    //parameter_list ',' '...'
    p_parameter_type_list->parameter_list = move parameter_list(ctx);

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
        //parser_match_tk(ctx, '...');
        p_parameter_type_list->is_var_args = true;
    }
    return p_parameter_type_list;
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
        p_parameter_list = move calloc(1, sizeof(struct parameter_list));
        if (p_parameter_list == NULL) throw;

        p_parameter_declaration = move parameter_declaration(ctx);
        if (p_parameter_declaration == NULL) throw;

        LIST_ADD(p_parameter_list, p_parameter_declaration);
        p_parameter_declaration = NULL; /*MOVED*/

        while (ctx->current != NULL && ctx->current->type == ',')
        {
            parser_match(ctx);
            if (ctx->current->type == '...')
            {
                //follow
                break;
            }

            p_parameter_declaration = move parameter_declaration(ctx);
            if (p_parameter_declaration == NULL) throw;

            LIST_ADD(p_parameter_list, p_parameter_declaration);
            p_parameter_declaration = NULL; /*MOVED*/
        }
    }
    catch
    {
    }
    return p_parameter_list;
}


struct parameter_declaration* owner parameter_declaration(struct parser_ctx* ctx)
{
    struct parameter_declaration* owner p_parameter_declaration = calloc(1, sizeof(struct parameter_declaration));

    if (ctx->current->type == TK_KEYWORD__IMPLICIT)
    {
        p_parameter_declaration->implicit_token = ctx->current;
        parser_match(ctx);
    }

    p_parameter_declaration->attribute_specifier_sequence_opt =
        move attribute_specifier_sequence_opt(ctx);

    p_parameter_declaration->declaration_specifiers = move declaration_specifiers(ctx, STORAGE_SPECIFIER_PARAMETER);


    //talvez no ctx colocar um flag que esta em argumentos
    //TODO se tiver uma struct tag novo...
    //warning: declaration of 'struct X' will not be visible outside of this function [-Wvisibility]
    struct token* p_token_name = 0;
    p_parameter_declaration->declarator = move declarator(ctx,
        /*specifier_qualifier_list*/NULL,
        p_parameter_declaration->declaration_specifiers,
        true/*can be abstract*/,
        &p_token_name);
    p_parameter_declaration->declarator->name = p_token_name;



    p_parameter_declaration->declarator->declaration_specifiers = p_parameter_declaration->declaration_specifiers;

    p_parameter_declaration->declarator->type = move make_type_using_declarator(ctx, p_parameter_declaration->declarator);

    if (p_parameter_declaration->attribute_specifier_sequence_opt)
    {
        /*
          we keep only function type so lets move implict flag to type
          void free( [[cake::implicit]] void *owner p) {}
        */
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

    //coloca o pametro no escpo atual que deve apontar para escopo paramtros
    // da funcao .
    // 
    //assert ctx->current_scope->variables parametrosd
    if (p_parameter_declaration->declarator->name)
    {
        //parametro void nao te name 
        hashmap_set(&ctx->scopes.tail->variables,
            p_parameter_declaration->declarator->name->lexeme,
            p_parameter_declaration->declarator,
            TAG_TYPE_ONLY_DECLARATOR);
        //print_scope(ctx->current_scope);
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
                p_specifier_qualifier->type_qualifier = move p_type_qualifier;
            }
            else if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
            {
                struct type_specifier* owner p_type_specifier = calloc(1, sizeof(struct type_specifier));

                p_type_specifier->flags = p_declaration_specifier->type_specifier_qualifier->type_specifier->flags;

                //todo
                assert(p_declaration_specifier->type_specifier_qualifier->type_specifier->struct_or_union_specifier == NULL);

                p_type_specifier->token = p_declaration_specifier->type_specifier_qualifier->type_specifier->token;
                p_specifier_qualifier->type_specifier = move p_type_specifier;
            }

            LIST_ADD(p_specifier_qualifier_list, p_specifier_qualifier);
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
        //Se is_abstract for true é pedido para nao imprimir o nome do indentificador
        ss_fprintf(ss, "%s", p_direct_declarator->name_opt->lexeme);
    }

    if (p_direct_declarator->function_declarator)
    {
        print_direct_declarator(ss, p_direct_declarator->function_declarator->direct_declarator, is_abstract);

        ss_fprintf(ss, "(");
        struct parameter_declaration* p_parameter_declaration =
            p_direct_declarator->function_declarator->parameter_type_list_opt ?
            p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head : NULL;

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
        //TODO
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

struct type_name* owner type_name(struct parser_ctx* ctx)
{
    struct type_name* owner p_type_name = calloc(1, sizeof(struct type_name));

    p_type_name->first_token = ctx->current;


    p_type_name->specifier_qualifier_list = move specifier_qualifier_list(ctx);


    p_type_name->declarator = move declarator(ctx,
        p_type_name->specifier_qualifier_list,//??
        /*declaration_specifiers*/ NULL,
        true /*DEVE SER TODO*/,
        NULL);
    p_type_name->declarator->specifier_qualifier_list = p_type_name->specifier_qualifier_list;
    p_type_name->declarator->type = move make_type_using_declarator(ctx, p_type_name->declarator);


    p_type_name->last_token = ctx->current->prev;
    p_type_name->type = move type_dup(&p_type_name->declarator->type);


    return p_type_name;
}

struct braced_initializer* owner braced_initializer(struct parser_ctx* ctx)
{
    /*
     { }
     { initializer-list }
     { initializer-list , }
    */

    struct braced_initializer* owner p_bracket_initializer_list = calloc(1, sizeof(struct braced_initializer));
    p_bracket_initializer_list->first_token = ctx->current;
    parser_match_tk(ctx, '{');
    if (ctx->current->type != '}')
    {
        p_bracket_initializer_list->initializer_list = move initializer_list(ctx);
    }
    parser_match_tk(ctx, '}');
    return p_bracket_initializer_list;
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
        p_initializer->braced_initializer = move braced_initializer(ctx);
    }
    else
    {
        p_initializer->p_attribute_specifier_sequence_opt =
            move attribute_specifier_sequence_opt(ctx);

        p_initializer->assignment_expression = move assignment_expression(ctx);
    }
    return p_initializer;
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
        p_designation = move designation(ctx);
    }
    struct initializer* owner p_initializer = initializer(ctx);

    p_initializer->designation = move p_designation;
    LIST_ADD(p_initializer_list, p_initializer);
    p_initializer_list->size++;

    while (ctx->current != NULL && ctx->current->type == ',')
    {
        parser_match(ctx);
        if (ctx->current->type == '}')
            break; //follow

        if (first_of_designator(ctx))
        {
            p_initializer->designation = move designation(ctx);
        }
        struct initializer* owner p_initializer2 = initializer(ctx);

        p_initializer2->designation = move p_designation;
        LIST_ADD(p_initializer_list, p_initializer2);
        p_initializer_list->size++;
    }

    return p_initializer_list;
}


struct designation* owner designation(struct parser_ctx* ctx)
{
    //designator_list '='
    struct designation* owner p_designation = calloc(1, sizeof(struct designation));
    p_designation->designator_list = move designator_list(ctx);
    parser_match_tk(ctx, '=');
    return p_designation;
}

struct designator_list* owner designator_list(struct parser_ctx* ctx)
{
    //designator
    //designator_list designator
    struct designator_list* owner p_designator_list = NULL;
    struct designator* owner p_designator = NULL;
    try
    {
        p_designator_list = move calloc(1, sizeof(struct designator_list));
        if (p_designator_list == NULL) throw;

        p_designator = move designator(ctx);
        if (p_designator == NULL) throw;
        LIST_ADD(p_designator_list, p_designator);
        p_designator = NULL; /*MOVED*/

        while (ctx->current != NULL && first_of_designator(ctx))
        {
            p_designator = move designator(ctx);
            if (p_designator == NULL) throw;
            LIST_ADD(p_designator_list, p_designator);
            p_designator = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return p_designator_list;
}


struct designator* owner designator(struct parser_ctx* ctx)
{
    //'[' constant_expression ']'
    //'.' identifier
    struct designator* owner p_designator = calloc(1, sizeof(struct designator));
    if (ctx->current->type == '[')
    {
        parser_match_tk(ctx, '[');
        p_designator->constant_expression_opt = move constant_expression(ctx);
        parser_match_tk(ctx, ']');
    }
    else if (ctx->current->type == '.')
    {
        parser_match(ctx);
        parser_match_tk(ctx, TK_IDENTIFIER);
    }
    return p_designator;
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
        p_static_assert_declaration = move calloc(1, sizeof(struct static_assert_declaration));
        if (p_static_assert_declaration == NULL) throw;

        p_static_assert_declaration->first_token = ctx->current;
        struct token* position = ctx->current;
        parser_match_tk(ctx, TK_KEYWORD__STATIC_ASSERT);
        parser_match_tk(ctx, '(');

        p_static_assert_declaration->constant_expression = move constant_expression(ctx);
        if (p_static_assert_declaration->constant_expression == NULL) throw;

        if (ctx->current->type == ',')
        {
            parser_match(ctx);
            p_static_assert_declaration->string_literal_opt = ctx->current;
            parser_match_tk(ctx, TK_STRING_LITERAL);
        }

        parser_match_tk(ctx, ')');
        p_static_assert_declaration->last_token = ctx->current;
        parser_match_tk(ctx, ';');

        if (!constant_value_to_bool(&p_static_assert_declaration->constant_expression->constant_value))
        {
            if (p_static_assert_declaration->string_literal_opt)
            {
                compiler_set_error_with_token(C_STATIC_ASSERT_FAILED, ctx, position, "_Static_assert failed %s\n",
                    p_static_assert_declaration->string_literal_opt->lexeme);
            }
            else
            {
                compiler_set_error_with_token(C_STATIC_ASSERT_FAILED, ctx, position, "_Static_assert failed");
            }

            if (p_static_assert_declaration->constant_expression->expression_type == EQUALITY_EXPRESSION_EQUAL)
            {
                if (p_static_assert_declaration->constant_expression->left->expression_type == UNARY_EXPRESSION_STATIC_DEBUG)
                {
                    compiler_set_info_with_token(W_NONE,
                        ctx,
                        position, "%llu != %llu",
                        constant_value_to_ull(&p_static_assert_declaration->constant_expression->left->constant_value),
                        constant_value_to_ull(&p_static_assert_declaration->constant_expression->right->constant_value)
                    );
                }
            }
        }
    }
    catch
    {}
    return p_static_assert_declaration;
}


struct attribute_specifier_sequence* owner attribute_specifier_sequence_opt(struct parser_ctx* ctx)
{
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence = NULL;

    if (first_of_attribute_specifier(ctx))
    {
        p_attribute_specifier_sequence = move calloc(1, sizeof(struct attribute_specifier_sequence));

        p_attribute_specifier_sequence->first_token = ctx->current;

        while (ctx->current != NULL &&
            first_of_attribute_specifier(ctx))
        {
            struct attribute_specifier* owner p_attribute_specifier = attribute_specifier(ctx);

            p_attribute_specifier_sequence->attributes_flags |=
                p_attribute_specifier->attribute_list->attributes_flags;

            LIST_ADD(p_attribute_specifier_sequence, p_attribute_specifier);
        }
        p_attribute_specifier_sequence->last_token = ctx->previous;
    }



    return p_attribute_specifier_sequence;
}

struct attribute_specifier_sequence* owner attribute_specifier_sequence(struct parser_ctx* ctx)
{
    //attribute_specifier_sequence_opt attribute_specifier
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence = calloc(1, sizeof(struct attribute_specifier_sequence));
    while (ctx->current != NULL && first_of_attribute_specifier(ctx))
    {
        LIST_ADD(p_attribute_specifier_sequence, attribute_specifier(ctx));
    }
    return p_attribute_specifier_sequence;
}


struct attribute_specifier* owner attribute_specifier(struct parser_ctx* ctx)
{
    struct attribute_specifier* owner p_attribute_specifier = calloc(1, sizeof(struct attribute_specifier));

    p_attribute_specifier->first_token = ctx->current;

    //'[' '[' attribute_list ']' ']'
    parser_match_tk(ctx, '[');
    parser_match_tk(ctx, '[');
    p_attribute_specifier->attribute_list = move attribute_list(ctx);
    parser_match_tk(ctx, ']');
    p_attribute_specifier->last_token = ctx->current;
    parser_match_tk(ctx, ']');
    return p_attribute_specifier;
}



struct attribute_list* owner attribute_list(struct parser_ctx* ctx)
{
    struct attribute_list* owner p_attribute_list = calloc(1, sizeof(struct attribute_list));
    //
    //attribute_list ',' attribute_opt
    while (ctx->current != NULL && (
        first_of_attribute(ctx) ||
        ctx->current->type == ','))
    {
        if (first_of_attribute(ctx))
        {
            struct attribute* owner p_attribute = attribute(ctx);
            p_attribute_list->attributes_flags |= p_attribute->attributes_flags;
            LIST_ADD(p_attribute_list, p_attribute);
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
    //attribute_token attribute_argument_clause_opt
    p_attribute->attribute_token = move attribute_token(ctx);
    p_attribute->attributes_flags = p_attribute->attribute_token->attributes_flags;
    if (ctx->current->type == '(') //first
    {
        p_attribute->attribute_argument_clause = move attribute_argument_clause(ctx);
    }
    return p_attribute;
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

    parser_match_tk(ctx, TK_IDENTIFIER);

    if (ctx->current->type == '::')
    {
        parser_match(ctx);
        if (is_cake_attr)
        {
            //if (strcmp(ctx->current->lexeme, "implicit") == 0)
            //{
              //  p_attribute_token->attributes_flags = CAKE_ATTRIBUTE_IMPLICT;
            //}
            //else
            {
                compiler_set_warning_with_token(W_ATTRIBUTES, ctx, attr_token, "warning '%s' is not an cake attribute", ctx->current->lexeme);
            }
        }
        parser_match_tk(ctx, TK_IDENTIFIER);
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
            compiler_set_warning_with_token(W_ATTRIBUTES, ctx, attr_token, "warning '%s' is not an standard attribute", attr_token->lexeme);
        }
    }
    return p_attribute_token;
}



struct attribute_argument_clause* owner attribute_argument_clause(struct parser_ctx* ctx)
{
    struct attribute_argument_clause* owner p_attribute_argument_clause = calloc(1, sizeof(struct attribute_argument_clause));
    //'(' balanced_token_sequence_opt ')'
    parser_match_tk(ctx, '(');
    p_attribute_argument_clause->p_balanced_token_sequence = move balanced_token_sequence_opt(ctx);
    parser_match_tk(ctx, ')');
    return p_attribute_argument_clause;
}


struct balanced_token_sequence* owner balanced_token_sequence_opt(struct parser_ctx* ctx)
{
    struct balanced_token_sequence* owner p_balanced_token_sequence = calloc(1, sizeof(struct balanced_token_sequence));
    //balanced_token
    //balanced_token_sequence balanced_token
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
                //parser_match(ctx);
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
        compiler_set_error_with_token(C_ATTR_UNBALANCED, ctx, ctx->current, "expected ']' before ')'");

    }
    if (count3 != 0)
    {
        compiler_set_error_with_token(C_ATTR_UNBALANCED, ctx, ctx->current, "expected '}' before ')'");

    }
    return p_balanced_token_sequence;
}


struct statement* owner statement(struct parser_ctx* ctx)
{
    struct statement* owner p_statement = calloc(1, sizeof(struct statement));
    if (first_of_labeled_statement(ctx))
    {
        p_statement->labeled_statement = move labeled_statement(ctx);
    }
    else
    {
        p_statement->unlabeled_statement = move unlabeled_statement(ctx);
    }

    return p_statement;
}

struct primary_block* owner primary_block(struct parser_ctx* ctx)
{
    assert(ctx->current != NULL);
    struct primary_block* owner p_primary_block = calloc(1, sizeof(struct primary_block));
    if (first_of_compound_statement(ctx))
    {
        p_primary_block->compound_statement = move compound_statement(ctx);
    }
    else if (first_of_selection_statement(ctx))
    {
        p_primary_block->selection_statement = move  selection_statement(ctx);
    }
    else if (first_of_iteration_statement(ctx))
    {
        p_primary_block->iteration_statement = move iteration_statement(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_DEFER)
    {
        p_primary_block->defer_statement = move defer_statement(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_TRY)
    {
        p_primary_block->try_statement = move try_statement(ctx);
    }
    else
    {
        compiler_set_error_with_token(C_UNEXPECTED_TOKEN, ctx, ctx->current, "unexpected token");
    }
    return p_primary_block;
}


struct secondary_block* owner secondary_block(struct parser_ctx* ctx)
{
    check_open_brace_style(ctx, ctx->current);

    struct secondary_block* owner  p_secondary_block = calloc(1, sizeof(struct secondary_block));
    p_secondary_block->first_token = ctx->current;


    p_secondary_block->statement = move statement(ctx);

    p_secondary_block->last_token = ctx->previous;

    check_close_brace_style(ctx, p_secondary_block->last_token);

    return p_secondary_block;
}

bool first_of_primary_block(struct parser_ctx* ctx)
{
    if (first_of_compound_statement(ctx) ||
        first_of_selection_statement(ctx) ||
        first_of_iteration_statement(ctx) ||
        ctx->current->type == TK_KEYWORD_DEFER /*extension*/ ||
        ctx->current->type == TK_KEYWORD_TRY/*extension*/
        )
    {
        return true;
    }
    return false;
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

    if (first_of_primary_block(ctx))
    {
        p_unlabeled_statement->primary_block = move primary_block(ctx);
    }
    else if (first_of_jump_statement(ctx))
    {
        p_unlabeled_statement->jump_statement = move jump_statement(ctx);
    }
    else
    {
        p_unlabeled_statement->expression_statement = move expression_statement(ctx);
        if (p_unlabeled_statement->expression_statement)
        {
            if (p_unlabeled_statement->expression_statement->expression_opt)
            {
                if (!type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type) &&
                    type_is_nodiscard(&p_unlabeled_statement->expression_statement->expression_opt->type) &&
                    p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
                {

                    if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                    {
                        compiler_set_warning_with_token(W_ATTRIBUTES, ctx,
                            p_unlabeled_statement->expression_statement->expression_opt->first_token,
                            "ignoring return value of function declared with 'nodiscard' attribute");
                    }

                }
                if (type_is_owner(&p_unlabeled_statement->expression_statement->expression_opt->type) &&
                    p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
                {

                    if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                    {
                        compiler_set_warning_with_token(W_ATTRIBUTES, ctx,
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
                    p_unlabeled_statement->expression_statement->expression_opt->expression_type != UNARY_DECLARATOR_ATTRIBUTE_EXPR)
                {
                    if (ctx->current &&
                        ctx->current->level == 0)
                    {
                        compiler_set_warning_with_token(W_UNUSED_VALUE,
                            ctx,
                            p_unlabeled_statement->expression_statement->expression_opt->first_token,
                            "expression not used");
                    }
                }
            }
        }
    }

    return p_unlabeled_statement;
}
struct label* owner label(struct parser_ctx* ctx)
{
    struct label* owner p_label = calloc(1, sizeof(struct label));
    if (ctx->current->type == TK_IDENTIFIER)
    {
        p_label->name = ctx->current;
        parser_match(ctx);
        parser_match_tk(ctx, ':');
    }
    else if (ctx->current->type == TK_KEYWORD_CASE)
    {
        parser_match(ctx);
        p_label->constant_expression = move constant_expression(ctx);
        parser_match_tk(ctx, ':');
    }
    else if (ctx->current->type == TK_KEYWORD_DEFAULT)
    {
        parser_match(ctx);
        parser_match_tk(ctx, ':');
    }
    //attribute_specifier_sequence_opt identifier ':'
    //attribute_specifier_sequence_opt 'case' constant_expression ':'
    //attribute_specifier_sequence_opt 'default' ':'
    return p_label;
}

struct labeled_statement* owner labeled_statement(struct parser_ctx* ctx)
{
    struct labeled_statement* owner p_labeled_statement = calloc(1, sizeof(struct labeled_statement));
    //label statement
    p_labeled_statement->label = move label(ctx);
    p_labeled_statement->statement = move statement(ctx);
    return p_labeled_statement;
}

struct compound_statement* owner compound_statement(struct parser_ctx* ctx)
{
    //'{' block_item_list_opt '}'
    struct compound_statement* owner p_compound_statement = calloc(1, sizeof(struct compound_statement));
    struct scope block_scope = {.variables.capacity = 10};
    scope_list_push(&ctx->scopes, &block_scope);

    p_compound_statement->first_token = ctx->current;
    parser_match_tk(ctx, '{');

    if (ctx->current->type != '}')
    {
        p_compound_statement->block_item_list = move block_item_list(ctx);
    }

    p_compound_statement->last_token = ctx->current;
    parser_match_tk(ctx, '}');

    //TODO ver quem nao foi usado.

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
                        compiler_set_warning_with_token(W_UNUSED_VARIABLE,
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

    scope_list_pop(&ctx->scopes);

    scope_destroy(&block_scope);

    return p_compound_statement;
}

struct block_item_list block_item_list(struct parser_ctx* ctx)
{
    /*
      block_item_list:
      block_item
      block_item_list block_item
    */
    struct block_item_list block_item_list = {0};
    struct block_item* owner p_block_item = NULL;
    try
    {
        p_block_item = move block_item(ctx);
        if (p_block_item == NULL) throw;
        LIST_ADD(&block_item_list, p_block_item);
        p_block_item = NULL; /*MOVED*/

        while (ctx->current != NULL && ctx->current->type != '}') //follow
        {
            p_block_item = move block_item(ctx);
            if (p_block_item == NULL) throw;
            LIST_ADD(&block_item_list, p_block_item);
            p_block_item = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return block_item_list;
}

struct block_item* owner block_item(struct parser_ctx* ctx)
{
    //   declaration
    //     unlabeled_statement
    //   label
    struct block_item* owner p_block_item = calloc(1, sizeof(struct block_item));


    /*
    * Attributes can be first of declaration, labels etc..
    * so it is better to parse it in advance.
    */
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt =
        move attribute_specifier_sequence_opt(ctx);

    p_block_item->first_token = ctx->current;

    if (ctx->current->type == TK_KEYWORD__ASM)
    {  /*
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
        first_of_static_assert_declaration(ctx))
    {
        p_block_item->declaration = move declaration(ctx, move p_attribute_specifier_sequence_opt, STORAGE_SPECIFIER_AUTOMATIC_STORAGE);

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
        //so identifier confunde com expression
        p_block_item->label = move label(ctx);
    }
    else
    {
        p_block_item->unlabeled_statement = move unlabeled_statement(ctx);
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
    return p_block_item;
}


struct try_statement* owner try_statement(struct parser_ctx* ctx)
{
    struct try_statement* owner p_try_statement = calloc(1, sizeof(struct try_statement));

    p_try_statement->first_token = ctx->current;

    assert(ctx->current->type == TK_KEYWORD_TRY);
    const struct try_statement* try_statement_copy_opt = ctx->p_current_try_statement_opt;
    ctx->p_current_try_statement_opt = p_try_statement;
    ctx->try_catch_block_index++;
    p_try_statement->try_catch_block_index = ctx->try_catch_block_index;
    parser_match_tk(ctx, TK_KEYWORD_TRY);

    p_try_statement->secondary_block = move secondary_block(ctx);
    /*retores the previous one*/
    ctx->p_current_try_statement_opt = try_statement_copy_opt;


    if (ctx->current->type == TK_KEYWORD_CATCH)
    {
        p_try_statement->catch_token_opt = ctx->current;
        parser_match(ctx);

        p_try_statement->catch_secondary_block_opt = move secondary_block(ctx);
    }
    p_try_statement->last_token = ctx->previous;

    return p_try_statement;
}

struct selection_statement* owner selection_statement(struct parser_ctx* ctx)
{
    /*
    init-statement:
    expression-statement
    simple-declaration
    */
    /*
       'if' '(' init_statement_opt expression ')' statement
       'if' '(' init_statement_opt expression ')' statement 'else' statement
       'switch' '(' expression ')' statement
    */
    /*
       'if' '(' expression ')' statement
       'if' '(' expression ')' statement 'else' statement
       'switch' '(' expression ')' statement
    */
    struct selection_statement* owner p_selection_statement = calloc(1, sizeof(struct selection_statement));

    p_selection_statement->first_token = ctx->current;

    struct scope if_scope = {0};
    scope_list_push(&ctx->scopes, &if_scope); //variaveis decladas no if

    if (ctx->current->type == TK_KEYWORD_IF)
    {
        parser_match(ctx);

        if (!(ctx->current->flags & TK_FLAG_MACRO_EXPANDED)
            && !style_has_one_space(ctx->current))
        {
            compiler_set_info_with_token(W_STYLE, ctx, ctx->current, "one space");
        }

        parser_match_tk(ctx, '(');
        if (first_of_declaration_specifier(ctx))
        {
            struct declaration_specifiers* owner p_declaration_specifiers = move declaration_specifiers(ctx, STORAGE_SPECIFIER_AUTOMATIC_STORAGE);
            struct init_declarator_list list = init_declarator_list(ctx, p_declaration_specifiers, NULL);
            p_selection_statement->init_declarator = move list.head; //only one
            parser_match_tk(ctx, ';');
        }


        p_selection_statement->expression = move expression(ctx);

        if (constant_value_is_valid(&p_selection_statement->expression->constant_value))
        {
            //parser_setwarning_with_token(ctx, p_selection_statement->expression->first_token, "conditional expression is constant");
        }


        if (type_is_function(&p_selection_statement->expression->type) ||
            type_is_array(&p_selection_statement->expression->type))
        {
            compiler_set_warning_with_token(W_ADDRESS, ctx, ctx->current, "always true");
        }

        parser_match_tk(ctx, ')');

        p_selection_statement->secondary_block = move secondary_block(ctx);

        if (ctx->current)
        {
            if (ctx->current->type == TK_KEYWORD_ELSE)
            {
                p_selection_statement->else_token_opt = ctx->current;
                parser_match(ctx);
                p_selection_statement->else_secondary_block_opt = move secondary_block(ctx);
            }
        }
        else
        {
            compiler_set_error_with_token(C_UNEXPECTED_END_OF_FILE, ctx, ctx->input_list.tail, "unexpected end of file");
        }
    }
    else if (ctx->current->type == TK_KEYWORD_SWITCH)
    {
        parser_match(ctx);
        parser_match_tk(ctx, '(');

        p_selection_statement->expression = move expression(ctx);
        parser_match_tk(ctx, ')');

        p_selection_statement->secondary_block = move secondary_block(ctx);

    }
    else
    {
        assert(false);
        compiler_set_error_with_token(C_UNEXPECTED_TOKEN, ctx, ctx->input_list.tail, "unexpected token");
    }

    p_selection_statement->last_token = ctx->previous;

    scope_list_pop(&ctx->scopes);

    scope_destroy(&if_scope);

    return p_selection_statement;
}

struct defer_statement* owner  defer_statement(struct parser_ctx* ctx)
{
    struct defer_statement* owner p_defer_statement = calloc(1, sizeof(struct defer_statement));
    if (ctx->current->type == TK_KEYWORD_DEFER)
    {
        p_defer_statement->first_token = ctx->current;
        parser_match(ctx);
        p_defer_statement->secondary_block = move secondary_block(ctx);
        p_defer_statement->last_token = ctx->previous;
    }
    return p_defer_statement;
}

struct iteration_statement* owner  iteration_statement(struct parser_ctx* ctx)
{
    /*
    iteration-statement:
      while ( expression ) statement
      do statement while ( expression ) ;
      for ( expressionopt ; expressionopt ; expressionopt ) statement
      for ( declaration expressionopt ; expressionopt ) statement
    */
    struct iteration_statement* owner  p_iteration_statement = calloc(1, sizeof(struct iteration_statement));
    p_iteration_statement->first_token = ctx->current;
    if (ctx->current->type == TK_KEYWORD_DO)
    {
        parser_match(ctx);
        p_iteration_statement->secondary_block = move secondary_block(ctx);
        p_iteration_statement->second_token = ctx->current;
        parser_match_tk(ctx, TK_KEYWORD_WHILE);
        parser_match_tk(ctx, '(');

        p_iteration_statement->expression1 = move expression(ctx);
        parser_match_tk(ctx, ')');
        parser_match_tk(ctx, ';');
    }
    else if (ctx->current->type == TK_KEYWORD_REPEAT)
    {
        parser_match(ctx);
        p_iteration_statement->secondary_block = move secondary_block(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_WHILE)
    {
        parser_match(ctx);
        parser_match_tk(ctx, '(');

        p_iteration_statement->expression1 = move expression(ctx);
        parser_match_tk(ctx, ')');
        p_iteration_statement->secondary_block = move secondary_block(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_FOR)
    {
        parser_match(ctx);
        parser_match_tk(ctx, '(');
        if (first_of_declaration_specifier(ctx))
        {
            struct scope for_scope = {0};
            scope_list_push(&ctx->scopes, &for_scope);

            struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt =
                move attribute_specifier_sequence_opt(ctx);

            p_iteration_statement->declaration = move declaration(ctx, move p_attribute_specifier_sequence_opt, STORAGE_SPECIFIER_AUTOMATIC_STORAGE);
            if (ctx->current->type != ';')
            {
                p_iteration_statement->expression1 = move expression(ctx);
            }
            parser_match_tk(ctx, ';');
            if (ctx->current->type != ')')
                p_iteration_statement->expression2 = move expression(ctx);

            parser_match_tk(ctx, ')');

            p_iteration_statement->secondary_block = move secondary_block(ctx);

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
                p_iteration_statement->expression0 = move expression(ctx);
            parser_match_tk(ctx, ';');
            if (ctx->current->type != ';')
                p_iteration_statement->expression1 = move expression(ctx);
            parser_match_tk(ctx, ';');
            if (ctx->current->type != ')')
                p_iteration_statement->expression2 = move expression(ctx);
            parser_match_tk(ctx, ')');

            p_iteration_statement->secondary_block = move secondary_block(ctx);
        }
    }
    return p_iteration_statement;
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

    p_jump_statement->first_token = ctx->current;

    if (ctx->current->type == TK_KEYWORD_GOTO)
    {
        parser_match(ctx);
        p_jump_statement->label = ctx->current;
        parser_match_tk(ctx, TK_IDENTIFIER);
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

            compiler_set_error_with_token(C_THROW_STATEMENT_NOT_WITHIN_TRY_BLOCK, ctx, ctx->current, "throw statement not within try block");
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

        if (ctx->current->type == TK_KEYWORD__MOVE)
        {
            p_jump_statement->token_move = ctx->current;
            parser_match(ctx);
        }

        if (ctx->current->type != ';')
        {
            p_jump_statement->expression_opt = move expression(ctx);


            if (p_jump_statement->expression_opt)
            {
                /*
                * Check is return type is compatible with function return
                */
                struct type return_type =
                    get_function_return_type(&ctx->p_current_function_opt->init_declarator_list.head->p_declarator->type);

                if (type_is_void(&return_type))
                {
                    compiler_set_error_with_token(C_VOID_FUNCTION_SHOULD_NOT_RETURN_VALUE,
                        ctx,
                        p_return_token,
                        "void function '%s' should not return a value",
                        ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name->lexeme);
                }
                else
                {
                    if (p_jump_statement->expression_opt)
                    {
                        check_assigment(ctx,
                            &return_type,
                            p_jump_statement->expression_opt,
                            p_jump_statement->token_move != NULL,
                            true);

                    }
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
    parser_match_tk(ctx, ';');
    return p_jump_statement;
}

struct expression_statement* owner expression_statement(struct parser_ctx* ctx)
{
    struct expression_statement* owner p_expression_statement = calloc(1, sizeof(struct expression_statement));
    /*
     expression-statement:
       expression opt ;
       attribute-specifier-sequence expression ;
    */

    p_expression_statement->p_attribute_specifier_sequence_opt =
        move attribute_specifier_sequence_opt(ctx);

    if (ctx->current->type != ';')
    {
        p_expression_statement->expression_opt = move expression(ctx);
    }

    parser_match_tk(ctx, ';');

    return p_expression_statement;
}

void declaration_list_add(struct declaration_list* list, struct declaration* owner p_declaration)
{
    if (list->head == NULL)
    {
        list->head = move p_declaration;
    }
    else
    {
        list->tail->next = move p_declaration;
    }
    list->tail = p_declaration;
}

void declaration_list_destroy(struct declaration_list* list)
{

}

struct declaration_list translation_unit(struct parser_ctx* ctx)
{
    struct declaration_list declaration_list = {0};
    /*
      translation_unit:
      external_declaration
      translation_unit external_declaration
    */
    while (ctx->current != NULL)
    {
        declaration_list_add(&declaration_list, move external_declaration(ctx));
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
                    compiler_set_warning_with_token(W_UNUSED_VARIABLE,
                        ctx,
                        p_declarator->name,
                        "declarator '%s' not used", p_declarator->name->lexeme);
                }
            }

            entry = entry->next;
        }
    }
}

struct declaration_list parse(struct parser_ctx* ctx,
    struct token_list* list)
{

    s_anonymous_struct_count = 0;

    struct scope file_scope = {0};

    scope_list_push(&ctx->scopes, &file_scope);
    ctx->input_list = *list;
    ctx->current = ctx->input_list.head;
    parser_skip_blanks(ctx);

    struct declaration_list l = translation_unit(ctx);
    show_unused_file_scope(ctx);


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
            char buffer[200];
            snprintf(buffer, sizeof buffer, "#define %s \n", argv[i] + 2);
            struct tokenizer_ctx tctx = {0};
            struct token_list l1 = tokenizer(&tctx, buffer, "", 0, TK_FLAG_NONE);
            struct token_list r = preprocessor(prectx, &l1, 0);
            token_list_clear(&l1);
            token_list_clear(&r);
            continue;
        }
    }
    return 0;
}

#ifdef _WIN32
unsigned long __stdcall GetEnvironmentVariableA(
    const char* lpname,
    char* lpbuffer,
    unsigned long nsize
);
#endif

void append_msvc_include_dir(struct preprocessor_ctx* prectx)
{
#ifdef _WIN32

    /*
     * Let's get the msvc command prompt INCLUDE
    */
    char env[2000];
    int n = GetEnvironmentVariableA("INCLUDE", env, sizeof(env));

    if (n == 0)
    {
        /*
         * Used in debug inside VC IDE
         * type on msvc command prompt:
         * echo %INCLUDE%
         * to generate this string
        */
#if 1  /*DEBUG INSIDE MSVC IDE*/

#define STR \
 "C:\\Program Files\\Microsoft Visual Studio\\2022\\Preview\\VC\\Tools\\MSVC\\14.37.32820\\include;C:\\Program Files\\Microsoft Visual Studio\\2022\\Preview\\VC\\Auxiliary\\VS\\include;C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.22000.0\\ucrt;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\um;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\shared;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\winrt;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\cppwinrt\n"\


#define STR_E \
 "C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Tools\\MSVC\\14.36.32532\\include;C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Tools\\MSVC\\14.36.32532\\ATLMFC\\include;C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\VS\\include;C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.22000.0\\ucrt;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\um;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\shared;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\winrt;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\cppwinrt;C:\\Program Files (x86)\\Windows Kits\\NETFXSDK\\4.8\\include\\um"



        //http://thradams.com/app/litapp.html
        snprintf(env, sizeof env,
            "%s",
            STR);


        n = (int) strlen(env);
#endif
    }


    if (n > 0 && n < sizeof(env))
    {
        const char* p = env;

        for (;;)
        {
            if (*p == '\0')
            {
                break;
            }
            char filename_local[500] = {0};
            int count = 0;
            while (*p != '\0' && *p != ';')
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
    struct ast ast = {0};
    const char* owner s = NULL;


    struct preprocessor_ctx prectx = {0};

    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx);

    struct report report = {0};
    struct parser_ctx ctx = {0};
    ctx.options = *options;
    ctx.p_report = &report;

    try
    {
        prectx.options = *options;
        append_msvc_include_dir(&prectx);

        struct tokenizer_ctx tctx = {0};
        struct token_list tokens = tokenizer(&tctx, content, "", 0, TK_FLAG_NONE);
        ast.token_list = move preprocessor(&prectx, &tokens, 0);
        if (prectx.n_errors != 0) throw;


        ast.declaration_list = move parse(&ctx, &ast.token_list);
        if (report.error_count > 0) throw;

        struct format_visit_ctx visit_ctx = {0};
        visit_ctx.ast = ast;
        format_visit(&visit_ctx);

        if (options->direct_compilation)
            s = move get_code_as_compiler_see(&visit_ctx.ast.token_list);
        else
            s = move get_code_as_we_see(&visit_ctx.ast.token_list, options->remove_comments);

    }
    catch
    {

    }

    parser_ctx_destroy(&ctx);
    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);
    return s;
}


void ast_format_visit(struct ast* ast)
{
    /*format input source before transformation*/
    struct format_visit_ctx visit_ctx = {0};
    visit_ctx.ast = *ast;
    format_visit(&visit_ctx);
}

void c_visit(struct ast* ast)
{

}

void ast_wasm_visit(struct ast* ast)
{
    struct wasm_visit_ctx ctx = {0};
    ctx.ast = *ast;
    wasm_visit(&ctx);
}

int compile_one_file(const char* file_name,
    struct options* options,
    const char* out_file_name,
    int argc,
    const char** argv,
    struct report* report)
{
    printf("%s\n", file_name);
    struct preprocessor_ctx prectx = {0};

    prectx.macros.capacity = 5000;

    add_standard_macros(&prectx);

    //print_all_macros(&prectx);

    struct ast ast = {0};

    const char* owner s = NULL;

    struct parser_ctx ctx = {0};
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



        content = move read_file(file_name);
        if (content == NULL)
        {
            report->error_count++;
            printf("file not found '%s'\n", file_name);
            throw;
        }

        if (options->sarif_output)
        {
            char sarif_file_name[260];
            strcpy(sarif_file_name, file_name);
            strcat(sarif_file_name, ".sarif");
            ctx.sarif_file = move(FILE * owner) fopen(sarif_file_name, "w");
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

        struct tokenizer_ctx tctx = {0};
        struct token_list tokens = tokenizer(&tctx, content, file_name, 0, TK_FLAG_NONE);


        ast.token_list = move preprocessor(&prectx, &tokens, 0);
        if (prectx.n_errors > 0) throw;


        if (options->preprocess_only)
        {
            const char* owner s2 = print_preprocessed_to_string2(ast.token_list.head);
            printf("%s", s2);
            free((void* owner) s2);
        }
        else
        {

            ast.declaration_list = move parse(&ctx, &ast.token_list);
            if (report->error_count > 0) throw;

            //ast_wasm_visit(&ast);

            if (!options->no_output)
            {
                if (options->format_input)
                {
                    struct format_visit_ctx f = {.ast = ast, .identation = 4};
                    format_visit(&f);
                }
                struct visit_ctx visit_ctx = {0};
                visit_ctx.target = options->target;
                visit_ctx.ast = ast;
                visit(&visit_ctx);

                if (options->direct_compilation)
                    s = move get_code_as_compiler_see(&visit_ctx.ast.token_list);
                else
                    s = move get_code_as_we_see(&visit_ctx.ast.token_list, options->remove_comments);

                if (options->format_ouput)
                {
                    /*re-parser ouput and format*/
                    const char* owner s2 = format_code(options, s);
                    free(s);
                    s = move s2;
                }

                FILE* out = fopen(out_file_name, "w");
                if (out)
                {
                    fprintf(out, "%s", s);
                    fclose(out);
                    //printf("%-30s ", path);
                }
                else
                {
                    report->error_count++;
                    printf("cannot open output file '%s' - %s\n", out_file_name, get_posix_error_message(errno));
                    throw;
                }
            }
        }
    }
    catch
    {
        //printf("Error %s\n", error->message);
    }

    if (ctx.sarif_file)
    {
        if (ctx.sarif_file)
        {
#define END \
"      ],\n"\
"      \"tool\": {\n"\
"        \"driver\": {\n"\
"          \"name\": \"cake\",\n"\
"          \"fullName\": \"cake code analysis\",\n"\
"          \"version\": \"0.5\",\n"\
"          \"informationUri\": \"https://github.com/cake-build\"\n"\
"        }\n"\
"      }\n"\
"    }\n"\
"  ]\n"\
"}\n"\
"\n"
            fprintf(ctx.sarif_file, "%s", END);
        }
        fclose(ctx.sarif_file);
    }


    parser_ctx_destroy(&ctx);
    free((char* owner) s);
    free(content);
    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);

    return report->error_count > 0;
}

static void longest_common_path(int argc, const char** argv, char root_dir[MAX_PATH])
{
    /*
     find the longest common path
    */
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
            continue;

        char fullpath_i[MAX_PATH] = {0};
        realpath(argv[i], fullpath_i);
        strcpy(root_dir, fullpath_i);
        dirname(root_dir);

        for (int k = 0; ; k++)
        {
            const char ch = fullpath_i[k];
            for (int j = 2; j < argc; j++)
            {
                if (argv[j][0] == '-')
                    continue;

                char fullpath_j[MAX_PATH] = {0};
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

    const char* p = outdir + strlen(root) + 1;
    for (;;)
    {
        if (*p != '\0' && *p != '/' && *p != '\\')
        {
            p++;
            continue;
        }

        char temp[MAX_PATH] = {0};
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
}

int compile(int argc, const char** argv, struct report* report)
{
    struct options options = {0};
    if (fill_options(&options, argc, argv) != 0)
    {
        return 1;
    }

    clock_t begin_clock = clock();
    int no_files = 0;

    char root_dir[MAX_PATH] = {0};

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
        char output_file[400] = {0};

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
                char fullpath[MAX_PATH] = {0};
                realpath(argv[i], fullpath);

                strcpy(output_file, root_dir);
                strcat(output_file, "\\out");

                strcat(output_file, fullpath + root_dir_len);

                char outdir[MAX_PATH];
                strcpy(outdir, output_file);
                dirname(outdir);
                if (create_multiple_paths(root_dir, outdir) != 0)
                {
                    return 1;
                }
            }
        }

        char fullpath[260];
        realpath(argv[i], fullpath);
        compile_one_file(fullpath, &options, output_file, argc, argv, report);
    }


    clock_t end_clock = clock();
    double cpu_time_used = ((double) (end_clock - begin_clock)) / CLOCKS_PER_SEC;
    report->no_files = no_files;
    report->cpu_time_used_sec = cpu_time_used;
    return 0;
}


struct ast get_ast(struct options* options,
    const char* filename,
    const char* source,
    struct report* report)
{
    struct ast ast = {0};
    struct tokenizer_ctx tctx = {0};

    struct token_list list = tokenizer(&tctx, source, filename, 0, TK_FLAG_NONE);

    struct preprocessor_ctx prectx = {0};
    prectx.options = *options;
    prectx.macros.capacity = 5000;

    add_standard_macros(&prectx);


    ast.token_list = move preprocessor(&prectx, &list, 0);
    token_list_destroy(&list);

    if (prectx.n_errors > 0)
        return ast;

    struct parser_ctx ctx = {0};
    ctx.options = *options;
    ctx.p_report = report;
    ast.declaration_list = move parse(&ctx, &ast.token_list);


    parser_ctx_destroy(&ctx);



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
            break;/*nao tem mais lugares*/
    }
    return argvc;
}

const char* owner compile_source(const char* pszoptions, const char* content, struct report* report)
{
    const char* argv[100] = {0};
    char string[200] = {0};
    snprintf(string, sizeof string, "exepath %s", pszoptions);

    const int argc = strtoargv(string, 10, argv);

    const char* owner s = NULL;

    struct preprocessor_ctx prectx = {0};
    struct ast ast = {0};
    struct options options = {.input = LANGUAGE_CXX};

    struct visit_ctx visit_ctx = {0};
    visit_ctx.target = options.target;

    //printf("options '%s'\n", pszoptions);
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
            struct tokenizer_ctx tctx = {0};
            struct token_list tokens = tokenizer(&tctx, content, "source", 0, TK_FLAG_NONE);


            struct token_list token_list = preprocessor(&prectx, &tokens, 0);
            if (prectx.n_errors == 0)
            {
                s = move print_preprocessed_to_string2(token_list.head);
            }

            preprocessor_ctx_destroy(&prectx);

            token_list_destroy(&tokens);
        }
        else
        {

            ast = move get_ast(&options, "source", content, report);
            if (report->error_count > 0) throw;

            visit_ctx.ast = ast;
            visit(&visit_ctx);

            if (options.direct_compilation)
            {
                s = move get_code_as_compiler_see(&visit_ctx.ast.token_list);
            }
            else
            {
                s = move get_code_as_we_see(&visit_ctx.ast.token_list, options.remove_comments);
            }
            if (options.format_ouput)
            {

                /*re-parser ouput and format*/
                const char* owner s2 = format_code(&options, s);
                free(s);
                s = move s2;
            }

        }
    }
    catch
    {
    }

    visit_ctx_destroy(&visit_ctx);
    ast_destroy(&ast);

    return s;
}


/*Função exportada para web*/
char* CompileText(const char* pszoptions, const char* content)
{
    printf(WHITE "Cake " CAKE_VERSION RESET "\n");
    struct report report = {0};
    return  (char*) compile_source(pszoptions, content, &report);
}

void ast_destroy(implicit struct ast* obj_owner ast)
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
            //ok
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
            //ok
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
            //ok
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
    if (!parser_is_warning_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (ctx->options.style == STYLE_CAKE)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_set_info_with_token(W_STYLE, ctx, token, "use snake_case for struct/union tags");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_pascal_case(token->lexeme))
        {
            compiler_set_info_with_token(W_STYLE, ctx, token, "use camelCase for struct/union tags");
        }
    }
}

void naming_convention_enum_tag(struct parser_ctx* ctx, struct token* token)
{
    if (!parser_is_warning_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }


    if (ctx->options.style == STYLE_CAKE)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_set_info_with_token(W_STYLE, ctx, token, "use snake_case for enum tags");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_pascal_case(token->lexeme))
        {
            compiler_set_info_with_token(W_STYLE, ctx, token, "use PascalCase for enum tags");
        }
    }
}

void naming_convention_function(struct parser_ctx* ctx, struct token* token)
{
    if (token == NULL)
        return;

    if (!parser_is_warning_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (ctx->options.style == STYLE_CAKE)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_set_info_with_token(W_STYLE, ctx, token, "use snake_case for functions");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_pascal_case(token->lexeme))
        {
            compiler_set_info_with_token(W_STYLE, ctx, token, "use PascalCase for functions");
        }
    }
}

void naming_convention_global_var(struct parser_ctx* ctx, struct token* token, struct type* type, enum storage_class_specifier_flags storage)
{
    if (!parser_is_warning_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }


    if (!type_is_function_or_function_pointer(type))
    {
        if (storage & STORAGE_SPECIFIER_STATIC)
        {
            if (token->lexeme[0] != 's' || token->lexeme[1] != '_')
            {
                compiler_set_info_with_token(W_STYLE, ctx, token, "use prefix s_ for static global variables");
            }
        }
        if (!is_snake_case(token->lexeme))
        {
            compiler_set_info_with_token(W_STYLE, ctx, token, "use snake_case global variables");
        }
    }
}

void naming_convention_local_var(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!parser_is_warning_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }


    if (ctx->options.style == STYLE_CAKE)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_set_info_with_token(W_STYLE, ctx, token, "use snake_case for local variables");
        }
    }
    else if (ctx->options.style == STYLE_MICROSOFT)
    {
        if (!is_camel_case(token->lexeme))
        {
            compiler_set_info_with_token(W_STYLE, ctx, token, "use camelCase for local variables");
        }
    }

}

void naming_convention_enumerator(struct parser_ctx* ctx, struct token* token)
{
    if (!parser_is_warning_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (!is_all_upper(token->lexeme))
    {
        compiler_set_info_with_token(W_STYLE, ctx, token, "use UPPERCASE for enumerators");
    }
}

void naming_convention_struct_member(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!parser_is_warning_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (!is_snake_case(token->lexeme))
    {
        compiler_set_info_with_token(W_STYLE, ctx, token, "use snake_case for struct members");
    }
}

void naming_convention_parameter(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!parser_is_warning_enabled(ctx, W_STYLE) || token->level != 0)
    {
        return;
    }

    if (!is_snake_case(token->lexeme))
    {
        compiler_set_info_with_token(W_STYLE, ctx, token, "use snake_case for arguments");
    }
}

#ifdef TEST
#include "unit_test.h"

static bool compile_without_errors(const char* src)
{
    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", src, &report);
    return report.error_count == 0;
}







static bool compile_with_errors(const char* src)
{

    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", src, &report);
    return report.error_count != 0;
}


void parser_specifier_test()
{
    const char* src = "long long long i;";
    assert(compile_with_errors(src));
}

void array_item_type_test()
{
    const char* src =
        "void (*pf[10])(void* val);\n"
        "static_assert(_is_same(typeof(pf[0]), void (*)(void* val)));\n";
    assert(compile_without_errors(src));
}

void take_address_type_test()
{
    const char* src =
        "void F(char(*p)[10])"
        "{"
        "    (*p)[0] = 'a';"
        "}";
    assert(compile_without_errors(src));
}

void parser_scope_test()
{
    const char* src = "void f() {int i; char i;}";
    assert(compile_with_errors(src));
}

void parser_tag_test()
{
    //mudou tipo do tag no mesmo escopo
    const char* src = "enum E { A }; struct E { int i; };";
    assert(compile_with_errors(src));
}

void string_concatenation_test()
{
    const char* src = " const char* s = \"part1\" \"part2\";";
    assert(compile_without_errors(src));
}

void test_digit_separator()
{
    struct report report = {0};
    char* result = compile_source("-std=c99", "int i = 1'000;", &report);
    assert(strcmp(result, "int i = 1000;") == 0);
    free(result);
}

void test_lit()
{
    struct report report = {0};
    char* result = compile_source("-std=c99", "char * s = u8\"maçã\";", &report);
    assert(strcmp(result, "char * s = \"ma\\xc3\\xa7\\xc3\\xa3\";") == 0);
    free(result);
}

void type_test2()
{
    char* src =
        "int a[10];\n"
        " static_assert(_is_same(typeof(&a) ,int (*)[10]));\n"
        ;

    assert(compile_without_errors(src));
}

void type_test3()
{
    char* src =
        "int i;"
        "int (*f)(void);"
        " static_assert(_is_same(typeof(&i), int *));"
        " static_assert(_is_same(typeof(&f), int (**)(void)));"
        ;

    assert(compile_without_errors(src));
}

void crazy_decl()
{
    const char* src =
        "void (*f(int i))(void)\n"
        "{\n"
        "   i = 1; \n"
        "    return 0;\n"
        "}\n";

    assert(compile_without_errors(src));
}

void crazy_decl2()
{
    const char* src =
        "void (*f(int i))(void)\n"
        "{\n"
        "   i = 1; \n"
        "    return 0;\n"
        "}\n"
        "int main()\n"
        "{\n"
        "  f(1);\n"
        "}\n";

    assert(compile_without_errors(src));
}


void crazy_decl4()
{
    const char* src =
        "void (*F(int a, int b))(void) { return 0; }\n"
        "void (*(*PF)(int a, int b))(void) = F;\n"
        "int main() {\n"
        "    PF(1, 2);\n"
        "}\n";

    assert(compile_without_errors(src));
}

void sizeof_array_test()
{
    assert(compile_without_errors(
        "int main() {\n"
        "int a[] = { 1, 2, 3 };\n"
        "static_assert(sizeof(a) == sizeof(int) * 3);\n"
        "}\n"
    ));
}

void sizeof_test()
{

    const char* src =
        "static_assert(sizeof(\"ABC\") == 4);"
        "char a[10];"
        "char b[10][2];"
        "static_assert(sizeof(a) == 10);"
        "static_assert(sizeof(b) == sizeof(char)*10*2);"
        "char *p[10];"
        "static_assert(sizeof(p) == 40);"
        "static_assert(sizeof(int) == 4);"
        "static_assert(sizeof(long) == 4);"
        "static_assert(sizeof(char) == 1);"
        "static_assert(sizeof(short) == 4);"
        "static_assert(sizeof(unsigned int) == 4);"
        "static_assert(sizeof(void (*pf)(int i)) == sizeof(void*));"
        ;

    assert(compile_without_errors(src));
}


void alignof_test()
{
    const char* src =
        "struct X { char s; double c; char s2;};\n"
        "static_assert(alignof(struct X) == 8);"
        "static_assert(sizeof(struct X) == 24);"
        ;

    assert(compile_without_errors(src));
}



void indirection_struct_size()
{
    const char* src =
        "typedef struct X X;\n"
        "struct X {\n"
        "    void* data;\n"
        "};\n"
        "static_assert(sizeof(X) == sizeof(void*));"
        ;

    assert(compile_without_errors(src));
}

void traits_test()
{
    //https://en.cppreference.com/w/cpp/header/type_traits
    const char* src =
        "void (*F)();\n"
        "static_assert(_is_pointer(F));\n"
        "static_assert(_is_integral(1));\n"
        "int a[2];\n"
        "static_assert(_is_array(a));\n"
        "int((a2))[10];\n"
        "static_assert(_is_array(a2));"
        ;
    assert(compile_without_errors(src));
}

void comp_error1()
{
    const char* src =
        "void F() {\n"
        "    char* z;\n"
        "    *z-- = '\\0';\n"
        "}\n";

    assert(compile_without_errors(src));
}

void array_size()
{
    const char* src =
        "void (*f[2][3])(int i);\n"
        "int main() {\n"
        "static_assert(sizeof(void (*[2])(int i)) == sizeof(void*) * 2);\n"
        "static_assert(sizeof(f) == sizeof(void (*[2])(int i)) * 3);\n"
        "}"
        ;

    assert(compile_without_errors(src));
}


void expr_type()
{
    const char* src =
        "static_assert(_is_same(typeof(1 + 2.0), double));";

    assert(compile_without_errors(src));
}

void expand_test()
{
    char* src =
        "typedef int A[2];"
        "typedef A *B [1];"
        "static_assert(_is_same(typeof(B), int (*[1])[2]));";
    ;

    assert(compile_without_errors(src));

    //https://godbolt.org/z/WbK9zP7zM
}

void expand_test2()
{

    char* src2 =
        "typedef char* A;"
        "typedef const A* B; "
        "static_assert(_is_same(typeof(B), char * const *));";

    assert(compile_without_errors(src2));

    //https://godbolt.org/z/WbK9zP7zM
}
void expand_test3()
{


    char* src3 =
        "typedef char* T1;"
        "typedef T1(*f[3])(int); "
        "static_assert(_is_same(typeof(f), char* (* [3])(int)));";

    assert(compile_without_errors(src3));

    //https://godbolt.org/z/WbK9zP7zM
}

void bigtest()
{
    const char* str =
        "\n"
        "\n"
        "struct X { int i; };\n"
        "\n"
        "struct Y { double d;};\n"
        "\n"
        "enum E { A = 1 };\n"
        "enum E e1;\n"
        "\n"
        "struct X* F() { return 0; }\n"
        "\n"
        "int main()\n"
        "{\n"
        "    enum E { B } e2;\n"
        "    static_assert(_is_same(typeof(e2), enum E));\n"
        "\n"
        "    static_assert(!_is_same(typeof(e2), typeof(e1)));\n"
        "\n"
        "\n"
        "    struct X x;\n"
        "    struct Y y;\n"
        "\n"
        "    static_assert(_is_same(typeof(x), struct X));\n"
        "    static_assert(!_is_same(typeof(x), struct Y));\n"
        "\n"
        "    static_assert(!_is_same(int(double), int()));\n"
        "\n"
        "    int aa[10];\n"
        "\n"
        "    static_assert(_is_same(typeof(*F()), struct X));\n"
        "    static_assert(_is_same(typeof(&aa), int(*)[10]));\n"
        "\n"
        "    int* p = 0;\n"
        "    static_assert(_is_same(typeof(*(p + 1)), int));\n"
        "\n"
        "    static_assert(_is_same(typeof(1), int));\n"
        "\n"
        "    static_assert(_is_same(typeof(main), int()));\n"
        "\n"
        "\n"
        "    static_assert(!_is_same(typeof(main), int(double)));\n"
        "    static_assert(!_is_same(typeof(main), int));\n"
        "\n"
        "\n"
        "    struct X x2;\n"
        "    enum E e;\n"
        "    static_assert(_is_same(typeof(e), enum E));\n"
        "    static_assert(_is_same(typeof(x2), struct X));\n"
        "    static_assert(!_is_same(typeof(e), struct X));\n"
        "\n"
        "\n"
        "\n"
        "    static_assert(_is_same(typeof(1L), long));\n"
        "    static_assert(_is_same(typeof(1UL) , unsigned long));\n"
        "    static_assert(_is_same(typeof(1ULL), unsigned long long));\n"
        "    \n"
        "    //static_assert(_is_same(typeof(A), int));\n"
        "\n"
        "    static_assert(_is_same(typeof(1.0), double));\n"
        "    static_assert(_is_same(typeof(1.0f), float));\n"
        "    static_assert(_is_same(typeof(1.0L), long double));\n"
        "    \n"
        "    \n"
        "    static_assert(_is_same(typeof(((int*)0) + 1), int*));\n"
        "    static_assert(_is_same(typeof(*(((int*)0) + 1)), int));\n"
        "\n"
        "}\n"
        "\n"
        "\n"
        ;
    assert(compile_without_errors(str));
}

void literal_string_type()
{
    const char* source =
        "    static_assert(_is_same(typeof(\"A\"),  char [2]));\n"
        "    static_assert(_is_same(typeof(\"AB\"),  char [3]));\n"
        ;

    assert(compile_without_errors(source));
}

void digit_separator_test()
{
    const char* source =
        "static_assert(1'00'00 == 10000);"
        ;

    assert(compile_without_errors(source));
}


void numbers_test()
{
    const char* source =
        "#if 0xA1 == 161\n"
        "_Static_assert(0xA1 == 161); \n"
        "#endif"
        ;

    assert(compile_without_errors(source));
}

void binary_digits_test()
{
    const char* source =
        "_Static_assert(0b101010 == 42);"
        "_Static_assert(0b1010'10 == 42);"
        "_Static_assert(052 == 42);"
        ;

    assert(compile_without_errors(source));
}


void type_suffix_test()
{
    const char* source =
        "\n"
        "#ifdef __cplusplus\n"
        "#include <type_traits>\n"
        "#define typeof decltype\n"
        "#define _is_same(a, b) std::is_same<a, b>::value\n"
        "#endif\n"
        "\n"
        "\n"
        "static_assert(_is_same(typeof(1), int));\n"
        "static_assert(_is_same(typeof(1L), long));\n"
        "static_assert(_is_same(typeof(1LL), long long));\n"
        "static_assert(_is_same(typeof(1U), unsigned int));\n"
        "static_assert(_is_same(typeof(1ULL), unsigned long long));\n"
        "static_assert(_is_same(typeof(1), int));\n"
        "static_assert(_is_same(typeof(1l), long));\n"
        "static_assert(_is_same(typeof(1ll), long long) );\n"
        "static_assert(_is_same(typeof(1u), unsigned int));\n"
        "static_assert(_is_same(typeof(1ull), unsigned long long));\n"
        "static_assert(_is_same(typeof(0x1), int));\n"
        "static_assert(_is_same(typeof(0x1L), long));\n"
        "static_assert(_is_same(typeof(0x1LL), long long));\n"
        "static_assert(_is_same(typeof(0x1U), unsigned int));\n"
        "static_assert(_is_same(typeof(0x1ULL), unsigned long long));  \n"
        "static_assert(_is_same(typeof(0x1), int));\n"
        "static_assert(_is_same(typeof(0x1l), long));\n"
        "static_assert(_is_same(typeof(0x1ll), long long));\n"
        "static_assert(_is_same(typeof(0x1u), unsigned int));\n"
        "static_assert(_is_same(typeof(0x1ull), unsigned long long));\n"
        "static_assert(_is_same(typeof(0b1), int));\n"
        "static_assert(_is_same(typeof(0b1L), long));\n"
        "static_assert(_is_same(typeof(0b1LL), long long));\n"
        "static_assert(_is_same(typeof(0b1U), unsigned int));\n"
        "static_assert(_is_same(typeof(0b1ULL), unsigned long long));\n"
        "static_assert(_is_same(typeof(0b1l), long));\n"
        "static_assert(_is_same(typeof(0b1ll), long long));\n"
        "static_assert(_is_same(typeof(0b1ul), unsigned long));\n"
        "static_assert(_is_same(typeof(0b1ull), unsigned long long));\n"
        "static_assert(_is_same(typeof(1.0f), float));\n"
        "static_assert(_is_same(typeof(1.0), double));\n"
        "static_assert(_is_same(typeof(1.0L), long double));\n"
        ;


    assert(compile_without_errors(source));
}


void type_test()
{
    const char* source =
        "int * p = 0;"
        "static_assert(_is_same( typeof( *(p + 1) ), int)   );"
        ;

    assert(compile_without_errors(source));
}

void is_pointer_test()
{
    const char* source =
        "\n"
        "int main()\n"
        "{\n"
        "  int i;\n"
        "  static_assert(_is_integral(i));\n"
        "  static_assert(_is_floating_point(double) && _is_floating_point(float));\n"
        "  static_assert(_is_function(main));\n"
        "\n"
        "  char * p;\n"
        "  static_assert(_is_scalar(p));\n"
        "  static_assert(_is_scalar(nullptr));\n"
        "\n"
        "  int a[10];\n"
        "  static_assert(_is_array(a));\n"
        "\n"
        "  /*pf = pointer to function (void) returning array 10 of int*/\n"
        "  int (*pf)(void)[10];\n"
        "  static_assert(!_is_array(pf));\n"
        "  static_assert(_is_pointer(pf));\n"
        "\n"
        "  static_assert(_is_same(int, typeof(i)));\n"
        "\n"
        "  static_assert(_is_const(const int));\n"
        "  static_assert(!_is_const(const int*));\n"
        "  static_assert(_is_const(int* const));\n"
        "\n"
        "}\n"
        ;
    assert(compile_without_errors(source));
}



void params_test()
{
    const char* source =
        "void f1();"
        "void f2(void);"
        "void f3(char * s, ...);"
        "int main()"
        "{"
        "  f1();"
        "  f2();"
        "  f3(\"\");"
        "  f3(\"\", 1, 2, 3);"
        "}"
        ;

    assert(compile_without_errors(source));
}


void test_compiler_constant_expression()
{
    const char* source =
        "int main()"
        "{"
        "  static_assert('ab' == 'a'*256+'b');\n"
        "  static_assert(sizeof(char)  == 1);\n"
        "  static_assert(true == 1);\n"
        "  static_assert(false == 0);\n"
        "}"
        ;

    assert(compile_without_errors(source));
}


void zerodiv()
{
    const char* source =
        "int main()"
        "{"
        "  int a = 2/0;\n"
        "}"
        ;

    assert(compile_with_errors(source));
}


void function_result_test()
{
    const char* source =
        "int (*(*F1)(void))(int, int*);\n"
        "int (* F2(void) )(int, int*);\n"
        "static_assert(_Generic(F1(), int (*)(int, int*) : 1));\n"
        "static_assert(_Generic(F2(), int (*)(int, int*) : 1));\n"
        ;

    assert(compile_without_errors(source));
}

void type_normalization()
{
    const char* source =
        "char ((a1));\n"
        "char b1;\n"
        "static_assert((typeof(a1)) == (typeof(b1)));\n"
        "\n"
        "char ((a2))[2];\n"
        "char b2[2];\n"
        "static_assert((typeof(a2)) == (typeof(b2)));\n"
        "\n"
        "char ((a3))(int (a));\n"
        "char (b3)(int a);\n"
        "static_assert((typeof(a3)) == (typeof(b3)));\n"
        ;


    assert(compile_without_errors(source));
}

void auto_test()
{
    const char* source =
        "    int main()\n"
        "    {\n"
        "        double const x = 78.9;\n"
        "        double y = 78.9;\n"
        "        auto q = x;\n"
        "        static_assert( (typeof(q)) == (double));\n"
        "        auto const p = &x;\n"
        "        static_assert( (typeof(p)) == (const double  * const));\n"
        "        auto const r = &y;\n"
        "        static_assert( (typeof(r)) == (double  * const));\n"
        "        auto s = \"test\";\n"
        "        static_assert(_is_same(typeof(s), char *));\n"
        "    }\n"
        ;

    assert(compile_without_errors(source));

}

void visit_test_auto_typeof()
{
    const char* source = "auto p2 = (typeof(int[2])*) 0;";

    struct report report = {0};
    char* result = compile_source("-std=c99", source, &report);
    assert(strcmp(result, "int  (* p2)[2] = (int(*)[2]) 0;") == 0);
    free(result);
}

void enum_scope()
{
    const char* source =
        "enum E { A = 1 };\n"
        "int main()\n"
        "{\n"
        "  enum E { B } e2; \n"
        "  static_assert( (typeof(e2)), (enum E) ); \n"
        "}\n";
    assert(compile_without_errors(source));
}

void const_member()
{
    const char* source
        =
        "struct X {\n"
        "  int i;\n"
        "};\n"
        "void f() {\n"
        "  const struct X x = {0};\n"
        "  x.i = 1;\n" //error x.i is constant
        "}\n"
        "";


    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 &&
        report.last_error == C_ASSIGNMENT_OF_READ_ONLY_OBJECT);
}



void register_struct_member()
{
    const char* source
        =
        "struct X {\n"
        "    int i;\n"
        "};\n"
        "\n"
        "int main() {\n"
        "  register struct X x;\n"
        "  int * p = &x.i;\n" //error: address of register variable 'x' requested
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1);
}


void address_of_const()
{
    const char* source =
        "int main()\n"
        "{    \n"
        "    const int i;\n"
        "    static_assert(_is_const(&i));   \n"
        "}";
    assert(compile_without_errors(source));
}

void lvalue_test()
{
    const char* source
        =
        "\n"
        "struct X {\n"
        "    int j;\n"
        "};\n"
        "struct X f() { struct X x; return x; }\n"
        "\n"
        "int main()\n"
        "{\n"
        "    struct X x;\n"
        "    static_assert(_is_lvalue(x));\n"
        "    static_assert(_is_lvalue(&x));\n"
        "    static_assert(_is_lvalue(x.j));\n"
        "    static_assert(_is_lvalue(&x.j));\n"
        "\n"
        "\n"
        "    int i;\n"
        "\n"
        "    static_assert(_is_lvalue(i));\n"
        "    static_assert(_is_lvalue(&i));\n"
        "    static_assert(_is_lvalue(i = 1));\n"
        "\n"
        "    int* p;\n"
        "    static_assert(_is_lvalue(p + 1));\n"
        "    static_assert(_is_lvalue(&p + 1));\n"
        "    static_assert(_is_lvalue(&(p + 1)));\n"
        "\n"
        "    static_assert(!_is_lvalue(~i));\n"
        "    static_assert(!_is_lvalue(1));\n"
        "    static_assert(!_is_lvalue(1.2));\n"
        "    static_assert(!_is_lvalue(\"a\"));\n"
        "    static_assert(!_is_lvalue('a'));\n"
        "    static_assert(!_is_lvalue(sizeof(int)));\n"
        "\n"
        "    static_assert(!_is_lvalue(1 + 1));\n"
        "    static_assert(!_is_lvalue(i + 1));\n"
        "    static_assert(!_is_lvalue(~i));\n"
        "    static_assert(!_is_lvalue(-i));\n"
        "\n"
        "    static_assert(!_is_lvalue(&f()));\n"
        "}\n"
        "\n"
        "";


    assert(compile_without_errors(source));
}

void simple_no_discard_test()
{
    const char* source
        =
        "[[nodiscard]] int destroy();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  destroy();\n"
        "}\n"
        "";

    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.warnings_count == 1 && report.last_warning == W_ATTRIBUTES);
}

void simple_no_discard_test2()
{
    const char* source
        =
        "[[nodiscard]] int destroy();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  int i;\n"
        "  i = destroy();\n"
        "}\n"
        "";


    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.warnings_count == 0 && report.error_count == 0);
}


void address_of_register()
{
    const char* source
        =
        "struct X\n"
        "{\n"
        "    int i;\n"
        "};\n"
        "\n"
        "void f()\n"
        "{\n"
        "  register struct X x;\n"
        "  &x;\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_ADDRESS_OF_REGISTER);
}

void return_address_of_local()
{
    const char* source
        =
        "struct X\n"
        "{\n"
        "    int i;\n"
        "};\n"
        "\n"
        "int* f()\n"
        "{\n"
        "  struct X x;\n"
        "  return &x.i;\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.warnings_count == 1 && report.last_warning == W_RETURN_LOCAL_ADDR);
}


void assignment_of_read_only_object()
{
    const char* source
        =
        "struct X\n"
        "{\n"
        "    int i;\n"
        "};\n"
        "\n"
        "int* f()\n"
        "{\n"
        "  const struct X * p;\n"
        "  p->i = 1;\n"
        "}\n";

    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_ASSIGNMENT_OF_READ_ONLY_OBJECT);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////     OWNER /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////



void simple_move()
{
    const char* source
        =
        "char * _Owner f() {\n"
        "    char * _Owner p = 0;\n"
        "    return p; /*implicit move*/\n"
        "}";
    assert(compile_without_errors(source));
}


void simple_move_error()
{
    const char* source
        =
        "char * f() {\n"
        "    char * _Owner p = 0;\n"
        "    return p; \n"
        "}";

    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 &&
        report.last_error == C_MOVE_ASSIGNMENT_OF_NON_OWNER);
}

void parameter_view()
{
    const char* source
        =
        "\n"
        "struct X { char  * _Owner owner_variable;   };\n"
        "char * f(struct X *parameter) \n"
        "{\n"
        "    return parameter->owner_variable;\n"  //ok to move from parameter
        "}\n";

    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);
}

void move_from_extern()
{
    const char* source
        =
        "struct X { char  * _Owner owner_variable;   };\n"
        "struct X global;\n"
        "char * f() \n"
        "{\n"
        "    return global.owner_variable;\n" /*makes a view*/
        "}\n";

    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);
}

void owner_type_test()
{
    const char* source
        =
        "\n"
        "struct Y { \n"
        "    char  * _Owner owner_variable;   \n"
        "    char  * non_owner_variable;   \n"
        "};\n"
        "\n"
        "struct X { \n"
        "    char  * _Owner owner_variable;   \n"
        "    char  * non_owner_variable;   \n"
        "    struct Y y1;\n"
        "    _View struct Y y2;\n"
        "};\n"
        "\n"
        "void f()\n"
        "{\n"
        "    struct X x;\n"
        "    \n"
        "    static_assert(_is_owner(typeof(x)));\n"
        "    static_assert(_is_owner(typeof(x.owner_variable)));\n"
        "    static_assert(!_is_owner(typeof(x.non_owner_variable)));\n"
        "    static_assert(_is_owner(struct X));\n"
        "    static_assert(_is_owner(typeof(x.y1)));\n"
        "    static_assert(!_is_owner(typeof(x.y2)));\n"
        "    \n"
        "    static_assert(_is_owner(typeof(x.y1.owner_variable)));\n"
        "    static_assert(!_is_owner(typeof(x.y1.non_owner_variable)));\n"
        "\n"
        "    static_assert(!_is_owner(typeof(x.y2.owner_variable)));\n"
        "    static_assert(!_is_owner(typeof(x.y2.non_owner_variable)));\n"
        "\n"
        "    _View struct X x2;\n"
        "    static_assert(!_is_owner(typeof(x2)));\n"
        "    static_assert(!_is_owner(typeof(x2.owner_variable)));\n"
        "    static_assert(!_is_owner(typeof(x2.non_owner_variable)));\n"
        "\n"
        "    _Owner char * p;\n"
        "    static_assert(!_is_owner(typeof(p)));\n"
        "    static_assert(_is_owner(typeof(*p)));    \n"
        "}\n";

    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);
}

void correct_move_assigment()
{
    const char* source
        =
        "\n"
        "struct Y { \n"
        "    int i;\n"
        "};\n"
        "\n"
        "struct X { \n"
        "    char * _Owner name;\n"
        "};\n"
        "\n"
        "int main()\n"
        "{\n"
        "    struct Y y1;\n"
        "    struct Y y2;\n"
        "    y1 = y2; //ok\n"
        "\n"
        "    struct X x1;\n"
        "    struct X x2;\n"
        "    x1 = _Move x2; //ok\n"
        "\n"
        "}";
    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);
}

void error_on_non_owner_move()
{
    const char* source
        =
        "\n"
        "struct Y { \n"
        "    int i;\n"
        "};\n"
        "\n"
        "struct X { \n"
        "    char * _Owner name;\n"
        "};\n"
        "\n"
        "int main()\n"
        "{\n"
        "    struct Y y1;\n"
        "    struct Y y2;\n"
        "    y1 = _Move y2;" //error
        "}\n";
    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_NON_OWNER_MOVE);
}

void move_required_on_assignment_owner()
{
    const char* source
        =
        "\n"
        "struct X { \n"
        "    char * _Owner name;\n"
        "};\n"
        "\n"
        "int main()\n"
        "{\n"
        "    struct X x1;\n"
        "    struct X x2;    \n"
        "    x1 = x2;\n" /*not ok*/
        "}\n";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.warnings_count == 1 && report.last_warning == W_EXPLICIT_MOVE);
}

void no_explicit_move_required()
{
    const char* source
        =
        "char * _Owner create();\n"
        "void f(char * _Owner p);\n"
        "\n"
        "int main()\n"
        "{\n"
        "    f(create());\n"
        "}\n"
        "\n"
        "";
    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);

}

void no_explicit_move_with_function_result()
{
    const char* source
        =
        "void destroy(char* _Owner x);\n"
        "char   * _Owner  get();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  destroy(get());\n"
        "}\n";

    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0 && report.warnings_count == 0);
}


void cannot_ignore_owner_result()
{
    const char* source
        =
        "struct X {\n"
        "  char * _Owner name;\n"
        "};\n"
        "\n"
        "struct X f();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  f();\n"
        "}\n";

    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.warnings_count == 1);
}


void can_ignore_owner_result()
{
    const char* source
        =
        "struct X {\n"
        "  char * _Owner name;\n"
        "};\n"
        "\n"
        "_View struct X f();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  f();\n"
        "}\n";

    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0 && report.warnings_count == 0);
}

void move_not_necessary_on_return()
{
    const char* source
        =
        "struct X {\n"
        "  char * _Owner name;\n"
        "};\n"
        "\n"
        "struct X f();\n"
        "struct X f2()\n"
        "{\n"
        "    return f();\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0 && report.warnings_count == 0);
}

void explicit_move_not_required()
{
    const char* source
        =
        "#define NULL ((void*)0)\n"
        "\n"
        "int main()\n"
        "{\n"
        "    const char * _Owner s;\n"
        "    s = NULL;    \n"
        "    s = 0;    \n"
        "    s = nullptr;    \n"
        "}\n"
        ;
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0 && report.warnings_count == 0);
}

void error_using_temporary_owner()
{
    const char* source
        =
        "\n"
        "void F(int i);\n"
        "_Owner int make();\n"
        "int main()\n"
        "{\n"
        "    F(make());\n"
        "}";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_USING_TEMPORARY_OWNER);

}

void passing_view_to_owner()
{
    const char* source
        =
        "void destroy(_Implicit _Owner int i);\n"
        "\n"
        "int main()\n"
        "{\n"
        "  _Owner int i = 0;\n"
        "  int v = i;\n"
        "  destroy(v);\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_MOVE_ASSIGNMENT_OF_NON_OWNER);
}

void obj_owner_cannot_be_used_in_non_pointer()
{
    const char* source
        =
        "void f() {\n"
        "    _Obj_owner int i;\n"
        "}\n"
        ;
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER);

}

void null_ptr_at_end_of_scope()
{
    const char* source
        =
        "void f() {\n"
        "    _Owner int * p = 0;\n"
        "}\n"
        " ";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);
}

void pointer_must_be_deleted()
{
    const char* source
        =
        "\n"
        "int* _Owner  get();\n"
        "\n"
        "void f() {\n"
        "    int * _Owner p = 0;\n"
        "    p = _Move get();\n"
        "}\n"
        " ";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_DESTRUCTOR_MUST_BE_CALLED_BEFORE_END_OF_SCOPE);
}

void basic_pointer_check()
{
    const char* source
        =
        "\n"
        "int* _Owner  get();\n"
        "void dtor(int* _Owner p);\n"
        "\n"
        "void f(int a)\n"
        "{\n"
        "    int* _Owner p = 0;\n"
        "    p = _Move get();    \n"
        "    dtor(_Move p);    \n"
        "}\n"
        "";

    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);
}


void struct_member_missing_free()
{
    const char* source
        =
        "\n"
        "char * _Owner strdup(const char* s);\n"
        "void free(_Implicit void* _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "};\n"
        "\n"
        "void f(int a)\n"
        "{\n"
        "    struct X x = {0};\n"
        "    x.text = _Move strdup(\"a\");\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_DESTRUCTOR_MUST_BE_CALLED_BEFORE_END_OF_SCOPE);
    ////TODO return ROOT object!

}


void struct_member_free()
{
    const char* source
        =
        "\n"
        "char * _Owner strdup(const char* s);\n"
        "void free(_Implicit void* _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "};\n"
        "\n"
        "void f(int a)\n"
        "{\n"
        "    struct X x = {0};\n"
        "    x.text = _Move strdup(\"a\");\n"
        "    free(x.text);\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);

}

void move_inside_if()
{
    const char* source
        =
        "void free(_Implicit void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "void f(int c) \n"
        "{\n"
        "    int * _Owner p = malloc(sizeof (int));    \n"
        "    if (c) {\n"
        "      free(p);\n"
        "    }\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1);
}

void goto_same_scope()
{
    const char* source
        =
        "void free(_Implicit void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "void f(int condition) \n"
        "{\n"
        "    int * _Owner p = malloc(sizeof(int));\n"
        "  \n"
        "    if (condition)\n"
        "       goto end;\n"
        "  end:\n"
        "    free(p);\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);
}

void jump_labels()
{
    const char* source
        =
        "void free(_Implicit void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "void f(int condition)\n"
        "{\n"
        "    int* _Owner p = malloc(sizeof(int));\n"
        "\n"
        "    if (condition)\n"
        "        goto end;\n"
        "\n"
        "    free(p);\n"
        "end:\n"
        "\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_DESTRUCTOR_MUST_BE_CALLED_BEFORE_END_OF_SCOPE);
}

void owner_if_pattern_1()
{
    const char* source
        =
        "\n"
        "void free(_Implicit void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "int main()\n"
        "{\n"
        "    int* _Owner p = malloc(sizeof(int));\n"
        "    if (p)\n"
        "    {\n"
        "       free(p);     \n"
        "    }\n"
        "}\n"
        "\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);
}
void owner_if_pattern_2()
{
    const char* source
        =
        "\n"
        "void free(_Implicit void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "int main()\n"
        "{\n"
        "    int* _Owner p = malloc(sizeof(int));\n"
        "    if (p != 0)\n"
        "    {\n"
        "       free(p);     \n"
        "    }\n"
        "}\n"
        "\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0);
}

void missing_destructor()
{
    const char* source
        =
        "struct X {\n"
        "  _Owner i;\n"
        "};\n"
        "void f() {\n"
        "  const struct X x = {0};\n"
        "}\n"
        "";


    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_DESTRUCTOR_MUST_BE_CALLED_BEFORE_END_OF_SCOPE);

}
void no_warning()
{
    const char* source
        =
        "void free(_Implicit void * _Owner p);\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "};\n"
        "void x_delete(_Implicit struct X * _Owner p)\n"
        "{\n"
        "    if (p)\n"
        "    {\n"
        "      free(p->text);\n"
        "      free(p);\n"
        "    }\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0 && report.warnings_count == 0);
}
void moved_if_not_null()
{
    const char* source
        =
        "void * _Owner malloc(int i);\n"
        "void free(_Implicit void * _Owner p);\n"
        "\n"
        "struct X { int i; };\n"
        "struct Y { struct X * _Owner p; };\n"
        "\n"
        "int main() {\n"
        "   struct Y y;\n"
        "   struct X * _Owner p = malloc(sizeof(struct X));\n"
        "   if (p){\n"
        "     y.p = _Move p;\n"
        "   }\n"
        "  free(y.p);\n"
        "}\n"
        "\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 0 && report.warnings_count == 0);
}
void void_ptr_conversion()
{
    const char* source
        =
        "\n"
        "void free(_Implicit void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "struct X { char * _Owner name; };\n"
        "\n"
        "void * _Owner f1(){\n"
        "  struct X * _Owner p = malloc(sizeof (struct X));\n"
        "  return p;\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count != 0 || report.warnings_count != 0);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////     OWNER /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


#endif




