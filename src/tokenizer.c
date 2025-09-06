/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

//#pragma safety enable

/*

   1 - The initial input is a string.
   2 - This string is transformed into a linked list of tokens

   ┌───┐   ┌───┐   ┌───┐   ┌───┐
   │   ├──►│   ├──►│   ├──►│   │──► NULL
   └───┘   └───┘   └───┘   └───┘

   The list is then expanded using includes and macros through a preprocessor.


   ┌───┐                  ┌───┐   ┌───┐   ┌───┐
   │   ├──────┐           │   ├──►│x  ├──►│x  │ ──► NULL   Level 0
   └───┘      │           └───┘   └───┘   └───┘
            ┌─▼─┐   ┌───┐   ▲
            │   ├───┤   ├───┘  (includes)                  Level 1
            └───┘   └───┘


    Each item in the list has additional properties:

    level         :  An integer indicating the level of inclusion.

    bmacroexpanded: A boolean indicating whether the token was generated
                    from macro expansion.

    bfinal        : A boolean indicating whether the token is the
                    final, the one seen by the parser.

*/

#include "ownership.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <time.h>
#include "console.h"
#include "hashmap.h"
#include "osstream.h"
#include "fs.h"

#include "error.h"
#include "pre_expressions.h"
#include "tokenizer.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


// Includes tokens that are not necessary for compilation at second level of includes
enum { INCLUDE_ALL = 1 };

///////////////////////////////////////////////////////////////////////////////
void naming_convention_macro(struct preprocessor_ctx* ctx, struct token* token);
///////////////////////////////////////////////////////////////////////////////

static bool is_builtin_macro(const char* name);


struct macro_parameter
{
    const char* _Owner name;
    struct macro_parameter* _Owner _Opt next;

    /*
      https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3457.htm#number-of-expansions
      For each such parameter this expansion is performed exactly once
      (this list and flag are clean and reused when performing argument expansion)
    */
    struct token_list expanded_list;
    bool already_expanded;
};



struct macro
{
    const struct token* p_name_token;
    const char* _Owner name;
    struct token_list replacement_list; /*copy*/
    struct macro_parameter* _Owner _Opt parameters;
    bool is_function;
    int usage;


    bool def_macro;
};


void macro_delete(struct macro* _Owner _Opt macro);
bool macro_is_same(const struct macro* macro_a, const struct macro* macro_b);


void include_dir_list_destroy(_Dtor struct include_dir_list* list)
{
    struct include_dir* _Owner _Opt p = list->head;
    while (p)
    {
        struct include_dir* _Owner _Opt next = p->next;
        free((void* _Owner)p->path);
        free(p);
        p = next;
    }
}

void preprocessor_ctx_destroy(_Dtor struct preprocessor_ctx* p)
{
    hashmap_destroy(&p->macros);
    include_dir_list_destroy(&p->include_dir);
    hashmap_destroy(&p->pragma_once_map);
    token_list_destroy(&p->input_list);
}

struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level);

static void tokenizer_set_error(struct tokenizer_ctx* ctx, struct stream* stream, const char* fmt, ...)
{
    ctx->n_errors++;

    char buffer[200] = { 0 };

#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wnullable-to-non-nullable"
#pragma CAKE diagnostic ignored "-Wanalyzer-null-dereference"



    va_list args = { 0 };
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

#pragma CAKE diagnostic pop

    print_position(stream->path, stream->line, stream->col, ctx->options.visual_studio_ouput_format);
    if (ctx->options.visual_studio_ouput_format)
    {
        printf("error: "  "%s\n", buffer);
    }
    else
    {
        printf(LIGHTRED "error: " WHITE "%s\n", buffer);
    }
}


static void tokenizer_set_warning(struct tokenizer_ctx* ctx, struct stream* stream, const char* fmt, ...)
{
    ctx->n_warnings++;


    char buffer[200] = { 0 };

#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wnullable-to-non-nullable"
#pragma CAKE diagnostic ignored "-Wanalyzer-null-dereference"


    va_list args = { 0 };
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

#pragma CAKE diagnostic pop

    print_position(stream->path, stream->line, stream->col, ctx->options.visual_studio_ouput_format);
    if (ctx->options.visual_studio_ouput_format)
    {
        printf("warning: " "%s\n", buffer);
    }
    else
    {
        printf(LIGHTMAGENTA "warning: " WHITE "%s\n", buffer);
    }

}


void pre_unexpected_end_of_file(struct token* _Opt p_token, struct preprocessor_ctx* ctx)
{
    preprocessor_diagnostic(C_ERROR_UNEXPECTED_TOKEN,
        ctx,
        p_token,
        "unexpected end of file");
}

bool preprocessor_diagnostic(enum diagnostic_id w, struct preprocessor_ctx* ctx, const struct token* _Opt p_token_opt, const char* fmt, ...)
{
    struct marker marker = { 0 };

    if (p_token_opt == NULL) return false;

    marker.file = p_token_opt->token_origin->lexeme;
    marker.line = p_token_opt->line;
    marker.start_col = p_token_opt->col;
    marker.end_col = p_token_opt->col;
    marker.p_token_caret = p_token_opt;

    /*warnings inside headers are ignored*/
    const bool included_file_location = p_token_opt->level > 0;

    bool is_error = false;
    bool is_warning = false;
    bool is_note = false;

    if (w > W_NOTE)
    {
        is_error = true;
    }
    else
    {
        is_error =
            (ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors & (1ULL << w)) != 0;

        is_warning =
            (ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings & (1ULL << w)) != 0;

        is_note =
            ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes & (1ULL << w)) != 0);
    }

    if (is_error)
    {
        ctx->n_errors++;
    }
    else if (is_warning)
    {
        ctx->n_warnings++;
    }
    else if (is_note)
    {

    }
    else
    {
        return false;
    }

    if (!is_error && included_file_location)
    {
        //notes are warning are not printed in included files
        return false;
    }

    print_position(marker.file, marker.line, marker.start_col, ctx->options.visual_studio_ouput_format);

    char buffer[200] = { 0 };

#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wnullable-to-non-nullable"
#pragma CAKE diagnostic ignored "-Wanalyzer-null-dereference"

    va_list args = { 0 };

    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
#pragma CAKE diagnostic pop

    if (ctx->options.visual_studio_ouput_format)
    {
        if (is_warning)
            printf("warning: " "%s\n", buffer);
        else if (is_error)
            printf("warning: " "%s\n", buffer);
        else if (is_note)
            printf("note: " "%s\n", buffer);

        print_line_and_token(&marker, ctx->options.visual_studio_ouput_format);
    }
    else
    {
        if (is_error)
            printf(LIGHTRED "error: " WHITE "%s\n", buffer);
        else if (is_warning)
            printf(LIGHTMAGENTA "warning: " WHITE "%s\n", buffer);
        else if (is_note)
            printf(LIGHTCYAN "note: " WHITE "%s\n", buffer);

        print_line_and_token(&marker, ctx->options.visual_studio_ouput_format);

    }

    return true;
}

struct include_dir* _Opt include_dir_add(struct include_dir_list* list, const char* path)
{
    try
    {
        struct include_dir* _Owner _Opt p_new_include_dir = calloc(1, sizeof * p_new_include_dir);
        if (p_new_include_dir == NULL)
            throw;

        int len = strlen(path);
        if (path[len - 1] == '\\')
        {
            //windows path format ending with \ .
            const char* _Owner _Opt temp = strdup(path);
            if (temp == NULL)
            {
                free(p_new_include_dir);
                throw;
            }
            p_new_include_dir->path = temp;
        }
        else if (path[len - 1] != '/')
        {
            /*
              not ending with \, we add it
            */
            const char* _Owner _Opt temp = calloc(len + 2, sizeof(char));
            if (temp == NULL)
            {
                free(p_new_include_dir);
                throw;
            }

            p_new_include_dir->path = temp;
            snprintf((char*)p_new_include_dir->path, len + 2, "%s/", path);
        }
        else
        {
            const char* _Owner _Opt temp = strdup(path);
            if (temp == NULL)
            {
                free(p_new_include_dir);
                throw;
            }
            p_new_include_dir->path = temp;
        }

        if (list->head == NULL)
        {
            list->head = p_new_include_dir;
            list->tail = p_new_include_dir;
        }
        else
        {
            assert(list->tail != NULL);
            assert(list->tail->next == NULL);
            list->tail->next = p_new_include_dir;
            list->tail = p_new_include_dir;
        }
        return list->tail;
    }
    catch
    {

    }
    return NULL;
}

/*
  We must ensure we have always the same representation for path when searching and inserting
  at pragma once map
*/

static void pragma_once_add(struct preprocessor_ctx* ctx, const char* path)
{
    //FAILING ON EMSCRIPT
    //assert(path_is_absolute(path));
    //assert(path_is_normalized(path));
    struct hash_item_set item = { 0 };
    item.number = 1;
    hashmap_set(&ctx->pragma_once_map, path, &item /*in out*/);
    hash_item_set_destroy(&item);
}

static bool pragma_once_already_included(struct preprocessor_ctx* ctx, const char* path)
{
    //FAILING ON EMSCRIPT
    //assert(path_is_absolute(path));
    //assert(path_is_normalized(path));
    return hashmap_find(&ctx->pragma_once_map, path) != NULL;
}

const char* _Owner _Opt  find_and_read_include_file(struct preprocessor_ctx* ctx,
    const char* path, /*as in include*/
    const char* current_file_dir, /*this is the dir of the file that includes*/
    bool is_angle_bracket_form,
    bool* p_already_included, /*out file already included pragma once*/
    char full_path_out[], /*this is the final full path of the file*/
    int full_path_out_size,
   bool include_next)
{
    char newpath[200] = { 0 };
    full_path_out[0] = '\0';

    if (path_is_absolute(path))
    {
        snprintf(newpath, sizeof newpath, "%s", path);
        path_normalize(newpath);
        if (pragma_once_already_included(ctx, newpath))
        {
            *p_already_included = true;
            return NULL;
        }

        char* _Owner _Opt content = read_file(newpath, true);
        if (content != NULL)
        {
            snprintf(full_path_out, full_path_out_size, "%s", path);
            return content;
        }
        return NULL;
    }


    char* _Owner _Opt content = NULL;

    if (!is_angle_bracket_form)
    {
        /*
          For the angle-bracket form #include <file>, the preprocessor’s default
          behavior is to look only in the standard system directories.
        */

        //https://gcc.gnu.org/onlinedocs/cpp/Search-Path.html
        //https://learn.microsoft.com/en-us/cpp/preprocessor/hash-include-directive-c-cpp?view=msvc-170

        snprintf(newpath, sizeof newpath, "%s/%s", current_file_dir, path);

#ifdef __EMSCRIPTEN__
        /*realpath returns empty on emscriptem*/
        snprintf(full_path_out, full_path_out_size, "%s", newpath);
#else
        if (!realpath(newpath, full_path_out))
            full_path_out[0] = '\0';
#endif

        path_normalize(full_path_out);


        if (pragma_once_already_included(ctx, full_path_out))
        {
            *p_already_included = true;
            return NULL;
        }

        if (full_path_out[0] != '\0')
        {
            content = read_file(full_path_out, true);
        }
        if (content != NULL)
            return content;
    }

    /*
       Searching on include directories
    */
    struct include_dir* _Opt current = ctx->include_dir.head;
    while (current)
    {
        int len = strlen(current->path);
        if (current->path[len - 1] == '/')
        {
            snprintf(full_path_out, full_path_out_size, "%s%s", current->path, path);
        }
        else
        {
            snprintf(full_path_out, full_path_out_size, "%s/%s", current->path, path);
        }

        path_normalize(full_path_out);
        if (pragma_once_already_included(ctx, full_path_out))
        {
            *p_already_included = true;
            return NULL;
        }

        content = read_file(full_path_out, true);
        if (content != NULL)
        {
            if (include_next)
            {
                free(content);
                content = NULL;
                include_next = false;
            }
            else
                return content;
        }
        current = current->next;
    }
    full_path_out[0] = '\0';
    return NULL;
}

/*used to check recursion*/
struct macro_expanded
{
    const char* name;
    struct macro_expanded* _Opt p_previous;
};


void add_macro(struct preprocessor_ctx* ctx, const char* name)
{
    try
    {
        char* _Owner _Opt name_local = strdup(name);
        if (name_local == NULL)
        {
            throw;
        }

        struct macro* _Owner _Opt macro = calloc(1, sizeof * macro);
        if (macro == NULL)
        {
            free(name_local);
            throw;
        }

        macro->name = name_local;
        struct hash_item_set item = { .p_macro = macro };
        hashmap_set(&ctx->macros, name, &item);
        hash_item_set_destroy(&item);
    }
    catch
    {
    }
}

struct macro_argument
{
    /*the parameter this argument is associated with*/
    struct macro_parameter* _Opt macro_parameter;
    struct token_list tokens;
    struct macro_argument* _Owner _Opt next; /*linked list*/
};

void macro_argument_delete(struct macro_argument* _Owner _Opt p);



struct token_list copy_argument_list_tokens(struct token_list* list)
{
    // Makes a copy of the tokens, trimming the beginning and end
    // Any space, comments, etc., will become a single space
    struct token_list r = { 0 };
    struct token* _Opt current = list->head; /*null is fine*/

    // skip all leading white spaces
    while (current &&
        (token_is_blank(current) ||
            current->type == TK_NEWLINE))
    {
        current = current->next;
    }

    // Removes leading space flag if present
    bool is_first = true;

    for (; current;)
    {
        if (current && (token_is_blank(current) ||
            current->type == TK_NEWLINE))
        {
            if (current == list->tail)
                break;

            current = current->next;
            continue;
        }
        struct token* token = token_list_clone_and_add(&r, current);
        if (token->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
        {
            token->flags = token->flags & ~TK_FLAG_HAS_NEWLINE_BEFORE;
            token->flags |= TK_FLAG_HAS_SPACE_BEFORE;
        }
        if (is_first)
        {
            token->flags = token->flags & ~TK_FLAG_HAS_SPACE_BEFORE;
            token->flags = token->flags & ~TK_FLAG_HAS_NEWLINE_BEFORE;
            is_first = false;
        }
        remove_line_continuation(token->lexeme);

        if (current == list->tail)
            break;

        current = current->next;
    }

    return r;
}

void macro_argument_delete(struct macro_argument* _Owner _Opt p)
{
    if (p)
    {
        assert(p->next == NULL);
        token_list_destroy(&p->tokens);
        free(p);
    }
}

struct token_list copy_argument_list(struct macro_argument* p_macro_argument)
{
    try
    {
        struct token_list list = copy_argument_list_tokens(&p_macro_argument->tokens);
        if (list.head == NULL)
        {
            /*it's never empty...if puts a TK PLACEMARKER*/
            struct token* _Owner _Opt p_new_token = calloc(1, sizeof * p_new_token);
            if (p_new_token == NULL) throw;

            char* _Owner _Opt temp = strdup("");
            if (temp == NULL)
            {
                token_delete(p_new_token);
                throw;
            }

            p_new_token->lexeme = temp;
            p_new_token->type = TK_PLACEMARKER;
            token_list_add(&list, p_new_token);
        }
        return list;
    }
    catch
    {
    }

    struct token_list empty = { 0 };
    return empty;
}


struct macro_argument_list
{
    struct token_list tokens;
    struct macro_argument* _Owner _Opt head;
    struct macro_argument* _Opt tail;
};

void macro_argument_list_destroy(_Dtor struct macro_argument_list* list)
{
    token_list_destroy(&list->tokens);
    struct macro_argument* _Owner _Opt p = list->head;
    while (p)
    {
        struct macro_argument* _Owner _Opt next = p->next;
        p->next = NULL;
        macro_argument_delete(p);
        p = next;
    }
}

void print_macro_arguments(struct macro_argument_list* arguments)
{
    struct macro_argument* _Opt p_argument = arguments->head;
    while (p_argument)
    {
        if (p_argument->macro_parameter)
            printf("%s:", p_argument->macro_parameter->name);

        print_list(&p_argument->tokens);
        p_argument = p_argument->next;
    }
}

struct macro_argument* _Opt find_macro_argument_by_name(struct macro_argument_list* parameters, const char* name)
{
    /*
     * The arguments are collected in the macro expansion and each one (except ...)
     * is associated with one of the macro parameters.
     */
    struct macro_argument* _Opt p_macro_argument = parameters->head;
    while (p_macro_argument)
    {
        if (strcmp(p_macro_argument->macro_parameter->name, name) == 0)
        {
            return p_macro_argument;
        }
        p_macro_argument = p_macro_argument->next;
    }
    return NULL;
}

void argument_list_add(struct macro_argument_list* list, struct macro_argument* _Owner pnew)
{
    assert(pnew->next == NULL);
    if (list->head == NULL)
    {
        list->head = pnew;
        assert(list->tail == NULL);
        list->tail = pnew;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = pnew;
        list->tail = pnew;
    }
}

void print_macro(struct macro* macro)
{
    printf("%s", macro->name);
    if (macro->is_function)
        printf("(");
    struct macro_parameter* _Opt parameter = macro->parameters;
    while (parameter)
    {
        if (macro->parameters != parameter)
            printf(",");
        printf("%s", parameter->name);
        parameter = parameter->next;
    }
    if (macro->is_function)
        printf(") ");
    print_list(&macro->replacement_list);
}

void macro_parameters_delete(struct macro_parameter* _Owner _Opt parameters)
{
    struct macro_parameter* _Owner _Opt p = parameters;
    while (p)
    {
        struct macro_parameter* _Owner _Opt p_next = p->next;
        free((void* _Owner)p->name);
        free(p);
        p = p_next;
    }
}

bool macro_is_same(const struct macro* macro_a, const struct macro* macro_b)
{
    if (macro_a->is_function != macro_b->is_function)
        return false;

    if (strcmp(macro_a->name, macro_b->name) != 0)
        return false;

    if (!token_list_is_equal(&macro_a->replacement_list, &macro_b->replacement_list) != 0)
        return false;

    const struct macro_parameter* p_a = macro_a->parameters;
    const struct macro_parameter* p_b = macro_b->parameters;
    while (p_a && p_b)
    {
        if (strcmp(p_a->name, p_b->name) != 0)
            return false;

        p_a = p_a->next;
        p_b = p_b->next;
    }

    return p_a == NULL && p_b == NULL;
}
void macro_delete(struct macro* _Owner _Opt macro)
{
    if (macro)
    {
        token_list_destroy(&macro->replacement_list);

        struct macro_parameter* _Owner _Opt p_macro_parameter = macro->parameters;
        while (p_macro_parameter)
        {
            struct macro_parameter* _Owner _Opt p_next = p_macro_parameter->next;
            free((void* _Owner)p_macro_parameter->name);
            token_list_destroy(&p_macro_parameter->expanded_list);
            free(p_macro_parameter);
            p_macro_parameter = p_next;
        }

        free((void* _Owner) macro->name);
        free(macro);
    }
}

struct macro* _Opt find_macro(struct preprocessor_ctx* ctx, const char* name)
{
    struct map_entry* _Opt p_entry = hashmap_find(&ctx->macros, name);
    if (p_entry == NULL)
        return NULL;

    return p_entry->data.p_macro;
}

void stream_print_line(struct stream* stream)
{
    const char* p = stream->current;
    while ((p - 1) >= stream->source &&
        *(p - 1) != '\n')
    {
        p--;
    }
    while (*p && *(p + 1) != '\n')
    {
        printf("%c", *p);
        p++;
    }
    printf("\n");
    for (int i = 0; i < stream->col - 1; i++)
        printf(" ");
    printf("^\n");
}

void stream_match(struct stream* stream)
{
    if (stream->current[0] == '\n')
    {
        stream->line++;
        stream->col = 1;
    }
    else
    {
        stream->col++;
    }

    if (stream->current[0] == '\0')
        return;
    stream->current++;

    while (stream->current[0] == '\\' &&
             (stream->current[1] == '\n' ||
                 (stream->current[1] == '\r' && stream->current[2] == '\n')
                 ))
    {
        /*
            2. Each instance of a backslash character (\) immediately followed by a new-line character is
            deleted, splicing physical source lines to form logical source lines. Only the last backslash on
            any physical source line shall be eligible for being part of such a splice. A source file that is
            not empty shall end in a new-line character, which shall not be immediately preceded by a
            backslash character before any such splicing takes place.
        */
        if (stream->current[1] == '\r' && stream->current[2] == '\n')
        {
            stream->current++; // 
            stream->current++; // r 
            stream->current++; // n
        }
        else
        {
            stream->current++;
            stream->current++;  // n
        }

        stream->line++;
        stream->col = 1;

        stream->line_continuation_count++;
    }

}

void print_line(struct token* p)
{
    printf("%s\n", p->token_origin->lexeme);
    struct token* _Opt prev = p;
    while (prev->prev && prev->prev->type != TK_NEWLINE)
    {
        prev = prev->prev;
    }
    struct token* _Opt next = prev;
    while (next && next->type != TK_NEWLINE)
    {
        printf("%s", next->lexeme);
        next = next->next;
    }
    printf("\n");
}

int is_nondigit(const struct stream* p)
{
    /*
    nondigit: one of
     _ a b c d e f g h i j k l m
     n o p q r s t u v w x y z
     A B C D E F G H I J K L M
     N O P Q R S T U V W X Y Z
    */
    return (p->current[0] >= 'a' && p->current[0] <= 'z') ||
        (p->current[0] >= 'A' && p->current[0] <= 'Z') ||
        (p->current[0] == '_');
}


enum token_type is_punctuator(struct stream* stream)
{
    enum token_type type = TK_NONE;
    /*
     punctuator: one of
      [ ] ( ) { } . ->
      ++ -- & * + - ~ !
      / % << >> < > <= >= == != ^ | && ||
      ? : :: ; ...
      = *= /= %= += -= <<= >>= &= ^= |=
      , # ##
      <: :> <% %> %: %:%:
    */
    switch (stream->current[0])
    {
    case '[':
        type = '[';
        stream_match(stream);
        break;
    case ']':
        type = ']';
        stream_match(stream);
        break;
    case '(':
        type = '(';
        stream_match(stream);
        break;
    case ')':
        type = ')';
        stream_match(stream);
        break;
    case '{':
        type = '{';
        stream_match(stream);
        break;
    case '}':
        type = '}';
        stream_match(stream);
        break;
    case ';':
        type = ';';
        stream_match(stream);
        break;
    case ',':
        type = ',';
        stream_match(stream);
        break;
    case '!':
        type = '!';
        stream_match(stream);
        if (stream->current[0] == '=')
        {
            type = '!=';
            stream_match(stream);
        }
        break;
    case ':':
        type = ':';
        stream_match(stream);
        if (stream->current[0] == ':')
        {
            type = '::';
            stream_match(stream);
        }
        break;
    case '~':
        type = '~';
        stream_match(stream);
        break;
    case '?':
        type = '?';
        stream_match(stream);
        break;
    case '/':
        type = '/';
        stream_match(stream);
        if (stream->current[0] == '=')
        {
            type = '/=';
            stream_match(stream);
        }
        break;
    case '*':
        type = '*';
        stream_match(stream);
        if (stream->current[0] == '=')
        {
            type = '*=';
            stream_match(stream);
        }
        break;
    case '%':
        type = '%';
        stream_match(stream);
        if (stream->current[0] == '=')
        {
            type = '%=';
            stream_match(stream);
        }
        break;
    case '-':
        type = '-';
        stream_match(stream);
        if (stream->current[0] == '>')
        {
            type = '->';
            stream_match(stream);
        }
        else if (stream->current[0] == '-')
        {
            type = '--';
            stream_match(stream);
        }
        else if (stream->current[0] == '=')
        {
            type = '-=';
            stream_match(stream);
        }
        break;
    case '|':
        type = '|';
        stream_match(stream);
        if (stream->current[0] == '|')
        {
            type = '||';
            stream_match(stream);
        }
        else if (stream->current[0] == '=')
        {
            type = '|=';
            stream_match(stream);
        }
        break;
    case '+':
        type = '+';
        stream_match(stream);
        if (stream->current[0] == '+')
        {
            type = '++';
            stream_match(stream);
        }
        else if (stream->current[0] == '=')
        {
            type = '+=';
            stream_match(stream);
        }
        break;
    case '=':
        type = '=';
        stream_match(stream);
        if (stream->current[0] == '=')
        {
            type = '==';
            stream_match(stream);
        }
        break;
    case '^':
        type = '^';
        stream_match(stream);
        if (stream->current[0] == '=')
        {
            type = '^=';
            stream_match(stream);
        }
        break;
    case '&':
        type = '&';
        stream_match(stream);
        if (stream->current[0] == '&')
        {
            type = '&&';
            stream_match(stream);
        }
        else if (stream->current[0] == '=')
        {
            type = '&=';
            stream_match(stream);
        }
        break;
    case '>':
        type = '>';
        stream_match(stream);
        if (stream->current[0] == '>')
        {
            type = '>>';
            stream_match(stream);
            if (stream->current[0] == '=')
            {
                type = '>>=';
                stream_match(stream);
            }
        }
        else if (stream->current[0] == '=')
        {
            type = '>=';
            stream_match(stream);
        }

        break;
    case '<':
        type = '<';
        stream_match(stream);
        if (stream->current[0] == '<')
        {
            type = '<<';
            stream_match(stream);
            if (stream->current[0] == '=')
            {
                type = '<<=';
                stream_match(stream);
            }
        }
        else if (stream->current[0] == '=')
        {
            type = '<=';
            stream_match(stream);
        }
        break;
    case '#':
        type = '#';
        stream_match(stream);
        if (stream->current[0] == '#')
        {
            type = '##';
            stream_match(stream);
        }
        break;
    case '.':
        type = '.';
        stream_match(stream);
        if (stream->current[0] == '.' && stream->current[1] == '.')
        {
            type = '...';
            stream_match(stream);
            stream_match(stream);
        }
        break;
    }
    return type;
}


struct token* _Owner _Opt new_token(const char* lexeme_head, const char* lexeme_tail, enum token_type type)
{
    struct token* _Owner _Opt p_new_token = NULL;

