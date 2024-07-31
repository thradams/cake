/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "ownership.h"

#include <stdio.h>
#include <errno.h>
#include "hashmap.h"
#include "tokenizer.h"
#include "expressions.h"
#include <stdbool.h>
#include "osstream.h"
#include "type.h"
#include "options.h"
#include "flow_object.h"


#if __STDC_VERSION__  >= 202311L 
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif
struct scope
{
    int scope_level;
    struct hash_map tags;
    struct hash_map variables;

    struct scope* _Opt next;
    struct scope* _Opt previous;
};

void scope_destroy(struct scope* _Obj_owner p);

struct scope_list
{
    struct scope* _Opt head;
    struct scope* _Opt tail;
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

    bool test_mode;
    int test_failed;
    int test_succeeded;

    /*
     Warnings are removed (-test-mode) on demand at next line,
     but we can have more than one warning at same line then we need this array
    */
    enum diagnostic_id last_diagnostics_ids[2];

    //this error is expected in test-mode
    //it is used when the error aborts parsing
    enum diagnostic_id fatal_error_expected;

    /*
      direct commands like -autoconfig doesnt use report
    */
    bool ignore_this_report;
};

struct switch_value
{
    long long value;
    struct label* _Opt p_label;
    struct switch_value* _Owner _Opt next;
};

struct  switch_value_list
{
    struct switch_value* _Owner _Opt head;
    struct switch_value* _Opt tail;
    struct switch_value* _Owner _Opt p_default;
};

void switch_value_destroy(struct switch_value_list* _Obj_owner list);
void switch_value_list_push(struct switch_value_list* list, struct switch_value* _Owner pnew);
struct switch_value* switch_value_list_find(struct switch_value_list* list, long long value);

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
    struct declaration* _Opt p_current_function_opt;

    /*
    * Points to the try-block we're in. Or null.
    */
    const struct try_statement* _Opt p_current_try_statement_opt;

    /*
    * Points to the selection_statement we're in. Or null.
    */
    const struct selection_statement* _Opt p_current_selection_statement;

    struct  switch_value_list* _Opt p_switch_value_list;

    FILE* _Owner _Opt sarif_file;
    unsigned int sarif_entries;

    _View struct token_list input_list;
    struct token* _Opt current;
    struct token* _Opt previous;
    int try_catch_block_index;

    /*
       Expression inside sizeof etc.. are not evaluated
    */
    bool evaluation_is_disabled;

    bool inside_generic_association;


    struct report* p_report;

};

///////////////////////////////////////////////////////

void parser_ctx_destroy(struct parser_ctx* _Obj_owner ctx);


struct token* _Opt parser_look_ahead(const struct parser_ctx* ctx);

void parser_match(struct parser_ctx* ctx);
NODISCARD
int parser_match_tk(struct parser_ctx* ctx, enum token_type type);

struct token* _Opt previous_parser_token(const struct token* token);
struct declarator* _Opt find_declarator(const struct parser_ctx* ctx, const char* lexeme, struct scope** _Opt ppscope_opt);
struct enumerator* _Opt find_enumerator(const struct parser_ctx* ctx, const char* lexeme, struct scope** _Opt ppscope_opt);
struct map_entry* _Opt find_variables(const struct parser_ctx* ctx, const char* lexeme, struct scope** _Opt ppscope_opt);

struct struct_or_union_specifier* _Opt find_struct_or_union_specifier(struct parser_ctx* ctx, const char* lexeme);

void print_scope(struct scope_list* e);

char* _Opt _Owner CompileText(const char* options, const char* content);



_Bool compiler_diagnostic_message(enum diagnostic_id w,
    const struct parser_ctx* ctx,
    const struct token* _Opt p_token,
    const struct marker* _Opt p_marker,
    const char* fmt, ...);


int compile(int argc, const char** argv, struct report* error);


void print_type_qualifier_flags(struct osstream* ss, bool* first, enum type_qualifier_flags e_type_qualifier_flags);

enum token_type parse_number(const char* lexeme, enum type_specifier_flags* _Opt flags_opt);
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
    struct storage_class_specifier* _Owner _Opt storage_class_specifier;

    struct type_specifier_qualifier* _Owner _Opt type_specifier_qualifier;

    struct function_specifier* _Owner _Opt function_specifier;

    struct declaration_specifier* _Owner _Opt next;
};

struct declaration_specifier* _Owner declaration_specifier(struct parser_ctx* ctx);
void declaration_specifier_delete(struct declaration_specifier* _Owner _Opt p);

struct declaration_specifiers
{
    /*
     declaration-specifiers:
       declaration-specifier attribute-specifier-sequence _Opt
       declaration-specifier declaration-specifiers
    */

    /*cumulative flags*/
    enum attribute_flags  attributes_flags;
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
    enum storage_class_specifier_flags storage_class_specifier_flags;

    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt;

    /*shortcuts*/
    struct struct_or_union_specifier* _Opt struct_or_union_specifier;
    struct enum_specifier* _Opt enum_specifier;
    struct declarator* _Opt typedef_declarator;
    struct typeof_specifier* _Opt typeof_specifier;

    struct token* first_token; /*not _Owner*/
    struct token* last_token; /*not _Owner*/

