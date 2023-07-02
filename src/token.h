#pragma once
#include <stdbool.h>
#include "annotations.h"

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
    TK_KEYWORD__BITINT /*C23*/,

    TK_KEYWORD__OWNER, 
    TK_KEYWORD__OBJ_OWNER, 
    TK_KEYWORD__VIEW,
    TK_KEYWORD__MOVE,
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
    char* _owner lexeme;
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
char* _owner token_list_join_tokens(struct token_list* list, bool bliteral);
void token_list_clear(struct token_list* list);
bool token_is_blank(struct token* p);
void token_range_add_flag(struct token* first, struct token* last, enum token_flags flag);
void token_range_remove_flag(struct token* first, struct token* last, enum token_flags flag);
void token_range_add_show(struct token* first, struct token* last);

void print_tokens_html(struct token* p_token);
void print_line_and_token(const struct token* p_token);
void print_position(const char* path, int line, int col);
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

bool style_has_space(const struct token* token);
bool style_has_one_space(const struct token* token);