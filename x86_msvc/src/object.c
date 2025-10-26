/* Cake 0.12.26 x86_msvc */
struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct map_entry;

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

struct token;

struct function_specifier {
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

struct enumerator;

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct expression;

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
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

struct expression_statement;
struct simple_declaration;

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct generic_association;

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

struct label;

struct case_label_list {
    struct label * head;
    struct label * tail;
};

struct attribute_specifier_sequence;
struct primary_block;
struct jump_statement;

struct unlabeled_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct param;

struct param_list {
    unsigned char  is_var_args;
    unsigned char  is_void;
    struct param * head;
    struct param * tail;
};

struct struct_entry;

struct enum_specifier;
struct struct_or_union_specifier;
struct declarator;
struct init_declarator;
struct macro;

union __tag12 {
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
    union __tag12  data;
};

struct attribute_specifier;

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int msvc_declspec_flags;
    int attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

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

struct param {
    struct type  type;
    struct param * next;
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

struct parameter_declaration;

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct diagnostic_stack {
    int top_index;
    struct diagnostic stack[10];
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

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct label_list_item;

struct label_list {
    struct label_list_item * head;
    struct label_list_item * tail;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct diagnostic_id_stack;
struct try_statement;
struct defer_statement;
struct selection_statement;
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

union __tag11 {
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
    union __tag11  value;
    struct object * parent;
    struct object * p_ref;
    struct expression * p_init_expression;
    struct object_list  members;
    struct object * next;
};

struct argument_expression_list {
    struct argument_expression * head;
    struct argument_expression * tail;
};

struct compound_statement;

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

struct specifier_qualifier_list;
struct member_declarator_list;
struct static_assert_declaration;
struct pragma_declaration;

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct member_declaration * next;
};

struct designation;
struct initializer;

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct declaration_specifiers;

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
};

struct typeof_specifier;
struct declaration_specifier;

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

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct balanced_token;

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
};

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
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

struct storage_class_specifier {
    int flags;
    struct token * token;
};

struct attribute;

struct attribute_list {
    int attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
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

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

struct defer_list_item;

struct defer_list {
    struct defer_list_item * head;
    struct defer_list_item * tail;
};

struct condition;
struct secondary_block;

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

struct pointer;
struct direct_declarator;
struct flow_object;

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

struct designator;

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
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

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
    struct defer_list  defer_list;
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

struct enumerator {
    unsigned char  has_shared_ownership;
    struct token * token;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct expression * constant_expression_opt;
    struct enum_specifier * enum_specifier;
    struct enumerator * next;
    struct object  value;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

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

struct alignment_specifier {
    int flags;
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct macro;

struct block_item;

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

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

struct iteration_statement;

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
};

struct statement;

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct type_qualifier_list;

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * calling_convention;
    struct pointer * pointer;
};

struct type_specifier {
    int flags;
    struct token * token;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct typeof_specifier * typeof_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct atomic_type_specifier * atomic_type_specifier;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct parameter_type_list {
    unsigned char  is_var_args;
    unsigned char  is_void;
    struct parameter_list * parameter_list;
};

struct type_qualifier {
    int flags;
    struct token * token;
    struct type_qualifier * next;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
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

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct alignment_specifier * alignment_specifier;
    struct declaration_specifier * next;
};

struct init_declarator {
    unsigned char  has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct diagnostic_id_stack {
    int size;
    int stack[10];
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

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct attribute_specifier {
    int ack;
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct type_qualifier_list {
    int flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct objects {
    struct object ** items;
    int size;
    int capacity;
};

struct attribute_argument_clause;

struct attribute {
    int msvc_declspec_flags;
    int attributes_flags;
    struct attribute_argument_clause * attribute_argument_clause;
    struct token * attribute_token;
    struct attribute * next;
};

struct label_list_item {
    struct token * p_last_usage;
    struct token * p_defined;
    struct label_list_item * next;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
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

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence;
    struct expression * expression_opt;
};

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
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

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};

struct flow_object;


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
int get_num_of_bits(int target, int type);

long long object_type_get_signed_max(int target, int type)
{
    int bits;

    bits = get_num_of_bits(target, type);
    return (1LL << (bits - 1)) - 1;
}


unsigned long long object_type_get_unsigned_max(int target, int type)
{
    int bits;

    bits = get_num_of_bits(target, type);
    return (1ULL << bits) - 1;
}


unsigned char unsigned_long_long_sub(unsigned long long * result, unsigned long long a, unsigned long long b)
{
    *result = 0;
    if (a < b)
    {
        return 0;
    }
    *result = a - b;
    return 1;
}


unsigned char unsigned_long_long_mul(unsigned long long * result, unsigned long long a, unsigned long long b)
{
    *result = 0;
    if (b == 0)
    {
        *result = 0;
        return 1;
    }
    if (a > 18446744073709551615ULL / b)
    {
        return 0;
    }
    *result = a * b;
    return 1;
}


unsigned char unsigned_long_long_add(unsigned long long * result, unsigned long long a, unsigned long long b)
{
    *result = 0;
    if (a > 18446744073709551615ULL - b)
    {
        return 0;
    }
    *result = a + b;
    return 1;
}


unsigned char signed_long_long_sub(signed long long * result, signed long long a, signed long long b)
{
    *result = 0;
    if (a >= 0 && b >= 0)
    {
    }
    else
    {
        if (a < 0 && b < 0)
        {
        }
        else
        {
            if (a < 0)
            {
                if (a < -9223372036854775808LL + b)
                {
                    return 0;
                }
            }
            else
            {
                if (b == -9223372036854775808LL)
                {
                    return 0;
                }
                if (a > 9223372036854775807LL - (-b))
                {
                    return 0;
                }
            }
        }
    }
    *result = a - b;
    return 1;
}


unsigned char signed_long_long_add(signed long long * result, signed long long a, signed long long b)
{
    *result = 0;
    if (a >= 0 && b >= 0)
    {
        if (a > 9223372036854775807LL - b)
        {
            return 0;
        }
    }
    else
    {
        if (a < 0 && b < 0)
        {
            if (a == -9223372036854775808LL || b == -9223372036854775808LL)
            {
                return 0;
            }
            if (a < -9223372036854775808LL - b)
            {
                return 0;
            }
        }
        else
        {
        }
    }
    *result = a + b;
    return 1;
}


unsigned char signed_long_long_mul(signed long long * result, signed long long a, signed long long b)
{
    *result = 0;
    if (a > 0 && b > 0)
    {
        if (a > 9223372036854775807LL / b)
        {
            return 0;
        }
    }
    else
    {
        if (a < 0 && b < 0)
        {
            if (a == -9223372036854775808LL || b == -9223372036854775808LL)
            {
                return 0;
            }
            if (-a > 9223372036854775807LL / -b)
            {
                return 0;
            }
        }
        else
        {
            if (a == 0 || b == 0)
            {
                *result = 0;
                return 1;
            }
            if (b > 0)
            {
                if (a < -9223372036854775808LL / b)
                {
                    return 0;
                }
            }
            else
            {
                if (b < -9223372036854775808LL / a)
                {
                    return 0;
                }
            }
        }
    }
    *result = a * b;
    return 1;
}


void object_list_push(struct object_list * list, struct object * pnew)
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
    list->count++;
}


void object_swap(struct object * a, struct object * b)
{
    struct object  temp;

    temp = *a;
    *a = *b;
    *b = temp;
}


void type_destroy(struct type * p_type);
void free(void * ptr);
void object_delete(struct object * p);

void object_destroy(struct object * p)
{
    struct object * item;

    ;
    type_destroy(&p->type);
    free((void *)p->member_designator);
    item = p->members.head;
    while (item)
    {
        struct object * next;

        next = item->next;
        item->next = 0;
        object_delete(item);
        item = next;
    }
}


void object_delete(struct object * p)
{
    if (p)
    {
        object_destroy(p);
        free(p);
    }
}


struct object *object_get_referenced(struct object * p_object);

unsigned char object_has_constant_value(struct object * a)
{
    a = object_get_referenced(a);
    return a->state == 2;
}


struct platform *get_platform(int target);

struct object object_make_size_t(int target, unsigned long long value)
{
    struct object  r;
    unsigned long long bits;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = get_platform(target)->size_t_type;
    bits = get_num_of_bits(target, r.value_type);
    r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (bits)) - 1))));
    return r;
}


struct object object_make_nullptr(int target)
{
    struct object  r;
    unsigned long long bits;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = get_platform(target)->size_t_type;
    bits = get_num_of_bits(target, r.value_type);
    r.value.host_u_long_long = ((unsigned long long)(0ULL & ((unsigned long long)((1ULL << (bits)) - 1))));
    return r;
}


static unsigned char object_type_is_signed_integer(int type);

