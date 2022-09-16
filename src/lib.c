


#include <stdlib.h>


#include <assert.h>


#include <stdio.h>


#include <string.h>


//#pragma once



#include <stdbool.h>

#ifdef _WIN32


#include <conio.h>
#else
//#include <curses.h>
#endif

#define ESC "\x1b"
#define CSI "\x1b["

bool enable_vt_mode(void);

//#define DISABLE_COLORS 1

#ifdef DISABLE_COLORS
#define BLACK      ""
#define BLUE      ""
#define GREEN     ""
#define CYAN     ""
#define RED ""
#define MAGENTA      ""
#define BROWN     ""
#define LIGHTGRAY  ""
#define DARKGRAY ""
#define LIGHTBLUE ""
#define  LIGHTGREEN ""
#define LIGHTCYAN  ""
#define LIGHTRED ""
#define LIGHTMAGENTA  ""
#define YELLOW ""
#define WHITE ""

//https//en.wikipedia.org/wiki/ANSI_escape_code


#define BK_BLACK "" 
#define BK_BLUE ""
#define BK_GREEN  ""
#define BK_CYAN ""
#define BK_RED ""
#define BK_MAGENTA ""
#define BK_BROWN ""
#define BK_LIGHTGRAY ""
#define BK_DARKGRAY ""
#define BK_LIGHTBLUE ""
#define BK_LIGHTGREEN ""
#define BK_LIGHTCYAN ""
#define BK_LIGHTRED ""
#define BK_LIGHTMAGENTA ""
#define BK_YELLOW       ""
#define BK_WHITE        ""
#define BK_BLINK ""
#define RESET ESC ""
#else
/*change foreground color*/

#define BLACK     "\x1b[30m"
#define BLUE     "\x1b[34m"
#define GREEN     "\x1b[32m"
#define CYAN     "\x1b[36m"
#define RED "\x1b[31;1m"
#define MAGENTA     "\x1b[35m"
#define BROWN     "\x1b[31m"
#define LIGHTGRAY "\x1b[37m"
#define DARKGRAY "\x1b[90m"
#define LIGHTBLUE    "\x1b[34;1m"
#define  LIGHTGREEN "\x1b[92m"
#define LIGHTCYAN "\x1b[36;1m"
#define LIGHTRED "\x1b[91m"
#define LIGHTMAGENTA "\x1b[95m"
#define YELLOW "\x1b[93m"
#define WHITE "\x1b[97m"

    //https//en.wikipedia.org/wiki/ANSI_escape_code


#define BK_BLACK "\x1b[40m"
#define BK_BLUE "\x1b[44m"
#define BK_GREEN  "\x1b[42m"
#define BK_CYAN "\x1b[46m"
#define BK_RED "\x1b[41;1m"
#define BK_MAGENTA "\x1b[45m"
#define BK_BROWN "\x1b[41m"
#define BK_LIGHTGRAY "\x1b[40;1m"
#define BK_DARKGRAY "\x1b[40m"
#define BK_LIGHTBLUE "\x1b[44;1m"
#define BK_LIGHTGREEN "\x1b[42,1m"
#define BK_LIGHTCYAN "\x1b[46;1m"
#define BK_LIGHTRED "\x1b[41;1m"
#define BK_LIGHTMAGENTA "\x1b[45;1m"
#define BK_YELLOW             "\x1b[43;1m"
#define BK_WHITE             "\x1b[47;1m"
#define BK_BLINK "\x1b[40m"
            
#define RESET ESC "[0m"

#endif


int c_kbhit(void);
int c_getch(void);

void c_clrscr();


//#pragma once


#include <stdarg.h>

struct osstream
{
    char* c_str;
    int size;
    int capacity;
};


int ss_close(struct osstream* stream);
int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args);
int ss_fprintf(struct osstream* stream, const char* fmt, ...);
int ss_putc(char ch, struct osstream* stream);
void ss_clear(struct osstream* stream);



//#pragma once


//#pragma once

struct type_tag_id
{
    int type;
};

struct map_entry {
    struct map_entry* next;
    unsigned int hash;
    char* key;
    struct type_tag_id* p;
};

struct hash_map {
    struct map_entry** table;
    int capacity;
    int  size;
};

void hashmap_remove_all(struct hash_map* pMap);
void hashmap_destroy(struct hash_map* pMap);
struct type_tag_id* hashmap_find(struct hash_map* pMap, const char* key);
struct type_tag_id* hashmap_remove(struct hash_map* map, const char* key);
int hashmap_set(struct hash_map* pMap, const char* key, struct type_tag_id* pNew);



//#pragma once

enum token_type
{
    TK_NONE = 0,
    TK_NEWLINE = '\n',
    TK_WHITE_SPACE = ' ',
    TK_EXCLAMATION_MARK = '!',
    TK_QUOTATION_MARK = '"',
    TK_NUMBER_SIGN = '#',
    TK_DOLLAR_SIGN = '$',
    TK_PERCENT_SIGN = '%',
    TK_AMPERSAND = '&',
    TK_APOSTROPHE = '\'',
    TK_LEFT_PARENTHESIS = '(',
    TK_RIGHT_PARENTHESIS = ')',
    TK_ASTERISK = '*',
    TK_PLUS_SIGN = '+',
    TK_COMMA = ',',
    TK_HYPHEN_MINUS = '-',
    TK_FULL_STOP = '.',
    TK_SOLIDUS = '/',
    TK_COLON = ':',
    TK_SEMICOLON = ';',
    TK_LESS_THAN_SIGN = '<',
    TK_EQUALS_SIGN = '=',
    TK_GREATER_THAN_SIGN = '>',
    TK_QUESTION_MARK = '?',
    TK_COMMERCIAL_AT = '@',
    TK_LEFT_SQUARE_BRACKET = '[',
    TK_REVERSE_SOLIDUS = '//',
    TK_RIGHT_SQUARE_BRACKET = ']',
    TK_CIRCUMFLEX_ACCENT = '^',
    TK_FLOW_LINE = '_',
    TK_GRAVE_ACCENT = '`',
    TK_LEFT_CURLY_BRACKET = '{',
    TK_VERTICAL_LINE = '|',
    TK_RIGHT_CURLY_BRACKET = '}',
    TK_TILDE = '~',
    TK_PREPROCESSOR_LINE,
    TK_STRING_LITERAL,
    TK_CHAR_CONSTANT,
    TK_LINE_COMMENT,
    TK_COMENT,
    TK_PPNUMBER,

    ANY_OTHER_PP_TOKEN, //@ por ex

    /*PPNUMBER sao convertidos para constantes antes do parse*/
    TK_COMPILER_DECIMAL_CONSTANT,
    TK_COMPILER_OCTAL_CONSTANT,
    TK_COMPILER_HEXADECIMAL_CONSTANT,
    TK_COMPILER_BINARY_CONSTANT,
    TK_COMPILER_DECIMAL_FLOATING_CONSTANT,
    TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT,
    

    TK_PLACEMARKER,

    TK_BLANKS,
    TK_PLUSPLUS = '++',
    TK_MINUSMINUS = '--',
    TK_ARROW = '->',
    TK_SHIFTLEFT = '<<',
    TK_SHIFTRIGHT = '>>',
    TK_LOGICAL_OPERATOR_OR = '||',
    TK_LOGICAL_OPERATOR_AND = '&&',

    TK_MACRO_CONCATENATE_OPERATOR = '##',

    TK_IDENTIFIER,
    TK_IDENTIFIER_RECURSIVE_MACRO, /*usado para evitar recursao expansao macro*/

    TK_BEGIN_OF_FILE,

    //C23 keywords
    TK_KEYWORD_AUTO,
    TK_KEYWORD_BREAK,
    TK_KEYWORD_CASE,
    TK_KEYWORD_CONSTEXPR,
    TK_KEYWORD_CHAR,
    TK_KEYWORD_CONST,
    TK_KEYWORD_CONTINUE,
    TK_KEYWORD_CATCH, /*extension*/
    TK_KEYWORD_DEFAULT,
    TK_KEYWORD_DO,
    TK_KEYWORD_DEFER, /*extension*/
    TK_KEYWORD_DOUBLE,
    TK_KEYWORD_ELSE,
    TK_KEYWORD_ENUM,
    TK_KEYWORD_EXTERN,
    TK_KEYWORD_FLOAT,
    TK_KEYWORD_FOR,
    TK_KEYWORD_GOTO,
    TK_KEYWORD_IF,
    TK_KEYWORD_INLINE,
    TK_KEYWORD_INT,
    TK_KEYWORD_LONG,
    TK_KEYWORD__INT8,
    TK_KEYWORD__INT16,
    TK_KEYWORD__INT32,
    TK_KEYWORD__INT64,

    TK_KEYWORD_REGISTER,
    TK_KEYWORD_RESTRICT,
    TK_KEYWORD_RETURN,
    TK_KEYWORD_SHORT,
    TK_KEYWORD_SIGNED,
    TK_KEYWORD_SIZEOF,
    TK_KEYWORD_HASHOF, /*extension*/
    TK_KEYWORD_STATIC,
    TK_KEYWORD_STRUCT,
    TK_KEYWORD_SWITCH,
    TK_KEYWORD_TYPEDEF,
    TK_KEYWORD_TRY, /*extension*/
    TK_KEYWORD_THROW, /*extension*/
    TK_KEYWORD_UNION,
    TK_KEYWORD_UNSIGNED,
    TK_KEYWORD_VOID,
    TK_KEYWORD_VOLATILE,
    TK_KEYWORD_WHILE,
    TK_KEYWORD_REPEAT,
    TK_KEYWORD__ALIGNAS,
    TK_KEYWORD__ALIGNOF,
    TK_KEYWORD__ATOMIC,
    //microsoft
    //KEYWORD__FASTCALL,
    //KEYWORD__STDCALL
    // 
    TK_KEYWORD__ASM,
    //end microsoft
    TK_KEYWORD__BOOL,
    TK_KEYWORD__COMPLEX,
    TK_KEYWORD__DECIMAL128,
    TK_KEYWORD__DECIMAL32,
    TK_KEYWORD__DECIMAL64,
    TK_KEYWORD__GENERIC,
    TK_KEYWORD__IMAGINARY,
    TK_KEYWORD__NORETURN,
    TK_KEYWORD__STATIC_ASSERT,
    TK_KEYWORD__THREAD_LOCAL,

    TK_KEYWORD_TYPEOF, /*C23*/
    TK_KEYWORD_TYPEID, //Extension
    TK_KEYWORD_TRUE,  /*C23*/
    TK_KEYWORD_FALSE,  /*C23*/
    TK_KEYWORD_NULLPTR,  /*C23*/
    TK_KEYWORD_TYPEOF_UNQUAL, /*C23*/
    TK_KEYWORD__BITINT /*C23*/
};

enum token_flags
{
    TK_FLAG_NONE = 0,
    TK_FLAG_FINAL = 1 << 0,
    TK_FLAG_MACRO_EXPANDED = 1 << 1,    /*tokens expandidos pela macro*/
    TK_FLAG_HAS_SPACE_BEFORE = 1 << 2,
    TK_FLAG_HAS_NEWLINE_BEFORE = 1 << 3,
    TK_FLAG_IDENTIFIER_IS_TYPEDEF = 1 << 4, /*indica que eh identificador typedef*/
    TK_FLAG_IDENTIFIER_IS_NOT_TYPEDEF = 1 << 5,
    TK_FLAG_HIDE = 1 << 6, /*alguem pediu p esconder*/
    TK_FLAG_IDENTIFIER_IS_ENUMERATOR = 1 << 7, /*indica que eh identificador enumerator separar?*/
    TK_FLAG_IDENTIFIER_IS_NOT_ENUMERATOR = 1 << 8 /*indica que eh identificador enumerator separar?*/
};

struct token
{
    enum token_type type;
    char* lexeme;
    char* original;

    int line;
    int col;

    /*nivel de includes, 0 primeiro arquivo*/
    int level;

    enum token_flags flags;

    //apontar para token q tem file, include, se for o primeiro colocamos
    struct token* pFile;

    struct token* next;
    struct token* prev;
};

struct token_list
{
    struct token* head;
    struct token* tail;
};
void token_list_set_file(struct token_list* list, struct token* filetoken, int line, int col);
bool token_list_is_empty(struct token_list* p);
struct token* clone_token(struct token* p);
struct token* token_list_add(struct token_list* list, struct token* pnew);
struct token_list token_list_remove(struct token_list* list, struct token* first, struct token* last);
void token_list_append_list(struct token_list* dest, struct token_list* source);
void token_list_append_list_at_beginning(struct token_list* dest, struct token_list* source);
struct token* token_list_clone_and_add(struct token_list* list, struct token* pnew);
char* token_list_join_tokens(struct token_list* list, bool bliteral);
void token_list_clear(struct token_list* list);
bool token_is_blank(struct token* p);
void token_range_add_flag(struct token* first, struct token* last, enum token_flags flag);
void token_range_remove_flag(struct token* first, struct token* last, enum token_flags flag);
void token_range_add_show(struct token* first, struct token* last);

struct stream
{
    const char* source;
    const char* current;
    int line;
    int col;
};

int is_digit(struct stream* p);
int is_nondigit(struct stream* p);
void stream_match(struct stream* stream);

#define list_add(list, pnew)\
do {\
    void* pitem = (pnew);\
    if ((list)->head == NULL)\
    {\
        (list)->head = pitem;\
        (list)->tail = pitem;\
    } else \
    {\
        (list)->tail->next = pitem;\
        (list)->tail = pitem;\
    }\
} while(0)



//#pragma once

#define try  if (1)
#define catch else catch_label:
#define throw goto catch_label

struct error
{
    char message[200];
    int code;
};

/*
  The seterror function returns the number of characters that are written,
  not counting the terminating null character.

  If the buffer size specified by er->capacity isn't sufficiently large to contain
  the output specified by string_format and argptr, the return value of seterror is the
  number of characters that would be written, not counting the null character,
  if er->capacity were sufficiently large.

  If the return value is greater than count - 1, the output has been truncated.

  A return value of -1 indicates that an encoding error has occurred.
*/

#ifndef _In_z_
#define _In_z_
#define _Printf_format_string_
#endif

int seterror(struct error* er, _In_z_ _Printf_format_string_  const char* fmt, ...);



void clearerror(struct error* err);

const char* get_posix_error_message(int error);
int GetWindowsOrLinuxSocketLastErrorAsPosix(void);
int windows_error_to_posix(int i);


//#pragma once

/*
* Compiler options shared with compiler and preprocessor
*/

enum language_version
{
    LANGUAGE_C89 = -1,
    LANGUAGE_C99 = 0, /*default*/
    LANGUAGE_C11 = 1,
    LANGUAGE_C2X = 2, //C23
    LANGUAGE_CXX = 3, //experimental
};

struct options
{
    enum language_version input;
    enum language_version target;
    bool bRemoveComments;
    bool bPreprocessOnly;
    bool bRemoveMacros;
    bool format_input;
    bool format_ouput;

    /*
    * true - to info about name convensions violations
    */
    bool check_naming_conventions;
};



struct include_dir
{
    const char* path;
    struct include_dir* next;
};

struct include_dir_list
{
    struct include_dir* head;
    struct include_dir* tail;
};


enum preprocessor_ctx_flags
{
    PREPROCESSOR_CTX_FLAGS_NONE = 0,
    PREPROCESSOR_CTX_FLAGS_ONLY_FINAL = 1 << 0
};

struct preprocessor_ctx
{
    struct options options;
    enum preprocessor_ctx_flags flags;
    struct hash_map macros;
    struct include_dir_list include_dir;

    /*lista dos headers marcados com pragma once*/
    struct hash_map pragmaOnce;
    
    struct token* current;
    struct token_list input_list;

    bool bConditionalInclusion;
    int n_warnings;
    int n_errors;
    int (*printf)(const char* fmt, ...);
};
void preprocessor_ctx_destroy(struct preprocessor_ctx* p);


struct token_list tokenizer(const char* text, const char* filename_opt, int level, enum token_flags addflags, struct error* error);
void add_standard_macros(struct preprocessor_ctx* ctx, struct error* error);
struct include_dir* include_dir_add(struct include_dir_list* list, const char* path);

struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level, struct error* error);
struct token_list  copy_replacement_list(struct token_list* list);

void token_list_append_list(struct token_list* dest, struct token_list* source);
void print_list(struct token_list* list);
void token_list_destroy(struct token_list* list);
bool token_is_blank(struct token* p);
struct token* token_list_pop_back(struct token_list* list);
struct token* token_list_pop_front(struct token_list* list);
void remove_line_continuation(char* s);
struct token* token_list_clone_and_add(struct token_list* list, struct token* pnew);
struct token_list token_list_remove(struct token_list* list, struct token* first, struct token* last);
void token_list_insert_after(struct token_list* list, struct token* pAfter, struct token_list* append);
struct token_list tokenizer(const char* text, const char* filename_opt, int level, enum token_flags addflags, struct error* error);

const char* get_code_as_we_see(struct token_list* list, bool removeComments);
const char* get_code_as_compiler_see(struct token_list* list);

const char* get_code_as_we_see_plusmacros(struct token_list* list);
const char* get_code_as_we_see(struct token_list* list, bool removeComments);

void print_tokens(struct token* p_token);
void print_preprocessed(struct token* p_token);
const char* print_preprocessed_to_string(struct token* p_token);
const char* print_preprocessed_to_string2(struct token* p_token);
void check_unused_macros(struct hash_map* pMap);

char* readfile(const char* path);
const char* get_token_name(enum token_type tk);



#ifdef _WIN32


#include <crtdbg.h>


#include <Windows.h>


#include <debugapi.h>

#undef assert
#define assert _ASSERTE
#endif
void print_literal2(const char* s);

void token_list_clear(struct token_list* list)
{
    token_list_destroy(list);
    list->head = NULL;
    list->tail = NULL;
}


void token_range_add_show(struct token* first, struct token* last)
{
    for (struct token* current = first;
        current != last->next;
        current = current->next)
    {
        current->flags = current->flags & ~TK_FLAG_HIDE;        
    }
}

void token_range_remove_flag(struct token* first, struct token* last, enum token_flags flag)
{
    for (struct token* current = first;
        current != last->next;
        current = current->next)
    {
        current->flags = current->flags & ~flag;        
    }
}

void token_range_add_flag(struct token* first, struct token* last, enum token_flags flag)
{
    for (struct token* current = first;
        current != last->next;
        current = current->next)
    {
        current->flags |= flag;
    }
}

struct token* token_list_pop_back(struct token_list* list)
{
    if (list->head == NULL)
        return NULL;

    struct token* p = list->tail;
    if (list->head == list->tail)
    {
        list->head = list->tail = NULL;
    }
    else
    {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
        if (list->tail == list->head)
        {
            list->tail->prev = NULL;
        }
    }
    p->next = NULL;
    p->prev = NULL;
    return p;
 }

struct token* token_list_pop_front(struct token_list* list)
{
    if (list->head == NULL)
        return NULL;

    struct token* p = list->head;
    if (list->head == list->tail)
    {
        list->head = list->tail = NULL;
    }
    else
    {
        list->head = list->head->next;
    }
    p->next = NULL;
    p->prev = NULL;
    return p;
}

void token_delete(struct token* p)
{
    if (p)
    {
        free(p->lexeme);
        free(p);
    }
}

void token_list_set_file(struct token_list* list, struct token* filetoken, int line, int col)
{
    //assert(filetoken != NULL);
    struct token* p = list->head;
    while (p)
    {     
        p->pFile = filetoken;
        p->line = line;
        p->col = col;
        p = p->next;
    }
}

void token_list_destroy(struct token_list* list)
{
    struct token* p = list->head;
    while (p)
    {
        struct token* next = p->next;
        token_delete(p);
        p = next;
    }
}

char* token_list_join_tokens(struct token_list* list, bool bliteral)
{
    struct osstream ss = { 0 };
    if (bliteral)
        ss_fprintf(&ss, "\"");
    bool has_space = false;
    struct token* current = list->head;
    
    while (current)
    {
        if (token_is_blank(current))
        {
            has_space = true;
            current = current->next;
            continue;
        }

        if (has_space)
            ss_fprintf(&ss, " ");

        const char* p = current->lexeme;
        while (*p)
        {
            if (*p == '"')
              ss_fprintf(&ss, "\\\"");
            else
              ss_fprintf(&ss, "%c", *p);
            p++;
        }
        

        current = current->next;
        if (current)
          has_space = current->flags & TK_FLAG_HAS_SPACE_BEFORE;
    }

    if (bliteral)
        ss_fprintf(&ss, "\"");

    return ss.c_str;
}

void token_list_insert_after(struct token_list* token_list, struct token* after, struct token_list* append_list)
{
    if (append_list->head == NULL)
        return;

    if (after == NULL)
    {
        append_list->tail->next = token_list->head;
        token_list->head->prev = append_list->tail;

        token_list->head = append_list->head;
        append_list->head->prev = NULL;
    }
    else
    {
        struct token* follow = after->next;
        if (token_list->tail == after)
        {
            token_list->tail = append_list->tail;
        }
        else if (token_list->head == after)
        {
        }
        append_list->tail->next = follow;
        follow->prev = append_list->tail;
        after->next = append_list->head;
        append_list->head->prev = after;

    }
}

struct token* token_list_add(struct token_list* list, struct token* pnew)
{
    /*evitar que sem querer esteja em 2 listas diferentes*/
    assert(pnew->next == NULL);
    assert(pnew->prev == NULL);

    if (list->head == NULL)
    {
        pnew->prev = NULL;
        pnew->next = NULL;
        list->head = pnew;
        list->tail = pnew;
        //pnew->prev = list->tail;
    }
    else
    {
        pnew->prev = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
    assert(list->tail->next == NULL);
    return pnew;
}

int is_digit(struct stream* p)
{
    /*
     digit : one of
     0 1 2 3 4 5 6 7 8 9
    */
    return (p->current[0] >= '0' && p->current[0] <= '9');
}

bool token_is_blank(struct token* p)
{
    return p->type == TK_BEGIN_OF_FILE ||
        p->type == TK_BLANKS ||
        p->type == TK_LINE_COMMENT ||
        p->type == TK_COMENT;
}

struct token* token_list_clone_and_add(struct token_list* list, struct token* pnew)
{
    struct token* clone = clone_token(pnew);
    token_list_add(list, clone);
    return clone;
}

void token_list_append_list_at_beginning(struct token_list* dest, struct token_list* source)
{
    //print_list(source);
    //printf("->");
    //print_list(dest);
    if (source->head == NULL)
    {
        return;
    }
    if (dest->head == NULL)
    {
        dest->head = source->head;
        dest->tail = source->tail;
    }
    else
    {
        //dest->tail = source->head;
        source->tail->next = dest->head;
        dest->head = source->head;
    }
    //print_list(dest);
}

void token_list_append_list(struct token_list* dest, struct token_list* source)
{
    if (source->head == NULL)
    {
        return;
    }
    if (dest->head == NULL)
    {
        dest->head = source->head;
        dest->tail = source->tail;
    }
    else
    {
        dest->tail->next = source->head;
        source->head->prev = dest->tail;
        dest->tail = source->tail;
    }
}


struct token* clone_token(struct token* p)
{
    struct token* token = calloc(1, sizeof * token);
    if (token)
    {
        token->flags = p->flags;
        token->lexeme = strdup(p->lexeme);
        token->type = p->type;        
    }
    return token;
}

struct token_list token_list_remove(struct token_list* list, struct token* first, struct token* last)
{
    struct token_list r = { 0 };

    struct token* before_first = first->prev;
    struct token* after_last = last->next;

    before_first->next = after_last;
    after_last->prev = before_first;

    r.head = first;
    first->prev = NULL;
    r.tail = last;
    last->next = NULL;

    return r;
}



bool token_list_is_empty(struct token_list* p)
{
    assert((p->head == NULL && p->tail == NULL) ||
           (p->head != NULL && p->tail != NULL));

    return p->head == NULL;
}



void print_list(struct token_list* list)
{
    struct token* current = list->head;
    while (current)
    {
        if (current != list->head)
        {
            printf(u8"˰");
            //printf("`");
        }
        print_literal2(current->lexeme);
        printf(RESET);
        if (current == list->tail)
        {
            //printf("`");
        }
        current = current->next;
    }
    printf(u8"\n");
}

void print_literal2(const char* s)
{
    if (s == NULL)
    {
        //printf("`");
        //printf("`");
        return;
    }
    //printf("`");
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
    //printf("`");
}


void print_token(struct token* p_token)
{
    for (int i = 0; i < p_token->level; i++)
    {
        printf("  ");
    }
    if (p_token->flags & TK_FLAG_FINAL)
        printf(LIGHTGREEN);
    else
        printf(LIGHTGRAY);
    char buffer0[50] = { 0 };
    snprintf(buffer0, sizeof buffer0, "%d:%d", p_token->line, p_token->col);
    printf("%-6s ", buffer0);
    printf("%-20s ", get_token_name(p_token->type));
    if (p_token->flags & TK_FLAG_MACRO_EXPANDED)
    {
        printf(LIGHTCYAN);
    }
    char buffer[50] = { 0 };
    strcat(buffer, "[");
    if (p_token->flags & TK_FLAG_FINAL)
    {
        strcat(buffer, "final ");
    }
    if (p_token->flags & TK_FLAG_HIDE)
    {
        strcat(buffer, "hide ");
    }
    if (p_token->flags & TK_FLAG_MACRO_EXPANDED)
    {
        strcat(buffer, "expanded ");
    }
    if (p_token->flags & TK_FLAG_HAS_SPACE_BEFORE)
    {
        strcat(buffer, "space ");
    }
    if (p_token->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
    {
        strcat(buffer, "newline ");
    }
    strcat(buffer, "]");
    printf("%-20s ", buffer);
    print_literal2(p_token->lexeme);
    printf("\n");
    printf(RESET);
}

void print_tokens(struct token* p_token)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
    struct token* current = p_token;
    while (current)
    {
        print_token(current);
        current = current->next;
    }
    printf("\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
    printf(RESET);
}






//#pragma once

unsigned int stringhash(const char* key);

unsigned int stringhash(const char* key)
{
    // hash key to unsigned int value by pseudorandomizing transform
    // (algorithm copied from STL char hash in xfunctional)
    unsigned int hash_val = 2166136261U;
    unsigned int first = 0;
    unsigned int last = (unsigned int)strlen(key);
    unsigned int stride = 1 + last / 10;

    for (; first < last; first += stride)
    {
        hash_val = 16777619U * hash_val ^ (unsigned int)key[first];
    }

    return (hash_val);
}


#ifdef _WIN32 
#endif

extern void node_delete(struct type_tag_id* pNode);




void hashmap_remove_all(struct hash_map* map)
{
    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct map_entry* pentry = map->table[i];

            while (pentry != NULL)
            {
                struct map_entry* pentryCurrent = pentry;

                //node_delete(pentryCurrent->p);

                pentry = pentry->next;
                free(pentryCurrent->key);
                free(pentryCurrent);
            }
        }

        free(map->table);
        map->table = NULL;
        map->size = 0;
    }
}

void hashmap_destroy(struct hash_map* map)
{
    hashmap_remove_all(map);
}

struct type_tag_id* hashmap_find(struct hash_map* map, const char* key)
{
    if (map->table == NULL)
      return NULL;

    struct type_tag_id* p = NULL;

    unsigned int hash = stringhash(key);
    int index = hash % map->capacity;

    struct map_entry* pentry = map->table[index];

    for (; pentry != NULL; pentry = pentry->next)
    {
        if (pentry->hash == hash && strcmp(pentry->key, key) == 0) {
            p = pentry->p;
            break;
        }
    }

    return p;
}


struct type_tag_id* hashmap_remove(struct hash_map* map, const char* key)
{
    struct type_tag_id* p = 0;

    if (map->table != NULL)
    {
        unsigned int hash = stringhash(key);
        struct map_entry** preventry = &map->table[hash % map->capacity];
        struct map_entry* pentry = *preventry;

        for (; pentry != NULL; pentry = pentry->next)
        {
            if ((pentry->hash == hash) && (strcmp(pentry->key, key) == 0))
            {
                *preventry = pentry->next;
                p = pentry->p;
                free(pentry->key);
                free(pentry);
                break;
            }
            preventry = &pentry->next;
        }
    }

    return p;
}

int hashmap_set(struct hash_map* map, const char* key, struct type_tag_id* pNew)
{
    int result = 0;

    if (map->table == NULL)
    {
        if (map->capacity < 1) {
            map->capacity = 1000;
        }

        map->table = calloc(map->capacity, sizeof(map->table[0]));
    }

    if (map->table != NULL)
    {
        unsigned int hash = stringhash(key);
        int index = hash % map->capacity;

        struct map_entry* pentry = map->table[index];

        for (; pentry != NULL; pentry = pentry->next) {
            if (pentry->hash == hash && strcmp(pentry->key, key) == 0) {
                break;
            }
        }

        if (pentry == NULL)
        {
            pentry = calloc(1, sizeof(*pentry));
            pentry->hash = hash;
            pentry->p = pNew;
            pentry->key = strdup(key);
            pentry->next = map->table[index];
            map->table[index] = pentry;
            map->size++;
            result = 0;
        }
        else
        {
            result = 1;
            pentry->p = pNew;
        }
    }

    return result;
}



#ifdef _WIN32
#else


#include <termios.h>


#include <unistd.h>


#include <fcntl.h>


#include <sys/ioctl.h>
#endif

#ifndef WIN32

bool enable_vt_mode(void)
{
    return true;
}

int c_kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

/* Read 1 character without echo */
int c_getch(void)
{
    struct termios old, new;
    int ch;

    tcgetattr(0, &old);

    new = old;
    new.c_lflag &= ~ICANON;
    new.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &new);

    ch = getchar();

    tcsetattr(0, TCSANOW, &old);

    return ch;
}

#else

bool enable_vt_mode(void)
{
    DWORD dwMode = 0;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if ((hOut = GetStdHandle(STD_OUTPUT_HANDLE)) != INVALID_HANDLE_VALUE &&
            GetConsoleMode(hOut, &dwMode) != 0 &&
            SetConsoleMode(hOut, dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0 &&
            SetConsoleOutputCP(CP_UTF8) != 0)
    {
        return true;//ok
    }
    return false;//error
}

int c_kbhit(void)
{
    return _kbhit();
}
int c_getch(void)
{
    return _getch();
}

#endif

void c_clrscr()
{
    puts("\x1b[2J\x1b[1;1H");
    fflush(stdout);
}


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


#include <sys/stat.h>


#include <errno.h>


#include <stddef.h>


#include <time.h>


//#pragma once

#ifdef _WIN32 


#include <direct.h>


#include <sys/types.h>

//https://docs.microsoft.com/pt-br/cpp/c-runtime-library/reference/mkdir-wmkdir?view=msvc-160
#define mkdir(a, b) _mkdir(a)
#define rmdir _rmdir
#define chdir _chdir
/*
 opendir,  readdir closedir for windows.
 include dirent.h on linux
*/

enum
{
    DT_UNKNOWN = 0,
    DT_FIFO = 1,
    DT_CHR = 2,
    DT_DIR = 4,
    DT_BLK = 6,
    DT_REG = 8,
    DT_LNK = 10,
    DT_SOCK = 12,
    DT_WHT = 14
};

struct dirent
{
    ino_t d_ino;             /* Inode number */
    off_t d_off;             /* Not an offset; see below */
    unsigned short d_reclen; /* Length of this record */
    unsigned char d_type;    /* Type of file; not supported
                                     by all filesystem types*/
    char d_name[256];        /* Null-terminated filename */
};


struct TAGDIR;
typedef struct TAGDIR DIR;

DIR* opendir(const char* name);
int closedir(DIR* dirp);
struct dirent* readdir(DIR* dirp);


#else

#define MAX_PATH 500

//https://man7.org/linux/man-pages/man2/mkdir.2.html


#include <dirent.h>
#endif



int get_self_path(char* buffer, int maxsize);

char* readfile(const char* path);
char* dirname(char* path);

const char* get_posix_error_message(int error);






//#pragma once

struct preprocessor_ctx;

int pre_constant_expression(struct preprocessor_ctx* ctx, struct error* error, long long* pvalue);



#ifdef _WIN32
#undef assert
#define assert _ASSERTE
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
            p_token->pFile->lexeme,
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
        ctx->printf(LIGHTGREEN "^\n");
    }
}



void pre_setinfo_with_token(struct preprocessor_ctx* ctx, struct token* p_token, const char* fmt, ...)
{
    if (p_token)
    {
        ctx->printf(WHITE "%s:%d:%d: ",
            p_token->pFile->lexeme,
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

    ctx->printf(LIGHTCYAN "info: " WHITE "%s\n", buffer);

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
        ctx->printf(LIGHTGREEN "^\n");
    }
}

void pre_error_warning_with_token(struct preprocessor_ctx* ctx, struct token* p_token, bool bError)
{
    ctx->n_warnings++;
    //er->code = 1;

    if (p_token)
    {
        ctx->printf(WHITE "%s:%d:%d: ",
            p_token->pFile->lexeme,
            p_token->line,
            p_token->col);
    }
    else
    {
        ctx->printf(WHITE "<>");
    }

    if (bError)
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
    struct include_dir* pNew = calloc(1, sizeof * pNew);
    pNew->path = strdup(path);
    if (list->head == NULL)
    {
        list->head = pNew;
        list->tail = pNew;
    }
    else
    {
        list->tail->next = pNew;
        list->tail = pNew;
    }
    return pNew;
}


const char* find_and_read_include_file(struct preprocessor_ctx* ctx, const char* path, char fullpath[300], bool* bAlreadyIncluded)
{
    snprintf(fullpath, 300, "%s", path);

    if (hashmap_find(&ctx->pragmaOnce, fullpath) != NULL)
    {
        *bAlreadyIncluded = true;
        return NULL;
    }


    char* content = readfile(fullpath);
    if (content == NULL)
    {
        struct include_dir* current = ctx->include_dir.head;
        while (current)
        {
            snprintf(fullpath, 300, "%s%s", current->path, path);

            if (hashmap_find(&ctx->pragmaOnce, fullpath) != NULL)
            {
                *bAlreadyIncluded = true;
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
    struct token_list replacementList; /*copia*/
    struct macro_parameter* pParameters;
    bool bIsFunction;
    int usage;

    bool bExpand;
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
    //pnew->tokens
    struct token_list list = copy_argument_list_tokens(&pMacroArgument->tokens);
    if (list.head == NULL)
    {
        /*nunca eh vazio..se for ele colocar um TK_PLACEMARKER*/
        struct token* pNew = calloc(1, sizeof * pNew);
        pNew->lexeme = strdup("");
        pNew->type = TK_PLACEMARKER;
        token_list_add(&list, pNew);
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
    if (macro->bIsFunction)
        printf("(");
    struct macro_parameter* pParameter = macro->pParameters;
    while (pParameter)
    {
        if (macro->pParameters != pParameter)
            printf(",");
        printf("%s", pParameter->name);
        pParameter = pParameter->next;
    }
    if (macro->bIsFunction)
        printf(") ");
    print_list(&macro->replacementList);
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
    printf("%s\n", p->pFile->lexeme);
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
        break;
    case '*':
        type = '*';
        stream_match(stream);
        break;
    case '%':
        type = '%';
        stream_match(stream);
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
    struct token* pNew = calloc(1, sizeof * pNew);
    size_t sz = lexeme_tail - lexeme_head;
    pNew->lexeme = calloc(sz + 1, sizeof(char));
    pNew->type = type;
    strncpy(pNew->lexeme, lexeme_head, sz);
    return pNew;
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

    struct token* pNew = new_token(start, stream->current, TK_IDENTIFIER);


    return pNew;
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
    }
    stream_match(stream);

    if (stream->current - start > 6)
    {
        //warning: character constant too long for its type
    }
    struct token* pNew = new_token(start, stream->current, TK_CHAR_CONSTANT);

    return pNew;
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
    struct token* pNew = NULL;

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
        pNew = new_token(start, stream->current, TK_STRING_LITERAL);
    }
    catch
    {
    }

    return pNew;
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
        if (is_digit(stream))
        {
            stream_match(stream);//digit
        }
        else if (is_nondigit(stream))
        {
            stream_match(stream);//nondigit
        }
        else if (stream->current[0] == '\'')
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
        else if (stream->current[0] == 'e' ||
            stream->current[0] == 'E' ||
            stream->current[0] == 'p' ||
            stream->current[0] == 'P')
        {
            stream_match(stream);//e E  p P
            stream_match(stream);//sign
        }
        else if (stream->current[0] == '\'')
        {
            //digit separators dentro preprocessador
            stream_match(stream);//'
            stream_match(stream);//.
        }
        else if (stream->current[0] == '.')
        {
            stream_match(stream);//.            
        }
        else
        {
            break;
        }
    }
    struct token* pNew = new_token(start, stream->current, TK_PPNUMBER);
    return pNew;
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
                struct token* pNew = new_token(b, &b[1], TK_COMMA);
                pNew->flags |= addflags;
                pNew->level = level;
                pNew->pFile = NULL;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);

                if (count > 0 && count % 25 == 0)
                {
                    /*new line*/
                    char newline[] = "\n";
                    struct token* pNew3 = new_token(newline, &newline[1], TK_NEWLINE);
                    pNew3->level = level;
                    pNew3->pFile = NULL;
                    pNew3->line = line;
                    pNew3->col = col;
                    token_list_add(&list, pNew3);
                }
            }

            char buffer[30];
            int c = snprintf(buffer, sizeof buffer, "%d", (int)ch);

            struct token* pNew = new_token(buffer, &buffer[c], TK_PPNUMBER);
            pNew->flags |= addflags;
            pNew->level = level;
            pNew->pFile = NULL;
            pNew->line = line;
            pNew->col = col;
            token_list_add(&list, pNew);


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
    struct token* pNew = new_token(newline, &newline[1], TK_NEWLINE);
    pNew->level = level;
    pNew->pFile = NULL;
    pNew->line = line;
    pNew->col = col;
    token_list_add(&list, pNew);

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
                struct token* pNew = ppnumber(&stream);
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);
                bNewLine = false;
                bHasSpace = false;
                continue;
            }

            /*
             Tem que vir antes identifier
            */
            if (first_of_string_literal(&stream))
            {
                struct token* pNew = string_literal(&stream, error);
                if (pNew == NULL)
                    throw;
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);;
                bNewLine = false;
                bHasSpace = false;
                continue;
            }

            if (first_of_character_constant(&stream))
            {
                struct token* pNew = character_constant(&stream);
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);
                bNewLine = false;
                bHasSpace = false;
                continue;
            }

            if (is_nondigit(&stream))
            {
                struct token* pNew = identifier(&stream);
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);
                bNewLine = false;
                bHasSpace = false;
                continue;
            }
            if (stream.current[0] == ' ' || stream.current[0] == '\t')
            {
                const char* start = stream.current;
                while (stream.current[0] == ' ' ||
                    stream.current[0] == '\t')
                {
                    stream_match(&stream);
                }
                struct token* pNew = new_token(start, stream.current, TK_BLANKS);
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);
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
                struct token* pNew = new_token(start, stream.current, TK_LINE_COMMENT);
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);
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
                struct token* pNew = new_token(start, stream.current, TK_COMENT);
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);
                bNewLine = false;
                bHasSpace = false;
                continue;
            }
            if (bNewLine && stream.current[0] == '#')
            {
                const char* start = stream.current;
                stream_match(&stream);
                struct token* pNew = new_token(start, stream.current, '#');
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                pNew->type = TK_PREPROCESSOR_LINE;
                token_list_add(&list, pNew);
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
                struct token* pNew = new_token(newline, newline + 1, TK_NEWLINE);
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);
                bNewLine = true;
                bHasSpace = false;
                continue;
            }
            const char* start = stream.current;
            enum token_type t = is_punctuator(&stream);
            if (t != TK_NONE)
            {

                struct token* pNew = new_token(start, stream.current, t);
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);
                bNewLine = false;
                bHasSpace = false;
                continue;
            }
            else
            {
                stream_match(&stream);
                struct token* pNew = new_token(start, stream.current, ANY_OTHER_PP_TOKEN);
                pNew->flags |= bHasSpace ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= bNewLine ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                pNew->flags |= addflags;

                pNew->level = level;
                pNew->pFile = pFirst;
                pNew->line = line;
                pNew->col = col;
                token_list_add(&list, pNew);
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
                struct token* pNew = token_list_pop_front(input_list);
                pNew->type = TK_PPNUMBER;
                free(pNew->lexeme);
                if (macro)
                {
                    pNew->lexeme = strdup("1");
                }
                else
                {
                    pNew->lexeme = strdup("0");
                }

                token_list_add(&r, pNew);

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
                strcmp(input_list->head->lexeme, "__has_include") == 0)
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
                char* s = find_and_read_include_file(ctx, path, fullpath, &bAlreadyIncluded);



                bool bHasInclude = s != NULL;
                free(s);

                struct token* pNew = calloc(1, sizeof * pNew);
                pNew->type = TK_PPNUMBER;
                free(pNew->lexeme);
                pNew->lexeme = strdup(bHasInclude ? "1" : "0");
                pNew->flags |= TK_FLAG_FINAL;

                token_list_add(&r, pNew);
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


                //TODO ver se existe criar unit test
                assert(false);
                //bool bAlreadyIncluded = false;
                //char* content = find_and_read_file(ctx, path + 1, path, &bAlreadyIncluded);
                bool bHas_C_Attribute = false;
                //free(content);

                struct token* pNew = calloc(1, sizeof * pNew);
                pNew->type = TK_PPNUMBER;
                free(pNew->lexeme);
                pNew->lexeme = strdup(bHas_C_Attribute ? "1" : "0");
                pNew->flags |= TK_FLAG_FINAL;

                token_list_add(&r, pNew);
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
            struct token* pNew = token_list_pop_front(list);
            pNew->type = TK_PPNUMBER;

            if (macro)
            {
                free(pNew->lexeme);
                pNew->lexeme = strdup("1");
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
                if (strcmp(pNew->lexeme, "true") == 0)
                {
                    pNew->lexeme[0] = '1';
                    pNew->lexeme[1] = '\0';
                }
                else if (strcmp(pNew->lexeme, "false") == 0)
                {
                    pNew->lexeme[0] = '0';
                    pNew->lexeme[1] = '\0';
                }
                else
                {
                    free(pNew->lexeme);
                    pNew->lexeme = strdup("0");
                }
            }
            token_list_add(&list2, pNew);
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
    ctx->bConditionalInclusion = true;
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

    ctx->bConditionalInclusion = false;
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
    macro->pParameters = pMacroParameter;
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
    assert(macro->replacementList.head == NULL);
    struct token_list copy = copy_replacement_list(&r);
    token_list_append_list(&macro->replacementList, &copy);
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
            # include pp-tokens new-line
            # define identifier replacement-list new-line
            # define identifier ( identifier-list_opt ) replacement-list new-line
            # define identifier ( ... ) replacement-list new-line
            # define identifier lparen identifier-list , ... ) replacement-list new-line
            # undef identifier new-line
            # line pp-tokens new-line
            # error pp-tokensopt new-line
            # pragma pp-tokensopt new-line
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
        //snprintf(line, sizeof line, "%s(%d,%d):\n", input_list->head->pFile->lexeme, input_list->head->line, input_list->head->col);
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
            char* content = find_and_read_include_file(ctx, path + 1, fullpath, &bAlreadyIncluded);
            if (content != NULL)
            {
                struct token_list list = tokenizer(content, fullpath, level + 1, TK_FLAG_NONE, error);
                free(content);

                struct token_list list2 = preprocessor(ctx, &list, level + 1, error);
                token_list_append_list(&r, &list2);
            }
            else
            {
                if (!bAlreadyIncluded)
                {
                    pre_seterror_with_token(ctx, r.tail, "file %s not found", path + 1);
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

            // printf("define %s\n%s : %d\n", input_list->head->lexeme, input_list->head->pFile->lexeme, input_list->head->line);

            struct token* macro_name_token = input_list->head;
            

            if (hashmap_find(&ctx->macros, input_list->head->lexeme) != NULL)
            {
                //printf("warning: '%s' macro redefined at %s %d\n",
                  //     input_list->head->lexeme,
                    ///   input_list->head->pFile->lexeme,
                      // input_list->head->line);
            }


            hashmap_set(&ctx->macros, input_list->head->lexeme, &macro->type_id);
            macro->name = strdup(input_list->head->lexeme);


            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error); //nome da macro
            /*sem skip*/
            //p = preprocessor_match_token(p, is_active, level, false, IDENTIFIER); /*name*/
            if (input_list->head->type == '(')
            {

                macro->bIsFunction = true;


                match_token_level(&r, input_list, '(', level, ctx, error); //nome da macro
                skip_blanks_level(&r, input_list, level);
                if (input_list->head->type == '...')
                {
                    struct macro_parameter* pMacroParameter = calloc(1, sizeof * pMacroParameter);
                    pMacroParameter->name = strdup("__VA_ARGS__");
                    macro->pParameters = pMacroParameter;

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
                        struct macro_parameter* pLast = macro->pParameters;
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
                macro->bIsFunction = false;
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
                    hashmap_set(&ctx->pragmaOnce, input_list->head->pFile->lexeme, (void*)1);
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//pragma
                }
                else if (strcmp(input_list->head->lexeme, "expand") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx, error);//pragma
                    skip_blanks_level(&r, input_list, level);

                    struct macro* macro = find_macro(ctx, input_list->head->lexeme);
                    if (macro)
                    {
                        macro->bExpand = true;
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
        if (!macro->bIsFunction)
        {
            //se nao eh funcao so faz isso e retorna o nome da macro
            return macroArgumentList;
        }

        struct macro_parameter* pCurrentParameter = macro->pParameters;
        int count = 1;
        skip_blanks(&macroArgumentList.tokens, input_list);
        match_token_level(&macroArgumentList.tokens, input_list, '(', level, ctx, error);
        skip_blanks(&macroArgumentList.tokens, input_list);
        if (input_list->head->type == ')')
        {
            if (macro->pParameters != NULL)
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
                    struct token* pNew = calloc(1, sizeof * pNew);
                    pNew->lexeme = s;
                    pNew->type = TK_STRING_LITERAL;
                    pNew->flags = flags;
                    token_list_add(&r, pNew);
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
                    macro->bIsFunction &&
                    !preprocessor_token_ahead_is(newList.head, '('))
                {
                    macro = NULL;
                }

                if (macro && macro_already_expanded(pList, newList.head->lexeme))
                {
                    newList.head->type = TK_IDENTIFIER_RECURSIVE_MACRO;
                    macro = NULL;
                }


                if (ctx->bConditionalInclusion)
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
    bool previousIsBlank = false;
    for (; current;)
    {
        if (current && token_is_blank(current))
        {
            if (current == list->tail)
                break;

            current = current->next;
            continue;
        }
        struct token* pAdded = token_list_clone_and_add(&r, current);
        if (pAdded->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
        {
            pAdded->flags = pAdded->flags & ~TK_FLAG_HAS_NEWLINE_BEFORE;
            pAdded->flags |= TK_FLAG_HAS_SPACE_BEFORE;
        }
        if (bIsFirst)
        {
            pAdded->flags = pAdded->flags & ~TK_FLAG_HAS_SPACE_BEFORE;
            pAdded->flags = pAdded->flags & ~TK_FLAG_HAS_NEWLINE_BEFORE;
            bIsFirst = false;
        }
        remove_line_continuation(pAdded->lexeme);
        previousIsBlank = false;

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

    return copy_replacement_list(&macro->replacementList);
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
        struct macro_expanded macro_expanded = {0};
        macro_expanded.name = macro->name;
        macro_expanded.pPrevious = pList;
        if (macro->bIsFunction)
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
            //assert(start_token->pFile != NULL);

            if (is_active && input_list->head->type == TK_IDENTIFIER)
            {


                macro = find_macro(ctx, input_list->head->lexeme);
                if (macro &&
                    macro->bIsFunction &&
                    !preprocessor_token_ahead_is(input_list->head, '('))
                {
                    macro = NULL;
                }

                if (ctx->bConditionalInclusion)
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
                if (input_list->head->pFile)
                {
                    //char line[1000] = { 0 };
                    //snprintf(line, sizeof line, "%s(%d,%d):\n", input_list->head->pFile->lexeme, input_list->head->line, input_list->head->col);
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


                struct token_list startMacro = expand_macro(ctx, NULL, macro, &arguments, level, error);
                if (startMacro.head)
                {
                    startMacro.head->flags |= flags;
                }

                if (macro->bExpand)
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
                    for (struct token* current = startMacro.head;
                        current != startMacro.tail->next;
                        current = current->next)
                    {
                        current->flags = current->flags & ~TK_FLAG_MACRO_EXPANDED;
                    }
                }

                //seta nos tokens expandidos da onde eles vieram
                token_list_set_file(&startMacro, start_token->pFile, start_token->line, start_token->col);

                token_list_append_list_at_beginning(input_list, &startMacro);

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
                        if (macro && macro->bIsFunction &&
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
                            token_list_set_file(&r3, start_token->pFile, start_token->line, start_token->col);

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

    //token_list_destroy(&l2);

    const char* pre_defined_macros_text =
        "#define __FILE__\n"
        "#define __LINE__\n"
        "#define __COUNT__\n"
        "#define _CONSOLE\n"
        //"#define _MSC_VER 1200\n"        
        "#define _WINDOWS\n"
        "#define _M_IX86\n"
        "#define _X86_\n"
        "#define __fastcall\n"
        "#define __stdcall\n"
        "#define _WIN32\n"
        "#define __cdecl\n"
        "#define __pragma(a)\n"
        "#define __declspec(a)\n"
        "#define __crt_va_start(X) \n"
        "#define __builtin_offsetof(type, member) 0\n"; //como nao defini msver ele pensa que eh gcc aqui

    struct token_list l = tokenizer(pre_defined_macros_text, "standard macros inclusion", 0, TK_FLAG_NONE, error);
    struct token_list l3 = preprocessor(ctx, &l, 0, error);

    //nao quer ver warning de nao usado nestas macros padrao
    mark_macros_as_used(&ctx->macros);
    token_list_destroy(&l);
    token_list_destroy(&l3);
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
    return ss.c_str;
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
    return ss.c_str;
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
    return ss.c_str;
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
    return ss.c_str; //MOVED
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
        assert(current->pFile != NULL);
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
    return ss.c_str;
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


//#pragma once

extern int g_unit_test_error_count;
extern int g_unit_test_success_count;
static void assert_func(int condition, const char* func, const char* file, int line, const char* message)
{
    if (!condition)
    {
        const char* pos = file;
        const char* p = file;
        while (*p)
        {
            if (*p == '/' || *p == '\\')
                pos = p;
            p++;
        }
        
        if (*pos == '/' || *pos == '\\')
            pos++;

        g_unit_test_error_count++;
        printf("\x1b[97m" "%s:%d:0:" "\x1b[91m" " test failed:" "\x1b[0m" " function '%s'\n", pos, line, func);
        
        char buffer[20] = { 0 };
        int n = snprintf(buffer, sizeof buffer, "%d", line);        
        printf(" %s |", buffer);
        printf("    assert(%s);\n", message);
        printf(" %*s |\n", n, " ");
    }
    else
    {
        g_unit_test_success_count++;
        //printf("\x1b[97m" "%s:%d:0" "\x1b[92m" " OK" "\x1b[0m" " at '%s'\n", file, line, func);        
    }
}

#undef assert
#define assert(expression) assert_func(expression, __func__, __FILE__, __LINE__, #expression)




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



void ss_clear(struct osstream* stream)
{
    stream->size = 0;
}

int ss_close(struct osstream* stream)
{
    free(stream->c_str);
    return 0;
}

static int reserve(struct osstream* stream, int size)
{
    int errorcode = 0;
    if (size > stream->capacity)
    {
        void* pnew = realloc(stream->c_str, (size + 1) * sizeof(char));
        if (pnew)
        {
            stream->c_str = pnew;
            stream->capacity = size;
            stream->c_str[size] = 0;
        }
        else
        {
            errno = ENOMEM;
            errorcode = 1;
        }
    }
    return errorcode;
}

int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args)
{
    assert(fmt != 0);
    int size = 0;
    va_list tmpa;

    va_copy(tmpa, args);

    size = vsnprintf(stream->c_str + stream->size, stream->capacity - stream->size, fmt, tmpa);

    va_end(tmpa);

    if (size < 0)
    {
        return -1;
    }

    if (reserve(stream, stream->size + size) != 0)
    {
        return -1;
    }

    size = vsprintf(stream->c_str + stream->size, fmt, args);
    if (size > 0)
    {
        stream->size += size;
    }
    return size;
}

/*
* Returns the character written. For fputc, a return value of EOF indicates an error
*/
int ss_putc(char ch, struct osstream* stream)
{
    if (reserve(stream, stream->size + 1) != 0)
    {
        return EOF;
    }
    stream->c_str[stream->size] = ch;
    stream->size++;

    return ch;
}

int ss_fprintf(struct osstream* stream, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int size = ss_vafprintf(stream, fmt, args);
    va_end(args);
    return size;
}







#ifdef _WIN32

#pragma comment (lib, "Rpcrt4.lib")

#else


#include <uuid/uuid.h>
/*
caso nao tenha este arquivos apt-get install uuid-dev
*/
#endif



#ifdef _WIN32


struct TAGDIR
{
    void* handle;
    struct dirent dirent;
};

//typedef struct TAGDIR DIR;


DIR* opendir(const char* name)
{
    assert(name != 0);
    WIN32_FIND_DATAA fdFile;

    char sPath[MAX_PATH] = { 0 };
    strcat(sPath, name);
    strcat(sPath, "\\*.*");

    HANDLE handle = FindFirstFileA(sPath, &fdFile);

    if (handle != INVALID_HANDLE_VALUE)
    {
        DIR* p = calloc(1, sizeof * p);
        if (p)
        {
            p->handle = handle;
            return p;
        }
        else
        {
            /*calloc sets errno to ENOMEM if a memory allocation fails */
            FindClose(handle);
        }
    }
    else
    {
        errno = windows_error_to_posix(GetLastError());
    }

    return NULL;
}

int closedir(DIR* dirp)
{
    FindClose(dirp->handle);
    free(dirp);
    return 0;
}

struct dirent* readdir(DIR* dirp)
{
    WIN32_FIND_DATAA fdFile;
    BOOL b = FindNextFileA(dirp->handle, &fdFile);
    if (b)
    {
        /*clear*/
        memset(&dirp->dirent, 0, sizeof(dirp->dirent));

        if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            dirp->dirent.d_type |= DT_DIR;
        }

        /*worst case trunks the string*/
        strncpy(dirp->dirent.d_name,
                fdFile.cFileName,
                sizeof(dirp->dirent.d_name) - 1);

        return &dirp->dirent;
    }
    else
    {
        errno = windows_error_to_posix(GetLastError());
    }
    return NULL;
}

#endif //_WINDOWS_

int copy_file(const char* pathfrom,
              const char* pathto)
{
    char buf[4096];
    size_t nread;
    int saved_errno;

    FILE* fd_from = fopen(pathfrom, "rb");
    if (fd_from < 0)
        return -1;

    FILE* fd_to = fopen(pathto, "wb");
    if (fd_to < 0)
        goto out_error;

    while (nread = fread(buf, sizeof(char), sizeof buf, fd_from), nread > 0) //lint !e668  (warning -- possibly passing null pointer to function 'fread(void *, size_t, size_t, FILE *)', arg. no. 4)
    {
        char* out_ptr = buf;
        size_t nwritten;

        do
        {
            nwritten = fwrite(out_ptr, sizeof(char), nread, fd_to);//lint !e668

            nread -= nwritten;
            out_ptr += nwritten;
        }
        while (nread > 0);
    }

    if (nread == 0)
    {
        if (fclose(fd_to) < 0)//lint !e668
        {
            fd_to = NULL;
            goto out_error;
        }
        fclose(fd_from);//lint !e668

        /* Success! */
        return 0;
    }

out_error:
    saved_errno = errno;

    fclose(fd_from); //lint !e668

    if (fd_to != NULL)
        fclose(fd_to);

    return saved_errno;
}

int copy_folder(const char* from, const char* to)
{
    int errcode = mkdir(to, 0700);
    if (errcode != 0)
    {
        return errcode;
    }

    DIR* dir = opendir(from);

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

        char fromlocal[MAX_PATH];
        snprintf(fromlocal, MAX_PATH, "%s/%s", from, dp->d_name);

        char tolocal[MAX_PATH];
        snprintf(tolocal, MAX_PATH, "%s/%s", to, dp->d_name);

        if (dp->d_type & DT_DIR)
        {
            errcode = copy_folder(fromlocal, tolocal);
        }
        else
        {
            errcode = copy_file(fromlocal, tolocal);
        }

        if (errcode != 0)
            break;
    }

    closedir(dir);
    return errcode;
}

#ifdef _WIN32
int get_self_path(char* buffer, int maxsize)
{
    DWORD r = GetModuleFileNameA(NULL, buffer, maxsize);
    return r;
}

#else

int get_self_path(char* buffer, int maxsize)
{

    memset(buffer, 0, maxsize); // readlink does not null terminate!
    if (readlink("/proc/self/exe", buffer, maxsize) == -1)
    {
        //perror("readlink");
        return 1;
    }
    else
    {
        //printf("%s\n", dest);
    }
    return 0;

}
/*


#include <limits.h>

int main()
{
  char dest[PATH_MAX];
  memset(dest,0,sizeof(dest)); // readlink does not null terminate!
  if (readlink("/proc/self/exe", dest, PATH_MAX) == -1) {
    perror("readlink");
  } else {
    printf("%s\n", dest);
  }
  return 0;
}
*/
#endif

#if !defined __EMSCRIPTEN__
char* dirname(char* path)
{
    int last = -1;
    for (int i = 0; path[i]; i++)
    {
        if (path[i] == '\\' || path[i] == '/')
            last = i;
    }

    if (last != -1)
    {
        path[last] = 0;
    }
    return path;
}
#endif

static bool fs_fread2(void* buffer, size_t size, size_t count, FILE* stream, size_t* sz)
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

char* readfile_core(const char* path)
{
    char* result = NULL;

    struct stat info;
    if (stat(path, &info) == 0)
    {
        char* data = (char*)malloc(sizeof(char) * info.st_size + 1);
        if (data != NULL)
        {
            FILE* file = fopen(path, "r");
            if (file != NULL)
            {
                if (info.st_size >= 3)
                {
                    size_t n = 0;
                    if (fs_fread2(data, 1, 3, file, &n))
                    {
                        if (n == 3)
                        {
                            if ((unsigned char) data[0] == (unsigned char)0xEF &&
                                    (unsigned char) data[1] == (unsigned char)0xBB &&
                                    (unsigned char) data[2] == (unsigned char)0xBF)
                            {
                                if (fs_fread2(data, 1, info.st_size - 3, file, &n))
                                {
                                    //ok
                                    data[n] = 0;
                                    result = data;
                                    data = 0;
                                }
                            }
                            else if (fs_fread2(data + 3, 1, info.st_size - 3, file, &n))
                            {
                                data[3 + n] = 0;
                                result = data;
                                data = 0;
                            }
                        }
                        else
                        {
                            data[n] = 0;
                            result = data;
                            data = 0;
                        }
                    }
                }
                else
                {
                    size_t n = 0;
                    if (fs_fread2(data, 1, info.st_size, file, &n))
                    {
                        data[n] = 0;
                        result = data;
                        data = 0;
                    }
                }
                fclose(file);
            }
            free(data);
        }
    }
#ifdef  DEBUG
    const char* p = result;
    while (*p)
    {
        assert(*p != '\r');
        p++;
    }
#endif //  DEBUG

    
    return result;
}


char* readfile(const char* path)
{
#ifdef MOCKFILES
    
    /*
    * .include files are generated by the build. (tool/embed)
    */

    static const unsigned char file_stdio_h[] = {



35,112,114,97,103,109,97,32,111,110,99,101,13,10,116,121,112,101,100,101,102,32,108,111,110
,103,32,108,111,110,103,32,102,112,111,115,95,116,59,13,10,116,121,112,101,100,101,102,32,105
,110,116,32,70,73,76,69,59,13,10,116,121,112,101,100,101,102,32,105,110,116,32,115,105,122
,101,95,116,59,13,10,116,121,112,101,100,101,102,32,118,111,105,100,42,32,118,97,95,108,105
,115,116,59,13,10,105,110,116,32,114,101,109,111,118,101,40,99,111,110,115,116,32,99,104,97
,114,42,32,102,105,108,101,110,97,109,101,41,59,13,10,105,110,116,32,114,101,110,97,109,101
,40,99,111,110,115,116,32,99,104,97,114,42,32,111,108,100,44,32,99,111,110,115,116,32,99
,104,97,114,42,32,110,101,119,115,41,59,13,10,70,73,76,69,42,32,116,109,112,102,105,108
,101,40,118,111,105,100,41,59,13,10,99,104,97,114,42,32,116,109,112,110,97,109,40,99,104
,97,114,42,32,115,41,59,13,10,105,110,116,32,102,99,108,111,115,101,40,70,73,76,69,42
,32,115,116,114,101,97,109,41,59,13,10,105,110,116,32,102,102,108,117,115,104,40,70,73,76
,69,42,32,115,116,114,101,97,109,41,59,13,10,70,73,76,69,42,32,102,111,112,101,110,40
,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,105,108,101
,110,97,109,101,44,32,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99
,116,32,109,111,100,101,41,59,13,10,70,73,76,69,42,32,102,114,101,111,112,101,110,40,99
,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,105,108,101,110
,97,109,101,44,32,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116
,32,109,111,100,101,44,32,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114
,101,97,109,41,59,13,10,118,111,105,100,32,115,101,116,98,117,102,40,70,73,76,69,42,32
,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,44,32,99,104,97,114,42,32,114,101
,115,116,114,105,99,116,32,98,117,102,41,59,13,10,105,110,116,32,115,101,116,118,98,117,102
,40,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,44,32,99
,104,97,114,42,32,114,101,115,116,114,105,99,116,32,98,117,102,44,32,105,110,116,32,109,111
,100,101,44,32,115,105,122,101,95,116,32,115,105,122,101,41,59,13,10,105,110,116,32,102,112
,114,105,110,116,102,40,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101
,97,109,44,32,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32
,102,111,114,109,97,116,44,32,46,46,46,41,59,13,10,105,110,116,32,102,115,99,97,110,102
,40,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,44,32,99
,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109,97
,116,44,32,46,46,46,41,59,13,10,105,110,116,32,112,114,105,110,116,102,40,99,111,110,115
,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32
,46,46,46,41,59,13,10,105,110,116,32,115,99,97,110,102,40,99,111,110,115,116,32,99,104
,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32,46,46,46,41
,59,13,10,105,110,116,32,115,110,112,114,105,110,116,102,40,99,104,97,114,42,32,114,101,115
,116,114,105,99,116,32,115,44,32,115,105,122,101,95,116,32,110,44,32,99,111,110,115,116,32
,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32,46,46
,46,41,59,13,10,105,110,116,32,115,112,114,105,110,116,102,40,99,104,97,114,42,32,114,101
,115,116,114,105,99,116,32,115,44,32,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115
,116,114,105,99,116,32,102,111,114,109,97,116,44,32,46,46,46,41,59,13,10,105,110,116,32
,115,115,99,97,110,102,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105
,99,116,32,115,44,32,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99
,116,32,102,111,114,109,97,116,44,32,46,46,46,41,59,13,10,105,110,116,32,118,102,112,114
,105,110,116,102,40,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97
,109,44,32,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102
,111,114,109,97,116,44,32,118,97,95,108,105,115,116,32,97,114,103,41,59,13,10,105,110,116
,32,118,102,115,99,97,110,102,40,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115
,116,114,101,97,109,44,32,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105
,99,116,32,102,111,114,109,97,116,44,32,118,97,95,108,105,115,116,32,97,114,103,41,59,13
,10,105,110,116,32,118,112,114,105,110,116,102,40,99,111,110,115,116,32,99,104,97,114,42,32
,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32,118,97,95,108,105,115,116,32
,97,114,103,41,59,13,10,105,110,116,32,118,115,99,97,110,102,40,99,111,110,115,116,32,99
,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32,118,97,95
,108,105,115,116,32,97,114,103,41,59,13,10,105,110,116,32,112,117,116,115,40,99,111,110,115
,116,32,99,104,97,114,42,32,115,116,114,41,59,13,10,105,110,116,32,102,112,117,116,115,40
,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,115,44,32,70
,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,41,59,13,10,105
,110,116,32,103,101,116,99,40,70,73,76,69,42,32,115,116,114,101,97,109,41,59,13,10,105
,110,116,32,103,101,116,99,104,97,114,40,118,111,105,100,41,59,13,10,105,110,116,32,112,117
,116,99,40,105,110,116,32,99,44,32,70,73,76,69,42,32,115,116,114,101,97,109,41,59,13
,10,105,110,116,32,112,117,116,99,104,97,114,40,105,110,116,32,99,41,59,13,10,105,110,116
,32,112,117,116,115,40,99,111,110,115,116,32,99,104,97,114,42,32,115,41,59,13,10,105,110
,116,32,117,110,103,101,116,99,40,105,110,116,32,99,44,32,70,73,76,69,42,32,115,116,114
,101,97,109,41,59,13,10,105,110,116,32,102,103,101,116,99,40,70,73,76,69,42,32,115,116
,114,101,97,109,41,59,13,10,115,105,122,101,95,116,32,102,114,101,97,100,40,118,111,105,100
,42,32,114,101,115,116,114,105,99,116,32,112,116,114,44,32,115,105,122,101,95,116,32,115,105
,122,101,44,32,115,105,122,101,95,116,32,110,109,101,109,98,44,32,70,73,76,69,42,32,114
,101,115,116,114,105,99,116,32,115,116,114,101,97,109,41,59,13,10,115,105,122,101,95,116,32
,102,119,114,105,116,101,40,99,111,110,115,116,32,118,111,105,100,42,32,114,101,115,116,114,105
,99,116,32,112,116,114,44,32,115,105,122,101,95,116,32,115,105,122,101,44,32,115,105,122,101
,95,116,32,110,109,101,109,98,44,32,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32
,115,116,114,101,97,109,41,59,13,10,105,110,116,32,102,103,101,116,112,111,115,40,70,73,76
,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,44,32,102,112,111,115,95
,116,42,32,114,101,115,116,114,105,99,116,32,112,111,115,41,59,13,10,105,110,116,32,102,115
,101,101,107,40,70,73,76,69,42,32,115,116,114,101,97,109,44,32,108,111,110,103,32,105,110
,116,32,111,102,102,115,101,116,44,32,105,110,116,32,119,104,101,110,99,101,41,59,13,10,105
,110,116,32,102,115,101,116,112,111,115,40,70,73,76,69,42,32,115,116,114,101,97,109,44,32
,99,111,110,115,116,32,102,112,111,115,95,116,42,32,112,111,115,41,59,13,10,108,111,110,103
,32,105,110,116,32,102,116,101,108,108,40,70,73,76,69,42,32,115,116,114,101,97,109,41,59
,13,10,118,111,105,100,32,114,101,119,105,110,100,40,70,73,76,69,42,32,115,116,114,101,97
,109,41,59,13,10,118,111,105,100,32,99,108,101,97,114,101,114,114,40,70,73,76,69,42,32
,115,116,114,101,97,109,41,59,13,10,105,110,116,32,102,101,111,102,40,70,73,76,69,42,32
,115,116,114,101,97,109,41,59,13,10,105,110,116,32,102,101,114,114,111,114,40,70,73,76,69
,42,32,115,116,114,101,97,109,41,59,13,10,118,111,105,100,32,112,101,114,114,111,114,40,99
,111,110,115,116,32,99,104,97,114,42,32,115,41,59,13,10,13,10,13,10,13,10
    ,0 };

    static const unsigned char file_stdlib_h[] = {



13,10,35,112,114,97,103,109,97,32,111,110,99,101,13,10,116,121,112,101,100,101,102,32,108
,111,110,103,32,108,111,110,103,32,102,112,111,115,95,116,59,13,10,116,121,112,101,100,101,102
,32,105,110,116,32,115,105,122,101,95,116,59,13,10,116,121,112,101,100,101,102,32,105,110,116
,32,119,99,104,97,114,95,116,59,13,10,100,111,117,98,108,101,32,97,116,111,102,40,99,111
,110,115,116,32,99,104,97,114,42,32,110,112,116,114,41,59,13,10,105,110,116,32,97,116,111
,105,40,99,111,110,115,116,32,99,104,97,114,42,32,110,112,116,114,41,59,13,10,108,111,110
,103,32,105,110,116,32,97,116,111,108,40,99,111,110,115,116,32,99,104,97,114,42,32,110,112
,116,114,41,59,13,10,108,111,110,103,32,108,111,110,103,32,105,110,116,32,97,116,111,108,108
,40,99,111,110,115,116,32,99,104,97,114,42,32,110,112,116,114,41,59,13,10,100,111,117,98
,108,101,32,115,116,114,116,111,100,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115
,116,114,105,99,116,32,110,112,116,114,44,32,99,104,97,114,42,42,32,114,101,115,116,114,105
,99,116,32,101,110,100,112,116,114,41,59,13,10,102,108,111,97,116,32,115,116,114,116,111,102
,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,110,112,116
,114,44,32,99,104,97,114,42,42,32,114,101,115,116,114,105,99,116,32,101,110,100,112,116,114
,41,59,13,10,108,111,110,103,32,100,111,117,98,108,101,32,115,116,114,116,111,108,100,40,99
,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,110,112,116,114,44
,32,99,104,97,114,42,42,32,114,101,115,116,114,105,99,116,32,101,110,100,112,116,114,41,59
,13,10,108,111,110,103,32,105,110,116,32,115,116,114,116,111,108,40,99,111,110,115,116,32,99
,104,97,114,42,32,114,101,115,116,114,105,99,116,32,110,112,116,114,44,32,99,104,97,114,42
,42,32,114,101,115,116,114,105,99,116,32,101,110,100,112,116,114,44,32,105,110,116,32,98,97
,115,101,41,59,13,10,108,111,110,103,32,108,111,110,103,32,105,110,116,32,115,116,114,116,111
,108,108,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,110
,112,116,114,44,32,99,104,97,114,42,42,32,114,101,115,116,114,105,99,116,32,101,110,100,112
,116,114,44,32,105,110,116,32,98,97,115,101,41,59,13,10,117,110,115,105,103,110,101,100,32
,108,111,110,103,32,105,110,116,32,115,116,114,116,111,117,108,40,99,111,110,115,116,32,99,104
,97,114,42,32,114,101,115,116,114,105,99,116,32,110,112,116,114,44,32,99,104,97,114,42,42
,32,114,101,115,116,114,105,99,116,32,101,110,100,112,116,114,44,32,105,110,116,32,98,97,115
,101,41,59,13,10,117,110,115,105,103,110,101,100,32,108,111,110,103,32,108,111,110,103,32,105
,110,116,32,115,116,114,116,111,117,108,108,40,99,111,110,115,116,32,99,104,97,114,42,32,114
,101,115,116,114,105,99,116,32,110,112,116,114,44,32,99,104,97,114,42,42,32,114,101,115,116
,114,105,99,116,32,101,110,100,112,116,114,44,32,105,110,116,32,98,97,115,101,41,59,13,10
,105,110,116,32,114,97,110,100,40,118,111,105,100,41,59,13,10,118,111,105,100,32,115,114,97
,110,100,40,117,110,115,105,103,110,101,100,32,105,110,116,32,115,101,101,100,41,59,13,10,118
,111,105,100,42,32,97,108,105,103,110,101,100,95,97,108,108,111,99,40,115,105,122,101,95,116
,32,97,108,105,103,110,109,101,110,116,44,32,115,105,122,101,95,116,32,115,105,122,101,41,59
,13,10,118,111,105,100,42,32,99,97,108,108,111,99,40,115,105,122,101,95,116,32,110,109,101
,109,98,44,32,115,105,122,101,95,116,32,115,105,122,101,41,59,13,10,118,111,105,100,32,102
,114,101,101,40,118,111,105,100,42,32,112,116,114,41,59,13,10,118,111,105,100,42,32,109,97
,108,108,111,99,40,115,105,122,101,95,116,32,115,105,122,101,41,59,13,10,118,111,105,100,42
,32,114,101,97,108,108,111,99,40,118,111,105,100,42,32,112,116,114,44,32,115,105,122,101,95
,116,32,115,105,122,101,41,59,13,10,95,78,111,114,101,116,117,114,110,32,118,111,105,100,32
,97,98,111,114,116,40,118,111,105,100,41,59,13,10,105,110,116,32,97,116,101,120,105,116,40
,118,111,105,100,32,40,42,102,117,110,99,41,40,118,111,105,100,41,41,59,13,10,105,110,116
,32,97,116,95,113,117,105,99,107,95,101,120,105,116,40,118,111,105,100,32,40,42,102,117,110
,99,41,40,118,111,105,100,41,41,59,13,10,95,78,111,114,101,116,117,114,110,32,118,111,105
,100,32,101,120,105,116,40,105,110,116,32,115,116,97,116,117,115,41,59,13,10,95,78,111,114
,101,116,117,114,110,32,118,111,105,100,32,95,69,120,105,116,40,105,110,116,32,115,116,97,116
,117,115,41,59,13,10,99,104,97,114,42,32,103,101,116,101,110,118,40,99,111,110,115,116,32
,99,104,97,114,42,32,110,97,109,101,41,59,13,10,95,78,111,114,101,116,117,114,110,32,118
,111,105,100,32,113,117,105,99,107,95,101,120,105,116,40,105,110,116,32,115,116,97,116,117,115
,41,59,13,10,105,110,116,32,115,121,115,116,101,109,40,99,111,110,115,116,32,99,104,97,114
,42,32,115,116,114,105,110,103,41,59,13,10
    ,0 };

    static const unsigned char file_math_h[] = {



35,112,114,97,103,109,97,32,111,110,99,101,13,10,13,10,100,111,117,98,108,101,32,97,99
,111,115,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,97
,115,105,110,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32
,97,116,97,110,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101
,32,97,116,97,110,50,40,100,111,117,98,108,101,32,95,95,121,44,32,100,111,117,98,108,101
,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,99,111,115,40,100,111,117,98,108,101
,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,115,105,110,40,100,111,117,98,108,101
,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,116,97,110,40,100,111,117,98,108,101
,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,99,111,115,104,40,100,111,117,98,108
,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,115,105,110,104,40,100,111,117,98
,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,116,97,110,104,40,100,111,117
,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,97,99,111,115,104,40,100
,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,97,115,105,110,104
,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,97,116,97
,110,104,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,101
,120,112,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,102
,114,101,120,112,40,100,111,117,98,108,101,32,95,95,120,44,32,105,110,116,42,32,95,95,101
,120,112,111,110,101,110,116,41,59,13,10,100,111,117,98,108,101,32,108,100,101,120,112,40,100
,111,117,98,108,101,32,95,95,120,44,32,105,110,116,32,95,95,101,120,112,111,110,101,110,116
,41,59,13,10,100,111,117,98,108,101,32,108,111,103,40,100,111,117,98,108,101,32,95,95,120
,41,59,13,10,100,111,117,98,108,101,32,108,111,103,49,48,40,100,111,117,98,108,101,32,95
,95,120,41,59,13,10,100,111,117,98,108,101,32,109,111,100,102,40,100,111,117,98,108,101,32
,95,95,120,44,32,100,111,117,98,108,101,42,32,95,95,105,112,116,114,41,59,13,10,100,111
,117,98,108,101,32,101,120,112,109,49,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10
,100,111,117,98,108,101,32,108,111,103,49,112,40,100,111,117,98,108,101,32,95,95,120,41,59
,13,10,100,111,117,98,108,101,32,108,111,103,98,40,100,111,117,98,108,101,32,95,95,120,41
,59,13,10,100,111,117,98,108,101,32,101,120,112,50,40,100,111,117,98,108,101,32,95,95,120
,41,59,13,10,100,111,117,98,108,101,32,108,111,103,50,40,100,111,117,98,108,101,32,95,95
,120,41,59,13,10,100,111,117,98,108,101,32,112,111,119,40,100,111,117,98,108,101,32,95,95
,120,44,32,100,111,117,98,108,101,32,95,95,121,41,59,13,10,100,111,117,98,108,101,32,115
,113,114,116,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32
,104,121,112,111,116,40,100,111,117,98,108,101,32,95,95,120,44,32,100,111,117,98,108,101,32
,95,95,121,41,59,13,10,100,111,117,98,108,101,32,99,98,114,116,40,100,111,117,98,108,101
,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,99,101,105,108,40,100,111,117,98,108
,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,102,97,98,115,40,100,111,117,98
,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,102,108,111,111,114,40,100,111
,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,102,109,111,100,40,100
,111,117,98,108,101,32,95,95,120,44,32,100,111,117,98,108,101,32,95,95,121,41,59,13,10
,105,110,116,32,105,115,105,110,102,40,100,111,117,98,108,101,32,95,95,118,97,108,117,101,41
,59,13,10,105,110,116,32,102,105,110,105,116,101,40,100,111,117,98,108,101,32,95,95,118,97
,108,117,101,41,59,13,10,100,111,117,98,108,101,32,100,114,101,109,40,100,111,117,98,108,101
,32,95,95,120,44,32,100,111,117,98,108,101,32,95,95,121,41,59,13,10,100,111,117,98,108
,101,32,115,105,103,110,105,102,105,99,97,110,100,40,100,111,117,98,108,101,32,95,95,120,41
,59,13,10,100,111,117,98,108,101,32,99,111,112,121,115,105,103,110,40,100,111,117,98,108,101
,32,95,95,120,44,32,100,111,117,98,108,101,32,95,95,121,41,59,13,10,100,111,117,98,108
,101,32,110,97,110,40,99,111,110,115,116,32,99,104,97,114,42,32,95,95,116,97,103,98,41
,59,13,10,105,110,116,32,105,115,110,97,110,40,100,111,117,98,108,101,32,95,95,118,97,108
,117,101,41,59,13,10,100,111,117,98,108,101,32,106,48,40,100,111,117,98,108,101,41,59,13
,10,100,111,117,98,108,101,32,106,49,40,100,111,117,98,108,101,41,59,13,10,100,111,117,98
,108,101,32,106,110,40,105,110,116,44,32,100,111,117,98,108,101,41,59,13,10,100,111,117,98
,108,101,32,121,48,40,100,111,117,98,108,101,41,59,13,10,100,111,117,98,108,101,32,121,49
,40,100,111,117,98,108,101,41,59,13,10,100,111,117,98,108,101,32,121,110,40,105,110,116,44
,32,100,111,117,98,108,101,41,59,13,10,100,111,117,98,108,101,32,101,114,102,40,100,111,117
,98,108,101,41,59,13,10,100,111,117,98,108,101,32,101,114,102,99,40,100,111,117,98,108,101
,41,59,13,10,100,111,117,98,108,101,32,108,103,97,109,109,97,40,100,111,117,98,108,101,41
,59,13,10,100,111,117,98,108,101,32,116,103,97,109,109,97,40,100,111,117,98,108,101,41,59
,13,10,100,111,117,98,108,101,32,103,97,109,109,97,40,100,111,117,98,108,101,41,59,13,10
,100,111,117,98,108,101,32,108,103,97,109,109,97,95,114,40,100,111,117,98,108,101,44,32,105
,110,116,42,32,95,95,115,105,103,110,103,97,109,112,41,59,13,10,100,111,117,98,108,101,32
,114,105,110,116,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101
,32,110,101,120,116,97,102,116,101,114,40,100,111,117,98,108,101,32,95,95,120,44,32,100,111
,117,98,108,101,32,95,95,121,41,59,13,10,100,111,117,98,108,101,32,110,101,120,116,116,111
,119,97,114,100,40,100,111,117,98,108,101,32,95,95,120,44,32,108,111,110,103,32,100,111,117
,98,108,101,32,95,95,121,41,59,13,10,100,111,117,98,108,101,32,114,101,109,97,105,110,100
,101,114,40,100,111,117,98,108,101,32,95,95,120,44,32,100,111,117,98,108,101,32,95,95,121
,41,59,13,10,100,111,117,98,108,101,32,115,99,97,108,98,110,40,100,111,117,98,108,101,32
,95,95,120,44,32,105,110,116,32,95,95,110,41,59,13,10,105,110,116,32,105,108,111,103,98
,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,115,99,97
,108,98,108,110,40,100,111,117,98,108,101,32,95,95,120,44,32,108,111,110,103,32,105,110,116
,32,95,95,110,41,59,13,10,100,111,117,98,108,101,32,110,101,97,114,98,121,105,110,116,40
,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,114,111,117,110
,100,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,116,114
,117,110,99,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32
,114,101,109,113,117,111,40,100,111,117,98,108,101,32,95,95,120,44,32,100,111,117,98,108,101
,32,95,95,121,44,32,105,110,116,42,32,95,95,113,117,111,41,59,13,10,108,111,110,103,32
,105,110,116,32,108,114,105,110,116,40,100,111,117,98,108,101,32,95,95,120,41,59,13,10,108
,111,110,103,32,108,111,110,103,32,105,110,116,32,108,108,114,111,117,110,100,40,100,111,117,98
,108,101,32,95,95,120,41,59,13,10,100,111,117,98,108,101,32,102,100,105,109,40,100,111,117
,98,108,101,32,95,95,120,44,32,100,111,117,98,108,101,32,95,95,121,41,59,13,10,100,111
,117,98,108,101,32,102,109,97,120,40,100,111,117,98,108,101,32,95,95,120,44,32,100,111,117
,98,108,101,32,95,95,121,41,59,13,10,100,111,117,98,108,101,32,102,109,105,110,40,100,111
,117,98,108,101,32,95,95,120,44,32,100,111,117,98,108,101,32,95,95,121,41,59,13,10,100
,111,117,98,108,101,32,102,109,97,40,100,111,117,98,108,101,32,95,95,120,44,32,100,111,117
,98,108,101,32,95,95,121,44,32,100,111,117,98,108,101,32,95,95,122,41,59,13,10,100,111
,117,98,108,101,32,115,99,97,108,98,40,100,111,117,98,108,101,32,95,95,120,44,32,100,111
,117,98,108,101,32,95,95,110,41,59,13,10,102,108,111,97,116,32,97,99,111,115,102,40,102
,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111,97,116,32,97,115,105,110,102,40,102
,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111,97,116,32,97,116,97,110,102,40,102
,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111,97,116,32,97,116,97,110,50,102,40
,102,108,111,97,116,32,95,95,121,44,32,102,108,111,97,116,32,95,95,120,41,59,13,10,102
,108,111,97,116,32,99,111,115,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102,108
,111,97,116,32,115,105,110,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111
,97,116,32,116,97,110,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111,97
,116,32,99,111,115,104,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111,97
,116,32,115,105,110,104,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111,97
,116,32,116,97,110,104,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111,97
,116,32,97,99,111,115,104,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111
,97,116,32,97,115,105,110,104,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102,108
,111,97,116,32,97,116,97,110,104,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102
,108,111,97,116,32,101,120,112,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102,108
,111,97,116,32,102,114,101,120,112,102,40,102,108,111,97,116,32,95,95,120,44,32,105,110,116
,42,32,95,95,101,120,112,111,110,101,110,116,41,59,13,10,102,108,111,97,116,32,108,100,101
,120,112,102,40,102,108,111,97,116,32,95,95,120,44,32,105,110,116,32,95,95,101,120,112,111
,110,101,110,116,41,59,13,10,102,108,111,97,116,32,108,111,103,102,40,102,108,111,97,116,32
,95,95,120,41,59,13,10,102,108,111,97,116,32,108,111,103,49,48,102,40,102,108,111,97,116
,32,95,95,120,41,59,32,102,108,111,97,116,32,95,95,108,111,103,49,48,102,40,102,108,111
,97,116,32,95,95,120,41,59,13,10,102,108,111,97,116,32,109,111,100,102,102,40,102,108,111
,97,116,32,95,95,120,44,32,102,108,111,97,116,42,32,95,95,105,112,116,114,41,59,13,10
,102,108,111,97,116,32,101,120,112,109,49,102,40,102,108,111,97,116,32,95,95,120,41,59,13
,10,102,108,111,97,116,32,108,111,103,49,112,102,40,102,108,111,97,116,32,95,95,120,41,59
,13,10,102,108,111,97,116,32,108,111,103,98,102,40,102,108,111,97,116,32,95,95,120,41,59
,13,10,102,108,111,97,116,32,101,120,112,50,102,40,102,108,111,97,116,32,95,95,120,41,59
,13,10,102,108,111,97,116,32,108,111,103,50,102,40,102,108,111,97,116,32,95,95,120,41,59
,13,10,102,108,111,97,116,32,112,111,119,102,40,102,108,111,97,116,32,95,95,120,44,32,102
,108,111,97,116,32,95,95,121,41,59,13,10,102,108,111,97,116,32,115,113,114,116,102,40,102
,108,111,97,116,32,95,95,120,41,59,13,10,102,108,111,97,116,32,104,121,112,111,116,102,40
,102,108,111,97,116,32,95,95,120,44,32,102,108,111,97,116,32,95,95,121,41,59,13,10,102
,108,111,97,116,32,99,98,114,116,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102
,108,111,97,116,32,99,101,105,108,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102
,108,111,97,116,32,102,97,98,115,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10,102
,108,111,97,116,32,102,108,111,111,114,102,40,102,108,111,97,116,32,95,95,120,41,59,13,10
,102,108,111,97,116,32,102,109,111,100,102,40,102,108,111,97,116,32,95,95,120,44,32,102,108
,111,97,116,32,95,95,121,41,59,32,13,10,105,110,116,32,105,115,105,110,102,102,40,102,108
,111,97,116,32,95,95,118,97,108,117,101,41,59,13,10,105,110,116,32,102,105,110,105,116,101
,102,40,102,108,111,97,116,32,95,95,118,97,108,117,101,41,59,13,10,102,108,111,97,116,32
,100,114,101,109,102,40,102,108,111,97,116,32,95,95,120,44,32,102,108,111,97,116,32,95,95
,121,41,59,13,10,102,108,111,97,116,32,115,105,103,110,105,102,105,99,97,110,100,102,40,102
,108,111,97,116,32,95,95,120,41,59,32,13,10,102,108,111,97,116,32,99,111,112,121,115,105
,103,110,102,40,102,108,111,97,116,32,95,95,120,44,32,102,108,111,97,116,32,95,95,121,41
,59,32,13,10,102,108,111,97,116,32,110,97,110,102,40,99,111,110,115,116,32,99,104,97,114
,42,32,95,95,116,97,103,98,41,59,32,13,10,105,110,116,32,105,115,110,97,110,102,40,102
,108,111,97,116,32,95,95,118,97,108,117,101,41,59,13,10,102,108,111,97,116,32,106,48,102
,40,102,108,111,97,116,41,59,32,13,10,102,108,111,97,116,32,106,49,102,40,102,108,111,97
,116,41,59,32,13,10,102,108,111,97,116,32,106,110,102,40,105,110,116,44,32,102,108,111,97
,116,41,59,32,13,10,102,108,111,97,116,32,121,48,102,40,102,108,111,97,116,41,59,32,13
,10,102,108,111,97,116,32,121,49,102,40,102,108,111,97,116,41,59,32,13,10,102,108,111,97
,116,32,121,110,102,40,105,110,116,44,32,102,108,111,97,116,41,59,32,13,10,102,108,111,97
,116,32,101,114,102,102,40,102,108,111,97,116,41,59,32,13,10,102,108,111,97,116,32,101,114
,102,99,102,40,102,108,111,97,116,41,59,13,10,102,108,111,97,116,32,108,103,97,109,109,97
,102,40,102,108,111,97,116,41,59,13,10,102,108,111,97,116,32,116,103,97,109,109,97,102,40
,102,108,111,97,116,41,59,13,10,102,108,111,97,116,32,103,97,109,109,97,102,40,102,108,111
,97,116,41,59,32,13,10,102,108,111,97,116,32,108,103,97,109,109,97,102,95,114,40,102,108
,111,97,116,44,32,105,110,116,42,32,95,95,115,105,103,110,103,97,109,112,41,59,32,13,10
,102,108,111,97,116,32,114,105,110,116,102,40,102,108,111,97,116,32,95,95,120,41,59,32,13
,10,102,108,111,97,116,32,110,101,120,116,97,102,116,101,114,102,40,102,108,111,97,116,32,95
,95,120,44,32,102,108,111,97,116,32,95,95,121,41,59,32,13,10,102,108,111,97,116,32,110
,101,120,116,116,111,119,97,114,100,102,40,102,108,111,97,116,32,95,95,120,44,32,108,111,110
,103,32,100,111,117,98,108,101,32,95,95,121,41,59,32,13,10,102,108,111,97,116,32,114,101
,109,97,105,110,100,101,114,102,40,102,108,111,97,116,32,95,95,120,44,32,102,108,111,97,116
,32,95,95,121,41,59,32,13,10,102,108,111,97,116,32,115,99,97,108,98,110,102,40,102,108
,111,97,116,32,95,95,120,44,32,105,110,116,32,95,95,110,41,59,32,13,10,105,110,116,32
,105,108,111,103,98,102,40,102,108,111,97,116,32,95,95,120,41,59,32,13,10,102,108,111,97
,116,32,115,99,97,108,98,108,110,102,40,102,108,111,97,116,32,95,95,120,44,32,108,111,110
,103,32,105,110,116,32,95,95,110,41,59,32,13,10,102,108,111,97,116,32,110,101,97,114,98
,121,105,110,116,102,40,102,108,111,97,116,32,95,95,120,41,59,32,13,10,102,108,111,97,116
,32,114,111,117,110,100,102,40,102,108,111,97,116,32,95,95,120,41,59,32,13,10,102,108,111
,97,116,32,116,114,117,110,99,102,40,102,108,111,97,116,32,95,95,120,41,59,32,13,10,102
,108,111,97,116,32,114,101,109,113,117,111,102,40,102,108,111,97,116,32,95,95,120,44,32,102
,108,111,97,116,32,95,95,121,44,32,105,110,116,42,32,95,95,113,117,111,41,59,32,13,10
,108,111,110,103,32,105,110,116,32,108,114,105,110,116,102,40,102,108,111,97,116,32,95,95,120
,41,59,32,13,10,108,111,110,103,32,108,111,110,103,32,105,110,116,32,108,108,114,111,117,110
,100,102,40,102,108,111,97,116,32,95,95,120,41,59,32,13,10,102,108,111,97,116,32,102,100
,105,109,102,40,102,108,111,97,116,32,95,95,120,44,32,102,108,111,97,116,32,95,95,121,41
,59,13,10,102,108,111,97,116,32,102,109,97,120,102,40,102,108,111,97,116,32,95,95,120,44
,32,102,108,111,97,116,32,95,95,121,41,59,32,13,10,102,108,111,97,116,32,102,109,105,110
,102,40,102,108,111,97,116,32,95,95,120,44,32,102,108,111,97,116,32,95,95,121,41,59,13
,10,102,108,111,97,116,32,102,109,97,102,40,102,108,111,97,116,32,95,95,120,44,32,102,108
,111,97,116,32,95,95,121,44,32,102,108,111,97,116,32,95,95,122,41,59,32,13,10,102,108
,111,97,116,32,115,99,97,108,98,102,40,102,108,111,97,116,32,95,95,120,44,32,102,108,111
,97,116,32,95,95,110,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,97,99
,111,115,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108
,111,110,103,32,100,111,117,98,108,101,32,97,115,105,110,108,40,108,111,110,103,32,100,111,117
,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,97
,116,97,110,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10
,108,111,110,103,32,100,111,117,98,108,101,32,97,116,97,110,50,108,40,108,111,110,103,32,100
,111,117,98,108,101,32,95,95,121,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95
,120,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,99,111,115,108,40,108,111
,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100,111
,117,98,108,101,32,115,105,110,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120
,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,116,97,110,108,40,108,111,110
,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100,111,117
,98,108,101,32,99,111,115,104,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120
,41,59,13,10,108,111,110,103,32,100,111,117,98,108,101,32,115,105,110,104,108,40,108,111,110
,103,32,100,111,117,98,108,101,32,95,95,120,41,59,13,10,108,111,110,103,32,100,111,117,98
,108,101,32,116,97,110,104,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41
,59,13,10,108,111,110,103,32,100,111,117,98,108,101,32,97,99,111,115,104,108,40,108,111,110
,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100,111,117
,98,108,101,32,97,115,105,110,104,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95
,120,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,97,116,97,110,104,108,40
,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32
,100,111,117,98,108,101,32,101,120,112,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95
,95,120,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,102,114,101,120,112,108
,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,44,32,105,110,116,42,32,95,95
,101,120,112,111,110,101,110,116,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32
,108,100,101,120,112,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,44,32,105
,110,116,32,95,95,101,120,112,111,110,101,110,116,41,59,32,13,10,108,111,110,103,32,100,111
,117,98,108,101,32,108,111,103,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120
,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,108,111,103,49,48,108,40,108
,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100
,111,117,98,108,101,32,109,111,100,102,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95
,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101,42,32,95,95,105,112,116,114,41,59
,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,101,120,112,109,49,108,40,108,111,110
,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100,111,117
,98,108,101,32,108,111,103,49,112,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95
,120,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,108,111,103,98,108,40,108
,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100
,111,117,98,108,101,32,101,120,112,50,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95
,95,120,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,108,111,103,50,108,40
,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32
,100,111,117,98,108,101,32,112,111,119,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95
,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95,121,41,59,32,13,10,108
,111,110,103,32,100,111,117,98,108,101,32,115,113,114,116,108,40,108,111,110,103,32,100,111,117
,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,104
,121,112,111,116,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,44,32,108,111
,110,103,32,100,111,117,98,108,101,32,95,95,121,41,59,32,13,10,108,111,110,103,32,100,111
,117,98,108,101,32,99,98,114,116,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95
,120,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,99,101,105,108,108,40,108
,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100
,111,117,98,108,101,32,102,97,98,115,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95
,95,120,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,102,108,111,111,114,108
,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,13,10,108,111,110,103,32
,100,111,117,98,108,101,32,102,109,111,100,108,40,108,111,110,103,32,100,111,117,98,108,101,32
,95,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95,121,41,59,32,13,10
,105,110,116,32,105,115,105,110,102,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95
,118,97,108,117,101,41,59,13,10,105,110,116,32,102,105,110,105,116,101,108,40,108,111,110,103
,32,100,111,117,98,108,101,32,95,95,118,97,108,117,101,41,59,13,10,108,111,110,103,32,100
,111,117,98,108,101,32,100,114,101,109,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95
,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95,121,41,59,32,13,10,108
,111,110,103,32,100,111,117,98,108,101,32,115,105,103,110,105,102,105,99,97,110,100,108,40,108
,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100
,111,117,98,108,101,32,99,111,112,121,115,105,103,110,108,40,108,111,110,103,32,100,111,117,98
,108,101,32,95,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95,121,41,59
,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,110,97,110,108,40,99,111,110,115,116
,32,99,104,97,114,42,32,95,95,116,97,103,98,41,59,32,13,10,105,110,116,32,105,115,110
,97,110,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,118,97,108,117,101,41,59
,13,10,108,111,110,103,32,100,111,117,98,108,101,32,106,48,108,40,108,111,110,103,32,100,111
,117,98,108,101,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,106,49,108,40
,108,111,110,103,32,100,111,117,98,108,101,41,59,32,13,10,108,111,110,103,32,100,111,117,98
,108,101,32,106,110,108,40,105,110,116,44,32,108,111,110,103,32,100,111,117,98,108,101,41,59
,13,10,108,111,110,103,32,100,111,117,98,108,101,32,121,48,108,40,108,111,110,103,32,100,111
,117,98,108,101,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,121,49,108,40
,108,111,110,103,32,100,111,117,98,108,101,41,59,32,13,10,108,111,110,103,32,100,111,117,98
,108,101,32,121,110,108,40,105,110,116,44,32,108,111,110,103,32,100,111,117,98,108,101,41,59
,13,10,108,111,110,103,32,100,111,117,98,108,101,32,101,114,102,108,40,108,111,110,103,32,100
,111,117,98,108,101,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,101,114,102
,99,108,40,108,111,110,103,32,100,111,117,98,108,101,41,59,13,10,108,111,110,103,32,100,111
,117,98,108,101,32,108,103,97,109,109,97,108,40,108,111,110,103,32,100,111,117,98,108,101,41
,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,116,103,97,109,109,97,108,40,108
,111,110,103,32,100,111,117,98,108,101,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108
,101,32,103,97,109,109,97,108,40,108,111,110,103,32,100,111,117,98,108,101,41,59,32,13,10
,108,111,110,103,32,100,111,117,98,108,101,32,108,103,97,109,109,97,108,95,114,40,108,111,110
,103,32,100,111,117,98,108,101,44,32,105,110,116,42,32,95,95,115,105,103,110,103,97,109,112
,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,114,105,110,116,108,40,108,111
,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,32,13,10,108,111,110,103,32,100,111
,117,98,108,101,32,110,101,120,116,97,102,116,101,114,108,40,108,111,110,103,32,100,111,117,98
,108,101,32,95,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95,121,41,59
,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,110,101,120,116,116,111,119,97,114,100
,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,44,32,108,111,110,103,32,100
,111,117,98,108,101,32,95,95,121,41,59,13,10,108,111,110,103,32,100,111,117,98,108,101,32
,114,101,109,97,105,110,100,101,114,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95
,120,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95,121,41,59,32,13,10,108,111
,110,103,32,100,111,117,98,108,101,32,115,99,97,108,98,110,108,40,108,111,110,103,32,100,111
,117,98,108,101,32,95,95,120,44,32,105,110,116,32,95,95,110,41,59,32,13,10,105,110,116
,32,105,108,111,103,98,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59
,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,115,99,97,108,98,108,110,108,40,108
,111,110,103,32,100,111,117,98,108,101,32,95,95,120,44,32,108,111,110,103,32,105,110,116,32
,95,95,110,41,59,32,13,10,108,111,110,103,32,100,111,117,98,108,101,32,110,101,97,114,98
,121,105,110,116,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,13,10
,108,111,110,103,32,100,111,117,98,108,101,32,114,111,117,110,100,108,40,108,111,110,103,32,100
,111,117,98,108,101,32,95,95,120,41,59,13,10,108,111,110,103,32,100,111,117,98,108,101,32
,116,114,117,110,99,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,13
,10,108,111,110,103,32,100,111,117,98,108,101,32,114,101,109,113,117,111,108,40,108,111,110,103
,32,100,111,117,98,108,101,32,95,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101,32
,95,95,121,44,32,105,110,116,42,32,95,95,113,117,111,41,59,13,10,108,111,110,103,32,105
,110,116,32,108,114,105,110,116,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120
,41,59,13,10,108,111,110,103,32,108,111,110,103,32,105,110,116,32,108,108,114,111,117,110,100
,108,40,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,41,59,13,10,108,111,110,103
,32,100,111,117,98,108,101,32,102,100,105,109,108,40,108,111,110,103,32,100,111,117,98,108,101
,32,95,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95,121,41,59,13,10
,108,111,110,103,32,100,111,117,98,108,101,32,102,109,97,120,108,40,108,111,110,103,32,100,111
,117,98,108,101,32,95,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95,121
,41,59,13,10,108,111,110,103,32,100,111,117,98,108,101,32,102,109,105,110,108,40,108,111,110
,103,32,100,111,117,98,108,101,32,95,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101
,32,95,95,121,41,59,13,10,108,111,110,103,32,100,111,117,98,108,101,32,102,109,97,108,40
,108,111,110,103,32,100,111,117,98,108,101,32,95,95,120,44,32,108,111,110,103,32,100,111,117
,98,108,101,32,95,95,121,44,32,108,111,110,103,32,100,111,117,98,108,101,32,95,95,122,41
,59,13,10,108,111,110,103,32,100,111,117,98,108,101,32,115,99,97,108,98,108,40,108,111,110
,103,32,100,111,117,98,108,101,32,95,95,120,44,32,108,111,110,103,32,100,111,117,98,108,101
,32,95,95,110,41,59,13,10
    ,0 };

    static const unsigned char file_errno_h[] = {



35,112,114,97,103,109,97,32,111,110,99,101,13,10,13,10,105,110,116,42,32,95,101,114,114
,110,111,40,118,111,105,100,41,59,13,10,35,100,101,102,105,110,101,32,101,114,114,110,111,32
,40,42,95,101,114,114,110,111,40,41,41,13,10,13,10,13,10,35,100,101,102,105,110,101,32
,69,80,69,82,77,32,32,32,32,32,32,32,32,32,32,32,49,13,10,35,100,101,102,105,110
,101,32,69,78,79,69,78,84,32,32,32,32,32,32,32,32,32,32,50,13,10,35,100,101,102
,105,110,101,32,69,83,82,67,72,32,32,32,32,32,32,32,32,32,32,32,51,13,10,35,100
,101,102,105,110,101,32,69,73,78,84,82,32,32,32,32,32,32,32,32,32,32,32,52,13,10
,35,100,101,102,105,110,101,32,69,73,79,32,32,32,32,32,32,32,32,32,32,32,32,32,53
,13,10,35,100,101,102,105,110,101,32,69,78,88,73,79,32,32,32,32,32,32,32,32,32,32
,32,54,13,10,35,100,101,102,105,110,101,32,69,50,66,73,71,32,32,32,32,32,32,32,32
,32,32,32,55,13,10,35,100,101,102,105,110,101,32,69,78,79,69,88,69,67,32,32,32,32
,32,32,32,32,32,56,13,10,35,100,101,102,105,110,101,32,69,66,65,68,70,32,32,32,32
,32,32,32,32,32,32,32,57,13,10,35,100,101,102,105,110,101,32,69,67,72,73,76,68,32
,32,32,32,32,32,32,32,32,32,49,48,13,10,35,100,101,102,105,110,101,32,69,65,71,65
,73,78,32,32,32,32,32,32,32,32,32,32,49,49,13,10,35,100,101,102,105,110,101,32,69
,78,79,77,69,77,32,32,32,32,32,32,32,32,32,32,49,50,13,10,35,100,101,102,105,110
,101,32,69,65,67,67,69,83,32,32,32,32,32,32,32,32,32,32,49,51,13,10,35,100,101
,102,105,110,101,32,69,70,65,85,76,84,32,32,32,32,32,32,32,32,32,32,49,52,13,10
,35,100,101,102,105,110,101,32,69,66,85,83,89,32,32,32,32,32,32,32,32,32,32,32,49
,54,13,10,35,100,101,102,105,110,101,32,69,69,88,73,83,84,32,32,32,32,32,32,32,32
,32,32,49,55,13,10,35,100,101,102,105,110,101,32,69,88,68,69,86,32,32,32,32,32,32
,32,32,32,32,32,49,56,13,10,35,100,101,102,105,110,101,32,69,78,79,68,69,86,32,32
,32,32,32,32,32,32,32,32,49,57,13,10,35,100,101,102,105,110,101,32,69,78,79,84,68
,73,82,32,32,32,32,32,32,32,32,32,50,48,13,10,35,100,101,102,105,110,101,32,69,73
,83,68,73,82,32,32,32,32,32,32,32,32,32,32,50,49,13,10,35,100,101,102,105,110,101
,32,69,78,70,73,76,69,32,32,32,32,32,32,32,32,32,32,50,51,13,10,35,100,101,102
,105,110,101,32,69,77,70,73,76,69,32,32,32,32,32,32,32,32,32,32,50,52,13,10,35
,100,101,102,105,110,101,32,69,78,79,84,84,89,32,32,32,32,32,32,32,32,32,32,50,53
,13,10,35,100,101,102,105,110,101,32,69,70,66,73,71,32,32,32,32,32,32,32,32,32,32
,32,50,55,13,10,35,100,101,102,105,110,101,32,69,78,79,83,80,67,32,32,32,32,32,32
,32,32,32,32,50,56,13,10,35,100,101,102,105,110,101,32,69,83,80,73,80,69,32,32,32
,32,32,32,32,32,32,32,50,57,13,10,35,100,101,102,105,110,101,32,69,82,79,70,83,32
,32,32,32,32,32,32,32,32,32,32,51,48,13,10,35,100,101,102,105,110,101,32,69,77,76
,73,78,75,32,32,32,32,32,32,32,32,32,32,51,49,13,10,35,100,101,102,105,110,101,32
,69,80,73,80,69,32,32,32,32,32,32,32,32,32,32,32,51,50,13,10,35,100,101,102,105
,110,101,32,69,68,79,77,32,32,32,32,32,32,32,32,32,32,32,32,51,51,13,10,35,100
,101,102,105,110,101,32,69,68,69,65,68,76,75,32,32,32,32,32,32,32,32,32,51,54,13
,10,35,100,101,102,105,110,101,32,69,78,65,77,69,84,79,79,76,79,78,71,32,32,32,32
,51,56,13,10,35,100,101,102,105,110,101,32,69,78,79,76,67,75,32,32,32,32,32,32,32
,32,32,32,51,57,13,10,35,100,101,102,105,110,101,32,69,78,79,83,89,83,32,32,32,32
,32,32,32,32,32,32,52,48,13,10,35,100,101,102,105,110,101,32,69,78,79,84,69,77,80
,84,89,32,32,32,32,32,32,32,52,49,13,10,13,10,13,10,47,47,32,83,117,112,112,111
,114,116,32,69,68,69,65,68,76,79,67,75,32,102,111,114,32,99,111,109,112,97,116,105,98
,105,108,105,116,121,32,119,105,116,104,32,111,108,100,101,114,32,77,105,99,114,111,115,111,102
,116,32,67,32,118,101,114,115,105,111,110,115,13,10,35,100,101,102,105,110,101,32,69,68,69
,65,68,76,79,67,75,32,32,32,32,32,32,32,69,68,69,65,68,76,75,13,10,13,10,35
,100,101,102,105,110,101,32,69,65,68,68,82,73,78,85,83,69,32,32,32,32,32,32,49,48
,48,13,10,35,100,101,102,105,110,101,32,69,65,68,68,82,78,79,84,65,86,65,73,76,32
,32,32,49,48,49,13,10,35,100,101,102,105,110,101,32,69,65,70,78,79,83,85,80,80,79
,82,84,32,32,32,32,49,48,50,13,10,35,100,101,102,105,110,101,32,69,65,76,82,69,65
,68,89,32,32,32,32,32,32,32,32,49,48,51,13,10,35,100,101,102,105,110,101,32,69,66
,65,68,77,83,71,32,32,32,32,32,32,32,32,32,49,48,52,13,10,35,100,101,102,105,110
,101,32,69,67,65,78,67,69,76,69,68,32,32,32,32,32,32,32,49,48,53,13,10,35,100
,101,102,105,110,101,32,69,67,79,78,78,65,66,79,82,84,69,68,32,32,32,32,49,48,54
,13,10,35,100,101,102,105,110,101,32,69,67,79,78,78,82,69,70,85,83,69,68,32,32,32
,32,49,48,55,13,10,35,100,101,102,105,110,101,32,69,67,79,78,78,82,69,83,69,84,32
,32,32,32,32,32,49,48,56,13,10,35,100,101,102,105,110,101,32,69,68,69,83,84,65,68
,68,82,82,69,81,32,32,32,32,49,48,57,13,10,35,100,101,102,105,110,101,32,69,72,79
,83,84,85,78,82,69,65,67,72,32,32,32,32,49,49,48,13,10,35,100,101,102,105,110,101
,32,69,73,68,82,77,32,32,32,32,32,32,32,32,32,32,32,49,49,49,13,10,35,100,101
,102,105,110,101,32,69,73,78,80,82,79,71,82,69,83,83,32,32,32,32,32,49,49,50,13
,10,35,100,101,102,105,110,101,32,69,73,83,67,79,78,78,32,32,32,32,32,32,32,32,32
,49,49,51,13,10,35,100,101,102,105,110,101,32,69,76,79,79,80,32,32,32,32,32,32,32
,32,32,32,32,49,49,52,13,10,35,100,101,102,105,110,101,32,69,77,83,71,83,73,90,69
,32,32,32,32,32,32,32,32,49,49,53,13,10,35,100,101,102,105,110,101,32,69,78,69,84
,68,79,87,78,32,32,32,32,32,32,32,32,49,49,54,13,10,35,100,101,102,105,110,101,32
,69,78,69,84,82,69,83,69,84,32,32,32,32,32,32,32,49,49,55,13,10,35,100,101,102
,105,110,101,32,69,78,69,84,85,78,82,69,65,67,72,32,32,32,32,32,49,49,56,13,10
,35,100,101,102,105,110,101,32,69,78,79,66,85,70,83,32,32,32,32,32,32,32,32,32,49
,49,57,13,10,35,100,101,102,105,110,101,32,69,78,79,68,65,84,65,32,32,32,32,32,32
,32,32,32,49,50,48,13,10,35,100,101,102,105,110,101,32,69,78,79,76,73,78,75,32,32
,32,32,32,32,32,32,32,49,50,49,13,10,35,100,101,102,105,110,101,32,69,78,79,77,83
,71,32,32,32,32,32,32,32,32,32,32,49,50,50,13,10,35,100,101,102,105,110,101,32,69
,78,79,80,82,79,84,79,79,80,84,32,32,32,32,32,49,50,51,13,10,35,100,101,102,105
,110,101,32,69,78,79,83,82,32,32,32,32,32,32,32,32,32,32,32,49,50,52,13,10,35
,100,101,102,105,110,101,32,69,78,79,83,84,82,32,32,32,32,32,32,32,32,32,32,49,50
,53,13,10,35,100,101,102,105,110,101,32,69,78,79,84,67,79,78,78,32,32,32,32,32,32
,32,32,49,50,54,13,10,35,100,101,102,105,110,101,32,69,78,79,84,82,69,67,79,86,69
,82,65,66,76,69,32,49,50,55,13,10,35,100,101,102,105,110,101,32,69,78,79,84,83,79
,67,75,32,32,32,32,32,32,32,32,49,50,56,13,10,35,100,101,102,105,110,101,32,69,78
,79,84,83,85,80,32,32,32,32,32,32,32,32,32,49,50,57,13,10,35,100,101,102,105,110
,101,32,69,79,80,78,79,84,83,85,80,80,32,32,32,32,32,32,49,51,48,13,10,35,100
,101,102,105,110,101,32,69,79,84,72,69,82,32,32,32,32,32,32,32,32,32,32,49,51,49
,13,10,35,100,101,102,105,110,101,32,69,79,86,69,82,70,76,79,87,32,32,32,32,32,32
,32,49,51,50,13,10,35,100,101,102,105,110,101,32,69,79,87,78,69,82,68,69,65,68,32
,32,32,32,32,32,49,51,51,13,10,35,100,101,102,105,110,101,32,69,80,82,79,84,79,32
,32,32,32,32,32,32,32,32,32,49,51,52,13,10,35,100,101,102,105,110,101,32,69,80,82
,79,84,79,78,79,83,85,80,80,79,82,84,32,49,51,53,13,10,35,100,101,102,105,110,101
,32,69,80,82,79,84,79,84,89,80,69,32,32,32,32,32,32,49,51,54,13,10,35,100,101
,102,105,110,101,32,69,84,73,77,69,32,32,32,32,32,32,32,32,32,32,32,49,51,55,13
,10,35,100,101,102,105,110,101,32,69,84,73,77,69,68,79,85,84,32,32,32,32,32,32,32
,49,51,56,13,10,35,100,101,102,105,110,101,32,69,84,88,84,66,83,89,32,32,32,32,32
,32,32,32,32,49,51,57,13,10,35,100,101,102,105,110,101,32,69,87,79,85,76,68,66,76
,79,67,75,32,32,32,32,32,49,52,48,13,10,13,10
    ,0 };

    static const unsigned char file_string_h[] = {



13,10,116,121,112,101,100,101,102,32,105,110,116,32,101,114,114,110,111,95,116,59,13,10,116
,121,112,101,100,101,102,32,117,110,115,105,103,110,101,100,32,108,111,110,103,32,108,111,110,103
,32,115,105,122,101,95,116,59,13,10,116,121,112,101,100,101,102,32,117,110,115,105,103,110,101
,100,32,108,111,110,103,32,108,111,110,103,32,114,115,105,122,101,95,116,59,13,10,116,121,112
,101,100,101,102,32,117,110,115,105,103,110,101,100,32,115,104,111,114,116,32,119,99,104,97,114
,95,116,59,13,10,13,10,13,10,118,111,105,100,42,32,109,101,109,99,104,114,40,118,111,105
,100,32,99,111,110,115,116,42,32,95,66,117,102,44,32,105,110,116,32,95,86,97,108,44,32
,115,105,122,101,95,116,32,95,77,97,120,67,111,117,110,116,41,59,13,10,105,110,116,32,109
,101,109,99,109,112,40,118,111,105,100,32,99,111,110,115,116,42,32,95,66,117,102,49,44,32
,118,111,105,100,32,99,111,110,115,116,42,32,95,66,117,102,50,44,32,115,105,122,101,95,116
,32,95,83,105,122,101,41,59,13,10,118,111,105,100,42,32,109,101,109,99,112,121,40,118,111
,105,100,42,32,95,68,115,116,44,32,118,111,105,100,32,99,111,110,115,116,42,32,95,83,114
,99,44,32,115,105,122,101,95,116,32,95,83,105,122,101,41,59,13,10,118,111,105,100,42,32
,109,101,109,109,111,118,101,40,118,111,105,100,42,32,95,68,115,116,44,32,118,111,105,100,32
,99,111,110,115,116,42,32,95,83,114,99,44,32,115,105,122,101,95,116,32,95,83,105,122,101
,41,59,13,10,118,111,105,100,42,32,109,101,109,115,101,116,40,118,111,105,100,42,32,95,68
,115,116,44,32,105,110,116,32,95,86,97,108,44,32,115,105,122,101,95,116,32,95,83,105,122
,101,41,59,13,10,99,104,97,114,42,32,115,116,114,99,104,114,40,99,104,97,114,32,99,111
,110,115,116,42,32,95,83,116,114,44,32,105,110,116,32,95,86,97,108,41,59,13,10,99,104
,97,114,42,32,115,116,114,114,99,104,114,40,99,104,97,114,32,99,111,110,115,116,42,32,95
,83,116,114,44,32,105,110,116,32,95,67,104,41,59,13,10,99,104,97,114,42,32,115,116,114
,115,116,114,40,99,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114,44,32,99,104,97
,114,32,99,111,110,115,116,42,32,95,83,117,98,83,116,114,41,59,13,10,119,99,104,97,114
,95,116,42,32,119,99,115,99,104,114,40,119,99,104,97,114,95,116,32,99,111,110,115,116,42
,32,95,83,116,114,44,32,119,99,104,97,114,95,116,32,95,67,104,41,59,13,10,119,99,104
,97,114,95,116,42,32,119,99,115,114,99,104,114,40,119,99,104,97,114,95,116,32,99,111,110
,115,116,42,32,95,83,116,114,44,32,119,99,104,97,114,95,116,32,95,67,104,41,59,13,10
,119,99,104,97,114,95,116,42,32,119,99,115,115,116,114,40,119,99,104,97,114,95,116,32,99
,111,110,115,116,42,32,95,83,116,114,44,32,119,99,104,97,114,95,116,32,99,111,110,115,116
,42,32,95,83,117,98,83,116,114,41,59,13,10,115,116,97,116,105,99,32,95,95,105,110,108
,105,110,101,32,101,114,114,110,111,95,116,32,109,101,109,99,112,121,95,115,40,118,111,105,100
,42,32,99,111,110,115,116,32,95,68,101,115,116,105,110,97,116,105,111,110,44,32,114,115,105
,122,101,95,116,32,99,111,110,115,116,32,95,68,101,115,116,105,110,97,116,105,111,110,83,105
,122,101,44,32,118,111,105,100,32,99,111,110,115,116,42,32,99,111,110,115,116,32,95,83,111
,117,114,99,101,44,32,114,115,105,122,101,95,116,32,99,111,110,115,116,32,95,83,111,117,114
,99,101,83,105,122,101,41,59,13,10,115,116,97,116,105,99,32,95,95,105,110,108,105,110,101
,32,101,114,114,110,111,95,116,32,109,101,109,109,111,118,101,95,115,40,118,111,105,100,42,32
,99,111,110,115,116,32,95,68,101,115,116,105,110,97,116,105,111,110,44,32,114,115,105,122,101
,95,116,32,99,111,110,115,116,32,95,68,101,115,116,105,110,97,116,105,111,110,83,105,122,101
,44,32,118,111,105,100,32,99,111,110,115,116,42,32,99,111,110,115,116,32,95,83,111,117,114
,99,101,44,32,114,115,105,122,101,95,116,32,99,111,110,115,116,32,95,83,111,117,114,99,101
,83,105,122,101,41,59,13,10,105,110,116,32,95,109,101,109,105,99,109,112,40,118,111,105,100
,32,99,111,110,115,116,42,32,95,66,117,102,49,44,32,118,111,105,100,32,99,111,110,115,116
,42,32,95,66,117,102,50,44,32,115,105,122,101,95,116,32,95,83,105,122,101,41,59,13,10
,118,111,105,100,42,32,109,101,109,99,99,112,121,40,118,111,105,100,42,32,95,68,115,116,44
,32,118,111,105,100,32,99,111,110,115,116,42,32,95,83,114,99,44,32,105,110,116,32,95,86
,97,108,44,32,115,105,122,101,95,116,32,95,83,105,122,101,41,59,13,10,105,110,116,32,109
,101,109,105,99,109,112,40,118,111,105,100,32,99,111,110,115,116,42,32,95,66,117,102,49,44
,32,118,111,105,100,32,99,111,110,115,116,42,32,95,66,117,102,50,44,32,115,105,122,101,95
,116,32,95,83,105,122,101,41,59,13,10,101,114,114,110,111,95,116,32,119,99,115,99,97,116
,95,115,40,119,99,104,97,114,95,116,42,32,95,68,101,115,116,105,110,97,116,105,111,110,44
,32,114,115,105,122,101,95,116,32,95,83,105,122,101,73,110,87,111,114,100,115,44,32,119,99
,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,111,117,114,99,101,41,59,13,10,101
,114,114,110,111,95,116,32,119,99,115,99,112,121,95,115,40,119,99,104,97,114,95,116,42,32
,95,68,101,115,116,105,110,97,116,105,111,110,44,32,114,115,105,122,101,95,116,32,95,83,105
,122,101,73,110,87,111,114,100,115,44,32,119,99,104,97,114,95,116,32,99,111,110,115,116,42
,32,95,83,111,117,114,99,101,41,59,13,10,101,114,114,110,111,95,116,32,119,99,115,110,99
,97,116,95,115,40,119,99,104,97,114,95,116,42,32,95,68,101,115,116,105,110,97,116,105,111
,110,44,32,114,115,105,122,101,95,116,32,95,83,105,122,101,73,110,87,111,114,100,115,44,32
,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,111,117,114,99,101,44,32,114
,115,105,122,101,95,116,32,95,77,97,120,67,111,117,110,116,41,59,13,10,101,114,114,110,111
,95,116,32,119,99,115,110,99,112,121,95,115,40,119,99,104,97,114,95,116,42,32,95,68,101
,115,116,105,110,97,116,105,111,110,44,32,114,115,105,122,101,95,116,32,95,83,105,122,101,73
,110,87,111,114,100,115,44,32,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83
,111,117,114,99,101,44,32,114,115,105,122,101,95,116,32,95,77,97,120,67,111,117,110,116,41
,59,13,10,119,99,104,97,114,95,116,42,32,119,99,115,116,111,107,95,115,40,119,99,104,97
,114,95,116,42,32,95,83,116,114,105,110,103,44,32,119,99,104,97,114,95,116,32,99,111,110
,115,116,42,32,95,68,101,108,105,109,105,116,101,114,44,32,119,99,104,97,114,95,116,42,42
,32,95,67,111,110,116,101,120,116,41,59,13,10,119,99,104,97,114,95,116,42,32,95,119,99
,115,100,117,112,40,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116,114,105
,110,103,41,59,13,10,119,99,104,97,114,95,116,42,32,119,99,115,99,97,116,40,119,99,104
,97,114,95,116,42,32,95,68,101,115,116,105,110,97,116,105,111,110,44,32,119,99,104,97,114
,95,116,32,99,111,110,115,116,42,32,95,83,111,117,114,99,101,41,59,32,105,110,116,32,119
,99,115,99,109,112,40,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116,114
,105,110,103,49,44,32,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116,114
,105,110,103,50,41,59,13,10,119,99,104,97,114,95,116,42,32,119,99,115,99,112,121,40,119
,99,104,97,114,95,116,42,32,95,68,101,115,116,105,110,97,116,105,111,110,44,32,119,99,104
,97,114,95,116,32,99,111,110,115,116,42,32,95,83,111,117,114,99,101,41,59,32,115,105,122
,101,95,116,32,119,99,115,99,115,112,110,40,119,99,104,97,114,95,116,32,99,111,110,115,116
,42,32,95,83,116,114,105,110,103,44,32,119,99,104,97,114,95,116,32,99,111,110,115,116,42
,32,95,67,111,110,116,114,111,108,41,59,13,10,115,105,122,101,95,116,32,119,99,115,108,101
,110,40,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,41
,59,13,10,115,105,122,101,95,116,32,119,99,115,110,108,101,110,40,119,99,104,97,114,95,116
,32,99,111,110,115,116,42,32,95,83,111,117,114,99,101,44,32,115,105,122,101,95,116,32,95
,77,97,120,67,111,117,110,116,41,59,13,10,115,116,97,116,105,99,32,95,95,105,110,108,105
,110,101,32,115,105,122,101,95,116,32,119,99,115,110,108,101,110,95,115,40,119,99,104,97,114
,95,116,32,99,111,110,115,116,42,32,95,83,111,117,114,99,101,44,32,115,105,122,101,95,116
,32,95,77,97,120,67,111,117,110,116,41,59,13,10,119,99,104,97,114,95,116,42,32,119,99
,115,110,99,97,116,40,119,99,104,97,114,95,116,42,32,95,68,101,115,116,105,110,97,116,105
,111,110,44,32,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,111,117,114,99
,101,44,32,115,105,122,101,95,116,32,95,67,111,117,110,116,41,59,13,10,105,110,116,32,119
,99,115,110,99,109,112,40,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116
,114,105,110,103,49,44,32,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116
,114,105,110,103,50,44,32,115,105,122,101,95,116,32,95,77,97,120,67,111,117,110,116,41,59
,13,10,119,99,104,97,114,95,116,42,32,119,99,115,110,99,112,121,40,119,99,104,97,114,95
,116,42,32,95,68,101,115,116,105,110,97,116,105,111,110,44,32,119,99,104,97,114,95,116,32
,99,111,110,115,116,42,32,95,83,111,117,114,99,101,44,32,115,105,122,101,95,116,32,95,67
,111,117,110,116,41,59,13,10,119,99,104,97,114,95,116,42,32,119,99,115,112,98,114,107,40
,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,44,32,119
,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,67,111,110,116,114,111,108,41,59,13
,10,115,105,122,101,95,116,32,119,99,115,115,112,110,40,119,99,104,97,114,95,116,32,99,111
,110,115,116,42,32,95,83,116,114,105,110,103,44,32,119,99,104,97,114,95,116,32,99,111,110
,115,116,42,32,95,67,111,110,116,114,111,108,41,59,13,10,119,99,104,97,114,95,116,42,32
,119,99,115,116,111,107,40,119,99,104,97,114,95,116,42,32,95,83,116,114,105,110,103,44,32
,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,68,101,108,105,109,105,116,101,114
,44,32,119,99,104,97,114,95,116,42,42,32,95,67,111,110,116,101,120,116,41,59,13,10,115
,105,122,101,95,116,32,119,99,115,120,102,114,109,40,119,99,104,97,114,95,116,42,32,95,68
,101,115,116,105,110,97,116,105,111,110,44,32,119,99,104,97,114,95,116,32,99,111,110,115,116
,42,32,95,83,111,117,114,99,101,44,32,115,105,122,101,95,116,32,95,77,97,120,67,111,117
,110,116,41,59,13,10,105,110,116,32,119,99,115,99,111,108,108,40,119,99,104,97,114,95,116
,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,49,44,32,119,99,104,97,114,95,116
,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,50,41,59,13,10,119,99,104,97,114
,95,116,42,32,119,99,115,100,117,112,40,119,99,104,97,114,95,116,32,99,111,110,115,116,42
,32,95,83,116,114,105,110,103,41,59,13,10,105,110,116,32,119,99,115,105,99,109,112,40,119
,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,49,44,32,119
,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,50,41,59,13
,10,105,110,116,32,119,99,115,110,105,99,109,112,40,119,99,104,97,114,95,116,32,99,111,110
,115,116,42,32,95,83,116,114,105,110,103,49,44,32,119,99,104,97,114,95,116,32,99,111,110
,115,116,42,32,95,83,116,114,105,110,103,50,44,32,115,105,122,101,95,116,32,95,77,97,120
,67,111,117,110,116,41,59,13,10,119,99,104,97,114,95,116,42,32,119,99,115,110,115,101,116
,40,119,99,104,97,114,95,116,42,32,95,83,116,114,105,110,103,44,32,119,99,104,97,114,95
,116,32,95,86,97,108,117,101,44,32,115,105,122,101,95,116,32,95,77,97,120,67,111,117,110
,116,41,59,13,10,119,99,104,97,114,95,116,42,32,119,99,115,114,101,118,40,119,99,104,97
,114,95,116,42,32,95,83,116,114,105,110,103,41,59,13,10,119,99,104,97,114,95,116,42,32
,119,99,115,115,101,116,40,119,99,104,97,114,95,116,42,32,95,83,116,114,105,110,103,44,32
,119,99,104,97,114,95,116,32,95,86,97,108,117,101,41,59,13,10,119,99,104,97,114,95,116
,42,32,119,99,115,108,119,114,40,119,99,104,97,114,95,116,42,32,95,83,116,114,105,110,103
,41,59,32,119,99,104,97,114,95,116,42,32,119,99,115,117,112,114,40,119,99,104,97,114,95
,116,42,32,95,83,116,114,105,110,103,41,59,32,13,10,105,110,116,32,119,99,115,105,99,111
,108,108,40,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103
,49,44,32,119,99,104,97,114,95,116,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103
,50,41,59,13,10,99,104,97,114,42,32,115,116,114,116,111,107,95,115,40,99,104,97,114,42
,32,95,83,116,114,105,110,103,44,32,99,104,97,114,32,99,111,110,115,116,42,32,95,68,101
,108,105,109,105,116,101,114,44,32,99,104,97,114,42,42,32,95,67,111,110,116,101,120,116,41
,59,32,13,10,118,111,105,100,42,32,95,109,101,109,99,99,112,121,40,118,111,105,100,42,32
,95,68,115,116,44,32,118,111,105,100,32,99,111,110,115,116,42,32,95,83,114,99,44,32,105
,110,116,32,95,86,97,108,44,32,115,105,122,101,95,116,32,95,77,97,120,67,111,117,110,116
,41,59,13,10,99,104,97,114,42,32,115,116,114,99,97,116,40,99,104,97,114,42,32,95,68
,101,115,116,105,110,97,116,105,111,110,44,32,99,104,97,114,32,99,111,110,115,116,42,32,95
,83,111,117,114,99,101,41,59,13,10,105,110,116,32,115,116,114,99,109,112,40,99,104,97,114
,32,99,111,110,115,116,42,32,95,83,116,114,49,44,32,99,104,97,114,32,99,111,110,115,116
,42,32,95,83,116,114,50,41,59,13,10,13,10,105,110,116,32,115,116,114,99,111,108,108,40
,99,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,49,44,32,99,104,97
,114,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,50,41,59,13,10,99,104,97,114
,42,32,115,116,114,101,114,114,111,114,40,105,110,116,32,95,69,114,114,111,114,77,101,115,115
,97,103,101,41,59,13,10,13,10,115,105,122,101,95,116,32,115,116,114,108,101,110,40,99,104
,97,114,32,99,111,110,115,116,42,32,95,83,116,114,41,59,13,10,13,10,99,104,97,114,42
,32,115,116,114,110,99,97,116,40,99,104,97,114,42,32,95,68,101,115,116,105,110,97,116,105
,111,110,44,32,99,104,97,114,32,99,111,110,115,116,42,32,95,83,111,117,114,99,101,44,32
,115,105,122,101,95,116,32,95,67,111,117,110,116,41,59,13,10,105,110,116,32,115,116,114,110
,99,109,112,40,99,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114,49,44,32,99,104
,97,114,32,99,111,110,115,116,42,32,95,83,116,114,50,44,32,115,105,122,101,95,116,32,95
,77,97,120,67,111,117,110,116,41,59,13,10,99,104,97,114,42,32,115,116,114,110,99,112,121
,40,99,104,97,114,42,32,95,68,101,115,116,105,110,97,116,105,111,110,44,32,99,104,97,114
,32,99,111,110,115,116,42,32,95,83,111,117,114,99,101,44,32,115,105,122,101,95,116,32,95
,67,111,117,110,116,41,59,13,10,115,105,122,101,95,116,32,115,116,114,110,108,101,110,40,99
,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,44,32,115,105,122,101,95
,116,32,95,77,97,120,67,111,117,110,116,41,59,13,10,115,116,97,116,105,99,32,95,95,105
,110,108,105,110,101,32,115,105,122,101,95,116,32,115,116,114,110,108,101,110,95,115,40,99,104
,97,114,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,44,32,115,105,122,101,95,116
,32,95,77,97,120,67,111,117,110,116,41,59,13,10,99,104,97,114,42,32,115,116,114,112,98
,114,107,40,99,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114,44,32,99,104,97,114
,32,99,111,110,115,116,42,32,95,67,111,110,116,114,111,108,41,59,13,10,115,105,122,101,95
,116,32,115,116,114,115,112,110,40,99,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114
,44,32,99,104,97,114,32,99,111,110,115,116,42,32,95,67,111,110,116,114,111,108,41,59,13
,10,99,104,97,114,42,32,115,116,114,116,111,107,40,99,104,97,114,42,32,95,83,116,114,105
,110,103,44,32,99,104,97,114,32,99,111,110,115,116,42,32,95,68,101,108,105,109,105,116,101
,114,41,59,32,13,10,99,104,97,114,42,32,115,116,114,100,117,112,40,99,104,97,114,32,99
,111,110,115,116,42,32,95,83,116,114,105,110,103,41,59,13,10,105,110,116,32,115,116,114,99
,109,112,105,40,99,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,49,44
,32,99,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,50,41,59,13,10
,105,110,116,32,115,116,114,105,99,109,112,40,99,104,97,114,32,99,111,110,115,116,42,32,95
,83,116,114,105,110,103,49,44,32,99,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114
,105,110,103,50,41,59,13,10,99,104,97,114,42,32,115,116,114,108,119,114,40,99,104,97,114
,42,32,95,83,116,114,105,110,103,41,59,13,10,105,110,116,32,115,116,114,110,105,99,109,112
,40,99,104,97,114,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,49,44,32,99,104
,97,114,32,99,111,110,115,116,42,32,95,83,116,114,105,110,103,50,44,32,115,105,122,101,95
,116,32,95,77,97,120,67,111,117,110,116,41,59,13,10,99,104,97,114,42,32,115,116,114,110
,115,101,116,40,99,104,97,114,42,32,95,83,116,114,105,110,103,44,32,105,110,116,32,95,86
,97,108,117,101,44,32,115,105,122,101,95,116,32,95,77,97,120,67,111,117,110,116,41,59,13
,10,99,104,97,114,42,32,115,116,114,114,101,118,40,99,104,97,114,42,32,95,83,116,114,105
,110,103,41,59,13,10,99,104,97,114,42,32,115,116,114,115,101,116,40,99,104,97,114,42,32
,95,83,116,114,105,110,103,44,32,105,110,116,32,95,86,97,108,117,101,41,59,32,99,104,97
,114,42,32,115,116,114,117,112,114,40,99,104,97,114,42,32,95,83,116,114,105,110,103,41,59
,13,10,13,10
    ,0 };


    if (strcmp(path, "stdio.h") == 0)
        return strdup(file_stdio_h);
    else if (strcmp(path, "stdlib.h") == 0)
        return strdup(file_stdlib_h);
    else if (strcmp(path, "math.h") == 0)
        return strdup(file_math_h);
    else if (strcmp(path, "errno.h") == 0)
        return strdup(file_errno_h);
    else if (strcmp(path, "string.h") == 0)
        return strdup(file_string_h);
#endif

    return readfile_core(path);
}








//#pragma once


//#pragma once


struct parser_ctx;

/*
* A type is characterized by its type category, which is either the outermost derivation
* of a derived type (as noted above in the construction of derived types), or the type
* itself if the type consists of no derived types.
*/

enum type_category
{
    TYPE_CATEGORY_ITSELF,
    TYPE_CATEGORY_FUNCTION, /*derived declarator types*/
    TYPE_CATEGORY_ARRAY,    /*derived declarator types*/
    TYPE_CATEGORY_POINTER,  /*derived declarator types*/
};


enum type_specifier_flags
{
    TYPE_SPECIFIER_NONE = 0,
    TYPE_SPECIFIER_VOID = 1 << 0,
    TYPE_SPECIFIER_CHAR = 1 << 1,
    TYPE_SPECIFIER_SHORT = 1 << 2,
    TYPE_SPECIFIER_INT = 1 << 3,
    TYPE_SPECIFIER_LONG = 1 << 4,

    TYPE_SPECIFIER_FLOAT = 1 << 5,
    TYPE_SPECIFIER_DOUBLE = 1 << 6,
    TYPE_SPECIFIER_SIGNED = 1 << 7,
    TYPE_SPECIFIER_UNSIGNED = 1 << 8,
    TYPE_SPECIFIER_BOOL = 1 << 9,
    TYPE_SPECIFIER_COMPLEX = 1 << 10,
    TYPE_SPECIFIER_DECIMAL32 = 1 << 11,
    TYPE_SPECIFIER_DECIMAL64 = 1 << 12,
    TYPE_SPECIFIER_DECIMAL128 = 1 << 13,
    TYPE_SPECIFIER_ATOMIC = 1 << 14,
    TYPE_SPECIFIER_STRUCT_OR_UNION = 1 << 15,
    TYPE_SPECIFIER_ENUM = 1 << 16,
    TYPE_SPECIFIER_TYPEDEF = 1 << 17,

    //MICROSOFT
    TYPE_SPECIFIER_INT8 = 1 << 18,
    TYPE_SPECIFIER_INT16 = 1 << 19,
    TYPE_SPECIFIER_INT32 = 1 << 20,
    TYPE_SPECIFIER_INT64 = 1 << 21,

    TYPE_SPECIFIER_LONG_LONG = 1 << 22,
    
    TYPE_SPECIFIER_TYPEOF = 1 << 23, //?
    TYPE_SPECIFIER_NULLPTR = 1 << 24,
};

enum type_qualifier_flags
{
    TYPE_QUALIFIER_NONE,
    TYPE_QUALIFIER_CONST = 1 << 0,
    TYPE_QUALIFIER_RESTRICT = 1 << 1,
    TYPE_QUALIFIER_VOLATILE = 1 << 2,
    TYPE_QUALIFIER__ATOMIC = 1 << 3,
};


struct type_list
{
    struct type* head;
    struct type* tail;
};

struct array_function_type
{
    int constant_size;

    struct type_list params; //se for funcao lista de tipos usado por cada parametro
    bool bVarArg; //true se for funcao ....
    bool bIsArray;
    bool bIsFunction;
    int array_size;

    struct array_function_type* next;
};

struct array_function_type_list
{
    struct array_function_type* head;
    struct array_function_type* tail;
};

struct array_function_type* array_function_type_list_pop_back(struct array_function_type_list* list);

struct direct_declarator_type
{
    struct declarator_type* declarator_opt; //nao nulo se tiver (declarator )
    struct array_function_type_list array_function_type_list;
};


struct pointer_type
{
    enum type_qualifier_flags type_qualifier_flags;
    struct pointer_type* next;
};

struct pointer_type_list
{
    struct pointer_type* head;
    struct pointer_type* tail;
};

void pointer_type_list_pop_front(struct pointer_type_list* list);

struct declarator_type
{
    struct pointer_type_list pointers;
    struct direct_declarator_type* direct_declarator_type;
};


struct type
{    
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
   
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;
    
    struct declarator_type* declarator_type;

    struct type* next; //se quiser usar lista  ligada
};
void print_type(struct osstream* ss, struct type* type);
void print_item(struct osstream* ss, bool* first, const char* item);
struct type type_copy(struct type* p_type);

struct declarator_type* declarator_type_copy(struct declarator_type* p_declarator_type);

void print_declarator_type(struct osstream* ss, struct declarator_type* p_declarator_type, const char* name);

struct type get_function_return_type(struct type* p_type);
struct type type_common(struct type* p_type1, struct type* p_type2, struct error* error);
struct type get_array_item_type(struct type* p_type);
struct type get_pointer_content_type(struct type* p_type);
bool type_is_array(struct type* p_type);
bool type_is_pointer(struct type* p_type);
bool type_is_integer(struct type* p_type);
bool type_is_arithmetic(struct type* p_type);
bool type_is_compatible(struct type* a, struct type* b);
bool type_is_compatible_type_function_call(struct type* a, struct type* b);
bool type_is_function_or_function_pointer(struct type* p_type);
struct type get_pointer_content_type(struct type* p_type);
struct type get_array_item_type(struct type* p_type);


void print_declarator_description(struct osstream* ss, struct declarator_type* declarator);
struct type get_function_return_type(struct type* p_type);
bool type_is_pointer_or_array(struct type* p_type);
int type_get_rank(struct type* p_type1, struct error* error);
void type_set_int(struct type* p_type);
int type_get_sizeof(struct parser_ctx* ctx, struct type* p_type, struct error* error);
unsigned int type_get_hashof(struct parser_ctx* ctx, struct type* p_type, struct error* error);
bool type_is_same(struct type* a, struct type* b, bool compare_qualifiers);
struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type);
struct type get_address_of_type(struct type* p_type);
void type_print(struct type* a);
bool type_is_scalar(struct type* p_type);
enum type_category find_type_category(const struct type* p_type);
void print_type_qualifier_specifiers(struct osstream* ss, struct type* type);


struct parser_ctx;

enum expression_type
{
    PRIMARY_IDENTIFIER,
    TYPEID_EXPRESSION_TYPE,

    PRIMARY_EXPRESSION_ENUMERATOR,
    PRIMARY_EXPRESSION_DECLARATOR,
    PRIMARY_EXPRESSION_STRING_LITERAL,
    PRIMARY_EXPRESSION_CHAR_LITERAL,
    PRIMARY_EXPRESSION_PREDEFINED_CONSTANT, /*true false*/
    PRIMARY_EXPRESSION_GENERIC,
    PRIMARY_EXPRESSION_NUMBER,

    POSTFIX_EXPRESSION_FUNCTION_LITERAL,
    POSTFIX_EXPRESSION_COMPOUND_LITERAL,

    POSTFIX_FUNCTION_CALL, // ( ) 
    POSTFIX_ARRAY, // [ ]
    POSTFIX_DOT, // .
    POSTFIX_ARROW, // .
    POSTFIX_INCREMENT,
    POSTFIX_DECREMENT,


    UNARY_EXPRESSION_SIZEOF_EXPRESSION,
    UNARY_EXPRESSION_SIZEOF_TYPE,
    UNARY_EXPRESSION_HASHOF_TYPE,
    UNARY_EXPRESSION_ALIGNOF,

    UNARY_EXPRESSION_INCREMENT,
    UNARY_EXPRESSION_DECREMENT,

    UNARY_EXPRESSION_NOT,
    UNARY_EXPRESSION_BITNOT,
    UNARY_EXPRESSION_NEG,
    UNARY_EXPRESSION_PLUS,
    UNARY_EXPRESSION_CONTENT,
    UNARY_EXPRESSION_ADDRESSOF,

    CAST_EXPRESSION,

    MULTIPLICATIVE_EXPRESSION_MULT,
    MULTIPLICATIVE_EXPRESSION_DIV,
    MULTIPLICATIVE_EXPRESSION_MOD,

    ADDITIVE_EXPRESSION_PLUS,
    ADDITIVE_EXPRESSION_MINUS,

    SHIFT_EXPRESSION_RIGHT,
    SHIFT_EXPRESSION_LEFT,

    RELATIONAL_EXPRESSION_BIGGER_THAN,
    RELATIONAL_EXPRESSION_LESS_THAN,
    RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN,
    RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN,

    EQUALITY_EXPRESSION_EQUAL,
    EQUALITY_EXPRESSION_NOT_EQUAL,

    AND_EXPRESSION,
    EXCLUSIVE_OR_EXPRESSION,
    INCLUSIVE_OR_EXPRESSION,
    INCLUSIVE_AND_EXPRESSION,
    LOGICAL_OR_EXPRESSION,
    ASSIGNMENT_EXPRESSION,
};


enum expression_flags
{
    EXPRESSION_NONE = 0,
    CONSTANT_EXPRESSION_FLAG = 1 << 0
};

struct argument_expression_list
{
    struct argument_expression* head;
    struct argument_expression* tail;
};



struct generic_association
{
    /*
     generic-association:
       type-name : assignment-expression
       "default" : assignment-expression
    */

    struct type type;
    struct type_name* p_type_name;
    struct expression* expression;

    struct token* first_token;
    struct token* last_token;

    struct generic_association* next;
};

struct generic_assoc_list
{
    struct generic_association* head;
    struct generic_association* tail;
};

struct generic_selection
{
    /*
      generic-selection:
      "_Generic" ( assignment-expression , generic-assoc-list )
    */


    struct expression* expression;

    /*
    * Points to the matching expression
    */
    struct expression* p_view_selected_expression;

    struct generic_assoc_list generic_assoc_list;
    struct token* first_token;
    struct token* last_token;
};

struct expression
{
    enum expression_type expression_type;
    enum expression_flags flags;
    struct type type;


    long long constant_value;
    unsigned long long constant_ull_value;

    struct type_name* type_name; //cast or compound literal    
    struct braced_initializer* braced_initializer;
    struct compound_statement* compound_statement; //function literal (lambda)
    struct generic_selection* generic_selection; //_Generic

    struct token* first;
    struct token* last;

    /*se expressão for um identificador ele aponta para declaração dele*/
    struct declarator* declarator;

    /*se for POSTFIX_FUNCTION_CALL post*/
    struct argument_expression_list argument_expression_list; //este node eh uma  chamada de funcao


    struct expression* left;
    struct expression* right;
};

/*contexto expressões parser*/
struct expression_ctx
{
    bool bConstantExpressionRequired;
    bool bResultIsType;
};

struct expression* assignment_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* constant_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);



//#pragma once




struct scope
{
    int scope_level;
    struct hash_map tags;
    struct hash_map variables;

    struct scope* next;
    struct scope* previous;
};



struct scope_list
{
    struct scope* head;
    struct scope* tail;
};
void scope_list_push(struct scope_list* list, struct scope* s);
void scope_list_pop(struct scope_list* list);

struct parser_ctx
{
    enum language_version input_language;
    bool check_naming_conventions;

    /*
    There are four kinds of scopes:
    function,
    file,
    block,
    function prototype.
    (A function prototype is a declaration of a function that declares the types of its parameters.)
    */
    struct scope_list scopes;
    
    /*
    * Points to the function we're in. Or null in file scope.
    */
    struct declaration* p_current_function_opt;

    /*
    * Points to the try-block we're in. Or null.
    */
    struct try_statement* p_current_try_statement_opt;

    struct token_list input_list;
    struct token* current;
    struct token* previous;
    int try_catch_block_index;
    int n_warnings;
    int n_errors;
    int (*printf)(const char* fmt, ...);
};

///////////////////////////////////////////////////////


struct token* parser_look_ahead(struct parser_ctx* ctx);

struct token* parser_match(struct parser_ctx* ctx);
void parser_match_tk(struct parser_ctx* ctx, enum token_type type, struct error* error);
struct token* parser_look_ahead(struct parser_ctx* ctx);
struct token* previous_parser_token(struct token* token);
struct declarator* find_declarator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt);
struct enumerator* find_enumerator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt);

struct struct_or_union_specifier* find_struct_or_union_specifier(struct parser_ctx* ctx, const char* lexeme);
bool first_is(struct parser_ctx* ctx, enum token_type type);
void print_scope(struct scope_list* e);
void parser_seterror_with_token(struct parser_ctx* er, struct token* p_token, const char* fmt, ...);

char* CompileText(const char* options, const char* content);

void parser_seterror_with_token(struct parser_ctx* ctx, struct token* p_token, const char* fmt, ...);
void parser_setwarning_with_token(struct parser_ctx* ctx, struct token* p_token, const char* fmt, ...);

int compile(int argc, char** argv, struct error* error);
struct declaration_list parse(struct options* options, struct token_list* list, struct error* error);


struct token* parser_skip_blanks(struct parser_ctx* ctx);





void print_type_qualifier_flags(struct osstream* ss, bool* first, enum type_qualifier_flags e_type_qualifier_flags);


enum token_type parse_number(const char* lexeme, enum type_specifier_flags* flags_opt, struct error* error);
bool print_type_specifier_flags(struct osstream* ss, bool* first, enum type_specifier_flags e_type_specifier_flags);

enum storage_class_specifier_flags
{
    STORAGE_SPECIFIER_NONE = 0,
    STORAGE_SPECIFIER_TYPEDEF = 1 << 0,
    STORAGE_SPECIFIER_EXTERN = 1 << 1,
    STORAGE_SPECIFIER_STATIC = 1 << 2,
    STORAGE_SPECIFIER_THREAD_LOCAL = 1 << 3,
    STORAGE_SPECIFIER_AUTO = 1 << 4,
    STORAGE_SPECIFIER_REGISTER = 1 << 5,
    STORAGE_SPECIFIER_CONSTEXPR = 1 << 6,
};

struct expression_ctx;

struct declaration_specifier
{
    struct storage_class_specifier* storage_class_specifier;
    struct type_specifier_qualifier* type_specifier_qualifier;
    struct function_specifier* function_specifier;

    struct declaration_specifier* next;
};

struct declaration_specifier* declaration_specifier(struct parser_ctx* ctx, struct error* error);


struct declaration_specifiers
{
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
    enum storage_class_specifier_flags storage_class_specifier_flags;

    //shortcut
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;

    struct declarator* typedef_declarator;
    struct typeof_specifier* typeof_specifier;

    struct expression* p_typeof_expression_opt;

    struct declaration_specifier* head;
    struct declaration_specifier* tail;
};

void print_declaration_specifiers(struct osstream* ss, struct declaration_specifiers* p);
struct declaration_specifiers* declaration_specifiers(struct parser_ctx* ctx, struct error* error);

struct static_assert_declaration
{
    /*
     static_assert-declaration:
       "static_assert" ( constant-expression , string-literal ) ;
       "static_assert" ( constant-expression ) ;
    */

    struct token* first_token;
    struct token* last_token;
    struct expression* constant_expression;
    struct token* string_literal_opt;
};
struct static_assert_declaration* static_assert_declaration(struct parser_ctx* ctx, struct error* error);

struct attribute_specifier_sequence
{
    struct attribute_specifier* head;
    struct attribute_specifier* tail;
};
struct attribute_specifier_sequence* attribute_specifier_sequence_opt(struct parser_ctx* ctx, struct error* error);

struct attribute_specifier
{
    struct token* first;
    struct token* last;
    struct attribute_list* attribute_list;
    struct attribute_specifier* next;
};
struct attribute_specifier* attribute_specifier(struct parser_ctx* ctx, struct error* error);
struct attribute* attribute(struct parser_ctx* ctx, struct error* error);


struct storage_class_specifier
{
    /*
     storage-class-specifier:
      "auto"
      "constexpr"
      "extern"
      "register"
      "static"
      "thread_local"
      "typedef"
    */
    enum storage_class_specifier_flags flags;
    struct token* token;
};

struct storage_class_specifier* storage_class_specifier(struct parser_ctx* ctx);

struct function_specifier
{
    struct token* token;
};
struct function_specifier* function_specifier(struct parser_ctx* ctx, struct error* error);


struct typeof_specifier_argument
{
    /*
     typeof-specifier-argument:
       expression
       type-name
    */
    struct expression* expression;
    struct type_name* type_name;
};

struct typeof_specifier
{
    /*
     typeof-specifier:
       "typeof" ( typeof-specifier-argument )
    */
    struct token* token;
    struct token* endtoken;
    struct typeof_specifier_argument* typeof_specifier_argument;
};

struct type_specifier
{
    /*
     type-specifier:
        "void"
        "char"
        "short"
        "int"
        "long"
        "float"
        "double"
        "signed"
        "unsigned"
        "_BitInt" ( constant-expression )
        "bool"
        "_Complex"
        "_Decimal32"
        "_Decimal64"
        "_Decimal128"
        atomic-type-specifier
        struct-or-union-specifier
        enum-specifier
        typedef-name
        typeof-specifier
    */
    enum type_specifier_flags flags;
    struct token* token;
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct typeof_specifier* typeof_specifier;
    struct enum_specifier* enum_specifier;
    struct declarator* typedef_declarator;
    struct atomic_type_specifier* atomic_type_specifier;
};

struct type_specifier* type_specifier(struct parser_ctx* ctx, struct error* error);

struct init_declarator_list
{
    struct init_declarator* head;
    struct init_declarator* tail;
};
struct init_declarator_list init_declarator_list(struct parser_ctx* ctx, struct declaration_specifiers* declaration_specifiers, struct error* error);


struct declaration
{
    /*
                                  declaration-specifiers init-declarator-list_opt ;
     attribute-specifier-sequence declaration-specifiers init-declarator-list ;
     static_assert-declaration
     attribute-declaration
    */
    struct static_assert_declaration* static_assert_declaration;

    struct declaration_specifiers* declaration_specifiers;
    struct init_declarator_list init_declarator_list;

    struct compound_statement* function_body;

    struct token* first_token;
    struct token* last_token;

    struct declaration* next;
};


struct declaration* external_declaration(struct parser_ctx* ctx, struct error* error);

struct atomic_type_specifier
{
    struct token* token;
};

struct atomic_type_specifier* atomic_type_specifier(struct parser_ctx* ctx, struct error* error);

struct enumerator_list
{
    struct enumerator* head;
    struct enumerator* tail;
};

struct enumerator_list enumerator_list(struct parser_ctx* ctx, struct error* error);


struct enum_specifier
{
    /*
     "enum" attribute-specifier-sequence opt identifier opt enum-type-specifier opt
      { enumerator-list }

     "enum" attribute-specifier-sequenceopt identifieropt enum-type-specifier opt
          { enumerator-list , }
     
     "enum" identifier enum-type-specifier opt
    */
    struct type_specifier_qualifier* type_specifier_qualifier;
    struct enumerator_list enumerator_list;
    struct type_tag_id type_id;
    struct token* tag_token;

    /*points to the complete enum (can be self pointed)*/
    struct enum_specifier* complete_enum_specifier;
};

struct enum_specifier* enum_specifier(struct parser_ctx*, struct error* error);

struct member_declaration_list
{
    struct token* first;
    struct token* last;
    struct member_declaration* head;
    struct member_declaration* tail;
};
struct member_declaration_list member_declaration_list(struct parser_ctx* ctx, struct error* error);
struct member_declarator* find_member_declarator(struct member_declaration_list* list, const char* name);

struct struct_or_union_specifier
{
    struct attribute_specifier_sequence* attribute_specifier_sequence;
    struct member_declaration_list member_declaration_list;
    struct type_tag_id type_id;
    struct token* first;
    struct token* last;

    /*
    * Token que possui tag da struct
    */
    struct token* tagtoken;

    char tag_name[200];
    /*geramos um tag name para anomimas, mas colocamos banonymousTag para true*/
    bool has_anonymous_tag;

    int scope_level; /*nivel escopo 0 global*/
    int visit_moved; /*nivel escopo 0 global*/

    /*
    * O tag de uma struct aponta para uma especificação completa (se existir)
    * da struct. Aqui apontamos para esta struct completa.
    */
    struct struct_or_union_specifier* complete_struct_or_union_specifier;
};
struct struct_or_union_specifier* struct_or_union_specifier(struct parser_ctx* ctx, struct error* error);


struct init_declarator
{
    struct declarator* declarator;
    struct initializer* initializer;

    struct init_declarator* next;
};
struct init_declarator* init_declarator(struct parser_ctx* ctx, struct declaration_specifiers* declaration_specifiers, struct error* error);

struct initializer
{
    /*
     initializer:
       assignment-expression
       braced-initializer
    */
    struct token* first_token;
    struct designation* designation; /*auxiliar para list??*/
    struct braced_initializer* braced_initializer;
    struct expression* assignment_expression;
    struct initializer* next;
};

struct initializer* initializer(struct parser_ctx* ctx, struct error* error);



struct declarator
{
    struct type_tag_id type_id;
    struct pointer* pointer;
    struct direct_declarator* direct_declarator;

    struct declaration_specifiers* declaration_specifiers; //aponta para este cara
    struct specifier_qualifier_list* specifier_qualifier_list; //aponta
    struct token* name; //shortcut

    struct compound_statement* function_body;

    int num_uses; /*used to show not used warnings*/

    /*Já mastiga o tipo dele*/
    struct type type;
};

struct declarator;
void print_declarator(struct osstream* ss, struct declarator* declarator, bool bAbstract);

struct declarator* declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* specifier_qualifier_list,
    struct declaration_specifiers* declaration_specifiers,
    bool bAbstractAcceptable,
    struct token** pptokenName,
    struct error* error);


struct array_function
{
    struct array_declarator* array_declarator;
    struct function_declarator* function_declarator;
    struct array_function* next;
};

struct array_function_list
{
    struct array_function* head;
    struct array_function* tail;
};

struct direct_declarator
{
    struct token* name;
    struct declarator* declarator;
    struct array_function_list array_function_list;
};

struct direct_declarator* direct_declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* specifier_qualifier_list,
    struct declaration_specifiers* declaration_specifiers,
    bool bAbstractAcceptable,
    struct token** pptokenName,
    struct error* error);

struct array_declarator
{
    struct expression* assignment_expression;
    struct expression* expression;
    struct type_qualifier_list* type_qualifier_list_opt;
    unsigned long long constant_size;
    struct token* token;
};
struct array_declarator* array_declarator(struct parser_ctx* ctx, struct error* error);

struct function_declarator
{
    struct scope parameters_scope; //usado para escopo parametros
    struct parameter_type_list* parameter_type_list_opt;
};

struct function_declarator* function_declarator(struct parser_ctx* ctx, struct error* error);

struct parameter_type_list
{
    bool is_var_args; /*(...)*/
    bool is_void;/*(void)*/
    struct parameter_list* parameter_list;
};
struct parameter_type_list* parameter_type_list(struct parser_ctx* ctx, struct error* error);

struct pointer
{
    struct type_qualifier_list* type_qualifier_list_opt;
    struct pointer* pointer;
};

struct pointer* pointer_opt(struct parser_ctx* ctx, struct error* error);

struct parameter_list
{
    struct parameter_declaration* head;
    struct parameter_declaration* tail;
};
struct parameter_list* parameter_list(struct parser_ctx* ctx, struct error* error);

struct parameter_declaration
{
    struct token* name;
    struct declaration_specifiers* declaration_specifiers;
    struct declarator* declarator;

    struct parameter_declaration* next;
};
struct parameter_declaration* parameter_declaration(struct parser_ctx* ctx, struct error* error);

struct type_name
{
    struct token* first;
    struct token* last;
    struct specifier_qualifier_list* specifier_qualifier_list;
    struct declarator* declarator;
};
struct type_name* type_name(struct parser_ctx* ctx, struct error* error);
void print_type_name(struct osstream* ss, struct type_name* p);

struct argument_expression
{
    struct expression* expression;
    struct argument_expression* next;
};

struct braced_initializer
{
    struct token* first;
    /*
     { }
     { initializer-list }
     { initializer-list , }
    */
    struct initializer_list* initializer_list;
};
struct braced_initializer* braced_initializer(struct parser_ctx* ctx, struct error* error);


struct specifier_qualifier
{
    struct type_specifier* type_specifier;
    struct type_qualifier* type_qualifier;
    struct specifier_qualifier* next;
};

struct specifier_qualifier_list
{
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;

    //shortcut
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;
    struct typeof_specifier* typeof_specifier;
    struct declarator* typedef_declarator;
    
    struct specifier_qualifier* head;
    struct specifier_qualifier* tail;
};
struct specifier_qualifier_list* specifier_qualifier_list(struct parser_ctx* ctx, struct error* error);
void print_specifier_qualifier_list(struct osstream* ss, bool* first, struct specifier_qualifier_list* p_specifier_qualifier_list);

struct type_specifier_qualifier
{
    struct type_specifier* type_specifier;
    struct type_qualifier* type_qualifier;
    struct alignment_specifier* alignment_specifier;

    struct type_specifier_qualifier* next;
};
struct type_specifier_qualifier* type_specifier_qualifier(struct parser_ctx* ctx, struct error* error);


struct alignment_specifier
{
    struct token* token;
};
struct alignment_specifier* alignment_specifier(struct parser_ctx* ctx, struct error* error);


struct type_qualifier
{
    enum type_qualifier_flags flags;
    struct token* token;
    struct type_qualifier* next;
};

struct type_qualifier* type_qualifier(struct parser_ctx* ctx, struct error* error);


struct member_declaration
{
    /*
     member-declaration:
       attribute-specifier-sequence opt specifier-qualifier-list member-declarator-list opt ;
       static_assert-declaration
    */
    struct specifier_qualifier_list* specifier_qualifier_list;
    struct member_declarator_list* member_declarator_list_opt;

    struct static_assert_declaration* static_assert_declaration;

    struct member_declaration* next;
};

struct member_declaration* member_declaration(struct parser_ctx* ctx, struct error* error);

struct member_declarator
{
    struct token* name;
    struct declarator* declarator;
    struct expression* constant_expression;
    struct member_declarator* next;
};
struct member_declarator_list
{
    struct member_declarator* head;
    struct member_declarator* tail;
};
struct member_declarator_list* member_declarator_list(struct parser_ctx* ctx,
    struct specifier_qualifier_list* specifier_qualifier_list,
    struct error* error);

struct block_item_list
{
    struct block_item* head;
    struct block_item* tail;
};
struct block_item_list block_item_list(struct parser_ctx* ctx, struct error* error);


struct compound_statement
{
    struct token* first_token;
    struct token* last_token;

    struct block_item_list block_item_list;
};
struct compound_statement* compound_statement(struct parser_ctx* ctx, struct error* error);

struct defer_statement
{
    /*
     defer-statement: (extension)
       "defer" secondary-block     
    */
    struct token* firsttoken;
    struct token* lasttoken;
    struct secondary_block* secondary_block;
};

struct try_statement
{
   
   /*
     try-statement: (extension)
      "try" secondary-block
      "try" secondary-block "catch" secondary-block
   */
    struct secondary_block* secondary_block;
    struct secondary_block* catch_secondary_block_opt;
    struct token* first_token;
    struct token* last_token;
    struct token* catch_token_opt;
    /*Used to generate label names*/
    int try_catch_block_index;
};

struct try_statement* try_statement(struct parser_ctx* ctx, struct error* error);

struct selection_statement
{
    /*
    selection-statement:
     "if" ( expression ) secondary-block
     "if" ( expression ) secondary-block "else" secondary-block
     "switch" ( expression ) secondary-block
    */

  

    struct init_declarator* init_declarator;
    struct expression* expression;

    struct secondary_block* secondary_block;
    struct secondary_block* else_secondary_block_opt;

    struct token* first_token;
    struct token* last_token;
    struct token* else_token_opt;

    
};

struct selection_statement* selection_statement(struct parser_ctx* ctx, struct error* error);

struct iteration_statement
{
    struct token* token;
    struct secondary_block* secondary_block;
    struct expression* expression1;
    struct expression* expression2;
};

struct iteration_statement* iteration_statement(struct parser_ctx* ctx, struct error* error);

struct jump_statement
{
    /*
    */
    struct token* token;
    struct token* label;
    struct token* lasttoken;
    struct expression* expression;
    
    int try_catch_block_index;
};
struct jump_statement* jump_statement(struct parser_ctx* ctx, struct error* error);

struct expression_statement
{
    struct token* first_token;
    struct expression* expression;
};
struct expression_statement* expression_statement(struct parser_ctx* ctx, struct error* error);


struct block_item
{
    struct token* first_token;
    struct declaration* declaration;
    struct labeled_statement* labeled_statement;
    struct label* label;
    struct unlabeled_statement* unlabeled_statement;

    struct block_item* next;
};
struct block_item* block_item(struct parser_ctx* ctx, struct error* error);


struct compound_statement* function_body(struct parser_ctx* ctx, struct error* error);

struct designator
{
    struct token* token;
    struct designator* next;
};
struct designator* designator(struct parser_ctx* ctx, struct error* error);

struct initializer_list
{
    struct token* first_token;
    struct initializer* head;
    struct initializer* tail;
};
struct initializer_list* initializer_list(struct parser_ctx* ctx, struct error* error);

struct primary_block
{
    /*
       primary-block:
       compound-statement
       selection-statement
       iteration-statement
       defer-statement (extension)
       try-statement (extension)
    */
    struct token* last;
    struct compound_statement* compound_statement;
    struct selection_statement* selection_statement;
    struct iteration_statement* iteration_statement;
    struct defer_statement* defer_statement;
    struct try_statement* try_statement;
};

struct secondary_block
{
    /*
     secondary-block:
       statement
    */
    struct token* first;
    struct token* last;
    struct statement* statement;
};

struct unlabeled_statement
{

    /*
      unlabeled-statement:
      expression-statement
      attribute-specifier-sequenceopt primary-block
      attribute-specifier-sequenceopt jump-statement
     */

    struct expression_statement* expression_statement;
    struct primary_block* primary_block;
    struct jump_statement* jump_statement;
};

struct unlabeled_statement* unlabeled_statement(struct parser_ctx* ctx, struct error* error);

struct labeled_statement
{
    struct label* label;
    struct statement* statement;
};
struct labeled_statement* labeled_statement(struct parser_ctx* ctx, struct error* error);

struct statement
{    
    struct labeled_statement* labeled_statement;
    struct unlabeled_statement* unlabeled_statement;
};
struct statement* statement(struct parser_ctx* ctx, struct error* error);

struct designator_list
{
    struct designator* head;
    struct designator* tail;
};
struct designator_list* designator_list(struct parser_ctx* ctx, struct error* error);

struct designation
{
    struct token* token;
};
struct designation* designation(struct parser_ctx* ctx, struct error* error);


struct type_qualifier_list
{
    enum type_qualifier_flags flags;
    struct type_qualifier* head;
    struct type_qualifier* tail;
};
struct type_qualifier_list* type_qualifier_list(struct parser_ctx* ctx, struct error* error);


struct attribute_token
{
    struct token* token;
};
struct attribute_token* attribute_token(struct parser_ctx* ctx, struct error* error);

struct attribute
{
    struct attribute_token* attribute_token;
    struct attribute_argument_clause* attribute_argument_clause;
    struct attribute_specifier* next;
};

struct attribute_list
{
    struct attribute* head;
    struct attribute* tail;
};
struct attribute_list* attribute_list(struct parser_ctx* ctx, struct error* error);

//struct token_list enum_specifier(struct parser_ctx* ctx, struct error* error);


struct enumerator
{
    struct type_tag_id type_id;
    struct token* token;

    struct expression* constant_expression_opt;

    struct enumerator* next;
    long long value;    
};

struct enumerator* enumerator(struct parser_ctx* ctx, struct error* error);

struct attribute_argument_clause
{
    struct token* token;
};
struct attribute_argument_clause* attribute_argument_clause(struct parser_ctx* ctx, struct error* error);

bool first_of_attribute(struct parser_ctx* ctx);

struct balanced_token_sequence
{
    struct balanced_token* head;
    struct balanced_token* tail;
};
struct balanced_token_sequence* balanced_token_sequence_opt(struct parser_ctx* ctx, struct error* error);


struct expression* typeid_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);

bool is_first_of_conditional_expression(struct parser_ctx* ctx);
bool first_of_type_name(struct parser_ctx* ctx);
bool first_of_type_name_ahead(struct parser_ctx* ctx);



struct argument_expression_list argument_expression_list(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);


struct declaration_list
{
    struct declaration* head;
    struct declaration* tail;
};

struct declaration_list translation_unit(struct parser_ctx* ctx, struct error* error);
void declaration_list_destroy(struct declaration_list* list);

struct label
{
    struct token* name;
};
struct label* label(struct parser_ctx* ctx, struct error* error);


struct ast
{
    struct token_list token_list;
    struct declaration_list declaration_list;
};


struct ast get_ast(struct options* options, const char* fileName, const char* source, struct error* error);
void ast_destroy(struct ast* ast);
struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);



#ifdef _WIN32
#undef assert
#define assert _ASSERTE
#endif




struct expression* postfix_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* cast_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* multiplicative_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* unary_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* additive_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* shift_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* relational_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* equality_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* and_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* exclusive_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* inclusive_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* logical_and_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* logical_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* conditional_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
struct expression* conditional_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);



int  compare_function_arguments(struct parser_ctx* ctx,
    struct type* p_type,
    struct argument_expression_list* p_argument_expression_list,
    struct error* error)
{
    try
    {
        struct type_list parameter_type = { 0 };


        bool is_var_args = false;
        bool is_void = false;

        if (p_type &&
            p_type->declarator_type &&
            p_type->declarator_type->direct_declarator_type &&
            p_type->declarator_type->direct_declarator_type->array_function_type_list.head)
        {
            parameter_type = p_type->declarator_type->direct_declarator_type->array_function_type_list.head->params;
            is_var_args = p_type->declarator_type->direct_declarator_type->array_function_type_list.head->bVarArg;

            /*detectar que o parametro é (void)*/
            is_void =
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head->params.head &&
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head->params.head->type_specifier_flags == TYPE_SPECIFIER_VOID &&
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head->params.head->declarator_type->pointers.head == NULL;
        }


        struct type* current_parameter_type = parameter_type.head;

        int param_num = 1;
        struct argument_expression* current_argument =
            p_argument_expression_list->head;
        while (current_argument && current_parameter_type)
        {

            if (!type_is_compatible_type_function_call(&current_argument->expression->type, current_parameter_type) != 0)
            {
                parser_seterror_with_token(ctx,
                    current_argument->expression->first,
                    " incompatible types at argument %d", param_num);
            }
            //compare
            current_argument = current_argument->next;
            current_parameter_type = current_parameter_type->next;
            param_num++;
        }

        if (current_argument != NULL && !is_var_args)
        {
            parser_seterror_with_token(ctx,
                p_argument_expression_list->tail->expression->first,
                "too many arguments");
            throw;
        }

        if (current_parameter_type != NULL && !is_void)
        {
            parser_seterror_with_token(ctx,
                p_argument_expression_list->tail->expression->first,
                "too few arguments");

            //seterror(error, "too few parameter_type");
            throw;
        }
    }
    catch
    {}
    return error->code;

}



bool is_enumeration_constant(struct parser_ctx* ctx)
{
    if (ctx->current->type != TK_IDENTIFIER)
    {
        return false;
    }

    if (ctx->current->flags & TK_FLAG_IDENTIFIER_IS_ENUMERATOR)
        return true;

    if (ctx->current->flags & TK_FLAG_IDENTIFIER_IS_NOT_ENUMERATOR)
        return false;

    const bool is_enumerator = find_enumerator(ctx, ctx->current->lexeme, NULL) != NULL;

    if (is_enumerator)
    {
        ctx->current->flags |= TK_FLAG_IDENTIFIER_IS_ENUMERATOR;
    }
    else
    {
        ctx->current->flags |= TK_FLAG_IDENTIFIER_IS_NOT_ENUMERATOR;
    }

    return is_enumerator;
}

bool is_first_of_floating_constant(struct parser_ctx* ctx)
{
    /*
     floating-constant:
      decimal-floating-constant
      hexadecimal-floating-constan
    */
    return ctx->current->type == TK_COMPILER_DECIMAL_FLOATING_CONSTANT ||
        ctx->current->type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT;
}

bool is_first_of_integer_constant(struct parser_ctx* ctx)
{
    /*
     integer-constant:
      decimal-constant integer-suffixopt
      octal-constant integer-suffixopt
      hexadecimal-constant integer-suffixopt
      binary-constant integer-suffixop
    */

    return ctx->current->type == TK_COMPILER_DECIMAL_CONSTANT ||
        ctx->current->type == TK_COMPILER_OCTAL_CONSTANT ||
        ctx->current->type == TK_COMPILER_HEXADECIMAL_CONSTANT ||
        ctx->current->type == TK_COMPILER_BINARY_CONSTANT;
}

bool is_predefined_constant(struct parser_ctx* ctx)
{
    return ctx->current->type == TK_KEYWORD_TRUE ||
        ctx->current->type == TK_KEYWORD_FALSE ||
        ctx->current->type == TK_KEYWORD_NULLPTR;
}

bool is_first_of_constant(struct parser_ctx* ctx)
{
    /*
     constant:
      integer-constant
      floating-constant
      enumeration-constant
      character-constant
      predefined-constant
    */
    return is_first_of_integer_constant(ctx) ||
        is_first_of_floating_constant(ctx) ||
        is_enumeration_constant(ctx) ||
        (ctx->current->type == TK_CHAR_CONSTANT) ||
        is_predefined_constant(ctx);
}

bool is_first_of_primary_expression(struct parser_ctx* ctx)
{
    /*
     primary-expression:
      identifier
      constant
      string-literal
      ( expression )
      generic-selection
      typeid (expression )
    */
    if (ctx->current == NULL)
        return false;

    return ctx->current->type == TK_IDENTIFIER ||
        is_first_of_constant(ctx) ||
        ctx->current->type == TK_STRING_LITERAL ||
        ctx->current->type == '(' ||
        ctx->current->type == TK_KEYWORD__GENERIC ||
        ctx->current->type == TK_KEYWORD_TYPEID;
}

struct generic_association* generic_association(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    struct generic_association* p_generic_association = NULL;
    try
    {
        p_generic_association = calloc(1, sizeof * p_generic_association);
        

        /*generic - association:
            type-name : assignment-expression
            default : assignment-expression
            */
        if (ctx->current && ctx->current->type == TK_KEYWORD_DEFAULT)
        {
            parser_match(ctx);
        }
        else if (first_of_type_name(ctx))
        {            
            p_generic_association->p_type_name = type_name(ctx, error);
            p_generic_association->type = make_type_using_declarator(ctx, p_generic_association->p_type_name->declarator);            
        }
        else
        {
            seterror(error, "error");
            error->code = 1;
        }
        parser_match_tk(ctx, ':', error);
        p_generic_association->expression =  assignment_expression(ctx, error, ectx);
    }
    catch{
    }

    return p_generic_association;
}

struct generic_assoc_list generic_association_list(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    struct generic_assoc_list list = { 0 };
    try
    {
        struct generic_association* p_generic_association = 
          generic_association(ctx, error, ectx);
        
        list_add(&list, p_generic_association);
        
        while (error->code == 0 &&
            ctx->current->type == ',')
        {
            parser_match(ctx);

            struct generic_association* p_generic_association2 =
                generic_association(ctx, error, ectx);
            list_add(&list, p_generic_association2);
        }
    }
    catch
    {
    }
    return list;
}

static void print_clean_list(struct token_list* list)
{
    struct token* current = list->head;
    while (current)
    {
        if (current != list->head &&
            (current->flags & TK_FLAG_HAS_SPACE_BEFORE ||
                current->flags & TK_FLAG_HAS_NEWLINE_BEFORE))
        {
            printf(" ");
        }
        printf("%s", current->lexeme);
        if (current == list->tail)
            break;
        current = current->next;
    }
}

struct generic_selection * generic_selection(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
    generic-selection:
    _Generic ( assignment-expression , generic-assoc-ctx )
    */
    struct generic_selection* p_generic_selection = NULL;
    try
    {
        p_generic_selection = calloc(1, sizeof * p_generic_selection);
        
        p_generic_selection->first_token = ctx->current;
        

        parser_match_tk(ctx, TK_KEYWORD__GENERIC, error);
        parser_match_tk(ctx, '(', error);
        struct token_list l = { 0 };
        l.head = ctx->current;
        p_generic_selection->expression = assignment_expression(ctx, error, ectx);
        l.tail = ctx->current->prev;
        
        parser_match_tk(ctx, ',', error);

        p_generic_selection->generic_assoc_list = generic_association_list(ctx, error, ectx);
        
        struct generic_association* current = p_generic_selection->generic_assoc_list.head;
        while (current)
        {
            if (current->p_type_name)
            {
                if (type_is_same(&p_generic_selection->expression->type, &current->type, true))
                {
                    p_generic_selection->p_view_selected_expression = current->expression;                    
                    break;
                }
            }
            else
            {
                /*default*/
                p_generic_selection->p_view_selected_expression = current->expression;
            }
            current = current->next;
        }
        
        p_generic_selection->last_token= ctx->current;
        parser_match_tk(ctx, ')', error);
    }
    catch
    {
    }
    return p_generic_selection;
}


struct expression* typeid_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = calloc(1, sizeof * p_expression_node);
        p_expression_node->expression_type = TYPEID_EXPRESSION_TYPE;
        p_expression_node->first = ctx->current;

        parser_match_tk(ctx, TK_KEYWORD_TYPEID, error);
        parser_match_tk(ctx, '(', error);
        if (first_of_type_name(ctx))
        {
            
            p_expression_node->type_name = type_name(ctx, error);
            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
            //printf("typeid() = ");
            //print_type(&p_expression_node->type);
            //printf("\n");
        }
        else
        {
        
            bool constant_expr_required_old = ectx->bConstantExpressionRequired;
            ectx->bConstantExpressionRequired = false;
            p_expression_node->right = expression(ctx, error, ectx);

            if (p_expression_node->right == NULL)
                throw;

            ectx->bConstantExpressionRequired = constant_expr_required_old;
            p_expression_node->type = type_copy(&p_expression_node->right->type);

            //printf("typeid() = ");
            //print_type(&p_expression_node->type);
            //printf("\n");
        }

        p_expression_node->last = ctx->current;
        parser_match_tk(ctx, ')', error);
    }
    catch
    {
    }

    return p_expression_node;
}

int char_constant_to_int(const char* text)
{
    const char* p = text + 1;
    int value = 0;
    while (*p != '\'')
    {
        value = value * 256 + *p;
        p++;
    }

    return value;
}


//enum token_type parse_number(const char* lexeme, enum type_specifier_flags* flags, struct error* error);

int convert_to_number(struct token* token, struct expression* p_expression_node, struct error* error)
{
    /*copia removendo os separadores*/
    //um dos maiores buffer necessarios seria 128 bits binario...
    //0xb1'1'1.... 
    int c = 0;
    char buffer[128 * 2 + 4] = { 0 };
    const char* s = token->lexeme;
    while (*s)
    {
        if (*s != '\'')
        {
            buffer[c] = *s;
            c++;
        }
        s++;
    }
    enum type_specifier_flags  flags = 0;
    parse_number(buffer, &flags, error);
    p_expression_node->type.type_specifier_flags = flags;

    switch (token->type)
    {
    case TK_COMPILER_DECIMAL_CONSTANT:

        if (flags && TYPE_SPECIFIER_UNSIGNED)
        {
            p_expression_node->constant_value = strtoll(buffer, 0, 10);
        }
        else
        {
            p_expression_node->constant_value = strtoll(buffer, 0, 10);
        }

        break;
    case TK_COMPILER_OCTAL_CONSTANT:
        p_expression_node->constant_value = strtoll(buffer, 0, 8);
        break;
    case TK_COMPILER_HEXADECIMAL_CONSTANT:
        p_expression_node->constant_value = strtoll(buffer + 2, 0, 16);
        break;
    case TK_COMPILER_BINARY_CONSTANT:
        p_expression_node->constant_value = strtoll(buffer + 2, 0, 2);
        break;
    case TK_COMPILER_DECIMAL_FLOATING_CONSTANT:
        //p_expression_node->type.type_specifier_flags |= TYPE_SPECIFIER_DOUBLE;
        //result = atof(buffer, 0, 10); 
        //assert(false);
        break;
    case TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT:
        //p_expression_node->type.type_specifier_flags |= TYPE_SPECIFIER_DOUBLE;
        //assert(false);
        break;
    default:
        assert(false);
    }

    return error->code;
}

static bool is_integer_or_floating_constant(enum token_type type)
{
    return type == TK_COMPILER_DECIMAL_CONSTANT ||
        type == TK_COMPILER_OCTAL_CONSTANT ||
        type == TK_COMPILER_HEXADECIMAL_CONSTANT ||
        type == TK_COMPILER_BINARY_CONSTANT ||
        type == TK_COMPILER_DECIMAL_FLOATING_CONSTANT ||
        type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT;
}


struct expression* primary_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
     primary-expression:
      identifier
      constant
      string-literal
      ( expression )
      generic-selection
    */
    struct expression* p_expression_node = NULL;
    try
    {
        if (ctx->current->type == TK_IDENTIFIER)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);

            if (p_expression_node == NULL) throw;
            
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            struct enumerator* p_enumerator = find_enumerator(ctx, ctx->current->lexeme, NULL);
            if (p_enumerator)
            {
                p_expression_node->expression_type = PRIMARY_EXPRESSION_ENUMERATOR;
                p_expression_node->constant_value = p_enumerator->value;
                type_set_int(&p_expression_node->type);
            }
            else
            {
                if (ectx->bConstantExpressionRequired)
                {
                    parser_seterror_with_token(ctx, ctx->current, "not constant");
                    error->code = 1;
                    throw;
                }
                struct declarator* p_declarator = find_declarator(ctx, ctx->current->lexeme, NULL);
                if (p_declarator == NULL)
                {
                    parser_seterror_with_token(ctx, ctx->current, "not found '%s'\n", ctx->current->lexeme);
                    error->code = 1;
                    throw;
                }
                else
                {
                    p_declarator->num_uses++;
                    p_expression_node->declarator = p_declarator;
                    p_expression_node->expression_type = PRIMARY_EXPRESSION_DECLARATOR;

                    assert(p_declarator->type.type_specifier_flags != 0);
                    p_expression_node->type = type_copy(&p_declarator->type);
                }
            }
            parser_match(ctx);
        }
        else if (ctx->current->type == TK_STRING_LITERAL)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_STRING_LITERAL;
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            p_expression_node->type.type_qualifier_flags = TYPE_QUALIFIER_CONST;
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_CHAR;
            
            struct declarator_type* p_declarator_type = calloc(1, sizeof * p_declarator_type);
            struct array_function_type* p_array_function_type = calloc(1, sizeof * p_array_function_type);
            struct direct_declarator_type* p_direct_declarator_type = calloc(1, sizeof * p_direct_declarator_type);

            p_declarator_type->direct_declarator_type = p_direct_declarator_type;
            
            //TODO decode before get size
            p_array_function_type->array_size = strlen(ctx->current->lexeme) - 2 /*2 quotes*/ + 1 /*\0*/;

            p_array_function_type->bIsArray = true;
            list_add(&p_declarator_type->direct_declarator_type->array_function_type_list, p_array_function_type);
            
            p_expression_node->type.declarator_type = p_declarator_type;
            
            //type_print(&p_expression_node->type);
            //printf("TODO warning correct type for literals not implemented yet\n");
            //DECLARE_AND_CREATE_STRUCT_POINTER(direct_declarator_type);
            //p_expression_node->type.declarator_type->direct_declarator_type.array_function_type_list;// = direct_declarator_type;

            //DECLARE_AND_CREATE_STRUCT_POINTER(direct_declarator_type);


            if (ectx->bConstantExpressionRequired)
            {
                parser_seterror_with_token(ctx, ctx->current, "not constant");
                error->code = 1;
                throw;
            }
            else
            {
                parser_match(ctx);
            }
            /*
            string concatenation deveria ser em uma phase anterior
            mas como mantemos as forma do fonte aqui foi uma alternativa
            */
            while (ctx->current &&
                error->code == 0 &&
                ctx->current->type == TK_STRING_LITERAL)
            {
                //TODO montar estring toda
                parser_match(ctx);
            }
        }
        else if (ctx->current->type == TK_CHAR_CONSTANT)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->constant_value = char_constant_to_int(ctx->current->lexeme);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_CHAR_LITERAL;
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            type_set_int(&p_expression_node->type);

            parser_match(ctx);
        }

        else if (ctx->current->type == TK_KEYWORD_TRUE ||
            ctx->current->type == TK_KEYWORD_FALSE)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_PREDEFINED_CONSTANT;
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            p_expression_node->constant_value =
                ctx->current->type == TK_KEYWORD_TRUE ? 1 : 0;

            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_BOOL;
            p_expression_node->type.type_qualifier_flags = 0;
            p_expression_node->type.declarator_type = NULL;


            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD_NULLPTR)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_PREDEFINED_CONSTANT;
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;

            p_expression_node->constant_value = 0;
            
            /*TODO nullptr type*/
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_LONG;
            p_expression_node->type.type_qualifier_flags = 0;
            p_expression_node->type.declarator_type = NULL;


            parser_match(ctx);
        }
        else if (is_integer_or_floating_constant(ctx->current->type))
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->first = ctx->current;
            p_expression_node->last = ctx->current;
            p_expression_node->expression_type = PRIMARY_EXPRESSION_NUMBER;
            convert_to_number(ctx->current, p_expression_node, error);
            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD__GENERIC)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->expression_type = PRIMARY_EXPRESSION_GENERIC;

            if (ectx->bConstantExpressionRequired)
            {
                parser_seterror_with_token(ctx, ctx->current, "not constant");
                error->code = 1;
            }
            else
            {
                
                p_expression_node->generic_selection = generic_selection(ctx, error, ectx);
                p_expression_node->first = p_expression_node->generic_selection->first_token;
                p_expression_node->last = p_expression_node->generic_selection->last_token;

                if (p_expression_node->generic_selection->p_view_selected_expression)
                {
                    p_expression_node->type = type_copy(&p_expression_node->generic_selection->p_view_selected_expression->type);
                }
                else
                {
                    parser_seterror_with_token(ctx, ctx->current, "no match for generic");
                    error->code = 1;
                }
            }
        }
        else if (ctx->current->type == TK_KEYWORD_TYPEID)
        {
            p_expression_node = typeid_expression(ctx, error, ectx);
        }
        else if (ctx->current->type == '(')
        {
            parser_match(ctx);
            p_expression_node = expression(ctx, error, ectx);
            if (error->code != 0)
                throw;
            parser_match_tk(ctx, ')', error);
        }
        else
        {
            parser_seterror_with_token(ctx, ctx->current, "unexpected");
        }
    }
    catch
    {
    }

    /*generic - assoc - ctx:
    generic-association
    generic-assoc-ctx , generic-association
    */
    //assert(p_expression_node&& p_expression_node->first);
    //assert(p_expression_node&& p_expression_node->last);
    
    return p_expression_node;
}



struct argument_expression_list argument_expression_list(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
     argument-expression-list:
      assignment-expression
      argument-expression-ctx , assignment-expression
    */
    struct argument_expression_list list = { 0 };

    struct argument_expression* p_argument_expression = calloc(1, sizeof(struct argument_expression));
    p_argument_expression->expression = assignment_expression(ctx, error, ectx);
    list_add(&list, p_argument_expression);

    while (error->code == 0 &&
        ctx->current->type == ',')
    {
        parser_match(ctx);

        struct argument_expression* p_argument_expression_2 = calloc(1, sizeof * p_argument_expression_2);
        p_argument_expression_2->expression = assignment_expression(ctx, error, ectx);
        list_add(&list, p_argument_expression_2);


    }

    return list;
}

bool first_of_postfix_expression(struct parser_ctx* ctx)
{
    //( type-name )  postfix confunde com (expression) primary
    if (first_of_type_name_ahead(ctx))
        return true; //acho q  nao precisa pq primary tb serve p postif
    return is_first_of_primary_expression(ctx);
}


struct expression* postfix_expression_tail(struct parser_ctx* ctx,
    struct error* error,
    struct expression* p_expression_node,
    struct expression_ctx* ectx)
{
    try
    {
        while (error->code == 0 && ctx->current != NULL)
        {
            if (ctx->current->type == '[')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_ARRAY;
                p_expression_node_new->left = p_expression_node;

                //TODO verificar se eh ponteiro ou array
                // 
                //if (!type_is_function_or_function_pointer(&p_expression_node->type))
                //{
                    //seterror_with_token(ctx, ctx->current,
                  //      "called object is not a function or function pointer");
                    //throw;
                //}
                if (type_is_pointer(&p_expression_node->type))
                {
                    p_expression_node_new->type = get_pointer_content_type(&p_expression_node->type);

                }
                else if (type_is_array(&p_expression_node->type))
                {
                    p_expression_node_new->type = get_array_item_type(&p_expression_node->type);
                }

                parser_match(ctx);
                /*contem a expresao de dentro do  [ ] */
                p_expression_node_new->right = expression(ctx, error, ectx);
                if (error->code != 0) throw;
                parser_match_tk(ctx, ']', error);
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '(')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_FUNCTION_CALL;
                p_expression_node_new->left = p_expression_node;


                if (!type_is_function_or_function_pointer(&p_expression_node->type))
                {
                    parser_seterror_with_token(ctx, ctx->current,
                        "called object is not a function or function pointer");
                    throw;
                }

                p_expression_node_new->type = get_function_return_type(&p_expression_node->type);


                parser_match(ctx);
                if (ctx->current->type != ')')
                {
                    p_expression_node_new->argument_expression_list = argument_expression_list(ctx, error, ectx);
                    if (error->code != 0) throw;
                }
                parser_match_tk(ctx, ')', error);

                //Agora vamos comparar os argumentos...
                struct error local = { 0 };
                if (compare_function_arguments(ctx, &p_expression_node->type, &p_expression_node_new->argument_expression_list, &local) != 0)
                {


                    throw;
                }

                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '.')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_DOT;
                p_expression_node_new->left = p_expression_node;

                parser_match(ctx);
                if (p_expression_node->type.type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
                {
                    struct struct_or_union_specifier* p =
                        find_struct_or_union_specifier(ctx,
                            p_expression_node->type.struct_or_union_specifier->tag_name);
                    if (p)
                    {
                        struct member_declarator* p_member_declarator =
                            find_member_declarator(&p->member_declaration_list, ctx->current->lexeme);
                        if (p_member_declarator)
                        {
                            p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                        }
                        else
                        {

                            parser_seterror_with_token(ctx, ctx->current, "struct member '%s' not found in '%s'",
                                ctx->current->lexeme,
                                p_expression_node->type.struct_or_union_specifier->tag_name);
                        }
                    }
                    else
                    {
                        print_scope(&ctx->scopes);
                    }
                    parser_match_tk(ctx, TK_IDENTIFIER, error);
                }
                else
                {
                    parser_seterror_with_token(ctx, ctx->current, "structure or union required");
                }
                //todo apontar pro nome?
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '->')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_ARROW;
                p_expression_node_new->left = p_expression_node;

                parser_match(ctx);
                if (p_expression_node->type.type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
                {
                    struct struct_or_union_specifier* p = find_struct_or_union_specifier(ctx,
                        p_expression_node->type.struct_or_union_specifier->tag_name);

                    if (p)
                    {
                        struct member_declarator* p_member_declarator =
                            find_member_declarator(&p->member_declaration_list, ctx->current->lexeme);
                        if (p_member_declarator)
                        {
                            p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                        }
                        else
                        {
                            parser_seterror_with_token(ctx,
                                ctx->current,
                                "struct member '%s' not found in '%s'",
                                ctx->current->lexeme, p_expression_node->type.struct_or_union_specifier->tag_name);
                        }
                    }
                    else
                    {
                        print_scope(&ctx->scopes);
                    }
                    parser_match_tk(ctx, TK_IDENTIFIER, error);
                }
                else
                {
                    parser_seterror_with_token(ctx, ctx->current, "structure or union required");
                }
                //todo apontar pro nome?
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '++')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_INCREMENT;
                p_expression_node_new->left = p_expression_node;
                p_expression_node_new->type = type_copy(&p_expression_node->type);
                parser_match(ctx);
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '--')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_DECREMENT;
                p_expression_node_new->left = p_expression_node;
                parser_match(ctx);
                p_expression_node = p_expression_node_new;
            }
            else
            {
                break;
            }
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* postfix_expression_type_name(struct parser_ctx* ctx, struct type_name* p_type_name, struct error* error, struct expression_ctx* ectx)
{
    /*
        ( type-name ) { initializer-ctx }
        ( type-name ) { initializer-ctx , }

        //My extension : if type-name is function then follow is compound-statement
        ( type-name ) compound-statement

    */

    struct expression* p_expression_node = calloc(1, sizeof * p_expression_node);

    assert(p_expression_node->type_name == NULL);
    p_expression_node->first = ctx->current;
    p_expression_node->type_name = p_type_name;
    p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
    bool is_function_type = false;
    if (p_expression_node->type.declarator_type->direct_declarator_type->array_function_type_list.head &&
        p_expression_node->type.declarator_type->direct_declarator_type->array_function_type_list.head->bIsFunction)
    {
        if (p_expression_node->type.declarator_type->direct_declarator_type->declarator_opt == NULL)
        {
            is_function_type = true;
        }
        else
        {
            /*funtion pointer*/
        }
    }
    if (is_function_type)
    {
        p_expression_node->expression_type = POSTFIX_EXPRESSION_FUNCTION_LITERAL;

        struct scope* parameters_scope =
            &p_expression_node->type_name->declarator->direct_declarator->array_function_list.head->function_declarator->parameters_scope;

        scope_list_push(&ctx->scopes, parameters_scope);
        p_expression_node->compound_statement = function_body(ctx, error);
        scope_list_pop(&ctx->scopes);

        p_expression_node->last = p_expression_node->compound_statement->last_token;
    }
    else
    {
        p_expression_node->expression_type = POSTFIX_EXPRESSION_COMPOUND_LITERAL;
        p_expression_node->braced_initializer = braced_initializer(ctx, error);
    }

    p_expression_node = postfix_expression_tail(ctx,
        error,
        p_expression_node,
        ectx);

    return p_expression_node;
}

struct expression* postfix_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
      postfix-expression:
        primary-expression
        postfix-expression [ expression ]
        postfix-expression ( argument-expression-list_opt)
        postfix-expression . identifier
        postfix-expression -> identifier
        postfix-expression ++
        postfix-expression --
        ( type-name ) { initializer-ctx }
        ( type-name ) { initializer-ctx , }

        //My extension : if type-name is function then follow is compound-statement
        ( type-name ) compound-statement

        */
    struct expression* p_expression_node = NULL;
    try
    {
        if (first_of_type_name_ahead(ctx)) //aqui preciso ver se nao eh primary
        {
            assert(false); //este caso esta pegando lá dentro deo cast expression.
            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->first = ctx->current;
            parser_match_tk(ctx, '(', error);
            p_expression_node->type_name = type_name(ctx, error);
            if (error->code != 0) throw;

            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
            parser_match_tk(ctx, ')', error);
            //printf("\n");
            //print_type(&p_expression_node->type);
            bool is_function_type = false;
            if (p_expression_node->type.declarator_type->direct_declarator_type->array_function_type_list.head &&
                p_expression_node->type.declarator_type->direct_declarator_type->array_function_type_list.head->bIsFunction)
            {
                if (p_expression_node->type.declarator_type->direct_declarator_type->declarator_opt == NULL)
                {
                    is_function_type = true;
                }
                else
                {
                    /*funtion pointer*/
                }
            }
            if (is_function_type)
            {
                p_expression_node->expression_type = POSTFIX_EXPRESSION_FUNCTION_LITERAL;
                p_expression_node->compound_statement = compound_statement(ctx, error);
                if (error->code != 0) throw;
                p_expression_node->last = p_expression_node->compound_statement->last_token;
            }
            else
            {
                p_expression_node->expression_type = POSTFIX_EXPRESSION_COMPOUND_LITERAL;
                p_expression_node->braced_initializer = braced_initializer(ctx, error);
                p_expression_node->last = ctx->current;
            }
        }
        else
        {
            p_expression_node = primary_expression(ctx, error, ectx);
        }
        
            p_expression_node = postfix_expression_tail(ctx,
                error,
                p_expression_node,
                ectx);
        
    }
    catch
    {
    }
    return p_expression_node;
}


bool is_first_of_unary_expression(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;
    return  first_of_postfix_expression(ctx) ||
        ctx->current->type == '++' ||
        ctx->current->type == '--' ||
        ctx->current->type == '&' ||
        ctx->current->type == '*' ||
        ctx->current->type == '+' ||
        ctx->current->type == '-' ||
        ctx->current->type == '~' ||
        ctx->current->type == '!' ||
        ctx->current->type == TK_KEYWORD_SIZEOF ||
        ctx->current->type == TK_KEYWORD_HASHOF ||
        ctx->current->type == TK_KEYWORD__ALIGNOF;
}

struct expression* unary_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;


    /*
    unary-expression:
      postfix-expression
      ++ unary-expression
      -- unary-expression

      one of (& * + - ~ !) cast-expression

      sizeof unary-expression
      sizeof ( type-name )
      _Alignof ( type-name )



      */
    struct expression* p_expression_node = NULL;
    try
    {
        if (ctx->current->type == '++' || ctx->current->type == '--')
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);

            if (ctx->current->type == '++')
                new_expression->expression_type = UNARY_EXPRESSION_INCREMENT;
            else
                new_expression->expression_type = UNARY_EXPRESSION_DECREMENT;
            parser_match(ctx);
            new_expression->right = unary_expression(ctx, error, ectx);
            if (error->code != 0) throw;

            new_expression->type = type_copy(&new_expression->right->type);
            p_expression_node = new_expression;
        }
        else if (ctx->current != NULL &&
            (ctx->current->type == '&'
                || ctx->current->type == '*'
                || ctx->current->type == '+'
                || ctx->current->type == '-'
                || ctx->current->type == '~'
                || ctx->current->type == '!'))
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->first = ctx->current;

          

            

            struct token* op_position = ctx->current; //marcar posicao
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->right = cast_expression(ctx, error, ectx);
            if (error->code != 0) throw;
            new_expression->last = new_expression->right->last;
            if (op == '!')
            {
                new_expression->expression_type = UNARY_EXPRESSION_NOT;
                new_expression->constant_value = !new_expression->right->constant_value;
                new_expression->type = type_copy(&new_expression->right->type);
            }
            else if (op == '~')
            {
                new_expression->expression_type = UNARY_EXPRESSION_BITNOT;
                new_expression->constant_value = ~new_expression->right->constant_value;
                new_expression->type = type_copy(&new_expression->right->type);
            }
            else if (op == '-')
            {
                new_expression->expression_type = UNARY_EXPRESSION_NEG;
                new_expression->constant_value = -new_expression->right->constant_value;
                new_expression->type = type_copy(&new_expression->right->type);
            }
            else if (op == '+')
            {
                new_expression->expression_type = UNARY_EXPRESSION_PLUS;
                new_expression->constant_value = +new_expression->right->constant_value;
                new_expression->type = type_copy(&new_expression->right->type);
            }
            else if (op == '*')
            {
                new_expression->expression_type = UNARY_EXPRESSION_CONTENT;
                if (!type_is_pointer(&new_expression->right->type))
                {
                    parser_seterror_with_token(ctx, op_position, "indirection requires pointer operand");
                }
                new_expression->type = get_pointer_content_type(&new_expression->right->type);                
            }
            else if (op == '&')
            {
                new_expression->expression_type = UNARY_EXPRESSION_ADDRESSOF;
                //TODO nao tem como tirar endereco de uma constante
                new_expression->type = get_address_of_type(&new_expression->right->type);
            }
            else
            {
                parser_seterror_with_token(ctx, ctx->current, "invalid token");
                throw;
            }
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_SIZEOF)
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            if (first_of_type_name_ahead(ctx))
            {
                new_expression->expression_type = UNARY_EXPRESSION_SIZEOF_TYPE;
                parser_match_tk(ctx, '(', error);
                new_expression->type_name = type_name(ctx, error);
                new_expression->type = make_type_using_declarator(ctx, new_expression->type_name->declarator);
                parser_match_tk(ctx, ')', error);
                new_expression->constant_value = type_get_sizeof(ctx, &new_expression->type, error);
            }
            else
            {
                bool old = ectx->bConstantExpressionRequired;
                ectx->bConstantExpressionRequired = false;
                new_expression->right = unary_expression(ctx, error, ectx);
                ectx->bConstantExpressionRequired = old;

                if (error->code != 0)
                    throw;

                new_expression->expression_type = UNARY_EXPRESSION_SIZEOF_EXPRESSION;
                new_expression->constant_value = type_get_sizeof(ctx, &new_expression->right->type, error);
            }
            type_set_int(&new_expression->type); //resultado sizeof
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_HASHOF)
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->first = ctx->current;

            parser_match(ctx);

            if (first_of_type_name_ahead(ctx))
            {
                new_expression->expression_type = UNARY_EXPRESSION_HASHOF_TYPE;
                parser_match_tk(ctx, '(', error);
                new_expression->type_name = type_name(ctx, error);
                new_expression->type = make_type_using_declarator(ctx, new_expression->type_name->declarator);

                new_expression->last = ctx->current;

                parser_match_tk(ctx, ')', error);
                new_expression->constant_value = type_get_hashof(ctx, &new_expression->type, error);
            }
            else
            {
                bool old = ectx->bConstantExpressionRequired;
                ectx->bConstantExpressionRequired = false;
                new_expression->right = unary_expression(ctx, error, ectx);
                ectx->bConstantExpressionRequired = old;

                if (error->code != 0)
                    throw;

                new_expression->expression_type = UNARY_EXPRESSION_HASHOF_TYPE;
                new_expression->constant_value = type_get_hashof(ctx, &new_expression->right->type, error);
                new_expression->last = previous_parser_token(ctx->current);
            }

            type_set_int(&new_expression->type); //resultado sizeof
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD__ALIGNOF)
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);


            new_expression->expression_type = UNARY_EXPRESSION_ALIGNOF;
            parser_match_tk(ctx, '(', error);
            new_expression->type_name = type_name(ctx, error);
            new_expression->type = make_type_using_declarator(ctx, new_expression->type_name->declarator);
            parser_match_tk(ctx, ')', error);
            new_expression->constant_value = type_get_sizeof(ctx, &new_expression->type, error);


            type_set_int(&new_expression->type); //resultado sizeof
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD__ALIGNAS)
        {
        }
        else //if (is_first_of_primary_expression(ctx))
        {
            p_expression_node = postfix_expression(ctx, error, ectx);
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* cast_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{



    /*
     cast-expression:
      unary-expression
      ( type-name ) cast-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        if (error->code != 0)
            throw;


        if (first_of_type_name_ahead(ctx))
        {

            p_expression_node = calloc(1, sizeof * p_expression_node);
            p_expression_node->first = ctx->current;
            p_expression_node->expression_type = CAST_EXPRESSION;
            parser_match_tk(ctx, '(', error);
            p_expression_node->type_name = type_name(ctx, error);
            if (error->code != 0)
                throw;

            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
            //type_set_int(&ectx->result_type);
            //print_type_name(p_cast_expression->type_name);
            parser_match_tk(ctx, ')', error);
            //struct token_list r = copy_replacement_list(&l);
            //pop_f
            if (ctx->current->type == '{')
            {
                // Achar que era um cast_expression foi um engano...
                // porque apareceu o { então é compound literal que eh postfix.
                struct expression* new_expression = postfix_expression_type_name(ctx, p_expression_node->type_name, error, ectx);

                new_expression->first = p_expression_node->first;

                free(p_expression_node);
                p_expression_node = new_expression;
            }
            else
            {
                p_expression_node->left = cast_expression(ctx, error, ectx);
                p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
            }
            //token_list_destroy(&ectx->type);
            //ectx->type = r;
            //print_list(&ectx->type);
        }
        else if (is_first_of_unary_expression(ctx))
        {
            p_expression_node = unary_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;
        }
        else
        {
            parser_seterror_with_token(ctx, ctx->current, "unexpected");
        }
    }
    catch
    {
    }
    if (p_expression_node && ctx->current)
      p_expression_node->last = previous_parser_token(ctx->current);
    return p_expression_node;
}

struct expression* multiplicative_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{


    /*
     multiplicative-expression:
    cast-expression
    multiplicative-expression * cast-expression
    multiplicative-expression / cast-expression
    multiplicative-expression % cast-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        if (error->code != 0)
            throw;

        p_expression_node = cast_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '*' ||
                ctx->current->type == '/' ||
                ctx->current->type == '%'))
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->left = p_expression_node;
            new_expression->right = cast_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            if (op == '*')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MULT;
                new_expression->constant_value = (new_expression->left->constant_value * new_expression->right->constant_value);
            }
            else if (op == '/')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_DIV;
                if (new_expression->right->constant_value != 0)
                {
                    new_expression->constant_value = (new_expression->left->constant_value / new_expression->right->constant_value);
                }
                else
                {
                }
            }
            else if (op == '%')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MOD;
                if (new_expression->right->constant_value != 0)
                    new_expression->constant_value = (new_expression->left->constant_value % new_expression->right->constant_value);
            }

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }
    return p_expression_node;
}

struct expression* additive_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
     additive-expression:
    multiplicative-expression
    additive-expression + multiplicative-expression
    additive-expression - multiplicative-expression
    */

    struct expression* p_expression_node = NULL;
    struct expression* new_expression = NULL;

    try
    {
        if (error->code != 0)
            throw;
        p_expression_node = multiplicative_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;


        while (ctx->current != NULL &&
            (ctx->current->type == '+' ||
                ctx->current->type == '-'))
        {
            struct token* operator_position = ctx->current;

            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->left = p_expression_node;

            static int count = 0;
            count++;
            new_expression->right = multiplicative_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            if (!type_is_scalar(&new_expression->left->type))
            {
                parser_seterror_with_token(ctx, operator_position, "left operator is not scalar");
            }
            if (!type_is_scalar(&new_expression->right->type))
            {
                parser_seterror_with_token(ctx, operator_position, "right operator is not scalar");
            }

            if (op == '+')
            {
                new_expression->expression_type = ADDITIVE_EXPRESSION_PLUS;
                new_expression->constant_value = (new_expression->left->constant_value + new_expression->right->constant_value);


            }
            else if (op == '-')
            {
                new_expression->expression_type = ADDITIVE_EXPRESSION_MINUS;
                new_expression->constant_value = (new_expression->left->constant_value - new_expression->right->constant_value);
            }

            if (type_is_pointer(&new_expression->left->type))
            {
                /*
                 pointer +- integer
                */
                if (type_is_integer(&new_expression->right->type))
                {
                    new_expression->type = type_copy(&new_expression->left->type);
                }
                else if (type_is_pointer_or_array(&new_expression->right->type))
                {
                    //tem que ser do mesmo tipo..
                    if (op == '-')
                    {
                        if (type_is_same(&new_expression->left->type, &new_expression->right->type, true))
                        {
                            type_set_int(&new_expression->type);//
                        }
                        else
                        {
                            parser_seterror_with_token(ctx, ctx->current, "incompatible types +-");
                        }
                    }
                    else
                    {
                        parser_seterror_with_token(ctx, ctx->current, "invalid operands of types");
                    }
                }
                else
                {
                    parser_seterror_with_token(ctx, ctx->current, "sorry not implemented yet...");
                }
            }
            else
            {
                struct error localerror = { 0 };
                new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
                if (localerror.code != 0)
                {
                    parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                    throw;
                }
            }

            if (error->code != 0)
                throw;

            p_expression_node = new_expression;
            new_expression = NULL; /*MOVED*/
        }
    }
    catch
    {
        if (p_expression_node)
        {
            //expression_node_delete(p_expression_node);
        }
        if (new_expression)
        {
            //expression_node_delete(p_expression_node);
        }
    }



    return p_expression_node;
}

struct expression* shift_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
     shift-expression:
       additive-expression
       shift-expression << additive-expression
       shift-expression >> additive-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = additive_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '>>' ||
                ctx->current->type == '<<'))
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->left = p_expression_node;
            new_expression->right = multiplicative_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            if (op == '>>')
            {
                new_expression->expression_type = SHIFT_EXPRESSION_RIGHT;
                new_expression->constant_value = (new_expression->left->constant_value >> new_expression->right->constant_value);
            }
            else if (op == '<<')
            {
                new_expression->expression_type = SHIFT_EXPRESSION_LEFT;
                new_expression->constant_value = (new_expression->left->constant_value << new_expression->right->constant_value);
            }

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* relational_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
    relational-expression:
    shift-expression
    relational-expression < shift-expression
    relational-expression > shift-expression
    relational-expression <= shift-expression
    relational-expression >= shift-expression
    */
    struct expression* p_expression_node = shift_expression(ctx, error, ectx);
    if (error->code != 0)
        return NULL;

    while (ctx->current != NULL &&
        (ctx->current->type == '>' ||
            ctx->current->type == '<' ||
            ctx->current->type == '>=' ||
            ctx->current->type == '<='))
    {
        struct expression* new_expression = calloc(1, sizeof * new_expression);
        enum token_type op = ctx->current->type;
        parser_match(ctx);
        new_expression->left = p_expression_node;
        new_expression->right = shift_expression(ctx, error, ectx);
        if (error->code != 0)
            break;

        if (op == '>')
        {
            new_expression->expression_type = RELATIONAL_EXPRESSION_BIGGER_THAN;
            new_expression->constant_value = (new_expression->left->constant_value > new_expression->right->constant_value);
        }
        else if (op == '<')
        {
            new_expression->expression_type = RELATIONAL_EXPRESSION_LESS_THAN;
            new_expression->constant_value = (new_expression->left->constant_value < new_expression->right->constant_value);
        }
        else if (op == '>=')
        {
            new_expression->expression_type = RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN;
            new_expression->constant_value = (new_expression->left->constant_value >= new_expression->right->constant_value);
        }
        else if (op == '<=')
        {
            new_expression->expression_type = RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN;
            new_expression->constant_value = (new_expression->left->constant_value <= new_expression->right->constant_value);
        }

        type_set_int(&new_expression->type);

        p_expression_node = new_expression;
    }
    return p_expression_node;
}

struct expression* equality_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
      equality-expression:
       relational-expression
       equality-expression == relational-expression
       equality-expression != relational-expression
    */
    /*
    * Equality operators
    One of the following shall hold:
    — both operands have arithmetic type;
    — both operands are pointers to qualified or unqualified versions of compatible types;
    — one operand is a pointer to an object type and the other is a pointer to a qualified or unqualified
    version of void; or
    — one operand is a pointer and the other is a null pointer constant.
    */
    struct expression* p_expression_node = relational_expression(ctx, error, ectx);
    if (error->code != 0)
        return NULL;


    while (ctx->current != NULL &&
        (ctx->current->type == '==' ||
            ctx->current->type == '!='))
    {
        struct expression* new_expression = calloc(1, sizeof * new_expression);
        enum token_type op = ctx->current->type;
        parser_match(ctx);
        new_expression->left = p_expression_node;
        new_expression->right = relational_expression(ctx, error, ectx);
        if (error->code != 0)
            break;

        if (op == '==')
        {
            new_expression->expression_type = EQUALITY_EXPRESSION_EQUAL;

            if (new_expression->left->expression_type == TYPEID_EXPRESSION_TYPE ||
                new_expression->right->expression_type == TYPEID_EXPRESSION_TYPE)
            {
                new_expression->constant_value = type_is_same(&new_expression->left->type, &new_expression->right->type, true);
            }
            else
            {
                new_expression->constant_value = (new_expression->left->constant_value == new_expression->right->constant_value);
            }
        }
        else if (op == '!=')
        {
            new_expression->expression_type = EQUALITY_EXPRESSION_EQUAL;

            if (new_expression->left->expression_type == TYPEID_EXPRESSION_TYPE ||
                new_expression->right->expression_type == TYPEID_EXPRESSION_TYPE)
            {
                new_expression->constant_value = !type_is_same(&new_expression->left->type, &new_expression->right->type, true);
            }
            else
            {
                new_expression->constant_value = (new_expression->left->constant_value != new_expression->right->constant_value);
            }
        }
        else
        {
            assert(false);
        }
        type_set_int(&new_expression->type);
        p_expression_node = new_expression;
    }
    return p_expression_node;
}

struct expression* and_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
     AND-expression:
      equality-expression
      AND-expression & equality-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = equality_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '&'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = AND_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = equality_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            new_expression->constant_value = (new_expression->left->constant_value & new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* exclusive_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
     exclusive-OR-expression:
      AND-expression
     exclusive-OR-expression ^ AND-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = and_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '^'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = EXCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = and_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            new_expression->constant_value = (new_expression->left->constant_value ^ new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* inclusive_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
    inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = exclusive_or_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '|'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);

            new_expression->expression_type = INCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = exclusive_or_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            new_expression->constant_value = (new_expression->left->constant_value | new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }
            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* logical_and_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
    logical-AND-expression:
     inclusive-OR-expression
     logical-AND-expression && inclusive-OR-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = inclusive_or_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '&&'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = INCLUSIVE_AND_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = inclusive_or_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            new_expression->constant_value = (new_expression->left->constant_value && new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }
            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* logical_or_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    /*
      logical-OR-expression:
       logical-AND-expression
       logical-OR-expression || logical-AND-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        if (error->code != 0)
            throw;

        p_expression_node = logical_and_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '||'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = LOGICAL_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = logical_and_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            //TODO converter cada lado para bool


            new_expression->constant_value = (new_expression->left->constant_value || new_expression->right->constant_value);

            struct error localerror = { 0 };
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, &localerror);
            if (localerror.code != 0)
            {
                parser_seterror_with_token(ctx, ctx->current, "%s", localerror.message);
                throw;
            }
            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}



struct expression* assignment_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
    assignment-expression:
       conditional-expression
       unary-expression assignment-operator assignment-expression
       */
       /*
          assignment-operator: one of
          = *= /= %= += -= <<= >>= &= ^= |=
       */
       //aqui eh duvidoso mas conditional faz a unary tb.
       //a diferenca q nao eh qualquer expressao
       //que pode ser de atribuicao
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = conditional_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '=' ||
                ctx->current->type == '*=' ||
                ctx->current->type == '/=' ||
                ctx->current->type == '+=' ||
                ctx->current->type == '-=' ||
                ctx->current->type == '<<=' ||
                ctx->current->type == '>>=' ||
                ctx->current->type == '&=' ||
                ctx->current->type == '^=' ||
                ctx->current->type == '|='))
        {
            parser_match(ctx);

            if (ectx->bConstantExpressionRequired)
            {
                parser_seterror_with_token(ctx, ctx->current, "assignment is not an constant expression");
                error->code = 1;
                throw;
            }

            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->expression_type = ASSIGNMENT_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = assignment_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;


            new_expression->type = type_copy(&new_expression->right->type);

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }
    return p_expression_node;
}

struct expression* expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*expression:
      assignment-expression
      expression, assignment-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = assignment_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        struct expression* p_expression_node_tail = p_expression_node;
        while (ctx->current->type == ',')
        {
            parser_match(ctx);
            p_expression_node_tail->right = expression(ctx, error, ectx);
            if (error->code != 0)
                throw;
            p_expression_node_tail = p_expression_node_tail->right;
        }
    }
    catch
    {
    }
    return p_expression_node;
}

bool is_first_of_conditional_expression(struct parser_ctx* ctx)
{
    return is_first_of_unary_expression(ctx) ||
        is_first_of_primary_expression(ctx);
}

struct expression* conditional_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    /*
      conditional-expression:
      logical-OR-expression
      logical-OR-expression ? expression : conditional-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = logical_or_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;


        if (ctx->current && ctx->current->type == '?')
        {
            //TODO VERIFICAR
            parser_match(ctx);
            if (p_expression_node->constant_value)
            {
                p_expression_node->left = expression(ctx, error, ectx);
                if (error->code != 0)
                    throw;

                parser_match(ctx); //:
                struct expression_ctx temp = { 0 };
                conditional_expression(ctx, error, &temp);
            }
            else
            {
                struct expression_ctx temp = { 0 };
                p_expression_node->left = expression(ctx, error, &temp);
                if (error->code != 0)
                    throw;

                parser_match(ctx); //:
                p_expression_node->right = conditional_expression(ctx, error, ectx);
                if (error->code != 0)
                    throw;
            }
        }
    }
    catch
    {
    }
    return p_expression_node;
}

struct expression* constant_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx)
{
    if (error->code != 0)
        return NULL;

    ectx->bConstantExpressionRequired = true;
    return conditional_expression(ctx, error, ectx);
}


#ifdef TEST

struct type type_make_using_string(const char* expr)
{
    struct error error = { 0 };

    struct token_list list = tokenizer(expr, "", 0, TK_FLAG_NONE, &error);
    struct preprocessor_ctx preprocessor_ctx = { 0 };
    struct token_list input = preprocessor(&preprocessor_ctx, &list, 0, &error);


    struct scope file_scope = { 0 };
    struct parser_ctx parser_ctx = { 0 };
    scope_list_push(&parser_ctx.scopes, &file_scope);
    parser_ctx.input_list = input;
    parser_ctx.current = parser_ctx.input_list.head;
    parser_skip_blanks(&parser_ctx);

    struct expression_ctx expression_ctx = { .bConstantExpressionRequired = true };
    struct expression* expression = conditional_expression(&parser_ctx, &error, &expression_ctx);
    assert(error.code == 0);
    return expression->type;
}

int test_constant_expression(const char* expr, int result)
{
    struct error error = { 0 };

    struct token_list list = tokenizer(expr, "", 0, TK_FLAG_NONE, &error);
    struct preprocessor_ctx preprocessor_ctx = { 0 };
    struct token_list input = preprocessor(&preprocessor_ctx, &list, 0, &error);


    struct scope file_scope = { 0 };
    struct parser_ctx parser_ctx = { 0 };
    scope_list_push(&parser_ctx.scopes, &file_scope);
    parser_ctx.input_list = input;
    parser_ctx.current = parser_ctx.input_list.head;
    parser_skip_blanks(&parser_ctx);

    struct expression_ctx expression_ctx = { .bConstantExpressionRequired = true };
    struct expression* expression = constant_expression(&parser_ctx, &error, &expression_ctx);

    return expression->constant_value == result ? 0 : 1;
}

void test_compiler_constant_expression()
{
    assert(test_constant_expression("sizeof(char)", sizeof(char)) == 0);
    assert(test_constant_expression("'A'", 'A') == 0);
    assert(test_constant_expression("'ab'", 'ab') == 0);
    assert(test_constant_expression("true", true) == 0);
    assert(test_constant_expression("false", false) == 0);
    assert(test_constant_expression("10+2*3/4", 10 + 2 * 3 / 4) == 0);
    assert(test_constant_expression("sizeof(int)", sizeof(int)) == 0);
    assert(test_constant_expression("sizeof(double)", sizeof(double)) == 0);
    assert(test_constant_expression("sizeof(unsigned long)", sizeof(unsigned long)) == 0);
}

void is_pointer_test()
{
    struct type t1 = type_make_using_string("(int *)0");
    assert(type_is_pointer(&t1));

    struct type t2 = type_make_using_string("1");
    assert(type_is_integer(&t2));

    struct type t3 = type_make_using_string("(void (*)(void))0");
    assert(type_is_pointer(&t3));
}

void sizeoftest1()
{
    const char* source =
        "char a[10];"
        "static_assert(sizeof(a) == 10);"
        "char *p[10];"
        "static_assert(sizeof(p) == 40);"
        "static_assert(sizeof(int) == 4);"
        "static_assert(sizeof(long) == 4);"
        "static_assert(sizeof(char) == 1);"
        "static_assert(sizeof(short) == 4);"
        "static_assert(sizeof(unsigned int) == 4);"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}


void sizeof_struct_test()
{
    const char* source =
        "struct X { int i; char c; };"
        "_Static_assert(sizeof(struct X) == 4);"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code != 0);
}

static int expression_type(const char* expression, const char* result)
{
    char source[200] = { 0 };
    snprintf(source, sizeof source, "_Static_assert(typeid(%s) == typeid(%s));", expression, result);
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    return error.code;
}

void test_expressions()
{
    const char* source =
        "\n"
        "\n"
        "struct X {\n"
        "    int i;\n"
        "};\n"
        "\n"
        "struct Y {\n"
        "    double d;\n"
        "};\n"
        "\n"
        "enum E { A = 1 };\n"
        "enum E e1;\n"
        "struct X* F() { return 0; }\n"
        "\n"
        "int main()\n"
        "{\n"
        "    enum E { B } e2; \n"
        "    static_assert(typeid(e2) == typeid(enum E));\n"
        "    static_assert(typeid(e2) != typeid(e1));\n"
        "\n"
        "    struct X x;\n"
        "    struct Y y;\n"
        "\n"
        "    static_assert(typeid(x) == typeid(struct X));\n"
        "    static_assert(typeid(x) != typeid(struct Y));\n"
        "\n"
        "    static_assert(typeid(int(double)) != typeid(int()));\n"
        "    int aa[10];\n"
        "\n"
        "    static_assert(typeid(*F()) == typeid(struct X));\n"
        "    static_assert(typeid(&aa) == typeid(int(*)[10]));\n"
        "\n"
        "    int* p = 0;\n"
        "    static_assert(typeid(*(p + 1)) == typeid(int));\n"
        "    \n"
        "    static_assert(1 == typeid(int));\n"
        "\n"
        "    static_assert(typeid(main) == typeid(int()));\n"
        "    \n"
        "    \n"
        "    static_assert(typeid(main) != typeid(int(double)));\n"
        "    static_assert(typeid(main) != typeid(int));\n"
        "\n"
        "\n"
        "    struct X x;\n"
        "    enum E e;\n"
        "    static_assert(typeid(e) == typeid(enum E));\n"
        "    static_assert(typeid(x) == typeid(struct X));\n"
        "    static_assert(typeid(e) != typeid(struct X));\n"
        "\n"
        "    \n"
        "        \n"
        "    static_assert(1L == typeid(long));\n"
        "    static_assert(1UL == typeid(unsigned long));\n"
        "    static_assert(1ULL == typeid(unsigned long long));\n"
        "    static_assert(A == typeid(int));\n"
        "    static_assert(1.0 == typeid(double));\n"
        "    static_assert(1.0f == typeid(float));\n"
        "    static_assert(1.0L == typeid(long double));\n"
        "\n"
        "    static_assert(typeid((((int*)0) + 1))  == typeid(int*));\n"
        "    static_assert(typeid(*(((int*)0) + 1))  == typeid(int));\n"
        "  \n"
        "}\n"
        "\n"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);    
}

void literal_string_type()
{
    const char* source =
        "\n"
        "    static_assert(typeid(\"A\") == typeid(const char [2]);\n"
        "    static_assert(typeid(\"A\\n\") != typeid(const char [3]));\n"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}
void type_suffix_test()
{

    const char* source =
        "_Static_assert("
        "1 == typeid(int) && "
        "1L == typeid(long) && "
        "1LL == typeid(long long)  && "
        "1U == typeid(unsigned int) && "
        "1ULL == typeid(unsigned long long) &&"
        "1 == typeid(int) && "
        "1l == typeid(long) && "
        "1ll == typeid(long long)  && "
        "1u == typeid(unsigned int) && "
        "1ull == typeid(unsigned long long) && "
        "0x1 == typeid(int) && "
        "0x1L == typeid(long) && "
        "0x1LL == typeid(long long)  && "
        "0x1U == typeid(unsigned int) && "
        "0x1ULL == typeid(unsigned long long) &&"
        "0x1 == typeid(int)  &&"
        "0x1l == typeid(long)  && "
        "0x1ll == typeid(long long)  && "
        "0x1u == typeid(unsigned int) && "
        "0x1ull == typeid(unsigned long long) &&"
        "0b1 == typeid(int)  &&"
        "0b1L == typeid(long) && "
        "0b1LL == typeid(long long) &&  "
        "0b1U == typeid(unsigned int) && "
        "0b1ULL == typeid(unsigned long long) &&"
        "0b1l == typeid(long) && "
        "0b1ll == typeid(long long)   &&"
        "0b1ul == typeid(unsigned long)  &&"
        "0b1ull == typeid(unsigned long long) && "
        "1.0f == typeid(float) && "
        "1.0 == typeid(double) "        
        ");"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void type_test()
{
    const char* source =
        "int * p = 0;"
        "_Static_assert(typeid(*(p + 1)) == typeid(int));"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void digit_separator_test()
{
    const char* source =
        "_Static_assert(1'00'00 == 10000);"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void numbers_test()
{
    const char* source =
        "#if 0xA1 == 161\n"
        "_Static_assert(0xA1 == 161); \n"
        "#endif"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void binary_digits_test()
{
    const char* source =
        "_Static_assert(0b101010 == 42);"
        "_Static_assert(0b1010'10 == 42);"
        "_Static_assert(052 == 42);"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void is_arithmetic_test()
{
    const char* source =
        "int (*d1)(int i)[10];"
        "int d2;"
        "long double d3;"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);

    struct declarator* d1 = ast.declaration_list.head->init_declarator_list.head->declarator;
    struct declarator* d2 = ast.declaration_list.head->next->init_declarator_list.head->declarator;
    struct declarator* d3 = ast.declaration_list.head->next->next->init_declarator_list.head->declarator;



    assert(!type_is_array(&d1->type));
    assert(type_is_pointer(&d1->type));
    assert(type_is_scalar(&d1->type));
    assert(!type_is_integer(&d1->type));

    assert(!type_is_array(&d2->type));
    assert(!type_is_pointer(&d2->type));
    assert(type_is_arithmetic(&d2->type));
    assert(type_is_integer(&d2->type));

    assert(!type_is_integer(&d3->type));

}

void type_is_pointer_test()
{
    const char* source =
        "const char* d1[10];\n"
        "char* (*f)(void);\n"
        ;
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);

    struct declarator* d1 = ast.declaration_list.head->init_declarator_list.head->declarator;
    struct declarator* d2 = ast.declaration_list.head->next->init_declarator_list.head->declarator;

    assert(!type_is_pointer(&d1->type));
    assert(type_is_array(&d1->type));

    assert(type_is_pointer(&d2->type));
    assert(type_is_function_or_function_pointer(&d2->type));

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
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}
#endif


/*
  For performance reasons we will separate expression from preprocessor from compiler.
*/


#ifdef _WIN32
#undef assert
#define assert _ASSERTE
#endif

/*contexto expressoes preprocessador*/
struct pre_expression_ctx
{
    /*todas expressões do preprocessador sao calculadas com long long*/
    long long value;
};


void pre_postfix_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_cast_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_multiplicative_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_unary_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_additive_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_shift_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_relational_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_equality_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_and_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_exclusive_or_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_inclusive_or_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_logical_and_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_logical_or_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_conditional_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);
void pre_conditional_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx);


void pre_seterror_with_token(struct preprocessor_ctx* ctx, struct token* p_token, const char* fmt, ...);

/*
* preprocessor uses long long
*/
int ppnumber_to_longlong(struct token* token, long long* result, struct error* error)
{

    error;

    /*copia removendo os separadores*/
    //um dos maiores buffer necessarios seria 128 bits binario...
    //0xb1'1'1.... 
    int c = 0;
    char buffer[128 * 2 + 4] = { 0 };
    const char* s = token->lexeme;
    while (*s)
    {
        if (*s != '\'')
        {
            buffer[c] = *s;
            c++;
        }
        s++;
    }

    if (buffer[0] == '0' &&
        buffer[1] == 'x')
    {
        //hex
        *result = strtoll(buffer + 2, 0, 16);
    }
    else if (buffer[0] == '0' &&
        buffer[1] == 'b')
    {
        //binario
        *result = strtoll(buffer + 2, 0, 2);
    }
    else if (buffer[0] == '0')
    {
        //octal
        *result = strtoll(buffer, 0, 8);
    }
    else
    {
        //decimal
        *result = strtoll(buffer, 0, 10);
    }

    //TOTO erro para floatu
    return error->code;
}

struct token* pre_match(struct preprocessor_ctx* ctx)
{
    ctx->current = ctx->current->next;

    while (ctx->current && token_is_blank(ctx->current))
    {
        ctx->current = ctx->current->next;
    }

    return ctx->current;
}

void pre_primary_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
     primary-expression:
      identifier
      constant
      string-literal
      ( expression )
      generic-selection
    */
    try
    {
        if (ctx->current->type == TK_CHAR_CONSTANT)
        {
            const char* p = ctx->current->lexeme + 1;
            ectx->value = 0;
            while (*p != '\'')
            {
                ectx->value = ectx->value * 256 + *p;
                p++;
            }

            pre_match(ctx);
        }
        else if (ctx->current->type == TK_PPNUMBER)
        {
            ppnumber_to_longlong(ctx->current, &ectx->value, error);
            pre_match(ctx);
        }
        else if (ctx->current->type == '(')
        {
            pre_match(ctx);
            pre_expression(ctx, error, ectx);
            if (error->code != 0) throw;
            if (ctx->current && ctx->current->type != ')')
            {
                pre_seterror_with_token(ctx, ctx->current, "expected )");
                throw;
            }
            pre_match(ctx);
        }
        else
        {
            seterror(error, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            throw;

        }
    }
    catch
    {
    }
}




void pre_postfix_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
      postfix-expression:
        primary-expression
        postfix-expression [ expression ]
        postfix-expression ( argument-expression-list_opt)
        postfix-expression . identifier
        postfix-expression -> identifier
        postfix-expression ++
        postfix-expression --
        ( type-name ) { initializer-ctx }
        ( type-name ) { initializer-ctx , }

        //My extension : if type-name is function then follow is compound-statement
        ( type-name ) compound-statement

        */
    try
    {
        pre_primary_expression(ctx, error, ectx);
        if (error->code != 0) throw;
    }
    catch
    {
    }
}


void pre_unary_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
    unary-expression:
      postfix-expression
      ++ unary-expression
      -- unary-expression

      one of (& * + - ~ !) cast-expression

      sizeof unary-expression
      sizeof ( type-name )
      _Alignof ( type-name )
      */
    try
    {
        if (ctx->current->type == '++' || ctx->current->type == '--')
        {
            seterror(error, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            throw;
        }
        else if (ctx->current != NULL &&
            (ctx->current->type == '&'
                || ctx->current->type == '*'
                || ctx->current->type == '+'
                || ctx->current->type == '-'
                || ctx->current->type == '~'
                || ctx->current->type == '!'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            pre_cast_expression(ctx, error, ectx);
            if (error->code != 0) throw;

            if (op == '!')
                ectx->value = !ectx->value;
            else if (op == '~')
                ectx->value = ~ectx->value;
            else if (op == '-')
                ectx->value = -ectx->value;
            else if (op == '+')
                ectx->value = +ectx->value;
            else if (op == '*')
            {
                seterror(error, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            }
            else if (op == '&')
            {
                seterror(error, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            }
            else
            {
                seterror(error, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            }
        }
        else 
        {
            pre_postfix_expression(ctx, error, ectx);
        }
    }
    catch
    {
    }
}

void pre_cast_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
     cast-expression:
      unary-expression
      ( type-name ) cast-expression
    */
    pre_unary_expression(ctx, error, ectx);    
}

void pre_multiplicative_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
     multiplicative-expression:
    cast-expression
    multiplicative-expression * cast-expression
    multiplicative-expression / cast-expression
    multiplicative-expression % cast-expression
    */
    try
    {
        pre_cast_expression(ctx, error, ectx);
        if (error->code != 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '*' ||
                ctx->current->type == '/' ||
                ctx->current->type == '%'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_cast_expression(ctx, error, ectx);
            if (error->code != 0) throw;

            if (op == '*')
            {
                ectx->value = (left_value * ectx->value);
            }
            else if (op == '/')
            {
                ectx->value = (left_value / ectx->value);
            }
            else if (op == '%')
            {
                ectx->value = (left_value % ectx->value);
            }
        }
    }
    catch
    {
    }
}

void pre_additive_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
     additive-expression:
    multiplicative-expression
    additive-expression + multiplicative-expression
    additive-expression - multiplicative-expression
    */
    try
    {
        pre_multiplicative_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '+' ||
                ctx->current->type == '-'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            if (ctx->current == NULL)
            {
                seterror(error, "unexpected end of file");
                throw;
            }
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, error, ectx);
            if (error->code != 0) throw;

            if (op == '+')
            {
                ectx->value = left_value + ectx->value;
            }
            else if (op == '-')
            {
                ectx->value = left_value - ectx->value;
            }
        }
    }
    catch
    {
    }
}

void pre_shift_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
     shift-expression:
       additive-expression
       shift-expression << additive-expression
       shift-expression >> additive-expression
    */
    try
    {
        pre_additive_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '>>' ||
                ctx->current->type == '<<'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, error, ectx);
            if (error->code != 0)
                throw;

            if (op == '>>')
            {
                ectx->value = left_value >> ectx->value;
            }
            else if (op == '<<')
            {
                ectx->value = left_value << ectx->value;
            }
        }
    }
    catch
    {
    }
}

void pre_relational_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
    relational-expression:
    shift-expression
    relational-expression < shift-expression
    relational-expression > shift-expression
    relational-expression <= shift-expression
    relational-expression >= shift-expression
    */
    try
    {
        pre_shift_expression(ctx, error, ectx);
        if (error->code != 0)
            throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '>' ||
                ctx->current->type == '<' ||
                ctx->current->type == '>=' ||
                ctx->current->type == '<='))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, error, ectx);
            if (error->code != 0) throw;

            if (op == '>')
            {
                ectx->value = left_value > ectx->value;
            }
            else if (op == '<')
            {
                ectx->value = left_value < ectx->value;
            }
            else if (op == '>=')
            {
                ectx->value = left_value >= ectx->value;
            }
            else if (op == '<=')
            {
                ectx->value = left_value <= ectx->value;
            }
        }
    }
    catch
    {
    }
}

void pre_equality_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
      equality-expression:
       relational-expression
       equality-expression == relational-expression
       equality-expression != relational-expression
    */
    /*
    * Equality operators
    One of the following shall hold:
    — both operands have arithmetic type;
    — both operands are pointers to qualified or unqualified versions of compatible types;
    — one operand is a pointer to an object type and the other is a pointer to a qualified or unqualified
    version of void; or
    — one operand is a pointer and the other is a null pointer constant.
    */
    try
    {
        pre_relational_expression(ctx, error, ectx);
        if (error->code != 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '==' ||
                ctx->current->type == '!='))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx, error, ectx);
            if (error->code != 0) throw;

            if (op == '==')
            {
                ectx->value = left_value == ectx->value;
            }
            else if (op == '!=')
            {
                ectx->value = left_value != ectx->value;
            }            
        }
    }
    catch
    {
    }
}

void pre_and_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
     AND-expression:
      equality-expression
      AND-expression & equality-expression
    */
    try
    {
        pre_equality_expression(ctx, error, ectx);
        if (error->code != 0) throw;
        while (ctx->current != NULL &&
            (ctx->current->type == '&'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_equality_expression(ctx, error, ectx);
            if (error->code != 0) throw;
            ectx->value = left_value & ectx->value;
        }
    }
    catch
    {
    }
}

void pre_exclusive_or_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
     exclusive-OR-expression:
      AND-expression
     exclusive-OR-expression ^ AND-expression
    */
    try
    {
        pre_and_expression(ctx, error, ectx);
        if (error->code != 0) throw;
        while (ctx->current != NULL &&
            (ctx->current->type == '^'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_and_expression(ctx, error, ectx);
            if (error->code != 0) throw;
            ectx->value = left_value ^ ectx->value;
        }
    }
    catch
    {
    }
}

void pre_inclusive_or_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
    inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */
    try
    {
        pre_exclusive_or_expression(ctx, error, ectx);
        if (error->code != 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '|'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_exclusive_or_expression(ctx, error, ectx);
            if (error->code != 0) throw;
            ectx->value = left_value | ectx->value;
        }
    }
    catch
    {
    }
}

void pre_logical_and_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
    logical-AND-expression:
     inclusive-OR-expression
     logical-AND-expression && inclusive-OR-expression
    */
    try
    {
        pre_inclusive_or_expression(ctx, error, ectx);
        if (error->code != 0) throw;
        while (ctx->current != NULL &&
            (ctx->current->type == '&&'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_inclusive_or_expression(ctx, error, ectx);
            if (error->code != 0) throw;
            ectx->value = left_value && ectx->value;
        }
    }
    catch
    {
    }
}

void pre_logical_or_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
      logical-OR-expression:
       logical-AND-expression
       logical-OR-expression || logical-AND-expression
    */
    try
    {
        pre_logical_and_expression(ctx, error, ectx);
        if (error->code != 0) throw;
        while (ctx->current != NULL &&
            (ctx->current->type == '||'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_logical_and_expression(ctx, error, ectx);
            if (error->code != 0) throw;
            ectx->value = left_value || ectx->value;

        }
    }
    catch
    {
    }
}



void pre_assignment_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
    assignment-expression:
       conditional-expression
       unary-expression assignment-operator assignment-expression
       */
       /*
          assignment-operator: one of
          = *= /= %= += -= <<= >>= &= ^= |=
       */
       //aqui eh duvidoso mas conditional faz a unary tb.
       //a diferenca q nao eh qualquer expressao
       //que pode ser de atribuicao
    try
    {
        pre_conditional_expression(ctx, error, ectx);

        if (error->code != 0) throw;
        while (ctx->current != NULL &&
            (ctx->current->type == '=' ||
                ctx->current->type == '*=' ||
                ctx->current->type == '/=' ||
                ctx->current->type == '+=' ||
                ctx->current->type == '-=' ||
                ctx->current->type == '<<=' ||
                ctx->current->type == '>>=' ||
                ctx->current->type == '&=' ||
                ctx->current->type == '^=' ||
                ctx->current->type == '|='))
        {
            seterror(error, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            throw;
        }
    }
    catch
    {
    }
}

void pre_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*expression:
      assignment-expression
      expression, assignment-expression
    */
    try
    {
        pre_assignment_expression(ctx, error, ectx);
        if (error->code != 0) throw;
        while (ctx->current->type == ',')
        {
            pre_match(ctx);
            pre_expression(ctx, error, ectx);
            if (error->code != 0) throw;
        }
    }
    catch
    {
    }
}



void pre_conditional_expression(struct preprocessor_ctx* ctx, struct error* error, struct pre_expression_ctx* ectx)
{
    /*
      conditional-expression:
      logical-OR-expression
      logical-OR-expression ? expression : conditional-expression
    */
    try
    {
        pre_logical_or_expression(ctx, error, ectx);
        if (error->code != 0) throw;

        if (ctx->current && ctx->current->type == '?')
        {
            pre_match(ctx);
            if (ectx->value)
            {
                pre_expression(ctx, error, ectx);
                if (error->code != 0) throw;

                pre_match(ctx); //:
                struct pre_expression_ctx temp;
                pre_conditional_expression(ctx, error, &temp);
                if (error->code != 0) throw;
            }
            else
            {
                struct pre_expression_ctx temp;
                pre_expression(ctx, error, &temp);
                if (error->code != 0) throw;
                pre_match(ctx); //:
                pre_conditional_expression(ctx, error, ectx);
                if (error->code != 0) throw;
            }
        }
    }
    catch
    {
    }
}

int pre_constant_expression(struct preprocessor_ctx* ctx, struct error* error, long long* pvalue)
{
    struct pre_expression_ctx ectx = { 0 };
    pre_conditional_expression(ctx, error, &ectx);
    *pvalue = ectx.value;
    return error->code;
}



struct declarator* find_declarator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt);



void print_item(struct osstream* ss, bool* first, const char* item)
{
    if (!(*first))
        ss_fprintf(ss, " ");
    ss_fprintf(ss, "%s", item);
    *first = false;

}

bool print_type_specifier_flags(struct osstream* ss, bool* first, enum type_specifier_flags e_type_specifier_flags)
{
    if (e_type_specifier_flags & TYPE_SPECIFIER_VOID)
        print_item(ss, first, "void");

    if (e_type_specifier_flags & TYPE_SPECIFIER_SIGNED)
        print_item(ss, first, "signed");

    if (e_type_specifier_flags & TYPE_SPECIFIER_UNSIGNED)
        print_item(ss, first, "unsigned");

    if (e_type_specifier_flags & TYPE_SPECIFIER_INT)
        print_item(ss, first, "int");

    if (e_type_specifier_flags & TYPE_SPECIFIER_SHORT)
        print_item(ss, first, "short");

    if (e_type_specifier_flags & TYPE_SPECIFIER_LONG)
        print_item(ss, first, "long");

    if (e_type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        print_item(ss, first, "long long");

    if (e_type_specifier_flags & TYPE_SPECIFIER_CHAR)
        print_item(ss, first, "char");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        print_item(ss, first, "double");

    if (e_type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        print_item(ss, first, "float");

    if (e_type_specifier_flags & TYPE_SPECIFIER_BOOL)
        print_item(ss, first, "_Bool");

    if (e_type_specifier_flags & TYPE_SPECIFIER_COMPLEX)
        print_item(ss, first, "_Complex");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DECIMAL32)
        print_item(ss, first, "_Decimal32");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DECIMAL64)
        print_item(ss, first, "_Decimal64");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DECIMAL128)
        print_item(ss, first, "_Decimal128");

    return first;
}



void print_direct_declarator_type(struct osstream* ss, struct direct_declarator_type* type, const char* name);

void print_declarator_type(struct osstream* ss, struct declarator_type* p_declarator_type, const char* name)
{
    if (p_declarator_type == NULL)
    {
        return;
    }

    bool first = false;
    struct pointer_type* pointer = p_declarator_type->pointers.head;
    while (pointer)
    {
        ss_fprintf(ss, "*");
        print_type_qualifier_flags(ss, &first, pointer->type_qualifier_flags);

        pointer = pointer->next;
    }

    if (p_declarator_type->direct_declarator_type)
    {
        print_direct_declarator_type(ss, p_declarator_type->direct_declarator_type, name);
    }


}

void print_direct_declarator_type(struct osstream* ss, struct direct_declarator_type* p_direct_declarator_type, const char* name)
{

    if (p_direct_declarator_type->declarator_opt)
    {
        ss_fprintf(ss, "(");
        print_declarator_type(ss, p_direct_declarator_type->declarator_opt, name);
        ss_fprintf(ss, ")");
    }

    if (name)
      ss_fprintf(ss, "%s", name);

    struct array_function_type* p_array_function_type =
        p_direct_declarator_type->array_function_type_list.head;
    for (; p_array_function_type; p_array_function_type = p_array_function_type->next)
    {
        if (p_array_function_type->bIsArray)
        {
            ss_fprintf(ss, "[%d]", p_array_function_type->array_size);
        }
        else if (p_array_function_type->bIsFunction)
        {
            ss_fprintf(ss, "(");
            struct type* param = p_array_function_type->params.head;
            while (param)
            {
                if (param != p_array_function_type->params.head)
                    ss_fprintf(ss, ",");
                print_type(ss, param);
                param = param->next;
            }
            if (p_array_function_type->bVarArg)
                ss_fprintf(ss, ",...");

            ss_fprintf(ss, ")");
        }
    }

}


void print_type_qualifier_flags(struct osstream* ss, bool* first, enum type_qualifier_flags e_type_qualifier_flags)
{

    if (e_type_qualifier_flags & TYPE_QUALIFIER_CONST)
        print_item(ss, first, "const");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_RESTRICT)
        print_item(ss, first, "restrict");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_VOLATILE)
        print_item(ss, first, "volatile");

}

void print_type_qualifier_specifiers(struct osstream* ss, struct type* type)
{
    bool first = true;
    print_type_qualifier_flags(ss, &first, type->type_qualifier_flags);

    if (type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
    {
        print_item(ss, &first, "struct ");
        ss_fprintf(ss, "%s", type->struct_or_union_specifier->tag_name);
    }
    else if (type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
    {
        print_item(ss, &first, "enum ");
        if (type->enum_specifier->tag_token)
            ss_fprintf(ss, "%s", type->enum_specifier->tag_token->lexeme);

    }
    else if (type->type_specifier_flags & TYPE_SPECIFIER_TYPEDEF)
    {
        assert(false);
    }
    else
    {
        print_type_specifier_flags(ss, &first, type->type_specifier_flags);
    }
}

void print_type(struct osstream* ss, struct type* type)
{
    print_type_qualifier_specifiers(ss, type);
    print_declarator_type(ss, type->declarator_type, /*name*/NULL);
}

void type_print(struct type* a) {
    struct osstream ss = { 0 };
    print_type(&ss, a);
    puts(ss.c_str);
    puts("\n");
    ss_close(&ss);
}

void pointer_type_list_pop_front(struct pointer_type_list* list)
{
    if (list->head != NULL)
    {
        struct pointer_type* p = list->head;
        if (list->head == list->tail)
        {
            list->head = list->tail = NULL;
        }
        else
        {
            list->head = list->head->next;
        }
        p->next = NULL;
    }
}

struct pointer_type_list pointer_type_list_copy(struct pointer_type_list* p_pointer_type_list)
{
    struct pointer_type_list list = { 0 };
    struct pointer_type* p = p_pointer_type_list->head;
    while (p)
    {
        struct pointer_type* p_pointer_type = calloc(1, sizeof * p_pointer_type);
        p_pointer_type->type_qualifier_flags = p->type_qualifier_flags;
        list_add(&list, p_pointer_type);
        p = p->next;
    }
    return list;
}



struct type_list params_copy(struct type_list* input)
{
    struct type_list r = { 0 };
    struct type* p_param_type = input->head;
    while (p_param_type)
    {
        struct type* par = calloc(1, sizeof * par);
        par->type_qualifier_flags = p_param_type->type_qualifier_flags;
        par->type_specifier_flags = p_param_type->type_specifier_flags;

        par->enum_specifier = p_param_type->enum_specifier;
        par->struct_or_union_specifier = p_param_type->struct_or_union_specifier;
        par->declarator_type = declarator_type_copy(p_param_type->declarator_type);
        list_add(&r, par);
        p_param_type = p_param_type->next;
    }
    return r;
}

struct array_function_type_list array_function_type_list_copy(struct array_function_type_list* p_array_function_type_list)
{
    struct array_function_type_list list = { 0 };
    struct array_function_type* p = p_array_function_type_list->head;
    while (p)
    {
        struct array_function_type* p_array_function_type = calloc(1, sizeof(struct array_function_type));
        p_array_function_type->array_size = p->array_size;
        p_array_function_type->bIsArray = p->bIsArray;
        p_array_function_type->bIsFunction = p->bIsFunction;
        p_array_function_type->bVarArg = p->bVarArg;
        p_array_function_type->params = params_copy(&p->params);
        list_add(&list, p_array_function_type);
        p = p->next;
    }
    return list;
}



struct direct_declarator_type* direct_declarator_type_copy(struct direct_declarator_type* p_direct_declarator_type_opt)
{
    if (p_direct_declarator_type_opt == NULL)
        return NULL;
    struct direct_declarator_type* p_direct_declarator_type = calloc(1, sizeof(struct direct_declarator_type));
    p_direct_declarator_type->declarator_opt = declarator_type_copy(p_direct_declarator_type_opt->declarator_opt);
    p_direct_declarator_type->array_function_type_list = array_function_type_list_copy(&p_direct_declarator_type_opt->array_function_type_list);
    return p_direct_declarator_type;
}

struct declarator_type* declarator_type_copy(struct declarator_type* p_declarator_type_opt)
{
    if (p_declarator_type_opt == NULL)
        return NULL;
    struct declarator_type* p_declarator_type = calloc(1, sizeof(struct declarator_type));
    p_declarator_type->pointers = pointer_type_list_copy(&p_declarator_type_opt->pointers);
    p_declarator_type->direct_declarator_type = direct_declarator_type_copy(p_declarator_type_opt->direct_declarator_type);
    return p_declarator_type;
}

struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type);


void visit_declarator_get(enum type_category* type_category, struct declarator_type* declarator);
void visit_direct_declarator_get(enum type_category* type_category, struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt)
        visit_declarator_get(type_category, p_direct_declarator_type->declarator_opt);

    struct array_function_type* p = p_direct_declarator_type->array_function_type_list.head;
    while (p)
    {
        if (p->bIsFunction)
        {
            if (*type_category == TYPE_CATEGORY_ITSELF)
            {
                *type_category = TYPE_CATEGORY_FUNCTION;
                break;
            }
        }
        else if (p->bIsArray)
        {
            if (*type_category == TYPE_CATEGORY_ITSELF)
            {
                *type_category = TYPE_CATEGORY_ARRAY; /*array*/
                break;
            }
        }
        p = p->next;
    }
}

void visit_declarator_get(enum type_category* type_category, struct declarator_type* declarator)
{
    if (declarator == NULL)
        return;

    if (declarator->direct_declarator_type)
        visit_direct_declarator_get(type_category, declarator->direct_declarator_type);

    if (*type_category == TYPE_CATEGORY_ITSELF)
    {
        if (declarator->pointers.head)
        {
            *type_category = TYPE_CATEGORY_POINTER;
        }
    }
}

enum type_category find_type_category(const struct type* p_type)
{
    enum type_category type_category = TYPE_CATEGORY_ITSELF;
    visit_declarator_get(&type_category, p_type->declarator_type);
    return type_category;
}

bool type_is_array(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ARRAY;
}




bool type_is_pointer(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_POINTER;
}


bool type_is_enum(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ITSELF &&
           p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM;
}

bool type_is_struct_or_union(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION;
}

/*
* The three types char, signed char, and unsigned char are collectively called the character types.
The implementation shall define char to have the same range, representation, and behavior as either
signed char or unsigned char.50)
*/

bool type_is_character(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR;
}

bool type_is_bool(struct type* p_type)
{
    return find_type_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL;
}

/*
  The type char, the signed and unsigned integer types, and the enumerated types are collectively
  called integer types. The integer and real floating types are collectively called real types
*/
bool type_is_integer(struct type* p_type)
{
    if (find_type_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_CHAR |
            TYPE_SPECIFIER_SHORT |
            TYPE_SPECIFIER_INT |
            TYPE_SPECIFIER_LONG |
            TYPE_SPECIFIER_SIGNED |
            TYPE_SPECIFIER_UNSIGNED |
            TYPE_SPECIFIER_INT8 |
            TYPE_SPECIFIER_INT16 |
            TYPE_SPECIFIER_INT64 |
            TYPE_SPECIFIER_LONG_LONG);
}

/*
* Integer and floating types are collectively called arithmetic types.
*/
bool type_is_arithmetic(struct type* p_type)
{
    if (find_type_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;


    return p_type->type_specifier_flags &
        (
            TYPE_SPECIFIER_FLOAT |
            TYPE_SPECIFIER_DOUBLE |

            TYPE_SPECIFIER_DECIMAL32 |
            TYPE_SPECIFIER_DECIMAL64 |
            TYPE_SPECIFIER_DECIMAL128 |

            TYPE_SPECIFIER_CHAR |
            TYPE_SPECIFIER_SHORT |
            TYPE_SPECIFIER_INT |
            TYPE_SPECIFIER_LONG |
            TYPE_SPECIFIER_SIGNED |
            TYPE_SPECIFIER_UNSIGNED |
            TYPE_SPECIFIER_INT8 |
            TYPE_SPECIFIER_INT16 |
            TYPE_SPECIFIER_INT64 |
            TYPE_SPECIFIER_LONG_LONG
            );
}

/*
 Arithmetic types, pointer types, and the nullptr_t type are collectively called scalar types. Array
and structure types are collectively called aggregate types51).
*/
bool type_is_scalar(struct type* p_type)
{

    if (type_is_arithmetic(p_type))
        return true;

    if (type_is_pointer_or_array(p_type))
        return true;

    if (find_type_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags & TYPE_SPECIFIER_NULLPTR;
}

bool type_is_compatible(struct type* expression_type, struct type* return_type)
{
    //TODO

    //if (!type_is_same(a, b))
      //  return false;

    return true;
}

bool type_is_compatible_type_function_call(struct type* argument_type, struct type* paramer_type)
{
    if (type_is_arithmetic(argument_type) && type_is_arithmetic(paramer_type))
    {
        return true;
    }

    if (type_is_arithmetic(argument_type) && type_is_bool(paramer_type))
    {
        /*passar 0 or 1 to bool*/
        return true;
    }


    if (type_is_bool(argument_type) && type_is_bool(paramer_type))
    {
        return true;
    }

    if (type_is_pointer(argument_type) && type_is_pointer(paramer_type))
    {
        if (argument_type->type_specifier_flags & TYPE_SPECIFIER_VOID)
        {
            /*void pointer can be converted to any type*/
            return true;
        }

        if (!type_is_same(argument_type, paramer_type, false))
        {
            //disabled for now util it works correctly
            //return false;
        }
        return true;
    }

    if (type_is_array(argument_type) && type_is_pointer(paramer_type))
    {
        //TODO
        return true;
    }

    if (type_is_pointer(argument_type) && type_is_array(paramer_type))
    {
        //TODO
        return true;
    }

    if (type_is_enum(argument_type) && type_is_enum(paramer_type))
    {
        if (!type_is_same(argument_type, paramer_type, false))
        {
            //disabled for now util it works correctly
            //return false;
        }
        return true;
    }

    if (type_is_arithmetic(argument_type) && type_is_enum(paramer_type))
    {
        return true;
    }

    if (type_is_enum(argument_type) && type_is_arithmetic(paramer_type))
    {
        return true;
    }


    //disabled for now util it works correctly
    //return false;
    return true;
}

bool type_is_function_or_function_pointer(struct type* p_type)
{
    if (p_type->declarator_type == NULL)
        return false;


    if (
        p_type->declarator_type->direct_declarator_type &&
        p_type->declarator_type->direct_declarator_type->array_function_type_list.head &&
        p_type->declarator_type->direct_declarator_type->array_function_type_list.head->bIsFunction)
    {
        return true;
    }

    return false;
}

struct type get_address_of_type(struct type* p_type)
{
    struct type r = type_copy(p_type);
    struct declarator_type* p = find_inner_declarator(r.declarator_type);
    if (p == NULL)
    {
        assert(false);
    }

    if (p->direct_declarator_type &&
        p->direct_declarator_type->array_function_type_list.head &&
        (p->direct_declarator_type->array_function_type_list.head->bIsArray ||
            p->direct_declarator_type->array_function_type_list.head->bIsFunction))
    {
        struct declarator_type* p2 = calloc(1, sizeof * p);
        p->direct_declarator_type->declarator_opt = p2;

        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        list_add(&p2->pointers, p_pointer_type);
    }
    else
    {
        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        list_add(&p->pointers, p_pointer_type);
    }



    return r;
}

struct type get_pointer_content_type(struct type* p_type)
{
    struct type r = type_copy(p_type);
    struct declarator_type* p_inner_declarator = find_inner_declarator(r.declarator_type);
    if (p_inner_declarator->pointers.head != NULL)
    {
        pointer_type_list_pop_front(&p_inner_declarator->pointers);
    }
    else
    {
        //parser_seterror_with_token(ctx, ctx->current, "indirection requires pointer operand");
    }
    return r;
}


struct type get_array_item_type(struct type* p_type)
{

    struct type r = { 0 };
    r.type_qualifier_flags = p_type->type_qualifier_flags;
    r.type_specifier_flags = p_type->type_specifier_flags;

    r.struct_or_union_specifier = p_type->struct_or_union_specifier;
    r.enum_specifier = p_type->enum_specifier;



    if (p_type->declarator_type)
    {
        r.declarator_type = declarator_type_copy(p_type->declarator_type);
        array_function_type_list_pop_back(&r.declarator_type->direct_declarator_type->array_function_type_list);
    }

    return r;
}

void print_declarator_description(struct osstream* ss, struct declarator_type* declarator);
void print_direct_declarator_description(struct osstream* ss, struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt)
        print_declarator_description(ss, p_direct_declarator_type->declarator_opt);

    struct array_function_type* p = p_direct_declarator_type->array_function_type_list.head;
    while (p)
    {
        if (p->bIsFunction)
        {
            ss_fprintf(ss, " function returning ");
        }
        else if (p->bIsArray)
        {
            ss_fprintf(ss, "array [%d] of ", p->array_size);
        }
        p = p->next;
    }
}

void print_declarator_description(struct osstream* ss, struct declarator_type* declarator)
{
    if (declarator->direct_declarator_type)
        print_direct_declarator_description(ss, declarator->direct_declarator_type);

    struct pointer_type* p = declarator->pointers.head;
    while (p)
    {
        ss_fprintf(ss, " pointer to ");
        p = p->next;
    }
}



void function_result_declarator(int* pop, struct declarator_type* declarator);
void function_result_ddeclarator(int* pop, struct direct_declarator_type* p_direct_declarator_type)
{
    if (p_direct_declarator_type->declarator_opt)
        function_result_declarator(pop, p_direct_declarator_type->declarator_opt);

    struct array_function_type* p = p_direct_declarator_type->array_function_type_list.head;
    while (p)
    {
        if (p->bIsFunction)
        {
            if (*pop == 0)
            {
                *pop = 1;
                struct array_function_type* removed =
                    array_function_type_list_pop_back(&p_direct_declarator_type->array_function_type_list);
                //array_function_type_delete(removed);
                break;
            }
        }
        p = p->next;
    }
}



void function_result_declarator(int* pop, struct declarator_type* declarator)
{
    if (declarator->direct_declarator_type)
        function_result_ddeclarator(pop, declarator->direct_declarator_type);
}

struct type get_function_return_type(struct type* p_type)
{
    struct type r = type_copy(p_type);
    int pop = 0;
    function_result_declarator(&pop, r.declarator_type);
    return r;
}


bool type_is_pointer_or_array(struct type* p_type)
{
    if (p_type->declarator_type == NULL)
        return false;

    struct declarator_type* p_inner_declarator = find_inner_declarator(p_type->declarator_type);

    if (p_inner_declarator &&
        p_inner_declarator->pointers.head != NULL)
    {
        return true;
    }

    if (p_inner_declarator->direct_declarator_type->array_function_type_list.tail)
    {
        if (p_inner_declarator->direct_declarator_type->array_function_type_list.tail->bIsArray)
        {
            /*arrays sao equivalentes a ponteiros em C*/
            return true;
        }
    }
    return false;
}


int type_get_rank(struct type* p_type1, struct error* error)
{
    if (type_is_pointer_or_array(p_type1))
    {
        return 40;
    }

    int rank = 0;
    if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_BOOL))
    {
        rank = 10;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_CHAR) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_INT8))
    {
        rank = 20;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_SHORT) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_INT16))
    {
        rank = 30;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_INT) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_ENUM))
    {
        rank = 40;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_LONG) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_INT32))
    {
        rank = 50;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_FLOAT))
    {
        rank = 60;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_DOUBLE))
    {
        rank = 70;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_INT64))
    {
        rank = 80;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION))
    {
        seterror(error, "internal error - struct is not valid for rank");
    }
    else
    {
        seterror(error, "unexpected type for rank");
    }
    return rank;
}

struct type type_common(struct type* p_type1, struct type* p_type2, struct error* error)
{
    try
    {
        int rank_left = type_get_rank(p_type1, error);
        if (error) throw;

        int rank_right = type_get_rank(p_type1, error);
        if (error) throw;


        if (rank_left >= rank_right)
            return type_copy(p_type1);
        else
            return type_copy(p_type2);
    }
    catch
    {
    }

    return type_copy(p_type1);
}

/*retorna uma cópia do tipo*/
struct type type_copy(struct type* p_type)
{
    struct type r = { 0 };
    r.type_qualifier_flags = p_type->type_qualifier_flags;
    r.type_specifier_flags = p_type->type_specifier_flags;
    r.struct_or_union_specifier = p_type->struct_or_union_specifier;
    r.enum_specifier = p_type->enum_specifier;


    if (p_type->declarator_type)
    {
        r.declarator_type = declarator_type_copy(p_type->declarator_type);
    }
    return r;
}


struct array_function_type* array_function_type_list_pop_back(struct array_function_type_list* list)
{
    if (list == NULL)
        return NULL;
    if (list->head == list->tail)
    {
        struct array_function_type* p = list->head;
        list->head = NULL;
        list->tail = NULL;
        return p;
    }
    struct array_function_type* before_last = list->head;
    while (before_last->next->next)
    {
        before_last = before_last->next;
    }
    struct array_function_type* p = before_last->next;
    before_last->next = NULL;
    return p;
}

int type_get_sizeof(struct parser_ctx* ctx, struct type* p_type, struct error* error)
{
    size_t size = 0;

    try
    {
        if (p_type->declarator_type &&
            p_type->declarator_type->pointers.head != NULL)
        {
            size = sizeof(void*);
        }
        else {

            if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
            {
                size = sizeof(char);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
            {
                size = sizeof(_Bool);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
            {
                size = sizeof(int);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT)
            {
                size = sizeof(int);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
            {
                size = sizeof(long);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
            {
                size = sizeof(long long);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT64)
            {
                size = sizeof(long long);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT32)
            {
                size = sizeof(long);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT16)
            {
                size = sizeof(short);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT8)
            {
                size = sizeof(char);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
            {
                size = sizeof(double);
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
            {
                size = 1;       //TODO
            }
            else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
            {
                size = sizeof(int);
            }
            else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
            {
                seterror(error, "type information is missing");
                throw;
            }
            else if (p_type->type_specifier_flags == TYPE_SPECIFIER_TYPEOF)
            {
                size = 1; //TODO
                //assert(false);
                //;; size =
                    //  type_get_sizeof(ctx, struct type* p_type, struct error* error)
            }
            else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
            {
                //
            }
            else
            {
                assert(false);
            }

        }

        //Multiplica size pelo numero de elementos
        if (p_type->declarator_type &&
            p_type->declarator_type->direct_declarator_type)
        {
            struct array_function_type* p_array_function_type =
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head;
            while (p_array_function_type)
            {
                if (p_array_function_type->bIsArray)
                {
                    size = size * p_array_function_type->array_size;
                }
                p_array_function_type = p_array_function_type->next;
            }
        }
    }
    catch
    {
    }

    return size;
}

unsigned int type_get_hashof(struct parser_ctx* ctx, struct type* p_type, struct error* error)
{
    unsigned int hash = 0;

    try
    {

        if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
        {
            //size = sizeof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        {
            //size = sizeof(_Bool);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        {
            //size = sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT)
        {
            //size = sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            //size = sizeof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        {
            //size = sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT64)
        {
            //size = sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT32)
        {
            //size = sizeof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT16)
        {
            //size = sizeof(short);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT8)
        {
            //size = sizeof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        {
            //size = sizeof(double);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            struct osstream ss = { 0 };

            struct struct_or_union_specifier* p_struct_or_union_specifier =
                p_type->struct_or_union_specifier;

            if (p_struct_or_union_specifier->member_declaration_list.head == NULL)
            {
                p_struct_or_union_specifier =
                    p_type->struct_or_union_specifier->complete_struct_or_union_specifier;
            }

            struct token* current = p_struct_or_union_specifier->first;
            for (;
                current != p_struct_or_union_specifier->last->next;
                current = current->next)
            {
                if (current->flags & TK_FLAG_FINAL)
                {
                    ss_fprintf(&ss, "%s", current->lexeme);

                }
            }
            hash = stringhash(ss.c_str);
            ss_close(&ss);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            //size = sizeof(int);
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            seterror(error, "type information is missing");
            throw;
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_TYPEOF)
        {
            //s//ize = 1; //TODO
            //assert(false);
            //;; size =
                //  type_get_sizeof(ctx, struct type* p_type, struct error* error)
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
        {
            if (p_type->declarator_type &&
                p_type->declarator_type->pointers.head != NULL)
            {
                // size = sizeof(void*);
            }
            else
            {
                seterror(error, "invalid application of 'sizeof' to a void type");
                throw;
            }
        }
        else
        {
            assert(false);
        }

        if (p_type->declarator_type &&
            p_type->declarator_type->direct_declarator_type)
        {
            struct array_function_type* p_array_function_type =
                p_type->declarator_type->direct_declarator_type->array_function_type_list.head;
            while (p_array_function_type)
            {
                if (p_array_function_type->bIsArray)
                {
                    //size = size * p_array_function_type->constant_size;
                }
                p_array_function_type = p_array_function_type->next;
            }
        }
    }
    catch
    {
    }

    return hash;
}


void type_set_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->type_qualifier_flags =
            pdeclarator->declaration_specifiers->type_qualifier_flags;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        p_type->type_qualifier_flags =
            pdeclarator->specifier_qualifier_list->type_qualifier_flags;
    }
}

void type_set_specifiers_using_declarator(struct parser_ctx* ctx, struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->type_specifier_flags =
            pdeclarator->declaration_specifiers->type_specifier_flags;

        p_type->enum_specifier = pdeclarator->declaration_specifiers->enum_specifier;
        p_type->struct_or_union_specifier = pdeclarator->declaration_specifiers->struct_or_union_specifier;

    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        p_type->type_specifier_flags =
            pdeclarator->specifier_qualifier_list->type_specifier_flags;
        p_type->enum_specifier = pdeclarator->specifier_qualifier_list->enum_specifier;
        p_type->struct_or_union_specifier = pdeclarator->specifier_qualifier_list->struct_or_union_specifier;

    }
}


struct pointer_type_list clone_pointer_to_pointer_type_list(struct pointer* p_pointer)
{
    struct pointer_type_list r = { 0 };
    if (p_pointer == NULL)
    {
        return r;
    }
    struct pointer* p = p_pointer;
    while (p)
    {
        struct pointer_type* p_pointer_type = calloc(1, sizeof(struct pointer_type));
        if (p_pointer->type_qualifier_list_opt)
            p_pointer_type->type_qualifier_flags = p_pointer->type_qualifier_list_opt->flags;
        list_add(&r, p_pointer_type);
        p = p->pointer;
    }
    return r;
}

struct declarator_type* clone_declarator_to_declarator_type(struct parser_ctx* ctx, struct declarator* p_declarator);

struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);

struct direct_declarator_type* clone_direct_declarator_to_direct_declarator_type(struct parser_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator == NULL)
    {
        return NULL;
    }
    struct direct_declarator_type* p_direct_declarator_type = calloc(1, sizeof(struct direct_declarator_type));
    if (p_direct_declarator->declarator)
    {
        p_direct_declarator_type->declarator_opt = clone_declarator_to_declarator_type(ctx, p_direct_declarator->declarator);
    }
    struct array_function* p_array_function = p_direct_declarator->array_function_list.head;
    while (p_array_function)
    {
        struct array_function_type* p_array_function_type = calloc(1, sizeof(struct array_function_type));
        if (p_array_function->function_declarator)
        {
            /*copy function argument type into params*/
            p_array_function_type->bIsFunction = true;



            struct parameter_declaration* p_parameter_declaration = NULL;

            if (p_array_function->function_declarator->parameter_type_list_opt != NULL)
            {
                p_array_function_type->bVarArg =
                    p_array_function->function_declarator->parameter_type_list_opt->is_var_args;

                p_parameter_declaration =
                    p_array_function->function_declarator->parameter_type_list_opt->parameter_list->head;

            }
            while (p_parameter_declaration)
            {
                struct type* p_type = calloc(1, sizeof(struct type));
                *p_type = make_type_using_declarator(ctx, p_parameter_declaration->declarator);
                //print_type(p_type);
                list_add(&p_array_function_type->params, p_type);
                p_parameter_declaration = p_parameter_declaration->next;
            }
        }
        else if (p_array_function->array_declarator)
        {
            p_array_function_type->array_size = p_array_function->array_declarator->constant_size;
            p_array_function_type->bIsArray = true;
        }
        else
        {
            assert(false);
        }
        list_add(&p_direct_declarator_type->array_function_type_list, p_array_function_type);
        p_array_function = p_array_function->next;
    }
    return p_direct_declarator_type;
}

struct declarator_type* clone_declarator_to_declarator_type(struct parser_ctx* ctx, struct declarator* p_declarator)
{
    if (p_declarator == NULL)
    {
        return NULL;
    }
    struct declarator_type* p_declarator_type = calloc(1, sizeof(struct declarator_type));
    //type_set_qualifiers_using_declarator(p_declarator_type, pdeclarator);
    //type_set_specifiers_using_declarator(declarator_type, pdeclarator);
    p_declarator_type->direct_declarator_type = clone_direct_declarator_to_direct_declarator_type(ctx, p_declarator->direct_declarator);
    p_declarator_type->pointers = clone_pointer_to_pointer_type_list(p_declarator->pointer);
    return p_declarator_type;
}
bool is_empty_declarator_type(struct declarator_type* p_declarator_type)
{
    return
        p_declarator_type->pointers.head == NULL &&
        p_declarator_type->direct_declarator_type->declarator_opt == NULL &&
        p_declarator_type->direct_declarator_type->array_function_type_list.head == NULL;
}

void type_merge(struct type* t1, struct type* typedef_type_copy)
{
#if 0    


#include <typeinfo>


#include <cxxabi.h>

    int status;
#define TYPE(EXPR) \
 printf("%s=", #EXPR); \
 printf("%s\n", abi::__cxa_demangle(typeid(typeof(EXPR)).name(),0,0,&status))


    typedef char* T1;
    typedef const T1 CONST_T1;
    typedef CONST_T1 T2[5];
    typedef T2 T3[2];

    int main()
    {
        TYPE(T1);
        TYPE(CONST_T1);
        TYPE(T2);
        TYPE(T3);
    }
#endif


    //struct type typedef_type_copy = type_copy(typedef_type);
    /*acho o nucleo do typedef*/
    struct declarator_type* p_typedef_decl = typedef_type_copy->declarator_type;
    while (p_typedef_decl)
    {
        if (p_typedef_decl->direct_declarator_type->declarator_opt)
            p_typedef_decl = p_typedef_decl->direct_declarator_type->declarator_opt;
        else
            break;
    }

    //    typedef_type_copy->type_qualifier_flags = t1->type_qualifier_flags;

    if (p_typedef_decl)
    {
        /*
        typedef int A[2];
        typedef A *B [1];
        =>int (* [1]) [2]


        typedef int A[2];
        typedef A *B ;
        =>int (*) [2]

        typedef int *A;
        typedef A *B;
        =>int**

        typedef int *A;
        typedef A B[1] ;
        =>int* [1]

        */
        if (p_typedef_decl->direct_declarator_type &&
            p_typedef_decl->direct_declarator_type->array_function_type_list.head)
        {
            p_typedef_decl->direct_declarator_type->declarator_opt =
                declarator_type_copy(t1->declarator_type);
        }
        else
        {
            struct declarator_type* copy = declarator_type_copy(t1->declarator_type);
            //copy->direct_declarator_type->array_function_type_list

            struct pointer_type* p = copy->pointers.head;
            while (p)
            {
                struct pointer_type* next = p->next;
                list_add(&p_typedef_decl->pointers, p);
                p = next;
            }
            struct array_function_type* p_array_function_type =
                copy->direct_declarator_type->array_function_type_list.head;
            while (p_array_function_type)
            {
                struct array_function_type* next = p_array_function_type->next;
                list_add(&p_typedef_decl->direct_declarator_type->array_function_type_list, p_array_function_type);
                p_array_function_type = next;
            }
            p_typedef_decl->direct_declarator_type->declarator_opt = copy->direct_declarator_type->declarator_opt;
        }


        //p_typedef_decl->direct_declarator_type->declarator_opt =
          //  declarator_type_copy(t1->declarator_type);
    }
}

struct type type_expand(struct parser_ctx* ctx, struct type* p_type);
struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator)
{
    struct type t = { 0 };
    memset(&t, 0, sizeof t);

    if (pdeclarator->specifier_qualifier_list)
    {
        if (pdeclarator->specifier_qualifier_list->typeof_specifier)
        {
         
            if (pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->expression)
            {
                t = type_copy(&pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->expression->type);
            }
            else if (pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->type_name)
            {
                type_set_qualifiers_using_declarator(&t, pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->type_name->declarator);
                type_set_specifiers_using_declarator(ctx, &t, pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->type_name->declarator);
                t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator->specifier_qualifier_list->typeof_specifier->typeof_specifier_argument->type_name->declarator);
            }
        }
        else    if (pdeclarator->specifier_qualifier_list->typedef_declarator)
        {
            struct type t0 = { 0 };
            type_set_qualifiers_using_declarator(&t0, pdeclarator);
            //type_set_specifiers_using_declarator(ctx, &t0, pdeclarator);
            t0.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator);
            struct type typedef_type = type_copy(&pdeclarator->specifier_qualifier_list->typedef_declarator->type);
            type_merge(&t0, &typedef_type);
            t = typedef_type; /*MOVED*/
            //struct osstream ss = { 0 };
            //print_type(&ss, &t);
            //ss_close(&ss);
        }
        else
        {
            type_set_qualifiers_using_declarator(&t, pdeclarator);
            type_set_specifiers_using_declarator(ctx, &t, pdeclarator);
            t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator);
        }
    }
    else if (pdeclarator->declaration_specifiers)
    {
        if (pdeclarator->declaration_specifiers->typeof_specifier)
        {
            if (pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->expression)
            {
                t = type_copy(&pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->expression->type);
            }
            else if (pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name)
            {
                type_set_qualifiers_using_declarator(&t, pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name->declarator);
                type_set_specifiers_using_declarator(ctx, &t, pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name->declarator);
                t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name->declarator);
            }
        }
        else if (pdeclarator->declaration_specifiers->typedef_declarator)
        {
            struct type t0 = { 0 };
            type_set_qualifiers_using_declarator(&t0, pdeclarator);
            //type_set_specifiers_using_declarator(ctx, &t0, pdeclarator);
            t0.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator);
            struct type typedef_type = type_copy(&pdeclarator->declaration_specifiers->typedef_declarator->type);
            type_merge(&t0, &typedef_type);
            t = typedef_type; /*MOVED*/
            struct osstream ss = { 0 };
            print_type(&ss, &t);
            ss_close(&ss);
        }
        else
        {
            type_set_qualifiers_using_declarator(&t, pdeclarator);
            type_set_specifiers_using_declarator(ctx, &t, pdeclarator);
            t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator);
        }
    }



    return t;
}

struct type make_type_using_declarator_do_not_expand(struct parser_ctx* ctx, struct declarator* pdeclarator)
{
    struct type t = { 0 };
    memset(&t, 0, sizeof t);
    type_set_qualifiers_using_declarator(&t, pdeclarator);
    type_set_specifiers_using_declarator(ctx, &t, pdeclarator);
    t.declarator_type = clone_declarator_to_declarator_type(ctx, pdeclarator);
    return t;
}

struct declarator_type* find_inner_declarator(struct declarator_type* p_declarator_type)
{
    struct declarator_type* p = p_declarator_type;
    while (p)
    {
        if (p->direct_declarator_type &&
            p->direct_declarator_type->declarator_opt)
            p = p->direct_declarator_type->declarator_opt;
        else
            break;
    }
    return p;
}

void type_set_int(struct type* p_type)
{
    p_type->type_specifier_flags = TYPE_SPECIFIER_INT;
    p_type->type_qualifier_flags = 0;
    p_type->declarator_type = NULL;
}


bool pointer_type_is_same(struct pointer_type* a, struct pointer_type* b)
{
    if (a && b)
    {
        if (a->type_qualifier_flags != b->type_qualifier_flags)
            return false;

        return true;
    }
    return a == NULL && b == NULL;
}

bool pointer_type_list_is_same(struct pointer_type_list* a, struct pointer_type_list* b)
{
    if (a && b)
    {
        struct pointer_type* pa = a->head;
        struct pointer_type* pb = b->head;

        if (pa && pb)
        {
            while (pa && pb)
            {
                if (!pointer_type_is_same(pa, pb))
                    return false;
                pa = pa->next;
                pb = pb->next;
            }
            return pa == NULL && pb == NULL;
        }
        return true;
    }

    return a == NULL && b == NULL;
}

bool type_is_same(struct type* a, struct type* b, bool compare_qualifiers);
bool type_list_is_same(struct type_list* a, struct type_list* b)
{
    if (a && b)
    {
        struct type* pa = a->head;
        struct type* pb = b->head;

        if (pa && pb)
        {
            while (pa && pb)
            {
                if (!type_is_same(pa, pb, true))
                    return false;
                pa = pa->next;
                pb = pb->next;
            }
            return pa == NULL && pb == NULL;
        }
        return pa == NULL && pb == NULL;

    }
    return a == NULL && b == NULL;
}

bool array_function_type_is_same(struct array_function_type* a, struct array_function_type* b)
{
    if (a && b)
    {
        if (a->array_size != b->array_size)
            return false;
        if (a->bIsArray != b->bIsArray)
            return false;
        if (a->bIsFunction != b->bIsFunction)
            return false;
        if (a->bVarArg && b->bVarArg)
            return false;

        if (a->array_size != b->array_size)
            return false;

        if (!type_list_is_same(&a->params, &b->params))
            return false;

        return true;
    }
    return a == NULL && b == NULL;
}

bool array_function_type_list_is_same(struct array_function_type_list* a, struct array_function_type_list* b)
{
    if (a && b)
    {
        struct array_function_type* pa = a->head;
        struct array_function_type* pb = b->head;
        if (pa && pb)
        {
            while (pa && pb)
            {
                if (!array_function_type_is_same(pa, pb))
                    return false;
                pa = pa->next;
                pb = pb->next;
            }
            return pa == NULL && pb == NULL;
        }
        return pa == NULL && pb == NULL;

    }
    return a == NULL && b == NULL;
}

bool declarator_type_is_same(struct declarator_type* a, struct declarator_type* b);

bool direct_declarator_type_is_empty(struct direct_declarator_type* a)
{
    if (a == NULL)
        return true;

    if (a->declarator_opt != NULL)
    {
        return false;
    }

    if (a->array_function_type_list.head != NULL)
    {
        return false;
    }

    return true;
}
bool direct_declarator_type_is_same(struct direct_declarator_type* a, struct direct_declarator_type* b)
{
    if (!direct_declarator_type_is_empty(a) && !direct_declarator_type_is_empty(b))
    {

        if (!array_function_type_list_is_same(&a->array_function_type_list, &b->array_function_type_list))
        {
            return false;
        }

        if (!declarator_type_is_same(a->declarator_opt, b->declarator_opt))
        {
            return false;
        }

        return true;
    }
    return direct_declarator_type_is_empty(a) && direct_declarator_type_is_empty(b);
}

bool struct_or_union_specifier_is_same(struct struct_or_union_specifier* a, struct struct_or_union_specifier* b)
{
    if (a && b)
    {
        if (a->complete_struct_or_union_specifier != NULL && b->complete_struct_or_union_specifier != NULL)
        {
            if (a->complete_struct_or_union_specifier != b->complete_struct_or_union_specifier)
            {
                return false;
            }
            return true;
        }
        else
        {
            /*both incomplete then we compare tag names*/
            if (a->tagtoken != NULL && b->tagtoken != NULL)
            {
                if (strcmp(a->tagtoken->lexeme, b->tagtoken->lexeme) == 0)
                    return true;
            }
        }
        return a->complete_struct_or_union_specifier == NULL &&
            b->complete_struct_or_union_specifier == NULL;
    }
    return a == NULL && b == NULL;
}

bool enum_specifier_is_same(struct enum_specifier* a, struct enum_specifier* b)
{
    if (a && b)
    {
        if (a->complete_enum_specifier && b->complete_enum_specifier)
        {
            if (a->complete_enum_specifier != b->complete_enum_specifier)
                return false;
            return true;
        }
        return a->complete_enum_specifier == NULL &&
            b->complete_enum_specifier == NULL;
    }
    return a == NULL && b == NULL;
}

bool declarator_type_is_empty(struct declarator_type* a)
{
    if (a == NULL)
        return true;

    if (a->direct_declarator_type)
    {
        if (a->direct_declarator_type->array_function_type_list.head != NULL ||
            a->direct_declarator_type->declarator_opt != NULL)
        {
            return false;
        }
    }
    if (a->pointers.head != NULL)
    {
        return false;
    }

    return true;
}

bool declarator_type_is_same(struct declarator_type* a, struct declarator_type* b)
{
    if (!declarator_type_is_empty(a) && !declarator_type_is_empty(b))
    {
        if (!pointer_type_list_is_same(&a->pointers, &b->pointers))
            return false;
        if (!direct_declarator_type_is_same(a->direct_declarator_type, b->direct_declarator_type))
            return false;

        return true;
    }
    return declarator_type_is_empty(a) && declarator_type_is_empty(b);
}



bool type_is_same(struct type* a, struct type* b, bool compare_qualifiers)
{
    //debug
    //type_print(a);
    //type_print(b);
    //TODO compare_qualifiers inner declarator
    compare_qualifiers;

    if (a && b)
    {
        if (a->type_qualifier_flags != b->type_qualifier_flags)
        {
            return false;
        }

        if (a->type_specifier_flags != b->type_specifier_flags)
        {
            return false;
        }

        if (!declarator_type_is_same(a->declarator_type, b->declarator_type))
        {
            return false;
        }

        if (!enum_specifier_is_same(a->enum_specifier, b->enum_specifier))
        {
            return false;
        }

        if (!struct_or_union_specifier_is_same(a->struct_or_union_specifier, b->struct_or_union_specifier))
        {
            return false;
        }

        return true;
    }

    return a == NULL && b == NULL;
}





//#pragma once

struct format_visit_ctx
{
    struct ast ast;
    int identation;
};

void format_visit(struct format_visit_ctx* ctx, struct error* error);

#ifdef _WIN32

#undef assert
#define assert _ASSERTE
#endif



//#pragma once


struct defer_scope
{
    struct defer_statement* defer_statement; // defer 
    struct try_statement* p_try_statement; //try
    struct selection_statement* p_selection_statement2; //if swith
    struct iteration_statement* p_iteration_statement; //for do while
    struct statement* p_statement; //
    struct compound_statement* p_function_body;
    struct defer_scope* lastchild;

    struct defer_scope* previous;
};

struct visit_ctx
{
    /*
    * It is necessary two passes to generate lambdas expressions
    * because some types maybe needs to be "globalized"
    * is_second_pass is true if the compiler is at second pass
    */
    bool is_second_pass;
   
    bool has_lambda;

    bool is_inside_lambda;

    /*these indexes are used to generate unique names at file scope*/
    int capture_index;
    int typeof_index;
    int lambdas_index;
    
    struct token_list insert_before_declaration;
    struct token_list insert_before_block_item;
    struct ast ast;
    enum language_version target;
    struct defer_scope* tail_block;
};

void visit(struct visit_ctx* ctx, struct error* error);


struct defer_statement* defer_statement(struct parser_ctx* ctx, struct error* error);

static int anonymous_struct_count = 0;

///////////////////////////////////////////////////////////////////////////////
void naming_convention_struct_tag(struct parser_ctx* ctx, struct token* token);
void naming_convention_enum_tag(struct parser_ctx* ctx, struct token* token);
void naming_convention_function(struct parser_ctx* ctx, struct token* token);
void naming_convention_enumerator(struct parser_ctx* ctx, struct token* token);
void naming_convention_struct_member(struct parser_ctx* ctx, struct token* token, struct type* type);
void naming_convention_parameter(struct parser_ctx* ctx, struct token* token, struct type* type);
void naming_convention_global_var(struct parser_ctx* ctx, struct token* token, struct type* type, enum storage_class_specifier_flags storage );
void naming_convention_local_var(struct parser_ctx* ctx, struct token* token, struct type* type);

///////////////////////////////////////////////////////////////////////////////

/*coisas que vao em hash map possuim um id
assim é possivel usar o mesmo mapa para achar tipos diferentes
*/
enum
{
    TAG_TYPE_NONE,
    TAG_TYPE_ENUN_SPECIFIER,
    TAG_TYPE_STRUCT_OR_UNION_SPECIFIER,
    TAG_TYPE_ENUMERATOR,
    TAG_TYPE_DECLARATOR,
};


#ifdef TEST
int printf_nothing(const char* fmt, ...) { return 0; }
#endif

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

    //return pnew;
}

void scope_list_pop(struct scope_list* list)
{


    if (list->head == NULL)
        return;

    struct scope* p = list->tail;
    if (list->head == list->tail)
    {
        list->head = list->tail = NULL;
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
    return;

}



void parser_seterror_with_token(struct parser_ctx* ctx, struct token* p_token, const char* fmt, ...)
{
    ctx->n_errors++;
    int line = 0;
    if (p_token)
    {
        if (p_token->pFile)
        {
            line = p_token->line;
            ctx->printf(WHITE "%s:%d:%d: ",
                p_token->pFile->lexeme,
                p_token->line,
                p_token->col);
        }
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




    ctx->printf(LIGHTGRAY);

    char nbuffer[20] = { 0 };
    int n = snprintf(nbuffer, sizeof nbuffer, "%d", line);
    ctx->printf(" %s |", nbuffer);

    struct token* prev = p_token;
    while (prev && prev->prev && (prev->prev->type != TK_NEWLINE && prev->prev->type != TK_BEGIN_OF_FILE))
    {
        prev = prev->prev;
    }
    struct token* next = prev;
    while (next && (next->type != TK_NEWLINE && next->type != TK_BEGIN_OF_FILE))
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
    ctx->printf(LIGHTGRAY);
    ctx->printf(" %*s |", n, " ");
    if (p_token)
    {
        for (int i = 1; i < (p_token->col - 1); i++)
        {
            ctx->printf(" ");
        }
    }
    ctx->printf(LIGHTGREEN "^\n");
}


void parser_setwarning_with_token(struct parser_ctx* ctx, struct token* p_token, const char* fmt, ...)
{
    ctx->n_warnings++;
    int line = 0;
    if (p_token)
    {
        if (p_token->pFile)
        {
            line = p_token->line;
            ctx->printf(WHITE "%s:%d:%d: ",
                p_token->pFile->lexeme,
                p_token->line,
                p_token->col);
        }
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

    ctx->printf(LIGHTMAGENTA "warning: " WHITE "%s\n", buffer);




    ctx->printf(LIGHTGRAY);

    char nbuffer[20] = { 0 };
    int n = snprintf(nbuffer, sizeof nbuffer, "%d", line);
    ctx->printf(" %s |", nbuffer);

    struct token* prev = p_token;
    while (prev && prev->prev && (prev->prev->type != TK_NEWLINE && prev->prev->type != TK_BEGIN_OF_FILE))
    {
        prev = prev->prev;
    }
    struct token* next = prev;
    while (next && (next->type != TK_NEWLINE && next->type != TK_BEGIN_OF_FILE))
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
    ctx->printf(LIGHTGRAY);
    ctx->printf(" %*s |", n, " ");
    if (p_token)
    {
        for (int i = 1; i < (p_token->col - 1); i++)
        {
            ctx->printf(" ");
        }
    }
    ctx->printf(LIGHTGREEN "^\n");
}


void parser_set_info_with_token(struct parser_ctx* ctx, struct token* p_token, const char* fmt, ...)
{
    ctx->n_warnings++;
    int line = 0;
    if (p_token)
    {
        if (p_token->pFile)
        {
            line = p_token->line;
            ctx->printf(WHITE "%s:%d:%d: ",
                p_token->pFile->lexeme,
                p_token->line,
                p_token->col);
        }
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

    ctx->printf(LIGHTCYAN "info: " WHITE "%s\n", buffer);




    ctx->printf(LIGHTGRAY);

    char nbuffer[20] = { 0 };
    int n = snprintf(nbuffer, sizeof nbuffer, "%d", line);
    ctx->printf(" %s |", nbuffer);

    struct token* prev = p_token;
    while (prev && prev->prev && (prev->prev->type != TK_NEWLINE && prev->prev->type != TK_BEGIN_OF_FILE))
    {
        prev = prev->prev;
    }
    struct token* next = prev;
    while (next && (next->type != TK_NEWLINE && next->type != TK_BEGIN_OF_FILE))
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
        else
        {
            ctx->printf("%s", next->lexeme);
        }

        next = next->next;
    }
    ctx->printf("\n");
    ctx->printf(LIGHTGRAY);
    ctx->printf(" %*s |", n, " ");
    if (p_token)
    {
        for (int i = 1; i < (p_token->col - 1); i++)
        {
            ctx->printf(" ");
        }
    }
    ctx->printf(LIGHTGREEN "^\n");
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

    return token->type == TK_KEYWORD_INLINE || token->type == TK_KEYWORD__NORETURN;
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

bool first_of_atomic_type_specifier_token(struct token* token)
{
    if (token == NULL)
        return false;

    return token->type == TK_KEYWORD__ATOMIC;
}

bool first_of_atomic_type_specifier(struct parser_ctx* ctx)
{
    return first_of_atomic_type_specifier_token(ctx->current);
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
        p_token->type == TK_KEYWORD__ATOMIC;
    //__fastcall
    //__stdcall
}

bool first_of_type_qualifier(struct parser_ctx* ctx)
{
    return first_of_type_qualifier_token(ctx->current);
}


//declaração da macro container_of
#ifndef container_of
#define container_of(ptr , type , member) (type *)( (char *) ptr - offsetof(type , member) )
#endif


struct type_tag_id* find_tag(struct parser_ctx* ctx, const char* lexeme)
{
    struct scope* scope = ctx->scopes.tail;
    while (scope)
    {
        struct type_tag_id* type_id = hashmap_find(&scope->tags, lexeme);
        if (type_id)
        {
            return type_id;
        }
        scope = scope->previous;
    }
    return NULL;
}




struct type_tag_id* find_variables(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt)
{
    if (ppscope_opt != NULL)
        *ppscope_opt = NULL; //out

    struct scope* scope = ctx->scopes.tail;
    while (scope)
    {
        struct type_tag_id* type_id = hashmap_find(&scope->variables, lexeme);
        if (type_id)
        {
            if (ppscope_opt)
                *ppscope_opt = scope;
            return type_id;
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
        struct type_tag_id* type_id = hashmap_find(&scope->tags, lexeme);
        if (type_id &&
            type_id->type == TAG_TYPE_ENUN_SPECIFIER)
        {

            best = container_of(type_id, struct enum_specifier, type_id);
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

    struct struct_or_union_specifier* best = NULL;
    struct scope* scope = ctx->scopes.tail;
    while (scope)
    {
        struct type_tag_id* type_id = hashmap_find(&scope->tags, lexeme);
        if (type_id &&
            type_id->type == TAG_TYPE_STRUCT_OR_UNION_SPECIFIER)
        {

            best = container_of(type_id, struct struct_or_union_specifier, type_id);
            if (best->member_declaration_list.head != NULL)
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


struct declarator* find_declarator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt)
{
    struct type_tag_id* type_id = find_variables(ctx, lexeme, ppscope_opt);

    if (type_id && type_id->type == TAG_TYPE_DECLARATOR)
        return container_of(type_id, struct declarator, type_id);

    return NULL;
}

struct enumerator* find_enumerator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt)
{
    struct type_tag_id* type_id = find_variables(ctx, lexeme, ppscope_opt);

    if (type_id && type_id->type == TAG_TYPE_ENUMERATOR)
        return container_of(type_id, struct enumerator, type_id);

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
    struct token* pAhead = parser_look_ahead(ctx);
    return first_of_type_specifier_token(ctx, pAhead) ||
        first_of_type_qualifier_token(pAhead);
}

bool first_of_type_name(struct parser_ctx* ctx)
{
    return first_of_type_specifier(ctx) || first_of_type_qualifier(ctx);
}

bool first_of_pointer(struct parser_ctx* ctx)
{
    return first_is(ctx, '*');
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
        first_of_atomic_type_specifier_token(p_token) ||
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
    case 'N':
        /*extension NULL alias for nullptr*/
        if (strcmp("NULL", text) == 0) result = TK_KEYWORD_NULLPTR;
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

        break;
    case 't':
        if (strcmp("typedef", text) == 0) result = TK_KEYWORD_TYPEDEF;
        else if (strcmp("typeof", text) == 0) result = TK_KEYWORD_TYPEOF; /*C23*/
        else if (strcmp("typeof_unqual", text) == 0) result = TK_KEYWORD_TYPEOF_UNQUAL; /*C23*/
        else if (strcmp("typeid", text) == 0) result = TK_KEYWORD_TYPEID; /*EXTENSION*/
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
        else if (strcmp("_Hashof", text) == 0) result = TK_KEYWORD_HASHOF;
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
        break;
    default:
        break;
    }
    return result;
}


static void token_promote(struct token* token, struct error* error)
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
        token->type = parse_number(token->lexeme, NULL, error);
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
        struct error error = { 0 };
        token_promote(p, &error);
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



enum token_type parse_number_core(struct stream* stream, enum type_specifier_flags* flags_opt, struct error* error)
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


enum token_type parse_number(const char* lexeme, enum type_specifier_flags* flags_opt, struct error* error)
{
    struct stream stream = { .source = lexeme, .current = lexeme, .line = 1, .col = 1 };
    return parse_number_core(&stream, flags_opt, error);
}

struct token* parser_skip_blanks(struct parser_ctx* ctx)
{
    while (ctx->current && !(ctx->current->flags & TK_FLAG_FINAL))
    {
        ctx->current = ctx->current->next;
    }

    if (ctx->current)
    {
        struct error error = { 0 };
        token_promote(ctx->current, &error); //transforma para token de parser
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



void parser_match_tk(struct parser_ctx* ctx, enum token_type type, struct error* error)
{
    if (error->code != 0)
        return;
    if (ctx->current == NULL)
    {
        parser_seterror_with_token(ctx, ctx->input_list.tail, "unexpected end of file after");
        error->code = 1;
        return;
    }

    if (ctx->current->type != type)
    {
        parser_seterror_with_token(ctx, ctx->current, "expected %s", get_token_name(type));
        return;
    }
    ctx->current = ctx->current->next;
    parser_skip_blanks(ctx);
    return;
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
        //
        if (p_declaration_specifiers->struct_or_union_specifier->tag_name)
            ss_fprintf(ss, "struct %s", p_declaration_specifiers->struct_or_union_specifier->tag_name);
        else
            assert(false);
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
int final_specifier(struct parser_ctx* ctx,
    enum type_specifier_flags* flags,
    struct error* error)
{
    ctx;
    if (((*flags) & TYPE_SPECIFIER_UNSIGNED) ||
        ((*flags) & TYPE_SPECIFIER_SIGNED))
    {
        if (!type_specifier_is_integer(*flags))
        {
            //se nao especificou nada vira integer
            (*flags) |= TYPE_SPECIFIER_INT;
        }
    }


    return error->code;
}

int add_specifier(struct parser_ctx* ctx,
    enum type_specifier_flags* flags,
    enum type_specifier_flags newFlag,
    struct error* error)
{
    /*
    * Verifica as combinaçòes possíveis
    */

    if (newFlag & TYPE_SPECIFIER_LONG) //adicionando um long
    {
        if ((*flags) & TYPE_SPECIFIER_LONG_LONG) //ja tinha long long
        {
            parser_seterror_with_token(ctx, ctx->current, "cannot combine with previous 'long long' declaration specifier");
            error->code = 1;

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
        (*flags) |= newFlag;
    }
    return error->code;
}

struct declaration_specifiers* declaration_specifiers(struct parser_ctx* ctx, struct error* error)
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

    struct declaration_specifiers* p_declaration_specifiers = calloc(1, sizeof(struct declaration_specifiers));

    try
    {
        while (error->code == 0 &&
            first_of_declaration_specifier(ctx))
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

            struct declaration_specifier* p_declaration_specifier = declaration_specifier(ctx, error);

            if (p_declaration_specifier->type_specifier_qualifier)
            {
                if (p_declaration_specifier->type_specifier_qualifier)
                {
                    if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                    {

                        if (add_specifier(ctx,
                            &p_declaration_specifiers->type_specifier_flags,
                            p_declaration_specifier->type_specifier_qualifier->type_specifier->flags,
                            error) != 0)
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

            list_add(p_declaration_specifiers, p_declaration_specifier);
            attribute_specifier_sequence_opt(ctx, error);

            if (ctx->current->type == TK_IDENTIFIER &&
                p_declaration_specifiers->type_specifier_flags != TK_NONE)
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
    }
    catch
    {
    }

    final_specifier(ctx, &p_declaration_specifiers->type_specifier_flags, error);

    return p_declaration_specifiers;
}

struct declaration* declaration_core(struct parser_ctx* ctx, bool canBeFunctionDefinition, bool* is_function_definition, struct error* error)
{
    /*
                                  declaration-specifiers init-declarator-list_opt ;
     attribute-specifier-sequence declaration-specifiers init-declarator-list ;
     static_assert-declaration
     attribute-declaration
 */

    struct declaration* p_declaration = calloc(1, sizeof(struct declaration));

    p_declaration->first_token = ctx->current;

    if (ctx->current->type == ';')
    {
        parser_match_tk(ctx, ';', error);
        //declaracao vazia nao eh erro
        return p_declaration;
    }

    if (first_of_static_assert_declaration(ctx))
    {
        p_declaration->static_assert_declaration = static_assert_declaration(ctx, error);
    }
    else
    {
        attribute_specifier_sequence_opt(ctx, error); //se tem aqui initi nao eh opcional!TODO
        if (first_of_declaration_specifier(ctx))
        {

            p_declaration->declaration_specifiers = declaration_specifiers(ctx, error);
            if (ctx->current->type != ';')
                p_declaration->init_declarator_list = init_declarator_list(ctx, p_declaration->declaration_specifiers, error);


            p_declaration->last_token = ctx->current;

            if (first_is(ctx, '{'))
            {
                if (canBeFunctionDefinition)
                    *is_function_definition = true;
                else
                {
                    assert(false);
                    error->code = 1;
                }
            }
            else
                parser_match_tk(ctx, ';', error);
        }
        else
        {
            parser_seterror_with_token(ctx, ctx->current, "unknown type name '%s'", ctx->current->lexeme);
            error->code = 1;
        }
    }


    return p_declaration;
}

struct declaration* function_definition_or_declaration(struct parser_ctx* ctx, struct error* error)
{

    bool is_function_definition = false;
    struct declaration* p_declaration = declaration_core(ctx, true, &is_function_definition, error);
    if (is_function_definition)
    {
        naming_convention_function(ctx, p_declaration->init_declarator_list.head->declarator->direct_declarator->name);

        ctx->p_current_function_opt = p_declaration;
        //tem que ter 1 so
        //tem 1 que ter  1 cara e ser funcao
        assert(p_declaration->init_declarator_list.head->declarator->direct_declarator->array_function_list.head->function_declarator);

        struct scope* parameters_scope =
            &p_declaration->init_declarator_list.head->declarator->direct_declarator->array_function_list.head->function_declarator->parameters_scope;


        scope_list_push(&ctx->scopes, parameters_scope);


        //o function_prototype_scope era um block_scope
        p_declaration->function_body = function_body(ctx, error);



        struct parameter_declaration* parameter = NULL;

        if (p_declaration->init_declarator_list.head->declarator->direct_declarator->array_function_list.head->function_declarator &&
            p_declaration->init_declarator_list.head->declarator->direct_declarator->array_function_list.head->function_declarator->parameter_type_list_opt &&
            p_declaration->init_declarator_list.head->declarator->direct_declarator->array_function_list.head->function_declarator->parameter_type_list_opt->parameter_list)
        {
            parameter = p_declaration->init_declarator_list.head->declarator->direct_declarator->array_function_list.head->function_declarator->parameter_type_list_opt->parameter_list->head;
        }

        /*parametros nao usados*/
        while (parameter)
        {
            if (parameter->declarator->num_uses == 0)
            {
                if (parameter->name &&
                    parameter->name->level == 0 /*direct source*/
                    )
                {

                    ctx->printf(WHITE "%s:%d:%d: ",
                        parameter->name->pFile->lexeme,
                        parameter->name->line,
                        parameter->name->col);

                    ctx->printf(LIGHTMAGENTA "warning: " WHITE "'%s': unreferenced formal parameter\n",
                        parameter->name->lexeme);
                }
            }
            parameter = parameter->next;
        }


        scope_list_pop(&ctx->scopes);
        ctx->p_current_function_opt = NULL;
    }
    else
    {
        struct init_declarator* p = p_declaration->init_declarator_list.head;
        while (p)
        {
            if (p->declarator && p->declarator->name)
            {
                
                naming_convention_global_var(ctx,
                    p->declarator->name,
                    &p->declarator->type,
                    p_declaration->declaration_specifiers->storage_class_specifier_flags);
            }
            p = p->next;
        }
    }

    return p_declaration;
}

struct declaration* declaration(struct parser_ctx* ctx, struct error* error)
{
    bool is_function_definition;
    return declaration_core(ctx, false, &is_function_definition, error);
}


//(6.7) declaration-specifiers:
//declaration-specifier attribute-specifier-sequenceopt
//declaration-specifier declaration-specifiers



struct declaration_specifier* declaration_specifier(struct parser_ctx* ctx, struct error* error)
{
    //    storage-class-specifier
    //    type-specifier-qualifier
    //    function-specifier
    struct declaration_specifier* pDeclaration_specifier = calloc(1, sizeof * pDeclaration_specifier);
    if (first_of_storage_class_specifier(ctx))
    {
        pDeclaration_specifier->storage_class_specifier = storage_class_specifier(ctx);
    }
    else if (first_of_type_specifier_qualifier(ctx))
    {
        pDeclaration_specifier->type_specifier_qualifier = type_specifier_qualifier(ctx, error);
    }
    else if (first_of_function_specifier(ctx))
    {
        pDeclaration_specifier->function_specifier = function_specifier(ctx, error);
    }
    else
    {
        parser_seterror_with_token(ctx, ctx->current, "unexpected");
    }
    return pDeclaration_specifier;
}


struct init_declarator* init_declarator(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct error* error)
{
    /*
     init-declarator:
       declarator
       declarator = initializer
    */
    struct init_declarator* p_init_declarator = calloc(1, sizeof(struct init_declarator));
    try
    {

        struct token* tkname = 0;
        p_init_declarator->declarator = declarator(ctx,
            NULL,
            p_declaration_specifiers,
            false,
            &tkname,
            error);
        if (tkname == NULL)
        {
            parser_seterror_with_token(ctx, ctx->current, "empty declarator name?? unexpected");
            error->code = 1;
            return p_init_declarator;
        }

        p_init_declarator->declarator->declaration_specifiers = p_declaration_specifiers;
        p_init_declarator->declarator->name = tkname;

        p_init_declarator->declarator->type =
            make_type_using_declarator(ctx, p_init_declarator->declarator);

        if (error->code != 0) throw;
        const char* name = p_init_declarator->declarator->name->lexeme;
        if (name)
        {
            //TODO se ja existe?
            hashmap_set(&ctx->scopes.tail->variables, name, &p_init_declarator->declarator->type_id);
        }
        else
        {
            assert(false);
        }
        if (ctx->current && ctx->current->type == '=')
        {
            parser_match(ctx);
            p_init_declarator->initializer = initializer(ctx, error);
        }
    }
    catch
    {
    }
    return p_init_declarator;
}

struct init_declarator_list init_declarator_list(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct error* error)
{
    /*
    init-declarator-list:
      init-declarator
      init-declarator-list , init-declarator
    */
    struct init_declarator_list init_declarator_list = { 0 };
    list_add(&init_declarator_list, init_declarator(ctx, p_declaration_specifiers, error));
    while (error->code == 0 &&
        ctx->current != NULL && ctx->current->type == ',')
    {
        parser_match(ctx);
        list_add(&init_declarator_list, init_declarator(ctx, p_declaration_specifiers, error));
        if (error->code) break;
    }
    return init_declarator_list;
}

struct storage_class_specifier* storage_class_specifier(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return NULL;

    struct storage_class_specifier* new_storage_class_specifier = calloc(1, sizeof(struct storage_class_specifier));
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

struct typeof_specifier_argument* typeof_specifier_argument(struct parser_ctx* ctx, struct error* error)
{
    struct typeof_specifier_argument* new_typeof_specifier_argument = calloc(1, sizeof(struct typeof_specifier_argument));
    if (new_typeof_specifier_argument == NULL)
        return NULL;

    if (first_of_type_name(ctx))
    {
        new_typeof_specifier_argument->type_name = type_name(ctx, error);
    }
    else
    {
        struct expression_ctx ectx = { 0 };
        new_typeof_specifier_argument->expression = expression(ctx, error, &ectx);
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

struct typeof_specifier* typeof_specifier(struct parser_ctx* ctx, struct error* error)
{
    struct typeof_specifier* p_typeof_specifier = calloc(1, sizeof(struct typeof_specifier));

    p_typeof_specifier->token = ctx->current;
    parser_match(ctx);
    parser_match_tk(ctx, '(', error);

    p_typeof_specifier->typeof_specifier_argument =
        typeof_specifier_argument(ctx, error);

    p_typeof_specifier->endtoken = ctx->current;
    parser_match_tk(ctx, ')', error);

    return p_typeof_specifier;
}

struct type_specifier* type_specifier(struct parser_ctx* ctx, struct error* error)
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

    struct type_specifier* p_type_specifier = calloc(1, sizeof * p_type_specifier);




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
        p_type_specifier->typeof_specifier = typeof_specifier(ctx, error);
    }
    else if (first_of_atomic_type_specifier(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_ATOMIC;
        p_type_specifier->atomic_type_specifier = atomic_type_specifier(ctx, error);
    }
    else if (first_of_struct_or_union(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_STRUCT_OR_UNION;
        p_type_specifier->struct_or_union_specifier = struct_or_union_specifier(ctx, error);
    }
    else if (first_of_enum_specifier(ctx))
    {
        p_type_specifier->token = ctx->current;
        p_type_specifier->flags = TYPE_SPECIFIER_ENUM;
        p_type_specifier->enum_specifier = enum_specifier(ctx, error);
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

struct struct_or_union_specifier* struct_or_union_specifier(struct parser_ctx* ctx, struct error* error)
{
    struct struct_or_union_specifier* pStruct_or_union_specifier = calloc(1, sizeof * pStruct_or_union_specifier);
    pStruct_or_union_specifier->type_id.type = TAG_TYPE_STRUCT_OR_UNION_SPECIFIER;

    if (ctx->current->type == TK_KEYWORD_STRUCT ||
        ctx->current->type == TK_KEYWORD_UNION)
    {
        pStruct_or_union_specifier->first = ctx->current;
        parser_match(ctx);
    }
    else
    {
        assert(false);
    }

    pStruct_or_union_specifier->attribute_specifier_sequence =
        attribute_specifier_sequence_opt(ctx, error);

    struct struct_or_union_specifier* pPreviousTagInThisScope = NULL;

    if (ctx->current->type == TK_IDENTIFIER)
    {
        pStruct_or_union_specifier->tagtoken = ctx->current;
        /*
         Structure, union, and enumeration tags have scope that begins just after the
         appearance of the tag in a type specifier that declares the tag.
        */

        snprintf(pStruct_or_union_specifier->tag_name, sizeof pStruct_or_union_specifier->tag_name, "%s", ctx->current->lexeme);

        struct type_tag_id* tag_type_id = hashmap_find(&ctx->scopes.tail->tags, ctx->current->lexeme);
        if (tag_type_id)
        {
            /*este tag já existe neste escopo*/
            if (tag_type_id->type == TAG_TYPE_STRUCT_OR_UNION_SPECIFIER)
            {
                pPreviousTagInThisScope = container_of(tag_type_id, struct struct_or_union_specifier, type_id);
                pStruct_or_union_specifier->complete_struct_or_union_specifier = pPreviousTagInThisScope;
            }
            else
            {
                parser_seterror_with_token(ctx, ctx->current, "use of '%s' with tag type that does not match previous declaration.", ctx->current->lexeme);
            }
        }
        else
        {
            /*ok neste escopo nao tinha este tag..vamos escopos para cima*/
            struct struct_or_union_specifier* pOther = find_struct_or_union_specifier(ctx, ctx->current->lexeme);
            if (pOther == NULL)
            {
                pStruct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
                /*nenhum escopo tinha este tag vamos adicionar no escopo local*/
                hashmap_set(&ctx->scopes.tail->tags, ctx->current->lexeme, &pStruct_or_union_specifier->type_id);
            }
            else
            {
                /*achou a tag em um escopo mais a cima*/
                pStruct_or_union_specifier->complete_struct_or_union_specifier = pOther;
            }
        }

        parser_match(ctx);
    }
    else
    {
        /*struct sem tag, neste caso vou inventar um tag "oculto" e adicionar no escopo atual*/
        snprintf(pStruct_or_union_specifier->tag_name, sizeof pStruct_or_union_specifier->tag_name, "_anonymous_struct_%d", anonymous_struct_count);
        anonymous_struct_count++;
        pStruct_or_union_specifier->has_anonymous_tag = true;
        pStruct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
        hashmap_set(&ctx->scopes.tail->tags, pStruct_or_union_specifier->tag_name, &pStruct_or_union_specifier->type_id);
    }



    if (ctx->current->type == '{')
    {
        if (pStruct_or_union_specifier->tagtoken)
            naming_convention_struct_tag(ctx, pStruct_or_union_specifier->tagtoken);

        struct token* first = ctx->current;
        parser_match(ctx);
        pStruct_or_union_specifier->member_declaration_list = member_declaration_list(ctx, error);
        pStruct_or_union_specifier->member_declaration_list.first = first;
        pStruct_or_union_specifier->last = ctx->current;
        pStruct_or_union_specifier->member_declaration_list.last = ctx->current;
        parser_match_tk(ctx, '}', error);

    }
    else
    {
        pStruct_or_union_specifier->last = ctx->current;
    }

    if (pPreviousTagInThisScope)
    {
        if (pPreviousTagInThisScope->member_declaration_list.head == NULL &&
            pStruct_or_union_specifier->member_declaration_list.head != NULL)
        {
            /*
              Temos uma versao mais completa deste tag neste escopo. Vamos ficar com ela.
            */
            hashmap_set(&ctx->scopes.tail->tags, pStruct_or_union_specifier->tag_name, &pStruct_or_union_specifier->type_id);
        }
    }

    return pStruct_or_union_specifier;
}

struct member_declarator* member_declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* p_specifier_qualifier_list,
    struct error* error)
{
    /*
    member-declarator:
     declarator
     declaratoropt : constant-expression
    */
    struct member_declarator* p_member_declarator = calloc(1, sizeof(struct member_declarator));
    //struct declarator* pdeclarator = calloc(1, sizeof * pdeclarator);
    p_member_declarator->declarator = declarator(ctx, p_specifier_qualifier_list, /*declaration_specifiers*/NULL, false, &p_member_declarator->name, error);
    p_member_declarator->declarator->specifier_qualifier_list = p_specifier_qualifier_list;

    p_member_declarator->declarator->type =
        make_type_using_declarator(ctx, p_member_declarator->declarator);

    if (p_member_declarator->name)
        naming_convention_struct_member(ctx, p_member_declarator->name, &p_member_declarator->declarator->type);

    if (ctx->current->type == ':')
    {
        parser_match(ctx);
        struct expression_ctx ectx = { 0 };
        p_member_declarator->constant_expression = constant_expression(ctx, error, &ectx);
    }
    return p_member_declarator;
}

struct member_declarator_list* member_declarator_list(struct parser_ctx* ctx,
    struct specifier_qualifier_list* p_specifier_qualifier_list,
    struct error* error)
{
    struct member_declarator_list* p_member_declarator_list = calloc(1, sizeof(struct member_declarator_list));
    list_add(p_member_declarator_list, member_declarator(ctx, p_specifier_qualifier_list, error));
    while (error->code == 0 &&
        ctx->current->type == ',')
    {
        parser_match(ctx);
        list_add(p_member_declarator_list, member_declarator(ctx, p_specifier_qualifier_list, error));
    }
    return p_member_declarator_list;
}


struct member_declaration_list member_declaration_list(struct parser_ctx* ctx, struct error* error)
{
    struct member_declaration_list list = { 0 };
    //member_declaration
    //member_declaration_list member_declaration


    list_add(&list, member_declaration(ctx, error));

    while (error->code == 0 &&
        ctx->current->type != '}')
    {
        list_add(&list, member_declaration(ctx, error));
    }
    return list;
}

struct member_declaration* member_declaration(struct parser_ctx* ctx, struct error* error)
{
    struct member_declaration* p_member_declaration = calloc(1, sizeof(struct member_declaration));
    //attribute_specifier_sequence_opt specifier_qualifier_list member_declarator_list_opt ';'
    //static_assert_declaration
    if (ctx->current->type == TK_KEYWORD__STATIC_ASSERT)
    {
        p_member_declaration->static_assert_declaration = static_assert_declaration(ctx, error);
    }
    else
    {
        attribute_specifier_sequence_opt(ctx, error);
        p_member_declaration->specifier_qualifier_list = specifier_qualifier_list(ctx, error);
        if (ctx->current->type != ';')
        {
            p_member_declaration->member_declarator_list_opt = member_declarator_list(ctx, p_member_declaration->specifier_qualifier_list, error);
        }
        parser_match_tk(ctx, ';', error);
    }
    return p_member_declaration;
}

struct member_declarator* find_member_declarator(struct member_declaration_list* list, const char* name)
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
                if (strcmp(p_member_declarator->name->lexeme, name) == 0)
                {
                    return p_member_declarator;
                }
                p_member_declarator = p_member_declarator->next;
            }
        }
        else
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
                struct member_declaration_list* list =
                    &p_member_declaration->specifier_qualifier_list->struct_or_union_specifier->member_declaration_list;
                struct member_declarator* p = find_member_declarator(list, name);
                if (p)
                {
                    return p;
                }
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

        //TODO
        assert(false);

    }
    else if (p_specifier_qualifier_list->struct_or_union_specifier)
    {
        if (p_specifier_qualifier_list->struct_or_union_specifier->tag_name)
            ss_fprintf(ss, "struct %s", p_specifier_qualifier_list->struct_or_union_specifier->tag_name);
        else
        {
            assert(false);
        }
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



struct specifier_qualifier_list* specifier_qualifier_list(struct parser_ctx* ctx, struct error* error)
{
    struct specifier_qualifier_list* p_specifier_qualifier_list = calloc(1, sizeof(struct specifier_qualifier_list));
    /*
      type_specifier_qualifier attribute_specifier_sequence_opt
      type_specifier_qualifier specifier_qualifier_list
    */
    try
    {
        while (error->code == 0 && ctx->current != NULL &&
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

            struct type_specifier_qualifier* p_type_specifier_qualifier = type_specifier_qualifier(ctx, error);

            if (p_type_specifier_qualifier->type_specifier)
            {
                if (add_specifier(ctx,
                    &p_specifier_qualifier_list->type_specifier_flags,
                    p_type_specifier_qualifier->type_specifier->flags,
                    error) != 0)
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

            list_add(p_specifier_qualifier_list, p_type_specifier_qualifier);
            attribute_specifier_sequence_opt(ctx, error);
        }
    }
    catch
    {
    }

    final_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags, error);
    return p_specifier_qualifier_list;
}

struct type_specifier_qualifier* type_specifier_qualifier(struct parser_ctx* ctx, struct error* error)
{
    struct type_specifier_qualifier* type_specifier_qualifier = calloc(1, sizeof * type_specifier_qualifier);
    //type_specifier
    //type_qualifier
    //alignment_specifier
    if (first_of_type_specifier(ctx))
    {
        type_specifier_qualifier->type_specifier = type_specifier(ctx, error);
    }
    else if (first_of_type_qualifier(ctx))
    {
        type_specifier_qualifier->type_qualifier = type_qualifier(ctx, error);
    }
    else if (first_of_alignment_specifier(ctx))
    {
        type_specifier_qualifier->alignment_specifier = alignment_specifier(ctx, error);
    }
    else
    {
        assert(false);
    }
    return type_specifier_qualifier;
}


struct enum_specifier* enum_specifier(struct parser_ctx* ctx, struct error* error)
{
    /*
        enum-specifier:

        enum attribute-specifier-sequenceopt identifieropt enum-type-specifieropt
        { enumerator-list }

        enum attribute-specifier-sequenceopt identifieropt enum-type-specifieropt
        { enumerator-list , }
        enum identifier enum-type-specifiero
    */
    struct enum_specifier* p_enum_specifier = NULL;
    try
    {
        p_enum_specifier = calloc(1, sizeof * p_enum_specifier);
        p_enum_specifier->type_id.type = TAG_TYPE_ENUN_SPECIFIER;

        parser_match_tk(ctx, TK_KEYWORD_ENUM, error);

        attribute_specifier_sequence_opt(ctx, error);
        struct enum_specifier* pPreviousTagInThisScope = NULL;
        bool bHasIdentifier = false;
        if (ctx->current->type == TK_IDENTIFIER)
        {
            bHasIdentifier = true;
            p_enum_specifier->tag_token = ctx->current;
            parser_match(ctx);
        }

        if (ctx->current->type == ':')
        {
            /*C23*/
            parser_match(ctx);
            p_enum_specifier->type_specifier_qualifier = type_specifier_qualifier(ctx, error);
        }

        if (ctx->current->type == '{')
        {
            if (p_enum_specifier->tag_token)
                naming_convention_enum_tag(ctx, p_enum_specifier->tag_token);

            /*TODO redeclaration?*/
            /*adicionar no escopo*/
            if (p_enum_specifier->tag_token)
            {
                hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_token->lexeme, &p_enum_specifier->type_id);
            }

            /*self*/
            p_enum_specifier->complete_enum_specifier = p_enum_specifier;

            parser_match_tk(ctx, '{', error);
            p_enum_specifier->enumerator_list = enumerator_list(ctx, error);
            if (ctx->current->type == ',')
            {
                parser_match(ctx);
            }
            parser_match_tk(ctx, '}', error);
        }
        else
        {
            if (!bHasIdentifier)
            {
                parser_seterror_with_token(ctx, ctx->current, "missing enum tag name");
                error->code = 1;
                throw;
            }


            /*searches for this tag in the current scope*/
            struct type_tag_id* tag_type_id = hashmap_find(&ctx->scopes.tail->tags, p_enum_specifier->tag_token->lexeme);
            if (tag_type_id)
            {
                /*we have this tag at this scope*/
                if (tag_type_id->type == TAG_TYPE_ENUN_SPECIFIER)
                {
                    pPreviousTagInThisScope = container_of(tag_type_id, struct enum_specifier, type_id);
                    p_enum_specifier->complete_enum_specifier = pPreviousTagInThisScope;
                }
                else
                {
                    parser_seterror_with_token(ctx, ctx->current, "use of '%s' with tag type that does not match previous declaration.", ctx->current->lexeme);
                    throw;
                }
            }
            else
            {
                struct enum_specifier* pOther = find_enum_specifier(ctx, p_enum_specifier->tag_token->lexeme);
                /*ok neste escopo nao tinha este tag..vamos escopos para cima*/
                if (pOther == NULL)
                {
                    hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_token->lexeme, &p_enum_specifier->type_id);
                }
                else
                {
                    /*achou a tag em um escopo mais a cima*/
                    p_enum_specifier->complete_enum_specifier = pOther;
                }
            }


        }
    }
    catch
    {}
    return p_enum_specifier;
}

struct enumerator_list enumerator_list(struct parser_ctx* ctx, struct error* error)
{
    struct enumerator_list enumeratorlist = { 0 };
    try
    {

        //enumerator
        //enumerator_list ',' enumerator
        list_add(&enumeratorlist, enumerator(ctx, error));
        if (error->code != 0) throw;

        while (ctx->current != NULL &&
            ctx->current->type == ',')
        {
            parser_match(ctx);  /*pode ter uma , vazia no fim*/

            if (ctx->current->type != '}')
                list_add(&enumeratorlist, enumerator(ctx, error));

            if (error->code != 0) throw;
        }
    }
    catch
    {
    }

    return enumeratorlist;
}

struct enumerator* enumerator(struct parser_ctx* ctx, struct error* error)
{
    //TODO VALUE
    struct enumerator* p_enumerator = calloc(1, sizeof(struct enumerator));
    p_enumerator->type_id.type = TAG_TYPE_ENUMERATOR;

    //enumeration_constant attribute_specifier_sequence_opt
    //enumeration_constant attribute_specifier_sequence_opt '=' constant_expression
    struct token* name = ctx->current;

    naming_convention_enumerator(ctx, name);


    parser_match_tk(ctx, TK_IDENTIFIER, error);
    attribute_specifier_sequence_opt(ctx, error);


    p_enumerator->token = name;
    hashmap_set(&ctx->scopes.tail->variables, p_enumerator->token->lexeme, &p_enumerator->type_id);

    if (ctx->current->type == '=')
    {
        parser_match(ctx);
        struct expression_ctx ectx = { .bConstantExpressionRequired = true };
        p_enumerator->constant_expression_opt = constant_expression(ctx, error, &ectx);
        p_enumerator->value = p_enumerator->constant_expression_opt->constant_value;
    }

    return p_enumerator;
}




struct alignment_specifier* alignment_specifier(struct parser_ctx* ctx, struct error* error)
{
    struct alignment_specifier* alignment_specifier = calloc(1, sizeof * alignment_specifier);
    alignment_specifier->token = ctx->current;
    parser_match_tk(ctx, TK_KEYWORD__ALIGNAS, error);
    parser_match_tk(ctx, '(', error);
    if (first_of_type_name(ctx))
    {

        type_name(ctx, error);

    }
    else
    {
        struct expression_ctx ectx = { .bConstantExpressionRequired = true };
        constant_expression(ctx, error, &ectx);
    }
    parser_match_tk(ctx, ')', error);
    return alignment_specifier;
}



struct atomic_type_specifier* atomic_type_specifier(struct parser_ctx* ctx, struct error* error)
{
    //'_Atomic' '(' type_name ')'
    struct atomic_type_specifier* p = calloc(1, sizeof * p);
    p->token = ctx->current;
    parser_match_tk(ctx, TK_KEYWORD__ATOMIC, error);
    parser_match_tk(ctx, '(', error);
    type_name(ctx, error);
    parser_match_tk(ctx, ')', error);
    return p;
}


struct type_qualifier* type_qualifier(struct parser_ctx* ctx, struct error* error)
{
    if (error->code) return NULL;
    struct type_qualifier* p_type_qualifier = calloc(1, sizeof * p_type_qualifier);
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
    }
    //'const'
    //'restrict'
    //'volatile'
    //'_Atomic'
    parser_match(ctx);
    return p_type_qualifier;
}
//

struct type_qualifier* type_qualifier_opt(struct parser_ctx* ctx, struct error* error)
{
    if (first_of_type_qualifier(ctx))
    {
        return type_qualifier(ctx, error);
    }
    return NULL;
}


struct function_specifier* function_specifier(struct parser_ctx* ctx, struct error* error)
{
    if (error->code) return NULL;
    struct function_specifier* p = calloc(1, sizeof * p);
    p->token = ctx->current;
    parser_match(ctx);
    //'inline'
    //'_Noreturn'
    return p;
}


struct declarator* declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* p_specifier_qualifier_list,
    struct declaration_specifiers* p_declaration_specifiers,
    bool bAbstractAcceptable,
    struct token** pptokenName,
    struct error* error)
{
    /*
      declarator:
      pointer_opt direct-declarator
    */
    struct declarator* p_declarator = calloc(1, sizeof(struct declarator));
    p_declarator->type_id.type = TAG_TYPE_DECLARATOR;
    p_declarator->pointer = pointer_opt(ctx, error);
    p_declarator->direct_declarator = direct_declarator(ctx, p_specifier_qualifier_list, p_declaration_specifiers, bAbstractAcceptable, pptokenName, error);


    return p_declarator;
}

const char* declarator_get_name(struct declarator* p_declarator)
{
    if (p_declarator->direct_declarator)
        return p_declarator->direct_declarator->name->lexeme;
    assert(false);
    return NULL;
}

bool declarator_is_function(struct declarator* p_declarator)
{
    return (p_declarator->direct_declarator &&
        p_declarator->direct_declarator->array_function_list.head &&
        p_declarator->direct_declarator->array_function_list.head->function_declarator != NULL);

}

struct direct_declarator* direct_declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* p_specifier_qualifier_list,
    struct declaration_specifiers* p_declaration_specifiers,
    bool bAbstractAcceptable,
    struct token** pptokenName,
    struct error* error)
{
    /*
    direct-declarator:
     identifier attribute-specifier-sequenceopt
     ( declarator )

     array-declarator attribute-specifier-sequenceopt
     function-declarator attribute-specifier-sequenceopt
    */
    struct direct_declarator* p_direct_declarator = calloc(1, sizeof(struct direct_declarator));
    try
    {
        if (ctx->current == NULL)
        {
            return p_direct_declarator;
        }

        struct token* p_token_ahead = parser_look_ahead(ctx);
        if (ctx->current->type == TK_IDENTIFIER)
        {
            struct scope* pscope = NULL;
            struct declarator* pdeclarator =
                find_declarator(ctx, ctx->current->lexeme, &pscope);
            if (pdeclarator)
            {



                if (pscope == ctx->scopes.tail)
                {
                    if (pscope->scope_level != 0)
                    {
                        if (declarator_is_function(pdeclarator))
                        {
                            //redeclaracao de algo q era funcao agora nao eh nao eh problema
                        }
                        else
                        {
                            //TODO tem que ver se esta dentro struct dai nao pode dar erro
                            //parser_seterror_with_token(ctx, ctx->current, "redefinition of '%s'", ctx->current->lexeme);
                        }
                    }
                    else
                    {
                        //global aceita redefinicao!
                        //TODO ver seh eh o mesmo                        
                    }
                }
                else
                {
                    if (pscope->scope_level != 0)
                    {
                        if (pdeclarator->direct_declarator &&
                            pdeclarator->direct_declarator->name &&
                            pdeclarator->direct_declarator->name->pFile)
                        {
                            //TODO ver se esta dentro de struct
                            //printf("warning '%s' at line %d hides previous definition %d\n",
                              //  ctx->current->lexeme,
                                //ctx->current->line,                                
                                //pdeclarator->direct_declarator->name->line);
                        }
                    }

                    //parser_seterror_with_token(ctx, ctx->current, "redefinition of '%s'", ctx->current->lexeme);
                }
            }

            p_direct_declarator->name = ctx->current;
            if (pptokenName != NULL)
            {
                *pptokenName = ctx->current;
            }
            else
            {
                seterror(error, "unexpected name on declarator (abstract?)");
                throw;
            }

            parser_match(ctx);
            attribute_specifier_sequence_opt(ctx, error);
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

                p_direct_declarator->declarator = declarator(ctx,
                    p_specifier_qualifier_list,
                    p_declaration_specifiers,
                    bAbstractAcceptable,
                    pptokenName,
                    error);


                parser_match(ctx); //)
            }
        }


        while (error->code == 0 && ctx->current != NULL &&
            (ctx->current->type == '[' || ctx->current->type == '('))
        {
            struct array_function* p_array_function = calloc(1, sizeof(struct array_function));
            list_add(&p_direct_declarator->array_function_list, p_array_function);
            if (ctx->current->type == '[')
            {
                p_array_function->array_declarator = array_declarator(ctx, error);
            }
            else if (ctx->current->type == '(')
            {

                p_array_function->function_declarator = function_declarator(ctx, error);
            }
        }
    }
    catch
    {
    }

    return p_direct_declarator;
}


struct array_declarator* array_declarator(struct parser_ctx* ctx, struct error* error)
{
    //direct_declarator '['          type_qualifier_list_opt           assignment_expression_opt ']'
    //direct_declarator '[' 'static' type_qualifier_list_opt           assignment_expression     ']'
    //direct_declarator '['          type_qualifier_list      'static' assignment_expression     ']'
    //direct_declarator '['          type_qualifier_list_opt  '*'           ']'

    struct array_declarator* p_array_declarator = NULL;
    try
    {
        if (error->code != 0)
            throw;

        p_array_declarator = calloc(1, sizeof * p_array_declarator);
        if (p_array_declarator == NULL)
            throw;

        parser_match_tk(ctx, '[', error);
        if (error->code != 0) throw;

        bool bIsStatic = false;
        if (ctx->current->type == TK_KEYWORD_STATIC)
        {
            parser_match(ctx);
            bIsStatic = true;
        }

        if (first_of_type_qualifier(ctx))
        {
            p_array_declarator->type_qualifier_list_opt = type_qualifier_list(ctx, error);
        }

        if (!bIsStatic)
        {
            if (ctx->current->type == TK_KEYWORD_STATIC)
            {
                parser_match(ctx);
                bIsStatic = true;
            }
        }

        if (bIsStatic)
        {
            //tem que ter..
            struct expression_ctx ectx = { 0 };
            p_array_declarator->assignment_expression = assignment_expression(ctx, error, &ectx);
            if (error->code != 0) throw;

            p_array_declarator->constant_size = p_array_declarator->assignment_expression->constant_value;
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
                struct expression_ctx ectx = { 0 };
                p_array_declarator->assignment_expression = assignment_expression(ctx, error, &ectx);
                if (error->code != 0) throw;

                p_array_declarator->constant_size = p_array_declarator->assignment_expression->constant_value;
            }

        }

        parser_match_tk(ctx, ']', error);
        if (error->code != 0) throw;
    }
    catch
    {
        if (p_array_declarator)
        {
        }
    }



    return p_array_declarator;
}


struct function_declarator* function_declarator(struct parser_ctx* ctx, struct error* error)
{
    struct function_declarator* p_function_declarator = calloc(1, sizeof(struct function_declarator));
    //faz um push da funcion_scope_declarator_list que esta vivendo mais em cima
    //eh feito o pop mais em cima tb.. aqui dentro do decide usar.
    //ctx->funcion_scope_declarator_list->outer_scope = ctx->current_scope;
    //ctx->current_scope = ctx->funcion_scope_declarator_list;
    //direct_declarator '(' parameter_type_list_opt ')'



    p_function_declarator->parameters_scope.scope_level = ctx->scopes.tail->scope_level + 1;

    scope_list_push(&ctx->scopes, &p_function_declarator->parameters_scope);

    //print_scope(&ctx->scopes);

    parser_match_tk(ctx, '(', error);
    if (error->code == 0 &&
        ctx->current->type != ')')
    {
        p_function_declarator->parameter_type_list_opt = parameter_type_list(ctx, error);
    }
    parser_match_tk(ctx, ')', error);

    //print_scope(&ctx->scopes);

    scope_list_pop(&ctx->scopes);

    //print_scope(&ctx->scopes);


    return p_function_declarator;
}


struct pointer* pointer_opt(struct parser_ctx* ctx, struct error* error)
{
    struct pointer* p = NULL;
    while (error->code == 0 && ctx->current != NULL &&
        ctx->current->type == '*')
    {
        struct pointer* p_pointer = calloc(1, sizeof(struct pointer));
        p = p_pointer;
        parser_match(ctx);
        attribute_specifier_sequence_opt(ctx, error);
        if (first_of_type_qualifier(ctx))
            p_pointer->type_qualifier_list_opt = type_qualifier_list(ctx, error);
        while (error->code == 0 && ctx->current != NULL &&
            ctx->current->type == '*')
        {
            p_pointer->pointer = pointer_opt(ctx, error);
        }
    }
    //'*' attribute_specifier_sequence_opt type_qualifier_list_opt
    //'*' attribute_specifier_sequence_opt type_qualifier_list_opt pointer
    return p;
}


struct type_qualifier_list* type_qualifier_list(struct parser_ctx* ctx, struct error* error)
{
    struct type_qualifier_list* p_type_qualifier_list = calloc(1, sizeof(struct type_qualifier_list));
    //type_qualifier
    //type_qualifier_list type_qualifier


    struct type_qualifier* p_type_qualifier = type_qualifier(ctx, error);
    p_type_qualifier_list->flags |= p_type_qualifier->flags;
    list_add(p_type_qualifier_list, p_type_qualifier);
    p_type_qualifier = NULL;

    while (error->code == 0 && ctx->current != NULL &&
        first_of_type_qualifier(ctx))
    {
        p_type_qualifier = type_qualifier(ctx, error);
        p_type_qualifier_list->flags |= p_type_qualifier->flags;
        list_add(p_type_qualifier_list, p_type_qualifier);
    }
    return p_type_qualifier_list;
}


struct parameter_type_list* parameter_type_list(struct parser_ctx* ctx, struct error* error)
{
    struct parameter_type_list* p_parameter_type_list = calloc(1, sizeof(struct parameter_type_list));
    //parameter_list
    //parameter_list ',' '...'
    p_parameter_type_list->parameter_list = parameter_list(ctx, error);
    /*ja esta saindo com a virgula consumida do parameter_list para evitar ahead*/
    if (ctx->current->type == '...')
    {
        parser_match(ctx);
        //parser_match_tk(ctx, '...', error);
        p_parameter_type_list->is_var_args = true;
    }
    return p_parameter_type_list;
}


struct parameter_list* parameter_list(struct parser_ctx* ctx, struct error* error)
{
    /*
      parameter_list
      parameter_declaration
      parameter_list ',' parameter_declaration
    */
    struct parameter_list* p_parameter_list = calloc(1, sizeof(struct parameter_list));
    list_add(p_parameter_list, parameter_declaration(ctx, error));
    while (error->code == 0 && ctx->current != NULL &&
        ctx->current->type == ',')
    {
        parser_match(ctx);
        if (ctx->current->type == '...')
        {
            //follow
            break;
        }
        list_add(p_parameter_list, parameter_declaration(ctx, error));
        if (error->code) break;
    }
    return p_parameter_list;
}


struct parameter_declaration* parameter_declaration(struct parser_ctx* ctx, struct error* error)
{
    struct parameter_declaration* p_parameter_declaration = calloc(1, sizeof(struct parameter_declaration));
    //attribute_specifier_sequence_opt declaration_specifiers declarator
    //attribute_specifier_sequence_opt declaration_specifiers abstract_declarator_opt
    attribute_specifier_sequence_opt(ctx, error);
    p_parameter_declaration->declaration_specifiers = declaration_specifiers(ctx, error);

    //talvez no ctx colocar um flag que esta em argumentos
    //TODO se tiver uma struct tag novo...
    //warning: declaration of 'struct X' will not be visible outside of this function [-Wvisibility]

    p_parameter_declaration->declarator = declarator(ctx,
        /*specifier_qualifier_list*/NULL,
        p_parameter_declaration->declaration_specifiers,
        true/*can be abstract*/,
        &p_parameter_declaration->name,
        error);
    p_parameter_declaration->declarator->declaration_specifiers = p_parameter_declaration->declaration_specifiers;

    p_parameter_declaration->declarator->type =
        make_type_using_declarator(ctx, p_parameter_declaration->declarator);

    if (p_parameter_declaration->name)
        naming_convention_parameter(ctx, p_parameter_declaration->name, &p_parameter_declaration->declarator->type);

    //coloca o pametro no escpo atual que deve apontar para escopo paramtros
    // da funcao .
    // 
    //assert ctx->current_scope->variables parametrosd
    if (p_parameter_declaration->name)
    {
        //parametro void nao te name 
        hashmap_set(&ctx->scopes.tail->variables,
            p_parameter_declaration->name->lexeme,
            &p_parameter_declaration->declarator->type_id);
        //print_scope(ctx->current_scope);
    }
    return p_parameter_declaration;
}


struct specifier_qualifier_list* copy(struct declaration_specifiers* p_declaration_specifiers)
{
    struct specifier_qualifier_list* p_specifier_qualifier_list = calloc(1, sizeof(struct specifier_qualifier_list));

    p_specifier_qualifier_list->type_qualifier_flags = p_declaration_specifiers->type_qualifier_flags;
    p_specifier_qualifier_list->type_specifier_flags = p_declaration_specifiers->type_specifier_flags;

    struct declaration_specifier* p_declaration_specifier =
        p_declaration_specifiers->head;

    while (p_declaration_specifier)
    {
        if (p_declaration_specifier->type_specifier_qualifier)
        {
            struct specifier_qualifier* p_specifier_qualifier = calloc(1, sizeof(struct specifier_qualifier));

            if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
            {
                struct type_qualifier* p_type_qualifier = calloc(1, sizeof(struct type_qualifier));

                p_type_qualifier->flags = p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;


                p_type_qualifier->token = p_declaration_specifier->type_specifier_qualifier->type_qualifier->token;
                p_specifier_qualifier->type_qualifier = p_type_qualifier;
            }
            else if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
            {
                struct type_specifier* p_type_specifier = calloc(1, sizeof(struct type_specifier));

                p_type_specifier->flags = p_declaration_specifier->type_specifier_qualifier->type_specifier->flags;

                //todo
                assert(p_declaration_specifier->type_specifier_qualifier->type_specifier->struct_or_union_specifier == NULL);

                p_type_specifier->token = p_declaration_specifier->type_specifier_qualifier->type_specifier->token;
                p_specifier_qualifier->type_specifier = p_type_specifier;
            }

            list_add(p_specifier_qualifier_list, p_specifier_qualifier);
        }
        p_declaration_specifier = p_declaration_specifier->next;
    }
    return p_specifier_qualifier_list;

}


void print_declarator(struct osstream* ss, struct declarator* p_declarator, bool bAbstract);

void print_direct_declarator(struct osstream* ss, struct direct_declarator* p_direct_declarator, bool bAbstract)
{
    if (p_direct_declarator->declarator)
    {
        ss_fprintf(ss, "(");
        print_declarator(ss, p_direct_declarator->declarator, bAbstract);
        ss_fprintf(ss, ")");
    }

    if (p_direct_declarator->name && !bAbstract)
    {
        //Se bAbstract for true é pedido para nao imprimir o nome do indentificador
        ss_fprintf(ss, "%s", p_direct_declarator->name->lexeme);
    }

    struct array_function* p_array_function =
        p_direct_declarator->array_function_list.head;

    while (p_array_function)
    {
        if (p_array_function->function_declarator)
        {
            ss_fprintf(ss, "(");
            struct parameter_declaration* p_parameter_declaration =
                p_array_function->function_declarator->parameter_type_list_opt ?
                p_array_function->function_declarator->parameter_type_list_opt->parameter_list->head : NULL;

            while (p_parameter_declaration)
            {
                if (p_parameter_declaration != p_array_function->function_declarator->parameter_type_list_opt->parameter_list->head)
                    ss_fprintf(ss, ",");

                print_declaration_specifiers(ss, p_parameter_declaration->declaration_specifiers);
                ss_fprintf(ss, " ");
                print_declarator(ss, p_parameter_declaration->declarator, bAbstract);

                p_parameter_declaration = p_parameter_declaration->next;
            }
            //... TODO
            ss_fprintf(ss, ")");
        }
        else if (p_array_function->array_declarator)
        {
            ss_fprintf(ss, "[]");
        }
        p_array_function = p_array_function->next;
    }

}



void print_declarator(struct osstream* ss, struct declarator* p_declarator, bool bAbstract)
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
    print_direct_declarator(ss, p_declarator->direct_declarator, bAbstract);

}

void print_type_name(struct osstream* ss, struct type_name* p)
{
    bool first = true;
    print_specifier_qualifier_list(ss, &first, p->specifier_qualifier_list);
    print_declarator(ss, p->declarator, true);
}

struct type_name* type_name(struct parser_ctx* ctx, struct error* error)
{
    struct type_name* p_type_name = calloc(1, sizeof(struct type_name));

    p_type_name->first = ctx->current;


    p_type_name->specifier_qualifier_list = specifier_qualifier_list(ctx, error);
    p_type_name->declarator = declarator(ctx,
        p_type_name->specifier_qualifier_list,//??
        /*declaration_specifiers*/ NULL,
        true /*DEVE SER TODO*/,
        NULL,
        error);

    p_type_name->last = ctx->current->prev;

    p_type_name->declarator->specifier_qualifier_list = p_type_name->specifier_qualifier_list;

    p_type_name->declarator->type = make_type_using_declarator(ctx, p_type_name->declarator);
    return p_type_name;
}

struct braced_initializer* braced_initializer(struct parser_ctx* ctx, struct error* error)
{
    /*
     { }
     { initializer-list }
     { initializer-list , }
    */

    struct braced_initializer* p_bracket_initializer_list = calloc(1, sizeof(struct braced_initializer));
    p_bracket_initializer_list->first = ctx->current;
    parser_match_tk(ctx, '{', error);
    if (ctx->current->type != '}')
    {
        p_bracket_initializer_list->initializer_list = initializer_list(ctx, error);
    }
    parser_match_tk(ctx, '}', error);
    return p_bracket_initializer_list;
}



struct initializer* initializer(struct parser_ctx* ctx, struct error* error)
{
    /*
    initializer:
      assignment-expression
      braced-initializer
    */

    struct initializer* p_initializer = calloc(1, sizeof(struct initializer));

    p_initializer->first_token = ctx->current;

    if (ctx->current->type == '{')
    {
        p_initializer->braced_initializer = braced_initializer(ctx, error);
    }
    else
    {
        struct expression_ctx ectx = { 0 };
        p_initializer->assignment_expression = assignment_expression(ctx, error, &ectx);
    }
    return p_initializer;
}


struct initializer_list* initializer_list(struct parser_ctx* ctx, struct error* error)
{
    /*
    initializer-list:
       designation opt initializer
       initializer-list , designation opt initializer
    */


    struct initializer_list* p_initializer_list = calloc(1, sizeof(struct initializer_list));

    p_initializer_list->first_token = ctx->current;

    struct designation* p_designation = NULL;
    if (first_of_designator(ctx))
    {
        p_designation = designation(ctx, error);
    }
    struct initializer* p_initializer = initializer(ctx, error);
    p_initializer->designation = p_designation;
    list_add(p_initializer_list, p_initializer);
    while (error->code == 0 && ctx->current != NULL &&
        ctx->current->type == ',')
    {
        parser_match(ctx);
        if (ctx->current->type == '}')
            break; //follow

        struct designation* p_designation2 = NULL;
        if (first_of_designator(ctx))
        {
            p_designation2 = designation(ctx, error);
        }
        struct initializer* p_initializer2 = initializer(ctx, error);
        p_initializer2->designation = p_designation;
        list_add(p_initializer_list, p_initializer2);
    }
    return p_initializer_list;
}


struct designation* designation(struct parser_ctx* ctx, struct error* error)
{
    //designator_list '='
    struct designation* p_designation = calloc(1, sizeof(struct designation));
    designator_list(ctx, error);
    parser_match_tk(ctx, '=', error);
    return p_designation;
}

struct designator_list* designator_list(struct parser_ctx* ctx, struct error* error)
{
    //designator
    //designator_list designator
    struct designator_list* p_designator_list = calloc(1, sizeof(struct designator_list));
    list_add(p_designator_list, designator(ctx, error));
    while (error->code == 0 && ctx->current != NULL &&
        first_of_designator(ctx))
    {
        list_add(p_designator_list, designator(ctx, error));
    }
    return p_designator_list;
}


struct designator* designator(struct parser_ctx* ctx, struct error* error)
{
    //'[' constant_expression ']'
    //'.' identifier
    struct designator* p_designator = calloc(1, sizeof(struct designator));
    if (ctx->current->type == '[')
    {
        parser_match_tk(ctx, '[', error);
        struct expression_ctx ectx = { 0 };
        constant_expression(ctx, error, &ectx);
        parser_match_tk(ctx, ']', error);
    }
    else if (ctx->current->type == '.')
    {
        parser_match(ctx);
        parser_match_tk(ctx, TK_IDENTIFIER, error);
    }
    return p_designator;
}





struct static_assert_declaration* static_assert_declaration(struct parser_ctx* ctx, struct error* error)
{

    /*
     static_assert-declaration:
      "static_assert" ( constant-expression , string-literal ) ;
      "static_assert" ( constant-expression ) ;
    */

    struct static_assert_declaration* p_static_assert_declaration = calloc(1, sizeof(struct static_assert_declaration));
    try
    {
        p_static_assert_declaration->first_token = ctx->current;
        struct token* position = ctx->current;
        parser_match_tk(ctx, TK_KEYWORD__STATIC_ASSERT, error);
        parser_match_tk(ctx, '(', error);
        struct expression_ctx ectx = { .bConstantExpressionRequired = true };
        p_static_assert_declaration->constant_expression = constant_expression(ctx, error, &ectx);

        if (error->code != 0)
            throw;

        if (ctx->current->type == ',')
        {
            parser_match(ctx);
            p_static_assert_declaration->string_literal_opt = ctx->current;
            parser_match_tk(ctx, TK_STRING_LITERAL, error);
        }

        parser_match_tk(ctx, ')', error);
        p_static_assert_declaration->last_token = ctx->current;
        parser_match_tk(ctx, ';', error);

        if (p_static_assert_declaration->constant_expression->constant_value == 0)
        {
            if (p_static_assert_declaration->string_literal_opt)
            {
                parser_seterror_with_token(ctx, position, "_Static_assert failed %s\n",
                    p_static_assert_declaration->string_literal_opt->lexeme);
            }
            else
            {
                parser_seterror_with_token(ctx, position, "_Static_assert failed");
            }
            error->code = 1;
        }
    }
    catch
    {}
    return p_static_assert_declaration;
}


struct attribute_specifier_sequence* attribute_specifier_sequence_opt(struct parser_ctx* ctx, struct error* error)
{
    //attribute_specifier_sequence_opt attribute_specifier
    struct attribute_specifier_sequence* p_attribute_specifier_sequence =
        calloc(1, sizeof(struct attribute_specifier_sequence));

    while (error->code == 0 &&
        ctx->current != NULL &&
        first_of_attribute_specifier(ctx))
    {
        list_add(p_attribute_specifier_sequence, attribute_specifier(ctx, error));
    }
    return p_attribute_specifier_sequence;
}

struct attribute_specifier_sequence* attribute_specifier_sequence(struct parser_ctx* ctx, struct error* error)
{
    //attribute_specifier_sequence_opt attribute_specifier
    struct attribute_specifier_sequence* p_attribute_specifier_sequence = calloc(1, sizeof(struct attribute_specifier_sequence));
    while (error->code == 0 && ctx->current != NULL &&
        first_of_attribute_specifier(ctx))
    {
        list_add(p_attribute_specifier_sequence, attribute_specifier(ctx, error));
    }
    return p_attribute_specifier_sequence;
}


struct attribute_specifier* attribute_specifier(struct parser_ctx* ctx, struct error* error)
{
    struct attribute_specifier* p_attribute_specifier = calloc(1, sizeof(struct attribute_specifier));

    p_attribute_specifier->first = ctx->current;



    //'[' '[' attribute_list ']' ']'
    parser_match_tk(ctx, '[', error);
    parser_match_tk(ctx, '[', error);
    p_attribute_specifier->attribute_list = attribute_list(ctx, error);
    parser_match_tk(ctx, ']', error);
    p_attribute_specifier->last = ctx->current;
    parser_match_tk(ctx, ']', error);
    return p_attribute_specifier;
}



struct attribute_list* attribute_list(struct parser_ctx* ctx, struct error* error)
{
    struct attribute_list* p_attribute_list = calloc(1, sizeof(struct attribute_list));
    //attribute_opt
    //attribute_list ',' attribute_opt
    while (error->code == 0 && ctx->current != NULL && (
        first_of_attribute(ctx) ||
        ctx->current->type == ','))
    {
        if (first_of_attribute(ctx))
        {
            list_add(p_attribute_list, attribute(ctx, error));
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

struct attribute* attribute(struct parser_ctx* ctx, struct error* error)
{
    struct attribute* p_attribute = calloc(1, sizeof(struct attribute));
    //attribute_token attribute_argument_clause_opt
    p_attribute->attribute_token = attribute_token(ctx, error);
    if (ctx->current->type == '(') //first
    {
        p_attribute->attribute_argument_clause = attribute_argument_clause(ctx, error);
    }
    return p_attribute;
}


struct attribute_token* attribute_token(struct parser_ctx* ctx, struct error* error)
{
    struct attribute_token* p_attribute_token = calloc(1, sizeof(struct attribute_token));
    //standard_attribute
    //attribute_prefixed_token
    bool bStandardAtt = strcmp(ctx->current->lexeme, "deprecated") == 0 ||
        strcmp(ctx->current->lexeme, "fallthrough") == 0 ||
        strcmp(ctx->current->lexeme, "maybe_unused") == 0 ||
        strcmp(ctx->current->lexeme, "nodiscard") == 0;
    parser_match_tk(ctx, TK_IDENTIFIER, error);
    if (ctx->current->type == '::')
    {
        parser_match(ctx);
        parser_match_tk(ctx, TK_IDENTIFIER, error);
    }
    else
    {
        if (!bStandardAtt)
        {
            printf("warning not std att\n");
        }
    }
    return p_attribute_token;
}



struct attribute_argument_clause* attribute_argument_clause(struct parser_ctx* ctx, struct error* error)
{
    struct attribute_argument_clause* p_attribute_argument_clause = calloc(1, sizeof(struct attribute_argument_clause));
    //'(' balanced_token_sequence_opt ')'
    parser_match_tk(ctx, '(', error);
    balanced_token_sequence_opt(ctx, error);
    parser_match_tk(ctx, ')', error);
    return p_attribute_argument_clause;
}


struct balanced_token_sequence* balanced_token_sequence_opt(struct parser_ctx* ctx, struct error* error)
{
    struct balanced_token_sequence* p_balanced_token_sequence = calloc(1, sizeof(struct balanced_token_sequence));
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
        parser_seterror_with_token(ctx, ctx->current, "expected ']' before ')'");
        error->code = 1;
    }
    if (count3 != 0)
    {
        parser_seterror_with_token(ctx, ctx->current, "expected '}' before ')'");
        error->code = 1;
    }
    return p_balanced_token_sequence;
}


struct statement* statement(struct parser_ctx* ctx, struct error* error)
{
    struct statement* p_statement = calloc(1, sizeof(struct statement));
    if (first_of_labeled_statement(ctx))
    {
        p_statement->labeled_statement = labeled_statement(ctx, error);
    }
    else
    {
        p_statement->unlabeled_statement = unlabeled_statement(ctx, error);
    }
    //labeled_statement
    //unlabeled_statement
    return p_statement;
}

struct primary_block* primary_block(struct parser_ctx* ctx, struct error* error)
{
    assert(ctx->current != NULL);
    struct primary_block* p_primary_block = calloc(1, sizeof(struct primary_block));
    if (first_of_compound_statement(ctx))
    {
        p_primary_block->compound_statement = compound_statement(ctx, error);
    }
    else if (first_of_selection_statement(ctx))
    {
        p_primary_block->selection_statement = selection_statement(ctx, error);
    }
    else if (first_of_iteration_statement(ctx))
    {
        p_primary_block->iteration_statement = iteration_statement(ctx, error);
    }
    else if (ctx->current->type == TK_KEYWORD_DEFER)
    {
        p_primary_block->defer_statement = defer_statement(ctx, error);
    }
    else if (ctx->current->type == TK_KEYWORD_TRY)
    {
        p_primary_block->try_statement = try_statement(ctx, error);
    }    
    else
    {
        seterror(error, "unexpected");
    }
    p_primary_block->last = previous_parser_token(ctx->current);
    return p_primary_block;
}

struct secondary_block* secondary_block(struct parser_ctx* ctx, struct error* error)
{
    struct secondary_block* p_secondary_block = calloc(1, sizeof(struct secondary_block));
    p_secondary_block->first = ctx->current;


    p_secondary_block->statement = statement(ctx, error);

    p_secondary_block->last = previous_parser_token(ctx->current);

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

struct unlabeled_statement* unlabeled_statement(struct parser_ctx* ctx, struct error* error)
{
    struct unlabeled_statement* p_unlabeled_statement = calloc(1, sizeof(struct unlabeled_statement));

    if (first_of_primary_block(ctx))
    {
        p_unlabeled_statement->primary_block = primary_block(ctx, error);
    }
    else if (first_of_jump_statement(ctx))
    {
        p_unlabeled_statement->jump_statement = jump_statement(ctx, error);
    }
    else
    {
        p_unlabeled_statement->expression_statement = expression_statement(ctx, error);
    }
    //expression_statement
    //attribute_specifier_sequence_opt compound_statement
    //attribute_specifier_sequence_opt selection_statement
    //attribute_specifier_sequence_opt iteration_statement
    //attribute_specifier_sequence_opt jump_statement
    return p_unlabeled_statement;
}
struct label* label(struct parser_ctx* ctx, struct error* error)
{
    struct label* p_label = calloc(1, sizeof(struct label));
    if (ctx->current->type == TK_IDENTIFIER)
    {
        p_label->name = ctx->current;
        parser_match(ctx);
        parser_match_tk(ctx, ':', error);
    }
    else if (ctx->current->type == TK_KEYWORD_CASE)
    {
        parser_match(ctx);
        struct expression_ctx ectx = { .bConstantExpressionRequired = true };
        constant_expression(ctx, error, &ectx);
        parser_match_tk(ctx, ':', error);
    }
    else if (ctx->current->type == TK_KEYWORD_DEFAULT)
    {
        parser_match(ctx);
        parser_match_tk(ctx, ':', error);
    }
    //attribute_specifier_sequence_opt identifier ':'
    //attribute_specifier_sequence_opt 'case' constant_expression ':'
    //attribute_specifier_sequence_opt 'default' ':'
    return p_label;
}

struct labeled_statement* labeled_statement(struct parser_ctx* ctx, struct error* error)
{
    struct labeled_statement* p_labeled_statement = calloc(1, sizeof(struct labeled_statement));
    //label statement
    p_labeled_statement->label = label(ctx, error);
    p_labeled_statement->statement = statement(ctx, error);
    return p_labeled_statement;
}

struct compound_statement* compound_statement(struct parser_ctx* ctx, struct error* error)
{
    //'{' block_item_list_opt '}'
    struct compound_statement* p_compound_statement = calloc(1, sizeof(struct compound_statement));
    struct scope block_scope = { .variables.capacity = 10 };
    scope_list_push(&ctx->scopes, &block_scope);

    p_compound_statement->first_token = ctx->current;
    parser_match_tk(ctx, '{', error);

    if (ctx->current->type != '}')
    {
        p_compound_statement->block_item_list = block_item_list(ctx, error);
    }

    p_compound_statement->last_token = ctx->current;
    parser_match_tk(ctx, '}', error);

    //TODO ver quem nao foi usado.

    for (int i = 0; i < block_scope.variables.capacity; i++)
    {
        if (block_scope.variables.table == NULL)
            continue;
        struct map_entry* entry = block_scope.variables.table[i];
        while (entry)
        {

            struct declarator* p_declarator =
                p_declarator = container_of(entry->p, struct declarator, type_id);

            if (p_declarator)
            {
                if (p_declarator->num_uses == 0)
                {
                    //setwarning_with_token(ctx, p_declarator->name, )
                    ctx->n_warnings++;
                    if (p_declarator->name && p_declarator->name->pFile)
                    {
                        ctx->printf(WHITE "%s:%d:%d: ",
                            p_declarator->name->pFile->lexeme,
                            p_declarator->name->line,
                            p_declarator->name->col);

                        ctx->printf(LIGHTMAGENTA "warning: " WHITE "'%s': unreferenced declarator\n",
                            p_declarator->name->lexeme);
                    }
                }
            }

            entry = entry->next;
        }
    }

    scope_list_pop(&ctx->scopes);

    return p_compound_statement;
}

struct block_item_list block_item_list(struct parser_ctx* ctx, struct error* error)
{
    /*
      block_item_list:
      block_item
      block_item_list block_item
    */
    struct block_item_list block_item_list = { 0 };
    list_add(&block_item_list, block_item(ctx, error));
    while (error->code == 0 && ctx->current != NULL &&
        ctx->current->type != '}') //follow
    {
        list_add(&block_item_list, block_item(ctx, error));
    }
    return block_item_list;
}

struct block_item* block_item(struct parser_ctx* ctx, struct error* error)
{
    //   declaration
    //     unlabeled_statement
    //   label
    struct block_item* p_block_item = calloc(1, sizeof(struct block_item));
    attribute_specifier_sequence_opt(ctx, error);

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
            while (error->code == 0 &&
                ctx->current->type != '}')
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
        p_block_item->declaration = declaration(ctx, error);
        struct init_declarator* p = p_block_item->declaration->init_declarator_list.head;
        while (p)
        {
            if (p->declarator && p->declarator->name)
            {
                naming_convention_local_var(ctx, p->declarator->name, &p->declarator->type);
            }
            p = p->next;
        }
    }
    else if (first_of_label(ctx))
    {
        //so identifier confunde com expression
        p_block_item->label = label(ctx, error);
    }
    else
    {
        p_block_item->unlabeled_statement = unlabeled_statement(ctx, error);
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


struct try_statement* try_statement(struct parser_ctx* ctx, struct error* error)
{
    struct try_statement* p_try_statement = calloc(1, sizeof(struct try_statement));

    p_try_statement->first_token = ctx->current;

    assert(ctx->current->type == TK_KEYWORD_TRY);
    const struct try_statement* try_statement_copy_opt = ctx->p_current_try_statement_opt;
    ctx->p_current_try_statement_opt = p_try_statement;
    ctx->try_catch_block_index++;
    p_try_statement->try_catch_block_index = ctx->try_catch_block_index;
    parser_match_tk(ctx, TK_KEYWORD_TRY, error);

    p_try_statement->secondary_block = secondary_block(ctx, error);
    /*retores the previous one*/
    ctx->p_current_try_statement_opt = try_statement_copy_opt;
    

    if (ctx->current->type == TK_KEYWORD_CATCH)
    {
        p_try_statement->catch_token_opt = ctx->current;
        parser_match(ctx);

        p_try_statement->catch_secondary_block_opt = secondary_block(ctx, error);
    }
    p_try_statement->last_token = previous_parser_token(ctx->current);

    

    return p_try_statement;
}

struct selection_statement* selection_statement(struct parser_ctx* ctx, struct error* error)
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
    struct selection_statement* p_selection_statement = calloc(1, sizeof(struct selection_statement));

    p_selection_statement->first_token = ctx->current;

    struct scope if_scope = { 0 };
    scope_list_push(&ctx->scopes, &if_scope); //variaveis decladas no if

    if (ctx->current->type == TK_KEYWORD_IF)
    {
        parser_match(ctx);
        parser_match_tk(ctx, '(', error);
        if (first_of_declaration_specifier(ctx))
        {
            struct declaration_specifiers* p_declaration_specifiers = declaration_specifiers(ctx, error);
            struct init_declarator_list list = init_declarator_list(ctx, p_declaration_specifiers, error);
            p_selection_statement->init_declarator = list.head; //only one
            parser_match_tk(ctx, ';', error);
        }

        struct expression_ctx ectx = { 0 };
        p_selection_statement->expression = expression(ctx, error, &ectx);

        parser_match_tk(ctx, ')', error);
        p_selection_statement->secondary_block = secondary_block(ctx, error);
        
        if (ctx->current)
        {
            if (ctx->current->type == TK_KEYWORD_ELSE)
            {
                p_selection_statement->else_token_opt = ctx->current;
                parser_match(ctx);
                p_selection_statement->else_secondary_block_opt = secondary_block(ctx, error);
            }
        }
        else
        {
            parser_seterror_with_token(ctx, ctx->input_list.tail, "unexpected end of file");
        }
    }
    else if (ctx->current->type == TK_KEYWORD_SWITCH)
    {
        parser_match(ctx);
        parser_match_tk(ctx, '(', error);
        struct expression_ctx ectx = { 0 };
        p_selection_statement->expression = expression(ctx, error, &ectx);
        parser_match_tk(ctx, ')', error);
        p_selection_statement->secondary_block = secondary_block(ctx, error);
        
    }
    else
    {
        assert(false);
        parser_seterror_with_token(ctx, ctx->input_list.tail, "unexpected token");
    }

    p_selection_statement->last_token = previous_parser_token(ctx->current);

    scope_list_pop(&ctx->scopes);

    return p_selection_statement;
}

struct defer_statement* defer_statement(struct parser_ctx* ctx, struct error* error)
{
    struct defer_statement* p_defer_statement = calloc(1, sizeof(struct defer_statement));
    if (ctx->current->type == TK_KEYWORD_DEFER)
    {
        p_defer_statement->firsttoken = ctx->current;
        parser_match(ctx);
        p_defer_statement->secondary_block = secondary_block(ctx, error);
        p_defer_statement->lasttoken = previous_parser_token(ctx->current);
    }
    return p_defer_statement;
}

struct iteration_statement* iteration_statement(struct parser_ctx* ctx, struct error* error)
{
    /*
    iteration-statement:
      while ( expression ) statement
      do statement while ( expression ) ;
      for ( expressionopt ; expressionopt ; expressionopt ) statement
      for ( declaration expressionopt ; expressionopt ) statement
    */
    struct iteration_statement* p_iteration_statement = calloc(1, sizeof(struct iteration_statement));
    p_iteration_statement->token = ctx->current;
    if (ctx->current->type == TK_KEYWORD_DO)
    {
        parser_match(ctx);
        p_iteration_statement->secondary_block = secondary_block(ctx, error);
        parser_match_tk(ctx, TK_KEYWORD_WHILE, error);
        parser_match_tk(ctx, '(', error);
        struct expression_ctx ectx = { 0 };
        p_iteration_statement->expression1 = expression(ctx, error, &ectx);
        parser_match_tk(ctx, ')', error);
        parser_match_tk(ctx, ';', error);
    }
    else if (ctx->current->type == TK_KEYWORD_REPEAT)
    {
        parser_match(ctx);
        p_iteration_statement->secondary_block = secondary_block(ctx, error);
    }
    else if (ctx->current->type == TK_KEYWORD_WHILE)
    {
        parser_match(ctx);
        parser_match_tk(ctx, '(', error);
        struct expression_ctx ectx = { 0 };
        p_iteration_statement->expression1 = expression(ctx, error, &ectx);
        parser_match_tk(ctx, ')', error);
        p_iteration_statement->secondary_block = secondary_block(ctx, error);
    }
    else if (ctx->current->type == TK_KEYWORD_FOR)
    {
        struct expression_ctx ectx = { 0 };
        parser_match(ctx);
        parser_match_tk(ctx, '(', error);
        if (first_of_declaration_specifier(ctx))
        {
            struct scope for_scope = { 0 };
            scope_list_push(&ctx->scopes, &for_scope);

            declaration(ctx, error);
            if (ctx->current->type != ';')
            {
                p_iteration_statement->expression1 = expression(ctx, error, &ectx);
            }
            parser_match_tk(ctx, ';', error);
            if (ctx->current->type != ')')
                p_iteration_statement->expression2 = expression(ctx, error, &ectx);

            parser_match_tk(ctx, ')', error);

            p_iteration_statement->secondary_block = secondary_block(ctx, error);

            scope_list_pop(&ctx->scopes);
        }
        else
        {
            if (ctx->current->type != ';')
                expression(ctx, error, &ectx);
            parser_match_tk(ctx, ';', error);
            if (ctx->current->type != ';')
                expression(ctx, error, &ectx);
            parser_match_tk(ctx, ';', error);
            if (ctx->current->type != ')')
                p_iteration_statement->expression1 = expression(ctx, error, &ectx);
            parser_match_tk(ctx, ')', error);

            p_iteration_statement->secondary_block = secondary_block(ctx, error);
        }
    }
    return p_iteration_statement;
}
struct jump_statement* jump_statement(struct parser_ctx* ctx, struct error* error)
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

    struct jump_statement* p_jump_statement = calloc(1, sizeof(struct jump_statement));

    p_jump_statement->token = ctx->current;

    if (ctx->current->type == TK_KEYWORD_GOTO)
    {
        parser_match(ctx);
        p_jump_statement->label = ctx->current;
        parser_match_tk(ctx, TK_IDENTIFIER, error);
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
            error->code = 1;
            parser_seterror_with_token(ctx, ctx->current, "throw statement not within try block");
        }
        else
        {
            p_jump_statement->try_catch_block_index = ctx->p_current_try_statement_opt->try_catch_block_index;
        }

        parser_match(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_RETURN)
    {
        parser_match(ctx);
        if (ctx->current->type != ';')
        {
            struct expression_ctx ectx = { 0 };
            p_jump_statement->expression = expression(ctx, error, &ectx);

            /*
            * Check is return type is compatible with function return
            */
            if (!type_is_compatible(&ctx->p_current_function_opt->init_declarator_list.head->declarator->type,
                &p_jump_statement->expression->type))
            {
                parser_seterror_with_token(ctx, p_jump_statement->expression->first, "return type is incompatible");
            }
        }
    }
    else
    {
        assert(false);
    }
    p_jump_statement->lasttoken = ctx->current;
    parser_match_tk(ctx, ';', error);
    return p_jump_statement;
}

struct expression_statement* expression_statement(struct parser_ctx* ctx, struct error* error)
{
    struct expression_statement* p_expression_statement = calloc(1, sizeof(struct expression_statement));
    /*
     expression-statement:
       expression_opt ;
       attribute-specifier-sequence expression ;
    */
    if (ctx->current->type != ';')
    {
        p_expression_statement->first_token = ctx->current;
        struct expression_ctx ectx = { 0 };
        p_expression_statement->expression = expression(ctx, error, &ectx);
    }
    parser_match_tk(ctx, ';', error);
    return p_expression_statement;
}

void declaration_list_destroy(struct declaration_list* list)
{

}

struct declaration_list translation_unit(struct parser_ctx* ctx, struct error* error)
{
    struct declaration_list declaration_list = { 0 };
    /*
      translation_unit:
      external_declaration
      translation_unit external_declaration
    */
    while (error->code == 0 &&
        ctx->current != NULL)
    {
        list_add(&declaration_list, external_declaration(ctx, error));
    }
    return declaration_list;
}


struct declaration* external_declaration(struct parser_ctx* ctx, struct error* error)
{
    /*
     function_definition
     declaration
     */
    return function_definition_or_declaration(ctx, error);
}

struct compound_statement* function_body(struct parser_ctx* ctx, struct error* error)
{
    /*
    * Used to give an unique index (inside the function)
    * for try-catch blocks
    */
    ctx->try_catch_block_index = 0;
    ctx->p_current_try_statement_opt = NULL;
    return compound_statement(ctx, error);
}


struct declaration_list parse(struct options* options, struct token_list* list, struct error* error)
{

    anonymous_struct_count = 0;

    struct scope file_scope = { 0 };
    struct parser_ctx ctx = { .input_language = options->input , .check_naming_conventions = options->check_naming_conventions };
#ifdef TEST
    ctx.printf = printf_nothing;
#else
    ctx.printf = printf;
#endif


    scope_list_push(&ctx.scopes, &file_scope);
    ctx.input_list = *list;
    ctx.current = ctx.input_list.head;
    parser_skip_blanks(&ctx);

    struct declaration_list l = translation_unit(&ctx, error);
    if (ctx.n_errors > 0)
        error->code = 1;

    return l;
}



int fill_options(struct options* options, int argc, char** argv, struct preprocessor_ctx* prectx, struct error* error)
{
    /*first loop used to collect options*/
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            continue;

        if (strcmp(argv[i], "-E") == 0)
        {
            options->bPreprocessOnly = true;
            continue;
        }
        if (strcmp(argv[i], "-r") == 0)
        {
            options->bRemoveComments = true;
            continue;
        }
        if (strcmp(argv[i], "-rm") == 0)
        {
            options->bRemoveMacros = true;
            continue;
        }
        if (strcmp(argv[i], "-n") == 0)
        {
            options->check_naming_conventions = true;
            continue;
        }
        if (strcmp(argv[i], "-fi") == 0)
        {
            options->format_input = true;
            continue;
        }
        if (strcmp(argv[i], "-fo") == 0)
        {
            options->format_ouput = true;
            continue;
        }

        //
        if (strcmp(argv[i], "-target=c89") == 0)
        {
            options->target = LANGUAGE_C89;
            continue;
        }

        if (strcmp(argv[i], "-target=c99") == 0)
        {
            options->target = LANGUAGE_C99;
            continue;
        }
        if (strcmp(argv[i], "-target=c11") == 0)
        {
            options->target = LANGUAGE_C11;
            continue;
        }
        if (strcmp(argv[i], "-target=c2x") == 0)
        {
            options->target = LANGUAGE_C2X;
            continue;
        }
        if (strcmp(argv[i], "-target=cxx") == 0)
        {
            options->target = LANGUAGE_CXX;
            continue;
        }
        //
        if (strcmp(argv[i], "-std=c99") == 0)
        {
            options->input = LANGUAGE_C99;
            continue;
        }
        if (strcmp(argv[i], "-std=c11") == 0)
        {
            options->input = LANGUAGE_C11;
            continue;
        }
        if (strcmp(argv[i], "-std=c2x") == 0)
        {
            options->input = LANGUAGE_C2X;
            continue;
        }
        if (strcmp(argv[i], "-std=cxx") == 0)
        {
            options->input = LANGUAGE_CXX;
            continue;
        }
        //
        if (argv[i][1] == 'I')
        {
            include_dir_add(&prectx->include_dir, argv[i] + 2);
            continue;
        }
        if (argv[i][1] == 'D')
        {
            char buffer[200];
            snprintf(buffer, sizeof buffer, "#define %s \n", argv[i] + 2);

            struct token_list l1 = tokenizer(buffer, "", 0, TK_FLAG_NONE, error);
            preprocessor(prectx, &l1, 0, error);
            token_list_clear(&l1);
            continue;
        }
    }
    return error->code;
}

#ifdef _WIN32
unsigned long __stdcall GetEnvironmentVariableA(
    const char* lpName,
    char* lpBuffer,
    unsigned long nSize
);
#endif

void append_msvc_include_dir(struct preprocessor_ctx* prectx)
{
#ifdef _WIN32
    /*
     Para ver as variaveis de ambiente pode-se digitar set no windows
    */

    //char env[2000];
    //int n = GetEnvironmentVariableA("INCLUDE", env, sizeof(env));

    char env[] = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.31.31103/ATLMFC/include;"
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.31.31103/include;"
        "C:/Program Files (x86)/Windows Kits/NETFXSDK/4.8/include/um;"
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.19041.0/ucrt;"
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.19041.0/shared;"
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.19041.0/um;"
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.19041.0/winrt;"
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.19041.0/cppwinrt";
    int n = strlen(env);

    if (n > 0 && n < sizeof(env))
    {
        const char* p = env;

        for (;;)
        {
            if (*p == '\0')
            {
                break;
            }
            char fileNameLocal[500] = { 0 };
            int count = 0;
            while (*p != '\0' && *p != ';')
            {
                fileNameLocal[count] = *p;
                p++;
                count++;
            }
            fileNameLocal[count] = 0;
            if (count > 0)
            {
                //printf("%s\n", fileNameLocal);
                strcat(fileNameLocal, "/");

                include_dir_add(&prectx->include_dir, fileNameLocal);
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

const char* format_code(struct options* options,
    const char* content,
    struct error* error)
{
    struct ast ast = { 0 };
    char* s = NULL;


    struct preprocessor_ctx prectx = { 0 };

#ifdef TEST
    prectx.printf = printf_nothing;
#else
    prectx.printf = printf;
#endif


    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx, error);


    try
    {
        prectx.options = *options;
        append_msvc_include_dir(&prectx);


        struct token_list tokens = tokenizer(content, "", 0, TK_FLAG_NONE, error);
        if (error->code != 0)
            throw;

        ast.token_list = preprocessor(&prectx, &tokens, 0, error);
        if (error->code != 0)
            throw;

        ast.declaration_list = parse(&options, &ast.token_list, error);
        if (error->code != 0)
        {
            throw;
        }
        struct format_visit_ctx visit_ctx = { 0 };
        visit_ctx.ast = ast;
        format_visit(&visit_ctx, error);

        if (options->bRemoveMacros)
            s = get_code_as_compiler_see(&visit_ctx.ast.token_list);
        else
            s = get_code_as_we_see(&visit_ctx.ast.token_list, options->bRemoveComments);

    }
    catch
    {

    }


    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);
    return s;
}

int compile_one_file(const char* file_name,
    int argc,
    char** argv,
    struct error* error)
{
    clock_t start_clock = clock();

    struct preprocessor_ctx prectx = { 0 };

#ifdef TEST
    prectx.printf = printf_nothing;
#else
    prectx.printf = printf;
#endif


    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx, error);


    //int no_files = 0;
    struct ast ast = { 0 };

    struct options options = { .input = LANGUAGE_CXX };

    char* s = NULL;

    try
    {
        if (fill_options(&options, argc, argv, &prectx, error) != 0)
        {
            throw;
        }
        prectx.options = options;
        append_msvc_include_dir(&prectx);

        //printf("%-20s ", file_name);

        char* content = readfile(file_name);
        if (content == NULL)
        {
            seterror(error, "file not found '%s'\n", file_name);
            throw;
        }
        //printf(".");//1

        struct token_list tokens = tokenizer(content, file_name, 0, TK_FLAG_NONE, error);
        if (error->code != 0)
            throw;
        //printf(".");//2
        ast.token_list = preprocessor(&prectx, &tokens, 0, error);
        if (error->code != 0)
            throw;

        //_splitpath

        char path[200] = { 0 };
        snprintf(path, sizeof path, "./out/%s", file_name);

        mkdir("./out", 0777);


        if (options.bPreprocessOnly)
        {
            const char* s = print_preprocessed_to_string2(ast.token_list.head);
            printf("%s", s);
            free(s);
        }
        else
        {

            ast.declaration_list = parse(&options, &ast.token_list, error);
            if (error->code != 0)
            {
                throw;
            }

            if (options.format_input)
            {
                /*format input source before transformation*/
                struct format_visit_ctx visit_ctx = { 0 };
                visit_ctx.ast = ast;
                format_visit(&visit_ctx, error);
            }

            struct visit_ctx visit_ctx = { 0 };
            visit_ctx.target = options.target;
            visit_ctx.ast = ast;
            visit(&visit_ctx, error);

            if (options.bRemoveMacros)
                s = get_code_as_compiler_see(&visit_ctx.ast.token_list);
            else
                s = get_code_as_we_see(&visit_ctx.ast.token_list, options.bRemoveComments);

            if (options.format_ouput)
            {
                /*re-parser ouput and format*/
                const char* s2 = format_code(&options, s, error);
                free(s);
                s = s2;
            }

            FILE* out = fopen(path, "w");
            if (out)
            {
                fprintf(out, "%s", s);
                fclose(out);
                //printf("%-30s ", path);
            }
            else
            {
                seterror(error, "cannot open output file");
                throw;
            }



            clock_t end_clock_file = clock();
            double cpu_time_used_file = ((double)(end_clock_file - start_clock)) / CLOCKS_PER_SEC;

            //printf("OK %f sec\n", cpu_time_used_file);

        }
    }
    catch
    {
        //printf("Error %s\n", error->message);
    }


    free(s);
    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);

    return error->code;
}

int compile(int argc, char** argv, struct error* error)
{
    int hasErrors = 0;
    clock_t begin_clock = clock();
    int no_files = 0;

    /*second loop to compile each file*/
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
            continue;
        no_files++;
        clearerror(error);
        compile_one_file(argv[i], argc, argv, error);
        if (error->code != 0)
            hasErrors = true;
    }

    /*tempo total da compilacao*/
    clock_t end_clock = clock();
    double cpu_time_used = ((double)(end_clock - begin_clock)) / CLOCKS_PER_SEC;
    printf("Total %d files %f seconds\n", no_files, cpu_time_used);
    return hasErrors;
}


struct ast get_ast(struct options* options, const char* fileName, const char* source, struct error* error)
{
    struct ast ast = { 0 };

    struct token_list list = tokenizer(source, fileName, 0, TK_FLAG_NONE, error);
    if (error->code != 0)
        return ast;

    struct preprocessor_ctx prectx = { 0 };
    prectx.options = *options;

#ifdef TEST
    prectx.printf = printf_nothing;
#else
    prectx.printf = printf;
#endif

    prectx.macros.capacity = 5000;

    ast.token_list = preprocessor(&prectx, &list, 0, error);
    if (error->code != 0)
        return ast;

    ast.declaration_list = parse(&options, &ast.token_list, error);
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

char* compile_source(const char* pszoptions, const char* content)
{
    const char* argv[100] = { 0 };
    char string[200] = { 0 };
    snprintf(string, sizeof string, "exepath %s", pszoptions);

    const int argc = strtoargv(string, 10, argv);

    char* s = NULL;
    struct error error = { 0 };
    struct preprocessor_ctx prectx = { 0 };
#ifdef TEST
    prectx.printf = printf_nothing;
#else
    prectx.printf = printf;
#endif

    struct options options = { .input = LANGUAGE_CXX };
    //printf("options '%s'\n", pszoptions);
    try
    {
        if (fill_options(&options, argc, argv, &prectx, &error) != 0)
        {
            throw;
        }

        prectx.options = options;


        if (options.bPreprocessOnly)
        {

            struct token_list tokens = tokenizer(content, "source", 0, TK_FLAG_NONE, &error);
            if (error.code == 0)
            {
                struct token_list token_list = preprocessor(&prectx, &tokens, 0, &error);
                if (error.code == 0)
                {

                    s = print_preprocessed_to_string2(token_list.head);
                }
            }
            preprocessor_ctx_destroy(&prectx);


        }
        else
        {
            struct visit_ctx visit_ctx = { 0 };
            visit_ctx.target = options.target;
            struct ast ast = get_ast(&options, "source", content, &error);
            visit_ctx.ast = ast;
            visit(&visit_ctx, &error);

            if (options.bRemoveMacros)
            {
                s = get_code_as_compiler_see(&visit_ctx.ast.token_list);
            }
            else
            {
                s = get_code_as_we_see(&visit_ctx.ast.token_list, options.bRemoveComments);
            }
            if (options.format_ouput)
            {
                struct error error = { 0 };

                /*re-parser ouput and format*/
                const char* s2 = format_code(&options, s, &error);
                free(s);
                s = s2;
            }
        }

        if (error.code)
        {
            free(s);
            s = strdup(error.message);
        }
    }
    catch
    {
    }

    return s;
}


/*Função exportada para web*/
char* CompileText(const char* pszoptions, const char* content)
{
    return  compile_source(pszoptions, content);
}

void ast_destroy(struct ast* ast)
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
    if (!ctx->check_naming_conventions || token->level != 0)
        return;

    if (!is_snake_case(token->lexeme)) {
        parser_set_info_with_token(ctx, token, "use snake_case for struct/union tags");
    }
}

void naming_convention_enum_tag(struct parser_ctx* ctx, struct token* token)
{
    if (!ctx->check_naming_conventions || token->level != 0)
        return;

    if (!is_snake_case(token->lexeme)) {
        parser_set_info_with_token(ctx, token, "use snake_case for enum tags");
    }
}

void naming_convention_function(struct parser_ctx* ctx, struct token* token)
{
    if (!ctx->check_naming_conventions || token->level != 0)
        return;


    if (!is_snake_case(token->lexeme)) {
        parser_set_info_with_token(ctx, token, "use snake_case for functions");
    }
}
void naming_convention_global_var(struct parser_ctx* ctx, struct token* token, struct type* type, enum storage_class_specifier_flags storage)
{
    if (!ctx->check_naming_conventions || token->level != 0)
        return;

    if (!type_is_function_or_function_pointer(type))
    {
        if (storage & STORAGE_SPECIFIER_STATIC)
        {
            if (token->lexeme[0] != 's' || token->lexeme[1] != '_')
            {
                parser_set_info_with_token(ctx, token, "use prefix s_ for static global variables");
            }
        }
        if (!is_snake_case(token->lexeme)) {
            parser_set_info_with_token(ctx, token, "use snake_case global variables");
        }
    }
}

void naming_convention_local_var(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!ctx->check_naming_conventions || token->level != 0)
        return;

    if (!type_is_function_or_function_pointer(type))
    {
        if (!is_snake_case(token->lexeme)) {
            parser_set_info_with_token(ctx, token, "use snake_case for local variables");
        }
    }
}

void naming_convention_enumerator(struct parser_ctx* ctx, struct token* token)
{
    if (!ctx->check_naming_conventions || token->level != 0)
        return;

    if (!is_all_upper(token->lexeme)) {
        parser_set_info_with_token(ctx, token, "use UPPERCASE for enumerators");
    }
}

void naming_convention_struct_member(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!ctx->check_naming_conventions || token->level != 0)
        return;

    if (!is_snake_case(token->lexeme)) {
        parser_set_info_with_token(ctx, token, "use snake_case for struct members");
    }
}

void naming_convention_parameter(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!ctx->check_naming_conventions || token->level != 0)
        return;

    if (!is_snake_case(token->lexeme)) {
        parser_set_info_with_token(ctx, token, "use snake_case for arguments");
    }
}

#ifdef TEST

void parser_specifier_test()
{
    const char* source = "long long long i;";
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code != 0); //esperado erro    
}

void take_address_type_test()
{
    const char* source =
        "void F(char(*p)[10])"
        "{"
        "    (*p)[0] = 'a';"
        "}";
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);

    assert(error.code == 0);
}

void parser_scope_test()
{
    const char* source = "void f() {int i; char i;}";
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code != 0); //tem que dar erro
}

void parser_tag_test()
{
    //mudou tipo do tag no mesmo escopo
    const char* source = "enum E { A }; struct E { int i; };";
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code != 0); //tem que dar erro        
}

void string_concatenation_test()
{
    const char* source = " \"part1\" \"part2\"";
    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    struct ast ast = get_ast(&options, "source", source, &error);
    assert(error.code == 0);
}

void test_digit_separator()
{
    char* result = compile_source("-std=C99", "int i = 1'000;");
    assert(strcmp(result, "int i = 1000;") == 0);
    free(result);
}

void test_lit()
{
    //_Static_assert(1 == 2, "");
    char* result = compile_source("-std=C99", "char * s = u8\"maçã\";");
    assert(strcmp(result, "char * s = \"ma\\xc3\\xa7\\xc3\\xa3\";") == 0);
    free(result);
}

void type_test2()
{
    char* src =
        "int a[10]; "
        "struct X* F() { return 0; }"
        " static_assert(typeid(*F()) == typeid(struct X));"
        " static_assert(typeid(&a) == typeid(int (*)[10]));"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    get_ast(&options, "source", src, &error);
    assert(error.code == 0);

}

void type_test3()
{
    char* src =
        "int i;"
        "int (*f)(void);"
        " static_assert(typeid(&i) == typeid(int *));"
        " static_assert(typeid(&f) == typeid(int (**)(void)));"
        ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C99 };
    get_ast(&options, "source", src, &error);
    assert(error.code == 0);

}

void expand_test()
{
    char* src =
        "typedef int A[2];"
        "typedef A *B [1];"
        "static_assert(typeid(B) == typeid(int (*[1])[2]);";
    ;

    struct error error = { 0 };
    struct options options = { .input = LANGUAGE_C2X };
    get_ast(&options, "source", src, &error);
    assert(error.code == 0);
    clearerror(&error);


    char* src2 =
        "typedef char* A;"
        "typedef const A* B; "
        "static_assert(typeid(B) == typeid(char * const *);";

    get_ast(&options, "source", src2, &error);
    assert(error.code == 0);
    clearerror(&error);



    char* src3 =
        "typedef char* T1;"
        "typedef T1(*f[3])(int); "
        "static_assert(typeid(f) == typeid(char* (* [3])(int)));";
    //char* (* [3])(int)



    get_ast(&options, "source", src3, &error);
    assert(error.code == 0);
    clearerror(&error);


    //https://godbolt.org/z/WbK9zP7zM
}




#endif







struct token* next_parser_token(struct token* token)
{
    struct token* r = token->next;
    while (!(r->flags & TK_FLAG_FINAL))
    {
        r = r->next;
    }
    return r;
}

struct defer_scope* get_last_defer_before_try(struct defer_scope* block)
{
    struct defer_scope* p_defer = block;
    while (p_defer)
    {
        if (p_defer->lastchild != NULL)
        {
            /*achou*/
            return p_defer->lastchild;
        }

        if (p_defer->p_selection_statement2 &&
            p_defer->p_selection_statement2->first_token->type == TK_KEYWORD_TRY)
        {
            /*chegamos no try sem achar nenhum defer*/
            break;
        }

        p_defer = p_defer->previous;
    }

    return NULL;
}

struct defer_scope* get_last_defer_before_try_up(struct defer_scope* block)
{
    struct defer_scope* p_defer = block->previous;
    while (p_defer)
    {
        if (p_defer->lastchild != NULL)
        {
            /*achou*/
            return p_defer->lastchild;
        }

        if (p_defer->p_selection_statement2 &&
            p_defer->p_selection_statement2->first_token->type == TK_KEYWORD_TRY)
        {
            /*chegamos no try sem achar nenhum defer*/
            break;
        }

        p_defer = p_defer->previous;
    }

    return NULL;
}


static void visit_struct_or_union_specifier(struct visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier, struct error* error);
static void visit_expression(struct visit_ctx* ctx, struct expression* p_expression, struct error* error);
static void visit_statement(struct visit_ctx* ctx, struct statement* p_statement, struct error* error);
static void visit_enum_specifier(struct visit_ctx* ctx, struct enum_specifier* p_enum_specifier, struct error* error);
static void visit_type_specifier(bool is_declaration, struct visit_ctx* ctx, struct type_specifier* p_type_specifier, struct error* error);

void convert_if_statement(struct visit_ctx* ctx, struct selection_statement* p_selection_statement, struct error* error)
{
    if (p_selection_statement->init_declarator == NULL ||
        p_selection_statement->first_token->type != TK_KEYWORD_IF)
    {
        return;
    }

    struct token* before_first_token =
        p_selection_statement->first_token->prev;
    struct token* token =
        next_parser_token(p_selection_statement->first_token);
    token = next_parser_token(token);
    struct token* first = token;
    while (token)
    {
        if (token->type == ';')
            break;
        token = next_parser_token(token);
    }
    struct token_list list =
        token_list_remove(&ctx->ast.token_list, first, token);
    token_list_insert_after(&ctx->ast.token_list,
        before_first_token,
        &list);
    struct token_list list1 = tokenizer("{", "", 0, TK_FLAG_NONE, error);
    token_list_insert_after(&ctx->ast.token_list,
        before_first_token,
        &list1);
    struct token_list list2 = tokenizer("}", "", 0, TK_FLAG_NONE, error);
    token_list_insert_after(&ctx->ast.token_list,
        previous_parser_token(p_selection_statement->last_token),
        &list2);
}

void print_block_defer(struct defer_scope* deferblock, struct osstream* ss, bool bHide)
{
    struct defer_scope* deferchild = deferblock->lastchild;
    while (deferchild != NULL)
    {
        struct token_list l = { 0 };
        l.head = deferchild->defer_statement->firsttoken;
        l.tail = deferchild->defer_statement->lasttoken;

        l.head->flags |= TK_FLAG_HIDE;
        const char* s = get_code_as_compiler_see(&l);
        assert(s != NULL);
        if (bHide)
            token_range_add_flag(l.head, l.tail, TK_FLAG_HIDE);

        ss_fprintf(ss, "%s", s);
        free(s);
        deferchild = deferchild->previous;
    }
}


void hide_block_defer(struct defer_scope* deferblock)
{
    struct defer_scope* deferchild = deferblock->lastchild;
    while (deferchild != NULL)
    {
        struct token_list l = { 0 };
        l.head = deferchild->defer_statement->firsttoken;
        l.tail = deferchild->defer_statement->lasttoken;
        token_range_add_flag(l.head, l.tail, TK_FLAG_HIDE);
        deferchild = deferchild->previous;
    }
}


void print_all_defer_until_try(struct defer_scope* deferblock, struct osstream* ss)
{
    struct defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        print_block_defer(p_defer, ss, false);
        if (p_defer->p_selection_statement2 &&
            p_defer->p_selection_statement2->first_token->type == TK_KEYWORD_TRY)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

bool find_label_statement(struct statement* statement, const char* label);
bool find_label_unlabeled_statement(struct unlabeled_statement* p_unlabeled_statement, const char* label)
{
    if (p_unlabeled_statement->primary_block)
    {
        if (p_unlabeled_statement->primary_block->compound_statement)
        {
            struct block_item* block_item =
                p_unlabeled_statement->primary_block->compound_statement->block_item_list.head;
            while (block_item)
            {
                if (block_item->label &&
                    strcmp(block_item->label->name->lexeme, label) == 0)
                {
                    /*achou*/
                    return true;
                }
                else if (block_item->unlabeled_statement)
                {
                    if (find_label_unlabeled_statement(block_item->unlabeled_statement, label))
                    {
                        return true;
                    }
                }
                else if (block_item->labeled_statement)
                {
                    if (find_label_statement(block_item->labeled_statement->statement, label))
                    {
                        return true;
                    }

                }
                block_item = block_item->next;
            }
        }
        else if (p_unlabeled_statement->primary_block->selection_statement)
        {
            if (find_label_statement(p_unlabeled_statement->primary_block->selection_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt)
            {
                if (find_label_statement(p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->try_statement)
        {
            if (find_label_statement(p_unlabeled_statement->primary_block->try_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt)
            {
                if (find_label_statement(p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->iteration_statement)
        {
            if (find_label_statement(p_unlabeled_statement->primary_block->iteration_statement->secondary_block->statement, label))
            {
                return true;
            }
        }
    }
    return false;
}

bool find_label_statement(struct statement* statement, const char* label)
{
    if (statement->labeled_statement)
    {
        if (statement->labeled_statement &&
            statement->labeled_statement->label &&
            strcmp(statement->labeled_statement->label->name->lexeme, label) == 0)
        {
            /*achou*/
            return true;
        }
    }
    else if (statement->unlabeled_statement)
    {
        if (find_label_unlabeled_statement(statement->unlabeled_statement, label))
            return true;
    }
    return false;
}

static bool find_label_scope(struct defer_scope* deferblock, const char* label)
{
    if (deferblock->p_iteration_statement)
    {
        if (deferblock->p_iteration_statement->secondary_block)
        {
            if (find_label_statement(deferblock->p_iteration_statement->secondary_block->statement, label))
                return true;
        }
    }
    else if (deferblock->p_selection_statement2)
    {
        if (deferblock->p_selection_statement2->secondary_block)
        {
            if (find_label_statement(deferblock->p_selection_statement2->secondary_block->statement, label))
                return true;
        }

        if (deferblock->p_selection_statement2->else_secondary_block_opt)
        {
            if (find_label_statement(deferblock->p_selection_statement2->else_secondary_block_opt->statement, label))
                return true;
        }
    }
    else if (deferblock->p_try_statement)
    {
        if (deferblock->p_try_statement->secondary_block)
        {
            if (find_label_statement(deferblock->p_try_statement->secondary_block->statement, label))
                return true;
        }

        if (deferblock->p_try_statement->catch_secondary_block_opt)
        {
            if (find_label_statement(deferblock->p_try_statement->catch_secondary_block_opt->statement, label))
                return true;
        }
    }
    else if (deferblock->p_statement)
    {
        if (find_label_statement(deferblock->p_statement, label))
            return true;
    }
    return false;
}

void print_all_defer_until_label(struct defer_scope* deferblock, const char* label, struct osstream* ss)
{
    /*
    * Precisamos saber quantos escopos nós saimos até achar o label.
    * Para isso procuramos no escopo atual aonde aparede o goto.
    * Se o label não esta diretamente neste escopo ou dentro de algum escopo interno
    * Não nós imprimos os defers pois estamos saindo do escopo e vamos para o escopo
    * de cima. Assim vamos repetindo em cada saida de escopo imprimos o defer.
    */
    struct defer_scope* p_defer = deferblock;

    while (p_defer != NULL)
    {
        if (!find_label_scope(p_defer, label))
        {
            print_block_defer(p_defer, ss, false);
        }
        else
        {
            break;
        }
        p_defer = p_defer->previous;
    }

}

void print_all_defer_until_iter(struct defer_scope* deferblock, struct osstream* ss)
{
    struct defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        print_block_defer(p_defer, ss, false);
        if (p_defer->p_iteration_statement)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
}

void print_all_defer_until_end(struct defer_scope* deferblock, struct osstream* ss)
{
    struct defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        print_block_defer(p_defer, ss, false);
        p_defer = p_defer->previous;
    }
}

static void visit_secondary_block(struct visit_ctx* ctx, struct secondary_block* p_secondary_block, struct error* error)
{
    visit_statement(ctx, p_secondary_block->statement, error);
}

static void visit_defer_statement(struct visit_ctx* ctx, struct defer_statement* p_defer_statement, struct error* error)
{


    if (!ctx->is_second_pass)
    {

        //adiciona como filho do ultimo bloco
        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->defer_statement = p_defer_statement;


        p_defer->previous = ctx->tail_block->lastchild;
        ctx->tail_block->lastchild = p_defer;


        if (p_defer_statement->secondary_block)
        {
            visit_secondary_block(ctx, p_defer_statement->secondary_block, error);
        }
    }
    else //if (ctx->is_second_pass)
    {
        if (p_defer_statement->secondary_block)
            visit_secondary_block(ctx, p_defer_statement->secondary_block, error);
    }
}

static void visit_try_statement(struct visit_ctx* ctx, struct try_statement* p_try_statement, struct error* error)
{
    if (!ctx->is_second_pass)
    {
        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->previous = ctx->tail_block;
        ctx->tail_block = p_defer;
        p_defer->p_try_statement = p_try_statement;

        if (p_try_statement->secondary_block)
            visit_secondary_block(ctx, p_try_statement->secondary_block, error);


        struct osstream ss = { 0 };

        print_block_defer(p_defer, &ss, true);
        struct token_list l = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);
        token_list_insert_after(&ctx->ast.token_list, p_try_statement->secondary_block->last->prev, &l);


        if (ctx->tail_block)
        {
            //POP
            ctx->tail_block = ctx->tail_block->previous;
        }

        free(p_try_statement->first_token->lexeme);
        p_try_statement->first_token->lexeme = strdup("if (1) /*try*/");


        char buffer[50] = { 0 };
        if (p_try_statement->catch_secondary_block_opt)
        {

            snprintf(buffer, sizeof buffer, "else _catch_label_%d:", p_try_statement->try_catch_block_index);

            free(p_try_statement->catch_token_opt->lexeme);
            p_try_statement->catch_token_opt->lexeme = strdup(buffer);

            visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt, error);
        }
        else
        {
            snprintf(buffer, sizeof buffer, "} else {_catch_label_%d:;}", p_try_statement->try_catch_block_index);
            free(p_try_statement->last_token->lexeme);
            p_try_statement->last_token->lexeme = strdup(buffer);
        }
    }
}

static void visit_selection_statement(struct visit_ctx* ctx, struct selection_statement* p_selection_statement, struct error* error)
{
    convert_if_statement(ctx, p_selection_statement, error);

    //PUSH
    struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
    p_defer->previous = ctx->tail_block;
    ctx->tail_block = p_defer;
    p_defer->p_selection_statement2 = p_selection_statement;

    if (p_selection_statement->secondary_block)
        visit_secondary_block(ctx, p_selection_statement->secondary_block, error);

    struct osstream ss = { 0 };
    print_block_defer(p_defer, &ss, true);

    if (ss.size > 0)
    {
        struct token_list l = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);
        token_list_insert_after(&ctx->ast.token_list, p_selection_statement->secondary_block->last->prev, &l);
    }
    //POP
    ctx->tail_block = ctx->tail_block->previous;

    if (p_selection_statement->else_secondary_block_opt)
        visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt, error);

}

static void visit_compound_statement(struct visit_ctx* ctx, struct compound_statement* p_compound_statement, struct error* error);



char* remove_char(char* input, char ch)
{
    if (input == NULL)
        return NULL;
    char* pWrite = input;
    const char* p = input;
    while (*p)
    {
        if (p[0] == ch)
        {
            p++;
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

static void visit_initializer_list(struct visit_ctx* ctx, struct initializer_list* p_initializer_list, struct error* error);

static void visit_bracket_initializer_list(struct visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list, struct error* error)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {
        if (ctx->target < LANGUAGE_C2X)
        {
            assert(p_bracket_initializer_list->first->type == '{');

            //Criar token 0
            struct token_list list2 = tokenizer("0", NULL, 0, TK_FLAG_NONE, error);

            //inserir na frente
            token_list_insert_after(&ctx->ast.token_list, p_bracket_initializer_list->first, &list2);
        }
    }
    else
    {
        visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list, error);
    }
}

static void visit_designation(struct visit_ctx* ctx, struct designation* p_designation, struct error* error)
{
}

static void visit_initializer(struct visit_ctx* ctx, struct initializer* p_initializer, struct error* error)
{
    if (p_initializer->designation)
    {
        visit_designation(ctx, p_initializer->designation, error);
    }

    if (p_initializer->assignment_expression)
    {
        visit_expression(ctx, p_initializer->assignment_expression, error);
    }
    else if (p_initializer->braced_initializer)
    {
        visit_bracket_initializer_list(ctx, p_initializer->braced_initializer, error);
    }
}

static void visit_initializer_list(struct visit_ctx* ctx, struct initializer_list* p_initializer_list, struct error* error)
{
    struct initializer* p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        visit_initializer(ctx, p_initializer, error);
        if (error->code != 0)
            break;
        p_initializer = p_initializer->next;
    }
}

static void visit_type_qualifier(struct visit_ctx* ctx, struct type_qualifier* p_type_qualifier, struct error* error)
{
    //p_type_qualifier->
}

static void visit_specifier_qualifier(bool is_declaration, struct visit_ctx* ctx, struct specifier_qualifier* p_specifier_qualifier, struct error* error)
{
    if (p_specifier_qualifier->type_specifier)
        visit_type_specifier(is_declaration, ctx, p_specifier_qualifier->type_specifier, error);

    if (p_specifier_qualifier->type_qualifier)
        visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier, error);
}

static void visit_specifier_qualifier_list(bool is_declaration, struct visit_ctx* ctx, struct specifier_qualifier_list* p_specifier_qualifier_list_opt, struct error* error)
{
    if (p_specifier_qualifier_list_opt == NULL)
        return;

    if (p_specifier_qualifier_list_opt->struct_or_union_specifier)
    {
        visit_struct_or_union_specifier(ctx, p_specifier_qualifier_list_opt->struct_or_union_specifier, error);
    }
    else if (p_specifier_qualifier_list_opt->enum_specifier)
    {
        visit_enum_specifier(ctx, p_specifier_qualifier_list_opt->enum_specifier, error);
    }
    else if (p_specifier_qualifier_list_opt->typedef_declarator)
    {
        //typedef name
    }
    //else if (p_specifier_qualifier_list->p_typeof_expression_opt)
    //{
      //  visit_expression(ctx, p_specifier_qualifier_list->p_typeof_expression_opt, error);
    //}
    else
    {
        struct specifier_qualifier* p_specifier_qualifier = p_specifier_qualifier_list_opt->head;
        while (p_specifier_qualifier)
        {
            visit_specifier_qualifier(is_declaration, ctx, p_specifier_qualifier, error);
            p_specifier_qualifier = p_specifier_qualifier->next;
        }
    }
}

static void visit_type_name(struct visit_ctx* ctx, struct type_name* p_type_name, struct error* error)
{
    visit_specifier_qualifier_list(false, ctx, p_type_name->specifier_qualifier_list, error);
    //visit_declarator
}

#pragma warning(default : 4061)


static void visit_argument_expression_list(struct visit_ctx* ctx, struct argument_expression_list* p_argument_expression_list, struct error* error)
{
    struct argument_expression* p_argument_expression =
        p_argument_expression_list->head;
    while (p_argument_expression)
    {
        visit_expression(ctx, p_argument_expression->expression, error);
        p_argument_expression = p_argument_expression->next;
    }
}

static void visit_generic_selection(struct visit_ctx* ctx, struct generic_selection* p_generic_selection, struct error* error)
{
    visit_expression(ctx, p_generic_selection->expression, error);
    if (ctx->target < LANGUAGE_C11)
    {
        token_range_add_flag(p_generic_selection->first_token, p_generic_selection->last_token, TK_FLAG_HIDE);
        if (p_generic_selection->p_view_selected_expression)
        {
            token_range_remove_flag(p_generic_selection->p_view_selected_expression->first,
                p_generic_selection->p_view_selected_expression->last,
                TK_FLAG_HIDE);
        }
    }

}


static void visit_expression(struct visit_ctx* ctx, struct expression* p_expression, struct error* error)
{
    switch (p_expression->expression_type)
    {
    case PRIMARY_IDENTIFIER:
        break;
    case TYPEID_EXPRESSION_TYPE:
        break;
    case PRIMARY_EXPRESSION_ENUMERATOR:
        break;
    case PRIMARY_EXPRESSION_DECLARATOR:
        //p_expression->declarator->name    
        break;
    case PRIMARY_EXPRESSION_STRING_LITERAL:
        break;
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
        break;
    case PRIMARY_EXPRESSION_NUMBER:
        break;

    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        visit_generic_selection(ctx, p_expression->generic_selection, error);
        break;

    case POSTFIX_DOT:
        break;
    case POSTFIX_ARROW:
        break;
    case POSTFIX_INCREMENT:
        break;
    case POSTFIX_DECREMENT:
        break;
    case POSTFIX_ARRAY:
        //visit_expression(ctx, p_expression->left, error);
        break;
    case POSTFIX_FUNCTION_CALL:
        visit_expression(ctx, p_expression->left, error);
        visit_argument_expression_list(ctx, &p_expression->argument_expression_list, error);
        break;
    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
    {
        ctx->has_lambda = true;
        ctx->is_inside_lambda = true;
        visit_compound_statement(ctx, p_expression->compound_statement, error);
        ctx->is_inside_lambda = false;

        if (ctx->is_second_pass)
        {
            /*no segundo passo nós copiamos a funcao*/
            char name[100] = { 0 };
            snprintf(name, sizeof name, " _lit_func_%d", ctx->lambdas_index);
            ctx->lambdas_index++;

            struct osstream ss = { 0 };

            bool bFirst = true;
            print_specifier_qualifier_list(&ss, &bFirst, p_expression->type_name->specifier_qualifier_list);
            ss_fprintf(&ss, "%s", name);
            print_declarator(&ss, p_expression->type_name->declarator, false);

            struct token_list l1 = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);

            token_list_append_list(&ctx->insert_before_declaration, &l1);
            ss_close(&ss);

            for (struct token* current = p_expression->compound_statement->first_token;
                current != p_expression->compound_statement->last_token->next;
                current = current->next)
            {
                token_list_clone_and_add(&ctx->insert_before_declaration, current);
            }

            token_range_add_flag(p_expression->first, p_expression->last, TK_FLAG_HIDE);

            struct token_list l3 = tokenizer("\n\n", NULL, 0, TK_FLAG_NONE, error);
            token_list_append_list(&ctx->insert_before_declaration, &l3);


            struct token_list l2 = tokenizer(name, NULL, 0, TK_FLAG_FINAL, error);
            token_list_insert_after(&ctx->ast.token_list, p_expression->last, &l2);
        }
    }
    break;

    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

        if (p_expression->type_name)
        {
            visit_type_name(ctx, p_expression->type_name, error);
        }

        //struct token_list list0 = tokenizer("int teste = 0;", NULL, 0, TK_FLAG_NONE, error);
        //token_list_append_list(&ctx->insert_before_block_item, &list0);

        visit_bracket_initializer_list(ctx, p_expression->braced_initializer, error);

        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);

        break;

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:
    case UNARY_EXPRESSION_SIZEOF_TYPE:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:

    case UNARY_EXPRESSION_NOT:
    case UNARY_EXPRESSION_BITNOT:
    case UNARY_EXPRESSION_NEG:
    case UNARY_EXPRESSION_PLUS:
    case UNARY_EXPRESSION_CONTENT:
    case UNARY_EXPRESSION_ADDRESSOF:
        if (p_expression->right)
        {
            visit_expression(ctx, p_expression->right, error);
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            visit_type_name(ctx, p_expression->type_name, error);
        }

        break;

    case UNARY_EXPRESSION_HASHOF_TYPE:

        if (!ctx->is_second_pass)
        {
            token_range_add_flag(p_expression->first, p_expression->last, TK_FLAG_HIDE);

            char buffer[30] = { 0 };
            snprintf(buffer, sizeof buffer, "%lld", p_expression->constant_value);

            struct token_list l3 = tokenizer(buffer, NULL, 0, TK_FLAG_NONE, error);
            token_list_insert_after(&ctx->ast.token_list, p_expression->last, &l3);
        }
        break;

    case UNARY_EXPRESSION_ALIGNOF:
    case CAST_EXPRESSION:
    case ASSIGNMENT_EXPRESSION:
    case MULTIPLICATIVE_EXPRESSION_MULT:
    case MULTIPLICATIVE_EXPRESSION_DIV:
    case MULTIPLICATIVE_EXPRESSION_MOD:
    case ADDITIVE_EXPRESSION_PLUS:
    case ADDITIVE_EXPRESSION_MINUS:
    case SHIFT_EXPRESSION_RIGHT:
    case SHIFT_EXPRESSION_LEFT:
    case RELATIONAL_EXPRESSION_BIGGER_THAN:
    case RELATIONAL_EXPRESSION_LESS_THAN:
    case EQUALITY_EXPRESSION_EQUAL:
    case EQUALITY_EXPRESSION_NOT_EQUAL:
    case AND_EXPRESSION:
    case EXCLUSIVE_OR_EXPRESSION:
    case INCLUSIVE_OR_EXPRESSION:
    case INCLUSIVE_AND_EXPRESSION:
    case LOGICAL_OR_EXPRESSION:
    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:

        if (p_expression->left)
        {
            visit_expression(ctx, p_expression->left, error);
        }
        if (p_expression->right)
        {
            visit_expression(ctx, p_expression->right, error);
        }
        if (p_expression->type_name)
        {
            visit_type_name(ctx, p_expression->type_name, error);
        }
        break;
    default:
        break;
    }
}

static void visit_expression_statement(struct visit_ctx* ctx, struct expression_statement* p_expression_statement, struct error* error)
{
    if (p_expression_statement->expression)
        visit_expression(ctx, p_expression_statement->expression, error);
}

static void visit_block_item_list(struct visit_ctx* ctx, struct block_item_list* p_block_item_list, struct error* error);

static void visit_compound_statement(struct visit_ctx* ctx, struct compound_statement* p_compound_statement, struct error* error)
{
    visit_block_item_list(ctx, &p_compound_statement->block_item_list, error);
}

//
static void visit_iteration_statement(struct visit_ctx* ctx, struct iteration_statement* p_iteration_statement, struct error* error)
{

    if (p_iteration_statement->expression1)
    {
        visit_expression(ctx, p_iteration_statement->expression1, error);
    }

    if (p_iteration_statement->expression2)
    {
        visit_expression(ctx, p_iteration_statement->expression2, error);
    }

    if (p_iteration_statement->token->type == TK_KEYWORD_REPEAT)
    {
        free(p_iteration_statement->token->lexeme);
        p_iteration_statement->token->lexeme = strdup("for(;;)/*repeat*/");
    }

    if (p_iteration_statement->secondary_block)
    {
        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->previous = ctx->tail_block;
        ctx->tail_block = p_defer;
        p_defer->p_iteration_statement = p_iteration_statement;

        visit_secondary_block(ctx, p_iteration_statement->secondary_block, error);

        struct osstream ss = { 0 };
        //defer_print(defer, &ss, ctx->bHasThrowWithVariable, ctx->bHasBreakWithVariable, ctx->bHasReturnWithVariable);
        print_block_defer(p_defer, &ss, true);

        struct token_list l = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);
        token_list_insert_after(&ctx->ast.token_list, p_iteration_statement->secondary_block->last->prev, &l);


        if (ctx->tail_block)
        {
            //POP
            ctx->tail_block = ctx->tail_block->previous;
        }

    }
}



static void visit_jump_statement(struct visit_ctx* ctx, struct jump_statement* p_jump_statement, struct error* error)
{

    if (p_jump_statement->token->type == TK_KEYWORD_THROW)
    {
        struct osstream ss0 = { 0 };
        print_all_defer_until_try(ctx->tail_block, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "goto _catch_label_%d;", p_jump_statement->try_catch_block_index);
            ss_fprintf(&ss, "}");
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;
            p_jump_statement->lasttoken->flags |= TK_FLAG_HIDE;
        }
        else
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "goto _catch_label_%d", p_jump_statement->try_catch_block_index);
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;
        }
    }
    else if (p_jump_statement->token->type == TK_KEYWORD_RETURN)
    {
        struct osstream ss0 = { 0 };
        print_all_defer_until_end(ctx->tail_block, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "return");
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;
            free(p_jump_statement->lasttoken->lexeme);
            p_jump_statement->lasttoken->lexeme = strdup(";}");
        }
    }
    else if (p_jump_statement->token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->token->type == TK_KEYWORD_CONTINUE)
    {
        struct osstream ss0 = { 0 };

        print_all_defer_until_iter(ctx->tail_block, &ss0);
        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "break;");
            ss_fprintf(&ss, "}");
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;  /*MOVED*/
            p_jump_statement->lasttoken->flags |= TK_FLAG_HIDE;
        }
    }
    else if (p_jump_statement->token->type == TK_KEYWORD_GOTO)
    {
        struct osstream ss0 = { 0 };
        print_all_defer_until_label(ctx->tail_block, p_jump_statement->label->lexeme, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "goto");
            free(p_jump_statement->token->lexeme);
            p_jump_statement->token->lexeme = ss.c_str;
            free(p_jump_statement->lasttoken->lexeme);
            p_jump_statement->lasttoken->lexeme = strdup(";}");
        }
    }
    else
    {
        assert(false);
    }
}


static void visit_labeled_statement(struct visit_ctx* ctx, struct labeled_statement* p_labeled_statement, struct error* error)
{
    //p_labeled_statement->label

    if (p_labeled_statement->statement)
    {
        visit_statement(ctx, p_labeled_statement->statement, error);
    }
}

static void visit_primary_block(struct visit_ctx* ctx, struct primary_block* p_primary_block, struct error* error)
{



    if (p_primary_block->defer_statement)
    {
        visit_defer_statement(ctx, p_primary_block->defer_statement, error);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            visit_compound_statement(ctx, p_primary_block->compound_statement, error);
        }
        else if (p_primary_block->iteration_statement)
        {
            visit_iteration_statement(ctx, p_primary_block->iteration_statement, error);
        }
        else if (p_primary_block->selection_statement)
        {
            visit_selection_statement(ctx, p_primary_block->selection_statement, error);
        }
        else if (p_primary_block->try_statement)
        {
            visit_try_statement(ctx, p_primary_block->try_statement, error);
        }
    }

}

static void visit_unlabeled_statement(struct visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, struct error* error)
{
    if (p_unlabeled_statement->primary_block)
    {
        visit_primary_block(ctx, p_unlabeled_statement->primary_block, error);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        visit_expression_statement(ctx, p_unlabeled_statement->expression_statement, error);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        visit_jump_statement(ctx, p_unlabeled_statement->jump_statement, error);
    }
    else
    {
        assert(false);
    }
}

static void visit_declaration(struct visit_ctx* ctx, struct declaration* p_declaration, struct error* error);

static void visit_statement(struct visit_ctx* ctx, struct statement* p_statement, struct error* error)
{
    if (p_statement->labeled_statement)
    {
        visit_labeled_statement(ctx, p_statement->labeled_statement, error);
    }
    else if (p_statement->unlabeled_statement)
    {
        visit_unlabeled_statement(ctx, p_statement->unlabeled_statement, error);
    }
}


static void visit_block_item(struct visit_ctx* ctx, struct block_item* p_block_item, struct error* error)
{
    if (p_block_item->declaration)
    {
        visit_declaration(ctx, p_block_item->declaration, error);
    }
    else if (p_block_item->unlabeled_statement)
    {
        visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement, error);
    }
    else if (p_block_item->labeled_statement)
    {
        visit_labeled_statement(ctx, p_block_item->labeled_statement, error);
    }
    if (ctx->insert_before_block_item.head != NULL)
    {
        //token_list_insert_after(&ctx->ast.token_list, p_statement->first_token->prev, &ctx->insert_before_statement);
        token_list_insert_after(&ctx->ast.token_list, p_block_item->first_token->prev, &ctx->insert_before_block_item);
        ctx->insert_before_block_item.head = NULL;
        ctx->insert_before_block_item.tail = NULL;
    }
}

static void visit_block_item_list(struct visit_ctx* ctx, struct block_item_list* p_block_item_list, struct error* error)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (error->code == 0 && p_block_item)
    {
        visit_block_item(ctx, p_block_item, error);
        p_block_item = p_block_item->next;
    }
}

static void visit_static_assert_declaration(struct visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration, struct error* error)
{
    visit_expression(ctx, p_static_assert_declaration->constant_expression, error);

    if (ctx->target < LANGUAGE_C11)
    {
        /*
        * Vamos apagar a parte do static assert. Não adianta so commentar
        * pq poderia ter um comentario dentro. (so se verificar que nao tem)
        */
        for (struct token* p = p_static_assert_declaration->first_token;
            p != p_static_assert_declaration->last_token->next;
            p = p->next)
        {
            /*
             Se eu colocar como sendo macro expanded vai esconder
            */
            p->flags |= TK_FLAG_MACRO_EXPANDED;
        }
    }
    else if (ctx->target == LANGUAGE_C11)
    {
        if (p_static_assert_declaration->string_literal_opt == NULL)
        {
            struct token* rp = previous_parser_token(p_static_assert_declaration->last_token);
            rp = previous_parser_token(rp);

            struct token_list list1 = tokenizer(", \"error\"", "", 0, TK_FLAG_NONE, error);
            token_list_insert_after(&ctx->ast.token_list, rp, &list1);
        }
        if (strcmp(p_static_assert_declaration->first_token->lexeme, "static_assert") == 0)
        {
            /*C23 has static_assert but C11 _Static_assert*/
            free(p_static_assert_declaration->first_token->lexeme);
            p_static_assert_declaration->first_token->lexeme = strdup("_Static_assert");
        }
    }
}


static void visit_init_declarator_list(struct visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list, struct declarator_type* p_declarator_type, struct error* error)
{
    struct init_declarator* p_init_declarator = p_init_declarator_list->head;


    while (p_init_declarator)
    {
        
        if (p_declarator_type)
        {
            
            struct osstream ss = { 0 };
            print_declarator_type(&ss, p_declarator_type, p_init_declarator->declarator->name->lexeme);
            
            struct token_list l2 = tokenizer(ss.c_str, NULL, 0, TK_FLAG_NONE, error);
            
            l2.head->flags = p_init_declarator->declarator->name->flags;
            token_list_insert_after(&ctx->ast.token_list, p_init_declarator->declarator->name, &l2);

            p_init_declarator->declarator->name->flags |= TK_FLAG_HIDE;
        }
        

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                visit_expression(ctx, p_init_declarator->initializer->assignment_expression, error);
            }
            else
            {
                assert(p_init_declarator->initializer->braced_initializer != NULL);
                if (p_init_declarator->initializer->braced_initializer)
                {
                    visit_bracket_initializer_list(ctx,
                        p_init_declarator->initializer->braced_initializer, error);
                }

            }
        }

        p_init_declarator = p_init_declarator->next;
    }
}


static void visit_member_declarator(struct visit_ctx* ctx, struct member_declarator* p_member_declarator, struct error* error)
{
    if (p_member_declarator->declarator)
    {

    }
}

static void visit_member_declarator_list(struct visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list, struct error* error)
{
    struct member_declarator* p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        visit_member_declarator(ctx, p_member_declarator, error);
        p_member_declarator = p_member_declarator->next;
    }
}
static void visit_member_declaration(struct visit_ctx* ctx, struct member_declaration* p_member_declaration, struct error* error)
{
    visit_specifier_qualifier_list(true, ctx, p_member_declaration->specifier_qualifier_list, error);

    if (p_member_declaration->member_declarator_list_opt)
    {
        visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt, error);
    }
}

static void visit_member_declaration_list(struct visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list, struct error* error)
{
    struct member_declaration* p_member_declaration =
        p_member_declaration_list->head;
    while (p_member_declaration)
    {
        visit_member_declaration(ctx, p_member_declaration, error);
        p_member_declaration = p_member_declaration->next;
    }
}

static void visit_attribute_specifier(struct visit_ctx* ctx, struct attribute_specifier* p_attribute_specifier, struct error* error)
{
    if (ctx->target < LANGUAGE_C2X)
    {
        token_range_add_flag(p_attribute_specifier->first, p_attribute_specifier->last, TK_FLAG_HIDE);
    }
}

static void visit_attribute_specifier_sequence(struct visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence, struct error* error)
{
    struct attribute_specifier* current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        visit_attribute_specifier(ctx, current, error);
        current = current->next;
    }
}

static void visit_struct_or_union_specifier(struct visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier, struct error* error)
{

    if (p_struct_or_union_specifier->attribute_specifier_sequence)
        visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence, error);

    if (p_struct_or_union_specifier->complete_struct_or_union_specifier)
    {
        if (ctx->is_inside_lambda && !ctx->is_second_pass)
        {
            /*
              Na primeira passada marcamos os tipos que são renomeados
            */
            if (p_struct_or_union_specifier->complete_struct_or_union_specifier->scope_level >
                p_struct_or_union_specifier->scope_level &&
                p_struct_or_union_specifier->complete_struct_or_union_specifier->visit_moved == 0)
            {
                char newtag[200];
                snprintf(newtag, sizeof newtag, "_%s%d", p_struct_or_union_specifier->tag_name, ctx->capture_index);
                ctx->capture_index++;

                free(p_struct_or_union_specifier->complete_struct_or_union_specifier->tagtoken->lexeme);
                p_struct_or_union_specifier->complete_struct_or_union_specifier->tagtoken->lexeme =
                    strdup(newtag);

                p_struct_or_union_specifier->complete_struct_or_union_specifier->visit_moved = 1;
            }
        }
        else if (ctx->is_second_pass)
        {
            /*
             Na segunda passada vou renomear quem usa este tag exporado
            */
            if (p_struct_or_union_specifier->complete_struct_or_union_specifier->visit_moved == 1)
            {
                free(p_struct_or_union_specifier->tagtoken->lexeme);
                p_struct_or_union_specifier->tagtoken->lexeme =
                    strdup(p_struct_or_union_specifier->complete_struct_or_union_specifier->tagtoken->lexeme);
            }
        }
    }



    visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list, error);

}

static void visit_enumerator(struct visit_ctx* ctx, struct enumerator* p_enumerator, struct error* error)
{
    if (p_enumerator->constant_expression_opt)
        visit_expression(ctx, p_enumerator->constant_expression_opt, error);

}

//struct enumerator_list* enumerator_list;
static void visit_enumerator_list(struct visit_ctx* ctx, struct enumerator_list* p_enumerator_list, struct error* error)
{
    struct enumerator* current = p_enumerator_list->head;
    while (error->code == 0 && current)
    {
        visit_enumerator(ctx, current, error);
        current = current->next;
    }
}

static void visit_enum_specifier(struct visit_ctx* ctx, struct enum_specifier* p_enum_specifier, struct error* error)
{

    visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list, error);

}

static void visit_typeof_specifier(bool is_declaration, struct visit_ctx* ctx, struct type_specifier* p_type_specifier, struct error* error)
{
  

    if (ctx->target < LANGUAGE_C2X)
    {
        if (p_type_specifier->typeof_specifier)
        {
            /*
            * let's hide the typeof(..) tokens
            */
            token_range_add_flag(p_type_specifier->typeof_specifier->token,
                p_type_specifier->typeof_specifier->endtoken,
                TK_FLAG_HIDE);


            struct osstream ss = { 0 };

            if (p_type_specifier->typeof_specifier->typeof_specifier_argument->expression)
            {
                if (!is_declaration)
                {
                    /*
                    * typeof used like type-name
                    * for instance sizeof(typeof(a)) -> sizeof(int [2])
                    */
                    print_type(&ss, &p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type);
                }
                else
                {
                    /*
                    * typeof used in declarations
                    * We need to split specifiers and later add declarator part.
                    */
                    print_type_qualifier_specifiers(&ss, &p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type);
                }

                /*
                * typeof of anonymous struct?
                */
                if (p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier &&
                    p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier->has_anonymous_tag)
                {
                    
                    p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier->has_anonymous_tag = false;

                    struct token* first = p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier->first;

                    const char* tag = p_type_specifier->typeof_specifier->typeof_specifier_argument->expression->type.struct_or_union_specifier->tag_name;
                    char buffer[200] = { 0 };
                    snprintf(buffer, sizeof buffer, " %s", tag);
                    struct token_list l2 = tokenizer(buffer, NULL, 0, TK_FLAG_NONE, error);
                    token_list_insert_after(&ctx->ast.token_list, first, &l2);

                }

            }
            else if (p_type_specifier->typeof_specifier->typeof_specifier_argument->type_name)
            {                
                if (!is_declaration)
                {
                    /*
                    * typeof used like type-name
                    * for instance sizeof(typeof(a)) -> sizeof(int [2])
                    */
                    print_type(&ss, &p_type_specifier->typeof_specifier->typeof_specifier_argument->type_name->declarator->type);
                }
                else
                {
                    /*
                    * typeof used in declarations
                    * We need to split specifiers and later add declarator part.
                    */
                    print_type_qualifier_specifiers(&ss, &p_type_specifier->typeof_specifier->typeof_specifier_argument->type_name->declarator->type);
                }
            }
            else
            {
                assert(false);
            }

            
       
            struct token_list list = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);            
            ss_close(&ss);
            token_list_insert_after(&ctx->ast.token_list, p_type_specifier->typeof_specifier->endtoken, &list);            
        }
    }
}

static void visit_type_specifier(bool is_declaration, struct visit_ctx* ctx, struct type_specifier* p_type_specifier, struct error* error)
{
    if (p_type_specifier->flags & TYPE_SPECIFIER_TYPEOF)
    {
        visit_typeof_specifier(is_declaration, ctx, p_type_specifier, error);
    }
}

static void visit_type_specifier_qualifier(bool is_declaration, struct visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier, struct error* error)
{
    if (p_type_specifier_qualifier->type_qualifier)
    {
    }
    else if (p_type_specifier_qualifier->type_specifier)
    {
        visit_type_specifier(is_declaration, ctx, p_type_specifier_qualifier->type_specifier, error);
    }
    else if (p_type_specifier_qualifier->alignment_specifier)
    {
    }
}
static void visit_declaration_specifier(bool is_declaration, struct visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier, struct error* error)
{

    if (p_declaration_specifier->function_specifier)
    {
    }

    if (p_declaration_specifier->storage_class_specifier)
    {
    }

    if (p_declaration_specifier->type_specifier_qualifier)
    {
        visit_type_specifier_qualifier(is_declaration, ctx, p_declaration_specifier->type_specifier_qualifier, error);

    }

}

static void visit_declaration_specifiers(struct visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers, struct error* error)
{
    //p_declaration_specifiers->head->type_specifier_qualifier->pType_specifier->p_typeof_expression_opt

    if (p_declaration_specifiers->struct_or_union_specifier)
    {
        visit_struct_or_union_specifier(ctx, p_declaration_specifiers->struct_or_union_specifier, error);
    }
    else if (p_declaration_specifiers->enum_specifier)
    {
        visit_enum_specifier(ctx, p_declaration_specifiers->enum_specifier, error);
    }
    else if (p_declaration_specifiers->typedef_declarator)
    {
        //typedef name
    }
    else if (p_declaration_specifiers->p_typeof_expression_opt)
    {
        visit_expression(ctx, p_declaration_specifiers->p_typeof_expression_opt, error);
    }
    else
    {
        struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;
        while (p_declaration_specifier)
        {
            visit_declaration_specifier(true, ctx, p_declaration_specifier, error);
            p_declaration_specifier = p_declaration_specifier->next;
        }
    }
}

/*
* retorna true se o ultimo item for um return
*/
static bool is_last_item_return(struct compound_statement* p_compound_statement)
{
    if (p_compound_statement &&
        p_compound_statement->block_item_list.tail &&
        p_compound_statement->block_item_list.tail->unlabeled_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->token &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->token->type == TK_KEYWORD_RETURN)
    {
        return true;
    }
    return false;
}

static void visit_declaration(struct visit_ctx* ctx, struct declaration* p_declaration, struct error* error)
{
    if (p_declaration->static_assert_declaration)
    {
        visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration, error);
    }

    if (p_declaration->declaration_specifiers)
    {
        visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, error);
    }

    if (ctx->is_second_pass)
    {

        if (p_declaration->declaration_specifiers &&
            p_declaration->declaration_specifiers->type_specifier_flags == TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            if (p_declaration->declaration_specifiers->struct_or_union_specifier->visit_moved == 1)
            {
                struct token_list list0 = tokenizer("struct ", NULL, 0, TK_FLAG_FINAL, error);
                token_list_append_list(&ctx->insert_before_declaration, &list0);

                struct token_list list = tokenizer(p_declaration->declaration_specifiers->struct_or_union_specifier->tagtoken->lexeme, NULL, 0, TK_FLAG_FINAL, error);
                token_list_append_list(&ctx->insert_before_declaration, &list);


                //struct token_list list3 = tokenizer("{", NULL, 0, TK_FLAG_FINAL, error);
                //token_list_append_list(&ctx->insert_before_declaration, &list3);



                struct token* first = p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.first;
                struct token* last = p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.last;
                for (struct token* current = first;
                    current != last->next;
                    current = current->next)
                {
                    token_list_clone_and_add(&ctx->insert_before_declaration, current);
                    //current->flags |= TK_FLAG_FINAL;
                }

                struct token_list list3 = tokenizer(";\n", NULL, 0, TK_FLAG_FINAL, error);
                token_list_append_list(&ctx->insert_before_declaration, &list3);


                if (p_declaration->init_declarator_list.head == NULL)
                {
                    token_range_add_flag(p_declaration->declaration_specifiers->struct_or_union_specifier->first,
                        p_declaration->declaration_specifiers->struct_or_union_specifier->last,
                        TK_FLAG_HIDE);
                }
                else
                {
                    token_range_add_flag(p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.first,
                        p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.last,
                        TK_FLAG_HIDE);
                }
            }
        }
    }


    if (p_declaration->init_declarator_list.head)
    {
        struct declarator_type* p_declarator_type = NULL;
        if (p_declaration->declaration_specifiers->type_specifier_flags & TYPE_SPECIFIER_TYPEOF)
        {
            /*we need to use transmit the declarator part of the type*/
            if (p_declaration->declaration_specifiers->typeof_specifier->typeof_specifier_argument->expression)
            {
                p_declarator_type = p_declaration->declaration_specifiers->typeof_specifier->typeof_specifier_argument->expression->type.declarator_type;
            }
            else if (p_declaration->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name)
            {
                p_declarator_type = p_declaration->declaration_specifiers->typeof_specifier->typeof_specifier_argument->type_name->declarator->type.declarator_type;
            }

        }
        visit_init_declarator_list(ctx, &p_declaration->init_declarator_list, p_declarator_type ,error);
    }

    if (p_declaration->function_body)
    {
        ctx->has_lambda = false;
        ctx->is_second_pass = false;


        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        ctx->tail_block = p_defer;
        p_defer->p_function_body = p_declaration->function_body;

        visit_compound_statement(ctx, p_declaration->function_body, error);

        if (!is_last_item_return(p_declaration->function_body))
        {
            struct osstream ss = { 0 };
            print_block_defer(p_defer, &ss, true);
            if (ss.size > 0)
            {
                struct token_list l = tokenizer(ss.c_str, NULL, 0, TK_FLAG_FINAL, error);
                token_list_insert_after(&ctx->ast.token_list, p_declaration->function_body->last_token->prev, &l);
            }

        }
        else
        {
            //ja tem um return antes que chama defer
            hide_block_defer(p_defer);
        }


        ctx->tail_block = NULL;


        if (ctx->has_lambda)
        {
            /*functions with lambdas requires two phases*/
            ctx->is_second_pass = true;
            visit_compound_statement(ctx, p_declaration->function_body, error);
        }
    }
}

int visit_literal_string(struct visit_ctx* ctx, struct token* current, struct error* error)
{
    bool bUtf8Prefix =
        current->lexeme[0] == 'u' && current->lexeme[1] == '8';

    if (bUtf8Prefix && ctx->target < LANGUAGE_C11)
    {
        struct osstream ss = { 0 };
        unsigned char* psz = current->lexeme + 2;

        while (*psz)
        {
            if (*psz >= 128)
            {
                ss_fprintf(&ss, "\\x%x", *psz);
            }
            else
            {
                ss_fprintf(&ss, "%c", *psz);
            }
            psz++;
        }

        free(current->lexeme);
        current->lexeme = ss.c_str;
        ss.c_str = NULL;
        ss_close(&ss);
    }

    return error->code;
}

int visit_tokens(struct visit_ctx* ctx, struct error* error)
{
    struct token* current = ctx->ast.token_list.head;
    while (current)
    {

        if (current->type == TK_STRING_LITERAL)
        {
            visit_literal_string(ctx, current, error);
        }

        if (ctx->target < LANGUAGE_C2X)
        {
            if (current->type == TK_KEYWORD_TRUE)
            {
                free(current->lexeme);
                if (ctx->target < LANGUAGE_C99)
                    current->lexeme = strdup("1");
                else
                    current->lexeme = strdup("((_Bool)1)");
            }
            else if (current->type == TK_KEYWORD_FALSE)
            {
                free(current->lexeme);
                if (ctx->target < LANGUAGE_C99)
                    current->lexeme = strdup("0");
                else
                    current->lexeme = strdup("((_Bool)0)");

            }
            else if (current->type == TK_KEYWORD_NULLPTR)
            {
                free(current->lexeme);
                current->lexeme = strdup("((void*)0)");
            }
            else if (current->type == TK_KEYWORD__BOOL)
            {

                if (ctx->target < LANGUAGE_C99)
                {
                    free(current->lexeme);
                    current->lexeme = strdup("int");
                }
                else
                {
                    if (ctx->target < LANGUAGE_C2X)
                    {
                        if (strcmp(current->lexeme, "bool") == 0)
                        {
                            free(current->lexeme);
                            current->lexeme = strdup("_Bool");
                        }
                    }
                }
            }
            else if (current->type == TK_KEYWORD_RESTRICT)
            {
                if (ctx->target < LANGUAGE_C99)
                {
                    free(current->lexeme);
                    current->lexeme = strdup("/*restrict*/");
                }
            }
            else if (current->type == TK_LINE_COMMENT)
            {
                if (ctx->target < LANGUAGE_C99)
                {
                    struct osstream ss = { 0 };
                    //TODO  check /* inside
                    ss_fprintf(&ss, "/*%s*/", current->lexeme + 2);
                    free(current->lexeme);
                    current->lexeme = ss.c_str;/*MOVED*/
                }
            }
            else if (current->type == TK_PREPROCESSOR_LINE)
            {
                while (current->next && current->next->type == TK_BLANKS)
                    current = current->next;

                /*
                  Trocar C23 #elifdef e #elifndef
                */
                if (current->next && strcmp(current->next->lexeme, "elifdef") == 0)
                {
                    free(current->next->lexeme);
                    current->next->lexeme = strdup("elif defined ");
                    current = current->next->next;
                    continue;
                }
                else if (current->next && strcmp(current->next->lexeme, "elifndef") == 0)
                {
                    free(current->next->lexeme);
                    current->lexeme = strdup("elif ! defined ");
                    current = current->next->next;
                    continue;
                }
            }

            if (
                (current->type == TK_COMPILER_BINARY_CONSTANT) ||
                (current->type == TK_PPNUMBER && current->lexeme[0] == '0' && (current->lexeme[1] == 'b' || current->lexeme[1] == 'B')) /*dentro macros*/
                )
            {
                /*remove digit separators*/
                remove_char(current->lexeme, '\'');

                /*
                * Binary literals were added into C23.
                * We are converting to C99 hex.
                */
                current->type = TK_COMPILER_HEXADECIMAL_CONSTANT;
                int value = strtol(current->lexeme + 2, 0, 2);
                char buffer[33 + 2] = { '0', 'x' };
                snprintf(buffer, sizeof buffer, "0x%x", value);
                free(current->lexeme);
                current->lexeme = strdup(buffer);
            }
            else if (current->type == TK_COMPILER_DECIMAL_CONSTANT ||
                current->type == TK_COMPILER_OCTAL_CONSTANT ||
                current->type == TK_COMPILER_HEXADECIMAL_CONSTANT ||
                current->type == TK_COMPILER_DECIMAL_FLOATING_CONSTANT ||
                current->type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT ||
                current->type == TK_PPNUMBER)
            {
                /*remove digit separators*/
                remove_char(current->lexeme, '\'');
            }
        }

        current = current->next;
    }
    return error->code;
}

void visit(struct visit_ctx* ctx, struct error* error)
{
    ctx->capture_index = 0;
    ctx->lambdas_index = 0;
    ctx->typeof_index = 0;

    if (visit_tokens(ctx, error) != 0)
        return;

    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (error->code == 0 && p_declaration)
    {
        visit_declaration(ctx, p_declaration, error);

        if (ctx->insert_before_block_item.head != NULL)
        {
            //token_list_insert_after(&ctx->ast.token_list, p_statement->first_token->prev, &ctx->insert_before_statement);
            token_list_insert_after(&ctx->ast.token_list, p_declaration->first_token->prev, &ctx->insert_before_block_item);
            ctx->insert_before_block_item.head = NULL;
            ctx->insert_before_block_item.tail = NULL;
        }


        /*
        * Tem que inserir algo antes desta declaracao?
        */
        if (ctx->insert_before_declaration.head != NULL)
        {
            token_list_insert_after(&ctx->ast.token_list, p_declaration->first_token->prev, &ctx->insert_before_declaration);
            ctx->insert_before_declaration.head = NULL;
            ctx->insert_before_declaration.tail = NULL;
        }

        p_declaration = p_declaration->next;
    }
    //if (ctx->instanciations.head != NULL)
    //{
    //    token_list_append_list(&ctx->ast.token_list, &ctx->instanciations);
    //}
}



#ifdef _WIN32


#include <winerror.h>
//#include <winsock2.h>
#endif

/*
  The seterror function returns the number of characters that are written,
  not counting the terminating null character.

  If the buffer size specified by er->capacity isn't sufficiently large to contain
  the output specified by string_format and argptr, the return value of seterror is the
  number of characters that would be written, not counting the null character,
  if er->capacity were sufficiently large.

  If the return value is greater than count - 1, the output has been truncated.

  A return value of -1 indicates that an encoding error has occurred.
*/
int seterror(struct error* er, _In_z_ _Printf_format_string_  const char* fmt, ...)
{

    /*the usage follow a pattern that the error is set just once*/
    assert(er->message[0] == 0);
    //assert(er->code == 0);

    er->code = 1;

    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(er->message, sizeof(er->message), fmt, args);
    va_end(args);

    //#ifdef _DEBUG
    //printf("seterror(%s)\n", er->message);
    //#endif

    //printf("%s\n",er->message);
    return n;
}

void clearerror(struct error* err)
{
    err->code = 0;
    err->message[0] = 0;
}

const char* get_posix_error_message(int error)
{
    switch (error)
    {
    case EPERM:
        return "Operation not permitted";
    case  ENOENT:
        return "No such file or directory";
    case  ESRCH:
        return "No such process";
    case  EINTR:
        return "Interrupted system call";
    case  EIO:
        return "I/O error";
    case  ENXIO:
        return "No such device or address";
    case  E2BIG:
        return "Arg list too long";
    case  ENOEXEC:
        return "Exec string_format error";
    case  EBADF:
        return "Bad file number";
    case  ECHILD:
        return "No child processes";
    case  EAGAIN:
        return "Try again";
    case  ENOMEM:
        return "Out of memory";
    case  EACCES:
        return "Permission denied";
    case  EFAULT:
        return "Bad address";
    case  EBUSY:
        return "Device or resource busy";
    case  EEXIST:
        return "File exists";
    case  EXDEV:
        return "Cross-device link";
    case  ENODEV:
        return "No such device";
    case  ENOTDIR:
        return "Not a directory";
    case  EISDIR:
        return "Is a directory";
    case  EINVAL:
        return "Invalid argument";
    case  ENFILE:
        return "File table overflow";
    case  EMFILE:
        return "Too many open files";
    case  ENOTTY:
        return "Not a typewriter";
    case  ETXTBSY:
        return "Text file busy";
    case  EFBIG:
        return "File too large";
    case  ENOSPC:
        return "No space left on device";
    case  ESPIPE:
        return "Illegal seek";
    case  EROFS:
        return "Read-only file system";
    case  EMLINK:
        return "Too many links";
    case  EPIPE:
        return "Broken pipe";
    case  EDOM:
        return "Math argument out of domain of func";
    case  ERANGE:
        return "Math result not representable";
    case  EDEADLK:
        return "Resource deadlock would occur";
    case  ENAMETOOLONG:
        return "File name too long";
    case  ENOLCK:
        return "No record locks available";
    case  ENOSYS:
        return "Function not implemented";
    case  ENOTEMPTY:
        return "Directory not empty";
    case  ELOOP:
        return "Too many symbolic links encountered";
    //case  EWOULDBLOCK:
    //case EAGAIN:  return "Operation would block";
    case  ENOMSG:
        return "No message of desired type";
    case  EIDRM:
        return "Identifier removed";
    case  ENOSTR:
        return "Device not a stream";
    case  ENODATA:
        return "No data available";
    case  ETIME:
        return "Timer expired";
    case  ENOSR:
        return "Out of streams resources";

    case  ENOLINK:
        return "Link has been severed";
    case  EPROTO:
        return "Protocol error";
    case  EBADMSG:
        return "Not a data message";
    case  EOVERFLOW:
        return "Value too large for defined data type";
    case  EILSEQ:
        return "Illegal byte sequence";
    case  ENOTSOCK:
        return "Socket operation on non-socket";
    case  EDESTADDRREQ:
        return "Destination address required";
    case  EMSGSIZE:
        return "Message too long";
    case  EPROTOTYPE:
        return "Protocol wrong type for socket";
    case  ENOPROTOOPT:
        return "Protocol not available";
    case  EPROTONOSUPPORT:
        return "Protocol not supported";

    case  EOPNOTSUPP:
        return "Operation not supported on transport endpoint";

    case  EAFNOSUPPORT:
        return "Address family not supported by protocol";
    case  EADDRINUSE:
        return "Address already in use";
    case  EADDRNOTAVAIL:
        return "Cannot assign requested address";
    case  ENETDOWN:
        return "Network is down";
    case  ENETUNREACH:
        return "Network is unreachable";
    case  ENETRESET:
        return "Network dropped connection because of reset";
    case  ECONNABORTED:
        return "Software caused connection abort";
    case  ECONNRESET:
        return "Connection reset by peer";
    case  ENOBUFS:
        return "No buffer space available";
    case  EISCONN:
        return "Transport endpoint is already connected";
    case  ENOTCONN:
        return "Transport endpoint is not connected";

    case  ETIMEDOUT:
        return "Connection timed out";
    case  ECONNREFUSED:
        return "Connection refused";

    case  EHOSTUNREACH:
        return "No route to host";
    case  EALREADY:
        return "Operation already in progress";
    case  EINPROGRESS:
        return "Operation now in progress";
#ifndef _WIN32
    case  ENOTBLK:
        return "Block device required";
    case  ECHRNG:
        return "Channel number out of range";
    case  EL2NSYNC:
        return "Level 2 not synchronized";
    case  EL3HLT:
        return "Level 3 halted";
    case  EL3RST:
        return "Level 3 reset";
    case  ELNRNG:
        return "Link number out of range";
    case  EUNATCH:
        return "Protocol driver not attached";
    case  ENOCSI:
        return "No CSI structure available";
    case  EL2HLT:
        return "Level 2 halted";
    case  EBADE:
        return "Invalid ex   ";
    case  EBADR:
        return "Invalid request descriptor";
    case  EXFULL:
        return "Exchange full";
    case  ENOANO:
        return "No anode";
    case  EBADRQC:
        return "Invalid request code";
    case  EBADSLT:
        return "Invalid slot";

    //case  EDEADLOCK:
    //case EDEADLK:

    case  EBFONT:
        return "Bad font file string_format";
    case  ENONET:
        return "Machine is not on the network";
    case  ENOPKG:
        return "Package not installed";
    case  EREMOTE:
        return "Object is remote";

    case  EMULTIHOP:
        return "Multihop attempted";
    case  EDOTDOT:
        return "RFS specific error";
    case  EADV:
        return "Advertise error";
    case  ESRMNT:
        return "Srmount error";
    case  ECOMM:
        return "Communication error on send";
    case  ERESTART:
        return "Interrupted system call should be restarted";
    case  ESTRPIPE:
        return "Streams pipe error";
    case  EUSERS:
        return "Too many users";
    case  ENOTUNIQ:
        return "Email not unique on network";
    case  EBADFD:
        return "File descriptor in bad state";
    case  EREMCHG:
        return "Remote address changed";
    case  ELIBACC:
        return "Can not access a needed shared library";
    case  ELIBBAD:
        return "Accessing a corrupted shared library";
    case  ELIBSCN:
        return ".lib section in a.out corrupted";
    case  ELIBMAX:
        return "Attempting to link in too many shared libraries";
    case  ELIBEXEC:
        return "Cannot exec a shared library directly";
    case  ESOCKTNOSUPPORT:
        return "Socket type not supported";
    case  EPFNOSUPPORT:
        return "Protocol family not supported";
    case  EHOSTDOWN:
        return "Host is down";
    case  ESHUTDOWN:
        return "Cannot send after transport endpoint shutdown";
    case  ETOOMANYREFS:
        return "Too many references: cannot splice";
    case  ESTALE:
        return "Stale NFS file handle";
    case  EUCLEAN:
        return "Structure needs cleaning";
    case  ENOTNAM:
        return "Not a XENIX named type file";
    case  ENAVAIL:
        return "No XENIX semaphores available";
    case  EISNAM:
        return "Is a named type file";
    case  EREMOTEIO:
        return "Remote I/O error";
    case  EDQUOT:
        return "Quota exceeded";
    case  ENOMEDIUM:
        return "No medium found";
    case  EMEDIUMTYPE:
        return "Wrong medium type";
#endif
    default:
        break;

    }

    return "Unknown";
}
#ifdef _WIN32


int windows_error_to_posix(int i)
{
    switch (i)
    {
    case ERROR_ACCESS_DENIED:
        return EACCES;
    case ERROR_ALREADY_EXISTS:
        return EEXIST;
    case ERROR_BAD_UNIT:
        return ENODEV;
    case ERROR_BUFFER_OVERFLOW:
        return ENAMETOOLONG;
    case ERROR_BUSY:
        return EBUSY;
    case ERROR_BUSY_DRIVE:
        return EBUSY;
    case ERROR_CANNOT_MAKE:
        return EACCES;
    case ERROR_CANTOPEN:
        return EIO;
    case ERROR_CANTREAD:
        return EIO;
    case ERROR_CANTWRITE:
        return EIO;
    case ERROR_CURRENT_DIRECTORY:
        return EACCES;
    case ERROR_DEV_NOT_EXIST:
        return ENODEV;
    case ERROR_DEVICE_IN_USE:
        return EBUSY;
    case ERROR_DIR_NOT_EMPTY:
        return ENOTEMPTY;
    case ERROR_DIRECTORY:
        return EINVAL;
    case ERROR_DISK_FULL:
        return ENOSPC;
    case ERROR_FILE_EXISTS:
        return EEXIST;
    case ERROR_FILE_NOT_FOUND:
        return ENOENT;
    case ERROR_HANDLE_DISK_FULL:
        return ENOSPC;
    case ERROR_INVALID_ACCESS:
        return EACCES;
    case ERROR_INVALID_DRIVE:
        return ENODEV;
    case ERROR_INVALID_FUNCTION:
        return ENOSYS;
    case ERROR_INVALID_HANDLE:
        return EINVAL;
    case ERROR_INVALID_NAME:
        return EINVAL;
    case ERROR_LOCK_VIOLATION:
        return ENOLCK;
    case ERROR_LOCKED:
        return ENOLCK;
    case ERROR_NEGATIVE_SEEK:
        return EINVAL;
    case ERROR_NOACCESS:
        return EACCES;
    case ERROR_NOT_ENOUGH_MEMORY:
        return ENOMEM;
    case ERROR_NOT_READY:
        return EAGAIN;
    case ERROR_NOT_SAME_DEVICE:
        return EXDEV;
    case ERROR_OPEN_FAILED:
        return EIO;
    case ERROR_OPEN_FILES:
        return EBUSY;
    case ERROR_OPERATION_ABORTED:
        return ECANCELED;
    case ERROR_OUTOFMEMORY:
        return ENOMEM;
    case ERROR_PATH_NOT_FOUND:
        return ENOENT;
    case ERROR_READ_FAULT:
        return EIO;
    case ERROR_RETRY:
        return EAGAIN;
    case ERROR_SEEK:
        return EIO;
    case ERROR_SHARING_VIOLATION:
        return EACCES;
    case ERROR_TOO_MANY_OPEN_FILES:
        return EMFILE;
    case ERROR_WRITE_FAULT:
        return EIO;
    case ERROR_WRITE_PROTECT:
        return EACCES;
    case WSAEACCES:
        return EACCES;
    case WSAEADDRINUSE:
        return EADDRINUSE;
    case WSAEADDRNOTAVAIL:
        return EADDRNOTAVAIL;
    case WSAEAFNOSUPPORT:
        return EAFNOSUPPORT;
    case WSAEALREADY:
        return EALREADY;
    case WSAEBADF:
        return EBADF;
    case WSAECONNABORTED:
        return ECONNABORTED;
    case WSAECONNREFUSED:
        return ECONNREFUSED;
    case WSAECONNRESET:
        return ECONNRESET;
    case WSAEDESTADDRREQ:
        return EDESTADDRREQ;
    case WSAEFAULT:
        return EFAULT;
    case WSAEHOSTUNREACH:
        return EHOSTUNREACH;
    case WSAEINPROGRESS:
        return EINPROGRESS;
    case WSAEINTR:
        return EINTR;
    case WSAEINVAL:
        return EINVAL;
    case WSAEISCONN:
        return EISCONN;
    case WSAEMFILE:
        return EMFILE;
    case WSAEMSGSIZE:
        return EMSGSIZE;
    case WSAENETDOWN:
        return ENETDOWN;
    case WSAENETRESET:
        return ENETRESET;
    case WSAENETUNREACH:
        return ENETUNREACH;
    case WSAENOBUFS:
        return ENOBUFS;
    case WSAENOPROTOOPT:
        return ENOPROTOOPT;
    case WSAENOTCONN:
        return ENOTCONN;
    case WSAENOTSOCK:
        return ENOTSOCK;
    case WSAEOPNOTSUPP:
        return EOPNOTSUPP;
    case WSAEPROTONOSUPPORT:
        return EPROTONOSUPPORT;
    case WSAEPROTOTYPE:
        return EPROTOTYPE;
    case WSAETIMEDOUT:
        return ETIMEDOUT;
    case WSAEWOULDBLOCK:
        return EWOULDBLOCK;
    default:
        break;
    }
    return EPERM;
}
#endif
/*
int GetWindowsOrLinuxSocketLastErrorAsPosix(void)
{
#ifdef _WIN32
    return windows_error_to_posix(WSAGetLastError());
#else
    return errno;
#endif
}
*/




void ajust_line_and_identation(struct token* token, struct format_visit_ctx* ctx)
{
    /*
    * Before this token we must have a identation and before identation a new line.
    * If we don't have it we need to insert.
    */

    if (token && token->level == 0)
    {
        struct token* previous_token = token->prev;
        if (previous_token)
        {
            if (previous_token->type == TK_BLANKS)
            {
                char blanks[50] = { 0 };
                if (ctx->identation > 0)
                    snprintf(blanks, sizeof blanks, "%*c", (ctx->identation * 4), ' ');

                /*only adjust the number of spaces*/
                free(previous_token->lexeme);
                previous_token->lexeme = strdup(blanks);

                struct token* previous_previous_token =
                    previous_token->prev;

                if (previous_previous_token->type != TK_NEWLINE)
                {
                    struct error error = { 0 };
                    struct token_list list = tokenizer("\n", NULL, 0, TK_FLAG_NONE, &error);
                    token_list_insert_after(&ctx->ast.token_list, previous_previous_token, &list);
                }
            }
            else if (previous_token->type != TK_NEWLINE)
            {
                char blanks[50];
                if (ctx->identation > 0)
                {
                    snprintf(blanks, sizeof blanks, "\n%*c", (ctx->identation * 4), ' ');
                }
                else
                {
                    snprintf(blanks, sizeof blanks, "\n");
                }

                struct error error = { 0 };
                struct token_list list = tokenizer(blanks, NULL, 0, TK_FLAG_NONE, &error);
                token_list_insert_after(&ctx->ast.token_list, previous_token, &list);
            }
        }
    }
}

static void format_visit_unlabeled_statement(struct format_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, struct error* error);

static void format_visit_statement(struct format_visit_ctx* ctx, struct statement* p_statement, struct error* error)
{
    if (p_statement->labeled_statement)
    {
        //format_visit_labeled_statement(ctx, p_statement->labeled_statement, error);
    }
    else if (p_statement->unlabeled_statement)
    {
        format_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement, error);
    }
}



static void format_visit_selection_statement(struct format_visit_ctx* ctx, struct selection_statement* p_selection_statement, struct error* error)
{
    if (p_selection_statement->secondary_block)
    {
        ajust_line_and_identation(p_selection_statement->secondary_block->first, ctx);

        if (p_selection_statement->secondary_block &&
            p_selection_statement->secondary_block->statement &&
            p_selection_statement->secondary_block->statement->unlabeled_statement &&
            p_selection_statement->secondary_block->statement->unlabeled_statement->primary_block &&
            p_selection_statement->secondary_block->statement->unlabeled_statement->primary_block->compound_statement)
        {
            format_visit_statement(ctx, p_selection_statement->secondary_block->statement, error);
        }
        else
        {
            ctx->identation++;
            ajust_line_and_identation(p_selection_statement->secondary_block->first, ctx);

            format_visit_statement(ctx, p_selection_statement->secondary_block->statement, error);
            ctx->identation--;
        }
        //ajust_line_and_identation(p_selection_statement->secondary_block->last, ctx);
    }

    if (p_selection_statement->else_secondary_block_opt)
    {
        ajust_line_and_identation(p_selection_statement->else_token_opt, ctx);
        ajust_line_and_identation(p_selection_statement->else_secondary_block_opt->first, ctx);

        if (p_selection_statement->else_secondary_block_opt->statement &&
            p_selection_statement->else_secondary_block_opt->statement->unlabeled_statement &&
            p_selection_statement->else_secondary_block_opt->statement->unlabeled_statement->primary_block &&
            p_selection_statement->else_secondary_block_opt->statement->unlabeled_statement->primary_block->compound_statement)
        {
            /*compound statement*/
            format_visit_statement(ctx, p_selection_statement->else_secondary_block_opt->statement, error);
        }
        else
        {
            ctx->identation++;
            format_visit_statement(ctx, p_selection_statement->else_secondary_block_opt->statement, error);
            ctx->identation--;
        }
    }

}

static void format_visit_jump_statement(struct format_visit_ctx* ctx, struct jump_statement* p_jump_statement, struct error* error)
{



    if (p_jump_statement->token->type == TK_KEYWORD_THROW ||
        p_jump_statement->token->type == TK_KEYWORD_RETURN ||
        p_jump_statement->token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->token->type == TK_KEYWORD_CONTINUE ||
        p_jump_statement->token->type == TK_KEYWORD_GOTO)
    {
        ajust_line_and_identation(p_jump_statement->token, ctx);
    }
    else
    {
        assert(false);
    }
}

static void format_visit_compound_statement(struct format_visit_ctx* ctx, struct compound_statement* p_compound_statement, struct error* error);


static void format_visit_secondary_block(struct format_visit_ctx* ctx, struct secondary_block* p_secondary_block, struct error* error)
{
    format_visit_statement(ctx, p_secondary_block->statement, error);
}

static void format_visit_iteration_statement(struct format_visit_ctx* ctx, struct iteration_statement* p_iteration_statement, struct error* error)
{

    if (p_iteration_statement->expression1)
    {
        //format_visit_expression(ctx, p_iteration_statement->expression1, error);
    }

    if (p_iteration_statement->expression2)
    {
        //format_visit_expression(ctx, p_iteration_statement->expression2, error);
    }



    if (p_iteration_statement->secondary_block)
    {
        format_visit_secondary_block(ctx, p_iteration_statement->secondary_block, error);
    }
}

static void format_visit_try_statement(struct format_visit_ctx* ctx, struct try_statement* p_try_statement, struct error* error)
{
    if (p_try_statement->secondary_block)
        format_visit_secondary_block(ctx, p_try_statement->secondary_block, error);

    if (p_try_statement->catch_secondary_block_opt)
    {
        format_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt, error);
    }
}


static void format_visit_primary_block(struct format_visit_ctx* ctx, struct primary_block* p_primary_block, struct error* error)
{

        if (p_primary_block->compound_statement)
        {
            format_visit_compound_statement(ctx, p_primary_block->compound_statement, error);
        }
        else if (p_primary_block->iteration_statement)
        {
            format_visit_iteration_statement(ctx, p_primary_block->iteration_statement, error);
        }
        else if (p_primary_block->selection_statement)
        {
            format_visit_selection_statement(ctx, p_primary_block->selection_statement, error);
        }
        else if (p_primary_block->defer_statement)
        {
            //visit_defer_statement(ctx, p_primary_block->defer_statement, error);
        }
        else if (p_primary_block->try_statement)
        {
            format_visit_try_statement(ctx, p_primary_block->try_statement, error);
        }
        else
        {
            assert(false);
        }
}


static void format_visit_expression_statement(struct format_visit_ctx* ctx, struct expression_statement* p_expression_statement, struct error* error)
{
    if (p_expression_statement->expression)
    {
        //ajust_line_and_identation(p_expression_statement->first_token, ctx);
    }
}

static void format_visit_unlabeled_statement(struct format_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, struct error* error)
{
    if (p_unlabeled_statement->primary_block)
    {
        format_visit_primary_block(ctx, p_unlabeled_statement->primary_block, error);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        format_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement, error);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        format_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement, error);
    }
    else
    {
        assert(false);
    }
}

static void format_visit_block_item(struct format_visit_ctx* ctx, struct block_item* p_block_item, struct error* error)
{
    ajust_line_and_identation(p_block_item->first_token, ctx);

    if (p_block_item->declaration)
    {
        //visit_declaration(ctx, p_block_item->declaration, error);
    }
    else if (p_block_item->unlabeled_statement)
    {
        format_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement, error);
    }
    else if (p_block_item->labeled_statement)
    {
        //visit_labeled_statement(ctx, p_block_item->labeled_statement, error);
    }
}

static void format_visit_block_item_list(struct format_visit_ctx* ctx, struct block_item_list* p_block_item_list, struct error* error)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (error->code == 0 && p_block_item)
    {
        format_visit_block_item(ctx, p_block_item, error);
        p_block_item = p_block_item->next;
    }
}

static void format_visit_compound_statement(struct format_visit_ctx* ctx, struct compound_statement* p_compound_statement, struct error* error)
{
    ajust_line_and_identation(p_compound_statement->first_token, ctx);

    ctx->identation++;
    format_visit_block_item_list(ctx, &p_compound_statement->block_item_list, error);

    ctx->identation--;

    ajust_line_and_identation(p_compound_statement->last_token, ctx);
}

static void format_visit_declaration(struct format_visit_ctx* ctx, struct declaration* p_declaration, struct error* error)
{
    if (p_declaration->static_assert_declaration)
    {
        //format_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration, error);
    }

    if (p_declaration->declaration_specifiers)
    {
        //format_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, error);

    }

    if (p_declaration->init_declarator_list.head)
    {
        //format_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list, error);
    }

    if (p_declaration->function_body)
    {
        format_visit_compound_statement(ctx, p_declaration->function_body, error);
    }
}

void format_visit(struct format_visit_ctx* ctx, struct error* error)
{
    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (error->code == 0 && p_declaration)
    {
        format_visit_declaration(ctx, p_declaration, error);
        p_declaration = p_declaration->next;
    }
}

