
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
static const bool INCLUDE_ALL = true;

///////////////////////////////////////////////////////////////////////////////
void naming_convention_macro(struct preprocessor_ctx* ctx, struct token* token);
///////////////////////////////////////////////////////////////////////////////



struct macro_parameter
{
    const char* owner name;
    struct macro_parameter* owner next;
};



struct macro
{
    const char* owner name;
    struct token_list replacement_list; /*copia*/
    struct macro_parameter* owner parameters;
    bool is_function;
    int usage;

    bool expand;
};


void delete_macro(struct macro* owner macro);

static void delete_macro_void(void* owner p)
{
    struct macro* owner p_macro = p;
    delete_macro(p_macro);
}

void include_dir_list_destroy(struct include_dir_list* obj_owner list)
{
    struct include_dir* owner p = list->head;
    while (p)
    {
        struct include_dir* owner next = p->next;
        free((void* owner)p->path);
        free(p);
        p = next;
    }
}

void preprocessor_ctx_destroy(struct preprocessor_ctx* obj_owner p)
{
    owner_hashmap_destroy(&p->macros, delete_macro_void);
    include_dir_list_destroy(&p->include_dir);
    hashmap_destroy(&p->pragma_once_map);
    token_list_destroy(&p->input_list);
}

struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level);

static void tokenizer_set_error(struct tokenizer_ctx* ctx, struct stream* stream, const char* fmt, ...)
{
    ctx->n_errors++;
#ifndef TEST
    char buffer[200] = { 0 };
    va_list args = {0};
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    print_position(stream->path, stream->line, stream->col, ctx->options.visual_studio_ouput_format);
    if (ctx->options.visual_studio_ouput_format)
    {
        printf("error: "  "%s\n", buffer);
    }
    else
    {
        printf(LIGHTRED "error: " WHITE "%s\n", buffer);
    }
#endif
}


static void tokenizer_set_warning(struct tokenizer_ctx* ctx, struct stream* stream, const char* fmt, ...)
{
    ctx->n_warnings++;

#ifndef TEST
    char buffer[200] = { 0 };
    va_list args= {0};
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    print_position(stream->path, stream->line, stream->col, ctx->options.visual_studio_ouput_format);
    if (ctx->options.visual_studio_ouput_format)
    {
        printf("warning: " "%s\n", buffer);
    }
    else
    {
        printf(LIGHTMAGENTA "warning: " WHITE "%s\n", buffer);
    }
#endif
}


bool preprocessor_diagnostic_message(enum diagnostic_id w, struct preprocessor_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    bool is_error = false;
    bool is_warning = false;
    bool is_note = false;

    if (p_token && p_token->level != 0)
    {
        //no message for include dir
        return false;
    }

    if (w > W_NOTE)
    {
        is_error = true;
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





#ifndef TEST

    if (p_token)
        print_position(p_token->token_origin->lexeme, p_token->line, p_token->col, ctx->options.visual_studio_ouput_format);

    char buffer[200] = { 0 };
    va_list args= {0};
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (ctx->options.visual_studio_ouput_format)
    {
        if (is_warning)
            printf("warning: " "%s\n", buffer);
        else if (is_error)
            printf("warning: " "%s\n", buffer);
        else if (is_note)
            printf("note: " "%s\n", buffer);

        print_line_and_token(p_token, ctx->options.visual_studio_ouput_format);
    }
    else
    {
        if (is_error)
            printf(LIGHTRED "error: " WHITE "%s\n", buffer);
        else if (is_warning)
            printf(LIGHTMAGENTA "warning: " WHITE "%s\n", buffer);
        else if (is_note)
            printf(LIGHTCYAN "note: " WHITE "%s\n", buffer);

        print_line_and_token(p_token, ctx->options.visual_studio_ouput_format);

    }
#endif
    return true;
}



struct include_dir* include_dir_add(struct include_dir_list* list, const char* path)
{
    struct include_dir* owner p_new_include_dir = calloc(1, sizeof * p_new_include_dir);
    p_new_include_dir->path = strdup(path);
    if (list->head == NULL)
    {
        list->head = p_new_include_dir;
        list->tail = p_new_include_dir;
    }
    else
    {
        assert(list->tail->next == NULL);
        list->tail->next = p_new_include_dir;
        list->tail = p_new_include_dir;
    }
    return list->tail;
}


const char* owner find_and_read_include_file(struct preprocessor_ctx* ctx,
    const char* path, /*as in include*/
    const char* current_file_dir, /*this is the dir of the file that includes*/
    bool* p_already_included, /*out file alread included pragma once*/
    char full_path_out[], /*this is the final full path of the file*/
    int full_path_out_size)
{

    full_path_out[0] = '\0';

    if (path_is_absolute(path))
    {
        //todo realpath
        if (hashmap_find(&ctx->pragma_once_map, path) != NULL)
        {
            *p_already_included = true;
            return NULL;
        }

        char* owner content = read_file(path);
        if (content != NULL)
        {
            snprintf(full_path_out, full_path_out_size, "%s", path);
            return content;
        }
        return NULL;
    }



    char newpath[200] = { 0 };
    snprintf(newpath, sizeof newpath, "%s/%s", current_file_dir, path);

#ifdef __EMSCRIPTEN__
    /*realpath returns empty on emscriptem*/
    snprintf(full_path_out, full_path_out_size, "%s", newpath);
#else
    if (!realpath(newpath, full_path_out))
        full_path_out[0] = '\0';
#endif


    if (hashmap_find(&ctx->pragma_once_map, full_path_out) != NULL)
    {
        *p_already_included = true;
        return NULL;
    }

    char* owner content = read_file(full_path_out);
    if (content != NULL)
        return content;


    struct include_dir* current = ctx->include_dir.head;
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

        if (hashmap_find(&ctx->pragma_once_map, full_path_out) != NULL)
        {
            *p_already_included = true;
            return NULL;
        }

        content = read_file(full_path_out);
        if (content != NULL)
        {
            return content;
        }
        current = current->next;
    }
    full_path_out[0] = '\0';
    return NULL;
}





/*usado para verificar recursividade*/
struct macro_expanded
{
    const char* name;
    struct macro_expanded* p_previous;
};


void add_macro(struct preprocessor_ctx* ctx, const char* name)
{
    struct macro* owner macro = calloc(1, sizeof * macro);
    if (macro != NULL)
    {
        macro->name = strdup(name);
        struct macro* owner previous = (struct macro* owner)owner_hashmap_set(&ctx->macros, name, (void* owner) macro, 0);
        if (previous)
        {
            delete_macro(previous);
            previous = NULL;
        }
    }
}

struct macro_argument
{
    const char* owner name;
    struct token_list tokens;
    struct macro_argument* owner next; /*linked list*/
};
void macro_argument_delete(struct macro_argument* owner p);


struct token_list copy_replacement_list(struct token_list* list);

struct token_list copy_argument_list_tokens(struct token_list* list)
{
    //Faz uma copia dos tokens fazendo um trim no iniico e fim
    //qualquer espaco coments etcc vira um unico  espaco
    struct token_list r = { 0 };
    struct token* current = list->head;
    //sai de cima de todos brancos iniciais
    while (current &&
        (token_is_blank(current) ||
            current->type == TK_NEWLINE))
    {
        current = current->next;
    }
    //remover flag de espaco antes se tiver
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

void macro_argument_delete(struct macro_argument* owner p)
{
    if (p)
    {
        assert(p->next == NULL);
        token_list_destroy(&p->tokens);
        free((void* owner) p->name);
        free(p);
    }
}

struct token_list copy_argument_list(struct macro_argument* p_macro_argument)
{
    struct token_list list = copy_argument_list_tokens(&p_macro_argument->tokens);
    if (list.head == NULL)
    {
        /*nunca eh vazio..se for ele colocar um TK_PLACEMARKER*/
        struct token* owner p_new_token = calloc(1, sizeof * p_new_token);
        if (p_new_token)
        {
            p_new_token->lexeme = strdup("");
            p_new_token->type = TK_PLACEMARKER;
            token_list_add(&list, p_new_token);

        }
    }
    //print_list(&list);
    return list;
}


struct macro_argument_list
{
    struct token_list tokens;
    struct macro_argument* owner head;
    struct macro_argument* tail;
};

void macro_argument_list_destroy(struct macro_argument_list* obj_owner list)
{
    token_list_destroy(&list->tokens);
    struct macro_argument* owner p = list->head;
    while (p)
    {
        struct macro_argument* owner next = p->next;
        p->next = NULL;
        macro_argument_delete(p);
        p = next;
    }
}

void print_macro_arguments(struct macro_argument_list* arguments)
{
    struct macro_argument* p_argument = arguments->head;
    while (p_argument)
    {
        printf("%s:", p_argument->name);
        print_list(&p_argument->tokens);
        p_argument = p_argument->next;
    }
}

struct macro_argument* find_macro_argument_by_name(struct macro_argument_list* parameters, const char* name)
{
    /*
    * Os argumentos são coletados na expansão da macro e cada um (exceto ...)
    * é associado a um dos parametros da macro.
    */
    struct macro_argument* p = parameters->head;
    while (p)
    {
        if (strcmp(p->name, name) == 0)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}


void argument_list_add(struct macro_argument_list* list, struct macro_argument* owner pnew)
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
    struct macro_parameter* parameter = macro->parameters;
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

void macro_parameters_delete(struct macro_parameter* owner parameters)
{
    struct macro_parameter* owner p = parameters;
    while (p)
    {
        struct macro_parameter* owner p_next = p->next;
        free((void* owner)p->name);
        free(p);
        p = p_next;
    }
}

void delete_macro(struct macro* owner macro)
{
    if (macro)
    {
        token_list_destroy(&macro->replacement_list);

        struct macro_parameter* owner p_macro_parameter = macro->parameters;
        while (p_macro_parameter)
        {
            struct macro_parameter* owner p_next = p_macro_parameter->next;
            free((void* owner)p_macro_parameter->name);
            free(p_macro_parameter);
            p_macro_parameter = p_next;
        }

        free((void* owner) macro->name);
        free(macro);
    }
}

struct macro* find_macro(struct preprocessor_ctx* ctx, const char* name)
{
    struct owner_map_entry* p_entry = owner_hashmap_find(&ctx->macros, name);
    if (p_entry == NULL)
        return NULL;

