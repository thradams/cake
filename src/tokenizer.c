//#include <corecrt.h>
/*
   "string com codigo" se transforma em uma lista ligada de tokens

   ┌───┐   ┌───┐   ┌───┐   ┌───┐
   │   ├──►│   ├──►│   ├──►│   │──► NULL
   └───┘   └───┘   └───┘   └───┘

   ao passar no preprocessaor esta lista é expandida com includes e macros


   ┌───┐                  ┌───┐   ┌───┐   ┌───┐
   │   ├──────┐           │   ├──►│x  ├──►│x  │ ──► NULL
   └───┘      │           └───┘   └───┘   └───┘
            ┌─▼─┐   ┌───┐   ▲
            │   ├───┤   ├───┘
            └───┘   └───┘

  cada item tem um int level que indica o nivel de incluldes
  tambem bmacroexapanded que indica se token foi gerado de expando
  de macro e bfinal se este token eh final o que o parser realmente  ve

  a parte que da dentro do include, para efeito de parser pode ignorar
  todos os espacos. Temo um modo que so coloca os nos finais dentro
  do nivel 1 2. 3..
  ou colocar tudo sempre. #define INCLUDE_ALL 1
  a vantagem de include tudo eh pode colocar uma mensagem de erro
  de um header por ex copiando a linha. a desvantagem eh mais memoria

*/


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


//declaração da macro container_of
#ifndef container_of
#define container_of(ptr , type , member) (type *)( (char *) ptr - offsetof(type , member) )
#endif

/*
 Se for 1 inclui todos os ignorados de dentro dos includes
 se for 0 ele faz so resumido e desctart oq nao eh usado.
*/
#define INCLUDE_ALL 1

///////////////////////////////////////////////////////////////////////////////
void naming_convention_macro(struct preprocessor_ctx* ctx, struct token* token);
///////////////////////////////////////////////////////////////////////////////

void preprocessor_ctx_destroy(struct preprocessor_ctx* p)
{
    hashmap_destroy(&p->macros);
}

struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level, struct error* error);


void pre_seterror_with_token(struct preprocessor_ctx* ctx, struct token* p_token, const char* fmt, ...)
{
    ctx->n_errors++;
    //er->code = 1;

    if (p_token)
    {
        ctx->printf(WHITE "%s:%d:%d: ",
            p_token->token_origin->lexeme,
            p_token->line,
            p_token->col);
    }
    else
    {
        ctx->printf(WHITE "<>");
    }

    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    ctx->printf(LIGHTRED "error: " WHITE "%s\n", buffer);


    struct token* prev = p_token;
    while (prev && prev->prev && prev->prev->type != TK_NEWLINE)
    {
        prev = prev->prev;
    }
    struct token* next = prev;
    while (next && next->type != TK_NEWLINE)
    {
        if (next->flags & TK_FLAG_MACRO_EXPANDED)
        {
            /*
            tokens expandidos da macro nao tem espaco entre
            vamos adicionar para ver melhor
            */
            if (next->flags & TK_FLAG_HAS_SPACE_BEFORE)
            {
                ctx->printf(" ");
            }
        }
        ctx->printf("%s", next->lexeme);
        next = next->next;
    }
    ctx->printf("\n");
    if (p_token)
    {
        for (int i = 1; i < p_token->col - 1; i++)
        {
            ctx->printf(" ");
        }
        ctx->printf(LIGHTGREEN "^\n" RESET);
    }
}



void pre_setinfo_with_token(struct preprocessor_ctx* ctx, struct token* p_token, const char* fmt, ...)
{
    if (p_token)
    {
        ctx->printf(WHITE "%s:%d:%d: ",
            p_token->token_origin->lexeme,
            p_token->line,
            p_token->col);
    }
    else
    {
        ctx->printf(WHITE "<>");
    }

    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    ctx->printf(LIGHTCYAN "note: " WHITE "%s\n", buffer);

    struct token* prev = p_token;
    while (prev && prev->prev && prev->prev->type != TK_NEWLINE)
    {
        prev = prev->prev;
    }
    struct token* next = prev;
    while (next && next->type != TK_NEWLINE)
    {
        if (next->flags & TK_FLAG_MACRO_EXPANDED)
        {
            /*
            tokens expandidos da macro nao tem espaco entre
            vamos adicionar para ver melhor
            */
            if (next->flags & TK_FLAG_HAS_SPACE_BEFORE)
            {
                ctx->printf(" ");
            }
        }
        ctx->printf("%s", next->lexeme);
        next = next->next;
    }
    ctx->printf("\n");
    if (p_token)
    {
        for (int i = 1; i < p_token->col - 1; i++)
        {
            ctx->printf(" ");
        }
        ctx->printf(LIGHTGREEN "^\n" RESET);
    }
}

void pre_error_warning_with_token(struct preprocessor_ctx* ctx, struct token* p_token, bool is_error)
{
    ctx->n_warnings++;
    //er->code = 1;

    if (p_token)
    {
        ctx->printf(WHITE "%s:%d:%d: ",
            p_token->token_origin->lexeme,
            p_token->line,
            p_token->col);
    }
    else
    {
        ctx->printf(WHITE "<>");
    }

    if (is_error)
        ctx->printf(LIGHTRED "error: " WHITE);
    else
        ctx->printf(LIGHTMAGENTA "warning: " WHITE);


    struct token* prev = p_token;
    while (prev && prev->prev && prev->prev->type != TK_NEWLINE)
    {
        prev = prev->prev;
    }
    struct token* next = prev;
    while (next && next->type != TK_NEWLINE)
    {
        ctx->printf("%s", next->lexeme);
        next = next->next;
    }
    ctx->printf("\n");

}


struct include_dir* include_dir_add(struct include_dir_list* list, const char* path)
{
    struct include_dir* p_new_include_dir = calloc(1, sizeof * p_new_include_dir);
    p_new_include_dir->path = strdup(path);
    if (list->head == NULL)
    {
        list->head = p_new_include_dir;
        list->tail = p_new_include_dir;
    }
    else
    {
        list->tail->next = p_new_include_dir;
        list->tail = p_new_include_dir;
    }
    return p_new_include_dir;
}


const char* find_and_read_include_file(struct preprocessor_ctx* ctx, const char* path, char fullpath[300], bool* p_already_included)
{
    snprintf(fullpath, 300, "%s", path);

    if (hashmap_find(&ctx->pragma_once_map, fullpath) != NULL)
    {
        *p_already_included = true;
        return NULL;
    }


    char* content = readfile(fullpath);
    if (content == NULL)
    {
        struct include_dir* current = ctx->include_dir.head;
        while (current)
        {
            snprintf(fullpath, 300, "%s%s", current->path, path);

            if (hashmap_find(&ctx->pragma_once_map, fullpath) != NULL)
            {
                *p_already_included = true;
                return NULL;
            }

            content = readfile(fullpath);
            if (content != NULL)
                break;
            current = current->next;
        }
    }
    return content;
}


struct macro_parameter
{
    const char* name;
    struct macro_parameter* next;
};


struct macro
{
    struct type_tag_id type_id;
    const char* name;
    struct token_list replacement_list; /*copia*/
    struct macro_parameter* parameters;
    bool is_function;
    int usage;

    bool expand;
};

/*usado para verificar recursividade*/
struct macro_expanded
{
    const char* name;
    struct macro_expanded* pPrevious;
};

void add_macro(struct preprocessor_ctx* ctx, const char* name)
{
    struct macro* macro = calloc(1, sizeof * macro);
    if (macro == NULL)
    {
    }
    macro->name = strdup(name);
    hashmap_set(&ctx->macros, name, &macro->type_id);
}


struct macro_argument
{
    const char* name;
    struct token_list tokens;
    struct macro_argument* next;
};


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
    bool previous_is_blank = false;
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
        previous_is_blank = false;

        if (current == list->tail)
            break;
        current = current->next;
    }

    return r;
}


struct token_list copy_argument_list(struct macro_argument* pMacroArgument)
{    
    struct token_list list = copy_argument_list_tokens(&pMacroArgument->tokens);
    if (list.head == NULL)
    {
        /*nunca eh vazio..se for ele colocar um TK_PLACEMARKER*/
        struct token* p_new_token = calloc(1, sizeof * p_new_token);
        p_new_token->lexeme = strdup("");
        p_new_token->type = TK_PLACEMARKER;
        token_list_add(&list, p_new_token);
    }
    //print_list(&list);
    return list;
}


struct macro_argument_list
{
    struct token_list tokens;
    struct macro_argument* head;
    struct macro_argument* tail;
};