    struct declaration_specifier* _Owner _Opt head;
    struct declaration_specifier* _Opt tail;
};

void print_declaration_specifiers(struct osstream* ss, struct declaration_specifiers* p);
struct declaration_specifiers* _Owner declaration_specifiers(struct parser_ctx* ctx, enum storage_class_specifier_flags default_storage_flag);
void declaration_specifiers_delete(struct declaration_specifiers* _Owner _Opt p);
void declaration_specifiers_add(struct declaration_specifiers* p, struct declaration_specifier* _Owner item);

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
      "static_set" ( constant-expression , string-literal) ;
    */

    struct token* first_token;
    struct token* last_token;
    struct expression* _Owner constant_expression;
    struct token* _Opt string_literal_opt;
};
struct static_assert_declaration* _Owner static_assert_declaration(struct parser_ctx* ctx);
void static_assert_declaration_delete(struct static_assert_declaration* _Owner _Opt p);

/*
  extension, pragma survives the preprocessor and become
  a pragma_declaration that can be used on the AST visit
*/

struct pragma_declaration {
    struct token* first_token;
    struct token* last_token;
};

struct pragma_declaration* _Owner pragma_declaration(struct parser_ctx* ctx);
void pragma_declaration_delete(struct pragma_declaration* _Owner _Opt p);

void execute_pragma(struct parser_ctx* ctx, struct pragma_declaration* p_pragma, bool on_flow_analysis);

struct attribute_specifier_sequence
{
    /*
     attribute-specifier-sequence:
       attribute-specifier-sequence _Opt attribute-specifier
    */

    struct token* first_token;
    struct token* last_token;
    enum attribute_flags  attributes_flags;
    struct attribute_specifier* _Owner _Opt head;
    struct attribute_specifier* _Opt tail;
};
struct attribute_specifier_sequence* _Owner attribute_specifier_sequence_opt(struct parser_ctx* ctx);
void attribute_specifier_sequence_delete(struct attribute_specifier_sequence* _Owner _Opt p);
void attribute_specifier_sequence_add(struct attribute_specifier_sequence* list, struct attribute_specifier* _Owner p_item);

struct attribute_specifier
{
    /*
     attribute-specifier:
        [ [ attribute-list ] ]
    */
    struct token* first_token;
    struct token* last_token;
    struct attribute_list* _Owner attribute_list;
    struct attribute_specifier* _Owner _Opt  next;
};

struct attribute_specifier* _Owner _Opt attribute_specifier(struct parser_ctx* ctx);
void attribute_specifier_delete(struct attribute_specifier* _Owner _Opt p);

struct attribute* _Owner _Opt attribute(struct parser_ctx* ctx);


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

struct storage_class_specifier* _Owner _Opt storage_class_specifier(struct parser_ctx* ctx);
void storage_class_specifier_delete(struct storage_class_specifier* _Owner _Opt p);

struct function_specifier
{
    /*
     function-specifier:
       inline
       _Noreturn
    */
    struct token* token;
};
struct function_specifier* _Owner _Opt  function_specifier(struct parser_ctx* ctx);
void function_specifier_delete(struct function_specifier* _Owner _Opt  p);

struct typeof_specifier_argument
{
    /*
     typeof-specifier-argument:
       expression
       type-name
    */
    struct expression* _Owner _Opt expression;
    struct type_name* _Owner _Opt type_name;
};

void typeof_specifier_argument_delete(struct typeof_specifier_argument* _Owner _Opt p);

struct typeof_specifier
{
    /*
     typeof-specifier:
       "typeof" ( typeof-specifier-argument )
    */
    struct token* first_token;
    struct token* last_token;
    struct typeof_specifier_argument* _Owner typeof_specifier_argument;
    struct type type;
};
void typeof_specifier_delete(struct typeof_specifier* _Owner _Opt p);

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
    struct struct_or_union_specifier* _Owner _Opt struct_or_union_specifier;
    struct typeof_specifier* _Owner _Opt  typeof_specifier;
    struct enum_specifier* _Owner _Opt enum_specifier;
    struct declarator* _Opt typedef_declarator;
    struct atomic_type_specifier* _Owner _Opt  atomic_type_specifier;
};

struct type_specifier* _Owner _Opt type_specifier(struct parser_ctx* ctx);
void type_specifier_delete(struct type_specifier* _Owner _Opt p);

struct init_declarator_list
{
    /*
     init-declarator-list:
       init-declarator
       init-declarator-list , init-declarator
    */
    struct init_declarator* _Owner _Opt head;
    struct init_declarator* _Opt tail;
};

struct init_declarator_list init_declarator_list(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers);

void init_declarator_list_destroy(struct init_declarator_list* _Obj_owner p);
void init_declarator_list_add(struct init_declarator_list* list, struct init_declarator* _Owner p_item);

struct declaration
{
    /*
      declaration:
        declaration-specifiers init-declarator-list _Opt ;
        attribute-specifier-sequence declaration-specifiers init-declarator-list ;
        static_assert-declaration
        attribute-declaration
    */
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt;

    struct static_assert_declaration* _Owner _Opt static_assert_declaration;
    struct pragma_declaration* _Owner _Opt pragma_declaration;


