/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include "cake_compat.h"

#include <stdio.h>
#include <errno.h>
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

void scope_destroy(_Dtor struct scope* p);
void scope_swap(struct scope* a, struct scope* b);

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

    /*has_errors is not reseted when we remove diagnostics*/
    bool has_errors;

    int error_count;
    int warnings_count;
    int info_count;

    bool test_mode;
    int test_failed;
    int test_succeeded;

    /*
      direct commands like -autoconfig doesnt use report
    */
    bool ignore_this_report;
    bool quiet; /* -quiet: no report on success */

    /* -find-definition: what the parse of one file found */
    bool find_definition_found;
    bool find_definition_is_declaration;
    bool find_definition_is_static;
    bool find_definition_is_tag;
    char find_definition_name[200];
    char find_definition_file[FS_MAX_PATH]; /* where the declaration is */
    int find_definition_line;
    int find_definition_col;
    bool find_definition_is_local; /* -rename: block scope or parameter */
    bool find_definition_is_macro; /* resolved by the preprocessor */

    /* optional: told about every #include of every file compiled */
    const struct include_listener* include_listener;
};

struct label_list_item
{
    struct token* p_last_usage;
    struct token* p_defined;
    struct label_list_item* _Owner  next;
};

struct label_list
{
    struct label_list_item* _Owner head;
    struct label_list_item* tail;
};

struct label_list_item* label_list_find(struct label_list* list, const char* label_name);
void label_list_push(struct label_list* list, struct label_list_item* _Owner pitem);
void label_list_clear(_Clear struct label_list* list);
void label_list_destroy(_Dtor struct label_list* list);


struct diagnostic_item;

struct diagnostic_queue
{
    struct diagnostic_item* _Owner head;
    struct diagnostic_item* tail;
    int count;
};

struct diagnostic_item
{
    int                line;
    enum diagnostic_id id;
    char* _Owner  text;
    char* _Owner  sarif_text;

    bool               is_error;
    bool               is_warning;
    bool               is_note;
    bool               is_location;

    struct diagnostic_queue children; /* W_LOCATION entries */

    struct diagnostic_item* _Owner next;
};

void diagnostic_queue_add(struct diagnostic_queue* q, struct diagnostic_item* _Owner e);
void diagnostic_queue_flush(struct diagnostic_queue* q, struct parser_ctx* ctx);
bool diagnostic_queue_remove(struct diagnostic_queue* q, enum diagnostic_id id);
void diagnostic_queue_destroy(_Dtor struct diagnostic_queue* q);


int parse_diagnostic_suppression(const char* p, int ids[], int ids_max);

struct block_item_list
{
    /*
     block-item-list:
       block-item
       block-item-list block-item
    */
    struct block_item* _Owner head;
    struct block_item* tail;
};

struct parser_ctx
{
    struct options options;

    /*
      file scope -> function params -> function -> inner scope
    */
    struct scope_list scopes;

    struct block_item_list used_incomplete_enums;

    /*
    * Points to the function we're in. Or null in file scope.
    */
    struct declarator* p_current_function_opt;

    /*
    * Points to the scope where the current function is. (used in local functions)
    */
    struct scope* p_current_function_scope_opt;


    /*
    *  Used to track non-used labels or used and not defined labels
    */
    struct label_list label_list;

    /*
    * Points to the try-block we're in. Or null.
    */
    const struct try_statement* p_current_try_statement_opt;

    const struct defer_statement* p_current_defer_statement_opt;

    /*
    * Points to the selection_statement we're in. Or null.
    */
    struct selection_statement* p_current_switch_statement;
    const struct iteration_statement* p_current_iteration_statement;


    FILE* _Owner sarif_file;
    unsigned int sarif_entries;

    const struct token_list* _Nonnull const p_input_list;
    struct token* current;

    bool inside_generic_association;

    /*
      Depth of enclosing UNEVALUATED operands -- sizeof, _Countof, _Alignof and
      traits. Their operand is analysed for its type only and never runs, so
      `sizeof(*p)` is not a use of what p points at (W_PARAM_COULD_BE_CONST).

      A dedicated counter rather than the parser's is_discarded flag, which is
      overloaded: for a non-constant condition it marks the TRUE arm of `?:` as
      discarded (see conditional_expression), so `n > 0 ? a[0] : 0` would have
      stopped counting as a use of a.
    */
    int unevaluated_operand_depth;

    int label_id; /*generates unique ids for labels*/

    /*
       complete structs have unique ids
    */
    unsigned int unique_tag_id;

    /*
      #pragma pack state. pack_alignment is the current maximum member
      alignment in bytes (0 = no packing); push/pop keep the history.
    */
    int pack_alignment;
    int pack_stack[32];
    int pack_stack_top;

    /*
      Used to generated id to vm dimension variables
    */
    unsigned int vm_dim_id;

    /*
       Generate tag names for anonymous structs
    */
    int anonymous_struct_count;

    struct report* _Nonnull p_report;


    struct diagnostic_queue diagnostic_queue;

    /*
       -format only: brace nesting depth, incremented/decremented around
       compound_statement()'s '{'/'}' (so it counts every block - function
       bodies, if/while/for/switch bodies, plain nested blocks - the same
       way, with no special case for switch: its body is one block like any
       other, so `case` lands one level deeper than `switch`).

       check_indentation_style() (parser.c) multiplies this by the style's
       indent_width to fix a block-item's leading whitespace.
    */
    int format_indent_level;

    /*
       -find-definition only: the definition of the identifier under the
       cursor, set where the parser resolves it (the scopes are live there).
       translation_unit stops after the external declaration that set it.
    */
    const struct token* p_find_definition;

    /* -find-definition: p_find_definition is only a declaration (prototype, extern) */
    bool find_definition_is_declaration;
    bool find_definition_is_static;
    bool find_definition_is_local;
    bool find_definition_is_tag; /* struct, union or enum not complete in this file */

    /* -find-definition: 'goto label' under the cursor whose label comes later in the function */
    const struct token* p_find_definition_label_use;

};

///////////////////////////////////////////////////////

void parser_ctx_destroy(_Dtor struct parser_ctx* ctx);

/*
   -format token surgery, shared with expressions.c - see parser.c for the
   rest of the brace/indentation fixers.
*/
bool format_active_for(const struct parser_ctx* ctx, const struct token* token);
struct token* format_next_real(struct token* token);
void format_ensure_one_space_before(struct token* token);
void format_ensure_no_space_before(struct token* token);
void format_align_if_already_wrapped(struct token* token, const char* indent);
void format_align_to_column_if_already_wrapped(struct token* token, int column);


struct token* parser_look_ahead(const struct parser_ctx* ctx);
void unexpected_end_of_file(const struct parser_ctx* ctx);
void parser_match(struct parser_ctx* ctx);
NODISCARD
int parser_match_tk(struct parser_ctx* ctx, enum token_type type);
int parser_match_tk_lint(struct parser_ctx* ctx, enum token_type type, struct token* _Nullable* pp_token_lint);

bool find_definition_is_cursor(const struct parser_ctx* ctx, const struct token* p_token);
void find_definition_set(struct parser_ctx* ctx, const struct token* _Nullable p_definition);
void find_definition_set_declarator(struct parser_ctx* ctx, const struct declarator* p_declarator);
void find_definition_report(const struct parser_ctx* ctx);