void print_macro_arguments(struct macro_argument_list* arguments)
{
    struct macro_argument* pArgument = arguments->head;
    while (pArgument)
    {
        printf("%s:", pArgument->name);
        print_list(&pArgument->tokens);
        pArgument = pArgument->next;
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


void argument_list_add(struct macro_argument_list* list, struct macro_argument* pnew)
{
    assert(pnew->next == NULL);
    if (list->head == NULL)
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        list->tail->next = pnew;
        list->tail = pnew;
    }
}


void print_macro(struct macro* macro)
{
    printf("%s", macro->name);
    if (macro->is_function)
        printf("(");
    struct macro_parameter* pParameter = macro->parameters;
    while (pParameter)
    {
        if (macro->parameters != pParameter)
            printf(",");
        printf("%s", pParameter->name);
        pParameter = pParameter->next;
    }
    if (macro->is_function)
        printf(") ");
    print_list(&macro->replacement_list);
}

void delete_macro(struct macro* macro)
{
    if (macro)
    {
        free(macro);
    }
}

struct macro* find_macro(struct preprocessor_ctx* ctx, const char* name)
{
    struct type_tag_id* pNode = hashmap_find(&ctx->macros, name);
    if (pNode == NULL)
        return NULL;
    struct macro* macro = container_of(pNode, struct macro, type_id);
    return macro;
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
    /*
    2. Each instance of a backslash character (\) immediately followed by a new-line character is
    deleted, splicing physical source lines to form logical source lines. Only the last backslash on
    any physical source line shall be eligible for being part of such a splice. A source file that is
    not empty shall end in a new-line character, which shall not be immediately preceded by a
    backslash character before any such splicing takes place.
    */
    stream->current++;
    if (stream->current[0] == '\n')
    {
        stream->line++;
        stream->col = 1;
    }
    else
    {
        stream->col++;
    }
    if (stream->current[0] == '\\' && stream->current[1] == '\n')
    {
        stream->current++;
        stream->current++;
        stream->line++;
        stream->col = 1;
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
    //TODO peprformance range?

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
    case '\\':
        type = '\\';
        stream_match(stream);
        break;
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


struct token* new_token(const char* lexeme_head, const char* lexeme_tail, enum token_type type)
{
    struct token* p_new_token = calloc(1, sizeof * p_new_token);
    size_t sz = lexeme_tail - lexeme_head;
    p_new_token->lexeme = calloc(sz + 1, sizeof(char));
    p_new_token->type = type;
    strncpy(p_new_token->lexeme, lexeme_head, sz);
    return p_new_token;
}

struct token* identifier(struct stream* stream)
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

    struct token* p_new_token = new_token(start, stream->current, TK_IDENTIFIER);


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

struct token* character_constant(struct stream* stream)
{
    const char* start = stream->current;

    /*encoding_prefix_opt*/
    if (stream->current[0] == 'u')
    {
        stream_match(stream);
        if (stream->current[1] == '8')
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

        if (stream->current[0] == '\0')
        {
            // ' not found..error or not char constant            
            //TODO 
            //#warning don't
            // here ' is not character_constant 
            break;
        }
    }
    stream_match(stream);

    if (stream->current - start > 6)
    {
        //warning: character constant too long for its type
    }
    struct token* p_new_token = new_token(start, stream->current, TK_CHAR_CONSTANT);

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

struct token* string_literal(struct stream* stream, struct error* error)
{
    struct token* p_new_token = NULL;

    const char* start = stream->current;
    int start_line = stream->line;
    int start_col = stream->col;

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
                seterror(error, "%s(%d:%d) missing terminating \" character",
                    stream->source,
                    start_line,
                    start_col);
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

struct token* ppnumber(struct stream* stream)
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
    struct token* p_new_token = new_token(start, stream->current, TK_PPNUMBER);
    return p_new_token;
}

struct token_list embed_tokenizer(const char* filename_opt, int level, enum token_flags addflags, struct error* error)
{
    struct token_list list = { 0 };

    FILE* file = NULL;

    bool bFirst = true;
    int line = 1;
    int col = 1;
    int count = 0;
    try
    {
#ifndef MOCKFILES
        file = fopen(filename_opt, "rb");
        if (file == NULL)
        {
            seterror(error, "file '%s' not found", filename_opt);
            throw;
        }
#else
        /*web versions only text files that are included*/
        const char* textfile = readfile(filename_opt);
        if (textfile == NULL)
        {
            seterror(error, "file '%s' not found", filename_opt);
            throw;
        }

        const char* pch = textfile;
#endif

        unsigned char ch;
#ifndef MOCKFILES
        while (fread(&ch, 1, 1, file))
        {
#else
        while (*pch)
        {
            ch = *pch;
            pch++;
#endif                    
            if (bFirst)
            {
                bFirst = false;
            }
            else
            {
                char b[] = ",";
                struct token* p_new_token = new_token(b, &b[1], TK_COMMA);
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
                    struct token* pNew3 = new_token(newline, &newline[1], TK_NEWLINE);
                    pNew3->level = level;
                    pNew3->token_origin = NULL;
                    pNew3->line = line;
                    pNew3->col = col;
                    token_list_add(&list, pNew3);
                }
            }

            char buffer[30];
            int c = snprintf(buffer, sizeof buffer, "%d", (int)ch);

            struct token* p_new_token = new_token(buffer, &buffer[c], TK_PPNUMBER);
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
    struct token* p_new_token = new_token(newline, &newline[1], TK_NEWLINE);
    p_new_token->level = level;
    p_new_token->token_origin = NULL;
    p_new_token->line = line;
    p_new_token->col = col;
    token_list_add(&list, p_new_token);

    if (file) fclose(file);



    assert(list.head != NULL);
    return list;
}

struct token_list tokenizer(const char* text, const char* filename_opt, int level, enum token_flags addflags, struct error* error)
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
        .current = text
    };

    try
    {
        struct token* pFirst = NULL;
        if (filename_opt != NULL)
        {
            const char* bof = "";
            pFirst = new_token(bof, bof + 1, TK_BEGIN_OF_FILE);
            pFirst->level = level;
            pFirst->lexeme = strdup(filename_opt);
            token_list_add(&list, pFirst);
        }


        //struct token* current = pFirst;
        bool bNewLine = true;
        bool bHasSpace = false;
        while (1)
        {
            const int line = stream.line;
            const int col = stream.col;

            if (stream.current[0] == '\0')
            {
                stream_match(&stream);
                break;
            }
            if (is_digit(&stream) ||
                (stream.current[0] == '.' && isdigit(stream.current[0])))
            {
                struct token* p_new_token = ppnumber(&stream);
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                bNewLine = false;
                bHasSpace = false;
                continue;
            }

            /*
             Tem que vir antes identifier
            */
            if (first_of_string_literal(&stream))
            {
                struct token* p_new_token = string_literal(&stream, error);
                if (p_new_token == NULL)
                    throw;
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);;
                bNewLine = false;
                bHasSpace = false;
                continue;
            }

            if (first_of_character_constant(&stream))
            {
                //TODO if we have ' in the middle then it is not character constant
                struct token* p_new_token = character_constant(&stream);
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                bNewLine = false;
                bHasSpace = false;
                continue;
            }

            if (is_nondigit(&stream))
            {
                struct token* p_new_token = identifier(&stream);
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                bNewLine = false;
                bHasSpace = false;
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
                struct token* p_new_token = new_token(start, stream.current, TK_BLANKS);
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                /*bNewLine = false;*/ //deixa assim
                bHasSpace = true;
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
                }
                struct token* p_new_token = new_token(start, stream.current, TK_LINE_COMMENT);
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                bNewLine = true;
                bHasSpace = false;
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
                        seterror(error, "missing end of comment");
                        break;
                    }
                    else
                    {
                        stream_match(&stream);
                    }
                }
                struct token* p_new_token = new_token(start, stream.current, TK_COMENT);
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                bNewLine = false;
                bHasSpace = false;
                continue;
            }
            if (bNewLine && stream.current[0] == '#')
            {
                const char* start = stream.current;
                stream_match(&stream);
                struct token* p_new_token = new_token(start, stream.current, '#');
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                p_new_token->type = TK_PREPROCESSOR_LINE;
                token_list_add(&list, p_new_token);
                bNewLine = false;
                bHasSpace = false;
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
                struct token* p_new_token = new_token(newline, newline + 1, TK_NEWLINE);
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                bNewLine = true;
                bHasSpace = false;
                continue;
            }
            const char* start = stream.current;
            enum token_type t = is_punctuator(&stream);
            if (t != TK_NONE)
            {

                struct token* p_new_token = new_token(start, stream.current, t);
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                bNewLine = false;
                bHasSpace = false;
                continue;
            }
            else
            {
                stream_match(&stream);
                struct token* p_new_token = new_token(start, stream.current, ANY_OTHER_PP_TOKEN);
                p_new_token->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = pFirst;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                bNewLine = false;
                bHasSpace = false;

                //            stream_print_line(&stream);
                            //printf("%s (%d, %d) invalid token ? '%c' %d\n",
                              //     filename,
                                //   line,
                                  // col,
                                   //stream.current[0],
                                   //(int)stream.current[0]);

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
struct token_list group_part(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, struct error* error);
struct token_list group_opt(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, struct error* error)
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
                struct token_list r2 = group_part(ctx, input_list, is_active, level, error);
                token_list_append_list(&r, &r2);
                if (error->code) throw;
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
    return p->type != TK_COMENT &&
        p->type != TK_BLANKS &&
        p->type != TK_LINE_COMMENT &&
        p->type != TK_NEWLINE;
}

bool is_never_final(enum token_type type)
{
    return type == TK_BEGIN_OF_FILE ||
        type == TK_BLANKS ||
        type == TK_LINE_COMMENT ||
        type == TK_COMENT ||
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
            current->type == TK_COMENT))
    {
        current = current->next;
    }
    return current;
}

bool preprocessor_token_ahead_is(struct token* p, enum token_type t)
{
    struct token* pA = preprocessor_look_ahead_core(p);
    if (pA != NULL && pA->type == t)
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
            current->type == TK_COMENT))
    {
        current = current->prev;
    }
    return (current && current->type == t);
}

bool preprocessor_token_ahead_is_identifier(struct token* p, const char* lexeme)
{
    assert(p != NULL);
    struct token* pA = preprocessor_look_ahead_core(p);
    if (pA != NULL && pA->type == TK_IDENTIFIER)
    {
        return strcmp(pA->lexeme, lexeme) == 0;
    }
    return false;
}

void skip_blanks_level(struct token_list* dest, struct token_list* input_list, int level)
{
    while (input_list->head &&
        token_is_blank(input_list->head))
    {
        if (INCLUDE_ALL || level == 0)
            token_list_add(dest, token_list_pop_front(input_list));
        else
            token_list_pop_front(input_list); //deletar
    }
}

void skip_blanks(struct token_list* dest, struct token_list* input_list)
{
    while (token_is_blank(input_list->head))
    {
        token_list_add(dest, token_list_pop_front(input_list));
    }
}

void prematch_level(struct token_list* dest, struct token_list* input_list, int level)
{
    if (INCLUDE_ALL || level == 0)
        token_list_add(dest, token_list_pop_front(input_list));
    else
        token_list_pop_front(input_list);
}

void prematch(struct token_list* dest, struct token_list* input_list)
{
    token_list_add(dest, token_list_pop_front(input_list));
}
struct token_list pp_tokens_opt(struct preprocessor_ctx* ctx, struct token_list* input_list, int level);