    struct declaration_specifiers* _Owner _Opt  declaration_specifiers;
    struct init_declarator_list init_declarator_list;

    struct compound_statement* _Owner _Opt  function_body;
    struct declarator* _Opt  contract_declarator;

    struct token* first_token;
    struct token* last_token;

    struct declaration* _Owner _Opt next;
};
void declaration_delete(struct declaration* _Owner _Opt p);
struct declaration* _Owner _Opt external_declaration(struct parser_ctx* ctx);

struct simple_declaration
{
    /*
    This is an extension to support C++ 17 if with initialization

    simple-declaration:
      declaration-specifiers init-declarator-list _Opt ;
      attribute-specifier-sequence declaration-specifiers init-declarator-list ;
    */
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt;
    struct declaration_specifiers* _Owner p_declaration_specifiers;
    struct init_declarator_list init_declarator_list;
    struct token* first_token;
    struct token* last_token;
};

void simple_declaration_delete(struct simple_declaration* _Owner _Opt p);

struct simple_declaration* _Owner _Opt simple_declaration(struct parser_ctx* ctx,
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt,
    bool ignore_semicolon);

struct condition {
    /*
      This is an extension to support C++ 17 if with initialization

      condition :
       expression
       attribute-specifier-seq _Opt decl-specifier-seq declarator initializer
    */
    struct expression* _Owner _Opt expression;
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt;
    struct declaration_specifiers* _Owner _Opt p_declaration_specifiers;

    /*
      OBS:
      We must use p_init_declarator because it is kept on the scope
      as init_declarator when we are trying to parse init-statement or condition that
      are very similar
    */
    struct init_declarator* _Owner p_init_declarator;

    struct token* first_token;
    struct token* last_token;
};

void condition_delete(struct condition* _Owner _Opt p);
struct condition* _Owner _Opt condition(struct parser_ctx* ctx);

struct init_statement
{
    /*
        This is an extension to support C++ 17 if with initialization

        init-statement :
          expression-statement
          simple-declaration
    */
    struct expression_statement* _Owner _Opt p_expression_statement;
    struct simple_declaration* _Owner _Opt p_simple_declaration;
};

void init_statement_delete(struct init_statement* _Owner _Opt p);
struct init_statement* _Owner init_statement(struct parser_ctx* ctx, bool ignore_semicolon);

struct atomic_type_specifier
{
    /*
      atomic-type-specifier:
        "_Atomic" ( type-name )
    */
    struct token* token;
    struct type_name* _Owner type_name;
};

struct atomic_type_specifier* _Owner _Opt atomic_type_specifier(struct parser_ctx* ctx);
void atomic_type_specifier_delete(struct atomic_type_specifier* _Owner _Opt  p);

struct enumerator_list
{
    /*
     enumerator-list:
       enumerator
       enumerator-list , enumerator
    */
    struct enumerator* _Owner _Opt head;
    struct enumerator* _Opt tail;
};

struct enumerator_list enumerator_list(struct parser_ctx* ctx,
    const struct enum_specifier* p_enum_specifier
);

void enumerator_list_destroy(struct enumerator_list* _Obj_owner p_enum_specifier);
void enumerator_list_add(struct enumerator_list* list, struct enumerator* _Owner p_item);

struct enum_specifier
{
    /*
     enum-type-specifier:
       : specifier-qualifier-lis

     enum-specifier:
       "enum" attribute-specifier-sequence _Opt identifier _Opt enum-type-specifier _Opt  { enumerator-list }
       "enum" attribute-specifier-sequence _Opt identifier _Opt enum-type-specifier _Opt  { enumerator-list , }
       "enum" identifier enum-type-specifier _Opt
    */
    struct attribute_specifier_sequence* _Owner _Opt attribute_specifier_sequence_opt;
    struct specifier_qualifier_list* _Owner _Opt specifier_qualifier_list;

    char tag_name[200];

    struct enumerator_list enumerator_list;

    struct token* _Opt tag_token;
    struct token* first_token;
    /*points to the complete enum (can be self pointed)*/
    struct enum_specifier* _Opt complete_enum_specifier2;
};

struct enum_specifier* _Owner _Opt enum_specifier(struct parser_ctx*);
void enum_specifier_delete(struct enum_specifier* _Owner _Opt p);
const struct enum_specifier* _Opt get_complete_enum_specifier(const struct enum_specifier* p_enum_specifier);

const struct enumerator* _Opt find_enumerator_by_value(const struct enum_specifier* p_enum_specifier, long long value);



struct member_declaration_list
{
    /*
     member-declaration-list:
       member-declaration
       member-declaration-list member-declaration
    */

    struct token* first_token; /*TODO ? necessary*/
    struct token* last_token;
    struct member_declaration* _Owner _Opt head;
    struct member_declaration* _Opt tail;
};

struct member_declaration_list member_declaration_list(struct parser_ctx* ctx, struct struct_or_union_specifier*);
void member_declaration_list_destroy(struct member_declaration_list* _Obj_owner p);
void member_declaration_list_add(struct member_declaration_list* list, struct member_declaration* _Owner p_item);

struct member_declarator* _Opt find_member_declarator(struct member_declaration_list* list, const char* name, int* p_member_index);