/* Every answer the parser prints (not diagnostics) goes through here, so it can be sent to another output */
void ctx_print(const struct parser_ctx* ctx, const char* fmt, ...);

/* -complete: true when p_token is the first token at or after the cursor line:col */
bool complete_is_cursor(const struct parser_ctx* ctx, const struct token* _Nullable p_token);
/* -complete: prints the names visible in the current scopes */
void complete_print_scopes(const struct parser_ctx* ctx);
/* -complete: prints the members of the struct or union (after '.' or '->') */
void complete_print_members(const struct parser_ctx* ctx, struct struct_or_union_specifier* p_complete);

const struct token* previous_parser_token_const(const struct token* token);
struct token* previous_parser_token(const struct token* token);
struct token* parser_get_previous_token(const struct parser_ctx* ctx);
struct declarator* find_declarator(const struct parser_ctx* ctx, const char* lexeme, struct scope* _Nullable* _Nullable ppscope_opt);
struct enumerator* find_enumerator(const struct parser_ctx* ctx, const char* lexeme, struct scope* _Nullable* _Nullable ppscope_opt);
struct map_entry* find_variables(const struct parser_ctx* ctx, const char* lexeme, struct scope* _Nullable* _Nullable ppscope_opt);

struct struct_or_union_specifier* find_struct_or_union_specifier(const struct parser_ctx* ctx, const char* lexeme);

void print_scope(struct scope_list* e);

/*
  This is the function called by the web version.
*/
char* _Owner CompileText(const char* options, const char* content);

_Bool diagnostic(enum diagnostic_id w,
    const struct parser_ctx* ctx,
    const struct token* _Nullable p_token,
    const struct marker* _Nullable p_marker,
    const char* fmt, ...);

/* diagnostic() without a parser context or token: position only, printed immediately */
bool pos_diagnostic(enum diagnostic_id w,
    const struct options* options,
    struct report* report,
    const char* file,
    int line,
    int col,
    const char* fmt, ...);

int compile(int argc, const char** argv, struct report* error);

void print_type_qualifier_flags(struct osstream* ss, bool* first, enum type_qualifier_flags e_type_qualifier_flags);
bool print_type_alignment_flags(struct osstream* ss, bool* first, enum alignment_specifier_flags flags, const struct platform* target);
enum alignment_specifier_flags alignment_value_to_flags(long long alignment);
bool print_type_specifier_flags(struct osstream* ss, bool* first, enum type_specifier_flags e_type_specifier_flags, int bitint_width);


struct expression_ctx;

struct declaration_specifier
{
    /*
       declaration-specifier:
         storage-class-specifier
         type-specifier-qualifier
         function-specifier
    */
    struct storage_class_specifier* _Owner storage_class_specifier;

    struct type_specifier_qualifier* _Owner type_specifier_qualifier;

    struct function_specifier* _Owner function_specifier;
    struct alignment_specifier* _Owner alignment_specifier;

    struct declaration_specifier* _Owner next;
};

struct declaration_specifier* _Owner declaration_specifier(struct parser_ctx* ctx);
void declaration_specifier_delete(_Dtor struct declaration_specifier* _Owner _Nullable p);

struct declaration_specifiers
{
    /*
     declaration-specifiers:
       declaration-specifier attribute-specifier-sequence opt
       declaration-specifier declaration-specifiers
    */

    /*cumulative flags*/
    enum attribute_flags  attributes_flags;
    enum msvc_declspec_flags msvc_declspec_flags;
    enum type_specifier_flags type_specifier_flags;
    enum type_qualifier_flags type_qualifier_flags;
    enum storage_class_specifier_flags storage_class_specifier_flags;
    enum function_specifier_flags function_specifier_flags;

    /* N of _BitInt(N) when type_specifier_flags has TYPE_SPECIFIER_BITINT */
    int bitint_width;

    enum alignment_specifier_flags alignment_specifier_flags;
    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;

    /*shortcuts*/
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;
    struct declarator* typedef_declarator;
    struct typeof_specifier* typeof_specifier;
    struct atomic_type_specifier* atomic_type_specifier;

    struct token* _Nonnull first_token; /*not _Owner*/
    struct token* _Nonnull last_token; /*not _Owner*/

    struct declaration_specifier* _Owner head;
    struct declaration_specifier* tail;
};

void print_declaration_specifiers(struct osstream* ss, const struct declaration_specifiers* p_declaration_specifiers);
struct declaration_specifiers* _Owner declaration_specifiers(struct parser_ctx* ctx, enum storage_class_specifier_flags default_storage_flag);
void declaration_specifiers_delete(_Dtor struct declaration_specifiers* _Owner _Nullable p);
void declaration_specifiers_add(struct declaration_specifiers* p, struct declaration_specifier* _Owner item);

struct static_assertion
{
    /*
        C2Y
        static-assertion:
        static_assert ( constant-expression , string-literal )
        static_assert ( constant-expression )
    */

    /*
      I am keeping the name 'static_assertion' but better is

      static_declaration:
       static_assertion
       static_debug_declaration

      extension:
      "static_debug" ( constant-expression ) ;
      "static_debug" ( ) ;

    */

    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
    struct token* p_lint_token;
    struct expression* _Owner constant_expression; /* null only in static_debug() */
    struct token* string_literal_opt;
};
struct static_assertion* _Owner static_assertion(struct parser_ctx* ctx);
void static_assertion_delete(_Dtor struct static_assertion* _Owner _Nullable p);

bool first_of_static_assertion(const struct parser_ctx* ctx);

/*
  extension, pragma survives the preprocessor and become
  a pragma_declaration that can be used on the AST visit
*/

struct pragma_declaration {
    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
};

struct pragma_declaration* _Nonnull _Owner pragma_declaration(struct parser_ctx* ctx);
void pragma_declaration_delete(_Dtor struct pragma_declaration* _Owner _Nullable p);

void execute_pragma_declaration(struct parser_ctx* ctx, struct pragma_declaration* p_pragma);

struct attribute_specifier_sequence
{
    /*
     attribute-specifier-sequence:
       attribute-specifier-sequence opt attribute-specifier
    */

    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
    enum msvc_declspec_flags msvc_declspec_flags;
    enum attribute_flags  attributes_flags;

    /*
        GCC __attribute__((packed)) / __attribute__((aligned(n))) - n in
        bytes, 0 when absent. Applied to a struct or to one of its members
        (see struct_or_union_specifier and declarator).
    */
    bool gcc_packed;
    int gcc_aligned;

    struct attribute_specifier* _Owner head;
    struct attribute_specifier* tail;
};
struct attribute_specifier_sequence* _Owner attribute_specifier_sequence_opt(struct parser_ctx* ctx);
void attribute_specifier_sequence_delete(_Dtor struct attribute_specifier_sequence* _Owner _Nullable p);
void attribute_specifier_sequence_add(struct attribute_specifier_sequence* list, struct attribute_specifier* _Owner p_item);

struct attribute_specifier
{
    /*
     attribute-specifier:
        [ [ attribute-list ] ]
    */
    struct token* _Nonnull first_token;
    struct token* last_token;
    struct attribute_list* _Owner attribute_list;
    struct attribute_specifier* _Owner  next;
};

struct attribute_specifier* _Owner attribute_specifier(struct parser_ctx* ctx);
void attribute_specifier_delete(_Dtor struct attribute_specifier* _Owner _Nullable p);

