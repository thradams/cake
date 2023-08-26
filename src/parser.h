#pragma once
#include <stdio.h>
#include <errno.h>
#include "hashmap.h"
#include "tokenizer.h"
#include "expressions.h"
#include <stdbool.h>
#include "osstream.h"
#include "type.h"
#include "options.h"
#include "ownership.h"

#define CAKE_VERSION "0.7"


struct scope
{
    int scope_level;
    struct hash_map tags;
    struct hash_map variables;
        
    struct scope* next;
    struct scope* previous;        
};

void scope_destroy( struct scope* obj_owner p);

struct scope_list
{
    struct scope* head;
    struct scope* tail;
};
void scope_list_push(struct scope_list* list, struct scope* s);
void scope_list_pop(struct scope_list* list);

struct report
{
    int no_files;
    double cpu_time_used_sec;
    int error_count;
    int warnings_count;
    int info_count;
    enum error last_error;
    enum warning last_warning;
};




struct parser_ctx
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

    FILE* owner sarif_file;

    view struct token_list input_list;
    struct token* current;
    struct token* previous;
    int try_catch_block_index;

    struct report* p_report;
    
};

///////////////////////////////////////////////////////

void parser_ctx_destroy( struct parser_ctx* obj_owner ctx);


struct token* parser_look_ahead(struct parser_ctx* ctx);

struct token* parser_match(struct parser_ctx* ctx);
int parser_match_tk(struct parser_ctx* ctx, enum token_type type);
struct token* parser_look_ahead(struct parser_ctx* ctx);
struct token* previous_parser_token(struct token* token);
struct declarator* find_declarator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt);
struct enumerator* find_enumerator(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt);
struct map_entry* find_variables(struct parser_ctx* ctx, const char* lexeme, struct scope** ppscope_opt);

struct struct_or_union_specifier* find_struct_or_union_specifier(struct parser_ctx* ctx, const char* lexeme);
bool first_is(struct parser_ctx* ctx, enum token_type type);
void print_scope(struct scope_list* e);

char* CompileText(const char* options, const char* content);

void compiler_set_error_with_token(enum error error, struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...);
_Bool compiler_set_warning_with_token(enum warning w, struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...);
void compiler_set_info_with_token(enum warning w, struct parser_ctx* ctx, const struct token* p_token, const char* fmt, ...);

int compile(int argc, const char** argv, struct report* error);


void print_type_qualifier_flags(struct osstream* ss, bool* first, enum type_qualifier_flags e_type_qualifier_flags);

enum token_type parse_number(const char* lexeme, enum type_specifier_flags* flags_opt);
bool print_type_specifier_flags(struct osstream* ss, bool* first, enum type_specifier_flags e_type_specifier_flags);


struct expression_ctx;

struct declaration_specifier
{
    /*
       declaration-specifier:
         storage-class-specifier
         type-specifier-qualifier
         function-specifier
    */
    struct storage_class_specifier* owner storage_class_specifier;

    struct type_specifier_qualifier* owner  type_specifier_qualifier;
    
    struct function_specifier* owner function_specifier;

    struct declaration_specifier* owner next;
};

struct declaration_specifier* owner declaration_specifier(struct parser_ctx* ctx);
void declaration_specifier_delete(struct declaration_specifier* owner p);

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

    struct declaration_specifier* owner head;
    struct declaration_specifier* tail;
};

void print_declaration_specifiers(struct osstream* ss, struct declaration_specifiers* p);
struct declaration_specifiers* owner declaration_specifiers(struct parser_ctx* ctx, enum storage_class_specifier_flags default_storage_flag);
void declaration_specifiers_delete(struct declaration_specifiers* owner p);

struct static_assert_declaration
{
    /*
     static_assert-declaration:
       "static_assert" ( constant-expression , string-literal ) ;
       "static_assert" ( constant-expression ) ;
    */