struct struct_or_union_specifier
{
    /*
     struct-or-union-specifier:
       struct-or-union attribute-specifier-sequence _Opt identifier _Opt { member-declaration-list }
       struct-or-union attribute-specifier-sequence _Opt identifier
    */
    struct attribute_specifier_sequence* _Owner _Opt attribute_specifier_sequence_opt;
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
    struct struct_or_union_specifier* _Opt complete_struct_or_union_specifier_indirection;
};

struct struct_or_union_specifier* _Owner _Opt struct_or_union_specifier(struct parser_ctx* ctx);
void struct_or_union_specifier_delete(struct struct_or_union_specifier* _Owner _Opt  p);

bool struct_or_union_specifier_is_complete(struct struct_or_union_specifier* p_struct_or_union_specifier);
struct struct_or_union_specifier* _Opt get_complete_struct_or_union_specifier(struct struct_or_union_specifier* p_struct_or_union_specifier);

struct init_declarator
{
    /*
     init-declarator:
        declarator
        declarator = initializer
    */

    struct declarator* _Owner p_declarator;
    struct initializer* _Owner _Opt initializer;
    struct init_declarator* _Owner _Opt next;
};

void init_declarator_delete(struct init_declarator* _Owner _Opt p);
struct init_declarator* _Owner _Opt init_declarator(struct parser_ctx* ctx,
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
    struct designation* _Owner designation; /*auxiliar para list??*/
    struct braced_initializer* _Owner _Opt braced_initializer;
    struct expression* _Owner _Opt assignment_expression;
    struct initializer* _Owner _Opt  next;

    /*
       cake extension
       int * p = [[cake::move]] p2;
    */
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt;
};

struct initializer* _Owner _Opt initializer(struct parser_ctx* ctx);
void initializer_delete(struct initializer* _Owner _Opt  p);



struct declarator
{
    /*
      declarator:
        pointer _Opt direct-declarator
    */

    struct token* first_token;
    struct token* last_token;


    struct pointer* _Owner _Opt pointer;
    struct direct_declarator* _Owner _Opt direct_declarator;


    struct declaration_specifiers* _Opt declaration_specifiers;
    const struct specifier_qualifier_list* _Opt specifier_qualifier_list;

    struct token* _Opt name_opt; //shortcut , null for abstract declarator

    struct compound_statement* _Opt function_body;

    int num_uses; /*used to show not used warnings*/

    /*user by flow analysis*/
    struct flow_object* _Opt p_object;

    /*final declarator type (after auto, typeof etc)*/
    struct type type;
};

enum type_specifier_flags declarator_get_type_specifier_flags(const struct declarator* p);

struct declarator;
void print_declarator(struct osstream* ss, struct declarator* declarator, bool is_abstract);

struct declarator* _Owner _Opt declarator(struct parser_ctx* ctx,
    const struct specifier_qualifier_list* specifier_qualifier_list,
    struct declaration_specifiers* declaration_specifiers,
    bool abstract_acceptable,
    struct token** pptokenname);

void declarator_delete(struct declarator* _Owner _Opt  p);
struct array_declarator
{
    /*
     array-declarator:
        direct-declarator [ type-qualifier-list _Opt assignment-expression _Opt ]
        direct-declarator [ "static" type-qualifier-list _Opt assignment-expression ]
        direct-declarator [ type-qualifier-list "static" assignment-expression ]
        direct-declarator [ type-qualifier-listopt * ]
    */
    struct direct_declarator* _Owner _Opt direct_declarator;
    struct expression* _Owner _Opt  assignment_expression;
    struct expression* _Owner _Opt expression;
    struct type_qualifier_list* _Owner _Opt type_qualifier_list_opt;

    struct token* token;
    struct token* _Opt static_token_opt;
};
void array_declarator_delete(struct array_declarator* _Owner _Opt p);
/*
  Return a value > 0 if it has constant size
*/
unsigned long long  array_declarator_get_size(const struct array_declarator* p_array_declarator);

struct function_declarator
{
    /*
     function-declarator:
       direct-declarator ( parameter-type-list _Opt )
    */
    struct direct_declarator* _Owner _Opt direct_declarator;
    struct scope parameters_scope; //usado para escopo parametros
    struct parameter_type_list* _Owner _Opt parameter_type_list_opt;
};
void function_declarator_delete(struct function_declarator* _Owner _Opt p);

struct direct_declarator
{
    /*
     direct-declarator:
        identifier attribute-specifier-sequence _Opt
        ( declarator )
        array-declarator attribute-specifier-sequence _Opt
        function-declarator attribute-specifier-sequence _Opt
    */
    struct token* _Opt name_opt;
    struct declarator* _Owner _Opt declarator;
    struct array_declarator* _Owner _Opt array_declarator;
    struct function_declarator* _Owner _Opt function_declarator;
    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt;
};

void direct_declarator_delete(struct direct_declarator* _Owner _Opt p);

struct direct_declarator* _Owner _Opt direct_declarator(struct parser_ctx* ctx,
    const struct specifier_qualifier_list* specifier_qualifier_list,
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
    struct parameter_list* _Owner _Opt parameter_list;
};

