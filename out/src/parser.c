struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int try_catch_block_index;
};

struct param_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct param * head;
    struct param * tail;
};

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct defer_list {
    struct defer_list_item * head;
    struct defer_list_item * tail;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct type {
    int   category;
    int   attributes_flags;
    int   type_specifier_flags;
    int   type_qualifier_flags;
    int   storage_class_specifier_flags;
    char * name_opt;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct enum_specifier * enum_specifier;
    struct expression * array_num_elements_expression;
    int num_of_elements;
    unsigned char   static_array;
    unsigned char   address_of;
    struct param_list  params;
    struct type * next;
};

struct find_object_result {
    struct object * object;
    struct type  type;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct label {
    struct expression * constant_expression;
    struct token * p_identifier_opt;
    struct token * p_first_token;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct declaration_list {
    struct declaration * head;
    struct declaration * tail;
};

struct ast {
    struct token_list  token_list;
    struct declaration_list  declaration_list;
};

struct flow_objects {
    struct flow_object ** data;
    int size;
    int capacity;
};

struct label_state {
    char * label_name;
    int state_number;
};

struct flow_visit_ctx {
    struct secondary_block * catch_secondary_block_opt;
    struct parser_ctx * ctx;
    struct ast  ast;
    struct type * p_return_type;
    int parameter_list;
    int state_number_generator;
    unsigned char   expression_is_not_evaluated;
    unsigned char   inside_assert;
    int throw_join_state;
    int break_join_state;
    int initial_state;
    struct flow_objects  arena;
    struct label_state labels[100];
    int labels_size;
};

struct direct_declarator {
    struct token * name_opt;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct scope {
    int scope_level;
    struct hash_map  tags;
    struct hash_map  variables;
    struct scope * next;
    struct scope * previous;
};

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

union _struct_tag_6 {
    unsigned char   bool_value;
    signed char signed_char_value;
    unsigned char unsigned_char_value;
    signed short signed_short_value;
    unsigned short unsigned_short_value;
    signed int signed_int_value;
    unsigned int unsigned_int_value;
    signed long signed_long_value;
    unsigned long unsigned_long_value;
    signed long long signed_long_long_value;
    unsigned long long unsigned_long_long_value;
    float float_value;
    double double_value;
    long double long_double_value;
};

struct TAGDIR;
struct diagnostic_stack {
    int top_index;
    struct diagnostic stack[10];
};

struct options {
    int   input;
    int   target;
    struct diagnostic_stack  diagnostic_stack;
    int   style;
    unsigned char   show_includes;
    unsigned char   disable_assert;
    unsigned char   flow_analysis;
    unsigned char   test_mode;
    unsigned char   null_checks_enabled;
    unsigned char   ownership_enabled;
    unsigned char   preprocess_only;
    unsigned char   clear_error_at_end;
    unsigned char   sarif_output;
    unsigned char   no_output;
    unsigned char   visual_studio_ouput_format;
    unsigned char   dump_tokens;
    unsigned char   dump_pptokens;
    unsigned char   auto_config;
    char output[200];
    char sarifpath[200];
};

struct include_dir_list {
    struct include_dir * head;
    struct include_dir * tail;
};

struct preprocessor_ctx {
    struct options  options;
    int   flags;
    struct hash_map  macros;
    struct include_dir_list  include_dir;
    struct hash_map  pragma_once_map;
    struct token * current;
    struct token_list  input_list;
    unsigned int count_macro_value;
    unsigned char   conditional_inclusion;
    int n_warnings;
    int n_errors;
};

struct flow_objects_view {
    struct flow_object ** data;
    int size;
    int capacity;
};

struct flow_object_state {
    char * dbg_name;
    int state_number;
    struct flow_object * pointed;
    int   state;
    struct flow_objects_view  alternatives;
    struct flow_object_state * next;
};

struct flow_object {
    unsigned int visit_number;
    struct flow_object * parent;
    struct declarator * p_declarator_origin;
    struct expression * p_expression_origin;
    struct flow_objects_view  members;
    struct flow_object_state  current;
    int id;
    unsigned char   is_temporary;
};

struct tokenizer_ctx {
    struct options  options;
    int n_warnings;
    int n_errors;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct struct_entry;
union _struct_tag_5 {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct macro0;
struct d_visit_ctx {
    int indentation;
    int locals_count;
    int extern_count;
    int tag_name_count;
    struct hash_map  tag_names;
    struct hash_map  structs_map;
    struct hash_map  function_map;
    struct osstream  add_this_before;
    struct osstream  add_this_before_external_decl;
    struct osstream  data_types;
    struct osstream  function_types;
    struct ast  ast;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct object {
    int   state;
    int   value_type;
    struct type  type2;
    char * debug_name;
    union _struct_tag_6  value;
    struct object * parent;
    struct expression * p_init_expression;
    struct object * members;
    struct object * next;
};

struct declarator {
    unsigned char   has_shared_ownership;
    struct token * first_token_opt;
    struct token * last_token_opt;
    struct pointer * pointer;
    struct direct_declarator * direct_declarator;
    struct declaration_specifiers * declaration_specifiers;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct token * name_opt;
    struct compound_statement * function_body;
    int num_uses;
    struct object  object;
    struct flow_object * p_object;
    struct type  type;
};

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
};

struct init_declarator_list {
    struct init_declarator * head;
    struct init_declarator * tail;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct attribute_token {
    int   attributes_flags;
    struct token * token;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct type_specifier {
    int   flags;
    struct token * token;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct typeof_specifier * typeof_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct atomic_type_specifier * atomic_type_specifier;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct hash_item_set {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro0 * p_macro;
    struct struct_entry1 * p_struct_entry;
};

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct defer_visit_ctx {
    struct secondary_block * catch_secondary_block_opt;
    struct parser_ctx * ctx;
    struct ast  ast;
    struct defer_defer_scope * tail_block;
    int parameter_list;
};

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int   attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct dirent {
    unsigned short d_ino;
    long d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};

struct macro;
struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct pointer * pointer;
};

struct switch_value_list {
    struct switch_value * head;
    struct switch_value * tail;
    struct switch_value * p_default;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct struct_or_union_specifier {
    unsigned char   has_shared_ownership;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct member_declaration_list  member_declaration_list;
    struct token * first_token;
    struct token * last_token;
    unsigned char   is_owner;
    struct token * tagtoken;
    char tag_name[200];
    unsigned char   has_anonymous_tag;
    unsigned char   show_anonymous_tag;
    int scope_level;
    int visit_moved;
    struct struct_or_union_specifier * complete_struct_or_union_specifier_indirection;
};

struct __crt_multibyte_data;
struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct storage_class_specifier {
    int   flags;
    struct token * token;
};

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct type_qualifier {
    int   flags;
    struct token * token;
    struct type_qualifier * next;
};

struct param {
    struct type  type;
    struct param * next;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct enum_specifier {
    unsigned char   has_shared_ownership;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct specifier_qualifier_list * specifier_qualifier_list;
    char tag_name[200];
    struct enumerator_list  enumerator_list;
    struct token * tag_token;
    struct token * first_token;
    struct enum_specifier * complete_enum_specifier2;
};

struct __crt_locale_data;
struct defer_defer_scope;
struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct struct_entry1;
struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct include_dir {
    char * path;
    struct include_dir * next;
};

struct parser_ctx {
    struct options  options;
    struct scope_list  scopes;
    struct declaration * p_current_function_opt;
    struct try_statement * p_current_try_statement_opt;
    struct selection_statement * p_current_selection_statement;
    struct switch_value_list * p_switch_value_list;
    struct _iobuf * sarif_file;
    unsigned int sarif_entries;
    struct token_list  input_list;
    struct token * current;
    struct token * previous;
    int try_catch_block_index;
    unsigned char   evaluation_is_disabled;
    unsigned char   inside_generic_association;
    struct report * p_report;
};

struct marker {
    char * file;
    int line;
    int start_col;
    int end_col;
    struct token * p_token_caret;
    struct token * p_token_begin;
    struct token * p_token_end;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct iteration_statement {
    struct token * first_token;
    struct token * second_token;
    struct secondary_block * secondary_block;
    struct expression * expression1;
    struct expression * expression2;
    struct expression * expression0;
    struct declaration * declaration;
    struct defer_list  defer_list;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct expression {
    int   expression_type;
    struct type  type;
    struct object  object;
    struct type_name * type_name;
    struct braced_initializer * braced_initializer;
    struct compound_statement * compound_statement;
    struct generic_selection * generic_selection;
    struct token * first_token;
    struct token * last_token;
    struct declarator * declarator;
    int member_index;
    struct argument_expression_list  argument_expression_list;
    struct expression * condition_expr;
    struct expression * left;
    struct expression * right;
    unsigned char   is_assignment_expression;
};

struct switch_value {
    long long value;
    struct label * p_label;
    struct switch_value * next;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct declaration_specifiers * declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct compound_statement * function_body;
    struct defer_list  defer_list;
    struct declarator * contract_declarator;
    struct token * first_token;
    struct token * last_token;
    struct declaration * next;
};

struct enumerator {
    unsigned char   has_shared_ownership;
    struct token * token;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct expression * constant_expression_opt;
    struct enum_specifier * enum_specifier;
    struct enumerator * next;
    struct object  value;
};

struct token {
    int   type;
    char * lexeme;
    char * original;
    int line;
    int col;
    int level;
    int   flags;
    struct token * token_origin;
    struct token * next;
    struct token * prev;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
};

struct selection_statement {
    struct init_statement * p_init_statement;
    struct condition * condition;
    struct secondary_block * secondary_block;
    struct secondary_block * else_secondary_block_opt;
    struct token * open_parentesis_token;
    struct token * close_parentesis_token;
    struct token * first_token;
    struct token * last_token;
    struct token * else_token_opt;
    struct defer_list  defer_list;
};

struct declaration_specifiers {
    int   attributes_flags;
    int   type_specifier_flags;
    int   type_qualifier_flags;
    int   storage_class_specifier_flags;
    int   function_specifier_flags;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct typeof_specifier * typeof_specifier;
    struct token * first_token;
    struct token * last_token;
    struct declaration_specifier * head;
    struct declaration_specifier * tail;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct generic_selection {
    struct expression * expression;
    struct type_name * type_name;
    struct expression * p_view_selected_expression;
    struct generic_assoc_list  generic_assoc_list;
    struct token * first_token;
    struct token * last_token;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int try_catch_block_index;
    struct defer_list  defer_list;
};

struct specifier_qualifier_list {
    int   type_specifier_flags;
    int   type_qualifier_flags;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct enum_specifier * enum_specifier;
    struct typeof_specifier * typeof_specifier;
    struct declarator * typedef_declarator;
    struct type_specifier_qualifier * head;
    struct type_specifier_qualifier * tail;
    struct token * first_token;
    struct token * last_token;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
};

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int   type;
    union _struct_tag_5  data;
};

struct report {
    int no_files;
    double cpu_time_used_sec;
    int error_count;
    int warnings_count;
    int info_count;
    unsigned char   test_mode;
    int test_failed;
    int test_succeeded;
    int  last_diagnostics_ids[2];
    int   fatal_error_expected;
    unsigned char   ignore_this_report;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct function_specifier {
    int   flags;
    struct token * token;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct _iobuf {
    void * _Placeholder;
};


void secondary_block_delete(struct secondary_block * p);
void free(void * ptr);

void defer_statement_delete(struct defer_statement * p)
{
    if (p)
    {
        secondary_block_delete(p->secondary_block);
        free(p);
    }
}

static int s_anonymous_struct_count = 0;
static unsigned char  parser_is_diagnostic_enabled(struct parser_ctx * ctx, int   w)
{
    if (w > 63)
    {
        return 1;
    }
    return ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors & w) != 0) || ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings & w) != 0) || ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes & w) != 0);
}

unsigned char  compiler_diagnostic_message(int   w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);

static void check_open_brace_style(struct parser_ctx * ctx, struct token * token)
{
    if (token->level == 0 &&  !(token->flags & 2) && token->type == 123 && token->prev && parser_is_diagnostic_enabled(ctx, 10))
    {
        if (ctx->options.style == 0)
        {
            if (token->prev->type == 143 && token->prev->prev && token->prev->prev->type == 10)
            {
            }
            else
            {
                compiler_diagnostic_message(10, ctx, token, ((void *)0), "not following correct brace style {");
            }
        }
    }
}

static void check_close_brace_style(struct parser_ctx * ctx, struct token * token)
{
    if (token->level == 0 &&  !(token->flags & 2) && token->type == 125 && token->prev && token->prev->prev && parser_is_diagnostic_enabled(ctx, 10))
    {
        if (ctx->options.style == 0)
        {
            if (token->prev->type == 143 && token->prev->prev->type == 10)
            {
            }
            else
            {
                compiler_diagnostic_message(10, ctx, token, ((void *)0), "not following correct close brace style }");
            }
        }
    }
}

static void check_func_open_brace_style(struct parser_ctx * ctx, struct token * token)
{
    if (token->level == 0 &&  !(token->flags & 2) && token->type == 123 && token->prev && parser_is_diagnostic_enabled(ctx, 10))
    {
        if (ctx->options.style == 0)
        {
            if (token->prev->type == 10)
            {
            }
            else
            {
                compiler_diagnostic_message(10, ctx, token, ((void *)0), "not following correct brace style {");
            }
        }
    }
}

void hashmap_destroy(struct hash_map * map);

void scope_destroy(struct scope * p)
{
    hashmap_destroy(&p->tags);
    hashmap_destroy(&p->variables);
}

void scope_list_push(struct scope_list * list, struct scope * pnew)
{
    if (list->tail)
    {
        pnew->scope_level = list->tail->scope_level + 1;
    }
    if (list->head == ((void *)0))
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        ;
        pnew->previous = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
}

void scope_list_pop(struct scope_list * list)
{
    if (list->head == ((void *)0))
    {
        return;
    }
    ;
    struct scope * p = list->tail;
    if (list->head == list->tail)
    {
        list->head = ((void *)0);
        list->tail = ((void *)0);
    }
    else
    {
        list->tail = list->tail->previous;
        if (list->tail == list->head)
        {
            ;
            list->tail->next = ((void *)0);
            list->tail->previous = ((void *)0);
        }
    }
    p->next = ((void *)0);
    p->previous = ((void *)0);
}

void switch_value_destroy(struct switch_value_list * p)
{
    struct switch_value * item = p->head;
    while (item)
    {
        struct switch_value * next = item->next;
        item->next = ((void *)0);
        free(item);
        item = next;
    }
    if (p->p_default)
    {
        ;
        free(p->p_default);
    }
}

void switch_value_list_push(struct switch_value_list * list, struct switch_value * pnew)
{
    if (list->head == ((void *)0))
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        ;
        ;
        list->tail->next = pnew;
        list->tail = pnew;
    }
}

struct switch_value *switch_value_list_find(struct switch_value_list * list, long long value)
{
    struct switch_value * p = list->head;
    while (p)
    {
        if (p->value == value)
        {
            return p;
        }
        p = p->next;
    }
    return ((void *)0);
}

int fclose(struct _iobuf * _Stream);

void parser_ctx_destroy(struct parser_ctx * ctx)
{
    if (ctx->sarif_file)
    {
        fclose(ctx->sarif_file);
    }
}

static void stringfy(char * input, char * json_str_message, int output_size)
{
    json_str_message[0] = 0;
    int k = 0;
    while ( *input != 0)
    {
        if ( *input == 34)
        {
            if (k < output_size)
            {
                json_str_message[k] = 92;
            }
            k++;
            if (k < output_size)
            {
                json_str_message[k] = 34;
            }
            k++;
            input++;
        }
        else
        {
            if ( *input == 10)
            {
                if (k < output_size)
                {
                    json_str_message[k] = 92;
                }
                k++;
                if (k < output_size)
                {
                    json_str_message[k] = 110;
                }
                k++;
                input++;
            }
            else
            {
                if (k < output_size)
                {
                    json_str_message[k] =  *input;
                }
                k++;
                input++;
            }
        }
    }
    if (k < output_size)
    {
        json_str_message[k] = 0;
    }
    else
    {
        json_str_message[output_size - 1] = 0;
    }
}

unsigned char  is_diagnostic_configurable(int   id);
unsigned char  is_diagnostic_note(int   id);
unsigned char  is_diagnostic_error(int   id);
unsigned char  is_diagnostic_warning(int   id);
int get_warning_name(int   w, int n, char buffer[]);
void print_position(char * path, int line, int col, unsigned char   msvc_format);

int __stdio_common_vsprintf(unsigned __int64 _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

inline unsigned __int64 *__local_stdio_printf_options(void)
{
    static unsigned __int64 _OptionsStorage;
    return &_OptionsStorage;
}
inline int vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result = __stdio_common_vsprintf(( *__local_stdio_printf_options()) | (1ULL << 1), _Buffer, _BufferCount, _Format, ((void *)0), _ArgList);
    return _Result < 0 ?  -1 : _Result;
}


int __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
inline int _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf(( *__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}
struct _iobuf *__acrt_iob_func(unsigned int _Ix);
inline int printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;
    ((void)(_ArgList = (char *)(&(_Format)) + ((sizeof (_Format) + sizeof (int) - 1) &  ~(sizeof (int) - 1))));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, ((void *)0), _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}
void print_line_and_token(struct marker * p_marker, unsigned char   visual_studio_ouput_format);

inline int fprintf(struct _iobuf * _Stream, char * _Format, ...)
{
    int _Result;
    char * _ArgList;
    ((void)(_ArgList = (char *)(&(_Format)) + ((sizeof (_Format) + sizeof (int) - 1) &  ~(sizeof (int) - 1))));
    _Result = _vfprintf_l(_Stream, _Format, ((void *)0), _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

unsigned char  compiler_diagnostic_message(int   w, struct parser_ctx * ctx, struct token * p_token_opt, struct marker * p_marker_temp, char * fmt, ...)
{
    unsigned char   included_file_location = 0;
    struct marker  marker = {0};
    if (p_marker_temp == ((void *)0))
    {
        if (p_token_opt == ((void *)0))
        {
            return 0;
        }
        marker.file = p_token_opt->token_origin->lexeme;
        marker.line = p_token_opt->line;
        marker.start_col = p_token_opt->col;
        marker.end_col = p_token_opt->col;
        marker.p_token_caret = p_token_opt;
        included_file_location = p_token_opt->level > 0;
    }
    else
    {
        marker =  *p_marker_temp;
        if (marker.p_token_caret)
        {
            p_token_opt = marker.p_token_caret;
        }
        else
        {
            if (marker.p_token_begin)
            {
                p_token_opt = marker.p_token_begin;
            }
        }
        if (p_token_opt == ((void *)0))
        {
            return 0;
        }
        marker.file = p_token_opt->token_origin->lexeme;
        included_file_location = p_token_opt->level > 0;
        marker.line = p_token_opt->line;
        marker.start_col = p_token_opt->col;
        marker.end_col = p_token_opt->col;
    }
    unsigned char   is_error = 0;
    unsigned char   is_warning = 0;
    unsigned char   is_note = 0;
    if (is_diagnostic_configurable(w))
    {
        is_error = (ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors & (1ULL << w)) != 0;
        is_warning = (ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings & (1ULL << w)) != 0;
        is_note = ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes & (1ULL << w)) != 0);
    }
    else
    {
        is_note = is_diagnostic_note(w);
        is_error = is_diagnostic_error(w);
        is_warning = is_diagnostic_warning(w);
    }
    if (is_error)
    {
        ctx->p_report->error_count++;
    }
    else
    {
        if (is_warning)
        {
            if (included_file_location)
            {
                return 0;
            }
            ctx->p_report->warnings_count++;
        }
        else
        {
            if (is_note)
            {
                if (included_file_location)
                {
                    return 0;
                }
                if (w != 62)
                {
                    ctx->p_report->info_count++;
                }
            }
            else
            {
                return 0;
            }
        }
    }
    if (w != 62)
    {
        ctx->p_report->last_diagnostics_ids[1] = ctx->p_report->last_diagnostics_ids[0];
        ctx->p_report->last_diagnostics_ids[0] = w;
    }
    char * func_name = "module";
    if (ctx->p_current_function_opt)
    {
        ;
        ;
        ;
        func_name = ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name_opt->lexeme;
    }
    char buffer[200] = {0};
    char diagnostic_name[100] = {0};
    get_warning_name(w, sizeof diagnostic_name, diagnostic_name);
    print_position(marker.file, marker.line, marker.start_col, ctx->options.visual_studio_ouput_format);
    char * args = {0};
    ((void)(args = (char *)(&(fmt)) + ((sizeof (fmt) + sizeof (int) - 1) &  ~(sizeof (int) - 1))));
    vsnprintf(buffer, sizeof (buffer), fmt, args);
    ((void)(args = (char *)0));
    if (ctx->options.visual_studio_ouput_format)
    {
        if (is_error)
        {
            printf("error: ");
        }
        else
        {
            if (is_warning)
            {
                printf("warning: ");
            }
            else
            {
                if (is_note)
                {
                    printf("note: ");
                }
            }
        }
        printf("%s", buffer);
        printf(" [%s]\n", diagnostic_name);
    }
    else
    {
        if (is_error)
        {
            printf("\x1b[91m", buffer, diagnostic_name);
        }
        else
        {
            if (is_warning)
            {
                printf("\x1b[95m", buffer, diagnostic_name);
            }
            else
            {
                if (is_note)
                {
                    if (w == 62)
                    {
                        printf("\x1b[36;1m", buffer);
                    }
                    else
                    {
                        printf("\x1b[36;1m", buffer, diagnostic_name);
                    }
                }
            }
        }
    }
    print_line_and_token(&marker, ctx->options.visual_studio_ouput_format);
    if (ctx->sarif_file)
    {
        char json_str_message[200] = {0};
        stringfy(buffer, json_str_message, sizeof json_str_message);
        if (ctx->sarif_entries > 0)
        {
            fprintf(ctx->sarif_file, "   ,\n");
        }
        ((struct parser_ctx *)ctx)->sarif_entries++;
        fprintf(ctx->sarif_file, "   {\n");
        fprintf(ctx->sarif_file, "     \"ruleId\":\"%s\",\n", diagnostic_name);
        if (is_error)
        {
            fprintf(ctx->sarif_file, "     \"level\":\"error\",\n");
        }
        else
        {
            if (is_warning)
            {
                fprintf(ctx->sarif_file, "     \"level\":\"warning\",\n");
            }
            else
            {
                if (is_note)
                {
                    fprintf(ctx->sarif_file, "     \"level\":\"note\",\n");
                }
            }
        }
        fprintf(ctx->sarif_file, "     \"message\": {\n");
        fprintf(ctx->sarif_file, "            \"text\": \"%s\"\n", json_str_message);
        fprintf(ctx->sarif_file, "      },\n");
        fprintf(ctx->sarif_file, "      \"locations\": [\n");
        fprintf(ctx->sarif_file, "       {\n");
        fprintf(ctx->sarif_file, "       \"physicalLocation\": {\n");
        fprintf(ctx->sarif_file, "             \"artifactLocation\": {\n");
        fprintf(ctx->sarif_file, "                 \"uri\": \"file:///%s\"\n", marker.file);
        fprintf(ctx->sarif_file, "              },\n");
        fprintf(ctx->sarif_file, "              \"region\": {\n");
        fprintf(ctx->sarif_file, "                  \"startLine\": %d,\n", marker.line);
        fprintf(ctx->sarif_file, "                  \"startColumn\": %d,\n", marker.start_col);
        fprintf(ctx->sarif_file, "                  \"endLine\": %d,\n", marker.line);
        fprintf(ctx->sarif_file, "                  \"endColumn\": %d\n", marker.end_col);
        fprintf(ctx->sarif_file, "               }\n");
        fprintf(ctx->sarif_file, "         },\n");
        fprintf(ctx->sarif_file, "         \"logicalLocations\": [\n");
        fprintf(ctx->sarif_file, "          {\n");
        fprintf(ctx->sarif_file, "              \"fullyQualifiedName\": \"%s\",\n", func_name);
        fprintf(ctx->sarif_file, "              \"decoratedName\": \"%s\",\n", func_name);
        fprintf(ctx->sarif_file, "              \"kind\": \"%s\"\n", "function");
        fprintf(ctx->sarif_file, "          }\n");
        fprintf(ctx->sarif_file, "         ]\n");
        fprintf(ctx->sarif_file, "       }\n");
        fprintf(ctx->sarif_file, "     ]\n");
        fprintf(ctx->sarif_file, "   }\n");
    }
    return 1;
}

void print_scope(struct scope_list * e)
{
    printf("--- begin of scope---\n");
    struct scope * p = e->head;
    int level = 0;
    while (p)
    {
        if (p->variables.table)
        {
            {
                int i = 0;
                for (; i < p->variables.capacity; i++)
                {
                    if (p->variables.table[i])
                    {
                        {
                            int k = 0;
                            for (; k < level; k++)
                            printf(" ");
                        }
                        printf("%s\n", p->variables.table[i]->key);
                    }
                }
            }
            {
                int i = 0;
                for (; i < p->tags.capacity; i++)
                {
                    if (p->tags.table[i])
                    {
                        {
                            int k = 0;
                            for (; k < level; k++)
                            printf(" ");
                        }
                        printf("tag %s\n", p->tags.table[i]->key);
                    }
                }
            }
        }
        level++;
        p = p->next;
    }
    printf("--- end of scope---\n");
}

unsigned char  first_of_function_specifier(struct parser_ctx * ctx)
{
    struct token * token = ctx->current;
    if (token == ((void *)0))
    {
        return 0;
    }
    return token->type == 9018 || token->type == 9054;
}

unsigned char  first_of_enum_specifier_token(struct token * token)
{
    return token->type == 9012;
}

unsigned char  first_of_enum_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return first_of_enum_specifier_token(ctx->current);
}

unsigned char  first_of_alignment_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 9043;
}

struct token *parser_look_ahead(struct parser_ctx * ctx);

unsigned char  first_of_atomic_type_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    if (ctx->current->type == 9045)
    {
        struct token * ahead = parser_look_ahead(ctx);
        if (ahead != ((void *)0))
        {
            return ahead->type == 40;
        }
    }
    return 0;
}

unsigned char  first_of_storage_class_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 9035 || ctx->current->type == 9002 || ctx->current->type == 9013 || ctx->current->type == 9032 || ctx->current->type == 9057 || ctx->current->type == 8999 || ctx->current->type == 9025;
}

unsigned char  first_of_struct_or_union_token(struct token * token)
{
    return token->type == 9033 || token->type == 9038;
}

unsigned char  first_of_struct_or_union(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return first_of_struct_or_union_token(ctx->current);
}

unsigned char  first_of_type_qualifier_token(struct token * p_token)
{
    return p_token->type == 9004 || p_token->type == 9026 || p_token->type == 9041 || p_token->type == 9045 || p_token->type == 9065 || p_token->type == 9064 || p_token->type == 9066 || p_token->type == 9067 || p_token->type == 9068;
}

unsigned char  first_of_type_qualifier(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return first_of_type_qualifier_token(ctx->current);
}

struct map_entry *hashmap_find(struct hash_map * map, char * key);

struct map_entry *find_tag(struct parser_ctx * ctx, char * lexeme)
{
    struct scope * scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry * p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry)
        {
            return p_entry;
        }
        scope = scope->previous;
    }
    return ((void *)0);
}

struct map_entry *find_variables(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt)
{
    if (ppscope_opt != ((void *)0))
    {
         *ppscope_opt = ((void *)0);
    }
    struct scope * scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry * p_entry = hashmap_find(&scope->variables, lexeme);
        if (p_entry)
        {
            if (ppscope_opt)
            {
                 *ppscope_opt = scope;
            }
            return p_entry;
        }
        scope = scope->previous;
    }
    return ((void *)0);
}

struct enum_specifier *find_enum_specifier(struct parser_ctx * ctx, char * lexeme)
{
    struct enum_specifier * best = ((void *)0);
    struct scope * scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry * p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry && p_entry->type == 1)
        {
            ;
            best = p_entry->data.p_enum_specifier;
            if (best->enumerator_list.head != ((void *)0))
            {
                return best;
            }
            else
            {
            }
        }
        scope = scope->previous;
    }
    return best;
}

struct struct_or_union_specifier *find_struct_or_union_specifier(struct parser_ctx * ctx, char * lexeme)
{
    struct struct_or_union_specifier * p = ((void *)0);
    struct scope * scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry * p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry && p_entry->type == 2)
        {
            ;
            p = p_entry->data.p_struct_or_union_specifier;
            break;
        }
        scope = scope->previous;
    }
    return p;
}

struct declarator *find_declarator(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt)
{
    struct map_entry * p_entry = find_variables(ctx, lexeme, ppscope_opt);
    if (p_entry)
    {
        if (p_entry->type == 5)
        {
            ;
            struct init_declarator * p_init_declarator = p_entry->data.p_init_declarator;
            return (struct declarator *)p_init_declarator->p_declarator;
        }
        else
        {
            if (p_entry->type == 4)
            {
                return p_entry->data.p_declarator;
            }
        }
    }
    return ((void *)0);
}

struct enumerator *find_enumerator(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt)
{
    struct map_entry * p_entry = find_variables(ctx, lexeme, ppscope_opt);
    if (p_entry && p_entry->type == 3)
    {
        return p_entry->data.p_enumerator;
    }
    return ((void *)0);
}

unsigned char  first_of_typedef_name(struct parser_ctx * ctx, struct token * p_token)
{
    if (p_token->type != 8996)
    {
        return 0;
    }
    if (p_token->flags & 16)
    {
        return 1;
    }
    if (p_token->flags & 32)
    {
        return 0;
    }
    struct declarator * p_declarator = find_declarator(ctx, p_token->lexeme, ((void *)0));
    if (p_declarator && p_declarator->declaration_specifiers && (p_declarator->declaration_specifiers->storage_class_specifier_flags & 1))
    {
        p_declarator->num_uses++;
        p_token->flags = 16;
        return 1;
    }
    else
    {
        p_token->flags = 32;
    }
    return 0;
}

unsigned char  first_of_type_specifier_token(struct parser_ctx * ctx, struct token * token);

unsigned char  first_of_type_name_ahead(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    if (ctx->current->type != 40)
    {
        return 0;
    }
    struct token * token_ahead = parser_look_ahead(ctx);
    if (token_ahead == ((void *)0))
    {
        return 0;
    }
    return first_of_type_specifier_token(ctx, token_ahead) || first_of_type_qualifier_token(token_ahead);
}

unsigned char  first_of_type_specifier(struct parser_ctx * ctx);

unsigned char  first_of_type_name(struct parser_ctx * ctx)
{
    return first_of_type_specifier(ctx) || first_of_type_qualifier(ctx);
}

unsigned char  first_of_type_specifier_token(struct parser_ctx * ctx, struct token * p_token)
{
    return p_token->type == 9040 || p_token->type == 9003 || p_token->type == 9028 || p_token->type == 9019 || p_token->type == 9020 || p_token->type == 9021 || p_token->type == 9022 || p_token->type == 9023 || p_token->type == 9024 || p_token->type == 9014 || p_token->type == 9010 || p_token->type == 9029 || p_token->type == 9039 || p_token->type == 9063 || p_token->type == 9047 || p_token->type == 9048 || p_token->type == 9050 || p_token->type == 9051 || p_token->type == 9049 || p_token->type == 9058 || p_token->type == 9062 || first_of_atomic_type_specifier(ctx) || first_of_struct_or_union_token(p_token) || first_of_enum_specifier_token(p_token) || first_of_typedef_name(ctx, p_token);
}

unsigned char  first_of_type_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return first_of_type_specifier_token(ctx, ctx->current);
}

unsigned char  first_of_type_specifier_qualifier(struct parser_ctx * ctx)
{
    return first_of_type_specifier(ctx) || first_of_type_qualifier(ctx) || first_of_alignment_specifier(ctx);
}

unsigned char  first_of_compound_statement(struct parser_ctx * ctx)
{
    return ctx->current != ((void *)0) && ctx->current->type == 123;
}

unsigned char  first_of_jump_statement(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 9016 || ctx->current->type == 9005 || ctx->current->type == 9000 || ctx->current->type == 9027 || ctx->current->type == 9037;
}

unsigned char  first_of_selection_statement(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 9017 || ctx->current->type == 9034;
}

unsigned char  first_of_iteration_statement(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 9042 || ctx->current->type == 9008 || ctx->current->type == 9015;
}

unsigned char  first_of_label(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    if (ctx->current->type == 8996)
    {
        struct token * next = parser_look_ahead(ctx);
        return next && next->type == 58;
    }
    else
    {
        if (ctx->current->type == 9001)
        {
            return 1;
        }
        else
        {
            if (ctx->current->type == 9007)
            {
                return 1;
            }
        }
    }
    return 0;
}

unsigned char  first_of_declaration_specifier(struct parser_ctx * ctx)
{
    return first_of_storage_class_specifier(ctx) || first_of_function_specifier(ctx) || first_of_type_specifier_qualifier(ctx);
}

unsigned char  first_of_pragma_declaration(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 128;
}

unsigned char  first_of_static_assert_declaration(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 9055 || ctx->current->type == 9069 || ctx->current->type == 9070 || ctx->current->type == 9071 || ctx->current->type == 9072;
}