struct token_list process_defined(struct preprocessor_ctx* ctx, struct token_list* input_list, struct error* error)
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
                skip_blanks(&r, input_list);

                bool bHasParentesis = false;
                if (input_list->head->type == '(')
                {
                    token_list_pop_front(input_list);
                    bHasParentesis = true;
                }

                skip_blanks(&r, input_list);



                struct macro* macro = find_macro(ctx, input_list->head->lexeme);
                struct token* p_new_token = token_list_pop_front(input_list);
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

                if (bHasParentesis)
                {
                    if (input_list->head->type != ')')
                    {
                        pre_seterror_with_token(ctx, input_list->head, "missing )");
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
                skip_blanks(&r, input_list);
                token_list_pop_front(input_list); //pop (
                skip_blanks(&r, input_list);


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



                bool bAlreadyIncluded = false;
                const char* s = find_and_read_include_file(ctx, path, fullpath, &bAlreadyIncluded);

                bool bHasInclude = s != NULL;
                free((void*)s);

                struct token* p_new_token = calloc(1, sizeof * p_new_token);
                p_new_token->type = TK_PPNUMBER;
                free(p_new_token->lexeme);
                p_new_token->lexeme = strdup(bHasInclude ? "1" : "0");
                p_new_token->flags |= TK_FLAG_FINAL;

                token_list_add(&r, p_new_token);
                token_list_pop_front(input_list); //pop )
            }
            else if (input_list->head->type == TK_IDENTIFIER &&
                strcmp(input_list->head->lexeme, "__has_c_attribute") == 0)
            {
                token_list_pop_front(input_list); //pop __has_include
                skip_blanks(&r, input_list);
                token_list_pop_front(input_list); //pop (
                skip_blanks(&r, input_list);


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
                bool bHas_C_Attribute = false;

                struct token* p_new_token = calloc(1, sizeof * p_new_token);
                p_new_token->type = TK_PPNUMBER;
                free(p_new_token->lexeme);
                p_new_token->lexeme = strdup(bHas_C_Attribute ? "1" : "0");
                p_new_token->flags |= TK_FLAG_FINAL;

                token_list_add(&r, p_new_token);
                token_list_pop_front(input_list); //pop )
            }
            else
            {
                token_list_add(&r, token_list_pop_front(input_list));
            }
        }
    }
    catch
    {
    }

    return r;
}

struct token_list process_identifiers(struct preprocessor_ctx* ctx, struct token_list* list)
{
    assert(!token_list_is_empty(list));

    struct token_list list2 = { 0 };


    while (list->head != NULL)
    {
        if (list->head->type == TK_IDENTIFIER)
        {

            struct macro* macro = find_macro(ctx, list->head->lexeme);
            struct token* p_new_token = token_list_pop_front(list);
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
            token_list_add(&list2, token_list_pop_front(list));
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
        token_list_add(&r, token_list_pop_front(input_list));
    }
    return r;
}

//todo passar lista para reotnro
long long preprocessor_constant_expression(struct preprocessor_ctx* ctx,
    struct token_list* output_list,
    struct token_list* input_list,
    int level,
    struct error* error)
{
    ctx->conditional_inclusion = true;
    struct token_list r = { 0 };
    while (input_list->head && input_list->head->type != TK_NEWLINE)
    {
        token_list_add(&r, token_list_pop_front(input_list));
    }
    *output_list = r;


    struct token_list list1 = copy_replacement_list(&r);
    //printf("\n");
    //print_list(&list1);
    //printf("\n");



    int flags = ctx->flags;
    ctx->flags |= PREPROCESSOR_CTX_FLAGS_ONLY_FINAL;

    /*defined X  por exemplo é mantido sem ser expandido*/

    struct token_list list2 = preprocessor(ctx, &list1, 1, error);
    ctx->flags = flags;
    //printf("apos preprocess\n");
    //print_list(&list2);
    //printf("\n");

    /*aonde defined has_c_aatribute sao transformados em constantes*/
    struct token_list list3 = process_defined(ctx, &list2, error);

    //printf("apos remove defined\n");
    //print_list(&list3);
    //printf("\n");

    struct token_list list4 = process_identifiers(ctx, &list3);

    //printf("apos remover identificadores restantes\n");
    //print_list(&list4);
    //printf("\n");

    assert(list4.head != NULL);

    struct preprocessor_ctx pre_ctx = { 0 };
    //struct parser_ctx parser_ctx = { 0 };
    pre_ctx.input_list = list4;
    pre_ctx.current = list4.head;
    //pre_skip_blanks(&parser_ctx);

    long long value = 0;
    if (pre_constant_expression(&pre_ctx, error, &value) != 0)
    {
        assert(false);
        //TODO error
    }

    ctx->conditional_inclusion = false;
    return value;
}

void match_level(struct token_list* dest, struct token_list* input_list, int level)
{
    if (INCLUDE_ALL || level == 0)
        token_list_add(dest, token_list_pop_front(input_list));
    else
        token_list_pop_front(input_list); //deletar
}


int match_token_level(struct token_list* dest, struct token_list* input_list, enum token_type type, int level,
    struct preprocessor_ctx* ctx, struct error* error)
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
                    pre_seterror_with_token(ctx, input_list->head, "expected token %s got %s\n", get_token_name(type), get_token_name(input_list->head->type));
                else
                    pre_seterror_with_token(ctx, dest->tail, "expected EOF \n");

                throw;
            }
        }
        if (input_list->head != NULL)
        {
            if (INCLUDE_ALL || level == 0)
                token_list_add(dest, token_list_pop_front(input_list));
            else
                token_list_pop_front(input_list); //deletar
        }
    }
    catch
    {
    }
    return error->code;
}


struct token_list if_group(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, bool* pbIfResult, struct error* error)
{
    *pbIfResult = 0; //out

    struct token_list r = { 0 };
    try
    {
        /*
         if-group:
           # if constant-expression new-line group_opt
           # ifdef identifier new-line group_opt
           # ifndef identifier new-line group_opt
        */
        match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx, error);
        skip_blanks_level(&r, input_list, level);
        assert(input_list->head->type == TK_IDENTIFIER);
        if (strcmp(input_list->head->lexeme, "ifdef") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error); //ifdef
            skip_blanks_level(&r, input_list, level);
            if (is_active)
            {
                struct macro* macro = find_macro(ctx, input_list->head->lexeme);
                *pbIfResult = (macro != NULL) ? 1 : 0;
                //printf("#ifdef %s (%s)\n", input_list->head->lexeme, *pbIfResult ? "true" : "false");
            }
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);
            skip_blanks_level(&r, input_list, level);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
        }
        else if (strcmp(input_list->head->lexeme, "ifndef") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error); //ifndef
            skip_blanks_level(&r, input_list, level);
            if (is_active)
            {
                struct macro* macro = find_macro(ctx, input_list->head->lexeme);
                *pbIfResult = (macro == NULL) ? 1 : 0;
            }
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);
            skip_blanks_level(&r, input_list, level);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
        }
        else if (strcmp(input_list->head->lexeme, "if") == 0)
        {
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error); //if
            skip_blanks_level(&r, input_list, level);
            if (is_active)
            {
                struct token_list r0 = { 0 };
                *pbIfResult = preprocessor_constant_expression(ctx, &r0, input_list, level, error);
                token_list_append_list(&r, &r0);
            }
            else
            {
                struct token_list r0 = ignore_preprocessor_line(input_list);
                token_list_append_list(&r, &r0);
            }
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
        }
        else
        {

            pre_seterror_with_token(ctx, input_list->head, "unexpected");
            throw;
        }
        struct token_list r2 = group_opt(ctx, input_list, is_active && *pbIfResult, level, error);
        token_list_append_list(&r, &r2);
    }
    catch
    {
    }

    return r;
}

struct token_list elif_group(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, bool* pElifResult, struct error* error)
{
    *pElifResult = 0; //out

    struct token_list r = { 0 };
    /*
     elif-group:
      # elif constant-expression new-line group_opt

      C23
      # elifdef identifier new-line group_opt
      # elifndef identifier new-line group_opt
    */
    match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx, error);
    skip_blanks(&r, input_list);
    int result = 0;
    if (strcmp(input_list->head->lexeme, "elif") == 0)
    {
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);
        skip_blanks(&r, input_list);

        if (is_active)
        {
            struct token_list r0 = { 0 };
            result = preprocessor_constant_expression(ctx, &r0, input_list, level, error);

            token_list_append_list(&r, &r0);


        }
        else
        {
            ignore_preprocessor_line(input_list);
        }
    }
    else if (strcmp(input_list->head->lexeme, "elifdef") == 0)
    {
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);
        skip_blanks(&r, input_list);

        if (is_active)
        {
            result = (hashmap_find(&ctx->macros, input_list->head->lexeme) != NULL) ? 1 : 0;
        }
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);
    }
    else if (strcmp(input_list->head->lexeme, "elifndef") == 0)
    {
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);
        skip_blanks(&r, input_list);

        if (is_active)
        {
            result = (hashmap_find(&ctx->macros, input_list->head->lexeme) == NULL) ? 1 : 0;
        }
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);
    }
    *pElifResult = (result != 0);
    skip_blanks(&r, input_list);
    match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
    struct token_list r2 = group_opt(ctx, input_list, is_active && *pElifResult, level, error);
    token_list_append_list(&r, &r2);
    return r;
}

struct token_list elif_groups(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, bool* pbelifResult, struct error* error)
{
    struct token_list r = { 0 };
    /*
    elif-groups:
      elif-group
      elif-groups elif-group
    */
    bool bAlreadyFoundElifTrue = false;
    bool bElifResult = false;
    struct token_list r2 = elif_group(ctx, input_list, is_active, level, &bElifResult, error);
    token_list_append_list(&r, &r2);
    if (bElifResult)
        bAlreadyFoundElifTrue = true;
    if (input_list->head->type == TK_PREPROCESSOR_LINE &&
        preprocessor_token_ahead_is_identifier(input_list->head, "elif") ||
        preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") ||
        preprocessor_token_ahead_is_identifier(input_list->head, "elifndef"))
    {
        /*
          Depois que acha 1 true bAlreadyFoundElifTrue os outros sao false.
        */
        struct token_list r3 = elif_groups(ctx, input_list, is_active && !bAlreadyFoundElifTrue, level, &bElifResult, error);
        token_list_append_list(&r, &r3);
        if (bElifResult)
            bAlreadyFoundElifTrue = true;
    }
    /*
       Se algum dos elifs foi true retorna true
    */
    *pbelifResult = bAlreadyFoundElifTrue;
    return r;
}