    /*
      I am keeping the name static_assert_declaration but better is
      
      static_declaration:
       static_assert_declaration
       static_debug_declaration

      extension:
      "static_debug" ( constant-expression ) ;      
      "static_debug" ( constant-expression , string-literal) ;      
    */
    
    struct token*  first_token;
    struct token*  last_token;
    struct expression* owner constant_expression;
    struct token*  string_literal_opt;
};
struct static_assert_declaration* owner static_assert_declaration(struct parser_ctx* ctx);
void static_assert_declaration_delete(struct static_assert_declaration* owner p);


struct attribute_specifier_sequence
{
    /*
     attribute-specifier-sequence:
       attribute-specifier-sequence opt attribute-specifier
    */

    struct token* first_token;
    struct token* last_token;
    enum attribute_flags  attributes_flags;
    struct attribute_specifier* owner head;
    struct attribute_specifier* tail;
};
struct attribute_specifier_sequence* owner attribute_specifier_sequence_opt(struct parser_ctx* ctx);
void attribute_specifier_sequence_delete( struct attribute_specifier_sequence* owner p);

struct attribute_specifier
{
    /*
     attribute-specifier:
        [ [ attribute-list ] ]
    */
    struct token* first_token;
    struct token* last_token;
    struct attribute_list* owner attribute_list;
    struct attribute_specifier* owner next;
};

struct attribute_specifier*  owner attribute_specifier(struct parser_ctx* ctx);
void attribute_specifier_delete( struct attribute_specifier*  owner p);

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

struct storage_class_specifier* owner storage_class_specifier(struct parser_ctx* ctx);
void storage_class_specifier_delete(struct storage_class_specifier* owner p);

struct function_specifier
{
    /*
     function-specifier:
       inline
       _Noreturn
    */
    struct token* token;
};
struct function_specifier*  owner function_specifier(struct parser_ctx* ctx);
void function_specifier_delete(struct function_specifier*  owner p);

struct typeof_specifier_argument
{
    /*
     typeof-specifier-argument:
       expression
       type-name
    */
    struct expression* owner expression;
    struct type_name* owner type_name;
};

void typeof_specifier_argument_delete(struct typeof_specifier_argument* owner p);

struct typeof_specifier
{
    /*
     typeof-specifier:
       "typeof" ( typeof-specifier-argument )
    */
    struct token* first_token;
    struct token* last_token;
    struct typeof_specifier_argument*  owner typeof_specifier_argument;
    struct type type;    
};
void typeof_specifier_delete(struct typeof_specifier * owner p);

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
    struct struct_or_union_specifier* owner struct_or_union_specifier;
    struct typeof_specifier*  owner  typeof_specifier;
    struct enum_specifier*  owner enum_specifier;
    struct declarator*  view typedef_declarator;
    struct atomic_type_specifier* owner atomic_type_specifier;
};

struct type_specifier*  owner type_specifier(struct parser_ctx* ctx);
void type_specifier_delete(struct type_specifier*  owner p);

struct init_declarator_list
{
    /*
     init-declarator-list:
       init-declarator
       init-declarator-list , init-declarator
    */
    struct init_declarator* owner head;
    struct init_declarator* tail;    
};

struct init_declarator_list init_declarator_list(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers);

void init_declarator_list_destroy(struct init_declarator_list * obj_owner p);

struct declaration
{
    /*
      declaration:
        declaration-specifiers init-declarator-list opt ;
        attribute-specifier-sequence declaration-specifiers init-declarator-list ;
        static_assert-declaration
        attribute-declaration
    */
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt;
    
    struct static_assert_declaration* owner static_assert_declaration;
    

    struct declaration_specifiers* owner declaration_specifiers;
    struct init_declarator_list init_declarator_list;

    struct compound_statement* owner function_body;
    struct declarator*  contract_declarator;

    struct token* first_token;
    struct token* last_token;