struct attribute* _Owner attribute(struct parser_ctx* ctx);


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
    struct token* _Nonnull token;
};

struct storage_class_specifier* _Owner storage_class_specifier(struct parser_ctx* ctx);
void storage_class_specifier_delete(_Dtor struct storage_class_specifier* _Owner _Nullable p);

bool first_of_storage_class_specifier_token(const struct token* p_token);

struct storage_class_specifier_node
{
    struct storage_class_specifier storage_class_specifier;
    struct storage_class_specifier_node* _Owner next;
};

void storage_class_specifier_node_delete(_Dtor struct storage_class_specifier_node* _Owner _Nullable p);

struct storage_class_specifiers
{
    /*
        C23
        storage-class-specifiers:
          storage-class-specifier
          storage-class-specifiers storage-class-specifier
    */
    enum storage_class_specifier_flags storage_class_specifier_flags;
    struct storage_class_specifier_node* _Owner head;
    struct storage_class_specifier_node* tail;
};

struct storage_class_specifiers* _Owner storage_class_specifiers(struct parser_ctx* ctx);
void storage_class_specifiers_delete(_Dtor struct storage_class_specifiers* _Nullable _Owner p);

struct function_specifier
{
    /*
     function-specifier:
       inline
       _Noreturn
    */
    enum function_specifier_flags flags;
    struct token* _Nonnull token;
};
struct function_specifier* _Owner  function_specifier(struct parser_ctx* ctx);
void function_specifier_delete(_Dtor struct function_specifier* _Owner _Nullable  p);

struct typeof_specifier_argument
{
    /*
     typeof-specifier-argument:
       expression
       type-name
    */
    struct expression* _Owner expression;
    struct type_name* _Owner type_name;
};

void typeof_specifier_argument_delete(_Dtor struct typeof_specifier_argument* _Owner _Nullable p);

struct typeof_specifier
{
    /*
     typeof-specifier:
       "typeof" ( typeof-specifier-argument )
    */
    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
    struct typeof_specifier_argument* _Nonnull _Owner typeof_specifier_argument;
    struct type type;
};

void typeof_specifier_delete(_Dtor struct typeof_specifier* _Owner _Nullable p);


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
    struct token* _Nonnull token;
    struct struct_or_union_specifier* _Owner struct_or_union_specifier;
    struct typeof_specifier* _Owner  typeof_specifier;
    struct enum_specifier* _Owner enum_specifier;
    struct declarator* typedef_declarator;
    struct atomic_type_specifier* _Owner  atomic_type_specifier;

    /* _BitInt ( constant-expression ) */
    struct expression* _Owner bitint_constant_expression;
    int bitint_width;
};

struct type_specifier* _Owner type_specifier(struct parser_ctx* ctx);
void type_specifier_delete(_Dtor struct type_specifier* _Owner _Nullable p);

struct init_declarator_list
{
    /*
     init-declarator-list:
       init-declarator
       init-declarator-list , init-declarator
    */
    struct init_declarator* _Owner head;
    struct init_declarator* tail;
};

struct init_declarator_list init_declarator_list(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers);

void init_declarator_list_destroy(_Dtor struct init_declarator_list* p);
void init_declarator_list_add(struct init_declarator_list* list, struct init_declarator* _Owner p_item);



struct declaration
{
    /*
      declaration:
        declaration-specifiers init-declarator-list _Nullable ;
        attribute-specifier-sequence declaration-specifiers init-declarator-list ;
        static_assert-declaration
        attribute-declaration

        declaration-specifiers init-declarator-list _Nullable pragma-declaration ; (cake)
        attribute-specifier-sequence declaration-specifiers init-declarator-list pragma-declaration ; (cake)
        pragma-declaration (cake)
    */
    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;

    struct static_assertion* _Owner static_assertion;

    struct pragma_declaration* _Owner pragma_declaration; /* can be alone or not */


    struct declaration_specifiers* _Owner  declaration_specifiers;
    struct init_declarator_list init_declarator_list;

    struct compound_statement* _Owner  function_body;
    struct defer_list defer_list; //arguments

    struct declarator* contract_declarator;

    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
    struct token* lint_token;

    struct declaration* _Owner next;
};

void declaration_delete(_Dtor struct declaration* _Owner _Nullable p);
struct declaration* _Owner external_declaration(struct parser_ctx* ctx);

struct simple_declaration
{
    /*
    This is an extension to support C++ 17 if with initialization

    simple-declaration:
      declaration-specifiers init-declarator-list _Nullable ;
      attribute-specifier-sequence declaration-specifiers init-declarator-list ;
    */
    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;
    struct declaration_specifiers* _Owner p_declaration_specifiers;
    struct init_declarator_list init_declarator_list;
    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
};

void simple_declaration_delete(_Dtor struct simple_declaration* _Owner _Nullable p);

struct simple_declaration* _Owner simple_declaration(struct parser_ctx* ctx,
    bool ignore_semicolon,
    struct attribute_specifier_sequence* _Owner _Nullable p_attribute_specifier_sequence);

struct condition
{
    /*
      This is an extension to support C++ 17 if with initialization

      condition :
       expression
       attribute-specifier-seq opt decl-specifier-seq declarator initializer
    */
    struct expression* _Owner expression;
    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;
    struct declaration_specifiers* _Owner p_declaration_specifiers;

    /*
      OBS:
      We must use p_init_declarator because it is kept on the scope
      as init_declarator when we are trying to parse init-statement or condition that
      are very similar
    */
    struct init_declarator* _Owner p_init_declarator;

    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
};

void condition_delete(_Dtor struct condition* _Owner _Nullable p);
struct condition* _Owner condition(struct parser_ctx* ctx);

struct init_statement
{
    /*
        This is an extension to support C++ 17 if with initialization

        init-statement :
          expression-statement
          simple-declaration
    */
    struct expression_statement* _Owner p_expression_statement;
    struct simple_declaration* _Owner p_simple_declaration;
};

void init_statement_delete(_Dtor struct init_statement* _Owner _Nullable p);
struct init_statement* _Owner init_statement(struct parser_ctx* ctx, bool ignore_semicolon);

struct atomic_type_specifier
{
    /*
      atomic-type-specifier:
        "_Atomic" ( type-name )
    */
    struct token* _Nonnull token;
    struct type_name* _Nonnull _Owner type_name;
    struct type type;
};

struct atomic_type_specifier* _Owner atomic_type_specifier(struct parser_ctx* ctx);
void atomic_type_specifier_delete(_Dtor struct atomic_type_specifier* _Owner _Nullable  p);

struct enumerator_list
{
    /*
     enumerator-list:
       enumerator
       enumerator-list , enumerator
    */
    struct enumerator* _Owner head;
    struct enumerator* tail;
};

struct enumerator_list enumerator_list(struct parser_ctx* ctx,
    struct enum_specifier* p_enum_specifier,
    const struct enum_specifier* _Nullable prev_decl_same_scope
);