struct parameter_type_list* _Owner _Opt parameter_type_list(struct parser_ctx* ctx);
void parameter_type_list_delete(struct parameter_type_list* _Owner _Opt  p);

struct pointer
{
    /*
     pointer:
        * attribute-specifier-sequence _Opt type-qualifier-list _Opt
        * attribute-specifier-sequence _Opt type-qualifier-list _Opt pointer
    */
    struct attribute_specifier_sequence* _Owner _Opt  attribute_specifier_sequence_opt;
    struct type_qualifier_list* _Owner _Opt type_qualifier_list_opt;

    struct pointer* _Owner _Opt pointer;
};

struct pointer* _Owner _Opt  pointer_opt(struct parser_ctx* ctx);
void pointer_delete(struct pointer* _Owner _Opt p);
struct parameter_list
{
    /*
     parameter-list:
        parameter-declaration
        parameter-list , parameter-declaration
    */
    struct parameter_declaration* _Owner _Opt head;
    struct parameter_declaration* _Opt tail;
};
struct parameter_list* _Owner _Opt  parameter_list(struct parser_ctx* ctx);
void parameter_list_delete(struct parameter_list* _Owner _Opt  p);
void parameter_list_add(struct parameter_list* list, struct parameter_declaration* _Owner p_item);

struct parameter_declaration
{
    /*
     parameter-declaration:
      attribute-specifier-sequence _Opt declaration-specifiers declarator
      attribute-specifier-sequence _Opt declaration-specifiers abstract-declarator _Opt
    */
    struct attribute_specifier_sequence* _Owner _Opt  attribute_specifier_sequence_opt;

    struct declaration_specifiers* _Owner declaration_specifiers;
    struct declarator* _Owner _Opt  declarator;
    struct parameter_declaration* _Owner _Opt next;
};
struct parameter_declaration* _Owner _Opt parameter_declaration(struct parser_ctx* ctx);
void parameter_declaration_delete(struct parameter_declaration* _Owner _Opt  p);


struct type_name
{
    /*
     type-name:
       specifier-qualifier-list abstract-declarator _Opt
    */

    struct token* first_token;
    struct token* last_token;
    struct specifier_qualifier_list* _Owner specifier_qualifier_list;
    struct declarator* _Owner declarator;
    struct type type;
};

struct type_name* _Owner _Opt type_name(struct parser_ctx* ctx);
void type_name_delete(struct type_name* _Owner _Opt p);
void print_type_name(struct osstream* ss, struct type_name* p);

struct argument_expression
{
    struct expression* _Owner expression;
    struct argument_expression* _Owner _Opt next;
    bool set_unkown; //used in flow analysis need to be removed..
};

void argument_expression_delete(struct argument_expression* _Owner _Opt  p);

struct braced_initializer
{
    /*
      { }
      { initializer-list }
      { initializer-list , }
    */

    struct token* first_token;
    struct token* last_token;
    struct initializer_list* _Owner _Opt initializer_list;
};
struct braced_initializer* _Owner _Opt braced_initializer(struct parser_ctx* ctx);
void braced_initializer_delete(struct braced_initializer* _Owner _Opt p);

struct type_specifier_qualifier
{
    /*
     type-specifier-qualifier:
       type-specifier
       type-qualifier
       alignment-specifier
    */

    struct type_specifier* _Owner _Opt type_specifier;
    struct type_qualifier* _Owner _Opt type_qualifier;
    struct alignment_specifier* _Owner _Opt alignment_specifier;

    struct type_specifier_qualifier* _Owner _Opt next;
};
struct type_specifier_qualifier* _Owner _Opt type_specifier_qualifier(struct parser_ctx* ctx);
void type_specifier_qualifier_delete(struct type_specifier_qualifier* _Owner _Opt  p);


struct specifier_qualifier_list
{
    /*
      specifier-qualifier-list:
        type-specifier-qualifier attribute-specifier-sequence _Opt
        type-specifier-qualifier specifier-qualifier-list
    */

    /*cumulative flags*/
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;

    /*shortcuts*/
    struct struct_or_union_specifier* _Opt struct_or_union_specifier;
    struct enum_specifier* _Opt enum_specifier;
    struct typeof_specifier* _Opt typeof_specifier;
    struct declarator* _Opt typedef_declarator;

    struct type_specifier_qualifier* _Owner _Opt head;
    struct type_specifier_qualifier* _Opt tail;
    struct token* first_token;
    struct token* last_token;

    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence;

};

struct specifier_qualifier_list* _Owner _Opt specifier_qualifier_list(struct parser_ctx* ctx);
void specifier_qualifier_list_delete(struct specifier_qualifier_list* _Owner _Opt p);
void specifier_qualifier_list_add(struct specifier_qualifier_list* list, struct type_specifier_qualifier* _Owner p_item);


void print_specifier_qualifier_list(struct osstream* ss, bool* first, struct specifier_qualifier_list* p_specifier_qualifier_list);


struct alignment_specifier
{
    /*
     alignment-specifier:
       "alignas" ( type-name )
       "alignas" ( constant-expression )
    */
    struct type_name* _Owner _Opt type_name;
    struct expression* _Owner _Opt constant_expression;
    struct token* token;
};
struct alignment_specifier* _Owner _Opt alignment_specifier(struct parser_ctx* ctx);
void alignment_specifier_delete(struct alignment_specifier* _Owner _Opt p);


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
    struct type_qualifier* _Owner _Opt next;
};