struct token_list else_group(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, struct error* error)
{
    /*
      else-group:
       # else new-line group_opt
    */

    struct token_list r = { 0 };
    match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx, error);
    skip_blanks_level(&r, input_list, level);

    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error); //else
    skip_blanks_level(&r, input_list, level);
    match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);

    struct token_list r2 = group_opt(ctx, input_list, is_active, level, error);
    token_list_append_list(&r, &r2);

    return r;
}

struct token_list endif_line(struct preprocessor_ctx* ctx, struct token_list* input_list, int level, struct error* error)
{
    /*
     endif-line:
       # endif new-line
    */

    struct token_list r = { 0 };

    match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx, error); //#
    skip_blanks_level(&r, input_list, level);
    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error); //endif
    skip_blanks_level(&r, input_list, level);
    match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);

    return r;
}

struct token_list if_section(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, struct error* error)
{
    /*
     if-section:
       if-group elif-groups_opt else-group_opt endif-line
    */

    struct token_list r = { 0 };

    try
    {
        bool bIfResult = false;
        struct token_list r2 = if_group(ctx, input_list, is_active, level, &bIfResult, error);
        if (error->code) throw;

        token_list_append_list(&r, &r2);
        bool bElifResult = false;
        if (input_list->head->type == TK_PREPROCESSOR_LINE &&
            preprocessor_token_ahead_is_identifier(input_list->head, "elif") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "elifndef"))
        {
            struct token_list r3 = elif_groups(ctx, input_list, is_active && !bIfResult, level, &bElifResult, error);
            token_list_append_list(&r, &r3);
        }
        if (input_list->head->type == TK_PREPROCESSOR_LINE &&
            preprocessor_token_ahead_is_identifier(input_list->head, "else"))
        {
            struct token_list r4 = else_group(ctx, input_list, is_active && !bIfResult && !bElifResult, level, error);
            token_list_append_list(&r, &r4);
        }

        if (error->code) throw;

        struct token_list r5 = endif_line(ctx, input_list, level, error);
        token_list_append_list(&r, &r5);
    }
    catch
    {
    }

    return r;
}

struct token_list identifier_list(struct preprocessor_ctx* ctx, struct macro* macro, struct token_list* input_list, int level, struct error* error)
{
    struct token_list r = { 0 };
    /*
      identifier-list:
      identifier
      identifier-list , identifier
    */
    skip_blanks(&r, input_list);
    struct macro_parameter* pMacroParameter = calloc(1, sizeof * pMacroParameter);
    pMacroParameter->name = strdup(input_list->head->lexeme);
    macro->parameters = pMacroParameter;
    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);
    skip_blanks(&r, input_list);
    while (input_list->head->type == ',')
    {
        match_token_level(&r, input_list, ',', level, ctx, error);
        skip_blanks(&r, input_list);
        if (input_list->head->type == '...')
        {
            break;
        }
        pMacroParameter->next = calloc(1, sizeof * pMacroParameter);
        pMacroParameter = pMacroParameter->next;
        pMacroParameter->name = strdup(input_list->head->lexeme);
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);
        skip_blanks(&r, input_list);
    }
    return r;
}


struct token_list replacement_list(struct macro* macro, struct token_list* input_list, int level)
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
    return r;
}

struct token_list pp_tokens_opt(struct preprocessor_ctx* ctx, struct token_list* input_list, int level)
{
    struct token_list r = { 0 };
    while (input_list->head->type != TK_NEWLINE)
    {
        prematch_level(&r, input_list, level);
    }
    return r;
}

struct token_list control_line(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, struct error* error)
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
            //se nao esta ativo eh ingorado
            struct token_list r7 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r7);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
            return r;
        }

#ifdef _WIN32
        //char line[1000] = { 0 };
        //snprintf(line, sizeof line, "%s(%d,%d):\n", input_list->head->token_origin->lexeme, input_list->head->line, input_list->head->col);
        //OutputDebugStringA(line);
#endif
        struct token* const ptoken = input_list->head;
        match_token_level(&r, input_list, TK_PREPROCESSOR_LINE, level, ctx, error);
        skip_blanks_level(&r, input_list, level);
        if (strcmp(input_list->head->lexeme, "include") == 0)
        {
            /*
              # include pp-tokens new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error); //include
            skip_blanks_level(&r, input_list, level);
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
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);

            char fullpath[300] = { 0 };


            path[strlen(path) - 1] = '\0';

            bool bAlreadyIncluded = false;
            const char* content = find_and_read_include_file(ctx, path + 1, fullpath, &bAlreadyIncluded);
            if (content != NULL)
            {
                struct token_list list = tokenizer(content, fullpath, level + 1, TK_FLAG_NONE, error);
                free((void*)content);

                struct token_list list2 = preprocessor(ctx, &list, level + 1, error);
                token_list_append_list(&r, &list2);
            }
            else
            {
                if (!bAlreadyIncluded)
                {
                    pre_seterror_with_token(ctx, r.tail, "file %s not found", path + 1);

                    for (struct include_dir* p = ctx->include_dir.head; p; p = p->next)
                    {
                        /*let's print the include path*/
                        ctx->printf("%s\n", p->path);
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
            match_token_level(p_list, input_list, TK_IDENTIFIER, level, ctx, error); //embed
            skip_blanks_level(p_list, input_list, level);
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
            match_token_level(p_list, input_list, TK_NEWLINE, level, ctx, error);



            char fullpath[300] = { 0 };
            path[strlen(path) - 1] = '\0';

            snprintf(fullpath, sizeof(fullpath), "%s", path + 1);
            struct error localerror = { 0 };

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

            struct token_list list = embed_tokenizer(fullpath, nlevel, f, &localerror);
            if (localerror.code != 0)
            {
                pre_seterror_with_token(ctx, input_list->head, "embed error: %s", localerror.message);
            }

            token_list_append_list(&r, &list);
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

            struct macro* macro = calloc(1, sizeof * macro);
            if (macro == NULL)
            {
                seterror(error, "out of memory");
                throw;
            }

            /*
                # define identifier                           replacement-list new-line
                # define identifier ( identifier-list_opt )    replacement-list new-line
                # define identifier ( ... )                   replacement-list new-line
                # define identifier ( identifier-list , ... ) replacement-list new-line
            */
            //p = preprocessor_match_identifier(p, is_active, level, false, "define");

            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error); //define
            skip_blanks_level(&r, input_list, level);

            // printf("define %s\n%s : %d\n", input_list->head->lexeme, input_list->head->token_origin->lexeme, input_list->head->line);

            struct token* macro_name_token = input_list->head;


            if (hashmap_find(&ctx->macros, input_list->head->lexeme) != NULL)
            {
                //printf("warning: '%s' macro redefined at %s %d\n",
                  //     input_list->head->lexeme,
                    ///   input_list->head->token_origin->lexeme,
                      // input_list->head->line);
            }


            hashmap_set(&ctx->macros, input_list->head->lexeme, &macro->type_id);
            macro->name = strdup(input_list->head->lexeme);


            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error); //nome da macro
            /*sem skip*/
            //p = preprocessor_match_token(p, is_active, level, false, IDENTIFIER); /*name*/
            if (input_list->head->type == '(')
            {

                macro->is_function = true;


                match_token_level(&r, input_list, '(', level, ctx, error); //nome da macro
                skip_blanks_level(&r, input_list, level);
                if (input_list->head->type == '...')
                {
                    struct macro_parameter* pMacroParameter = calloc(1, sizeof * pMacroParameter);
                    pMacroParameter->name = strdup("__VA_ARGS__");
                    macro->parameters = pMacroParameter;

                    // assert(false);
                    match_token_level(&r, input_list, '...', level, ctx, error); //nome da macro
                    skip_blanks_level(&r, input_list, level);
                    match_token_level(&r, input_list, ')', level, ctx, error); //nome da macro
                }
                else if (input_list->head->type == ')')
                {
                    match_token_level(&r, input_list, ')', level, ctx, error);
                    skip_blanks_level(&r, input_list, level);
                }
                else
                {
                    struct token_list r3 = identifier_list(ctx, macro, input_list, level, error);
                    token_list_append_list(&r, &r3);
                    skip_blanks_level(&r, input_list, level);
                    if (input_list->head->type == '...')
                    {
                        struct macro_parameter* pMacroParameter = calloc(1, sizeof * pMacroParameter);
                        pMacroParameter->name = strdup("__VA_ARGS__");
                        struct macro_parameter* pLast = macro->parameters;
                        assert(pLast != NULL);
                        while (pLast->next)
                        {
                            pLast = pLast->next;
                        }
                        pLast->next = pMacroParameter;


                        match_token_level(&r, input_list, '...', level, ctx, error);
                    }
                    skip_blanks_level(&r, input_list, level);
                    match_token_level(&r, input_list, ')', level, ctx, error);
                }
            }
            else
            {
                macro->is_function = false;
            }
            struct token_list r4 = replacement_list(macro, input_list, level);
            token_list_append_list(&r, &r4);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);

            if (macro_name_token)
                naming_convention_macro(ctx, macro_name_token);
        }
        else if (strcmp(input_list->head->lexeme, "undef") == 0)
        {
            /*
             # undef identifier new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//undef
            skip_blanks_level(&r, input_list, level);
            struct type_tag_id* pNode = hashmap_remove(&ctx->macros, input_list->head->lexeme);
            assert(find_macro(ctx, input_list->head->lexeme) == NULL);
            if (pNode)
            {
                struct macro* macro = container_of(pNode, struct macro, type_id);
                delete_macro(macro);
                match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//undef
            }
            else
            {
                match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//undef
                /*no warnings*/
            }
            skip_blanks_level(&r, input_list, level);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
        }
        else if (strcmp(input_list->head->lexeme, "line") == 0)
        {
            /*
               # line pp-tokens new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//line
            struct token_list r5 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r5);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
        }
        else if (strcmp(input_list->head->lexeme, "error") == 0)
        {
            /*
              # error pp-tokensopt new-line
            */
            ctx->n_warnings++;
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//error
            struct token_list r6 = pp_tokens_opt(ctx, input_list, level);
            pre_error_warning_with_token(ctx, input_list->head, true);
            token_list_append_list(&r, &r6);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);


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
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//warning

            struct token_list r6 = pp_tokens_opt(ctx, input_list, level);
            pre_error_warning_with_token(ctx, input_list->head, false);
            token_list_append_list(&r, &r6);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
        }
        else if (strcmp(input_list->head->lexeme, "pragma") == 0)
        {
            /*
              # pragma pp-tokensopt new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//pragma
            skip_blanks_level(&r, input_list, level);

            if (input_list->head->type == TK_IDENTIFIER)
            {
                if (strcmp(input_list->head->lexeme, "once") == 0)
                {
                    hashmap_set(&ctx->pragma_once_map, input_list->head->token_origin->lexeme, (void*)1);
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//pragma
                }
                else if (strcmp(input_list->head->lexeme, "expand") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//pragma
                    skip_blanks_level(&r, input_list, level);

                    struct macro* macro = find_macro(ctx, input_list->head->lexeme);
                    if (macro)
                    {
                        macro->expand = true;
                    }

                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//pragma

                }
            }

            struct token_list r7 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r7);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
        }
    }
    catch
    {
    }

    return r;
}


struct token_list non_directive(struct preprocessor_ctx* ctx, struct token_list* input_list, int level, struct error* error)
{
    /*
      non-directive:
      pp-tokens new-line
     */
    struct token_list r = pp_tokens_opt(ctx, input_list, level);
    skip_blanks_level(&r, input_list, level);
    match_token_level(&r, input_list, TK_NEWLINE, level, ctx, error);
    return r;
}