void enumerator_list_destroy(_Dtor struct enumerator_list* p_enum_specifier);
void enumerator_list_swap(struct enumerator_list* a, struct enumerator_list* b);
void enumerator_list_add(struct enumerator_list* list, struct enumerator* _Owner p_item);

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

    /*
       If has_shared_ownership is
        - true,  both AST and some map are sharing the ownership
        - false, only AST OR and some map have the ownership
    */
    bool has_shared_ownership;
    bool has_underlying;

    struct type integer_type;

    struct attribute_specifier_sequence* _Owner attribute_specifier_sequence_opt;

    char tag_name[200];

    struct enumerator_list enumerator_list;

    struct token* tag_token;
    /* the tag of the first declaration of this enum in its scope (the identity for -rename, -find-usages) */
    const struct token* first_tag_token;
    struct token* _Nonnull first_token;
    /*points to the complete enum (can be self pointed)*/
    struct enum_specifier* p_complete_enum_specifier;
};

struct enum_specifier* _Owner enum_specifier(struct parser_ctx*);

struct enum_specifier* _Nonnull _Owner enum_specifier_add_ref(struct enum_specifier* p);
void enum_specifier_delete(_Dtor struct enum_specifier* _Owner _Nullable p);
const struct enum_specifier* get_complete_enum_specifier(const struct enum_specifier* p_enum_specifier);
const struct enum_specifier* get_enum_specifier_definition(const struct enum_specifier* p_enum_specifier);
enum type_specifier_flags get_enum_type_specifier_flags(const struct enum_specifier* p_enum_specifier);

const struct enumerator* find_enumerator_by_value(const struct parser_ctx* ctx, const struct enum_specifier* p_enum_specifier, const struct object* object);

struct member_declaration_list
{
    /*
     member-declaration-list:
       member-declaration
       member-declaration-list member-declaration
    */

    struct token* first_token; /*TODO ? necessary*/
    struct token* last_token;
    struct member_declaration* _Owner head;
    struct member_declaration* tail;
};

struct member_declaration_list member_declaration_list(struct parser_ctx* ctx, struct struct_or_union_specifier*);
void member_declaration_list_destroy(_Dtor struct member_declaration_list* p);
void member_declaration_list_swap(struct member_declaration_list* a, struct member_declaration_list* b);
void member_declaration_list_add(struct member_declaration_list* list, struct member_declaration* _Owner p_item);

struct member_declarator* find_member_declarator(struct member_declaration_list* list, const char* name, int* p_member_index);

struct struct_or_union_specifier
{
    /*
       If has_shared_ownership is
        - true,  both AST and some map are sharing the ownership
        - false, only AST OR and some map have the ownership
    */
    bool has_shared_ownership;

    struct attribute_specifier_sequence* _Owner attribute_specifier_sequence_opt;
    struct member_declaration_list member_declaration_list;

    struct token* _Nonnull first_token;
    struct token* last_token;

    bool is_owner;

    /*
    *Token that has a strut tag
    */
    struct token* tagtoken;

    unsigned int unique_id;

    char tag_name[200];
    /*geramos um tag name para anomimas, mas colocamos banonymousTag para true*/
    bool has_anonymous_tag;

    /* set when defined inside a parameter list: that scope. The tag is moved to the enclosing scope (C23 tag compatibility) */
    struct scope* p_parameters_scope_opt;
    /*it was asked to show struct tag created for anonymous*/
    bool show_anonymous_tag;

    int scope_level; /*nivel escopo 0 global*/
    int visit_moved; /*nivel escopo 0 global*/

    /*
        #pragma pack in effect when the members were parsed, or 1 for
        __attribute__((packed)): caps every member's alignment (0 = none).
        See get_sizeof_struct.
    */
    int pack_alignment;

    /*
        __attribute__((aligned(n))) on the struct itself: raises its
        alignment to at least n bytes (0 = none).
    */
    int aligned_attribute;

    /*
    * This points to the first struct_or_union_specifier that will have it´s
    * complete_struct_or_union_specifier_indirection pointing to the complete
    * struct_or_union_specifier.
    */
    struct struct_or_union_specifier* complete_struct_or_union_specifier_indirection;

    /*
      The tag of the first declaration of this struct (the identity for
      -find-declaration, -rename, -find-usages). A definition with the same
      content as a previous one (C23), also from an enclosing scope, is the
      same struct.
    */
    const struct token* first_tag_token;
};

struct struct_or_union_specifier* _Owner struct_or_union_specifier(struct parser_ctx* ctx);
struct struct_or_union_specifier* _Nonnull _Owner struct_or_union_specifier_add_ref(struct struct_or_union_specifier* p);
bool struct_or_union_specifier_is_union(const struct struct_or_union_specifier* p);
void struct_or_union_specifier_delete(_Dtor struct struct_or_union_specifier* _Owner _Nullable  p);

struct struct_or_union_specifier* get_complete_struct_or_union_specifier(const struct struct_or_union_specifier* p_struct_or_union_specifier);

struct init_declarator
{
    /*
     init-declarator:
        declarator
        declarator = initializer
    */

    /*
       If has_shared_ownership is
        - true,  both AST and some map are sharing the ownership
        - false, only AST OR and some map have the ownership
    */
    bool has_shared_ownership;

    struct declarator* _Nonnull _Owner p_declarator;
    struct initializer* _Owner initializer;
    struct init_declarator* _Owner next;
};


struct init_declarator* _Nonnull _Owner init_declarator_add_ref(struct init_declarator* p);
void init_declarator_delete(_Dtor struct init_declarator* _Owner _Nullable p);
struct init_declarator* _Owner init_declarator(struct parser_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers
);

struct initializer
{
    /*
     initializer:
       assignment-expression
       braced-initializer
    */
    struct token* _Nonnull first_token;
    struct braced_initializer* _Owner braced_initializer;
    struct expression* _Owner assignment_expression;
};

struct initializer* _Owner initializer(struct parser_ctx* ctx, bool is_discarded);
void initializer_delete(_Dtor struct initializer* _Owner _Nullable  p);

struct declarator
{
    /*
      declarator:
        pointer _Nullable direct-declarator
    */

    /*
       If has_shared_ownership is
        - true,  both AST and Map are sharing the ownership
        - false, only AST OR Map have the ownership
    */
    bool has_shared_ownership;
    struct token* first_token_opt;
    struct token* last_token_opt;

    struct pointer* _Owner pointer;
    struct direct_declarator* _Owner direct_declarator;

    struct declaration_specifiers* declaration_specifiers;
    const struct specifier_qualifier_list* specifier_qualifier_list;

    struct token* name_opt; //shortcut , null for abstract declarator

    /*
        GCC attributes written on this declarator or on its declaration:
        packed gives a struct member alignment 1; aligned(n) is folded into
        the type's alignment_specifier_flags (see
        type_set_alignment_specifier_flags_using_declarator).
    */
    bool gcc_packed;
    int gcc_aligned;

    struct compound_statement* function_body;

    /* file scope objects the function body uses: flow may assume a parameter points to one */
    const struct declarator* _Nullable* _Owner used_globals;
    int used_globals_count;
    int used_globals_capacity;

    /*
        points to someone that has the function_body or
        to someone that points to someone that has the function_body
    */
    struct declarator* p_complete_declarator;

    int num_uses; /*used to show not used warnings*/
    int num_evaluated_uses; /*uses outside sizeof, typeof etc. (N3884 local function definition)*/
    int num_writes;
    bool address_taken;
    bool pointee_written;
    bool pointee_escaped;
    /* something reachable below the pointee is written or passed to a non-const pointer */
    bool pointee_reachable_written;
    bool pointee_used;

    struct object object;

    /*user by flow analysis*/
    struct flow1_object* p_flow_object;

    struct expression* _Owner p_expression_true;
    struct expression* _Owner p_expression_false;