unsigned char  first_of_attribute_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    if (ctx->current->type != 91)
    {
        return 0;
    }
    struct token * p_token = parser_look_ahead(ctx);
    return p_token != ((void *)0) && p_token->type == 91;
}

unsigned char  first_of_labeled_statement(struct parser_ctx * ctx)
{
    return first_of_label(ctx);
}

unsigned char  first_of_designator(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 91 || ctx->current->type == 46;
}

struct token *previous_parser_token(struct token * token)
{
    if (token->prev == ((void *)0))
    {
        return ((void *)0);
    }
    struct token * prev = token->prev;
    while (prev &&  !(prev->flags & 1))
    {
        prev = prev->prev;
    }
    return prev;
}

int strcmp(char * _Str1, char * _Str2);

int  is_keyword(char * text)
{
    int   result = 0;
    switch (text[0])
    {
        case 97 :
        if (strcmp("alignof", text) == 0)
        {
            result = 9044;
        }
        else
        {
            if (strcmp("auto", text) == 0)
            {
                result = 8999;
            }
            else
            {
                if (strcmp("alignas", text) == 0)
                {
                    result = 9043;
                }
                else
                {
                    if (strcmp("alignof", text) == 0)
                    {
                        result = 9043;
                    }
                    else
                    {
                        if (strcmp("assert", text) == 0)
                        {
                            result = 9056;
                        }
                    }
                }
            }
        }
        break;
        case 98 :
        if (strcmp("break", text) == 0)
        {
            result = 9000;
        }
        else
        {
            if (strcmp("bool", text) == 0)
            {
                result = 9047;
            }
        }
        break;
        case 99 :
        if (strcmp("case", text) == 0)
        {
            result = 9001;
        }
        else
        {
            if (strcmp("char", text) == 0)
            {
                result = 9003;
            }
            else
            {
                if (strcmp("const", text) == 0)
                {
                    result = 9004;
                }
                else
                {
                    if (strcmp("constexpr", text) == 0)
                    {
                        result = 9002;
                    }
                    else
                    {
                        if (strcmp("continue", text) == 0)
                        {
                            result = 9005;
                        }
                        else
                        {
                            if (strcmp("catch", text) == 0)
                            {
                                result = 9006;
                            }
                        }
                    }
                }
            }
        }
        break;
        case 100 :
        if (strcmp("default", text) == 0)
        {
            result = 9007;
        }
        else
        {
            if (strcmp("do", text) == 0)
            {
                result = 9008;
            }
            else
            {
                if (strcmp("defer", text) == 0)
                {
                    result = 9009;
                }
                else
                {
                    if (strcmp("double", text) == 0)
                    {
                        result = 9010;
                    }
                }
            }
        }
        break;
        case 101 :
        if (strcmp("else", text) == 0)
        {
            result = 9011;
        }
        else
        {
            if (strcmp("enum", text) == 0)
            {
                result = 9012;
            }
            else
            {
                if (strcmp("extern", text) == 0)
                {
                    result = 9013;
                }
            }
        }
        break;
        case 102 :
        if (strcmp("float", text) == 0)
        {
            result = 9014;
        }
        else
        {
            if (strcmp("for", text) == 0)
            {
                result = 9015;
            }
            else
            {
                if (strcmp("false", text) == 0)
                {
                    result = 9060;
                }
            }
        }
        break;
        case 103 :
        if (strcmp("goto", text) == 0)
        {
            result = 9016;
        }
        break;
        case 105 :
        if (strcmp("if", text) == 0)
        {
            result = 9017;
        }
        else
        {
            if (strcmp("inline", text) == 0)
            {
                result = 9018;
            }
            else
            {
                if (strcmp("int", text) == 0)
                {
                    result = 9019;
                }
            }
        }
        break;
        case 110 :
        if (strcmp("nullptr", text) == 0)
        {
            result = 9061;
        }
        break;
        case 108 :
        if (strcmp("long", text) == 0)
        {
            result = 9020;
        }
        break;
        case 114 :
        if (strcmp("register", text) == 0)
        {
            result = 9025;
        }
        else
        {
            if (strcmp("restrict", text) == 0)
            {
                result = 9026;
            }
            else
            {
                if (strcmp("return", text) == 0)
                {
                    result = 9027;
                }
            }
        }
        break;
        case 115 :
        if (strcmp("short", text) == 0)
        {
            result = 9028;
        }
        else
        {
            if (strcmp("signed", text) == 0)
            {
                result = 9029;
            }
            else
            {
                if (strcmp("sizeof", text) == 0)
                {
                    result = 9030;
                }
                else
                {
                    if (strcmp("static", text) == 0)
                    {
                        result = 9032;
                    }
                    else
                    {
                        if (strcmp("struct", text) == 0)
                        {
                            result = 9033;
                        }
                        else
                        {
                            if (strcmp("switch", text) == 0)
                            {
                                result = 9034;
                            }
                            else
                            {
                                if (strcmp("static_assert", text) == 0)
                                {
                                    result = 9055;
                                }
                                else
                                {
                                    if (strcmp("static_debug", text) == 0)
                                    {
                                        result = 9069;
                                    }
                                    else
                                    {
                                        if (strcmp("static_debug_ex", text) == 0)
                                        {
                                            result = 9070;
                                        }
                                        else
                                        {
                                            if (strcmp("static_state", text) == 0)
                                            {
                                                result = 9071;
                                            }
                                            else
                                            {
                                                if (strcmp("static_set", text) == 0)
                                                {
                                                    result = 9072;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
        case 116 :
        if (strcmp("typedef", text) == 0)
        {
            result = 9035;
        }
        else
        {
            if (strcmp("typeof", text) == 0)
            {
                result = 9058;
            }
            else
            {
                if (strcmp("typeof_unqual", text) == 0)
                {
                    result = 9062;
                }
                else
                {
                    if (strcmp("true", text) == 0)
                    {
                        result = 9059;
                    }
                    else
                    {
                        if (strcmp("thread_local", text) == 0)
                        {
                            result = 9057;
                        }
                        else
                        {
                            if (strcmp("try", text) == 0)
                            {
                                result = 9036;
                            }
                            else
                            {
                                if (strcmp("throw", text) == 0)
                                {
                                    result = 9037;
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
        case 117 :
        if (strcmp("union", text) == 0)
        {
            result = 9038;
        }
        else
        {
            if (strcmp("unsigned", text) == 0)
            {
                result = 9039;
            }
        }
        break;
        case 118 :
        if (strcmp("void", text) == 0)
        {
            result = 9040;
        }
        else
        {
            if (strcmp("volatile", text) == 0)
            {
                result = 9041;
            }
        }
        break;
        case 119 :
        if (strcmp("while", text) == 0)
        {
            result = 9042;
        }
        break;
        case 95 :
        if (strcmp("_Out", text) == 0)
        {
            result = 9065;
        }
        else
        {
            if (strcmp("_Owner", text) == 0)
            {
                result = 9064;
            }
            else
            {
                if (strcmp("_Obj_owner", text) == 0)
                {
                    result = 9066;
                }
                else
                {
                    if (strcmp("_Opt", text) == 0)
                    {
                        result = 9068;
                    }
                    else
                    {
                        if (strcmp("_View", text) == 0)
                        {
                            result = 9067;
                        }
                        else
                        {
                            if (strcmp("_Lengthof", text) == 0)
                            {
                                result = 9031;
                            }
                            else
                            {
                                if (strcmp("_is_lvalue", text) == 0)
                                {
                                    result = 9074;
                                }
                                else
                                {
                                    if (strcmp("_is_const", text) == 0)
                                    {
                                        result = 9075;
                                    }
                                    else
                                    {
                                        if (strcmp("_is_owner", text) == 0)
                                        {
                                            result = 9076;
                                        }
                                        else
                                        {
                                            if (strcmp("_is_pointer", text) == 0)
                                            {
                                                result = 9073;
                                            }
                                            else
                                            {
                                                if (strcmp("_is_array", text) == 0)
                                                {
                                                    result = 9077;
                                                }
                                                else
                                                {
                                                    if (strcmp("_is_function", text) == 0)
                                                    {
                                                        result = 9078;
                                                    }
                                                    else
                                                    {
                                                        if (strcmp("_is_arithmetic", text) == 0)
                                                        {
                                                            result = 9080;
                                                        }
                                                        else
                                                        {
                                                            if (strcmp("_is_floating_point", text) == 0)
                                                            {
                                                                result = 9081;
                                                            }
                                                            else
                                                            {
                                                                if (strcmp("_is_integral", text) == 0)
                                                                {
                                                                    result = 9082;
                                                                }
                                                                else
                                                                {
                                                                    if (strcmp("_is_scalar", text) == 0)
                                                                    {
                                                                        result = 9079;
                                                                    }
                                                                    else
                                                                    {
                                                                        if (strcmp("_Alignof", text) == 0)
                                                                        {
                                                                            result = 9044;
                                                                        }
                                                                        else
                                                                        {
                                                                            if (strcmp("_Alignas", text) == 0)
                                                                            {
                                                                                result = 9043;
                                                                            }
                                                                            else
                                                                            {
                                                                                if (strcmp("_Atomic", text) == 0)
                                                                                {
                                                                                    result = 9045;
                                                                                }
                                                                                else
                                                                                {
                                                                                    if (strcmp("_Bool", text) == 0)
                                                                                    {
                                                                                        result = 9047;
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        if (strcmp("_Complex", text) == 0)
                                                                                        {
                                                                                            result = 9048;
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            if (strcmp("_Decimal32", text) == 0)
                                                                                            {
                                                                                                result = 9050;
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                if (strcmp("_Decimal64", text) == 0)
                                                                                                {
                                                                                                    result = 9051;
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    if (strcmp("_Decimal128", text) == 0)
                                                                                                    {
                                                                                                        result = 9049;
                                                                                                    }
                                                                                                    else
                                                                                                    {
                                                                                                        if (strcmp("_Generic", text) == 0)
                                                                                                        {
                                                                                                            result = 9052;
                                                                                                        }
                                                                                                        else
                                                                                                        {
                                                                                                            if (strcmp("_Imaginary", text) == 0)
                                                                                                            {
                                                                                                                result = 9053;
                                                                                                            }
                                                                                                            else
                                                                                                            {
                                                                                                                if (strcmp("_Noreturn", text) == 0)
                                                                                                                {
                                                                                                                    result = 9054;
                                                                                                                }
                                                                                                                else
                                                                                                                {
                                                                                                                    if (strcmp("_Static_assert", text) == 0)
                                                                                                                    {
                                                                                                                        result = 9055;
                                                                                                                    }
                                                                                                                    else
                                                                                                                    {
                                                                                                                        if (strcmp("_Thread_local", text) == 0)
                                                                                                                        {
                                                                                                                            result = 9057;
                                                                                                                        }
                                                                                                                        else
                                                                                                                        {
                                                                                                                            if (strcmp("_BitInt", text) == 0)
                                                                                                                            {
                                                                                                                                result = 9063;
                                                                                                                            }
                                                                                                                            else
                                                                                                                            {
                                                                                                                                if (strcmp("__typeof__", text) == 0)
                                                                                                                                {
                                                                                                                                    result = 9058;
                                                                                                                                }
                                                                                                                                else
                                                                                                                                {
                                                                                                                                    if (strcmp("__int8", text) == 0)
                                                                                                                                    {
                                                                                                                                        result = 9021;
                                                                                                                                    }
                                                                                                                                    else
                                                                                                                                    {
                                                                                                                                        if (strcmp("__int16", text) == 0)
                                                                                                                                        {
                                                                                                                                            result = 9022;
                                                                                                                                        }
                                                                                                                                        else
                                                                                                                                        {
                                                                                                                                            if (strcmp("__int32", text) == 0)
                                                                                                                                            {
                                                                                                                                                result = 9023;
                                                                                                                                            }
                                                                                                                                            else
                                                                                                                                            {
                                                                                                                                                if (strcmp("__int64", text) == 0)
                                                                                                                                                {
                                                                                                                                                    result = 9024;
                                                                                                                                                }
                                                                                                                                                else
                                                                                                                                                {
                                                                                                                                                    if (strcmp("__forceinline", text) == 0)
                                                                                                                                                    {
                                                                                                                                                        result = 9018;
                                                                                                                                                    }
                                                                                                                                                    else
                                                                                                                                                    {
                                                                                                                                                        if (strcmp("__inline", text) == 0)
                                                                                                                                                        {
                                                                                                                                                            result = 9018;
                                                                                                                                                        }
                                                                                                                                                        else
                                                                                                                                                        {
                                                                                                                                                            if (strcmp("_asm", text) == 0 || strcmp("__asm", text) == 0)
                                                                                                                                                            {
                                                                                                                                                                result = 9046;
                                                                                                                                                            }
                                                                                                                                                            else
                                                                                                                                                            {
                                                                                                                                                                if (strcmp("__alignof", text) == 0)
                                                                                                                                                                {
                                                                                                                                                                    result = 9044;
                                                                                                                                                                }
                                                                                                                                                                else
                                                                                                                                                                {
                                                                                                                                                                    if (strcmp("__restrict", text) == 0)
                                                                                                                                                                    {
                                                                                                                                                                        result = 9026;
                                                                                                                                                                    }
                                                                                                                                                                }
                                                                                                                                                            }
                                                                                                                                                        }
                                                                                                                                                    }
                                                                                                                                                }
                                                                                                                                            }
                                                                                                                                        }
                                                                                                                                    }
                                                                                                                                }
                                                                                                                            }
                                                                                                                        }
                                                                                                                    }
                                                                                                                }
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
        default:
        break;
    }
    return result;
}

int  parse_number(char * lexeme, char suffix[4], char erromsg[100]);

static void token_promote(struct parser_ctx * ctx, struct token * token)
{
    if (token->type == 8997)
    {
        token->type = 8996;
    }
    if (token->type == 8996)
    {
        int   t = is_keyword(token->lexeme);
        if (t != 0)
        {
            token->type = t;
        }
    }
    else
    {
        if (token->type == 134)
        {
            char errormsg[100] = {0};
            char suffix[4] = {0};
            token->type = parse_number(token->lexeme, suffix, errormsg);
            if (token->type == 0)
            {
                compiler_diagnostic_message(1380, ctx, token, ((void *)0), errormsg);
            }
        }
    }
}

struct token *parser_look_ahead(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return ((void *)0);
    }
    struct token * p = ctx->current->next;
    while (p &&  !(p->flags & 1))
    {
        p = p->next;
    }
    if (p)
    {
        token_promote(ctx, p);
    }
    return p;
}

static struct token *pragma_match(struct token * p_current)
{
    struct token * p_token = p_current->next;
    while (p_token && p_token->type == 143)
    {
        p_token = p_token->next;
    }
    return p_token;
}

static void pragma_skip_blanks(struct parser_ctx * ctx)
{
    while (ctx->current && ctx->current->type == 143)
    {
        ctx->current = ctx->current->next;
    }
}

void unexpected_end_of_file(struct parser_ctx * ctx);
unsigned long long get_warning_bit_mask(char * wname);
int  get_warning(char * wname);
int get_diagnostic_type(struct diagnostic * d, int   w);

static void parse_pragma(struct parser_ctx * ctx, struct token * token)
{
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 128)
        {
            ctx->current = ctx->current->next;
            pragma_skip_blanks(ctx);
            if (ctx->current && (strcmp(ctx->current->lexeme, "CAKE") == 0 || strcmp(ctx->current->lexeme, "cake") == 0))
            {
                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);
            }
            if (ctx->current && strcmp(ctx->current->lexeme, "nullchecks") == 0)
            {
                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);
                unsigned char   onoff = 0;
                if (ctx->current && strcmp(ctx->current->lexeme, "ON") == 0)
                {
                    onoff = 1;
                }
                else
                {
                    if (ctx->current && strcmp(ctx->current->lexeme, "OFF") == 0)
                    {
                        onoff = 0;
                    }
                    else
                    {
                        compiler_diagnostic_message(1250, ctx, ctx->current, ((void *)0), "nullchecks pragma needs to use ON OFF");
                    }
                }
                ctx->options.null_checks_enabled = onoff;
            }
            if (ctx->current && strcmp(ctx->current->lexeme, "diagnostic") == 0)
            {
                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);
                if (ctx->current && strcmp(ctx->current->lexeme, "push") == 0)
                {
                    if (ctx->options.diagnostic_stack.top_index < sizeof (ctx->options.diagnostic_stack) / sizeof (ctx->options.diagnostic_stack.stack[0]))
                    {
                        ctx->options.diagnostic_stack.top_index++;
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index - 1];
                    }
                    ctx->current = ctx->current->next;
                    pragma_skip_blanks(ctx);
                }
                else
                {
                    if (ctx->current && strcmp(ctx->current->lexeme, "pop") == 0)
                    {
                        if (ctx->options.diagnostic_stack.top_index > 0)
                        {
                            ctx->options.diagnostic_stack.top_index--;
                        }
                        ctx->current = ctx->current->next;
                        pragma_skip_blanks(ctx);
                    }
                    else
                    {
                        if (ctx->current && (strcmp(ctx->current->lexeme, "error") == 0 || strcmp(ctx->current->lexeme, "warning") == 0 || strcmp(ctx->current->lexeme, "note") == 0 || strcmp(ctx->current->lexeme, "ignored") == 0))
                        {
                            unsigned char   is_error = strcmp(ctx->current->lexeme, "error") == 0;
                            unsigned char   is_warning = strcmp(ctx->current->lexeme, "warning") == 0;
                            unsigned char   is_note = strcmp(ctx->current->lexeme, "note") == 0;
                            ctx->current = ctx->current->next;
                            pragma_skip_blanks(ctx);
                            if (ctx->current && ctx->current->type == 130)
                            {
                                unsigned long long w = get_warning_bit_mask(ctx->current->lexeme + 1);
                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors =  ~w;
                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes =  ~w;
                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings =  ~w;
                                if (is_error)
                                {
                                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors = w;
                                }
                                else
                                {
                                    if (is_warning)
                                    {
                                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings = w;
                                    }
                                    else
                                    {
                                        if (is_note)
                                        {
                                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes = w;
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (ctx->current && (strcmp(ctx->current->lexeme, "check") == 0))
                            {
                                ctx->current = ctx->current->next;
                                pragma_skip_blanks(ctx);
                                if (ctx->current && ctx->current->type == 130)
                                {
                                    int   id = get_warning(ctx->current->lexeme + 1 + 2);
                                    unsigned char   found = 0;
                                    {
                                        int i = 0;
                                        for (; i < (int)(sizeof (ctx->p_report->last_diagnostics_ids) / sizeof (ctx->p_report->last_diagnostics_ids[0])); i++)
                                        {
                                            if (ctx->p_report->last_diagnostics_ids[i] == 0)
                                            {
                                                break;
                                            }
                                            if (ctx->p_report->last_diagnostics_ids[i] == id)
                                            {
                                                found = 1;
                                                int t = get_diagnostic_type(&ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index], id);
                                                if (t == 3)
                                                {
                                                    ctx->p_report->error_count--;
                                                }
                                                else
                                                {
                                                    if (t == 2)
                                                    {
                                                        ctx->p_report->warnings_count--;
                                                    }
                                                    else
                                                    {
                                                        if (t == 1)
                                                        {
                                                            ctx->p_report->info_count--;
                                                        }
                                                    }
                                                }
                                                break;
                                            }
                                        }
                                    }
                                    if ( !found)
                                    {
                                        compiler_diagnostic_message(650, ctx, ctx->current, ((void *)0), "pragma check failed");
                                    }
                                }
                            }
                            else
                            {
                                compiler_diagnostic_message(650, ctx, ctx->current, ((void *)0), "unknown pragma");
                            }
                        }
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
}

static void parser_skip_blanks(struct parser_ctx * ctx)
{
    while (ctx->current &&  !(ctx->current->flags & 1))
    {
        if (ctx->current->type == 128)
        {
            parse_pragma(ctx, ctx->current);
        }
        if (ctx->current)
        {
            ctx->current = ctx->current->next;
        }
    }
    if (ctx->current)
    {
        token_promote(ctx, ctx->current);
    }
}

void parser_match(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return;
    }
    ctx->previous = ctx->current;
    ctx->current = ctx->current->next;
    parser_skip_blanks(ctx);
}

void unexpected_end_of_file(struct parser_ctx * ctx)
{
    compiler_diagnostic_message(970, ctx, ctx->input_list.tail, ((void *)0), "unexpected end of file");
}

char *get_token_name(int   tk);

int parser_match_tk(struct parser_ctx * ctx, int   type)
{
    int error = 0;
    if (ctx->current != ((void *)0))
    {
        if (ctx->current->type != type)
        {
            compiler_diagnostic_message(970, ctx, ctx->current, ((void *)0), "expected %s", get_token_name(type));
            error = 1;
        }
        ctx->previous = ctx->current;
        ctx->current = ctx->current->next;
        parser_skip_blanks(ctx);
    }
    else
    {
        compiler_diagnostic_message(970, ctx, ctx->input_list.tail, ((void *)0), "unexpected end of file after");
        error = 1;
    }
    return error;
}

void print_type_qualifier_flags(struct osstream * ss, unsigned char  * first, int   e_type_qualifier_flags);
int ss_fprintf(struct osstream * stream, char * fmt, ...);
void print_item(struct osstream * ss, unsigned char  * first, char * item);
unsigned char  print_type_specifier_flags(struct osstream * ss, unsigned char  * first, int   e_type_specifier_flags);

void print_declaration_specifiers(struct osstream * ss, struct declaration_specifiers * p_declaration_specifiers)
{
    unsigned char   first = 1;
    print_type_qualifier_flags(ss, &first, p_declaration_specifiers->type_qualifier_flags);
    if (p_declaration_specifiers->enum_specifier)
    {
        if (p_declaration_specifiers->enum_specifier->tag_token)
        {
            ss_fprintf(ss, "enum %s", p_declaration_specifiers->enum_specifier->tag_token->lexeme);
        }
        else
        {
            ;
        }
    }
    else
    {
        if (p_declaration_specifiers->struct_or_union_specifier)
        {
            ss_fprintf(ss, "struct %s", p_declaration_specifiers->struct_or_union_specifier->tag_name);
        }
        else
        {
            if (p_declaration_specifiers->typedef_declarator)
            {
                if (p_declaration_specifiers->typedef_declarator->name_opt)
                {
                    print_item(ss, &first, p_declaration_specifiers->typedef_declarator->name_opt->lexeme);
                }
            }
            else
            {
                print_type_specifier_flags(ss, &first, p_declaration_specifiers->type_specifier_flags);
            }
        }
    }
}

unsigned char  type_specifier_is_integer(int   flags)
{
    if ((flags & 2) || (flags & 4) || (flags & 8) || (flags & 16) || (flags & 8) || (flags & 262144) || (flags & 524288) || (flags & 1048576) || (flags & 2097152) || (flags & 4194304))
    {
        return 1;
    }
    return 0;
}

int final_specifier(struct parser_ctx * ctx, int  * flags)
{
    if ((( *flags) & 256) || (( *flags) & 128))
    {
        if ( !type_specifier_is_integer( *flags))
        {
            ( *flags) = 8;
        }
    }
    return 0;
}

int add_specifier(struct parser_ctx * ctx, int  * flags, int   new_flag)
{
    if (new_flag & 16)
    {
        if (( *flags) & 4194304)
        {
            compiler_diagnostic_message(980, ctx, ctx->current, ((void *)0), "cannot combine with previous 'long long' declaration specifier");
            return 1;
        }
        else
        {
            if (( *flags) & 16)
            {
                ( *flags) = ( *flags) &  ~16;
                ( *flags) = 4194304;
            }
            else
            {
                ( *flags) = ( *flags) &  ~8;
                ( *flags) = 16;
            }
        }
    }
    else
    {
        ( *flags) = new_flag;
    }
    switch ((unsigned int) *flags)
    {
        case 1 :
        case 2 :
        case 130 :
        case 258 :
        case 4 :
        case 132 :
        case 12 :
        case 140 :
        case 260 :
        case 268 :
        case 8 :
        case 128 :
        case 136 :
        case 256 :
        case 264 :
        case 16 :
        case 144 :
        case 24 :
        case 152 :
        case 272 :
        case 280 :
        case 4194304 :
        case 4194432 :
        case 4194312 :
        case 4194440 :
        case 4194560 :
        case 4194568 :
        case 32 :
        case 64 :
        case 80 :
        case 2048 :
        case 4096 :
        case 8192 :
        case 512 :
        case 1056 :
        case 1088 :
        case 1104 :
        case 16384 :
        case 32768 :
        case 65536 :
        case 8388608 :
        case 131072 :
        case 262144 :
        case 262400 :
        case 524288 :
        case 524544 :
        case 524416 :
        case 1048576 :
        case 1048832 :
        case 1048704 :
        case 2097152 :
        case 2097408 :
        case 2097280 :
        break;
        default:
        compiler_diagnostic_message(1300, ctx, ctx->current, ((void *)0), "incompatible specifiers");
        return 1;
    }
    return 0;
}

void attribute_specifier_sequence_delete(struct attribute_specifier_sequence * p);
void declaration_specifier_delete(struct declaration_specifier * p);

void declaration_specifiers_delete(struct declaration_specifiers * p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        struct declaration_specifier * item = p->head;
        while (item)
        {
            struct declaration_specifier * next = item->next;
            item->next = ((void *)0);
            declaration_specifier_delete(item);
            item = next;
        }
        free(p);
    }
}

void declaration_specifiers_add(struct declaration_specifiers * list, struct declaration_specifier * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void *calloc(int nmemb, unsigned int size);
struct declaration_specifier *declaration_specifier(struct parser_ctx * ctx);
struct attribute_specifier_sequence *attribute_specifier_sequence_opt(struct parser_ctx * ctx);

struct declaration_specifiers *declaration_specifiers(struct parser_ctx * ctx, int   default_storage_flag)
{
    if (ctx->current == ((void *)0))
    {
        return ((void *)0);
    }
    struct declaration_specifiers * p_declaration_specifiers = calloc(1, sizeof (struct declaration_specifiers));
    if (1)
    {
        if (p_declaration_specifiers == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_declaration_specifiers->first_token = ctx->current;
        while (first_of_declaration_specifier(ctx))
        {
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->flags & 16)
            {
                if (p_declaration_specifiers->type_specifier_flags != 0)
                {
                    break;
                }
            }
            struct declaration_specifier * p_declaration_specifier = declaration_specifier(ctx);
            if (p_declaration_specifier == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (p_declaration_specifier->type_specifier_qualifier)
            {
                if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                {
                    if (add_specifier(ctx, &p_declaration_specifiers->type_specifier_flags, p_declaration_specifier->type_specifier_qualifier->type_specifier->flags) != 0)
                    {
                    }
                    if (p_declaration_specifier->type_specifier_qualifier->type_specifier->struct_or_union_specifier)
                    {
                        p_declaration_specifiers->struct_or_union_specifier = p_declaration_specifier->type_specifier_qualifier->type_specifier->struct_or_union_specifier;
                    }
                    else
                    {
                        if (p_declaration_specifier->type_specifier_qualifier->type_specifier->enum_specifier)
                        {
                            p_declaration_specifiers->enum_specifier = p_declaration_specifier->type_specifier_qualifier->type_specifier->enum_specifier;
                        }
                        else
                        {
                            if (p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier)
                            {
                                p_declaration_specifiers->typeof_specifier = p_declaration_specifier->type_specifier_qualifier->type_specifier->typeof_specifier;
                            }
                            else
                            {
                                if (p_declaration_specifier->type_specifier_qualifier->type_specifier->token->type == 8996)
                                {
                                    p_declaration_specifiers->typedef_declarator = find_declarator(ctx, p_declaration_specifier->type_specifier_qualifier->type_specifier->token->lexeme, ((void *)0));
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                    {
                        p_declaration_specifiers->type_qualifier_flags = p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;
                    }
                }
            }
            else
            {
                if (p_declaration_specifier->storage_class_specifier)
                {
                    p_declaration_specifiers->storage_class_specifier_flags = p_declaration_specifier->storage_class_specifier->flags;
                }
                else
                {
                    if (p_declaration_specifier->function_specifier)
                    {
                        p_declaration_specifiers->function_specifier_flags = p_declaration_specifier->function_specifier->flags;
                    }
                }
            }
            declaration_specifiers_add(p_declaration_specifiers, p_declaration_specifier);
            ;
            p_declaration_specifiers->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type == 8996 && p_declaration_specifiers->type_specifier_flags != 0)
            {
                break;
            }
        }
        struct token * prev = previous_parser_token(ctx->current);
        if (prev == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_declaration_specifiers->last_token = prev;
        final_specifier(ctx, &p_declaration_specifiers->type_specifier_flags);
        p_declaration_specifiers->storage_class_specifier_flags = default_storage_flag;
        if (p_declaration_specifiers->storage_class_specifier_flags & 4)
        {
            p_declaration_specifiers->storage_class_specifier_flags =  ~4096;
        }
    }
    else _catch_label_1:
    {
        declaration_specifiers_delete(p_declaration_specifiers);
        p_declaration_specifiers = ((void *)0);
    }
    return p_declaration_specifiers;
}

struct static_assert_declaration *static_assert_declaration(struct parser_ctx * ctx);
struct pragma_declaration *pragma_declaration(struct parser_ctx * ctx);
struct init_declarator_list init_declarator_list(struct parser_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers);
void declaration_delete(struct declaration * p);

struct declaration *declaration_core(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt, unsigned char   can_be_function_definition, unsigned char  * is_function_definition, int   default_storage_class_specifier_flags, unsigned char   without_semicolon)
{
    struct declaration * p_declaration = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_declaration = calloc(1, sizeof (struct declaration));
        if (p_declaration == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_declaration->p_attribute_specifier_sequence_opt = p_attribute_specifier_sequence_opt;
        p_attribute_specifier_sequence_opt = ((void *)0);
        p_declaration->first_token = ctx->current;
        if (ctx->current->type == 59)
        {
            parser_match(ctx);
            return p_declaration;
        }
        if (first_of_static_assert_declaration(ctx))
        {
            p_declaration->static_assert_declaration = static_assert_declaration(ctx);
        }
        else
        {
            if (first_of_pragma_declaration(ctx))
            {
                p_declaration->pragma_declaration = pragma_declaration(ctx);
            }
            else
            {
                if (first_of_declaration_specifier(ctx))
                {
                    p_declaration->declaration_specifiers = declaration_specifiers(ctx, default_storage_class_specifier_flags);
                    if (p_declaration->declaration_specifiers == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    if (p_declaration->p_attribute_specifier_sequence_opt)
                    {
                        p_declaration->declaration_specifiers->attributes_flags = p_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
                    }
                    if (ctx->current == ((void *)0))
                    {
                        unexpected_end_of_file(ctx);
                        goto _catch_label_1;
                    }
                    if (ctx->current->type != 59)
                    {
                        p_declaration->init_declarator_list = init_declarator_list(ctx, p_declaration->declaration_specifiers);
                    }
                    if (ctx->current == ((void *)0))
                    {
                        unexpected_end_of_file(ctx);
                        goto _catch_label_1;
                    }
                    p_declaration->last_token = ctx->current;
                    if (ctx->current->type == 123)
                    {
                        if (can_be_function_definition)
                        {
                             *is_function_definition = 1;
                        }
                    }
                    else
                    {
                        if ( !without_semicolon && parser_match_tk(ctx, 59) != 0)
                        {
                            goto _catch_label_1;
                        }
                    }
                }
                else
                {
                    if (ctx->current->type == 8996)
                    {
                        compiler_diagnostic_message(860, ctx, ctx->current, ((void *)0), "invalid type '%s'", ctx->current->lexeme);
                    }
                    else
                    {
                        compiler_diagnostic_message(990, ctx, ctx->current, ((void *)0), "expected declaration not '%s'", ctx->current->lexeme);
                    }
                    parser_match(ctx);
                }
            }
        }
    }
    else _catch_label_1:
    {
        declaration_delete(p_declaration);
        p_declaration = ((void *)0);
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
    return p_declaration;
}

struct compound_statement *function_body(struct parser_ctx * ctx);
void defer_start_visit_declaration(struct defer_visit_ctx * ctx, struct declaration * p_declaration);
void defer_visit_ctx_destroy(struct defer_visit_ctx * p);
void flow_start_visit_declaration(struct flow_visit_ctx * ctx, struct declaration * p_declaration);
void flow_visit_ctx_destroy(struct flow_visit_ctx * p);
unsigned char  type_is_maybe_unused(struct type * p_type);

struct declaration *function_definition_or_declaration(struct parser_ctx * ctx)
{
    struct declaration * p_declaration = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        unsigned char   is_function_definition = 0;
        p_declaration = declaration_core(ctx, p_attribute_specifier_sequence_opt, 1, &is_function_definition, 0, 0);
        if (p_declaration == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (is_function_definition)
        {
            if (p_declaration->init_declarator_list.head == ((void *)0) || p_declaration->init_declarator_list.head->p_declarator->direct_declarator == ((void *)0) || p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator == ((void *)0))
            {
                goto _catch_label_1;
            }
            ctx->p_current_function_opt = p_declaration;
            ;
            struct declarator * inner = p_declaration->init_declarator_list.head->p_declarator;
            for (; ; )
            {
                if (inner->direct_declarator && inner->direct_declarator->function_declarator && inner->direct_declarator->function_declarator->direct_declarator && inner->direct_declarator->function_declarator->direct_declarator->declarator)
                {
                    inner = inner->direct_declarator->function_declarator->direct_declarator->declarator;
                }
                else
                {
                    break;
                }
            }
            struct scope * parameters_scope = &inner->direct_declarator->function_declarator->parameters_scope;
            scope_list_push(&ctx->scopes, parameters_scope);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            check_func_open_brace_style(ctx, ctx->current);
            struct diagnostic  before_function_diagnostics = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];
            struct compound_statement * p_function_body = function_body(ctx);
            if (p_function_body == ((void *)0))
            {
                goto _catch_label_1;
            }
            ;
            p_declaration->function_body = p_function_body;
            p_declaration->init_declarator_list.head->p_declarator->function_body = p_declaration->function_body;
            {
                struct defer_visit_ctx  ctx2 = {0};
                ctx2.ctx = ctx;
                defer_start_visit_declaration(&ctx2, p_declaration);
                defer_visit_ctx_destroy(&ctx2);
            }
            if (ctx->options.flow_analysis)
            {
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = before_function_diagnostics;
                struct flow_visit_ctx  ctx2 = {0};
                ctx2.ctx = ctx;
                flow_start_visit_declaration(&ctx2, p_declaration);
                flow_visit_ctx_destroy(&ctx2);
            }
            struct parameter_declaration * parameter = ((void *)0);
            if (p_declaration->init_declarator_list.head && p_declaration->init_declarator_list.head->p_declarator->direct_declarator && p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator && p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt && p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
            {
                parameter = p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
            }
            while (parameter)
            {
                if ( !type_is_maybe_unused(&parameter->declarator->type) && parameter->declarator && parameter->declarator->num_uses == 0)
                {
                    if (parameter->declarator->name_opt && parameter->declarator->name_opt->level == 0)
                    {
                        compiler_diagnostic_message(5, ctx, parameter->declarator->name_opt, ((void *)0), "'%s': unreferenced formal parameter", parameter->declarator->name_opt->lexeme);
                    }
                }
                parameter = parameter->next;
            }
            scope_list_pop(&ctx->scopes);
            ctx->p_current_function_opt = ((void *)0);
        }
        else
        {
            if (ctx->options.flow_analysis)
            {
                struct flow_visit_ctx  ctx2 = {0};
                ctx2.ctx = ctx;
                flow_start_visit_declaration(&ctx2, p_declaration);
                flow_visit_ctx_destroy(&ctx2);
            }
        }
    }
    else _catch_label_1:
    {
        declaration_delete(p_declaration);
        p_declaration = ((void *)0);
    }
    return p_declaration;
}

void simple_declaration_delete(struct simple_declaration * p);

struct simple_declaration *simple_declaration(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt, unsigned char   ignore_semicolon)
{
    if (ctx->current == ((void *)0))
    {
        unexpected_end_of_file(ctx);
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
        return ((void *)0);
    }
    int   storage_specifier_flags = 4096;
    struct simple_declaration * p_simple_declaration = calloc(1, sizeof (struct simple_declaration));
    if (1)
    {
        if (p_simple_declaration == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_simple_declaration->first_token = ctx->current;
        p_simple_declaration->p_attribute_specifier_sequence_opt = p_attribute_specifier_sequence_opt;
        p_attribute_specifier_sequence_opt = ((void *)0);
        struct declaration_specifiers * ptemp = declaration_specifiers(ctx, storage_specifier_flags);
        if (ptemp == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_simple_declaration->p_declaration_specifiers = ptemp;
        if (p_simple_declaration->p_attribute_specifier_sequence_opt)
        {
            p_simple_declaration->p_declaration_specifiers->attributes_flags = p_simple_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
        }
        p_simple_declaration->init_declarator_list = init_declarator_list(ctx, p_simple_declaration->p_declaration_specifiers);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        struct token * prev = previous_parser_token(ctx->current);
        if (prev == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_simple_declaration->last_token = prev;
        if ( !ignore_semicolon && parser_match_tk(ctx, 59) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        simple_declaration_delete(p_simple_declaration);
        p_simple_declaration = ((void *)0);
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
    return p_simple_declaration;
}

struct declaration *declaration(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt, int   storage_specifier_flags)
{
    unsigned char   is_function_definition = 0;
    return declaration_core(ctx, p_attribute_specifier_sequence_opt, 0, &is_function_definition, storage_specifier_flags, 0);
}

void type_specifier_qualifier_delete(struct type_specifier_qualifier * p);

void declaration_specifier_delete(struct declaration_specifier * p)
{
    if (p)
    {
        free(p->function_specifier);
        type_specifier_qualifier_delete(p->type_specifier_qualifier);
        free(p->storage_class_specifier);
        ;
        free(p);
    }
}

struct storage_class_specifier *storage_class_specifier(struct parser_ctx * ctx);
struct type_specifier_qualifier *type_specifier_qualifier(struct parser_ctx * ctx);
struct function_specifier *function_specifier(struct parser_ctx * ctx);

struct declaration_specifier *declaration_specifier(struct parser_ctx * ctx)
{
    struct declaration_specifier * p_declaration_specifier = ((void *)0);
    if (1)
    {
        p_declaration_specifier = calloc(1, sizeof  *p_declaration_specifier);
        if (p_declaration_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (first_of_storage_class_specifier(ctx))
        {
            p_declaration_specifier->storage_class_specifier = storage_class_specifier(ctx);
            if (p_declaration_specifier->storage_class_specifier == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        else
        {
            if (first_of_type_specifier_qualifier(ctx))
            {
                p_declaration_specifier->type_specifier_qualifier = type_specifier_qualifier(ctx);
                if (p_declaration_specifier->type_specifier_qualifier == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
            else
            {
                if (first_of_function_specifier(ctx))
                {
                    p_declaration_specifier->function_specifier = function_specifier(ctx);
                    if (p_declaration_specifier->function_specifier == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                }
                else
                {
                    compiler_diagnostic_message(650, ctx, ctx->current, ((void *)0), "unexpected");
                }
            }
        }
    }
    else _catch_label_1:
    {
        declaration_specifier_delete(p_declaration_specifier);
        p_declaration_specifier = ((void *)0);
    }
    return p_declaration_specifier;
}

struct init_declarator *init_declarator_add_ref(struct init_declarator * p)
{
    p->has_shared_ownership = 1;
    return (struct init_declarator *)p;
}

void init_declarator_sink(struct init_declarator * p);
void initializer_delete(struct initializer * p);
void declarator_delete(struct declarator * p);

void init_declarator_delete(struct init_declarator * p)
{
    if (p)
    {
        if (p->has_shared_ownership)
        {
            p->has_shared_ownership = 0;
            init_declarator_sink(p);
            return;
        }
        initializer_delete(p->initializer);
        declarator_delete(p->p_declarator);
        ;
        free(p);
    }
}

struct declarator *declarator(struct parser_ctx * ctx, struct specifier_qualifier_list * specifier_qualifier_list, struct declaration_specifiers * declaration_specifiers, unsigned char   abstract_acceptable, struct token ** pptokenname);
struct type make_type_using_declarator(struct parser_ctx * ctx, struct declarator * pdeclarator);
void naming_convention_global_var(struct parser_ctx * ctx, struct token * token, struct type * type, int   storage);
unsigned char  type_is_same(struct type * a, struct type * b, unsigned char   compare_qualifiers);
int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item);
void hash_item_set_destroy(struct hash_item_set * p);
struct initializer *initializer(struct parser_ctx * ctx);
int make_object(struct type * p_type, struct object * obj);
unsigned char  type_is_const(struct type * p_type);
int initializer_init_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * p_current_object, struct initializer * braced_initializer, unsigned char   is_constant);
unsigned char  type_is_array(struct type * p_type);
struct type type_dup(struct type * p_type);
struct type type_lvalue_conversion(struct type * p_type, unsigned char   nullchecks_enabled);
void type_swap(struct type * a, struct type * b);
void type_destroy(struct type * p_type);
void type_remove_names(struct type * p_type);
char *strdup(char * src);
void type_set_qualifiers_using_declarator(struct type * p_type, struct declarator * pdeclarator);
void type_visit_to_mark_anonymous(struct type * p_type);
void check_assigment(struct parser_ctx * ctx, struct type * left_type, struct expression * right, int   assigment_type);
unsigned char  type_is_pointer(struct type * p_type);
unsigned char  type_is_function(struct type * p_type);
unsigned char  type_is_vla(struct type * p_type);
unsigned int type_get_sizeof(struct type * p_type);

struct init_declarator *init_declarator(struct parser_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers)
{
    struct init_declarator * p_init_declarator = ((void *)0);
    if (1)
    {
        p_init_declarator = calloc(1, sizeof (struct init_declarator));
        if (p_init_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct token * tkname = ((void *)0);
        {
            struct declarator * p_temp_declarator = declarator(ctx, ((void *)0), p_declaration_specifiers, 0, &tkname);
            if (p_temp_declarator == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_init_declarator->p_declarator = p_temp_declarator;
        }
        if (tkname == ((void *)0))
        {
            compiler_diagnostic_message(650, ctx, ctx->current, ((void *)0), "init declarator must have a name");
            goto _catch_label_1;
        }
        p_init_declarator->p_declarator->declaration_specifiers = p_declaration_specifiers;
        p_init_declarator->p_declarator->name_opt = tkname;
        if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 16)
        {
        }
        else
        {
            ;
            p_init_declarator->p_declarator->type = make_type_using_declarator(ctx, p_init_declarator->p_declarator);
        }
        ;
        ;
        if (ctx->scopes.tail->scope_level == 0)
        {
            naming_convention_global_var(ctx, tkname, &p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags);
        }
        char * name = p_init_declarator->p_declarator->name_opt->lexeme;
        struct scope * out_scope = ((void *)0);
        struct declarator * previous = find_declarator(ctx, name, &out_scope);
        if (previous)
        {
            ;
            ;
            if (out_scope->scope_level == ctx->scopes.tail->scope_level)
            {
                if (out_scope->scope_level == 0)
                {
                    if ( !type_is_same(&previous->type, &p_init_declarator->p_declarator->type, 1))
                    {
                    }
                }
                else
                {
                    compiler_diagnostic_message(1020, ctx, ctx->current, ((void *)0), "redeclaration");
                    compiler_diagnostic_message(63, ctx, previous->name_opt, ((void *)0), "previous declaration");
                }
            }
            else
            {
                struct hash_item_set  item = {0};
                item.p_init_declarator = init_declarator_add_ref(p_init_declarator);
                hashmap_set(&ctx->scopes.tail->variables, name, &item);
                hash_item_set_destroy(&item);
                if (out_scope->scope_level != 0)
                {
                    if (compiler_diagnostic_message(6, ctx, p_init_declarator->p_declarator->first_token_opt, ((void *)0), "declaration of '%s' hides previous declaration", name))
                    {
                        compiler_diagnostic_message(63, ctx, previous->first_token_opt, ((void *)0), "previous declaration is here");
                    }
                }
            }
        }
        else
        {
            struct hash_item_set  item = {0};
            item.p_init_declarator = init_declarator_add_ref(p_init_declarator);
            hashmap_set(&ctx->scopes.tail->variables, name, &item);
            hash_item_set_destroy(&item);
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 61)
        {
            parser_match(ctx);
            ;
            p_init_declarator->initializer = initializer(ctx);
            if (p_init_declarator->initializer == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (p_init_declarator->initializer->braced_initializer)
            {
                if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 16)
                {
                    compiler_diagnostic_message(1290, ctx, p_init_declarator->p_declarator->first_token_opt, ((void *)0), "'auto' requires a plain identifier");
                    goto _catch_label_1;
                }
                int er = make_object(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object);
                if (er != 0)
                {
                    compiler_diagnostic_message(740, ctx, p_init_declarator->p_declarator->first_token_opt, ((void *)0), "incomplete struct/union type");
                    goto _catch_label_1;
                }
                unsigned char   is_constant = type_is_const(&p_init_declarator->p_declarator->type) || p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 64;
                initializer_init_new(ctx, &p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, p_init_declarator->initializer, is_constant);
            }
            else
            {
                if (p_init_declarator->initializer->assignment_expression)
                {
                    if (type_is_array(&p_init_declarator->p_declarator->type))
                    {
                        int array_size_elements = p_init_declarator->p_declarator->type.num_of_elements;
                        if (array_size_elements == 0)
                        {
                            p_init_declarator->p_declarator->type.num_of_elements = p_init_declarator->initializer->assignment_expression->type.num_of_elements;
                        }
                        else
                        {
                            if (p_init_declarator->initializer->assignment_expression->type.num_of_elements > array_size_elements)
                            {
                                if (p_init_declarator->p_declarator->first_token_opt)
                                {
                                    compiler_diagnostic_message(51, ctx, p_init_declarator->p_declarator->first_token_opt, ((void *)0), "initializer for array is too long");
                                }
                            }
                        }
                    }
                    ;
                    if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 16)
                    {
                        if (p_init_declarator->p_declarator->direct_declarator && (p_init_declarator->p_declarator->direct_declarator->array_declarator != ((void *)0) || p_init_declarator->p_declarator->direct_declarator->function_declarator != ((void *)0)))
                        {
                            compiler_diagnostic_message(1290, ctx, p_init_declarator->p_declarator->first_token_opt, ((void *)0), "'auto' requires a plain identifier");
                            goto _catch_label_1;
                        }
                        if (p_init_declarator->p_declarator->pointer != ((void *)0))
                        {
                            compiler_diagnostic_message(1290, ctx, p_init_declarator->p_declarator->first_token_opt, ((void *)0), "'auto' requires a plain identifier");
                            goto _catch_label_1;
                        }
                        struct type  t = {0};
                        if (p_init_declarator->initializer->assignment_expression->expression_type == 33)
                        {
                            t = type_dup(&p_init_declarator->initializer->assignment_expression->type);
                        }
                        else
                        {
                            struct type  t2 = type_lvalue_conversion(&p_init_declarator->initializer->assignment_expression->type, ctx->options.null_checks_enabled);
                            type_swap(&t2, &t);
                            type_destroy(&t2);
                        }
                        type_remove_names(&t);
                        ;
                        t.name_opt = strdup(p_init_declarator->p_declarator->name_opt->lexeme);
                        type_set_qualifiers_using_declarator(&t, p_init_declarator->p_declarator);
                        type_visit_to_mark_anonymous(&t);
                        type_swap(&p_init_declarator->p_declarator->type, &t);
                        type_destroy(&t);
                    }
                    check_assigment(ctx, &p_init_declarator->p_declarator->type, p_init_declarator->initializer->assignment_expression, 2);
                    int er = make_object(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object);
                    if (er != 0)
                    {
                        goto _catch_label_1;
                    }
                    unsigned char   is_constant = type_is_const(&p_init_declarator->p_declarator->type) || p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 64;
                    initializer_init_new(ctx, &p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, p_init_declarator->initializer, is_constant);
                }
            }
        }
        else
        {
            if (type_is_const(&p_init_declarator->p_declarator->type))
            {
                if (p_declaration_specifiers->storage_class_specifier_flags & 1)
                {
                }
                else
                {
                    compiler_diagnostic_message(47, ctx, p_init_declarator->p_declarator->first_token_opt, ((void *)0), "const object should be initialized");
                }
            }
        }
        if (p_init_declarator->p_declarator)
        {
            if (type_is_array(&p_init_declarator->p_declarator->type))
            {
                if (p_init_declarator->p_declarator->type.type_qualifier_flags != 0 || p_init_declarator->p_declarator->type.static_array)
                {
                    if (p_init_declarator->p_declarator->first_token_opt)
                    {
                        compiler_diagnostic_message(1000, ctx, p_init_declarator->p_declarator->first_token_opt, ((void *)0), "static or type qualifiers are not allowed in non-parameter array declarator");
                    }
                    else
                    {
                        if (p_init_declarator->initializer)
                        {
                            compiler_diagnostic_message(1000, ctx, p_init_declarator->initializer->first_token, ((void *)0), "static or type qualifiers are not allowed in non-parameter array declarator");
                        }
                    }
                }
            }
            if ( !type_is_pointer(&p_init_declarator->p_declarator->type) && p_init_declarator->p_declarator->type.type_qualifier_flags & 32)
            {
                if (p_init_declarator->p_declarator->first_token_opt)
                {
                    compiler_diagnostic_message(1010, ctx, p_init_declarator->p_declarator->first_token_opt, ((void *)0), "_Obj_owner qualifier can only be used with pointers");
                }
                else
                {
                    if (p_init_declarator->initializer)
                    {
                        compiler_diagnostic_message(1010, ctx, p_init_declarator->initializer->first_token, ((void *)0), "_Obj_owner qualifier can only be used with pointers");
                    }
                }
            }
        }
        if ( !(p_init_declarator->p_declarator->type.storage_class_specifier_flags & 1) &&  !type_is_function(&p_init_declarator->p_declarator->type))
        {
            if (type_is_vla(&p_init_declarator->p_declarator->type))
            {
            }
            else
            {
                int sz = type_get_sizeof(&p_init_declarator->p_declarator->type);
                if (sz ==  -3)
                {
                }
                else
                {
                    if (sz < 0)
                    {
                        if (p_init_declarator->p_declarator->type.storage_class_specifier_flags & 2)
                        {
                        }
                        else
                        {
                            compiler_diagnostic_message(1270, ctx, p_init_declarator->p_declarator->name_opt, ((void *)0), "storage size of '%s' isn't known", p_init_declarator->p_declarator->name_opt->lexeme);
                        }
                    }
                    else
                    {
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
        init_declarator_delete(p_init_declarator);
        p_init_declarator = ((void *)0);
    }
    return p_init_declarator;
}

void init_declarator_list_add(struct init_declarator_list * list, struct init_declarator * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void init_declarator_list_destroy(struct init_declarator_list * p)
{
    struct init_declarator * item = p->head;
    while (item)
    {
        struct init_declarator * next = item->next;
        item->next = ((void *)0);
        init_declarator_delete(item);
        item = next;
    }
}

struct init_declarator_list init_declarator_list(struct parser_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers)
{
    struct init_declarator_list  init_declarator_list = {0};
    struct init_declarator * p_init_declarator = ((void *)0);
    if (1)
    {
        p_init_declarator = init_declarator(ctx, p_declaration_specifiers);
        if (p_init_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        init_declarator_list_add(&init_declarator_list, p_init_declarator);
        p_init_declarator = ((void *)0);
        while (ctx->current != ((void *)0) && ctx->current->type == 44)
        {
            parser_match(ctx);
            p_init_declarator = init_declarator(ctx, p_declaration_specifiers);
            if (p_init_declarator == ((void *)0))
            {
                goto _catch_label_1;
            }
            init_declarator_list_add(&init_declarator_list, p_init_declarator);
            p_init_declarator = ((void *)0);
        }
    }
    else _catch_label_1:
    {
    }
    return init_declarator_list;
}

void storage_class_specifier_delete(struct storage_class_specifier * p)
{
    if (p)
    {
        free(p);
    }
}

struct storage_class_specifier *storage_class_specifier(struct parser_ctx * ctx)
{
    struct storage_class_specifier * p_storage_class_specifier = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_storage_class_specifier = calloc(1, sizeof (struct storage_class_specifier));
        if (p_storage_class_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_storage_class_specifier->token = ctx->current;
        switch (ctx->current->type)
        {
            case 9035 :
            p_storage_class_specifier->flags = 1;
            break;
            case 9013 :
            p_storage_class_specifier->flags = 2;
            break;
            case 9002 :
            p_storage_class_specifier->flags = 64;
            if (ctx->scopes.tail && ctx->scopes.tail->scope_level == 0)
            {
                p_storage_class_specifier->flags = 128;
            }
            break;
            case 9032 :
            p_storage_class_specifier->flags = 4;
            break;
            case 9057 :
            p_storage_class_specifier->flags = 8;
            break;
            case 8999 :
            p_storage_class_specifier->flags = 16;
            break;
            case 9025 :
            p_storage_class_specifier->flags = 32;
            break;
            default:
            ;
        }
        parser_match(ctx);
    }
    else _catch_label_1:
    {
        storage_class_specifier_delete(p_storage_class_specifier);
        p_storage_class_specifier = ((void *)0);
    }
    return p_storage_class_specifier;
}

struct type_name *type_name(struct parser_ctx * ctx);
struct expression *expression(struct parser_ctx * ctx);
void typeof_specifier_argument_delete(struct typeof_specifier_argument * p);

struct typeof_specifier_argument *typeof_specifier_argument(struct parser_ctx * ctx)
{
    struct typeof_specifier_argument * new_typeof_specifier_argument = ((void *)0);
    if (1)
    {
        new_typeof_specifier_argument = calloc(1, sizeof (struct typeof_specifier_argument));
        if (new_typeof_specifier_argument == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (first_of_type_name(ctx))
        {
            new_typeof_specifier_argument->type_name = type_name(ctx);
        }
        else
        {
            unsigned char   disable_evaluation_copy = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = 1;
            new_typeof_specifier_argument->expression = expression(ctx);
            ctx->evaluation_is_disabled = disable_evaluation_copy;
            if (new_typeof_specifier_argument->expression == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
    }
    else _catch_label_1:
    {
        typeof_specifier_argument_delete(new_typeof_specifier_argument);
        new_typeof_specifier_argument = ((void *)0);
    }
    return new_typeof_specifier_argument;
}

unsigned char  first_of_typeof_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 9058 || ctx->current->type == 9062;
}

struct type type_param_array_to_pointer(struct type * p_type, unsigned char   null_checks_enabled);
void type_remove_qualifiers(struct type * p_type);
void typeof_specifier_delete(struct typeof_specifier * p);

struct typeof_specifier *typeof_specifier(struct parser_ctx * ctx)
{
    struct typeof_specifier * p_typeof_specifier = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_typeof_specifier = calloc(1, sizeof (struct typeof_specifier));
        if (p_typeof_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_typeof_specifier->first_token = ctx->current;
        unsigned char   is_typeof_unqual = ctx->current->type == 9062;
        parser_match(ctx);
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto _catch_label_1;
        }
        struct typeof_specifier_argument * p_typeof_specifier_argument = typeof_specifier_argument(ctx);
        if (p_typeof_specifier_argument == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_typeof_specifier->typeof_specifier_argument = p_typeof_specifier_argument;
        if (p_typeof_specifier->typeof_specifier_argument->expression)
        {
            p_typeof_specifier->type = type_dup(&p_typeof_specifier->typeof_specifier_argument->expression->type);
        }
        else
        {
            if (p_typeof_specifier->typeof_specifier_argument->type_name)
            {
                p_typeof_specifier->type = type_dup(&p_typeof_specifier->typeof_specifier_argument->type_name->abstract_declarator->type);
            }
        }
        if (p_typeof_specifier->type.storage_class_specifier_flags & 2048)
        {
            compiler_diagnostic_message(7, ctx, ctx->current, ((void *)0), "typeof used in array arguments");
            if (type_is_array(&p_typeof_specifier->type))
            {
                struct type  t = type_param_array_to_pointer(&p_typeof_specifier->type, ctx->options.null_checks_enabled);
                type_swap(&t, &p_typeof_specifier->type);
                type_destroy(&t);
            }
        }
        if (is_typeof_unqual)
        {
            type_remove_qualifiers(&p_typeof_specifier->type);
        }
        type_visit_to_mark_anonymous(&p_typeof_specifier->type);
        free((void *)p_typeof_specifier->type.name_opt);
        p_typeof_specifier->type.name_opt = ((void *)0);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_typeof_specifier->last_token = ctx->current;
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        typeof_specifier_delete(p_typeof_specifier);
        p_typeof_specifier = ((void *)0);
    }
    return p_typeof_specifier;
}

void expression_delete(struct expression * p);
void type_name_delete(struct type_name * p);

void typeof_specifier_argument_delete(struct typeof_specifier_argument * p)
{
    if (p)
    {
        expression_delete(p->expression);
        type_name_delete(p->type_name);
        free(p);
    }
}

void typeof_specifier_delete(struct typeof_specifier * p)
{
    if (p)
    {
        typeof_specifier_argument_delete(p->typeof_specifier_argument);
        type_destroy(&p->type);
        free(p);
    }
}

void struct_or_union_specifier_delete(struct struct_or_union_specifier * p);
void enum_specifier_delete(struct enum_specifier * p);
void atomic_type_specifier_delete(struct atomic_type_specifier * p);

void type_specifier_delete(struct type_specifier * p)
{
    if (p)
    {
        struct_or_union_specifier_delete(p->struct_or_union_specifier);
        typeof_specifier_delete(p->typeof_specifier);
        enum_specifier_delete(p->enum_specifier);
        atomic_type_specifier_delete(p->atomic_type_specifier);
        free(p);
    }
}

struct atomic_type_specifier *atomic_type_specifier(struct parser_ctx * ctx);
struct struct_or_union_specifier *struct_or_union_specifier(struct parser_ctx * ctx);
struct enum_specifier *enum_specifier(struct parser_ctx *);

struct type_specifier *type_specifier(struct parser_ctx * ctx)
{
    struct type_specifier * p_type_specifier = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_type_specifier = calloc(1, sizeof  *p_type_specifier);
        if (p_type_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        switch (ctx->current->type)
        {
            case 9040 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 1;
            parser_match(ctx);
            return p_type_specifier;
            case 9003 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 2;
            parser_match(ctx);
            return p_type_specifier;
            case 9028 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 4;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9019 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 8;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9021 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 262144;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9022 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 524288;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9023 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 1048576;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9024 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 2097152;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9020 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 16;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9014 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 32;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9010 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 64;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9029 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 128;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9039 :
            p_type_specifier->flags = 256;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9047 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 512;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9048 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 1024;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9050 :
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 2048;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9051 :
            p_type_specifier->flags = 4096;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            case 9049 :
            p_type_specifier->flags = 8192;
            p_type_specifier->token = ctx->current;
            parser_match(ctx);
            return p_type_specifier;
            default:
            break;
        }
        if (first_of_typeof_specifier(ctx))
        {
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 8388608;
            p_type_specifier->typeof_specifier = typeof_specifier(ctx);
            if (p_type_specifier->typeof_specifier == ((void *)0))
            {
                type_specifier_delete(p_type_specifier);
                return ((void *)0);
            }
        }
        else
        {
            if (first_of_atomic_type_specifier(ctx))
            {
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 16384;
                p_type_specifier->atomic_type_specifier = atomic_type_specifier(ctx);
                if (p_type_specifier->atomic_type_specifier == ((void *)0))
                {
                    type_specifier_delete(p_type_specifier);
                    return ((void *)0);
                }
            }
            else
            {
                if (first_of_struct_or_union(ctx))
                {
                    p_type_specifier->token = ctx->current;
                    p_type_specifier->flags = 32768;
                    p_type_specifier->struct_or_union_specifier = struct_or_union_specifier(ctx);
                    if (p_type_specifier->struct_or_union_specifier == ((void *)0))
                    {
                        type_specifier_delete(p_type_specifier);
                        return ((void *)0);
                    }
                }
                else
                {
                    if (first_of_enum_specifier(ctx))
                    {
                        p_type_specifier->token = ctx->current;
                        p_type_specifier->flags = 65536;
                        p_type_specifier->enum_specifier = enum_specifier(ctx);
                        if (p_type_specifier->enum_specifier == ((void *)0))
                        {
                            type_specifier_delete(p_type_specifier);
                            return ((void *)0);
                        }
                    }
                    else
                    {
                        if (ctx->current->type == 9063)
                        {
                            type_specifier_delete(p_type_specifier);
                            return ((void *)0);
                        }
                        else
                        {
                            if (ctx->current->type == 8996)
                            {
                                p_type_specifier->token = ctx->current;
                                p_type_specifier->flags = 131072;
                                p_type_specifier->typedef_declarator = find_declarator(ctx, ctx->current->lexeme, ((void *)0));
                                ;
                                parser_match(ctx);
                            }
                        }
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
        type_specifier_delete(p_type_specifier);
        p_type_specifier = ((void *)0);
    }
    return p_type_specifier;
}

struct enum_specifier *get_complete_enum_specifier(struct enum_specifier * p_enum_specifier)
{
    if (p_enum_specifier->enumerator_list.head)
    {
        return p_enum_specifier;
    }
    else
    {
        if (p_enum_specifier->complete_enum_specifier2 && p_enum_specifier->complete_enum_specifier2->enumerator_list.head)
        {
            return p_enum_specifier->complete_enum_specifier2;
        }
        else
        {
            if (p_enum_specifier->complete_enum_specifier2 && p_enum_specifier->complete_enum_specifier2->complete_enum_specifier2 && p_enum_specifier->complete_enum_specifier2->complete_enum_specifier2->enumerator_list.head)
            {
                return p_enum_specifier->complete_enum_specifier2->complete_enum_specifier2;
            }
        }
    }
    return ((void *)0);
}

struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    if (p_struct_or_union_specifier->member_declaration_list.head)
    {
        return p_struct_or_union_specifier;
    }
    else
    {
        if (p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection && p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection->member_declaration_list.head)
        {
            return p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection;
        }
        else
        {
            if (p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection && p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection->complete_struct_or_union_specifier_indirection && p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection->complete_struct_or_union_specifier_indirection->member_declaration_list.head)
            {
                return p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection->complete_struct_or_union_specifier_indirection;
            }
        }
    }
    return ((void *)0);
}

unsigned char  struct_or_union_specifier_is_complete(struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    return get_complete_struct_or_union_specifier(p_struct_or_union_specifier) != ((void *)0);
}

struct struct_or_union_specifier *struct_or_union_specifier_add_ref(struct struct_or_union_specifier * p)
{
    p->has_shared_ownership = 1;
    return (struct struct_or_union_specifier *)p;
}

unsigned char  struct_or_union_specifier_is_union(struct struct_or_union_specifier * p)
{
    return p->first_token->type == 9038;
}

void struct_or_union_specifier_sink(struct struct_or_union_specifier * p);
void member_declaration_list_destroy(struct member_declaration_list * p);

void struct_or_union_specifier_delete(struct struct_or_union_specifier * p)
{
    if (p)
    {
        if (p->has_shared_ownership > 0)
        {
            p->has_shared_ownership = 0;
            struct_or_union_specifier_sink(p);
            return;
        }
        member_declaration_list_destroy(&p->member_declaration_list);
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        free(p);
    }
}

int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
void naming_convention_struct_tag(struct parser_ctx * ctx, struct token * token);
struct member_declaration_list member_declaration_list(struct parser_ctx * ctx, struct struct_or_union_specifier *);

struct struct_or_union_specifier *struct_or_union_specifier(struct parser_ctx * ctx)
{
    struct struct_or_union_specifier * p_struct_or_union_specifier = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_struct_or_union_specifier = calloc(1, sizeof  *p_struct_or_union_specifier);
        if (p_struct_or_union_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current->type == 9033 || ctx->current->type == 9038)
        {
            p_struct_or_union_specifier->first_token = ctx->current;
            parser_match(ctx);
        }
        else
        {
            goto _catch_label_1;
        }
        p_struct_or_union_specifier->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        struct struct_or_union_specifier * p_first_tag_in_this_scope = ((void *)0);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 8996)
        {
            p_struct_or_union_specifier->tagtoken = ctx->current;
            snprintf(p_struct_or_union_specifier->tag_name, sizeof p_struct_or_union_specifier->tag_name, "%s", ctx->current->lexeme);
            struct map_entry * p_entry = hashmap_find(&ctx->scopes.tail->tags, ctx->current->lexeme);
            if (p_entry)
            {
                if (p_entry->type == 2)
                {
                    ;
                    p_first_tag_in_this_scope = p_entry->data.p_struct_or_union_specifier;
                    p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection = p_first_tag_in_this_scope;
                }
                else
                {
                    compiler_diagnostic_message(1030, ctx, ctx->current, ((void *)0), "use of '%s' with tag type that does not match previous declaration.", ctx->current->lexeme);
                }
            }
            else
            {
                struct struct_or_union_specifier * p_first_tag_previous_scopes = find_struct_or_union_specifier(ctx, ctx->current->lexeme);
                if (p_first_tag_previous_scopes == ((void *)0))
                {
                    p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
                    struct hash_item_set  item = {0};
                    item.p_struct_or_union_specifier = struct_or_union_specifier_add_ref(p_struct_or_union_specifier);
                    hashmap_set(&ctx->scopes.tail->tags, ctx->current->lexeme, &item);
                    hash_item_set_destroy(&item);
                }
                else
                {
                    p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection = p_first_tag_previous_scopes;
                }
            }
            parser_match(ctx);
        }
        else
        {
            snprintf(p_struct_or_union_specifier->tag_name, sizeof p_struct_or_union_specifier->tag_name, "_struct_tag_%d", s_anonymous_struct_count);
            s_anonymous_struct_count++;
            p_struct_or_union_specifier->has_anonymous_tag = 1;
            p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
            struct hash_item_set  item = {0};
            item.p_struct_or_union_specifier = struct_or_union_specifier_add_ref(p_struct_or_union_specifier);
            hashmap_set(&ctx->scopes.tail->tags, p_struct_or_union_specifier->tag_name, &item);
            hash_item_set_destroy(&item);
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 123)
        {
            struct struct_or_union_specifier * first = find_struct_or_union_specifier(ctx, p_struct_or_union_specifier->tag_name);
            if (first)
            {
                first->complete_struct_or_union_specifier_indirection = p_struct_or_union_specifier;
            }
            if (p_struct_or_union_specifier->tagtoken)
            {
                naming_convention_struct_tag(ctx, p_struct_or_union_specifier->tagtoken);
            }
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            struct token * firsttoken = ctx->current;
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type != 125)
            {
                p_struct_or_union_specifier->member_declaration_list = member_declaration_list(ctx, p_struct_or_union_specifier);
                if (p_struct_or_union_specifier->member_declaration_list.head == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            p_struct_or_union_specifier->member_declaration_list.first_token = firsttoken;
            p_struct_or_union_specifier->last_token = ctx->current;
            p_struct_or_union_specifier->member_declaration_list.last_token = ctx->current;
            if (parser_match_tk(ctx, 125) != 0)
            {
                goto _catch_label_1;
            }
        }
        else
        {
            p_struct_or_union_specifier->last_token = ctx->current;
        }
        struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);
        if (p_complete)
        {
            if (p_complete->attribute_specifier_sequence_opt && p_complete->attribute_specifier_sequence_opt->attributes_flags & 1)
            {
                if (p_struct_or_union_specifier->tagtoken)
                {
                    compiler_diagnostic_message(2, ctx, p_struct_or_union_specifier->first_token, ((void *)0), "'%s' is deprecated", p_struct_or_union_specifier->tagtoken->lexeme);
                }
                else
                {
                    compiler_diagnostic_message(2, ctx, p_struct_or_union_specifier->first_token, ((void *)0), "deprecated");
                }
            }
        }
    }
    else _catch_label_1:
    {
        struct_or_union_specifier_delete(p_struct_or_union_specifier);
        p_struct_or_union_specifier = ((void *)0);
    }
    return p_struct_or_union_specifier;
}

unsigned char  type_is_owner(struct type * p_type);
void naming_convention_struct_member(struct parser_ctx * ctx, struct token * token, struct type * type);
struct expression *constant_expression(struct parser_ctx * ctx, unsigned char   show_error_if_not_constant);
void member_declarator_delete(struct member_declarator * p);

struct member_declarator *member_declarator(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier, struct specifier_qualifier_list * p_specifier_qualifier_list)
{
    struct member_declarator * p_member_declarator = ((void *)0);
    if (1)
    {
        p_member_declarator = calloc(1, sizeof (struct member_declarator));
        if (p_member_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct token * p_token_name = ((void *)0);
        p_member_declarator->declarator = declarator(ctx, p_specifier_qualifier_list, ((void *)0), 0, &p_token_name);
        if (p_member_declarator->declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_member_declarator->declarator->name_opt = p_token_name;
        p_member_declarator->declarator->specifier_qualifier_list = p_specifier_qualifier_list;
        p_member_declarator->declarator->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
        if (type_is_owner(&p_member_declarator->declarator->type))
        {
            p_struct_or_union_specifier->is_owner = 1;
        }
        if (p_member_declarator->declarator->name_opt)
        {
            naming_convention_struct_member(ctx, p_member_declarator->declarator->name_opt, &p_member_declarator->declarator->type);
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 58)
        {
            parser_match(ctx);
            p_member_declarator->constant_expression = constant_expression(ctx, 1);
        }
    }
    else _catch_label_1:
    {
        member_declarator_delete(p_member_declarator);
        p_member_declarator = ((void *)0);
    }
    return p_member_declarator;
}

void member_declarator_delete(struct member_declarator * p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        ;
        declarator_delete(p->declarator);
        free(p);
    }
}

void member_declarator_list_add(struct member_declarator_list * list, struct member_declarator * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void member_declarator_list_delete(struct member_declarator_list * p)
{
    if (p)
    {
        struct member_declarator * item = p->head;
        while (item)
        {
            struct member_declarator * next = item->next;
            item->next = ((void *)0);
            member_declarator_delete(item);
            item = next;
        }
        free(p);
    }
}

struct member_declarator_list *member_declarator_list(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier, struct specifier_qualifier_list * p_specifier_qualifier_list)
{
    struct member_declarator_list * p_member_declarator_list = calloc(1, sizeof (struct member_declarator_list));
    if (1)
    {
        if (p_member_declarator_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct member_declarator * p_member_declarator = member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list);
        if (p_member_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        member_declarator_list_add(p_member_declarator_list, p_member_declarator);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        while (ctx->current->type == 44)
        {
            parser_match(ctx);
            struct member_declarator * p_member_declarator2 = member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list);
            if (p_member_declarator2 == ((void *)0))
            {
                goto _catch_label_1;
            }
            member_declarator_list_add(p_member_declarator_list, p_member_declarator2);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
        }
    }
    else _catch_label_1:
    {
        member_declarator_list_delete(p_member_declarator_list);
        p_member_declarator_list = ((void *)0);
    }
    return p_member_declarator_list;
}

void member_declaration_list_add(struct member_declaration_list * list, struct member_declaration * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void member_declaration_delete(struct member_declaration * p);

void member_declaration_list_destroy(struct member_declaration_list * p)
{
    struct member_declaration * item = p->head;
    while (item)
    {
        struct member_declaration * next = item->next;
        item->next = ((void *)0);
        member_declaration_delete(item);
        item = next;
    }
}

struct member_declaration *member_declaration(struct parser_ctx * ctx, struct struct_or_union_specifier *);

struct member_declaration_list member_declaration_list(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    struct member_declaration_list  list = {0};
    struct member_declaration * p_member_declaration = ((void *)0);
    if (1)
    {
        p_member_declaration = member_declaration(ctx, p_struct_or_union_specifier);
        if (p_member_declaration == ((void *)0))
        {
            goto _catch_label_1;
        }
        member_declaration_list_add(&list, p_member_declaration);
        p_member_declaration = ((void *)0);
        while (ctx->current && ctx->current->type != 125)
        {
            p_member_declaration = member_declaration(ctx, p_struct_or_union_specifier);
            if (p_member_declaration == ((void *)0))
            {
                goto _catch_label_1;
            }
            member_declaration_list_add(&list, p_member_declaration);
            p_member_declaration = ((void *)0);
        }
    }
    else _catch_label_1:
    {
        member_declaration_list_destroy(&list);
        struct member_declaration_list  __cmp_lt_0 = {0, 0, 0, 0};
        list = __cmp_lt_0;
    }
    return list;
}

void specifier_qualifier_list_delete(struct specifier_qualifier_list * p);
void static_assert_declaration_delete(struct static_assert_declaration * p);
void pragma_declaration_delete(struct pragma_declaration * p);

void member_declaration_delete(struct member_declaration * p)
{
    if (p)
    {
        ;
        specifier_qualifier_list_delete(p->specifier_qualifier_list);
        member_declarator_list_delete(p->member_declarator_list_opt);
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        static_assert_declaration_delete(p->static_assert_declaration);
        pragma_declaration_delete(p->pragma_declaration);
        free(p);
    }
}

struct specifier_qualifier_list *specifier_qualifier_list(struct parser_ctx * ctx);

struct member_declaration *member_declaration(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    struct member_declaration * p_member_declaration = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_member_declaration = calloc(1, sizeof (struct member_declaration));
        if (p_member_declaration == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current->type == 9055)
        {
            p_member_declaration->static_assert_declaration = static_assert_declaration(ctx);
        }
        else
        {
            if (ctx->current->type == 128)
            {
                p_member_declaration->pragma_declaration = pragma_declaration(ctx);
            }
            else
            {
                p_member_declaration->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
                p_member_declaration->specifier_qualifier_list = specifier_qualifier_list(ctx);
                if (p_member_declaration->specifier_qualifier_list == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (ctx->current == ((void *)0))
                {
                    unexpected_end_of_file(ctx);
                    goto _catch_label_1;
                }
                if (ctx->current->type != 59)
                {
                    p_member_declaration->member_declarator_list_opt = member_declarator_list(ctx, p_struct_or_union_specifier, p_member_declaration->specifier_qualifier_list);
                    if (p_member_declaration->member_declarator_list_opt == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                }
                if (parser_match_tk(ctx, 59) != 0)
                {
                    goto _catch_label_1;
                }
            }
        }
    }
    else _catch_label_1:
    {
        member_declaration_delete(p_member_declaration);
        p_member_declaration = ((void *)0);
    }
    return p_member_declaration;
}

struct member_declarator *find_member_declarator(struct member_declaration_list * list, char * name, int * p_member_index)
{
    if (list->head == ((void *)0))
    {
        return ((void *)0);
    }
    struct member_declaration * p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator = ((void *)0);
        if (p_member_declaration->member_declarator_list_opt)
        {
            p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
            while (p_member_declarator)
            {
                if (p_member_declarator->declarator)
                {
                    if (p_member_declarator->declarator->name_opt && strcmp(p_member_declarator->declarator->name_opt->lexeme, name) == 0)
                    {
                        return p_member_declarator;
                    }
                }
                ( *p_member_index)++;
                p_member_declarator = p_member_declarator->next;
            }
        }
        else
        {
            if (p_member_declaration->specifier_qualifier_list && p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_member_declaration->specifier_qualifier_list->struct_or_union_specifier);
                if (p_complete)
                {
                    p_member_declarator = find_member_declarator(&p_complete->member_declaration_list, name, p_member_index);
                    if (p_member_declarator)
                    {
                        return p_member_declarator;
                    }
                }
            }
        }
        p_member_declaration = p_member_declaration->next;
    }
    return ((void *)0);
}

static struct member_declarator *find_member_declarator_by_index_core(struct member_declaration_list * list, int member_index, int * p_count)
{
    if (list->head == ((void *)0))
    {
        return ((void *)0);
    }
    struct member_declaration * p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator = ((void *)0);
        if (p_member_declaration->member_declarator_list_opt)
        {
            p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
            while (p_member_declarator)
            {
                if (p_member_declarator->declarator)
                {
                    if (member_index ==  *p_count)
                    {
                        return p_member_declarator;
                    }
                }
                ( *p_count)++;
                p_member_declarator = p_member_declarator->next;
            }
        }
        else
        {
            if (p_member_declaration->specifier_qualifier_list && p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct member_declaration_list * p_member_declaration_list = &p_member_declaration->specifier_qualifier_list->struct_or_union_specifier->member_declaration_list;
                p_member_declarator = find_member_declarator_by_index_core(p_member_declaration_list, member_index, p_count);
                if (p_member_declarator)
                {
                    return p_member_declarator;
                }
            }
        }
        p_member_declaration = p_member_declaration->next;
    }
    return ((void *)0);
}

struct member_declarator *find_member_declarator_by_index(struct member_declaration_list * list, int member_index)
{
    int count = 0;
    return find_member_declarator_by_index_core(list, member_index, &count);
}

void print_specifier_qualifier_list(struct osstream * ss, unsigned char  * first, struct specifier_qualifier_list * p_specifier_qualifier_list)
{
    print_type_qualifier_flags(ss, first, p_specifier_qualifier_list->type_qualifier_flags);
    if (p_specifier_qualifier_list->enum_specifier)
    {
        ;
    }
    else
    {
        if (p_specifier_qualifier_list->struct_or_union_specifier)
        {
            ss_fprintf(ss, "struct %s", p_specifier_qualifier_list->struct_or_union_specifier->tag_name);
        }
        else
        {
            if (p_specifier_qualifier_list->typedef_declarator)
            {
                if (p_specifier_qualifier_list->typedef_declarator->name_opt)
                {
                    print_item(ss, first, p_specifier_qualifier_list->typedef_declarator->name_opt->lexeme);
                }
            }
            else
            {
                print_type_specifier_flags(ss, first, p_specifier_qualifier_list->type_specifier_flags);
            }
        }
    }
}

void specifier_qualifier_list_add(struct specifier_qualifier_list * list, struct type_specifier_qualifier * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void specifier_qualifier_list_delete(struct specifier_qualifier_list * p)
{
    if (p)
    {
        struct type_specifier_qualifier * item = p->head;
        while (item)
        {
            struct type_specifier_qualifier * next = item->next;
            item->next = ((void *)0);
            type_specifier_qualifier_delete(item);
            item = next;
        }
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence);
        free(p);
    }
}

struct specifier_qualifier_list *specifier_qualifier_list(struct parser_ctx * ctx)
{
    struct specifier_qualifier_list * p_specifier_qualifier_list = ((void *)0);
    if (1)
    {
        if ( !first_of_type_specifier(ctx) &&  !first_of_type_qualifier(ctx))
        {
            compiler_diagnostic_message(1040, ctx, ctx->current, ((void *)0), "type specifier or qualifier expected");
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_specifier_qualifier_list = calloc(1, sizeof (struct specifier_qualifier_list));
        if (p_specifier_qualifier_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_specifier_qualifier_list->first_token = ctx->current;
        while (ctx->current != ((void *)0) && (first_of_type_specifier(ctx) || first_of_type_qualifier(ctx)))
        {
            if (ctx->current->flags & 16)
            {
                if (p_specifier_qualifier_list->type_specifier_flags != 0)
                {
                    break;
                }
            }
            struct type_specifier_qualifier * p_type_specifier_qualifier = type_specifier_qualifier(ctx);
            if (p_type_specifier_qualifier == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (p_type_specifier_qualifier->type_specifier)
            {
                if (add_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags, p_type_specifier_qualifier->type_specifier->flags) != 0)
                {
                    type_specifier_qualifier_delete(p_type_specifier_qualifier);
                    goto _catch_label_1;
                }
                if (p_type_specifier_qualifier->type_specifier->struct_or_union_specifier)
                {
                    p_specifier_qualifier_list->struct_or_union_specifier = p_type_specifier_qualifier->type_specifier->struct_or_union_specifier;
                }
                else
                {
                    if (p_type_specifier_qualifier->type_specifier->enum_specifier)
                    {
                        p_specifier_qualifier_list->enum_specifier = p_type_specifier_qualifier->type_specifier->enum_specifier;
                    }
                    else
                    {
                        if (p_type_specifier_qualifier->type_specifier->typeof_specifier)
                        {
                            p_specifier_qualifier_list->typeof_specifier = p_type_specifier_qualifier->type_specifier->typeof_specifier;
                        }
                        else
                        {
                            if (p_type_specifier_qualifier->type_specifier->token->type == 8996)
                            {
                                p_specifier_qualifier_list->typedef_declarator = find_declarator(ctx, p_type_specifier_qualifier->type_specifier->token->lexeme, ((void *)0));
                            }
                        }
                    }
                }
            }
            else
            {
                if (p_type_specifier_qualifier->type_qualifier)
                {
                    p_specifier_qualifier_list->type_qualifier_flags = p_type_specifier_qualifier->type_qualifier->flags;
                }
            }
            ;
            p_specifier_qualifier_list->p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
            specifier_qualifier_list_add(p_specifier_qualifier_list, p_type_specifier_qualifier);
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        final_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags);
        struct token * p_previous_parser_token = previous_parser_token(ctx->current);
        if (p_previous_parser_token == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_specifier_qualifier_list->last_token = p_previous_parser_token;
    }
    else _catch_label_1:
    {
        specifier_qualifier_list_delete(p_specifier_qualifier_list);
        p_specifier_qualifier_list = ((void *)0);
    }
    return p_specifier_qualifier_list;
}

void type_qualifier_delete(struct type_qualifier * p)
{
    if (p)
    {
        ;
        free(p);
    }
}

void alignment_specifier_delete(struct alignment_specifier * p);

void type_specifier_qualifier_delete(struct type_specifier_qualifier * p)
{
    if (p)
    {
        ;
        if (p->type_qualifier)
        {
            ;
            free(p->type_qualifier);
        }
        alignment_specifier_delete(p->alignment_specifier);
        type_specifier_delete(p->type_specifier);
        free(p);
    }
}

struct type_qualifier *type_qualifier(struct parser_ctx * ctx);
struct alignment_specifier *alignment_specifier(struct parser_ctx * ctx);

struct type_specifier_qualifier *type_specifier_qualifier(struct parser_ctx * ctx)
{
    struct type_specifier_qualifier * type_specifier_qualifier = ((void *)0);
    if (1)
    {
        type_specifier_qualifier = calloc(1, sizeof  *type_specifier_qualifier);
        if (type_specifier_qualifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (first_of_type_specifier(ctx))
        {
            type_specifier_qualifier->type_specifier = type_specifier(ctx);
            if (type_specifier_qualifier->type_specifier == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        else
        {
            if (first_of_type_qualifier(ctx))
            {
                type_specifier_qualifier->type_qualifier = type_qualifier(ctx);
                if (type_specifier_qualifier->type_qualifier == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
            else
            {
                if (first_of_alignment_specifier(ctx))
                {
                    type_specifier_qualifier->alignment_specifier = alignment_specifier(ctx);
                    if (type_specifier_qualifier->alignment_specifier == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                }
                else
                {
                    ;
                }
            }
        }
    }
    else _catch_label_1:
    {
        type_specifier_qualifier_delete(type_specifier_qualifier);
        type_specifier_qualifier = ((void *)0);
    }
    return type_specifier_qualifier;
}

signed long long object_to_signed_long_long(struct object * a);

struct enumerator *find_enumerator_by_value(struct enum_specifier * p_enum_specifier, long long value)
{
    if (p_enum_specifier->enumerator_list.head == ((void *)0))
    {
        return ((void *)0);
    }
    struct enumerator * p = p_enum_specifier->enumerator_list.head;
    while (p)
    {
        if (object_to_signed_long_long(&p->value) == value)
        {
            return p;
        }
        p = p->next;
    }
    return ((void *)0);
}

struct enum_specifier *enum_specifier_add_ref(struct enum_specifier * p)
{
    p->has_shared_ownership = 1;
    return (struct enum_specifier *)p;
}

void enum_specifier_delete_sink(struct enum_specifier * p);
void enumerator_list_destroy(struct enumerator_list * p_enum_specifier);

void enum_specifier_delete(struct enum_specifier * p)
{
    if (p)
    {
        if (p->has_shared_ownership > 0)
        {
            p->has_shared_ownership = 0;
            enum_specifier_delete_sink(p);
            return;
        }
        specifier_qualifier_list_delete(p->specifier_qualifier_list);
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        enumerator_list_destroy(&p->enumerator_list);
        free(p);
    }
}

void naming_convention_enum_tag(struct parser_ctx * ctx, struct token * token);
struct enumerator_list enumerator_list(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier);

struct enum_specifier *enum_specifier(struct parser_ctx * ctx)
{
    struct enum_specifier * p_enum_specifier = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_enum_specifier = calloc(1, sizeof  *p_enum_specifier);
        if (p_enum_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_enum_specifier->first_token = ctx->current;
        if (parser_match_tk(ctx, 9012) != 0)
        {
            goto _catch_label_1;
        }
        p_enum_specifier->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 8996)
        {
            snprintf(p_enum_specifier->tag_name, sizeof p_enum_specifier->tag_name, "%s", ctx->current->lexeme);
            p_enum_specifier->tag_token = ctx->current;
            parser_match(ctx);
        }
        else
        {
            snprintf(p_enum_specifier->tag_name, sizeof p_enum_specifier->tag_name, "_anonymous_struct_%d", s_anonymous_struct_count);
            s_anonymous_struct_count++;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 58)
        {
            if ( !ctx->inside_generic_association)
            {
                parser_match(ctx);
                p_enum_specifier->specifier_qualifier_list = specifier_qualifier_list(ctx);
            }
            else
            {
            }
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 123)
        {
            if (p_enum_specifier->tag_token)
            {
                naming_convention_enum_tag(ctx, p_enum_specifier->tag_token);
            }
            p_enum_specifier->complete_enum_specifier2 = p_enum_specifier;
            if (parser_match_tk(ctx, 123) != 0)
            {
                goto _catch_label_1;
            }
            p_enum_specifier->enumerator_list = enumerator_list(ctx, p_enum_specifier);
            if (p_enum_specifier->enumerator_list.head == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type == 44)
            {
                parser_match(ctx);
            }
            if (parser_match_tk(ctx, 125) != 0)
            {
                goto _catch_label_1;
            }
            struct hash_item_set  item = {0};
            item.p_enum_specifier = enum_specifier_add_ref(p_enum_specifier);
            hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, &item);
            p_enum_specifier->complete_enum_specifier2 = p_enum_specifier;
            hash_item_set_destroy(&item);
        }
        else
        {
            struct enum_specifier * p_existing_enum_specifier = find_enum_specifier(ctx, p_enum_specifier->tag_token->lexeme);
            if (p_existing_enum_specifier)
            {
                p_enum_specifier->complete_enum_specifier2 = p_existing_enum_specifier;
            }
            else
            {
                struct hash_item_set  item = {0};
                item.p_enum_specifier = enum_specifier_add_ref(p_enum_specifier);
                hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, &item);
                p_enum_specifier->complete_enum_specifier2 = p_enum_specifier;
                hash_item_set_destroy(&item);
            }
        }
    }
    else _catch_label_1:
    {
        enum_specifier_delete(p_enum_specifier);
        p_enum_specifier = ((void *)0);
    }
    return p_enum_specifier;
}

void enumerator_list_add(struct enumerator_list * list, struct enumerator * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void enumerator_delete(struct enumerator * p);

void enumerator_list_destroy(struct enumerator_list * p)
{
    struct enumerator * item = p->head;
    while (item)
    {
        struct enumerator * next = item->next;
        item->next = ((void *)0);
        enumerator_delete(item);
        item = next;
    }
}

struct object object_make_signed_int(signed int value);
int  type_specifier_to_object_type(int   type_specifier_flags);
struct object object_cast(int   e, struct object * a);
struct enumerator *enumerator(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier, struct object * p_enumerator_value);

struct enumerator_list enumerator_list(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier)
{
    struct object  next_enumerator_value = object_make_signed_int(0);
    if (p_enum_specifier->specifier_qualifier_list)
    {
        int   vt = type_specifier_to_object_type(p_enum_specifier->specifier_qualifier_list->type_specifier_flags);
        next_enumerator_value = object_cast(vt, &next_enumerator_value);
    }
    struct enumerator_list  enumeratorlist = {0};
    struct enumerator * p_enumerator = ((void *)0);
    if (1)
    {
        p_enumerator = enumerator(ctx, p_enum_specifier, &next_enumerator_value);
        if (p_enumerator == ((void *)0))
        {
            goto _catch_label_1;
        }
        enumerator_list_add(&enumeratorlist, p_enumerator);
        while (ctx->current != ((void *)0) && ctx->current->type == 44)
        {
            parser_match(ctx);
            if (ctx->current && ctx->current->type != 125)
            {
                p_enumerator = enumerator(ctx, p_enum_specifier, &next_enumerator_value);
                if (p_enumerator == ((void *)0))
                {
                    goto _catch_label_1;
                }
                enumerator_list_add(&enumeratorlist, p_enumerator);
            }
        }
    }
    else _catch_label_1:
    {
        enumerator_list_destroy(&enumeratorlist);
        enumeratorlist.head = ((void *)0);
        enumeratorlist.tail = ((void *)0);
    }
    return enumeratorlist;
}

struct enumerator *enumerator_add_ref(struct enumerator * p)
{
    p->has_shared_ownership = 1;
    return (struct enumerator *)p;
}

void enumerator_sink(struct enumerator * p);

void enumerator_delete(struct enumerator * p)
{
    if (p)
    {
        if (p->has_shared_ownership > 0)
        {
            p->has_shared_ownership = 0;
            enumerator_sink(p);
            return;
        }
        ;
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        expression_delete(p->constant_expression_opt);
        free(p);
    }
}

void naming_convention_enumerator(struct parser_ctx * ctx, struct token * token);
int object_increment_value(struct object * a);

struct enumerator *enumerator(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier, struct object * p_next_enumerator_value)
{
    struct enumerator * p_enumerator = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_enumerator = calloc(1, sizeof (struct enumerator));
        if (p_enumerator == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_enumerator->enum_specifier = p_enum_specifier;
        struct token * name = ctx->current;
        naming_convention_enumerator(ctx, name);
        if (parser_match_tk(ctx, 8996) != 0)
        {
            goto _catch_label_1;
        }
        p_enumerator->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        p_enumerator->token = name;
        struct hash_item_set  item = {0};
        item.p_enumerator = enumerator_add_ref(p_enumerator);
        hashmap_set(&ctx->scopes.tail->variables, p_enumerator->token->lexeme, &item);
        hash_item_set_destroy(&item);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 61)
        {
            parser_match(ctx);
            ;
            p_enumerator->constant_expression_opt = constant_expression(ctx, 1);
            if (p_enumerator->constant_expression_opt == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_enumerator->value = p_enumerator->constant_expression_opt->object;
             *p_next_enumerator_value = p_enumerator->value;
            if (object_increment_value(p_next_enumerator_value) != 0)
            {
            }
        }
        else
        {
            p_enumerator->value =  *p_next_enumerator_value;
            if (object_increment_value(p_next_enumerator_value) != 0)
            {
            }
        }
    }
    else _catch_label_1:
    {
        enumerator_delete(p_enumerator);
        p_enumerator = ((void *)0);
    }
    return p_enumerator;
}

void alignment_specifier_delete(struct alignment_specifier * p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        type_name_delete(p->type_name);
        free(p);
    }
}

struct alignment_specifier *alignment_specifier(struct parser_ctx * ctx)
{
    struct alignment_specifier * alignment_specifier = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        alignment_specifier = calloc(1, sizeof  *alignment_specifier);
        if (alignment_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        alignment_specifier->token = ctx->current;
        if (parser_match_tk(ctx, 9043) != 0)
        {
            goto _catch_label_1;
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto _catch_label_1;
        }
        if (first_of_type_name(ctx))
        {
            alignment_specifier->type_name = type_name(ctx);
            if (alignment_specifier->type_name == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        else
        {
            alignment_specifier->constant_expression = constant_expression(ctx, 1);
            if (alignment_specifier->constant_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
    }
    return alignment_specifier;
}

void atomic_type_specifier_delete(struct atomic_type_specifier * p)
{
    if (p)
    {
        type_name_delete(p->type_name);
        free(p);
    }
}

struct atomic_type_specifier *atomic_type_specifier(struct parser_ctx * ctx)
{
    struct atomic_type_specifier * p_atomic_type_specifier = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_atomic_type_specifier = calloc(1, sizeof  *p_atomic_type_specifier);
        if (p_atomic_type_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_atomic_type_specifier->token = ctx->current;
        if (parser_match_tk(ctx, 9045) != 0)
        {
            goto _catch_label_1;
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto _catch_label_1;
        }
        struct type_name * ptemp = type_name(ctx);
        if (ptemp == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_atomic_type_specifier->type_name = ptemp;
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        atomic_type_specifier_delete(p_atomic_type_specifier);
        p_atomic_type_specifier = ((void *)0);
    }
    return p_atomic_type_specifier;
}

struct type_qualifier *type_qualifier(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        unexpected_end_of_file(ctx);
        return ((void *)0);
    }
    struct type_qualifier * p_type_qualifier = calloc(1, sizeof  *p_type_qualifier);
    if (p_type_qualifier == ((void *)0))
    {
        return ((void *)0);
    }
    switch (ctx->current->type)
    {
        case 9004 :
        p_type_qualifier->flags = 1;
        break;
        case 9026 :
        p_type_qualifier->flags = 2;
        break;
        case 9041 :
        p_type_qualifier->flags = 4;
        break;
        case 9045 :
        p_type_qualifier->flags = 8;
        break;
        default:
        break;
    }
    if (ctx->options.ownership_enabled)
    {
        switch (ctx->current->type)
        {
            case 9065 :
            p_type_qualifier->flags = 512;
            break;
            case 9064 :
            p_type_qualifier->flags = 16;
            break;
            case 9066 :
            p_type_qualifier->flags = 32;
            break;
            case 9067 :
            p_type_qualifier->flags = 64;
            break;
            default:
            break;
        }
    }
    if (ctx->options.null_checks_enabled)
    {
        switch (ctx->current->type)
        {
            case 9068 :
            p_type_qualifier->flags = 128;
            break;
            default:
            break;
        }
    }
    p_type_qualifier->token = ctx->current;
    parser_match(ctx);
    return p_type_qualifier;
}

struct type_qualifier *type_qualifier_opt(struct parser_ctx * ctx)
{
    if (first_of_type_qualifier(ctx))
    {
        return type_qualifier(ctx);
    }
    return ((void *)0);
}

void function_specifier_delete(struct function_specifier * p)
{
    if (p)
    {
        free(p);
    }
}

struct function_specifier *function_specifier(struct parser_ctx * ctx)
{
    struct function_specifier * p_function_specifier = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_function_specifier = calloc(1, sizeof  *p_function_specifier);
        if (p_function_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current->type == 9054)
        {
            compiler_diagnostic_message(10, ctx, ctx->current, ((void *)0), "_Noreturn is deprecated use attributes");
        }
        if (ctx->current->type == 9018)
        {
            p_function_specifier->flags = 1;
        }
        p_function_specifier->token = ctx->current;
        parser_match(ctx);
    }
    else _catch_label_1:
    {
        function_specifier_delete(p_function_specifier);
        p_function_specifier = ((void *)0);
    }
    return p_function_specifier;
}

struct declarator *declarator_add_ref(struct declarator * p)
{
    p->has_shared_ownership = 1;
    return (struct declarator *)p;
}

void declarator_sink(struct declarator * p);
void direct_declarator_delete(struct direct_declarator * p);
void pointer_delete(struct pointer * p);

void declarator_delete(struct declarator * p)
{
    if (p)
    {
        if (p->has_shared_ownership > 0)
        {
            p->has_shared_ownership = 0;
            declarator_sink(p);
            return;
        }
        type_destroy(&p->type);
        direct_declarator_delete(p->direct_declarator);
        pointer_delete(p->pointer);
        free(p);
    }
}

struct pointer *pointer_opt(struct parser_ctx * ctx);
struct direct_declarator *direct_declarator(struct parser_ctx * ctx, struct specifier_qualifier_list * specifier_qualifier_list, struct declaration_specifiers * declaration_specifiers, unsigned char   abstract_acceptable, struct token ** pptoken_name);

struct declarator *declarator(struct parser_ctx * ctx, struct specifier_qualifier_list * p_specifier_qualifier_list_opt, struct declaration_specifiers * p_declaration_specifiers_opt, unsigned char   abstract_acceptable, struct token ** pp_token_name_opt)
{
    struct declarator * p_declarator = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_declarator = calloc(1, sizeof (struct declarator));
        if (p_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_declarator->first_token_opt = ctx->current;
        p_declarator->pointer = pointer_opt(ctx);
        p_declarator->direct_declarator = direct_declarator(ctx, p_specifier_qualifier_list_opt, p_declaration_specifiers_opt, abstract_acceptable, pp_token_name_opt);
        if (p_declarator->direct_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current != p_declarator->first_token_opt)
        {
            p_declarator->last_token_opt = previous_parser_token(ctx->current);
        }
        else
        {
            p_declarator->last_token_opt = p_declarator->first_token_opt;
            p_declarator->first_token_opt = ((void *)0);
        }
    }
    else _catch_label_1:
    {
        declarator_delete(p_declarator);
        p_declarator = ((void *)0);
    }
    return p_declarator;
}

char *declarator_get_name(struct declarator * p_declarator)
{
    if (p_declarator->direct_declarator)
    {
        if (p_declarator->direct_declarator->name_opt)
        {
            return p_declarator->direct_declarator->name_opt->lexeme;
        }
    }
    return ((void *)0);
}

unsigned char  declarator_is_function(struct declarator * p_declarator)
{
    return (p_declarator->direct_declarator && p_declarator->direct_declarator->function_declarator != ((void *)0));
}

void parameter_type_list_delete(struct parameter_type_list * p);

void function_declarator_delete(struct function_declarator * p)
{
    if (p)
    {
        direct_declarator_delete(p->direct_declarator);
        scope_destroy(&p->parameters_scope);
        parameter_type_list_delete(p->parameter_type_list_opt);
        free(p);
    }
}

void array_declarator_delete(struct array_declarator * p);

void direct_declarator_delete(struct direct_declarator * p)
{
    if (p)
    {
        declarator_delete(p->declarator);
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        array_declarator_delete(p->array_declarator);
        function_declarator_delete(p->function_declarator);
        free(p);
    }
}

struct array_declarator *array_declarator(struct direct_declarator * p_direct_declarator, struct parser_ctx * ctx);
struct function_declarator *function_declarator(struct direct_declarator * p_direct_declarator, struct parser_ctx * ctx);

struct direct_declarator *direct_declarator(struct parser_ctx * ctx, struct specifier_qualifier_list * p_specifier_qualifier_list, struct declaration_specifiers * p_declaration_specifiers, unsigned char   abstract_acceptable, struct token ** pp_token_name_opt)
{
    struct direct_declarator * p_direct_declarator = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_direct_declarator = calloc(1, sizeof (struct direct_declarator));
        if (p_direct_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct token * p_token_ahead = parser_look_ahead(ctx);
        if (p_token_ahead == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current->type == 8996)
        {
            p_direct_declarator->name_opt = ctx->current;
            if (pp_token_name_opt != ((void *)0))
            {
                 *pp_token_name_opt = ctx->current;
            }
            parser_match(ctx);
            p_direct_declarator->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        }
        else
        {
            if (ctx->current->type == 40)
            {
                struct token * ahead = parser_look_ahead(ctx);
                if (ahead == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if ( !first_of_type_specifier_token(ctx, p_token_ahead) &&  !first_of_type_qualifier_token(p_token_ahead) && ahead->type != 41 && ahead->type != 11822)
                {
                    parser_match(ctx);
                    struct declarator * p_declarator_temp = declarator(ctx, p_specifier_qualifier_list, p_declaration_specifiers, abstract_acceptable, pp_token_name_opt);
                    if (p_declarator_temp == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    p_direct_declarator->declarator = p_declarator_temp;
                    parser_match(ctx);
                }
            }
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 91 || ctx->current->type == 40))
        {
            struct direct_declarator * p_direct_declarator2 = calloc(1, sizeof (struct direct_declarator));
            if (p_direct_declarator2 == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (ctx->current->type == 91)
            {
                p_direct_declarator2->array_declarator = array_declarator(p_direct_declarator, ctx);
                p_direct_declarator = ((void *)0);
                if (p_direct_declarator2->array_declarator == ((void *)0))
                {
                    direct_declarator_delete(p_direct_declarator2);
                    goto _catch_label_1;
                }
            }
            else
            {
                p_direct_declarator2->function_declarator = function_declarator(p_direct_declarator, ctx);
                p_direct_declarator = ((void *)0);
                if (p_direct_declarator2->function_declarator == ((void *)0))
                {
                    direct_declarator_delete(p_direct_declarator2);
                    goto _catch_label_1;
                }
            }
            p_direct_declarator = p_direct_declarator2;
        }
    }
    else _catch_label_1:
    {
        direct_declarator_delete(p_direct_declarator);
        p_direct_declarator = ((void *)0);
    }
    return p_direct_declarator;
}

void type_qualifier_list_delete(struct type_qualifier_list * p);

void array_declarator_delete(struct array_declarator * p)
{
    if (p)
    {
        expression_delete(p->assignment_expression);
        direct_declarator_delete(p->direct_declarator);
        expression_delete(p->expression);
        type_qualifier_list_delete(p->type_qualifier_list_opt);
        free(p);
    }
}

unsigned char  object_has_constant_value(struct object * a);
unsigned long long object_to_unsigned_long_long(struct object * a);

unsigned long long array_declarator_get_size(struct array_declarator * p_array_declarator)
{
    if (p_array_declarator->assignment_expression)
    {
        if (object_has_constant_value(&p_array_declarator->assignment_expression->object))
        {
            return object_to_unsigned_long_long(&p_array_declarator->assignment_expression->object);
        }
    }
    return 0;
}

struct type_qualifier_list *type_qualifier_list(struct parser_ctx * ctx);
struct expression *assignment_expression(struct parser_ctx * ctx);

struct array_declarator *array_declarator(struct direct_declarator * p_direct_declarator, struct parser_ctx * ctx)
{
    struct array_declarator * p_array_declarator = ((void *)0);
    if (1)
    {
        p_array_declarator = calloc(1, sizeof  *p_array_declarator);
        if (p_array_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_array_declarator->direct_declarator = p_direct_declarator;
        p_direct_declarator = ((void *)0);
        if (parser_match_tk(ctx, 91) != 0)
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        unsigned char   has_static = 0;
        if (ctx->current->type == 9032)
        {
            p_array_declarator->static_token_opt = ctx->current;
            parser_match(ctx);
            has_static = 1;
        }
        if (first_of_type_qualifier(ctx))
        {
            p_array_declarator->type_qualifier_list_opt = type_qualifier_list(ctx);
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if ( !has_static)
        {
            if (ctx->current->type == 9032)
            {
                parser_match(ctx);
                has_static = 1;
            }
        }
        if (has_static)
        {
            unsigned char   evaluation_is_disabled = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = 0;
            p_array_declarator->assignment_expression = assignment_expression(ctx);
            ctx->evaluation_is_disabled = evaluation_is_disabled;
            if (p_array_declarator->assignment_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        else
        {
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type == 42)
            {
                parser_match(ctx);
            }
            else
            {
                if (ctx->current->type != 93)
                {
                    unsigned char   evaluation_is_disabled = ctx->evaluation_is_disabled;
                    ctx->evaluation_is_disabled = 0;
                    p_array_declarator->assignment_expression = assignment_expression(ctx);
                    ctx->evaluation_is_disabled = evaluation_is_disabled;
                    if (p_array_declarator->assignment_expression == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                }
                else
                {
                }
            }
        }
        if (parser_match_tk(ctx, 93) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        array_declarator_delete(p_array_declarator);
        p_array_declarator = ((void *)0);
    }
    direct_declarator_delete(p_direct_declarator);
    return p_array_declarator;
}

struct parameter_type_list *parameter_type_list(struct parser_ctx * ctx);

struct function_declarator *function_declarator(struct direct_declarator * p_direct_declarator_arg, struct parser_ctx * ctx)
{
    struct direct_declarator * p_direct_declarator = p_direct_declarator_arg;
    struct function_declarator * p_function_declarator = calloc(1, sizeof (struct function_declarator));
    if (1)
    {
        if (p_function_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_function_declarator->direct_declarator = p_direct_declarator;
        p_direct_declarator = ((void *)0);
        p_function_declarator->parameters_scope.scope_level = ctx->scopes.tail->scope_level + 1;
        p_function_declarator->parameters_scope.variables.capacity = 5;
        p_function_declarator->parameters_scope.tags.capacity = 1;
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type != 41)
        {
            scope_list_push(&ctx->scopes, &p_function_declarator->parameters_scope);
            p_function_declarator->parameter_type_list_opt = parameter_type_list(ctx);
            scope_list_pop(&ctx->scopes);
            if (p_function_declarator->parameter_type_list_opt == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        function_declarator_delete(p_function_declarator);
        p_function_declarator = ((void *)0);
    }
    direct_declarator_delete(p_direct_declarator);
    return p_function_declarator;
}

void pointer_delete(struct pointer * p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        type_qualifier_list_delete(p->type_qualifier_list_opt);
        struct pointer * item = p->pointer;
        while (item)
        {
            struct pointer * next = item->pointer;
            attribute_specifier_sequence_delete(item->attribute_specifier_sequence_opt);
            type_qualifier_list_delete(item->type_qualifier_list_opt);
            free(item);
            item = next;
        }
        free(p);
    }
}

struct pointer *pointer_opt(struct parser_ctx * ctx)
{
    struct pointer * p = ((void *)0);
    struct pointer * p_pointer = ((void *)0);
    if (1)
    {
        while (ctx->current != ((void *)0) && ctx->current->type == 42)
        {
            p_pointer = calloc(1, sizeof (struct pointer));
            if (p_pointer == ((void *)0))
            {
                goto _catch_label_1;
            }
            p = p_pointer;
            parser_match(ctx);
            p_pointer->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
            if (first_of_type_qualifier(ctx))
            {
                ;
                if (ctx->current->type == 9067)
                {
                    compiler_diagnostic_message(640, ctx, ctx->current, ((void *)0), "invalid qualifier for pointer");
                }
                else
                {
                    p_pointer->type_qualifier_list_opt = type_qualifier_list(ctx);
                }
            }
            while (ctx->current != ((void *)0) && ctx->current->type == 42)
            {
                p_pointer->pointer = pointer_opt(ctx);
                if (p_pointer->pointer == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
    return p;
}

void type_qualifier_list_add(struct type_qualifier_list * list, struct type_qualifier * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void type_qualifier_list_delete(struct type_qualifier_list * p)
{
    if (p)
    {
        struct type_qualifier * item = p->head;
        while (item)
        {
            struct type_qualifier * next = item->next;
            item->next = ((void *)0);
            free(item);
            item = next;
        }
        free(p);
    }
}

struct type_qualifier_list *type_qualifier_list(struct parser_ctx * ctx)
{
    struct type_qualifier_list * p_type_qualifier_list = ((void *)0);
    struct type_qualifier * p_type_qualifier = ((void *)0);
    if (1)
    {
        p_type_qualifier_list = calloc(1, sizeof (struct type_qualifier_list));
        if (p_type_qualifier_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_type_qualifier = type_qualifier(ctx);
        if (p_type_qualifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_type_qualifier_list->flags = p_type_qualifier->flags;
        type_qualifier_list_add(p_type_qualifier_list, p_type_qualifier);
        p_type_qualifier = ((void *)0);
        while (ctx->current != ((void *)0) && first_of_type_qualifier(ctx))
        {
            p_type_qualifier = type_qualifier(ctx);
            if (p_type_qualifier == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_type_qualifier_list->flags = p_type_qualifier->flags;
            type_qualifier_list_add(p_type_qualifier_list, p_type_qualifier);
            p_type_qualifier = ((void *)0);
        }
    }
    else _catch_label_1:
    {
    }
    return p_type_qualifier_list;
}

void parameter_list_delete(struct parameter_list * p);

void parameter_type_list_delete(struct parameter_type_list * p)
{
    if (p)
    {
        parameter_list_delete(p->parameter_list);
        free(p);
    }
}

struct parameter_list *parameter_list(struct parser_ctx * ctx);
unsigned char  type_is_void(struct type * p_type);

struct parameter_type_list *parameter_type_list(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return ((void *)0);
    }
    struct parameter_type_list * p_parameter_type_list = calloc(1, sizeof (struct parameter_type_list));
    if (1)
    {
        if (p_parameter_type_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_parameter_type_list->parameter_list = parameter_list(ctx);
        if (p_parameter_type_list->parameter_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (p_parameter_type_list->parameter_list->head == p_parameter_type_list->parameter_list->tail)
        {
            if (type_is_void(&p_parameter_type_list->parameter_list->head->declarator->type))
            {
                p_parameter_type_list->is_void = 1;
            }
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 11822)
        {
            parser_match(ctx);
            p_parameter_type_list->is_var_args = 1;
        }
    }
    else _catch_label_1:
    {
        parameter_type_list_delete(p_parameter_type_list);
        p_parameter_type_list = ((void *)0);
    }
    return p_parameter_type_list;
}

void parameter_list_add(struct parameter_list * list, struct parameter_declaration * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void parameter_declaration_delete(struct parameter_declaration * p);

void parameter_list_delete(struct parameter_list * p)
{
    if (p)
    {
        struct parameter_declaration * item = p->head;
        while (item)
        {
            struct parameter_declaration * next = item->next;
            item->next = ((void *)0);
            parameter_declaration_delete(item);
            item = next;
        }
        free(p);
    }
}

struct parameter_declaration *parameter_declaration(struct parser_ctx * ctx);

struct parameter_list *parameter_list(struct parser_ctx * ctx)
{
    struct parameter_list * p_parameter_list = ((void *)0);
    struct parameter_declaration * p_parameter_declaration = ((void *)0);
    if (1)
    {
        p_parameter_list = calloc(1, sizeof (struct parameter_list));
        if (p_parameter_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_parameter_declaration = parameter_declaration(ctx);
        if (p_parameter_declaration == ((void *)0))
        {
            goto _catch_label_1;
        }
        parameter_list_add(p_parameter_list, p_parameter_declaration);
        p_parameter_declaration = ((void *)0);
        while (ctx->current != ((void *)0) && ctx->current->type == 44)
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type == 11822)
            {
                break;
            }
            p_parameter_declaration = parameter_declaration(ctx);
            if (p_parameter_declaration == ((void *)0))
            {
                goto _catch_label_1;
            }
            parameter_list_add(p_parameter_list, p_parameter_declaration);
            p_parameter_declaration = ((void *)0);
        }
    }
    else _catch_label_1:
    {
        parameter_list_delete(p_parameter_list);
        p_parameter_list = ((void *)0);
    }
    return p_parameter_list;
}

void parameter_declaration_delete(struct parameter_declaration * p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        declaration_specifiers_delete(p->declaration_specifiers);
        declarator_delete(p->declarator);
        ;
        free(p);
    }
}

void naming_convention_parameter(struct parser_ctx * ctx, struct token * token, struct type * type);

struct parameter_declaration *parameter_declaration(struct parser_ctx * ctx)
{
    struct parameter_declaration * p_parameter_declaration = calloc(1, sizeof (struct parameter_declaration));
    if (1)
    {
        if (p_parameter_declaration == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_parameter_declaration->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        struct declaration_specifiers * p_declaration_specifiers = declaration_specifiers(ctx, 2048 | 4096);
        if (p_declaration_specifiers == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_parameter_declaration->declaration_specifiers = p_declaration_specifiers;
        struct token * p_token_name = ((void *)0);
        p_parameter_declaration->declarator = declarator(ctx, ((void *)0), p_parameter_declaration->declaration_specifiers, 1, &p_token_name);
        if (p_parameter_declaration->declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_parameter_declaration->declarator->name_opt = p_token_name;
        p_parameter_declaration->declarator->declaration_specifiers = p_parameter_declaration->declaration_specifiers;
        p_parameter_declaration->declarator->type = make_type_using_declarator(ctx, p_parameter_declaration->declarator);
        if (p_parameter_declaration->attribute_specifier_sequence_opt)
        {
            p_parameter_declaration->declarator->type.attributes_flags = p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags;
        }
        p_parameter_declaration->declarator->type.storage_class_specifier_flags = 2048;
        if (p_parameter_declaration->declarator->name_opt)
        {
            naming_convention_parameter(ctx, p_parameter_declaration->declarator->name_opt, &p_parameter_declaration->declarator->type);
        }
        if (p_parameter_declaration->declarator->name_opt)
        {
            struct hash_item_set  item = {0};
            item.p_declarator = declarator_add_ref(p_parameter_declaration->declarator);
            ;
            hashmap_set(&ctx->scopes.tail->variables, p_parameter_declaration->declarator->name_opt->lexeme, &item);
            hash_item_set_destroy(&item);
        }
    }
    else _catch_label_1:
    {
        parameter_declaration_delete(p_parameter_declaration);
        p_parameter_declaration = ((void *)0);
    }
    return p_parameter_declaration;
}

struct specifier_qualifier_list *copy(struct declaration_specifiers * p_declaration_specifiers)
{
    struct specifier_qualifier_list * p_specifier_qualifier_list = calloc(1, sizeof (struct specifier_qualifier_list));
    if (1)
    {
        if (p_specifier_qualifier_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_specifier_qualifier_list->type_qualifier_flags = p_declaration_specifiers->type_qualifier_flags;
        p_specifier_qualifier_list->type_specifier_flags = p_declaration_specifiers->type_specifier_flags;
        struct declaration_specifier * p_declaration_specifier = p_declaration_specifiers->head;
        while (p_declaration_specifier)
        {
            if (p_declaration_specifier->type_specifier_qualifier)
            {
                struct type_specifier_qualifier * p_specifier_qualifier = calloc(1, sizeof (struct type_specifier_qualifier));
                if (p_specifier_qualifier == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                {
                    struct type_qualifier * p_type_qualifier = calloc(1, sizeof (struct type_qualifier));
                    if (p_type_qualifier == ((void *)0))
                    {
                        type_specifier_qualifier_delete(p_specifier_qualifier);
                        goto _catch_label_1;
                    }
                    p_type_qualifier->flags = p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;
                    p_type_qualifier->token = p_declaration_specifier->type_specifier_qualifier->type_qualifier->token;
                    p_specifier_qualifier->type_qualifier = p_type_qualifier;
                }
                else
                {
                    if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                    {
                        struct type_specifier * p_type_specifier = calloc(1, sizeof (struct type_specifier));
                        if (p_type_specifier == ((void *)0))
                        {
                            type_specifier_qualifier_delete(p_specifier_qualifier);
                            goto _catch_label_1;
                        }
                        p_type_specifier->flags = p_declaration_specifier->type_specifier_qualifier->type_specifier->flags;
                        ;
                        p_type_specifier->token = p_declaration_specifier->type_specifier_qualifier->type_specifier->token;
                        p_specifier_qualifier->type_specifier = p_type_specifier;
                    }
                }
                specifier_qualifier_list_add(p_specifier_qualifier_list, p_specifier_qualifier);
            }
            p_declaration_specifier = p_declaration_specifier->next;
        }
    }
    else _catch_label_1:
    {
        specifier_qualifier_list_delete(p_specifier_qualifier_list);
        p_specifier_qualifier_list = ((void *)0);
    }
    return p_specifier_qualifier_list;
}

void print_declarator(struct osstream * ss, struct declarator * declarator, unsigned char   is_abstract);

void print_direct_declarator(struct osstream * ss, struct direct_declarator * p_direct_declarator, unsigned char   is_abstract)
{
    if (p_direct_declarator->declarator)
    {
        ss_fprintf(ss, "(");
        print_declarator(ss, p_direct_declarator->declarator, is_abstract);
        ss_fprintf(ss, ")");
    }
    if (p_direct_declarator->name_opt &&  !is_abstract)
    {
        ss_fprintf(ss, "%s", p_direct_declarator->name_opt->lexeme);
    }
    if (p_direct_declarator->function_declarator)
    {
        ;
        print_direct_declarator(ss, p_direct_declarator->function_declarator->direct_declarator, is_abstract);
        ss_fprintf(ss, "(");
        struct parameter_declaration * p_parameter_declaration = p_direct_declarator->function_declarator->parameter_type_list_opt ? p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head : ((void *)0);
        while (p_parameter_declaration)
        {
            if (p_parameter_declaration != p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head)
            {
                ss_fprintf(ss, ",");
            }
            print_declaration_specifiers(ss, p_parameter_declaration->declaration_specifiers);
            ss_fprintf(ss, " ");
            if (p_parameter_declaration->declarator)
            {
                print_declarator(ss, p_parameter_declaration->declarator, is_abstract);
            }
            p_parameter_declaration = p_parameter_declaration->next;
        }
        ss_fprintf(ss, ")");
    }
    if (p_direct_declarator->array_declarator)
    {
        ss_fprintf(ss, "[]");
    }
}

int  declarator_get_type_specifier_flags(struct declarator * p)
{
    if (p->declaration_specifiers)
    {
        return p->declaration_specifiers->type_specifier_flags;
    }
    if (p->specifier_qualifier_list)
    {
        return p->specifier_qualifier_list->type_specifier_flags;
    }
    return 0;
}

void print_declarator(struct osstream * ss, struct declarator * p_declarator, unsigned char   is_abstract)
{
    unsigned char   first = 1;
    if (p_declarator->pointer)
    {
        struct pointer * p = p_declarator->pointer;
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
    if (p_declarator->direct_declarator)
    {
        print_direct_declarator(ss, p_declarator->direct_declarator, is_abstract);
    }
}

void print_type_name(struct osstream * ss, struct type_name * p)
{
    unsigned char   first = 1;
    print_specifier_qualifier_list(ss, &first, p->specifier_qualifier_list);
    print_declarator(ss, p->abstract_declarator, 1);
}

void type_name_delete(struct type_name * p)
{
    if (p)
    {
        specifier_qualifier_list_delete(p->specifier_qualifier_list);
        type_destroy(&p->type);
        declarator_delete(p->abstract_declarator);
        free(p);
    }
}

struct type_name *type_name(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return ((void *)0);
    }
    struct type_name * p_type_name = calloc(1, sizeof (struct type_name));
    if (1)
    {
        if (p_type_name == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_type_name->first_token = ctx->current;
        struct specifier_qualifier_list * p_specifier_qualifier_list = specifier_qualifier_list(ctx);
        if (p_specifier_qualifier_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_type_name->specifier_qualifier_list = p_specifier_qualifier_list;
        struct declarator * p_declarator = declarator(ctx, p_type_name->specifier_qualifier_list, ((void *)0), 1, ((void *)0));
        if (p_declarator == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_type_name->abstract_declarator = p_declarator;
        p_type_name->abstract_declarator->specifier_qualifier_list = p_type_name->specifier_qualifier_list;
        p_type_name->abstract_declarator->type = make_type_using_declarator(ctx, p_type_name->abstract_declarator);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->prev == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_type_name->last_token = ctx->current->prev;
        p_type_name->type = type_dup(&p_type_name->abstract_declarator->type);
    }
    else _catch_label_1:
    {
        type_name_delete(p_type_name);
        p_type_name = ((void *)0);
    }
    return p_type_name;
}

void initializer_list_delete(struct initializer_list * p);

void braced_initializer_delete(struct braced_initializer * p)
{
    if (p)
    {
        initializer_list_delete(p->initializer_list);
        free(p);
    }
}

struct initializer_list *initializer_list(struct parser_ctx * ctx);

struct braced_initializer *braced_initializer(struct parser_ctx * ctx)
{
    struct braced_initializer * p_bracket_initializer_list = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_bracket_initializer_list = calloc(1, sizeof (struct braced_initializer));
        if (p_bracket_initializer_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_bracket_initializer_list->first_token = ctx->current;
        if (parser_match_tk(ctx, 123) != 0)
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type != 125)
        {
            p_bracket_initializer_list->initializer_list = initializer_list(ctx);
        }
        if (parser_match_tk(ctx, 125) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        braced_initializer_delete(p_bracket_initializer_list);
        p_bracket_initializer_list = ((void *)0);
    }
    return p_bracket_initializer_list;
}

void initializer_delete(struct initializer * p)
{
    if (p)
    {
        expression_delete(p->assignment_expression);
        braced_initializer_delete(p->braced_initializer);
        free(p);
    }
}

struct initializer *initializer(struct parser_ctx * ctx)
{
    struct initializer * p_initializer = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_initializer = calloc(1, sizeof (struct initializer));
        if (p_initializer == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_initializer->first_token = ctx->current;
        if (ctx->current->type == 123)
        {
            p_initializer->braced_initializer = braced_initializer(ctx);
            if (p_initializer->braced_initializer == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        else
        {
            p_initializer->assignment_expression = assignment_expression(ctx);
            if (p_initializer->assignment_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
    }
    else _catch_label_1:
    {
        initializer_delete(p_initializer);
        p_initializer = ((void *)0);
    }
    return p_initializer;
}

void defer_list_add(struct defer_list * list, struct defer_list_item * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void defer_list_destroy(struct defer_list * p)
{
    struct defer_list_item * item = p->head;
    while (item)
    {
        struct defer_list_item * next = item->next;
        item->next = ((void *)0);
        free(item);
        item = next;
    }
    free(p);
}

void initializer_list_add(struct initializer_list * list, struct initializer_list_item * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void designation_delete(struct designation * p);

void initializer_list_item_delete(struct initializer_list_item * p)
{
    if (p)
    {
        designation_delete(p->designation);
        initializer_delete(p->initializer);
        free(p);
    }
}

void initializer_list_delete(struct initializer_list * p)
{
    if (p)
    {
        struct initializer_list_item * item = p->head;
        while (item)
        {
            struct initializer_list_item * next = item->next;
            item->next = ((void *)0);
            initializer_list_item_delete(item);
            item = next;
        }
        free(p);
    }
}

struct designation *designation(struct parser_ctx * ctx);

struct initializer_list *initializer_list(struct parser_ctx * ctx)
{
    struct initializer_list * p_initializer_list = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            return ((void *)0);
        }
        p_initializer_list = calloc(1, sizeof (struct initializer_list));
        if (p_initializer_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_initializer_list->first_token = ctx->current;
        struct initializer_list_item * p_initializer_list_item = calloc(1, sizeof  *p_initializer_list_item);
        if (p_initializer_list_item == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct designation * p_designation = ((void *)0);
        if (first_of_designator(ctx))
        {
            p_designation = designation(ctx);
            if (p_designation == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_initializer_list_item->designation = p_designation;
        }
        struct initializer * p_initializer = initializer(ctx);
        if (p_initializer == ((void *)0))
        {
            designation_delete(p_designation);
            goto _catch_label_1;
        }
        p_initializer_list_item->initializer = p_initializer;
        initializer_list_add(p_initializer_list, p_initializer_list_item);
        p_initializer_list_item = ((void *)0);
        while (ctx->current != ((void *)0) && ctx->current->type == 44)
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type == 125)
            {
                break;
            }
            p_initializer_list_item = calloc(1, sizeof  *p_initializer_list_item);
            if (p_initializer_list_item == ((void *)0))
            {
                goto _catch_label_1;
            }
            struct designation * p_designation2 = ((void *)0);
            if (first_of_designator(ctx))
            {
                p_designation2 = designation(ctx);
                if (p_designation2 == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
            p_initializer_list_item->designation = p_designation2;
            struct initializer * p_initializer2 = initializer(ctx);
            if (p_initializer2 == ((void *)0))
            {
                designation_delete(p_designation2);
                goto _catch_label_1;
            }
            p_initializer_list_item->initializer = p_initializer2;
            initializer_list_add(p_initializer_list, p_initializer_list_item);
            p_initializer_list_item = ((void *)0);
            p_initializer_list->size++;
        }
    }
    else _catch_label_1:
    {
        initializer_list_delete(p_initializer_list);
        p_initializer_list = ((void *)0);
    }
    return p_initializer_list;
}

void designator_list_delete(struct designator_list * p);

void designation_delete(struct designation * p)
{
    if (p)
    {
        designator_list_delete(p->designator_list);
        free(p);
    }
}

struct designator_list *designator_list(struct parser_ctx * ctx);

struct designation *designation(struct parser_ctx * ctx)
{
    struct designation * p_designation = ((void *)0);
    if (1)
    {
        p_designation = calloc(1, sizeof (struct designation));
        if (p_designation == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct designator_list * p_designator_list = designator_list(ctx);
        if (p_designator_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_designation->designator_list = p_designator_list;
        if (parser_match_tk(ctx, 61) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        designation_delete(p_designation);
        p_designation = ((void *)0);
    }
    return p_designation;
}

void designator_list_add(struct designator_list * list, struct designator * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void designator_delete(struct designator * p);

void designator_list_delete(struct designator_list * p)
{
    if (p)
    {
        struct designator * item = p->head;
        while (item)
        {
            struct designator * next = item->next;
            item->next = ((void *)0);
            designator_delete(item);
            item = next;
        }
        free(p);
    }
}

struct designator *designator(struct parser_ctx * ctx);

struct designator_list *designator_list(struct parser_ctx * ctx)
{
    struct designator_list * p_designator_list = ((void *)0);
    struct designator * p_designator = ((void *)0);
    if (1)
    {
        p_designator_list = calloc(1, sizeof (struct designator_list));
        if (p_designator_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_designator = designator(ctx);
        if (p_designator == ((void *)0))
        {
            goto _catch_label_1;
        }
        designator_list_add(p_designator_list, p_designator);
        p_designator = ((void *)0);
        while (ctx->current != ((void *)0) && first_of_designator(ctx))
        {
            p_designator = designator(ctx);
            if (p_designator == ((void *)0))
            {
                goto _catch_label_1;
            }
            designator_list_add(p_designator_list, p_designator);
            p_designator = ((void *)0);
        }
    }
    else _catch_label_1:
    {
        designator_list_delete(p_designator_list);
        p_designator_list = ((void *)0);
    }
    return p_designator_list;
}

void designator_delete(struct designator * p)
{
    if (p)
    {
        ;
        expression_delete(p->constant_expression_opt);
        free(p);
    }
}

struct designator *designator(struct parser_ctx * ctx)
{
    struct designator * p_designator = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_designator = calloc(1, sizeof (struct designator));
        if (p_designator == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current->type == 91)
        {
            if (parser_match_tk(ctx, 91) != 0)
            {
                goto _catch_label_1;
            }
            p_designator->constant_expression_opt = constant_expression(ctx, 1);
            if (parser_match_tk(ctx, 93) != 0)
            {
                goto _catch_label_1;
            }
        }
        else
        {
            if (ctx->current->type == 46)
            {
                parser_match(ctx);
                p_designator->token = ctx->current;
                if (parser_match_tk(ctx, 8996) != 0)
                {
                    goto _catch_label_1;
                }
            }
        }
    }
    else _catch_label_1:
    {
        designator_delete(p_designator);
        p_designator = ((void *)0);
    }
    return p_designator;
}

void static_assert_declaration_delete(struct static_assert_declaration * p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        free(p);
    }
}

void pragma_declaration_delete(struct pragma_declaration * p)
{
    if (p)
    {
        free(p);
    }
}

int get_diagnostic_phase(int   w);
int atoi(char * _String);

void execute_pragma(struct parser_ctx * ctx, struct pragma_declaration * p_pragma, unsigned char   on_flow_analysis)
{
    struct token * p_pragma_token = p_pragma->first_token;
    if (p_pragma_token->type != 128)
    {
        ;
        return;
    }
    p_pragma_token = pragma_match(p_pragma_token);
    if (p_pragma_token && (strcmp(p_pragma_token->lexeme, "CAKE") == 0 || strcmp(p_pragma_token->lexeme, "cake") == 0))
    {
        p_pragma_token = pragma_match(p_pragma_token);
    }
    if (p_pragma_token && strcmp(p_pragma_token->lexeme, "diagnostic") == 0)
    {
        p_pragma_token = pragma_match(p_pragma_token);
        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "push") == 0)
        {
            if (ctx->options.diagnostic_stack.top_index < sizeof (ctx->options.diagnostic_stack) / sizeof (ctx->options.diagnostic_stack.stack[0]))
            {
                ctx->options.diagnostic_stack.top_index++;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index - 1];
            }
            p_pragma_token = p_pragma_token->next;
        }
        else
        {
            if (p_pragma_token && strcmp(p_pragma_token->lexeme, "pop") == 0)
            {
                if (ctx->options.diagnostic_stack.top_index > 0)
                {
                    ctx->options.diagnostic_stack.top_index--;
                }
                p_pragma_token = pragma_match(p_pragma_token);
            }
            else
            {
                if (p_pragma_token && (strcmp(p_pragma_token->lexeme, "error") == 0 || strcmp(p_pragma_token->lexeme, "warning") == 0 || strcmp(p_pragma_token->lexeme, "note") == 0 || strcmp(p_pragma_token->lexeme, "ignored") == 0))
                {
                    unsigned char   is_error = strcmp(p_pragma_token->lexeme, "error") == 0;
                    unsigned char   is_warning = strcmp(p_pragma_token->lexeme, "warning") == 0;
                    unsigned char   is_note = strcmp(p_pragma_token->lexeme, "note") == 0;
                    p_pragma_token = pragma_match(p_pragma_token);
                    if (p_pragma_token && p_pragma_token->type == 130)
                    {
                        unsigned long long w = get_warning_bit_mask(p_pragma_token->lexeme + 1);
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors =  ~w;
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes =  ~w;
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings =  ~w;
                        if (is_error)
                        {
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors = w;
                        }
                        else
                        {
                            if (is_warning)
                            {
                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings = w;
                            }
                            else
                            {
                                if (is_note)
                                {
                                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes = w;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (p_pragma_token && (strcmp(p_pragma_token->lexeme, "check") == 0))
                    {
                        p_pragma_token = pragma_match(p_pragma_token);
                        if (p_pragma_token && p_pragma_token->type == 130)
                        {
                            int   id = get_warning(p_pragma_token->lexeme + 1);
                            if (( !on_flow_analysis && get_diagnostic_phase(id) != 2) || (on_flow_analysis && get_diagnostic_phase(id) == 2))
                            {
                                unsigned char   found = 0;
                                {
                                    int i = 0;
                                    for (; i < (int)(sizeof (ctx->p_report->last_diagnostics_ids) / sizeof (ctx->p_report->last_diagnostics_ids[0])); i++)
                                    {
                                        if (ctx->p_report->last_diagnostics_ids[i] == 0)
                                        {
                                            break;
                                        }
                                        if (ctx->p_report->last_diagnostics_ids[i] == id)
                                        {
                                            found = 1;
                                            int t = get_diagnostic_type(&ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index], id);
                                            if (t == 3)
                                            {
                                                ctx->p_report->error_count--;
                                            }
                                            else
                                            {
                                                if (t == 2)
                                                {
                                                    ctx->p_report->warnings_count--;
                                                }
                                                else
                                                {
                                                    if (t == 1)
                                                    {
                                                        ctx->p_report->info_count--;
                                                    }
                                                }
                                            }
                                            break;
                                        }
                                    }
                                }
                                if ( !found)
                                {
                                    ctx->p_report->fatal_error_expected = atoi(p_pragma_token->lexeme + 3);
                                    compiler_diagnostic_message(650, ctx, p_pragma_token, ((void *)0), "pragma check failed");
                                }
                            }
                        }
                    }
                    else
                    {
                        compiler_diagnostic_message(650, ctx, p_pragma_token, ((void *)0), "unknown pragma");
                    }
                }
            }
        }
    }
    else
    {
        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "nullable") == 0)
        {
            p_pragma_token = pragma_match(p_pragma_token);
            if (p_pragma_token && strcmp(p_pragma_token->lexeme, "enable") == 0)
            {
                unsigned long long w = ((1ULL << 32) | (1ULL << 34));
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors =  ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes =  ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings =  ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings = w;
                ctx->options.null_checks_enabled = 1;
                ctx->options.flow_analysis = 1;
            }
            if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
            {
                unsigned long long w = ((1ULL << 32) | (1ULL << 34));
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors =  ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes =  ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings =  ~w;
                ctx->options.null_checks_enabled = 0;
            }
        }
        else
        {
            if (p_pragma_token && strcmp(p_pragma_token->lexeme, "ownership") == 0)
            {
                p_pragma_token = pragma_match(p_pragma_token);
                if (p_pragma_token && strcmp(p_pragma_token->lexeme, "enable") == 0)
                {
                    unsigned long long w = ((1ULL << 29));
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors =  ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes =  ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings =  ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings = w;
                    ctx->options.ownership_enabled = 1;
                    ctx->options.flow_analysis = 1;
                }
                if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
                {
                    unsigned long long w = ((1ULL << 29));
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors =  ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes =  ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings =  ~w;
                    ctx->options.ownership_enabled = 0;
                }
            }
            else
            {
                if (p_pragma_token && strcmp(p_pragma_token->lexeme, "flow") == 0)
                {
                    p_pragma_token = pragma_match(p_pragma_token);
                    if (p_pragma_token && strcmp(p_pragma_token->lexeme, "enable") == 0)
                    {
                        ctx->options.flow_analysis = 1;
                    }
                    if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
                    {
                        ctx->options.flow_analysis = 0;
                    }
                }
                else
                {
                    if (p_pragma_token && strcmp(p_pragma_token->lexeme, "safety") == 0)
                    {
                        p_pragma_token = pragma_match(p_pragma_token);
                        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "enable") == 0)
                        {
                            unsigned long long w = ((1ULL << 32) | (1ULL << 34)) | ((1ULL << 29));
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors =  ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes =  ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings =  ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings = w;
                            ctx->options.null_checks_enabled = 1;
                            ctx->options.flow_analysis = 1;
                            ctx->options.ownership_enabled = 1;
                        }
                        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
                        {
                            unsigned long long w = ((1ULL << 32) | (1ULL << 34)) | ((1ULL << 29));
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors =  ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes =  ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings =  ~w;
                            ctx->options.null_checks_enabled = 0;
                            ctx->options.ownership_enabled = 0;
                            ctx->options.flow_analysis = 0;
                        }
                    }
                }
            }
        }
    }
}

struct pragma_declaration *pragma_declaration(struct parser_ctx * ctx)
{
    struct pragma_declaration * p_pragma_declaration = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0) || ctx->current->type != 128)
        {
            ;
            goto _catch_label_1;
        }
        p_pragma_declaration = calloc(1, sizeof (struct pragma_declaration));
        if (p_pragma_declaration == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_pragma_declaration->first_token = ctx->current;
        parser_match(ctx);
        while (ctx->current && ctx->current->type != 129)
        {
            ctx->current = ctx->current->next;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_pragma_declaration->last_token = ctx->current;
        parser_match(ctx);
    }
    else _catch_label_1:
    {
    }
    if (p_pragma_declaration)
    {
        execute_pragma(ctx, p_pragma_declaration, 0);
    }
    return p_pragma_declaration;
}

unsigned char  object_to_bool(struct object * a);

struct static_assert_declaration *static_assert_declaration(struct parser_ctx * ctx)
{
    struct static_assert_declaration * p_static_assert_declaration = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_static_assert_declaration = calloc(1, sizeof (struct static_assert_declaration));
        if (p_static_assert_declaration == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_static_assert_declaration->first_token = ctx->current;
        struct token * position = ctx->current;
        parser_match(ctx);
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto _catch_label_1;
        }
        unsigned char   show_error_if_not_constant = 0;
        if (p_static_assert_declaration->first_token->type == 9055)
        {
            show_error_if_not_constant = 1;
        }
        struct expression * p_constant_expression = constant_expression(ctx, show_error_if_not_constant);
        if (p_constant_expression == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_static_assert_declaration->constant_expression = p_constant_expression;
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 44)
        {
            parser_match(ctx);
            p_static_assert_declaration->string_literal_opt = ctx->current;
            if (parser_match_tk(ctx, 130) != 0)
            {
                goto _catch_label_1;
            }
        }
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_static_assert_declaration->last_token = ctx->current;
        if (parser_match_tk(ctx, 59) != 0)
        {
            goto _catch_label_1;
        }
        if (position->type == 9055)
        {
            if ( !object_to_bool(&p_static_assert_declaration->constant_expression->object))
            {
                if (p_static_assert_declaration->string_literal_opt)
                {
                    compiler_diagnostic_message(1060, ctx, position, ((void *)0), "static_assert failed %s\n", p_static_assert_declaration->string_literal_opt->lexeme);
                }
                else
                {
                    compiler_diagnostic_message(1060, ctx, position, ((void *)0), "static_assert failed");
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
    return p_static_assert_declaration;
}

void attribute_specifier_sequence_add(struct attribute_specifier_sequence * list, struct attribute_specifier * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void attribute_specifier_delete(struct attribute_specifier * p);

void attribute_specifier_sequence_delete(struct attribute_specifier_sequence * p)
{
    if (p)
    {
        struct attribute_specifier * item = p->head;
        while (item)
        {
            struct attribute_specifier * next = item->next;
            item->next = ((void *)0);
            attribute_specifier_delete(item);
            item = next;
        }
        free(p);
    }
}

struct attribute_specifier *attribute_specifier(struct parser_ctx * ctx);

struct attribute_specifier_sequence *attribute_specifier_sequence_opt(struct parser_ctx * ctx)
{
    struct attribute_specifier_sequence * p_attribute_specifier_sequence = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (first_of_attribute_specifier(ctx))
        {
            p_attribute_specifier_sequence = calloc(1, sizeof (struct attribute_specifier_sequence));
            if (p_attribute_specifier_sequence == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_attribute_specifier_sequence->first_token = ctx->current;
            while (ctx->current != ((void *)0) && first_of_attribute_specifier(ctx))
            {
                struct attribute_specifier * p_attribute_specifier = attribute_specifier(ctx);
                if (p_attribute_specifier == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_attribute_specifier_sequence->attributes_flags = p_attribute_specifier->attribute_list->attributes_flags;
                attribute_specifier_sequence_add(p_attribute_specifier_sequence, p_attribute_specifier);
            }
            if (ctx->previous == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_attribute_specifier_sequence->last_token = ctx->previous;
        }
    }
    else _catch_label_1:
    {
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = ((void *)0);
    }
    return p_attribute_specifier_sequence;
}

struct attribute_specifier_sequence *attribute_specifier_sequence(struct parser_ctx * ctx)
{
    struct attribute_specifier_sequence * p_attribute_specifier_sequence = ((void *)0);
    if (1)
    {
        p_attribute_specifier_sequence = calloc(1, sizeof (struct attribute_specifier_sequence));
        if (p_attribute_specifier_sequence == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (first_of_attribute_specifier(ctx))
        {
            struct attribute_specifier * p_attribute_specifier = attribute_specifier(ctx);
            if (p_attribute_specifier == ((void *)0))
            {
                goto _catch_label_1;
            }
            attribute_specifier_sequence_add(p_attribute_specifier_sequence, p_attribute_specifier);
        }
    }
    else _catch_label_1:
    {
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = ((void *)0);
    }
    return p_attribute_specifier_sequence;
}

void attribute_list_delete(struct attribute_list * p);

void attribute_specifier_delete(struct attribute_specifier * p)
{
    if (p)
    {
        attribute_list_delete(p->attribute_list);
        ;
        free(p);
    }
}

struct attribute_list *attribute_list(struct parser_ctx * ctx);

struct attribute_specifier *attribute_specifier(struct parser_ctx * ctx)
{
    struct attribute_specifier * p_attribute_specifier = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_attribute_specifier = calloc(1, sizeof (struct attribute_specifier));
        if (p_attribute_specifier == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_attribute_specifier->first_token = ctx->current;
        if (parser_match_tk(ctx, 91) != 0)
        {
            goto _catch_label_1;
        }
        if (parser_match_tk(ctx, 91) != 0)
        {
            goto _catch_label_1;
        }
        struct attribute_list * p_attribute_list = attribute_list(ctx);
        if (p_attribute_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_attribute_specifier->attribute_list = p_attribute_list;
        if (parser_match_tk(ctx, 93) != 0)
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_attribute_specifier->last_token = ctx->current;
        if (parser_match_tk(ctx, 93) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        attribute_specifier_delete(p_attribute_specifier);
        p_attribute_specifier = ((void *)0);
    }
    return p_attribute_specifier;
}

void attribute_token_delete(struct attribute_token * p);
void attribute_argument_clause_delete(struct attribute_argument_clause * p);

void attribute_delete(struct attribute * p)
{
    if (p)
    {
        attribute_token_delete(p->attribute_token);
        attribute_argument_clause_delete(p->attribute_argument_clause);
        ;
        free(p);
    }
}

void attribute_list_add(struct attribute_list * list, struct attribute * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void attribute_list_destroy(struct attribute_list * p)
{
    struct attribute * item = p->head;
    while (item)
    {
        struct attribute * next = item->next;
        item->next = ((void *)0);
        attribute_delete(item);
        item = next;
    }
}

void attribute_list_delete(struct attribute_list * p)
{
    if (p)
    {
        attribute_list_destroy(p);
        free(p);
    }
}

unsigned char  first_of_attribute(struct parser_ctx * ctx);
struct attribute *attribute(struct parser_ctx * ctx);

struct attribute_list *attribute_list(struct parser_ctx * ctx)
{
    struct attribute_list * p_attribute_list = ((void *)0);
    if (1)
    {
        p_attribute_list = calloc(1, sizeof (struct attribute_list));
        if (p_attribute_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (first_of_attribute(ctx) || ctx->current->type == 44))
        {
            if (first_of_attribute(ctx))
            {
                struct attribute * p_attribute = attribute(ctx);
                if (p_attribute == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_attribute_list->attributes_flags = p_attribute->attributes_flags;
                attribute_list_add(p_attribute_list, p_attribute);
            }
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type == 44)
            {
                parser_match(ctx);
            }
        }
    }
    else _catch_label_1:
    {
        attribute_list_delete(p_attribute_list);
        p_attribute_list = ((void *)0);
    }
    return p_attribute_list;
}

unsigned char  first_of_attribute(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 8996;
}

struct attribute_token *attribute_token(struct parser_ctx * ctx);
struct attribute_argument_clause *attribute_argument_clause(struct parser_ctx * ctx);

struct attribute *attribute(struct parser_ctx * ctx)
{
    struct attribute * p_attribute = ((void *)0);
    if (1)
    {
        p_attribute = calloc(1, sizeof (struct attribute));
        if (p_attribute == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct attribute_token * p_attribute_token = attribute_token(ctx);
        if (p_attribute_token == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_attribute->attribute_token = p_attribute_token;
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_attribute->attributes_flags = p_attribute->attribute_token->attributes_flags;
        if (ctx->current->type == 40)
        {
            struct attribute_argument_clause * p_attribute_argument_clause = attribute_argument_clause(ctx);
            if (p_attribute_argument_clause == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_attribute->attribute_argument_clause = p_attribute_argument_clause;
        }
    }
    else _catch_label_1:
    {
        attribute_delete(p_attribute);
        p_attribute = ((void *)0);
    }
    return p_attribute;
}

void attribute_token_delete(struct attribute_token * p)
{
    if (p)
    {
        free(p);
    }
}

unsigned char  token_is_identifier_or_keyword(int   t);

struct attribute_token *attribute_token(struct parser_ctx * ctx)
{
    struct attribute_token * p_attribute_token = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_attribute_token = calloc(1, sizeof (struct attribute_token));
        if (p_attribute_token == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct token * attr_token = ctx->current;
        unsigned char   is_standard_attribute = 0;
        if (strcmp(attr_token->lexeme, "deprecated") == 0)
        {
            is_standard_attribute = 1;
            p_attribute_token->attributes_flags = 1;
        }
        else
        {
            if (strcmp(attr_token->lexeme, "fallthrough") == 0)
            {
                is_standard_attribute = 1;
            }
            else
            {
                if (strcmp(attr_token->lexeme, "maybe_unused") == 0)
                {
                    is_standard_attribute = 1;
                    p_attribute_token->attributes_flags = 4;
                }
                else
                {
                    if (strcmp(attr_token->lexeme, "noreturn") == 0)
                    {
                        is_standard_attribute = 1;
                        p_attribute_token->attributes_flags = 16;
                    }
                    else
                    {
                        if (strcmp(attr_token->lexeme, "reproducible") == 0)
                        {
                            is_standard_attribute = 1;
                            p_attribute_token->attributes_flags = 64;
                        }
                        else
                        {
                            if (strcmp(attr_token->lexeme, "unsequenced") == 0)
                            {
                                is_standard_attribute = 1;
                                p_attribute_token->attributes_flags = 32;
                            }
                            else
                            {
                                if (strcmp(attr_token->lexeme, "nodiscard") == 0)
                                {
                                    is_standard_attribute = 1;
                                    p_attribute_token->attributes_flags = 8;
                                }
                            }
                        }
                    }
                }
            }
        }
        unsigned char   is_cake_attr = strcmp(attr_token->lexeme, "cake") == 0;
        if (token_is_identifier_or_keyword(ctx->current->type))
        {
            parser_match(ctx);
        }
        else
        {
            compiler_diagnostic_message(970, ctx, attr_token, ((void *)0), "expected identifier");
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 14906)
        {
            parser_match(ctx);
            if (is_cake_attr)
            {
                compiler_diagnostic_message(8, ctx, attr_token, ((void *)0), "warning '%s' is not an cake attribute", ctx->current->lexeme);
            }
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (token_is_identifier_or_keyword(ctx->current->type))
            {
                parser_match(ctx);
            }
            else
            {
                compiler_diagnostic_message(970, ctx, attr_token, ((void *)0), "expected identifier");
            }
        }
        else
        {
            if ( !is_standard_attribute)
            {
                compiler_diagnostic_message(8, ctx, attr_token, ((void *)0), "warning '%s' is not an standard attribute", attr_token->lexeme);
            }
        }
    }
    else _catch_label_1:
    {
        attribute_token_delete(p_attribute_token);
        p_attribute_token = ((void *)0);
    }
    return p_attribute_token;
}

void balanced_token_sequence_delete(struct balanced_token_sequence * p);

void attribute_argument_clause_delete(struct attribute_argument_clause * p)
{
    if (p)
    {
        balanced_token_sequence_delete(p->p_balanced_token_sequence);
        free(p);
    }
}

struct balanced_token_sequence *balanced_token_sequence_opt(struct parser_ctx * ctx);

struct attribute_argument_clause *attribute_argument_clause(struct parser_ctx * ctx)
{
    struct attribute_argument_clause * p_attribute_argument_clause = calloc(1, sizeof (struct attribute_argument_clause));
    if (1)
    {
        if (p_attribute_argument_clause == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto _catch_label_1;
        }
        p_attribute_argument_clause->p_balanced_token_sequence = balanced_token_sequence_opt(ctx);
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        attribute_argument_clause_delete(p_attribute_argument_clause);
        p_attribute_argument_clause = ((void *)0);
    }
    return p_attribute_argument_clause;
}

void balanced_token_sequence_delete(struct balanced_token_sequence * p)
{
    if (p)
    {
        struct balanced_token * item = p->head;
        while (item)
        {
            struct balanced_token * next = item->next;
            free(item);
            item = next;
        }
        free(p);
    }
}

struct balanced_token_sequence *balanced_token_sequence_opt(struct parser_ctx * ctx)
{
    struct balanced_token_sequence * p_balanced_token_sequence = calloc(1, sizeof (struct balanced_token_sequence));
    if (1)
    {
        if (p_balanced_token_sequence == ((void *)0))
        {
            goto _catch_label_1;
        }
        int count1 = 0;
        int count2 = 0;
        int count3 = 0;
        for (; ctx->current; )
        {
            if (ctx->current->type == 40)
            {
                count1++;
            }
            else
            {
                if (ctx->current->type == 91)
                {
                    count2++;
                }
                else
                {
                    if (ctx->current->type == 123)
                    {
                        count3++;
                    }
                    else
                    {
                        if (ctx->current->type == 41)
                        {
                            if (count1 == 0)
                            {
                                break;
                            }
                            count1--;
                        }
                        else
                        {
                            if (ctx->current->type == 91)
                            {
                                count2--;
                            }
                            else
                            {
                                if (ctx->current->type == 123)
                                {
                                    count3--;
                                }
                            }
                        }
                    }
                }
            }
            parser_match(ctx);
        }
        if (count2 != 0)
        {
            compiler_diagnostic_message(1090, ctx, ctx->current, ((void *)0), "expected ']' before ')'");
        }
        if (count3 != 0)
        {
            compiler_diagnostic_message(1090, ctx, ctx->current, ((void *)0), "expected '}' before ')'");
        }
    }
    else _catch_label_1:
    {
        balanced_token_sequence_delete(p_balanced_token_sequence);
        p_balanced_token_sequence = ((void *)0);
    }
    return p_balanced_token_sequence;
}

void labeled_statement_delete(struct labeled_statement * p);
void unlabeled_statement_delete(struct unlabeled_statement * p);

void statement_delete(struct statement * p)
{
    if (p)
    {
        labeled_statement_delete(p->labeled_statement);
        unlabeled_statement_delete(p->unlabeled_statement);
        free(p);
    }
}

struct labeled_statement *labeled_statement(struct parser_ctx * ctx);
struct unlabeled_statement *unlabeled_statement(struct parser_ctx * ctx);

struct statement *statement(struct parser_ctx * ctx)
{
    struct statement * p_statement = calloc(1, sizeof (struct statement));
    if (1)
    {
        if (p_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (first_of_labeled_statement(ctx))
        {
            p_statement->labeled_statement = labeled_statement(ctx);
            if (p_statement->labeled_statement == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        else
        {
            p_statement->unlabeled_statement = unlabeled_statement(ctx);
            if (p_statement->unlabeled_statement == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
    }
    else _catch_label_1:
    {
        statement_delete(p_statement);
        p_statement = ((void *)0);
    }
    return p_statement;
}

struct compound_statement *compound_statement(struct parser_ctx * ctx);
struct selection_statement *selection_statement(struct parser_ctx * ctx);
struct iteration_statement *iteration_statement(struct parser_ctx * ctx);
struct defer_statement *defer_statement(struct parser_ctx * ctx);
struct try_statement *try_statement(struct parser_ctx * ctx);
void primary_block_delete(struct primary_block * p);

struct primary_block *primary_block(struct parser_ctx * ctx)
{
    ;
    struct primary_block * p_primary_block = calloc(1, sizeof (struct primary_block));
    if (1)
    {
        if (p_primary_block == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (first_of_compound_statement(ctx))
        {
            p_primary_block->compound_statement = compound_statement(ctx);
            if (p_primary_block->compound_statement == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        else
        {
            if (first_of_selection_statement(ctx))
            {
                p_primary_block->selection_statement = selection_statement(ctx);
                if (p_primary_block->selection_statement == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
            else
            {
                if (first_of_iteration_statement(ctx))
                {
                    p_primary_block->iteration_statement = iteration_statement(ctx);
                    if (p_primary_block->iteration_statement == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                }
                else
                {
                    if (ctx->current->type == 9009)
                    {
                        p_primary_block->defer_statement = defer_statement(ctx);
                        if (p_primary_block->defer_statement == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                    }
                    else
                    {
                        if (ctx->current->type == 9036)
                        {
                            p_primary_block->try_statement = try_statement(ctx);
                            if (p_primary_block->try_statement == ((void *)0))
                            {
                                goto _catch_label_1;
                            }
                        }
                        else
                        {
                            compiler_diagnostic_message(970, ctx, ctx->current, ((void *)0), "unexpected token");
                        }
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
        primary_block_delete(p_primary_block);
        p_primary_block = ((void *)0);
    }
    return p_primary_block;
}

struct secondary_block *secondary_block(struct parser_ctx * ctx)
{
    struct secondary_block * p_secondary_block = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        check_open_brace_style(ctx, ctx->current);
        p_secondary_block = calloc(1, sizeof (struct secondary_block));
        if (p_secondary_block == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_secondary_block->first_token = ctx->current;
        struct statement * p_statement = statement(ctx);
        if (p_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_secondary_block->statement = p_statement;
        if (ctx->previous == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_secondary_block->last_token = ctx->previous;
        check_close_brace_style(ctx, p_secondary_block->last_token);
    }
    else _catch_label_1:
    {
        secondary_block_delete(p_secondary_block);
        p_secondary_block = ((void *)0);
    }
    return p_secondary_block;
}

unsigned char  unlabeled_statement_ends_with_jump(struct unlabeled_statement * p_unlabeled_statement)
{
    struct expression * p_expression = ((void *)0);
    if (p_unlabeled_statement->expression_statement)
    {
        p_expression = p_unlabeled_statement->expression_statement->expression_opt;
    }
    else
    {
        if (p_unlabeled_statement->jump_statement)
        {
            return 1;
        }
        else
        {
            if (p_unlabeled_statement->primary_block && p_unlabeled_statement->primary_block->compound_statement && p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail && p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement && p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement->expression_statement)
            {
                p_expression = p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement->expression_statement->expression_opt;
            }
        }
    }
    if (p_expression)
    {
        return p_expression->type.attributes_flags & 16;
    }
    if (p_unlabeled_statement->primary_block && p_unlabeled_statement->primary_block->compound_statement && p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail && p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement)
    {
        return p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != ((void *)0);
    }
    return 0;
}

unsigned char  secondary_block_ends_with_jump(struct secondary_block * p_secondary_block)
{
    if (p_secondary_block && p_secondary_block->statement->unlabeled_statement)
    {
        return unlabeled_statement_ends_with_jump(p_secondary_block->statement->unlabeled_statement);
    }
    return 0;
}

void secondary_block_delete(struct secondary_block * p)
{
    if (p)
    {
        statement_delete(p->statement);
        free(p);
    }
}

void compound_statement_delete(struct compound_statement * p);
void iteration_statement_delete(struct iteration_statement * p);
void selection_statement_delete(struct selection_statement * p);
void try_statement_delete(struct try_statement * p);

void primary_block_delete(struct primary_block * p)
{
    if (p)
    {
        compound_statement_delete(p->compound_statement);
        defer_statement_delete(p->defer_statement);
        iteration_statement_delete(p->iteration_statement);
        selection_statement_delete(p->selection_statement);
        try_statement_delete(p->try_statement);
        free(p);
    }
}

static unsigned char  first_of_primary_block(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    if (first_of_compound_statement(ctx) || first_of_selection_statement(ctx) || first_of_iteration_statement(ctx) || ctx->current->type == 9009 || ctx->current->type == 9036)
    {
        return 1;
    }
    return 0;
}

void expression_statement_delete(struct expression_statement * p);
void jump_statement_delete(struct jump_statement * p);

void unlabeled_statement_delete(struct unlabeled_statement * p)
{
    if (p)
    {
        expression_statement_delete(p->expression_statement);
        jump_statement_delete(p->jump_statement);
        primary_block_delete(p->primary_block);
        free(p);
    }
}

struct jump_statement *jump_statement(struct parser_ctx * ctx);
struct expression_statement *expression_statement(struct parser_ctx * ctx, unsigned char   ignore_semicolon);
unsigned char  type_is_nodiscard(struct type * p_type);

struct unlabeled_statement *unlabeled_statement(struct parser_ctx * ctx)
{
    struct unlabeled_statement * p_unlabeled_statement = calloc(1, sizeof (struct unlabeled_statement));
    if (1)
    {
        if (p_unlabeled_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (first_of_primary_block(ctx))
        {
            p_unlabeled_statement->primary_block = primary_block(ctx);
            if (p_unlabeled_statement->primary_block == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        else
        {
            if (first_of_jump_statement(ctx))
            {
                p_unlabeled_statement->jump_statement = jump_statement(ctx);
                if (p_unlabeled_statement->jump_statement == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
            else
            {
                p_unlabeled_statement->expression_statement = expression_statement(ctx, 0);
                if (p_unlabeled_statement->expression_statement == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (p_unlabeled_statement->expression_statement->expression_opt)
                {
                    if ( !type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type) && type_is_nodiscard(&p_unlabeled_statement->expression_statement->expression_opt->type) && p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & 8192)
                    {
                        if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                        {
                            compiler_diagnostic_message(8, ctx, p_unlabeled_statement->expression_statement->expression_opt->first_token, ((void *)0), "ignoring return value of function declared with 'nodiscard' attribute");
                        }
                    }
                    if (type_is_owner(&p_unlabeled_statement->expression_statement->expression_opt->type) && p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & 8192)
                    {
                        if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                        {
                            compiler_diagnostic_message(8, ctx, p_unlabeled_statement->expression_statement->expression_opt->first_token, ((void *)0), "ignoring the result of _Owner type ");
                        }
                    }
                }
                if (p_unlabeled_statement->expression_statement->expression_opt && p_unlabeled_statement->expression_statement->expression_opt->expression_type == 12)
                {
                }
                else
                {
                    if (p_unlabeled_statement != ((void *)0) && p_unlabeled_statement->jump_statement == ((void *)0) && p_unlabeled_statement->expression_statement != ((void *)0) && p_unlabeled_statement->expression_statement->expression_opt &&  !type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type) && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 53 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 12 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 16 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 17 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 26 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 27 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 23 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 25)
                    {
                        if (ctx->current && ctx->current->level == 0)
                        {
                        }
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
        unlabeled_statement_delete(p_unlabeled_statement);
        p_unlabeled_statement = ((void *)0);
    }
    return p_unlabeled_statement;
}

void label_delete(struct label * p)
{
    if (p)
    {
        expression_delete(p->constant_expression);
        free(p);
    }
}

unsigned char  type_is_enum(struct type * p_type);
void check_diferent_enuns(struct parser_ctx * ctx, struct token * operator_token, struct expression * left, struct expression * right, char * message);

struct label *label(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return ((void *)0);
    }
    struct label * p_label = calloc(1, sizeof (struct label));
    if (1)
    {
        if (p_label == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_label->p_first_token = ctx->current;
        if (ctx->current->type == 8996)
        {
            p_label->p_identifier_opt = ctx->current;
            parser_match(ctx);
            if (parser_match_tk(ctx, 58) != 0)
            {
                goto _catch_label_1;
            }
        }
        else
        {
            if (ctx->current->type == 9001)
            {
                if (ctx->p_current_selection_statement == ((void *)0) || ctx->p_current_selection_statement->condition == ((void *)0))
                {
                    goto _catch_label_1;
                }
                parser_match(ctx);
                p_label->constant_expression = constant_expression(ctx, 1);
                if (p_label->constant_expression == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (parser_match_tk(ctx, 58) != 0)
                {
                    goto _catch_label_1;
                }
                long long case_value = object_to_signed_long_long(&p_label->constant_expression->object);
                if (ctx->p_switch_value_list == ((void *)0))
                {
                    goto _catch_label_1;
                }
                struct switch_value * p_switch_value = switch_value_list_find(ctx->p_switch_value_list, case_value);
                if (p_switch_value)
                {
                    compiler_diagnostic_message(44, ctx, p_label->constant_expression->first_token, ((void *)0), "duplicate case value '%lld'", case_value);
                    ;
                    compiler_diagnostic_message(62, ctx, p_switch_value->p_label->constant_expression->first_token, ((void *)0), "previous declaration");
                }
                struct switch_value * newvalue = calloc(1, sizeof  *newvalue);
                if (newvalue == ((void *)0))
                {
                    goto _catch_label_1;
                }
                newvalue->p_label = p_label;
                newvalue->value = case_value;
                switch_value_list_push(ctx->p_switch_value_list, newvalue);
                if (ctx->p_current_selection_statement && ctx->p_current_selection_statement->condition && ctx->p_current_selection_statement->condition->expression)
                {
                    if (type_is_enum(&ctx->p_current_selection_statement->condition->expression->type))
                    {
                        if (type_is_enum(&p_label->constant_expression->type))
                        {
                            check_diferent_enuns(ctx, p_label->constant_expression->first_token, p_label->constant_expression, ctx->p_current_selection_statement->condition->expression, "mismatch in enumeration types");
                        }
                        else
                        {
                        }
                    }
                    if (ctx->p_current_selection_statement == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    struct enum_specifier * p_enum_specifier = ((void *)0);
                    if (ctx->p_current_selection_statement->condition && ctx->p_current_selection_statement->condition->expression && ctx->p_current_selection_statement->condition->expression->type.enum_specifier)
                    {
                        p_enum_specifier = get_complete_enum_specifier(ctx->p_current_selection_statement->condition->expression->type.enum_specifier);
                    }
                    if (p_enum_specifier)
                    {
                        struct enumerator * p_enumerator = find_enumerator_by_value(p_enum_specifier, case_value);
                        if (p_enumerator == ((void *)0))
                        {
                            compiler_diagnostic_message(3, ctx, p_label->constant_expression->first_token, ((void *)0), "case value '%lld' not in enumerated type 'enum %s'", case_value, p_enum_specifier->tag_name);
                        }
                        else
                        {
                        }
                    }
                }
            }
            else
            {
                if (ctx->current->type == 9007)
                {
                    if (ctx->p_switch_value_list == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    if (ctx->p_switch_value_list->p_default)
                    {
                        goto _catch_label_1;
                    }
                    struct switch_value * p_default = calloc(1, sizeof  *p_default);
                    if (p_default == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    p_default->p_label = p_label;
                    ctx->p_switch_value_list->p_default = p_default;
                    parser_match(ctx);
                    if (parser_match_tk(ctx, 58) != 0)
                    {
                        goto _catch_label_1;
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
        label_delete(p_label);
        p_label = ((void *)0);
    }
    return p_label;
}

void labeled_statement_delete(struct labeled_statement * p)
{
    if (p)
    {
        label_delete(p->label);
        statement_delete(p->statement);
        free(p);
    }
}

struct labeled_statement *labeled_statement(struct parser_ctx * ctx)
{
    struct labeled_statement * p_labeled_statement = calloc(1, sizeof (struct labeled_statement));
    if (1)
    {
        if (p_labeled_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct label * p_label = label(ctx);
        if (p_label == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_labeled_statement->label = p_label;
        struct statement * p_statement = statement(ctx);
        if (p_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_labeled_statement->statement = p_statement;
    }
    else _catch_label_1:
    {
        labeled_statement_delete(p_labeled_statement);
        p_labeled_statement = ((void *)0);
    }
    return p_labeled_statement;
}

void block_item_list_destroy(struct block_item_list * p);

void compound_statement_delete(struct compound_statement * p)
{
    if (p)
    {
        block_item_list_destroy(&p->block_item_list);
        free(p);
    }
}

struct block_item_list block_item_list(struct parser_ctx * ctx, unsigned char  * error);

struct compound_statement *compound_statement(struct parser_ctx * ctx)
{
    struct scope  block_scope = {0, 0, 0, 0, 0, 10, 0, 0, 0};
    struct compound_statement * p_compound_statement = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_compound_statement = calloc(1, sizeof (struct compound_statement));
        if (p_compound_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_compound_statement->diagnostic_flags = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];
        scope_list_push(&ctx->scopes, &block_scope);
        p_compound_statement->first_token = ctx->current;
        if (parser_match_tk(ctx, 123) != 0)
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            compiler_diagnostic_message(970, ctx, ctx->input_list.tail, ((void *)0), "unexpected end of file");
            goto _catch_label_1;
        }
        if (ctx->current->type != 125)
        {
            unsigned char   berror = 0;
            p_compound_statement->block_item_list = block_item_list(ctx, &berror);
            if (berror)
            {
                goto _catch_label_1;
            }
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_compound_statement->last_token = ctx->current;
        if (parser_match_tk(ctx, 125) != 0)
        {
            goto _catch_label_1;
        }
        {
            int i = 0;
            for (; i < block_scope.variables.capacity; i++)
            {
                if (block_scope.variables.table == ((void *)0))
                {
                    continue;
                }
                struct map_entry * entry = block_scope.variables.table[i];
                while (entry)
                {
                    if (entry->type != 4 && entry->type != 5)
                    {
                        entry = entry->next;
                        continue;
                    }
                    struct declarator * p_declarator = ((void *)0);
                    struct init_declarator * p_init_declarator = ((void *)0);
                    if (entry->type == 5)
                    {
                        ;
                        p_init_declarator = entry->data.p_init_declarator;
                        p_declarator = p_init_declarator->p_declarator;
                    }
                    else
                    {
                        p_declarator = entry->data.p_declarator;
                    }
                    if (p_declarator)
                    {
                        if ( !type_is_maybe_unused(&p_declarator->type) && p_declarator->num_uses == 0)
                        {
                            if (p_declarator->name_opt && p_declarator->name_opt->token_origin->level == 0)
                            {
                                compiler_diagnostic_message(1, ctx, p_declarator->name_opt, ((void *)0), "'%s': unreferenced declarator", p_declarator->name_opt->lexeme);
                            }
                        }
                    }
                    entry = entry->next;
                }
            }
        }
    }
    else _catch_label_1:
    {
        compound_statement_delete(p_compound_statement);
        p_compound_statement = ((void *)0);
    }
    scope_list_pop(&ctx->scopes);
    scope_destroy(&block_scope);
    return p_compound_statement;
}

void block_item_list_add(struct block_item_list * list, struct block_item * p_item)
{
    if (list->head == ((void *)0))
    {
        list->head = p_item;
    }
    else
    {
        ;
        ;
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void block_item_delete(struct block_item * p);

void block_item_list_destroy(struct block_item_list * list)
{
    struct block_item * item = list->head;
    while (item)
    {
        struct block_item * next = item->next;
        item->next = ((void *)0);
        block_item_delete(item);
        item = next;
    }
}

struct block_item *block_item(struct parser_ctx * ctx);

struct block_item_list block_item_list(struct parser_ctx * ctx, unsigned char  * error)
{
     *error = 0;
    struct block_item_list  block_item_list = {0};
    struct block_item * p_block_item = ((void *)0);
    if (1)
    {
        p_block_item = block_item(ctx);
        if (p_block_item == ((void *)0))
        {
            goto _catch_label_1;
        }
        block_item_list_add(&block_item_list, p_block_item);
        p_block_item = ((void *)0);
        while (ctx->current != ((void *)0) && ctx->current->type != 125)
        {
            p_block_item = block_item(ctx);
            if (p_block_item == ((void *)0))
            {
                goto _catch_label_1;
            }
            block_item_list_add(&block_item_list, p_block_item);
            p_block_item = ((void *)0);
        }
    }
    else _catch_label_1:
    {
         *error = 1;
    }
    return block_item_list;
}

void block_item_delete(struct block_item * p)
{
    if (p)
    {
        declaration_delete(p->declaration);
        label_delete(p->label);
        unlabeled_statement_delete(p->unlabeled_statement);
        ;
        free(p);
    }
}

void naming_convention_local_var(struct parser_ctx * ctx, struct token * token, struct type * type);

struct block_item *block_item(struct parser_ctx * ctx)
{
    struct block_item * p_block_item = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_block_item = calloc(1, sizeof (struct block_item));
        if (p_block_item == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
            goto _catch_label_1;
        }
        p_block_item->first_token = ctx->current;
        if (ctx->current->type == 9046)
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type == 123)
            {
                parser_match(ctx);
                while (ctx->current && ctx->current->type != 125)
                {
                    parser_match(ctx);
                }
                parser_match(ctx);
            }
            else
            {
                while (ctx->current && ctx->current->type != 10)
                {
                    ctx->current = ctx->current->next;
                }
                parser_match(ctx);
            }
            if (ctx->current == ((void *)0))
            {
                attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type == 59)
            {
                parser_match(ctx);
            }
        }
        else
        {
            if (first_of_declaration_specifier(ctx) || first_of_static_assert_declaration(ctx) || first_of_pragma_declaration(ctx))
            {
                p_block_item->declaration = declaration(ctx, p_attribute_specifier_sequence_opt, 4096);
                if (p_block_item->declaration == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_attribute_specifier_sequence_opt = ((void *)0);
                struct init_declarator * p = p_block_item->declaration->init_declarator_list.head;
                while (p)
                {
                    if (p->p_declarator->name_opt)
                    {
                        naming_convention_local_var(ctx, p->p_declarator->name_opt, &p->p_declarator->type);
                    }
                    p = p->next;
                }
            }
            else
            {
                if (first_of_label(ctx))
                {
                    p_block_item->label = label(ctx);
                    if (p_block_item->label == ((void *)0))
                    {
                        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                        goto _catch_label_1;
                    }
                }
                else
                {
                    p_block_item->unlabeled_statement = unlabeled_statement(ctx);
                    if (p_block_item->unlabeled_statement == ((void *)0))
                    {
                        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                        goto _catch_label_1;
                    }
                }
            }
        }
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
    }
    else _catch_label_1:
    {
        block_item_delete(p_block_item);
        p_block_item = ((void *)0);
    }
    return p_block_item;
}

void try_statement_delete(struct try_statement * p)
{
    if (p)
    {
        secondary_block_delete(p->catch_secondary_block_opt);
        secondary_block_delete(p->secondary_block);
        free(p);
    }
}

struct try_statement *try_statement(struct parser_ctx * ctx)
{
    struct try_statement * p_try_statement = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_try_statement = calloc(1, sizeof (struct try_statement));
        if (p_try_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_try_statement->first_token = ctx->current;
        ;
        struct try_statement * try_statement_copy_opt = ctx->p_current_try_statement_opt;
        ctx->p_current_try_statement_opt = p_try_statement;
        ctx->try_catch_block_index++;
        p_try_statement->try_catch_block_index = ctx->try_catch_block_index;
        if (parser_match_tk(ctx, 9036) != 0)
        {
            goto _catch_label_1;
        }
        struct secondary_block * p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_try_statement->secondary_block = p_secondary_block;
        ctx->p_current_try_statement_opt = try_statement_copy_opt;
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 9006)
        {
            p_try_statement->catch_token_opt = ctx->current;
            parser_match(ctx);
            ;
            p_try_statement->catch_secondary_block_opt = secondary_block(ctx);
            if (p_try_statement->catch_secondary_block_opt == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        if (ctx->previous == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_try_statement->last_token = ctx->previous;
    }
    else _catch_label_1:
    {
        try_statement_delete(p_try_statement);
        p_try_statement = ((void *)0);
    }
    return p_try_statement;
}

void condition_delete(struct condition * p);
void init_statement_delete(struct init_statement * p);

void selection_statement_delete(struct selection_statement * p)
{
    if (p)
    {
        secondary_block_delete(p->secondary_block);
        secondary_block_delete(p->else_secondary_block_opt);
        condition_delete(p->condition);
        init_statement_delete(p->p_init_statement);
        free(p);
    }
}

unsigned char  style_has_one_space(struct token * token);
struct init_statement *init_statement(struct parser_ctx * ctx, unsigned char   ignore_semicolon);
struct condition *condition(struct parser_ctx * ctx);

struct selection_statement *selection_statement(struct parser_ctx * ctx)
{
    struct scope  if_scope = {0};
    scope_list_push(&ctx->scopes, &if_scope);
    struct selection_statement * p_selection_statement = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_selection_statement = calloc(1, sizeof (struct selection_statement));
        if (p_selection_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_selection_statement->first_token = ctx->current;
        unsigned char   is_if = (ctx->current->type == 9017);
        if (ctx->current->type == 9017 || ctx->current->type == 9034)
        {
            parser_match(ctx);
        }
        else
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if ( !(ctx->current->flags & 2) &&  !style_has_one_space(ctx->current))
        {
            compiler_diagnostic_message(10, ctx, ctx->current, ((void *)0), "one space");
        }
        p_selection_statement->open_parentesis_token = ctx->current;
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto _catch_label_1;
        }
        p_selection_statement->p_init_statement = init_statement(ctx, 1);
        if (p_selection_statement->p_init_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 59)
        {
            if (p_selection_statement->p_init_statement->p_simple_declaration)
            {
                p_selection_statement->p_init_statement->p_simple_declaration->last_token = ctx->current;
            }
            else
            {
                if (p_selection_statement->p_init_statement->p_expression_statement)
                {
                    p_selection_statement->p_init_statement->p_simple_declaration->last_token = ctx->current;
                }
            }
            parser_match(ctx);
            p_selection_statement->condition = condition(ctx);
        }
        else
        {
            if (ctx->current->type == 41)
            {
                p_selection_statement->condition = calloc(1, sizeof  *p_selection_statement->condition);
                if (p_selection_statement->condition == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (p_selection_statement->p_init_statement->p_expression_statement)
                {
                    p_selection_statement->condition->first_token = p_selection_statement->p_init_statement->p_expression_statement->expression_opt->first_token;
                    p_selection_statement->condition->last_token = p_selection_statement->p_init_statement->p_expression_statement->expression_opt->last_token;
                    p_selection_statement->condition->expression = p_selection_statement->p_init_statement->p_expression_statement->expression_opt;
                    p_selection_statement->p_init_statement->p_expression_statement->expression_opt = ((void *)0);
                }
                if (p_selection_statement->p_init_statement->p_simple_declaration)
                {
                    p_selection_statement->condition->first_token = p_selection_statement->p_init_statement->p_simple_declaration->first_token;
                    p_selection_statement->condition->last_token = p_selection_statement->p_init_statement->p_simple_declaration->last_token;
                    if (p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head != p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.tail)
                    {
                        ;
                        goto _catch_label_1;
                    }
                    p_selection_statement->condition->p_init_declarator = p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head;
                    p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head = ((void *)0);
                    p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.tail = ((void *)0);
                    p_selection_statement->condition->p_declaration_specifiers = p_selection_statement->p_init_statement->p_simple_declaration->p_declaration_specifiers;
                    p_selection_statement->p_init_statement->p_simple_declaration->p_declaration_specifiers = ((void *)0);
                }
                init_statement_delete(p_selection_statement->p_init_statement);
                p_selection_statement->p_init_statement = ((void *)0);
            }
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_selection_statement->close_parentesis_token = ctx->current;
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto _catch_label_1;
        }
        struct selection_statement * previous = ctx->p_current_selection_statement;
        ctx->p_current_selection_statement = p_selection_statement;
        struct switch_value_list * previous_switch_value_list = ctx->p_switch_value_list;
        struct switch_value_list  switch_value_list = {0};
        ctx->p_switch_value_list = &switch_value_list;
        struct secondary_block * p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == ((void *)0))
        {
            switch_value_destroy(&switch_value_list);
            goto _catch_label_1;
        }
        if (p_secondary_block->statement->unlabeled_statement && p_secondary_block->statement->unlabeled_statement->expression_statement && p_secondary_block->statement->unlabeled_statement->expression_statement->expression_opt == ((void *)0))
        {
            compiler_diagnostic_message(44, ctx, p_secondary_block->first_token, ((void *)0), "empty controlled statement found; is this the intent?");
        }
        ;
        p_selection_statement->secondary_block = p_secondary_block;
        if (p_selection_statement->first_token->type == 9034)
        {
            if (switch_value_list.p_default == ((void *)0))
            {
                struct enum_specifier * p_enum_specifier = ((void *)0);
                if (ctx->p_current_selection_statement && ctx->p_current_selection_statement->condition && ctx->p_current_selection_statement->condition->expression && ctx->p_current_selection_statement->condition->expression->type.enum_specifier)
                {
                    p_enum_specifier = get_complete_enum_specifier(ctx->p_current_selection_statement->condition->expression->type.enum_specifier);
                }
                if (p_enum_specifier)
                {
                    struct enumerator * p = p_enum_specifier->enumerator_list.head;
                    while (p)
                    {
                        struct switch_value * p_used = switch_value_list_find(&switch_value_list, object_to_signed_long_long(&p->value));
                        if (p_used == ((void *)0))
                        {
                            compiler_diagnostic_message(44, ctx, ctx->current, ((void *)0), "enumeration value '%s' not handled in switch", p->token->lexeme);
                        }
                        p = p->next;
                    }
                }
            }
        }
        ctx->p_current_selection_statement = previous;
        ctx->p_switch_value_list = previous_switch_value_list;
        switch_value_destroy(&switch_value_list);
        if (is_if && ctx->current && ctx->current->type == 9011)
        {
            p_selection_statement->else_token_opt = ctx->current;
            parser_match(ctx);
            ;
            struct secondary_block * p_secondary_block2 = secondary_block(ctx);
            if (p_secondary_block2 == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_selection_statement->else_secondary_block_opt = p_secondary_block2;
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        struct token * p_tk = previous_parser_token(ctx->current);
        if (p_tk == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_selection_statement->last_token = p_tk;
    }
    else _catch_label_1:
    {
        selection_statement_delete(p_selection_statement);
        p_selection_statement = ((void *)0);
    }
    scope_list_pop(&ctx->scopes);
    scope_destroy(&if_scope);
    return p_selection_statement;
}

struct defer_statement *defer_statement(struct parser_ctx * ctx)
{
    struct defer_statement * p_defer_statement = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_defer_statement = calloc(1, sizeof (struct defer_statement));
        if (p_defer_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current->type == 9009)
        {
            p_defer_statement->first_token = ctx->current;
            parser_match(ctx);
            struct secondary_block * p_secondary_block = secondary_block(ctx);
            if (p_secondary_block == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_defer_statement->secondary_block = p_secondary_block;
            if (ctx->previous == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_defer_statement->last_token = ctx->previous;
        }
    }
    else _catch_label_1:
    {
        defer_statement_delete(p_defer_statement);
        p_defer_statement = ((void *)0);
    }
    return p_defer_statement;
}

void iteration_statement_delete(struct iteration_statement * p)
{
    if (p)
    {
        expression_delete(p->expression0);
        expression_delete(p->expression1);
        expression_delete(p->expression2);
        declaration_delete(p->declaration);
        secondary_block_delete(p->secondary_block);
        free(p);
    }
}

struct iteration_statement *iteration_statement(struct parser_ctx * ctx)
{
    struct iteration_statement * p_iteration_statement = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_iteration_statement = calloc(1, sizeof (struct iteration_statement));
        if (p_iteration_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_iteration_statement->first_token = ctx->current;
        if (ctx->current->type == 9008)
        {
            parser_match(ctx);
            struct secondary_block * p_secondary_block = secondary_block(ctx);
            if (p_secondary_block == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_iteration_statement->secondary_block = p_secondary_block;
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            p_iteration_statement->second_token = ctx->current;
            if (parser_match_tk(ctx, 9042) != 0)
            {
                goto _catch_label_1;
            }
            if (parser_match_tk(ctx, 40) != 0)
            {
                goto _catch_label_1;
            }
            p_iteration_statement->expression1 = expression(ctx);
            if (parser_match_tk(ctx, 41) != 0)
            {
                goto _catch_label_1;
            }
            if (parser_match_tk(ctx, 59) != 0)
            {
                goto _catch_label_1;
            }
        }
        else
        {
            if (ctx->current->type == 9042)
            {
                parser_match(ctx);
                if (parser_match_tk(ctx, 40) != 0)
                {
                    goto _catch_label_1;
                }
                p_iteration_statement->expression1 = expression(ctx);
                if (parser_match_tk(ctx, 41) != 0)
                {
                    goto _catch_label_1;
                }
                struct secondary_block * p_secondary_block = secondary_block(ctx);
                if (p_secondary_block == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_iteration_statement->secondary_block = p_secondary_block;
            }
            else
            {
                if (ctx->current->type == 9015)
                {
                    parser_match(ctx);
                    if (parser_match_tk(ctx, 40) != 0)
                    {
                        goto _catch_label_1;
                    }
                    if (first_of_declaration_specifier(ctx))
                    {
                        struct scope  for_scope = {0};
                        scope_list_push(&ctx->scopes, &for_scope);
                        p_iteration_statement->declaration = declaration(ctx, ((void *)0), 4096);
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto _catch_label_1;
                        }
                        if (ctx->current->type != 59)
                        {
                            p_iteration_statement->expression1 = expression(ctx);
                            if (p_iteration_statement->expression1 == ((void *)0))
                            {
                                scope_list_pop(&ctx->scopes);
                                scope_destroy(&for_scope);
                                goto _catch_label_1;
                            }
                        }
                        if (parser_match_tk(ctx, 59) != 0)
                        {
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto _catch_label_1;
                        }
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto _catch_label_1;
                        }
                        if (ctx->current->type != 41)
                        {
                            p_iteration_statement->expression2 = expression(ctx);
                        }
                        if (parser_match_tk(ctx, 41) != 0)
                        {
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto _catch_label_1;
                        }
                        struct secondary_block * p_secondary_block = secondary_block(ctx);
                        if (p_secondary_block == ((void *)0))
                        {
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto _catch_label_1;
                        }
                        p_iteration_statement->secondary_block = p_secondary_block;
                        scope_list_pop(&ctx->scopes);
                        scope_destroy(&for_scope);
                    }
                    else
                    {
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            goto _catch_label_1;
                        }
                        if (ctx->current->type != 59)
                        {
                            p_iteration_statement->expression0 = expression(ctx);
                        }
                        if (parser_match_tk(ctx, 59) != 0)
                        {
                            goto _catch_label_1;
                        }
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            goto _catch_label_1;
                        }
                        if (ctx->current->type != 59)
                        {
                            p_iteration_statement->expression1 = expression(ctx);
                        }
                        if (parser_match_tk(ctx, 59) != 0)
                        {
                            goto _catch_label_1;
                        }
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            goto _catch_label_1;
                        }
                        if (ctx->current->type != 41)
                        {
                            p_iteration_statement->expression2 = expression(ctx);
                        }
                        if (parser_match_tk(ctx, 41) != 0)
                        {
                            goto _catch_label_1;
                        }
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            goto _catch_label_1;
                        }
                        struct secondary_block * p_secondary_block = secondary_block(ctx);
                        if (p_secondary_block == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                        p_iteration_statement->secondary_block = p_secondary_block;
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
        iteration_statement_delete(p_iteration_statement);
        p_iteration_statement = ((void *)0);
    }
    return p_iteration_statement;
}

void jump_statement_delete(struct jump_statement * p)
{
    if (p)
    {
        expression_delete(p->expression_opt);
        free(p);
    }
}

struct type get_function_return_type(struct type * p_type);

struct jump_statement *jump_statement(struct parser_ctx * ctx)
{
    struct jump_statement * p_jump_statement = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_jump_statement = calloc(1, sizeof (struct jump_statement));
        if (p_jump_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_jump_statement->first_token = ctx->current;
        if (ctx->current->type == 9016)
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            p_jump_statement->label = ctx->current;
            if (parser_match_tk(ctx, 8996) != 0)
            {
                goto _catch_label_1;
            }
        }
        else
        {
            if (ctx->current->type == 9005)
            {
                parser_match(ctx);
            }
            else
            {
                if (ctx->current->type == 9000)
                {
                    parser_match(ctx);
                }
                else
                {
                    if (ctx->current->type == 9037)
                    {
                        if (ctx->p_current_try_statement_opt == ((void *)0))
                        {
                            compiler_diagnostic_message(1110, ctx, ctx->current, ((void *)0), "throw statement not within try block");
                        }
                        else
                        {
                            p_jump_statement->try_catch_block_index = ctx->p_current_try_statement_opt->try_catch_block_index;
                        }
                        parser_match(ctx);
                    }
                    else
                    {
                        if (ctx->current->type == 9027)
                        {
                            struct token * p_return_token = ctx->current;
                            parser_match(ctx);
                            if (ctx->current == ((void *)0))
                            {
                                unexpected_end_of_file(ctx);
                                goto _catch_label_1;
                            }
                            if (ctx->current->type != 59)
                            {
                                p_jump_statement->expression_opt = expression(ctx);
                                if (p_jump_statement->expression_opt)
                                {
                                    struct type  return_type = get_function_return_type(&ctx->p_current_function_opt->init_declarator_list.head->p_declarator->type);
                                    if (type_is_void(&return_type))
                                    {
                                        compiler_diagnostic_message(1120, ctx, p_return_token, ((void *)0), "void function '%s' should not return a value", ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name_opt->lexeme);
                                    }
                                    else
                                    {
                                        check_assigment(ctx, &return_type, p_jump_statement->expression_opt, 0);
                                    }
                                    type_destroy(&return_type);
                                }
                            }
                        }
                        else
                        {
                            ;
                        }
                    }
                }
            }
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_jump_statement->last_token = ctx->current;
        if (parser_match_tk(ctx, 59) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        jump_statement_delete(p_jump_statement);
        p_jump_statement = ((void *)0);
    }
    return p_jump_statement;
}

void expression_statement_delete(struct expression_statement * p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        expression_delete(p->expression_opt);
        free(p);
    }
}

struct expression_statement *expression_statement(struct parser_ctx * ctx, unsigned char   ignore_semicolon)
{
    struct expression_statement * p_expression_statement = calloc(1, sizeof (struct expression_statement));
    if (1)
    {
        if (p_expression_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_expression_statement->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type != 59)
        {
            p_expression_statement->expression_opt = expression(ctx);
            if (p_expression_statement->expression_opt == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        if ( !ignore_semicolon && parser_match_tk(ctx, 59) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        expression_statement_delete(p_expression_statement);
        p_expression_statement = ((void *)0);
    }
    return p_expression_statement;
}

void declaration_list_add(struct declaration_list * list, struct declaration * p_declaration)
{
    if (list->head == ((void *)0))
    {
        list->head = p_declaration;
    }
    else
    {
        ;
        ;
        list->tail->next = p_declaration;
    }
    list->tail = p_declaration;
}

void simple_declaration_delete(struct simple_declaration * p_simple_declaration)
{
    if (p_simple_declaration)
    {
        declaration_specifiers_delete(p_simple_declaration->p_declaration_specifiers);
        init_declarator_list_destroy(&p_simple_declaration->init_declarator_list);
        attribute_specifier_sequence_delete(p_simple_declaration->p_attribute_specifier_sequence_opt);
        free(p_simple_declaration);
    }
}

void condition_delete(struct condition * p_condition)
{
    if (p_condition)
    {
        init_declarator_delete(p_condition->p_init_declarator);
        expression_delete(p_condition->expression);
        attribute_specifier_sequence_delete(p_condition->p_attribute_specifier_sequence_opt);
        declaration_specifiers_delete(p_condition->p_declaration_specifiers);
        free(p_condition);
    }
}

struct condition *condition(struct parser_ctx * ctx)
{
    struct condition * p_condition = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_condition = calloc(1, sizeof  *p_condition);
        if (p_condition == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_condition->first_token = ctx->current;
        if (first_of_declaration_specifier(ctx))
        {
            p_condition->p_attribute_specifier_sequence_opt = attribute_specifier_sequence(ctx);
            p_condition->p_declaration_specifiers = declaration_specifiers(ctx, 4096);
            if (p_condition->p_declaration_specifiers == ((void *)0))
            {
                goto _catch_label_1;
            }
            struct init_declarator * p_init_declarator = init_declarator(ctx, p_condition->p_declaration_specifiers);
            if (p_init_declarator == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_condition->p_init_declarator = p_init_declarator;
        }
        else
        {
            p_condition->expression = expression(ctx);
            if (p_condition->expression == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        struct token * previous = previous_parser_token(ctx->current);
        if (previous)
        {
            p_condition->last_token = previous;
        }
    }
    else _catch_label_1:
    {
        condition_delete(p_condition);
        p_condition = ((void *)0);
    }
    return p_condition;
}

void init_statement_delete(struct init_statement * p_init_statement)
{
    if (p_init_statement)
    {
        expression_statement_delete(p_init_statement->p_expression_statement);
        simple_declaration_delete(p_init_statement->p_simple_declaration);
        free(p_init_statement);
    }
}

struct init_statement *init_statement(struct parser_ctx * ctx, unsigned char   ignore_semicolon)
{
    struct init_statement * p_init_statement = ((void *)0);
    if (1)
    {
        p_init_statement = calloc(1, sizeof  *p_init_statement);
        if (p_init_statement == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (first_of_declaration_specifier(ctx))
        {
            p_init_statement->p_simple_declaration = simple_declaration(ctx, ((void *)0), ignore_semicolon);
        }
        else
        {
            p_init_statement->p_expression_statement = expression_statement(ctx, ignore_semicolon);
        }
    }
    else _catch_label_1:
    {
        init_statement_delete(p_init_statement);
        p_init_statement = ((void *)0);
    }
    return p_init_statement;
}

void declaration_delete(struct declaration * p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        static_assert_declaration_delete(p->static_assert_declaration);
        declaration_specifiers_delete(p->declaration_specifiers);
        compound_statement_delete(p->function_body);
        pragma_declaration_delete(p->pragma_declaration);
        init_declarator_list_destroy(&p->init_declarator_list);
        ;
        free(p);
    }
}

void declaration_list_destroy(struct declaration_list * list)
{
    struct declaration * p = list->head;
    while (p)
    {
        struct declaration * next = p->next;
        p->next = ((void *)0);
        declaration_delete(p);
        p = next;
    }
}

struct declaration *external_declaration(struct parser_ctx * ctx);

struct declaration_list translation_unit(struct parser_ctx * ctx, unsigned char  * berror)
{
     *berror = 0;
    struct declaration_list  declaration_list = {0};
    if (1)
    {
        while (ctx->current != ((void *)0))
        {
            struct declaration * p = external_declaration(ctx);
            if (p == ((void *)0))
            {
                goto _catch_label_1;
            }
            declaration_list_add(&declaration_list, p);
        }
    }
    else _catch_label_1:
    {
         *berror = 1;
    }
    return declaration_list;
}

struct declaration *external_declaration(struct parser_ctx * ctx)
{
    return function_definition_or_declaration(ctx);
}

struct compound_statement *function_body(struct parser_ctx * ctx)
{
    ctx->try_catch_block_index = 0;
    ctx->p_current_try_statement_opt = ((void *)0);
    return compound_statement(ctx);
}

static void show_unused_file_scope(struct parser_ctx * ctx)
{
    if (ctx->scopes.head == ((void *)0))
    {
        return;
    }
    {
        int i = 0;
        for (; i < ctx->scopes.head->variables.capacity; i++)
        {
            if (ctx->scopes.head->variables.table == ((void *)0))
            {
                continue;
            }
            struct map_entry * entry = ctx->scopes.head->variables.table[i];
            while (entry)
            {
                if (entry->type != 4 && entry->type != 5)
                {
                    entry = entry->next;
                    continue;
                }
                struct declarator * p_declarator = ((void *)0);
                struct init_declarator * p_init_declarator = ((void *)0);
                if (entry->type == 5)
                {
                    ;
                    p_init_declarator = entry->data.p_init_declarator;
                    p_declarator = p_init_declarator->p_declarator;
                }
                else
                {
                    p_declarator = entry->data.p_declarator;
                }
                if (p_declarator && p_declarator->first_token_opt && p_declarator->first_token_opt->level == 0 && declarator_is_function(p_declarator) && p_declarator->declaration_specifiers && (p_declarator->declaration_specifiers->storage_class_specifier_flags & 4))
                {
                    if ( !type_is_maybe_unused(&p_declarator->type) && p_declarator->num_uses == 0)
                    {
                        if (p_declarator->name_opt)
                        {
                            compiler_diagnostic_message(1, ctx, p_declarator->name_opt, ((void *)0), "declarator '%s' not used", p_declarator->name_opt->lexeme);
                        }
                    }
                }
                entry = entry->next;
            }
        }
    }
}

struct declaration_list parse(struct parser_ctx * ctx, struct token_list * list, unsigned char  * berror)
{
     *berror = 0;
    s_anonymous_struct_count = 0;
    struct declaration_list  l = {0};
    struct scope  file_scope = {0};
    if (1)
    {
        scope_list_push(&ctx->scopes, &file_scope);
        ctx->input_list =  *list;
        ctx->current = ctx->input_list.head;
        parser_skip_blanks(ctx);
        unsigned char   local_error = 0;
        l = translation_unit(ctx, &local_error);
        if (local_error)
        {
            goto _catch_label_1;
        }
        show_unused_file_scope(ctx);
    }
    else _catch_label_1:
    {
         *berror = 1;
    }
    scope_destroy(&file_scope);
    return l;
}

struct include_dir *include_dir_add(struct include_dir_list * list, char * path);
struct token_list tokenizer(struct tokenizer_ctx * ctx, char * text, char * filename_opt, int level, int   addflags);
struct token_list preprocessor(struct preprocessor_ctx * ctx, struct token_list * input_list, int level);
void token_list_destroy(struct token_list * list);

int fill_preprocessor_options(int argc, char ** argv, struct preprocessor_ctx * prectx)
{
    {
        int i = 1;
        for (; i < argc; i++)
        {
            if (argv[i][0] != 45)
            {
                continue;
            }
            if (argv[i][1] == 73)
            {
                include_dir_add(&prectx->include_dir, argv[i] + 2);
                continue;
            }
            if (argv[i][1] == 68)
            {
                char buffer[200] = {0};
                snprintf(buffer, sizeof buffer, "#define %s \n", argv[i] + 2);
                char * p = &buffer[7];
                while ( *p)
                {
                    if ( *p == 61)
                    {
                         *p = 32;
                        break;
                    }
                    p++;
                }
                struct tokenizer_ctx  tctx = {0};
                struct token_list  l1 = tokenizer(&tctx, buffer, "", 0, 0);
                struct token_list  r = preprocessor(prectx, &l1, 0);
                token_list_destroy(&l1);
                token_list_destroy(&r);
                continue;
            }
        }
    }
    return 0;
}

unsigned long GetEnvironmentVariableA(char * lpName, char * lpBuffer, unsigned long nSize);
char *strcat(char * _Destination, char * _Source);

void append_msvc_include_dir(struct preprocessor_ctx * prectx)
{
    char env[2000] = {0};
    int n = GetEnvironmentVariableA("INCLUDE", env, sizeof (env));
    if (n > 0)
    {
        char * p = env;
        for (; ; )
        {
            if ( *p == 0)
            {
                break;
            }
            char filename_local[500] = {0};
            int count = 0;
            while ( *p != 0 && ( *p != 59 &&  *p != 10))
            {
                filename_local[count] =  *p;
                p++;
                count++;
            }
            filename_local[count] = 0;
            if (count > 0)
            {
                strcat(filename_local, "/");
                include_dir_add(&prectx->include_dir, filename_local);
            }
            if ( *p == 0)
            {
                break;
            }
            p++;
        }
    }
}

void c_visit(struct ast * ast)
{
}

struct _iobuf *fopen(char * _FileName, char * _Mode);
int *_errno(void);

int generate_config_file(char * configpath)
{
    struct _iobuf * outfile = ((void *)0);
    int error = 0;
    if (1)
    {
        outfile = fopen(configpath, "w");
        if (outfile == ((void *)0))
        {
            printf("Cannot open the file '%s' for writing.\n", configpath);
            error = ( *_errno());
            goto _catch_label_1;
        }
        fprintf(outfile, "//This was generated by running cake -autoconfig \n");
        char env[2000] = {0};
        int n = GetEnvironmentVariableA("INCLUDE", env, sizeof (env));
        if (n <= 0)
        {
            printf("INCLUDE not found.\nPlease, run cake -autoconfig inside visual studio command prompty.\n");
            error = 1;
            goto _catch_label_1;
        }
        fprintf(outfile, "//This file was generated reading the variable INCLUDE inside Visual Studio Command Prompt.\n");
        fprintf(outfile, "//echo %%INCLUDE%% \n");
        char * p = env;
        for (; ; )
        {
            if ( *p == 0)
            {
                break;
            }
            char filename_local[500] = {0};
            int count = 0;
            while ( *p != 0 && ( *p != 59 &&  *p != 10))
            {
                filename_local[count] =  *p;
                p++;
                count++;
            }
            filename_local[count] = 0;
            if (count > 0)
            {
                strcat(filename_local, "/");
                char * pch = filename_local;
                while ( *pch)
                {
                    if ( *pch == 92)
                    {
                         *pch = 47;
                    }
                    pch++;
                }
                fprintf(outfile, "#pragma dir \"%s\"\n", filename_local);
            }
            if ( *p == 0)
            {
                break;
            }
            p++;
        }
    }
    else _catch_label_1:
    {
    }
    if (outfile)
    {
        fclose(outfile);
    }
    if (error == 0)
    {
        printf("file '%s'\n", configpath);
        printf("successfully generated\n");
    }
    return error;
}

void add_standard_macros(struct preprocessor_ctx * ctx);
int include_config_header(struct preprocessor_ctx * ctx, char * file_name);
char *read_file(char * path, unsigned char   append_newline);
int _mkdir(char * _Path);
char *basename(char * filename);
void print_tokens(struct token * p_token);
char *print_preprocessed_to_string2(struct token * p_token);
void d_visit(struct d_visit_ctx * ctx, struct osstream * oss);
char *get_posix_error_message(int error);
void ast_destroy(struct ast * ast);
void preprocessor_ctx_destroy(struct preprocessor_ctx * p);

int compile_one_file(char * file_name, struct options * options, char * out_file_name, int argc, char ** argv, struct report * report)
{
    printf("%s\n", file_name);
    struct preprocessor_ctx  prectx = {0};
    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx);
    if (include_config_header(&prectx, file_name) != 0)
    {
    }
    struct ast  ast = {0};
    char * s = ((void *)0);
    struct parser_ctx  ctx = {0};
    struct tokenizer_ctx  tctx = {0};
    struct token_list  tokens = {0};
    ctx.options =  *options;
    ctx.p_report = report;
    char * content = ((void *)0);
    if (1)
    {
        if (fill_preprocessor_options(argc, argv, &prectx) != 0)
        {
            goto _catch_label_1;
        }
        prectx.options =  *options;
        append_msvc_include_dir(&prectx);
        content = read_file(file_name, 1);
        if (content == ((void *)0))
        {
            report->error_count++;
            printf("file not found '%s'\n", file_name);
            goto _catch_label_1;
        }
        if (options->sarif_output)
        {
            char sarif_file_name[260] = {0};
            if (options->sarifpath[0] != 0)
            {
                _mkdir(options->sarifpath);
                snprintf(sarif_file_name, sizeof sarif_file_name, "%s/%s.cake.sarif", options->sarifpath, basename(file_name));
            }
            else
            {
                snprintf(sarif_file_name, sizeof sarif_file_name, "%s.cake.sarif", file_name);
            }
            ctx.sarif_file = (struct _iobuf *)fopen(sarif_file_name, "w");
            if (ctx.sarif_file)
            {
                char * begin_sarif = "{\n";
                fprintf(ctx.sarif_file, "%s", begin_sarif);
            }
            else
            {
                report->error_count++;
                printf("cannot open Sarif output file '%s'\n", sarif_file_name);
                goto _catch_label_1;
            }
        }
        tokens = tokenizer(&tctx, content, file_name, 0, 0);
        if (tctx.n_errors > 0)
        {
            goto _catch_label_1;
        }
        if (options->dump_tokens)
        {
            print_tokens(tokens.head);
        }
        ast.token_list = preprocessor(&prectx, &tokens, 0);
        report->warnings_count = prectx.n_warnings;
        report->error_count = prectx.n_errors;
        if (prectx.n_errors > 0)
        {
            goto _catch_label_1;
        }
        if (options->dump_pptokens)
        {
            if (ast.token_list.head != ((void *)0))
            {
                print_tokens(ast.token_list.head);
            }
        }
        if (options->preprocess_only)
        {
            char * s2 = print_preprocessed_to_string2(ast.token_list.head);
            printf("%s", s2);
            free((void *)s2);
        }
        else
        {
            unsigned char   berror = 0;
            ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
            if (berror || report->error_count > 0)
            {
                goto _catch_label_1;
            }
            if ( !options->no_output)
            {
                if (options->target == 0)
                {
                    struct osstream  ss = {0};
                    struct d_visit_ctx  ctx2 = {0};
                    ctx2.ast = ast;
                    d_visit(&ctx2, &ss);
                    s = ss.c_str;
                }
                struct _iobuf * outfile = fopen(out_file_name, "w");
                if (outfile)
                {
                    if (s)
                    {
                        fprintf(outfile, "%s", s);
                    }
                    fclose(outfile);
                }
                else
                {
                    report->error_count++;
                    printf("cannot open output file '%s' - %s\n", out_file_name, get_posix_error_message(( *_errno())));
                    goto _catch_label_1;
                }
            }
        }
        if (ctx.sarif_file)
        {
            fprintf(ctx.sarif_file, "%s", "      ],\n");
            fclose(ctx.sarif_file);
            ctx.sarif_file = ((void *)0);
        }
    }
    else _catch_label_1:
    {
    }
    if (ctx.options.test_mode)
    {
        char buf[260] = {0};
        snprintf(buf, sizeof buf, "%s.txt", file_name);
        char * content_expected = read_file(buf, 0);
        if (content_expected)
        {
            if (s && strcmp(content_expected, s) != 0)
            {
                printf("diferent");
                report->error_count++;
            }
            free(content_expected);
        }
        if (report->fatal_error_expected != 0)
        {
            if (report->last_diagnostics_ids[0] == report->fatal_error_expected)
            {
                report->error_count--;
            }
        }
        if (report->error_count > 0 || report->warnings_count > 0)
        {
            printf("-------------------------------------------\n");
            printf("%s", content);
            printf("\n-------------------------------------------\n");
            printf("\x1b[91m", report->error_count, report->warnings_count);
            printf("\n\n");
            report->test_failed++;
        }
        else
        {
            report->test_succeeded++;
            printf("\x1b[92m");
        }
    }
    token_list_destroy(&tokens);
    parser_ctx_destroy(&ctx);
    free((void *)s);
    free(content);
    ast_destroy(&ast);
    preprocessor_ctx_destroy(&prectx);
    return report->error_count > 0;
}

char *strrchr(char * _String, int _Ch);
char *dirname(char * path);
struct TAGDIR *opendir(char * name);
struct dirent *readdir(struct TAGDIR * dirp);
char *strcpy(char * _Destination, char * _Source);
int closedir(struct TAGDIR * dirp);

int compile_many_files(char * file_name, struct options * options, char * out_file_name, int argc, char ** argv, struct report * report)
{
    char * file_name_name = basename(file_name);
    char * file_name_extension = strrchr((char *)file_name_name, 46);
    if (file_name_extension == ((void *)0))
    {
        ;
    }
    int num_files = 0;
    char path[260] = {0};
    snprintf(path, sizeof path, "%s", file_name);
    dirname(path);
    struct TAGDIR * dir = opendir(path);
    if (dir == ((void *)0))
    {
        return ( *_errno());
    }
    struct dirent * dp;
    while ((dp = readdir(dir)) != ((void *)0))
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }
        char fromlocal[257] = {0};
        snprintf(fromlocal, sizeof fromlocal, "%s/%s", "", dp->d_name);
        if (dp->d_type & 4)
        {
        }
        else
        {
            char * file_name_iter = basename(dp->d_name);
            char * file_extension = strrchr((char *)file_name_iter, 46);
            if (file_name_extension && file_extension && strcmp(file_name_extension, file_extension) == 0)
            {
                char out_file_name_final[260] = {0};
                strcpy(out_file_name_final, out_file_name);
                dirname(out_file_name_final);
                strcat(out_file_name_final, "/");
                strcat(out_file_name_final, file_name_iter);
                char in_file_name_final[260] = {0};
                strcpy(in_file_name_final, file_name);
                dirname(in_file_name_final);
                strcat(in_file_name_final, "/");
                strcat(in_file_name_final, file_name_iter);
                struct report  report_local = {0};
                report_local.test_mode = report->test_mode;
                compile_one_file(in_file_name_final, options, out_file_name_final, argc, argv, &report_local);
                report->fatal_error_expected = report_local.fatal_error_expected;
                report->error_count = report_local.error_count;
                report->warnings_count = report_local.warnings_count;
                report->info_count = report_local.info_count;
                report->test_succeeded = report_local.test_succeeded;
                report->test_failed = report_local.test_failed;
                num_files++;
            }
        }
    }
    closedir(dir);
    return num_files;
}

char *realpath(char * path, char * resolved_path);
char *strncpy(char * _Destination, char * _Source, unsigned int _Count);

static void longest_common_path(int argc, char ** argv, char root_dir[260])
{
    {
        int i = 1;
        for (; i < argc; i++)
        {
            if (argv[i][0] == 45)
            {
                continue;
            }
            char fullpath_i[260] = {0};
            realpath(argv[i], fullpath_i);
            strcpy(root_dir, fullpath_i);
            dirname(root_dir);
            {
                int k = 0;
                for (; k < 260; k++)
                {
                    char ch = fullpath_i[k];
                    {
                        int j = 2;
                        for (; j < argc; j++)
                        {
                            if (argv[j][0] == 45)
                            {
                                continue;
                            }
                            char fullpath_j[260] = {0};
                            realpath(argv[j], fullpath_j);
                            if (fullpath_j[k] != ch)
                            {
                                strncpy(root_dir, fullpath_j, k);
                                root_dir[k] = 0;
                                dirname(root_dir);
                                goto exit;
                            }
                        }
                    }
                    if (ch == 0)
                    {
                        break;
                    }
                }
            }
        }
    }
    exit:
    ;
}

unsigned int strlen(char * _Str);

static int create_multiple_paths(char * root, char * outdir)
{
    char * p = outdir + strlen(root) + 1;
    for (; ; )
    {
        if ( *p != 0 &&  *p != 47 &&  *p != 92)
        {
            p++;
            continue;
        }
        char temp[260] = {0};
        strncpy(temp, outdir, p - outdir);
        int er = _mkdir(temp);
        if (er != 0)
        {
            er = ( *_errno());
            if (er != 17)
            {
                printf("error creating output folder '%s' - %s\n", temp, get_posix_error_message(er));
                return er;
            }
        }
        if ( *p == 0)
        {
            break;
        }
        p++;
    }
    return 0;
}

int fill_options(struct options * options, int argc, char ** argv);
int get_self_path(char * buffer, int maxsize);
long clock(void);

int compile(int argc, char ** argv, struct report * report)
{
    struct options  options = {0};
    if (fill_options(&options, argc, argv) != 0)
    {
        return 1;
    }
    char executable_path[246] = {0};
    get_self_path(executable_path, sizeof (executable_path));
    dirname(executable_path);
    char cakeconfig_path[260] = {0};
    snprintf(cakeconfig_path, sizeof cakeconfig_path, "%s", executable_path);
    if (options.auto_config)
    {
        report->ignore_this_report = 1;
        return generate_config_file(cakeconfig_path);
    }
    report->test_mode = options.test_mode;
    long begin_clock = clock();
    int no_files = 0;
    char root_dir[260] = {0};
    if ( !options.no_output)
    {
        longest_common_path(argc, argv, root_dir);
    }
    int root_dir_len = strlen(root_dir);
    {
        int i = 1;
        for (; i < argc; i++)
        {
            if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-sarif-path") == 0)
            {
                i++;
                continue;
            }
            if (argv[i][0] == 45)
            {
                continue;
            }
            no_files++;
            char output_file[260] = {0};
            if ( !options.no_output)
            {
                if (no_files == 1 && options.output[0] != 0)
                {
                    strcat(output_file, options.output);
                }
                else
                {
                    char fullpath[260] = {0};
                    realpath(argv[i], fullpath);
                    strcpy(output_file, root_dir);
                    strcat(output_file, "/out");
                    strcat(output_file, fullpath + root_dir_len);
                    char outdir[260] = {0};
                    strcpy(outdir, output_file);
                    dirname(outdir);
                    if (create_multiple_paths(root_dir, outdir) != 0)
                    {
                        return 1;
                    }
                }
            }
            char fullpath[260] = {0};
            realpath(argv[i], fullpath);
            char * file_extension = basename(fullpath);
            if (file_extension[0] == 42)
            {
                no_files--;
                no_files = compile_many_files(fullpath, &options, output_file, argc, argv, report);
            }
            else
            {
                struct report  report_local = {0};
                compile_one_file(fullpath, &options, output_file, argc, argv, &report_local);
                report->fatal_error_expected = report_local.fatal_error_expected;
                report->error_count = report_local.error_count;
                report->warnings_count = report_local.warnings_count;
                report->info_count = report_local.info_count;
                report->test_succeeded = report_local.test_succeeded;
                report->test_failed = report_local.test_failed;
            }
        }
    }
    long end_clock = clock();
    double cpu_time_used = ((double)(end_clock - begin_clock)) / ((long)1000);
    report->no_files = no_files;
    report->cpu_time_used_sec = cpu_time_used;
    return 0;
}

struct ast get_ast(struct options * options, char * filename, char * source, struct report * report)
{
    struct ast  ast = {0};
    struct tokenizer_ctx  tctx = {0};
    struct token_list  list = tokenizer(&tctx, source, filename, 0, 0);
    struct preprocessor_ctx  prectx = {0};
    struct parser_ctx  ctx = {0};
    ctx.p_report = report;
    if (1)
    {
        prectx.options =  *options;
        prectx.macros.capacity = 5000;
        add_standard_macros(&prectx);
        ast.token_list = preprocessor(&prectx, &list, 0);
        if (prectx.n_errors != 0)
        {
            goto _catch_label_1;
        }
        ctx.options =  *options;
        unsigned char   berror = 0;
        ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
        if (berror)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
    }
    parser_ctx_destroy(&ctx);
    token_list_destroy(&list);
    preprocessor_ctx_destroy(&prectx);
    return ast;
}

int strtoargv(char * s, int n, char *argv[])
{
    int argvc = 0;
    char * p = s;
    while ( *p)
    {
        while ( *p == 32)
        p++;
        if ( *p == 0)
        {
            break;
        }
        argv[argvc] = p;
        argvc++;
        while ( *p != 32 &&  *p != 0)
        p++;
        if ( *p == 0)
        {
            break;
        }
         *p = 0;
        p++;
        if (argvc >= n)
        {
            break;
        }
    }
    return argvc;
}

char *compile_source(char * pszoptions, char * content, struct report * report)
{
    char *argv[100] = {0};
    char string[200] = {0};
    snprintf(string, sizeof string, "exepath %s", pszoptions);
    int argc = strtoargv(string, 10, argv);
    char * s = ((void *)0);
    struct preprocessor_ctx  prectx = {0};
    struct ast  ast = {0};
    struct options  options = {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (1)
    {
        if (fill_options(&options, argc, argv) != 0)
        {
            goto _catch_label_1;
        }
        prectx.options = options;
        add_standard_macros(&prectx);
        if (options.preprocess_only)
        {
            struct tokenizer_ctx  tctx = {0};
            struct token_list  tokens = tokenizer(&tctx, content, "c:/main.c", 0, 0);
            struct token_list  token_list = preprocessor(&prectx, &tokens, 0);
            if (prectx.n_errors == 0)
            {
                s = print_preprocessed_to_string2(token_list.head);
            }
            token_list_destroy(&tokens);
            token_list_destroy(&token_list);
        }
        else
        {
            ast = get_ast(&options, "c:/main.c", content, report);
            if (report->error_count > 0)
            {
                goto _catch_label_1;
            }
            if (options.target == 0)
            {
                struct osstream  ss = {0};
                struct d_visit_ctx  ctx2 = {0};
                ctx2.ast = ast;
                d_visit(&ctx2, &ss);
                s = ss.c_str;
            }
        }
    }
    else _catch_label_1:
    {
    }
    preprocessor_ctx_destroy(&prectx);
    ast_destroy(&ast);
    return s;
}

char *CompileText(char * pszoptions, char * content)
{
    printf("\x1b[97m");
    printf("\x1b[97m", pszoptions);
    struct report  report = {0};
    return (char *)compile_source(pszoptions, content, &report);
}

void ast_destroy(struct ast * ast)
{
    token_list_destroy(&ast->token_list);
    declaration_list_destroy(&ast->declaration_list);
}

int toupper(int _C);

static unsigned char  is_all_upper(char * text)
{
    char * p = text;
    while ( *p)
    {
        if ( *p != toupper( *p))
        {
            return 0;
        }
        p++;
    }
    return 1;
}

static unsigned char  is_snake_case(char * text)
{
    if ( !( *text >= 97 &&  *text <= 122))
    {
        return 0;
    }
    while ( *text)
    {
        if (( *text >= 97 &&  *text <= 122) ||  *text == 95 || ( *text >= 48 &&  *text <= 57))
        {
        }
        else
        {
            return 0;
        }
        text++;
    }
    return 1;
}

static unsigned char  is_camel_case(char * text)
{
    if ( !( *text >= 97 &&  *text <= 122))
    {
        return 0;
    }
    while ( *text)
    {
        if (( *text >= 97 &&  *text <= 122) || ( *text >= 65 &&  *text <= 90) || ( *text >= 48 &&  *text <= 57))
        {
        }
        else
        {
            return 0;
        }
        text++;
    }
    return 1;
}

static unsigned char  is_pascal_case(char * text)
{
    if ( !(text[0] >= 65 && text[0] <= 90))
    {
        return 0;
    }
    while ( *text)
    {
        if (( *text >= 97 &&  *text <= 122) || ( *text >= 65 &&  *text <= 90) || ( *text >= 48 &&  *text <= 57))
        {
        }
        else
        {
            return 0;
        }
        text++;
    }
    return 1;
}

void naming_convention_struct_tag(struct parser_ctx * ctx, struct token * token)
{
    if ( !parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if ( !is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(10, ctx, token, ((void *)0), "use snake_case for struct/union tags");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if ( !is_pascal_case(token->lexeme))
            {
                compiler_diagnostic_message(10, ctx, token, ((void *)0), "use camelCase for struct/union tags");
            }
        }
    }
}

void naming_convention_enum_tag(struct parser_ctx * ctx, struct token * token)
{
    if ( !parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if ( !is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(10, ctx, token, ((void *)0), "use snake_case for enum tags");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if ( !is_pascal_case(token->lexeme))
            {
                compiler_diagnostic_message(10, ctx, token, ((void *)0), "use PascalCase for enum tags");
            }
        }
    }
}

void naming_convention_function(struct parser_ctx * ctx, struct token * token)
{
    if ( !parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if ( !is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(10, ctx, token, ((void *)0), "use snake_case for functions");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if ( !is_pascal_case(token->lexeme))
            {
                compiler_diagnostic_message(10, ctx, token, ((void *)0), "use PascalCase for functions");
            }
        }
    }
}

unsigned char  type_is_function_or_function_pointer(struct type * p_type);

void naming_convention_global_var(struct parser_ctx * ctx, struct token * token, struct type * type, int   storage)
{
    if ( !parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if ( !type_is_function_or_function_pointer(type))
    {
        if (storage & 4)
        {
            if (type_is_const(type))
            {
            }
            else
            {
                if (token->lexeme[0] != 115 || token->lexeme[1] != 95)
                {
                    compiler_diagnostic_message(10, ctx, token, ((void *)0), "use prefix s_ for static global variables");
                }
            }
        }
        if ( !is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(10, ctx, token, ((void *)0), "use snake_case global variables");
        }
    }
}

void naming_convention_local_var(struct parser_ctx * ctx, struct token * token, struct type * type)
{
    if ( !parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if ( !is_snake_case(token->lexeme))
        {
            compiler_diagnostic_message(10, ctx, token, ((void *)0), "use snake_case for local variables");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if ( !is_camel_case(token->lexeme))
            {
                compiler_diagnostic_message(10, ctx, token, ((void *)0), "use camelCase for local variables");
            }
        }
    }
}

void naming_convention_enumerator(struct parser_ctx * ctx, struct token * token)
{
    if ( !parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if ( !is_all_upper(token->lexeme))
    {
        compiler_diagnostic_message(10, ctx, token, ((void *)0), "use UPPERCASE for enumerators");
    }
}

void naming_convention_struct_member(struct parser_ctx * ctx, struct token * token, struct type * type)
{
    if ( !parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if ( !is_snake_case(token->lexeme))
    {
        compiler_diagnostic_message(10, ctx, token, ((void *)0), "use snake_case for struct members");
    }
}

void naming_convention_parameter(struct parser_ctx * ctx, struct token * token, struct type * type)
{
    if ( !parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if ( !is_snake_case(token->lexeme))
    {
        compiler_diagnostic_message(10, ctx, token, ((void *)0), "use snake_case for arguments");
    }
}

static void designation_to_string(struct parser_ctx * ctx, struct designation * designation, char buffer[], int sz)
{
    buffer[0] = 0;
    if (1)
    {
        struct designator * designator = designation->designator_list->head;
        while (designator)
        {
            if (designator->constant_expression_opt)
            {
                if ( !object_has_constant_value(&designator->constant_expression_opt->object))
                {
                    goto _catch_label_1;
                }
                unsigned long long index = object_to_unsigned_long_long(&designator->constant_expression_opt->object);
                snprintf(buffer, sz, "%s[%llu]", buffer, index);
            }
            else
            {
                if (designator->token)
                {
                    snprintf(buffer, sz, "%s.%s", buffer, designator->token->lexeme);
                }
            }
            designator = designator->next;
        }
    }
    else _catch_label_1:
    {
    }
}

struct object *object_get_referenced(struct object * p_object);
unsigned char  type_is_union(struct type * p_type);

static struct object *find_first_subobject_old(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out, unsigned char  * sub_object_of_union)
{
    p_object = (struct object *)object_get_referenced(p_object);
    if (p_object->members == ((void *)0))
    {
         *sub_object_of_union = 0;
         *p_type_out = type_dup(&p_object->type2);
        return p_object;
    }
     *sub_object_of_union = type_is_union(&p_object->type2);
     *p_type_out = type_dup(&p_object->members->type2);
    return p_object->members;
}

static struct object *find_first_subobject(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out, unsigned char  * sub_object_of_union)
{
    return find_first_subobject_old(p_type_not_used, p_object, p_type_out, sub_object_of_union);
}

static struct object *find_last_suboject_of_suboject_old(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out)
{
    p_object = (struct object *)object_get_referenced(p_object);
    if (p_object->members == ((void *)0))
    {
         *p_type_out = type_dup(&p_object->type2);
        return p_object;
    }
    struct object * it = p_object->members;
    while (it)
    {
        if (it->next == ((void *)0))
        {
            return find_last_suboject_of_suboject_old(p_type_not_used, it, p_type_out);
        }
        it = it->next;
    }
     *p_type_out = type_dup(&p_object->type2);
    return p_object;
}

static struct object *find_last_suboject_of_suboject(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out)
{
    return find_last_suboject_of_suboject_old(p_type_not_used, p_object, p_type_out);
}

void type_clear(struct type * a);

static struct object *find_next_subobject_old(struct type * p_top_object_not_used, struct object * current_object, struct object * it, struct type * p_type_out, unsigned char  * sub_object_of_union)
{
    type_clear(p_type_out);
    if (it == ((void *)0))
    {
        return ((void *)0);
    }
    if (it->members)
    {
         *sub_object_of_union = type_is_union(&it->type2);
        it = it->members;
         *p_type_out = type_dup(&it->type2);
        return it;
    }
    for (; ; )
    {
        if (it == ((void *)0))
        {
            break;
        }
        struct object * next = it->next;
        if (next != ((void *)0))
        {
            if (it->parent)
            {
                 *sub_object_of_union = type_is_union(&it->parent->type2);
            }
            it = next;
            break;
        }
        it = it->parent;
    }
    if (it != ((void *)0))
    {
         *p_type_out = type_dup(&it->type2);
    }
    return it;
}

static struct object *find_next_subobject(struct type * p_top_object_not_used, struct object * current_object, struct object * it, struct type * p_type_out, unsigned char  * sub_object_of_union)
{
    return find_next_subobject_old(p_top_object_not_used, current_object, it, p_type_out, sub_object_of_union);
}

unsigned char  type_is_scalar(struct type * p_type);
struct type get_array_item_type(struct type * p_type);

static unsigned char  find_next_subobject_core(struct type * p_type, struct object * obj, struct object * subobj, struct find_object_result * result)
{
    if (1)
    {
        if (type_is_scalar(p_type))
        {
            if (result->object != ((void *)0))
            {
                result->object = obj;
                result->type = type_dup(p_type);
                return 1;
            }
        }
        if (subobj == obj)
        {
            result->object = obj;
            return 0;
        }
        if (type_is_array(p_type))
        {
            struct type  item_type = get_array_item_type(p_type);
            struct object * it = obj->members;
            for (; it; it = it->next)
            {
                if (find_next_subobject_core(&item_type, it, subobj, result))
                {
                    return 1;
                }
            }
            type_destroy(&item_type);
            return 0;
        }
        if (p_type->struct_or_union_specifier)
        {
            struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
            if (p_struct_or_union_specifier == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (subobj == obj)
            {
                result->object = obj;
            }
            struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            struct object * member_object = obj->members;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (find_next_subobject_core(&p_member_declarator->declarator->type, member_object, subobj, result))
                            {
                                return 1;
                            }
                            member_object = member_object->next;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list != ((void *)0))
                    {
                        if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                        {
                            struct type  t = {0};
                            t.category = 0;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = 32768;
                            if (find_next_subobject_core(&t, member_object, subobj, result))
                            {
                                return 1;
                            }
                            type_destroy(&t);
                        }
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
        }
    }
    else _catch_label_1:
    {
    }
    return 0;
}

static struct object *next_sub_object2(struct type * p_type, struct object * obj, struct object * subobj, struct type * p_type_out)
{
    type_clear(p_type_out);
    struct find_object_result  find_object_result = {0};
    if (find_next_subobject_core(p_type, obj, subobj, &find_object_result))
    {
        type_swap(&find_object_result.type, p_type_out);
        return find_object_result.object;
    }
    return ((void *)0);
}

unsigned char  type_is_struct_or_union(struct type * p_type);
struct object *object_extend_array_to_index(struct type * p_type, struct object * a, int n, unsigned char   is_constant);
struct object *object_get_member(struct object * p_object, int index);

static struct object *find_designated_subobject(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * current_object, struct designator * p_designator, unsigned char   is_constant, struct type * p_type_out)
{
    if (1)
    {
        if (type_is_struct_or_union(p_current_object_type))
        {
            struct struct_or_union_specifier * p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_current_object_type->struct_or_union_specifier);
            if (p_struct_or_union_specifier == ((void *)0))
            {
                goto _catch_label_1;
            }
            struct member_declaration * p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            struct member_declarator * p_member_declarator = ((void *)0);
            char * name = p_designator->token->lexeme;
            struct object * p_member_object = current_object->members;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_member_declarator->declarator->name_opt && strcmp(p_member_declarator->declarator->name_opt->lexeme, name) == 0)
                            {
                                if (p_designator->next != ((void *)0))
                                {
                                    return find_designated_subobject(ctx, &p_member_declarator->declarator->type, p_member_object, p_designator->next, is_constant, p_type_out);
                                }
                                else
                                {
                                     *p_type_out = type_dup(&p_member_declarator->declarator->type);
                                    return p_member_object;
                                }
                            }
                        }
                        p_member_object = p_member_object->next;
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list && p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            compiler_diagnostic_message(720, ctx, p_designator->token, ((void *)0), "member '%s' not found in '%s'", name, p_struct_or_union_specifier->tag_name);
            return ((void *)0);
        }
        else
        {
            if (type_is_array(p_current_object_type))
            {
                unsigned char   compute_array_size = p_current_object_type->array_num_elements_expression == ((void *)0);
                long long index =  -1;
                int max_index =  -1;
                struct type  array_item_type = get_array_item_type(p_current_object_type);
                struct object * member_obj = current_object->members;
                if (p_designator->constant_expression_opt)
                {
                    index = object_to_signed_long_long(&p_designator->constant_expression_opt->object);
                    if (index > max_index)
                    {
                        max_index = index;
                        if (compute_array_size)
                        {
                            member_obj = object_extend_array_to_index(&array_item_type, current_object, max_index, is_constant);
                        }
                    }
                    member_obj = object_get_member(current_object, index);
                    if (member_obj == ((void *)0))
                    {
                        if (index < 0)
                        {
                            compiler_diagnostic_message(720, ctx, p_designator->constant_expression_opt->first_token, ((void *)0), "array designator value '%d' is negative", index);
                        }
                        else
                        {
                            if (index > p_current_object_type->num_of_elements)
                            {
                                compiler_diagnostic_message(720, ctx, p_designator->constant_expression_opt->first_token, ((void *)0), "array index '%d' in initializer exceeds array bounds", index);
                            }
                        }
                        type_destroy(&array_item_type);
                        return ((void *)0);
                    }
                    if (p_designator->next != ((void *)0))
                    {
                        struct object * p = find_designated_subobject(ctx, &array_item_type, member_obj, p_designator->next, is_constant, p_type_out);
                        type_destroy(&array_item_type);
                        return p;
                    }
                    else
                    {
                        type_swap(p_type_out, &array_item_type);
                        type_destroy(&array_item_type);
                    }
                    return member_obj;
                }
                else
                {
                }
            }
        }
    }
    else _catch_label_1:
    {
    }
    return ((void *)0);
}

static struct initializer_list_item *find_innner_initializer_list_item(struct braced_initializer * braced_initializer)
{
    struct initializer_list_item * p_initializer_list_item = braced_initializer->initializer_list->head;
    while (p_initializer_list_item->initializer->braced_initializer)
    {
        p_initializer_list_item = p_initializer_list_item->initializer->braced_initializer->initializer_list->head;
        if (p_initializer_list_item->next == ((void *)0))
        {
            return p_initializer_list_item;
        }
        p_initializer_list_item = p_initializer_list_item->next;
    }
    return p_initializer_list_item;
}

void object_default_initialization(struct object * p_object, unsigned char   is_constant);
void object_set(struct object * to, struct expression * init_expression, struct object * from, unsigned char   is_constant);
unsigned char  type_is_char(struct type * p_type);
unsigned char  type_is_array_of_char(struct type * p_type);

static int braced_initializer_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * current_object, struct braced_initializer * braced_initializer, unsigned char   is_constant)
{
    if (braced_initializer->initializer_list == ((void *)0))
    {
        object_default_initialization(current_object, is_constant);
        return 0;
    }
    if ( !type_is_union(p_current_object_type))
    {
        object_default_initialization(current_object, is_constant);
    }
    if (type_is_scalar(p_current_object_type) && braced_initializer != ((void *)0))
    {
        struct initializer_list_item * p_initializer_list_item = find_innner_initializer_list_item(braced_initializer);
        if (p_initializer_list_item == ((void *)0))
        {
            return 0;
        }
        if (p_initializer_list_item->initializer->assignment_expression != ((void *)0))
        {
            object_set(current_object, p_initializer_list_item->initializer->assignment_expression, &p_initializer_list_item->initializer->assignment_expression->object, is_constant);
        }
        p_initializer_list_item = p_initializer_list_item->next;
        if (p_initializer_list_item != ((void *)0))
        {
            compiler_diagnostic_message(100, ctx, p_initializer_list_item->initializer->first_token, ((void *)0), "warning: excess elements in initializer");
        }
        return 0;
    }
    struct object * parent_copy = current_object->parent;
    current_object->parent = ((void *)0);
    struct initializer_list_item * p_initializer_list_item = braced_initializer->initializer_list->head;
    long long array_to_expand_index =  -1;
    int array_to_expand_max_index =  -1;
    unsigned char   compute_array_size = 0;
    struct type  array_item_type = {0};
    if (type_is_array(p_current_object_type))
    {
        array_item_type = get_array_item_type(p_current_object_type);
        compute_array_size = p_current_object_type->array_num_elements_expression == ((void *)0);
        if (type_is_char(&array_item_type))
        {
            struct initializer_list_item * p_initializer_list_item2 = find_innner_initializer_list_item(braced_initializer);
            if (p_initializer_list_item2 == ((void *)0))
            {
                return 0;
            }
            if (p_initializer_list_item2->initializer->assignment_expression != ((void *)0))
            {
                if (p_initializer_list_item2->initializer->assignment_expression->expression_type == 3)
                {
                    unsigned long long num_of_elements = p_initializer_list_item2->initializer->assignment_expression->type.num_of_elements;
                    if (compute_array_size)
                    {
                        object_extend_array_to_index(&array_item_type, current_object, num_of_elements - 1, is_constant);
                    }
                    object_set(current_object, p_initializer_list_item2->initializer->assignment_expression, &p_initializer_list_item2->initializer->assignment_expression->object, is_constant);
                    p_current_object_type->num_of_elements = num_of_elements;
                    return 0;
                }
            }
        }
    }
    struct object * p_subobject = ((void *)0);
    for (; ; )
    {
        unsigned char   is_subobject_of_union = 0;
        struct type  subobject_type = {0};
        if (p_initializer_list_item == ((void *)0))
        {
            break;
        }
        if (p_initializer_list_item->designation)
        {
            if (compute_array_size)
            {
                array_to_expand_index = object_to_signed_long_long(&p_initializer_list_item->designation->designator_list->head->constant_expression_opt->object);
                if (array_to_expand_index > array_to_expand_max_index)
                {
                    array_to_expand_max_index = array_to_expand_index;
                }
                object_extend_array_to_index(&array_item_type, current_object, array_to_expand_max_index, is_constant);
            }
            is_subobject_of_union = type_is_union(&subobject_type);
            p_subobject = find_designated_subobject(ctx, p_current_object_type, current_object, p_initializer_list_item->designation->designator_list->head, is_constant, &subobject_type);
            if (p_subobject == ((void *)0))
            {
                p_initializer_list_item = p_initializer_list_item->next;
                break;
            }
        }
        else
        {
            if (compute_array_size)
            {
                struct object * po = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                if (po == ((void *)0))
                {
                    array_to_expand_index++;
                    if (array_to_expand_index > array_to_expand_max_index)
                    {
                        array_to_expand_max_index = array_to_expand_index;
                    }
                    object_extend_array_to_index(&array_item_type, current_object, array_to_expand_max_index, is_constant);
                }
            }
            if (p_subobject == ((void *)0))
            {
                p_subobject = find_first_subobject(p_current_object_type, current_object, &subobject_type, &is_subobject_of_union);
            }
            else
            {
                p_subobject = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
            }
        }
        if (p_subobject == ((void *)0))
        {
            break;
        }
        if (p_initializer_list_item->initializer->braced_initializer)
        {
            braced_initializer_new(ctx, &subobject_type, p_subobject, p_initializer_list_item->initializer->braced_initializer, is_constant);
            struct type  t = {0};
            is_subobject_of_union = type_is_union(&subobject_type);
            p_subobject = find_last_suboject_of_suboject(&subobject_type, p_subobject, &t);
            type_swap(&t, &subobject_type);
            type_destroy(&t);
        }
        else
        {
            unsigned char   entire_object_initialized = 0;
            if (type_is_array_of_char(&subobject_type) && p_initializer_list_item->initializer->assignment_expression->expression_type == 3)
            {
                entire_object_initialized = 1;
            }
            else
            {
                if (type_is_array(&subobject_type))
                {
                    while (type_is_array(&subobject_type))
                    {
                        p_subobject = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                    }
                }
                else
                {
                    if (type_is_struct_or_union(&subobject_type))
                    {
                        if (type_is_struct_or_union(&p_initializer_list_item->initializer->assignment_expression->type))
                        {
                            entire_object_initialized = 1;
                        }
                        else
                        {
                            p_subobject = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                        }
                    }
                }
            }
            object_set(p_subobject, p_initializer_list_item->initializer->assignment_expression, &p_initializer_list_item->initializer->assignment_expression->object, is_constant);
            if (is_subobject_of_union)
            {
                struct type  t = {0};
                is_subobject_of_union = 1;
                p_subobject = find_last_suboject_of_suboject(&p_subobject->parent->type2, p_subobject->parent, &t);
                type_swap(&t, &subobject_type);
                type_destroy(&t);
                if (p_subobject)
                {
                    subobject_type = type_dup(&p_subobject->type2);
                }
            }
            else
            {
                if (entire_object_initialized)
                {
                    struct type  t = {0};
                    is_subobject_of_union = type_is_union(p_current_object_type);
                    p_subobject = find_last_suboject_of_suboject(&subobject_type, p_subobject, &t);
                    type_swap(&t, &subobject_type);
                    type_destroy(&t);
                    if (p_subobject)
                    {
                        subobject_type = type_dup(&p_subobject->type2);
                    }
                }
            }
        }
        p_initializer_list_item = p_initializer_list_item->next;
        type_destroy(&subobject_type);
    }
    if (p_initializer_list_item != ((void *)0))
    {
        compiler_diagnostic_message(100, ctx, p_initializer_list_item->initializer->first_token, ((void *)0), "warning: excess elements in initializer");
    }
    if (compute_array_size)
    {
        current_object->type2.num_of_elements = array_to_expand_max_index + 1;
        p_current_object_type->num_of_elements = array_to_expand_max_index + 1;
    }
    current_object->parent = parent_copy;
    return 0;
}

int initializer_init_new(struct parser_ctx * ctx, struct type * p_type, struct object * object, struct initializer * initializer, unsigned char   is_constant)
{
    if (initializer->assignment_expression != ((void *)0))
    {
        object_set(object, initializer->assignment_expression, &initializer->assignment_expression->object, is_constant);
    }
    else
    {
        if (initializer->braced_initializer)
        {
            if (braced_initializer_new(ctx, p_type, object, initializer->braced_initializer, is_constant) != 0)
            {
                return 1;
            }
        }
    }
    return 0;
}