    struct declaration* owner next;
};
void declaration_delete( struct declaration* owner p);
struct declaration*  owner external_declaration(struct parser_ctx* ctx);

struct atomic_type_specifier
{
    /*
      atomic-type-specifier:
        "_Atomic" ( type-name )
    */
    struct token* token;
    struct type_name* owner type_name;
};

struct atomic_type_specifier*  owner atomic_type_specifier(struct parser_ctx* ctx);
void atomic_type_specifier_delete(struct atomic_type_specifier*  owner p);

struct enumerator_list
{
    /*
     enumerator-list:
       enumerator
       enumerator-list , enumerator
    */
    struct enumerator* owner head;
    struct enumerator* tail;
};

struct enumerator_list enumerator_list(struct parser_ctx* ctx,
    struct enum_specifier*  p_enum_specifier
    );

void enumerator_list_destroy(struct enumerator_list*  obj_owner p_enum_specifier);
    
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
    struct attribute_specifier_sequence*  owner attribute_specifier_sequence_opt;
    struct specifier_qualifier_list*  owner specifier_qualifier_list;
    

    struct enumerator_list enumerator_list;
    
    struct token* tag_token;
    struct token* first_token;
    /*points to the complete enum (can be self pointed)*/
    struct enum_specifier* complete_enum_specifier;
};

struct enum_specifier*  owner enum_specifier(struct parser_ctx*);
void enum_specifier_delete(struct enum_specifier*  owner p);

struct member_declaration_list
{
    /*
     member-declaration-list:
       member-declaration
       member-declaration-list member-declaration
    */

    struct token* first_token; /*TODO ? necessary*/
    struct token* last_token;
    struct member_declaration* owner head;
    struct member_declaration* tail;
};

struct member_declaration_list member_declaration_list(struct parser_ctx* ctx, struct struct_or_union_specifier*);
void member_declaration_list_destroy(struct member_declaration_list * obj_owner);

struct member_declarator* find_member_declarator(struct member_declaration_list* list, const char* name, int* p_member_index);

struct struct_or_union_specifier
{
    /*
     struct-or-union-specifier:
       struct-or-union attribute-specifier-sequence opt identifier opt { member-declaration-list }
       struct-or-union attribute-specifier-sequence opt identifier
    */
    struct attribute_specifier_sequence* owner attribute_specifier_sequence_opt;
    struct member_declaration_list member_declaration_list;
    
    struct token* first_token;
    struct token* last_token;
        
    bool is_owner;

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

struct struct_or_union_specifier* owner struct_or_union_specifier(struct parser_ctx* ctx);
void struct_or_union_specifier_delete(struct struct_or_union_specifier* owner p);

bool struct_or_union_specifier_is_complete(struct struct_or_union_specifier* p_struct_or_union_specifier);
struct struct_or_union_specifier* view get_complete_struct_or_union_specifier(struct struct_or_union_specifier* p_struct_or_union_specifier);

struct init_declarator
{
    /*
     init-declarator:
        declarator
        declarator = initializer
    */

    struct declarator* owner p_declarator;
    struct initializer* owner initializer;
    struct init_declarator* owner next;    
};

void init_declarator_delete(struct init_declarator*  owner p);
struct init_declarator*  owner init_declarator(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers
    );

struct initializer
{
    /*
     initializer:
       assignment-expression
       braced-initializer
    */
    struct token* first_token;
    struct designation*  owner designation; /*auxiliar para list??*/
    struct braced_initializer*  owner braced_initializer;
    struct expression*  owner assignment_expression;
    struct initializer* owner next;
    
