


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



bool enable_vt_mode(void);

//#define DISABLE_COLORS 1

#ifdef DISABLE_COLORS

#define ESC ""
#define CSI ""

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

#define ESC "\x1b"
#define CSI "\x1b["

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



//#pragma once



#ifdef __CAKE__

#define _destroy [[destroy]]
#define _delete [[destroy,free]]
#define _free [[free]]

#else

#define _destroy 
#define _delete
#define _free

#define _del_attr(a, b)
#define _add_attr(a, b)

#endif


struct _destroy osstream
{
    char* c_str;
    int size;
    int capacity;
};


void ss_close(_destroy struct osstream* stream);
int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args);
int ss_fprintf(struct osstream* stream, const char* fmt, ...);
int ss_putc(char ch, struct osstream* stream);
void ss_clear(struct osstream* stream);
void ss_swap(struct osstream* a, struct osstream* b);



//#pragma once


//#pragma once

/*
* tag allow more than one type of object be inserted at the same map
*/
enum tag
{
    TAG_TYPE_NONE,
    
    TAG_TYPE_ENUN_SPECIFIER,
    TAG_TYPE_STRUCT_OR_UNION_SPECIFIER,

    TAG_TYPE_ENUMERATOR,
    TAG_TYPE_DECLARATOR,
};


struct map_entry {
    struct map_entry* next;
    unsigned int hash;
    char* key;
    void* p;
    enum tag type; /*type of the object pointed by p*/
};

struct hash_map {
    struct map_entry** table;
    int capacity;
    int  size;
};

void hashmap_remove_all(struct hash_map* map);
void hashmap_destroy(struct hash_map* map);
struct map_entry* hashmap_find(struct hash_map* map, const char* key);
void* hashmap_remove(struct hash_map* map, const char* key, enum tag* p_type_opt);
int hashmap_set(struct hash_map* map, const char* key, void* p, enum tag type);



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
    TK_PRAGMA,
    TK_STRING_LITERAL,
    TK_CHAR_CONSTANT,
    TK_LINE_COMMENT,
    TK_COMMENT,
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

    /*compile time functions*/
    TK_KEYWORD_HASHOF, /*extension*/
    TK_KEYWORD_ATTR_ADD, /*extension*/
    TK_KEYWORD_ATTR_REMOVE, /*extension*/
    TK_KEYWORD_ATTR_HAS, /*extension*/
    /*https://en.cppreference.com/w/cpp/header/type_traits*/
    TK_KEYWORD_IS_POINTER,
    TK_KEYWORD_IS_CONST,
    TK_KEYWORD_IS_ARRAY,
    TK_KEYWORD_IS_FUNCTION,
    TK_KEYWORD_IS_SCALAR,
    TK_KEYWORD_IS_ARITHMETIC,
    TK_KEYWORD_IS_FLOATING_POINT,
    TK_KEYWORD_IS_INTEGRAL,
    TK_KEYWORD_IS_SAME,

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
    TK_FLAG_IDENTIFIER_IS_NOT_ENUMERATOR = 1 << 8, /*indica que eh identificador enumerator separar?*/    
    TK_FLAG_SLICED = 1 << 9, /*line-slicing in the middle*/
    TK_FLAG_LINE_CONTINUATION = 1 << 10 /*token has one or more line-slicing*/
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

    /*points to the token with file name or macro*/
    struct token* token_origin;

    struct token* next;
    struct token* prev;
};

void token_delete(struct token* p);

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

void print_tokens_html(struct token* p_token);
void print_line_and_token(int (*printf)(const char* fmt, ...), const struct token* p_token);
void print_position(int (*printf)(const char* fmt, ...), const char* path, int line, int col);
struct stream
{
    const char* source;
    const char* current;
    int line;
    int col;
    int line_continuation_count;
    const char* path; /*non owner*/
};

int is_digit(struct stream* p);
int is_nondigit(struct stream* p);
void stream_match(struct stream* stream);

#define LIST_ADD(list, pnew)\
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

#define try  
#define catch if (0) catch_label:
#define throw goto catch_label

const char* get_posix_error_message(int error);
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

enum compiler_warning {    
    W_UNUSED_VARIABLE = 1 << 1, //-Wunused-variable
    W_DEPRECATED = 1 << 2,
    W_ENUN_COMPARE = 1 << 3 ,//-Wenum-compare
    W_NON_NULL = 1 << 4, //-Wnonnull
    W_ADDRESS = 1 << 5, //-Waddress (always true)
    W_UNUSED_PARAMETER = 1 << 6, //-Wno-unused-parameter
    W_DECLARATOR_HIDE = 1 << 7, // gcc no
    W_TYPEOF_ARRAY_PARAMETER = 1 << 8,//
    W_ATTRIBUTES = 1 << 9, //-Wattributes
    W_UNUSED_VALUE = 1 << 10, //-Wunused-value
};
const char* get_warning_name(enum compiler_warning w);
enum compiler_warning  get_warning_flag(const char* wname, bool* p_disable_warning);

struct options
{
    /*
       -std=c99
    */
    enum language_version input;

    /*
       -target=c99
    */
    enum language_version target;
    
    /*
      #pragma CAKE diagnostic push
      #pragma CAKE diagnostic pop
    */
    int enabled_warnings_stack_top_index;
    enum compiler_warning enabled_warnings_stack[10];

    /*
       -remove-comments  
    */
    bool remove_comments;    

    /*
      -E 
    */
    bool preprocess_only;

    /*
      -rm
      -direct-compilation
    */
    bool direct_compilation;

    bool format_input;
    bool format_ouput;
    
    /*
      -no-discard 
    */
    bool nodiscard_is_default;


    bool do_static_analisys;

    /*
      -no-output
      if true cake does not generate ouput
    */
    bool no_output;
    
    /*
      -n
      true - to info about name conventions violations
    */
    bool check_naming_conventions;

    /*
      -o filename
      defines the ouputfile when 1 file is used
    */
    char output[200];
};

int fill_options(struct options* options,
                 int argc,
                 const char** argv);

void print_help();



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
    struct hash_map pragma_once_map;
    
    struct token* current;
    struct token_list input_list;

    bool conditional_inclusion;
    int n_warnings;
    int n_errors;
    int (*printf)(const char* fmt, ...);
};
void preprocessor_ctx_destroy(struct preprocessor_ctx* p);

void preprocessor_set_info_with_token(struct preprocessor_ctx* ctx, const struct token* p_token, const char* fmt, ...);
void preprocessor_set_warning_with_token(struct preprocessor_ctx* ctx, const struct token* p_token, const char* fmt, ...);
void preprocessor_set_error_with_token(struct preprocessor_ctx* ctx, const struct token* p_token, const char* fmt, ...);


struct tokenizer_ctx
{
    struct options options;
    int n_warnings;
    int n_errors;
    int (*printf)(const char* fmt, ...);
};

struct token_list tokenizer(struct tokenizer_ctx* ctx, const char* text, const char* filename_opt, int level, enum token_flags addflags);
void add_standard_macros(struct preprocessor_ctx* ctx);
struct include_dir* include_dir_add(struct include_dir_list* list, const char* path);

struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level);
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
void token_list_insert_after(struct token_list* list, struct token* after, struct token_list* append);
struct token_list tokenizer(struct tokenizer_ctx* p, const char* text, const char* filename_opt, int level, enum token_flags addflags);

const char* get_code_as_we_see(struct token_list* list, bool remove_comments);
const char* get_code_as_compiler_see(struct token_list* list);

const char* get_code_as_we_see_plusmacros(struct token_list* list);
const char* get_code_as_we_see(struct token_list* list, bool remove_comments);

void print_tokens(struct token* p_token);
void print_preprocessed(struct token* p_token);
const char* print_preprocessed_to_string(struct token* p_token);
const char* print_preprocessed_to_string2(struct token* p_token);
void check_unused_macros(struct hash_map* map);

char* readfile(const char* path);
const char* get_token_name(enum token_type tk);
void print_all_macros(struct preprocessor_ctx* prectx);

int string_literal_byte_size(const char* s);
int get_char_type(const char* s);

#ifdef _WIN32


#include <Windows.h>
#endif

#if defined _MSC_VER && !defined __POCC__


#include <crtdbg.h>
#endif

#if defined _MSC_VER && !defined __POCC__


#include <debugapi.h>
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
        p->token_origin = filetoken;
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

    const char* cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return (char*)cstr;
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
    //assert(pnew->token_origin);

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
        p->type == TK_COMMENT;
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
        *token = *p;
        token->lexeme = strdup(p->lexeme);
        token->next = NULL;
        token->prev = NULL;
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


void print_token_html(struct token* p_token)
{
    printf("<span class=\"");


    if (!(p_token->flags & TK_FLAG_FINAL))
    {
        printf("notfinal ");
    }

    if (p_token->flags & TK_FLAG_FINAL)
    {
        printf("final ");
    }
    if (p_token->flags & TK_FLAG_HIDE)
    {
        printf("hide ");
    }
    if (p_token->flags & TK_FLAG_MACRO_EXPANDED)
    {
        printf("expanded ");
    }
    if (p_token->flags & TK_FLAG_HAS_SPACE_BEFORE)
    {
        printf("space ");
    }
    if (p_token->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
    {
        printf("newline ");
    }

    printf("\">");

    print_literal2(p_token->lexeme);

    printf("</span>");

    if (p_token->type == TK_NEWLINE || p_token->type == TK_BEGIN_OF_FILE)
    {
        printf("<br>\n");
    }
}

/*
 CSS for html ouput

 <style>
        .final {
          color:blue;
        }

        .notfinal {
          color:gray;
        }

        .hide {
          text-decoration: line-through;
          color:red;
        }

        .expanded {
           background-color:yellow;
        }

        span {
            border-style: solid;
            border-color: gray;
            border-width: 1px 1px;
            padding:1px;
            margin:2px;
        }

</style>

*/
void print_tokens_html(struct token* p_token)
{
    printf("<pre>\n");
    struct token* current = p_token;
    while (current)
    {
        print_token_html(current);
        current = current->next;
    }
    printf("\n</pre>");
}

void print_position(int (*printf)(const char* fmt, ...), const char* path, int line, int col)
{
    printf(WHITE "%s:%d:%d: ", path ? path : "<>", line, col);
}

void print_line_and_token(int (*printf)(const char* fmt, ...), const struct token* p_token)
{
    if (p_token == NULL)
        return;

    int line = p_token->line;
    printf(LIGHTGRAY);

    char nbuffer[20] = { 0 };
    int n = snprintf(nbuffer, sizeof nbuffer, "%d", line);
    printf(" %s |", nbuffer);

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
            if (next->flags & TK_FLAG_HAS_SPACE_BEFORE)
            {
                printf(" ");
            }
        }
        if (next->flags & TK_FLAG_MACRO_EXPANDED) {
            printf(DARKGRAY "%s" RESET, next->lexeme);
        }
        else
            printf("%s", next->lexeme);

        next = next->next;
    }
    printf("\n");
    printf(LIGHTGRAY);
    printf(" %*s |", n, " ");
    if (p_token)
    {
        for (int i = 1; i <= (p_token->col - 1); i++) {
            printf(" ");
        }
    }

    printf(LIGHTGREEN);

    printf("^");

    char* p = p_token->lexeme + 1;
    while (p && *p)
    {
        printf("~");
        p++;
    }

    printf(RESET);
    printf("\n");
}





//#pragma once

unsigned int string_hash(const char* key);


unsigned int string_hash(const char* key)
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

#if defined _MSC_VER && !defined __POCC__
#endif


void hashmap_remove_all(struct hash_map* map)
{
    if (map->table != NULL)
    {
        for (int i = 0; i < map->capacity; i++)
        {
            struct map_entry* pentry = map->table[i];

            while (pentry != NULL)
            {
                struct map_entry* p_entry_current = pentry;
                pentry = pentry->next;
                free(p_entry_current->key);
                free(p_entry_current);
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

struct map_entry* hashmap_find(struct hash_map* map, const char* key)
{
    if (map->table == NULL)
      return NULL;

    const unsigned int hash = string_hash(key);
    const int index = hash % map->capacity;

    struct map_entry* pentry = map->table[index];

    for (; pentry != NULL; pentry = pentry->next)
    {
        if (pentry->hash == hash && strcmp(pentry->key, key) == 0) {
            return pentry;
        }
    }

    return NULL;
}


void * hashmap_remove(struct hash_map* map, const char* key, enum tag * p_type_opt)
{
    if (map->table != NULL)
    {
        const unsigned int hash = string_hash(key);
        struct map_entry** preventry = &map->table[hash % map->capacity];
        struct map_entry* pentry = *preventry;

        for (; pentry != NULL; pentry = pentry->next)
        {
            if ((pentry->hash == hash) && (strcmp(pentry->key, key) == 0))
            {
                *preventry = pentry->next;

                if (p_type_opt)
                    *p_type_opt = pentry->type;

                void* p = pentry->p;
                free(pentry->key);
                free(pentry);
                
                return p;
            }
            preventry = &pentry->next;
        }
    }

    return NULL;
}


int hashmap_set(struct hash_map* map, const char* key, void* p, enum tag type)
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
        unsigned int hash = string_hash(key);
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
            pentry->p = p;
            pentry->type = type;
            pentry->key = strdup(key);
            pentry->next = map->table[index];
            map->table[index] = pentry;
            map->size++;
            result = 0;
        }
        else
        {
            result = 1;
            pentry->p = p;
            pentry->type = type;
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
//missing in mingw (installed with codeblocs)
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING  
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif

    DWORD mode = 0;
    HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
    if ((h_out = GetStdHandle(STD_OUTPUT_HANDLE)) != INVALID_HANDLE_VALUE &&
            GetConsoleMode(h_out, &mode) != 0 &&
            SetConsoleMode(h_out, mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0 &&
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

#ifdef __POCC__
/*missing in pelles c*/
typedef unsigned short ino_t; // inode number (unused on Windows)
typedef long off_t; // file offset value
#endif

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



char* realpath(const char* restrict path, char* restrict resolved_path);

int get_self_path(char* buffer, int maxsize);

char* readfile(const char* path);
char* dirname(char* path);

const char* get_posix_error_message(int error);






//#pragma once

struct preprocessor_ctx;

int pre_constant_expression(struct preprocessor_ctx* ctx, long long* pvalue);



#ifdef _WIN32
#endif

#if defined _MSC_VER && !defined __POCC__
#endif



static int printf_nothing(char const* const format, ...) { return 0; }


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

struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level);

static void tokenizer_set_error(struct tokenizer_ctx* ctx, struct stream* stream, const char* fmt, ...)
{
    ctx->n_errors++;
    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    print_position(ctx->printf, stream->path, stream->line, stream->col);
    ctx->printf(LIGHTRED "error: " WHITE "%s\n", buffer);
}


static void tokenizer_set_warning(struct tokenizer_ctx* ctx, struct stream* stream, const char* fmt, ...)
{
    ctx->n_warnings++;    
    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    print_position(ctx->printf, stream->path, stream->line, stream->col);
    ctx->printf(LIGHTMAGENTA "warning: " WHITE "%s\n", buffer);    
}

void preprocessor_set_info_with_token(struct preprocessor_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    

    if (p_token)
      print_position(ctx->printf, p_token->token_origin->lexeme, p_token->line, p_token->col);

    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    ctx->printf(LIGHTCYAN "note: " WHITE "%s\n", buffer);

    print_line_and_token(ctx->printf, p_token);
}

void preprocessor_set_warning_with_token(struct preprocessor_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    ctx->n_warnings++;
    if (p_token)
      print_position(ctx->printf, p_token->token_origin->lexeme, p_token->line, p_token->col);

    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    ctx->printf(LIGHTMAGENTA "warning: " WHITE "%s\n", buffer);

    print_line_and_token(ctx->printf, p_token);
}

void preprocessor_set_error_with_token(struct preprocessor_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    ctx->n_errors++;

    if (p_token)
      print_position(ctx->printf, p_token->token_origin->lexeme, p_token->line, p_token->col);

    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    ctx->printf(LIGHTRED"error: " WHITE "%s\n", buffer);

    print_line_and_token(ctx->printf, p_token);
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
    struct macro_expanded* p_previous;
};

void add_macro(struct preprocessor_ctx* ctx, const char* name)
{
    struct macro* macro = calloc(1, sizeof * macro);
    if (macro == NULL)
    {
    }
    macro->name = strdup(name);
    hashmap_set(&ctx->macros, name, macro, 0);
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


struct token_list copy_argument_list(struct macro_argument* p_macro_argument)
{
    struct token_list list = copy_argument_list_tokens(&p_macro_argument->tokens);
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

void delete_macro(struct macro* macro)
{
    if (macro)
    {
        free(macro);
    }
}

struct macro* find_macro(struct preprocessor_ctx* ctx, const char* name)
{
    struct map_entry* p_entry = hashmap_find(&ctx->macros, name);
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

struct token* string_literal(struct tokenizer_ctx* ctx, struct stream* stream)
{
    struct token* p_new_token = NULL;
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

    struct stream stream;
    stream.source = s;
    stream.current = s;
    stream.line = 1;
    stream.col = 1;


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

struct token_list embed_tokenizer(struct preprocessor_ctx* ctx, const char* filename_opt, int level, enum token_flags addflags)
{
    struct token_list list = { 0 };

    FILE* file = NULL;

    bool b_first = true;
    int line = 1;
    int col = 1;
    int count = 0;
    try
    {
#ifndef MOCKFILES
        file = fopen(filename_opt, "rb");
        if (file == NULL)
        {
            preprocessor_set_error_with_token(ctx, ctx->current, "file '%s' not found", filename_opt);
            throw;
        }
#else
        /*web versions only text files that are included*/
        const char* textfile = readfile(filename_opt);
        if (textfile == NULL)
        {
            preprocessor_set_error_with_token(ctx, ctx->current, "file '%s' not found", filename_opt);
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
            if (b_first)
            {
                b_first = false;
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
                    struct token* p_new3 = new_token(newline, &newline[1], TK_NEWLINE);
                    p_new3->level = level;
                    p_new3->token_origin = NULL;
                    p_new3->line = line;
                    p_new3->col = col;
                    token_list_add(&list, p_new3);
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

static bool set_sliced_flag(struct stream* stream, struct token* p_new_token)
{
    if (stream->line_continuation_count > 0) {
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
        else {
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
            const char* bof = "";
            p_first = new_token(bof, bof + 1, TK_BEGIN_OF_FILE);
            p_first->level = level;
            p_first->lexeme = strdup(filename_opt);
            token_list_add(&list, p_first);
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
                struct token* p_new_token = ppnumber(&stream);
                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                new_line = false;
                has_space = false;

                set_sliced_flag(&stream, p_new_token);
                continue;
            }

            /*
             Tem que vir antes identifier
            */
            if (first_of_string_literal(&stream))
            {
                struct token* p_new_token = string_literal(ctx, &stream);
                if (p_new_token == NULL) throw;

                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);;
                new_line = false;
                has_space = false;
                set_sliced_flag(&stream, p_new_token);
                continue;
            }

            if (first_of_character_constant(&stream))
            {
                //TODO if we have ' in the middle then it is not character constant
                struct token* p_new_token = character_constant(&stream);
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
                set_sliced_flag(&stream, p_new_token);
                continue;
            }

            if (is_nondigit(&stream))
            {
                struct token* p_new_token = identifier(&stream);
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
                if (set_sliced_flag(&stream, p_new_token))
                {
                    tokenizer_set_warning(ctx, &stream, "token sliced");
                }
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
                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                /*bNewLine = false;*/ //deixa assim
                has_space = true;
                set_sliced_flag(&stream, p_new_token);
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
                struct token* p_new_token = new_token(start, stream.current, TK_LINE_COMMENT);
                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                new_line = true;
                has_space = false;
                set_sliced_flag(&stream, p_new_token);

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
                struct token* p_new_token = new_token(start, stream.current, TK_COMMENT);
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
                struct token* p_new_token = new_token(start, stream.current, '#');
                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                p_new_token->type = TK_PREPROCESSOR_LINE;
                token_list_add(&list, p_new_token);
                new_line = false;
                has_space = false;
                set_sliced_flag(&stream, p_new_token);
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
                p_new_token->flags |= has_space ? TK_FLAG_HAS_SPACE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= new_line ? TK_FLAG_HAS_NEWLINE_BEFORE : TK_FLAG_NONE;
                p_new_token->flags |= addflags;

                p_new_token->level = level;
                p_new_token->token_origin = p_first;
                p_new_token->line = line;
                p_new_token->col = col;
                token_list_add(&list, p_new_token);
                new_line = true;
                has_space = false;
                set_sliced_flag(&stream, p_new_token);
                continue;
            }
            const char* start = stream.current;
            enum token_type t = is_punctuator(&stream);
            if (t != TK_NONE)
            {

                struct token* p_new_token = new_token(start, stream.current, t);
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
                set_sliced_flag(&stream, p_new_token);
                continue;
            }
            else
            {
                stream_match(&stream);
                struct token* p_new_token = new_token(start, stream.current, ANY_OTHER_PP_TOKEN);
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


                set_sliced_flag(&stream, p_new_token);
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
            token_list_add(dest, token_list_pop_front(input_list));
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

                if (has_parentesis)
                {
                    if (input_list->head->type != ')')
                    {
                        preprocessor_set_error_with_token(ctx, input_list->head, "missing )");
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



                bool already_included = false;
                const char* s = find_and_read_include_file(ctx, path, fullpath, &already_included);

                bool has_include = s != NULL;
                free((void*)s);

                struct token* p_new_token = calloc(1, sizeof * p_new_token);
                p_new_token->type = TK_PPNUMBER;
                free(p_new_token->lexeme);
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

                struct token* p_new_token = calloc(1, sizeof * p_new_token);
                p_new_token->type = TK_PPNUMBER;
                free(p_new_token->lexeme);
                p_new_token->lexeme = strdup(has_c_attribute ? "1" : "0");
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
    int level
)
{
    struct token* first = input_list->head;

    ctx->conditional_inclusion = true;
    struct token_list r = { 0 };
    while (input_list->head && input_list->head->type != TK_NEWLINE)
    {
        token_list_add(&r, token_list_pop_front(input_list));

        /*
          We call preprocessor that emmit warnings if line continuation
          is used outside macro directives.
          Let's remove TK_FLAG_LINE_CONTINUATION from the original token
          to avoid warning inside constant expressions
        */
        r.tail->flags &= ~TK_FLAG_LINE_CONTINUATION;
    }
    *output_list = r;


    struct token_list list1 = copy_replacement_list(&r);
    //printf("\n");
    //print_list(&list1);
    //printf("\n");



    int flags = ctx->flags;
    ctx->flags |= PREPROCESSOR_CTX_FLAGS_ONLY_FINAL;

    /*defined X  por exemplo é mantido sem ser expandido*/

    struct token_list list2 = preprocessor(ctx, &list1, 1);
    ctx->flags = flags;
    //printf("apos preprocess\n");
    //print_list(&list2);
    //printf("\n");

    /*aonde defined has_c_aatribute sao transformados em constantes*/
    struct token_list list3 = process_defined(ctx, &list2);

    //printf("apos remove defined\n");
    //print_list(&list3);
    //printf("\n");

    struct token_list list4 = process_identifiers(ctx, &list3);

    //printf("apos remover identificadores restantes\n");
    //print_list(&list4);
    //printf("\n");

    assert(list4.head != NULL);

    struct preprocessor_ctx pre_ctx = { 0 };
#ifdef TEST
    pre_ctx.printf = printf_nothing;
#else
    pre_ctx.printf = printf;
#endif

    //struct parser_ctx parser_ctx = { 0 };
    pre_ctx.input_list = list4;
    pre_ctx.current = list4.head;
    //pre_skip_blanks(&parser_ctx);

    long long value = 0;
    if (pre_constant_expression(&pre_ctx, &value) != 0)
    {
        preprocessor_set_error_with_token(ctx, first, "expression error");
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
                    preprocessor_set_error_with_token(ctx, input_list->head, "expected token %s got %s\n", get_token_name(type), get_token_name(input_list->head->type));
                else
                    preprocessor_set_error_with_token(ctx, dest->tail, "expected EOF \n");

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
            }
            else
            {
                struct token_list r0 = ignore_preprocessor_line(input_list);
                token_list_append_list(&r, &r0);
            }
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
        }
        else
        {

            preprocessor_set_error_with_token(ctx, input_list->head, "unexpected");
            throw;
        }
        struct token_list r2 = group_opt(ctx, input_list, is_active && *p_result, level);
        token_list_append_list(&r, &r2);
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


        }
        else
        {
            struct token_list r0 = ignore_preprocessor_line(input_list);
            token_list_append_list(&r, &r0);
        }
    }
    else if (strcmp(input_list->head->lexeme, "elifdef") == 0)
    {
        match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
        skip_blanks(ctx, &r, input_list);

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
    }
    /*
       Se algum dos elifs foi true retorna true
    */
    *pelif_result = already_found_elif_true;
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
        if (ctx->n_errors > 0) throw;

        token_list_append_list(&r, &r2);
        bool elif_result = false;
        if (input_list->head->type == TK_PREPROCESSOR_LINE && (
            preprocessor_token_ahead_is_identifier(input_list->head, "elif") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "elifdef") ||
            preprocessor_token_ahead_is_identifier(input_list->head, "elifndef")))
        {
            struct token_list r3 = elif_groups(ctx, input_list, is_active && !if_result, level, &elif_result);
            token_list_append_list(&r, &r3);
        }
        if (input_list->head->type == TK_PREPROCESSOR_LINE &&
            preprocessor_token_ahead_is_identifier(input_list->head, "else"))
        {
            struct token_list r4 = else_group(ctx, input_list, is_active && !if_result && !elif_result, level);
            token_list_append_list(&r, &r4);
        }

        if (ctx->n_errors > 0) throw;

        struct token_list r5 = endif_line(ctx, input_list, level);
        token_list_append_list(&r, &r5);
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
    struct macro_parameter* p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
    p_macro_parameter->name = strdup(input_list->head->lexeme);
    macro->parameters = p_macro_parameter;
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
        p_macro_parameter->next = calloc(1, sizeof * p_macro_parameter);
        p_macro_parameter = p_macro_parameter->next;
        p_macro_parameter->name = strdup(input_list->head->lexeme);
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
            //se nao esta ativo eh ingorado
            struct token_list r7 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r7);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
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

            char fullpath[300] = { 0 };


            path[strlen(path) - 1] = '\0';

            bool already_included = false;
            const char* content = find_and_read_include_file(ctx, path + 1, fullpath, &already_included);
            if (content != NULL)
            {
                struct tokenizer_ctx tctx = { 0 };
#ifdef TEST
                tctx.printf = printf_nothing;
#else
                tctx.printf = printf;
#endif

                struct token_list list = tokenizer(&tctx, content, fullpath, level + 1, TK_FLAG_NONE);
                free((void*)content);

                struct token_list list2 = preprocessor(ctx, &list, level + 1);
                token_list_append_list(&r, &list2);
            }
            else
            {
                if (!already_included)
                {
                    preprocessor_set_error_with_token(ctx, r.tail, "file %s not found", path + 1);

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
                preprocessor_set_error_with_token(ctx, ctx->current, "out of mem");
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


            if (hashmap_find(&ctx->macros, input_list->head->lexeme) != NULL)
            {
                //printf("warning: '%s' macro redefined at %s %d\n",
                  //     input_list->head->lexeme,
                    ///   input_list->head->token_origin->lexeme,
                      // input_list->head->line);
            }


            hashmap_set(&ctx->macros, input_list->head->lexeme, macro, 0);
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
                    struct macro_parameter* p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
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
                    skip_blanks_level(ctx, &r, input_list, level);
                    if (input_list->head->type == '...')
                    {
                        struct macro_parameter* p_macro_parameter = calloc(1, sizeof * p_macro_parameter);
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
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);

            if (macro_name_token)
                naming_convention_macro(ctx, macro_name_token);
        }
        else if (strcmp(input_list->head->lexeme, "undef") == 0)
        {
            /*
             # undef identifier new-line
            */
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//undef
            skip_blanks_level(ctx, &r, input_list, level);

            struct macro* macro = hashmap_remove(&ctx->macros, input_list->head->lexeme, NULL);
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
            preprocessor_set_error_with_token(ctx, input_list->head, "#error");
            token_list_append_list(&r, &r6);
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
            preprocessor_set_warning_with_token(ctx, input_list->head, "#warning");
            token_list_append_list(&r, &r6);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
        }
        else if (strcmp(input_list->head->lexeme, "pragma") == 0)
        {
            /*
              # pragma pp-tokensopt new-line
            */            
            match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma
            r.tail->type = TK_PRAGMA;
            skip_blanks_level(ctx, &r, input_list, level);

            if (input_list->head->type == TK_IDENTIFIER)
            {
                if (strcmp(input_list->head->lexeme, "CAKE") == 0 ||
                    strcmp(input_list->head->lexeme, "GCC") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);
                    skip_blanks_level(ctx, &r, input_list, level);
                }

                if (strcmp(input_list->head->lexeme, "once") == 0)
                {
                    hashmap_set(&ctx->pragma_once_map, input_list->head->token_origin->lexeme, (void*)1, 0);
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma
                }
                else if (strcmp(input_list->head->lexeme, "expand") == 0)
                {
                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma
                    skip_blanks_level(ctx, &r, input_list, level);

                    struct macro* macro = find_macro(ctx, input_list->head->lexeme);
                    if (macro)
                    {
                        macro->expand = true;
                    }

                    match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//pragma

                }
                else if (strcmp(input_list->head->lexeme, "diagnostic") == 0)
                {
                   // match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//diagnostic
                   // skip_blanks_level(ctx, &r, input_list, level);
                   // if (strcmp(ctx->current->lexeme )
                   // match_token_level(&r, input_list, TK_IDENTIFIER, level, ctx);//warning
                   // skip_blanks_level(ctx, &r, input_list, level);
                   // match_token_level(&r, input_list, TK_STRING_LITERAL, level, ctx);//
                }                
            }

            struct token_list r7 = pp_tokens_opt(ctx, input_list, level);
            token_list_append_list(&r, &r7);
            match_token_level(&r, input_list, TK_NEWLINE, level, ctx);
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
                struct macro_argument* p_argument = calloc(1, sizeof(struct macro_argument));
                p_argument->name = strdup(p_current_parameter->name);
                argument_list_add(&macro_argument_list, p_argument);
            }
            match_token_level(&macro_argument_list.tokens, input_list, ')', level, ctx);
            return macro_argument_list;
        }
        struct macro_argument* p_argument = calloc(1, sizeof(struct macro_argument));
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
                    p_current_parameter = p_current_parameter->next;

                    if (p_current_parameter != NULL)
                    {
                        if (strcmp(p_current_parameter->name, "__VA_ARGS__") == 0)
                        {
                            //adicionamos este argumento como sendo vazio
                            p_argument = calloc(1, sizeof(struct macro_argument));
                            p_argument->name = strdup(p_current_parameter->name);
                            argument_list_add(&macro_argument_list, p_argument);
                        }
                        else
                        {
                            preprocessor_set_error_with_token(ctx, macro_name_token, "too few arguments provided to function-like macro invocation\n");
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
                    p_argument = NULL; /*tem mais?*/
                    p_argument = calloc(1, sizeof(struct macro_argument));
                    p_current_parameter = p_current_parameter->next;
                    if (p_current_parameter == NULL)
                    {
                        preprocessor_set_error_with_token(ctx, macro_name_token, "invalid args");
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
    }
    catch
    {
    }

    return macro_argument_list;
}

struct token_list expand_macro(struct preprocessor_ctx* ctx, struct macro_expanded* p_list, struct macro* macro, struct macro_argument_list* arguments, int level);
struct token_list replacement_list_reexamination(struct preprocessor_ctx* ctx, struct macro_expanded* p_list, struct token_list* oldlist, int level);


struct token_list macro_copy_replacement_list(struct preprocessor_ctx* ctx, struct macro* macro);

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
                preprocessor_set_error_with_token(ctx, input_list->head, "missing macro argument (should be checked before)");
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
#ifdef TEST
            tctx.printf = printf_nothing;
#else
            tctx.printf = printf;
#endif

            struct token_list newlist = tokenizer(&tctx, ss.c_str, NULL, level, TK_FLAG_NONE);

            if (newlist.head)
            {
                newlist.head->flags = r.tail->flags;
            }
            else
            {
                struct token* p_new_token = calloc(1, sizeof * p_new_token);
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
    struct macro_argument_list* p_macro_argument_list)
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

        struct token_list r4 = replacement_list_reexamination(ctx, p_list, &argumentlist, 0);
        const bool results_in_empty_substituition = (r4.head == NULL || r4.head->type == TK_PLACEMARKER);
        token_list_destroy(&r4);

        return results_in_empty_substituition;
    }

    return false;
}

static struct token_list replace_macro_arguments(struct preprocessor_ctx* ctx, struct macro_expanded* p_list, struct token_list* input_list, struct macro_argument_list* arguments)
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
                        has_argument_list_empty_substitution(ctx, p_list, arguments);

                    if (discard_va_opt)
                    {
                        //discard all tokens __VA_OPT__(...)
                        while (input_list->head)
                        {
                            if (input_list->head->type == '(') parenteses_count++;
                            else if (input_list->head->type == ')') parenteses_count--;
                            token_delete(token_list_pop_front(input_list));
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
                    char* s = token_list_join_tokens(&argumentlist, true);
                    if (s == NULL)
                    {
                        preprocessor_set_error_with_token(ctx, input_list->head, "unexpected");
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
                    struct token_list argumentlist = copy_argument_list(p_argument);
                    token_list_append_list(&r, &argumentlist);
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
                    struct token_list r4 = replacement_list_reexamination(ctx, p_list, &argumentlist, 0);
                    if (ctx->n_errors > 0) throw;
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

struct token_list replacement_list_reexamination(struct preprocessor_ctx* ctx, struct macro_expanded* p_list, struct token_list* oldlist, int level)
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
                if (ctx->n_errors > 0) throw;


                struct token_list r3 = expand_macro(ctx, p_list, macro, &arguments, level);
                if (ctx->n_errors > 0) throw;

                if (r3.head)
                {
                    r3.head->flags = flags;
                }
                token_list_append_list_at_beginning(&new_list, &r3);
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



struct token_list macro_copy_replacement_list(struct preprocessor_ctx* ctx, struct macro* macro)
{
    /*macros de conteudo dinamico*/
    if (strcmp(macro->name, "__LINE__") == 0)
    {
        struct tokenizer_ctx tctx = { 0 };
#ifdef TEST
        tctx.printf = printf_nothing;
#else
        tctx.printf = printf;
#endif
        struct token_list r = tokenizer(&tctx, "1", "", 0, TK_FLAG_NONE);
        token_list_pop_front(&r);
        r.head->flags = 0;
        return r;
    }
    else if (strcmp(macro->name, "__FILE__") == 0)
    {
        struct tokenizer_ctx tctx = { 0 };
#ifdef TEST
        tctx.printf = printf_nothing;
#else
        tctx.printf = printf;
#endif

        struct token_list r = tokenizer(&tctx, "\"file\"", "", 0, TK_FLAG_NONE);
        token_list_pop_front(&r);
        r.head->flags = 0;
        return r;
    }
    else if (strcmp(macro->name, "__COUNT__") == 0)
    {
        assert(false);//TODO
        struct tokenizer_ctx tctx = { 0 };
        struct token_list r = tokenizer(&tctx, "1", "", 0, TK_FLAG_NONE);
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
struct token_list expand_macro(struct preprocessor_ctx* ctx, struct macro_expanded* list, struct macro* macro, struct macro_argument_list* arguments, int level)
{
    macro->usage++;

    //printf("\nexpanding ");
    //print_macro(macro);
    //print_macro_arguments(arguments);
    //printf("\n");
    struct token_list r = { 0 };
    try
    {
        assert(!macro_already_expanded(list, macro->name));
        struct macro_expanded macro_expanded = { 0 };
        macro_expanded.name = macro->name;
        macro_expanded.p_previous = list;
        if (macro->is_function)
        {
            struct token_list copy = macro_copy_replacement_list(ctx, macro);
            struct token_list copy2 = replace_macro_arguments(ctx, &macro_expanded, &copy, arguments);
            if (ctx->n_errors > 0) throw;

            struct token_list r2 = replacement_list_reexamination(ctx, &macro_expanded, &copy2, level);
            if (ctx->n_errors > 0) throw;

            token_list_append_list(&r, &r2);
        }
        else
        {
            struct token_list copy = macro_copy_replacement_list(ctx, macro);
            struct token_list r3 = replacement_list_reexamination(ctx, &macro_expanded, &copy, level);
            if (ctx->n_errors > 0) throw;

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
                struct macro_argument_list arguments = collect_macro_arguments(ctx, macro, input_list, level);
                if (ctx->n_errors > 0) throw;


                struct token_list start_macro = expand_macro(ctx, NULL, macro, &arguments, level);
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
                            struct macro_argument_list arguments2 = collect_macro_arguments(ctx, macro, input_list, level);
                            if (ctx->n_errors > 0) throw;

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


                            struct token_list r3 = expand_macro(ctx, NULL, macro, &arguments2, level);
                            if (ctx->n_errors > 0) throw;

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
                        preprocessor_set_info_with_token(ctx, input_list->head, "you can use \"adjacent\" \"strings\"");
                    else if (input_list->head->type == TK_LINE_COMMENT)
                        preprocessor_set_warning_with_token(ctx, input_list->head, "multi-line //comment");
                    else
                        preprocessor_set_warning_with_token(ctx, input_list->head, "unnecessary line-slicing");
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
            struct map_entry* pentry = map->table[i];

            while (pentry != NULL)
            {
                struct macro* macro = pentry->p;
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

void add_standard_macros(struct preprocessor_ctx* ctx)
{

    static char mon[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };

    time_t now = time(NULL);
    struct tm* tm = localtime(&now);

    struct tokenizer_ctx tctx = { 0 };
#ifdef TEST
    tctx.printf = printf_nothing;
#else
    tctx.printf = printf;
#endif

    char datastr[100] = { 0 };
    snprintf(datastr, sizeof datastr, "#define __DATE__ \"%s %2d %d\"\n", mon[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);
    struct token_list l1 = tokenizer(&tctx, datastr, "__DATE__ macro inclusion", 0, TK_FLAG_NONE);
    preprocessor(ctx, &l1, 0);

    char timestr[100] = { 0 };
    snprintf(timestr, sizeof timestr, "#define __TIME__ \"%02d:%02d:%02d\"\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    struct token_list l2 = tokenizer(&tctx, timestr, "__TIME__ macro inclusion", 0, TK_FLAG_NONE);
    preprocessor(ctx, &l2, 0);


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
        "#define _INTEGRAL_MAX_BITS 64\n" /*Use of __int64 should be conditional on the predefined macro _INTEGRAL_MAX_BITS*/
#endif

#ifdef __linux__
        /*some gcc stuff need to parse linux headers*/
        "#define __linux__\n"
        "#define __builtin_va_list\n"
        "#define __builtin_va_start(a, b)\n"
        "#define __builtin_va_end(a)\n"
        "#define __builtin_va_arg(a, b)\n"
        "#define __builtin_va_copy(a, b)\n"
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

    struct token_list l = tokenizer(&tctx, pre_defined_macros_text, "standard macros inclusion", 0, TK_FLAG_NONE);
    struct token_list l10 = preprocessor(ctx, &l, 0);

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
    case TK_COMMENT: return "TK_COMENT";
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
            else if (current->type == TK_COMMENT)
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
        current->type == TK_COMMENT ||
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

void print_all_macros(struct preprocessor_ctx* prectx)
{
    for (int i = 0; i < prectx->macros.capacity; i++)
    {
        struct map_entry* entry = prectx->macros.table[i];
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
    if (!ctx->options.check_naming_conventions || token->level != 0)
        return;

    if (!is_screaming_case(token->lexeme)) {
        preprocessor_set_info_with_token(ctx, token, "use SCREAMING_CASE for macros");
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
    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);

    struct preprocessor_ctx ctx = { 0 };
    ctx.printf = printf;

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


int test_preprocessor_in_out(const char* input, const char* output)
{
    int res = 0;

    struct tokenizer_ctx tctx = { 0 };
    struct token_list list = tokenizer(&tctx, input, "source", 0, TK_FLAG_NONE);

    struct preprocessor_ctx ctx = { 0 };
    ctx.printf = printf_nothing;

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
        res = test_preprocessor_in_out(input, output);
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
    ctx.printf = printf;

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
    ctx.printf = printf;
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
    struct token_list list = tokenizer(&tctx, input, "", 0, TK_FLAG_NONE);

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

#endif



void ss_swap(struct osstream* a, struct osstream* b)
{
    struct osstream r = *a;
    *a = *b;
    *b = r;
    _del_attr(r, "must destroy");
}

void ss_clear(struct osstream* stream)
{
    stream->size = 0;
}

void ss_close(struct osstream* stream)
{
    free(stream->c_str);    
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
#endif


#if defined _MSC_VER && !defined __POCC__
#endif


#ifdef __POCC__
#define stat _stat
#endif

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


DIR* opendir(const char* name)
{
    assert(name != 0);
    WIN32_FIND_DATAA fdfile;

    char path[MAX_PATH] = { 0 };
    strcat(path, name);
    strcat(path, "\\*.*");

    HANDLE handle = FindFirstFileA(path, &fdfile);

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
    WIN32_FIND_DATAA fdfile;
    BOOL b = FindNextFileA(dirp->handle, &fdfile);
    if (b)
    {
        /*clear*/
        memset(&dirp->dirent, 0, sizeof(dirp->dirent));

        if (fdfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            dirp->dirent.d_type |= DT_DIR;
        }

        /*worst case trunks the string*/
        strncpy(dirp->dirent.d_name,
                fdfile.cFileName,
                sizeof(dirp->dirent.d_name) - 1);

        return &dirp->dirent;
    }
    else
    {
        errno = windows_error_to_posix(GetLastError());
    }
    return NULL;
}

/*
* realpath
   If there is no error, realpath() returns a pointer to the
       resolved_path.

       Otherwise, it returns NULL, the contents of the array
       resolved_path are undefined, and errno is set to indicate the
       error.
*/
char* realpath(const char* restrict path, char* restrict resolved_path)
{
    /*
    * _fullpath
    * Each of these functions returns a pointer to a buffer 
      containing the absolute path name (absPath). If there's an 
      error (for example, if the value passed in relPath includes a drive 
      letter that isn't valid or can't be found, or if the length of the 
      created absolute path name (absPath) is greater than maxLength), the function returns NULL.
    */
    return _fullpath(resolved_path, path, MAX_PATH);
}

#endif //_WINDOWS_

int copy_file(const char* pathfrom,
              const char* pathto)
{
    char buf[4096];
    size_t nread;
    int saved_errno;

    FILE* fd_from = fopen(pathfrom, "rb");
    if (fd_from == NULL)
        return -1;

    FILE* fd_to = fopen(pathto, "wb");
    if (fd_to == NULL)
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
    else
    {
        path[0] = 0;
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
,110,116,32,70,73,76,69,59,13,10,13,10,101,120,116,101,114,110,32,70,73,76,69,42,32
,115,116,100,105,110,59,13,10,101,120,116,101,114,110,32,70,73,76,69,42,32,115,116,100,111
,117,116,59,13,10,101,120,116,101,114,110,32,70,73,76,69,42,32,115,116,100,101,114,114,59
,13,10,13,10,116,121,112,101,100,101,102,32,105,110,116,32,115,105,122,101,95,116,59,13,10
,116,121,112,101,100,101,102,32,118,111,105,100,42,32,118,97,95,108,105,115,116,59,13,10,105
,110,116,32,114,101,109,111,118,101,40,99,111,110,115,116,32,99,104,97,114,42,32,102,105,108
,101,110,97,109,101,41,59,13,10,105,110,116,32,114,101,110,97,109,101,40,99,111,110,115,116
,32,99,104,97,114,42,32,111,108,100,44,32,99,111,110,115,116,32,99,104,97,114,42,32,110
,101,119,115,41,59,13,10,70,73,76,69,42,32,116,109,112,102,105,108,101,40,118,111,105,100
,41,59,13,10,99,104,97,114,42,32,116,109,112,110,97,109,40,99,104,97,114,42,32,115,41
,59,13,10,105,110,116,32,102,99,108,111,115,101,40,70,73,76,69,42,32,115,116,114,101,97
,109,41,59,13,10,105,110,116,32,102,102,108,117,115,104,40,70,73,76,69,42,32,115,116,114
,101,97,109,41,59,13,10,70,73,76,69,42,32,102,111,112,101,110,40,99,111,110,115,116,32
,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,105,108,101,110,97,109,101,44,32
,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,109,111,100,101
,41,59,13,10,70,73,76,69,42,32,102,114,101,111,112,101,110,40,99,111,110,115,116,32,99
,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,105,108,101,110,97,109,101,44,32,99
,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,109,111,100,101,44
,32,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,41,59,13
,10,118,111,105,100,32,115,101,116,98,117,102,40,70,73,76,69,42,32,114,101,115,116,114,105
,99,116,32,115,116,114,101,97,109,44,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116
,32,98,117,102,41,59,13,10,105,110,116,32,115,101,116,118,98,117,102,40,70,73,76,69,42
,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,44,32,99,104,97,114,42,32,114
,101,115,116,114,105,99,116,32,98,117,102,44,32,105,110,116,32,109,111,100,101,44,32,115,105
,122,101,95,116,32,115,105,122,101,41,59,13,10,105,110,116,32,102,112,114,105,110,116,102,40
,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,44,32,99,111
,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116
,44,32,46,46,46,41,59,13,10,105,110,116,32,102,115,99,97,110,102,40,70,73,76,69,42
,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,44,32,99,111,110,115,116,32,99
,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32,46,46,46
,41,59,13,10,105,110,116,32,112,114,105,110,116,102,40,99,111,110,115,116,32,99,104,97,114
,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32,46,46,46,41,59,13
,10,105,110,116,32,115,99,97,110,102,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101
,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32,46,46,46,41,59,13,10,105,110,116
,32,115,110,112,114,105,110,116,102,40,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32
,115,44,32,115,105,122,101,95,116,32,110,44,32,99,111,110,115,116,32,99,104,97,114,42,32
,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32,46,46,46,41,59,13,10,105
,110,116,32,115,112,114,105,110,116,102,40,99,104,97,114,42,32,114,101,115,116,114,105,99,116
,32,115,44,32,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32
,102,111,114,109,97,116,44,32,46,46,46,41,59,13,10,105,110,116,32,115,115,99,97,110,102
,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,115,44,32
,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109
,97,116,44,32,46,46,46,41,59,13,10,105,110,116,32,118,102,112,114,105,110,116,102,40,70
,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,44,32,99,111,110
,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44
,32,118,97,95,108,105,115,116,32,97,114,103,41,59,13,10,105,110,116,32,118,102,115,99,97
,110,102,40,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109,44
,32,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,102,111,114
,109,97,116,44,32,118,97,95,108,105,115,116,32,97,114,103,41,59,13,10,105,110,116,32,118
,112,114,105,110,116,102,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105
,99,116,32,102,111,114,109,97,116,44,32,118,97,95,108,105,115,116,32,97,114,103,41,59,13
,10,105,110,116,32,118,115,99,97,110,102,40,99,111,110,115,116,32,99,104,97,114,42,32,114
,101,115,116,114,105,99,116,32,102,111,114,109,97,116,44,32,118,97,95,108,105,115,116,32,97
,114,103,41,59,13,10,105,110,116,32,112,117,116,115,40,99,111,110,115,116,32,99,104,97,114
,42,32,115,116,114,41,59,13,10,105,110,116,32,102,112,117,116,115,40,99,111,110,115,116,32
,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,115,44,32,70,73,76,69,42,32,114
,101,115,116,114,105,99,116,32,115,116,114,101,97,109,41,59,13,10,105,110,116,32,103,101,116
,99,40,70,73,76,69,42,32,115,116,114,101,97,109,41,59,13,10,105,110,116,32,103,101,116
,99,104,97,114,40,118,111,105,100,41,59,13,10,105,110,116,32,112,117,116,99,40,105,110,116
,32,99,44,32,70,73,76,69,42,32,115,116,114,101,97,109,41,59,13,10,105,110,116,32,112
,117,116,99,104,97,114,40,105,110,116,32,99,41,59,13,10,105,110,116,32,112,117,116,115,40
,99,111,110,115,116,32,99,104,97,114,42,32,115,41,59,13,10,105,110,116,32,117,110,103,101
,116,99,40,105,110,116,32,99,44,32,70,73,76,69,42,32,115,116,114,101,97,109,41,59,13
,10,105,110,116,32,102,103,101,116,99,40,70,73,76,69,42,32,115,116,114,101,97,109,41,59
,13,10,115,105,122,101,95,116,32,102,114,101,97,100,40,118,111,105,100,42,32,114,101,115,116
,114,105,99,116,32,112,116,114,44,32,115,105,122,101,95,116,32,115,105,122,101,44,32,115,105
,122,101,95,116,32,110,109,101,109,98,44,32,70,73,76,69,42,32,114,101,115,116,114,105,99
,116,32,115,116,114,101,97,109,41,59,13,10,115,105,122,101,95,116,32,102,119,114,105,116,101
,40,99,111,110,115,116,32,118,111,105,100,42,32,114,101,115,116,114,105,99,116,32,112,116,114
,44,32,115,105,122,101,95,116,32,115,105,122,101,44,32,115,105,122,101,95,116,32,110,109,101
,109,98,44,32,70,73,76,69,42,32,114,101,115,116,114,105,99,116,32,115,116,114,101,97,109
,41,59,13,10,105,110,116,32,102,103,101,116,112,111,115,40,70,73,76,69,42,32,114,101,115
,116,114,105,99,116,32,115,116,114,101,97,109,44,32,102,112,111,115,95,116,42,32,114,101,115
,116,114,105,99,116,32,112,111,115,41,59,13,10,105,110,116,32,102,115,101,101,107,40,70,73
,76,69,42,32,115,116,114,101,97,109,44,32,108,111,110,103,32,105,110,116,32,111,102,102,115
,101,116,44,32,105,110,116,32,119,104,101,110,99,101,41,59,13,10,105,110,116,32,102,115,101
,116,112,111,115,40,70,73,76,69,42,32,115,116,114,101,97,109,44,32,99,111,110,115,116,32
,102,112,111,115,95,116,42,32,112,111,115,41,59,13,10,108,111,110,103,32,105,110,116,32,102
,116,101,108,108,40,70,73,76,69,42,32,115,116,114,101,97,109,41,59,13,10,118,111,105,100
,32,114,101,119,105,110,100,40,70,73,76,69,42,32,115,116,114,101,97,109,41,59,13,10,118
,111,105,100,32,99,108,101,97,114,101,114,114,40,70,73,76,69,42,32,115,116,114,101,97,109
,41,59,13,10,105,110,116,32,102,101,111,102,40,70,73,76,69,42,32,115,116,114,101,97,109
,41,59,13,10,105,110,116,32,102,101,114,114,111,114,40,70,73,76,69,42,32,115,116,114,101
,97,109,41,59,13,10,118,111,105,100,32,112,101,114,114,111,114,40,99,111,110,115,116,32,99
,104,97,114,42,32,115,41,59,13,10,13,10,13,10,13,10,35,105,102,110,100,101,102,32,78
,85,76,76,13,10,35,100,101,102,105,110,101,32,78,85,76,76,32,40,40,118,111,105,100,42
,41,48,41,13,10,35,101,110,100,105,102,13,10
    ,0 };

    static const unsigned char file_stdlib_h[] = {



13,10,35,112,114,97,103,109,97,32,111,110,99,101,13,10,13,10,116,121,112,101,100,101,102
,32,108,111,110,103,32,108,111,110,103,32,102,112,111,115,95,116,59,13,10,116,121,112,101,100
,101,102,32,105,110,116,32,115,105,122,101,95,116,59,13,10,116,121,112,101,100,101,102,32,105
,110,116,32,119,99,104,97,114,95,116,59,13,10,13,10,91,91,110,111,100,105,115,99,97,114
,100,93,93,32,100,111,117,98,108,101,32,97,116,111,102,40,99,111,110,115,116,32,99,104,97
,114,42,32,110,112,116,114,41,59,13,10,91,91,110,111,100,105,115,99,97,114,100,93,93,32
,105,110,116,32,97,116,111,105,40,99,111,110,115,116,32,99,104,97,114,42,32,110,112,116,114
,41,59,13,10,91,91,110,111,100,105,115,99,97,114,100,93,93,32,108,111,110,103,32,105,110
,116,32,97,116,111,108,40,99,111,110,115,116,32,99,104,97,114,42,32,110,112,116,114,41,59
,13,10,91,91,110,111,100,105,115,99,97,114,100,93,93,32,108,111,110,103,32,108,111,110,103
,32,105,110,116,32,97,116,111,108,108,40,99,111,110,115,116,32,99,104,97,114,42,32,110,112
,116,114,41,59,13,10,13,10,100,111,117,98,108,101,32,115,116,114,116,111,100,40,99,111,110
,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,110,112,116,114,44,32,99
,104,97,114,42,42,32,114,101,115,116,114,105,99,116,32,101,110,100,112,116,114,41,59,13,10
,102,108,111,97,116,32,115,116,114,116,111,102,40,99,111,110,115,116,32,99,104,97,114,42,32
,114,101,115,116,114,105,99,116,32,110,112,116,114,44,32,99,104,97,114,42,42,32,114,101,115
,116,114,105,99,116,32,101,110,100,112,116,114,41,59,13,10,108,111,110,103,32,100,111,117,98
,108,101,32,115,116,114,116,111,108,100,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101
,115,116,114,105,99,116,32,110,112,116,114,44,32,99,104,97,114,42,42,32,114,101,115,116,114
,105,99,116,32,101,110,100,112,116,114,41,59,13,10,108,111,110,103,32,105,110,116,32,115,116
,114,116,111,108,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116
,32,110,112,116,114,44,32,99,104,97,114,42,42,32,114,101,115,116,114,105,99,116,32,101,110
,100,112,116,114,44,32,105,110,116,32,98,97,115,101,41,59,13,10,108,111,110,103,32,108,111
,110,103,32,105,110,116,32,115,116,114,116,111,108,108,40,99,111,110,115,116,32,99,104,97,114
,42,32,114,101,115,116,114,105,99,116,32,110,112,116,114,44,32,99,104,97,114,42,42,32,114
,101,115,116,114,105,99,116,32,101,110,100,112,116,114,44,32,105,110,116,32,98,97,115,101,41
,59,13,10,117,110,115,105,103,110,101,100,32,108,111,110,103,32,105,110,116,32,115,116,114,116
,111,117,108,40,99,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32
,110,112,116,114,44,32,99,104,97,114,42,42,32,114,101,115,116,114,105,99,116,32,101,110,100
,112,116,114,44,32,105,110,116,32,98,97,115,101,41,59,13,10,117,110,115,105,103,110,101,100
,32,108,111,110,103,32,108,111,110,103,32,105,110,116,32,115,116,114,116,111,117,108,108,40,99
,111,110,115,116,32,99,104,97,114,42,32,114,101,115,116,114,105,99,116,32,110,112,116,114,44
,32,99,104,97,114,42,42,32,114,101,115,116,114,105,99,116,32,101,110,100,112,116,114,44,32
,105,110,116,32,98,97,115,101,41,59,13,10,105,110,116,32,114,97,110,100,40,118,111,105,100
,41,59,13,10,118,111,105,100,32,115,114,97,110,100,40,117,110,115,105,103,110,101,100,32,105
,110,116,32,115,101,101,100,41,59,13,10,118,111,105,100,42,32,97,108,105,103,110,101,100,95
,97,108,108,111,99,40,115,105,122,101,95,116,32,97,108,105,103,110,109,101,110,116,44,32,115
,105,122,101,95,116,32,115,105,122,101,41,59,13,10,91,91,110,111,100,105,115,99,97,114,100
,93,93,32,118,111,105,100,42,32,99,97,108,108,111,99,40,115,105,122,101,95,116,32,110,109
,101,109,98,44,32,115,105,122,101,95,116,32,115,105,122,101,41,59,13,10,118,111,105,100,32
,102,114,101,101,40,118,111,105,100,42,32,112,116,114,41,59,13,10,13,10,91,91,110,111,100
,105,115,99,97,114,100,93,93,32,118,111,105,100,42,32,109,97,108,108,111,99,40,115,105,122
,101,95,116,32,115,105,122,101,41,59,13,10,91,91,110,111,100,105,115,99,97,114,100,93,93
,32,118,111,105,100,42,32,114,101,97,108,108,111,99,40,118,111,105,100,42,32,112,116,114,44
,32,115,105,122,101,95,116,32,115,105,122,101,41,59,13,10,13,10,91,91,110,111,114,101,116
,117,114,110,93,93,32,118,111,105,100,32,97,98,111,114,116,40,118,111,105,100,41,59,13,10
,105,110,116,32,97,116,101,120,105,116,40,118,111,105,100,32,40,42,102,117,110,99,41,40,118
,111,105,100,41,41,59,13,10,105,110,116,32,97,116,95,113,117,105,99,107,95,101,120,105,116
,40,118,111,105,100,32,40,42,102,117,110,99,41,40,118,111,105,100,41,41,59,13,10,91,91
,110,111,114,101,116,117,114,110,93,93,32,118,111,105,100,32,101,120,105,116,40,105,110,116,32
,115,116,97,116,117,115,41,59,13,10,91,91,110,111,114,101,116,117,114,110,93,93,32,118,111
,105,100,32,95,69,120,105,116,40,105,110,116,32,115,116,97,116,117,115,41,59,13,10,99,104
,97,114,42,32,103,101,116,101,110,118,40,99,111,110,115,116,32,99,104,97,114,42,32,110,97
,109,101,41,59,13,10,91,91,110,111,114,101,116,117,114,110,93,93,32,118,111,105,100,32,113
,117,105,99,107,95,101,120,105,116,40,105,110,116,32,115,116,97,116,117,115,41,59,13,10,105
,110,116,32,115,121,115,116,101,109,40,99,111,110,115,116,32,99,104,97,114,42,32,115,116,114
,105,110,103,41,59,13,10,13,10,35,105,102,110,100,101,102,32,78,85,76,76,13,10,35,100
,101,102,105,110,101,32,78,85,76,76,32,40,40,118,111,105,100,42,41,48,41,13,10,35,101
,110,100,105,102,13,10,13,10
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

    static const unsigned char file_assert_h[] = {



13,10,35,105,102,100,101,102,32,68,69,66,85,71,13,10,35,100,101,102,105,110,101,32,97
,115,115,101,114,116,40,99,41,32,100,111,32,123,32,105,102,32,40,33,40,99,41,41,123,125
,32,125,32,119,104,105,108,101,40,48,41,13,10,35,101,108,115,101,13,10,35,100,101,102,105
,110,101,32,97,115,115,101,114,116,40,99,41,32,13,10,35,101,110,100,105,102,13,10,13,10
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
    else if (strcmp(path, "assert.h") == 0)
        return strdup(file_assert_h);
  
#endif

    return readfile_core(path);
}





static struct w {
    enum compiler_warning w;
    const char* name;
}
s_warnings[] = {
    {W_UNUSED_VARIABLE, "unused-variable"},
    {W_DEPRECATED, "deprecated"},
    {W_ENUN_COMPARE,"enum-compare"},
    {W_NON_NULL, "nonnull"},
    {W_ADDRESS, "address"},
    {W_UNUSED_PARAMETER, "unused-parameter"},
    {W_DECLARATOR_HIDE, "hide-declarator"},
    {W_TYPEOF_ARRAY_PARAMETER, "typeof-parameter"},
    {W_ATTRIBUTES, "attributes"},
    {W_UNUSED_VALUE, "unused-value"}
};

enum compiler_warning  get_warning_flag(const char* wname, bool* p_disable_warning)
{
    const bool disable_warning = (wname[2] == 'n' && wname[3] == 'o');
    *p_disable_warning = disable_warning;

    for (int j = 0; j < sizeof(s_warnings) / sizeof(s_warnings[0]); j++)
    {
        if (disable_warning)
        {
            if (strncmp(s_warnings[j].name, wname + 5, strlen(s_warnings[j].name)) == 0)
            {
                return s_warnings[j].w;
            }
        }
        else
        {
            if (strncmp(s_warnings[j].name, wname + 2, strlen(s_warnings[j].name)) == 0)
            {
                return s_warnings[j].w;
            }
        }
    }
    return 0;
}

const char* get_warning_name(enum compiler_warning w)
{
    int lower_index = 0;
    int upper_index = sizeof(s_warnings) / sizeof(s_warnings[0]) - 1;

    while (lower_index <= upper_index)
    {
        const int mid = (lower_index + upper_index) / 2;
        const int cmp = w - s_warnings[mid].w;

        if (cmp == 0)
        {
            return s_warnings[mid].name;
        }
        else if (cmp < 0)
        {
            upper_index = mid - 1;
        }
        else
        {
            lower_index = mid + 1;
        }
    }

    assert(false);
    return "";
}

int fill_options(struct options* options,
    int argc,
    const char** argv)
{

    /*
       default at this moment is same as -Wall
    */
    options->enabled_warnings_stack[0] = ~0;


    /*first loop used to collect options*/
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            continue;

        if (strcmp(argv[i], "-no-output") == 0)
        {
            options->no_output = true;
            continue;
        }

        if (strcmp(argv[i], "-o") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(options->output, argv[i + 1]);
                i++;
            }
            else
            {
                //ops
            }
            continue;
        }

        if (strcmp(argv[i], "-E") == 0)
        {
            options->preprocess_only = true;
            continue;
        }
        if (strcmp(argv[i], "-remove-comments") == 0)
        {
            options->remove_comments = true;
            continue;
        }
        if (strcmp(argv[i], "-direct-compilation") == 0 ||
            strcmp(argv[i], "-rm") == 0)
        {
            options->direct_compilation = true;
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


        if (strcmp(argv[i], "-st") == 0)
        {
            options->do_static_analisys = true;
            continue;
        }


        if (strcmp(argv[i], "-fo") == 0)
        {
            options->format_ouput = true;
            continue;
        }

        if (strcmp(argv[i], "-no-discard") == 0)
        {
            options->nodiscard_is_default = true;
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

        //warnings
        if (argv[i][1] == 'W')
        {
            if (strcmp(argv[i], "-Wall") == 0)
            {
                options->enabled_warnings_stack[0] = ~0;
                continue;
            }

            bool disable_warning = 0;
            enum compiler_warning  w = get_warning_flag(argv[i], &disable_warning);

            if (disable_warning)
            {
                options->enabled_warnings_stack[0] &= ~w;
            }
            else
            {
                options->enabled_warnings_stack[0] |= w;
            }
            continue;
        }
    }
    return 0;
}


void print_help()
{
    const char* options =
        "cake [options] source1.c source2.c ...\n"
        "\n"
        WHITE "SAMPLES\n" RESET
        "\n"
        WHITE "    cake source.c\n" RESET
        "    Compiles source.c and ouputs /out/source.c\n"
        "\n"
        WHITE "    cake -target=C11 source.c\n" RESET
        "    Compiles source.c and ouputs C11 code at /out/source.c\n"
        "\n"
        WHITE "OPTIONS\n" RESET
        "\n"
        WHITE "  -I                   " RESET " Adds a directory to the list of directories searched for include files \n"
        "                        (On windows, if you run cake at the visual studio command prompt cake \n"
        "                        uses the same include files used by msvc )\n"
        "\n"
        WHITE "  -no-output            " RESET "Cake will not generate ouput\n"
        "\n"
        WHITE "  -D                    " RESET "Defines a preprocessing symbol for a source file \n"
        "\n"
        WHITE "  -E                    " RESET "Copies preprocessor output to standard output \n"
        "\n"
        WHITE "  -o name.c             " RESET "Defines the ouput name. used when we compile one file\n"
        "\n"
        WHITE "  -remove-comments      " RESET "Remove all comments from the ouput file \n"
        "\n"
        WHITE "  -direct-compilation   " RESET "output without macros/preprocessor parts\n"
        "\n"
        WHITE "  -target=standard      " RESET "Output target C standard (c89, c99, c11, c2x, cxx) \n"
        "                        C99 is the default and C89 (ANSI C) is the minimum target \n"
        "\n"
        WHITE "  -std=standard         " RESET "Assume that the input sources are for standard (c89, c99, c11, c2x, cxx) \n"
        "                        (not implented yet, input is considered C23)                    \n"
        "\n"
        WHITE "  -n                    " RESET "Check naming conventions (it is hardcoded for its own naming convention)\n"
        "\n"
        WHITE "  -fi                   " RESET "Format input (format before language convertion)\n"
        "\n"
        WHITE "  -fo                   " RESET "Format output (format after language convertion, result parsed again)\n"
        "\n"
        WHITE "  --no-discard          " RESET "Makes [[nodiscard]] default implicitly \n"
        "\n"
        WHITE "  -Wname -Wno-name      " RESET "Enables or disable warning\n"

        "\n"
        "More details at http://thradams.com/cake/manual.html\n"
        ;

    printf("%s", options);
}





//#pragma once


//#pragma once


struct parser_ctx;


enum type_category
{
    TYPE_CATEGORY_ITSELF,
    TYPE_CATEGORY_FUNCTION,
    TYPE_CATEGORY_ARRAY,
    TYPE_CATEGORY_POINTER,
    TYPE_CATEGORY_NONE,
};

enum static_analisys_flags
{
    ISVALID = 1 << 1,
    UNINITIALIZED = 1 << 2,
    MUST_DESTROY = 1 << 3,
    MUST_FREE = 1 << 4
};


enum attribute_flags
{
    STD_ATTRIBUTE_NONE = 0,
    STD_ATTRIBUTE_DEPRECATED = 1 << 0,
    STD_ATTRIBUTE_FALLTHROUGH = 1 << 1,
    STD_ATTRIBUTE_MAYBE_UNUSED = 1 << 2,
    STD_ATTRIBUTE_NODISCARD = 1 << 3,
    STD_ATTRIBUTE_NORETURN = 1 << 4,
    STD_ATTRIBUTE_UNSEQUENCED = 1 << 5,
    STD_ATTRIBUTE_REPRODUCIBLE = 1 << 6,

    CUSTOM_ATTRIBUTE_FREE = 1 << 7,
    CUSTOM_ATTRIBUTE_DESTROY = 1 << 8,
    
    /*
     Used to detect argument type
    */
    CUSTOM_ATTRIBUTE_PARAM = 1 << 9,

    /*
     1 == 2 results in int in C
     lets add extra flag here
     not sure what is the best place to put in
     type specifier my generate some error
    */
    CUSTOM_ATTRIBUTE_LIKE_BOOL = 1 << 10,
    // 'a'
    CUSTOM_ATTRIBUTE_LIKE_CHAR = 1 << 11
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

    TYPE_SPECIFIER_TYPEOF = 1 << 23,

    TYPE_SPECIFIER_NULLPTR_T = 1 << 24,


};

enum type_qualifier_flags
{
    TYPE_QUALIFIER_NONE,
    TYPE_QUALIFIER_CONST = 1 << 0,
    TYPE_QUALIFIER_RESTRICT = 1 << 1,
    TYPE_QUALIFIER_VOLATILE = 1 << 2,
    TYPE_QUALIFIER__ATOMIC = 1 << 3    
};


struct declarator;
struct type;


struct type_list {
    struct type* head;
    struct type* tail;
};

void type_list_push_back(struct type_list* books, struct type* new_book);
void type_list_push_front(struct type_list* books, struct type* new_book);

struct param {
    struct type* type;
    struct param* next;
};

struct param_list {
    bool is_var_args;
    bool is_void;
    struct param* head;
    struct param* tail;
};

struct _destroy type 
{
    enum type_category category;
    enum attribute_flags  attributes_flags;
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;

    const char* name_opt;

    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;

    int array_size;
    bool static_array;
    struct param_list params;
    struct type* next;
};

const struct param_list* type_get_func_or_func_ptr_params(const struct type* p_type);

struct expression;

void check_assigment(struct parser_ctx* ctx,
    struct type* left_type,
    struct expression* right);

void print_type(struct osstream* ss, const  struct type* type);
void print_item(struct osstream* ss, bool* first, const char* item);
struct type type_dup(const struct type* p_type);
void type_destroy(_destroy struct type* p_type);


struct type get_function_return_type(struct type* p_type);

int type_common(struct type* p_type1, struct type* p_type2, struct type* out);
struct type get_array_item_type(const struct type* p_type);
struct type type_remove_pointer(const struct type* p_type);
int type_get_array_size(const struct type* p_type);
int type_set_array_size(struct type* p_type, int size);

bool type_is_enum(const struct type* p_type);
bool type_is_array(const struct type* p_type);
bool type_is_const(const struct type* p_type);
bool type_is_pointer(const struct type* p_type);
bool type_is_nullptr_t(const struct type* p_type);
bool type_is_void_ptr(const struct type* p_type);
bool type_is_integer(const struct type* p_type);
bool type_is_unsigned_integer(const struct type* p_type);
bool type_is_floating_point(const struct type* p_type);

bool type_is_arithmetic(const struct type* p_type);

bool type_is_struct_or_union(const struct type* p_type);
bool type_is_void(const struct type* p_type);
bool type_is_function_or_function_pointer(const struct type* p_type);
bool type_is_function(const struct type* p_type);
bool type_is_nodiscard(const struct type* p_type);
bool type_is_destroy(const struct type* p_type);
bool type_is_deprecated(const struct type* p_type);
bool type_is_maybe_unused(const struct type* p_type);
bool type_is_pointer_or_array(const struct type* p_type);
bool type_is_same(const struct type* a, const struct type* b, bool compare_qualifiers);
bool type_is_scalar(const struct type* p_type);
bool type_has_attribute(const struct type* p_type, enum attribute_flags attributes);
bool type_is_bool(const struct type* p_type);

struct type type_get_enum_type(const struct type* p_type);

struct argument_expression;
void check_function_argument_and_parameter(struct parser_ctx* ctx,
    struct argument_expression* current_argument,
    struct type* paramer_type,
    int param_num);

struct type type_convert_to(struct type* p_type, enum language_version target);
struct type type_lvalue_conversion(struct type* p_type);
void type_remove_qualifiers(struct type* p_type);
void type_add_const(struct type* p_type);
void type_swap(struct type* a, struct type* b);


struct type type_remove_pointer(const struct type* p_type);
struct type get_array_item_type(const struct type* p_type);

struct type type_param_array_to_pointer(const struct type* p_type);

struct type type_make_literal_string(int size, enum type_specifier_flags chartype);
struct type type_make_int();
struct type type_make_int_bool_like();
struct type type_make_size_t();
struct type type_make_enumerator(struct enum_specifier* enum_specifier);
struct type make_void_type();
struct type make_void_ptr_type();

struct type get_function_return_type(struct type* p_type);

int type_get_sizeof(struct type* p_type);
int type_get_alignof(struct type* p_type);
unsigned int type_get_hashof(struct parser_ctx* ctx, struct type* p_type);

struct type type_add_pointer(struct type* p_type);
void type_print(const struct type* a);
enum type_category type_get_category(const struct type* p_type);
void print_type_qualifier_specifiers(struct osstream* ss, const struct type* type);


void type_visit_to_mark_anonymous(struct type* p_type);

void type_flat_set_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator);
void print_type_declarator(struct osstream* ss, const struct type* p_type);
void type_remove_names(struct type* p_type);
const struct type* type_get_specifer_part(const struct type* p_type);


struct parser_ctx;

enum expression_type
{
    PRIMARY_IDENTIFIER,
    

    PRIMARY_EXPRESSION_ENUMERATOR,
    PRIMARY_EXPRESSION_DECLARATOR,
    PRIMARY_EXPRESSION_STRING_LITERAL,
    PRIMARY_EXPRESSION__FUNC__, /*predefined identifier __func__ */
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
    UNARY_EXPRESSION_TRAITS,
    UNARY_EXPRESSION_IS_SAME,
    UNARY_DECLARATOR_ATTRIBUTE_EXPR,
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

    CONDITIONAL_EXPRESSION,
};

struct argument_expression_list
{
    /*
     argument-expression-list:
        assignment-expression
        argument-expression-list , assignment-expression
    */
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


    /*
      Extension
      generic-selection:
        "_Generic" ( generic-argument, generic-assoc-list )

        generic-argument:
          assignment-expression
          type-name
    */


    struct expression* expression;
    struct type_name* type_name;
    /*
    * Points to the matching expression
    */
    struct expression* p_view_selected_expression;

    struct generic_assoc_list generic_assoc_list;
    struct token* first_token;
    struct token* last_token;
};

enum constant_value_type {
    TYPE_NOT_CONSTANT,
    TYPE_EMPTY,
    TYPE_LONG_LONG,
    TYPE_DOUBLE,
    TYPE_UNSIGNED_LONG_LONG
};

struct constant_value {
    enum constant_value_type type;
    union {
        unsigned long long ullvalue;
        long long llvalue;
        double dvalue;
    };
};

struct constant_value make_constant_value_double(double d);
struct constant_value make_constant_value_ull(unsigned long long d);
struct constant_value make_constant_value_ll(long long d);
struct constant_value constant_value_op(const struct constant_value* a, const  struct constant_value* b, int op);
unsigned long long constant_value_to_ull(const struct constant_value* a);
long long constant_value_to_ll(const struct constant_value* a);
long long constant_value_to_ll(const struct constant_value* a);
bool constant_value_to_bool(const struct constant_value* a);
bool constant_value_is_valid(const struct constant_value* a);

struct expression
{
    enum expression_type expression_type;
    struct type type;

    struct constant_value constant_value;

    struct type_name* type_name; 
    struct type_name* type_name2; /*is_same*/
    struct braced_initializer* braced_initializer;
    struct compound_statement* compound_statement; //function literal (lambda)
    struct generic_selection* generic_selection; //_Generic

    struct token* first_token;
    struct token* last_token;

    /*token used in _add_attr, _has_attr, _del_attr, return*/
    struct token* contract_arg_token;

    /*se expressão for um identificador ele aponta para declaração dele*/
    struct declarator* declarator;

    /*se for POSTFIX_FUNCTION_CALL post*/
    struct argument_expression_list argument_expression_list; //este node eh uma  chamada de funcao

    struct expression* condition_expr;
    struct expression* left;
    struct expression* right;
};

struct expression* assignment_expression(struct parser_ctx* ctx);
struct expression* expression(struct parser_ctx* ctx);
struct expression* constant_expression(struct parser_ctx* ctx);
bool expression_is_subjected_to_lvalue_conversion(struct expression*);


//#pragma once


#define CAKE_VERSION "0.5.9"


struct _destroy scope
{
    int scope_level;
    struct hash_map tags;
    struct hash_map variables;
        
    struct scope* next;
    struct scope* previous;        
};

void scope_destroy(_destroy struct scope* p);

struct scope_list
{
    struct scope* head;
    struct scope* tail;
};
void scope_list_push(struct scope_list* list, struct scope* s);
void scope_list_pop(struct scope_list* list);

struct report
{
    int error_count;
    int warnings_count;
    int info_count;
};




struct _destroy parser_ctx
{
    struct options options;
    
    /*
      file scope -> function params -> function -> inner scope
    */
    struct scope_list scopes;
    
    /*
    * Points to the function we're in. Or null in file scope.
    */
    struct declaration* p_current_function_opt;

    /*
    * Points to the try-block we're in. Or null.
    */
    const struct try_statement* p_current_try_statement_opt;

    struct token_list input_list;
    struct token* current;
    struct token* previous;
    int try_catch_block_index;
    int n_warnings;
    int n_errors;
    int n_info;
    int (*printf)(const char* fmt, ...);    
};

///////////////////////////////////////////////////////

void parser_ctx_destroy(_destroy struct parser_ctx* ctx);


struct token* parser_look_ahead(struct parser_ctx* ctx);

struct token* parser_match(struct parser_ctx* ctx);
int parser_match_tk(struct parser_ctx* ctx, enum token_type type);
struct token* parser_look_ahead(struct parser_ctx* ctx);
struct token* previous_parser_token(struct token* token);
struct declarator* find_declarator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt);
struct enumerator* find_enumerator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt);

struct struct_or_union_specifier* find_struct_or_union_specifier(struct parser_ctx* ctx, const char* lexeme);
bool first_is(struct parser_ctx* ctx, enum token_type type);
void print_scope(struct scope_list* e);

char* CompileText(const char* options, const char* content);

void compiler_set_error_with_token(struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...);
_Bool compiler_set_warning_with_token(enum compiler_warning w, struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...);
void compiler_set_info_with_token(struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...);

int compile(int argc, const char** argv, struct report* error);
struct declaration_list parse(struct options* options, struct token_list* list, struct report* report);


void print_type_qualifier_flags(struct osstream* ss, bool* first, enum type_qualifier_flags e_type_qualifier_flags);

enum token_type parse_number(const char* lexeme, enum type_specifier_flags* flags_opt);
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
    /*
       declaration-specifier:
         storage-class-specifier
         type-specifier-qualifier
         function-specifier
    */
    struct storage_class_specifier* storage_class_specifier;

    struct type_specifier_qualifier* type_specifier_qualifier;
    
    struct function_specifier* function_specifier;

    struct declaration_specifier* next;
};

struct declaration_specifier* declaration_specifier(struct parser_ctx* ctx);


struct declaration_specifiers
{
    /*
     declaration-specifiers:
       declaration-specifier attribute-specifier-sequence opt
       declaration-specifier declaration-specifiers
    */

    /*cumulative flags*/
    enum attribute_flags  attributes_flags;
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
    enum storage_class_specifier_flags storage_class_specifier_flags;

    /*shortcuts*/
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;
    struct declarator* typedef_declarator;
    struct typeof_specifier* typeof_specifier;

    struct token* first_token; /*not owner*/
    struct token* last_token; /*not owner*/

    struct declaration_specifier* head;
    struct declaration_specifier* tail;
};

void print_declaration_specifiers(struct osstream* ss, struct declaration_specifiers* p);
struct declaration_specifiers* declaration_specifiers(struct parser_ctx* ctx);

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
struct static_assert_declaration* static_assert_declaration(struct parser_ctx* ctx);

struct attribute_specifier_sequence
{
    /*
     attribute-specifier-sequence:
       attribute-specifier-sequence opt attribute-specifier
    */

    struct token* first_token;
    struct token* last_token;
    enum attribute_flags  attributes_flags;
    struct attribute_specifier* head;
    struct attribute_specifier* tail;
};
struct attribute_specifier_sequence* attribute_specifier_sequence_opt(struct parser_ctx* ctx);

struct attribute_specifier
{
    /*
     attribute-specifier:
        [ [ attribute-list ] ]
    */
    struct token* first_token;
    struct token* last_token;
    struct attribute_list* attribute_list;
    struct attribute_specifier* next;
};

struct attribute_specifier* attribute_specifier(struct parser_ctx* ctx);
struct attribute* attribute(struct parser_ctx* ctx);


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
    /*
     function-specifier:
       inline
       _Noreturn
    */
    struct token* token;
};
struct function_specifier* function_specifier(struct parser_ctx* ctx);


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
    struct token* first_token;
    struct token* last_token;
    struct typeof_specifier_argument* typeof_specifier_argument;
    struct type type;    
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

struct type_specifier* type_specifier(struct parser_ctx* ctx);

struct init_declarator_list
{
    /*
     init-declarator-list:
       init-declarator
       init-declarator-list , init-declarator
    */
    struct init_declarator* head;
    struct init_declarator* tail;    
};

struct init_declarator_list init_declarator_list(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt
    );

struct declaration
{
    /*
      declaration:
        declaration-specifiers init-declarator-list opt ;
        attribute-specifier-sequence declaration-specifiers init-declarator-list ;
        static_assert-declaration
        attribute-declaration
    */
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt;
    
    struct static_assert_declaration* static_assert_declaration;

    struct declaration_specifiers* declaration_specifiers;
    struct init_declarator_list init_declarator_list;

    struct compound_statement* function_body;
    struct declarator* contract_declarator;

    struct token* first_token;
    struct token* last_token;

    struct declaration* next;
};

struct declaration* external_declaration(struct parser_ctx* ctx);

struct atomic_type_specifier
{
    /*
      atomic-type-specifier:
        "_Atomic" ( type-name )
    */
    struct token* token;
    struct type_name* type_name;
};

struct atomic_type_specifier* atomic_type_specifier(struct parser_ctx* ctx);

struct enumerator_list
{
    /*
     enumerator-list:
       enumerator
       enumerator-list , enumerator
    */
    struct enumerator* head;
    struct enumerator* tail;
};

struct enumerator_list enumerator_list(struct parser_ctx* ctx,
    struct enum_specifier*  p_enum_specifier
    );


struct enum_specifier
{
    /*
     enum-type-specifier:
       : specifier-qualifier-lis
    
     enum-specifier:
       "enum" attribute-specifier-sequence opt identifier opt enum-type-specifier opt  { enumerator-list }
       "enum" attribute-specifier-sequence opt identifier opt enum-type-specifier opt  { enumerator-list , }
       "enum" identifier enum-type-specifier opt
    */
    struct attribute_specifier_sequence* attribute_specifier_sequence_opt;
    struct specifier_qualifier_list* specifier_qualifier_list;
    

    struct enumerator_list enumerator_list;
    
    struct token* tag_token;
    struct token* first_token;
    /*points to the complete enum (can be self pointed)*/
    struct enum_specifier* complete_enum_specifier;
};

struct enum_specifier* enum_specifier(struct parser_ctx*);

struct member_declaration_list
{
    /*
     member-declaration-list:
       member-declaration
       member-declaration-list member-declaration
    */

    struct token* first_token; /*TODO ? necessary*/
    struct token* last_token;
    struct member_declaration* head;
    struct member_declaration* tail;
};

struct member_declaration_list member_declaration_list(struct parser_ctx* ctx);
struct member_declarator* find_member_declarator(struct member_declaration_list* list, const char* name);

struct struct_or_union_specifier
{
    /*
     struct-or-union-specifier:
       struct-or-union attribute-specifier-sequence opt identifier opt { member-declaration-list }
       struct-or-union attribute-specifier-sequence opt identifier
    */
    struct attribute_specifier_sequence* attribute_specifier_sequence_opt;
    struct member_declaration_list member_declaration_list;
    
    struct token* first_token;
    struct token* last_token;

    /*
    * Token que possui tag da struct
    */
    struct token* tagtoken;

    char tag_name[200];
    /*geramos um tag name para anomimas, mas colocamos banonymousTag para true*/
    bool has_anonymous_tag;
    /*it was asked to show struct tag created for anonymous*/
    bool show_anonymous_tag;

    int scope_level; /*nivel escopo 0 global*/
    int visit_moved; /*nivel escopo 0 global*/
        
    /*
    * This points to the first struct_or_union_specifier that will have it´s
    * complete_struct_or_union_specifier_indirection pointing to the complete
    * struct_or_union_specifier.
    */
    struct struct_or_union_specifier* complete_struct_or_union_specifier_indirection;
};

struct struct_or_union_specifier* struct_or_union_specifier(struct parser_ctx* ctx);
bool struct_or_union_specifier_is_complete(struct struct_or_union_specifier* p_struct_or_union_specifier);
struct struct_or_union_specifier* get_complete_struct_or_union_specifier(struct struct_or_union_specifier* p_struct_or_union_specifier);

struct init_declarator
{
    /*
     init-declarator:
        declarator
        declarator = initializer
    */

    struct declarator* declarator;
    struct initializer* initializer;
    struct init_declarator* next;
};

struct init_declarator* init_declarator(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt
    );

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

struct initializer* initializer(struct parser_ctx* ctx);


struct declarator
{
    /*
      declarator:
        pointer opt direct-declarator
    */

    struct token* first_token;
    struct token* last_token;


    struct pointer* pointer;
    struct direct_declarator* direct_declarator;

    
    struct declaration_specifiers* declaration_specifiers; //aponta para este cara
    struct specifier_qualifier_list* specifier_qualifier_list; //aponta
    struct token* name; //shortcut

    struct compound_statement* function_body;
           
    int num_uses; /*used to show not used warnings*/

    bool is_parameter_declarator;
    
    enum static_analisys_flags static_analisys_flags;    

    /*Já mastiga o tipo dele*/
    struct type type;    
};

enum type_specifier_flags declarator_get_type_specifier_flags(const struct declarator* p);

struct declarator;
void print_declarator(struct osstream* ss, struct declarator* declarator, bool is_abstract);

struct declarator* declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* specifier_qualifier_list,
    struct declaration_specifiers* declaration_specifiers,
    bool abstract_acceptable,
    struct token** pptokenname);

struct array_declarator
{
    /*
     array-declarator:
        direct-declarator [ type-qualifier-list opt assignment-expression opt ]
        direct-declarator [ "static" type-qualifier-list opt assignment-expression ]
        direct-declarator [ type-qualifier-list "static" assignment-expression ]
        direct-declarator [ type-qualifier-listopt * ]
    */
    struct direct_declarator* direct_declarator;
    struct expression* assignment_expression;
    struct expression* expression;
    struct type_qualifier_list* type_qualifier_list_opt;
    unsigned long long constant_size;
    struct token* token;
    struct token* static_token_opt;
};

struct function_declarator
{
    /*
     function-declarator:
       direct-declarator ( parameter-type-list opt )
    */
    struct direct_declarator* direct_declarator;
    struct scope parameters_scope; //usado para escopo parametros
    struct parameter_type_list* parameter_type_list_opt;
};

struct direct_declarator
{
    /*
     direct-declarator:
        identifier attribute-specifier-sequence opt
        ( declarator )
        array-declarator attribute-specifier-sequence opt
        function-declarator attribute-specifier-sequence opt
    */
    struct token* name_opt;
    struct declarator* declarator;
    struct array_declarator* array_declarator;
    struct function_declarator* function_declarator;
};

struct direct_declarator* direct_declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* specifier_qualifier_list,
    struct declaration_specifiers* declaration_specifiers,
    bool abstract_acceptable,
    struct token** pptoken_name
    );

struct parameter_type_list
{
    /*
    parameter-type-list:
        parameter-list
        parameter-list , ...
        ...
    */
    bool is_var_args; /*(...)*/
    bool is_void;/*(void)*/
    struct parameter_list* parameter_list;
};

struct parameter_type_list* parameter_type_list(struct parser_ctx* ctx);

struct pointer
{
    /*
     pointer:
        * attribute-specifier-sequence opt type-qualifier-list opt
        * attribute-specifier-sequence opt type-qualifier-list opt pointer    
    */
    struct attribute_specifier_sequence* attribute_specifier_sequence_opt;
    struct type_qualifier_list* type_qualifier_list_opt;
    struct pointer* pointer;
};

struct pointer* pointer_opt(struct parser_ctx* ctx);

struct parameter_list
{
    /*
     parameter-list:
        parameter-declaration
        parameter-list , parameter-declaration
    */
    struct parameter_declaration* head;
    struct parameter_declaration* tail;
};
struct parameter_list* parameter_list(struct parser_ctx* ctx);

struct parameter_declaration
{
    /*
     parameter-declaration:
      attribute-specifier-sequence opt declaration-specifiers declarator
      attribute-specifier-sequence opt declaration-specifiers abstract-declarator opt
    */
    struct attribute_specifier_sequence* attribute_specifier_sequence_opt;
    
    struct declaration_specifiers* declaration_specifiers;
    struct declarator* declarator;

    struct parameter_declaration* next;
};
struct parameter_declaration* parameter_declaration(struct parser_ctx* ctx);

struct type_name
{
    /*
     type-name:
       specifier-qualifier-list abstract-declarator opt
    */

    struct token* first_token;
    struct token* last_token;
    struct specifier_qualifier_list* specifier_qualifier_list;
    struct declarator* declarator;
    struct type type;
};

struct type_name* type_name(struct parser_ctx* ctx);
void print_type_name(struct osstream* ss, struct type_name* p);

struct argument_expression
{
    struct expression* expression;
    struct argument_expression* next;
};

struct braced_initializer
{    
    /*
      { }
      { initializer-list }
      { initializer-list , }
    */

    struct token* first_token;
    struct initializer_list* initializer_list;
};
struct braced_initializer* braced_initializer(struct parser_ctx* ctx);

struct type_specifier_qualifier
{
    /*
     type-specifier-qualifier:
       type-specifier
       type-qualifier
       alignment-specifier
    */

    struct type_specifier* type_specifier;
    struct type_qualifier* type_qualifier;
    struct alignment_specifier* alignment_specifier;

    struct type_specifier_qualifier* next;
};
struct type_specifier_qualifier* type_specifier_qualifier(struct parser_ctx* ctx);


struct specifier_qualifier_list
{
    /*
      specifier-qualifier-list:
        type-specifier-qualifier attribute-specifier-sequence opt
        type-specifier-qualifier specifier-qualifier-list
    */

    /*cumulative flags*/
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;

    /*shortcuts*/
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;
    struct typeof_specifier* typeof_specifier;
    struct declarator* typedef_declarator;
    
    struct type_specifier_qualifier* head;
    struct type_specifier_qualifier* tail;
    struct token* first_token;
    struct token* last_token;

};

struct specifier_qualifier_list* specifier_qualifier_list(struct parser_ctx* ctx);
void print_specifier_qualifier_list(struct osstream* ss, bool* first, struct specifier_qualifier_list* p_specifier_qualifier_list);


struct alignment_specifier
{
    /*
     alignment-specifier:
       "alignas" ( type-name )
       "alignas" ( constant-expression )
    */
    struct token* token;
};
struct alignment_specifier* alignment_specifier(struct parser_ctx* ctx);


struct type_qualifier
{
    /*
     type-qualifier:
       "const"
       "restrict"
       "volatile"
       "_Atomic"
    */
    enum type_qualifier_flags flags;
    struct token* token;
    struct type_qualifier* next;
};

struct type_qualifier* type_qualifier(struct parser_ctx* ctx);

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

struct member_declaration* member_declaration(struct parser_ctx* ctx);

struct member_declarator
{
    /*    
     member-declarator:
       declarator
       declarator opt : constant-expression
    */
    
    struct declarator* declarator;
    struct expression* constant_expression;
    struct member_declarator* next;
};

struct member_declarator_list
{
    /*    
     member-declarator-list:
        member-declarator
        member-declarator-list , member-declarator
    */

    struct member_declarator* head;
    struct member_declarator* tail;
};

struct member_declarator_list* member_declarator_list(struct parser_ctx* ctx,
    struct specifier_qualifier_list* specifier_qualifier_list
    );

struct block_item_list
{
    /*
     block-item-list:
       block-item
       block-item-list block-item
    */
    struct block_item* head;
    struct block_item* tail;
};
struct block_item_list block_item_list(struct parser_ctx* ctx);


struct compound_statement
{
    /*
     compound-statement:
       { block-item-list opt }
    */
    struct token* first_token; /*{*/
    struct token* last_token; /*}*/

    struct block_item_list block_item_list;
};
struct compound_statement* compound_statement(struct parser_ctx* ctx);

struct defer_statement
{
    /*
     defer-statement: (extension)
       "defer" secondary-block     
    */
    struct token* first_token;
    struct token* last_token;
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
    struct token* first_token; /*try*/
    struct token* last_token;
    struct token* catch_token_opt; /*catch*/
    /*Used to generate label names*/
    int try_catch_block_index;
};

struct try_statement* try_statement(struct parser_ctx* ctx);

struct selection_statement
{
    /*
      selection-statement:
        "if" ( expression ) secondary-block
        "if" ( expression ) secondary-block "else" secondary-block
        "switch" ( expression ) secondary-block
    */

    /*C++ 17 if with initialization extension*/
    struct init_declarator* init_declarator;

    struct expression* expression;
    struct secondary_block* secondary_block;
    struct secondary_block* else_secondary_block_opt;

    struct token* first_token;
    struct token* last_token;
    struct token* else_token_opt;    
};

struct selection_statement* selection_statement(struct parser_ctx* ctx);

struct iteration_statement
{
    /*
      iteration-statement:
        "while" ( expression ) secondary-block
        "do" secondary-block "while" ( expression ) ;
        "for" ( expression opt ; expression opt ; expression opt ) secondary-block
        "for" ( declaration expression opt ; expression opt ) secondary-block
        "repeat" secondary-block  (extension)
    */
    
    struct token* first_token;
    struct token* second_token; /*do {} while*/

    struct secondary_block* secondary_block;
    struct expression* expression1;
    struct expression* expression2;
};

struct iteration_statement* iteration_statement(struct parser_ctx* ctx);

struct jump_statement
{
    /*
     jump-statement:
       "goto" identifier ;
       "continue" ;
       "break" ;
       "return" expression opt ;
    */
    
    struct token* label;
    struct token* first_token;
    struct token* last_token;
    struct expression* expression_opt;
    
    int try_catch_block_index;
};

struct jump_statement* jump_statement(struct parser_ctx* ctx);

struct expression_statement
{
    /*
     expression-statement:
       expression opt;
       attribute-specifier-sequence expression ;
    */
    
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt;
    struct expression* expression_opt;
};

struct expression_statement* expression_statement(struct parser_ctx* ctx);

struct block_item
{
    /*
     block-item:
      declaration
      unlabeled-statement
      label
    */
    struct token* first_token; //?necessary
    struct declaration* declaration;    
    struct unlabeled_statement* unlabeled_statement;
    struct label* label;

    struct block_item* next;
};
struct block_item* block_item(struct parser_ctx* ctx);

struct compound_statement* function_body(struct parser_ctx* ctx);

struct designator
{
    /*
     designator:
       [ constant-expression ]
       . identifier
    */
    struct expression* constant_expression_opt;
    struct token* token;
    struct designator* next;
};
struct designator* designator(struct parser_ctx* ctx);

struct initializer_list
{
    /*
      initializer-list:
        designation opt initializer
        initializer-list , designation opt initializer
    */
    struct token* first_token;
    struct initializer* head;
    struct initializer* tail;
    int size;
};
struct initializer_list* initializer_list(struct parser_ctx* ctx);

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
    struct token* first_token;
    struct token* last_token;
    struct statement* statement;
};

struct unlabeled_statement
{
    /*
      unlabeled-statement:
        expression-statement
        attribute-specifier-sequence opt primary-block
        attribute-specifier-sequence opt jump-statement
     */

    struct expression_statement* expression_statement;
    struct primary_block* primary_block;
    struct jump_statement* jump_statement;
};

struct unlabeled_statement* unlabeled_statement(struct parser_ctx* ctx);

struct labeled_statement
{
    /*
     label statement:
       label statement
    */
    struct label* label;
    struct statement* statement;
};
struct labeled_statement* labeled_statement(struct parser_ctx* ctx);

struct statement
{    
    /*
     statement:
       labeled-statement
       unlabeled-statemen
    */
    struct labeled_statement* labeled_statement;
    struct unlabeled_statement* unlabeled_statement;
};
struct statement* statement(struct parser_ctx* ctx);

struct designator_list
{
    /*
     designator-list:
       designator
       designator-list designator
    */
    struct designator* head;
    struct designator* tail;
};

struct designator_list* designator_list(struct parser_ctx* ctx);

struct designation
{
    /*
     designation:
       designator-list =
    */
    struct token* token;
};
struct designation* designation(struct parser_ctx* ctx);


struct type_qualifier_list
{
    /*
     type-qualifier-list:
       type-qualifier
       type-qualifier-list type-qualifier
    */
    enum type_qualifier_flags flags;
    struct type_qualifier* head;
    struct type_qualifier* tail;
};

struct type_qualifier_list* type_qualifier_list(struct parser_ctx* ctx);

struct attribute_token
{
    enum attribute_flags attributes_flags;
    struct token* token;
};
struct attribute_token* attribute_token(struct parser_ctx* ctx);

struct attribute
{
    enum attribute_flags  attributes_flags;
    struct attribute_token* attribute_token;
    struct attribute_argument_clause* attribute_argument_clause;
    struct attribute_specifier* next;
};

struct attribute_list
{
    enum attribute_flags  attributes_flags;
    struct attribute* head;
    struct attribute* tail;
};
struct attribute_list* attribute_list(struct parser_ctx* ctx);

struct enumerator
{
    /*
      enumeration-constant:
        identifier
   
      enumerator:
        enumeration-constant attribute-specifier-sequence opt
        enumeration-constant attribute-specifier-sequence opt = constant-expression
    */
    
    struct token* token;
    struct attribute_specifier_sequence* attribute_specifier_sequence_opt;

    struct expression* constant_expression_opt;
    struct enum_specifier* enum_specifier;
    struct enumerator* next;
    long long value;    
};

struct enumerator* enumerator(struct parser_ctx* ctx, struct enum_specifier* p_enum_specifier);

struct attribute_argument_clause
{
    /*
     attribute-argument-clause:
       ( balanced-token-sequence opt )
    */
    struct token* token;
};

struct attribute_argument_clause* attribute_argument_clause(struct parser_ctx* ctx);

bool first_of_attribute(struct parser_ctx* ctx);

struct balanced_token_sequence
{
    struct balanced_token* head;
    struct balanced_token* tail;
};
struct balanced_token_sequence* balanced_token_sequence_opt(struct parser_ctx* ctx);

bool is_first_of_conditional_expression(struct parser_ctx* ctx);
bool first_of_type_name(struct parser_ctx* ctx);
bool first_of_type_name_ahead(struct parser_ctx* ctx);

struct argument_expression_list argument_expression_list(struct parser_ctx* ctx);

struct declaration_list
{
    struct declaration* head;
    struct declaration* tail;
};

struct declaration_list translation_unit(struct parser_ctx* ctx);
void declaration_list_destroy(struct declaration_list* list);

struct label
{
    /*
     label:
       attribute-specifier-sequence opt identifier :
       attribute-specifier-sequence opt "case" constant-expression :
       attribute-specifier-sequence opt "default" :
    */
    struct expression* constant_expression;
    struct token* name;
};

struct label* label(struct parser_ctx* ctx);

struct _destroy ast
{
    struct token_list token_list;
    struct declaration_list declaration_list;
};


struct ast get_ast(struct options* options, const char* filename, const char* source, struct report* report);
void ast_destroy(_destroy struct ast* ast);
struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);




#ifdef _WIN32
#endif

#if defined _MSC_VER && !defined __POCC__
#endif


struct constant_value make_constant_value_double(double d) {
    struct constant_value r;
    r.dvalue = d;
    r.type = TYPE_DOUBLE;
    return r;
}

struct constant_value make_constant_value_ull(unsigned long long d) {
    struct constant_value r;
    r.ullvalue = d;
    r.type = TYPE_UNSIGNED_LONG_LONG;
    return r;
}

struct constant_value make_constant_value_ll(long long d) {
    struct constant_value r;
    r.llvalue = d;
    r.type = TYPE_LONG_LONG;
    return r;
}

double constant_value_to_double(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_LONG_LONG: return (double)a->llvalue;
    case TYPE_DOUBLE: return  a->dvalue;
    case TYPE_UNSIGNED_LONG_LONG: return (double)a->ullvalue;
    }

    return 0;
}

bool constant_value_is_valid(const struct constant_value* a)
{
    return a->type != TYPE_NOT_CONSTANT &&
        a->type != TYPE_EMPTY;
}

unsigned long long constant_value_to_ull(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_LONG_LONG: return (unsigned long long)a->llvalue;
    case TYPE_DOUBLE: return  (unsigned long long)a->dvalue;
    case TYPE_UNSIGNED_LONG_LONG: return (unsigned long long) a->ullvalue;
    }

    return 0;
}
long long constant_value_to_ll(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_LONG_LONG: return (long long)a->llvalue;
    case TYPE_DOUBLE: return  (long long)a->dvalue;
    case TYPE_UNSIGNED_LONG_LONG: return (long long)a->ullvalue;
    }

    return 0;
}
bool constant_value_to_bool(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_LONG_LONG: return a->llvalue != 0;
    case TYPE_DOUBLE: return  a->dvalue != 0;
    case TYPE_UNSIGNED_LONG_LONG: return a->ullvalue != 0;
    }

    return 0;
}

struct constant_value constant_value_cast(const struct constant_value* a, enum constant_value_type type)
{
    struct constant_value r = *a;

    if (a->type == TYPE_EMPTY || a->type == TYPE_NOT_CONSTANT)
    {
        return r;
    }

    switch (type)
    {
    case TYPE_NOT_CONSTANT:
    case TYPE_EMPTY:
        assert(false);
        break;

    case TYPE_LONG_LONG:
        r.type = TYPE_LONG_LONG;
        r.llvalue = constant_value_to_ll(a);
        break;
    case TYPE_DOUBLE:
        r.type = TYPE_DOUBLE;
        r.dvalue = constant_value_to_double(a);
        break;
    case TYPE_UNSIGNED_LONG_LONG:
        r.type = TYPE_UNSIGNED_LONG_LONG;
        r.ullvalue = constant_value_to_ull(a);
        break;
    }
    return r;
}

struct constant_value constant_value_unary_op(const struct constant_value* a, int op)
{
    struct constant_value r = { 0 };
    if (!constant_value_is_valid(a))
    {
        return r;
    }

    if (a->type == TYPE_DOUBLE)
    {
        r.type = TYPE_DOUBLE;
        switch (op)
        {
        case '!':r.dvalue = !a->dvalue;  break;
            //case '~':r.dvalue = ~ a->dvalue;  break;
        case '+':r.dvalue = +a->dvalue;  break;
        case '-':r.dvalue = -a->dvalue;  break;
        default:
            assert(false);
            break;
        }
        return r;
    }
    else if (a->type == TYPE_UNSIGNED_LONG_LONG)
    {
        r.type = TYPE_UNSIGNED_LONG_LONG;
        switch (op)
        {
        case '!':r.ullvalue = !a->ullvalue;  break;
        case '~':r.ullvalue = ~a->ullvalue;  break;
        case '+':r.ullvalue = a->ullvalue;  break;
            //case '-':r.dvalue = -a->ullvalue;  break;
        case '-':
            r.dvalue = 0;// -a->ullvalue;
            break;
        default:
            assert(false);
            break;
        }
        return r;
    }
    else if (a->type == TYPE_LONG_LONG)
    {
        r.type = TYPE_UNSIGNED_LONG_LONG;
        switch (op)
        {
        case '!':r.llvalue = !((long long)a->llvalue);  break;
        case '~':r.llvalue = ~((long long)a->llvalue);  break;
        case '+':r.llvalue = +((long long)a->llvalue);  break;
        case '-':r.llvalue = -((long long)a->llvalue);  break;
        default:
            assert(false);
            break;
        }
        return r;
    }

    return r;
}


struct constant_value constant_value_op(const struct constant_value* a, const struct constant_value* b, int op)
{
    //TODO https://github.com/thradams/checkedints
    struct constant_value r = { 0 };
    if (!constant_value_is_valid(a) || !constant_value_is_valid(b))
    {
        return r;
    }

    if (a->type == TYPE_DOUBLE || b->type == TYPE_DOUBLE)
    {
        double va = constant_value_to_double(a);
        double vb = constant_value_to_double(b);
        r.type = TYPE_DOUBLE;

        switch (op)
        {
            //Arithmetic Operators
        case '+':r.dvalue = va + vb;  break;
        case '-':r.dvalue = va - vb;  break;
        case '*':r.dvalue = va * vb;  break;
        case '/':
            if (vb != 0)
                r.dvalue = va / vb;
            else
                r.type = TYPE_NOT_CONSTANT;
            break;

            //case '%':r.dvalue = va % vb;  break;

                //Relational Operators
        case '==':r.dvalue = va == vb;  break;
        case '!=':r.dvalue = va != vb;  break;
        case '>':r.dvalue = va > vb;  break;
        case '<':r.dvalue = va < vb;  break;
        case '<=':r.dvalue = va <= vb;  break;
        case '>=':r.dvalue = va >= vb;  break;

            //Logical Operators
        case '&&':r.dvalue = va && vb;  break;
        case '||':r.dvalue = va || vb;  break;

            //Bitwise Operators
        //case '|':r.dvalue = va | vb;  break;
        //case '&':r.dvalue = va & vb;  break;
        //case '^':r.dvalue = va ^ vb;  break;
        //case '>>':r.dvalue = va >> vb;  break;
        //case '<<':r.dvalue = va << vb;  break;

        default:
            assert(false);
            break;
        }

        return r;
    }

    if (a->type == TYPE_UNSIGNED_LONG_LONG || b->type == TYPE_UNSIGNED_LONG_LONG)
    {
        unsigned long long va = constant_value_to_ull(a);
        unsigned long long vb = constant_value_to_ull(b);
        r.type = TYPE_UNSIGNED_LONG_LONG;

        switch (op)
        {
            //Arithmetic Operators
        case '+':r.ullvalue = va + vb;  break;
        case '-':r.ullvalue = va - vb;  break;
        case '*':r.ullvalue = va * vb;  break;
        case '/':
            if (vb != 0)
                r.ullvalue = va / vb;
            else
                r.type = TYPE_NOT_CONSTANT;
            break;

        case '%':
            if (vb != 0)
                r.ullvalue = va % vb;
            else
                r.type = TYPE_NOT_CONSTANT;
            break;

            //Relational Operators
        case '==':r.ullvalue = va == vb;  break;
        case '!=':r.ullvalue = va != vb;  break;
        case '>':r.ullvalue = va > vb;  break;
        case '<':r.ullvalue = va < vb;  break;
        case '<=':r.ullvalue = va <= vb;  break;
        case '>=':r.ullvalue = va >= vb;  break;

            //Logical Operators
        case '&&':r.ullvalue = va && vb;  break;
        case '||':r.ullvalue = va || vb;  break;

            //Bitwise Operators
        case '|':r.ullvalue = va | vb;  break;
        case '&':r.ullvalue = va & vb;  break;
        case '^':r.ullvalue = va ^ vb;  break;
        case '>>':r.ullvalue = va >> vb;  break;
        case '<<':r.ullvalue = va << vb;  break;

        default:
            assert(false);
            break;
        }

        return r;
    }

    unsigned long long va = a->llvalue;
    unsigned long long vb = b->llvalue;
    r.type = TYPE_LONG_LONG;
    switch (op)
    {
        //Arithmetic Operators
    case '+':r.llvalue = va + vb;  break;
    case '-':r.llvalue = va - vb;  break;
    case '*':r.llvalue = va * vb;  break;

    case '/':
        if (vb != 0)
            r.llvalue = va / vb;
        else
            r.type = TYPE_NOT_CONSTANT;
        break;

    case '%':
        if (vb != 0)
            r.llvalue = va % vb;
        else
            r.type = TYPE_NOT_CONSTANT;
        break;

        //Relational Operators
    case '==':r.llvalue = va == vb;  break;
    case '!=':r.llvalue = va != vb;  break;
    case '>':r.llvalue = va > vb;  break;
    case '<':r.llvalue = va < vb;  break;
    case '<=':r.llvalue = va <= vb;  break;
    case '>=':r.llvalue = va >= vb;  break;

        //Logical Operators
    case '&&':r.llvalue = va && vb;  break;
    case '||':r.llvalue = va || vb;  break;

        //Bitwise Operators
    case '|':r.llvalue = va | vb;  break;
    case '&':r.llvalue = va & vb;  break;
    case '^':r.llvalue = va ^ vb;  break;
    case '>>':r.llvalue = va >> vb;  break;
    case '<<':r.llvalue = va << vb;  break;

    default:
        assert(false);
        break;
    }


    return r;
}

struct expression* postfix_expression(struct parser_ctx* ctx);
struct expression* cast_expression(struct parser_ctx* ctx);
struct expression* multiplicative_expression(struct parser_ctx* ctx);
struct expression* unary_expression(struct parser_ctx* ctx);
struct expression* additive_expression(struct parser_ctx* ctx);
struct expression* shift_expression(struct parser_ctx* ctx);
struct expression* relational_expression(struct parser_ctx* ctx);
struct expression* equality_expression(struct parser_ctx* ctx);
struct expression* and_expression(struct parser_ctx* ctx);
struct expression* exclusive_or_expression(struct parser_ctx* ctx);
struct expression* inclusive_or_expression(struct parser_ctx* ctx);
struct expression* logical_and_expression(struct parser_ctx* ctx);
struct expression* logical_or_expression(struct parser_ctx* ctx);
struct expression* conditional_expression(struct parser_ctx* ctx);
struct expression* expression(struct parser_ctx* ctx);
struct expression* conditional_expression(struct parser_ctx* ctx);



int  compare_function_arguments(struct parser_ctx* ctx,
    struct type* p_type,
    struct argument_expression_list* p_argument_expression_list)
{
    try
    {




        struct param* current_parameter_type = NULL;

        const struct param_list* p_param_list = type_get_func_or_func_ptr_params(p_type);

        if (p_param_list)
        {
            current_parameter_type = p_param_list->head;
        }


        int param_num = 1;
        struct argument_expression* current_argument =
            p_argument_expression_list->head;

        while (current_argument && current_parameter_type)
        {
            check_function_argument_and_parameter(ctx, current_argument, current_parameter_type->type, param_num);

            struct declarator* arg_declarator = NULL;
            if (current_argument->expression->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
            {
                arg_declarator = current_argument->expression->declarator;
            }
            else if (current_argument->expression->expression_type == UNARY_EXPRESSION_ADDRESSOF)
            {
                struct expression* right = current_argument->expression->right;
                if (right->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
                {
                    arg_declarator = right->declarator;
                }
            }
            else
            {
                arg_declarator = NULL;
            }

            if (arg_declarator &&
                !arg_declarator->is_parameter_declarator)
            {
                if (type_is_pointer(current_parameter_type->type) &&
                    type_has_attribute(current_parameter_type->type->next, CUSTOM_ATTRIBUTE_DESTROY))
                {
                    arg_declarator->static_analisys_flags &= ~MUST_DESTROY;
                    arg_declarator->static_analisys_flags |= UNINITIALIZED;
                }


                if (type_is_pointer(current_parameter_type->type) &&
                    type_has_attribute(current_parameter_type->type->next, CUSTOM_ATTRIBUTE_FREE))
                {
                    arg_declarator->static_analisys_flags &= ~MUST_FREE;
                    arg_declarator->static_analisys_flags |= UNINITIALIZED;
                }

            }


            current_argument = current_argument->next;
            current_parameter_type = current_parameter_type->next;
            param_num++;
        }

        if (current_argument != NULL && !p_param_list->is_var_args)
        {
            compiler_set_error_with_token(ctx,
                p_argument_expression_list->tail->expression->first_token,
                "too many arguments");
            throw;
        }

        if (current_parameter_type != NULL && !p_param_list->is_void)
        {
            if (p_argument_expression_list->tail)
            {
                compiler_set_error_with_token(ctx,
                    p_argument_expression_list->tail->expression->first_token,
                    "too few arguments");
            }
            else
            {
                compiler_set_error_with_token(ctx,
                    ctx->current,
                    "too few arguments");
            }
            throw;
        }
    }
    catch
    {
        return 1; /*error*/
    }

    return 0;
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
        ctx->current->type == TK_KEYWORD__GENERIC;
}

struct generic_association* generic_association(struct parser_ctx* ctx)
{
    struct generic_association* p_generic_association = NULL;
    try
    {
        p_generic_association = calloc(1, sizeof * p_generic_association);
        if (p_generic_association == NULL)
            throw;

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
            p_generic_association->p_type_name = type_name(ctx);
            p_generic_association->type = make_type_using_declarator(ctx, p_generic_association->p_type_name->declarator);
        }
        else
        {
            compiler_set_error_with_token(ctx, ctx->current, "unexpected");
        }
        parser_match_tk(ctx, ':');
        p_generic_association->expression = assignment_expression(ctx);
    }
    catch{
    }

    return p_generic_association;
}

struct generic_assoc_list generic_association_list(struct parser_ctx* ctx)
{
    struct generic_assoc_list list = { 0 };
    try
    {
        struct generic_association* p_generic_association =
            generic_association(ctx);

        if (p_generic_association == NULL) throw;

        LIST_ADD(&list, p_generic_association);

        while (ctx->current->type == ',')
        {
            parser_match(ctx);

            struct generic_association* p_generic_association2 = generic_association(ctx);
            if (p_generic_association2 == NULL) throw;

            LIST_ADD(&list, p_generic_association2);
        }
    }
    catch
    {
    }
    return list;
}

struct generic_selection* generic_selection(struct parser_ctx* ctx)
{
    /*C23
      generic-selection:
        _Generic ( assignment-expression , generic-assoc-ctx )
    */

    /*
      Extension
      generic-selection:
        "_Generic" ( generic-argument, generic-assoc-list )

        generic-argument:
          assignment-expression
          type-name
    */

    struct generic_selection* p_generic_selection = NULL;
    try
    {
        p_generic_selection = calloc(1, sizeof * p_generic_selection);
        if (p_generic_selection == NULL)
            throw;

        p_generic_selection->first_token = ctx->current;



        parser_match_tk(ctx, TK_KEYWORD__GENERIC);
        parser_match_tk(ctx, '(');

        if (first_of_type_name(ctx))
        {
            /*extension*/
            p_generic_selection->type_name = type_name(ctx);
        }
        else
        {
            p_generic_selection->expression = assignment_expression(ctx);
        }

        parser_match_tk(ctx, ',');

        p_generic_selection->generic_assoc_list = generic_association_list(ctx);


        struct type lvalue_type = { 0 };

        struct type* p_type = NULL;

        if (p_generic_selection->expression)
        {
            p_type = &p_generic_selection->expression->type;


            if (expression_is_subjected_to_lvalue_conversion(p_generic_selection->expression))
            {
                lvalue_type = type_lvalue_conversion(&p_generic_selection->expression->type);
                p_type = &lvalue_type;
            }

        }
        else
        {
            p_type = &p_generic_selection->type_name->declarator->type;
        }


        struct generic_association* current = p_generic_selection->generic_assoc_list.head;
        while (current)
        {
            if (current->p_type_name)
            {
                if (type_is_same(p_type, &current->type, true))
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

        p_generic_selection->last_token = ctx->current;
        parser_match_tk(ctx, ')');
        type_destroy(&lvalue_type);
    }
    catch
    {
    }
    return p_generic_selection;
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


//enum token_type parse_number(const char* lexeme, enum type_specifier_flags* flags);

int convert_to_number(struct token* token, struct expression* p_expression_node)
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
    parse_number(buffer, &flags);
    p_expression_node->type.type_specifier_flags = flags;

    switch (token->type)
    {
    case TK_COMPILER_DECIMAL_CONSTANT:

        if (flags & TYPE_SPECIFIER_UNSIGNED)
        {
            p_expression_node->constant_value = make_constant_value_ull(strtoull(buffer, 0, 10));
        }
        else
        {
            p_expression_node->constant_value = make_constant_value_ll(strtoll(buffer, 0, 10));
        }

        break;
    case TK_COMPILER_OCTAL_CONSTANT:
        p_expression_node->constant_value = make_constant_value_ll(strtoll(buffer, 0, 8));

        break;
    case TK_COMPILER_HEXADECIMAL_CONSTANT:
        p_expression_node->constant_value = make_constant_value_ll(strtoll(buffer + 2, 0, 16));

        break;
    case TK_COMPILER_BINARY_CONSTANT:
        p_expression_node->constant_value = make_constant_value_ll(strtoll(buffer + 2, 0, 2));
        break;
    case TK_COMPILER_DECIMAL_FLOATING_CONSTANT:
        p_expression_node->constant_value = make_constant_value_double(strtod(buffer, 0));
        break;
    case TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT:
        p_expression_node->constant_value = make_constant_value_double(strtod(buffer + 2, 0));
        break;
    default:
        assert(false);
    }

    return 0;
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


struct expression* primary_expression(struct parser_ctx* ctx)
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

            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            struct enumerator* p_enumerator = find_enumerator(ctx, ctx->current->lexeme, NULL);
            if (p_enumerator)
            {
                p_expression_node->expression_type = PRIMARY_EXPRESSION_ENUMERATOR;
                p_expression_node->constant_value = make_constant_value_ll(p_enumerator->value);

                p_expression_node->type = type_make_enumerator(p_enumerator->enum_specifier);

            }
            else if (ctx->p_current_function_opt &&
                strcmp(ctx->current->lexeme, "__func__") == 0)
            {
                /*
                   not sure if this is the best way to implement but
                   works for now
                */
                const char* funcname =
                    ctx->p_current_function_opt->init_declarator_list.head->declarator->name->lexeme;

                p_expression_node = calloc(1, sizeof * p_expression_node);
                if (p_expression_node == NULL) throw;

                p_expression_node->expression_type = PRIMARY_EXPRESSION__FUNC__;
                p_expression_node->first_token = ctx->current;
                p_expression_node->last_token = ctx->current;

                p_expression_node->type = type_make_literal_string(strlen(funcname) + 1, TYPE_SPECIFIER_CHAR);
            }
            else
            {
                struct declarator* p_declarator = find_declarator(ctx, ctx->current->lexeme, NULL);
                if (p_declarator == NULL)
                {
                    compiler_set_error_with_token(ctx, ctx->current, "not found '%s'", ctx->current->lexeme);
                    throw;
                }
                else
                {

                    if (type_is_deprecated(&p_declarator->type))
                    {
                        compiler_set_warning_with_token(W_DEPRECATED, ctx, ctx->current, "'%s' is deprecated", ctx->current->lexeme);
                    }

                    p_declarator->num_uses++;
                    p_expression_node->declarator = p_declarator;
                    p_expression_node->expression_type = PRIMARY_EXPRESSION_DECLARATOR;


                    p_expression_node->type = type_dup(&p_declarator->type);
                }
            }
            parser_match(ctx);
        }
        else if (ctx->current->type == TK_STRING_LITERAL)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL) throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_STRING_LITERAL;
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            enum type_specifier_flags char_type = TYPE_SPECIFIER_CHAR;

            if (get_char_type(ctx->current->lexeme) == 2)
            {
                if (sizeof(wchar_t) == 2)
                    char_type = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_SHORT;
                else if (sizeof(wchar_t) == 4)
                    char_type = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT;
            }

            p_expression_node->type = type_make_literal_string(string_literal_byte_size(ctx->current->lexeme), char_type);

            parser_match(ctx);

            /*
            string concatenation deveria ser em uma phase anterior
            mas como mantemos as forma do fonte aqui foi uma alternativa
            */
            while (ctx->current &&
                ctx->current->type == TK_STRING_LITERAL)
            {
                parser_match(ctx);
            }
        }
        else if (ctx->current->type == TK_CHAR_CONSTANT)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL) throw;

            p_expression_node->constant_value = make_constant_value_ll(char_constant_to_int(ctx->current->lexeme));

            p_expression_node->expression_type = PRIMARY_EXPRESSION_CHAR_LITERAL;
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            p_expression_node->type = type_make_int();
            p_expression_node->type.attributes_flags |= CUSTOM_ATTRIBUTE_LIKE_CHAR;

            parser_match(ctx);
        }

        else if (ctx->current->type == TK_KEYWORD_TRUE ||
            ctx->current->type == TK_KEYWORD_FALSE)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL) throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_PREDEFINED_CONSTANT;
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            p_expression_node->constant_value =
                make_constant_value_ll(ctx->current->type == TK_KEYWORD_TRUE ? 1 : 0);

            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_BOOL;
            p_expression_node->type.type_qualifier_flags = 0;



            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD_NULLPTR)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL) throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_PREDEFINED_CONSTANT;
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;

            p_expression_node->constant_value = make_constant_value_ll(0);

            /*TODO nullptr type*/
            p_expression_node->type.type_specifier_flags = TYPE_SPECIFIER_NULLPTR_T;
            p_expression_node->type.type_qualifier_flags = 0;



            parser_match(ctx);
        }
        else if (is_integer_or_floating_constant(ctx->current->type))
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL) throw;

            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;
            p_expression_node->expression_type = PRIMARY_EXPRESSION_NUMBER;
            convert_to_number(ctx->current, p_expression_node);
            parser_match(ctx);
        }
        else if (ctx->current->type == TK_KEYWORD__GENERIC)
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL) throw;

            p_expression_node->expression_type = PRIMARY_EXPRESSION_GENERIC;

            p_expression_node->generic_selection = generic_selection(ctx);
            p_expression_node->first_token = p_expression_node->generic_selection->first_token;
            p_expression_node->last_token = p_expression_node->generic_selection->last_token;

            if (p_expression_node->generic_selection->p_view_selected_expression)
            {
                p_expression_node->type = type_dup(&p_expression_node->generic_selection->p_view_selected_expression->type);

                p_expression_node->constant_value = p_expression_node->generic_selection->p_view_selected_expression->constant_value;
            }
            else
            {
                compiler_set_error_with_token(ctx, ctx->current, "no match for generic");
            }
        }
        else if (ctx->current->type == '(')
        {
            parser_match(ctx);
            p_expression_node = expression(ctx);
            if (p_expression_node == NULL) throw;
            parser_match_tk(ctx, ')');
        }
        else
        {
            compiler_set_error_with_token(ctx, ctx->current, "unexpected");
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



struct argument_expression_list argument_expression_list(struct parser_ctx* ctx)
{
    /*
     argument-expression-list:
      assignment-expression
      argument-expression-ctx , assignment-expression
    */
    struct argument_expression_list list = { 0 };
    struct argument_expression* p_argument_expression = NULL;

    try
    {
        p_argument_expression = calloc(1, sizeof(struct argument_expression));
        if (p_argument_expression == NULL) throw;

        p_argument_expression->expression = assignment_expression(ctx);
        LIST_ADD(&list, p_argument_expression);

        while (ctx->current->type == ',')
        {
            parser_match(ctx);

            struct argument_expression* p_argument_expression_2 = calloc(1, sizeof * p_argument_expression_2);
            if (p_argument_expression_2 == NULL) throw;

            p_argument_expression_2->expression = assignment_expression(ctx);
            if (p_argument_expression_2->expression == NULL) throw;

            LIST_ADD(&list, p_argument_expression_2);
        }
    }
    catch
    {
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


struct expression* postfix_expression_tail(struct parser_ctx* ctx, struct expression* p_expression_node)
{
    try
    {
        while (ctx->current != NULL)
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
                    p_expression_node_new->type = type_remove_pointer(&p_expression_node->type);

                }
                else if (type_is_array(&p_expression_node->type))
                {
                    p_expression_node_new->type = get_array_item_type(&p_expression_node->type);
                }

                parser_match(ctx);
                /*contem a expresao de dentro do  [ ] */
                p_expression_node_new->right = expression(ctx);
                if (p_expression_node_new->right == NULL) throw;

                parser_match_tk(ctx, ']');
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '(')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = POSTFIX_FUNCTION_CALL;
                p_expression_node_new->left = p_expression_node;


                if (!type_is_function_or_function_pointer(&p_expression_node->type))
                {
                    compiler_set_error_with_token(ctx, ctx->current,
                        "called object is not attr function or function pointer");
                    throw;
                }

                p_expression_node_new->type = get_function_return_type(&p_expression_node->type);

                parser_match(ctx);
                if (ctx->current->type != ')')
                {
                    p_expression_node_new->argument_expression_list = argument_expression_list(ctx);
                }
                parser_match_tk(ctx, ')');
                compare_function_arguments(ctx, &p_expression_node->type, &p_expression_node_new->argument_expression_list);
                p_expression_node_new->last_token = ctx->previous;
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
                        find_struct_or_union_specifier(ctx, p_expression_node->type.struct_or_union_specifier->tag_name);
                    p = get_complete_struct_or_union_specifier(p);
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
                            compiler_set_error_with_token(ctx, ctx->current, "struct member '%s' not found in '%s'",
                                ctx->current->lexeme,
                                p_expression_node->type.struct_or_union_specifier->tag_name);
                        }
                    }
                    else
                    {
                        print_scope(&ctx->scopes);
                    }
                    parser_match_tk(ctx, TK_IDENTIFIER);
                }
                else
                {
                    compiler_set_error_with_token(ctx, ctx->current, "structure or union required");
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
                if (type_is_pointer(&p_expression_node->type) &&
                    type_is_struct_or_union(p_expression_node->type.next))
                {
                    struct struct_or_union_specifier* p_complete =
                        get_complete_struct_or_union_specifier(p_expression_node->type.next->struct_or_union_specifier);

                    if (p_complete)
                    {
                        struct member_declarator* p_member_declarator =
                            find_member_declarator(&p_complete->member_declaration_list, ctx->current->lexeme);
                        if (p_member_declarator)
                        {
                            p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                        }
                        else
                        {
                            compiler_set_error_with_token(ctx,
                                ctx->current,
                                "struct member '%s' not found in '%s'",
                                ctx->current->lexeme, p_expression_node->type.struct_or_union_specifier->tag_name);
                        }
                    }
                    else
                    {
                        compiler_set_error_with_token(ctx,
                            ctx->current,
                            "struct '%s' is incomplete.",
                            p_expression_node->type.struct_or_union_specifier->tag_name);
                    }
                    parser_match_tk(ctx, TK_IDENTIFIER);
                }
                else
                {
                    compiler_set_error_with_token(ctx, ctx->current, "structure or union required");
                }
                //todo apontar pro nome?
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '++')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_INCREMENT;
                p_expression_node_new->left = p_expression_node;
                p_expression_node_new->type = type_dup(&p_expression_node->type);
                parser_match(ctx);
                p_expression_node = p_expression_node_new;
            }
            else if (ctx->current->type == '--')
            {
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                p_expression_node_new->expression_type = POSTFIX_DECREMENT;
                p_expression_node_new->left = p_expression_node;
                p_expression_node_new->type = type_dup(&p_expression_node->type);
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

struct expression* postfix_expression_type_name(struct parser_ctx* ctx, struct type_name* p_type_name)
{
    /*
        ( type-name ) { initializer-ctx }
        ( type-name ) { initializer-ctx , }

        //My extension : if type-name is function then follow is compound-statement
        ( type-name ) compound-statement

    */

    struct expression* p_expression_node = NULL;

    try
    {
        p_expression_node = calloc(1, sizeof * p_expression_node);
        if (p_expression_node == NULL) throw;

        assert(p_expression_node->type_name == NULL);

        p_expression_node->first_token = previous_parser_token(p_type_name->first_token);
        assert(p_expression_node->first_token->type == '(');

        p_expression_node->type_name = p_type_name;
        p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);


        if (type_is_function(&p_type_name->declarator->type))
        {
            p_expression_node->expression_type = POSTFIX_EXPRESSION_FUNCTION_LITERAL;

            struct scope* parameters_scope =
                &p_expression_node->type_name->declarator->direct_declarator->function_declarator->parameters_scope;

            scope_list_push(&ctx->scopes, parameters_scope);
            p_expression_node->compound_statement = function_body(ctx);
            scope_list_pop(&ctx->scopes);

        }
        else
        {
            p_expression_node->expression_type = POSTFIX_EXPRESSION_COMPOUND_LITERAL;
            p_expression_node->braced_initializer = braced_initializer(ctx);
        }

        p_expression_node->last_token = ctx->previous;

        p_expression_node = postfix_expression_tail(ctx, p_expression_node);
        if (p_expression_node == NULL) throw;

    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* postfix_expression(struct parser_ctx* ctx)
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
            if (p_expression_node == NULL) throw;

            p_expression_node->first_token = ctx->current;
            parser_match_tk(ctx, '(');
            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == NULL) throw;

            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
            parser_match_tk(ctx, ')');
            //printf("\n");
            //print_type(&p_expression_node->type);
            bool is_function_type = type_is_function(&p_expression_node->type);

            if (is_function_type)
            {
                p_expression_node->expression_type = POSTFIX_EXPRESSION_FUNCTION_LITERAL;
                p_expression_node->compound_statement = compound_statement(ctx);
                if (p_expression_node->compound_statement == NULL) throw;

                p_expression_node->last_token = p_expression_node->compound_statement->last_token;
            }
            else
            {
                p_expression_node->expression_type = POSTFIX_EXPRESSION_COMPOUND_LITERAL;
                p_expression_node->braced_initializer = braced_initializer(ctx);
                p_expression_node->last_token = ctx->current;
            }
        }
        else
        {
            p_expression_node = primary_expression(ctx);
            if (p_expression_node == NULL) throw;
        }

        p_expression_node = postfix_expression_tail(ctx, p_expression_node);
        if (p_expression_node == NULL) throw;
    }
    catch
    {
    }
    return p_expression_node;
}

bool is_first_of_compiler_function(struct parser_ctx* ctx)
{
    if (ctx->current == NULL)
        return false;

    return
        //traits
        ctx->current->type == TK_KEYWORD_IS_CONST ||
        ctx->current->type == TK_KEYWORD_IS_POINTER ||
        ctx->current->type == TK_KEYWORD_IS_ARRAY ||
        ctx->current->type == TK_KEYWORD_IS_FUNCTION ||

        ctx->current->type == TK_KEYWORD_IS_SCALAR ||
        ctx->current->type == TK_KEYWORD_IS_ARITHMETIC ||
        ctx->current->type == TK_KEYWORD_IS_FLOATING_POINT ||
        ctx->current->type == TK_KEYWORD_IS_INTEGRAL ||
        //
        ctx->current->type == TK_KEYWORD_HASHOF ||
        ctx->current->type == TK_KEYWORD_IS_SAME ||
        ctx->current->type == TK_KEYWORD_ATTR_ADD ||
        ctx->current->type == TK_KEYWORD_ATTR_REMOVE ||
        ctx->current->type == TK_KEYWORD_ATTR_HAS;
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
        ctx->current->type == TK_KEYWORD__ALIGNOF ||
        is_first_of_compiler_function(ctx);
}

enum static_analisys_flags string_to_static_analisys_flags(const char* s)
{
    if (strcmp(s, "\"must free\"") == 0)
    {
        return MUST_FREE;
    }
    else if (strcmp(s, "\"must destroy\"") == 0)
    {
        return MUST_DESTROY;
    }
    else if (strcmp(s, "\"uninitialized\"") == 0)
    {
        return UNINITIALIZED;
    }
    return 0;
}

struct expression* declarator_attribute_expression(struct parser_ctx* ctx)
{
    struct expression* new_expression = calloc(1, sizeof * new_expression);
    new_expression->expression_type = UNARY_DECLARATOR_ATTRIBUTE_EXPR;
    new_expression->first_token = ctx->current;
    struct token* func = ctx->current;
    parser_match(ctx);

    parser_match_tk(ctx, '(');


    new_expression->contract_arg_token = ctx->current;


    if (new_expression->contract_arg_token->type == TK_IDENTIFIER)
    {
        new_expression->declarator = find_declarator(ctx,
            new_expression->contract_arg_token->lexeme,
            NULL);

        if (new_expression->declarator == NULL)
        {
            compiler_set_error_with_token(ctx,
                new_expression->contract_arg_token,
                "declarator not found");
        }
        else
        {
            new_expression->declarator->num_uses++;
        }
        parser_match(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_RETURN)
    {
        parser_match(ctx);
    }
    else
    {
        compiler_set_error_with_token(ctx, new_expression->contract_arg_token, "expected declarator name or return");
    }

    parser_match_tk(ctx, ',');

    enum static_analisys_flags attr = 0;
    if (ctx->current->type == TK_STRING_LITERAL)
    {
        attr = string_to_static_analisys_flags(ctx->current->lexeme);



        if (attr != 0)
        {
            new_expression->right = NULL;
        }
        else
        {
            compiler_set_error_with_token(ctx, ctx->current, "unknown attribute name");
        }
        parser_match(ctx);
    }
    else
    {
        new_expression->right = constant_expression(ctx);
        attr = constant_value_to_ull(&new_expression->right->constant_value);
    }
    parser_match_tk(ctx, ')');


    if (new_expression->declarator == NULL ||
        new_expression->declarator->is_parameter_declarator)
    {
    }
    else
    {
        new_expression->declarator->static_analisys_flags |= ISVALID;

        switch (func->type)
        {
        case TK_KEYWORD_ATTR_ADD:

            new_expression->declarator->static_analisys_flags |=
                (unsigned int)(attr);

            new_expression->constant_value = make_constant_value_ull(new_expression->declarator->static_analisys_flags);

            break;
        case TK_KEYWORD_ATTR_REMOVE:
            new_expression->declarator->static_analisys_flags &= ~
                (unsigned int)(attr);

            new_expression->constant_value = make_constant_value_ull(new_expression->declarator->static_analisys_flags);

            break;

        case TK_KEYWORD_ATTR_HAS:
            new_expression->constant_value =
                make_constant_value_ull(new_expression->declarator->static_analisys_flags & (unsigned int)(attr));

            break;
        }
    }

    return new_expression;
}

struct expression* unary_expression(struct parser_ctx* ctx)
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
            new_expression->right = unary_expression(ctx);
            if (new_expression->right == NULL) throw;

            new_expression->type = type_dup(&new_expression->right->type);
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
            new_expression->first_token = ctx->current;


            struct token* op_position = ctx->current; //marcar posicao
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->right = cast_expression(ctx);
            if (new_expression->right == NULL) throw;

            new_expression->last_token = new_expression->right->last_token;
            if (op == '!')
            {
                new_expression->expression_type = UNARY_EXPRESSION_NOT;
                new_expression->constant_value = constant_value_unary_op(&new_expression->right->constant_value, op);

                //same as v == 0

                new_expression->type = type_make_int_bool_like();
            }
            else if (op == '~')
            {
                new_expression->expression_type = UNARY_EXPRESSION_BITNOT;
                new_expression->constant_value = constant_value_unary_op(&new_expression->right->constant_value, op);

                new_expression->type = type_dup(&new_expression->right->type);
            }
            else if (op == '-')
            {
                new_expression->expression_type = UNARY_EXPRESSION_NEG;

                new_expression->constant_value = constant_value_unary_op(&new_expression->right->constant_value, op);

                new_expression->type = type_dup(&new_expression->right->type);
            }
            else if (op == '+')
            {
                new_expression->expression_type = UNARY_EXPRESSION_PLUS;

                new_expression->constant_value = constant_value_unary_op(&new_expression->right->constant_value, op);

                new_expression->type = type_dup(&new_expression->right->type);
            }
            else if (op == '*')
            {
                new_expression->expression_type = UNARY_EXPRESSION_CONTENT;
                if (!type_is_pointer(&new_expression->right->type))
                {
                    compiler_set_error_with_token(ctx, op_position, "indirection requires pointer operand");
                }
                new_expression->type = type_remove_pointer(&new_expression->right->type);
            }
            else if (op == '&')
            {
                new_expression->expression_type = UNARY_EXPRESSION_ADDRESSOF;
                //TODO nao tem como tirar endereco de uma constante
                new_expression->type = type_add_pointer(&new_expression->right->type);
            }
            else
            {
                compiler_set_error_with_token(ctx, ctx->current, "invalid token");
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
                parser_match_tk(ctx, '(');
                new_expression->type_name = type_name(ctx);

                new_expression->type = type_make_int();

                parser_match_tk(ctx, ')');
                new_expression->constant_value = make_constant_value_ll(type_get_sizeof(&new_expression->type_name->declarator->type));

            }
            else
            {
                new_expression->right = unary_expression(ctx);
                if (new_expression->right == NULL) throw;


                new_expression->expression_type = UNARY_EXPRESSION_SIZEOF_EXPRESSION;
                new_expression->constant_value = make_constant_value_ll(type_get_sizeof(&new_expression->right->type));

            }

            new_expression->type = type_make_size_t();
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_ATTR_ADD ||
            ctx->current->type == TK_KEYWORD_ATTR_REMOVE ||
            ctx->current->type == TK_KEYWORD_ATTR_HAS)
        {
            p_expression_node = declarator_attribute_expression(ctx);
            if (p_expression_node == NULL) throw;
        }
        else if (
            ctx->current->type == TK_KEYWORD_IS_CONST ||
            ctx->current->type == TK_KEYWORD_IS_POINTER ||
            ctx->current->type == TK_KEYWORD_IS_ARRAY ||
            ctx->current->type == TK_KEYWORD_IS_FUNCTION ||
            ctx->current->type == TK_KEYWORD_IS_ARITHMETIC ||
            ctx->current->type == TK_KEYWORD_IS_SCALAR ||
            ctx->current->type == TK_KEYWORD_IS_FLOATING_POINT ||
            ctx->current->type == TK_KEYWORD_IS_INTEGRAL)
        {
            struct token* traits_token = ctx->current;

            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->first_token = ctx->current;
            new_expression->expression_type = UNARY_EXPRESSION_TRAITS;

            parser_match(ctx);

            struct type* p_type = NULL;
            if (first_of_type_name_ahead(ctx))
            {
                parser_match_tk(ctx, '(');
                new_expression->type_name = type_name(ctx);
                new_expression->last_token = ctx->current;
                parser_match_tk(ctx, ')');
                p_type = &new_expression->type_name->declarator->type;
            }
            else
            {


                new_expression->right = unary_expression(ctx);
                if (new_expression->right == NULL) throw;

                p_type = &new_expression->right->type;
                new_expression->last_token = ctx->previous;
            }
            switch (traits_token->type)
            {
            case TK_KEYWORD_IS_CONST:
                new_expression->constant_value = make_constant_value_ll(type_is_const(p_type));

                break;

            case TK_KEYWORD_IS_POINTER:
                new_expression->constant_value = make_constant_value_ll(type_is_pointer(p_type));

                break;
            case TK_KEYWORD_IS_FUNCTION:
                new_expression->constant_value = make_constant_value_ll(type_is_function(p_type));

                break;
            case TK_KEYWORD_IS_ARRAY:
                new_expression->constant_value = make_constant_value_ll(type_is_array(p_type));

                break;
            case TK_KEYWORD_IS_ARITHMETIC:
                new_expression->constant_value = make_constant_value_ll(type_is_arithmetic(p_type));

                break;
            case TK_KEYWORD_IS_SCALAR:
                new_expression->constant_value = make_constant_value_ll(type_is_scalar(p_type));

                break;
            case TK_KEYWORD_IS_FLOATING_POINT:
                new_expression->constant_value = make_constant_value_ll(type_is_floating_point(p_type));

                break;
            case TK_KEYWORD_IS_INTEGRAL:
                new_expression->constant_value = make_constant_value_ll(type_is_integer(p_type));

                break;

            default:
                assert(false);

            }


            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_IS_SAME)
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->first_token = ctx->current;
            parser_match(ctx);
            new_expression->expression_type = UNARY_EXPRESSION_IS_SAME;
            parser_match_tk(ctx, '(');
            new_expression->type_name = type_name(ctx);
            parser_match_tk(ctx, ',');
            new_expression->type_name2 = type_name(ctx);
            parser_match_tk(ctx, ')');

            new_expression->constant_value =
                make_constant_value_ll(
                    type_is_same(&new_expression->type_name->declarator->type,
                        &new_expression->type_name2->declarator->type, true));


            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD_HASHOF)
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            new_expression->first_token = ctx->current;
            new_expression->expression_type = UNARY_EXPRESSION_HASHOF_TYPE;

            parser_match(ctx);

            if (first_of_type_name_ahead(ctx))
            {
                parser_match_tk(ctx, '(');
                new_expression->type_name = type_name(ctx);
                new_expression->last_token = ctx->current;
                parser_match_tk(ctx, ')');
                new_expression->constant_value = make_constant_value_ull(type_get_hashof(ctx, &new_expression->type));

            }
            else
            {
                new_expression->right = unary_expression(ctx);
                if (new_expression->right == NULL)
                    throw;

                new_expression->constant_value = make_constant_value_ull(type_get_hashof(ctx, &new_expression->right->type));


                new_expression->last_token = ctx->previous;
            }


            new_expression->type = type_make_int();
            p_expression_node = new_expression;
        }
        else if (ctx->current->type == TK_KEYWORD__ALIGNOF)
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);

            new_expression->expression_type = UNARY_EXPRESSION_ALIGNOF;
            new_expression->first_token = ctx->current;

            parser_match(ctx);
            parser_match_tk(ctx, '(');
            new_expression->type_name = type_name(ctx);
            new_expression->type = make_type_using_declarator(ctx, new_expression->type_name->declarator);
            parser_match_tk(ctx, ')');

            new_expression->constant_value = make_constant_value_ll(type_get_alignof(&new_expression->type));

            new_expression->type = type_make_int();
            p_expression_node = new_expression;
            new_expression->last_token = ctx->previous;
        }
        else if (ctx->current->type == TK_KEYWORD__ALIGNAS)
        {
        }
        else //if (is_first_of_primary_expression(ctx))
        {
            p_expression_node = postfix_expression(ctx);
            if (p_expression_node == NULL) throw;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* cast_expression(struct parser_ctx* ctx)
{
    /*
     cast-expression:
      unary-expression
      ( type-name ) cast-expression


      ( type-name ) //<- extension void value
    */
    struct expression* p_expression_node = NULL;
    try
    {
        if (first_of_type_name_ahead(ctx))
        {
            p_expression_node = calloc(1, sizeof * p_expression_node);
            if (p_expression_node == NULL) throw;

            p_expression_node->first_token = ctx->current;
            p_expression_node->expression_type = CAST_EXPRESSION;
            parser_match_tk(ctx, '(');
            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == NULL)
                throw;


            p_expression_node->type = type_dup(&p_expression_node->type_name->type);

            //type_set_int(&ctx->result_type);
            //print_type_name(p_cast_expression->type_name);
            parser_match_tk(ctx, ')');
            //struct token_list r = copy_replacement_list(&l);
            //pop_f
            if (ctx->current->type == '{')
            {
                // Achar que era um cast_expression foi um engano...
                // porque apareceu o { então é compound literal que eh postfix.
                struct expression* new_expression = postfix_expression_type_name(ctx, p_expression_node->type_name);


                free(p_expression_node);
                p_expression_node = new_expression;
            }
            else if (is_first_of_unary_expression(ctx))
            {
                p_expression_node->left = cast_expression(ctx);
                if (p_expression_node->left == NULL) throw;
                p_expression_node->constant_value = p_expression_node->left->constant_value;
                p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
                if (type_is_floating_point(&p_expression_node->type))
                {
                    p_expression_node->constant_value =
                        constant_value_cast(&p_expression_node->constant_value, TYPE_DOUBLE);
                }
                else if (type_is_integer(&p_expression_node->type))
                {
                    if (type_is_unsigned_integer(&p_expression_node->type))
                    {
                        p_expression_node->constant_value =
                            constant_value_cast(&p_expression_node->constant_value, TYPE_UNSIGNED_LONG_LONG);
                    }
                    else
                    {
                        p_expression_node->constant_value =
                            constant_value_cast(&p_expression_node->constant_value, TYPE_LONG_LONG);
                    }
                }
            }
            else
            {
                p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->declarator);
                p_expression_node->constant_value.type = TYPE_EMPTY;
            }
        }
        else if (is_first_of_unary_expression(ctx))
        {
            p_expression_node = unary_expression(ctx);
            if (p_expression_node == NULL) throw;
        }
        else
        {
            compiler_set_error_with_token(ctx, ctx->current, "unexpected");
        }
    }
    catch
    {
    }
    if (p_expression_node && ctx->current)
    {
        p_expression_node->last_token = ctx->previous;
    }
    return p_expression_node;
}

struct expression* multiplicative_expression(struct parser_ctx* ctx)
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
        p_expression_node = cast_expression(ctx);
        if (p_expression_node == NULL) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '*' ||
                ctx->current->type == '/' ||
                ctx->current->type == '%'))
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->left = p_expression_node;
            new_expression->right = cast_expression(ctx);

            if (new_expression->left == NULL ||
                new_expression->right == NULL)
            {
                throw;
            }

            if (op == '*')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MULT;


                if (!type_is_arithmetic(&new_expression->left->type))
                {
                    compiler_set_error_with_token(ctx, ctx->current, "left * is not arithmetic");

                }
                if (!type_is_arithmetic(&new_expression->right->type))
                {
                    compiler_set_error_with_token(ctx, ctx->current, "right * is not arithmetic");
                }

            }
            else if (op == '/')
            {

                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_DIV;

                new_expression->constant_value =
                    constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '/');

                if (constant_value_is_valid(&new_expression->right->constant_value) &&
                    constant_value_to_ll(&new_expression->right->constant_value) == 0)
                {
                    compiler_set_error_with_token(ctx, ctx->current, "divizion by zero");
                }

                if (!type_is_arithmetic(&new_expression->left->type))
                {
                    compiler_set_error_with_token(ctx, ctx->current, "left / is not arithmetic");

                }
                if (!type_is_arithmetic(&new_expression->right->type))
                {
                    compiler_set_error_with_token(ctx, ctx->current, "right / is not arithmetic");
                }
            }
            else if (op == '%')
            {
                new_expression->expression_type = MULTIPLICATIVE_EXPRESSION_MOD;


                if (!type_is_integer(&new_expression->left->type))
                {
                    compiler_set_error_with_token(ctx, ctx->current, "left is not integer");

                }
                if (!type_is_integer(&new_expression->right->type))
                {
                    compiler_set_error_with_token(ctx, ctx->current, "right is not integer");
                }

                if (constant_value_is_valid(&new_expression->right->constant_value) &&
                    constant_value_to_ll(&new_expression->right->constant_value) == 0)
                {
                    compiler_set_error_with_token(ctx, ctx->current, "divizion by zero");
                }
            }

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, op);


            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                compiler_set_error_with_token(ctx, ctx->current, "invalid type multiplicative expression");
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

struct expression* additive_expression(struct parser_ctx* ctx)
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
        p_expression_node = multiplicative_expression(ctx);
        if (p_expression_node == NULL) throw;


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
            new_expression->right = multiplicative_expression(ctx);
            if (new_expression->right == NULL) throw;

            if (!type_is_scalar(&new_expression->left->type))
            {
                compiler_set_error_with_token(ctx, operator_position, "left operator is not scalar");
            }
            if (!type_is_scalar(&new_expression->right->type))
            {
                compiler_set_error_with_token(ctx, operator_position, "right operator is not scalar");
            }


            const bool b_left_is_arithmetic = type_is_arithmetic(&new_expression->left->type);
            const bool b_right_is_arithmetic = type_is_arithmetic(&new_expression->right->type);

            const enum type_category left_category = type_get_category(&new_expression->left->type);
            const enum type_category right_category = type_get_category(&new_expression->right->type);

            if (op == '+')
            {
                new_expression->expression_type = ADDITIVE_EXPRESSION_PLUS;


                /*
                 For addition, either both operands shall have arithmetic type,
                 or one operand shall be a pointer to a complete object type and
                 the other shall have integer type. (Incrementing is equivalent to adding 1.)
                */
                if (b_left_is_arithmetic && b_right_is_arithmetic)
                {
                    int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
                    if (code != 0)
                    {
                        compiler_set_error_with_token(ctx, ctx->current, "internal error");
                        throw;
                    }
                }
                else
                {
                    if (left_category == TYPE_CATEGORY_POINTER || left_category == TYPE_CATEGORY_ARRAY  /* || left_category == TYPE_CATEGORY_FUNCTION*/)
                    {
                        if (type_is_integer(&new_expression->right->type))
                        {
                            if (left_category == TYPE_CATEGORY_ARRAY)
                            {
                                struct type t = get_array_item_type(&new_expression->left->type);
                                new_expression->type = type_add_pointer(&t);
                                type_destroy(&t);
                            }
                            else
                            {
                                new_expression->type = type_dup(&new_expression->left->type);
                            }
                        }
                        else
                        {
                            compiler_set_error_with_token(ctx, ctx->current, "expected integer type on right");
                        }
                    }
                    else if (right_category == TYPE_CATEGORY_POINTER || right_category == TYPE_CATEGORY_ARRAY /*|| right_category == TYPE_CATEGORY_FUNCTION*/)
                    {
                        if (type_is_integer(&new_expression->left->type))
                        {
                            if (right_category == TYPE_CATEGORY_ARRAY)
                            {
                                new_expression->type = get_array_item_type(&new_expression->right->type);
                            }
                            else
                            {
                                new_expression->type = type_dup(&new_expression->right->type);
                            }
                        }
                        else
                        {
                            compiler_set_error_with_token(ctx, ctx->current, "expected integer type on left");
                        }
                    }
                    else
                    {
                        compiler_set_error_with_token(ctx, ctx->current, "invalid types additive expression");
                    }
                }
            }
            else if (op == '-')
            {
                new_expression->expression_type = ADDITIVE_EXPRESSION_MINUS;


                /*
                 For subtraction, one of the following shall hold:
                    — both operands have arithmetic type;
                    — both operands are pointers to qualified or unqualified versions of compatible complete object
                    types; or
                    — the left operand is a pointer to a complete object type and the right operand has integer type.
                    (Decrementing is equivalent to subtracting 1.)
                */
                if (b_left_is_arithmetic && b_right_is_arithmetic)
                {
                    //— both operands have arithmetic type;
                    int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
                    if (code != 0)
                    {
                        compiler_set_error_with_token(ctx, ctx->current, "internal error type_common");
                        throw;
                    }
                }
                else
                {
                    if (left_category == TYPE_CATEGORY_POINTER || left_category == TYPE_CATEGORY_ARRAY)
                    {
                        if (right_category == TYPE_CATEGORY_POINTER || right_category == TYPE_CATEGORY_ARRAY)
                        {
                            //— both operands are pointers to qualified or unqualified versions of compatible complete object
                            // types;
                            struct type t1 = type_lvalue_conversion(&new_expression->left->type);
                            struct type t2 = type_lvalue_conversion(&new_expression->right->type);

                            if (!type_is_same(&t1, &t2, false))
                            {
                                compiler_set_error_with_token(ctx, ctx->current, "incompatible pointer types");
                            }

                            new_expression->type = type_make_int();
                            type_destroy(&t1);
                            type_destroy(&t2);
                        }
                        else
                        {
                            if (type_is_integer(&new_expression->right->type))
                            {
                                //- the left operand is a pointer to a complete object typeand the right operand has integer type.
                                new_expression->type = type_dup(&new_expression->left->type);
                            }
                            else
                            {
                                compiler_set_error_with_token(ctx, ctx->current, "right must be integer type");
                            }
                        }
                    }
                    else
                    {
                        compiler_set_error_with_token(ctx, ctx->current, "invalid types for operator -");
                    }
                }
            }

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, op);


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

struct expression* shift_expression(struct parser_ctx* ctx)
{
    /*
     shift-expression:
       additive-expression
       shift-expression << additive-expression
       shift-expression >> additive-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = additive_expression(ctx);
        if (p_expression_node == NULL) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '>>' ||
                ctx->current->type == '<<'))
        {
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->left = p_expression_node;
            new_expression->right = multiplicative_expression(ctx);
            if (new_expression->left == NULL || new_expression->right == NULL)
            {
                throw;
            }

            if (op == '>>')
            {
                new_expression->expression_type = SHIFT_EXPRESSION_RIGHT;
            }
            else if (op == '<<')
            {
                new_expression->expression_type = SHIFT_EXPRESSION_LEFT;

            }

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, op);


            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                compiler_set_error_with_token(ctx, ctx->current, "invalid type shift expression");
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

struct expression* relational_expression(struct parser_ctx* ctx)
{
    /*
    relational-expression:
        shift-expression
        relational-expression < shift-expression
        relational-expression > shift-expression
        relational-expression <= shift-expression
        relational-expression >= shift-expression
    */

    struct expression* p_expression_node = NULL;
    struct expression* new_expression = NULL;
    try
    {
        p_expression_node = shift_expression(ctx);
        if (p_expression_node == NULL) throw;


        while (ctx->current != NULL &&
            (ctx->current->type == '>' ||
                ctx->current->type == '<' ||
                ctx->current->type == '>=' ||
                ctx->current->type == '<='))
        {
            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            enum token_type op = ctx->current->type;
            parser_match(ctx);
            new_expression->left = p_expression_node;

            new_expression->right = shift_expression(ctx);
            if (new_expression->right == NULL) throw;

            if (op == '>')
            {
                new_expression->expression_type = RELATIONAL_EXPRESSION_BIGGER_THAN;

            }
            else if (op == '<')
            {
                new_expression->expression_type = RELATIONAL_EXPRESSION_LESS_THAN;
            }
            else if (op == '>=')
            {
                new_expression->expression_type = RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN;
            }
            else if (op == '<=')
            {
                new_expression->expression_type = RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN;
            }

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, op);

            new_expression->type = type_make_int_bool_like();

            p_expression_node = new_expression;
            new_expression = NULL;/*MOVED*/
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* equality_expression(struct parser_ctx* ctx)
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
    struct expression* p_expression_node = NULL;
    struct expression* new_expression = NULL;

    try
    {
        p_expression_node = relational_expression(ctx);
        if (p_expression_node == NULL) throw;


        while (ctx->current != NULL &&
            (ctx->current->type == '==' ||
                ctx->current->type == '!='))
        {
            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            struct  token* operator_token = ctx->current;
            parser_match(ctx);
            new_expression->left = p_expression_node;
            new_expression->right = relational_expression(ctx);
            if (new_expression->right == NULL) throw;

            if (new_expression->left->type.type_specifier_flags & TYPE_SPECIFIER_ENUM &&
                new_expression->right->type.type_specifier_flags & TYPE_SPECIFIER_ENUM)
            {
                if (new_expression->left->type.enum_specifier->complete_enum_specifier !=
                    new_expression->right->type.enum_specifier->complete_enum_specifier)
                {
                    const char* lefttag = "";
                    if (new_expression->left->type.enum_specifier->tag_token)
                        lefttag = new_expression->left->type.enum_specifier->tag_token->lexeme;

                    const char* righttag = "";
                    if (new_expression->right->type.enum_specifier->tag_token)
                        righttag = new_expression->right->type.enum_specifier->tag_token->lexeme;

                    /*
                     * This comparison by name is not 100% correct because they be from
                     * diferent scopes.
                    */

                    compiler_set_warning_with_token(W_ENUN_COMPARE,
                        ctx,
                        operator_token,
                        "comparison between 'enum %s' and 'enum %s'",
                        lefttag,
                        righttag);
                }
            }

            new_expression->first_token = operator_token;

            if (operator_token->type == '==')
            {
                new_expression->expression_type = EQUALITY_EXPRESSION_EQUAL;
                new_expression->constant_value =
                    constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '==');

                if (new_expression->left->constant_value.type == TYPE_EMPTY ||
                    new_expression->right->constant_value.type == TYPE_EMPTY)
                {
                    new_expression->constant_value =
                        make_constant_value_ll(type_is_same(&new_expression->left->type, &new_expression->right->type, true));
                }
            }
            else if (operator_token->type == '!=')
            {
                new_expression->expression_type = EQUALITY_EXPRESSION_EQUAL;

                new_expression->constant_value =
                    constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '!=');


                if (new_expression->left->constant_value.type == TYPE_EMPTY ||
                    new_expression->right->constant_value.type == TYPE_EMPTY)
                {
                    new_expression->constant_value = make_constant_value_ll
                    (!type_is_same(&new_expression->left->type, &new_expression->right->type, true));

                }
            }
            else
            {
                assert(false);
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
            new_expression = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* and_expression(struct parser_ctx* ctx)
{
    /*
     AND-expression:
      equality-expression
      AND-expression & equality-expression
    */
    struct expression* p_expression_node = NULL;
    struct expression* new_expression = NULL;

    try
    {
        p_expression_node = equality_expression(ctx);
        if (p_expression_node == NULL)
            throw;

        while (ctx->current != NULL && ctx->current->type == '&')
        {
            parser_match(ctx);

            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->expression_type = AND_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = equality_expression(ctx);
            if (new_expression->right == NULL) throw;

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '&');


            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                compiler_set_error_with_token(ctx, ctx->current, "invalid types and expression");
                throw;
            }

            p_expression_node = new_expression;
            new_expression = NULL; /*MOVED*/
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* exclusive_or_expression(struct parser_ctx* ctx)
{
    /*
     exclusive-OR-expression:
      AND-expression
      exclusive-OR-expression ^ AND-expression
    */
    struct expression* p_expression_node = NULL;
    struct expression* new_expression = NULL;

    try
    {
        p_expression_node = and_expression(ctx);
        if (p_expression_node == NULL) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '^'))
        {
            parser_match(ctx);

            assert(new_expression == NULL);
            new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->expression_type = EXCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = and_expression(ctx);
            if (new_expression->right == NULL) throw;

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '^');


            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                compiler_set_error_with_token(ctx, ctx->current, "invalid types or expression");
                throw;
            }

            p_expression_node = new_expression;
            new_expression = NULL;
        }
    }
    catch
    {
    }

    return p_expression_node;
}

struct expression* inclusive_or_expression(struct parser_ctx* ctx)
{

    /*
    inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = exclusive_or_expression(ctx);
        if (p_expression_node == NULL) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '|'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->expression_type = INCLUSIVE_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = exclusive_or_expression(ctx);
            if (new_expression->right == NULL) throw;

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '|');



            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                compiler_set_error_with_token(ctx, ctx->current, "invalid types inclusive or expression");
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

struct expression* logical_and_expression(struct parser_ctx* ctx)
{
    /*
    logical-AND-expression:
     inclusive-OR-expression
     logical-AND-expression && inclusive-OR-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = inclusive_or_expression(ctx);
        if (p_expression_node == NULL) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '&&'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->expression_type = INCLUSIVE_AND_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = inclusive_or_expression(ctx);
            if (new_expression->right == NULL) throw;

            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '&&');


            int code = type_common(&new_expression->left->type, &new_expression->right->type, &new_expression->type);
            if (code != 0)
            {
                type_print(&new_expression->left->type);
                type_print(&new_expression->right->type);
                compiler_set_error_with_token(ctx, ctx->current, "invalid types logicl and expression");
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

struct expression* logical_or_expression(struct parser_ctx* ctx)
{
    /*
      logical-OR-expression:
       logical-AND-expression
       logical-OR-expression || logical-AND-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = logical_and_expression(ctx);
        if (p_expression_node == NULL) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '||'))
        {
            parser_match(ctx);
            struct expression* new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->expression_type = LOGICAL_OR_EXPRESSION;
            new_expression->left = p_expression_node;
            new_expression->right = logical_and_expression(ctx);
            if (new_expression->right == NULL) throw;


            new_expression->constant_value =
                constant_value_op(&new_expression->left->constant_value, &new_expression->right->constant_value, '||');


            if (!type_is_scalar(&new_expression->left->type)) {
                compiler_set_error_with_token(ctx, ctx->current, "left type is not scalar for or expression");
                throw;
            }

            if (!type_is_scalar(&new_expression->right->type)) {
                compiler_set_error_with_token(ctx, ctx->current, "right type is not scalar for or expression");
                throw;
            }

            new_expression->type = type_make_int_bool_like();

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }

    return p_expression_node;
}



struct expression* assignment_expression(struct parser_ctx* ctx)
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
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = conditional_expression(ctx);
        if (p_expression_node == NULL) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '=' ||
                ctx->current->type == '*=' ||
                ctx->current->type == '/=' ||
                ctx->current->type == '%=' ||
                ctx->current->type == '+=' ||
                ctx->current->type == '-=' ||
                ctx->current->type == '<<=' ||
                ctx->current->type == '>>=' ||
                ctx->current->type == '&=' ||
                ctx->current->type == '^=' ||
                ctx->current->type == '|='))
        {
            parser_match(ctx);

            struct expression* new_expression = calloc(1, sizeof * new_expression);
            if (new_expression == NULL) throw;

            new_expression->expression_type = ASSIGNMENT_EXPRESSION;
            new_expression->left = p_expression_node;

            enum type_category category =
                type_get_category(&new_expression->left->type);

            if (category == TYPE_CATEGORY_FUNCTION)
            {
                compiler_set_error_with_token(ctx, ctx->current, "assignment of function");
            }
            else if (category == TYPE_CATEGORY_ARRAY)
            {
                compiler_set_error_with_token(ctx, ctx->current, "assignment to expression with array type");
            }
            else {

                if (type_is_const(&new_expression->left->type))
                {
                    compiler_set_error_with_token(ctx, ctx->current, "assignment of read-only object");
                }
            }

            new_expression->right = assignment_expression(ctx);
            if (new_expression->right == NULL) throw;

            if (ctx->current->type == '=')
            {
                check_assigment(ctx, &new_expression->left->type, new_expression->right);
            }

            if (new_expression->left->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
            {
                /*let's remove the UNINITIALIZED flag*/
                new_expression->left->declarator->static_analisys_flags &=
                    ~UNINITIALIZED;


                if (new_expression->right->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
                {
                    /*let's remove the UNINITIALIZED flag*/
                    if (new_expression->right->declarator->static_analisys_flags & MUST_DESTROY)
                    {
                        new_expression->left->declarator->static_analisys_flags |= MUST_DESTROY;
                        new_expression->right->declarator->static_analisys_flags &= ~MUST_DESTROY;
                    }

                    if (new_expression->right->declarator->static_analisys_flags & MUST_FREE)
                    {
                        new_expression->left->declarator->static_analisys_flags |= MUST_FREE;
                        new_expression->right->declarator->static_analisys_flags &= ~MUST_FREE;
                    }

                    new_expression->right->declarator->static_analisys_flags |= UNINITIALIZED;

                    if (new_expression->right->declarator->static_analisys_flags & UNINITIALIZED)
                    {
                        //TODO fix uninitialized value
                        //parser_setwarning_with_token(ctx, ctx->current, "using uninitialized value");
                    }
                }


            }



            new_expression->type = type_dup(&new_expression->right->type);

            p_expression_node = new_expression;
        }
    }
    catch
    {
    }
    return p_expression_node;
}

struct expression* expression(struct parser_ctx* ctx)
{
    /*expression:
      assignment-expression
      expression, assignment-expression
    */
    struct expression* p_expression_node = NULL;
    try
    {
        p_expression_node = assignment_expression(ctx);
        if (p_expression_node == NULL) throw;

        if (ctx->current->type == ',')
        {
            while (ctx->current->type == ',')
            {
                parser_match(ctx);
                struct expression* p_expression_node_new = calloc(1, sizeof * p_expression_node_new);
                if (p_expression_node_new == NULL) throw;

                p_expression_node_new->expression_type = ASSIGNMENT_EXPRESSION;
                p_expression_node_new->left = p_expression_node;

                p_expression_node_new->right = expression(ctx);
                if (p_expression_node_new->right == NULL) throw;

                p_expression_node = p_expression_node_new;
            }

            if (p_expression_node->right == NULL) throw;

            /*same type of the last expression*/
            p_expression_node->type = type_dup(&p_expression_node->right->type);
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

bool is_null_pointer_constant(const struct expression* expression)
{
    if (expression)
    {
        return constant_value_is_valid(&expression->constant_value) &&
            constant_value_to_bool(&expression->constant_value) == 0;
    }
    return false;
}

struct expression* conditional_expression(struct parser_ctx* ctx)
{
    /*
      conditional-expression:
      logical-OR-expression
      logical-OR-expression ? expression : conditional-expression
    */
    struct expression* p_expression_node = NULL;
    struct type left_type = { 0 };
    struct type right_type = { 0 };
    try
    {
        p_expression_node = logical_or_expression(ctx);
        if (p_expression_node == NULL) throw;


        if (ctx->current && ctx->current->type == '?')
        {
            struct expression* p_conditional_expression = calloc(1, sizeof * p_conditional_expression);
            p_conditional_expression->expression_type = CONDITIONAL_EXPRESSION;
            p_conditional_expression->condition_expr = p_expression_node;
            p_expression_node = p_conditional_expression;

            parser_match(ctx); //?

            p_conditional_expression->left = expression(ctx);
            if (p_conditional_expression->left == NULL) throw;

            parser_match(ctx); //:
            p_conditional_expression->right = conditional_expression(ctx);
            if (p_conditional_expression->right == NULL) throw;


            if (constant_value_is_valid(&p_conditional_expression->condition_expr->constant_value))
            {
                if (constant_value_to_bool(&p_conditional_expression->condition_expr->constant_value))
                {
                    /*this is an extensions.. in constant expression we can mix types!*/
                    p_conditional_expression->type = type_dup(&p_conditional_expression->left->type);
                    p_conditional_expression->constant_value = p_conditional_expression->left->constant_value;
                    goto continuation;
                }
                else
                {
                    /*this is an extensions.. in constant expression we can mix types!*/
                    p_conditional_expression->type = type_dup(&p_conditional_expression->right->type);
                    p_conditional_expression->constant_value = p_conditional_expression->right->constant_value;
                    goto continuation;
                }
            }

            if (expression_is_subjected_to_lvalue_conversion(p_conditional_expression->left))
            {
                left_type = type_lvalue_conversion(&p_conditional_expression->left->type);
            }
            else
            {
                left_type = type_dup(&p_conditional_expression->left->type);
            }

            if (expression_is_subjected_to_lvalue_conversion(p_conditional_expression->right))
            {
                right_type = type_lvalue_conversion(&p_conditional_expression->right->type);
            }
            else
            {
                right_type = type_dup(&p_conditional_expression->right->type);
            }

            /*The first operand shall have scalar type*/
            if (!type_is_scalar(&p_conditional_expression->condition_expr->type))
            {
                compiler_set_error_with_token(ctx, ctx->current, "condition must have scalar type");
                goto continuation;
            }

            /*
            *  both operands have arithmetic type;
            */
            if (type_is_arithmetic(&left_type) &&
                type_is_arithmetic(&right_type))
            {
                type_common(&left_type, &right_type, &p_conditional_expression->type);
                goto continuation;
            }

            /*
            *  both operands have compatible structure or union type;
            */
            if (type_is_struct_or_union(&left_type) !=
                type_is_struct_or_union(&right_type))
            {
                if (!type_is_same(&left_type, &right_type, true))
                {
                    compiler_set_error_with_token(ctx, p_conditional_expression->condition_expr->first_token, "incompatible types");
                }
                type_swap(&p_conditional_expression->type, &right_type);
                goto continuation;
            }

            /*
             *  both operands have void type;
            */
            if (type_is_void(&left_type) && type_is_void(&right_type))
            {
                type_swap(&p_conditional_expression->type, &right_type);
                goto continuation;
            }


            /* both operands have nullptr_t type;*/
            if (type_is_nullptr_t(&left_type) && type_is_nullptr_t(&right_type))
            {
                type_swap(&p_conditional_expression->type, &right_type);
                goto continuation; //ok
            }

            /*
                — one operand is a pointer and the other is a null pointer constant or has type nullptr_t; or
                    — one operand is a pointer to an object type and the other is a pointer to a qualified or unqualified
                    version of void
            */
            if (type_is_pointer(&left_type))
            {
                if (is_null_pointer_constant(p_conditional_expression->right) ||
                    type_is_nullptr_t(&right_type) ||
                    type_is_void_ptr(&right_type))
                {
                    type_swap(&p_conditional_expression->type, &left_type);
                    goto continuation; //ok
                }

                if (type_is_pointer(&right_type))
                {
                    if (type_is_nullptr_t(&left_type) || type_is_void_ptr(&left_type))
                    {
                        type_swap(&p_conditional_expression->type, &left_type);
                        goto continuation; //ok
                    }

                    if (!type_is_same(&left_type, &right_type, false))
                    {
                        type_print(&left_type);
                        type_print(&right_type);
                        compiler_set_error_with_token(ctx, ctx->current, "incompatible types");
                        goto continuation;
                    }
                    type_swap(&p_conditional_expression->type, &right_type);
                    goto continuation;
                }

                compiler_set_error_with_token(ctx, p_conditional_expression->condition_expr->first_token, "incompatible types");
                goto continuation;
            }

            if (type_is_pointer(&right_type))
            {
                if (is_null_pointer_constant(p_conditional_expression->left) ||
                    type_is_nullptr_t(&left_type) ||
                    type_is_void_ptr(&left_type))
                {
                    type_swap(&p_conditional_expression->type, &right_type);
                    goto continuation; //ok
                }

                if (type_is_pointer(&left_type))
                {
                    if (type_is_nullptr_t(&left_type) || type_is_void_ptr(&left_type))
                    {
                        type_swap(&p_conditional_expression->type, &right_type);
                        goto continuation; //ok
                    }

                    if (!type_is_same(&left_type, &right_type, false))
                    {
                        compiler_set_error_with_token(ctx, p_conditional_expression->condition_expr->first_token, "incompatible types");
                        goto continuation;
                    }
                    type_swap(&p_conditional_expression->type, &right_type);
                    goto continuation;
                }

                compiler_set_error_with_token(ctx, p_conditional_expression->condition_expr->first_token, "incompatible types");
                goto continuation;
            }
        continuation:;
        }
    }
    catch
    {
    }

    type_destroy(&left_type);
    type_destroy(&right_type);

    return p_expression_node;
}

struct expression* constant_expression(struct parser_ctx* ctx)
{
    struct expression* p_expression = conditional_expression(ctx);


    if (p_expression && !constant_value_is_valid(&p_expression->constant_value))
    {
        compiler_set_error_with_token(ctx, ctx->current, "expected constant expression");
    }

    return p_expression;
}

/*
* Returns true if the type of expression is subjected to type_lvalue_conversion
*/
bool expression_is_subjected_to_lvalue_conversion(struct expression* expression)
{

    switch (expression->expression_type)
    {
    case UNARY_EXPRESSION_ADDRESSOF:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:
    case POSTFIX_INCREMENT:
    case POSTFIX_DECREMENT:
        return false;
    }

    if (expression->type.attributes_flags & CUSTOM_ATTRIBUTE_PARAM)
        return true;

    return true;
}


/*
  For performance reasons we will separate expression from preprocessor from compiler.
*/

#ifdef _WIN32
#endif

#if defined _MSC_VER && !defined __POCC__
#endif


/*contexto expressoes preprocessador*/
struct pre_expression_ctx
{
    /*todas expressões do preprocessador sao calculadas com long long*/
    long long value;
};


void pre_postfix_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_cast_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_multiplicative_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_unary_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_additive_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_shift_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_relational_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_equality_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_and_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_exclusive_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_inclusive_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_logical_and_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_logical_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_conditional_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);
void pre_conditional_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx);



/*
* preprocessor uses long long
*/
int ppnumber_to_longlong(struct token* token, long long* result)
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

    
    return 0;
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

void pre_primary_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
            ppnumber_to_longlong(ctx->current, &ectx->value);
            pre_match(ctx);
        }
        else if (ctx->current->type == '(')
        {
            pre_match(ctx);
            pre_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;
            if (ctx->current && ctx->current->type != ')')
            {
                preprocessor_set_error_with_token(ctx, ctx->current, "expected )");
                throw;
            }
            pre_match(ctx);
        }
        else
        {
            preprocessor_set_error_with_token(ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            throw;

        }
    }
    catch
    {
    }
}




void pre_postfix_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        pre_primary_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;
    }
    catch
    {
    }
}


void pre_unary_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
            preprocessor_set_error_with_token(ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
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
            pre_cast_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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
                preprocessor_set_error_with_token(ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            }
            else if (op == '&')
            {
                preprocessor_set_error_with_token(ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            }
            else
            {
                preprocessor_set_error_with_token(ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            }
        }
        else 
        {
            pre_postfix_expression(ctx,  ectx);
        }
    }
    catch
    {
    }
}

void pre_cast_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     cast-expression:
      unary-expression
      ( type-name ) cast-expression
    */
    pre_unary_expression(ctx,  ectx);    
}

void pre_multiplicative_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        pre_cast_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '*' ||
                ctx->current->type == '/' ||
                ctx->current->type == '%'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_cast_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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

void pre_additive_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     additive-expression:
       multiplicative-expression
       additive-expression + multiplicative-expression
       additive-expression - multiplicative-expression
    */
    try
    {
        pre_multiplicative_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '+' ||
                ctx->current->type == '-'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            if (ctx->current == NULL)
            {
                preprocessor_set_error_with_token(ctx, ctx->current, "unexpected end of file");
                throw;
            }
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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

void pre_shift_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     shift-expression:
       additive-expression
       shift-expression << additive-expression
       shift-expression >> additive-expression
    */
    try
    {
        pre_additive_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '>>' ||
                ctx->current->type == '<<'))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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

void pre_relational_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        pre_shift_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '>' ||
                ctx->current->type == '<' ||
                ctx->current->type == '>=' ||
                ctx->current->type == '<='))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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

void pre_equality_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        pre_relational_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '==' ||
                ctx->current->type == '!='))
        {
            enum token_type op = ctx->current->type;
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_multiplicative_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

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

void pre_and_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     AND-expression:
      equality-expression
      AND-expression & equality-expression
    */
    try
    {
        pre_equality_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;
        while (ctx->current != NULL &&
            (ctx->current->type == '&'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_equality_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;
            ectx->value = left_value & ectx->value;
        }
    }
    catch
    {
    }
}

void pre_exclusive_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
     exclusive-OR-expression:
      AND-expression
     exclusive-OR-expression ^ AND-expression
    */
    try
    {
        pre_and_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        
        while (ctx->current != NULL &&
            (ctx->current->type == '^'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_and_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;
            ectx->value = left_value ^ ectx->value;
        }
    }
    catch
    {
    }
}

void pre_inclusive_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
    inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */
    try
    {
        pre_exclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '|'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_exclusive_or_expression(ctx, ectx);
            if (ctx->n_errors > 0) throw;

            ectx->value = left_value | ectx->value;
        }
    }
    catch
    {
    }
}

void pre_logical_and_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
    logical-AND-expression:
     inclusive-OR-expression
     logical-AND-expression && inclusive-OR-expression
    */
    try
    {
        pre_inclusive_or_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '&&'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_inclusive_or_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

            ectx->value = left_value && ectx->value;
        }
    }
    catch
    {
    }
}

void pre_logical_or_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
      logical-OR-expression:
       logical-AND-expression
       logical-OR-expression || logical-AND-expression
    */
    try
    {
        pre_logical_and_expression(ctx,  ectx);
        if (ctx->n_errors > 0) throw;

        while (ctx->current != NULL &&
            (ctx->current->type == '||'))
        {
            pre_match(ctx);
            long long left_value = ectx->value;
            pre_logical_and_expression(ctx,  ectx);
            if (ctx->n_errors > 0) throw;

            ectx->value = left_value || ectx->value;

        }
    }
    catch
    {
    }
}



void pre_assignment_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
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
        pre_conditional_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        
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
            preprocessor_set_error_with_token(ctx, ctx->current, "token '%s' is not valid in preprocessor expressions", ctx->current->lexeme);
            throw;
        }
    }
    catch
    {
    }
}

void pre_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*expression:
      assignment-expression
      expression, assignment-expression
    */
    try
    {
        pre_assignment_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;
        
        while (ctx->current->type == ',')
        {
            pre_match(ctx);
            pre_expression(ctx, ectx);
            if (ctx->n_errors > 0) throw;            
        }
    }
    catch
    {
    }
}



void pre_conditional_expression(struct preprocessor_ctx* ctx,struct pre_expression_ctx* ectx)
{
    /*
      conditional-expression:
      logical-OR-expression
      logical-OR-expression ? expression : conditional-expression
    */
    try
    {
        pre_logical_or_expression(ctx, ectx);
        if (ctx->n_errors > 0) throw;

        if (ctx->current && ctx->current->type == '?')
        {
            pre_match(ctx);
            if (ectx->value)
            {
                pre_expression(ctx, ectx);
                if (ctx->n_errors > 0) throw;

                pre_match(ctx); //:
                struct pre_expression_ctx temp;
                pre_conditional_expression(ctx, &temp);                
                if (ctx->n_errors > 0) throw;
            }
            else
            {
                struct pre_expression_ctx temp;
                pre_expression(ctx, &temp);
                if (ctx->n_errors > 0) throw;

                pre_match(ctx); //:
                pre_conditional_expression(ctx, ectx);
                if (ctx->n_errors > 0) throw;
            }
        }
    }
    catch
    {
    }
}

int pre_constant_expression(struct preprocessor_ctx* ctx,long long* pvalue)
{
    struct pre_expression_ctx ectx = { 0 };
    pre_conditional_expression(ctx, &ectx);
    *pvalue = ectx.value;
    return ctx->n_errors > 0;
}




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

    if (e_type_specifier_flags & TYPE_SPECIFIER_INT16)
        print_item(ss, first, "__int16");

    if (e_type_specifier_flags & TYPE_SPECIFIER_INT32)
        print_item(ss, first, "__int32");

    if (e_type_specifier_flags & TYPE_SPECIFIER_INT64)
        print_item(ss, first, "__int64");

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

    if (e_type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T)
        print_item(ss, first, "nullptr_t");

    return *first;
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

void print_type_qualifier_specifiers(struct osstream* ss, const struct type* type)
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

void type_add_const(struct type* p_type)
{
    p_type->type_qualifier_flags |= TYPE_QUALIFIER_CONST;
}

void type_remove_qualifiers(struct type* p_type)
{
    p_type->type_qualifier_flags = 0;
}

struct type type_lvalue_conversion(struct type* p_type)
{

    enum type_category category = type_get_category(p_type);
    switch (category)
    {
    case TYPE_CATEGORY_FUNCTION:
    {
        /*
           "function returning type" is converted to an expression that has type
           "pointer to function returning type".
        */
        struct type t = type_add_pointer(p_type);
        t.attributes_flags &= ~CUSTOM_ATTRIBUTE_PARAM;
        t.category = t.category;
        return t;
    }

    case TYPE_CATEGORY_ARRAY:
    {
        /*
          An expression that has type "array of type" is converted
          to an expression with type "pointer to type" that points to the initial element
          of the array object and s not an lvalue.
          If the array object has register storage class, the behavior is undefined.
        */
        struct type t = get_array_item_type(p_type);
        struct type t2 = type_add_pointer(&t);

        type_remove_qualifiers(&t2);
        /*
        int g(const int a[const 20]) {
            // in this function, a has type const int* const (const pointer to const int)
            }
        */
        type_destroy(&t);
        t2.attributes_flags &= ~CUSTOM_ATTRIBUTE_PARAM;
        return t2;
    }

    case TYPE_CATEGORY_POINTER:
    case TYPE_CATEGORY_ITSELF:
    default:
        break;
    }

    struct type t = type_dup(p_type);
    type_remove_qualifiers(&t);
    t.attributes_flags &= ~CUSTOM_ATTRIBUTE_PARAM;

    t.category = type_get_category(&t);

    return t;
}

struct type type_convert_to(struct type* p_type, enum language_version target)
{
    /*
    * Convert types to previous standard format
    */

    if (type_is_nullptr_t(p_type))
    {
        if (target < LANGUAGE_C2X)
        {
            struct type t = make_void_ptr_type();
            if (p_type->name_opt)
            {
                t.name_opt = strdup(p_type->name_opt);
            }
            return t;
        }
    }
    else if (type_is_bool(p_type))
    {
        if (target < LANGUAGE_C99)
        {
            struct type t = type_dup(p_type);
            t.type_specifier_flags &= ~TYPE_SPECIFIER_BOOL;
            t.type_specifier_flags |= TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR;
            return t;
        }
    }

    return type_dup(p_type);
}

void print_type_core(struct osstream* ss, const struct type* p_type, bool onlydeclarator)
{
    const struct type* p = p_type;

    while (p)
    {
        if (onlydeclarator && p->next == NULL)
            break;

        switch (p->category)
        {
        case TYPE_CATEGORY_ITSELF:
        {
            struct osstream local = { 0 };
            bool first = true;

            print_type_qualifier_flags(&local, &first, p->type_qualifier_flags);

            if (p->struct_or_union_specifier) {

                ss_fprintf(&local, "struct %s", p->struct_or_union_specifier->tag_name);
            }
            else if (p->enum_specifier) {
                if (p->enum_specifier->tag_token->lexeme)
                    ss_fprintf(&local, "enum %s", p->enum_specifier->tag_token->lexeme);
            }
            else
            {
                print_type_specifier_flags(&local, &first, p->type_specifier_flags);
            }



            if (p->name_opt)
            {
                if (first)
                {
                    ss_fprintf(ss, " ");
                    first = false;
                }
                ss_fprintf(ss, "%s", p->name_opt);
            }

            struct osstream local2 = { 0 };
            if (ss->c_str)
                ss_fprintf(&local2, "%s %s", local.c_str, ss->c_str);
            else
                ss_fprintf(&local2, "%s", local.c_str);

            ss_swap(ss, &local2);
            ss_close(&local);
            ss_close(&local2);
        }
        break;
        case TYPE_CATEGORY_ARRAY:


            if (p->name_opt)
            {
                //if (first)
                //{
                  //  ss_fprintf(ss, " ");
                    //first = false;
                //}
                ss_fprintf(ss, "%s", p->name_opt);
            }

            ss_fprintf(ss, "[");

            bool b = true;
            if (p->static_array)
            {
                ss_fprintf(ss, "static");
                b = false;
            }

            print_type_qualifier_flags(ss, &b, p->type_qualifier_flags);

            if (p->array_size > 0)
            {
                if (!b)
                    ss_fprintf(ss, " ");

                ss_fprintf(ss, "%d", p->array_size);
            }
            ss_fprintf(ss, "]");

            break;
        case TYPE_CATEGORY_FUNCTION:

            if (p->name_opt)
            {
                //if (first)
                //{
                  //  ss_fprintf(ss, " ");
                    //first = false;
                //}
                ss_fprintf(ss, "%s", p->name_opt);
            }
            ss_fprintf(ss, "(");




            struct param* pa = p->params.head;

            while (pa)
            {
                struct osstream sslocal = { 0 };
                print_type(&sslocal, pa->type);
                ss_fprintf(ss, "%s", sslocal.c_str);
                if (pa->next)
                    ss_fprintf(ss, ",");
                ss_close(&sslocal);
                pa = pa->next;
            }
            ss_fprintf(ss, ")");
            break;

        case TYPE_CATEGORY_POINTER:
        {
            struct osstream local = { 0 };
            if (p->next &&(
                (p->next->category == TYPE_CATEGORY_FUNCTION ||
                p->next->category == TYPE_CATEGORY_ARRAY)))
            {
                ss_fprintf(&local, "(");
            }

            ss_fprintf(&local, "*");
            bool first = false;
            print_type_qualifier_flags(&local, &first, p->type_qualifier_flags);

            if (p->name_opt)
            {
                if (!first)
                {
                    ss_fprintf(ss, " ");
                }
                ss_fprintf(ss, "%s", p->name_opt);
                first = false;
            }

            if (ss->c_str)
                ss_fprintf(&local, "%s", ss->c_str);

            if (p->next &&
                (p->next->category == TYPE_CATEGORY_FUNCTION ||
                p->next->category == TYPE_CATEGORY_ARRAY))
            {
                ss_fprintf(&local, ")", ss->c_str);
            }

            ss_swap(ss, &local);
            ss_close(&local);
        }
        break;
        }

        p = p->next;

    }
}

void print_type(struct osstream* ss, const struct type* p_type)
{
    print_type_core(ss, p_type, false);
}


void print_type_declarator(struct osstream* ss, const struct type* p_type)
{
    print_type_core(ss, p_type, true);
}


void type_print(const struct type* a) {
    struct osstream ss = { 0 };
    print_type(&ss, a);
    puts(ss.c_str);
    puts("\n");
    ss_close(&ss);
}




enum type_category type_get_category(const struct type* p_type)
{
    return p_type->category;
}

void type_destroy(struct type* p_type)
{
    //TODO
}

bool type_has_attribute(const struct type* p_type, enum attribute_flags attributes)
{
    if (p_type->attributes_flags & attributes)
    {
        /*like
          [[maybe_unused]] int i;
        */
        return true;
    }

    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt = NULL;


    if (p_type->struct_or_union_specifier)
    {
        /*like
          struct [[maybe_unused]] X { }
          struct X x;
        */
        p_attribute_specifier_sequence_opt = p_type->struct_or_union_specifier->attribute_specifier_sequence_opt;

        struct struct_or_union_specifier* p_complete =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_attribute_specifier_sequence_opt == NULL && p_complete)
        {
            p_attribute_specifier_sequence_opt = p_complete->attribute_specifier_sequence_opt;
        }
    }
    else if (p_type->enum_specifier)
    {
        p_attribute_specifier_sequence_opt = p_type->enum_specifier->attribute_specifier_sequence_opt;
        if (p_attribute_specifier_sequence_opt == NULL &&
            p_type->enum_specifier->complete_enum_specifier)
        {
            p_attribute_specifier_sequence_opt = p_type->enum_specifier->complete_enum_specifier->attribute_specifier_sequence_opt;
        }
    }

    if (p_attribute_specifier_sequence_opt &&
        p_attribute_specifier_sequence_opt->attributes_flags & attributes)
    {
        return true;
    }

    return false;
}

bool type_is_maybe_unused(const struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_MAYBE_UNUSED);
}

bool type_is_deprecated(const struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_DEPRECATED);
}

bool type_is_nodiscard(const struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_NODISCARD);
}

bool type_is_destroy(const struct type* p_type)
{
    return type_has_attribute(p_type, CUSTOM_ATTRIBUTE_DESTROY);
}

bool type_is_array(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ARRAY;
}

bool type_is_const(const struct type* p_type)
{
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_CONST;
}

bool type_is_void_ptr(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_POINTER)
    {
        if (p_type->next)
        {
            return p_type->next->type_specifier_flags & TYPE_SPECIFIER_VOID;
        }
    }
    return false;
}

bool type_is_void(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_ITSELF)
    {
        return p_type->type_specifier_flags & TYPE_SPECIFIER_VOID;
    }

    return false;
}

bool type_is_nullptr_t(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_ITSELF)
    {
        return p_type->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T;
    }

    return false;
}

bool type_is_pointer(const struct type* p_type)
{
    return p_type->category == TYPE_CATEGORY_POINTER;
}

bool type_is_enum(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM;
}

bool type_is_struct_or_union(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION;
}

/*
  The three types
  char, signed char, and unsigned char
  are collectively called the character types.
*/
bool type_is_character(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR;
}

bool type_is_bool(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL;
}

/*
 There are three standard floating types, designated as
 float, double, and long double.

 There are three decimal floating types, designated as _Decimal32, _Decimal64, and _Decimal128.
*/
bool type_is_floating_point(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_DOUBLE |
            TYPE_SPECIFIER_FLOAT);
}

bool type_is_unsigned_integer(const struct type* p_type)
{
    if (type_is_integer(p_type) &&
        (p_type->type_specifier_flags & TYPE_SPECIFIER_UNSIGNED))
    {
        return true;
    }

    return false;
}
/*
  The type char, the signed and unsigned integer types,
  and the enumerated types
  are collectively  called integer types.
*/
bool type_is_integer(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {
        /*we cannot check long without check double*/
        //long double
        return false;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
    {
        return true;
    }

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_CHAR |
            TYPE_SPECIFIER_SHORT |
            TYPE_SPECIFIER_INT |

            TYPE_SPECIFIER_INT16 |
            TYPE_SPECIFIER_INT32 |
            TYPE_SPECIFIER_INT64 |

            TYPE_SPECIFIER_INT |
            TYPE_SPECIFIER_LONG |
            TYPE_SPECIFIER_SIGNED |
            TYPE_SPECIFIER_UNSIGNED |
            TYPE_SPECIFIER_INT8 |
            TYPE_SPECIFIER_INT16 |
            TYPE_SPECIFIER_INT64 |
            TYPE_SPECIFIER_LONG_LONG |
            TYPE_SPECIFIER_BOOL);
}

/*
* Integer and floating types are collectively called arithmetic types.
*/
bool type_is_arithmetic(const struct type* p_type)
{
    return type_is_integer(p_type) || type_is_floating_point(p_type);
}

/*
 Arithmetic types, pointer types, and the nullptr_t type are collectively
 called scalar types.
*/
bool type_is_scalar(const struct type* p_type)
{
    //TODO we need two concepts...is_scalar on real type or is_scalar after lvalue converison

    if (type_is_arithmetic(p_type))
        return true;

    if (type_is_pointer_or_array(p_type))
        return true;

    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;


    if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        return true;
    if (p_type->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T)
        return true;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        return true;

    return false;
}


const struct param_list* type_get_func_or_func_ptr_params(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_FUNCTION)
    {
        return &p_type->params;
    }
    else if (p_type->category == TYPE_CATEGORY_POINTER)
    {
        if (p_type->next &&
            p_type->next->category == TYPE_CATEGORY_FUNCTION)
        {
            return &p_type->next->params;
        }
    }
    return NULL;
}

void check_function_argument_and_parameter(struct parser_ctx* ctx,
    struct argument_expression* current_argument,
    struct type* paramer_type,
    int param_num)
{
    //see also check_assigment

    struct type* argument_type = &current_argument->expression->type;
    bool is_null_pointer_constant = false;

    if (type_is_nullptr_t(&current_argument->expression->type) ||
        (constant_value_is_valid(&current_argument->expression->constant_value) &&
            constant_value_to_ull(&current_argument->expression->constant_value) == 0))
    {
        is_null_pointer_constant = true;
    }

    struct type t1 = { 0 };
    struct type t2 = { 0 };
    /*
       less generic tests are first
    */
    if (type_is_enum(argument_type) && type_is_enum(paramer_type))
    {
        if (!type_is_same(argument_type, paramer_type, false))
        {
            compiler_set_error_with_token(ctx,
                current_argument->expression->first_token,
                " incompatible types at argument %d", param_num);
        }
        goto continuation;
    }

    if (type_is_arithmetic(argument_type) && type_is_arithmetic(paramer_type))
    {
        goto continuation;
    }

    if (is_null_pointer_constant && type_is_pointer(paramer_type))
    {
        //TODO void F(int * [[opt]] p)
        // F(0) when passing null we will check if the parameter 
        //have the anotation [[opt]]

        /*can be converted to any type*/
        goto continuation;
    }

    if (is_null_pointer_constant && type_is_array(paramer_type))
    {
        compiler_set_warning_with_token(W_NON_NULL,
            ctx,
            current_argument->expression->first_token,
            " passing null as array");

        goto continuation;
    }

    /*
       We have two pointers or pointer/array combination
    */
    if (type_is_pointer_or_array(argument_type) && type_is_pointer_or_array(paramer_type))
    {
        if (type_is_void_ptr(argument_type))
        {
            /*void pointer can be converted to any type*/
            goto continuation;
        }

        if (type_is_void_ptr(paramer_type))
        {
            /*any pointer can be converted to void* */
            goto continuation;
        }


        //TODO  lvalue

        if (type_is_array(paramer_type))
        {
            int parameter_array_size = type_get_array_size(paramer_type);
            if (type_is_array(argument_type))
            {
                int argument_array_size = type_get_array_size(argument_type);
                if (parameter_array_size != 0 &&
                    argument_array_size < parameter_array_size)
                {
                    compiler_set_error_with_token(ctx,
                        current_argument->expression->first_token,
                        " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                }
            }
            else if (is_null_pointer_constant || type_is_nullptr_t(argument_type))
            {
                compiler_set_error_with_token(ctx,
                    current_argument->expression->first_token,
                    " passing null as array");
            }
            t2 = type_lvalue_conversion(paramer_type);
        }
        else
        {
            t2 = type_dup(paramer_type);
        }

        if (expression_is_subjected_to_lvalue_conversion(current_argument->expression))
        {
            t1 = type_lvalue_conversion(argument_type);
        }
        else
        {
            t1 = type_dup(argument_type);
        }


        if (!type_is_same(&t1, &t2, false))
        {
            type_print(&t1);
            type_print(&t2);

            compiler_set_error_with_token(ctx,
                current_argument->expression->first_token,
                " incompatible types at argument %d", param_num);
            //disabled for now util it works correctly
            //return false;
        }

        if (type_is_pointer(&t1) && type_is_pointer(&t2))
        {
            //parameter pointer do non const
            //argument const.
            struct type argument_pointer_to = type_remove_pointer(&t1);
            struct type parameter_pointer_to = type_remove_pointer(&t2);
            if (type_is_const(&argument_pointer_to) && !type_is_const(&parameter_pointer_to))
            {
                compiler_set_error_with_token(ctx,
                    current_argument->expression->first_token,
                    " discarding const at argument %d", param_num);
            }
            type_destroy(&argument_pointer_to);
            type_destroy(&parameter_pointer_to);
        }
        //return true;
    }

    //TODO
    //if (!type_is_same(paramer_type, &current_argument->expression->type, false))
    //{
    //    compiler_set_error_with_token(ctx,
    //        current_argument->expression->first_token,
    //        " incompatible types at argument %d ", param_num);
    //}

continuation:
    type_destroy(&t1);
    type_destroy(&t2);
}

void check_assigment(struct parser_ctx* ctx,
                     struct type* left_type,
                     struct expression* right)
{

    //see also check_function_argument_and_parameter

    struct type* p_right_type = &right->type;
    bool is_null_pointer_constant = false;

    if (type_is_nullptr_t(& right->type) ||
        (constant_value_is_valid(&right->constant_value) &&
            constant_value_to_ull(&right->constant_value) == 0))
    {
        is_null_pointer_constant = true;
    }

    struct type lvalue_right_type = { 0 };
    struct type t2 = { 0 };

    if (expression_is_subjected_to_lvalue_conversion(right))
    {
        lvalue_right_type = type_lvalue_conversion(p_right_type);
    }
    else
    {
        lvalue_right_type = type_dup(p_right_type);
    }


    /*
       less generic tests are first
    */
    if (type_is_enum(p_right_type) && type_is_enum(left_type))
    {
        if (!type_is_same(p_right_type, left_type, false))
        {
            compiler_set_error_with_token(ctx,
                right->first_token,
                " incompatible types ");
        }
        goto continuation;
    }

    if (type_is_arithmetic(p_right_type) && type_is_arithmetic(left_type))
    {
        goto continuation;
    }

    if (is_null_pointer_constant && type_is_pointer(left_type))
    {
        //TODO void F(int * [[opt]] p)
        // F(0) when passing null we will check if the parameter 
        //have the anotation [[opt]]

        /*can be converted to any type*/
        goto continuation;
    }

    if (is_null_pointer_constant && type_is_array(left_type))
    {
        compiler_set_warning_with_token(W_NON_NULL,
            ctx,
            right->first_token,
            " passing null as array");

        goto continuation;
    }

    /*
       We have two pointers or pointer/array combination
    */
    if (type_is_pointer_or_array(p_right_type) && type_is_pointer_or_array(left_type))
    {
        if (type_is_void_ptr(p_right_type))
        {
            /*void pointer can be converted to any type*/
            goto continuation;
        }

        if (type_is_void_ptr(left_type))
        {
            /*any pointer can be converted to void* */
            goto continuation;
        }


        //TODO  lvalue

        if (type_is_array(left_type))
        {
            int parameter_array_size = type_get_array_size(left_type);
            if (type_is_array(p_right_type))
            {
                int argument_array_size = type_get_array_size(p_right_type);
                if (parameter_array_size != 0 &&
                    argument_array_size < parameter_array_size)
                {
                    compiler_set_error_with_token(ctx,
                        right->first_token,
                        " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                }
            }
            else if (is_null_pointer_constant || type_is_nullptr_t(p_right_type))
            {
                compiler_set_error_with_token(ctx,
                    right->first_token,
                    " passing null as array");
            }
            t2 = type_lvalue_conversion(left_type);
        }
        else
        {
            t2 = type_dup(left_type);
        }



        if (!type_is_same(&lvalue_right_type, &t2, false))
        {
            type_print(&lvalue_right_type);
            type_print(&t2);

            compiler_set_error_with_token(ctx,
                right->first_token,
                " incompatible types at argument " );
            //disabled for now util it works correctly
            //return false;
        }

        if (type_is_pointer(&lvalue_right_type) && type_is_pointer(&t2))
        {
            //parameter pointer do non const
            //argument const.
            struct type argument_pointer_to = type_remove_pointer(&lvalue_right_type);
            struct type parameter_pointer_to = type_remove_pointer(&t2);
            if (type_is_const(&argument_pointer_to) && !type_is_const(&parameter_pointer_to))
            {
                compiler_set_error_with_token(ctx,
                    right->first_token,
                    " discarding const at argument ");
            }
            type_destroy(&argument_pointer_to);
            type_destroy(&parameter_pointer_to);
        }
        //return true;
    }

    if (!type_is_same(left_type, &lvalue_right_type, false))
    {
      //TODO more rules..but it is good to check worst case!
      // 
      //  compiler_set_error_with_token(ctx,
      //      right->first_token,
      //      " incompatible types ");
    }

continuation:
    type_destroy(&lvalue_right_type);
    type_destroy(&t2);
    
}

bool type_is_function(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_FUNCTION;
}

bool type_is_function_or_function_pointer(const struct type* p_type)
{
    if (type_is_function(p_type))
        return true;

    if (type_is_pointer(p_type))
    {
        //TODO not optimized
        struct type t = type_remove_pointer(p_type);
        bool r = type_is_function(&t);
        type_destroy(&t);
        return r;
    }

    return false;
}

struct type type_add_pointer(struct type* p_type)
{
    struct type r = type_dup(p_type);

    struct type* p = calloc(1, sizeof(struct type));
    *p = r;

    memset(&r, 0, sizeof r);

    r.next = p;
    r.category = TYPE_CATEGORY_POINTER;

    return r;
}

struct type type_remove_pointer(const struct type* p_type)
{
    struct type r = type_dup(p_type);
    r = *r.next;
    return r;
}

struct type get_array_item_type(const struct type* p_type)
{
    struct type r = type_dup(p_type);
    r = *r.next;
    return r;
}

struct type type_param_array_to_pointer(const struct type* p_type)
{
    assert(type_is_array(p_type));
    struct type t = get_array_item_type(p_type);
    struct type t2 = type_add_pointer(&t);

    if (p_type->type_qualifier_flags & TYPE_QUALIFIER_CONST)
    {
      /*
       void F(int a[static const 5]) {
        static_assert((typeof(a)) == (int* const));
      }
      */
      t2.type_qualifier_flags |= TYPE_QUALIFIER_CONST;
    }

    type_destroy(&t);
    t2.attributes_flags &= ~CUSTOM_ATTRIBUTE_PARAM;

    return t2;
}

bool type_is_pointer_or_array(const struct type* p_type)
{
    const enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER ||
        category == TYPE_CATEGORY_ARRAY)
        return true;

    if (category == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags == TYPE_SPECIFIER_NULLPTR_T)
    {
        return true;
    }

    return false;
}


//See 6.3.1.1
int type_get_rank(struct type* p_type1)
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
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T))
    {
        rank = 50; //?
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
        return -1;
        //seterror(error, "internal error - struct is not valid for rank");
    }
    else
    {
        return -2;
        //seterror(error, "unexpected type for rank");
    }
    return rank;
}

int type_common(struct type* p_type1, struct type* p_type2, struct type* out)
{
    try
    {
        type_destroy(out);

        int rank_left = type_get_rank(p_type1);
        if (rank_left < 0) throw;

        int rank_right = type_get_rank(p_type2);
        if (rank_right < 0) throw;

        if (rank_left >= rank_right)
            *out = type_dup(p_type1);
        else
            *out = type_dup(p_type2);
    }
    catch
    {
        return 1;
    }

    return 0;
}

struct type type_dup(const struct type* p_type)
{
    struct type_list l = { 0 };
    const struct type* p = p_type;
    while (p)
    {
        struct type* p_new = calloc(1, sizeof(struct type));
        *p_new = *p;
        p_new->next = NULL;

        if (p->name_opt)
            p_new->name_opt = strdup(p->name_opt);

        if (p->category == TYPE_CATEGORY_FUNCTION)
        {
            p_new->params = (struct param_list){ 0 };
            p_new->params.is_var_args = p->params.is_var_args;
            p_new->params.is_void = p->params.is_void;

            struct param* p_param = p->params.head;
            while (p_param)
            {
                struct param* p_new_param = calloc(1, sizeof * p_new_param);
                p_new_param->type = calloc(1, sizeof(struct type));
                *p_new_param->type = type_dup(p_param->type);

                LIST_ADD(&p_new->params, p_new_param);
                p_param = p_param->next;
            }
        }

        type_list_push_back(&l, p_new);
        p = p->next;
    }
    return *l.head;
}


int type_get_array_size(const struct type* p_type)
{
    return p_type->array_size;
}


int type_set_array_size(struct type* p_type, int size)
{
    p_type->array_size = size;
    return 0;
}

int type_get_sizeof(struct type* p_type);
int get_sizeof_struct(struct struct_or_union_specifier* complete_struct_or_union_specifier)
{
    int maxalign = 0;
    int size = 0;
    struct member_declaration* d = complete_struct_or_union_specifier->member_declaration_list.head;
    while (d)
    {
        if (d->member_declarator_list_opt)
        {
            struct member_declarator* md = d->member_declarator_list_opt->head;
            while (md)
            {
                int align = type_get_alignof(&md->declarator->type);

                if (align > maxalign)
                {
                    maxalign = align;
                }
                if (size % align != 0)
                {
                    size += align - (size % align);
                }

                size += type_get_sizeof(&md->declarator->type);
                md = md->next;
            }
        }
        d = d->next;
    }
    if (maxalign != 0)
    {
        if (size % maxalign != 0)
        {
            size += maxalign - (size % maxalign);
        }
    }
    else
    {
        assert(false);
    }

    return size;
}
int type_get_alignof(struct type* p_type);
int get_alignof_struct(struct struct_or_union_specifier* complete_struct_or_union_specifier)
{
    int align = 0;
    struct member_declaration* d = complete_struct_or_union_specifier->member_declaration_list.head;
    while (d)
    {
        if (d->member_declarator_list_opt)
        {
            struct member_declarator* md = d->member_declarator_list_opt->head;
            while (md)
            {
                //TODO padding
                int temp_align = type_get_alignof(&md->declarator->type);
                if (temp_align > align)
                {
                    align = temp_align;
                }
                md = md->next;
            }
        }
        else
        {
            /*We don't have the declarator like in */
            /*
              struct X {
                union {
                    struct {
                        int Zone;
                    };
                    int Value;
                };
            };
            static_assert(alignof(struct X) == 1);
            */

            /*so we create a type using only specifiers*/

            struct type type = { 0 };
            if (d->specifier_qualifier_list)
            {
                type.type_specifier_flags =
                    d->specifier_qualifier_list->type_specifier_flags;

                type.enum_specifier = d->specifier_qualifier_list->enum_specifier;
                type.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;

            }

            int temp_align = type_get_alignof(&type);
            if (temp_align > align)
            {
                align = temp_align;
            }

            type_destroy(&type);
        }
        d = d->next;
    }
    assert(align != 0);
    return align;
}

int type_get_alignof(struct type* p_type)
{
    int align = 0;

    enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        align = _Alignof(void*);
    }
    else if (category == TYPE_CATEGORY_FUNCTION)
    {
        align = -1;
        //seterror(error, "sizeof function");
    }
    else if (category == TYPE_CATEGORY_ITSELF)
    {
        if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
        {
            align = _Alignof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        {
            align = _Alignof(_Bool);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        {
            align = _Alignof(short);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT)
        {
            align = _Alignof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            //TODO enum type
            align = _Alignof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            align = _Alignof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        {
            align = _Alignof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT64)
        {
            align = _Alignof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT32)
        {
            align = _Alignof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT16)
        {
            align = _Alignof(short);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT8)
        {
            align = _Alignof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        {
            align = _Alignof(float);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        {
            align = _Alignof(double);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            struct struct_or_union_specifier* p_complete =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            align = 1;
            if (p_complete)
            {
                align = get_alignof_struct(p_complete);
            }
            else
            {
                align = -2;
                //seterror(error, "invalid application of 'sizeof' to incomplete type 'struct %s'", p_type->struct_or_union_specifier->tag_name);
            }
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            align = _Alignof(int);
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            align = -3;
            //seterror(error, "type information is missing");
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
        {
            align = 1;
        }
        else
        {
            assert(false);
        }
    }
    else if (category == TYPE_CATEGORY_ARRAY)
    {

        struct type type = get_array_item_type(p_type);
        align = type_get_alignof(&type);
        type_destroy(&type);
    }
    assert(align > 0);
    return align;
}


int type_get_sizeof(struct type* p_type)
{
    int size = 0;

    enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        size = (int)sizeof(void*);
    }
    else if (category == TYPE_CATEGORY_FUNCTION)
    {
        size = -1;
        //seterror(error, "sizeof function");
    }
    else if (category == TYPE_CATEGORY_ITSELF)
    {
        if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
        {
            size = (int)sizeof(char);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        {
            size = (int)sizeof(_Bool);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        {
            size = (int)sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT)
        {
            size = (int)sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            //TODO enum type
            size = (int)sizeof(int);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            size = (int)sizeof(long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        {
            size = (int)sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT64)
        {
            size = (int)sizeof(long long);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT32)
        {
            size = 4;
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT16)
        {
            size = 2;
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT8)
        {
            size = 1;
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        {
            size = (int)sizeof(float);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        {
            size = (int)sizeof(double);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            struct struct_or_union_specifier* p_complete =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            size = 1;
            if (p_complete)
            {
                size = get_sizeof_struct(p_complete);
            }
            else
            {
                size = -2;
                //seterror(error, "invalid application of 'sizeof' to incomplete type 'struct %s'", p_type->struct_or_union_specifier->tag_name);
            }
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            size = (int)sizeof(int);
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            size = -3;
            //seterror(error, "type information is missing");
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
        {
            size = 1;
        }
        else
        {
            assert(false);
        }
    }
    else if (category == TYPE_CATEGORY_ARRAY)
    {
        int arraysize = type_get_array_size(p_type);
        struct type type = get_array_item_type(p_type);
        int sz = type_get_sizeof(&type);
        size = sz * arraysize;
        type_destroy(&type);
    }

    return size;
}

unsigned int type_get_hashof(struct parser_ctx* ctx, struct type* p_type)
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

            struct struct_or_union_specifier* p_complete =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            if (p_complete)
            {
                struct token* current = p_complete->first_token;
                for (;
                    current != p_complete->last_token->next;
                    current = current->next)
                {
                    if (current->flags & TK_FLAG_FINAL)
                    {
                        ss_fprintf(&ss, "%s", current->lexeme);

                    }
                }
            }

            hash = string_hash(ss.c_str);
            ss_close(&ss);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            //size = sizeof(int);
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            compiler_set_info_with_token(ctx, ctx->current, "type information is missing");
            throw;
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_TYPEOF)
        {
            //s//ize = 1; //TODO
            //assert(false);
            //;; size =
                //  type_get_sizeof(ctx, struct type* p_type)
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
        {
            if (type_is_void(p_type))
            {
                compiler_set_error_with_token(ctx,
                    ctx->current,
                    "invalid application of 'sizeof' to a void type");
                throw;
            }
        }
        else
        {
            assert(false);
        }
    }
    catch
    {
    }

    return hash;
}


void type_set_attributes(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->attributes_flags =
            pdeclarator->declaration_specifiers->attributes_flags;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        //p_type->type_qualifier_flags =
          //  pdeclarator->specifier_qualifier_list->ATR;
    }
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

void type_set_specifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
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


struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);


#if 0    
/*this sample is useful to try in compiler explorer*/


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
}
#endif
/*

typedef char *T;
T a[2]; //char * [2]

typedef char *T[1];
T a[2]; // char* [2][1]

typedef char (*PF)(void);
PF a[2]; //char (* [2])()

typedef char *T;
T (*a)(void); //char* (*)()

typedef char const *T;
T (*a)(void); //char const* (*)()

typedef char (*PF)(void);
    PF (*a)(void); //char (*(*)())()

typedef char (*PF)(double);
    PF (*a)(int); //char (*(*)(int))(double)

 typedef char (*PF)(double);
 const PF (*a)(int); //char (* const (*)(int))(double)

*/




struct type get_function_return_type(struct type* p_type)
{

    if (type_is_pointer(p_type))
    {
        /*pointer to function returning ... */
        struct type r = type_dup(p_type->next->next);
        return r;
    }
    

    /*function returning ... */
    struct type r = type_dup(p_type->next);
    return r;
}


void type_set_int(struct type* p_type)
{
    p_type->type_specifier_flags = TYPE_SPECIFIER_INT;
    p_type->type_qualifier_flags = 0;
    p_type->category = TYPE_CATEGORY_ITSELF;
}

struct type type_make_enumerator(struct enum_specifier* enum_specifier)
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_ENUM;
    t.enum_specifier = enum_specifier;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}
struct type type_get_enum_type(const struct type* p_type) 
{
    assert(p_type->enum_specifier != NULL);
    
    if (p_type->enum_specifier->complete_enum_specifier &&
        p_type->enum_specifier->complete_enum_specifier->specifier_qualifier_list)
    {
        struct type t = { 0 };
        t.type_qualifier_flags = p_type->enum_specifier->complete_enum_specifier->specifier_qualifier_list->type_qualifier_flags;
        t.type_specifier_flags= p_type->enum_specifier->complete_enum_specifier->specifier_qualifier_list->type_specifier_flags;
        return t;
    }
        
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_INT;
    return t;
}

struct type type_make_size_t()
{
    struct type t = { 0 };

#ifdef _WIN64
    t.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT64;
#else
    t.type_specifier_flags = TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT;
#endif

    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type make_void_ptr_type()
{
    struct type t = { 0 };
    t.category = TYPE_CATEGORY_POINTER;

    struct type* p = calloc(1, sizeof * p);
    p->category = TYPE_CATEGORY_ITSELF;
    p->type_specifier_flags = TYPE_SPECIFIER_VOID;
    t.next = p;

    return t;
}

struct type make_void_type()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_VOID;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_int_bool_like()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_INT ;
    t.attributes_flags = CUSTOM_ATTRIBUTE_LIKE_BOOL;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_int()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_INT;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_literal_string(int size, enum type_specifier_flags chartype)
{
    struct type t = { 0 };
    t.category = TYPE_CATEGORY_ARRAY;
    t.array_size = size;
    struct type* p2 = calloc(1, sizeof(struct type));
    p2->category = TYPE_CATEGORY_ITSELF;
    p2->type_specifier_flags = chartype;
    t.next = p2;
    return t;
}

bool struct_or_union_specifier_is_same(struct struct_or_union_specifier* a, struct struct_or_union_specifier* b)
{
    if (a && b)
    {
        struct struct_or_union_specifier* p_complete_a = get_complete_struct_or_union_specifier(a);
        struct struct_or_union_specifier* p_complete_b = get_complete_struct_or_union_specifier(b);

        if (p_complete_a != NULL && p_complete_b != NULL)
        {
            if (p_complete_a != p_complete_b)
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
        return p_complete_a == NULL && p_complete_b == NULL;
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




bool type_is_same(const struct type* a, const struct type* b, bool compare_qualifiers)
{
    const struct type* pa = a;
    const struct type* pb = b;


    while (pa && pb)
    {


        if (pa->array_size != pb->array_size) return false;

        if (pa->category != pb->category) return false;

        if (pa->enum_specifier &&
            pb->enum_specifier &&
            pa->enum_specifier->complete_enum_specifier !=
            pb->enum_specifier->complete_enum_specifier)
        {
            return false;
        }

        
        if (pa->enum_specifier && !pb->enum_specifier)
        {            
            //TODO enum with types
            //enum  x int
           //return false;
        }
        
        if (!pa->enum_specifier && pb->enum_specifier)
        {
            //TODO enum with types
            //int x enum
            //return false;
        }
        
        //if (pa->name_opt != pb->name_opt) return false;
        if (pa->static_array != pb->static_array) return false;

        if (pa->category == TYPE_CATEGORY_FUNCTION)
        {

            if (pa->params.is_var_args != pb->params.is_var_args)
            {
                return false;
            }

            if (pa->params.is_void != pb->params.is_void)
            {
                return false;
            }

            struct param* p_param_a = pa->params.head;
            struct param* p_param_b = pb->params.head;
            while (p_param_a && p_param_b)
            {
                if (!type_is_same(p_param_a->type, p_param_b->type, true))
                {
                    return false;
                }
                p_param_a = p_param_a->next;
                p_param_b = p_param_b->next;
            }
            return p_param_a == NULL && p_param_b == NULL;
        }

        if (pa->struct_or_union_specifier &&
            pb->struct_or_union_specifier)
        {

            if (pa->struct_or_union_specifier->complete_struct_or_union_specifier_indirection !=
                pb->struct_or_union_specifier->complete_struct_or_union_specifier_indirection)
            {
                //this should work but it is not... 
            }

            if (strcmp(pa->struct_or_union_specifier->tag_name, pb->struct_or_union_specifier->tag_name) != 0)
            {
                return false;
            }
        }

        if (compare_qualifiers && pa->type_qualifier_flags != pb->type_qualifier_flags)
        {
            return false;
        }

        if (pa->type_specifier_flags != pb->type_specifier_flags)
        {
            return false;
        }


        pa = pa->next;
        pb = pb->next;
    }
    return pa == NULL && pb == NULL;
}


void type_swap(struct type* a, struct type* b)
{
    struct type temp = *a;
    *a = *b;
    *b = temp;
    _del_attr(temp, MUST_DESTROY);
}


void type_visit_to_mark_anonymous(struct type* p_type)
{
    //TODO better visit?
    if (p_type->struct_or_union_specifier != NULL &&
        p_type->struct_or_union_specifier->has_anonymous_tag)
    {
        if (p_type->struct_or_union_specifier->complete_struct_or_union_specifier_indirection)
        {
            p_type->struct_or_union_specifier->complete_struct_or_union_specifier_indirection->show_anonymous_tag = true;
        }
        p_type->struct_or_union_specifier->show_anonymous_tag = true;
    }

}



void type_flat_set_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
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

void type_flat_set_specifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->type_specifier_flags =
            pdeclarator->declaration_specifiers->type_specifier_flags;

        p_type->attributes_flags =
            pdeclarator->declaration_specifiers->attributes_flags;

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

void type_list_push_front(struct type_list* books, struct type* new_book)
{
    assert(books != NULL);
    assert(new_book != NULL);
    assert(new_book->next == NULL);

    if (books->head == NULL) {
        books->head = new_book;
        books->tail = new_book;
    }
    else {
        new_book->next = books->head;
        books->head = new_book;
    }
}


void type_list_push_back(struct type_list* books, struct type* new_book)
{
    assert(books != NULL);
    assert(new_book != NULL);
    //assert(new_book->next == NULL);

    if (books->tail == NULL) {
        books->head = new_book;
    }
    else {
        books->tail->next = new_book;
    }
    books->tail = new_book;
}

void make_type_using_declarator_core(struct parser_ctx* ctx, struct declarator* pdeclarator, char** ppname, struct type_list* list);

void  make_type_using_direct_declarator(struct parser_ctx* ctx,
    struct direct_declarator* pdirectdeclarator,
    char** ppname,
    struct type_list* list)
{
    if (pdirectdeclarator->declarator)
    {
        make_type_using_declarator_core(ctx, pdirectdeclarator->declarator, ppname, list);
    }

    else if (pdirectdeclarator->function_declarator)
    {
        if (pdirectdeclarator->function_declarator->direct_declarator)
        {
            make_type_using_direct_declarator(ctx,
                pdirectdeclarator->function_declarator->direct_declarator,
                ppname,
                list);
        }

        struct type* p_func = calloc(1, sizeof(struct type));
        p_func->category = TYPE_CATEGORY_FUNCTION;


        if (pdirectdeclarator->function_declarator->parameter_type_list_opt &&
            pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list)
        {

            struct parameter_declaration* p =
                pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list->head;

            p_func->params.is_var_args = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_var_args;

            p_func->params.is_void = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_void;



            while (p)
            {
                struct param* p_new_param = calloc(1, sizeof(struct param));

                struct type* pt = calloc(1, sizeof(struct type));
                struct type nt =
                    make_type_using_declarator(ctx, p->declarator);
                *pt = nt; /*MOVED*/
                _del_attr(nt,"must destroy");
                
                p_new_param->type = pt;

                LIST_ADD(&p_func->params, p_new_param);
                p = p->next;
            }
        }


        type_list_push_back(list, p_func);
    }
    else if (pdirectdeclarator->array_declarator)
    {

        if (pdirectdeclarator->array_declarator->direct_declarator)
        {
            make_type_using_direct_declarator(ctx,
                pdirectdeclarator->array_declarator->direct_declarator,
                ppname,
                list);
        }

        struct type* p = calloc(1, sizeof(struct type));
        p->category = TYPE_CATEGORY_ARRAY;

        p->array_size = (int) pdirectdeclarator->array_declarator->constant_size;

        if (pdirectdeclarator->array_declarator->static_token_opt)
        {
            p->static_array = true;
        }

        if (pdirectdeclarator->array_declarator->type_qualifier_list_opt)
        {
            p->type_qualifier_flags = pdirectdeclarator->array_declarator->type_qualifier_list_opt->flags;
        }

        type_list_push_back(list, p);

        // if (pdirectdeclarator->name_opt)
         //{
           //  p->name_opt = strdup(pdirectdeclarator->name_opt->lexeme);
         //}
    }

    if (pdirectdeclarator->name_opt)
    {
        *ppname = pdirectdeclarator->name_opt->lexeme;
    }


}

void make_type_using_declarator_core(struct parser_ctx* ctx, struct declarator* pdeclarator,
    char** ppname, struct type_list* list)
{
    struct type_list pointers = { 0 };
    struct pointer* pointer = pdeclarator->pointer;
    while (pointer)
    {
        struct type* p_flat = calloc(1, sizeof(struct type));

        if (pointer->type_qualifier_list_opt)
        {
          p_flat->type_qualifier_flags = pointer->type_qualifier_list_opt->flags;
        }
        
        if (pointer->attribute_specifier_sequence_opt)
        {
            p_flat->attributes_flags |= pointer->pointer->attribute_specifier_sequence_opt->attributes_flags;
        }
        p_flat->category = TYPE_CATEGORY_POINTER;
        type_list_push_front(&pointers, p_flat); /*invertido*/
        pointer = pointer->pointer;
    }

    if (pdeclarator->direct_declarator)
    {
        make_type_using_direct_declarator(ctx, pdeclarator->direct_declarator, ppname, list);
    }

    if (pointers.head)
    {
        struct type* p = pointers.head;
        while (p) {
            type_list_push_back(list, p);
            p = p->next;
        }
    }

}

struct enum_specifier* declarator_get_enum_specifier(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers &&
        pdeclarator->declaration_specifiers->enum_specifier)
    {
        return pdeclarator->declaration_specifiers->enum_specifier;
    }
    if (pdeclarator->specifier_qualifier_list &&
        pdeclarator->specifier_qualifier_list->enum_specifier)
    {
        return pdeclarator->specifier_qualifier_list->enum_specifier;
    }
    return NULL;
}


struct struct_or_union_specifier* declarator_get_struct_or_union_specifier(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers &&
        pdeclarator->declaration_specifiers->struct_or_union_specifier)
    {
        return pdeclarator->declaration_specifiers->struct_or_union_specifier;
    }
    if (pdeclarator->specifier_qualifier_list &&
        pdeclarator->specifier_qualifier_list->struct_or_union_specifier)
    {
        return pdeclarator->specifier_qualifier_list->struct_or_union_specifier;
    }
    return NULL;
}

struct typeof_specifier* declarator_get_typeof_specifier(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        return pdeclarator->declaration_specifiers->typeof_specifier;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        return pdeclarator->specifier_qualifier_list->typeof_specifier;
    }
    return NULL;
}

struct declarator* declarator_get_typedef_declarator(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        return pdeclarator->declaration_specifiers->typedef_declarator;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        return pdeclarator->specifier_qualifier_list->typedef_declarator;
    }

    return NULL;
}

struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator)
{
    struct type_list list = { 0 };
    char* name = 0;
    make_type_using_declarator_core(ctx, pdeclarator, &name, &list);

    //type_print(list.head);

    if (declarator_get_typeof_specifier(pdeclarator))
    {
        struct type nt =
            type_dup(&declarator_get_typeof_specifier(pdeclarator)->type);

        struct type* p_nt = calloc(1, sizeof(struct type));
        *p_nt = nt;

        _del_attr(nt, "must destroy"); /*MOVED*/

        bool head = list.head != NULL;

        if (head)
            type_flat_set_qualifiers_using_declarator(list.head, pdeclarator);

        if (list.tail)
            list.tail->next = p_nt;
        else
        {
            type_list_push_back(&list, p_nt);
        }

        //if (!head)
          //type_flat_set_qualifiers_using_declarator(list.head, pdeclarator);


    }
    else if (declarator_get_typedef_declarator(pdeclarator))
    {
        struct declarator* p_typedef_declarator =
            declarator_get_typedef_declarator(pdeclarator);

        struct type nt =
            type_dup(&p_typedef_declarator->type);

        struct type* p_nt = calloc(1, sizeof(struct type));
        *p_nt = nt;

        _del_attr(nt, "must destroy"); /*MOVED*/


        type_flat_set_qualifiers_using_declarator(p_nt, pdeclarator);

        if (list.tail)
            list.tail->next = p_nt;
        else
        {
            type_list_push_back(&list, p_nt);
        }
    }
    else
    {
        struct type* p = calloc(1, sizeof(struct type));
        p->category = TYPE_CATEGORY_ITSELF;
        type_flat_set_specifiers_using_declarator(p, pdeclarator);
        type_flat_set_qualifiers_using_declarator(p, pdeclarator);


        type_list_push_back(&list, p);

        //if (name)
        //{
          //  if (list.head->name_opt == NULL)
            //{
              //  list.head->name_opt = strdup(name);
            //}
        //}
        type_flat_set_qualifiers_using_declarator(list.tail, pdeclarator);
    }


    //type_flat_set_qualifiers_using_declarator(list.head, pdeclarator);

    if (pdeclarator->name)
    {
        free((void*)list.head->name_opt);
        list.head->name_opt = pdeclarator->name->lexeme;
    }
    return *list.head;
}

void type_remove_names(struct type* p_type)
{
    /*
      function parameters names are preserved
    */
    struct type* p = p_type;

    while (p)
    {
        if (p->name_opt)
        {
            free((void*)p->name_opt);
            p->name_opt = NULL;
        }
        p = p->next;
    }
}

const struct type* type_get_specifer_part(const struct type* p_type)
{
    /*
     last part is the specifier
    */
    const struct type* p = p_type;
    while (p->next) p = p->next;
    return p;
}



//#pragma once

struct format_visit_ctx
{
    struct ast ast;
    int identation;
};

void format_visit(struct format_visit_ctx* ctx);



//#pragma once

struct wasm_visit_ctx
{
    struct osstream ss;
    struct ast ast;
};

void wasm_visit(struct wasm_visit_ctx* ctx);



#ifdef _WIN32
#endif

#if defined _MSC_VER && !defined __POCC__
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
    int lambdas_index;
    
    struct token_list insert_before_declaration;
    struct token_list insert_before_block_item;
    struct ast ast;
    enum language_version target;
    struct defer_scope* tail_block;
};

void visit(struct visit_ctx* ctx);




struct defer_statement* defer_statement(struct parser_ctx* ctx);


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



#ifdef TEST
int printf_nothing(const char* fmt, ...) { return 0; }
#endif

void scope_destroy(struct scope* p)
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
}


void parser_ctx_destroy(struct parser_ctx* ctx)
{

}


void compiler_set_error_with_token(struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    ctx->n_errors++;

    if (p_token)
        print_position(ctx->printf, p_token->token_origin->lexeme, p_token->line, p_token->col);
    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    ctx->printf(LIGHTRED "error: " WHITE "%s\n", buffer);


    print_line_and_token(ctx->printf, p_token);
}


_Bool compiler_set_warning_with_token(enum compiler_warning w, struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    if (!(ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index] & w))
    {
        //not default, not added
        return 0;
    }
    ctx->n_warnings++;

    print_position(ctx->printf, p_token->token_origin->lexeme, p_token->line, p_token->col);

    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    ctx->printf(LIGHTMAGENTA "warning: " WHITE "%s [" LIGHTMAGENTA "-W%s" WHITE "]\n" RESET, buffer, get_warning_name(w));

    print_line_and_token(ctx->printf, p_token);
    return 1;
}


void compiler_set_info_with_token(struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...)
{
    ctx->n_info++;

    print_position(ctx->printf, p_token->token_origin->lexeme, p_token->line, p_token->col);
    char buffer[200] = { 0 };
    va_list args;
    va_start(args, fmt);
    /*int n =*/ vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    ctx->printf(LIGHTCYAN "note: " WHITE "%s\n", buffer);

    print_line_and_token(ctx->printf, p_token);
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
        p_token->type == TK_KEYWORD__ATOMIC;
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

    if (p_entry && p_entry->type == TAG_TYPE_DECLARATOR)
        return p_entry->p;

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
        else if (strcmp("_is_const", text) == 0) result = TK_KEYWORD_IS_CONST;
        else if (strcmp("_is_pointer", text) == 0) result = TK_KEYWORD_IS_POINTER;
        else if (strcmp("_is_array", text) == 0) result = TK_KEYWORD_IS_ARRAY;
        else if (strcmp("_is_function", text) == 0) result = TK_KEYWORD_IS_FUNCTION;
        else if (strcmp("_is_arithmetic", text) == 0) result = TK_KEYWORD_IS_ARITHMETIC;
        else if (strcmp("_is_floating_point", text) == 0) result = TK_KEYWORD_IS_FLOATING_POINT;
        else if (strcmp("_is_integral", text) == 0) result = TK_KEYWORD_IS_INTEGRAL;
        else if (strcmp("_is_scalar", text) == 0) result = TK_KEYWORD_IS_SCALAR;
        /*TRAITS EXTENSION*/

        else if (strcmp("_Hashof", text) == 0) result = TK_KEYWORD_HASHOF;
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
    struct stream stream = { .source = lexeme, .current = lexeme, .line = 1, .col = 1 };
    return parse_number_core(&stream, flags_opt);
}

static void pragma_skip_blanks(struct parser_ctx* ctx)
{
    while (ctx->current && ctx->current->type == TK_BLANKS)
    {
        ctx->current = ctx->current->next;
    }
}

static void parse_pragma(struct parser_ctx* ctx, struct token* token)
{
    if (ctx->current->type == TK_PRAGMA)
    {
        ctx->current = ctx->current->next;
        pragma_skip_blanks(ctx);

        if (ctx->current && strcmp(ctx->current->lexeme, "CAKE") == 0) {
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);
        }

        if (ctx->current && strcmp(ctx->current->lexeme, "diagnostic") == 0) {
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);
        }

        
        if (ctx->current && strcmp(ctx->current->lexeme, "push") == 0) {
            //#pragma GCC diagnostic push
            if (ctx->options.enabled_warnings_stack_top_index < 
                sizeof(ctx->options.enabled_warnings_stack) / sizeof(ctx->options.enabled_warnings_stack[0]))
            {
                ctx->options.enabled_warnings_stack_top_index++;
                ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index] =
                    ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index - 1];
            }
            else
            {
            }
            
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);
        }

        if (ctx->current && strcmp(ctx->current->lexeme, "pop") == 0) {
            
            if (ctx->options.enabled_warnings_stack_top_index > 0)
            {
                ctx->options.enabled_warnings_stack_top_index--;
            }
            else
            {
            }

            //#pragma GCC diagnostic pop
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);
        }

        if (ctx->current && strcmp(ctx->current->lexeme, "warning") == 0) {
            //#pragma CAKE diagnostic warning "-Wno-enum-compare"

            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);

            if (ctx->current && ctx->current->type == TK_STRING_LITERAL)
            {
                bool disable = false;
                enum compiler_warning  w =
                    get_warning_flag(ctx->current->lexeme + 1, &disable);
                if (disable)
                {
                    ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index] &= ~w;
                }
                else
                {
                    ctx->options.enabled_warnings_stack[ctx->options.enabled_warnings_stack_top_index] |= w;
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
            compiler_set_error_with_token(ctx, ctx->current, "expected %s", get_token_name(type));
            error = 1;
        }

        ctx->previous = ctx->current;
        ctx->current = ctx->current->next;
        parser_skip_blanks(ctx);
    }
    else
    {
        compiler_set_error_with_token(ctx, ctx->input_list.tail, "unexpected end of file after");
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
            compiler_set_error_with_token(ctx, ctx->current, "cannot combine with previous 'long long' declaration specifier");
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

struct declaration_specifiers* declaration_specifiers(struct parser_ctx* ctx)
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

            struct declaration_specifier* p_declaration_specifier = declaration_specifier(ctx);

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
            attribute_specifier_sequence_opt(ctx);

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
        final_specifier(ctx, &p_declaration_specifiers->type_specifier_flags);
    }


    return p_declaration_specifiers;
}

struct declaration* declaration_core(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt /*SINK*/,
    bool can_be_function_definition,
    bool* is_function_definition
)
{
    /*
                                  declaration-specifiers init-declarator-list_opt ;
     attribute-specifier-sequence declaration-specifiers init-declarator-list ;
     static_assert-declaration
     attribute-declaration
  */


    struct declaration* p_declaration = calloc(1, sizeof(struct declaration));

    p_declaration->p_attribute_specifier_sequence_opt = p_attribute_specifier_sequence_opt;

    p_declaration->first_token = ctx->current;

    if (ctx->current->type == ';')
    {
        parser_match_tk(ctx, ';');
        //declaracao vazia nao eh erro
        return p_declaration;
    }

    if (first_of_static_assert_declaration(ctx))
    {
        if (p_attribute_specifier_sequence_opt)
        {
            compiler_set_error_with_token(ctx, ctx->current, "");
        }

        p_declaration->static_assert_declaration = static_assert_declaration(ctx);
    }
    else
    {

        if (first_of_declaration_specifier(ctx))
        {
            p_declaration->declaration_specifiers = declaration_specifiers(ctx);

            if (p_declaration->p_attribute_specifier_sequence_opt)
            {
                p_declaration->declaration_specifiers->attributes_flags =
                    p_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
            }

            if (ctx->current->type != ';')
            {
                p_declaration->init_declarator_list = init_declarator_list(ctx,
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
                compiler_set_error_with_token(ctx, ctx->current, "invalid type '%s'", ctx->current->lexeme);
            }
            else
            {
                compiler_set_error_with_token(ctx, ctx->current, "expected declaration not '%s'", ctx->current->lexeme);
            }
            parser_match(ctx); //we need to go ahead
        }
    }


    return p_declaration;
}

struct declaration* function_definition_or_declaration(struct parser_ctx* ctx)
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

    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt =
        attribute_specifier_sequence_opt(ctx);


    bool is_function_definition = false;
    struct declaration* p_declaration = declaration_core(ctx, p_attribute_specifier_sequence_opt, true, &is_function_definition);
    if (is_function_definition)
    {
        naming_convention_function(ctx, p_declaration->init_declarator_list.head->declarator->direct_declarator->name_opt);


        ctx->p_current_function_opt = p_declaration;
        //tem que ter 1 so
        //tem 1 que ter  1 cara e ser funcao
        assert(p_declaration->init_declarator_list.head->declarator->direct_declarator->function_declarator);

        /*
            scope of parameters is the inner declarator

            void (*f(int i))(void) {
                i = 1;
                return 0;
            }
        */

        struct declarator* inner = p_declaration->init_declarator_list.head->declarator;
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



        //o function_prototype_scope era um block_scope
        p_declaration->function_body = function_body(ctx);
        p_declaration->init_declarator_list.head->declarator->function_body = p_declaration->function_body;


        struct parameter_declaration* parameter = NULL;

        if (p_declaration->init_declarator_list.head->declarator->direct_declarator->function_declarator &&
            p_declaration->init_declarator_list.head->declarator->direct_declarator->function_declarator->parameter_type_list_opt &&
            p_declaration->init_declarator_list.head->declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
        {
            parameter = p_declaration->init_declarator_list.head->declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
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

struct declaration* declaration(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt
)
{
    bool is_function_definition;
    return declaration_core(ctx, p_attribute_specifier_sequence_opt, false, &is_function_definition);
}


//(6.7) declaration-specifiers:
//declaration-specifier attribute-specifier-sequenceopt
//declaration-specifier declaration-specifiers



struct declaration_specifier* declaration_specifier(struct parser_ctx* ctx)
{
    //    storage-class-specifier
    //    type-specifier-qualifier
    //    function-specifier
    struct declaration_specifier* p_declaration_specifier = calloc(1, sizeof * p_declaration_specifier);
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
        compiler_set_error_with_token(ctx, ctx->current, "unexpected");
    }
    return p_declaration_specifier;
}


struct init_declarator* init_declarator(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt)
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
            &tkname);

        if (p_init_declarator->declarator == NULL) throw;

        p_init_declarator->declarator->name = tkname;


        if (tkname == NULL)
        {
            compiler_set_error_with_token(ctx, ctx->current, "empty declarator name?? unexpected");

            return p_init_declarator;
        }

        if (p_attribute_specifier_sequence_opt &&
            p_attribute_specifier_sequence_opt->attributes_flags)
        {
            if (p_attribute_specifier_sequence_opt->attributes_flags & CUSTOM_ATTRIBUTE_FREE)
            {
                p_init_declarator->declarator->static_analisys_flags |= MUST_FREE;
            }
            if (p_attribute_specifier_sequence_opt->attributes_flags & CUSTOM_ATTRIBUTE_DESTROY)
            {
                p_init_declarator->declarator->static_analisys_flags |= MUST_DESTROY;
            }
        }

        p_init_declarator->declarator->declaration_specifiers = p_declaration_specifiers;
        p_init_declarator->declarator->name = tkname;

        if (p_init_declarator->declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
        {
            /*
              auto requires we find the type after initializer
            */
        }
        else
        {
            p_init_declarator->declarator->type =
                make_type_using_declarator(ctx, p_init_declarator->declarator);

            if (type_is_array(&p_init_declarator->declarator->type))
            {
                if (p_init_declarator->declarator->type.type_qualifier_flags != 0 ||
                    p_init_declarator->declarator->type.static_array)
                {
                    compiler_set_error_with_token(ctx,
                        p_init_declarator->declarator->first_token,
                        "static or type qualifiers are not allowed in non-parameter array declarator");
                }
            }

            if ((p_init_declarator->declarator->type.type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION) &&
                type_is_destroy(&p_init_declarator->declarator->type) &&
                !type_is_pointer(&p_init_declarator->declarator->type))
            {
                p_init_declarator->declarator->static_analisys_flags = MUST_DESTROY | ISVALID;
            }
        }

        const char* name = p_init_declarator->declarator->name->lexeme;
        if (name)
        {
            struct scope* out = NULL;
            struct declarator* previous = find_declarator(ctx, name, &out);
            if (previous)
            {
                if (out->scope_level == ctx->scopes.tail->scope_level)
                {
                    if (out->scope_level == 0)
                    {
                        /*file scope*/
                        if (!type_is_same(&previous->type, &p_init_declarator->declarator->type, true))
                        {
                            //TODO failing on windows headers
                            //parser_seterror_with_token(ctx, p_init_declarator->declarator->name, "redeclaration of  '%s' with diferent types", previous->name->lexeme);
                            //parser_set_info_with_token(ctx, previous->name, "previous declaration");
                        }
                    }
                    else
                    {
                        compiler_set_error_with_token(ctx, ctx->current, "redeclaration");
                        compiler_set_info_with_token(ctx, previous->name, "previous declaration");
                    }
                }
                else
                {
                    hashmap_set(&ctx->scopes.tail->variables, name, p_init_declarator->declarator, TAG_TYPE_DECLARATOR);

                    /*global scope no warning...*/
                    if (out->scope_level != 0)
                    {
                        /*but redeclaration at function scope we show warning*/
                        if (compiler_set_warning_with_token(W_DECLARATOR_HIDE, ctx, p_init_declarator->declarator->first_token, "declaration of '%s' hides previous declaration", name))
                        {
                            compiler_set_info_with_token(ctx, previous->first_token, "previous declaration is here");
                        }
                    }
                }
            }
            else
            {
                /*first time we see this declarator*/
                hashmap_set(&ctx->scopes.tail->variables, name, p_init_declarator->declarator, TAG_TYPE_DECLARATOR);
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
                if (type_is_array(&p_init_declarator->declarator->type))
                {
                    const int sz = type_get_array_size(&p_init_declarator->declarator->type);
                    if (sz == 0)
                    {
                        /*int a[] = {1, 2, 3}*/
                        const int braced_initializer_size =
                            p_init_declarator->initializer->braced_initializer->initializer_list->size;

                        type_set_array_size(&p_init_declarator->declarator->type, braced_initializer_size);
                    }
                }
            }
            else if (p_init_declarator->initializer->assignment_expression)
            {
                if (p_init_declarator->initializer->assignment_expression->expression_type == POSTFIX_FUNCTION_CALL &&
                    type_is_function(&p_init_declarator->initializer->assignment_expression->left->type))
                {
                    /*
                      FILE * f = fopen();
                    */

                    p_init_declarator->declarator->static_analisys_flags |=
                        p_init_declarator->initializer->assignment_expression->left->declarator->static_analisys_flags;
                }


                if ((p_init_declarator->declarator->type.type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION) &&
                    (p_init_declarator->declarator->static_analisys_flags & MUST_FREE) &&
                    type_is_nodiscard(&p_init_declarator->declarator->type) &&
                    type_is_pointer(&p_init_declarator->declarator->type))
                {
                    /*pointer to MUST_FREE of a struct [[nodiscard]] has must_destroy*/
                    p_init_declarator->declarator->static_analisys_flags |= (MUST_DESTROY);
                }
            }
            /*
               auto requires we find the type after initializer
            */
            if (p_init_declarator->declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
            {
                if (p_init_declarator->initializer &&
                    p_init_declarator->initializer->assignment_expression)
                {
                    struct type t = { 0 };

                    if (p_init_declarator->initializer->assignment_expression->expression_type == UNARY_EXPRESSION_ADDRESSOF)
                    {
                        t = type_dup(&p_init_declarator->initializer->assignment_expression->type);
                    }
                    else
                    {
                        struct type t2 = type_lvalue_conversion(&p_init_declarator->initializer->assignment_expression->type);
                        struct type temp = t2;
                        t2 = t;
                        t = temp;
                        type_destroy(&t2);
                    }

                    type_remove_names(&t);
                    t.name_opt = strdup(p_init_declarator->declarator->name->lexeme);

                    type_flat_set_qualifiers_using_declarator(&t, p_init_declarator->declarator);
                    type_visit_to_mark_anonymous(&t);
                    type_swap(&p_init_declarator->declarator->type, &t);
                    type_destroy(&t);
                }
            }
        }
        else
        {
            p_init_declarator->declarator->static_analisys_flags |= UNINITIALIZED;
        }
    }
    catch
    {
    }
    return p_init_declarator;
}

struct init_declarator_list init_declarator_list(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt
)
{
    /*
    init-declarator-list:
      init-declarator
      init-declarator-list , init-declarator
    */
    struct init_declarator_list init_declarator_list = { 0 };
    struct init_declarator* p_init_declarator = NULL;

    try
    {
        p_init_declarator = init_declarator(ctx,
            p_declaration_specifiers,
            p_attribute_specifier_sequence_opt);
        if (p_init_declarator == NULL) throw;
        LIST_ADD(&init_declarator_list, p_init_declarator);
        p_init_declarator = NULL; /*MOVED*/

        while (ctx->current != NULL && ctx->current->type == ',')
        {
            parser_match(ctx);
            p_init_declarator = init_declarator(ctx, p_declaration_specifiers, p_attribute_specifier_sequence_opt);
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

struct typeof_specifier_argument* typeof_specifier_argument(struct parser_ctx* ctx)
{
    struct typeof_specifier_argument* new_typeof_specifier_argument = calloc(1, sizeof(struct typeof_specifier_argument));
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
            new_typeof_specifier_argument->expression = expression(ctx);
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

struct typeof_specifier* typeof_specifier(struct parser_ctx* ctx)
{
    struct typeof_specifier* p_typeof_specifier = NULL;
    try
    {
        p_typeof_specifier = calloc(1, sizeof(struct typeof_specifier));
        if (p_typeof_specifier == NULL) throw;

        p_typeof_specifier->first_token = ctx->current;

        const bool is_typeof_unqual = ctx->current->type == TK_KEYWORD_TYPEOF_UNQUAL;
        parser_match(ctx);
        if (parser_match_tk(ctx, '(') != 0) throw;

        p_typeof_specifier->typeof_specifier_argument = typeof_specifier_argument(ctx);
        if (p_typeof_specifier->typeof_specifier_argument == NULL) throw;

        if (p_typeof_specifier->typeof_specifier_argument->expression)
        {
            p_typeof_specifier->type = type_dup(&p_typeof_specifier->typeof_specifier_argument->expression->type);
        }
        else if (p_typeof_specifier->typeof_specifier_argument->type_name)
        {
            p_typeof_specifier->type = type_dup(&p_typeof_specifier->typeof_specifier_argument->type_name->declarator->type);
        }

        if (p_typeof_specifier->type.attributes_flags & CUSTOM_ATTRIBUTE_PARAM)
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

struct type_specifier* type_specifier(struct parser_ctx* ctx)
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

struct struct_or_union_specifier* struct_or_union_specifier(struct parser_ctx* ctx)
{
    struct struct_or_union_specifier* p_struct_or_union_specifier = calloc(1, sizeof * p_struct_or_union_specifier);

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
                compiler_set_error_with_token(ctx, ctx->current, "use of '%s' with tag type that does not match previous declaration.", ctx->current->lexeme);
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
        p_struct_or_union_specifier->member_declaration_list = member_declaration_list(ctx);
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

struct member_declarator* member_declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* p_specifier_qualifier_list
)
{
    /*
    member-declarator:
     declarator
     declaratoropt : constant-expression
    */
    struct member_declarator* p_member_declarator = calloc(1, sizeof(struct member_declarator));

    struct token* p_token_name = NULL;

    p_member_declarator->declarator = declarator(ctx, p_specifier_qualifier_list, /*declaration_specifiers*/NULL, false, &p_token_name);
    p_member_declarator->declarator->name = p_token_name;
    p_member_declarator->declarator->specifier_qualifier_list = p_specifier_qualifier_list;

    p_member_declarator->declarator->type =
        make_type_using_declarator(ctx, p_member_declarator->declarator);

    if (p_member_declarator->declarator->name)
        naming_convention_struct_member(ctx, p_member_declarator->declarator->name, &p_member_declarator->declarator->type);

    if (ctx->current->type == ':')
    {
        parser_match(ctx);
        p_member_declarator->constant_expression = constant_expression(ctx);
    }
    return p_member_declarator;
}

struct member_declarator_list* member_declarator_list(struct parser_ctx* ctx,
    struct specifier_qualifier_list* p_specifier_qualifier_list)
{
    struct member_declarator_list* p_member_declarator_list = calloc(1, sizeof(struct member_declarator_list));
    LIST_ADD(p_member_declarator_list, member_declarator(ctx, p_specifier_qualifier_list));
    while (ctx->current->type == ',')
    {
        parser_match(ctx);
        LIST_ADD(p_member_declarator_list, member_declarator(ctx, p_specifier_qualifier_list));
    }
    return p_member_declarator_list;
}


struct member_declaration_list member_declaration_list(struct parser_ctx* ctx)
{
    struct member_declaration_list list = { 0 };
    //member_declaration
    //member_declaration_list member_declaration

    struct member_declaration* p_member_declaration = NULL;

    try
    {
        p_member_declaration = member_declaration(ctx);
        if (p_member_declaration == NULL) throw;
        LIST_ADD(&list, p_member_declaration);
        p_member_declaration = NULL; /*MOVED*/

        while (ctx->current && ctx->current->type != '}')
        {
            p_member_declaration = member_declaration(ctx);
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

struct member_declaration* member_declaration(struct parser_ctx* ctx)
{
    struct member_declaration* p_member_declaration = calloc(1, sizeof(struct member_declaration));
    //attribute_specifier_sequence_opt specifier_qualifier_list member_declarator_list_opt ';'
    //static_assert_declaration
    if (ctx->current->type == TK_KEYWORD__STATIC_ASSERT)
    {
        p_member_declaration->static_assert_declaration = static_assert_declaration(ctx);
    }
    else
    {
        attribute_specifier_sequence_opt(ctx);
        p_member_declaration->specifier_qualifier_list = specifier_qualifier_list(ctx);
        if (ctx->current->type != ';')
        {
            p_member_declaration->member_declarator_list_opt = member_declarator_list(ctx, p_member_declaration->specifier_qualifier_list);
        }
        parser_match_tk(ctx, ';');
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
                if (strcmp(p_member_declarator->declarator->name->lexeme, name) == 0)
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
                struct member_declaration_list* p_member_declaration_list =
                    &p_member_declaration->specifier_qualifier_list->struct_or_union_specifier->member_declaration_list;

                struct member_declarator* p = find_member_declarator(p_member_declaration_list, name);
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



struct specifier_qualifier_list* specifier_qualifier_list(struct parser_ctx* ctx)
{
    struct specifier_qualifier_list* p_specifier_qualifier_list = calloc(1, sizeof(struct specifier_qualifier_list));
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

            struct type_specifier_qualifier* p_type_specifier_qualifier = type_specifier_qualifier(ctx);

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
            attribute_specifier_sequence_opt(ctx);
        }
    }
    catch
    {
    }

    final_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags);
    p_specifier_qualifier_list->last_token = previous_parser_token(ctx->current);
    return p_specifier_qualifier_list;
}

struct type_specifier_qualifier* type_specifier_qualifier(struct parser_ctx* ctx)
{
    struct type_specifier_qualifier* type_specifier_qualifier = calloc(1, sizeof * type_specifier_qualifier);
    //type_specifier
    //type_qualifier
    //alignment_specifier
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


struct enum_specifier* enum_specifier(struct parser_ctx* ctx)
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
    struct enum_specifier* p_enum_specifier = NULL;
    try
    {
        p_enum_specifier = calloc(1, sizeof * p_enum_specifier);

        p_enum_specifier->first_token = ctx->current;
        parser_match_tk(ctx, TK_KEYWORD_ENUM);

        p_enum_specifier->attribute_specifier_sequence_opt =
            attribute_specifier_sequence_opt(ctx);


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
            p_enum_specifier->specifier_qualifier_list = specifier_qualifier_list(ctx);
        }

        if (ctx->current->type == '{')
        {
            if (p_enum_specifier->tag_token)
                naming_convention_enum_tag(ctx, p_enum_specifier->tag_token);

            /*points to itself*/
            p_enum_specifier->complete_enum_specifier = p_enum_specifier;

            parser_match_tk(ctx, '{');
            p_enum_specifier->enumerator_list = enumerator_list(ctx, p_enum_specifier);
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
                compiler_set_error_with_token(ctx, ctx->current, "missing enum tag name");
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
                    compiler_set_error_with_token(ctx, p_enum_specifier->tag_token, "multiple definition of 'enum %s'",
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
                compiler_set_error_with_token(ctx, ctx->current, "use of '%s' with tag type that does not match previous declaration.", ctx->current->lexeme);
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

    struct enumerator_list enumeratorlist = { 0 };
    struct enumerator* p_enumerator = NULL;
    try
    {


        p_enumerator = enumerator(ctx, p_enum_specifier);
        if (p_enumerator == NULL) throw;
        LIST_ADD(&enumeratorlist, p_enumerator);


        while (ctx->current != NULL && ctx->current->type == ',')
        {
            parser_match(ctx);  /*pode ter uma , vazia no fim*/

            if (ctx->current && ctx->current->type != '}')
            {
                p_enumerator = enumerator(ctx, p_enum_specifier);
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

struct enumerator* enumerator(struct parser_ctx* ctx,
    struct enum_specifier* p_enum_specifier)
{
    //TODO VALUE
    struct enumerator* p_enumerator = calloc(1, sizeof(struct enumerator));
    p_enumerator->enum_specifier = p_enum_specifier;
    struct token* name = ctx->current;

    naming_convention_enumerator(ctx, name);

    parser_match_tk(ctx, TK_IDENTIFIER);

    p_enumerator->attribute_specifier_sequence_opt =
        attribute_specifier_sequence_opt(ctx);

    p_enumerator->token = name;
    hashmap_set(&ctx->scopes.tail->variables, p_enumerator->token->lexeme, p_enumerator, TAG_TYPE_ENUMERATOR);

    if (ctx->current->type == '=')
    {
        parser_match(ctx);
        p_enumerator->constant_expression_opt = constant_expression(ctx);
        p_enumerator->value = constant_value_to_ll(&p_enumerator->constant_expression_opt->constant_value);
    }

    return p_enumerator;
}




struct alignment_specifier* alignment_specifier(struct parser_ctx* ctx)
{
    struct alignment_specifier* alignment_specifier = calloc(1, sizeof * alignment_specifier);
    alignment_specifier->token = ctx->current;
    parser_match_tk(ctx, TK_KEYWORD__ALIGNAS);
    parser_match_tk(ctx, '(');
    if (first_of_type_name(ctx))
    {

        type_name(ctx);

    }
    else
    {
        constant_expression(ctx);
    }
    parser_match_tk(ctx, ')');
    return alignment_specifier;
}



struct atomic_type_specifier* atomic_type_specifier(struct parser_ctx* ctx)
{
    //'_Atomic' '(' type_name ')'
    struct atomic_type_specifier* p = calloc(1, sizeof * p);
    p->token = ctx->current;
    parser_match_tk(ctx, TK_KEYWORD__ATOMIC);
    parser_match_tk(ctx, '(');
    p->type_name = type_name(ctx);
    parser_match_tk(ctx, ')');
    return p;
}


struct type_qualifier* type_qualifier(struct parser_ctx* ctx)
{
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

    p_type_qualifier->token = ctx->current;

    //'const'
    //'restrict'
    //'volatile'
    //'_Atomic'
    parser_match(ctx);
    return p_type_qualifier;
}
//

struct type_qualifier* type_qualifier_opt(struct parser_ctx* ctx)
{
    if (first_of_type_qualifier(ctx))
    {
        return type_qualifier(ctx);
    }
    return NULL;
}


struct function_specifier* function_specifier(struct parser_ctx* ctx)
{
    if (ctx->current->type == TK_KEYWORD__NORETURN)
    {
        compiler_set_info_with_token(ctx, ctx->current, "_Noreturn is deprecated use attributes");
    }

    struct function_specifier* p_function_specifier = NULL;
    try
    {
        p_function_specifier = calloc(1, sizeof * p_function_specifier);
        if (p_function_specifier == NULL) throw;

        p_function_specifier->token = ctx->current;
        parser_match(ctx);

    }
    catch
    {
    }

    return p_function_specifier;
}


struct declarator* declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* p_specifier_qualifier_list,
    struct declaration_specifiers* p_declaration_specifiers,
    bool abstract_acceptable,
    struct token** pp_token_name)
{
    /*
      declarator:
      pointer_opt direct-declarator
    */
    struct declarator* p_declarator = calloc(1, sizeof(struct declarator));
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

struct array_declarator* array_declarator(struct direct_declarator* p_direct_declarator, struct parser_ctx* ctx);
struct function_declarator* function_declarator(struct direct_declarator* p_direct_declarator, struct parser_ctx* ctx);

struct direct_declarator* direct_declarator(struct parser_ctx* ctx,
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
    struct direct_declarator* p_direct_declarator = calloc(1, sizeof(struct direct_declarator));
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
            attribute_specifier_sequence_opt(ctx);
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
                    abstract_acceptable,
                    pptoken_name);


                parser_match(ctx); //)
            }
        }


        while (ctx->current != NULL &&
            (ctx->current->type == '[' || ctx->current->type == '('))
        {
            struct direct_declarator* p_direct_declarator2 = calloc(1, sizeof(struct direct_declarator));

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


struct array_declarator* array_declarator(struct direct_declarator* p_direct_declarator, struct parser_ctx* ctx)
{
    //direct_declarator '['          type_qualifier_list_opt           assignment_expression_opt ']'
    //direct_declarator '[' 'static' type_qualifier_list_opt           assignment_expression     ']'
    //direct_declarator '['          type_qualifier_list      'static' assignment_expression     ']'
    //direct_declarator '['          type_qualifier_list_opt  '*'           ']'

    struct array_declarator* p_array_declarator = NULL;
    try
    {
        p_array_declarator = calloc(1, sizeof * p_array_declarator);
        if (p_array_declarator == NULL) throw;

        p_array_declarator->direct_declarator = p_direct_declarator;
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
            //tem que ter..

            p_array_declarator->assignment_expression = assignment_expression(ctx);
            if (p_array_declarator->assignment_expression == NULL) throw;

            p_array_declarator->constant_size = constant_value_to_ull(&p_array_declarator->assignment_expression->constant_value);
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

                p_array_declarator->assignment_expression = assignment_expression(ctx);
                if (p_array_declarator->assignment_expression == NULL) throw;

                p_array_declarator->constant_size = constant_value_to_ull(&p_array_declarator->assignment_expression->constant_value);
            }
            else
            {
                //int a [] = {};
                p_array_declarator->constant_size = 0;
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


struct function_declarator* function_declarator(struct direct_declarator* p_direct_declarator, struct parser_ctx* ctx)
{
    struct function_declarator* p_function_declarator = calloc(1, sizeof(struct function_declarator));
    //faz um push da funcion_scope_declarator_list que esta vivendo mais em cima
    //eh feito o pop mais em cima tb.. aqui dentro do decide usar.
    //ctx->funcion_scope_declarator_list->outer_scope = ctx->current_scope;
    //ctx->current_scope = ctx->funcion_scope_declarator_list;
    //direct_declarator '(' parameter_type_list_opt ')'


    p_function_declarator->direct_declarator = p_direct_declarator;
    p_function_declarator->parameters_scope.scope_level = ctx->scopes.tail->scope_level + 1;
    p_function_declarator->parameters_scope.variables.capacity = 5;
    p_function_declarator->parameters_scope.tags.capacity = 1;


    scope_list_push(&ctx->scopes, &p_function_declarator->parameters_scope);

    //print_scope(&ctx->scopes);

    parser_match_tk(ctx, '(');
    if (ctx->current->type != ')')
    {
        p_function_declarator->parameter_type_list_opt = parameter_type_list(ctx);
    }
    parser_match_tk(ctx, ')');

    //print_scope(&ctx->scopes);

    scope_list_pop(&ctx->scopes);

    //print_scope(&ctx->scopes);


    return p_function_declarator;
}


struct pointer* pointer_opt(struct parser_ctx* ctx)
{
    struct pointer* p = NULL;
    struct pointer* p_pointer = NULL;
    try
    {
        while (ctx->current != NULL && ctx->current->type == '*')
        {
            p_pointer = calloc(1, sizeof(struct pointer));
            if (p_pointer == NULL) throw;
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


struct type_qualifier_list* type_qualifier_list(struct parser_ctx* ctx)
{
    //type_qualifier
    //type_qualifier_list type_qualifier

    struct type_qualifier_list* p_type_qualifier_list = NULL;
    struct type_qualifier* p_type_qualifier = NULL;

    try
    {
        p_type_qualifier_list = calloc(1, sizeof(struct type_qualifier_list));
        if (p_type_qualifier_list == NULL) throw;


        p_type_qualifier = type_qualifier(ctx);
        if (p_type_qualifier == NULL) throw;

        p_type_qualifier_list->flags |= p_type_qualifier->flags;
        LIST_ADD(p_type_qualifier_list, p_type_qualifier);
        p_type_qualifier = NULL; /*MOVED*/

        while (ctx->current != NULL && first_of_type_qualifier(ctx))
        {
            p_type_qualifier = type_qualifier(ctx);
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


struct parameter_type_list* parameter_type_list(struct parser_ctx* ctx)
{
    struct parameter_type_list* p_parameter_type_list = calloc(1, sizeof(struct parameter_type_list));
    //parameter_list
    //parameter_list ',' '...'
    p_parameter_type_list->parameter_list = parameter_list(ctx);

    if (p_parameter_type_list->parameter_list->head ==
        p_parameter_type_list->parameter_list->tail)
    {
        if (p_parameter_type_list->parameter_list->head->declaration_specifiers->type_specifier_flags == TYPE_SPECIFIER_VOID &&
            p_parameter_type_list->parameter_list->head->declarator->pointer == NULL)
        {
            //pattern f(void)
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


struct parameter_list* parameter_list(struct parser_ctx* ctx)
{
    /*
      parameter_list
      parameter_declaration
      parameter_list ',' parameter_declaration
    */
    struct parameter_list* p_parameter_list = NULL;
    struct parameter_declaration* p_parameter_declaration = NULL;
    try
    {
        p_parameter_list = calloc(1, sizeof(struct parameter_list));
        if (p_parameter_list == NULL) throw;

        p_parameter_declaration = parameter_declaration(ctx);
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

            p_parameter_declaration = parameter_declaration(ctx);
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


struct parameter_declaration* parameter_declaration(struct parser_ctx* ctx)
{
    struct parameter_declaration* p_parameter_declaration = calloc(1, sizeof(struct parameter_declaration));

    p_parameter_declaration->attribute_specifier_sequence_opt =
        attribute_specifier_sequence_opt(ctx);

    p_parameter_declaration->declaration_specifiers = declaration_specifiers(ctx);

    if (p_parameter_declaration->attribute_specifier_sequence_opt)
    {
        p_parameter_declaration->declaration_specifiers->attributes_flags =
            p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags;
    }

    //talvez no ctx colocar um flag que esta em argumentos
    //TODO se tiver uma struct tag novo...
    //warning: declaration of 'struct X' will not be visible outside of this function [-Wvisibility]
    struct token* p_token_name = 0;
    p_parameter_declaration->declarator = declarator(ctx,
        /*specifier_qualifier_list*/NULL,
        p_parameter_declaration->declaration_specifiers,
        true/*can be abstract*/,
        &p_token_name);
    p_parameter_declaration->declarator->name = p_token_name;


    if (p_parameter_declaration->attribute_specifier_sequence_opt)
    {
        if (p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags & CUSTOM_ATTRIBUTE_DESTROY)
        {
            p_parameter_declaration->declarator->static_analisys_flags |= MUST_DESTROY;
        }
        if (p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags & CUSTOM_ATTRIBUTE_FREE)
        {
            p_parameter_declaration->declarator->static_analisys_flags |= MUST_FREE;
        }
    }
    p_parameter_declaration->declarator->is_parameter_declarator = true;
    p_parameter_declaration->declarator->declaration_specifiers = p_parameter_declaration->declaration_specifiers;

    p_parameter_declaration->declarator->type =
        make_type_using_declarator(ctx, p_parameter_declaration->declarator);


    p_parameter_declaration->declarator->type.attributes_flags |= CUSTOM_ATTRIBUTE_PARAM;

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
            TAG_TYPE_DECLARATOR);
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
            struct type_specifier_qualifier* p_specifier_qualifier = calloc(1, sizeof(struct type_specifier_qualifier));

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

struct type_name* type_name(struct parser_ctx* ctx)
{
    struct type_name* p_type_name = calloc(1, sizeof(struct type_name));

    p_type_name->first_token = ctx->current;


    p_type_name->specifier_qualifier_list = specifier_qualifier_list(ctx);


    p_type_name->declarator = declarator(ctx,
        p_type_name->specifier_qualifier_list,//??
        /*declaration_specifiers*/ NULL,
        true /*DEVE SER TODO*/,
        NULL);
    p_type_name->declarator->specifier_qualifier_list = p_type_name->specifier_qualifier_list;
    p_type_name->declarator->type =
        make_type_using_declarator(ctx, p_type_name->declarator);


    p_type_name->last_token = ctx->current->prev;
    p_type_name->type = type_dup(&p_type_name->declarator->type);
    //p_type_name->type = make_type_using_declarator(ctx, p_type_name->declarator);

    return p_type_name;
}

struct braced_initializer* braced_initializer(struct parser_ctx* ctx)
{
    /*
     { }
     { initializer-list }
     { initializer-list , }
    */

    struct braced_initializer* p_bracket_initializer_list = calloc(1, sizeof(struct braced_initializer));
    p_bracket_initializer_list->first_token = ctx->current;
    parser_match_tk(ctx, '{');
    if (ctx->current->type != '}')
    {
        p_bracket_initializer_list->initializer_list = initializer_list(ctx);
    }
    parser_match_tk(ctx, '}');
    return p_bracket_initializer_list;
}



struct initializer* initializer(struct parser_ctx* ctx)
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
        p_initializer->braced_initializer = braced_initializer(ctx);
    }
    else
    {

        p_initializer->assignment_expression = assignment_expression(ctx);
    }
    return p_initializer;
}


struct initializer_list* initializer_list(struct parser_ctx* ctx)
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
        p_designation = designation(ctx);
    }
    struct initializer* p_initializer = initializer(ctx);
    p_initializer->designation = p_designation;
    LIST_ADD(p_initializer_list, p_initializer);
    p_initializer_list->size++;

    while (ctx->current != NULL && ctx->current->type == ',')
    {
        parser_match(ctx);
        if (ctx->current->type == '}')
            break; //follow

        if (first_of_designator(ctx))
        {
            p_initializer->designation = designation(ctx);
        }
        struct initializer* p_initializer2 = initializer(ctx);
        p_initializer2->designation = p_designation;
        LIST_ADD(p_initializer_list, p_initializer2);
        p_initializer_list->size++;
    }

    return p_initializer_list;
}


struct designation* designation(struct parser_ctx* ctx)
{
    //designator_list '='
    struct designation* p_designation = calloc(1, sizeof(struct designation));
    designator_list(ctx);
    parser_match_tk(ctx, '=');
    return p_designation;
}

struct designator_list* designator_list(struct parser_ctx* ctx)
{
    //designator
    //designator_list designator
    struct designator_list* p_designator_list = NULL;
    struct designator* p_designator = NULL;
    try
    {
        p_designator_list = calloc(1, sizeof(struct designator_list));
        if (p_designator_list == NULL) throw;

        p_designator = designator(ctx);
        if (p_designator == NULL) throw;
        LIST_ADD(p_designator_list, p_designator);
        p_designator = NULL; /*MOVED*/

        while (ctx->current != NULL && first_of_designator(ctx))
        {
            p_designator = designator(ctx);
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


struct designator* designator(struct parser_ctx* ctx)
{
    //'[' constant_expression ']'
    //'.' identifier
    struct designator* p_designator = calloc(1, sizeof(struct designator));
    if (ctx->current->type == '[')
    {
        parser_match_tk(ctx, '[');
        p_designator->constant_expression_opt = constant_expression(ctx);
        parser_match_tk(ctx, ']');
    }
    else if (ctx->current->type == '.')
    {
        parser_match(ctx);
        parser_match_tk(ctx, TK_IDENTIFIER);
    }
    return p_designator;
}





struct static_assert_declaration* static_assert_declaration(struct parser_ctx* ctx)
{

    /*
     static_assert-declaration:
      "static_assert" ( constant-expression , string-literal ) ;
      "static_assert" ( constant-expression ) ;
    */

    struct static_assert_declaration* p_static_assert_declaration = NULL;
    try
    {
        p_static_assert_declaration = calloc(1, sizeof(struct static_assert_declaration));
        if (p_static_assert_declaration == NULL) throw;

        p_static_assert_declaration->first_token = ctx->current;
        struct token* position = ctx->current;
        parser_match_tk(ctx, TK_KEYWORD__STATIC_ASSERT);
        parser_match_tk(ctx, '(');

        p_static_assert_declaration->constant_expression = constant_expression(ctx);
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
                compiler_set_error_with_token(ctx, position, "_Static_assert failed %s\n",
                    p_static_assert_declaration->string_literal_opt->lexeme);
            }
            else
            {
                compiler_set_error_with_token(ctx, position, "_Static_assert failed");
            }
        }
    }
    catch
    {}
    return p_static_assert_declaration;
}


struct attribute_specifier_sequence* attribute_specifier_sequence_opt(struct parser_ctx* ctx)
{
    struct attribute_specifier_sequence* p_attribute_specifier_sequence = NULL;

    if (first_of_attribute_specifier(ctx))
    {
        p_attribute_specifier_sequence = calloc(1, sizeof(struct attribute_specifier_sequence));

        p_attribute_specifier_sequence->first_token = ctx->current;

        while (ctx->current != NULL &&
            first_of_attribute_specifier(ctx))
        {
            struct attribute_specifier* p_attribute_specifier =
                attribute_specifier(ctx);

            p_attribute_specifier_sequence->attributes_flags |=
                p_attribute_specifier->attribute_list->attributes_flags;

            LIST_ADD(p_attribute_specifier_sequence, p_attribute_specifier);
        }
        p_attribute_specifier_sequence->last_token = ctx->previous;
    }



    return p_attribute_specifier_sequence;
}

struct attribute_specifier_sequence* attribute_specifier_sequence(struct parser_ctx* ctx)
{
    //attribute_specifier_sequence_opt attribute_specifier
    struct attribute_specifier_sequence* p_attribute_specifier_sequence = calloc(1, sizeof(struct attribute_specifier_sequence));
    while (ctx->current != NULL && first_of_attribute_specifier(ctx))
    {
        LIST_ADD(p_attribute_specifier_sequence, attribute_specifier(ctx));
    }
    return p_attribute_specifier_sequence;
}


struct attribute_specifier* attribute_specifier(struct parser_ctx* ctx)
{
    struct attribute_specifier* p_attribute_specifier = calloc(1, sizeof(struct attribute_specifier));

    p_attribute_specifier->first_token = ctx->current;

    //'[' '[' attribute_list ']' ']'
    parser_match_tk(ctx, '[');
    parser_match_tk(ctx, '[');
    p_attribute_specifier->attribute_list = attribute_list(ctx);
    parser_match_tk(ctx, ']');
    p_attribute_specifier->last_token = ctx->current;
    parser_match_tk(ctx, ']');
    return p_attribute_specifier;
}



struct attribute_list* attribute_list(struct parser_ctx* ctx)
{
    struct attribute_list* p_attribute_list = calloc(1, sizeof(struct attribute_list));
    //
    //attribute_list ',' attribute_opt
    while (ctx->current != NULL && (
        first_of_attribute(ctx) ||
        ctx->current->type == ','))
    {
        if (first_of_attribute(ctx))
        {
            struct attribute* p_attribute = attribute(ctx);
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

struct attribute* attribute(struct parser_ctx* ctx)
{
    struct attribute* p_attribute = calloc(1, sizeof(struct attribute));
    //attribute_token attribute_argument_clause_opt
    p_attribute->attribute_token = attribute_token(ctx);
    p_attribute->attributes_flags = p_attribute->attribute_token->attributes_flags;
    if (ctx->current->type == '(') //first
    {
        p_attribute->attribute_argument_clause = attribute_argument_clause(ctx);
    }
    return p_attribute;
}


struct attribute_token* attribute_token(struct parser_ctx* ctx)
{
    struct attribute_token* p_attribute_token = calloc(1, sizeof(struct attribute_token));

    struct token* attr_token = ctx->current;

    bool is_standard_attribute = false;
    if (strcmp(attr_token->lexeme, "deprecated") == 0) {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_DEPRECATED;
    }
    else if (strcmp(attr_token->lexeme, "fallthrough") == 0) {
        is_standard_attribute = true;
    }
    else if (strcmp(attr_token->lexeme, "maybe_unused") == 0) {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_MAYBE_UNUSED;
    }
    else if (strcmp(attr_token->lexeme, "noreturn") == 0) {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_NORETURN;
    }
    else if (strcmp(attr_token->lexeme, "reproducible") == 0) {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_REPRODUCIBLE;
    }
    else if (strcmp(attr_token->lexeme, "unsequenced") == 0) {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_UNSEQUENCED;
    }
    else if (strcmp(attr_token->lexeme, "nodiscard") == 0) {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = STD_ATTRIBUTE_NODISCARD;
    }
    else if (strcmp(attr_token->lexeme, "free") == 0) {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = CUSTOM_ATTRIBUTE_FREE;
    }
    else if (strcmp(attr_token->lexeme, "destroy") == 0) {
        is_standard_attribute = true;
        p_attribute_token->attributes_flags = CUSTOM_ATTRIBUTE_DESTROY;
    }

    parser_match_tk(ctx, TK_IDENTIFIER);

    if (ctx->current->type == '::')
    {
        parser_match(ctx);
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



struct attribute_argument_clause* attribute_argument_clause(struct parser_ctx* ctx)
{
    struct attribute_argument_clause* p_attribute_argument_clause = calloc(1, sizeof(struct attribute_argument_clause));
    //'(' balanced_token_sequence_opt ')'
    parser_match_tk(ctx, '(');
    balanced_token_sequence_opt(ctx);
    parser_match_tk(ctx, ')');
    return p_attribute_argument_clause;
}


struct balanced_token_sequence* balanced_token_sequence_opt(struct parser_ctx* ctx)
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
        compiler_set_error_with_token(ctx, ctx->current, "expected ']' before ')'");

    }
    if (count3 != 0)
    {
        compiler_set_error_with_token(ctx, ctx->current, "expected '}' before ')'");

    }
    return p_balanced_token_sequence;
}


struct statement* statement(struct parser_ctx* ctx)
{
    struct statement* p_statement = calloc(1, sizeof(struct statement));
    if (first_of_labeled_statement(ctx))
    {
        p_statement->labeled_statement = labeled_statement(ctx);
    }
    else
    {
        p_statement->unlabeled_statement = unlabeled_statement(ctx);
    }
    //labeled_statement
    //unlabeled_statement
    return p_statement;
}

struct primary_block* primary_block(struct parser_ctx* ctx)
{
    assert(ctx->current != NULL);
    struct primary_block* p_primary_block = calloc(1, sizeof(struct primary_block));
    if (first_of_compound_statement(ctx))
    {
        p_primary_block->compound_statement = compound_statement(ctx);
    }
    else if (first_of_selection_statement(ctx))
    {
        p_primary_block->selection_statement = selection_statement(ctx);
    }
    else if (first_of_iteration_statement(ctx))
    {
        p_primary_block->iteration_statement = iteration_statement(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_DEFER)
    {
        p_primary_block->defer_statement = defer_statement(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_TRY)
    {
        p_primary_block->try_statement = try_statement(ctx);
    }
    else
    {
        compiler_set_error_with_token(ctx, ctx->current, "unexpected token");
    }
    return p_primary_block;
}

struct secondary_block* secondary_block(struct parser_ctx* ctx)
{
    struct secondary_block* p_secondary_block = calloc(1, sizeof(struct secondary_block));
    p_secondary_block->first_token = ctx->current;


    p_secondary_block->statement = statement(ctx);

    p_secondary_block->last_token = ctx->previous;

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

struct unlabeled_statement* unlabeled_statement(struct parser_ctx* ctx)
{
    /*
     unlabeled-statement:
       expression-statement
       attribute-specifier-sequence opt primary-block
       attribute-specifier-sequence opt jump-statement
    */
    struct unlabeled_statement* p_unlabeled_statement = calloc(1, sizeof(struct unlabeled_statement));

    if (first_of_primary_block(ctx))
    {
        p_unlabeled_statement->primary_block = primary_block(ctx);
    }
    else if (first_of_jump_statement(ctx))
    {
        p_unlabeled_statement->jump_statement = jump_statement(ctx);
    }
    else
    {
        p_unlabeled_statement->expression_statement = expression_statement(ctx);
        if (p_unlabeled_statement->expression_statement)
        {
            if (p_unlabeled_statement->expression_statement->expression_opt &&
                p_unlabeled_statement->expression_statement->expression_opt->expression_type == POSTFIX_FUNCTION_CALL)
            {
                if (!type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type))
                {
                    if (ctx->options.nodiscard_is_default ||
                        type_is_nodiscard(&p_unlabeled_statement->expression_statement->expression_opt->type))
                    {
                        if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                        {
                            compiler_set_warning_with_token(W_ATTRIBUTES, ctx,
                                p_unlabeled_statement->expression_statement->expression_opt->first_token,
                                "ignoring return value of function declared with 'nodiscard' attribute");
                        }
                    }
                }
            }
            else {
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
                    if (ctx->current->level == 0)
                    {
                        compiler_set_warning_with_token(W_UNUSED_VALUE,
                            ctx,
                            ctx->current,
                            "expression not used");
                    }
                }
            }
        }
    }

    return p_unlabeled_statement;
}
struct label* label(struct parser_ctx* ctx)
{
    struct label* p_label = calloc(1, sizeof(struct label));
    if (ctx->current->type == TK_IDENTIFIER)
    {
        p_label->name = ctx->current;
        parser_match(ctx);
        parser_match_tk(ctx, ':');
    }
    else if (ctx->current->type == TK_KEYWORD_CASE)
    {
        parser_match(ctx);
        p_label->constant_expression = constant_expression(ctx);
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

struct labeled_statement* labeled_statement(struct parser_ctx* ctx)
{
    struct labeled_statement* p_labeled_statement = calloc(1, sizeof(struct labeled_statement));
    //label statement
    p_labeled_statement->label = label(ctx);
    p_labeled_statement->statement = statement(ctx);
    return p_labeled_statement;
}

struct compound_statement* compound_statement(struct parser_ctx* ctx)
{
    //'{' block_item_list_opt '}'
    struct compound_statement* p_compound_statement = calloc(1, sizeof(struct compound_statement));
    struct scope block_scope = { .variables.capacity = 10 };
    scope_list_push(&ctx->scopes, &block_scope);

    p_compound_statement->first_token = ctx->current;
    parser_match_tk(ctx, '{');

    if (ctx->current->type != '}')
    {
        p_compound_statement->block_item_list = block_item_list(ctx);
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

            if (entry->type != TAG_TYPE_DECLARATOR)
            {
                entry = entry->next;
                continue;
            }

            struct declarator* p_declarator = entry->p;

            if (p_declarator)
            {
                /*
                  let's print the declarators that were not cleared for these
                  flags
                */
                if (p_declarator->static_analisys_flags & MUST_DESTROY)
                {
                    compiler_set_error_with_token(ctx,
                        p_declarator->name,
                        "destructor of '%s' must be called before the end of scope",
                        p_declarator->name->lexeme);

                }

                if (p_declarator->static_analisys_flags & MUST_FREE)
                {

                    compiler_set_error_with_token(ctx,
                        p_declarator->name,
                        "free('%s') must be called before the end of scope",
                        p_declarator->name->lexeme);
                }

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
    struct block_item_list block_item_list = { 0 };
    struct block_item* p_block_item = NULL;
    try
    {
        p_block_item = block_item(ctx);
        if (p_block_item == NULL) throw;
        LIST_ADD(&block_item_list, p_block_item);
        p_block_item = NULL; /*MOVED*/

        while (ctx->current != NULL && ctx->current->type != '}') //follow
        {
            p_block_item = block_item(ctx);
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

struct block_item* block_item(struct parser_ctx* ctx)
{
    //   declaration
    //     unlabeled_statement
    //   label
    struct block_item* p_block_item = calloc(1, sizeof(struct block_item));


    /*
    * Attributes can be first of declaration, labels etc..
    * so it is better to parse it in advance.
    */
    struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt =
        attribute_specifier_sequence_opt(ctx);

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
        p_block_item->declaration = declaration(ctx, p_attribute_specifier_sequence_opt);

        p_attribute_specifier_sequence_opt = NULL; /*MOVED*/

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
        p_block_item->label = label(ctx);
    }
    else
    {
        p_block_item->unlabeled_statement = unlabeled_statement(ctx);
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


struct try_statement* try_statement(struct parser_ctx* ctx)
{
    struct try_statement* p_try_statement = calloc(1, sizeof(struct try_statement));

    p_try_statement->first_token = ctx->current;

    assert(ctx->current->type == TK_KEYWORD_TRY);
    const struct try_statement* try_statement_copy_opt = ctx->p_current_try_statement_opt;
    ctx->p_current_try_statement_opt = p_try_statement;
    ctx->try_catch_block_index++;
    p_try_statement->try_catch_block_index = ctx->try_catch_block_index;
    parser_match_tk(ctx, TK_KEYWORD_TRY);

    p_try_statement->secondary_block = secondary_block(ctx);
    /*retores the previous one*/
    ctx->p_current_try_statement_opt = try_statement_copy_opt;


    if (ctx->current->type == TK_KEYWORD_CATCH)
    {
        p_try_statement->catch_token_opt = ctx->current;
        parser_match(ctx);

        p_try_statement->catch_secondary_block_opt = secondary_block(ctx);
    }
    p_try_statement->last_token = ctx->previous;



    return p_try_statement;
}

struct selection_statement* selection_statement(struct parser_ctx* ctx)
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
        parser_match_tk(ctx, '(');
        if (first_of_declaration_specifier(ctx))
        {
            struct declaration_specifiers* p_declaration_specifiers = declaration_specifiers(ctx);
            struct init_declarator_list list = init_declarator_list(ctx, p_declaration_specifiers, NULL);
            p_selection_statement->init_declarator = list.head; //only one
            parser_match_tk(ctx, ';');
        }


        p_selection_statement->expression = expression(ctx);

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
        p_selection_statement->secondary_block = secondary_block(ctx);

        if (ctx->current)
        {
            if (ctx->current->type == TK_KEYWORD_ELSE)
            {
                p_selection_statement->else_token_opt = ctx->current;
                parser_match(ctx);
                p_selection_statement->else_secondary_block_opt = secondary_block(ctx);
            }
        }
        else
        {
            compiler_set_error_with_token(ctx, ctx->input_list.tail, "unexpected end of file");
        }
    }
    else if (ctx->current->type == TK_KEYWORD_SWITCH)
    {
        parser_match(ctx);
        parser_match_tk(ctx, '(');

        p_selection_statement->expression = expression(ctx);
        parser_match_tk(ctx, ')');
        p_selection_statement->secondary_block = secondary_block(ctx);

    }
    else
    {
        assert(false);
        compiler_set_error_with_token(ctx, ctx->input_list.tail, "unexpected token");
    }

    p_selection_statement->last_token = ctx->previous;

    scope_list_pop(&ctx->scopes);

    scope_destroy(&if_scope);

    return p_selection_statement;
}

struct defer_statement* defer_statement(struct parser_ctx* ctx)
{
    struct defer_statement* p_defer_statement = calloc(1, sizeof(struct defer_statement));
    if (ctx->current->type == TK_KEYWORD_DEFER)
    {
        p_defer_statement->first_token = ctx->current;
        parser_match(ctx);
        p_defer_statement->secondary_block = secondary_block(ctx);
        p_defer_statement->last_token = ctx->previous;
    }
    return p_defer_statement;
}

struct iteration_statement* iteration_statement(struct parser_ctx* ctx)
{
    /*
    iteration-statement:
      while ( expression ) statement
      do statement while ( expression ) ;
      for ( expressionopt ; expressionopt ; expressionopt ) statement
      for ( declaration expressionopt ; expressionopt ) statement
    */
    struct iteration_statement* p_iteration_statement = calloc(1, sizeof(struct iteration_statement));
    p_iteration_statement->first_token = ctx->current;
    if (ctx->current->type == TK_KEYWORD_DO)
    {
        parser_match(ctx);
        p_iteration_statement->secondary_block = secondary_block(ctx);
        p_iteration_statement->second_token = ctx->current;
        parser_match_tk(ctx, TK_KEYWORD_WHILE);
        parser_match_tk(ctx, '(');

        p_iteration_statement->expression1 = expression(ctx);
        parser_match_tk(ctx, ')');
        parser_match_tk(ctx, ';');
    }
    else if (ctx->current->type == TK_KEYWORD_REPEAT)
    {
        parser_match(ctx);
        p_iteration_statement->secondary_block = secondary_block(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_WHILE)
    {
        parser_match(ctx);
        parser_match_tk(ctx, '(');

        p_iteration_statement->expression1 = expression(ctx);
        parser_match_tk(ctx, ')');
        p_iteration_statement->secondary_block = secondary_block(ctx);
    }
    else if (ctx->current->type == TK_KEYWORD_FOR)
    {
        parser_match(ctx);
        parser_match_tk(ctx, '(');
        if (first_of_declaration_specifier(ctx))
        {
            struct scope for_scope = { 0 };
            scope_list_push(&ctx->scopes, &for_scope);

            struct attribute_specifier_sequence* p_attribute_specifier_sequence_opt =
                attribute_specifier_sequence_opt(ctx);

            declaration(ctx, p_attribute_specifier_sequence_opt);
            if (ctx->current->type != ';')
            {
                p_iteration_statement->expression1 = expression(ctx);
            }
            parser_match_tk(ctx, ';');
            if (ctx->current->type != ')')
                p_iteration_statement->expression2 = expression(ctx);

            parser_match_tk(ctx, ')');

            p_iteration_statement->secondary_block = secondary_block(ctx);

            scope_list_pop(&ctx->scopes);

            scope_destroy(&for_scope);
        }
        else
        {
            if (ctx->current->type != ';')
                expression(ctx);
            parser_match_tk(ctx, ';');
            if (ctx->current->type != ';')
                expression(ctx);
            parser_match_tk(ctx, ';');
            if (ctx->current->type != ')')
                p_iteration_statement->expression1 = expression(ctx);
            parser_match_tk(ctx, ')');

            p_iteration_statement->secondary_block = secondary_block(ctx);
        }
    }
    return p_iteration_statement;
}
struct jump_statement* jump_statement(struct parser_ctx* ctx)
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

            compiler_set_error_with_token(ctx, ctx->current, "throw statement not within try block");
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
            p_jump_statement->expression_opt = expression(ctx);

            if (p_jump_statement->expression_opt &&
                p_jump_statement->expression_opt->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
            {
                /*
                   returning a declarator will remove the flags must destroy or must free,
                   similar of moving
                */
                p_jump_statement->expression_opt->declarator->static_analisys_flags &= ~(MUST_DESTROY | MUST_FREE);
            }

            if (p_jump_statement->expression_opt)
            {
                /*
                * Check is return type is compatible with function return
                */
                struct type return_type =
                    get_function_return_type(&ctx->p_current_function_opt->init_declarator_list.head->declarator->type);

                check_assigment(ctx, &return_type, p_jump_statement->expression_opt);

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

struct expression_statement* expression_statement(struct parser_ctx* ctx)
{
    struct expression_statement* p_expression_statement = calloc(1, sizeof(struct expression_statement));
    /*
     expression-statement:
       expression opt ;
       attribute-specifier-sequence expression ;
    */

    p_expression_statement->p_attribute_specifier_sequence_opt =
        attribute_specifier_sequence_opt(ctx);

    if (ctx->current->type != ';')
    {
        p_expression_statement->expression_opt = expression(ctx);
    }

    parser_match_tk(ctx, ';');

    return p_expression_statement;
}

void declaration_list_destroy(struct declaration_list* list)
{

}

struct declaration_list translation_unit(struct parser_ctx* ctx)
{
    struct declaration_list declaration_list = { 0 };
    /*
      translation_unit:
      external_declaration
      translation_unit external_declaration
    */
    while (ctx->current != NULL)
    {
        LIST_ADD(&declaration_list, external_declaration(ctx));
    }
    return declaration_list;
}


struct declaration* external_declaration(struct parser_ctx* ctx)
{
    /*
     function_definition
     declaration
     */
    return function_definition_or_declaration(ctx);
}

struct compound_statement* function_body(struct parser_ctx* ctx)
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

            if (entry->type != TAG_TYPE_DECLARATOR)
            {
                entry = entry->next;
                continue;
            }

            struct declarator* p_declarator = entry->p;

            if (p_declarator &&
                p_declarator->first_token &&
                p_declarator->first_token->level == 0 &&
                declarator_is_function(p_declarator) &&
                (p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC))
            {
                /*
                  let's print the declarators that were not cleared for these
                  flags
                */
                if (p_declarator->static_analisys_flags & MUST_DESTROY)
                {
                    ctx->printf(WHITE "%s:%d:%d: ",
                        p_declarator->name->token_origin->lexeme,
                        p_declarator->name->line,
                        p_declarator->name->col);

                    if (p_declarator->static_analisys_flags & MUST_DESTROY)
                        ctx->printf(LIGHTMAGENTA "warning: " WHITE "MUST_DESTROY declarator flag of '%s' must be cleared before and of scope.\n",
                            p_declarator->name->lexeme);
                }

                if (p_declarator->static_analisys_flags & MUST_FREE)
                {
                    ctx->printf(WHITE "%s:%d:%d: ",
                        p_declarator->name->token_origin->lexeme,
                        p_declarator->name->line,
                        p_declarator->name->col);

                    if (p_declarator->static_analisys_flags & MUST_FREE)
                        ctx->printf(LIGHTMAGENTA "warning: " WHITE "MUST_FREE declarator flag of '%s' must be cleared before end of scope\n",
                            p_declarator->name->lexeme);
                }

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

struct declaration_list parse(struct options* options,
    struct token_list* list,
    struct report* report)
{

    s_anonymous_struct_count = 0;

    struct scope file_scope = { 0 };
    struct parser_ctx ctx = { .options = *options };
#ifdef TEST
    ctx.printf = printf_nothing;
#else
    ctx.printf = printf;
#endif


    scope_list_push(&ctx.scopes, &file_scope);
    ctx.input_list = *list;
    ctx.current = ctx.input_list.head;
    parser_skip_blanks(&ctx);

    struct declaration_list l = translation_unit(&ctx);
    show_unused_file_scope(&ctx);

    report->error_count = ctx.n_errors;
    report->warnings_count = ctx.n_warnings;
    report->info_count = ctx.n_info;

    parser_ctx_destroy(&ctx);

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
            struct tokenizer_ctx tctx = { 0 };
#ifdef TEST
            tctx.printf = printf_nothing;
#else
            tctx.printf = printf;
#endif
            struct token_list l1 = tokenizer(&tctx, buffer, "", 0, TK_FLAG_NONE);
            preprocessor(prectx, &l1, 0);
            token_list_clear(&l1);
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
"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Tools\\MSVC\\14.36.32532\\include;C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Tools\\MSVC\\14.36.32532\\ATLMFC\\include;C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\VS\\include;C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.22000.0\\ucrt;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\um;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\shared;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\winrt;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\cppwinrt;C:\\Program Files (x86)\\Windows Kits\\NETFXSDK\\4.8\\include\\um\n"\
"\n"

#define STR0 \
"C:\\Program Files\\Microsoft Visual Studio\\2022\\Preview\\VC\\Tools\\MSVC\\14.36.32522\\include;C:\\Program Files\\Microsoft Visual Studio\\2022\\Preview\\VC\\Auxiliary\\VS\\include;C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.22000.0\\ucrt;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\um;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\shared;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\winrt;C:\\Program Files (x86)\\Windows Kits\\10\\\\include\\10.0.22000.0\\\\cppwinrt\n"

        //http://thradams.com/app/litapp.html
        snprintf(env, sizeof env,
            "%s",
            STR);


        n = (int)strlen(env);
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
            char filename_local[500] = { 0 };
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

const char* format_code(struct options* options, const char* content)
{
    struct ast ast = { 0 };
    const char* s = NULL;


    struct preprocessor_ctx prectx = { 0 };

#ifdef TEST
    prectx.printf = printf_nothing;
#else
    prectx.printf = printf;
#endif


    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx);


    try
    {
        prectx.options = *options;
        append_msvc_include_dir(&prectx);

        struct tokenizer_ctx tctx = { 0 };
#ifdef TEST
        tctx.printf = printf_nothing;
#else
        tctx.printf = printf;
#endif

        struct token_list tokens = tokenizer(&tctx, content, "", 0, TK_FLAG_NONE);
        ast.token_list = preprocessor(&prectx, &tokens, 0);
        if (prectx.n_errors != 0) throw;

        struct report report = { 0 };
        ast.declaration_list = parse(options, &ast.token_list, &report);
        if (report.error_count > 0) throw;

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
{

}

void ast_wasm_visit(struct ast* ast)
{
    struct wasm_visit_ctx ctx = { 0 };
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

    struct preprocessor_ctx prectx = { 0 };

#ifdef TEST
    prectx.printf = printf_nothing;
#else
    prectx.printf = printf;
#endif


    prectx.macros.capacity = 5000;

    add_standard_macros(&prectx);

    //print_all_macros(&prectx);

    struct ast ast = { 0 };

    const char* s = NULL;

    try
    {


        if (fill_preprocessor_options(argc, argv, &prectx) != 0)
        {
            throw;
        }

        prectx.options = *options;
        append_msvc_include_dir(&prectx);


        char* content = readfile(file_name);
        if (content == NULL)
        {
            report->error_count++;
            printf("file not found '%s'\n", file_name);
            throw;
        }

        struct tokenizer_ctx tctx = { 0 };
#ifdef TEST
        tctx.printf = printf_nothing;
#else
        tctx.printf = printf;
#endif

        struct token_list tokens = tokenizer(&tctx, content, file_name, 0, TK_FLAG_NONE);


        ast.token_list = preprocessor(&prectx, &tokens, 0);
        if (prectx.n_errors > 0) throw;


        if (options->preprocess_only)
        {
            const char* s2 = print_preprocessed_to_string2(ast.token_list.head);
            printf("%s", s2);
            free((void*)s2);
        }
        else
        {
            ast.declaration_list = parse(options, &ast.token_list, report);
            if (report->error_count > 0) throw;

            //ast_wasm_visit(&ast);

            if (!options->no_output)
            {
                if (options->format_input)
                {
                    struct format_visit_ctx f = { .ast = ast, .identation = 4 };
                    format_visit(&f);
                }
                struct visit_ctx visit_ctx = { 0 };
                visit_ctx.target = options->target;
                visit_ctx.ast = ast;
                visit(&visit_ctx);

                if (options->direct_compilation)
                    s = get_code_as_compiler_see(&visit_ctx.ast.token_list);
                else
                    s = get_code_as_we_see(&visit_ctx.ast.token_list, options->remove_comments);

                if (options->format_ouput)
                {
                    /*re-parser ouput and format*/
                    const char* s2 = format_code(options, s);
                    free((void*)s);
                    s = s2;
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
                    printf("cannot open output file '%s'", out_file_name);
                    throw;
                }
            }
        }
    }
    catch
    {
        //printf("Error %s\n", error->message);
    }


    free((void*)s);
    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);

    return report->error_count > 0;
}

int compile(int argc, const char** argv, struct report* report)
{
    struct options options = { 0 };
    if (fill_options(&options, argc, argv) != 0)
    {
        return 1;
    }

    clock_t begin_clock = clock();
    int no_files = 0;

    char root_dir[MAX_PATH] = { 0 };

    if (!options.no_output)
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
                continue;

            char fullpath[MAX_PATH] = { 0 };
            realpath(argv[i], fullpath);

            if (root_dir[0] == 0 ||
                (strlen(fullpath) < strlen(root_dir)))
            {
                strcpy(root_dir, fullpath);
            }
        }
        dirname(root_dir);
    }

    const int root_dir_len = strlen(root_dir);

    /*second loop to compile each file*/
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
            continue;
        no_files++;
        char output_file[MAX_PATH] = { 0 };

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
                char fullpath[MAX_PATH] = { 0 };
                realpath(argv[i], fullpath);

                strcpy(output_file, root_dir);
                strcat(output_file, "/out");

                strcat(output_file, fullpath + root_dir_len);

                char outdir[MAX_PATH];
                strcpy(outdir, output_file);
                dirname(outdir);

                /*let´s create output dir in case it does not exist*/
                mkdir(outdir, 0777);
            }
        }

        struct report local_report = { 0 };
        compile_one_file(argv[i], &options, output_file, argc, argv, &local_report);


        report->error_count += local_report.error_count;
        report->warnings_count += local_report.warnings_count;
        report->info_count += local_report.info_count;
    }

    /*tempo total da compilacao*/
    clock_t end_clock = clock();
    double cpu_time_used = ((double)(end_clock - begin_clock)) / CLOCKS_PER_SEC;

    printf("\n");
    printf("Total %d files %f seconds\n", no_files, cpu_time_used);
    printf("%d errors %d warnings %d notes\n", report->error_count, report->warnings_count, report->info_count);

    return 0;
}


struct ast get_ast(struct options* options,
    const char* filename,
    const char* source,
    struct report* report)
{
    struct ast ast = { 0 };
    struct tokenizer_ctx tctx = { 0 };
#ifdef TEST
    tctx.printf = printf_nothing;
#else
    tctx.printf = printf;
#endif

    struct token_list list = tokenizer(&tctx, source, filename, 0, TK_FLAG_NONE);

    struct preprocessor_ctx prectx = { 0 };
    prectx.options = *options;

#ifdef TEST
    prectx.printf = printf_nothing;
#else
    prectx.printf = printf;
#endif

    prectx.macros.capacity = 5000;

    add_standard_macros(&prectx);


    ast.token_list = preprocessor(&prectx, &list, 0);
    if (prectx.n_errors > 0)
        return ast;

    ast.declaration_list = parse(options, &ast.token_list, report);

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

const char* compile_source(const char* pszoptions, const char* content, struct report* report)
{
    const char* argv[100] = { 0 };
    char string[200] = { 0 };
    snprintf(string, sizeof string, "exepath %s", pszoptions);

    const int argc = strtoargv(string, 10, argv);

    const char* s = NULL;

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
        if (fill_options(&options, argc, argv) != 0)
        {
            throw;
        }

        prectx.options = options;
        add_standard_macros(&prectx);


        if (options.preprocess_only)
        {
            struct tokenizer_ctx tctx = { 0 };
#ifdef TEST
            tctx.printf = printf_nothing;
#else
            tctx.printf = printf;
#endif

            struct token_list tokens = tokenizer(&tctx, content, "source", 0, TK_FLAG_NONE);


            struct token_list token_list = preprocessor(&prectx, &tokens, 0);
            if (prectx.n_errors == 0)
            {
                s = print_preprocessed_to_string2(token_list.head);
            }

            preprocessor_ctx_destroy(&prectx);


        }
        else
        {
            struct visit_ctx visit_ctx = { 0 };
            visit_ctx.target = options.target;
            struct ast ast = get_ast(&options, "source", content, report);
            if (report->error_count > 0) throw;

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
                const char* s2 = format_code(&options, s);
                free((void*)s);
                s = s2;
            }
            ast_destroy(&ast);
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
    printf(WHITE "Cake " CAKE_VERSION RESET "\n");
    struct report report = { 0 };
    return  (char*)compile_source(pszoptions, content, &report);
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
    if (!ctx->options.check_naming_conventions || token->level != 0)
        return;

    if (!is_snake_case(token->lexeme)) {
        compiler_set_info_with_token(ctx, token, "use snake_case for struct/union tags");
    }
}

void naming_convention_enum_tag(struct parser_ctx* ctx, struct token* token)
{
    if (!ctx->options.check_naming_conventions || token->level != 0)
        return;

    if (!is_snake_case(token->lexeme)) {
        compiler_set_info_with_token(ctx, token, "use snake_case for enum tags");
    }
}

void naming_convention_function(struct parser_ctx* ctx, struct token* token)
{

    if (token == NULL || !ctx->options.check_naming_conventions || token->level != 0)
        return;


    if (!is_snake_case(token->lexeme)) {
        compiler_set_info_with_token(ctx, token, "use snake_case for functions");
    }
}
void naming_convention_global_var(struct parser_ctx* ctx, struct token* token, struct type* type, enum storage_class_specifier_flags storage)
{
    if (!ctx->options.check_naming_conventions || token->level != 0)
        return;

    if (!type_is_function_or_function_pointer(type))
    {
        if (storage & STORAGE_SPECIFIER_STATIC)
        {
            if (token->lexeme[0] != 's' || token->lexeme[1] != '_')
            {
                compiler_set_info_with_token(ctx, token, "use prefix s_ for static global variables");
            }
        }
        if (!is_snake_case(token->lexeme)) {
            compiler_set_info_with_token(ctx, token, "use snake_case global variables");
        }
    }
}

void naming_convention_local_var(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!ctx->options.check_naming_conventions || token->level != 0)
        return;

    if (!type_is_function_or_function_pointer(type))
    {
        if (!is_snake_case(token->lexeme)) {
            compiler_set_info_with_token(ctx, token, "use snake_case for local variables");
        }
    }
}

void naming_convention_enumerator(struct parser_ctx* ctx, struct token* token)
{
    if (!ctx->options.check_naming_conventions || token->level != 0)
        return;

    if (!is_all_upper(token->lexeme)) {
        compiler_set_info_with_token(ctx, token, "use UPPERCASE for enumerators");
    }
}

void naming_convention_struct_member(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!ctx->options.check_naming_conventions || token->level != 0)
        return;

    if (!is_snake_case(token->lexeme)) {
        compiler_set_info_with_token(ctx, token, "use snake_case for struct members");
    }
}

void naming_convention_parameter(struct parser_ctx* ctx, struct token* token, struct type* type)
{
    if (!ctx->options.check_naming_conventions || token->level != 0)
        return;

    if (!is_snake_case(token->lexeme)) {
        compiler_set_info_with_token(ctx, token, "use snake_case for arguments");
    }
}

#ifdef TEST

static bool compile_without_errors(const char* src)
{

    struct options options = { .input = LANGUAGE_C99 };
    struct report report = { 0 };
    get_ast(&options, "source", src, &report);
    return report.error_count == 0;
}

static bool compile_with_errors(const char* src)
{

    struct options options = { .input = LANGUAGE_C99 };
    struct report report = { 0 };
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
    struct report report = { 0 };
    char* result = compile_source("-std=C99", "int i = 1'000;", &report);
    assert(strcmp(result, "int i = 1000;") == 0);
    free(result);
}

void test_lit()
{
    struct report report = { 0 };
    char* result = compile_source("-std=C99", "char * s = u8\"maçã\";", &report);
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
        "    *z-- = '\0';\n"
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

    struct report report = { 0 };
    char* result = compile_source("-std=C99", source, &report);
    assert(strcmp(result, "int  (* p2)[2] = (int(*)[2]) 0;") == 0);
    free(result);
}

void enum_scope() {
    const char* source =
        "enum E { A = 1 };\n"
        "int main()\n"
        "{\n"
        "  enum E { B } e2; \n"
        "  static_assert( (typeof(e2)), (enum E) ); \n"
        "}\n";
    assert(compile_without_errors(source));
}
#endif





static void visit_attribute_specifier_sequence(struct visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence);

struct token* next_parser_token(struct token* token)
{
    struct token* r = token->next;
    while (!(r->flags & TK_FLAG_FINAL))
    {
        r = r->next;
    }
    return r;
}

static void visit_struct_or_union_specifier(struct visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void visit_expression(struct visit_ctx* ctx, struct expression* p_expression);
static void visit_statement(struct visit_ctx* ctx, struct statement* p_statement);
static void visit_enum_specifier(struct visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void visit_type_specifier(struct visit_ctx* ctx, struct type_specifier* p_type_specifier);

void convert_if_statement(struct visit_ctx* ctx, struct selection_statement* p_selection_statement)
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
    struct tokenizer_ctx tctx = { 0 };
    struct token_list list1 = tokenizer(&tctx, "{", "", 0, TK_FLAG_NONE);
    token_list_insert_after(&ctx->ast.token_list,
        before_first_token,
        &list1);
    struct token_list list2 = tokenizer(&tctx, "}", "", 0, TK_FLAG_NONE);
    token_list_insert_after(&ctx->ast.token_list,
        p_selection_statement->last_token,
        &list2);
}

void print_block_defer(struct defer_scope* deferblock, struct osstream* ss, bool hide_tokens)
{
    struct defer_scope* deferchild = deferblock->lastchild;
    while (deferchild != NULL)
    {
        struct token_list l = { 0 };
        l.head = deferchild->defer_statement->first_token;
        l.tail = deferchild->defer_statement->last_token;

        l.head->flags |= TK_FLAG_HIDE;
        const char* s = get_code_as_compiler_see(&l);
        assert(s != NULL);
        if (hide_tokens)
            token_range_add_flag(l.head, l.tail, TK_FLAG_HIDE);

        ss_fprintf(ss, "%s", s);
        free((void*)s);
        deferchild = deferchild->previous;
    }
}


void hide_block_defer(struct defer_scope* deferblock)
{
    struct defer_scope* deferchild = deferblock->lastchild;
    while (deferchild != NULL)
    {
        struct token_list l = { 0 };
        l.head = deferchild->defer_statement->first_token;
        l.tail = deferchild->defer_statement->last_token;
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

        if (p_defer->p_try_statement)
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
                if (block_item &&
                    block_item->label &&
                    block_item->label->name &&
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

static void visit_secondary_block(struct visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    visit_statement(ctx, p_secondary_block->statement);
}

static void visit_defer_statement(struct visit_ctx* ctx, struct defer_statement* p_defer_statement)
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
            visit_secondary_block(ctx, p_defer_statement->secondary_block);
        }
    }
    else //if (ctx->is_second_pass)
    {
        if (p_defer_statement->secondary_block)
            visit_secondary_block(ctx, p_defer_statement->secondary_block);
    }
}

static void visit_try_statement(struct visit_ctx* ctx, struct try_statement* p_try_statement)
{
    if (!ctx->is_second_pass)
    {
        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->previous = ctx->tail_block;
        ctx->tail_block = p_defer;
        p_defer->p_try_statement = p_try_statement;

        if (p_try_statement->secondary_block)
            visit_secondary_block(ctx, p_try_statement->secondary_block);


        struct osstream ss = { 0 };

        print_block_defer(p_defer, &ss, true);
        struct tokenizer_ctx tctx = { 0 };
        struct token_list l = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, p_try_statement->secondary_block->last_token->prev, &l);


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

            visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
        }
        else
        {
            snprintf(buffer, sizeof buffer, "} else {_catch_label_%d:;}", p_try_statement->try_catch_block_index);
            free(p_try_statement->last_token->lexeme);
            p_try_statement->last_token->lexeme = strdup(buffer);
        }

        ss_close(&ss);
    }
}

static void visit_selection_statement(struct visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    convert_if_statement(ctx, p_selection_statement);

    //PUSH
    struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
    p_defer->previous = ctx->tail_block;
    ctx->tail_block = p_defer;
    p_defer->p_selection_statement2 = p_selection_statement;

    if (p_selection_statement->secondary_block)
        visit_secondary_block(ctx, p_selection_statement->secondary_block);

    struct osstream ss = { 0 };
    print_block_defer(p_defer, &ss, true);

    if (ss.size > 0)
    {
        struct tokenizer_ctx tctx = { 0 };
        struct token_list l = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, p_selection_statement->secondary_block->last_token->prev, &l);
    }
    //POP
    ctx->tail_block = ctx->tail_block->previous;

    if (p_selection_statement->else_secondary_block_opt)
        visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);

    ss_close(&ss);
}

static void visit_compound_statement(struct visit_ctx* ctx, struct compound_statement* p_compound_statement);



char* remove_char(char* input, char ch)
{
    if (input == NULL)
        return NULL;
    char* p_write = input;
    const char* p = input;
    while (*p)
    {
        if (p[0] == ch)
        {
            p++;
        }
        else
        {
            *p_write = *p;
            p++;
            p_write++;
        }
    }
    *p_write = 0;
    return input;
}

static void visit_initializer_list(struct visit_ctx* ctx, struct initializer_list* p_initializer_list);

static void visit_bracket_initializer_list(struct visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {
        if (ctx->target < LANGUAGE_C2X)
        {
            assert(p_bracket_initializer_list->first_token->type == '{');

            //Criar token 0
            struct tokenizer_ctx tctx = { 0 };
            struct token_list list2 = tokenizer(&tctx, "0", NULL, 0, TK_FLAG_NONE);

            //inserir na frente
            token_list_insert_after(&ctx->ast.token_list, p_bracket_initializer_list->first_token, &list2);
        }
    }
    else
    {
        visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void visit_designation(struct visit_ctx* ctx, struct designation* p_designation)
{
}

static void visit_initializer(struct visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->designation)
    {
        visit_designation(ctx, p_initializer->designation);
    }

    if (p_initializer->assignment_expression)
    {
        visit_expression(ctx, p_initializer->assignment_expression);
    }
    else if (p_initializer->braced_initializer)
    {
        visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void visit_initializer_list(struct visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer* p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        visit_initializer(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void visit_type_qualifier(struct visit_ctx* ctx, struct type_qualifier* p_type_qualifier)
{
    if (p_type_qualifier->token &&
        p_type_qualifier->token->type == TK_KEYWORD_RESTRICT)
    {
        if (ctx->target < LANGUAGE_C99)
        {
            free(p_type_qualifier->token);
            p_type_qualifier->token->lexeme = strdup("/*restrict*/");
        }
    }
}

static void visit_specifier_qualifier(struct visit_ctx* ctx, struct type_specifier_qualifier* p_specifier_qualifier)
{
    if (p_specifier_qualifier->type_specifier)
        visit_type_specifier(ctx, p_specifier_qualifier->type_specifier);

    if (p_specifier_qualifier->type_qualifier)
        visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier);
}

static void visit_specifier_qualifier_list(struct visit_ctx* ctx, struct specifier_qualifier_list* p_specifier_qualifier_list_opt,
    struct type* p_type)
{

    //(typeof(int[2])*)
    // 
    //TODO se tiver typeof em qualquer parte tem que imprimir todo  tipo
    // tem que refazer
    if (p_specifier_qualifier_list_opt->type_specifier_flags & TYPE_SPECIFIER_TYPEOF)
    {
        token_range_add_flag(p_specifier_qualifier_list_opt->first_token,
            p_specifier_qualifier_list_opt->last_token, TK_FLAG_HIDE);

        struct osstream ss = { 0 };
        print_type(&ss, type_get_specifer_part(p_type));

        struct tokenizer_ctx tctx = { 0 };
        struct token_list l2 = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, p_specifier_qualifier_list_opt->last_token, &l2);

        ss_close(&ss);
    }

    if (p_specifier_qualifier_list_opt == NULL)
        return;

    if (p_specifier_qualifier_list_opt->struct_or_union_specifier)
    {
        visit_struct_or_union_specifier(ctx, p_specifier_qualifier_list_opt->struct_or_union_specifier);
    }
    else if (p_specifier_qualifier_list_opt->enum_specifier)
    {
        visit_enum_specifier(ctx, p_specifier_qualifier_list_opt->enum_specifier);
    }
    else if (p_specifier_qualifier_list_opt->typedef_declarator)
    {
        //typedef name
    }
    //else if (p_specifier_qualifier_list->p_typeof_expression_opt)
    //{
      //  visit_expression(ctx, p_specifier_qualifier_list->p_typeof_expression_opt);
    //}
    else
    {
        struct type_specifier_qualifier* p_specifier_qualifier = p_specifier_qualifier_list_opt->head;
        while (p_specifier_qualifier)
        {
            visit_specifier_qualifier(ctx, p_specifier_qualifier);
            p_specifier_qualifier = p_specifier_qualifier->next;
        }
    }
}
static void visit_declarator(struct visit_ctx* ctx, struct declarator* p_declarator);
static void visit_type_name(struct visit_ctx* ctx, struct type_name* p_type_name)
{

    visit_specifier_qualifier_list(ctx, p_type_name->specifier_qualifier_list, &p_type_name->type);
    visit_declarator(ctx, p_type_name->declarator);


    /*
    * Vamos esconder tudo e gerar um novo
    *  Exemplo
    *  (const typeof(int (*)())) -> *  ( int (*const )() )
    */
}



static void visit_argument_expression_list(struct visit_ctx* ctx, struct argument_expression_list* p_argument_expression_list)
{
    struct argument_expression* p_argument_expression =
        p_argument_expression_list->head;
    while (p_argument_expression)
    {
        visit_expression(ctx, p_argument_expression->expression);
        p_argument_expression = p_argument_expression->next;
    }
}

static void visit_generic_selection(struct visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        visit_expression(ctx, p_generic_selection->expression);
    }
    else if (p_generic_selection->type_name)
    {
        visit_type_name(ctx, p_generic_selection->type_name);
    }

    if (ctx->target < LANGUAGE_C11)
    {
        token_range_add_flag(p_generic_selection->first_token, p_generic_selection->last_token, TK_FLAG_HIDE);
        if (p_generic_selection->p_view_selected_expression)
        {
            token_range_remove_flag(p_generic_selection->p_view_selected_expression->first_token,
                p_generic_selection->p_view_selected_expression->last_token,
                TK_FLAG_HIDE);
        }
    }

}


static void visit_expression(struct visit_ctx* ctx, struct expression* p_expression)
{
    switch (p_expression->expression_type)
    {
    case PRIMARY_EXPRESSION__FUNC__:
        break;
    case PRIMARY_IDENTIFIER:
        break;
    case PRIMARY_EXPRESSION_ENUMERATOR:
        if (ctx->target < LANGUAGE_C2X)
        {
            struct type t = type_get_enum_type(&p_expression->type);
            if (t.type_specifier_flags != TYPE_SPECIFIER_INT)
            {
                struct osstream ss0 = { 0 };
                print_type(&ss0, &t);
                struct osstream ss = { 0 };
                ss_fprintf(&ss, "((%s)%s)", ss0.c_str, p_expression->first_token->lexeme);
                
                free(p_expression->first_token->lexeme);
                p_expression->first_token->lexeme = ss.c_str;
                ss.c_str = NULL; /*MOVED*/
                ss_close(&ss);
                ss_close(&ss0);
            }
            
        }        
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
        if (p_expression->first_token &&
            p_expression->first_token->type == TK_KEYWORD_NULLPTR)
        {
            if (ctx->target < LANGUAGE_C2X)
            {
                free(p_expression->first_token->lexeme);
                p_expression->first_token->lexeme = strdup("((void*)0)");
            }
        }
        else if (p_expression->first_token &&
            p_expression->first_token->type == TK_KEYWORD_TRUE)
        {
            if (ctx->target < LANGUAGE_C99)
            {
                free(p_expression->first_token->lexeme);
                p_expression->first_token->lexeme = strdup("1");
            }
            else if (ctx->target < LANGUAGE_C2X)
            {
                free(p_expression->first_token->lexeme);
                p_expression->first_token->lexeme = strdup("((_Bool)1)");
            }
        }
        else if (p_expression->first_token &&
            p_expression->first_token->type == TK_KEYWORD_FALSE)
        {
            if (ctx->target < LANGUAGE_C99)
            {
                free(p_expression->first_token->lexeme);
                p_expression->first_token->lexeme = strdup("0");
            }
            else if (ctx->target < LANGUAGE_C2X)
            {
                free(p_expression->first_token->lexeme);
                p_expression->first_token->lexeme = strdup("((_Bool)0)");
            }
        }
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        visit_generic_selection(ctx, p_expression->generic_selection);
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
        //visit_expression(ctx, p_expression->left);
        break;
    case POSTFIX_FUNCTION_CALL:
        visit_expression(ctx, p_expression->left);
        visit_argument_expression_list(ctx, &p_expression->argument_expression_list);
        break;
    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
    {
        ctx->has_lambda = true;
        ctx->is_inside_lambda = true;
        visit_type_name(ctx, p_expression->type_name);
        visit_compound_statement(ctx, p_expression->compound_statement);
        ctx->is_inside_lambda = false;

        if (ctx->is_second_pass)
        {
            /*no segundo passo nós copiamos a funcao*/
            char name[100] = { 0 };
            snprintf(name, sizeof name, " _lit_func_%d", ctx->lambdas_index);
            ctx->lambdas_index++;

            struct osstream ss = { 0 };

            

            bool is_first = true;
            print_type_qualifier_flags(&ss, &is_first, p_expression->type_name->declarator->type.type_qualifier_flags);
            print_type_specifier_flags(&ss, &is_first, p_expression->type_name->declarator->type.type_specifier_flags);


            free((void*)p_expression->type_name->declarator->type.name_opt);
            p_expression->type_name->declarator->type.name_opt = strdup(name);
            
            struct osstream ss0 = { 0 };
            print_type(&ss0, &p_expression->type_name->declarator->type);
            ss_fprintf(&ss, "static %s", ss0.c_str);

            ss_close(&ss0);

            struct tokenizer_ctx tctx = { 0 };
            struct token_list l1 = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);

            token_list_append_list(&ctx->insert_before_declaration, &l1);
            ss_close(&ss);

            for (struct token* current = p_expression->compound_statement->first_token;
                current != p_expression->compound_statement->last_token->next;
                current = current->next)
            {
                token_list_clone_and_add(&ctx->insert_before_declaration, current);
            }

            token_range_add_flag(p_expression->first_token, p_expression->last_token, TK_FLAG_HIDE);


            struct token_list l3 = tokenizer(&tctx, "\n\n", NULL, 0, TK_FLAG_NONE);
            token_list_append_list(&ctx->insert_before_declaration, &l3);


            struct token_list l2 = tokenizer(&tctx, name, NULL, 0, TK_FLAG_FINAL);
            token_list_insert_after(&ctx->ast.token_list, p_expression->last_token, &l2);
        }
    }
    break;

    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

        if (p_expression->type_name)
        {
            visit_type_name(ctx, p_expression->type_name);
        }

        visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);

        break;

    case UNARY_EXPRESSION_ALIGNOF:

        if (ctx->target < LANGUAGE_C11)
        {
            if (p_expression->first_token->level == 0)
            {
                token_range_add_flag(p_expression->first_token, p_expression->last_token, TK_FLAG_HIDE);
                char buffer[30] = { 0 };
                snprintf(buffer, sizeof buffer, "%lld", constant_value_to_ll(&p_expression->constant_value));
                struct tokenizer_ctx tctx = { 0 };
                struct token_list l3 = tokenizer(&tctx, buffer, NULL, 0, TK_FLAG_NONE);
                l3.head->flags = p_expression->last_token->flags;                
                token_list_insert_after(&ctx->ast.token_list, p_expression->last_token, &l3);
            }
        }

        if (p_expression->right)
        {
            visit_expression(ctx, p_expression->right);
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            visit_type_name(ctx, p_expression->type_name);
        }
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
            visit_expression(ctx, p_expression->right);
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            visit_type_name(ctx, p_expression->type_name);
        }

        break;

    case UNARY_EXPRESSION_HASHOF_TYPE:

        if (!ctx->is_second_pass)
        {
            token_range_add_flag(p_expression->first_token, p_expression->last_token, TK_FLAG_HIDE);

            char buffer[30] = { 0 };
            snprintf(buffer, sizeof buffer, "%llu", constant_value_to_ull(&p_expression->constant_value));

            struct tokenizer_ctx tctx = { 0 };
            struct token_list l3 = tokenizer(&tctx, buffer, NULL, 0, TK_FLAG_NONE);
            token_list_insert_after(&ctx->ast.token_list, p_expression->last_token, &l3);
        }
        break;


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
            visit_expression(ctx, p_expression->left);
        }
        if (p_expression->right)
        {
            visit_expression(ctx, p_expression->right);
        }
        if (p_expression->type_name)
        {
            visit_type_name(ctx, p_expression->type_name);
        }
        break;

    case UNARY_EXPRESSION_TRAITS:
    {
        if (ctx->target < LANGUAGE_CXX)
        {
            struct tokenizer_ctx tctx = { 0 };
            struct token_list l2 = { 0 };

            if (constant_value_to_bool(&p_expression->constant_value))
                l2 = tokenizer(&tctx, "1", NULL, 0, TK_FLAG_NONE);
            else
                l2 = tokenizer(&tctx, "0", NULL, 0, TK_FLAG_NONE);


            token_list_insert_after(&ctx->ast.token_list,
                p_expression->last_token,
                &l2);

            token_range_add_flag(p_expression->first_token,
                p_expression->last_token,
                TK_FLAG_HIDE);
        }
    }
    break;

    case UNARY_EXPRESSION_IS_SAME:
        break;

    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
        break;

    case CONDITIONAL_EXPRESSION:
        if (p_expression->condition_expr)
        {
            visit_expression(ctx, p_expression->condition_expr);
        }

        if (p_expression->left)
        {
            visit_expression(ctx, p_expression->left);
        }
        if (p_expression->right)
        {
            visit_expression(ctx, p_expression->right);
        }

        break;

    default:
        break;
    }
}

static void visit_expression_statement(struct visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
        visit_expression(ctx, p_expression_statement->expression_opt);
}

static void visit_block_item_list(struct visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void visit_compound_statement(struct visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    visit_block_item_list(ctx, &p_compound_statement->block_item_list);
}

//
static void visit_iteration_statement(struct visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{

    if (p_iteration_statement->expression1)
    {
        visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->expression2)
    {
        visit_expression(ctx, p_iteration_statement->expression2);
    }

    if (p_iteration_statement->first_token->type == TK_KEYWORD_REPEAT)
    {
        free(p_iteration_statement->first_token->lexeme);
        p_iteration_statement->first_token->lexeme = strdup("for(;;)/*repeat*/");
    }

    if (p_iteration_statement->secondary_block)
    {
        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        p_defer->previous = ctx->tail_block;
        ctx->tail_block = p_defer;
        p_defer->p_iteration_statement = p_iteration_statement;

        visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        struct osstream ss = { 0 };
        //defer_print(defer, &ss, ctx->bHasThrowWithVariable, ctx->bHasBreakWithVariable, ctx->bHasReturnWithVariable);
        print_block_defer(p_defer, &ss, true);

        struct tokenizer_ctx tctx = { 0 };
        struct token_list l = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
        token_list_insert_after(&ctx->ast.token_list, p_iteration_statement->secondary_block->last_token->prev, &l);


        if (ctx->tail_block)
        {
            //POP
            ctx->tail_block = ctx->tail_block->previous;
        }

        ss_close(&ss);
    }
}



static void visit_jump_statement(struct visit_ctx* ctx, struct jump_statement* p_jump_statement)
{

    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
    {
        struct osstream ss0 = { 0 };
        print_all_defer_until_try(ctx->tail_block, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "goto _catch_label_%d;", p_jump_statement->try_catch_block_index);
            ss_fprintf(&ss, "}");
            free(p_jump_statement->first_token->lexeme);
            p_jump_statement->first_token->lexeme = ss.c_str;
            _del_attr(ss, MUST_DESTROY); /*MOVED*/

            p_jump_statement->last_token->flags |= TK_FLAG_HIDE;

        }
        else
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "goto _catch_label_%d", p_jump_statement->try_catch_block_index);
            free(p_jump_statement->first_token->lexeme);
            p_jump_statement->first_token->lexeme = ss.c_str; /*MOVED*/
            _del_attr(ss, MUST_DESTROY); /*MOVED*/
        }

        ss_close(&ss0);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        struct osstream ss0 = { 0 };
        print_all_defer_until_end(ctx->tail_block, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "return");
            free(p_jump_statement->first_token->lexeme);

            p_jump_statement->first_token->lexeme = ss.c_str; /*MOVED*/
            ss.c_str = NULL; /*MOVED*/

            free(p_jump_statement->last_token->lexeme);
            p_jump_statement->last_token->lexeme = strdup(";}");
            ss_close(&ss);
        }
        ss_close(&ss0);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {
        struct osstream ss0 = { 0 };

        print_all_defer_until_iter(ctx->tail_block, &ss0);
        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "break;");
            ss_fprintf(&ss, "}");
            free(p_jump_statement->first_token->lexeme);
            p_jump_statement->first_token->lexeme = ss.c_str;  /*MOVED*/
            ss.c_str = NULL;

            p_jump_statement->last_token->flags |= TK_FLAG_HIDE;
            ss_close(&ss);
        }

        ss_close(&ss0);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {
        struct osstream ss0 = { 0 };
        print_all_defer_until_label(ctx->tail_block, p_jump_statement->label->lexeme, &ss0);

        if (ss0.size > 0)
        {
            struct osstream ss = { 0 };
            ss_fprintf(&ss, "{ %s ", ss0.c_str);
            ss_fprintf(&ss, "goto");
            free(p_jump_statement->first_token->lexeme);
            p_jump_statement->first_token->lexeme = ss.c_str; /*MOVED*/
            ss.c_str = NULL; /*MOVED*/
            free(p_jump_statement->last_token->lexeme);
            p_jump_statement->last_token->lexeme = strdup(";}");
            ss_close(&ss);
        }

        ss_close(&ss0);
    }
    else
    {
        assert(false);
    }
}


static void visit_labeled_statement(struct visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    //p_labeled_statement->label

    if (p_labeled_statement->statement)
    {
        visit_statement(ctx, p_labeled_statement->statement);
    }
}

static void visit_primary_block(struct visit_ctx* ctx, struct primary_block* p_primary_block)
{



    if (p_primary_block->defer_statement)
    {
        visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            visit_compound_statement(ctx, p_primary_block->compound_statement);
        }
        else if (p_primary_block->iteration_statement)
        {
            visit_iteration_statement(ctx, p_primary_block->iteration_statement);
        }
        else if (p_primary_block->selection_statement)
        {
            visit_selection_statement(ctx, p_primary_block->selection_statement);
        }
        else if (p_primary_block->try_statement)
        {
            visit_try_statement(ctx, p_primary_block->try_statement);
        }
    }

}

static void visit_unlabeled_statement(struct visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}

static void visit_declaration(struct visit_ctx* ctx, struct declaration* p_declaration);

static void visit_statement(struct visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void visit_label(struct visit_ctx* ctx, struct label* p_label)
{
    //p_label->name
}
static void visit_block_item(struct visit_ctx* ctx, struct block_item* p_block_item)
{
    if (p_block_item->declaration)
    {
        visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        visit_label(ctx, p_block_item->label);
    }
    if (ctx->insert_before_block_item.head != NULL)
    {
        //token_list_insert_after(&ctx->ast.token_list, p_statement->first_token->prev, &ctx->insert_before_statement);
        token_list_insert_after(&ctx->ast.token_list, p_block_item->first_token->prev, &ctx->insert_before_block_item);
        ctx->insert_before_block_item.head = NULL;
        ctx->insert_before_block_item.tail = NULL;
    }
}

static void visit_block_item_list(struct visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void visit_static_assert_declaration(struct visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration)
{
    visit_expression(ctx, p_static_assert_declaration->constant_expression);

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

            struct tokenizer_ctx tctx = { 0 };
            struct token_list list1 = tokenizer(&tctx, ", \"error\"", "", 0, TK_FLAG_NONE);
            token_list_insert_after(&ctx->ast.token_list, rp, &list1);
        }
        if (strcmp(p_static_assert_declaration->first_token->lexeme, "static_assert") == 0)
        {
            /*C23 has static_assert but C11 _Static_assert*/
            free(p_static_assert_declaration->first_token->lexeme);
            p_static_assert_declaration->first_token->lexeme = strdup("_Static_assert");
        }
    }
    else
    {
        free(p_static_assert_declaration->first_token->lexeme);
        p_static_assert_declaration->first_token->lexeme = strdup("static_assert");
    }
}

static void visit_declaration_specifiers(struct visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* p_type);


static void visit_direct_declarator(struct visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration* parameter = NULL;

        if (p_direct_declarator->function_declarator->parameter_type_list_opt)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }

        while (parameter)
        {
            if (parameter->attribute_specifier_sequence_opt)
            {
                visit_attribute_specifier_sequence(ctx, parameter->attribute_specifier_sequence_opt);
            }

            visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);
            visit_declarator(ctx, parameter->declarator);
            parameter = parameter->next;
        }

    }
    else if (p_direct_declarator->array_declarator)
    {
        if (ctx->target < LANGUAGE_C99)
        {
            /*static and type qualifiers in parameter array declarators where added in C99*/
            if (p_direct_declarator->array_declarator->static_token_opt)
            {
                p_direct_declarator->array_declarator->static_token_opt->flags |= TK_FLAG_HIDE;

                if (p_direct_declarator->array_declarator->type_qualifier_list_opt)
                {
                    struct type_qualifier* p_type_qualifier =
                        p_direct_declarator->array_declarator->type_qualifier_list_opt->head;

                    while (p_type_qualifier)
                    {
                        p_type_qualifier->token->flags |= TK_FLAG_HIDE;
                        p_type_qualifier = p_type_qualifier->next;
                    }
                }
            }
        }
    }
}

static void visit_declarator(struct visit_ctx* ctx, struct declarator* p_declarator)
{
    bool need_transformation = false;

    if (p_declarator->pointer)
    {
        struct pointer* p = p_declarator->pointer;
        while (p)
        {
            p = p->pointer;
        }
    }

    if (ctx->target < LANGUAGE_C2X)
    {
        if (p_declarator->declaration_specifiers)
        {
            if (p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO)
            {
                need_transformation = true;
            }
            if (p_declarator->declaration_specifiers->type_specifier_flags & TYPE_SPECIFIER_TYPEOF)
            {
                need_transformation = true;
            }
        }

        if (p_declarator->specifier_qualifier_list &&
            p_declarator->specifier_qualifier_list->type_specifier_flags & TYPE_SPECIFIER_TYPEOF)
        {
            need_transformation = true;
        }
    }


    //we may have a diference type from the current syntax 
    if (need_transformation)
    {

        struct osstream ss = { 0 };

        /*types like nullptr are converted to other types like void* */
        struct type new_type = type_convert_to(&p_declarator->type, ctx->target);

        type_remove_names(&new_type);
        if (p_declarator->name)
            new_type.name_opt = strdup(p_declarator->name->lexeme);
        print_type_declarator(&ss, &new_type);

        if (ss.c_str != NULL)
        {
            struct tokenizer_ctx tctx = { 0 };
            struct token_list l2 = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_NONE);


            /*let's hide the old declarator*/
            if (p_declarator->first_token != NULL &&
                p_declarator->first_token != p_declarator->last_token)
            {
                l2.head->flags = p_declarator->first_token->flags;
                token_list_insert_after(&ctx->ast.token_list, p_declarator->last_token, &l2);
                token_range_add_flag(p_declarator->first_token, p_declarator->last_token, TK_FLAG_HIDE);
            }
            else
            {

                if (p_declarator->first_token == NULL) {
                    l2.head->flags = p_declarator->last_token->flags;
                    /*it is a empty declarator, so first_token is not part of declarator it only marks de position*/
                    token_list_insert_after(&ctx->ast.token_list, p_declarator->last_token->prev, &l2);
                }
                else
                {
                    l2.head->flags = p_declarator->first_token->flags;
                    /*it is a empty declarator, so first_token is not part of declarator it only marks de position*/
                    token_list_insert_after(&ctx->ast.token_list, p_declarator->last_token, &l2);
                    token_range_add_flag(p_declarator->first_token, p_declarator->last_token, TK_FLAG_HIDE);
                }

            }
        }

        type_destroy(&new_type);
        ss_close(&ss);
    }


    if (p_declarator->direct_declarator)
    {
        visit_direct_declarator(ctx, p_declarator->direct_declarator);
    }
}

static void visit_init_declarator_list(struct visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* p_init_declarator = p_init_declarator_list->head;



    while (p_init_declarator)
    {
        if (p_init_declarator->declarator)
        {
            visit_declarator(ctx, p_init_declarator->declarator);
        }

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                visit_expression(ctx, p_init_declarator->initializer->assignment_expression);
            }
            else
            {
                assert(p_init_declarator->initializer->braced_initializer != NULL);
                if (p_init_declarator->initializer->braced_initializer)
                {
                    visit_bracket_initializer_list(ctx,
                        p_init_declarator->initializer->braced_initializer);
                }

            }
        }

        p_init_declarator = p_init_declarator->next;
    }
}



static void visit_member_declarator(struct visit_ctx* ctx, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        visit_declarator(ctx, p_member_declarator->declarator);
    }
}

static void visit_member_declarator_list(struct visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}
static void visit_member_declaration(struct visit_ctx* ctx, struct member_declaration* p_member_declaration)
{
    if (p_member_declaration->member_declarator_list_opt)
    {
        visit_specifier_qualifier_list(ctx,
            p_member_declaration->specifier_qualifier_list,
            &p_member_declaration->member_declarator_list_opt->head->declarator->type); /*se nao tem?*/
    }

    if (p_member_declaration->member_declarator_list_opt)
    {
        visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void visit_member_declaration_list(struct visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* p_member_declaration =
        p_member_declaration_list->head;
    while (p_member_declaration)
    {
        visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void visit_attribute_specifier(struct visit_ctx* ctx, struct attribute_specifier* p_attribute_specifier)
{
    if (ctx->target < LANGUAGE_C2X)
    {
        token_range_add_flag(p_attribute_specifier->first_token, p_attribute_specifier->last_token, TK_FLAG_HIDE);
    }
}

static void visit_attribute_specifier_sequence(struct visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence)
{
    struct attribute_specifier* current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        visit_attribute_specifier(ctx, current);
        current = current->next;
    }
}

static void visit_struct_or_union_specifier(struct visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{

    if (p_struct_or_union_specifier->attribute_specifier_sequence_opt)
        visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence_opt);

    struct struct_or_union_specifier* p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);

    if (p_struct_or_union_specifier->show_anonymous_tag && !ctx->is_second_pass)
    {
        struct token* first = p_struct_or_union_specifier->first_token;

        const char* tag = p_struct_or_union_specifier->tag_name;
        char buffer[200] = { 0 };
        snprintf(buffer, sizeof buffer, " %s", tag);
        struct tokenizer_ctx tctx = { 0 };
        struct token_list l2 = tokenizer(&tctx, buffer, NULL, 0, TK_FLAG_NONE);
        token_list_insert_after(&ctx->ast.token_list, first, &l2);
    }

    if (p_complete)
    {
        if (ctx->is_inside_lambda && !ctx->is_second_pass)
        {
            /*
              Na primeira passada marcamos os tipos que são renomeados
            */
            if (p_complete->scope_level >
                p_struct_or_union_specifier->scope_level &&
                p_complete->visit_moved == 0)
            {
                char newtag[212];
                snprintf(newtag, sizeof newtag, "_%s%d", p_struct_or_union_specifier->tag_name, ctx->capture_index);
                ctx->capture_index++;

                free(p_complete->tagtoken->lexeme);
                p_complete->tagtoken->lexeme = strdup(newtag);
                p_complete->visit_moved = 1;
            }
        }
        else if (ctx->is_second_pass)
        {
            /*
             Na segunda passada vou renomear quem usa este tag exporado
            */
            if (p_complete->visit_moved == 1)
            {
                if (p_struct_or_union_specifier != p_complete)
                {
                    free(p_struct_or_union_specifier->tagtoken->lexeme);
                    p_struct_or_union_specifier->tagtoken->lexeme = strdup(p_complete->tagtoken->lexeme);
                }
            }
        }
    }



    visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);

}

static void visit_enumerator(struct visit_ctx* ctx, struct enumerator* p_enumerator)
{
    if (p_enumerator->constant_expression_opt)
        visit_expression(ctx, p_enumerator->constant_expression_opt);

}

//struct enumerator_list* enumerator_list;
static void visit_enumerator_list(struct visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* current = p_enumerator_list->head;
    while (current)
    {
        visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void visit_enum_specifier(struct visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    if (ctx->target < LANGUAGE_C2X)
    {
        if (p_enum_specifier->specifier_qualifier_list) 
        {
            struct token* tk = p_enum_specifier->specifier_qualifier_list->first_token;
            while (tk)
            {
                if (tk->type == ':')
                    break;
                tk = tk->prev;
            }
            token_range_add_flag(tk,
                p_enum_specifier->specifier_qualifier_list->last_token,
                TK_FLAG_HIDE);
        }


        if (p_enum_specifier->complete_enum_specifier != NULL &&
            p_enum_specifier != p_enum_specifier->complete_enum_specifier &&
            p_enum_specifier->complete_enum_specifier->specifier_qualifier_list)
        {
            p_enum_specifier->first_token->flags |= TK_FLAG_HIDE;

            if (p_enum_specifier->tag_token)
                p_enum_specifier->tag_token->flags |= TK_FLAG_HIDE;

            struct osstream ss = { 0 };
            bool b_first = true;

            print_type_qualifier_flags(&ss, &b_first, p_enum_specifier->complete_enum_specifier->specifier_qualifier_list->type_qualifier_flags);
            print_type_specifier_flags(&ss, &b_first, p_enum_specifier->complete_enum_specifier->specifier_qualifier_list->type_specifier_flags);

            struct tokenizer_ctx tctx = { 0 };
            struct token_list l2 = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_NONE);

            token_list_insert_after(&ctx->ast.token_list,
                p_enum_specifier->tag_token,
                &l2);

            ss_close(&ss);
        }

    }

    if (p_enum_specifier->attribute_specifier_sequence_opt)
    {
        visit_attribute_specifier_sequence(ctx, p_enum_specifier->attribute_specifier_sequence_opt);
    }

    visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void visit_typeof_specifier(struct visit_ctx* ctx, struct typeof_specifier* p_typeof_specifier)
{
}

static void visit_type_specifier(struct visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->typeof_specifier)
    {
        visit_typeof_specifier(ctx, p_type_specifier->typeof_specifier);
    }

    if (p_type_specifier->struct_or_union_specifier)
    {
        visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }


    if (p_type_specifier->atomic_type_specifier)
    {
        //visit_deped(ctx, p_type_specifier->enum_specifier);
    }

    if (p_type_specifier->flags & TYPE_SPECIFIER_BOOL)
    {
        if (ctx->target < LANGUAGE_C99)
        {
            free(p_type_specifier->token->lexeme);
            p_type_specifier->token->lexeme = strdup("unsigned char");
        }
        else
        {
            if (ctx->target < LANGUAGE_C2X)
            {
                if (strcmp(p_type_specifier->token->lexeme, "bool") == 0)
                {
                    free(p_type_specifier->token->lexeme);
                    p_type_specifier->token->lexeme = strdup("_Bool");
                }
            }
            else
            {
                free(p_type_specifier->token->lexeme);
                p_type_specifier->token->lexeme = strdup("bool");
            }
        }
    }
}

static void visit_type_specifier_qualifier(struct visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_qualifier)
    {
    }
    else if (p_type_specifier_qualifier->type_specifier)
    {
        visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
    else if (p_type_specifier_qualifier->alignment_specifier)
    {
    }
}

static void visit_storage_class_specifier(struct visit_ctx* ctx, struct storage_class_specifier* p_storage_class_specifier)
{
    if (p_storage_class_specifier->flags & STORAGE_SPECIFIER_CONSTEXPR)
    {
        if (ctx->target < LANGUAGE_C2X)
        {
            p_storage_class_specifier->token->flags |= TK_FLAG_HIDE;
        }
    }
    if (p_storage_class_specifier->flags & STORAGE_SPECIFIER_AUTO)
    {
        if (ctx->target < LANGUAGE_C2X)
        {
            p_storage_class_specifier->token->flags |= TK_FLAG_HIDE;
        }
    }
}

static void visit_declaration_specifier(struct visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{

    if (p_declaration_specifier->function_specifier)
    {
        if (p_declaration_specifier->function_specifier->token->type == TK_KEYWORD__NORETURN)
        {
            if (ctx->target < LANGUAGE_C11)
            {
                p_declaration_specifier->function_specifier->token->lexeme = strdup("/*[[noreturn]]*/");
            }
            else if (ctx->target == LANGUAGE_C11)
            {
                /*nothing*/
            }
            else if (ctx->target > LANGUAGE_C11)
            {
                /*use attributes*/
                p_declaration_specifier->function_specifier->token->lexeme = strdup("[[noreturn]]");
            }

        }
    }


    if (p_declaration_specifier->storage_class_specifier)
    {
        visit_storage_class_specifier(ctx, p_declaration_specifier->storage_class_specifier);

    }

    if (p_declaration_specifier->type_specifier_qualifier)
    {
        visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);

    }

}

static void visit_declaration_specifiers(struct visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* p_type_opt)
{
    /*
        * Se tiver typeof ou auto vamos apagar todos type specifiers.
        * e trocar por um novo
        * const typeof(int (*)()) a;
           //a = 1;
          auto p = (const typeof(int (*)())) 0;

          TODO esconder os type spefiver e qualifider , esconder auto.
          o resto tipo static deixar.

        */
        //
    if (!ctx->is_second_pass &&
        ctx->target < LANGUAGE_C2X &&
        (p_declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTO ||
        p_declaration_specifiers->type_specifier_flags & TYPE_SPECIFIER_TYPEOF))
    {

        struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;
        while (p_declaration_specifier)
        {
            if (p_declaration_specifier->function_specifier)
            {
            }
            if (p_declaration_specifier->storage_class_specifier)
            {
            }
            if (p_declaration_specifier->type_specifier_qualifier)
            {
                if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                {
                    p_declaration_specifier->type_specifier_qualifier->type_qualifier->token->flags |= TK_FLAG_HIDE;
                }
                if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                {
                    if (p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier)
                    {
                        token_range_add_flag(p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier->first_token,
                            p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier->last_token,
                            TK_FLAG_HIDE);
                    }
                    p_declaration_specifier->type_specifier_qualifier->type_specifier->token->flags |= TK_FLAG_HIDE;
                }
            }
            p_declaration_specifier = p_declaration_specifier->next;
        }


        /*now we print new especifiers then convert to tokens*/
        struct osstream ss0 = { 0 };
        struct type new_type = { 0 };

        if (p_type_opt)
            new_type = type_convert_to(p_type_opt, ctx->target);

        const struct type* p = type_get_specifer_part(&new_type);
        print_type_qualifier_specifiers(&ss0, p);

        struct tokenizer_ctx tctx = { 0 };
        struct token_list l2 = tokenizer(&tctx, ss0.c_str, NULL, 0, TK_FLAG_NONE);

        token_list_insert_after(&ctx->ast.token_list,
            p_declaration_specifiers->last_token,
            &l2);

        type_destroy(&new_type);
        ss_close(&ss0);
    }

    struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;
    while (p_declaration_specifier)
    {
        visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
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
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        return true;
    }
    return false;
}

static void visit_declaration(struct visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
        visit_attribute_specifier_sequence(ctx, p_declaration->p_attribute_specifier_sequence_opt);
    }


    if (p_declaration->declaration_specifiers)
    {

        if (p_declaration->init_declarator_list.head)
        {
            visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers,
                &p_declaration->init_declarator_list.head->declarator->type);
        }
        else
        {
            visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, NULL);
        }

    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
        if (!ctx->is_second_pass)
        {
            token_range_add_flag(p_declaration->p_attribute_specifier_sequence_opt->first_token,
                p_declaration->p_attribute_specifier_sequence_opt->last_token,
                TK_FLAG_HIDE);

        }
    }
    if (ctx->is_second_pass)
    {

        if (p_declaration->declaration_specifiers &&
            p_declaration->declaration_specifiers->type_specifier_flags == TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            if (p_declaration->declaration_specifiers->struct_or_union_specifier->visit_moved == 1)
            {
                struct tokenizer_ctx tctx = { 0 };
                struct token_list list0 = tokenizer(&tctx, "struct ", NULL, 0, TK_FLAG_FINAL);
                token_list_append_list(&ctx->insert_before_declaration, &list0);


                struct token_list list = tokenizer(&tctx, p_declaration->declaration_specifiers->struct_or_union_specifier->tagtoken->lexeme, NULL, 0, TK_FLAG_FINAL);
                token_list_append_list(&ctx->insert_before_declaration, &list);


                //struct token_list list3 = tokenizer("{", NULL, 0, TK_FLAG_FINAL);
                //token_list_append_list(&ctx->insert_before_declaration, &list3);



                struct token* first = p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.first_token;
                struct token* last = p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.last_token;
                for (struct token* current = first;
                    current != last->next;
                    current = current->next)
                {
                    token_list_clone_and_add(&ctx->insert_before_declaration, current);
                    //current->flags |= TK_FLAG_FINAL;
                }

                struct token_list list3 = tokenizer(&tctx, ";\n", NULL, 0, TK_FLAG_FINAL);
                token_list_append_list(&ctx->insert_before_declaration, &list3);


                if (p_declaration->init_declarator_list.head == NULL)
                {
                    token_range_add_flag(p_declaration->declaration_specifiers->struct_or_union_specifier->first_token,
                        p_declaration->declaration_specifiers->struct_or_union_specifier->last_token,
                        TK_FLAG_HIDE);
                }
                else
                {
                    token_range_add_flag(p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.first_token,
                        p_declaration->declaration_specifiers->struct_or_union_specifier->member_declaration_list.last_token,
                        TK_FLAG_HIDE);
                }
            }
        }
    }


    if (p_declaration->init_declarator_list.head)
    {
        visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        ctx->has_lambda = false;
        ctx->is_second_pass = false;


        struct defer_scope* p_defer = calloc(1, sizeof * p_defer);
        ctx->tail_block = p_defer;
        p_defer->p_function_body = p_declaration->function_body;

        visit_compound_statement(ctx, p_declaration->function_body);

        if (!is_last_item_return(p_declaration->function_body))
        {
            struct osstream ss = { 0 };
            print_block_defer(p_defer, &ss, true);
            if (ss.size > 0)
            {
                struct tokenizer_ctx tctx = { 0 };
                struct token_list l = tokenizer(&tctx, ss.c_str, NULL, 0, TK_FLAG_FINAL);
                token_list_insert_after(&ctx->ast.token_list, p_declaration->function_body->last_token->prev, &l);
            }
            ss_close(&ss);
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
            visit_compound_statement(ctx, p_declaration->function_body);
        }
    }
}

int visit_literal_string(struct visit_ctx* ctx, struct token* current)
{
    bool has_u8_prefix =
        current->lexeme[0] == 'u' && current->lexeme[1] == '8';

    if (has_u8_prefix && ctx->target < LANGUAGE_C11)
    {
        struct osstream ss = { 0 };
        unsigned char* psz = (unsigned char*)(current->lexeme + 2);

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

    return 0;
}

int visit_tokens(struct visit_ctx* ctx)
{
    struct token* current = ctx->ast.token_list.head;
    while (current)
    {

        if (current->type == TK_STRING_LITERAL)
        {
            visit_literal_string(ctx, current);
        }

        if (ctx->target < LANGUAGE_C2X)
        {
            if (current->type == TK_LINE_COMMENT)
            {
                if (ctx->target < LANGUAGE_C99)
                {
                    struct osstream ss = { 0 };
                    //TODO  check /* inside
                    ss_fprintf(&ss, "/*%s*/", current->lexeme + 2);
                    free(current->lexeme);
                    _del_attr(ss, MUST_DESTROY);
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
            else if (current->type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT)
            {
                remove_char(current->lexeme, '\'');

                if (ctx->target < LANGUAGE_C99)
                {
                    /*
                     * C99 Hexadecimal floating constants to C89.
                     */
                    long double d = strtold(current->lexeme, 0);
                    char buffer[50] = { 0 };
                    snprintf(buffer, sizeof buffer, "%Lg", d);
                    free(current->lexeme);
                    current->lexeme = strdup(buffer);
                }
            }
            else if (current->type == TK_COMPILER_DECIMAL_CONSTANT ||
                current->type == TK_COMPILER_OCTAL_CONSTANT ||
                current->type == TK_COMPILER_HEXADECIMAL_CONSTANT ||
                current->type == TK_COMPILER_DECIMAL_FLOATING_CONSTANT ||
                current->type == TK_PPNUMBER)
            {
                /*remove digit separators*/
                remove_char(current->lexeme, '\'');
            }
        }

        current = current->next;
    }
    return 0;
}

void visit(struct visit_ctx* ctx)
{
    ctx->capture_index = 0;
    ctx->lambdas_index = 0;

    if (visit_tokens(ctx) != 0)
        return;

    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        visit_declaration(ctx, p_declaration);

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
                    struct tokenizer_ctx tctx = { 0 };
                    struct token_list list = tokenizer(&tctx, "\n", NULL, 0, TK_FLAG_NONE);
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

                struct tokenizer_ctx tctx = { 0 };
                struct token_list list = tokenizer(&tctx, blanks, NULL, 0, TK_FLAG_NONE);
                token_list_insert_after(&ctx->ast.token_list, previous_token, &list);
            }
        }
    }
}

void ajust_if_begin(struct token* token, struct format_visit_ctx* ctx)
{
    /*
    * if we have 
      newline blancks
      then we ident
    */
    if (token && token->level == 0)
    {
        struct token* previous_token = token->prev;
        if (previous_token && previous_token->type == TK_BLANKS)
        {
            struct token* previous_previous_token =
                previous_token->prev;
            if (previous_previous_token &&
                previous_previous_token->type == TK_NEWLINE)
            {
                char blanks[50] = { 0 };
                if (ctx->identation > 0)
                    snprintf(blanks, sizeof blanks, "%*c", (ctx->identation * 4), ' ');

                /*only adjust the number of spaces*/
                free(previous_token->lexeme);
                previous_token->lexeme = strdup(blanks);
            }
        }
    }
}

static void format_visit_unlabeled_statement(struct format_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement);
static void format_visit_labeled_statement(struct format_visit_ctx* ctx, struct labeled_statement* p_labeled_statement);

static void format_visit_statement(struct format_visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        format_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        format_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}



static void format_visit_selection_statement(struct format_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    if (p_selection_statement->secondary_block)
    {
        ajust_line_and_identation(p_selection_statement->secondary_block->first_token, ctx);

        if (p_selection_statement->secondary_block &&
            p_selection_statement->secondary_block->statement &&
            p_selection_statement->secondary_block->statement->unlabeled_statement &&
            p_selection_statement->secondary_block->statement->unlabeled_statement->primary_block &&
            p_selection_statement->secondary_block->statement->unlabeled_statement->primary_block->compound_statement)
        {
            format_visit_statement(ctx, p_selection_statement->secondary_block->statement);
        }
        else
        {
            ajust_line_and_identation(p_selection_statement->secondary_block->first_token, ctx);

            format_visit_statement(ctx, p_selection_statement->secondary_block->statement);            
        }        
    }

    if (p_selection_statement->else_secondary_block_opt)
    {
        ajust_line_and_identation(p_selection_statement->else_token_opt, ctx);
        ajust_line_and_identation(p_selection_statement->else_secondary_block_opt->first_token, ctx);

        if (p_selection_statement->else_secondary_block_opt->statement &&
            p_selection_statement->else_secondary_block_opt->statement->unlabeled_statement &&
            p_selection_statement->else_secondary_block_opt->statement->unlabeled_statement->primary_block &&
            p_selection_statement->else_secondary_block_opt->statement->unlabeled_statement->primary_block->compound_statement)
        {
            /*compound statement*/
            format_visit_statement(ctx, p_selection_statement->else_secondary_block_opt->statement);
        }
        else
        {            
            format_visit_statement(ctx, p_selection_statement->else_secondary_block_opt->statement);         
        }
    }

}

static void format_visit_jump_statement(struct format_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{



    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW ||
        p_jump_statement->first_token->type == TK_KEYWORD_RETURN ||
        p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE ||
        p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {
        ajust_line_and_identation(p_jump_statement->first_token, ctx);
    }
    else
    {
        assert(false);
    }
}

static void format_visit_compound_statement(struct format_visit_ctx* ctx, struct compound_statement* p_compound_statement);


static void format_visit_secondary_block(struct format_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    format_visit_statement(ctx, p_secondary_block->statement);
}

static void format_visit_iteration_statement(struct format_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    ajust_line_and_identation(p_iteration_statement->first_token, ctx);

    if (p_iteration_statement->expression1)
    {
        //format_visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->expression2)
    {
        //format_visit_expression(ctx, p_iteration_statement->expression2);
    }

    if (p_iteration_statement->first_token->type == TK_KEYWORD_DO)
    {
        ajust_line_and_identation(p_iteration_statement->second_token, ctx);
    }

    if (p_iteration_statement->secondary_block)
    {
        format_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    }
}

static void format_visit_try_statement(struct format_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    if (p_try_statement->secondary_block)
        format_visit_secondary_block(ctx, p_try_statement->secondary_block);

    if (p_try_statement->catch_secondary_block_opt)
    {
        format_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
    }
}


static void format_visit_primary_block(struct format_visit_ctx* ctx, struct primary_block* p_primary_block)
{

    if (p_primary_block->compound_statement)
    {
        format_visit_compound_statement(ctx, p_primary_block->compound_statement);
    }
    else if (p_primary_block->iteration_statement)
    {
        format_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
    }
    else if (p_primary_block->selection_statement)
    {
        format_visit_selection_statement(ctx, p_primary_block->selection_statement);
    }
    else if (p_primary_block->defer_statement)
    {
        //visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else if (p_primary_block->try_statement)
    {
        format_visit_try_statement(ctx, p_primary_block->try_statement);
    }
    else
    {
        assert(false);
    }
}


static void format_visit_expression_statement(struct format_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
    {
        //ajust_line_and_identation(p_expression_statement->first_token, ctx);
    }
}

static void format_visit_labeled_statement(struct format_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    ajust_line_and_identation(p_labeled_statement->label->name, ctx);

    if (p_labeled_statement->statement)
        format_visit_statement(ctx, p_labeled_statement->statement);
}

static void format_visit_unlabeled_statement(struct format_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        format_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        format_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        format_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}

static void format_visit_block_item(struct format_visit_ctx* ctx, struct block_item* p_block_item)
{
    ajust_line_and_identation(p_block_item->first_token, ctx);

    if (p_block_item->declaration)
    {
        //visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        format_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        //visit_labeled_statement(ctx, p_block_item->labeled_statement);
    }
}

static void format_visit_block_item_list(struct format_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        format_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void format_visit_compound_statement(struct format_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    ajust_line_and_identation(p_compound_statement->first_token, ctx);

    ctx->identation++;
    format_visit_block_item_list(ctx, &p_compound_statement->block_item_list);

    ctx->identation++;
    /*fix comments anything that is not part of AST*/
    struct token* tk = p_compound_statement->first_token;
    while (tk)
    {
        if (tk->type == TK_LINE_COMMENT ||
            tk->type == TK_COMMENT)
        {
            ajust_if_begin(tk, ctx);
        }
        tk = tk->next;
    }
    ctx->identation--;

    ctx->identation--;

    ajust_line_and_identation(p_compound_statement->last_token, ctx);
}

static void format_visit_declaration(struct format_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        //format_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }

    if (p_declaration->declaration_specifiers)
    {
        //format_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers);

    }

    if (p_declaration->init_declarator_list.head)
    {
        //format_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        format_visit_compound_statement(ctx, p_declaration->function_body);
    }
}

void format_visit(struct format_visit_ctx* ctx)
{
    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        format_visit_declaration(ctx, p_declaration);
        p_declaration = p_declaration->next;
    }
}




static void wasm_visit_attribute_specifier_sequence(struct wasm_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence);


static void wasm_visit_struct_or_union_specifier(struct wasm_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void wasm_visit_expression(struct wasm_visit_ctx* ctx, struct expression* p_expression);
static void wasm_visit_statement(struct wasm_visit_ctx* ctx, struct statement* p_statement);
static void wasm_visit_enum_specifier(struct wasm_visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void wasm_visit_type_specifier(struct wasm_visit_ctx* ctx, struct type_specifier* p_type_specifier);


static void wasm_visit_secondary_block(struct wasm_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    wasm_visit_statement(ctx, p_secondary_block->statement);
}

static void wasm_visit_defer_statement(struct wasm_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{






    if (p_defer_statement->secondary_block)
    {
        wasm_visit_secondary_block(ctx, p_defer_statement->secondary_block);
    }



}

static void wasm_visit_try_statement(struct wasm_visit_ctx* ctx, struct try_statement* p_try_statement)
{



    if (p_try_statement->secondary_block)
        wasm_visit_secondary_block(ctx, p_try_statement->secondary_block);




}

static void wasm_visit_selection_statement(struct wasm_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{

    if (p_selection_statement->else_secondary_block_opt)
        wasm_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);


}

static void wasm_visit_compound_statement(struct wasm_visit_ctx* ctx, struct compound_statement* p_compound_statement);



static void wasm_visit_initializer_list(struct wasm_visit_ctx* ctx, struct initializer_list* p_initializer_list);

static void wasm_visit_bracket_initializer_list(struct wasm_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {

    }
    else
    {
        wasm_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void wasm_visit_designation(struct wasm_visit_ctx* ctx, struct designation* p_designation)
{
}

static void wasm_visit_initializer(struct wasm_visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->designation)
    {
        wasm_visit_designation(ctx, p_initializer->designation);
    }

    if (p_initializer->assignment_expression)
    {
        wasm_visit_expression(ctx, p_initializer->assignment_expression);
    }
    else if (p_initializer->braced_initializer)
    {
        wasm_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void wasm_visit_initializer_list(struct wasm_visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer* p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        wasm_visit_initializer(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void wasm_visit_type_qualifier(struct wasm_visit_ctx* ctx, struct type_qualifier* p_type_qualifier)
{
    if (p_type_qualifier->token &&
        p_type_qualifier->token->type == TK_KEYWORD_RESTRICT)
    {

    }
}

static void wasm_visit_specifier_qualifier(struct wasm_visit_ctx* ctx, struct type_specifier_qualifier* p_specifier_qualifier)
{
    if (p_specifier_qualifier->type_specifier)
        wasm_visit_type_specifier(ctx, p_specifier_qualifier->type_specifier);

    if (p_specifier_qualifier->type_qualifier)
        wasm_visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier);
}

static void wasm_visit_specifier_qualifier_list(struct wasm_visit_ctx* ctx, struct specifier_qualifier_list* p_specifier_qualifier_list_opt)
{
    if (p_specifier_qualifier_list_opt == NULL)
        return;

    if (p_specifier_qualifier_list_opt->struct_or_union_specifier)
    {
        wasm_visit_struct_or_union_specifier(ctx, p_specifier_qualifier_list_opt->struct_or_union_specifier);
    }
    else if (p_specifier_qualifier_list_opt->enum_specifier)
    {
        wasm_visit_enum_specifier(ctx, p_specifier_qualifier_list_opt->enum_specifier);
    }
    else if (p_specifier_qualifier_list_opt->typedef_declarator)
    {
        //typedef name
    }
    //else if (p_specifier_qualifier_list->p_typeof_expression_opt)
    //{
      //  wasm_visit_expression(ctx, p_specifier_qualifier_list->p_typeof_expression_opt);
    //}
    else
    {
        struct type_specifier_qualifier* p_specifier_qualifier = p_specifier_qualifier_list_opt->head;
        while (p_specifier_qualifier)
        {
            wasm_visit_specifier_qualifier(ctx, p_specifier_qualifier);
            p_specifier_qualifier = p_specifier_qualifier->next;
        }
    }
}
static void wasm_visit_declarator(struct wasm_visit_ctx* ctx, struct declarator* p_declarator);
static void wasm_visit_type_name(struct wasm_visit_ctx* ctx, struct type_name* p_type_name)
{
    wasm_visit_specifier_qualifier_list(ctx, p_type_name->specifier_qualifier_list);
    wasm_visit_declarator(ctx, p_type_name->declarator);
}


static void wasm_visit_argument_expression_list(struct wasm_visit_ctx* ctx, struct argument_expression_list* p_argument_expression_list)
{
    struct argument_expression* p_argument_expression =
        p_argument_expression_list->head;
    while (p_argument_expression)
    {
        wasm_visit_expression(ctx, p_argument_expression->expression);
        p_argument_expression = p_argument_expression->next;
    }
}

static void wasm_visit_generic_selection(struct wasm_visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        wasm_visit_expression(ctx, p_generic_selection->expression);
    }
    else if (p_generic_selection->type_name)
    {
        wasm_visit_type_name(ctx, p_generic_selection->type_name);
    }



}


static void wasm_visit_expression(struct wasm_visit_ctx* ctx, struct expression* p_expression)
{
    switch (p_expression->expression_type)
    {
    case PRIMARY_EXPRESSION__FUNC__:
        break;
    case PRIMARY_IDENTIFIER:
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
        if (p_expression->first_token &&
            p_expression->first_token->type == TK_KEYWORD_NULLPTR)
        {

        }
        else if (p_expression->first_token &&
            p_expression->first_token->type == TK_KEYWORD_TRUE)
        {

        }
        else if (p_expression->first_token &&
            p_expression->first_token->type == TK_KEYWORD_FALSE)
        {

        }
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        wasm_visit_generic_selection(ctx, p_expression->generic_selection);
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
        //visit_expression(ctx, p_expression->left);
        break;
    case POSTFIX_FUNCTION_CALL:
        wasm_visit_expression(ctx, p_expression->left);
        wasm_visit_argument_expression_list(ctx, &p_expression->argument_expression_list);
        break;
    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
    {

        wasm_visit_type_name(ctx, p_expression->type_name);
        wasm_visit_compound_statement(ctx, p_expression->compound_statement);


    }
    break;

    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

        if (p_expression->type_name)
        {
            wasm_visit_type_name(ctx, p_expression->type_name);
        }

        //struct token_list list0 = tokenizer("int teste = 0;", NULL, 0, TK_FLAG_NONE);
        //token_list_append_list(&ctx->insert_before_block_item, &list0);

        wasm_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);

        break;

    case UNARY_EXPRESSION_ALIGNOF:


        if (p_expression->right)
        {
            wasm_visit_expression(ctx, p_expression->right);
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            wasm_visit_type_name(ctx, p_expression->type_name);
        }
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
            wasm_visit_expression(ctx, p_expression->right);
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            wasm_visit_type_name(ctx, p_expression->type_name);
        }

        break;
    
    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
        break;

    case UNARY_EXPRESSION_HASHOF_TYPE:


        break;


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
            wasm_visit_expression(ctx, p_expression->left);
        }
        if (p_expression->right)
        {
            wasm_visit_expression(ctx, p_expression->right);
        }
        if (p_expression->type_name)
        {
            wasm_visit_type_name(ctx, p_expression->type_name);
        }
        break;

    case CONDITIONAL_EXPRESSION:
        if (p_expression->left)
        {
            wasm_visit_expression(ctx, p_expression->left);
        }
        if (p_expression->right)
        {
            wasm_visit_expression(ctx, p_expression->right);
        }
        if (p_expression->condition_expr)
        {
            wasm_visit_expression(ctx, p_expression->condition_expr);
        }
        break;
    case UNARY_EXPRESSION_TRAITS:
    {

    }
    break;

    case UNARY_EXPRESSION_IS_SAME:
        break;

    default:
        break;
    }
}

static void wasm_visit_expression_statement(struct wasm_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
        wasm_visit_expression(ctx, p_expression_statement->expression_opt);
}

static void wasm_visit_block_item_list(struct wasm_visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void wasm_visit_compound_statement(struct wasm_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    wasm_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
}

//
static void wasm_visit_iteration_statement(struct wasm_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{

    if (p_iteration_statement->expression1)
    {
        wasm_visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->expression2)
    {
        wasm_visit_expression(ctx, p_iteration_statement->expression2);
    }

    if (p_iteration_statement->first_token->type == TK_KEYWORD_REPEAT)
    {
        free(p_iteration_statement->first_token->lexeme);
        p_iteration_statement->first_token->lexeme = strdup("for(;;)/*repeat*/");
    }

    if (p_iteration_statement->secondary_block)
    {

        wasm_visit_secondary_block(ctx, p_iteration_statement->secondary_block);



    }
}



static void wasm_visit_jump_statement(struct wasm_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{

    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
    {

    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {

    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {

    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {

    }
    else
    {
        assert(false);
    }
}


static void wasm_visit_labeled_statement(struct wasm_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{

    if (p_labeled_statement->statement)
    {
        wasm_visit_statement(ctx, p_labeled_statement->statement);
    }
}

static void wasm_visit_primary_block(struct wasm_visit_ctx* ctx, struct primary_block* p_primary_block)
{



    if (p_primary_block->defer_statement)
    {
        wasm_visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            wasm_visit_compound_statement(ctx, p_primary_block->compound_statement);
        }
        else if (p_primary_block->iteration_statement)
        {
            wasm_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
        }
        else if (p_primary_block->selection_statement)
        {
            wasm_visit_selection_statement(ctx, p_primary_block->selection_statement);
        }
        else if (p_primary_block->try_statement)
        {
            wasm_visit_try_statement(ctx, p_primary_block->try_statement);
        }
    }

}

static void wasm_visit_unlabeled_statement(struct wasm_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        wasm_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        wasm_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        wasm_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}

static void wasm_visit_declaration(struct wasm_visit_ctx* ctx, struct declaration* p_declaration);

static void wasm_visit_statement(struct wasm_visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        wasm_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        wasm_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void wasm_visit_label(struct wasm_visit_ctx* ctx, struct label* p_label)
{
    //p_label->name
}
static void wasm_visit_block_item(struct wasm_visit_ctx* ctx, struct block_item* p_block_item)
{
    if (p_block_item->declaration)
    {
        wasm_visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        wasm_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        wasm_visit_label(ctx, p_block_item->label);
    }

}

static void wasm_visit_block_item_list(struct wasm_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        wasm_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void wasm_visit_static_assert_declaration(struct wasm_visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration)
{
    wasm_visit_expression(ctx, p_static_assert_declaration->constant_expression);


}
static void wasm_visit_declaration_specifiers(struct wasm_visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers);


static void wasm_visit_direct_declarator(struct wasm_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {


        struct parameter_declaration* parameter = NULL;

        if (p_direct_declarator->function_declarator->parameter_type_list_opt)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }

        while (parameter)
        {
            ss_fprintf(&ctx->ss, "(param ");

            ss_fprintf(&ctx->ss, " i32"); //todo type


            if (parameter->declarator->name)
                ss_fprintf(&ctx->ss, " %%%s", parameter->declarator->name->lexeme);


            if (parameter->attribute_specifier_sequence_opt)
            {
                wasm_visit_attribute_specifier_sequence(ctx, parameter->attribute_specifier_sequence_opt);
            }

            wasm_visit_declaration_specifiers(ctx, parameter->declaration_specifiers);
            wasm_visit_declarator(ctx, parameter->declarator);
            parameter = parameter->next;
            ss_fprintf(&ctx->ss, ")");
        }

    }
    else if (p_direct_declarator->array_declarator)
    {

    }
}

static void wasm_visit_declarator(struct wasm_visit_ctx* ctx, struct declarator* p_declarator)
{
    if (p_declarator->direct_declarator)
    {
        wasm_visit_direct_declarator(ctx, p_declarator->direct_declarator);
    }
}

static void wasm_visit_init_declarator_list(struct wasm_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* p_init_declarator = p_init_declarator_list->head;



    while (p_init_declarator)
    {
        if (p_init_declarator->declarator)
        {
            wasm_visit_declarator(ctx, p_init_declarator->declarator);
        }

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                wasm_visit_expression(ctx, p_init_declarator->initializer->assignment_expression);
            }
            else
            {
                assert(p_init_declarator->initializer->braced_initializer != NULL);
                if (p_init_declarator->initializer->braced_initializer)
                {
                    wasm_visit_bracket_initializer_list(ctx,
                        p_init_declarator->initializer->braced_initializer);
                }

            }
        }

        p_init_declarator = p_init_declarator->next;
    }
}



static void wasm_visit_member_declarator(struct wasm_visit_ctx* ctx, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {

    }
}

static void wasm_visit_member_declarator_list(struct wasm_visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        wasm_visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}
static void wasm_visit_member_declaration(struct wasm_visit_ctx* ctx, struct member_declaration* p_member_declaration)
{
    wasm_visit_specifier_qualifier_list(ctx, p_member_declaration->specifier_qualifier_list);

    if (p_member_declaration->member_declarator_list_opt)
    {
        wasm_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void wasm_visit_member_declaration_list(struct wasm_visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* p_member_declaration =
        p_member_declaration_list->head;
    while (p_member_declaration)
    {
        wasm_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void wasm_visit_attribute_specifier(struct wasm_visit_ctx* ctx, struct attribute_specifier* p_attribute_specifier)
{

}

static void wasm_visit_attribute_specifier_sequence(struct wasm_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence)
{
    struct attribute_specifier* current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        wasm_visit_attribute_specifier(ctx, current);
        current = current->next;
    }
}

static void wasm_visit_struct_or_union_specifier(struct wasm_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{

    if (p_struct_or_union_specifier->attribute_specifier_sequence_opt)
        wasm_visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence_opt);


    wasm_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);

}

static void wasm_visit_enumerator(struct wasm_visit_ctx* ctx, struct enumerator* p_enumerator)
{
    if (p_enumerator->constant_expression_opt)
        wasm_visit_expression(ctx, p_enumerator->constant_expression_opt);

}

//struct enumerator_list* enumerator_list;
static void wasm_visit_enumerator_list(struct wasm_visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* current = p_enumerator_list->head;
    while (current)
    {
        wasm_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void wasm_visit_enum_specifier(struct wasm_visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    

    if (p_enum_specifier->attribute_specifier_sequence_opt)
    {
        wasm_visit_attribute_specifier_sequence(ctx, p_enum_specifier->attribute_specifier_sequence_opt);
    }
    wasm_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);

}

static void wasm_visit_typeof_specifier(struct wasm_visit_ctx* ctx, struct typeof_specifier* p_typeof_specifier)
{




}

static void wasm_visit_type_specifier(struct wasm_visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->typeof_specifier)
    {
        wasm_visit_typeof_specifier(ctx, p_type_specifier->typeof_specifier);
    }

    if (p_type_specifier->struct_or_union_specifier)
    {
        wasm_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        wasm_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }


    if (p_type_specifier->atomic_type_specifier)
    {
        //visit_deped(ctx, p_type_specifier->enum_specifier);
    }


}

static void wasm_visit_type_specifier_qualifier(struct wasm_visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_qualifier)
    {
    }
    else if (p_type_specifier_qualifier->type_specifier)
    {
        wasm_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
    else if (p_type_specifier_qualifier->alignment_specifier)
    {
    }
}

static void wasm_visit_storage_class_specifier(struct wasm_visit_ctx* ctx, struct storage_class_specifier* p_storage_class_specifier)
{
    if (p_storage_class_specifier->flags & STORAGE_SPECIFIER_CONSTEXPR)
    {


    }
}

static void wasm_visit_declaration_specifier(struct wasm_visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{

    if (p_declaration_specifier->function_specifier)
    {
        if (p_declaration_specifier->function_specifier->token->type == TK_KEYWORD__NORETURN)
        {


        }
    }


    if (p_declaration_specifier->storage_class_specifier)
    {
        wasm_visit_storage_class_specifier(ctx, p_declaration_specifier->storage_class_specifier);

    }

    if (p_declaration_specifier->type_specifier_qualifier)
    {
        wasm_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);

    }

}

static void wasm_visit_declaration_specifiers(struct wasm_visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers)
{
    struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;
    while (p_declaration_specifier)
    {
        wasm_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}


static void wasm_visit_declaration(struct wasm_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        wasm_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
        wasm_visit_attribute_specifier_sequence(ctx, p_declaration->p_attribute_specifier_sequence_opt);
    }

    if (p_declaration &&
        p_declaration->init_declarator_list.head &&
        p_declaration->init_declarator_list.head->declarator)
    {
        if (type_is_function(&p_declaration->init_declarator_list.head->declarator->type))
        {
            ss_fprintf(&ctx->ss, "(func\n");
        }
    }

    if (p_declaration->declaration_specifiers)
    {
        wasm_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers);
    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
    }


    if (p_declaration->init_declarator_list.head)
    {
        wasm_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    //if (type_is_function(&p_declaration->init_declarator_list.head->declarator->type))
    //{
        //TODO result
      //  ss_fprintf(&ctx->ss, " (result i23)\n");
    //}

    if (p_declaration->function_body)
    {
        wasm_visit_compound_statement(ctx, p_declaration->function_body);
    }
    ss_fprintf(&ctx->ss, ")\n");
}

int wasm_visit_literal_string(struct wasm_visit_ctx* ctx, struct token* current)
{


    return 0;
}


void wasm_visit(struct wasm_visit_ctx* ctx)
{
    //https://developer.mozilla.org/en-US/docs/WebAssembly
    //https://wasdk.github.io/WasmFiddle/    
    //https://webassembly.github.io/wabt/demo/wat2wasm/
    //https://webassembly.github.io/wabt/demo/wasm2wat/index.html

    ss_fprintf(&ctx->ss, "(module\n");
    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        wasm_visit_declaration(ctx, p_declaration);


        p_declaration = p_declaration->next;
    }
    ss_fprintf(&ctx->ss, "\n)");

}
