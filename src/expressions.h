#pragma once
#include "type.h"
#include  "tokenizer.h"

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
