struct param_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct param * head;
    struct param * tail;
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
    unsigned char   has_static_array_size;
    unsigned char   address_of;
    struct param_list  params;
    struct type * next;
};

union _struct_tag_16 {
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
    void * void_pointer;
};

struct object {
    int   state;
    int   value_type;
    struct type  type;
    char * debug_name;
    union _struct_tag_16  value;
    struct object * parent;
    struct expression * p_init_expression;
    struct object * members;
    struct object * next;
};

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
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
    struct init_declarator * p_init_declarator;
    int member_index;
    struct argument_expression_list  argument_expression_list;
    struct expression * condition_expr;
    struct expression * left;
    struct expression * right;
    unsigned char   is_assignment_expression;
};

struct type_qualifier {
    int   flags;
    struct token * token;
    struct type_qualifier * next;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
};

struct generic_selection {
    struct expression * expression;
    struct type_name * type_name;
    struct expression * p_view_selected_expression;
    struct generic_assoc_list  generic_assoc_list;
    struct token * first_token;
    struct token * last_token;
};

struct case_label_list {
    struct label * head;
    struct label * tail;
};

struct defer_list {
    struct defer_list_item * head;
    struct defer_list_item * tail;
};

struct selection_statement {
    struct init_statement * p_init_statement;
    struct condition * condition;
    struct secondary_block * secondary_block;
    struct secondary_block * else_secondary_block_opt;
    struct token * open_parentesis_token;
    struct token * close_parentesis_token;
    struct case_label_list  label_list;
    struct token * first_token;
    struct token * last_token;
    struct token * else_token_opt;
    struct defer_list  defer_list;
    int label_id;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct param {
    struct type  type;
    struct param * next;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct dirent {
    unsigned short d_ino;
    long d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
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

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
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

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

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
    unsigned char   preprocess_def_macro;
    unsigned char   clear_error_at_end;
    unsigned char   sarif_output;
    unsigned char   no_output;
    unsigned char   const_literal;
    unsigned char   visual_studio_ouput_format;
    unsigned char   dump_tokens;
    unsigned char   dump_pptokens;
    unsigned char   auto_config;
    unsigned char   do_static_debug;
    int static_debug_lines;
    char output[200];
    char sarifpath[200];
};

struct include_dir_list {
    struct include_dir * head;
    struct include_dir * tail;
};

struct token_list {
    struct token * head;
    struct token * tail;
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

struct labeled_statement {
    struct label * label;
    struct statement * statement;
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
    struct type  type;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
};

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct label_list_item {
    struct token * p_last_usage;
    struct token * p_defined;
    struct label_list_item * next;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct function_specifier {
    int   flags;
    struct token * token;
};

struct label_list {
    struct label_list_item * head;
    struct label_list_item * tail;
};

struct hash_item_set {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct label {
    struct expression * constant_expression;
    struct expression * constant_expression_end;
    struct token * p_identifier_opt;
    struct token * p_first_token;
    struct label * next;
    int label_id;
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

struct storage_class_specifier {
    int   flags;
    struct token * token;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct find_object_result {
    struct object * object;
    struct type  type;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
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

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
    struct defer_list  defer_list;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct _iobuf {
    void * _Placeholder;
};

struct tokenizer_ctx {
    struct options  options;
    int n_warnings;
    int n_errors;
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
    struct flow_object * p_flow_object;
    struct expression * p_expression_true;
    struct expression * p_expression_false;
    struct expression * p_alias_of_expression;
    struct type  type;
};

struct declaration_list {
    struct declaration * head;
    struct declaration * tail;
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

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
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

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
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

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct break_reference {
    struct selection_statement * p_selection_statement;
    struct iteration_statement * p_iteration_statement;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct parser_ctx {
    struct options  options;
    struct scope_list  scopes;
    struct scope * p_scope;
    struct declarator * p_current_function_opt;
    struct label_list  label_list;
    struct try_statement * p_current_try_statement_opt;
    struct defer_statement * p_current_defer_statement_opt;
    struct selection_statement * p_current_selection_statement;
    struct _iobuf * sarif_file;
    unsigned int sarif_entries;
    struct token_list  input_list;
    struct token * current;
    struct token * previous;
    unsigned char   evaluation_is_disabled;
    unsigned char   inside_generic_association;
    int label_id;
    struct report * p_report;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct label_state {
    char * label_name;
    int state_number;
};

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int   type;
    union _struct_tag_5  data;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
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

struct ast {
    struct token_list  token_list;
    struct declaration_list  declaration_list;
};

struct flow_objects {
    struct flow_object ** data;
    int size;
    int capacity;
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
    unsigned char   inside_contract;
    unsigned char   inside_loop;
    int throw_join_state;
    int break_join_state;
    int initial_state;
    struct flow_objects  arena;
    struct label_state labels[100];
    int labels_size;
};

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int   attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct include_dir {
    char * path;
    struct include_dir * next;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct defer_visit_ctx {
    struct secondary_block * catch_secondary_block_opt;
    struct parser_ctx * ctx;
    struct ast  ast;
    struct defer_defer_scope * tail_block;
    int parameter_list;
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

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
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

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct d_visit_ctx {
    int indentation;
    int locals_count;
    int extern_count;
    int tag_name_count;
    struct hash_map  tag_names;
    struct hash_map  structs_map;
    struct hash_map  function_map;
    struct hash_map  static_declarators;
    struct osstream  local_declarators;
    struct osstream  add_this_before;
    struct osstream  add_this_before_external_decl;
    unsigned char   is_local;
    struct osstream  data_types;
    struct osstream  function_types;
    unsigned char   zero_mem_used;
    unsigned char   memcpy_used;
    struct declarator * p_current_function_opt;
    struct break_reference  break_reference;
    unsigned char   is__func__predefined_identifier_added;
    struct ast  ast;
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

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct attribute_token {
    int   attributes_flags;
    struct token * token;
};

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int _Avx2WmemEnabledWeakValue = 0;
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
    return !!(((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors & w) != 0) || ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings & w) != 0) || ((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes & w) != 0));
}

unsigned char  compiler_diagnostic(int   w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);

static void check_open_brace_style(struct parser_ctx * ctx, struct token * token)
{
    if (token->level == 0 && !(token->flags & 2) && token->type == 123 && token->prev && parser_is_diagnostic_enabled(ctx, 10))
    {
        if (ctx->options.style == 0)
        {
            if (token->prev->type == 143 && token->prev->prev && token->prev->prev->type == 10)
            {
            }
            else
            {
                compiler_diagnostic(10, ctx, token, 0U, "not following correct brace style {");
            }
        }
    }
}

static void check_close_brace_style(struct parser_ctx * ctx, struct token * token)
{
    if (token->level == 0 && !(token->flags & 2) && token->type == 125 && token->prev && token->prev->prev && parser_is_diagnostic_enabled(ctx, 10))
    {
        if (ctx->options.style == 0)
        {
            if (token->prev->type == 143 && token->prev->prev->type == 10)
            {
            }
            else
            {
                compiler_diagnostic(10, ctx, token, 0U, "not following correct close brace style }");
            }
        }
    }
}

static void check_func_open_brace_style(struct parser_ctx * ctx, struct token * token)
{
    if (token->level == 0 && !(token->flags & 2) && token->type == 123 && token->prev && parser_is_diagnostic_enabled(ctx, 10))
    {
        if (ctx->options.style == 0)
        {
            if (token->prev->type == 10)
            {
            }
            else
            {
                compiler_diagnostic(10, ctx, token, 0U, "not following correct brace style {");
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
    if (list->head == 0U)
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
    struct scope * p;

    if (list->head == 0U)
    {
        return;
    }
    ;
    p = list->tail;
    if (list->head == list->tail)
    {
        list->head = 0U;
        list->tail = 0U;
    }
    else
    {
        list->tail = list->tail->previous;
        if (list->tail == list->head)
        {
            ;
            list->tail->next = 0U;
            list->tail->previous = 0U;
        }
    }
    p->next = 0U;
    p->previous = 0U;
}

void label_list_clear(struct label_list * list);
int fclose(struct _iobuf * _Stream);

void parser_ctx_destroy(struct parser_ctx * ctx)
{
    label_list_clear(&ctx->label_list);
    ;
    ;
    if (ctx->sarif_file)
    {
        fclose(ctx->sarif_file);
    }
}

static void stringfy(char * input, char * json_str_message, int output_size)
{
    int k;

    json_str_message[0] = 0;
    k = 0;
    while (*input != 0)
    {
        if (*input == 34)
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
            if (*input == 10)
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
                    json_str_message[k] = *input;
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

int __cdecl __stdio_common_vsprintf(unsigned __int64 _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return !!(&__ck__OptionsStorage1);
}
inline int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 2ULL, _Buffer, _BufferCount, _Format, 0U, _ArgList);
    return !!(_Result < 0 ? -1 : _Result);
}


int __cdecl __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
inline int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return !!(__stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList));
}
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
inline int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return !!(_Result);
}
void print_line_and_token(struct marker * p_marker, unsigned char   visual_studio_ouput_format);

inline int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l(_Stream, _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return !!(_Result);
}

unsigned char  compiler_diagnostic(int   w, struct parser_ctx * ctx, struct token * p_token_opt, struct marker * p_marker_temp, char * fmt, ...)
{
    unsigned char   included_file_location;
    struct marker  marker;
    unsigned char   is_error;
    unsigned char   is_warning;
    unsigned char   is_note;
    char * func_name;
    char buffer[200];
    char diagnostic_name[100];
    char * args;

    included_file_location = 0;
    _cake_zmem(&marker, 28);
    if (p_marker_temp == 0U)
    {
        if (p_token_opt == 0U)
        {
            return 0;
        }
        marker.file = p_token_opt->token_origin->lexeme;
        marker.line = p_token_opt->line;
        marker.start_col = p_token_opt->col;
        marker.end_col = p_token_opt->col;
        marker.p_token_caret = p_token_opt;
        included_file_location = !!(p_token_opt->level > 0);
    }
    else
    {
        marker = *p_marker_temp;
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
        if (p_token_opt == 0U)
        {
            return 0;
        }
        marker.file = p_token_opt->token_origin->lexeme;
        included_file_location = !!(p_token_opt->level > 0);
        marker.line = p_token_opt->line;
        marker.start_col = p_token_opt->col;
        marker.end_col = p_token_opt->col;
    }
    is_error = 0;
    is_warning = 0;
    is_note = 0;
    if (is_diagnostic_configurable(w))
    {
        is_error = !!((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors & (1ULL << w)) != 0);
        is_warning = !!((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings & (1ULL << w)) != 0);
        is_note = !!(((ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes & (1ULL << w)) != 0));
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
    func_name = "module";
    if (ctx->p_current_function_opt)
    {
        if (ctx->p_current_function_opt->name_opt)
        {
            func_name = ctx->p_current_function_opt->name_opt->lexeme;
        }
        else
        {
            func_name = "unnamed";
        }
    }
    _cake_zmem(&buffer, 200);
    _cake_zmem(&diagnostic_name, 100);
    get_warning_name(w, 100U, diagnostic_name);
    print_position(marker.file, marker.line, marker.start_col, ctx->options.visual_studio_ouput_format);
    _cake_zmem(&args, 4);
    ((void)(args = (char *)(&(fmt)) + 4U));
    vsnprintf(buffer, 200U, fmt, args);
    ((void)(args = 0U));
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
            printf("\x1b[91m""error: ""\x1b[97m""%s [""\x1b[91m""%s""\x1b[97m""]\n""\x1b[0m", buffer, diagnostic_name);
        }
        else
        {
            if (is_warning)
            {
                printf("\x1b[95m""warning: ""\x1b[97m""%s [""\x1b[95m""%s""\x1b[97m""]\n""\x1b[0m", buffer, diagnostic_name);
            }
            else
            {
                if (is_note)
                {
                    if (w == 62)
                    {
                        printf("\x1b[36;1m""note: ""\x1b[97m""%s\n""\x1b[0m", buffer);
                    }
                    else
                    {
                        printf("\x1b[36;1m""note: ""\x1b[97m""%s [""\x1b[36;1m""%s""\x1b[97m""]\n""\x1b[0m", buffer, diagnostic_name);
                    }
                }
            }
        }
    }
    print_line_and_token(&marker, ctx->options.visual_studio_ouput_format);
    if (ctx->sarif_file)
    {
        char json_str_message[200];

        _cake_zmem(&json_str_message, 200);
        stringfy(buffer, json_str_message, 200U);
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
    struct scope * p;
    int level;

    printf("--- begin of scope---\n");
    p = e->head;
    level = 0;
    while (p)
    {
        if (p->variables.table)
        {
            {
                int i;
                i = 0;
                for (; i < p->variables.capacity; i++)
                {
                    if (p->variables.table[i])
                    {
                        {
                            int k;
                            k = 0;
                            for (; k < level; k++)
                            printf(" ");
                        }
                        printf("%s\n", p->variables.table[i]->key);
                    }
                }
            }
            {
                int i;
                i = 0;
                for (; i < p->tags.capacity; i++)
                {
                    if (p->tags.table[i])
                    {
                        {
                            int k;
                            k = 0;
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
    struct token * token;

    token = ctx->current;
    if (token == 0U)
    {
        return 0;
    }
    return !!(token->type == 9018 || token->type == 9057);
}

unsigned char  first_of_enum_specifier_token(struct token * token)
{
    return !!(token->type == 9012);
}

unsigned char  first_of_enum_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return first_of_enum_specifier_token(ctx->current);
}

unsigned char  first_of_alignment_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 9043);
}

struct token *parser_look_ahead(struct parser_ctx * ctx);

unsigned char  first_of_atomic_type_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    if (ctx->current->type == 9045)
    {
        struct token * ahead;

        ahead = parser_look_ahead(ctx);
        if (ahead != 0U)
        {
            return !!(ahead->type == 40);
        }
    }
    return 0;
}

unsigned char  first_of_storage_class_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 9035 || ctx->current->type == 9002 || ctx->current->type == 9013 || ctx->current->type == 9032 || ctx->current->type == 9060 || ctx->current->type == 8999 || ctx->current->type == 9025);
}

unsigned char  first_of_struct_or_union_token(struct token * token)
{
    return !!(token->type == 9033 || token->type == 9038);
}

unsigned char  first_of_struct_or_union(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return first_of_struct_or_union_token(ctx->current);
}

unsigned char  first_of_type_qualifier_token(struct token * p_token)
{
    return !!(p_token->type == 9004 || p_token->type == 9026 || p_token->type == 9041 || p_token->type == 9045 || p_token->type == 9068 || p_token->type == 9067 || p_token->type == 9069 || p_token->type == 9070 || p_token->type == 9071);
}

unsigned char  first_of_type_qualifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return first_of_type_qualifier_token(ctx->current);
}

struct map_entry *hashmap_find(struct hash_map * map, char * key);

struct map_entry *find_tag(struct parser_ctx * ctx, char * lexeme)
{
    struct scope * scope;

    scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry * p_entry;

        p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry)
        {
            return p_entry;
        }
        scope = scope->previous;
    }
    return 0U;
}

struct map_entry *find_variables(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt)
{
    struct scope * scope;

    if (ppscope_opt != 0U)
    {
        *ppscope_opt = 0U;
    }
    scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry * p_entry;

        p_entry = hashmap_find(&scope->variables, lexeme);
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
    return 0U;
}

struct enum_specifier *find_enum_specifier(struct parser_ctx * ctx, char * lexeme)
{
    struct enum_specifier * best;
    struct scope * scope;

    best = 0U;
    scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry * p_entry;

        p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry && p_entry->type == 1)
        {
            ;
            best = p_entry->data.p_enum_specifier;
            if (best->enumerator_list.head != 0U)
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
    struct struct_or_union_specifier * p;
    struct scope * scope;

    p = 0U;
    scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry * p_entry;

        p_entry = hashmap_find(&scope->tags, lexeme);
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
    struct map_entry * p_entry;

    p_entry = find_variables(ctx, lexeme, ppscope_opt);
    if (p_entry)
    {
        if (p_entry->type == 5)
        {
            struct init_declarator * p_init_declarator;

            ;
            p_init_declarator = p_entry->data.p_init_declarator;
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
    return 0U;
}

struct enumerator *find_enumerator(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt)
{
    struct map_entry * p_entry;

    p_entry = find_variables(ctx, lexeme, ppscope_opt);
    if (p_entry && p_entry->type == 3)
    {
        return p_entry->data.p_enumerator;
    }
    return 0U;
}

unsigned char  first_of_typedef_name(struct parser_ctx * ctx, struct token * p_token)
{
    struct declarator * p_declarator;

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
    p_declarator = find_declarator(ctx, p_token->lexeme, 0U);
    if (p_declarator && p_declarator->declaration_specifiers && (p_declarator->declaration_specifiers->storage_class_specifier_flags & 1))
    {
        p_declarator->num_uses++;
        p_token->flags |= 16;
        return 1;
    }
    else
    {
        p_token->flags |= 32;
    }
    return 0;
}

unsigned char  first_of_type_specifier_token(struct parser_ctx * ctx, struct token * token);

unsigned char  first_of_type_name_ahead(struct parser_ctx * ctx)
{
    struct token * token_ahead;

    if (ctx->current == 0U)
    {
        return 0;
    }
    if (ctx->current->type != 40)
    {
        return 0;
    }
    token_ahead = parser_look_ahead(ctx);
    if (token_ahead == 0U)
    {
        return 0;
    }
    return !!(first_of_type_specifier_token(ctx, token_ahead) || first_of_type_qualifier_token(token_ahead));
}

unsigned char  first_of_type_specifier(struct parser_ctx * ctx);

unsigned char  first_of_type_name(struct parser_ctx * ctx)
{
    return !!(first_of_type_specifier(ctx) || first_of_type_qualifier(ctx));
}

unsigned char  first_of_type_specifier_token(struct parser_ctx * ctx, struct token * p_token)
{
    return !!(p_token->type == 9040 || p_token->type == 9003 || p_token->type == 9028 || p_token->type == 9019 || p_token->type == 9020 || p_token->type == 9021 || p_token->type == 9022 || p_token->type == 9023 || p_token->type == 9024 || p_token->type == 9014 || p_token->type == 9010 || p_token->type == 9029 || p_token->type == 9039 || p_token->type == 9066 || p_token->type == 9050 || p_token->type == 9051 || p_token->type == 9053 || p_token->type == 9054 || p_token->type == 9052 || p_token->type == 9061 || p_token->type == 9065 || first_of_atomic_type_specifier(ctx) || first_of_struct_or_union_token(p_token) || first_of_enum_specifier_token(p_token) || first_of_typedef_name(ctx, p_token));
}

unsigned char  first_of_type_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return first_of_type_specifier_token(ctx, ctx->current);
}

unsigned char  first_of_type_specifier_qualifier(struct parser_ctx * ctx)
{
    return !!(first_of_type_specifier(ctx) || first_of_type_qualifier(ctx) || first_of_alignment_specifier(ctx));
}

unsigned char  first_of_compound_statement(struct parser_ctx * ctx)
{
    return !!(ctx->current != 0U && ctx->current->type == 123);
}

unsigned char  first_of_jump_statement(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 9016 || ctx->current->type == 9005 || ctx->current->type == 9000 || ctx->current->type == 9027 || ctx->current->type == 9037);
}

unsigned char  first_of_selection_statement(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 9017 || ctx->current->type == 9034);
}

unsigned char  first_of_iteration_statement(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 9042 || ctx->current->type == 9008 || ctx->current->type == 9015);
}

unsigned char  first_of_label(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    if (ctx->current->type == 8996)
    {
        struct token * next;

        next = parser_look_ahead(ctx);
        return !!(next && next->type == 58);
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
    return !!(first_of_storage_class_specifier(ctx) || first_of_function_specifier(ctx) || first_of_type_specifier_qualifier(ctx));
}

unsigned char  first_of_pragma_declaration(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 128);
}

unsigned char  first_of_static_assert_declaration(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 9058 || ctx->current->type == 9072 || ctx->current->type == 9073 || ctx->current->type == 9074 || ctx->current->type == 9075);
}

unsigned char  first_of_attribute_specifier(struct parser_ctx * ctx)
{
    struct token * p_token;

    if (ctx->current == 0U)
    {
        return 0;
    }
    if (ctx->current->type != 91)
    {
        return 0;
    }
    p_token = parser_look_ahead(ctx);
    return !!(p_token != 0U && p_token->type == 91);
}

unsigned char  first_of_labeled_statement(struct parser_ctx * ctx)
{
    return first_of_label(ctx);
}

unsigned char  first_of_designator(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 91 || ctx->current->type == 46);
}

struct token *previous_parser_token(struct token * token)
{
    struct token * prev;

    if (token->prev == 0U)
    {
        return 0U;
    }
    prev = token->prev;
    while (prev && !(prev->flags & 1))
    {
        prev = prev->prev;
    }
    return prev;
}

int __cdecl strcmp(char * _Str1, char * _Str2);

int  is_keyword(char * text)
{
    /*switch*/
    {
        register char _R0 = text[0];
        if (_R0 == 97) goto _CKL1; /*case 97*/
        if (_R0 == 98) goto _CKL7; /*case 98*/
        if (_R0 == 99) goto _CKL10; /*case 99*/
        if (_R0 == 100) goto _CKL17; /*case 100*/
        if (_R0 == 101) goto _CKL22; /*case 101*/
        if (_R0 == 102) goto _CKL26; /*case 102*/
        if (_R0 == 103) goto _CKL30; /*case 103*/
        if (_R0 == 105) goto _CKL32; /*case 105*/
        if (_R0 == 110) goto _CKL36; /*case 110*/
        if (_R0 == 108) goto _CKL38; /*case 108*/
        if (_R0 == 114) goto _CKL40; /*case 114*/
        if (_R0 == 115) goto _CKL44; /*case 115*/
        if (_R0 == 116) goto _CKL56; /*case 116*/
        if (_R0 == 117) goto _CKL64; /*case 117*/
        if (_R0 == 118) goto _CKL67; /*case 118*/
        if (_R0 == 119) goto _CKL70; /*case 119*/
        if (_R0 == 95) goto _CKL72; /*case 95*/
        goto /*default*/ _CKL116;

        {
            /*case 97*/ _CKL1:
            if (strcmp("alignof", text) == 0)
            {
                return 9044;
            }
            if (strcmp("auto", text) == 0)
            {
                return 8999;
            }
            if (strcmp("alignas", text) == 0)
            {
                return 9043;
            }
            if (strcmp("alignof", text) == 0)
            {
                return 9043;
            }
            if (strcmp("assert", text) == 0)
            {
                return 9059;
            }
            /*break*/ goto _CKL0;

            /*case 98*/ _CKL7:
            if (strcmp("break", text) == 0)
            {
                return 9000;
            }
            if (strcmp("bool", text) == 0)
            {
                return 9050;
            }
            /*break*/ goto _CKL0;

            /*case 99*/ _CKL10:
            if (strcmp("case", text) == 0)
            {
                return 9001;
            }
            if (strcmp("char", text) == 0)
            {
                return 9003;
            }
            if (strcmp("const", text) == 0)
            {
                return 9004;
            }
            if (strcmp("constexpr", text) == 0)
            {
                return 9002;
            }
            if (strcmp("continue", text) == 0)
            {
                return 9005;
            }
            if (strcmp("catch", text) == 0)
            {
                return 9006;
            }
            /*break*/ goto _CKL0;

            /*case 100*/ _CKL17:
            if (strcmp("default", text) == 0)
            {
                return 9007;
            }
            if (strcmp("do", text) == 0)
            {
                return 9008;
            }
            if (strcmp("defer", text) == 0)
            {
                return 9009;
            }
            if (strcmp("double", text) == 0)
            {
                return 9010;
            }
            /*break*/ goto _CKL0;

            /*case 101*/ _CKL22:
            if (strcmp("else", text) == 0)
            {
                return 9011;
            }
            if (strcmp("enum", text) == 0)
            {
                return 9012;
            }
            if (strcmp("extern", text) == 0)
            {
                return 9013;
            }
            /*break*/ goto _CKL0;

            /*case 102*/ _CKL26:
            if (strcmp("float", text) == 0)
            {
                return 9014;
            }
            if (strcmp("for", text) == 0)
            {
                return 9015;
            }
            if (strcmp("false", text) == 0)
            {
                return 9063;
            }
            /*break*/ goto _CKL0;

            /*case 103*/ _CKL30:
            if (strcmp("goto", text) == 0)
            {
                return 9016;
            }
            /*break*/ goto _CKL0;

            /*case 105*/ _CKL32:
            if (strcmp("if", text) == 0)
            {
                return 9017;
            }
            if (strcmp("inline", text) == 0)
            {
                return 9018;
            }
            if (strcmp("int", text) == 0)
            {
                return 9019;
            }
            /*break*/ goto _CKL0;

            /*case 110*/ _CKL36:
            if (strcmp("nullptr", text) == 0)
            {
                return 9064;
            }
            /*break*/ goto _CKL0;

            /*case 108*/ _CKL38:
            if (strcmp("long", text) == 0)
            {
                return 9020;
            }
            /*break*/ goto _CKL0;

            /*case 114*/ _CKL40:
            if (strcmp("register", text) == 0)
            {
                return 9025;
            }
            if (strcmp("restrict", text) == 0)
            {
                return 9026;
            }
            if (strcmp("return", text) == 0)
            {
                return 9027;
            }
            /*break*/ goto _CKL0;

            /*case 115*/ _CKL44:
            if (strcmp("short", text) == 0)
            {
                return 9028;
            }
            if (strcmp("signed", text) == 0)
            {
                return 9029;
            }
            if (strcmp("sizeof", text) == 0)
            {
                return 9030;
            }
            if (strcmp("static", text) == 0)
            {
                return 9032;
            }
            if (strcmp("struct", text) == 0)
            {
                return 9033;
            }
            if (strcmp("switch", text) == 0)
            {
                return 9034;
            }
            if (strcmp("static_assert", text) == 0)
            {
                return 9058;
            }
            if (strcmp("static_debug", text) == 0)
            {
                return 9072;
            }
            if (strcmp("static_debug_ex", text) == 0)
            {
                return 9073;
            }
            if (strcmp("static_state", text) == 0)
            {
                return 9074;
            }
            if (strcmp("static_set", text) == 0)
            {
                return 9075;
            }
            /*break*/ goto _CKL0;

            /*case 116*/ _CKL56:
            if (strcmp("typedef", text) == 0)
            {
                return 9035;
            }
            if (strcmp("typeof", text) == 0)
            {
                return 9061;
            }
            if (strcmp("typeof_unqual", text) == 0)
            {
                return 9065;
            }
            if (strcmp("true", text) == 0)
            {
                return 9062;
            }
            if (strcmp("thread_local", text) == 0)
            {
                return 9060;
            }
            if (strcmp("try", text) == 0)
            {
                return 9036;
            }
            if (strcmp("throw", text) == 0)
            {
                return 9037;
            }
            /*break*/ goto _CKL0;

            /*case 117*/ _CKL64:
            if (strcmp("union", text) == 0)
            {
                return 9038;
            }
            if (strcmp("unsigned", text) == 0)
            {
                return 9039;
            }
            /*break*/ goto _CKL0;

            /*case 118*/ _CKL67:
            if (strcmp("void", text) == 0)
            {
                return 9040;
            }
            if (strcmp("volatile", text) == 0)
            {
                return 9041;
            }
            /*break*/ goto _CKL0;

            /*case 119*/ _CKL70:
            if (strcmp("while", text) == 0)
            {
                return 9042;
            }
            /*break*/ goto _CKL0;

            /*case 95*/ _CKL72:
            if (strcmp("_Ctor", text) == 0)
            {
                return 9068;
            }
            if (strcmp("_Owner", text) == 0)
            {
                return 9067;
            }
            if (strcmp("_Dtor", text) == 0)
            {
                return 9069;
            }
            if (strcmp("_Opt", text) == 0)
            {
                return 9071;
            }
            if (strcmp("_View", text) == 0)
            {
                return 9070;
            }
            if (strcmp("_Countof", text) == 0)
            {
                return 9031;
            }
            if (strcmp("_is_lvalue", text) == 0)
            {
                return 9077;
            }
            if (strcmp("_is_const", text) == 0)
            {
                return 9078;
            }
            if (strcmp("_is_owner", text) == 0)
            {
                return 9079;
            }
            if (strcmp("_is_pointer", text) == 0)
            {
                return 9076;
            }
            if (strcmp("_is_array", text) == 0)
            {
                return 9080;
            }
            if (strcmp("_is_function", text) == 0)
            {
                return 9081;
            }
            if (strcmp("_is_arithmetic", text) == 0)
            {
                return 9083;
            }
            if (strcmp("_is_floating_point", text) == 0)
            {
                return 9084;
            }
            if (strcmp("_is_integral", text) == 0)
            {
                return 9085;
            }
            if (strcmp("_is_scalar", text) == 0)
            {
                return 9082;
            }
            if (strcmp("_Alignof", text) == 0)
            {
                return 9044;
            }
            if (strcmp("_Alignas", text) == 0)
            {
                return 9043;
            }
            if (strcmp("_Atomic", text) == 0)
            {
                return 9045;
            }
            if (strcmp("_Bool", text) == 0)
            {
                return 9050;
            }
            if (strcmp("_Complex", text) == 0)
            {
                return 9051;
            }
            if (strcmp("_Decimal32", text) == 0)
            {
                return 9053;
            }
            if (strcmp("_Decimal64", text) == 0)
            {
                return 9054;
            }
            if (strcmp("_Decimal128", text) == 0)
            {
                return 9052;
            }
            if (strcmp("_Generic", text) == 0)
            {
                return 9055;
            }
            if (strcmp("_Imaginary", text) == 0)
            {
                return 9056;
            }
            if (strcmp("_Noreturn", text) == 0)
            {
                return 9057;
            }
            if (strcmp("_Static_assert", text) == 0)
            {
                return 9058;
            }
            if (strcmp("_Thread_local", text) == 0)
            {
                return 9060;
            }
            if (strcmp("_BitInt", text) == 0)
            {
                return 9066;
            }
            if (strcmp("__typeof__", text) == 0)
            {
                return 9061;
            }
            if (strcmp("__int8", text) == 0)
            {
                return 9021;
            }
            if (strcmp("__int16", text) == 0)
            {
                return 9022;
            }
            if (strcmp("__int32", text) == 0)
            {
                return 9023;
            }
            if (strcmp("__int64", text) == 0)
            {
                return 9024;
            }
            if (strcmp("__forceinline", text) == 0)
            {
                return 9018;
            }
            if (strcmp("__inline", text) == 0)
            {
                return 9018;
            }
            if (strcmp("_asm", text) == 0 || strcmp("__asm", text) == 0)
            {
                return 9049;
            }
            if (strcmp("__stdcall", text) == 0 || strcmp("_stdcall", text) == 0)
            {
                return 9047;
            }
            if (strcmp("__cdecl", text) == 0)
            {
                return 9048;
            }
            if (strcmp("__fastcall", text) == 0)
            {
                return 9046;
            }
            if (strcmp("__alignof", text) == 0)
            {
                return 9044;
            }
            if (strcmp("__restrict", text) == 0)
            {
                return 9026;
            }
            /*break*/ goto _CKL0;

            /*default*/ _CKL116:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return 0;
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
        int   t;

        t = is_keyword(token->lexeme);
        if (t != 0)
        {
            token->type = t;
        }
    }
    else
    {
        if (token->type == 134)
        {
            char errormsg[100];
            char suffix[4];

            _cake_zmem(&errormsg, 100);
            _cake_zmem(&suffix, 4);
            token->type = parse_number(token->lexeme, suffix, errormsg);
            if (token->type == 0)
            {
                compiler_diagnostic(1380, ctx, token, 0U, errormsg);
            }
        }
    }
}

struct token *parser_look_ahead(struct parser_ctx * ctx)
{
    struct token * p;

    if (ctx->current == 0U)
    {
        return 0U;
    }
    p = ctx->current->next;
    while (p && !(p->flags & 1))
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
    struct token * p_token;

    p_token = p_current->next;
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
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
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
                unsigned char   onoff;

                ctx->current = ctx->current->next;
                pragma_skip_blanks(ctx);
                onoff = 0;
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
                        compiler_diagnostic(1250, ctx, ctx->current, 0U, "nullchecks pragma needs to use ON OFF");
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
                    if (ctx->options.diagnostic_stack.top_index < 10U)
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
                            unsigned char   is_error;
                            unsigned char   is_warning;
                            unsigned char   is_note;

                            is_error = !!(strcmp(ctx->current->lexeme, "error") == 0);
                            is_warning = !!(strcmp(ctx->current->lexeme, "warning") == 0);
                            is_note = !!(strcmp(ctx->current->lexeme, "note") == 0);
                            ctx->current = ctx->current->next;
                            pragma_skip_blanks(ctx);
                            if (ctx->current && ctx->current->type == 130)
                            {
                                unsigned long long w;

                                w = get_warning_bit_mask(ctx->current->lexeme + 1);
                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
                                if (is_error)
                                {
                                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors |= w;
                                }
                                else
                                {
                                    if (is_warning)
                                    {
                                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
                                    }
                                    else
                                    {
                                        if (is_note)
                                        {
                                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes |= w;
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
                                    int   id;
                                    unsigned char   found;

                                    id = get_warning(ctx->current->lexeme + 1 + 2);
                                    found = 0;
                                    {
                                        int i;
                                        i = 0;
                                        for (; i < 2; i++)
                                        {
                                            if (ctx->p_report->last_diagnostics_ids[i] == 0)
                                            {
                                                break;
                                            }
                                            if (ctx->p_report->last_diagnostics_ids[i] == id)
                                            {
                                                int t;

                                                found = 1;
                                                t = get_diagnostic_type(&ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index], id);
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
                                    if (!found)
                                    {
                                        compiler_diagnostic(650, ctx, ctx->current, 0U, "pragma check failed");
                                    }
                                }
                            }
                            else
                            {
                                compiler_diagnostic(650, ctx, ctx->current, 0U, "unknown pragma");
                            }
                        }
                    }
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

static void parser_skip_blanks(struct parser_ctx * ctx)
{
    while (ctx->current && !(ctx->current->flags & 1))
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
    if (ctx->current == 0U)
    {
        return;
    }
    ctx->previous = ctx->current;
    ctx->current = ctx->current->next;
    parser_skip_blanks(ctx);
}

void unexpected_end_of_file(struct parser_ctx * ctx)
{
    compiler_diagnostic(970, ctx, ctx->input_list.tail, 0U, "unexpected end of file");
}

char *get_token_name(int   tk);

int parser_match_tk(struct parser_ctx * ctx, int   type)
{
    int error;

    error = 0;
    if (ctx->current != 0U)
    {
        if (ctx->current->type != type)
        {
            compiler_diagnostic(970, ctx, ctx->current, 0U, "expected %s", get_token_name(type));
            error = 1;
        }
        ctx->previous = ctx->current;
        ctx->current = ctx->current->next;
        parser_skip_blanks(ctx);
    }
    else
    {
        compiler_diagnostic(970, ctx, ctx->input_list.tail, 0U, "unexpected end of file after");
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
    unsigned char   first;

    first = 1;
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
    if (((*flags) & 256) || ((*flags) & 128))
    {
        if (!type_specifier_is_integer(*flags))
        {
            (*flags) |= 8;
        }
    }
    return 0;
}

int add_specifier(struct parser_ctx * ctx, int  * flags, int   new_flag)
{
    if (new_flag & 16)
    {
        if ((*flags) & 4194304)
        {
            compiler_diagnostic(980, ctx, ctx->current, 0U, "cannot combine with previous 'long long' declaration specifier");
            return 1;
        }
        else
        {
            if ((*flags) & 16)
            {
                (*flags) = (*flags) & -17;
                (*flags) |= 4194304;
            }
            else
            {
                (*flags) = (*flags) & -9;
                (*flags) |= 16;
            }
        }
    }
    else
    {
        (*flags) |= new_flag;
    }
    /*switch*/
    {
        register unsigned int _R1 = (unsigned int)*flags;
        if (_R1 == 1) goto _CKL4; /*case 1*/
        if (_R1 == 2) goto _CKL5; /*case 2*/
        if (_R1 == 130) goto _CKL6; /*case 130*/
        if (_R1 == 258) goto _CKL7; /*case 258*/
        if (_R1 == 4) goto _CKL8; /*case 4*/
        if (_R1 == 132) goto _CKL9; /*case 132*/
        if (_R1 == 12) goto _CKL10; /*case 12*/
        if (_R1 == 140) goto _CKL11; /*case 140*/
        if (_R1 == 260) goto _CKL12; /*case 260*/
        if (_R1 == 268) goto _CKL13; /*case 268*/
        if (_R1 == 8) goto _CKL14; /*case 8*/
        if (_R1 == 128) goto _CKL15; /*case 128*/
        if (_R1 == 136) goto _CKL16; /*case 136*/
        if (_R1 == 256) goto _CKL17; /*case 256*/
        if (_R1 == 264) goto _CKL18; /*case 264*/
        if (_R1 == 16) goto _CKL19; /*case 16*/
        if (_R1 == 144) goto _CKL20; /*case 144*/
        if (_R1 == 24) goto _CKL21; /*case 24*/
        if (_R1 == 152) goto _CKL22; /*case 152*/
        if (_R1 == 272) goto _CKL23; /*case 272*/
        if (_R1 == 280) goto _CKL24; /*case 280*/
        if (_R1 == 4194304) goto _CKL25; /*case 4194304*/
        if (_R1 == 4194432) goto _CKL26; /*case 4194432*/
        if (_R1 == 4194312) goto _CKL27; /*case 4194312*/
        if (_R1 == 4194440) goto _CKL28; /*case 4194440*/
        if (_R1 == 4194560) goto _CKL29; /*case 4194560*/
        if (_R1 == 4194568) goto _CKL30; /*case 4194568*/
        if (_R1 == 32) goto _CKL31; /*case 32*/
        if (_R1 == 64) goto _CKL32; /*case 64*/
        if (_R1 == 80) goto _CKL33; /*case 80*/
        if (_R1 == 2048) goto _CKL34; /*case 2048*/
        if (_R1 == 4096) goto _CKL35; /*case 4096*/
        if (_R1 == 8192) goto _CKL36; /*case 8192*/
        if (_R1 == 512) goto _CKL37; /*case 512*/
        if (_R1 == 1056) goto _CKL38; /*case 1056*/
        if (_R1 == 1088) goto _CKL39; /*case 1088*/
        if (_R1 == 1104) goto _CKL40; /*case 1104*/
        if (_R1 == 16384) goto _CKL41; /*case 16384*/
        if (_R1 == 32768) goto _CKL42; /*case 32768*/
        if (_R1 == 65536) goto _CKL43; /*case 65536*/
        if (_R1 == 8388608) goto _CKL44; /*case 8388608*/
        if (_R1 == 131072) goto _CKL45; /*case 131072*/
        if (_R1 == 262144) goto _CKL46; /*case 262144*/
        if (_R1 == 262400) goto _CKL47; /*case 262400*/
        if (_R1 == 524288) goto _CKL48; /*case 524288*/
        if (_R1 == 524544) goto _CKL49; /*case 524544*/
        if (_R1 == 524416) goto _CKL50; /*case 524416*/
        if (_R1 == 1048576) goto _CKL51; /*case 1048576*/
        if (_R1 == 1048832) goto _CKL52; /*case 1048832*/
        if (_R1 == 1048704) goto _CKL53; /*case 1048704*/
        if (_R1 == 2097152) goto _CKL54; /*case 2097152*/
        if (_R1 == 2097408) goto _CKL55; /*case 2097408*/
        if (_R1 == 2097280) goto _CKL56; /*case 2097280*/
        goto /*default*/ _CKL57;

        {
            /*case 1*/ _CKL4:
            /*case 2*/ _CKL5:
            /*case 130*/ _CKL6:
            /*case 258*/ _CKL7:
            /*case 4*/ _CKL8:
            /*case 132*/ _CKL9:
            /*case 12*/ _CKL10:
            /*case 140*/ _CKL11:
            /*case 260*/ _CKL12:
            /*case 268*/ _CKL13:
            /*case 8*/ _CKL14:
            /*case 128*/ _CKL15:
            /*case 136*/ _CKL16:
            /*case 256*/ _CKL17:
            /*case 264*/ _CKL18:
            /*case 16*/ _CKL19:
            /*case 144*/ _CKL20:
            /*case 24*/ _CKL21:
            /*case 152*/ _CKL22:
            /*case 272*/ _CKL23:
            /*case 280*/ _CKL24:
            /*case 4194304*/ _CKL25:
            /*case 4194432*/ _CKL26:
            /*case 4194312*/ _CKL27:
            /*case 4194440*/ _CKL28:
            /*case 4194560*/ _CKL29:
            /*case 4194568*/ _CKL30:
            /*case 32*/ _CKL31:
            /*case 64*/ _CKL32:
            /*case 80*/ _CKL33:
            /*case 2048*/ _CKL34:
            /*case 4096*/ _CKL35:
            /*case 8192*/ _CKL36:
            /*case 512*/ _CKL37:
            /*case 1056*/ _CKL38:
            /*case 1088*/ _CKL39:
            /*case 1104*/ _CKL40:
            /*case 16384*/ _CKL41:
            /*case 32768*/ _CKL42:
            /*case 65536*/ _CKL43:
            /*case 8388608*/ _CKL44:
            /*case 131072*/ _CKL45:
            /*case 262144*/ _CKL46:
            /*case 262400*/ _CKL47:
            /*case 524288*/ _CKL48:
            /*case 524544*/ _CKL49:
            /*case 524416*/ _CKL50:
            /*case 1048576*/ _CKL51:
            /*case 1048832*/ _CKL52:
            /*case 1048704*/ _CKL53:
            /*case 2097152*/ _CKL54:
            /*case 2097408*/ _CKL55:
            /*case 2097280*/ _CKL56:
            /*break*/ goto _CKL3;

            /*default*/ _CKL57:
            compiler_diagnostic(1300, ctx, ctx->current, 0U, "incompatible specifiers");
            return 1;
        }
        _CKL3:;
    }
    return 0;
}

void attribute_specifier_sequence_delete(struct attribute_specifier_sequence * p);
void declaration_specifier_delete(struct declaration_specifier * p);

void declaration_specifiers_delete(struct declaration_specifiers * p)
{
    if (p)
    {
        struct declaration_specifier * item;

        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence_opt);
        item = p->head;
        while (item)
        {
            struct declaration_specifier * next;

            next = item->next;
            item->next = 0U;
            declaration_specifier_delete(item);
            item = next;
        }
        free(p);
    }
}

void declaration_specifiers_add(struct declaration_specifiers * list, struct declaration_specifier * p_item)
{
    if (list->head == 0U)
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

void *calloc(unsigned int nmemb, unsigned int size);
struct declaration_specifier *declaration_specifier(struct parser_ctx * ctx);
struct attribute_specifier_sequence *attribute_specifier_sequence_opt(struct parser_ctx * ctx);

struct declaration_specifiers *declaration_specifiers(struct parser_ctx * ctx, int   default_storage_flag)
{
    struct declaration_specifiers * p_declaration_specifiers;

    if (ctx->current == 0U)
    {
        return 0U;
    }
    p_declaration_specifiers = calloc(1, 56U);
    /*try*/ if (1)
    {
        struct token * prev;

        if (p_declaration_specifiers == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_declaration_specifiers->first_token = ctx->current;
        while (first_of_declaration_specifier(ctx))
        {
            struct declaration_specifier * p_declaration_specifier;

            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL1;
            }
            if (ctx->current->flags & 16)
            {
                if (p_declaration_specifiers->type_specifier_flags != 0)
                {
                    break;
                }
            }
            p_declaration_specifier = declaration_specifier(ctx);
            if (p_declaration_specifier == 0U)
            {
                /*throw*/ goto _CKL1;
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
                                    p_declaration_specifiers->typedef_declarator = find_declarator(ctx, p_declaration_specifier->type_specifier_qualifier->type_specifier->token->lexeme, 0U);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                    {
                        p_declaration_specifiers->type_qualifier_flags |= p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;
                    }
                }
            }
            else
            {
                if (p_declaration_specifier->storage_class_specifier)
                {
                    p_declaration_specifiers->storage_class_specifier_flags |= p_declaration_specifier->storage_class_specifier->flags;
                }
                else
                {
                    if (p_declaration_specifier->function_specifier)
                    {
                        p_declaration_specifiers->function_specifier_flags |= p_declaration_specifier->function_specifier->flags;
                    }
                }
            }
            declaration_specifiers_add(p_declaration_specifiers, p_declaration_specifier);
            ;
            p_declaration_specifiers->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL1;
            }
            if (ctx->current->type == 8996 && p_declaration_specifiers->type_specifier_flags != 0)
            {
                break;
            }
        }
        prev = previous_parser_token(ctx->current);
        if (prev == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_declaration_specifiers->last_token = prev;
        final_specifier(ctx, &p_declaration_specifiers->type_specifier_flags);
        p_declaration_specifiers->storage_class_specifier_flags |= default_storage_flag;
        if (p_declaration_specifiers->storage_class_specifier_flags & 4)
        {
            p_declaration_specifiers->storage_class_specifier_flags &= -4097;
        }
    }
    /*catch*/ else _CKL1:
    {
        declaration_specifiers_delete(p_declaration_specifiers);
        p_declaration_specifiers = 0U;
    }
    return p_declaration_specifiers;
}

struct static_assert_declaration *static_assert_declaration(struct parser_ctx * ctx);
struct pragma_declaration *pragma_declaration(struct parser_ctx * ctx);
struct init_declarator_list init_declarator_list(struct parser_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers);
void declaration_delete(struct declaration * p);

struct declaration *declaration_core(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt, unsigned char   can_be_function_definition, unsigned char  * is_function_definition, int   default_storage_class_specifier_flags, unsigned char   without_semicolon)
{
    struct declaration * p_declaration;

    p_declaration = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_declaration = calloc(1, 52U);
        if (p_declaration == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_declaration->p_attribute_specifier_sequence_opt = p_attribute_specifier_sequence_opt;
        p_attribute_specifier_sequence_opt = 0U;
        p_declaration->first_token = ctx->current;
        if (ctx->current->type == 59)
        {
            p_declaration->last_token = ctx->current;
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
                    if (p_declaration->declaration_specifiers == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    if (p_declaration->p_attribute_specifier_sequence_opt)
                    {
                        p_declaration->declaration_specifiers->attributes_flags = p_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
                    }
                    if (ctx->current == 0U)
                    {
                        unexpected_end_of_file(ctx);
                        /*throw*/ goto _CKL0;
                    }
                    if (ctx->current->type != 59)
                    {
                        p_declaration->init_declarator_list = init_declarator_list(ctx, p_declaration->declaration_specifiers);
                        if (p_declaration->init_declarator_list.head == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                    }
                    if (ctx->current == 0U)
                    {
                        unexpected_end_of_file(ctx);
                        /*throw*/ goto _CKL0;
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
                        if (ctx->current->type == 9062 || ctx->current->type == 9063 || ctx->current->type == 8996)
                        {
                            if (can_be_function_definition)
                            {
                                *is_function_definition = 1;
                            }
                        }
                        else
                        {
                            if (!without_semicolon && parser_match_tk(ctx, 59) != 0)
                            {
                                /*throw*/ goto _CKL0;
                            }
                        }
                    }
                }
                else
                {
                    if (ctx->current->type == 8996)
                    {
                        compiler_diagnostic(860, ctx, ctx->current, 0U, "invalid type '%s'", ctx->current->lexeme);
                    }
                    else
                    {
                        compiler_diagnostic(990, ctx, ctx->current, 0U, "expected declaration not '%s'", ctx->current->lexeme);
                    }
                    parser_match(ctx);
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        declaration_delete(p_declaration);
        p_declaration = 0U;
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
    return p_declaration;
}

struct expression *expression(struct parser_ctx * ctx);
struct compound_statement *function_body(struct parser_ctx * ctx);
void defer_start_visit_declaration(struct defer_visit_ctx * ctx, struct declaration * p_declaration);
void defer_visit_ctx_destroy(struct defer_visit_ctx * p);
void flow_start_visit_declaration(struct flow_visit_ctx * ctx, struct declaration * p_declaration);
void flow_visit_ctx_destroy(struct flow_visit_ctx * p);
unsigned char  type_is_maybe_unused(struct type * p_type);

struct declaration *function_definition_or_declaration(struct parser_ctx * ctx)
{
    struct declaration * p_declaration;

    p_declaration = 0U;
    /*try*/ if (1)
    {
        struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
        unsigned char   is_function_definition;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        is_function_definition = 0;
        p_declaration = declaration_core(ctx, p_attribute_specifier_sequence_opt, 1, &is_function_definition, 0, 0);
        if (p_declaration == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (is_function_definition)
        {
            struct declarator * inner;
            struct scope * parameters_scope;
            struct diagnostic  before_function_diagnostics;
            struct declarator * p_declarator;
            struct declarator * p_current_function_opt;
            struct compound_statement * p_function_body;
            struct parameter_declaration * parameter;

            if (p_declaration->init_declarator_list.head == 0U || p_declaration->init_declarator_list.head->p_declarator->direct_declarator == 0U || p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            ctx->p_current_function_opt = p_declaration->init_declarator_list.head->p_declarator;
            ;
            inner = p_declaration->init_declarator_list.head->p_declarator;
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
            parameters_scope = &inner->direct_declarator->function_declarator->parameters_scope;
            scope_list_push(&ctx->scopes, parameters_scope);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            check_func_open_brace_style(ctx, ctx->current);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            before_function_diagnostics = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];
            p_declarator = p_declaration->init_declarator_list.head->p_declarator;
            if (ctx->current->type == 9062 || ctx->current->type == 9063 || ctx->current->type == 8996)
            {
                for (; ; )
                {
                    int   type;

                    if (ctx->current == 0U)
                    {
                        unexpected_end_of_file(ctx);
                        /*throw*/ goto _CKL0;
                    }
                    type = ctx->current->type;
                    if (type != 9062 && type != 9063 && type != 8996)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    parser_match(ctx);
                    parser_match(ctx);
                    if (type != 9063)
                    {
                        ;
                        p_declarator->p_expression_true = expression(ctx);
                    }
                    else
                    {
                        ;
                        p_declarator->p_expression_false = expression(ctx);
                    }
                    parser_match(ctx);
                    if (ctx->current == 0U)
                    {
                        unexpected_end_of_file(ctx);
                        /*throw*/ goto _CKL0;
                    }
                    if (ctx->current->type != 44)
                    {
                        break;
                    }
                    parser_match(ctx);
                }
            }
            p_current_function_opt = ctx->p_current_function_opt;
            ctx->p_current_function_opt = p_declarator;
            p_function_body = function_body(ctx);
            ctx->p_current_function_opt = p_current_function_opt;
            if (p_function_body == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            ;
            p_declaration->function_body = p_function_body;
            p_declaration->init_declarator_list.head->p_declarator->function_body = p_declaration->function_body;
            {
                struct defer_visit_ctx  ctx2;

                ctx2.catch_secondary_block_opt = 0;
                ctx2.ctx = ctx;
                ctx2.ast.token_list.head = 0;
                ctx2.ast.token_list.tail = 0;
                ctx2.ast.declaration_list.head = 0;
                ctx2.ast.declaration_list.tail = 0;
                ctx2.tail_block = 0;
                ctx2.parameter_list = 0;
                defer_start_visit_declaration(&ctx2, p_declaration);
                defer_visit_ctx_destroy(&ctx2);
            }
            if (ctx->options.flow_analysis)
            {
                struct flow_visit_ctx  ctx2;

                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = before_function_diagnostics;
                _cake_zmem(&ctx2, 868);
                ctx2.ctx = ctx;
                flow_start_visit_declaration(&ctx2, p_declaration);
                flow_visit_ctx_destroy(&ctx2);
            }
            parameter = 0U;
            if (p_declaration->init_declarator_list.head && p_declaration->init_declarator_list.head->p_declarator->direct_declarator && p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator && p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt && p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
            {
                parameter = p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
            }
            while (parameter)
            {
                if (!type_is_maybe_unused(&parameter->declarator->type) && parameter->declarator && parameter->declarator->num_uses == 0)
                {
                    if (parameter->declarator->name_opt && parameter->declarator->name_opt->level == 0)
                    {
                        compiler_diagnostic(5, ctx, parameter->declarator->name_opt, 0U, "'%s': unreferenced formal parameter", parameter->declarator->name_opt->lexeme);
                    }
                }
                parameter = parameter->next;
            }
            scope_list_pop(&ctx->scopes);
            ctx->p_current_function_opt = 0U;
        }
        else
        {
            if (ctx->options.flow_analysis)
            {
                struct flow_visit_ctx  ctx2;

                _cake_zmem(&ctx2, 868);
                ctx2.ctx = ctx;
                flow_start_visit_declaration(&ctx2, p_declaration);
                flow_visit_ctx_destroy(&ctx2);
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        declaration_delete(p_declaration);
        p_declaration = 0U;
    }
    return p_declaration;
}

void simple_declaration_delete(struct simple_declaration * p);

struct simple_declaration *simple_declaration(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt, unsigned char   ignore_semicolon)
{
    int   storage_specifier_flags;
    struct simple_declaration * p_simple_declaration;

    if (ctx->current == 0U)
    {
        unexpected_end_of_file(ctx);
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
        return 0U;
    }
    storage_specifier_flags = 4096;
    p_simple_declaration = calloc(1, 24U);
    /*try*/ if (1)
    {
        struct declaration_specifiers * ptemp;
        struct token * prev;

        if (p_simple_declaration == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_simple_declaration->first_token = ctx->current;
        p_simple_declaration->p_attribute_specifier_sequence_opt = p_attribute_specifier_sequence_opt;
        p_attribute_specifier_sequence_opt = 0U;
        ptemp = declaration_specifiers(ctx, storage_specifier_flags);
        if (ptemp == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_simple_declaration->p_declaration_specifiers = ptemp;
        if (p_simple_declaration->p_attribute_specifier_sequence_opt)
        {
            p_simple_declaration->p_declaration_specifiers->attributes_flags = p_simple_declaration->p_attribute_specifier_sequence_opt->attributes_flags;
        }
        p_simple_declaration->init_declarator_list = init_declarator_list(ctx, p_simple_declaration->p_declaration_specifiers);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL1;
        }
        prev = previous_parser_token(ctx->current);
        if (prev == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_simple_declaration->last_token = prev;
        if (!ignore_semicolon && parser_match_tk(ctx, 59) != 0)
        {
            /*throw*/ goto _CKL1;
        }
    }
    /*catch*/ else _CKL1:
    {
        simple_declaration_delete(p_simple_declaration);
        p_simple_declaration = 0U;
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
    return p_simple_declaration;
}

struct declaration *declaration(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt, int   storage_specifier_flags)
{
    unsigned char   is_function_definition;

    is_function_definition = 0;
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
    struct declaration_specifier * p_declaration_specifier;

    p_declaration_specifier = 0U;
    /*try*/ if (1)
    {
        p_declaration_specifier = calloc(1, 16U);
        if (p_declaration_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (first_of_storage_class_specifier(ctx))
        {
            p_declaration_specifier->storage_class_specifier = storage_class_specifier(ctx);
            if (p_declaration_specifier->storage_class_specifier == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            if (first_of_type_specifier_qualifier(ctx))
            {
                p_declaration_specifier->type_specifier_qualifier = type_specifier_qualifier(ctx);
                if (p_declaration_specifier->type_specifier_qualifier == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            else
            {
                if (first_of_function_specifier(ctx))
                {
                    p_declaration_specifier->function_specifier = function_specifier(ctx);
                    if (p_declaration_specifier->function_specifier == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                }
                else
                {
                    compiler_diagnostic(650, ctx, ctx->current, 0U, "unexpected");
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        declaration_specifier_delete(p_declaration_specifier);
        p_declaration_specifier = 0U;
    }
    return p_declaration_specifier;
}

struct init_declarator *init_declarator_add_ref(struct init_declarator * p)
{
    p->has_shared_ownership = 1;
    return (struct init_declarator *)p;
}

void init_declarator_sink(struct init_declarator * p)
{
}

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
void print_type_no_names(struct osstream * ss, struct type * p_type);
void ss_clear(struct osstream * stream);
void ss_close(struct osstream * stream);
int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item);
void hash_item_set_destroy(struct hash_item_set * p);
struct initializer *initializer(struct parser_ctx * ctx);
int make_object(struct type * p_type, struct object * obj);
unsigned char  type_is_const(struct type * p_type);
int initializer_init_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * p_current_object, struct initializer * braced_initializer, unsigned char   is_constant, unsigned char   requires_constant_initialization);
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
int make_object_with_name(struct type * p_type, struct object * obj, char * name);
unsigned char  type_is_pointer(struct type * p_type);
unsigned char  type_is_function(struct type * p_type);
unsigned char  type_is_vla(struct type * p_type);
unsigned int type_get_sizeof(struct type * p_type);

struct init_declarator *init_declarator(struct parser_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers)
{
    struct init_declarator * p_init_declarator;

    p_init_declarator = 0U;
    /*try*/ if (1)
    {
        struct token * tkname;
        char * name;
        struct scope * out_scope;
        struct declarator * previous;

        p_init_declarator = calloc(1, 16U);
        if (p_init_declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        tkname = 0U;
        {
            struct declarator * p_temp_declarator;

            p_temp_declarator = declarator(ctx, 0U, p_declaration_specifiers, 0, &tkname);
            if (p_temp_declarator == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_init_declarator->p_declarator = p_temp_declarator;
        }
        if (tkname == 0U)
        {
            compiler_diagnostic(650, ctx, ctx->current, 0U, "init declarator must have a name");
            /*throw*/ goto _CKL0;
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
        name = p_init_declarator->p_declarator->name_opt->lexeme;
        out_scope = 0U;
        previous = find_declarator(ctx, name, &out_scope);
        if (previous)
        {
            ;
            ;
            if (out_scope->scope_level == ctx->scopes.tail->scope_level)
            {
                if (out_scope->scope_level == 0)
                {
                    if (strcmp(name, "__C_ASSERT__") != 0)
                    {
                        if (!type_is_same(&previous->type, &p_init_declarator->p_declarator->type, 0))
                        {
                            struct osstream  ss;

                            _cake_zmem(&ss, 12);
                            print_type_no_names(&ss, &previous->type);
                            compiler_diagnostic(1020, ctx, ctx->current, 0U, "conflicting types for '%s' (%s)", name, ss.c_str);
                            ss_clear(&ss);
                            print_type_no_names(&ss, &p_init_declarator->p_declarator->type);
                            compiler_diagnostic(1020, ctx, previous->name_opt, 0U, "previous declaration (%s)", ss.c_str);
                            ss_close(&ss);
                        }
                    }
                }
                else
                {
                    compiler_diagnostic(1020, ctx, ctx->current, 0U, "redeclaration");
                    compiler_diagnostic(63, ctx, previous->name_opt, 0U, "previous declaration");
                }
            }
            else
            {
                struct hash_item_set  item;

                _cake_zmem(&item, 32);
                item.p_init_declarator = init_declarator_add_ref(p_init_declarator);
                hashmap_set(&ctx->scopes.tail->variables, name, &item);
                hash_item_set_destroy(&item);
                if (out_scope->scope_level != 0)
                {
                    if (compiler_diagnostic(6, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "declaration of '%s' hides previous declaration", name))
                    {
                        compiler_diagnostic(63, ctx, previous->first_token_opt, 0U, "previous declaration is here");
                    }
                }
            }
        }
        else
        {
            struct hash_item_set  item;

            _cake_zmem(&item, 32);
            item.p_init_declarator = init_declarator_add_ref(p_init_declarator);
            hashmap_set(&ctx->scopes.tail->variables, name, &item);
            hash_item_set_destroy(&item);
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 61)
        {
            unsigned char   requires_constant_initialization;

            requires_constant_initialization = !!((ctx->p_current_function_opt == 0U) || (p_declaration_specifiers->storage_class_specifier_flags & 4));
            parser_match(ctx);
            ;
            p_init_declarator->initializer = initializer(ctx);
            if (p_init_declarator->initializer == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (p_init_declarator->initializer->braced_initializer)
            {
                int er;
                unsigned char   is_constant;

                if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 16)
                {
                    compiler_diagnostic(1290, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "'auto' requires a plain identifier");
                    /*throw*/ goto _CKL0;
                }
                er = make_object(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object);
                if (er != 0)
                {
                    compiler_diagnostic(740, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "incomplete struct/union type");
                    /*throw*/ goto _CKL0;
                }
                is_constant = !!(type_is_const(&p_init_declarator->p_declarator->type) || p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 64);
                if (initializer_init_new(ctx, &p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, p_init_declarator->initializer, is_constant, requires_constant_initialization) != 0)
                {
                    /*throw*/ goto _CKL0;
                }
                p_init_declarator->p_declarator->object.type.num_of_elements = p_init_declarator->p_declarator->type.num_of_elements;
            }
            else
            {
                if (p_init_declarator->initializer->assignment_expression)
                {
                    char * name;
                    int er;
                    unsigned char   is_constant;

                    if (type_is_array(&p_init_declarator->p_declarator->type))
                    {
                        int array_size_elements;

                        array_size_elements = p_init_declarator->p_declarator->type.num_of_elements;
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
                                    compiler_diagnostic(51, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "initializer for array is too long");
                                }
                            }
                        }
                    }
                    ;
                    if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 16)
                    {
                        struct type  t;

                        if (p_init_declarator->p_declarator->direct_declarator && (p_init_declarator->p_declarator->direct_declarator->array_declarator != 0U || p_init_declarator->p_declarator->direct_declarator->function_declarator != 0U))
                        {
                            compiler_diagnostic(1290, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "'auto' requires a plain identifier");
                            /*throw*/ goto _CKL0;
                        }
                        if (p_init_declarator->p_declarator->pointer != 0U)
                        {
                            compiler_diagnostic(1290, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "'auto' requires a plain identifier");
                        }
                        _cake_zmem(&t, 60);
                        if (p_init_declarator->initializer->assignment_expression->expression_type == 33)
                        {
                            t = type_dup(&p_init_declarator->initializer->assignment_expression->type);
                        }
                        else
                        {
                            struct type  t2;

                            t2 = type_lvalue_conversion(&p_init_declarator->initializer->assignment_expression->type, ctx->options.null_checks_enabled);
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
                    check_assigment(ctx, &p_init_declarator->p_declarator->type, p_init_declarator->initializer->assignment_expression, 3);
                    name = p_init_declarator->p_declarator->name_opt ? p_init_declarator->p_declarator->name_opt->lexeme : "";
                    er = make_object_with_name(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, name);
                    if (er != 0)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    is_constant = !!(type_is_const(&p_init_declarator->p_declarator->type) || p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 64);
                    if (initializer_init_new(ctx, &p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, p_init_declarator->initializer, is_constant, requires_constant_initialization) != 0)
                    {
                        /*throw*/ goto _CKL0;
                    }
                }
            }
        }
        else
        {
            if (p_init_declarator->p_declarator->type.category != 1 && !(p_init_declarator->p_declarator->type.storage_class_specifier_flags & 1))
            {
                char * name;
                int er;

                name = p_init_declarator->p_declarator->name_opt ? p_init_declarator->p_declarator->name_opt->lexeme : "";
                er = make_object_with_name(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, name);
                if (er != 0)
                {
                    if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 2)
                    {
                    }
                    else
                    {
                        compiler_diagnostic(740, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "incomplete struct/union type");
                        /*throw*/ goto _CKL0;
                    }
                }
                if (type_is_const(&p_init_declarator->p_declarator->type))
                {
                    if (p_declaration_specifiers->storage_class_specifier_flags & 1)
                    {
                    }
                    else
                    {
                        compiler_diagnostic(47, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "const object should be initialized");
                    }
                }
            }
        }
        if (p_init_declarator->p_declarator)
        {
            if (type_is_array(&p_init_declarator->p_declarator->type))
            {
                if (p_init_declarator->p_declarator->type.type_qualifier_flags != 0 || p_init_declarator->p_declarator->type.has_static_array_size)
                {
                    if (p_init_declarator->p_declarator->first_token_opt)
                    {
                        compiler_diagnostic(1000, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "static or type qualifiers are not allowed in non-parameter array declarator");
                    }
                    else
                    {
                        if (p_init_declarator->initializer)
                        {
                            compiler_diagnostic(1000, ctx, p_init_declarator->initializer->first_token, 0U, "static or type qualifiers are not allowed in non-parameter array declarator");
                        }
                    }
                }
            }
            if (!type_is_pointer(&p_init_declarator->p_declarator->type) && p_init_declarator->p_declarator->type.type_qualifier_flags & 128)
            {
                if (p_init_declarator->p_declarator->first_token_opt)
                {
                    compiler_diagnostic(1010, ctx, p_init_declarator->p_declarator->first_token_opt, 0U, "_Dtor qualifier can only be used with pointers");
                }
                else
                {
                    if (p_init_declarator->initializer)
                    {
                        compiler_diagnostic(1010, ctx, p_init_declarator->initializer->first_token, 0U, "_Dtor qualifier can only be used with pointers");
                    }
                }
            }
        }
        if (!(p_init_declarator->p_declarator->type.storage_class_specifier_flags & 1) && !type_is_function(&p_init_declarator->p_declarator->type))
        {
            if (type_is_vla(&p_init_declarator->p_declarator->type))
            {
            }
            else
            {
                int sz;

                sz = type_get_sizeof(&p_init_declarator->p_declarator->type);
                if (sz == -3)
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
                            compiler_diagnostic(1270, ctx, p_init_declarator->p_declarator->name_opt, 0U, "storage size of '%s' isn't known", p_init_declarator->p_declarator->name_opt->lexeme);
                        }
                    }
                    else
                    {
                    }
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        init_declarator_delete(p_init_declarator);
        p_init_declarator = 0U;
    }
    return p_init_declarator;
}

void init_declarator_list_add(struct init_declarator_list * list, struct init_declarator * p_item)
{
    if (list->head == 0U)
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
    struct init_declarator * item;

    item = p->head;
    while (item)
    {
        struct init_declarator * next;

        next = item->next;
        item->next = 0U;
        init_declarator_delete(item);
        item = next;
    }
}

struct init_declarator_list init_declarator_list(struct parser_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers)
{
    struct init_declarator_list  init_declarator_list;
    struct init_declarator * p_init_declarator;

    _cake_zmem(&init_declarator_list, 8);
    p_init_declarator = 0U;
    /*try*/ if (1)
    {
        p_init_declarator = init_declarator(ctx, p_declaration_specifiers);
        if (p_init_declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        init_declarator_list_add(&init_declarator_list, p_init_declarator);
        p_init_declarator = 0U;
        while (ctx->current != 0U && ctx->current->type == 44)
        {
            parser_match(ctx);
            p_init_declarator = init_declarator(ctx, p_declaration_specifiers);
            if (p_init_declarator == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            init_declarator_list_add(&init_declarator_list, p_init_declarator);
            p_init_declarator = 0U;
        }
    }
    /*catch*/ else _CKL0:
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
    struct storage_class_specifier * p_storage_class_specifier;

    p_storage_class_specifier = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_storage_class_specifier = calloc(1, 8U);
        if (p_storage_class_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_storage_class_specifier->token = ctx->current;
        /*switch*/
        {
            register int   _R2 = ctx->current->type;
            if (_R2 == 9035) goto _CKL4; /*case 9035*/
            if (_R2 == 9013) goto _CKL5; /*case 9013*/
            if (_R2 == 9002) goto _CKL6; /*case 9002*/
            if (_R2 == 9032) goto _CKL8; /*case 9032*/
            if (_R2 == 9060) goto _CKL9; /*case 9060*/
            if (_R2 == 8999) goto _CKL10; /*case 8999*/
            if (_R2 == 9025) goto _CKL11; /*case 9025*/
            goto /*default*/ _CKL12;

            {
                /*case 9035*/ _CKL4:
                p_storage_class_specifier->flags = 1;
                /*break*/ goto _CKL3;

                /*case 9013*/ _CKL5:
                p_storage_class_specifier->flags = 2;
                /*break*/ goto _CKL3;

                /*case 9002*/ _CKL6:
                p_storage_class_specifier->flags = 64;
                if (ctx->scopes.tail && ctx->scopes.tail->scope_level == 0)
                {
                    p_storage_class_specifier->flags |= 128;
                }
                /*break*/ goto _CKL3;

                /*case 9032*/ _CKL8:
                p_storage_class_specifier->flags = 4;
                /*break*/ goto _CKL3;

                /*case 9060*/ _CKL9:
                p_storage_class_specifier->flags = 8;
                /*break*/ goto _CKL3;

                /*case 8999*/ _CKL10:
                p_storage_class_specifier->flags = 16;
                /*break*/ goto _CKL3;

                /*case 9025*/ _CKL11:
                p_storage_class_specifier->flags = 32;
                /*break*/ goto _CKL3;

                /*default*/ _CKL12:
                ;
            }
            _CKL3:;
        }
        parser_match(ctx);
    }
    /*catch*/ else _CKL0:
    {
        storage_class_specifier_delete(p_storage_class_specifier);
        p_storage_class_specifier = 0U;
    }
    return p_storage_class_specifier;
}

struct type_name *type_name(struct parser_ctx * ctx);
void typeof_specifier_argument_delete(struct typeof_specifier_argument * p);

struct typeof_specifier_argument *typeof_specifier_argument(struct parser_ctx * ctx)
{
    struct typeof_specifier_argument * new_typeof_specifier_argument;

    new_typeof_specifier_argument = 0U;
    /*try*/ if (1)
    {
        new_typeof_specifier_argument = calloc(1, 8U);
        if (new_typeof_specifier_argument == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (first_of_type_name(ctx))
        {
            new_typeof_specifier_argument->type_name = type_name(ctx);
        }
        else
        {
            unsigned char   disable_evaluation_copy;

            disable_evaluation_copy = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = 1;
            new_typeof_specifier_argument->expression = expression(ctx);
            ctx->evaluation_is_disabled = disable_evaluation_copy;
            if (new_typeof_specifier_argument->expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        typeof_specifier_argument_delete(new_typeof_specifier_argument);
        new_typeof_specifier_argument = 0U;
    }
    return new_typeof_specifier_argument;
}

unsigned char  first_of_typeof_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 9061 || ctx->current->type == 9065);
}

struct type type_param_array_to_pointer(struct type * p_type, unsigned char   null_checks_enabled);
void type_remove_qualifiers(struct type * p_type);
void typeof_specifier_delete(struct typeof_specifier * p);

struct typeof_specifier *typeof_specifier(struct parser_ctx * ctx)
{
    struct typeof_specifier * p_typeof_specifier;

    p_typeof_specifier = 0U;
    /*try*/ if (1)
    {
        unsigned char   is_typeof_unqual;
        struct typeof_specifier_argument * p_typeof_specifier_argument;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_typeof_specifier = calloc(1, 72U);
        if (p_typeof_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_typeof_specifier->first_token = ctx->current;
        is_typeof_unqual = !!(ctx->current->type == 9065);
        parser_match(ctx);
        if (parser_match_tk(ctx, 40) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        p_typeof_specifier_argument = typeof_specifier_argument(ctx);
        if (p_typeof_specifier_argument == 0U)
        {
            /*throw*/ goto _CKL0;
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
            compiler_diagnostic(7, ctx, ctx->current, 0U, "typeof used in array arguments");
            if (type_is_array(&p_typeof_specifier->type))
            {
                struct type  t;

                t = type_param_array_to_pointer(&p_typeof_specifier->type, ctx->options.null_checks_enabled);
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
        p_typeof_specifier->type.name_opt = 0U;
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_typeof_specifier->last_token = ctx->current;
        if (parser_match_tk(ctx, 41) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        typeof_specifier_delete(p_typeof_specifier);
        p_typeof_specifier = 0U;
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
    struct type_specifier * p_type_specifier;

    p_type_specifier = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_type_specifier = calloc(1, 28U);
        if (p_type_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        /*switch*/
        {
            register int   _R3 = ctx->current->type;
            if (_R3 == 9040) goto _CKL4; /*case 9040*/
            if (_R3 == 9003) goto _CKL5; /*case 9003*/
            if (_R3 == 9028) goto _CKL6; /*case 9028*/
            if (_R3 == 9019) goto _CKL7; /*case 9019*/
            if (_R3 == 9021) goto _CKL8; /*case 9021*/
            if (_R3 == 9022) goto _CKL9; /*case 9022*/
            if (_R3 == 9023) goto _CKL10; /*case 9023*/
            if (_R3 == 9024) goto _CKL11; /*case 9024*/
            if (_R3 == 9020) goto _CKL12; /*case 9020*/
            if (_R3 == 9014) goto _CKL13; /*case 9014*/
            if (_R3 == 9010) goto _CKL14; /*case 9010*/
            if (_R3 == 9029) goto _CKL15; /*case 9029*/
            if (_R3 == 9039) goto _CKL16; /*case 9039*/
            if (_R3 == 9050) goto _CKL17; /*case 9050*/
            if (_R3 == 9051) goto _CKL18; /*case 9051*/
            if (_R3 == 9053) goto _CKL19; /*case 9053*/
            if (_R3 == 9054) goto _CKL20; /*case 9054*/
            if (_R3 == 9052) goto _CKL21; /*case 9052*/
            goto /*default*/ _CKL22;

            {
                /*case 9040*/ _CKL4:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 1;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9003*/ _CKL5:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 2;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9028*/ _CKL6:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 4;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9019*/ _CKL7:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 8;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9021*/ _CKL8:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 262144;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9022*/ _CKL9:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 524288;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9023*/ _CKL10:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 1048576;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9024*/ _CKL11:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 2097152;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9020*/ _CKL12:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 16;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9014*/ _CKL13:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 32;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9010*/ _CKL14:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 64;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9029*/ _CKL15:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 128;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9039*/ _CKL16:
                p_type_specifier->flags = 256;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9050*/ _CKL17:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 512;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9051*/ _CKL18:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 1024;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9053*/ _CKL19:
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 2048;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9054*/ _CKL20:
                p_type_specifier->flags = 4096;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*case 9052*/ _CKL21:
                p_type_specifier->flags = 8192;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                /*default*/ _CKL22:
                /*break*/ goto _CKL3;

            }
            _CKL3:;
        }
        if (first_of_typeof_specifier(ctx))
        {
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 8388608;
            p_type_specifier->typeof_specifier = typeof_specifier(ctx);
            if (p_type_specifier->typeof_specifier == 0U)
            {
                type_specifier_delete(p_type_specifier);
                return 0U;
            }
        }
        else
        {
            if (first_of_atomic_type_specifier(ctx))
            {
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 16384;
                p_type_specifier->atomic_type_specifier = atomic_type_specifier(ctx);
                if (p_type_specifier->atomic_type_specifier == 0U)
                {
                    type_specifier_delete(p_type_specifier);
                    return 0U;
                }
            }
            else
            {
                if (first_of_struct_or_union(ctx))
                {
                    p_type_specifier->token = ctx->current;
                    p_type_specifier->flags = 32768;
                    p_type_specifier->struct_or_union_specifier = struct_or_union_specifier(ctx);
                    if (p_type_specifier->struct_or_union_specifier == 0U)
                    {
                        type_specifier_delete(p_type_specifier);
                        return 0U;
                    }
                }
                else
                {
                    if (first_of_enum_specifier(ctx))
                    {
                        p_type_specifier->token = ctx->current;
                        p_type_specifier->flags = 65536;
                        p_type_specifier->enum_specifier = enum_specifier(ctx);
                        if (p_type_specifier->enum_specifier == 0U)
                        {
                            type_specifier_delete(p_type_specifier);
                            return 0U;
                        }
                    }
                    else
                    {
                        if (ctx->current->type == 9066)
                        {
                            type_specifier_delete(p_type_specifier);
                            return 0U;
                        }
                        else
                        {
                            if (ctx->current->type == 8996)
                            {
                                p_type_specifier->token = ctx->current;
                                p_type_specifier->flags = 131072;
                                p_type_specifier->typedef_declarator = find_declarator(ctx, ctx->current->lexeme, 0U);
                                ;
                                parser_match(ctx);
                            }
                        }
                    }
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        type_specifier_delete(p_type_specifier);
        p_type_specifier = 0U;
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
    return 0U;
}

struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    if (p_struct_or_union_specifier->member_declaration_list.head)
    {
        return (struct struct_or_union_specifier *)p_struct_or_union_specifier;
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
    return 0U;
}

unsigned char  struct_or_union_specifier_is_complete(struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    return !!(get_complete_struct_or_union_specifier(p_struct_or_union_specifier) != 0U);
}

struct struct_or_union_specifier *struct_or_union_specifier_add_ref(struct struct_or_union_specifier * p)
{
    p->has_shared_ownership = 1;
    return (struct struct_or_union_specifier *)p;
}

void struct_or_union_specifier_sink(struct struct_or_union_specifier * p)
{
}

unsigned char  struct_or_union_specifier_is_union(struct struct_or_union_specifier * p)
{
    return !!(p->first_token->type == 9038);
}

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
    struct struct_or_union_specifier * p_struct_or_union_specifier;

    p_struct_or_union_specifier = 0U;
    /*try*/ if (1)
    {
        struct struct_or_union_specifier * p_first_tag_in_this_scope;
        struct struct_or_union_specifier * p_complete;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_struct_or_union_specifier = calloc(1, 256U);
        if (p_struct_or_union_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 9033 || ctx->current->type == 9038)
        {
            p_struct_or_union_specifier->first_token = ctx->current;
            parser_match(ctx);
        }
        else
        {
            /*throw*/ goto _CKL0;
        }
        p_struct_or_union_specifier->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        p_first_tag_in_this_scope = 0U;
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 8996)
        {
            struct map_entry * p_entry;

            p_struct_or_union_specifier->tagtoken = ctx->current;
            snprintf(p_struct_or_union_specifier->tag_name, 200U, "%s", ctx->current->lexeme);
            p_entry = hashmap_find(&ctx->scopes.tail->tags, ctx->current->lexeme);
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
                    compiler_diagnostic(1030, ctx, ctx->current, 0U, "use of '%s' with tag type that does not match previous declaration.", ctx->current->lexeme);
                }
            }
            else
            {
                struct struct_or_union_specifier * p_first_tag_previous_scopes;

                p_first_tag_previous_scopes = find_struct_or_union_specifier(ctx, ctx->current->lexeme);
                if (p_first_tag_previous_scopes == 0U)
                {
                    struct hash_item_set  item;

                    p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
                    _cake_zmem(&item, 32);
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
            struct hash_item_set  item;

            snprintf(p_struct_or_union_specifier->tag_name, 200U, "_struct_tag_%d", s_anonymous_struct_count);
            s_anonymous_struct_count++;
            p_struct_or_union_specifier->has_anonymous_tag = 1;
            p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
            _cake_zmem(&item, 32);
            item.p_struct_or_union_specifier = struct_or_union_specifier_add_ref(p_struct_or_union_specifier);
            hashmap_set(&ctx->scopes.tail->tags, p_struct_or_union_specifier->tag_name, &item);
            hash_item_set_destroy(&item);
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 123)
        {
            struct struct_or_union_specifier * first;
            struct token * firsttoken;

            first = find_struct_or_union_specifier(ctx, p_struct_or_union_specifier->tag_name);
            if (first)
            {
                first->complete_struct_or_union_specifier_indirection = p_struct_or_union_specifier;
            }
            if (p_struct_or_union_specifier->tagtoken)
            {
                naming_convention_struct_tag(ctx, p_struct_or_union_specifier->tagtoken);
            }
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            firsttoken = ctx->current;
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            if (ctx->current->type != 125)
            {
                p_struct_or_union_specifier->member_declaration_list = member_declaration_list(ctx, p_struct_or_union_specifier);
                if (p_struct_or_union_specifier->member_declaration_list.head == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            p_struct_or_union_specifier->member_declaration_list.first_token = firsttoken;
            p_struct_or_union_specifier->last_token = ctx->current;
            p_struct_or_union_specifier->member_declaration_list.last_token = ctx->current;
            if (parser_match_tk(ctx, 125) != 0)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            p_struct_or_union_specifier->last_token = ctx->current;
        }
        p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);
        if (p_complete)
        {
            if (p_complete->attribute_specifier_sequence_opt && p_complete->attribute_specifier_sequence_opt->attributes_flags & 1)
            {
                if (p_struct_or_union_specifier->tagtoken)
                {
                    compiler_diagnostic(2, ctx, p_struct_or_union_specifier->first_token, 0U, "'%s' is deprecated", p_struct_or_union_specifier->tagtoken->lexeme);
                }
                else
                {
                    compiler_diagnostic(2, ctx, p_struct_or_union_specifier->first_token, 0U, "deprecated");
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        struct_or_union_specifier_delete(p_struct_or_union_specifier);
        p_struct_or_union_specifier = 0U;
    }
    return p_struct_or_union_specifier;
}

unsigned char  type_is_owner(struct type * p_type);
void naming_convention_struct_member(struct parser_ctx * ctx, struct token * token, struct type * type);
struct expression *constant_expression(struct parser_ctx * ctx, unsigned char   show_error_if_not_constant);
void member_declarator_delete(struct member_declarator * p);

struct member_declarator *member_declarator(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier, struct specifier_qualifier_list * p_specifier_qualifier_list)
{
    struct member_declarator * p_member_declarator;

    p_member_declarator = 0U;
    /*try*/ if (1)
    {
        struct token * p_token_name;

        p_member_declarator = calloc(1, 12U);
        if (p_member_declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_token_name = 0U;
        p_member_declarator->declarator = declarator(ctx, p_specifier_qualifier_list, 0U, 0, &p_token_name);
        if (p_member_declarator->declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_member_declarator->declarator->name_opt = p_token_name;
        p_member_declarator->declarator->specifier_qualifier_list = p_specifier_qualifier_list;
        p_member_declarator->declarator->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
        if (type_is_function(&p_member_declarator->declarator->type))
        {
            struct token * p_token;

            p_token = p_member_declarator->declarator->first_token_opt;
            if (p_token == 0U)
            {
                p_token = ctx->current;
            }
            compiler_diagnostic(1840, ctx, p_token, 0U, "members having a function type are not allowed");
            /*throw*/ goto _CKL0;
        }
        if (type_is_owner(&p_member_declarator->declarator->type))
        {
            p_struct_or_union_specifier->is_owner = 1;
        }
        if (p_member_declarator->declarator->name_opt)
        {
            naming_convention_struct_member(ctx, p_member_declarator->declarator->name_opt, &p_member_declarator->declarator->type);
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 58)
        {
            parser_match(ctx);
            p_member_declarator->constant_expression = constant_expression(ctx, 1);
        }
    }
    /*catch*/ else _CKL0:
    {
        member_declarator_delete(p_member_declarator);
        p_member_declarator = 0U;
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
    if (list->head == 0U)
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
        struct member_declarator * item;

        item = p->head;
        while (item)
        {
            struct member_declarator * next;

            next = item->next;
            item->next = 0U;
            member_declarator_delete(item);
            item = next;
        }
        free(p);
    }
}

struct member_declarator_list *member_declarator_list(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier, struct specifier_qualifier_list * p_specifier_qualifier_list)
{
    struct member_declarator_list * p_member_declarator_list;

    p_member_declarator_list = calloc(1, 8U);
    /*try*/ if (1)
    {
        struct member_declarator * p_member_declarator;

        if (p_member_declarator_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_member_declarator = member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list);
        if (p_member_declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        member_declarator_list_add(p_member_declarator_list, p_member_declarator);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        while (ctx->current->type == 44)
        {
            struct member_declarator * p_member_declarator2;

            parser_match(ctx);
            p_member_declarator2 = member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list);
            if (p_member_declarator2 == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            member_declarator_list_add(p_member_declarator_list, p_member_declarator2);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        member_declarator_list_delete(p_member_declarator_list);
        p_member_declarator_list = 0U;
    }
    return p_member_declarator_list;
}

void member_declaration_list_add(struct member_declaration_list * list, struct member_declaration * p_item)
{
    if (list->head == 0U)
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
    struct member_declaration * item;

    item = p->head;
    while (item)
    {
        struct member_declaration * next;

        next = item->next;
        item->next = 0U;
        member_declaration_delete(item);
        item = next;
    }
}

struct member_declaration *member_declaration(struct parser_ctx * ctx, struct struct_or_union_specifier *);

struct member_declaration_list member_declaration_list(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    struct member_declaration_list  list;
    struct member_declaration * p_member_declaration;

    _cake_zmem(&list, 16);
    p_member_declaration = 0U;
    /*try*/ if (1)
    {
        p_member_declaration = member_declaration(ctx, p_struct_or_union_specifier);
        if (p_member_declaration == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        member_declaration_list_add(&list, p_member_declaration);
        p_member_declaration = 0U;
        while (ctx->current && ctx->current->type != 125)
        {
            p_member_declaration = member_declaration(ctx, p_struct_or_union_specifier);
            if (p_member_declaration == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            member_declaration_list_add(&list, p_member_declaration);
            p_member_declaration = 0U;
        }
    }
    /*catch*/ else _CKL0:
    {
        struct member_declaration_list  __cmp_lt_4;

        member_declaration_list_destroy(&list);
        __cmp_lt_4.first_token = 0;
        __cmp_lt_4.last_token = 0;
        __cmp_lt_4.head = 0;
        __cmp_lt_4.tail = 0;
        list = __cmp_lt_4;
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
    struct member_declaration * p_member_declaration;

    p_member_declaration = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_member_declaration = calloc(1, 24U);
        if (p_member_declaration == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 9058)
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
                if (p_member_declaration->specifier_qualifier_list == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                if (ctx->current == 0U)
                {
                    unexpected_end_of_file(ctx);
                    /*throw*/ goto _CKL0;
                }
                if (ctx->current->type != 59)
                {
                    p_member_declaration->member_declarator_list_opt = member_declarator_list(ctx, p_struct_or_union_specifier, p_member_declaration->specifier_qualifier_list);
                    if (p_member_declaration->member_declarator_list_opt == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                }
                if (parser_match_tk(ctx, 59) != 0)
                {
                    /*throw*/ goto _CKL0;
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        member_declaration_delete(p_member_declaration);
        p_member_declaration = 0U;
    }
    return p_member_declaration;
}

struct member_declarator *find_member_declarator(struct member_declaration_list * list, char * name, int * p_member_index)
{
    struct member_declaration * p_member_declaration;

    if (list->head == 0U)
    {
        return 0U;
    }
    p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator;

        p_member_declarator = 0U;
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
                (*p_member_index)++;
                p_member_declarator = p_member_declarator->next;
            }
        }
        else
        {
            if (p_member_declaration->specifier_qualifier_list && p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct struct_or_union_specifier * p_complete;

                p_complete = get_complete_struct_or_union_specifier(p_member_declaration->specifier_qualifier_list->struct_or_union_specifier);
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
    return 0U;
}

static struct member_declarator *find_member_declarator_by_index_core(struct member_declaration_list * list, int member_index, int * p_count)
{
    struct member_declaration * p_member_declaration;

    if (list->head == 0U)
    {
        return 0U;
    }
    p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator;

        p_member_declarator = 0U;
        if (p_member_declaration->member_declarator_list_opt)
        {
            p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
            while (p_member_declarator)
            {
                if (p_member_declarator->declarator)
                {
                    if (member_index == *p_count)
                    {
                        return p_member_declarator;
                    }
                }
                (*p_count)++;
                p_member_declarator = p_member_declarator->next;
            }
        }
        else
        {
            if (p_member_declaration->specifier_qualifier_list && p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
            {
                struct member_declaration_list * p_member_declaration_list;

                p_member_declaration_list = &p_member_declaration->specifier_qualifier_list->struct_or_union_specifier->member_declaration_list;
                p_member_declarator = find_member_declarator_by_index_core(p_member_declaration_list, member_index, p_count);
                if (p_member_declarator)
                {
                    return p_member_declarator;
                }
            }
        }
        p_member_declaration = p_member_declaration->next;
    }
    return 0U;
}

struct member_declarator *find_member_declarator_by_index(struct member_declaration_list * list, int member_index)
{
    int count;

    count = 0;
    return find_member_declarator_by_index_core(list, member_index, &count);
}

unsigned char  object_is_reference(struct object * p_object);
struct object *object_get_referenced(struct object * p_object);

static struct object *find_object_declarator_by_index_core(struct object * p_object0, struct member_declaration_list * list, int member_index, int * p_count)
{
    struct object * p_object;
    struct object * p_member_object;
    struct member_declaration * p_member_declaration;

    p_object = object_is_reference(p_object0) ? object_get_referenced(p_object0) : p_object0;
    if (list->head == 0U)
    {
        return 0U;
    }
    if (p_object->members == 0U)
    {
        return 0U;
    }
    p_member_object = p_object->members;
    p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator;

        p_member_declarator = 0U;
        if (p_member_declaration->member_declarator_list_opt)
        {
            p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
            while (p_member_declarator)
            {
                if (p_member_declarator->declarator)
                {
                    if (member_index == *p_count)
                    {
                        return p_member_object;
                    }
                }
                (*p_count)++;
                p_member_declarator = p_member_declarator->next;
                if (p_member_object == 0U)
                {
                    return 0U;
                }
                p_member_object = p_member_object->next;
            }
        }
        else
        {
            if (p_member_declaration->specifier_qualifier_list)
            {
                if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                {
                    struct member_declaration_list * p_member_declaration_list;
                    struct object * p_member_object2;

                    p_member_declaration_list = &p_member_declaration->specifier_qualifier_list->struct_or_union_specifier->member_declaration_list;
                    p_member_object2 = find_object_declarator_by_index_core(p_member_object, p_member_declaration_list, member_index, p_count);
                    if (p_member_object2)
                    {
                        return p_member_object2;
                    }
                }
                p_member_object = p_member_object->next;
            }
        }
        p_member_declaration = p_member_declaration->next;
    }
    return 0U;
}

struct object *find_object_declarator_by_index(struct object * p_object, struct member_declaration_list * list, int member_index)
{
    int count;

    count = 0;
    return find_object_declarator_by_index_core(p_object, list, member_index, &count);
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
    if (list->head == 0U)
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
        struct type_specifier_qualifier * item;

        item = p->head;
        while (item)
        {
            struct type_specifier_qualifier * next;

            next = item->next;
            item->next = 0U;
            type_specifier_qualifier_delete(item);
            item = next;
        }
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence);
        free(p);
    }
}

struct specifier_qualifier_list *specifier_qualifier_list(struct parser_ctx * ctx)
{
    struct specifier_qualifier_list * p_specifier_qualifier_list;

    p_specifier_qualifier_list = 0U;
    /*try*/ if (1)
    {
        struct token * p_previous_parser_token;

        if (!first_of_type_specifier_qualifier(ctx))
        {
            compiler_diagnostic(1040, ctx, ctx->current, 0U, "type specifier or qualifier expected");
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_specifier_qualifier_list = calloc(1, 44U);
        if (p_specifier_qualifier_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_specifier_qualifier_list->first_token = ctx->current;
        while (ctx->current != 0U && (first_of_type_specifier_qualifier(ctx)))
        {
            struct type_specifier_qualifier * p_type_specifier_qualifier;

            if (ctx->current->flags & 16)
            {
                if (p_specifier_qualifier_list->type_specifier_flags != 0)
                {
                    break;
                }
            }
            p_type_specifier_qualifier = type_specifier_qualifier(ctx);
            if (p_type_specifier_qualifier == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (p_type_specifier_qualifier->type_specifier)
            {
                if (add_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags, p_type_specifier_qualifier->type_specifier->flags) != 0)
                {
                    type_specifier_qualifier_delete(p_type_specifier_qualifier);
                    /*throw*/ goto _CKL0;
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
                                p_specifier_qualifier_list->typedef_declarator = find_declarator(ctx, p_type_specifier_qualifier->type_specifier->token->lexeme, 0U);
                            }
                        }
                    }
                }
            }
            else
            {
                if (p_type_specifier_qualifier->type_qualifier)
                {
                    p_specifier_qualifier_list->type_qualifier_flags |= p_type_specifier_qualifier->type_qualifier->flags;
                }
            }
            ;
            p_specifier_qualifier_list->p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
            specifier_qualifier_list_add(p_specifier_qualifier_list, p_type_specifier_qualifier);
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        final_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags);
        p_previous_parser_token = previous_parser_token(ctx->current);
        if (p_previous_parser_token == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_specifier_qualifier_list->last_token = p_previous_parser_token;
    }
    /*catch*/ else _CKL0:
    {
        specifier_qualifier_list_delete(p_specifier_qualifier_list);
        p_specifier_qualifier_list = 0U;
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
    struct type_specifier_qualifier * type_specifier_qualifier;

    type_specifier_qualifier = 0U;
    /*try*/ if (1)
    {
        type_specifier_qualifier = calloc(1, 16U);
        if (type_specifier_qualifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (first_of_type_specifier(ctx))
        {
            type_specifier_qualifier->type_specifier = type_specifier(ctx);
            if (type_specifier_qualifier->type_specifier == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            if (first_of_type_qualifier(ctx))
            {
                type_specifier_qualifier->type_qualifier = type_qualifier(ctx);
                if (type_specifier_qualifier->type_qualifier == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            else
            {
                if (first_of_alignment_specifier(ctx))
                {
                    type_specifier_qualifier->alignment_specifier = alignment_specifier(ctx);
                    if (type_specifier_qualifier->alignment_specifier == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                }
                else
                {
                    ;
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        type_specifier_qualifier_delete(type_specifier_qualifier);
        type_specifier_qualifier = 0U;
    }
    return type_specifier_qualifier;
}

int object_equal(struct object * a, struct object * b);

struct enumerator *find_enumerator_by_value(struct enum_specifier * p_enum_specifier, struct object * object)
{
    struct enumerator * p;

    if (p_enum_specifier->enumerator_list.head == 0U)
    {
        return 0U;
    }
    p = p_enum_specifier->enumerator_list.head;
    while (p)
    {
        if (object_equal(&p->value, object))
        {
            return p;
        }
        p = p->next;
    }
    return 0U;
}

struct enum_specifier *enum_specifier_add_ref(struct enum_specifier * p)
{
    p->has_shared_ownership = 1;
    return (struct enum_specifier *)p;
}

void enum_specifier_delete_sink(struct enum_specifier * p)
{
}

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

unsigned char  enum_specifier_has_fixed_underlying_type(struct enum_specifier * p_enum_specifier)
{
    return !!(p_enum_specifier->specifier_qualifier_list != 0U);
}

struct type make_with_type_specifier_flags(int   f);
unsigned char  type_is_integer(struct type * p_type);
struct type type_make_int();
void naming_convention_enum_tag(struct parser_ctx * ctx, struct token * token);
struct enumerator_list enumerator_list(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier);

struct enum_specifier *enum_specifier(struct parser_ctx * ctx)
{
    struct enum_specifier * p_enum_specifier;

    p_enum_specifier = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_enum_specifier = calloc(1, 292U);
        if (p_enum_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_enum_specifier->first_token = ctx->current;
        if (parser_match_tk(ctx, 9012) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        p_enum_specifier->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 8996)
        {
            snprintf(p_enum_specifier->tag_name, 200U, "%s", ctx->current->lexeme);
            p_enum_specifier->tag_token = ctx->current;
            parser_match(ctx);
        }
        else
        {
            snprintf(p_enum_specifier->tag_name, 200U, "_anonymous_struct_%d", s_anonymous_struct_count);
            s_anonymous_struct_count++;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 58)
        {
            if (!ctx->inside_generic_association)
            {
                parser_match(ctx);
                p_enum_specifier->specifier_qualifier_list = specifier_qualifier_list(ctx);
                if (p_enum_specifier->specifier_qualifier_list == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_enum_specifier->type = make_with_type_specifier_flags(p_enum_specifier->specifier_qualifier_list->type_specifier_flags);
                if (!type_is_integer(&p_enum_specifier->type))
                {
                    compiler_diagnostic(1850, ctx, p_enum_specifier->specifier_qualifier_list->first_token, 0U, "expected an integer type");
                    /*throw*/ goto _CKL0;
                }
            }
            else
            {
            }
        }
        else
        {
            p_enum_specifier->type = type_make_int();
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 123)
        {
            struct hash_item_set  item;

            if (p_enum_specifier->tag_token)
            {
                naming_convention_enum_tag(ctx, p_enum_specifier->tag_token);
            }
            p_enum_specifier->complete_enum_specifier2 = p_enum_specifier;
            if (parser_match_tk(ctx, 123) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            p_enum_specifier->enumerator_list = enumerator_list(ctx, p_enum_specifier);
            if (p_enum_specifier->enumerator_list.head == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            if (ctx->current->type == 44)
            {
                parser_match(ctx);
            }
            if (parser_match_tk(ctx, 125) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            _cake_zmem(&item, 32);
            item.p_enum_specifier = enum_specifier_add_ref(p_enum_specifier);
            hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, &item);
            p_enum_specifier->complete_enum_specifier2 = p_enum_specifier;
            hash_item_set_destroy(&item);
        }
        else
        {
            struct enum_specifier * p_existing_enum_specifier;

            p_existing_enum_specifier = find_enum_specifier(ctx, p_enum_specifier->tag_token->lexeme);
            if (p_existing_enum_specifier)
            {
                p_enum_specifier->complete_enum_specifier2 = p_existing_enum_specifier;
            }
            else
            {
                struct hash_item_set  item;

                _cake_zmem(&item, 32);
                item.p_enum_specifier = enum_specifier_add_ref(p_enum_specifier);
                hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, &item);
                p_enum_specifier->complete_enum_specifier2 = p_enum_specifier;
                hash_item_set_destroy(&item);
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        enum_specifier_delete(p_enum_specifier);
        p_enum_specifier = 0U;
    }
    return p_enum_specifier;
}

void enumerator_list_add(struct enumerator_list * list, struct enumerator * p_item)
{
    if (list->head == 0U)
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
    struct enumerator * item;

    item = p->head;
    while (item)
    {
        struct enumerator * next;

        next = item->next;
        item->next = 0U;
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
    struct object  next_enumerator_value;
    struct enumerator_list  enumeratorlist;
    struct enumerator * p_enumerator;

    next_enumerator_value = object_make_signed_int(0);
    if (p_enum_specifier->specifier_qualifier_list)
    {
        int   vt;

        vt = type_specifier_to_object_type(p_enum_specifier->specifier_qualifier_list->type_specifier_flags);
        next_enumerator_value = object_cast(vt, &next_enumerator_value);
    }
    _cake_zmem(&enumeratorlist, 8);
    p_enumerator = 0U;
    /*try*/ if (1)
    {
        p_enumerator = enumerator(ctx, p_enum_specifier, &next_enumerator_value);
        if (p_enumerator == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        enumerator_list_add(&enumeratorlist, p_enumerator);
        while (ctx->current != 0U && ctx->current->type == 44)
        {
            parser_match(ctx);
            if (ctx->current && ctx->current->type != 125)
            {
                p_enumerator = enumerator(ctx, p_enum_specifier, &next_enumerator_value);
                if (p_enumerator == 0U)
                {
                    /*throw*/ goto _CKL1;
                }
                enumerator_list_add(&enumeratorlist, p_enumerator);
            }
        }
    }
    /*catch*/ else _CKL1:
    {
        enumerator_list_destroy(&enumeratorlist);
        enumeratorlist.head = 0U;
        enumeratorlist.tail = 0U;
    }
    return enumeratorlist;
}

struct enumerator *enumerator_add_ref(struct enumerator * p)
{
    p->has_shared_ownership = 1;
    return (struct enumerator *)p;
}

void enumerator_sink(struct enumerator * p)
{
}

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
    struct enumerator * p_enumerator;

    p_enumerator = 0U;
    /*try*/ if (1)
    {
        struct token * name;
        struct hash_item_set  item;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_enumerator = calloc(1, 120U);
        if (p_enumerator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_enumerator->enum_specifier = p_enum_specifier;
        name = ctx->current;
        naming_convention_enumerator(ctx, name);
        if (parser_match_tk(ctx, 8996) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        p_enumerator->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        p_enumerator->token = name;
        _cake_zmem(&item, 32);
        item.p_enumerator = enumerator_add_ref(p_enumerator);
        hashmap_set(&ctx->scopes.tail->variables, p_enumerator->token->lexeme, &item);
        hash_item_set_destroy(&item);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 61)
        {
            parser_match(ctx);
            ;
            p_enumerator->constant_expression_opt = constant_expression(ctx, 1);
            if (p_enumerator->constant_expression_opt == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (enum_specifier_has_fixed_underlying_type(p_enum_specifier))
            {
            }
            else
            {
            }
            p_enumerator->value = p_enumerator->constant_expression_opt->object;
            *p_next_enumerator_value = *object_get_referenced(&p_enumerator->value);
            if (object_increment_value(p_next_enumerator_value) != 0)
            {
            }
        }
        else
        {
            p_enumerator->value = *p_next_enumerator_value;
            if (object_increment_value(p_next_enumerator_value) != 0)
            {
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        enumerator_delete(p_enumerator);
        p_enumerator = 0U;
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
    struct alignment_specifier * alignment_specifier;

    alignment_specifier = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        alignment_specifier = calloc(1, 12U);
        if (alignment_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        alignment_specifier->token = ctx->current;
        if (parser_match_tk(ctx, 9043) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (first_of_type_name(ctx))
        {
            alignment_specifier->type_name = type_name(ctx);
            if (alignment_specifier->type_name == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            alignment_specifier->constant_expression = constant_expression(ctx, 1);
            if (alignment_specifier->constant_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        if (parser_match_tk(ctx, 41) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
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
    struct atomic_type_specifier * p_atomic_type_specifier;

    p_atomic_type_specifier = 0U;
    /*try*/ if (1)
    {
        struct type_name * ptemp;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_atomic_type_specifier = calloc(1, 8U);
        if (p_atomic_type_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_atomic_type_specifier->token = ctx->current;
        if (parser_match_tk(ctx, 9045) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        ptemp = type_name(ctx);
        if (ptemp == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_atomic_type_specifier->type_name = ptemp;
        if (parser_match_tk(ctx, 41) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        atomic_type_specifier_delete(p_atomic_type_specifier);
        p_atomic_type_specifier = 0U;
    }
    return p_atomic_type_specifier;
}

struct type_qualifier *type_qualifier(struct parser_ctx * ctx)
{
    struct type_qualifier * p_type_qualifier;

    if (ctx->current == 0U)
    {
        unexpected_end_of_file(ctx);
        return 0U;
    }
    p_type_qualifier = calloc(1, 12U);
    if (p_type_qualifier == 0U)
    {
        return 0U;
    }
    /*switch*/
    {
        register int   _R5 = ctx->current->type;
        if (_R5 == 9004) goto _CKL3; /*case 9004*/
        if (_R5 == 9026) goto _CKL4; /*case 9026*/
        if (_R5 == 9041) goto _CKL5; /*case 9041*/
        if (_R5 == 9045) goto _CKL6; /*case 9045*/
        goto /*default*/ _CKL7;

        {
            /*case 9004*/ _CKL3:
            p_type_qualifier->flags = 1;
            /*break*/ goto _CKL2;

            /*case 9026*/ _CKL4:
            p_type_qualifier->flags = 2;
            /*break*/ goto _CKL2;

            /*case 9041*/ _CKL5:
            p_type_qualifier->flags = 4;
            /*break*/ goto _CKL2;

            /*case 9045*/ _CKL6:
            p_type_qualifier->flags = 8;
            /*break*/ goto _CKL2;

            /*default*/ _CKL7:
            /*break*/ goto _CKL2;

        }
        _CKL2:;
    }
    if (ctx->options.ownership_enabled)
    {
        /*switch*/
        {
            register int   _R6 = ctx->current->type;
            if (_R6 == 9068) goto _CKL10; /*case 9068*/
            if (_R6 == 9069) goto _CKL11; /*case 9069*/
            if (_R6 == 9067) goto _CKL12; /*case 9067*/
            if (_R6 == 9070) goto _CKL13; /*case 9070*/
            goto /*default*/ _CKL14;

            {
                /*case 9068*/ _CKL10:
                p_type_qualifier->flags = 256;
                /*break*/ goto _CKL9;

                /*case 9069*/ _CKL11:
                p_type_qualifier->flags = 128;
                /*break*/ goto _CKL9;

                /*case 9067*/ _CKL12:
                p_type_qualifier->flags = 16;
                /*break*/ goto _CKL9;

                /*case 9070*/ _CKL13:
                p_type_qualifier->flags = 32;
                /*break*/ goto _CKL9;

                /*default*/ _CKL14:
                /*break*/ goto _CKL9;

            }
            _CKL9:;
        }
    }
    if (ctx->options.null_checks_enabled)
    {
        /*switch*/
        {
            register int   _R7 = ctx->current->type;
            if (_R7 == 9071) goto _CKL17; /*case 9071*/
            goto /*default*/ _CKL18;

            {
                /*case 9071*/ _CKL17:
                p_type_qualifier->flags = 64;
                /*break*/ goto _CKL16;

                /*default*/ _CKL18:
                /*break*/ goto _CKL16;

            }
            _CKL16:;
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
    return 0U;
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
    struct function_specifier * p_function_specifier;

    p_function_specifier = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_function_specifier = calloc(1, 8U);
        if (p_function_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 9057)
        {
            compiler_diagnostic(10, ctx, ctx->current, 0U, "_Noreturn is deprecated use attributes");
        }
        if (ctx->current->type == 9018)
        {
            p_function_specifier->flags |= 1;
        }
        p_function_specifier->token = ctx->current;
        parser_match(ctx);
    }
    /*catch*/ else _CKL0:
    {
        function_specifier_delete(p_function_specifier);
        p_function_specifier = 0U;
    }
    return p_function_specifier;
}

struct declarator *declarator_add_ref(struct declarator * p)
{
    p->has_shared_ownership = 1;
    return (struct declarator *)p;
}

void declarator_sink(struct declarator * p)
{
}

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
    struct declarator * p_declarator;

    p_declarator = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_declarator = calloc(1, 216U);
        if (p_declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_declarator->first_token_opt = ctx->current;
        p_declarator->pointer = pointer_opt(ctx);
        p_declarator->direct_declarator = direct_declarator(ctx, p_specifier_qualifier_list_opt, p_declaration_specifiers_opt, abstract_acceptable, pp_token_name_opt);
        if (p_declarator->direct_declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (pp_token_name_opt && *pp_token_name_opt)
        {
            free(p_declarator->object.debug_name);
            p_declarator->object.debug_name = strdup((*pp_token_name_opt)->lexeme);
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current != p_declarator->first_token_opt)
        {
            p_declarator->last_token_opt = previous_parser_token(ctx->current);
        }
        else
        {
            p_declarator->last_token_opt = p_declarator->first_token_opt;
            p_declarator->first_token_opt = 0U;
        }
    }
    /*catch*/ else _CKL0:
    {
        declarator_delete(p_declarator);
        p_declarator = 0U;
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
    return 0U;
}

unsigned char  declarator_is_function(struct declarator * p_declarator)
{
    return !!((p_declarator->direct_declarator && p_declarator->direct_declarator->function_declarator != 0U));
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
    struct direct_declarator * p_direct_declarator;

    p_direct_declarator = 0U;
    /*try*/ if (1)
    {
        struct token * p_token_ahead;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_direct_declarator = calloc(1, 24U);
        if (p_direct_declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_token_ahead = parser_look_ahead(ctx);
        if (p_token_ahead == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 9047 || ctx->current->type == 9048 || ctx->current->type == 9046)
        {
            p_direct_declarator->p_calling_convention = ctx->current;
            parser_match(ctx);
        }
        if (ctx->current->type == 8996)
        {
            p_direct_declarator->name_opt = ctx->current;
            if (pp_token_name_opt != 0U)
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
                struct token * ahead;

                ahead = parser_look_ahead(ctx);
                if (ahead == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                if (!first_of_type_specifier_token(ctx, p_token_ahead) && !first_of_type_qualifier_token(p_token_ahead) && ahead->type != 41 && ahead->type != 11822)
                {
                    struct declarator * p_declarator_temp;

                    parser_match(ctx);
                    p_declarator_temp = declarator(ctx, p_specifier_qualifier_list, p_declaration_specifiers, abstract_acceptable, pp_token_name_opt);
                    if (p_declarator_temp == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    p_direct_declarator->declarator = p_declarator_temp;
                    parser_match(ctx);
                }
            }
        }
        while (ctx->current != 0U && (ctx->current->type == 91 || ctx->current->type == 40))
        {
            struct direct_declarator * p_direct_declarator2;

            p_direct_declarator2 = calloc(1, 24U);
            if (p_direct_declarator2 == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (ctx->current->type == 91)
            {
                p_direct_declarator2->array_declarator = array_declarator(p_direct_declarator, ctx);
                p_direct_declarator = 0U;
                if (p_direct_declarator2->array_declarator == 0U)
                {
                    direct_declarator_delete(p_direct_declarator2);
                    /*throw*/ goto _CKL0;
                }
            }
            else
            {
                p_direct_declarator2->function_declarator = function_declarator(p_direct_declarator, ctx);
                p_direct_declarator = 0U;
                if (p_direct_declarator2->function_declarator == 0U)
                {
                    direct_declarator_delete(p_direct_declarator2);
                    /*throw*/ goto _CKL0;
                }
            }
            p_direct_declarator = p_direct_declarator2;
        }
    }
    /*catch*/ else _CKL0:
    {
        direct_declarator_delete(p_direct_declarator);
        p_direct_declarator = 0U;
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
    struct array_declarator * p_array_declarator;

    p_array_declarator = 0U;
    /*try*/ if (1)
    {
        unsigned char   has_static;

        p_array_declarator = calloc(1, 24U);
        if (p_array_declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_array_declarator->direct_declarator = p_direct_declarator;
        p_direct_declarator = 0U;
        if (parser_match_tk(ctx, 91) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        has_static = 0;
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
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (!has_static)
        {
            if (ctx->current->type == 9032)
            {
                parser_match(ctx);
                has_static = 1;
            }
        }
        if (has_static)
        {
            unsigned char   evaluation_is_disabled;

            evaluation_is_disabled = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = 0;
            p_array_declarator->assignment_expression = assignment_expression(ctx);
            ctx->evaluation_is_disabled = evaluation_is_disabled;
            if (p_array_declarator->assignment_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            if (ctx->current->type == 42)
            {
                parser_match(ctx);
            }
            else
            {
                if (ctx->current->type != 93)
                {
                    unsigned char   evaluation_is_disabled;

                    evaluation_is_disabled = ctx->evaluation_is_disabled;
                    ctx->evaluation_is_disabled = 0;
                    p_array_declarator->assignment_expression = assignment_expression(ctx);
                    ctx->evaluation_is_disabled = evaluation_is_disabled;
                    if (p_array_declarator->assignment_expression == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                }
                else
                {
                }
            }
        }
        if (parser_match_tk(ctx, 93) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        array_declarator_delete(p_array_declarator);
        p_array_declarator = 0U;
    }
    direct_declarator_delete(p_direct_declarator);
    return p_array_declarator;
}

struct parameter_type_list *parameter_type_list(struct parser_ctx * ctx);

struct function_declarator *function_declarator(struct direct_declarator * p_direct_declarator_arg, struct parser_ctx * ctx)
{
    struct direct_declarator * p_direct_declarator;
    struct function_declarator * p_function_declarator;

    p_direct_declarator = p_direct_declarator_arg;
    p_function_declarator = calloc(1, 44U);
    /*try*/ if (1)
    {
        if (p_function_declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_function_declarator->direct_declarator = p_direct_declarator;
        p_direct_declarator = 0U;
        p_function_declarator->parameters_scope.scope_level = ctx->scopes.tail->scope_level + 1;
        p_function_declarator->parameters_scope.variables.capacity = 5;
        p_function_declarator->parameters_scope.tags.capacity = 1;
        if (parser_match_tk(ctx, 40) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type != 41)
        {
            scope_list_push(&ctx->scopes, &p_function_declarator->parameters_scope);
            p_function_declarator->parameter_type_list_opt = parameter_type_list(ctx);
            scope_list_pop(&ctx->scopes);
            if (p_function_declarator->parameter_type_list_opt == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        if (parser_match_tk(ctx, 41) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        function_declarator_delete(p_function_declarator);
        p_function_declarator = 0U;
    }
    direct_declarator_delete(p_direct_declarator);
    return p_function_declarator;
}

void pointer_delete(struct pointer * p)
{
    if (p)
    {
        struct pointer * item;

        attribute_specifier_sequence_delete(p->attribute_specifier_sequence_opt);
        type_qualifier_list_delete(p->type_qualifier_list_opt);
        item = p->pointer;
        while (item)
        {
            struct pointer * next;

            next = item->pointer;
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
    struct pointer * p;
    struct pointer * p_pointer;

    p = 0U;
    p_pointer = 0U;
    /*try*/ if (1)
    {
        struct token * calling_convention;
        struct token * ahead;

        calling_convention = 0U;
        ahead = parser_look_ahead(ctx);
        if (ahead != 0U && ahead->type == 42)
        {
            if (ctx->current->type == 9047 || ctx->current->type == 9048 || ctx->current->type == 9046)
            {
                calling_convention = ctx->current;
                parser_match(ctx);
            }
        }
        while (ctx->current != 0U && ctx->current->type == 42)
        {
            p_pointer = calloc(1, 16U);
            if (p_pointer == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_pointer->calling_convention = calling_convention;
            calling_convention = 0U;
            p = p_pointer;
            parser_match(ctx);
            p_pointer->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
            if (first_of_type_qualifier(ctx))
            {
                ;
                if (ctx->current->type == 9070)
                {
                    compiler_diagnostic(640, ctx, ctx->current, 0U, "invalid qualifier for pointer");
                }
                else
                {
                    p_pointer->type_qualifier_list_opt = type_qualifier_list(ctx);
                }
            }
            while (ctx->current != 0U && ctx->current->type == 42)
            {
                p_pointer->pointer = pointer_opt(ctx);
                if (p_pointer->pointer == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    return p;
}

void type_qualifier_list_add(struct type_qualifier_list * list, struct type_qualifier * p_item)
{
    if (list->head == 0U)
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
        struct type_qualifier * item;

        item = p->head;
        while (item)
        {
            struct type_qualifier * next;

            next = item->next;
            item->next = 0U;
            free(item);
            item = next;
        }
        free(p);
    }
}

struct type_qualifier_list *type_qualifier_list(struct parser_ctx * ctx)
{
    struct type_qualifier_list * p_type_qualifier_list;
    struct type_qualifier * p_type_qualifier;

    p_type_qualifier_list = 0U;
    p_type_qualifier = 0U;
    /*try*/ if (1)
    {
        p_type_qualifier_list = calloc(1, 12U);
        if (p_type_qualifier_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_type_qualifier = type_qualifier(ctx);
        if (p_type_qualifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_type_qualifier_list->flags |= p_type_qualifier->flags;
        type_qualifier_list_add(p_type_qualifier_list, p_type_qualifier);
        p_type_qualifier = 0U;
        while (ctx->current != 0U && first_of_type_qualifier(ctx))
        {
            p_type_qualifier = type_qualifier(ctx);
            if (p_type_qualifier == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_type_qualifier_list->flags |= p_type_qualifier->flags;
            type_qualifier_list_add(p_type_qualifier_list, p_type_qualifier);
            p_type_qualifier = 0U;
        }
    }
    /*catch*/ else _CKL0:
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
    struct parameter_type_list * p_parameter_type_list;

    if (ctx->current == 0U)
    {
        return 0U;
    }
    p_parameter_type_list = calloc(1, 8U);
    /*try*/ if (1)
    {
        if (p_parameter_type_list == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_parameter_type_list->parameter_list = parameter_list(ctx);
        if (p_parameter_type_list->parameter_list == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        if (p_parameter_type_list->parameter_list->head == p_parameter_type_list->parameter_list->tail)
        {
            if (type_is_void(&p_parameter_type_list->parameter_list->head->declarator->type))
            {
                p_parameter_type_list->is_void = 1;
            }
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL1;
        }
        if (ctx->current->type == 11822)
        {
            parser_match(ctx);
            p_parameter_type_list->is_var_args = 1;
        }
    }
    /*catch*/ else _CKL1:
    {
        parameter_type_list_delete(p_parameter_type_list);
        p_parameter_type_list = 0U;
    }
    return p_parameter_type_list;
}

void parameter_list_add(struct parameter_list * list, struct parameter_declaration * p_item)
{
    if (list->head == 0U)
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
        struct parameter_declaration * item;

        item = p->head;
        while (item)
        {
            struct parameter_declaration * next;

            next = item->next;
            item->next = 0U;
            parameter_declaration_delete(item);
            item = next;
        }
        free(p);
    }
}

struct parameter_declaration *parameter_declaration(struct parser_ctx * ctx);

struct parameter_list *parameter_list(struct parser_ctx * ctx)
{
    struct parameter_list * p_parameter_list;
    struct parameter_declaration * p_parameter_declaration;

    p_parameter_list = 0U;
    p_parameter_declaration = 0U;
    /*try*/ if (1)
    {
        p_parameter_list = calloc(1, 8U);
        if (p_parameter_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_parameter_declaration = parameter_declaration(ctx);
        if (p_parameter_declaration == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        parameter_list_add(p_parameter_list, p_parameter_declaration);
        p_parameter_declaration = 0U;
        while (ctx->current != 0U && ctx->current->type == 44)
        {
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            if (ctx->current->type == 11822)
            {
                break;
            }
            p_parameter_declaration = parameter_declaration(ctx);
            if (p_parameter_declaration == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            parameter_list_add(p_parameter_list, p_parameter_declaration);
            p_parameter_declaration = 0U;
        }
    }
    /*catch*/ else _CKL0:
    {
        parameter_list_delete(p_parameter_list);
        p_parameter_list = 0U;
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

void object_set_any(struct object * p_object);
void naming_convention_parameter(struct parser_ctx * ctx, struct token * token, struct type * type);

struct parameter_declaration *parameter_declaration(struct parser_ctx * ctx)
{
    struct parameter_declaration * p_parameter_declaration;

    p_parameter_declaration = calloc(1, 16U);
    /*try*/ if (1)
    {
        struct declaration_specifiers * p_declaration_specifiers;
        struct token * p_token_name;

        if (p_parameter_declaration == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_parameter_declaration->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        p_declaration_specifiers = declaration_specifiers(ctx, 6144);
        if (p_declaration_specifiers == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (p_parameter_declaration->attribute_specifier_sequence_opt)
        {
            if (p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags & 128)
            {
                p_declaration_specifiers->type_qualifier_flags |= 256;
            }
            else
            {
                if (p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags & 256)
                {
                    p_declaration_specifiers->type_qualifier_flags |= 128;
                }
            }
        }
        p_parameter_declaration->declaration_specifiers = p_declaration_specifiers;
        p_token_name = 0U;
        p_parameter_declaration->declarator = declarator(ctx, 0U, p_parameter_declaration->declaration_specifiers, 1, &p_token_name);
        if (p_parameter_declaration->declarator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_parameter_declaration->declarator->name_opt = p_token_name;
        p_parameter_declaration->declarator->declaration_specifiers = p_parameter_declaration->declaration_specifiers;
        p_parameter_declaration->declarator->type = make_type_using_declarator(ctx, p_parameter_declaration->declarator);
        if (p_parameter_declaration->declarator->type.storage_class_specifier_flags & 1)
        {
        }
        else
        {
            int er;

            er = make_object(&p_parameter_declaration->declarator->type, &p_parameter_declaration->declarator->object);
            if (er != 0)
            {
                /*throw*/ goto _CKL0;
            }
        }
        if (p_parameter_declaration->declarator->name_opt)
        {
            free(p_parameter_declaration->declarator->object.debug_name);
            p_parameter_declaration->declarator->object.debug_name = strdup(p_parameter_declaration->declarator->name_opt->lexeme);
        }
        object_set_any(&p_parameter_declaration->declarator->object);
        if (p_parameter_declaration->attribute_specifier_sequence_opt)
        {
            p_parameter_declaration->declarator->type.attributes_flags |= p_parameter_declaration->attribute_specifier_sequence_opt->attributes_flags;
        }
        p_parameter_declaration->declarator->type.storage_class_specifier_flags |= 2048;
        if (p_parameter_declaration->declarator->name_opt)
        {
            naming_convention_parameter(ctx, p_parameter_declaration->declarator->name_opt, &p_parameter_declaration->declarator->type);
        }
        if (p_parameter_declaration->declarator->name_opt)
        {
            struct hash_item_set  item;

            _cake_zmem(&item, 32);
            item.p_declarator = declarator_add_ref(p_parameter_declaration->declarator);
            ;
            hashmap_set(&ctx->scopes.tail->variables, p_parameter_declaration->declarator->name_opt->lexeme, &item);
            hash_item_set_destroy(&item);
        }
    }
    /*catch*/ else _CKL0:
    {
        parameter_declaration_delete(p_parameter_declaration);
        p_parameter_declaration = 0U;
    }
    return p_parameter_declaration;
}

struct specifier_qualifier_list *copy(struct declaration_specifiers * p_declaration_specifiers)
{
    struct specifier_qualifier_list * p_specifier_qualifier_list;

    p_specifier_qualifier_list = calloc(1, 44U);
    /*try*/ if (1)
    {
        struct declaration_specifier * p_declaration_specifier;

        if (p_specifier_qualifier_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_specifier_qualifier_list->type_qualifier_flags = p_declaration_specifiers->type_qualifier_flags;
        p_specifier_qualifier_list->type_specifier_flags = p_declaration_specifiers->type_specifier_flags;
        p_declaration_specifier = p_declaration_specifiers->head;
        while (p_declaration_specifier)
        {
            if (p_declaration_specifier->type_specifier_qualifier)
            {
                struct type_specifier_qualifier * p_specifier_qualifier;

                p_specifier_qualifier = calloc(1, 16U);
                if (p_specifier_qualifier == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                {
                    struct type_qualifier * p_type_qualifier;

                    p_type_qualifier = calloc(1, 12U);
                    if (p_type_qualifier == 0U)
                    {
                        type_specifier_qualifier_delete(p_specifier_qualifier);
                        /*throw*/ goto _CKL0;
                    }
                    p_type_qualifier->flags = p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;
                    p_type_qualifier->token = p_declaration_specifier->type_specifier_qualifier->type_qualifier->token;
                    p_specifier_qualifier->type_qualifier = p_type_qualifier;
                }
                else
                {
                    if (p_declaration_specifier->type_specifier_qualifier->type_specifier)
                    {
                        struct type_specifier * p_type_specifier;

                        p_type_specifier = calloc(1, 28U);
                        if (p_type_specifier == 0U)
                        {
                            type_specifier_qualifier_delete(p_specifier_qualifier);
                            /*throw*/ goto _CKL0;
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
    /*catch*/ else _CKL0:
    {
        specifier_qualifier_list_delete(p_specifier_qualifier_list);
        p_specifier_qualifier_list = 0U;
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
    if (p_direct_declarator->name_opt && !is_abstract)
    {
        ss_fprintf(ss, "%s", p_direct_declarator->name_opt->lexeme);
    }
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration * p_parameter_declaration;

        ;
        print_direct_declarator(ss, p_direct_declarator->function_declarator->direct_declarator, is_abstract);
        ss_fprintf(ss, "(");
        p_parameter_declaration = p_direct_declarator->function_declarator->parameter_type_list_opt ? p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head : 0U;
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
    unsigned char   first;

    first = 1;
    if (p_declarator->pointer)
    {
        struct pointer * p;

        p = p_declarator->pointer;
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
    unsigned char   first;

    first = 1;
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
    struct type_name * p_type_name;

    if (ctx->current == 0U)
    {
        return 0U;
    }
    p_type_name = calloc(1, 76U);
    /*try*/ if (1)
    {
        struct specifier_qualifier_list * p_specifier_qualifier_list;
        struct declarator * p_declarator;

        if (p_type_name == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_type_name->first_token = ctx->current;
        p_specifier_qualifier_list = specifier_qualifier_list(ctx);
        if (p_specifier_qualifier_list == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_type_name->specifier_qualifier_list = p_specifier_qualifier_list;
        p_declarator = declarator(ctx, p_type_name->specifier_qualifier_list, 0U, 1, 0U);
        if (p_declarator == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_type_name->abstract_declarator = p_declarator;
        p_type_name->abstract_declarator->specifier_qualifier_list = p_type_name->specifier_qualifier_list;
        p_type_name->abstract_declarator->type = make_type_using_declarator(ctx, p_type_name->abstract_declarator);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL1;
        }
        if (ctx->current->prev == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_type_name->last_token = ctx->current->prev;
        p_type_name->type = type_dup(&p_type_name->abstract_declarator->type);
    }
    /*catch*/ else _CKL1:
    {
        type_name_delete(p_type_name);
        p_type_name = 0U;
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
    struct braced_initializer * p_bracket_initializer_list;

    p_bracket_initializer_list = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_bracket_initializer_list = calloc(1, 12U);
        if (p_bracket_initializer_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_bracket_initializer_list->first_token = ctx->current;
        if (parser_match_tk(ctx, 123) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type != 125)
        {
            p_bracket_initializer_list->initializer_list = initializer_list(ctx);
        }
        if (parser_match_tk(ctx, 125) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        braced_initializer_delete(p_bracket_initializer_list);
        p_bracket_initializer_list = 0U;
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
    struct initializer * p_initializer;

    p_initializer = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_initializer = calloc(1, 12U);
        if (p_initializer == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_initializer->first_token = ctx->current;
        if (ctx->current->type == 123)
        {
            p_initializer->braced_initializer = braced_initializer(ctx);
            if (p_initializer->braced_initializer == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            p_initializer->assignment_expression = assignment_expression(ctx);
            if (p_initializer->assignment_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        initializer_delete(p_initializer);
        p_initializer = 0U;
    }
    return p_initializer;
}

void defer_list_add(struct defer_list * list, struct defer_list_item * p_item)
{
    if (list->head == 0U)
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
    struct defer_list_item * item;

    item = p->head;
    while (item)
    {
        struct defer_list_item * next;

        next = item->next;
        item->next = 0U;
        free(item);
        item = next;
    }
    free(p);
}

void initializer_list_add(struct initializer_list * list, struct initializer_list_item * p_item)
{
    if (list->head == 0U)
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
        struct initializer_list_item * item;

        item = p->head;
        while (item)
        {
            struct initializer_list_item * next;

            next = item->next;
            item->next = 0U;
            initializer_list_item_delete(item);
            item = next;
        }
        free(p);
    }
}

struct designation *designation(struct parser_ctx * ctx);

struct initializer_list *initializer_list(struct parser_ctx * ctx)
{
    struct initializer_list * p_initializer_list;

    p_initializer_list = 0U;
    /*try*/ if (1)
    {
        struct initializer_list_item * p_initializer_list_item;
        struct designation * p_designation;
        struct initializer * p_initializer;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            return 0U;
        }
        p_initializer_list = calloc(1, 16U);
        if (p_initializer_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_initializer_list->first_token = ctx->current;
        p_initializer_list_item = calloc(1, 12U);
        if (p_initializer_list_item == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_designation = 0U;
        if (first_of_designator(ctx))
        {
            p_designation = designation(ctx);
            if (p_designation == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_initializer_list_item->designation = p_designation;
        }
        p_initializer = initializer(ctx);
        if (p_initializer == 0U)
        {
            designation_delete(p_designation);
            /*throw*/ goto _CKL0;
        }
        p_initializer_list_item->initializer = p_initializer;
        initializer_list_add(p_initializer_list, p_initializer_list_item);
        p_initializer_list_item = 0U;
        while (ctx->current != 0U && ctx->current->type == 44)
        {
            struct designation * p_designation2;
            struct initializer * p_initializer2;

            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            if (ctx->current->type == 125)
            {
                break;
            }
            p_initializer_list_item = calloc(1, 12U);
            if (p_initializer_list_item == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_designation2 = 0U;
            if (first_of_designator(ctx))
            {
                p_designation2 = designation(ctx);
                if (p_designation2 == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            p_initializer_list_item->designation = p_designation2;
            p_initializer2 = initializer(ctx);
            if (p_initializer2 == 0U)
            {
                designation_delete(p_designation2);
                /*throw*/ goto _CKL0;
            }
            p_initializer_list_item->initializer = p_initializer2;
            initializer_list_add(p_initializer_list, p_initializer_list_item);
            p_initializer_list_item = 0U;
            p_initializer_list->size++;
        }
    }
    /*catch*/ else _CKL0:
    {
        initializer_list_delete(p_initializer_list);
        p_initializer_list = 0U;
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
    struct designation * p_designation;

    p_designation = 0U;
    /*try*/ if (1)
    {
        struct designator_list * p_designator_list;

        p_designation = calloc(1, 8U);
        if (p_designation == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_designator_list = designator_list(ctx);
        if (p_designator_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_designation->designator_list = p_designator_list;
        if (parser_match_tk(ctx, 61) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        designation_delete(p_designation);
        p_designation = 0U;
    }
    return p_designation;
}

void designator_list_add(struct designator_list * list, struct designator * p_item)
{
    if (list->head == 0U)
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
        struct designator * item;

        item = p->head;
        while (item)
        {
            struct designator * next;

            next = item->next;
            item->next = 0U;
            designator_delete(item);
            item = next;
        }
        free(p);
    }
}

struct designator *designator(struct parser_ctx * ctx);

struct designator_list *designator_list(struct parser_ctx * ctx)
{
    struct designator_list * p_designator_list;
    struct designator * p_designator;

    p_designator_list = 0U;
    p_designator = 0U;
    /*try*/ if (1)
    {
        p_designator_list = calloc(1, 8U);
        if (p_designator_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_designator = designator(ctx);
        if (p_designator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        designator_list_add(p_designator_list, p_designator);
        p_designator = 0U;
        while (ctx->current != 0U && first_of_designator(ctx))
        {
            p_designator = designator(ctx);
            if (p_designator == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            designator_list_add(p_designator_list, p_designator);
            p_designator = 0U;
        }
    }
    /*catch*/ else _CKL0:
    {
        designator_list_delete(p_designator_list);
        p_designator_list = 0U;
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
    struct designator * p_designator;

    p_designator = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_designator = calloc(1, 12U);
        if (p_designator == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 91)
        {
            if (parser_match_tk(ctx, 91) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            p_designator->constant_expression_opt = constant_expression(ctx, 1);
            if (parser_match_tk(ctx, 93) != 0)
            {
                /*throw*/ goto _CKL0;
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
                    /*throw*/ goto _CKL0;
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        designator_delete(p_designator);
        p_designator = 0U;
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
int __cdecl atoi(char * _String);

void execute_pragma(struct parser_ctx * ctx, struct pragma_declaration * p_pragma, unsigned char   on_flow_analysis)
{
    struct token * p_pragma_token;

    p_pragma_token = p_pragma->first_token;
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
            if (ctx->options.diagnostic_stack.top_index < 10U)
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
                    unsigned char   is_error;
                    unsigned char   is_warning;
                    unsigned char   is_note;

                    is_error = !!(strcmp(p_pragma_token->lexeme, "error") == 0);
                    is_warning = !!(strcmp(p_pragma_token->lexeme, "warning") == 0);
                    is_note = !!(strcmp(p_pragma_token->lexeme, "note") == 0);
                    p_pragma_token = pragma_match(p_pragma_token);
                    if (p_pragma_token && p_pragma_token->type == 130)
                    {
                        unsigned long long w;

                        w = get_warning_bit_mask(p_pragma_token->lexeme + 1);
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                        ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
                        if (is_error)
                        {
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors |= w;
                        }
                        else
                        {
                            if (is_warning)
                            {
                                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
                            }
                            else
                            {
                                if (is_note)
                                {
                                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes |= w;
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
                            int   id;

                            id = get_warning(p_pragma_token->lexeme + 1);
                            if ((!on_flow_analysis && get_diagnostic_phase(id) != 2) || (on_flow_analysis && get_diagnostic_phase(id) == 2))
                            {
                                unsigned char   found;

                                found = 0;
                                {
                                    int i;
                                    i = 0;
                                    for (; i < 2; i++)
                                    {
                                        if (ctx->p_report->last_diagnostics_ids[i] == 0)
                                        {
                                            break;
                                        }
                                        if (ctx->p_report->last_diagnostics_ids[i] == id)
                                        {
                                            int t;

                                            found = 1;
                                            t = get_diagnostic_type(&ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index], id);
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
                                if (!found)
                                {
                                    ctx->p_report->fatal_error_expected = atoi(p_pragma_token->lexeme + 3);
                                    compiler_diagnostic(650, ctx, p_pragma_token, 0U, "pragma check failed");
                                }
                            }
                        }
                    }
                    else
                    {
                        compiler_diagnostic(650, ctx, p_pragma_token, 0U, "unknown pragma");
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
                unsigned long long w;

                w = 21474836480ULL;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
                ctx->options.null_checks_enabled = 1;
                ctx->options.flow_analysis = 1;
            }
            if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
            {
                unsigned long long w;

                w = 21474836480ULL;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
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
                    unsigned long long w;

                    w = 536870912ULL;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
                    ctx->options.ownership_enabled = 1;
                    ctx->options.flow_analysis = 1;
                }
                if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
                {
                    unsigned long long w;

                    w = 536870912ULL;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
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
                            unsigned long long w;

                            w = 22011707392ULL;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings |= w;
                            ctx->options.null_checks_enabled = 1;
                            ctx->options.flow_analysis = 1;
                            ctx->options.ownership_enabled = 1;
                        }
                        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "disable") == 0)
                        {
                            unsigned long long w;

                            w = 22011707392ULL;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
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
    struct pragma_declaration * p_pragma_declaration;

    p_pragma_declaration = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U || ctx->current->type != 128)
        {
            ;
            /*throw*/ goto _CKL0;
        }
        p_pragma_declaration = calloc(1, 8U);
        if (p_pragma_declaration == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_pragma_declaration->first_token = ctx->current;
        parser_match(ctx);
        while (ctx->current && ctx->current->type != 129)
        {
            ctx->current = ctx->current->next;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_pragma_declaration->last_token = ctx->current;
        parser_match(ctx);
    }
    /*catch*/ else _CKL0:
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
    struct static_assert_declaration * p_static_assert_declaration;

    p_static_assert_declaration = 0U;
    /*try*/ if (1)
    {
        struct token * position;
        unsigned char   show_error_if_not_constant;
        struct expression * p_constant_expression;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_static_assert_declaration = calloc(1, 16U);
        if (p_static_assert_declaration == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_static_assert_declaration->first_token = ctx->current;
        position = ctx->current;
        parser_match(ctx);
        if (parser_match_tk(ctx, 40) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        show_error_if_not_constant = 0;
        if (p_static_assert_declaration->first_token->type == 9058)
        {
            show_error_if_not_constant = 1;
        }
        if (ctx->options.flow_analysis)
        {
            show_error_if_not_constant = 0;
        }
        p_constant_expression = constant_expression(ctx, show_error_if_not_constant);
        if (p_constant_expression == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_static_assert_declaration->constant_expression = p_constant_expression;
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 44)
        {
            parser_match(ctx);
            p_static_assert_declaration->string_literal_opt = ctx->current;
            if (parser_match_tk(ctx, 130) != 0)
            {
                /*throw*/ goto _CKL0;
            }
        }
        if (parser_match_tk(ctx, 41) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_static_assert_declaration->last_token = ctx->current;
        if (parser_match_tk(ctx, 59) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (position->type == 9058)
        {
            if (object_has_constant_value(&p_static_assert_declaration->constant_expression->object) && !object_to_bool(&p_static_assert_declaration->constant_expression->object))
            {
                if (p_static_assert_declaration->string_literal_opt)
                {
                    compiler_diagnostic(1060, ctx, position, 0U, "static_assert failed %s\n", p_static_assert_declaration->string_literal_opt->lexeme);
                }
                else
                {
                    compiler_diagnostic(1060, ctx, position, 0U, "static_assert failed");
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    return p_static_assert_declaration;
}

void attribute_specifier_sequence_add(struct attribute_specifier_sequence * list, struct attribute_specifier * p_item)
{
    if (list->head == 0U)
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
        struct attribute_specifier * item;

        item = p->head;
        while (item)
        {
            struct attribute_specifier * next;

            next = item->next;
            item->next = 0U;
            attribute_specifier_delete(item);
            item = next;
        }
        free(p);
    }
}

struct attribute_specifier *attribute_specifier(struct parser_ctx * ctx);

struct attribute_specifier_sequence *attribute_specifier_sequence_opt(struct parser_ctx * ctx)
{
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;

    p_attribute_specifier_sequence = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (first_of_attribute_specifier(ctx))
        {
            p_attribute_specifier_sequence = calloc(1, 20U);
            if (p_attribute_specifier_sequence == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_attribute_specifier_sequence->first_token = ctx->current;
            while (ctx->current != 0U && first_of_attribute_specifier(ctx))
            {
                struct attribute_specifier * p_attribute_specifier;

                p_attribute_specifier = attribute_specifier(ctx);
                if (p_attribute_specifier == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_attribute_specifier_sequence->attributes_flags |= p_attribute_specifier->attribute_list->attributes_flags;
                attribute_specifier_sequence_add(p_attribute_specifier_sequence, p_attribute_specifier);
            }
            if (ctx->previous == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_attribute_specifier_sequence->last_token = ctx->previous;
        }
    }
    /*catch*/ else _CKL0:
    {
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = 0U;
    }
    return p_attribute_specifier_sequence;
}

static struct attribute_specifier_sequence *attribute_specifier_sequence(struct parser_ctx * ctx)
{
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;

    p_attribute_specifier_sequence = 0U;
    /*try*/ if (1)
    {
        p_attribute_specifier_sequence = calloc(1, 20U);
        if (p_attribute_specifier_sequence == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (first_of_attribute_specifier(ctx))
        {
            struct attribute_specifier * p_attribute_specifier;

            p_attribute_specifier = attribute_specifier(ctx);
            if (p_attribute_specifier == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            attribute_specifier_sequence_add(p_attribute_specifier_sequence, p_attribute_specifier);
        }
    }
    /*catch*/ else _CKL0:
    {
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = 0U;
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
    struct attribute_specifier * p_attribute_specifier;

    p_attribute_specifier = 0U;
    /*try*/ if (1)
    {
        struct attribute_list * p_attribute_list;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_attribute_specifier = calloc(1, 16U);
        if (p_attribute_specifier == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_attribute_specifier->first_token = ctx->current;
        if (parser_match_tk(ctx, 91) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (parser_match_tk(ctx, 91) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        p_attribute_list = attribute_list(ctx);
        if (p_attribute_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_attribute_specifier->attribute_list = p_attribute_list;
        if (parser_match_tk(ctx, 93) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_attribute_specifier->last_token = ctx->current;
        if (parser_match_tk(ctx, 93) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        attribute_specifier_delete(p_attribute_specifier);
        p_attribute_specifier = 0U;
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
    if (list->head == 0U)
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
    struct attribute * item;

    item = p->head;
    while (item)
    {
        struct attribute * next;

        next = item->next;
        item->next = 0U;
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
    struct attribute_list * p_attribute_list;

    p_attribute_list = 0U;
    /*try*/ if (1)
    {
        p_attribute_list = calloc(1, 12U);
        if (p_attribute_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (first_of_attribute(ctx) || ctx->current->type == 44))
        {
            if (first_of_attribute(ctx))
            {
                struct attribute * p_attribute;

                p_attribute = attribute(ctx);
                if (p_attribute == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_attribute_list->attributes_flags |= p_attribute->attributes_flags;
                attribute_list_add(p_attribute_list, p_attribute);
            }
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            if (ctx->current->type == 44)
            {
                parser_match(ctx);
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        attribute_list_delete(p_attribute_list);
        p_attribute_list = 0U;
    }
    return p_attribute_list;
}

unsigned char  first_of_attribute(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    if (ctx->current->type == 8996)
    {
        return 1;
    }
    if (is_keyword(ctx->current->lexeme) != 0)
    {
        return 1;
    }
    return 0;
}

struct attribute_token *attribute_token(struct parser_ctx * ctx);
struct attribute_argument_clause *attribute_argument_clause(struct parser_ctx * ctx);

struct attribute *attribute(struct parser_ctx * ctx)
{
    struct attribute * p_attribute;

    p_attribute = 0U;
    /*try*/ if (1)
    {
        struct attribute_token * p_attribute_token;

        p_attribute = calloc(1, 16U);
        if (p_attribute == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_attribute_token = attribute_token(ctx);
        if (p_attribute_token == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_attribute->attribute_token = p_attribute_token;
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_attribute->attributes_flags = p_attribute->attribute_token->attributes_flags;
        if (ctx->current->type == 40)
        {
            struct attribute_argument_clause * p_attribute_argument_clause;

            p_attribute_argument_clause = attribute_argument_clause(ctx);
            if (p_attribute_argument_clause == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_attribute->attribute_argument_clause = p_attribute_argument_clause;
        }
    }
    /*catch*/ else _CKL0:
    {
        attribute_delete(p_attribute);
        p_attribute = 0U;
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
    struct attribute_token * p_attribute_token;

    p_attribute_token = 0U;
    /*try*/ if (1)
    {
        struct token * attr_token;
        unsigned char   is_standard_attribute;
        unsigned char   is_cake_attr;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_attribute_token = calloc(1, 8U);
        if (p_attribute_token == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        attr_token = ctx->current;
        is_standard_attribute = 0;
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
                                else
                                {
                                    if (strcmp(attr_token->lexeme, "dtor") == 0)
                                    {
                                        is_standard_attribute = 1;
                                        p_attribute_token->attributes_flags = 256;
                                    }
                                    else
                                    {
                                        if (strcmp(attr_token->lexeme, "ctor") == 0)
                                        {
                                            is_standard_attribute = 1;
                                            p_attribute_token->attributes_flags = 128;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        is_cake_attr = !!(strcmp(attr_token->lexeme, "cake") == 0);
        if (token_is_identifier_or_keyword(ctx->current->type))
        {
            parser_match(ctx);
        }
        else
        {
            compiler_diagnostic(970, ctx, attr_token, 0U, "expected identifier");
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 14906)
        {
            parser_match(ctx);
            if (is_cake_attr)
            {
                compiler_diagnostic(8, ctx, attr_token, 0U, "warning '%s' is not an cake attribute", ctx->current->lexeme);
            }
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            if (token_is_identifier_or_keyword(ctx->current->type))
            {
                parser_match(ctx);
            }
            else
            {
                compiler_diagnostic(970, ctx, attr_token, 0U, "expected identifier");
            }
        }
        else
        {
            if (!is_standard_attribute)
            {
                compiler_diagnostic(8, ctx, attr_token, 0U, "warning '%s' is not an standard attribute", attr_token->lexeme);
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        attribute_token_delete(p_attribute_token);
        p_attribute_token = 0U;
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
    struct attribute_argument_clause * p_attribute_argument_clause;

    p_attribute_argument_clause = calloc(1, 8U);
    /*try*/ if (1)
    {
        if (p_attribute_argument_clause == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        p_attribute_argument_clause->p_balanced_token_sequence = balanced_token_sequence_opt(ctx);
        if (parser_match_tk(ctx, 41) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        attribute_argument_clause_delete(p_attribute_argument_clause);
        p_attribute_argument_clause = 0U;
    }
    return p_attribute_argument_clause;
}

void balanced_token_sequence_delete(struct balanced_token_sequence * p)
{
    if (p)
    {
        struct balanced_token * item;

        item = p->head;
        while (item)
        {
            struct balanced_token * next;

            next = item->next;
            free(item);
            item = next;
        }
        free(p);
    }
}

struct balanced_token_sequence *balanced_token_sequence_opt(struct parser_ctx * ctx)
{
    struct balanced_token_sequence * p_balanced_token_sequence;

    p_balanced_token_sequence = calloc(1, 8U);
    /*try*/ if (1)
    {
        int count1;
        int count2;
        int count3;

        if (p_balanced_token_sequence == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        count1 = 0;
        count2 = 0;
        count3 = 0;
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
            compiler_diagnostic(1090, ctx, ctx->current, 0U, "expected ']' before ')'");
        }
        if (count3 != 0)
        {
            compiler_diagnostic(1090, ctx, ctx->current, 0U, "expected '}' before ')'");
        }
    }
    /*catch*/ else _CKL0:
    {
        balanced_token_sequence_delete(p_balanced_token_sequence);
        p_balanced_token_sequence = 0U;
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
    struct statement * p_statement;

    p_statement = calloc(1, 8U);
    /*try*/ if (1)
    {
        if (p_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (first_of_labeled_statement(ctx))
        {
            p_statement->labeled_statement = labeled_statement(ctx);
            if (p_statement->labeled_statement == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            p_statement->unlabeled_statement = unlabeled_statement(ctx);
            if (p_statement->unlabeled_statement == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        statement_delete(p_statement);
        p_statement = 0U;
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
    struct primary_block * p_primary_block;

    ;
    p_primary_block = calloc(1, 20U);
    /*try*/ if (1)
    {
        if (p_primary_block == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (first_of_compound_statement(ctx))
        {
            p_primary_block->compound_statement = compound_statement(ctx);
            if (p_primary_block->compound_statement == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            if (first_of_selection_statement(ctx))
            {
                p_primary_block->selection_statement = selection_statement(ctx);
                if (p_primary_block->selection_statement == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            else
            {
                if (first_of_iteration_statement(ctx))
                {
                    p_primary_block->iteration_statement = iteration_statement(ctx);
                    if (p_primary_block->iteration_statement == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                }
                else
                {
                    if (ctx->current->type == 9009)
                    {
                        p_primary_block->defer_statement = defer_statement(ctx);
                        if (p_primary_block->defer_statement == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                    }
                    else
                    {
                        if (ctx->current->type == 9036)
                        {
                            p_primary_block->try_statement = try_statement(ctx);
                            if (p_primary_block->try_statement == 0U)
                            {
                                /*throw*/ goto _CKL0;
                            }
                        }
                        else
                        {
                            compiler_diagnostic(970, ctx, ctx->current, 0U, "unexpected token");
                        }
                    }
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        primary_block_delete(p_primary_block);
        p_primary_block = 0U;
    }
    return p_primary_block;
}

struct secondary_block *secondary_block(struct parser_ctx * ctx)
{
    struct secondary_block * p_secondary_block;

    p_secondary_block = 0U;
    /*try*/ if (1)
    {
        struct statement * p_statement;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        check_open_brace_style(ctx, ctx->current);
        p_secondary_block = calloc(1, 12U);
        if (p_secondary_block == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_secondary_block->first_token = ctx->current;
        p_statement = statement(ctx);
        if (p_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_secondary_block->statement = p_statement;
        if (ctx->previous == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_secondary_block->last_token = ctx->previous;
        check_close_brace_style(ctx, p_secondary_block->last_token);
    }
    /*catch*/ else _CKL0:
    {
        secondary_block_delete(p_secondary_block);
        p_secondary_block = 0U;
    }
    return p_secondary_block;
}

unsigned char  unlabeled_statement_ends_with_jump(struct unlabeled_statement * p_unlabeled_statement)
{
    struct expression * p_expression;

    p_expression = 0U;
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
        return !!(p_expression->type.attributes_flags & 16);
    }
    if (p_unlabeled_statement->primary_block && p_unlabeled_statement->primary_block->compound_statement && p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail && p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement)
    {
        return !!(p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != 0U);
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
    if (ctx->current == 0U)
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
    struct unlabeled_statement * p_unlabeled_statement;

    p_unlabeled_statement = calloc(1, 12U);
    /*try*/ if (1)
    {
        if (p_unlabeled_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (first_of_primary_block(ctx))
        {
            p_unlabeled_statement->primary_block = primary_block(ctx);
            if (p_unlabeled_statement->primary_block == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            if (first_of_jump_statement(ctx))
            {
                p_unlabeled_statement->jump_statement = jump_statement(ctx);
                if (p_unlabeled_statement->jump_statement == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            else
            {
                p_unlabeled_statement->expression_statement = expression_statement(ctx, 0);
                if (p_unlabeled_statement->expression_statement == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                if (p_unlabeled_statement->expression_statement->expression_opt)
                {
                    if (!type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type) && type_is_nodiscard(&p_unlabeled_statement->expression_statement->expression_opt->type) && p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & 8192)
                    {
                        if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                        {
                            compiler_diagnostic(8, ctx, p_unlabeled_statement->expression_statement->expression_opt->first_token, 0U, "ignoring return value of function declared with 'nodiscard' attribute");
                        }
                    }
                    if (type_is_owner(&p_unlabeled_statement->expression_statement->expression_opt->type) && p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & 8192)
                    {
                        if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                        {
                            compiler_diagnostic(8, ctx, p_unlabeled_statement->expression_statement->expression_opt->first_token, 0U, "ignoring the result of _Owner type ");
                        }
                    }
                }
                if (p_unlabeled_statement->expression_statement->expression_opt && p_unlabeled_statement->expression_statement->expression_opt->expression_type == 12)
                {
                }
                else
                {
                    if (p_unlabeled_statement != 0U && p_unlabeled_statement->jump_statement == 0U && p_unlabeled_statement->expression_statement != 0U && p_unlabeled_statement->expression_statement->expression_opt && !type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type) && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 53 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 12 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 16 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 17 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 26 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 27 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 23 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 25)
                    {
                        if (ctx->current && ctx->current->level == 0)
                        {
                        }
                    }
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        unlabeled_statement_delete(p_unlabeled_statement);
        p_unlabeled_statement = 0U;
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

struct label_list_item *label_list_find(struct label_list * list, char * label_name);
void label_list_push(struct label_list * list, struct label_list_item * pitem);
struct label *case_label_list_find_range(struct case_label_list * list, struct object * begin, struct object * end);
int object_to_str(struct object * a, int n, char str[]);
struct label *case_label_list_find(struct case_label_list * list, struct object * object);
void case_label_list_push(struct case_label_list * list, struct label * pnew);
unsigned char  type_is_enum(struct type * p_type);
void check_diferent_enuns(struct parser_ctx * ctx, struct token * operator_token, struct expression * left, struct expression * right, char * message);
struct label *case_label_list_find_default(struct case_label_list * list);

struct label *label(struct parser_ctx * ctx)
{
    struct label * p_label;

    if (ctx->current == 0U)
    {
        return 0U;
    }
    p_label = calloc(1, 24U);
    /*try*/ if (1)
    {
        if (p_label == 0U)
        {
            /*throw*/ goto _CKL1;
        }
        p_label->label_id = ctx->label_id++;
        p_label->p_first_token = ctx->current;
        if (ctx->current->type == 8996)
        {
            struct label_list_item * p_label_list_item;

            p_label_list_item = label_list_find(&ctx->label_list, ctx->current->lexeme);
            if (p_label_list_item == 0U)
            {
                struct label_list_item * p_label_list_item_new;

                p_label_list_item_new = calloc(1, 12U);
                if (p_label_list_item_new)
                {
                    p_label_list_item_new->p_defined = ctx->current;
                    label_list_push(&ctx->label_list, p_label_list_item_new);
                }
            }
            else
            {
                if (p_label_list_item->p_defined)
                {
                    compiler_diagnostic(1440, ctx, ctx->current, 0U, "duplicated label '%s'", ctx->current->lexeme);
                    compiler_diagnostic(63, ctx, p_label_list_item->p_defined, 0U, "previous definition of '%s'", ctx->current->lexeme);
                }
                else
                {
                    p_label_list_item->p_defined = ctx->current;
                }
            }
            p_label->p_identifier_opt = ctx->current;
            parser_match(ctx);
            if (parser_match_tk(ctx, 58) != 0)
            {
                /*throw*/ goto _CKL1;
            }
        }
        else
        {
            if (ctx->current->type == 9001)
            {
                if (ctx->p_current_selection_statement == 0U || ctx->p_current_selection_statement->condition == 0U)
                {
                    /*throw*/ goto _CKL1;
                }
                parser_match(ctx);
                p_label->constant_expression = constant_expression(ctx, 1);
                if (p_label->constant_expression == 0U)
                {
                    /*throw*/ goto _CKL1;
                }
                if (ctx->current == 0U)
                {
                    unexpected_end_of_file(ctx);
                    /*throw*/ goto _CKL1;
                }
                if (ctx->current->type == 11822)
                {
                    struct label * p_existing_label;

                    parser_match(ctx);
                    p_label->constant_expression_end = constant_expression(ctx, 1);
                    if (p_label->constant_expression_end == 0U)
                    {
                        /*throw*/ goto _CKL1;
                    }
                    p_existing_label = case_label_list_find_range(&ctx->p_current_selection_statement->label_list, &p_label->constant_expression->object, &p_label->constant_expression_end->object);
                    if (p_existing_label)
                    {
                        char str1[50];
                        char str2[50];

                        object_to_str(&p_label->constant_expression->object, 50, str1);
                        object_to_str(&p_label->constant_expression_end->object, 50, str2);
                        compiler_diagnostic(1450, ctx, p_label->constant_expression->first_token, 0U, "case '%s' ... '%s' is duplicating values", str1, str2);
                        ;
                        compiler_diagnostic(62, ctx, p_existing_label->constant_expression->first_token, 0U, "previous case");
                    }
                }
                else
                {
                    struct label * p_existing_label;

                    p_existing_label = case_label_list_find(&ctx->p_current_selection_statement->label_list, &p_label->constant_expression->object);
                    if (p_existing_label)
                    {
                        char str[50];

                        object_to_str(&p_label->constant_expression->object, 50, str);
                        compiler_diagnostic(1450, ctx, p_label->constant_expression->first_token, 0U, "duplicate case '%s'", str);
                        ;
                        compiler_diagnostic(62, ctx, p_existing_label->constant_expression->first_token, 0U, "previous declaration");
                    }
                }
                if (parser_match_tk(ctx, 58) != 0)
                {
                    /*throw*/ goto _CKL1;
                }
                case_label_list_push(&ctx->p_current_selection_statement->label_list, p_label);
                if (ctx->p_current_selection_statement && ctx->p_current_selection_statement->condition && ctx->p_current_selection_statement->condition->expression)
                {
                    struct enum_specifier * p_enum_specifier;

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
                    if (ctx->p_current_selection_statement == 0U)
                    {
                        /*throw*/ goto _CKL1;
                    }
                    p_enum_specifier = 0U;
                    if (ctx->p_current_selection_statement->condition && ctx->p_current_selection_statement->condition->expression && ctx->p_current_selection_statement->condition->expression->type.enum_specifier)
                    {
                        p_enum_specifier = get_complete_enum_specifier(ctx->p_current_selection_statement->condition->expression->type.enum_specifier);
                    }
                    if (p_enum_specifier)
                    {
                        struct enumerator * p_enumerator;

                        p_enumerator = find_enumerator_by_value(p_enum_specifier, &p_label->constant_expression->object);
                        if (p_enumerator == 0U)
                        {
                            char str[50];

                            object_to_str(&p_label->constant_expression->object, 50, str);
                            compiler_diagnostic(3, ctx, p_label->constant_expression->first_token, 0U, "case value '%s' not in enumerated type 'enum %s'", str, p_enum_specifier->tag_name);
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
                    struct label * p_existing_default_label;

                    p_existing_default_label = case_label_list_find_default(&ctx->p_current_selection_statement->label_list);
                    if (p_existing_default_label)
                    {
                        compiler_diagnostic(1780, ctx, p_label->p_first_token, 0U, "multiple default labels in one switch");
                        compiler_diagnostic(63, ctx, p_existing_default_label->p_first_token, 0U, "previous default");
                        /*throw*/ goto _CKL1;
                    }
                    parser_match(ctx);
                    if (parser_match_tk(ctx, 58) != 0)
                    {
                        /*throw*/ goto _CKL1;
                    }
                    case_label_list_push(&ctx->p_current_selection_statement->label_list, p_label);
                }
            }
        }
    }
    /*catch*/ else _CKL1:
    {
        label_delete(p_label);
        p_label = 0U;
    }
    return p_label;
}

struct label *case_label_list_find_default(struct case_label_list * list)
{
    struct label * p;

    p = list->head;
    while (p)
    {
        if (p->p_first_token->type == 9007)
        {
            return p;
        }
        p = p->next;
    }
    return 0U;
}

int object_greater_than_or_equal(struct object * a, struct object * b);
int object_smaller_than_or_equal(struct object * a, struct object * b);

struct label *case_label_list_find_range(struct case_label_list * list, struct object * begin, struct object * end)
{
    struct label * p;

    p = list->head;
    while (p)
    {
        if (p->constant_expression_end == 0U)
        {
            if (object_greater_than_or_equal(&p->constant_expression->object, begin) && object_smaller_than_or_equal(&p->constant_expression_end->object, end))
            {
                return p;
            }
        }
        else
        {
            if (object_smaller_than_or_equal(&p->constant_expression->object, end) && object_smaller_than_or_equal(begin, &p->constant_expression_end->object))
            {
                return p;
            }
        }
        p = p->next;
    }
    return 0U;
}

struct label *case_label_list_find(struct case_label_list * list, struct object * object)
{
    struct label * p;

    p = list->head;
    while (p)
    {
        if (p->constant_expression_end == 0U)
        {
            if (object_equal(&p->constant_expression->object, object))
            {
                return p;
            }
        }
        else
        {
            if (object_greater_than_or_equal(object, &p->constant_expression->object) && object_smaller_than_or_equal(object, &p->constant_expression_end->object))
            {
                return p;
            }
        }
        p = p->next;
    }
    return 0U;
}

void case_label_list_push(struct case_label_list * list, struct label * pnew)
{
    if (list->head == 0U)
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        ;
        list->tail->next = pnew;
        list->tail = pnew;
    }
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
    struct labeled_statement * p_labeled_statement;

    p_labeled_statement = calloc(1, 8U);
    /*try*/ if (1)
    {
        struct label * p_label;
        struct statement * p_statement;

        if (p_labeled_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_label = label(ctx);
        if (p_label == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_labeled_statement->label = p_label;
        p_statement = statement(ctx);
        if (p_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_labeled_statement->statement = p_statement;
    }
    /*catch*/ else _CKL0:
    {
        labeled_statement_delete(p_labeled_statement);
        p_labeled_statement = 0U;
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
    struct scope  block_scope;
    struct compound_statement * p_compound_statement;

    block_scope.scope_level = 0;
    block_scope.tags.table = 0;
    block_scope.tags.capacity = 0;
    block_scope.tags.size = 0;
    block_scope.variables.table = 0;
    block_scope.variables.capacity = 10;
    block_scope.variables.size = 0;
    block_scope.next = 0;
    block_scope.previous = 0;
    p_compound_statement = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_compound_statement = calloc(1, 48U);
        if (p_compound_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_compound_statement->diagnostic_flags = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];
        scope_list_push(&ctx->scopes, &block_scope);
        p_compound_statement->first_token = ctx->current;
        if (parser_match_tk(ctx, 123) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            compiler_diagnostic(970, ctx, ctx->input_list.tail, 0U, "unexpected end of file");
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type != 125)
        {
            unsigned char   berror;

            berror = 0;
            p_compound_statement->block_item_list = block_item_list(ctx, &berror);
            if (berror)
            {
                /*throw*/ goto _CKL0;
            }
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_compound_statement->last_token = ctx->current;
        if (parser_match_tk(ctx, 125) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        {
            int i;
            i = 0;
            for (; i < block_scope.variables.capacity; i++)
            {
                struct map_entry * entry;

                if (block_scope.variables.table == 0U)
                {
                    continue;
                }
                entry = block_scope.variables.table[i];
                while (entry)
                {
                    struct declarator * p_declarator;
                    struct init_declarator * p_init_declarator;

                    if (entry->type != 4 && entry->type != 5)
                    {
                        entry = entry->next;
                        continue;
                    }
                    p_declarator = 0U;
                    p_init_declarator = 0U;
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
                        if (!type_is_maybe_unused(&p_declarator->type) && p_declarator->num_uses == 0)
                        {
                            if (p_declarator->name_opt && p_declarator->name_opt->token_origin->level == 0)
                            {
                                compiler_diagnostic(1, ctx, p_declarator->name_opt, 0U, "'%s': unreferenced declarator", p_declarator->name_opt->lexeme);
                            }
                        }
                    }
                    entry = entry->next;
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        compound_statement_delete(p_compound_statement);
        p_compound_statement = 0U;
    }
    scope_list_pop(&ctx->scopes);
    scope_destroy(&block_scope);
    return p_compound_statement;
}

void block_item_list_add(struct block_item_list * list, struct block_item * p_item)
{
    if (list->head == 0U)
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
    struct block_item * item;

    item = list->head;
    while (item)
    {
        struct block_item * next;

        next = item->next;
        item->next = 0U;
        block_item_delete(item);
        item = next;
    }
}

struct block_item *block_item(struct parser_ctx * ctx);

struct block_item_list block_item_list(struct parser_ctx * ctx, unsigned char  * error)
{
    struct block_item_list  block_item_list;
    struct block_item * p_block_item;

    *error = 0;
    _cake_zmem(&block_item_list, 8);
    p_block_item = 0U;
    /*try*/ if (1)
    {
        p_block_item = block_item(ctx);
        if (p_block_item == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        block_item_list_add(&block_item_list, p_block_item);
        p_block_item = 0U;
        while (ctx->current != 0U && ctx->current->type != 125)
        {
            p_block_item = block_item(ctx);
            if (p_block_item == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            block_item_list_add(&block_item_list, p_block_item);
            p_block_item = 0U;
        }
    }
    /*catch*/ else _CKL0:
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
    struct block_item * p_block_item;

    p_block_item = 0U;
    /*try*/ if (1)
    {
        struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_block_item = calloc(1, 20U);
        if (p_block_item == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
            /*throw*/ goto _CKL0;
        }
        p_block_item->first_token = ctx->current;
        if (ctx->current->type == 9049)
        {
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
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
            if (ctx->current == 0U)
            {
                attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
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
                struct init_declarator * p;

                p_block_item->declaration = declaration(ctx, p_attribute_specifier_sequence_opt, 4096);
                if (p_block_item->declaration == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_attribute_specifier_sequence_opt = 0U;
                p = p_block_item->declaration->init_declarator_list.head;
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
                    if (p_block_item->label == 0U)
                    {
                        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                        /*throw*/ goto _CKL0;
                    }
                }
                else
                {
                    p_block_item->unlabeled_statement = unlabeled_statement(ctx);
                    if (p_block_item->unlabeled_statement == 0U)
                    {
                        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
                        /*throw*/ goto _CKL0;
                    }
                }
            }
        }
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence_opt);
    }
    /*catch*/ else _CKL0:
    {
        block_item_delete(p_block_item);
        p_block_item = 0U;
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
    struct try_statement * p_try_statement;

    p_try_statement = 0U;
    /*try*/ if (1)
    {
        struct try_statement * try_statement_copy_opt;
        struct secondary_block * p_secondary_block;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_try_statement = calloc(1, 24U);
        if (p_try_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_try_statement->first_token = ctx->current;
        ;
        try_statement_copy_opt = ctx->p_current_try_statement_opt;
        ctx->p_current_try_statement_opt = p_try_statement;
        p_try_statement->catch_label_id = ctx->label_id++;
        if (parser_match_tk(ctx, 9036) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_try_statement->secondary_block = p_secondary_block;
        ctx->p_current_try_statement_opt = try_statement_copy_opt;
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 9006)
        {
            p_try_statement->catch_token_opt = ctx->current;
            parser_match(ctx);
            ;
            p_try_statement->catch_secondary_block_opt = secondary_block(ctx);
            if (p_try_statement->catch_secondary_block_opt == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        if (ctx->previous == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_try_statement->last_token = ctx->previous;
    }
    /*catch*/ else _CKL0:
    {
        try_statement_delete(p_try_statement);
        p_try_statement = 0U;
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
unsigned char  type_is_bool(struct type * p_type);
unsigned char  type_is_essential_bool(struct type * p_type);

struct selection_statement *selection_statement(struct parser_ctx * ctx)
{
    struct scope  if_scope;
    struct selection_statement * p_selection_statement;

    _cake_zmem(&if_scope, 36);
    scope_list_push(&ctx->scopes, &if_scope);
    p_selection_statement = 0U;
    /*try*/ if (1)
    {
        unsigned char   is_if;
        struct selection_statement * previous;
        struct secondary_block * p_secondary_block;
        struct token * p_tk;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_selection_statement = calloc(1, 56U);
        if (p_selection_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_selection_statement->label_id = ctx->label_id++;
        p_selection_statement->first_token = ctx->current;
        is_if = !!((ctx->current->type == 9017));
        if (ctx->current->type == 9017 || ctx->current->type == 9034)
        {
            parser_match(ctx);
        }
        else
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (!(ctx->current->flags & 2) && !style_has_one_space(ctx->current))
        {
            compiler_diagnostic(10, ctx, ctx->current, 0U, "one space");
        }
        p_selection_statement->open_parentesis_token = ctx->current;
        if (parser_match_tk(ctx, 40) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        p_selection_statement->p_init_statement = init_statement(ctx, 1);
        if (p_selection_statement->p_init_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
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
                p_selection_statement->condition = calloc(1, 24U);
                if (p_selection_statement->condition == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                if (p_selection_statement->p_init_statement->p_expression_statement)
                {
                    p_selection_statement->condition->first_token = p_selection_statement->p_init_statement->p_expression_statement->expression_opt->first_token;
                    p_selection_statement->condition->last_token = p_selection_statement->p_init_statement->p_expression_statement->expression_opt->last_token;
                    p_selection_statement->condition->expression = p_selection_statement->p_init_statement->p_expression_statement->expression_opt;
                    p_selection_statement->p_init_statement->p_expression_statement->expression_opt = 0U;
                }
                if (p_selection_statement->p_init_statement->p_simple_declaration)
                {
                    p_selection_statement->condition->first_token = p_selection_statement->p_init_statement->p_simple_declaration->first_token;
                    p_selection_statement->condition->last_token = p_selection_statement->p_init_statement->p_simple_declaration->last_token;
                    if (p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head != p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.tail)
                    {
                        ;
                        /*throw*/ goto _CKL0;
                    }
                    p_selection_statement->condition->p_init_declarator = p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head;
                    p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head = 0U;
                    p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.tail = 0U;
                    p_selection_statement->condition->p_declaration_specifiers = p_selection_statement->p_init_statement->p_simple_declaration->p_declaration_specifiers;
                    p_selection_statement->p_init_statement->p_simple_declaration->p_declaration_specifiers = 0U;
                }
                init_statement_delete(p_selection_statement->p_init_statement);
                p_selection_statement->p_init_statement = 0U;
            }
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_selection_statement->close_parentesis_token = ctx->current;
        if (parser_match_tk(ctx, 41) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (!is_if && p_selection_statement && p_selection_statement->condition)
        {
            if (type_is_bool(&p_selection_statement->condition->expression->type) || type_is_essential_bool(&p_selection_statement->condition->expression->type))
            {
                compiler_diagnostic(44, ctx, p_selection_statement->condition->first_token, 0U, "switch condition has boolean value");
            }
        }
        previous = ctx->p_current_selection_statement;
        ctx->p_current_selection_statement = p_selection_statement;
        p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (p_secondary_block->statement->unlabeled_statement && p_secondary_block->statement->unlabeled_statement->expression_statement && p_secondary_block->statement->unlabeled_statement->expression_statement->expression_opt == 0U)
        {
            compiler_diagnostic(44, ctx, p_secondary_block->first_token, 0U, "empty controlled statement found; is this the intent?");
        }
        ;
        p_selection_statement->secondary_block = p_secondary_block;
        if (p_selection_statement->first_token->type == 9034)
        {
            if (case_label_list_find_default(&p_selection_statement->label_list) == 0U)
            {
                struct enum_specifier * p_enum_specifier;

                p_enum_specifier = 0U;
                if (ctx->p_current_selection_statement && ctx->p_current_selection_statement->condition && ctx->p_current_selection_statement->condition->expression && ctx->p_current_selection_statement->condition->expression->type.enum_specifier)
                {
                    p_enum_specifier = get_complete_enum_specifier(ctx->p_current_selection_statement->condition->expression->type.enum_specifier);
                }
                if (p_enum_specifier)
                {
                    struct enumerator * p;

                    p = p_enum_specifier->enumerator_list.head;
                    while (p)
                    {
                        struct label * p_used;

                        p_used = case_label_list_find(&p_selection_statement->label_list, &p->value);
                        if (p_used == 0U)
                        {
                            compiler_diagnostic(44, ctx, ctx->current, 0U, "enumeration '%s' not handled in switch", p->token->lexeme);
                        }
                        p = p->next;
                    }
                }
            }
        }
        ctx->p_current_selection_statement = previous;
        if (is_if && ctx->current && ctx->current->type == 9011)
        {
            struct secondary_block * p_secondary_block2;

            p_selection_statement->else_token_opt = ctx->current;
            parser_match(ctx);
            ;
            p_secondary_block2 = secondary_block(ctx);
            if (p_secondary_block2 == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_selection_statement->else_secondary_block_opt = p_secondary_block2;
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_tk = previous_parser_token(ctx->current);
        if (p_tk == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_selection_statement->last_token = p_tk;
    }
    /*catch*/ else _CKL0:
    {
        selection_statement_delete(p_selection_statement);
        p_selection_statement = 0U;
    }
    scope_list_pop(&ctx->scopes);
    scope_destroy(&if_scope);
    return p_selection_statement;
}

struct defer_statement *defer_statement(struct parser_ctx * ctx)
{
    struct defer_statement * p_defer_statement;

    p_defer_statement = 0U;
    /*try*/ if (1)
    {
        struct defer_statement * p_previous_defer_statement_opt;
        struct secondary_block * p_secondary_block;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type != 9009)
        {
            /*throw*/ goto _CKL0;
        }
        p_defer_statement = calloc(1, 12U);
        if (p_defer_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_defer_statement->first_token = ctx->current;
        parser_match(ctx);
        p_previous_defer_statement_opt = ctx->p_current_defer_statement_opt;
        ctx->p_current_defer_statement_opt = p_defer_statement;
        p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_defer_statement->secondary_block = p_secondary_block;
        if (ctx->previous == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_defer_statement->last_token = ctx->previous;
        ctx->p_current_defer_statement_opt = p_previous_defer_statement_opt;
    }
    /*catch*/ else _CKL0:
    {
        defer_statement_delete(p_defer_statement);
        p_defer_statement = 0U;
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
    struct iteration_statement * p_iteration_statement;

    p_iteration_statement = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_iteration_statement = calloc(1, 36U);
        if (p_iteration_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_iteration_statement->first_token = ctx->current;
        if (ctx->current->type == 9008)
        {
            struct secondary_block * p_secondary_block;

            parser_match(ctx);
            p_secondary_block = secondary_block(ctx);
            if (p_secondary_block == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_iteration_statement->secondary_block = p_secondary_block;
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            p_iteration_statement->second_token = ctx->current;
            if (parser_match_tk(ctx, 9042) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            if (parser_match_tk(ctx, 40) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            p_iteration_statement->expression1 = expression(ctx);
            if (parser_match_tk(ctx, 41) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            if (parser_match_tk(ctx, 59) != 0)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            if (ctx->current->type == 9042)
            {
                struct secondary_block * p_secondary_block;

                parser_match(ctx);
                if (parser_match_tk(ctx, 40) != 0)
                {
                    /*throw*/ goto _CKL0;
                }
                p_iteration_statement->expression1 = expression(ctx);
                if (parser_match_tk(ctx, 41) != 0)
                {
                    /*throw*/ goto _CKL0;
                }
                p_secondary_block = secondary_block(ctx);
                if (p_secondary_block == 0U)
                {
                    /*throw*/ goto _CKL0;
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
                        /*throw*/ goto _CKL0;
                    }
                    if (first_of_declaration_specifier(ctx))
                    {
                        struct scope  for_scope;
                        struct secondary_block * p_secondary_block;

                        _cake_zmem(&for_scope, 36);
                        scope_list_push(&ctx->scopes, &for_scope);
                        p_iteration_statement->declaration = declaration(ctx, 0U, 4096);
                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            /*throw*/ goto _CKL0;
                        }
                        if (ctx->current->type != 59)
                        {
                            p_iteration_statement->expression1 = expression(ctx);
                            if (p_iteration_statement->expression1 == 0U)
                            {
                                scope_list_pop(&ctx->scopes);
                                scope_destroy(&for_scope);
                                /*throw*/ goto _CKL0;
                            }
                        }
                        if (parser_match_tk(ctx, 59) != 0)
                        {
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            /*throw*/ goto _CKL0;
                        }
                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            /*throw*/ goto _CKL0;
                        }
                        if (ctx->current->type != 41)
                        {
                            p_iteration_statement->expression2 = expression(ctx);
                        }
                        if (parser_match_tk(ctx, 41) != 0)
                        {
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            /*throw*/ goto _CKL0;
                        }
                        p_secondary_block = secondary_block(ctx);
                        if (p_secondary_block == 0U)
                        {
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            /*throw*/ goto _CKL0;
                        }
                        p_iteration_statement->secondary_block = p_secondary_block;
                        scope_list_pop(&ctx->scopes);
                        scope_destroy(&for_scope);
                    }
                    else
                    {
                        struct secondary_block * p_secondary_block;

                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            /*throw*/ goto _CKL0;
                        }
                        if (ctx->current->type != 59)
                        {
                            p_iteration_statement->expression0 = expression(ctx);
                        }
                        if (parser_match_tk(ctx, 59) != 0)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            /*throw*/ goto _CKL0;
                        }
                        if (ctx->current->type != 59)
                        {
                            p_iteration_statement->expression1 = expression(ctx);
                        }
                        if (parser_match_tk(ctx, 59) != 0)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            /*throw*/ goto _CKL0;
                        }
                        if (ctx->current->type != 41)
                        {
                            p_iteration_statement->expression2 = expression(ctx);
                        }
                        if (parser_match_tk(ctx, 41) != 0)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            /*throw*/ goto _CKL0;
                        }
                        p_secondary_block = secondary_block(ctx);
                        if (p_secondary_block == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        p_iteration_statement->secondary_block = p_secondary_block;
                    }
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        iteration_statement_delete(p_iteration_statement);
        p_iteration_statement = 0U;
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
    struct jump_statement * p_jump_statement;

    p_jump_statement = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_jump_statement = calloc(1, 28U);
        if (p_jump_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_jump_statement->first_token = ctx->current;
        if (ctx->current->type == 9016)
        {
            struct label_list_item * p_label_list_item;

            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            p_label_list_item = label_list_find(&ctx->label_list, ctx->current->lexeme);
            if (p_label_list_item == 0U)
            {
                struct label_list_item * p_label_list_item_new;

                p_label_list_item_new = calloc(1, 12U);
                if (p_label_list_item_new)
                {
                    p_label_list_item_new->p_last_usage = ctx->current;
                    label_list_push(&ctx->label_list, p_label_list_item_new);
                }
            }
            else
            {
                p_label_list_item->p_last_usage = ctx->current;
            }
            p_jump_statement->label = ctx->current;
            if (parser_match_tk(ctx, 8996) != 0)
            {
                /*throw*/ goto _CKL0;
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
                        if (ctx->p_current_try_statement_opt == 0U)
                        {
                            compiler_diagnostic(1110, ctx, ctx->current, 0U, "throw statement not within try block");
                        }
                        else
                        {
                            p_jump_statement->label_id = ctx->p_current_try_statement_opt->catch_label_id;
                        }
                        parser_match(ctx);
                    }
                    else
                    {
                        if (ctx->current->type == 9027)
                        {
                            struct token * p_return_token;
                            struct type  return_type;

                            if (ctx->p_current_defer_statement_opt != 0U)
                            {
                                compiler_diagnostic(1400, ctx, ctx->current, 0U, "%s", "return cannot be used inside defer statement");
                                /*throw*/ goto _CKL0;
                            }
                            p_return_token = ctx->current;
                            parser_match(ctx);
                            if (ctx->current == 0U)
                            {
                                unexpected_end_of_file(ctx);
                                /*throw*/ goto _CKL0;
                            }
                            return_type = get_function_return_type(&ctx->p_current_function_opt->type);
                            if (ctx->current->type != 59)
                            {
                                p_jump_statement->expression_opt = expression(ctx);
                                if (p_jump_statement->expression_opt)
                                {
                                    if (type_is_void(&return_type))
                                    {
                                        char * func_name;

                                        func_name = ctx->p_current_function_opt->name_opt ? ctx->p_current_function_opt->name_opt->lexeme : "unnamed";
                                        compiler_diagnostic(1120, ctx, p_return_token, 0U, "void function '%s' should not return a value", func_name);
                                    }
                                    else
                                    {
                                        check_assigment(ctx, &return_type, p_jump_statement->expression_opt, 0);
                                    }
                                }
                            }
                            else
                            {
                                if (!type_is_void(&return_type))
                                {
                                    char * func_name;

                                    func_name = ctx->p_current_function_opt->name_opt ? ctx->p_current_function_opt->name_opt->lexeme : "unnamed";
                                    compiler_diagnostic(1121, ctx, p_return_token, 0U, "non void function '%s' should return a value", func_name);
                                }
                            }
                            type_destroy(&return_type);
                        }
                        else
                        {
                            ;
                        }
                    }
                }
            }
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_jump_statement->last_token = ctx->current;
        if (parser_match_tk(ctx, 59) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        jump_statement_delete(p_jump_statement);
        p_jump_statement = 0U;
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
    struct expression_statement * p_expression_statement;

    p_expression_statement = calloc(1, 8U);
    /*try*/ if (1)
    {
        if (p_expression_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_expression_statement->p_attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type != 59)
        {
            p_expression_statement->expression_opt = expression(ctx);
            if (p_expression_statement->expression_opt == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        if (!ignore_semicolon && parser_match_tk(ctx, 59) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_statement_delete(p_expression_statement);
        p_expression_statement = 0U;
    }
    return p_expression_statement;
}

void declaration_list_add(struct declaration_list * list, struct declaration * p_declaration)
{
    if (list->head == 0U)
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
    struct condition * p_condition;

    p_condition = 0U;
    /*try*/ if (1)
    {
        struct token * previous;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_condition = calloc(1, 24U);
        if (p_condition == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_condition->first_token = ctx->current;
        if (first_of_declaration_specifier(ctx))
        {
            struct init_declarator * p_init_declarator;

            p_condition->p_attribute_specifier_sequence_opt = attribute_specifier_sequence(ctx);
            p_condition->p_declaration_specifiers = declaration_specifiers(ctx, 4096);
            if (p_condition->p_declaration_specifiers == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_init_declarator = init_declarator(ctx, p_condition->p_declaration_specifiers);
            if (p_init_declarator == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_condition->p_init_declarator = p_init_declarator;
        }
        else
        {
            p_condition->expression = expression(ctx);
            if (p_condition->expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        previous = previous_parser_token(ctx->current);
        if (previous)
        {
            p_condition->last_token = previous;
        }
    }
    /*catch*/ else _CKL0:
    {
        condition_delete(p_condition);
        p_condition = 0U;
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
    struct init_statement * p_init_statement;

    p_init_statement = 0U;
    /*try*/ if (1)
    {
        p_init_statement = calloc(1, 8U);
        if (p_init_statement == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (first_of_declaration_specifier(ctx))
        {
            p_init_statement->p_simple_declaration = simple_declaration(ctx, 0U, ignore_semicolon);
        }
        else
        {
            p_init_statement->p_expression_statement = expression_statement(ctx, ignore_semicolon);
        }
    }
    /*catch*/ else _CKL0:
    {
        init_statement_delete(p_init_statement);
        p_init_statement = 0U;
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
    struct declaration * p;

    p = list->head;
    while (p)
    {
        struct declaration * next;

        next = p->next;
        p->next = 0U;
        declaration_delete(p);
        p = next;
    }
}

static void check_unused_static_declarators(struct parser_ctx * ctx, struct declaration_list * declaration_list)
{
    struct declaration * p;

    p = declaration_list->head;
    while (p)
    {
        if (p->declaration_specifiers && p->declaration_specifiers->storage_class_specifier_flags & 4)
        {
            if (p->init_declarator_list.head && p->init_declarator_list.head->p_declarator)
            {
                struct map_entry * p_entry;

                p_entry = find_variables(ctx, p->init_declarator_list.head->p_declarator->name_opt->lexeme, 0U);
                if (p_entry && (p_entry->type == 4 || p_entry->type == 5))
                {
                    struct declarator * p_declarator_local;
                    int num_uses;

                    p_declarator_local = 0U;
                    if (p_entry->type == 5)
                    {
                        p_declarator_local = p_entry->data.p_init_declarator->p_declarator;
                    }
                    else
                    {
                        p_declarator_local = p_entry->data.p_declarator;
                    }
                    num_uses = p_declarator_local->num_uses;
                    if (num_uses == 0)
                    {
                        if (type_is_function(&p->init_declarator_list.head->p_declarator->type))
                        {
                            compiler_diagnostic(56, ctx, p->init_declarator_list.head->p_declarator->name_opt, 0U, "warning: static function '%s' not used.", p->init_declarator_list.head->p_declarator->name_opt->lexeme);
                        }
                        else
                        {
                            compiler_diagnostic(1, ctx, p->init_declarator_list.head->p_declarator->name_opt, 0U, "warning: '%s' not used.", p->init_declarator_list.head->p_declarator->name_opt->lexeme);
                        }
                    }
                }
            }
        }
        p = p->next;
    }
}

struct declaration *external_declaration(struct parser_ctx * ctx);

struct declaration_list translation_unit(struct parser_ctx * ctx, unsigned char  * berror)
{
    struct declaration_list  declaration_list;

    *berror = 0;
    _cake_zmem(&declaration_list, 8);
    /*try*/ if (1)
    {
        while (ctx->current != 0U)
        {
            struct declaration * p;

            p = external_declaration(ctx);
            if (p == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            declaration_list_add(&declaration_list, p);
        }
        check_unused_static_declarators(ctx, &declaration_list);
    }
    /*catch*/ else _CKL0:
    {
        *berror = 1;
    }
    return declaration_list;
}

struct declaration *external_declaration(struct parser_ctx * ctx)
{
    return function_definition_or_declaration(ctx);
}

struct label_list_item *label_list_find(struct label_list * list, char * label_name)
{
    struct label_list_item * item;

    item = list->head;
    while (item)
    {
        if (item->p_defined && strcmp(item->p_defined->lexeme, label_name) == 0)
        {
            return item;
        }
        else
        {
            if (item->p_last_usage && strcmp(item->p_last_usage->lexeme, label_name) == 0)
            {
                return item;
            }
        }
        item = item->next;
    }
    return 0U;
}

void label_list_swap(struct label_list * a, struct label_list * b)
{
    struct label_list  temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

void label_list_clear(struct label_list * list)
{
    struct label_list_item * item;

    item = list->head;
    while (item)
    {
        struct label_list_item * next;

        next = item->next;
        item->next = 0U;
        free(item);
        item = next;
    }
    list->head = 0U;
    list->tail = 0U;
}

void label_list_push(struct label_list * list, struct label_list_item * pitem)
{
    if (list->head == 0U)
    {
        list->head = pitem;
    }
    else
    {
        ;
        ;
        list->tail->next = pitem;
    }
    list->tail = pitem;
}

void check_labels(struct parser_ctx * ctx)
{
    struct label_list_item * item;

    item = ctx->label_list.head;
    while (item)
    {
        if (item->p_defined == 0U && item->p_last_usage != 0U)
        {
            compiler_diagnostic(1430, ctx, item->p_last_usage, 0U, "label '%s' used but not defined", item->p_last_usage->lexeme);
        }
        else
        {
            if (item->p_defined != 0U && item->p_last_usage == 0U)
            {
                compiler_diagnostic(54, ctx, item->p_defined, 0U, "label '%s' defined but not used", item->p_defined->lexeme);
            }
        }
        item = item->next;
    }
}

struct compound_statement *function_body(struct parser_ctx * ctx)
{
    struct try_statement * p_current_try_statement_opt;
    int label_id;
    struct defer_statement * p_current_defer_statement_opt;
    struct selection_statement * p_current_selection_statement;
    struct label_list  label_list;
    struct compound_statement * p_compound_statement;

    p_current_try_statement_opt = ctx->p_current_try_statement_opt;
    ctx->p_current_try_statement_opt = 0U;
    label_id = ctx->label_id;
    ctx->label_id = 0;
    p_current_defer_statement_opt = ctx->p_current_defer_statement_opt;
    ctx->p_current_defer_statement_opt = 0U;
    p_current_selection_statement = ctx->p_current_selection_statement;
    ctx->p_current_selection_statement = 0U;
    _cake_zmem(&label_list, 8);
    label_list_swap(&label_list, &ctx->label_list);
    p_compound_statement = compound_statement(ctx);
    if (p_compound_statement)
    {
        check_labels(ctx);
    }
    label_list_swap(&label_list, &ctx->label_list);
    ctx->label_id = label_id;
    ctx->p_current_try_statement_opt = p_current_try_statement_opt;
    ctx->p_current_defer_statement_opt = p_current_defer_statement_opt;
    ctx->p_current_selection_statement = p_current_selection_statement;
    label_list_clear(&label_list);
    return p_compound_statement;
}

struct declaration_list parse(struct parser_ctx * ctx, struct token_list * list, unsigned char  * berror)
{
    struct declaration_list  l;
    struct scope  file_scope;

    *berror = 0;
    s_anonymous_struct_count = 0;
    _cake_zmem(&l, 8);
    _cake_zmem(&file_scope, 36);
    /*try*/ if (1)
    {
        unsigned char   local_error;

        scope_list_push(&ctx->scopes, &file_scope);
        ctx->input_list = *list;
        ctx->current = ctx->input_list.head;
        parser_skip_blanks(ctx);
        local_error = 0;
        l = translation_unit(ctx, &local_error);
        if (local_error)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
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
        int i;
        i = 1;
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
                char buffer[200];
                char * p;
                struct tokenizer_ctx  tctx;
                struct token_list  l1;
                struct token_list  r;

                _cake_zmem(&buffer, 200);
                snprintf(buffer, 200U, "#define %s \n", argv[i] + 2);
                p = &buffer[7];
                while (*p)
                {
                    if (*p == 61)
                    {
                        *p = 32;
                        break;
                    }
                    p++;
                }
                _cake_zmem(&tctx, 696);
                l1 = tokenizer(&tctx, buffer, "", 0, 0);
                r = preprocessor(prectx, &l1, 0);
                token_list_destroy(&l1);
                token_list_destroy(&r);
                continue;
            }
        }
    }
    return 0;
}

unsigned long __stdcall GetEnvironmentVariableA(char * lpName, char * lpBuffer, unsigned long nSize);
char *__cdecl strcat(char * _Destination, char * _Source);

void append_msvc_include_dir(struct preprocessor_ctx * prectx)
{
    char env[2000];
    int n;

    _cake_zmem(&env, 2000);
    n = GetEnvironmentVariableA("INCLUDE", env, 2000U);
    if (n > 0)
    {
        char * p;

        p = env;
        for (; ; )
        {
            char filename_local[500];
            int count;

            if (*p == 0)
            {
                break;
            }
            _cake_zmem(&filename_local, 500);
            count = 0;
            while (*p != 0 && (*p != 59 && *p != 10))
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
            if (*p == 0)
            {
                break;
            }
            p++;
        }
    }
}

struct _iobuf *fopen(char * _FileName, char * _Mode);
int *__cdecl _errno(void);

int generate_config_file(char * configpath)
{
    struct _iobuf * outfile;
    int error;

    outfile = 0U;
    error = 0;
    /*try*/ if (1)
    {
        char env[2000];
        int n;
        char * p;

        outfile = fopen(configpath, "w");
        if (outfile == 0U)
        {
            printf("Cannot open the file '%s' for writing.\n", configpath);
            error = (*_errno());
            /*throw*/ goto _CKL0;
        }
        fprintf(outfile, "//This was generated by running cake -autoconfig \n");
        _cake_zmem(&env, 2000);
        n = GetEnvironmentVariableA("INCLUDE", env, 2000U);
        if (n <= 0)
        {
            printf("INCLUDE not found.\nPlease, run cake -autoconfig inside visual studio command prompty.\n");
            error = 1;
            /*throw*/ goto _CKL0;
        }
        fprintf(outfile, "//This file was generated reading the variable INCLUDE inside Visual Studio Command Prompt.\n");
        fprintf(outfile, "//echo %%INCLUDE%% \n");
        p = env;
        for (; ; )
        {
            char filename_local[500];
            int count;

            if (*p == 0)
            {
                break;
            }
            _cake_zmem(&filename_local, 500);
            count = 0;
            while (*p != 0 && (*p != 59 && *p != 10))
            {
                filename_local[count] = *p;
                p++;
                count++;
            }
            filename_local[count] = 0;
            if (count > 0)
            {
                char * pch;

                strcat(filename_local, "/");
                pch = filename_local;
                while (*pch)
                {
                    if (*pch == 92)
                    {
                        *pch = 47;
                    }
                    pch++;
                }
                fprintf(outfile, "#pragma dir \"%s\"\n", filename_local);
            }
            if (*p == 0)
            {
                break;
            }
            p++;
        }
    }
    /*catch*/ else _CKL0:
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
int __cdecl _mkdir(char * _Path);
char *basename(char * filename);
void print_tokens(struct token * p_token);
char *print_preprocessed_to_string2(struct token * p_token);
void d_visit(struct d_visit_ctx * ctx, struct osstream * oss);
void d_visit_ctx_destroy(struct d_visit_ctx * ctx);
char *get_posix_error_message(int error);
void ast_destroy(struct ast * ast);
void preprocessor_ctx_destroy(struct preprocessor_ctx * p);

int compile_one_file(char * file_name, struct options * options, char * out_file_name, int argc, char ** argv, struct report * report)
{
    struct preprocessor_ctx  prectx;
    struct ast  ast;
    char * s;
    struct parser_ctx  ctx;
    struct tokenizer_ctx  tctx;
    struct token_list  tokens;
    char * content;

    printf("%s\n", file_name);
    _cake_zmem(&prectx, 752);
    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx);
    if (include_config_header(&prectx, file_name) != 0)
    {
    }
    _cake_zmem(&ast, 16);
    s = 0U;
    _cake_zmem(&ctx, 760);
    _cake_zmem(&tctx, 696);
    _cake_zmem(&tokens, 8);
    ctx.options = *options;
    ctx.p_report = report;
    content = 0U;
    /*try*/ if (1)
    {
        if (fill_preprocessor_options(argc, argv, &prectx) != 0)
        {
            /*throw*/ goto _CKL1;
        }
        prectx.options = *options;
        append_msvc_include_dir(&prectx);
        content = read_file(file_name, 1);
        if (content == 0U)
        {
            report->error_count++;
            printf("file not found '%s'\n", file_name);
            /*throw*/ goto _CKL1;
        }
        if (options->sarif_output)
        {
            char sarif_file_name[260];

            _cake_zmem(&sarif_file_name, 260);
            if (options->sarifpath[0] != 0)
            {
                _mkdir(options->sarifpath);
                snprintf(sarif_file_name, 260U, "%s/%s.cake.sarif", options->sarifpath, basename(file_name));
            }
            else
            {
                snprintf(sarif_file_name, 260U, "%s.cake.sarif", file_name);
            }
            ctx.sarif_file = (struct _iobuf *)fopen(sarif_file_name, "w");
            if (ctx.sarif_file)
            {
                char * begin_sarif;

                begin_sarif = "{\n""  \"version\": \"2.1.0\",\n""  \"$schema\": \"https://schemastore.azurewebsites.net/schemas/json/sarif-2.1.0-rtm.5.json\",\n""  \"runs\": [\n""    {\n""      \"results\": [\n""\n";
                fprintf(ctx.sarif_file, "%s", begin_sarif);
            }
            else
            {
                report->error_count++;
                printf("cannot open Sarif output file '%s'\n", sarif_file_name);
                /*throw*/ goto _CKL1;
            }
        }
        tokens = tokenizer(&tctx, content, file_name, 0, 0);
        if (tctx.n_errors > 0)
        {
            /*throw*/ goto _CKL1;
        }
        if (options->dump_tokens)
        {
            print_tokens(tokens.head);
        }
        prectx.options.diagnostic_stack.stack[prectx.options.diagnostic_stack.top_index].notes |= 9223372036854775808ULL;
        ast.token_list = preprocessor(&prectx, &tokens, 0);
        report->warnings_count += prectx.n_warnings;
        report->error_count += prectx.n_errors;
        if (prectx.n_errors > 0)
        {
            /*throw*/ goto _CKL1;
        }
        if (options->dump_pptokens)
        {
            if (ast.token_list.head != 0U)
            {
                print_tokens(ast.token_list.head);
            }
        }
        if (options->preprocess_only)
        {
            char * s2;

            s2 = print_preprocessed_to_string2(ast.token_list.head);
            printf("%s", s2);
            free((void *)s2);
        }
        else
        {
            unsigned char   berror;

            berror = 0;
            ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
            if (berror || report->error_count > 0)
            {
                /*throw*/ goto _CKL1;
            }
            if (!options->no_output)
            {
                struct _iobuf * outfile;

                if (options->target == 0)
                {
                    struct osstream  ss;
                    struct d_visit_ctx  ctx2;

                    _cake_zmem(&ss, 12);
                    _cake_zmem(&ctx2, 164);
                    ctx2.ast = ast;
                    d_visit(&ctx2, &ss);
                    s = ss.c_str;
                    d_visit_ctx_destroy(&ctx2);
                }
                outfile = fopen(out_file_name, "w");
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
                    printf("cannot open output file '%s' - %s\n", out_file_name, get_posix_error_message((*_errno())));
                    /*throw*/ goto _CKL1;
                }
            }
        }
        if (ctx.sarif_file)
        {
            fprintf(ctx.sarif_file, "%s", "      ],\n""      \"tool\": {\n""        \"driver\": {\n""          \"name\": \"cake\",\n""          \"fullName\": \"cake code analysis\",\n""          \"version\": \"""0.10.25""\",\n""          \"informationUri\": \"https://https://github.com/thradams/cake\"\n""        }\n""      }\n""    }\n""  ]\n""}\n""\n");
            fclose(ctx.sarif_file);
            ctx.sarif_file = 0U;
        }
    }
    /*catch*/ else _CKL1:
    {
    }
    if (ctx.options.test_mode)
    {
        char buf[260];
        char * content_expected;

        _cake_zmem(&buf, 260);
        snprintf(buf, 260U, "%s_expected.c", file_name);
        content_expected = read_file(buf, 0);
        if (content_expected)
        {
            if (s && strcmp(content_expected, s) != 0)
            {
                printf("different");
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
            printf("\x1b[91m""TEST FAILED""\x1b[0m"" : error=%d, warnings=%d\n", report->error_count, report->warnings_count);
            printf("\n\n");
            report->test_failed++;
        }
        else
        {
            report->test_succeeded++;
            printf("\x1b[92m""TEST OK\n""\x1b[0m");
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
char *__cdecl strcpy(char * _Destination, char * _Source);
int closedir(struct TAGDIR * dirp);

static int compile_many_files(char * file_name, struct options * options, char * out_file_name, int argc, char ** argv, struct report * report)
{
    char * file_name_name;
    char * file_name_extension;
    int num_files;
    char path[260];
    struct TAGDIR * dir;
    struct dirent * dp;

    file_name_name = basename(file_name);
    file_name_extension = strrchr((char *)file_name_name, 46);
    if (file_name_extension == 0U)
    {
        ;
    }
    num_files = 0;
    _cake_zmem(&path, 260);
    snprintf(path, 260U, "%s", file_name);
    dirname(path);
    dir = opendir(path);
    if (dir == 0U)
    {
        return (*_errno());
    }
    while ((dp = readdir(dir)) != 0U)
    {
        char fromlocal[257];

        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }
        _cake_zmem(&fromlocal, 257);
        snprintf(fromlocal, 257U, "%s/%s", "", dp->d_name);
        if (dp->d_type & 4)
        {
        }
        else
        {
            char * file_name_iter;
            char * file_extension;

            file_name_iter = basename(dp->d_name);
            file_extension = strrchr((char *)file_name_iter, 46);
            if (file_name_extension && file_extension && strcmp(file_name_extension, file_extension) == 0)
            {
                char out_file_name_final[260];
                char in_file_name_final[260];
                struct report  report_local;

                _cake_zmem(&out_file_name_final, 260);
                strcpy(out_file_name_final, out_file_name);
                dirname(out_file_name_final);
                strcat(out_file_name_final, "/");
                strcat(out_file_name_final, file_name_iter);
                _cake_zmem(&in_file_name_final, 260);
                strcpy(in_file_name_final, file_name);
                dirname(in_file_name_final);
                strcat(in_file_name_final, "/");
                strcat(in_file_name_final, file_name_iter);
                _cake_zmem(&report_local, 56);
                report_local.test_mode = report->test_mode;
                compile_one_file(in_file_name_final, options, out_file_name_final, argc, argv, &report_local);
                report->fatal_error_expected = report_local.fatal_error_expected;
                report->error_count += report_local.error_count;
                report->warnings_count += report_local.warnings_count;
                report->info_count += report_local.info_count;
                report->test_succeeded += report_local.test_succeeded;
                report->test_failed += report_local.test_failed;
                num_files++;
            }
        }
    }
    closedir(dir);
    return num_files;
}

char *realpath(char * path, char * resolved_path);
char *__cdecl strncpy(char * _Destination, char * _Source, unsigned int _Count);

static void longest_common_path(int argc, char ** argv, char root_dir[260])
{
    {
        int i;
        i = 1;
        for (; i < argc; i++)
        {
            char fullpath_i[260];

            if (argv[i][0] == 45)
            {
                continue;
            }
            _cake_zmem(&fullpath_i, 260);
            realpath(argv[i], fullpath_i);
            strcpy(root_dir, fullpath_i);
            dirname(root_dir);
            {
                int k;
                k = 0;
                for (; k < 260; k++)
                {
                    char ch;

                    ch = fullpath_i[k];
                    {
                        int j;
                        j = 2;
                        for (; j < argc; j++)
                        {
                            char fullpath_j[260];

                            if (argv[j][0] == 45)
                            {
                                continue;
                            }
                            _cake_zmem(&fullpath_j, 260);
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

unsigned int __cdecl strlen(char * _Str);

static int create_multiple_paths(char * root, char * outdir)
{
    char * p;

    p = outdir + strlen(root) + 1;
    for (; ; )
    {
        char temp[260];
        int er;

        if (*p != 0 && *p != 47 && *p != 92)
        {
            p++;
            continue;
        }
        _cake_zmem(&temp, 260);
        strncpy(temp, outdir, p - outdir);
        er = _mkdir(temp);
        if (er != 0)
        {
            er = (*_errno());
            if (er != 17)
            {
                printf("error creating output folder '%s' - %s\n", temp, get_posix_error_message(er));
                return er;
            }
        }
        if (*p == 0)
        {
            break;
        }
        p++;
    }
    return 0;
}

int fill_options(struct options * options, int argc, char ** argv);
int get_self_path(char * buffer, int maxsize);
long __cdecl clock(void);

int compile(int argc, char ** argv, struct report * report)
{
    struct options  options;
    char executable_path[246];
    char cakeconfig_path[260];
    long begin_clock;
    int no_files;
    char root_dir[260];
    int root_dir_len;
    long end_clock;
    double cpu_time_used;

    _cake_zmem(&options, 688);
    if (fill_options(&options, argc, argv) != 0)
    {
        return 1;
    }
    _cake_zmem(&executable_path, 246);
    get_self_path(executable_path, 246U);
    dirname(executable_path);
    _cake_zmem(&cakeconfig_path, 260);
    snprintf(cakeconfig_path, 260U, "%s""/cakeconfig.h", executable_path);
    if (options.auto_config)
    {
        report->ignore_this_report = 1;
        return generate_config_file(cakeconfig_path);
    }
    report->test_mode = options.test_mode;
    begin_clock = clock();
    no_files = 0;
    _cake_zmem(&root_dir, 260);
    if (!options.no_output)
    {
        longest_common_path(argc, argv, root_dir);
    }
    root_dir_len = strlen(root_dir);
    {
        int i;
        i = 1;
        for (; i < argc; i++)
        {
            char output_file[260];
            char fullpath[260];
            char * file_extension;

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
            _cake_zmem(&output_file, 260);
            if (!options.no_output)
            {
                if (no_files == 1 && options.output[0] != 0)
                {
                    strcat(output_file, options.output);
                }
                else
                {
                    char fullpath[260];
                    char outdir[260];

                    _cake_zmem(&fullpath, 260);
                    realpath(argv[i], fullpath);
                    strcpy(output_file, root_dir);
                    strcat(output_file, "/out");
                    strcat(output_file, fullpath + root_dir_len);
                    _cake_zmem(&outdir, 260);
                    strcpy(outdir, output_file);
                    dirname(outdir);
                    if (create_multiple_paths(root_dir, outdir) != 0)
                    {
                        return 1;
                    }
                }
            }
            _cake_zmem(&fullpath, 260);
            realpath(argv[i], fullpath);
            file_extension = basename(fullpath);
            if (file_extension[0] == 42)
            {
                no_files--;
                no_files += compile_many_files(fullpath, &options, output_file, argc, argv, report);
            }
            else
            {
                struct report  report_local;

                _cake_zmem(&report_local, 56);
                compile_one_file(fullpath, &options, output_file, argc, argv, &report_local);
                report->fatal_error_expected = report_local.fatal_error_expected;
                report->error_count += report_local.error_count;
                report->warnings_count += report_local.warnings_count;
                report->info_count += report_local.info_count;
                report->test_succeeded += report_local.test_succeeded;
                report->test_failed += report_local.test_failed;
            }
        }
    }
    end_clock = clock();
    cpu_time_used = ((double)(end_clock - begin_clock)) / 1000L;
    report->no_files = no_files;
    report->cpu_time_used_sec = cpu_time_used;
    return 0;
}

struct ast get_ast(struct options * options, char * filename, char * source, struct report * report)
{
    struct ast  ast;
    struct tokenizer_ctx  tctx;
    struct token_list  list;
    struct preprocessor_ctx  prectx;
    struct parser_ctx  ctx;

    _cake_zmem(&ast, 16);
    _cake_zmem(&tctx, 696);
    list = tokenizer(&tctx, source, filename, 0, 0);
    _cake_zmem(&prectx, 752);
    _cake_zmem(&ctx, 760);
    ctx.p_report = report;
    /*try*/ if (1)
    {
        unsigned char   berror;

        prectx.options = *options;
        prectx.macros.capacity = 5000;
        add_standard_macros(&prectx);
        ast.token_list = preprocessor(&prectx, &list, 0);
        if (prectx.n_errors != 0)
        {
            /*throw*/ goto _CKL0;
        }
        ctx.options = *options;
        berror = 0;
        ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
        if (berror)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    parser_ctx_destroy(&ctx);
    token_list_destroy(&list);
    preprocessor_ctx_destroy(&prectx);
    return ast;
}

int strtoargv(char * s, int n, char *argv[])
{
    int argvc;
    char * p;

    argvc = 0;
    p = s;
    while (*p)
    {
        while (*p == 32)
        p++;
        if (*p == 0)
        {
            break;
        }
        argv[argvc] = p;
        argvc++;
        while (*p != 32 && *p != 0)
        p++;
        if (*p == 0)
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
    char *argv[100];
    char string[200];
    int argc;
    char * s;
    struct preprocessor_ctx  prectx;
    struct ast  ast;
    struct options  options;

    _cake_zmem(&argv, 400);
    _cake_zmem(&string, 200);
    snprintf(string, 200U, "exepath %s", pszoptions);
    argc = strtoargv(string, 10, argv);
    s = 0U;
    _cake_zmem(&prectx, 752);
    _cake_zmem(&ast, 16);
    options.input = 2;
    options.target = 0;
    options.diagnostic_stack.top_index = 0;
    options.diagnostic_stack.stack[0].errors = 0;
    options.diagnostic_stack.stack[0].warnings = 0;
    options.diagnostic_stack.stack[0].notes = 0;
    options.diagnostic_stack.stack[1].errors = 0;
    options.diagnostic_stack.stack[1].warnings = 0;
    options.diagnostic_stack.stack[1].notes = 0;
    options.diagnostic_stack.stack[2].errors = 0;
    options.diagnostic_stack.stack[2].warnings = 0;
    options.diagnostic_stack.stack[2].notes = 0;
    options.diagnostic_stack.stack[3].errors = 0;
    options.diagnostic_stack.stack[3].warnings = 0;
    options.diagnostic_stack.stack[3].notes = 0;
    options.diagnostic_stack.stack[4].errors = 0;
    options.diagnostic_stack.stack[4].warnings = 0;
    options.diagnostic_stack.stack[4].notes = 0;
    options.diagnostic_stack.stack[5].errors = 0;
    options.diagnostic_stack.stack[5].warnings = 0;
    options.diagnostic_stack.stack[5].notes = 0;
    options.diagnostic_stack.stack[6].errors = 0;
    options.diagnostic_stack.stack[6].warnings = 0;
    options.diagnostic_stack.stack[6].notes = 0;
    options.diagnostic_stack.stack[7].errors = 0;
    options.diagnostic_stack.stack[7].warnings = 0;
    options.diagnostic_stack.stack[7].notes = 0;
    options.diagnostic_stack.stack[8].errors = 0;
    options.diagnostic_stack.stack[8].warnings = 0;
    options.diagnostic_stack.stack[8].notes = 0;
    options.diagnostic_stack.stack[9].errors = 0;
    options.diagnostic_stack.stack[9].warnings = 0;
    options.diagnostic_stack.stack[9].notes = 0;
    options.style = 0;
    options.show_includes = 0;
    options.disable_assert = 0;
    options.flow_analysis = 0;
    options.test_mode = 0;
    options.null_checks_enabled = 0;
    options.ownership_enabled = 0;
    options.preprocess_only = 0;
    options.preprocess_def_macro = 0;
    options.clear_error_at_end = 0;
    options.sarif_output = 0;
    options.no_output = 0;
    options.const_literal = 0;
    options.visual_studio_ouput_format = 0;
    options.dump_tokens = 0;
    options.dump_pptokens = 0;
    options.auto_config = 0;
    options.do_static_debug = 0;
    options.static_debug_lines = 0;
    options.output[0] = 0;
    options.output[1] = 0;
    options.output[2] = 0;
    options.output[3] = 0;
    options.output[4] = 0;
    options.output[5] = 0;
    options.output[6] = 0;
    options.output[7] = 0;
    options.output[8] = 0;
    options.output[9] = 0;
    options.output[10] = 0;
    options.output[11] = 0;
    options.output[12] = 0;
    options.output[13] = 0;
    options.output[14] = 0;
    options.output[15] = 0;
    options.output[16] = 0;
    options.output[17] = 0;
    options.output[18] = 0;
    options.output[19] = 0;
    options.output[20] = 0;
    options.output[21] = 0;
    options.output[22] = 0;
    options.output[23] = 0;
    options.output[24] = 0;
    options.output[25] = 0;
    options.output[26] = 0;
    options.output[27] = 0;
    options.output[28] = 0;
    options.output[29] = 0;
    options.output[30] = 0;
    options.output[31] = 0;
    options.output[32] = 0;
    options.output[33] = 0;
    options.output[34] = 0;
    options.output[35] = 0;
    options.output[36] = 0;
    options.output[37] = 0;
    options.output[38] = 0;
    options.output[39] = 0;
    options.output[40] = 0;
    options.output[41] = 0;
    options.output[42] = 0;
    options.output[43] = 0;
    options.output[44] = 0;
    options.output[45] = 0;
    options.output[46] = 0;
    options.output[47] = 0;
    options.output[48] = 0;
    options.output[49] = 0;
    options.output[50] = 0;
    options.output[51] = 0;
    options.output[52] = 0;
    options.output[53] = 0;
    options.output[54] = 0;
    options.output[55] = 0;
    options.output[56] = 0;
    options.output[57] = 0;
    options.output[58] = 0;
    options.output[59] = 0;
    options.output[60] = 0;
    options.output[61] = 0;
    options.output[62] = 0;
    options.output[63] = 0;
    options.output[64] = 0;
    options.output[65] = 0;
    options.output[66] = 0;
    options.output[67] = 0;
    options.output[68] = 0;
    options.output[69] = 0;
    options.output[70] = 0;
    options.output[71] = 0;
    options.output[72] = 0;
    options.output[73] = 0;
    options.output[74] = 0;
    options.output[75] = 0;
    options.output[76] = 0;
    options.output[77] = 0;
    options.output[78] = 0;
    options.output[79] = 0;
    options.output[80] = 0;
    options.output[81] = 0;
    options.output[82] = 0;
    options.output[83] = 0;
    options.output[84] = 0;
    options.output[85] = 0;
    options.output[86] = 0;
    options.output[87] = 0;
    options.output[88] = 0;
    options.output[89] = 0;
    options.output[90] = 0;
    options.output[91] = 0;
    options.output[92] = 0;
    options.output[93] = 0;
    options.output[94] = 0;
    options.output[95] = 0;
    options.output[96] = 0;
    options.output[97] = 0;
    options.output[98] = 0;
    options.output[99] = 0;
    options.output[100] = 0;
    options.output[101] = 0;
    options.output[102] = 0;
    options.output[103] = 0;
    options.output[104] = 0;
    options.output[105] = 0;
    options.output[106] = 0;
    options.output[107] = 0;
    options.output[108] = 0;
    options.output[109] = 0;
    options.output[110] = 0;
    options.output[111] = 0;
    options.output[112] = 0;
    options.output[113] = 0;
    options.output[114] = 0;
    options.output[115] = 0;
    options.output[116] = 0;
    options.output[117] = 0;
    options.output[118] = 0;
    options.output[119] = 0;
    options.output[120] = 0;
    options.output[121] = 0;
    options.output[122] = 0;
    options.output[123] = 0;
    options.output[124] = 0;
    options.output[125] = 0;
    options.output[126] = 0;
    options.output[127] = 0;
    options.output[128] = 0;
    options.output[129] = 0;
    options.output[130] = 0;
    options.output[131] = 0;
    options.output[132] = 0;
    options.output[133] = 0;
    options.output[134] = 0;
    options.output[135] = 0;
    options.output[136] = 0;
    options.output[137] = 0;
    options.output[138] = 0;
    options.output[139] = 0;
    options.output[140] = 0;
    options.output[141] = 0;
    options.output[142] = 0;
    options.output[143] = 0;
    options.output[144] = 0;
    options.output[145] = 0;
    options.output[146] = 0;
    options.output[147] = 0;
    options.output[148] = 0;
    options.output[149] = 0;
    options.output[150] = 0;
    options.output[151] = 0;
    options.output[152] = 0;
    options.output[153] = 0;
    options.output[154] = 0;
    options.output[155] = 0;
    options.output[156] = 0;
    options.output[157] = 0;
    options.output[158] = 0;
    options.output[159] = 0;
    options.output[160] = 0;
    options.output[161] = 0;
    options.output[162] = 0;
    options.output[163] = 0;
    options.output[164] = 0;
    options.output[165] = 0;
    options.output[166] = 0;
    options.output[167] = 0;
    options.output[168] = 0;
    options.output[169] = 0;
    options.output[170] = 0;
    options.output[171] = 0;
    options.output[172] = 0;
    options.output[173] = 0;
    options.output[174] = 0;
    options.output[175] = 0;
    options.output[176] = 0;
    options.output[177] = 0;
    options.output[178] = 0;
    options.output[179] = 0;
    options.output[180] = 0;
    options.output[181] = 0;
    options.output[182] = 0;
    options.output[183] = 0;
    options.output[184] = 0;
    options.output[185] = 0;
    options.output[186] = 0;
    options.output[187] = 0;
    options.output[188] = 0;
    options.output[189] = 0;
    options.output[190] = 0;
    options.output[191] = 0;
    options.output[192] = 0;
    options.output[193] = 0;
    options.output[194] = 0;
    options.output[195] = 0;
    options.output[196] = 0;
    options.output[197] = 0;
    options.output[198] = 0;
    options.output[199] = 0;
    options.sarifpath[0] = 0;
    options.sarifpath[1] = 0;
    options.sarifpath[2] = 0;
    options.sarifpath[3] = 0;
    options.sarifpath[4] = 0;
    options.sarifpath[5] = 0;
    options.sarifpath[6] = 0;
    options.sarifpath[7] = 0;
    options.sarifpath[8] = 0;
    options.sarifpath[9] = 0;
    options.sarifpath[10] = 0;
    options.sarifpath[11] = 0;
    options.sarifpath[12] = 0;
    options.sarifpath[13] = 0;
    options.sarifpath[14] = 0;
    options.sarifpath[15] = 0;
    options.sarifpath[16] = 0;
    options.sarifpath[17] = 0;
    options.sarifpath[18] = 0;
    options.sarifpath[19] = 0;
    options.sarifpath[20] = 0;
    options.sarifpath[21] = 0;
    options.sarifpath[22] = 0;
    options.sarifpath[23] = 0;
    options.sarifpath[24] = 0;
    options.sarifpath[25] = 0;
    options.sarifpath[26] = 0;
    options.sarifpath[27] = 0;
    options.sarifpath[28] = 0;
    options.sarifpath[29] = 0;
    options.sarifpath[30] = 0;
    options.sarifpath[31] = 0;
    options.sarifpath[32] = 0;
    options.sarifpath[33] = 0;
    options.sarifpath[34] = 0;
    options.sarifpath[35] = 0;
    options.sarifpath[36] = 0;
    options.sarifpath[37] = 0;
    options.sarifpath[38] = 0;
    options.sarifpath[39] = 0;
    options.sarifpath[40] = 0;
    options.sarifpath[41] = 0;
    options.sarifpath[42] = 0;
    options.sarifpath[43] = 0;
    options.sarifpath[44] = 0;
    options.sarifpath[45] = 0;
    options.sarifpath[46] = 0;
    options.sarifpath[47] = 0;
    options.sarifpath[48] = 0;
    options.sarifpath[49] = 0;
    options.sarifpath[50] = 0;
    options.sarifpath[51] = 0;
    options.sarifpath[52] = 0;
    options.sarifpath[53] = 0;
    options.sarifpath[54] = 0;
    options.sarifpath[55] = 0;
    options.sarifpath[56] = 0;
    options.sarifpath[57] = 0;
    options.sarifpath[58] = 0;
    options.sarifpath[59] = 0;
    options.sarifpath[60] = 0;
    options.sarifpath[61] = 0;
    options.sarifpath[62] = 0;
    options.sarifpath[63] = 0;
    options.sarifpath[64] = 0;
    options.sarifpath[65] = 0;
    options.sarifpath[66] = 0;
    options.sarifpath[67] = 0;
    options.sarifpath[68] = 0;
    options.sarifpath[69] = 0;
    options.sarifpath[70] = 0;
    options.sarifpath[71] = 0;
    options.sarifpath[72] = 0;
    options.sarifpath[73] = 0;
    options.sarifpath[74] = 0;
    options.sarifpath[75] = 0;
    options.sarifpath[76] = 0;
    options.sarifpath[77] = 0;
    options.sarifpath[78] = 0;
    options.sarifpath[79] = 0;
    options.sarifpath[80] = 0;
    options.sarifpath[81] = 0;
    options.sarifpath[82] = 0;
    options.sarifpath[83] = 0;
    options.sarifpath[84] = 0;
    options.sarifpath[85] = 0;
    options.sarifpath[86] = 0;
    options.sarifpath[87] = 0;
    options.sarifpath[88] = 0;
    options.sarifpath[89] = 0;
    options.sarifpath[90] = 0;
    options.sarifpath[91] = 0;
    options.sarifpath[92] = 0;
    options.sarifpath[93] = 0;
    options.sarifpath[94] = 0;
    options.sarifpath[95] = 0;
    options.sarifpath[96] = 0;
    options.sarifpath[97] = 0;
    options.sarifpath[98] = 0;
    options.sarifpath[99] = 0;
    options.sarifpath[100] = 0;
    options.sarifpath[101] = 0;
    options.sarifpath[102] = 0;
    options.sarifpath[103] = 0;
    options.sarifpath[104] = 0;
    options.sarifpath[105] = 0;
    options.sarifpath[106] = 0;
    options.sarifpath[107] = 0;
    options.sarifpath[108] = 0;
    options.sarifpath[109] = 0;
    options.sarifpath[110] = 0;
    options.sarifpath[111] = 0;
    options.sarifpath[112] = 0;
    options.sarifpath[113] = 0;
    options.sarifpath[114] = 0;
    options.sarifpath[115] = 0;
    options.sarifpath[116] = 0;
    options.sarifpath[117] = 0;
    options.sarifpath[118] = 0;
    options.sarifpath[119] = 0;
    options.sarifpath[120] = 0;
    options.sarifpath[121] = 0;
    options.sarifpath[122] = 0;
    options.sarifpath[123] = 0;
    options.sarifpath[124] = 0;
    options.sarifpath[125] = 0;
    options.sarifpath[126] = 0;
    options.sarifpath[127] = 0;
    options.sarifpath[128] = 0;
    options.sarifpath[129] = 0;
    options.sarifpath[130] = 0;
    options.sarifpath[131] = 0;
    options.sarifpath[132] = 0;
    options.sarifpath[133] = 0;
    options.sarifpath[134] = 0;
    options.sarifpath[135] = 0;
    options.sarifpath[136] = 0;
    options.sarifpath[137] = 0;
    options.sarifpath[138] = 0;
    options.sarifpath[139] = 0;
    options.sarifpath[140] = 0;
    options.sarifpath[141] = 0;
    options.sarifpath[142] = 0;
    options.sarifpath[143] = 0;
    options.sarifpath[144] = 0;
    options.sarifpath[145] = 0;
    options.sarifpath[146] = 0;
    options.sarifpath[147] = 0;
    options.sarifpath[148] = 0;
    options.sarifpath[149] = 0;
    options.sarifpath[150] = 0;
    options.sarifpath[151] = 0;
    options.sarifpath[152] = 0;
    options.sarifpath[153] = 0;
    options.sarifpath[154] = 0;
    options.sarifpath[155] = 0;
    options.sarifpath[156] = 0;
    options.sarifpath[157] = 0;
    options.sarifpath[158] = 0;
    options.sarifpath[159] = 0;
    options.sarifpath[160] = 0;
    options.sarifpath[161] = 0;
    options.sarifpath[162] = 0;
    options.sarifpath[163] = 0;
    options.sarifpath[164] = 0;
    options.sarifpath[165] = 0;
    options.sarifpath[166] = 0;
    options.sarifpath[167] = 0;
    options.sarifpath[168] = 0;
    options.sarifpath[169] = 0;
    options.sarifpath[170] = 0;
    options.sarifpath[171] = 0;
    options.sarifpath[172] = 0;
    options.sarifpath[173] = 0;
    options.sarifpath[174] = 0;
    options.sarifpath[175] = 0;
    options.sarifpath[176] = 0;
    options.sarifpath[177] = 0;
    options.sarifpath[178] = 0;
    options.sarifpath[179] = 0;
    options.sarifpath[180] = 0;
    options.sarifpath[181] = 0;
    options.sarifpath[182] = 0;
    options.sarifpath[183] = 0;
    options.sarifpath[184] = 0;
    options.sarifpath[185] = 0;
    options.sarifpath[186] = 0;
    options.sarifpath[187] = 0;
    options.sarifpath[188] = 0;
    options.sarifpath[189] = 0;
    options.sarifpath[190] = 0;
    options.sarifpath[191] = 0;
    options.sarifpath[192] = 0;
    options.sarifpath[193] = 0;
    options.sarifpath[194] = 0;
    options.sarifpath[195] = 0;
    options.sarifpath[196] = 0;
    options.sarifpath[197] = 0;
    options.sarifpath[198] = 0;
    options.sarifpath[199] = 0;
    /*try*/ if (1)
    {
        if (fill_options(&options, argc, argv) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        prectx.options = options;
        add_standard_macros(&prectx);
        if (options.preprocess_only)
        {
            struct tokenizer_ctx  tctx;
            struct token_list  tokens;
            struct token_list  token_list;

            _cake_zmem(&tctx, 696);
            tokens = tokenizer(&tctx, content, "c:/main.c", 0, 0);
            token_list = preprocessor(&prectx, &tokens, 0);
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
                /*throw*/ goto _CKL0;
            }
            if (options.target == 0)
            {
                struct osstream  ss;
                struct d_visit_ctx  ctx2;

                _cake_zmem(&ss, 12);
                _cake_zmem(&ctx2, 164);
                ctx2.ast = ast;
                d_visit(&ctx2, &ss);
                s = ss.c_str;
                d_visit_ctx_destroy(&ctx2);
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    preprocessor_ctx_destroy(&prectx);
    ast_destroy(&ast);
    return s;
}

char *CompileText(char * pszoptions, char * content)
{
    struct report  report;

    printf("\x1b[97m""Cake ""0.10.25""\x1b[0m""\n");
    printf("\x1b[97m""cake %s main.c\n", pszoptions);
    _cake_zmem(&report, 56);
    return (char *)compile_source(pszoptions, content, &report);
}

void ast_destroy(struct ast * ast)
{
    token_list_destroy(&ast->token_list);
    declaration_list_destroy(&ast->declaration_list);
}

int __cdecl toupper(int _C);

static unsigned char  is_all_upper(char * text)
{
    char * p;

    p = text;
    while (*p)
    {
        if (*p != toupper(*p))
        {
            return 0;
        }
        p++;
    }
    return 1;
}

static unsigned char  is_snake_case(char * text)
{
    if (!(*text >= 97 && *text <= 122))
    {
        return 0;
    }
    while (*text)
    {
        if ((*text >= 97 && *text <= 122) || *text == 95 || (*text >= 48 && *text <= 57))
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
    if (!(*text >= 97 && *text <= 122))
    {
        return 0;
    }
    while (*text)
    {
        if ((*text >= 97 && *text <= 122) || (*text >= 65 && *text <= 90) || (*text >= 48 && *text <= 57))
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
    if (!(text[0] >= 65 && text[0] <= 90))
    {
        return 0;
    }
    while (*text)
    {
        if ((*text >= 97 && *text <= 122) || (*text >= 65 && *text <= 90) || (*text >= 48 && *text <= 57))
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
    if (!parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(10, ctx, token, 0U, "use snake_case for struct/union tags");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if (!is_pascal_case(token->lexeme))
            {
                compiler_diagnostic(10, ctx, token, 0U, "use camelCase for struct/union tags");
            }
        }
    }
}

void naming_convention_enum_tag(struct parser_ctx * ctx, struct token * token)
{
    if (!parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(10, ctx, token, 0U, "use snake_case for enum tags");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if (!is_pascal_case(token->lexeme))
            {
                compiler_diagnostic(10, ctx, token, 0U, "use PascalCase for enum tags");
            }
        }
    }
}

void naming_convention_function(struct parser_ctx * ctx, struct token * token)
{
    if (!parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(10, ctx, token, 0U, "use snake_case for functions");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if (!is_pascal_case(token->lexeme))
            {
                compiler_diagnostic(10, ctx, token, 0U, "use PascalCase for functions");
            }
        }
    }
}

unsigned char  type_is_function_or_function_pointer(struct type * p_type);

void naming_convention_global_var(struct parser_ctx * ctx, struct token * token, struct type * type, int   storage)
{
    if (!parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (!type_is_function_or_function_pointer(type))
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
                    compiler_diagnostic(10, ctx, token, 0U, "use prefix s_ for static global variables");
                }
            }
        }
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(10, ctx, token, 0U, "use snake_case global variables");
        }
    }
}

void naming_convention_local_var(struct parser_ctx * ctx, struct token * token, struct type * type)
{
    if (!parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(10, ctx, token, 0U, "use snake_case for local variables");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if (!is_camel_case(token->lexeme))
            {
                compiler_diagnostic(10, ctx, token, 0U, "use camelCase for local variables");
            }
        }
    }
}

void naming_convention_enumerator(struct parser_ctx * ctx, struct token * token)
{
    if (!parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (!is_all_upper(token->lexeme))
    {
        compiler_diagnostic(10, ctx, token, 0U, "use UPPERCASE for enumerators");
    }
}

void naming_convention_struct_member(struct parser_ctx * ctx, struct token * token, struct type * type)
{
    if (!parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (!is_snake_case(token->lexeme))
    {
        compiler_diagnostic(10, ctx, token, 0U, "use snake_case for struct members");
    }
}

void naming_convention_parameter(struct parser_ctx * ctx, struct token * token, struct type * type)
{
    if (!parser_is_diagnostic_enabled(ctx, 10) || token->level != 0)
    {
        return;
    }
    if (!is_snake_case(token->lexeme))
    {
        compiler_diagnostic(10, ctx, token, 0U, "use snake_case for arguments");
    }
}

unsigned char  type_is_union(struct type * p_type);

static struct object *find_first_subobject_old(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out, unsigned char  * sub_object_of_union)
{
    struct type  t;

    p_object = (struct object *)object_get_referenced(p_object);
    if (p_object->members == 0U)
    {
        struct type  t;

        *sub_object_of_union = 0;
        t = type_dup(&p_object->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
        return p_object;
    }
    *sub_object_of_union = type_is_union(&p_object->type);
    t = type_dup(&p_object->members->type);
    type_swap(&t, p_type_out);
    type_destroy(&t);
    return p_object->members;
}

static struct object *find_first_subobject(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out, unsigned char  * sub_object_of_union)
{
    return find_first_subobject_old(p_type_not_used, p_object, p_type_out, sub_object_of_union);
}

static struct object *find_last_suboject_of_suboject_old(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out)
{
    struct object * it;
    struct type  t;

    p_object = (struct object *)object_get_referenced(p_object);
    if (p_object->members == 0U)
    {
        struct type  t;

        t = type_dup(&p_object->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
        return p_object;
    }
    it = p_object->members;
    while (it)
    {
        if (it->next == 0U)
        {
            return find_last_suboject_of_suboject_old(p_type_not_used, it, p_type_out);
        }
        it = it->next;
    }
    t = type_dup(&p_object->type);
    type_swap(&t, p_type_out);
    type_destroy(&t);
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
    if (it == 0U)
    {
        return 0U;
    }
    if (it->members)
    {
        struct type  t;

        *sub_object_of_union = type_is_union(&it->type);
        it = it->members;
        t = type_dup(&it->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
        return it;
    }
    for (; ; )
    {
        struct object * next;

        if (it == 0U)
        {
            break;
        }
        next = it->next;
        if (next != 0U)
        {
            if (it->parent)
            {
                *sub_object_of_union = type_is_union(&it->parent->type);
            }
            it = next;
            break;
        }
        it = it->parent;
    }
    if (it != 0U)
    {
        struct type  t;

        t = type_dup(&it->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
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
    /*try*/ if (1)
    {
        if (type_is_scalar(p_type))
        {
            if (result->object != 0U)
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
            struct type  item_type;
            struct object * it;

            item_type = get_array_item_type(p_type);
            it = obj->members;
            for (; it; it = it->next)
            {
                if (find_next_subobject_core(&item_type, it, subobj, result))
                {
                    type_destroy(&item_type);
                    return 1;
                }
            }
            type_destroy(&item_type);
            return 0;
        }
        if (p_type->struct_or_union_specifier)
        {
            struct struct_or_union_specifier * p_struct_or_union_specifier;
            struct member_declaration * p_member_declaration;
            struct object * member_object;

            p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
            if (p_struct_or_union_specifier == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (subobj == obj)
            {
                result->object = obj;
            }
            p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            member_object = obj->members;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator * p_member_declarator;

                    p_member_declarator = p_member_declaration->member_declarator_list_opt->head;
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
                    if (p_member_declaration->specifier_qualifier_list != 0U)
                    {
                        if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                        {
                            struct type  t;

                            _cake_zmem(&t, 60);
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
    /*catch*/ else _CKL0:
    {
    }
    return 0;
}

unsigned char  type_is_struct_or_union(struct type * p_type);
signed long long object_to_signed_long_long(struct object * a);
struct object *object_extend_array_to_index(struct type * p_type, struct object * a, int n, unsigned char   is_constant);
struct object *object_get_member(struct object * p_object, int index);

static struct object *find_designated_subobject(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * current_object, struct designator * p_designator, unsigned char   is_constant, struct type * p_type_out2, unsigned char   not_error)
{
    /*try*/ if (1)
    {
        if (type_is_struct_or_union(p_current_object_type))
        {
            struct struct_or_union_specifier * p_struct_or_union_specifier;
            struct member_declaration * p_member_declaration;
            struct member_declarator * p_member_declarator;
            char * name;
            struct object * p_member_object;

            ;
            p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_current_object_type->struct_or_union_specifier);
            if (p_struct_or_union_specifier == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            p_member_declarator = 0U;
            name = p_designator->token->lexeme;
            p_member_object = current_object->members;
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
                                if (p_designator->next != 0U)
                                {
                                    return find_designated_subobject(ctx, &p_member_declarator->declarator->type, p_member_object, p_designator->next, is_constant, p_type_out2, 0);
                                }
                                else
                                {
                                    struct type  t;

                                    t = type_dup(&p_member_declarator->declarator->type);
                                    type_swap(&t, p_type_out2);
                                    type_destroy(&t);
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
                    if (p_member_declaration->specifier_qualifier_list)
                    {
                        if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                        {
                            struct struct_or_union_specifier * p_complete;

                            p_complete = get_complete_struct_or_union_specifier(p_member_declaration->specifier_qualifier_list->struct_or_union_specifier);
                            if (p_complete)
                            {
                                struct type  t;
                                struct object * p;

                                _cake_zmem(&t, 60);
                                t.category = 0;
                                t.struct_or_union_specifier = p_complete;
                                t.type_specifier_flags = 32768;
                                p = find_designated_subobject(ctx, &t, p_member_object, p_designator, is_constant, p_type_out2, 1);
                                if (p)
                                {
                                    type_destroy(&t);
                                    return p;
                                }
                                p_member_object = p_member_object->next;
                                type_destroy(&t);
                            }
                        }
                    }
                    else
                    {
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            if (!not_error)
            {
                compiler_diagnostic(720, ctx, p_designator->token, 0U, "member '%s' not found in '%s'", name, p_struct_or_union_specifier->tag_name);
            }
            return 0U;
        }
        else
        {
            if (type_is_array(p_current_object_type))
            {
                unsigned char   compute_array_size;
                long long index;
                int max_index;
                struct type  array_item_type;
                struct object * member_obj;

                compute_array_size = !!(p_current_object_type->array_num_elements_expression == 0U);
                index = -1;
                max_index = -1;
                array_item_type = get_array_item_type(p_current_object_type);
                member_obj = current_object->members;
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
                    if (member_obj == 0U)
                    {
                        if (index < 0)
                        {
                            compiler_diagnostic(720, ctx, p_designator->constant_expression_opt->first_token, 0U, "array designator value '%d' is negative", index);
                        }
                        else
                        {
                            if (index > p_current_object_type->num_of_elements)
                            {
                                compiler_diagnostic(720, ctx, p_designator->constant_expression_opt->first_token, 0U, "array index '%d' in initializer exceeds array bounds", index);
                            }
                        }
                        type_destroy(&array_item_type);
                        return 0U;
                    }
                    if (p_designator->next != 0U)
                    {
                        struct object * p;

                        p = find_designated_subobject(ctx, &array_item_type, member_obj, p_designator->next, is_constant, p_type_out2, 0);
                        type_destroy(&array_item_type);
                        return p;
                    }
                    else
                    {
                        type_swap(p_type_out2, &array_item_type);
                        type_destroy(&array_item_type);
                    }
                    return member_obj;
                }
                else
                {
                }
                type_destroy(&array_item_type);
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    return 0U;
}

static struct initializer_list_item *find_innner_initializer_list_item(struct braced_initializer * braced_initializer)
{
    struct initializer_list_item * p_initializer_list_item;

    ;
    p_initializer_list_item = braced_initializer->initializer_list->head;
    while (p_initializer_list_item->initializer->braced_initializer)
    {
        p_initializer_list_item = p_initializer_list_item->initializer->braced_initializer->initializer_list->head;
        if (p_initializer_list_item == 0U)
        {
            ;
            return 0U;
        }
        if (p_initializer_list_item->next == 0U)
        {
            return p_initializer_list_item;
        }
        p_initializer_list_item = p_initializer_list_item->next;
    }
    return p_initializer_list_item;
}

void object_default_initialization(struct object * p_object, unsigned char   is_constant);
int object_set(struct parser_ctx * ctx, struct object * to, struct expression * init_expression, struct object * from, unsigned char   is_constant, unsigned char   requires_constant_initialization);
unsigned char  type_is_char(struct type * p_type);
unsigned char  type_is_array_of_char(struct type * p_type);

static int braced_initializer_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * current_object, struct braced_initializer * braced_initializer, unsigned char   is_constant, unsigned char   requires_constant_initialization)
{
    /*try*/ if (1)
    {
        struct object * parent_copy;
        struct initializer_list_item * p_initializer_list_item;
        long long array_to_expand_index;
        int array_to_expand_max_index;
        unsigned char   compute_array_size;
        struct type  array_item_type;
        struct object * p_subobject;

        if (braced_initializer->initializer_list == 0U)
        {
            object_default_initialization(current_object, is_constant);
            return 0;
        }
        if (!type_is_union(p_current_object_type))
        {
            object_default_initialization(current_object, is_constant);
        }
        if (type_is_scalar(p_current_object_type))
        {
            struct initializer_list_item * p_initializer_list_item;

            p_initializer_list_item = find_innner_initializer_list_item(braced_initializer);
            if (p_initializer_list_item == 0U)
            {
                return 0;
            }
            if (p_initializer_list_item->initializer->assignment_expression != 0U)
            {
                if (object_set(ctx, current_object, p_initializer_list_item->initializer->assignment_expression, &p_initializer_list_item->initializer->assignment_expression->object, is_constant, requires_constant_initialization) != 0)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            p_initializer_list_item = p_initializer_list_item->next;
            if (p_initializer_list_item != 0U)
            {
                compiler_diagnostic(100, ctx, p_initializer_list_item->initializer->first_token, 0U, "warning: excess elements in initializer");
            }
            return 0;
        }
        parent_copy = current_object->parent;
        current_object->parent = 0U;
        p_initializer_list_item = braced_initializer->initializer_list->head;
        array_to_expand_index = -1;
        array_to_expand_max_index = -1;
        compute_array_size = 0;
        _cake_zmem(&array_item_type, 60);
        if (type_is_array(p_current_object_type))
        {
            array_item_type = get_array_item_type(p_current_object_type);
            compute_array_size = !!(p_current_object_type->array_num_elements_expression == 0U);
            if (type_is_char(&array_item_type))
            {
                struct initializer_list_item * p_initializer_list_item2;

                p_initializer_list_item2 = find_innner_initializer_list_item(braced_initializer);
                if (p_initializer_list_item2 == 0U)
                {
                    type_destroy(&array_item_type);
                    return 0;
                }
                if (p_initializer_list_item2->initializer->assignment_expression != 0U)
                {
                    if (p_initializer_list_item2->initializer->assignment_expression->expression_type == 3)
                    {
                        unsigned long long num_of_elements;

                        num_of_elements = p_initializer_list_item2->initializer->assignment_expression->type.num_of_elements;
                        if (compute_array_size)
                        {
                            object_extend_array_to_index(&array_item_type, current_object, num_of_elements - 1, is_constant);
                        }
                        if (object_set(ctx, current_object, p_initializer_list_item2->initializer->assignment_expression, &p_initializer_list_item2->initializer->assignment_expression->object, is_constant, requires_constant_initialization) != 0)
                        {
                            type_destroy(&array_item_type);
                            /*throw*/ goto _CKL0;
                        }
                        p_current_object_type->num_of_elements = num_of_elements;
                        type_destroy(&array_item_type);
                        return 0;
                    }
                }
            }
        }
        p_subobject = 0U;
        for (; ; )
        {
            unsigned char   is_subobject_of_union;
            struct type  subobject_type;

            is_subobject_of_union = 0;
            _cake_zmem(&subobject_type, 60);
            if (p_initializer_list_item == 0U)
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
                p_subobject = find_designated_subobject(ctx, p_current_object_type, current_object, p_initializer_list_item->designation->designator_list->head, is_constant, &subobject_type, 0);
                if (p_subobject == 0U)
                {
                    p_initializer_list_item = p_initializer_list_item->next;
                    type_destroy(&subobject_type);
                    break;
                }
            }
            else
            {
                if (compute_array_size)
                {
                    struct object * po;

                    po = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                    if (po == 0U)
                    {
                        array_to_expand_index++;
                        if (array_to_expand_index > array_to_expand_max_index)
                        {
                            array_to_expand_max_index = array_to_expand_index;
                        }
                        object_extend_array_to_index(&array_item_type, current_object, array_to_expand_max_index, is_constant);
                    }
                }
                if (p_subobject == 0U)
                {
                    p_subobject = find_first_subobject(p_current_object_type, current_object, &subobject_type, &is_subobject_of_union);
                }
                else
                {
                    p_subobject = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                }
            }
            if (p_subobject == 0U)
            {
                type_destroy(&subobject_type);
                break;
            }
            if (p_initializer_list_item->initializer->braced_initializer)
            {
                struct type  t;

                if (braced_initializer_new(ctx, &subobject_type, p_subobject, p_initializer_list_item->initializer->braced_initializer, is_constant, requires_constant_initialization) != 0)
                {
                    type_destroy(&array_item_type);
                    type_destroy(&subobject_type);
                    /*throw*/ goto _CKL0;
                }
                _cake_zmem(&t, 60);
                is_subobject_of_union = type_is_union(&subobject_type);
                p_subobject = find_last_suboject_of_suboject(&subobject_type, p_subobject, &t);
                type_swap(&t, &subobject_type);
                type_destroy(&t);
            }
            else
            {
                if (p_initializer_list_item->initializer->assignment_expression)
                {
                    unsigned char   entire_object_initialized;

                    entire_object_initialized = 0;
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
                    if (object_set(ctx, p_subobject, p_initializer_list_item->initializer->assignment_expression, &p_initializer_list_item->initializer->assignment_expression->object, is_constant, requires_constant_initialization) != 0)
                    {
                        type_destroy(&array_item_type);
                        type_destroy(&subobject_type);
                        /*throw*/ goto _CKL0;
                    }
                    if (is_subobject_of_union)
                    {
                        struct type  t;

                        ;
                        ;
                        _cake_zmem(&t, 60);
                        is_subobject_of_union = 1;
                        p_subobject = find_last_suboject_of_suboject(&p_subobject->parent->type, p_subobject->parent, &t);
                        type_swap(&t, &subobject_type);
                        type_destroy(&t);
                        if (p_subobject)
                        {
                            type_destroy(&subobject_type);
                            subobject_type = type_dup(&p_subobject->type);
                        }
                    }
                    else
                    {
                        if (entire_object_initialized)
                        {
                            struct type  t;

                            ;
                            _cake_zmem(&t, 60);
                            is_subobject_of_union = type_is_union(p_current_object_type);
                            p_subobject = find_last_suboject_of_suboject(&subobject_type, p_subobject, &t);
                            type_swap(&t, &subobject_type);
                            type_destroy(&t);
                            if (p_subobject)
                            {
                                type_destroy(&subobject_type);
                                subobject_type = type_dup(&p_subobject->type);
                            }
                        }
                    }
                }
            }
            p_initializer_list_item = p_initializer_list_item->next;
            type_destroy(&subobject_type);
        }
        if (p_initializer_list_item != 0U)
        {
            compiler_diagnostic(100, ctx, p_initializer_list_item->initializer->first_token, 0U, "warning: excess elements in initializer");
        }
        if (compute_array_size)
        {
            current_object->type.num_of_elements = array_to_expand_max_index + 1;
            p_current_object_type->num_of_elements = array_to_expand_max_index + 1;
        }
        current_object->parent = parent_copy;
        type_destroy(&array_item_type);
    }
    /*catch*/ else _CKL0:
    {
        return 1;
    }
    return 0;
}

int initializer_init_new(struct parser_ctx * ctx, struct type * p_type, struct object * object, struct initializer * initializer, unsigned char   is_constant, unsigned char   requires_constant_initialization)
{
    /*try*/ if (1)
    {
        if (initializer->assignment_expression != 0U)
        {
            if (object_set(ctx, object, initializer->assignment_expression, &initializer->assignment_expression->object, is_constant, requires_constant_initialization) != 0)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            if (initializer->braced_initializer)
            {
                if (braced_initializer_new(ctx, p_type, object, initializer->braced_initializer, is_constant, requires_constant_initialization) != 0)
                {
                    /*throw*/ goto _CKL0;
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        return 1;
    }
    return 0;
}

