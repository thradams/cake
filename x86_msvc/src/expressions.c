/* Cake 0.12.26 x86_msvc */
struct param;

struct param_list {
    unsigned char  is_var_args;
    unsigned char  is_void;
    struct param * head;
    struct param * tail;
};

struct struct_or_union_specifier;
struct enum_specifier;
struct expression;

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

struct expression_statement;
struct simple_declaration;

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct token;

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

struct enumerator;
struct macro;
struct struct_entry;

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

struct secondary_block;

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

struct flow_object;

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
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

struct token_list {
    struct token * head;
    struct token * tail;
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

struct initializer;

struct init_declarator {
    unsigned char  has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
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

struct balanced_token_sequence;

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
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

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
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

struct attribute_list {
    int attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct scope {
    int scope_level;
    struct hash_map  tags;
    struct hash_map  variables;
    struct scope * next;
    struct scope * previous;
};

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
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

struct function_declarator {
    struct direct_declarator * direct_declarator;
    struct scope  parameters_scope;
    struct parameter_type_list * parameter_type_list_opt;
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

struct iteration_statement;

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
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

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
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

struct diagnostic_id_stack;
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

struct struct_entry;

struct macro;

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

struct diagnostic_id_stack {
    int size;
    int stack[10];
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

struct enumerator {
    unsigned char  has_shared_ownership;
    struct token * token;
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct expression * constant_expression_opt;
    struct enum_specifier * enum_specifier;
    struct enumerator * next;
    struct object  value;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
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

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
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
struct enumerator *find_enumerator(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt);

unsigned char is_enumeration_constant(struct parser_ctx * ctx)
{
    unsigned char  is_enumerator;

    if (ctx->current == 0)
    {
        return 0;
    }
    if (ctx->current->type != 8996)
    {
        return 0;
    }
    if (ctx->current->flags & 128)
    {
        return 1;
    }
    if (ctx->current->flags & 256)
    {
        return 0;
    }
    is_enumerator = find_enumerator(ctx, ctx->current->lexeme, 0) != 0;
    if (is_enumerator)
    {
        ctx->current->flags |= 128;
    }
    else
    {
        ctx->current->flags |= 256;
    }
    return is_enumerator;
}


unsigned char is_first_of_floating_constant(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 140 || ctx->current->type == 141;
}


unsigned char is_first_of_integer_constant(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 136 || ctx->current->type == 137 || ctx->current->type == 138 || ctx->current->type == 139;
}


unsigned char is_predefined_constant(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 9072 || ctx->current->type == 9073 || ctx->current->type == 9074;
}


unsigned char is_first_of_constant(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return is_first_of_integer_constant(ctx) || is_first_of_floating_constant(ctx) || is_enumeration_constant(ctx) || (ctx->current->type == 131) || is_predefined_constant(ctx);
}


unsigned char is_first_of_primary_expression(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 8996 || is_first_of_constant(ctx) || ctx->current->type == 130 || ctx->current->type == 40 || ctx->current->type == 9065;
}


void unexpected_end_of_file(struct parser_ctx * ctx);
void *calloc(unsigned int nmemb, unsigned int size);
void parser_match(struct parser_ctx * ctx);
unsigned char first_of_type_name(struct parser_ctx * ctx);
struct type_name *type_name(struct parser_ctx * ctx);
struct type make_type_using_declarator(struct parser_ctx * ctx, struct declarator * pdeclarator);
unsigned char compiler_diagnostic(int w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);
int parser_match_tk(struct parser_ctx * ctx, int type);
struct expression *assignment_expression(struct parser_ctx * ctx, int eval_mode);
void generic_association_delete(struct generic_association * p);

struct generic_association *generic_association(struct parser_ctx * ctx, int eval_mode)
{
    struct generic_association * p_generic_association;

    p_generic_association = 0;
    if (1) /*try*/
    {
        struct expression * p_expression_temp;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_generic_association = calloc(1, 88);
        if (p_generic_association == 0)
        {
            goto __L0; /* throw */
        }
        p_generic_association->first_token = ctx->current;
        if (ctx->current->type == 9007)
        {
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
        }
        else
        {
            if (first_of_type_name(ctx))
            {
                unsigned char  old;

                old = ctx->inside_generic_association;
                ctx->inside_generic_association = 1;
                p_generic_association->p_type_name = type_name(ctx);
                if (p_generic_association->p_type_name == 0)
                {
                    goto __L0; /* throw */
                }
                ;
                ctx->inside_generic_association = old;
                p_generic_association->type = make_type_using_declarator(ctx, p_generic_association->p_type_name->abstract_declarator);
            }
            else
            {
                compiler_diagnostic(650, ctx, ctx->current, 0, "unexpected");
            }
        }
        if (parser_match_tk(ctx, 58) != 0)
        {
            goto __L0; /* throw */
        }
        p_expression_temp = assignment_expression(ctx, eval_mode);
        if (p_expression_temp == 0)
        {
            goto __L0; /* throw */
        }
        p_generic_association->expression = p_expression_temp;
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_generic_association->last_token = ctx->current;
    }
    else __L0: /*catch*/ 
    {
        generic_association_delete(p_generic_association);
        p_generic_association = 0;
    }
    return p_generic_association;
}


void generic_assoc_list_add(struct generic_assoc_list * p, struct generic_association * item);

struct generic_assoc_list generic_association_list(struct parser_ctx * ctx, int eval_mode)
{
    struct generic_assoc_list  list;