    /*Used to check contracts*/
    struct expression* p_alias_of_expression;

    /*
      used in code generation to indicate when the declarator was renamed
    */
    bool declarator_renamed;

    /*
      source name before renaming (used by __func__ in local functions)
    */
    char* _Owner original_name;

    /*
      true once this declarator has been given an initializer,
      used to detect redefinition of file-scope objects
    */
    bool initialized;
};

struct function_declarator* declarator_find_function_declarator(const struct declarator* p_declarator);

const struct declarator* declarator_get_function_definition_const(const struct declarator* p);
struct declarator* declarator_get_function_definition(struct declarator* declarator);

struct declarator;
void print_declarator(struct osstream* ss, struct declarator* declarator, bool is_abstract);

struct declarator* _Owner declarator(struct parser_ctx* ctx,
    const struct specifier_qualifier_list* _Nullable specifier_qualifier_list,
    struct declaration_specifiers* _Nullable declaration_specifiers,
    bool abstract_acceptable,
    struct token*_Nullable * _Nullable pptokenname);

struct declarator* _Nonnull _Owner declarator_add_ref(struct declarator* p);
void declarator_delete(_Dtor struct declarator* _Owner _Nullable  p);

struct array_declarator
{
    /*
     array-declarator:
        direct-declarator [ type-qualifier-list opt assignment-expression opt ]
        direct-declarator [ "static" type-qualifier-list opt assignment-expression ]
        direct-declarator [ type-qualifier-list "static" assignment-expression ]
        direct-declarator [ type-qualifier-list opt * ]
    */
    struct direct_declarator* _Owner direct_declarator;
    struct expression* _Owner  assignment_expression;
    struct expression* _Owner expression;
    struct type_qualifier_list* _Owner type_qualifier_list_opt;

    struct token* token;
    struct token* static_token_opt;
};

void array_declarator_delete(_Dtor struct array_declarator* _Owner _Nullable p);

/*
  Return a value > 0 if it has constant size
*/
size_t array_declarator_is_vla(const struct array_declarator* p_array_declarator);

struct function_declarator
{
    /*
     function-declarator:
       direct-declarator ( parameter-type-list _Nullable )
    */
    struct direct_declarator* _Owner direct_declarator;
    struct scope parameters_scope;// used for scope parameters
    struct parameter_type_list* _Owner parameter_type_list_opt;


#define EXPERIMENTAL_CONTRACTS_II 1
#ifdef EXPERIMENTAL_CONTRACTS_II
    struct parameter_declaration* _Owner p_out_parameter_declaration;
    struct secondary_block* _Owner p_in_block;
    struct secondary_block* _Owner p_out_block;
#endif

};

void function_declarator_delete(_Dtor struct function_declarator* _Owner _Nullable p);

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
    struct token* p_calling_convention;
    struct declarator* _Owner declarator;
    struct array_declarator* _Owner array_declarator;
    struct function_declarator* _Owner function_declarator;
    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;
};

void direct_declarator_delete(_Dtor struct direct_declarator* _Owner _Nullable p);

struct direct_declarator* _Owner direct_declarator(struct parser_ctx* ctx,
    const struct specifier_qualifier_list* _Nullable specifier_qualifier_list,
    struct declaration_specifiers* _Nullable declaration_specifiers,
    bool abstract_acceptable,
    struct token* _Nullable* _Nullable pptoken_name
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
    struct parameter_list* _Owner parameter_list;
};

struct parameter_type_list* _Owner parameter_type_list(struct parser_ctx* ctx);
void parameter_type_list_delete(_Dtor struct parameter_type_list* _Owner _Nullable  p);

struct pointer
{
    /*
     pointer:
        * attribute-specifier-sequence opt type-qualifier-list opt
        * attribute-specifier-sequence opt type-qualifier-list opt pointer
    */
    struct attribute_specifier_sequence* _Owner  attribute_specifier_sequence_opt;
    struct type_qualifier_list* _Owner type_qualifier_list_opt;

    /*
      typedef int (__fastcall *pf)();
    */
    struct token* calling_convention;

    struct pointer* _Owner pointer;
};

struct pointer* _Owner  pointer_opt(struct parser_ctx* ctx);
void pointer_delete(_Dtor struct pointer* _Owner _Nullable p);

struct parameter_list
{
    /*
     parameter-list:
        parameter-declaration
        parameter-list , parameter-declaration
    */
    struct parameter_declaration* _Owner head;
    struct parameter_declaration* tail;
};

struct parameter_list* _Owner  parameter_list(struct parser_ctx* ctx);
void parameter_list_delete(_Dtor struct parameter_list* _Owner _Nullable  p);
void parameter_list_add(struct parameter_list* list, struct parameter_declaration* _Owner p_item);

struct parameter_declaration
{
    /*
     parameter-declaration:
      attribute-specifier-sequence opt declaration-specifiers declarator
      attribute-specifier-sequence opt declaration-specifiers abstract-declarator opt
    */
    struct attribute_specifier_sequence* _Owner  attribute_specifier_sequence_opt;

    struct declaration_specifiers* _Nonnull _Owner declaration_specifiers;
    struct declarator* _Owner  declarator;

    /* void f(int i compile_assert(i > 0)); checked at each call */
    struct static_assertion* _Owner p_compile_assert;
    struct parameter_declaration* _Owner next;
};

struct parameter_declaration* _Owner parameter_declaration(struct parser_ctx* ctx);
void parameter_declaration_delete(_Dtor struct parameter_declaration* _Owner _Nullable  p);


struct type_name
{
    /*
     type-name:
       specifier-qualifier-list abstract-declarator opt
    */

    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
    struct specifier_qualifier_list* _Nonnull _Owner specifier_qualifier_list;
    struct declarator* _Nonnull _Owner abstract_declarator; //TODO OPT
    struct type type;
};

struct type_name* _Owner type_name(struct parser_ctx* ctx);
void type_name_delete(_Dtor struct type_name* _Owner _Nullable p);

struct argument_expression
{
    struct expression* _Nonnull _Owner expression;
    struct argument_expression* _Owner next;
    bool set_unkown; //used in flow analysis need to be removed..
};

void argument_expression_delete(_Dtor struct argument_expression* _Owner _Nullable  p);

struct braced_initializer
{
    /*
      { }
      { initializer-list }
      { initializer-list , }
    */

    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
    struct initializer_list* _Owner initializer_list;
};

struct braced_initializer* _Owner braced_initializer(struct parser_ctx* ctx);
void braced_initializer_delete(_Dtor struct braced_initializer* _Owner _Nullable p);
bool braced_initializer_is_empty(const struct braced_initializer* p_braced_initializer);

struct type_specifier_qualifier
{
    /*
     type-specifier-qualifier:
       type-specifier
       type-qualifier
       alignment-specifier
    */

    struct type_specifier* _Owner type_specifier;
    struct type_qualifier* _Owner type_qualifier;
    struct alignment_specifier* _Owner alignment_specifier;