    /*
       cake extension
       int * p = [[cake::move]] p2;
    */
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt;
};

struct initializer* owner initializer(struct parser_ctx* ctx);
void initializer_delete(struct initializer* owner p);


enum object_state
{
    OBJECT_STATE_STRUCT = 0,
    OBJECT_STATE_UNINITIALIZED = 1 << 0,        
    OBJECT_STATE_NULL = 1 << 1,
    OBJECT_STATE_NOT_NULL = 1 << 2,
    OBJECT_STATE_MOVED = 1 << 3
};

void object_state_to_string(enum object_state e);

struct object_state_stack
{
    enum object_state * owner data;
    int size;
    int capacity;
};
void object_state_stack_destroy(struct object_state_stack * obj_owner p);

struct objects {
    struct object* owner data;
    int size;
    int capacity;
};
void objects_destroy(struct objects* obj_owner p);
/*
  Used in flow analysis to represent the object instance
*/
struct object
{
  /*
     state should not be used for struct, unless
     members_size is zero.
  */
  enum object_state state;    
  struct object * owner pointed;

  /*declarator is used only to print the error message*/
  struct declarator* declarator;

  struct objects members;      
  struct object_state_stack object_state_stack;
};
void object_destroy(struct object* obj_owner p);
void object_delete(struct object* owner p);

struct declarator
{
    /*
      declarator:
        pointer opt direct-declarator
    */

    struct token* first_token;
    struct token* last_token;


    struct pointer*  owner pointer;
    struct direct_declarator*  owner direct_declarator;

    
    struct declaration_specifiers* view declaration_specifiers;
    struct specifier_qualifier_list* view specifier_qualifier_list;

    struct token* name; //shortcut

    struct compound_statement* view function_body;
           
    int num_uses; /*used to show not used warnings*/
    
    /*user by flow analysis*/
    struct object object;

    /*final declarator type (after auto, typeof etc)*/
    struct type type;    
};

enum type_specifier_flags declarator_get_type_specifier_flags(const struct declarator* p);

struct declarator;
void print_declarator(struct osstream* ss, struct declarator* declarator, bool is_abstract);

struct declarator* owner declarator(struct parser_ctx* ctx,
    struct specifier_qualifier_list* specifier_qualifier_list,
    struct declaration_specifiers* declaration_specifiers,
    bool abstract_acceptable,
    struct token** pptokenname);

void declarator_delete(struct declarator* owner p);
struct array_declarator
{
    /*
     array-declarator:
        direct-declarator [ type-qualifier-list opt assignment-expression opt ]
        direct-declarator [ "static" type-qualifier-list opt assignment-expression ]
        direct-declarator [ type-qualifier-list "static" assignment-expression ]
        direct-declarator [ type-qualifier-listopt * ]
    */
    struct direct_declarator* owner direct_declarator;
    struct expression* owner assignment_expression;
    struct expression* owner expression;
    struct type_qualifier_list* owner type_qualifier_list_opt;
    
    struct token* token;
    struct token* static_token_opt;
};
void array_declarator_delete(struct array_declarator* owner p);
/*
  Return a value > 0 if it has constant size
*/
unsigned long long  array_declarator_get_size(struct array_declarator* p_array_declarator);

struct function_declarator
{
    /*
     function-declarator:
       direct-declarator ( parameter-type-list opt )
    */
    struct direct_declarator* owner direct_declarator;
    struct scope parameters_scope; //usado para escopo parametros
    struct parameter_type_list* owner parameter_type_list_opt;
};
void function_declarator_delete(struct function_declarator * owner p);

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
    struct declarator* owner declarator;
    struct array_declarator* owner array_declarator;
    struct function_declarator* owner function_declarator;
    struct attribute_specifier_sequence * owner p_attribute_specifier_sequence_opt;
};

void direct_declarator_delete(struct direct_declarator* owner p);

struct direct_declarator*  owner  direct_declarator(struct parser_ctx* ctx,
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
    struct parameter_list* owner parameter_list;
};

struct parameter_type_list*  owner parameter_type_list(struct parser_ctx* ctx);
void parameter_type_list_delete(struct parameter_type_list*  owner p);

