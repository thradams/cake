/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once

#include "type.h"
#include  "tokenizer.h"
#include "ownership.h"
#include "object.h"

struct parser_ctx;
struct defer_list_item;

struct defer_list
{
    struct defer_list_item* _Opt _Owner head;
    struct defer_list_item* _Opt tail;
};


enum expression_type
{
    EXPR_INVALID, 

    EXPR_PRIMARY_ENUMERATOR,
    EXPR_PRIMARY_DECLARATOR,    
    EXPR_PRIMARY_STRING_LITERAL,
    EXPR_PRIMARY__FUNC__, /*predefined identifier __func__ */
    EXPR_PRIMARY_CHAR_LITERAL,
    EXPR_PRIMARY_PREDEFINED_CONSTANT, /*true false*/
    EXPR_PRIMARY_GENERIC,
    EXPR_PRIMARY_NUMBER,
    EXPR_PRIMARY_PARENTHESIS,
    EXPR_PRIMARY_STATEMENT_EXPRESSION, //GCC

    EXPR_POSTFIX_FUNCTION_LITERAL,
    EXPR_POSTFIX_COMPOUND_LITERAL,

    EXPR_POSTFIX_FUNCTION_CALL, // ( ) 
    EXPR_POSTFIX_ARRAY, // [ ]
    EXPR_POSTFIX_DOT, // .
    EXPR_POSTFIX_ARROW, // .
    EXPR_POSTFIX_INCREMENT,
    EXPR_POSTFIX_DECREMENT,    


    EXPR_UNARY_SIZEOF_EXPRESSION,
    EXPR_UNARY_SIZEOF_TYPE,
    EXPR_UNARY_COUNTOF,


    EXPR_UNARY_STATIC_ASSERTION, //not defined yet C2Y
    
    EXPR_UNARY_GCC__BUILTIN_VA_START,
    EXPR_UNARY_GCC__BUILTIN_VA_END,    
    EXPR_UNARY_GCC__BUILTIN_VA_COPY,
    EXPR_UNARY_GCC__BUILTIN_VA_ARG,
    EXPR_UNARY_GCC__BUILTIN_OFFSETOF,


    EXPR_UNARY_TRAITS,
    EXPR_UNARY_IS_SAME,
    EXPR_UNARY_DECLARATOR_ATTRIBUTE,
    EXPR_UNARY_ALIGNOF_TYPE,
    EXPR_UNARY_ALIGNOF_EXPRESSION,
    EXPR_UNARY_ASSERT,
    EXPR_UNARY_CONSTEVAL,

    EXPR_UNARY_INCREMENT,
    EXPR_UNARY_DECREMENT,

    EXPR_UNARY_NOT,
    EXPR_UNARY_BITNOT,
    EXPR_UNARY_NEG,
    EXPR_UNARY_PLUS,
    EXPR_UNARY_CONTENT,
    EXPR_UNARY_ADDRESSOF,

    EXPR_CAST,

    EXPR_MULTIPLICATIVE_MULT,
    EXPR_MULTIPLICATIVE_DIV,
    EXPR_MULTIPLICATIVE_MOD,

    EXPR_ADDITIVE_PLUS,
    EXPR_ADDITIVE_MINUS,

    EXPR_SHIFT_RIGHT,
    EXPR_SHIFT_LEFT,

    EXPR_RELATIONAL_BIGGER_THAN,
    EXPR_RELATIONAL_LESS_THAN,
    EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN,
    EXPR_RELATIONAL_LESS_OR_EQUAL_THAN,

    EXPR_EQUALITY_EQUAL,
    EXPR_EQUALITY_NOT_EQUAL,

    EXPR_AND,
    EXPR_EXCLUSIVE_OR,
    EXPR_INCLUSIVE_OR,

    EXPR_LOGICAL_OR,  //||
    EXPR_LOGICAL_AND, //&&
    
    EXPR_ASSIGNMENT_ASSIGN,
    EXPR_ASSIGNMENT_PLUS_ASSIGN,
    EXPR_ASSIGNMENT_MINUS_ASSIGN,
    EXPR_ASSIGNMENT_MULTI_ASSIGN,
    EXPR_ASSIGNMENT_DIV_ASSIGN,
    EXPR_ASSIGNMENT_MOD_ASSIGN,
    EXPR_ASSIGNMENT_SHIFT_LEFT_ASSIGN,
    EXPR_ASSIGNMENT_SHIFT_RIGHT_ASSIGN,
    EXPR_ASSIGNMENT_AND_ASSIGN,
    EXPR_ASSIGNMENT_OR_ASSIGN,
    EXPR_ASSIGNMENT_NOT_ASSIGN,
                       