struct type_qualifier* _Owner _Opt type_qualifier(struct parser_ctx* ctx);
void type_qualifier_delete(struct type_qualifier* _Owner _Opt p);

struct member_declaration
{
    /*
     member-declaration:
       attribute-specifier-sequence _Opt specifier-qualifier-list member-declarator-list _Opt ;
       static_assert-declaration
    */
    struct specifier_qualifier_list* _Owner _Opt specifier_qualifier_list;
    struct member_declarator_list* _Owner _Opt member_declarator_list_opt;

    struct static_assert_declaration* _Owner _Opt static_assert_declaration;
    struct pragma_declaration* _Owner _Opt pragma_declaration;

    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt;
    struct member_declaration* _Owner _Opt next;

};

struct member_declaration* _Owner _Opt member_declaration(struct parser_ctx* ctx,
    struct struct_or_union_specifier*);
void member_declaration_delete(struct member_declaration* _Owner _Opt p);

struct member_declarator
{
    /*
     member-declarator:
       declarator
       declarator opt : constant-expression
    */

    struct declarator* _Owner _Opt declarator;
    struct expression* _Owner _Opt constant_expression;
    struct member_declarator* _Owner _Opt next;
};
void member_declarator_delete(struct member_declarator* _Owner _Opt p);

struct member_declarator_list
{
    /*
     member-declarator-list:
        member-declarator
        member-declarator-list , member-declarator
    */

    struct member_declarator* _Owner _Opt head;
    struct member_declarator* _Opt tail;
};

struct member_declarator_list* _Owner _Opt member_declarator_list(struct parser_ctx* ctx,
    struct struct_or_union_specifier*, /*not const*/
    const struct specifier_qualifier_list* specifier_qualifier_list
);
void member_declarator_list_delete(struct member_declarator_list* _Owner _Opt p);
void member_declarator_list_add(struct member_declarator_list* list, struct member_declarator* _Owner p_item);

struct block_item_list
{
    /*
     block-item-list:
       block-item
       block-item-list block-item
    */
    struct block_item* _Owner _Opt head;
    struct block_item* _Opt tail;
};

struct block_item_list block_item_list(struct parser_ctx* ctx, bool* error);
void block_item_list_destroy(struct block_item_list* _Obj_owner p);
void block_item_list_add(struct block_item_list* list, struct block_item* _Owner p_item);

struct compound_statement
{
    /*
     compound-statement:
       { block-item-list _Opt }
    */
    struct token* first_token; /*{*/
    struct token* last_token; /*}*/

    struct block_item_list block_item_list;

    //flow analysys flags
    struct diagnostic diagnostic_flags;
};
struct compound_statement* _Owner _Opt compound_statement(struct parser_ctx* ctx);
void compound_statement_delete(struct compound_statement* _Owner _Opt p);

struct defer_statement
{
    /*
     defer-statement: (extension)
       "defer" secondary-block
    */
    struct token* first_token;
    struct token* last_token;
    struct secondary_block* _Owner secondary_block;
};

void defer_statement_delete(struct defer_statement* _Owner _Opt p);

struct try_statement
{
    /*
      try-statement: (extension)
       "try" secondary-block
       "try" secondary-block "catch" secondary-block
    */
    struct secondary_block* _Owner secondary_block;
    struct secondary_block* _Owner _Opt catch_secondary_block_opt;
    struct token* first_token; /*try*/
    struct token* last_token;
    struct token* _Opt catch_token_opt; /*catch*/
    /*Used to generate label names*/
    int try_catch_block_index;
};

struct try_statement* _Owner _Opt try_statement(struct parser_ctx* ctx);
void try_statement_delete(struct try_statement* _Owner _Opt p);

struct selection_statement
{
    /*
      selection-statement:
        "if" ( expression ) secondary-block
        "if" ( expression ) secondary-block "else" secondary-block
        "switch" ( expression ) secondary-block
    */

    /*
    Extension to support C++ 17 if with initialization

    selection-statement:
       "if" ( init-statement _Opt condition ) secondary-block
       "if" ( init-statement _Opt condition ) secondary-block "else" secondary-block
       switch ( init-statement _Opt condition ) secondary-block
    */
    struct init_statement* _Owner _Opt p_init_statement;
    struct condition* _Owner _Opt _Opt condition;

    struct secondary_block* _Owner secondary_block;
    struct secondary_block* _Owner _Opt else_secondary_block_opt;

    struct token* open_parentesis_token;
    struct token* close_parentesis_token;

    struct token* first_token;
    struct token* last_token;
    struct token* _Opt else_token_opt;
};

struct selection_statement* _Owner _Opt selection_statement(struct parser_ctx* ctx);
void selection_statement_delete(struct selection_statement* _Owner _Opt  p);
struct iteration_statement
{
    /*
      iteration-statement:
        "while" ( expression ) secondary-block
        "do" secondary-block "while" ( expression ) ;
        "for" ( expression _Opt ; expression _Opt ; expression _Opt ) secondary-block
        "for" ( declaration expression _Opt ; expression _Opt ) secondary-block
        "repeat" secondary-block  (extension)
    */