    struct type_specifier_qualifier* _Owner next;
};
struct type_specifier_qualifier* _Owner type_specifier_qualifier(struct parser_ctx* ctx);
void type_specifier_qualifier_delete(_Dtor struct type_specifier_qualifier* _Owner _Nullable  p);

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
    enum alignment_specifier_flags alignment_specifier_flags;

    /* N of _BitInt(N) when type_specifier_flags has TYPE_SPECIFIER_BITINT */
    int bitint_width;

    /*shortcuts*/
    struct struct_or_union_specifier* struct_or_union_specifier;
    struct enum_specifier* enum_specifier;
    struct typeof_specifier* typeof_specifier;
    struct atomic_type_specifier* atomic_type_specifier;
    struct declarator* typedef_declarator;

    struct type_specifier_qualifier* _Owner head;
    struct type_specifier_qualifier* tail;
    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;

    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;

};

struct specifier_qualifier_list* _Owner specifier_qualifier_list(struct parser_ctx* ctx);
void specifier_qualifier_list_delete(_Dtor struct specifier_qualifier_list* _Owner _Nullable p);
void specifier_qualifier_list_add(struct specifier_qualifier_list* list, struct type_specifier_qualifier* _Owner p_item);

struct alignment_specifier
{
    /*
     alignment-specifier:
       "alignas" ( type-name )
       "alignas" ( constant-expression )
    */
    enum alignment_specifier_flags flags;
    struct type_name* _Owner type_name;
    struct expression* _Owner constant_expression;
    struct token* _Nonnull token;
};

struct alignment_specifier* _Owner alignment_specifier(struct parser_ctx* ctx);
void alignment_specifier_delete(_Dtor struct alignment_specifier* _Owner _Nullable p);

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
    struct token* _Nonnull token;

    /*
      Next is used when inside struct type_qualifier_list
      Not used when inside struct type_specifier_qualifier
    */
    struct type_qualifier* _Owner next;
};

struct type_qualifier* _Owner type_qualifier(struct parser_ctx* ctx);

struct member_declaration
{
    /*
     member-declaration:
       attribute-specifier-sequence opt specifier-qualifier-list member-declarator-list opt ;
       static_assert-declaration
    */
    struct specifier_qualifier_list* _Owner specifier_qualifier_list;
    struct member_declarator_list* _Owner member_declarator_list_opt;

    struct static_assertion* _Owner static_assertion;
    struct pragma_declaration* _Owner pragma_declaration;

    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;
    struct member_declaration* _Owner next;

};

struct member_declaration* _Owner member_declaration(struct parser_ctx* ctx,
    struct struct_or_union_specifier*);
void member_declaration_delete(_Dtor struct member_declaration* _Owner _Nullable p);

struct member_declarator
{
    /*
     member-declarator:
       declarator
       declarator opt : constant-expression
    */

    struct declarator* _Owner declarator;
    struct expression* _Owner constant_expression;

    /*
      Position of the member's first bit, counted from the start of the
      struct (bit 0 is the low bit of the first byte). Written by
      get_sizeof_struct for the target it was called with.
    */
    size_t bit_offset;

    struct member_declarator* _Owner next;
};
void member_declarator_delete(_Dtor struct member_declarator* _Owner _Nullable p);

struct member_declarator_list
{
    /*
     member-declarator-list:
        member-declarator
        member-declarator-list , member-declarator
    */

    struct member_declarator* _Owner head;
    struct member_declarator* tail;
};

struct member_declarator* find_member_declarator_by_index(struct member_declaration_list* list, int member_index);

struct member_declarator_list* _Owner member_declarator_list(struct parser_ctx* ctx,
    struct struct_or_union_specifier*, /*not const*/
    const struct specifier_qualifier_list* specifier_qualifier_list
);
void member_declarator_list_delete(_Dtor struct member_declarator_list* _Owner _Nullable p);
void member_declarator_list_add(struct member_declarator_list* list, struct member_declarator* _Owner p_item);

struct block_item_list block_item_list(struct parser_ctx* ctx, bool* error);
void block_item_list_destroy(_Dtor struct block_item_list* p);
void block_item_list_add(struct block_item_list* list, struct block_item* _Owner p_item);


struct compound_statement
{
    /*
     compound-statement:
       { block-item-list _Nullable }
    */
    struct token* first_token; /*{*/
    struct token* last_token; /*}*/
    struct token* lint_token; /* //lint */

    struct block_item_list block_item_list;

    //flow analysis flags
    struct diagnostic diagnostic_flags;

    struct defer_list defer_list;
};
/*
  is_function_body: the outermost compound statement of a function definition.
  Its trailing `//lint N` must NOT be consumed here -- declaration_core consumes
  it after the parameter checks have run. See compound_statement's body.
*/
struct compound_statement* _Owner compound_statement(struct parser_ctx* ctx, bool is_function_body);
void compound_statement_delete(_Dtor struct compound_statement* _Owner _Nullable p);
bool compound_statement_is_last_item_return(const struct compound_statement* p_compound_statement);

struct defer_statement
{
    /*
     https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3734.pdf

     defer-statement:
         _Defer deferred-block

     deferred-block:
         unlabeled-statement
    */
    struct token* _Nonnull first_token;
    struct token* last_token;
    struct unlabeled_statement* _Owner unlabeled_statement;
};

void defer_statement_delete(_Dtor struct defer_statement* _Owner _Nullable p);

struct defer_list_item
{
    struct declarator* declarator;
    struct expression* compound_literal;
    struct defer_statement* defer_statement;
    struct defer_list_item* _Owner next;
};



void defer_list_add(struct defer_list* list, struct defer_list_item* _Owner p_item);
void defer_list_destroy(_Dtor struct defer_list* p);

struct try_statement
{
    /*
      try-statement: (cake- extension)
       "try" secondary-block
       "try" secondary-block "catch" secondary-block
    */

    /*
      __try: (msvc extension)
       "__try" secondary-block
       "__finally" secondary-block
       "__except(expression)" secondary-block
    */
    struct secondary_block* _Owner secondary_block;
    struct secondary_block* _Owner catch_secondary_block_opt;
    struct token* _Nonnull first_token; /*try*/
    struct token* last_token;
    struct token* catch_token_opt; /*catch*/

    struct expression* _Owner msvc_except_expression;
    int catch_label_id;
};

struct try_statement* _Owner try_statement(struct parser_ctx* ctx);
void try_statement_delete(_Dtor struct try_statement* _Owner _Nullable p);

struct asm_statement
{
    struct token* _Nonnull p_first_token;
    struct token* _Nonnull p_last_token;
};

struct asm_statement* _Owner asm_statement(struct parser_ctx* ctx);

void asm_statement_delete(_Dtor struct asm_statement* _Owner _Nullable p);

struct case_label_list
{
    /*
       it is not the owner. The owner is the label statement
    */
    struct label* head;
    struct label* tail;
};

void case_label_list_push(struct case_label_list* list, struct label* pnew);
struct label* case_label_list_find(const struct parser_ctx* ctx, struct case_label_list* list, const struct object* object);
const struct label* case_label_list_find_const(const struct parser_ctx* ctx, const struct case_label_list* list, const struct object* object);
struct label* case_label_list_find_default( struct case_label_list* list);
const struct label* case_label_list_find_default_const(const struct case_label_list* list);
struct label* case_label_list_find_range(const struct parser_ctx* ctx, struct case_label_list* list, const struct object* begin, const struct object* end);
const struct label* case_label_list_find_range_const(const struct parser_ctx* ctx, const struct case_label_list* list, const struct object* begin, const struct object* end);

struct selection_statement
{
    /*
      selection-statement:
        "if" ( expression ) secondary-block
        "if" ( expression ) secondary-block "else" secondary-block
        "switch" ( expression ) secondary-block
    */