    EXPR_CHECKED,

    EXPR_EXPRESSION,

    EXPR_CONDITIONAL,
};

bool is_primary_expression(enum expression_type t);

struct argument_expression_list
{
    /*
     argument-expression-list:
        assignment-expression
        argument-expression-list , assignment-expression
    */
    struct argument_expression* _Owner _Opt head;
    struct argument_expression* _Opt tail;
};

void argument_expression_list_destroy(_Dtor struct argument_expression_list* p);
void argument_expression_list_push(struct argument_expression_list* list, struct argument_expression* _Owner p);

struct generic_association
{
    /*
     generic-association:
       type-name : assignment-expression
       "default" : assignment-expression
    */

    struct type type;
    struct type_name* _Owner _Opt p_type_name;
    struct expression* _Owner expression;

    struct token* first_token;
    struct token* last_token;

    struct generic_association* _Owner _Opt next;
};

void generic_association_delete(struct generic_association* _Owner _Opt p);

struct generic_assoc_list
{
    struct generic_association* _Owner _Opt head;
    struct generic_association* _Opt tail;
};

void generic_assoc_list_add(struct generic_assoc_list* p, struct generic_association* _Owner item);
void generic_assoc_list_destroy(_Dtor struct generic_assoc_list* p);

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


    struct expression* _Owner _Opt expression;
    struct type_name* _Owner _Opt type_name;
    /*
    * Points to the matching expression
    */
    struct expression* _Opt p_view_selected_expression;

    struct generic_assoc_list generic_assoc_list;
    struct token* first_token;
    struct token* last_token;
};

void generic_selection_delete(struct generic_selection* _Owner _Opt p);

struct expression
{
    enum expression_type expression_type;
    struct type type;

    struct object object;

    struct type_name* _Owner _Opt type_name;

    struct storage_class_specifiers * _Owner _Opt p_storage_class_specifiers;
    struct braced_initializer* _Owner _Opt braced_initializer;
    struct compound_statement* _Owner _Opt compound_statement; //function literal (lambda)
    struct generic_selection* _Owner _Opt generic_selection; //_Generic
    struct static_assertion * _Owner _Opt static_assertion; //_Generic
    struct token* first_token;
    struct token* last_token;
    
    //TODO https://gcc.gnu.org/onlinedocs/gcc/Offsetof.html#Offsetof
    struct token* _Opt offsetof_member_designator;

    /*if expression is an identifier it points to its declaration*/
    struct declarator* _Opt declarator;
    struct init_declarator* _Opt p_init_declarator;

    int member_index; //used in post_fix .

    /*se for EXPR_POSTFIX_FUNCTION_CALL post*/
    struct argument_expression_list argument_expression_list; //este node eh uma  chamada de funcao

    struct expression* _Owner _Opt condition_expr;
    struct expression* _Owner _Opt left;
    struct expression* _Owner _Opt right;

    bool is_assignment_expression;

    /*  used to check how contexpr can be used inside function literals */
    bool lvalue_disabled;       

    struct defer_list defer_list; //arguments
};

//built-in semantics
bool expression_is_malloc(const struct expression* p);
bool expression_is_calloc(const struct expression* p);

void expression_delete(struct expression* _Owner _Opt p);

/*cake extension*/
struct expression* _Owner _Opt checked_expression(struct parser_ctx* ctx, bool is_discarded);

struct expression* _Owner _Opt assignment_expression(struct parser_ctx* ctx, bool is_discarded);
struct expression* _Owner _Opt expression(struct parser_ctx* ctx, bool is_discarded);
struct expression* _Owner _Opt constant_expression(struct parser_ctx* ctx, bool show_error_if_not_constant, bool is_discarded);
bool expression_is_subjected_to_lvalue_conversion(const struct expression*);


bool expression_is_lvalue(const struct expression* expr);

bool expression_has_side_effects(const struct expression* expr);

bool expression_is_one(const struct expression* expression);
bool expression_is_zero(const struct expression* expression);
bool expression_is_null_pointer_constant(const struct expression* expression);

void check_diferent_enuns(struct parser_ctx* ctx,
                          const struct token* operator_token,
                          const struct expression* left,
                          const struct expression* right,
                          const char* message);

void check_assigment(struct parser_ctx* ctx,
    const struct type* left_type,
    const struct expression* right,
    enum assigment_type assigment_type);

struct marker expression_to_marker(const struct expression* p_expression);

void flow3_expression_to_string(const struct expression* p_expression, struct osstream* oss);