struct macro_argument_list collect_macro_arguments(struct preprocessor_ctx* ctx,
    struct macro* macro,
    struct token_list* input_list, int level, struct error* error)
{
    struct macro_argument_list macroArgumentList = { 0 };
    try
    {
        assert(input_list->head->type == TK_IDENTIFIER); //nome da macro

        match_token_level(&macroArgumentList.tokens, input_list, TK_IDENTIFIER, level, ctx, error); //NOME DA MACRO
        if (!macro->is_function)
        {
            //se nao eh funcao so faz isso e retorna o nome da macro
            return macroArgumentList;
        }

        struct macro_parameter* pCurrentParameter = macro->parameters;
        int count = 1;
        skip_blanks(&macroArgumentList.tokens, input_list);
        match_token_level(&macroArgumentList.tokens, input_list, '(', level, ctx, error);
        skip_blanks(&macroArgumentList.tokens, input_list);
        if (input_list->head->type == ')')
        {
            if (macro->parameters != NULL)
            {
                struct macro_argument* pArgument = calloc(1, sizeof(struct macro_argument));
                pArgument->name = strdup(pCurrentParameter->name);
                argument_list_add(&macroArgumentList, pArgument);
            }
            match_token_level(&macroArgumentList.tokens, input_list, ')', level, ctx, error);
            return macroArgumentList;
        }
        struct macro_argument* pCurrentArgument = calloc(1, sizeof(struct macro_argument));
        pCurrentArgument->name = strdup(pCurrentParameter->name);
        while (input_list->head != NULL)
        {
            if (input_list->head->type == '(')
            {
                count++;
                token_list_clone_and_add(&pCurrentArgument->tokens, input_list->head);
                match_token_level(&macroArgumentList.tokens, input_list, '(', level, ctx, error);
            }
            else if (input_list->head->type == ')')
            {
                count--;
                if (count == 0)
                {
                    match_token_level(&macroArgumentList.tokens, input_list, ')', level, ctx, error);
                    argument_list_add(&macroArgumentList, pCurrentArgument);
                    pCurrentParameter = pCurrentParameter->next;

                    if (pCurrentParameter != NULL)
                    {
                        if (strcmp(pCurrentParameter->name, "__VA_ARGS__") == 0)
                        {
                            //adicionamos este argumento como sendo vazio
                            pCurrentArgument = calloc(1, sizeof(struct macro_argument));
                            pCurrentArgument->name = strdup(pCurrentParameter->name);
                            argument_list_add(&macroArgumentList, pCurrentArgument);
                        }
                        else
                        {
                            //tODO
                            pre_seterror_with_token(ctx, input_list->head, "too few arguments provided to function-like macro invocation\n");
                            error->code = 1;
                            throw;
                        }
                    }


                    break;
                }
                else
                {
                    token_list_clone_and_add(&pCurrentArgument->tokens, input_list->head);
                    match_token_level(&macroArgumentList.tokens, input_list, ')', level, ctx, error);
                }
            }
            else if (count == 1 && input_list->head->type == ',')
            {
                if (strcmp(pCurrentParameter->name, "__VA_ARGS__") == 0)
                {
                    token_list_clone_and_add(&pCurrentArgument->tokens, input_list->head);
                    match_token_level(&macroArgumentList.tokens, input_list, ',', level, ctx, error);
                }
                else //if (count == 1)
                {
                    match_token_level(&macroArgumentList.tokens, input_list, ',', level, ctx, error);
                    argument_list_add(&macroArgumentList, pCurrentArgument);
                    pCurrentArgument = NULL; /*tem mais?*/
                    pCurrentArgument = calloc(1, sizeof(struct macro_argument));
                    pCurrentParameter = pCurrentParameter->next;
                    if (pCurrentParameter == NULL)
                    {
                        pre_seterror_with_token(ctx, input_list->head, "invalid args");
                        throw;
                    }
                    pCurrentArgument->name = strdup(pCurrentParameter->name);
                }



            }
            else
            {
                token_list_clone_and_add(&pCurrentArgument->tokens, input_list->head);
                prematch_level(&macroArgumentList.tokens, input_list, level);
                //token_list_add(&list, token_list_pop_front(input_list));
            }
        }
    }
    catch
    {
    }

    return macroArgumentList;
}

struct token_list expand_macro(struct preprocessor_ctx* ctx, struct macro_expanded* pList, struct macro* macro, struct macro_argument_list* arguments, int level, struct error* error);
struct token_list replacement_list_reexamination(struct preprocessor_ctx* ctx, struct macro_expanded* pList, struct token_list* oldlist, int level, struct error* error);


struct token_list macro_copy_replacement_list(struct preprocessor_ctx* ctx, struct macro* macro, struct error* error);

/*#define hash_hash # ## #
#define mkstr(a) # a
#define in_between(a) mkstr(a)
#define join(c, d) in_between(c hash_hash d)

hash_hash

join(x, y)
*/
struct token_list concatenate(struct preprocessor_ctx* ctx, struct token_list* input_list, struct error* error)
{
    //printf("input="); print_list(input_list);