    //TODO rename
    /*
        C2Y
         selection-statement:
            if ( selection-header ) secondary-block
            if ( selection-header ) secondary-block else secondary-block
            switch ( selection-header ) secondary-block

        selection-header:
          expression
          declaration expression
          simple-declaration


        simple-declaration:
           attribute-specifier-sequenceopt declaration-specifiers declarator = initializer
    */

    struct init_statement* _Owner p_init_statement;
    struct condition* _Owner condition;

    /* if (T x = ...): the declarator as the condition expression, created by flow; maps key on its object */
    struct expression* _Owner p_hidden_expression;

    struct secondary_block* _Owner secondary_block;
    struct secondary_block* _Owner else_secondary_block_opt;

    struct token* _Nonnull open_parentesis_token;
    struct token* _Nonnull close_parentesis_token;

    //case labels inside the switch
    struct case_label_list label_list;

    struct token* _Nonnull first_token;
    struct token* last_token;
    struct token* lint_token;
    struct token* else_token_opt;
    struct defer_list defer_list;

    int label_id;
};

struct selection_statement* _Owner selection_statement(struct parser_ctx* ctx);
void selection_statement_delete(_Dtor struct selection_statement* _Owner _Nullable  p);

struct iteration_statement
{
    /*
      iteration-statement:
        "while" ( expression ) secondary-block
        "do" secondary-block "while" ( expression ) ;
        "for" ( expression _Nullable ; expression _Nullable ; expression _Nullable ) secondary-block
        "for" ( declaration expression _Nullable ; expression _Nullable ) secondary-block
    */

    struct token* _Nonnull first_token;
    struct token* second_token; /*do {} while*/
    struct token* p_lint_token; /*do {} while*/
    struct secondary_block* _Owner secondary_block;

    struct expression* _Owner expression1;
    struct expression* _Owner expression2;
    struct expression* _Owner expression0;
    struct declaration* _Owner declaration;
    struct defer_list defer_list;
};

struct iteration_statement* _Owner iteration_statement(struct parser_ctx* ctx);
void iteration_statement_delete(_Dtor struct iteration_statement* _Owner _Nullable p);

struct jump_statement
{
    /*
     jump-statement:
       "goto" identifier ;
       "continue" ;
       "break" ;
       "return" expression _Nullable ;
    */

    struct token* label;
    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
    struct token* p_lint_token;

    struct expression* _Owner expression_opt;

    int label_id;
    struct defer_list defer_list;
};

struct jump_statement* _Owner jump_statement(struct parser_ctx* ctx);
void jump_statement_delete(_Dtor struct jump_statement* _Owner _Nullable p);

struct expression_statement
{
    /*
     expression-statement:
       expression _Nullable;
       attribute-specifier-sequence expression ;
    */

    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;
    struct expression* _Owner expression_opt;
    struct token* p_lint_token;
};

struct expression_statement* _Owner expression_statement(struct parser_ctx* ctx, bool ignore_semicolon, struct attribute_specifier_sequence* _Owner _Nullable p_attribute_specifier_sequence);
void expression_statement_delete(_Dtor struct expression_statement* _Owner _Nullable  p);

struct block_item
{
    /*
     block-item:
      declaration
      unlabeled-statement
      label
    */
    struct token* _Nonnull first_token; //?necessary
    struct declaration* _Owner declaration;
    /* not owner, points to a declarator owned by its init_declarator */
    struct declarator* declarator;
    struct unlabeled_statement* _Owner unlabeled_statement;
    struct label* _Owner label;

    struct block_item* _Owner next;
};

struct block_item* _Owner block_item(struct parser_ctx* ctx);
void block_item_delete(_Dtor struct block_item* _Owner _Nullable p);

struct compound_statement* _Owner function_body(struct parser_ctx* ctx);

struct designator
{
    /*
     designator:
       [ constant-expression ]
       . identifier
    */
    struct expression* _Owner constant_expression_opt;
    struct token* token; /* the identifier of `.identifier`, NULL for `[ ]` */
    struct designator* _Owner next;
};

struct designator* _Owner designator(struct parser_ctx* ctx);
void designator_delete(_Dtor struct designator* _Owner _Nullable p);

struct initializer_list_item
{
    //A small grammar rewrite    
    /*
      initializer-list:
        designation opt initializer
        initializer-list , designation opt initializer
    */

    /*
      initializer-list-item:
        designation opt initializer

      initializer-list:
        initializer-list-item
        initializer-list , initializer-list-item
     */

    struct designation* _Owner designation;
    struct initializer* _Nonnull _Owner initializer;
    struct initializer_list_item* _Owner next;
};

void initializer_list_item_delete(_Dtor struct initializer_list_item* _Owner _Nullable p);

struct initializer_list
{
    /*
      initializer-list:
        designation opt initializer
        initializer-list , designation opt initializer
    */
    struct token* _Nonnull first_token;
    struct initializer_list_item* _Owner head;
    struct initializer_list_item* tail;
    int size;
};

struct initializer_list* _Owner initializer_list(struct parser_ctx* ctx, bool is_discarded);
void initializer_list_delete(_Dtor struct initializer_list* _Owner _Nullable p);
void initializer_list_add(struct initializer_list* list, struct initializer_list_item* _Owner p_item);

struct primary_block
{
    /*
       primary-block:
         compound-statement
         selection-statement
         iteration-statement
         try-statement (extension)
         gcc_asm_statement
    */

    struct compound_statement* _Owner compound_statement;
    struct selection_statement* _Owner selection_statement;
    struct iteration_statement* _Owner iteration_statement;
    struct try_statement* _Owner try_statement;
    struct asm_statement* _Owner asm_statement;
};

void primary_block_delete(_Dtor struct primary_block* _Owner _Nullable p);

struct secondary_block
{
    /*
     secondary-block:
       statement
    */
    struct token* _Nonnull first_token;
    struct token* _Nonnull last_token;
    struct statement* _Nonnull _Owner statement;
};

void secondary_block_delete(_Dtor struct secondary_block* _Owner _Nullable p);
bool secondary_block_ends_with_jump(const struct secondary_block* _Nullable p_secondary_block);

struct unlabeled_statement
{
    /*
      unlabeled-statement:
        expression-statement
        attribute-specifier-sequence opt primary-block
        attribute-specifier-sequence opt jump-statement
     */
    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;
    struct expression_statement* _Owner expression_statement;
    struct primary_block* _Owner primary_block;
    struct jump_statement* _Owner jump_statement;
    struct defer_statement* _Owner defer_statement;
};

struct unlabeled_statement* _Owner unlabeled_statement(struct parser_ctx* ctx, struct attribute_specifier_sequence* _Owner _Nullable p_attribute_specifier_sequence);
void unlabeled_statement_delete(_Dtor struct unlabeled_statement* _Owner _Nullable p);

struct labeled_statement
{
    /*
     label statement:
       label statement
    */
    struct label* _Nonnull _Owner label;
    struct statement* _Nonnull _Owner statement;
};

struct labeled_statement* _Owner labeled_statement(struct parser_ctx* ctx, struct attribute_specifier_sequence* _Owner _Nullable p_attribute_specifier_sequence);
void labeled_statement_delete(_Dtor struct labeled_statement* _Owner _Nullable p);