    struct token* first_token;
    struct token* second_token; /*do {} while*/

    struct secondary_block* _Owner _Opt secondary_block;
    struct expression* _Owner _Opt expression1;
    struct expression* _Owner _Opt expression2;
    struct expression* _Owner _Opt expression0;
    struct declaration* _Owner declaration;
};

struct iteration_statement* _Owner _Opt iteration_statement(struct parser_ctx* ctx);
void iteration_statement_delete(struct iteration_statement* _Owner _Opt p);

struct jump_statement
{
    /*
     jump-statement:
       "goto" identifier ;
       "continue" ;
       "break" ;
       "return" expression _Opt ;
    */

    struct token* _Opt label;
    struct token* first_token;
    struct token* last_token;
    struct expression* _Owner _Opt expression_opt;

    int try_catch_block_index;
};

struct jump_statement* _Owner _Opt jump_statement(struct parser_ctx* ctx);
void jump_statement_delete(struct jump_statement* _Owner _Opt p);

struct expression_statement
{
    /*
     expression-statement:
       expression _Opt;
       attribute-specifier-sequence expression ;
    */

    struct attribute_specifier_sequence* _Owner _Opt p_attribute_specifier_sequence_opt;
    struct expression* _Owner _Opt expression_opt;
};

struct expression_statement* _Owner _Opt expression_statement(struct parser_ctx* ctx, bool ignore_semicolon);
void expression_statement_delete(struct expression_statement* _Owner _Opt  p);

struct block_item
{
    /*
     block-item:
      declaration
      unlabeled-statement
      label
    */
    struct token* first_token; //?necessary
    struct declaration* _Owner declaration;
    struct unlabeled_statement* _Owner _Opt unlabeled_statement;
    struct label* _Owner _Opt label;

    struct block_item* _Owner _Opt next;
};

struct block_item* _Owner _Opt block_item(struct parser_ctx* ctx);
void block_item_delete(struct block_item* _Owner _Opt p);

struct compound_statement* _Owner _Opt function_body(struct parser_ctx* ctx);

struct designator
{
    /*
     designator:
       [ constant-expression ]
       . identifier
    */
    struct expression* _Owner _Opt constant_expression_opt;
    struct token* token;
    struct designator* _Owner _Opt next;
};
struct designator* _Owner _Opt designator(struct parser_ctx* ctx);
void designator_delete(struct designator* _Owner _Opt p);
struct initializer_list
{
    /*
      initializer-list:
        designation _Opt initializer
        initializer-list , designation _Opt initializer
    */
    struct token* first_token;
    struct initializer* _Owner _Opt head;
    struct initializer* _Opt tail;
    int size;
};
struct initializer_list* _Owner _Opt initializer_list(struct parser_ctx* ctx);
void initializer_list_delete(struct initializer_list* _Owner _Opt p);
void initializer_list_add(struct initializer_list* list, struct initializer* _Owner p_item);

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

    struct compound_statement* _Owner _Opt compound_statement;
    struct selection_statement* _Owner _Opt selection_statement;
    struct iteration_statement* _Owner _Opt iteration_statement;
    struct defer_statement* _Owner _Opt defer_statement;
    struct try_statement* _Owner _Opt try_statement;
};

void primary_block_delete(struct primary_block* _Owner _Opt p);

struct secondary_block
{
    /*
     secondary-block:
       statement
    */
    struct token* first_token;
    struct token* last_token;
    struct statement* _Owner statement;
};

void secondary_block_delete(struct secondary_block* _Owner _Opt p);
bool secondary_block_ends_with_jump(struct secondary_block* _Opt p_secondary_block);

struct unlabeled_statement
{
    /*
      unlabeled-statement:
        expression-statement
        attribute-specifier-sequence _Opt primary-block
        attribute-specifier-sequence _Opt jump-statement
     */

    struct expression_statement* _Owner _Opt expression_statement;
    struct primary_block* _Owner _Opt primary_block;
    struct jump_statement* _Owner _Opt jump_statement;
};

struct unlabeled_statement* _Owner _Opt unlabeled_statement(struct parser_ctx* ctx);
void unlabeled_statement_delete(struct unlabeled_statement* _Owner _Opt p);

struct labeled_statement
{
    /*
     label statement:
       label statement
    */
    struct label* _Owner label;
    struct statement* _Owner statement;
};
struct labeled_statement* _Owner _Opt labeled_statement(struct parser_ctx* ctx);
void labeled_statement_delete(struct labeled_statement* _Owner _Opt p);

struct statement
{
    /*
     statement:
       labeled-statement
       unlabeled-statemen
    */
    struct labeled_statement* _Owner _Opt labeled_statement;
    struct unlabeled_statement* _Owner _Opt unlabeled_statement;
};
struct statement* _Owner _Opt statement(struct parser_ctx* ctx);
void statement_delete(struct statement* _Owner _Opt  p);

struct designator_list
{
    /*
     designator-list:
       designator
       designator-list designator
    */
    struct designator* _Owner _Opt head;
    struct designator* _Opt tail;
};