    try
    {
        p_new_token = calloc(1, sizeof * p_new_token);
        if (p_new_token == NULL) throw;

        size_t sz = lexeme_tail - lexeme_head;
        char* _Owner _Opt temp = calloc(sz + 1, sizeof(char));
        if (temp == NULL) throw;

        p_new_token->lexeme = temp;
        p_new_token->type = type;
        strncpy(p_new_token->lexeme, lexeme_head, sz);
    }
    catch
    {
        token_delete(p_new_token);
        p_new_token = NULL;
    }
    return p_new_token;
}

struct token* _Owner _Opt identifier(struct stream* stream)
{
    const char* start = stream->current;
    stream_match(stream);
    /*
    identifier:
      identifier-nondigit
      identifier identifier-nondigit
      identifier digit

    identifier-nondigit:
      nondigit
      universal-character-name
      other implementation-defined characters
    */
    while (is_nondigit(stream) || is_digit(stream))
    {
        stream_match(stream);
    }

    struct token* _Owner _Opt p_new_token = new_token(start, stream->current, TK_IDENTIFIER);


    return p_new_token;
}


static bool first_of_character_constant(struct stream* stream)
{
    return stream->current[0] == '\'' ||
        (stream->current[0] == 'u' && stream->current[1] == '8' && stream->current[2] == '\'') ||
        (stream->current[0] == 'u' && stream->current[1] == '\'') ||
        (stream->current[0] == 'U' && stream->current[1] == '\'') ||
        (stream->current[0] == 'L' && stream->current[1] == '\'');
}

struct token* _Owner _Opt character_constant(struct tokenizer_ctx* ctx, struct stream* stream)
{
    const char* start = stream->current;

    /*
      encoding-prefix: one of
       u8 u U L
    */
    if (stream->current[0] == 'u')
    {
        stream_match(stream);
        if (stream->current[0] == '8')
            stream_match(stream);
    }
    else if (stream->current[0] == 'U' ||
        stream->current[0] == 'L')
    {
        stream_match(stream);
    }


    stream_match(stream); //"


    while (stream->current[0] != '\'')
    {
        if (stream->current[0] == '\\')
        {
            stream_match(stream);
            stream_match(stream);
        }
        else
            stream_match(stream);

        if (stream->current[0] == '\0' ||
            stream->current[0] == '\n')
        {
            tokenizer_set_warning(ctx, stream, "missing terminating ' character");
            break;
        }
    }
    stream_match(stream);
    struct token* _Owner _Opt p_new_token = new_token(start, stream->current, TK_CHAR_CONSTANT);
    return p_new_token;
}

static bool first_of_string_literal(struct stream* stream)
{
    /*
    string-literal:
    encoding_prefix_opt " s-char-sequenceopt "

    encoding_prefix:
    u8
    u
    U
    L
    */

    return stream->current[0] == '"' ||
        (stream->current[0] == 'u' && stream->current[1] == '8' && stream->current[2] == '"') ||
        (stream->current[0] == 'u' && stream->current[1] == '"') ||
        (stream->current[0] == 'U' && stream->current[1] == '"') ||
        (stream->current[0] == 'L' && stream->current[1] == '"');
}

struct token* _Owner _Opt string_literal(struct tokenizer_ctx* ctx, struct stream* stream)
{
    struct token* _Owner _Opt p_new_token = NULL;
    const char* start = stream->current;

    try
    {
        /*encoding_prefix_opt*/
        if (stream->current[0] == 'u')
        {
            stream_match(stream);
            if (stream->current[0] == '8')
                stream_match(stream);
        }
        else if (stream->current[0] == 'U' ||
            stream->current[0] == 'L')
        {
            stream_match(stream);
        }


        stream_match(stream); //"


        while (stream->current[0] != '"')
        {
            if (stream->current[0] == '\0' ||
                stream->current[0] == '\n')
            {
                tokenizer_set_error(ctx, stream, "missing terminating \" character");
                throw;
            }

            if (stream->current[0] == '\\')
            {
                stream_match(stream);
                stream_match(stream);
            }
            else
                stream_match(stream);
        }
        stream_match(stream);
        p_new_token = new_token(start, stream->current, TK_STRING_LITERAL);
    }
    catch
    {
    }

    return p_new_token;
}

int get_char_type(const char* s)
{
    if (s[0] == 'L')
        return 2; /*wchar*/

    return 1;
}
/*
  Returns the char byte size according with the literal suffix
*/
int string_literal_char_byte_size(const char* s)
{
    if (s[0] == 'u')
    {
        //must be followed by u8 but not checked here
    }
    else if (s[0] == 'U' || s[0] == 'L')
    {
        return (int)sizeof(wchar_t);
    }

    return 1;
}

int string_literal_byte_size_not_zero_included(const char* s)
{

    _Opt struct stream stream = { .source = s };

    stream.current = s;
    stream.line = 1;
    stream.col = 1;
    stream.path = "";

    int size = 0;
    const int charsize = string_literal_char_byte_size(s);

    try
    {
        /*encoding_prefix_opt*/
        if (stream.current[0] == 'u')
        {
            stream_match(&stream);
            if (stream.current[0] == '8')
                stream_match(&stream);
        }
        else if (stream.current[0] == 'U' ||
            stream.current[0] == 'L')
        {
            stream_match(&stream);
        }


        stream_match(&stream); //"


        while (stream.current[0] != '"')
        {
            if (stream.current[0] == '\0' ||
                stream.current[0] == '\n')
            {
                throw;
            }

            if (stream.current[0] == '\\')
            {
                stream_match(&stream);
                stream_match(&stream);
                size++;
            }
            else
            {
                stream_match(&stream);
                size++;
            }
        }
        stream_match(&stream);
    }
    catch
    {
    }

    /*
       Last \0 is not included
    */

    return size * charsize;
}

static struct token* _Owner _Opt ppnumber(struct stream* stream)
{
    /*
     pp-number:
      digit
      . digit
      pp-number identifier-continue
      pp-number ’ digit
      pp-number ’ nondigit
      pp-number e sign
      pp-number E sign
      pp-number p sign
      pp-number P sign
      pp-number .
    */

    /*
     identifier-continue:
       digit
       nondigit
       XID_Continue character
       universal-character-name of class XID_Continue
    */

    const char* start = stream->current;
    if (is_digit(stream))
    {
        stream_match(stream);//digit
    }
    else if (stream->current[0] == '.')
    {
        stream_match(stream); //.
        stream_match(stream); //digit
    }
    else
    {
        assert(false);
    }

    for (;;)
    {
        if (stream->current[0] == '\'')
        {
            //digit separators c23
            stream_match(stream);
            if (is_digit(stream))
            {
                stream_match(stream);
            }
            else if (is_nondigit(stream))
            {
                stream_match(stream);
            }
            else
            {
                assert(false);
                break;
            }
        }
        else if ((stream->current[0] == 'e' ||
            stream->current[0] == 'E' ||
            stream->current[0] == 'p' ||
            stream->current[0] == 'P') &&
            (stream->current[1] == '+' || stream->current[1] == '-'))
        {
            stream_match(stream);//e E  p P
            stream_match(stream);//sign
        }
        else if (stream->current[0] == '.')
        {
            stream_match(stream);//.
        }
        else if (is_digit(stream) || is_nondigit(stream))
        {
            ////identifier-continue
            /*
            * OBS test for is_nondigit must be AFTER
            * test for e E p P
            */
            stream_match(stream);//nondigit
        }
        else
        {
            break;
        }
    }
    struct token* _Owner _Opt p_new_token = new_token(start, stream->current, TK_PPNUMBER);

    return p_new_token;
}

struct token_list embed_tokenizer(struct preprocessor_ctx* ctx,
    const struct token* position,
    const char* filename_opt,
    int level, enum token_flags addflags)
{
    struct token_list list = { 0 };

    FILE* _Owner _Opt file = NULL;

    bool b_first = true;
    int line = 1;
    int col = 1;
    int count = 0;
    try
    {
#ifndef MOCKFILES
        file = (FILE * _Owner _Opt)fopen(filename_opt, "rb");
        if (file == NULL)
        {
            preprocessor_diagnostic(C_ERROR_FILE_NOT_FOUND, ctx, position, "file '%s' not found", filename_opt);
            throw;
        }
#else
        /*web versions only text files that are included*/
        char* textfile = read_file(filename_opt, true);
        if (textfile == NULL)
        {
            preprocessor_diagnostic(C_ERROR_FILE_NOT_FOUND, ctx, ctx->current, "file '%s' not found", filename_opt);
            throw;
        }

        const char* pch = textfile;
#endif

        unsigned char ch = 0;
#ifndef MOCKFILES
        while (fread(&ch, 1, 1, file))
        {
#else
        while (*pch)
        {
            ch = *pch;
            pch++;
#endif
            if (b_first)
            {
                b_first = false;
            }
            else
            {
                char b[] = ",";
                struct token* _Owner _Opt p_new_token = new_token(b, &b[1], TK_COMMA);
                if (p_new_token == NULL)
                {
                    throw;
                }

                p_new_token->flags |= addflags;
                p_new_token->level = level;
                p_new_token->token_origin = NULL;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);

                if (count > 0 && count % 25 == 0)
                {
                    /*new line*/
                    char newline[] = "\n";
                    struct token* _Owner _Opt p_new3 = new_token(newline, &newline[1], TK_NEWLINE);
                    if (p_new3 == NULL)
                    {
                        throw;
                    }

                    p_new3->level = level;
                    p_new3->token_origin = NULL;
                    p_new3->line = line;
                    p_new3->col = col;
                    token_list_add(&list, p_new3);
                }
            }

            char buffer[30] = { 0 };
            int c = snprintf(buffer, sizeof buffer, "%d", (int)ch);

            struct token* _Owner _Opt p_new_token = new_token(buffer, &buffer[c], TK_PPNUMBER);
            if (p_new_token == NULL)
            {
                throw;
            }

            p_new_token->flags |= addflags;
            p_new_token->level = level;
            p_new_token->token_origin = NULL;
            p_new_token->line = line;
            p_new_token->col = col;
            token_list_add(&list, p_new_token);


            count++;
        }
#ifdef MOCKFILES
        free(textfile);
#endif

        /*new line*/
        char newline[] = "\n";
        struct token* _Owner _Opt p_new_token = new_token(newline, &newline[1], TK_NEWLINE);
        if (p_new_token == NULL)
        {
            throw;
        }

        p_new_token->level = level;
        p_new_token->token_origin = NULL;
        p_new_token->line = line;
        p_new_token->col = col;
        token_list_add(&list, p_new_token);

        assert(list.head != NULL);
    }
    catch
    {
    }

    if (file)
        fclose(file);

    return list;
}

static bool set_sliced_flag(struct stream* stream, struct token* p_new_token)
{
    if (stream->line_continuation_count > 0)
    {
        p_new_token->flags |= TK_FLAG_LINE_CONTINUATION;
        if (stream->line_continuation_count == 1)
        {
            int l = strlen(p_new_token->lexeme);
            if (p_new_token->lexeme[l - 1] == '\n')
            {
                /*not sliced, line continuation is at end of token*/
            }
            else
            {
                p_new_token->flags |= TK_FLAG_SLICED;
            }
        }
        else
        {
            p_new_token->flags |= TK_FLAG_SLICED;
        }
    }

    return p_new_token->flags & TK_FLAG_SLICED;
}

struct token_list tokenizer(struct tokenizer_ctx* ctx, const char* text, const char* _Opt filename_opt, int level, enum token_flags addflags)
{
    struct token_list list = { 0 };

    struct stream stream =
    {
        .col = 1,
        .line = 1,
        .source = text,
        .current = text,
        .path = filename_opt ? filename_opt : ""
    };

    try
    {
        struct token* _Opt p_first = NULL;
        if (filename_opt != NULL)
        {
            const char* begin = filename_opt;
            const char* end = filename_opt + strlen(filename_opt);
            struct token* _Owner _Opt p_new = new_token(begin, end, TK_BEGIN_OF_FILE);
            if (p_new == NULL)
                throw;

            path_normalize(p_new->lexeme);
            p_new->level = level;
            p_first = token_list_add(&list, p_new);
        }


        //struct token* current = pFirst;
        bool new_line = true;
        bool has_space = false;
        while (1)
        {
            const int line = stream.line;
            const int col = stream.col;
            stream.line_continuation_count = 0;

            if (stream.current[0] == '\0')
            {
                stream_match(&stream);
                break;
            }
            if (is_digit(&stream) ||
                (stream.current[0] == '.' && isdigit(stream.current[0])))
            {
                struct token* _Owner _Opt p_new_token = ppnumber(&stream);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = false;
                has_space = false;
                continue;
            }

            /*
             Tem que vir antes identifier
            */
            if (first_of_string_literal(&stream))
            {
                struct token* _Owner _Opt p_new_token = string_literal(ctx, &stream);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = false;
                has_space = false;

                continue;
            }

            if (first_of_character_constant(&stream))
            {
                //TODO if we have ' in the middle then it is not character constant
                struct token* _Owner _Opt p_new_token = character_constant(ctx, &stream);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = false;
                has_space = false;

                continue;
            }

            if (is_nondigit(&stream))
            {
                struct token* _Owner _Opt p_new_token = identifier(&stream);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;

                new_line = false;
                has_space = false;
                if (set_sliced_flag(&stream, p_new_token))
                {
                    tokenizer_set_warning(ctx, &stream, "token sliced");
                }
                token_list_add(&list, p_new_token);
                continue;
            }

            if (stream.current[0] == ' ' ||
                stream.current[0] == '\t' ||
                stream.current[0] == '\f')
            {
                const char* start = stream.current;
                while (stream.current[0] == ' ' ||
                    stream.current[0] == '\t' ||
                    stream.current[0] == '\f'
                    )
                {
                    stream_match(&stream);
                }
                struct token* _Owner _Opt p_new_token = new_token(start, stream.current, TK_BLANKS);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                /*bNewLine = false;*/ //deixa assim
                has_space = true;

                continue;
            }
            if (stream.current[0] == '/' &&
                stream.current[1] == '/')
            {
                const char* start = stream.current;
                stream_match(&stream);
                stream_match(&stream);
                //line comment
                while (stream.current[0] != '\n')
                {
                    stream_match(&stream);

                    if (stream.current[0] == '\0')
                        break;
                }
                struct token* _Owner _Opt p_new_token = new_token(start, stream.current, TK_LINE_COMMENT);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = true;
                has_space = false;


                if (stream.current[0] == '\0')
                    break;

                continue;
            }
            if (stream.current[0] == '/' &&
                stream.current[1] == '*')
            {
                const char* start = stream.current;
                stream_match(&stream);
                stream_match(&stream);
                //line comment
                for (;;)
                {
                    if (stream.current[0] == '*' && stream.current[1] == '/')
                    {
                        stream_match(&stream);
                        stream_match(&stream);
                        break;
                    }
                    else if (stream.current[0] == '\0')
                    {
                        tokenizer_set_error(ctx, &stream, "missing end of comment");
                        break;
                    }
                    else
                    {
                        stream_match(&stream);
                    }
                }
                struct token* _Owner _Opt p_new_token = new_token(start, stream.current, TK_COMMENT);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                new_line = false;
                has_space = false;

                /*
                * Ignore line splicing inside comments.
                * if you are curious to see when it happens just add
                * set_sliced_flag
                */

                continue;
            }
            if (new_line && stream.current[0] == '#')
            {
                const char* start = stream.current;
                stream_match(&stream);
                struct token* _Owner _Opt p_new_token = new_token(start, stream.current, '#');
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                p_new_token->type = TK_PREPROCESSOR_LINE;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = false;
                has_space = false;

                continue;
            }


            if (stream.current[0] == '\n' || stream.current[0] == '\r')
            {
                if (stream.current[0] == '\r' && stream.current[1] == '\n')
                {
                    stream_match(&stream);
                    stream_match(&stream);
                }
                else
                {
                    stream_match(&stream);
                }
                char  newline[] = "\n";
                struct token* _Owner _Opt p_new_token = new_token(newline, newline + 1, TK_NEWLINE);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = true;
                has_space = false;

                continue;
            }
            const char* start = stream.current;
            enum token_type t = is_punctuator(&stream);
            if (t != TK_NONE)
            {

                struct token* _Owner _Opt p_new_token = new_token(start, stream.current, t);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = false;
                has_space = false;

                continue;
            }
            else
            {
                stream_match(&stream);
                struct token* _Owner _Opt p_new_token = new_token(start, stream.current, ANY_OTHER_PP_TOKEN);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                set_sliced_flag(&stream, p_new_token);
                token_list_add(&list, p_new_token);
                new_line = false;
                has_space = false;

                continue;
            }

            break;
        }
    }
    catch
    {
    }

    assert(list.head != NULL);
    return list;
}


bool fread2(void* buffer, size_t size, size_t count, FILE * stream, size_t * sz)
{
    *sz = 0;//out
    bool result = false;
    size_t n = fread(buffer, size, count, stream);
    if (n == count)
    {
        *sz = n;
        result = true;
    }
    else if (n < count)
    {
        if (feof(stream))
        {
            *sz = n;
            result = true;
        }
    }
    return result;
}


bool preprocessor_token_ahead_is_identifier(struct token* p, const char* lexeme);
struct token_list group_part(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level);
struct token_list group_opt(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    /*
      group:
       group-part
       group group-part
    */
    struct token_list r = { 0 };
    try
    {

        if (token_list_is_empty(input_list))
        {
            return r;
        }
        while (!token_list_is_empty(input_list))
        {
            assert(input_list->head != NULL);

            if (input_list->head->type == TK_PREPROCESSOR_LINE &&
                (preprocessor_token_ahead_is_identifier(input_list->head, "endif") ||
                    preprocessor_token_ahead_is_identifier(input_list->head, "else") ||
                    preprocessor_token_ahead_is_identifier(input_list->head, "elif") ||
                    preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") ||
                    preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")))
            {
                /*follow of group-part*/
                break;
            }
            else
            {
                struct token_list r2 = group_part(ctx, input_list, is_active, level);
                token_list_append_list(&r, &r2);
                token_list_destroy(&r2);
                if (ctx->n_errors > 0) throw;
            }
        }
    }
    catch
    {
    }

    return r;
}

bool is_parser_token(struct token* p)
{
    return p->type != TK_COMMENT &&
        p->type != TK_BLANKS &&
        p->type != TK_LINE_COMMENT &&
        p->type != TK_NEWLINE;
}

bool is_never_final(enum token_type type)
{
    return type == TK_BEGIN_OF_FILE ||
        type == TK_BLANKS ||
        type == TK_LINE_COMMENT ||
        type == TK_COMMENT ||
        type == TK_PLACEMARKER ||
        type == TK_NEWLINE;
}

enum token_type is_keyword(const char* text);

struct token* _Opt preprocessor_look_ahead_core(struct token* p)
{
    struct token* _Opt current = p->next;

    while (current &&
        (current->type == TK_BLANKS ||
            current->type == TK_PLACEMARKER ||
            current->type == TK_LINE_COMMENT ||
            current->type == TK_COMMENT))
    {
        current = current->next;
    }
    return current;
}

bool preprocessor_token_ahead_is(struct token* p, enum token_type t)
{
    struct token* _Opt p_token = preprocessor_look_ahead_core(p);
    if (p_token != NULL && p_token->type == t)
        return true;
    return false;
}

bool preprocessor_token_ahead_is_identifier(struct token* p, const char* lexeme)
{
    assert(p != NULL);
    struct token* _Opt p_token = preprocessor_look_ahead_core(p);
    if (p_token != NULL && p_token->type == TK_IDENTIFIER)
    {
        return strcmp(p_token->lexeme, lexeme) == 0;
    }
    return false;
}

static void skip_blanks_level(struct preprocessor_ctx* ctx, struct token_list* dest, struct token_list* input_list, int level)
{
    while (input_list->head)
    {
        if (!token_is_blank(input_list->head))
            break;

        if (INCLUDE_ALL || level == 0)
        {
            struct token* _Owner _Opt p =
                token_list_pop_front_get(input_list);
            assert(p != NULL); //because input_list is not empty
            token_list_add(dest, p);
        }
        else
            token_list_pop_front(input_list);
    }
}

static void skip_blanks(struct preprocessor_ctx* ctx, struct token_list* dest, struct token_list* input_list)
{
    while (input_list->head)
    {
        if (!token_is_blank(input_list->head))
            break;
        struct token* _Owner _Opt p =
            token_list_pop_front_get(input_list);
        assert(p != NULL); //because input_list is not empty

        token_list_add(dest, p);
    }
}

void prematch_level(struct token_list* dest, struct token_list* input_list, int level)
{
    if (INCLUDE_ALL || level == 0)
    {
        struct token* _Owner _Opt p = token_list_pop_front_get(input_list);
        if (p)
        {
            token_list_add(dest, p);
        }
    }
    else
        token_list_pop_front(input_list);
}

void prematch(struct token_list* dest, struct token_list* input_list)
{
    struct token* _Owner _Opt p = token_list_pop_front_get(input_list);
    if (p)
    {
        token_list_add(dest, p);
    }
}

struct token_list pp_tokens_opt(struct preprocessor_ctx* ctx, struct token_list* input_list, int level);

struct token_list process_defined(struct preprocessor_ctx* ctx, struct token_list* input_list)
{
    struct token_list r = { 0 };

    try
    {
        while (input_list->head != NULL)
        {
            if (input_list->head->type == TK_IDENTIFIER &&
                strcmp(input_list->head->lexeme, "defined") == 0)
            {
                token_list_pop_front(input_list);
                skip_blanks(ctx, &r, input_list);

                if (input_list->head == NULL)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    throw;
                }

                bool has_parentesis = false;
                if (input_list->head->type == '(')
                {
                    token_list_pop_front(input_list);
                    has_parentesis = true;
                }

                skip_blanks(ctx, &r, input_list);


                if (input_list->head == NULL)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    throw;
                }

                struct macro* _Opt macro = find_macro(ctx, input_list->head->lexeme);
                struct token* _Owner _Opt p_new_token = token_list_pop_front_get(input_list);
                if (p_new_token == NULL)
                {
                    throw;
                }

                p_new_token->type = TK_PPNUMBER;
                char* _Owner _Opt temp = NULL;

                if (macro)
                {
                    temp = strdup("1");
                }
                else
                {
                    temp = strdup("0");
                }

                if (temp == NULL)
                {
                    token_delete(p_new_token);
                    throw;
                }

                free(p_new_token->lexeme);
                p_new_token->lexeme = temp;

                token_list_add(&r, p_new_token);

                if (has_parentesis)
                {
                    if (input_list->head == NULL)
                    {
                        throw;
                    }

                    if (input_list->head->type != ')')
                    {
                        preprocessor_diagnostic(C_ERROR_MISSING_CLOSE_PARENTHESIS, ctx, input_list->head, "missing )");
                        throw;
                    }
                    token_list_pop_front(input_list);
                }


            }
            else if (input_list->head->type == TK_IDENTIFIER &&
                (strcmp(input_list->head->lexeme, "__has_include") == 0 ||
                    strcmp(input_list->head->lexeme, "__has_embed") == 0)
                )
            {
                token_list_pop_front(input_list); //pop __has_include
                skip_blanks(ctx, &r, input_list);
                token_list_pop_front(input_list); //pop (
                skip_blanks(ctx, &r, input_list);


                char path[100] = { 0 };
                bool is_angle_bracket_form = false;

                if (input_list->head == NULL)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    throw;
                }

                if (input_list->head->type == TK_STRING_LITERAL)
                {
                    strcat(path, input_list->head->lexeme);
                    token_list_pop_front(input_list); //pop "file"
                }
                else
                {
                    is_angle_bracket_form = true;
                    token_list_pop_front(input_list); //pop <

                    if (input_list->head == NULL)
                    {
                        pre_unexpected_end_of_file(r.tail, ctx);
                        throw;
                    }

                    while (input_list->head->type != '>')
                    {
                        strcat(path, input_list->head->lexeme);
                        token_list_pop_front(input_list); //pop (

                        if (input_list->head == NULL)
                        {
                            pre_unexpected_end_of_file(r.tail, ctx);
                            throw;
                        }
                    }
                    token_list_pop_front(input_list); //pop >
                }

                char fullpath[300] = { 0 };



                char full_path_result[200] = { 0 };
                bool already_included = false;
                const char* _Owner _Opt s = find_and_read_include_file(ctx,
                    path,
                    fullpath,
                    is_angle_bracket_form,
                    &already_included,
                    full_path_result,
                    sizeof full_path_result,
                  false);

                bool has_include = s != NULL;
                free((void* _Owner)s);

                struct token* _Owner _Opt p_new_token = calloc(1, sizeof * p_new_token);
                if (p_new_token == NULL)
                {
                    throw;
                }

                p_new_token->type = TK_PPNUMBER;

                char* _Owner _Opt temp = strdup(has_include ? "1" : "0");
                if (temp == NULL)
                {
                    token_delete(p_new_token);
                    throw;
                }
                p_new_token->lexeme = temp;
                p_new_token->flags |= TK_FLAG_FINAL;

                token_list_add(&r, p_new_token);
                token_list_pop_front(input_list); //pop )
            }
            else if (input_list->head->type == TK_IDENTIFIER &&
                strcmp(input_list->head->lexeme, "__has_c_attribute") == 0)
            {
                token_list_pop_front(input_list); //pop __has_include
                skip_blanks(ctx, &r, input_list);
                token_list_pop_front(input_list); //pop (
                skip_blanks(ctx, &r, input_list);

                if (input_list->head == NULL)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    throw;
                }

                char path[100] = { 0 };
                while (input_list->head->type != ')')
                {
                    strcat(path, input_list->head->lexeme);
                    token_list_pop_front(input_list); //pop (

                    if (input_list->head == NULL)
                    {
                        pre_unexpected_end_of_file(r.tail, ctx);
                        throw;
                    }
                }
                token_list_pop_front(input_list); //pop >

                /*nodiscard
                * The __has_c_attribute conditional inclusion expression (6.10.1) shall
                * return the value 202003L
                * when given nodiscard as the pp-tokens operand.
                */

                /*maybe_unused
                * The __has_c_attribute conditional inclusion expression (6.10.1) shall return
                * the value 202106L when given maybe_unused as the pp-tokens operand.
                */

                /*deprecated
                * The __has_c_attribute conditional inclusion expression (6.10.1) shall return the value 201904L
                * when given deprecated as the pp-tokens operand
                */

                /*noreturn
                * The __has_c_attribute conditional inclusion expression (6.10.1) shall return the value 202202L
                * when given noreturn as the pp-tokens operand.
                */

                /*reproducible
                 * The __has_c_attribute conditional inclusion expression (6.10.1) shall return the value 202207L
                 * when given reproducible as the pp-tokens operand.
                */

                /*
                * The __has_c_attribute conditional inclusion expression (6.10.1) shall return the value 202207L
                * when given unsequenced as the pp-tokens operand.
                */
                bool has_c_attribute = false;

                struct token* _Owner _Opt p_new_token = calloc(1, sizeof * p_new_token);
                if (p_new_token == NULL)
                {
                    throw;
                }

                p_new_token->type = TK_PPNUMBER;
                char* _Owner _Opt temp = strdup(has_c_attribute ? "1" : "0");
                if (temp == NULL)
                {
                    token_delete(p_new_token);
                    throw;
                }

                p_new_token->lexeme = temp;
                p_new_token->flags |= TK_FLAG_FINAL;

                token_list_add(&r, p_new_token);
                token_list_pop_front(input_list); //pop )
            }
            else
            {
                struct token* _Owner _Opt tk = token_list_pop_front_get(input_list);
                if (tk)
                    token_list_add(&r, tk);
            }
        }
    }
    catch
    {
        //TODO clear?
    }

    return r;
}