    _cake_zmem(&list, 8);
    if (1) /*try*/
    {
        struct generic_association * p_default_generic_association;
        struct generic_association * p_generic_association;

        p_default_generic_association = 0;
        p_generic_association = generic_association(ctx, 1);
        if (p_generic_association == 0)
        {
            goto __L0; /* throw */
        }
        if (p_generic_association->first_token->type == 9007)
        {
            p_default_generic_association = p_generic_association;
        }
        generic_assoc_list_add(&list, p_generic_association);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        while (ctx->current->type == 44)
        {
            struct generic_association * p_generic_association2;

            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            p_generic_association2 = generic_association(ctx, eval_mode);
            if (p_generic_association2 == 0)
            {
                goto __L0; /* throw */
            }
            if (p_generic_association2->first_token->type == 9007)
            {
                if (p_default_generic_association != 0)
                {
                    compiler_diagnostic(1570, ctx, p_generic_association2->first_token, 0, "duplicate default generic association.");
                    compiler_diagnostic(63, ctx, p_default_generic_association->first_token, 0, "previous default generic association");
                }
                else
                {
                    p_default_generic_association = p_generic_association2;
                }
            }
            generic_assoc_list_add(&list, p_generic_association2);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
    return list;
}


void type_name_delete(struct type_name * p);
void expression_delete(struct expression * p);
void type_destroy(struct type * p_type);
void free(void * ptr);

void generic_association_delete(struct generic_association * p)
{
    if (p)
    {
        ;
        type_name_delete(p->p_type_name);
        expression_delete(p->expression);
        type_destroy(&p->type);
        free(p);
    }
}


void generic_assoc_list_add(struct generic_assoc_list * list, struct generic_association * pitem)
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


void generic_assoc_list_destroy(struct generic_assoc_list * p)
{
    struct generic_association * item;

    item = p->head;
    while (item)
    {
        struct generic_association * next;

        next = item->next;
        item->next = 0;
        generic_association_delete(item);
        item = next;
    }
}


void generic_selection_delete(struct generic_selection * p)
{
    if (p)
    {
        expression_delete(p->expression);
        type_name_delete(p->type_name);
        generic_assoc_list_destroy(&p->generic_assoc_list);
        free(p);
    }
}


unsigned char expression_is_subjected_to_lvalue_conversion(struct expression *);
struct type type_lvalue_conversion(struct type * p_type, unsigned char  nullchecks_enabled);
unsigned char type_is_same(struct type * a, struct type * b, unsigned char  compare_qualifiers);

struct generic_selection *generic_selection(struct parser_ctx * ctx, int eval_mode)
{
    struct generic_selection * p_generic_selection;

    p_generic_selection = 0;
    if (1) /*try*/
    {
        struct type  lvalue_type;
        struct type * p_type;
        struct generic_association * current;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_generic_selection = calloc(1, 28);
        if (p_generic_selection == 0)
        {
            goto __L0; /* throw */
        }
        p_generic_selection->first_token = ctx->current;
        if (parser_match_tk(ctx, 9065) != 0)
        {
            goto __L0; /* throw */
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto __L0; /* throw */
        }
        if (first_of_type_name(ctx))
        {
            p_generic_selection->type_name = type_name(ctx);
            if (p_generic_selection->type_name == 0)
            {
                goto __L0; /* throw */
            }
        }
        else
        {
            p_generic_selection->expression = assignment_expression(ctx, eval_mode);
            if (p_generic_selection->expression == 0)
            {
                goto __L0; /* throw */
            }
        }
        if (parser_match_tk(ctx, 44) != 0)
        {
            goto __L0; /* throw */
        }
        p_generic_selection->generic_assoc_list = generic_association_list(ctx, eval_mode);
        if (p_generic_selection->generic_assoc_list.head == 0)
        {
            goto __L0; /* throw */
        }
        _cake_zmem(&lvalue_type, 68);
        p_type = 0;
        if (p_generic_selection->expression)
        {
            p_type = &p_generic_selection->expression->type;
            if (expression_is_subjected_to_lvalue_conversion(p_generic_selection->expression))
            {
                lvalue_type = type_lvalue_conversion(&p_generic_selection->expression->type, ctx->options.null_checks_enabled);
                p_type = &lvalue_type;
            }
        }
        else
        {
            if (p_generic_selection->type_name)
            {
                p_type = &p_generic_selection->type_name->abstract_declarator->type;
            }
            else
            {
                goto __L0; /* throw */
            }
        }
        current = p_generic_selection->generic_assoc_list.head;
        while (current)
        {
            if (current->p_type_name)
            {
                if (type_is_same(p_type, &current->type, 1))
                {
                    p_generic_selection->p_view_selected_expression = current->expression;
                    break;
                }
            }
            else
            {
                p_generic_selection->p_view_selected_expression = current->expression;
            }
            current = current->next;
        }
        type_destroy(&lvalue_type);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_generic_selection->last_token = ctx->current;
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        generic_selection_delete(p_generic_selection);
        p_generic_selection = 0;
    }
    return p_generic_selection;
}


unsigned long long object_type_get_unsigned_max(int target, int type);
struct platform *get_platform(int target);
unsigned char *utf8_decode(unsigned char * s, unsigned int * c);
unsigned char *escape_sequences_decode_opt(unsigned char * p, unsigned int * out_value);
struct object object_make_unsigned_char(int target, unsigned char value);
struct object object_make_wchar_t(int target, int value);
int object_type_to_type_specifier(int type);
struct object object_make_signed_int(int target, long long value);

struct expression *character_constant_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;
    unsigned long long wchar_max_value;

    p_expression_node = 0;
    wchar_max_value = object_type_get_unsigned_max(ctx->options.target, get_platform(ctx->options.target)->wchar_t_type);
    if (1) /*try*/
    {
        unsigned char * p;

        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_expression_node = calloc(1, 256);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        p_expression_node->expression_type = 5;
        p_expression_node->first_token = ctx->current;
        p_expression_node->last_token = p_expression_node->first_token;
        p_expression_node->type.attributes_flags |= 67108864;
        p_expression_node->type.category = 0;
        p = (unsigned char *)ctx->current->lexeme;
        if (p[0] == 117 && p[1] == 56)
        {
            unsigned int c;

            p++;
            p++;
            p++;
            p_expression_node->type.type_specifier_flags = 258;
            c = 0;
            p = utf8_decode(p, &c);
            if (p == 0)
            {
                goto __L0; /* throw */
            }
            if (c == 92)
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == 0)
                {
                    goto __L0; /* throw */
                }
            }
            if (*p != 39)
            {
                compiler_diagnostic(1370, ctx, ctx->current, 0, "Unicode character literals may not contain multiple characters.");
            }
            if (c > 128)
            {
                compiler_diagnostic(1360, ctx, ctx->current, 0, "character not encodable in a single code unit.");
            }
            p_expression_node->object = object_make_unsigned_char(ctx->options.target, (unsigned char)c);
        }
        else
        {
            if (p[0] == 117)
            {
                unsigned int c;

                p++;
                p++;
                p_expression_node->type.type_specifier_flags = 260;
                c = 0;
                p = utf8_decode(p, &c);
                if (p == 0)
                {
                    goto __L0; /* throw */
                }
                if (c == 92)
                {
                    p = escape_sequences_decode_opt(p, &c);
                    if (p == 0)
                    {
                        goto __L0; /* throw */
                    }
                }
                if (*p != 39)
                {
                    compiler_diagnostic(41, ctx, ctx->current, 0, "Unicode character literals may not contain multiple characters.");
                }
                if (c > wchar_max_value)
                {
                    compiler_diagnostic(41, ctx, ctx->current, 0, "Character too large for enclosing character literal type.");
                }
                p_expression_node->object = object_make_wchar_t(ctx->options.target, c);
            }
            else
            {
                if (p[0] == 85)
                {
                    unsigned int c;

                    p++;
                    p++;
                    p_expression_node->type.type_specifier_flags = 264;
                    c = 0;
                    p = utf8_decode(p, &c);
                    if (p == 0)
                    {
                        goto __L0; /* throw */
                    }
                    if (c == 92)
                    {
                        p = escape_sequences_decode_opt(p, &c);
                        if (p == 0)
                        {
                            goto __L0; /* throw */
                        }
                    }
                    if (*p != 39)
                    {
                        compiler_diagnostic(41, ctx, ctx->current, 0, "Unicode character literals may not contain multiple characters.");
                    }
                    if (c > 4294967295L)
                    {
                        compiler_diagnostic(41, ctx, ctx->current, 0, "Character too large for enclosing character literal type.");
                    }
                    p_expression_node->object = object_make_wchar_t(ctx->options.target, c);
                }
                else
                {
                    if (p[0] == 76)
                    {
                        long long value;

                        p++;
                        p++;
                        p_expression_node->type.type_specifier_flags = object_type_to_type_specifier(get_platform(ctx->options.target)->wchar_t_type);
                        value = 0;
                        while (*p != 39)
                        {
                            unsigned int c;

                            c = 0;
                            p = utf8_decode(p, &c);
                            if (p == 0)
                            {
                                goto __L0; /* throw */
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == 0)
                                {
                                    goto __L0; /* throw */
                                }
                            }
                            if (c < 128)
                            {
                                value = value * 256 + c;
                            }
                            else
                            {
                                value = c;
                            }
                            if (value > (long long)wchar_max_value)
                            {
                                compiler_diagnostic(42, ctx, ctx->current, 0, "character constant too long for its type", ctx->current->lexeme);
                                break;
                            }
                        }
                        p_expression_node->object = object_make_wchar_t(ctx->options.target, (unsigned int)value);
                    }
                    else
                    {
                        long long value;

                        p++;
                        p_expression_node->type.type_specifier_flags = 8;
                        value = 0;
                        while (*p != 39)
                        {
                            unsigned int c;

                            c = 0;
                            p = utf8_decode(p, &c);
                            if (p == 0)
                            {
                                goto __L0; /* throw */
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == 0)
                                {
                                    goto __L0; /* throw */
                                }
                            }
                            value = value * 256 + c;
                            if (value > 2147483647)
                            {
                                compiler_diagnostic(42, ctx, ctx->current, 0, "character constant too long for its type", ctx->current->lexeme);
                                break;
                            }
                        }
                        p_expression_node->object = object_make_signed_int(ctx->options.target, value);
                    }
                }
            }
        }
        parser_match(ctx);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


long long object_type_get_signed_max(int target, int type);
int parse_number(char * lexeme, char suffix[4], char erromsg[100]);
unsigned long long strtoull(char * _String, char ** _EndPtr, int _Radix);
int *__cdecl _errno(void);
void object_destroy(struct object * p);
struct object object_make_unsigned_int(int target, unsigned int value);
struct object object_make_unsigned_long(int target, unsigned long long value);
struct object object_make_unsigned_long_long(int target, unsigned long long value);
struct object object_make_signed_long(int target, signed long long value);
struct object object_make_signed_long_long(int target, signed long long value);
float strtof(char * _String, char ** _EndPtr);
short __cdecl _fdclass(float _X);
short __cdecl _dclass(double _X);
short __cdecl _ldclass(long double _X);
struct object object_make_float(float value);
long double strtold(char * _String, char ** _EndPtr);
struct object object_make_long_double(long double value);
double strtod(char * _String, char ** _EndPtr);
struct object object_make_double(double value);

int convert_to_number(struct parser_ctx * ctx, struct expression * p_expression_node, unsigned char  disabled, int target)
{
    unsigned long long unsigned_int_max_value;
    unsigned long long signed_int_max_value;
    unsigned long long signed_long_max_value;
    unsigned long long unsigned_long_max_value;
    unsigned long long signed_long_long_max_value;
    unsigned long long unsigned_long_long_max_value;
    struct token * token;
    int c;
    char buffer[260];
    char * s;
    char errormsg[100];
    char suffix[4];
    int r;

    unsigned_int_max_value = object_type_get_signed_max(ctx->options.target, 5);
    signed_int_max_value = object_type_get_signed_max(ctx->options.target, 4);
    signed_long_max_value = object_type_get_signed_max(ctx->options.target, 6);
    unsigned_long_max_value = object_type_get_unsigned_max(ctx->options.target, 7);
    signed_long_long_max_value = object_type_get_signed_max(ctx->options.target, 8);
    unsigned_long_long_max_value = object_type_get_unsigned_max(ctx->options.target, 9);
    if (ctx->current == 0)
    {
        unexpected_end_of_file(ctx);
        return 1;
    }
    token = ctx->current;
    c = 0;
    _cake_zmem(&buffer, 260);
    s = token->lexeme;
    while (*s)
    {
        if (*s != 39)
        {
            buffer[c] = *s;
            c++;
        }
        s++;
    }
    _cake_zmem(&errormsg, 100);
    _cake_zmem(&suffix, 4);
    r = parse_number(buffer, suffix, errormsg);
    if (r == 0)
    {
        compiler_diagnostic(1380, ctx, token, 0, errormsg);
        return 0;
    }
    /*switch*/
    {
        int __v0 = token->type;
        if (__v0 == 136) goto __L4; /*case 136*/
        if (__v0 == 137) goto __L5; /*case 137*/
        if (__v0 == 138) goto __L6; /*case 138*/
        if (__v0 == 139) goto __L7; /*case 139*/
        if (__v0 == 140) goto __L24; /*case 140*/
        if (__v0 == 141) goto __L25; /*case 141*/
        goto __L31; /* default */

        {
            __L4: /*case 136*/ 
            __L5: /*case 137*/ 
            __L6: /*case 138*/ 
            __L7: /*case 139*/ 
            {
                unsigned long long value;

                value = 0;
                /*switch*/
                {
                    int __v1 = token->type;
                    if (__v1 == 136) goto __L9; /*case 136*/
                    if (__v1 == 137) goto __L10; /*case 137*/
                    if (__v1 == 138) goto __L12; /*case 138*/
                    if (__v1 == 139) goto __L13; /*case 139*/
                    goto __L14; /* default */

                    {
                        __L9: /*case 136*/ 
                        value = strtoull(buffer, 0, 10);
                        goto __L8; /* break */

                        __L10: /*case 137*/ 
                        if (buffer[1] == 111 || buffer[1] == 79)
                        {
                            value = strtoull(buffer + 2, 0, 8);
                        }
                        else
                        {
                            value = strtoull(buffer + 1, 0, 8);
                        }
                        goto __L8; /* break */

                        __L12: /*case 138*/ 
                        value = strtoull(buffer + 2, 0, 16);
                        goto __L8; /* break */

                        __L13: /*case 139*/ 
                        value = strtoull(buffer + 2, 0, 2);
                        goto __L8; /* break */

                        __L14: /* default */ 
                        goto __L8; /* break */

                    }
                    __L8:;
                }
                if (value == unsigned_long_long_max_value && (*_errno()) == 34)
                {
                    compiler_diagnostic(1350, ctx, token, 0, "integer literal is too large to be represented in any integer type");
                }
                if (suffix[0] == 85)
                {
                    if (value <= unsigned_int_max_value && suffix[1] != 76)
                    {
                        object_destroy(&p_expression_node->object);
                        p_expression_node->object = object_make_unsigned_int(ctx->options.target, (unsigned int)value);
                        p_expression_node->type.type_specifier_flags = 264;
                    }
                    else
                    {
                        if (value <= unsigned_long_max_value && suffix[2] != 76)
                        {
                            object_destroy(&p_expression_node->object);
                            p_expression_node->object = object_make_unsigned_long(target, value);
                            p_expression_node->type.type_specifier_flags = 272;
                        }
                        else
                        {
                            object_destroy(&p_expression_node->object);
                            p_expression_node->object = object_make_unsigned_long_long(ctx->options.target, value);
                            p_expression_node->type.type_specifier_flags = 4194560;
                        }
                    }
                }
                else
                {
                    if (value <= signed_int_max_value && suffix[0] != 76)
                    {
                        object_destroy(&p_expression_node->object);
                        p_expression_node->object = object_make_signed_int(ctx->options.target, (int)value);
                        p_expression_node->type.type_specifier_flags = 8;
                    }
                    else
                    {
                        if (value <= signed_long_max_value && suffix[1] != 76)
                        {
                            object_destroy(&p_expression_node->object);
                            p_expression_node->object = object_make_signed_long(target, (int)value);
                            p_expression_node->type.type_specifier_flags = 16;
                        }
                        else
                        {
                            if ((target == 1 || target == 2) && (value <= unsigned_long_max_value) && suffix[1] != 76)
                            {
                                object_destroy(&p_expression_node->object);
                                p_expression_node->object = object_make_unsigned_long(target, value);
                                p_expression_node->type.type_specifier_flags = 272;
                            }
                            else
                            {
                                if (value <= signed_long_max_value && suffix[1] != 76)
                                {
                                    object_destroy(&p_expression_node->object);
                                    p_expression_node->object = object_make_signed_long(target, value);
                                    p_expression_node->type.type_specifier_flags = 16;
                                }
                                else
                                {
                                    if (value <= signed_long_long_max_value)
                                    {
                                        object_destroy(&p_expression_node->object);
                                        p_expression_node->object = object_make_signed_long_long(ctx->options.target, value);
                                        p_expression_node->type.type_specifier_flags = 4194304;
                                    }
                                    else
                                    {
                                        compiler_diagnostic(50, ctx, token, 0, "integer literal is too large to be represented in a signed integer type, interpreting as unsigned");
                                        object_destroy(&p_expression_node->object);
                                        p_expression_node->object = object_make_unsigned_long_long(ctx->options.target, value);
                                        p_expression_node->type.type_specifier_flags = 4194560;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            goto __L3; /* break */

            __L24: /*case 140*/ 
            __L25: /*case 141*/ 
            {
                if (suffix[0] == 70)
                {
                    float value;

                    value = strtof(buffer, 0);
                    if (((1 ? _fdclass((float)(value)) : (4 == 4 ? 102 : 8 == 8 ? 100 : 108) == 100 ? _dclass((double)(value)) : _ldclass((long double)(value))) == 1) && (*_errno()) == 34)
                    {
                    }
                    p_expression_node->type.type_specifier_flags = 32;
                    object_destroy(&p_expression_node->object);
                    p_expression_node->object = object_make_float(value);
                }
                else
                {
                    if (suffix[0] == 76)
                    {
                        long double value;

                        value = strtold(buffer, 0);
                        if (((0 ? _fdclass((float)(value)) : 1 ? _dclass((double)(value)) : _ldclass((long double)(value))) == 1) && (*_errno()) == 34)
                        {
                        }
                        p_expression_node->type.type_specifier_flags = 80;
                        object_destroy(&p_expression_node->object);
                        p_expression_node->object = object_make_long_double(value);
                    }
                    else
                    {
                        double value;

                        value = strtod(buffer, 0);
                        if (((0 ? _fdclass((float)(value)) : 1 ? _dclass((double)(value)) : _ldclass((long double)(value))) == 1) && (*_errno()) == 34)
                        {
                        }
                        object_destroy(&p_expression_node->object);
                        p_expression_node->object = object_make_double(value);
                        p_expression_node->type.type_specifier_flags = 64;
                    }
                }
            }
            goto __L3; /* break */

            __L31: /* default */ 
            ;
        }
        __L3:;
    }
    return 0;
}


struct map_entry *find_variables(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt);
struct type type_make_enumerator(struct enum_specifier * enum_specifier);
unsigned char type_is_deprecated(struct type * p_type);
unsigned char type_is_function(struct type * p_type);
struct type type_dup(struct type * p_type);
struct object object_make_reference(struct object * object);
int __cdecl strcmp(char * _Str1, char * _Str2);
struct type type_make_literal_string2(int size, int chartype, int qualifiers, int target);
unsigned int __cdecl strlen(char * _Str);
struct object object_make_uint8(int target, unsigned char value);
struct object object_make_uint16(int target, unsigned short value);
struct object object_make_uint32(int target, unsigned int value);
struct object object_make_char(int target, int value);
struct object object_make_bool(int target, unsigned char  value);
struct object object_make_nullptr(int target);
static unsigned char is_integer_or_floating_constant(int type);
struct expression *expression(struct parser_ctx * ctx, int eval_mode);

struct expression *primary_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    if (ctx->current == 0)
    {
        unexpected_end_of_file(ctx);
        return 0;
    }
    p_expression_node = 0;
    if (1) /*try*/
    {
        if (ctx->current->type == 8996)
        {
            struct scope * p_scope;
            struct map_entry * p_entry;

            p_expression_node = calloc(1, 256);
            if (p_expression_node == 0)
            {
                goto __L1; /* throw */
            }
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;
            p_scope = 0;
            p_entry = find_variables(ctx, ctx->current->lexeme, &p_scope);
            if (p_entry && p_entry->type == 3)
            {
                struct enumerator * p_enumerator;

                ;
                p_enumerator = p_entry->data.p_enumerator;
                p_expression_node->expression_type = 1;
                p_expression_node->object = p_enumerator->value;
                p_expression_node->type = type_make_enumerator(p_enumerator->enum_specifier);
            }
            else
            {
                if (p_entry && (p_entry->type == 4 || p_entry->type == 5))
                {
                    struct declarator * p_declarator;
                    struct init_declarator * p_init_declarator;

                    p_declarator = 0;
                    p_init_declarator = 0;
                    if (p_entry->type == 5)
                    {
                        ;
                        p_init_declarator = p_entry->data.p_init_declarator;
                        p_declarator = p_init_declarator->p_declarator;
                    }
                    else
                    {
                        p_declarator = p_entry->data.p_declarator;
                    }
                    ;
                    if (type_is_deprecated(&p_declarator->type))
                    {
                        compiler_diagnostic(3, ctx, ctx->current, 0, "'%s' is deprecated", ctx->current->lexeme);
                    }
                    if (p_scope->scope_level == 0)
                    {
                    }
                    else
                    {
                        if ((p_declarator->type.storage_class_specifier_flags & 4) || (p_declarator->type.storage_class_specifier_flags & 8))
                        {
                        }
                        else
                        {
                            if (ctx->p_current_function_scope_opt)
                            {
                                unsigned char  b_type_is_function;

                                b_type_is_function = type_is_function(&p_declarator->type);
                                if (eval_mode == 2 && !b_type_is_function)
                                {
                                    unsigned char  inside_current_function_scope;

                                    inside_current_function_scope = 0;
                                    while (p_scope)
                                    {
                                        if (ctx->p_current_function_scope_opt == p_scope)
                                        {
                                            inside_current_function_scope = 1;
                                            break;
                                        }
                                        p_scope = p_scope->previous;
                                    }
                                    if (!inside_current_function_scope)
                                    {
                                        compiler_diagnostic(1870, ctx, ctx->current, 0, "'%s' cannot be evaluated in this scope", ctx->current->lexeme);
                                    }
                                }
                            }
                        }
                    }
                    p_declarator->num_uses++;
                    p_expression_node->declarator = p_declarator;
                    p_expression_node->p_init_declarator = p_init_declarator;
                    p_expression_node->expression_type = 2;
                    p_expression_node->type = type_dup(&p_declarator->type);
                    p_expression_node->object = object_make_reference(&p_declarator->object);
                }
                else
                {
                    if (ctx->p_current_function_opt && strcmp(ctx->current->lexeme, "__func__") == 0)
                    {
                        char * func_name;

                        func_name = ctx->p_current_function_opt->name_opt ? ctx->p_current_function_opt->name_opt->lexeme : "unnamed";
                        p_expression_node->expression_type = 4;
                        p_expression_node->first_token = ctx->current;
                        p_expression_node->last_token = ctx->current;
                        p_expression_node->type = type_make_literal_string2((int)strlen(func_name) + 1, 2, 1, ctx->options.target);
                    }
                    else
                    {
                        compiler_diagnostic(680, ctx, ctx->current, 0, "not found '%s'", ctx->current->lexeme);
                        goto __L1; /* throw */
                    }
                }
            }
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L1; /* throw */
            }
        }
        else
        {
            if (ctx->current->type == 130)
            {
                int char_type_specifiers;
                unsigned char  is_bigger_than_char;
                unsigned char  is_wide;
                unsigned char  is_u8;
                unsigned char  is_u32;
                unsigned char  is_u16;
                unsigned int number_of_elements_including_zero;
                struct object * last;
                struct object * p_new;
                int lit_flags;

                p_expression_node = calloc(1, 256);
                if (p_expression_node == 0)
                {
                    goto __L1; /* throw */
                }
                p_expression_node->expression_type = 3;
                p_expression_node->first_token = ctx->current;
                p_expression_node->last_token = ctx->current;
                char_type_specifiers = 2;
                is_bigger_than_char = 0;
                is_wide = 0;
                is_u8 = 0;
                is_u32 = 0;
                is_u16 = 0;
                if (ctx->current->lexeme[0] == 76)
                {
                    is_wide = 1;
                    is_bigger_than_char = 1;
                    char_type_specifiers = object_type_to_type_specifier(get_platform(ctx->options.target)->wchar_t_type);
                }
                else
                {
                    if (ctx->current->lexeme[0] == 117 && ctx->current->lexeme[1] == 56)
                    {
                        is_u8 = 1;
                        char_type_specifiers = 2;
                    }
                    else
                    {
                        if (ctx->current->lexeme[0] == 117)
                        {
                            is_u16 = 1;
                            is_bigger_than_char = 1;
                            char_type_specifiers = 256 | object_type_to_type_specifier(get_platform(ctx->options.target)->int16_type);
                        }
                        else
                        {
                            if (ctx->current->lexeme[0] == 85)
                            {
                                is_u32 = 1;
                                is_bigger_than_char = 1;
                                char_type_specifiers = 256 | object_type_to_type_specifier(get_platform(ctx->options.target)->int32_type);
                            }
                            else
                            {
                                char_type_specifiers = 2;
                            }
                        }
                    }
                }
                number_of_elements_including_zero = 0;
                last = 0;
                while (ctx->current->type == 130)
                {
                    unsigned char * it;
                    unsigned int value;

                    it = (unsigned char *)ctx->current->lexeme;
                    while (*it != 34)
                    it++;
                    ;
                    it++;
                    value = 0;
                    while (it && *it != 34)
                    {
                        unsigned int c;
                        struct object * p_new;

                        c = 0;
                        if (is_bigger_than_char)
                        {
                            it = utf8_decode(it, &c);
                            if (it == 0)
                            {
                                goto __L1; /* throw */
                            }
                        }
                        else
                        {
                            c = *it;
                            it++;
                        }
                        if (c == 92)
                        {
                            it = escape_sequences_decode_opt(it, &value);
                        }
                        else
                        {
                            value = c;
                        }
                        p_new = calloc(1, 120);
                        if (p_new == 0)
                        {
                            goto __L1; /* throw */
                        }
                        if (is_wide)
                        {
                            *p_new = object_make_wchar_t(ctx->options.target, value);
                        }
                        else
                        {
                            if (is_u8)
                            {
                                *p_new = object_make_uint8(ctx->options.target, (unsigned char)value);
                            }
                            else
                            {
                                if (is_u16)
                                {
                                    *p_new = object_make_uint16(ctx->options.target, (unsigned short)value);
                                }
                                else
                                {
                                    if (is_u32)
                                    {
                                        *p_new = object_make_uint32(ctx->options.target, (unsigned int)value);
                                    }
                                    else
                                    {
                                        *p_new = object_make_char(ctx->options.target, value);
                                    }
                                }
                            }
                        }
                        number_of_elements_including_zero++;
                        if (p_expression_node->object.members.head == 0)
                        {
                            p_expression_node->object.members.head = p_new;
                        }
                        else
                        {
                            if (last)
                            {
                                last->next = p_new;
                            }
                        }
                        last = p_new;
                    }
                    parser_match(ctx);
                    if (ctx->current == 0)
                    {
                        unexpected_end_of_file(ctx);
                        goto __L1; /* throw */
                    }
                }
                p_new = calloc(1, 120);
                if (p_new == 0)
                {
                    goto __L1; /* throw */
                }
                if (is_wide)
                {
                    *p_new = object_make_wchar_t(ctx->options.target, 0);
                }
                else
                {
                    if (is_u8)
                    {
                        *p_new = object_make_uint8(ctx->options.target, 0);
                    }
                    else
                    {
                        if (is_u16)
                        {
                            *p_new = object_make_uint16(ctx->options.target, 0);
                        }
                        else
                        {
                            if (is_u32)
                            {
                                *p_new = object_make_uint32(ctx->options.target, 0);
                            }
                            else
                            {
                                *p_new = object_make_char(ctx->options.target, 0);
                            }
                        }
                    }
                }
                number_of_elements_including_zero++;
                if (last == 0)
                {
                    p_expression_node->object.members.head = p_new;
                }
                else
                {
                    last->next = p_new;
                }
                lit_flags = ctx->options.const_literal ? 1 : 0;
                p_expression_node->type = type_make_literal_string2(number_of_elements_including_zero, char_type_specifiers, lit_flags, ctx->options.target);
            }
            else
            {
                if (ctx->current->type == 131)
                {
                    p_expression_node = character_constant_expression(ctx);
                }
                else
                {
                    if (ctx->current->type == 9072 || ctx->current->type == 9073)
                    {
                        p_expression_node = calloc(1, 256);
                        if (p_expression_node == 0)
                        {
                            goto __L1; /* throw */
                        }
                        p_expression_node->expression_type = 6;
                        p_expression_node->first_token = ctx->current;
                        p_expression_node->last_token = ctx->current;
                        p_expression_node->object = object_make_bool(ctx->options.target, ctx->current->type == 9072);
                        p_expression_node->type.type_specifier_flags = 512;
                        p_expression_node->type.type_qualifier_flags = 0;
                        parser_match(ctx);
                        if (ctx->current == 0)
                        {
                            unexpected_end_of_file(ctx);
                            goto __L1; /* throw */
                        }
                    }
                    else
                    {
                        if (ctx->current->type == 9074)
                        {
                            p_expression_node = calloc(1, 256);
                            if (p_expression_node == 0)
                            {
                                goto __L1; /* throw */
                            }
                            p_expression_node->expression_type = 6;
                            p_expression_node->first_token = ctx->current;
                            p_expression_node->last_token = ctx->current;
                            p_expression_node->object = object_make_nullptr(ctx->options.target);
                            p_expression_node->type.type_specifier_flags = 16777216;
                            p_expression_node->type.type_qualifier_flags = 0;
                            parser_match(ctx);
                            if (ctx->current == 0)
                            {
                                unexpected_end_of_file(ctx);
                                goto __L1; /* throw */
                            }
                        }
                        else
                        {
                            if (is_integer_or_floating_constant(ctx->current->type))
                            {
                                p_expression_node = calloc(1, 256);
                                if (p_expression_node == 0)
                                {
                                    goto __L1; /* throw */
                                }
                                p_expression_node->first_token = ctx->current;
                                p_expression_node->last_token = ctx->current;
                                p_expression_node->expression_type = 8;
                                convert_to_number(ctx, p_expression_node, 0, ctx->options.target);
                                parser_match(ctx);
                                if (ctx->current == 0)
                                {
                                    unexpected_end_of_file(ctx);
                                    goto __L1; /* throw */
                                }
                            }
                            else
                            {
                                if (ctx->current->type == 9065)
                                {
                                    p_expression_node = calloc(1, 256);
                                    if (p_expression_node == 0)
                                    {
                                        goto __L1; /* throw */
                                    }
                                    p_expression_node->first_token = ctx->current;
                                    p_expression_node->expression_type = 7;
                                    p_expression_node->generic_selection = generic_selection(ctx, eval_mode);
                                    if (p_expression_node->generic_selection == 0)
                                    {
                                        goto __L1; /* throw */
                                    }
                                    p_expression_node->last_token = p_expression_node->generic_selection->last_token;
                                    if (p_expression_node->generic_selection->p_view_selected_expression)
                                    {
                                        p_expression_node->type = type_dup(&p_expression_node->generic_selection->p_view_selected_expression->type);
                                        p_expression_node->object = p_expression_node->generic_selection->p_view_selected_expression->object;
                                    }
                                    else
                                    {
                                        compiler_diagnostic(690, ctx, ctx->current, 0, "no match for generic");
                                    }
                                }
                                else
                                {
                                    if (ctx->current->type == 40)
                                    {
                                        p_expression_node = calloc(1, 256);
                                        if (p_expression_node == 0)
                                        {
                                            goto __L1; /* throw */
                                        }
                                        p_expression_node->expression_type = 9;
                                        p_expression_node->first_token = ctx->current;
                                        parser_match(ctx);
                                        if (ctx->current == 0)
                                        {
                                            unexpected_end_of_file(ctx);
                                            goto __L1; /* throw */
                                        }
                                        p_expression_node->right = expression(ctx, eval_mode);
                                        if (p_expression_node->right == 0)
                                        {
                                            goto __L1; /* throw */
                                        }
                                        p_expression_node->type = type_dup(&p_expression_node->right->type);
                                        p_expression_node->object = p_expression_node->right->object;
                                        if (ctx->current == 0)
                                        {
                                            unexpected_end_of_file(ctx);
                                            goto __L1; /* throw */
                                        }
                                        p_expression_node->last_token = ctx->current;
                                        if (parser_match_tk(ctx, 41) != 0)
                                        {
                                            goto __L1; /* throw */
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic(650, ctx, ctx->current, 0, "unexpected");
                                        goto __L1; /* throw */
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else __L1: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    ;
    return p_expression_node;
}



static unsigned char is_integer_or_floating_constant(int type)
{
    return type == 136 || type == 137 || type == 138 || type == 139 || type == 140 || type == 141;
}
void argument_expression_delete(struct argument_expression * p)
{
    if (p)
    {
        expression_delete(p->expression);
        ;
        free(p);
    }
}


void argument_expression_list_push(struct argument_expression_list * list, struct argument_expression * p);

struct argument_expression_list argument_expression_list(struct parser_ctx * ctx, int eval_mode)
{
    struct argument_expression_list  list;
    struct argument_expression * p_argument_expression;

    _cake_zmem(&list, 8);
    p_argument_expression = 0;
    if (1) /*try*/
    {
        struct expression * p_assignment_expression;

        p_argument_expression = calloc(1, 12);
        if (p_argument_expression == 0)
        {
            goto __L0; /* throw */
        }
        p_assignment_expression = assignment_expression(ctx, eval_mode);
        if (p_assignment_expression == 0)
        {
            argument_expression_delete(p_argument_expression);
            goto __L0; /* throw */
        }
        p_argument_expression->expression = p_assignment_expression;
        argument_expression_list_push(&list, p_argument_expression);
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        while (ctx->current->type == 44)
        {
            struct argument_expression * p_argument_expression_2;
            struct expression * p_assignment_expression_2;

            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            p_argument_expression_2 = calloc(1, 12);
            if (p_argument_expression_2 == 0)
            {
                goto __L0; /* throw */
            }
            p_assignment_expression_2 = assignment_expression(ctx, eval_mode);
            if (p_assignment_expression_2 == 0)
            {
                argument_expression_delete(p_argument_expression_2);
                goto __L0; /* throw */
            }
            p_argument_expression_2->expression = p_assignment_expression_2;
            argument_expression_list_push(&list, p_argument_expression_2);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
    return list;
}


unsigned char first_of_type_name_ahead(struct parser_ctx * ctx);

unsigned char first_of_postfix_expression(struct parser_ctx * ctx)
{
    if (first_of_type_name_ahead(ctx))
    {
        return 1;
    }
    return is_first_of_primary_expression(ctx);
}


unsigned char type_is_pointer_or_array(struct type * p_type);
unsigned char type_is_pointer(struct type * p_type);
struct type type_remove_pointer(struct type * p_type);
unsigned char type_is_array(struct type * p_type);
struct type get_array_item_type(struct type * p_type);
unsigned char type_is_integer(struct type * p_type);
unsigned char object_has_constant_value(struct object * a);
unsigned long long object_to_unsigned_long_long(struct object * a);
struct object *object_get_member(struct object * p_object, unsigned int index);
unsigned char type_is_function_or_function_pointer(struct type * p_type);
struct type get_function_return_type(struct type * p_type);
static int compare_function_arguments(struct parser_ctx * ctx, struct type * p_type, struct argument_expression_list * p_argument_expression_list);
struct param_list *type_get_func_or_func_ptr_params(struct type * p_type);
unsigned char type_is_void(struct type * p_type);
void check_assigment(struct parser_ctx * ctx, struct type * left_type, struct expression * right, int assigment_type);
int make_object(struct type * p_type, struct object * obj, int target);
struct struct_or_union_specifier *find_struct_or_union_specifier(struct parser_ctx * ctx, char * lexeme);
struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);
struct member_declarator *find_member_declarator(struct member_declaration_list * list, char * name, int * p_member_index);
static void fix_member_type(struct type * p_type, struct type * struct_type, struct type * member_type);
struct object *find_object_declarator_by_index(struct object * p_object, struct member_declaration_list * list, int member_index);
unsigned char type_is_struct_or_union(struct type * p_type);
static void fix_arrow_member_type(struct type * p_type, struct type * left, struct type * member_type);
unsigned char type_is_owner(struct type * p_type);
unsigned char expression_is_lvalue(struct expression * expr);
struct token *previous_parser_token(struct token * token);

struct expression *postfix_expression_tail(struct parser_ctx * ctx, struct expression * p_expression_node_param, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = p_expression_node_param;
    if (1) /*try*/
    {
        while (ctx->current != 0)
        {
            if (ctx->current->type == 91)
            {
                struct expression * p_expression_node_new;

                p_expression_node_new = calloc(1, 256);
                if (p_expression_node_new == 0)
                {
                    goto __L0; /* throw */
                }
                p_expression_node->last_token = ctx->current;
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = 13;
                if (!type_is_pointer_or_array(&p_expression_node->type))
                {
                    compiler_diagnostic(700, ctx, ctx->current, 0, "subscripted value is neither array nor pointer");
                }
                if (type_is_pointer(&p_expression_node->type))
                {
                    p_expression_node_new->type = type_remove_pointer(&p_expression_node->type);
                }
                else
                {
                    if (type_is_array(&p_expression_node->type))
                    {
                        p_expression_node_new->type = get_array_item_type(&p_expression_node->type);
                    }
                }
                parser_match(ctx);
                if (ctx->current == 0)
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(p_expression_node_new);
                    goto __L0; /* throw */
                }
                p_expression_node_new->right = expression(ctx, eval_mode);
                if (p_expression_node_new->right == 0)
                {
                    expression_delete(p_expression_node_new);
                    goto __L0; /* throw */
                }
                if (!type_is_integer(&p_expression_node_new->right->type))
                {
                    compiler_diagnostic(1560, ctx, p_expression_node_new->right->first_token, 0, "array subscript is not an integer");
                }
                if (object_has_constant_value(&p_expression_node_new->right->object))
                {
                    unsigned long long index;

                    index = object_to_unsigned_long_long(&p_expression_node_new->right->object);
                    if (type_is_array(&p_expression_node->type))
                    {
                        if (p_expression_node->type.num_of_elements > 0)
                        {
                            struct object * it;

                            if (index >= (unsigned long long)p_expression_node->type.num_of_elements)
                            {
                                compiler_diagnostic(42, ctx, ctx->current, 0, "index %d is past the end of the array", index);
                            }
                            it = object_get_member(&p_expression_node->object, (int)index);
                            if (it != 0)
                            {
                                p_expression_node_new->object = object_make_reference(it);
                            }
                        }
                    }
                }
                if (parser_match_tk(ctx, 93) != 0)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = 0;
                    goto __L0; /* throw */
                }
                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else
            {
                if (ctx->current->type == 40)
                {
                    struct expression * p_expression_node_new;

                    p_expression_node_new = calloc(1, 256);
                    if (p_expression_node_new == 0)
                    {
                        goto __L0; /* throw */
                    }
                    p_expression_node->last_token = ctx->current;
                    p_expression_node_new->first_token = p_expression_node->first_token;
                    p_expression_node_new->expression_type = 12;
                    if (!type_is_function_or_function_pointer(&p_expression_node->type))
                    {
                        compiler_diagnostic(710, ctx, ctx->current, 0, "called object is not attr function or function pointer");
                    }
                    p_expression_node_new->type = get_function_return_type(&p_expression_node->type);
                    parser_match(ctx);
                    if (ctx->current == 0)
                    {
                        unexpected_end_of_file(ctx);
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = 0;
                        goto __L0; /* throw */
                    }
                    if (ctx->current->type != 41)
                    {
                        p_expression_node_new->argument_expression_list = argument_expression_list(ctx, eval_mode);
                    }
                    if (parser_match_tk(ctx, 41) != 0)
                    {
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = 0;
                        goto __L0; /* throw */
                    }
                    compare_function_arguments(ctx, &p_expression_node->type, &p_expression_node_new->argument_expression_list);
                    if (ctx->previous == 0)
                    {
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = 0;
                        goto __L0; /* throw */
                    }
                    make_object(&p_expression_node_new->type, &p_expression_node_new->object, ctx->options.target);
                    p_expression_node_new->last_token = ctx->previous;
                    p_expression_node_new->left = p_expression_node;
                    p_expression_node = p_expression_node_new;
                }
                else
                {
                    if (ctx->current->type == 46)
                    {
                        struct expression * p_expression_node_new;

                        p_expression_node_new = calloc(1, 256);
                        if (p_expression_node_new == 0)
                        {
                            goto __L0; /* throw */
                        }
                        p_expression_node->last_token = ctx->current;
                        p_expression_node_new->first_token = ctx->current;
                        p_expression_node_new->expression_type = 14;
                        p_expression_node_new->left = p_expression_node;
                        p_expression_node = 0;
                        p_expression_node_new->declarator = p_expression_node_new->left->declarator;
                        parser_match(ctx);
                        if (ctx->current == 0)
                        {
                            unexpected_end_of_file(ctx);
                            expression_delete(p_expression_node_new);
                            p_expression_node_new = 0;
                            goto __L0; /* throw */
                        }
                        if (p_expression_node_new->left->type.type_specifier_flags & 32768)
                        {
                            struct struct_or_union_specifier * p_complete;

                            ;
                            p_complete = find_struct_or_union_specifier(ctx, p_expression_node_new->left->type.struct_or_union_specifier->tag_name);
                            if (p_complete)
                            {
                                p_complete = get_complete_struct_or_union_specifier(p_complete);
                            }
                            if (p_complete)
                            {
                                int member_index;
                                struct member_declarator * p_member_declarator;

                                ;
                                member_index = 0;
                                p_member_declarator = find_member_declarator(&p_complete->member_declaration_list, ctx->current->lexeme, &member_index);
                                if (p_member_declarator)
                                {
                                    struct object * object;

                                    p_expression_node_new->member_index = member_index;
                                    if (p_member_declarator->declarator)
                                    {
                                        p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                                    }
                                    else
                                    {
                                    }
                                    if (p_member_declarator->declarator != 0)
                                    {
                                        fix_member_type(&p_expression_node_new->type, &p_expression_node_new->left->type, &p_member_declarator->declarator->type);
                                    }
                                    object = find_object_declarator_by_index(&p_expression_node_new->left->object, &p_complete->member_declaration_list, member_index);
                                    if (object)
                                    {
                                        p_expression_node_new->object = object_make_reference(object);
                                    }
                                    else
                                    {
                                    }
                                }
                                else
                                {
                                    compiler_diagnostic(720, ctx, ctx->current, 0, "member '%s' not found in 'struct %s'", ctx->current->lexeme, p_complete->tag_name);
                                }
                            }
                            else
                            {
                                compiler_diagnostic(720, ctx, ctx->current, 0, "incomplete struct type '%s'", p_expression_node_new->left->type.struct_or_union_specifier->tag_name);
                            }
                            if (parser_match_tk(ctx, 8996) != 0)
                            {
                                expression_delete(p_expression_node_new);
                                p_expression_node_new = 0;
                                goto __L0; /* throw */
                            }
                        }
                        else
                        {
                            compiler_diagnostic(730, ctx, ctx->current, 0, "structure or union required");
                        }
                        p_expression_node = p_expression_node_new;
                    }
                    else
                    {
                        if (ctx->current->type == 11582)
                        {
                            struct expression * p_expression_node_new;

                            p_expression_node_new = calloc(1, 256);
                            if (p_expression_node_new == 0)
                            {
                                goto __L0; /* throw */
                            }
                            p_expression_node->last_token = ctx->current;
                            p_expression_node_new->first_token = p_expression_node->first_token;
                            p_expression_node_new->last_token = ctx->current;
                            p_expression_node_new->expression_type = 15;
                            parser_match(ctx);
                            if (ctx->current == 0)
                            {
                                expression_delete(p_expression_node_new);
                                p_expression_node_new = 0;
                                goto __L0; /* throw */
                            }
                            if (type_is_pointer_or_array(&p_expression_node->type))
                            {
                                struct type  item_type;

                                _cake_zmem(&item_type, 68);
                                if (type_is_array(&p_expression_node->type))
                                {
                                    compiler_diagnostic(20, ctx, ctx->current, 0, "using indirection '->' in array");
                                    item_type = get_array_item_type(&p_expression_node->type);
                                }
                                else
                                {
                                    item_type = type_remove_pointer(&p_expression_node->type);
                                }
                                if (type_is_struct_or_union(&item_type))
                                {
                                    struct struct_or_union_specifier * p_complete;

                                    ;
                                    ;
                                    p_complete = get_complete_struct_or_union_specifier(p_expression_node->type.next->struct_or_union_specifier);
                                    if (p_complete)
                                    {
                                        int member_index;
                                        struct member_declarator * p_member_declarator;

                                        member_index = 0;
                                        p_member_declarator = find_member_declarator(&p_complete->member_declaration_list, ctx->current->lexeme, &member_index);
                                        if (p_member_declarator)
                                        {
                                            if (p_member_declarator->declarator)
                                            {
                                                p_expression_node_new->member_index = member_index;
                                                p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                                                fix_arrow_member_type(&p_expression_node_new->type, &p_expression_node->type, &p_expression_node_new->type);
                                            }
                                            else
                                            {
                                                ;
                                            }
                                        }
                                        else
                                        {
                                            compiler_diagnostic(720, ctx, ctx->current, 0, "member '%s' not found in struct '%s'", ctx->current->lexeme, p_expression_node->type.next->struct_or_union_specifier->tag_name);
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic(740, ctx, ctx->current, 0, "struct '%s' is incomplete.", ctx->current->lexeme);
                                    }
                                    if (parser_match_tk(ctx, 8996) != 0)
                                    {
                                        type_destroy(&item_type);
                                        expression_delete(p_expression_node_new);
                                        p_expression_node_new = 0;
                                        goto __L0; /* throw */
                                    }
                                }
                                else
                                {
                                    compiler_diagnostic(730, ctx, ctx->current, 0, "structure or union required");
                                }
                                type_destroy(&item_type);
                            }
                            else
                            {
                                compiler_diagnostic(730, ctx, ctx->current, 0, "structure or union required");
                            }
                            p_expression_node_new->left = p_expression_node;
                            p_expression_node = p_expression_node_new;
                        }
                        else
                        {
                            if (ctx->current->type == 11051)
                            {
                                struct expression * p_expression_node_new;

                                p_expression_node->last_token = ctx->current;
                                if (type_is_owner(&p_expression_node->type))
                                {
                                    compiler_diagnostic(1310, ctx, p_expression_node->first_token, 0, "operator ++ cannot be used in _Owner pointers");
                                }
                                if (!expression_is_lvalue(p_expression_node))
                                {
                                    compiler_diagnostic(1230, ctx, p_expression_node->first_token, 0, "lvalue required as increment operand");
                                }
                                p_expression_node_new = calloc(1, 256);
                                if (p_expression_node_new == 0)
                                {
                                    goto __L0; /* throw */
                                }
                                p_expression_node->last_token = ctx->current;
                                p_expression_node_new->first_token = ctx->current;
                                p_expression_node_new->expression_type = 16;
                                p_expression_node_new->type = type_dup(&p_expression_node->type);
                                parser_match(ctx);
                                if (ctx->current == 0)
                                {
                                    unexpected_end_of_file(ctx);
                                    expression_delete(p_expression_node_new);
                                    p_expression_node_new = 0;
                                    goto __L0; /* throw */
                                }
                                p_expression_node_new->left = p_expression_node;
                                p_expression_node = p_expression_node_new;
                            }
                            else
                            {
                                if (ctx->current->type == 11565)
                                {
                                    struct expression * p_expression_node_new;

                                    p_expression_node->last_token = ctx->current;
                                    if (type_is_owner(&p_expression_node->type))
                                    {
                                        compiler_diagnostic(1320, ctx, p_expression_node->first_token, 0, "operator -- cannot be used in owner pointers");
                                    }
                                    if (!expression_is_lvalue(p_expression_node))
                                    {
                                        compiler_diagnostic(1230, ctx, p_expression_node->first_token, 0, "lvalue required as decrement operand");
                                    }
                                    p_expression_node_new = calloc(1, 256);
                                    if (p_expression_node_new == 0)
                                    {
                                        goto __L0; /* throw */
                                    }
                                    p_expression_node_new->first_token = ctx->current;
                                    p_expression_node_new->expression_type = 17;
                                    p_expression_node_new->type = type_dup(&p_expression_node->type);
                                    parser_match(ctx);
                                    if (ctx->current == 0)
                                    {
                                        unexpected_end_of_file(ctx);
                                        expression_delete(p_expression_node_new);
                                        p_expression_node_new = 0;
                                        goto __L0; /* throw */
                                    }
                                    p_expression_node_new->left = p_expression_node;
                                    p_expression_node = p_expression_node_new;
                                }
                                else
                                {
                                    struct token * p_last;

                                    p_last = previous_parser_token(ctx->current);
                                    if (p_last == 0)
                                    {
                                        goto __L0; /* throw */
                                    }
                                    p_expression_node->last_token = p_last;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
    return p_expression_node;
}



static int compare_function_arguments(struct parser_ctx * ctx, struct type * p_type, struct argument_expression_list * p_argument_expression_list)
{
    if (1) /*try*/
    {
        struct param * p_current_parameter_type;
        struct param_list * p_param_list;
        struct argument_expression * p_current_argument;

        p_current_parameter_type = 0;
        p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == 0)
        {
            goto __L0; /* throw */
        }
        p_current_parameter_type = p_param_list->head;
        p_current_argument = p_argument_expression_list->head;
        if (p_current_parameter_type && type_is_void(&p_current_parameter_type->type))
        {
            p_current_parameter_type = 0;
        }
        while (p_current_argument && p_current_parameter_type)
        {
            check_assigment(ctx, &p_current_parameter_type->type, p_current_argument->expression, 1);
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }
        if (p_current_argument != 0 && !p_param_list->is_var_args)
        {
            compiler_diagnostic(660, ctx, p_current_argument->expression->first_token, 0, "too many arguments");
            goto __L0; /* throw */
        }
        if (p_current_parameter_type != 0 && !p_param_list->is_void)
        {
            if (p_argument_expression_list->tail)
            {
                compiler_diagnostic(670, ctx, p_argument_expression_list->tail->expression->first_token, 0, "too few arguments");
            }
            else
            {
                compiler_diagnostic(670, ctx, ctx->current, 0, "too few arguments");
            }
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        return 1;
    }
    return 0;
}

static void fix_member_type(struct type * p_type, struct type * struct_type, struct type * member_type)
{
    if (struct_type->type_qualifier_flags & 1)
    {
        p_type->type_qualifier_flags |= 1;
    }
    p_type->storage_class_specifier_flags = struct_type->storage_class_specifier_flags;
    if (struct_type->type_qualifier_flags & 32)
    {
        p_type->type_qualifier_flags &= -17;
    }
    if (struct_type->type_qualifier_flags & 64)
    {
        p_type->type_qualifier_flags |= 64;
    }
}

static void fix_arrow_member_type(struct type * p_type, struct type * left, struct type * member_type)
{
    struct type  t;

    t = type_remove_pointer(left);
    if (t.type_qualifier_flags & 1)
    {
        p_type->type_qualifier_flags |= 1;
    }
    if (t.type_qualifier_flags & 64)
    {
        p_type->type_qualifier_flags |= 64;
    }
    if (t.type_qualifier_flags & 32)
    {
        p_type->type_qualifier_flags &= -17;
    }
    type_destroy(&t);
}
struct declarator *declarator_get_innert_function_declarator(struct declarator * p);
void scope_list_push(struct scope_list * list, struct scope * s);
struct compound_statement *function_body(struct parser_ctx * ctx);
void scope_list_pop(struct scope_list * list);
struct braced_initializer *braced_initializer(struct parser_ctx * ctx);
unsigned char type_is_const_or_constexpr(struct type * p_type);
void object_default_initialization(struct object * p_object, unsigned char  is_constant);
int initializer_init_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * p_current_object, struct initializer * braced_initializer, unsigned char  is_constant, unsigned char  requires_constant_initialization);

struct expression *postfix_expression_type_name(struct parser_ctx * ctx, struct type_name * p_type_name_par, int eval_mode)
{
    struct type_name * p_type_name;
    struct expression * p_expression_node;

    p_type_name = p_type_name_par;
    p_expression_node = 0;
    if (1) /*try*/
    {
        struct token * p_previous;

        p_expression_node = calloc(1, 256);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        ;
        p_previous = previous_parser_token(p_type_name->first_token);
        if (p_previous == 0)
        {
            goto __L0; /* throw */
        }
        p_expression_node->first_token = p_previous;
        ;
        p_expression_node->type_name = p_type_name;
        p_type_name = 0;
        p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);
        if (type_is_function(&p_expression_node->type_name->abstract_declarator->type))
        {
            struct declarator * inner;
            struct scope * parameters_scope;
            struct declarator * p_current_function_opt;
            struct scope * p_current_function_scope_opt;

            inner = declarator_get_innert_function_declarator(p_expression_node->type_name->abstract_declarator);
            if (inner->direct_declarator->function_declarator == 0)
            {
                compiler_diagnostic(650, ctx, p_expression_node->type_name->first_token, 0, "missing function declarator");
                goto __L0; /* throw */
            }
            p_expression_node->expression_type = 10;
            parameters_scope = &p_expression_node->type_name->abstract_declarator->direct_declarator->function_declarator->parameters_scope;
            scope_list_push(&ctx->scopes, parameters_scope);
            p_current_function_opt = ctx->p_current_function_opt;
            ctx->p_current_function_opt = p_expression_node->type_name->abstract_declarator;
            p_current_function_scope_opt = ctx->p_current_function_scope_opt;
            ctx->p_current_function_scope_opt = ctx->scopes.tail;
            p_expression_node->compound_statement = function_body(ctx);
            scope_list_pop(&ctx->scopes);
            ctx->p_current_function_opt = p_current_function_opt;
            ctx->p_current_function_scope_opt = p_current_function_scope_opt;
        }
        else
        {
            unsigned char  is_constant;
            struct initializer  initializer;
            unsigned char  requires_constant_initialization;

            p_expression_node->expression_type = 11;
            p_expression_node->braced_initializer = braced_initializer(ctx);
            p_expression_node->type = type_dup(&p_expression_node->type_name->type);
            if (p_expression_node->type.storage_class_specifier_flags & 1)
            {
            }
            else
            {
                int er;

                er = make_object(&p_expression_node->type, &p_expression_node->object, ctx->options.target);
                if (er != 0)
                {
                    compiler_diagnostic(740, ctx, p_expression_node->first_token, 0, "incomplete struct/union type");
                    goto __L0; /* throw */
                }
            }
            is_constant = type_is_const_or_constexpr(&p_expression_node->type);
            object_default_initialization(&p_expression_node->object, is_constant);
            _cake_zmem(&initializer, 12);
            initializer.braced_initializer = p_expression_node->braced_initializer;
            initializer.first_token = p_expression_node->first_token;
            requires_constant_initialization = 0;
            initializer_init_new(ctx, &p_expression_node->type, &p_expression_node->object, &initializer, is_constant, 0);
        }
        if (ctx->previous == 0)
        {
            goto __L0; /* throw */
        }
        p_expression_node->last_token = ctx->previous;
        p_expression_node = postfix_expression_tail(ctx, p_expression_node, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    type_name_delete(p_type_name);
    return p_expression_node;
}


struct compound_statement *compound_statement(struct parser_ctx * ctx);

struct expression *postfix_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        if (first_of_type_name_ahead(ctx))
        {
            ;
            p_expression_node = calloc(1, 256);
            if (p_expression_node == 0)
            {
                goto __L0; /* throw */
            }
            ;
            p_expression_node->first_token = ctx->current;
            if (parser_match_tk(ctx, 40) != 0)
            {
                goto __L0; /* throw */
            }
            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == 0)
            {
                goto __L0; /* throw */
            }
            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);
            if (parser_match_tk(ctx, 41) != 0)
            {
                goto __L0; /* throw */
            }
            if (type_is_function(&p_expression_node->type))
            {
                p_expression_node->expression_type = 10;
                p_expression_node->compound_statement = compound_statement(ctx);
                if (p_expression_node->compound_statement == 0)
                {
                    goto __L0; /* throw */
                }
                p_expression_node->last_token = p_expression_node->compound_statement->last_token;
            }
            else
            {
                p_expression_node->expression_type = 11;
                p_expression_node->braced_initializer = braced_initializer(ctx);
                if (p_expression_node->braced_initializer == 0)
                {
                    goto __L0; /* throw */
                }
                if (ctx->current == 0)
                {
                    unexpected_end_of_file(ctx);
                    goto __L0; /* throw */
                }
                p_expression_node->last_token = ctx->current;
            }
        }
        else
        {
            p_expression_node = primary_expression(ctx, eval_mode);
            if (p_expression_node == 0)
            {
                goto __L0; /* throw */
            }
        }
        p_expression_node = postfix_expression_tail(ctx, p_expression_node, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


unsigned char is_first_of_compiler_function(struct parser_ctx * ctx)
{
    if (ctx->current == 0)
    {
        return 0;
    }
    return ctx->current->type == 9087 || ctx->current->type == 9089 || ctx->current->type == 9088 || ctx->current->type == 9086 || ctx->current->type == 9090 || ctx->current->type == 9091 || ctx->current->type == 9069 || ctx->current->type == 9092 || ctx->current->type == 9093 || ctx->current->type == 9094 || ctx->current->type == 9095;
}


struct token *parser_look_ahead(struct parser_ctx * ctx);

unsigned char is_first_of_unary_expression(struct parser_ctx * ctx)
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
            return 1;
        }
    }
    return first_of_postfix_expression(ctx) || ctx->current->type == 11051 || ctx->current->type == 11565 || ctx->current->type == 38 || ctx->current->type == 42 || ctx->current->type == 43 || ctx->current->type == 45 || ctx->current->type == 126 || ctx->current->type == 33 || ctx->current->type == 9030 || ctx->current->type == 9031 || ctx->current->type == 9044 || ctx->current->type == 9048 || ctx->current->type == 9049 || ctx->current->type == 9050 || ctx->current->type == 9051 || ctx->current->type == 9052 || is_first_of_compiler_function(ctx);
}


struct expression *cast_expression(struct parser_ctx * ctx, int eval_mode);
struct object object_logical_not(int target, struct object * a, char warning_message[200]);
struct type type_make_int_bool_like();
void type_integer_promotion(struct type * a);
struct object object_bitwise_not(int target, struct object * a, char warning_message[200]);
struct type type_common(struct type * p_type1, struct type * p_type2, int target);
struct object object_unary_minus(int target, struct object * a, char warning_message[200]);
struct object object_unary_plus(int target, struct object * a, char warning_message[200]);
struct type type_add_pointer(struct type * p_type, unsigned char  null_checks_enabled);
struct type make_void_type();
struct type make_size_t_type(int target);
int type_get_offsetof(struct type * p_type, char * member, unsigned int * size, int target);
struct object object_make_size_t(int target, unsigned long long value);
static int check_sizeof_argument(struct parser_ctx * ctx, struct expression * p_expression, struct type * p_type);
int type_get_category(struct type * p_type);
unsigned char type_is_vla(struct type * p_type);
int type_get_sizeof(struct type * p_type, unsigned int * size, int target);
struct type type_make_size_t(int target);
unsigned char type_is_enum(struct type * p_type);
struct enum_specifier *get_complete_enum_specifier(struct enum_specifier * p_enum_specifier);
struct expression *constant_expression(struct parser_ctx * ctx, unsigned char  show_error_if_not_constant, int eval_mode);
unsigned int type_get_alignof(struct type * p_type, int target);
unsigned char type_is_const(struct type * p_type);
unsigned char type_is_arithmetic(struct type * p_type);
unsigned char type_is_scalar(struct type * p_type);
unsigned char type_is_floating_point(struct type * p_type);

struct expression *unary_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (ctx->current->type == 11051 || ctx->current->type == 11565)
        {
            struct expression * new_expression;

            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            if (ctx->current->type == 11051)
            {
                new_expression->expression_type = 33;
            }
            else
            {
                new_expression->expression_type = 34;
            }
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            new_expression->right = unary_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            new_expression->type = type_dup(&new_expression->right->type);
            p_expression_node = new_expression;
        }
        else
        {
            if (ctx->current->type == 38 || ctx->current->type == 42 || ctx->current->type == 43 || ctx->current->type == 45 || ctx->current->type == 126 || ctx->current->type == 33)
            {
                struct expression * new_expression;
                struct token * op_position;
                int op;

                new_expression = calloc(1, 256);
                if (new_expression == 0)
                {
                    goto __L0; /* throw */
                }
                new_expression->first_token = ctx->current;
                op_position = ctx->current;
                op = ctx->current->type;
                parser_match(ctx);
                if (ctx->current == 0)
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(new_expression);
                    goto __L0; /* throw */
                }
                new_expression->right = cast_expression(ctx, eval_mode);
                if (new_expression->right == 0)
                {
                    expression_delete(new_expression);
                    goto __L0; /* throw */
                }
                new_expression->last_token = new_expression->right->last_token;
                if (op == 33)
                {
                    new_expression->expression_type = 35;
                    if (eval_mode == 2 && object_has_constant_value(&new_expression->right->object))
                    {
                        char warning_message[200];

                        _cake_zmem(&warning_message, 200);
                        new_expression->object = object_logical_not(ctx->options.target, &new_expression->right->object, warning_message);
                    }
                    new_expression->type = type_make_int_bool_like();
                }
                else
                {
                    if (op == 126)
                    {
                        struct type  promoted;

                        if (!type_is_integer(&new_expression->right->type))
                        {
                            compiler_diagnostic(850, ctx, op_position, 0, "requires integer type");
                            expression_delete(new_expression);
                            goto __L0; /* throw */
                        }
                        new_expression->expression_type = 36;
                        promoted = type_dup(&new_expression->right->type);
                        type_integer_promotion(&promoted);
                        new_expression->type = promoted;
                        if (eval_mode == 2 && object_has_constant_value(&new_expression->right->object))
                        {
                            char warning_message[200];

                            _cake_zmem(&warning_message, 200);
                            new_expression->object = object_bitwise_not(ctx->options.target, &new_expression->right->object, warning_message);
                        }
                    }
                    else
                    {
                        if (op == 45 || op == 43)
                        {
                            if (op == 45)
                            {
                                new_expression->expression_type = 37;
                            }
                            else
                            {
                                new_expression->expression_type = 38;
                            }
                            new_expression->type = type_common(&new_expression->right->type, &new_expression->right->type, ctx->options.target);
                            if (eval_mode == 2 && object_has_constant_value(&new_expression->right->object))
                            {
                                char warning_message[200];

                                _cake_zmem(&warning_message, 200);
                                if (op == 45)
                                {
                                    new_expression->object = object_unary_minus(ctx->options.target, &new_expression->right->object, warning_message);
                                }
                                else
                                {
                                    if (op == 43)
                                    {
                                        new_expression->object = object_unary_plus(ctx->options.target, &new_expression->right->object, warning_message);
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (op == 42)
                            {
                                new_expression->expression_type = 39;
                                if (!type_is_pointer_or_array(&new_expression->right->type))
                                {
                                    compiler_diagnostic(780, ctx, op_position, 0, "indirection requires pointer operand");
                                }
                                if (type_is_pointer(&new_expression->right->type))
                                {
                                    new_expression->type = type_remove_pointer(&new_expression->right->type);
                                }
                                else
                                {
                                    compiler_diagnostic(20, ctx, op_position, 0, "array indirection");
                                    new_expression->type = get_array_item_type(&new_expression->right->type);
                                }
                            }
                            else
                            {
                                if (op == 38)
                                {
                                    new_expression->expression_type = 40;
                                    if (!expression_is_lvalue(new_expression->right))
                                    {
                                        compiler_diagnostic(1220, ctx, new_expression->right->first_token, 0, "lvalue required as unary '&' operand");
                                    }
                                    if (new_expression->right->type.storage_class_specifier_flags & 32)
                                    {
                                        char * variable_name;

                                        variable_name = "?";
                                        if (new_expression->right->declarator && new_expression->right->declarator->name_opt)
                                        {
                                            variable_name = new_expression->right->declarator->name_opt->lexeme;
                                        }
                                        compiler_diagnostic(1220, ctx, new_expression->right->first_token, 0, "address of register variable 'x' requested", variable_name);
                                    }
                                    new_expression->type = type_add_pointer(&new_expression->right->type, ctx->options.null_checks_enabled);
                                    new_expression->type.address_of = 1;
                                }
                                else
                                {
                                    expression_delete(new_expression);
                                    compiler_diagnostic(790, ctx, ctx->current, 0, "invalid token");
                                    goto __L0; /* throw */
                                }
                            }
                        }
                    }
                }
                p_expression_node = new_expression;
            }
            else
            {
                if (ctx->current->type == 9050)
                {
                    struct expression * new_expression;

                    new_expression = calloc(1, 256);
                    if (new_expression == 0)
                    {
                        goto __L0; /* throw */
                    }
                    new_expression->first_token = ctx->current;
                    new_expression->expression_type = 21;
                    parser_match(ctx);
                    if (ctx->current == 0)
                    {
                        unexpected_end_of_file(ctx);
                        expression_delete(new_expression);
                        goto __L0; /* throw */
                    }
                    if (parser_match_tk(ctx, 40) != 0)
                    {
                        expression_delete(new_expression);
                        goto __L0; /* throw */
                    }
                    new_expression->left = unary_expression(ctx, eval_mode);
                    if (new_expression->left == 0)
                    {
                        expression_delete(new_expression);
                        goto __L0; /* throw */
                    }
                    if (ctx->current->type == 44)
                    {
                        parser_match(ctx);
                        new_expression->right = unary_expression(ctx, eval_mode);
                        if (new_expression->right == 0)
                        {
                            expression_delete(new_expression);
                            goto __L0; /* throw */
                        }
                    }
                    if (parser_match_tk(ctx, 41) != 0)
                    {
                        expression_delete(new_expression);
                        goto __L0; /* throw */
                    }
                    new_expression->type = make_void_type();
                    return new_expression;
                }
                else
                {
                    if (ctx->current->type == 9048)
                    {
                        struct expression * new_expression;

                        new_expression = calloc(1, 256);
                        if (new_expression == 0)
                        {
                            goto __L0; /* throw */
                        }
                        new_expression->first_token = ctx->current;
                        new_expression->expression_type = 22;
                        parser_match(ctx);
                        if (ctx->current == 0)
                        {
                            unexpected_end_of_file(ctx);
                            expression_delete(new_expression);
                            goto __L0; /* throw */
                        }
                        if (parser_match_tk(ctx, 40) != 0)
                        {
                            expression_delete(new_expression);
                            goto __L0; /* throw */
                        }
                        new_expression->left = unary_expression(ctx, eval_mode);
                        if (new_expression->left == 0)
                        {
                            expression_delete(new_expression);
                            goto __L0; /* throw */
                        }
                        if (parser_match_tk(ctx, 41) != 0)
                        {
                            expression_delete(new_expression);
                            goto __L0; /* throw */
                        }
                        new_expression->type = make_void_type();
                        return new_expression;
                    }
                    else
                    {
                        if (ctx->current->type == 9049)
                        {
                            struct expression * new_expression;

                            new_expression = calloc(1, 256);
                            if (new_expression == 0)
                            {
                                goto __L0; /* throw */
                            }
                            new_expression->first_token = ctx->current;
                            new_expression->expression_type = 24;
                            parser_match(ctx);
                            if (ctx->current == 0)
                            {
                                unexpected_end_of_file(ctx);
                                expression_delete(new_expression);
                                goto __L0; /* throw */
                            }
                            if (parser_match_tk(ctx, 40) != 0)
                            {
                                expression_delete(new_expression);
                                goto __L0; /* throw */
                            }
                            new_expression->left = unary_expression(ctx, eval_mode);
                            if (new_expression->left == 0)
                            {
                                expression_delete(new_expression);
                                goto __L0; /* throw */
                            }
                            if (parser_match_tk(ctx, 44) != 0)
                            {
                                expression_delete(new_expression);
                                goto __L0; /* throw */
                            }
                            new_expression->type_name = type_name(ctx);
                            if (new_expression->type_name == 0)
                            {
                                expression_delete(new_expression);
                                goto __L0; /* throw */
                            }
                            if (parser_match_tk(ctx, 41) != 0)
                            {
                                expression_delete(new_expression);
                                goto __L0; /* throw */
                            }
                            new_expression->type = type_dup(&new_expression->type_name->type);
                            return new_expression;
                        }
                        else
                        {
                            if (ctx->current->type == 9051)
                            {
                                struct expression * new_expression;

                                new_expression = calloc(1, 256);
                                if (new_expression == 0)
                                {
                                    goto __L0; /* throw */
                                }
                                new_expression->first_token = ctx->current;
                                new_expression->expression_type = 23;
                                parser_match(ctx);
                                if (ctx->current == 0)
                                {
                                    unexpected_end_of_file(ctx);
                                    expression_delete(new_expression);
                                    goto __L0; /* throw */
                                }
                                if (parser_match_tk(ctx, 40) != 0)
                                {
                                    expression_delete(new_expression);
                                    goto __L0; /* throw */
                                }
                                new_expression->left = unary_expression(ctx, eval_mode);
                                if (new_expression->left == 0)
                                {
                                    expression_delete(new_expression);
                                    goto __L0; /* throw */
                                }
                                if (parser_match_tk(ctx, 44) != 0)
                                {
                                    expression_delete(new_expression);
                                    goto __L0; /* throw */
                                }
                                new_expression->right = unary_expression(ctx, eval_mode);
                                if (new_expression->right == 0)
                                {
                                    expression_delete(new_expression);
                                    goto __L0; /* throw */
                                }
                                if (parser_match_tk(ctx, 41) != 0)
                                {
                                    expression_delete(new_expression);
                                    goto __L0; /* throw */
                                }
                                new_expression->type = make_void_type();
                                return new_expression;
                            }
                            else
                            {
                                if (ctx->current->type == 9052)
                                {
                                    struct expression * new_expression;
                                    unsigned int offsetof;
                                    int e;

                                    new_expression = calloc(1, 256);
                                    if (new_expression == 0)
                                    {
                                        goto __L0; /* throw */
                                    }
                                    new_expression->first_token = ctx->current;
                                    new_expression->expression_type = 25;
                                    parser_match(ctx);
                                    if (ctx->current == 0)
                                    {
                                        unexpected_end_of_file(ctx);
                                        expression_delete(new_expression);
                                        goto __L0; /* throw */
                                    }
                                    if (parser_match_tk(ctx, 40) != 0)
                                    {
                                        expression_delete(new_expression);
                                        goto __L0; /* throw */
                                    }
                                    new_expression->type_name = type_name(ctx);
                                    if (new_expression->type_name == 0)
                                    {
                                        expression_delete(new_expression);
                                        goto __L0; /* throw */
                                    }
                                    if (parser_match_tk(ctx, 44) != 0)
                                    {
                                        expression_delete(new_expression);
                                        goto __L0; /* throw */
                                    }
                                    if (ctx->current == 0)
                                    {
                                        unexpected_end_of_file(ctx);
                                        expression_delete(new_expression);
                                        goto __L0; /* throw */
                                    }
                                    if (ctx->current->type != 8996)
                                    {
                                        expression_delete(new_expression);
                                        goto __L0; /* throw */
                                    }
                                    new_expression->offsetof_member_designator = ctx->current;
                                    parser_match(ctx);
                                    if (parser_match_tk(ctx, 41) != 0)
                                    {
                                        expression_delete(new_expression);
                                        goto __L0; /* throw */
                                    }
                                    new_expression->type = make_size_t_type(ctx->options.target);
                                    offsetof = 0;
                                    e = type_get_offsetof(&new_expression->type_name->type, new_expression->offsetof_member_designator->lexeme, &offsetof, ctx->options.target);
                                    if (e != 0)
                                    {
                                        expression_delete(new_expression);
                                        goto __L0; /* throw */
                                    }
                                    new_expression->object = object_make_size_t(ctx->options.target, offsetof);
                                    return new_expression;
                                }
                                else
                                {
                                    if (ctx->current->type == 9030)
                                    {
                                        struct expression * new_expression;

                                        parser_match(ctx);
                                        if (ctx->current == 0)
                                        {
                                            unexpected_end_of_file(ctx);
                                            goto __L0; /* throw */
                                        }
                                        new_expression = calloc(1, 256);
                                        if (new_expression == 0)
                                        {
                                            goto __L0; /* throw */
                                        }
                                        new_expression->first_token = ctx->current;
                                        if (first_of_type_name_ahead(ctx))
                                        {
                                            new_expression->expression_type = 19;
                                            if (parser_match_tk(ctx, 40) != 0)
                                            {
                                                expression_delete(new_expression);
                                                goto __L0; /* throw */
                                            }
                                            new_expression->type_name = type_name(ctx);
                                            if (new_expression->type_name == 0)
                                            {
                                                expression_delete(new_expression);
                                                goto __L0; /* throw */
                                            }
                                            new_expression->type = make_size_t_type(ctx->options.target);
                                            if (ctx->current == 0)
                                            {
                                                unexpected_end_of_file(ctx);
                                                expression_delete(new_expression);
                                                goto __L0; /* throw */
                                            }
                                            new_expression->last_token = ctx->current;
                                            if (parser_match_tk(ctx, 41) != 0)
                                            {
                                                expression_delete(new_expression);
                                                goto __L0; /* throw */
                                            }
                                            if (check_sizeof_argument(ctx, new_expression, &new_expression->type_name->type) != 0)
                                            {
                                            }
                                            else
                                            {
                                                if (type_is_vla(&new_expression->type_name->abstract_declarator->type))
                                                {
                                                }
                                                else
                                                {
                                                    unsigned int type_sizeof;

                                                    type_sizeof = 0;
                                                    if (type_get_sizeof(&new_expression->type_name->abstract_declarator->type, &type_sizeof, ctx->options.target) != 0)
                                                    {
                                                        expression_delete(new_expression);
                                                        goto __L0; /* throw */
                                                    }
                                                    new_expression->object = object_make_size_t(ctx->options.target, type_sizeof);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            new_expression->right = unary_expression(ctx, 1);
                                            if (new_expression->right == 0)
                                            {
                                                expression_delete(new_expression);
                                                goto __L0; /* throw */
                                            }
                                            new_expression->expression_type = 18;
                                            if (check_sizeof_argument(ctx, new_expression->right, &new_expression->right->type) != 0)
                                            {
                                                expression_delete(new_expression);
                                                goto __L0; /* throw */
                                            }
                                            if (type_is_vla(&new_expression->right->type))
                                            {
                                            }
                                            else
                                            {
                                                unsigned int sz;

                                                sz = 0;
                                                if (type_get_sizeof(&new_expression->right->type, &sz, ctx->options.target) != 0)
                                                {
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                new_expression->object = object_make_size_t(ctx->options.target, sz);
                                            }
                                        }
                                        type_destroy(&new_expression->type);
                                        new_expression->type = type_make_size_t(ctx->options.target);
                                        p_expression_node = new_expression;
                                    }
                                    else
                                    {
                                        if (ctx->current->type == 9031)
                                        {
                                            struct expression * new_expression;

                                            new_expression = calloc(1, 256);
                                            if (new_expression == 0)
                                            {
                                                goto __L0; /* throw */
                                            }
                                            new_expression->first_token = ctx->current;
                                            parser_match(ctx);
                                            if (ctx->current == 0)
                                            {
                                                unexpected_end_of_file(ctx);
                                                expression_delete(new_expression);
                                                goto __L0; /* throw */
                                            }
                                            new_expression->expression_type = 20;
                                            if (first_of_type_name_ahead(ctx))
                                            {
                                                if (parser_match_tk(ctx, 40) != 0)
                                                {
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                new_expression->type_name = type_name(ctx);
                                                if (new_expression->type_name == 0)
                                                {
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                new_expression->type = make_size_t_type(ctx->options.target);
                                                if (ctx->current == 0)
                                                {
                                                    unexpected_end_of_file(ctx);
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                new_expression->last_token = ctx->current;
                                                if (parser_match_tk(ctx, 41) != 0)
                                                {
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                if (type_is_enum(&new_expression->type_name->abstract_declarator->type))
                                                {
                                                    struct enum_specifier * p_enum_specifier;
                                                    unsigned int nelements;

                                                    p_enum_specifier = get_complete_enum_specifier(new_expression->type_name->type.enum_specifier);
                                                    nelements = 0;
                                                    if (p_enum_specifier)
                                                    {
                                                        struct enumerator * p;

                                                        p = p_enum_specifier->enumerator_list.head;
                                                        while (p)
                                                        {
                                                            nelements++;
                                                            p = p->next;
                                                        }
                                                    }
                                                    new_expression->object = object_make_size_t(ctx->options.target, nelements);
                                                }
                                                else
                                                {
                                                    if (type_is_array(&new_expression->type_name->abstract_declarator->type))
                                                    {
                                                        unsigned int nelements;

                                                        nelements = new_expression->type_name->abstract_declarator->type.num_of_elements;
                                                        if (nelements > 0)
                                                        {
                                                            new_expression->object = object_make_size_t(ctx->options.target, nelements);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        compiler_diagnostic(1390, ctx, new_expression->type_name->first_token, 0, "argument of _Countof must be an array");
                                                        expression_delete(new_expression);
                                                        goto __L0; /* throw */
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (parser_match_tk(ctx, 40) != 0)
                                                {
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                new_expression->right = unary_expression(ctx, 1);
                                                if (new_expression->right == 0)
                                                {
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                if (ctx->current == 0)
                                                {
                                                    unexpected_end_of_file(ctx);
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                new_expression->last_token = ctx->current;
                                                if (parser_match_tk(ctx, 41) != 0)
                                                {
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                if (type_is_enum(&new_expression->right->type))
                                                {
                                                    struct enum_specifier * p_enum_specifier;
                                                    unsigned int nelements;

                                                    p_enum_specifier = get_complete_enum_specifier(new_expression->right->type.enum_specifier);
                                                    nelements = 0;
                                                    if (p_enum_specifier)
                                                    {
                                                        struct enumerator * p;

                                                        p = p_enum_specifier->enumerator_list.head;
                                                        while (p)
                                                        {
                                                            nelements++;
                                                            p = p->next;
                                                        }
                                                    }
                                                    new_expression->object = object_make_size_t(ctx->options.target, nelements);
                                                }
                                                else
                                                {
                                                    if (type_is_array(&new_expression->right->type))
                                                    {
                                                        unsigned int nelements;

                                                        nelements = new_expression->right->type.num_of_elements;
                                                        if (nelements > 0)
                                                        {
                                                            new_expression->object = object_make_size_t(ctx->options.target, nelements);
                                                        }
                                                        else
                                                        {
                                                        }
                                                    }
                                                    else
                                                    {
                                                        compiler_diagnostic(1390, ctx, new_expression->right->first_token, 0, "argument of _Countof must be an array");
                                                        expression_delete(new_expression);
                                                        goto __L0; /* throw */
                                                    }
                                                }
                                            }
                                            type_destroy(&new_expression->type);
                                            new_expression->type = type_make_size_t(ctx->options.target);
                                            p_expression_node = new_expression;
                                        }
                                        else
                                        {
                                            if (ctx->current->type == 9069)
                                            {
                                                struct expression * new_expression;

                                                new_expression = calloc(1, 256);
                                                if (new_expression == 0)
                                                {
                                                    goto __L0; /* throw */
                                                }
                                                new_expression->expression_type = 31;
                                                new_expression->first_token = ctx->current;
                                                parser_match(ctx);
                                                if (ctx->current == 0 || parser_match_tk(ctx, 40) != 0)
                                                {
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                new_expression->right = expression(ctx, eval_mode);
                                                if (parser_match_tk(ctx, 41) != 0)
                                                {
                                                    expression_delete(new_expression);
                                                    goto __L0; /* throw */
                                                }
                                                return new_expression;
                                            }
                                            else
                                            {
                                                if (ctx->current->type == 9004)
                                                {
                                                    struct expression * new_expression;

                                                    new_expression = calloc(1, 256);
                                                    if (new_expression == 0)
                                                    {
                                                        goto __L0; /* throw */
                                                    }
                                                    new_expression->expression_type = 32;
                                                    new_expression->first_token = ctx->current;
                                                    parser_match(ctx);
                                                    if (ctx->current == 0 || parser_match_tk(ctx, 40) != 0)
                                                    {
                                                        expression_delete(new_expression);
                                                        goto __L0; /* throw */
                                                    }
                                                    new_expression->right = constant_expression(ctx, 1, eval_mode);
                                                    if (parser_match_tk(ctx, 41) != 0)
                                                    {
                                                        expression_delete(new_expression);
                                                        goto __L0; /* throw */
                                                    }
                                                    return new_expression;
                                                }
                                                else
                                                {
                                                    if (ctx->current->type == 9044)
                                                    {
                                                        struct expression * new_expression;

                                                        parser_match(ctx);
                                                        if (ctx->current == 0)
                                                        {
                                                            unexpected_end_of_file(ctx);
                                                            goto __L0; /* throw */
                                                        }
                                                        new_expression = calloc(1, 256);
                                                        if (new_expression == 0)
                                                        {
                                                            goto __L0; /* throw */
                                                        }
                                                        new_expression->first_token = ctx->current;
                                                        if (first_of_type_name_ahead(ctx))
                                                        {
                                                            new_expression->expression_type = 29;
                                                            if (parser_match_tk(ctx, 40) != 0)
                                                            {
                                                                expression_delete(new_expression);
                                                                goto __L0; /* throw */
                                                            }
                                                            new_expression->type_name = type_name(ctx);
                                                            if (new_expression->type_name == 0)
                                                            {
                                                                expression_delete(new_expression);
                                                                goto __L0; /* throw */
                                                            }
                                                            new_expression->type = make_size_t_type(ctx->options.target);
                                                            if (ctx->current == 0)
                                                            {
                                                                unexpected_end_of_file(ctx);
                                                                expression_delete(new_expression);
                                                                goto __L0; /* throw */
                                                            }
                                                            new_expression->last_token = ctx->current;
                                                            if (parser_match_tk(ctx, 41) != 0)
                                                            {
                                                                expression_delete(new_expression);
                                                                goto __L0; /* throw */
                                                            }
                                                            if (check_sizeof_argument(ctx, new_expression, &new_expression->type_name->type) != 0)
                                                            {
                                                            }
                                                            else
                                                            {
                                                                if (type_is_vla(&new_expression->type_name->abstract_declarator->type))
                                                                {
                                                                }
                                                                else
                                                                {
                                                                    unsigned int type_alignof;

                                                                    type_alignof = 0;
                                                                    type_alignof = type_get_alignof(&new_expression->type_name->abstract_declarator->type, ctx->options.target);
                                                                    new_expression->object = object_make_size_t(ctx->options.target, type_alignof);
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {
                                                            new_expression->right = unary_expression(ctx, 1);
                                                            if (new_expression->right == 0)
                                                            {
                                                                expression_delete(new_expression);
                                                                goto __L0; /* throw */
                                                            }
                                                            new_expression->expression_type = 30;
                                                            if (check_sizeof_argument(ctx, new_expression->right, &new_expression->right->type) != 0)
                                                            {
                                                                expression_delete(new_expression);
                                                                goto __L0; /* throw */
                                                            }
                                                            if (type_is_vla(&new_expression->right->type))
                                                            {
                                                            }
                                                            else
                                                            {
                                                                unsigned int sz;

                                                                sz = 0;
                                                                sz = type_get_alignof(&new_expression->right->type, ctx->options.target);
                                                                new_expression->object = object_make_size_t(ctx->options.target, sz);
                                                            }
                                                        }
                                                        type_destroy(&new_expression->type);
                                                        new_expression->type = type_make_size_t(ctx->options.target);
                                                        p_expression_node = new_expression;
                                                    }
                                                    else
                                                    {
                                                        if (ctx->current->type == 9087 || ctx->current->type == 9089 || ctx->current->type == 9088 || ctx->current->type == 9086 || ctx->current->type == 9090 || ctx->current->type == 9091 || ctx->current->type == 9093 || ctx->current->type == 9092 || ctx->current->type == 9094 || ctx->current->type == 9095)
                                                        {
                                                            struct token * traits_token;
                                                            struct expression * new_expression;
                                                            struct type * p_type;

                                                            traits_token = ctx->current;
                                                            new_expression = calloc(1, 256);
                                                            if (new_expression == 0)
                                                            {
                                                                goto __L0; /* throw */
                                                            }
                                                            new_expression->first_token = ctx->current;
                                                            new_expression->expression_type = 26;
                                                            parser_match(ctx);
                                                            if (ctx->current == 0)
                                                            {
                                                                unexpected_end_of_file(ctx);
                                                                expression_delete(new_expression);
                                                                goto __L0; /* throw */
                                                            }
                                                            p_type = 0;
                                                            if (first_of_type_name_ahead(ctx))
                                                            {
                                                                if (parser_match_tk(ctx, 40) != 0)
                                                                {
                                                                    expression_delete(new_expression);
                                                                    goto __L0; /* throw */
                                                                }
                                                                new_expression->type_name = type_name(ctx);
                                                                if (new_expression->type_name == 0)
                                                                {
                                                                    expression_delete(new_expression);
                                                                    goto __L0; /* throw */
                                                                }
                                                                if (ctx->current == 0)
                                                                {
                                                                    unexpected_end_of_file(ctx);
                                                                    expression_delete(new_expression);
                                                                    goto __L0; /* throw */
                                                                }
                                                                new_expression->last_token = ctx->current;
                                                                if (parser_match_tk(ctx, 41) != 0)
                                                                {
                                                                    expression_delete(new_expression);
                                                                    goto __L0; /* throw */
                                                                }
                                                                p_type = &new_expression->type_name->abstract_declarator->type;
                                                            }
                                                            else
                                                            {
                                                                new_expression->right = unary_expression(ctx, 1);
                                                                if (new_expression->right == 0)
                                                                {
                                                                    expression_delete(new_expression);
                                                                    goto __L0; /* throw */
                                                                }
                                                                p_type = &new_expression->right->type;
                                                                if (ctx->previous == 0)
                                                                {
                                                                    expression_delete(new_expression);
                                                                    goto __L0; /* throw */
                                                                }
                                                                new_expression->last_token = ctx->previous;
                                                            }
                                                            /*switch*/
                                                            {
                                                                int __v0 = traits_token->type;
                                                                if (__v0 == 9087) goto __L135; /*case 9087*/
                                                                if (__v0 == 9088) goto __L136; /*case 9088*/
                                                                if (__v0 == 9089) goto __L137; /*case 9089*/
                                                                if (__v0 == 9086) goto __L138; /*case 9086*/
                                                                if (__v0 == 9091) goto __L139; /*case 9091*/
                                                                if (__v0 == 9090) goto __L140; /*case 9090*/
                                                                if (__v0 == 9093) goto __L141; /*case 9093*/
                                                                if (__v0 == 9092) goto __L142; /*case 9092*/
                                                                if (__v0 == 9094) goto __L143; /*case 9094*/
                                                                if (__v0 == 9095) goto __L144; /*case 9095*/
                                                                goto __L145; /* default */

                                                                {
                                                                    __L135: /*case 9087*/ 
                                                                    ;
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, expression_is_lvalue(new_expression->right));
                                                                    goto __L134; /* break */

                                                                    __L136: /*case 9088*/ 
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, type_is_const(p_type));
                                                                    goto __L134; /* break */

                                                                    __L137: /*case 9089*/ 
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, type_is_owner(p_type));
                                                                    goto __L134; /* break */

                                                                    __L138: /*case 9086*/ 
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, type_is_pointer(p_type));
                                                                    goto __L134; /* break */

                                                                    __L139: /*case 9091*/ 
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, type_is_function(p_type));
                                                                    goto __L134; /* break */

                                                                    __L140: /*case 9090*/ 
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, type_is_array(p_type));
                                                                    goto __L134; /* break */

                                                                    __L141: /*case 9093*/ 
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, type_is_arithmetic(p_type));
                                                                    goto __L134; /* break */

                                                                    __L142: /*case 9092*/ 
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, type_is_scalar(p_type));
                                                                    goto __L134; /* break */

                                                                    __L143: /*case 9094*/ 
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, type_is_floating_point(p_type));
                                                                    goto __L134; /* break */

                                                                    __L144: /*case 9095*/ 
                                                                    new_expression->object = object_make_signed_int(ctx->options.target, type_is_integer(p_type));
                                                                    goto __L134; /* break */

                                                                    __L145: /* default */ 
                                                                    ;
                                                                }
                                                                __L134:;
                                                            }
                                                            new_expression->type = type_make_int_bool_like();
                                                            p_expression_node = new_expression;
                                                        }
                                                        else
                                                        {
                                                            p_expression_node = postfix_expression(ctx, eval_mode);
                                                            if (p_expression_node == 0)
                                                            {
                                                                goto __L0; /* throw */
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
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}



static int check_sizeof_argument(struct parser_ctx * ctx, struct expression * p_expression, struct type * p_type)
{
    int category;

    category = type_get_category(p_type);
    if (category == 1)
    {
    }
    else
    {
        if (category == 0 && p_type->type_specifier_flags & 32768)
        {
            struct struct_or_union_specifier * p_complete;

            ;
            p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
            if (p_complete == 0)
            {
                compiler_diagnostic(740, ctx, p_expression->first_token, 0, "struct is incomplete type");
                return -1;
            }
        }
        else
        {
            if (category == 2)
            {
                if (type_is_vla(p_type))
                {
                    return 0;
                }
                if (p_type->storage_class_specifier_flags & 2048)
                {
                    compiler_diagnostic(47, ctx, p_expression->first_token, 0, "sizeof applied to array function parameter");
                }
            }
        }
    }
    return 0;
}
unsigned char type_is_nullptr_t(struct type * p_type);
unsigned char type_is_bool(struct type * p_type);
unsigned char expression_is_null_pointer_constant(struct expression * expression);
int type_to_object_type(struct type * type, int target);
struct object object_cast(int target, int e, struct object * a);

struct expression *cast_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        if (first_of_type_name_ahead(ctx))
        {
            p_expression_node = calloc(1, 256);
            if (p_expression_node == 0)
            {
                goto __L0; /* throw */
            }
            p_expression_node->first_token = ctx->current;
            p_expression_node->expression_type = 41;
            if (parser_match_tk(ctx, 40) != 0)
            {
                goto __L0; /* throw */
            }
            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == 0)
            {
                expression_delete(p_expression_node);
                p_expression_node = 0;
                goto __L0; /* throw */
            }
            p_expression_node->type = type_dup(&p_expression_node->type_name->type);
            if (parser_match_tk(ctx, 41) != 0)
            {
                goto __L0; /* throw */
            }
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (ctx->current->type == 123)
            {
                struct expression * new_expression;

                new_expression = postfix_expression_type_name(ctx, p_expression_node->type_name, eval_mode);
                p_expression_node->type_name = 0;
                if (new_expression == 0)
                {
                    goto __L0; /* throw */
                }
                expression_delete(p_expression_node);
                p_expression_node = new_expression;
            }
            else
            {
                if (is_first_of_unary_expression(ctx))
                {
                    p_expression_node->left = cast_expression(ctx, eval_mode);
                    if (p_expression_node->left == 0)
                    {
                        expression_delete(p_expression_node);
                        p_expression_node = 0;
                        goto __L0; /* throw */
                    }
                    if (type_is_floating_point(&p_expression_node->type) && type_is_pointer(&p_expression_node->left->type))
                    {
                        compiler_diagnostic(1790, ctx, p_expression_node->first_token, 0, "pointer type cannot be converted to any floating type");
                    }
                    else
                    {
                        if (type_is_pointer(&p_expression_node->type) && type_is_floating_point(&p_expression_node->left->type))
                        {
                            compiler_diagnostic(1800, ctx, p_expression_node->first_token, 0, "A floating type cannot be converted to any pointer type");
                        }
                        else
                        {
                            if (type_is_nullptr_t(&p_expression_node->left->type))
                            {
                                if (type_is_void(&p_expression_node->type) || type_is_bool(&p_expression_node->type) || type_is_pointer(&p_expression_node->type))
                                {
                                }
                                else
                                {
                                    compiler_diagnostic(1810, ctx, p_expression_node->first_token, 0, "cannot cast nullptr_t to this type");
                                }
                            }
                            else
                            {
                                if (type_is_nullptr_t(&p_expression_node->type))
                                {
                                    if (expression_is_null_pointer_constant(p_expression_node->left) || type_is_nullptr_t(&p_expression_node->left->type))
                                    {
                                    }
                                    else
                                    {
                                        compiler_diagnostic(1810, ctx, p_expression_node->left->first_token, 0, "cannot cast this expression to nullptr_t");
                                    }
                                }
                            }
                        }
                    }
                    if (p_expression_node->left->type.storage_class_specifier_flags & 8192 && type_is_owner(&p_expression_node->left->type))
                    {
                        if (!type_is_owner(&p_expression_node->type))
                        {
                            if (type_is_pointer(&p_expression_node->left->type))
                            {
                                compiler_diagnostic(26, ctx, p_expression_node->first_token, 0, "discarding _Owner pointer");
                            }
                            else
                            {
                                compiler_diagnostic(26, ctx, p_expression_node->first_token, 0, "discarding _Owner");
                            }
                        }
                    }
                    type_destroy(&p_expression_node->type);
                    p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);
                    if (eval_mode == 2 && object_has_constant_value(&p_expression_node->left->object))
                    {
                        int vt;

                        vt = type_to_object_type(&p_expression_node->type, ctx->options.target);
                        p_expression_node->object = object_cast(ctx->options.target, vt, &p_expression_node->left->object);
                    }
                    p_expression_node->type.storage_class_specifier_flags = p_expression_node->left->type.storage_class_specifier_flags;
                }
                else
                {
                    compiler_diagnostic(650, ctx, ctx->current, 0, "expected expression");
                }
            }
        }
        else
        {
            if (is_first_of_unary_expression(ctx))
            {
                p_expression_node = unary_expression(ctx, eval_mode);
                if (p_expression_node == 0)
                {
                    goto __L0; /* throw */
                }
            }
            else
            {
                compiler_diagnostic(650, ctx, ctx->current, 0, "expected expression");
                ;
                goto __L0; /* throw */
            }
        }
        if (ctx->current == 0 || ctx->previous == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_expression_node->last_token = ctx->previous;
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


struct object object_add(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_sub(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_mul(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_div(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_mod(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_equal(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_not_equal(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_greater_than_or_equal(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_smaller_than_or_equal(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_greater_than(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_smaller_than(int target, struct object * a, struct object * b, char warning_message[200]);

int execute_arithmetic(struct parser_ctx * ctx, struct expression * new_expression, int op, struct object * result)
{
    struct object  empty;

    if (1) /*try*/
    {
        if (new_expression->left == 0 || new_expression->right == 0)
        {
            ;
            goto __L0; /* throw */
        }
        if (!type_is_arithmetic(&new_expression->left->type))
        {
            compiler_diagnostic(840, ctx, ctx->current, 0, "left type must be an arithmetic type");
            goto __L0; /* throw */
        }
        if (!type_is_arithmetic(&new_expression->right->type))
        {
            compiler_diagnostic(840, ctx, ctx->current, 0, "right type must be an arithmetic type");
            goto __L0; /* throw */
        }
        if (object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
        {
            struct marker  m;
            char warning_message[200];
            int warning_id;

            m.file = 0;
            m.line = 0;
            m.start_col = 0;
            m.end_col = 0;
            m.p_token_caret = 0;
            m.p_token_begin = new_expression->left->first_token;
            m.p_token_end = new_expression->right->last_token;
            _cake_zmem(&warning_message, 200);
            warning_id = 0;
            if (op == 43)
            {
                *result = object_add(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 51;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 45)
            {
                *result = object_sub(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 51;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 42)
            {
                *result = object_mul(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 51;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 47)
            {
                *result = object_div(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 37;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 37)
            {
                *result = object_mod(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 37;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 15677)
            {
                *result = object_equal(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 37;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 8509)
            {
                *result = object_not_equal(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 51;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 15933)
            {
                *result = object_greater_than_or_equal(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 51;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 15421)
            {
                *result = object_smaller_than_or_equal(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 51;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 62)
            {
                *result = object_greater_than(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 51;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
            if (op == 60)
            {
                *result = object_smaller_than(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                warning_id = 51;
                if (warning_message[0] != 0)
                {
                    compiler_diagnostic(warning_id, ctx, 0, &m, "%s", warning_message);
                }
                return 0;
            }
        }
    }
    else __L0: /*catch*/ 
    {
    }
    _cake_zmem(&empty, 120);
    *result = empty;
    return 0;
}


struct expression *multiplicative_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        p_expression_node = cast_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && (ctx->current->type == 42 || ctx->current->type == 47 || ctx->current->type == 37))
        {
            struct expression * new_expression;
            int op;

            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                expression_delete(p_expression_node);
                p_expression_node = 0;
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            /*switch*/
            {
                int __v0 = op;
                if (__v0 == 42) goto __L5; /*case 42*/
                if (__v0 == 47) goto __L6; /*case 47*/
                if (__v0 == 37) goto __L7; /*case 37*/
                goto __L8; /* default */

                {
                    __L5: /*case 42*/ 
                    new_expression->expression_type = 42;
                    goto __L4; /* break */

                    __L6: /*case 47*/ 
                    new_expression->expression_type = 43;
                    goto __L4; /* break */

                    __L7: /*case 37*/ 
                    new_expression->expression_type = 44;
                    goto __L4; /* break */

                    __L8: /* default */ 
                    ;
                    goto __L4; /* break */

                }
                __L4:;
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            new_expression->right = cast_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            new_expression->last_token = new_expression->right->last_token;
            if (op == 37)
            {
                if (!type_is_integer(&new_expression->left->type))
                {
                    compiler_diagnostic(840, ctx, new_expression->left->first_token, 0, "left is not an integer type");
                }
                if (!type_is_integer(&new_expression->right->type))
                {
                    compiler_diagnostic(850, ctx, new_expression->right->first_token, 0, "right is not an integer type");
                }
            }
            else
            {
                if (!type_is_arithmetic(&new_expression->left->type))
                {
                    compiler_diagnostic(820, ctx, new_expression->left->first_token, 0, "left is not an arithmetic type");
                }
                if (!type_is_arithmetic(&new_expression->right->type))
                {
                    compiler_diagnostic(830, ctx, new_expression->right->first_token, 0, "right is not an arithmetic type");
                }
            }
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);
            if (eval_mode == 2 && execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
            {
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            p_expression_node = new_expression;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


struct type type_make_ptrdiff_t(int target);

struct expression *additive_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        p_expression_node = multiplicative_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && (ctx->current->type == 43 || ctx->current->type == 45))
        {
            struct token * operator_position;
            struct expression * new_expression;
            int op;
            unsigned char  b_left_is_arithmetic;
            unsigned char  b_right_is_arithmetic;
            int left_category;
            int right_category;

            operator_position = ctx->current;
            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                compiler_diagnostic(1260, ctx, ctx->current, 0, "out of mem");
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            new_expression->right = multiplicative_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            new_expression->last_token = new_expression->right->last_token;
            if (!type_is_scalar(&new_expression->left->type) && !type_is_array(&new_expression->left->type))
            {
                compiler_diagnostic(870, ctx, operator_position, 0, "left operator is not scalar");
            }
            if (!type_is_scalar(&new_expression->right->type) && !type_is_array(&new_expression->right->type))
            {
                compiler_diagnostic(880, ctx, operator_position, 0, "right operator is not scalar");
            }
            b_left_is_arithmetic = type_is_arithmetic(&new_expression->left->type);
            b_right_is_arithmetic = type_is_arithmetic(&new_expression->right->type);
            left_category = type_get_category(&new_expression->left->type);
            right_category = type_get_category(&new_expression->right->type);
            if (op == 43)
            {
                new_expression->expression_type = 45;
                if (b_left_is_arithmetic && b_right_is_arithmetic)
                {
                    new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);
                    if (eval_mode == 2 && execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
                    {
                        expression_delete(new_expression);
                        goto __L0; /* throw */
                    }
                }
                else
                {
                    if (left_category == 3 || left_category == 2)
                    {
                        if (type_is_integer(&new_expression->right->type))
                        {
                            if (left_category == 2)
                            {
                                struct type  t;

                                t = get_array_item_type(&new_expression->left->type);
                                new_expression->type = type_add_pointer(&t, ctx->options.null_checks_enabled);
                                type_destroy(&t);
                            }
                            else
                            {
                                new_expression->type = type_dup(&new_expression->left->type);
                            }
                        }
                        else
                        {
                            compiler_diagnostic(850, ctx, ctx->current, 0, "expected integer type on right");
                        }
                    }
                    else
                    {
                        if (right_category == 3 || right_category == 2)
                        {
                            if (type_is_integer(&new_expression->left->type))
                            {
                                if (right_category == 2)
                                {
                                    new_expression->type = get_array_item_type(&new_expression->right->type);
                                }
                                else
                                {
                                    new_expression->type = type_dup(&new_expression->right->type);
                                }
                            }
                            else
                            {
                                compiler_diagnostic(840, ctx, ctx->current, 0, "expected integer type on left");
                            }
                        }
                        else
                        {
                            compiler_diagnostic(860, ctx, ctx->current, 0, "invalid types additive expression");
                        }
                    }
                }
            }
            else
            {
                if (op == 45)
                {
                    new_expression->expression_type = 46;
                    if (b_left_is_arithmetic && b_right_is_arithmetic)
                    {
                        new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);
                        if (eval_mode == 2 && execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
                        {
                            expression_delete(new_expression);
                            goto __L0; /* throw */
                        }
                    }
                    else
                    {
                        if (left_category == 3 || left_category == 2)
                        {
                            if (right_category == 3 || right_category == 2)
                            {
                                struct type  t1;
                                struct type  t2;

                                t1 = type_lvalue_conversion(&new_expression->left->type, ctx->options.null_checks_enabled);
                                t2 = type_lvalue_conversion(&new_expression->right->type, ctx->options.null_checks_enabled);
                                if (!type_is_same(&t1, &t2, 0))
                                {
                                    compiler_diagnostic(890, ctx, ctx->current, 0, "incompatible pointer types");
                                }
                                new_expression->type = type_make_ptrdiff_t(ctx->options.target);
                                type_destroy(&t1);
                                type_destroy(&t2);
                            }
                            else
                            {
                                if (type_is_integer(&new_expression->right->type))
                                {
                                    new_expression->type = type_dup(&new_expression->left->type);
                                }
                                else
                                {
                                    compiler_diagnostic(850, ctx, ctx->current, 0, "right must be integer type");
                                }
                            }
                        }
                        else
                        {
                            compiler_diagnostic(860, ctx, ctx->current, 0, "invalid types for operator -");
                        }
                    }
                }
            }
            p_expression_node = new_expression;
            new_expression = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


struct object object_shift_left(int target, struct object * a, struct object * b, char warning_message[200]);
struct object object_shift_right(int target, struct object * a, struct object * b, char warning_message[200]);

struct expression *shift_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        p_expression_node = additive_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && (ctx->current->type == 15934 || ctx->current->type == 15420))
        {
            struct expression * new_expression;
            int op;

            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = 0;
                goto __L0; /* throw */
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            new_expression->right = multiplicative_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            new_expression->last_token = new_expression->right->last_token;
            if (op == 15934)
            {
                new_expression->expression_type = 47;
            }
            else
            {
                if (op == 15420)
                {
                    new_expression->expression_type = 48;
                }
            }
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);
            if (!type_is_integer(&new_expression->left->type))
            {
                compiler_diagnostic(840, ctx, ctx->current, 0, "left type must be an integer type");
                goto __L0; /* throw */
            }
            if (!type_is_integer(&new_expression->right->type))
            {
                compiler_diagnostic(840, ctx, ctx->current, 0, "right type must be an integer type");
                goto __L0; /* throw */
            }
            if (object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
            {
                char warning_message[200];

                _cake_zmem(&warning_message, 200);
                if (op == 15420)
                {
                    new_expression->object = object_shift_left(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                }
                else
                {
                    new_expression->object = object_shift_right(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
                }
            }
            p_expression_node = new_expression;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


static void check_comparison(struct parser_ctx * ctx, struct expression * p_a_expression, struct expression * p_b_expression, struct token * op_token);
unsigned char expression_is_zero(struct expression * expression);
unsigned char type_is_essential_bool(struct type * p_type);
unsigned char object_is_zero(struct object * p_object);
unsigned char object_is_one(struct object * p_object);
void check_diferent_enuns(struct parser_ctx * ctx, struct token * operator_token, struct expression * left, struct expression * right, char * message);

struct expression *relational_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;
    struct expression * new_expression;

    p_expression_node = 0;
    new_expression = 0;
    if (1) /*try*/
    {
        p_expression_node = shift_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && (ctx->current->type == 62 || ctx->current->type == 60 || ctx->current->type == 15933 || ctx->current->type == 15421))
        {
            struct token * optk;
            int op;

            ;
            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            optk = ctx->current;
            op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            new_expression->right = shift_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                expression_delete(new_expression);
                new_expression = 0;
                goto __L0; /* throw */
            }
            new_expression->last_token = new_expression->right->last_token;
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = 0;
                goto __L0; /* throw */
            }
            check_comparison(ctx, new_expression->left, new_expression->right, optk);
            if (op == 62)
            {
                new_expression->expression_type = 49;
            }
            else
            {
                if (op == 60)
                {
                    new_expression->expression_type = 50;
                }
                else
                {
                    if (op == 15933)
                    {
                        new_expression->expression_type = 51;
                    }
                    else
                    {
                        if (op == 15421)
                        {
                            new_expression->expression_type = 52;
                        }
                    }
                }
            }
            if (type_is_arithmetic(&new_expression->left->type) && type_is_arithmetic(&new_expression->right->type))
            {
                new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);
                if (eval_mode == 2 && execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
                {
                    expression_delete(new_expression);
                    new_expression = 0;
                    goto __L0; /* throw */
                }
            }
            type_destroy(&new_expression->type);
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
            new_expression = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(new_expression);
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}



static void check_comparison(struct parser_ctx * ctx, struct expression * p_a_expression, struct expression * p_b_expression, struct token * op_token)
{
    unsigned char  equal_not_equal;
    struct type * p_a_type;
    struct type * p_b_type;

    equal_not_equal = op_token->type == 8509 || op_token->type == 15677;
    p_a_type = &p_a_expression->type;
    p_b_type = &p_b_expression->type;
    if (type_is_pointer(p_a_type) && type_is_integer(p_b_type))
    {
        if (expression_is_zero(p_b_expression))
        {
        }
        else
        {
            compiler_diagnostic(4, ctx, op_token, 0, "comparison between pointer and integer");
        }
    }
    if (type_is_bool(p_a_type) && !(type_is_bool(p_b_type) || type_is_essential_bool(p_b_type)))
    {
        if (equal_not_equal && (object_is_zero(&p_b_expression->object) || object_is_one(&p_b_expression->object)))
        {
        }
        else
        {
            compiler_diagnostic(58, ctx, op_token, 0, "comparison bool with non bool");
        }
    }
    if (type_is_bool(p_b_type) && !(type_is_bool(p_a_type) || type_is_essential_bool(p_a_type)))
    {
        if (equal_not_equal && (object_is_zero(&p_a_expression->object) || object_is_one(&p_a_expression->object)))
        {
        }
        else
        {
            compiler_diagnostic(58, ctx, op_token, 0, "comparison bool with non bool");
        }
    }
    check_diferent_enuns(ctx, op_token, p_a_expression, p_b_expression, "comparing different enums.");
}
static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;

void check_diferent_enuns(struct parser_ctx * ctx, struct token * operator_token, struct expression * left, struct expression * right, char * message)
{
    if (left->type.type_specifier_flags & 65536 && right->type.type_specifier_flags & 65536)
    {
        ;
        ;
        if (get_complete_enum_specifier(left->type.enum_specifier) != get_complete_enum_specifier(right->type.enum_specifier))
        {
            char * lefttag;
            char * righttag;
            char finalmessage[200];

            ;
            ;
            lefttag = "";
            if (left->type.enum_specifier->tag_token)
            {
                lefttag = left->type.enum_specifier->tag_token->lexeme;
            }
            righttag = "";
            if (right->type.enum_specifier->tag_token)
            {
                righttag = right->type.enum_specifier->tag_token->lexeme;
            }
            _cake_zmem(&finalmessage, 200);
            snprintf(finalmessage, 200, "%s (enum %s, enum %s)", message, lefttag, righttag);
            compiler_diagnostic(4, ctx, operator_token, 0, finalmessage, lefttag, righttag);
        }
    }
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
void expression_evaluate_equal_not_equal(struct expression * left, struct expression * right, struct expression * result, int op, unsigned char  disabled)
{
    ;
}


struct expression *equality_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;
    struct expression * new_expression;

    p_expression_node = 0;
    new_expression = 0;
    if (1) /*try*/
    {
        p_expression_node = relational_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && (ctx->current->type == 15677 || ctx->current->type == 8509))
        {
            struct token * op;
            struct token * operator_token;

            op = ctx->current;
            ;
            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            if (operator_token->type == 15677)
            {
                new_expression->expression_type = 53;
            }
            else
            {
                new_expression->expression_type = 54;
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            new_expression->right = relational_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                goto __L0; /* throw */
            }
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            check_comparison(ctx, new_expression->left, new_expression->right, op);
            new_expression->last_token = new_expression->right->last_token;
            new_expression->first_token = operator_token;
            if (type_is_arithmetic(&new_expression->left->type) && type_is_arithmetic(&new_expression->right->type))
            {
                if (eval_mode == 2 && execute_arithmetic(ctx, new_expression, operator_token->type, &new_expression->object) != 0)
                {
                    goto __L0; /* throw */
                }
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
            new_expression = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    expression_delete(new_expression);
    return p_expression_node;
}


struct object object_bitwise_and(int target, struct object * a, struct object * b, char warning_message[200]);

struct expression *and_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;
    struct expression * new_expression;

    p_expression_node = 0;
    new_expression = 0;
    if (1) /*try*/
    {
        p_expression_node = equality_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && ctx->current->type == 38)
        {
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            ;
            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 55;
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            new_expression->right = equality_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->last_token = new_expression->right->last_token;
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);
            if (!type_is_integer(&new_expression->left->type))
            {
                compiler_diagnostic(840, ctx, ctx->current, 0, "left type must be an integer type");
                goto __L0; /* throw */
            }
            if (!type_is_integer(&new_expression->right->type))
            {
                compiler_diagnostic(840, ctx, ctx->current, 0, "right type must be an integer type");
                goto __L0; /* throw */
            }
            if (object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
            {
                char warning_message[200];

                _cake_zmem(&warning_message, 200);
                new_expression->object = object_bitwise_and(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
            }
            p_expression_node = new_expression;
            new_expression = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    expression_delete(new_expression);
    return p_expression_node;
}


struct object object_bitwise_xor(int target, struct object * a, struct object * b, char warning_message[200]);

struct expression *exclusive_or_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;
    struct expression * new_expression;

    p_expression_node = 0;
    new_expression = 0;
    if (1) /*try*/
    {
        p_expression_node = and_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && (ctx->current->type == 94))
        {
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            ;
            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 56;
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            new_expression->right = and_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->last_token = new_expression->right->last_token;
            if (!type_is_integer(&new_expression->left->type))
            {
                compiler_diagnostic(840, ctx, ctx->current, 0, "left type must be an integer type");
                goto __L0; /* throw */
            }
            if (!type_is_integer(&new_expression->right->type))
            {
                compiler_diagnostic(840, ctx, ctx->current, 0, "right type must be an integer type");
                goto __L0; /* throw */
            }
            if (object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
            {
                char warning_message[200];

                _cake_zmem(&warning_message, 200);
                new_expression->object = object_bitwise_xor(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
            }
            p_expression_node = new_expression;
            new_expression = 0;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    expression_delete(new_expression);
    return p_expression_node;
}


struct object object_bitwise_or(int target, struct object * a, struct object * b, char warning_message[200]);

struct expression *inclusive_or_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        p_expression_node = exclusive_or_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && (ctx->current->type == 124))
        {
            struct token * operator_token;
            struct expression * new_expression;

            operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 57;
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            new_expression->right = exclusive_or_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            check_diferent_enuns(ctx, operator_token, new_expression->left, new_expression->right, "operator '|' between enumerations of different types.");
            new_expression->last_token = new_expression->right->last_token;
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type, ctx->options.target);
            if (!type_is_integer(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(840, ctx, ctx->current, 0, "left type must be an integer type");
                goto __L0; /* throw */
            }
            if (!type_is_integer(&new_expression->right->type))
            {
                compiler_diagnostic(840, ctx, ctx->current, 0, "right type must be an integer type");
                goto __L0; /* throw */
            }
            if (object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
            {
                char warning_message[200];

                _cake_zmem(&warning_message, 200);
                new_expression->object = object_bitwise_or(ctx->options.target, &new_expression->left->object, &new_expression->right->object, warning_message);
            }
            p_expression_node = new_expression;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


unsigned char object_is_true(struct object * a);

struct expression *logical_and_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        p_expression_node = inclusive_or_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        while (ctx->current != 0 && (ctx->current->type == 9766))
        {
            struct expression * new_expression;
            unsigned char  right_evaluation_is_disabled;

            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 59;
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            right_evaluation_is_disabled = 0;
            if (object_has_constant_value(&new_expression->left->object))
            {
                if (!object_is_true(&new_expression->left->object))
                {
                    right_evaluation_is_disabled = 1;
                }
            }
            new_expression->right = inclusive_or_expression(ctx, right_evaluation_is_disabled ? 1 : 2);
            if (new_expression->right == 0)
            {
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            new_expression->last_token = new_expression->right->last_token;
            if (eval_mode == 2)
            {
                if (object_has_constant_value(&new_expression->left->object))
                {
                    unsigned char  a;

                    a = object_is_true(&new_expression->left->object);
                    if (a == 0)
                    {
                        new_expression->object = object_make_signed_int(ctx->options.target, 0);
                    }
                    else
                    {
                        if (object_has_constant_value(&new_expression->right->object))
                        {
                            unsigned char  b;

                            b = object_is_true(&new_expression->right->object);
                            new_expression->object = object_make_signed_int(ctx->options.target, a && b);
                        }
                    }
                }
            }
            if (!type_is_scalar(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(870, ctx, ctx->current, 0, "left type is not scalar for or expression");
                goto __L0; /* throw */
            }
            if (!type_is_scalar(&new_expression->right->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(880, ctx, ctx->current, 0, "right type is not scalar for or expression");
                goto __L0; /* throw */
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


struct expression *logical_or_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        int current_eval_mode;

        p_expression_node = logical_and_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        current_eval_mode = eval_mode;
        while (ctx->current != 0 && (ctx->current->type == 31868))
        {
            struct expression * new_expression;
            unsigned char  right_evaluation_is_disabled;

            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 58;
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            right_evaluation_is_disabled = 0;
            if (object_has_constant_value(&new_expression->left->object))
            {
                if (object_is_true(&new_expression->left->object))
                {
                    right_evaluation_is_disabled = 1;
                }
            }
            /*switch*/
            {
                int __v0 = current_eval_mode;
                if (__v0 == 0) goto __L7; /*case 0*/
                if (__v0 == 1) goto __L8; /*case 1*/
                if (__v0 == 2) goto __L9; /*case 2*/
                goto __L6;

                {
                    __L7: /*case 0*/ 
                    __L8: /*case 1*/ 
                    goto __L6; /* break */

                    __L9: /*case 2*/ 
                    if (right_evaluation_is_disabled)
                    {
                        current_eval_mode = 1;
                    }
                    goto __L6; /* break */

                }
                __L6:;
            }
            new_expression->right = logical_and_expression(ctx, current_eval_mode);
            if (new_expression->right == 0)
            {
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            new_expression->last_token = new_expression->right->last_token;
            if (eval_mode == 2)
            {
                if (object_has_constant_value(&new_expression->left->object))
                {
                    unsigned char  a;

                    a = object_is_true(&new_expression->left->object);
                    if (a == 1)
                    {
                        new_expression->object = object_make_signed_int(ctx->options.target, 1);
                    }
                    else
                    {
                        if (object_has_constant_value(&new_expression->right->object))
                        {
                            unsigned char  b;

                            b = object_is_true(&new_expression->right->object);
                            new_expression->object = object_make_signed_int(ctx->options.target, a || b);
                        }
                    }
                }
            }
            if (!type_is_scalar(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(870, ctx, ctx->current, 0, "left type is not scalar for or expression");
                goto __L0; /* throw */
            }
            if (!type_is_scalar(&new_expression->right->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(880, ctx, ctx->current, 0, "right type is not scalar for or expression");
                goto __L0; /* throw */
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


struct expression *conditional_expression(struct parser_ctx * ctx, int eval_mode);

struct expression *assignment_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        p_expression_node = conditional_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        ;
        while (ctx->current != 0 && (ctx->current->type == 61 || ctx->current->type == 10813 || ctx->current->type == 12093 || ctx->current->type == 9533 || ctx->current->type == 11069 || ctx->current->type == 11581 || ctx->current->type == 3947581 || ctx->current->type == 4079165 || ctx->current->type == 9789 || ctx->current->type == 24125 || ctx->current->type == 31805))
        {
            struct token * op_token;
            struct expression * new_expression;
            struct marker  left_operand_marker;

            op_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                goto __L0; /* throw */
            }
            new_expression = calloc(1, 256);
            if (new_expression == 0)
            {
                goto __L0; /* throw */
            }
            new_expression->first_token = ctx->current;
            /*switch*/
            {
                int __v0 = op_token->type;
                if (__v0 == 61) goto __L5; /*case 61*/
                if (__v0 == 10813) goto __L6; /*case 10813*/
                if (__v0 == 12093) goto __L7; /*case 12093*/
                if (__v0 == 9533) goto __L8; /*case 9533*/
                if (__v0 == 11069) goto __L9; /*case 11069*/
                if (__v0 == 11581) goto __L10; /*case 11581*/
                if (__v0 == 3947581) goto __L11; /*case 3947581*/
                if (__v0 == 4079165) goto __L12; /*case 4079165*/
                if (__v0 == 9789) goto __L13; /*case 9789*/
                if (__v0 == 24125) goto __L14; /*case 24125*/
                if (__v0 == 31805) goto __L15; /*case 31805*/
                goto __L16; /* default */

                {
                    __L5: /*case 61*/ 
                    new_expression->expression_type = 60;
                    goto __L4; /* break */

                    __L6: /*case 10813*/ 
                    new_expression->expression_type = 63;
                    goto __L4; /* break */

                    __L7: /*case 12093*/ 
                    new_expression->expression_type = 64;
                    goto __L4; /* break */

                    __L8: /*case 9533*/ 
                    new_expression->expression_type = 65;
                    goto __L4; /* break */

                    __L9: /*case 11069*/ 
                    new_expression->expression_type = 61;
                    goto __L4; /* break */

                    __L10: /*case 11581*/ 
                    new_expression->expression_type = 62;
                    goto __L4; /* break */

                    __L11: /*case 3947581*/ 
                    new_expression->expression_type = 66;
                    goto __L4; /* break */

                    __L12: /*case 4079165*/ 
                    new_expression->expression_type = 67;
                    goto __L4; /* break */

                    __L13: /*case 9789*/ 
                    new_expression->expression_type = 68;
                    goto __L4; /* break */

                    __L14: /*case 24125*/ 
                    new_expression->expression_type = 70;
                    goto __L4; /* break */

                    __L15: /*case 31805*/ 
                    new_expression->expression_type = 69;
                    goto __L4; /* break */

                    __L16: /* default */ 
                    ;
                    goto __L4; /* break */

                }
                __L4:;
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0;
            left_operand_marker.file = 0;
            left_operand_marker.line = 0;
            left_operand_marker.start_col = 0;
            left_operand_marker.end_col = 0;
            left_operand_marker.p_token_caret = 0;
            left_operand_marker.p_token_begin = new_expression->left->first_token;
            left_operand_marker.p_token_end = new_expression->left->last_token;
            if (type_is_function(&new_expression->left->type))
            {
                compiler_diagnostic(900, ctx, 0, &left_operand_marker, "assignment of function");
            }
            else
            {
                if (type_is_array(&new_expression->left->type))
                {
                    if (new_expression->left->type.storage_class_specifier_flags & 2048)
                    {
                        compiler_diagnostic(43, ctx, 0, &left_operand_marker, "assignment to array parameter");
                    }
                    else
                    {
                        compiler_diagnostic(910, ctx, 0, &left_operand_marker, "assignment to expression with array type");
                    }
                }
            }
            if (type_is_const(&new_expression->left->type))
            {
                compiler_diagnostic(920, ctx, 0, &left_operand_marker, "assignment of read-only object");
            }
            if (!expression_is_lvalue(new_expression->left))
            {
                compiler_diagnostic(1230, ctx, 0, &left_operand_marker, "lvalue required as left operand of assignment");
            }
            new_expression->right = assignment_expression(ctx, eval_mode);
            if (new_expression->right == 0)
            {
                expression_delete(new_expression);
                goto __L0; /* throw */
            }
            if (op_token->type == 61)
            {
                check_assigment(ctx, &new_expression->left->type, new_expression->right, 2);
            }
            new_expression->last_token = new_expression->right->last_token;
            new_expression->type = type_dup(&new_expression->left->type);
            new_expression->type.storage_class_specifier_flags &= -8193;
            new_expression->type.storage_class_specifier_flags &= -16385;
            check_diferent_enuns(ctx, op_token, new_expression->left, new_expression->right, "assignment of different enums.");
            new_expression->left->is_assignment_expression = 1;
            if (new_expression->left->left)
            {
                new_expression->left->left->is_assignment_expression = 1;
            }
            p_expression_node = new_expression;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


void argument_expression_list_push(struct argument_expression_list * list, struct argument_expression * pitem)
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


void argument_expression_list_destroy(struct argument_expression_list * p)
{
    struct argument_expression * item;

    item = p->head;
    while (item)
    {
        struct argument_expression * next;

        next = item->next;
        item->next = 0;
        argument_expression_delete(item);
        item = next;
    }
}


unsigned char expression_is_malloc(struct expression * p)
{
    if (p->expression_type == 12 && p->left && p->left->declarator && p->left->declarator->name_opt)
    {
        if (strcmp(p->left->declarator->name_opt->lexeme, "malloc") == 0)
        {
            return 1;
        }
    }
    return 0;
}


unsigned char expression_is_calloc(struct expression * p)
{
    if (p->expression_type == 12 && p->left && p->left->declarator && p->left->declarator->name_opt)
    {
        if (strcmp(p->left->declarator->name_opt->lexeme, "calloc") == 0)
        {
            return 1;
        }
    }
    return 0;
}


void compound_statement_delete(struct compound_statement * p);
void braced_initializer_delete(struct braced_initializer * p);

void expression_delete(struct expression * p)
{
    if (p)
    {
        expression_delete(p->condition_expr);
        compound_statement_delete(p->compound_statement);
        type_name_delete(p->type_name);
        expression_delete(p->right);
        expression_delete(p->left);
        braced_initializer_delete(p->braced_initializer);
        generic_selection_delete(p->generic_selection);
        type_destroy(&p->type);
        argument_expression_list_destroy(&p->argument_expression_list);
        free(p);
    }
}


struct expression *expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;

    p_expression_node = 0;
    if (1) /*try*/
    {
        if (ctx->current == 0)
        {
            unexpected_end_of_file(ctx);
            goto __L0; /* throw */
        }
        p_expression_node = assignment_expression(ctx, eval_mode);
        if (p_expression_node == 0)
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
            while (ctx->current->type == 44)
            {
                struct expression * p_expression_node_new;

                parser_match(ctx);
                if (ctx->current == 0)
                {
                    unexpected_end_of_file(ctx);
                    goto __L0; /* throw */
                }
                p_expression_node_new = calloc(1, 256);
                if (p_expression_node_new == 0)
                {
                    goto __L0; /* throw */
                }
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = 71;
                p_expression_node_new->left = p_expression_node;
                p_expression_node = 0;
                p_expression_node_new->right = expression(ctx, eval_mode);
                if (p_expression_node_new->right == 0)
                {
                    expression_delete(p_expression_node_new);
                    goto __L0; /* throw */
                }
                p_expression_node_new->left->last_token = p_expression_node_new->right->last_token;
                p_expression_node = p_expression_node_new;
                if (ctx->current == 0)
                {
                    unexpected_end_of_file(ctx);
                    goto __L0; /* throw */
                }
            }
            type_destroy(&p_expression_node->type);
            p_expression_node->type = type_dup(&p_expression_node->right->type);
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    return p_expression_node;
}


unsigned char is_first_of_conditional_expression(struct parser_ctx * ctx)
{
    return is_first_of_unary_expression(ctx) || is_first_of_primary_expression(ctx);
}


unsigned char expression_is_one(struct expression * expression)
{
    if (expression->expression_type == 8)
    {
        return (object_has_constant_value(&expression->object) && object_is_one(&expression->object));
    }
    return 0;
}


unsigned char expression_is_zero(struct expression * expression)
{
    if (expression->expression_type == 8)
    {
        return (object_has_constant_value(&expression->object) && object_is_zero(&expression->object));
    }
    return 0;
}


unsigned char type_is_void_ptr(struct type * p_type);

unsigned char expression_is_null_pointer_constant(struct expression * expression)
{
    if (type_is_integer(&expression->type) && object_has_constant_value(&expression->object) && object_is_zero(&expression->object))
    {
        return 1;
    }
    if (type_is_void_ptr(&expression->type) && object_has_constant_value(&expression->object) && object_is_zero(&expression->object))
    {
        return 1;
    }
    if (type_is_nullptr_t(&expression->type))
    {
        return 1;
    }
    return 0;
}


void type_swap(struct type * a, struct type * b);

struct expression *conditional_expression(struct parser_ctx * ctx, int eval_mode)
{
    struct expression * p_expression_node;
    struct type  left_type;
    struct type  right_type;

    p_expression_node = 0;
    _cake_zmem(&left_type, 68);
    _cake_zmem(&right_type, 68);
    if (1) /*try*/
    {
        int current_eval_mode;

        p_expression_node = logical_or_expression(ctx, eval_mode);
        if (p_expression_node == 0)
        {
            goto __L0; /* throw */
        }
        current_eval_mode = eval_mode;
        if (ctx->current && ctx->current->type == 63)
        {
            struct expression * p_conditional_expression;
            unsigned char  constant_expression_is_true;
            unsigned char  has_constant_expression;
            struct expression * p_left;
            int new_eval_mode;
            struct expression * p_right;

            p_conditional_expression = calloc(1, 256);
            if (p_conditional_expression == 0)
            {
                goto __L0; /* throw */
            }
            p_conditional_expression->first_token = ctx->current;
            p_conditional_expression->expression_type = 72;
            p_conditional_expression->condition_expr = p_expression_node;
            p_expression_node = 0;
            parser_match(ctx);
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                goto __L0; /* throw */
            }
            constant_expression_is_true = 0;
            has_constant_expression = 0;
            if (object_has_constant_value(&p_conditional_expression->condition_expr->object))
            {
                has_constant_expression = 1;
                if (object_is_true(&p_conditional_expression->condition_expr->object))
                {
                    constant_expression_is_true = 1;
                }
            }
            /*switch*/
            {
                int __v0 = current_eval_mode;
                if (__v0 == 0) goto __L8; /*case 0*/
                if (__v0 == 1) goto __L9; /*case 1*/
                if (__v0 == 2) goto __L10; /*case 2*/
                goto __L7;

                {
                    __L8: /*case 0*/ 
                    __L9: /*case 1*/ 
                    goto __L7; /* break */

                    __L10: /*case 2*/ 
                    current_eval_mode = has_constant_expression && !constant_expression_is_true;
                    goto __L7; /* break */

                }
                __L7:;
            }
            p_left = expression(ctx, current_eval_mode);
            if (p_left == 0)
            {
                expression_delete(p_conditional_expression);
                goto __L0; /* throw */
            }
            p_conditional_expression->left = p_left;
            if (parser_match_tk(ctx, 58) != 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                goto __L0; /* throw */
            }
            if (ctx->current == 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                goto __L0; /* throw */
            }
            new_eval_mode = 2;
            /*switch*/
            {
                int __v1 = eval_mode;
                if (__v1 == 0) goto __L15; /*case 0*/
                if (__v1 == 1) goto __L16; /*case 1*/
                if (__v1 == 2) goto __L17; /*case 2*/
                goto __L14;

                {
                    __L15: /*case 0*/ 
                    __L16: /*case 1*/ 
                    new_eval_mode = eval_mode;
                    goto __L14; /* break */

                    __L17: /*case 2*/ 
                    new_eval_mode = eval_mode;
                    if (has_constant_expression && constant_expression_is_true)
                    {
                        new_eval_mode = 1;
                    }
                    goto __L14; /* break */

                }
                __L14:;
            }
            p_right = conditional_expression(ctx, new_eval_mode);
            if (p_right == 0)
            {
                expression_delete(p_conditional_expression);
                goto __L0; /* throw */
            }
            p_conditional_expression->right = p_right;
            if (object_has_constant_value(&p_conditional_expression->condition_expr->object))
            {
                if (object_is_true(&p_conditional_expression->condition_expr->object))
                {
                    p_conditional_expression->object = object_make_reference(&p_conditional_expression->left->object);
                }
                else
                {
                    p_conditional_expression->object = object_make_reference(&p_conditional_expression->right->object);
                }
            }
            if (expression_is_subjected_to_lvalue_conversion(p_conditional_expression->left))
            {
                left_type = type_lvalue_conversion(&p_conditional_expression->left->type, ctx->options.null_checks_enabled);
            }
            else
            {
                left_type = type_dup(&p_conditional_expression->left->type);
            }
            if (expression_is_subjected_to_lvalue_conversion(p_conditional_expression->right))
            {
                right_type = type_lvalue_conversion(&p_conditional_expression->right->type, ctx->options.null_checks_enabled);
            }
            else
            {
                right_type = type_dup(&p_conditional_expression->right->type);
            }
            if (!type_is_scalar(&p_conditional_expression->condition_expr->type))
            {
                compiler_diagnostic(940, ctx, ctx->current, 0, "condition must have scalar type");
            }
            else
            {
                if (type_is_arithmetic(&left_type) && type_is_arithmetic(&right_type))
                {
                    type_destroy(&p_conditional_expression->type);
                    p_conditional_expression->type = type_common(&left_type, &right_type, ctx->options.target);
                }
                else
                {
                    if (type_is_struct_or_union(&left_type) && type_is_struct_or_union(&right_type))
                    {
                        if (!type_is_same(&left_type, &right_type, 1))
                        {
                            compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0, "incompatible types");
                        }
                        type_swap(&p_conditional_expression->type, &right_type);
                    }
                    else
                    {
                        if (type_is_void(&left_type) && type_is_void(&right_type))
                        {
                            type_swap(&p_conditional_expression->type, &right_type);
                        }
                        else
                        {
                            if (type_is_nullptr_t(&left_type) && type_is_nullptr_t(&right_type))
                            {
                                type_swap(&p_conditional_expression->type, &right_type);
                            }
                            else
                            {
                                if (type_is_pointer(&left_type))
                                {
                                    if (expression_is_null_pointer_constant(p_conditional_expression->right) || type_is_nullptr_t(&right_type) || type_is_void_ptr(&right_type))
                                    {
                                        type_swap(&p_conditional_expression->type, &left_type);
                                    }
                                    else
                                    {
                                        if (type_is_pointer(&right_type))
                                        {
                                            if (type_is_nullptr_t(&left_type) || type_is_void_ptr(&left_type))
                                            {
                                                type_swap(&p_conditional_expression->type, &left_type);
                                            }
                                            else
                                            {
                                                if (!type_is_same(&left_type, &right_type, 0))
                                                {
                                                    compiler_diagnostic(950, ctx, ctx->current, 0, "incompatible types");
                                                }
                                                else
                                                {
                                                    type_swap(&p_conditional_expression->type, &right_type);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0, "incompatible types");
                                        }
                                    }
                                }
                                else
                                {
                                    if (type_is_pointer(&right_type))
                                    {
                                        if (expression_is_null_pointer_constant(p_conditional_expression->left) || type_is_nullptr_t(&left_type) || type_is_void_ptr(&left_type))
                                        {
                                            type_swap(&p_conditional_expression->type, &right_type);
                                        }
                                        else
                                        {
                                            if (type_is_pointer(&left_type))
                                            {
                                                if (type_is_nullptr_t(&left_type) || type_is_void_ptr(&left_type))
                                                {
                                                    type_swap(&p_conditional_expression->type, &right_type);
                                                }
                                                else
                                                {
                                                    if (!type_is_same(&left_type, &right_type, 0))
                                                    {
                                                        compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0, "incompatible types");
                                                    }
                                                    else
                                                    {
                                                        type_swap(&p_conditional_expression->type, &right_type);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0, "incompatible types");
                                            }
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0, "incompatible types");
                                    }
                                }
                            }
                        }
                    }
                }
            }
            p_expression_node = p_conditional_expression;
        }
    }
    else __L0: /*catch*/ 
    {
        expression_delete(p_expression_node);
        p_expression_node = 0;
    }
    type_destroy(&left_type);
    type_destroy(&right_type);
    return p_expression_node;
}


struct expression *constant_expression(struct parser_ctx * ctx, unsigned char  show_error_if_not_constant, int eval_mode)
{
    struct expression * p_expression;

    p_expression = conditional_expression(ctx, eval_mode);
    if (show_error_if_not_constant && p_expression && !object_has_constant_value(&p_expression->object))
    {
        compiler_diagnostic(960, ctx, ctx->current, 0, "expected constant expression");
    }
    return p_expression;
}


struct object *object_get_non_const_referenced(struct object * p_object);

unsigned char expression_get_variables(struct expression * expr, int n, struct object *variables[])
{
    int count;

    count = 0;
    /*switch*/
    {
        int __v0 = expr->expression_type;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 1) goto __L2; /*case 1*/
        if (__v0 == 2) goto __L3; /*case 2*/
        if (__v0 == 3) goto __L6; /*case 3*/
        if (__v0 == 4) goto __L7; /*case 4*/
        if (__v0 == 5) goto __L8; /*case 5*/
        if (__v0 == 6) goto __L9; /*case 6*/
        if (__v0 == 7) goto __L10; /*case 7*/
        if (__v0 == 8) goto __L11; /*case 8*/
        if (__v0 == 9) goto __L12; /*case 9*/
        if (__v0 == 10) goto __L13; /*case 10*/
        if (__v0 == 11) goto __L14; /*case 11*/
        if (__v0 == 12) goto __L15; /*case 12*/
        if (__v0 == 13) goto __L16; /*case 13*/
        if (__v0 == 14) goto __L17; /*case 14*/
        if (__v0 == 15) goto __L18; /*case 15*/
        if (__v0 == 16) goto __L19; /*case 16*/
        if (__v0 == 17) goto __L20; /*case 17*/
        if (__v0 == 18) goto __L21; /*case 18*/
        if (__v0 == 19) goto __L22; /*case 19*/
        if (__v0 == 20) goto __L23; /*case 20*/
        if (__v0 == 26) goto __L24; /*case 26*/
        if (__v0 == 27) goto __L25; /*case 27*/
        if (__v0 == 28) goto __L26; /*case 28*/
        if (__v0 == 29) goto __L27; /*case 29*/
        if (__v0 == 30) goto __L28; /*case 30*/
        if (__v0 == 31) goto __L29; /*case 31*/
        if (__v0 == 33) goto __L30; /*case 33*/
        if (__v0 == 34) goto __L31; /*case 34*/
        if (__v0 == 35) goto __L32; /*case 35*/
        if (__v0 == 36) goto __L33; /*case 36*/
        if (__v0 == 37) goto __L34; /*case 37*/
        if (__v0 == 38) goto __L35; /*case 38*/
        if (__v0 == 39) goto __L36; /*case 39*/
        if (__v0 == 40) goto __L37; /*case 40*/
        if (__v0 == 41) goto __L38; /*case 41*/
        if (__v0 == 42) goto __L39; /*case 42*/
        if (__v0 == 43) goto __L40; /*case 43*/
        if (__v0 == 44) goto __L41; /*case 44*/
        if (__v0 == 45) goto __L42; /*case 45*/
        if (__v0 == 46) goto __L43; /*case 46*/
        if (__v0 == 47) goto __L44; /*case 47*/
        if (__v0 == 48) goto __L45; /*case 48*/
        if (__v0 == 49) goto __L46; /*case 49*/
        if (__v0 == 50) goto __L47; /*case 50*/
        if (__v0 == 51) goto __L48; /*case 51*/
        if (__v0 == 52) goto __L49; /*case 52*/
        if (__v0 == 53) goto __L50; /*case 53*/
        if (__v0 == 54) goto __L51; /*case 54*/
        if (__v0 == 55) goto __L52; /*case 55*/
        if (__v0 == 56) goto __L53; /*case 56*/
        if (__v0 == 57) goto __L54; /*case 57*/
        if (__v0 == 58) goto __L55; /*case 58*/
        if (__v0 == 59) goto __L56; /*case 59*/
        if (__v0 == 60) goto __L57; /*case 60*/
        if (__v0 == 61) goto __L58; /*case 61*/
        if (__v0 == 62) goto __L59; /*case 62*/
        if (__v0 == 63) goto __L60; /*case 63*/
        if (__v0 == 64) goto __L61; /*case 64*/
        if (__v0 == 65) goto __L62; /*case 65*/
        if (__v0 == 66) goto __L63; /*case 66*/
        if (__v0 == 67) goto __L64; /*case 67*/
        if (__v0 == 68) goto __L65; /*case 68*/
        if (__v0 == 69) goto __L66; /*case 69*/
        if (__v0 == 70) goto __L67; /*case 70*/
        if (__v0 == 71) goto __L68; /*case 71*/
        if (__v0 == 72) goto __L69; /*case 72*/
        if (__v0 == 21) goto __L70; /*case 21*/
        if (__v0 == 22) goto __L71; /*case 22*/
        if (__v0 == 23) goto __L72; /*case 23*/
        if (__v0 == 24) goto __L73; /*case 24*/
        if (__v0 == 25) goto __L74; /*case 25*/
        if (__v0 == 32) goto __L75; /*case 32*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            goto __L0; /* break */

            __L2: /*case 1*/ 
            goto __L0; /* break */

            __L3: /*case 2*/ 
            if (!object_has_constant_value(&expr->object))
            {
                if (count < n)
                {
                    variables[count] = object_get_non_const_referenced(&expr->object);
                    count++;
                }
            }
            goto __L0; /* break */

            __L6: /*case 3*/ 
            goto __L0; /* break */

            __L7: /*case 4*/ 
            goto __L0; /* break */

            __L8: /*case 5*/ 
            goto __L0; /* break */

            __L9: /*case 6*/ 
            goto __L0; /* break */

            __L10: /*case 7*/ 
            goto __L0; /* break */

            __L11: /*case 8*/ 
            goto __L0; /* break */

            __L12: /*case 9*/ 
            ;
            count += expression_get_variables(expr->right, n, variables);
            goto __L0; /* break */

            __L13: /*case 10*/ 
            goto __L0; /* break */

            __L14: /*case 11*/ 
            goto __L0; /* break */

            __L15: /*case 12*/ 
            goto __L0; /* break */

            __L16: /*case 13*/ 
            goto __L0; /* break */

            __L17: /*case 14*/ 
            goto __L0; /* break */

            __L18: /*case 15*/ 
            goto __L0; /* break */

            __L19: /*case 16*/ 
            goto __L0; /* break */

            __L20: /*case 17*/ 
            goto __L0; /* break */

            __L21: /*case 18*/ 
            goto __L0; /* break */

            __L22: /*case 19*/ 
            goto __L0; /* break */

            __L23: /*case 20*/ 
            goto __L0; /* break */

            __L24: /*case 26*/ 
            goto __L0; /* break */

            __L25: /*case 27*/ 
            goto __L0; /* break */

            __L26: /*case 28*/ 
            goto __L0; /* break */

            __L27: /*case 29*/ 
            goto __L0; /* break */

            __L28: /*case 30*/ 
            goto __L0; /* break */

            __L29: /*case 31*/ 
            goto __L0; /* break */

            __L30: /*case 33*/ 
            goto __L0; /* break */

            __L31: /*case 34*/ 
            goto __L0; /* break */

            __L32: /*case 35*/ 
            goto __L0; /* break */

            __L33: /*case 36*/ 
            goto __L0; /* break */

            __L34: /*case 37*/ 
            goto __L0; /* break */

            __L35: /*case 38*/ 
            goto __L0; /* break */

            __L36: /*case 39*/ 
            goto __L0; /* break */

            __L37: /*case 40*/ 
            goto __L0; /* break */

            __L38: /*case 41*/ 
            goto __L0; /* break */

            __L39: /*case 42*/ 
            __L40: /*case 43*/ 
            __L41: /*case 44*/ 
            ;
            ;
            count += expression_get_variables(expr->left, n, variables);
            count += expression_get_variables(expr->right, n, variables);
            goto __L0; /* break */

            __L42: /*case 45*/ 
            __L43: /*case 46*/ 
            ;
            ;
            count += expression_get_variables(expr->left, n, variables);
            count += expression_get_variables(expr->right, n, variables);
            goto __L0; /* break */

            __L44: /*case 47*/ 
            __L45: /*case 48*/ 
            __L46: /*case 49*/ 
            __L47: /*case 50*/ 
            __L48: /*case 51*/ 
            __L49: /*case 52*/ 
            __L50: /*case 53*/ 
            __L51: /*case 54*/ 
            ;
            ;
            count += expression_get_variables(expr->left, n, variables);
            count += expression_get_variables(expr->right, n, variables);
            goto __L0; /* break */

            __L52: /*case 55*/ 
            goto __L0; /* break */

            __L53: /*case 56*/ 
            goto __L0; /* break */

            __L54: /*case 57*/ 
            goto __L0; /* break */

            __L55: /*case 58*/ 
            __L56: /*case 59*/ 
            ;
            ;
            count += expression_get_variables(expr->left, n, variables);
            count += expression_get_variables(expr->right, n, variables);
            goto __L0; /* break */

            __L57: /*case 60*/ 
            goto __L0; /* break */

            __L58: /*case 61*/ 
            goto __L0; /* break */

            __L59: /*case 62*/ 
            goto __L0; /* break */

            __L60: /*case 63*/ 
            goto __L0; /* break */

            __L61: /*case 64*/ 
            goto __L0; /* break */

            __L62: /*case 65*/ 
            goto __L0; /* break */

            __L63: /*case 66*/ 
            goto __L0; /* break */

            __L64: /*case 67*/ 
            goto __L0; /* break */

            __L65: /*case 68*/ 
            goto __L0; /* break */

            __L66: /*case 69*/ 
            goto __L0; /* break */

            __L67: /*case 70*/ 
            goto __L0; /* break */

            __L68: /*case 71*/ 
            goto __L0; /* break */

            __L69: /*case 72*/ 
            goto __L0; /* break */

            __L70: /*case 21*/ 
            __L71: /*case 22*/ 
            __L72: /*case 23*/ 
            __L73: /*case 24*/ 
            __L74: /*case 25*/ 
            __L75: /*case 32*/ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return (count != 0);
}


unsigned char expression_is_lvalue(struct expression * expr)
{
    /*switch*/
    {
        int __v0 = expr->expression_type;
        if (__v0 == 2) goto __L1; /*case 2*/
        if (__v0 == 4) goto __L2; /*case 4*/
        if (__v0 == 3) goto __L3; /*case 3*/
        if (__v0 == 13) goto __L4; /*case 13*/
        if (__v0 == 15) goto __L5; /*case 15*/
        if (__v0 == 11) goto __L6; /*case 11*/
        if (__v0 == 39) goto __L7; /*case 39*/
        goto __L8; /* default */

        {
            __L1: /*case 2*/ 
            __L2: /*case 4*/ 
            __L3: /*case 3*/ 
            __L4: /*case 13*/ 
            __L5: /*case 15*/ 
            __L6: /*case 11*/ 
            __L7: /*case 39*/ 
            return 1;
            __L8: /* default */ 
            goto __L0; /* break */

        }
        __L0:;
    }
    if (expr->expression_type == 9 && expr->right)
    {
        return expression_is_lvalue(expr->right);
    }
    else
    {
        if (expr->expression_type == 14 && expr->left)
        {
            return expression_is_lvalue(expr->left);
        }
    }
    return 0;
}


unsigned char expression_is_subjected_to_lvalue_conversion(struct expression * expression)
{
    /*switch*/
    {
        int __v0 = expression->expression_type;
        if (__v0 == 40) goto __L1; /*case 40*/
        if (__v0 == 33) goto __L2; /*case 33*/
        if (__v0 == 34) goto __L3; /*case 34*/
        if (__v0 == 16) goto __L4; /*case 16*/
        if (__v0 == 17) goto __L5; /*case 17*/
        goto __L6; /* default */

        {
            __L1: /*case 40*/ 
            __L2: /*case 33*/ 
            __L3: /*case 34*/ 
            __L4: /*case 16*/ 
            __L5: /*case 17*/ 
            return 0;
            __L6: /* default */ 
            if (expression->type.storage_class_specifier_flags & 2048)
            {
                return 1;
            }
        }
        __L0:;
    }
    return 1;
}


unsigned char type_is_owner_or_pointer_to_dtor(struct type * p_type);
unsigned char is_automatic_variable(int f);
unsigned char type_is_pointed_dtor(struct type * p_type);
unsigned char type_is_opt(struct type * p_type, unsigned char  nullable_enabled);
void type_print(struct type * a, int target);

void check_assigment(struct parser_ctx * ctx, struct type * p_a_type, struct expression * p_b_expression, int assignment_type)
{
    struct type * p_b_type;
    unsigned char  is_null_pointer_constant;
    struct type  b_type_lvalue;

    p_b_type = &p_b_expression->type;
    is_null_pointer_constant = expression_is_null_pointer_constant(p_b_expression);
    if (type_is_pointer(p_a_type))
    {
        if (!type_is_nullptr_t(p_b_type) && !type_is_pointer_or_array(p_b_type) && !type_is_function(p_b_type))
        {
            if (is_null_pointer_constant)
            {
                if (p_b_expression->expression_type == 8)
                {
                    compiler_diagnostic(11, ctx, p_b_expression->first_token, 0, "use NULL instead of 0");
                }
                else
                {
                    compiler_diagnostic(46, ctx, p_b_expression->first_token, 0, "unusual expression/type used as null pointer constant");
                }
            }
            else
            {
                compiler_diagnostic(1340, ctx, p_b_expression->first_token, 0, "non-pointer to pointer");
            }
        }
    }
    if (type_is_bool(p_a_type) && type_is_nullptr_t(p_b_type))
    {
        struct marker  marker;

        marker.file = 0;
        marker.line = 0;
        marker.start_col = 0;
        marker.end_col = 0;
        marker.p_token_caret = 0;
        marker.p_token_begin = p_b_expression->first_token;
        marker.p_token_end = p_b_expression->first_token;
        compiler_diagnostic(49, ctx, 0, &marker, "implicit conversion of nullptr constant to 'bool'");
    }
    _cake_zmem(&b_type_lvalue, 68);
    if (expression_is_subjected_to_lvalue_conversion(p_b_expression))
    {
        b_type_lvalue = type_lvalue_conversion(p_b_type, ctx->options.null_checks_enabled);
    }
    else
    {
        b_type_lvalue = type_dup(p_b_type);
    }
    if (type_is_owner(p_a_type) && !type_is_owner(&p_b_expression->type))
    {
        if (!is_null_pointer_constant)
        {
            compiler_diagnostic(25, ctx, p_b_expression->first_token, 0, "cannot assign a non-owner to owner");
            type_destroy(&b_type_lvalue);
            return;
        }
    }
    if (!type_is_owner(p_a_type) && type_is_owner_or_pointer_to_dtor(&p_b_expression->type))
    {
        if (p_b_expression->type.storage_class_specifier_flags & 8192)
        {
            compiler_diagnostic(23, ctx, p_b_expression->first_token, 0, "cannot assign a temporary owner to non-owner object.");
            type_destroy(&b_type_lvalue);
            return;
        }
    }
    if (assignment_type == 0)
    {
        if (!type_is_owner(p_a_type) && type_is_owner_or_pointer_to_dtor(&p_b_expression->type))
        {
            if (is_automatic_variable(p_b_expression->type.storage_class_specifier_flags))
            {
                compiler_diagnostic(1280, ctx, p_b_expression->first_token, 0, "cannot return a automatic storage duration _Owner to non-owner");
                type_destroy(&b_type_lvalue);
                return;
            }
        }
    }
    if (type_is_pointed_dtor(p_a_type) && type_is_pointer(p_a_type))
    {
        if (type_is_owner(p_b_type))
        {
        }
        else
        {
            if (!p_b_type->address_of)
            {
                compiler_diagnostic(19, ctx, p_b_expression->first_token, 0, "source expression of _Dtor must be addressof");
            }
        }
    }
    if (type_is_pointer(p_a_type) && !type_is_opt(p_a_type, ctx->options.null_checks_enabled) && is_null_pointer_constant)
    {
        compiler_diagnostic(60, ctx, p_b_expression->first_token, 0, "cannot convert a null pointer constant to non-nullable pointer");
        type_destroy(&b_type_lvalue);
        return;
    }
    if (type_is_enum(p_b_type) && type_is_enum(p_a_type))
    {
        if (!type_is_same(p_b_type, p_a_type, 0))
        {
            compiler_diagnostic(40, ctx, p_b_expression->first_token, 0, " incompatible types ");
        }
        type_destroy(&b_type_lvalue);
        return;
    }
    if (type_is_arithmetic(p_b_type) && type_is_arithmetic(p_a_type))
    {
        type_destroy(&b_type_lvalue);
        return;
    }
    if (is_null_pointer_constant && type_is_pointer(p_a_type))
    {
        type_destroy(&b_type_lvalue);
        return;
    }
    if (is_null_pointer_constant && type_is_array(p_a_type))
    {
        compiler_diagnostic(39, ctx, p_b_expression->first_token, 0, " passing null as array");
        type_destroy(&b_type_lvalue);
        return;
    }
    if (type_is_pointer_or_array(p_b_type) && type_is_pointer_or_array(p_a_type))
    {
        struct type  a_type_lvalue;

        if (type_is_void_ptr(p_b_type))
        {
            type_destroy(&b_type_lvalue);
            return;
        }
        if (type_is_void_ptr(p_a_type))
        {
            type_destroy(&b_type_lvalue);
            return;
        }
        _cake_zmem(&a_type_lvalue, 68);
        if (type_is_array(p_a_type))
        {
            if (assignment_type == 1)
            {
                unsigned int parameter_array_size;

                parameter_array_size = p_a_type->num_of_elements;
                if (type_is_array(p_b_type))
                {
                    unsigned int argument_array_size;

                    argument_array_size = p_b_type->num_of_elements;
                    if (parameter_array_size != 0 && argument_array_size < parameter_array_size)
                    {
                        compiler_diagnostic(1130, ctx, p_b_expression->first_token, 0, " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                    }
                }
                else
                {
                    if (is_null_pointer_constant || type_is_nullptr_t(p_b_type))
                    {
                        compiler_diagnostic(39, ctx, p_b_expression->first_token, 0, " passing null as array");
                    }
                }
            }
            a_type_lvalue = type_lvalue_conversion(p_a_type, ctx->options.null_checks_enabled);
        }
        else
        {
            a_type_lvalue = type_dup(p_a_type);
        }
        if (!type_is_same(&b_type_lvalue, &a_type_lvalue, 0))
        {
            type_print(&b_type_lvalue, ctx->options.target);
            type_print(&a_type_lvalue, ctx->options.target);
            compiler_diagnostic(54, ctx, p_b_expression->first_token, 0, " incompatible types");
        }
        if (assignment_type == 1)
        {
            if (type_is_pointer(&b_type_lvalue) && type_is_pointer(&a_type_lvalue))
            {
                struct type  b_pointed_type_lvalue;
                struct type  a_lvalue_pointed_type;

                b_pointed_type_lvalue = type_remove_pointer(&b_type_lvalue);
                a_lvalue_pointed_type = type_remove_pointer(&a_type_lvalue);
                if (type_is_const(&b_pointed_type_lvalue) && !type_is_const(&a_lvalue_pointed_type))
                {
                    compiler_diagnostic(15, ctx, p_b_expression->first_token, 0, " discarding const at argument ");
                }
                type_destroy(&b_pointed_type_lvalue);
                type_destroy(&a_lvalue_pointed_type);
            }
        }
        else
        {
            if (type_is_pointer(p_a_type) && type_is_pointer(&b_type_lvalue))
            {
                struct type  b_pointed_type;
                struct type  a_pointed_type;

                b_pointed_type = type_remove_pointer(&b_type_lvalue);
                a_pointed_type = type_remove_pointer(p_a_type);
                if (type_is_const(&b_pointed_type) && !type_is_const(&a_pointed_type))
                {
                    compiler_diagnostic(15, ctx, p_b_expression->first_token, 0, " discarding const");
                }
                type_destroy(&b_pointed_type);
                type_destroy(&a_pointed_type);
            }
        }
        type_destroy(&a_type_lvalue);
    }
    if (!type_is_same(p_a_type, &b_type_lvalue, 0))
    {
    }
    type_destroy(&b_type_lvalue);
}