struct pointer
{
    /*
     pointer:
        * attribute-specifier-sequence opt type-qualifier-list opt
        * attribute-specifier-sequence opt type-qualifier-list opt pointer    
    */
    struct attribute_specifier_sequence* owner attribute_specifier_sequence_opt;
    struct type_qualifier_list* owner type_qualifier_list_opt;

    struct pointer* owner pointer;
};

struct pointer*  owner pointer_opt(struct parser_ctx* ctx);
void pointer_delete(struct pointer*  owner p);
struct parameter_list
{
    /*
     parameter-list:
        parameter-declaration
        parameter-list , parameter-declaration
    */
    struct parameter_declaration* owner head;
    struct parameter_declaration* tail;
};
struct parameter_list*  owner  parameter_list(struct parser_ctx* ctx);
void parameter_list_delete(struct parameter_list*  owner  p);

struct parameter_declaration
{
    /*
     parameter-declaration:
      attribute-specifier-sequence opt declaration-specifiers declarator
      attribute-specifier-sequence opt declaration-specifiers abstract-declarator opt
    */
    struct attribute_specifier_sequence* owner attribute_specifier_sequence_opt;
    
    struct declaration_specifiers* owner declaration_specifiers;
    struct declarator* owner declarator;
    struct token * implicit_token;
    struct parameter_declaration* owner next;
};
struct parameter_declaration*  owner parameter_declaration(struct parser_ctx* ctx);
void parameter_declaration_delete(struct parameter_declaration*  owner p);


struct type_name
{
    /*
     type-name:
       specifier-qualifier-list abstract-declarator opt
    */

    struct token* first_token;
    struct token* last_token;
    struct specifier_qualifier_list* owner specifier_qualifier_list;
    struct declarator* owner declarator;
    struct type type;
};

struct type_name*  owner type_name(struct parser_ctx* ctx);
void type_name_delete(struct type_name*  owner p);
void print_type_name(struct osstream* ss, struct type_name* p);

struct argument_expression
{    
    struct expression* owner expression;
    struct argument_expression* owner next;
};

struct braced_initializer
{    
    /*
      { }
      { initializer-list }
      { initializer-list , }
    */

    struct token* first_token;
    struct initializer_list* owner initializer_list;
};
struct braced_initializer*  owner braced_initializer(struct parser_ctx* ctx);
void braced_initializer_delete(struct braced_initializer*  owner p);

struct type_specifier_qualifier
{
    /*
     type-specifier-qualifier:
       type-specifier
       type-qualifier
       alignment-specifier
    */

    struct type_specifier* owner type_specifier;
    struct type_qualifier* owner type_qualifier;
    struct alignment_specifier* owner alignment_specifier;

    struct type_specifier_qualifier* owner next;
};
struct type_specifier_qualifier*  owner type_specifier_qualifier(struct parser_ctx* ctx);
void type_specifier_qualifier_delete(struct type_specifier_qualifier*  owner p);


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
    struct struct_or_union_specifier* view struct_or_union_specifier;
    struct enum_specifier* view enum_specifier;
    struct typeof_specifier* view typeof_specifier;
    struct declarator* view typedef_declarator;
    
    struct type_specifier_qualifier* owner head;
    struct type_specifier_qualifier* tail;
    struct token* first_token;
    struct token* last_token;

    struct attribute_specifier_sequence * owner p_attribute_specifier_sequence;

};

struct specifier_qualifier_list* owner specifier_qualifier_list(struct parser_ctx* ctx);
void specifier_qualifier_list_delete(struct specifier_qualifier_list* owner p);

void print_specifier_qualifier_list(struct osstream* ss, bool* first, struct specifier_qualifier_list* p_specifier_qualifier_list);