struct token_list process_identifiers(struct preprocessor_ctx* ctx, _Dtor struct token_list* list)
{
    assert(!token_list_is_empty(list));

    struct token_list list2 = { 0 };

    try
    {
        while (list->head != NULL)
        {
            if (list->head->type == TK_IDENTIFIER ||
                list->head->type == TK_IDENTIFIER_RECURSIVE_MACRO)
            {
                /*
                after  all  replacements  due  to  macro expansion and evaluations of
                defined    macro    expressions,    has_include    expressions,   and
                has_c_attribute   expressions  have  been  performed,  all  remaining
                identifiers  other  than true (including those lexically identical to
                keywords  such  as  false) are replaced with the pp-number 0, true is
                replaced  with  pp-number  1,  and  then  each preprocessing token is
                converted into a token.
                */

                struct token* _Owner _Opt p_new_token = token_list_pop_front_get(list);
                assert(p_new_token != NULL); //because the list is not empty
                p_new_token->type = TK_PPNUMBER;

                if (strcmp(p_new_token->lexeme, "true") == 0)
                {
                    p_new_token->lexeme[0] = '1';
                    p_new_token->lexeme[1] = '\0';
                }
                else if (strcmp(p_new_token->lexeme, "false") == 0)
                {
                    p_new_token->lexeme[0] = '0';
                    p_new_token->lexeme[1] = '\0';
                }
                else
                {
                    char* _Owner _Opt temp = strdup("0");
                    if (temp == NULL)
                    {
                        token_delete(p_new_token);
                        throw;
                    }
                    free(p_new_token->lexeme);
                    p_new_token->lexeme = temp;
                }

                token_list_add(&list2, p_new_token);
            }
            else
            {
                struct token* _Owner _Opt ptk = token_list_pop_front_get(list);
                assert(ptk != NULL); //because the list is not empty
                token_list_add(&list2, ptk);
            }
        }
        assert(!token_list_is_empty(&list2));
    }
    catch
    {
    }
    return list2;
}

struct token_list ignore_preprocessor_line(struct token_list* input_list)
{
    struct token_list r = { 0 };
    while (input_list->head && input_list->head->type != TK_NEWLINE)
    {
        struct token* _Owner _Opt tk = token_list_pop_front_get(input_list);
        assert(tk != NULL); //because the list is not empty
        token_list_add(&r, tk);
    }
    return r;
}

//todo passar lista para reotnro
long long preprocessor_constant_expression(struct preprocessor_ctx* ctx,
    struct token_list* output_list,
    struct token_list* input_list,
    int level
)
{
    assert(input_list->head != NULL);

    struct token* first = input_list->head;

    ctx->conditional_inclusion = true;

    struct token_list r = { 0 };
    while (input_list->head && input_list->head->type != TK_NEWLINE)
    {
        struct token* _Owner _Opt tk = token_list_pop_front_get(input_list);
        assert(tk != NULL); //because the list is not empty
        token_list_add(&r, tk);

        /*
          We call preprocessor that emmit warnings if line continuation
          is used outside macro directives.
          Let's remove TK_FLAG_LINE_CONTINUATION from the original token
          to avoid warning inside constant expressions
        */
        assert(r.tail != NULL);
        r.tail->flags &= ~TK_FLAG_LINE_CONTINUATION;
    }

    struct token_list list1 = copy_replacement_list(ctx, &r);
    token_list_swap(output_list, &r);


    int flags = ctx->flags;
    ctx->flags |= PREPROCESSOR_CTX_FLAGS_ONLY_FINAL;

    /*defined X  por exemplo é mantido sem ser expandido*/

    struct token_list list2 = preprocessor(ctx, &list1, 1);
    ctx->flags = flags;

    long long value = 0;

    if (list2.head == NULL)
    {
        preprocessor_diagnostic(C_ERROR_EXPRESSION_ERROR, ctx, first, "empty expression");
    }
    else
    {
        /*aonde defined has_c_aatribute sao transformados em constantes*/
        struct token_list list3 = process_defined(ctx, &list2);

        struct token_list list4 = process_identifiers(ctx, &list3);

        assert(list4.head != NULL);

        struct preprocessor_ctx pre_ctx = { 0 };

        pre_ctx.options = ctx->options;
        pre_ctx.input_list = list4;
        pre_ctx.current = pre_ctx.input_list.head;

        if (pre_constant_expression(&pre_ctx, &value) != 0)
        {
            preprocessor_diagnostic(C_ERROR_EXPRESSION_ERROR, ctx, first, "expression error");
        }

        ctx->conditional_inclusion = false;

        preprocessor_ctx_destroy(&pre_ctx);
    }

    token_list_destroy(&list1);
    token_list_destroy(&r);
    token_list_destroy(&list2);

    return value;
}

void match_level(struct token_list* dest, struct token_list* input_list, int level)
{
    if (INCLUDE_ALL || level == 0)
    {
        struct token* _Owner _Opt tk = token_list_pop_front_get(input_list);
        if (tk)
        {
            token_list_add(dest, tk);
        }
    }
    else
        token_list_pop_front(input_list);
}


int match_token_level(struct token_list* dest, struct token_list* input_list, enum token_type type, int level,
    struct preprocessor_ctx* ctx)
{
    try
    {
        if (input_list->head == NULL ||
            input_list->head->type != type)
        {
            if (type == TK_NEWLINE && input_list->head == NULL)
            {
                //vou aceitar final de arquivo como substituro do endline
                //exemplo #endif sem quebra de linha
            }
            else
            {
                if (input_list->head)
                    preprocessor_diagnostic(C_ERROR_UNEXPECTED_TOKEN, ctx, input_list->head, "expected token %s got %s\n", get_token_name(type), get_token_name(input_list->head->type));
                else
                    preprocessor_diagnostic(C_ERROR_UNEXPECTED_TOKEN, ctx, dest->tail, "expected EOF \n");

                throw;
            }
        }
        if (input_list->head != NULL)
        {
            if (INCLUDE_ALL || level == 0)
                token_list_add(dest, token_list_pop_front_get(input_list));
            else
                token_list_pop_front(input_list);
        }
    }
    catch
    {
    }
    return ctx->n_errors > 0;
}


struct token_list if_group(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, bool* p_result)
{
    *p_result = 0; //out

    assert(input_list->head != NULL);

    struct token_list r = { 0 };
    try
    {
        /*
         if-group:
           # if constant-expression new-line group_opt
           # ifdef identifier new-line group_opt
           # ifndef identifier new-line group_opt
        */
        match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);

        if (input_list->head == NULL)
            throw;

        assert(input_list->head->type == TK_IDENTIFIER);
        if (strcmp(input_list->head->lexeme, "ifdef") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //ifdef
            skip_blanks_level(ctx, &r, input_list, level);

            if (input_list->head == NULL)
            {
                pre_unexpected_end_of_file(r.tail, ctx);
                throw;
            }

            if (is_active)
            {
                struct macro* _Opt macro = find_macro(ctx, input_list->head->lexeme);
                *p_result = (macro != NULL) ? 1 : 0;
                //printf("#ifdef %s (%s)\n", input_list->head->lexeme, *p_result ? "true" : "false");
            }
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
            skip_blanks_level(ctx, &r, input_list, level);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
        }
        else if (strcmp(input_list->head->lexeme, "ifndef") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //ifndef
            skip_blanks_level(ctx, &r, input_list, level);

            if (input_list->head == NULL)
            {
                pre_unexpected_end_of_file(r.tail, ctx);
                throw;
            }

            if (is_active)
            {
                struct macro* _Opt macro = find_macro(ctx, input_list->head->lexeme);
                *p_result = (macro == NULL) ? 1 : 0;
            }
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
            skip_blanks_level(ctx, &r, input_list, level);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
        }
        else if (strcmp(input_list->head->lexeme, "if") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //if
            skip_blanks_level(ctx, &r, input_list, level);
            if (is_active)
            {
                struct token_list r0 = { 0 };
                *p_result = preprocessor_constant_expression(ctx, &r0, input_list, level);
                token_list_append_list(&r, &r0);
                token_list_destroy(&r0);
            }
            else
            {
                struct token_list r0 = ignore_preprocessor_line(input_list);
                token_list_append_list(&r, &r0);
                token_list_destroy(&r0);
            }
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
        }
        else
        {

            preprocessor_diagnostic(C_ERROR_UNEXPECTED, ctx, input_list->head, "unexpected");
            throw;
        }
        struct token_list r2 = group_opt(ctx, input_list, is_active && *p_result, level);
        token_list_append_list(&r, &r2);
        assert(r2.head == NULL);
        assert(r2.tail == NULL);
    }
    catch
    {
    }

    return r;
}

struct token_list elif_group(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, bool* p_elif_result)
{
    *p_elif_result = 0; //out
    assert(input_list->head != NULL);

    struct token_list r = { 0 };

    try
    {
        /*
         elif-group:
          # elif constant-expression new-line group_opt

          C23
          # elifdef identifier new-line group_opt
          # elifndef identifier new-line group_opt
        */
        match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx);
        skip_blanks(ctx, &r, input_list);

        if (input_list->head == NULL)
            throw;

        unsigned long long result = 0;
        if (strcmp(input_list->head->lexeme, "elif") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
            skip_blanks(ctx, &r, input_list);

            if (is_active)
            {
                struct token_list r0 = { 0 };
                result = preprocessor_constant_expression(ctx, &r0, input_list, level);
                token_list_append_list(&r, &r0);
                token_list_destroy(&r0);
            }
            else
            {
                struct token_list r0 = ignore_preprocessor_line(input_list);
                token_list_append_list(&r, &r0);
                token_list_destroy(&r0);
            }
        }
        else if (strcmp(input_list->head->lexeme, "elifdef") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
            skip_blanks(ctx, &r, input_list);

            if (input_list->head == NULL)
                throw;

            if (is_active)
            {
                result = (hashmap_find(&ctx->macros, input_list->head->lexeme) != NULL) ? 1 : 0;
            }
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
        }
        else if (strcmp(input_list->head->lexeme, "elifndef") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
            skip_blanks(ctx, &r, input_list);

            if (input_list->head == NULL)
                throw;

            if (is_active)
            {
                result = (hashmap_find(&ctx->macros, input_list->head->lexeme) == NULL) ? 1 : 0;
            }
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
        }
        *p_elif_result = (result != 0);
        skip_blanks(ctx, &r, input_list);
        match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
        struct token_list r2 = group_opt(ctx, input_list, is_active && *p_elif_result, level);
        token_list_append_list(&r, &r2);
        token_list_destroy(&r2);
    }
    catch
    {
    }

    return r;
}

struct token_list elif_groups(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, bool* pelif_result)
{
    assert(input_list->head != NULL);

    struct token_list r = { 0 };
    /*
    elif-groups:
      elif-group
      elif-groups elif-group
    */
    try
    {
        bool already_found_elif_true = false;
        bool elif_result = false;
        struct token_list r2 = elif_group(ctx, input_list, is_active, level, &elif_result);

        if (input_list->head == NULL)
        {
            token_list_destroy(&r2);
            throw;
        }

        token_list_append_list(&r, &r2);

        if (elif_result)
            already_found_elif_true = true;

        if (input_list->head->type == TK_PREPROCESSOR_LINE &&
            (
                preprocessor_token_ahead_is_identifier(input_list->head, "elif") ||
                preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") ||
                preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")
                )
            )
        {
            /*
              Depois que acha 1 true already_found_elif_true os outros sao false.
            */
            struct token_list r3 = elif_groups(ctx, input_list, is_active && !already_found_elif_true, level, &elif_result);
            token_list_append_list(&r, &r3);
            if (elif_result)
                already_found_elif_true = true;
            token_list_destroy(&r3);
        }
        /*
           Se algum dos elifs foi true retorna true
        */
        *pelif_result = already_found_elif_true;
        token_list_destroy(&r2);
    }
    catch
    {
        //TODO clear
    }

    return r;
}

struct token_list else_group(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    /*
      else-group:
       # else new-line group_opt
    */

    struct token_list r = { 0 };
    try
    {
        match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        if (ctx->n_errors > 0) throw;

        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //else
        skip_blanks_level(ctx, &r, input_list, level);
        match_token_level(&r, input_list, TK_NEWLINE, level, ctx);

        struct token_list r2 = group_opt(ctx, input_list, is_active, level);
        token_list_append_list(&r, &r2);

        token_list_destroy(&r2);
    }
    catch
    {
        //tODO
    }

    return r;
}

struct token_list endif_line(struct preprocessor_ctx* ctx, struct token_list* input_list, int level)
{
    /*
     endif-line:
       # endif new-line
    */

    struct token_list r = { 0 };

    match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx); //#
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //endif
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, TK_NEWLINE, level, ctx);

    return r;
}
struct token_list identifier_list(struct preprocessor_ctx* ctx, struct macro* macro, struct token_list* input_list, int level);
struct token_list replacement_list(struct preprocessor_ctx* ctx, struct macro* macro, struct token_list* input_list, int level);
static bool is_empty_assert(struct token_list* replacement_list);

struct token_list def_line(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, struct macro** pp_macro)
{
    //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt

    /*
    def-line:
       # def identifier new-line
       # def identifier lparen identifier-list(opt) ) new-line
       # def identifier lparen ... ) new-line
       # def identifier lparen identifier-list , ... ) new-line
    */
    struct token_list r = { 0 };

    try
    {
        /*
          This code is the same of define...TODO share
        */
        struct macro* _Owner _Opt macro = calloc(1, sizeof * macro);
        if (macro == NULL)
        {
            preprocessor_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, "out of mem");
            throw;
        }

        macro->def_macro = true;

        match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx); //#

        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //def
        skip_blanks_level(ctx, &r, input_list, level);

        if (input_list->head == NULL)
        {
            macro_delete(macro);
            pre_unexpected_end_of_file(r.tail, ctx);
            throw;
        }

        struct token* macro_name_token = input_list->head;

        if (is_builtin_macro(macro_name_token->lexeme))
        {
            preprocessor_diagnostic(W_REDEFINING_BUITIN_MACRO,
                ctx,
                input_list->head,
                "redefining builtin macro");
        }

        if (hashmap_find(&ctx->macros, input_list->head->lexeme) != NULL)
        {
            //printf("warning: '%s' macro redefined at %s %d\n",
              //     input_list->head->lexeme,
                ///   input_list->head->token_origin->lexeme,
                  // input_list->head->line);
        }

        char* _Owner _Opt temp = strdup(input_list->head->lexeme);
        if (temp == NULL)
        {
            macro_delete(macro);
            throw;
        }
        assert(macro->name == NULL);
        macro->name = temp;


        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //nome da macro

        if (input_list->head == NULL)
        {
            macro_delete(macro);
            pre_unexpected_end_of_file(r.tail, ctx);
            throw;
        }

        /*sem skip*/
        //p = preprocessor_match_token(p, is_active, level, false, IDENTIFIER); /*name*/
        if (input_list->head->type == '(')
        {
            macro->is_function = true;

            match_token_level(&r, input_list, '(', level, ctx);
            skip_blanks_level(ctx, &r, input_list, level);

            if (input_list->head == NULL)
            {
                macro_delete(macro);
                pre_unexpected_end_of_file(r.tail, ctx);
                throw;
            }

            if (input_list->head->type == '...')
            {
                struct macro_parameter* _Owner _Opt p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
                if (p_macro_parameter == NULL)
                {
                    macro_delete(macro);
                    throw;
                }

                char* _Owner _Opt temp2 = strdup("__VA_ARGS__");
                if (temp2 == NULL)
                {
                    macro_delete(macro);
                    macro_parameters_delete(p_macro_parameter);
                    throw;
                }

                p_macro_parameter->name = temp2;
                macro->parameters = p_macro_parameter;

                // assert(false);
                match_token_level(&r, input_list, '...', level, ctx); //nome da macro
                skip_blanks_level(ctx, &r, input_list, level);
                match_token_level(&r, input_list, ')', level, ctx); //nome da macro
            }
            else if (input_list->head->type == ')')
            {
                match_token_level(&r, input_list, ')', level, ctx);
                skip_blanks_level(ctx, &r, input_list, level);
            }
            else
            {
                struct token_list r3 = identifier_list(ctx, macro, input_list, level);
                token_list_append_list(&r, &r3);
                token_list_destroy(&r3);

                skip_blanks_level(ctx, &r, input_list, level);
                if (input_list->head == NULL)
                {
                    macro_delete(macro);
                    pre_unexpected_end_of_file(r.tail, ctx);
                    throw;
                }

                if (input_list->head->type == '...')
                {
                    struct macro_parameter* _Owner _Opt p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
                    if (p_macro_parameter == NULL)
                    {
                        macro_delete(macro);
                        throw;
                    }

                    char* _Owner _Opt temp3 = strdup("__VA_ARGS__");
                    if (temp3 == NULL)
                    {
                        macro_delete(macro);
                        macro_parameters_delete(p_macro_parameter);
                        throw;
                    }

                    p_macro_parameter->name = temp3;
                    struct macro_parameter* _Opt p_last = macro->parameters;
                    assert(p_last != NULL);
                    while (p_last->next)
                    {
                        p_last = p_last->next;
                    }
                    p_last->next = p_macro_parameter;

                    match_token_level(&r, input_list, '...', level, ctx);
                }
                skip_blanks_level(ctx, &r, input_list, level);
                match_token_level(&r, input_list, ')', level, ctx);
            }
        }
        else
        {
            macro->is_function = false;
        }
        skip_blanks_level(ctx, &r, input_list, level);
        if (input_list->head == NULL)
        {
            macro_delete(macro);
            pre_unexpected_end_of_file(r.tail, ctx);
            throw;
        }

        naming_convention_macro(ctx, macro_name_token);

        struct hash_item_set item = { 0 };
        item.p_macro = macro;
        hashmap_set(&ctx->macros, macro->name, &item);
        hash_item_set_destroy(&item);
        *pp_macro = macro;
    }
    catch
    {
    }

    return r;
}
struct token_list replacement_group(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt

    /*replacement-group:
        pp-tokens(opt) new-line
        replacement-group pp-tokens(opt) new-line
    */
    struct token_list r = { 0 };
    try
    {
        for (;;)
        {
            if (input_list->head == NULL)
            {
                preprocessor_diagnostic(C_ERROR_UNEXPECTED, ctx, r.tail, "missing #enddef");
                throw;
            }

            if (input_list->head->type == TK_PREPROCESSOR_LINE && (
                preprocessor_token_ahead_is_identifier(input_list->head, "enddef")))
            {
                break;
            }
            prematch_level(&r, input_list, level);
        }
    }
    catch
    {
    }
    return r;
}


struct token_list enddef_line(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt
    /*
      enddef-line:
        # enddef new-line
    */

    struct token_list r = { 0 };
    try
    {
        if (input_list->head == NULL)
        {
            pre_unexpected_end_of_file(r.tail, ctx);
            throw;
        }

        match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx); //#
        skip_blanks_level(ctx, &r, input_list, level);
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //enddef
        skip_blanks_level(ctx, &r, input_list, level);
        match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
    }
    catch
    {
    }
    return r;
}

struct token_list def_section(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    /*
     def-section:
       def-line replacement-group(opt) enddef-line
    */
    struct token_list r = { 0 };
    try
    {
        struct macro* p_macro = NULL;
        struct token_list r2 = def_line(ctx, input_list, is_active, level, &p_macro);
        token_list_append_list(&r, &r2);

        if (ctx->n_errors > 0 || p_macro == NULL)
        {
            token_list_destroy(&r2);
            throw;
        }

        struct token_list r3 = replacement_group(ctx, input_list, is_active, level);

        if (ctx->n_errors > 0)
        {

            token_list_destroy(&r2);
            token_list_destroy(&r3);
            throw;
        }

        struct token_list copy = copy_replacement_list(ctx, &r3);
        token_list_append_list(&p_macro->replacement_list, &copy);

        token_list_append_list(&r, &r3);
        struct token_list r4 = enddef_line(ctx, input_list, is_active, level);
        token_list_append_list(&r, &r4);

        token_list_destroy(&r2);
        token_list_destroy(&r3);
        token_list_destroy(&r4);
        token_list_destroy(&copy);
    }
    catch
    {
    }


    return r;
}