struct statement
{
    /*
     statement:
       labeled-statement
       unlabeled-statement
    */
    struct labeled_statement* _Owner labeled_statement;
    struct unlabeled_statement* _Owner unlabeled_statement;
};

struct statement* _Owner statement(struct parser_ctx* ctx, struct attribute_specifier_sequence* _Owner _Nullable p_attribute_specifier_sequence);
void statement_delete(_Dtor struct statement* _Owner _Nullable  p);

struct designator_list
{
    /*
     designator-list:
       designator
       designator-list designator
    */
    struct designator* _Owner head;
    struct designator* tail;
};

struct designator_list* _Owner designator_list(struct parser_ctx* ctx);
void designator_list_delete(_Dtor struct designator_list* _Owner _Nullable p);
void designator_list_add(struct designator_list* list, struct designator* _Owner p_item);

struct designation
{
    /*
     designation:
       designator-list =
    */
    struct designator_list* _Nonnull _Owner designator_list;
};

struct designation* _Owner designation(struct parser_ctx* ctx);
void designation_delete(_Dtor struct designation* _Owner _Nullable p);

struct type_qualifier_list
{
    /*
     type-qualifier-list:
       type-qualifier
       type-qualifier-list type-qualifier
    */
    enum type_qualifier_flags flags;
    struct type_qualifier* _Owner head;
    struct type_qualifier* tail;
};

struct type_qualifier_list* _Owner type_qualifier_list(struct parser_ctx* ctx);
void type_qualifier_list_delete(_Dtor struct type_qualifier_list* _Owner _Nullable p);
void type_qualifier_list_add(struct type_qualifier_list* list, struct type_qualifier* _Owner p_item);



struct attribute
{
    /*
    attribute:
      attribute-token attribute-argument-clause opt

    attribute-token attribute-argument-clause opt
      attribute-token:
      standard-attribute
      attribute-prefixed-token

    standard-attribute:
      identifier

    attribute-prefixed-token:
      attribute-prefix :: identifier

    attribute-prefix:
      identifier
    */

    enum msvc_declspec_flags msvc_declspec_flags;
    enum attribute_flags  attributes_flags;
    struct attribute_argument_clause* _Owner attribute_argument_clause;
    struct token* attribute_token;
    struct token* attribute_prefix;
    struct attribute* _Owner next;
};

void attribute_delete(_Dtor struct attribute* _Owner _Nullable p);

struct attribute_list
{
    enum attribute_flags  attributes_flags;
    struct attribute* _Owner head;
    struct attribute* tail;
};

struct attribute_list* _Owner attribute_list(struct parser_ctx* ctx);
void attribute_list_destroy(_Dtor struct attribute_list* p);
void attribute_list_delete(_Dtor struct attribute_list* _Owner _Nullable p);

void attribute_list_add(struct attribute_list* list, struct attribute* _Owner p_item);

struct enumerator
{
    /*
      enumeration-constant:
        identifier

      enumerator:
        enumeration-constant attribute-specifier-sequence opt
        enumeration-constant attribute-specifier-sequence opt = constant-expression
    */

    /*
       If has_shared_ownership is
        - true,  both AST and some map are sharing the ownership
        - false, only AST OR and some map have the ownership
    */
    bool has_shared_ownership;
    bool used; /* -unused-extern-report */

    struct token* _Nonnull token;
    struct attribute_specifier_sequence* _Owner attribute_specifier_sequence_opt;

    struct expression* _Owner constant_expression_opt;

    /*
      having the enum specifier we have better information about the type
    */
    const struct enum_specifier* _Nonnull enum_specifier;

    struct enumerator* _Owner next;
    struct object value;
};

struct enumerator* _Owner enumerator(struct parser_ctx* ctx, const struct enum_specifier* p_enum_specifier, const struct enum_specifier* _Nullable prev_decl_same_scope, struct object* p_enumerator_value, struct integer128* min_value, struct integer128* max_value, bool* next_ovf);
struct enumerator* _Nonnull _Owner enumerator_add_ref(struct enumerator* p);
void enumerator_delete(_Dtor struct enumerator* _Owner _Nullable  p);

struct attribute_argument_clause
{
    /*
     attribute-argument-clause:
       ( balanced-token-sequence opt )
    */
    struct balanced_token_sequence* _Owner p_balanced_token_sequence;
    struct token* token;
};

struct attribute_argument_clause* _Owner attribute_argument_clause(struct parser_ctx* ctx);
void attribute_argument_clause_delete(_Dtor struct attribute_argument_clause* _Owner _Nullable p);

bool first_of_attribute(const struct parser_ctx* ctx);

struct balanced_token
{
    struct token* _Nonnull token;
    struct balanced_token* _Owner next;
};

struct balanced_token_sequence
{
    struct balanced_token* _Owner head;
    struct balanced_token* tail;
};

struct balanced_token_sequence* _Owner balanced_token_sequence_opt(struct parser_ctx* ctx);
void balanced_token_sequence_delete(_Dtor struct balanced_token_sequence* _Owner _Nullable  p);

bool first_of_type_name(const struct parser_ctx* ctx);
bool first_of_type_name_ahead(const struct parser_ctx* ctx);
bool first_of_type_name_token(const struct parser_ctx* ctx /*only to typedef*/, struct token* p_token);

struct argument_expression_list argument_expression_list(struct parser_ctx* ctx, bool is_discarded);

struct declaration_list
{
    struct declaration* _Owner head;
    struct declaration* tail;
};

struct declaration_list translation_unit(struct parser_ctx* ctx, bool* berror);
void declaration_list_destroy(_Dtor struct declaration_list* list);

/* reports and then clears the list */
void global_unused_functions_report(_Clear struct global_unused_list* p, const struct options* options, struct report* report);


struct label
{
    /*
     label:
       attribute-specifier-sequence opt identifier :
       attribute-specifier-sequence opt "case" constant-expression :
       attribute-specifier-sequence opt "default" :
    */
    struct attribute_specifier_sequence* _Owner p_attribute_specifier_sequence;
    struct expression* _Owner constant_expression;
    struct expression* _Owner constant_expression_end;

    struct token* p_identifier_opt;
    struct token* _Nonnull p_first_token;
    struct label* next;

    int label_id; //unique id inside the function scope
};

struct label* _Owner label(struct parser_ctx* ctx, struct attribute_specifier_sequence* _Owner _Nullable p_attribute_specifier_sequence);
void label_delete(_Dtor struct label* _Owner _Nullable p);

struct ast
{
    struct token_list token_list;
    struct declaration_list declaration_list;

    /* tags and variables from file scope */
    struct scope file_scope;
};


struct ast get_ast(const struct options* options,
    const char* filename,
    const char* source,
    struct report* report);
    
void ast_destroy(_Dtor struct ast* ast);

struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);

struct declaration_list parse(struct parser_ctx* ctx, struct token_list* list, struct scope* _Nullable p_file_scope_out, bool* berror);

int initializer_init_new(struct parser_ctx* ctx,
                         struct type* p_current_object_type,
                         struct object* p_current_object,
                         struct initializer* braced_initializer,
                         bool is_constant,
                         bool requires_constant_initialization);

struct object* find_object_declarator_by_index(const struct object* p_object, struct member_declaration_list* list, int member_index);

void check_dianostic_suppression_phase(struct parser_ctx* ctx, const struct token* p_token, int phase);
const struct direct_declarator* get_innermost_direct_declarator(const struct direct_declarator* _Nullable p);