struct object object_make_char(int target, int value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = get_platform(target)->char_t_type;
    if (object_type_is_signed_integer(r.value_type))
    {
        r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->char_n_bits)) - 1))))) & (1ULL << ((get_platform(target)->char_n_bits) - 1))) ? (((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->char_n_bits)) - 1)))) | ~((unsigned long long)((1ULL << (get_platform(target)->char_n_bits)) - 1))) : (((long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->char_n_bits)) - 1))))) & ((unsigned long long)((1ULL << (get_platform(target)->char_n_bits)) - 1)))));
    }
    else
    {
        r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->char_n_bits)) - 1))));
    }
    return r;
}



static unsigned char object_type_is_signed_integer(int type)
{
    /*switch*/
    {
        int __v0 = type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            return 1;
            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            __L11: /*case 10*/ 
            __L12: /*case 11*/ 
            __L13: /*case 12*/ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return 0;
}
struct object object_make_wchar_t(int target, int value)
{
    struct object  r;
    unsigned long long bits;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = get_platform(target)->wchar_t_type;
    bits = get_num_of_bits(target, r.value_type);
    r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (bits)) - 1))));
    return r;
}


struct object object_make_bool(int target, unsigned char  value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = get_platform(target)->bool_type;
    if (object_type_is_signed_integer(r.value_type))
    {
        r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->bool_n_bits)) - 1))))) & (1ULL << ((get_platform(target)->bool_n_bits) - 1))) ? (((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->bool_n_bits)) - 1)))) | ~((unsigned long long)((1ULL << (get_platform(target)->bool_n_bits)) - 1))) : (((long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->bool_n_bits)) - 1))))) & ((unsigned long long)((1ULL << (get_platform(target)->bool_n_bits)) - 1)))));
    }
    else
    {
        r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->bool_n_bits)) - 1))));
    }
    return r;
}


static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;

int object_to_str(struct object * a, int n, char str[])
{
    str[0] = 0;
    a = object_get_referenced(a);
    /*switch*/
    {
        int __v0 = a->value_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            snprintf(str, n, "%lld", a->value.host_long_long);
            goto __L0; /* break */

            goto __L0; /* break */

            __L4: /*case 6*/ 
            snprintf(str, n, "%lldL", a->value.host_long_long);
            goto __L0; /* break */

            __L5: /*case 8*/ 
            snprintf(str, n, "%lldLL", a->value.host_long_long);
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            snprintf(str, n, "%lluU", a->value.host_u_long_long);
            goto __L0; /* break */

            __L9: /*case 7*/ 
            snprintf(str, n, "%lluUL", a->value.host_u_long_long);
            goto __L0; /* break */

            __L10: /*case 9*/ 
            snprintf(str, n, "%lluULL", a->value.host_u_long_long);
            goto __L0; /* break */

            __L11: /*case 10*/ 
            __L12: /*case 11*/ 
            {
                snprintf(str, n, "%ff", a->value.host_double);
            }
            goto __L0; /* break */

            __L13: /*case 12*/ 
            {
                snprintf(str, n, "%LfLF", a->value.long_double_val);
            }
            goto __L0; /* break */

        }
        __L0:;
    }
    return 0;
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

static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}
unsigned char object_is_true(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        int __v0 = a->value_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            return (a->value.host_long_long != 0);
            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            return (a->value.host_u_long_long != 0);
            __L11: /*case 10*/ 
            return (a->value.host_float != 0);
            __L12: /*case 11*/ 
            return (a->value.host_double != 0);
            __L13: /*case 12*/ 
            return (a->value.long_double_val != 0);
        }
        __L0:;
    }
    ;
    return 0;
}


struct object object_make_signed_char(signed char value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 0;
    r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(1)->char_n_bits)) - 1))))) & (1ULL << ((get_platform(1)->char_n_bits) - 1))) ? (((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(1)->char_n_bits)) - 1)))) | ~((unsigned long long)((1ULL << (get_platform(1)->char_n_bits)) - 1))) : (((long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(1)->char_n_bits)) - 1))))) & ((unsigned long long)((1ULL << (get_platform(1)->char_n_bits)) - 1)))));
    return r;
}


void object_increment_value(int target, struct object * a)
{
    /*switch*/
    {
        int __v0 = a->value_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            a->value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a->value.host_long_long + 1)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, a->value_type)) - 1))) ? (((long long)(((unsigned long long)(a->value.host_long_long + 1)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a->value.host_long_long + 1)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1)))));
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            a->value.host_u_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a->value.host_u_long_long + 1)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, a->value_type)) - 1))) ? (((long long)(((unsigned long long)(a->value.host_u_long_long + 1)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a->value.host_u_long_long + 1)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, a->value_type))) - 1)))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            a->value.host_float++;
            goto __L0; /* break */

            __L12: /*case 11*/ 
            a->value.host_double++;
            goto __L0; /* break */

            __L13: /*case 12*/ 
            a->value.long_double_val++;
            goto __L0; /* break */

        }
        __L0:;
    }
}


struct object object_make_unsigned_char(int target, unsigned char value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 1;
    r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->char_n_bits)) - 1))));
    return r;
}


struct object object_make_signed_short(signed short value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 2;
    r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(value)) & 65535ULL))) & 32768ULL) ? (((long long)(((unsigned long long)(value)) & ((unsigned long long)(65536ULL - 1)))) | ~((unsigned long long)(65536ULL - 1))) : (((long long)((long long)(((unsigned long long)(value)) & 65535ULL))) & 65535ULL)));
    return r;
}


static int to_unsigned(int t);

struct object object_make_uint8(int target, unsigned char value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.value_type = to_unsigned(get_platform(target)->int8_type);
    r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(value)) & 255ULL));
    return r;
}



static int to_unsigned(int t)
{
    /*switch*/
    {
        int __v0 = t;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            return 1;
            __L2: /*case 2*/ 
            return 3;
            __L3: /*case 4*/ 
            return 5;
            __L4: /*case 6*/ 
            return 7;
            __L5: /*case 8*/ 
            return 9;
        }
        __L0:;
    }
    ;
    return t;
}
struct object object_make_uint16(int target, unsigned short value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.value_type = to_unsigned(get_platform(target)->int16_type);
    r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(value)) & 65535ULL));
    return r;
}


struct object object_make_uint32(int target, unsigned int value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.value_type = to_unsigned(get_platform(target)->int32_type);
    r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(value)) & 4294967295ULL));
    return r;
}


struct object object_make_signed_int(int target, long long value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 4;
    r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->int_n_bits)) - 1))))) & (1ULL << ((get_platform(target)->int_n_bits) - 1))) ? (((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->int_n_bits)) - 1)))) | ~((unsigned long long)((1ULL << (get_platform(target)->int_n_bits)) - 1))) : (((long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->int_n_bits)) - 1))))) & ((unsigned long long)((1ULL << (get_platform(target)->int_n_bits)) - 1)))));
    return r;
}


struct object object_make_unsigned_int(int target, unsigned int value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 5;
    r.value.host_long_long = ((unsigned long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->int_n_bits)) - 1))));
    return r;
}


struct object object_make_signed_long(int target, signed long long value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 4;
    r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->long_n_bits)) - 1))))) & (1ULL << ((get_platform(target)->long_n_bits) - 1))) ? (((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->long_n_bits)) - 1)))) | ~((unsigned long long)((1ULL << (get_platform(target)->long_n_bits)) - 1))) : (((long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->long_n_bits)) - 1))))) & ((unsigned long long)((1ULL << (get_platform(target)->long_n_bits)) - 1)))));
    return r;
}


struct object object_make_unsigned_long(int target, unsigned long long value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 7;
    r.value.host_long_long = ((unsigned long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->long_n_bits)) - 1))));
    return r;
}


struct object object_make_signed_long_long(int target, signed long long value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 8;
    r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->long_long_n_bits)) - 1))))) & (1ULL << ((get_platform(target)->long_long_n_bits) - 1))) ? (((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->long_long_n_bits)) - 1)))) | ~((unsigned long long)((1ULL << (get_platform(target)->long_long_n_bits)) - 1))) : (((long long)((long long)(((unsigned long long)(value)) & ((unsigned long long)((1ULL << (get_platform(target)->long_long_n_bits)) - 1))))) & ((unsigned long long)((1ULL << (get_platform(target)->long_long_n_bits)) - 1)))));
    return r;
}


signed long long object_to_signed_long_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        int __v0 = a->value_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            return a->value.host_long_long;
            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            return a->value.host_u_long_long;
            __L11: /*case 10*/ 
            return a->value.host_float;
            __L12: /*case 11*/ 
            return a->value.host_double;
            __L13: /*case 12*/ 
            return a->value.long_double_val;
        }
        __L0:;
    }
    ;
    return 0;
}


struct object object_make_unsigned_long_long(int target, unsigned long long value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 9;
    r.value.host_u_long_long = value;
    return r;
}


unsigned long long object_to_unsigned_long_long(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        int __v0 = a->value_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            return a->value.host_long_long;
            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            return a->value.host_u_long_long;
            __L11: /*case 10*/ 
            return a->value.host_float;
            __L12: /*case 11*/ 
            return a->value.host_double;
            __L13: /*case 12*/ 
            return a->value.long_double_val;
        }
        __L0:;
    }
    ;
    return 0;
}