struct token_list if_section(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    /*
     if-section:
       if-group elif-groups_opt else-group_opt endif-line
    */
    assert(input_list->head != NULL);

    struct token_list r = { 0 };

    try
    {
        bool if_result = false;
        struct token_list r2 = if_group(ctx, input_list, is_active, level, &if_result);
        if (ctx->n_errors > 0)
        {
            token_list_destroy(&r2);
            throw;
        }

        if (input_list->head == NULL)
        {
            token_list_destroy(&r2);
            throw;
        }

        token_list_append_list(&r, &r2);
        bool elif_result = false;
        if (input_list->head->type == TK_PREPROCESSOR_LINE && (
            preprocessor_token_ahead_is_identifier(input_list->head, "elif") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")))
        {
            struct token_list r3 = elif_groups(ctx, input_list, is_active && !if_result, level, &elif_result);
            token_list_append_list(&r, &r3);
            token_list_destroy(&r3);
        }

        if (input_list->head == NULL)
        {
            token_list_destroy(&r2);
            pre_unexpected_end_of_file(r.tail, ctx);
            throw;
        }


        if (input_list->head->type == TK_PREPROCESSOR_LINE &&
            preprocessor_token_ahead_is_identifier(input_list->head, "else"))
        {
            struct token_list r4 = else_group(ctx, input_list, is_active && !if_result && !elif_result, level);
            token_list_append_list(&r, &r4);
            token_list_destroy(&r4);
        }

        if (ctx->n_errors > 0)
        {
            token_list_destroy(&r2);
            throw;
        }

        struct token_list r5 = endif_line(ctx, input_list, level);
        token_list_append_list(&r, &r5);
        token_list_destroy(&r5);
        token_list_destroy(&r2);
    }
    catch
    {
    }

    return r;
}

struct token_list identifier_list(struct preprocessor_ctx* ctx, struct macro* macro, struct token_list* input_list, int level)
{
    assert(input_list->head != NULL);
    struct token_list r = { 0 };

    try
    {
        /*
          identifier-list:
          identifier
          identifier-list , identifier
        */
        skip_blanks(ctx, &r, input_list);

        if (input_list->head == NULL)
            throw;

        struct macro_parameter* _Owner _Opt p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
        if (p_macro_parameter == NULL) throw;

        char* _Owner _Opt temp = strdup(input_list->head->lexeme);
        if (temp == NULL)
        {
            macro_parameters_delete(p_macro_parameter);
            throw;
        }
        p_macro_parameter->name = temp;

        assert(macro->parameters == NULL);
        macro->parameters = p_macro_parameter;

        struct macro_parameter* p_last_parameter = macro->parameters;

        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
        skip_blanks(ctx, &r, input_list);

        if (input_list->head == NULL)
        {
            throw;
        }

        while (input_list->head->type == ',')
        {
            match_token_level(&r, input_list, ',', level, ctx);
            skip_blanks(ctx, &r, input_list);

            if (input_list->head == NULL)
            {
                throw;
            }

            if (input_list->head->type == '...')
            {
                break;
            }

            struct macro_parameter* _Owner _Opt p_new_macro_parameter = calloc(1, sizeof * p_new_macro_parameter);
            if (p_new_macro_parameter == NULL)
                throw;

            char* _Opt _Owner temp2 = strdup(input_list->head->lexeme);
            if (temp2 == NULL)
            {
                macro_parameters_delete(p_new_macro_parameter);
                throw;
            }

            p_new_macro_parameter->name = temp2;

            assert(p_last_parameter->next == NULL);
            p_last_parameter->next = p_new_macro_parameter;
            p_last_parameter = p_last_parameter->next;

            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
            skip_blanks(ctx, &r, input_list);

            if (input_list->head == NULL)
            {
                throw;
            }
        }
    }
    catch
    {
    }
    return r;
}


struct token_list replacement_list(struct preprocessor_ctx* ctx, struct macro* macro, struct token_list* input_list, int level)
{
    struct token_list r = { 0 };

    try
    {
        if (input_list->head == NULL)
        {
            pre_unexpected_end_of_file(NULL, ctx);
            throw;
        }

        while (input_list->head->type != TK_NEWLINE)
        {
            match_level(&r, input_list, level);
            if (input_list->head == NULL)
            {
                //terminou define sem quebra de linha
                break;
            }
        }

        assert(macro->replacement_list.head == NULL);
        struct token_list copy = copy_replacement_list(ctx, &r);
        token_list_append_list(&macro->replacement_list, &copy);
        token_list_destroy(&copy);
    }
    catch
    {
    }
    return r;
}

struct token_list pp_tokens_opt(struct preprocessor_ctx* ctx, struct token_list* input_list, int level)
{
    struct token_list r = { 0 };
    while (input_list->head && input_list->head->type != TK_NEWLINE)
    {
        prematch_level(&r, input_list, level);
    }
    return r;
}

static bool is_empty_assert(struct token_list* replacement_list)
{
    struct token* _Opt token = replacement_list->head;

    if (token == NULL)
        return false;

    if (strcmp(token->lexeme, "(")) return false;
    token = token->next;

    if (token == NULL) return false;
    if (strcmp(token->lexeme, "(")) return false;
    token = token->next;

    if (token == NULL) return false;
    if (strcmp(token->lexeme, "void")) return false;
    token = token->next;

    if (token == NULL) return false;
    if (strcmp(token->lexeme, ")")) return false;
    token = token->next;

    if (token == NULL) return false;
    if (strcmp(token->lexeme, "0")) return false;
    token = token->next;

    if (token == NULL) return false;
    if (strcmp(token->lexeme, ")")) return false;
    token = token->next;

    if (token != NULL) return false;

    return true;
}


struct token_list control_line(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{

    /*
        control-line:
            # "include" pp-tokens new-line
            # "embed" pp-tokens new-line
            # "define" identifier replacement-list new-line
            # "define" identifier ( identifier-list _Opt ) replacement-list new-line
            # "define" identifier ( ... ) replacement-list new-line
            # "define" identifier ( identifier-list , ... ) replacement-list new-line
            # "undef" identifier new-line
            # "line" pp-tokens new-line
            # "error" pp-tokens _Opt new-line
            # "warning" pp-tokens _Opt new-line
            # "pragma" pp-tokens _Opt new-line
            # new-line
    */

    struct token_list r = { 0 };

    try
    {
        if (!is_active)
        {
            //se nao esta ativo eh ignorado
            struct token_list r7 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r7);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
            token_list_destroy(&r7);
            return r;
        }

        if (input_list->head == NULL)
        {
            pre_unexpected_end_of_file(r.tail, ctx);
            throw;
        }

        //struct token* const ptoken = input_list->head;
        match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);

        if (input_list->head == NULL)
        {
            pre_unexpected_end_of_file(r.tail, ctx);
            throw;
        }

        if (strcmp(input_list->head->lexeme, "include") == 0 ||
            strcmp(input_list->head->lexeme, "include_next") == 0)
        {
            bool include_next = strcmp(input_list->head->lexeme, "include_next") == 0;
            /*
              # include pp-tokens new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //include
            skip_blanks_level(ctx, &r, input_list, level);

            if (input_list->head == NULL)
            {
                pre_unexpected_end_of_file(r.tail, ctx);
                throw;
            }

            char path[100] = { 0 };
            bool is_angle_bracket_form = false;
            if (input_list->head->type == TK_STRING_LITERAL)
            {
                strcat(path, input_list->head->lexeme);
                prematch_level(&r, input_list, level);
            }
            else
            {
                is_angle_bracket_form = true;
                while (input_list->head->type != '>')
                {
                    strcat(path, input_list->head->lexeme);
                    prematch_level(&r, input_list, level);

                    if (input_list->head == NULL)
                    {
                        pre_unexpected_end_of_file(r.tail, ctx);
                        throw;
                    }
                }
                strcat(path, input_list->head->lexeme);
                prematch_level(&r, input_list, level);
            }


            while (input_list->head->type != TK_NEWLINE)
            {
                prematch_level(&r, input_list, level);
                if (input_list->head == NULL)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    throw;
                }
            }

            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);

            path[strlen(path) - 1] = '\0';

            /*this is the dir of the current file*/
            char current_file_dir[300] = { 0 };
            snprintf(current_file_dir, sizeof current_file_dir, "%s", r.tail->token_origin->lexeme);
            dirname(current_file_dir);


            char full_path_result[200] = { 0 };
            bool already_included = false;
            const char* _Owner _Opt content = find_and_read_include_file(ctx,
                path + 1,
                current_file_dir,
                is_angle_bracket_form,
                &already_included,
                full_path_result,
                sizeof full_path_result,
                include_next);

            if (content != NULL)
            {
                if (ctx->options.show_includes)
                {
                    for (int i = 0; i < (level + 1); i++)
                        printf(".");
                    printf("%s\n", full_path_result);
                }

                struct tokenizer_ctx tctx = { 0 };
                struct token_list list = tokenizer(&tctx, content, full_path_result, level + 1, TK_FLAG_NONE);
                free((void* _Owner)content);

                struct token_list list2 = preprocessor(ctx, &list, level + 1);
                token_list_append_list(&r, &list2);

                token_list_destroy(&list2);
                token_list_destroy(&list);
            }
            else
            {
                if (!already_included)
                {
                    preprocessor_diagnostic(C_ERROR_FILE_NOT_FOUND, ctx, r.tail, "file %s not found", path + 1);

                    for (struct include_dir* _Opt p = ctx->include_dir.head; p; p = p->next)
                    {
                        /*let's print the include path*/
                        preprocessor_diagnostic(W_NOTE, ctx, r.tail, "dir = '%s'", p->path);
                    }
                }
                else
                {
                    //pragma once..
                }
            }

        }
        else if (strcmp(input_list->head->lexeme, "embed") == 0)
        {
            struct token_list discard0 = { 0 };
            struct token_list* p_list = &r;

            /*
              C23
              # embed pp-tokens new-line
            */

            const struct token* const p_embed_token = input_list->head;

            match_token_level(p_list, input_list, TK_IDENTIFIER, level, ctx); //embed

            skip_blanks_level(ctx, p_list, input_list, level);


            if (input_list->head == NULL)
            {
                throw;
            }

            char path[100] = { 0 };
            if (input_list->head->type == TK_STRING_LITERAL)
            {
                strcat(path, input_list->head->lexeme);
                prematch_level(p_list, input_list, level);
            }
            else
            {
                while (input_list->head->type != '>')
                {
                    strcat(path, input_list->head->lexeme);
                    prematch_level(p_list, input_list, level);

                    if (input_list->head == NULL)
                    {
                        throw;
                    }
                }
                strcat(path, input_list->head->lexeme);
                prematch_level(p_list, input_list, level);
            }

            if (input_list->head)
            {
                while (input_list->head->type != TK_NEWLINE)
                {
                    prematch_level(p_list, input_list, level);
                    if (input_list->head == NULL)
                    {
                        pre_unexpected_end_of_file(p_list->tail, ctx);
                        throw;
                    }
                }
            }
            match_token_level(p_list, input_list, TK_NEWLINE, level, ctx);

            char fullpath[300] = { 0 };
            path[strlen(path) - 1] = '\0';

            snprintf(fullpath, sizeof(fullpath), "%s", path + 1);


            int nlevel = level;

            enum token_flags f = 0;

            f = TK_FLAG_FINAL;
            //we cannot see it just like include
            nlevel = nlevel + 1;

            struct token_list list = embed_tokenizer(ctx, p_embed_token, fullpath, nlevel, f);

            if (ctx->n_errors > 0)
            {
                token_list_destroy(&list);
                throw;
            }

            token_list_append_list(&r, &list);
            token_list_destroy(&list);
            token_list_destroy(&discard0);
        }
        else if (strcmp(input_list->head->lexeme, "define") == 0)
        {
            //TODO strcmp nao pode ser usado temos que criar uma funcao especial

            /*
             #de\
             fine A 1

            A
            */

            struct macro* _Owner _Opt macro = calloc(1, sizeof * macro);
            if (macro == NULL)
            {
                preprocessor_diagnostic(C_ERROR_UNEXPECTED, ctx, ctx->current, "out of mem");
                throw;
            }

            /*
                # define identifier                           replacement-list new-line
                # define identifier ( identifier-list_opt )    replacement-list new-line
                # define identifier ( ... )                   replacement-list new-line
                # define identifier ( identifier-list , ... ) replacement-list new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //define
            skip_blanks_level(ctx, &r, input_list, level);

            if (input_list->head == NULL)
            {
                macro_delete(macro);
                pre_unexpected_end_of_file(r.tail, ctx);
                throw;
            }

            struct token* macro_name_token = input_list->head;

            if (is_builtin_macro(macro_name_token->lexeme))
            {
                preprocessor_diagnostic(W_REDEFINING_BUITIN_MACRO,
                    ctx,
                    input_list->head,
                    "redefining builtin macro");
            }

            macro->p_name_token = macro_name_token;

            char* _Owner _Opt temp = strdup(input_list->head->lexeme);
            if (temp == NULL)
            {
                macro_delete(macro);
                throw;
            }
            assert(macro->name == NULL);
            macro->name = temp;


            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //nome da macro

            if (input_list->head == NULL)
            {
                macro_delete(macro);
                pre_unexpected_end_of_file(r.tail, ctx);
                throw;
            }

            if (input_list->head->type == '(')
            {
                macro->is_function = true;

                match_token_level(&r, input_list, '(', level, ctx);
                skip_blanks_level(ctx, &r, input_list, level);

                if (input_list->head == NULL)
                {
                    macro_delete(macro);
                    pre_unexpected_end_of_file(r.tail, ctx);
                    throw;
                }

                if (input_list->head->type == '...')
                {
                    struct macro_parameter* _Owner _Opt p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
                    if (p_macro_parameter == NULL)
                    {
                        macro_delete(macro);
                        throw;
                    }

                    char* _Owner _Opt temp2 = strdup("__VA_ARGS__");
                    if (temp2 == NULL)
                    {
                        macro_delete(macro);
                        macro_parameters_delete(p_macro_parameter);
                        throw;
                    }

                    p_macro_parameter->name = temp2;
                    macro->parameters = p_macro_parameter;

                    // assert(false);
                    match_token_level(&r, input_list, '...', level, ctx); //nome da macro
                    skip_blanks_level(ctx, &r, input_list, level);
                    match_token_level(&r, input_list, ')', level, ctx); //nome da macro
                }
                else if (input_list->head->type == ')')
                {
                    match_token_level(&r, input_list, ')', level, ctx);
                    skip_blanks_level(ctx, &r, input_list, level);
                }
                else
                {

                    struct token_list r3 = identifier_list(ctx, macro, input_list, level);
                    token_list_append_list(&r, &r3);
                    token_list_destroy(&r3);

                    skip_blanks_level(ctx, &r, input_list, level);
                    if (input_list->head == NULL)
                    {
                        macro_delete(macro);
                        pre_unexpected_end_of_file(r.tail, ctx);
                        throw;
                    }

                    if (input_list->head->type == '...')
                    {
                        struct macro_parameter* _Owner _Opt p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
                        if (p_macro_parameter == NULL)
                        {
                            macro_delete(macro);
                            throw;
                        }

                        char* _Owner _Opt temp3 = strdup("__VA_ARGS__");
                        if (temp3 == NULL)
                        {
                            macro_delete(macro);
                            macro_parameters_delete(p_macro_parameter);
                            throw;
                        }

                        p_macro_parameter->name = temp3;
                        struct macro_parameter* _Opt p_last = macro->parameters;
                        assert(p_last != NULL);
                        while (p_last->next)
                        {
                            p_last = p_last->next;
                        }
                        p_last->next = p_macro_parameter;

                        match_token_level(&r, input_list, '...', level, ctx);
                    }
                    skip_blanks_level(ctx, &r, input_list, level);
                    match_token_level(&r, input_list, ')', level, ctx);
                }
            }
            else
            {
                macro->is_function = false;
            }
            skip_blanks_level(ctx, &r, input_list, level);
            if (input_list->head == NULL)
            {
                macro_delete(macro);
                pre_unexpected_end_of_file(r.tail, ctx);
                throw;
            }
            struct token_list r4 = replacement_list(ctx, macro, input_list, level);
            token_list_append_list(&r, &r4);
            token_list_destroy(&r4);

            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
            if (!ctx->options.disable_assert && strcmp(macro->name, "assert") == 0)
            {
                //cake overrides macro assert in debug and release to be defined as 
                //assert(__VA_ARGS__)
                if (!is_empty_assert(&macro->replacement_list))
                {
                    macro_parameters_delete(macro->parameters);

                    struct macro_parameter* _Owner _Opt p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
                    if (p_macro_parameter == NULL)
                    {
                        macro_delete(macro);
                        throw;
                    }

                    char* _Owner _Opt temp2 = strdup("__VA_ARGS__");
                    if (temp2 == NULL)
                    {
                        macro_delete(macro);
                        macro_parameters_delete(p_macro_parameter);
                        throw;
                    }
                    p_macro_parameter->name = temp2;
                    macro->parameters = p_macro_parameter;

                    token_list_destroy(&macro->replacement_list);
                    struct tokenizer_ctx tctx = { 0 };
                    macro->replacement_list = tokenizer(&tctx, "assert(__VA_ARGS__)", NULL, level, TK_FLAG_NONE);
                }
            }

            naming_convention_macro(ctx, macro_name_token);

            struct macro* existing_macro = find_macro(ctx, macro->name);
            if (existing_macro)
            {
                if (!macro_is_same(macro, existing_macro))
                {
                    preprocessor_diagnostic(C_ERROR_MACRO_REDEFINITION,
                    ctx,
                    macro->p_name_token,
                    "macro redefinition");

                    preprocessor_diagnostic(W_NOTE,
                    ctx,
                    existing_macro->p_name_token,
                    "previous definition");

                    macro_delete(macro);
                    throw;
                }
                macro_delete(macro);
            }
            else
            {
                struct hash_item_set item = { 0 };
                item.p_macro = macro;
                hashmap_set(&ctx->macros, macro->name, &item);
                hash_item_set_destroy(&item);
            }
        }
        else if (strcmp(input_list->head->lexeme, "undef") == 0)
        {
            /*
             # undef identifier new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//undef
            skip_blanks_level(ctx, &r, input_list, level);

            if (input_list->head == NULL)
            {
                pre_unexpected_end_of_file(r.tail, ctx);
                throw;
            }

            struct macro* _Owner _Opt macro = (struct macro* _Owner _Opt) hashmap_remove(&ctx->macros, input_list->head->lexeme, NULL);
            assert(find_macro(ctx, input_list->head->lexeme) == NULL);
            if (macro)
            {
                macro_delete(macro);
                match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//undef
            }
            else
            {
                match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//undef
                /*no warnings*/
            }
            skip_blanks_level(ctx, &r, input_list, level);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
        }
        else if (strcmp(input_list->head->lexeme, "line") == 0)
        {
            /*
               # line pp-tokens new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//line
            struct token_list r5 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r5);
            token_list_destroy(&r5);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
        }
        else if (strcmp(input_list->head->lexeme, "error") == 0)
        {
            /*
              # error pp-tokensopt new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//error
            struct token_list r6 = pp_tokens_opt(ctx, input_list, level);

            token_list_append_list(&r, &r6);
            token_list_destroy(&r6);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
            preprocessor_diagnostic(C_ERROR_PREPROCESSOR_C_ERROR_DIRECTIVE, ctx, r.head, "#error");


        }
        else if (strcmp(input_list->head->lexeme, "warning") == 0)
        {
            /*
              # warning pp-tokensopt new-line
            */

            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//warning
            struct token_list r6 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r6);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
            preprocessor_diagnostic(W_WARNING_DIRECTIVE, ctx, r.head, "#warning");
            token_list_destroy(&r6);
        }
        else if (strcmp(input_list->head->lexeme, "pragma") == 0)
        {
            /*
              # pragma pp-tokensopt new-line
            */
            /*
               #pragma will survive and compiler will handle as
               pragma declaration
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma

            if (r.tail)
            {
                r.tail->type = TK_PRAGMA;
                r.tail->flags |= TK_FLAG_FINAL;
            }
            skip_blanks_level(ctx, &r, input_list, level);

            if (input_list->head == NULL)
            {
                pre_unexpected_end_of_file(r.tail, ctx);
                throw;
            }

            if (input_list->head->type == TK_IDENTIFIER)
            {
                if (strcmp(input_list->head->lexeme, "CAKE") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
                    if (r.tail)
                    {
                        r.tail->flags |= TK_FLAG_FINAL;
                    }
                    skip_blanks_level(ctx, &r, input_list, level);
                }

                if (input_list->head == NULL)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    throw;
                }

                if (strcmp(input_list->head->lexeme, "once") == 0)
                {
                    pragma_once_add(ctx, input_list->head->token_origin->lexeme);
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma
                    if (r.tail)
                    {
                        r.tail->flags |= TK_FLAG_FINAL;
                    }
                }
                else if (strcmp(input_list->head->lexeme, "dir") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma
                    skip_blanks_level(ctx, &r, input_list, level);

                    if (input_list->head == NULL)
                    {
                        pre_unexpected_end_of_file(r.tail, ctx);
                        throw;
                    }

                    if (input_list->head->type != TK_STRING_LITERAL)
                    {
                        preprocessor_diagnostic(C_ERROR_UNEXPECTED, ctx, input_list->head, "expected string");
                        throw;
                    }

                    char path[200] = { 0 };
                    strncpy(path, input_list->head->lexeme + 1, strlen(input_list->head->lexeme) - 2);
                    include_dir_add(&ctx->include_dir, path);
                    match_token_level(&r, input_list, TK_STRING_LITERAL, level, ctx);//pragma
                    if (r.tail)
                    {
                        r.tail->flags |= TK_FLAG_FINAL;
                    }
                }
                else if (strcmp(input_list->head->lexeme, "nullchecks") == 0)
                {
                    assert(false);
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//nullchecks
                    assert(r.tail != NULL);
                    r.tail->flags |= TK_FLAG_FINAL;

                    skip_blanks_level(ctx, &r, input_list, level);
                    ctx->options.null_checks_enabled = true;
                }

                if (input_list->head == NULL)
                {
                    pre_unexpected_end_of_file(r.tail, ctx);
                    throw;
                }

                if (strcmp(input_list->head->lexeme, "diagnostic") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//diagnostic
                    assert(r.tail != NULL);
                    r.tail->flags |= TK_FLAG_FINAL;

                    skip_blanks_level(ctx, &r, input_list, level);

                    if (input_list->head == NULL)
                    {
                        pre_unexpected_end_of_file(r.tail, ctx);
                        throw;
                    }

                    if (strcmp(input_list->head->lexeme, "push") == 0)
                    {
                        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//diagnostic
                        assert(r.tail != NULL);
                        r.tail->flags |= TK_FLAG_FINAL;

                        //#pragma GCC diagnostic push
                        if (ctx->options.diagnostic_stack.top_index <
                            sizeof(ctx->options.diagnostic_stack) / sizeof(ctx->options.diagnostic_stack.stack[0]))
                        {
                            ctx->options.diagnostic_stack.top_index++;

                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] =
                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index - 1];
                        }
                    }
                    else if (strcmp(input_list->head->lexeme, "pop") == 0)
                    {
                        //#pragma GCC diagnostic pop
                        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pop
                        assert(r.tail != NULL);
                        r.tail->flags |= TK_FLAG_FINAL;
                        if (ctx->options.diagnostic_stack.top_index > 0)
                        {
                            ctx->options.diagnostic_stack.top_index--;
                        }
                    }
                    else if (strcmp(input_list->head->lexeme, "warning") == 0)
                    {
                        //#pragma CAKE diagnostic warning "-Wenum-compare"

                        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//warning
                        assert(r.tail != NULL);
                        r.tail->flags |= TK_FLAG_FINAL;
                        skip_blanks_level(ctx, &r, input_list, level);

                        if (input_list->head && input_list->head->type == TK_STRING_LITERAL)
                        {
                            match_token_level(&r, input_list, TK_STRING_LITERAL, level, ctx);//""
                            assert(r.tail != NULL);
                            r.tail->flags |= TK_FLAG_FINAL;

                            unsigned long long  w = get_warning_bit_mask(input_list->head->lexeme + 1 + 2);
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
                        }
                    }
                    else if (strcmp(input_list->head->lexeme, "ignore") == 0)
                    {
                        //#pragma CAKE diagnostic ignore "-Wenum-compare"

                        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//ignore
                        assert(r.tail != NULL);
                        r.tail->flags |= TK_FLAG_FINAL;

                        skip_blanks_level(ctx, &r, input_list, level);

                        if (input_list->head && input_list->head->type == TK_STRING_LITERAL)
                        {
                            unsigned long long w = get_warning_bit_mask(input_list->head->lexeme + 1 + 2);
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
                        }
                    }
                }
            }

            struct token_list r7 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r7);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
            assert(r.tail != NULL);
            r.tail->type = TK_PRAGMA_END;
            r.tail->flags |= TK_FLAG_FINAL;
            token_list_destroy(&r7);
        }
        else if (input_list->head->type == TK_NEWLINE)
        {
            skip_blanks_level(ctx, &r, input_list, level);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
        }
        else
        {
            //handled by the caller
            preprocessor_diagnostic(C_ERROR_UNEXPECTED_TOKEN,
                ctx,
                input_list->head,
                "unexpected\n");
            throw;
        }
    }
    catch
    {
    }

    return r;
}


static struct token_list non_directive(struct preprocessor_ctx* ctx, struct token_list* input_list, int level)
{
    /*
      non-directive:
      pp-tokens new-line
     */
    struct token_list r = pp_tokens_opt(ctx, input_list, level);
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
    return r;
}

static struct macro_argument_list collect_macro_arguments(struct preprocessor_ctx* ctx,
    struct macro* macro,
    struct token_list* input_list, int level)
{

    struct macro_argument_list macro_argument_list = { 0 };

    try
    {
        if (input_list->head == NULL)
        {
            throw;
        }

        assert(input_list->head->type == TK_IDENTIFIER);//macro name
        const struct token* const macro_name_token = input_list->head;

        match_token_level(&macro_argument_list.tokens, input_list, TK_IDENTIFIER, level, ctx); //MACRO NAME

        if (!macro->is_function)
        {
            //This function is also called for non function like macros.
            //In this case we return empty
            return macro_argument_list;
        }


        int count = 1;

        //skip spaces after macro name
        skip_blanks(ctx, &macro_argument_list.tokens, input_list);

        //macro is a function
        match_token_level(&macro_argument_list.tokens, input_list, '(', level, ctx);

        //skip spaces after (
        skip_blanks(ctx, &macro_argument_list.tokens, input_list);

        if (input_list->head == NULL)
        {
            pre_unexpected_end_of_file(macro_argument_list.tokens.tail, ctx);
            throw;
        }

        if (input_list->head->type == ')')
        {
            /*
               empty argument list
            */

            if (macro->parameters != NULL)
            {
                struct macro_argument* _Owner _Opt  p_argument = calloc(1, sizeof(struct macro_argument));
                if (p_argument == NULL)
                {
                    throw;
                }
                struct macro_parameter* p_current_parameter = macro->parameters;
                p_argument->macro_parameter = p_current_parameter;
                argument_list_add(&macro_argument_list, p_argument);
            }
            match_token_level(&macro_argument_list.tokens, input_list, ')', level, ctx);
            return macro_argument_list;
        }

        if (macro->parameters == NULL)
        {
            //we have a non empty argument list, calling a macro without parameters
            preprocessor_diagnostic(C_ERROR_TOO_MANY_ARGUMENTS_TO_FUNCTION_LIKE_MACRO,
                ctx,
                macro_name_token, "too many arguments provided to function-like macro invocation\n");
            throw;
        }

        struct macro_parameter* p_current_parameter = macro->parameters;
        struct macro_argument* _Owner _Opt p_argument = calloc(1, sizeof(struct macro_argument));
        if (p_argument == NULL)
        {
            throw;
        }

        p_argument->macro_parameter = p_current_parameter;
        //collect next arguments...
        while (input_list->head != NULL)
        {
            if (input_list->head->type == '(')
            {
                count++;
                token_list_clone_and_add(&p_argument->tokens, input_list->head);
                match_token_level(&macro_argument_list.tokens, input_list, '(', level, ctx);
            }
            else if (input_list->head->type == ')')
            {
                count--;
                if (count == 0)
                {
                    match_token_level(&macro_argument_list.tokens, input_list, ')', level, ctx);
                    argument_list_add(&macro_argument_list, p_argument);
                    p_argument = NULL; //MOVED

                    if (p_current_parameter->next != NULL)
                    {
                        p_current_parameter = p_current_parameter->next;
                        if (strcmp(p_current_parameter->name, "__VA_ARGS__") == 0)
                        {
                            //we add this argument as being empty
                            p_argument = calloc(1, sizeof(struct macro_argument));
                            if (p_argument == NULL)
                            {
                                throw;
                            }

                            p_argument->macro_parameter = p_current_parameter;
                            argument_list_add(&macro_argument_list, p_argument);
                            p_argument = NULL; //MOVED
                        }
                        else
                        {
                            preprocessor_diagnostic(C_ERROR_TOO_FEW_ARGUMENTS_TO_FUNCTION_LIKE_MACRO,
                                ctx,
                                macro_name_token,
                                "too few arguments provided to function-like macro invocation\n");
                            throw;
                        }
                    }

                    break;
                }
                else
                {
                    token_list_clone_and_add(&p_argument->tokens, input_list->head);
                    match_token_level(&macro_argument_list.tokens, input_list, ')', level, ctx);
                }
            }
            else if (count == 1 && input_list->head->type == ',')
            {
                if (strcmp(p_current_parameter->name, "__VA_ARGS__") == 0)
                {
                    token_list_clone_and_add(&p_argument->tokens, input_list->head);
                    match_token_level(&macro_argument_list.tokens, input_list, ',', level, ctx);
                }
                else //if (count == 1)
                {
                    match_token_level(&macro_argument_list.tokens, input_list, ',', level, ctx);
                    argument_list_add(&macro_argument_list, p_argument);
                    p_argument = NULL; /*MOVED*/

                    p_argument = calloc(1, sizeof(struct macro_argument));
                    if (p_argument == NULL)
                    {
                        throw;
                    }

                    if (p_current_parameter->next == NULL)
                    {
                        preprocessor_diagnostic(C_ERROR_TOO_MANY_ARGUMENTS_TO_FUNCTION_LIKE_MACRO,
                            ctx,
                            macro_argument_list.tokens.tail,
                            "too many arguments provided to function-like macro invocation\n");
                        macro_argument_delete(p_argument);
                        p_argument = NULL; //DELETED
                        throw;
                    }

                    p_current_parameter = p_current_parameter->next;

                    p_argument->macro_parameter = p_current_parameter;
                }
            }
            else
            {
                token_list_clone_and_add(&p_argument->tokens, input_list->head);
                prematch_level(&macro_argument_list.tokens, input_list, level);
                //token_list_add(&list, token_list_pop_front(input_list));
            }
        }

        assert(p_argument == NULL);
    }
    catch
    {
    }

    return macro_argument_list;
}

struct token_list expand_macro(struct preprocessor_ctx* ctx, struct macro_expanded* _Opt p_list, struct macro* macro, struct macro_argument_list* arguments, int level, const struct token* origin);
struct token_list replacement_list_reexamination(struct preprocessor_ctx* ctx, struct macro_expanded* p_list, struct token_list* oldlist, int level, const struct token* origin);


struct token_list macro_copy_replacement_list(struct preprocessor_ctx* ctx, struct macro* macro, const struct token* origin);

/*#define hash_hash # ## #
#define mkstr(a) # a
#define in_between(a) mkstr(a)
#define join(c, d) in_between(c hash_hash d)

hash_hash

join(x, y)
*/
static struct token_list concatenate(struct preprocessor_ctx* ctx, struct token_list* input_list)
{
    //printf("input="); print_list(input_list);

    struct token_list  r = { 0 };
    try
    {
        //todo juntar tokens mesmo objet macro
        //struct token* p_previousNonBlank = 0;
        while (input_list->head)
        {
            //printf("r="); print_list(&r);
            //printf("input="); print_list(input_list);

            //#def macro
            //assert(!(input_list->head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
            if (input_list->head->type == '##')
            {
                if (r.tail == NULL)
                {
                    preprocessor_diagnostic(C_ERROR_PREPROCESSOR_MISSING_MACRO_ARGUMENT,
                        ctx,
                        input_list->head, "missing macro argument (should be checked before)");
                    break;
                }
                /*
                * arranca ## do input (sem adicionar)
                */
                token_list_pop_front(input_list);

                struct osstream ss = { 0 };

                /*
                *  Faz uma string com o fim r + começo do input (## ja foi removido)
                */
                if (r.tail->lexeme[0] != '\0')
                    ss_fprintf(&ss, "%s", r.tail->lexeme);

                if (input_list->head && input_list->head->lexeme[0] != '\0')
                    ss_fprintf(&ss, "%s", input_list->head->lexeme);


                //copiar o level para gerar um novo igual
                int level = input_list->head ? input_list->head->level : 0;

                /*
                * Já paga do input o token usado na concatenacao
                */
                token_list_pop_front(input_list);

                /*
                * Faz um novo token com a string montada
                */
                struct tokenizer_ctx tctx = { 0 };
                struct token_list newlist = { 0 };

                if (ss.c_str != NULL)
                {
                    newlist = tokenizer(&tctx, ss.c_str, NULL, level, TK_FLAG_NONE);
                }

                if (newlist.head)
                {
                    newlist.head->flags = r.tail->flags;
                }
                else
                {
                    struct token* _Owner _Opt p_new_token = calloc(1, sizeof * p_new_token);
                    if (p_new_token == NULL)
                    {
                        ss_close(&ss);
                        throw;
                    }
                    char* _Owner _Opt temp = strdup("");
                    if (temp == NULL)
                    {
                        ss_close(&ss);
                        token_delete(p_new_token);
                        throw;
                    }
                    p_new_token->lexeme = temp;
                    p_new_token->type = TK_PLACEMARKER;
                    token_list_add(&newlist, p_new_token);
                    assert(newlist.head != NULL);
                    newlist.head->flags = r.tail->flags;
                }
                /*
                * Arranca o anterior do r que foi usado para formar string
                */
                token_list_pop_back(&r);

                /*adiciona novo token no fim do r*/
                token_list_append_list(&r, &newlist);

                ss_close(&ss);

                token_list_destroy(&newlist);

                if (input_list->head == NULL)
                    break;
            }
            else
            {
                prematch(&r, input_list);
            }
        }
    }
    catch
    {
    }

    return r;
}

/*
  check if the argument list that corresponds to a trailing ...
  of the parameter list is present and has a non-empty substitution.
*/
static bool has_argument_list_empty_substitution(struct preprocessor_ctx* ctx,
    struct macro_expanded* p_list,
    struct macro_argument_list* p_macro_argument_list,
    const struct token* origin)
{
    if (p_macro_argument_list->head == NULL)
        return true;

    struct macro_argument* _Opt p_va_args_argument =
        find_macro_argument_by_name(p_macro_argument_list, "__VA_ARGS__");

    if (p_va_args_argument)
    {
        if (p_va_args_argument->tokens.head == NULL)
            return true;

        struct token_list argumentlist = copy_argument_list(p_va_args_argument);

        struct token_list r4 = replacement_list_reexamination(ctx, p_list, &argumentlist, 0, origin);
        const bool results_in_empty_substituition = (r4.head == NULL || r4.head->type == TK_PLACEMARKER);
        token_list_destroy(&r4);

        token_list_destroy(&argumentlist);

        return results_in_empty_substituition;
    }

    return false;
}

static struct token_list replace_macro_arguments(struct preprocessor_ctx* ctx, struct macro_expanded* p_list, struct token_list* input_list, struct macro_argument_list* arguments, const struct token* origin)
{
    struct token_list r = { 0 };

    try
    {
        /*clear previous usage*/
        struct macro_argument* _Owner _Opt p = arguments->head;
        while (p)
        {
            struct macro_argument* _Owner _Opt next = p->next;
            if (p->macro_parameter)
            {
                p->macro_parameter->already_expanded = false;
                token_list_clear(&p->macro_parameter->expanded_list);
            }
            p = next;
        }

        while (input_list->head)
        {
            assert(!(input_list->head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
            assert(!token_is_blank(input_list->head));
            assert(r.tail == NULL || !token_is_blank(r.tail));
            struct macro_argument* _Opt p_argument = NULL;
            if (input_list->head->type == TK_IDENTIFIER)
            {
                if (strcmp(input_list->head->lexeme, "__VA_OPT__") == 0)
                {
                    token_list_pop_front(input_list); //pop __VA_OPT__
                    token_list_pop_front(input_list); //pop (
                    int parenteses_count = 1;         //we already have one

                    const bool discard_va_opt =
                        has_argument_list_empty_substitution(ctx, p_list, arguments, origin);

                    if (discard_va_opt)
                    {
                        //discard all tokens __VA_OPT__(...)
                        while (input_list->head)
                        {
                            if (input_list->head->type == '(') parenteses_count++;
                            else if (input_list->head->type == ')') parenteses_count--;
                            token_list_pop_front(input_list);
                            if (parenteses_count == 0)
                                break;
                        }
                    }
                    else
                    {
                        // Search and remove the last balanced ')'
                        struct token* _Opt p_token = input_list->head;
                        for (; p_token; p_token = p_token->next)
                        {
                            if (p_token->type == '(') parenteses_count++;
                            else if (p_token->type == ')') parenteses_count--;

                            if (parenteses_count == 0)
                                break;
                        }
                        token_list_remove(input_list, p_token, p_token);
                    }
                    continue;
                }

                p_argument = find_macro_argument_by_name(arguments, input_list->head->lexeme);
            }

            if (p_argument)
            {
                if (r.tail != NULL && r.tail->type == '#')
                {

                    /*
                      deleta nome parametro da lista
                      antes copia flags dele
                    */

                    const enum token_flags flags = r.tail->flags;
                    token_list_pop_front(input_list);

                    //deleta tambem # do fim
                    while (token_is_blank(r.tail))
                    {
                        token_list_pop_back(&r);
                    }
                    token_list_pop_back(&r);

                    ///----------------------------
                    //transforma tudo em string e coloca no resultado
                    struct token_list argumentlist = copy_argument_list(p_argument);
                    char* _Owner _Opt s = token_list_join_tokens(&argumentlist, true);
                    if (s == NULL)
                    {
                        token_list_destroy(&argumentlist);
                        preprocessor_diagnostic(C_ERROR_UNEXPECTED,
                            ctx,
                            input_list->head, "unexpected");
                        throw;
                    }
                    struct token* _Owner _Opt p_new_token = calloc(1, sizeof * p_new_token);

                    if (p_new_token == NULL)
                    {
                        free(s);
                        token_list_destroy(&argumentlist);
                        throw;
                    }

                    p_new_token->lexeme = s;
                    p_new_token->type = TK_STRING_LITERAL;
                    p_new_token->flags = flags;
                    token_list_add(&r, p_new_token);
                    token_list_destroy(&argumentlist);
                    continue;
                }
                else if (r.tail != NULL && r.tail->type == '##')
                {
                    //estou parametro e anterior era ##
                    token_list_pop_front(input_list);
                    struct token_list argumentlist = copy_argument_list(p_argument);
                    token_list_append_list(&r, &argumentlist);
                    token_list_destroy(&argumentlist);
                }
                else if (input_list->head->next && input_list->head->next->type == '##')
                {
                    //estou no parametro e o da frente eh ##
                    int flags = input_list->head->flags;
                    //tira nome parametro a lista
                    token_list_pop_front(input_list);
                    //passa tudo p resultado
                    struct token_list argumentlist = copy_argument_list(p_argument);
                    if (argumentlist.head != NULL)
                    {
                        argumentlist.head->flags = flags;
                    }
                    token_list_append_list(&r, &argumentlist);
                    // ja passa o ## tambem
                    prematch(&r, input_list);
                    token_list_destroy(&argumentlist);
                }
                else
                {
                    const int flags = input_list->head->flags;

                    //remove nome parametro do input
                    token_list_pop_front(input_list);

                    if (p_argument->macro_parameter == NULL)
                    {
                        throw;
                    }

                    if (!p_argument->macro_parameter->already_expanded)
                    {
                        /*
                          https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3457.htm#number-of-expansions
                          For each such parameter this expansion is performed exactly once,
                          and then preprocessing tokens naming the parameter are each replaced
                          with the resulting token list.
                        */
                        struct token_list copy_list = copy_argument_list(p_argument);
                        struct token_list r4 = replacement_list_reexamination(ctx, p_list, &copy_list, 0, origin);
                        token_list_swap(&p_argument->macro_parameter->expanded_list, &r4);
                        token_list_destroy(&r4);
                        p_argument->macro_parameter->already_expanded = true;
                    }

                    //Use the previous expansion
                    struct token_list copy_list = copy_argument_list_tokens(&p_argument->macro_parameter->expanded_list);
                    if (copy_list.head)
                    {
                        //fix flags
                        copy_list.head->flags = flags;
                    }
                    token_list_append_list(&r, &copy_list);
                    token_list_destroy(&copy_list);
                    if (ctx->n_errors > 0)
                    {
                        throw;
                    }
                }
            }
            else
            {
                prematch(&r, input_list);
            }
        }
    }
    catch
    {
    }

    return r;
}

struct token_list concatenate(struct preprocessor_ctx* ctx, struct token_list* input_list);

static bool macro_already_expanded(struct macro_expanded* _Opt p_list, const char* name)
{
    struct macro_expanded* _Opt p_item = p_list;
    while (p_item)
    {
        if (strcmp(name, p_item->name) == 0)
        {
            return true;
        }
        p_item = p_item->p_previous;
    }
    return false;
}

static char* _Owner decode_pragma_string(const char* literal)
{
    /*
      The string literal is destringized
      - by deleting any encoding prefix,
      - deleting the leading and trailing double-quotes,
      - replacing each escape sequence \" by a double-quote,
      - and replacing each escape sequence \\ by a single backslash.
   */

    while (*literal != '"')
        literal++; //skip string prefix u8 etc

    literal++; //skip leading double-quotes

    size_t len = 0;
    const char* p = literal;

    // Compute the maximum possible length
    p = literal;
    while (*p && *p != '"')
    {
        if (*p == '\\' && *(p + 1))
        {
            p++;
            switch (*p)
            {
            case '"':
            case '\\':              
                break;

            default:
                len++;
            }
        }
        
        len++;        
        p++;
    }

    char* _Owner result = (char*)malloc(len + 1);
    if (!result) return NULL;
    
    char* out = result;
    p = literal;
    while (*p && *p != '"')
    {
        if (*p == '\\' && *(p + 1))
        {
            p++;
            switch (*p)
            {
            case '"':
            case '\\':
                break;

            default:
                *out++ = '\\';
                break;
            }
        }
        
        *out++ = *p;        
        p++;
    }
    *out = '\0';
    return result;
}


static struct token_list operator_pragma(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    struct token_list r = { 0 };
    try
    {
        if (input_list->head->type != TK_IDENTIFIER)
        {
            throw; //internal error
        }

        prematch(&r, input_list);
        r.tail->type = TK_PRAGMA;
        r.tail->flags |= TK_FLAG_FINAL;


        skip_blanks_level(ctx, &r, input_list, level);

        if (input_list->head->type != '(')
        {
            preprocessor_diagnostic(C_ERROR_UNEXPECTED_TOKEN,
            ctx,
            input_list->head,
            "expected (");
            throw; //internal error
        }

        token_list_pop_front(input_list); // (

        skip_blanks_level(ctx, &r, input_list, level);

        if (input_list->head->type != TK_STRING_LITERAL)
        {
            preprocessor_diagnostic(C_ERROR_UNEXPECTED_TOKEN,
            ctx,
            input_list->head,
            "expected string");
            throw; //internal error
        }

        char* _Owner line = decode_pragma_string(input_list->head->lexeme);
        if (line == NULL)
            throw;

        token_list_pop_front(input_list); // ""

        struct tokenizer_ctx tctx = { 0 };
        struct token_list r0 = tokenizer(&tctx, line, "", 0, TK_FLAG_NONE);
        free(line);

        token_list_pop_front(&r0); // (
        token_list_append_list(&r, &r0); //)    

        skip_blanks_level(ctx, &r, input_list, level);

        if (input_list->head->type != ')')
        {
            preprocessor_diagnostic(C_ERROR_UNEXPECTED_TOKEN,
            ctx,
            input_list->head,
            "expected (");
            throw; //internal error
        }

        prematch(&r, input_list); //)
        r.tail->type = TK_PRAGMA_END;
        r.tail->flags |= TK_FLAG_FINAL;
    }
    catch
    {
        token_list_clear(&r);
    }
    return r;
}

struct token_list replacement_list_reexamination(struct preprocessor_ctx* ctx,
    struct macro_expanded* p_list,
    struct token_list* oldlist,
    int level,
    const struct token* origin)
{
    struct token_list r = { 0 };
    try
    {
        //replacement_list_reexamination
        /*
        For both object-like and function-like macro invocations, before the replacement list is reexamined
        for more macro names to replace, each instance of a ## preprocessing token in the replacement list
        (not from an argument) is deleted and the preceding preprocessing token is concatenated with the
        following preprocessing token.
        */
        struct token_list new_list = concatenate(ctx, oldlist);
        while (new_list.head != NULL)
        {
            if (new_list.head->type == TK_IDENTIFIER &&
                strcmp(new_list.head->lexeme, "_Pragma") == 0)
            {
                struct token_list list = operator_pragma(ctx, &new_list, true, level);
                token_list_append_list(&new_list, &list);
                continue;
            }

            struct macro* _Opt macro = NULL;
            if (new_list.head->type == TK_IDENTIFIER)
            {
                macro = find_macro(ctx, new_list.head->lexeme);
                if (macro &&
                    macro->is_function &&
                    !preprocessor_token_ahead_is(new_list.head, '('))
                {
                    macro = NULL;
                }

                if (macro && macro_already_expanded(p_list, new_list.head->lexeme))
                {
                    new_list.head->type = TK_IDENTIFIER_RECURSIVE_MACRO;
                    macro = NULL;
                }


                if (ctx->conditional_inclusion)
                {
                    /*
                        When we are expanding in conditional inclusion the defined macro or defined (macro)
                        is not expanded and is considered later
                    */
                    if (r.tail &&
                        r.tail->type == TK_IDENTIFIER &&
                        strcmp(r.tail->lexeme, "defined") == 0)
                    {
                        macro = NULL;
                    }
                    else if (r.tail &&
                        r.tail->type == '(')
                    {
                        struct token* _Opt previous = r.tail->prev;
                        if (previous != NULL &&
                            previous->type == TK_IDENTIFIER &&
                            strcmp(previous->lexeme, "defined") == 0)
                        {
                            macro = NULL;
                        }
                    }
                }

            }
            if (macro)
            {
                int flags = new_list.head->flags;
                struct macro_argument_list arguments = collect_macro_arguments(ctx, macro, &new_list, level);
                if (ctx->n_errors > 0)
                {
                    macro_argument_list_destroy(&arguments);
                    token_list_destroy(&new_list);
                    throw;
                }


                struct token_list r3 = expand_macro(ctx, p_list, macro, &arguments, level, origin);
                if (ctx->n_errors > 0)
                {
                    token_list_destroy(&new_list);
                    token_list_destroy(&r3);
                    macro_argument_list_destroy(&arguments);
                    throw;
                }

                if (r3.head)
                {
                    r3.head->flags = flags;
                }
                token_list_append_list_at_beginning(&new_list, &r3);
                macro_argument_list_destroy(&arguments);
                token_list_destroy(&r3);
            }
            else
            {
                /*
                 This is a good place to set the level and macro flags
                 because there is always a macro rescan at the end
                */
                new_list.head->level = level;
                new_list.head->flags |= TK_FLAG_MACRO_EXPANDED;

                //OBS: #def macro have newlinew
                //assert(!(new_list.head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
                prematch(&r, &new_list); //it wasn't macro
            }
        }
    }
    catch
    {
    }

    return r;
}

/*
Performs the comparison ignoring the continuation of the line
TODO do a general review where strcmp is used in lexeme
and replace it with this one.
*/
int lexeme_cmp(const char* s1, const char* s2)
{
    while (*s1 && *s2)
    {

        while ((s1[0] == '\\' && s1[1] == '\n'))
        {
            s1++;
            s1++;
        }


        while (s2[0] == '\\' && s2[1] == '\n')
        {
            s2++;
            s2++;
        }

        if (*s1 != *s2)
            break;

        s1++;
        s2++;
    }

    while ((s1[0] == '\\' && s1[1] == '\n'))
    {
        s1++;
        s1++;
    }


    while (s2[0] == '\\' && s2[1] == '\n')
    {
        s2++;
        s2++;
    }

    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void remove_line_continuation(char* s)
{
    char* pread = s;
    char* pwrite = s;
    while (*pread)
    {
        if (pread[0] == '\\' &&
              (pread[1] == '\n' ||
                  (pread[1] == '\r' && pread[2] == '\n'))
            )
        {
            if (pread[1] == '\r' && pread[2] == '\n')
            {
                pread++;
                pread++;
                pread++;
            }
            else
            {
                pread++;
                pread++;
            }

        }
        else
        {
            *pwrite = *pread;
            pread++;
            pwrite++;
        }
    }
    *pwrite = *pread;
}

struct token_list  copy_replacement_list_core(struct preprocessor_ctx* ctx,
    const struct token_list* list,
    bool new_line_is_space)
{
    //Makes a copy of the tokens by trimming the beginning and end 
    //any space in comments etc. becomes a single space

    struct token_list r = { 0 };
    struct token* _Opt current = list->head;

    //get off all initial whites
    if (!new_line_is_space)
    {
        while (current && token_is_blank(current))
        {
            current = current->next;
        }
    }
    else
    {
        while (current && (token_is_blank(current) || current->type == TK_NEWLINE))
        {
            current = current->next;
        }
    }

    //remove space flag before if present
    bool is_first = true;

    for (; current;)
    {
        if (!new_line_is_space)
        {
            if (current && token_is_blank(current))
            {
                if (current == list->tail)
                    break;

                current = current->next;
                continue;
            }
        }
        else
        {
            if (current && (token_is_blank(current) || current->type == TK_NEWLINE))
            {
                if (current == list->tail)
                    break;

                current = current->next;
                continue;
            }
        }
        struct token* token_added = token_list_clone_and_add(&r, current);


        if (!ctx->options.preprocess_def_macro && token_added->type == TK_PREPROCESSOR_LINE)
        {
            token_added->type = '#';
            free(token_added->lexeme);
            token_added->lexeme = strdup("#");
        }

        if (token_added->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
        {
            token_added->flags = token_added->flags & ~TK_FLAG_HAS_NEWLINE_BEFORE;
            token_added->flags |= TK_FLAG_HAS_SPACE_BEFORE;
        }
        if (is_first)
        {
            token_added->flags = token_added->flags & ~TK_FLAG_HAS_SPACE_BEFORE;
            token_added->flags = token_added->flags & ~TK_FLAG_HAS_NEWLINE_BEFORE;
            is_first = false;
        }
        remove_line_continuation(token_added->lexeme);

        if (current == list->tail)
            break;
        current = current->next;

    }
    return r;
}

struct token_list  copy_replacement_list(struct preprocessor_ctx* ctx,
    const struct token_list* list)
{
    return copy_replacement_list_core(ctx, list, !ctx->options.preprocess_def_macro);
}

struct token_list macro_copy_replacement_list(struct preprocessor_ctx* ctx, struct macro* macro, const struct token* origin)
{
    /*dynamic content macros*/
    if (strcmp(macro->name, "__LINE__") == 0)
    {
        struct tokenizer_ctx tctx = { 0 };
        char line[50] = { 0 };

        assert(origin != NULL);
        snprintf(line, sizeof line, "%d", origin->line);

        struct token_list r = tokenizer(&tctx, line, "", 0, TK_FLAG_NONE);

        token_list_pop_front(&r);

        if (r.head != NULL)
        {
            r.head->flags = 0;
        }
        return r;
    }
    else if (strcmp(macro->name, "__FILE__") == 0)
    {
        char buffer[300] = { 0 };
        if (stringify(origin->token_origin->lexeme, sizeof buffer, buffer) < 0)
        {
            //ops TODO
        }

        struct tokenizer_ctx tctx = { 0 };
        struct token_list r = tokenizer(&tctx, buffer, "", 0, TK_FLAG_NONE);
        token_list_pop_front(&r);
        if (r.head)
        {
            r.head->flags = 0;
        }
        return r;
    }
    else if (strcmp(macro->name, "__COUNTER__") == 0)
    {
        char line[50] = { 0 };
        snprintf(line, sizeof line, "%d", ctx->count_macro_value);
        ctx->count_macro_value++;

        struct tokenizer_ctx tctx = { 0 };
        struct token_list r = tokenizer(&tctx, line, "", 0, TK_FLAG_NONE);
        token_list_pop_front(&r);

        if (r.head != NULL)
        {
            r.head->flags = 0;
        }
        return r;
    }

    return copy_replacement_list(ctx, &macro->replacement_list);
}

void print_literal2(const char* s);


struct token_list expand_macro(struct preprocessor_ctx* ctx,
    struct macro_expanded* _Opt p_list_of_macro_expanded_opt,
    struct macro* macro,
    struct macro_argument_list* arguments,
    int level,
    const struct token* origin)
{
    macro->usage++;

    struct token_list r = { 0 };
    try
    {
        assert(!macro_already_expanded(p_list_of_macro_expanded_opt, macro->name));
        _Opt struct macro_expanded macro_expanded = { 0 };
        macro_expanded.name = macro->name;
        macro_expanded.p_previous = p_list_of_macro_expanded_opt;
        if (macro->is_function)
        {
            struct token_list copy = macro_copy_replacement_list(ctx, macro, origin);
            struct token_list copy2 = replace_macro_arguments(ctx, &macro_expanded, &copy, arguments, origin);
            struct token_list r2 = replacement_list_reexamination(ctx, &macro_expanded, &copy2, level, origin);

            token_list_append_list(&r, &r2);

            token_list_destroy(&copy);
            token_list_destroy(&copy2);
            token_list_destroy(&r2);

            if (ctx->n_errors > 0) throw;
        }
        else
        {
            struct token_list copy = macro_copy_replacement_list(ctx, macro, origin);
            struct token_list r3 = replacement_list_reexamination(ctx, &macro_expanded, &copy, level, origin);
            if (ctx->n_errors > 0)
            {
                token_list_destroy(&copy);
                token_list_destroy(&r3);
                throw;
            }

            token_list_append_list(&r, &r3);
            token_list_destroy(&copy);
            token_list_destroy(&r3);
        }

        if (ctx->n_errors > 0) throw;

        if (ctx->options.preprocess_def_macro && macro->def_macro)
        {
            struct token_list r0 = { 0 };
            token_list_append_list(&r0, &r);

            struct token_list list2 = preprocessor(ctx, &r0, level + 1);
            struct tokenizer_ctx tctx = { 0 };
            const char* _Opt _Owner result = print_preprocessed_to_string2(list2.head);

            token_list_clear(&r);
            r = tokenizer(&tctx, result, "", 0, TK_FLAG_MACRO_EXPANDED);
            struct token_list list3 = copy_replacement_list_core(ctx, &r, true);
            token_list_swap(&list3, &r);
            free((void*)result);
            token_list_destroy(&list2);
            token_list_destroy(&list3);
        }

    }
    catch
    {
    }

    //printf("result=");
    //print_tokens(r.head);
    return r;
}
void print_token(const struct token* p_token);

static struct token_list text_line(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    /*
          text-line:
          pp-tokens_opt new-line
    */
    struct token_list r = { 0 };

    try
    {
        while (input_list->head &&
            input_list->head->type != TK_PREPROCESSOR_LINE)
        {
            struct macro* _Opt macro = NULL;
            struct token* _Opt start_token = input_list->head;
            const struct token* _Opt origin = NULL;

            if (input_list->head->type == TK_IDENTIFIER &&
                strcmp(input_list->head->lexeme, "_Pragma") == 0)
            {
                struct token_list r0 = operator_pragma(ctx, input_list, is_active, level);
                token_list_append_list(&r, &r0);
                continue;
            }

            if (is_active && input_list->head->type == TK_IDENTIFIER)
            {
                origin = input_list->head;
                macro = find_macro(ctx, input_list->head->lexeme);
                if (macro &&
                    macro->is_function &&
                    !preprocessor_token_ahead_is(input_list->head, '('))
                {
                    macro = NULL;
                }

                if (ctx->conditional_inclusion)
                {
                    /*
                     Quando estamos expandindo em condinonal inclusion o defined macro ou defined (macro)
                     não é expandido e é considerado depois
                    */

                    if (r.tail &&
                        r.tail->type == TK_IDENTIFIER &&
                        strcmp(r.tail->lexeme, "defined") == 0)
                    {
                        macro = NULL;
                    }
                    else if (r.tail &&
                        r.tail->type == '(')
                    {
                        struct token* _Opt previous = r.tail->prev;
                        if (previous != NULL &&
                            previous->type == TK_IDENTIFIER &&
                            strcmp(previous->lexeme, "defined") == 0)
                        {
                            macro = NULL;
                        }
                    }
                }
            }
            if (macro)
            {
                // "Tetris" effect
                // #define f(a) a
                // #define F g
                // F(1)
                //quero deixar F(g) na saida.
                //e toda parte de dentro escondida no caso  1
                //F(1)`a` acho que vou imprimir desta forma ou so fundo diferente
                //
                enum token_flags flags = input_list->head->flags;
                struct macro_argument_list arguments = collect_macro_arguments(ctx, macro, input_list, level);
                if (ctx->n_errors > 0)
                {
                    macro_argument_list_destroy(&arguments);
                    throw;
                }


                struct token_list start_macro = expand_macro(ctx, NULL, macro, &arguments, level, origin);
                if (start_macro.head)
                {
                    start_macro.head->flags |= flags;
                }


                //seta nos tokens expandidos da onde eles vieram
                token_list_set_file(&start_macro, start_token->token_origin, start_token->line, start_token->col);

                token_list_append_list_at_beginning(input_list, &start_macro);

                if (ctx->flags & PREPROCESSOR_CTX_FLAGS_ONLY_FINAL)
                {
                }
                else
                {
                    if (level == 0 || INCLUDE_ALL)
                        token_list_append_list(&r, &arguments.tokens);
                }

                //print_tokens(r.head);
                while (macro)
                {
                    macro = NULL;
                    if (input_list->head && input_list->head->type == TK_IDENTIFIER)
                    {
                        macro = find_macro(ctx, input_list->head->lexeme);
                        if (macro && macro->is_function &&
                            !preprocessor_token_ahead_is(input_list->head, '('))
                        {
                            macro = NULL;
                        }
                        if (macro)
                        {
                            // printf("tetris\n");
                            int flags2 = input_list->head->flags;
                            struct macro_argument_list arguments2 = collect_macro_arguments(ctx, macro, input_list, level);
                            if (ctx->n_errors > 0)
                            {
                                macro_argument_list_destroy(&arguments2);
                                macro_argument_list_destroy(&arguments);
                                token_list_destroy(&start_macro);
                                throw;
                            }

                            if (ctx->flags & PREPROCESSOR_CTX_FLAGS_ONLY_FINAL)
                            {
                            }
                            else
                            {
                                if (level == 0 || INCLUDE_ALL)
                                {
                                    token_list_append_list(&r, &arguments2.tokens);
                                }
                            }


                            struct token_list r3 = expand_macro(ctx, NULL, macro, &arguments2, level, origin);
                            if (ctx->n_errors > 0)
                            {
                                macro_argument_list_destroy(&arguments2);
                                token_list_destroy(&r3);
                                macro_argument_list_destroy(&arguments);
                                token_list_destroy(&start_macro);
                                throw;
                            }

                            //seta nos tokens expandidos da onde eles vieram
                            token_list_set_file(&r3, start_token->token_origin, start_token->line, start_token->col);

                            if (r3.head)
                            {
                                r3.head->flags = flags2;
                            }
                            token_list_append_list_at_beginning(input_list, &r3);
                            macro_argument_list_destroy(&arguments2);
                            token_list_destroy(&r3);
                        }
                    }
                }

                macro_argument_list_destroy(&arguments);
                token_list_destroy(&start_macro);

                continue;
                //exit tetris...
                //entao tudo foi expandido desde a primeiroa
            }
            else
            {
                if (input_list->head->flags & TK_FLAG_LINE_CONTINUATION &&
                    !(input_list->head->flags & TK_FLAG_MACRO_EXPANDED)
                    )
                {
                    /*
                       The only place were line-continuation are really necessary is
                       inside preprocessor directives.
                       Here we are inside text-line so we can send a info that
                       here is optional.
                    */
                    if (input_list->head->type == TK_STRING_LITERAL)
                    {
                        preprocessor_diagnostic(W_NOTE, ctx, input_list->head, "you can use \"adjacent\" \"strings\"");
                    }
                    else if (input_list->head->type == TK_LINE_COMMENT)
                        preprocessor_diagnostic(W_COMMENT, ctx, input_list->head, "multi-line //comment");
                    else
                        preprocessor_diagnostic(W_LINE_SLICING, ctx, input_list->head, "unnecessary line-slicing");
                }

                bool blanks = token_is_blank(input_list->head) || input_list->head->type == TK_NEWLINE;
                bool is_final = is_active && !is_never_final(input_list->head->type);

                if (ctx->flags & PREPROCESSOR_CTX_FLAGS_ONLY_FINAL)
                {
                    if (is_final)
                    {
                        prematch(&r, input_list);
                        assert(r.tail != NULL);
                        r.tail->flags |= TK_FLAG_FINAL;
                    }
                    else
                    {
                        token_list_pop_front(input_list);//todo deletar
                    }
                }
                else
                {
                    if (blanks)
                    {
                        if (level == 0 || INCLUDE_ALL)
                        {
                            prematch(&r, input_list);
                        }
                        else
                            token_list_pop_front(input_list);//todo deletar
                    }
                    else
                    {
                        if (level == 0 || INCLUDE_ALL)
                        {
                            prematch(&r, input_list);
                            if (is_final)
                            {
                                assert(r.tail != NULL);
                                r.tail->flags |= TK_FLAG_FINAL;
                            }
                        }
                        else
                        {
                            if (is_final)
                            {
                                prematch(&r, input_list);
                                assert(r.tail != NULL);
                                r.tail->flags |= TK_FLAG_FINAL;
                            }
                            else
                            {
                                token_list_pop_front(input_list);//todo deletar
                            }
                        }
                    }
                }


            }
        }
    }
    catch
    {
    }

    return r;
}

struct token_list group_part(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    /*
    group-part:
     if-section
     control-line
     text-line
     # non-directive
    */

    assert(input_list->head != NULL);

    if (input_list->head->type == TK_PREPROCESSOR_LINE)
    {
        if (preprocessor_token_ahead_is_identifier(input_list->head, "if") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "ifdef") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "ifndef"))
        {
            return if_section(ctx, input_list, is_active, level);
        }
        else if (preprocessor_token_ahead_is_identifier(input_list->head, "def"))
        {
            //C2Y
            return def_section(ctx, input_list, is_active, level);
        }
        else if (preprocessor_token_ahead_is_identifier(input_list->head, "include") ||
        preprocessor_token_ahead_is_identifier(input_list->head, "include_next") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "embed") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "define") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "undef") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "warning") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "line") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "error") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "pragma") ||
            preprocessor_token_ahead_is(input_list->head, TK_NEWLINE))
        {
            return control_line(ctx, input_list, is_active, level);
        }
        else
        {
            if (is_active)
            {
                struct token* _Opt p_token = preprocessor_look_ahead_core(input_list->head);
                const char* directive_name = p_token ? p_token->lexeme : "";
                /*
                   13 The execution of a non-directive preprocessing directive results in undefined behavior.
                */
                preprocessor_diagnostic(C_ERROR_INVALID_PREPROCESSING_DIRECTIVE,
                    ctx,
                    input_list->head,
                    "invalid preprocessor directive '#%s'\n", directive_name);
            }
            //here I will consume the # inside to make it symmetrical
            return non_directive(ctx, input_list, level);
        }
    }
    return text_line(ctx, input_list, is_active, level);
}


struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level)
{
    struct token_list r = { 0 };
    if (input_list->head == NULL)
    {
        return r;
    }

    if (input_list->head->type == TK_BEGIN_OF_FILE)
    {
        prematch_level(&r, input_list, 1); //sempre coloca
    }

    struct token_list g = group_opt(ctx, input_list, true /*active*/, level);
    token_list_append_list(&r, &g);
    token_list_destroy(&g);
    return r;
}


static void mark_macros_as_used(struct hash_map* map)
{
    /*
     *  Objetivo era alertar macros nao usadas...
     */

    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct map_entry* _Opt pentry = map->table[i];

            while (pentry != NULL)
            {
                assert(pentry->data.p_macro != NULL);
                struct macro* macro = pentry->data.p_macro;
                macro->usage = 1;
                pentry = pentry->next;
            }
        }
    }
}

void check_unused_macros(const struct hash_map* map)
{
    /*
     *  Objetivo era alertar macros nao usadas...
     */

    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct map_entry* _Opt pentry = map->table[i];

            while (pentry != NULL)
            {
                assert(pentry->data.p_macro != NULL);

                struct macro* macro = pentry->data.p_macro;
                if (macro->usage == 0)
                {
                    //TODO adicionar conceito meu codigo , codigo de outros nao vou colocar erro
                    printf("%s not used\n", macro->name);
                }
                pentry = pentry->next;
            }
        }
    }
}