struct alignment_specifier
{
    /*
     alignment-specifier:
       "alignas" ( type-name )
       "alignas" ( constant-expression )
    */
    struct type_name * owner type_name;
    struct expression * owner constant_expression;
    struct token* token;
};
struct alignment_specifier*  owner alignment_specifier(struct parser_ctx* ctx);
void alignment_specifier_delete(struct alignment_specifier*  owner p);


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

    /*
      Next is used when inside struct type_qualifier_list
      Not used when inside struct type_specifier_qualifier    
    */
    struct type_qualifier* owner next;
};

struct type_qualifier*  owner type_qualifier(struct parser_ctx* ctx);

struct member_declaration
{
    /*
     member-declaration:
       attribute-specifier-sequence opt specifier-qualifier-list member-declarator-list opt ;
       static_assert-declaration
    */
    struct specifier_qualifier_list* owner specifier_qualifier_list;
    struct member_declarator_list* owner member_declarator_list_opt;

    struct static_assert_declaration* owner static_assert_declaration;
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt;
    struct member_declaration* owner next;
    
};

struct member_declaration*  owner member_declaration(struct parser_ctx* ctx, struct struct_or_union_specifier*);
void member_declaration_delete(struct member_declaration*  owner p);

struct member_declarator
{
    /*    
     member-declarator:
       declarator
       declarator opt : constant-expression
    */
    
    struct declarator* owner declarator;
    struct expression* owner constant_expression;
    struct member_declarator* owner next;
};
void member_declarator_delete(struct member_declarator * owner p);

struct member_declarator_list
{
    /*    
     member-declarator-list:
        member-declarator
        member-declarator-list , member-declarator
    */

    struct member_declarator* owner head;
    struct member_declarator* tail;
};

struct member_declarator_list*  owner member_declarator_list(struct parser_ctx* ctx,
    struct struct_or_union_specifier* ,
    struct specifier_qualifier_list* specifier_qualifier_list
    );
void member_declarator_list_delete(struct member_declarator_list*  owner p);

struct block_item_list
{
    /*
     block-item-list:
       block-item
       block-item-list block-item
    */
    struct block_item* owner head;
    struct block_item* tail;
};

struct block_item_list block_item_list(struct parser_ctx* ctx);
void block_item_list_destroy( struct block_item_list * obj_owner p);

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
struct compound_statement*  owner compound_statement(struct parser_ctx* ctx);
void compound_statement_delete( struct compound_statement*  owner p);

struct defer_statement
{
    /*
     defer-statement: (extension)
       "defer" secondary-block     
    */
    struct token* first_token;
    struct token* last_token;
    struct secondary_block* owner secondary_block;
};

void defer_statement_delete(struct defer_statement * owner p);

struct try_statement
{   
   /*
     try-statement: (extension)
      "try" secondary-block
      "try" secondary-block "catch" secondary-block
   */
    struct secondary_block* owner secondary_block;
    struct secondary_block* owner catch_secondary_block_opt;
    struct token* first_token; /*try*/
    struct token* last_token;
    struct token*  catch_token_opt; /*catch*/
    /*Used to generate label names*/
    int try_catch_block_index;
};

struct try_statement*  owner try_statement(struct parser_ctx* ctx);
void try_statement_delete(struct try_statement*  owner p);

struct selection_statement
{
    /*
      selection-statement:
        "if" ( expression ) secondary-block
        "if" ( expression ) secondary-block "else" secondary-block
        "switch" ( expression ) secondary-block
    */

    /*C++ 17 if with initialization extension*/
    struct init_declarator* owner init_declarator;
    struct declaration_specifiers* owner declaration_specifiers;

    struct expression* owner expression;
    struct secondary_block* owner secondary_block;
    struct secondary_block* owner else_secondary_block_opt;

    struct token* first_token;
    struct token* last_token;
    struct token* else_token_opt;    
};

struct selection_statement*  owner selection_statement(struct parser_ctx* ctx);
void selection_statement_delete(struct selection_statement* owner p);
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

    struct secondary_block* owner secondary_block;
    struct expression* owner expression1;
    struct expression* owner expression2;
    struct expression* owner expression0;
    struct declaration * owner declaration;
};

