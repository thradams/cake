#pragma once
#include "type.h"
#include  "tokenizer.h"
#include "ownership.h"

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
    struct argument_expression* owner head;
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
    struct type_name* owner p_type_name;
    struct expression* owner expression;

    struct token* first_token;
    struct token* last_token;

    struct generic_association* owner next;
};

struct generic_assoc_list
{
    struct generic_association* owner head;
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


    struct expression* owner expression;
    struct type_name* owner type_name;
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
void constant_value_to_string(const struct constant_value* a, char buffer[], int sz);

struct expression
{
    enum expression_type expression_type;
    struct type type;

    struct constant_value constant_value;

    struct type_name* owner type_name; 
    struct type_name* owner type_name2; /*is_same*/
    struct braced_initializer* owner braced_initializer;
    struct compound_statement* owner compound_statement; //function literal (lambda)
    struct generic_selection* owner generic_selection; //_Generic

    struct token* first_token;
    struct token* last_token;

    /*token used in _add_attr, _has_attr, _del_attr, return*/
    struct token* contract_arg_token;

    struct token* move_assignment;

    /*se expressão for um identificador ele aponta para declaração dele*/
    struct declarator* declarator;
    int member_index; //used in post_fix .

    /*se for POSTFIX_FUNCTION_CALL post*/
    struct argument_expression_list argument_expression_list; //este node eh uma  chamada de funcao

    struct expression* owner condition_expr;
    struct expression* owner left;
    struct expression* owner right;
};
void expression_delete(implicit struct expression* owner);

struct expression* owner assignment_expression(struct parser_ctx* ctx);
struct expression* owner expression(struct parser_ctx* ctx);
struct expression* owner constant_expression(struct parser_ctx* ctx, bool show_error_if_not_constant);
bool expression_is_subjected_to_lvalue_conversion(struct expression*);
bool expression_is_zero(struct expression*);
struct object* expression_get_object(struct expression* p_expression, struct type* p_type);
bool is_null_pointer_constant(const struct expression* expression);
void expression_evaluate_equal_not_equal(const struct expression* left,
    const struct expression* right,
    struct expression* result,
    int op);