int include_config_header(struct preprocessor_ctx* ctx, const char* file_name)
{
    char local_cakeconfig_path[MAX_PATH] = { 0 };
    snprintf(local_cakeconfig_path, sizeof local_cakeconfig_path, "%s", file_name);
    dirname(local_cakeconfig_path);

    snprintf(local_cakeconfig_path, sizeof local_cakeconfig_path, "%s" CAKE_CFG_FNAME, local_cakeconfig_path);

    char* _Owner _Opt str = read_file(local_cakeconfig_path, true);

    if (str && ctx->options.show_includes)
    {
        printf(".%s\n", local_cakeconfig_path);
    }

    while (str == NULL)
    {
        dirname(local_cakeconfig_path);
        dirname(local_cakeconfig_path);
        if (local_cakeconfig_path[0] == '\0')
            break;
        str = read_file(local_cakeconfig_path, true);
        if (str && ctx->options.show_includes)
        {
            printf(".%s\n", local_cakeconfig_path);
        }
    }

    if (str == NULL)
    {
        //Search cakeconfig at cake executable dir

        char executable_path[MAX_PATH - sizeof(CAKE_CFG_FNAME)] = { 0 };
        get_self_path(executable_path, sizeof(executable_path));
        dirname(executable_path);
        char root_cakeconfig_path[MAX_PATH] = { 0 };
        snprintf(root_cakeconfig_path, sizeof root_cakeconfig_path, "%s" CAKE_CFG_FNAME, executable_path);
        str = read_file(root_cakeconfig_path, true);
        if (str && ctx->options.show_includes)
        {
            printf(".%s\n", root_cakeconfig_path);
        }
    }

    if (str == NULL)
    {
        if (ctx->options.show_includes)
        {
            printf(".(cakeconfig.h not found)\n");
        }
        //"No such file or directory";
        return  ENOENT;
    }

    const enum diagnostic_id w =
        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings;

    struct tokenizer_ctx tctx = { 0 };
    struct token_list l = tokenizer(&tctx, str, "standard macros inclusion", 0, TK_FLAG_NONE);
    struct token_list l10 = preprocessor(ctx, &l, 0);
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    free(str);
    token_list_destroy(&l10);

    /*restore*/
    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings = w;

    return  0;
}

static bool is_builtin_macro(const char* name)
{
    if (strcmp(name, "__FILE__") == 0)
        return true;

    if (strcmp(name, "__CAKE__") == 0)
        return true;

    return false;
}