    return p_entry->p;
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

    while (stream->current[0] == '\\' && stream->current[1] == '\n')
    {
        /*
            2. Each instance of a backslash character (\) immediately followed by a new-line character is
            deleted, splicing physical source lines to form logical source lines. Only the last backslash on
            any physical source line shall be eligible for being part of such a splice. A source file that is
            not empty shall end in a new-line character, which shall not be immediately preceded by a
            backslash character before any such splicing takes place.
        */
        stream->current++;
        stream->current++;
        stream->line++;
        stream->col = 1;

        stream->line_continuation_count++;
    }

}

void print_line(struct token* p)
{
    printf("%s\n", p->token_origin->lexeme);
    struct token* prev = p;
    while (prev->prev && prev->prev->type != TK_NEWLINE)
    {
        prev = prev->prev;
    }
    struct token* next = prev;
    while (next && next->type != TK_NEWLINE)
    {
        printf("%s", next->lexeme);
        next = next->next;
    }
    printf("\n");
}

int is_nondigit(struct stream* p)
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


struct token* owner new_token(const char* lexeme_head, const char* lexeme_tail, enum token_type type)
{
    struct token* owner p_new_token = calloc(1, sizeof * p_new_token);
    size_t sz = lexeme_tail - lexeme_head;
    p_new_token->lexeme = calloc(sz + 1, sizeof(char));
    p_new_token->type = type;
    strncpy(p_new_token->lexeme, lexeme_head, sz);
    return p_new_token;
}

struct token* owner identifier(struct stream* stream)
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

    struct token* owner p_new_token = new_token(start, stream->current, TK_IDENTIFIER);


    return p_new_token;
}




bool first_of_character_constant(struct stream* stream)
{
    return stream->current[0] == '\'' ||
        (stream->current[0] == 'u' && stream->current[1] == '8' && stream->current[2] == '\'') ||
        (stream->current[0] == 'u' && stream->current[1] == '\'') ||
        (stream->current[0] == 'U' && stream->current[1] == '\'') ||
        (stream->current[0] == 'L' && stream->current[1] == '\'');
}

struct token* owner character_constant(struct tokenizer_ctx* ctx, struct stream* stream)
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
    struct token* owner p_new_token = new_token(start, stream->current, TK_CHAR_CONSTANT);
    return p_new_token;
}

bool first_of_string_literal(struct stream* stream)
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

struct token* owner string_literal(struct tokenizer_ctx* ctx, struct stream* stream)
{
    struct token* owner p_new_token = NULL;
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

int string_literal_byte_size(const char* s)
{

    struct stream stream = { .source = s,
        .current = s,
        .line = 1,
        .col = 1 };

    int size = 0;
    int charsize = sizeof(char);

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
            charsize = sizeof(wchar_t);
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

    size++; /* /0 included */

    return size * charsize;
}

struct token* owner ppnumber(struct stream* stream)
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
    struct token* owner p_new_token = new_token(start, stream->current, TK_PPNUMBER);
    return p_new_token;
}

struct token_list embed_tokenizer(struct preprocessor_ctx* ctx, const char* filename_opt, int level, enum token_flags addflags)
{
    struct token_list list = { 0 };

    FILE* owner file = NULL;

    bool b_first = true;
    int line = 1;
    int col = 1;
    int count = 0;
    try
    {
#ifndef MOCKFILES
        file = (FILE * owner)fopen(filename_opt, "rb");
        if (file == NULL)
        {
            preprocessor_diagnostic_message(C_ERROR_FILE_NOT_FOUND, ctx, ctx->current, "file '%s' not found", filename_opt);
            throw;
        }
#else
        /*web versions only text files that are included*/
        char* textfile = read_file(filename_opt);
        if (textfile == NULL)
        {
            preprocessor_diagnostic_message(C_ERROR_FILE_NOT_FOUND, ctx, ctx->current, "file '%s' not found", filename_opt);
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
                struct token* owner p_new_token = new_token(b, &b[1], TK_COMMA);
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
                    struct token* owner p_new3 = new_token(newline, &newline[1], TK_NEWLINE);
                    p_new3->level = level;
                    p_new3->token_origin = NULL;
                    p_new3->line = line;
                    p_new3->col = col;
                    token_list_add(&list, p_new3);
                }
            }

            char buffer[30] = { 0 };
            int c = snprintf(buffer, sizeof buffer, "%d", (int) ch);

            struct token* owner p_new_token = new_token(buffer, &buffer[c], TK_PPNUMBER);
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
    }
    catch
    {
    }

    /*new line*/
    char newline[] = "\n";
    struct token* owner p_new_token = new_token(newline, &newline[1], TK_NEWLINE);
    p_new_token->level = level;
    p_new_token->token_origin = NULL;
    p_new_token->line = line;
    p_new_token->col = col;
    token_list_add(&list, p_new_token);

    if (file) fclose(file);



    assert(list.head != NULL);
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

struct token_list tokenizer(struct tokenizer_ctx* ctx, const char* text, const char* filename_opt, int level, enum token_flags addflags)
{
    struct token_list list = { 0 };
    if (text == NULL)
    {
        return list;
    }

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
        struct token* p_first = NULL;
        if (filename_opt != NULL)
        {
            const char* begin = filename_opt;
            const char* end = filename_opt + strlen(filename_opt);
            struct token* owner p_new = new_token(begin, end, TK_BEGIN_OF_FILE);
#ifdef _WINDOWS_
            //windows have case insensive paths
            for (char* p = p_new->lexeme; *p; p++)
            {
                *p = (char) tolower(*p);
            }
#endif
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
                struct token* owner p_new_token = ppnumber(&stream);
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
                struct token* owner p_new_token = string_literal(ctx, &stream);
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
                struct token* owner p_new_token = character_constant(ctx, &stream);
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
                struct token* owner p_new_token = identifier(&stream);
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
                struct token* owner p_new_token = new_token(start, stream.current, TK_BLANKS);
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
                struct token* owner p_new_token = new_token(start, stream.current, TK_LINE_COMMENT);
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
                struct token* owner p_new_token = new_token(start, stream.current, TK_COMMENT);
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
                struct token* owner p_new_token = new_token(start, stream.current, '#');
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
                struct token* owner p_new_token = new_token(newline, newline + 1, TK_NEWLINE);
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

                struct token* owner p_new_token = new_token(start, stream.current, t);
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
                struct token* owner p_new_token = new_token(start, stream.current, ANY_OTHER_PP_TOKEN);
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






struct token* preprocessor_look_ahead_core(struct token* p)
{
    if (p->next == NULL)
    {
        return NULL;
    }
    struct token* current = p->next;
    if (current == NULL)
        return NULL;
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
    struct token* p_token = preprocessor_look_ahead_core(p);
    if (p_token != NULL && p_token->type == t)
        return true;
    return false;
}

bool preprocessor_token_previous_is(struct token* p, enum token_type t)
{
    if (p == NULL)
    {
        return false;
    }
    struct token* current = p->prev;
    if (current == NULL)
        return false;
    while (current &&
        (current->type == TK_BLANKS ||
            current->type == TK_LINE_COMMENT ||
            current->type == TK_PLACEMARKER ||
            current->type == TK_COMMENT))
    {
        current = current->prev;
    }
    return (current && current->type == t);
}

bool preprocessor_token_ahead_is_identifier(struct token* p, const char* lexeme)
{
    assert(p != NULL);
    struct token* p_token = preprocessor_look_ahead_core(p);
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
            token_list_add(dest, token_list_pop_front_get(input_list));
        else
            token_list_pop_front(input_list); //deletar
    }
}

static void skip_blanks(struct preprocessor_ctx* ctx, struct token_list* dest, struct token_list* input_list)
{
    while (input_list->head)
    {
        if (!token_is_blank(input_list->head))
            break;
        token_list_add(dest, token_list_pop_front_get(input_list));
    }
}

void prematch_level(struct token_list* dest, struct token_list* input_list, int level)
{
    if (INCLUDE_ALL || level == 0)
        token_list_add(dest, token_list_pop_front_get(input_list));
    else
        token_list_pop_front(input_list);
}

void prematch(struct token_list* dest, struct token_list* input_list)
{
    token_list_add(dest, token_list_pop_front_get(input_list));
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

                bool has_parentesis = false;
                if (input_list->head->type == '(')
                {
                    token_list_pop_front(input_list);
                    has_parentesis = true;
                }

                skip_blanks(ctx, &r, input_list);



                struct macro* macro = find_macro(ctx, input_list->head->lexeme);
                struct token* owner p_new_token = token_list_pop_front_get(input_list);
                p_new_token->type = TK_PPNUMBER;
                free(p_new_token->lexeme);
                if (macro)
                {
                    p_new_token->lexeme = strdup("1");
                }
                else
                {
                    p_new_token->lexeme = strdup("0");
                }

                token_list_add(&r, p_new_token);

                if (has_parentesis)
                {
                    if (input_list->head->type != ')')
                    {
                        preprocessor_diagnostic_message(C_ERROR_MISSING_CLOSE_PARENTHESIS, ctx, input_list->head, "missing )");
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

                if (input_list->head->type == TK_STRING_LITERAL)
                {
                    strcat(path, input_list->head->lexeme);
                    token_list_pop_front(input_list); //pop "file"
                }
                else
                {
                    token_list_pop_front(input_list); //pop <

                    while (input_list->head->type != '>')
                    {
                        strcat(path, input_list->head->lexeme);
                        token_list_pop_front(input_list); //pop (
                    }
                    token_list_pop_front(input_list); //pop >
                }

                char fullpath[300] = { 0 };



                char full_path_result[200] = { 0 };
                bool already_included = false;
                const char* owner s = find_and_read_include_file(ctx,
                    path,
                    fullpath,
                    &already_included,
                    full_path_result,
                    sizeof full_path_result);

                bool has_include = s != NULL;
                free((void* owner)s);

                struct token* owner p_new_token = calloc(1, sizeof * p_new_token);
                p_new_token->type = TK_PPNUMBER;
                p_new_token->lexeme = strdup(has_include ? "1" : "0");
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


                char path[100] = { 0 };
                while (input_list->head->type != ')')
                {
                    strcat(path, input_list->head->lexeme);
                    token_list_pop_front(input_list); //pop (
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

                struct token* owner p_new_token = calloc(1, sizeof * p_new_token);
                p_new_token->type = TK_PPNUMBER;
                p_new_token->lexeme = strdup(has_c_attribute ? "1" : "0");
                p_new_token->flags |= TK_FLAG_FINAL;

                token_list_add(&r, p_new_token);
                token_list_pop_front(input_list); //pop )
            }
            else
            {
                token_list_add(&r, token_list_pop_front_get(input_list));
            }
        }
    }
    catch
    {
    }

    return r;
}

struct token_list process_identifiers(struct preprocessor_ctx* ctx, struct token_list* obj_owner list)
{
    assert(!token_list_is_empty(list));