struct iteration_statement*  owner iteration_statement(struct parser_ctx* ctx);
void iteration_statement_delete(struct iteration_statement*owner p);

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
    struct expression* owner expression_opt;
    
    int try_catch_block_index;
};

struct jump_statement*  owner jump_statement(struct parser_ctx* ctx);
void jump_statement_delete(struct jump_statement*  owner p);

struct expression_statement
{
    /*
     expression-statement:
       expression opt;
       attribute-specifier-sequence expression ;
    */
    
    struct attribute_specifier_sequence* owner p_attribute_specifier_sequence_opt;
    struct expression* owner expression_opt;
};

struct expression_statement*  owner expression_statement(struct parser_ctx* ctx);
void expression_statement_delete(struct expression_statement*  owner p);

struct block_item
{
    /*
     block-item:
      declaration
      unlabeled-statement
      label
    */
    struct token* first_token; //?necessary
    struct declaration* owner declaration;    
    struct unlabeled_statement* owner unlabeled_statement;
    struct label* owner label;

    struct block_item* owner next;
};

struct block_item*  owner block_item(struct parser_ctx* ctx);
void block_item_delete( struct block_item*  owner p);

struct compound_statement*  owner function_body(struct parser_ctx* ctx);

struct designator
{
    /*
     designator:
       [ constant-expression ]
       . identifier
    */
    struct expression* owner constant_expression_opt;
    struct token* token;
    struct designator* owner next;
};
struct designator*  owner designator(struct parser_ctx* ctx);
void designator_delete(struct designator*  owner p);
struct initializer_list
{
    /*
      initializer-list:
        designation opt initializer
        initializer-list , designation opt initializer
    */
    struct token* first_token;
    struct initializer* owner head;
    struct initializer* tail;
    int size;
};
struct initializer_list*  owner initializer_list(struct parser_ctx* ctx);
void initializer_list_delete(struct initializer_list*  owner p);

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
    
    struct compound_statement* owner compound_statement;
    struct selection_statement* owner selection_statement;
    struct iteration_statement* owner iteration_statement;
    struct defer_statement* owner defer_statement;
    struct try_statement* owner try_statement;
};

void primary_block_delete(struct primary_block * owner p);

struct secondary_block
{
    /*
     secondary-block:
       statement
    */
    struct token* view first_token;
    struct token* view last_token;
    struct statement* owner statement;
};

void secondary_block_delete(struct secondary_block * owner p);

struct unlabeled_statement
{
    /*
      unlabeled-statement:
        expression-statement
        attribute-specifier-sequence opt primary-block
        attribute-specifier-sequence opt jump-statement
     */

    struct expression_statement*  owner expression_statement;
    struct primary_block*  owner primary_block;
    struct jump_statement*  owner jump_statement;
};

struct unlabeled_statement* owner unlabeled_statement(struct parser_ctx* ctx);
void unlabeled_statement_delete( struct unlabeled_statement* owner p);

struct labeled_statement
{
    /*
     label statement:
       label statement
    */
    struct label* owner label;
    struct statement* owner statement;
};
struct labeled_statement* owner labeled_statement(struct parser_ctx* ctx);
void labeled_statement_delete(struct labeled_statement * owner p);

struct statement
{    
    /*
     statement:
       labeled-statement
       unlabeled-statemen
    */
    struct labeled_statement* owner labeled_statement;
    struct unlabeled_statement* owner unlabeled_statement;
};
struct statement* owner statement(struct parser_ctx* ctx);
void statement_delete(struct statement* owner p);

struct designator_list
{
    /*
     designator-list:
       designator
       designator-list designator
    */
    struct designator* owner head;
    struct designator* tail;
};

struct designator_list* owner designator_list(struct parser_ctx* ctx);
void designator_list_delete(struct designator_list* owner p);