void add_standard_macros(struct preprocessor_ctx* ctx, enum target target)
{
    /*
      This command prints all macros used by gcc
      echo | gcc -dM -E -
    */
    const struct diagnostic w =
        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];

    /*we dont want warnings here*/
    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] =
        (struct diagnostic){ 0 };

    static char mon[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };

    time_t now = time(NULL);
    struct tm* tm = localtime(&now);

    struct tokenizer_ctx tctx = { 0 };


    char datastr[100] = { 0 };
    snprintf(datastr, sizeof datastr, "#define __DATE__ \"%s %2d %d\"\n", mon[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);
    struct token_list l1 = tokenizer(&tctx, datastr, "__DATE__ macro inclusion", 0, TK_FLAG_NONE);
    struct token_list tl1 = preprocessor(ctx, &l1, 0);

    token_list_destroy(&tl1);
    token_list_destroy(&l1);

    char timestr[100] = { 0 };
    snprintf(timestr, sizeof timestr, "#define __TIME__ \"%02d:%02d:%02d\"\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    struct token_list l2 = tokenizer(&tctx, timestr, "__TIME__ macro inclusion", 0, TK_FLAG_NONE);
    struct token_list tl2 = preprocessor(ctx, &l2, 0);

    token_list_destroy(&tl2);
    token_list_destroy(&l2);


    /*
      Some macros are dynamic like __LINE__ they are replaced  at
      macro_copy_replacement_list but they need to be registered here.
    */

    const char* pre_defined_macros_text = NULL;

    switch (target)
    {
    case TARGET_X86_X64_GCC:
        pre_defined_macros_text = TARGET_X86_X64_GCC_PREDEFINED_MACROS;
        break;
    case TARGET_X86_MSVC:
        pre_defined_macros_text = TARGET_X86_MSVC_PREDEFINED_MACROS;
        break;
    case TARGET_X64_MSVC:
        pre_defined_macros_text = TARGET_X64_MSVC_PREDEFINED_MACROS;
        break;
    }

    struct token_list l = tokenizer(&tctx, pre_defined_macros_text, "standard macros inclusion", 0, TK_FLAG_NONE);
    struct token_list l10 = preprocessor(ctx, &l, 0);

    //nao quer ver warning de nao usado nestas macros padrao
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    token_list_destroy(&l10);

    /*restore*/
    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = w;
}




const char* get_token_name(enum token_type tk)
{
    switch (tk)
    {
    case TK_NONE: return "TK_NONE";
    case TK_NEWLINE: return "TK_NEWLINE";
    case TK_WHITE_SPACE: return "TK_WHITE_SPACE";
    case TK_EXCLAMATION_MARK: return "TK_EXCLAMATION_MARK";
    case TK_QUOTATION_MARK: return "TK_QUOTATION_MARK";
    case TK_NUMBER_SIGN: return "TK_NUMBER_SIGN";
    case TK_DOLLAR_SIGN: return "TK_DOLLAR_SIGN";
    case TK_PERCENT_SIGN: return "TK_PERCENT_SIGN";
    case TK_AMPERSAND: return "TK_AMPERSAND";
    case TK_APOSTROPHE: return "TK_APOSTROPHE";
    case TK_LEFT_PARENTHESIS: return "TK_LEFT_PARENTHESIS";
    case TK_RIGHT_PARENTHESIS: return "TK_RIGHT_PARENTHESIS";
    case TK_ASTERISK: return "TK_ASTERISK";
    case TK_PLUS_SIGN: return "TK_PLUS_SIGN";
    case TK_COMMA: return "TK_COMMA";
    case TK_HYPHEN_MINUS: return "TK_HYPHEN_MINUS";
    case TK_FULL_STOP: return "TK_FULL_STOP";
    case TK_SOLIDUS: return "TK_SOLIDUS";
    case TK_COLON: return "TK_COLON";
    case TK_SEMICOLON: return "TK_SEMICOLON";
    case TK_LESS_THAN_SIGN: return "TK_LESS_THAN_SIGN";
    case TK_EQUALS_SIGN: return "TK_EQUALS_SIGN";
    case TK_GREATER_THAN_SIGN: return "TK_GREATER_THAN_SIGN";
    case TK_QUESTION_MARK: return "TK_QUESTION_MARK";
    case TK_COMMERCIAL_AT: return "TK_COMMERCIAL_AT";
    case TK_LEFT_SQUARE_BRACKET: return "TK_LEFT_SQUARE_BRACKET";
    case TK_REVERSE_SOLIDUS: return "TK_REVERSE_SOLIDUS";
    case TK_RIGHT_SQUARE_BRACKET: return "TK_RIGHT_SQUARE_BRACKET";
    case TK_CIRCUMFLEX_ACCENT: return "TK_CIRCUMFLEX_ACCENT";
    case TK_FLOW_LINE: return "TK_FLOW_LINE";
    case TK_GRAVE_ACCENT: return "TK_GRAVE_ACCENT";
    case TK_LEFT_CURLY_BRACKET: return "TK_LEFT_CURLY_BRACKET";
    case TK_VERTICAL_LINE: return "TK_VERTICAL_LINE";
    case TK_RIGHT_CURLY_BRACKET: return "TK_RIGHT_CURLY_BRACKET";
    case TK_TILDE: return "TK_TILDE";
    case TK_PREPROCESSOR_LINE: return "TK_PREPROCESSOR_LINE";
    case TK_PRAGMA: return "TK_PRAGMA";
    case TK_STRING_LITERAL: return "TK_STRING_LITERAL";
    case TK_CHAR_CONSTANT: return "TK_CHAR_CONSTANT";
    case TK_LINE_COMMENT: return "TK_LINE_COMMENT";
    case TK_COMMENT: return "TK_COMMENT";
    case TK_PPNUMBER: return "TK_PPNUMBER";

    case TK_KEYWORD_GCC__ATTRIBUTE:return "TK_KEYWORD_GCC__ATTRIBUTE";
    case TK_KEYWORD_GCC__BUILTIN_VA_LIST:return "TK_KEYWORD_GCC__BUILTIN_VA_LIST";
    case TK_KEYWORD_MSVC__PTR32:return "TK_KEYWORD_MSVC__PTR32";
    case TK_KEYWORD_MSVC__PTR64:return "TK_KEYWORD_MSVC__PTR64";

    case ANY_OTHER_PP_TOKEN: return "ANY_OTHER_PP_TOKEN"; //@ por ex

        /*PPNUMBER sao convertidos para constantes antes do parse*/
    case TK_COMPILER_DECIMAL_CONSTANT: return "TK_COMPILER_DECIMAL_CONSTANT";
    case TK_COMPILER_OCTAL_CONSTANT: return "TK_COMPILER_OCTAL_CONSTANT";
    case TK_COMPILER_HEXADECIMAL_CONSTANT: return "TK_COMPILER_HEXADECIMAL_CONSTANT";
    case TK_COMPILER_BINARY_CONSTANT: return "TK_COMPILER_BINARY_CONSTANT";
    case TK_COMPILER_DECIMAL_FLOATING_CONSTANT: return "TK_COMPILER_DECIMAL_FLOATING_CONSTANT";
    case TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT: return "TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT";


    case TK_PLACEMARKER: return "TK_PLACEMARKER";

    case TK_BLANKS: return "TK_BLANKS";
    case TK_PLUSPLUS: return "TK_PLUSPLUS";
    case TK_MINUSMINUS: return "TK_MINUSMINUS";
    case TK_ARROW: return "TK_ARROW";
    case TK_SHIFTLEFT: return "TK_SHIFTLEFT";
    case TK_SHIFTRIGHT: return "TK_SHIFTRIGHT";
    case TK_LOGICAL_OPERATOR_OR: return "TK_LOGICAL_OPERATOR_OR";
    case TK_LOGICAL_OPERATOR_AND: return "TK_LOGICAL_OPERATOR_AND";

    case TK_MACRO_CONCATENATE_OPERATOR: return "TK_MACRO_CONCATENATE_OPERATOR";

    case TK_IDENTIFIER: return "TK_IDENTIFIER";
    case TK_IDENTIFIER_RECURSIVE_MACRO: return "TK_IDENTIFIER_RECURSIVE_MACRO"; /*usado para evitar recursao expansao macro*/

    case TK_BEGIN_OF_FILE: return "TK_BEGIN_OF_FILE";

        //C23 keywords
    case TK_KEYWORD_AUTO: return "TK_KEYWORD_AUTO";
    case TK_KEYWORD_BREAK: return "TK_KEYWORD_BREAK";
    case TK_KEYWORD_CASE: return "TK_KEYWORD_CASE";
    case TK_KEYWORD_CONSTEXPR: return "TK_KEYWORD_CONSTEXPR";
    case TK_KEYWORD_CHAR: return "TK_KEYWORD_CHAR";
    case TK_KEYWORD_CONST: return "TK_KEYWORD_CONST";
    case TK_KEYWORD_CONTINUE: return "TK_KEYWORD_CONTINUE";
    case TK_KEYWORD_CAKE_CATCH: return "TK_KEYWORD_CAKE_CATCH"; /*extension*/
    case TK_KEYWORD_DEFAULT: return "TK_KEYWORD_DEFAULT";
    case TK_KEYWORD_DO: return "TK_KEYWORD_DO";
    case TK_KEYWORD_DEFER: return "TK_KEYWORD_DEFER"; /*extension*/
    case TK_KEYWORD_DOUBLE: return "TK_KEYWORD_DOUBLE";
    case TK_KEYWORD_ELSE: return "TK_KEYWORD_ELSE";
    case TK_KEYWORD_ENUM: return "TK_KEYWORD_ENUM";
    case TK_KEYWORD_EXTERN: return "TK_KEYWORD_EXTERN";
    case TK_KEYWORD_FLOAT: return "TK_KEYWORD_FLOAT";
    case TK_KEYWORD_FOR: return "TK_KEYWORD_FOR";
    case TK_KEYWORD_GOTO: return "TK_KEYWORD_GOTO";
    case TK_KEYWORD_IF: return "TK_KEYWORD_IF";
    case TK_KEYWORD_INLINE: return "TK_KEYWORD_INLINE";
    case TK_KEYWORD_INT: return "TK_KEYWORD_INT";
    case TK_KEYWORD_LONG: return "TK_KEYWORD_LONG";
    case TK_KEYWORD_MSVC__INT8: return "TK_KEYWORD_MSVC__INT8";
    case TK_KEYWORD_MSVC__INT16: return "TK_KEYWORD_MSVC__INT16";
    case TK_KEYWORD_MSVC__INT32: return "TK_KEYWORD_MSVC__INT32";
    case TK_KEYWORD_MSVC__INT64: return "TK_KEYWORD_MSVC__INT64";


    case TK_KEYWORD_REGISTER: return "TK_KEYWORD_REGISTER";
    case TK_KEYWORD_RESTRICT: return "TK_KEYWORD_RESTRICT";
    case TK_KEYWORD_RETURN: return "TK_KEYWORD_RETURN";
    case TK_KEYWORD_SHORT: return "TK_KEYWORD_SHORT";
    case TK_KEYWORD_SIGNED: return "TK_KEYWORD_SIGNED";
    case TK_KEYWORD_SIZEOF: return "TK_KEYWORD_SIZEOF";

    case TK_KEYWORD_STATIC: return "TK_KEYWORD_STATIC";
    case TK_KEYWORD_STRUCT: return "TK_KEYWORD_STRUCT";
    case TK_KEYWORD_SWITCH: return "TK_KEYWORD_SWITCH";
    case TK_KEYWORD_TYPEDEF: return "TK_KEYWORD_TYPEDEF";
    case TK_KEYWORD_CAKE_TRY: return "TK_KEYWORD_CAKE_TRY"; /*extension*/
    case TK_KEYWORD_CAKE_THROW: return "TK_KEYWORD_CAKE_THROW"; /*extension*/
    case TK_KEYWORD_UNION: return "TK_KEYWORD_UNION";
    case TK_KEYWORD_UNSIGNED: return "TK_KEYWORD_UNSIGNED";
    case TK_KEYWORD_VOID: return "TK_KEYWORD_VOID";
    case TK_KEYWORD_VOLATILE: return "TK_KEYWORD_VOLATILE";
    case TK_KEYWORD_WHILE: return "TK_KEYWORD_WHILE";

    case TK_KEYWORD__ALIGNAS: return "TK_KEYWORD__ALIGNAS";
    case TK_KEYWORD__ALIGNOF: return "TK_KEYWORD__ALIGNOF";
    case TK_KEYWORD__ATOMIC: return "TK_KEYWORD__ATOMIC";

        //#ifdef _WIN32
    case TK_KEYWORD_MSVC__FASTCALL: return "TK_KEYWORD_MSVC__FASTCALL";
    case TK_KEYWORD_MSVC__STDCALL:return "TK_KEYWORD_MSVC__STDCALL";
    case TK_KEYWORD_MSVC__CDECL:return "TK_KEYWORD_MSVC__CDECL";
    case TK_KEYWORD_MSVC__DECLSPEC:return "TK_KEYWORD_MSVC__DECLSPEC";
        //#endif
    case TK_KEYWORD__ASM: return "TK_KEYWORD__ASM";
        //end microsoft
    case TK_KEYWORD__BOOL: return "TK_KEYWORD__BOOL";
    case TK_KEYWORD__COMPLEX: return "TK_KEYWORD__COMPLEX";
    case TK_KEYWORD__DECIMAL128: return "TK_KEYWORD__DECIMAL128";
    case TK_KEYWORD__DECIMAL32: return "TK_KEYWORD__DECIMAL32";
    case TK_KEYWORD__DECIMAL64: return "TK_KEYWORD__DECIMAL64";
    case TK_KEYWORD__GENERIC: return "TK_KEYWORD__GENERIC";
    case TK_KEYWORD__IMAGINARY: return "TK_KEYWORD__IMAGINARY";
    case TK_KEYWORD__NORETURN: return "TK_KEYWORD__NORETURN";
    case TK_KEYWORD__STATIC_ASSERT: return "TK_KEYWORD__STATIC_ASSERT";
    case TK_KEYWORD_ASSERT: return "TK_KEYWORD_ASSERT"; /*extension*/
    case TK_KEYWORD__THREAD_LOCAL: return "TK_KEYWORD__THREAD_LOCAL";

    case TK_KEYWORD_TYPEOF: return "TK_KEYWORD_TYPEOF"; /*C23*/

    case TK_KEYWORD_TRUE: return "TK_KEYWORD_TRUE";  /*C23*/
    case TK_KEYWORD_FALSE: return "TK_KEYWORD_FALSE";  /*C23*/
    case TK_KEYWORD_NULLPTR: return "TK_KEYWORD_NULLPTR";  /*C23*/
    case TK_KEYWORD_TYPEOF_UNQUAL: return "TK_KEYWORD_TYPEOF_UNQUAL"; /*C23*/
    case TK_KEYWORD__BITINT: return "TK_KEYWORD__BITINT";  /*C23*/



        /*cake extension*/
    case TK_KEYWORD_CAKE_OWNER: return "TK_KEYWORD_CAKE_OWNER";
    case TK_KEYWORD__CTOR: return "TK_KEYWORD__OUT";
    case TK_KEYWORD__DTOR: return "TK_KEYWORD__OBJ_OWNER";
    case TK_KEYWORD_CAKE_VIEW: return "TK_KEYWORD_CAKE_VIEW";
    case TK_KEYWORD_CAKE_OPT: return "TK_KEYWORD_CAKE_OPT";


        /*extension compile time functions*/
    case TK_KEYWORD_CAKE_STATIC_DEBUG: return "TK_KEYWORD_CAKE_STATIC_DEBUG"; /*extension*/
    case TK_KEYWORD_CAKE_STATIC_DEBUG_EX: return "TK_KEYWORD_CAKE_STATIC_DEBUG_EX"; /*extension*/
    case TK_KEYWORD_STATIC_STATE: return "TK_KEYWORD_STATIC_STATE"; /*extension*/
    case TK_KEYWORD_STATIC_SET: return "TK_KEYWORD_STATIC_SET"; /*extension*/

        /*https://en.cppreference.com/w/cpp/header/type_traits*/

    case TK_KEYWORD_IS_POINTER: return "TK_KEYWORD_IS_POINTER";
    case TK_KEYWORD_IS_LVALUE: return "TK_KEYWORD_IS_LVALUE";
    case TK_KEYWORD_IS_CONST: return "TK_KEYWORD_IS_CONST";
    case TK_KEYWORD_IS_OWNER: return "TK_KEYWORD_IS_OWNER";
    case TK_KEYWORD_IS_ARRAY: return "TK_KEYWORD_IS_ARRAY";
    case TK_KEYWORD_IS_FUNCTION: return "TK_KEYWORD_IS_FUNCTION";
    case TK_KEYWORD_IS_SCALAR: return "TK_KEYWORD_IS_SCALAR";
    case TK_KEYWORD_IS_ARITHMETIC: return "TK_KEYWORD_IS_ARITHMETIC";
    case TK_KEYWORD_IS_FLOATING_POINT: return "TK_KEYWORD_IS_FLOATING_POINT";
    case TK_KEYWORD_IS_INTEGRAL: return "TK_KEYWORD_IS_INTEGRAL";
    case TK_PRAGMA_END: return "TK_PRAGMA_END";
    case TK_KEYWORD__COUNTOF: return "TK_KEYWORD__COUNTOF";
    case TK_PLUS_ASSIGN: return "TK_PLUS_ASSIGN";
    case TK_MINUS_ASSIGN: return "TK_MINUS_ASSIGN";
    case TK_MULTI_ASSIGN: return "TK_MULTI_ASSIGN";
    case TK_DIV_ASSIGN: return "TK_DIV_ASSIGN";
    case TK_MOD_ASSIGN: return "TK_MOD_ASSIGN";
    case TK_SHIFT_LEFT_ASSIGN: return "TK_SHIFT_LEFT_ASSIGN";
    case TK_SHIFT_RIGHT_ASSIGN: return "TK_SHIFT_RIGHT_ASSIGN";
    case TK_AND_ASSIGN: return "TK_AND_ASSIGN";
    case TK_OR_ASSIGN: return "TK_OR_ASSIGN";
    case TK_NOT_ASSIGN: return "TK_NOT_ASSIGN";

    case TK_KEYWORD_GCC__BUILTIN_VA_END: return "TK_KEYWORD_GCC__BUILTIN_VA_END";
    case TK_KEYWORD_GCC__BUILTIN_VA_ARG: return "TK_KEYWORD_GCC__BUILTIN_VA_ARG";
    case TK_KEYWORD_GCC__BUILTIN_C23_VA_START: return "TK_KEYWORD_GCC__BUILTIN_C23_VA_START";
    case TK_KEYWORD_GCC__BUILTIN_VA_COPY: return "TK_KEYWORD_GCC__BUILTIN_VA_COPY";
    case TK_KEYWORD_GCC__BUILTIN_OFFSETOF: return "TK_KEYWORD_GCC__BUILTIN_OFFSETOF";

    }
    return "TK_X_MISSING_NAME";
};


int stringify(const char* input, int n, char output[])
{
    int count = 0;
    if (count < n)
        output[count++] = '"';

    const char* p = input;
    while (*p)
    {
        if (*p == '\"' ||
            *p == '\\')
        {
            if (count < n)
                output[count++] = '\\';

            if (count < n)
                output[count++] = *p;
            p++;
        }
        else
        {
            if (count < n)
                output[count++] = *p;
            p++;
        }
    }

    if (count < n)
        output[count++] = '"';
    if (count < n)
        output[count++] = 0;

    if (count >= n)
        return -count;

    return count;
}


void print_literal(const char* _Opt s)
{
    if (s == NULL)
    {
        printf("\"");
        printf("\"");
        return;
    }
    printf("\"");
    while (*s)
    {
        switch (*s)
        {
        case '\n':
            printf("\\n");
            break;
        default:
            printf("%c", *s);
        }
        s++;
    }
    printf("\"");
}


const char* _Owner _Opt get_code_as_we_see_plus_macros(const struct token_list* list)
{
    struct osstream ss = { 0 };
    struct token* _Opt current = list->head;
    while (current)
    {
        if (current->level == 0 &&
            current->type != TK_BEGIN_OF_FILE)
        {
            if (current->flags & TK_FLAG_MACRO_EXPANDED)
                ss_fprintf(&ss, LIGHTCYAN);
            else
                ss_fprintf(&ss, WHITE);
            ss_fprintf(&ss, "%s", current->lexeme);
            ss_fprintf(&ss, RESET);
        }
        current = current->next;
    }

    const char* _Owner _Opt cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}

/*useful to debug visit.c*/
void print_code_as_we_see(const struct token_list* list, bool remove_comments)
{
    if (list->head == NULL || list->tail == NULL)
    {
        return;
    }

    struct token* _Opt current = list->head;
    while (current && current != list->tail->next)
    {
        if (current->level == 0 &&
            !(current->flags & TK_FLAG_MACRO_EXPANDED) &&
            !(current->flags & TK_C_BACKEND_FLAG_HIDE) &&
            current->type != TK_BEGIN_OF_FILE)
        {
            if ((current->flags & TK_FLAG_HAS_SPACE_BEFORE) &&
                (current->prev != NULL && current->prev->type != TK_BLANKS))
            {
                //if an expanded macro is shown it does not have spaces so we insert
                printf(" ");
            }

            if (remove_comments)
            {
                if (current->type == TK_LINE_COMMENT)
                    printf("\n");
                else if (current->type == TK_COMMENT)
                    printf(" ");
                else
                    printf("%s", current->lexeme);
            }
            else
            {
                printf("%s", current->lexeme);
            }
        }
        current = current->next;
    }
}
const char* _Owner _Opt get_code_as_we_see(const struct token_list* list, bool remove_comments)
{
    if (list->head == NULL || list->tail == NULL)
        return NULL;

    struct osstream ss = { 0 };
    struct token* _Opt current = list->head;
    while (current && current != list->tail->next)
    {
        if (current->level == 0 &&
            !(current->flags & TK_FLAG_MACRO_EXPANDED) &&
            !(current->flags & TK_C_BACKEND_FLAG_HIDE) &&
            current->type != TK_BEGIN_OF_FILE)
        {
            if ((current->flags & TK_FLAG_HAS_SPACE_BEFORE) &&
                (current->prev != NULL && current->prev->type != TK_BLANKS))
            {
                //if an expanded macro is shown it has no spaces so we insert
                ss_fprintf(&ss, " ");
            }

            if (remove_comments)
            {
                if (current->type == TK_LINE_COMMENT)
                    ss_fprintf(&ss, "\n");
                else if (current->type == TK_COMMENT)
                    ss_fprintf(&ss, " ");
                else
                    ss_fprintf(&ss, "%s", current->lexeme);
            }
            else
            {
                ss_fprintf(&ss, "%s", current->lexeme);
            }
        }
        current = current->next;
    }

    const char* _Owner _Opt cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}


const char* _Owner _Opt get_code_as_compiler_see(const struct token_list* list)
{
    if (list->head == NULL || list->tail == NULL)
    {
        return NULL;
    }

    struct osstream ss = { 0 };


    struct token* _Opt current = list->head;
    while (current && current != list->tail->next)
    {
        if (!(current->flags & TK_C_BACKEND_FLAG_HIDE) &&
            current->type != TK_BEGIN_OF_FILE &&
            (current->flags & TK_FLAG_FINAL))
        {
            if (current->flags & TK_FLAG_HAS_SPACE_BEFORE)
                ss_fprintf(&ss, " ");

            if (current->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
                ss_fprintf(&ss, "\n");

            if (current->type == TK_LINE_COMMENT)
                ss_fprintf(&ss, "\n");
            else if (current->type == TK_COMMENT)
                ss_fprintf(&ss, " ");
            else
                ss_fprintf(&ss, "%s", current->lexeme);
        }
        current = current->next;
    }

    return ss.c_str;
}

const char* _Owner _Opt print_preprocessed_to_string2(const struct token* _Opt p_token)
{
    /*
      * At level > 0 (i.e. inside the includes)
      * This function prints the tokens as the compiler sees them
      * and inserts a space or line break to represent
      * the separation between tokens.

      * At level 0 (main file) it prints spaces, comments
      * etc. and inserts spaces in the macro expansion.
  */

    if (p_token == NULL)
        return strdup("(null)");

    struct osstream ss = { 0 };
    const struct token* _Opt current = p_token;
    while (current)
    {

        //We ignored the line continuation and it can appear anywhere in the lexemes. 
        //instead of removing it, you could just skip it when printing
        remove_line_continuation(current->lexeme);

        if (current->flags & TK_FLAG_FINAL)
        {
            if (current->level > 0)
            {
                //at the include levels we may be ignoring all
                //the spaces. in this case it is necessary to include them so as not to add the tokens
                if ((current->flags & TK_FLAG_HAS_NEWLINE_BEFORE))
                    ss_fprintf(&ss, "\n");
                else if ((current->flags & TK_FLAG_HAS_SPACE_BEFORE))
                    ss_fprintf(&ss, " ");
            }
            else
            {
                /*
                 at level 0 we print the spaces.. however in the case of macros
                 it is necessary to put a space because it does not exist.
                */
                if (current->flags & TK_FLAG_MACRO_EXPANDED)
                {
                    if ((current->flags & TK_FLAG_HAS_SPACE_BEFORE))
                        ss_fprintf(&ss, " ");
                }
            }

            if (current->type == TK_PRAGMA)
            {
                /*
                   This is not exactly how the compiler interprets see pragma;
                   it is representation where _Pragma and #pragma are printed in the same way.
                   Compiler sees TK_PRAGMA ..tokens.. TK_PRAGMA_END
                */
                if (strcmp(current->lexeme, "_Pragma") == 0)
                {
                    ss_fprintf(&ss, "\n"); /*added for visualization*/
                }

                ss_fprintf(&ss, "#pragma ");

                current = current->next;

                while (current->type != TK_PRAGMA_END)
                {
                    ss_fprintf(&ss, "%s", current->lexeme);
                    current = current->next;
                }

                ss_fprintf(&ss, "\n"); /*added for visualization*/
                current = current->next;
                continue;
            }

            if (current->lexeme[0] != '\0')
            {
                ss_fprintf(&ss, "%s", current->lexeme);
            }

            current = current->next;
        }
        else
        {
            if (current->level == 0)
            {
                if (current->type == TK_BLANKS ||
                    current->type == TK_NEWLINE)
                {
                    ss_fprintf(&ss, "%s", current->lexeme);
                }
            }

            current = current->next;
        }
    }

    return ss.c_str;
}

const char* _Owner _Opt print_preprocessed_to_string(const struct token* p_token)
{
    /*
    * Esta funcao imprime os tokens como o compilador ve
    * e insere um espaco ou quebra de linha para poder representar
    * a separacao entre os tokens.
    */

    struct osstream ss = { 0 };
    const struct token* _Opt current = p_token;

    /*
    * Ignora tudo o que é espaço no início
    */
    while (!(current->flags & TK_FLAG_FINAL) ||
        current->type == TK_BLANKS ||
        current->type == TK_COMMENT ||
        current->type == TK_LINE_COMMENT ||
        current->type == TK_NEWLINE ||
        current->type == TK_PREPROCESSOR_LINE)
    {
        current = current->next;
        if (current == NULL)
            return ss.c_str; /*MOVED*/
    }

    bool first = true;
    while (current)
    {
        assert(current->token_origin != NULL);
        if (current->flags & TK_FLAG_FINAL)
        {
            if (!first && current->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
                ss_fprintf(&ss, "\n");
            else if (!first && current->flags & TK_FLAG_HAS_SPACE_BEFORE)
                ss_fprintf(&ss, " ");
            if (current->lexeme[0] != '\0')
                ss_fprintf(&ss, "%s", current->lexeme);
            first = false;
            current = current->next;
        }
        else
        {
            current = current->next;
        }
    }

    return ss.c_str; /*MOVED*/
}

void print_preprocessed(const struct token* p_token)
{
    const char* _Owner _Opt s = print_preprocessed_to_string(p_token);
    if (s)
    {
        printf("%s", s);
        free((void* _Owner)s);
    }
}

static bool is_screaming_case(const char* text)
{

    bool screaming_case = false;

    while (*text)
    {
        if ((*text >= 'A' && *text <= 'Z') ||
            (*text >= '0' && *text <= '9') ||
            (*text == '_'))
        {
            //ok
            screaming_case = true;
        }
        else
            return false;
        text++;
    }

    return screaming_case;
}

void print_all_macros(const struct preprocessor_ctx* prectx)
{
    for (int i = 0; i < prectx->macros.capacity; i++)
    {
        struct map_entry* _Opt entry = prectx->macros.table[i];
        if (entry == NULL) continue;
        assert(entry->data.p_macro != NULL);

        struct macro* macro = entry->data.p_macro;
        printf("#define %s", macro->name);
        if (macro->is_function)
        {
            printf("(");

            struct macro_parameter* _Opt parameter = macro->parameters;
            while (parameter)
            {
                printf("%s", parameter->name);
                if (parameter->next)
                    printf(",");
                parameter = parameter->next;
            }
            printf(")");
        }
        printf(" ");

        struct token* _Opt token = macro->replacement_list.head;
        while (token)
        {
            printf("%s", token->lexeme);

            if (token == macro->replacement_list.tail)
                break;

            token = token->next;
        }
        printf("\n");
    }
}
void naming_convention_macro(struct preprocessor_ctx* ctx, struct token* token)
{
    if (!is_screaming_case(token->lexeme))
    {
        preprocessor_diagnostic(W_NOTE, ctx, token, "use SCREAMING_CASE for macros");
    }

}


#ifdef TEST
#include "unit_test.h"


void print_asserts(struct token* p_token)
{
    struct token* current = p_token;
    printf("struct { const char* lexeme; enum token_type token; int is_active; int is_final; } result[] = { \n");
    while (current)
    {
        printf("{ %-20s, %d, ", get_token_name(current->type), (current->flags & TK_FLAG_FINAL));
        print_literal(current->lexeme);
        printf("},\n");
        current = current->next;
    }
    printf("}\n");
}

void show_all(struct token* p_token)
{
    struct token* current = p_token;
    while (current)
    {
        if (current->flags & TK_FLAG_FINAL)
        {
            if (current->level == 0)
                printf(WHITE);
            else
                printf(BROWN);
        }
        else
        {
            if (current->level == 0)
                printf(LIGHTGRAY);
            else
                printf(BLACK);
        }
        printf("%s", current->lexeme);
        printf(RESET);
        current = current->next;
    }
}

void print_preprocessed_to_file(struct token* p_token, const char* filename)
{
    FILE* f = fopen(filename, "r");
    if (f)
    {
        const char* s = print_preprocessed_to_string(p_token);
        if (s)
        {
            fprintf(f, "%s", s);
            free((void* _Owner)s);
        }
        fclose(f);
    }
}

void show_visible(struct token* p_token)
{
    printf(WHITE "visible used   / " LIGHTGRAY "visible ignored\n" RESET);
    struct token* current = p_token;
    while (current)
    {
        if (current->level == 0)
        {
            if (current->flags & TK_FLAG_FINAL)
                printf(WHITE);
            else
                printf(LIGHTGRAY);
        }
        else
        {
            if (current->level == 0)
                printf(BLACK);
            else
                printf(BLACK);
        }
        printf("%s", current->lexeme);
        printf(RESET);
        current = current->next;
    }
}

void show_visible_and_invisible(struct token* p_token)
{
    printf(LIGHTGREEN "visible used   / " LIGHTGRAY "visible ignored\n" RESET);
    printf(LIGHTBLUE  "invisible used / " BROWN     "invisible ignored\n" RESET);
    struct token* current = p_token;
    while (current)
    {
        if (current->level == 0)
        {
            if (current->flags & TK_FLAG_FINAL)
                printf(LIGHTGREEN);
            else
                printf(LIGHTGRAY);
        }
        else
        {
            if (current->flags & TK_FLAG_FINAL)
                printf(LIGHTBLUE);
            else
                printf(BROWN);
        }
        printf("%s", current->lexeme);
        printf(RESET);
        current = current->next;
    }
}

int test_preprossessor_input_output(const char* input, const char* output)
{
    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);

    struct preprocessor_ctx ctx = { 0 };

    struct token_list r = preprocessor(&ctx, &list, 0);
    const char* s = print_preprocessed_to_string(r.head);
    if (strcmp(s, output) != 0)
    {
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("expected\n%s", output);
        printf("HAS\n%s", s);
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        print_tokens(r.head);
        printf("TEST 0 FAILED\n");
        return 1;
    }
    free((void* _Owner)s);
    return 0;
}

char* normalize_line_end(char* input)
{
    if (input == NULL)
        return NULL;
    char* pWrite = input;
    const char* p = input;
    while (*p)
    {
        if (p[0] == '\r' && p[1] == '\n')
        {
            *pWrite = '\n';
            p++;
            p++;
            pWrite++;
        }
        else
        {
            *pWrite = *p;
            p++;
            pWrite++;
        }
    }
    *pWrite = 0;
    return input;
}


int test_preprocessor_in_out(const char* input, const char* output)
{
    int res = 0;

    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);

    struct preprocessor_ctx ctx = { 0 };

    struct token_list r = preprocessor(&ctx, &list, 0);
    const char* result = print_preprocessed_to_string(r.head);
    if (result == NULL)
    {
        result = strdup("");
    }

    if (strcmp(result, output) != 0)
    {
        /*
        printf("FAILED\n");
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("assert\n");
        printf("%s`", output);
        printf("\nGOT\n");
        printf("%s`", result);
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        print_tokens(r.head);

        */
        res = 1;
    }

    free(result);

    return res;
}

int test_preprocessor_in_out_using_file(const char* fileName)
{
    int res = 0;
    const char* input = normalize_line_end(read_file(fileName, true));
    char* output = 0;
    if (input)
    {
        char* pos = strstr(input, "\n---");
        if (pos)
        {
            *pos = 0;
            //anda ate sair ---
            pos++;
            while (*pos != '\n')
            {
                pos++;
            }
            pos++; //skip \n
            output = pos;
            /*optional*/
            pos = strstr(output, "\n---");
            if (pos)
                *pos = 0;
        }
        res = test_preprocessor_in_out(input, output);
        free((void* _Owner)input);
    }
    return res;
}

void test_lexeme_cmp()
{
    assert(lexeme_cmp("a", "\\\na") == 0);
    assert(lexeme_cmp("a", "a\\\n") == 0);
    assert(lexeme_cmp("\\\na", "a") == 0);
    assert(lexeme_cmp("a\\\n", "a") == 0);
    assert(lexeme_cmp("a\\\nb", "ab") == 0);
    assert(lexeme_cmp("define", "define") == 0);
    assert(lexeme_cmp("de\\\nfine", "define") == 0);
}

void token_list_pop_front_test()
{

    struct token_list list = { 0 };
    token_list_pop_front(&list);
    struct tokenizer_ctx tctx = { 0 };
    list = tokenizer(&tctx, "a", NULL, 0, TK_FLAG_NONE);
    token_list_pop_front(&list);

    list = tokenizer(&tctx, "a,", NULL, 0, TK_FLAG_NONE);
    token_list_pop_front(&list);

    list = tokenizer(&tctx, "a,b", NULL, 0, TK_FLAG_NONE);
    token_list_pop_front(&list);
}

void token_list_pop_back_test()
{

    struct token_list list = { 0 };
    token_list_pop_back(&list);

    /*pop back quando so tem 1*/
    token_list_clear(&list);
    struct tokenizer_ctx tctx = { 0 };
    list = tokenizer(&tctx, "a", NULL, 0, TK_FLAG_NONE);
    token_list_pop_back(&list);
    assert(list.head == NULL && list.tail == NULL);


    /*
    * pop bacl com 2
    */

    token_list_clear(&list);
    list = tokenizer(&tctx, "a,", NULL, 0, TK_FLAG_NONE);
    token_list_pop_back(&list);

    assert(strcmp(list.head->lexeme, "a") == 0);

    assert(list.head != NULL &&
        list.head->prev == NULL &&
        list.head->next == NULL &&
        list.tail->prev == NULL &&
        list.tail->next == NULL &&
        list.tail == list.head);

    /*
    * pop back com 3
    */

    list = tokenizer(&tctx, "a,b", NULL, 0, TK_FLAG_NONE);
    token_list_pop_back(&list);
    assert(strcmp(list.head->lexeme, "a") == 0);
    assert(strcmp(list.head->next->lexeme, ",") == 0);
    assert(strcmp(list.tail->lexeme, ",") == 0);
    assert(strcmp(list.tail->prev->lexeme, "a") == 0);
    assert(list.head->prev == NULL);
    assert(list.tail->next == NULL);
}

int token_list_append_list_test()
{

    struct tokenizer_ctx tctx = { 0 };
    struct token_list source = { 0 };
    struct token_list dest = tokenizer(&tctx, "a", NULL, 0, TK_FLAG_NONE);
    token_list_append_list(&dest, &source);
    assert(strcmp(dest.head->lexeme, "a") == 0);


    token_list_clear(&source);
    token_list_clear(&dest);


    dest = tokenizer(&tctx, "a", NULL, 0, TK_FLAG_NONE);
    token_list_append_list(&dest, &source);

    assert(strcmp(dest.head->lexeme, "a") == 0);

    token_list_clear(&source);
    token_list_clear(&dest);
    source = tokenizer(&tctx, "a,", NULL, 0, TK_FLAG_NONE);
    dest = tokenizer(&tctx, "1", NULL, 0, TK_FLAG_NONE);
    token_list_append_list(&dest, &source);
    assert(strcmp(dest.head->lexeme, "1") == 0);
    assert(strcmp(dest.tail->lexeme, ",") == 0);
    assert(dest.tail->next == NULL);
    assert(dest.head->next->next == dest.tail);
    assert(dest.tail->prev->prev == dest.head);

    return 0;
}

void test_collect()
{
    const char* input =
        "#define F(A, B) A ## B\n"
        "F(a \n, b)";

    const char* output =
        "ab"
        ;


    assert(test_preprocessor_in_out(input, output) == 0);

}


void test_va_opt_0()
{
    const char* input =
        "#define F(...)  f(0 __VA_OPT__(,) __VA_ARGS__)\n"
        "F(a, b, c)";
    const char* output =
        "f(0, a, b, c)";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void test_va_opt_1()
{
    const char* input =
        "#define F(...)  f(0 __VA_OPT__(,) __VA_ARGS__)\n"
        "F()";
    const char* output =
        "f(0)";
    assert(test_preprocessor_in_out(input, output) == 0);
}


void test_va_opt_2()
{
    const char* input =
        "#define empty(...) (__VA_OPT__(!)1)\n"
        "empty()";
    const char* output =
        "(1)";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void test_va_opt_3()
{
    const char* input =
        "#define empty(...) (__VA_OPT__(!)1)\n"
        "empty(1)";
    const char* output =
        "(!1)";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void test_va_opt_4()
{
    const char* input =
        "#define LPAREN() (\n"
        "#define G(Q) 42\n"
        "#define F(R, X, ...) __VA_OPT__(G R X) )\n"
        "int x = F(LPAREN(), 0, <:-);\n"
        ;
    const char* output =
        "int x = 42;";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void test_va_opt_5()
{
    const char* input =
        "#define F(...) f(0 __VA_OPT__(,) __VA_ARGS__)\n"
        "#define EMPTY\n"
        "F(EMPTY)"
        ;
    const char* output =
        "f(0)";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void test_va_opt_6()
{
    const char* input =
        "#define G(X, ...) f(0, X __VA_OPT__(,) __VA_ARGS__)\n"
        "G(a)"
        ;

    const char* output =
        "f(0, a)";

    assert(test_preprocessor_in_out(input, output) == 0);
}
void test_va_opt_7()
{
    const char* input =
        "#define H4(X, ...) __VA_OPT__(a X ## X) ## b\n"
        "H4(, 1)"
        ;

    const char* output =
        "a b";

    assert(test_preprocessor_in_out(input, output) == 0);
}

void concatenation_problem()
{
    const char* input =
        "#define H4(X, ...) a X ## X ## b\n"
        "H4()"
        ;

    const char* output =
        "a b";

    assert(test_preprocessor_in_out(input, output) == 0);
}


void test_va_opt_G2()
{
    const char* input =
        "#define G(X, ...) f(0, X __VA_OPT__(,) __VA_ARGS__)\n"
        "G(a, )"
        ;

    const char* output =
        "f(0, a)";

    assert(test_preprocessor_in_out(input, output) == 0);
}


void test_va_opt()
{
    const char* input =
        "#define F(...)  f(0 __VA_OPT__(,) __VA_ARGS__)\n"
        "#define EMPTY\n"
        "F(EMPTY)";
    const char* output =
        "f(0)";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void test_empty_va_args()
{
    const char* input = "#define M(a, ...) a, __VA_ARGS__\n"
        "M(1)\n";
    const char* output =
        "1,";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void test_va_args_single()
{
    const char* input =
        "#define F(...) __VA_ARGS__\n"
        "F(1, 2)";
    const char* output =
        "1, 2";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void test_va_args_extra_args()
{
    const char* input =
        "#define F(a, ...) a __VA_ARGS__\n"
        "F(0, 1, 2)";
    const char* output =
        "0 1, 2";
    assert(test_preprocessor_in_out(input, output) == 0);
}


void test_empty_va_args_empty()
{
    const char* input =
        "#define F(...) a __VA_ARGS__\n"
        "F()";
    const char* output =
        "a";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void test_defined()
{
    const char* input =
        "#if defined X || defined (X)\n"
        "A\n"
        "#else\n"
        "B\n"
        "#endif\n";
    const char* output =
        "B";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void testline()
{
    const char* input =
        "#define M \\\n"
        "        a\\\n"
        "        b\n"
        "M";
    const char* output =
        "a b";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void ifelse()
{
    const char* input =
        "#if 1\n"
        "A\n"
        "#else\n"
        "B\n"
        "#endif\n";
    const char* output =
        "A";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void T1()
{
    const char* input =
        "#define f(a) f(x * (a))\n"
        "f(2 * (0, 1))";
    const char* output =
        "f(x * (2 * (0, 1)))";
    //se f tivesse 2 parametros
    //error: too few arguments provided to function-like macro invocation
    //se f nao tivesse nenhum ou menus
    //too many arguments provided to function-like macro invocation
    assert(test_preprocessor_in_out(input, output) == 0);
}

int EXAMPLE5()
{
    /*
    //EXAMPLE 5 To illustrate the rules for placemarker preprocessing tokens, the sequence

    //const char* input =
    //"#define t(x,y,z) x ## y ## z\n"
    //"int j[] = {t(+1,2,3), t(,4,5), t(6,,7), t(8,9,),t(10,,), t(,11,), t(,,12), t(,,) };";

    //const char* output =
      //  "int j[] = {+123, 45, 67, 89,10, 11, 12, };";

    const char* input =
        "#define t(x,y,z) x ## y ## z\n"
        "t(+1,2,3)";

    const char* output =
        "int j[] = {+123, 45, 67, 89,10, 11, 12, };";

    //se f tivesse 2 parametros
    //error: too few arguments provided to function-like macro invocation

    //se f nao tivesse nenhum ou menus
    //too many arguments provided to function-like macro invocation
    //test_preprocessor_in_out(input, output);
    */
    return 0;
}

void recursivetest1()
{
    //acho que este vai sero caso que precisa do hidden set.
    const char* input =
        "#define x 2\n"
        "#define f(a) f(x * (a))\n"
        "#define z z[0]\n"
        "f(f(z))";
    //resultado gcc da
    //const char* output =
    //  "f(2 * (f(2 * (z[0]))))";
    const char* output =
        "f(2 * (f(z[0])))";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void rectest()
{
    const char* input =
        "#define x 2\n"
        "#define f(a) f(x * (a))\n"
        "#define g f\n"
        "#define z z[0]\n"
        "f(y + 1) + f(f(z)) % t(t(g)(0) + t)(1);";
    //GCC
    //const char* output =
    //  "f(2 * (y + 1)) + f(2 * (f(2 * (z[0])))) % t(t(f)(0) + t)(1);";
    const char* output =
        "f(2 * (y + 1)) + f(2 * (f(z[0]))) % t(t(f)(0) + t)(1);";
    assert(test_preprocessor_in_out(input, output) == 0);
}

void emptycall()
{
    const char* input =
        "#define F(x) x\n"
        "F()"
        ;
    const char* output =
        ""
        ;
    assert(test_preprocessor_in_out(input, output) == 0);
}

void semiempty()
{
    const char* input =
        "#define F(x,y) x ## y\n"
        "F(1,)"
        ;
    const char* output =
        "1"
        ;
    assert(test_preprocessor_in_out(input, output) == 0);
}

void calling_one_arg_with_empty_arg()
{
    const char* input =
        "#define F(a) # a\n"
        "F()"
        ;
    const char* output =
        "\"\""
        ;
    assert(test_preprocessor_in_out(input, output) == 0);
}


void test_argument_with_parentesis()
{
    const char* input =
        "#define F(a, b) a ## b\n"
        "F((1, 2, 3),4)"
        ;
    const char* output =
        "(1, 2, 3)4"
        ;
    assert(test_preprocessor_in_out(input, output) == 0);
}

void two_empty_arguments()
{
    const char* input =
        "#define F(a, b) a ## b\n"
        "F(,)\n"
        ;
    const char* output =
        ""
        ;
    assert(test_preprocessor_in_out(input, output) == 0);
}

void simple_object_macro()
{
    const char* input = "#define B b\n"
        "#define M a B\n"
        "M\n"
        "c\n";
    const char* output =
        "a b\n"
        "c";
    assert(test_preprocessor_in_out(input, output) == 0);
}


void test_one_file()
{
    assert(test_preprocessor_in_out_using_file("tests/pre_debug.c") == 0);
}

void test2()
{
    const char* input =
        "#define F(a, b) 1 a ## b 4\n"
        "F(  2  ,  3 )"
        ;
    const char* output =
        "1 23 4"
        ;

    assert(test_preprocessor_in_out(input, output) == 0);
}


void test3()
{
#if 0
    const char* input =
        "#define F(a, b) 1 a ## 3 4\n"
        "F(  2   )"
        ;
    const char* output =
        "1 23 4"
        ;
#endif
    //este erro falta parametro b
    //too few arguments provided to function - like macro invocation
    //test_preprocessor_in_out(input, output);
}


void tetris()
{
    const char* input =
        "#define D(a) a\n"
        "#define C(a) a\n"
        "#define F(a) a\n"
        "#define M F\n"
        "M(F)(C)(D)e"
        ;
    const char* output =
        "De"
        ;
    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);

    struct preprocessor_ctx ctx = { 0 };

    struct token_list r = preprocessor(&ctx, &list, 0);

    assert(test_preprocessor_in_out(input, output) == 0);
}

void recursive_macro_expansion()
{
    const char* input =
        "#define A 3 4 B\n"
        "#define B 1 2 A\n"
        "B";
    const char* output =
        "1 2 3 4 B"
        ;
    assert(test_preprocessor_in_out(input, output) == 0);
}

void empty_and_no_args()
{
    const char* input =
        "#define F() 1\n"
        "F()";
    const char* output =
        "1"
        ;
    assert(test_preprocessor_in_out(input, output) == 0);
}

void empty_and_args()
{
    const char* input =
        "#define F() 1\n"
        "F(1)";
    const char* output =
        "1"
        ;
    int code = test_preprocessor_in_out(input, output);
    assert(code != 0);
}

void test4()
{
    const char* input =
        "#define F(a, b) 1 2 ## a 4\n"
        "F(  3   )"
        ;
    const char* output =
        "1 23 4"
        ;


    int code = test_preprocessor_in_out(input, output);

    //esperado um erro (falta mensagem)
    //too few arguments provided to function-like macro invocation F (3)
    //engracado msc eh warning  warning C4003: not enough actual parameters for macro 'F'
    assert(code != 0);
}

void test_string()
{
    const char* input =
        "#define M(a, b) a # b\n"
        "M(A, \"B\")"
        ;
    const char* output =
        "A \"\\\"B\\\"\""
        ;


    test_preprocessor_in_out(input, output);
}

void test6()
{
    /*

    #define Y
    #define X defined (Y)

    #if X
    #warning !
    #endif
    */
}

void testerror()
{
    /*
    const char* input =
        "#define F(a) #b\n"
        "F(1)\n"
        ;
    const char* output =
        ""
        ;
    //tem que dar error
    test_preprocessor_in_out(input, output);
    */
}

int test_preprocessor_expression(const char* expr, long long expected)
{

    struct preprocessor_ctx ctx = { 0 };

    struct token_list r = { 0 };
    struct tokenizer_ctx tctx = { 0 };
    struct token_list input = tokenizer(&tctx, expr, "", 0, TK_FLAG_NONE);

    long long result = preprocessor_constant_expression(&ctx, &r, &input, 0);
    return result == expected ? 0 : 1;
}

int test_expression()
{

    //TODO preprocessador eh sempre long long.. signed passadno maior
    //deve dar erro

    if (test_preprocessor_expression("true", true) != 0)
        return __LINE__;

    if (test_preprocessor_expression("false", false) != 0)
        return __LINE__;


    if (test_preprocessor_expression("'A'", 'A') != 0)
        return __LINE__;

    if (test_preprocessor_expression("'ab'", 'ab') != 0)
        return __LINE__;

    if (test_preprocessor_expression("1+2", 1 + 2) != 0)
        return __LINE__;

    if (test_preprocessor_expression("1 + 2 * 3 / 2 ^ 2 & 4 | 3 % 6 >> 2 << 5 - 4 + !7",
        1 + 2 * 3 / 2 ^ 2 & 4 | 3 % 6 >> 2 << 5 - 4 + !7) != 0)
        return __LINE__;

    if (test_preprocessor_expression("1ull + 2l * 3ll",
        1ull + 2l * 3ll) != 0)
        return __LINE__;


    return 0;
}

int test_concatenation_o()
{
    const char* input =
        "# define F(t1, t2, t3) *i_##t1##_j k\n"
        "F(A, B, C)\n";

    const char* output =
        "*i_A_j k"
        ;


    return test_preprocessor_in_out(input, output);
}

int test_concatenation()
{
    const char* input =
        "#define F(t1, t2, t3) i##j##k\n"
        "F(A, B, C)\n";

    const char* output =
        "ijk"
        ;


    return test_preprocessor_in_out(input, output);


}

void bad_test()
{
    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, "0xfe-BAD(3)", "source", 0, TK_FLAG_NONE);

    const char* input = "#define BAD(x) ((x) & 0xff)\n"
        "0xfe-BAD(3);";
    const char* output =
        "0xfe-BAD(3);"
        ;

    test_preprocessor_in_out(input, output);
}
/*
#define A0
#define B0
#define A1(x) x B##x(
#define B1(x) x A##x(
A1(1)1)1)1)1)0))
*/
int test_spaces()
{
    const char* input =
        "#define throw A B\n"
        "throw\n"
        ;
    const char* output =
        "A B"
        ;


    return test_preprocessor_in_out(input, output);
}

int test_stringfy()
{
    const char* input =
        "#define M(T) #T\n"
        "M(unsigned   int)\n"
        ;
    const char* output =
        "\"unsigned int\""
        ;


    return test_preprocessor_in_out(input, output);

}


int test_tokens()
{
    const char* input =
        "L\"s1\" u8\"s2\""
        ;

    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "", 0, TK_FLAG_NONE);

    if (list.head->next->type != TK_STRING_LITERAL)
    {
        return __LINE__;
    }

    if (list.head->next->next->next->type != TK_STRING_LITERAL)
    {
        return __LINE__;
    }

    return tctx.n_errors;
}

int test_predefined_macros()
{
    const char* input =
        "__LINE__ __FILE__"
        ;
    const char* output =
        "1 \"source\""
        ;

    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);

    struct preprocessor_ctx prectx = { 0 };
    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx, CAKE_COMPILE_TIME_SELECTED_TARGET);
    struct token_list list2 = preprocessor(&prectx, &list, 0);


    const char* result = print_preprocessed_to_string(list2.head);
    if (result == NULL)
    {
        result = strdup("");
    }
    if (strcmp(result, output) != 0)
    {

    }


    return 0;
}

int test_utf8()
{

    const char* input =
        "u8\"maçã\"";

    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);
    if (strcmp(list.head->next->lexeme, u8"u8\"maçã\"") != 0)
        return __LINE__;
    token_list_destroy(&list);
    return 0;
}

int test_counter()
{
    //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3457.htm#number-of-expansions

    const char* input =
        "#define X(Z) Z Z\n"
        "X(__COUNTER__)\n";

    const char* output =
        "0 0"
        ;

    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);

    struct preprocessor_ctx prectx = { 0 };
    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx, CAKE_COMPILE_TIME_SELECTED_TARGET);
    struct token_list list2 = preprocessor(&prectx, &list, 0);

    const char* result = print_preprocessed_to_string(list2.head);
    if (result == NULL)
    {
        result = strdup("");
    }

    assert(test_preprocessor_in_out(result, output) == 0);

    return 0;
}

int bug_test()
{
    const char* input =
        "#define M(b) a #b \n"
        "M(1)\n";

    const char* output =
        "a \"1\""
        ;


    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);



    assert(test_preprocessor_in_out(input, output) == 0);

    return 0;
}
int test_line_continuation()
{


    const char* input =
        "#define A B \\\n"
        "C\n"
        "A";

    const char* output =
        "1 \"source\""
        ;


    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "", 0, TK_FLAG_NONE);

    struct preprocessor_ctx prectx = { 0 };
    prectx.macros.capacity = 5000;

    struct token_list list2 = preprocessor(&prectx, &list, 0);

    const char* result = print_preprocessed_to_string(list2.head);
    if (result == NULL)
    {
        result = strdup("");
    }
    if (strcmp(result, output) != 0)
    {
    }


    return 0;
}

int stringify_test()
{
    char buffer[200];
    int n = stringify("\"ab\\c\"", sizeof buffer, buffer);
    assert(n == sizeof(STRINGIFY("\"ab\\c\"")));
    const char* r = STRINGIFY("\"ab\\c\"");

    assert(strcmp(buffer, r) == 0);
    return 0;

}

void recursive_macro_expr()
{

    const char* input =
        "#define A A\n"
        "#if A == 0\n"
        "1\n"
        "#endif";

    const char* output =
        "1"
        ;

    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);

    assert(test_preprocessor_in_out(input, output) == 0);

}

#endif