    struct token_list  r = { 0 };
    //todo juntar tokens mesmo objet macro
    //struct token* pPreviousNonBlank = 0;
    while (input_list->head)
    {
        //printf("r="); print_list(&r);
        //printf("input="); print_list(input_list);

        assert(!(input_list->head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
        if (input_list->head->type == '##')
        {
            if (r.tail == NULL)
            {
                pre_seterror_with_token(ctx, input_list->head, "missing macro argument (should be checked before)");
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
            struct token_list newlist = tokenizer(ss.c_str, NULL, level, TK_FLAG_NONE, error);


            if (newlist.head)
            {
                //flags ficam sendo o mesmo do anterior
                newlist.head->flags = r.tail->flags;
            }

            /*
            * Arranca o anterior do r que foi usado para formar string
            */
            token_list_pop_back(&r);

            /*adiciona novo token no fim do r*/
            token_list_append_list(&r, &newlist);

            ss_close(&ss);

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

struct token_list replace_vaopt(struct preprocessor_ctx* ctx, struct token_list* input_list, bool bvaargs_was_empty)
{
    /*
    4  If the pp-token sequence that is attributed to the variable arguments is
    the empty pp-token sequence, after argument substitution for the following
    rescan of the replacement list (see 6.10.3.4), the identifier __VA_OPT__
    behaves as if defined as:
    */
    struct token_list r = { 0 };
    try
    {
        while (input_list->head)
        {
            if (input_list->head->type == TK_IDENTIFIER &&
                strcmp(input_list->head->lexeme, "__VA_OPT__") == 0)
            {
                //int flags = input_list->head->flags;
                token_list_pop_front(input_list);
                token_list_pop_front(input_list);

                if (bvaargs_was_empty)
                {
                    //remove tudo
                    int count = 1;
                    for (; input_list->head;)
                    {
                        if (input_list->head->type == '(')
                        {
                            token_list_pop_front(input_list);
                            count++;
                        }
                        else if (input_list->head->type == ')')
                        {
                            count--;
                            token_list_pop_front(input_list);
                            if (count == 0)
                                break;
                        }
                        else
                            token_list_pop_front(input_list);
                    }
                }
                else
                {
                    int count = 1;
                    for (; input_list->head;)
                    {
                        if (input_list->head->type == '(')
                        {
                            prematch(&r, input_list);
                            count++;
                        }
                        else if (input_list->head->type == ')')
                        {
                            count--;

                            if (count == 0)
                            {
                                token_list_pop_front(input_list);
                                break;
                            }
                            prematch(&r, input_list);
                        }
                        else
                            prematch(&r, input_list);
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
struct token_list replace_macro_arguments(struct preprocessor_ctx* ctx, struct macro_expanded* pList, struct token_list* input_list, struct macro_argument_list* arguments, struct error* error)
{
    struct token_list r = { 0 };
    bool bVarArgsWasEmpty = false;
    bool is_var_args = false;
    try
    {
        while (input_list->head)
        {
            assert(!(input_list->head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
            assert(!token_is_blank(input_list->head));
            assert(r.tail == NULL || !token_is_blank(r.tail));
            struct macro_argument* pArgument = NULL;
            if (input_list->head->type == TK_IDENTIFIER)
            {
                pArgument = find_macro_argument_by_name(arguments, input_list->head->lexeme);
            }
            if (pArgument)
            {
                bool check = false;
                if (strcmp(input_list->head->lexeme, "__VA_ARGS__") == 0)
                {
                    check = true;
                }

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
                    struct token_list argumentlist = copy_argument_list(pArgument);
                    if (check)
                    {
                        is_var_args = true;
                        bVarArgsWasEmpty = (argumentlist.head == NULL || argumentlist.head->type == TK_PLACEMARKER);
                    }

                    char* s = token_list_join_tokens(&argumentlist, true);
                    if (s == NULL)
                    {
                        pre_seterror_with_token(ctx, input_list->head, "unexpected");
                        throw;
                    }
                    struct token* p_new_token = calloc(1, sizeof * p_new_token);
                    p_new_token->lexeme = s;
                    p_new_token->type = TK_STRING_LITERAL;
                    p_new_token->flags = flags;
                    token_list_add(&r, p_new_token);
                    continue;
                }
                else if (r.tail != NULL && r.tail->type == '##')
                {
                    //estou parametro e anterior era ##
                    token_list_pop_front(input_list);
                    struct token_list argumentlist = copy_argument_list(pArgument);
                    if (check)
                    {
                        is_var_args = true;
                        bVarArgsWasEmpty = (argumentlist.head == NULL || argumentlist.head->type == TK_PLACEMARKER);
                    }
                    token_list_append_list(&r, &argumentlist);
                }
                else if (input_list->head->next && input_list->head->next->type == '##')
                {
                    //estou no parametro e o da frente eh ##
                    int flags = input_list->head->flags;
                    //tira nome parametro a lista
                    token_list_pop_front(input_list);
                    //passa tudo p resultado
                    struct token_list argumentlist = copy_argument_list(pArgument);
                    if (argumentlist.head != NULL)
                    {
                        argumentlist.head->flags = flags;
                    }
                    if (check)
                    {
                        is_var_args = true;
                        bVarArgsWasEmpty = (argumentlist.head == NULL || argumentlist.head->type == TK_PLACEMARKER);
                    }

                    token_list_append_list(&r, &argumentlist);
                    // ja passa o ## tambem
                    prematch(&r, input_list);
                }
                else
                {

                    int flags = input_list->head->flags;
                    //remove nome parametro do input
                    token_list_pop_front(input_list);
                    //coloca a expansao no resultado
                    struct token_list argumentlist = copy_argument_list(pArgument);
                    if (argumentlist.head)
                    {
                        //copia os flags do identificador
                        argumentlist.head->flags = flags;
                    }
                    /*depois reescan vai corrigir level*/
                    struct token_list r4 = replacement_list_reexamination(ctx, pList, &argumentlist, 0, error/*por enquanto*/);
                    if (error->code) throw;

                    if (check)
                    {
                        is_var_args = true;
                        bVarArgsWasEmpty = (r4.head == NULL || r4.head->type == TK_PLACEMARKER);
                    }
                    token_list_append_list(&r, &r4);
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

    if (is_var_args)
    {
        struct token_list r2 = replace_vaopt(ctx, &r, bVarArgsWasEmpty);
        return r2;
    }
    return r;
}

struct token_list concatenate(struct preprocessor_ctx* ctx, struct token_list* input_list, struct error* error);

bool macro_already_expanded(struct macro_expanded* pList, const char* name)
{
    struct macro_expanded* pItem = pList;
    while (pItem)
    {
        if (strcmp(name, pItem->name) == 0)
        {
            return true;
        }
        pItem = pItem->pPrevious;
    }
    return false;
}

struct token_list replacement_list_reexamination(struct preprocessor_ctx* ctx, struct macro_expanded* pList, struct token_list* oldlist, int level, struct error* error)
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
        struct token_list newList = concatenate(ctx, oldlist, error);
        while (newList.head != NULL)
        {
            assert(!(newList.head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
            assert(!token_is_blank(newList.head));
            struct macro* macro = NULL;
            if (newList.head->type == TK_IDENTIFIER)
            {
                macro = find_macro(ctx, newList.head->lexeme);
                if (macro &&
                    macro->is_function &&
                    !preprocessor_token_ahead_is(newList.head, '('))
                {
                    macro = NULL;
                }

                if (macro && macro_already_expanded(pList, newList.head->lexeme))
                {
                    newList.head->type = TK_IDENTIFIER_RECURSIVE_MACRO;
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
                int flags = newList.head->flags;
                struct macro_argument_list arguments = collect_macro_arguments(ctx, macro, &newList, level, error);
                if (error->code) throw;


                struct token_list r3 = expand_macro(ctx, pList, macro, &arguments, level, error);
                if (error->code) throw;

                if (r3.head)
                {
                    r3.head->flags = flags;
                }
                token_list_append_list_at_beginning(&newList, &r3);
            }
            else
            {
                /*
                aqui eh um bom lugar para setar o level e macro flags
                poq sempre tem a re scann da macro no fim
                */
                newList.head->level = level;
                newList.head->flags |= TK_FLAG_MACRO_EXPANDED;
                assert(!(newList.head->flags & TK_FLAG_HAS_NEWLINE_BEFORE));
                prematch(&r, &newList); //nao era macro
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

    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
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
    bool bIsFirst = true;
    bool previous_is_blank = false;
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
        if (bIsFirst)
        {
            token_added->flags = token_added->flags & ~TK_FLAG_HAS_SPACE_BEFORE;
            token_added->flags = token_added->flags & ~TK_FLAG_HAS_NEWLINE_BEFORE;
            bIsFirst = false;
        }
        remove_line_continuation(token_added->lexeme);
        previous_is_blank = false;

        if (current == list->tail)
            break;
        current = current->next;

    }
    return r;
}



struct token_list macro_copy_replacement_list(struct preprocessor_ctx* ctx, struct macro* macro, struct error* error)
{
    /*macros de conteudo dinamico*/
    if (strcmp(macro->name, "__LINE__") == 0)
    {
        struct token_list r = tokenizer("1", "", 0, TK_FLAG_NONE, error);
        token_list_pop_front(&r);
        r.head->flags = 0;
        return r;
    }
    else if (strcmp(macro->name, "__FILE__") == 0)
    {
        struct token_list r = tokenizer("\"file\"", "", 0, TK_FLAG_NONE, error);
        token_list_pop_front(&r);
        r.head->flags = 0;
        return r;
    }
    else if (strcmp(macro->name, "__COUNT__") == 0)
    {
        assert(false);//TODO
        struct token_list r = tokenizer("1", "", 0, TK_FLAG_NONE, error);
        token_list_pop_front(&r);
        r.head->flags = 0;
        return r;
    }

    return copy_replacement_list(&macro->replacement_list);
}

void print_literal2(const char* s);



/*
    Se p for macro expande completamente e retorna o ponteiro
    para o primeiro item da expansao
    caso contrario, se p nao for macro, retorna null.
*/
struct token_list expand_macro(struct preprocessor_ctx* ctx, struct macro_expanded* pList, struct macro* macro, struct macro_argument_list* arguments, int level, struct error* error)
{
    macro->usage++;

    //printf("\nexpanding ");
    //print_macro(macro);
    //print_macro_arguments(arguments);
    //printf("\n");
    struct token_list r = { 0 };
    try
    {
        assert(!macro_already_expanded(pList, macro->name));
        struct macro_expanded macro_expanded = { 0 };
        macro_expanded.name = macro->name;
        macro_expanded.pPrevious = pList;
        if (macro->is_function)
        {
            struct token_list copy = macro_copy_replacement_list(ctx, macro, error);
            struct token_list copy2 = replace_macro_arguments(ctx, &macro_expanded, &copy, arguments, error);
            if (error->code) throw;

            struct token_list r2 = replacement_list_reexamination(ctx, &macro_expanded, &copy2, level, error);
            if (error->code) throw;

            token_list_append_list(&r, &r2);
        }
        else
        {
            struct token_list copy = macro_copy_replacement_list(ctx, macro, error);
            struct token_list r3 = replacement_list_reexamination(ctx, &macro_expanded, &copy, level, error);
            if (error->code) throw;

            token_list_append_list(&r, &r3);
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

struct token_list text_line(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, struct error* error)
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
            //assert(start_token->token_origin != NULL);

            if (is_active && input_list->head->type == TK_IDENTIFIER)
            {


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
                struct macro_argument_list arguments = collect_macro_arguments(ctx, macro, input_list, level, error);
                if (error->code) throw;


                struct token_list start_macro = expand_macro(ctx, NULL, macro, &arguments, level, error);
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
                        current->flags |= TK_FLAG_HIDE;
                    }

                    //mostra a expansao da macro
                    /*teste de expandir so algumas macros*/
                    for (struct token* current = start_macro.head;
                        current != start_macro.tail->next;
                        current = current->next)
                    {
                        current->flags = current->flags & ~TK_FLAG_MACRO_EXPANDED;
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
                    if (input_list->head->type == TK_IDENTIFIER)
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
                            struct macro_argument_list arguments2 = collect_macro_arguments(ctx, macro, input_list, level, error);
                            if (error->code) throw;

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


                            struct token_list r3 = expand_macro(ctx, NULL, macro, &arguments2, level, error);
                            if (error->code) throw;

                            //seta nos tokens expandidos da onde eles vieram
                            token_list_set_file(&r3, start_token->token_origin, start_token->line, start_token->col);

                            if (r3.head)
                            {
                                r3.head->flags = flags2;
                            }
                            token_list_append_list_at_beginning(input_list, &r3);
                        }
                    }
                }
                continue;
                //saiu tetris...
                //entao tudo foi expandido desde a primeiroa
            }
            else
            {
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

struct token_list group_part(struct preprocessor_ctx* ctx, struct token_list* input_list, bool is_active, int level, struct error* error)
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
            return if_section(ctx, input_list, is_active, level, error);
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
            return control_line(ctx, input_list, is_active, level, error);
        }
        else
        {
            //aqui vou consumir o # dentro para ficar simetrico
            return non_directive(ctx, input_list, level, error);
        }
    }
    return text_line(ctx, input_list, is_active, level, error);
}


struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level, struct error* error)
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

    struct token_list g = group_opt(ctx, input_list, true /*active*/, level, error);
    token_list_append_list(&r, &g);
    return r;
}


void mark_macros_as_used(struct hash_map* map)
{
    /*
     *  Objetivo era alertar macros nao usadas...
     */

    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct map_entry* pentry = map->table[i];

            while (pentry != NULL)
            {
                struct macro* macro = container_of(pentry->p, struct macro, type_id);
                macro->usage = 1;
                pentry = pentry->next;
            }
        }
    }
}

void check_unused_macros(struct hash_map* map)
{
    /*
     *  Objetivo era alertar macros nao usadas...
     */

    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct map_entry* pentry = map->table[i];

            while (pentry != NULL)
            {
                struct macro* macro = container_of(pentry->p, struct macro, type_id);
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

void add_standard_macros(struct preprocessor_ctx* ctx, struct error* error)
{

    static char mon[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };

    time_t now = time(NULL);
    struct tm* tm = localtime(&now);

    char datastr[100] = { 0 };
    snprintf(datastr, sizeof datastr, "#define __DATE__ \"%s %2d %d\"\n", mon[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);
    struct token_list l1 = tokenizer(datastr, "__DATE__ macro inclusion", 0, TK_FLAG_NONE, error);
    preprocessor(ctx, &l1, 0, error);

    char timestr[100] = { 0 };
    snprintf(timestr, sizeof timestr, "#define __TIME__ \"%02d:%02d:%02d\"\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    struct token_list l2 = tokenizer(timestr, "__TIME__ macro inclusion", 0, TK_FLAG_NONE, error);
    preprocessor(ctx, &l2, 0, error);


    /*
      Some macros are dynamic like __LINE__ they are replaced  at
      macro_copy_replacement_list but they need to be registered here.
    */

    const char* pre_defined_macros_text =
        "#define __CAKE__ 202311L\n"
        "#define __STDC_VERSION__ 202311L\n"
        "#define __FILE__ \"__FILE__\"\n"
        "#define __LINE__ 0\n"
        "#define __COUNT__ 0\n"
        "#define _CONSOLE\n"

#ifdef WIN32
        "#define _WINDOWS\n"
        "#define _WIN32\n"
#endif

#ifdef __linux__
        "#define __linux__\n"
#endif

        "#define _M_IX86\n"
        "#define _X86_\n"
        "#define __fastcall\n"
        "#define __stdcall\n"
        "#define __cdecl\n"
        "#define __pragma(a)\n"
        "#define __declspec(a)\n"
        "#define __crt_va_start(X) \n"
        "#define __builtin_offsetof(type, member) 0\n"; //como nao defini msver ele pensa que eh gcc aqui

    struct token_list l = tokenizer(pre_defined_macros_text, "standard macros inclusion", 0, TK_FLAG_NONE, error);
    struct token_list l10 = preprocessor(ctx, &l, 0, error);

    //nao quer ver warning de nao usado nestas macros padrao
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    token_list_destroy(&l10);
}




const char* get_token_name(enum token_type tk)
{
    switch (tk)
    {
    case TK_NONE: return "NONE";
    case TK_NEWLINE: return "NEWLINE";
    case TK_WHITE_SPACE: return "SPACE";
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
    case TK_STRING_LITERAL: return "TK_STRING_LITERAL";
    case TK_LINE_COMMENT: return "TK_LINE_COMMENT";
    case TK_COMENT: return "TK_COMENT";
    case TK_PPNUMBER: return "TK_PPNUMBER";
    case ANY_OTHER_PP_TOKEN: return "ANY_OTHER_PP_TOKEN";
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
    case TK_IDENTIFIER_RECURSIVE_MACRO: return "TK_IDENTIFIER_RECURSIVE_MACRO";
    case TK_BEGIN_OF_FILE: return "TK_BEGIN_OF_FILE";
    case TK_KEYWORD_AUTO: return "TK_KEYWORD_AUTO";
    case TK_KEYWORD_BREAK: return "TK_KEYWORD_BREAK";
    case TK_KEYWORD_CASE: return "TK_KEYWORD_CASE";
    case TK_KEYWORD_CONSTEXPR: return "TK_KEYWORD_CONSTEXPR";
    case TK_KEYWORD_CHAR: return "TK_KEYWORD_CHAR";
    case TK_KEYWORD_CONST: return "TK_KEYWORD_CONST";
    case TK_KEYWORD_CONTINUE: return "TK_KEYWORD_CONTINUE";
    case TK_KEYWORD_DEFAULT: return "TK_KEYWORD_DEFAULT";
    case TK_KEYWORD_DO: return "TK_KEYWORD_DO";
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
    case TK_KEYWORD_HASHOF: return "TK_KEYWORD_HASHOF";
    case TK_KEYWORD_STATIC: return "TK_KEYWORD_STATIC";
    case TK_KEYWORD_STRUCT: return "TK_KEYWORD_STRUCT";
    case TK_KEYWORD_SWITCH: return "TK_KEYWORD_SWITCH";
    case TK_KEYWORD_TYPEDEF: return "TK_KEYWORD_TYPEDEF";
    case TK_KEYWORD_UNION: return "TK_KEYWORD_UNION";
    case TK_KEYWORD_UNSIGNED: return "TK_KEYWORD_UNSIGNED";
    case TK_KEYWORD_VOID: return "TK_KEYWORD_VOID";
    case TK_KEYWORD_VOLATILE: return "TK_KEYWORD_VOLATILE";
    case TK_KEYWORD_WHILE: return "TK_KEYWORD_WHILE";
    case TK_KEYWORD__ALIGNAS: return "TK_KEYWORD__ALIGNAS";
    case TK_KEYWORD__ALIGNOF: return "TK_KEYWORD__ALIGNOF";
    case TK_KEYWORD__ATOMIC: return "TK_KEYWORD__ATOMIC";
    case TK_KEYWORD__BOOL: return "TK_KEYWORD__BOOL";
    case TK_KEYWORD__COMPLEX: return "TK_KEYWORD__COMPLEX";
    case TK_KEYWORD__DECIMAL128: return "TK_KEYWORD__DECIMAL128";
    case TK_KEYWORD__DECIMAL32: return "TK_KEYWORD__DECIMAL32";
    case TK_KEYWORD__DECIMAL64: return "TK_KEYWORD__DECIMAL64";
    case TK_KEYWORD__GENERIC: return "TK_KEYWORD__GENERIC";
    case TK_KEYWORD__IMAGINARY: return "TK_KEYWORD__IMAGINARY";
    case TK_KEYWORD__NORETURN: return "TK_KEYWORD__NORETURN";
    case TK_KEYWORD__STATIC_ASSERT: return "TK_KEYWORD__STATIC_ASSERT";
    case TK_KEYWORD__THREAD_LOCAL: return "TK_KEYWORD__THREAD_LOCAL";
    case TK_KEYWORD_TYPEOF: return "TK_KEYWORD_TYPEOF";
    case TK_KEYWORD_TYPEID: return "TK_KEYWORD_TYPEID";

    case TK_KEYWORD_TRUE: return "TK_KEYWORD_TRUE";
    case TK_KEYWORD_FALSE: return "TK_KEYWORD_FALSE";
    case TK_KEYWORD_NULLPTR: return "TK_KEYWORD_NULLPTR";
    case TK_KEYWORD_DEFER: return "TK_KEYWORD_DEFER";
    case TK_KEYWORD__BITINT: return "TK_KEYWORD__BITINT";
    case TK_KEYWORD__ASM: return "TK_KEYWORD__ASM";
    case TK_KEYWORD_CATCH: return "TK_KEYWORD_CATCH";
    case TK_KEYWORD_TRY: return "TK_KEYWORD_TRY";
    case TK_KEYWORD_THROW: return "TK_KEYWORD_THROW";
    case TK_KEYWORD_REPEAT: return "TK_KEYWORD_REPEAT";
    case TK_KEYWORD_TYPEOF_UNQUAL: return "TK_KEYWORD_TYPEOF_UNQUAL";


    }
    assert(false);
    return "";
};


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





const char* get_code_as_we_see_plusmacros(struct token_list* list)
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
    
    const char* cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);    

    return cstr;
}

const char* get_code_as_we_see(struct token_list* list, bool remove_comments)
{
    struct osstream ss = { 0 };
    struct token* current = list->head;
    while (current != list->tail->next)
    {
        if (current->level == 0 &&
            !(current->flags & TK_FLAG_MACRO_EXPANDED) &&
            !(current->flags & TK_FLAG_HIDE) &&
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
                else if (current->type == TK_COMENT)
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
    
    const char* cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}


const char* get_code_as_compiler_see(struct token_list* list)
{
    struct osstream ss = { 0 };
    struct token* current = list->head;
    while (current != list->tail->next)
    {
        if (!(current->flags & TK_FLAG_HIDE) &&
            current->type != TK_BEGIN_OF_FILE &&
            (current->flags & TK_FLAG_FINAL))
        {
            if (current->flags & TK_FLAG_HAS_SPACE_BEFORE)
                ss_fprintf(&ss, " ");

            if (current->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
                ss_fprintf(&ss, "\n");

            if (current->type == TK_LINE_COMMENT)
                ss_fprintf(&ss, "\n");
            else if (current->type == TK_COMENT)
                ss_fprintf(&ss, " ");
            else
                ss_fprintf(&ss, "%s", current->lexeme);
        }
        current = current->next;
    }

    const char* cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}

const char* print_preprocessed_to_string2(struct token* p_token)
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

    const char* cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}

const char* print_preprocessed_to_string(struct token* p_token)
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
        current->type == TK_COMENT ||
        current->type == TK_LINE_COMMENT ||
        current->type == TK_NEWLINE ||
        current->type == TK_PREPROCESSOR_LINE)
    {
        current = current->next;
        if (current == NULL)
            return ss.c_str;
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

    const char* cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}

void print_preprocessed(struct token* p_token)
{
    const char* s = print_preprocessed_to_string(p_token);
    if (s)
    {
        printf("%s", s);
        free((void*)s);
    }
}

static bool is_screaming_case(const char* text)
{
    if (text == NULL)
        return true;

    if (!(text[0] >= 'A' && text[0] <= 'Z'))
    {
        /*first letter lower case*/
        return false;
    }

    while (*text)
    {
        if ((*text >= 'A' && *text <= 'Z') ||
            (*text >= '0' && *text <= '9') ||
            (*text == '_'))
        {
            //ok
        }
        else
            return false;
        text++;
    }

    return true;

}

void naming_convention_macro(struct preprocessor_ctx* ctx, struct token* token)
{
    if (!ctx->options.check_naming_conventions || token->level != 0)
        return;

    if (!is_screaming_case(token->lexeme)) {
        pre_setinfo_with_token(ctx, token, "use SCREAMING_CASE for macros");
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
            free((void*)s);
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
    struct error error = { 0 };
    struct token_list list = tokenizer(input, "source", 0, TK_FLAG_NONE, &error);

    struct preprocessor_ctx ctx = { 0 };
    ctx.printf = printf;

    struct token_list r = preprocessor(&ctx, &list, 0, &error);
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
    free((void*)s);
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

static int printf_nothing(char const* const _Format, ...) { return 0; }

int test_preprocessor_in_out(const char* input, const char* output, struct error* error_opt)
{
    int res = 0;
    struct error error = { 0 };
    struct token_list list = tokenizer(input, "source", 0, TK_FLAG_NONE, &error);

    struct preprocessor_ctx ctx = { 0 };
    ctx.printf = printf_nothing;

    struct token_list r = preprocessor(&ctx, &list, 0, &error);
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

    if (error_opt)
        *error_opt = error;

    return res;
}

int test_preprocessor_in_out_using_file(const char* fileName)
{
    int res = 0;
    const char* input = normalize_line_end(readfile(fileName));
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
        res = test_preprocessor_in_out(input, output, NULL);
        free((void*)input);
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
    struct error error = { 0 };
    struct token_list list = { 0 };
    token_list_pop_front(&list);

    list = tokenizer("a", NULL, 0, TK_FLAG_NONE, &error);
    token_list_pop_front(&list);

    list = tokenizer("a,", NULL, 0, TK_FLAG_NONE, &error);
    token_list_pop_front(&list);

    list = tokenizer("a,b", NULL, 0, TK_FLAG_NONE, &error);
    token_list_pop_front(&list);
}

void token_list_pop_back_test()
{
    struct error error = { 0 };
    struct token_list list = { 0 };
    token_list_pop_back(&list);

    /*pop back quando so tem 1*/
    token_list_clear(&list);
    list = tokenizer("a", NULL, 0, TK_FLAG_NONE, &error);
    token_list_pop_back(&list);
    assert(list.head == NULL && list.tail == NULL);


    /*
    * pop bacl com 2
    */
    token_list_clear(&list);
    list = tokenizer("a,", NULL, 0, TK_FLAG_NONE, &error);
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
    list = tokenizer("a,b", NULL, 0, TK_FLAG_NONE, &error);
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
    struct error error = { 0 };

    struct token_list source = { 0 };
    struct token_list dest = tokenizer("a", NULL, 0, TK_FLAG_NONE, &error);
    token_list_append_list(&dest, &source);
    assert(strcmp(dest.head->lexeme, "a") == 0);


    token_list_clear(&source);
    token_list_clear(&dest);
    dest = tokenizer("a", NULL, 0, TK_FLAG_NONE, &error);
    token_list_append_list(&dest, &source);

    assert(strcmp(dest.head->lexeme, "a") == 0);

    token_list_clear(&source);
    token_list_clear(&dest);
    source = tokenizer("a,", NULL, 0, TK_FLAG_NONE, &error);
    dest = tokenizer("1", NULL, 0, TK_FLAG_NONE, &error);
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

    struct error error = { 0 };
    assert(test_preprocessor_in_out(input, output, &error) == 0);

}

void test_va_opt_0()
{
    const char* input =
        "#define F(...)  f(0 __VA_OPT__(,) __VA_ARGS__)\n"
        "F(a, b, c)";
    const char* output =
        "f(0, a, b, c)";
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
}

void test_va_opt_1()
{
    const char* input =
        "#define F(...)  f(0 __VA_OPT__(,) __VA_ARGS__)\n"
        "F()";
    const char* output =
        "f(0)";
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
}

void test_va_opt()
{
    //TODO esta falando um  monte de casos ainda ...
    // //http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2856.htm
    // 
    //demstra que primerio
    //tem que expandir varargs
    //para depois concluir se era vazio ou nao
    //
    const char* input =
        "#define F(...)  f(0 __VA_OPT__(,) __VA_ARGS__)\n"
        "#define EMPTY\n"
        "F(EMPTY)";
    const char* output =
        "f(0)";
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
}


void test_empty_va_args()
{
    const char* input = "#define M(a, ...) a, __VA_ARGS__\n"
        "M(1)\n";
    const char* output =
        "1,";
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
}

void test_va_args_single()
{
    const char* input =
        "#define F(...) __VA_ARGS__\n"
        "F(1, 2)";
    const char* output =
        "1, 2";
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
}

void test_va_args_extra_args()
{
    const char* input =
        "#define F(a, ...) a __VA_ARGS__\n"
        "F(0, 1, 2)";
    const char* output =
        "0 1, 2";
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
}


void test_empty_va_args_empty()
{
    const char* input =
        "#define F(...) a __VA_ARGS__\n"
        "F()";
    const char* output =
        "a";
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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

    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    //test_preprocessor_in_out(input, output, NULL);
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
    struct error error = { 0 };
    struct token_list list = tokenizer(input, "source", 0, TK_FLAG_NONE, &error);

    struct preprocessor_ctx ctx = { 0 };
    ctx.printf = printf;

    struct token_list r = preprocessor(&ctx, &list, 0, &error);

    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
}

void empty_and_no_args()
{
    const char* input =
        "#define F() 1\n"
        "F()";
    const char* output =
        "1"
        ;
    assert(test_preprocessor_in_out(input, output, NULL) == 0);
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

    struct error error = { 0 };
    test_preprocessor_in_out(input, output, &error);

    //esperado um erro (falta mensagem)
    //too few arguments provided to function-like macro invocation F (3)
    //engracado msc eh warning  warning C4003: not enough actual parameters for macro 'F'
    assert(error.code != 0);
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

    struct error error = { 0 };
    test_preprocessor_in_out(input, output, &error);

    assert(error.code == 0);
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
    test_preprocessor_in_out(input, output, NULL);
    */
}

int test_preprocessor_expression(const char* expr, long long expected)
{
    struct error error = { 0 };
    struct preprocessor_ctx ctx = { 0 };
    ctx.printf = printf;
    struct token_list r = { 0 };
    struct token_list input = tokenizer(expr, "", 0, TK_FLAG_NONE, &error);

    long long result = preprocessor_constant_expression(&ctx, &r, &input, 0, &error);
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

    struct error error = { 0 };
    return test_preprocessor_in_out(input, output, &error);
}

int test_concatenation()
{
    const char* input =
        "#define F(t1, t2, t3) i##j##k\n"
        "F(A, B, C)\n";

    const char* output =
        "ijk"
        ;

    struct error error = { 0 };
    return test_preprocessor_in_out(input, output, &error);


}

void bad_test()
{
    struct error error = { 0 };
    struct token_list list = tokenizer("0xfe-BAD(3)", "source", 0, TK_FLAG_NONE, &error);

    const char* input = "#define BAD(x) ((x) & 0xff)\n"
        "0xfe-BAD(3);";
    const char* output =
        "0xfe-BAD(3);"
        ;

    return test_preprocessor_in_out(input, output, &error);

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

    struct error error = { 0 };
    test_preprocessor_in_out(input, output, &error);

    return error.code;
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

    struct error error = { 0 };
    test_preprocessor_in_out(input, output, &error);

    return error.code;
}


int test_tokens()
{
    const char* input =
        "L\"s1\" u8\"s2\""
        ;
    struct error error = { 0 };
    struct token_list list
        = tokenizer(input, "", 0, TK_FLAG_NONE, &error);

    if (list.head->next->type != TK_STRING_LITERAL)
    {
        return __LINE__;
    }

    if (list.head->next->next->next->type != TK_STRING_LITERAL)
    {
        return __LINE__;
    }

    return error.code;
}

int test_predefined_macros()
{
    const char* input =
        "__LINE__ __FILE__"
        ;
    const char* output =
        "1 \"source\""
        ;

    struct error error = { 0 };
    struct token_list list = tokenizer(input, "", 0, TK_FLAG_NONE, &error);

    struct preprocessor_ctx prectx = { 0 };
    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx, &error);
    struct token_list list2 = preprocessor(&prectx, &list, 0, &error);


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
    struct error error = { 0 };
    const char* input =
        "u8\"maçã\"";
    struct token_list list = tokenizer(input, "source", 0, TK_FLAG_NONE, &error);
    if (strcmp(list.head->next->lexeme, u8"u8\"maçã\"") != 0)
        return __LINE__;
    token_list_destroy(&list);
    return 0;
}


int test_line_continuation()
{
    struct error error = { 0 };

    const char* input =
        "#define A B \\\n"
        "C\n"
        "A";

    const char* output =
        "1 \"source\""
        ;


    struct token_list list = tokenizer(input, "", 0, TK_FLAG_NONE, &error);

    struct preprocessor_ctx prectx = { 0 };
    prectx.macros.capacity = 5000;

    struct token_list list2 = preprocessor(&prectx, &list, 0, &error);

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

#endif