struct designator_list* _Owner _Opt designator_list(struct parser_ctx* ctx);
void designator_list_delete(struct designator_list* _Owner _Opt p);
void designator_list_add(struct designator_list* list, struct designator* _Owner p_item);

struct designation
{
    /*
     designation:
       designator-list =
    */
    struct designator_list* _Owner designator_list;
    struct token* token;
};
struct designation* _Owner _Opt designation(struct parser_ctx* ctx);
void designation_delete(struct designation* _Owner _Opt p);

struct type_qualifier_list
{
    /*
     type-qualifier-list:
       type-qualifier
       type-qualifier-list type-qualifier
    */
    enum type_qualifier_flags flags;
    struct type_qualifier* _Owner _Opt head;
    struct type_qualifier* _Opt tail;
};

struct type_qualifier_list* _Owner _Opt type_qualifier_list(struct parser_ctx* ctx);
void type_qualifier_list_delete(struct type_qualifier_list* _Owner _Opt p);
void type_qualifier_list_add(struct type_qualifier_list* list, struct type_qualifier* _Owner p_item);

struct attribute_token
{
    enum attribute_flags attributes_flags;
    struct token* token;
};
struct attribute_token* _Owner _Opt attribute_token(struct parser_ctx* ctx);
void attribute_token_delete(struct attribute_token* _Owner _Opt p);
struct attribute
{
    enum attribute_flags  attributes_flags;
    struct attribute_token* _Owner attribute_token;
    struct attribute_argument_clause* _Owner attribute_argument_clause;
    struct attribute* _Owner _Opt next;
};
void attribute_delete(struct attribute* _Owner _Opt p);

struct attribute_list
{
    enum attribute_flags  attributes_flags;
    struct attribute* _Owner _Opt head;
    struct attribute* _Opt tail;
};
struct attribute_list* _Owner _Opt attribute_list(struct parser_ctx* ctx);
void attribute_list_destroy(struct attribute_list* _Obj_owner p);
void attribute_list_delete(struct attribute_list* _Owner _Opt p);

void attribute_list_add(struct attribute_list* list, struct attribute* _Owner p_item);

struct enumerator
{
    /*
      enumeration-constant:
        identifier

      enumerator:
        enumeration-constant attribute-specifier-sequence _Opt
        enumeration-constant attribute-specifier-sequence _Opt = constant-expression
    */

    struct token* token;
    struct attribute_specifier_sequence* _Owner _Opt attribute_specifier_sequence_opt;

    struct expression* _Owner _Opt constant_expression_opt;

    /*
      having the enum specifier we have better information about the type
    */
    const struct enum_specifier* enum_specifier;

    struct enumerator* _Owner _Opt next;
    long long value;
};

struct enumerator* _Owner _Opt enumerator(struct parser_ctx* ctx, const struct enum_specifier* p_enum_specifier, long long* p_enumerator_value);
void enumerator_delete(struct enumerator* _Owner _Opt  p);

struct attribute_argument_clause
{
    /*
     attribute-argument-clause:
       ( balanced-token-sequence _Opt )
    */
    struct balanced_token_sequence* _Owner _Opt p_balanced_token_sequence;
    struct token* token;
};

struct attribute_argument_clause* _Owner _Opt attribute_argument_clause(struct parser_ctx* ctx);
void attribute_argument_clause_delete(struct attribute_argument_clause* _Owner _Opt p);

bool first_of_attribute(const struct parser_ctx* ctx);

struct balanced_token
{
    struct token* token;
    struct balanced_token* _Owner _Opt next;
};

struct balanced_token_sequence
{
    struct balanced_token* _Owner _Opt head;
    struct balanced_token* _Opt tail;
};
struct balanced_token_sequence* _Owner _Opt balanced_token_sequence_opt(struct parser_ctx* ctx);
void balanced_token_sequence_delete(struct balanced_token_sequence* _Owner _Opt  p);

bool is_first_of_conditional_expression(struct parser_ctx* ctx);
bool first_of_type_name(const struct parser_ctx* ctx);
bool first_of_type_name_ahead(const struct parser_ctx* ctx);

struct argument_expression_list argument_expression_list(struct parser_ctx* ctx);

struct declaration_list
{
    struct declaration* _Owner _Opt head;
    struct declaration* _Opt tail;
};

struct declaration_list translation_unit(struct parser_ctx* ctx, bool* berror);
void declaration_list_destroy(struct declaration_list* _Obj_owner list);

struct label
{
    /*
     label:
       attribute-specifier-sequence _Opt identifier :
       attribute-specifier-sequence _Opt "case" constant-expression :
       attribute-specifier-sequence _Opt "default" :
    */
    struct expression* _Owner _Opt constant_expression;
    struct token* _Opt p_identifier_opt;
};

struct label* _Owner _Opt label(struct parser_ctx* ctx);
void label_delete(struct label* _Owner _Opt p);

struct ast
{
    struct token_list token_list;
    struct declaration_list declaration_list;
};


struct ast get_ast(struct options* options, const char* filename, const char* source, struct report* report);
void ast_destroy(struct ast* _Obj_owner ast);
struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);


struct declaration_list parse(struct parser_ctx* ctx, struct token_list* list, bool* berror);
const char* _Owner _Opt compile_source(const char* pszoptions, const char* content, struct report* report);