    struct token_list list2 = { 0 };


    while (list->head != NULL)
    {
        if (list->head->type == TK_IDENTIFIER)
        {

            struct macro* macro = find_macro(ctx, list->head->lexeme);
            struct token* owner p_new_token = token_list_pop_front_get(list);
            p_new_token->type = TK_PPNUMBER;

            if (macro)
            {
                free(p_new_token->lexeme);
                p_new_token->lexeme = strdup("1");
            }
            else
            {
                /*
                * after all replacements due to macro expansion and
                  evaluations of defined macro expressions, has_include expressions, and has_c_attribute expressions
                  have been performed, all remaining identifiers other than true (including those lexically identical
                  to keywords such as false) are replaced with the pp-number 0, true is replaced with pp-number
                  1, and then each preprocessing token is converted into a token.
                */
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
                    free(p_new_token->lexeme);
                    p_new_token->lexeme = strdup("0");
                }
            }
            token_list_add(&list2, p_new_token);
        }
        else
        {
            token_list_add(&list2, token_list_pop_front_get(list));
        }
    }
    assert(!token_list_is_empty(&list2));
    return list2;
}

struct token_list ignore_preprocessor_line(struct token_list* input_list)
{
    struct token_list r = { 0 };
    while (input_list->head->type != TK_NEWLINE)
    {
        token_list_add(&r, token_list_pop_front_get(input_list));
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
    struct token* first = input_list->head;

    ctx->conditional_inclusion = true;

    struct token_list r = { 0 };
    while (input_list->head && input_list->head->type != TK_NEWLINE)
    {
        token_list_add(&r, token_list_pop_front_get(input_list));

        /*
          We call preprocessor that emmit warnings if line continuation
          is used outside macro directives.
          Let's remove TK_FLAG_LINE_CONTINUATION from the original token
          to avoid warning inside constant expressions
        */
        r.tail->flags &= ~TK_FLAG_LINE_CONTINUATION;
    }

    struct token_list list1 = copy_replacement_list(&r);
    token_list_swap(output_list, &r);


    int flags = ctx->flags;
    ctx->flags |= PREPROCESSOR_CTX_FLAGS_ONLY_FINAL;

    /*defined X  por exemplo é mantido sem ser expandido*/

    struct token_list list2 = preprocessor(ctx, &list1, 1);
    ctx->flags = flags;

    long long value = 0;

    if (list2.head == NULL)
    {
        preprocessor_diagnostic_message(C_ERROR_EXPRESSION_ERROR, ctx, first, "empty expression");
    }
    else
    {

        /*aonde defined has_c_aatribute sao transformados em constantes*/
        struct token_list list3 = process_defined(ctx, &list2);


        struct token_list list4 = process_identifiers(ctx, &list3);


        assert(list4.head != NULL);

        struct preprocessor_ctx pre_ctx = { 0 };


        pre_ctx.input_list = list4;
        pre_ctx.current = pre_ctx.input_list.head;

        if (pre_constant_expression(&pre_ctx, &value) != 0)
        {
            preprocessor_diagnostic_message(C_ERROR_EXPRESSION_ERROR, ctx, first, "expression error");
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
        token_list_add(dest, token_list_pop_front_get(input_list));
    else
        token_list_pop_front(input_list); //deletar
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
                    preprocessor_diagnostic_message(C_ERROR_UNEXPECTED_TOKEN, ctx, input_list->head, "expected token %s got %s\n", get_token_name(type), get_token_name(input_list->head->type));
                else
                    preprocessor_diagnostic_message(C_ERROR_UNEXPECTED_TOKEN, ctx, dest->tail, "expected EOF \n");

                throw;
            }
        }
        if (input_list->head != NULL)
        {
            if (INCLUDE_ALL || level == 0)
                token_list_add(dest, token_list_pop_front_get(input_list));
            else
                token_list_pop_front(input_list); //deletar
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
        assert(input_list->head->type == TK_IDENTIFIER);
        if (strcmp(input_list->head->lexeme, "ifdef") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //ifdef
            skip_blanks_level(ctx, &r, input_list, level);
            if (is_active)
            {
                struct macro* macro = find_macro(ctx, input_list->head->lexeme);
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
            if (is_active)
            {
                struct macro* macro = find_macro(ctx, input_list->head->lexeme);
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

            preprocessor_diagnostic_message(C_ERROR_UNEXPECTED, ctx, input_list->head, "unexpected");
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

    struct token_list r = { 0 };
    /*
     elif-group:
      # elif constant-expression new-line group_opt

      C23
      # elifdef identifier new-line group_opt
      # elifndef identifier new-line group_opt
    */
    match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx);
    skip_blanks(ctx, &r, input_list);
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

        if (is_active)
        {
            result = (owner_hashmap_find(&ctx->macros, input_list->head->lexeme) != NULL) ? 1 : 0;
        }
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
    }
    else if (strcmp(input_list->head->lexeme, "elifndef") == 0)
    {
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
        skip_blanks(ctx, &r, input_list);

        if (is_active)
        {
            result = (owner_hashmap_find(&ctx->macros, input_list->head->lexeme) == NULL) ? 1 : 0;
        }
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
    }
    *p_elif_result = (result != 0);
    skip_blanks(ctx, &r, input_list);
    match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
    struct token_list r2 = group_opt(ctx, input_list, is_active && *p_elif_result, level);
    token_list_append_list(&r, &r2);
    token_list_destroy(&r2);
    return r;
}

struct token_list elif_groups(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, bool* pelif_result)
{
    struct token_list r = { 0 };
    /*
    elif-groups:
      elif-group
      elif-groups elif-group
    */
    bool already_found_elif_true = false;
    bool elif_result = false;
    struct token_list r2 = elif_group(ctx, input_list, is_active, level, &elif_result);
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
    return r;
}

struct token_list else_group(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    /*
      else-group:
       # else new-line group_opt
    */

    struct token_list r = { 0 };
    match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx);
    skip_blanks_level(ctx, &r, input_list, level);

    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //else
    skip_blanks_level(ctx, &r, input_list, level);
    match_token_level(&r, input_list, TK_NEWLINE, level, ctx);

    struct token_list r2 = group_opt(ctx, input_list, is_active, level);
    token_list_append_list(&r, &r2);

    token_list_destroy(&r2);

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

struct token_list if_section(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level)
{
    /*
     if-section:
       if-group elif-groups_opt else-group_opt endif-line
    */

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
    struct token_list r = { 0 };
    /*
      identifier-list:
      identifier
      identifier-list , identifier
    */
    skip_blanks(ctx, &r, input_list);

    struct macro_parameter* owner p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
    p_macro_parameter->name = strdup(input_list->head->lexeme);

    assert(macro->parameters == NULL);
    macro->parameters = p_macro_parameter;

    struct macro_parameter* p_last_parameter = macro->parameters;

    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
    skip_blanks(ctx, &r, input_list);

    while (input_list->head->type == ',')
    {
        match_token_level(&r, input_list, ',', level, ctx);
        skip_blanks(ctx, &r, input_list);
        if (input_list->head->type == '...')
        {
            break;
        }

        struct macro_parameter* owner p_new_macro_parameter = calloc(1, sizeof * p_new_macro_parameter);
        p_new_macro_parameter->name = strdup(input_list->head->lexeme);

        assert(p_last_parameter->next == NULL);
        p_last_parameter->next = p_new_macro_parameter;
        p_last_parameter = p_last_parameter->next;

        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
        skip_blanks(ctx, &r, input_list);
    }
    return r;
}


struct token_list replacement_list(struct preprocessor_ctx* ctx, struct macro* macro, struct token_list* input_list, int level)
{
    struct token_list r = { 0 };

    while (input_list->head->type != TK_NEWLINE)
    {
        match_level(&r, input_list, level);
        if (input_list->head == NULL)
        {
            //terminou define sem quebra de linha
        }
    }

    assert(macro->replacement_list.head == NULL);
    struct token_list copy = copy_replacement_list(&r);
    token_list_append_list(&macro->replacement_list, &copy);
    token_list_destroy(&copy);
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
    struct token* token = replacement_list->head;

    if (token == NULL) return false;
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
            # "define" identifier ( identifier-list opt ) replacement-list new-line
            # "define" identifier ( ... ) replacement-list new-line
            # "define" identifier ( identifier-list , ... ) replacement-list new-line
            # "undef" identifier new-line
            # "line" pp-tokens new-line
            # "error" pp-tokens opt new-line
            # "warning" pp-tokens opt new-line
            # "pragma" pp-tokens opt new-line
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

#ifdef _WIN32
        //char line[1000] = { 0 };
        //snprintf(line, sizeof line, "%s(%d,%d):\n", input_list->head->token_origin->lexeme, input_list->head->line, input_list->head->col);
        //OutputDebugStringA(line);
#endif
        struct token* const ptoken = input_list->head;
        match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx);
        skip_blanks_level(ctx, &r, input_list, level);
        if (strcmp(input_list->head->lexeme, "include") == 0)
        {
            /*
              # include pp-tokens new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //include
            skip_blanks_level(ctx, &r, input_list, level);
            char path[100] = { 0 };

            if (input_list->head->type == TK_STRING_LITERAL)
            {
                strcat(path, input_list->head->lexeme);
                prematch_level(&r, input_list, level);
            }
            else
            {
                while (input_list->head->type != '>')
                {
                    strcat(path, input_list->head->lexeme);
                    prematch_level(&r, input_list, level);
                }
                strcat(path, input_list->head->lexeme);
                prematch_level(&r, input_list, level);
            }

            if (input_list->head)
            {
                while (input_list->head->type != TK_NEWLINE)
                {
                    prematch_level(&r, input_list, level);
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
            const char* owner content = find_and_read_include_file(ctx,
                path + 1,
                current_file_dir,
                &already_included,
                full_path_result,
                sizeof full_path_result);

            if (content != NULL)
            {
                if (ctx->options.show_includes)
                {
                    printf("Note: including file:");
                    printf("%*c", level + 1, ' ');
                    printf("%s\n", full_path_result);
                }

                struct tokenizer_ctx tctx = { 0 };
                struct token_list list = tokenizer(&tctx, content, full_path_result, level + 1, TK_FLAG_NONE);
                free((void* owner)content);

                struct token_list list2 = preprocessor(ctx, &list, level + 1);
                token_list_append_list(&r, &list2);

                token_list_destroy(&list2);
                token_list_destroy(&list);
            }
            else
            {
                if (!already_included)
                {
                    preprocessor_diagnostic_message(C_ERROR_FILE_NOT_FOUND, ctx, r.tail, "file %s not found", path + 1);

                    for (struct include_dir* p = ctx->include_dir.head; p; p = p->next)
                    {
                        /*let's print the include path*/
                        preprocessor_diagnostic_message(W_NOTE, ctx, r.tail, "dir = '%s'", p->path);
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
            if (ctx->options.target < LANGUAGE_C2X)
            {
                p_list = &discard0;

                free(ptoken->lexeme);
                ptoken->lexeme = strdup(" ");

            }

            /*
              C23
              # embed pp-tokens new-line
            */
            match_token_level(p_list, input_list, TK_IDENTIFIER, level, ctx); //embed
            skip_blanks_level(ctx, p_list, input_list, level);
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
                }
                strcat(path, input_list->head->lexeme);
                prematch_level(p_list, input_list, level);
            }

            if (input_list->head)
            {
                while (input_list->head->type != TK_NEWLINE)
                {
                    prematch_level(p_list, input_list, level);
                }
            }
            match_token_level(p_list, input_list, TK_NEWLINE, level, ctx);



            char fullpath[300] = { 0 };
            path[strlen(path) - 1] = '\0';

            snprintf(fullpath, sizeof(fullpath), "%s", path + 1);


            int nlevel = level;

            enum token_flags f = 0;
            if (ctx->options.target < LANGUAGE_C2X)
            {
                //we can see it
                f = TK_FLAG_FINAL;
            }
            else
            {
                f = TK_FLAG_FINAL;
                //we cannot see it just like include
                nlevel = nlevel + 1;
            }

            struct token_list list = embed_tokenizer(ctx, fullpath, nlevel, f);


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

            struct macro* owner macro = calloc(1, sizeof * macro);
            if (macro == NULL)
            {
                preprocessor_diagnostic_message(C_ERROR_UNEXPECTED, ctx, ctx->current, "out of mem");
                throw;
            }

            /*
                # define identifier                           replacement-list new-line
                # define identifier ( identifier-list_opt )    replacement-list new-line
                # define identifier ( ... )                   replacement-list new-line
                # define identifier ( identifier-list , ... ) replacement-list new-line
            */
            //p = preprocessor_match_identifier(p, is_active, level, false, "define");

            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //define
            skip_blanks_level(ctx, &r, input_list, level);

            // printf("define %s\n%s : %d\n", input_list->head->lexeme, input_list->head->token_origin->lexeme, input_list->head->line);

            struct token* macro_name_token = input_list->head;


            if (owner_hashmap_find(&ctx->macros, input_list->head->lexeme) != NULL)
            {
                //printf("warning: '%s' macro redefined at %s %d\n",
                  //     input_list->head->lexeme,
                    ///   input_list->head->token_origin->lexeme,
                      // input_list->head->line);
            }

            assert(macro->name == NULL);
            macro->name = strdup(input_list->head->lexeme);


            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx); //nome da macro


