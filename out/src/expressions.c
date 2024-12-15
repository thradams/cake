struct balanced_token_sequence {
    struct balanced_token * head;
    struct balanced_token * tail;
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

struct scope_list {
    struct scope * head;
    struct scope * tail;
};

struct token_list {
    struct token * head;
    struct token * tail;
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
    unsigned char   static_array;
    unsigned char   address_of;
    struct param_list  params;
    struct type * next;
};

struct param {
    struct type  type;
    struct param * next;
};

struct block_item {
    struct token * first_token;
    struct declaration * declaration;
    struct unlabeled_statement * unlabeled_statement;
    struct label * label;
    struct block_item * next;
};

struct type_name {
    struct token * first_token;
    struct token * last_token;
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct declarator * abstract_declarator;
    struct type  type;
};

struct designator_list {
    struct designator * head;
    struct designator * tail;
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

struct member_declaration_list {
    struct token * first_token;
    struct token * last_token;
    struct member_declaration * head;
    struct member_declaration * tail;
};

struct _iobuf {
    void * _Placeholder;
};

struct defer_list {
    struct defer_list_item * head;
    struct defer_list_item * tail;
};

struct jump_statement {
    struct token * label;
    struct token * first_token;
    struct token * last_token;
    struct expression * expression_opt;
    int try_catch_block_index;
    struct defer_list  defer_list;
};

struct parameter_type_list {
    unsigned char   is_var_args;
    unsigned char   is_void;
    struct parameter_list * parameter_list;
};

struct attribute_token {
    int   attributes_flags;
    struct token * token;
};

struct alignment_specifier {
    struct type_name * type_name;
    struct expression * constant_expression;
    struct token * token;
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

struct type_specifier {
    int   flags;
    struct token * token;
    struct struct_or_union_specifier * struct_or_union_specifier;
    struct typeof_specifier * typeof_specifier;
    struct enum_specifier * enum_specifier;
    struct declarator * typedef_declarator;
    struct atomic_type_specifier * atomic_type_specifier;
};

struct array_declarator {
    struct direct_declarator * direct_declarator;
    struct expression * assignment_expression;
    struct expression * expression;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct token * token;
    struct token * static_token_opt;
};

struct attribute_list {
    int   attributes_flags;
    struct attribute * head;
    struct attribute * tail;
};

struct struct_entry;
struct switch_value_list {
    struct switch_value * head;
    struct switch_value * tail;
    struct switch_value * p_default;
};

struct type_specifier_qualifier {
    struct type_specifier * type_specifier;
    struct type_qualifier * type_qualifier;
    struct alignment_specifier * alignment_specifier;
    struct type_specifier_qualifier * next;
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

struct balanced_token {
    struct token * token;
    struct balanced_token * next;
};

struct condition {
    struct expression * expression;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator * p_init_declarator;
    struct token * first_token;
    struct token * last_token;
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

struct function_specifier {
    int   flags;
    struct token * token;
};

struct pointer {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct type_qualifier_list * type_qualifier_list_opt;
    struct pointer * pointer;
};

struct initializer_list {
    struct token * first_token;
    struct initializer_list_item * head;
    struct initializer_list_item * tail;
    int size;
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
    int member_index;
    struct argument_expression_list  argument_expression_list;
    struct expression * condition_expr;
    struct expression * left;
    struct expression * right;
    unsigned char   is_assignment_expression;
};

struct macro;
struct initializer {
    struct token * first_token;
    struct braced_initializer * braced_initializer;
    struct expression * assignment_expression;
};

struct unlabeled_statement {
    struct expression_statement * expression_statement;
    struct primary_block * primary_block;
    struct jump_statement * jump_statement;
};

struct init_declarator_list {
    struct init_declarator * head;
    struct init_declarator * tail;
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

struct statement {
    struct labeled_statement * labeled_statement;
    struct unlabeled_statement * unlabeled_statement;
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

struct member_declarator {
    struct declarator * declarator;
    struct expression * constant_expression;
    struct member_declarator * next;
};

struct attribute_specifier_sequence {
    struct token * first_token;
    struct token * last_token;
    int   attributes_flags;
    struct attribute_specifier * head;
    struct attribute_specifier * tail;
};

struct argument_expression {
    struct expression * expression;
    struct argument_expression * next;
    unsigned char   set_unkown;
};

struct label {
    struct expression * constant_expression;
    struct token * p_identifier_opt;
    struct token * p_first_token;
};

struct expression_statement {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct expression * expression_opt;
};

struct block_item_list {
    struct block_item * head;
    struct block_item * tail;
};

struct pragma_declaration {
    struct token * first_token;
    struct token * last_token;
};

struct compound_statement {
    struct token * first_token;
    struct token * last_token;
    struct block_item_list  block_item_list;
    struct diagnostic  diagnostic_flags;
    struct defer_list  defer_list;
};

struct generic_association {
    struct type  type;
    struct type_name * p_type_name;
    struct expression * expression;
    struct token * first_token;
    struct token * last_token;
    struct generic_association * next;
};

struct defer_statement {
    struct token * first_token;
    struct token * last_token;
    struct secondary_block * secondary_block;
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

struct primary_block {
    struct compound_statement * compound_statement;
    struct selection_statement * selection_statement;
    struct iteration_statement * iteration_statement;
    struct defer_statement * defer_statement;
    struct try_statement * try_statement;
};

struct hash_map {
    struct map_entry ** table;
    int capacity;
    int size;
};

struct atomic_type_specifier {
    struct token * token;
    struct type_name * type_name;
};

struct switch_value {
    long long value;
    struct label * p_label;
    struct switch_value * next;
};

struct storage_class_specifier {
    int   flags;
    struct token * token;
};

struct direct_declarator {
    struct token * name_opt;
    struct declarator * declarator;
    struct array_declarator * array_declarator;
    struct function_declarator * function_declarator;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
};

struct typeof_specifier_argument {
    struct expression * expression;
    struct type_name * type_name;
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

struct static_assert_declaration {
    struct token * first_token;
    struct token * last_token;
    struct expression * constant_expression;
    struct token * string_literal_opt;
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

struct try_statement {
    struct secondary_block * secondary_block;
    struct secondary_block * catch_secondary_block_opt;
    struct token * first_token;
    struct token * last_token;
    struct token * catch_token_opt;
    int try_catch_block_index;
};

struct initializer_list_item {
    struct designation * designation;
    struct initializer * initializer;
    struct initializer_list_item * next;
};

struct attribute_specifier {
    struct token * first_token;
    struct token * last_token;
    struct attribute_list * attribute_list;
    struct attribute_specifier * next;
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

struct parameter_list {
    struct parameter_declaration * head;
    struct parameter_declaration * tail;
};

struct labeled_statement {
    struct label * label;
    struct statement * statement;
};

struct type_qualifier {
    int   flags;
    struct token * token;
    struct type_qualifier * next;
};

struct init_declarator {
    unsigned char   has_shared_ownership;
    struct declarator * p_declarator;
    struct initializer * initializer;
    struct init_declarator * next;
};

struct attribute {
    int   attributes_flags;
    struct attribute_token * attribute_token;
    struct attribute_argument_clause * attribute_argument_clause;
    struct attribute * next;
};

struct simple_declaration {
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct declaration_specifiers * p_declaration_specifiers;
    struct init_declarator_list  init_declarator_list;
    struct token * first_token;
    struct token * last_token;
};

struct init_statement {
    struct expression_statement * p_expression_statement;
    struct simple_declaration * p_simple_declaration;
};

struct declaration_specifier {
    struct storage_class_specifier * storage_class_specifier;
    struct type_specifier_qualifier * type_specifier_qualifier;
    struct function_specifier * function_specifier;
    struct declaration_specifier * next;
};

struct member_declarator_list {
    struct member_declarator * head;
    struct member_declarator * tail;
};

struct attribute_argument_clause {
    struct balanced_token_sequence * p_balanced_token_sequence;
    struct token * token;
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

struct secondary_block {
    struct token * first_token;
    struct token * last_token;
    struct statement * statement;
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

struct braced_initializer {
    struct token * first_token;
    struct token * last_token;
    struct initializer_list * initializer_list;
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

struct typeof_specifier {
    struct token * first_token;
    struct token * last_token;
    struct typeof_specifier_argument * typeof_specifier_argument;
    struct type  type;
};

struct type_qualifier_list {
    int   flags;
    struct type_qualifier * head;
    struct type_qualifier * tail;
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

struct parameter_declaration {
    struct attribute_specifier_sequence * attribute_specifier_sequence_opt;
    struct declaration_specifiers * declaration_specifiers;
    struct declarator * declarator;
    struct parameter_declaration * next;
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

struct designator {
    struct expression * constant_expression_opt;
    struct token * token;
    struct designator * next;
};

struct defer_list_item {
    struct declarator * declarator;
    struct defer_statement * defer_statement;
    struct defer_list_item * next;
};

struct member_declaration {
    struct specifier_qualifier_list * specifier_qualifier_list;
    struct member_declarator_list * member_declarator_list_opt;
    struct static_assert_declaration * static_assert_declaration;
    struct pragma_declaration * pragma_declaration;
    struct attribute_specifier_sequence * p_attribute_specifier_sequence_opt;
    struct member_declaration * next;
};

struct designation {
    struct designator_list * designator_list;
    struct token * token;
};


struct param_list *type_get_func_or_func_ptr_params(struct type * p_type);
void check_assigment(struct parser_ctx * ctx, struct type * left_type, struct expression * right, int   assigment_type);
unsigned char  compiler_diagnostic_message(int   w, struct parser_ctx * ctx, struct token * p_token, struct marker * p_marker, char * fmt, ...);

static int compare_function_arguments(struct parser_ctx * ctx, struct type * p_type, struct argument_expression_list * p_argument_expression_list)
{
    if (1)
    {
        struct param * p_current_parameter_type = ((void *)0);
        struct param_list * p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_current_parameter_type = p_param_list->head;
        struct argument_expression * p_current_argument = p_argument_expression_list->head;
        while (p_current_argument && p_current_parameter_type)
        {
            check_assigment(ctx, &p_current_parameter_type->type, p_current_argument->expression, 1);
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }
        if (p_current_argument != ((void *)0) &&  !p_param_list->is_var_args)
        {
            compiler_diagnostic_message(660, ctx, p_current_argument->expression->first_token, ((void *)0), "too many arguments");
            goto _catch_label_1;
        }
        if (p_current_parameter_type != ((void *)0) &&  !p_param_list->is_void)
        {
            if (p_argument_expression_list->tail)
            {
                compiler_diagnostic_message(670, ctx, p_argument_expression_list->tail->expression->first_token, ((void *)0), "too few arguments");
            }
            else
            {
                compiler_diagnostic_message(670, ctx, ctx->current, ((void *)0), "too few arguments");
            }
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        return 1;
    }
    return 0;
}

struct enumerator *find_enumerator(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt);

unsigned char  is_enumeration_constant(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
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
    unsigned char   is_enumerator = find_enumerator(ctx, ctx->current->lexeme, ((void *)0)) != ((void *)0);
    if (is_enumerator)
    {
        ctx->current->flags = 128;
    }
    else
    {
        ctx->current->flags = 256;
    }
    return is_enumerator;
}

unsigned char  is_first_of_floating_constant(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 140 || ctx->current->type == 141;
}

unsigned char  is_first_of_integer_constant(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 136 || ctx->current->type == 137 || ctx->current->type == 138 || ctx->current->type == 139;
}

unsigned char  is_predefined_constant(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 9059 || ctx->current->type == 9060 || ctx->current->type == 9061;
}

unsigned char  is_first_of_constant(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return is_first_of_integer_constant(ctx) || is_first_of_floating_constant(ctx) || is_enumeration_constant(ctx) || (ctx->current->type == 131) || is_predefined_constant(ctx);
}

unsigned char  is_first_of_primary_expression(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 8996 || is_first_of_constant(ctx) || ctx->current->type == 130 || ctx->current->type == 40 || ctx->current->type == 9052;
}

void unexpected_end_of_file(struct parser_ctx * ctx);
void *calloc(int nmemb, unsigned int size);
void parser_match(struct parser_ctx * ctx);
unsigned char  first_of_type_name(struct parser_ctx * ctx);
struct type_name *type_name(struct parser_ctx * ctx);
struct type make_type_using_declarator(struct parser_ctx * ctx, struct declarator * pdeclarator);
int parser_match_tk(struct parser_ctx * ctx, int   type);
struct expression *assignment_expression(struct parser_ctx * ctx);
void generic_association_delete(struct generic_association * p);

struct generic_association *generic_association(struct parser_ctx * ctx)
{
    struct generic_association * p_generic_association = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_generic_association = calloc(1, sizeof  *p_generic_association);
        if (p_generic_association == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_generic_association->first_token = ctx->current;
        if (ctx->current->type == 9007)
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
        }
        else
        {
            if (first_of_type_name(ctx))
            {
                unsigned char   old = ctx->inside_generic_association;
                ctx->inside_generic_association = 1;
                p_generic_association->p_type_name = type_name(ctx);
                if (p_generic_association->p_type_name == ((void *)0))
                {
                    goto _catch_label_1;
                }
                ;
                ctx->inside_generic_association = old;
                p_generic_association->type = make_type_using_declarator(ctx, p_generic_association->p_type_name->abstract_declarator);
            }
            else
            {
                compiler_diagnostic_message(650, ctx, ctx->current, ((void *)0), "unexpected");
            }
        }
        if (parser_match_tk(ctx, 58) != 0)
        {
            goto _catch_label_1;
        }
        struct expression * p_expression_temp = assignment_expression(ctx);
        if (p_expression_temp == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_generic_association->expression = p_expression_temp;
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_generic_association->last_token = ctx->current;
    }
    else _catch_label_1:
    {
        generic_association_delete(p_generic_association);
        p_generic_association = ((void *)0);
    }
    return p_generic_association;
}

void generic_assoc_list_add(struct generic_assoc_list * p, struct generic_association * item);

struct generic_assoc_list generic_association_list(struct parser_ctx * ctx)
{
    struct generic_assoc_list  list = {0};
    if (1)
    {
        struct generic_association * p_generic_association = generic_association(ctx);
        if (p_generic_association == ((void *)0))
        {
            goto _catch_label_1;
        }
        generic_assoc_list_add(&list, p_generic_association);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        while (ctx->current->type == 44)
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            struct generic_association * p_generic_association2 = generic_association(ctx);
            if (p_generic_association2 == ((void *)0))
            {
                goto _catch_label_1;
            }
            generic_assoc_list_add(&list, p_generic_association2);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
        }
    }
    else _catch_label_1:
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
    if (list->head == ((void *)0))
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
    struct generic_association * item = p->head;
    while (item)
    {
        struct generic_association * next = item->next;
        item->next = ((void *)0);
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
    struct generic_selection * p_generic_selection = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_generic_selection = calloc(1, sizeof  *p_generic_selection);
        if (p_generic_selection == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_generic_selection->first_token = ctx->current;
        if (parser_match_tk(ctx, 9052) != 0)
        {
            goto _catch_label_1;
        }
        if (parser_match_tk(ctx, 40) != 0)
        {
            goto _catch_label_1;
        }
        if (first_of_type_name(ctx))
        {
            p_generic_selection->type_name = type_name(ctx);
            if (p_generic_selection->type_name == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        else
        {
            p_generic_selection->expression = assignment_expression(ctx);
            if (p_generic_selection->expression == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        if (parser_match_tk(ctx, 44) != 0)
        {
            goto _catch_label_1;
        }
        p_generic_selection->generic_assoc_list = generic_association_list(ctx);
        if (p_generic_selection->generic_assoc_list.head == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct type  lvalue_type = {0};
        struct type * p_type = ((void *)0);
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
                goto _catch_label_1;
            }
        }
        struct generic_association * current = p_generic_selection->generic_assoc_list.head;
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
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_generic_selection->last_token = ctx->current;
        if (parser_match_tk(ctx, 41) != 0)
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        generic_selection_delete(p_generic_selection);
        p_generic_selection = ((void *)0);
    }
    return p_generic_selection;
}

unsigned char *utf8_decode(unsigned char * s, unsigned int * c);
unsigned char *escape_sequences_decode_opt(unsigned char * p, unsigned int * out_value);
struct object object_make_unsigned_char(unsigned char value);
struct object object_make_wchar_t(unsigned short value);

struct expression *character_constant_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_expression_node = calloc(1, sizeof  *p_expression_node);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_expression_node->expression_type = 5;
        p_expression_node->first_token = ctx->current;
        p_expression_node->last_token = p_expression_node->first_token;
        p_expression_node->type.attributes_flags = 67108864;
        p_expression_node->type.category = 0;
        unsigned char * p = (unsigned char *)ctx->current->lexeme;
        if (p[0] == 117 && p[1] == 56)
        {
            p++;
            p++;
            p++;
            p_expression_node->type.type_specifier_flags = 256 | 2;
            unsigned int c = 0;
            p = utf8_decode(p, &c);
            if (p == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (c == 92)
            {
                p = escape_sequences_decode_opt(p, &c);
                if (p == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
            if ( *p != 39)
            {
                compiler_diagnostic_message(1370, ctx, ctx->current, ((void *)0), "Unicode character literals may not contain multiple characters.");
            }
            if (c > 128)
            {
                compiler_diagnostic_message(1360, ctx, ctx->current, ((void *)0), "character not encodable in a single code unit.");
            }
            p_expression_node->object = object_make_unsigned_char((unsigned char)c);
        }
        else
        {
            if (p[0] == 117)
            {
                p++;
                p++;
                p_expression_node->type.type_specifier_flags = 256 | 4;
                unsigned int c = 0;
                p = utf8_decode(p, &c);
                if (p == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (c == 92)
                {
                    p = escape_sequences_decode_opt(p, &c);
                    if (p == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                }
                if ( *p != 39)
                {
                    compiler_diagnostic_message(40, ctx, ctx->current, ((void *)0), "Unicode character literals may not contain multiple characters.");
                }
                if (c > 65535)
                {
                    compiler_diagnostic_message(40, ctx, ctx->current, ((void *)0), "Character too large for enclosing character literal type.");
                }
                p_expression_node->object = object_make_wchar_t((unsigned short)c);
            }
            else
            {
                if (p[0] == 85)
                {
                    p++;
                    p++;
                    p_expression_node->type.type_specifier_flags = 256 | 8;
                    unsigned int c = 0;
                    p = utf8_decode(p, &c);
                    if (p == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    if (c == 92)
                    {
                        p = escape_sequences_decode_opt(p, &c);
                        if (p == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                    }
                    if ( *p != 39)
                    {
                        compiler_diagnostic_message(40, ctx, ctx->current, ((void *)0), "Unicode character literals may not contain multiple characters.");
                    }
                    if (c > 4294967295LL)
                    {
                        compiler_diagnostic_message(40, ctx, ctx->current, ((void *)0), "Character too large for enclosing character literal type.");
                    }
                    p_expression_node->object = object_make_wchar_t((unsigned short)c);
                }
                else
                {
                    if (p[0] == 76)
                    {
                        p++;
                        p++;
                        p_expression_node->type.type_specifier_flags = (256 | 4);
                        long long value = 0;
                        while ( *p != 39)
                        {
                            unsigned int c = 0;
                            p = utf8_decode(p, &c);
                            if (p == ((void *)0))
                            {
                                goto _catch_label_1;
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == ((void *)0))
                                {
                                    goto _catch_label_1;
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
                                compiler_diagnostic_message(41, ctx, ctx->current, ((void *)0), "character constant too long for its type", ctx->current->lexeme);
                                break;
                            }
                        }
                        p_expression_node->object = object_make_wchar_t((unsigned short)value);
                    }
                    else
                    {
                        p++;
                        p_expression_node->type.type_specifier_flags = 8;
                        long long value = 0;
                        while ( *p != 39)
                        {
                            unsigned int c = 0;
                            p = utf8_decode(p, &c);
                            if (p == ((void *)0))
                            {
                                goto _catch_label_1;
                            }
                            if (c == 92)
                            {
                                p = escape_sequences_decode_opt(p, &c);
                                if (p == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                            }
                            value = value * 256 + c;
                            if (value > 2147483647)
                            {
                                compiler_diagnostic_message(41, ctx, ctx->current, ((void *)0), "character constant too long for its type", ctx->current->lexeme);
                                break;
                            }
                        }
                        p_expression_node->object = object_make_wchar_t((unsigned short)value);
                    }
                }
            }
        }
        parser_match(ctx);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

int  parse_number(char * lexeme, char suffix[4], char erromsg[100]);
unsigned long long strtoull(char * _String, char ** _EndPtr, int _Radix);
int *_errno(void);
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
    if (ctx->current == ((void *)0))
    {
        unexpected_end_of_file(ctx);
        return 1;
    }
    struct token * token = ctx->current;
    int c = 0;
    char buffer[260] = {0};
    char * s = token->lexeme;
    while ( *s)
    {
        if ( *s != 39)
        {
            buffer[c] =  *s;
            c++;
        }
        s++;
    }
    char errormsg[100] = {0};
    char suffix[4] = {0};
    int   r = parse_number(buffer, suffix, errormsg);
    if (r == 0)
    {
        compiler_diagnostic_message(1380, ctx, token, ((void *)0), errormsg);
        return 0;
    }
    switch (token->type)
    {
        case 136 :
        case 137 :
        case 138 :
        case 139 :
        {
            unsigned long long value = 0;
            switch (token->type)
            {
                case 136 :
                value = strtoull(buffer, ((void *)0), 10);
                break;
                case 137 :
                if (buffer[1] == 111 || buffer[1] == 79)
                {
                    value = strtoull(buffer + 2, ((void *)0), 8);
                }
                else
                {
                    value = strtoull(buffer + 1, ((void *)0), 8);
                }
                break;
                case 138 :
                value = strtoull(buffer + 2, ((void *)0), 16);
                break;
                case 139 :
                value = strtoull(buffer + 2, ((void *)0), 2);
                break;
                default:
                break;
            }
            if (value == 18446744073709551615ULL && ( *_errno()) == 34)
            {
                compiler_diagnostic_message(1350, ctx, token, ((void *)0), "integer literal is too large to be represented in any integer type");
            }
            if (suffix[0] == 85)
            {
                if (value <= 4294967295LL && suffix[1] != 76)
                {
                    p_expression_node->object = object_make_unsigned_int((unsigned int)value);
                    p_expression_node->type.type_specifier_flags = (8 | 256);
                }
                else
                {
                    if (value <= 4294967295UL && suffix[2] != 76)
                    {
                        p_expression_node->object = object_make_unsigned_long((unsigned long)value);
                        p_expression_node->type.type_specifier_flags = 16 | 256;
                    }
                    else
                    {
                        p_expression_node->object = object_make_unsigned_long_long((unsigned long long)value);
                        p_expression_node->type.type_specifier_flags = 4194304 | 256;
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
                            compiler_diagnostic_message(49, ctx, token, ((void *)0), "integer literal is too large to be represented in a signed integer type, interpreting as unsigned");
                            p_expression_node->object = object_make_signed_long_long(value);
                            p_expression_node->type.type_specifier_flags = 4194304 | 256;
                        }
                    }
                }
            }
        }
        break;
        case 140 :
        case 141 :
        {
            if (suffix[0] == 70)
            {
                float value = strtof(buffer, ((void *)0));
                if (value == ((float)((float)(1 * 1))) && ( *_errno()) == 34)
                {
                }
                p_expression_node->type.type_specifier_flags = 32;
                p_expression_node->object = object_make_float(value);
            }
            else
            {
                if (suffix[0] == 76)
                {
                    long double value = strtold(buffer, ((void *)0));
                    if (value == ((long double)((float)(1 * 1))) && ( *_errno()) == 34)
                    {
                    }
                    p_expression_node->type.type_specifier_flags = 64 | 16;
                    p_expression_node->object = object_make_long_double(value);
                }
                else
                {
                    double value = strtod(buffer, ((void *)0));
                    if (value == ((double)((float)(1 * 1))) && ( *_errno()) == 34)
                    {
                    }
                    p_expression_node->object = object_make_double(value);
                    p_expression_node->type.type_specifier_flags = 64;
                }
            }
        }
        break;
        default:
        ;
    }
    return 0;
}

static unsigned char  is_integer_or_floating_constant(int   type)
{
    return type == 136 || type == 137 || type == 138 || type == 139 || type == 140 || type == 141;
}

struct map_entry *find_variables(struct parser_ctx * ctx, char * lexeme, struct scope ** ppscope_opt);
struct type type_make_enumerator(struct enum_specifier * enum_specifier);
unsigned char  type_is_deprecated(struct type * p_type);
struct type type_dup(struct type * p_type);
struct object object_make_reference(struct object * object);
int strcmp(char * _Str1, char * _Str2);
struct type type_make_literal_string(int size, int   chartype);
unsigned int strlen(char * _Str);
int get_char_type(char * s);
int string_literal_char_byte_size(char * s);
int string_literal_byte_size_not_zero_included(char * s);
struct object object_make_bool(unsigned char   value);
struct object object_make_nullptr();
struct expression *expression(struct parser_ctx * ctx);

struct expression *primary_expression(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        unexpected_end_of_file(ctx);
        return ((void *)0);
    }
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        if (ctx->current->type == 8996)
        {
            p_expression_node = calloc(1, sizeof  *p_expression_node);
            if (p_expression_node == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_expression_node->first_token = ctx->current;
            p_expression_node->last_token = ctx->current;
            struct map_entry * p_entry = find_variables(ctx, ctx->current->lexeme, ((void *)0));
            if (p_entry && p_entry->type == 3)
            {
                ;
                struct enumerator * p_enumerator = p_entry->data.p_enumerator;
                p_expression_node->expression_type = 1;
                p_expression_node->object = p_enumerator->value;
                p_expression_node->type = type_make_enumerator(p_enumerator->enum_specifier);
            }
            else
            {
                if (p_entry && (p_entry->type == 4 || p_entry->type == 5))
                {
                    struct declarator * p_declarator = ((void *)0);
                    struct init_declarator * p_init_declarator = ((void *)0);
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
                        compiler_diagnostic_message(2, ctx, ctx->current, ((void *)0), "'%s' is deprecated", ctx->current->lexeme);
                    }
                    p_declarator->num_uses++;
                    p_expression_node->declarator = p_declarator;
                    p_expression_node->expression_type = 2;
                    p_expression_node->type = type_dup(&p_declarator->type);
                    p_expression_node->object = object_make_reference(&p_declarator->object);
                }
                else
                {
                    if (ctx->p_current_function_opt && strcmp(ctx->current->lexeme, "__func__") == 0)
                    {
                        char * func_str = "?";
                        if (ctx->p_current_function_opt->init_declarator_list.head != ((void *)0) && ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name_opt)
                        {
                            func_str = ctx->p_current_function_opt->init_declarator_list.head->p_declarator->name_opt->lexeme;
                        }
                        p_expression_node->expression_type = 4;
                        p_expression_node->first_token = ctx->current;
                        p_expression_node->last_token = ctx->current;
                        p_expression_node->type = type_make_literal_string(strlen(func_str) + 1, 2);
                    }
                    else
                    {
                        compiler_diagnostic_message(680, ctx, ctx->current, ((void *)0), "not found '%s'", ctx->current->lexeme);
                        goto _catch_label_1;
                    }
                }
            }
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
        }
        else
        {
            if (ctx->current->type == 130)
            {
                p_expression_node = calloc(1, sizeof  *p_expression_node);
                if (p_expression_node == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_expression_node->expression_type = 3;
                p_expression_node->first_token = ctx->current;
                p_expression_node->last_token = ctx->current;
                int   char_type = 2;
                if (get_char_type(ctx->current->lexeme) == 2)
                {
                    char_type = (256 | 4);
                }
                int char_byte_size = string_literal_char_byte_size(ctx->current->lexeme);
                int number_of_bytes = 0;
                struct object * last = ((void *)0);
                while (ctx->current->type == 130)
                {
                    unsigned char * it = ctx->current->lexeme + 1;
                    unsigned int value = 0;
                    while (it &&  *it != 34)
                    {
                        if ( *it == 92)
                        {
                            it = escape_sequences_decode_opt(it, &value);
                        }
                        else
                        {
                            value =  *it;
                            it++;
                        }
                        struct object * p_new = calloc(1, sizeof  *p_new);
                        if (p_new == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                        p_new->state = 3;
                        p_new->value_type = 3;
                        p_new->value.signed_char_value = value;
                        if (p_expression_node->object.members == ((void *)0))
                        {
                            p_expression_node->object.members = p_new;
                        }
                        else
                        {
                            last->next = p_new;
                        }
                        last = p_new;
                    }
                    struct object * p_new = calloc(1, sizeof  *p_new);
                    if (p_new == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    p_new->state = 3;
                    p_new->value_type = 3;
                    p_new->value.signed_char_value = 0;
                    if (last == ((void *)0))
                    {
                        p_expression_node->object.members = p_new;
                    }
                    else
                    {
                        last->next = p_new;
                    }
                    number_of_bytes = string_literal_byte_size_not_zero_included(ctx->current->lexeme);
                    parser_match(ctx);
                    if (ctx->current == ((void *)0))
                    {
                        unexpected_end_of_file(ctx);
                        goto _catch_label_1;
                    }
                }
                p_expression_node->type = type_make_literal_string(number_of_bytes + (1 * char_byte_size), char_type);
            }
            else
            {
                if (ctx->current->type == 131)
                {
                    p_expression_node = character_constant_expression(ctx);
                }
                else
                {
                    if (ctx->current->type == 9059 || ctx->current->type == 9060)
                    {
                        p_expression_node = calloc(1, sizeof  *p_expression_node);
                        if (p_expression_node == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                        p_expression_node->expression_type = 6;
                        p_expression_node->first_token = ctx->current;
                        p_expression_node->last_token = ctx->current;
                        p_expression_node->object = object_make_bool(ctx->current->type == 9059);
                        p_expression_node->type.type_specifier_flags = 512;
                        p_expression_node->type.type_qualifier_flags = 0;
                        parser_match(ctx);
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            goto _catch_label_1;
                        }
                    }
                    else
                    {
                        if (ctx->current->type == 9061)
                        {
                            p_expression_node = calloc(1, sizeof  *p_expression_node);
                            if (p_expression_node == ((void *)0))
                            {
                                goto _catch_label_1;
                            }
                            p_expression_node->expression_type = 6;
                            p_expression_node->first_token = ctx->current;
                            p_expression_node->last_token = ctx->current;
                            p_expression_node->object = object_make_nullptr();
                            p_expression_node->type.type_specifier_flags = 16777216;
                            p_expression_node->type.type_qualifier_flags = 0;
                            parser_match(ctx);
                            if (ctx->current == ((void *)0))
                            {
                                unexpected_end_of_file(ctx);
                                goto _catch_label_1;
                            }
                        }
                        else
                        {
                            if (is_integer_or_floating_constant(ctx->current->type))
                            {
                                p_expression_node = calloc(1, sizeof  *p_expression_node);
                                if (p_expression_node == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                                p_expression_node->first_token = ctx->current;
                                p_expression_node->last_token = ctx->current;
                                p_expression_node->expression_type = 8;
                                convert_to_number(ctx, p_expression_node, 0);
                                parser_match(ctx);
                                if (ctx->current == ((void *)0))
                                {
                                    unexpected_end_of_file(ctx);
                                    goto _catch_label_1;
                                }
                            }
                            else
                            {
                                if (ctx->current->type == 9052)
                                {
                                    p_expression_node = calloc(1, sizeof  *p_expression_node);
                                    if (p_expression_node == ((void *)0))
                                    {
                                        goto _catch_label_1;
                                    }
                                    p_expression_node->first_token = ctx->current;
                                    p_expression_node->expression_type = 7;
                                    p_expression_node->generic_selection = generic_selection(ctx);
                                    if (p_expression_node->generic_selection == ((void *)0))
                                    {
                                        goto _catch_label_1;
                                    }
                                    p_expression_node->last_token = p_expression_node->generic_selection->last_token;
                                    if (p_expression_node->generic_selection->p_view_selected_expression)
                                    {
                                        p_expression_node->type = type_dup(&p_expression_node->generic_selection->p_view_selected_expression->type);
                                        p_expression_node->object = p_expression_node->generic_selection->p_view_selected_expression->object;
                                    }
                                    else
                                    {
                                        compiler_diagnostic_message(690, ctx, ctx->current, ((void *)0), "no match for generic");
                                    }
                                }
                                else
                                {
                                    if (ctx->current->type == 40)
                                    {
                                        p_expression_node = calloc(1, sizeof  *p_expression_node);
                                        if (p_expression_node == ((void *)0))
                                        {
                                            goto _catch_label_1;
                                        }
                                        p_expression_node->expression_type = 9;
                                        p_expression_node->first_token = ctx->current;
                                        parser_match(ctx);
                                        if (ctx->current == ((void *)0))
                                        {
                                            unexpected_end_of_file(ctx);
                                            goto _catch_label_1;
                                        }
                                        p_expression_node->right = expression(ctx);
                                        if (p_expression_node->right == ((void *)0))
                                        {
                                            goto _catch_label_1;
                                        }
                                        p_expression_node->type = type_dup(&p_expression_node->right->type);
                                        p_expression_node->object = p_expression_node->right->object;
                                        if (ctx->current == ((void *)0))
                                        {
                                            unexpected_end_of_file(ctx);
                                            goto _catch_label_1;
                                        }
                                        p_expression_node->last_token = ctx->current;
                                        if (parser_match_tk(ctx, 41) != 0)
                                        {
                                            goto _catch_label_1;
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic_message(650, ctx, ctx->current, ((void *)0), "unexpected");
                                        goto _catch_label_1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
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
    struct argument_expression_list  list = {0};
    struct argument_expression * p_argument_expression = ((void *)0);
    if (1)
    {
        p_argument_expression = calloc(1, sizeof (struct argument_expression));
        if (p_argument_expression == ((void *)0))
        {
            goto _catch_label_1;
        }
        struct expression * p_assignment_expression = assignment_expression(ctx);
        if (p_assignment_expression == ((void *)0))
        {
            argument_expression_delete(p_argument_expression);
            goto _catch_label_1;
        }
        p_argument_expression->expression = p_assignment_expression;
        argument_expression_list_push(&list, p_argument_expression);
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        while (ctx->current->type == 44)
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            struct argument_expression * p_argument_expression_2 = calloc(1, sizeof  *p_argument_expression_2);
            if (p_argument_expression_2 == ((void *)0))
            {
                goto _catch_label_1;
            }
            struct expression * p_assignment_expression_2 = assignment_expression(ctx);
            if (p_assignment_expression_2 == ((void *)0))
            {
                argument_expression_delete(p_argument_expression_2);
                goto _catch_label_1;
            }
            p_argument_expression_2->expression = p_assignment_expression_2;
            argument_expression_list_push(&list, p_argument_expression_2);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
        }
    }
    else _catch_label_1:
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
        p_type->type_qualifier_flags = 1;
    }
    p_type->storage_class_specifier_flags = struct_type->storage_class_specifier_flags;
    if (struct_type->type_qualifier_flags & 64)
    {
        p_type->type_qualifier_flags =  ~16;
    }
    if (struct_type->type_qualifier_flags & 128)
    {
        p_type->type_qualifier_flags = 128;
    }
}

struct type type_remove_pointer(struct type * p_type);

static void fix_arrow_member_type(struct type * p_type, struct type * left, struct type * member_type)
{
    struct type  t = type_remove_pointer(left);
    if (t.type_qualifier_flags & 1)
    {
        p_type->type_qualifier_flags = 1;
    }
    if (t.type_qualifier_flags & 128)
    {
        p_type->type_qualifier_flags = 128;
    }
    if (t.type_qualifier_flags & 64)
    {
        p_type->type_qualifier_flags =  ~16;
    }
    type_destroy(&t);
}

unsigned char  type_is_pointer_or_array(struct type * p_type);
unsigned char  type_is_pointer(struct type * p_type);
unsigned char  type_is_array(struct type * p_type);
struct type get_array_item_type(struct type * p_type);
unsigned char  object_has_constant_value(struct object * a);
unsigned long long object_to_unsigned_long_long(struct object * a);
struct object *object_get_member(struct object * p_object, int index);
unsigned char  type_is_function_or_function_pointer(struct type * p_type);
struct type get_function_return_type(struct type * p_type);
int make_object(struct type * p_type, struct object * obj);
struct struct_or_union_specifier *find_struct_or_union_specifier(struct parser_ctx * ctx, char * lexeme);
struct struct_or_union_specifier *get_complete_struct_or_union_specifier(struct struct_or_union_specifier * p_struct_or_union_specifier);
struct member_declarator *find_member_declarator(struct member_declaration_list * list, char * name, int * p_member_index);
unsigned char  type_is_struct_or_union(struct type * p_type);
unsigned char  type_is_owner(struct type * p_type);
unsigned char  expression_is_lvalue(struct expression * expr);
struct token *previous_parser_token(struct token * token);

struct expression *postfix_expression_tail(struct parser_ctx * ctx, struct expression * p_expression_node_param)
{
    struct expression * p_expression_node = p_expression_node_param;
    if (1)
    {
        while (ctx->current != ((void *)0))
        {
            if (ctx->current->type == 91)
            {
                struct expression * p_expression_node_new = calloc(1, sizeof  *p_expression_node_new);
                if (p_expression_node_new == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_expression_node->last_token = ctx->current;
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = 13;
                if ( !type_is_pointer_or_array(&p_expression_node->type))
                {
                    compiler_diagnostic_message(700, ctx, ctx->current, ((void *)0), "subscripted value is neither array nor pointer");
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
                if (ctx->current == ((void *)0))
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(p_expression_node_new);
                    goto _catch_label_1;
                }
                p_expression_node_new->right = expression(ctx);
                if (p_expression_node_new->right == ((void *)0))
                {
                    expression_delete(p_expression_node_new);
                    goto _catch_label_1;
                }
                if (object_has_constant_value(&p_expression_node_new->right->object))
                {
                    unsigned long long index = object_to_unsigned_long_long(&p_expression_node_new->right->object);
                    if (type_is_array(&p_expression_node->type))
                    {
                        if (p_expression_node->type.num_of_elements > 0)
                        {
                            if (index >= (unsigned long long)p_expression_node->type.num_of_elements)
                            {
                                compiler_diagnostic_message(41, ctx, ctx->current, ((void *)0), "index %d is past the end of the array", index);
                            }
                            struct object * it = object_get_member(&p_expression_node->object, index);
                            if (it != ((void *)0))
                            {
                                p_expression_node_new->object = object_make_reference(it);
                            }
                        }
                    }
                }
                if (parser_match_tk(ctx, 93) != 0)
                {
                    expression_delete(p_expression_node_new);
                    p_expression_node_new = ((void *)0);
                    goto _catch_label_1;
                }
                p_expression_node_new->left = p_expression_node;
                p_expression_node = p_expression_node_new;
            }
            else
            {
                if (ctx->current->type == 40)
                {
                    struct expression * p_expression_node_new = calloc(1, sizeof  *p_expression_node_new);
                    if (p_expression_node_new == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    p_expression_node->last_token = ctx->current;
                    p_expression_node_new->first_token = p_expression_node->first_token;
                    p_expression_node_new->expression_type = 12;
                    if ( !type_is_function_or_function_pointer(&p_expression_node->type))
                    {
                        compiler_diagnostic_message(710, ctx, ctx->current, ((void *)0), "called object is not attr function or function pointer");
                    }
                    p_expression_node_new->type = get_function_return_type(&p_expression_node->type);
                    parser_match(ctx);
                    if (ctx->current == ((void *)0))
                    {
                        unexpected_end_of_file(ctx);
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = ((void *)0);
                        goto _catch_label_1;
                    }
                    if (ctx->current->type != 41)
                    {
                        p_expression_node_new->argument_expression_list = argument_expression_list(ctx);
                    }
                    if (parser_match_tk(ctx, 41) != 0)
                    {
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = ((void *)0);
                        goto _catch_label_1;
                    }
                    compare_function_arguments(ctx, &p_expression_node->type, &p_expression_node_new->argument_expression_list);
                    if (ctx->previous == ((void *)0))
                    {
                        expression_delete(p_expression_node_new);
                        p_expression_node_new = ((void *)0);
                        goto _catch_label_1;
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
                        struct expression * p_expression_node_new = calloc(1, sizeof  *p_expression_node_new);
                        if (p_expression_node_new == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                        p_expression_node->last_token = ctx->current;
                        p_expression_node_new->first_token = ctx->current;
                        p_expression_node_new->expression_type = 14;
                        p_expression_node_new->left = p_expression_node;
                        p_expression_node = ((void *)0);
                        p_expression_node_new->declarator = p_expression_node_new->left->declarator;
                        parser_match(ctx);
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            expression_delete(p_expression_node_new);
                            p_expression_node_new = ((void *)0);
                            goto _catch_label_1;
                        }
                        if (p_expression_node_new->left->type.type_specifier_flags & 32768)
                        {
                            ;
                            struct struct_or_union_specifier * p_complete = find_struct_or_union_specifier(ctx, p_expression_node_new->left->type.struct_or_union_specifier->tag_name);
                            if (p_complete)
                            {
                                p_complete = get_complete_struct_or_union_specifier(p_complete);
                            }
                            if (p_complete)
                            {
                                ;
                                int member_index = 0;
                                struct member_declarator * p_member_declarator = find_member_declarator(&p_complete->member_declaration_list, ctx->current->lexeme, &member_index);
                                if (p_member_declarator)
                                {
                                    p_expression_node_new->member_index = member_index;
                                    if (p_member_declarator->declarator)
                                    {
                                        p_expression_node_new->type = make_type_using_declarator(ctx, p_member_declarator->declarator);
                                    }
                                    else
                                    {
                                    }
                                    if (p_member_declarator->declarator != ((void *)0))
                                    {
                                        fix_member_type(&p_expression_node_new->type, &p_expression_node_new->left->type, &p_member_declarator->declarator->type);
                                    }
                                    struct object * object = object_get_member(&p_expression_node_new->left->object, member_index);
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
                                    compiler_diagnostic_message(720, ctx, ctx->current, ((void *)0), "member '%s' not found in 'struct %s'", ctx->current->lexeme, p_complete->tag_name);
                                }
                            }
                            else
                            {
                                compiler_diagnostic_message(720, ctx, ctx->current, ((void *)0), "incomplete struct type '%s'", p_expression_node_new->left->type.struct_or_union_specifier->tag_name);
                            }
                            if (parser_match_tk(ctx, 8996) != 0)
                            {
                                expression_delete(p_expression_node_new);
                                p_expression_node_new = ((void *)0);
                                goto _catch_label_1;
                            }
                        }
                        else
                        {
                            compiler_diagnostic_message(730, ctx, ctx->current, ((void *)0), "structure or union required");
                        }
                        p_expression_node = p_expression_node_new;
                    }
                    else
                    {
                        if (ctx->current->type == 11582)
                        {
                            struct expression * p_expression_node_new = calloc(1, sizeof  *p_expression_node_new);
                            if (p_expression_node_new == ((void *)0))
                            {
                                goto _catch_label_1;
                            }
                            p_expression_node->last_token = ctx->current;
                            p_expression_node_new->first_token = p_expression_node->first_token;
                            p_expression_node_new->last_token = ctx->current;
                            p_expression_node_new->expression_type = 15;
                            parser_match(ctx);
                            if (ctx->current == ((void *)0))
                            {
                                expression_delete(p_expression_node_new);
                                p_expression_node_new = ((void *)0);
                                goto _catch_label_1;
                            }
                            if (type_is_pointer_or_array(&p_expression_node->type))
                            {
                                struct type  item_type = {0};
                                if (type_is_array(&p_expression_node->type))
                                {
                                    compiler_diagnostic_message(19, ctx, ctx->current, ((void *)0), "using indirection '->' in array");
                                    item_type = get_array_item_type(&p_expression_node->type);
                                }
                                else
                                {
                                    item_type = type_remove_pointer(&p_expression_node->type);
                                }
                                if (type_is_struct_or_union(&item_type))
                                {
                                    ;
                                    ;
                                    struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_expression_node->type.next->struct_or_union_specifier);
                                    if (p_complete)
                                    {
                                        int member_index = 0;
                                        struct member_declarator * p_member_declarator = find_member_declarator(&p_complete->member_declaration_list, ctx->current->lexeme, &member_index);
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
                                            compiler_diagnostic_message(720, ctx, ctx->current, ((void *)0), "member '%s' not found in struct '%s'", ctx->current->lexeme, p_expression_node->type.next->struct_or_union_specifier->tag_name);
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic_message(740, ctx, ctx->current, ((void *)0), "struct '%s' is incomplete.", ctx->current->lexeme);
                                    }
                                    if (parser_match_tk(ctx, 8996) != 0)
                                    {
                                        type_destroy(&item_type);
                                        expression_delete(p_expression_node_new);
                                        p_expression_node_new = ((void *)0);
                                        goto _catch_label_1;
                                    }
                                }
                                else
                                {
                                    compiler_diagnostic_message(730, ctx, ctx->current, ((void *)0), "structure or union required");
                                }
                                type_destroy(&item_type);
                            }
                            else
                            {
                                compiler_diagnostic_message(730, ctx, ctx->current, ((void *)0), "structure or union required");
                            }
                            p_expression_node_new->left = p_expression_node;
                            p_expression_node = p_expression_node_new;
                        }
                        else
                        {
                            if (ctx->current->type == 11051)
                            {
                                p_expression_node->last_token = ctx->current;
                                if (type_is_owner(&p_expression_node->type))
                                {
                                    compiler_diagnostic_message(1310, ctx, p_expression_node->first_token, ((void *)0), "operator ++ cannot be used in _Owner pointers");
                                }
                                if ( !expression_is_lvalue(p_expression_node))
                                {
                                    compiler_diagnostic_message(1230, ctx, p_expression_node->first_token, ((void *)0), "lvalue required as increment operand");
                                }
                                struct expression * p_expression_node_new = calloc(1, sizeof  *p_expression_node_new);
                                if (p_expression_node_new == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                                p_expression_node->last_token = ctx->current;
                                p_expression_node_new->first_token = ctx->current;
                                p_expression_node_new->expression_type = 16;
                                p_expression_node_new->type = type_dup(&p_expression_node->type);
                                parser_match(ctx);
                                if (ctx->current == ((void *)0))
                                {
                                    unexpected_end_of_file(ctx);
                                    expression_delete(p_expression_node_new);
                                    p_expression_node_new = ((void *)0);
                                    goto _catch_label_1;
                                }
                                p_expression_node_new->left = p_expression_node;
                                p_expression_node = p_expression_node_new;
                            }
                            else
                            {
                                if (ctx->current->type == 11565)
                                {
                                    p_expression_node->last_token = ctx->current;
                                    if (type_is_owner(&p_expression_node->type))
                                    {
                                        compiler_diagnostic_message(1320, ctx, p_expression_node->first_token, ((void *)0), "operator -- cannot be used in owner pointers");
                                    }
                                    if ( !expression_is_lvalue(p_expression_node))
                                    {
                                        compiler_diagnostic_message(1230, ctx, p_expression_node->first_token, ((void *)0), "lvalue required as decrement operand");
                                    }
                                    struct expression * p_expression_node_new = calloc(1, sizeof  *p_expression_node_new);
                                    if (p_expression_node_new == ((void *)0))
                                    {
                                        goto _catch_label_1;
                                    }
                                    p_expression_node_new->first_token = ctx->current;
                                    p_expression_node_new->expression_type = 17;
                                    p_expression_node_new->type = type_dup(&p_expression_node->type);
                                    parser_match(ctx);
                                    if (ctx->current == ((void *)0))
                                    {
                                        unexpected_end_of_file(ctx);
                                        expression_delete(p_expression_node_new);
                                        p_expression_node_new = ((void *)0);
                                        goto _catch_label_1;
                                    }
                                    p_expression_node_new->left = p_expression_node;
                                    p_expression_node = p_expression_node_new;
                                }
                                else
                                {
                                    struct token * p_last = previous_parser_token(ctx->current);
                                    if (p_last == ((void *)0))
                                    {
                                        goto _catch_label_1;
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
    else _catch_label_1:
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
int initializer_init_new(struct parser_ctx * ctx, struct type * p_current_object_type, struct object * p_current_object, struct initializer * braced_initializer, unsigned char   is_constant);

struct expression *postfix_expression_type_name(struct parser_ctx * ctx, struct type_name * p_type_name_par)
{
    struct type_name * p_type_name = p_type_name_par;
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        p_expression_node = calloc(1, sizeof  *p_expression_node);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        ;
        struct token * p_previous = previous_parser_token(p_type_name->first_token);
        if (p_previous == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_expression_node->first_token = p_previous;
        ;
        p_expression_node->type_name = p_type_name;
        p_type_name = ((void *)0);
        p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);
        if (type_is_function(&p_expression_node->type_name->abstract_declarator->type))
        {
            p_expression_node->expression_type = 10;
            struct scope * parameters_scope = &p_expression_node->type_name->abstract_declarator->direct_declarator->function_declarator->parameters_scope;
            scope_list_push(&ctx->scopes, parameters_scope);
            p_expression_node->compound_statement = function_body(ctx);
            scope_list_pop(&ctx->scopes);
        }
        else
        {
            p_expression_node->expression_type = 11;
            p_expression_node->braced_initializer = braced_initializer(ctx);
            p_expression_node->type = type_dup(&p_expression_node->type_name->type);
            int er = make_object(&p_expression_node->type, &p_expression_node->object);
            if (er != 0)
            {
                compiler_diagnostic_message(740, ctx, p_expression_node->first_token, ((void *)0), "incomplete struct/union type");
                goto _catch_label_1;
            }
            unsigned char   is_constant = type_is_const(&p_expression_node->type) || p_expression_node->type.storage_class_specifier_flags & 64;
            object_default_initialization(&p_expression_node->object, is_constant);
            struct initializer  initializer = {0};
            initializer.braced_initializer = p_expression_node->braced_initializer;
            initializer.first_token = p_expression_node->first_token;
            initializer_init_new(ctx, &p_expression_node->type, &p_expression_node->object, &initializer, is_constant);
        }
        if (ctx->previous == ((void *)0))
        {
            goto _catch_label_1;
        }
        p_expression_node->last_token = ctx->previous;
        p_expression_node = postfix_expression_tail(ctx, p_expression_node);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    type_name_delete(p_type_name);
    return p_expression_node;
}

struct compound_statement *compound_statement(struct parser_ctx * ctx);

struct expression *postfix_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        if (first_of_type_name_ahead(ctx))
        {
            ;
            p_expression_node = calloc(1, sizeof  *p_expression_node);
            if (p_expression_node == ((void *)0))
            {
                goto _catch_label_1;
            }
            ;
            p_expression_node->first_token = ctx->current;
            if (parser_match_tk(ctx, 40) != 0)
            {
                goto _catch_label_1;
            }
            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);
            if (parser_match_tk(ctx, 41) != 0)
            {
                goto _catch_label_1;
            }
            unsigned char   is_function_type = type_is_function(&p_expression_node->type);
            if (is_function_type)
            {
                p_expression_node->expression_type = 10;
                p_expression_node->compound_statement = compound_statement(ctx);
                if (p_expression_node->compound_statement == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_expression_node->last_token = p_expression_node->compound_statement->last_token;
            }
            else
            {
                p_expression_node->expression_type = 11;
                p_expression_node->braced_initializer = braced_initializer(ctx);
                if (p_expression_node->braced_initializer == ((void *)0))
                {
                    goto _catch_label_1;
                }
                if (ctx->current == ((void *)0))
                {
                    unexpected_end_of_file(ctx);
                    goto _catch_label_1;
                }
                p_expression_node->last_token = ctx->current;
            }
        }
        else
        {
            p_expression_node = primary_expression(ctx);
            if (p_expression_node == ((void *)0))
            {
                goto _catch_label_1;
            }
        }
        p_expression_node = postfix_expression_tail(ctx, p_expression_node);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

unsigned char  is_first_of_compiler_function(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return ctx->current->type == 9074 || ctx->current->type == 9076 || ctx->current->type == 9075 || ctx->current->type == 9073 || ctx->current->type == 9077 || ctx->current->type == 9078 || ctx->current->type == 9056 || ctx->current->type == 9079 || ctx->current->type == 9080 || ctx->current->type == 9081 || ctx->current->type == 9082;
}

unsigned char  is_first_of_unary_expression(struct parser_ctx * ctx)
{
    if (ctx->current == ((void *)0))
    {
        return 0;
    }
    return first_of_postfix_expression(ctx) || ctx->current->type == 11051 || ctx->current->type == 11565 || ctx->current->type == 38 || ctx->current->type == 42 || ctx->current->type == 43 || ctx->current->type == 45 || ctx->current->type == 126 || ctx->current->type == 33 || ctx->current->type == 9030 || ctx->current->type == 9031 || ctx->current->type == 9044 || is_first_of_compiler_function(ctx);
}

int  type_get_category(struct type * p_type);
unsigned char  type_is_vla(struct type * p_type);

static int check_sizeof_argument(struct parser_ctx * ctx, struct expression * p_expression, struct type * p_type)
{
    int   category = type_get_category(p_type);
    if (category == 1)
    {
    }
    else
    {
        if (category == 0 && p_type->type_specifier_flags & 32768)
        {
            ;
            struct struct_or_union_specifier * p_complete = get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);
            if (p_complete == ((void *)0))
            {
                compiler_diagnostic_message(740, ctx, p_expression->first_token, ((void *)0), "struct is incomplete type");
                return  -1;
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
                    compiler_diagnostic_message(46, ctx, p_expression->first_token, ((void *)0), "sizeof applied to array function parameter");
                }
            }
        }
    }
    return 0;
}

struct expression *cast_expression(struct parser_ctx * ctx);
unsigned char  object_to_bool(struct object * a);
struct type type_make_int_bool_like();
unsigned char  type_is_integer(struct type * p_type);
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
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (ctx->current->type == 11051 || ctx->current->type == 11565)
        {
            struct expression * new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
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
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            new_expression->right = unary_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            new_expression->type = type_dup(&new_expression->right->type);
            p_expression_node = new_expression;
        }
        else
        {
            if (ctx->current->type == 38 || ctx->current->type == 42 || ctx->current->type == 43 || ctx->current->type == 45 || ctx->current->type == 126 || ctx->current->type == 33)
            {
                struct expression * new_expression = calloc(1, sizeof  *new_expression);
                if (new_expression == ((void *)0))
                {
                    goto _catch_label_1;
                }
                new_expression->first_token = ctx->current;
                struct token * op_position = ctx->current;
                int   op = ctx->current->type;
                parser_match(ctx);
                if (ctx->current == ((void *)0))
                {
                    unexpected_end_of_file(ctx);
                    expression_delete(new_expression);
                    goto _catch_label_1;
                }
                new_expression->right = cast_expression(ctx);
                if (new_expression->right == ((void *)0))
                {
                    expression_delete(new_expression);
                    goto _catch_label_1;
                }
                new_expression->last_token = new_expression->right->last_token;
                if (op == 33)
                {
                    new_expression->expression_type = 28;
                    if ( !ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->right->object))
                    {
                        unsigned char   v = object_to_bool(&new_expression->right->object);
                        new_expression->object = object_make_signed_int( !v);
                    }
                    new_expression->type = type_make_int_bool_like();
                }
                else
                {
                    if (op == 126)
                    {
                        if ( !type_is_integer(&new_expression->right->type))
                        {
                            compiler_diagnostic_message(850, ctx, op_position, ((void *)0), "requires integer type");
                            expression_delete(new_expression);
                            goto _catch_label_1;
                        }
                        new_expression->expression_type = 29;
                        struct type  promoted = type_dup(&new_expression->right->type);
                        type_integer_promotion(&promoted);
                        new_expression->type = promoted;
                        if ( !ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->right->object))
                        {
                            int   vt = type_to_object_type(&new_expression->type);
                            switch (vt)
                            {
                                case 0 :
                                {
                                    signed int r = object_to_signed_int(&new_expression->right->object);
                                    new_expression->object = object_make_signed_int( ~r);
                                }
                                break;
                                case 1 :
                                {
                                    unsigned int r = object_to_unsigned_int(&new_expression->right->object);
                                    new_expression->object = object_make_unsigned_int( ~r);
                                }
                                break;
                                case 7 :
                                {
                                    signed long r = object_to_signed_long(&new_expression->right->object);
                                    new_expression->object = object_make_signed_long( ~r);
                                }
                                break;
                                case 8 :
                                {
                                    unsigned long r = object_to_unsigned_long(&new_expression->right->object);
                                    new_expression->object = object_make_unsigned_long( ~r);
                                }
                                break;
                                case 9 :
                                {
                                    signed long long r = object_to_signed_long_long(&new_expression->right->object);
                                    new_expression->object = object_make_signed_long_long( ~r);
                                }
                                break;
                                case 10 :
                                {
                                    unsigned long long r = object_to_unsigned_long_long(&new_expression->right->object);
                                    new_expression->object = object_make_unsigned_long_long( ~r);
                                }
                                break;
                                case 5 :
                                case 6 :
                                case 3 :
                                case 4 :
                                case 2 :
                                case 11 :
                                case 12 :
                                case 13 :
                                break;
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
                            if ( !ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->right->object))
                            {
                                int   vt = type_to_object_type(&new_expression->type);
                                switch (vt)
                                {
                                    case 0 :
                                    case 7 :
                                    {
                                        int a = object_to_signed_int(&new_expression->right->object);
                                        if (op == 45)
                                        {
                                            new_expression->object = object_make_signed_int( -a);
                                        }
                                        else
                                        {
                                            new_expression->object = object_make_signed_int( +a);
                                        }
                                    }
                                    break;
                                    case 1 :
                                    case 8 :
                                    {
                                        unsigned int a = object_to_unsigned_int(&new_expression->right->object);
                                        if (op == 45)
                                        {
                                            new_expression->object = object_make_unsigned_int( -a);
                                        }
                                        else
                                        {
                                            new_expression->object = object_make_unsigned_int( +a);
                                        }
                                    }
                                    break;
                                    case 9 :
                                    {
                                        long long a = object_to_signed_long_long(&new_expression->right->object);
                                        if (op == 45)
                                        {
                                            new_expression->object = object_make_signed_long_long( -a);
                                        }
                                        else
                                        {
                                            new_expression->object = object_make_signed_long_long( +a);
                                        }
                                    }
                                    break;
                                    case 10 :
                                    {
                                        unsigned long long a = object_to_unsigned_long_long(&new_expression->right->object);
                                        if (op == 45)
                                        {
                                            new_expression->object = object_make_unsigned_long_long( -a);
                                        }
                                        else
                                        {
                                            new_expression->object = object_make_unsigned_long_long( +a);
                                        }
                                    }
                                    break;
                                    case 2 :
                                    case 3 :
                                    case 4 :
                                    case 5 :
                                    case 6 :
                                    ;
                                    expression_delete(new_expression);
                                    goto _catch_label_1;
                                    break;
                                    case 11 :
                                    {
                                        float a = object_to_float(&new_expression->right->object);
                                        if (op == 45)
                                        {
                                            new_expression->object = object_make_float( -a);
                                        }
                                        else
                                        {
                                            new_expression->object = object_make_float( +a);
                                        }
                                    }
                                    break;
                                    case 12 :
                                    {
                                        double a = object_to_double(&new_expression->right->object);
                                        if (op == 45)
                                        {
                                            new_expression->object = object_make_double( -a);
                                        }
                                        else
                                        {
                                            new_expression->object = object_make_double( +a);
                                        }
                                    }
                                    break;
                                    case 13 :
                                    {
                                        long double a = object_to_long_double(&new_expression->right->object);
                                        if (op == 45)
                                        {
                                            new_expression->object = object_make_long_double( -a);
                                        }
                                        else
                                        {
                                            new_expression->object = object_make_long_double( +a);
                                        }
                                    }
                                    break;
                                }
                                ;
                            }
                        }
                        else
                        {
                            if (op == 42)
                            {
                                new_expression->expression_type = 32;
                                if ( !type_is_pointer_or_array(&new_expression->right->type))
                                {
                                    compiler_diagnostic_message(780, ctx, op_position, ((void *)0), "indirection requires pointer operand");
                                }
                                if (type_is_pointer(&new_expression->right->type))
                                {
                                    new_expression->type = type_remove_pointer(&new_expression->right->type);
                                }
                                else
                                {
                                    compiler_diagnostic_message(19, ctx, op_position, ((void *)0), "array indirection");
                                    new_expression->type = get_array_item_type(&new_expression->right->type);
                                }
                            }
                            else
                            {
                                if (op == 38)
                                {
                                    new_expression->expression_type = 33;
                                    if ( !expression_is_lvalue(new_expression->right))
                                    {
                                        compiler_diagnostic_message(1220, ctx, new_expression->right->first_token, ((void *)0), "lvalue required as unary '&' operand");
                                    }
                                    if (new_expression->right->type.storage_class_specifier_flags & 32)
                                    {
                                        char * variable_name = "?";
                                        if (new_expression->right->declarator && new_expression->right->declarator->name_opt)
                                        {
                                            variable_name = new_expression->right->declarator->name_opt->lexeme;
                                        }
                                        compiler_diagnostic_message(1220, ctx, new_expression->right->first_token, ((void *)0), "address of register variable 'x' requested", variable_name);
                                    }
                                    new_expression->type = type_add_pointer(&new_expression->right->type, ctx->options.null_checks_enabled);
                                    new_expression->type.address_of = 1;
                                }
                                else
                                {
                                    expression_delete(new_expression);
                                    compiler_diagnostic_message(790, ctx, ctx->current, ((void *)0), "invalid token");
                                    goto _catch_label_1;
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
                    unsigned char   disable_evaluation_copy = ctx->evaluation_is_disabled;
                    ctx->evaluation_is_disabled = 1;
                    parser_match(ctx);
                    if (ctx->current == ((void *)0))
                    {
                        unexpected_end_of_file(ctx);
                        goto _catch_label_1;
                    }
                    struct expression * new_expression = calloc(1, sizeof  *new_expression);
                    if (new_expression == ((void *)0))
                    {
                        goto _catch_label_1;
                    }
                    new_expression->first_token = ctx->current;
                    if (first_of_type_name_ahead(ctx))
                    {
                        new_expression->expression_type = 19;
                        if (parser_match_tk(ctx, 40) != 0)
                        {
                            expression_delete(new_expression);
                            new_expression = ((void *)0);
                            goto _catch_label_1;
                        }
                        new_expression->type_name = type_name(ctx);
                        if (new_expression->type_name == ((void *)0))
                        {
                            expression_delete(new_expression);
                            new_expression = ((void *)0);
                            goto _catch_label_1;
                        }
                        new_expression->type = make_size_t_type();
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            expression_delete(new_expression);
                            goto _catch_label_1;
                        }
                        new_expression->last_token = ctx->current;
                        if (parser_match_tk(ctx, 41) != 0)
                        {
                            expression_delete(new_expression);
                            goto _catch_label_1;
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
                        if (new_expression->right == ((void *)0))
                        {
                            ctx->evaluation_is_disabled = disable_evaluation_copy;
                            expression_delete(new_expression);
                            goto _catch_label_1;
                        }
                        new_expression->expression_type = 18;
                        if (check_sizeof_argument(ctx, new_expression->right, &new_expression->right->type) != 0)
                        {
                            expression_delete(new_expression);
                            goto _catch_label_1;
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
                        struct expression * new_expression = calloc(1, sizeof  *new_expression);
                        if (new_expression == ((void *)0))
                        {
                            goto _catch_label_1;
                        }
                        new_expression->first_token = ctx->current;
                        parser_match(ctx);
                        if (ctx->current == ((void *)0))
                        {
                            unexpected_end_of_file(ctx);
                            expression_delete(new_expression);
                            goto _catch_label_1;
                        }
                        new_expression->expression_type = 20;
                        if (first_of_type_name_ahead(ctx))
                        {
                            if (parser_match_tk(ctx, 40) != 0)
                            {
                                expression_delete(new_expression);
                                goto _catch_label_1;
                            }
                            new_expression->type_name = type_name(ctx);
                            if (new_expression->type_name == ((void *)0))
                            {
                                expression_delete(new_expression);
                                goto _catch_label_1;
                            }
                            new_expression->type = make_size_t_type();
                            if (ctx->current == ((void *)0))
                            {
                                unexpected_end_of_file(ctx);
                                expression_delete(new_expression);
                                goto _catch_label_1;
                            }
                            new_expression->last_token = ctx->current;
                            if (parser_match_tk(ctx, 41) != 0)
                            {
                                expression_delete(new_expression);
                                goto _catch_label_1;
                            }
                            if ( !type_is_array(&new_expression->type_name->abstract_declarator->type))
                            {
                                compiler_diagnostic_message(1390, ctx, new_expression->type_name->first_token, ((void *)0), "argument of _Lengthof must be an array");
                                expression_delete(new_expression);
                                goto _catch_label_1;
                            }
                            int nelements = new_expression->type_name->abstract_declarator->type.num_of_elements;
                            if (nelements > 0)
                            {
                                new_expression->object = object_make_size_t(nelements);
                            }
                        }
                        else
                        {
                            if (parser_match_tk(ctx, 40) != 0)
                            {
                                expression_delete(new_expression);
                                new_expression = ((void *)0);
                                goto _catch_label_1;
                            }
                            unsigned char   disable_evaluation_copy = ctx->evaluation_is_disabled;
                            ctx->evaluation_is_disabled = 1;
                            new_expression->right = unary_expression(ctx);
                            ctx->evaluation_is_disabled = disable_evaluation_copy;
                            if (new_expression->right == ((void *)0))
                            {
                                expression_delete(new_expression);
                                goto _catch_label_1;
                            }
                            if (ctx->current == ((void *)0))
                            {
                                unexpected_end_of_file(ctx);
                                expression_delete(new_expression);
                                goto _catch_label_1;
                            }
                            new_expression->last_token = ctx->current;
                            if (parser_match_tk(ctx, 41) != 0)
                            {
                                expression_delete(new_expression);
                                goto _catch_label_1;
                            }
                            if ( !type_is_array(&new_expression->right->type))
                            {
                                compiler_diagnostic_message(1390, ctx, new_expression->right->first_token, ((void *)0), "argument of _Lengthof must be an array");
                                expression_delete(new_expression);
                                goto _catch_label_1;
                            }
                            int nelements = new_expression->right->type.num_of_elements;
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
                        if (ctx->current->type == 9056)
                        {
                            struct expression * new_expression = calloc(1, sizeof  *new_expression);
                            if (new_expression == ((void *)0))
                            {
                                goto _catch_label_1;
                            }
                            new_expression->expression_type = 25;
                            new_expression->first_token = ctx->current;
                            parser_match(ctx);
                            if (ctx->current == ((void *)0) || parser_match_tk(ctx, 40) != 0)
                            {
                                expression_delete(new_expression);
                                new_expression = ((void *)0);
                                goto _catch_label_1;
                            }
                            new_expression->right = expression(ctx);
                            if (parser_match_tk(ctx, 41) != 0)
                            {
                                expression_delete(new_expression);
                                new_expression = ((void *)0);
                                goto _catch_label_1;
                            }
                            return new_expression;
                        }
                        else
                        {
                            if (ctx->current->type == 9044)
                            {
                                struct expression * new_expression = calloc(1, sizeof  *new_expression);
                                if (new_expression == ((void *)0))
                                {
                                    goto _catch_label_1;
                                }
                                new_expression->expression_type = 24;
                                new_expression->first_token = ctx->current;
                                parser_match(ctx);
                                if (ctx->current == ((void *)0) || parser_match_tk(ctx, 40) != 0)
                                {
                                    expression_delete(new_expression);
                                    new_expression = ((void *)0);
                                    goto _catch_label_1;
                                }
                                new_expression->type_name = type_name(ctx);
                                if (parser_match_tk(ctx, 41) != 0)
                                {
                                    expression_delete(new_expression);
                                    new_expression = ((void *)0);
                                    goto _catch_label_1;
                                }
                                if ( !ctx->evaluation_is_disabled)
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
                                if (ctx->current->type == 9074 || ctx->current->type == 9076 || ctx->current->type == 9075 || ctx->current->type == 9073 || ctx->current->type == 9077 || ctx->current->type == 9078 || ctx->current->type == 9080 || ctx->current->type == 9079 || ctx->current->type == 9081 || ctx->current->type == 9082)
                                {
                                    unsigned char   disable_evaluation_copy = ctx->evaluation_is_disabled;
                                    ctx->evaluation_is_disabled = 1;
                                    struct token * traits_token = ctx->current;
                                    struct expression * new_expression = calloc(1, sizeof  *new_expression);
                                    if (new_expression == ((void *)0))
                                    {
                                        goto _catch_label_1;
                                    }
                                    new_expression->first_token = ctx->current;
                                    new_expression->expression_type = 21;
                                    parser_match(ctx);
                                    if (ctx->current == ((void *)0))
                                    {
                                        unexpected_end_of_file(ctx);
                                        expression_delete(new_expression);
                                        new_expression = ((void *)0);
                                        goto _catch_label_1;
                                    }
                                    struct type * p_type = ((void *)0);
                                    if (first_of_type_name_ahead(ctx))
                                    {
                                        if (parser_match_tk(ctx, 40) != 0)
                                        {
                                            expression_delete(new_expression);
                                            new_expression = ((void *)0);
                                            goto _catch_label_1;
                                        }
                                        new_expression->type_name = type_name(ctx);
                                        if (new_expression->type_name == ((void *)0))
                                        {
                                            expression_delete(new_expression);
                                            new_expression = ((void *)0);
                                            goto _catch_label_1;
                                        }
                                        if (ctx->current == ((void *)0))
                                        {
                                            unexpected_end_of_file(ctx);
                                            expression_delete(new_expression);
                                            new_expression = ((void *)0);
                                            goto _catch_label_1;
                                        }
                                        new_expression->last_token = ctx->current;
                                        if (parser_match_tk(ctx, 41) != 0)
                                        {
                                            expression_delete(new_expression);
                                            new_expression = ((void *)0);
                                            goto _catch_label_1;
                                        }
                                        p_type = &new_expression->type_name->abstract_declarator->type;
                                    }
                                    else
                                    {
                                        new_expression->right = unary_expression(ctx);
                                        if (new_expression->right == ((void *)0))
                                        {
                                            ctx->evaluation_is_disabled = disable_evaluation_copy;
                                            expression_delete(new_expression);
                                            goto _catch_label_1;
                                        }
                                        p_type = &new_expression->right->type;
                                        if (ctx->previous == ((void *)0))
                                        {
                                            expression_delete(new_expression);
                                            new_expression = ((void *)0);
                                            goto _catch_label_1;
                                        }
                                        new_expression->last_token = ctx->previous;
                                    }
                                    switch (traits_token->type)
                                    {
                                        case 9074 :
                                        ;
                                        new_expression->object = object_make_signed_int(expression_is_lvalue(new_expression->right));
                                        break;
                                        case 9075 :
                                        new_expression->object = object_make_signed_int(type_is_const(p_type));
                                        break;
                                        case 9076 :
                                        new_expression->object = object_make_signed_int(type_is_owner(p_type));
                                        break;
                                        case 9073 :
                                        new_expression->object = object_make_signed_int(type_is_pointer(p_type));
                                        break;
                                        case 9078 :
                                        new_expression->object = object_make_signed_int(type_is_function(p_type));
                                        break;
                                        case 9077 :
                                        new_expression->object = object_make_signed_int(type_is_array(p_type));
                                        break;
                                        case 9080 :
                                        new_expression->object = object_make_signed_int(type_is_arithmetic(p_type));
                                        break;
                                        case 9079 :
                                        new_expression->object = object_make_signed_int(type_is_scalar(p_type));
                                        break;
                                        case 9081 :
                                        new_expression->object = object_make_signed_int(type_is_floating_point(p_type));
                                        break;
                                        case 9082 :
                                        new_expression->object = object_make_signed_int(type_is_integer(p_type));
                                        break;
                                        default:
                                        ;
                                    }
                                    new_expression->type = type_make_int_bool_like();
                                    p_expression_node = new_expression;
                                    ctx->evaluation_is_disabled = disable_evaluation_copy;
                                }
                                else
                                {
                                    p_expression_node = postfix_expression(ctx);
                                    if (p_expression_node == ((void *)0))
                                    {
                                        goto _catch_label_1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

struct object object_cast(int   e, struct object * a);

struct expression *cast_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        if (first_of_type_name_ahead(ctx))
        {
            p_expression_node = calloc(1, sizeof  *p_expression_node);
            if (p_expression_node == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_expression_node->first_token = ctx->current;
            p_expression_node->expression_type = 34;
            if (parser_match_tk(ctx, 40) != 0)
            {
                goto _catch_label_1;
            }
            p_expression_node->type_name = type_name(ctx);
            if (p_expression_node->type_name == ((void *)0))
            {
                expression_delete(p_expression_node);
                p_expression_node = ((void *)0);
                goto _catch_label_1;
            }
            p_expression_node->type = type_dup(&p_expression_node->type_name->type);
            if (parser_match_tk(ctx, 41) != 0)
            {
                goto _catch_label_1;
            }
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            if (ctx->current->type == 123)
            {
                struct expression * new_expression = postfix_expression_type_name(ctx, p_expression_node->type_name);
                if (new_expression == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_expression_node->type_name = ((void *)0);
                expression_delete(p_expression_node);
                p_expression_node = new_expression;
            }
            else
            {
                if (is_first_of_unary_expression(ctx))
                {
                    p_expression_node->left = cast_expression(ctx);
                    if (p_expression_node->left == ((void *)0))
                    {
                        expression_delete(p_expression_node);
                        p_expression_node = ((void *)0);
                        goto _catch_label_1;
                    }
                    if (p_expression_node->left->type.storage_class_specifier_flags & 8192 && type_is_owner(&p_expression_node->left->type))
                    {
                        if ( !type_is_owner(&p_expression_node->type))
                        {
                            if (type_is_pointer(&p_expression_node->left->type))
                            {
                                compiler_diagnostic_message(25, ctx, p_expression_node->first_token, ((void *)0), "discarding _Owner pointer");
                            }
                            else
                            {
                                compiler_diagnostic_message(25, ctx, p_expression_node->first_token, ((void *)0), "discarding _Owner");
                            }
                        }
                    }
                    type_destroy(&p_expression_node->type);
                    p_expression_node->type = make_type_using_declarator(ctx, p_expression_node->type_name->abstract_declarator);
                    if ( !ctx->evaluation_is_disabled && object_has_constant_value(&p_expression_node->left->object))
                    {
                        int   vt = type_to_object_type(&p_expression_node->type);
                        p_expression_node->object = object_cast(vt, &p_expression_node->left->object);
                    }
                    p_expression_node->type.storage_class_specifier_flags = p_expression_node->left->type.storage_class_specifier_flags;
                }
                else
                {
                    compiler_diagnostic_message(650, ctx, ctx->current, ((void *)0), "expected expression");
                }
            }
        }
        else
        {
            if (is_first_of_unary_expression(ctx))
            {
                p_expression_node = unary_expression(ctx);
                if (p_expression_node == ((void *)0))
                {
                    goto _catch_label_1;
                }
            }
            else
            {
                compiler_diagnostic_message(650, ctx, ctx->current, ((void *)0), "expected expression");
                ;
                goto _catch_label_1;
            }
        }
        if (ctx->current == ((void *)0) || ctx->previous == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_expression_node->last_token = ctx->previous;
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
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
    struct type  common_type = {0};
    if (1)
    {
        if (new_expression->left == ((void *)0) || new_expression->right == ((void *)0))
        {
            ;
            goto _catch_label_1;
        }
        struct object  value = {0};
        switch (op)
        {
            case 43 :
            case 45 :
            case 42 :
            case 47 :
            case 37 :
            case 62 :
            case 60 :
            case 15933 :
            case 15421 :
            case 15677 :
            case 8509 :
            break;
            default:
            ;
            goto _catch_label_1;
        }
        if ( !type_is_arithmetic(&new_expression->left->type))
        {
            compiler_diagnostic_message(840, ctx, ctx->current, ((void *)0), "left type must be an arithmetic type");
            goto _catch_label_1;
        }
        if ( !type_is_arithmetic(&new_expression->right->type))
        {
            compiler_diagnostic_message(840, ctx, ctx->current, ((void *)0), "right type must be an arithmetic type");
            goto _catch_label_1;
        }
        if ( !ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
        {
            struct marker  m = {0, 0, 0, 0, 0, 0, 0};
            m.p_token_begin = new_expression->left->first_token;
            m.p_token_end = new_expression->right->last_token;
            common_type = type_common(&new_expression->left->type, &new_expression->right->type);
            int   vt = type_to_object_type(&common_type);
            switch (vt)
            {
                case 0 :
                case 7 :
                {
                    int a = object_to_signed_int(&new_expression->left->object);
                    int b = object_to_signed_int(&new_expression->right->object);
                    if (op == 43)
                    {
                        int computed_result = a + b;
                        signed long long exact_result;
                        if (signed_long_long_add(&exact_result, a, b))
                        {
                            if (computed_result != exact_result)
                            {
                                compiler_diagnostic_message(50, ctx, ((void *)0), &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
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
                            int computed_result = a - b;
                            signed long long exact_result;
                            if (signed_long_long_sub(&exact_result, a, b))
                            {
                                if (computed_result != exact_result)
                                {
                                    compiler_diagnostic_message(50, ctx, ((void *)0), &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
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
                                int computed_result = a * b;
                                signed long long exact_result;
                                if (signed_long_long_mul(&exact_result, a, b))
                                {
                                    if (computed_result != exact_result)
                                    {
                                        compiler_diagnostic_message(50, ctx, ((void *)0), &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
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
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
                                    }
                                    else
                                    {
                                        value = object_make_signed_int(a / b);
                                    }
                                }
                                else
                                {
                                    if (op == 37)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
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
                break;
                case 1 :
                case 8 :
                {
                    unsigned int a = object_to_unsigned_int(&new_expression->left->object);
                    unsigned int b = object_to_unsigned_int(&new_expression->right->object);
                    if (op == 43)
                    {
                        unsigned int computed_result = a + b;
                        unsigned long long exact_result;
                        if (unsigned_long_long_add(&exact_result, a, b))
                        {
                            if (computed_result != exact_result)
                            {
                                compiler_diagnostic_message(50, ctx, ((void *)0), &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
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
                            unsigned int computed_result = a - b;
                            unsigned long long exact_result;
                            if (unsigned_long_long_sub(&exact_result, a, b))
                            {
                                if (computed_result != exact_result)
                                {
                                    compiler_diagnostic_message(50, ctx, ((void *)0), &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
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
                                unsigned int computed_result = a * b;
                                unsigned long long exact_result;
                                if (unsigned_long_long_mul(&exact_result, a, b))
                                {
                                    if (computed_result != exact_result)
                                    {
                                        compiler_diagnostic_message(50, ctx, ((void *)0), &m, "integer overflow results in '%d'. Exactly result is '%lld'.", computed_result, exact_result);
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
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
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
                                            compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
                                            goto _catch_label_1;
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
                break;
                case 9 :
                {
                    long long a = object_to_signed_long_long(&new_expression->left->object);
                    long long b = object_to_signed_long_long(&new_expression->right->object);
                    if (op == 43)
                    {
                        long long computed_result = a + b;
                        signed long long exact_result;
                        if ( !signed_long_long_add(&exact_result, a, b))
                        {
                            compiler_diagnostic_message(50, ctx, ((void *)0), &m, "integer overflow results in '%dll'. ", computed_result);
                        }
                        value = object_make_signed_long_long(computed_result);
                    }
                    else
                    {
                        if (op == 45)
                        {
                            long long computed_result = a - b;
                            signed long long exact_result;
                            if ( !signed_long_long_sub(&exact_result, a, b))
                            {
                                compiler_diagnostic_message(50, ctx, ((void *)0), &m, "integer overflow results in '%dll'.", computed_result);
                            }
                            value = object_make_signed_long_long(computed_result);
                        }
                        else
                        {
                            if (op == 42)
                            {
                                long long computed_result = a * b;
                                signed long long exact_result;
                                if ( !signed_long_long_mul(&exact_result, a, b))
                                {
                                    compiler_diagnostic_message(50, ctx, ((void *)0), &m, "integer overflow results in '%dll", computed_result);
                                }
                                value = object_make_signed_long_long(computed_result);
                            }
                            else
                            {
                                if (op == 47)
                                {
                                    if (b == 0)
                                    {
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
                                        goto _catch_label_1;
                                    }
                                    value = object_make_signed_long_long(a / b);
                                }
                                else
                                {
                                    if (op == 37)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
                                            goto _catch_label_1;
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
                break;
                case 10 :
                {
                    unsigned long long a = object_to_unsigned_long(&new_expression->left->object);
                    unsigned long long b = object_to_unsigned_long(&new_expression->right->object);
                    if (op == 43)
                    {
                        value = object_make_unsigned_long_long(a + b);
                    }
                    else
                    {
                        if (op == 45)
                        {
                            value = object_make_unsigned_long_long(a - b);
                        }
                        else
                        {
                            if (op == 42)
                            {
                                value = object_make_unsigned_long_long(a * b);
                            }
                            else
                            {
                                if (op == 47)
                                {
                                    if (b == 0)
                                    {
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
                                        goto _catch_label_1;
                                    }
                                    value = object_make_unsigned_long_long(a / b);
                                }
                                else
                                {
                                    if (op == 37)
                                    {
                                        if (b == 0)
                                        {
                                            compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
                                            goto _catch_label_1;
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
                break;
                case 2 :
                case 3 :
                case 4 :
                case 5 :
                case 6 :
                ;
                goto _catch_label_1;
                break;
                case 11 :
                {
                    float a = object_to_float(&new_expression->left->object);
                    float b = object_to_float(&new_expression->right->object);
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
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
                                        goto _catch_label_1;
                                    }
                                    value = object_make_float(a / b);
                                }
                                else
                                {
                                    if (op == 37)
                                    {
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "'%': not valid as left operand has type 'float'");
                                        goto _catch_label_1;
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
                break;
                case 12 :
                {
                    double a = object_to_double(&new_expression->left->object);
                    double b = object_to_double(&new_expression->right->object);
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
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
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
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "'%': not valid as left operand has type 'float'");
                                        goto _catch_label_1;
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
                break;
                case 13 :
                {
                    long double a = object_to_long_double(&new_expression->left->object);
                    long double b = object_to_long_double(&new_expression->right->object);
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
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "division by zero");
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
                                        compiler_diagnostic_message(36, ctx, new_expression->right->first_token, ((void *)0), "'%': not valid as left operand has type 'float'");
                                        goto _catch_label_1;
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
                break;
            }
            ;
        }
        type_destroy(&common_type);
         *result = value;
        return 0;
    }
    else _catch_label_1:
    {
    }
    type_destroy(&common_type);
    struct object  empty = {0};
     *result = empty;
    return 1;
}

struct expression *multiplicative_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        p_expression_node = cast_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 42 || ctx->current->type == 47 || ctx->current->type == 37))
        {
            struct expression * new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                expression_delete(p_expression_node);
                p_expression_node = ((void *)0);
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            int   op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            switch (op)
            {
                case 42 :
                new_expression->expression_type = 35;
                break;
                case 47 :
                new_expression->expression_type = 36;
                break;
                case 37 :
                new_expression->expression_type = 37;
                break;
                default:
                ;
                break;
            }
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            new_expression->right = cast_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            new_expression->last_token = new_expression->right->last_token;
            new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type);
            if (execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
            {
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            p_expression_node = new_expression;
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

struct expression *additive_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        p_expression_node = multiplicative_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 43 || ctx->current->type == 45))
        {
            struct token * operator_position = ctx->current;
            struct expression * new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                compiler_diagnostic_message(1260, ctx, ctx->current, ((void *)0), "_Out of mem");
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            int   op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = ((void *)0);
                goto _catch_label_1;
            }
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            new_expression->right = multiplicative_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                expression_delete(new_expression);
                new_expression = ((void *)0);
                goto _catch_label_1;
            }
            new_expression->last_token = new_expression->right->last_token;
            if ( !type_is_scalar(&new_expression->left->type) &&  !type_is_array(&new_expression->left->type))
            {
                compiler_diagnostic_message(870, ctx, operator_position, ((void *)0), "left operator is not scalar");
            }
            if ( !type_is_scalar(&new_expression->right->type) &&  !type_is_array(&new_expression->right->type))
            {
                compiler_diagnostic_message(880, ctx, operator_position, ((void *)0), "right operator is not scalar");
            }
            unsigned char   b_left_is_arithmetic = type_is_arithmetic(&new_expression->left->type);
            unsigned char   b_right_is_arithmetic = type_is_arithmetic(&new_expression->right->type);
            int   left_category = type_get_category(&new_expression->left->type);
            int   right_category = type_get_category(&new_expression->right->type);
            if (op == 43)
            {
                new_expression->expression_type = 38;
                if (b_left_is_arithmetic && b_right_is_arithmetic)
                {
                    new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type);
                    if (execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
                    {
                        expression_delete(new_expression);
                        goto _catch_label_1;
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
                                struct type  t = get_array_item_type(&new_expression->left->type);
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
                            compiler_diagnostic_message(850, ctx, ctx->current, ((void *)0), "expected integer type on right");
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
                                compiler_diagnostic_message(840, ctx, ctx->current, ((void *)0), "expected integer type on left");
                            }
                        }
                        else
                        {
                            compiler_diagnostic_message(860, ctx, ctx->current, ((void *)0), "invalid types additive expression");
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
                        if (execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
                        {
                            expression_delete(new_expression);
                            goto _catch_label_1;
                        }
                    }
                    else
                    {
                        if (left_category == 3 || left_category == 2)
                        {
                            if (right_category == 3 || right_category == 2)
                            {
                                struct type  t1 = type_lvalue_conversion(&new_expression->left->type, ctx->options.null_checks_enabled);
                                struct type  t2 = type_lvalue_conversion(&new_expression->right->type, ctx->options.null_checks_enabled);
                                if ( !type_is_same(&t1, &t2, 0))
                                {
                                    compiler_diagnostic_message(890, ctx, ctx->current, ((void *)0), "incompatible pointer types");
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
                                    compiler_diagnostic_message(850, ctx, ctx->current, ((void *)0), "right must be integer type");
                                }
                            }
                        }
                        else
                        {
                            compiler_diagnostic_message(860, ctx, ctx->current, ((void *)0), "invalid types for operator -");
                        }
                    }
                }
            }
            p_expression_node = new_expression;
            new_expression = ((void *)0);
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

static int execute_bitwise_operator(struct parser_ctx * ctx, struct expression * new_expression, int op);

struct expression *shift_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        p_expression_node = additive_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 15934 || ctx->current->type == 15420))
        {
            struct expression * new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            int   op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = ((void *)0);
                goto _catch_label_1;
            }
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            new_expression->right = multiplicative_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                expression_delete(new_expression);
                goto _catch_label_1;
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
                goto _catch_label_1;
            }
            p_expression_node = new_expression;
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

void check_comparison(struct parser_ctx * ctx, struct expression * p_a_expression, struct expression * p_b_expression, struct token * op_token);

struct expression *relational_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    struct expression * new_expression = ((void *)0);
    if (1)
    {
        p_expression_node = shift_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 62 || ctx->current->type == 60 || ctx->current->type == 15933 || ctx->current->type == 15421))
        {
            ;
            new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            int   op = ctx->current->type;
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            new_expression->right = shift_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                expression_delete(new_expression);
                new_expression = ((void *)0);
                goto _catch_label_1;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                expression_delete(new_expression);
                new_expression = ((void *)0);
                goto _catch_label_1;
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
                if (execute_arithmetic(ctx, new_expression, op, &new_expression->object) != 0)
                {
                    expression_delete(new_expression);
                    new_expression = ((void *)0);
                    goto _catch_label_1;
                }
            }
            type_destroy(&new_expression->type);
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
            new_expression = ((void *)0);
        }
    }
    else _catch_label_1:
    {
        expression_delete(new_expression);
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
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
            ;
            ;
            char * lefttag = "";
            if (left->type.enum_specifier->tag_token)
            {
                lefttag = left->type.enum_specifier->tag_token->lexeme;
            }
            char * righttag = "";
            if (right->type.enum_specifier->tag_token)
            {
                righttag = right->type.enum_specifier->tag_token->lexeme;
            }
            char finalmessage[200] = {0};
            snprintf(finalmessage, sizeof finalmessage, "%s (enum %s, enum %s)", message, lefttag, righttag);
            compiler_diagnostic_message(3, ctx, operator_token, ((void *)0), finalmessage, lefttag, righttag);
        }
    }
}

void expression_evaluate_equal_not_equal(struct expression * left, struct expression * right, struct expression * result, int op, unsigned char   disabled)
{
    ;
}

struct expression *equality_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    struct expression * new_expression = ((void *)0);
    if (1)
    {
        p_expression_node = relational_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 15677 || ctx->current->type == 8509))
        {
            ;
            new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            struct token * operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
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
            p_expression_node = ((void *)0);
            new_expression->right = relational_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                goto _catch_label_1;
            }
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            check_comparison(ctx, new_expression->left, new_expression->right, ctx->current);
            new_expression->last_token = new_expression->right->last_token;
            new_expression->first_token = operator_token;
            if (type_is_arithmetic(&new_expression->left->type) && type_is_arithmetic(&new_expression->right->type))
            {
                if (execute_arithmetic(ctx, new_expression, operator_token->type, &new_expression->object) != 0)
                {
                    goto _catch_label_1;
                }
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
            new_expression = ((void *)0);
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    expression_delete(new_expression);
    return p_expression_node;
}

struct expression *and_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    struct expression * new_expression = ((void *)0);
    if (1)
    {
        p_expression_node = equality_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && ctx->current->type == 38)
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            ;
            new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 48;
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            new_expression->right = equality_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (execute_bitwise_operator(ctx, new_expression, 38) != 0)
            {
                goto _catch_label_1;
            }
            p_expression_node = new_expression;
            new_expression = ((void *)0);
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    expression_delete(new_expression);
    return p_expression_node;
}

struct expression *exclusive_or_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    struct expression * new_expression = ((void *)0);
    if (1)
    {
        p_expression_node = and_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 94))
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            ;
            new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 49;
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            new_expression->right = and_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->last_token = new_expression->right->last_token;
            if (execute_bitwise_operator(ctx, new_expression, 94) != 0)
            {
                goto _catch_label_1;
            }
            p_expression_node = new_expression;
            new_expression = ((void *)0);
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    expression_delete(new_expression);
    return p_expression_node;
}

static int execute_bitwise_operator(struct parser_ctx * ctx, struct expression * new_expression, int op)
{
    if (1)
    {
        switch (op)
        {
            case 38 :
            case 94 :
            case 124 :
            case 15934 :
            case 15420 :
            break;
            default:
            ;
            goto _catch_label_1;
        }
        if ( !type_is_integer(&new_expression->left->type))
        {
            compiler_diagnostic_message(840, ctx, ctx->current, ((void *)0), "left type must be an integer type");
            goto _catch_label_1;
        }
        if ( !type_is_integer(&new_expression->right->type))
        {
            compiler_diagnostic_message(840, ctx, ctx->current, ((void *)0), "right type must be an integer type");
            goto _catch_label_1;
        }
        type_destroy(&new_expression->type);
        new_expression->type = type_common(&new_expression->left->type, &new_expression->right->type);
        if ( !ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
        {
            int   vt = type_to_object_type(&new_expression->type);
            switch (vt)
            {
                case 0 :
                case 7 :
                {
                    int a = object_to_signed_int(&new_expression->left->object);
                    int b = object_to_signed_int(&new_expression->right->object);
                    int r = 0;
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
                    new_expression->object = object_make_signed_int(r);
                }
                break;
                case 1 :
                case 8 :
                {
                    unsigned int a = object_to_unsigned_int(&new_expression->left->object);
                    unsigned int b = object_to_unsigned_int(&new_expression->right->object);
                    int r = 0;
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
                    new_expression->object = object_make_unsigned_int(r);
                }
                break;
                case 9 :
                {
                    long long a = object_to_signed_long(&new_expression->left->object);
                    long long b = object_to_signed_long(&new_expression->right->object);
                    long long r = 0;
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
                    new_expression->object = object_make_signed_long_long(r);
                }
                break;
                case 10 :
                {
                    unsigned long long a = object_to_unsigned_long(&new_expression->left->object);
                    unsigned long long b = object_to_unsigned_long(&new_expression->right->object);
                    unsigned long long r = 0;
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
                    new_expression->object = object_make_unsigned_long_long(r);
                }
                break;
                case 2 :
                case 3 :
                case 4 :
                case 5 :
                case 6 :
                ;
                goto _catch_label_1;
                break;
                case 11 :
                case 12 :
                case 13 :
                ;
                goto _catch_label_1;
                break;
            }
            ;
        }
        return 0;
    }
    else _catch_label_1:
    {
    }
    return 1;
}

struct expression *inclusive_or_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        p_expression_node = exclusive_or_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 124))
        {
            struct token * operator_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            struct expression * new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 50;
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            new_expression->right = exclusive_or_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            check_diferent_enuns(ctx, operator_token, new_expression->left, new_expression->right, "operator '|' between enumerations of different types.");
            new_expression->last_token = new_expression->right->last_token;
            if (execute_bitwise_operator(ctx, new_expression, 124) != 0)
            {
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            p_expression_node = new_expression;
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

struct expression *logical_and_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        p_expression_node = inclusive_or_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 9766))
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            struct expression * new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 52;
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            new_expression->right = inclusive_or_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            new_expression->last_token = new_expression->right->last_token;
            if ( !ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
            {
                unsigned char   a = object_to_bool(&new_expression->left->object);
                unsigned char   b = object_to_bool(&new_expression->right->object);
                new_expression->object = object_make_signed_int(a && b);
            }
            if ( !type_is_scalar(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(870, ctx, ctx->current, ((void *)0), "left type is not scalar for or expression");
                goto _catch_label_1;
            }
            if ( !type_is_scalar(&new_expression->right->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(880, ctx, ctx->current, ((void *)0), "right type is not scalar for or expression");
                goto _catch_label_1;
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

struct expression *logical_or_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        p_expression_node = logical_and_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        while (ctx->current != ((void *)0) && (ctx->current->type == 31868))
        {
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            struct expression * new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 51;
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            new_expression->right = logical_and_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            new_expression->last_token = new_expression->right->last_token;
            if ( !ctx->evaluation_is_disabled && object_has_constant_value(&new_expression->left->object) && object_has_constant_value(&new_expression->right->object))
            {
                unsigned char   a = object_to_bool(&new_expression->left->object);
                unsigned char   b = object_to_bool(&new_expression->right->object);
                new_expression->object = object_make_signed_int(a || b);
            }
            if ( !type_is_scalar(&new_expression->left->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(870, ctx, ctx->current, ((void *)0), "left type is not scalar for or expression");
                goto _catch_label_1;
            }
            if ( !type_is_scalar(&new_expression->right->type))
            {
                expression_delete(new_expression);
                compiler_diagnostic_message(880, ctx, ctx->current, ((void *)0), "right type is not scalar for or expression");
                goto _catch_label_1;
            }
            new_expression->type = type_make_int_bool_like();
            p_expression_node = new_expression;
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

struct expression *conditional_expression(struct parser_ctx * ctx);

struct expression *assignment_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        p_expression_node = conditional_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        ;
        while (ctx->current != ((void *)0) && (ctx->current->type == 61 || ctx->current->type == 10813 || ctx->current->type == 12093 || ctx->current->type == 9533 || ctx->current->type == 11069 || ctx->current->type == 11581 || ctx->current->type == 15421 || ctx->current->type == 15933 || ctx->current->type == 9789 || ctx->current->type == 24125 || ctx->current->type == 31805))
        {
            struct token * op_token = ctx->current;
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                goto _catch_label_1;
            }
            struct expression * new_expression = calloc(1, sizeof  *new_expression);
            if (new_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            new_expression->first_token = ctx->current;
            new_expression->expression_type = 53;
            new_expression->left = p_expression_node;
            p_expression_node = ((void *)0);
            struct marker  left_operand_marker = {0, 0, 0, 0, 0, 0, 0};
            left_operand_marker.p_token_begin = new_expression->left->first_token;
            left_operand_marker.p_token_end = new_expression->left->last_token;
            if (type_is_function(&new_expression->left->type))
            {
                compiler_diagnostic_message(900, ctx, ((void *)0), &left_operand_marker, "assignment of function");
            }
            else
            {
                if (type_is_array(&new_expression->left->type))
                {
                    if (new_expression->left->type.storage_class_specifier_flags & 2048)
                    {
                        compiler_diagnostic_message(42, ctx, ((void *)0), &left_operand_marker, "assignment to array parameter");
                    }
                    else
                    {
                        compiler_diagnostic_message(910, ctx, ((void *)0), &left_operand_marker, "assignment to expression with array type");
                    }
                }
            }
            if (type_is_const(&new_expression->left->type))
            {
                compiler_diagnostic_message(920, ctx, ((void *)0), &left_operand_marker, "assignment of read-only object");
            }
            if ( !expression_is_lvalue(new_expression->left))
            {
                compiler_diagnostic_message(1230, ctx, ((void *)0), &left_operand_marker, "lvalue required as left operand of assignment");
            }
            new_expression->right = assignment_expression(ctx);
            if (new_expression->right == ((void *)0))
            {
                expression_delete(new_expression);
                goto _catch_label_1;
            }
            if (op_token->type == 61)
            {
                check_assigment(ctx, &new_expression->left->type, new_expression->right, 2);
            }
            new_expression->last_token = new_expression->right->last_token;
            new_expression->type = type_dup(&new_expression->left->type);
            new_expression->type.storage_class_specifier_flags =  ~8192;
            new_expression->type.storage_class_specifier_flags =  ~16384;
            check_diferent_enuns(ctx, op_token, new_expression->left, new_expression->right, "assignment of different enums.");
            new_expression->left->is_assignment_expression = 1;
            if (new_expression->left->left)
            {
                new_expression->left->left->is_assignment_expression = 1;
            }
            p_expression_node = new_expression;
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

void argument_expression_list_push(struct argument_expression_list * list, struct argument_expression * pitem)
{
    if (list->head == ((void *)0))
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
    struct argument_expression * item = p->head;
    while (item)
    {
        struct argument_expression * next = item->next;
        item->next = ((void *)0);
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
    struct expression * p_expression_node = ((void *)0);
    if (1)
    {
        if (ctx->current == ((void *)0))
        {
            unexpected_end_of_file(ctx);
            goto _catch_label_1;
        }
        p_expression_node = assignment_expression(ctx);
        if (p_expression_node == ((void *)0))
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
            while (ctx->current->type == 44)
            {
                parser_match(ctx);
                if (ctx->current == ((void *)0))
                {
                    unexpected_end_of_file(ctx);
                    goto _catch_label_1;
                }
                struct expression * p_expression_node_new = calloc(1, sizeof  *p_expression_node_new);
                if (p_expression_node_new == ((void *)0))
                {
                    goto _catch_label_1;
                }
                p_expression_node_new->first_token = ctx->current;
                p_expression_node_new->expression_type = 54;
                p_expression_node_new->left = p_expression_node;
                p_expression_node = ((void *)0);
                p_expression_node_new->right = expression(ctx);
                if (p_expression_node_new->right == ((void *)0))
                {
                    expression_delete(p_expression_node_new);
                    goto _catch_label_1;
                }
                p_expression_node_new->left->last_token = p_expression_node_new->right->last_token;
                p_expression_node = p_expression_node_new;
                if (ctx->current == ((void *)0))
                {
                    unexpected_end_of_file(ctx);
                    goto _catch_label_1;
                }
            }
            type_destroy(&p_expression_node->type);
            p_expression_node->type = type_dup(&p_expression_node->right->type);
        }
    }
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    return p_expression_node;
}

unsigned char  is_first_of_conditional_expression(struct parser_ctx * ctx)
{
    return is_first_of_unary_expression(ctx) || is_first_of_primary_expression(ctx);
}

unsigned char  expression_is_one(struct expression * expression)
{
    if (expression->expression_type == 8)
    {
        return (object_has_constant_value(&expression->object) && object_to_signed_int(&expression->object) == 1);
    }
    return 0;
}

unsigned char  expression_is_zero(struct expression * expression)
{
    if (expression->expression_type == 8)
    {
        return (object_has_constant_value(&expression->object) && object_to_signed_int(&expression->object) == 0);
    }
    return 0;
}

unsigned char  type_is_nullptr_t(struct type * p_type);

unsigned char  expression_is_null_pointer_constant(struct expression * expression)
{
    if (type_is_nullptr_t(&expression->type) || (object_has_constant_value(&expression->object) && object_to_signed_int(&expression->object) == 0))
    {
        return 1;
    }
    return 0;
}

void type_swap(struct type * a, struct type * b);
unsigned char  type_is_void(struct type * p_type);
unsigned char  type_is_void_ptr(struct type * p_type);

struct expression *conditional_expression(struct parser_ctx * ctx)
{
    struct expression * p_expression_node = ((void *)0);
    struct type  left_type = {0};
    struct type  right_type = {0};
    if (1)
    {
        p_expression_node = logical_or_expression(ctx);
        if (p_expression_node == ((void *)0))
        {
            goto _catch_label_1;
        }
        if (ctx->current && ctx->current->type == 63)
        {
            struct expression * p_conditional_expression = calloc(1, sizeof  *p_conditional_expression);
            if (p_conditional_expression == ((void *)0))
            {
                goto _catch_label_1;
            }
            p_conditional_expression->first_token = ctx->current;
            p_conditional_expression->expression_type = 55;
            p_conditional_expression->condition_expr = p_expression_node;
            p_expression_node = ((void *)0);
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                goto _catch_label_1;
            }
            struct expression * p_left = expression(ctx);
            if (p_left == ((void *)0))
            {
                expression_delete(p_conditional_expression);
                goto _catch_label_1;
            }
            p_conditional_expression->left = p_left;
            parser_match(ctx);
            if (ctx->current == ((void *)0))
            {
                unexpected_end_of_file(ctx);
                expression_delete(p_conditional_expression);
                goto _catch_label_1;
            }
            struct expression * p_right = conditional_expression(ctx);
            if (p_right == ((void *)0))
            {
                expression_delete(p_conditional_expression);
                goto _catch_label_1;
            }
            p_conditional_expression->right = p_right;
            if (object_has_constant_value(&p_conditional_expression->condition_expr->object))
            {
                if (object_to_bool(&p_conditional_expression->condition_expr->object))
                {
                    p_conditional_expression->type = type_dup(&p_conditional_expression->left->type);
                    p_conditional_expression->object = p_conditional_expression->left->object;
                }
                else
                {
                    p_conditional_expression->type = type_dup(&p_conditional_expression->right->type);
                    p_conditional_expression->object = p_conditional_expression->right->object;
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
            if ( !type_is_scalar(&p_conditional_expression->condition_expr->type))
            {
                compiler_diagnostic_message(940, ctx, ctx->current, ((void *)0), "condition must have scalar type");
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
                        if ( !type_is_same(&left_type, &right_type, 1))
                        {
                            compiler_diagnostic_message(950, ctx, p_conditional_expression->condition_expr->first_token, ((void *)0), "incompatible types");
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
                                                if ( !type_is_same(&left_type, &right_type, 0))
                                                {
                                                    compiler_diagnostic_message(950, ctx, ctx->current, ((void *)0), "incompatible types");
                                                }
                                                else
                                                {
                                                    type_swap(&p_conditional_expression->type, &right_type);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            compiler_diagnostic_message(950, ctx, p_conditional_expression->condition_expr->first_token, ((void *)0), "incompatible types");
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
                                                    if ( !type_is_same(&left_type, &right_type, 0))
                                                    {
                                                        compiler_diagnostic_message(950, ctx, p_conditional_expression->condition_expr->first_token, ((void *)0), "incompatible types");
                                                    }
                                                    else
                                                    {
                                                        type_swap(&p_conditional_expression->type, &right_type);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                compiler_diagnostic_message(950, ctx, p_conditional_expression->condition_expr->first_token, ((void *)0), "incompatible types");
                                            }
                                        }
                                    }
                                    else
                                    {
                                        compiler_diagnostic_message(950, ctx, p_conditional_expression->condition_expr->first_token, ((void *)0), "incompatible types??");
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
    else _catch_label_1:
    {
        expression_delete(p_expression_node);
        p_expression_node = ((void *)0);
    }
    type_destroy(&left_type);
    type_destroy(&right_type);
    return p_expression_node;
}

struct expression *constant_expression(struct parser_ctx * ctx, unsigned char   show_error_if_not_constant)
{
    struct expression * p_expression = conditional_expression(ctx);
    if (show_error_if_not_constant && p_expression &&  !object_has_constant_value(&p_expression->object))
    {
        compiler_diagnostic_message(960, ctx, ctx->current, ((void *)0), "expected constant expression");
    }
    return p_expression;
}

unsigned char  expression_is_lvalue(struct expression * expr)
{
    switch (expr->expression_type)
    {
        case 2 :
        case 4 :
        case 3 :
        case 13 :
        case 15 :
        case 11 :
        case 32 :
        return 1;
        default:
        break;
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
    switch (expression->expression_type)
    {
        case 33 :
        case 26 :
        case 27 :
        case 16 :
        case 17 :
        return 0;
        default:
        if (expression->type.storage_class_specifier_flags & 2048)
        {
            return 1;
        }
    }
    return 1;
}

void check_comparison(struct parser_ctx * ctx, struct expression * p_a_expression, struct expression * p_b_expression, struct token * op_token)
{
    struct type * p_a_type = &p_a_expression->type;
    struct type * p_b_type = &p_b_expression->type;
    if (type_is_pointer(p_a_type) && type_is_integer(p_b_type))
    {
        if (expression_is_zero(p_b_expression))
        {
        }
        else
        {
            compiler_diagnostic_message(3, ctx, op_token, ((void *)0), "comparison between pointer and integer");
        }
    }
    check_diferent_enuns(ctx, op_token, p_a_expression, p_b_expression, "comparing different enums.");
}

unsigned char  type_is_bool(struct type * p_type);
unsigned char  type_is_any_owner(struct type * p_type);
unsigned char  type_is_obj_owner(struct type * p_type);
unsigned char  type_is_nullable(struct type * p_type, unsigned char   nullable_enabled);
unsigned char  type_is_enum(struct type * p_type);
void type_print(struct type * a);

void check_assigment(struct parser_ctx * ctx, struct type * p_a_type, struct expression * p_b_expression, int   assignment_type)
{
    struct type * p_b_type = &p_b_expression->type;
    unsigned char   is_null_pointer_constant = expression_is_null_pointer_constant(p_b_expression);
    if (type_is_pointer(p_a_type))
    {
        if ( !type_is_nullptr_t(p_b_type) &&  !type_is_pointer_or_array(p_b_type) &&  !type_is_function(p_b_type))
        {
            if (is_null_pointer_constant)
            {
                if (p_b_expression->expression_type == 8)
                {
                    compiler_diagnostic_message(10, ctx, p_b_expression->first_token, ((void *)0), "use NULL instead of 0");
                }
                else
                {
                    compiler_diagnostic_message(45, ctx, p_b_expression->first_token, ((void *)0), "unusual expression/type used as null pointer constant");
                }
            }
            else
            {
                compiler_diagnostic_message(1340, ctx, p_b_expression->first_token, ((void *)0), "non-pointer to pointer");
            }
        }
    }
    if (type_is_bool(p_a_type) && type_is_nullptr_t(p_b_type))
    {
        struct marker  marker = {0, 0, 0, 0, 0, 0, 0};
        marker.p_token_begin = p_b_expression->first_token;
        marker.p_token_end = p_b_expression->first_token;
        compiler_diagnostic_message(48, ctx, ((void *)0), &marker, "implicit conversion of nullptr constant to 'bool'");
    }
    struct type  lvalue_right_type = {0};
    struct type  t2 = {0};
    if (expression_is_subjected_to_lvalue_conversion(p_b_expression))
    {
        lvalue_right_type = type_lvalue_conversion(p_b_type, ctx->options.null_checks_enabled);
    }
    else
    {
        lvalue_right_type = type_dup(p_b_type);
    }
    if (type_is_owner(p_a_type) &&  !type_is_owner(&p_b_expression->type))
    {
        if ( !is_null_pointer_constant)
        {
            compiler_diagnostic_message(24, ctx, p_b_expression->first_token, ((void *)0), "cannot assign a non-owner to owner");
            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }
    }
    if ( !type_is_owner(p_a_type) && type_is_any_owner(&p_b_expression->type))
    {
        if (p_b_expression->type.storage_class_specifier_flags & 8192)
        {
            compiler_diagnostic_message(22, ctx, p_b_expression->first_token, ((void *)0), "cannot assign a temporary owner to non-owner object.");
            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }
    }
    if (assignment_type == 0)
    {
        if ( !type_is_owner(p_a_type) && type_is_any_owner(&p_b_expression->type))
        {
            if (p_b_expression->type.storage_class_specifier_flags & 4096)
            {
                compiler_diagnostic_message(1280, ctx, p_b_expression->first_token, ((void *)0), "cannot return a automatic storage duration _Owner to non-owner");
                type_destroy(&lvalue_right_type);
                type_destroy(&t2);
                return;
            }
        }
    }
    if (type_is_obj_owner(p_a_type) && type_is_pointer(p_a_type))
    {
        if (type_is_owner(p_b_type))
        {
        }
        else
        {
            if ( !p_b_type->address_of)
            {
                compiler_diagnostic_message(18, ctx, p_b_expression->first_token, ((void *)0), "source expression of _Obj_owner must be addressof");
            }
        }
    }
    if (type_is_pointer(p_a_type) &&  !type_is_nullable(p_a_type, ctx->options.null_checks_enabled) && is_null_pointer_constant)
    {
        compiler_diagnostic_message(34, ctx, p_b_expression->first_token, ((void *)0), "cannot convert a null pointer constant to non-nullable pointer");
        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }
    if (type_is_enum(p_b_type) && type_is_enum(p_a_type))
    {
        if ( !type_is_same(p_b_type, p_a_type, 0))
        {
            compiler_diagnostic_message(39, ctx, p_b_expression->first_token, ((void *)0), " incompatible types ");
        }
        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }
    if (type_is_arithmetic(p_b_type) && type_is_arithmetic(p_a_type))
    {
        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }
    if (is_null_pointer_constant && type_is_pointer(p_a_type))
    {
        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }
    if (is_null_pointer_constant && type_is_array(p_a_type))
    {
        compiler_diagnostic_message(27, ctx, p_b_expression->first_token, ((void *)0), " passing null as array");
        type_destroy(&lvalue_right_type);
        type_destroy(&t2);
        return;
    }
    if (type_is_pointer_or_array(p_b_type) && type_is_pointer_or_array(p_a_type))
    {
        if (type_is_void_ptr(p_b_type))
        {
            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }
        if (type_is_void_ptr(p_a_type))
        {
            type_destroy(&lvalue_right_type);
            type_destroy(&t2);
            return;
        }
        if (type_is_array(p_a_type))
        {
            int parameter_array_size = p_a_type->num_of_elements;
            if (type_is_array(p_b_type))
            {
                int argument_array_size = p_b_type->num_of_elements;
                if (parameter_array_size != 0 && argument_array_size < parameter_array_size)
                {
                    compiler_diagnostic_message(1130, ctx, p_b_expression->first_token, ((void *)0), " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                }
            }
            else
            {
                if (is_null_pointer_constant || type_is_nullptr_t(p_b_type))
                {
                    compiler_diagnostic_message(38, ctx, p_b_expression->first_token, ((void *)0), " passing null as array");
                }
            }
            t2 = type_lvalue_conversion(p_a_type, ctx->options.null_checks_enabled);
        }
        else
        {
            t2 = type_dup(p_a_type);
        }
        if ( !type_is_same(&lvalue_right_type, &t2, 0))
        {
            type_print(&lvalue_right_type);
            type_print(&t2);
            compiler_diagnostic_message(53, ctx, p_b_expression->first_token, ((void *)0), " incompatible types");
        }
        if (type_is_pointer(&lvalue_right_type) && type_is_pointer(&t2))
        {
            struct type  argument_pointer_to = type_remove_pointer(&lvalue_right_type);
            struct type  parameter_pointer_to = type_remove_pointer(&t2);
            if (type_is_const(&argument_pointer_to) &&  !type_is_const(&parameter_pointer_to))
            {
                compiler_diagnostic_message(14, ctx, p_b_expression->first_token, ((void *)0), " discarding const at argument ");
            }
            type_destroy(&argument_pointer_to);
            type_destroy(&parameter_pointer_to);
        }
    }
    if ( !type_is_same(p_a_type, &lvalue_right_type, 0))
    {
    }
    type_destroy(&lvalue_right_type);
    type_destroy(&t2);
}

