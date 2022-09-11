#pragma once
#include "hashmap.h"
#include "tokenizer.h"
#include "expressions.h"
#include <stdbool.h>
#include "osstream.h"
#include "type.h"
#include "options.h"




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
    * Points the the function we are inside or null
    */
    struct declaration* p_current_function_opt;

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

//////////
//
//PARA representar tipos fora da AST
//funciona clonando e nao depende da AST

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
    struct token* first_token;
    struct token* last_token;
    struct expression* p_conditional_expression;
    struct token* text_opt;
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
    enum storage_class_specifier_flags flags;
    struct token* token;
};
struct storage_class_specifier* storage_class_specifier(struct parser_ctx* ctx, struct error* error);

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
       typeof ( typeof-specifier-argument )
    */
    struct token* token;
    struct token* endtoken;
    struct typeof_specifier_argument* typeof_specifier_argument;
};

struct type_specifier
{
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

    char tagName[200];
    /*geramos um tag name para anomimas, mas colocamos banonymousTag para true*/
    bool bAnonymousTag;

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

    int nUses; /*used to show not used warnings*/

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
    bool bVarArgs; /*(...)*/
    bool bVoid;/*(void)*/
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
    struct type_specifier* pType_specifier;
    struct type_qualifier* pType_qualifier;
    struct alignment_specifier* pAlignment_specifier;

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

    struct static_assert_declaration* p_static_assert_declaration;

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
    struct token* first;
    struct token* last;

    struct block_item_list block_item_list;
};
struct compound_statement* compound_statement(struct parser_ctx* ctx, struct error* error);

struct defer_statement
{
    struct token* firsttoken;
    struct token* lasttoken;
    struct secondary_block* secondary_block;
};

struct selection_statement
{
    /*
    selection-statement:
     if ( expression ) secondary-block
     if ( expression ) secondary-block else secondary-block
     switch ( expression ) secondary-block
    */

    /*
     try secondary-block
     try secondary-block catch secondary-block
    */

    struct init_declarator* init_declarator;
    struct expression* expression;

    struct secondary_block* secondary_block;
    struct secondary_block* else_secondary_block;

    struct token* first_token;
    struct token* last_token;
    struct token* else_catch_token;

    int try_catch_block_index;
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
    */
    struct token* last;
    struct compound_statement* compound_statement;
    struct selection_statement* selection_statement;
    struct iteration_statement* iteration_statement;
    struct defer_statement* defer_statement;
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
    //unsigned long long value; tb?
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
//struct attribute* attribute(struct parser_ctx* ctx, struct error* error);


struct expression* typeid_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);

bool is_first_of_conditional_expression(struct parser_ctx* ctx);
//void assignment_expression(struct parser_ctx* ctx, struct error* error, struct expression_ctx* ectx);
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