            /*sem skip*/
            //p = preprocessor_match_token(p, is_active, level, false, IDENTIFIER); /*name*/
            if (input_list->head->type == '(')
            {

                macro->is_function = true;


                match_token_level(&r, input_list, '(', level, ctx);
                skip_blanks_level(ctx, &r, input_list, level);
                if (input_list->head->type == '...')
                {
                    struct macro_parameter* owner p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
                    p_macro_parameter->name = strdup("__VA_ARGS__");
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
                    if (input_list->head->type == '...')
                    {
                        struct macro_parameter* owner p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
                        p_macro_parameter->name = strdup("__VA_ARGS__");
                        struct macro_parameter* p_last = macro->parameters;
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

                    struct macro_parameter* owner p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
                    p_macro_parameter->name = strdup("__VA_ARGS__");
                    macro->parameters = p_macro_parameter;

                    token_list_destroy(&macro->replacement_list);
                    struct tokenizer_ctx tctx = { 0 };
                    macro->replacement_list = tokenizer(&tctx, "assert(__VA_ARGS__)", NULL, level, TK_FLAG_NONE);
                }
            }

            if (macro_name_token)
                naming_convention_macro(ctx, macro_name_token);

            struct macro* owner previous =
                owner_hashmap_set(&ctx->macros, macro->name, (void* owner)macro, 0);

            if (previous)
            {
                delete_macro(previous);
                previous = NULL;
            }

        }
        else if (strcmp(input_list->head->lexeme, "undef") == 0)
        {
            /*
             # undef identifier new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//undef
            skip_blanks_level(ctx, &r, input_list, level);

            struct macro* owner macro = (struct macro* owner) owner_hashmap_remove(&ctx->macros, input_list->head->lexeme, NULL);
            assert(find_macro(ctx, input_list->head->lexeme) == NULL);
            if (macro)
            {
                delete_macro(macro);
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
            ctx->n_warnings++;
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//error
            struct token_list r6 = pp_tokens_opt(ctx, input_list, level);
            preprocessor_diagnostic_message(C_ERROR_PREPROCESSOR_C_ERROR_DIRECTIVE, ctx, input_list->head, "#error");
            token_list_append_list(&r, &r6);
            token_list_destroy(&r6);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);


        }
        else if (strcmp(input_list->head->lexeme, "warning") == 0)
        {
            /*
              # warning pp-tokensopt new-line
            */
            ctx->n_warnings++;
            if (ctx->options.target < LANGUAGE_C2X)
            {
                /*insert comment before #*/
                free(ptoken->lexeme);
                ptoken->lexeme = strdup("//#");
            }
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//warning

            struct token_list r6 = pp_tokens_opt(ctx, input_list, level);
            preprocessor_diagnostic_message(W_NONE, ctx, input_list->head, "#warning");
            token_list_append_list(&r, &r6);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
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
            r.tail->type = TK_PRAGMA;
            r.tail->flags |= TK_FLAG_FINAL;
            skip_blanks_level(ctx, &r, input_list, level);

            if (input_list->head->type == TK_IDENTIFIER)
            {
                if (strcmp(input_list->head->lexeme, "CAKE") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
                    r.tail->flags |= TK_FLAG_FINAL;
                    skip_blanks_level(ctx, &r, input_list, level);
                }

                if (input_list->head && strcmp(input_list->head->lexeme, "once") == 0)
                {
                    hashmap_set(&ctx->pragma_once_map, input_list->head->token_origin->lexeme, (void*) 1, 0);
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma
                    r.tail->flags |= TK_FLAG_FINAL;
                }
                else if (input_list->head && strcmp(input_list->head->lexeme, "dir") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma
                    skip_blanks_level(ctx, &r, input_list, level);
                    char path[200] = { 0 };
                    strncpy(path, input_list->head->lexeme + 1, strlen(input_list->head->lexeme) - 2);
                    include_dir_add(&ctx->include_dir, path);
                    match_token_level(&r, input_list, TK_STRING_LITERAL, level, ctx);//pragma
                    r.tail->flags |= TK_FLAG_FINAL;
                }
                else if (input_list->head && strcmp(input_list->head->lexeme, "expand") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma
                    r.tail->flags |= TK_FLAG_FINAL;
                    skip_blanks_level(ctx, &r, input_list, level);

                    struct macro* macro = find_macro(ctx, input_list->head->lexeme);
                    if (macro)
                    {
                        macro->expand = true;
                    }

                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma
                }
                else if (input_list->head && strcmp(input_list->head->lexeme, "nullchecks") == 0)
                {
                    assert(false);
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//nullchecks
                    r.tail->flags |= TK_FLAG_FINAL;
                    skip_blanks_level(ctx, &r, input_list, level);
                    ctx->options.null_checks_enabled = true;
                }

                if (input_list->head && strcmp(input_list->head->lexeme, "diagnostic") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//diagnostic
                    r.tail->flags |= TK_FLAG_FINAL;
                    skip_blanks_level(ctx, &r, input_list, level);

                    if (input_list->head && strcmp(input_list->head->lexeme, "push") == 0)
                    {
                        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//diagnostic
                        r.tail->flags |= TK_FLAG_FINAL;
                        //#pragma GCC diagnostic push
                        if (ctx->options.diagnostic_stack_top_index <
                            sizeof(ctx->options.diagnostic_stack) / sizeof(ctx->options.diagnostic_stack[0]))
                        {
                            ctx->options.diagnostic_stack_top_index++;

                            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index] =
                                ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index - 1];
                        }
                    }
                    else if (input_list->head && strcmp(input_list->head->lexeme, "pop") == 0)
                    {
                        //#pragma GCC diagnostic pop
                        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pop
                        r.tail->flags |= TK_FLAG_FINAL;
                        if (ctx->options.diagnostic_stack_top_index > 0)
                        {
                            ctx->options.diagnostic_stack_top_index--;
                        }
                    }
                    else if (input_list->head && strcmp(input_list->head->lexeme, "warning") == 0)
                    {
                        //#pragma CAKE diagnostic warning "-Wenum-compare"

                        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//warning
                        r.tail->flags |= TK_FLAG_FINAL;
                        skip_blanks_level(ctx, &r, input_list, level);

                        if (input_list->head && input_list->head->type == TK_STRING_LITERAL)
                        {
                            match_token_level(&r, input_list, TK_STRING_LITERAL, level, ctx);//""
                            r.tail->flags |= TK_FLAG_FINAL;

                            unsigned long long  w = get_warning_bit_mask(input_list->head->lexeme + 1 + 2);
                            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings |= w;
                        }
                    }
                    else if (input_list->head && strcmp(input_list->head->lexeme, "ignore") == 0)
                    {
                        //#pragma CAKE diagnostic ignore "-Wenum-compare"

                        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//ignore
                        r.tail->flags |= TK_FLAG_FINAL;
                        skip_blanks_level(ctx, &r, input_list, level);

                        if (input_list->head && input_list->head->type == TK_STRING_LITERAL)
                        {
                            unsigned long long w = get_warning_bit_mask(input_list->head->lexeme + 1 + 2);
                            ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings &= ~w;
                        }
                    }
                }
            }

            struct token_list r7 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r7);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
            r.tail->type = TK_PRAGMA_END;
            r.tail->flags |= TK_FLAG_FINAL;
            token_list_destroy(&r7);
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
        assert(input_list->head->type == TK_IDENTIFIER); //nome da macro
        const struct token* const macro_name_token = input_list->head;

        match_token_level(&macro_argument_list.tokens, input_list, TK_IDENTIFIER, level, ctx); //NOME DA MACRO
        if (!macro->is_function)
        {
            //se nao eh funcao so faz isso e retorna o nome da macro
            return macro_argument_list;
        }

        struct macro_parameter* p_current_parameter = macro->parameters;
        int count = 1;
        skip_blanks(ctx, &macro_argument_list.tokens, input_list);
        match_token_level(&macro_argument_list.tokens, input_list, '(', level, ctx);
        skip_blanks(ctx, &macro_argument_list.tokens, input_list);
        if (input_list->head->type == ')')
        {
            if (macro->parameters != NULL)
            {
                struct macro_argument* owner p_argument = calloc(1, sizeof(struct macro_argument));
                p_argument->name = strdup(p_current_parameter->name);
                argument_list_add(&macro_argument_list, p_argument);
            }
            match_token_level(&macro_argument_list.tokens, input_list, ')', level, ctx);
            return macro_argument_list;
        }
        struct macro_argument* owner p_argument = calloc(1, sizeof(struct macro_argument));
        p_argument->name = strdup(p_current_parameter->name);
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

                    p_current_parameter = p_current_parameter->next;

                    if (p_current_parameter != NULL)
                    {
                        if (strcmp(p_current_parameter->name, "__VA_ARGS__") == 0)
                        {
                            //adicionamos este argumento como sendo vazio
                            p_argument = calloc(1, sizeof(struct macro_argument));
                            p_argument->name = strdup(p_current_parameter->name);
                            argument_list_add(&macro_argument_list, p_argument);
                            p_argument = NULL; //MOVED
                        }
                        else
                        {
                            preprocessor_diagnostic_message(C_ERROR_TOO_FEW_ARGUMENTS_TO_FUNCTION_LIKE_MACRO, ctx, macro_name_token, "too few arguments provided to function-like macro invocation\n");
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
                    p_current_parameter = p_current_parameter->next;
                    if (p_current_parameter == NULL)
                    {
                        preprocessor_diagnostic_message(C_ERROR_PREPROCESSOR_MACRO_INVALID_ARG, ctx, macro_name_token, "invalid args");
                        macro_argument_delete(p_argument);
                        p_argument = NULL; //DELETED
                        throw;
                    }
                    p_argument->name = strdup(p_current_parameter->name);
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

struct token_list expand_macro(struct preprocessor_ctx* ctx, struct macro_expanded* p_list, struct macro* macro, struct macro_argument_list* arguments, int level, const struct token* origin);
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
    //todo juntar tokens mesmo objet macro
    //struct token* p_previousNonBlank = 0;
    while (input_list->head)
    {
        //printf("r="); print_list(&r);
        //printf("input="); print_list(input_list);

        assert(!(input_list->head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
        if (input_list->head->type == '##')
        {
            if (r.tail == NULL)
            {
                preprocessor_diagnostic_message(C_ERROR_PREPROCESSOR_MISSING_MACRO_ARGUMENT, ctx, input_list->head, "missing macro argument (should be checked before)");
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
            struct token_list newlist = tokenizer(&tctx, ss.c_str, NULL, level, TK_FLAG_NONE);

            if (newlist.head)
            {
                newlist.head->flags = r.tail->flags;
            }
            else
            {
                struct token* owner p_new_token = calloc(1, sizeof * p_new_token);
                p_new_token->lexeme = strdup("");
                p_new_token->type = TK_PLACEMARKER;
                token_list_add(&newlist, p_new_token);
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

    struct macro_argument* p_va_args_argument =
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
        while (input_list->head)
        {
            assert(!(input_list->head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
            assert(!token_is_blank(input_list->head));
            assert(r.tail == NULL || !token_is_blank(r.tail));
            struct macro_argument* p_argument = NULL;
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
                        struct token* p_token = input_list->head;
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

                    const enum token_flags flags = input_list->head->flags;
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
                    char* owner s = token_list_join_tokens(&argumentlist, true);
                    if (s == NULL)
                    {
                        token_list_destroy(&argumentlist);
                        preprocessor_diagnostic_message(C_ERROR_UNEXPECTED, ctx, input_list->head, "unexpected");
                        throw;
                    }
                    struct token* owner p_new_token = calloc(1, sizeof * p_new_token);
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
                    int flags = input_list->head->flags;
                    //remove nome parametro do input
                    token_list_pop_front(input_list);
                    //coloca a expansao no resultado
                    struct token_list argumentlist = copy_argument_list(p_argument);
                    if (argumentlist.head)
                    {
                        //copia os flags do identificador
                        argumentlist.head->flags = flags;
                    }
                    /*depois reescan vai corrigir level*/
                    struct token_list r4 = replacement_list_reexamination(ctx, p_list, &argumentlist, 0, origin);
                    token_list_append_list(&r, &r4);
                    token_list_destroy(&r4);
                    if (ctx->n_errors > 0)
                    {
                        token_list_destroy(&argumentlist);
                        throw;
                    }
                    token_list_destroy(&argumentlist);
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

static bool macro_already_expanded(struct macro_expanded* p_list, const char* name)
{
    struct macro_expanded* p_item = p_list;
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
            assert(!(new_list.head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
            assert(!token_is_blank(new_list.head));
            struct macro* macro = NULL;
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
                        struct token* previous = r.tail->prev;
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
                aqui eh um bom lugar para setar o level e macro flags
                poq sempre tem a re scann da macro no fim
                */
                new_list.head->level = level;
                new_list.head->flags |= TK_FLAG_MACRO_EXPANDED;
                assert(!(new_list.head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
                prematch(&r, &new_list); //nao era macro
            }
        }
    }
    catch
    {
    }

    return r;
}

/*
  Faz a comparação ignorando a continuacao da linha
  TODO fazer uma revisão geral aonde se usa strcmp em lexeme
  e trocar por esta.
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

    return *(const unsigned char*) s1 - *(const unsigned char*) s2;
}

void remove_line_continuation(char* s)
{
    char* pread = s;
    char* pwrite = s;
    while (*pread)
    {
        if (pread[0] == '\\' && pread[1] == '\n')
        {
            pread++;
            pread++;
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

struct token_list  copy_replacement_list(struct token_list* list)
{
    //Faz uma copia dos tokens fazendo um trim no iniico e fim
    //qualquer espaco coments etcc vira um unico  espaco
    struct token_list r = { 0 };
    struct token* current = list->head;
    //sai de cima de todos brancos iniciais
    while (current && token_is_blank(current))
    {
        current = current->next;
    }
    //remover flag de espaco antes se tiver
    bool is_first = true;

    for (; current;)
    {
        if (current && token_is_blank(current))
        {
            if (current == list->tail)
                break;

            current = current->next;
            continue;
        }
        struct token* token_added = token_list_clone_and_add(&r, current);
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



struct token_list macro_copy_replacement_list(struct preprocessor_ctx* ctx, struct macro* macro, const struct token* origin)
{
    /*macros de conteudo dinamico*/
    if (strcmp(macro->name, "__LINE__") == 0)
    {
        struct tokenizer_ctx tctx = { 0 };
        char line[50] = { 0 };

        assert(origin != NULL);
        snprintf(line, sizeof line, "%d", origin->line);

        struct token_list r = tokenizer(&tctx, line, "", 0, TK_FLAG_NONE);
        token_list_pop_front(&r);
        r.head->flags = 0;
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
        r.head->flags = 0;
        return r;
    }
    else if (strcmp(macro->name, "__COUNTER__") == 0)
    {
        //TODO unit test
        char line[50] = { 0 };
        ctx->count_macro_value++;
        snprintf(line, sizeof line, "%d", ctx->count_macro_value);
        struct tokenizer_ctx tctx = { 0 };
        struct token_list r = tokenizer(&tctx, line, "", 0, TK_FLAG_NONE);
        token_list_pop_front(&r);
        r.head->flags = 0;
        return r;
    }

    return copy_replacement_list(&macro->replacement_list);
}

void print_literal2(const char* s);




struct token_list expand_macro(struct preprocessor_ctx* ctx,
    struct macro_expanded* list,
    struct macro* macro,
    struct macro_argument_list* arguments,
    int level,
    const struct token* origin)
{
    macro->usage++;

    struct token_list r = { 0 };
    try
    {
        assert(!macro_already_expanded(list, macro->name));
        struct macro_expanded macro_expanded = { 0 };
        macro_expanded.name = macro->name;
        macro_expanded.p_previous = list;
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
    }
    catch
    {
    }

    //printf("result=");
    //print_list(&r);
    return r;
}
void print_token(struct token* p_token);

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
            struct macro* macro = NULL;
            struct token* start_token = input_list->head;
            const struct token* origin = NULL;

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
                        struct token* previous = r.tail->prev;
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
#ifdef _WIN32
                if (input_list->head->token_origin)
                {
                    //char line[1000] = { 0 };
                    //snprintf(line, sizeof line, "%s(%d,%d):\n", input_list->head->token_origin->lexeme, input_list->head->line, input_list->head->col);
                    //OutputDebugStringA(line);
                }
#endif

                //efeito tetris
                //#define f(a) a
                //#define F g
                //F(1)
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

                if (macro->expand)
                {
                    //Esconde a macro e os argumentos
                    for (struct token* current = arguments.tokens.head;
                        current != arguments.tokens.tail->next;
                        current = current->next)
                    {
                        current->flags |= TK_C_BACKEND_FLAG_HIDE;
                    }

                    //mostra a expansao da macro
                    /*teste de expandir so algumas macros*/
                    for (struct token* current = start_macro.head;
                        current != start_macro.tail->next;
                        current = current->next)
                    {
                        current->flags &= ~(TK_FLAG_MACRO_EXPANDED | TK_FLAG_SLICED | TK_FLAG_LINE_CONTINUATION);
                    }
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
                //saiu tetris...
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
                        preprocessor_diagnostic_message(W_NOTE, ctx, input_list->head, "you can use \"adjacent\" \"strings\"");
                    }
                    else if (input_list->head->type == TK_LINE_COMMENT)
                        preprocessor_diagnostic_message(W_COMMENT, ctx, input_list->head, "multi-line //comment");
                    else
                        preprocessor_diagnostic_message(W_LINE_SLICING, ctx, input_list->head, "unnecessary line-slicing");
                }

                bool blanks = token_is_blank(input_list->head) || input_list->head->type == TK_NEWLINE;
                bool is_final = is_active && !is_never_final(input_list->head->type);

                if (ctx->flags & PREPROCESSOR_CTX_FLAGS_ONLY_FINAL)
                {
                    if (is_final)
                    {
                        prematch(&r, input_list);
                        r.tail->flags |= TK_FLAG_FINAL;
                        //token_promote(r.tail);
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
                                // if (strcmp(r.tail->lexeme, "_CRT_STDIO_INLINE") == 0)
                                 //{
                                   //  printf("");
                                 //}

                                r.tail->flags |= TK_FLAG_FINAL;
                                //token_promote(r.tail);

                            }
                        }
                        else
                        {
                            if (is_final)
                            {
                                //if (strcmp(r.tail->lexeme, "_CRT_STDIO_INLINE") == 0)
                                //{
                                 //   printf("");
                                //}

                                prematch(&r, input_list);
                                r.tail->flags |= TK_FLAG_FINAL;
                                //token_promote(r.tail);
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

    if (input_list->head->type == TK_PREPROCESSOR_LINE)
    {
        if (preprocessor_token_ahead_is_identifier(input_list->head, "if") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "ifdef") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "ifndef"))
        {
            return if_section(ctx, input_list, is_active, level);
        }
        else if (preprocessor_token_ahead_is_identifier(input_list->head, "include") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "embed") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "define") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "undef") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "warning") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "line") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "error") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "pragma"))
        {
            return control_line(ctx, input_list, is_active, level);
        }
        else
        {
            //aqui vou consumir o # dentro para ficar simetrico
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


static void mark_macros_as_used(struct owner_hash_map* map)
{
    /*
     *  Objetivo era alertar macros nao usadas...
     */

    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct owner_map_entry* pentry = map->table[i];

            while (pentry != NULL)
            {
                struct macro* macro = pentry->p;
                macro->usage = 1;
                pentry = pentry->next;
            }
        }
    }
}

void check_unused_macros(struct owner_hash_map* map)
{
    /*
     *  Objetivo era alertar macros nao usadas...
     */

    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct owner_map_entry* pentry = map->table[i];

            while (pentry != NULL)
            {
                struct macro* macro = pentry->p;
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

void include_config_header(struct preprocessor_ctx* ctx)
{
#define CAKE_CFG_FNAME "/cakeconfig.h"
    char executable_path[MAX_PATH - sizeof(CAKE_CFG_FNAME)] = { 0 };
    get_self_path(executable_path, sizeof(executable_path));
    dirname(executable_path);
    char path[MAX_PATH] = { 0 };
    snprintf(path, sizeof path, "%s" CAKE_CFG_FNAME, executable_path);
#undef CAKE_CFG_FNAME
    /*
    * windows
     echo %INCLUDE%
     Linux
     echo | gcc -E -Wp,-v -
    */

    char* owner str = read_file(path);


    const enum diagnostic_id w =
        ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings;



    struct tokenizer_ctx tctx = { 0 };
    struct token_list l = tokenizer(&tctx, str, "standard macros inclusion", 0, TK_FLAG_NONE);
    struct token_list l10 = preprocessor(ctx, &l, 0);
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    free(str);
    token_list_destroy(&l10);

    /*restore*/
    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index].warnings = w;
}
void add_standard_macros(struct preprocessor_ctx* ctx)
{
    /*
      This command prints all macros used by gcc
      echo | gcc -dM -E -
    */
    const struct diagnostic w =
        ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index];

    /*we dont want warnings here*/
    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index] =
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

    const char* pre_defined_macros_text =
        "#define __CAKE__ 202311L\n"
        "#define __STDC_VERSION__ 202311L\n"
        "#define __FILE__ \"__FILE__\"\n"
        "#define __LINE__ 0\n"
        "#define __COUNTER__ 0\n"
        "#define _CONSOLE\n"
        "#define __STDC_OWNERSHIP__ 1\n"
        "#define _W_DIVIZION_BY_ZERO_ 29\n"



#ifdef _WIN32

        //see
        //https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
        "#define _WIN32 " TOSTRING(_WIN32) "\n"


#ifdef _WIN64
        "#define _WIN64 " TOSTRING(_WIN64) "\n"
#endif

        "#define _INTEGRAL_MAX_BITS " TOSTRING(_INTEGRAL_MAX_BITS) "\n" /*Use of __int64 should be conditional on the predefined macro _INTEGRAL_MAX_BITS*/

        "#define _MSC_VER " TOSTRING(_MSC_VER) "\n"
        "#define _M_IX86 "  TOSTRING(_M_IX86) "\n"
        "#define __fastcall\n"
        "#define __stdcall\n"
        "#define __cdecl\n"
        "#define __pragma(a)\n"
        "#define __declspec(a)\n"
        "#define __builtin_offsetof(type, member) 0\n"
        "#define __ptr64\n"
        "#define __ptr32\n";

#endif

#if defined __linux__ || defined __APPLE__

    //https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
        /*some gcc stuff need to parse linux headers*/
#ifdef __linux__
    "#define __linux__\n"
#elif defined __APPLE__
    "#define __APPLE__\n"
#endif

        "#define __builtin_va_list\n"
        "#define __builtin_va_start(a, b)\n"
        "#define __builtin_va_end(a)\n"
        "#define __builtin_va_arg(a, b) ((b)a)\n"
        "#define __builtin_va_copy(a, b)\n"
        "#define __builtin_offsetof(type, member) 0\n"

        "#define __CHAR_BIT__ " TOSTRING(__CHAR_BIT__) "\n"
        "#define __SIZE_TYPE__ " TOSTRING(__SIZE_TYPE__) "\n"
        "#define __PTRDIFF_TYPE__ " TOSTRING(__PTRDIFF_TYPE__) "\n"
        "#define __WCHAR_TYPE__ " TOSTRING(__WCHAR_TYPE__) "\n"
        "#define __WINT_TYPE__ " TOSTRING(__WINT_TYPE__) "\n"
        "#define __INTMAX_TYPE__ " TOSTRING(__INTMAX_TYPE__) "\n"
        "#define __UINTMAX_TYPE__ " TOSTRING(__UINTMAX_TYPE__) "\n"
        "#define __SIG_ATOMIC_TYPE__ " TOSTRING(__SIG_ATOMIC_TYPE__) "\n"
        "#define __INT8_TYPE__ " TOSTRING(__INT8_TYPE__) "\n"
        "#define __INT16_TYPE__ " TOSTRING(__INT16_TYPE__) "\n"
        "#define __INT32_TYPE__ " TOSTRING(__INT32_TYPE__) "\n"
        "#define __INT64_TYPE__ " TOSTRING(__INT64_TYPE__) "\n"
        "#define __UINT8_TYPE__ " TOSTRING(__UINT8_TYPE__) "\n"
        "#define __UINT16_TYPE__ " TOSTRING(__UINT16_TYPE__) "\n"
        "#define __UINT32_TYPE__ " TOSTRING(__UINT32_TYPE__) "\n"
        "#define __UINT64_TYPE__ " TOSTRING(__UINT64_TYPE__) "\n"
        "#define __INT_LEAST8_TYPE__ " TOSTRING(__INT_LEAST8_TYPE__) "\n"
        "#define __INT_LEAST16_TYPE__ " TOSTRING(__INT_LEAST16_TYPE__) "\n"
        "#define __INT_LEAST32_TYPE__ " TOSTRING(__INT_LEAST32_TYPE__) "\n"
        "#define __INT_LEAST64_TYPE__ " TOSTRING(__INT_LEAST64_TYPE__) "\n"
        "#define __UINT_LEAST8_TYPE__ " TOSTRING(__UINT_LEAST8_TYPE__) "\n"
        "#define __UINT_LEAST16_TYPE__ " TOSTRING(__UINT_LEAST16_TYPE__) "\n"
        "#define __UINT_LEAST32_TYPE__ " TOSTRING(__UINT_LEAST32_TYPE__) "\n"
        "#define __UINT_LEAST64_TYPE__ " TOSTRING(__UINT_LEAST64_TYPE__) "\n"
        "#define __INT_FAST8_TYPE__ " TOSTRING(__INT_FAST8_TYPE__) "\n"
        "#define __INT_FAST16_TYPE__ " TOSTRING(__INT_FAST16_TYPE__) "\n"
        "#define __INT_FAST32_TYPE__ " TOSTRING(__INT_FAST32_TYPE__) "\n"
        "#define __INT_FAST64_TYPE__ " TOSTRING(__INT_FAST64_TYPE__) "\n"
        "#define __UINT_FAST8_TYPE__ " TOSTRING(__UINT_FAST8_TYPE__) "\n"
        "#define __UINT_FAST16_TYPE__ " TOSTRING(__UINT_FAST16_TYPE__) "\n"
        "#define __UINT_FAST32_TYPE__ " TOSTRING(__UINT_FAST32_TYPE__) "\n"
        "#define __UINT_FAST64_TYPE__ " TOSTRING(__UINT_FAST64_TYPE__) "\n"
        "#define __INTPTR_TYPE__ " TOSTRING(__INTPTR_TYPE__) "\n"
        "#define __UINTPTR_TYPE__ " TOSTRING(__UINTPTR_TYPE__) "\n"

        "#define __DBL_MAX__ " TOSTRING(__DBL_MAX__) "\n"
        "#define __DBL_MIN__ " TOSTRING(__DBL_MIN__) "\n"
        "#define __FLT_RADIX__ " TOSTRING(__FLT_RADIX__) "\n"
        "#define __FLT_EPSILON__ " TOSTRING(__FLT_EPSILON__) "\n"
        "#define __DBL_EPSILON__ " TOSTRING(__DBL_EPSILON__) "\n"
        "#define __LDBL_EPSILON__ " TOSTRING(__LDBL_EPSILON__) "\n"
        "#define __DBL_DECIMAL_DIG__ " TOSTRING(__DBL_DECIMAL_DIG__) "\n"
        "#define __FLT_EVAL_METHOD__ " TOSTRING(__FLT_EVAL_METHOD__) "\n"
        "#define __FLT_RADIX__ " TOSTRING(__FLT_RADIX__) "\n"


        "#define __SCHAR_MAX__ " TOSTRING(__SCHAR_MAX__) "\n"
        "#define __WCHAR_MAX__ " TOSTRING(__WCHAR_MAX__) "\n"
        "#define __SHRT_MAX__ " TOSTRING(__SHRT_MAX__) "\n"
        "#define __INT_MAX__ " TOSTRING(__INT_MAX__) "\n"
        "#define __LONG_MAX__ " TOSTRING(__LONG_MAX__) "\n"
        "#define __LONG_LONG_MAX__ " TOSTRING(__LONG_LONG_MAX__) "\n"
        "#define __WINT_MAX__ " TOSTRING(__WINT_MAX__) "\n"
        "#define __SIZE_MAX__ " TOSTRING(__SIZE_MAX__) "\n"
        "#define __PTRDIFF_MAX__ " TOSTRING(__PTRDIFF_MAX__) "\n"
        "#define __INTMAX_MAX__ " TOSTRING(__INTMAX_MAX__) "\n"
        "#define __UINTMAX_MAX__ " TOSTRING(__UINTMAX_MAX__) "\n"
        "#define __SIG_ATOMIC_MAX__ " TOSTRING(__SIG_ATOMIC_MAX__) "\n"
        "#define __INT8_MAX__ " TOSTRING(__INT8_MAX__) "\n"
        "#define __INT16_MAX__ " TOSTRING(__INT16_MAX__) "\n"
        "#define __INT32_MAX__ " TOSTRING(__INT32_MAX__) "\n"
        "#define __INT64_MAX__ " TOSTRING(__INT64_MAX__) "\n"
        "#define __UINT8_MAX__ " TOSTRING(__UINT8_MAX__) "\n"
        "#define __UINT16_MAX__ " TOSTRING(__UINT16_MAX__) "\n"
        "#define __UINT32_MAX__ " TOSTRING(__UINT32_MAX__) "\n"
        "#define __UINT64_MAX__ " TOSTRING(__UINT64_MAX__) "\n"
        "#define __INT_LEAST8_MAX__ " TOSTRING(__INT_LEAST8_MAX__) "\n"
        "#define __INT_LEAST16_MAX__ " TOSTRING(__INT_LEAST16_MAX__) "\n"
        "#define __INT_LEAST32_MAX__ " TOSTRING(__INT_LEAST32_MAX__) "\n"
        "#define __INT_LEAST64_MAX__ " TOSTRING(__INT_LEAST64_MAX__) "\n"
        "#define __UINT_LEAST8_MAX__ " TOSTRING(__UINT_LEAST8_MAX__) "\n"
        "#define __UINT_LEAST16_MAX__ " TOSTRING(__UINT_LEAST16_MAX__) "\n"
        "#define __UINT_LEAST32_MAX__ " TOSTRING(__UINT_LEAST32_MAX__) "\n"
        "#define __UINT_LEAST64_MAX__ " TOSTRING(__UINT_LEAST64_MAX__) "\n"
        "#define __INT_FAST8_MAX__ " TOSTRING(__INT_FAST8_MAX__) "\n"
        "#define __INT_FAST16_MAX__ " TOSTRING(__INT_FAST16_MAX__) "\n"
        "#define __INT_FAST32_MAX__ " TOSTRING(__INT_FAST32_MAX__) "\n"
        "#define __INT_FAST64_MAX__ " TOSTRING(__INT_FAST64_MAX__) "\n"
        "#define __UINT_FAST8_MAX__ " TOSTRING(__UINT_FAST8_MAX__) "\n"
        "#define __UINT_FAST16_MAX__ " TOSTRING(__UINT_FAST16_MAX__) "\n"
        "#define __UINT_FAST32_MAX__ " TOSTRING(__UINT_FAST32_MAX__) "\n"
        "#define __UINT_FAST64_MAX__ " TOSTRING(__UINT_FAST64_MAX__) "\n"
        "#define __INTPTR_MAX__ " TOSTRING(__INTPTR_MAX__) "\n"
        "#define __UINTPTR_MAX__ " TOSTRING(__UINTPTR_MAX__) "\n"
        "#define __WCHAR_MIN__ " TOSTRING(__WCHAR_MIN__) "\n"
        "#define __WINT_MIN__ " TOSTRING(__WINT_MIN__) "\n"
        "#define __SIG_ATOMIC_MIN__ " TOSTRING(__SIG_ATOMIC_MIN__) "\n"

        "#define __INT8_C " TOSTRING(__SIG_ATOMIC_MIN__) "\n"
        "#define __INT16_C " TOSTRING(__INT16_C) "\n"
        "#define __INT32_C " TOSTRING(__INT32_C) "\n"
        "#define __INT64_C " TOSTRING(__INT64_C) "\n"
        "#define __UINT8_C " TOSTRING(__UINT8_C) "\n"
        "#define __UINT16_C " TOSTRING(__UINT16_C) "\n"
        "#define __UINT32_C " TOSTRING(__UINT32_C) "\n"
        "#define __UINT64_C " TOSTRING(__UINT64_C) "\n"
        "#define __INTMAX_C " TOSTRING(__INTMAX_C) "\n"
        "#define __UINTMAX_C " TOSTRING(__UINTMAX_C) "\n"

        "#define __SCHAR_WIDTH__ " TOSTRING(__SCHAR_WIDTH__) "\n"
        "#define __SHRT_WIDTH__ " TOSTRING(__SHRT_WIDTH__) "\n"
        "#define __INT_WIDTH__ " TOSTRING(__INT_WIDTH__) "\n"
        "#define __LONG_WIDTH__ " TOSTRING(__LONG_WIDTH__) "\n"
        "#define __LONG_LONG_WIDTH__ " TOSTRING(__LONG_LONG_WIDTH__) "\n"
        "#define __PTRDIFF_WIDTH__ " TOSTRING(__PTRDIFF_WIDTH__) "\n"
        "#define __SIG_ATOMIC_WIDTH__ " TOSTRING(__SIG_ATOMIC_WIDTH__) "\n"
        "#define __SIZE_WIDTH__ " TOSTRING(__SIZE_WIDTH__) "\n"
        "#define __WCHAR_WIDTH__ " TOSTRING(__WCHAR_WIDTH__) "\n"
        "#define __WINT_WIDTH__ " TOSTRING(__WINT_WIDTH__) "\n"
        "#define __INT_LEAST8_WIDTH__ " TOSTRING(__INT_LEAST8_WIDTH__) "\n"
        "#define __INT_LEAST16_WIDTH__ " TOSTRING(__INT_LEAST16_WIDTH__) "\n"
        "#define __INT_LEAST32_WIDTH__ " TOSTRING(__INT_LEAST32_WIDTH__) "\n"
        "#define __INT_LEAST64_WIDTH__ " TOSTRING(__INT_LEAST64_WIDTH__) "\n"
        "#define __INT_FAST8_WIDTH__ " TOSTRING(__INT_FAST8_WIDTH__) "\n"
        "#define __INT_FAST16_WIDTH__ " TOSTRING(__INT_FAST16_WIDTH__) "\n"
        "#define __INT_FAST32_WIDTH__ " TOSTRING(__INT_FAST32_WIDTH__) "\n"
        "#define __INT_FAST64_WIDTH__ " TOSTRING(__INT_FAST64_WIDTH__) "\n"
        "#define __INTPTR_WIDTH__ " TOSTRING(__INTPTR_WIDTH__) "\n"
        "#define __INTMAX_WIDTH__ " TOSTRING(__INTMAX_WIDTH__) "\n"


        "#define __SIZEOF_INT__ " TOSTRING(__SIZEOF_INT__) "\n"
        "#define __SIZEOF_LONG__ " TOSTRING(__SIZEOF_LONG__) "\n"
        "#define __SIZEOF_LONG_LONG__ " TOSTRING(__SIZEOF_LONG_LONG__) "\n"
        "#define __SIZEOF_SHORT__ " TOSTRING(__SIZEOF_SHORT__) "\n"
        "#define __SIZEOF_POINTER__ " TOSTRING(__SIZEOF_POINTER__) "\n"
        "#define __SIZEOF_FLOAT__ " TOSTRING(__SIZEOF_FLOAT__) "\n"
        "#define __SIZEOF_DOUBLE__ " TOSTRING(__SIZEOF_DOUBLE__) "\n"
        "#define __SIZEOF_LONG_DOUBLE__ " TOSTRING(__SIZEOF_LONG_DOUBLE__) "\n"
        "#define __SIZEOF_SIZE_T__ " TOSTRING(__SIZEOF_SIZE_T__) "\n"
        "#define __SIZEOF_WCHAR_T__ " TOSTRING(__SIZEOF_WCHAR_T__) "\n"
        "#define __SIZEOF_WINT_T__ " TOSTRING(__SIZEOF_WINT_T__) "\n"
        "#define __SIZEOF_PTRDIFF_T__ " TOSTRING(__SIZEOF_PTRDIFF_T__) "\n"
#endif
        "\n";

    struct token_list l = tokenizer(&tctx, pre_defined_macros_text, "standard macros inclusion", 0, TK_FLAG_NONE);
    struct token_list l10 = preprocessor(ctx, &l, 0);

    //nao quer ver warning de nao usado nestas macros padrao
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    token_list_destroy(&l10);

    /*restore*/
    ctx->options.diagnostic_stack[ctx->options.diagnostic_stack_top_index] = w;
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
        case TK_KEYWORD_CATCH: return "TK_KEYWORD_CATCH"; /*extension*/
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
        case TK_KEYWORD__INT8: return "TK_KEYWORD__INT8";
        case TK_KEYWORD__INT16: return "TK_KEYWORD__INT16";
        case TK_KEYWORD__INT32: return "TK_KEYWORD__INT32";
        case TK_KEYWORD__INT64: return "TK_KEYWORD__INT64";


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
        case TK_KEYWORD_TRY: return "TK_KEYWORD_TRY"; /*extension*/
        case TK_KEYWORD_THROW: return "TK_KEYWORD_THROW"; /*extension*/
        case TK_KEYWORD_UNION: return "TK_KEYWORD_UNION";
        case TK_KEYWORD_UNSIGNED: return "TK_KEYWORD_UNSIGNED";
        case TK_KEYWORD_VOID: return "TK_KEYWORD_VOID";
        case TK_KEYWORD_VOLATILE: return "TK_KEYWORD_VOLATILE";
        case TK_KEYWORD_WHILE: return "TK_KEYWORD_WHILE";

        case TK_KEYWORD__ALIGNAS: return "TK_KEYWORD__ALIGNAS";
        case TK_KEYWORD__ALIGNOF: return "TK_KEYWORD__ALIGNOF";
        case TK_KEYWORD__ATOMIC: return "TK_KEYWORD__ATOMIC";
            //microsoft
            //KEYWORD__FASTCALL,
            //KEYWORD__STDCALL
            //
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
        case TK_KEYWORD__OWNER: return "TK_KEYWORD__OWNER";
        case TK_KEYWORD__OUT: return "TK_KEYWORD__OUT";
        case TK_KEYWORD__OBJ_OWNER: return "TK_KEYWORD__OBJ_OWNER";
        case TK_KEYWORD__VIEW: return "TK_KEYWORD__VIEW";
        case TK_KEYWORD__OPT: return "TK_KEYWORD__OPT";


            /*extension compile time functions*/
        case TK_KEYWORD_STATIC_DEBUG: return "TK_KEYWORD_STATIC_DEBUG"; /*extension*/
        case TK_KEYWORD_STATIC_DEBUG_EX: return "TK_KEYWORD_STATIC_DEBUG_EX"; /*extension*/
        case TK_KEYWORD_STATIC_STATE: return "TK_KEYWORD_STATIC_STATE"; /*extension*/
        case TK_KEYWORD_STATIC_SET: return "TK_KEYWORD_STATIC_SET"; /*extension*/
        case TK_KEYWORD_ATTR_ADD: return "TK_KEYWORD_ATTR_ADD"; /*extension*/
        case TK_KEYWORD_ATTR_REMOVE: return "TK_KEYWORD_ATTR_REMOVE"; /*extension*/
        case TK_KEYWORD_ATTR_HAS: return "TK_KEYWORD_ATTR_HAS"; /*extension*/

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

        default:
            return "TK_X_MISSING_NAME";
    }
    return "";
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


void print_literal(const char* s)
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





const char* owner get_code_as_we_see_plus_macros(struct token_list* list)
{
    struct osstream ss = { 0 };
    struct token* current = list->head;
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

    const char* owner cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}

/*useful to debug visit.c*/
void print_code_as_we_see(struct token_list* list, bool remove_comments)
{

    struct token* current = list->head;
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
                //se uma macro expandida for mostrada ele nao tem espacos entao inserimos
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
const char* owner get_code_as_we_see(struct token_list* list, bool remove_comments)
{
    struct osstream ss = { 0 };
    struct token* current = list->head;
    while (current != list->tail->next)
    {
        if (current->level == 0 &&
            !(current->flags & TK_FLAG_MACRO_EXPANDED) &&
            !(current->flags & TK_C_BACKEND_FLAG_HIDE) &&
            current->type != TK_BEGIN_OF_FILE)
        {
            if ((current->flags & TK_FLAG_HAS_SPACE_BEFORE) &&
                (current->prev != NULL && current->prev->type != TK_BLANKS))
            {
                //se uma macro expandida for mostrada ele nao tem espacos entao inserimos
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

    const char* owner cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}


const char* owner get_code_as_compiler_see(struct token_list* list)
{
    struct osstream ss = { 0 };


    struct token* current = list->head;
    while (current != list->tail->next)
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

const char* owner print_preprocessed_to_string2(struct token* opt p_token)
{
    /*
    * No nivel > 0 (ou seja dentro dos includes)
    * Esta funcao imprime os tokens como o compilador ve
    * e insere um espaco ou quebra de linha para poder representar
    * a separacao entre os tokens.

    * Ja no nivel 0 (arquivo principal) ele imprime espacos comentarios
    * etc.. e insere espacos na expancao da macro.
    */

    if (p_token == NULL)
        return strdup("(null)");

    struct osstream ss = { 0 };
    struct token* current = p_token;
    while (current)
    {

        //Nós ignorados a line continuation e ela pode aparecer em qualquer parte
        //dos lexemes.
        //inves de remover poderia so pular ao imprimir
        remove_line_continuation(current->lexeme);

        if (current->flags & TK_FLAG_FINAL)
        {
            if (current->level > 0)
            {
                //nos niveis de include nos podemos estar ignorando todos
                //os espacos. neste caso eh preciso incluilos para nao juntar os tokens

                if ((current->flags & TK_FLAG_HAS_NEWLINE_BEFORE))
                    ss_fprintf(&ss, "\n");
                else if ((current->flags & TK_FLAG_HAS_SPACE_BEFORE))
                    ss_fprintf(&ss, " ");
            }
            else
            {
                /*
                  no nivel 0 nos imprimimos os espacos.. porem no caso das macros
                  eh preciso colocar um espaco pq ele nao existe.
                */
                if (current->flags & TK_FLAG_MACRO_EXPANDED)
                {
                    if ((current->flags & TK_FLAG_HAS_SPACE_BEFORE))
                        ss_fprintf(&ss, " ");
                }
            }

            //}

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

const char* owner print_preprocessed_to_string(struct token* p_token)
{
    /*
    * Esta funcao imprime os tokens como o compilador ve
    * e insere um espaco ou quebra de linha para poder representar
    * a separacao entre os tokens.
    */

    struct osstream ss = { 0 };
    struct token* current = p_token;

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

void print_preprocessed(struct token* p_token)
{
    const char* owner s = print_preprocessed_to_string(p_token);
    if (s)
    {
        printf("%s", s);
        free((void* owner)s);
    }
}

static bool is_screaming_case(const char* text)
{
    if (text == NULL)
        return true;

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

void print_all_macros(struct preprocessor_ctx* prectx)
{
    for (int i = 0; i < prectx->macros.capacity; i++)
    {
        struct owner_map_entry* entry = prectx->macros.table[i];
        if (entry == NULL) continue;
        struct macro* macro = entry->p;
        printf("#define %s", macro->name);
        if (macro->is_function)
        {
            printf("(");

            struct macro_parameter* parameter = macro->parameters;
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

        struct token* token = macro->replacement_list.head;
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
        preprocessor_diagnostic_message(W_NOTE, ctx, token, "use SCREAMING_CASE for macros");
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
            free((void* owner)s);
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
    free((void* owner)s);
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



    return res;
}

int test_preprocessor_in_out_using_file(const char* fileName)
{
    int res = 0;
    const char* input = normalize_line_end(read_file(fileName));
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
        free((void* owner)input);
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
    add_standard_macros(&prectx);
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

#endif