struct object object_make_float(float value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 10;
    r.value.host_float = value;
    return r;
}


struct object object_make_double(double value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 11;
    r.value.host_double = value;
    return r;
}


struct object *object_get_non_const_referenced(struct object * p_object);

struct object object_make_pointer(struct object * object)
{
    struct object  r;

    object = object_get_non_const_referenced(object);
    _cake_zmem(&r, 120);
    r.state = 4;
    r.value_type = 9;
    r.value.host_u_long_long = (unsigned long long)object;
    return r;
}


struct object object_make_reference(struct object * object)
{
    struct object  r;

    object = object_get_non_const_referenced(object);
    _cake_zmem(&r, 120);
    r.state = 4;
    r.value_type = 9;
    r.value.host_u_long_long = (unsigned long long)object;
    r.p_ref = object;
    return r;
}


struct object object_make_long_double(long double value)
{
    struct object  r;

    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = 12;
    r.value.long_double_val = value;
    return r;
}


static unsigned char object_type_is_unsigned_integer(int type);

struct object object_cast(int target, int dest_type, struct object * v)
{
    int source_type;
    struct object  r;
    int dest_n_bits;

    v = object_get_referenced(v);
    if (v->value_type == dest_type)
    {
        return *v;
    }
    source_type = v->value_type;
    _cake_zmem(&r, 120);
    r.state = 2;
    r.value_type = dest_type;
    dest_n_bits = get_num_of_bits(target, dest_type);
    if (object_type_is_signed_integer(source_type))
    {
        if (object_type_is_signed_integer(dest_type))
        {
            r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(v->value.host_long_long)) & ((unsigned long long)((1ULL << (dest_n_bits)) - 1))))) & (1ULL << ((dest_n_bits) - 1))) ? (((long long)(((unsigned long long)(v->value.host_long_long)) & ((unsigned long long)((1ULL << (dest_n_bits)) - 1)))) | ~((unsigned long long)((1ULL << (dest_n_bits)) - 1))) : (((long long)((long long)(((unsigned long long)(v->value.host_long_long)) & ((unsigned long long)((1ULL << (dest_n_bits)) - 1))))) & ((unsigned long long)((1ULL << (dest_n_bits)) - 1)))));
        }
        else
        {
            if (object_type_is_unsigned_integer(dest_type))
            {
                r.value.host_long_long = ((unsigned long long)(((unsigned long long)(v->value.host_long_long)) & ((unsigned long long)((1ULL << (dest_n_bits)) - 1))));
            }
            else
            {
                if (dest_type == 10)
                {
                    r.value.host_float = (float)v->value.host_long_long;
                }
                else
                {
                    if (dest_type == 11)
                    {
                        r.value.host_double = (double)v->value.host_long_long;
                    }
                    else
                    {
                        if (dest_type == 12)
                        {
                            r.value.long_double_val = (long double)v->value.host_long_long;
                        }
                        else
                        {
                            ;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (object_type_is_unsigned_integer(source_type))
        {
            if (object_type_is_signed_integer(dest_type))
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(v->value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))))) & (1ULL << ((get_num_of_bits(1, dest_type)) - 1))) ? (((long long)(((unsigned long long)(v->value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))) : (((long long)((long long)(((unsigned long long)(v->value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1)))));
            }
            else
            {
                if (object_type_is_unsigned_integer(dest_type))
                {
                    r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(v->value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))));
                }
                else
                {
                    if (dest_type == 10)
                    {
                        r.value.host_float = (float)v->value.host_u_long_long;
                    }
                    else
                    {
                        if (dest_type == 11)
                        {
                            r.value.host_double = (double)v->value.host_u_long_long;
                        }
                        else
                        {
                            if (dest_type == 12)
                            {
                                r.value.long_double_val = (long double)v->value.host_u_long_long;
                            }
                            else
                            {
                                ;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if (source_type == 10)
            {
                if (object_type_is_signed_integer(dest_type))
                {
                    r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)((long long)v->value.host_float)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))))) & (1ULL << ((get_num_of_bits(1, dest_type)) - 1))) ? (((long long)(((unsigned long long)((long long)v->value.host_float)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))) : (((long long)((long long)(((unsigned long long)((long long)v->value.host_float)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1)))));
                }
                else
                {
                    if (object_type_is_unsigned_integer(dest_type))
                    {
                        r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)((long long)v->value.host_float)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))));
                    }
                    else
                    {
                        if (dest_type == 10)
                        {
                            r.value.host_float = (float)v->value.host_float;
                        }
                        else
                        {
                            if (dest_type == 11)
                            {
                                r.value.host_double = v->value.host_float;
                            }
                            else
                            {
                                if (dest_type == 12)
                                {
                                    r.value.long_double_val = (long double)v->value.host_float;
                                }
                                else
                                {
                                    ;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (source_type == 11)
                {
                    if (object_type_is_signed_integer(dest_type))
                    {
                        r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)((long long)v->value.host_double)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))))) & (1ULL << ((get_num_of_bits(1, dest_type)) - 1))) ? (((long long)(((unsigned long long)((long long)v->value.host_double)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))) : (((long long)((long long)(((unsigned long long)((long long)v->value.host_double)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1)))));
                    }
                    else
                    {
                        if (object_type_is_unsigned_integer(dest_type))
                        {
                            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)((long long)v->value.host_double)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))));
                        }
                        else
                        {
                            if (dest_type == 10)
                            {
                                r.value.host_float = (float)v->value.host_double;
                            }
                            else
                            {
                                if (dest_type == 11)
                                {
                                    r.value.host_double = (double)v->value.host_double;
                                }
                                else
                                {
                                    if (dest_type == 12)
                                    {
                                        r.value.long_double_val = (long double)v->value.host_double;
                                    }
                                    else
                                    {
                                        ;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (source_type == 12)
                    {
                        if (object_type_is_signed_integer(dest_type))
                        {
                            r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)((long long)v->value.long_double_val)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))))) & (1ULL << ((get_num_of_bits(1, dest_type)) - 1))) ? (((long long)(((unsigned long long)((long long)v->value.long_double_val)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))) : (((long long)((long long)(((unsigned long long)((long long)v->value.long_double_val)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1)))));
                        }
                        else
                        {
                            if (object_type_is_unsigned_integer(dest_type))
                            {
                                r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)((long long)v->value.long_double_val)) & ((unsigned long long)((1ULL << (get_num_of_bits(1, dest_type))) - 1))));
                            }
                            else
                            {
                                if (dest_type == 10)
                                {
                                    r.value.host_float = (float)v->value.long_double_val;
                                }
                                else
                                {
                                    if (dest_type == 11)
                                    {
                                        r.value.host_double = (double)v->value.long_double_val;
                                    }
                                    else
                                    {
                                        if (dest_type == 12)
                                        {
                                            r.value.long_double_val = (long double)v->value.long_double_val;
                                        }
                                        else
                                        {
                                            ;
                                        }
                                    }
                                }
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
    return r;
}