struct designation
{
    /*
     designation:
       designator-list =
    */
    struct designator_list * owner designator_list;
    struct token* token;
};
struct designation* owner designation(struct parser_ctx* ctx);
void designation_delete(struct designation* owner p);

struct type_qualifier_list
{
    /*
     type-qualifier-list:
       type-qualifier
       type-qualifier-list type-qualifier
    */
    enum type_qualifier_flags flags;
    struct type_qualifier* owner head;
    struct type_qualifier* tail;
};

struct type_qualifier_list* owner type_qualifier_list(struct parser_ctx* ctx);
void type_qualifier_list_delete(struct type_qualifier_list* owner p);

struct attribute_token
{
    enum attribute_flags attributes_flags;
    struct token* token;
};
struct attribute_token* owner attribute_token(struct parser_ctx* ctx);
void attribute_token_delete(struct attribute_token* owner p);
struct attribute
{
    enum attribute_flags  attributes_flags;
    struct attribute_token* owner attribute_token;
    struct attribute_argument_clause* owner attribute_argument_clause;
    struct attribute* owner next;
};
void attribute_delete(struct attribute * owner p);

struct attribute_list
{
    enum attribute_flags  attributes_flags;
    struct attribute* owner head;
    struct attribute* tail;
};
struct attribute_list* owner attribute_list(struct parser_ctx* ctx);
void attribute_list_destroy( struct attribute_list* obj_owner p);

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
    struct attribute_specifier_sequence* owner attribute_specifier_sequence_opt;

    struct expression* owner constant_expression_opt;
    
    /*
      having the enum specifier we have better information about the type
    */
    struct enum_specifier* view enum_specifier;

    struct enumerator* owner next;
    long long value;    
};

struct enumerator* owner enumerator(struct parser_ctx* ctx, struct enum_specifier* p_enum_specifier, long long *p_enumerator_value);
void enumerator_delete(struct enumerator* owner p);

struct attribute_argument_clause
{
    /*
     attribute-argument-clause:
       ( balanced-token-sequence opt )
    */
    struct balanced_token_sequence * owner p_balanced_token_sequence;
    struct token* token;
};

struct attribute_argument_clause* owner attribute_argument_clause(struct parser_ctx* ctx);
void attribute_argument_clause_delete(struct attribute_argument_clause* owner p);

bool first_of_attribute(struct parser_ctx* ctx);

struct balanced_token
{
    struct token * token;
    struct balanced_token* owner next;
};

struct balanced_token_sequence
{
    struct balanced_token* owner head;
    struct balanced_token* tail;
};
struct balanced_token_sequence* owner balanced_token_sequence_opt(struct parser_ctx* ctx);
void balanced_token_sequence_delete(struct balanced_token_sequence* owner p);

bool is_first_of_conditional_expression(struct parser_ctx* ctx);
bool first_of_type_name(struct parser_ctx* ctx);
bool first_of_type_name_ahead(struct parser_ctx* ctx);

struct argument_expression_list argument_expression_list(struct parser_ctx* ctx);

struct declaration_list
{
    struct declaration* owner head;
    struct declaration* tail;
};

struct declaration_list translation_unit(struct parser_ctx* ctx);
void declaration_list_destroy( struct declaration_list* obj_owner list);

struct label
{
    /*
     label:
       attribute-specifier-sequence opt identifier :
       attribute-specifier-sequence opt "case" constant-expression :
       attribute-specifier-sequence opt "default" :
    */
    struct expression* owner constant_expression;
    struct token* name;
};

struct label* owner label(struct parser_ctx* ctx);
void label_delete( struct label* owner p);

struct ast
{
    struct token_list token_list;
    struct declaration_list declaration_list;
};


struct ast get_ast(struct options* options, const char* filename, const char* source, struct report* report);
void ast_destroy( struct ast* obj_owner ast);
struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);


struct declaration_list parse(struct parser_ctx* ctx, struct token_list* list);
