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

struct diagnostic {
    unsigned long long errors;
    unsigned long long warnings;
    unsigned long long notes;
};

struct case_label_list {
    struct label * head;
    struct label * tail;
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

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

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

struct type_specifier {
    int   flags;
    struct token * token;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct typeof_specifier * typeof_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct atomic_type_specifier * atomic_type_specifier;
};

struct enumerator_list {
    struct enumerator * head;
    struct enumerator * tail;
};

struct direct_declarator {
    struct token * name_opt;
    struct token * p_calling_convention;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
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

struct label_list {
    struct label_list_item * head;
    struct label_list_item * tail;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
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

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
};

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
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

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
};

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
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

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct _iobuf {
    void * _Placeholder;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct function_specifier {
    int   flags;
    struct token * token;
};

struct param {
    struct type  type;
    struct param * next;
};

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct attribute_token {
    int   attributes_flags;
    struct token * token;
};

struct token_list {
    struct token * head;
    struct token * tail;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
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

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
};

struct storage_class_specifier {
    int   flags;
    struct token * token;
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

struct type_qualifier {
    int   flags;
    struct token * token;
    struct type_qualifier * next;
};

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
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

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
};

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
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

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
};

struct label {
    struct expression * constant_expression;
    struct expression * constant_expression_end;
    struct token * p_identifier_opt;
    struct token * p_first_token;
    struct label * next;
    int label_id;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct label_list_item {
    struct token * p_last_usage;
    struct token * p_defined;
    struct label_list_item * next;
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

struct map_entry {
    struct map_entry * next;
    unsigned int hash;
    char * key;
    int   type;
    union _struct_tag_5  data;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int label_id;
    struct defer_list  defer_list;
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

struct labeled_statement {
    struct label * label;
    struct statement * statement;
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

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
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

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
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

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
};

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int catch_label_id;
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

struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
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

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int _Avx2WmemEnabledWeakValue = 0;
struct param_list *type_get_func_or_func_ptr_params(struct type * p_type);
unsigned char  type_is_void(struct type * p_type);
void check_assigment(struct parser_ctx * ctx, struct type * left_type, struct expression * right, int   assigment_type);
unsigned char  compiler_diagnostic(int   w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);

static int compare_function_arguments(struct parser_ctx * ctx, struct type * p_type, struct argument_expression_list * p_argument_expression_list)
{
    /*try*/ if (1)
    {
        struct param * p_current_parameter_type;
        struct param_list * p_param_list;
        struct argument_expression * p_current_argument;

        p_current_parameter_type = 0U;
        p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_current_parameter_type = p_param_list->head;
        p_current_argument = p_argument_expression_list->head;
        if (p_current_parameter_type && type_is_void(&p_current_parameter_type->type))
        {
            p_current_parameter_type = 0U;
        }
        while (p_current_argument && p_current_parameter_type)
        {
            check_assigment(ctx, &p_current_parameter_type->type, p_current_argument->expression, 1);
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }
        if (p_current_argument != 0U && !p_param_list->is_var_args)
        {
            compiler_diagnostic(660, ctx, p_current_argument->expression->first_token, 0U, "too many arguments");
            /*throw*/ goto _CKL0;
        }
        if (p_current_parameter_type != 0U && !p_param_list->is_void)
        {
            if (p_argument_expression_list->tail)
            {
                compiler_diagnostic(670, ctx, p_argument_expression_list->tail->expression->first_token, 0U, "too few arguments");
            }
            else
            {
                compiler_diagnostic(670, ctx, ctx->current, 0U, "too few arguments");
            }
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        return 1;
    }
    return 0;
}

struct enumerator *find_enumerator(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt);

unsigned char  is_enumeration_constant(struct parser_ctx * ctx)
{
    unsigned char   is_enumerator;

    if (ctx->current == 0U)
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
    is_enumerator = !!(find_enumerator(ctx, ctx->current->lexeme, 0U) != 0U);
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

unsigned char  is_first_of_floating_constant(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 140 || ctx->current->type == 141);
}

unsigned char  is_first_of_integer_constant(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 136 || ctx->current->type == 137 || ctx->current->type == 138 || ctx->current->type == 139);
}

unsigned char  is_predefined_constant(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 9062 || ctx->current->type == 9063 || ctx->current->type == 9064);
}

unsigned char  is_first_of_constant(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(is_first_of_integer_constant(ctx) || is_first_of_floating_constant(ctx) || is_enumeration_constant(ctx) || (ctx->current->type == 131) || is_predefined_constant(ctx));
}

unsigned char  is_first_of_primary_expression(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 8996 || is_first_of_constant(ctx) || ctx->current->type == 130 || ctx->current->type == 40 || ctx->current->type == 9055);
}

void unexpected_end_of_file(struct parser_ctx * ctx);
void *calloc(unsigned int nmemb, unsigned int size);
void parser_match(struct parser_ctx * ctx);
unsigned char  first_of_type_name(struct parser_ctx * ctx);
struct type_name *type_name(struct parser_ctx * ctx);
struct type make_type_using_declarator(struct parser_ctx * ctx, struct declarator * pdeclarator);
int parser_match_tk(struct parser_ctx * ctx, int   type);
struct expression *assignment_expression(struct parser_ctx * ctx);
void generic_association_delete(struct generic_association * p);

struct generic_association *generic_association(struct parser_ctx * ctx)
{
    struct generic_association * p_generic_association;