static unsigned char object_type_is_unsigned_integer(int type)
{
    /*switch*/
    {
        int __v0 = type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            return 1;
            __L11: /*case 10*/ 
            __L12: /*case 11*/ 
            __L13: /*case 12*/ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return 0;
}
unsigned char type_is_union(struct type * p_type);

void object_default_initialization(struct object * p_object, unsigned char  is_constant)
{
    if (p_object->members.head == 0)
    {
        if (is_constant)
        {
            p_object->state = 2;
        }
        else
        {
            p_object->state = 4;
        }
        p_object->value.host_u_long_long = 0;
    }
    if (type_is_union(&p_object->type))
    {
        struct object * p;

        p = p_object->members.head;
        if (p)
        {
            object_default_initialization(p, is_constant);
        }
    }
    else
    {
        struct object * p;

        p = p_object->members.head;
        while (p)
        {
            object_default_initialization(p, is_constant);
            p = p->next;
        }
    }
}


struct object *object_get_non_const_referenced(struct object * p_object)
{
    if (p_object->p_ref != 0)
    {
        p_object = p_object->p_ref;
    }
    return p_object;
}


struct object *object_get_referenced(struct object * p_object)
{
    if (p_object->p_ref != 0)
    {
        p_object = p_object->p_ref;
    }
    return p_object;
}


int get_rank(int t)
{
    if (t == 8 || t == 9)
    {
        return 80;
    }
    else
    {
        if (t == 4 || t == 5)
        {
            return 40;
        }
        else
        {
            if (t == 2 || t == 3)
            {
                return 30;
            }
            else
            {
                if (t == 0 || t == 1)
                {
                    return 20;
                }
            }
        }
    }
    return 0;
}


int get_size(int t)
{
    if (t == 8 || t == 9)
    {
        return 8;
    }
    else
    {
        if (t == 4 || t == 5)
        {
            return 4;
        }
        else
        {
            if (t == 2 || t == 3)
            {
                return 2;
            }
            else
            {
                if (t == 0 || t == 1)
                {
                    return 1;
                }
            }
        }
    }
    return 1;
}


unsigned char is_signed(int t)
{
    /*switch*/
    {
        int __v0 = t;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 11) goto __L6; /*case 11*/
        if (__v0 == 12) goto __L7; /*case 12*/
        goto __L8; /* default */

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            __L6: /*case 11*/ 
            return 1;
            __L7: /*case 12*/ 
            return 1;
            __L8: /* default */ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return 0;
}


unsigned char is_unsigned(int t)
{
    /*switch*/
    {
        int __v0 = t;
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 3) goto __L2; /*case 3*/
        if (__v0 == 5) goto __L3; /*case 5*/
        if (__v0 == 7) goto __L4; /*case 7*/
        if (__v0 == 9) goto __L5; /*case 9*/
        goto __L6; /* default */

        {
            __L1: /*case 1*/ 
            __L2: /*case 3*/ 
            __L3: /*case 5*/ 
            __L4: /*case 7*/ 
            __L5: /*case 9*/ 
            return 1;
            __L6: /* default */ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return 0;
}


void object_set_any(struct object * p_object)
{
    struct object * p;

    p_object = object_get_non_const_referenced(p_object);
    p_object->state = 1;
    p = p_object->members.head;
    while (p)
    {
        object_set_any(p);
        p = p->next;
    }
}


unsigned char object_is_zero(struct object * p_object)
{
    p_object = (struct object *)object_get_referenced(p_object);
    if (!object_has_constant_value(p_object))
    {
        return 0;
    }
    /*switch*/
    {
        int __v0 = p_object->value_type;
        if (__v0 == 0) goto __L2; /*case 0*/
        if (__v0 == 2) goto __L3; /*case 2*/
        if (__v0 == 4) goto __L4; /*case 4*/
        if (__v0 == 6) goto __L5; /*case 6*/
        if (__v0 == 8) goto __L6; /*case 8*/
        if (__v0 == 1) goto __L7; /*case 1*/
        if (__v0 == 3) goto __L8; /*case 3*/
        if (__v0 == 5) goto __L9; /*case 5*/
        if (__v0 == 7) goto __L10; /*case 7*/
        if (__v0 == 9) goto __L11; /*case 9*/
        if (__v0 == 10) goto __L12; /*case 10*/
        if (__v0 == 11) goto __L13; /*case 11*/
        if (__v0 == 12) goto __L14; /*case 12*/
        goto __L1;

        {
            __L2: /*case 0*/ 
            __L3: /*case 2*/ 
            __L4: /*case 4*/ 
            __L5: /*case 6*/ 
            __L6: /*case 8*/ 
            return p_object->value.host_long_long == 0;
            __L7: /*case 1*/ 
            __L8: /*case 3*/ 
            __L9: /*case 5*/ 
            __L10: /*case 7*/ 
            __L11: /*case 9*/ 
            return p_object->value.host_u_long_long == 0;
            __L12: /*case 10*/ 
            return p_object->value.host_float == 0.0;
            __L13: /*case 11*/ 
            return p_object->value.host_double == 0.0;
            __L14: /*case 12*/ 
            return p_object->value.long_double_val == 0.0;
        }
        __L1:;
    }
    ;
    return 0;
}


unsigned char object_is_one(struct object * p_object)
{
    p_object = (struct object *)object_get_referenced(p_object);
    if (!object_has_constant_value(p_object))
    {
        return 0;
    }
    /*switch*/
    {
        int __v0 = p_object->value_type;
        if (__v0 == 0) goto __L2; /*case 0*/
        if (__v0 == 2) goto __L3; /*case 2*/
        if (__v0 == 4) goto __L4; /*case 4*/
        if (__v0 == 6) goto __L5; /*case 6*/
        if (__v0 == 8) goto __L6; /*case 8*/
        if (__v0 == 1) goto __L7; /*case 1*/
        if (__v0 == 3) goto __L8; /*case 3*/
        if (__v0 == 5) goto __L9; /*case 5*/
        if (__v0 == 7) goto __L10; /*case 7*/
        if (__v0 == 9) goto __L11; /*case 9*/
        if (__v0 == 10) goto __L12; /*case 10*/
        if (__v0 == 11) goto __L13; /*case 11*/
        if (__v0 == 12) goto __L14; /*case 12*/
        goto __L1;

        {
            __L2: /*case 0*/ 
            __L3: /*case 2*/ 
            __L4: /*case 4*/ 
            __L5: /*case 6*/ 
            __L6: /*case 8*/ 
            return p_object->value.host_long_long == 1;
            __L7: /*case 1*/ 
            __L8: /*case 3*/ 
            __L9: /*case 5*/ 
            __L10: /*case 7*/ 
            __L11: /*case 9*/ 
            return p_object->value.host_u_long_long == 1;
            __L12: /*case 10*/ 
            return p_object->value.host_float == 1.0;
            __L13: /*case 11*/ 
            return p_object->value.host_double == 1.0;
            __L14: /*case 12*/ 
            return p_object->value.long_double_val == 1.0;
        }
        __L1:;
    }
    ;
    return 0;
}


unsigned char object_is_signed(struct object * p_object)
{
    p_object = (struct object *)object_get_referenced(p_object);
    return is_signed(p_object->value_type);
}


unsigned char object_is_derived(struct object * p_object)
{
    if (p_object->p_ref != 0)
    {
        return 0;
    }
    return p_object->members.head != 0;
}


unsigned char object_is_reference(struct object * p_object)
{
    return p_object->p_ref != 0;
}


struct object *object_get_member(struct object * p_object, unsigned int index)
{
    struct object * it;
    unsigned int count;

    p_object = (struct object *)object_get_referenced(p_object);
    if (p_object->members.head == 0)
    {
        return 0;
    }
    it = p_object->members.head;
    count = 0;
    while (it)
    {
        if (index == count)
        {
            return it;
        }
        count++;
        it = it->next;
    }
    return 0;
}


unsigned char type_is_pointer_or_array(struct type * p_type);
unsigned char type_is_function(struct type * p_type);
unsigned char compiler_diagnostic(int w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);

int object_set(struct parser_ctx * ctx, struct object * to, struct expression * p_init_expression, struct object * from, unsigned char  is_constant, unsigned char  requires_constant_initialization)
{
    if (1) /*try*/
    {
        from = object_get_referenced(from);
        to->p_init_expression = p_init_expression;
        if (object_is_derived(to))
        {
            struct object * it_to;
            struct object * it_from;

            it_to = to->members.head;
            it_from = from->members.head;
            while (it_from && it_to)
            {
                object_set(ctx, it_to, 0, it_from, is_constant, requires_constant_initialization);
                it_to = it_to->next;
                it_from = it_from->next;
            }
            if (it_from != 0 || it_to != 0)
            {
            }
        }
        else
        {
            ;
            to->state = from->state;
            to->value = object_cast(ctx->options.target, to->value_type, from).value;
            if (requires_constant_initialization && !object_has_constant_value(from))
            {
                if (!type_is_pointer_or_array(&p_init_expression->type) && !type_is_function(&p_init_expression->type))
                {
                    struct token * tk;

                    tk = p_init_expression ? p_init_expression->first_token : ctx->current;
                    compiler_diagnostic(1860, ctx, tk, 0, "requires a compile time object");
                    goto __L0; /* throw */
                }
            }
            if (is_constant && from->state == 2)
            {
                to->state = 2;
            }
            else
            {
                if (to->state == 2)
                {
                    to->state = 4;
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


void *calloc(unsigned int nmemb, unsigned int size);
char *strdup(char * src);
struct type type_dup(struct type * p_type);
struct type get_array_item_type(struct type * p_type);
int type_to_object_type(struct type * type, int target);
struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);

struct object *make_object_ptr_core(struct type * p_type, char * member_designator, int target)
{
    struct object * p_object;

    p_object = 0;
    if (1) /*try*/
    {
        struct struct_or_union_specifier * p_struct_or_union_specifier;
        struct member_declaration * p_member_declaration;

        if (p_type->category == 1)
        {
            p_object = calloc(1, 120);
            if (p_object == 0)
            {
                goto __L0; /* throw */
            }
            p_object->member_designator = strdup(member_designator);
            p_object->type = type_dup(p_type);
            return p_object;
        }
        if (p_type->category == 3)
        {
            p_object = calloc(1, 120);
            if (p_object == 0)
            {
                goto __L0; /* throw */
            }
            *p_object = object_make_nullptr(target);
            p_object->state = 0;
            ;
            p_object->member_designator = strdup(member_designator);
            type_destroy(&p_object->type);
            p_object->type = type_dup(p_type);
            return p_object;
        }
        if (p_type->category == 2)
        {
            p_object = calloc(1, 120);
            if (p_object == 0)
            {
                goto __L0; /* throw */
            }
            p_object->type = type_dup(p_type);
            p_object->member_designator = strdup(member_designator);
            if (p_type->num_of_elements > 0)
            {
                struct type  array_item_type;
                unsigned long long max_elements;

                array_item_type = get_array_item_type(p_type);
                max_elements = p_type->num_of_elements > 1000 ? 1000 : p_type->num_of_elements;
                {
                    unsigned long long i;

                    i = 0;
                    for (; i < max_elements; i++)
                    {
                        char buffer[200];
                        struct object * p_member_obj;

                        _cake_zmem(&buffer, 200);
                        snprintf(buffer, 200, "%s[%llu]", member_designator, i);
                        p_member_obj = make_object_ptr_core(&array_item_type, buffer, target);
                        if (p_member_obj == 0)
                        {
                            type_destroy(&array_item_type);
                            goto __L0; /* throw */
                        }
                        p_member_obj->parent = p_object;
                        free((void *)p_member_obj->member_designator);
                        p_member_obj->member_designator = strdup(buffer);
                        object_list_push(&p_object->members, p_member_obj);
                    }
                }
                type_destroy(&array_item_type);
            }
            return p_object;
        }
        if (p_type->struct_or_union_specifier == 0)
        {
            p_object = calloc(1, 120);
            if (p_object == 0)
            {
                goto __L0; /* throw */
            }
            p_object->state = 0;
            p_object->value_type = type_to_object_type(p_type, target);
            p_object->value.host_long_long = -1;
            p_object->member_designator = strdup(member_designator);
            p_object->type = type_dup(p_type);
            return p_object;
        }
        p_struct_or_union_specifier = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
        if (p_struct_or_union_specifier == 0)
        {
            goto __L0; /* throw */
        }
        p_object = calloc(1, 120);
        if (p_object == 0)
        {
            goto __L0; /* throw */
        }
        p_object->member_designator = strdup(member_designator);
        p_object->type = type_dup(p_type);
        p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;
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
                        char buffer[200];
                        struct object * p_member_obj;

                        _cake_zmem(&buffer, 200);
                        snprintf(buffer, 200, "%s.%s", member_designator, p_member_declarator->declarator->name_opt->lexeme);
                        p_member_obj = make_object_ptr_core(&p_member_declarator->declarator->type, buffer, target);
                        if (p_member_obj == 0)
                        {
                            goto __L0; /* throw */
                        }
                        p_member_obj->parent = p_object;
                        free((void *)p_member_obj->member_designator);
                        p_member_obj->member_designator = strdup(buffer);
                        object_list_push(&p_object->members, p_member_obj);
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else
            {
                if (p_member_declaration->specifier_qualifier_list != 0)
                {
                    if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type  t;
                        char buffer[200];
                        struct object * p_member_obj;

                        _cake_zmem(&t, 68);
                        t.category = 0;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = 32768;
                        _cake_zmem(&buffer, 200);
                        snprintf(buffer, 200, ".%s", member_designator);
                        p_member_obj = make_object_ptr_core(&t, buffer, target);
                        if (p_member_obj == 0)
                        {
                            goto __L0; /* throw */
                        }
                        free((void *)p_member_obj->member_designator);
                        p_member_obj->member_designator = strdup(buffer);
                        p_member_obj->parent = p_object;
                        object_list_push(&p_object->members, p_member_obj);
                        type_destroy(&t);
                    }
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return p_object;
    }
    else __L0: /*catch*/ 
    {
        object_delete(p_object);
        p_object = 0;
    }
    return 0;
}


struct object *make_object_ptr(struct type * p_type, int target)
{
    return make_object_ptr_core(p_type, "", target);
}


static void object_fix_parent(struct object * p_object, struct object * parent);

int make_object_with_member_designator(struct type * p_type, struct object * obj, char * name, int target)
{
    struct object * p;

    p = make_object_ptr_core(p_type, name, target);
    if (p)
    {
        *obj = *p;
        object_fix_parent(obj, obj);
        free(p);
        return 0;
    }
    return 1;
}



static void object_fix_parent(struct object * p_object, struct object * parent)
{
    struct object * it;

    it = p_object->members.head;
    while (it)
    {
        it->parent = parent;
        it = it->next;
    }
}
struct object object_dup(struct object * src)
{
    struct object  result;

    ;
    result = *src;
    result.type = type_dup(&src->type);
    if (src->member_designator)
    {
        result.member_designator = strdup(src->member_designator);
    }
    result.next = 0;
    return result;
}


int make_object(struct type * p_type, struct object * obj, int target)
{
    return make_object_with_member_designator(p_type, obj, "", target);
}


int object_type_to_type_specifier(int type)
{
    /*switch*/
    {
        int __v0 = type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 1) goto __L2; /*case 1*/
        if (__v0 == 2) goto __L3; /*case 2*/
        if (__v0 == 3) goto __L4; /*case 3*/
        if (__v0 == 4) goto __L5; /*case 4*/
        if (__v0 == 5) goto __L6; /*case 5*/
        if (__v0 == 6) goto __L7; /*case 6*/
        if (__v0 == 7) goto __L8; /*case 7*/
        if (__v0 == 8) goto __L9; /*case 8*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            return 130;
            __L2: /*case 1*/ 
            return 258;
            __L3: /*case 2*/ 
            return 4;
            __L4: /*case 3*/ 
            return 260;
            __L5: /*case 4*/ 
            return 8;
            __L6: /*case 5*/ 
            return 264;
            __L7: /*case 6*/ 
            return 16;
            __L8: /*case 7*/ 
            return 272;
            __L9: /*case 8*/ 
            return 4194304;
            __L10: /*case 9*/ 
            return 4194560;
            __L11: /*case 10*/ 
            return 32;
            __L12: /*case 11*/ 
            return 64;
            __L13: /*case 12*/ 
            return 80;
        }
        __L0:;
    }
    ;
    return 0;
}


int type_specifier_to_object_type(int type_specifier_flags, int target)
{
    if (type_specifier_flags & 512)
    {
        return get_platform(target)->bool_type;
    }
    if (type_specifier_flags & 32)
    {
        return 10;
    }
    if (type_specifier_flags & 64)
    {
        if (type_specifier_flags & 16)
        {
            return 12;
        }
        return 11;
    }
    if (type_specifier_flags & 256)
    {
        if (type_specifier_flags & 2)
        {
            return 1;
        }
        if (type_specifier_flags & 4)
        {
            return 3;
        }
        if (type_specifier_flags & 16)
        {
            return 7;
        }
        if (type_specifier_flags & 8)
        {
            return 5;
        }
        if (type_specifier_flags & 4194304)
        {
            return 9;
        }
    }
    else
    {
        if (type_specifier_flags & 2)
        {
            return 0;
        }
        if (type_specifier_flags & 4)
        {
            return 2;
        }
        if (type_specifier_flags & 16)
        {
            return 6;
        }
        if (type_specifier_flags & 8)
        {
            return 4;
        }
        if (type_specifier_flags & 4194304)
        {
            return 8;
        }
    }
    return 4;
}


unsigned char type_is_pointer(struct type * p_type);

int type_to_object_type(struct type * type, int target)
{
    if (type_is_pointer(type))
    {
        return get_platform(target)->size_t_type;
    }
    return type_specifier_to_object_type(type->type_specifier_flags, target);
}


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void object_print_value_debug(struct object * a)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        int __v0 = a->value_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 1) goto __L2; /*case 1*/
        if (__v0 == 2) goto __L3; /*case 2*/
        if (__v0 == 3) goto __L4; /*case 3*/
        if (__v0 == 4) goto __L5; /*case 4*/
        if (__v0 == 5) goto __L6; /*case 5*/
        if (__v0 == 8) goto __L7; /*case 8*/
        if (__v0 == 9) goto __L8; /*case 9*/
        if (__v0 == 10) goto __L9; /*case 10*/
        if (__v0 == 11) goto __L10; /*case 11*/
        if (__v0 == 12) goto __L11; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            printf("%lld (signed char)", a->value.host_long_long);
            goto __L0; /* break */

            __L2: /*case 1*/ 
            printf("%llu (unsigned char)", a->value.host_u_long_long);
            goto __L0; /* break */

            __L3: /*case 2*/ 
            printf("%lld (short)", a->value.host_long_long);
            goto __L0; /* break */

            __L4: /*case 3*/ 
            printf("%llu (unsigned short)", a->value.host_u_long_long);
            goto __L0; /* break */

            __L5: /*case 4*/ 
            printf("%lld (int)", a->value.host_long_long);
            goto __L0; /* break */

            __L6: /*case 5*/ 
            printf("%llu (unsigned int)", a->value.host_u_long_long);
            goto __L0; /* break */

            __L7: /*case 8*/ 
            printf("%lld (long long)", a->value.host_long_long);
            goto __L0; /* break */

            __L8: /*case 9*/ 
            printf("%llu (unsigned long long)", a->value.host_u_long_long);
            goto __L0; /* break */

            __L9: /*case 10*/ 
            printf("%f (float)", a->value.host_float);
            goto __L0; /* break */

            __L10: /*case 11*/ 
            printf("%lf (double)", a->value.host_double);
            goto __L0; /* break */

            __L11: /*case 12*/ 
            printf("%Lf (long double)", a->value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
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
void type_print(struct type * a, int target);

void object_print_to_debug_core(struct object * object, int n, int target)
{
    if (object_is_reference(object))
    {
        object = object_get_referenced(object);
    }
    {
        int i;

        i = 0;
        for (; i < n; i++)
        printf("  ");
    }
    if (object->member_designator)
    {
        printf("%s ", object->member_designator);
    }
    if (object->members.head != 0)
    {
        struct object * member;

        type_print(&object->type, target);
        printf(" {\n");
        member = object->members.head;
        while (member)
        {
            object_print_to_debug_core(member, n + 1, target);
            member = member->next;
        }
        {
            int i;

            i = 0;
            for (; i < n; i++)
            printf("  ");
        }
        printf("}\n");
    }
    else
    {
        type_print(&object->type, target);
        printf(" = ");
        object_print_value_debug(object);
        /*switch*/
        {
            int __v0 = object->state;
            if (__v0 == 0) goto __L4; /*case 0*/
            if (__v0 == 1) goto __L5; /*case 1*/
            if (__v0 == 4) goto __L6; /*case 4*/
            if (__v0 == 2) goto __L7; /*case 2*/
            goto __L3;

            {
                __L4: /*case 0*/ 
                printf(" uninitialized ");
                goto __L3; /* break */

                __L5: /*case 1*/ 
                printf(" unknown ");
                goto __L3; /* break */

                __L6: /*case 4*/ 
                printf(" exact ");
                goto __L3; /* break */

                __L7: /*case 2*/ 
                printf(" constant_exact ");
                goto __L3; /* break */

            }
            __L3:;
        }
        printf("\n");
    }
}


void object_print_to_debug(struct object * object, int target)
{
    int n;

    n = 0;
    object_print_to_debug_core(object, n, target);
}


struct object *object_extend_array_to_index(struct type * p_type, struct object * a, unsigned int max_index, unsigned char  is_constant, int target)
{
    if (1) /*try*/
    {
        {
            unsigned int count;

            count = a->members.count;
            for (; count < (max_index + 1); count++)
            {
                char name[50];
                struct object * p;

                _cake_zmem(&name, 50);
                snprintf(name, 50, "[%zu]", count);
                p = make_object_ptr_core(p_type, name, target);
                if (p == 0)
                {
                    goto __L0; /* throw */
                }
                p->parent = a;
                object_default_initialization(p, is_constant);
                object_list_push(&a->members, p);
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
    return a->members.tail;
}


unsigned char object_is_promoted(struct object * a)
{
    if ((a->value_type == 0) || (a->value_type == 1) || (a->value_type == 2) || a->value_type == 3)
    {
        return 1;
    }
    return 0;
}


int object_common(struct object * a, struct object * b)
{
    int a_type;
    int b_type;
    int signed_promoted;
    int unsigned_promoted;

    a_type = a->value_type;
    b_type = b->value_type;
    if (a_type == 12 || b_type == 12)
    {
        return 12;
    }
    if (a_type == 11 || b_type == 11)
    {
        return 11;
    }
    if (a_type == 10 || b_type == 10)
    {
        return 10;
    }
    if (object_is_promoted(a))
    {
        a_type = 4;
    }
    if (object_is_promoted(b))
    {
        b_type = 4;
    }
    if (a_type == b_type)
    {
        return a_type;
    }
    if (is_signed(a_type) == is_signed(b_type))
    {
        if (get_rank(a_type) > get_rank(b_type))
        {
            return a_type;
        }
        return b_type;
    }
    signed_promoted = is_signed(a_type) ? a_type : b_type;
    unsigned_promoted = is_unsigned(a_type) ? a_type : b_type;
    if (get_rank(unsigned_promoted) >= get_rank(signed_promoted))
    {
        return unsigned_promoted;
    }
    if (get_size(signed_promoted) > get_size(unsigned_promoted))
    {
        return signed_promoted;
    }
    return to_unsigned(signed_promoted);
}


void objects_destroy(struct objects * arr)
{
    free(arr->items);
}


void *malloc(unsigned int size);
void *realloc(void * ptr, unsigned int size);

int objects_push(struct objects * arr, struct object * obj)
{
    if (arr->items == 0)
    {
        arr->items = malloc(32);
        if (!arr->items)
        {
            arr->size = 0;
            arr->capacity = 0;
            return 12;
        }
        arr->size = 0;
        arr->capacity = 8;
    }
    if (arr->size == arr->capacity)
    {
        unsigned int new_capacity;
        struct object ** new_items;

        new_capacity = arr->capacity ? arr->capacity * 2 : 8;
        new_items = realloc(arr->items, new_capacity * 4);
        if (!new_items)
        {
            return 12;
        }
        arr->items = new_items;
        arr->capacity = (int)new_capacity;
    }
    arr->items[arr->size++] = obj;
    return 0;
}


int ss_fprintf(struct osstream * stream, char * fmt, ...);
short __cdecl _fdclass(float _X);
short __cdecl _dclass(double _X);
short __cdecl _ldclass(long double _X);

void object_print_value(struct osstream * ss, struct object * a, int target)
{
    a = object_get_referenced(a);
    /*switch*/
    {
        int __v0 = a->value_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 1) goto __L2; /*case 1*/
        if (__v0 == 2) goto __L3; /*case 2*/
        if (__v0 == 3) goto __L4; /*case 3*/
        if (__v0 == 4) goto __L5; /*case 4*/
        if (__v0 == 6) goto __L6; /*case 6*/
        if (__v0 == 7) goto __L7; /*case 7*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 8) goto __L9; /*case 8*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L13; /*case 11*/
        if (__v0 == 12) goto __L17; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            ss_fprintf(ss, "%lld", a->value.host_long_long);
            goto __L0; /* break */

            __L2: /*case 1*/ 
            ss_fprintf(ss, "%llu", a->value.host_u_long_long);
            goto __L0; /* break */

            __L3: /*case 2*/ 
            ss_fprintf(ss, "%lld", a->value.host_long_long);
            goto __L0; /* break */

            __L4: /*case 3*/ 
            ss_fprintf(ss, "%llu", a->value.host_u_long_long);
            goto __L0; /* break */

            __L5: /*case 4*/ 
            ss_fprintf(ss, "%lld", a->value.host_long_long);
            goto __L0; /* break */

            __L6: /*case 6*/ 
            ss_fprintf(ss, "%lldL", a->value.host_long_long);
            goto __L0; /* break */

            __L7: /*case 7*/ 
            ss_fprintf(ss, "%lluL", a->value.host_u_long_long);
            goto __L0; /* break */

            __L8: /*case 5*/ 
            ss_fprintf(ss, "%llu", a->value.host_u_long_long);
            goto __L0; /* break */

            __L9: /*case 8*/ 
            ss_fprintf(ss, "%lldLL", a->value.host_long_long);
            goto __L0; /* break */

            __L10: /*case 9*/ 
            ss_fprintf(ss, "%lluULL", a->value.host_u_long_long);
            goto __L0; /* break */

            __L11: /*case 10*/ 
            if (((1 ? _fdclass((float)(a->value.host_float)) : (4 == 4 ? 102 : 8 == 8 ? 100 : 108) == 100 ? _dclass((double)(a->value.host_float)) : _ldclass((long double)(a->value.host_float))) == 1))
            {
                ;
                ss_fprintf(ss, "%f", a->value.host_float);
            }
            else
            {
                ss_fprintf(ss, "%f", a->value.host_float);
            }
            ss_fprintf(ss, "f");
            goto __L0; /* break */

            __L13: /*case 11*/ 
            if (((0 ? _fdclass((float)(a->value.host_double)) : 1 ? _dclass((double)(a->value.host_double)) : _ldclass((long double)(a->value.host_double))) == 1))
            {
                ;
                ss_fprintf(ss, "%.17g", a->value.host_double);
            }
            else
            {
                char temp[64];
                char * p;
                unsigned char  dot_found;

                _cake_zmem(&temp, 64);
                snprintf(temp, 64, "%.17g", a->value.host_double);
                p = temp;
                dot_found = 0;
                while (*p)
                {
                    if (*p == 46)
                    {
                        dot_found = 1;
                        break;
                    }
                    p++;
                }
                if (!dot_found)
                {
                    *p = 46;
                    p++;
                    *p = 48;
                    p++;
                    *p = 0;
                }
                ss_fprintf(ss, "%s", temp);
            }
            goto __L0; /* break */

            __L17: /*case 12*/ 
            ss_fprintf(ss, "%Lf", a->value.host_double);
            ss_fprintf(ss, "L");
            goto __L0; /* break */

        }
        __L0:;
    }
}


struct object object_equal(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = 4;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long == b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long == b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long == b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long == b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            r.value.host_u_long_long = (a0.value.host_float == b0.value.host_float);
            goto __L0; /* break */

            __L12: /*case 11*/ 
            r.value.host_u_long_long = (a0.value.host_double == b0.value.host_double);
            goto __L0; /* break */

            __L13: /*case 12*/ 
            r.value.host_u_long_long = (a0.value.long_double_val == b0.value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_not_equal(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long != b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long != b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long != b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long != b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            r.value.host_u_long_long = (a0.value.host_float != b0.value.host_float);
            goto __L0; /* break */

            __L12: /*case 11*/ 
            r.value.host_u_long_long = (a0.value.host_double != b0.value.host_double);
            goto __L0; /* break */

            __L13: /*case 12*/ 
            r.value.host_u_long_long = (a0.value.long_double_val != b0.value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_greater_than_or_equal(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long >= b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long >= b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long >= b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long >= b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            r.value.host_u_long_long = (a0.value.host_float >= b0.value.host_float);
            goto __L0; /* break */

            __L12: /*case 11*/ 
            r.value.host_u_long_long = (a0.value.host_double >= b0.value.host_double);
            goto __L0; /* break */

            __L13: /*case 12*/ 
            r.value.host_u_long_long = (a0.value.long_double_val >= b0.value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_greater_than(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long > b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long > b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long > b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long > b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            r.value.host_u_long_long = (a0.value.host_float > b0.value.host_float);
            goto __L0; /* break */

            __L12: /*case 11*/ 
            r.value.host_u_long_long = (a0.value.host_double > b0.value.host_double);
            goto __L0; /* break */

            __L13: /*case 12*/ 
            r.value.host_u_long_long = (a0.value.long_double_val > b0.value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_smaller_than_or_equal(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long <= b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long <= b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long <= b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long <= b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            r.value.host_u_long_long = (a0.value.host_float <= b0.value.host_float);
            goto __L0; /* break */

            __L12: /*case 11*/ 
            r.value.host_u_long_long = (a0.value.host_double <= b0.value.host_double);
            goto __L0; /* break */

            __L13: /*case 12*/ 
            r.value.host_u_long_long = (a0.value.long_double_val <= b0.value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_smaller_than(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long < b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long < b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long < b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long < b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            r.value.host_u_long_long = (a0.value.host_float < b0.value.host_float);
            goto __L0; /* break */

            __L12: /*case 11*/ 
            r.value.host_u_long_long = (a0.value.host_double < b0.value.host_double);
            goto __L0; /* break */

            __L13: /*case 12*/ 
            r.value.host_u_long_long = (a0.value.long_double_val < b0.value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_add(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L8; /*case 1*/
        if (__v0 == 3) goto __L9; /*case 3*/
        if (__v0 == 5) goto __L10; /*case 5*/
        if (__v0 == 7) goto __L11; /*case 7*/
        if (__v0 == 9) goto __L12; /*case 9*/
        if (__v0 == 10) goto __L15; /*case 10*/
        if (__v0 == 11) goto __L16; /*case 11*/
        if (__v0 == 12) goto __L17; /*case 12*/
        goto __L0;

        {
            unsigned long long exact_result;

            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                signed long long exact_result;

                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long + b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long + b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long + b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
                if (signed_long_long_add(&exact_result, a0.value.host_long_long, b0.value.host_long_long))
                {
                    if (r.value.host_long_long != exact_result)
                    {
                        snprintf(warning_message, 200, "integer overflow results in '%lld'. The exact result is '%lld'.", r.value.host_long_long, exact_result);
                    }
                }
                else
                {
                    snprintf(warning_message, 200, "integer overflow");
                }
            }
            goto __L0; /* break */

            __L8: /*case 1*/ 
            __L9: /*case 3*/ 
            __L10: /*case 5*/ 
            __L11: /*case 7*/ 
            __L12: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long + b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            if (unsigned_long_long_add(&exact_result, a0.value.host_u_long_long, b0.value.host_u_long_long))
            {
                if (r.value.host_u_long_long != exact_result)
                {
                    snprintf(warning_message, 200, "integer wrap-around results in '%llu'. The exact result is '%llu'.", r.value.host_u_long_long, exact_result);
                }
            }
            else
            {
                snprintf(warning_message, 200, "integer wrap-around results in '%llu'. ", r.value.host_u_long_long);
            }
            goto __L0; /* break */

            __L15: /*case 10*/ 
            r.value.host_float = a0.value.host_float + b0.value.host_float;
            goto __L0; /* break */

            __L16: /*case 11*/ 
            r.value.host_double = a0.value.host_float + b0.value.host_double;
            goto __L0; /* break */

            __L17: /*case 12*/ 
            r.value.long_double_val = a0.value.long_double_val + b0.value.long_double_val;
        }
        __L0:;
    }
    return r;
}


struct object object_sub(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L8; /*case 1*/
        if (__v0 == 3) goto __L9; /*case 3*/
        if (__v0 == 5) goto __L10; /*case 5*/
        if (__v0 == 7) goto __L11; /*case 7*/
        if (__v0 == 9) goto __L12; /*case 9*/
        if (__v0 == 10) goto __L15; /*case 10*/
        if (__v0 == 11) goto __L16; /*case 11*/
        if (__v0 == 12) goto __L17; /*case 12*/
        goto __L0;

        {
            unsigned long long exact_result;

            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                signed long long exact_result;

                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long - b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long - b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long - b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
                if (signed_long_long_sub(&exact_result, a0.value.host_long_long, b0.value.host_long_long))
                {
                    if (r.value.host_long_long != exact_result)
                    {
                        snprintf(warning_message, 200, "integer overflow results in '%lld'. The exact result is '%lld'.", r.value.host_long_long, exact_result);
                    }
                }
                else
                {
                    snprintf(warning_message, 200, "integer overflow");
                }
            }
            goto __L0; /* break */

            __L8: /*case 1*/ 
            __L9: /*case 3*/ 
            __L10: /*case 5*/ 
            __L11: /*case 7*/ 
            __L12: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long - b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            if (unsigned_long_long_sub(&exact_result, a0.value.host_u_long_long, b0.value.host_u_long_long))
            {
                if (r.value.host_u_long_long != exact_result)
                {
                    snprintf(warning_message, 200, "integer wrap-around results in '%llu'. The exact result is '%llu'.", r.value.host_u_long_long, exact_result);
                }
            }
            else
            {
                snprintf(warning_message, 200, "integer wrap-around results in '%llu'. ", r.value.host_u_long_long);
            }
            goto __L0; /* break */

            __L15: /*case 10*/ 
            r.value.host_float = a0.value.host_float - b0.value.host_float;
            goto __L0; /* break */

            __L16: /*case 11*/ 
            r.value.host_double = a0.value.host_float - b0.value.host_double;
            goto __L0; /* break */

            __L17: /*case 12*/ 
            r.value.long_double_val = a0.value.long_double_val - b0.value.long_double_val;
        }
        __L0:;
    }
    return r;
}


struct object object_mul(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L8; /*case 1*/
        if (__v0 == 3) goto __L9; /*case 3*/
        if (__v0 == 5) goto __L10; /*case 5*/
        if (__v0 == 7) goto __L11; /*case 7*/
        if (__v0 == 9) goto __L12; /*case 9*/
        if (__v0 == 10) goto __L15; /*case 10*/
        if (__v0 == 11) goto __L16; /*case 11*/
        if (__v0 == 12) goto __L17; /*case 12*/
        goto __L0;

        {
            unsigned long long exact_result;

            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                signed long long exact_result;

                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long * b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long * b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long * b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
                if (signed_long_long_mul(&exact_result, a0.value.host_long_long, b0.value.host_long_long))
                {
                    if (r.value.host_long_long != exact_result)
                    {
                        snprintf(warning_message, 200, "integer overflow results in '%lld'. The exact result is '%lld'.", r.value.host_long_long, exact_result);
                    }
                }
                else
                {
                    snprintf(warning_message, 200, "integer overflow");
                }
            }
            goto __L0; /* break */

            __L8: /*case 1*/ 
            __L9: /*case 3*/ 
            __L10: /*case 5*/ 
            __L11: /*case 7*/ 
            __L12: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long * b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            if (unsigned_long_long_mul(&exact_result, a0.value.host_u_long_long, b0.value.host_u_long_long))
            {
                if (r.value.host_u_long_long != exact_result)
                {
                    snprintf(warning_message, 200, "integer wrap-around results in '%llu'. The exact result is '%llu'.", r.value.host_u_long_long, exact_result);
                }
            }
            else
            {
                snprintf(warning_message, 200, "integer wrap-around results in '%llu'. ", r.value.host_u_long_long);
            }
            goto __L0; /* break */

            __L15: /*case 10*/ 
            r.value.host_float = a0.value.host_float * b0.value.host_float;
            goto __L0; /* break */

            __L16: /*case 11*/ 
            r.value.host_double = a0.value.host_float * b0.value.host_double;
            goto __L0; /* break */

            __L17: /*case 12*/ 
            r.value.long_double_val = a0.value.long_double_val * b0.value.long_double_val;
        }
        __L0:;
    }
    return r;
}


struct object object_div(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L7; /*case 1*/
        if (__v0 == 3) goto __L8; /*case 3*/
        if (__v0 == 5) goto __L9; /*case 5*/
        if (__v0 == 7) goto __L10; /*case 7*/
        if (__v0 == 9) goto __L11; /*case 9*/
        if (__v0 == 10) goto __L13; /*case 10*/
        if (__v0 == 11) goto __L14; /*case 11*/
        if (__v0 == 12) goto __L15; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                if (b0.value.host_long_long == 0)
                {
                    snprintf(warning_message, 200, "division by zero");
                    return r;
                }
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long / b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long / b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long / b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L7: /*case 1*/ 
            __L8: /*case 3*/ 
            __L9: /*case 5*/ 
            __L10: /*case 7*/ 
            __L11: /*case 9*/ 
            if (b0.value.host_u_long_long == 0)
            {
                snprintf(warning_message, 200, "division by zero");
                return r;
            }
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long / b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L13: /*case 10*/ 
            r.value.host_float = a0.value.host_float / b0.value.host_float;
            goto __L0; /* break */

            __L14: /*case 11*/ 
            r.value.host_double = a0.value.host_float / b0.value.host_double;
            goto __L0; /* break */

            __L15: /*case 12*/ 
            r.value.long_double_val = a0.value.long_double_val / b0.value.long_double_val;
        }
        __L0:;
    }
    return r;
}


struct object object_mod(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L7; /*case 1*/
        if (__v0 == 3) goto __L8; /*case 3*/
        if (__v0 == 5) goto __L9; /*case 5*/
        if (__v0 == 7) goto __L10; /*case 7*/
        if (__v0 == 9) goto __L11; /*case 9*/
        if (__v0 == 10) goto __L13; /*case 10*/
        if (__v0 == 11) goto __L14; /*case 11*/
        if (__v0 == 12) goto __L15; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                if (b0.value.host_long_long == 0)
                {
                    snprintf(warning_message, 200, "division by zero");
                    return r;
                }
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long % b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long % b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long % b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L7: /*case 1*/ 
            __L8: /*case 3*/ 
            __L9: /*case 5*/ 
            __L10: /*case 7*/ 
            __L11: /*case 9*/ 
            if (b0.value.host_u_long_long == 0)
            {
                snprintf(warning_message, 200, "division by zero");
                return r;
            }
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long % b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L13: /*case 10*/ 
            __L14: /*case 11*/ 
            __L15: /*case 12*/ 
            ;
            snprintf(warning_message, 200, " invalid operands for");
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


int object_is_equal(int target, struct object * a, struct object * b)
{
    char message[200];
    struct object  r;

    r = object_equal(target, a, b, message);
    return r.value.host_long_long != 0;
}


int object_is_not_equal(int target, struct object * a, struct object * b)
{
    char message[200];
    struct object  r;

    r = object_not_equal(target, a, b, message);
    return r.value.host_long_long != 0;
}


int object_is_greater_than_or_equal(int target, struct object * a, struct object * b)
{
    char message[200];
    struct object  r;

    r = object_greater_than_or_equal(target, a, b, message);
    return r.value.host_long_long != 0;
}


int object_is_smaller_than_or_equal(int target, struct object * a, struct object * b)
{
    char message[200];
    struct object  r;

    r = object_smaller_than_or_equal(target, a, b, message);
    return r.value.host_long_long != 0;
}


struct object object_logical_not(int target, struct object * a, char warning_message[200])
{
    struct object  r;
    int common_type;

    a = object_get_referenced(a);
    _cake_zmem(&r, 120);
    r.value_type = 4;
    r.state = 2;
    common_type = a->value_type;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(!a->value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(!a->value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(!a->value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(!a->value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            r.value.host_u_long_long = (!a->value.host_float);
            goto __L0; /* break */

            __L12: /*case 11*/ 
            r.value.host_u_long_long = (!a->value.host_double);
            goto __L0; /* break */

            __L13: /*case 12*/ 
            r.value.host_u_long_long = (!a->value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_bitwise_not(int target, struct object * a, char warning_message[200])
{
    struct object  r;
    int common_type;

    a = object_get_referenced(a);
    _cake_zmem(&r, 120);
    r.state = 2;
    common_type = a->value_type;
    r.value_type = common_type;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(~a->value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(~a->value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(~a->value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(~a->value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            __L12: /*case 11*/ 
            __L13: /*case 12*/ 
            snprintf(warning_message, 200, "invalid operand");
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_unary_minus(int target, struct object * a, char warning_message[200])
{
    struct object  r;
    int common_type;

    a = object_get_referenced(a);
    _cake_zmem(&r, 120);
    r.state = 2;
    common_type = a->value_type;
    r.value_type = common_type;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(-(a->value.host_long_long))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(-(a->value.host_long_long))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(-(a->value.host_long_long))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(-(a->value.host_u_long_long))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            r.value.host_float = -(a->value.host_float);
            goto __L0; /* break */

            __L12: /*case 11*/ 
            r.value.host_double = -(a->value.host_double);
            goto __L0; /* break */

            __L13: /*case 12*/ 
            r.value.long_double_val = -(a->value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_unary_plus(int target, struct object * a, char warning_message[200])
{
    struct object  r;
    int common_type;

    a = object_get_referenced(a);
    _cake_zmem(&r, 120);
    r.state = 2;
    common_type = a->value_type;
    r.value_type = common_type;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(+(a->value.host_long_long))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(+(a->value.host_long_long))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(+(a->value.host_long_long))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(+(a->value.host_u_long_long))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            r.value.host_float = +(a->value.host_float);
            goto __L0; /* break */

            __L12: /*case 11*/ 
            r.value.host_double = +(a->value.host_double);
            goto __L0; /* break */

            __L13: /*case 12*/ 
            r.value.long_double_val = +(a->value.long_double_val);
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_bitwise_xor(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long ^ b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long ^ b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long ^ b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long ^ b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            __L12: /*case 11*/ 
            __L13: /*case 12*/ 
            ;
            snprintf(warning_message, 200, " invalid operands");
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_bitwise_or(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long | b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long | b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long | b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long | b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            __L12: /*case 11*/ 
            __L13: /*case 12*/ 
            ;
            snprintf(warning_message, 200, " invalid operands");
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_bitwise_and(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long & b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long & b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long & b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long & b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            __L12: /*case 11*/ 
            __L13: /*case 12*/ 
            ;
            snprintf(warning_message, 200, " invalid operands");
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_shift_left(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long << b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long << b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long << b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long << b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            __L12: /*case 11*/ 
            __L13: /*case 12*/ 
            ;
            snprintf(warning_message, 200, " invalid operands");
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


struct object object_shift_right(int target, struct object * a, struct object * b, char warning_message[200])
{
    int common_type;
    struct object  a0;
    struct object  b0;
    struct object  r;

    a = object_get_referenced(a);
    b = object_get_referenced(b);
    common_type = object_common(a, b);
    a0 = object_cast(target, common_type, a);
    b0 = object_cast(target, common_type, b);
    _cake_zmem(&r, 120);
    r.value_type = common_type;
    r.state = 2;
    /*switch*/
    {
        int __v0 = common_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 4) goto __L3; /*case 4*/
        if (__v0 == 6) goto __L4; /*case 6*/
        if (__v0 == 8) goto __L5; /*case 8*/
        if (__v0 == 1) goto __L6; /*case 1*/
        if (__v0 == 3) goto __L7; /*case 3*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 7) goto __L9; /*case 7*/
        if (__v0 == 9) goto __L10; /*case 9*/
        if (__v0 == 10) goto __L11; /*case 10*/
        if (__v0 == 11) goto __L12; /*case 11*/
        if (__v0 == 12) goto __L13; /*case 12*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            __L2: /*case 2*/ 
            __L3: /*case 4*/ 
            __L4: /*case 6*/ 
            __L5: /*case 8*/ 
            {
                r.value.host_long_long = ((long long)((((unsigned long long)((long long)(((unsigned long long)(a0.value.host_long_long >> b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & (1ULL << ((get_num_of_bits(target, common_type)) - 1))) ? (((long long)(((unsigned long long)(a0.value.host_long_long >> b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))) | ~((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))) : (((long long)((long long)(((unsigned long long)(a0.value.host_long_long >> b0.value.host_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))))) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1)))));
            }
            goto __L0; /* break */

            __L6: /*case 1*/ 
            __L7: /*case 3*/ 
            __L8: /*case 5*/ 
            __L9: /*case 7*/ 
            __L10: /*case 9*/ 
            r.value.host_u_long_long = ((unsigned long long)(((unsigned long long)(a0.value.host_u_long_long >> b0.value.host_u_long_long)) & ((unsigned long long)((1ULL << (get_num_of_bits(target, common_type))) - 1))));
            goto __L0; /* break */

            __L11: /*case 10*/ 
            __L12: /*case 11*/ 
            __L13: /*case 12*/ 
            ;
            snprintf(warning_message, 200, " invalid operands");
            goto __L0; /* break */

        }
        __L0:;
    }
    return r;
}


