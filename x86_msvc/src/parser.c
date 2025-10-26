/* Cake 0.12.26 x86_msvc */
struct token;

struct token_list {
    struct token * head;
    struct token * tail;
};

struct label_state {
    char * label_name;
    int state_number;
};

struct declaration;

struct declaration_list {
    struct declaration * head;
    struct declaration * tail;
};

struct ast {
    struct token_list  token_list;
    struct declaration_list  declaration_list;
};

struct flow_object;

struct flow_objects {
    struct flow_object ** data;
    int size;
    int capacity;
};

struct secondary_block;
struct parser_ctx;
struct type;

struct flow_visit_ctx {
    struct secondary_block * catch_secondary_block_opt;
    struct parser_ctx * ctx;
    struct ast  ast;
    struct type * p_return_type;
    int parameter_list;
    int state_number_generator;
    unsigned char  expression_is_not_evaluated;
    unsigned char  inside_assert;
    unsigned char  inside_contract;
    unsigned char  inside_loop;
    int throw_join_state;
    int break_join_state;
    int initial_state;
    struct flow_objects  arena;
    struct label_state labels[100];
    int labels_size;
};

struct expression_statement;
struct simple_declaration;

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct storage_class_specifier {
    int flags;
    struct token * token;
};

struct type_name;

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct _iobuf {
    void * _Placeholder;
};

struct expression;
struct attribute_specifier_sequence;
struct declaration_specifiers;
struct init_declarator;

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct generic_association;

struct generic_assoc_list {
    struct generic_association * head;
    struct generic_association * tail;
};

struct param;

struct param_list {
    unsigned char  is_var_args;
    unsigned char  is_void;
    struct param * head;
    struct param * tail;
};

struct struct_or_union_specifier;
struct enum_specifier;

struct type {
    int category;
    int attributes_flags;
    int msvc_declspec_flags;
    int alignment_specifier_flags;
    int type_specifier_flags;
    int type_qualifier_flags;
    int storage_class_specifier_flags;
    char * name_opt;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct enum_specifier * enum_specifier;
    struct expression * array_num_elements_expression;
    unsigned int num_of_elements;
    unsigned char  has_static_array_size;
    unsigned char  address_of;
    struct param_list  params;
    struct type * next;
};

struct specifier_qualifier_list;
struct declarator;

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
};

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
};

struct defer_statement;

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct statement;

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct platform {
    char * name;
    char * thread_local_attr;
    char * alignas_fmt_must_have_one_percent_d;
    int bool_n_bits;
    int bool_aligment;
    int bool_type;
    int char_n_bits;
    int char_t_type;
    int char_aligment;
    int short_n_bits;
    int short_aligment;
    int int_n_bits;
    int int_aligment;
    int long_n_bits;
    int long_aligment;
    int long_long_n_bits;
    int long_long_aligment;
    int float_n_bits;
    int float_aligment;
    int double_n_bits;
    int double_aligment;
    int long_double_n_bits;
    int long_double_aligment;
    int pointer_n_bits;
    int pointer_aligment;
    int wchar_t_type;
    int int8_type;
    int int16_type;
    int int32_type;
    int int64_type;
    int size_t_type;
    int ptrdiff_type;
};

struct map_entry;

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct enumerator;
struct macro;
struct struct_entry;

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

struct diagnostic_stack {
    int top_index;
    struct diagnostic stack[10];
};

struct parameter_list;

struct parameter_type_list {
    unsigned char  is_var_args;
    unsigned char  is_void;
    struct parameter_list * parameter_list;
};

struct type_qualifier_list;

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
};

struct label_list_item;

struct label_list {
    struct label_list_item * head;
    struct label_list_item * tail;
};

struct generic_selection {
    struct expression * expression;
    struct type_name * type_name;
    struct expression * p_view_selected_expression;
    struct generic_assoc_list  generic_assoc_list;
    struct token * first_token;
    struct token * last_token;
};

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct enum_specifier {
    unsigned char  has_shared_ownership;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct specifier_qualifier_list * specifier_qualifier_list;
    char tag_name[200];
    struct enumerator_list  enumerator_list;
    struct token * tag_token;
    struct token * first_token;
    struct enum_specifier * p_complete_enum_specifier;
};

struct type_qualifier {
    int flags;
    struct token * token;
    struct type_qualifier * next;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct parameter_declaration;

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct label;

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

struct iteration_statement;

struct break_reference {
    struct selection_statement * p_selection_statement;
    struct iteration_statement * p_iteration_statement;
};

struct include_dir {
    char * path;
    struct include_dir * next;
};

struct type_specifier_qualifier;
struct function_specifier;
struct alignment_specifier;

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct alignment_specifier * alignment_specifier;
    struct declaration_specifier * next;
};

struct initializer;

struct init_declarator {
    unsigned char  has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct balanced_token_sequence;

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};

struct diagnostic_id_stack {
    int size;
    int stack[10];
};

struct options {
    int input;
    int target;
    struct diagnostic_stack  diagnostic_stack;
    int style;
    unsigned char  show_includes;
    unsigned char  disable_assert;
    unsigned char  flow_analysis;
    unsigned char  test_mode;
    unsigned char  null_checks_enabled;
    unsigned char  ownership_enabled;
    unsigned char  preprocess_only;
    unsigned char  preprocess_def_macro;
    unsigned char  clear_error_at_end;
    unsigned char  sarif_output;
    unsigned char  no_output;
    unsigned char  const_literal;
    unsigned char  visual_studio_ouput_format;
    unsigned char  disable_colors;
    unsigned char  dump_tokens;
    unsigned char  dump_pptokens;
    unsigned char  auto_config;
    unsigned char  do_static_debug;
    int static_debug_lines;
    char output[200];
    char sarifpath[200];
};

union __tag17 {
    signed long long host_long_long;
    unsigned long long host_u_long_long;
    float host_float;
    double host_double;
    long double long_double_val;
};

struct object;

struct object_list {
    struct object * head;
    struct object * tail;
    unsigned int count;
};

struct object {
    int state;
    int value_type;
    struct type  type;
    char * member_designator;
    union __tag17  value;
    struct object * parent;
    struct object * p_ref;
    struct expression * p_init_expression;
    struct object_list  members;
    struct object * next;
};

struct attribute_list;

struct attribute_specifier {
    int ack;
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct label {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct expression * constant_expression;
    struct expression * constant_expression_end;
    struct token * p_identifier_opt;
    struct token * p_first_token;
    struct label * next;
    int label_id;
};

struct member_declarator_list;
struct static_assert_declaration;

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct member_declaration * next;
};

struct attribute {
    int msvc_declspec_flags;
    int attributes_flags;
    struct attribute_argument_clause * attribute_argument_clause;
    struct token * attribute_token;
    struct attribute * next;
};

struct defer_defer_scope;

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct argument_expression;

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
};

struct typeof_specifier;

struct type_specifier {
    int flags;
    struct token * token;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct typeof_specifier * typeof_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct atomic_type_specifier * atomic_type_specifier;
};

struct param {
    struct type  type;
    struct param * next;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct braced_initializer;

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct unlabeled_statement;

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
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
    int state;
    struct flow_objects_view  alternatives;
    struct flow_object_state * next;
};

struct attribute_list {
    int attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct flow_object {
    unsigned int visit_number;
    struct flow_object * parent;
    struct declarator * p_declarator_origin;
    struct expression * p_expression_origin;
    struct flow_objects_view  members;
    struct flow_object_state  current;
    int id;
    unsigned char  is_temporary;
};

struct scope {
    int scope_level;
    struct hash_map  tags;
    struct hash_map  variables;
    struct scope * next;
    struct scope * previous;
};

struct struct_entry;

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct macro;

struct array_declarator;
struct function_declarator;

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
};

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
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

struct specifier_qualifier_list {
    int type_specifier_flags;
    int type_qualifier_flags;
    int alignment_specifier_flags;
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

struct init_declarator_list {
    struct init_declarator * head;
    struct init_declarator * tail;
};

struct compound_statement;

struct declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
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

struct block_item;

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct token {
    int type;
    char * lexeme;
    char * original;
    int line;
    int col;
    int level;
    int flags;
    struct token * token_origin;
    struct token * next;
    struct token * prev;
};

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct report;

struct parser_ctx {
    struct options  options;
    struct diagnostic_id_stack * p_diagnostic_id_stack;
    struct scope_list  scopes;
    struct declarator * p_current_function_opt;
    struct scope * p_current_function_scope_opt;
    struct label_list  label_list;
    struct try_statement * p_current_try_statement_opt;
    struct defer_statement * p_current_defer_statement_opt;
    struct selection_statement * p_current_selection_statement;
    struct _iobuf * sarif_file;
    unsigned int sarif_entries;
    struct token_list  input_list;
    struct token * current;
    struct token * previous;
    unsigned char  inside_generic_association;
    int label_id;
    unsigned int unique_tag_id;
    int anonymous_struct_count;
    struct report * p_report;
};

struct typeof_specifier_argument;

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
};

struct function_specifier {
    int flags;
    struct token * token;
};