    p_generic_association = 0U;
    /*try*/ if (1)
    {
        struct expression * p_expression_temp;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_generic_association = calloc(1, 80U);
        if (p_generic_association == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_generic_association->first_token = ctx->current;
        if (ctx->current->type == 9007)
        {
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            if (first_of_type_name(ctx))
            {
                unsigned char   old;

                old = ctx->inside_generic_association;
                ctx->inside_generic_association = 1;
                p_generic_association->p_type_name = type_name(ctx);
                if (p_generic_association->p_type_name == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                ;
                ctx->inside_generic_association = old;
                p_generic_association->type = make_type_using_declarator(ctx, p_generic_association->p_type_name->abstract_declarator);
            }
            else
            {
                compiler_diagnostic(650, ctx, ctx->current, 0U, "unexpected");
            }
        }
        if (parser_match_tk(ctx, 58) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        p_expression_temp = assignment_expression(ctx);
        if (p_expression_temp == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_generic_association->expression = p_expression_temp;
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_generic_association->last_token = ctx->current;
    }
    /*catch*/ else _CKL0:
    {
        generic_association_delete(p_generic_association);
        p_generic_association = 0U;
    }
    return p_generic_association;
}

void generic_assoc_list_add(struct generic_assoc_list * p, struct generic_association * item);

struct generic_assoc_list generic_association_list(struct parser_ctx * ctx)
{
    struct generic_assoc_list  list;

    _cake_zmem(&list, 8);
    /*try*/ if (1)
    {
        struct generic_association * p_default_generic_association;
        struct generic_association * p_generic_association;

        p_default_generic_association = 0U;
        p_generic_association = generic_association(ctx);
        if (p_generic_association == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (p_generic_association->first_token->type == 9007)
        {
            p_default_generic_association = p_generic_association;
        }
        generic_assoc_list_add(&list, p_generic_association);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        while (ctx->current->type == 44)
        {
            struct generic_association * p_generic_association2;

            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            p_generic_association2 = generic_association(ctx);
            if (p_generic_association2 == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (p_generic_association2->first_token->type == 9007)
            {
                if (p_default_generic_association != 0U)
                {
                    compiler_diagnostic(1570, ctx, p_generic_association2->first_token, 0U, "duplicate default generic association.");
                    compiler_diagnostic(63, ctx, p_default_generic_association->first_token, 0U, "previous default generic association");
                }
                else
                {
                    p_default_generic_association = p_generic_association2;
                }
            }
            generic_assoc_list_add(&list, p_generic_association2);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
        }
    }
    /*catch*/ else _CKL0:
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

void generic_assoc_list_destroy(struct generic_assoc_list * p)
{
    struct generic_association * item;

    item = p->head;
    while (item)
    {
        struct generic_association * next;

        next = item->next;
        item->next = 0U;
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

unsigned char  expression_is_subjected_to_lvalue_conversion(struct expression *);
struct type type_lvalue_conversion(struct type * p_type, unsigned char   nullchecks_enabled);
unsigned char  type_is_same(struct type * a, struct type * b, unsigned char   compare_qualifiers);

struct generic_selection *generic_selection(struct parser_ctx * ctx)
{
    struct generic_selection * p_generic_selection;

    p_generic_selection = 0U;
    /*try*/ if (1)
    {
        struct type  lvalue_type;
        struct type * p_type;
        struct generic_association * current;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_generic_selection = calloc(1, 28U);
        if (p_generic_selection == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_generic_selection->first_token = ctx->current;
        if (parser_match_tk(ctx, 9055) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        if (first_of_type_name(ctx))
        {
            p_generic_selection->type_name = type_name(ctx);
            if (p_generic_selection->type_name == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        else
        {
            p_generic_selection->expression = assignment_expression(ctx);
            if (p_generic_selection->expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        if (parser_match_tk(ctx, 44) != 0)
        {
            /*throw*/ goto _CKL0;
        }
        p_generic_selection->generic_assoc_list = generic_association_list(ctx);
        if (p_generic_selection->generic_assoc_list.head == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        _cake_zmem(&lvalue_type, 60);
        p_type = 0U;
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
                /*throw*/ goto _CKL0;
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
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_generic_selection->last_token = ctx->current;
        if (parser_match_tk(ctx, 41) != 0)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        generic_selection_delete(p_generic_selection);
        p_generic_selection = 0U;
    }
    return p_generic_selection;
}

unsigned char *utf8_decode(unsigned char * s, unsigned int * c);
unsigned char *escape_sequences_decode_opt(unsigned char * p, unsigned int * out_value);
struct object object_make_unsigned_char(unsigned char value);
struct object object_make_wchar_t(unsigned short value);

struct expression *character_constant_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        unsigned char * p;

        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_expression_node = calloc(1, 224U);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
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
            if (p == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (c == 92)
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            if (*p != 39)
            {
                compiler_diagnostic(1370, ctx, ctx->current, 0U, "Unicode character literals may not contain multiple characters.");
            }
            if (c > 128)
            {
                compiler_diagnostic(1360, ctx, ctx->current, 0U, "character not encodable in a single code unit.");
            }
            p_expression_node->object = object_make_unsigned_char((unsigned char)c);
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
                if (p == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                if (c == 92)
                {
                    p = escape_sequences_decode_opt(p, &c);
                    if (p == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                }
                if (*p != 39)
                {
                    compiler_diagnostic(40, ctx, ctx->current, 0U, "Unicode character literals may not contain multiple characters.");
                }
                if (c > 65535)
                {
                    compiler_diagnostic(40, ctx, ctx->current, 0U, "Character too large for enclosing character literal type.");
                }
                p_expression_node->object = object_make_wchar_t((unsigned short)c);
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
                    if (p == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    if (c == 92)
                    {
                        p = escape_sequences_decode_opt(p, &c);
                        if (p == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                    }
                    if (*p != 39)
                    {
                        compiler_diagnostic(40, ctx, ctx->current, 0U, "Unicode character literals may not contain multiple characters.");
                    }
                    if (c > 4294967295LL)
                    {
                        compiler_diagnostic(40, ctx, ctx->current, 0U, "Character too large for enclosing character literal type.");
                    }
                    p_expression_node->object = object_make_wchar_t((unsigned short)c);
                }
                else
                {
                    if (p[0] == 76)
                    {
                        long long value;

                        p++;
                        p++;
                        p_expression_node->type.type_specifier_flags = 260;
                        value = 0;
                        while (*p != 39)
                        {
                            unsigned int c;

                            c = 0;
                            p = utf8_decode(p, &c);
                            if (p == 0U)
                            {
                                /*throw*/ goto _CKL0;
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == 0U)
                                {
                                    /*throw*/ goto _CKL0;
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
                            if (value > 65535)
                            {
                                compiler_diagnostic(41, ctx, ctx->current, 0U, "character constant too long for its type", ctx->current->lexeme);
                                break;
                            }
                        }
                        p_expression_node->object = object_make_wchar_t((unsigned short)value);
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
                            if (p == 0U)
                            {
                                /*throw*/ goto _CKL0;
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == 0U)
                                {
                                    /*throw*/ goto _CKL0;
                                }
                            }
                            value = value * 256 + c;
                            if (value > 2147483647)
                            {
                                compiler_diagnostic(41, ctx, ctx->current, 0U, "character constant too long for its type", ctx->current->lexeme);
                                break;
                            }
                        }
                        p_expression_node->object = object_make_wchar_t((unsigned short)value);
                    }
                }
            }
        }
        parser_match(ctx);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

int  parse_number(char * lexeme, char suffix[4], char erromsg[100]);
unsigned long long strtoull(char * _String, char ** _EndPtr, int _Radix);
int *__cdecl _errno(void);
struct object object_make_unsigned_int(unsigned int value);
struct object object_make_unsigned_long(unsigned long value);
struct object object_make_unsigned_long_long(unsigned long long value);
struct object object_make_signed_int(signed int value);
struct object object_make_signed_long(signed long value);
struct object object_make_signed_long_long(signed long long value);
float strtof(char * _String, char ** _EndPtr);
struct object object_make_float(float value);
long double strtold(char * _String, char ** _EndPtr);
struct object object_make_long_double(long double value);
double strtod(char * _String, char ** _EndPtr);
struct object object_make_double(double value);

int convert_to_number(struct parser_ctx * ctx, struct expression * p_expression_node, unsigned char   disabled)
{
    struct token * token;
    int c;
    char buffer[260];
    char * s;
    char errormsg[100];
    char suffix[4];
    int   r;

    if (ctx->current == 0U)
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
        compiler_diagnostic(1380, ctx, token, 0U, errormsg);
        return 0;
    }
    /*switch*/
    {
        register int   _R0 = token->type;
        if (_R0 == 136) goto _CKL4; /*case 136*/
        if (_R0 == 137) goto _CKL5; /*case 137*/
        if (_R0 == 138) goto _CKL6; /*case 138*/
        if (_R0 == 139) goto _CKL7; /*case 139*/
        if (_R0 == 140) goto _CKL22; /*case 140*/
        if (_R0 == 141) goto _CKL23; /*case 141*/
        goto /*default*/ _CKL29;

        {
            /*case 136*/ _CKL4:
            /*case 137*/ _CKL5:
            /*case 138*/ _CKL6:
            /*case 139*/ _CKL7:
            {
                unsigned long long value;

                value = 0;
                /*switch*/
                {
                    register int   _R1 = token->type;
                    if (_R1 == 136) goto _CKL9; /*case 136*/
                    if (_R1 == 137) goto _CKL10; /*case 137*/
                    if (_R1 == 138) goto _CKL12; /*case 138*/
                    if (_R1 == 139) goto _CKL13; /*case 139*/
                    goto /*default*/ _CKL14;

                    {
                        /*case 136*/ _CKL9:
                        value = strtoull(buffer, 0U, 10);
                        /*break*/ goto _CKL8;

                        /*case 137*/ _CKL10:
                        if (buffer[1] == 111 || buffer[1] == 79)
                        {
                            value = strtoull(buffer + 2, 0U, 8);
                        }
                        else
                        {
                            value = strtoull(buffer + 1, 0U, 8);
                        }
                        /*break*/ goto _CKL8;

                        /*case 138*/ _CKL12:
                        value = strtoull(buffer + 2, 0U, 16);
                        /*break*/ goto _CKL8;

                        /*case 139*/ _CKL13:
                        value = strtoull(buffer + 2, 0U, 2);
                        /*break*/ goto _CKL8;

                        /*default*/ _CKL14:
                        /*break*/ goto _CKL8;

                    }
                    _CKL8:;
                }
                if (value == 18446744073709551615ULL && (*_errno()) == 34)
                {
                    compiler_diagnostic(1350, ctx, token, 0U, "integer literal is too large to be represented in any integer type");
                }
                if (suffix[0] == 85)
                {
                    if (value <= 4294967295LL && suffix[1] != 76)
                    {
                        p_expression_node->object = object_make_unsigned_int((unsigned int)value);
                        p_expression_node->type.type_specifier_flags = 264;
                    }
                    else
                    {
                        if (value <= 4294967295UL && suffix[2] != 76)
                        {
                            p_expression_node->object = object_make_unsigned_long((unsigned long)value);
                            p_expression_node->type.type_specifier_flags = 272;
                        }
                        else
                        {
                            p_expression_node->object = object_make_unsigned_long_long((unsigned long long)value);
                            p_expression_node->type.type_specifier_flags = 4194560;
                        }
                    }
                }
                else
                {
                    if (value <= 2147483647 && suffix[0] != 76)
                    {
                        p_expression_node->object = object_make_signed_int((int)value);
                        p_expression_node->type.type_specifier_flags = 8;
                    }
                    else
                    {
                        if (value <= 2147483647L && suffix[1] != 76)
                        {
                            p_expression_node->object = object_make_signed_long((long)value);
                            p_expression_node->type.type_specifier_flags = 16;
                        }
                        else
                        {
                            if (value <= 9223372036854775807LL)
                            {
                                p_expression_node->object = object_make_signed_long_long((long long)value);
                                p_expression_node->type.type_specifier_flags = 4194304;
                            }
                            else
                            {
                                compiler_diagnostic(49, ctx, token, 0U, "integer literal is too large to be represented in a signed integer type, interpreting as unsigned");
                                p_expression_node->object = object_make_signed_long_long(value);
                                p_expression_node->type.type_specifier_flags = 4194560;
                            }
                        }
                    }
                }
            }
            /*break*/ goto _CKL3;

            /*case 140*/ _CKL22:
            /*case 141*/ _CKL23:
            {
                if (suffix[0] == 70)
                {
                    float value;

                    value = strtof(buffer, 0U);
                    if (value == 1.000000f && (*_errno()) == 34)
                    {
                    }
                    p_expression_node->type.type_specifier_flags = 32;
                    p_expression_node->object = object_make_float(value);
                }
                else
                {
                    if (suffix[0] == 76)
                    {
                        long double value;

                        value = strtold(buffer, 0U);
                        if (value == 1.000000L && (*_errno()) == 34)
                        {
                        }
                        p_expression_node->type.type_specifier_flags = 80;
                        p_expression_node->object = object_make_long_double(value);
                    }
                    else
                    {
                        double value;

                        value = strtod(buffer, 0U);
                        if (value == 1.000000 && (*_errno()) == 34)
                        {
                        }
                        p_expression_node->object = object_make_double(value);
                        p_expression_node->type.type_specifier_flags = 64;
                    }
                }
            }
            /*break*/ goto _CKL3;

            /*default*/ _CKL29:
            ;
        }
        _CKL3:;
    }
    return 0;
}

static unsigned char  is_integer_or_floating_constant(int   type)
{
    return !!(type == 136 || type == 137 || type == 138 || type == 139 || type == 140 || type == 141);
}

struct map_entry *find_variables(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt);
struct type type_make_enumerator(struct enum_specifier * enum_specifier);
unsigned char  type_is_deprecated(struct type * p_type);
struct type type_dup(struct type * p_type);
struct object object_make_reference(struct object * object);
int __cdecl strcmp(char * _Str1, char * _Str2);
struct type type_make_literal_string(int size, int   chartype, int   qualifiers);
unsigned int __cdecl strlen(char * _Str);
int get_char_type(char * s);
int string_literal_char_byte_size(char * s);
int string_literal_byte_size_not_zero_included(char * s);
struct object object_make_bool(unsigned char   value);
struct object object_make_nullptr();
struct expression *expression(struct parser_ctx * ctx);

struct expression *primary_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    if (ctx->current == 0U)
    {
        unexpected_end_of_file(ctx);
        return 0U;
    }
    p_expression_node = 0U;
    /*try*/ if (1)
    {
        if (ctx->current->type == 8996)
        {
            struct scope * p_scope;
            struct map_entry * p_entry;

            p_expression_node = calloc(1, 224U);
            if (p_expression_node == 0U)
            {
                /*throw*/ goto _CKL1;
            }
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;
            p_scope = 0U;
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

                    p_declarator = 0U;
                    p_init_declarator = 0U;
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
                        compiler_diagnostic(2, ctx, ctx->current, 0U, "'%s' is deprecated", ctx->current->lexeme);
                    }
                    if (p_scope->scope_level == 0)
                    {
                    }
                    else
                    {
                        if (ctx->p_scope)
                        {
                            if (!ctx->evaluation_is_disabled)
                            {
                                unsigned char   inside_current_function_scope;

                                inside_current_function_scope = 0;
                                while (p_scope)
                                {
                                    if (ctx->p_scope == p_scope)
                                    {
                                        inside_current_function_scope = 1;
                                        break;
                                    }
                                    p_scope = p_scope->previous;
                                }
                                if (!inside_current_function_scope)
                                {
                                    compiler_diagnostic(1870, ctx, ctx->current, 0U, "'%s' cannot be evaluated in this scope", ctx->current->lexeme);
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
                        p_expression_node->type = type_make_literal_string(strlen(func_name) + 1, 2, 1);
                    }
                    else
                    {
                        compiler_diagnostic(680, ctx, ctx->current, 0U, "not found '%s'", ctx->current->lexeme);
                        /*throw*/ goto _CKL1;
                    }
                }
            }
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL1;
            }
        }
        else
        {
            if (ctx->current->type == 130)
            {
                int   char_type;
                int char_byte_size;
                int number_of_bytes;
                struct object * last;
                int   lit_flags;

                p_expression_node = calloc(1, 224U);
                if (p_expression_node == 0U)
                {
                    /*throw*/ goto _CKL1;
                }
                p_expression_node->expression_type = 3;
                p_expression_node->first_token = ctx->current;
                p_expression_node->last_token = ctx->current;
                char_type = 2;
                if (get_char_type(ctx->current->lexeme) == 2)
                {
                    char_type = 260;
                }
                char_byte_size = string_literal_char_byte_size(ctx->current->lexeme);
                number_of_bytes = 0;
                last = 0U;
                while (ctx->current->type == 130)
                {
                    unsigned char * it;
                    unsigned int value;
                    struct object * p_new;

                    it = ctx->current->lexeme + 1;
                    value = 0;
                    while (it && *it != 34)
                    {
                        struct object * p_new;

                        if (*it == 92)
                        {
                            it = escape_sequences_decode_opt(it, &value);
                        }
                        else
                        {
                            value = *it;
                            it++;
                        }
                        p_new = calloc(1, 96U);
                        if (p_new == 0U)
                        {
                            /*throw*/ goto _CKL1;
                        }
                        p_new->state = 2;
                        p_new->value_type = 3;
                        p_new->value.signed_char_value = value;
                        if (p_expression_node->object.members == 0U)
                        {
                            p_expression_node->object.members = p_new;
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
                    p_new = calloc(1, 96U);
                    if (p_new == 0U)
                    {
                        /*throw*/ goto _CKL1;
                    }
                    p_new->state = 2;
                    p_new->value_type = 3;
                    p_new->value.signed_char_value = 0;
                    if (last == 0U)
                    {
                        p_expression_node->object.members = p_new;
                    }
                    else
                    {
                        last->next = p_new;
                    }
                    number_of_bytes += string_literal_byte_size_not_zero_included(ctx->current->lexeme);
                    parser_match(ctx);
                    if (ctx->current == 0U)
                    {
                        unexpected_end_of_file(ctx);
                        /*throw*/ goto _CKL1;
                    }
                }
                lit_flags = ctx->options.const_literal ? 1 : 0;
                p_expression_node->type = type_make_literal_string(number_of_bytes + (1 * char_byte_size), char_type, lit_flags);
            }
            else
            {
                if (ctx->current->type == 131)
                {
                    p_expression_node = character_constant_expression(ctx);
                }
                else
                {
                    if (ctx->current->type == 9062 || ctx->current->type == 9063)
                    {
                        p_expression_node = calloc(1, 224U);
                        if (p_expression_node == 0U)
                        {
                            /*throw*/ goto _CKL1;
                        }
                        p_expression_node->expression_type = 6;
                        p_expression_node->first_token = ctx->current;
                        p_expression_node->last_token = ctx->current;
                        p_expression_node->object = object_make_bool(ctx->current->type == 9062);
                        p_expression_node->type.type_specifier_flags = 512;
                        p_expression_node->type.type_qualifier_flags = 0;
                        parser_match(ctx);
                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            /*throw*/ goto _CKL1;
                        }
                    }
                    else
                    {
                        if (ctx->current->type == 9064)
                        {
                            p_expression_node = calloc(1, 224U);
                            if (p_expression_node == 0U)
                            {
                                /*throw*/ goto _CKL1;
                            }
                            p_expression_node->expression_type = 6;
                            p_expression_node->first_token = ctx->current;
                            p_expression_node->last_token = ctx->current;
                            p_expression_node->object = object_make_nullptr();
                            p_expression_node->type.type_specifier_flags = 16777216;
                            p_expression_node->type.type_qualifier_flags = 0;
                            parser_match(ctx);
                            if (ctx->current == 0U)
                            {
                                unexpected_end_of_file(ctx);
                                /*throw*/ goto _CKL1;
                            }
                        }
                        else
                        {
                            if (is_integer_or_floating_constant(ctx->current->type))
                            {
                                p_expression_node = calloc(1, 224U);
                                if (p_expression_node == 0U)
                                {
                                    /*throw*/ goto _CKL1;
                                }
                                p_expression_node->first_token = ctx->current;
                                p_expression_node->last_token = ctx->current;
                                p_expression_node->expression_type = 8;
                                convert_to_number(ctx, p_expression_node, 0);
                                parser_match(ctx);
                                if (ctx->current == 0U)
                                {
                                    unexpected_end_of_file(ctx);
                                    /*throw*/ goto _CKL1;
                                }
                            }
                            else
                            {
                                if (ctx->current->type == 9055)
                                {
                                    p_expression_node = calloc(1, 224U);
                                    if (p_expression_node == 0U)
                                    {
                                        /*throw*/ goto _CKL1;
                                    }
                                    p_expression_node->first_token = ctx->current;
                                    p_expression_node->expression_type = 7;
                                    p_expression_node->generic_selection = generic_selection(ctx);
                                    if (p_expression_node->generic_selection == 0U)
                                    {
                                        /*throw*/ goto _CKL1;
                                    }
                                    p_expression_node->last_token = p_expression_node->generic_selection->last_token;
                                    if (p_expression_node->generic_selection->p_view_selected_expression)
                                    {
                                        p_expression_node->type = type_dup(&p_expression_node->generic_selection->p_view_selected_expression->type);
                                        p_expression_node->object = p_expression_node->generic_selection->p_view_selected_expression->object;
                                    }
                                    else
                                    {
                                        compiler_diagnostic(690, ctx, ctx->current, 0U, "no match for generic");
                                    }
                                }
                                else
                                {
                                    if (ctx->current->type == 40)
                                    {
                                        p_expression_node = calloc(1, 224U);
                                        if (p_expression_node == 0U)
                                        {
                                            /*throw*/ goto _CKL1;
                                        }
                                        p_expression_node->expression_type = 9;
                                        p_expression_node->first_token = ctx->current;
                                        parser_match(ctx);
                                        if (ctx->current == 0U)
                                        {
                                            unexpected_end_of_file(ctx);
                                            /*throw*/ goto _CKL1;
                                        }
                                        p_expression_node->right = expression(ctx);
                                        if (p_expression_node->right == 0U)
                                        {
                                            /*throw*/ goto _CKL1;
                                        }
                                        p_expression_node->type = type_dup(&p_expression_node->right->type);
                                        p_expression_node->object = p_expression_node->right->object;
                                        if (ctx->current == 0U)
                                        {
                                            unexpected_end_of_file(ctx);
                                            /*throw*/ goto _CKL1;
                                        }
                                        p_expression_node->last_token = ctx->current;
                                        if (parser_match_tk(ctx, 41) != 0)
                                        {
                                            /*throw*/ goto _CKL1;
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic(650, ctx, ctx->current, 0U, "unexpected");
                                        /*throw*/ goto _CKL1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    /*catch*/ else _CKL1:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    ;
    return p_expression_node;
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

struct argument_expression_list argument_expression_list(struct parser_ctx * ctx)
{
    struct argument_expression_list  list;
    struct argument_expression * p_argument_expression;

    _cake_zmem(&list, 8);
    p_argument_expression = 0U;
    /*try*/ if (1)
    {
        struct expression * p_assignment_expression;

        p_argument_expression = calloc(1, 12U);
        if (p_argument_expression == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_assignment_expression = assignment_expression(ctx);
        if (p_assignment_expression == 0U)
        {
            argument_expression_delete(p_argument_expression);
            /*throw*/ goto _CKL0;
        }
        p_argument_expression->expression = p_assignment_expression;
        argument_expression_list_push(&list, p_argument_expression);
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        while (ctx->current->type == 44)
        {
            struct argument_expression * p_argument_expression_2;
            struct expression * p_assignment_expression_2;

            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            p_argument_expression_2 = calloc(1, 12U);
            if (p_argument_expression_2 == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_assignment_expression_2 = assignment_expression(ctx);
            if (p_assignment_expression_2 == 0U)
            {
                argument_expression_delete(p_argument_expression_2);
                /*throw*/ goto _CKL0;
            }
            p_argument_expression_2->expression = p_assignment_expression_2;
            argument_expression_list_push(&list, p_argument_expression_2);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    return list;
}

unsigned char  first_of_type_name_ahead(struct parser_ctx * ctx);

unsigned char  first_of_postfix_expression(struct parser_ctx * ctx)
{
    if (first_of_type_name_ahead(ctx))
    {
        return 1;
    }
    return is_first_of_primary_expression(ctx);
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

struct type type_remove_pointer(struct type * p_type);

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

unsigned char  type_is_pointer_or_array(struct type * p_type);
unsigned char  type_is_pointer(struct type * p_type);
unsigned char  type_is_array(struct type * p_type);
struct type get_array_item_type(struct type * p_type);
unsigned char  type_is_integer(struct type * p_type);
unsigned char  object_has_constant_value(struct object * a);
unsigned long long object_to_unsigned_long_long(struct object * a);
struct object *object_get_member(struct object * p_object, int index);
unsigned char  type_is_function_or_function_pointer(struct type * p_type);
struct type get_function_return_type(struct type * p_type);
int make_object(struct type * p_type, struct object * obj);
struct struct_or_union_specifier *find_struct_or_union_specifier(struct parser_ctx * ctx, char * lexeme);
struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);
struct member_declarator *find_member_declarator(struct member_declaration_list * list, char * name, int * p_member_index);
struct object *find_object_declarator_by_index(struct object * p_object, struct member_declaration_list * list, int member_index);
unsigned char  type_is_struct_or_union(struct type * p_type);
unsigned char  type_is_owner(struct type * p_type);
unsigned char  expression_is_lvalue(struct expression * expr);
struct token *previous_parser_token(struct token * token);

struct expression *postfix_expression_tail(struct parser_ctx * ctx, struct expression * p_expression_node_param)
{
    struct expression * p_expression_node;

    p_expression_node = p_expression_node_param;
    /*try*/ if (1)
    {
        while (ctx->current != 0U)
        {
            if (ctx->current->type == 91)
            {
                struct expression * p_expression_node_new;

                p_expression_node_new = calloc(1, 224U);
                if (p_expression_node_new == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_expression_node->last_token = ctx->current;
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = 13;
                if (!type_is_pointer_or_array(&p_expression_node->type))
                {
                    compiler_diagnostic(700, ctx, ctx->current, 0U, "subscripted value is neither array nor pointer");
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
                if (ctx->current == 0U)
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(p_expression_node_new);
                    /*throw*/ goto _CKL0;
                }
                p_expression_node_new->right = expression(ctx);
                if (p_expression_node_new->right == 0U)
                {
                    expression_delete(p_expression_node_new);
                    /*throw*/ goto _CKL0;
                }
                if (!type_is_integer(&p_expression_node_new->right->type))
                {
                    compiler_diagnostic(1560, ctx, p_expression_node_new->right->first_token, 0U, "array subscript is not an integer");
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
                                compiler_diagnostic(41, ctx, ctx->current, 0U, "index %d is past the end of the array", index);
                            }
                            it = object_get_member(&p_expression_node->object, (int)index);
                            if (it != 0U)
                            {
                                p_expression_node_new->object = object_make_reference(it);
                            }
                        }
                    }
                }
                if (parser_match_tk(ctx, 93) != 0)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = 0U;
                    /*throw*/ goto _CKL0;
                }
                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else
            {
                if (ctx->current->type == 40)
                {
                    struct expression * p_expression_node_new;

                    p_expression_node_new = calloc(1, 224U);
                    if (p_expression_node_new == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    p_expression_node->last_token = ctx->current;
                    p_expression_node_new->first_token = p_expression_node->first_token;
                    p_expression_node_new->expression_type = 12;
                    if (!type_is_function_or_function_pointer(&p_expression_node->type))
                    {
                        compiler_diagnostic(710, ctx, ctx->current, 0U, "called object is not attr function or function pointer");
                    }
                    p_expression_node_new->type = get_function_return_type(&p_expression_node->type);
                    parser_match(ctx);
                    if (ctx->current == 0U)
                    {
                        unexpected_end_of_file(ctx);
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = 0U;
                        /*throw*/ goto _CKL0;
                    }
                    if (ctx->current->type != 41)
                    {
                        p_expression_node_new->argument_expression_list = argument_expression_list(ctx);
                    }
                    if (parser_match_tk(ctx, 41) != 0)
                    {
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = 0U;
                        /*throw*/ goto _CKL0;
                    }
                    compare_function_arguments(ctx, &p_expression_node->type, &p_expression_node_new->argument_expression_list);
                    if (ctx->previous == 0U)
                    {
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = 0U;
                        /*throw*/ goto _CKL0;
                    }
                    make_object(&p_expression_node_new->type, &p_expression_node_new->object);
                    p_expression_node_new->last_token = ctx->previous;
                    p_expression_node_new->left = p_expression_node;
                    p_expression_node = p_expression_node_new;
                }
                else
                {
                    if (ctx->current->type == 46)
                    {
                        struct expression * p_expression_node_new;

                        p_expression_node_new = calloc(1, 224U);
                        if (p_expression_node_new == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        p_expression_node->last_token = ctx->current;
                        p_expression_node_new->first_token = ctx->current;
                        p_expression_node_new->expression_type = 14;
                        p_expression_node_new->left = p_expression_node;
                        p_expression_node = 0U;
                        p_expression_node_new->declarator = p_expression_node_new->left->declarator;
                        parser_match(ctx);
                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            expression_delete(p_expression_node_new);
                            p_expression_node_new = 0U;
                            /*throw*/ goto _CKL0;
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
                                    if (p_member_declarator->declarator != 0U)
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
                                    compiler_diagnostic(720, ctx, ctx->current, 0U, "member '%s' not found in 'struct %s'", ctx->current->lexeme, p_complete->tag_name);
                                }
                            }
                            else
                            {
                                compiler_diagnostic(720, ctx, ctx->current, 0U, "incomplete struct type '%s'", p_expression_node_new->left->type.struct_or_union_specifier->tag_name);
                            }
                            if (parser_match_tk(ctx, 8996) != 0)
                            {
                                expression_delete(p_expression_node_new);
                                p_expression_node_new = 0U;
                                /*throw*/ goto _CKL0;
                            }
                        }
                        else
                        {
                            compiler_diagnostic(730, ctx, ctx->current, 0U, "structure or union required");
                        }
                        p_expression_node = p_expression_node_new;
                    }
                    else
                    {
                        if (ctx->current->type == 11582)
                        {
                            struct expression * p_expression_node_new;

                            p_expression_node_new = calloc(1, 224U);
                            if (p_expression_node_new == 0U)
                            {
                                /*throw*/ goto _CKL0;
                            }
                            p_expression_node->last_token = ctx->current;
                            p_expression_node_new->first_token = p_expression_node->first_token;
                            p_expression_node_new->last_token = ctx->current;
                            p_expression_node_new->expression_type = 15;
                            parser_match(ctx);
                            if (ctx->current == 0U)
                            {
                                expression_delete(p_expression_node_new);
                                p_expression_node_new = 0U;
                                /*throw*/ goto _CKL0;
                            }
                            if (type_is_pointer_or_array(&p_expression_node->type))
                            {
                                struct type  item_type;

                                _cake_zmem(&item_type, 60);
                                if (type_is_array(&p_expression_node->type))
                                {
                                    compiler_diagnostic(19, ctx, ctx->current, 0U, "using indirection '->' in array");
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
                                            compiler_diagnostic(720, ctx, ctx->current, 0U, "member '%s' not found in struct '%s'", ctx->current->lexeme, p_expression_node->type.next->struct_or_union_specifier->tag_name);
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic(740, ctx, ctx->current, 0U, "struct '%s' is incomplete.", ctx->current->lexeme);
                                    }
                                    if (parser_match_tk(ctx, 8996) != 0)
                                    {
                                        type_destroy(&item_type);
                                        expression_delete(p_expression_node_new);
                                        p_expression_node_new = 0U;
                                        /*throw*/ goto _CKL0;
                                    }
                                }
                                else
                                {
                                    compiler_diagnostic(730, ctx, ctx->current, 0U, "structure or union required");
                                }
                                type_destroy(&item_type);
                            }
                            else
                            {
                                compiler_diagnostic(730, ctx, ctx->current, 0U, "structure or union required");
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
                                    compiler_diagnostic(1310, ctx, p_expression_node->first_token, 0U, "operator ++ cannot be used in _Owner pointers");
                                }
                                if (!expression_is_lvalue(p_expression_node))
                                {
                                    compiler_diagnostic(1230, ctx, p_expression_node->first_token, 0U, "lvalue required as increment operand");
                                }
                                p_expression_node_new = calloc(1, 224U);
                                if (p_expression_node_new == 0U)
                                {
                                    /*throw*/ goto _CKL0;
                                }
                                p_expression_node->last_token = ctx->current;
                                p_expression_node_new->first_token = ctx->current;
                                p_expression_node_new->expression_type = 16;
                                p_expression_node_new->type = type_dup(&p_expression_node->type);
                                parser_match(ctx);
                                if (ctx->current == 0U)
                                {
                                    unexpected_end_of_file(ctx);
                                    expression_delete(p_expression_node_new);
                                    p_expression_node_new = 0U;
                                    /*throw*/ goto _CKL0;
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
                                        compiler_diagnostic(1320, ctx, p_expression_node->first_token, 0U, "operator -- cannot be used in owner pointers");
                                    }
                                    if (!expression_is_lvalue(p_expression_node))
                                    {
                                        compiler_diagnostic(1230, ctx, p_expression_node->first_token, 0U, "lvalue required as decrement operand");
                                    }
                                    p_expression_node_new = calloc(1, 224U);
                                    if (p_expression_node_new == 0U)
                                    {
                                        /*throw*/ goto _CKL0;
                                    }
                                    p_expression_node_new->first_token = ctx->current;
                                    p_expression_node_new->expression_type = 17;
                                    p_expression_node_new->type = type_dup(&p_expression_node->type);
                                    parser_match(ctx);
                                    if (ctx->current == 0U)
                                    {
                                        unexpected_end_of_file(ctx);
                                        expression_delete(p_expression_node_new);
                                        p_expression_node_new = 0U;
                                        /*throw*/ goto _CKL0;
                                    }
                                    p_expression_node_new->left = p_expression_node;
                                    p_expression_node = p_expression_node_new;
                                }
                                else
                                {
                                    struct token * p_last;

                                    p_last = previous_parser_token(ctx->current);
                                    if (p_last == 0U)
                                    {
                                        /*throw*/ goto _CKL0;
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
    /*catch*/ else _CKL0:
    {
    }
    return p_expression_node;
}

unsigned char  type_is_function(struct type * p_type);
void scope_list_push(struct scope_list * list, struct scope * s);
struct compound_statement *function_body(struct parser_ctx * ctx);
void scope_list_pop(struct scope_list * list);
struct braced_initializer *braced_initializer(struct parser_ctx * ctx);
unsigned char  type_is_const(struct type * p_type);
void object_default_initialization(struct object * p_object, unsigned char   is_constant);
int initializer_init_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * p_current_object, struct initializer * braced_initializer, unsigned char   is_constant, unsigned char   requires_constant_initialization);

struct expression *postfix_expression_type_name(struct parser_ctx * ctx, struct type_name * p_type_name_par)
{
    struct type_name * p_type_name;
    struct expression * p_expression_node;

    p_type_name = p_type_name_par;
    p_expression_node = 0U;
    /*try*/ if (1)
    {
        struct token * p_previous;

        p_expression_node = calloc(1, 224U);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        ;
        p_previous = previous_parser_token(p_type_name->first_token);
        if (p_previous == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_expression_node->first_token = p_previous;
        ;
        p_expression_node->type_name = p_type_name;
        p_type_name = 0U;
        p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);
        if (type_is_function(&p_expression_node->type_name->abstract_declarator->type))
        {
            struct scope * parameters_scope;
            struct scope * p_scope;
            struct declarator * p_current_function_opt;

            p_expression_node->expression_type = 10;
            parameters_scope = &p_expression_node->type_name->abstract_declarator->direct_declarator->function_declarator->parameters_scope;
            scope_list_push(&ctx->scopes, parameters_scope);
            p_scope = ctx->p_scope;
            ctx->p_scope = ctx->scopes.tail;
            p_current_function_opt = ctx->p_current_function_opt;
            ctx->p_current_function_opt = p_expression_node->type_name->abstract_declarator;
            p_expression_node->compound_statement = function_body(ctx);
            scope_list_pop(&ctx->scopes);
            ctx->p_scope = p_scope;
            ctx->p_current_function_opt = p_current_function_opt;
        }
        else
        {
            unsigned char   is_constant;
            struct initializer  initializer;
            unsigned char   requires_constant_initialization;

            p_expression_node->expression_type = 11;
            p_expression_node->braced_initializer = braced_initializer(ctx);
            p_expression_node->type = type_dup(&p_expression_node->type_name->type);
            if (p_expression_node->type.storage_class_specifier_flags & 1)
            {
            }
            else
            {
                int er;

                er = make_object(&p_expression_node->type, &p_expression_node->object);
                if (er != 0)
                {
                    compiler_diagnostic(740, ctx, p_expression_node->first_token, 0U, "incomplete struct/union type");
                    /*throw*/ goto _CKL0;
                }
            }
            is_constant = !!(type_is_const(&p_expression_node->type) || p_expression_node->type.storage_class_specifier_flags & 64);
            object_default_initialization(&p_expression_node->object, is_constant);
            _cake_zmem(&initializer, 12);
            initializer.braced_initializer = p_expression_node->braced_initializer;
            initializer.first_token = p_expression_node->first_token;
            requires_constant_initialization = 0;
            initializer_init_new(ctx, &p_expression_node->type, &p_expression_node->object, &initializer, is_constant, 0);
        }
        if (ctx->previous == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        p_expression_node->last_token = ctx->previous;
        p_expression_node = postfix_expression_tail(ctx, p_expression_node);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    type_name_delete(p_type_name);
    return p_expression_node;
}

struct compound_statement *compound_statement(struct parser_ctx * ctx);

struct expression *postfix_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        if (first_of_type_name_ahead(ctx))
        {
            unsigned char   is_function_type;

            ;
            p_expression_node = calloc(1, 224U);
            if (p_expression_node == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            ;
            p_expression_node->first_token = ctx->current;
            if (parser_match_tk(ctx, 40) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);
            if (parser_match_tk(ctx, 41) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            is_function_type = type_is_function(&p_expression_node->type);
            if (is_function_type)
            {
                p_expression_node->expression_type = 10;
                p_expression_node->compound_statement = compound_statement(ctx);
                if (p_expression_node->compound_statement == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_expression_node->last_token = p_expression_node->compound_statement->last_token;
            }
            else
            {
                p_expression_node->expression_type = 11;
                p_expression_node->braced_initializer = braced_initializer(ctx);
                if (p_expression_node->braced_initializer == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                if (ctx->current == 0U)
                {
                    unexpected_end_of_file(ctx);
                    /*throw*/ goto _CKL0;
                }
                p_expression_node->last_token = ctx->current;
            }
        }
        else
        {
            p_expression_node = primary_expression(ctx);
            if (p_expression_node == 0U)
            {
                /*throw*/ goto _CKL0;
            }
        }
        p_expression_node = postfix_expression_tail(ctx, p_expression_node);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

unsigned char  is_first_of_compiler_function(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(ctx->current->type == 9077 || ctx->current->type == 9079 || ctx->current->type == 9078 || ctx->current->type == 9076 || ctx->current->type == 9080 || ctx->current->type == 9081 || ctx->current->type == 9059 || ctx->current->type == 9082 || ctx->current->type == 9083 || ctx->current->type == 9084 || ctx->current->type == 9085);
}

unsigned char  is_first_of_unary_expression(struct parser_ctx * ctx)
{
    if (ctx->current == 0U)
    {
        return 0;
    }
    return !!(first_of_postfix_expression(ctx) || ctx->current->type == 11051 || ctx->current->type == 11565 || ctx->current->type == 38 || ctx->current->type == 42 || ctx->current->type == 43 || ctx->current->type == 45 || ctx->current->type == 126 || ctx->current->type == 33 || ctx->current->type == 9030 || ctx->current->type == 9031 || ctx->current->type == 9044 || is_first_of_compiler_function(ctx));
}

int  type_get_category(struct type * p_type);
unsigned char  type_is_vla(struct type * p_type);

static int check_sizeof_argument(struct parser_ctx * ctx, struct expression * p_expression, struct type * p_type)
{
    int   category;

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
            if (p_complete == 0U)
            {
                compiler_diagnostic(740, ctx, p_expression->first_token, 0U, "struct is incomplete type");
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
                    compiler_diagnostic(46, ctx, p_expression->first_token, 0U, "sizeof applied to array function parameter");
                }
            }
        }
    }
    return 0;
}

struct expression *cast_expression(struct parser_ctx * ctx);
unsigned char  object_to_bool(struct object * a);
struct type type_make_int_bool_like();
void type_integer_promotion(struct type * a);
int  type_to_object_type(struct type * type);
signed int object_to_signed_int(struct object * a);
unsigned int object_to_unsigned_int(struct object * a);
signed long object_to_signed_long(struct object * a);
unsigned long object_to_unsigned_long(struct object * a);
signed long long object_to_signed_long_long(struct object * a);
struct type type_common(struct type * p_type1, struct type * p_type2);
float object_to_float(struct object * a);
double object_to_double(struct object * a);
long double object_to_long_double(struct object * a);
struct type type_add_pointer(struct type * p_type, unsigned char   null_checks_enabled);
struct type make_size_t_type();
struct object object_make_size_t(unsigned int value);
unsigned int type_get_sizeof(struct type * p_type);
struct type type_make_size_t();
unsigned int type_get_alignof(struct type * p_type);
struct type type_make_int();
unsigned char  type_is_arithmetic(struct type * p_type);
unsigned char  type_is_scalar(struct type * p_type);
unsigned char  type_is_floating_point(struct type * p_type);

struct expression *unary_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (ctx->current->type == 11051 || ctx->current->type == 11565)
        {
            struct expression * new_expression;

            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            if (ctx->current->type == 11051)
            {
                new_expression->expression_type = 26;
            }
            else
            {
                new_expression->expression_type = 27;
            }
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            new_expression->right = unary_expression(ctx);
            if (new_expression->right == 0U)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
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
                int   op;

                new_expression = calloc(1, 224U);
                if (new_expression == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                new_expression->first_token = ctx->current;
                op_position = ctx->current;
                op = ctx->current->type;
                parser_match(ctx);
                if (ctx->current == 0U)
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(new_expression);
                    /*throw*/ goto _CKL0;
                }
                new_expression->right = cast_expression(ctx);
                if (new_expression->right == 0U)
                {
                    expression_delete(new_expression);
                    /*throw*/ goto _CKL0;
                }
                new_expression->last_token = new_expression->right->last_token;
                if (op == 33)
                {
                    new_expression->expression_type = 28;
                    if (!ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->right->object))
                    {
                        unsigned char   v;

                        v = object_to_bool(&new_expression->right->object);
                        new_expression->object = object_make_signed_int(!v);
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
                            compiler_diagnostic(850, ctx, op_position, 0U, "requires integer type");
                            expression_delete(new_expression);
                            /*throw*/ goto _CKL0;
                        }
                        new_expression->expression_type = 29;
                        promoted = type_dup(&new_expression->right->type);
                        type_integer_promotion(&promoted);
                        new_expression->type = promoted;
                        if (!ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->right->object))
                        {
                            int   vt;

                            vt = type_to_object_type(&new_expression->type);
                            /*switch*/
                            {
                                register int   _R2 = vt;
                                if (_R2 == 0) goto _CKL17; /*case 0*/
                                if (_R2 == 1) goto _CKL18; /*case 1*/
                                if (_R2 == 7) goto _CKL19; /*case 7*/
                                if (_R2 == 8) goto _CKL20; /*case 8*/
                                if (_R2 == 9) goto _CKL21; /*case 9*/
                                if (_R2 == 10) goto _CKL22; /*case 10*/
                                if (_R2 == 5) goto _CKL23; /*case 5*/
                                if (_R2 == 6) goto _CKL24; /*case 6*/
                                if (_R2 == 3) goto _CKL25; /*case 3*/
                                if (_R2 == 4) goto _CKL26; /*case 4*/
                                if (_R2 == 2) goto _CKL27; /*case 2*/
                                if (_R2 == 11) goto _CKL28; /*case 11*/
                                if (_R2 == 12) goto _CKL29; /*case 12*/
                                if (_R2 == 13) goto _CKL30; /*case 13*/
                                goto _CKL16;

                                {
                                    /*case 0*/ _CKL17:
                                    {
                                        signed int r;

                                        r = object_to_signed_int(&new_expression->right->object);
                                        new_expression->object = object_make_signed_int(~r);
                                    }
                                    /*break*/ goto _CKL16;

                                    /*case 1*/ _CKL18:
                                    {
                                        unsigned int r;

                                        r = object_to_unsigned_int(&new_expression->right->object);
                                        new_expression->object = object_make_unsigned_int(~r);
                                    }
                                    /*break*/ goto _CKL16;

                                    /*case 7*/ _CKL19:
                                    {
                                        signed long r;

                                        r = object_to_signed_long(&new_expression->right->object);
                                        new_expression->object = object_make_signed_long(~r);
                                    }
                                    /*break*/ goto _CKL16;

                                    /*case 8*/ _CKL20:
                                    {
                                        unsigned long r;

                                        r = object_to_unsigned_long(&new_expression->right->object);
                                        new_expression->object = object_make_unsigned_long(~r);
                                    }
                                    /*break*/ goto _CKL16;

                                    /*case 9*/ _CKL21:
                                    {
                                        signed long long r;

                                        r = object_to_signed_long_long(&new_expression->right->object);
                                        new_expression->object = object_make_signed_long_long(~r);
                                    }
                                    /*break*/ goto _CKL16;

                                    /*case 10*/ _CKL22:
                                    {
                                        unsigned long long r;

                                        r = object_to_unsigned_long_long(&new_expression->right->object);
                                        new_expression->object = object_make_unsigned_long_long(~r);
                                    }
                                    /*break*/ goto _CKL16;

                                    /*case 5*/ _CKL23:
                                    /*case 6*/ _CKL24:
                                    /*case 3*/ _CKL25:
                                    /*case 4*/ _CKL26:
                                    /*case 2*/ _CKL27:
                                    /*case 11*/ _CKL28:
                                    /*case 12*/ _CKL29:
                                    /*case 13*/ _CKL30:
                                    /*break*/ goto _CKL16;

                                }
                                _CKL16:;
                            }
                            ;
                        }
                    }
                    else
                    {
                        if (op == 45 || op == 43)
                        {
                            if (op == 45)
                            {
                                new_expression->expression_type = 30;
                            }
                            else
                            {
                                new_expression->expression_type = 31;
                            }
                            new_expression->type = type_common(&new_expression->right->type, &new_expression->right->type);
                            if (!ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->right->object))
                            {
                                int   vt;

                                vt = type_to_object_type(&new_expression->type);
                                /*switch*/
                                {
                                    register int   _R3 = vt;
                                    if (_R3 == 0) goto _CKL35; /*case 0*/
                                    if (_R3 == 1) goto _CKL37; /*case 1*/
                                    if (_R3 == 7) goto _CKL39; /*case 7*/
                                    if (_R3 == 8) goto _CKL41; /*case 8*/
                                    if (_R3 == 9) goto _CKL43; /*case 9*/
                                    if (_R3 == 10) goto _CKL45; /*case 10*/
                                    if (_R3 == 2) goto _CKL47; /*case 2*/
                                    if (_R3 == 3) goto _CKL48; /*case 3*/
                                    if (_R3 == 4) goto _CKL49; /*case 4*/
                                    if (_R3 == 5) goto _CKL50; /*case 5*/
                                    if (_R3 == 6) goto _CKL51; /*case 6*/
                                    if (_R3 == 11) goto _CKL52; /*case 11*/
                                    if (_R3 == 12) goto _CKL54; /*case 12*/
                                    if (_R3 == 13) goto _CKL56; /*case 13*/
                                    goto _CKL34;

                                    {
                                        /*case 0*/ _CKL35:
                                        {
                                            int a;

                                            a = object_to_signed_int(&new_expression->right->object);
                                            if (op == 45)
                                            {
                                                new_expression->object = object_make_signed_int(-a);
                                            }
                                            else
                                            {
                                                new_expression->object = object_make_signed_int(+a);
                                            }
                                        }
                                        /*break*/ goto _CKL34;

                                        /*case 1*/ _CKL37:
                                        {
                                            unsigned int a;

                                            a = object_to_unsigned_int(&new_expression->right->object);
                                            if (op == 45)
                                            {
                                                new_expression->object = object_make_unsigned_int(-a);
                                            }
                                            else
                                            {
                                                new_expression->object = object_make_unsigned_int(+a);
                                            }
                                        }
                                        /*break*/ goto _CKL34;

                                        /*case 7*/ _CKL39:
                                        {
                                            signed long a;

                                            a = object_to_signed_long(&new_expression->right->object);
                                            if (op == 45)
                                            {
                                                new_expression->object = object_make_signed_long(-a);
                                            }
                                            else
                                            {
                                                new_expression->object = object_make_signed_long(+a);
                                            }
                                        }
                                        /*break*/ goto _CKL34;

                                        /*case 8*/ _CKL41:
                                        {
                                            unsigned long a;

                                            a = object_to_unsigned_long(&new_expression->right->object);
                                            if (op == 45)
                                            {
                                                new_expression->object = object_make_unsigned_long(-a);
                                            }
                                            else
                                            {
                                                new_expression->object = object_make_unsigned_long(+a);
                                            }
                                        }
                                        /*break*/ goto _CKL34;

                                        /*case 9*/ _CKL43:
                                        {
                                            signed long long a;

                                            a = object_to_signed_long_long(&new_expression->right->object);
                                            if (op == 45)
                                            {
                                                new_expression->object = object_make_signed_long_long(-a);
                                            }
                                            else
                                            {
                                                new_expression->object = object_make_signed_long_long(+a);
                                            }
                                        }
                                        /*break*/ goto _CKL34;

                                        /*case 10*/ _CKL45:
                                        {
                                            unsigned long long a;

                                            a = object_to_unsigned_long_long(&new_expression->right->object);
                                            if (op == 45)
                                            {
                                                new_expression->object = object_make_unsigned_long_long(-a);
                                            }
                                            else
                                            {
                                                new_expression->object = object_make_unsigned_long_long(+a);
                                            }
                                        }
                                        /*break*/ goto _CKL34;

                                        /*case 2*/ _CKL47:
                                        /*case 3*/ _CKL48:
                                        /*case 4*/ _CKL49:
                                        /*case 5*/ _CKL50:
                                        /*case 6*/ _CKL51:
                                        ;
                                        expression_delete(new_expression);
                                        /*throw*/ goto _CKL0;
                                        /*break*/ goto _CKL34;

                                        /*case 11*/ _CKL52:
                                        {
                                            float a;

                                            a = object_to_float(&new_expression->right->object);
                                            if (op == 45)
                                            {
                                                new_expression->object = object_make_float(-a);
                                            }
                                            else
                                            {
                                                new_expression->object = object_make_float(+a);
                                            }
                                        }
                                        /*break*/ goto _CKL34;

                                        /*case 12*/ _CKL54:
                                        {
                                            double a;

                                            a = object_to_double(&new_expression->right->object);
                                            if (op == 45)
                                            {
                                                new_expression->object = object_make_double(-a);
                                            }
                                            else
                                            {
                                                new_expression->object = object_make_double(+a);
                                            }
                                        }
                                        /*break*/ goto _CKL34;

                                        /*case 13*/ _CKL56:
                                        {
                                            long double a;

                                            a = object_to_long_double(&new_expression->right->object);
                                            if (op == 45)
                                            {
                                                new_expression->object = object_make_long_double(-a);
                                            }
                                            else
                                            {
                                                new_expression->object = object_make_long_double(+a);
                                            }
                                        }
                                        /*break*/ goto _CKL34;

                                    }
                                    _CKL34:;
                                }
                                ;
                            }
                        }
                        else
                        {
                            if (op == 42)
                            {
                                new_expression->expression_type = 32;
                                if (!type_is_pointer_or_array(&new_expression->right->type))
                                {
                                    compiler_diagnostic(780, ctx, op_position, 0U, "indirection requires pointer operand");
                                }
                                if (type_is_pointer(&new_expression->right->type))
                                {
                                    new_expression->type = type_remove_pointer(&new_expression->right->type);
                                }
                                else
                                {
                                    compiler_diagnostic(19, ctx, op_position, 0U, "array indirection");
                                    new_expression->type = get_array_item_type(&new_expression->right->type);
                                }
                            }
                            else
                            {
                                if (op == 38)
                                {
                                    new_expression->expression_type = 33;
                                    if (!expression_is_lvalue(new_expression->right))
                                    {
                                        compiler_diagnostic(1220, ctx, new_expression->right->first_token, 0U, "lvalue required as unary '&' operand");
                                    }
                                    if (new_expression->right->type.storage_class_specifier_flags & 32)
                                    {
                                        char * variable_name;

                                        variable_name = "?";
                                        if (new_expression->right->declarator && new_expression->right->declarator->name_opt)
                                        {
                                            variable_name = new_expression->right->declarator->name_opt->lexeme;
                                        }
                                        compiler_diagnostic(1220, ctx, new_expression->right->first_token, 0U, "address of register variable 'x' requested", variable_name);
                                    }
                                    new_expression->type = type_add_pointer(&new_expression->right->type, ctx->options.null_checks_enabled);
                                    new_expression->type.address_of = 1;
                                }
                                else
                                {
                                    expression_delete(new_expression);
                                    compiler_diagnostic(790, ctx, ctx->current, 0U, "invalid token");
                                    /*throw*/ goto _CKL0;
                                }
                            }
                        }
                    }
                }
                p_expression_node = new_expression;
            }
            else
            {
                if (ctx->current->type == 9030)
                {
                    unsigned char   disable_evaluation_copy;
                    struct expression * new_expression;

                    disable_evaluation_copy = ctx->evaluation_is_disabled;
                    ctx->evaluation_is_disabled = 1;
                    parser_match(ctx);
                    if (ctx->current == 0U)
                    {
                        unexpected_end_of_file(ctx);
                        /*throw*/ goto _CKL0;
                    }
                    new_expression = calloc(1, 224U);
                    if (new_expression == 0U)
                    {
                        /*throw*/ goto _CKL0;
                    }
                    new_expression->first_token = ctx->current;
                    if (first_of_type_name_ahead(ctx))
                    {
                        new_expression->expression_type = 19;
                        if (parser_match_tk(ctx, 40) != 0)
                        {
                            expression_delete(new_expression);
                            new_expression = 0U;
                            /*throw*/ goto _CKL0;
                        }
                        new_expression->type_name = type_name(ctx);
                        if (new_expression->type_name == 0U)
                        {
                            expression_delete(new_expression);
                            new_expression = 0U;
                            /*throw*/ goto _CKL0;
                        }
                        new_expression->type = make_size_t_type();
                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            expression_delete(new_expression);
                            /*throw*/ goto _CKL0;
                        }
                        new_expression->last_token = ctx->current;
                        if (parser_match_tk(ctx, 41) != 0)
                        {
                            expression_delete(new_expression);
                            /*throw*/ goto _CKL0;
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
                                new_expression->object = object_make_size_t(type_get_sizeof(&new_expression->type_name->abstract_declarator->type));
                            }
                        }
                    }
                    else
                    {
                        new_expression->right = unary_expression(ctx);
                        if (new_expression->right == 0U)
                        {
                            ctx->evaluation_is_disabled = disable_evaluation_copy;
                            expression_delete(new_expression);
                            /*throw*/ goto _CKL0;
                        }
                        new_expression->expression_type = 18;
                        if (check_sizeof_argument(ctx, new_expression->right, &new_expression->right->type) != 0)
                        {
                            expression_delete(new_expression);
                            /*throw*/ goto _CKL0;
                        }
                        if (type_is_vla(&new_expression->right->type))
                        {
                        }
                        else
                        {
                            new_expression->object = object_make_size_t(type_get_sizeof(&new_expression->right->type));
                        }
                    }
                    type_destroy(&new_expression->type);
                    new_expression->type = type_make_size_t();
                    p_expression_node = new_expression;
                    ctx->evaluation_is_disabled = disable_evaluation_copy;
                }
                else
                {
                    if (ctx->current->type == 9031)
                    {
                        struct expression * new_expression;

                        new_expression = calloc(1, 224U);
                        if (new_expression == 0U)
                        {
                            /*throw*/ goto _CKL0;
                        }
                        new_expression->first_token = ctx->current;
                        parser_match(ctx);
                        if (ctx->current == 0U)
                        {
                            unexpected_end_of_file(ctx);
                            expression_delete(new_expression);
                            /*throw*/ goto _CKL0;
                        }
                        new_expression->expression_type = 20;
                        if (first_of_type_name_ahead(ctx))
                        {
                            int nelements;

                            if (parser_match_tk(ctx, 40) != 0)
                            {
                                expression_delete(new_expression);
                                /*throw*/ goto _CKL0;
                            }
                            new_expression->type_name = type_name(ctx);
                            if (new_expression->type_name == 0U)
                            {
                                expression_delete(new_expression);
                                /*throw*/ goto _CKL0;
                            }
                            new_expression->type = make_size_t_type();
                            if (ctx->current == 0U)
                            {
                                unexpected_end_of_file(ctx);
                                expression_delete(new_expression);
                                /*throw*/ goto _CKL0;
                            }
                            new_expression->last_token = ctx->current;
                            if (parser_match_tk(ctx, 41) != 0)
                            {
                                expression_delete(new_expression);
                                /*throw*/ goto _CKL0;
                            }
                            if (!type_is_array(&new_expression->type_name->abstract_declarator->type))
                            {
                                compiler_diagnostic(1390, ctx, new_expression->type_name->first_token, 0U, "argument of _Countof must be an array");
                                expression_delete(new_expression);
                                /*throw*/ goto _CKL0;
                            }
                            nelements = new_expression->type_name->abstract_declarator->type.num_of_elements;
                            if (nelements > 0)
                            {
                                new_expression->object = object_make_size_t(nelements);
                            }
                        }
                        else
                        {
                            unsigned char   disable_evaluation_copy;
                            int nelements;

                            if (parser_match_tk(ctx, 40) != 0)
                            {
                                expression_delete(new_expression);
                                new_expression = 0U;
                                /*throw*/ goto _CKL0;
                            }
                            disable_evaluation_copy = ctx->evaluation_is_disabled;
                            ctx->evaluation_is_disabled = 1;
                            new_expression->right = unary_expression(ctx);
                            ctx->evaluation_is_disabled = disable_evaluation_copy;
                            if (new_expression->right == 0U)
                            {
                                expression_delete(new_expression);
                                /*throw*/ goto _CKL0;
                            }
                            if (ctx->current == 0U)
                            {
                                unexpected_end_of_file(ctx);
                                expression_delete(new_expression);
                                /*throw*/ goto _CKL0;
                            }
                            new_expression->last_token = ctx->current;
                            if (parser_match_tk(ctx, 41) != 0)
                            {
                                expression_delete(new_expression);
                                /*throw*/ goto _CKL0;
                            }
                            if (!type_is_array(&new_expression->right->type))
                            {
                                compiler_diagnostic(1390, ctx, new_expression->right->first_token, 0U, "argument of _Countof must be an array");
                                expression_delete(new_expression);
                                /*throw*/ goto _CKL0;
                            }
                            nelements = new_expression->right->type.num_of_elements;
                            if (nelements > 0)
                            {
                                new_expression->object = object_make_size_t(nelements);
                            }
                            else
                            {
                            }
                        }
                        type_destroy(&new_expression->type);
                        new_expression->type = type_make_size_t();
                        p_expression_node = new_expression;
                    }
                    else
                    {
                        if (ctx->current->type == 9059)
                        {
                            struct expression * new_expression;

                            new_expression = calloc(1, 224U);
                            if (new_expression == 0U)
                            {
                                /*throw*/ goto _CKL0;
                            }
                            new_expression->expression_type = 25;
                            new_expression->first_token = ctx->current;
                            parser_match(ctx);
                            if (ctx->current == 0U || parser_match_tk(ctx, 40) != 0)
                            {
                                expression_delete(new_expression);
                                new_expression = 0U;
                                /*throw*/ goto _CKL0;
                            }
                            new_expression->right = expression(ctx);
                            if (parser_match_tk(ctx, 41) != 0)
                            {
                                expression_delete(new_expression);
                                new_expression = 0U;
                                /*throw*/ goto _CKL0;
                            }
                            return new_expression;
                        }
                        else
                        {
                            if (ctx->current->type == 9044)
                            {
                                struct expression * new_expression;

                                new_expression = calloc(1, 224U);
                                if (new_expression == 0U)
                                {
                                    /*throw*/ goto _CKL0;
                                }
                                new_expression->expression_type = 24;
                                new_expression->first_token = ctx->current;
                                parser_match(ctx);
                                if (ctx->current == 0U || parser_match_tk(ctx, 40) != 0)
                                {
                                    expression_delete(new_expression);
                                    new_expression = 0U;
                                    /*throw*/ goto _CKL0;
                                }
                                new_expression->type_name = type_name(ctx);
                                if (parser_match_tk(ctx, 41) != 0)
                                {
                                    expression_delete(new_expression);
                                    new_expression = 0U;
                                    /*throw*/ goto _CKL0;
                                }
                                if (!ctx->evaluation_is_disabled)
                                {
                                    new_expression->object = object_make_size_t(type_get_alignof(&new_expression->type_name->type));
                                }
                                new_expression->type = type_make_int();
                                ;
                                new_expression->last_token = ctx->previous;
                                p_expression_node = new_expression;
                            }
                            else
                            {
                                if (ctx->current->type == 9077 || ctx->current->type == 9079 || ctx->current->type == 9078 || ctx->current->type == 9076 || ctx->current->type == 9080 || ctx->current->type == 9081 || ctx->current->type == 9083 || ctx->current->type == 9082 || ctx->current->type == 9084 || ctx->current->type == 9085)
                                {
                                    unsigned char   disable_evaluation_copy;
                                    struct token * traits_token;
                                    struct expression * new_expression;
                                    struct type * p_type;

                                    disable_evaluation_copy = ctx->evaluation_is_disabled;
                                    ctx->evaluation_is_disabled = 1;
                                    traits_token = ctx->current;
                                    new_expression = calloc(1, 224U);
                                    if (new_expression == 0U)
                                    {
                                        /*throw*/ goto _CKL0;
                                    }
                                    new_expression->first_token = ctx->current;
                                    new_expression->expression_type = 21;
                                    parser_match(ctx);
                                    if (ctx->current == 0U)
                                    {
                                        unexpected_end_of_file(ctx);
                                        expression_delete(new_expression);
                                        new_expression = 0U;
                                        /*throw*/ goto _CKL0;
                                    }
                                    p_type = 0U;
                                    if (first_of_type_name_ahead(ctx))
                                    {
                                        if (parser_match_tk(ctx, 40) != 0)
                                        {
                                            expression_delete(new_expression);
                                            new_expression = 0U;
                                            /*throw*/ goto _CKL0;
                                        }
                                        new_expression->type_name = type_name(ctx);
                                        if (new_expression->type_name == 0U)
                                        {
                                            expression_delete(new_expression);
                                            new_expression = 0U;
                                            /*throw*/ goto _CKL0;
                                        }
                                        if (ctx->current == 0U)
                                        {
                                            unexpected_end_of_file(ctx);
                                            expression_delete(new_expression);
                                            new_expression = 0U;
                                            /*throw*/ goto _CKL0;
                                        }
                                        new_expression->last_token = ctx->current;
                                        if (parser_match_tk(ctx, 41) != 0)
                                        {
                                            expression_delete(new_expression);
                                            new_expression = 0U;
                                            /*throw*/ goto _CKL0;
                                        }
                                        p_type = &new_expression->type_name->abstract_declarator->type;
                                    }
                                    else
                                    {
                                        new_expression->right = unary_expression(ctx);
                                        if (new_expression->right == 0U)
                                        {
                                            ctx->evaluation_is_disabled = disable_evaluation_copy;
                                            expression_delete(new_expression);
                                            /*throw*/ goto _CKL0;
                                        }
                                        p_type = &new_expression->right->type;
                                        if (ctx->previous == 0U)
                                        {
                                            expression_delete(new_expression);
                                            new_expression = 0U;
                                            /*throw*/ goto _CKL0;
                                        }
                                        new_expression->last_token = ctx->previous;
                                    }
                                    /*switch*/
                                    {
                                        register int   _R4 = traits_token->type;
                                        if (_R4 == 9077) goto _CKL114; /*case 9077*/
                                        if (_R4 == 9078) goto _CKL115; /*case 9078*/
                                        if (_R4 == 9079) goto _CKL116; /*case 9079*/
                                        if (_R4 == 9076) goto _CKL117; /*case 9076*/
                                        if (_R4 == 9081) goto _CKL118; /*case 9081*/
                                        if (_R4 == 9080) goto _CKL119; /*case 9080*/
                                        if (_R4 == 9083) goto _CKL120; /*case 9083*/
                                        if (_R4 == 9082) goto _CKL121; /*case 9082*/
                                        if (_R4 == 9084) goto _CKL122; /*case 9084*/
                                        if (_R4 == 9085) goto _CKL123; /*case 9085*/
                                        goto /*default*/ _CKL124;

                                        {
                                            /*case 9077*/ _CKL114:
                                            ;
                                            new_expression->object = object_make_signed_int(expression_is_lvalue(new_expression->right));
                                            /*break*/ goto _CKL113;

                                            /*case 9078*/ _CKL115:
                                            new_expression->object = object_make_signed_int(type_is_const(p_type));
                                            /*break*/ goto _CKL113;

                                            /*case 9079*/ _CKL116:
                                            new_expression->object = object_make_signed_int(type_is_owner(p_type));
                                            /*break*/ goto _CKL113;

                                            /*case 9076*/ _CKL117:
                                            new_expression->object = object_make_signed_int(type_is_pointer(p_type));
                                            /*break*/ goto _CKL113;

                                            /*case 9081*/ _CKL118:
                                            new_expression->object = object_make_signed_int(type_is_function(p_type));
                                            /*break*/ goto _CKL113;

                                            /*case 9080*/ _CKL119:
                                            new_expression->object = object_make_signed_int(type_is_array(p_type));
                                            /*break*/ goto _CKL113;

                                            /*case 9083*/ _CKL120:
                                            new_expression->object = object_make_signed_int(type_is_arithmetic(p_type));
                                            /*break*/ goto _CKL113;

                                            /*case 9082*/ _CKL121:
                                            new_expression->object = object_make_signed_int(type_is_scalar(p_type));
                                            /*break*/ goto _CKL113;

                                            /*case 9084*/ _CKL122:
                                            new_expression->object = object_make_signed_int(type_is_floating_point(p_type));
                                            /*break*/ goto _CKL113;

                                            /*case 9085*/ _CKL123:
                                            new_expression->object = object_make_signed_int(type_is_integer(p_type));
                                            /*break*/ goto _CKL113;

                                            /*default*/ _CKL124:
                                            ;
                                        }
                                        _CKL113:;
                                    }
                                    new_expression->type = type_make_int_bool_like();
                                    p_expression_node = new_expression;
                                    ctx->evaluation_is_disabled = disable_evaluation_copy;
                                }
                                else
                                {
                                    p_expression_node = postfix_expression(ctx);
                                    if (p_expression_node == 0U)
                                    {
                                        /*throw*/ goto _CKL0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

unsigned char  type_is_nullptr_t(struct type * p_type);
unsigned char  type_is_bool(struct type * p_type);
unsigned char  expression_is_null_pointer_constant(struct expression * expression);
struct object object_cast(int   e, struct object * a);

struct expression *cast_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        if (first_of_type_name_ahead(ctx))
        {
            p_expression_node = calloc(1, 224U);
            if (p_expression_node == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_expression_node->first_token = ctx->current;
            p_expression_node->expression_type = 34;
            if (parser_match_tk(ctx, 40) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == 0U)
            {
                expression_delete(p_expression_node);
                p_expression_node = 0U;
                /*throw*/ goto _CKL0;
            }
            p_expression_node->type = type_dup(&p_expression_node->type_name->type);
            if (parser_match_tk(ctx, 41) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            if (ctx->current->type == 123)
            {
                struct expression * new_expression;

                new_expression = postfix_expression_type_name(ctx, p_expression_node->type_name);
                p_expression_node->type_name = 0U;
                if (new_expression == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                expression_delete(p_expression_node);
                p_expression_node = new_expression;
            }
            else
            {
                if (is_first_of_unary_expression(ctx))
                {
                    p_expression_node->left = cast_expression(ctx);
                    if (p_expression_node->left == 0U)
                    {
                        expression_delete(p_expression_node);
                        p_expression_node = 0U;
                        /*throw*/ goto _CKL0;
                    }
                    if (type_is_floating_point(&p_expression_node->type) && type_is_pointer(&p_expression_node->left->type))
                    {
                        compiler_diagnostic(1790, ctx, p_expression_node->first_token, 0U, "pointer type cannot be converted to any floating type");
                    }
                    else
                    {
                        if (type_is_pointer(&p_expression_node->type) && type_is_floating_point(&p_expression_node->left->type))
                        {
                            compiler_diagnostic(1800, ctx, p_expression_node->first_token, 0U, "A floating type cannot be converted to any pointer type");
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
                                    compiler_diagnostic(1810, ctx, p_expression_node->first_token, 0U, "cannot cast nullptr_t to this type");
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
                                        compiler_diagnostic(1810, ctx, p_expression_node->left->first_token, 0U, "cannot cast this expression to nullptr_t");
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
                                compiler_diagnostic(25, ctx, p_expression_node->first_token, 0U, "discarding _Owner pointer");
                            }
                            else
                            {
                                compiler_diagnostic(25, ctx, p_expression_node->first_token, 0U, "discarding _Owner");
                            }
                        }
                    }
                    type_destroy(&p_expression_node->type);
                    p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);
                    if (!ctx->evaluation_is_disabled && object_has_constant_value(&p_expression_node->left->object))
                    {
                        int   vt;

                        vt = type_to_object_type(&p_expression_node->type);
                        p_expression_node->object = object_cast(vt, &p_expression_node->left->object);
                    }
                    p_expression_node->type.storage_class_specifier_flags = p_expression_node->left->type.storage_class_specifier_flags;
                }
                else
                {
                    compiler_diagnostic(650, ctx, ctx->current, 0U, "expected expression");
                }
            }
        }
        else
        {
            if (is_first_of_unary_expression(ctx))
            {
                p_expression_node = unary_expression(ctx);
                if (p_expression_node == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            else
            {
                compiler_diagnostic(650, ctx, ctx->current, 0U, "expected expression");
                ;
                /*throw*/ goto _CKL0;
            }
        }
        if (ctx->current == 0U || ctx->previous == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_expression_node->last_token = ctx->previous;
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

unsigned char  signed_long_long_add(signed long long * result, signed long long a, signed long long b);
unsigned char  signed_long_long_sub(signed long long * result, signed long long a, signed long long b);
unsigned char  signed_long_long_mul(signed long long * result, signed long long a, signed long long b);
unsigned char  unsigned_long_long_add(unsigned long long * result, unsigned long long a, unsigned long long b);
unsigned char  unsigned_long_long_sub(unsigned long long * result, unsigned long long a, unsigned long long b);
unsigned char  unsigned_long_long_mul(unsigned long long * result, unsigned long long a, unsigned long long b);

int execute_arithmetic(struct parser_ctx * ctx, struct expression * new_expression, int op, struct object * result)
{
    struct type  common_type;
    struct object  empty;

    ;
    _cake_zmem(&common_type, 60);
    /*try*/ if (1)
    {
        struct object  value;

        if (new_expression->left == 0U || new_expression->right == 0U)
        {
            ;
            /*throw*/ goto _CKL0;
        }
        _cake_zmem(&value, 96);
        /*switch*/
        {
            register int _R5 = op;
            if (_R5 == 43) goto _CKL3; /*case 43*/
            if (_R5 == 45) goto _CKL4; /*case 45*/
            if (_R5 == 42) goto _CKL5; /*case 42*/
            if (_R5 == 47) goto _CKL6; /*case 47*/
            if (_R5 == 37) goto _CKL7; /*case 37*/
            if (_R5 == 62) goto _CKL8; /*case 62*/
            if (_R5 == 60) goto _CKL9; /*case 60*/
            if (_R5 == 15933) goto _CKL10; /*case 15933*/
            if (_R5 == 15421) goto _CKL11; /*case 15421*/
            if (_R5 == 15677) goto _CKL12; /*case 15677*/
            if (_R5 == 8509) goto _CKL13; /*case 8509*/
            goto /*default*/ _CKL14;

            {
                /*case 43*/ _CKL3:
                /*case 45*/ _CKL4:
                /*case 42*/ _CKL5:
                /*case 47*/ _CKL6:
                /*case 37*/ _CKL7:
                /*case 62*/ _CKL8:
                /*case 60*/ _CKL9:
                /*case 15933*/ _CKL10:
                /*case 15421*/ _CKL11:
                /*case 15677*/ _CKL12:
                /*case 8509*/ _CKL13:
                /*break*/ goto _CKL2;

                /*default*/ _CKL14:
                ;
                /*throw*/ goto _CKL0;
            }
            _CKL2:;
        }
        if (!type_is_arithmetic(&new_expression->left->type))
        {
            compiler_diagnostic(840, ctx, ctx->current, 0U, "left type must be an arithmetic type");
            /*throw*/ goto _CKL0;
        }
        if (!type_is_arithmetic(&new_expression->right->type))
        {
            compiler_diagnostic(840, ctx, ctx->current, 0U, "right type must be an arithmetic type");
            /*throw*/ goto _CKL0;
        }
        if (!ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
        {
            struct marker  m;
            int   vt;

            m.file = 0;
            m.line = 0;
            m.start_col = 0;
            m.end_col = 0;
            m.p_token_caret = 0;
            m.p_token_begin = new_expression->left->first_token;
            m.p_token_end = new_expression->right->last_token;
            common_type = type_common(&new_expression->left->type, &new_expression->right->type);
            vt = type_to_object_type(&common_type);
            /*switch*/
            {
                register int   _R6 = vt;
                if (_R6 == 0) goto _CKL19; /*case 0*/
                if (_R6 == 1) goto _CKL40; /*case 1*/
                if (_R6 == 7) goto _CKL60; /*case 7*/
                if (_R6 == 8) goto _CKL81; /*case 8*/
                if (_R6 == 9) goto _CKL101; /*case 9*/
                if (_R6 == 10) goto _CKL119; /*case 10*/
                if (_R6 == 2) goto _CKL136; /*case 2*/
                if (_R6 == 3) goto _CKL137; /*case 3*/
                if (_R6 == 4) goto _CKL138; /*case 4*/
                if (_R6 == 5) goto _CKL139; /*case 5*/
                if (_R6 == 6) goto _CKL140; /*case 6*/
                if (_R6 == 11) goto _CKL141; /*case 11*/
                if (_R6 == 12) goto _CKL154; /*case 12*/
                if (_R6 == 13) goto _CKL167; /*case 13*/
                goto _CKL18;

                {
                    /*case 0*/ _CKL19:
                    {
                        int a;
                        int b;

                        a = object_to_signed_int(&new_expression->left->object);
                        b = object_to_signed_int(&new_expression->right->object);
                        if (op == 43)
                        {
                            int computed_result;
                            signed long long exact_result;

                            computed_result = a + b;
                            if (signed_long_long_add(&exact_result, a, b))
                            {
                                if (computed_result != exact_result)
                                {
                                    compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                }
                            }
                            else
                            {
                                ;
                            }
                            value = object_make_signed_int(computed_result);
                        }
                        else
                        {
                            if (op == 45)
                            {
                                int computed_result;
                                signed long long exact_result;

                                computed_result = a - b;
                                if (signed_long_long_sub(&exact_result, a, b))
                                {
                                    if (computed_result != exact_result)
                                    {
                                        compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                    }
                                }
                                else
                                {
                                    ;
                                }
                                value = object_make_signed_int(computed_result);
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    int computed_result;
                                    signed long long exact_result;

                                    computed_result = a * b;
                                    if (signed_long_long_mul(&exact_result, a, b))
                                    {
                                        if (computed_result != exact_result)
                                        {
                                            compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                        }
                                    }
                                    else
                                    {
                                        ;
                                    }
                                    value = object_make_signed_int(computed_result);
                                }
                                else
                                {
                                    if (op == 47)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                        }
                                        else
                                        {
                                            if (a == -2147483648 && b == -1)
                                            {
                                                compiler_diagnostic(50, ctx, new_expression->right->first_token, 0U, "integer overflow");
                                                value = object_make_signed_int(-2147483648);
                                            }
                                            else
                                            {
                                                value = object_make_signed_int(a / b);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (op == 37)
                                        {
                                            if (b == 0)
                                            {
                                                compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                            }
                                            else
                                            {
                                                value = object_make_signed_int(a % b);
                                            }
                                        }
                                        else
                                        {
                                            if (op == 62)
                                            {
                                                value = object_make_signed_int(a > b);
                                            }
                                            else
                                            {
                                                if (op == 60)
                                                {
                                                    value = object_make_signed_int(a < b);
                                                }
                                                else
                                                {
                                                    if (op == 15933)
                                                    {
                                                        value = object_make_signed_int(a >= b);
                                                    }
                                                    else
                                                    {
                                                        if (op == 15421)
                                                        {
                                                            value = object_make_signed_int(a <= b);
                                                        }
                                                        else
                                                        {
                                                            if (op == 15677)
                                                            {
                                                                value = object_make_signed_int(a == b);
                                                            }
                                                            else
                                                            {
                                                                if (op == 8509)
                                                                {
                                                                    value = object_make_signed_int(a != b);
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
                    /*break*/ goto _CKL18;

                    /*case 1*/ _CKL40:
                    {
                        unsigned int a;
                        unsigned int b;

                        a = object_to_unsigned_int(&new_expression->left->object);
                        b = object_to_unsigned_int(&new_expression->right->object);
                        if (op == 43)
                        {
                            unsigned int computed_result;
                            unsigned long long exact_result;

                            computed_result = a + b;
                            if (unsigned_long_long_add(&exact_result, a, b))
                            {
                                if (computed_result != exact_result)
                                {
                                    compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                }
                            }
                            else
                            {
                                ;
                            }
                            value = object_make_signed_int(computed_result);
                        }
                        else
                        {
                            if (op == 45)
                            {
                                unsigned int computed_result;
                                unsigned long long exact_result;

                                computed_result = a - b;
                                if (unsigned_long_long_sub(&exact_result, a, b))
                                {
                                    if (computed_result != exact_result)
                                    {
                                        compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                    }
                                }
                                else
                                {
                                    ;
                                }
                                value = object_make_signed_int(computed_result);
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    unsigned int computed_result;
                                    unsigned long long exact_result;

                                    computed_result = a * b;
                                    if (unsigned_long_long_mul(&exact_result, a, b))
                                    {
                                        if (computed_result != exact_result)
                                        {
                                            compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                        }
                                    }
                                    else
                                    {
                                        ;
                                    }
                                    value = object_make_signed_int(computed_result);
                                }
                                else
                                {
                                    if (op == 47)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                        }
                                        else
                                        {
                                            value = object_make_unsigned_int(a / b);
                                        }
                                    }
                                    else
                                    {
                                        if (op == 37)
                                        {
                                            if (b == 0)
                                            {
                                                compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                                /*throw*/ goto _CKL0;
                                            }
                                            value = object_make_unsigned_int(a % b);
                                        }
                                        else
                                        {
                                            if (op == 62)
                                            {
                                                value = object_make_signed_int(a > b);
                                            }
                                            else
                                            {
                                                if (op == 60)
                                                {
                                                    value = object_make_signed_int(a < b);
                                                }
                                                else
                                                {
                                                    if (op == 15933)
                                                    {
                                                        value = object_make_signed_int(a >= b);
                                                    }
                                                    else
                                                    {
                                                        if (op == 15421)
                                                        {
                                                            value = object_make_signed_int(a <= b);
                                                        }
                                                        else
                                                        {
                                                            if (op == 15677)
                                                            {
                                                                value = object_make_signed_int(a == b);
                                                            }
                                                            else
                                                            {
                                                                if (op == 8509)
                                                                {
                                                                    value = object_make_signed_int(a != b);
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
                    /*break*/ goto _CKL18;

                    /*case 7*/ _CKL60:
                    {
                        signed long a;
                        signed long b;

                        a = object_to_signed_long(&new_expression->left->object);
                        b = object_to_signed_long(&new_expression->right->object);
                        if (op == 43)
                        {
                            signed long computed_result;
                            signed long long exact_result;

                            computed_result = a + b;
                            if (signed_long_long_add(&exact_result, a, b))
                            {
                                if (computed_result != exact_result)
                                {
                                    compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                }
                            }
                            else
                            {
                                compiler_diagnostic(50, ctx, 0U, &m, "integer overflow");
                            }
                            value = object_make_signed_long(computed_result);
                        }
                        else
                        {
                            if (op == 45)
                            {
                                signed long computed_result;
                                signed long long exact_result;

                                computed_result = a - b;
                                if (signed_long_long_sub(&exact_result, a, b))
                                {
                                    if (computed_result != exact_result)
                                    {
                                        compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                    }
                                }
                                else
                                {
                                    compiler_diagnostic(50, ctx, 0U, &m, "integer overflow");
                                }
                                value = object_make_signed_long(computed_result);
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    signed long computed_result;
                                    signed long long exact_result;

                                    computed_result = a * b;
                                    if (signed_long_long_mul(&exact_result, a, b))
                                    {
                                        if (computed_result != exact_result)
                                        {
                                            compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic(50, ctx, 0U, &m, "integer overflow");
                                    }
                                    value = object_make_signed_long(computed_result);
                                }
                                else
                                {
                                    if (op == 47)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                        }
                                        else
                                        {
                                            if (a == -2147483648L && b == -1)
                                            {
                                                compiler_diagnostic(50, ctx, new_expression->right->first_token, 0U, "integer overflow");
                                                value = object_make_signed_long(-2147483648L);
                                            }
                                            else
                                            {
                                                value = object_make_signed_long(a / b);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (op == 37)
                                        {
                                            if (b == 0)
                                            {
                                                compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                            }
                                            else
                                            {
                                                value = object_make_signed_long(a % b);
                                            }
                                        }
                                        else
                                        {
                                            if (op == 62)
                                            {
                                                value = object_make_signed_long(a > b);
                                            }
                                            else
                                            {
                                                if (op == 60)
                                                {
                                                    value = object_make_signed_long(a < b);
                                                }
                                                else
                                                {
                                                    if (op == 15933)
                                                    {
                                                        value = object_make_signed_long(a >= b);
                                                    }
                                                    else
                                                    {
                                                        if (op == 15421)
                                                        {
                                                            value = object_make_signed_long(a <= b);
                                                        }
                                                        else
                                                        {
                                                            if (op == 15677)
                                                            {
                                                                value = object_make_signed_long(a == b);
                                                            }
                                                            else
                                                            {
                                                                if (op == 8509)
                                                                {
                                                                    value = object_make_signed_long(a != b);
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
                    /*break*/ goto _CKL18;

                    /*case 8*/ _CKL81:
                    {
                        unsigned long a;
                        unsigned long b;

                        a = object_to_unsigned_long(&new_expression->left->object);
                        b = object_to_unsigned_long(&new_expression->right->object);
                        if (op == 43)
                        {
                            unsigned long computed_result;
                            unsigned long long exact_result;

                            computed_result = a + b;
                            if (unsigned_long_long_add(&exact_result, a, b))
                            {
                                if (computed_result != exact_result)
                                {
                                    compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                }
                            }
                            else
                            {
                                compiler_diagnostic(50, ctx, 0U, &m, "integer overflow");
                            }
                            value = object_make_unsigned_long(computed_result);
                        }
                        else
                        {
                            if (op == 45)
                            {
                                unsigned long computed_result;
                                unsigned long long exact_result;

                                computed_result = a - b;
                                if (unsigned_long_long_sub(&exact_result, a, b))
                                {
                                    if (computed_result != exact_result)
                                    {
                                        compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                    }
                                }
                                else
                                {
                                    compiler_diagnostic(50, ctx, 0U, &m, "integer overflow");
                                }
                                value = object_make_unsigned_long(computed_result);
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    unsigned long computed_result;
                                    unsigned long long exact_result;

                                    computed_result = a * b;
                                    if (unsigned_long_long_mul(&exact_result, a, b))
                                    {
                                        if (computed_result != exact_result)
                                        {
                                            compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic(50, ctx, 0U, &m, "integer overflow");
                                    }
                                    value = object_make_unsigned_long(computed_result);
                                }
                                else
                                {
                                    if (op == 47)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                        }
                                        else
                                        {
                                            value = object_make_unsigned_long(a / b);
                                        }
                                    }
                                    else
                                    {
                                        if (op == 37)
                                        {
                                            if (b == 0)
                                            {
                                                compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                                /*throw*/ goto _CKL0;
                                            }
                                            value = object_make_unsigned_long(a % b);
                                        }
                                        else
                                        {
                                            if (op == 62)
                                            {
                                                value = object_make_unsigned_long(a > b);
                                            }
                                            else
                                            {
                                                if (op == 60)
                                                {
                                                    value = object_make_unsigned_long(a < b);
                                                }
                                                else
                                                {
                                                    if (op == 15933)
                                                    {
                                                        value = object_make_unsigned_long(a >= b);
                                                    }
                                                    else
                                                    {
                                                        if (op == 15421)
                                                        {
                                                            value = object_make_unsigned_long(a <= b);
                                                        }
                                                        else
                                                        {
                                                            if (op == 15677)
                                                            {
                                                                value = object_make_unsigned_long(a == b);
                                                            }
                                                            else
                                                            {
                                                                if (op == 8509)
                                                                {
                                                                    value = object_make_unsigned_long(a != b);
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
                    /*break*/ goto _CKL18;

                    /*case 9*/ _CKL101:
                    {
                        long long a;
                        long long b;

                        a = object_to_signed_long_long(&new_expression->left->object);
                        b = object_to_signed_long_long(&new_expression->right->object);
                        if (op == 43)
                        {
                            long long computed_result;
                            signed long long exact_result;

                            computed_result = a + b;
                            if (!signed_long_long_add(&exact_result, a, b))
                            {
                                compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%dll'. ", computed_result);
                            }
                            value = object_make_signed_long_long(computed_result);
                        }
                        else
                        {
                            if (op == 45)
                            {
                                long long computed_result;
                                signed long long exact_result;

                                computed_result = a - b;
                                if (!signed_long_long_sub(&exact_result, a, b))
                                {
                                    compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%dll'.", computed_result);
                                }
                                value = object_make_signed_long_long(computed_result);
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    long long computed_result;
                                    signed long long exact_result;

                                    computed_result = a * b;
                                    if (!signed_long_long_mul(&exact_result, a, b))
                                    {
                                        compiler_diagnostic(50, ctx, 0U, &m, "integer overflow results in '%dll", computed_result);
                                    }
                                    value = object_make_signed_long_long(computed_result);
                                }
                                else
                                {
                                    if (op == 47)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                        }
                                        else
                                        {
                                            if (a == -9223372036854775808LL && b == -1)
                                            {
                                                compiler_diagnostic(50, ctx, new_expression->right->first_token, 0U, "integer overflow");
                                                value = object_make_signed_long_long(-9223372036854775808LL);
                                            }
                                            else
                                            {
                                                value = object_make_signed_long_long(a / b);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (op == 37)
                                        {
                                            if (b == 0)
                                            {
                                                compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                                /*throw*/ goto _CKL0;
                                            }
                                            value = object_make_signed_long_long(a % b);
                                        }
                                        else
                                        {
                                            if (op == 62)
                                            {
                                                value = object_make_signed_int(a > b);
                                            }
                                            else
                                            {
                                                if (op == 60)
                                                {
                                                    value = object_make_signed_int(a < b);
                                                }
                                                else
                                                {
                                                    if (op == 15933)
                                                    {
                                                        value = object_make_signed_int(a >= b);
                                                    }
                                                    else
                                                    {
                                                        if (op == 15421)
                                                        {
                                                            value = object_make_signed_int(a <= b);
                                                        }
                                                        else
                                                        {
                                                            if (op == 15677)
                                                            {
                                                                value = object_make_signed_int(a == b);
                                                            }
                                                            else
                                                            {
                                                                if (op == 8509)
                                                                {
                                                                    value = object_make_signed_int(a != b);
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
                    /*break*/ goto _CKL18;

                    /*case 10*/ _CKL119:
                    {
                        unsigned long long a;
                        unsigned long long b;

                        a = object_to_unsigned_long_long(&new_expression->left->object);
                        b = object_to_unsigned_long_long(&new_expression->right->object);
                        if (op == 43)
                        {
                            unsigned long long exact_result;

                            if (unsigned_long_long_add(&exact_result, a, b))
                            {
                            }
                            else
                            {
                                compiler_diagnostic(50, ctx, 0U, &m, "integer overflow");
                            }
                            value = object_make_unsigned_long_long(a + b);
                        }
                        else
                        {
                            if (op == 45)
                            {
                                unsigned long long exact_result;

                                if (unsigned_long_long_sub(&exact_result, a, b))
                                {
                                }
                                else
                                {
                                    compiler_diagnostic(50, ctx, 0U, &m, "integer overflow");
                                }
                                value = object_make_unsigned_long_long(a - b);
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    unsigned long long exact_result;

                                    if (unsigned_long_long_mul(&exact_result, a, b))
                                    {
                                    }
                                    else
                                    {
                                        compiler_diagnostic(50, ctx, 0U, &m, "integer overflow");
                                    }
                                    value = object_make_unsigned_long_long(a * b);
                                }
                                else
                                {
                                    if (op == 47)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                        }
                                        value = object_make_unsigned_long_long(a / b);
                                    }
                                    else
                                    {
                                        if (op == 37)
                                        {
                                            if (b == 0)
                                            {
                                                compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                                /*throw*/ goto _CKL0;
                                            }
                                            value = object_make_unsigned_long_long(a % b);
                                        }
                                        else
                                        {
                                            if (op == 62)
                                            {
                                                value = object_make_signed_int(a > b);
                                            }
                                            else
                                            {
                                                if (op == 60)
                                                {
                                                    value = object_make_signed_int(a < b);
                                                }
                                                else
                                                {
                                                    if (op == 15933)
                                                    {
                                                        value = object_make_signed_int(a >= b);
                                                    }
                                                    else
                                                    {
                                                        if (op == 15421)
                                                        {
                                                            value = object_make_signed_int(a <= b);
                                                        }
                                                        else
                                                        {
                                                            if (op == 15677)
                                                            {
                                                                value = object_make_signed_int(a == b);
                                                            }
                                                            else
                                                            {
                                                                if (op == 8509)
                                                                {
                                                                    value = object_make_signed_int(a != b);
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
                    /*break*/ goto _CKL18;

                    /*case 2*/ _CKL136:
                    /*case 3*/ _CKL137:
                    /*case 4*/ _CKL138:
                    /*case 5*/ _CKL139:
                    /*case 6*/ _CKL140:
                    ;
                    /*throw*/ goto _CKL0;
                    /*break*/ goto _CKL18;

                    /*case 11*/ _CKL141:
                    {
                        float a;
                        float b;

                        a = object_to_float(&new_expression->left->object);
                        b = object_to_float(&new_expression->right->object);
                        if (op == 43)
                        {
                            value = object_make_float(a + b);
                        }
                        else
                        {
                            if (op == 45)
                            {
                                value = object_make_float(a - b);
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    value = object_make_float(a * b);
                                }
                                else
                                {
                                    if (op == 47)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                        }
                                        value = object_make_float(a / b);
                                    }
                                    else
                                    {
                                        if (op == 37)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "'%': not valid as left operand has type 'float'");
                                            /*throw*/ goto _CKL0;
                                        }
                                        else
                                        {
                                            if (op == 62)
                                            {
                                                value = object_make_signed_int(a > b);
                                            }
                                            else
                                            {
                                                if (op == 60)
                                                {
                                                    value = object_make_signed_int(a < b);
                                                }
                                                else
                                                {
                                                    if (op == 15933)
                                                    {
                                                        value = object_make_signed_int(a >= b);
                                                    }
                                                    else
                                                    {
                                                        if (op == 15421)
                                                        {
                                                            value = object_make_signed_int(a <= b);
                                                        }
                                                        else
                                                        {
                                                            if (op == 15677)
                                                            {
                                                                value = object_make_signed_int(a == b);
                                                            }
                                                            else
                                                            {
                                                                if (op == 8509)
                                                                {
                                                                    value = object_make_signed_int(a != b);
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
                    /*break*/ goto _CKL18;

                    /*case 12*/ _CKL154:
                    {
                        double a;
                        double b;

                        a = object_to_double(&new_expression->left->object);
                        b = object_to_double(&new_expression->right->object);
                        if (op == 43)
                        {
                            value = object_make_double(a + b);
                        }
                        else
                        {
                            if (op == 45)
                            {
                                value = object_make_double(a - b);
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    value = object_make_double(a * b);
                                }
                                else
                                {
                                    if (op == 47)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                        }
                                        else
                                        {
                                            value = object_make_double(a / b);
                                        }
                                    }
                                    else
                                    {
                                        if (op == 37)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "'%': not valid as left operand has type 'float'");
                                            /*throw*/ goto _CKL0;
                                        }
                                        else
                                        {
                                            if (op == 62)
                                            {
                                                value = object_make_signed_int(a > b);
                                            }
                                            else
                                            {
                                                if (op == 60)
                                                {
                                                    value = object_make_signed_int(a < b);
                                                }
                                                else
                                                {
                                                    if (op == 15933)
                                                    {
                                                        value = object_make_signed_int(a >= b);
                                                    }
                                                    else
                                                    {
                                                        if (op == 15421)
                                                        {
                                                            value = object_make_signed_int(a <= b);
                                                        }
                                                        else
                                                        {
                                                            if (op == 15677)
                                                            {
                                                                value = object_make_signed_int(a == b);
                                                            }
                                                            else
                                                            {
                                                                if (op == 8509)
                                                                {
                                                                    value = object_make_signed_int(a != b);
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
                    /*break*/ goto _CKL18;

                    /*case 13*/ _CKL167:
                    {
                        long double a;
                        long double b;

                        a = object_to_long_double(&new_expression->left->object);
                        b = object_to_long_double(&new_expression->right->object);
                        if (op == 43)
                        {
                            value = object_make_long_double(a + b);
                        }
                        else
                        {
                            if (op == 45)
                            {
                                value = object_make_long_double(a - b);
                            }
                            else
                            {
                                if (op == 42)
                                {
                                    value = object_make_long_double(a * b);
                                }
                                else
                                {
                                    if (op == 47)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "division by zero");
                                        }
                                        else
                                        {
                                            value = object_make_long_double(a / b);
                                        }
                                    }
                                    else
                                    {
                                        if (op == 37)
                                        {
                                            compiler_diagnostic(36, ctx, new_expression->right->first_token, 0U, "'%': not valid as left operand has type 'float'");
                                            /*throw*/ goto _CKL0;
                                        }
                                        else
                                        {
                                            if (op == 62)
                                            {
                                                value = object_make_signed_int(a > b);
                                            }
                                            else
                                            {
                                                if (op == 60)
                                                {
                                                    value = object_make_signed_int(a < b);
                                                }
                                                else
                                                {
                                                    if (op == 15933)
                                                    {
                                                        value = object_make_signed_int(a >= b);
                                                    }
                                                    else
                                                    {
                                                        if (op == 15421)
                                                        {
                                                            value = object_make_signed_int(a <= b);
                                                        }
                                                        else
                                                        {
                                                            if (op == 15677)
                                                            {
                                                                value = object_make_signed_int(a == b);
                                                            }
                                                            else
                                                            {
                                                                if (op == 8509)
                                                                {
                                                                    value = object_make_signed_int(a != b);
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
                    /*break*/ goto _CKL18;

                }
                _CKL18:;
            }
            ;
        }
        type_destroy(&common_type);
        *result = value;
        return 0;
    }
    /*catch*/ else _CKL0:
    {
    }
    type_destroy(&common_type);
    _cake_zmem(&empty, 96);
    *result = empty;
    return 1;
}

struct expression *multiplicative_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        p_expression_node = cast_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 42 || ctx->current->type == 47 || ctx->current->type == 37))
        {
            struct expression * new_expression;
            int   op;

            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                expression_delete(p_expression_node);
                p_expression_node = 0U;
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            /*switch*/
            {
                register int   _R7 = op;
                if (_R7 == 42) goto _CKL5; /*case 42*/
                if (_R7 == 47) goto _CKL6; /*case 47*/
                if (_R7 == 37) goto _CKL7; /*case 37*/
                goto /*default*/ _CKL8;

                {
                    /*case 42*/ _CKL5:
                    new_expression->expression_type = 35;
                    /*break*/ goto _CKL4;

                    /*case 47*/ _CKL6:
                    new_expression->expression_type = 36;
                    /*break*/ goto _CKL4;

                    /*case 37*/ _CKL7:
                    new_expression->expression_type = 37;
                    /*break*/ goto _CKL4;

                    /*default*/ _CKL8:
                    ;
                    /*break*/ goto _CKL4;

                }
                _CKL4:;
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            new_expression->right = cast_expression(ctx);
            if (new_expression->right == 0U)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (op == 37)
            {
                if (!type_is_integer(&new_expression->left->type))
                {
                    compiler_diagnostic(840, ctx, new_expression->left->first_token, 0U, "left is not an integer type");
                }
                if (!type_is_integer(&new_expression->right->type))
                {
                    compiler_diagnostic(850, ctx, new_expression->right->first_token, 0U, "right is not an integer type");
                }
            }
            else
            {
                if (!type_is_arithmetic(&new_expression->left->type))
                {
                    compiler_diagnostic(820, ctx, new_expression->left->first_token, 0U, "left is not an arithmetic type");
                }
                if (!type_is_arithmetic(&new_expression->right->type))
                {
                    compiler_diagnostic(830, ctx, new_expression->right->first_token, 0U, "right is not an arithmetic type");
                }
            }
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type);
            if (!ctx->evaluation_is_disabled && execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            p_expression_node = new_expression;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

struct expression *additive_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        p_expression_node = multiplicative_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 43 || ctx->current->type == 45))
        {
            struct token * operator_position;
            struct expression * new_expression;
            int   op;
            unsigned char   b_left_is_arithmetic;
            unsigned char   b_right_is_arithmetic;
            int   left_category;
            int   right_category;

            operator_position = ctx->current;
            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                compiler_diagnostic(1260, ctx, ctx->current, 0U, "out of mem");
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = 0U;
                /*throw*/ goto _CKL0;
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            new_expression->right = multiplicative_expression(ctx);
            if (new_expression->right == 0U)
            {
                expression_delete(new_expression);
                new_expression = 0U;
                /*throw*/ goto _CKL0;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (!type_is_scalar(&new_expression->left->type) && !type_is_array(&new_expression->left->type))
            {
                compiler_diagnostic(870, ctx, operator_position, 0U, "left operator is not scalar");
            }
            if (!type_is_scalar(&new_expression->right->type) && !type_is_array(&new_expression->right->type))
            {
                compiler_diagnostic(880, ctx, operator_position, 0U, "right operator is not scalar");
            }
            b_left_is_arithmetic = type_is_arithmetic(&new_expression->left->type);
            b_right_is_arithmetic = type_is_arithmetic(&new_expression->right->type);
            left_category = type_get_category(&new_expression->left->type);
            right_category = type_get_category(&new_expression->right->type);
            if (op == 43)
            {
                new_expression->expression_type = 38;
                if (b_left_is_arithmetic && b_right_is_arithmetic)
                {
                    new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type);
                    if (!ctx->evaluation_is_disabled && execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
                    {
                        expression_delete(new_expression);
                        /*throw*/ goto _CKL0;
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
                            compiler_diagnostic(850, ctx, ctx->current, 0U, "expected integer type on right");
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
                                compiler_diagnostic(840, ctx, ctx->current, 0U, "expected integer type on left");
                            }
                        }
                        else
                        {
                            compiler_diagnostic(860, ctx, ctx->current, 0U, "invalid types additive expression");
                        }
                    }
                }
            }
            else
            {
                if (op == 45)
                {
                    new_expression->expression_type = 39;
                    if (b_left_is_arithmetic && b_right_is_arithmetic)
                    {
                        new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type);
                        if (!ctx->evaluation_is_disabled && execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
                        {
                            expression_delete(new_expression);
                            /*throw*/ goto _CKL0;
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
                                    compiler_diagnostic(890, ctx, ctx->current, 0U, "incompatible pointer types");
                                }
                                new_expression->type = type_make_int();
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
                                    compiler_diagnostic(850, ctx, ctx->current, 0U, "right must be integer type");
                                }
                            }
                        }
                        else
                        {
                            compiler_diagnostic(860, ctx, ctx->current, 0U, "invalid types for operator -");
                        }
                    }
                }
            }
            p_expression_node = new_expression;
            new_expression = 0U;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

static int execute_bitwise_operator(struct parser_ctx * ctx, struct expression * new_expression, int op);

struct expression *shift_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        p_expression_node = additive_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 15934 || ctx->current->type == 15420))
        {
            struct expression * new_expression;
            int   op;

            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = 0U;
                /*throw*/ goto _CKL0;
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            new_expression->right = multiplicative_expression(ctx);
            if (new_expression->right == 0U)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (op == 15934)
            {
                new_expression->expression_type = 40;
            }
            else
            {
                if (op == 15420)
                {
                    new_expression->expression_type = 41;
                }
            }
            if (execute_bitwise_operator(ctx, new_expression, op) != 0)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            p_expression_node = new_expression;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

void check_comparison(struct parser_ctx * ctx, struct expression * p_a_expression, struct expression * p_b_expression, struct token * op_token);

struct expression *relational_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;
    struct expression * new_expression;

    p_expression_node = 0U;
    new_expression = 0U;
    /*try*/ if (1)
    {
        p_expression_node = shift_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 62 || ctx->current->type == 60 || ctx->current->type == 15933 || ctx->current->type == 15421))
        {
            int   op;

            ;
            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            new_expression->right = shift_expression(ctx);
            if (new_expression->right == 0U)
            {
                expression_delete(new_expression);
                new_expression = 0U;
                /*throw*/ goto _CKL0;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = 0U;
                /*throw*/ goto _CKL0;
            }
            check_comparison(ctx, new_expression->left, new_expression->right, ctx->current);
            if (op == 62)
            {
                new_expression->expression_type = 42;
            }
            else
            {
                if (op == 60)
                {
                    new_expression->expression_type = 43;
                }
                else
                {
                    if (op == 15933)
                    {
                        new_expression->expression_type = 44;
                    }
                    else
                    {
                        if (op == 15421)
                        {
                            new_expression->expression_type = 45;
                        }
                    }
                }
            }
            if (type_is_arithmetic(&new_expression->left->type) && type_is_arithmetic(&new_expression->right->type))
            {
                new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type);
                if (!ctx->evaluation_is_disabled && execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
                {
                    expression_delete(new_expression);
                    new_expression = 0U;
                    /*throw*/ goto _CKL0;
                }
            }
            type_destroy(&new_expression->type);
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
            new_expression = 0U;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(new_expression);
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

struct enum_specifier *get_complete_enum_specifier(struct enum_specifier * p_enum_specifier);
int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);

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
            snprintf(finalmessage, 200U, "%s (enum %s, enum %s)", message, lefttag, righttag);
            compiler_diagnostic(3, ctx, operator_token, 0U, finalmessage, lefttag, righttag);
        }
    }
}

void expression_evaluate_equal_not_equal(struct expression * left, struct expression * right, struct expression * result, int op, unsigned char   disabled)
{
    ;
}

struct expression *equality_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;
    struct expression * new_expression;

    p_expression_node = 0U;
    new_expression = 0U;
    /*try*/ if (1)
    {
        p_expression_node = relational_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 15677 || ctx->current->type == 8509))
        {
            struct token * operator_token;

            ;
            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            if (operator_token->type == 15677)
            {
                new_expression->expression_type = 46;
            }
            else
            {
                new_expression->expression_type = 47;
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            new_expression->right = relational_expression(ctx);
            if (new_expression->right == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            check_comparison(ctx, new_expression->left, new_expression->right, ctx->current);
            new_expression->last_token = new_expression->right->last_token;
            new_expression->first_token = operator_token;
            if (type_is_arithmetic(&new_expression->left->type) && type_is_arithmetic(&new_expression->right->type))
            {
                if (!ctx->evaluation_is_disabled && execute_arithmetic(ctx, new_expression, operator_token->type, &new_expression->object) != 0)
                {
                    /*throw*/ goto _CKL0;
                }
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
            new_expression = 0U;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    expression_delete(new_expression);
    return p_expression_node;
}

struct expression *and_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;
    struct expression * new_expression;

    p_expression_node = 0U;
    new_expression = 0U;
    /*try*/ if (1)
    {
        p_expression_node = equality_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && ctx->current->type == 38)
        {
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            ;
            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 48;
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            new_expression->right = equality_expression(ctx);
            if (new_expression->right == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (execute_bitwise_operator(ctx, new_expression, 38) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            p_expression_node = new_expression;
            new_expression = 0U;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    expression_delete(new_expression);
    return p_expression_node;
}

struct expression *exclusive_or_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;
    struct expression * new_expression;

    p_expression_node = 0U;
    new_expression = 0U;
    /*try*/ if (1)
    {
        p_expression_node = and_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 94))
        {
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            ;
            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 49;
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            new_expression->right = and_expression(ctx);
            if (new_expression->right == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (execute_bitwise_operator(ctx, new_expression, 94) != 0)
            {
                /*throw*/ goto _CKL0;
            }
            p_expression_node = new_expression;
            new_expression = 0U;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    expression_delete(new_expression);
    return p_expression_node;
}

void object_destroy(struct object * p);

static int execute_bitwise_operator(struct parser_ctx * ctx, struct expression * new_expression, int op)
{
    /*try*/ if (1)
    {
        /*switch*/
        {
            register int _R8 = op;
            if (_R8 == 38) goto _CKL2; /*case 38*/
            if (_R8 == 94) goto _CKL3; /*case 94*/
            if (_R8 == 124) goto _CKL4; /*case 124*/
            if (_R8 == 15934) goto _CKL5; /*case 15934*/
            if (_R8 == 15420) goto _CKL6; /*case 15420*/
            goto /*default*/ _CKL7;

            {
                /*case 38*/ _CKL2:
                /*case 94*/ _CKL3:
                /*case 124*/ _CKL4:
                /*case 15934*/ _CKL5:
                /*case 15420*/ _CKL6:
                /*break*/ goto _CKL1;

                /*default*/ _CKL7:
                ;
                /*throw*/ goto _CKL0;
            }
            _CKL1:;
        }
        if (!type_is_integer(&new_expression->left->type))
        {
            compiler_diagnostic(840, ctx, ctx->current, 0U, "left type must be an integer type");
            /*throw*/ goto _CKL0;
        }
        if (!type_is_integer(&new_expression->right->type))
        {
            compiler_diagnostic(840, ctx, ctx->current, 0U, "right type must be an integer type");
            /*throw*/ goto _CKL0;
        }
        type_destroy(&new_expression->type);
        new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type);
        if (!ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
        {
            int   vt;

            vt = type_to_object_type(&new_expression->type);
            /*switch*/
            {
                register int   _R9 = vt;
                if (_R9 == 0) goto _CKL12; /*case 0*/
                if (_R9 == 1) goto _CKL18; /*case 1*/
                if (_R9 == 7) goto _CKL24; /*case 7*/
                if (_R9 == 8) goto _CKL30; /*case 8*/
                if (_R9 == 9) goto _CKL36; /*case 9*/
                if (_R9 == 10) goto _CKL42; /*case 10*/
                if (_R9 == 2) goto _CKL48; /*case 2*/
                if (_R9 == 3) goto _CKL49; /*case 3*/
                if (_R9 == 4) goto _CKL50; /*case 4*/
                if (_R9 == 5) goto _CKL51; /*case 5*/
                if (_R9 == 6) goto _CKL52; /*case 6*/
                if (_R9 == 11) goto _CKL53; /*case 11*/
                if (_R9 == 12) goto _CKL54; /*case 12*/
                if (_R9 == 13) goto _CKL55; /*case 13*/
                goto _CKL11;

                {
                    /*case 0*/ _CKL12:
                    {
                        int a;
                        int b;
                        int r;

                        a = object_to_signed_int(&new_expression->left->object);
                        b = object_to_signed_int(&new_expression->right->object);
                        r = 0;
                        if (op == 124)
                        {
                            r = a | b;
                        }
                        else
                        {
                            if (op == 94)
                            {
                                r = a ^ b;
                            }
                            else
                            {
                                if (op == 38)
                                {
                                    r = a & b;
                                }
                                else
                                {
                                    if (op == 15934)
                                    {
                                        r = a >> b;
                                    }
                                    else
                                    {
                                        if (op == 15420)
                                        {
                                            r = a << b;
                                        }
                                    }
                                }
                            }
                        }
                        object_destroy(&new_expression->object);
                        new_expression->object = object_make_signed_int(r);
                    }
                    /*break*/ goto _CKL11;

                    /*case 1*/ _CKL18:
                    {
                        unsigned int a;
                        unsigned int b;
                        int r;

                        a = object_to_unsigned_int(&new_expression->left->object);
                        b = object_to_unsigned_int(&new_expression->right->object);
                        r = 0;
                        if (op == 124)
                        {
                            r = a | b;
                        }
                        else
                        {
                            if (op == 94)
                            {
                                r = a ^ b;
                            }
                            else
                            {
                                if (op == 38)
                                {
                                    r = a & b;
                                }
                                else
                                {
                                    if (op == 15934)
                                    {
                                        r = a >> b;
                                    }
                                    else
                                    {
                                        if (op == 15420)
                                        {
                                            r = a << b;
                                        }
                                    }
                                }
                            }
                        }
                        object_destroy(&new_expression->object);
                        new_expression->object = object_make_unsigned_int(r);
                    }
                    /*break*/ goto _CKL11;

                    /*case 7*/ _CKL24:
                    {
                        signed long a;
                        signed long b;
                        int r;

                        a = object_to_signed_long(&new_expression->left->object);
                        b = object_to_signed_long(&new_expression->right->object);
                        r = 0;
                        if (op == 124)
                        {
                            r = a | b;
                        }
                        else
                        {
                            if (op == 94)
                            {
                                r = a ^ b;
                            }
                            else
                            {
                                if (op == 38)
                                {
                                    r = a & b;
                                }
                                else
                                {
                                    if (op == 15934)
                                    {
                                        r = a >> b;
                                    }
                                    else
                                    {
                                        if (op == 15420)
                                        {
                                            r = a << b;
                                        }
                                    }
                                }
                            }
                        }
                        object_destroy(&new_expression->object);
                        new_expression->object = object_make_signed_long(r);
                    }
                    /*break*/ goto _CKL11;

                    /*case 8*/ _CKL30:
                    {
                        unsigned long a;
                        unsigned long b;
                        int r;

                        a = object_to_unsigned_long(&new_expression->left->object);
                        b = object_to_unsigned_long(&new_expression->right->object);
                        r = 0;
                        if (op == 124)
                        {
                            r = a | b;
                        }
                        else
                        {
                            if (op == 94)
                            {
                                r = a ^ b;
                            }
                            else
                            {
                                if (op == 38)
                                {
                                    r = a & b;
                                }
                                else
                                {
                                    if (op == 15934)
                                    {
                                        r = a >> b;
                                    }
                                    else
                                    {
                                        if (op == 15420)
                                        {
                                            r = a << b;
                                        }
                                    }
                                }
                            }
                        }
                        object_destroy(&new_expression->object);
                        new_expression->object = object_make_unsigned_long(r);
                    }
                    /*break*/ goto _CKL11;

                    /*case 9*/ _CKL36:
                    {
                        signed long long a;
                        signed long long b;
                        signed long long r;

                        a = object_to_signed_long_long(&new_expression->left->object);
                        b = object_to_signed_long_long(&new_expression->right->object);
                        r = 0;
                        if (op == 124)
                        {
                            r = a | b;
                        }
                        else
                        {
                            if (op == 94)
                            {
                                r = a ^ b;
                            }
                            else
                            {
                                if (op == 38)
                                {
                                    r = a & b;
                                }
                                else
                                {
                                    if (op == 15934)
                                    {
                                        r = a >> b;
                                    }
                                    else
                                    {
                                        if (op == 15420)
                                        {
                                            r = a << b;
                                        }
                                    }
                                }
                            }
                        }
                        object_destroy(&new_expression->object);
                        new_expression->object = object_make_signed_long_long(r);
                    }
                    /*break*/ goto _CKL11;

                    /*case 10*/ _CKL42:
                    {
                        unsigned long long a;
                        unsigned long long b;
                        unsigned long long r;

                        a = object_to_unsigned_long_long(&new_expression->left->object);
                        b = object_to_unsigned_long_long(&new_expression->right->object);
                        r = 0;
                        if (op == 124)
                        {
                            r = a | b;
                        }
                        else
                        {
                            if (op == 94)
                            {
                                r = a ^ b;
                            }
                            else
                            {
                                if (op == 38)
                                {
                                    r = a & b;
                                }
                                else
                                {
                                    if (op == 15934)
                                    {
                                        r = a >> b;
                                    }
                                    else
                                    {
                                        if (op == 15420)
                                        {
                                            r = a << b;
                                        }
                                    }
                                }
                            }
                        }
                        object_destroy(&new_expression->object);
                        new_expression->object = object_make_unsigned_long_long(r);
                    }
                    /*break*/ goto _CKL11;

                    /*case 2*/ _CKL48:
                    /*case 3*/ _CKL49:
                    /*case 4*/ _CKL50:
                    /*case 5*/ _CKL51:
                    /*case 6*/ _CKL52:
                    ;
                    /*throw*/ goto _CKL0;
                    /*break*/ goto _CKL11;

                    /*case 11*/ _CKL53:
                    /*case 12*/ _CKL54:
                    /*case 13*/ _CKL55:
                    ;
                    /*throw*/ goto _CKL0;
                    /*break*/ goto _CKL11;

                }
                _CKL11:;
            }
            ;
        }
        return 0;
    }
    /*catch*/ else _CKL0:
    {
    }
    return 1;
}

struct expression *inclusive_or_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        p_expression_node = exclusive_or_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 124))
        {
            struct token * operator_token;
            struct expression * new_expression;

            operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 50;
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            new_expression->right = exclusive_or_expression(ctx);
            if (new_expression->right == 0U)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            check_diferent_enuns(ctx, operator_token, new_expression->left, new_expression->right, "operator '|' between enumerations of different types.");
            new_expression->last_token = new_expression->right->last_token;
            if (execute_bitwise_operator(ctx, new_expression, 124) != 0)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            p_expression_node = new_expression;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

struct expression *logical_and_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        p_expression_node = inclusive_or_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 9766))
        {
            struct expression * new_expression;
            unsigned char   right_evaluation_is_disabled;
            unsigned char   old_evaluation_is_disabled;

            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 52;
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            right_evaluation_is_disabled = 0;
            if (object_has_constant_value(&new_expression->left->object))
            {
                if (!object_to_bool(&new_expression->left->object))
                {
                    right_evaluation_is_disabled = 1;
                }
            }
            old_evaluation_is_disabled = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = right_evaluation_is_disabled;
            new_expression->right = inclusive_or_expression(ctx);
            ctx->evaluation_is_disabled = old_evaluation_is_disabled;
            if (new_expression->right == 0U)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (!ctx->evaluation_is_disabled)
            {
                if (object_has_constant_value(&new_expression->left->object))
                {
                    unsigned char   a;

                    a = object_to_bool(&new_expression->left->object);
                    if (a == 0)
                    {
                        new_expression->object = object_make_signed_int(0);
                    }
                    else
                    {
                        if (object_has_constant_value(&new_expression->right->object))
                        {
                            unsigned char   b;

                            b = object_to_bool(&new_expression->right->object);
                            new_expression->object = object_make_signed_int(a && b);
                        }
                    }
                }
            }
            if (!type_is_scalar(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(870, ctx, ctx->current, 0U, "left type is not scalar for or expression");
                /*throw*/ goto _CKL0;
            }
            if (!type_is_scalar(&new_expression->right->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(880, ctx, ctx->current, 0U, "right type is not scalar for or expression");
                /*throw*/ goto _CKL0;
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

struct expression *logical_or_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        p_expression_node = logical_and_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        while (ctx->current != 0U && (ctx->current->type == 31868))
        {
            struct expression * new_expression;
            unsigned char   right_evaluation_is_disabled;
            unsigned char   old_evaluation_is_disabled;

            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 51;
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            right_evaluation_is_disabled = 0;
            if (object_has_constant_value(&new_expression->left->object))
            {
                if (object_to_bool(&new_expression->left->object))
                {
                    right_evaluation_is_disabled = 1;
                }
            }
            old_evaluation_is_disabled = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = right_evaluation_is_disabled;
            new_expression->right = logical_and_expression(ctx);
            ctx->evaluation_is_disabled = old_evaluation_is_disabled;
            if (new_expression->right == 0U)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (!ctx->evaluation_is_disabled)
            {
                if (object_has_constant_value(&new_expression->left->object))
                {
                    unsigned char   a;

                    a = object_to_bool(&new_expression->left->object);
                    if (a == 1)
                    {
                        new_expression->object = object_make_signed_int(1);
                    }
                    else
                    {
                        if (object_has_constant_value(&new_expression->right->object))
                        {
                            unsigned char   b;

                            b = object_to_bool(&new_expression->right->object);
                            new_expression->object = object_make_signed_int(a || b);
                        }
                    }
                }
            }
            if (!type_is_scalar(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(870, ctx, ctx->current, 0U, "left type is not scalar for or expression");
                /*throw*/ goto _CKL0;
            }
            if (!type_is_scalar(&new_expression->right->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic(880, ctx, ctx->current, 0U, "right type is not scalar for or expression");
                /*throw*/ goto _CKL0;
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

struct expression *conditional_expression(struct parser_ctx * ctx);

struct expression *assignment_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        p_expression_node = conditional_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        ;
        while (ctx->current != 0U && (ctx->current->type == 61 || ctx->current->type == 10813 || ctx->current->type == 12093 || ctx->current->type == 9533 || ctx->current->type == 11069 || ctx->current->type == 11581 || ctx->current->type == 15421 || ctx->current->type == 15933 || ctx->current->type == 9789 || ctx->current->type == 24125 || ctx->current->type == 31805))
        {
            struct token * op_token;
            struct expression * new_expression;
            struct marker  left_operand_marker;

            op_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                /*throw*/ goto _CKL0;
            }
            new_expression = calloc(1, 224U);
            if (new_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            new_expression->first_token = ctx->current;
            /*switch*/
            {
                register int   _R10 = op_token->type;
                if (_R10 == 61) goto _CKL5; /*case 61*/
                if (_R10 == 10813) goto _CKL6; /*case 10813*/
                if (_R10 == 12093) goto _CKL7; /*case 12093*/
                if (_R10 == 9533) goto _CKL8; /*case 9533*/
                if (_R10 == 11069) goto _CKL9; /*case 11069*/
                if (_R10 == 11581) goto _CKL10; /*case 11581*/
                if (_R10 == 15421) goto _CKL11; /*case 15421*/
                if (_R10 == 15933) goto _CKL12; /*case 15933*/
                if (_R10 == 9789) goto _CKL13; /*case 9789*/
                if (_R10 == 24125) goto _CKL14; /*case 24125*/
                if (_R10 == 31805) goto _CKL15; /*case 31805*/
                goto /*default*/ _CKL16;

                {
                    /*case 61*/ _CKL5:
                    new_expression->expression_type = 53;
                    /*break*/ goto _CKL4;

                    /*case 10813*/ _CKL6:
                    new_expression->expression_type = 56;
                    /*break*/ goto _CKL4;

                    /*case 12093*/ _CKL7:
                    new_expression->expression_type = 57;
                    /*break*/ goto _CKL4;

                    /*case 9533*/ _CKL8:
                    new_expression->expression_type = 58;
                    /*break*/ goto _CKL4;

                    /*case 11069*/ _CKL9:
                    new_expression->expression_type = 54;
                    /*break*/ goto _CKL4;

                    /*case 11581*/ _CKL10:
                    new_expression->expression_type = 55;
                    /*break*/ goto _CKL4;

                    /*case 15421*/ _CKL11:
                    new_expression->expression_type = 59;
                    /*break*/ goto _CKL4;

                    /*case 15933*/ _CKL12:
                    new_expression->expression_type = 60;
                    /*break*/ goto _CKL4;

                    /*case 9789*/ _CKL13:
                    new_expression->expression_type = 61;
                    /*break*/ goto _CKL4;

                    /*case 24125*/ _CKL14:
                    new_expression->expression_type = 63;
                    /*break*/ goto _CKL4;

                    /*case 31805*/ _CKL15:
                    new_expression->expression_type = 62;
                    /*break*/ goto _CKL4;

                    /*default*/ _CKL16:
                    ;
                    /*break*/ goto _CKL4;

                }
                _CKL4:;
            }
            new_expression->left = p_expression_node;
            p_expression_node = 0U;
            left_operand_marker.file = 0;
            left_operand_marker.line = 0;
            left_operand_marker.start_col = 0;
            left_operand_marker.end_col = 0;
            left_operand_marker.p_token_caret = 0;
            left_operand_marker.p_token_begin = new_expression->left->first_token;
            left_operand_marker.p_token_end = new_expression->left->last_token;
            if (type_is_function(&new_expression->left->type))
            {
                compiler_diagnostic(900, ctx, 0U, &left_operand_marker, "assignment of function");
            }
            else
            {
                if (type_is_array(&new_expression->left->type))
                {
                    if (new_expression->left->type.storage_class_specifier_flags & 2048)
                    {
                        compiler_diagnostic(42, ctx, 0U, &left_operand_marker, "assignment to array parameter");
                    }
                    else
                    {
                        compiler_diagnostic(910, ctx, 0U, &left_operand_marker, "assignment to expression with array type");
                    }
                }
            }
            if (type_is_const(&new_expression->left->type))
            {
                compiler_diagnostic(920, ctx, 0U, &left_operand_marker, "assignment of read-only object");
            }
            if (!expression_is_lvalue(new_expression->left))
            {
                compiler_diagnostic(1230, ctx, 0U, &left_operand_marker, "lvalue required as left operand of assignment");
            }
            new_expression->right = assignment_expression(ctx);
            if (new_expression->right == 0U)
            {
                expression_delete(new_expression);
                /*throw*/ goto _CKL0;
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
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

void argument_expression_list_push(struct argument_expression_list * list, struct argument_expression * pitem)
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

void argument_expression_list_destroy(struct argument_expression_list * p)
{
    struct argument_expression * item;

    item = p->head;
    while (item)
    {
        struct argument_expression * next;

        next = item->next;
        item->next = 0U;
        argument_expression_delete(item);
        item = next;
    }
}

unsigned char  expression_is_malloc(struct expression * p)
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

unsigned char  expression_is_calloc(struct expression * p)
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

struct expression *expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;

    p_expression_node = 0U;
    /*try*/ if (1)
    {
        if (ctx->current == 0U)
        {
            unexpected_end_of_file(ctx);
            /*throw*/ goto _CKL0;
        }
        p_expression_node = assignment_expression(ctx);
        if (p_expression_node == 0U)
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
            while (ctx->current->type == 44)
            {
                struct expression * p_expression_node_new;

                parser_match(ctx);
                if (ctx->current == 0U)
                {
                    unexpected_end_of_file(ctx);
                    /*throw*/ goto _CKL0;
                }
                p_expression_node_new = calloc(1, 224U);
                if (p_expression_node_new == 0U)
                {
                    /*throw*/ goto _CKL0;
                }
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = 64;
                p_expression_node_new->left = p_expression_node;
                p_expression_node = 0U;
                p_expression_node_new->right = expression(ctx);
                if (p_expression_node_new->right == 0U)
                {
                    expression_delete(p_expression_node_new);
                    /*throw*/ goto _CKL0;
                }
                p_expression_node_new->left->last_token = p_expression_node_new->right->last_token;
                p_expression_node = p_expression_node_new;
                if (ctx->current == 0U)
                {
                    unexpected_end_of_file(ctx);
                    /*throw*/ goto _CKL0;
                }
            }
            type_destroy(&p_expression_node->type);
            p_expression_node->type = type_dup(&p_expression_node->right->type);
        }
    }
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    return p_expression_node;
}

unsigned char  is_first_of_conditional_expression(struct parser_ctx * ctx)
{
    return !!(is_first_of_unary_expression(ctx) || is_first_of_primary_expression(ctx));
}

unsigned char  expression_is_one(struct expression * expression)
{
    if (expression->expression_type == 8)
    {
        return !!((object_has_constant_value(&expression->object) && object_to_signed_int(&expression->object) == 1));
    }
    return 0;
}

unsigned char  expression_is_zero(struct expression * expression)
{
    if (expression->expression_type == 8)
    {
        return !!((object_has_constant_value(&expression->object) && object_to_signed_int(&expression->object) == 0));
    }
    return 0;
}

unsigned char  type_is_void_ptr(struct type * p_type);

unsigned char  expression_is_null_pointer_constant(struct expression * expression)
{
    if (type_is_integer(&expression->type) && object_has_constant_value(&expression->object) && object_to_signed_int(&expression->object) == 0)
    {
        return 1;
    }
    if (type_is_void_ptr(&expression->type) && object_has_constant_value(&expression->object) && object_to_signed_int(&expression->object) == 0)
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

struct expression *conditional_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node;
    struct type  left_type;
    struct type  right_type;

    p_expression_node = 0U;
    _cake_zmem(&left_type, 60);
    _cake_zmem(&right_type, 60);
    /*try*/ if (1)
    {
        p_expression_node = logical_or_expression(ctx);
        if (p_expression_node == 0U)
        {
            /*throw*/ goto _CKL0;
        }
        if (ctx->current && ctx->current->type == 63)
        {
            struct expression * p_conditional_expression;
            unsigned char   constant_expression_is_true;
            unsigned char   has_constant_expression;
            unsigned char   old_evaluation_is_disabled;
            struct expression * p_left;
            struct expression * p_right;

            p_conditional_expression = calloc(1, 224U);
            if (p_conditional_expression == 0U)
            {
                /*throw*/ goto _CKL0;
            }
            p_conditional_expression->first_token = ctx->current;
            p_conditional_expression->expression_type = 65;
            p_conditional_expression->condition_expr = p_expression_node;
            p_expression_node = 0U;
            parser_match(ctx);
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                /*throw*/ goto _CKL0;
            }
            constant_expression_is_true = 0;
            has_constant_expression = 0;
            if (object_has_constant_value(&p_conditional_expression->condition_expr->object))
            {
                has_constant_expression = 1;
                if (object_to_bool(&p_conditional_expression->condition_expr->object))
                {
                    constant_expression_is_true = 1;
                }
            }
            old_evaluation_is_disabled = ctx->evaluation_is_disabled;
            ctx->evaluation_is_disabled = !!(has_constant_expression && !constant_expression_is_true);
            p_left = expression(ctx);
            ctx->evaluation_is_disabled = old_evaluation_is_disabled;
            if (p_left == 0U)
            {
                expression_delete(p_conditional_expression);
                /*throw*/ goto _CKL0;
            }
            p_conditional_expression->left = p_left;
            if (parser_match_tk(ctx, 58) != 0)
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                /*throw*/ goto _CKL0;
            }
            if (ctx->current == 0U)
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                /*throw*/ goto _CKL0;
            }
            ctx->evaluation_is_disabled = !!(has_constant_expression && constant_expression_is_true);
            p_right = conditional_expression(ctx);
            ctx->evaluation_is_disabled = old_evaluation_is_disabled;
            if (p_right == 0U)
            {
                expression_delete(p_conditional_expression);
                /*throw*/ goto _CKL0;
            }
            p_conditional_expression->right = p_right;
            if (object_has_constant_value(&p_conditional_expression->condition_expr->object))
            {
                if (object_to_bool(&p_conditional_expression->condition_expr->object))
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
                compiler_diagnostic(940, ctx, ctx->current, 0U, "condition must have scalar type");
            }
            else
            {
                if (type_is_arithmetic(&left_type) && type_is_arithmetic(&right_type))
                {
                    type_destroy(&p_conditional_expression->type);
                    p_conditional_expression->type = type_common(&left_type, &right_type);
                }
                else
                {
                    if (type_is_struct_or_union(&left_type) && type_is_struct_or_union(&right_type))
                    {
                        if (!type_is_same(&left_type, &right_type, 1))
                        {
                            compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0U, "incompatible types");
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
                                                    compiler_diagnostic(950, ctx, ctx->current, 0U, "incompatible types");
                                                }
                                                else
                                                {
                                                    type_swap(&p_conditional_expression->type, &right_type);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0U, "incompatible types");
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
                                                        compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0U, "incompatible types");
                                                    }
                                                    else
                                                    {
                                                        type_swap(&p_conditional_expression->type, &right_type);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0U, "incompatible types");
                                            }
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic(950, ctx, p_conditional_expression->condition_expr->first_token, 0U, "incompatible types??");
                                        ;
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
    /*catch*/ else _CKL0:
    {
        expression_delete(p_expression_node);
        p_expression_node = 0U;
    }
    type_destroy(&left_type);
    type_destroy(&right_type);
    return p_expression_node;
}

struct expression *constant_expression(struct parser_ctx * ctx, unsigned char   show_error_if_not_constant)
{
    struct expression * p_expression;

    p_expression = conditional_expression(ctx);
    if (show_error_if_not_constant && p_expression && !object_has_constant_value(&p_expression->object))
    {
        compiler_diagnostic(960, ctx, ctx->current, 0U, "expected constant expression");
    }
    return p_expression;
}

struct object *object_get_non_const_referenced(struct object * p_object);

unsigned char  expression_get_variables(struct expression * expr, int n, struct object *variables[])
{
    int count;

    count = 0;
    /*switch*/
    {
        register int   _R11 = expr->expression_type;
        if (_R11 == 0) goto _CKL1; /*case 0*/
        if (_R11 == 1) goto _CKL2; /*case 1*/
        if (_R11 == 2) goto _CKL3; /*case 2*/
        if (_R11 == 3) goto _CKL6; /*case 3*/
        if (_R11 == 4) goto _CKL7; /*case 4*/
        if (_R11 == 5) goto _CKL8; /*case 5*/
        if (_R11 == 6) goto _CKL9; /*case 6*/
        if (_R11 == 7) goto _CKL10; /*case 7*/
        if (_R11 == 8) goto _CKL11; /*case 8*/
        if (_R11 == 9) goto _CKL12; /*case 9*/
        if (_R11 == 10) goto _CKL13; /*case 10*/
        if (_R11 == 11) goto _CKL14; /*case 11*/
        if (_R11 == 12) goto _CKL15; /*case 12*/
        if (_R11 == 13) goto _CKL16; /*case 13*/
        if (_R11 == 14) goto _CKL17; /*case 14*/
        if (_R11 == 15) goto _CKL18; /*case 15*/
        if (_R11 == 16) goto _CKL19; /*case 16*/
        if (_R11 == 17) goto _CKL20; /*case 17*/
        if (_R11 == 18) goto _CKL21; /*case 18*/
        if (_R11 == 19) goto _CKL22; /*case 19*/
        if (_R11 == 20) goto _CKL23; /*case 20*/
        if (_R11 == 21) goto _CKL24; /*case 21*/
        if (_R11 == 22) goto _CKL25; /*case 22*/
        if (_R11 == 23) goto _CKL26; /*case 23*/
        if (_R11 == 24) goto _CKL27; /*case 24*/
        if (_R11 == 25) goto _CKL28; /*case 25*/
        if (_R11 == 26) goto _CKL29; /*case 26*/
        if (_R11 == 27) goto _CKL30; /*case 27*/
        if (_R11 == 28) goto _CKL31; /*case 28*/
        if (_R11 == 29) goto _CKL32; /*case 29*/
        if (_R11 == 30) goto _CKL33; /*case 30*/
        if (_R11 == 31) goto _CKL34; /*case 31*/
        if (_R11 == 32) goto _CKL35; /*case 32*/
        if (_R11 == 33) goto _CKL36; /*case 33*/
        if (_R11 == 34) goto _CKL37; /*case 34*/
        if (_R11 == 35) goto _CKL38; /*case 35*/
        if (_R11 == 36) goto _CKL39; /*case 36*/
        if (_R11 == 37) goto _CKL40; /*case 37*/
        if (_R11 == 38) goto _CKL41; /*case 38*/
        if (_R11 == 39) goto _CKL42; /*case 39*/
        if (_R11 == 40) goto _CKL43; /*case 40*/
        if (_R11 == 41) goto _CKL44; /*case 41*/
        if (_R11 == 42) goto _CKL45; /*case 42*/
        if (_R11 == 43) goto _CKL46; /*case 43*/
        if (_R11 == 44) goto _CKL47; /*case 44*/
        if (_R11 == 45) goto _CKL48; /*case 45*/
        if (_R11 == 46) goto _CKL49; /*case 46*/
        if (_R11 == 47) goto _CKL50; /*case 47*/
        if (_R11 == 48) goto _CKL51; /*case 48*/
        if (_R11 == 49) goto _CKL52; /*case 49*/
        if (_R11 == 50) goto _CKL53; /*case 50*/
        if (_R11 == 51) goto _CKL54; /*case 51*/
        if (_R11 == 52) goto _CKL55; /*case 52*/
        if (_R11 == 53) goto _CKL56; /*case 53*/
        if (_R11 == 54) goto _CKL57; /*case 54*/
        if (_R11 == 55) goto _CKL58; /*case 55*/
        if (_R11 == 56) goto _CKL59; /*case 56*/
        if (_R11 == 57) goto _CKL60; /*case 57*/
        if (_R11 == 58) goto _CKL61; /*case 58*/
        if (_R11 == 59) goto _CKL62; /*case 59*/
        if (_R11 == 60) goto _CKL63; /*case 60*/
        if (_R11 == 61) goto _CKL64; /*case 61*/
        if (_R11 == 62) goto _CKL65; /*case 62*/
        if (_R11 == 63) goto _CKL66; /*case 63*/
        if (_R11 == 64) goto _CKL67; /*case 64*/
        if (_R11 == 65) goto _CKL68; /*case 65*/
        goto _CKL0;

        {
            /*case 0*/ _CKL1:
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL2:
            /*break*/ goto _CKL0;

            /*case 2*/ _CKL3:
            if (!object_has_constant_value(&expr->object))
            {
                if (count < n)
                {
                    variables[count] = object_get_non_const_referenced(&expr->object);
                    count++;
                }
            }
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL6:
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL7:
            /*break*/ goto _CKL0;

            /*case 5*/ _CKL8:
            /*break*/ goto _CKL0;

            /*case 6*/ _CKL9:
            /*break*/ goto _CKL0;

            /*case 7*/ _CKL10:
            /*break*/ goto _CKL0;

            /*case 8*/ _CKL11:
            /*break*/ goto _CKL0;

            /*case 9*/ _CKL12:
            count += expression_get_variables(expr->right, n, variables);
            /*break*/ goto _CKL0;

            /*case 10*/ _CKL13:
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL14:
            /*break*/ goto _CKL0;

            /*case 12*/ _CKL15:
            /*break*/ goto _CKL0;

            /*case 13*/ _CKL16:
            /*break*/ goto _CKL0;

            /*case 14*/ _CKL17:
            /*break*/ goto _CKL0;

            /*case 15*/ _CKL18:
            /*break*/ goto _CKL0;

            /*case 16*/ _CKL19:
            /*break*/ goto _CKL0;

            /*case 17*/ _CKL20:
            /*break*/ goto _CKL0;

            /*case 18*/ _CKL21:
            /*break*/ goto _CKL0;

            /*case 19*/ _CKL22:
            /*break*/ goto _CKL0;

            /*case 20*/ _CKL23:
            /*break*/ goto _CKL0;

            /*case 21*/ _CKL24:
            /*break*/ goto _CKL0;

            /*case 22*/ _CKL25:
            /*break*/ goto _CKL0;

            /*case 23*/ _CKL26:
            /*break*/ goto _CKL0;

            /*case 24*/ _CKL27:
            /*break*/ goto _CKL0;

            /*case 25*/ _CKL28:
            /*break*/ goto _CKL0;

            /*case 26*/ _CKL29:
            /*break*/ goto _CKL0;

            /*case 27*/ _CKL30:
            /*break*/ goto _CKL0;

            /*case 28*/ _CKL31:
            /*break*/ goto _CKL0;

            /*case 29*/ _CKL32:
            /*break*/ goto _CKL0;

            /*case 30*/ _CKL33:
            /*break*/ goto _CKL0;

            /*case 31*/ _CKL34:
            /*break*/ goto _CKL0;

            /*case 32*/ _CKL35:
            /*break*/ goto _CKL0;

            /*case 33*/ _CKL36:
            /*break*/ goto _CKL0;

            /*case 34*/ _CKL37:
            /*break*/ goto _CKL0;

            /*case 35*/ _CKL38:
            /*case 36*/ _CKL39:
            /*case 37*/ _CKL40:
            count += expression_get_variables(expr->left, n, variables);
            count += expression_get_variables(expr->right, n, variables);
            /*break*/ goto _CKL0;

            /*case 38*/ _CKL41:
            /*case 39*/ _CKL42:
            count += expression_get_variables(expr->left, n, variables);
            count += expression_get_variables(expr->right, n, variables);
            /*break*/ goto _CKL0;

            /*case 40*/ _CKL43:
            /*case 41*/ _CKL44:
            /*case 42*/ _CKL45:
            /*case 43*/ _CKL46:
            /*case 44*/ _CKL47:
            /*case 45*/ _CKL48:
            /*case 46*/ _CKL49:
            /*case 47*/ _CKL50:
            count += expression_get_variables(expr->left, n, variables);
            count += expression_get_variables(expr->right, n, variables);
            /*break*/ goto _CKL0;

            /*case 48*/ _CKL51:
            /*break*/ goto _CKL0;

            /*case 49*/ _CKL52:
            /*break*/ goto _CKL0;

            /*case 50*/ _CKL53:
            /*break*/ goto _CKL0;

            /*case 51*/ _CKL54:
            /*case 52*/ _CKL55:
            count += expression_get_variables(expr->left, n, variables);
            count += expression_get_variables(expr->right, n, variables);
            /*break*/ goto _CKL0;

            /*case 53*/ _CKL56:
            /*break*/ goto _CKL0;

            /*case 54*/ _CKL57:
            /*break*/ goto _CKL0;

            /*case 55*/ _CKL58:
            /*break*/ goto _CKL0;

            /*case 56*/ _CKL59:
            /*break*/ goto _CKL0;

            /*case 57*/ _CKL60:
            /*break*/ goto _CKL0;

            /*case 58*/ _CKL61:
            /*break*/ goto _CKL0;

            /*case 59*/ _CKL62:
            /*break*/ goto _CKL0;

            /*case 60*/ _CKL63:
            /*break*/ goto _CKL0;

            /*case 61*/ _CKL64:
            /*break*/ goto _CKL0;

            /*case 62*/ _CKL65:
            /*break*/ goto _CKL0;

            /*case 63*/ _CKL66:
            /*break*/ goto _CKL0;

            /*case 64*/ _CKL67:
            /*break*/ goto _CKL0;

            /*case 65*/ _CKL68:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return !!(count);
}

unsigned char  expression_is_lvalue(struct expression * expr)
{
    /*switch*/
    {
        register int   _R12 = expr->expression_type;
        if (_R12 == 2) goto _CKL1; /*case 2*/
        if (_R12 == 4) goto _CKL2; /*case 4*/
        if (_R12 == 3) goto _CKL3; /*case 3*/
        if (_R12 == 13) goto _CKL4; /*case 13*/
        if (_R12 == 15) goto _CKL5; /*case 15*/
        if (_R12 == 11) goto _CKL6; /*case 11*/
        if (_R12 == 32) goto _CKL7; /*case 32*/
        goto /*default*/ _CKL8;

        {
            /*case 2*/ _CKL1:
            /*case 4*/ _CKL2:
            /*case 3*/ _CKL3:
            /*case 13*/ _CKL4:
            /*case 15*/ _CKL5:
            /*case 11*/ _CKL6:
            /*case 32*/ _CKL7:
            return 1;
            /*default*/ _CKL8:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
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

unsigned char  expression_is_subjected_to_lvalue_conversion(struct expression * expression)
{
    /*switch*/
    {
        register int   _R13 = expression->expression_type;
        if (_R13 == 33) goto _CKL1; /*case 33*/
        if (_R13 == 26) goto _CKL2; /*case 26*/
        if (_R13 == 27) goto _CKL3; /*case 27*/
        if (_R13 == 16) goto _CKL4; /*case 16*/
        if (_R13 == 17) goto _CKL5; /*case 17*/
        goto /*default*/ _CKL6;

        {
            /*case 33*/ _CKL1:
            /*case 26*/ _CKL2:
            /*case 27*/ _CKL3:
            /*case 16*/ _CKL4:
            /*case 17*/ _CKL5:
            return 0;
            /*default*/ _CKL6:
            if (expression->type.storage_class_specifier_flags & 2048)
            {
                return 1;
            }
        }
        _CKL0:;
    }
    return 1;
}

void check_comparison(struct parser_ctx * ctx, struct expression * p_a_expression, struct expression * p_b_expression, struct token * op_token)
{
    struct type * p_a_type;
    struct type * p_b_type;

    p_a_type = &p_a_expression->type;
    p_b_type = &p_b_expression->type;
    if (type_is_pointer(p_a_type) && type_is_integer(p_b_type))
    {
        if (expression_is_zero(p_b_expression))
        {
        }
        else
        {
            compiler_diagnostic(3, ctx, op_token, 0U, "comparison between pointer and integer");
        }
    }
    check_diferent_enuns(ctx, op_token, p_a_expression, p_b_expression, "comparing different enums.");
}

unsigned char  type_is_owner_or_pointer_to_dtor(struct type * p_type);
unsigned char  type_is_pointed_dtor(struct type * p_type);
unsigned char  type_is_opt(struct type * p_type, unsigned char   nullable_enabled);
unsigned char  type_is_enum(struct type * p_type);
void type_print(struct type * a);

void check_assigment(struct parser_ctx * ctx, struct type * p_a_type, struct expression * p_b_expression, int   assignment_type)
{
    struct type * p_b_type;
    unsigned char   is_null_pointer_constant;
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
                    compiler_diagnostic(10, ctx, p_b_expression->first_token, 0U, "use NULL instead of 0");
                }
                else
                {
                    compiler_diagnostic(45, ctx, p_b_expression->first_token, 0U, "unusual expression/type used as null pointer constant");
                }
            }
            else
            {
                compiler_diagnostic(1340, ctx, p_b_expression->first_token, 0U, "non-pointer to pointer");
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
        compiler_diagnostic(48, ctx, 0U, &marker, "implicit conversion of nullptr constant to 'bool'");
    }
    _cake_zmem(&b_type_lvalue, 60);
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
            compiler_diagnostic(24, ctx, p_b_expression->first_token, 0U, "cannot assign a non-owner to owner");
            type_destroy(&b_type_lvalue);
            return;
        }
    }
    if (!type_is_owner(p_a_type) && type_is_owner_or_pointer_to_dtor(&p_b_expression->type))
    {
        if (p_b_expression->type.storage_class_specifier_flags & 8192)
        {
            compiler_diagnostic(22, ctx, p_b_expression->first_token, 0U, "cannot assign a temporary owner to non-owner object.");
            type_destroy(&b_type_lvalue);
            return;
        }
    }
    if (assignment_type == 0)
    {
        if (!type_is_owner(p_a_type) && type_is_owner_or_pointer_to_dtor(&p_b_expression->type))
        {
            if (p_b_expression->type.storage_class_specifier_flags & 4096)
            {
                compiler_diagnostic(1280, ctx, p_b_expression->first_token, 0U, "cannot return a automatic storage duration _Owner to non-owner");
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
                compiler_diagnostic(18, ctx, p_b_expression->first_token, 0U, "source expression of _Dtor must be addressof");
            }
        }
    }
    if (type_is_pointer(p_a_type) && !type_is_opt(p_a_type, ctx->options.null_checks_enabled) && is_null_pointer_constant)
    {
        compiler_diagnostic(34, ctx, p_b_expression->first_token, 0U, "cannot convert a null pointer constant to non-nullable pointer");
        type_destroy(&b_type_lvalue);
        return;
    }
    if (type_is_enum(p_b_type) && type_is_enum(p_a_type))
    {
        if (!type_is_same(p_b_type, p_a_type, 0))
        {
            compiler_diagnostic(39, ctx, p_b_expression->first_token, 0U, " incompatible types ");
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
        compiler_diagnostic(27, ctx, p_b_expression->first_token, 0U, " passing null as array");
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
        _cake_zmem(&a_type_lvalue, 60);
        if (type_is_array(p_a_type))
        {
            if (assignment_type == 1)
            {
                int parameter_array_size;

                parameter_array_size = p_a_type->num_of_elements;
                if (type_is_array(p_b_type))
                {
                    int argument_array_size;

                    argument_array_size = p_b_type->num_of_elements;
                    if (parameter_array_size != 0 && argument_array_size < parameter_array_size)
                    {
                        compiler_diagnostic(1130, ctx, p_b_expression->first_token, 0U, " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                    }
                }
                else
                {
                    if (is_null_pointer_constant || type_is_nullptr_t(p_b_type))
                    {
                        compiler_diagnostic(38, ctx, p_b_expression->first_token, 0U, " passing null as array");
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
            type_print(&b_type_lvalue);
            type_print(&a_type_lvalue);
            compiler_diagnostic(53, ctx, p_b_expression->first_token, 0U, " incompatible types");
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
                    compiler_diagnostic(14, ctx, p_b_expression->first_token, 0U, " discarding const at argument ");
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
                    compiler_diagnostic(14, ctx, p_b_expression->first_token, 0U, " discarding const");
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

struct object object_dup(struct object * src);
struct object object_add(struct object * a, struct object * b);
struct object object_sub(struct object * a, struct object * b);
int object_equal(struct object * a, struct object * b);
int object_not_equal(struct object * a, struct object * b);

struct object expression_eval(struct expression * p_expression)
{
    struct object  result;

    _cake_zmem(&result, 96);
    /*switch*/
    {
        register int   _R14 = p_expression->expression_type;
        if (_R14 == 0) goto _CKL1; /*case 0*/
        if (_R14 == 1) goto _CKL2; /*case 1*/
        if (_R14 == 2) goto _CKL3; /*case 2*/
        if (_R14 == 3) goto _CKL4; /*case 3*/
        if (_R14 == 4) goto _CKL5; /*case 4*/
        if (_R14 == 5) goto _CKL6; /*case 5*/
        if (_R14 == 6) goto _CKL7; /*case 6*/
        if (_R14 == 7) goto _CKL8; /*case 7*/
        if (_R14 == 8) goto _CKL9; /*case 8*/
        if (_R14 == 9) goto _CKL10; /*case 9*/
        if (_R14 == 10) goto _CKL11; /*case 10*/
        if (_R14 == 11) goto _CKL12; /*case 11*/
        if (_R14 == 12) goto _CKL13; /*case 12*/
        if (_R14 == 13) goto _CKL14; /*case 13*/
        if (_R14 == 14) goto _CKL15; /*case 14*/
        if (_R14 == 15) goto _CKL16; /*case 15*/
        if (_R14 == 16) goto _CKL17; /*case 16*/
        if (_R14 == 17) goto _CKL18; /*case 17*/
        if (_R14 == 18) goto _CKL19; /*case 18*/
        if (_R14 == 19) goto _CKL20; /*case 19*/
        if (_R14 == 20) goto _CKL21; /*case 20*/
        if (_R14 == 21) goto _CKL22; /*case 21*/
        if (_R14 == 22) goto _CKL23; /*case 22*/
        if (_R14 == 23) goto _CKL24; /*case 23*/
        if (_R14 == 24) goto _CKL25; /*case 24*/
        if (_R14 == 25) goto _CKL26; /*case 25*/
        if (_R14 == 26) goto _CKL27; /*case 26*/
        if (_R14 == 27) goto _CKL28; /*case 27*/
        if (_R14 == 28) goto _CKL29; /*case 28*/
        if (_R14 == 29) goto _CKL30; /*case 29*/
        if (_R14 == 30) goto _CKL31; /*case 30*/
        if (_R14 == 31) goto _CKL32; /*case 31*/
        if (_R14 == 32) goto _CKL33; /*case 32*/
        if (_R14 == 33) goto _CKL34; /*case 33*/
        if (_R14 == 34) goto _CKL35; /*case 34*/
        if (_R14 == 35) goto _CKL36; /*case 35*/
        if (_R14 == 36) goto _CKL37; /*case 36*/
        if (_R14 == 37) goto _CKL38; /*case 37*/
        if (_R14 == 38) goto _CKL39; /*case 38*/
        if (_R14 == 39) goto _CKL42; /*case 39*/
        if (_R14 == 40) goto _CKL45; /*case 40*/
        if (_R14 == 41) goto _CKL46; /*case 41*/
        if (_R14 == 42) goto _CKL47; /*case 42*/
        if (_R14 == 43) goto _CKL48; /*case 43*/
        if (_R14 == 44) goto _CKL49; /*case 44*/
        if (_R14 == 45) goto _CKL50; /*case 45*/
        if (_R14 == 46) goto _CKL51; /*case 46*/
        if (_R14 == 47) goto _CKL53; /*case 47*/
        if (_R14 == 48) goto _CKL55; /*case 48*/
        if (_R14 == 49) goto _CKL56; /*case 49*/
        if (_R14 == 50) goto _CKL57; /*case 50*/
        if (_R14 == 51) goto _CKL58; /*case 51*/
        if (_R14 == 52) goto _CKL63; /*case 52*/
        if (_R14 == 53) goto _CKL64; /*case 53*/
        if (_R14 == 54) goto _CKL65; /*case 54*/
        if (_R14 == 55) goto _CKL66; /*case 55*/
        if (_R14 == 56) goto _CKL67; /*case 56*/
        if (_R14 == 57) goto _CKL68; /*case 57*/
        if (_R14 == 58) goto _CKL69; /*case 58*/
        if (_R14 == 59) goto _CKL70; /*case 59*/
        if (_R14 == 60) goto _CKL71; /*case 60*/
        if (_R14 == 61) goto _CKL72; /*case 61*/
        if (_R14 == 62) goto _CKL73; /*case 62*/
        if (_R14 == 63) goto _CKL74; /*case 63*/
        if (_R14 == 64) goto _CKL75; /*case 64*/
        if (_R14 == 65) goto _CKL76; /*case 65*/
        goto _CKL0;

        {
            /*case 0*/ _CKL1:
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL2:
            /*case 2*/ _CKL3:
            result = object_dup(&p_expression->object);
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL4:
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL5:
            /*break*/ goto _CKL0;

            /*case 5*/ _CKL6:
            /*case 6*/ _CKL7:
            result = object_dup(&p_expression->object);
            /*break*/ goto _CKL0;

            /*case 7*/ _CKL8:
            /*break*/ goto _CKL0;

            /*case 8*/ _CKL9:
            result = object_dup(&p_expression->object);
            /*break*/ goto _CKL0;

            /*case 9*/ _CKL10:
            result = expression_eval(p_expression->right);
            /*break*/ goto _CKL0;

            /*case 10*/ _CKL11:
            /*break*/ goto _CKL0;

            /*case 11*/ _CKL12:
            /*break*/ goto _CKL0;

            /*case 12*/ _CKL13:
            /*break*/ goto _CKL0;

            /*case 13*/ _CKL14:
            /*break*/ goto _CKL0;

            /*case 14*/ _CKL15:
            /*break*/ goto _CKL0;

            /*case 15*/ _CKL16:
            /*break*/ goto _CKL0;

            /*case 16*/ _CKL17:
            /*break*/ goto _CKL0;

            /*case 17*/ _CKL18:
            /*break*/ goto _CKL0;

            /*case 18*/ _CKL19:
            /*break*/ goto _CKL0;

            /*case 19*/ _CKL20:
            /*break*/ goto _CKL0;

            /*case 20*/ _CKL21:
            /*break*/ goto _CKL0;

            /*case 21*/ _CKL22:
            /*break*/ goto _CKL0;

            /*case 22*/ _CKL23:
            /*break*/ goto _CKL0;

            /*case 23*/ _CKL24:
            /*break*/ goto _CKL0;

            /*case 24*/ _CKL25:
            /*break*/ goto _CKL0;

            /*case 25*/ _CKL26:
            /*break*/ goto _CKL0;

            /*case 26*/ _CKL27:
            /*break*/ goto _CKL0;

            /*case 27*/ _CKL28:
            /*break*/ goto _CKL0;

            /*case 28*/ _CKL29:
            /*break*/ goto _CKL0;

            /*case 29*/ _CKL30:
            /*break*/ goto _CKL0;

            /*case 30*/ _CKL31:
            /*break*/ goto _CKL0;

            /*case 31*/ _CKL32:
            /*break*/ goto _CKL0;

            /*case 32*/ _CKL33:
            /*break*/ goto _CKL0;

            /*case 33*/ _CKL34:
            /*break*/ goto _CKL0;

            /*case 34*/ _CKL35:
            /*break*/ goto _CKL0;

            /*case 35*/ _CKL36:
            /*break*/ goto _CKL0;

            /*case 36*/ _CKL37:
            /*break*/ goto _CKL0;

            /*case 37*/ _CKL38:
            /*break*/ goto _CKL0;

            /*case 38*/ _CKL39:
            {
                struct object  a;

                a = expression_eval(p_expression->left);
                if (object_has_constant_value(&a))
                {
                    struct object  b;

                    b = expression_eval(p_expression->right);
                    if (object_has_constant_value(&b))
                    {
                        result = object_add(&a, &b);
                    }
                }
            }
            /*break*/ goto _CKL0;

            /*case 39*/ _CKL42:
            {
                struct object  a;

                a = expression_eval(p_expression->left);
                if (object_has_constant_value(&a))
                {
                    struct object  b;

                    b = expression_eval(p_expression->right);
                    if (object_has_constant_value(&b))
                    {
                        result = object_sub(&a, &b);
                    }
                }
            }
            /*break*/ goto _CKL0;

            /*case 40*/ _CKL45:
            /*break*/ goto _CKL0;

            /*case 41*/ _CKL46:
            /*break*/ goto _CKL0;

            /*case 42*/ _CKL47:
            /*break*/ goto _CKL0;

            /*case 43*/ _CKL48:
            /*break*/ goto _CKL0;

            /*case 44*/ _CKL49:
            /*break*/ goto _CKL0;

            /*case 45*/ _CKL50:
            /*break*/ goto _CKL0;

            /*case 46*/ _CKL51:
            if (object_equal(&p_expression->left->object, &p_expression->right->object))
            {
                result = object_make_signed_int(1);
            }
            else
            {
                result = object_make_signed_int(0);
            }
            /*break*/ goto _CKL0;

            /*case 47*/ _CKL53:
            if (object_not_equal(&p_expression->left->object, &p_expression->right->object))
            {
                result = object_make_signed_int(1);
            }
            else
            {
                result = object_make_signed_int(0);
            }
            /*break*/ goto _CKL0;

            /*case 48*/ _CKL55:
            /*break*/ goto _CKL0;

            /*case 49*/ _CKL56:
            /*break*/ goto _CKL0;

            /*case 50*/ _CKL57:
            /*break*/ goto _CKL0;

            /*case 51*/ _CKL58:
            {
                struct object  a;

                a = expression_eval(p_expression->left);
                if (object_has_constant_value(&a))
                {
                    unsigned char   r1;

                    r1 = object_to_bool(&a);
                    if (r1)
                    {
                        result = object_make_signed_int(1);
                    }
                    else
                    {
                        struct object  b;

                        b = expression_eval(p_expression->right);
                        if (object_has_constant_value(&b))
                        {
                            unsigned char   r2;

                            r2 = object_to_bool(&b);
                            if (r2)
                            {
                                result = object_make_signed_int(r2);
                            }
                        }
                    }
                }
            }
            /*break*/ goto _CKL0;

            /*case 52*/ _CKL63:
            /*break*/ goto _CKL0;

            /*case 53*/ _CKL64:
            /*break*/ goto _CKL0;

            /*case 54*/ _CKL65:
            /*break*/ goto _CKL0;

            /*case 55*/ _CKL66:
            /*break*/ goto _CKL0;

            /*case 56*/ _CKL67:
            /*break*/ goto _CKL0;

            /*case 57*/ _CKL68:
            /*break*/ goto _CKL0;

            /*case 58*/ _CKL69:
            /*break*/ goto _CKL0;

            /*case 59*/ _CKL70:
            /*break*/ goto _CKL0;

            /*case 60*/ _CKL71:
            /*break*/ goto _CKL0;

            /*case 61*/ _CKL72:
            /*break*/ goto _CKL0;

            /*case 62*/ _CKL73:
            /*break*/ goto _CKL0;

            /*case 63*/ _CKL74:
            /*break*/ goto _CKL0;

            /*case 64*/ _CKL75:
            /*break*/ goto _CKL0;

            /*case 65*/ _CKL76:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return result;
}