struct alignment_specifier {
    int flags;
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct initializer_list;

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char  set_unkown;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
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

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int msvc_declspec_flags;
    int attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct designator_list;

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct type_qualifier_list {
    int flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct label_list_item {
    struct token * p_last_usage;
    struct token * p_defined;
    struct label_list_item * next;
};

struct report {
    int no_files;
    double cpu_time_used_sec;
    int error_count;
    int warnings_count;
    int info_count;
    unsigned char  test_mode;
    int test_failed;
    int test_succeeded;
    unsigned char  ignore_this_report;
};

struct expression {
    int expression_type;
    struct type  type;
    struct object  object;
    struct type_name * type_name;
    struct braced_initializer * braced_initializer;
    struct compound_statement * compound_statement;
    struct generic_selection * generic_selection;
    struct token * first_token;
    struct token * last_token;
    struct token * offsetof_member_designator;
    struct declarator * declarator;
    struct init_declarator * p_init_declarator;
    int member_index;
    struct argument_expression_list  argument_expression_list;
    struct expression * condition_expr;
    struct expression * left;
    struct expression * right;
    unsigned char  is_assignment_expression;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct struct_or_union_specifier {
    unsigned char  has_shared_ownership;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct member_declaration_list  member_declaration_list;
    struct token * first_token;
    struct token * last_token;
    unsigned char  is_owner;
    struct token * tagtoken;
    unsigned int unique_id;
    char tag_name[200];
    unsigned char  has_anonymous_tag;
    unsigned char  show_anonymous_tag;
    int scope_level;
    int visit_moved;
    struct struct_or_union_specifier * complete_struct_or_union_specifier_indirection;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct jump_statement;

struct unlabeled_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct expression * expression_opt;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
    struct defer_list  defer_list;
};

union __tag5 {
    unsigned int number;
    struct enum_specifier * p_enum_specifier;
    struct enumerator * p_enumerator;
    struct struct_or_union_specifier * p_struct_or_union_specifier;
    struct declarator * p_declarator;
    struct init_declarator * p_init_declarator;
    struct macro * p_macro;
    struct struct_entry * p_struct_entry;
};

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int type;
    union __tag5  data;
};

struct declaration_specifiers {
    int attributes_flags;
    int msvc_declspec_flags;
    int type_specifier_flags;
    int type_qualifier_flags;
    int storage_class_specifier_flags;
    int function_specifier_flags;
    int alignment_specifier_flags;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct typeof_specifier * typeof_specifier;
    struct token * first_token;
    struct token * last_token;
    struct declaration_specifier * head;
    struct declaration_specifier * tail;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct enumerator {
    unsigned char  has_shared_ownership;
    struct token * token;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct expression * constant_expression_opt;
    struct enum_specifier * enum_specifier;
    struct enumerator * next;
    struct object  value;
};

struct include_dir_list {
    struct include_dir * head;
    struct include_dir * tail;
};

struct preprocessor_ctx {
    struct options  options;
    int flags;
    struct hash_map  macros;
    struct include_dir_list  include_dir;
    struct hash_map  pragma_once_map;
    struct token * current;
    struct token_list  input_list;
    unsigned int count_macro_value;
    unsigned char  conditional_inclusion;
    int n_warnings;
    int n_errors;
};

struct member_declarator;

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct defer_visit_ctx {
    struct secondary_block * catch_secondary_block_opt;
    struct parser_ctx * ctx;
    struct ast  ast;
    struct defer_defer_scope * tail_block;
    int parameter_list;
};

struct tokenizer_ctx {
    struct options  options;
    int n_warnings;
    int n_errors;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
};

struct initializer_list_item;

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct dirent {
    unsigned short d_ino;
    long d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct TAGDIR;

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct d_visit_ctx {
    struct options  options;
    int indentation;
    unsigned char  print_qualifiers;
    unsigned int cake_file_scope_declarator_number;
    unsigned int cake_local_declarator_number;
    struct hash_map  tag_names;
    struct hash_map  structs_map;
    struct hash_map  file_scope_declarator_map;
    struct hash_map  instantiated_function_literals;
    struct osstream  block_scope_declarators;
    struct osstream  add_this_before;
    struct osstream  add_this_before_external_decl;
    struct osstream  add_this_after_external_decl;
    unsigned char  is_local;
    unsigned char  zero_mem_used;
    unsigned char  memcpy_used;
    unsigned char  define_nullptr;
    unsigned char  null_pointer_constant_used;
    unsigned char  address_of_argument;
    struct declarator * p_current_function_opt;
    struct break_reference  break_reference;
    unsigned char  is__func__predefined_identifier_added;
    struct ast  ast;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct declarator {
    unsigned char  has_shared_ownership;
    struct token * first_token_opt;
    struct token * last_token_opt;
    struct pointer * pointer;
    struct direct_declarator * direct_declarator;
    struct declaration_specifiers * declaration_specifiers;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct token * name_opt;
    struct compound_statement * function_body;
    struct declarator * p_complete_declarator;
    int num_uses;
    struct object  object;
    struct flow_object * p_flow_object;
    struct expression * p_expression_true;
    struct expression * p_expression_false;
    struct expression * p_alias_of_expression;
    struct type  type;
    unsigned char  declarator_renamed;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
char *strrchr2(char * s, int c)
{
    char * last;
    unsigned char ch;

    last = 0;
    ch = (unsigned char)c;
    while (*s)
    {
        if ((unsigned char)*s == ch)
        {
            last = s;
        }
        s++;
    }
    if (ch == 0)
    {
        return (char *)s;
    }
    return (char *)last;
}


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
    if (list->head == 0)
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

    if (list->head == 0)
    {
        return;
    }
    ;
    p = list->tail;
    if (list->head == list->tail)
    {
        list->head = 0;
        list->tail = 0;
    }
    else
    {
        list->tail = list->tail->previous;
        if (list->tail == list->head)
        {
            ;
            list->tail->next = 0;
            list->tail->previous = 0;
        }
    }
    p->next = 0;
    p->previous = 0;
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


void diagnostic_id_stack_pop(struct diagnostic_id_stack * diagnostic_stack);
unsigned char is_diagnostic_configurable(int id);
unsigned char is_diagnostic_note(int id);
unsigned char is_diagnostic_error(int id);
unsigned char is_diagnostic_warning(int id);
void print_position(char * path, int line, int col, unsigned char  msvc_format);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
void print_line_and_token(struct marker * p_marker, unsigned char  visual_studio_ouput_format);
static void stringfy(char * input, char * json_str_message, int output_size);
static int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...);

unsigned char compiler_diagnostic(int w, struct parser_ctx * ctx, struct token * p_token_opt, struct marker * p_marker_temp, char * fmt, ...)
{
    unsigned char  included_file_location;
    struct marker  marker;
    unsigned char  is_error;
    unsigned char  is_warning;
    unsigned char  is_note;
    char * func_name;
    char buffer[200];
    char * args;

    if (ctx->p_diagnostic_id_stack && ctx->p_diagnostic_id_stack->size > 0)
    {
        int d;

        d = ctx->p_diagnostic_id_stack->stack[ctx->p_diagnostic_id_stack->size - 1];
        if (d == w)
        {
            diagnostic_id_stack_pop(ctx->p_diagnostic_id_stack);
            return 0;
        }
        else
        {
        }
    }
    included_file_location = 0;
    _cake_zmem(&marker, 28);
    if (p_marker_temp == 0)
    {
        if (p_token_opt == 0)
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
        if (p_token_opt == 0)
        {
            return 0;
        }
        marker.file = p_token_opt->token_origin->lexeme;
        included_file_location = p_token_opt->level > 0;
        marker.line = p_token_opt->line;
        marker.start_col = p_token_opt->col;
        marker.end_col = p_token_opt->col;
    }
    is_error = 0;
    is_warning = 0;
    is_note = 0;
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
    print_position(marker.file, marker.line, marker.start_col, ctx->options.visual_studio_ouput_format);
    _cake_zmem(&args, 4);
    ((void)(args = (char *)(&(fmt)) + 4));
    vsnprintf(buffer, 200, fmt, args);
    ((void)(args = (char *)0));
    if (ctx->options.visual_studio_ouput_format)
    {
        if (is_error)
        {
            printf("error C%d: ", w);
        }
        else
        {
            if (is_warning)
            {
                printf("warning C%d: ", w);
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
    }
    else
    {
        if (is_error)
        {
            printf("\x1b[91merror \x1b[97mC%04d: %s\n\x1b[0m", w, buffer);
        }
        else
        {
            if (is_warning)
            {
                printf("\x1b[95mwarning \x1b[97mC%04d: %s\n\x1b[0m", w, buffer);
            }
            else
            {
                if (is_note)
                {
                    if (w == 62)
                    {
                        printf("\x1b[36;1mnote: \x1b[97m%s\n\x1b[0m", buffer);
                    }
                    else
                    {
                        printf("\x1b[36;1mnote: \x1b[97m%s\n\x1b[0m", buffer);
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
        stringfy(buffer, json_str_message, 200);
        if (ctx->sarif_entries > 0)
        {
            fprintf(ctx->sarif_file, "   ,\n");
        }
        ((struct parser_ctx *)ctx)->sarif_entries++;
        fprintf(ctx->sarif_file, "   {\n");
        fprintf(ctx->sarif_file, "     \"ruleId\":\"C%d\",\n", w);
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



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 2ULL, _Buffer, _BufferCount, _Format, 0, _ArgList);
    return _Result < 0 ? -1 : _Result;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
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

static int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfprintf_l(_Stream, _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
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


unsigned char first_of_function_specifier(struct parser_ctx * ctx)
{
    struct token * token;

    token = ctx->current;
    if (token == 0)
    {
        return 0;
    }
    return token->type == 9018 || token->type == 9067;
}


unsigned char first_of_enum_specifier_token(struct token * token)
{
    return token->type == 9012;
}


unsigned char first_of_enum_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return first_of_enum_specifier_token(ctx->current);
}


unsigned char first_of_alignment_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 9043;
}


struct token *parser_look_ahead(struct parser_ctx * ctx);

unsigned char first_of_atomic_type_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    if (ctx->current->type == 9045)
    {
        struct token * ahead;

        ahead = parser_look_ahead(ctx);
        if (ahead != 0)
        {
            return ahead->type == 40;
        }
    }
    return 0;
}


unsigned char first_of_storage_class_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 9035 || ctx->current->type == 9002 || ctx->current->type == 9013 || ctx->current->type == 9032 || ctx->current->type == 9070 || ctx->current->type == 8999 || ctx->current->type == 9025;
}


unsigned char first_of_struct_or_union_token(struct token * token)
{
    return token->type == 9033 || token->type == 9038;
}


unsigned char first_of_struct_or_union(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return first_of_struct_or_union_token(ctx->current);
}


unsigned char first_of_type_qualifier_token(struct token * p_token)
{
    return p_token->type == 9004 || p_token->type == 9026 || p_token->type == 9041 || p_token->type == 9045 || p_token->type == 9053 || p_token->type == 9054 || p_token->type == 9078 || p_token->type == 9077 || p_token->type == 9079 || p_token->type == 9080 || p_token->type == 9081;
}


unsigned char first_of_type_qualifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    if (ctx->current->type == 9004)
    {
        struct token * ahead;

        ahead = parser_look_ahead(ctx);
        if (ahead && ahead->type == 40)
        {
            return 0;
        }
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
    return 0;
}


struct map_entry *find_variables(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt)
{
    struct scope * scope;

    if (ppscope_opt != 0)
    {
        *ppscope_opt = 0;
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
    return 0;
}


struct enum_specifier *find_enum_specifier(struct parser_ctx * ctx, char * lexeme)
{
    struct enum_specifier * best;
    struct scope * scope;

    best = 0;
    scope = ctx->scopes.tail;
    while (scope)
    {
        struct map_entry * p_entry;

        p_entry = hashmap_find(&scope->tags, lexeme);
        if (p_entry && p_entry->type == 1)
        {
            ;
            best = p_entry->data.p_enum_specifier;
            if (best->enumerator_list.head != 0)
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

    p = 0;
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
    return 0;
}


struct enumerator *find_enumerator(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt)
{
    struct map_entry * p_entry;

    p_entry = find_variables(ctx, lexeme, ppscope_opt);
    if (p_entry && p_entry->type == 3)
    {
        return p_entry->data.p_enumerator;
    }
    return 0;
}


unsigned char first_of_typedef_name(struct parser_ctx * ctx, struct token * p_token)
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
    p_declarator = find_declarator(ctx, p_token->lexeme, 0);
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


unsigned char first_of_type_specifier_token(struct parser_ctx * ctx, struct token * token);

unsigned char first_of_type_name_ahead(struct parser_ctx * ctx)
{
    struct token * token_ahead;

    if (ctx->current == 0)
    {
        return 0;
    }
    if (ctx->current->type != 40)
    {
        return 0;
    }
    token_ahead = parser_look_ahead(ctx);
    if (token_ahead == 0)
    {
        return 0;
    }
    return first_of_type_specifier_token(ctx, token_ahead) || first_of_type_qualifier_token(token_ahead);
}


unsigned char first_of_type_specifier(struct parser_ctx * ctx);

unsigned char first_of_type_name(struct parser_ctx * ctx)
{
    return first_of_type_specifier(ctx) || first_of_type_qualifier(ctx);
}


unsigned char first_of_type_specifier_token(struct parser_ctx * ctx, struct token * p_token)
{
    return p_token->type == 9040 || p_token->type == 9003 || p_token->type == 9028 || p_token->type == 9019 || p_token->type == 9020 || p_token->type == 9014 || p_token->type == 9010 || p_token->type == 9029 || p_token->type == 9039 || p_token->type == 9076 || p_token->type == 9060 || p_token->type == 9061 || p_token->type == 9063 || p_token->type == 9064 || p_token->type == 9062 || p_token->type == 9071 || p_token->type == 9075 || p_token->type == 9047 || p_token->type == 9021 || p_token->type == 9022 || p_token->type == 9023 || p_token->type == 9024 || first_of_atomic_type_specifier(ctx) || first_of_struct_or_union_token(p_token) || first_of_enum_specifier_token(p_token) || first_of_typedef_name(ctx, p_token);
}


unsigned char first_of_type_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return first_of_type_specifier_token(ctx, ctx->current);
}


unsigned char first_of_type_specifier_qualifier(struct parser_ctx * ctx)
{
    return first_of_type_specifier(ctx) || first_of_type_qualifier(ctx) || first_of_alignment_specifier(ctx);
}


unsigned char first_of_compound_statement(struct parser_ctx * ctx)
{
    return ctx->current != 0 && ctx->current->type == 123;
}


unsigned char first_of_jump_statement(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 9016 || ctx->current->type == 9005 || ctx->current->type == 9000 || ctx->current->type == 9027 || ctx->current->type == 9037;
}


unsigned char first_of_selection_statement(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 9017 || ctx->current->type == 9034;
}


unsigned char first_of_iteration_statement(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 9042 || ctx->current->type == 9008 || ctx->current->type == 9015;
}


unsigned char first_of_label(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    if (ctx->current->type == 8996)
    {
        struct token * next;

        next = parser_look_ahead(ctx);
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


unsigned char first_of_declaration_specifier(struct parser_ctx * ctx)
{
    return first_of_storage_class_specifier(ctx) || first_of_function_specifier(ctx) || first_of_type_specifier_qualifier(ctx);
}


unsigned char first_of_pragma_declaration(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 128;
}


unsigned char first_of_static_assert_declaration(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 9068 || ctx->current->type == 9082 || ctx->current->type == 9083 || ctx->current->type == 9084 || ctx->current->type == 9085;
}


unsigned char first_of_attribute_specifier(struct parser_ctx * ctx)
{
    struct token * p_token;

    if (ctx->current == 0)
    {
        return 0;
    }
    if (ctx->current->type != 91)
    {
        return 0;
    }
    p_token = parser_look_ahead(ctx);
    return p_token != 0 && p_token->type == 91;
}


unsigned char first_of_labeled_statement(struct parser_ctx * ctx)
{
    return first_of_label(ctx);
}


unsigned char first_of_designator(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 91 || ctx->current->type == 46;
}


struct token *previous_parser_token(struct token * token)
{
    struct token * prev;

    if (token->prev == 0)
    {
        return 0;
    }
    prev = token->prev;
    while (prev && !(prev->flags & 1))
    {
        prev = prev->prev;
    }
    return prev;
}


int __cdecl strcmp(char * _Str1, char * _Str2);

int is_keyword(char * text, int target)
{
    /*switch*/
    {
        char __v0 = text[0];
        if (__v0 == 97) goto __L1; /*case 97*/
        if (__v0 == 98) goto __L7; /*case 98*/
        if (__v0 == 99) goto __L10; /*case 99*/
        if (__v0 == 100) goto __L17; /*case 100*/
        if (__v0 == 101) goto __L22; /*case 101*/
        if (__v0 == 102) goto __L26; /*case 102*/
        if (__v0 == 103) goto __L30; /*case 103*/
        if (__v0 == 105) goto __L32; /*case 105*/
        if (__v0 == 110) goto __L36; /*case 110*/
        if (__v0 == 108) goto __L38; /*case 108*/
        if (__v0 == 114) goto __L40; /*case 114*/
        if (__v0 == 115) goto __L44; /*case 115*/
        if (__v0 == 116) goto __L56; /*case 116*/
        if (__v0 == 117) goto __L64; /*case 117*/
        if (__v0 == 118) goto __L67; /*case 118*/
        if (__v0 == 119) goto __L70; /*case 119*/
        if (__v0 == 95) goto __L72; /*case 95*/
        goto __L129; /* default */

        {
            __L1: /*case 97*/ 
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
                return 9069;
            }
            goto __L0; /* break */

            __L7: /*case 98*/ 
            if (strcmp("break", text) == 0)
            {
                return 9000;
            }
            if (strcmp("bool", text) == 0)
            {
                return 9060;
            }
            goto __L0; /* break */

            __L10: /*case 99*/ 
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
            goto __L0; /* break */

            __L17: /*case 100*/ 
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
            goto __L0; /* break */

            __L22: /*case 101*/ 
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
            goto __L0; /* break */

            __L26: /*case 102*/ 
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
                return 9073;
            }
            goto __L0; /* break */

            __L30: /*case 103*/ 
            if (strcmp("goto", text) == 0)
            {
                return 9016;
            }
            goto __L0; /* break */

            __L32: /*case 105*/ 
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
            goto __L0; /* break */

            __L36: /*case 110*/ 
            if (strcmp("nullptr", text) == 0)
            {
                return 9074;
            }
            goto __L0; /* break */

            __L38: /*case 108*/ 
            if (strcmp("long", text) == 0)
            {
                return 9020;
            }
            goto __L0; /* break */

            __L40: /*case 114*/ 
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
            goto __L0; /* break */

            __L44: /*case 115*/ 
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
                return 9068;
            }
            if (strcmp("static_debug", text) == 0)
            {
                return 9082;
            }
            if (strcmp("static_debug_ex", text) == 0)
            {
                return 9083;
            }
            if (strcmp("static_state", text) == 0)
            {
                return 9084;
            }
            if (strcmp("static_set", text) == 0)
            {
                return 9085;
            }
            goto __L0; /* break */

            __L56: /*case 116*/ 
            if (strcmp("typedef", text) == 0)
            {
                return 9035;
            }
            if (strcmp("typeof", text) == 0)
            {
                return 9071;
            }
            if (strcmp("typeof_unqual", text) == 0)
            {
                return 9075;
            }
            if (strcmp("true", text) == 0)
            {
                return 9072;
            }
            if (strcmp("thread_local", text) == 0)
            {
                return 9070;
            }
            if (strcmp("try", text) == 0)
            {
                return 9036;
            }
            if (strcmp("throw", text) == 0)
            {
                return 9037;
            }
            goto __L0; /* break */

            __L64: /*case 117*/ 
            if (strcmp("union", text) == 0)
            {
                return 9038;
            }
            if (strcmp("unsigned", text) == 0)
            {
                return 9039;
            }
            goto __L0; /* break */

            __L67: /*case 118*/ 
            if (strcmp("void", text) == 0)
            {
                return 9040;
            }
            if (strcmp("volatile", text) == 0)
            {
                return 9041;
            }
            goto __L0; /* break */

            __L70: /*case 119*/ 
            if (strcmp("while", text) == 0)
            {
                return 9042;
            }
            goto __L0; /* break */

            __L72: /*case 95*/ 
            if (strcmp("_Ctor", text) == 0)
            {
                return 9078;
            }
            if (strcmp("_Owner", text) == 0)
            {
                return 9077;
            }
            if (strcmp("_Dtor", text) == 0)
            {
                return 9079;
            }
            if (strcmp("_Opt", text) == 0)
            {
                return 9081;
            }
            if (strcmp("_View", text) == 0)
            {
                return 9080;
            }
            if (strcmp("_Countof", text) == 0)
            {
                return 9031;
            }
            if (strcmp("_is_lvalue", text) == 0)
            {
                return 9087;
            }
            if (strcmp("_is_const", text) == 0)
            {
                return 9088;
            }
            if (strcmp("_is_owner", text) == 0)
            {
                return 9089;
            }
            if (strcmp("_is_pointer", text) == 0)
            {
                return 9086;
            }
            if (strcmp("_is_array", text) == 0)
            {
                return 9090;
            }
            if (strcmp("_is_function", text) == 0)
            {
                return 9091;
            }
            if (strcmp("_is_arithmetic", text) == 0)
            {
                return 9093;
            }
            if (strcmp("_is_floating_point", text) == 0)
            {
                return 9094;
            }
            if (strcmp("_is_integral", text) == 0)
            {
                return 9095;
            }
            if (strcmp("_is_scalar", text) == 0)
            {
                return 9092;
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
            if (strcmp("__builtin_va_list", text) == 0)
            {
                return 9047;
            }
            if (strcmp("__attribute__", text) == 0)
            {
                return 9046;
            }
            if (strcmp("__builtin_offsetof", text) == 0)
            {
                return 9052;
            }
            if (strcmp("__builtin_va_end", text) == 0)
            {
                return 9048;
            }
            if (strcmp("__builtin_va_arg", text) == 0)
            {
                return 9049;
            }
            if (strcmp("__builtin_c23_va_start", text) == 0)
            {
                return 9050;
            }
            if (strcmp("__builtin_va_start", text) == 0)
            {
                return 9050;
            }
            if (strcmp("__builtin_va_copy", text) == 0)
            {
                return 9051;
            }
            if (target == 1 || target == 2)
            {
                if (strcmp("__ptr32", text) == 0)
                {
                    return 9053;
                }
                if (strcmp("__ptr64", text) == 0)
                {
                    return 9054;
                }
            }
            if (strcmp("_Bool", text) == 0)
            {
                return 9060;
            }
            if (strcmp("_Complex", text) == 0)
            {
                return 9061;
            }
            if (strcmp("_Decimal32", text) == 0)
            {
                return 9063;
            }
            if (strcmp("_Decimal64", text) == 0)
            {
                return 9064;
            }
            if (strcmp("_Decimal128", text) == 0)
            {
                return 9062;
            }
            if (strcmp("_Generic", text) == 0)
            {
                return 9065;
            }
            if (strcmp("_Imaginary", text) == 0)
            {
                return 9066;
            }
            if (strcmp("_Noreturn", text) == 0)
            {
                return 9067;
            }
            if (strcmp("_Static_assert", text) == 0)
            {
                return 9068;
            }
            if (strcmp("_Thread_local", text) == 0)
            {
                return 9070;
            }
            if (strcmp("_BitInt", text) == 0)
            {
                return 9076;
            }
            if (strcmp("__typeof__", text) == 0)
            {
                return 9071;
            }
            if (target == 1 || target == 2)
            {
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
                    return 9059;
                }
                if (strcmp("__stdcall", text) == 0 || strcmp("_stdcall", text) == 0)
                {
                    return 9056;
                }
                if (strcmp("__cdecl", text) == 0)
                {
                    return 9057;
                }
                if (strcmp("__fastcall", text) == 0)
                {
                    return 9055;
                }
                if (strcmp("__alignof", text) == 0)
                {
                    return 9044;
                }
                if (strcmp("__restrict", text) == 0)
                {
                    return 9026;
                }
                if (strcmp("__declspec", text) == 0)
                {
                    return 9058;
                }
            }
            goto __L0; /* break */

            __L129: /* default */ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return 0;
}


static void token_promote(struct parser_ctx * ctx, struct token * token);
int parse_number(char * lexeme, char suffix[4], char erromsg[100]);

struct token *parser_look_ahead(struct parser_ctx * ctx)
{
    struct token * p;

    if (ctx->current == 0)
    {
        return 0;
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



static void token_promote(struct parser_ctx * ctx, struct token * token)
{
    if (token->type == 8997)
    {
        token->type = 8996;
    }
    if (token->type == 8996)
    {
        int t;

        t = is_keyword(token->lexeme, ctx->options.target);
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
                compiler_diagnostic(1380, ctx, token, 0, errormsg);
            }
        }
    }
}
static void parser_skip_blanks(struct parser_ctx * ctx);

void parser_match(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return;
    }
    ctx->previous = ctx->current;
    ctx->current = ctx->current->next;
    parser_skip_blanks(ctx);
}



static void parser_skip_blanks(struct parser_ctx * ctx)
{
    while (ctx->current && !(ctx->current->flags & 1))
    {
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
void unexpected_end_of_file(struct parser_ctx * ctx)
{
    compiler_diagnostic(970, ctx, ctx->input_list.tail, 0, "unexpected end of file");
}


char *get_diagnostic_friendly_token_name(int tk);

int parser_match_tk(struct parser_ctx * ctx, int type)
{
    int error;

    error = 0;
    if (ctx->current != 0)
    {
        if (ctx->current->type != type)
        {
            compiler_diagnostic(970, ctx, ctx->current, 0, "expected token '%s', got '%s' ", get_diagnostic_friendly_token_name(type), get_diagnostic_friendly_token_name(ctx->current->type));
            error = 1;
        }
        ctx->previous = ctx->current;
        ctx->current = ctx->current->next;
        parser_skip_blanks(ctx);
    }
    else
    {
        compiler_diagnostic(970, ctx, ctx->input_list.tail, 0, "unexpected end of file after");
        error = 1;
    }
    return error;
}


void print_type_qualifier_flags(struct osstream * ss, unsigned char * first, int e_type_qualifier_flags);
int ss_fprintf(struct osstream * stream, char * fmt, ...);
void print_item(struct osstream * ss, unsigned char * first, char * item);
unsigned char print_type_specifier_flags(struct osstream * ss, unsigned char * first, int e_type_specifier_flags);

void print_declaration_specifiers(struct osstream * ss, struct declaration_specifiers * p_declaration_specifiers)
{
    unsigned char  first;

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


unsigned char type_specifier_is_integer(int flags)
{
    if ((flags & 2) || (flags & 4) || (flags & 8) || (flags & 16) || (flags & 8) || (flags & 4194304))
    {
        return 1;
    }
    return 0;
}


int final_specifier(struct parser_ctx * ctx, int * flags)
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


int add_specifier(struct parser_ctx * ctx, int * flags, int new_flag)
{
    if (new_flag & 16)
    {
        if ((*flags) & 4194304)
        {
            compiler_diagnostic(980, ctx, ctx->current, 0, "cannot combine with previous 'long long' declaration specifier");
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
        unsigned int __v0 = (unsigned int)*flags;
        if (__v0 == 0) goto __L4; /*case 0*/
        if (__v0 == 1) goto __L5; /*case 1*/
        if (__v0 == 33554432) goto __L6; /*case 33554432*/
        if (__v0 == 2) goto __L7; /*case 2*/
        if (__v0 == 130) goto __L8; /*case 130*/
        if (__v0 == 258) goto __L9; /*case 258*/
        if (__v0 == 4) goto __L10; /*case 4*/
        if (__v0 == 132) goto __L11; /*case 132*/
        if (__v0 == 12) goto __L12; /*case 12*/
        if (__v0 == 140) goto __L13; /*case 140*/
        if (__v0 == 260) goto __L14; /*case 260*/
        if (__v0 == 268) goto __L15; /*case 268*/
        if (__v0 == 8) goto __L16; /*case 8*/
        if (__v0 == 128) goto __L17; /*case 128*/
        if (__v0 == 136) goto __L18; /*case 136*/
        if (__v0 == 256) goto __L19; /*case 256*/
        if (__v0 == 264) goto __L20; /*case 264*/
        if (__v0 == 16) goto __L21; /*case 16*/
        if (__v0 == 144) goto __L22; /*case 144*/
        if (__v0 == 24) goto __L23; /*case 24*/
        if (__v0 == 152) goto __L24; /*case 152*/
        if (__v0 == 272) goto __L25; /*case 272*/
        if (__v0 == 280) goto __L26; /*case 280*/
        if (__v0 == 4194304) goto __L27; /*case 4194304*/
        if (__v0 == 4194432) goto __L28; /*case 4194432*/
        if (__v0 == 4194312) goto __L29; /*case 4194312*/
        if (__v0 == 4194440) goto __L30; /*case 4194440*/
        if (__v0 == 4194560) goto __L31; /*case 4194560*/
        if (__v0 == 4194568) goto __L32; /*case 4194568*/
        if (__v0 == 32) goto __L33; /*case 32*/
        if (__v0 == 64) goto __L34; /*case 64*/
        if (__v0 == 80) goto __L35; /*case 80*/
        if (__v0 == 2048) goto __L36; /*case 2048*/
        if (__v0 == 4096) goto __L37; /*case 4096*/
        if (__v0 == 8192) goto __L38; /*case 8192*/
        if (__v0 == 512) goto __L39; /*case 512*/
        if (__v0 == 1056) goto __L40; /*case 1056*/
        if (__v0 == 1088) goto __L41; /*case 1088*/
        if (__v0 == 1104) goto __L42; /*case 1104*/
        if (__v0 == 16384) goto __L43; /*case 16384*/
        if (__v0 == 32768) goto __L44; /*case 32768*/
        if (__v0 == 65536) goto __L45; /*case 65536*/
        if (__v0 == 8388608) goto __L46; /*case 8388608*/
        if (__v0 == 131072) goto __L47; /*case 131072*/
        goto __L48; /* default */

        {
            __L4: /*case 0*/ 
            __L5: /*case 1*/ 
            __L6: /*case 33554432*/ 
            __L7: /*case 2*/ 
            __L8: /*case 130*/ 
            __L9: /*case 258*/ 
            __L10: /*case 4*/ 
            __L11: /*case 132*/ 
            __L12: /*case 12*/ 
            __L13: /*case 140*/ 
            __L14: /*case 260*/ 
            __L15: /*case 268*/ 
            __L16: /*case 8*/ 
            __L17: /*case 128*/ 
            __L18: /*case 136*/ 
            __L19: /*case 256*/ 
            __L20: /*case 264*/ 
            __L21: /*case 16*/ 
            __L22: /*case 144*/ 
            __L23: /*case 24*/ 
            __L24: /*case 152*/ 
            __L25: /*case 272*/ 
            __L26: /*case 280*/ 
            __L27: /*case 4194304*/ 
            __L28: /*case 4194432*/ 
            __L29: /*case 4194312*/ 
            __L30: /*case 4194440*/ 
            __L31: /*case 4194560*/ 
            __L32: /*case 4194568*/ 
            __L33: /*case 32*/ 
            __L34: /*case 64*/ 
            __L35: /*case 80*/ 
            __L36: /*case 2048*/ 
            __L37: /*case 4096*/ 
            __L38: /*case 8192*/ 
            __L39: /*case 512*/ 
            __L40: /*case 1056*/ 
            __L41: /*case 1088*/ 
            __L42: /*case 1104*/ 
            __L43: /*case 16384*/ 
            __L44: /*case 32768*/ 
            __L45: /*case 65536*/ 
            __L46: /*case 8388608*/ 
            __L47: /*case 131072*/ 
            goto __L3; /* break */

            __L48: /* default */ 
            compiler_diagnostic(1300, ctx, ctx->current, 0, "incompatible specifiers");
            return 1;
        }
        __L3:;
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

        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence);
        item = p->head;
        while (item)
        {
            struct declaration_specifier * next;

            next = item->next;
            item->next = 0;
            declaration_specifier_delete(item);
            item = next;
        }
        free(p);
    }
}


void declaration_specifiers_add(struct declaration_specifiers * list, struct declaration_specifier * p_item)
{
    if (list->head == 0)
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

struct declaration_specifiers *declaration_specifiers(struct parser_ctx * ctx, int default_storage_flag)
{
    struct declaration_specifiers * p_declaration_specifiers;

    if (ctx->current == 0)
    {
        return 0;
    }
    p_declaration_specifiers = calloc(1, 64);
    if (1) /*try*/
    {
        struct token * prev;

        if (p_declaration_specifiers == 0)
        {
            goto __L1; /* throw */
        }
        p_declaration_specifiers->first_token = ctx->current;
        while (first_of_declaration_specifier(ctx))
        {
            struct declaration_specifier * p_declaration_specifier;

            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L1; /* throw */
            }
            if (ctx->current->flags & 16)
            {
                if (p_declaration_specifiers->type_specifier_flags != 0)
                {
                    break;
                }
            }
            p_declaration_specifier = declaration_specifier(ctx);
            if (p_declaration_specifier == 0)
            {
                goto __L1; /* throw */
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
                                    p_declaration_specifiers->typedef_declarator = find_declarator(ctx, p_declaration_specifier->type_specifier_qualifier->type_specifier->token->lexeme, 0);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (p_declaration_specifier->type_specifier_qualifier->alignment_specifier)
                    {
                        p_declaration_specifiers->alignment_specifier_flags = p_declaration_specifier->type_specifier_qualifier->alignment_specifier->flags;
                    }
                    else
                    {
                        if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                        {
                            p_declaration_specifiers->type_qualifier_flags |= p_declaration_specifier->type_specifier_qualifier->type_qualifier->flags;
                        }
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
                    else
                    {
                        if (p_declaration_specifier->alignment_specifier)
                        {
                            p_declaration_specifiers->alignment_specifier_flags |= p_declaration_specifier->alignment_specifier->flags;
                        }
                    }
                }
            }
            declaration_specifiers_add(p_declaration_specifiers, p_declaration_specifier);
            if (p_declaration_specifiers->p_attribute_specifier_sequence == 0)
            {
                attribute_specifier_sequence_delete(p_declaration_specifiers->p_attribute_specifier_sequence);
                p_declaration_specifiers->p_attribute_specifier_sequence = 0;
            }
            if (p_declaration_specifiers->p_attribute_specifier_sequence == 0)
            {
                free(p_declaration_specifiers->p_attribute_specifier_sequence);
                p_declaration_specifiers->p_attribute_specifier_sequence = 0;
            }
            p_declaration_specifiers->p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L1; /* throw */
            }
            if (ctx->current->type == 8996 && p_declaration_specifiers->type_specifier_flags != 0)
            {
                break;
            }
        }
        prev = previous_parser_token(ctx->current);
        if (prev == 0)
        {
            goto __L1; /* throw */
        }
        p_declaration_specifiers->last_token = prev;
        final_specifier(ctx, &p_declaration_specifiers->type_specifier_flags);
        p_declaration_specifiers->storage_class_specifier_flags |= default_storage_flag;
    }
    else __L1: /*catch*/ 
    {
        declaration_specifiers_delete(p_declaration_specifiers);
        p_declaration_specifiers = 0;
    }
    return p_declaration_specifiers;
}


struct static_assert_declaration *static_assert_declaration(struct parser_ctx * ctx);
struct pragma_declaration *pragma_declaration(struct parser_ctx * ctx);
struct init_declarator_list init_declarator_list(struct parser_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers);
void declaration_delete(struct declaration * p);

struct declaration *declaration_core(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence, unsigned char  can_be_function_definition, unsigned char * is_function_definition, int default_storage_class_specifier_flags, unsigned char  without_semicolon)
{
    struct declaration * p_declaration;

    p_declaration = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_declaration = calloc(1, 52);
        if (p_declaration == 0)
        {
            goto __L0; /* throw */
        }
        p_declaration->p_attribute_specifier_sequence = p_attribute_specifier_sequence;
        p_attribute_specifier_sequence = 0;
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
                    if (p_declaration->declaration_specifiers == 0)
                    {
                        goto __L0; /* throw */
                    }
                    if (p_declaration->p_attribute_specifier_sequence)
                    {
                        p_declaration->declaration_specifiers->attributes_flags = p_declaration->p_attribute_specifier_sequence->attributes_flags;
                        p_declaration->declaration_specifiers->msvc_declspec_flags = p_declaration->p_attribute_specifier_sequence->msvc_declspec_flags;
                    }
                    if (ctx->current == 0)
                    {
                        unexpected_end_of_file(ctx);
                        goto __L0; /* throw */
                    }
                    if (ctx->current->type != 59)
                    {
                        p_declaration->init_declarator_list = init_declarator_list(ctx, p_declaration->declaration_specifiers);
                        if (p_declaration->init_declarator_list.head == 0)
                        {
                            goto __L0; /* throw */
                        }
                    }
                    if (ctx->current == 0)
                    {
                        unexpected_end_of_file(ctx);
                        goto __L0; /* throw */
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
                        if (ctx->current->type == 9072 || ctx->current->type == 9073 || ctx->current->type == 8996)
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
                                goto __L0; /* throw */
                            }
                        }
                    }
                }
                else
                {
                    if (ctx->current->type == 8996)
                    {
                        compiler_diagnostic(860, ctx, ctx->current, 0, "invalid type '%s'", ctx->current->lexeme);
                    }
                    else
                    {
                        compiler_diagnostic(990, ctx, ctx->current, 0, "expected declaration not '%s'", get_diagnostic_friendly_token_name(ctx->current->type));
                    }
                    parser_match(ctx);
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        declaration_delete(p_declaration);
        p_declaration = 0;
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
    return p_declaration;
}


struct declaration *declaration(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence, int storage_specifier_flags, unsigned char  extern_declaration);

struct declaration *function_definition_or_declaration(struct parser_ctx * ctx)
{
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;

    p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
    return declaration(ctx, p_attribute_specifier_sequence, 0, 1);
}


void simple_declaration_delete(struct simple_declaration * p);

struct simple_declaration *simple_declaration(struct parser_ctx * ctx, unsigned char  ignore_semicolon, struct attribute_specifier_sequence * p_attribute_specifier_sequence)
{
    int storage_specifier_flags;
    struct simple_declaration * p_simple_declaration;

    if (ctx->current == 0)
    {
        unexpected_end_of_file(ctx);
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
        return 0;
    }
    storage_specifier_flags = 4096;
    p_simple_declaration = calloc(1, 24);
    if (1) /*try*/
    {
        struct declaration_specifiers * ptemp;
        struct token * prev;

        if (p_simple_declaration == 0)
        {
            goto __L1; /* throw */
        }
        p_simple_declaration->first_token = ctx->current;
        p_simple_declaration->p_attribute_specifier_sequence = p_attribute_specifier_sequence;
        p_attribute_specifier_sequence = 0;
        ptemp = declaration_specifiers(ctx, storage_specifier_flags);
        if (ptemp == 0)
        {
            goto __L1; /* throw */
        }
        p_simple_declaration->p_declaration_specifiers = ptemp;
        if (p_simple_declaration->p_attribute_specifier_sequence)
        {
            p_simple_declaration->p_declaration_specifiers->attributes_flags = p_simple_declaration->p_attribute_specifier_sequence->attributes_flags;
        }
        p_simple_declaration->init_declarator_list = init_declarator_list(ctx, p_simple_declaration->p_declaration_specifiers);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L1; /* throw */
        }
        prev = previous_parser_token(ctx->current);
        if (prev == 0)
        {
            goto __L1; /* throw */
        }
        p_simple_declaration->last_token = prev;
        if (!ignore_semicolon && parser_match_tk(ctx, 59) != 0)
        {
            goto __L1; /* throw */
        }
    }
    else __L1: /*catch*/ 
    {
        simple_declaration_delete(p_simple_declaration);
        p_simple_declaration = 0;
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
    return p_simple_declaration;
}


struct diagnostic_id_stack *build_diagnostic_id_stack(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence, struct diagnostic_id_stack * stack, int diagnostic_phase);
static void check_func_open_brace_style(struct parser_ctx * ctx, struct token * token);
unsigned char is_diagnostic_enabled(struct options * options, int w);
struct expression *expression(struct parser_ctx * ctx, int eval_mode);
struct compound_statement *function_body(struct parser_ctx * ctx);
static void check_unused_parameters(struct parser_ctx * ctx, struct parameter_list * parameter_list);
unsigned char type_is_maybe_unused(struct type * p_type);
void defer_start_visit_declaration(struct defer_visit_ctx * ctx, struct declaration * p_declaration);
void defer_visit_ctx_destroy(struct defer_visit_ctx * p);
void flow_start_visit_declaration(struct flow_visit_ctx * ctx, struct declaration * p_declaration);
void flow_visit_ctx_destroy(struct flow_visit_ctx * p);
void warn_unrecognized_warnings(struct parser_ctx * ctx, struct diagnostic_id_stack * stack, struct attribute_specifier_sequence * p_attribute_specifier_sequence, struct diagnostic_id_stack * p_diagnostic_id_stack);

struct declaration *declaration(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence, int storage_specifier_flags, unsigned char  extern_declaration)
{
    struct declaration * p_declaration;

    p_declaration = 0;
    if (1) /*try*/
    {
        unsigned char  is_function_definition;
        struct diagnostic_id_stack  stack;
        struct diagnostic_id_stack * p_diagnostic_id_stack;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        is_function_definition = 0;
        _cake_zmem(&stack, 44);
        p_diagnostic_id_stack = build_diagnostic_id_stack(ctx, p_attribute_specifier_sequence, &stack, 0);
        p_declaration = declaration_core(ctx, p_attribute_specifier_sequence, 1, &is_function_definition, storage_specifier_flags, 0);
        p_attribute_specifier_sequence = 0;
        if (p_declaration == 0)
        {
            goto __L0; /* throw */
        }
        if (is_function_definition)
        {
            struct declarator * p_declarator;
            struct declarator * inner;
            struct diagnostic  before_function_diagnostics;
            struct declarator * p_current_function_opt;
            struct scope * parameters_scope;
            struct scope * p_current_function_scope_opt;
            struct compound_statement * p_function_body;

            if (p_declaration->init_declarator_list.head == 0 || p_declaration->init_declarator_list.head->p_declarator->direct_declarator == 0 || p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator == 0)
            {
                compiler_diagnostic(650, ctx, ctx->current, 0, "unexpected");
                goto __L0; /* throw */
            }
            p_declarator = p_declaration->init_declarator_list.head->p_declarator;
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
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            check_func_open_brace_style(ctx, ctx->current);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            before_function_diagnostics = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];
            if (ctx->current->type == 9072 || ctx->current->type == 9073 || ctx->current->type == 8996)
            {
                for (; ; )
                {
                    int type;

                    if (ctx->current == 0)
                    {
                        unexpected_end_of_file(ctx);
                        goto __L0; /* throw */
                    }
                    type = ctx->current->type;
                    if (type != 9072 && type != 9073 && type != 8996)
                    {
                        goto __L0; /* throw */
                    }
                    parser_match(ctx);
                    parser_match(ctx);
                    if (type != 9073)
                    {
                        ;
                        p_declarator->p_expression_true = expression(ctx, 2);
                    }
                    else
                    {
                        ;
                        p_declarator->p_expression_false = expression(ctx, 2);
                    }
                    parser_match(ctx);
                    if (ctx->current == 0)
                    {
                        unexpected_end_of_file(ctx);
                        goto __L0; /* throw */
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
            if (inner->direct_declarator->function_declarator == 0)
            {
                if (inner->first_token_opt)
                {
                    compiler_diagnostic(650, ctx, inner->first_token_opt, 0, "missing function declarator");
                }
                else
                {
                    compiler_diagnostic(650, ctx, ctx->current, 0, "missing function declarator");
                }
                goto __L0; /* throw */
            }
            parameters_scope = &inner->direct_declarator->function_declarator->parameters_scope;
            scope_list_push(&ctx->scopes, parameters_scope);
            p_current_function_scope_opt = ctx->p_current_function_scope_opt;
            ctx->p_current_function_scope_opt = ctx->scopes.tail;
            p_function_body = function_body(ctx);
            ctx->p_current_function_scope_opt = p_current_function_scope_opt;
            ctx->p_current_function_opt = p_current_function_opt;
            scope_list_pop(&ctx->scopes);
            if (p_function_body == 0)
            {
                goto __L0; /* throw */
            }
            ;
            p_declaration->function_body = p_function_body;
            p_declaration->init_declarator_list.head->p_declarator->function_body = p_declaration->function_body;
            if (p_declaration->init_declarator_list.head && p_declaration->init_declarator_list.head->p_declarator->direct_declarator && p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator && p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt && p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
            {
                check_unused_parameters(ctx, p_declaration->init_declarator_list.head->p_declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list);
            }
            if (p_declaration->function_body)
            {
                char * func_name;
                struct scope * p_previous_scope;
                struct declarator * p_previous_declarator;

                func_name = p_declaration->init_declarator_list.head->p_declarator->name_opt->lexeme;
                p_previous_scope = 0;
                p_previous_declarator = find_declarator(ctx, func_name, &p_previous_scope);
                if (p_previous_declarator && p_previous_declarator != p_declaration->init_declarator_list.head->p_declarator)
                {
                    struct scope * p_current_scope;

                    p_previous_declarator->p_complete_declarator = p_declaration->init_declarator_list.head->p_declarator;
                    p_current_scope = ctx->scopes.tail;
                    if (p_current_scope == p_previous_scope)
                    {
                        if (p_previous_declarator->function_body)
                        {
                            compiler_diagnostic(1020, ctx, p_declaration->init_declarator_list.head->p_declarator->name_opt, 0, "function redefinition");
                            compiler_diagnostic(62, ctx, p_previous_declarator->name_opt, 0, "previous definition");
                        }
                        else
                        {
                        }
                    }
                }
            }
            if (extern_declaration && ctx->p_report->error_count == 0)
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
                if (ctx->options.flow_analysis)
                {
                    struct flow_visit_ctx  ctx3;

                    ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index] = before_function_diagnostics;
                    _cake_zmem(&ctx3, 868);
                    ctx3.ctx = ctx;
                    flow_start_visit_declaration(&ctx3, p_declaration);
                    flow_visit_ctx_destroy(&ctx3);
                }
            }
        }
        else
        {
            if (ctx->options.flow_analysis && extern_declaration)
            {
                struct flow_visit_ctx  ctx2;

                _cake_zmem(&ctx2, 868);
                ctx2.ctx = ctx;
                flow_start_visit_declaration(&ctx2, p_declaration);
                flow_visit_ctx_destroy(&ctx2);
            }
        }
        warn_unrecognized_warnings(ctx, &stack, p_declaration->p_attribute_specifier_sequence, p_diagnostic_id_stack);
    }
    else __L0: /*catch*/ 
    {
        declaration_delete(p_declaration);
        p_declaration = 0;
    }
    return p_declaration;
}



static void check_func_open_brace_style(struct parser_ctx * ctx, struct token * token)
{
    if (token->level == 0 && !(token->flags & 2) && token->type == 123 && token->prev && is_diagnostic_enabled(&ctx->options, 11))
    {
        if (ctx->options.style == 0)
        {
            if (token->prev->type == 10)
            {
            }
            else
            {
                compiler_diagnostic(11, ctx, token, 0, "not following correct brace style {");
            }
        }
    }
}

static void check_unused_parameters(struct parser_ctx * ctx, struct parameter_list * parameter_list)
{
    struct parameter_declaration * parameter;

    parameter = 0;
    parameter = parameter_list->head;
    while (parameter)
    {
        if (!type_is_maybe_unused(&parameter->declarator->type) && parameter->declarator && parameter->declarator->num_uses == 0)
        {
            if (parameter->declarator->name_opt && parameter->declarator->name_opt->level == 0)
            {
                compiler_diagnostic(6, ctx, parameter->declarator->name_opt, 0, "'%s': unreferenced formal parameter", parameter->declarator->name_opt->lexeme);
            }
        }
        parameter = parameter->next;
    }
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

    p_declaration_specifier = 0;
    if (1) /*try*/
    {
        p_declaration_specifier = calloc(1, 20);
        if (p_declaration_specifier == 0)
        {
            goto __L0; /* throw */
        }
        if (first_of_storage_class_specifier(ctx))
        {
            p_declaration_specifier->storage_class_specifier = storage_class_specifier(ctx);
            if (p_declaration_specifier->storage_class_specifier == 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            if (first_of_type_specifier_qualifier(ctx))
            {
                p_declaration_specifier->type_specifier_qualifier = type_specifier_qualifier(ctx);
                if (p_declaration_specifier->type_specifier_qualifier == 0)
                {
                    goto __L0; /* throw */
                }
            }
            else
            {
                if (first_of_function_specifier(ctx))
                {
                    p_declaration_specifier->function_specifier = function_specifier(ctx);
                    if (p_declaration_specifier->function_specifier == 0)
                    {
                        goto __L0; /* throw */
                    }
                }
                else
                {
                    compiler_diagnostic(650, ctx, ctx->current, 0, "unexpected");
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        declaration_specifier_delete(p_declaration_specifier);
        p_declaration_specifier = 0;
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


struct declarator *declarator(struct parser_ctx * ctx, struct specifier_qualifier_list * specifier_qualifier_list, struct declaration_specifiers * declaration_specifiers, unsigned char  abstract_acceptable, struct token ** pptokenname);
struct type make_type_using_declarator(struct parser_ctx * ctx, struct declarator * pdeclarator);
void naming_convention_global_var(struct parser_ctx * ctx, struct token * token, struct type * type, int storage);
unsigned char type_is_same(struct type * a, struct type * b, unsigned char  compare_qualifiers);
void print_type_no_names(struct osstream * ss, struct type * p_type, int target);
void ss_clear(struct osstream * stream);
void ss_close(struct osstream * stream);
unsigned char type_is_function(struct type * p_type);
int hashmap_set(struct hash_map * map, char * key, struct hash_item_set * item);
void hash_item_set_destroy(struct hash_item_set * p);
struct initializer *initializer(struct parser_ctx * ctx, int eval_mode);
int make_object(struct type * p_type, struct object * obj, int target);
unsigned char type_is_const_or_constexpr(struct type * p_type);
int initializer_init_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * p_current_object, struct initializer * braced_initializer, unsigned char  is_constant, unsigned char  requires_constant_initialization);
unsigned char type_is_array(struct type * p_type);
struct type type_dup(struct type * p_type);
struct type type_lvalue_conversion(struct type * p_type, unsigned char  nullchecks_enabled);
void type_swap(struct type * a, struct type * b);
void type_destroy(struct type * p_type);
void type_remove_names(struct type * p_type);
char *strdup(char * src);
void type_set_storage_specifiers_using_declarator(struct type * p_type, struct declarator * pdeclarator);
void type_set_qualifiers_using_declarator(struct type * p_type, struct declarator * pdeclarator);
void type_visit_to_mark_anonymous(struct type * p_type);
void check_assigment(struct parser_ctx * ctx, struct type * left_type, struct expression * right, int assigment_type);
int make_object_with_member_designator(struct type * p_type, struct object * obj, char * member_designator, int target);
unsigned char type_is_const(struct type * p_type);
unsigned char type_is_pointer(struct type * p_type);
unsigned char type_is_vla(struct type * p_type);
int type_get_sizeof(struct type * p_type, unsigned int * size, int target);

struct init_declarator *init_declarator(struct parser_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers)
{
    struct init_declarator * p_init_declarator;

    p_init_declarator = 0;
    if (1) /*try*/
    {
        struct token * tkname;
        char * declarator_name;
        struct scope * out_scope;
        struct declarator * p_previous_declarator;

        p_init_declarator = calloc(1, 16);
        if (p_init_declarator == 0)
        {
            goto __L0; /* throw */
        }
        tkname = 0;
        {
            struct declarator * p_temp_declarator;

            p_temp_declarator = declarator(ctx, 0, p_declaration_specifiers, 0, &tkname);
            if (p_temp_declarator == 0)
            {
                goto __L0; /* throw */
            }
            p_init_declarator->p_declarator = p_temp_declarator;
        }
        if (tkname == 0)
        {
            compiler_diagnostic(650, ctx, ctx->current, 0, "init declarator must have a name");
            goto __L0; /* throw */
        }
        p_init_declarator->p_declarator->declaration_specifiers = p_declaration_specifiers;
        p_init_declarator->p_declarator->name_opt = tkname;
        if ((p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 16) && (p_init_declarator->p_declarator->declaration_specifiers->type_specifier_flags == 0))
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
        declarator_name = p_init_declarator->p_declarator->name_opt->lexeme;
        out_scope = 0;
        p_previous_declarator = find_declarator(ctx, declarator_name, &out_scope);
        if (p_previous_declarator)
        {
            p_init_declarator->p_declarator->p_complete_declarator = p_previous_declarator;
            ;
            ;
            if (out_scope->scope_level == ctx->scopes.tail->scope_level)
            {
                if (out_scope->scope_level == 0)
                {
                    if (strcmp(declarator_name, "__C_ASSERT__") != 0)
                    {
                        if (!type_is_same(&p_previous_declarator->type, &p_init_declarator->p_declarator->type, 0))
                        {
                            struct osstream  ss;

                            _cake_zmem(&ss, 12);
                            print_type_no_names(&ss, &p_previous_declarator->type, ctx->options.target);
                            compiler_diagnostic(1020, ctx, ctx->current, 0, "conflicting types for '%s' (%s)", declarator_name, ss.c_str);
                            ss_clear(&ss);
                            print_type_no_names(&ss, &p_init_declarator->p_declarator->type, ctx->options.target);
                            compiler_diagnostic(1020, ctx, p_previous_declarator->name_opt, 0, "previous declaration (%s)", ss.c_str);
                            ss_close(&ss);
                        }
                    }
                }
                else
                {
                    if (type_is_function(&p_init_declarator->p_declarator->type))
                    {
                    }
                    else
                    {
                        if (compiler_diagnostic(1020, ctx, ctx->current, 0, "redeclaration"))
                        {
                            compiler_diagnostic(63, ctx, p_previous_declarator->name_opt, 0, "previous declaration");
                        }
                    }
                }
            }
            else
            {
                struct hash_item_set  item;

                _cake_zmem(&item, 32);
                item.p_init_declarator = init_declarator_add_ref(p_init_declarator);
                hashmap_set(&ctx->scopes.tail->variables, declarator_name, &item);
                hash_item_set_destroy(&item);
                if (out_scope->scope_level != 0)
                {
                    if (compiler_diagnostic(7, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "declaration of '%s' hides previous declaration", declarator_name))
                    {
                        compiler_diagnostic(63, ctx, p_previous_declarator->first_token_opt, 0, "previous declaration is here");
                    }
                }
            }
        }
        else
        {
            struct hash_item_set  item;

            _cake_zmem(&item, 32);
            item.p_init_declarator = init_declarator_add_ref(p_init_declarator);
            hashmap_set(&ctx->scopes.tail->variables, declarator_name, &item);
            hash_item_set_destroy(&item);
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 61)
        {
            unsigned char  requires_constant_initialization;

            requires_constant_initialization = (ctx->p_current_function_opt == 0) || (p_declaration_specifiers->storage_class_specifier_flags & 4);
            parser_match(ctx);
            ;
            p_init_declarator->initializer = initializer(ctx, 2);
            if (p_init_declarator->initializer == 0)
            {
                goto __L0; /* throw */
            }
            if (p_init_declarator->initializer->braced_initializer)
            {
                int er;
                unsigned char  is_constant;

                if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 16)
                {
                    compiler_diagnostic(1290, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "'auto' requires a plain identifier");
                    goto __L0; /* throw */
                }
                er = make_object(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, ctx->options.target);
                if (er != 0)
                {
                    compiler_diagnostic(740, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "incomplete struct/union type");
                    goto __L0; /* throw */
                }
                is_constant = type_is_const_or_constexpr(&p_init_declarator->p_declarator->type);
                if (initializer_init_new(ctx, &p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, p_init_declarator->initializer, is_constant, requires_constant_initialization) != 0)
                {
                    goto __L0; /* throw */
                }
                p_init_declarator->p_declarator->object.type.num_of_elements = p_init_declarator->p_declarator->type.num_of_elements;
            }
            else
            {
                if (p_init_declarator->initializer->assignment_expression)
                {
                    char * name2;
                    int er;
                    unsigned char  is_constant;

                    if (type_is_array(&p_init_declarator->p_declarator->type))
                    {
                        unsigned long long array_size_elements;

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
                                    compiler_diagnostic(52, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "initializer for array is too long");
                                }
                            }
                        }
                    }
                    ;
                    if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 16)
                    {
                        struct type  t;

                        if (p_init_declarator->p_declarator->direct_declarator && (p_init_declarator->p_declarator->direct_declarator->array_declarator != 0 || p_init_declarator->p_declarator->direct_declarator->function_declarator != 0))
                        {
                            compiler_diagnostic(1290, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "'auto' requires a plain identifier");
                            goto __L0; /* throw */
                        }
                        if (p_init_declarator->p_declarator->pointer != 0)
                        {
                            compiler_diagnostic(1290, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "'auto' requires a plain identifier");
                        }
                        _cake_zmem(&t, 68);
                        if (p_init_declarator->initializer->assignment_expression->expression_type == 40)
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
                        t.storage_class_specifier_flags = 0;
                        type_set_storage_specifiers_using_declarator(&t, p_init_declarator->p_declarator);
                        t.storage_class_specifier_flags &= -17;
                        type_set_qualifiers_using_declarator(&t, p_init_declarator->p_declarator);
                        type_visit_to_mark_anonymous(&t);
                        type_swap(&p_init_declarator->p_declarator->type, &t);
                        type_destroy(&t);
                    }
                    check_assigment(ctx, &p_init_declarator->p_declarator->type, p_init_declarator->initializer->assignment_expression, 3);
                    name2 = p_init_declarator->p_declarator->name_opt ? p_init_declarator->p_declarator->name_opt->lexeme : "";
                    er = make_object_with_member_designator(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, name2, ctx->options.target);
                    if (er != 0)
                    {
                        goto __L0; /* throw */
                    }
                    is_constant = type_is_const_or_constexpr(&p_init_declarator->p_declarator->type);
                    if (initializer_init_new(ctx, &p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, p_init_declarator->initializer, is_constant, requires_constant_initialization) != 0)
                    {
                        goto __L0; /* throw */
                    }
                }
            }
        }
        else
        {
            if (p_init_declarator->p_declarator->type.category != 1 && !(p_init_declarator->p_declarator->type.storage_class_specifier_flags & 1))
            {
                char * name2;
                int er;

                name2 = p_init_declarator->p_declarator->name_opt ? p_init_declarator->p_declarator->name_opt->lexeme : "";
                er = make_object_with_member_designator(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, name2, ctx->options.target);
                if (er != 0)
                {
                    if (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & 2)
                    {
                    }
                    else
                    {
                        compiler_diagnostic(740, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "incomplete struct/union type");
                        goto __L0; /* throw */
                    }
                }
                if (type_is_const(&p_init_declarator->p_declarator->type))
                {
                    if (p_declaration_specifiers->storage_class_specifier_flags & 1)
                    {
                    }
                    else
                    {
                        compiler_diagnostic(48, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "const object should be initialized");
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
                        compiler_diagnostic(1000, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "static or type qualifiers are not allowed in non-parameter array declarator");
                    }
                    else
                    {
                        if (p_init_declarator->initializer)
                        {
                            compiler_diagnostic(1000, ctx, p_init_declarator->initializer->first_token, 0, "static or type qualifiers are not allowed in non-parameter array declarator");
                        }
                    }
                }
            }
            if (!type_is_pointer(&p_init_declarator->p_declarator->type) && p_init_declarator->p_declarator->type.type_qualifier_flags & 128)
            {
                if (p_init_declarator->p_declarator->first_token_opt)
                {
                    compiler_diagnostic(1010, ctx, p_init_declarator->p_declarator->first_token_opt, 0, "_Dtor qualifier can only be used with pointers");
                }
                else
                {
                    if (p_init_declarator->initializer)
                    {
                        compiler_diagnostic(1010, ctx, p_init_declarator->initializer->first_token, 0, "_Dtor qualifier can only be used with pointers");
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
                if (type_is_function(&p_init_declarator->p_declarator->type))
                {
                    compiler_diagnostic(1270, ctx, p_init_declarator->p_declarator->name_opt, 0, "invalid application of 'sizeof' to a function type");
                }
                else
                {
                    unsigned int sz;
                    int size_result;

                    sz = 0;
                    size_result = type_get_sizeof(&p_init_declarator->p_declarator->type, &sz, ctx->options.target);
                    if (size_result == 0)
                    {
                    }
                    else
                    {
                        if (size_result == 3)
                        {
                            if (p_init_declarator->p_declarator->type.storage_class_specifier_flags & 2)
                            {
                            }
                            else
                            {
                                compiler_diagnostic(1270, ctx, p_init_declarator->p_declarator->name_opt, 0, "storage size of '%s' isn't known", p_init_declarator->p_declarator->name_opt->lexeme);
                            }
                        }
                        else
                        {
                            if (size_result == 1)
                            {
                                compiler_diagnostic(1270, ctx, p_init_declarator->p_declarator->name_opt, 0, "sizeof '%s' is too large", p_init_declarator->p_declarator->name_opt->lexeme);
                            }
                            else
                            {
                                compiler_diagnostic(1270, ctx, p_init_declarator->p_declarator->name_opt, 0, "storage size of '%s' isn't known", p_init_declarator->p_declarator->name_opt->lexeme);
                            }
                        }
                    }
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        init_declarator_delete(p_init_declarator);
        p_init_declarator = 0;
    }
    return p_init_declarator;
}


void init_declarator_list_add(struct init_declarator_list * list, struct init_declarator * p_item)
{
    if (list->head == 0)
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
        item->next = 0;
        init_declarator_delete(item);
        item = next;
    }
}


struct init_declarator_list init_declarator_list(struct parser_ctx * ctx, struct declaration_specifiers * p_declaration_specifiers)
{
    struct init_declarator_list  init_declarator_list;
    struct init_declarator * p_init_declarator;

    _cake_zmem(&init_declarator_list, 8);
    p_init_declarator = 0;
    if (1) /*try*/
    {
        p_init_declarator = init_declarator(ctx, p_declaration_specifiers);
        if (p_init_declarator == 0)
        {
            goto __L0; /* throw */
        }
        init_declarator_list_add(&init_declarator_list, p_init_declarator);
        p_init_declarator = 0;
        while (ctx->current != 0 && ctx->current->type == 44)
        {
            parser_match(ctx);
            p_init_declarator = init_declarator(ctx, p_declaration_specifiers);
            if (p_init_declarator == 0)
            {
                goto __L0; /* throw */
            }
            init_declarator_list_add(&init_declarator_list, p_init_declarator);
            p_init_declarator = 0;
        }
    }
    else __L0: /*catch*/ 
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

    p_storage_class_specifier = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_storage_class_specifier = calloc(1, 8);
        if (p_storage_class_specifier == 0)
        {
            goto __L0; /* throw */
        }
        p_storage_class_specifier->token = ctx->current;
        /*switch*/
        {
            int __v0 = ctx->current->type;
            if (__v0 == 9035) goto __L4; /*case 9035*/
            if (__v0 == 9013) goto __L5; /*case 9013*/
            if (__v0 == 9002) goto __L6; /*case 9002*/
            if (__v0 == 9032) goto __L8; /*case 9032*/
            if (__v0 == 9070) goto __L9; /*case 9070*/
            if (__v0 == 8999) goto __L10; /*case 8999*/
            if (__v0 == 9025) goto __L11; /*case 9025*/
            goto __L12; /* default */

            {
                __L4: /*case 9035*/ 
                p_storage_class_specifier->flags = 1;
                goto __L3; /* break */

                __L5: /*case 9013*/ 
                p_storage_class_specifier->flags = 2;
                goto __L3; /* break */

                __L6: /*case 9002*/ 
                p_storage_class_specifier->flags = 64;
                if (ctx->scopes.tail && ctx->scopes.tail->scope_level == 0)
                {
                    p_storage_class_specifier->flags |= 128;
                }
                goto __L3; /* break */

                __L8: /*case 9032*/ 
                p_storage_class_specifier->flags = 4;
                goto __L3; /* break */

                __L9: /*case 9070*/ 
                p_storage_class_specifier->flags = 8;
                goto __L3; /* break */

                __L10: /*case 8999*/ 
                p_storage_class_specifier->flags = 16;
                goto __L3; /* break */

                __L11: /*case 9025*/ 
                p_storage_class_specifier->flags = 32;
                goto __L3; /* break */

                __L12: /* default */ 
                ;
            }
            __L3:;
        }
        parser_match(ctx);
    }
    else __L0: /*catch*/ 
    {
        storage_class_specifier_delete(p_storage_class_specifier);
        p_storage_class_specifier = 0;
    }
    return p_storage_class_specifier;
}


struct type_name *type_name(struct parser_ctx * ctx);
void typeof_specifier_argument_delete(struct typeof_specifier_argument * p);

struct typeof_specifier_argument *typeof_specifier_argument(struct parser_ctx * ctx)
{
    struct typeof_specifier_argument * new_typeof_specifier_argument;

    new_typeof_specifier_argument = 0;
    if (1) /*try*/
    {
        new_typeof_specifier_argument = calloc(1, 8);
        if (new_typeof_specifier_argument == 0)
        {
            goto __L0; /* throw */
        }
        if (first_of_type_name(ctx))
        {
            new_typeof_specifier_argument->type_name = type_name(ctx);
        }
        else
        {
            new_typeof_specifier_argument->expression = expression(ctx, 1);
            if (new_typeof_specifier_argument->expression == 0)
            {
                goto __L0; /* throw */
            }
        }
    }
    else __L0: /*catch*/ 
    {
        typeof_specifier_argument_delete(new_typeof_specifier_argument);
        new_typeof_specifier_argument = 0;
    }
    return new_typeof_specifier_argument;
}


unsigned char first_of_typeof_specifier(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 9071 || ctx->current->type == 9075;
}


struct type type_param_array_to_pointer(struct type * p_type, unsigned char  null_checks_enabled);
void type_remove_qualifiers(struct type * p_type);
void typeof_specifier_delete(struct typeof_specifier * p);

struct typeof_specifier *typeof_specifier(struct parser_ctx * ctx)
{
    struct typeof_specifier * p_typeof_specifier;

    p_typeof_specifier = 0;
    if (1) /*try*/
    {
        unsigned char  is_typeof_unqual;
        struct typeof_specifier_argument * p_typeof_specifier_argument;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_typeof_specifier = calloc(1, 80);
        if (p_typeof_specifier == 0)
        {
            goto __L0; /* throw */
        }
        p_typeof_specifier->first_token = ctx->current;
        is_typeof_unqual = ctx->current->type == 9075;
        parser_match(ctx);
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto __L0; /* throw */
        }
        p_typeof_specifier_argument = typeof_specifier_argument(ctx);
        if (p_typeof_specifier_argument == 0)
        {
            goto __L0; /* throw */
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
        if (type_is_array(&p_typeof_specifier->type) && p_typeof_specifier->type.storage_class_specifier_flags & 2048)
        {
            compiler_diagnostic(8, ctx, ctx->current, 0, "typeof used in array arguments");
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
        p_typeof_specifier->type.name_opt = 0;
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_typeof_specifier->last_token = ctx->current;
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        typeof_specifier_delete(p_typeof_specifier);
        p_typeof_specifier = 0;
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


int __cdecl atoi(char * _String);
void attribute_delete(struct attribute * p);

struct attribute *extended_decl_modifier_seq(struct parser_ctx * ctx)
{
    struct attribute * p_type_specifier;

    p_type_specifier = 0;
    if (1) /*try*/
    {
        int count;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_type_specifier = calloc(1, 20);
        if (p_type_specifier == 0)
        {
            goto __L0; /* throw */
        }
        p_type_specifier->attribute_token = ctx->current;
        parser_match(ctx);
        if (strcmp(p_type_specifier->attribute_token->lexeme, "align") == 0)
        {
            int a;

            parser_match(ctx);
            a = atoi(ctx->current->lexeme);
            if (a == 8)
            {
                p_type_specifier->msvc_declspec_flags |= 2;
            }
            else
            {
                if (a == 16)
                {
                    p_type_specifier->msvc_declspec_flags |= 4;
                }
                else
                {
                    if (a == 32)
                    {
                        p_type_specifier->msvc_declspec_flags |= 8;
                    }
                    else
                    {
                        if (a == 64)
                        {
                            p_type_specifier->msvc_declspec_flags |= 16;
                        }
                    }
                }
            }
            parser_match(ctx);
            parser_match(ctx);
        }
        else
        {
            if (strcmp(p_type_specifier->attribute_token->lexeme, "allocate") == 0)
            {
                p_type_specifier->msvc_declspec_flags |= 32;
            }
            else
            {
                if (strcmp(p_type_specifier->attribute_token->lexeme, "allocator") == 0)
                {
                    p_type_specifier->msvc_declspec_flags |= 64;
                }
                else
                {
                    if (strcmp(p_type_specifier->attribute_token->lexeme, "appdomain") == 0)
                    {
                        p_type_specifier->msvc_declspec_flags |= 128;
                    }
                    else
                    {
                        if (strcmp(p_type_specifier->attribute_token->lexeme, "code_seg") == 0)
                        {
                            p_type_specifier->msvc_declspec_flags |= 256;
                        }
                        else
                        {
                            if (strcmp(p_type_specifier->attribute_token->lexeme, "deprecated") == 0)
                            {
                                p_type_specifier->msvc_declspec_flags |= 512;
                            }
                            else
                            {
                                if (strcmp(p_type_specifier->attribute_token->lexeme, "dllimport") == 0)
                                {
                                    p_type_specifier->msvc_declspec_flags |= 1024;
                                }
                                else
                                {
                                    if (strcmp(p_type_specifier->attribute_token->lexeme, "dllexport") == 0)
                                    {
                                        p_type_specifier->msvc_declspec_flags |= 2048;
                                    }
                                    else
                                    {
                                        if (strcmp(p_type_specifier->attribute_token->lexeme, "hybrid_patchable") == 0)
                                        {
                                        }
                                        else
                                        {
                                            if (strcmp(p_type_specifier->attribute_token->lexeme, "no_init_all") == 0)
                                            {
                                            }
                                            else
                                            {
                                                if (strcmp(p_type_specifier->attribute_token->lexeme, "jitintrinsic") == 0)
                                                {
                                                }
                                                else
                                                {
                                                    if (strcmp(p_type_specifier->attribute_token->lexeme, "intrin_type") == 0)
                                                    {
                                                    }
                                                    else
                                                    {
                                                        if (strcmp(p_type_specifier->attribute_token->lexeme, "naked") == 0)
                                                        {
                                                        }
                                                        else
                                                        {
                                                            if (strcmp(p_type_specifier->attribute_token->lexeme, "noalias") == 0)
                                                            {
                                                            }
                                                            else
                                                            {
                                                                if (strcmp(p_type_specifier->attribute_token->lexeme, "noinline") == 0)
                                                                {
                                                                }
                                                                else
                                                                {
                                                                    if (strcmp(p_type_specifier->attribute_token->lexeme, "noreturn") == 0)
                                                                    {
                                                                    }
                                                                    else
                                                                    {
                                                                        if (strcmp(p_type_specifier->attribute_token->lexeme, "nothrow") == 0)
                                                                        {
                                                                        }
                                                                        else
                                                                        {
                                                                            if (strcmp(p_type_specifier->attribute_token->lexeme, "novtable") == 0)
                                                                            {
                                                                            }
                                                                            else
                                                                            {
                                                                                if (strcmp(p_type_specifier->attribute_token->lexeme, "no_sanitize_address") == 0)
                                                                                {
                                                                                }
                                                                                else
                                                                                {
                                                                                    if (strcmp(p_type_specifier->attribute_token->lexeme, "process") == 0)
                                                                                    {
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        if (strcmp(p_type_specifier->attribute_token->lexeme, "property") == 0)
                                                                                        {
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            if (strcmp(p_type_specifier->attribute_token->lexeme, "restrict") == 0)
                                                                                            {
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                if (strcmp(p_type_specifier->attribute_token->lexeme, "safebuffers") == 0)
                                                                                                {
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    if (strcmp(p_type_specifier->attribute_token->lexeme, "selectany") == 0)
                                                                                                    {
                                                                                                        p_type_specifier->msvc_declspec_flags |= 67108864;
                                                                                                    }
                                                                                                    else
                                                                                                    {
                                                                                                        if (strcmp(p_type_specifier->attribute_token->lexeme, "thread") == 0)
                                                                                                        {
                                                                                                            p_type_specifier->msvc_declspec_flags |= 67108864;
                                                                                                        }
                                                                                                        else
                                                                                                        {
                                                                                                            if (strcmp(p_type_specifier->attribute_token->lexeme, "uuid") == 0)
                                                                                                            {
                                                                                                                p_type_specifier->msvc_declspec_flags |= 536870912;
                                                                                                            }
                                                                                                            else
                                                                                                            {
                                                                                                                compiler_diagnostic(9, ctx, p_type_specifier->attribute_token, 0, "unknown '%s'\n", p_type_specifier->attribute_token->lexeme);
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
        count = 1;
        while (ctx->current)
        {
            if (ctx->current->type == 40)
            {
                parser_match(ctx);
                count++;
            }
            else
            {
                if (ctx->current->type == 41)
                {
                    if (count == 1)
                    {
                        break;
                    }
                    count--;
                    parser_match(ctx);
                }
                else
                {
                    parser_match(ctx);
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        attribute_delete(p_type_specifier);
        p_type_specifier = 0;
    }
    return p_type_specifier;
}


struct attribute *msvc_declspec(struct parser_ctx * ctx)
{
    struct attribute * p_decl_specifier;

    p_decl_specifier = 0;
    if (1) /*try*/
    {
        if (ctx->current->type != 9058)
        {
            goto __L0; /* throw */
        }
        parser_match_tk(ctx, 9058);
        parser_match_tk(ctx, 40);
        p_decl_specifier = extended_decl_modifier_seq(ctx);
        parser_match_tk(ctx, 41);
    }
    else __L0: /*catch*/ 
    {
        attribute_delete(p_decl_specifier);
        p_decl_specifier = 0;
    }
    return p_decl_specifier;
}


int msvc_declspec_sequence_opt(struct parser_ctx * ctx)
{
    int msvc_declspec_flags;

    msvc_declspec_flags = 0;
    if (1) /*try*/
    {
        for (; ; )
        {
            struct attribute * p;

            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (ctx->current->type != 9058)
            {
                return msvc_declspec_flags;
            }
            p = msvc_declspec(ctx);
            if (p)
            {
                msvc_declspec_flags |= p->msvc_declspec_flags;
            }
            attribute_delete(p);
        }
    }
    else __L0: /*catch*/ 
    {
    }
    return msvc_declspec_flags;
}


static void gcc_attribute_list(struct parser_ctx * ctx);
static void gcc_attribute(struct parser_ctx * ctx);
unsigned char token_is_identifier_or_keyword(int t);
static void gcc_attribute_argument_list(struct parser_ctx * ctx);
static void gcc_attribute_argument(struct parser_ctx * ctx);
struct expression *constant_expression(struct parser_ctx * ctx, unsigned char  show_error_if_not_constant, int eval_mode);

void gcc_attribute_specifier_opt(struct parser_ctx * ctx)
{
    if (ctx->current == 0 || ctx->current->type != 9046)
    {
        return;
    }
    parser_match(ctx);
    parser_match_tk(ctx, 40);
    parser_match_tk(ctx, 40);
    gcc_attribute_list(ctx);
    parser_match_tk(ctx, 41);
    parser_match_tk(ctx, 41);
}



static void gcc_attribute_argument(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        unexpected_end_of_file(ctx);
        return;
    }
    if (token_is_identifier_or_keyword(ctx->current->type))
    {
        parser_match(ctx);
    }
    else
    {
        if (ctx->current->type == 130)
        {
            parser_match(ctx);
        }
        else
        {
            struct expression * expr;

            expr = constant_expression(ctx, 1, 2);
            expression_delete(expr);
        }
    }
}

static void gcc_attribute_argument_list(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        unexpected_end_of_file(ctx);
        return;
    }
    for (; ; )
    {
        gcc_attribute_argument(ctx);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            return;
        }
        if (ctx->current->type != 44)
        {
            break;
        }
        parser_match(ctx);
    }
}

static void gcc_attribute(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        unexpected_end_of_file(ctx);
        return;
    }
    if (!token_is_identifier_or_keyword(ctx->current->type))
    {
        compiler_diagnostic(650, ctx, ctx->current, 0, "expected identifier");
        return;
    }
    parser_match(ctx);
    if (ctx->current->type == 40)
    {
        parser_match(ctx);
        if (ctx->current->type != 41)
        {
            gcc_attribute_argument_list(ctx);
        }
        parser_match_tk(ctx, 41);
    }
}

static void gcc_attribute_list(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        unexpected_end_of_file(ctx);
        return;
    }
    if (ctx->current->type == 41)
    {
        return;
    }
    for (; ; )
    {
        gcc_attribute(ctx);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            return;
        }
        if (ctx->current->type != 44)
        {
            break;
        }
        parser_match(ctx);
    }
}
int object_type_to_type_specifier(int type);
struct platform *get_platform(int target);
struct atomic_type_specifier *atomic_type_specifier(struct parser_ctx * ctx);
struct struct_or_union_specifier *struct_or_union_specifier(struct parser_ctx * ctx);
struct enum_specifier *enum_specifier(struct parser_ctx *);

struct type_specifier *type_specifier(struct parser_ctx * ctx)
{
    struct type_specifier * p_type_specifier;

    p_type_specifier = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_type_specifier = calloc(1, 28);
        if (p_type_specifier == 0)
        {
            goto __L0; /* throw */
        }
        /*switch*/
        {
            int __v0 = ctx->current->type;
            if (__v0 == 9040) goto __L4; /*case 9040*/
            if (__v0 == 9003) goto __L5; /*case 9003*/
            if (__v0 == 9028) goto __L6; /*case 9028*/
            if (__v0 == 9019) goto __L7; /*case 9019*/
            if (__v0 == 9047) goto __L8; /*case 9047*/
            if (__v0 == 9021) goto __L9; /*case 9021*/
            if (__v0 == 9022) goto __L10; /*case 9022*/
            if (__v0 == 9023) goto __L11; /*case 9023*/
            if (__v0 == 9024) goto __L12; /*case 9024*/
            if (__v0 == 9020) goto __L13; /*case 9020*/
            if (__v0 == 9014) goto __L14; /*case 9014*/
            if (__v0 == 9010) goto __L15; /*case 9010*/
            if (__v0 == 9029) goto __L16; /*case 9029*/
            if (__v0 == 9039) goto __L17; /*case 9039*/
            if (__v0 == 9060) goto __L18; /*case 9060*/
            if (__v0 == 9061) goto __L19; /*case 9061*/
            if (__v0 == 9063) goto __L20; /*case 9063*/
            if (__v0 == 9064) goto __L21; /*case 9064*/
            if (__v0 == 9062) goto __L22; /*case 9062*/
            goto __L23; /* default */

            {
                __L4: /*case 9040*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 1;
                parser_match(ctx);
                return p_type_specifier;
                __L5: /*case 9003*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 2;
                parser_match(ctx);
                return p_type_specifier;
                __L6: /*case 9028*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 4;
                parser_match(ctx);
                return p_type_specifier;
                __L7: /*case 9019*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 8;
                parser_match(ctx);
                return p_type_specifier;
                __L8: /*case 9047*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 33554432;
                parser_match(ctx);
                return p_type_specifier;
                __L9: /*case 9021*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = object_type_to_type_specifier(get_platform(ctx->options.target)->int8_type) & -129;
                parser_match(ctx);
                return p_type_specifier;
                __L10: /*case 9022*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = object_type_to_type_specifier(get_platform(ctx->options.target)->int16_type);
                parser_match(ctx);
                return p_type_specifier;
                __L11: /*case 9023*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = object_type_to_type_specifier(get_platform(ctx->options.target)->int32_type);
                parser_match(ctx);
                return p_type_specifier;
                __L12: /*case 9024*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = object_type_to_type_specifier(get_platform(ctx->options.target)->int64_type);
                parser_match(ctx);
                return p_type_specifier;
                __L13: /*case 9020*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 16;
                parser_match(ctx);
                return p_type_specifier;
                __L14: /*case 9014*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 32;
                parser_match(ctx);
                return p_type_specifier;
                __L15: /*case 9010*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 64;
                parser_match(ctx);
                return p_type_specifier;
                __L16: /*case 9029*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 128;
                parser_match(ctx);
                return p_type_specifier;
                __L17: /*case 9039*/ 
                p_type_specifier->flags = 256;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                __L18: /*case 9060*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 512;
                parser_match(ctx);
                return p_type_specifier;
                __L19: /*case 9061*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 1024;
                parser_match(ctx);
                return p_type_specifier;
                __L20: /*case 9063*/ 
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 2048;
                parser_match(ctx);
                return p_type_specifier;
                __L21: /*case 9064*/ 
                p_type_specifier->flags = 4096;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                __L22: /*case 9062*/ 
                p_type_specifier->flags = 8192;
                p_type_specifier->token = ctx->current;
                parser_match(ctx);
                return p_type_specifier;
                __L23: /* default */ 
                goto __L3; /* break */

            }
            __L3:;
        }
        if (first_of_typeof_specifier(ctx))
        {
            p_type_specifier->token = ctx->current;
            p_type_specifier->flags = 8388608;
            p_type_specifier->typeof_specifier = typeof_specifier(ctx);
            if (p_type_specifier->typeof_specifier == 0)
            {
                type_specifier_delete(p_type_specifier);
                return 0;
            }
        }
        else
        {
            if (first_of_atomic_type_specifier(ctx))
            {
                p_type_specifier->token = ctx->current;
                p_type_specifier->flags = 16384;
                p_type_specifier->atomic_type_specifier = atomic_type_specifier(ctx);
                if (p_type_specifier->atomic_type_specifier == 0)
                {
                    type_specifier_delete(p_type_specifier);
                    return 0;
                }
            }
            else
            {
                if (first_of_struct_or_union(ctx))
                {
                    p_type_specifier->token = ctx->current;
                    p_type_specifier->flags = 32768;
                    p_type_specifier->struct_or_union_specifier = struct_or_union_specifier(ctx);
                    if (p_type_specifier->struct_or_union_specifier == 0)
                    {
                        type_specifier_delete(p_type_specifier);
                        return 0;
                    }
                }
                else
                {
                    if (first_of_enum_specifier(ctx))
                    {
                        p_type_specifier->token = ctx->current;
                        p_type_specifier->flags = 65536;
                        p_type_specifier->enum_specifier = enum_specifier(ctx);
                        if (p_type_specifier->enum_specifier == 0)
                        {
                            type_specifier_delete(p_type_specifier);
                            return 0;
                        }
                    }
                    else
                    {
                        if (ctx->current->type == 9076)
                        {
                            type_specifier_delete(p_type_specifier);
                            return 0;
                        }
                        else
                        {
                            if (ctx->current->type == 8996)
                            {
                                p_type_specifier->token = ctx->current;
                                p_type_specifier->flags = 131072;
                                p_type_specifier->typedef_declarator = find_declarator(ctx, ctx->current->lexeme, 0);
                                ;
                                parser_match(ctx);
                            }
                        }
                    }
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        type_specifier_delete(p_type_specifier);
        p_type_specifier = 0;
    }
    return p_type_specifier;
}


int get_enum_type_specifier_flags(struct enum_specifier * p_enum_specifier)
{
    if (p_enum_specifier->specifier_qualifier_list)
    {
        return p_enum_specifier->specifier_qualifier_list->type_specifier_flags;
    }
    if (p_enum_specifier->p_complete_enum_specifier && p_enum_specifier->p_complete_enum_specifier->specifier_qualifier_list)
    {
        return p_enum_specifier->p_complete_enum_specifier->specifier_qualifier_list->type_specifier_flags;
    }
    else
    {
        if (p_enum_specifier->p_complete_enum_specifier && p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier && p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier->specifier_qualifier_list)
        {
            return p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier->specifier_qualifier_list->type_specifier_flags;
        }
    }
    return 8;
}


struct enum_specifier *get_complete_enum_specifier(struct enum_specifier * p_enum_specifier)
{
    if (p_enum_specifier->enumerator_list.head)
    {
        return p_enum_specifier;
    }
    else
    {
        if (p_enum_specifier->p_complete_enum_specifier && p_enum_specifier->p_complete_enum_specifier->enumerator_list.head)
        {
            return p_enum_specifier->p_complete_enum_specifier;
        }
        else
        {
            if (p_enum_specifier->p_complete_enum_specifier && p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier && p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier->enumerator_list.head)
            {
                return p_enum_specifier->p_complete_enum_specifier->p_complete_enum_specifier;
            }
        }
    }
    return 0;
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
    return 0;
}


unsigned char struct_or_union_specifier_is_complete(struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    return get_complete_struct_or_union_specifier(p_struct_or_union_specifier) != 0;
}


struct struct_or_union_specifier *struct_or_union_specifier_add_ref(struct struct_or_union_specifier * p)
{
    p->has_shared_ownership = 1;
    return (struct struct_or_union_specifier *)p;
}


void struct_or_union_specifier_sink(struct struct_or_union_specifier * p)
{
}


unsigned char struct_or_union_specifier_is_union(struct struct_or_union_specifier * p)
{
    return p->first_token->type == 9038;
}


void member_declaration_list_destroy(struct member_declaration_list * p);

void struct_or_union_specifier_delete(struct struct_or_union_specifier * p)
{
    if (p)
    {
        if (p->has_shared_ownership)
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


static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
void naming_convention_struct_tag(struct parser_ctx * ctx, struct token * token);
struct member_declaration_list member_declaration_list(struct parser_ctx * ctx, struct struct_or_union_specifier *);

struct struct_or_union_specifier *struct_or_union_specifier(struct parser_ctx * ctx)
{
    struct struct_or_union_specifier * p_struct_or_union_specifier;

    p_struct_or_union_specifier = 0;
    if (1) /*try*/
    {
        struct struct_or_union_specifier * p_first_tag_in_this_scope;
        struct struct_or_union_specifier * p_complete;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_struct_or_union_specifier = calloc(1, 260);
        if (p_struct_or_union_specifier == 0)
        {
            goto __L0; /* throw */
        }
        ctx->unique_tag_id++;
        p_struct_or_union_specifier->unique_id = ctx->unique_tag_id;
        if (ctx->current->type == 9033 || ctx->current->type == 9038)
        {
            p_struct_or_union_specifier->first_token = ctx->current;
            parser_match(ctx);
        }
        else
        {
            goto __L0; /* throw */
        }
        ;
        p_struct_or_union_specifier->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        p_first_tag_in_this_scope = 0;
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 8996)
        {
            unsigned char  is_struct_definition;
            struct map_entry * p_entry;

            p_struct_or_union_specifier->tagtoken = ctx->current;
            snprintf(p_struct_or_union_specifier->tag_name, 200, "%s", p_struct_or_union_specifier->tagtoken->lexeme);
            parser_match(ctx);
            is_struct_definition = (ctx->current->type == 123);
            p_entry = hashmap_find(&ctx->scopes.tail->tags, p_struct_or_union_specifier->tagtoken->lexeme);
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
                    compiler_diagnostic(1030, ctx, p_struct_or_union_specifier->tagtoken, 0, "use of '%s' with tag type that does not match previous declaration.", p_struct_or_union_specifier->tagtoken->lexeme);
                }
            }
            else
            {
                if (is_struct_definition)
                {
                    struct hash_item_set  item;

                    _cake_zmem(&item, 32);
                    item.p_struct_or_union_specifier = struct_or_union_specifier_add_ref(p_struct_or_union_specifier);
                    hashmap_set(&ctx->scopes.tail->tags, p_struct_or_union_specifier->tagtoken->lexeme, &item);
                    hash_item_set_destroy(&item);
                    p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection = p_struct_or_union_specifier;
                }
                else
                {
                    struct struct_or_union_specifier * p_first_tag_previous_scopes;

                    p_first_tag_previous_scopes = find_struct_or_union_specifier(ctx, p_struct_or_union_specifier->tagtoken->lexeme);
                    if (p_first_tag_previous_scopes == 0)
                    {
                        struct hash_item_set  item;

                        p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
                        _cake_zmem(&item, 32);
                        item.p_struct_or_union_specifier = struct_or_union_specifier_add_ref(p_struct_or_union_specifier);
                        hashmap_set(&ctx->scopes.tail->tags, p_struct_or_union_specifier->tagtoken->lexeme, &item);
                        hash_item_set_destroy(&item);
                    }
                    else
                    {
                        p_struct_or_union_specifier->complete_struct_or_union_specifier_indirection = p_first_tag_previous_scopes;
                    }
                }
            }
        }
        else
        {
            struct hash_item_set  item;

            snprintf(p_struct_or_union_specifier->tag_name, 200, "__tag%d", ctx->anonymous_struct_count++);
            p_struct_or_union_specifier->has_anonymous_tag = 1;
            p_struct_or_union_specifier->scope_level = ctx->scopes.tail->scope_level;
            _cake_zmem(&item, 32);
            item.p_struct_or_union_specifier = struct_or_union_specifier_add_ref(p_struct_or_union_specifier);
            hashmap_set(&ctx->scopes.tail->tags, p_struct_or_union_specifier->tag_name, &item);
            hash_item_set_destroy(&item);
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
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
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            firsttoken = ctx->current;
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (ctx->current->type != 125)
            {
                p_struct_or_union_specifier->member_declaration_list = member_declaration_list(ctx, p_struct_or_union_specifier);
                if (p_struct_or_union_specifier->member_declaration_list.head == 0)
                {
                    goto __L0; /* throw */
                }
            }
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            p_struct_or_union_specifier->member_declaration_list.first_token = firsttoken;
            p_struct_or_union_specifier->last_token = ctx->current;
            p_struct_or_union_specifier->member_declaration_list.last_token = ctx->current;
            if (parser_match_tk(ctx, 125) != 0)
            {
                goto __L0; /* throw */
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
                    compiler_diagnostic(3, ctx, p_struct_or_union_specifier->first_token, 0, "'%s' is deprecated", p_struct_or_union_specifier->tagtoken->lexeme);
                }
                else
                {
                    compiler_diagnostic(3, ctx, p_struct_or_union_specifier->first_token, 0, "deprecated");
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        struct_or_union_specifier_delete(p_struct_or_union_specifier);
        p_struct_or_union_specifier = 0;
    }
    return p_struct_or_union_specifier;
}



static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}
unsigned char type_is_owner(struct type * p_type);
void naming_convention_struct_member(struct parser_ctx * ctx, struct token * token, struct type * type);
void member_declarator_delete(struct member_declarator * p);

struct member_declarator *member_declarator(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier, struct specifier_qualifier_list * p_specifier_qualifier_list)
{
    struct member_declarator * p_member_declarator;

    p_member_declarator = 0;
    if (1) /*try*/
    {
        struct token * p_token_name;

        p_member_declarator = calloc(1, 12);
        if (p_member_declarator == 0)
        {
            goto __L0; /* throw */
        }
        p_token_name = 0;
        p_member_declarator->declarator = declarator(ctx, p_specifier_qualifier_list, 0, 0, &p_token_name);
        if (p_member_declarator->declarator == 0)
        {
            goto __L0; /* throw */
        }
        p_member_declarator->declarator->name_opt = p_token_name;
        p_member_declarator->declarator->specifier_qualifier_list = p_specifier_qualifier_list;
        p_member_declarator->declarator->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
        if (type_is_function(&p_member_declarator->declarator->type))
        {
            struct token * p_token;

            p_token = p_member_declarator->declarator->first_token_opt;
            if (p_token == 0)
            {
                p_token = ctx->current;
            }
            compiler_diagnostic(1840, ctx, p_token, 0, "members having a function type are not allowed");
            goto __L0; /* throw */
        }
        if (type_is_owner(&p_member_declarator->declarator->type))
        {
            p_struct_or_union_specifier->is_owner = 1;
        }
        if (p_member_declarator->declarator->name_opt)
        {
            naming_convention_struct_member(ctx, p_member_declarator->declarator->name_opt, &p_member_declarator->declarator->type);
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 58)
        {
            parser_match(ctx);
            p_member_declarator->constant_expression = constant_expression(ctx, 1, 2);
        }
    }
    else __L0: /*catch*/ 
    {
        member_declarator_delete(p_member_declarator);
        p_member_declarator = 0;
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
    if (list->head == 0)
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
            item->next = 0;
            member_declarator_delete(item);
            item = next;
        }
        free(p);
    }
}


struct member_declarator_list *member_declarator_list(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier, struct specifier_qualifier_list * p_specifier_qualifier_list)
{
    struct member_declarator_list * p_member_declarator_list;

    p_member_declarator_list = calloc(1, 8);
    if (1) /*try*/
    {
        struct member_declarator * p_member_declarator;

        if (p_member_declarator_list == 0)
        {
            goto __L0; /* throw */
        }
        p_member_declarator = member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list);
        if (p_member_declarator == 0)
        {
            goto __L0; /* throw */
        }
        member_declarator_list_add(p_member_declarator_list, p_member_declarator);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        while (ctx->current->type == 44)
        {
            struct member_declarator * p_member_declarator2;

            parser_match(ctx);
            p_member_declarator2 = member_declarator(ctx, p_struct_or_union_specifier, p_specifier_qualifier_list);
            if (p_member_declarator2 == 0)
            {
                goto __L0; /* throw */
            }
            member_declarator_list_add(p_member_declarator_list, p_member_declarator2);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
        }
    }
    else __L0: /*catch*/ 
    {
        member_declarator_list_delete(p_member_declarator_list);
        p_member_declarator_list = 0;
    }
    return p_member_declarator_list;
}


void member_declaration_list_add(struct member_declaration_list * list, struct member_declaration * p_item)
{
    if (list->head == 0)
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
        item->next = 0;
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
    p_member_declaration = 0;
    if (1) /*try*/
    {
        p_member_declaration = member_declaration(ctx, p_struct_or_union_specifier);
        if (p_member_declaration == 0)
        {
            goto __L0; /* throw */
        }
        member_declaration_list_add(&list, p_member_declaration);
        p_member_declaration = 0;
        while (ctx->current && ctx->current->type != 125)
        {
            p_member_declaration = member_declaration(ctx, p_struct_or_union_specifier);
            if (p_member_declaration == 0)
            {
                goto __L0; /* throw */
            }
            member_declaration_list_add(&list, p_member_declaration);
            p_member_declaration = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        struct member_declaration_list  __v0;

        member_declaration_list_destroy(&list);
        __v0.first_token = 0;
        __v0.last_token = 0;
        __v0.head = 0;
        __v0.tail = 0;
        list = __v0;
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
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence);
        static_assert_declaration_delete(p->static_assert_declaration);
        pragma_declaration_delete(p->pragma_declaration);
        free(p);
    }
}


struct specifier_qualifier_list *specifier_qualifier_list(struct parser_ctx * ctx);

struct member_declaration *member_declaration(struct parser_ctx * ctx, struct struct_or_union_specifier * p_struct_or_union_specifier)
{
    struct member_declaration * p_member_declaration;

    p_member_declaration = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_member_declaration = calloc(1, 24);
        if (p_member_declaration == 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current->type == 9068)
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
                ;
                p_member_declaration->p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
                p_member_declaration->specifier_qualifier_list = specifier_qualifier_list(ctx);
                if (p_member_declaration->specifier_qualifier_list == 0)
                {
                    goto __L0; /* throw */
                }
                if (ctx->current == 0)
                {
                    unexpected_end_of_file(ctx);
                    goto __L0; /* throw */
                }
                if (ctx->current->type != 59)
                {
                    p_member_declaration->member_declarator_list_opt = member_declarator_list(ctx, p_struct_or_union_specifier, p_member_declaration->specifier_qualifier_list);
                    if (p_member_declaration->member_declarator_list_opt == 0)
                    {
                        goto __L0; /* throw */
                    }
                }
                if (parser_match_tk(ctx, 59) != 0)
                {
                    goto __L0; /* throw */
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        member_declaration_delete(p_member_declaration);
        p_member_declaration = 0;
    }
    return p_member_declaration;
}


struct member_declarator *find_member_declarator(struct member_declaration_list * list, char * name, int * p_member_index)
{
    struct member_declaration * p_member_declaration;

    if (list->head == 0)
    {
        return 0;
    }
    p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator;

        p_member_declarator = 0;
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
    return 0;
}


static struct member_declarator *find_member_declarator_by_index_core(struct member_declaration_list * list, int member_index, int * p_count);

struct member_declarator *find_member_declarator_by_index(struct member_declaration_list * list, int member_index)
{
    int count;

    count = 0;
    return find_member_declarator_by_index_core(list, member_index, &count);
}



static struct member_declarator *find_member_declarator_by_index_core(struct member_declaration_list * list, int member_index, int * p_count)
{
    struct member_declaration * p_member_declaration;

    if (list->head == 0)
    {
        return 0;
    }
    p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator;

        p_member_declarator = 0;
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
    return 0;
}
static struct object *find_object_declarator_by_index_core(struct object * p_object0, struct member_declaration_list * list, int member_index, int * p_count);
unsigned char object_is_reference(struct object * p_object);
struct object *object_get_referenced(struct object * p_object);

struct object *find_object_declarator_by_index(struct object * p_object, struct member_declaration_list * list, int member_index)
{
    int count;

    count = 0;
    return find_object_declarator_by_index_core(p_object, list, member_index, &count);
}



static struct object *find_object_declarator_by_index_core(struct object * p_object0, struct member_declaration_list * list, int member_index, int * p_count)
{
    struct object * p_object;
    struct object * p_member_object;
    struct member_declaration * p_member_declaration;

    p_object = object_is_reference(p_object0) ? object_get_referenced(p_object0) : p_object0;
    if (list->head == 0)
    {
        return 0;
    }
    if (p_object->members.head == 0)
    {
        return 0;
    }
    p_member_object = p_object->members.head;
    p_member_declaration = list->head;
    while (p_member_declaration)
    {
        struct member_declarator * p_member_declarator;

        p_member_declarator = 0;
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
                if (p_member_object == 0)
                {
                    return 0;
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
    return 0;
}
void print_specifier_qualifier_list(struct osstream * ss, unsigned char * first, struct specifier_qualifier_list * p_specifier_qualifier_list)
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
    if (list->head == 0)
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
            item->next = 0;
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

    p_specifier_qualifier_list = 0;
    if (1) /*try*/
    {
        struct token * p_previous_parser_token;

        if (!first_of_type_specifier_qualifier(ctx))
        {
            compiler_diagnostic(1040, ctx, ctx->current, 0, "type specifier or qualifier expected");
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_specifier_qualifier_list = calloc(1, 48);
        if (p_specifier_qualifier_list == 0)
        {
            goto __L0; /* throw */
        }
        p_specifier_qualifier_list->first_token = ctx->current;
        while (ctx->current != 0 && (first_of_type_specifier_qualifier(ctx)))
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
            if (p_type_specifier_qualifier == 0)
            {
                goto __L0; /* throw */
            }
            if (p_type_specifier_qualifier->type_specifier)
            {
                if (add_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags, p_type_specifier_qualifier->type_specifier->flags) != 0)
                {
                    type_specifier_qualifier_delete(p_type_specifier_qualifier);
                    goto __L0; /* throw */
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
                                p_specifier_qualifier_list->typedef_declarator = find_declarator(ctx, p_type_specifier_qualifier->type_specifier->token->lexeme, 0);
                            }
                        }
                    }
                }
            }
            else
            {
                if (p_type_specifier_qualifier->alignment_specifier)
                {
                    p_specifier_qualifier_list->alignment_specifier_flags |= p_type_specifier_qualifier->alignment_specifier->flags;
                }
                else
                {
                    if (p_type_specifier_qualifier->type_qualifier)
                    {
                        p_specifier_qualifier_list->type_qualifier_flags |= p_type_specifier_qualifier->type_qualifier->flags;
                    }
                }
            }
            ;
            p_specifier_qualifier_list->p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
            specifier_qualifier_list_add(p_specifier_qualifier_list, p_type_specifier_qualifier);
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        final_specifier(ctx, &p_specifier_qualifier_list->type_specifier_flags);
        p_previous_parser_token = previous_parser_token(ctx->current);
        if (p_previous_parser_token == 0)
        {
            goto __L0; /* throw */
        }
        p_specifier_qualifier_list->last_token = p_previous_parser_token;
    }
    else __L0: /*catch*/ 
    {
        specifier_qualifier_list_delete(p_specifier_qualifier_list);
        p_specifier_qualifier_list = 0;
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

    type_specifier_qualifier = 0;
    if (1) /*try*/
    {
        type_specifier_qualifier = calloc(1, 16);
        if (type_specifier_qualifier == 0)
        {
            goto __L0; /* throw */
        }
        if (first_of_type_specifier(ctx))
        {
            type_specifier_qualifier->type_specifier = type_specifier(ctx);
            if (type_specifier_qualifier->type_specifier == 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            if (first_of_type_qualifier(ctx))
            {
                type_specifier_qualifier->type_qualifier = type_qualifier(ctx);
                if (type_specifier_qualifier->type_qualifier == 0)
                {
                    goto __L0; /* throw */
                }
            }
            else
            {
                if (first_of_alignment_specifier(ctx))
                {
                    type_specifier_qualifier->alignment_specifier = alignment_specifier(ctx);
                    if (type_specifier_qualifier->alignment_specifier == 0)
                    {
                        goto __L0; /* throw */
                    }
                }
                else
                {
                    ;
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        type_specifier_qualifier_delete(type_specifier_qualifier);
        type_specifier_qualifier = 0;
    }
    return type_specifier_qualifier;
}


int object_is_equal(int target, struct object * a, struct object * b);

struct enumerator *find_enumerator_by_value(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier, struct object * object)
{
    struct enumerator * p;

    if (p_enum_specifier->enumerator_list.head == 0)
    {
        return 0;
    }
    p = p_enum_specifier->enumerator_list.head;
    while (p)
    {
        if (object_is_equal(ctx->options.target, &p->value, object))
        {
            return p;
        }
        p = p->next;
    }
    return 0;
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
        if (p->has_shared_ownership)
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


unsigned char enum_specifier_has_fixed_underlying_type(struct enum_specifier * p_enum_specifier)
{
    return p_enum_specifier->specifier_qualifier_list != 0;
}


struct type make_with_type_specifier_flags(int f);
unsigned char type_is_integer(struct type * p_type);
void naming_convention_enum_tag(struct parser_ctx * ctx, struct token * token);
struct enumerator_list enumerator_list(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier);

struct enum_specifier *enum_specifier(struct parser_ctx * ctx)
{
    struct enum_specifier * p_enum_specifier;

    p_enum_specifier = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_enum_specifier = calloc(1, 232);
        if (p_enum_specifier == 0)
        {
            goto __L0; /* throw */
        }
        p_enum_specifier->first_token = ctx->current;
        if (parser_match_tk(ctx, 9012) != 0)
        {
            goto __L0; /* throw */
        }
        p_enum_specifier->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 8996)
        {
            snprintf(p_enum_specifier->tag_name, 200, "%s", ctx->current->lexeme);
            p_enum_specifier->tag_token = ctx->current;
            parser_match(ctx);
        }
        else
        {
            snprintf(p_enum_specifier->tag_name, 200, "__tag%d", ctx->anonymous_struct_count++);
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 58)
        {
            if (!ctx->inside_generic_association)
            {
                struct type  enum_underline_type;

                parser_match(ctx);
                p_enum_specifier->specifier_qualifier_list = specifier_qualifier_list(ctx);
                if (p_enum_specifier->specifier_qualifier_list == 0)
                {
                    goto __L0; /* throw */
                }
                enum_underline_type = make_with_type_specifier_flags(p_enum_specifier->specifier_qualifier_list->type_specifier_flags);
                if (!type_is_integer(&enum_underline_type))
                {
                    type_destroy(&enum_underline_type);
                    compiler_diagnostic(1850, ctx, p_enum_specifier->specifier_qualifier_list->first_token, 0, "expected an integer type");
                    goto __L0; /* throw */
                }
                type_destroy(&enum_underline_type);
            }
            else
            {
            }
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 123)
        {
            struct hash_item_set  item;

            if (p_enum_specifier->tag_token)
            {
                naming_convention_enum_tag(ctx, p_enum_specifier->tag_token);
            }
            p_enum_specifier->p_complete_enum_specifier = p_enum_specifier;
            if (parser_match_tk(ctx, 123) != 0)
            {
                goto __L0; /* throw */
            }
            p_enum_specifier->enumerator_list = enumerator_list(ctx, p_enum_specifier);
            if (p_enum_specifier->enumerator_list.head == 0)
            {
                goto __L0; /* throw */
            }
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (ctx->current->type == 44)
            {
                parser_match(ctx);
            }
            if (parser_match_tk(ctx, 125) != 0)
            {
                goto __L0; /* throw */
            }
            _cake_zmem(&item, 32);
            item.p_enum_specifier = enum_specifier_add_ref(p_enum_specifier);
            hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, &item);
            p_enum_specifier->p_complete_enum_specifier = p_enum_specifier;
            hash_item_set_destroy(&item);
        }
        else
        {
            struct enum_specifier * p_existing_enum_specifier;

            p_existing_enum_specifier = 0;
            if (p_enum_specifier->tag_token)
            {
                p_existing_enum_specifier = find_enum_specifier(ctx, p_enum_specifier->tag_token->lexeme);
            }
            if (p_existing_enum_specifier)
            {
                p_enum_specifier->p_complete_enum_specifier = p_existing_enum_specifier;
            }
            else
            {
                struct hash_item_set  item;

                _cake_zmem(&item, 32);
                item.p_enum_specifier = enum_specifier_add_ref(p_enum_specifier);
                hashmap_set(&ctx->scopes.tail->tags, p_enum_specifier->tag_name, &item);
                p_enum_specifier->p_complete_enum_specifier = p_enum_specifier;
                hash_item_set_destroy(&item);
            }
        }
    }
    else __L0: /*catch*/ 
    {
        enum_specifier_delete(p_enum_specifier);
        p_enum_specifier = 0;
    }
    return p_enum_specifier;
}


void enumerator_list_add(struct enumerator_list * list, struct enumerator * p_item)
{
    if (list->head == 0)
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
        item->next = 0;
        enumerator_delete(item);
        item = next;
    }
}


struct object object_make_signed_int(int target, long long value);
int type_specifier_to_object_type(int type_specifier_flags, int target);
struct object object_cast(int target, int e, struct object * a);
struct enumerator *enumerator(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier, struct object * p_enumerator_value);

struct enumerator_list enumerator_list(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier)
{
    struct object  next_enumerator_value;
    struct enumerator_list  enumeratorlist;
    struct enumerator * p_enumerator;

    next_enumerator_value = object_make_signed_int(ctx->options.target, 0);
    if (p_enum_specifier->specifier_qualifier_list)
    {
        int vt;

        vt = type_specifier_to_object_type(p_enum_specifier->specifier_qualifier_list->type_specifier_flags, ctx->options.target);
        next_enumerator_value = object_cast(ctx->options.target, vt, &next_enumerator_value);
    }
    _cake_zmem(&enumeratorlist, 8);
    p_enumerator = 0;
    if (1) /*try*/
    {
        p_enumerator = enumerator(ctx, p_enum_specifier, &next_enumerator_value);
        if (p_enumerator == 0)
        {
            goto __L1; /* throw */
        }
        enumerator_list_add(&enumeratorlist, p_enumerator);
        while (ctx->current != 0 && ctx->current->type == 44)
        {
            parser_match(ctx);
            if (ctx->current && ctx->current->type != 125)
            {
                p_enumerator = enumerator(ctx, p_enum_specifier, &next_enumerator_value);
                if (p_enumerator == 0)
                {
                    goto __L1; /* throw */
                }
                enumerator_list_add(&enumeratorlist, p_enumerator);
            }
        }
    }
    else __L1: /*catch*/ 
    {
        enumerator_list_destroy(&enumeratorlist);
        enumeratorlist.head = 0;
        enumeratorlist.tail = 0;
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
        if (p->has_shared_ownership)
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
void object_increment_value(int target, struct object * a);

struct enumerator *enumerator(struct parser_ctx * ctx, struct enum_specifier * p_enum_specifier, struct object * p_next_enumerator_value)
{
    struct enumerator * p_enumerator;

    p_enumerator = 0;
    if (1) /*try*/
    {
        struct token * name;
        struct hash_item_set  item;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_enumerator = calloc(1, 144);
        if (p_enumerator == 0)
        {
            goto __L0; /* throw */
        }
        p_enumerator->enum_specifier = p_enum_specifier;
        name = ctx->current;
        naming_convention_enumerator(ctx, name);
        if (parser_match_tk(ctx, 8996) != 0)
        {
            goto __L0; /* throw */
        }
        p_enumerator->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        p_enumerator->token = name;
        _cake_zmem(&item, 32);
        item.p_enumerator = enumerator_add_ref(p_enumerator);
        hashmap_set(&ctx->scopes.tail->variables, p_enumerator->token->lexeme, &item);
        hash_item_set_destroy(&item);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 61)
        {
            parser_match(ctx);
            ;
            p_enumerator->constant_expression_opt = constant_expression(ctx, 1, 2);
            if (p_enumerator->constant_expression_opt == 0)
            {
                goto __L0; /* throw */
            }
            if (enum_specifier_has_fixed_underlying_type(p_enum_specifier))
            {
            }
            else
            {
            }
            p_enumerator->value = p_enumerator->constant_expression_opt->object;
            *p_next_enumerator_value = *object_get_referenced(&p_enumerator->value);
            object_increment_value(ctx->options.target, p_next_enumerator_value);
        }
        else
        {
            p_enumerator->value = *p_next_enumerator_value;
            object_increment_value(ctx->options.target, p_next_enumerator_value);
        }
    }
    else __L0: /*catch*/ 
    {
        enumerator_delete(p_enumerator);
        p_enumerator = 0;
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


unsigned char object_has_constant_value(struct object * a);
signed long long object_to_signed_long_long(struct object * a);

struct alignment_specifier *alignment_specifier(struct parser_ctx * ctx)
{
    struct alignment_specifier * alignment_specifier;

    alignment_specifier = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        alignment_specifier = calloc(1, 16);
        if (alignment_specifier == 0)
        {
            goto __L0; /* throw */
        }
        alignment_specifier->token = ctx->current;
        if (parser_match_tk(ctx, 9043) != 0)
        {
            goto __L0; /* throw */
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto __L0; /* throw */
        }
        if (first_of_type_name(ctx))
        {
            alignment_specifier->type_name = type_name(ctx);
            if (alignment_specifier->type_name == 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            alignment_specifier->constant_expression = constant_expression(ctx, 1, 2);
            if (alignment_specifier->constant_expression == 0)
            {
                goto __L0; /* throw */
            }
            if (object_has_constant_value(&alignment_specifier->constant_expression->object))
            {
                long long a;

                a = object_to_signed_long_long(&alignment_specifier->constant_expression->object);
                if (a == 8)
                {
                    alignment_specifier->flags |= 1;
                }
                else
                {
                    if (a == 16)
                    {
                        alignment_specifier->flags |= 2;
                    }
                    else
                    {
                        if (a == 32)
                        {
                            alignment_specifier->flags |= 4;
                        }
                        else
                        {
                            if (a == 64)
                            {
                                alignment_specifier->flags |= 8;
                            }
                            else
                            {
                                if (a == 128)
                                {
                                    alignment_specifier->flags |= 16;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
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

    p_atomic_type_specifier = 0;
    if (1) /*try*/
    {
        struct type_name * ptemp;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_atomic_type_specifier = calloc(1, 8);
        if (p_atomic_type_specifier == 0)
        {
            goto __L0; /* throw */
        }
        p_atomic_type_specifier->token = ctx->current;
        if (parser_match_tk(ctx, 9045) != 0)
        {
            goto __L0; /* throw */
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto __L0; /* throw */
        }
        ptemp = type_name(ctx);
        if (ptemp == 0)
        {
            goto __L0; /* throw */
        }
        p_atomic_type_specifier->type_name = ptemp;
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        atomic_type_specifier_delete(p_atomic_type_specifier);
        p_atomic_type_specifier = 0;
    }
    return p_atomic_type_specifier;
}


struct type_qualifier *type_qualifier(struct parser_ctx * ctx)
{
    struct type_qualifier * p_type_qualifier;

    if (ctx->current == 0)
    {
        unexpected_end_of_file(ctx);
        return 0;
    }
    p_type_qualifier = calloc(1, 12);
    if (p_type_qualifier == 0)
    {
        return 0;
    }
    /*switch*/
    {
        int __v0 = ctx->current->type;
        if (__v0 == 9004) goto __L3; /*case 9004*/
        if (__v0 == 9026) goto __L4; /*case 9026*/
        if (__v0 == 9041) goto __L5; /*case 9041*/
        if (__v0 == 9045) goto __L6; /*case 9045*/
        if (__v0 == 9053) goto __L7; /*case 9053*/
        if (__v0 == 9054) goto __L8; /*case 9054*/
        goto __L9; /* default */

        {
            __L3: /*case 9004*/ 
            p_type_qualifier->flags = 1;
            goto __L2; /* break */

            __L4: /*case 9026*/ 
            p_type_qualifier->flags = 2;
            goto __L2; /* break */

            __L5: /*case 9041*/ 
            p_type_qualifier->flags = 4;
            goto __L2; /* break */

            __L6: /*case 9045*/ 
            p_type_qualifier->flags = 8;
            goto __L2; /* break */

            __L7: /*case 9053*/ 
            p_type_qualifier->flags = 512;
            goto __L2; /* break */

            __L8: /*case 9054*/ 
            p_type_qualifier->flags = 1024;
            goto __L2; /* break */

            __L9: /* default */ 
            goto __L2; /* break */

        }
        __L2:;
    }
    if (ctx->options.ownership_enabled)
    {
        /*switch*/
        {
            int __v1 = ctx->current->type;
            if (__v1 == 9078) goto __L12; /*case 9078*/
            if (__v1 == 9079) goto __L13; /*case 9079*/
            if (__v1 == 9077) goto __L14; /*case 9077*/
            if (__v1 == 9080) goto __L15; /*case 9080*/
            goto __L16; /* default */

            {
                __L12: /*case 9078*/ 
                p_type_qualifier->flags = 256;
                goto __L11; /* break */

                __L13: /*case 9079*/ 
                p_type_qualifier->flags = 128;
                goto __L11; /* break */

                __L14: /*case 9077*/ 
                p_type_qualifier->flags = 16;
                goto __L11; /* break */

                __L15: /*case 9080*/ 
                p_type_qualifier->flags = 32;
                goto __L11; /* break */

                __L16: /* default */ 
                goto __L11; /* break */

            }
            __L11:;
        }
    }
    if (ctx->options.null_checks_enabled)
    {
        /*switch*/
        {
            int __v2 = ctx->current->type;
            if (__v2 == 9081) goto __L19; /*case 9081*/
            goto __L20; /* default */

            {
                __L19: /*case 9081*/ 
                p_type_qualifier->flags = 64;
                goto __L18; /* break */

                __L20: /* default */ 
                goto __L18; /* break */

            }
            __L18:;
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
    return 0;
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

    p_function_specifier = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_function_specifier = calloc(1, 8);
        if (p_function_specifier == 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current->type == 9067)
        {
            compiler_diagnostic(11, ctx, ctx->current, 0, "_Noreturn is deprecated use attributes");
        }
        if (ctx->current->type == 9018)
        {
            p_function_specifier->flags |= 1;
        }
        p_function_specifier->token = ctx->current;
        parser_match(ctx);
    }
    else __L0: /*catch*/ 
    {
        function_specifier_delete(p_function_specifier);
        p_function_specifier = 0;
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
        if (p->has_shared_ownership)
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
struct direct_declarator *direct_declarator(struct parser_ctx * ctx, struct specifier_qualifier_list * specifier_qualifier_list, struct declaration_specifiers * declaration_specifiers, unsigned char  abstract_acceptable, struct token ** pptoken_name);

struct declarator *declarator(struct parser_ctx * ctx, struct specifier_qualifier_list * p_specifier_qualifier_list_opt, struct declaration_specifiers * p_declaration_specifiers_opt, unsigned char  abstract_acceptable, struct token ** pp_token_name_opt)
{
    struct declarator * p_declarator;

    p_declarator = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_declarator = calloc(1, 256);
        if (p_declarator == 0)
        {
            goto __L0; /* throw */
        }
        p_declarator->first_token_opt = ctx->current;
        p_declarator->pointer = pointer_opt(ctx);
        p_declarator->direct_declarator = direct_declarator(ctx, p_specifier_qualifier_list_opt, p_declaration_specifiers_opt, abstract_acceptable, pp_token_name_opt);
        if (p_declarator->direct_declarator == 0)
        {
            goto __L0; /* throw */
        }
        if (pp_token_name_opt && *pp_token_name_opt)
        {
            free((void *)p_declarator->object.member_designator);
            p_declarator->object.member_designator = strdup((*pp_token_name_opt)->lexeme);
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current != p_declarator->first_token_opt)
        {
            p_declarator->last_token_opt = previous_parser_token(ctx->current);
        }
        else
        {
            p_declarator->last_token_opt = p_declarator->first_token_opt;
            p_declarator->first_token_opt = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        declarator_delete(p_declarator);
        p_declarator = 0;
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
    return 0;
}


unsigned char declarator_is_function(struct declarator * p_declarator)
{
    return (p_declarator->direct_declarator && p_declarator->direct_declarator->function_declarator != 0);
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
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence);
        array_declarator_delete(p->array_declarator);
        function_declarator_delete(p->function_declarator);
        free(p);
    }
}


struct array_declarator *array_declarator(struct direct_declarator * p_direct_declarator, struct parser_ctx * ctx, int eval_mode);
struct function_declarator *function_declarator(struct direct_declarator * p_direct_declarator, struct parser_ctx * ctx);

struct direct_declarator *direct_declarator(struct parser_ctx * ctx, struct specifier_qualifier_list * p_specifier_qualifier_list, struct declaration_specifiers * p_declaration_specifiers, unsigned char  abstract_acceptable, struct token ** pp_token_name_opt)
{
    struct direct_declarator * p_direct_declarator;

    p_direct_declarator = 0;
    if (1) /*try*/
    {
        struct token * p_token_ahead;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_direct_declarator = calloc(1, 24);
        if (p_direct_declarator == 0)
        {
            goto __L0; /* throw */
        }
        p_token_ahead = parser_look_ahead(ctx);
        if (p_token_ahead == 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current->type == 9056 || ctx->current->type == 9057 || ctx->current->type == 9055)
        {
            p_direct_declarator->p_calling_convention = ctx->current;
            parser_match(ctx);
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 8996)
        {
            p_direct_declarator->name_opt = ctx->current;
            if (pp_token_name_opt != 0)
            {
                *pp_token_name_opt = ctx->current;
            }
            parser_match(ctx);
            p_direct_declarator->p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
        }
        else
        {
            if (ctx->current->type == 40)
            {
                struct token * ahead;

                ahead = parser_look_ahead(ctx);
                if (ahead == 0)
                {
                    goto __L0; /* throw */
                }
                if (!first_of_type_specifier_token(ctx, p_token_ahead) && !first_of_type_qualifier_token(p_token_ahead) && ahead->type != 41 && ahead->type != 3026478)
                {
                    struct declarator * p_declarator_temp;

                    parser_match(ctx);
                    p_declarator_temp = declarator(ctx, p_specifier_qualifier_list, p_declaration_specifiers, abstract_acceptable, pp_token_name_opt);
                    if (p_declarator_temp == 0)
                    {
                        goto __L0; /* throw */
                    }
                    p_direct_declarator->declarator = p_declarator_temp;
                    parser_match(ctx);
                }
            }
        }
        while (ctx->current != 0 && (ctx->current->type == 91 || ctx->current->type == 40))
        {
            struct direct_declarator * p_direct_declarator2;

            p_direct_declarator2 = calloc(1, 24);
            if (p_direct_declarator2 == 0)
            {
                goto __L0; /* throw */
            }
            if (ctx->current->type == 91)
            {
                p_direct_declarator2->array_declarator = array_declarator(p_direct_declarator, ctx, 2);
                p_direct_declarator = 0;
                if (p_direct_declarator2->array_declarator == 0)
                {
                    direct_declarator_delete(p_direct_declarator2);
                    goto __L0; /* throw */
                }
            }
            else
            {
                p_direct_declarator2->function_declarator = function_declarator(p_direct_declarator, ctx);
                p_direct_declarator = 0;
                if (p_direct_declarator2->function_declarator == 0)
                {
                    direct_declarator_delete(p_direct_declarator2);
                    goto __L0; /* throw */
                }
            }
            p_direct_declarator = p_direct_declarator2;
            ;
            p_direct_declarator->p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
        }
    }
    else __L0: /*catch*/ 
    {
        direct_declarator_delete(p_direct_declarator);
        p_direct_declarator = 0;
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


unsigned int array_declarator_is_vla(struct array_declarator * p_array_declarator)
{
    if (p_array_declarator->assignment_expression)
    {
        return !object_has_constant_value(&p_array_declarator->assignment_expression->object);
    }
    return 0;
}


unsigned long long object_to_unsigned_long_long(struct object * a);

unsigned int array_declarator_get_size(struct array_declarator * p_array_declarator)
{
    if (p_array_declarator->assignment_expression)
    {
        if (object_has_constant_value(&p_array_declarator->assignment_expression->object))
        {
            return (unsigned int)object_to_unsigned_long_long(&p_array_declarator->assignment_expression->object);
        }
    }
    return 0;
}


struct type_qualifier_list *type_qualifier_list(struct parser_ctx * ctx);
struct expression *assignment_expression(struct parser_ctx * ctx, int eval_mode);

struct array_declarator *array_declarator(struct direct_declarator * p_direct_declarator, struct parser_ctx * ctx, int eval_mode)
{
    struct array_declarator * p_array_declarator;

    p_array_declarator = 0;
    if (1) /*try*/
    {
        unsigned char  has_static;

        p_array_declarator = calloc(1, 24);
        if (p_array_declarator == 0)
        {
            goto __L0; /* throw */
        }
        p_array_declarator->direct_declarator = p_direct_declarator;
        p_direct_declarator = 0;
        if (parser_match_tk(ctx, 91) != 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
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
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
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
            p_array_declarator->assignment_expression = assignment_expression(ctx, 2);
            if (p_array_declarator->assignment_expression == 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (ctx->current->type == 42)
            {
                parser_match(ctx);
            }
            else
            {
                if (ctx->current->type != 93)
                {
                    p_array_declarator->assignment_expression = assignment_expression(ctx, 2);
                    if (p_array_declarator->assignment_expression == 0)
                    {
                        goto __L0; /* throw */
                    }
                }
                else
                {
                }
            }
        }
        if (parser_match_tk(ctx, 93) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        array_declarator_delete(p_array_declarator);
        p_array_declarator = 0;
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
    p_function_declarator = calloc(1, 44);
    if (1) /*try*/
    {
        if (p_function_declarator == 0)
        {
            goto __L0; /* throw */
        }
        p_function_declarator->direct_declarator = p_direct_declarator;
        p_direct_declarator = 0;
        p_function_declarator->parameters_scope.scope_level = ctx->scopes.tail->scope_level + 1;
        p_function_declarator->parameters_scope.variables.capacity = 5;
        p_function_declarator->parameters_scope.tags.capacity = 1;
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type != 41)
        {
            scope_list_push(&ctx->scopes, &p_function_declarator->parameters_scope);
            p_function_declarator->parameter_type_list_opt = parameter_type_list(ctx);
            scope_list_pop(&ctx->scopes);
            if (p_function_declarator->parameter_type_list_opt == 0)
            {
                goto __L0; /* throw */
            }
        }
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        function_declarator_delete(p_function_declarator);
        p_function_declarator = 0;
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

    p = 0;
    p_pointer = 0;
    if (1) /*try*/
    {
        struct token * calling_convention;
        struct token * ahead;

        calling_convention = 0;
        ahead = parser_look_ahead(ctx);
        if (ahead != 0 && ahead->type == 42)
        {
            if (ctx->current->type == 9056 || ctx->current->type == 9057 || ctx->current->type == 9055)
            {
                calling_convention = ctx->current;
                parser_match(ctx);
            }
        }
        while (ctx->current != 0 && ctx->current->type == 42)
        {
            p_pointer = calloc(1, 16);
            if (p_pointer == 0)
            {
                goto __L0; /* throw */
            }
            p_pointer->calling_convention = calling_convention;
            calling_convention = 0;
            p = p_pointer;
            parser_match(ctx);
            p_pointer->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
            if (first_of_type_qualifier(ctx))
            {
                ;
                if (ctx->current->type == 9080)
                {
                    compiler_diagnostic(640, ctx, ctx->current, 0, "invalid qualifier for pointer");
                }
                else
                {
                    p_pointer->type_qualifier_list_opt = type_qualifier_list(ctx);
                }
            }
            while (ctx->current != 0 && ctx->current->type == 42)
            {
                p_pointer->pointer = pointer_opt(ctx);
                if (p_pointer->pointer == 0)
                {
                    goto __L0; /* throw */
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
    return p;
}


void type_qualifier_list_add(struct type_qualifier_list * list, struct type_qualifier * p_item)
{
    if (list->head == 0)
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
            item->next = 0;
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

    p_type_qualifier_list = 0;
    p_type_qualifier = 0;
    if (1) /*try*/
    {
        p_type_qualifier_list = calloc(1, 12);
        if (p_type_qualifier_list == 0)
        {
            goto __L0; /* throw */
        }
        p_type_qualifier = type_qualifier(ctx);
        if (p_type_qualifier == 0)
        {
            goto __L0; /* throw */
        }
        p_type_qualifier_list->flags |= p_type_qualifier->flags;
        type_qualifier_list_add(p_type_qualifier_list, p_type_qualifier);
        p_type_qualifier = 0;
        while (ctx->current != 0 && first_of_type_qualifier(ctx))
        {
            p_type_qualifier = type_qualifier(ctx);
            if (p_type_qualifier == 0)
            {
                goto __L0; /* throw */
            }
            p_type_qualifier_list->flags |= p_type_qualifier->flags;
            type_qualifier_list_add(p_type_qualifier_list, p_type_qualifier);
            p_type_qualifier = 0;
        }
    }
    else __L0: /*catch*/ 
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
unsigned char type_is_void(struct type * p_type);

struct parameter_type_list *parameter_type_list(struct parser_ctx * ctx)
{
    struct parameter_type_list * p_parameter_type_list;

    if (ctx->current == 0)
    {
        return 0;
    }
    p_parameter_type_list = calloc(1, 8);
    if (1) /*try*/
    {
        if (p_parameter_type_list == 0)
        {
            goto __L1; /* throw */
        }
        if (ctx->current->type == 3026478)
        {
            parser_match(ctx);
            p_parameter_type_list->is_var_args = 1;
            return p_parameter_type_list;
        }
        p_parameter_type_list->parameter_list = parameter_list(ctx);
        if (p_parameter_type_list->parameter_list == 0)
        {
            goto __L1; /* throw */
        }
        if (p_parameter_type_list->parameter_list->head == p_parameter_type_list->parameter_list->tail)
        {
            if (type_is_void(&p_parameter_type_list->parameter_list->head->declarator->type))
            {
                p_parameter_type_list->is_void = 1;
            }
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L1; /* throw */
        }
        if (ctx->current->type == 3026478)
        {
            parser_match(ctx);
            p_parameter_type_list->is_var_args = 1;
        }
    }
    else __L1: /*catch*/ 
    {
        parameter_type_list_delete(p_parameter_type_list);
        p_parameter_type_list = 0;
    }
    return p_parameter_type_list;
}


void parameter_list_add(struct parameter_list * list, struct parameter_declaration * p_item)
{
    if (list->head == 0)
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
            item->next = 0;
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

    p_parameter_list = 0;
    p_parameter_declaration = 0;
    if (1) /*try*/
    {
        p_parameter_list = calloc(1, 8);
        if (p_parameter_list == 0)
        {
            goto __L0; /* throw */
        }
        p_parameter_declaration = parameter_declaration(ctx);
        if (p_parameter_declaration == 0)
        {
            goto __L0; /* throw */
        }
        parameter_list_add(p_parameter_list, p_parameter_declaration);
        p_parameter_declaration = 0;
        while (ctx->current != 0 && ctx->current->type == 44)
        {
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (ctx->current->type == 3026478)
            {
                break;
            }
            p_parameter_declaration = parameter_declaration(ctx);
            if (p_parameter_declaration == 0)
            {
                goto __L0; /* throw */
            }
            parameter_list_add(p_parameter_list, p_parameter_declaration);
            p_parameter_declaration = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        parameter_list_delete(p_parameter_list);
        p_parameter_list = 0;
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

    p_parameter_declaration = calloc(1, 16);
    if (1) /*try*/
    {
        struct declaration_specifiers * p_declaration_specifiers;
        struct token * p_token_name;

        if (p_parameter_declaration == 0)
        {
            goto __L0; /* throw */
        }
        p_parameter_declaration->attribute_specifier_sequence_opt = attribute_specifier_sequence_opt(ctx);
        p_declaration_specifiers = declaration_specifiers(ctx, 6144);
        if (p_declaration_specifiers == 0)
        {
            goto __L0; /* throw */
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
        p_token_name = 0;
        p_parameter_declaration->declarator = declarator(ctx, 0, p_parameter_declaration->declaration_specifiers, 1, &p_token_name);
        if (p_parameter_declaration->declarator == 0)
        {
            goto __L0; /* throw */
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

            er = make_object(&p_parameter_declaration->declarator->type, &p_parameter_declaration->declarator->object, ctx->options.target);
            if (er != 0)
            {
                goto __L0; /* throw */
            }
        }
        if (p_parameter_declaration->declarator->name_opt)
        {
            free((void *)p_parameter_declaration->declarator->object.member_designator);
            p_parameter_declaration->declarator->object.member_designator = strdup(p_parameter_declaration->declarator->name_opt->lexeme);
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
    else __L0: /*catch*/ 
    {
        parameter_declaration_delete(p_parameter_declaration);
        p_parameter_declaration = 0;
    }
    return p_parameter_declaration;
}


struct specifier_qualifier_list *copy(struct declaration_specifiers * p_declaration_specifiers)
{
    struct specifier_qualifier_list * p_specifier_qualifier_list;

    p_specifier_qualifier_list = calloc(1, 48);
    if (1) /*try*/
    {
        struct declaration_specifier * p_declaration_specifier;

        if (p_specifier_qualifier_list == 0)
        {
            goto __L0; /* throw */
        }
        p_specifier_qualifier_list->type_qualifier_flags = p_declaration_specifiers->type_qualifier_flags;
        p_specifier_qualifier_list->type_specifier_flags = p_declaration_specifiers->type_specifier_flags;
        p_declaration_specifier = p_declaration_specifiers->head;
        while (p_declaration_specifier)
        {
            if (p_declaration_specifier->type_specifier_qualifier)
            {
                struct type_specifier_qualifier * p_specifier_qualifier;

                p_specifier_qualifier = calloc(1, 16);
                if (p_specifier_qualifier == 0)
                {
                    goto __L0; /* throw */
                }
                if (p_declaration_specifier->type_specifier_qualifier->type_qualifier)
                {
                    struct type_qualifier * p_type_qualifier;

                    p_type_qualifier = calloc(1, 12);
                    if (p_type_qualifier == 0)
                    {
                        type_specifier_qualifier_delete(p_specifier_qualifier);
                        goto __L0; /* throw */
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

                        p_type_specifier = calloc(1, 28);
                        if (p_type_specifier == 0)
                        {
                            type_specifier_qualifier_delete(p_specifier_qualifier);
                            goto __L0; /* throw */
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
    else __L0: /*catch*/ 
    {
        specifier_qualifier_list_delete(p_specifier_qualifier_list);
        p_specifier_qualifier_list = 0;
    }
    return p_specifier_qualifier_list;
}


void print_declarator(struct osstream * ss, struct declarator * declarator, unsigned char  is_abstract);

void print_direct_declarator(struct osstream * ss, struct direct_declarator * p_direct_declarator, unsigned char  is_abstract)
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
        p_parameter_declaration = p_direct_declarator->function_declarator->parameter_type_list_opt ? p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head : 0;
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


struct declarator *declarator_get_innert_function_declarator(struct declarator * p)
{
    struct declarator * inner;

    inner = p;
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
    return inner;
}


struct declarator *declarator_get_function_definition(struct declarator * declarator)
{
    struct declarator * p_function_defined;

    p_function_defined = 0;
    if (declarator->function_body)
    {
        p_function_defined = declarator;
    }
    if (!p_function_defined && declarator->p_complete_declarator && declarator->p_complete_declarator->function_body)
    {
        p_function_defined = declarator->p_complete_declarator;
    }
    if (!p_function_defined && declarator->p_complete_declarator && declarator->p_complete_declarator->p_complete_declarator && declarator->p_complete_declarator->p_complete_declarator->function_body)
    {
        p_function_defined = declarator->p_complete_declarator->p_complete_declarator;
    }
    ;
    return p_function_defined;
}


int declarator_get_type_specifier_flags(struct declarator * p)
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


void print_declarator(struct osstream * ss, struct declarator * p_declarator, unsigned char  is_abstract)
{
    unsigned char  first;

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
    unsigned char  first;

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

    if (ctx->current == 0)
    {
        return 0;
    }
    p_type_name = calloc(1, 84);
    if (1) /*try*/
    {
        struct specifier_qualifier_list * p_specifier_qualifier_list;
        struct declarator * p_declarator;

        if (p_type_name == 0)
        {
            goto __L1; /* throw */
        }
        p_type_name->first_token = ctx->current;
        p_specifier_qualifier_list = specifier_qualifier_list(ctx);
        if (p_specifier_qualifier_list == 0)
        {
            goto __L1; /* throw */
        }
        p_type_name->specifier_qualifier_list = p_specifier_qualifier_list;
        p_declarator = declarator(ctx, p_type_name->specifier_qualifier_list, 0, 1, 0);
        if (p_declarator == 0)
        {
            goto __L1; /* throw */
        }
        p_type_name->abstract_declarator = p_declarator;
        p_type_name->abstract_declarator->specifier_qualifier_list = p_type_name->specifier_qualifier_list;
        p_type_name->abstract_declarator->type = make_type_using_declarator(ctx, p_type_name->abstract_declarator);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L1; /* throw */
        }
        if (ctx->current->prev == 0)
        {
            goto __L1; /* throw */
        }
        p_type_name->last_token = ctx->current->prev;
        p_type_name->type = type_dup(&p_type_name->abstract_declarator->type);
    }
    else __L1: /*catch*/ 
    {
        type_name_delete(p_type_name);
        p_type_name = 0;
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


struct initializer_list *initializer_list(struct parser_ctx * ctx, int eval_mode);

struct braced_initializer *braced_initializer(struct parser_ctx * ctx)
{
    struct braced_initializer * p_bracket_initializer_list;

    p_bracket_initializer_list = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_bracket_initializer_list = calloc(1, 12);
        if (p_bracket_initializer_list == 0)
        {
            goto __L0; /* throw */
        }
        p_bracket_initializer_list->first_token = ctx->current;
        if (parser_match_tk(ctx, 123) != 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type != 125)
        {
            p_bracket_initializer_list->initializer_list = initializer_list(ctx, 2);
        }
        if (parser_match_tk(ctx, 125) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        braced_initializer_delete(p_bracket_initializer_list);
        p_bracket_initializer_list = 0;
    }
    return p_bracket_initializer_list;
}


void initializer_destroy(struct initializer * p)
{
    expression_delete(p->assignment_expression);
    braced_initializer_delete(p->braced_initializer);
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


struct initializer *initializer(struct parser_ctx * ctx, int eval_mode)
{
    struct initializer * p_initializer;

    p_initializer = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_initializer = calloc(1, 12);
        if (p_initializer == 0)
        {
            goto __L0; /* throw */
        }
        p_initializer->first_token = ctx->current;
        if (ctx->current->type == 123)
        {
            p_initializer->braced_initializer = braced_initializer(ctx);
            if (p_initializer->braced_initializer == 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            p_initializer->assignment_expression = assignment_expression(ctx, eval_mode);
            if (p_initializer->assignment_expression == 0)
            {
                goto __L0; /* throw */
            }
        }
    }
    else __L0: /*catch*/ 
    {
        initializer_delete(p_initializer);
        p_initializer = 0;
    }
    return p_initializer;
}


void defer_list_add(struct defer_list * list, struct defer_list_item * p_item)
{
    if (list->head == 0)
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
        item->next = 0;
        free(item);
        item = next;
    }
    free(p);
}


void initializer_list_add(struct initializer_list * list, struct initializer_list_item * p_item)
{
    if (list->head == 0)
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
            item->next = 0;
            initializer_list_item_delete(item);
            item = next;
        }
        free(p);
    }
}


struct designation *designation(struct parser_ctx * ctx);

struct initializer_list *initializer_list(struct parser_ctx * ctx, int eval_mode)
{
    struct initializer_list * p_initializer_list;

    p_initializer_list = 0;
    if (1) /*try*/
    {
        struct initializer_list_item * p_initializer_list_item;
        struct designation * p_designation;
        struct initializer * p_initializer;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            return 0;
        }
        p_initializer_list = calloc(1, 16);
        if (p_initializer_list == 0)
        {
            goto __L0; /* throw */
        }
        p_initializer_list->first_token = ctx->current;
        p_initializer_list_item = calloc(1, 12);
        if (p_initializer_list_item == 0)
        {
            goto __L0; /* throw */
        }
        p_designation = 0;
        if (first_of_designator(ctx))
        {
            p_designation = designation(ctx);
            if (p_designation == 0)
            {
                goto __L0; /* throw */
            }
            p_initializer_list_item->designation = p_designation;
        }
        p_initializer = initializer(ctx, eval_mode);
        if (p_initializer == 0)
        {
            designation_delete(p_designation);
            goto __L0; /* throw */
        }
        p_initializer_list_item->initializer = p_initializer;
        initializer_list_add(p_initializer_list, p_initializer_list_item);
        p_initializer_list_item = 0;
        while (ctx->current != 0 && ctx->current->type == 44)
        {
            struct designation * p_designation2;
            struct initializer * p_initializer2;

            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (ctx->current->type == 125)
            {
                break;
            }
            p_initializer_list_item = calloc(1, 12);
            if (p_initializer_list_item == 0)
            {
                goto __L0; /* throw */
            }
            p_designation2 = 0;
            if (first_of_designator(ctx))
            {
                p_designation2 = designation(ctx);
                if (p_designation2 == 0)
                {
                    goto __L0; /* throw */
                }
            }
            p_initializer_list_item->designation = p_designation2;
            p_initializer2 = initializer(ctx, eval_mode);
            if (p_initializer2 == 0)
            {
                designation_delete(p_designation2);
                goto __L0; /* throw */
            }
            p_initializer_list_item->initializer = p_initializer2;
            initializer_list_add(p_initializer_list, p_initializer_list_item);
            p_initializer_list_item = 0;
            p_initializer_list->size++;
        }
    }
    else __L0: /*catch*/ 
    {
        initializer_list_delete(p_initializer_list);
        p_initializer_list = 0;
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

    p_designation = 0;
    if (1) /*try*/
    {
        struct designator_list * p_designator_list;

        p_designation = calloc(1, 8);
        if (p_designation == 0)
        {
            goto __L0; /* throw */
        }
        p_designator_list = designator_list(ctx);
        if (p_designator_list == 0)
        {
            goto __L0; /* throw */
        }
        p_designation->designator_list = p_designator_list;
        if (parser_match_tk(ctx, 61) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        designation_delete(p_designation);
        p_designation = 0;
    }
    return p_designation;
}


void designator_list_add(struct designator_list * list, struct designator * p_item)
{
    if (list->head == 0)
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
            item->next = 0;
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

    p_designator_list = 0;
    p_designator = 0;
    if (1) /*try*/
    {
        p_designator_list = calloc(1, 8);
        if (p_designator_list == 0)
        {
            goto __L0; /* throw */
        }
        p_designator = designator(ctx);
        if (p_designator == 0)
        {
            goto __L0; /* throw */
        }
        designator_list_add(p_designator_list, p_designator);
        p_designator = 0;
        while (ctx->current != 0 && first_of_designator(ctx))
        {
            p_designator = designator(ctx);
            if (p_designator == 0)
            {
                goto __L0; /* throw */
            }
            designator_list_add(p_designator_list, p_designator);
            p_designator = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        designator_list_delete(p_designator_list);
        p_designator_list = 0;
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

    p_designator = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_designator = calloc(1, 12);
        if (p_designator == 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current->type == 91)
        {
            if (parser_match_tk(ctx, 91) != 0)
            {
                goto __L0; /* throw */
            }
            p_designator->constant_expression_opt = constant_expression(ctx, 1, 2);
            if (parser_match_tk(ctx, 93) != 0)
            {
                goto __L0; /* throw */
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
                    goto __L0; /* throw */
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        designator_delete(p_designator);
        p_designator = 0;
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


static struct token *pragma_match(struct token * p_current);
unsigned long long get_warning_bit_mask(char * wname);
int parse_target(char * targetstr, int * target);

void execute_pragma(struct parser_ctx * ctx, struct pragma_declaration * p_pragma, unsigned char  on_flow_analysis)
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
            if (ctx->options.diagnostic_stack.top_index < 10)
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
                    unsigned char  is_error;
                    unsigned char  is_warning;
                    unsigned char  is_note;

                    is_error = strcmp(p_pragma_token->lexeme, "error") == 0;
                    is_warning = strcmp(p_pragma_token->lexeme, "warning") == 0;
                    is_note = strcmp(p_pragma_token->lexeme, "note") == 0;
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
                    compiler_diagnostic(650, ctx, p_pragma_token, 0, "unknown pragma");
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

                w = 42949672960ULL;
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

                w = 42949672960ULL;
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

                    w = 1073741824ULL;
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

                    w = 1073741824ULL;
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

                            w = 44023414784ULL;
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

                            w = 44023414784ULL;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].errors &= ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].notes &= ~w;
                            ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index].warnings &= ~w;
                            ctx->options.null_checks_enabled = 0;
                            ctx->options.ownership_enabled = 0;
                            ctx->options.flow_analysis = 0;
                        }
                    }
                    else
                    {
                        if (p_pragma_token && strcmp(p_pragma_token->lexeme, "target") == 0)
                        {
                            p_pragma_token = pragma_match(p_pragma_token);
                            if (parse_target(p_pragma_token->lexeme, &ctx->options.target) != 0)
                            {
                                compiler_diagnostic(650, ctx, ctx->current, 0, "unknown target");
                            }
                        }
                    }
                }
            }
        }
    }
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
struct pragma_declaration *pragma_declaration(struct parser_ctx * ctx)
{
    struct pragma_declaration * p_pragma_declaration;

    p_pragma_declaration = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0 || ctx->current->type != 128)
        {
            ;
            goto __L0; /* throw */
        }
        p_pragma_declaration = calloc(1, 8);
        if (p_pragma_declaration == 0)
        {
            goto __L0; /* throw */
        }
        p_pragma_declaration->first_token = ctx->current;
        parser_match(ctx);
        while (ctx->current && ctx->current->type != 129)
        {
            ctx->current = ctx->current->next;
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_pragma_declaration->last_token = ctx->current;
        parser_match(ctx);
    }
    else __L0: /*catch*/ 
    {
    }
    if (p_pragma_declaration)
    {
        execute_pragma(ctx, p_pragma_declaration, 0);
    }
    return p_pragma_declaration;
}


unsigned char object_is_true(struct object * a);

struct static_assert_declaration *static_assert_declaration(struct parser_ctx * ctx)
{
    struct static_assert_declaration * p_static_assert_declaration;

    p_static_assert_declaration = 0;
    if (1) /*try*/
    {
        struct token * position;
        unsigned char  show_error_if_not_constant;
        struct expression * p_constant_expression;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_static_assert_declaration = calloc(1, 16);
        if (p_static_assert_declaration == 0)
        {
            goto __L0; /* throw */
        }
        p_static_assert_declaration->first_token = ctx->current;
        position = ctx->current;
        parser_match(ctx);
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto __L0; /* throw */
        }
        show_error_if_not_constant = 0;
        if (p_static_assert_declaration->first_token->type == 9068)
        {
            show_error_if_not_constant = 1;
        }
        if (ctx->options.flow_analysis)
        {
            show_error_if_not_constant = 0;
        }
        p_constant_expression = constant_expression(ctx, show_error_if_not_constant, 2);
        if (p_constant_expression == 0)
        {
            goto __L0; /* throw */
        }
        p_static_assert_declaration->constant_expression = p_constant_expression;
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 44)
        {
            parser_match(ctx);
            p_static_assert_declaration->string_literal_opt = ctx->current;
            if (parser_match_tk(ctx, 130) != 0)
            {
                goto __L0; /* throw */
            }
        }
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_static_assert_declaration->last_token = ctx->current;
        if (parser_match_tk(ctx, 59) != 0)
        {
            goto __L0; /* throw */
        }
        if (position->type == 9068)
        {
            if (object_has_constant_value(&p_static_assert_declaration->constant_expression->object) && !object_is_true(&p_static_assert_declaration->constant_expression->object))
            {
                if (p_static_assert_declaration->string_literal_opt)
                {
                    compiler_diagnostic(1060, ctx, position, 0, "static_assert failed %s\n", p_static_assert_declaration->string_literal_opt->lexeme);
                }
                else
                {
                    compiler_diagnostic(1060, ctx, position, 0, "static_assert failed");
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
    return p_static_assert_declaration;
}


void attribute_specifier_sequence_add(struct attribute_specifier_sequence * list, struct attribute_specifier * p_item)
{
    if (list->head == 0)
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
            item->next = 0;
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

    p_attribute_specifier_sequence = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 9058)
        {
            p_attribute_specifier_sequence = calloc(1, 24);
            if (p_attribute_specifier_sequence == 0)
            {
                goto __L0; /* throw */
            }
            p_attribute_specifier_sequence->first_token = ctx->current;
            p_attribute_specifier_sequence->msvc_declspec_flags |= msvc_declspec_sequence_opt(ctx);
            return p_attribute_specifier_sequence;
        }
        if (ctx->current->type == 9046)
        {
            p_attribute_specifier_sequence = calloc(1, 24);
            if (p_attribute_specifier_sequence == 0)
            {
                goto __L0; /* throw */
            }
            p_attribute_specifier_sequence->first_token = ctx->current;
            gcc_attribute_specifier_opt(ctx);
            return p_attribute_specifier_sequence;
        }
        if (first_of_attribute_specifier(ctx))
        {
            p_attribute_specifier_sequence = calloc(1, 24);
            if (p_attribute_specifier_sequence == 0)
            {
                goto __L0; /* throw */
            }
            p_attribute_specifier_sequence->first_token = ctx->current;
            while (ctx->current != 0 && first_of_attribute_specifier(ctx))
            {
                struct attribute_specifier * p_attribute_specifier;

                p_attribute_specifier = attribute_specifier(ctx);
                if (p_attribute_specifier == 0)
                {
                    goto __L0; /* throw */
                }
                p_attribute_specifier_sequence->attributes_flags |= p_attribute_specifier->attribute_list->attributes_flags;
                attribute_specifier_sequence_add(p_attribute_specifier_sequence, p_attribute_specifier);
            }
            if (ctx->previous == 0)
            {
                goto __L0; /* throw */
            }
            p_attribute_specifier_sequence->last_token = ctx->previous;
        }
    }
    else __L0: /*catch*/ 
    {
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = 0;
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


struct attribute_list *attribute_list(struct parser_ctx * ctx, struct attribute_specifier * p_attribute_specifier);

struct attribute_specifier *attribute_specifier(struct parser_ctx * ctx)
{
    struct attribute_specifier * p_attribute_specifier;

    p_attribute_specifier = 0;
    if (1) /*try*/
    {
        struct attribute_list * p_attribute_list;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_attribute_specifier = calloc(1, 20);
        if (p_attribute_specifier == 0)
        {
            goto __L0; /* throw */
        }
        p_attribute_specifier->first_token = ctx->current;
        if (parser_match_tk(ctx, 91) != 0)
        {
            goto __L0; /* throw */
        }
        if (parser_match_tk(ctx, 91) != 0)
        {
            goto __L0; /* throw */
        }
        p_attribute_list = attribute_list(ctx, p_attribute_specifier);
        if (p_attribute_list == 0)
        {
            goto __L0; /* throw */
        }
        p_attribute_specifier->attribute_list = p_attribute_list;
        if (parser_match_tk(ctx, 93) != 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_attribute_specifier->last_token = ctx->current;
        if (parser_match_tk(ctx, 93) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        attribute_specifier_delete(p_attribute_specifier);
        p_attribute_specifier = 0;
    }
    return p_attribute_specifier;
}


void attribute_argument_clause_delete(struct attribute_argument_clause * p);

void attribute_delete(struct attribute * p)
{
    if (p)
    {
        attribute_argument_clause_delete(p->attribute_argument_clause);
        ;
        free(p);
    }
}


void attribute_list_add(struct attribute_list * list, struct attribute * p_item)
{
    if (list->head == 0)
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
        item->next = 0;
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


unsigned char first_of_attribute(struct parser_ctx * ctx);
struct attribute *attribute(struct parser_ctx * ctx, struct attribute_specifier * p_attribute_specifier);

struct attribute_list *attribute_list(struct parser_ctx * ctx, struct attribute_specifier * p_attribute_specifier)
{
    struct attribute_list * p_attribute_list;

    p_attribute_list = 0;
    if (1) /*try*/
    {
        p_attribute_list = calloc(1, 12);
        if (p_attribute_list == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && (first_of_attribute(ctx) || ctx->current->type == 44))
        {
            if (first_of_attribute(ctx))
            {
                struct attribute * p_attribute;

                p_attribute = attribute(ctx, p_attribute_specifier);
                if (p_attribute == 0)
                {
                    goto __L0; /* throw */
                }
                p_attribute_list->attributes_flags |= p_attribute->attributes_flags;
                attribute_list_add(p_attribute_list, p_attribute);
            }
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (ctx->current->type == 44)
            {
                parser_match(ctx);
            }
        }
    }
    else __L0: /*catch*/ 
    {
        attribute_list_delete(p_attribute_list);
        p_attribute_list = 0;
    }
    return p_attribute_list;
}


unsigned char first_of_attribute(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    if (ctx->current->type == 8996)
    {
        return 1;
    }
    if (is_keyword(ctx->current->lexeme, ctx->options.target) != 0)
    {
        return 1;
    }
    return 0;
}


int attribute_token(struct parser_ctx * ctx, struct attribute_specifier * p_attribute_specifier)
{
    int attribute_flags;

    attribute_flags = 0;
    if (1) /*try*/
    {
        struct token * attr_token;
        unsigned char  is_standard_attribute;
        unsigned char  is_cake_attr;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        attr_token = ctx->current;
        is_standard_attribute = 0;
        if (strcmp(attr_token->lexeme, "deprecated") == 0)
        {
            is_standard_attribute = 1;
            attribute_flags = 1;
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
                    attribute_flags = 4;
                }
                else
                {
                    if (strcmp(attr_token->lexeme, "noreturn") == 0)
                    {
                        is_standard_attribute = 1;
                        attribute_flags = 16;
                    }
                    else
                    {
                        if (strcmp(attr_token->lexeme, "reproducible") == 0)
                        {
                            is_standard_attribute = 1;
                            attribute_flags = 64;
                        }
                        else
                        {
                            if (strcmp(attr_token->lexeme, "unsequenced") == 0)
                            {
                                is_standard_attribute = 1;
                                attribute_flags = 32;
                            }
                            else
                            {
                                if (strcmp(attr_token->lexeme, "nodiscard") == 0)
                                {
                                    is_standard_attribute = 1;
                                    attribute_flags = 8;
                                }
                                else
                                {
                                    if (strcmp(attr_token->lexeme, "dtor") == 0)
                                    {
                                        is_standard_attribute = 1;
                                        attribute_flags = 256;
                                    }
                                    else
                                    {
                                        if (strcmp(attr_token->lexeme, "ctor") == 0)
                                        {
                                            is_standard_attribute = 1;
                                            attribute_flags = 128;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        is_cake_attr = strcmp(attr_token->lexeme, "cake") == 0;
        if (token_is_identifier_or_keyword(ctx->current->type))
        {
            parser_match(ctx);
        }
        else
        {
            compiler_diagnostic(970, ctx, attr_token, 0, "expected identifier");
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 14906)
        {
            parser_match(ctx);
            if (is_cake_attr)
            {
                if (ctx->current->lexeme[0] == 69 || ctx->current->lexeme[0] == 101 || ctx->current->lexeme[0] == 87 || ctx->current->lexeme[0] == 119)
                {
                    p_attribute_specifier->ack = atoi(ctx->current->lexeme + 1);
                }
                else
                {
                    if (strcmp(ctx->current->lexeme, "leak") == 0)
                    {
                        attribute_flags = 1073741824;
                    }
                    else
                    {
                        compiler_diagnostic(9, ctx, attr_token, 0, "warning '%s' is not an cake attribute", ctx->current->lexeme);
                    }
                }
            }
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (token_is_identifier_or_keyword(ctx->current->type))
            {
                parser_match(ctx);
            }
            else
            {
                compiler_diagnostic(970, ctx, attr_token, 0, "expected identifier");
            }
        }
        else
        {
            if (!is_standard_attribute)
            {
                compiler_diagnostic(9, ctx, attr_token, 0, "warning '%s' is not an standard attribute", attr_token->lexeme);
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
    return attribute_flags;
}


struct attribute_argument_clause *attribute_argument_clause(struct parser_ctx * ctx);

struct attribute *attribute(struct parser_ctx * ctx, struct attribute_specifier * p_attribute_specifier)
{
    struct attribute * p_attribute;

    p_attribute = 0;
    if (1) /*try*/
    {
        int attribute_flags;

        p_attribute = calloc(1, 20);
        if (p_attribute == 0)
        {
            goto __L0; /* throw */
        }
        attribute_flags = attribute_token(ctx, p_attribute_specifier);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_attribute->attributes_flags |= attribute_flags;
        if (ctx->current->type == 40)
        {
            struct attribute_argument_clause * p_attribute_argument_clause;

            p_attribute_argument_clause = attribute_argument_clause(ctx);
            if (p_attribute_argument_clause == 0)
            {
                goto __L0; /* throw */
            }
            p_attribute->attribute_argument_clause = p_attribute_argument_clause;
        }
    }
    else __L0: /*catch*/ 
    {
        attribute_delete(p_attribute);
        p_attribute = 0;
    }
    return p_attribute;
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

    p_attribute_argument_clause = calloc(1, 8);
    if (1) /*try*/
    {
        if (p_attribute_argument_clause == 0)
        {
            goto __L0; /* throw */
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto __L0; /* throw */
        }
        p_attribute_argument_clause->p_balanced_token_sequence = balanced_token_sequence_opt(ctx);
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        attribute_argument_clause_delete(p_attribute_argument_clause);
        p_attribute_argument_clause = 0;
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

    p_balanced_token_sequence = calloc(1, 8);
    if (1) /*try*/
    {
        int count1;
        int count2;
        int count3;

        if (p_balanced_token_sequence == 0)
        {
            goto __L0; /* throw */
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
            compiler_diagnostic(1090, ctx, ctx->current, 0, "expected ']' before ')'");
        }
        if (count3 != 0)
        {
            compiler_diagnostic(1090, ctx, ctx->current, 0, "expected '}' before ')'");
        }
    }
    else __L0: /*catch*/ 
    {
        balanced_token_sequence_delete(p_balanced_token_sequence);
        p_balanced_token_sequence = 0;
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


struct labeled_statement *labeled_statement(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence);
struct unlabeled_statement *unlabeled_statement(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence);

struct statement *statement(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence)
{
    struct statement * p_statement;

    p_statement = calloc(1, 8);
    if (1) /*try*/
    {
        if (p_statement == 0)
        {
            goto __L0; /* throw */
        }
        if (first_of_labeled_statement(ctx))
        {
            p_statement->labeled_statement = labeled_statement(ctx, p_attribute_specifier_sequence);
            p_attribute_specifier_sequence = 0;
            if (p_statement->labeled_statement == 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            p_statement->unlabeled_statement = unlabeled_statement(ctx, p_attribute_specifier_sequence);
            p_attribute_specifier_sequence = 0;
            if (p_statement->unlabeled_statement == 0)
            {
                goto __L0; /* throw */
            }
        }
    }
    else __L0: /*catch*/ 
    {
        statement_delete(p_statement);
        p_statement = 0;
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
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
    p_primary_block = calloc(1, 20);
    if (1) /*try*/
    {
        if (p_primary_block == 0)
        {
            goto __L0; /* throw */
        }
        if (first_of_compound_statement(ctx))
        {
            p_primary_block->compound_statement = compound_statement(ctx);
            if (p_primary_block->compound_statement == 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            if (first_of_selection_statement(ctx))
            {
                p_primary_block->selection_statement = selection_statement(ctx);
                if (p_primary_block->selection_statement == 0)
                {
                    goto __L0; /* throw */
                }
            }
            else
            {
                if (first_of_iteration_statement(ctx))
                {
                    p_primary_block->iteration_statement = iteration_statement(ctx);
                    if (p_primary_block->iteration_statement == 0)
                    {
                        goto __L0; /* throw */
                    }
                }
                else
                {
                    if (ctx->current->type == 9009)
                    {
                        p_primary_block->defer_statement = defer_statement(ctx);
                        if (p_primary_block->defer_statement == 0)
                        {
                            goto __L0; /* throw */
                        }
                    }
                    else
                    {
                        if (ctx->current->type == 9036)
                        {
                            p_primary_block->try_statement = try_statement(ctx);
                            if (p_primary_block->try_statement == 0)
                            {
                                goto __L0; /* throw */
                            }
                        }
                        else
                        {
                            compiler_diagnostic(970, ctx, ctx->current, 0, "unexpected token");
                        }
                    }
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        primary_block_delete(p_primary_block);
        p_primary_block = 0;
    }
    return p_primary_block;
}


static void check_open_brace_style(struct parser_ctx * ctx, struct token * token);
static void check_close_brace_style(struct parser_ctx * ctx, struct token * token);

struct secondary_block *secondary_block(struct parser_ctx * ctx)
{
    struct secondary_block * p_secondary_block;

    p_secondary_block = 0;
    if (1) /*try*/
    {
        struct attribute_specifier_sequence * p_attribute_specifier_sequence;
        struct statement * p_statement;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        check_open_brace_style(ctx, ctx->current);
        p_secondary_block = calloc(1, 12);
        if (p_secondary_block == 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_secondary_block->first_token = ctx->current;
        p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
        p_statement = statement(ctx, p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = 0;
        if (p_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_secondary_block->statement = p_statement;
        if (ctx->previous == 0)
        {
            goto __L0; /* throw */
        }
        p_secondary_block->last_token = ctx->previous;
        check_close_brace_style(ctx, p_secondary_block->last_token);
    }
    else __L0: /*catch*/ 
    {
        secondary_block_delete(p_secondary_block);
        p_secondary_block = 0;
    }
    return p_secondary_block;
}



static void check_open_brace_style(struct parser_ctx * ctx, struct token * token)
{
    if (token->level == 0 && !(token->flags & 2) && token->type == 123 && token->prev && is_diagnostic_enabled(&ctx->options, 11))
    {
        if (ctx->options.style == 0)
        {
            if (token->prev->type == 143 && token->prev->prev && token->prev->prev->type == 10)
            {
            }
            else
            {
                compiler_diagnostic(11, ctx, token, 0, "not following correct brace style {");
            }
        }
    }
}

static void check_close_brace_style(struct parser_ctx * ctx, struct token * token)
{
    if (token->level == 0 && !(token->flags & 2) && token->type == 125 && token->prev && token->prev->prev && is_diagnostic_enabled(&ctx->options, 11))
    {
        if (ctx->options.style == 0)
        {
            if (token->prev->type == 143 && token->prev->prev->type == 10)
            {
            }
            else
            {
                compiler_diagnostic(11, ctx, token, 0, "not following correct close brace style }");
            }
        }
    }
}
unsigned char unlabeled_statement_ends_with_jump(struct unlabeled_statement * p_unlabeled_statement)
{
    struct expression * p_expression;

    p_expression = 0;
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
        return ((p_expression->type.attributes_flags & 16) != 0);
    }
    if (p_unlabeled_statement->primary_block && p_unlabeled_statement->primary_block->compound_statement && p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail && p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement)
    {
        return p_unlabeled_statement->primary_block->compound_statement->block_item_list.tail->unlabeled_statement->jump_statement != 0;
    }
    return 0;
}


unsigned char secondary_block_ends_with_jump(struct secondary_block * p_secondary_block)
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


int get_diagnostic_phase(int w);
int diagnostic_id_stack_push(struct diagnostic_id_stack * diagnostic_stack, int id);

struct diagnostic_id_stack *build_diagnostic_id_stack(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence, struct diagnostic_id_stack * stack, int diagnostic_phase)
{
    struct diagnostic_id_stack * previous;
    struct attribute_specifier * p_attribute_specifier;

    previous = ctx->p_diagnostic_id_stack;
    ctx->p_diagnostic_id_stack = stack;
    if (p_attribute_specifier_sequence == 0)
    {
        return previous;
    }
    p_attribute_specifier = p_attribute_specifier_sequence->head;
    while (p_attribute_specifier)
    {
        int d;

        d = p_attribute_specifier->ack;
        if (d != 0)
        {
            if (get_diagnostic_phase(d) == diagnostic_phase)
            {
                diagnostic_id_stack_push(stack, p_attribute_specifier->ack);
            }
        }
        p_attribute_specifier = p_attribute_specifier->next;
    }
    return previous;
}


int get_warning_name_and_number(int w, int n, char buffer[]);

void warn_unrecognized_warnings(struct parser_ctx * ctx, struct diagnostic_id_stack * stack, struct attribute_specifier_sequence * p_attribute_specifier_sequence, struct diagnostic_id_stack * p_diagnostic_id_stack)
{
    struct token * token;

    ctx->p_diagnostic_id_stack = p_diagnostic_id_stack;
    if (p_attribute_specifier_sequence == 0)
    {
        return;
    }
    token = p_attribute_specifier_sequence->first_token;
    {
        int i;

        i = stack->size - 1;
        for (; i >= 0; i--)
        {
            char warning_name[200];

            _cake_zmem(&warning_name, 200);
            get_warning_name_and_number(stack->stack[i], 200, warning_name);
            compiler_diagnostic(59, ctx, token, 0, "warning '%s' was not recognized", warning_name);
        }
    }
}


static unsigned char first_of_primary_block(struct parser_ctx * ctx);
struct jump_statement *jump_statement(struct parser_ctx * ctx);
struct expression_statement *expression_statement(struct parser_ctx * ctx, unsigned char  ignore_semicolon, struct attribute_specifier_sequence * p_attribute_specifier_sequence);
unsigned char type_is_nodiscard(struct type * p_type);

struct unlabeled_statement *unlabeled_statement(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence)
{
    struct unlabeled_statement * p_unlabeled_statement;

    p_unlabeled_statement = calloc(1, 16);
    if (1) /*try*/
    {
        if (p_unlabeled_statement == 0)
        {
            goto __L0; /* throw */
        }
        if (first_of_primary_block(ctx))
        {
            struct diagnostic_id_stack  stack;
            struct diagnostic_id_stack * p_diagnostic_id_stack;

            p_unlabeled_statement->p_attribute_specifier_sequence = p_attribute_specifier_sequence;
            p_attribute_specifier_sequence = 0;
            _cake_zmem(&stack, 44);
            p_diagnostic_id_stack = build_diagnostic_id_stack(ctx, p_unlabeled_statement->p_attribute_specifier_sequence, &stack, 0);
            p_unlabeled_statement->primary_block = primary_block(ctx);
            warn_unrecognized_warnings(ctx, &stack, p_unlabeled_statement->p_attribute_specifier_sequence, p_diagnostic_id_stack);
            if (p_unlabeled_statement->primary_block == 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            if (first_of_jump_statement(ctx))
            {
                struct diagnostic_id_stack  stack;
                struct diagnostic_id_stack * p_diagnostic_id_stack;

                p_unlabeled_statement->p_attribute_specifier_sequence = p_attribute_specifier_sequence;
                p_attribute_specifier_sequence = 0;
                _cake_zmem(&stack, 44);
                p_diagnostic_id_stack = build_diagnostic_id_stack(ctx, p_unlabeled_statement->p_attribute_specifier_sequence, &stack, 0);
                p_unlabeled_statement->jump_statement = jump_statement(ctx);
                warn_unrecognized_warnings(ctx, &stack, p_unlabeled_statement->p_attribute_specifier_sequence, p_diagnostic_id_stack);
                if (p_unlabeled_statement->jump_statement == 0)
                {
                    goto __L0; /* throw */
                }
            }
            else
            {
                p_unlabeled_statement->expression_statement = expression_statement(ctx, 0, p_attribute_specifier_sequence);
                p_attribute_specifier_sequence = 0;
                if (p_unlabeled_statement->expression_statement == 0)
                {
                    goto __L0; /* throw */
                }
                if (p_unlabeled_statement->expression_statement->expression_opt)
                {
                    if (!type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type) && type_is_nodiscard(&p_unlabeled_statement->expression_statement->expression_opt->type) && p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & 8192)
                    {
                        if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                        {
                            compiler_diagnostic(9, ctx, p_unlabeled_statement->expression_statement->expression_opt->first_token, 0, "ignoring return value of function declared with 'nodiscard' attribute");
                        }
                    }
                    if (type_is_owner(&p_unlabeled_statement->expression_statement->expression_opt->type) && p_unlabeled_statement->expression_statement->expression_opt->type.storage_class_specifier_flags & 8192)
                    {
                        if (p_unlabeled_statement->expression_statement->expression_opt->first_token->level == 0)
                        {
                            compiler_diagnostic(9, ctx, p_unlabeled_statement->expression_statement->expression_opt->first_token, 0, "ignoring the result of _Owner type ");
                        }
                    }
                }
                if (p_unlabeled_statement->expression_statement->expression_opt && p_unlabeled_statement->expression_statement->expression_opt->expression_type == 12)
                {
                }
                else
                {
                    if (p_unlabeled_statement != 0 && p_unlabeled_statement->jump_statement == 0 && p_unlabeled_statement->expression_statement != 0 && p_unlabeled_statement->expression_statement->expression_opt && !type_is_void(&p_unlabeled_statement->expression_statement->expression_opt->type) && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 60 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 12 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 16 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 17 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 33 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 34 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 28 && p_unlabeled_statement->expression_statement->expression_opt->expression_type != 31)
                    {
                        if (ctx->current && ctx->current->level == 0)
                        {
                        }
                    }
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        unlabeled_statement_delete(p_unlabeled_statement);
        p_unlabeled_statement = 0;
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
    return p_unlabeled_statement;
}



static unsigned char first_of_primary_block(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    if (first_of_compound_statement(ctx) || first_of_selection_statement(ctx) || first_of_iteration_statement(ctx) || ctx->current->type == 9009 || ctx->current->type == 9036)
    {
        return 1;
    }
    return 0;
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
struct label *case_label_list_find_range(struct parser_ctx * ctx, struct case_label_list * list, struct object * begin, struct object * end);
int object_to_str(struct object * a, int n, char str[]);
struct label *case_label_list_find(struct parser_ctx * ctx, struct case_label_list * list, struct object * object);
void case_label_list_push(struct case_label_list * list, struct label * pnew);
unsigned char type_is_enum(struct type * p_type);
void check_diferent_enuns(struct parser_ctx * ctx, struct token * operator_token, struct expression * left, struct expression * right, char * message);
struct label *case_label_list_find_default(struct parser_ctx * ctx, struct case_label_list * list);

struct label *label(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence)
{
    struct label * p_label;

    if (ctx->current == 0)
    {
        return 0;
    }
    p_label = calloc(1, 28);
    if (1) /*try*/
    {
        struct diagnostic_id_stack  stack;
        struct diagnostic_id_stack * p_diagnostic_id_stack;

        if (p_label == 0)
        {
            goto __L1; /* throw */
        }
        p_label->p_attribute_specifier_sequence = p_attribute_specifier_sequence;
        p_attribute_specifier_sequence = 0;
        _cake_zmem(&stack, 44);
        p_diagnostic_id_stack = build_diagnostic_id_stack(ctx, p_label->p_attribute_specifier_sequence, &stack, 0);
        p_label->label_id = ctx->label_id++;
        p_label->p_first_token = ctx->current;
        if (ctx->current->type == 8996)
        {
            struct label_list_item * p_label_list_item;

            p_label_list_item = label_list_find(&ctx->label_list, ctx->current->lexeme);
            if (p_label_list_item == 0)
            {
                struct label_list_item * p_label_list_item_new;

                p_label_list_item_new = calloc(1, 12);
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
                    compiler_diagnostic(1440, ctx, ctx->current, 0, "duplicated label '%s'", ctx->current->lexeme);
                    compiler_diagnostic(63, ctx, p_label_list_item->p_defined, 0, "previous definition of '%s'", ctx->current->lexeme);
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
                goto __L1; /* throw */
            }
        }
        else
        {
            if (ctx->current->type == 9001)
            {
                if (ctx->p_current_selection_statement == 0 || ctx->p_current_selection_statement->condition == 0)
                {
                    goto __L1; /* throw */
                }
                parser_match(ctx);
                p_label->constant_expression = constant_expression(ctx, 1, 2);
                if (p_label->constant_expression == 0)
                {
                    goto __L1; /* throw */
                }
                if (ctx->current == 0)
                {
                    unexpected_end_of_file(ctx);
                    goto __L1; /* throw */
                }
                if (ctx->current->type == 3026478)
                {
                    struct label * p_existing_label;

                    parser_match(ctx);
                    p_label->constant_expression_end = constant_expression(ctx, 1, 2);
                    if (p_label->constant_expression_end == 0)
                    {
                        goto __L1; /* throw */
                    }
                    p_existing_label = case_label_list_find_range(ctx, &ctx->p_current_selection_statement->label_list, &p_label->constant_expression->object, &p_label->constant_expression_end->object);
                    if (p_existing_label)
                    {
                        char str1[50];
                        char str2[50];

                        object_to_str(&p_label->constant_expression->object, 50, str1);
                        object_to_str(&p_label->constant_expression_end->object, 50, str2);
                        compiler_diagnostic(1450, ctx, p_label->constant_expression->first_token, 0, "case '%s' ... '%s' is duplicating values", str1, str2);
                        ;
                        compiler_diagnostic(62, ctx, p_existing_label->constant_expression->first_token, 0, "previous case");
                    }
                }
                else
                {
                    struct label * p_existing_label;

                    p_existing_label = case_label_list_find(ctx, &ctx->p_current_selection_statement->label_list, &p_label->constant_expression->object);
                    if (p_existing_label)
                    {
                        char str[50];

                        object_to_str(&p_label->constant_expression->object, 50, str);
                        compiler_diagnostic(1450, ctx, p_label->constant_expression->first_token, 0, "duplicate case '%s'", str);
                        ;
                        compiler_diagnostic(62, ctx, p_existing_label->constant_expression->first_token, 0, "previous declaration");
                    }
                }
                if (parser_match_tk(ctx, 58) != 0)
                {
                    goto __L1; /* throw */
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
                    if (ctx->p_current_selection_statement == 0)
                    {
                        goto __L1; /* throw */
                    }
                    p_enum_specifier = 0;
                    if (ctx->p_current_selection_statement->condition && ctx->p_current_selection_statement->condition->expression && ctx->p_current_selection_statement->condition->expression->type.enum_specifier)
                    {
                        p_enum_specifier = get_complete_enum_specifier(ctx->p_current_selection_statement->condition->expression->type.enum_specifier);
                    }
                    if (p_enum_specifier)
                    {
                        struct enumerator * p_enumerator;

                        p_enumerator = find_enumerator_by_value(ctx, p_enum_specifier, &p_label->constant_expression->object);
                        if (p_enumerator == 0)
                        {
                            char str[50];

                            object_to_str(&p_label->constant_expression->object, 50, str);
                            compiler_diagnostic(4, ctx, p_label->constant_expression->first_token, 0, "case value '%s' not in enumerated type 'enum %s'", str, p_enum_specifier->tag_name);
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

                    p_existing_default_label = case_label_list_find_default(ctx, &ctx->p_current_selection_statement->label_list);
                    if (p_existing_default_label)
                    {
                        compiler_diagnostic(1780, ctx, p_label->p_first_token, 0, "multiple default labels in one switch");
                        compiler_diagnostic(63, ctx, p_existing_default_label->p_first_token, 0, "previous default");
                        goto __L1; /* throw */
                    }
                    parser_match(ctx);
                    if (parser_match_tk(ctx, 58) != 0)
                    {
                        goto __L1; /* throw */
                    }
                    case_label_list_push(&ctx->p_current_selection_statement->label_list, p_label);
                }
            }
        }
        warn_unrecognized_warnings(ctx, &stack, p_label->p_attribute_specifier_sequence, p_diagnostic_id_stack);
    }
    else __L1: /*catch*/ 
    {
        label_delete(p_label);
        p_label = 0;
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
    return p_label;
}


struct label *case_label_list_find_default(struct parser_ctx * ctx, struct case_label_list * list)
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
    return 0;
}


int object_is_greater_than_or_equal(int target, struct object * a, struct object * b);
int object_is_smaller_than_or_equal(int target, struct object * a, struct object * b);

struct label *case_label_list_find_range(struct parser_ctx * ctx, struct case_label_list * list, struct object * begin, struct object * end)
{
    struct label * p;

    p = list->head;
    while (p)
    {
        if (p->constant_expression_end == 0)
        {
            if (object_is_greater_than_or_equal(ctx->options.target, &p->constant_expression->object, begin) && object_is_smaller_than_or_equal(ctx->options.target, &p->constant_expression_end->object, end))
            {
                return p;
            }
        }
        else
        {
            if (object_is_smaller_than_or_equal(ctx->options.target, &p->constant_expression->object, end) && object_is_smaller_than_or_equal(ctx->options.target, begin, &p->constant_expression_end->object))
            {
                return p;
            }
        }
        p = p->next;
    }
    return 0;
}


struct label *case_label_list_find(struct parser_ctx * ctx, struct case_label_list * list, struct object * object)
{
    struct label * p;

    p = list->head;
    while (p)
    {
        if (p->constant_expression_end == 0)
        {
            if (p->constant_expression && object_is_equal(ctx->options.target, &p->constant_expression->object, object))
            {
                return p;
            }
        }
        else
        {
            if (p->constant_expression && object_is_greater_than_or_equal(ctx->options.target, object, &p->constant_expression->object) && object_is_smaller_than_or_equal(ctx->options.target, object, &p->constant_expression_end->object))
            {
                return p;
            }
        }
        p = p->next;
    }
    return 0;
}


void case_label_list_push(struct case_label_list * list, struct label * pnew)
{
    if (list->head == 0)
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


struct labeled_statement *labeled_statement(struct parser_ctx * ctx, struct attribute_specifier_sequence * p_attribute_specifier_sequence)
{
    struct labeled_statement * p_labeled_statement;

    p_labeled_statement = calloc(1, 8);
    if (1) /*try*/
    {
        struct label * p_label;
        struct statement * p_statement;

        if (p_labeled_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_label = label(ctx, p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = 0;
        if (p_label == 0)
        {
            goto __L0; /* throw */
        }
        p_labeled_statement->label = p_label;
        p_statement = statement(ctx, p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = 0;
        if (p_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_labeled_statement->statement = p_statement;
    }
    else __L0: /*catch*/ 
    {
        labeled_statement_delete(p_labeled_statement);
        p_labeled_statement = 0;
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
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


struct block_item_list block_item_list(struct parser_ctx * ctx, unsigned char * error);

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
    p_compound_statement = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_compound_statement = calloc(1, 48);
        if (p_compound_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_compound_statement->diagnostic_flags = ctx->options.diagnostic_stack.stack[ctx->options.diagnostic_stack.top_index];
        scope_list_push(&ctx->scopes, &block_scope);
        p_compound_statement->first_token = ctx->current;
        if (parser_match_tk(ctx, 123) != 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            compiler_diagnostic(970, ctx, ctx->input_list.tail, 0, "unexpected end of file");
            goto __L0; /* throw */
        }
        if (ctx->current->type != 125)
        {
            unsigned char  berror;

            berror = 0;
            p_compound_statement->block_item_list = block_item_list(ctx, &berror);
            if (berror)
            {
                goto __L0; /* throw */
            }
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_compound_statement->last_token = ctx->current;
        if (parser_match_tk(ctx, 125) != 0)
        {
            goto __L0; /* throw */
        }
        {
            int i;

            i = 0;
            for (; i < block_scope.variables.capacity; i++)
            {
                struct map_entry * entry;

                if (block_scope.variables.table == 0)
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
                    p_declarator = 0;
                    p_init_declarator = 0;
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
                                compiler_diagnostic(2, ctx, p_declarator->name_opt, 0, "'%s': unreferenced declarator", p_declarator->name_opt->lexeme);
                            }
                        }
                    }
                    entry = entry->next;
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        compound_statement_delete(p_compound_statement);
        p_compound_statement = 0;
    }
    scope_list_pop(&ctx->scopes);
    scope_destroy(&block_scope);
    return p_compound_statement;
}


void block_item_list_add(struct block_item_list * list, struct block_item * p_item)
{
    if (list->head == 0)
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
        item->next = 0;
        block_item_delete(item);
        item = next;
    }
}


struct block_item *block_item(struct parser_ctx * ctx);

struct block_item_list block_item_list(struct parser_ctx * ctx, unsigned char * error)
{
    struct block_item_list  block_item_list;
    struct block_item * p_block_item;

    *error = 0;
    _cake_zmem(&block_item_list, 8);
    p_block_item = 0;
    if (1) /*try*/
    {
        p_block_item = block_item(ctx);
        if (p_block_item == 0)
        {
            goto __L0; /* throw */
        }
        block_item_list_add(&block_item_list, p_block_item);
        p_block_item = 0;
        while (ctx->current != 0 && ctx->current->type != 125)
        {
            p_block_item = block_item(ctx);
            if (p_block_item == 0)
            {
                goto __L0; /* throw */
            }
            block_item_list_add(&block_item_list, p_block_item);
            p_block_item = 0;
        }
    }
    else __L0: /*catch*/ 
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
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;

    p_block_item = 0;
    p_attribute_specifier_sequence = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_block_item = calloc(1, 20);
        if (p_block_item == 0)
        {
            goto __L0; /* throw */
        }
        p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_block_item->first_token = ctx->current;
        if (ctx->current->type == 9059)
        {
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
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
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
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

                p_block_item->declaration = declaration(ctx, p_attribute_specifier_sequence, 4096, 0);
                p_attribute_specifier_sequence = 0;
                if (p_block_item->declaration == 0)
                {
                    goto __L0; /* throw */
                }
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
                    p_block_item->label = label(ctx, p_attribute_specifier_sequence);
                    p_attribute_specifier_sequence = 0;
                    if (p_block_item->label == 0)
                    {
                        goto __L0; /* throw */
                    }
                }
                else
                {
                    p_block_item->unlabeled_statement = unlabeled_statement(ctx, p_attribute_specifier_sequence);
                    p_attribute_specifier_sequence = 0;
                    if (p_block_item->unlabeled_statement == 0)
                    {
                        goto __L0; /* throw */
                    }
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        block_item_delete(p_block_item);
        p_block_item = 0;
    }
    attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
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

    p_try_statement = 0;
    if (1) /*try*/
    {
        struct try_statement * try_statement_copy_opt;
        struct secondary_block * p_secondary_block;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_try_statement = calloc(1, 24);
        if (p_try_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_try_statement->first_token = ctx->current;
        ;
        try_statement_copy_opt = ctx->p_current_try_statement_opt;
        ctx->p_current_try_statement_opt = p_try_statement;
        p_try_statement->catch_label_id = ctx->label_id++;
        if (parser_match_tk(ctx, 9036) != 0)
        {
            goto __L0; /* throw */
        }
        p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == 0)
        {
            goto __L0; /* throw */
        }
        p_try_statement->secondary_block = p_secondary_block;
        ctx->p_current_try_statement_opt = try_statement_copy_opt;
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 9006)
        {
            p_try_statement->catch_token_opt = ctx->current;
            parser_match(ctx);
            ;
            p_try_statement->catch_secondary_block_opt = secondary_block(ctx);
            if (p_try_statement->catch_secondary_block_opt == 0)
            {
                goto __L0; /* throw */
            }
        }
        if (ctx->previous == 0)
        {
            goto __L0; /* throw */
        }
        p_try_statement->last_token = ctx->previous;
    }
    else __L0: /*catch*/ 
    {
        try_statement_delete(p_try_statement);
        p_try_statement = 0;
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


unsigned char style_has_one_space(struct token * token);
struct init_statement *init_statement(struct parser_ctx * ctx, unsigned char  ignore_semicolon);
struct condition *condition(struct parser_ctx * ctx);
unsigned char type_is_bool(struct type * p_type);
unsigned char type_is_essential_bool(struct type * p_type);

struct selection_statement *selection_statement(struct parser_ctx * ctx)
{
    struct scope  if_scope;
    struct selection_statement * p_selection_statement;

    _cake_zmem(&if_scope, 36);
    scope_list_push(&ctx->scopes, &if_scope);
    p_selection_statement = 0;
    if (1) /*try*/
    {
        unsigned char  is_if;
        struct selection_statement * previous;
        struct secondary_block * p_secondary_block;
        struct token * p_tk;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_selection_statement = calloc(1, 56);
        if (p_selection_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_selection_statement->label_id = ctx->label_id++;
        p_selection_statement->first_token = ctx->current;
        is_if = (ctx->current->type == 9017);
        if (ctx->current->type == 9017 || ctx->current->type == 9034)
        {
            parser_match(ctx);
        }
        else
        {
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (!(ctx->current->flags & 2) && !style_has_one_space(ctx->current))
        {
            compiler_diagnostic(11, ctx, ctx->current, 0, "one space");
        }
        p_selection_statement->open_parentesis_token = ctx->current;
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto __L0; /* throw */
        }
        p_selection_statement->p_init_statement = init_statement(ctx, 1);
        if (p_selection_statement->p_init_statement == 0)
        {
            goto __L0; /* throw */
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
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
                p_selection_statement->condition = calloc(1, 24);
                if (p_selection_statement->condition == 0)
                {
                    goto __L0; /* throw */
                }
                if (p_selection_statement->p_init_statement->p_expression_statement && p_selection_statement->p_init_statement->p_expression_statement->expression_opt)
                {
                    p_selection_statement->condition->first_token = p_selection_statement->p_init_statement->p_expression_statement->expression_opt->first_token;
                    p_selection_statement->condition->last_token = p_selection_statement->p_init_statement->p_expression_statement->expression_opt->last_token;
                    p_selection_statement->condition->expression = p_selection_statement->p_init_statement->p_expression_statement->expression_opt;
                    p_selection_statement->p_init_statement->p_expression_statement->expression_opt = 0;
                }
                if (p_selection_statement->p_init_statement->p_simple_declaration)
                {
                    p_selection_statement->condition->first_token = p_selection_statement->p_init_statement->p_simple_declaration->first_token;
                    p_selection_statement->condition->last_token = p_selection_statement->p_init_statement->p_simple_declaration->last_token;
                    if (p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head != p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.tail)
                    {
                        ;
                        goto __L0; /* throw */
                    }
                    p_selection_statement->condition->p_init_declarator = p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head;
                    p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.head = 0;
                    p_selection_statement->p_init_statement->p_simple_declaration->init_declarator_list.tail = 0;
                    p_selection_statement->condition->p_declaration_specifiers = p_selection_statement->p_init_statement->p_simple_declaration->p_declaration_specifiers;
                    p_selection_statement->p_init_statement->p_simple_declaration->p_declaration_specifiers = 0;
                }
                init_statement_delete(p_selection_statement->p_init_statement);
                p_selection_statement->p_init_statement = 0;
            }
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_selection_statement->close_parentesis_token = ctx->current;
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto __L0; /* throw */
        }
        if (!is_if && p_selection_statement && p_selection_statement->condition)
        {
            if (type_is_bool(&p_selection_statement->condition->expression->type) || type_is_essential_bool(&p_selection_statement->condition->expression->type))
            {
                compiler_diagnostic(45, ctx, p_selection_statement->condition->first_token, 0, "switch condition has boolean value");
            }
        }
        previous = ctx->p_current_selection_statement;
        ctx->p_current_selection_statement = p_selection_statement;
        p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == 0)
        {
            goto __L0; /* throw */
        }
        if (p_secondary_block->statement->unlabeled_statement && p_secondary_block->statement->unlabeled_statement->expression_statement && p_secondary_block->statement->unlabeled_statement->expression_statement->expression_opt == 0)
        {
            compiler_diagnostic(45, ctx, p_secondary_block->first_token, 0, "empty controlled statement found; is this the intent?");
        }
        ;
        p_selection_statement->secondary_block = p_secondary_block;
        if (p_selection_statement->first_token->type == 9034)
        {
            if (case_label_list_find_default(ctx, &p_selection_statement->label_list) == 0)
            {
                struct enum_specifier * p_enum_specifier;

                p_enum_specifier = 0;
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

                        p_used = case_label_list_find(ctx, &p_selection_statement->label_list, &p->value);
                        if (p_used == 0)
                        {
                            compiler_diagnostic(45, ctx, ctx->current, 0, "enumeration '%s' not handled in switch", p->token->lexeme);
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
            if (p_secondary_block2 == 0)
            {
                goto __L0; /* throw */
            }
            p_selection_statement->else_secondary_block_opt = p_secondary_block2;
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_tk = previous_parser_token(ctx->current);
        if (p_tk == 0)
        {
            goto __L0; /* throw */
        }
        p_selection_statement->last_token = p_tk;
    }
    else __L0: /*catch*/ 
    {
        selection_statement_delete(p_selection_statement);
        p_selection_statement = 0;
    }
    scope_list_pop(&ctx->scopes);
    scope_destroy(&if_scope);
    return p_selection_statement;
}


struct defer_statement *defer_statement(struct parser_ctx * ctx)
{
    struct defer_statement * p_defer_statement;

    p_defer_statement = 0;
    if (1) /*try*/
    {
        struct defer_statement * p_previous_defer_statement_opt;
        struct secondary_block * p_secondary_block;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type != 9009)
        {
            goto __L0; /* throw */
        }
        p_defer_statement = calloc(1, 12);
        if (p_defer_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_defer_statement->first_token = ctx->current;
        parser_match(ctx);
        p_previous_defer_statement_opt = ctx->p_current_defer_statement_opt;
        ctx->p_current_defer_statement_opt = p_defer_statement;
        p_secondary_block = secondary_block(ctx);
        if (p_secondary_block == 0)
        {
            goto __L0; /* throw */
        }
        p_defer_statement->secondary_block = p_secondary_block;
        if (ctx->previous == 0)
        {
            goto __L0; /* throw */
        }
        p_defer_statement->last_token = ctx->previous;
        ctx->p_current_defer_statement_opt = p_previous_defer_statement_opt;
    }
    else __L0: /*catch*/ 
    {
        defer_statement_delete(p_defer_statement);
        p_defer_statement = 0;
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

    p_iteration_statement = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_iteration_statement = calloc(1, 36);
        if (p_iteration_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_iteration_statement->first_token = ctx->current;
        if (ctx->current->type == 9008)
        {
            struct secondary_block * p_secondary_block;

            parser_match(ctx);
            p_secondary_block = secondary_block(ctx);
            if (p_secondary_block == 0)
            {
                goto __L0; /* throw */
            }
            p_iteration_statement->secondary_block = p_secondary_block;
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            p_iteration_statement->second_token = ctx->current;
            if (parser_match_tk(ctx, 9042) != 0)
            {
                goto __L0; /* throw */
            }
            if (parser_match_tk(ctx, 40) != 0)
            {
                goto __L0; /* throw */
            }
            p_iteration_statement->expression1 = expression(ctx, 2);
            if (parser_match_tk(ctx, 41) != 0)
            {
                goto __L0; /* throw */
            }
            if (parser_match_tk(ctx, 59) != 0)
            {
                goto __L0; /* throw */
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
                    goto __L0; /* throw */
                }
                p_iteration_statement->expression1 = expression(ctx, 2);
                if (parser_match_tk(ctx, 41) != 0)
                {
                    goto __L0; /* throw */
                }
                p_secondary_block = secondary_block(ctx);
                if (p_secondary_block == 0)
                {
                    goto __L0; /* throw */
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
                        goto __L0; /* throw */
                    }
                    if (first_of_declaration_specifier(ctx))
                    {
                        struct scope  for_scope;
                        struct secondary_block * p_secondary_block;

                        _cake_zmem(&for_scope, 36);
                        scope_list_push(&ctx->scopes, &for_scope);
                        p_iteration_statement->declaration = declaration(ctx, 0, 4096, 0);
                        if (ctx->current == 0)
                        {
                            unexpected_end_of_file(ctx);
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto __L0; /* throw */
                        }
                        if (ctx->current->type != 59)
                        {
                            p_iteration_statement->expression1 = expression(ctx, 2);
                            if (p_iteration_statement->expression1 == 0)
                            {
                                scope_list_pop(&ctx->scopes);
                                scope_destroy(&for_scope);
                                goto __L0; /* throw */
                            }
                        }
                        if (parser_match_tk(ctx, 59) != 0)
                        {
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto __L0; /* throw */
                        }
                        if (ctx->current == 0)
                        {
                            unexpected_end_of_file(ctx);
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto __L0; /* throw */
                        }
                        if (ctx->current->type != 41)
                        {
                            p_iteration_statement->expression2 = expression(ctx, 2);
                        }
                        if (parser_match_tk(ctx, 41) != 0)
                        {
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto __L0; /* throw */
                        }
                        p_secondary_block = secondary_block(ctx);
                        if (p_secondary_block == 0)
                        {
                            scope_list_pop(&ctx->scopes);
                            scope_destroy(&for_scope);
                            goto __L0; /* throw */
                        }
                        p_iteration_statement->secondary_block = p_secondary_block;
                        scope_list_pop(&ctx->scopes);
                        scope_destroy(&for_scope);
                    }
                    else
                    {
                        struct secondary_block * p_secondary_block;

                        if (ctx->current == 0)
                        {
                            unexpected_end_of_file(ctx);
                            goto __L0; /* throw */
                        }
                        if (ctx->current->type != 59)
                        {
                            p_iteration_statement->expression0 = expression(ctx, 2);
                        }
                        if (parser_match_tk(ctx, 59) != 0)
                        {
                            goto __L0; /* throw */
                        }
                        if (ctx->current == 0)
                        {
                            unexpected_end_of_file(ctx);
                            goto __L0; /* throw */
                        }
                        if (ctx->current->type != 59)
                        {
                            p_iteration_statement->expression1 = expression(ctx, 2);
                        }
                        if (parser_match_tk(ctx, 59) != 0)
                        {
                            goto __L0; /* throw */
                        }
                        if (ctx->current == 0)
                        {
                            unexpected_end_of_file(ctx);
                            goto __L0; /* throw */
                        }
                        if (ctx->current->type != 41)
                        {
                            p_iteration_statement->expression2 = expression(ctx, 2);
                        }
                        if (parser_match_tk(ctx, 41) != 0)
                        {
                            goto __L0; /* throw */
                        }
                        if (ctx->current == 0)
                        {
                            unexpected_end_of_file(ctx);
                            goto __L0; /* throw */
                        }
                        p_secondary_block = secondary_block(ctx);
                        if (p_secondary_block == 0)
                        {
                            goto __L0; /* throw */
                        }
                        p_iteration_statement->secondary_block = p_secondary_block;
                    }
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        iteration_statement_delete(p_iteration_statement);
        p_iteration_statement = 0;
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

    p_jump_statement = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_jump_statement = calloc(1, 28);
        if (p_jump_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_jump_statement->first_token = ctx->current;
        if (ctx->current->type == 9016)
        {
            struct label_list_item * p_label_list_item;

            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            p_label_list_item = label_list_find(&ctx->label_list, ctx->current->lexeme);
            if (p_label_list_item == 0)
            {
                struct label_list_item * p_label_list_item_new;

                p_label_list_item_new = calloc(1, 12);
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
                goto __L0; /* throw */
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
                        if (ctx->p_current_try_statement_opt == 0)
                        {
                            compiler_diagnostic(1110, ctx, ctx->current, 0, "throw statement not within try block");
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

                            if (ctx->p_current_defer_statement_opt != 0)
                            {
                                compiler_diagnostic(1400, ctx, ctx->current, 0, "%s", "return cannot be used inside defer statement");
                                goto __L0; /* throw */
                            }
                            p_return_token = ctx->current;
                            parser_match(ctx);
                            if (ctx->current == 0)
                            {
                                unexpected_end_of_file(ctx);
                                goto __L0; /* throw */
                            }
                            return_type = get_function_return_type(&ctx->p_current_function_opt->type);
                            if (ctx->current->type != 59)
                            {
                                p_jump_statement->expression_opt = expression(ctx, 2);
                                if (p_jump_statement->expression_opt)
                                {
                                    if (type_is_void(&return_type))
                                    {
                                        char * func_name;

                                        func_name = ctx->p_current_function_opt->name_opt ? ctx->p_current_function_opt->name_opt->lexeme : "unnamed";
                                        compiler_diagnostic(1120, ctx, p_return_token, 0, "void function '%s' should not return a value", func_name);
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
                                    compiler_diagnostic(1121, ctx, p_return_token, 0, "non void function '%s' should return a value", func_name);
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
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_jump_statement->last_token = ctx->current;
        if (parser_match_tk(ctx, 59) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        jump_statement_delete(p_jump_statement);
        p_jump_statement = 0;
    }
    return p_jump_statement;
}


void expression_statement_delete(struct expression_statement * p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence);
        expression_delete(p->expression_opt);
        free(p);
    }
}


struct expression_statement *expression_statement(struct parser_ctx * ctx, unsigned char  ignore_semicolon, struct attribute_specifier_sequence * p_attribute_specifier_sequence)
{
    struct expression_statement * p_expression_statement;

    p_expression_statement = calloc(1, 8);
    if (1) /*try*/
    {
        struct diagnostic_id_stack  stack;
        struct diagnostic_id_stack * p_diagnostic_id_stack;

        if (p_expression_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_expression_statement->p_attribute_specifier_sequence = p_attribute_specifier_sequence;
        p_attribute_specifier_sequence = 0;
        _cake_zmem(&stack, 44);
        p_diagnostic_id_stack = build_diagnostic_id_stack(ctx, p_expression_statement->p_attribute_specifier_sequence, &stack, 0);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type != 59)
        {
            p_expression_statement->expression_opt = expression(ctx, 2);
            warn_unrecognized_warnings(ctx, &stack, p_expression_statement->p_attribute_specifier_sequence, p_diagnostic_id_stack);
            if (p_expression_statement->expression_opt == 0)
            {
                goto __L0; /* throw */
            }
        }
        ctx->p_diagnostic_id_stack = p_diagnostic_id_stack;
        if (!ignore_semicolon && parser_match_tk(ctx, 59) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        expression_statement_delete(p_expression_statement);
        p_expression_statement = 0;
    }
    return p_expression_statement;
}


void declaration_list_add(struct declaration_list * list, struct declaration * p_declaration)
{
    if (list->head == 0)
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
        attribute_specifier_sequence_delete(p_simple_declaration->p_attribute_specifier_sequence);
        free(p_simple_declaration);
    }
}


void condition_delete(struct condition * p_condition)
{
    if (p_condition)
    {
        init_declarator_delete(p_condition->p_init_declarator);
        expression_delete(p_condition->expression);
        attribute_specifier_sequence_delete(p_condition->p_attribute_specifier_sequence);
        declaration_specifiers_delete(p_condition->p_declaration_specifiers);
        free(p_condition);
    }
}


static struct attribute_specifier_sequence *attribute_specifier_sequence(struct parser_ctx * ctx);

struct condition *condition(struct parser_ctx * ctx)
{
    struct condition * p_condition;

    p_condition = 0;
    if (1) /*try*/
    {
        struct token * previous;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_condition = calloc(1, 24);
        if (p_condition == 0)
        {
            goto __L0; /* throw */
        }
        p_condition->first_token = ctx->current;
        if (first_of_declaration_specifier(ctx))
        {
            struct init_declarator * p_init_declarator;

            p_condition->p_attribute_specifier_sequence = attribute_specifier_sequence(ctx);
            p_condition->p_declaration_specifiers = declaration_specifiers(ctx, 4096);
            if (p_condition->p_declaration_specifiers == 0)
            {
                goto __L0; /* throw */
            }
            p_init_declarator = init_declarator(ctx, p_condition->p_declaration_specifiers);
            if (p_init_declarator == 0)
            {
                goto __L0; /* throw */
            }
            p_condition->p_init_declarator = p_init_declarator;
        }
        else
        {
            p_condition->expression = expression(ctx, 2);
            if (p_condition->expression == 0)
            {
                goto __L0; /* throw */
            }
        }
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        previous = previous_parser_token(ctx->current);
        if (previous)
        {
            p_condition->last_token = previous;
        }
    }
    else __L0: /*catch*/ 
    {
        condition_delete(p_condition);
        p_condition = 0;
    }
    return p_condition;
}



static struct attribute_specifier_sequence *attribute_specifier_sequence(struct parser_ctx * ctx)
{
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;

    p_attribute_specifier_sequence = 0;
    if (1) /*try*/
    {
        p_attribute_specifier_sequence = calloc(1, 24);
        if (p_attribute_specifier_sequence == 0)
        {
            goto __L0; /* throw */
        }
        while (first_of_attribute_specifier(ctx))
        {
            struct attribute_specifier * p_attribute_specifier;

            p_attribute_specifier = attribute_specifier(ctx);
            if (p_attribute_specifier == 0)
            {
                goto __L0; /* throw */
            }
            attribute_specifier_sequence_add(p_attribute_specifier_sequence, p_attribute_specifier);
        }
    }
    else __L0: /*catch*/ 
    {
        attribute_specifier_sequence_delete(p_attribute_specifier_sequence);
        p_attribute_specifier_sequence = 0;
    }
    return p_attribute_specifier_sequence;
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


struct init_statement *init_statement(struct parser_ctx * ctx, unsigned char  ignore_semicolon)
{
    struct init_statement * p_init_statement;

    p_init_statement = 0;
    if (1) /*try*/
    {
        struct attribute_specifier_sequence * p_attribute_specifier_sequence;

        p_init_statement = calloc(1, 8);
        if (p_init_statement == 0)
        {
            goto __L0; /* throw */
        }
        p_attribute_specifier_sequence = attribute_specifier_sequence_opt(ctx);
        if (first_of_declaration_specifier(ctx))
        {
            p_init_statement->p_simple_declaration = simple_declaration(ctx, ignore_semicolon, p_attribute_specifier_sequence);
            p_attribute_specifier_sequence = 0;
        }
        else
        {
            p_init_statement->p_expression_statement = expression_statement(ctx, ignore_semicolon, p_attribute_specifier_sequence);
            p_attribute_specifier_sequence = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        init_statement_delete(p_init_statement);
        p_init_statement = 0;
    }
    return p_init_statement;
}


void declaration_delete(struct declaration * p)
{
    if (p)
    {
        attribute_specifier_sequence_delete(p->p_attribute_specifier_sequence);
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
        p->next = 0;
        declaration_delete(p);
        p = next;
    }
}


struct declaration *external_declaration(struct parser_ctx * ctx)
{
    return function_definition_or_declaration(ctx);
}


static void check_unused_static_declarators(struct parser_ctx * ctx, struct declaration_list * declaration_list);

struct declaration_list translation_unit(struct parser_ctx * ctx, unsigned char * berror)
{
    struct declaration_list  declaration_list;

    *berror = 0;
    _cake_zmem(&declaration_list, 8);
    if (1) /*try*/
    {
        while (ctx->current != 0)
        {
            struct declaration * p;

            p = external_declaration(ctx);
            if (p == 0)
            {
                goto __L0; /* throw */
            }
            declaration_list_add(&declaration_list, p);
        }
        check_unused_static_declarators(ctx, &declaration_list);
    }
    else __L0: /*catch*/ 
    {
        *berror = 1;
    }
    return declaration_list;
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

                p_entry = find_variables(ctx, p->init_declarator_list.head->p_declarator->name_opt->lexeme, 0);
                if (p_entry && (p_entry->type == 4 || p_entry->type == 5))
                {
                    struct declarator * p_declarator_local;
                    int num_uses;

                    p_declarator_local = 0;
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
                            compiler_diagnostic(57, ctx, p->init_declarator_list.head->p_declarator->name_opt, 0, "warning: static function '%s' not used.", p->init_declarator_list.head->p_declarator->name_opt->lexeme);
                        }
                        else
                        {
                            compiler_diagnostic(2, ctx, p->init_declarator_list.head->p_declarator->name_opt, 0, "warning: '%s' not used.", p->init_declarator_list.head->p_declarator->name_opt->lexeme);
                        }
                    }
                }
            }
        }
        p = p->next;
    }
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
    return 0;
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
        item->next = 0;
        free(item);
        item = next;
    }
    list->head = 0;
    list->tail = 0;
}


void label_list_push(struct label_list * list, struct label_list_item * pitem)
{
    if (list->head == 0)
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
        if (item->p_defined == 0 && item->p_last_usage != 0)
        {
            compiler_diagnostic(1430, ctx, item->p_last_usage, 0, "label '%s' used but not defined", item->p_last_usage->lexeme);
        }
        else
        {
            if (item->p_defined != 0 && item->p_last_usage == 0)
            {
                compiler_diagnostic(55, ctx, item->p_defined, 0, "label '%s' defined but not used", item->p_defined->lexeme);
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
    ctx->p_current_try_statement_opt = 0;
    label_id = ctx->label_id;
    ctx->label_id = 0;
    p_current_defer_statement_opt = ctx->p_current_defer_statement_opt;
    ctx->p_current_defer_statement_opt = 0;
    p_current_selection_statement = ctx->p_current_selection_statement;
    ctx->p_current_selection_statement = 0;
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


struct declaration_list parse(struct parser_ctx * ctx, struct token_list * list, unsigned char * berror)
{
    struct declaration_list  l;
    struct scope  file_scope;

    *berror = 0;
    _cake_zmem(&l, 8);
    _cake_zmem(&file_scope, 36);
    if (1) /*try*/
    {
        unsigned char  local_error;

        scope_list_push(&ctx->scopes, &file_scope);
        ctx->input_list = *list;
        ctx->current = ctx->input_list.head;
        parser_skip_blanks(ctx);
        local_error = 0;
        l = translation_unit(ctx, &local_error);
        if (local_error)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        *berror = 1;
    }
    scope_destroy(&file_scope);
    return l;
}


struct include_dir *include_dir_add(struct include_dir_list * list, char * path);
struct token_list tokenizer(struct tokenizer_ctx * ctx, char * text, char * filename_opt, int level, int addflags);
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
                snprintf(buffer, 200, "#define %s \n", argv[i] + 2);
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
    n = GetEnvironmentVariableA("INCLUDE", env, 2000);
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

    outfile = 0;
    error = 0;
    if (1) /*try*/
    {
        char env[2000];
        int n;
        char * p;

        outfile = fopen(configpath, "w");
        if (outfile == 0)
        {
            printf("Cannot open the file '%s' for writing.\n", configpath);
            error = (*_errno());
            goto __L0; /* throw */
        }
        fprintf(outfile, "//This was generated by running cake -autoconfig \n");
        _cake_zmem(&env, 2000);
        n = GetEnvironmentVariableA("INCLUDE", env, 2000);
        if (n <= 0)
        {
            printf("INCLUDE not found.\nPlease, run cake -autoconfig inside visual studio command prompt.\n");
            error = 1;
            goto __L0; /* throw */
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
    else __L0: /*catch*/ 
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


void add_standard_macros(struct preprocessor_ctx * ctx, int target);
int include_config_header(struct preprocessor_ctx * ctx, char * file_name);
char *read_file(char * path, unsigned char  append_newline);
int __cdecl _mkdir(char * _Path);
char *basename(char * filename);
void print_tokens(struct token * p_token);
char *print_preprocessed_to_string2(struct token * p_token);
void d_visit(struct d_visit_ctx * ctx, struct osstream * oss);
void d_visit_ctx_destroy(struct d_visit_ctx * ctx);
char *get_posix_error_message(int error);
void remove_file_extension(char * filename, int n, char out[]);
static int get_first_line_len(char * s);
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
    prectx.options = *options;
    prectx.macros.capacity = 5000;
    add_standard_macros(&prectx, options->target);
    if (include_config_header(&prectx, file_name) != 0)
    {
    }
    _cake_zmem(&ast, 16);
    s = 0;
    _cake_zmem(&ctx, 776);
    _cake_zmem(&tctx, 696);
    _cake_zmem(&tokens, 8);
    ctx.options = *options;
    ctx.p_report = report;
    content = 0;
    if (1) /*try*/
    {
        if (fill_preprocessor_options(argc, argv, &prectx) != 0)
        {
            goto __L1; /* throw */
        }
        prectx.options = *options;
        append_msvc_include_dir(&prectx);
        content = read_file(file_name, 1);
        if (content == 0)
        {
            report->error_count++;
            printf("file not found '%s'\n", file_name);
            goto __L1; /* throw */
        }
        if (options->sarif_output)
        {
            char sarif_file_name[260];

            _cake_zmem(&sarif_file_name, 260);
            if (options->sarifpath[0] != 0)
            {
                _mkdir(options->sarifpath);
                snprintf(sarif_file_name, 260, "%s/%s.cake.sarif", options->sarifpath, basename(file_name));
            }
            else
            {
                snprintf(sarif_file_name, 260, "%s.cake.sarif", file_name);
            }
            ctx.sarif_file = (struct _iobuf *)fopen(sarif_file_name, "w");
            if (ctx.sarif_file)
            {
                char * begin_sarif;

                begin_sarif = "{\n  \"version\": \"2.1.0\",\n  \"$schema\": \"https://schemastore.azurewebsites.net/schemas/json/sarif-2.1.0-rtm.5.json\",\n  \"runs\": [\n    {\n      \"results\": [\n\n";
                fprintf(ctx.sarif_file, "%s", begin_sarif);
            }
            else
            {
                report->error_count++;
                printf("cannot open Sarif output file '%s'\n", sarif_file_name);
                goto __L1; /* throw */
            }
        }
        tokens = tokenizer(&tctx, content, file_name, 0, 0);
        if (tctx.n_errors > 0)
        {
            goto __L1; /* throw */
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
            goto __L1; /* throw */
        }
        if (options->dump_pptokens)
        {
            if (ast.token_list.head != 0)
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
            unsigned char  berror;

            berror = 0;
            ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
            if (berror || report->error_count > 0)
            {
                goto __L1; /* throw */
            }
            if (!options->no_output)
            {
                struct osstream  ss;
                struct d_visit_ctx  ctx2;
                struct _iobuf * outfile;

                _cake_zmem(&ss, 12);
                _cake_zmem(&ctx2, 840);
                ctx2.ast = ast;
                ctx2.options = ctx.options;
                d_visit(&ctx2, &ss);
                s = ss.c_str;
                d_visit_ctx_destroy(&ctx2);
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
                    goto __L1; /* throw */
                }
            }
        }
        if (ctx.sarif_file)
        {
            fprintf(ctx.sarif_file, "%s", "      ],\n      \"tool\": {\n        \"driver\": {\n          \"name\": \"cake\",\n          \"fullName\": \"cake code analysis\",\n          \"version\": \"0.12.26\",\n          \"informationUri\": \"https://https://github.com/thradams/cake\"\n        }\n      }\n    }\n  ]\n}\n\n");
            fclose(ctx.sarif_file);
            ctx.sarif_file = 0;
        }
    }
    else __L1: /*catch*/ 
    {
    }
    if (ctx.options.test_mode)
    {
        char file_name_no_ext[500];
        char buf[500];
        char * content_expected;

        _cake_zmem(&file_name_no_ext, 500);
        remove_file_extension(file_name, 500, file_name_no_ext);
        _cake_zmem(&buf, 500);
        snprintf(buf, 500, "%s_%s.out", file_name_no_ext, get_platform(ctx.options.target)->name);
        content_expected = read_file(buf, 0);
        if (content_expected)
        {
            int s_first_line_len;
            int content_expected_first_line_len;

            s_first_line_len = get_first_line_len(s);
            content_expected_first_line_len = get_first_line_len(content_expected);
            if (s && strcmp(content_expected + content_expected_first_line_len, s + s_first_line_len) != 0)
            {
                printf("different");
                report->error_count++;
            }
            free(content_expected);
        }
        if (report->error_count > 0 || report->warnings_count > 0)
        {
            printf("-------------------------------------------\n");
            printf("%s", content);
            printf("\n-------------------------------------------\n");
            if (ctx.options.disable_colors)
            {
                printf("TEST FAILED : error=%d, warnings=%d\n", report->error_count, report->warnings_count);
            }
            else
            {
                printf("\x1b[91mTEST FAILED\x1b[0m : error=%d, warnings=%d\n", report->error_count, report->warnings_count);
            }
            printf("\n\n");
            report->test_failed++;
        }
        else
        {
            report->test_succeeded++;
            if (ctx.options.disable_colors)
            {
                printf("TEST OK\n");
            }
            else
            {
                printf("\x1b[92mTEST OK\n\x1b[0m");
            }
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



static int get_first_line_len(char * s)
{
    int n;

    n = 0;
    while (*s && (*s != 13 && *s != 10))
    {
        s++;
        n++;
    }
    return n;
}
int fill_options(struct options * options, int argc, char ** argv);
int get_self_path(char * buffer, int maxsize);
char *dirname(char * path);
long __cdecl clock(void);
static void longest_common_path(int argc, char ** argv, char root_dir[500]);
char *realpath(char * path, char * resolved_path);
char *__cdecl strcpy(char * _Destination, char * _Source);
char *__cdecl strncpy(char * _Destination, char * _Source, unsigned int _Count);
unsigned int __cdecl strlen(char * _Str);
static int create_multiple_paths(char * root, char * outdir);
static int compile_many_files(char * file_name, struct options * options, char * out_file_name, int argc, char ** argv, struct report * report);
struct TAGDIR *opendir(char * name);
struct dirent *readdir(struct TAGDIR * dirp);
int closedir(struct TAGDIR * dirp);

int compile(int argc, char ** argv, struct report * report)
{
    struct options  options;
    char executable_path[486];
    char cakeconfig_path[500];
    long begin_clock;
    int no_files;
    char root_dir[500];
    unsigned int root_dir_len;
    long end_clock;
    double cpu_time_used;

    _cake_zmem(&options, 688);
    if (fill_options(&options, argc, argv) != 0)
    {
        return 1;
    }
    if (options.target != 1)
    {
        printf("emulating %s\n", get_platform(options.target)->name);
    }
    _cake_zmem(&executable_path, 486);
    get_self_path(executable_path, 486);
    dirname(executable_path);
    _cake_zmem(&cakeconfig_path, 500);
    snprintf(cakeconfig_path, 500, "%s/cakeconfig.h", executable_path);
    if (options.auto_config)
    {
        report->ignore_this_report = 1;
        return generate_config_file(cakeconfig_path);
    }
    report->test_mode = options.test_mode;
    begin_clock = clock();
    no_files = 0;
    _cake_zmem(&root_dir, 500);
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
            char output_file[500];
            char fullpath[500];
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
            _cake_zmem(&output_file, 500);
            if (!options.no_output)
            {
                if (no_files == 1 && options.output[0] != 0)
                {
                    strcat(output_file, options.output);
                }
                else
                {
                    char fullpath[500];
                    char outdir[500];

                    _cake_zmem(&fullpath, 500);
                    realpath(argv[i], fullpath);
                    strcpy(output_file, root_dir);
                    strcat(output_file, "/");
                    strcat(output_file, get_platform(options.target)->name);
                    strcat(output_file, fullpath + root_dir_len);
                    _cake_zmem(&outdir, 500);
                    strcpy(outdir, output_file);
                    dirname(outdir);
                    if (create_multiple_paths(root_dir, outdir) != 0)
                    {
                        return 1;
                    }
                }
            }
            _cake_zmem(&fullpath, 500);
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

                _cake_zmem(&report_local, 48);
                compile_one_file(fullpath, &options, output_file, argc, argv, &report_local);
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



static void longest_common_path(int argc, char ** argv, char root_dir[500])
{
    {
        int i;

        i = 1;
        for (; i < argc; i++)
        {
            char fullpath_i[500];

            if (argv[i][0] == 45)
            {
                continue;
            }
            _cake_zmem(&fullpath_i, 500);
            realpath(argv[i], fullpath_i);
            strcpy(root_dir, fullpath_i);
            dirname(root_dir);
            {
                int k;

                k = 0;
                for (; k < 500; k++)
                {
                    char ch;

                    ch = fullpath_i[k];
                    {
                        int j;

                        j = 2;
                        for (; j < argc; j++)
                        {
                            char fullpath_j[500];

                            if (argv[j][0] == 45)
                            {
                                continue;
                            }
                            _cake_zmem(&fullpath_j, 500);
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

static int create_multiple_paths(char * root, char * outdir)
{
    char * p;

    p = outdir + strlen(root) + 1;
    for (; ; )
    {
        char temp[500];
        int er;

        if (*p != 0 && *p != 47 && *p != 92)
        {
            p++;
            continue;
        }
        _cake_zmem(&temp, 500);
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

static int compile_many_files(char * file_name, struct options * options, char * out_file_name, int argc, char ** argv, struct report * report)
{
    char * file_name_name;
    char * file_name_extension;
    int num_files;
    char path[500];
    struct TAGDIR * dir;
    struct dirent * dp;

    file_name_name = basename(file_name);
    file_name_extension = strrchr2((char *)file_name_name, 46);
    if (file_name_extension == 0)
    {
        ;
    }
    num_files = 0;
    _cake_zmem(&path, 500);
    snprintf(path, 500, "%s", file_name);
    dirname(path);
    dir = opendir(path);
    if (dir == 0)
    {
        return (*_errno());
    }
    while ((dp = readdir(dir)) != 0)
    {
        char fromlocal[257];

        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }
        _cake_zmem(&fromlocal, 257);
        snprintf(fromlocal, 257, "%s/%s", "", dp->d_name);
        if (dp->d_type & 4)
        {
        }
        else
        {
            char * file_name_iter;
            char * file_extension;

            file_name_iter = basename(dp->d_name);
            file_extension = strrchr2((char *)file_name_iter, 46);
            if (file_name_extension && file_extension && strcmp(file_name_extension, file_extension) == 0)
            {
                char out_file_name_final[500];
                char in_file_name_final[500];
                struct report  report_local;

                _cake_zmem(&out_file_name_final, 500);
                strcpy(out_file_name_final, out_file_name);
                dirname(out_file_name_final);
                strcat(out_file_name_final, "/");
                strcat(out_file_name_final, file_name_iter);
                _cake_zmem(&in_file_name_final, 500);
                strcpy(in_file_name_final, file_name);
                dirname(in_file_name_final);
                strcat(in_file_name_final, "/");
                strcat(in_file_name_final, file_name_iter);
                _cake_zmem(&report_local, 48);
                report_local.test_mode = report->test_mode;
                compile_one_file(in_file_name_final, options, out_file_name_final, argc, argv, &report_local);
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
    _cake_zmem(&ctx, 776);
    ctx.p_report = report;
    if (1) /*try*/
    {
        unsigned char  berror;

        prectx.options = *options;
        prectx.macros.capacity = 5000;
        add_standard_macros(&prectx, options->target);
        ast.token_list = preprocessor(&prectx, &list, 0);
        if (prectx.n_errors != 0)
        {
            goto __L0; /* throw */
        }
        ctx.options = *options;
        berror = 0;
        ast.declaration_list = parse(&ctx, &ast.token_list, &berror);
        if (berror)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
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
    snprintf(string, 200, "exepath %s", pszoptions);
    argc = strtoargv(string, 10, argv);
    s = 0;
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
    options.disable_colors = 0;
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
    if (1) /*try*/
    {
        if (fill_options(&options, argc, argv) != 0)
        {
            goto __L0; /* throw */
        }
        prectx.options = options;
        add_standard_macros(&prectx, options.target);
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
            struct osstream  ss;
            struct d_visit_ctx  ctx2;

            ast = get_ast(&options, "c:/main.c", content, report);
            if (report->error_count > 0)
            {
                goto __L0; /* throw */
            }
            _cake_zmem(&ss, 12);
            _cake_zmem(&ctx2, 840);
            ctx2.ast = ast;
            ctx2.options = options;
            d_visit(&ctx2, &ss);
            s = ss.c_str;
            d_visit_ctx_destroy(&ctx2);
        }
    }
    else __L0: /*catch*/ 
    {
    }
    preprocessor_ctx_destroy(&prectx);
    ast_destroy(&ast);
    return s;
}


char *CompileText(char * pszoptions, char * content)
{
    struct report  report;

    printf("\x1b[97mCake 0.12.26\x1b[0m\n");
    printf("\x1b[97mcake %s main.c\n", pszoptions);
    _cake_zmem(&report, 48);
    return (char *)compile_source(pszoptions, content, &report);
}


void ast_destroy(struct ast * ast)
{
    token_list_destroy(&ast->token_list);
    declaration_list_destroy(&ast->declaration_list);
}


static unsigned char is_snake_case(char * text);
static unsigned char is_pascal_case(char * text);

void naming_convention_struct_tag(struct parser_ctx * ctx, struct token * token)
{
    if (!is_diagnostic_enabled(&ctx->options, 11) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(11, ctx, token, 0, "use snake_case for struct/union tags");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if (!is_pascal_case(token->lexeme))
            {
                compiler_diagnostic(11, ctx, token, 0, "use camelCase for struct/union tags");
            }
        }
    }
}



static unsigned char is_snake_case(char * text)
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

static unsigned char is_pascal_case(char * text)
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
void naming_convention_enum_tag(struct parser_ctx * ctx, struct token * token)
{
    if (!is_diagnostic_enabled(&ctx->options, 11) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(11, ctx, token, 0, "use snake_case for enum tags");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if (!is_pascal_case(token->lexeme))
            {
                compiler_diagnostic(11, ctx, token, 0, "use PascalCase for enum tags");
            }
        }
    }
}


void naming_convention_function(struct parser_ctx * ctx, struct token * token)
{
    if (!is_diagnostic_enabled(&ctx->options, 11) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(11, ctx, token, 0, "use snake_case for functions");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if (!is_pascal_case(token->lexeme))
            {
                compiler_diagnostic(11, ctx, token, 0, "use PascalCase for functions");
            }
        }
    }
}


unsigned char type_is_function_or_function_pointer(struct type * p_type);

void naming_convention_global_var(struct parser_ctx * ctx, struct token * token, struct type * type, int storage)
{
    if (!is_diagnostic_enabled(&ctx->options, 11) || token->level != 0)
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
                    compiler_diagnostic(11, ctx, token, 0, "use prefix s_ for static global variables");
                }
            }
        }
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(11, ctx, token, 0, "use snake_case global variables");
        }
    }
}


static unsigned char is_camel_case(char * text);

void naming_convention_local_var(struct parser_ctx * ctx, struct token * token, struct type * type)
{
    if (!is_diagnostic_enabled(&ctx->options, 11) || token->level != 0)
    {
        return;
    }
    if (ctx->options.style == 0)
    {
        if (!is_snake_case(token->lexeme))
        {
            compiler_diagnostic(11, ctx, token, 0, "use snake_case for local variables");
        }
    }
    else
    {
        if (ctx->options.style == 6)
        {
            if (!is_camel_case(token->lexeme))
            {
                compiler_diagnostic(11, ctx, token, 0, "use camelCase for local variables");
            }
        }
    }
}



static unsigned char is_camel_case(char * text)
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
static unsigned char is_all_upper(char * text);
int __cdecl toupper(int _C);

void naming_convention_enumerator(struct parser_ctx * ctx, struct token * token)
{
    if (!is_diagnostic_enabled(&ctx->options, 11) || token->level != 0)
    {
        return;
    }
    if (!is_all_upper(token->lexeme))
    {
        compiler_diagnostic(11, ctx, token, 0, "use UPPERCASE for enumerators");
    }
}



static unsigned char is_all_upper(char * text)
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
void naming_convention_struct_member(struct parser_ctx * ctx, struct token * token, struct type * type)
{
    if (!is_diagnostic_enabled(&ctx->options, 11) || token->level != 0)
    {
        return;
    }
    if (!is_snake_case(token->lexeme))
    {
        compiler_diagnostic(11, ctx, token, 0, "use snake_case for struct members");
    }
}


void naming_convention_parameter(struct parser_ctx * ctx, struct token * token, struct type * type)
{
    if (!is_diagnostic_enabled(&ctx->options, 11) || token->level != 0)
    {
        return;
    }
    if (!is_snake_case(token->lexeme))
    {
        compiler_diagnostic(11, ctx, token, 0, "use snake_case for arguments");
    }
}


int object_set(struct parser_ctx * ctx, struct object * to, struct expression * init_expression, struct object * from, unsigned char  is_constant, unsigned char  requires_constant_initialization);
static int braced_initializer_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * current_object, struct braced_initializer * braced_initializer, unsigned char  is_constant, unsigned char  requires_constant_initialization);
void object_default_initialization(struct object * p_object, unsigned char  is_constant);
unsigned char type_is_union(struct type * p_type);
unsigned char type_is_scalar(struct type * p_type);
static struct initializer_list_item *find_innner_initializer_list_item(struct braced_initializer * braced_initializer);
struct type get_array_item_type(struct type * p_type);
unsigned char type_is_char(struct type * p_type);
struct object *object_extend_array_to_index(struct type * p_type, struct object * a, unsigned int n, unsigned char  is_constant, int target);
static struct object *find_designated_subobject(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * current_object, struct designator * p_designator, unsigned char  is_constant, struct type * p_type_out2, unsigned char  not_error, int target);
unsigned char type_is_struct_or_union(struct type * p_type);
struct object *object_get_member(struct object * p_object, unsigned int index);
static struct object *find_next_subobject(struct type * p_top_object_not_used, struct object * current_object, struct object * it, struct type * p_type_out, unsigned char * sub_object_of_union);
static struct object *find_next_subobject_old(struct type * p_top_object_not_used, struct object * current_object, struct object * it, struct type * p_type_out, unsigned char * sub_object_of_union);
void type_clear(struct type * a);
static struct object *find_first_subobject(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out, unsigned char * sub_object_of_union);
static struct object *find_first_subobject_old(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out, unsigned char * sub_object_of_union);
static struct object *find_last_suboject_of_suboject(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out);
static struct object *find_last_suboject_of_suboject_old(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out);
unsigned char type_is_array_of_char(struct type * p_type);

int initializer_init_new(struct parser_ctx * ctx, struct type * p_type, struct object * object, struct initializer * initializer, unsigned char  is_constant, unsigned char  requires_constant_initialization)
{
    if (1) /*try*/
    {
        if (initializer->assignment_expression != 0)
        {
            if (object_set(ctx, object, initializer->assignment_expression, &initializer->assignment_expression->object, is_constant, requires_constant_initialization) != 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            if (initializer->braced_initializer)
            {
                if (braced_initializer_new(ctx, p_type, object, initializer->braced_initializer, is_constant, requires_constant_initialization) != 0)
                {
                    goto __L0; /* throw */
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
        return 1;
    }
    return 0;
}



static struct initializer_list_item *find_innner_initializer_list_item(struct braced_initializer * braced_initializer)
{
    struct initializer_list_item * p_initializer_list_item;

    ;
    p_initializer_list_item = braced_initializer->initializer_list->head;
    while (p_initializer_list_item->initializer->braced_initializer)
    {
        p_initializer_list_item = p_initializer_list_item->initializer->braced_initializer->initializer_list->head;
        if (p_initializer_list_item == 0)
        {
            ;
            return 0;
        }
        if (p_initializer_list_item->next == 0)
        {
            return p_initializer_list_item;
        }
        p_initializer_list_item = p_initializer_list_item->next;
    }
    return p_initializer_list_item;
}

static struct object *find_designated_subobject(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * current_object, struct designator * p_designator, unsigned char  is_constant, struct type * p_type_out2, unsigned char  not_error, int target)
{
    if (1) /*try*/
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
            if (p_struct_or_union_specifier == 0)
            {
                goto __L0; /* throw */
            }
            p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
            p_member_declarator = 0;
            name = p_designator->token->lexeme;
            p_member_object = current_object->members.head;
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
                                if (p_designator->next != 0)
                                {
                                    return find_designated_subobject(ctx, &p_member_declarator->declarator->type, p_member_object, p_designator->next, is_constant, p_type_out2, 0, ctx->options.target);
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

                                _cake_zmem(&t, 68);
                                t.category = 0;
                                t.struct_or_union_specifier = p_complete;
                                t.type_specifier_flags = 32768;
                                p = find_designated_subobject(ctx, &t, p_member_object, p_designator, is_constant, p_type_out2, 1, ctx->options.target);
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
                compiler_diagnostic(720, ctx, p_designator->token, 0, "member '%s' not found in '%s'", name, p_struct_or_union_specifier->tag_name);
            }
            return 0;
        }
        else
        {
            if (type_is_array(p_current_object_type))
            {
                unsigned char  compute_array_size;
                long long index;
                long long max_index;
                struct type  array_item_type;
                struct object * member_obj;

                compute_array_size = p_current_object_type->array_num_elements_expression == 0;
                index = -1;
                max_index = -1;
                array_item_type = get_array_item_type(p_current_object_type);
                member_obj = current_object->members.head;
                if (p_designator->constant_expression_opt)
                {
                    index = object_to_signed_long_long(&p_designator->constant_expression_opt->object);
                    if (index > max_index)
                    {
                        max_index = index;
                        if (compute_array_size)
                        {
                            member_obj = object_extend_array_to_index(&array_item_type, current_object, (unsigned int)max_index, is_constant, target);
                        }
                    }
                    member_obj = object_get_member(current_object, (unsigned int)index);
                    if (member_obj == 0)
                    {
                        if (index < 0)
                        {
                            compiler_diagnostic(720, ctx, p_designator->constant_expression_opt->first_token, 0, "array designator value '%d' is negative", index);
                        }
                        else
                        {
                            if (index > (int)p_current_object_type->num_of_elements)
                            {
                                compiler_diagnostic(720, ctx, p_designator->constant_expression_opt->first_token, 0, "array index '%d' in initializer exceeds array bounds", index);
                            }
                        }
                        type_destroy(&array_item_type);
                        return 0;
                    }
                    if (p_designator->next != 0)
                    {
                        struct object * p;

                        p = find_designated_subobject(ctx, &array_item_type, member_obj, p_designator->next, is_constant, p_type_out2, 0, ctx->options.target);
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
    else __L0: /*catch*/ 
    {
    }
    return 0;
}

static struct object *find_next_subobject_old(struct type * p_top_object_not_used, struct object * current_object, struct object * it, struct type * p_type_out, unsigned char * sub_object_of_union)
{
    type_clear(p_type_out);
    if (it == 0)
    {
        return 0;
    }
    if (it->members.head)
    {
        struct type  t;

        *sub_object_of_union = type_is_union(&it->type);
        it = it->members.head;
        t = type_dup(&it->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
        return it;
    }
    for (; ; )
    {
        struct object * next;

        if (it == 0)
        {
            break;
        }
        next = it->next;
        if (next != 0)
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
    if (it != 0)
    {
        struct type  t;

        t = type_dup(&it->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
    }
    return it;
}

static struct object *find_next_subobject(struct type * p_top_object_not_used, struct object * current_object, struct object * it, struct type * p_type_out, unsigned char * sub_object_of_union)
{
    return find_next_subobject_old(p_top_object_not_used, current_object, it, p_type_out, sub_object_of_union);
}

static struct object *find_first_subobject_old(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out, unsigned char * sub_object_of_union)
{
    struct type  t;

    p_object = (struct object *)object_get_referenced(p_object);
    if (p_object->members.head == 0)
    {
        struct type  t;

        *sub_object_of_union = 0;
        t = type_dup(&p_object->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
        return p_object;
    }
    *sub_object_of_union = type_is_union(&p_object->type);
    t = type_dup(&p_object->members.head->type);
    type_swap(&t, p_type_out);
    type_destroy(&t);
    return p_object->members.head;
}

static struct object *find_first_subobject(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out, unsigned char * sub_object_of_union)
{
    return find_first_subobject_old(p_type_not_used, p_object, p_type_out, sub_object_of_union);
}

static struct object *find_last_suboject_of_suboject_old(struct type * p_type_not_used, struct object * p_object, struct type * p_type_out)
{
    struct object * it;
    struct type  t;

    p_object = (struct object *)object_get_referenced(p_object);
    if (p_object->members.head == 0)
    {
        struct type  t;

        t = type_dup(&p_object->type);
        type_swap(&t, p_type_out);
        type_destroy(&t);
        return p_object;
    }
    it = p_object->members.head;
    while (it)
    {
        if (it->next == 0)
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

static int braced_initializer_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * current_object, struct braced_initializer * braced_initializer, unsigned char  is_constant, unsigned char  requires_constant_initialization)
{
    if (1) /*try*/
    {
        struct object * parent_copy;
        struct initializer_list_item * p_initializer_list_item;
        int array_to_expand_index;
        int array_to_expand_max_index;
        unsigned char  compute_array_size;
        struct type  array_item_type;
        struct object * p_subobject;

        if (braced_initializer->initializer_list == 0)
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
            if (p_initializer_list_item == 0)
            {
                return 0;
            }
            if (p_initializer_list_item->initializer->assignment_expression != 0)
            {
                if (object_set(ctx, current_object, p_initializer_list_item->initializer->assignment_expression, &p_initializer_list_item->initializer->assignment_expression->object, is_constant, requires_constant_initialization) != 0)
                {
                    goto __L0; /* throw */
                }
            }
            p_initializer_list_item = p_initializer_list_item->next;
            if (p_initializer_list_item != 0)
            {
                compiler_diagnostic(100, ctx, p_initializer_list_item->initializer->first_token, 0, "warning: excess elements in initializer");
            }
            return 0;
        }
        parent_copy = current_object->parent;
        current_object->parent = 0;
        p_initializer_list_item = braced_initializer->initializer_list->head;
        array_to_expand_index = -1;
        array_to_expand_max_index = -1;
        compute_array_size = 0;
        _cake_zmem(&array_item_type, 68);
        if (type_is_array(p_current_object_type))
        {
            array_item_type = get_array_item_type(p_current_object_type);
            compute_array_size = p_current_object_type->array_num_elements_expression == 0;
            if (type_is_char(&array_item_type))
            {
                struct initializer_list_item * p_initializer_list_item2;

                p_initializer_list_item2 = find_innner_initializer_list_item(braced_initializer);
                if (p_initializer_list_item2 == 0)
                {
                    type_destroy(&array_item_type);
                    return 0;
                }
                if (p_initializer_list_item2->initializer->assignment_expression != 0)
                {
                    if (p_initializer_list_item2->initializer->assignment_expression->expression_type == 3)
                    {
                        unsigned int num_of_elements;

                        num_of_elements = p_initializer_list_item2->initializer->assignment_expression->type.num_of_elements;
                        if (compute_array_size)
                        {
                            object_extend_array_to_index(&array_item_type, current_object, num_of_elements - 1, is_constant, ctx->options.target);
                        }
                        if (object_set(ctx, current_object, p_initializer_list_item2->initializer->assignment_expression, &p_initializer_list_item2->initializer->assignment_expression->object, is_constant, requires_constant_initialization) != 0)
                        {
                            type_destroy(&array_item_type);
                            goto __L0; /* throw */
                        }
                        p_current_object_type->num_of_elements = num_of_elements;
                        type_destroy(&array_item_type);
                        return 0;
                    }
                }
            }
        }
        p_subobject = 0;
        for (; ; )
        {
            unsigned char  is_subobject_of_union;
            struct type  subobject_type;

            is_subobject_of_union = 0;
            _cake_zmem(&subobject_type, 68);
            if (p_initializer_list_item == 0)
            {
                break;
            }
            if (p_initializer_list_item->designation)
            {
                if (compute_array_size)
                {
                    array_to_expand_index = (int)object_to_signed_long_long(&p_initializer_list_item->designation->designator_list->head->constant_expression_opt->object);
                    if (array_to_expand_index > array_to_expand_max_index)
                    {
                        array_to_expand_max_index = array_to_expand_index;
                    }
                    object_extend_array_to_index(&array_item_type, current_object, array_to_expand_max_index, is_constant, ctx->options.target);
                }
                is_subobject_of_union = type_is_union(&subobject_type);
                p_subobject = find_designated_subobject(ctx, p_current_object_type, current_object, p_initializer_list_item->designation->designator_list->head, is_constant, &subobject_type, 0, ctx->options.target);
                if (p_subobject == 0)
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
                    if (po == 0)
                    {
                        array_to_expand_index++;
                        if (array_to_expand_index > array_to_expand_max_index)
                        {
                            array_to_expand_max_index = array_to_expand_index;
                        }
                        object_extend_array_to_index(&array_item_type, current_object, array_to_expand_max_index, is_constant, ctx->options.target);
                    }
                }
                if (p_subobject == 0)
                {
                    p_subobject = find_first_subobject(p_current_object_type, current_object, &subobject_type, &is_subobject_of_union);
                }
                else
                {
                    p_subobject = find_next_subobject(p_current_object_type, current_object, p_subobject, &subobject_type, &is_subobject_of_union);
                }
            }
            if (p_subobject == 0)
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
                    goto __L0; /* throw */
                }
                _cake_zmem(&t, 68);
                is_subobject_of_union = type_is_union(&subobject_type);
                p_subobject = find_last_suboject_of_suboject(&subobject_type, p_subobject, &t);
                type_swap(&t, &subobject_type);
                type_destroy(&t);
            }
            else
            {
                if (p_initializer_list_item->initializer->assignment_expression)
                {
                    unsigned char  entire_object_initialized;

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
                        goto __L0; /* throw */
                    }
                    if (is_subobject_of_union)
                    {
                        struct type  t;

                        ;
                        ;
                        _cake_zmem(&t, 68);
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
                            _cake_zmem(&t, 68);
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
        if (p_initializer_list_item != 0)
        {
            compiler_diagnostic(100, ctx, p_initializer_list_item->initializer->first_token, 0, "warning: excess elements in initializer");
        }
        if (compute_array_size)
        {
            current_object->type.num_of_elements = array_to_expand_max_index + 1;
            p_current_object_type->num_of_elements = array_to_expand_max_index + 1;
        }
        current_object->parent = parent_copy;
        type_destroy(&array_item_type);
    }
    else __L0: /*catch*/ 
    {
        return 1;
    }
    return 0;
}
